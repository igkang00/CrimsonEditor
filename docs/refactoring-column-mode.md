# Column (block) mode in the Unicode era

Column mode pretends text is a grid of cells. Under MBCS that pretence was nearly free.
Under Unicode it is a fiction the editor has stopped maintaining — and the fiction shows up
as a selection that highlights one thing and copies another.

> Status: **planned.** This document is written *before* the work, like
> [refactoring-line-container.md](refactoring-line-container.md) was, so that the decisions
> and the things that will break are on the record rather than discovered halfway through.

---

## The assumption that died

> One DBCS character is exactly two English character widths.

CP949 stored a Hangul syllable as two bytes, the code counted bytes, and a CJK glyph drew at
roughly twice the Latin advance. Byte count, cell count and pixel width all lined up, so
`nPosX / GetAveCharWidth()` was a perfectly good "which column am I in".

Unicode breaks that chain in several independent places:

| Cause | Effect |
| --- | --- |
| **Font dependence** | The 2:1 ratio holds only in a genuine dual-width monospace. **D2Coding is** — that is its whole point. **Consolas font-linked to Malgun Gothic is not**: the linked Hangul advance is not exactly twice the Consolas Latin advance. |
| **The standard punts** | Unicode's East Asian Width has an **`Ambiguous`** class (Greek, Cyrillic, box drawing) that is 1 cell in Western context and 2 in East Asian. Undecidable without a locale. |
| **Emoji** | The advance varies by font; colour emoji fonts carry their own metrics. |
| **Not cells at all** | Combining marks (zero width), variation selectors, ZWJ sequences (👨‍👩‍👧 — five code points, one grapheme), skin-tone modifiers. |

The deeper truth: **"column" is only well defined when text is a grid.** A terminal keeps the
grid honest by *forcing* every glyph into one or two cells. Crimson Editor draws with real
font metrics, so it has a sequence of glyph advances, not a grid. The grid is a fiction the
editor maintains — and Unicode is where it starts to leak.

---

## What the code actually does today

**There is no column coordinate anywhere in the program.** The editor has exactly two
coordinate spaces: **pixel** (`m_nCaretPosX`) and **code-unit index** (`nIdxX`). Column mode
is not a third one. It is a pair of tricks played on the first two:

1. Do not clamp pixel X at the end of the line — every edit call site threads
   `bAdjust = ! m_bColumnMode` into `GetIdxXFromPosX` / `GetPosXFromIdxX`
   ([cedtViewMap.cpp:180](../src/view/cedtViewMap.cpp#L180),
   [:235](../src/view/cedtViewMap.cpp#L235)).
2. Quantize the unclamped pixel to multiples of `GetAveCharWidth()`.

So **every "column N" in this program is really "pixel N × tmAveCharWidth"**, and the
"virtual space" past the end of a line is a grid of uniform *narrow* cells.

That yields a model that contradicts itself:

| | inside the line | past the end of the line |
| --- | --- | --- |
| how a pixel becomes an index | measured per-character advances (`GetWordIndex`, surrogate-aware, correct for CJK and emoji) | `(nPosX - nLastPosX) / GetAveCharWidth()` — one narrow cell per code unit |

The two agree only for pure-ASCII text in a fixed-pitch font. Everywhere else they disagree,
and the disagreement is the bug.

### What it looks like to a user

**The selection highlights one thing and copies another.** `InvertScreenSelected`
([cedtViewDraw.cpp:263](../src/view/cedtViewDraw.cpp#L263)) paints the *same raw pixel
rectangle* on every row — `left = nBegX`, `right = nEndX`. The block operations then map
that pixel into each line separately and **snap it to the nearest character boundary**
([cedtViewEditAdv.cpp:386](../src/view/cedtViewEditAdv.cpp#L386)). On a line with Hangul the
snapped boundary is not where the rectangle was drawn. You see one selection and you get a
different one — and the rectangle can be drawn through the middle of a glyph.

**Block paste does not align.** `InsertColumnSelection`
([cedtViewEditAdv.cpp:407](../src/view/cedtViewEditAdv.cpp#L407)):

```cpp
rBlock.MakeEqualLength();                                   // pads to equal CODE-UNIT count
INT nMaxLen = rBlock.GetMaxLength();

nEndX = nBegX + nMaxLen * nAveCharWidth;                    // block width = chars × narrow cell
...
InsertString( nIdxX, nIdxY, CString(' ', (nBegX - nLstX) / nAveCharWidth) );   // pad = pixels ÷ narrow cell
```

`MakeEqualLength` ([cedtElement.cpp:765](../src/core/cedtElement.cpp#L765)) pads to equal
**character count**, not equal display width, so a block whose lines contain Hangul is
ragged before it is even pasted. `nMaxLen * nAveCharWidth` then computes a width that is too
small by one cell per wide character. **This is wrong even in a perfect dual-width font** —
it is a counting error, not a metrics error.

**Backspace moves the block by half a character.** `ActionDeleteColumnPrevChar`
([cedtViewEditAdv.cpp:47](../src/view/cedtViewEditAdv.cpp#L47)) shifts the block left by
exactly one `nAveCharWidth`. In front of a Hangul syllable that lands mid-glyph.

**Data is safe.** Every block range derives its indices from `GetIdxXFromPosX`, which the
surrogate work made boundary-snapped, so a block operation **cannot cut a character in
half**. What is broken is alignment and honesty, not integrity.

---

## Decision: the grid model, with cells derived from measurement

Block editing fundamentally needs a *cell* concept — "insert this at column N on every line"
has no other meaning. So: every character gets a cell width, a block selection is a range of
**columns**, and it means the same thing on every line.

### Where the cell width comes from — and why not from a table

An earlier version of this document proposed reintroducing `_CharColumnWidth`: a hand-written
table of East Asian Width ranges plus the Wide emoji ranges. **That is the wrong answer, and
we already know it is, because that function existed and was deleted.** Its table listed the
CJK ranges but no emoji, so U+2705 (✅) and friends were classified as one narrow cell while
actually rendering two cells wide. Layout and rendering disagreed and the caret landed inside
the glyph. Enumerating wide characters is a losing game: Unicode adds more every version.

Two facts make a better answer available now:

- **Column mode always runs in a fixed-pitch font.** If the current screen font is
  proportional, entering column mode silently substitutes the dedicated Column Mode font
  ([cedtViewFont.cpp:20](../src/view/cedtViewFont.cpp#L20)). So a narrow cell has a definite
  width.
- **A per-character advance cache already exists** — `_charWidth[0x10000]`, keyed on the
  HFONT in the DC ([cedtViewFormat.cpp:112](../src/view/cedtViewFormat.cpp#L112)), built for
  the 3.91 width-cache work.

So derive the cell count from what will actually be drawn:

```
cells(ch) = round( advance(ch) / narrowWidth )
```

This **cannot disagree with rendering**, which is exactly the failure that killed the table.
It needs no maintenance as Unicode grows. Zero-width combining marks fall out as 0 cells
without a special case. An emoji is whatever the font really draws it as.

> **`narrowWidth` must be the width of a SPACE, not `tmAveCharWidth`.** Alignment padding is
> done by inserting spaces, so if the cell is not exactly a space wide, padding can never
> land on a column. In a fixed-pitch font these are the same number — but the space advance
> is the one the model actually depends on, so it is the one to use.

### Two coordinate systems, coexisting

This must not be conflated. The editor will have **two different notions of "column"**:

| | **Character position** (existing, unchanged) | **Display column** (new) |
| --- | --- | --- |
| Latin `a` | 1 | 1 cell |
| Hangul `한` | **1** | **2 cells** |
| Emoji `😀` | **1** | **2 cells** |
| Used by | caret / arrows / Backspace / delete unit | **column-mode geometry only** |

**No user-visible behaviour changes outside column mode.** Arrow keys still cross `한` in one
press; Backspace still deletes it in one press. We are not "making Hangul two columns" — we
are adding a column-mode-only alignment coordinate. Vim does exactly this, and shows both
numbers in its ruler.

### The boundary rule: expand both edges

When a column edge falls inside a wide character, **the character is included** — the left
edge snaps to its start, the right edge snaps to its end.

```
col:   1 2 3 4 5 6 7 8
       a b 한  한  c          한 occupies 3-4 and 5-6
block:     [------]           columns 3..6 requested

expand:    [한  한 ]          both straddling characters included
```

Because:

- **See == get.** The highlight is painted at the expanded boundary, so what is blue is what
  is copied. Fixing the see/get mismatch is the whole point of this work.
- **No character is ever split**, at either edge.
- **It is symmetric, so it round-trips.** Cut a block and paste it back and it lands where it
  started. The rejected alternative — shrink the left edge, expand the right — walks the
  block one cell to the right every time, which for the one operation column mode exists to
  do is fatal.

The cost: on a line where a wide character straddles an edge, the selection looks one cell
wider than the requested range. That is not a lie — that is exactly what you are about to
take.

---

## Plan

**Phase 0 — the cell classifier, and a test that does not need a screen.**
Split it in two so the interesting half is testable:

- `CellsFromAdvance(nAdvance, nNarrow)` — pure arithmetic, unit-tested directly: 0 for a
  zero-advance mark, 1 for narrow, 2 for exactly-double, 2 for the 1.9× a font-linked face
  actually reports, and pinned behaviour for the awkward ratios in between.
- `CCedtView::GetCharCells(LPCTSTR, INT nIdxX)` — the GDI half: one lookup in a
  `_charCells[0x10000]` cache sitting beside `_charWidth[]` and invalidated with it, plus a
  measured branch for surrogate pairs (which `_charWidth` deliberately does not cache).

Also: `GetNarrowWidth()` = the space advance in the current font, and a check that column
mode's font really is fixed-pitch.

**Phase 1 — the column coordinate, with nothing wired to it yet.**

- `GetColumnFromIdxX(rLine, nIdxX)` — sum the cells before `nIdxX`.
- `GetIdxXFromColumn(rLine, nColumn, edge)` — walk cells to `nColumn`; if it lands inside a
  character, return that character's **start** for a left edge and its **end** for a right
  edge (the expand rule, in one place).
- `GetLastColumn(rLine)` — the line's width in cells.

These are the entire model. Everything after this is replacing arithmetic with calls to them.
They must be correct before anything depends on them, so they get the same treatment
`CLineList` got: a table-driven test over lines mixing ASCII, Hangul, astral emoji, combining
marks and tabs, checking the round trip `column → index → column` and both edge rules.

**Tabs are part of this, not an afterthought.** A tab's cell width depends on where it
starts — it advances to the next tab stop. The grid absorbs this cleanly *provided* tab stops
are whole numbers of narrow cells, which they are (`_nTabWidth = m_nTabSize * _nSpaceWidth`).
The helpers must walk tabs by tab stop, not by a fixed width.

**Phase 2 — the caret and virtual space move onto the grid.**
In column mode `m_nCaretPosX` stays the storage (it feeds drawing, scrolling and a hundred
call sites; making it a column is a much larger blast radius) but it becomes **grid-aligned
by construction**: always `column × narrowWidth`. The four quantizing sites
([cedtViewCaret.cpp:127](../src/view/cedtViewCaret.cpp#L127),
[:151](../src/view/cedtViewCaret.cpp#L151), [:187](../src/view/cedtViewCaret.cpp#L187),
[:208](../src/view/cedtViewCaret.cpp#L208)) and the two movement sites
([cedtViewMove.cpp:197](../src/view/cedtViewMove.cpp#L197),
[:218](../src/view/cedtViewMove.cpp#L218)) stop dividing by `tmAveCharWidth` and start
stepping columns.

The consequence to accept deliberately: **in column mode a click inside a line snaps the
caret to a grid column, not to a glyph edge.** In a dual-width font those are the same place.
In a font where they are not, the caret follows the grid — which is what a grid model *is*,
and what makes the selection a true rectangle.

**Phase 3 — the selection is painted where it will actually cut.**
`InvertScreenSelected` stops painting the raw pixel rectangle and paints, per row, the pixels
of the **expanded snapped indices**. This is what closes the see/get gap, and it is the phase
a user would notice first.

**Phase 4 — the block operations count cells, not characters.**
`CopyToColumnSelection`, `InsertColumnSelection`, `DeleteColumnSelection`,
`ActionDeleteColumnPrevChar`, `ActionInsertColumnChar`,
`ActionInsertColumnSpacesInPlaceOfTab`. The two counting bugs die here: `MakeEqualLength` and
`GetMaxLength` must pad and measure in **cells**, and the pad count must come from a column
difference rather than `pixels ÷ tmAveCharWidth`.

`CMemText` is a general container, so rather than teach it about display width, the column
paste path should pad the block itself before handing it over — the block is a column-mode
artefact and its width rule belongs to column mode.

**Phase 5 — the status bar tells the truth.**
In column mode the `Col` field shows the **display column**; outside it, the character
position, exactly as now ([cedtView.cpp:590](../src/view/cedtView.cpp#L590)). A user aligning
a block needs the number that matches the grid they are aligning to.

**Phase 6 — measure, and write down what is still not exact.**

---

## What this does not fix

- **`Ambiguous` width, ZWJ sequences, combining marks.** No grid model can be exact here, and
  neither can a terminal. Deriving cells from the font's own advances means the editor at
  least agrees with itself.
- **Fonts that are not truly dual-width.** The grid stays consistent as *text* — column N is
  the same character on every line — but the glyphs will not sit exactly on it. Recommend
  D2Coding for CJK column work, in the README and in Preferences. Consider making it the
  default Column Mode font, since we already bundle it.
- **The commands column mode simply refuses**: Select All, indent / unindent, comment /
  uncomment, cut-append / copy-append, search-in-selection
  ([cedtViewEvent.cpp](../src/view/cedtViewEvent.cpp), and
  [cedtViewHndrEdit.cpp:782](../src/view/cedtViewHndrEdit.cpp#L782)). Each is its own design
  question. Out of scope; listed here so the next person does not think they were missed.
- **Undo does not know about column mode at all.** A column edit is recorded as N independent
  per-line operations, and undo restores the caret by index — so a caret parked in virtual
  space is not restored. Pre-existing, orthogonal, and worth its own pass.

---

## What to know before touching this

**Two coordinates, and they are not interchangeable.** A character index is not a column and
a column is not a pixel. The moment one is used as the other, Hangul breaks and nobody
notices for a month.

**Do not reintroduce a width table.** It has been tried, it shipped a caret that landed
inside emoji, and it was deleted for that. The font knows how wide the glyph is; ask it.

**The cell is a space, not `tmAveCharWidth`.** Padding is spaces. If those two numbers ever
diverge, alignment silently stops working.

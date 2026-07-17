# Column (block) mode in the Unicode era

Column mode pretends text is a grid of cells. Under MBCS the pretence was nearly free. Under
Unicode the editor stopped maintaining it, and the result was a selection that highlighted one
thing and copied another.

The editor now **imposes** the grid rather than hoping the font provides one: a character
occupies a whole number of cells and is drawn at that cell whatever the font's own advance
would be. Hangul lines up in Consolas, which font-links it at 1.43× the Latin cell and can
therefore never align on its own.

> Status: **done.** This document was written *before* the work, as a plan, like
> [refactoring-line-container.md](refactoring-line-container.md). The phases below record what
> actually happened — including the two places the plan turned out to be wrong, which are
> marked where they occur rather than quietly edited out.

---

## The assumption that died

> One DBCS character is exactly two English character widths.

CP949 stored a Hangul syllable as two bytes, the code counted bytes, and a CJK glyph drew at
roughly twice the Latin advance. Byte count, cell count and pixel width all lined up, so
`nPosX / GetAveCharWidth()` was a perfectly good "which column am I in".

That is not true any more, and it is worth being precise about *how* untrue, because the
answer decides the design. Measured with GDI at 10 pt / 96 dpi, taking the advance of `a` as
one narrow cell — re-runnable with
[`analysis/font-cell-width.ps1`](../analysis/font-cell-width.ps1):

| font | narrow cell | Hangul `가` | Han `漢` | 😀 U+1F600 |
| --- | ---: | ---: | ---: | ---: |
| **Consolas** (today's Column Mode font) | 7 px | **10 px — 1.43×** | 14 px — 2.0× | 12 px — 1.71× |
| **D2Coding** (bundled) | 7 px | **14 px — 2.0×** | 14 px — 2.0× | 12 px — 1.71× |
| Courier New | 8 px | **9 px — 1.13×** | 16 px — 2.0× | 16 px — 2.0× |

**A fixed-pitch font is not a dual-width font.** Consolas has no Hangul glyphs; Windows
font-links them to another face whose advance has no reason to be exactly twice the Consolas
Latin advance — and is not. Han happens to land on 2.0× in all three fonts; Hangul does not,
because it font-links somewhere else. Only D2Coding, which carries its own Hangul, is a true
2:1 font.

So: **the editor's default Column Mode font cannot form a grid for Korean text at all.**
([cedtAppConf.cpp:392](../src/app/cedtAppConf.cpp#L392) — and the comment above it, claiming
font-linked Hangul "stays aligned even without D2Coding", is measurably false.)

The deeper truth: **"column" is only well defined when text is a grid**, and a font does not
give you one. A terminal has a grid because it *forces* every glyph into one or two cells.
Crimson Editor draws with real font metrics, so it has a sequence of glyph advances — and no
assignment of cells to characters can turn Consolas's 10 px Hangul and 14 px double-Latin
into the same column boundary.

---

## What the code actually does today

**There is no column coordinate anywhere in the program.** The editor has exactly two
coordinate spaces: **pixel** (`m_nCaretPosX`) and **code-unit index** (`nIdxX`). Column mode
is not a third one. It is two tricks played on the first two:

1. Do not clamp pixel X at the end of the line — every edit call site threads
   `bAdjust = ! m_bColumnMode` into `GetIdxXFromPosX` / `GetPosXFromIdxX`
   ([cedtViewMap.cpp:180](../src/view/cedtViewMap.cpp#L180),
   [:235](../src/view/cedtViewMap.cpp#L235)).
2. Quantize the unclamped pixel to multiples of `GetAveCharWidth()`.

So **every "column N" in this program is really "pixel N × tmAveCharWidth"**, and the virtual
space past the end of a line is a grid of uniform *narrow* cells — while the inside of a line
uses real measured advances. The two agree only for pure-ASCII fixed-pitch text.

### What that costs a user

**The selection highlights one thing and copies another.** `InvertScreenSelected`
([cedtViewDraw.cpp:263](../src/view/cedtViewDraw.cpp#L263)) paints the *same raw pixel
rectangle* on every row. The block operations then map that pixel into each line separately
and snap it to the nearest character boundary
([cedtViewEditAdv.cpp:386](../src/view/cedtViewEditAdv.cpp#L386)). On a line with Hangul the
snapped boundary is not where the rectangle was drawn. The rectangle can also be drawn
straight through the middle of a glyph.

**Block paste does not align.** `InsertColumnSelection`
([cedtViewEditAdv.cpp:407](../src/view/cedtViewEditAdv.cpp#L407)):

```cpp
rBlock.MakeEqualLength();                                  // pads to equal CODE-UNIT count
nEndX = nBegX + rBlock.GetMaxLength() * nAveCharWidth;     // width = chars × narrow cell
...
InsertString( nIdxX, nIdxY, CString(' ', (nBegX - nLstX) / nAveCharWidth) );  // pad = pixels ÷ narrow cell
```

`MakeEqualLength` ([cedtElement.cpp:765](../src/core/cedtElement.cpp#L765)) pads to equal
**character count**, not display width. This is a counting error, not a metrics error: it is
wrong even in a perfect dual-width font.

**Data is safe.** Every block range derives its indices from `GetIdxXFromPosX`, which the
surrogate work made boundary-snapped, so a block operation **cannot cut a character in half**.
What is broken is alignment and honesty, not integrity.

---

## Decision: build the grid, do not hope for one

The editor stops asking the font where characters go, and **places them itself**. In column
mode a character occupies a whole number of cells, and it is drawn starting at that cell's
pixel — whatever the font would have done on its own.

This is what a terminal does, and it is the thing that makes the problem tractable. The
impossibility above — no cell assignment can reconcile 10 px Hangul with 14 px double-Latin —
only holds while *rendering* follows the font's advances. Take that away and the grid is
exact by construction, in any fixed-pitch font.

Consequences, all of them good:

- **The font requirement disappears.** Consolas's Hangul is 10 px and its cell is 14 px, so it
  sits in its cell with 4 px of air. It is not as pretty as D2Coding, but **the columns line
  up exactly**. D2Coding becomes a recommendation, not a prerequisite.
- **Column-mode layout gets faster.** A character's pixel position becomes
  `cells-before × narrowWidth` — arithmetic. No `GetTextExtent` at all.
- **Layout and rendering cannot disagree**, because they are the same number.

**Fixed pitch is still required.** In a proportional font, `i` and `m` are not both one cell,
and forcing them into one would overlap. The existing font substitution
([cedtViewFont.cpp:20](../src/view/cedtViewFont.cpp#L20)) stays exactly as it is.

**Drawing has to change.** `TextOut` lays a run out with the font's own advances, so inside a
word `한글abc` the `a` would land off-grid. Column mode must draw with **`ExtTextOut` and an
explicit `lpDx` advance array** built from the cell widths. Same number of calls; this is what
that parameter exists for.

### Where the cell width comes from

Under forced placement, a misclassification is **asymmetric**:

| we say | it really is | result |
| --- | --- | --- |
| 2 cells | 1 cell wide | a little air around the glyph — **harmless** |
| 1 cell | 2 cells wide | the glyph **overlaps the next cell** — bad |

So the classifier must lean wide. Use the **union of two sources**:

- **A table** — East Asian Width `W` and `F` (CJK ideographs, Hangul, Kana, fullwidth forms)
  plus the emoji ranges. This is font-independent: the same document has the same columns
  everywhere, which is the point of a grid.
- **A measurement** — `advance(ch) > 1.2 × narrowWidth` ⇒ 2 cells, using the per-character
  advance cache that already exists ([cedtViewFormat.cpp:112](../src/view/cedtViewFormat.cpp#L112)).

They cover each other's holes. The table catches Hangul that a bad fallback draws too narrow
(Courier New's 1.13×). The measurement catches emoji added to Unicode after the table was
written — which is exactly how `_CharColumnWidth` failed last time.

**Why the threshold is 1.2 and not 1.5.** Sort the measured ratios and the reason is on the
page:

```
1.00   a, space
1.13   Courier New Hangul      genuinely wide, drawn narrow by a poor fallback
1.14   Consolas ★ ⭐
1.25   Courier New ★ ⭐
────────────────────────────── 1.2
1.43   Consolas Hangul         genuinely wide
1.71   emoji
────────────────────────────── 1.5
2.00   Han, D2Coding Hangul
```

**Consolas Hangul sits at 1.43 — under a 1.5 threshold.** A safety net that misses the single
most important character in the exercise, in the font that is the current default, is not a
safety net; it just leaves the table carrying the whole weight alone. At 1.2 both sources
catch it, which is what redundancy is supposed to mean.

The only classification 1.2 changes is Courier New's ★ (1.25), which becomes 2 cells. ★ is
East Asian Width `Ambiguous` — the standard explicitly declines to say — so calling it wide is
not an error, it is the East Asian convention, and the font is already drawing it 1.25 cells
wide anyway. Given the asymmetry above, a lower threshold is structurally the safe direction.

> **That failure is worth naming, because this brings the table back.** The old
> `_CharColumnWidth` was deleted for classifying U+2705 (✅) as one narrow cell while the font
> drew it two cells wide: *layout* used the table, *rendering* used the font, they disagreed,
> and the caret landed inside the glyph. Under forced placement rendering obeys the table, so
> the disagreement cannot exist. The table is only dangerous when something else is allowed to
> overrule it.

**The cell must be exactly a space wide**, i.e. `GetSpaceWidth()`, not `tmAveCharWidth`.
Alignment padding is done by inserting spaces; if a cell is not a space, padding can never
land on a column. In a fixed-pitch font the two numbers agree — but the model depends on the
space, so use the space.

### Two coordinate systems, coexisting

| | **Character position** (existing, unchanged) | **Display column** (new) |
| --- | --- | --- |
| Latin `a` | 1 | 1 cell |
| Hangul `한` | **1** | **2 cells** |
| Emoji `😀` | **1** | **2 cells** |
| Used by | caret / arrows / Backspace / delete unit | **column-mode geometry only** |

**Nothing changes outside column mode.** Arrow keys still cross `한` in one press; Backspace
still deletes it in one press; the character count in the status bar still counts it once. We
are not "making Hangul two columns" — we are adding a column-mode-only alignment coordinate,
exactly as vim does.

### The boundary rule: a character belongs to the block holding its first cell

When a column edge falls inside a wide character, the question is which side it goes to. One
rule answers it:

> **A character belongs to the column range that contains its FIRST cell.**

Equivalently, and this is how it reads in code: **both edges snap to the next character
boundary at or after the column.** One function, no direction parameter.

Read from the two edges it looks asymmetric — a character straddling the left edge is
excluded (its first cell is before the block), one straddling the right edge is included (its
first cell is inside) — but that is one rule seen twice, not two rules.

```
line:   a 한  b            한 occupies columns 2-3
block A: [1,3)   block B: [3,5)

this rule:  A → [a]      B → [한 b]      exact partition — no gap, no overlap
expand:     A → [a 한]   B → [한 b]      한 in BOTH
shrink:     A → [a]      B → [b]         한 in NEITHER
```

**That partition property is why.** Adjacent column ranges tile the text exactly: every
character belongs to exactly one of them. Under *expand* a straddling character is duplicated
into both; under *shrink* it falls through the crack and belongs to neither. In an editor
where column operations get applied one after another, that is the difference that matters.

- **See == get.** The highlight is painted at the snapped boundary, so what is blue is what
  gets copied — including the case where a line's selection is empty.
- **No character is split**, at either edge.
- **It round-trips.** Cut a block, paste it back at the same column, it lands where it started.

The cost: a one-cell block over the *right* half of a wide character selects nothing on that
line, because that character's first cell is outside the block. The highlight is empty there
too, so the reason is visible.

*(An earlier draft of this document chose "expand both edges" and justified it by claiming the
single-rule alternative breaks the cut/paste round trip. That was simply wrong — checked
against worked examples, it round-trips fine — and the partition property, which the earlier
draft missed entirely, points the other way.)*

### Column Backspace

The block's column decreases by **one**, and each row deletes **one whole character**.

Rows whose deleted character was two cells wide shrink by two; rows whose character was one
cell shrink by one. The lines end up different lengths — and that is correct, because the
things deleted really were different widths. The block is a column, not a property of the
text; it does not owe the text alignment after the text has changed.

---

## Plan

**Phase 0 — the cell classifier, split so the interesting half is testable. DONE.**
The pure half lives in `src/include/cedtCharWidth.h` and is unit-tested in
`tests/CharWidth_test.cpp`: `IsWideByTable(cp)` (the EAW table), `CellsFor(cp, advance, narrow)`
(the union, the `advance*10 > narrow*12` threshold, and the zero-cell combining-mark case, all
taking plain values), and `CodepointAt` (the surrogate-pair decode, so the table can be keyed
on astral emoji). The GDI half is `CCedtView::GetCharCells`, a shell that measures one
character's advance through the existing width cache (`_MeasureRun`) and calls `CellsFor`.

- The narrow cell is the space advance — the existing `GetSpaceWidth()`, reused rather than a
  new `GetNarrowWidth()`. It is what alignment padding depends on, and it already exists.
- `GetCharCells` asserts the font is fixed pitch. Column mode already forces this; the assert
  records that the grid model now depends on it.
- Writing the tests surfaced the real shape of the table: it carries whole astral emoji blocks
  and the CJK/Hangul/fullwidth ranges, but deliberately does **not** enumerate the emoji
  scattered through the BMP (U+2705, U+2B50 …). That scattering is exactly what made the old
  `_CharColumnWidth` miss U+2705; here the measurement half catches them, so the table does not
  have to pretend to.

**Phase 1 — the column coordinate, wired to nothing yet. DONE.**
The pure walk lives in `cedtCharWidth.h` — `ColumnFromIdxX`, `IdxXFromColumn`, `LastColumn` —
and takes a cell-count callback so it is tested without a device context. The thin
`CCedtView` wrappers (`GetColumnFromIdxX` / `GetIdxXFromColumn` / `GetLastColumn`, in
`cedtViewMap.cpp`) pass a static callback that reaches `GetCharCells` through the `this` it
carries as context. The line length comes from the string, not `GetLastIdxX`, so the walk does
not depend on the row being laid out.

- `GetIdxXFromColumn(rLine, nColumn)` is the first character whose start column is `>= nColumn`
  — **no edge parameter**. The boundary rule is one function, so a column inside a wide
  character resolves to that character's far side, and adjacent ranges tile the text.

**Tabs are handled in the walk, not by the callback.** A tab advances to the next tab stop, so
its width depends on the column it starts at — which only the walk knows. `_TabCells(col,
tabSize)` returns whole tab stops, keeping the grid integer.

`tests/CharWidth_test.cpp` gets the `CLineList` treatment: a fixed cell rule (ASCII 1, Hangul
and astral emoji 2, combining mark 0) drives lines mixing all of those plus tabs, checking the
round trip `column → index → column`, the boundary rule on a column that lands mid-character,
and the partition property — adjacent ranges map to index ranges that neither gap nor overlap.

**Phase 2 — column-mode layout is placed on the grid. DONE (with Phase 3).**
The measuring is done in `_GetWordWidth` / `_GetWordIndex`, so those are where the grid goes
in: a file-global `_bGridLayout` (distinct from `m_bColumnMode` — it is the LAYOUT choice, off
for the printer, which measures for real) switches a word's width from its real advance to
`cells × narrow`. `_WordCellWidth` sums the cells. Because `GetPosXFromIdxX` / `GetIdxXFromPosX`
compute a position inside a word as `word start + GetWordWidth(prefix)`, making `GetWordWidth`
cell-based carries the whole coordinate system with it — the caret follows the grid with no
further change. The `/ GetAveCharWidth()` extrapolation past the end of a line was already
correct: in a fixed-pitch font `tmAveCharWidth == GetSpaceWidth`, which is why ASCII column
mode worked before any of this.

**Phase 3 — drawing obeys the grid. DONE (with Phase 2).**
`DrawColumnWord` draws with `ExtTextOut` and a per-code-unit `lpDx` (a character's whole width
on its first unit, 0 on the low half of a surrogate pair), so a glyph the font would advance
off-grid lands in its cell. Both draw sites — the text and the embolden pass — branch on
`m_bColumnMode`.

**Flushed out here: the toggle did not reformat.** `CCedtApp::OnEditColumnMode` only reformatted
when the *font* changed. That was correct while column mode left the layout alone, and this work
made it wrong: a fixed-pitch user's font never switches, so entering or leaving column mode
skipped the reformat and rows kept the geometry of the mode they were laid out in — grid-placed
text drawn with `TextOut` after leaving column mode. The toggle now always reformats. (The plan
above claimed the reformat was already forced; it was not, and this is where that showed.)

**Phase 4 — the selection is painted where it will actually cut. DONE.**
`InvertScreenSelected` snaps each edge to a character boundary *in that row* rather than
painting the raw pixel rectangle, so the highlight stops cutting through a Hangul glyph and
starts showing what a copy would take. This is the phase a user notices first: see == get.

**The virtual space stays.** The snap passes `bAdjust = FALSE` — a column block is a
*rectangle*, not a piece of text. Where a row is too short to reach the block, the block is
still there over empty cells, and a paste fills them with spaces
([cedtViewEditAdv.cpp:433](../src/view/cedtViewEditAdv.cpp#L433)); clamping to the line end
would draw a ragged block and hide the region about to be pasted into. The first cut of this
phase clamped, on a reading of "see == get" that only counted the text a *copy* returns. That
was wrong: the highlight also has to answer "where will a paste land", and that is the whole
rectangle. `bAdjust` already meant exactly this — column mode passes `! m_bColumnMode` to
these functions everywhere else precisely to keep the virtual space.

*(A note on the boundary rule: the block operations snap with `GetIdxXFromPosX`, which resolves
a column inside a wide character to that character's NEAR side — the opposite of
`GetIdxXFromColumn`'s rule from Phase 1. The highlight matches the operations here, so see ==
get holds today. Unifying both on the documented rule is Phase 5's job, and they have to move
together or the highlight starts lying again.)*

**Phase 5 — the block operations count cells, not characters. DONE.**
The counting bug was in `InsertColumnSelection`: `CMemText::MakeEqualLength` pads to an equal
number of *characters* and `GetMaxLength` counts *characters*, so a block whose lines mixed
scripts came out ragged and `nEndX` (chars × cell) fell short of where the text really ended.
The paste path now measures each line with `GetStringColumns` (the Phase 1 walk, so tabs are
handled) and pads to the widest in cells. `CMemText` is a general container and stays ignorant
of display width — the block is a column-mode artefact and its width rule belongs here.

Everything else was already right, for a reason worth writing down: the padding counts
(`(nBegX - nLstX) / nAveCharWidth`) and the Backspace step were pixel arithmetic over a
fixed-pitch font, where `tmAveCharWidth == GetSpaceWidth`. They are now spelled `GetSpaceWidth`
because *that* is the number the model depends on — padding is spaces — but the behaviour did
not change.

**The boundary rule is now the documented one.** Two adapters carry it, and nothing else may
snap a block edge:

- `GetIdxXFromBlockEdge(rLine, nPosX)` — where the edge *cuts*. Pixel → column →
  `GetIdxXFromColumn`. Every block operation asks this.
- `GetPosXFromBlockEdge(rLine, nPosX)` — where the edge is *drawn*. Inside the line, the
  snapped character's pixel; past the end, the pixel unchanged, because an index cannot
  express virtual space and the grid there is already exact. The highlight asks this.

`GetIdxXFromPosX` deliberately stays as it was, resolving to the NEAR side. It answers a
different question — "which character is at this pixel" — and the caret and the mouse's
nearest-edge test ([cedtViewCaret.cpp:162](../src/view/cedtViewCaret.cpp#L162)) depend on
that: with far-side snapping, `nPosX - nPos1` goes negative and every click lands on the
character to the right. The two functions look like duplicates and are not; they disagree only
when an edge falls inside a wide character, which is the one case a block needs a rule for.

**Phase 6 — the status bar tells the truth. DONE.**
In column mode the field shows the display column; outside it, the character position, exactly
as before. The column is taken from the caret pixel over the cell rather than from
`GetColumnFromIdxX`: the caret is grid-aligned in column mode anyway, and that also works in
the virtual space past the end of a line, where there is no character index to convert.

**And the label moves with it.** The pane reads `Ln 3, Ch 5` normally and `Ln 3, Co 7` in
column mode. This is where the two coordinate systems first become visible to a user — the same
caret, two different numbers — so the label has to say which one is being shown. Leaving it at
`Ch` while the number had become a column would have been a small lie in the one place built to
explain the difference.

**Phase 7 — measure, and write down what is still not exact. DONE.**
The measurements are unchanged from the ones at the top of this document — the classifier
reads the same advances; what changed is that the editor no longer *believes* them for
placement. Re-run `analysis/font-cell-width.ps1` to check.

**The Column Mode font stays Consolas.** The idea was to default it to the bundled D2Coding,
"the only font where the glyphs actually fill their cells". Considered, and rejected:

- That slot is only reached by a user whose screen font is **proportional** — column mode
  substitutes it then and leaves a fixed-pitch screen font alone. So it is a *fallback*.
- D2Coding is an **optional install** ([installer.iss:158](../installer.iss#L158)) and may not
  be present. The same reasoning already put Consolas in screen slot 0
  ([cedtAppConf.cpp:341](../src/app/cedtAppConf.cpp#L341)): the default has to render on every
  machine.
- And the point of this work is that **D2Coding is no longer needed**. Forcing it as the
  fallback would contradict the thing the grid was built to prove. A user who wants it selects
  screen font slot 1, and column mode then leaves it alone.

The comment that shipped over that font default claimed font-linked Hangul "stays aligned
even without D2Coding" because the math "routes CJK through GDI GetTextExtent". It is now
true and the reason was always false — honouring GDI is precisely what cannot align a 1.43×
Hangul. It has been corrected to say what actually holds it up.


**Phase 8 — typing Hangul into a multi-row block. DONE, after the fact.**
Not in the original plan; found by using the thing. A letter went to every row of a block and
a Hangul syllable went to one, because the two took different paths: `EventInsertChar` reached
`ActionInsertColumnChar` and grew the multi-row loop, while `EventCompositionCompose` threw
the selection away and never had one.

Composition is a one-line affair by construction — the document holds exactly one saved line
to restore on each keystroke, and the preview overwrites the anchor to select the syllable
being assembled, which is where the block lived. So the block is *parked* when the composition
opens and put back when it commits, which is the only moment the finished text exists to give
to every row. The preview stays on the caret's row; there is nothing to show the others yet.

The two paths are now one loop — `ActionInsertColumnString`, with the character form a wrapper
over it. Them being separate is what caused this. (A Hangul syllable is one `TCHAR` now that
the editor is Unicode, so the character form would nearly serve; but IME delivers a *string*,
and a Hanja conversion or a multi-syllable commit is more than one character.)

**The trap, for the next person:** on a commit, `WM_IME_COMPOSITION` calls
`OnImeCompositionEnd` **before** `OnImeCompositionResult`
([cedtView.cpp:1432](../src/view/cedtView.cpp#L1432)). Clearing composition state in
`EventCompositionEnd` therefore destroys it one step before the code that needs it — exactly
the bug I wrote while trying to handle an abandoned composition. Nothing needs clearing there:
the next composition's first keystroke recomputes the state from the selection that exists then.

**Phase 9 — commenting a column block. DONE, after the fact.**
Also not in the plan; one of the commands listed below as refused, picked off because the
answer turned out to be clear.

A column block gets wrapped in **block** delimiters — `/* ... */` — on each row, not given a
line comment. A line comment kills the whole line, which is to say it ignores the columns the
user picked; commenting out part of a line is exactly what block delimiters are for. A
language without them (Python) cannot express this, so it beeps rather than substituting
something else. The delimiter test therefore moved inside the per-mode branch: the two modes
want different delimiters, and the old code tested `HasLineCommentDelimiter` around both.

Three details, all of them found by using it rather than by reading it:

- **A space inside each delimiter** (`/* x */`), because `FastMakeCommentLine` already writes
  `"// "` and the two commands should not disagree. Uncomment accepts either form, the same
  tolerance `FastReleaseCommentLine` shows.
- **Rows that stop short of the block's right edge are padded out to it**, so the closing
  delimiter stands on the edge and the comment is the same rectangle the block is. Without it
  the delimiters land wherever each row happens to end.
- **Uncomment only narrows the block if something actually came out of it.** It used to narrow
  unconditionally, so a block over uncommented rows walked its right edge left on every press.

A row that does not match is skipped silently — no beep. A block often spans rows where only
some are commented, and refusing the whole operation over one of them would be worse than
doing the part that makes sense.

---

## What this does not fix

- **`Ambiguous` width, ZWJ sequences, combining marks.** No grid model can be exact here, and
  neither can a terminal. The editor will at least agree with itself.
- **Glyphs wider than their cells still overlap slightly.** U+2B50 (⭐) measures 8 px against a
  7 px cell. The union classifier keeps this rare and small; a terminal clips, and we will not.
- **The commands column mode still refuses**: Select All, Select Block, indent / unindent,
  cut-append / copy-append ([cedtViewEvent.cpp](../src/view/cedtViewEvent.cpp)). Each is its
  own design question, and the answers are not obvious — cut-append has to say what it means
  to concatenate two rectangles, and indent has to say whether it moves the block or the lines
  under it. Listed so the next person knows they were not missed.

  **Search in selection is not one of these.** It looks like a silent substitution
  ([cedtViewHndrEdit.cpp:782](../src/view/cedtViewHndrEdit.cpp#L782) forces range 1 = whole
  file) and it is not: passing anything but 0 makes `CReplaceDialog::OnInitDialog` grey the
  "in selection" radio out, so the user is told. It is excluded on purpose — the search walks
  a *continuous* range and `IsStringInSelection` is written for one, while a column block is a
  rectangle over many rows. A stream selection of nothing is excluded the same way.
- **Undo does not know about column mode.** A column edit is recorded as N independent per-line
  operations and undo restores the caret by index, so a caret parked in virtual space is not
  restored. Pre-existing, orthogonal, worth its own pass.

---

## What to know before touching this

**Two coordinates, and they are not interchangeable.** A character index is not a column and a
column is not a pixel. The moment one is used as the other, Hangul breaks and nobody notices
for a month.

**The grid is ours, not the font's.** If anything in column mode ever measures a glyph and
believes the answer over the cell count, the caret starts landing inside characters again —
that is precisely how the last attempt at this died.

**The cell is a space, not `tmAveCharWidth`.** Padding is spaces. If those two numbers ever
diverge, alignment silently stops working.

**A block edge is not a caret.** `GetIdxXFromBlockEdge` / `GetPosXFromBlockEdge` are for block
edges; `GetIdxXFromPosX` is for carets and clicks. They resolve a column inside a wide
character to opposite sides, deliberately, and they look enough alike to invite "simplifying"
one into the other. Do not: pointing the caret at the far side makes the mouse's nearest-edge
test compare a negative distance and every click lands one character right. Pointing a block
edge at the near side silently reverses the boundary rule and nothing fails visibly.

**The highlight and the block operations snap through the same call, and must keep doing so.**
The moment they use different rules, the editor highlights one thing and cuts another — which
is the bug this whole document exists to remove.

**A keystroke and an IME commit go through the same insert.** They were separate once, and the
multi-row loop got added to one of them, so letters filled a block and Hangul filled one row.
If they ever diverge again, so will they.

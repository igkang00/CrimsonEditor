# Column (Block) Mode in the Unicode Era

Design notes for column/block mode after the Unicode migration. This is a **design** problem, not a bug fix — unlike `docs/refactoring-surrogate-pairs.md`, there is no single correct answer here, and the trade-offs need to be chosen deliberately.

---

## The assumption that died

Column mode works by pretending text is a **grid of cells**. Under MBCS that pretence was cheap and nearly true:

> One DBCS character is exactly two English character widths.

CP949 stored a Hangul syllable as two bytes, the code counted bytes, and a CJK glyph drew at roughly twice the Latin advance. Byte count, cell count and pixel width all lined up, so `nPosX / GetAveCharWidth()` was a perfectly good "which column am I in".

Under Unicode that chain breaks in several independent places:

| Cause | Effect |
| --- | --- |
| **Font dependence** | The 2:1 ratio only holds if the font is a genuine dual-width monospace. **D2Coding is** (that is its whole point). **Consolas + Windows font-linking to Malgun Gothic is not** — the linked Hangul advance is not exactly twice the Consolas Latin advance. |
| **The standard itself punts** | Unicode's East Asian Width property has an **`Ambiguous`** class (Greek, Cyrillic, box-drawing, some punctuation) that is 1 cell in Western context and 2 in East Asian context. Undecidable without locale. |
| **Emoji** | Advance varies by font; colour emoji fonts carry their own metrics. |
| **Things that are not cells at all** | Combining marks (zero width), variation selectors, **ZWJ sequences** (👨‍👩‍👧 — several code points, one grapheme), skin-tone modifiers. |

The deeper truth: **"column" is only well defined when text is a grid.** A terminal keeps the grid honest by *forcing* every glyph into one or two cells. Crimson Editor renders with real font metrics (`TextOut` / `GetTextExtent`), so it has a sequence of glyph advances, not a grid. The grid is a fiction the editor maintains — and Unicode is where the fiction starts to show.

## Current state of the code

Column mode divides and multiplies pixel X by `GetAveCharWidth()`, i.e. it assumes **1 index == 1 uniform narrow cell**:

- `src/view/cedtViewCaret.cpp:127-128, 151-154, 184-185, 205-208`
- `src/view/cedtViewMove.cpp:200, 218`
- `src/view/cedtViewEditAdv.cpp:415, 435`
- `src/view/cedtViewEdit.cpp:91, 121`

That assumption is **already wrong for every wide CJK character**, before emoji enter the picture. Surrogate pairs join this problem; they do not cause it.

**Data corruption is not part of this problem.** Every block copy/delete range in `src/view/cedtViewEditAdv.cpp` derives its indices from `GetIdxXFromPosX`, which the surrogate work makes boundary-snapped. After that, a block operation on a line containing emoji may produce a *visually ragged* selection, but it **cannot** cut a character in half. What remains here is alignment quality, not integrity.

---

## The two candidate models

### Grid model — select by logical cell

Give every character a cell width: Latin `1`, CJK/emoji `2`. A block selection is a **range of cell numbers**, applied to every line.

- ✅ **Text-consistent** — "cells 2–3" means the same thing on every line.
- ✅ **Block insert / align is well defined** — pad with spaces to cell N, insert. This is what block editing is *for*.
- ❌ **Looks ragged unless the font really renders wide chars at exactly 2×.** If `한` draws at 17px while two Latin chars are 20px, cell 2 sits at a different pixel on different lines.

This is the terminal / vim model.

### Pixel-rectangle model — select by screen X

A block selection is a **pixel X range**, snapped per line to the nearest character boundary.

- ✅ **Always a true rectangle on screen**, in any font.
- ❌ **No stable "column" concept** — different lines yield different character counts and positions.
- ❌ **Block insert becomes approximate.** To insert at pixel 50 you can only pad with **spaces**, which come in whole cells. If a line ends at pixel 37 after a Hangul syllable, padding gives 47 then 57 — you can never land on 50.

### The key observation

**When the font renders wide characters at exactly 2× the narrow width, the two models are identical.** Cell N is always at pixel N × charwidth, so grid == pixel, the rectangle is true, and text consistency holds. Everything works.

**D2Coding is exactly such a font**, and it is now bundled with the installer (see `docs/refactoring-unicode-migration.md`, phase 6c).

---

## Decision: adopt the grid model

Block editing fundamentally needs a *cell* concept — that is what "insert this at column N on every line" means. The grid model's only weakness (visual alignment) disappears with a dual-width font, and we ship one.

### Two coordinate systems, coexisting

This is the part that must not be conflated. The editor will have **two different notions of "column"**, used for different things:

| | **Character position** (existing, unchanged) | **Display column** (new, block mode only) |
| --- | --- | --- |
| Latin `a` | 1 | 1 cell |
| Hangul `한`, Han `漢` | **1** | **2 cells** |
| Emoji `😀` | **1** (once surrogate work lands) | **2 cells** |
| Used by | caret / arrow keys, status-bar `Ch`, delete unit | **block-mode alignment only** |

**No user-visible behaviour changes for Korean.** Arrow keys still cross `한` in one press, Backspace still deletes it in one press, the status bar still counts it as one character. We are **not** "making Hangul two columns" — we are **adding a block-mode-only alignment coordinate**.

Vim does precisely this, and shows both numbers in its ruler (`col` and virtual `col`).

---

## Implementation sketch

**First, a width classifier has to exist.** There used to be a `_CharColumnWidth` in `src/view/cedtViewFormat.cpp` that returned 1 or 2 from a hand-written table of East Asian Width ranges. The surrogate work **deleted** it, for a reason worth recording here:

> That table listed the CJK ranges but no emoji, so BMP emoji — U+2705 (✅), U+2B50 (⭐) and friends — were classified as one narrow cell while actually rendering about two cells wide. Layout and rendering disagreed and the caret landed inside the glyph. Enumerating wide characters is a losing game: Unicode adds more every version. For *measurement*, the fix was to stop classifying and just ask GDI (`_NeedsGdiMeasure` → `GetTextExtent`), which reports what will really be drawn.

A grid model, however, genuinely does need a 1-or-2 answer per character, and GDI cannot give it (GDI returns pixels, not cells). So this work must reintroduce a classifier — and it must be built properly:

- East Asian Width **W** and **F** (CJK ideographs, Hangul, Kana, fullwidth forms), **and**
- the BMP emoji ranges that are also Wide (U+231A–231B, U+2614–2615, U+2648–2653, U+26AA–26AB, U+2705, U+270A–270B, U+2728, U+274C, U+2753–2755, U+2757, U+2795–2797, U+27B0, U+27BF, U+2B1B–2B1C, U+2B50, U+2B55, …), **and**
- astral characters (a surrogate pair is one character, and emoji/CJK-Ext-B render wide).

Then add two helpers:

- `IdxXToColumn(line, nIdxX)` — character index → display column (accumulate cell widths)
- `ColumnToIdxX(line, nColumn)` — display column → character index (snap to a character boundary; decide a rule for a column that lands inside a wide character — expand or shrink)

Then re-base column mode's caret, selection, copy, insert and delete on this coordinate system, replacing the `/ GetAveCharWidth()` arithmetic at the sites listed above.

## Documented limitations

- The grid is only **visually** a rectangle when the font renders wide characters at exactly 2× the narrow width. **Recommend D2Coding for CJK column work** — state this in the README and in Preferences help.
- East Asian Width `Ambiguous`, ZWJ emoji sequences and combining marks cannot be made exact by *any* grid model. Accept and document.

## Prerequisite

`docs/refactoring-surrogate-pairs.md` phases 0–1 must land first: `_CharColumnWidth` needs its astral branch, and block ranges need to be boundary-snapped, before the grid coordinate system is meaningful.

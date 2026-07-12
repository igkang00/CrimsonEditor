# UTF-16 Surrogate Pairs (Astral Characters / Emoji)

Working notes for teaching the editor that a surrogate pair is **one character**, not two.

This is a direct follow-up to the Unicode migration (`docs/refactoring-unicode-migration.md`). That branch made the editor UTF-16 internally; this one fixes the one place where "1 code unit = 1 character" is not true.

---

## The problem

Reported during install testing: Korean, Chinese and Japanese characters are each recognised as **1 column**, but an emoji is recognised as **2 columns**. That looks like an inconsistency between scripts, but it isn't — the real dividing line is **BMP vs supplementary plane**.

The editor indexes text by **UTF-16 code units** (`TCHAR` = `wchar_t`). Under that model:

| Character | Plane | UTF-16 | Code units | Columns |
| --- | --- | --- | --- | --- |
| `한` U+D55C | BMP | 1 unit | 1 | 1 |
| `漢` U+6F22 | BMP | 1 unit | 1 | 1 |
| `あ` U+3042 | BMP | 1 unit | 1 | 1 |
| `😀` U+1F600 | **Supplementary** | **surrogate pair** `D83D DE00` | **2** | **2** |

Common CJK is all BMP, so it happens to be 1 unit. Emoji — and rare CJK such as Extension B (`𠀋` U+2000B) — live above U+FFFF and are stored as a **surrogate pair**: a high surrogate (`0xD800`–`0xDBFF`) followed by a low surrogate (`0xDC00`–`0xDFFF`). This is the same reason JavaScript reports `"한".length === 1` but `"😀".length === 2`.

**There is currently zero surrogate handling in the codebase** — a grep for `0xD800` / `0xDC00` / `surrogate` returns nothing.

### This is not a cosmetic bug — it corrupts files

Because nothing knows about pairs, every per-character operation steps by one code unit:

- **Left/Right arrow** takes two presses to cross an emoji, and the caret can rest *between* the two halves.
- **Backspace / Delete** removes **half a pair**, leaving an unpaired (lone) surrogate in the buffer.
- A lone surrogate is **not a valid character in any encoding**. On save, `WideCharToMultiByte(CP_UTF8, ...)` cannot encode it and substitutes `U+FFFD`. **The data is permanently destroyed.**
- **Word wrap** can split a pair across two display rows.
- **Overwrite (OVR) mode** replaces exactly one code unit, orphaning the other half.

### What is *not* broken

File I/O is already correct. UTF-8 has no surrogates at all — a supplementary code point is simply a 4-byte sequence (`😀` = `F0 9F 98 80`). `MultiByteToWideChar(CP_UTF8, ...)` turns those 4 bytes into a surrogate pair on load, and `WideCharToMultiByte` turns the pair back into 4 bytes on save. The round-trip works today. **The bug lives entirely in the in-memory editing paths.**

---

## Facts established by code exploration

Three assumptions we started with turned out to be wrong. They are recorded here because they shaped the design.

1. **The analyzer does NOT split an emoji in the normal path.** Tracing `U+1F600` through `_AnalyzeLine` (`src/doc/cedtDocAnal.cpp`), it reaches state `0x0700` (CHECK IDENTIFIER, line 375) whose `while( * fwd && ! _istspace(* fwd) && ! _tcschr(DEL, * fwd) )` loop consumes **both halves** into a single `WT_IDENTIFIER` word. The `WT_GRAPH` else-branch (line 402) is only reachable for a control char listed as a delimiter. So the `ANALYZEDWORD` / `FORMATEDWORD` layer already carries pairs intact — the damage is all downstream.
   **But there is one real analyzer split:** the escape-character branch (line 132) does `fwd += 2` unconditionally, so in a C/JS file `"\😀"` emits a `WT_CONSTANT` word of `\` + **high surrogate only**, and the low surrogate becomes a separate word.

2. **The caret is stored as a PIXEL X position** (`m_nCaretPosX`), not a character index. Every operation converts pixel→index via `GetIdxXFromPosX` and index→pixel via `GetPosXFromIdxX` (`src/view/cedtViewMap.cpp`). `m_nCaretIdxX` is only a save/restore snapshot across reformat. This means a single surrogate-unaware conversion function can drop the caret into the middle of a pair, and every editing action inherits it.

3. **The highest-leverage chokepoint is `_GetWordIndex`** (`src/view/cedtViewFormat.cpp:103`), not `GetIdxXFromPosX`. That one function computes **both** (a) the pixel→index mapping (via `GetIdxXFromPosX` → `GetWordIndex`) and (b) the **word-wrap split point** (`_FormatLineWrap:199`, `_FormatLineWrapContinue:252`). Fixing it once fixes caret placement, mouse hit-testing, selection, and wrap splitting together.

Two more findings that simplified the work considerably:

4. **The undo format needs no change.** `FastDeleteString` / `AT_DELETESTRING` already store the full deleted `CString` and replay it, for arbitrary lengths. So a 2-unit delete becomes **one atomic record** — one analyze pass, no transient lone-surrogate document state, and a single Ctrl+Z restores the whole emoji. This is strictly better than calling the 1-unit delete twice.

5. **Column/block mode gets corruption-safety for free.** Every block copy/delete range in `src/view/cedtViewEditAdv.cpp` derives its indices from `GetIdxXFromPosX`. Once that is boundary-snapped (Phase 1), a block operation can never cut a pair in half. Column mode's *visual alignment* is a separate concern — see `docs/refactoring-column-mode.md`.

---

## Plan

### Phase 0 — Shared primitives

New header `src/include/cedtUnicode.h` (the project's global include dir), pulled in from `cedtHeader.h`. Header-only inlines, no-ops under `#ifndef _UNICODE` (same guard style as `_CharColumnWidth`):

| Helper | Meaning |
| --- | --- |
| `IsHighSurrogate(TCHAR)` | `0xD800`–`0xDBFF` |
| `IsLowSurrogate(TCHAR)` | `0xDC00`–`0xDFFF` |
| `IsCharBoundary(p, nIdx)` | index does not point at a low surrogate |
| `CharUnitsAt(p, nIdx, nLen)` | 2 for a valid pair, else 1 |
| `SnapIdxX(p, nIdx)` | pull an index off a low surrogate, back onto the high one |
| `NextIdxX(p, nIdx, nLen)` / `PrevIdxX(p, nIdx)` | step forward/back by one **character** |

### Phase 1 — The two chokepoints (`src/view/cedtViewFormat.cpp`)

1. **`_NeedsGdiMeasure`** (replaces `_HasWideChar` + `_CharColumnWidth`) — decide whether a word can use the fixed-pitch arithmetic (`_nSpaceWidth * siLength`) or must be measured by GDI.

   The old code answered this from a hand-written table of East Asian Width ranges. That is the wrong shape of solution, and testing found the hole: **BMP emoji**. U+2705 (✅) and U+2B50 (⭐) are ordinary BMP code points — *not* surrogate pairs — sit in none of the CJK ranges, and yet render about two cells wide. They took the fast path, were billed one cell, and were drawn two: the caret landed inside the glyph and everything after them on the line was shifted. Unicode adds more such characters every version, so a table can only ever be behind.

   So the test is inverted: **only plain ASCII is guaranteed to be one narrow cell.** Everything else — CJK, surrogate pairs, BMP emoji, anything font-linking substitutes from a fallback face — goes to `GetTextExtent`, which by definition reports the width that will actually be drawn. Pure-ASCII source still takes the fast path.

2. **`_GetWordIndex`** — advance the scan by `CharUnitsAt` instead of `i++`, so the returned index is always a character boundary; `SnapIdxX` the result as a belt-and-braces guard.

Also snap defensively at the top of `CCedtView::GetPosXFromIdxX` (`src/view/cedtViewMap.cpp:212`).

**What this one set of edits buys:** every pixel↔index conversion in the app (mouse click, drag, Shift-selection, vertical caret movement, every `ActionXxx`), the word-wrap split point, and every column-mode copy/delete range.

### Phase 2 — Caret stepping (the `±1` sites)

`GetIdxXFromPosX` now returns a boundary, but a `±1` after it still lands mid-pair.

- `src/view/cedtViewMove.cpp:205` / `:223` — `MoveCaretLeft` / `MoveCaretRight` → `PrevIdxX` / `NextIdxX`
- `src/view/cedtViewCaret.cpp:160` / `:214` — mouse click and drag nearest-boundary snap (`nIdxX + 1`) → `NextIdxX`
- `src/view/cedtViewMetric.cpp:145` — `GetCharWidth` hard-codes length `1`; use `CharUnitsAt` so the OVR block caret covers the whole emoji

**The word walkers in `src/view/cedtViewMapAdv.cpp` need no change.** `CCedtDoc::GetCharType` returns `CH_CHARACTER` for *both* surrogate halves, so the segment scanners can only stop at a type *change*, which is never mid-pair. Lock this in with a test rather than editing the code.

### Phase 3 — Delete (atomic, one undo record)

Add `CCedtDoc::DeleteCharacter(nIdxX, nIdxY)` in `src/doc/cedtDocEdit.cpp`: dispatch on `CharUnitsAt` — 1 unit uses the existing `FastDeleteChar`, 2 units use `FastDeleteString(..., 2)` (one `AT_DELETESTRING` record).

Leave the existing 1-unit `DeleteChar` alone — undo replay of `AT_INSERTCHAR` depends on it.

Repoint the callers:
- `ActionBackspace` (`src/view/cedtViewEdit.cpp:251`) — step back with `PrevIdxX`, then `DeleteCharacter`
- `ActionDeleteChar` (:273)
- **Overwrite mode** (:15, :64) and the string-overwrite paths (:148, plus IME overwrite in `src/view/cedtViewEditCompose.cpp:52`, `:96`) — extend the deleted range to a character boundary so no half-pair survives

### Phase 4 — Insert (WM_CHAR arrives twice)

Windows delivers an astral character as **two `WM_CHAR` messages** (high surrogate, then low). Today each half gets its own undo record, its own `AnalyzeText`/`FormatScreenText` pass, and creates a transient lone-surrogate document.

- Add `m_chPendingHighSurrogate` to `CCedtView` (`src/view/cedtView.h`).
- In `PreTranslateMessage`, for **both** `WM_CHAR` (`src/view/cedtView.cpp:1317`) and `WM_IME_CHAR` (:1333): buffer a high surrogate and consume the message; when the low surrogate arrives, commit **both as a string**. Drop a lone low surrogate; clear the pending high on any non-surrogate, on `WM_KEYDOWN`, and on `OnKillFocus`.
- New `OnStringKeyDown` handler (`src/view/cedtViewHndrEdit.cpp`) mirroring `OnCharKeyDown` but routing to `EventInsertString` → `ActionInsertString`: **one** `InsertString`, **one** `AT_INSERTSTRING` record, **one** analyze pass. Macro recording uses `MacroRecordString` (recording a half would replay broken).

Result: one Ctrl+Z removes the whole emoji, and the document never contains a lone surrogate.

### Phase 5 — Analyzer

- `src/doc/cedtDocAnal.cpp:132` — the escape branch's `fwd += 2` must advance by `CharUnitsAt` past the escaped character. **This is a real bug** (`"\😀"`).
- `:402` — the `WT_GRAPH` else-branch `fwd++`: unreachable for emoji today, but advance by `CharUnitsAt` anyway so a future language spec listing a surrogate-range delimiter can't regress it.

### Phase 6 — Ingest hardening

A UTF-8 file containing CESU-8 or otherwise malformed input can put a lone surrogate into the buffer at load time. Scrub unpaired surrogates to `U+FFFD` on load (`src/core/cedtElement.cpp`, UTF-8 path). This establishes the invariant **"the document never contains a lone surrogate"**, which is what makes every helper above sound.

---

## Verification

### Unit tests

New `tests/cedtUnicode_test.cpp` (+ a `<ClCompile>` entry in `tests/cedt_tests.vcxproj`):

- `SnapIdxX` / `NextIdxX` / `PrevIdxX` / `CharUnitsAt` over `"a😀b"`, `"😀"`, `"😀😀"`, a lone high, a lone low, empty string, index-at-end.
- `CCedtDoc::GetCharType` returns the same value for both halves of `U+1F600` — this locks in the assumption that lets the word walkers stay untouched.
- **UTF-8 round-trip regression:** save a line containing `U+1F600`, read the bytes back, expect exactly `F0 9F 98 80` and **no `EF BF BD`** (U+FFFD). This is the regression test for the actual corruption.

The existing 60 tests must stay green.

### Manual end-to-end

Fixture `tests/data/astral.txt`, UTF-8 without BOM:

```
ascii 😀 tail
한글 😀 𠀋 mix
"\😀"
<a long line of emoji, to force word wrap>
```

1. **Left / Right** cross the emoji in one press; the caret never renders between the halves.
2. **Up / Down** through the emoji line and back — the caret must not drift into a pair.
3. **Backspace** after the emoji, and **Delete** before it — the whole emoji disappears in one press.
4. **OVR mode** — the block caret covers the whole emoji; typing over it replaces the whole emoji.
5. **Type an emoji** (Win+`.` panel) and paste one — **one Ctrl+Z removes it entirely**; Ctrl+Y restores it.
6. **Word wrap** — shrink the window one pixel at a time across the emoji; the wrap never lands between the halves.
7. **Ctrl+Left / Ctrl+Right and double-click** — selection edges land on emoji boundaries.
8. **Column mode (Alt+drag)** copy and delete — the result contains whole emoji or none; no `?` or `U+FFFD` appears.
9. **Save as UTF-8, close, reopen** — the glyph is identical. Hexdump the file: `F0 9F 98 80` present, **no `EF BF BD`**. This is the definitive corruption check.

---

## Scope

**In scope:** Phases 0–6 — the complete data-integrity and caret-correctness story.

**Out of scope:** column/block mode **visual alignment**. Its cell math assumes `1 index == 1 average char cell`, which is **already wrong for every wide CJK character** — a pre-existing issue that surrogates merely join rather than cause. Phase 1 makes block operations corruption-safe; making the selection rectangle look right is a separate design problem tracked in `docs/refactoring-column-mode.md`.

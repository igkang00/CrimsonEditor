# Large-file loading performance

Opening a 100 MB / 900,000-line text file took **28 seconds**. Saving it took **13 seconds**.
It now takes **2.2 seconds** to open and **0.14 seconds** to save.

This document records what was actually slow, why, and what changed. It is written for whoever
touches this code next, because several of the fixes look like they could be reverted safely and
cannot.

---

## Measuring first

The three stages of opening a document are:

1. `CAnalyzedText::FileLoad` — read the bytes off disk, decode them, split into lines
2. `CCedtDoc::AnalyzeText` — tokenize every line (keywords, strings, comments, numbers)
3. `CCedtView::FormatScreenText` — measure every word and lay out the screen rows

A temporary `perflog.h` was added to time each stage on the real application (the timings below
come from `build/x64/Release-KR/cedt_kr.exe C:\Temp\big_100mb.txt`; the instrumentation was
removed once the work landed).

| Stage | Before | After | Factor |
| --- | ---: | ---: | ---: |
| 1. `FileLoad` | 3,673 ms | 752 ms | 4.9× |
| 2. `AnalyzeText` | 6,149 ms | 557 ms | 11× |
| 3. `FormatScreenText` | 17,417 ms | 49 ms | **355×** |
| **Open (wall clock)** | **28.4 s** | **2.2 s** | **13×** |
| `FileSave` | 13,021 ms | 136 ms | 96× |

The guess going in was that the syntax analyzer's state machine was the bottleneck. It was not.
The measurements are the only reason the right thing got fixed.

---

## 1. The screen formatter laid out the whole document

`FormatScreenText()` walked all 900,000 lines, and for each one it asked GDI
(`CDC::GetTextExtent`) for the pixel width of every word and heap-allocated a
`FORMATEDWORD` array to hold the result. About fifty of those lines are on screen. The other
899,950 layouts were computed and thrown away.

**Now**: with word wrap off, one screen row corresponds to exactly one logical line, so the row
*count* — the only thing the vertical scroll bar needs — is known without laying anything out.
`FormatScreenText()` fills the list with unformatted placeholder rows and returns.
`EnsureFormattedRange(row, count)` lays out a row the first time something actually looks at it.

Two things had to be true for this to work, and finding them was most of the effort:

**Nobody outside the visible rows needs the layout.** Vertical scrolling uses the row count.
The horizontal scroll bar is *hardcoded* to 4096 and never measures anything
([cedtView.cpp](../src/view/cedtView.cpp)). The draw loops already iterate only visible rows.
Every other consumer — caret, editing, navigation, search, highlight — funnels through
`GetLineFromPosY` ([cedtViewMap.cpp](../src/view/cedtViewMap.cpp)), so a single
`EnsureFormattedAt` there covers all of them.

**The syntax carry-over state must survive lazy layout.** Whether line 5,000 is inside a block
comment depends on lines 1..4,999. That state is a single `USHORT` flag plus a here-doc
terminator string, and computing it needs the *tokens*, which `AnalyzeText` already produced —
it needs no pixel widths and no allocation at all. So `SeedScreenTextFlags()` walks the analyzed
list and the row list in lockstep and seeds the flag onto every row, with zero GDI calls. That
pass is what costs the 49 ms.

### Three traps

**`_FinishLine` used to zero the carry flag** (`rFmtLine.m_usLineFlag = 0x0000;`). That line was
dead code — every caller overwrote the flag immediately afterwards — but under lazy layout it
would have wiped the seeded state the moment the user scrolled to the line, and the line would
have forgotten it was inside a block comment. **Deleting it is load-bearing.** There is a comment
in the source saying so.

**The fast-path gate read `m_usLineInfo` off the row, not the line.** An unformatted row has
`m_usLineInfo == 0`, which made the gate skip lines containing `/*` entirely — block comments
would simply not have been detected. The carry-state computation was moved into
`_CheckLineFlagAnalyzed`, which reads the *analyzed* line. This is a correctness fix, not an
optimization.

**The post-edit fixup loop walked formatted rows.** Type `/*` on line 100 and the loop propagates
"we are inside a comment" downwards until it finds the `*/` that closes it. Unformatted rows have
`m_siWordCount == 0`, so the loop would never *see* the closing `*/` — it would walk to the end of
the file, colouring everything as comment, on every keystroke. Rewritten to walk the analyzed list.

**`CList::FindIndex` is O(n) from the head.** Calling `EnsureFormatted` per row would be
50 rows × 800,000 hops per repaint — worse than the 17 seconds being fixed. `EnsureFormattedRange`
does `FindIndex` once per list and then advances both with `GetNext`.

---

## 2. File I/O read and wrote one line at a time

`FileLoad` read a 512-byte buffer, took the text up to the **first** line break in it, then
*seeked backwards* so the next read would start at the next line. A 900,000-line file therefore
did ~900,000 reads and ~900,000 seeks, re-reading most bytes several times.

`FileSave` was the mirror image: one `CFile::Write` per line, so ~900,000 write syscalls.

**Now**: `FileLoad` reads 64 KiB blocks and consumes *every* line in each block, carrying a partial
line across the boundary. `FileSave` accumulates into a 64 KiB `CWriteBuffer` and flushes when full.
`CMemText` (used for clipboard and the output pane) got the same treatment.

`CWriteBuffer` deliberately has **no flush in its destructor**: it is used inside a `try` block that
can unwind on a disk error, and throwing from a destructor during unwind terminates the process.
Callers call `Flush()` explicitly.

While rewriting `CMemText::FileSave` a latent bug surfaced: it passed a *character* count to
`CFile::Write`, which wants a *byte* count. Under MBCS those were the same number. Under Unicode
it wrote half the string.

---

## 3. The analyzer repainted the progress bar 45,000 times

`AnalyzeText` calls `SetProgress` every 20 lines. On 900,000 lines that is 45,000 calls, and each
one builds an offscreen DC and bitmap, draws the border and the text, blits it, and tears it all
down — for a bar that has only 101 distinct appearances. **This, not the state machine, was the
bulk of the 6.1 seconds.**

An audit of every `SetProgress` caller turned up a worse one: the dictionary loader fires its
callback every 100 words of a 1 MB dictionary, and the counter it tests only advances on a *new*
word — so once it reaches a multiple of 100 it stays there, and every subsequent duplicate word
triggers another repaint.

**Now**: the guard lives inside `CStatusBarEx::SetProgress` — if the percentage has not changed,
there is nothing to draw, so return. That covers every caller, including ones added later, and it
belongs there because the expense is in the drawing, not in the asking.

The state machine did also get faster: character classification (`_istalpha`, `_istdigit`,
`_tcschr(DELIMITER, ...)`, …) now comes from a 64 KiB lookup table built once per language spec
instead of a CRT call or a string scan per character. That is worth about 12 % — real, but an order
of magnitude less than the progress bar.

> `CCedtDoc::GetCharType` deliberately still calls `_tcschr`. The lookup table is a global belonging
> to whichever document `AnalyzeText` last ran on, and `GetCharType` is called against whichever
> document currently has focus — possibly one with a different language spec.

---

## What is still slow

`GetLineFromPosY` and `CCedtDoc::GetLineFromIdxY` still reach a line with `CList::FindIndex`, which
walks from the head. Sequential access — the overwhelmingly common pattern — would be O(1) with a
one-entry cursor cache (remember the last index and position, invalidate on structural change).
This is orthogonal to lazy layout and was left out on purpose.

**Word wrap is still eager.** With wrap on, the number of screen rows depends on the pixel width of
every word in every line, so the vertical scroll bar cannot be sized without laying the whole
document out. Making it lazy needs an estimate-then-correct scheme plus a cumulative row-count
index to keep the row↔line mapping cheap — a larger change than everything above, for a mode that
is off by default.

---

## Verification

- 88 unit tests green, including 10 new ones in `tests/CAnalyzedTextFile_test.cpp` that use
  2,400-byte lines specifically to straddle the 64 KiB block boundary in both directions.
- `tests/data/blockcomment.c` holds a block comment open across 5,000 lines: scrolling to the middle
  must still render as comment (proves the seeded carry state survives lazy layout), and deleting
  the closing `*/` must flip the colour of everything below it (proves the rewritten fixup loop
  re-converges).
- The 10 MB and 100 MB fixtures are re-opened with word wrap **on** as well, to confirm the eager
  path is untouched.

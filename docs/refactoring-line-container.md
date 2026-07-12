# Replacing the line container

The document's lines used to live in an MFC `CList` — a doubly-linked list. Reaching line
*n* meant walking *n* nodes from the head. They now live in an array.

With the caret on the last line of a 900,000-line file, the lookup every keystroke goes
through cost **7–10 ms**. It now costs **0.2 µs**, the same as it does on line 1.

> Status: **done.** This document was written *before* the work, as a plan, so that the two
> things that would break were on the record rather than discovered halfway through. Both
> did break, exactly where it said they would. The phase list at the bottom records what
> actually happened.

---

## The problem

```cpp
class CAnalyzedText : public CList<CAnalyzedString, LPCTSTR>          // text + tokens
class CFormatedText  : public CList<CFormatedString, CFormatedString &>  // screen layout
```

`CList::FindIndex(n)` has no way to jump. It walks. Measured on a 900,000-line document:

| line index | `FindIndex` |
| ---: | ---: |
| 1,000 | 2.4 µs |
| 100,000 | 367 µs |
| 450,000 | 3,220 µs |
| 899,999 | **6,984 µs** |

Note it is **worse than linear** — 9× the depth costs 19× the time. List nodes are
scattered across the heap, so a deep walk misses every level of cache. The big-O
notation flatters it.

`CCedtView::GetLineFromPosY` ([cedtViewMap.cpp](../src/view/cedtViewMap.cpp)) is the
gateway every caret move, edit, search, highlight and repaint passes through, and it
calls `FindIndex` every time. `cedtDocSearch.cpp` alone has 13 more. With the caret near
the end of a large file, each of those is ~7 ms.

Until now this was hidden behind a 28-second file open. Now that opening takes 2 seconds,
it is the next thing you feel.

## The surprise: the linked list loses at its own game

The reason to accept O(n) lookup is supposed to be O(1) insertion. It does not work out
that way, because **to insert you must first find the place**:

| operation (900,000 lines) | `CList` | `std::vector` of pointers |
| --- | ---: | ---: |
| reach the last line | 6,879 µs | ~0 µs |
| **insert one line in the middle** | **3,378 µs** | **187 µs** |
| insert one line at the very front | 0.3 µs | 265 µs |

`InsertBefore(pos, …)` really is O(1) — but the `FindIndex` that produces `pos` is not,
and it dominates. A vector is **18× faster at the operation the list exists for**.

The vector loses only on inserting at the very front, and that costs 265 µs — a quarter
of a millisecond, once, for an operation nobody does in a loop.

---

## Design

### Vector of pointers, not vector of values

```cpp
std::vector<CAnalyzedString *> m_lines;   // the objects stay put; only pointers move
```

This is forced, not preferred. Code holds **references into the container across
mutations of it**:

```cpp
// CCedtDoc::InsertBlock — cedtDocEdit.cpp
CAnalyzedString & rLineBeg = GetLineFromIdxY(nBegY);   // reference into the container
...
for(INT i = 1; i < rBlock.GetCount(); i++)
    pos = m_clsAnalyzedText.InsertAfter(pos, ...);     // mutates the container
...
rLineEnd += szSplit;                                    // still using references
```

A `vector<CAnalyzedString>` would move its elements on insert and leave `rLineBeg`
dangling — a use-after-free that would usually *appear to work*. With a vector of
pointers the line objects never move; only the 8-byte pointers do. Every existing
reference stays valid, exactly as it does today.

It also keeps insertion cheap: moving a line means moving 8 bytes, not a whole
`CAnalyzedString` (which owns a heap `m_pWord` array). That is what the 187 µs above
measures.

### POSITION stays `POSITION`

The call sites use a small, fixed slice of the `CList` API — a survey of every call on
the two lists:

| method | calls |
| --- | ---: |
| `GetNext` | 45 |
| `FindIndex` | 32 |
| `GetAt` | 24 |
| `GetCount` | 20 |
| `GetHeadPosition` | 15 |
| `GetPrev` | 6 |
| `InsertAfter` | 5 |
| `GetHead` | 5 |
| `RemoveAt` | 4 |
| `AddTail` | 4 |
| `RemoveAll` | 3 |
| `GetTailPosition` | 3 |
| `GetTail` | 2 |
| `AddHead` | 1 |

Fourteen methods. A `CLineList<T>` exposing exactly these, backed by the pointer vector,
is a drop-in replacement for the ~140 call sites: `POSITION` becomes `(index + 1)` cast
to `void *` (the `+1` so that index 0 is not `NULL`).

---

## The two things that will break

This is the whole risk of the refactor, and both were found by reading the code, not by
running it.

### 1. Remove-while-iterating

A `CList` POSITION is a node pointer: removing *some other* node does not disturb it. An
index-based POSITION is not — removing element *i* shifts everything after it down by
one, and any POSITION past *i* silently refers to the wrong line.

Three loops do exactly this:

```cpp
// CCedtDoc::DeleteBlock — cedtDocEdit.cpp:110
POSITION pos = m_clsAnalyzedText.FindIndex(nBegY+1);
for(INT i = nBegY+1; i <= nEndY; i++) {
    POSITION posRemove = pos; m_clsAnalyzedText.GetNext(pos);   // pos = i+1
    m_clsAnalyzedText.RemoveAt(posRemove);                       // everything after i shifts
}
```

Also `CCedtView::FlattenScreenTextAt` and `CCedtView::RemoveScreenTextAt`
([cedtViewFormat.cpp](../src/view/cedtViewFormat.cpp)).

**This is precisely the failure mode the refactor is supposed to avoid** — silent
corruption of the wrong line, no crash. So it does not get to be a thing we remember to
check. See *Making it loud*, below.

### 2. Bulk edits become quadratic

`DeleteBlock` removes lines **one at a time in a loop**. On a linked list each removal is
O(1). On a vector each removal is a memmove of the tail — so deleting a 100,000-line
selection out of a 900,000-line file would be 100,000 memmoves. That is not a slowdown,
it is a hang.

`InsertBlock` has the same shape.

**These loops must be rewritten to bulk operations before the container is swapped, not
after.** `CLineList` gets `RemoveRange(nIndex, nCount)` and `InsertRange(nIndex, …)`,
each one memmove regardless of count. This is a *fix*, not a workaround: pasting 100,000
lines is one structural change to the document, and it should cost one.

---

## Making it loud

The plan is not to be careful. The plan is to make carelessness impossible to miss.

`CLineList` keeps a modification counter. Every structural change (`InsertAfter`,
`RemoveAt`, `AddTail`, `RemoveAll`, `InsertRange`, `RemoveRange`) increments it. In debug
builds, the counter is packed into the high 32 bits of the `POSITION` when it is minted,
and every `GetAt` / `GetNext` / `GetPrev` / `RemoveAt` asserts that the POSITION's counter
still matches the list's:

```cpp
// _DEBUG only
ASSERT( _Gen(pos) == m_nModCount );   // "this POSITION was minted before a mutation"
```

A stale POSITION then stops the debugger **on the exact line that used it**, instead of
quietly editing line 41 when it meant line 42.

Legitimate code is unaffected: the patterns that reassign from the return value —
`pos = InsertAfter(pos, x)` — get a freshly minted POSITION and pass. The three broken
loops fail immediately, loudly, the first time they run.

In release builds the POSITION is just the index and the check compiles away.

---

## Plan

**Phase 0 — measure the allocation cost. DONE: the design survives.** `CList` allocates
its nodes from plex blocks; a vector of pointers means one `new` per line. The worry was
that 900,000 individual allocations would eat the win. Measured, building 900,000 lines:

| container | build | teardown | working set |
| --- | ---: | ---: | ---: |
| `CList` (plex-allocated nodes) | 144.3 ms | 76.5 ms | 180.6 MB |
| **`vector<T*>`, one `new` per line** | **141.0 ms** | 93.9 ms | 185.6 MB |
| `vector<T*>` + 1024-line block pool | 139.7 ms | 68.0 ms | 171.3 MB |

The individual allocations are, if anything, marginally *faster*. The reason is that
`CAnalyzedString` already owns a `CString`, so **there is already one heap allocation per
line** — the plex only batches the node wrapper around it, which was never the expensive
part. Moving to a pointer vector does not add an allocation that was not already there.

The block-pool variant wins nothing outside the noise, so it is not worth its complexity.
Plain `vector<CAnalyzedString *>` it is. Memory grows by 5 MB (2.8 %) — the vector itself,
900,000 × 8 bytes — which is the expected price and an acceptable one.

**Phase 1 — bulk operations, on the current `CList`. DONE.** `DeleteBlock` and
`InsertBlock` now ask `CAnalyzedText::InsertLines` / `RemoveLines` for a range instead of
spelling out a loop. On the linked list this changed nothing and sped up nothing — that
was the point. The meaning of an edit gets pinned down, with tests, on the container that
already works, so the swap is not also free to change it.

**Phase 2 — `CLineList<T>`. DONE.** The differential test runs the same pseudo-random
operation sequence against a `CList` and a `CLineList` and compares the entire visible
state — count, forward walk, backward walk, head, tail, every index — after every step,
over 20 seeds. The stale-POSITION detector is itself tested: that every structural change
invalidates, and that the legitimate `pos = InsertAfter(pos, x)` pattern does *not* get
flagged. A false-positive assert is worse than none, because someone would delete it.

**Phase 3 — swap `CAnalyzedText`. DONE.** One base-class change. 140 call sites, none
touched, zero errors, zero warnings. `InsertLines` had to be rewritten on the range
primitives — its first version looped `InsertBefore`, which on an array is both a memmove
per line and, because a POSITION does not survive the first insert, would have laid the
lines down in reverse. The Phase 1 tests caught nothing because they did not have to: they
were written before the swap precisely so they *could* catch it.

**Phase 4 — swap `CFormatedText`. DONE.** `FlattenScreenTextAt` and `RemoveScreenTextAt`
worked in POSITIONs and removed rows while holding one. They now work in row indices,
which is what they were open-coding all along. `RemoveScreenTextAt` is gone: removing a
paragraph at a time would have been a memmove each, so `RemoveScreenText` counts the rows
its lines occupy and drops them in one `RemoveRange`.

**Phase 5 --- measure. DONE.** The claim to verify was never "opening got faster" --- the
open path already avoided FindIndex. It was **typing at line 900,000 stops being slow**.

A SendKeys harness could not see it: SendKeys imposes a flat ~30 ms per keystroke of its
own, and that floor was identical at the top and the bottom of the file *even on the old
build*. So the gateway itself was instrumented instead --- the same ten-line probe applied
to both trees, `CCedtView::GetLineFromPosY`, timed and averaged over 200 calls:

| caret position | before (CList) | after (CLineList) |
| --- | ---: | ---: |
| line 1 | 0.1 us | 0.2 us |
| **line 900,000** | **6,582 - 10,445 us** | **0.2 us** |

Before, reaching the line under the caret cost **7 to 10 milliseconds**, every time, and a
keystroke does it more than once. After, it costs the same at the bottom of the file as at
the top, because there is no longer a walk to do. Opening the 100 MB file is unchanged at
2.5 s.

Out of scope: `CMemText`, `CUndoBuffer` and the other small `CList`s. They are appended to
and walked head-to-tail; they never index. Leave them.

---

## What to know before touching this

**A POSITION does not survive a structural change.** This is the one thing that is
different, and it is the one thing that can go wrong silently. Do not carry a POSITION
across an `InsertAfter` / `RemoveAt` / `AddTail` / `InsertGap` / `RemoveRange` — re-fetch
it, or use the one the mutating call returned. Debug builds assert if you get it wrong;
release builds do not, so do not develop against release only.

**Removing in a loop is quadratic.** `RemoveAt` in a loop is a memmove per element. Use
`RemoveRange`. The same goes for `InsertGap` versus repeated `InsertAfter`. Two tests
(`RemoveALargeRange`, `LargeBulkOpsAreNotQuadratic`) exist to make a regression here show
up as a hang in CI rather than a bug report about pasting.

**Elements are held by pointer on purpose.** Do not "simplify" `std::vector<TYPE *>` to
`std::vector<TYPE>`. The editor holds C++ references into the container across mutations
of it, and values would relocate and dangle — silently, and usually appearing to work.
`CLineList.ReferencesIntoTheListSurviveInsertsAndRemovals` pins this.

**Fixed on the way past.** `FindScreenTextIndex` returned an uninitialised `POSITION` when
the row list was empty. Its replacement, `FindScreenTextRow`, returns -1 and the callers
check.

**Flushed out on the way past: not every view on the document is a `CCedtView`.** Print
preview crashed in the access-violation way — no assert, just gone. It had nothing to do
with the container, and everything to do with having moved it.

MFC's print preview registers its view *with the document*: `DoPrintPreview` builds the
`CPreviewView` with a `CCreateContext` naming the document, and `CView::OnCreate` obliges
with `pDoc->AddView(this)`. So while preview is up, the view list holds a view that is not
ours. All fourteen walks of that list did this:

```cpp
CCedtView * pView = (CCedtView *)pDoc->GetNextView( posView );   // no check
```

Resize the main window during preview and `ApplyCurrentScreenFont` reaches the
`CPreviewView`, calls `OnScreenFontChange()` on it, and reads `m_dcActiveLine` — at an
offset that lies past the end of the real object. The garbage there was non-zero, so the
`if( m_dcActiveLine.m_hDC )` guard passed and the garbage went to `DeleteDC()`. The
debugger showed `attrib=0xcdcdcdcd00000333`: `0xcdcdcdcd` is what the debug heap writes
into memory it has not constructed. `m_hWnd` looked fine, because both classes are `CWnd`s
and the front of the object still lines up.

The cast was always wrong. What changed is that `CFormatedText` swapped `CList` for
`CLineList`, which moved every member after it — and the garbage `m_dcActiveLine` landed on
went from harmlessly zero to not. It was a latent bug living on luck, and the refactor
spent the luck.

Fixed by routing every walk of the view list through `CCedtDoc::GetNextCedtView`, which
skips what is not ours, rather than by adding fourteen guards someone can forget the
fifteenth time.

**Memory.** A `CList` node carries two pointers of overhead (16 bytes). A pointer vector
carries 8. Slightly better, and the line objects themselves are unchanged.

---

## The quadratic loop this document warned about, in this document's own code

*Fixed after the fact. Recorded because the way it was missed is the point.*

The plan above has a section called **Bulk edits become quadratic**. It says: on an array,
removing or inserting one element at a time is a memmove per element, so the loops that do
that must be rewritten to range operations *before* the container is swapped. `DeleteBlock`
and `InsertBlock` were. `RemoveScreenText` was. **The word-wrap formatter was not.**

`FormatScreenText` lays out a wrapped line and appends however many continuation rows it
turns out to need. Phase 4 translated that faithfully:

```cpp
po2 = m_clsFormatedScreenText.InsertAfter(po2, dummyLine);   // CList: O(1) node splice
m_clsFormatedScreenText.InsertGap(nRow + 1, 1);              // CLineList: memmove the tail
```

Identical meaning, and the cost went from O(1) to O(n) — per row, over the whole document.
Measured on 90,000 lines wrapping into 360,063 rows:

| | wrap-on `FormatScreenText` |
| --- | ---: |
| before the refactor (`CList`) | 167.9 ms |
| **after Phase 4 (`CLineList`)** | **2,778.9 ms** |
| after the fix | 194.1 ms |

**16× slower, and it shipped.** Nothing caught it: the differential tests compare *what the
container holds*, not what it costs, and `LargeBulkOpsAreNotQuadratic` covers `RemoveRange`
and `InsertGap` — the calls that were already correct. Phase 5 measured with word wrap
**off**, where a line is exactly one row and nothing is ever inserted, and reported the open
path unchanged. It was. The one path that inserts was the one path not measured.

The fix is the same one the plan prescribes: produce the run, hand it over once.
`CLineList::ReplaceRange(nIndex, nOldCount, ppNew, nNewCount)` swaps a run of rows for a run
of a different length in one structural change, and the formatter builds its rows off to the
side and splices them in. `FormatPrintText` had the identical loop and got the identical
fix. `FlattenScreenTextAt` — which existed only to remove last time's continuation rows one
line at a time — is gone; the splice replaces them.

`CLineList.ReplaceRangeOverAWholeListIsNotQuadratic` now pins it. A cost regression needs a
test that fails on cost, and there wasn't one.

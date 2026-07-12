# Replacing the line container

The document's lines live in an MFC `CList` — a doubly-linked list. Reaching line *n*
means walking *n* nodes from the head. This document plans replacing it with a
random-access container.

> Status: **planned, not implemented.** Written before the work so the risks are on the
> record rather than discovered halfway through.

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

**Phase 1 — bulk operations, on the current `CList`.** Rewrite `DeleteBlock` and
`InsertBlock` to remove and insert ranges rather than looping element by element. This is
correct and slightly faster on the list too, it is independently testable, and it means
the container swap does not also have to change edit semantics. Lands on its own.

**Phase 2 — `CLineList<T>`.** New container in `src/core`, with the 14 methods plus the
two range operations, the modification counter, and the debug POSITION check. It is pure
data structure with no MFC-app dependency, so it goes straight into `cedt_tests` — and the
test that matters is a **differential** one: run the same random sequence of operations
against `CList` and `CLineList` and assert the visible state is identical after every step.
That is what proves it is a drop-in, rather than hoping.

**Phase 3 — swap `CAnalyzedText`.** One base-class change. Build, run the suite, run the
app under the debug assert. Fix whatever the assert catches.

**Phase 4 — swap `CFormatedText`.** Same, plus `FlattenScreenTextAt` /
`RemoveScreenTextAt`.

**Phase 5 — measure.** The claim to verify is not "opening got faster" (it will not; the
open path already avoids `FindIndex`). It is **typing at line 900,000 stops being slow**,
and **`FindIndex` disappears from the profile**.

Out of scope: `CMemText`, `CUndoBuffer` and the other small `CList`s. They are appended to
and walked head-to-tail; they never index. Leave them.

---

## What could still go wrong

**`GetCount()` returns `INT_PTR` today and is called in loop conditions.** Types must
match or `for(INT i = 0; i < GetCount(); i++)` changes behaviour at the boundary. Keep the
signature.

**A POSITION crossing a function boundary.** No `POSITION` is stored as a member anywhere
(checked), so every one lives and dies inside a single function — but `FindScreenTextIndex`
and `FlattenScreenTextAt` *return* one to their caller. Those returns are fine as long as
the caller does not mutate before using it; the debug assert covers it.

**Memory.** A `CList` node carries two pointers of overhead (16 bytes). A pointer vector
carries 8. Slightly better, and the line objects themselves are unchanged.

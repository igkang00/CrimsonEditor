# Memory-Safety Refactoring Notes

A pass over the codebase looking for memory-safety hazards — buffer overruns, unsafe ownership, lifetime / Rule-of-Three gaps. Same "discover → categorise → fix" approach as [configuration.md](configuration.md).

The actual fixes are not yet applied. This document is the working list to decide which ones to tackle and in what order.

---

## 1. Scope and method

Three layers of scanning:

1. **Broad pattern counts** across `src/` for the usual memory-safety smells: `new TYPE[]` / `delete[]`, `new TYPE(...)` / `delete X`, `strcpy` / `sprintf` / `strcat`, `strncpy` / `_snprintf` / `lstrcpy`, `memcpy` / `memset`, fixed-size stack buffers (`TCHAR name[N]`), function-scope `static` buffers.
2. **Per-call review** of the worst hotspot — all 52 `strcpy` / `sprintf` / `strcat` sites — checking destination size against source guarantees.
3. **Structural review** of two more hotspots: ownership in the network layer (`FtpClnt.cpp` + `RemoteFile.cpp`) and raw owning pointers inside the `cedtElement` domain classes.

### Pattern counts (src/)

| Pattern | Count | Files |
| --- | --- | --- |
| `new TYPE[...]` (array) | 21 | 9 |
| `delete[]` | 21 | 8 |
| `new TYPE(...)` (single object) | 10 | 2 (mostly `FtpClnt.cpp`) |
| `delete X` (single object) | 56 | 11 (`FtpClnt.cpp` 18, `RemoteFile.cpp` 17) |
| **`strcpy` / `strcat` / `sprintf` (unbounded)** | **52** | 13 |
| `strncpy` / `_snprintf` / `lstrcpy` (bounded) | 15 | 6 |
| `memcpy` / `memset` / `memmove` | 57 | 18 |
| Fixed-size `TCHAR name[N]` | 44 | 23 |
| `static TCHAR/CHAR name[N]` (function-scope) | 2 | 2 |

The ratio of unbounded to bounded string-copy calls (52 / 15 — roughly 77 % unbounded) is the strongest single signal: the project hardly uses the safer `strncpy` / `_snprintf` variants at all.

---

## 2. Background — what the project's actual line / word limits are

This matters for evaluating buffer-size assumptions.

| Limit | Macro | Value | Where enforced |
| --- | --- | --- | --- |
| Max line length (chars) | `MAX_STRING_LENGTH` | **32767** | [cedtDocAnal.cpp:109](../src/doc/cedtDocAnal.cpp#L109) sets `LI_HAVEOVERFLOW` on the analyzed line |
| Line buffer size (chars incl. NUL) | `MAX_LINE_BUFFER_SIZE` | 32768 | derived as `MAX_STRING_LENGTH + 1` |
| Max word length (chars) | `MAX_WORD_LENGTH` | 255 | `CDictionary::AddWord` etc. |
| Max words per line | `MAX_WORDS_COUNT` | 32767 | analysis loop |

Originally there was no `MAX_LINE_BUFFER_SIZE` (or even a clearly named "buffer for one line") macro. The number **2048 appears as a magic number in dozens of places** for unrelated short-string buffers (config-file version headers, evaluator tokens, dialog buffers, etc.). A subset of these are buffers that need to hold a whole *line* — and those silently assume "a line fits in 2048" which is wrong. See §6 below for the structural side of this and what was added.

---

## 3. Findings

### 🔴 High — confirmed bug

#### H1. ~~`ActionEvaluateLine` stack overflow on long lines~~ — fixed

[src/view/cedtViewAction.cpp:16](../src/view/cedtViewAction.cpp#L16)

```cpp
void CCedtView::ActionEvaluateLine()
{
    INT nIdxX, nIdxY; PositionToIndex( m_nCaretPosX, m_nCaretPosY, nIdxX, nIdxY );
    CAnalyzedString & rString = GetLineFromIdxY( nIdxY );
    TCHAR szFormula[2048]; strcpy(szFormula, rString);   // <-- here
    ...
}
```

Reads the caret's current line into a 2048-byte stack buffer. The project's enforced line limit is 32767 (§2). Any line in the range 2048 … 32767 chars (minified JS, JSON, log lines, generated code, ...) triggers a stack overrun the moment the user presses the "evaluate expression" shortcut on that line. `/RTC1` catches it in Debug; Release silently corrupts the stack.

Suggested fix — drop the fixed buffer entirely, work straight from the `CString`:

```cpp
double dValue; INT nError;
TCHAR * pExpr = EVAL::Evaluate(
    const_cast<TCHAR *>((LPCTSTR)rString), &dValue, &nError );
```

(`EVAL::Evaluate` already takes a `TCHAR *`; the const_cast matches the existing signature without copying.) If a defensive copy is preferred, allocate `new TCHAR[rString.GetLength()+1]` and delete after use.

**Applied.** The 2048-byte buffer and the `strcpy` were removed; the evaluator is now called on the line directly. No more stack overrun regardless of line length.

### 🟠 Medium — likely bug, fix scope to be decided

#### M1. ~~`PathName.cpp:38-39` — 256-byte filter buffer~~ — fixed

[src/util/PathName.cpp:38](../src/util/PathName.cpp#L38)

```cpp
TCHAR szFilter[256]; strcpy( szFilter, lpszFilter );
if( szFilter[strlen(szFilter)-1] != ';' ) strcat( szFilter, ";" );
```

256 is small. User-defined file-filter strings can exceed it (e.g. a multi-language project with dozens of extension globs). The companion at line 7 is `szFilter[4096]` which is also a guess but much safer. Fix: switch to `CString` so no fixed bound is needed; or at least bump to 4096 and use `strncpy`.

**Applied.** Replaced the fixed-size stack buffer with a heap allocation sized to `lstrlen(lpszFilter) + 2`. The function now handles any filter length and cleans up via `delete[]` on the single exit. The same pattern would apply to `PathName.cpp:7-8` if needed in the future, but that buffer is 4096 bytes which is unlikely to overrun in practice.

#### M2. ~~`cedtAppProject.cpp` — MAX_PATH unguarded copy of `CString`~~ — fixed

[src/app/cedtAppProject.cpp:16](../src/app/cedtAppProject.cpp#L16) and `:56`

```cpp
TCHAR szInitialDirectory[MAX_PATH]; strcpy( szInitialDirectory, m_szProjectInitialDirectory );
if( ! strlen( szInitialDirectory ) ) strcpy( szInitialDirectory, szCurrentDirectory );
```

Both sources are `CString` values whose length is not checked. Windows extended paths (`\\?\…`) can be much longer than `MAX_PATH` (260). Fix: keep them as `CString` throughout, or use `strncpy` with `MAX_PATH - 1` and explicit null termination.

**Applied.** Switched both call sites to `lstrcpyn(dst, src, MAX_PATH)`. Truncation rather than overrun on absurdly long paths — acceptable here because the value goes into the initial-directory hint of a file dialog, where a truncated path just falls back to a different starting folder rather than corrupting state.

#### M3. ~~`cedtViewCommand.cpp` — external-process command line~~ — fixed

Five sites, [src/view/cedtViewCommand.cpp:156](../src/view/cedtViewCommand.cpp#L156), `:360`, `:362`, `:444`, `:451`. `szCommandLine[2048]` and read/write buffers for child-process I/O. The command line gets built from `lpszCommand + " " + lpszArgument` with `sprintf`; the user can configure arbitrarily long tool arguments.

Fix: build the command line in a `CString` and pass its `LPCTSTR` to `CreateProcess`.

**Applied.**
- `:156` — `lstrcpyn` for the initial-directory copy (same pattern as M2).
- `:360`, `:362` — `sprintf` replaced with `_snprintf(buf, kCmdLineMax - 1, ...)` plus an explicit null-terminator at the end (MSVC `_snprintf` does not null-terminate on overflow).
- `:444-451` — child-process write/read 4 KB buffers. Length capped to `kWriteBufMax - 2` before `lstrcpyn` (leaving room for the `'\n' '\0'` postfix), and the local-echo copy on `:451` only runs when `dwSave + dwWrite + 1 < kReadBufMax`. Truncation can now occur on multi-KB single-line input to a child process — acceptable; the alternative was a guaranteed overrun.

#### M4. ~~`FileWndDirectory.cpp` — MAX_PATH unguarded across file ops~~ — fixed

About 7 sites between [src/panels/FileWndDirectory.cpp:418](../src/panels/FileWndDirectory.cpp#L418) and `:1028` — every shell-like file op (copy, move, rename, delete) builds a `szFrom[MAX_PATH]` / `szDest[MAX_PATH]` with `strcpy`. Same root cause as M2.

**Applied.** Two sub-patterns:
- `:418`, `:561`, `:573` — copies that are followed by an appended `'\\'`. Used `lstrcpyn(..., MAX_PATH - 1)` to leave one byte of head-room for the append, and added a `nLen > 0` guard on the in-place `'\\'` write.
- `:984`–`:1028` — `SHFileOperation` buffers that are deliberately pre-zeroed with `memset` to satisfy the API's double-null-terminated requirement. Switched the copy to `lstrcpyn(..., MAX_PATH)`; the `memset` is still doing the double-null work.

#### M5. ~~`cedtViewHndrEdit.cpp:516` — function-scope `static` MAX_PATH buffer~~ — fixed

```cpp
static TCHAR szInitialDirectory[MAX_PATH] = "";
if( ! strlen( szInitialDirectory ) ) strcpy( szInitialDirectory, szCurrentDirectory );
```

Two problems: no length guard on the strcpy, and `static` means there's a single shared instance — fine in the current single-threaded MFC main loop, but a footgun if anything ever drives this off the main thread.

**Applied.** Length problem fixed with `lstrcpyn(szInitialDirectory, szCurrentDirectory, MAX_PATH)`. The `static`-storage / thread-safety concern is left untouched (the function is only ever reached from the main UI thread); it remains noted under L2.

#### M6. ~~`FtpClnt.cpp` — `delete` without `NULL` assignment in fail handlers~~ — reanalysed, not a real bug

The lines I initially flagged as "fail handlers" (`FtpClnt.cpp:502-508` and `:578-584`) are actually the bodies of `CloseControlChannel` and `CloseDataChannel`, and they **already follow the `delete x; x = NULL;` pattern**. `CloseServer` does the same. The destructor (lines 29-42) deletes without nulling, but that is harmless because the object is about to be destroyed — no further access reaches those pointers.

A truly defensive `delete x; x = NULL;` in the destructor would be valid style, but it does not change observable behaviour. Treating this as resolved with no code change. The earlier write-up came from misreading the function boundary.

#### M7. ~~`CAnalyzedString` / `CFormatedString` — Rule of Three violation~~ — fixed

[src/core/cedtElement.h](../src/core/cedtElement.h) lines 305-352. Both classes own a raw array via `m_pWord` (`ANALYZEDWORD*` / `FORMATEDWORD*`), with:

- 6+ user-defined constructors that initialise `m_pWord = NULL` ✅
- destructor that does `delete [] m_pWord` ✅
- `operator=` declared (and defined elsewhere) ✅
- **No user-defined copy constructor.** The compiler generates one that shallow-copies `m_pWord`, so any accidental by-value copy hands two objects the same array, leading to dangling-pointer reads and a double-free on destruction.

Today this is mostly latent because:
- `CAnalyzedText` is `CList<CAnalyzedString, LPCTSTR>` — `AddTail` takes a `LPCTSTR` and constructs a new element via the `LPCTSTR` constructor (not the copy constructor)
- The codebase consistently passes these by reference

…but it is a footgun: a future change that takes `CAnalyzedString` by value, or pushes it into a generic container, immediately corrupts memory.

Fix: define the copy constructor explicitly. Either delete it (`= delete`) to make any by-value use a compile error, or implement a proper deep copy. Same for `CFormatedString`.

**Applied.** Added `CAnalyzedString(const CAnalyzedString &) = delete;` and `CFormatedString(const CFormatedString &) = delete;` in `cedtElement.h`. The full project (all four configurations) + `cedt_tests` still build, which is also a compile-time proof that the codebase nowhere copies these objects by value today. Any future accidental by-value use becomes a compile error rather than a memory corruption.

### 🟡 Low — structural / nice-to-have

#### L1. `RemoteFile.cpp` — manual `delete` in every error path

[src/network/RemoteFile.cpp](../src/network/RemoteFile.cpp). 17 `delete` calls across multiple functions; each error branch repeats the `Close(); delete` pattern. Currently consistent, but fragile — adding a new error path is one missed cleanup away from a leak. Replacing the raw `delete`s with `std::unique_ptr` + a custom deleter that calls `Close()` would make the cleanup automatic. Larger refactor, not urgent.

#### L2. Function-scope `static` buffers

Two occurrences (`cedtElement.cpp` `CDictionary::LookupTable`, `cedtView.cpp`). Safe under the current single-threaded UI assumption, but should be flagged any time multithreading enters this code (e.g. background syntax analysis).

#### L3. ~~Sizes to double-check (currently unrated)~~ — checked

Checked one by one:

- [cedtElement.cpp:289-323](../src/core/cedtElement.cpp#L289) — `sprintf(szBuffer, "I:%s"/"C:%s", szWord)`. `szBuffer` is declared `[MAX_WORD_LENGTH+3]` (= 258) and the caller asserts `siLength > MAX_WORD_LENGTH` then returns FALSE. The 2-byte prefix `"I:"`/`"C:"` + 255 + null fits exactly. **Safe as written.** A separate concern is the `sin >> szWord` operator>> in `CKeywords::FileLoad` which does not bound the read; that's a different bug class (operator>>) and is captured below.
- [FileWndProject.cpp:726-728](../src/panels/FileWndProject.cpp#L726) — `PROJECTITEMINFO::szText` / `szPathName` are both `TCHAR[MAX_PATH]`, same pattern as M2/M4. **Fixed alongside M4** with `lstrcpyn(..., MAX_PATH)`.
- [FileTab.cpp:137](../src/panels/FileTab.cpp#L137) — destination `szText[MAX_PATH]`, source `pDoc->GetTitle()` (a `CString`, unbounded). **Fixed** with `lstrcpyn(szText, pDoc->GetTitle(), MAX_PATH)`.

New finding surfaced during L3: `CKeywords::FileLoad` uses `sin >> szWord` without an `setw` width — a long word in a keyword file would overrun. Worth a follow-up but lives outside the strcpy/sprintf family this review focused on.

#### L4. `memcpy` of POD structs

57 calls. The bulk are safe — copying `LOGFONT[]`, `COLORREF[]`, etc. as `sizeof(target)`. Worth a one-pass review to catch any odd cases but no obvious smoking gun.

---

## 4. Status

All H/M items and L3 are applied. The cross-cutting §5 macro is partially applied (the macro is defined; old literal usages were left in place for the reasons noted in §5).

Applied:

- **H1** — `ActionEvaluateLine` no longer copies into a 2048-byte stack buffer.
- **M1** — `MatchFileFilter` switched to a heap buffer sized to the input length.
- **M2** — `cedtAppProject.cpp` initial-directory copies use `lstrcpyn(MAX_PATH)`.
- **M3** — `cedtViewCommand.cpp` external-process cmdline uses `_snprintf` + explicit null-terminate; child-process I/O buffers length-capped.
- **M4** — `FileWndDirectory.cpp` all 7 file-op buffers use bounded copies (one variant for paths followed by `'\\'`, another for `SHFileOperation` double-null buffers).
- **M5** — `cedtViewHndrEdit.cpp` insert-file initial-directory uses `lstrcpyn`.
- **M6** — reanalysed and resolved with no code change.
- **M7** — `CAnalyzedString` / `CFormatedString` copy constructors `= delete`d.
- **L3** — sized buffers triaged: one safe-as-written, two fixed (FileTab.cpp:137, FileWndProject.cpp:726-728).
- **§5** — `MAX_STRING_SIZE` renamed to `MAX_STRING_LENGTH` (matches existing convention) and `MAX_LINE_BUFFER_SIZE = MAX_STRING_LENGTH + 1` added for one-line buffers; literal-sweep deferred.

Still open:

- **L1** — `RemoteFile.cpp` raw `delete` pattern, candidate for `unique_ptr` + custom deleter. Larger refactor, not urgent.
- **L2** — function-scope `static` buffers; safe under current single-threaded UI assumption.
- **L4** — POD-struct `memcpy` sites; one-pass sanity check welcome but no smoking gun.
- **Follow-up from L3** — `CKeywords::FileLoad` does `sin >> szWord` without a width — a long word in a keyword file would overrun. Outside the strcpy/sprintf family but worth a follow-up.

---

## 5. Cross-cutting structural finding — magic number `2048`

Separate from the bug list, the recurring `2048` is itself a maintenance hazard:

- No `MAX_LINE_LENGTH` (or any well-named constant) anchors the value.
- Some uses are intentional short-buffer sizes (config-file version headers — known to be short).
- Some uses are "should be enough for one line" assumptions — which contradict the actual `MAX_STRING_LENGTH = 32767` line limit.
- A reader can't tell the two apart without reading the surrounding code.

Suggested grooming (low priority, but easy to introduce alongside the fixes above):

- Add `MAX_LINE_BUFFER_SIZE = MAX_STRING_LENGTH + 1` to [src/core/cedtElement.h](../src/core/cedtElement.h).
- Replace the "holds-a-whole-line" usages with the named constant.
- Leave the genuinely short-string usages as their own named constants (`MAX_VERSION_HEADER`, `MAX_EVAL_TOKEN`, etc.) — or `CString` where appropriate.

Even partial application makes the next "is this buffer big enough?" question answerable from the macro alone.

**Partially applied.** Two changes in [src/core/cedtElement.h](../src/core/cedtElement.h):

1. Renamed `MAX_STRING_SIZE` (32767) → **`MAX_STRING_LENGTH`** so the project's "LENGTH = number of characters, SIZE = number of slots" convention (already used by `MAX_WORD_LENGTH` / `MAX_WORDS_COUNT`) becomes consistent. All 11 in-tree call sites were updated.
2. Added **`MAX_LINE_BUFFER_SIZE` = `MAX_STRING_LENGTH + 1`** so a buffer that needs to hold one full line plus its trailing NUL has a named, correct-by-construction size. The existing `TCHAR szLine[MAX_STRING_SIZE+1]` in `cedtElement.cpp` (which was already doing the `+1` by hand) was switched to use the new macro.

Existing `2048` / `32767` literal usages elsewhere were left untouched — most are short-string buffers (config headers, evaluator tokens) that are not line buffers and would become misleading if renamed. A follow-up sweep can adopt the new name in any new code or any place a "whole-line" buffer is genuinely needed.

---

## 6. Source pointers

- [src/view/cedtViewAction.cpp](../src/view/cedtViewAction.cpp) — H1 target
- [src/util/PathName.cpp](../src/util/PathName.cpp) — M1 target
- [src/app/cedtAppProject.cpp](../src/app/cedtAppProject.cpp), [src/app/cedtAppHndr.cpp](../src/app/cedtAppHndr.cpp), [src/view/cedtViewCommand.cpp](../src/view/cedtViewCommand.cpp), [src/view/cedtViewHndrEdit.cpp](../src/view/cedtViewHndrEdit.cpp), [src/panels/FileWndDirectory.cpp](../src/panels/FileWndDirectory.cpp) — M2 / M3 / M4 / M5 targets
- [src/network/FtpClnt.cpp](../src/network/FtpClnt.cpp) — M6 target
- [src/network/RemoteFile.cpp](../src/network/RemoteFile.cpp) — L1
- [src/core/cedtElement.h](../src/core/cedtElement.h) — M7 target + §5 macro suggestion
- [src/core/cedtElement.cpp](../src/core/cedtElement.cpp), [src/panels/FileWndProject.cpp](../src/panels/FileWndProject.cpp), [src/panels/FileTab.cpp](../src/panels/FileTab.cpp) — L3 to-verify list
- [src/doc/cedtDocAnal.cpp](../src/doc/cedtDocAnal.cpp) — the analyzer that enforces `MAX_STRING_LENGTH`

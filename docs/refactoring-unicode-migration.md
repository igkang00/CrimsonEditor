# MBCS → Unicode Migration

Working notes for the `feat/unicode-migration` branch — turning Crimson Editor from an MFC MBCS build (CP949 on a Korean host, CP1252 on an English host) into a native Unicode (UTF-16 internal) build.

The end state of this branch is: every string CString/API touches is UTF-16, the two editions still ship as `cedt_kr.exe` / `cedt_us.exe` but their input path can handle *any* Unicode character (em dash, arrows, CJK extensions, filenames with characters outside CP949, etc.). Files on disk keep their existing encoding — the load/save conversion just no longer downgrades through the ANSI code page.

---

## Decisions (locked in at branch start)

| | Decision | Rationale |
| --- | --- | --- |
| Branch name | `feat/unicode-migration` | mirrors `feat/64bit-port` — same shape of "big architectural refactor on its own branch" |
| Internal representation | **UTF-16** (MFC `CStringW`, `TCHAR = wchar_t`) | MFC + Windows-only app. Every W-suffix API is UTF-16 native; caret/selection stays character-indexed with no continuation-byte scanning; `CString` becomes UTF-16 automatically once `_UNICODE` is defined. UTF-8 internal would require a wrapper class replacing every `CString` use, for no Windows-native benefit |
| File format on disk | unchanged | The existing encoding detector (ASCII / UTF-8 with/without BOM / UTF-16 LE/BE) stays. Load path stops downgrading to `CP_ACP`; save path keeps writing the file's declared encoding. This is what makes em dash / CJK-outside-CP949 characters survive |
| Legacy MBCS-only edition | **drop** | We're already x64-only after the previous branch. Adding a Unicode-only build is the natural next step; no reason to dual-ship |
| User settings compatibility | **bump `STRING_CONFIGURATIONVER` etc., let the old configs reset** | Same rationale as the x64 branch: config files are binary and hold `TCHAR` buffers whose element size just changed from 1 byte to 2 bytes. A migration path is real work for no gain — reset with an explanation is the honest move. `cedt.color`, `cedt.ftp`, `cedt.tools`, `cedt.macro` all get the same treatment |
| Target version | **3.90** | Semi-major bump. User-visible behavior changes (all Unicode characters render, filenames with non-CP949 characters open) warrant more than a `.01` bump but this isn't an API break for anyone embedding the app |
| Approach | **Phase by phase**, each with its own build + test cycle | Unicode migration touches more surface area than the x64 port (IME, file I/O, string literals across ~100 files). Landing it in one go is high risk. Better to compile-clean each phase before the next |

---

## Environment check (Phase 0 — done)

| Component | Status |
| --- | --- |
| MSVC v145 x64 toolset (`cl.exe Hostx64\x64`) | ✅ already used for x64 build at `VS\18\Professional\VC\Tools\MSVC\14.51.36231` |
| MFC v145 x64 Unicode dynamic libraries | ✅ `mfc140u.lib` (9.1 MB release) + `mfc140ud.lib` (10.4 MB debug) present under `atlmfc\lib\x64` |
| MFC v145 x64 Unicode support libraries | ✅ `mfcs140u.lib` / `mfcs140ud.lib` + `MFCM140U.lib` / `MFCM140Ud.lib` present |
| Windows SDK 10 — Unicode `Imm32.lib` | ✅ standard, same lib for both A/W |
| Bundled `HtmlHelp.h` (1999-vintage) | ✅ ships **both** `HtmlHelpA` and `HtmlHelpW` prototypes with a `#ifdef UNICODE` macro routing to the right one. No wrapper needed. Structure fields are already `LPCTSTR` throughout (one stray `LPCSTR pszCatName` in the info-type-enum struct, which we don't use) |
| Test project (`cedt_tests`) built with GoogleTest through vcpkg | ✅ vcpkg manifest tolerant of either charset, gtest itself doesn't care |

---

## Phase plan

### Phase 1 — Build-system flip + first compile attempt (done)

1. ✅ `cedt.vcxproj`:
    - `<CharacterSet>MultiByte</CharacterSet>` → `Unicode` in both `Release-KR|Release-US` and `Debug-KR|Debug-US` Configuration groups.
    - `<PreprocessorDefinitions>` — `_MBCS` → `_UNICODE;UNICODE` in both Release and Debug ItemDefinitionGroups.
2. ✅ `tests/cedt_tests.vcxproj` — same treatment (CharacterSet × 2, PreprocessorDefinitions × 2).
3. ✅ `tools/launch/launch.vcxproj` — same treatment. (`tools/shellext/shellext.vcxproj` was already Unicode.)
4. ✅ Verification: `_MBCS` / `MultiByte` count is `0` across all three projects; `_UNICODE` / `Unicode` count matches the expected number of ClCompile blocks.

**First Debug-KR|x64 rebuild produced 2,521 errors** — as expected. Distribution:

| Code | Count | Meaning | Fix category |
| --- | --- | --- | --- |
| `C2664` | 2,133 (85%) | function argument `char*` ↔ `TCHAR*` mismatch | wrap literal with `_T("...")`, retype local buffer |
| `C2665` | 309 (12%) | overloaded function had no viable `TCHAR*` overload for a `char*` arg | same fix pattern as C2664 |
| `C2440` | 55 (2%) | plain `=` assignment `char*` ↔ `TCHAR*` | retype variable, or wrap literal |
| `C2678` | 20 (<1%) | `std::istream::operator>>` on a `char` when the sink is now `wchar_t` | use `std::wistringstream` / `std::wifstream` (or bridge through a narrow string on the IO side) |
| `C2660` | 1 | `MultiByteToWideChar` called with 5 args instead of 6 | manual fix in `XPTabCtrl.cpp:422` |
| `C1003` | 3 | "too many errors in one file, stopping" | not real errors, just cl.exe giving up early — the actual count is larger than 2,521 |

Nothing surprising. The overwhelming majority (99%) is one class of problem: a string literal or a `char`-typed variable being handed to an API that is now `TCHAR`-typed. All addressable by the two Phase 2 patterns (wrap the literal with `_T()`; retype the variable). The three exotic codes are one-off targeted fixes.

The `C1003` note means we're not seeing every error yet — `cl.exe` stops after 100 in a single translation unit. Phase 2 fixes will make more errors visible in later passes. This is normal; we knew going in.

Raw log is at `%TEMP%\cedt-unicode-first.log` for anyone who wants the individual sites.

### Phase 2 — Mechanical fixes (bulk-scriptable)

Groupable into a small number of patterns, each of which can be handled by a targeted find/replace:

1. **String literals**: `"..."` used in `TCHAR`-taking context → wrap with `_T("...")`.
    - Don't blindly wrap every literal — a literal passed to a socket API stays `char`.
    - Two-pass approach: compile errors identify which literals need wrapping; a script edits those specific sites.
2. **Explicit char types**: `char szBuf[N]` used with `TCHAR`-family APIs → `TCHAR szBuf[N]`.
    - Or `WCHAR szBuf[N]` where already Unicode-specific.
3. **String function calls**:
    - `strlen` → `_tcslen`
    - `strcpy` / `strcpy_s` → `_tcscpy` / `_tcscpy_s`
    - `strcmp` / `stricmp` / `_stricmp` → `_tcscmp` / `_tcsicmp`
    - `strcat` → `_tcscat`
    - `sprintf` / `_snprintf` → `_stprintf` / `_sntprintf`
    - `atoi` / `atol` → `_ttoi` / `_ttol`
    - `strncpy`, `strchr`, `strstr`, `strtok` → their `_tcs*` equivalents
    - Some of this migration is already partly done from the x64 port (the codebase already uses `_tcslen` in several places) — audit remaining `str*` calls.
4. **Char literals passed to APIs**: `'A'` sometimes → `_T('A')` when compared against `TCHAR`.
5. Recompile. Expect a much smaller residual error set.

### Phase 3 — File I/O logic (the actual behavior change)

The heart of the "em dash disappears" bug lives here. Currently `src/core/cedtElement.cpp` UTF-8 load path does:

```cpp
MultiByteToWideChar(CP_UTF8, 0, szBuffer, -1, szWideBuffer, ...);   // step 1 — UTF-8 → UTF-16, GOOD
WideCharToMultiByte(CP_ACP,  0, szWideBuffer, -1, szBuffer, ...);   // step 2 — UTF-16 → CP949, DATA LOSS
```

Step 2 exists because the document's in-memory buffer is `char*`. Once the app is `_UNICODE`, the in-memory buffer is `wchar_t*` and step 2 goes away entirely — the file's UTF-8 bytes reach the buffer as their original code points and no CP949 downgrade happens.

Tasks:

1. Change `CFormatedText` / `CAnalyzedString` / `CUndoBuffer` etc. internal storage to `wchar_t`-based (`CStringW` / `CList<wchar_t>` — whatever the current shape is, mirrored in wide).
2. Rewrite load functions in `cedtElement.cpp` for each encoding:
    - ASCII → wide via `MultiByteToWideChar(CP_ACP, ...)` (or `CP_1252`)
    - UTF-8 (with or without BOM) → wide via `MultiByteToWideChar(CP_UTF8, ...)` (drop the second conversion)
    - UTF-16 LE/BE → memcpy (with byte-swap for BE) into `wchar_t`
3. Rewrite save functions symmetrically: wide → target encoding on the way out.
4. Preserve the BOM handling that's already there.
5. Test round-trip: load `em dash → save → reload` and check the character survives.

### Phase 4 — IME / character input

Windows Unicode-mode IME behavior is different from MBCS:

- `WM_IME_CHAR` fires once per full character (UTF-16 code unit), not once per byte. Existing MBCS logic that expects two `WM_IME_CHAR` events (lead byte + trail byte) for a Korean syllable needs to collapse to one.
- `ImmGetCompositionStringA` → `ImmGetCompositionStringW` — the `A`/`W` selection is via the same `TCHAR` macro that `_UNICODE` flips. Free change.
- `IsDBCSLeadByte` — no longer needed anywhere in the code path; every occurrence should be removed. Grep + delete.
- `g_bDoubleByteCharacterSet` global (see `src/view/cedtView.cpp`) — decide whether to keep it (for legacy files stored in MBCS that the user is editing) or retire it. Probably retire — Unicode-in-buffer means no DBCS.

Tasks:

1. Rewrite `WM_IME_CHAR` / `WM_CHAR` handlers in `src/view/cedtView.cpp` and `src/view/cedtViewEditCompose.cpp` for the Unicode path.
2. Delete DBCS lead-byte tracking and the `cLeadByte` state variable.
3. Retire `g_bDoubleByteCharacterSet` (or keep as `TRUE` shim if there's non-obvious downstream usage — check).
4. Test hard: Korean IME on/off cycles, Alt+space language toggle, composition cancel, backspace during composition.

### Phase 5 — Test pass

1. `cedt_tests` — 60 tests must still be green. Any that hard-coded MBCS byte counts (e.g., "한글 = 2 bytes") need updating to code-unit counts.
2. Manual smoke test grid:
    - Load an ASCII source file → edit → save. No change on disk.
    - Load a CP949 source file → edit (add ASCII) → save. Verify still CP949.
    - Load a UTF-8 (no BOM) source file with em dash → edit → save. **Em dash survives** (the whole point).
    - Load a UTF-16 LE file → edit → save. Round-trips.
    - Open a file whose *filename* contains characters outside CP949 (e.g., Chinese hanzi via SMB share). Currently the file dialog can't even show it; after Unicode it should.
    - IME test: type 한글 sentence via IME, backspace mid-composition, save, reload, characters still there.
    - Find / Replace with 한글 search term.
    - Regex search with a Unicode pattern.
    - Multi-cursor / column select over CJK text.
    - Big file (10 MB, 100 MB) load + scroll — check no regression from doubled buffer size.

### Phase 6 — UI resources + filesystem

1. `.rc` files (`cedt_kr.rc`, `cedt_us.rc`) — the `LANGUAGE` directives stay; strings encoded as-is in the RC file are already correctly interpreted by `LoadString` in Unicode mode as long as `CODEPAGE_UTF8` is not misapplied. Verify.
2. File dialog wrappers — verify the `TCHAR`-based path types survive round-trip.
3. Registry keys — the settings we own live at `HKLM\Software\Crimson System\Crimson Editor` and `HKCU\Software\Crimson System\Crimson Editor\...`. `RegSetValueExW` accepts `REG_SZ` as UTF-16 natively; the string routing macros handle this automatically.
4. Command line: `_tmain` / `CommandLineToArgvW` — verify the launcher's argument passing handles wide correctly (`tools/launch/launch.exe` might need a small fix).
5. Shell integration: `ShellExt.dll` is already Unicode. Sanity-check the interop.
6. Clipboard: `CF_UNICODETEXT` — should Just Work once the app is Unicode.

### Phase 7 — Docs + release

1. Update this planning doc with what actually happened, per phase.
2. `README.md` — flip the roadmap checkbox `[x] Unicode build`, add a short note in "Compatibility" that filenames and file contents now support the full Unicode range.
3. `docs/configuration.md` — reflect the config-format bump (magic strings changed, old configs auto-reset once on first launch).
4. Add a note in the release entry about what's expected to change for a Korean user: previously-broken characters (em dash, thin dash, quote marks, math symbols) will now display and round-trip.
5. Version bump 3.83 → 3.90.
6. Merge back to `main`, tag `v3.90`, cut the installer.

---

## Risks

| Risk | Mitigation |
| --- | --- |
| Missed string literal in Phase 2 → runtime "?" or clipped strings | Rely on compile-time errors for most cases (the `TCHAR*` mismatch shows up as a hard type error). Complement with a grep sweep for suspicious patterns (`"..."` inside macros that receive `LPCTSTR`). |
| IME regression under Korean composition | The hardest to catch statically. Reserve real hands-on testing at Phase 4. Compare-recording video of "before" vs "after" IME behavior on the same input. |
| File loaded in encoding X, saved in encoding Y silently, because the encoding detector or save path has a bug specific to the new Unicode buffer type | Automated round-trip tests: for each encoding, load a canned file, save to `tmp`, byte-diff against source. Any diff is a regression. |
| The bundled `HtmlHelp.h`'s `HtmlHelp()` prototype takes `LPCSTR` even in Unicode builds (it's a very old header) | Wrap the call in a `#ifdef _UNICODE` bridge that converts the URL string. One-line fix. |
| Old `cedt.conf` from 3.83 → 3.90 upgrade fails badly instead of quietly resetting | Bump `STRING_CONFIGURATIONVER` to `"Configuration 3.90 x64 Unicode"` or similar. Load path already treats mismatch as "start clean" — verified during x64 work. |
| Someone's project file references filename encoded in CP949 that's not representable outside it | Would need a per-project migration. Given the small user base and rarity of the failure mode, ship a release note pointing to a workaround (re-open the file by hand). |
| `g_bDoubleByteCharacterSet` used somewhere non-obvious (e.g., a syntax highlighting rule) | grep every reference before deleting the global. Keep as `#define g_bDoubleByteCharacterSet FALSE` shim if in doubt. |
| Performance regression on large files due to doubled buffer size | 64-bit x64 process address space makes this practically irrelevant. Confirmed once during test pass. |

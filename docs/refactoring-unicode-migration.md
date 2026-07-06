# MBCS → Unicode Migration

Working notes for the `feat/unicode-migration` branch — turning Crimson Editor from an MFC MBCS build (CP949 on a Korean host, CP1252 on an English host) into a native Unicode (UTF-16 internal) build.

The end state of this branch is: every string CString/API touches is UTF-16, the two editions still ship as `cedt_kr.exe` / `cedt_us.exe` but their input path can handle *any* Unicode character (em dash, arrows, CJK extensions, filenames with characters outside CP949, etc.). Files on disk keep their existing encoding — the load/save conversion just no longer downgrades through the ANSI code page.

---

## Status board

Done / verified are the phases below where every task is checked. WIP has the concrete work remaining. Skip the plan tables when reading; the phase sections below have the actual detail.

### Done (compiles, ships, runtime-verified)

| Phase | Commit | Outcome |
| --- | --- | --- |
| 0 — Environment check | `0a3bb12` | MFC Unicode libs / HtmlHelp.h dual A/W verified |
| 1 — Build system flip | `0054644` | 2,521 first-compile errors |
| 2a — Bulk mechanical fixes | `202cee2` | 22 hotspot files → 653 errors |
| 2b step 1 — RegExp + TRACE self-wrap | `73e6748` | → 369 errors |
| 2b step 2 — `#define` literals | `ff1056a` | → 288 errors |
| 2b step 3 — compile clean | `038c6c7` | **0 errors**, cedt_tests 55/60 |
| 3.1 — CRegExp bytecode fix | `c5f045b` | `OPERAND(p) = p + 3`, cedt_tests **60/60** |
| 3.2 — File I/O: drop CP_ACP round-trip | `c5f045b` | em dash / smart quotes / CJK survive load |
| 3.3 — em dash rendering | verified in-session | UTF-8 (BOM) load + save-as round-trip, screenshot |
| 3.4 — IME composition | `d004fb8` | `ImmGetCompositionString` wide buffer + DBCS branches `#ifdef`-guarded; 한글 IME 입력 정상 확인 |
| 3.5 — Config compat + wchar_t builtin | `d004fb8` | Magic strings 3.80 → 3.90, `TreatWChar_tAsBuiltInType=true`, `_WRITE_WIDE_STR` / `_READ_WIDE_STR` helpers across every `StreamSave`/`StreamLoad` in `cedtElement.cpp` |
| 3.6 — Project file XML round-trip | verified in-session | `test.prj` saved and reloaded as pure XML text; no more `wofstream << CString → pointer address` |
| 3.7 — Warnings clean + GetHotKeyText UB | `f050cff` | 18 C4244 warnings → 0 (DBCS branches `#ifdef`-guarded); `TCHAR szKeyName[1024]` uninit stack bug fixed in `GetHotKeyText` for both `CUserCommand` and `CMacroBuffer` — Tools/Macros menu items now render cleanly |
| 3.8 — Version bump | `912a117` | `STRING_PROJECTFILEVER` / installer.iss / `.rc` files / README all 3.83 → 3.90 |

### WIP (not yet done)

| Phase | Remaining task | Where |
| --- | --- | --- |
| 3.8 — Release 3.90 | installer built (`dist/cedt-390-setup.exe`, 25.8 MB) but **not yet installed / smoke-tested** on a clean box. `main` merge, `v3.90` tag, GitHub Release entry all pending user go-ahead. | `scripts/build_installer.ps1` output; release script TBD |
| 4 — Extended smoke grid | most of the "manual smoke test grid" from the old Phase 5 plan hasn't been walked through end to end. See checklist below. | manual |
| 5 — UI resource + filesystem verification | `.rc` LoadString round-trip, `launch.exe` wide arg passing, ShellExt.dll interop, CF_UNICODETEXT clipboard round-trip — none touched yet | manual + code |
| 6 — Documentation | `docs/configuration.md` still describes 3.83-era config; README roadmap checkbox not flipped; no release-note entry | `docs/`, `README.md` |

### Extended smoke grid (still to walk)

Everything on this list is *not* yet run against the 3.90 build. Some items are trivially covered by earlier phase verification (marked ✓); the rest is real hands-on work.

- ✓ Load UTF-8 (with BOM) file with em dash → edit → save → reload
- [ ] Load ASCII source file → edit → save → byte-diff against original (no change on disk)
- [ ] Load CP949 source file → add ASCII text → save → verify still CP949
- [ ] Load UTF-8 (no BOM) file with em dash → edit → save → em dash still there
- [ ] Load UTF-16 LE file → edit → save → round-trips as UTF-16 LE
- [ ] Filename with characters outside CP949 (Chinese hanzi via SMB, "測試.txt") — file dialog can show and open
- ✓ IME 한글 입력 (compose, backspace mid-composition, commit)
- [ ] Find / Replace with 한글 search term
- [ ] Regex search with Unicode-only pattern (e.g. `[぀-ゟ]+`)
- [ ] Multi-cursor / column select over CJK text
- [ ] Large file (10 MB, 100 MB) load + scroll — verify no regression from doubled `wchar_t` buffer size
- [ ] Clipboard round-trip: copy CJK from another Unicode app (VS Code, Notepad), paste into Crimson, save
- [ ] Command-line launch: `cedt_kr.exe "path with 한글 characters.txt"` — file loads with the right encoding
- [ ] Reopen last workspace at startup, project with paths outside CP949
- [ ] `launch.exe` / shell-execute tool with wide-char argument
- [ ] Right-click "Edit with Crimson Editor" (ShellExt.dll) on a CJK-named file

### Bugs found during the migration (annotated)

1. **CRegExp: `OPERAND(p) = (TCHAR*)((short*)(p+1)+1)`** resolves to `p+3` under MBCS but `p+2` under `_UNICODE` — `regnode()` always reserves 3 TCHAR slots so every operand read landed one TCHAR early. Fix: `OPERAND(p) = p + 3` for both widths. This one line was the entire regex breakage (5 of 5 failing tests were regex).
2. **File I/O CP_ACP round-trip** — the pre-Unicode UTF-8 load path was `UTF-8 → wide → CP_ACP → narrow → CString`. The CP_ACP step silently mapped every non-ANSI code point to `?`. Rewrote every encoding path in `cedtElement.cpp::FileLoad` / `FileSave` to append `(LPCWSTR)szWideBuffer` directly.
3. **`ImmGetCompositionString` with narrow buffer** — under `_UNICODE`, this API is the W variant and writes UTF-16 into the given buffer, returning the length in bytes. The pre-Unicode call used a `CHAR buf[1024]` and assigned it to a wide `CString`, which routed through CP_ACP and mangled every Korean composition.
4. **StreamSave / StreamLoad width bug** — `nLength = m_szXxx.GetLength()` is a *character* count, but `fout.write(ptr, nLength)` writes *bytes*. Under Unicode every stored string was chopped in half. Wrapped every site in `cedtElement.cpp` with `_WRITE_WIDE_STR` / `_READ_WIDE_STR` helpers that multiply by `sizeof(TCHAR)` at the byte boundary.
5. **`wofstream << CString` writing pointer addresses** — with `TreatWChar_tAsBuiltInType=false`, `wchar_t` is `unsigned short`, and the standard `basic_ostream<wchar_t>::operator<<(const wchar_t*)` overload doesn't match the CString → LPCTSTR conversion — the compiler falls back to `operator<<(const void*)` and prints the pointer. Fix: flip the setting to `true` project-wide, then add explicit `.GetString()` at every `<<` site to be robust regardless.
6. **Config file magic string read/write with `_tcslen`** — `_tcslen("Configuration 3.90 x64")` is 22 (chars), but the buffer was read as bytes, so the compare always failed and the user got a "config file corrupted" popup on every start. Rewrote every magic-string site (`cedtAppConf.cpp`) to use `CStringA` — one byte per char on disk, matches Save side that was already narrow.
7. **`GetHotKeyText` uninitialized-stack UB** — `TCHAR szKeyName[1024]` declared but never initialized; `GetKeyNameText` returns 0 for the default (`m_wVirtualKeyCode = 0`) case without touching the buffer, and the code then trusted `_tcslen(szKeyName)` as the "did it work?" check. The pre-Unicode build happened to see zero-filled stack on this path; Unicode doesn't, so every "Empty" Tools/Macros menu row rendered `- Empty -\t<garbage wchars>`. Fix: check the API return value; on `<= 0`, return empty.

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

## Original plan (kept for reference)

The following is the plan as written at branch start. It's kept verbatim so a future reader can compare the plan to what actually happened. See the *Status board* above for the real state.

### Environment check (Phase 0 — done)

| Component | Status |
| --- | --- |
| MSVC v145 x64 toolset (`cl.exe Hostx64\x64`) | ✅ already used for x64 build at `VS\18\Professional\VC\Tools\MSVC\14.51.36231` |
| MFC v145 x64 Unicode dynamic libraries | ✅ `mfc140u.lib` (9.1 MB release) + `mfc140ud.lib` (10.4 MB debug) present under `atlmfc\lib\x64` |
| MFC v145 x64 Unicode support libraries | ✅ `mfcs140u.lib` / `mfcs140ud.lib` + `MFCM140U.lib` / `MFCM140Ud.lib` present |
| Windows SDK 10 — Unicode `Imm32.lib` | ✅ standard, same lib for both A/W |
| Bundled `HtmlHelp.h` (1999-vintage) | ✅ ships **both** `HtmlHelpA` and `HtmlHelpW` prototypes with a `#ifdef UNICODE` macro routing to the right one. No wrapper needed. Structure fields are already `LPCTSTR` throughout (one stray `LPCSTR pszCatName` in the info-type-enum struct, which we don't use) |
| Test project (`cedt_tests`) built with GoogleTest through vcpkg | ✅ vcpkg manifest tolerant of either charset, gtest itself doesn't care |

### Phase 1 — Build-system flip + first compile attempt

1. `cedt.vcxproj`:
    - `<CharacterSet>MultiByte</CharacterSet>` → `Unicode` in both `Release-KR|Release-US` and `Debug-KR|Debug-US` Configuration groups.
    - `<PreprocessorDefinitions>` — `_MBCS` → `_UNICODE;UNICODE` in both Release and Debug ItemDefinitionGroups.
2. `tests/cedt_tests.vcxproj` — same treatment.
3. `tools/launch/launch.vcxproj` — same treatment. (`tools/shellext/shellext.vcxproj` was already Unicode.)

**First Debug-KR|x64 rebuild produced 2,521 errors** — as expected. Distribution:

| Code | Count | Meaning | Fix category |
| --- | --- | --- | --- |
| `C2664` | 2,133 (85%) | function argument `char*` ↔ `TCHAR*` mismatch | wrap literal with `_T("...")`, retype local buffer |
| `C2665` | 309 (12%) | overloaded function had no viable `TCHAR*` overload for a `char*` arg | same fix pattern as C2664 |
| `C2440` | 55 (2%) | plain `=` assignment `char*` ↔ `TCHAR*` | retype variable, or wrap literal |
| `C2678` | 20 (<1%) | `std::istream::operator>>` on a `char` when the sink is now `wchar_t` | use `std::wistringstream` / `std::wifstream` (or bridge through a narrow string on the IO side) |
| `C2660` | 1 | `MultiByteToWideChar` called with 5 args instead of 6 | manual fix in `XPTabCtrl.cpp:422` |
| `C1003` | 3 | "too many errors in one file, stopping" | not real errors — the actual count is larger than 2,521 |

### Phase 2 — Mechanical fixes (bulk-scriptable)

1. **String literals**: `"..."` used in `TCHAR`-taking context → wrap with `_T("...")`. Don't blindly wrap every literal — a literal passed to a socket API stays `char`.
2. **Explicit char types**: `char szBuf[N]` used with `TCHAR`-family APIs → `TCHAR szBuf[N]`.
3. **String function calls**: `strlen`→`_tcslen`, `strcpy`→`_tcscpy`, `strcmp`→`_tcscmp`, `sprintf`→`_stprintf`, `atoi`→`_ttoi`, and friends.
4. **Char literals passed to APIs**: `'A'` sometimes → `_T('A')` when compared against `TCHAR`.

### Phase 3 — File I/O logic (the actual behavior change)

The heart of the "em dash disappears" bug lives here. Currently `src/core/cedtElement.cpp` UTF-8 load path does:

```cpp
MultiByteToWideChar(CP_UTF8, 0, szBuffer, -1, szWideBuffer, ...);   // step 1 — UTF-8 → UTF-16, GOOD
WideCharToMultiByte(CP_ACP,  0, szWideBuffer, -1, szBuffer, ...);   // step 2 — UTF-16 → CP949, DATA LOSS
```

Step 2 exists because the document's in-memory buffer is `char*`. Once the app is `_UNICODE`, the in-memory buffer is `wchar_t*` and step 2 goes away entirely — the file's UTF-8 bytes reach the buffer as their original code points and no CP949 downgrade happens.

### Phase 4 — IME / character input

- `WM_IME_CHAR` fires once per full character (UTF-16 code unit), not once per byte.
- `ImmGetCompositionStringA` → `ImmGetCompositionStringW` — the `A`/`W` selection is via the same `TCHAR` macro that `_UNICODE` flips.
- `IsDBCSLeadByte` — no longer needed anywhere in the code path.
- `g_bDoubleByteCharacterSet` global — retire under `_UNICODE` (force FALSE).

### Phase 5 — Test pass

1. `cedt_tests` — 60 tests must still be green.
2. Manual smoke test grid — see the *Extended smoke grid* checklist in the Status board.

### Phase 6 — UI resources + filesystem

1. `.rc` files — verify `LoadString` round-trip.
2. File dialog wrappers — verify wide path round-trip.
3. Registry — `RegSetValueExW` accepts `REG_SZ` as UTF-16 natively.
4. Command line: `_tmain` / `CommandLineToArgvW` — verify `launch.exe` argument passing.
5. Shell integration: `ShellExt.dll` interop sanity check.
6. Clipboard: `CF_UNICODETEXT` round-trip.

### Phase 7 — Docs + release

1. Update this planning doc with what actually happened.
2. `README.md` — flip the roadmap checkbox `[x] Unicode build`.
3. `docs/configuration.md` — reflect the config-format bump.
4. Add a release-note entry for Korean users about what previously-broken characters now work.
5. Version bump 3.83 → 3.90.
6. Merge back to `main`, tag `v3.90`, cut the installer.

---

## Risks (as identified at branch start)

| Risk | Mitigation |
| --- | --- |
| Missed string literal in Phase 2 → runtime "?" or clipped strings | Rely on compile-time errors for most cases (the `TCHAR*` mismatch shows up as a hard type error). Complement with a grep sweep for suspicious patterns (`"..."` inside macros that receive `LPCTSTR`). |
| IME regression under Korean composition | The hardest to catch statically. Reserve real hands-on testing at Phase 4. Compare-recording video of "before" vs "after" IME behavior on the same input. |
| File loaded in encoding X, saved in encoding Y silently, because the encoding detector or save path has a bug specific to the new Unicode buffer type | Automated round-trip tests: for each encoding, load a canned file, save to `tmp`, byte-diff against source. Any diff is a regression. |
| The bundled `HtmlHelp.h`'s `HtmlHelp()` prototype takes `LPCSTR` even in Unicode builds (it's a very old header) | Wrap the call in a `#ifdef _UNICODE` bridge that converts the URL string. One-line fix. |
| Old `cedt.conf` from 3.83 → 3.90 upgrade fails badly instead of quietly resetting | Bump `STRING_CONFIGURATIONVER` to `"Configuration 3.90 x64"`. Load path already treats mismatch as "start clean" — verified during x64 work. |
| Someone's project file references filename encoded in CP949 that's not representable outside it | Would need a per-project migration. Given the small user base and rarity of the failure mode, ship a release note pointing to a workaround (re-open the file by hand). |
| `g_bDoubleByteCharacterSet` used somewhere non-obvious (e.g., a syntax highlighting rule) | grep every reference before deleting the global. Keep as `#define g_bDoubleByteCharacterSet FALSE` shim if in doubt. |
| Performance regression on large files due to doubled buffer size | 64-bit x64 process address space makes this practically irrelevant. Confirmed once during test pass. |

# 32-bit → 64-bit Migration

Working notes for the `feat/64bit-port` branch — turning Crimson Editor from a Win32 (x86) MFC application into a native x64 build.

The end state of this branch is: every shipped binary (`cedt_kr.exe`, `cedt_us.exe`, `launch.exe`) is 64-bit, the Win32 (x86) build is removed from the solution, and the installer ships an `vc_redist.x64.exe` instead of `vc_redist.x86.exe`. `ShellExt.dll` is already x64 since the installer work, so it just stays the same.

---

## Decisions (locked in at branch start)

| | Decision | Rationale |
| --- | --- | --- |
| Branch name | `feat/64bit-port` | follows the `feat/` convention; specific to this migration |
| 32-bit build | **drop entirely** | every shipped artifact ends up x64. Win32 lives on `main` and in the git history; no business reason to dual-ship |
| User settings compatibility | **bump `STRING_CONFIGURATIONVER` (and friends), let 32-bit configs reset** | a binary `cedt.conf` written by the Win32 build is not guaranteed to be byte-compatible on x64 — any `size_t` / `INT_PTR` / `LPARAM` field changes width. Writing a migration path is real work for very little gain; reset-with-explanation is the honest move |

---

## Environment check (Phase 0 — done)

| Component | Status |
| --- | --- |
| MSVC v145 x64 toolset (`cl.exe Hostx64\x64`) | ✅ present at `VS\18\Professional\VC\Tools\MSVC\14.51.36231` |
| MFC x64 libraries (12 `.lib` files under `atlmfc\lib\x64`) | ✅ present |
| Windows SDK `HtmlHelp.Lib` x64 | ✅ `Windows Kits\10\Lib\10.0.26100.0\um\x64\HtmlHelp.Lib` |
| Bundled `third_party\htmlhelp\HtmlHelp.lib` | ⚠️ 32-bit only (1999 vintage), can't use for x64 — switch to Windows SDK lib for x64 builds |

---

## Phase plan

### Phase 1 — Build system + first compile attempt

1. Add `x64` platform to `cedt.vcxproj` (Debug-KR / Release-KR / Debug-US / Release-US, four configurations × x64).
2. Update `cedt.sln` to map the existing solution configurations to `|x64`.
3. Switch the `HtmlHelp.lib` reference: keep the bundled 32-bit lib for Win32 (still in tree for now), but for x64 link against the Windows SDK's `HtmlHelp.Lib` directly.
4. Add `x64` platform to `tests/cedt_tests.vcxproj` and `tools/launch/launch.vcxproj` the same way. (`tools/shellext/shellext.vcxproj` is already x64.)
5. First build attempt — expect a bucket of warnings and possibly errors. Collect them.

### Phase 2 — Code fixes

Likely categories, in order of expected impact:

- **`size_t` vs `int` truncation** — 20+ years of MFC code with `int nLength = strlen(...)`. v145 will warn (C4267 / C4244). Mostly mechanical.
- **Pointer cast truncation** — `(DWORD)pSomething` patterns that worked on Win32 because `DWORD` and pointer are both 32-bit. On x64 this loses the upper 32 bits. Replace with `DWORD_PTR` / `INT_PTR`. (C4311 / C4312.)
- **`LRESULT` / `WPARAM` / `LPARAM` usage** — these went from `LONG` to pointer-sized. Any code that stored them in `LONG` / `DWORD` is broken on x64.
- **MFC handler signatures** — already updated for v145 on `main`, so probably nothing new here, but worth a clean recompile.
- **Anything in `_msize` / pointer arithmetic** — code that assumes `sizeof(void*) == 4`.

### Phase 3 — cedt_tests x64

The test project is the smaller surface and exercises the same util code (`encode`, `evaluate`, `RegExp`, `PathName`, ...). Bringing it green on x64 first is the cheapest correctness signal.

Target: 60/60 still passing on x64 Debug.

### Phase 4 — Boot + smoke test

Drive `cedt_kr.exe` x64 manually:

- File open / save (encoding, EOL modes — DOS/UNIX/MAC)
- IME composition (CP949 Korean input — the most x64-suspicious feature, given how much pointer juggling lives in `cedtViewEditCompose.cpp`)
- FTP open + login (network code path)
- User Tool execution + capture output
- Macro record / replay
- Find in Files
- Preferences dialog OK (saves all settings — exercises the `cedt.conf` binary write path)

### Phase 5 — Config compatibility + Win32 drop

Once x64 is stable:

- Bump the format-version magic headers in `src/include/cedtHeader.h` (`STRING_CONFIGURATIONVER`, `STRING_COLORSETTINGSVER`, `STRING_FTPACCOUNTVER`) by appending `" x64"` or similar — so a Win32-written `cedt.conf` is correctly rejected on x64 and falls through to defaults, rather than reading garbage off a width mismatch.
- Remove Win32 platform entries from `cedt.vcxproj`, `tests/cedt_tests.vcxproj`, `tools/launch/launch.vcxproj`, and the solution configurations in `cedt.sln`.
- Remove the bundled `third_party/htmlhelp/HtmlHelp.lib` since nothing references it anymore.
- README: replace the "Win32 / x86" line in the Build Environment table with "x64".

### Phase 6 — Installer

- `installer.iss`:
  - `ArchitecturesAllowed = x64compatible` (drop `x86` from the allowed list).
  - VC++ redist swap: `vc_redist.x86.exe` → `vc_redist.x64.exe` in `[Files]` and `[Run]`.
  - The `64bit` flag on `ShellExt.dll`'s `[Files]` entry can come off — it's no longer "the one 64-bit thing in a 32-bit installer".
- `build_installer.ps1`:
  - cedt builds: `Win32` → `x64`.
  - `launch.exe` build: `Win32` → `x64`.
  - Redist download URL: `aka.ms/vs/17/release/vc_redist.x86.exe` → `aka.ms/vs/17/release/vc_redist.x64.exe`.

### Phase 7 — Verification + merge

- Clean Windows Sandbox install of the x64 setup.exe (or VM if Sandbox isn't available on the host).
- Walk through the install / first-run / shell-extension / uninstall sequence to make sure nothing regressed from the x86 build.
- Open a PR from `feat/64bit-port` → `main`. Resolve any review comments. Merge.

---

## Risks / things to keep in mind

- **MFC MBCS on x64** is still supported by v145 but is being de-emphasized by Microsoft. The Korean IME path (`cedtViewEditCompose.cpp`) is the most likely place to hit a subtle x64 issue.
- **Binary settings file format** — already discussed; reset is the chosen mitigation.
- **Third-party plugins / external tools** — Crimson Editor exposes no plugin ABI, so no third-party x86 DLLs would be loaded into the cedt process. Safe.
- **`launch.exe` 32-bit could in theory stay** (a 32-bit launcher can still spawn 64-bit children just fine), but mixed-bit is confusing and `launch.exe` is 30 lines of code — converting it costs nothing.
- **Bundled `HtmlHelp.lib` from 1999** — it's tiny, has no `/SAFESEH`, and only exists because the Windows SDK 6 / 7 era of MFC needed something. Switching to the modern Windows SDK's `HtmlHelp.Lib` for x64 also lets us drop the Release `ImageHasSafeExceptionHandlers=false` workaround for x64.

---

## Status

- [x] Phase 0 — environment check
- [ ] Phase 1 — build system + first compile attempt
- [ ] Phase 2 — code fixes
- [ ] Phase 3 — cedt_tests x64 60/60
- [ ] Phase 4 — cedt_kr.exe x64 smoke test
- [ ] Phase 5 — config magic bump + Win32 drop
- [ ] Phase 6 — installer x64 swap
- [ ] Phase 7 — verification + merge

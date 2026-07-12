# Crimson Editor

A freeware source-code editor for Windows, originally written between 1999 and 2005 and modernized for 64-bit Windows in 2026.

![Crimson Editor screenshot](docs/screenshot.png)

---

## Download

The current release ships an x64 installer for Windows 10 and 11.

- **[Download cedt-392-setup.exe](https://github.com/igkang00/CrimsonEditor/releases/latest)** (≈ 26 MB) — from the GitHub Releases page

Run the installer and accept the UAC prompt. It installs to `Program Files\Crimson Editor`, bundles the Visual C++ x64 runtime, and optionally adds an "Edit with Crimson Editor" entry to the Explorer right-click menu.

> **Heads-up**: Crimson Editor stores all per-user state under `HKCU\Software\Crimson System\Crimson Editor` and `%APPDATA%\Crimson Editor\`. Uninstalling does **not** touch those, so reinstalling later picks back up where you left off.

---

## Features

- **Source-aware editing** — syntax highlighting for ~50 languages, configurable color schemes, word autocompletion from a dictionary
- **Multi-document interface (MDI)** — file tabs along the top, split views, drag-and-drop between tabs
- **Project workspace** — group related files and folders, switch between projects from the side panel
- **Find / Replace** — including regex and Find-in-Files across a folder tree
- **Integrated FTP** — open and save remote files directly, browse a remote tree
- **User-defined tools and macros** — wire external commands (compilers, formatters, scripts) into menus and shortcut keys, with output captured into a docked console
- **Korean / English editions** — both shipped in the single installer; pick at install time
- **Lightweight** — single executable, no managed runtime, < 2 MB Release binary

For the older feature documentation, see [docs/](docs/) and the bundled help in the install directory.

---

## Building from Source

Targets **Visual Studio 2026** (v145 toolset), MFC dynamic, x64.

```powershell
# Build all 4 configurations
msbuild cedt.sln /p:Configuration=Release-KR /p:Platform=x64
msbuild cedt.sln /p:Configuration=Release-US /p:Platform=x64
msbuild cedt.sln /p:Configuration=Debug-KR   /p:Platform=x64
msbuild cedt.sln /p:Configuration=Debug-US   /p:Platform=x64

# Or build the installer end-to-end (compiles binaries, downloads VC++ x64
# redist, runs Inno Setup):
.\scripts\build_installer.ps1
```

Build artifacts land in `build\x64\<Configuration>\` (e.g. `build\x64\Release-KR\cedt_kr.exe`); the installer lands in `dist\cedt-392-setup.exe`. Both `build\` and `dist\` are gitignored.

### Prerequisites

| | |
| --- | --- |
| **IDE** | Visual Studio 2026 (18.x) |
| **Toolset** | `v145` |
| **MFC component** | **C++ MFC for latest v145 build tools (x86 & x64)** — *not* part of the default VS install since VS 2017; add it from the **Individual components** tab. The MBCS libraries are bundled into this component on modern VS. |
| **Inno Setup 6** | only needed to build the installer — get it from <https://jrsoftware.org/isdl.php> |
| **vcpkg** | only needed to build/run the unit tests — `gtest` is declared as a manifest dependency in [tests/vcpkg.json](tests/vcpkg.json) |

The legacy VC6 build files (`.dsw` / `.dsp` / `.mak` / `.clw`) are preserved on the `cedt371-original` branch.

---

## Tests

```powershell
msbuild tests\cedt_tests.vcxproj /p:Configuration=Debug /p:Platform=x64
tests\build\x64\Debug\cedt_tests.exe
```

Current coverage: **60 tests across 12 suites, all green** — algorithm modules in `src/util/` (RegExp, evaluate, date, encode, PathName) and the MFC-data-container classes in `src/core/cedtElement.cpp` plus `src/util/SortStringArray`.

See [docs/testing.md](docs/testing.md) for the per-module test list, project settings, how to add a new test, and the planned roadmap for integration (L2) and end-to-end (L3) coverage.

---

## Project Structure

```
CrimsonEditor/
├── src/             # All cedt source: include / core / app / doc / view / frame /
│                    # panels / dialogs / network / util
├── tests/           # Google Test project (cedt_tests)
├── tools/           # launch.exe, ShellExt.dll — shipped with the installer
├── res/             # Icons, bitmaps, cursors, manifest
├── runtime/         # Files installed alongside cedt.exe (dictionaries, syntax specs,
│                    # color schemes, templates, docs)
├── scripts/         # Build automation (build_installer.ps1)
├── docs/            # Long-form design / behavior / refactoring notes
├── third_party/     # Bundled SDK headers (HtmlHelp.h)
├── cedt.sln, cedt.vcxproj    # Visual Studio 2026 build system
├── cedt_kr.rc, cedt_us.rc    # Resource files for KR / US editions
└── installer.iss             # Inno Setup installer script
```

For the full source breakdown — every `.cpp` and what it does, the MFC class diagram, and how shell integration is wired up — see **[docs/source-layout.md](docs/source-layout.md)**.

---

## Documentation

| | |
| --- | --- |
| [docs/source-layout.md](docs/source-layout.md) | Architecture overview, full source-file tour, shell integration internals |
| [docs/configuration.md](docs/configuration.md) | How settings are loaded at startup, where each piece of state lives, what happens on a clean first run |
| [docs/testing.md](docs/testing.md) | Test project layout, how to add a test, roadmap toward integration / E2E coverage |
| [docs/refactoring-memory-safety.md](docs/refactoring-memory-safety.md) | Memory-safety review with severities and a recommended fix order |
| [docs/refactoring-x64-migration.md](docs/refactoring-x64-migration.md) | Planning doc for the Win32 → x64 migration done in v3.81 |

---

## Roadmap

- [x] **VS 2026 migration** — `.dsp`/`.dsw` → `.vcxproj`/`.sln`, v145 toolset, MFC Dynamic + MBCS (originals preserved on `cedt371-original`)
- [x] **Source tree cleanup** — the previously flat ~180-file tree is now split into `src/{include,core,app,doc,view,frame,panels,dialogs,util,network}`
- [x] **64-bit migration** — shipped in v3.81
- [x] **Inno Setup installer** — replaces the legacy NSIS installer, bundles VC++ x64 redist
- [x] **Unit tests for the "medium" group** — `CSortStringArray`, `CMemText`, `CUndoBuffer`, `CKeywords`, `CDictionary`, `CLangSpec`, `CAnalyzedString`
- [x] **Unicode build** — shipped in v3.90. UTF-16 internally; the on-disk encoding is preserved (UTF-8, UTF-16 LE/BE, CP949/CP1252). Characters and filenames outside the ANSI code page work, and the Korean IME was rewritten for the wide-char path. See [docs/refactoring-unicode-migration.md](docs/refactoring-unicode-migration.md)
- [x] **Surrogate pairs (emoji, CJK Ext-B)** — characters above U+FFFF used to split in half under the caret and under Backspace. They are now one character everywhere. See [docs/refactoring-surrogate-pairs.md](docs/refactoring-surrogate-pairs.md)
- [x] **Large-file loading** — shipped in v3.91. A 100 MB / 900 k-line file took 28 s to open and 13 s to save; now **2.2 s** and **0.14 s**. Lazy row layout, block file I/O, table-driven analyzer, per-character width cache. See [docs/refactoring-large-file-perf.md](docs/refactoring-large-file-perf.md)
- [x] **Line container** — shipped in v3.92. The lines moved from a linked list to an array, so finding the line under the caret no longer walks the document: at line 900,000, **7–10 ms → 0.2 µs**. See [docs/refactoring-line-container.md](docs/refactoring-line-container.md)
- [ ] **Color emoji rendering (DirectWrite)** — emoji render monochrome: GDI `TextOut` predates colour fonts and ignores the COLR/CPAL layers. Moving the draw and measure paths to DirectWrite would also bring proper grapheme clusters (skin tones, ZWJ sequences) and shaping for complex scripts. Large project — it replaces the whole text rendering and measurement layer
- [ ] **Column (block) mode in the Unicode era** — the cell math assumes `1 index == 1 character cell`, so the selection rectangle looks ragged around wide CJK. Text cannot be corrupted; the display-column model needs replacing. See [docs/refactoring-column-mode.md](docs/refactoring-column-mode.md)
- [ ] **High-DPI awareness** — ships DPI-unaware on purpose (see *Known issues*); needs the legacy dialogs and toolbar modernized first
- [ ] **GitHub Actions CI** — automatically build the four configurations and run `cedt_tests` on every push
- [ ] **Integration tests (L2)** — exercise `CCedtDoc` and other CWinApp-dependent code without showing real windows. Requires extracting a `cedt_core` static library; see [docs/testing.md](docs/testing.md)
- [ ] **End-to-end UI tests (L3)** — drive `cedt_*.exe` through WinAppDriver / FlaUI / PyWinAuto / AutoHotkey

---

## Known issues

### Very large files hold the whole document in memory

Large-file loading was slow until v3.91 (a 100 MB file took ~28 s to open
and ~13 s to save); it now opens in ~2 s and saves in ~0.14 s. See
[docs/refactoring-large-file-perf.md](docs/refactoring-large-file-perf.md).

What remains is memory, not time: the document, its token analysis, and its
screen layout are all held in RAM in full, so a file well beyond 100 MB will
run the process out of address space long before it feels slow. Streaming —
holding only the viewport and a page either side — is the fix, and it is not
implemented.

### High-DPI displays (display scaling > 100%)

The app is built **DPI-unaware** on purpose — `Manifest → DPI Awareness = None`
(`<EnableDpiAwareness>false</EnableDpiAwareness>`) in [cedt.vcxproj](cedt.vcxproj).
At scaling factors other than 100%, Windows bitmap-scales the whole window
uniformly: the UI stays correctly laid out but looks slightly soft. At 100% it
is pixel-crisp.

This is a deliberate workaround. The VS 2026 linker defaults to marking the app
**System DPI-aware**, which looked crisper but **broke layout at non-100%
scaling** because the legacy UI predates high-DPI:

- **Dialogs clip.** Every dialog template uses `FONT 8, "MS Sans Serif"` — a
  bitmap font that cannot scale to fractional DPI. Control rectangles are sized
  in *dialog units* derived from that font, so at e.g. 150% the rendered text
  outgrows its controls: truncated category labels and cut-off OK/Cancel/Apply
  buttons on the Preferences dialog, clipped tab contents, etc.
- **Toolbar shrinks.** The toolbar image list is a fixed 16×16 px bitmap with no
  DPI scaling, so it stays physically tiny while the rest of the UI grows.

**Proper fix — what it takes to re-enable DPI awareness:**

1. Replace `FONT 8, "MS Sans Serif"` with a scalable UI font
   (`FONT 9, "Segoe UI"`, or `"MS Shell Dlg 2"`) in **all** dialog templates in
   `cedt_us.rc` / `cedt_kr.rc`, and convert plain `DIALOG` → `DIALOGEX`.
   Re-check each template's width/height for the new font metrics.
2. Provide DPI-scaled, multi-size **32-bit** toolbar icons chosen at runtime by
   the display DPI.
3. Only then switch `Manifest → DPI Awareness` back to `PerMonitorHighDPIAware`
   (or `High DPI Aware`) and re-test every dialog at 100 / 150 / 200 %.

---

- **Version**: 3.92
- **Copyright**: © 1999–2026 Ingyu Kang
- **License**: see [LICENSE](LICENSE)

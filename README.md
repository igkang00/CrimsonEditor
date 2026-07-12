# Crimson Editor

A freeware source-code editor for Windows, originally written between 1999 and 2005 and modernized for 64-bit Windows in 2026.

![Crimson Editor screenshot](docs/screenshot.png)

---

## Download

The current release ships an x64 installer for Windows 10 and 11.

- **[Download cedt-391-setup.exe](https://github.com/igkang00/CrimsonEditor/releases/latest)** (≈ 26 MB) — from the GitHub Releases page

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

Build artifacts land in `build\x64\<Configuration>\` (e.g. `build\x64\Release-KR\cedt_kr.exe`); the installer lands in `dist\cedt-391-setup.exe`. Both `build\` and `dist\` are gitignored.

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
- [x] **Unicode build** — shipped in v3.90. Internal representation is UTF-16, file I/O keeps the on-disk encoding (UTF-8 with or without BOM, UTF-16 LE/BE, CP949/CP1252). Characters outside the ANSI code page (em dash, smart quotes, box drawing, CJK characters not in CP949) now display and round-trip through save/load. Filenames with characters outside the ANSI code page open. Korean IME composition rewritten for the wide-char path
- [x] **Surrogate pairs (emoji, CJK Ext-B)** — characters above U+FFFF are stored as two UTF-16 code units, so the caret used to stop between the halves and Backspace deleted only one of them, leaving a lone surrogate that save then destroyed (`U+FFFD`). Astral characters are now one character for caret movement, delete, overwrite, undo, word wrap and typing. See [docs/refactoring-surrogate-pairs.md](docs/refactoring-surrogate-pairs.md)
- [x] **Large-file loading performance** — a 100 MB / 900 k-line file took **28 s** to open and **13 s** to save. Every stage was doing work proportional to the whole document when only ~50 lines are on screen. Now **2.2 s** to open and **0.14 s** to save. The screen formatter lays out only the rows that are actually drawn (`FormatScreenText` 17.4 s → 46 ms); file I/O reads and writes in 64 KiB blocks instead of seeking back after every line (`FileLoad` 3.7 s → 0.75 s, `FileSave` 13 s → 0.14 s); the syntax analyzer classifies characters from a lookup table and no longer repaints the progress bar 45 000 times (`AnalyzeText` 6.1 s → 0.55 s); and the formatter measures each *character* once instead of each *word* every time, cutting GDI `GetTextExtent` calls from 3.6 million to 1 — which is what makes **word wrap**, the one mode that cannot skip the layout, usable at that size (12.3 s → 0.66 s; opening with wrap on, 14.5 s → 2.9 s). See [docs/refactoring-large-file-perf.md](docs/refactoring-large-file-perf.md)
- [ ] **Color emoji rendering (DirectWrite)** — emoji render **monochrome**, because the editor draws text with GDI `TextOut`, an API that predates colour fonts and ignores the COLR/CPAL layers in Segoe UI Emoji. VS Code shows them in colour only because Electron/Chromium renders through DirectWrite. Moving the draw and measure paths (`cedtViewDraw.cpp`, `cedtViewFormat.cpp`) to DirectWrite / Direct2D would also give proper **grapheme clusters** — `IDWriteTextAnalyzer` reports cluster boundaries, so a skin-tone emoji (👍🏽 = two code points) or a ZWJ sequence (👨‍👩‍👧 = five) would finally behave as one character — plus correct shaping for complex scripts, replacing the `GetTextExtent` measurement the layout code leans on today. Large project: it replaces the whole text rendering and measurement layer
- [ ] **Column (block) mode in the Unicode era** — the block-mode cell math assumes `1 index == 1 average character cell`, which was already wrong for wide CJK before emoji existed. Block operations cannot corrupt text (ranges are snapped to character boundaries), but the selection rectangle can look ragged. Needs a proper display-column model. See [docs/refactoring-column-mode.md](docs/refactoring-column-mode.md)
- [ ] **High-DPI awareness** — the app currently ships **DPI-unaware** on purpose (see *Known issues* below); making it DPI-aware requires modernizing the legacy dialogs and toolbar first
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

- **Version**: 3.91
- **Copyright**: © 1999–2026 Ingyu Kang
- **License**: see [LICENSE](LICENSE)

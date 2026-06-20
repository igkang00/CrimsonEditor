# Crimson Editor

A freeware text editor for Windows, developed from 1999 to 2005. Features include syntax highlighting, a multi-document interface (MDI), a project workspace, integrated FTP, and more.

- **Version**: 3.71 (Korean)
- **Copyright**: © 1999–2005 Ingyu Kang
- **Build environment**: Visual Studio 2026 (v145 toolset), MFC (shared DLL), Win32 / x86, MBCS

---

## TODO

- [x] **Migrated to VS 2026** — `.dsp`/`.dsw` → `.vcxproj`/`.sln`, v145 toolset, MFC Dynamic + MBCS. The original VC6 build files are preserved on the `cedt371-original` branch.
- [x] **Source/header directory cleanup done** — the previously flat ~180-file tree is now split into `src/{include,core,app,doc,view,frame,panels,dialogs,util,network}`. The bundled external SDK lives under `third_party/htmlhelp/`.
- [ ] **Review Unicode build** — currently MBCS-only. The Korean input method / IME handling ([cedtViewEditCompose.cpp](src/view/cedtViewEditCompose.cpp)) and the `char`/`TCHAR` assumptions throughout the codebase need to be revisited.
- [x] **Tests for the "medium" group** — `CSortStringArray`, `CMemText`, `CUndoBuffer`, `CKeywords`, `CDictionary`, `CLangSpec`, `CAnalyzedString`. These pull in `cedtElement`/`SortStringArray`/`Utility`/`registry` from the main app.
- [ ] **GitHub Actions CI** — automatically build the four main configurations and run `cedt_tests` on every push.
- [ ] **Integration tests (L2)** — exercise `CCedtDoc` and other CWinApp-dependent code without showing real windows. Requires extracting a `cedt_core` static library so the test EXE can host its own `CWinApp` without colliding with the main app. See "Roadmap beyond unit tests" in the Tests section.
- [ ] **End-to-end UI tests (L3)** — drive `cedt_*.exe` through external automation (WinAppDriver / FlaUI / PyWinAuto / AutoHotkey). Worth picking up once GUI regressions become a recurring pain.

---

## Build Environment

[cedt.sln](cedt.sln) + [cedt.vcxproj](cedt.vcxproj) — the Visual Studio 2026 build system. The legacy VC6 build files (`.dsw`/`.dsp`/`.mak`/`.clw`) are preserved on the `cedt371-original` branch.

| Item | Value |
| --- | --- |
| IDE | Visual Studio 2026 (18.x) |
| Toolset | `v145` |
| Target | Win32 (x86) Application |
| MFC | Dynamic (`_AFXDLL` shared DLL) |
| Character set | `_MBCS` (multi-byte) |
| External libraries | `imm32.lib`, `htmlhelp.lib` |
| Bundled headers | `HtmlHelp.h`, `HtmlHelp.lib` ([third_party/htmlhelp/](third_party/htmlhelp/)) |
| Precompiled header | [src/include/StdAfx.h](src/include/StdAfx.h) / [src/include/StdAfx.cpp](src/include/StdAfx.cpp) |

### Build Configurations

Debug/Release × KR/US are split into four configurations. Each configuration compiles only one of [cedt_kr.rc](cedt_kr.rc) / [cedt_us.rc](cedt_us.rc), producing a separate Korean and English executable.

| Configuration | Artifact |
| --- | --- |
| `Debug-KR \| Win32`   | `Debug-KR\cedt_kr.exe` |
| `Release-KR \| Win32` | `Release-KR\cedt_kr.exe` |
| `Debug-US \| Win32`   | `Debug-US\cedt_us.exe` |
| `Release-US \| Win32` | `Release-US\cedt_us.exe` |

### Build Setup

In Visual Studio Installer, on the **Individual components** tab, install the following (it is not part of the default install since VS 2017):

- **C++ MFC for latest v145 build tools (x86 & x64)** — the MFC core. In modern VS the MBCS libraries are bundled into this component.

### Notes

- The bundled [HtmlHelp.lib](third_party/htmlhelp/HtmlHelp.lib) is an old SDK library that does not support `/SAFESEH`. The Release configurations set `ImageHasSafeExceptionHandlers=false`.
- Compatibility shims and fixes applied while migrating from VC6 to a modern toolset:
    - `<fstream.h>` / `<strstrea.h>` are absorbed by the [fstream_compat.h](src/include/fstream_compat.h) shim (using-declarations to expose `ofstream` and friends in the global namespace).
    - `ios::nocreate` removed (non-standard — a modern `ifstream` already fails to open if the file is missing).
    - `istrstream` → `std::istringstream`, `eatwhite()` → `>> std::ws`.
    - Loop induction variables re-declared where the code relied on VC6-era for-loop scope leakage.
    - MFC handler signatures updated: `OnActivateApp(BOOL, HTASK)` → `(BOOL, DWORD)`, `OnNcHitTest` return type `UINT` → `LRESULT`.
    - `CStringArray::GetAt()` results bound to `const CString&` (modern MFC returns const).
- Source files are stored as **UTF-8 with BOM**; `cl.exe` is invoked with `/source-charset:utf-8`. The execution charset is left at the system default so string literals are still embedded in the binary as MBCS (e.g. CP949 on a Korean Windows host), preserving the MFC MBCS runtime behaviour.

### For installer authors

At startup the app resolves its install directory in this order:

1. `HKCU\Software\Crimson System\Crimson Editor\Install Directory` (per-user, written back on first run)
2. `HKLM\Software\Crimson System\Crimson Editor` value `InstallDir` (machine-wide, intended for installers)
3. The directory of the running EXE itself (automatic fallback so unzip-and-run setups work without any setup)

Step 3 means the app boots without help, but **installers are recommended to write step 2 explicitly**. Doing so gives every user on the machine a consistent path, makes uninstallers / updaters able to locate the install without guessing, and avoids surprises if a user later runs the EXE from a different directory (e.g. a USB stick) before the per-user HKCU value is set.

See [docs/configuration.md](docs/configuration.md) for the full picture of how settings — including this install-directory lookup — are loaded and saved.

---

## Tests

Unit tests live in [tests/](tests/) as a `cedt_tests` project inside the same solution, built with **Google Test** pulled in via **vcpkg manifest mode**.

Current coverage: **60 tests across 12 suites, all green** — the algorithm modules in `src/util/` (RegExp, evaluate, date, encode, PathName) and the MFC-data-container classes in `src/core/cedtElement.cpp` plus `src/util/SortStringArray`.

Quick run:

```
msbuild tests\cedt_tests.vcxproj /p:Configuration=Debug /p:Platform=Win32
tests\Debug\cedt_tests.exe
```

See [docs/testing.md](docs/testing.md) for the per-module test list, project settings (toolset, charset, subsystem), how to add a new test, and the planned roadmap for the integration (L2) and end-to-end (L3) layers — including the rationale for extracting `cedt_core` as a **static library** (not a DLL) as the L2 prerequisite.

---

## Architecture Overview

A standard MFC **MDI Document/View** structure.

```
CCedtApp  (src/app/cedtapp.h)        ── derived from CWinApp, application entry point
   │
   ├─ CMainFrame  (src/frame/MainFrm.h)  ── CMDIFrameWnd, hosts menus/toolbar/status bar/docking panes
   │     ├─ CStatusBarEx
   │     ├─ CToolBar
   │     ├─ CMDIFileTab              ── top file-tab control
   │     ├─ CFileWindow              ── left dock: Directory/Project/Remote panels
   │     └─ COutputWindow            ── bottom dock: external tool output console
   │
   ├─ CChildFrame (src/frame/ChildFrm.h) ── CMDIChildWnd, split view (SplitterWndEx)
   │
   ├─ CCedtDoc    (src/doc/cedtDoc.h)    ── CDocument, text buffer / undo / syntax / file I/O
   │     └─ CAnalyzedText : CList<CAnalyzedString, LPCTSTR>
   │
   └─ CCedtView   (src/view/cedtView.h)  ── CView, caret / drawing / input / macros / commands
```

> Long-form notes that are too detailed for this README live under [docs/](docs/):
> - [docs/configuration.md](docs/configuration.md) — how user settings are loaded at startup, where each piece of state is stored (registry vs `%APPDATA%`), what the fallback chain does, and what happens on a clean first run.
> - [docs/testing.md](docs/testing.md) — test project layout, how to add a new test, and the roadmap toward integration and end-to-end coverage.
> - [docs/refactoring-memory-safety.md](docs/refactoring-memory-safety.md) — memory-safety review of the codebase: one confirmed crash, several likely buffer issues, ownership notes for the network code, and a Rule-of-Three gap in the domain classes — with severities and a recommended fix order.

### Core Domain Classes ([src/core/cedtElement.h](src/core/cedtElement.h))

| Class | Role |
| --- | --- |
| `CAnalyzedString` / `CAnalyzedText` | Text buffer holding per-line analysis results |
| `CFormatedString` / `CFormatedText` | Word-wrapped / formatted on-screen representation |
| `CLangSpec`, `CKeywords` | Language specification and keyword map |
| `CDictionary` | Dictionary used for word autocompletion |
| `CUndoBuffer` | Undo/redo history |
| `CUserCommand`, `CMacroBuffer` | User-defined tools and macros |
| `CSyntaxType`, `COutputPattern`, `CFileFilter` | Syntax / output parser / file filter settings |
| `CFtpAccount` | FTP connection info |
| `CMemText` | In-memory scratch text |

---

## Source File Groups

The `.cpp`/`.h` files live under [src/](src/), split into domain-specific directories. Large classes (`CCedtApp`, `CCedtDoc`, `CCedtView`, `CMainFrame`, `CFileWindow`) are spread across multiple `.cpp` files by feature area.

### 1. Application (`CCedtApp`) — [src/app/](src/app/)

| File | Role |
| --- | --- |
| [cedtapp.cpp](src/app/cedtapp.cpp), [cedtapp.h](src/app/cedtapp.h) | `InitInstance`, message map, global state |
| [cedtAppConf.cpp](src/app/cedtAppConf.cpp) | Save/load of user settings, color schemes, macros |
| [cedtAppDirectory.cpp](src/app/cedtAppDirectory.cpp) | Directory panel command handling |
| [cedtAppFile.cpp](src/app/cedtAppFile.cpp) | Document open/save/spawn, shell command handling |
| [cedtAppFilter.cpp](src/app/cedtAppFilter.cpp) | File filter refresh and callbacks |
| [cedtAppHndr.cpp](src/app/cedtAppHndr.cpp) | Menu/command handlers (font, tab, color, etc.) |
| [cedtAppProject.cpp](src/app/cedtAppProject.cpp) | Project workspace |
| [cedtAppRegistry.cpp](src/app/cedtAppRegistry.cpp) | Registry I/O (including shell integration) |
| [cedtAppSearch.cpp](src/app/cedtAppSearch.cpp) | Find In Files |
| [cedtAppView.cpp](src/app/cedtAppView.cpp) | Refresh all open views |

### 2. Document (`CCedtDoc`) — [src/doc/](src/doc/)

| File | Role |
| --- | --- |
| [cedtDoc.cpp](src/doc/cedtDoc.cpp), [cedtDoc.h](src/doc/cedtDoc.h) | Document basics / flags / remote paths |
| [cedtDocAnal.cpp](src/doc/cedtDocAnal.cpp) | Text analysis (word tokenisation) |
| [cedtDocDictionary.cpp](src/doc/cedtDocDictionary.cpp) | Autocompletion dictionary |
| [cedtDocEdit.cpp](src/doc/cedtDocEdit.cpp), [cedtDocEditAdv.cpp](src/doc/cedtDocEditAdv.cpp) | Edit operations (basic / advanced) |
| [cedtDocFile.cpp](src/doc/cedtDocFile.cpp) | Encoding (ASCII/Unicode/UTF-8), line endings (DOS/UNIX/MAC), backups |
| [cedtDocHndr.cpp](src/doc/cedtDocHndr.cpp) | Document command handlers |
| [cedtDocMap.cpp](src/doc/cedtDocMap.cpp) | Line coordinate mapping |
| [cedtDocSearch.cpp](src/doc/cedtDocSearch.cpp) | Find/replace (including RegExp) |
| [cedtDocSyntax.cpp](src/doc/cedtDocSyntax.cpp) | Apply syntax / language specification |
| [cedtDocUndo.cpp](src/doc/cedtDocUndo.cpp) | Undo/redo |
| [cedtDocView.cpp](src/doc/cedtDocView.cpp) | View synchronisation |

### 3. View (`CCedtView`) — [src/view/](src/view/)

| File | Role |
| --- | --- |
| [cedtView.cpp](src/view/cedtView.cpp), [cedtView.h](src/view/cedtView.h) | View basics / static members |
| [cedtViewAction.cpp](src/view/cedtViewAction.cpp) | User action dispatch |
| [cedtViewCaret.cpp](src/view/cedtViewCaret.cpp) | Caret position / shape |
| [cedtViewCommand.cpp](src/view/cedtViewCommand.cpp) | Running user tools, child-process I/O |
| [cedtViewDraw.cpp](src/view/cedtViewDraw.cpp) | Screen rendering |
| [cedtViewEdit.cpp](src/view/cedtViewEdit.cpp), [cedtViewEditAdv.cpp](src/view/cedtViewEditAdv.cpp) | Edit commands |
| [cedtViewEditCompose.cpp](src/view/cedtViewEditCompose.cpp) | IME composition handling |
| [cedtViewEvent.cpp](src/view/cedtViewEvent.cpp) | Keyboard / mouse events |
| [cedtViewFont.cpp](src/view/cedtViewFont.cpp) | Font metrics |
| [cedtViewFormat.cpp](src/view/cedtViewFormat.cpp) | Word wrap / formatting |
| [cedtViewHighlight.cpp](src/view/cedtViewHighlight.cpp) | Syntax highlighting |
| [cedtViewHndrEdit.cpp](src/view/cedtViewHndrEdit.cpp), [cedtViewHndrMisc.cpp](src/view/cedtViewHndrMisc.cpp) | Menu handlers |
| [cedtViewMacro.cpp](src/view/cedtViewMacro.cpp) | Macro record/replay |
| [cedtViewMap.cpp](src/view/cedtViewMap.cpp), [cedtViewMapAdv.cpp](src/view/cedtViewMapAdv.cpp) | Screen ↔ document coordinate mapping |
| [cedtViewMetric.cpp](src/view/cedtViewMetric.cpp) | Character / line metrics |
| [cedtViewMisc.cpp](src/view/cedtViewMisc.cpp) | Miscellaneous |
| [cedtViewMove.cpp](src/view/cedtViewMove.cpp) | Caret movement |
| [cedtViewPrint.cpp](src/view/cedtViewPrint.cpp) | Print / print preview |
| [cedtViewScroll.cpp](src/view/cedtViewScroll.cpp) | Scrolling |
| [cedtViewSearch.cpp](src/view/cedtViewSearch.cpp) | Find (view side) |
| [cedtViewSelect.cpp](src/view/cedtViewSelect.cpp) | Selection |
| [cedtViewUndo.cpp](src/view/cedtViewUndo.cpp) | Undo coordination on the view side |

### 4. Main Frame / Child Frame — [src/frame/](src/frame/)

| File | Role |
| --- | --- |
| [MainFrm.cpp](src/frame/MainFrm.cpp), [MainFrm.h](src/frame/MainFrm.h) | `CMDIFrameWnd`, hosts docking controls |
| [MainFrmHndr.cpp](src/frame/MainFrmHndr.cpp) | Main frame command handlers |
| [MainFrmDropTarget.cpp](src/frame/MainFrmDropTarget.cpp), [MainFrmDropTarget.h](src/frame/MainFrmDropTarget.h) | OLE Drag & Drop |
| [ChildFrm.cpp](src/frame/ChildFrm.cpp), [ChildFrm.h](src/frame/ChildFrm.h) | MDI child, splitter window |
| [SplitterWndEx.cpp](src/frame/SplitterWndEx.cpp), [SplitterWndEx.h](src/frame/SplitterWndEx.h) | Extended splitter window |
| [StatusBarEx.cpp](src/frame/StatusBarEx.cpp), [StatusBarEx.h](src/frame/StatusBarEx.h) | Status bar with progress / flash messages |

### 5. Docking Panels — [src/panels/](src/panels/)

| File | Role |
| --- | --- |
| [SizeCBar.cpp](src/panels/SizeCBar.cpp), [SizeCBar.h](src/panels/SizeCBar.h) | Resizable control bar base |
| [FileWnd.cpp](src/panels/FileWnd.cpp), [FileWnd.h](src/panels/FileWnd.h) | Left panel (Directory / Project / Remote) |
| [FileWndDirectory.cpp](src/panels/FileWndDirectory.cpp) | Directory tree |
| [FileWndProject.cpp](src/panels/FileWndProject.cpp) | Project tree |
| [FileWndRemote.cpp](src/panels/FileWndRemote.cpp) | Remote (FTP) tree |
| [FileWndDropTarget.cpp](src/panels/FileWndDropTarget.cpp) | Panel D&D target |
| [OutputWindow.cpp](src/panels/OutputWindow.cpp), [OutputWindow.h](src/panels/OutputWindow.h) | Bottom output / input console |
| [FileTab.cpp](src/panels/FileTab.cpp), [FileTab.h](src/panels/FileTab.h) | MDI file tab control |
| [FileTabDropTarget.cpp](src/panels/FileTabDropTarget.cpp) | File tab D&D |
| [XPTabCtrl.cpp](src/panels/XPTabCtrl.cpp), [XPTabCtrl.h](src/panels/XPTabCtrl.h) | XP-style tab control |

### 6. Dialogs — [src/dialogs/](src/dialogs/)

#### Preferences — [src/dialogs/preferences/](src/dialogs/preferences/)

[prefdialog.cpp](src/dialogs/preferences/prefdialog.cpp), [prefdialog.h](src/dialogs/preferences/prefdialog.h) act as the property sheet hosting these tab pages:

| File | Role |
| --- | --- |
| [PrefDialogGeneral.cpp](src/dialogs/preferences/PrefDialogGeneral.cpp) | General |
| [PrefDialogFile.cpp](src/dialogs/preferences/PrefDialogFile.cpp) | File / Encoding |
| [PrefDialogBackup.cpp](src/dialogs/preferences/PrefDialogBackup.cpp) | Backup |
| [PrefDialogDirectory.cpp](src/dialogs/preferences/PrefDialogDirectory.cpp) | Working directory |
| [PrefDialogVisual.cpp](src/dialogs/preferences/PrefDialogVisual.cpp) | Visual effects |
| [PrefDialogColors.cpp](src/dialogs/preferences/PrefDialogColors.cpp) | Colors |
| [PrefDialogFonts.cpp](src/dialogs/preferences/PrefDialogFonts.cpp) | Fonts |
| [PrefDialogSyntax.cpp](src/dialogs/preferences/PrefDialogSyntax.cpp) | Syntax |
| [PrefDialogPrint.cpp](src/dialogs/preferences/PrefDialogPrint.cpp) | Printing |
| [PrefDialogOutput.cpp](src/dialogs/preferences/PrefDialogOutput.cpp) | Output window |
| [PrefDialogTools.cpp](src/dialogs/preferences/PrefDialogTools.cpp) | User tools |
| [PrefDialogCommands.cpp](src/dialogs/preferences/PrefDialogCommands.cpp) | Commands |
| [PrefDialogMacros.cpp](src/dialogs/preferences/PrefDialogMacros.cpp) | Macros |
| [PrefDialogFilters.cpp](src/dialogs/preferences/PrefDialogFilters.cpp) | File filters |
| [PrefDialogAssoc.cpp](src/dialogs/preferences/PrefDialogAssoc.cpp) | File associations |

#### Feature Dialogs

| File | Role |
| --- | --- |
| [AboutDialog.cpp](src/dialogs/AboutDialog.cpp) | About |
| [FindDialog.cpp](src/dialogs/FindDialog.cpp), [ReplaceDialog.cpp](src/dialogs/ReplaceDialog.cpp), [AskReplaceDialog.cpp](src/dialogs/AskReplaceDialog.cpp) | Find / Replace |
| [FindInFilesDialog.cpp](src/dialogs/FindInFilesDialog.cpp) | Find in Files |
| [GoToDialog.cpp](src/dialogs/GoToDialog.cpp) | Go to line number |
| [FolderDialog.cpp](src/util/FolderDialog.cpp) | Folder picker (lives in the util group) |
| [ReloadAsDialog.cpp](src/dialogs/ReloadAsDialog.cpp) | Reload with a different encoding |
| [UserInputDialog.cpp](src/dialogs/UserInputDialog.cpp) | Input prompt for macros / commands |
| [MacroDefineDialog.cpp](src/dialogs/MacroDefineDialog.cpp) | Macro definition |
| [DocumentSummary.cpp](src/dialogs/DocumentSummary.cpp) | Document summary |
| [DummyDialog.cpp](src/dialogs/DummyDialog.cpp) | Placeholder |
| [FtpSettingsDialog.cpp](src/dialogs/FtpSettingsDialog.cpp), [FtpAdvancedDialog.cpp](src/dialogs/FtpAdvancedDialog.cpp), [FtpPasswordDialog.cpp](src/dialogs/FtpPasswordDialog.cpp), [FtpTransferDialog.cpp](src/dialogs/FtpTransferDialog.cpp), [OpenRemoteDialog.cpp](src/dialogs/OpenRemoteDialog.cpp) | FTP-related dialogs |

### 7. Network / Remote Files — [src/network/](src/network/)

| File | Role |
| --- | --- |
| [FtpClnt.cpp](src/network/FtpClnt.cpp), [FtpClnt.h](src/network/FtpClnt.h) | FTP client |
| [RemoteFile.cpp](src/network/RemoteFile.cpp), [RemoteFile.h](src/network/RemoteFile.h) | Remote file handling |

### 8. Utilities / Common — [src/util/](src/util/)

| File | Role |
| --- | --- |
| [Utility.cpp](src/util/Utility.cpp), [Utility.h](src/util/Utility.h) | Helper functions |
| [PathName.cpp](src/util/PathName.cpp), [PathName.h](src/util/PathName.h) | Path manipulation |
| [RegExp.cpp](src/util/RegExp.cpp), [RegExp.h](src/util/RegExp.h) | Regex engine ([RegExp.html](RegExp.html) bundled as docs) |
| [SortStringArray.cpp](src/util/SortStringArray.cpp) | Sorted string array |
| [ColorListBox.cpp](src/util/ColorListBox.cpp) | List box used by color combos |
| [HyperLink.cpp](src/util/HyperLink.cpp) | Clickable hyperlink static |
| [VerticalStatic.cpp](src/util/VerticalStatic.cpp) | Vertical text static |
| [Separator.cpp](src/util/Separator.cpp) | Separator control |
| [FolderDialog.cpp](src/util/FolderDialog.cpp) | Folder picker dialog |
| [CmdLine.cpp](src/util/CmdLine.cpp) | Command-line parser (and inter-instance IPC) |
| [registry.cpp](src/util/registry.cpp), [registry.h](src/util/registry.h) | Registry helpers |
| [encode.cpp](src/util/encode.cpp), [encode.h](src/util/encode.h) | Character encoding conversion |
| [date.cpp](src/util/date.cpp), [date.h](src/util/date.h) | Date / time formatting |
| [evaluate.cpp](src/util/evaluate.cpp), [evaluate.h](src/util/evaluate.h) | Expression evaluator |

### 9. Global Headers / PCH — [src/include/](src/include/)

| File | Role |
| --- | --- |
| [StdAfx.cpp](src/include/StdAfx.cpp), [StdAfx.h](src/include/StdAfx.h) | PCH (bulk-includes MFC headers) |
| [cedtHeader.h](src/include/cedtHeader.h) | Global macros / constants, bulk header includes |
| [cedtColors.h](src/include/cedtColors.h) | Color index definitions |
| [resource.h](src/include/resource.h) | Resource IDs |
| [fstream_compat.h](src/include/fstream_compat.h) | Shim for VC6 `<fstream.h>` |

### 10. Domain Core — [src/core/](src/core/)

[cedtElement.h](src/core/cedtElement.h) / [cedtElement.cpp](src/core/cedtElement.cpp) hold 50+ global constants and 13 domain classes in a single pair — the project-wide "core definitions" unit that App, Doc, and View all depend on. See the "Core Domain Classes" table above.

### 11. Resources

| File | Role |
| --- | --- |
| [cedt_kr.rc](cedt_kr.rc) | Korean resources |
| [cedt_us.rc](cedt_us.rc) | English resources |
| [res/](res/) | Icons, bitmaps, cursors, manifest, `cedt.rc2` |
| [third_party/htmlhelp/](third_party/htmlhelp/) | Bundled HTML Help SDK (`HtmlHelp.h`, `HtmlHelp.lib`) |

---

## Directory Layout (Summary)

```
CrimsonEditor/
├── cedt.sln, cedt.vcxproj, cedt.vcxproj.filters  # Visual Studio 2026
├── README.md, CLAUDE.md, LICENSE, .gitignore
├── cedt_kr.rc, cedt_us.rc                # Resources (KR/US split builds)
├── res/                                  # Icons, bitmaps, cursors, manifest, cedt.rc2
├── docs/                                 # Long-form design / behavior notes
├── third_party/
│   └── htmlhelp/                         # HtmlHelp.h, HtmlHelp.lib (bundled legacy SDK)
├── tests/                                # Google Test project (cedt_tests)
│   ├── vcpkg.json                        #   gtest dependency declaration
│   ├── cedt_tests.vcxproj                #   Test executable
│   └── <Module>_test.cpp                 #   One file per module under test
└── src/
    ├── include/                          # Global headers + PCH + compatibility shim
    │     ├── StdAfx.{h,cpp}              #   PCH
    │     ├── cedtHeader.h, cedtColors.h
    │     ├── resource.h
    │     └── fstream_compat.h            #   Shim for VC6 <fstream.h>
    ├── core/                             # Domain core (cedtElement)
    ├── app/                              # CCedtApp + cedtApp*.cpp
    ├── doc/                              # CCedtDoc + cedtDoc*.cpp
    ├── view/                             # CCedtView + cedtView*.cpp
    ├── frame/                            # MainFrm/ChildFrm/Splitter/StatusBar
    ├── panels/                           # FileWnd/FileTab/OutputWindow/SizeCBar/XPTabCtrl
    ├── dialogs/                          # Feature dialogs
    │     └── preferences/                #   prefdialog + PrefDialog* pages
    ├── network/                          # FtpClnt, RemoteFile
    └── util/                             # Utility/PathName/RegExp/encode/...
```

---

## Shell Integration / Multi-Instance

- **Single-instance IPC**: a named mutex `CrimsonEditor.CmdLine` detects the first instance, and a `WM_ANOTHER_INSTANCE` message is sent to the first window (see the `#pragma data_seg("Shared")` block near [cedtapp.cpp:22](src/app/cedtapp.cpp#L22)).
- **Shell extension**: integrates with the right-click menu via `HKCR\*\shellex\ContextMenuHandlers\Crimson Editor`.
- **IE integration**: can be registered as "View Source Editor".

The relevant constants live at the top of [src/include/cedtHeader.h](src/include/cedtHeader.h).

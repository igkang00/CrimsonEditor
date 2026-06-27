# User Configuration Loading

How Crimson Editor finds and loads user settings when it starts up, where each piece of state is stored, what happens when files are missing, and what the hardcoded defaults are.

The authoritative entry point is [`CCedtApp::InitInstance()`](../src/app/cedtapp.cpp) at `src/app/cedtapp.cpp:240`. The settings-related code lives in [src/app/cedtAppConf.cpp](../src/app/cedtAppConf.cpp) and [src/app/cedtAppRegistry.cpp](../src/app/cedtAppRegistry.cpp).

---

## 1. Startup sequence

Excerpt of `CCedtApp::InitInstance()`, in order:

```
1.  SetRegistryKey("Crimson System")                    HKCU\Software\Crimson System\Crimson Editor\*
    + override m_pszProfileName = "Crimson Editor"      (so KR and US builds share the same subtree)
2.  Resolve InstallDirectory                            (HKLM only; falls back to the EXE's own directory)
3.  Compute AppDataDirectory = %APPDATA%\Crimson Editor (auto-created if missing)
4.  LoadMultiInstancesFlag                              (registry)
5.  Single-instance check                               (IPC + ghost-exit if a previous instance is alive)
6.  AfxOleInit / InitCommonControls / Enable3dControls
7.  LoadUserConfiguration(cedt.conf)                    file
    LoadColorScheme(cedt.color)                         file
    LoadFtpAccountInfo(cedt.ftp)                        file
    LoadUserCommands(cedt.tools)                        file
    LoadMacroBuffers(cedt.macro)                        file
8.  LoadStdProfileSettings(8)                           MFC MRU (registry)
9.  Create CMainFrame
    LoadBrowsingDirectory / LoadWorkingDirectory        registry
    LoadWorkspaceFilePath                               registry
    LoadWindowPlacement / LoadBarState                  registry
```

Step 7 is where the bulk of user preferences come back, and it uses the file backend. Steps 1, 4, 8, and 9 use the registry.

---

## 2. Storage map

### 2.1 Windows Registry — `HKCU\Software\Crimson System\Crimson Editor\`

`SetRegistryKey(STRING_COMPANYNAME)` at the top of `InitInstance` parks every subsequent `WriteProfile*` / `GetProfile*` call under this subtree. `STRING_COMPANYNAME` resolves to the literal `"Crimson System"`. The MFC registry path is normally `Software\<RegistryKey>\<ProfileName>\`, and MFC seeds `m_pszProfileName` from `m_pszAppName` (i.e. the value loaded from `AFX_IDS_APP_TITLE` — `"크림슨에디터"` for the KR build, `"Crimson Editor"` for US). The code immediately overrides `m_pszProfileName` with the fixed English `"Crimson Editor"` so both builds share the same registry subtree, while leaving `m_pszAppName` (used by `AfxGetAppName()` in window titles and message-box captions) alone.

| Subkey / Value | Purpose |
| --- | --- |
| `REGKEY_ALLOW_MULTI_INSTANCES` | Whether more than one process is allowed at once. |
| `REGKEY_BROWSING_DIRECTORY` | Path shown in the left-side Directory panel. |
| `REGKEY_WORKING_DIRECTORY` | Process current working directory at startup. |
| `REGKEY_LAST_WORKSPACE` | Last project workspace file (re-opened automatically). |
| `REGKEY_WINDOW_PLACEMENT` | Main window position / size / maximized state. |
| `REGKEY_BAR_STATE` | Docking layout of toolbars and side panels. |
| MFC standard MRU | Written by `LoadStdProfileSettings(8)`; Recent Files menu. |

Code for the registry helpers: [src/app/cedtAppRegistry.cpp](../src/app/cedtAppRegistry.cpp).

> **InstallDir lives in HKLM, not HKCU.** It is a machine property (where Crimson is installed on this box), so the installer writes it once to `HKLM\Software\Crimson System\Crimson Editor` value `InstallDir`. The running app reads it from there and falls back to the EXE's own directory when HKLM is empty (installer-less / portable use). It is intentionally not cached in HKCU — see §6 and §7 for the rationale.

### 2.2 File system — `%APPDATA%\Crimson Editor\`

`SHGetSpecialFolderPath(NULL, ..., CSIDL_APPDATA, TRUE)` resolves to e.g. `C:\Users\<user>\AppData\Roaming\Crimson Editor`. The `TRUE` last argument means the folder is created automatically if it does not exist.

| File | Contents | Loader |
| --- | --- | --- |
| `cedt.conf` | Main settings — fonts, view options, syntax options, print options. Most of the Preferences dialog ends up here. | [`LoadUserConfiguration`](../src/app/cedtAppConf.cpp) |
| `cedt.color` | Active color scheme (text/background/syntax category colors). | [`LoadColorScheme`](../src/app/cedtAppConf.cpp) |
| `cedt.ftp` | FTP account list. | [`LoadFtpAccountInfo`](../src/app/cedtAppConf.cpp) |
| `cedt.tools` | User-defined external tools (the Tools menu). | [`LoadUserCommands`](../src/app/cedtAppConf.cpp) |
| `cedt.macro` | Recorded macros. | [`LoadMacroBuffers`](../src/app/cedtAppConf.cpp) |

All five are stored as **binary** files (opened with `ios::in | ios::binary`), with a version magic written at the top of each so the loader can reject schema mismatches. They are not meant to be hand-edited.

### 2.3 InstallDir copies as factory defaults

`cedt.conf` and `cedt.color` may also exist inside the install directory itself, shipped by the installer. They serve as the "factory default" that the loader falls back to before resorting to hardcoded defaults (see §3).

---

## 3. Per-file load behavior

### 3.1 `cedt.conf` — three-tier fallback **with** an error dialog

```cpp
if( ! LoadUserConfiguration(AppData + "\\cedt.conf") ) {
    if( ! LoadUserConfiguration(InstallDir + "\\cedt.conf") ) {
        AfxMessageBox(IDS_ERR_CORRUPT_CONFIG_FILE, MB_OK | MB_ICONEXCLAMATION);
        SetDefaultConfiguration();
    }
    SaveUserConfiguration(AppData + "\\cedt.conf");
}
```

Behavior:

1. Read `%APPDATA%\Crimson Editor\cedt.conf` — if OK, done.
2. Otherwise try `<InstallDir>\cedt.conf` — if OK, fall through to step 4.
3. Otherwise show the `IDS_ERR_CORRUPT_CONFIG_FILE` message box and run `SetDefaultConfiguration()` (hardcoded baseline, see §4).
4. Whatever ended up in memory is then written to `%APPDATA%\Crimson Editor\cedt.conf` so the next launch finds it directly.

> ⚠️ The dialog is the same for "file missing" and "file corrupted", which makes the very first launch (no AppData copy yet, no factory copy yet) display a "settings are corrupted" message even though nothing is actually wrong. See §6 for the implication.

### 3.2 `cedt.color` — three-tier fallback **without** a dialog

```cpp
if( ! LoadColorScheme(AppData + "\\cedt.color") ) {
    if( ! LoadColorScheme(InstallDir + "\\cedt.color") )
        SetPredefinedColorScheme(COLOR_SCHEME_DEFAULT);
    SaveColorScheme(AppData + "\\cedt.color");
}
```

Same shape as `cedt.conf` but quieter — when both file copies are absent, `SetPredefinedColorScheme(COLOR_SCHEME_DEFAULT)` silently installs the built-in default scheme and persists it to AppData.

### 3.3 `cedt.ftp`, `cedt.tools`, `cedt.macro` — silent miss

```cpp
LoadFtpAccountInfo(AppData + "\\cedt.ftp");
LoadUserCommands  (AppData + "\\cedt.tools");
LoadMacroBuffers  (AppData + "\\cedt.macro");
```

Return value is not checked. There is no install-directory fallback for these three. If the file is missing or unreadable, the corresponding in-memory collection simply stays empty:

- No FTP accounts configured.
- No user-defined tools in the Tools menu.
- No macros.

The file is created the first time the user adds an entry through Preferences and the app calls the matching `Save*` function.

### 3.4 Registry-backed items — silent miss

If the registry values do not exist, the loaders return defaults:

| Missing value | Resulting state |
| --- | --- |
| MRU list | Recent Files menu is empty. |
| `REGKEY_BROWSING_DIRECTORY` / `REGKEY_WORKING_DIRECTORY` | OS current directory at startup time. |
| `REGKEY_LAST_WORKSPACE` | No workspace re-opened. |
| `REGKEY_WINDOW_PLACEMENT` | MFC default window position / size. |
| `REGKEY_BAR_STATE` | All docking panels in their default position. |
| `REGKEY_ALLOW_MULTI_INSTANCES` | `FALSE` (single-instance behavior). |

---

## 4. `SetDefaultConfiguration` — hardcoded baseline

Defined at `src/app/cedtAppConf.cpp:330`. This is the last-resort baseline used when both the AppData and InstallDir copies of `cedt.conf` are unavailable.

Key defaults set by this function:

| Category | Default |
| --- | --- |
| Screen fonts (6 slots) | Courier New, FixedSys, Verdana, Arial, Lucida Console, Terminal — current = Courier New |
| Printer fonts (4 slots) | Courier New / Verdana / Arial / Lucida Console |
| Syntax highlighting | **ON** |
| Active-line highlighting | **ON** |
| Bracket pair matching | **ON** |
| Auto indent | **ON** |
| Show line numbers | OFF |
| Bold keywords / italicize comments | OFF |
| Word wrap on open | OFF |
| Replace tabs with spaces | OFF |
| Search wraps at end of file | ON |
| Drag-and-drop editing | ON |
| `Home` jumps to first non-blank column | OFF |
| Copy current line when nothing is selected | OFF |
| Show whitespace / tabs / line breaks | OFF |

The `SetPredefinedColorScheme(COLOR_SCHEME_DEFAULT)` companion handles the equivalent baseline for colors.

---

## 5. When configuration is saved

Crimson Editor does **not** wait until shutdown to flush settings. Most changes are persisted **immediately** — right after the user's action — so an unexpected crash typically loses very little. There are also a few "save on main-frame close" writes for window-layout state.

### 5.1 `cedt.conf` — every time a setting changes

Triggered by:

- **View menu toggles** — `Line Numbers`, `Show Spaces`, `Show Tabs`, `Embolden Keywords`, `Italicize Comment`, and similar. Each `On*` handler in [../src/app/cedtAppHndr.cpp](../src/app/cedtAppHndr.cpp) ends with `SaveUserConfiguration`.
- **View menu picks** — line spacing, screen/printer font, tab size, column markers, ...
- **Directory panel "Set as working directory"** — [../src/app/cedtAppDirectory.cpp](../src/app/cedtAppDirectory.cpp) at line 52.
- **Preferences dialog OK** — `SaveAllPrefSettings()` in [../src/dialogs/preferences/prefdialog.cpp](../src/dialogs/preferences/prefdialog.cpp) calls `SaveUserConfiguration` after committing every page.
- **Main frame close** — [../src/frame/MainFrm.cpp](../src/frame/MainFrm.cpp) at line 229 — a final write before exit.

### 5.2 `cedt.color` — only by explicit color-scheme commands

Triggered by:

- **View → Color Scheme → Default / Simplified / Reversed / Lightgray / Darkblue** (`OnViewColorScheme(nScheme)`) — picking a predefined scheme.
- **View → Color Scheme → Saved...** (`OnViewColorSchemeSaved()`) — loading a `.scheme` file from disk.

> ⚠️ **Pitfall**: `SaveAllPrefSettings()` in the Preferences dialog only updates **in-memory** colors via `SaveColorSettings()` ([../src/dialogs/preferences/PrefDialogColors.cpp](../src/dialogs/preferences/PrefDialogColors.cpp) line 109) — it does **not** write `cedt.color`. Colors edited on the Preferences → Colors page apply for the current session but are **lost on the next launch** unless the user also picks a scheme through the View → Color Scheme menu (which does call `SaveColorScheme`). The dedicated "Save Color Scheme" button on the Colors page writes to a user-chosen file, not to `cedt.color`.

### 5.3 `cedt.ftp` — after the FTP dialogs close

- After `OnFileOpenRemote` (`Open Remote` dialog) — [../src/app/cedtAppFile.cpp](../src/app/cedtAppFile.cpp) at line 73 and again on the FTP login path at line 249.
- After `OnFileFtpSettings` (FTP settings dialog) — [../src/app/cedtAppHndr.cpp](../src/app/cedtAppHndr.cpp) at line 23.

The save is unconditional after `DoModal()`, so even cancelling the dialog still rewrites the file with the most recent in-memory account list.

### 5.4 `cedt.tools` — when user commands are modified

- **Preferences dialog OK** — `SaveAllPrefSettings()` calls `SaveUserCommands`.
- **Tools → Open User Command File N** (`OnCommandUserFile0..7`) — after loading the chosen `.tools` file, the merged set is re-saved.

### 5.5 `cedt.macro` — when macros are modified

- **Preferences dialog OK** — same path as `cedt.tools`.
- **Macros → Open Macro File N** (`OnMacroUserFile0..7`).
- **Stop Macro Recording** (`OnMacroEndRecording`) — [../src/view/cedtViewHndrMisc.cpp](../src/view/cedtViewHndrMisc.cpp) at line 283 saves the buffer the moment recording ends.

### 5.6 Registry-backed items

| Value | Saved when |
| --- | --- |
| `REGKEY_ALLOW_MULTI_INSTANCES` | Preferences dialog OK |
| `REGKEY_BROWSING_DIRECTORY` / `REGKEY_WORKING_DIRECTORY` | Main frame close ([MainFrm.cpp](../src/frame/MainFrm.cpp) lines 232-235) |
| `REGKEY_LAST_WORKSPACE` | Main frame close, **first instance only** ([MainFrm.cpp](../src/frame/MainFrm.cpp) lines 237-239) |
| `REGKEY_WINDOW_PLACEMENT` / `REGKEY_BAR_STATE` | Main frame close ([MainFrm.cpp](../src/frame/MainFrm.cpp) lines 226-227) |
| MFC standard MRU (Recent Files) | Each time a document is opened or saved (MFC built-in) |

### 5.7 `ExitInstance()` does *not* save anything itself

[`CCedtApp::ExitInstance()`](../src/app/cedtapp.cpp) only releases HtmlHelp resources. The header comment at [cedtapp.cpp lines 440-441](../src/app/cedtapp.cpp) explicitly states that user configuration is saved "when mainframe is closed and whenever there is a change", so there is no further save-on-exit step.

### 5.8 Summary table

| Trigger | What gets written |
| --- | --- |
| View-menu option toggle / pick | `cedt.conf` |
| Directory panel: set working directory | `cedt.conf` |
| View → Color Scheme menu | `cedt.color` |
| FTP dialog close | `cedt.ftp` |
| Tools → Open User Command File N | `cedt.tools` |
| Macros → Open Macro File N | `cedt.macro` |
| Macros → Stop Recording | `cedt.macro` |
| Preferences dialog OK | `cedt.conf` + `cedt.tools` + `cedt.macro` + `REGKEY_ALLOW_MULTI_INSTANCES` (but **not** `cedt.color`) |
| Main frame close | `cedt.conf` + window placement + bar state + browsing dir + working dir + last workspace |
| Opening / saving a document | MFC MRU (registry, automatic) |

---

## 6. The startup sequence in plain words

Putting the file behaviors next to the registry behaviors:

1. Establish the registry root under `HKCU\Software\Crimson System\Crimson Editor` for all per-user state.
2. Figure out where Crimson Editor is installed by reading `HKLM\Software\Crimson System\Crimson Editor` value `InstallDir`, falling back to the EXE's own directory if HKLM is empty (portable / unzip-and-run case).
3. Compute `%APPDATA%\Crimson Editor\` and create the folder if needed.
4. Read the multi-instance flag from the registry.
5. If another instance is already running and multi-instance is disabled, forward the command line to that instance and exit silently.
6. Initialize OLE, common controls, and the 3D control flag.
7. Load `cedt.conf`, then `cedt.color`, then `cedt.ftp` / `cedt.tools` / `cedt.macro` — with the per-file fallback semantics described in §3.
8. Load MFC standard registry items (MRU).
9. Build the main MDI frame, then restore browsing directory, working directory, last workspace, window placement, and docking-bar state from the registry.

---

## 7. What happens when nothing exists yet (first run)

When neither the registry subtree nor any file in `%APPDATA%\Crimson Editor\` exists yet:

1. **Install directory** — HKLM is empty (no installer was run, or this is a fresh portable copy), so the code derives the install directory from the running EXE's own location via `GetModuleFileName` + `GetFileDirectory`. This derivation runs **every launch** rather than being cached anywhere, so moving the EXE to another folder or to a different drive letter (USB stick) is followed automatically with no stale-pointer risk. (Earlier versions popped up a folder-picker dialog at this point; that was removed so unzip-and-run setups work without prompting.)
2. **`cedt.conf`** — both AppData and InstallDir copies are missing, so the user sees the `IDS_ERR_CORRUPT_CONFIG_FILE` message box, the hardcoded defaults (§4) are applied, and the result is written to AppData.
3. **`cedt.color`** — both copies missing → the predefined default scheme is installed silently and persisted to AppData.
4. **`cedt.ftp` / `cedt.tools` / `cedt.macro`** — all missing → start empty; nothing is shown to the user.
5. **Registry** — everything else falls back to its built-in default (empty MRU, default window placement, etc.).

After this first run completes, every settings file exists in AppData, and the next launch is silent and instant.

### UX edge case

Because step 2 cannot distinguish "file is missing" (legitimate first run) from "file is corrupted" (something went wrong), brand-new users see a scary-looking error message that is actually harmless. Splitting those two cases inside `LoadUserConfiguration` (return distinct codes) would let `InitInstance` either stay silent on first run or show a welcoming "Initialized with defaults" message instead.

---

## 8. Backup and migration

To move a user's setup to another machine, copy two things:

| Source | Destination |
| --- | --- |
| `%APPDATA%\Crimson Editor\` (the whole folder) | Same path on the target |
| `HKEY_CURRENT_USER\Software\Crimson System\Crimson Editor` | Same subkey on the target |

The first carries fonts, colors, FTP accounts, tools, and macros; the second carries window/dock layout, MRU, last workspace, and other UI state.

`InstallDir` is **not** part of the migration. It lives in HKLM on the target host and is set by that host's installer — the install path on the new machine may legitimately differ from the source machine.

---

## 9. Improvement candidates

Items left for future work, originally surfaced while documenting the loading-and-saving behaviour. The bug-shaped fixes (Preferences-colors persistence, first-run corruption message, registry-path unification, install-directory inference) have all been applied — see git history for details. What's listed below is the work still pending.

### 9.1 Structural — medium effort

#### 9.1.1 No schema migration for `cedt.conf`

`LoadUserConfiguration` compares the version string at the head of the file; a mismatch bails out and the loader cascades down to the InstallDir copy or to `SetDefaultConfiguration`. Every binary-layout change wipes the user's settings.

Suggested fix: keep version-tagged loader branches (`LoadUserConfiguration_v1`, `_v2`, ...) and an in-memory upgrade step, so a version bump preserves what it can.

#### 9.1.2 32 → 64 bit transition will break `cedt.conf`

Most fields are written via raw `fread((char*)&member, sizeof(member), ...)` against types like `LONG` whose widths differ across platforms. The README's TODO item "Review Unicode build" already lives in this area — worth scheduling them together.

#### 9.1.3 Disk I/O on every View-menu toggle

Each toggle (`Show Spaces`, `Show Tabs`, ...) rewrites the whole `cedt.conf`. Harmless on local disks but noticeable on a network home directory.

Suggested fix: keep a "dirty" flag in memory, debounce with `SetTimer` (e.g. 1 s), flush once.

### 9.2 Nice-to-have — large refactor

| # | Item | Notes |
| --- | --- | --- |
| 9.2.1 | All configuration files are binary | Users cannot diff, edit, or version-control them. Migrating to JSON / TOML / INI is a sizeable schema-definition exercise. |
| 9.2.2 | `SetDefaultConfiguration` is hardcoded in C++ | Changing the shipped defaults requires a recompile. An `InstallDir\defaults.conf` seed file would let packagers tune without rebuilding. |
| 9.2.3 | AppData folder is flat | `cedt.conf` / `cedt.color` / `cedt.ftp` / `cedt.tools` / `cedt.macro` all sit at the same level. Subdirectories (`config/`, `colors/`, `scripts/`) would make manual backup cleaner, but require migration. |
| 9.2.4 | No final flush in `ExitInstance` | If main-frame `OnClose` is bypassed (e.g. forced shutdown while modal dialogs are open) the last in-memory changes may be lost. Most settings are saved immediately so the window is narrow, but a defensive final save would close it. |
| 9.2.5 | Registry-vs-file split is not documented as a rule | The unwritten convention "UI state in registry, user preferences in files" is mostly followed, but `BrowsingDirectory` / `WorkingDirectory` blur the line. A short guideline here would prevent drift when adding new state. |

### 9.3 Security — needs a dedicated audit

#### 9.3.1 `cedt.ftp` password storage

`CFtpAccount::m_szPassword` is persisted when `m_bSavePassword` is `TRUE`. The only obfuscation primitive nearby is `map_encode` / `map_decode` in [../src/util/encode.cpp](../src/util/encode.cpp), which is scrambling rather than encryption. A dedicated audit is needed before treating `cedt.ftp` as anything more than mildly obscured plain text — moving to DPAPI (`CryptProtectData`) would close the gap.

---

## 10. Source pointers

- [src/app/cedtapp.cpp](../src/app/cedtapp.cpp) — `InitInstance` (the orchestrator).
- [src/app/cedtAppConf.cpp](../src/app/cedtAppConf.cpp) — file-backed Save/Load and `SetDefaultConfiguration`.
- [src/app/cedtAppRegistry.cpp](../src/app/cedtAppRegistry.cpp) — registry-backed Save/Load helpers.
- [src/include/cedtHeader.h](../src/include/cedtHeader.h) — `REGKEY_*` / `REGPATH_*` / `STRING_*` constants.

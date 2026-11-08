# Portable edition — a design

**Status:** design only, not started. This records what it would take to run Crimson Editor from a
folder (a USB stick, a synced directory, a machine without admin rights) with every setting
following the folder instead of the machine.

The good news first: most of the state already goes through two chokepoints — one directory
variable and MFC's profile API — so the core of the change is a few dozen lines in
`CCedtApp::InitInstance`. The rest is switching off the parts that are, by nature, about
integrating with one particular machine.

## Where state lives today

[configuration-reference.md](configuration-reference.md) has the full storage map. Grouped by what a
portable build has to do with each:

| Location | What is there | Portable |
| --- | --- | --- |
| **HKCU** `Software\Crimson System\Crimson Editor\` | Recent-file list (MRU), window placement, toolbar/docking layout (`SaveBarState`), find/replace/find-in-files history and options, browsing and working directory, last workspace, allow-multi-instances flag | **Must move** |
| **`%APPDATA%\Crimson Editor\`** | `cedt_kr.conf` / `cedt_us.conf` (one per edition), `cedt.colors`, `cedt.ftp`, `cedt.tools`, `cedt.macros`, `tools\` `macros\` `colors\`, `cmdline.txt`, downloaded remote (FTP) files | Path change only |
| **Install directory** (read) | `spec\`, `template\`, `cedt.dic`, `user.dic`, factory-default `cedt_kr.conf` / `cedt_us.conf` / `cedt.colors` | Unchanged — read next to the exe |
| **HKLM / HKCR** | `InstallDir`, Explorer context menu (`ShellExt.dll`), file associations, IE "view source" editor | **Disable** |

## Design

### 1. Detecting portable mode

Portable mode is on when a marker sits next to the executable — `portable.ini`, or a `data\`
folder. This is the convention Notepad++ and VS Code use: the same binary serves both editions, and
turning a copy portable means dropping one file beside it.

Detection has to happen first in `InitInstance`, **before** `SetRegistryKey`, because step 3 depends
on that call not being made.

### 2. Data directory

`m_szAppDataDirectory` is assigned in one place ([cedtapp.cpp](../src/app/cedtapp.cpp), right after
the install directory is resolved) and every load and save of the edition's `.conf`, `cedt.colors`,
`cedt.ftp`, `cedt.tools`, `cedt.macros`, the `tools\`/`macros\`/`colors\` subfolders, `cmdline.txt`
and the remote-file cache builds on it. In portable mode it becomes `<exe folder>\data`. Nothing
downstream changes.

### 3. Registry → ini file

MFC already does this. When `SetRegistryKey` is **not** called and `m_pszProfileName` holds a full
path to an `.ini`, every `GetProfileString` / `WriteProfileString` / `GetProfileInt` /
`WriteProfileInt` call reads and writes that file instead of the registry. So the find/replace
history, MRU list (`LoadStdProfileSettings`), window placement and directory settings follow with
**no change at their call sites**: point `m_pszProfileName` at `<exe folder>\data\cedt.ini`. One file serves both editions, as the
registry subtree does today.

To verify: `SaveBarState` / `LoadBarState` also go through the profile API and work against an
ini, but they write one section per control bar, so the file grows untidy. Check that the docked
panels restore correctly before calling this done.

### 4. Install directory resolution — required

Today `InitInstance` reads `InstallDir` from **HKLM first** and only falls back to the executable's
own folder when that is missing. On a machine that also has the installed edition, a portable copy
would therefore read the *installed* copy's `spec\` files and dictionaries. In portable mode the
install directory must always be the executable's folder.

### 5. Switching off machine integration

The Preferences options that register file associations, the Explorer context-menu handler
(`ShellExt.dll`) and the Internet Explorer view-source editor all write HKLM/HKCR. In portable mode
they are disabled (greyed out with a short note). Writing someone else's registry is not portable,
and HKLM needs admin rights anyway.

### 6. Packaging

The executable links MFC and the CRT dynamically (`UseOfMfc=Dynamic`, `/MD`), so it will not start
on a machine without the VC++ redistributable. Two options:

- **(a) App-local DLLs — recommended.** Ship `mfc140u.dll`, `vcruntime140.dll`,
  `vcruntime140_1.dll` and `msvcp140.dll` beside the exe, copied from the VS `Redist\MSVC\...\x64`
  folders. Microsoft supports this deployment model, and the build stays as it is.
- **(b) Static linking.** One self-contained file, but several MB larger, which breaks the
  "< 2 MB Release binary" claim in the README. It would also need a separate configuration.

The package is a zip — `scripts/build_installer.ps1` gains a step that assembles it next to the
installer.

D2Coding cannot be installed system-wide from a portable folder. `AddFontResourceEx` with
`FR_PRIVATE` can register `fonts\D2Coding.ttf` for the running process only.

### 7. Things to know

- **FTP passwords.** `cedt.ftp` stores saved passwords with a reversible encoding (`map_encode`), not
  encryption. On a USB stick that matters more than on a home directory. Options: refuse "save
  password" in portable mode, or warn when it is ticked. Machine-bound encryption (DPAPI) is not an
  answer here — it would make the file unreadable on the next machine, which defeats portability.
- **Installed and portable side by side.** The single-instance check uses a shared data section, and
  a shared section is shared only among processes of the *same image file*. An installed and a
  portable copy therefore behave as two different applications and do not hand files to each other.
  `cmdline.txt`, the other half of that handoff, lives in each edition's own data directory.
- **Paths inside the settings.** MRU entries, the last workspace and project files hold absolute
  paths, which go stale when the stick mounts on a different drive letter. Rewriting them relative
  to the exe drive is possible but out of scope for a first version; stale entries already fail
  with a "file not found" message rather than silently.

## Scope

| Step | Size |
| --- | --- |
| 1–4: detection, data directory, ini profile, install directory | A few dozen lines in `InitInstance` |
| 5: disable machine integration | A few spots in the Preferences pages |
| 6: zip packaging | Build script |

Steps 1–4 alone give a working portable edition whose settings follow the folder; 5–7 are polish
and safety.

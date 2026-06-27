; Crimson Editor — Inno Setup installer script
;
; Build:   ISCC installer.iss
; Or use:  .\scripts\build_installer.ps1   (builds prerequisites + redist + ISCC)
;
; Design decisions, in case a future reader wonders:
;
; - Single installer that bundles BOTH cedt_kr.exe and cedt_us.exe under
;   Types/Components. Picking "Korean edition" or "English edition" on
;   the Select Components page installs only the matching EXE; "Custom"
;   lets the user check both.
;
; - InstallDir is written to HKLM\Software\Crimson System\Crimson Editor
;   exactly once, as a 64-bit-view value, and is the single source of
;   truth both cedt itself and ShellExt.dll read from. (See
;   docs/configuration.md §2.1.)
;
; - ShellExt.dll is a 64-bit DLL; we register it through the System32
;   regsvr32.exe (the actual native regsvr32 on 64-bit Windows) since
;   we run the installer in 64-bit mode via
;   ArchitecturesInstallIn64BitMode.
;
; - cedt is a 64-bit MFC dynamic-link build (Phase 6 migration), so
;   the Visual C++ x64 runtime is required. The redist is bundled and
;   run quietly during install. ShellExt and launch use static CRT,
;   so they need nothing extra.
;
; - User settings live entirely under HKCU, so the default uninstall
;   leaves the user's preferences/MRU/window state intact. A future
;   pass can add an "also remove user settings" check box to the
;   uninstall flow.

#define MyAppName         "Crimson Editor"
#define MyAppVersion      "3.81"
#define MyAppPublisher    "Ingyu Kang"
#define MyAppURL          "https://github.com/igkang00/CrimsonEditor"
#define MyAppExeKr        "cedt_kr.exe"
#define MyAppExeUs        "cedt_us.exe"

[Setup]
; A fresh AppId — unrelated to the legacy 3.70 NSIS install entry, so
; the new and old packages can coexist (and can be cleanly uninstalled
; separately) on a machine that has both.
AppId={{F2B5C7A8-3D9E-4C61-8F4A-1B7D6E2C9F03}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}/releases

DefaultDirName={autopf}\Crimson Editor
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=yes
AllowNoIcons=yes

OutputDir=dist
; cedt-380-setup.exe — matches the cedt.exe/cedt.dic/cedt.ico file
; naming used inside the install folder. {#StringChange ...} strips
; the dot so we get "380" rather than "3.80" (shorter on disk,
; URL-friendly when uploaded as a release artifact).
OutputBaseFilename=cedt-{#StringChange(MyAppVersion, ".", "")}-setup
SetupIconFile=runtime\cedt.ico
UninstallDisplayIcon={app}\cedt.ico

LicenseFile=runtime\LICENSE.txt
InfoAfterFile=runtime\README.txt

Compression=lzma2/ultra64
SolidCompression=yes
WizardStyle=modern

; All native binaries (cedt, ShellExt, launch) are 64-bit — run in
; 64-bit mode so HKLM writes hit the native (not WOW6432Node) view
; and {sys} resolves to System32 (so regsvr32 can load the x64 DLL).
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible

; Admin-only install. We need HKLM write access (for the
; InstallDir registry value that cedt + ShellExt read at runtime)
; and HKCR write access (for regsvr32 to register ShellExt.dll's
; context menu handler) — both of which require elevation. The
; per-user override was tried briefly but only ends up failing
; at the HKLM/regsvr32 steps, so disallow it outright and let
; Inno Setup raise the UAC prompt up front.
PrivilegesRequired=admin

ChangesAssociations=no


[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "korean";  MessagesFile: "compiler:Languages\Korean.isl"


[Types]
Name: "kr";     Description: "Korean edition (한국어판)"
Name: "us";     Description: "English edition"
Name: "custom"; Description: "Custom installation"; Flags: iscustom


[Components]
Name: "exe_kr"; Description: "Crimson Editor (Korean)";  Types: kr custom
Name: "exe_us"; Description: "Crimson Editor (English)"; Types: us custom


[Tasks]
Name: "desktopicon";      Description: "{cm:CreateDesktopIcon}";                                                  GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "shellcontextmenu"; Description: "Add 'Edit with Crimson Editor' to Explorer's right-click menu";          GroupDescription: "Shell integration:"


[Files]
; Application executables — one of the two ships, based on Types/Components.
Source: "build\x64\Release-KR\{#MyAppExeKr}";          DestDir: "{app}"; Components: exe_kr; Flags: ignoreversion
Source: "build\x64\Release-US\{#MyAppExeUs}";          DestDir: "{app}"; Components: exe_us; Flags: ignoreversion

; Helper binaries — always installed.
Source: "tools\launch\build\x64\Release\launch.exe";    DestDir: "{app}"; Flags: ignoreversion
Source: "tools\shellext\build\x64\Release\ShellExt.dll"; DestDir: "{app}"; Flags: ignoreversion 64bit

; Runtime assets — always installed, the support files cedt looks up
; under <InstallDir> at runtime (dictionaries, syntax specs, color
; schemes, templates, docs, ...).
Source: "runtime\cedt.ico";                             DestDir: "{app}";          Flags: ignoreversion
Source: "runtime\cedt.dic";                             DestDir: "{app}";          Flags: ignoreversion
Source: "runtime\user.dic";                             DestDir: "{app}";          Flags: ignoreversion onlyifdoesntexist
Source: "runtime\LICENSE.txt";                          DestDir: "{app}";          Flags: ignoreversion
Source: "runtime\README.txt";                           DestDir: "{app}";          Flags: ignoreversion
Source: "runtime\docs\*";                               DestDir: "{app}\docs";     Flags: ignoreversion recursesubdirs createallsubdirs
Source: "runtime\link\*";                               DestDir: "{app}\link";     Flags: ignoreversion
Source: "runtime\schemes\*";                            DestDir: "{app}\schemes";  Flags: ignoreversion
Source: "runtime\spec\*";                               DestDir: "{app}\spec";     Flags: ignoreversion
Source: "runtime\template\*";                           DestDir: "{app}\template"; Flags: ignoreversion
Source: "runtime\tools\*";                              DestDir: "{app}\tools";    Flags: ignoreversion

; Visual C++ x64 runtime redistributable — copied to {tmp}, run from
; [Run], deleted afterwards. build_installer.ps1 downloads this into
; dist\redist\ before invoking ISCC.
Source: "dist\redist\vc_redist.x64.exe";                DestDir: "{tmp}"; Flags: deleteafterinstall


[Icons]
; Start Menu
Name: "{group}\Crimson Editor (Korean)";  Filename: "{app}\{#MyAppExeKr}"; Components: exe_kr; Comment: "{#MyAppName} {#MyAppVersion} — Korean edition"
Name: "{group}\Crimson Editor (English)"; Filename: "{app}\{#MyAppExeUs}"; Components: exe_us; Comment: "{#MyAppName} {#MyAppVersion} — English edition"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"

; Desktop (optional)
Name: "{autodesktop}\Crimson Editor (KR)"; Filename: "{app}\{#MyAppExeKr}"; Components: exe_kr; Tasks: desktopicon
Name: "{autodesktop}\Crimson Editor (US)"; Filename: "{app}\{#MyAppExeUs}"; Components: exe_us; Tasks: desktopicon


[Registry]
; The single source of truth for "where is Crimson Editor installed".
; Read by both cedt itself and ShellExt.dll; written in 64-bit view
; (no WOW6432Node) because we're in 64-bit install mode.
Root: HKLM; Subkey: "Software\Crimson System\Crimson Editor"; ValueType: string; ValueName: "InstallDir"; ValueData: "{app}"; Flags: uninsdeletekey


[Run]
; 1) VC++ x64 runtime first. cedt is /MD (dynamic CRT) and needs it.
Filename: "{tmp}\vc_redist.x64.exe"; Parameters: "/install /quiet /norestart"; StatusMsg: "Installing Visual C++ x64 runtime..."; Flags: waituntilterminated

; 2) Register the shell extension (only if the user opted into the
;    "Edit with Crimson Editor" menu item). regsvr32 in System32 is
;    the native 64-bit one in 64-bit install mode.
Filename: "{sys}\regsvr32.exe"; Parameters: "/s ""{app}\ShellExt.dll"""; StatusMsg: "Registering shell extension..."; Flags: waituntilterminated runascurrentuser; Tasks: shellcontextmenu

; 3) Offer to launch the just-installed editor.
Filename: "{app}\{#MyAppExeKr}"; Description: "Launch Crimson Editor"; Flags: nowait postinstall skipifsilent; Components: exe_kr
Filename: "{app}\{#MyAppExeUs}"; Description: "Launch Crimson Editor"; Flags: nowait postinstall skipifsilent; Components: exe_us


[UninstallRun]
; Unregister the shell extension before files are removed. Use
; RunOnceId so multiple uninstall runs don't unregister twice.
Filename: "{sys}\regsvr32.exe"; Parameters: "/u /s ""{app}\ShellExt.dll"""; Flags: runascurrentuser; RunOnceId: "UnregShellExt"

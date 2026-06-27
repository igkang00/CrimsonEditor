// shellext.h — common declarations for the Crimson Editor shell extension.
//
// Implements an Explorer right-click context-menu handler that adds
// "Edit with Crimson Editor" (or "크림슨에디터로 편집" on a Korean OS)
// for every file. On click it spawns cedt_kr.exe / cedt_us.exe from
// the install directory that the installer recorded under
// HKLM\Software\Crimson System\Crimson Editor\InstallDir.
//
// The CLSID is kept identical to the 2002-era ShellExt.dll
// ({475A9681-F01B-11d5-BC5E-0050CE184C9B}, defined in
// src/include/cedtHeader.h as CLSID_SHELLEXT_CRIMSONEDITOR) so that a
// machine that already had the old handler registered does not end up
// with two competing entries — re-registering the new DLL just
// replaces the InProcServer32 pointer.

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shellapi.h>   // HDROP, DragQueryFile
#include <olectl.h>     // SELFREG_E_CLASS

// {475A9681-F01B-11d5-BC5E-0050CE184C9B}
// Matches CLSID_SHELLEXT_CRIMSONEDITOR in src/include/cedtHeader.h.
extern "C" const GUID CLSID_CrimsonShellExt;

#define SHELLEXT_PROGID         L"Crimson Editor"
#define SHELLEXT_DESCRIPTION    L"Crimson Editor Shell Extension"

// Resource ID for the menu icon embedded in shellext.rc.
#define IDI_CRIMSON             101

// DLL-wide reference counters consulted by DllCanUnloadNow().
extern HINSTANCE g_hInstDll;
extern LONG      g_cRefDll;
extern LONG      g_cLockDll;

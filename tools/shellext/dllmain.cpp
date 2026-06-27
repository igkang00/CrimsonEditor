// dllmain.cpp — DLL entry point, COM exports, regsvr32 support.

#include "shellext.h"
#include <stdio.h>

// {475A9681-F01B-11d5-BC5E-0050CE184C9B}
extern "C" const GUID CLSID_CrimsonShellExt =
    { 0x475A9681, 0xF01B, 0x11d5, { 0xBC, 0x5E, 0x00, 0x50, 0xCE, 0x18, 0x4C, 0x9B } };

HINSTANCE g_hInstDll = NULL;
LONG      g_cRefDll  = 0;
LONG      g_cLockDll = 0;

// Forward decl from classfactory.cpp.
class CClassFactory;
extern "C" HRESULT CreateCrimsonClassFactory(REFIID riid, void** ppv);

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID)
{
	if (dwReason == DLL_PROCESS_ATTACH) {
		g_hInstDll = hInst;
		DisableThreadLibraryCalls(hInst);
	}
	return TRUE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	if (ppv == NULL) return E_POINTER;
	*ppv = NULL;
	if (!IsEqualCLSID(rclsid, CLSID_CrimsonShellExt))
		return CLASS_E_CLASSNOTAVAILABLE;
	return CreateCrimsonClassFactory(riid, ppv);
}

STDAPI DllCanUnloadNow()
{
	return (g_cRefDll == 0 && g_cLockDll == 0) ? S_OK : S_FALSE;
}

// ---- Registry helpers ------------------------------------------------------

static LSTATUS WriteRegStringW(HKEY hRoot, LPCWSTR pszSubKey, LPCWSTR pszValue, LPCWSTR pszData)
{
	HKEY hKey;
	LSTATUS s = RegCreateKeyExW(hRoot, pszSubKey, 0, NULL, 0,
	                            KEY_WRITE, NULL, &hKey, NULL);
	if (s != ERROR_SUCCESS) return s;
	s = RegSetValueExW(hKey, pszValue, 0, REG_SZ,
	                   (const BYTE*)pszData, (DWORD)((wcslen(pszData) + 1) * sizeof(WCHAR)));
	RegCloseKey(hKey);
	return s;
}

// Recursively delete a key (RegDeleteTree analogue, kept local so we
// stay independent of shlwapi version quirks).
static LSTATUS DeleteRegKeyRecursive(HKEY hRoot, LPCWSTR pszSubKey)
{
	HKEY hKey;
	LSTATUS s = RegOpenKeyExW(hRoot, pszSubKey, 0, KEY_READ | KEY_WRITE, &hKey);
	if (s != ERROR_SUCCESS) return s;

	for (;;) {
		WCHAR szName[256]; DWORD cchName = 256;
		s = RegEnumKeyExW(hKey, 0, szName, &cchName, NULL, NULL, NULL, NULL);
		if (s != ERROR_SUCCESS) break;
		DeleteRegKeyRecursive(hKey, szName);
	}
	RegCloseKey(hKey);
	return RegDeleteKeyW(hRoot, pszSubKey);
}

STDAPI DllRegisterServer()
{
	WCHAR szModule[MAX_PATH];
	if (!GetModuleFileNameW(g_hInstDll, szModule, MAX_PATH))
		return SELFREG_E_CLASS;

	LPOLESTR pszCLSID = NULL;
	if (FAILED(StringFromCLSID(CLSID_CrimsonShellExt, &pszCLSID)))
		return SELFREG_E_CLASS;

	WCHAR szKey[256];

	// HKCR\CLSID\{...}    (default = description)
	swprintf_s(szKey, L"CLSID\\%s", pszCLSID);
	WriteRegStringW(HKEY_CLASSES_ROOT, szKey, NULL, SHELLEXT_DESCRIPTION);

	// HKCR\CLSID\{...}\InProcServer32   (default = DLL path, ThreadingModel = Apartment)
	swprintf_s(szKey, L"CLSID\\%s\\InProcServer32", pszCLSID);
	WriteRegStringW(HKEY_CLASSES_ROOT, szKey, NULL, szModule);
	WriteRegStringW(HKEY_CLASSES_ROOT, szKey, L"ThreadingModel", L"Apartment");

	// HKCR\*\shellex\ContextMenuHandlers\Crimson Editor   (default = CLSID string)
	WriteRegStringW(HKEY_CLASSES_ROOT,
	                L"*\\shellex\\ContextMenuHandlers\\Crimson Editor",
	                NULL, pszCLSID);

	// HKLM\Software\Microsoft\Windows\CurrentVersion\Shell Extensions\Approved
	// {CLSID} = description    (required on shipping Windows so Explorer
	// is allowed to load the in-proc server in elevated sessions)
	WriteRegStringW(HKEY_LOCAL_MACHINE,
	                L"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved",
	                pszCLSID, SHELLEXT_DESCRIPTION);

	CoTaskMemFree(pszCLSID);

	// Ask Explorer to re-read its assoc cache so the new menu shows
	// up without a logoff. (Best-effort; ignore failure.)
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);

	return S_OK;
}

STDAPI DllUnregisterServer()
{
	LPOLESTR pszCLSID = NULL;
	if (FAILED(StringFromCLSID(CLSID_CrimsonShellExt, &pszCLSID)))
		return SELFREG_E_CLASS;

	WCHAR szKey[256];

	// Drop the ContextMenuHandlers entry first — that's the user-visible
	// one. If we crash partway through, at least the menu item is gone.
	DeleteRegKeyRecursive(HKEY_CLASSES_ROOT,
	                      L"*\\shellex\\ContextMenuHandlers\\Crimson Editor");

	// HKLM Approved entry.
	HKEY hApproved;
	if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
	                  L"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved",
	                  0, KEY_WRITE, &hApproved) == ERROR_SUCCESS) {
		RegDeleteValueW(hApproved, pszCLSID);
		RegCloseKey(hApproved);
	}

	// HKCR\CLSID\{...} (subtree).
	swprintf_s(szKey, L"CLSID\\%s", pszCLSID);
	DeleteRegKeyRecursive(HKEY_CLASSES_ROOT, szKey);

	CoTaskMemFree(pszCLSID);
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
	return S_OK;
}

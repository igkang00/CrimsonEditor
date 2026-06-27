// contextmenu.cpp — IShellExtInit + IContextMenu implementation.
//
// Lifecycle on a right-click in Explorer:
//
//   Initialize(pDataObj, ...)
//       Collect every selected path from the CF_HDROP IDataObject into
//       m_files.
//
//   QueryContextMenu(hMenu, idxMenu, idCmdFirst, ...)
//       Insert one menu item at idxMenu. Language picked from
//       GetUserDefaultUILanguage() — Korean OS gets the Korean label,
//       everyone else gets English. Return the number of command IDs
//       used as MAKE_HRESULT(SEVERITY_SUCCESS, 0, used).
//
//   InvokeCommand(pici)
//       Read InstallDir from HKLM\Software\Crimson System\Crimson
//       Editor. Prefer cedt_kr.exe; fall back to cedt_us.exe (the
//       installer only writes one of the two, based on the user's
//       language choice). ShellExecuteW each selected file as an
//       argument so each file opens in its own MDI tab.

#include "shellext.h"
#include <vector>
#include <string>
#include <new>          // std::nothrow

// Shared with dllmain.cpp — lazy-loaded on first QueryContextMenu,
// freed in DLL_PROCESS_DETACH.
HBITMAP g_hMenuBitmap = NULL;

// Render the embedded IDI_CRIMSON icon into a 32-bit top-down DIB so
// it shows up correctly (with alpha) in Explorer's context menu.
// Uses the small-icon metric so it visually matches the system menu
// font on whatever DPI the user is at.
static HBITMAP CreateMenuBitmap()
{
	const int cx = GetSystemMetrics(SM_CXSMICON);
	const int cy = GetSystemMetrics(SM_CYSMICON);

	HICON hIcon = (HICON)LoadImageW(g_hInstDll, MAKEINTRESOURCEW(IDI_CRIMSON),
	                                IMAGE_ICON, cx, cy, LR_DEFAULTCOLOR);
	if (!hIcon) return NULL;

	HDC hdcScreen = GetDC(NULL);
	HDC hdcMem    = CreateCompatibleDC(hdcScreen);

	BITMAPINFO bi = { 0 };
	bi.bmiHeader.biSize        = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth       = cx;
	bi.bmiHeader.biHeight      = -cy;   // top-down
	bi.bmiHeader.biPlanes      = 1;
	bi.bmiHeader.biBitCount    = 32;
	bi.bmiHeader.biCompression = BI_RGB;

	void* pBits = NULL;
	HBITMAP hBmp = CreateDIBSection(hdcScreen, &bi, DIB_RGB_COLORS, &pBits, NULL, 0);
	if (hBmp) {
		HGDIOBJ hOld = SelectObject(hdcMem, hBmp);
		DrawIconEx(hdcMem, 0, 0, hIcon, cx, cy, 0, NULL, DI_NORMAL);
		SelectObject(hdcMem, hOld);
	}

	DeleteDC(hdcMem);
	ReleaseDC(NULL, hdcScreen);
	DestroyIcon(hIcon);
	return hBmp;
}

static HBITMAP GetCachedMenuBitmap()
{
	// One-shot lazy init. Once we've decided whether the icon load
	// succeeded or failed, we don't retry; both outcomes are sticky
	// for the life of the DLL. The InterlockedCompareExchange makes
	// the "first to set the flag" race resolve to a single load.
	static volatile LONG s_loaded = 0;
	if (InterlockedCompareExchange(&s_loaded, 1, 0) == 0) {
		g_hMenuBitmap = CreateMenuBitmap();
	}
	return g_hMenuBitmap;
}

class CCrimsonContextMenu : public IShellExtInit, public IContextMenu
{
public:
	CCrimsonContextMenu() : m_cRef(1) { InterlockedIncrement(&g_cRefDll); }

	// IUnknown
	IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv) override
	{
		if (ppv == NULL) return E_POINTER;
		if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IShellExtInit)) {
			*ppv = static_cast<IShellExtInit*>(this);
		} else if (IsEqualIID(riid, IID_IContextMenu)) {
			*ppv = static_cast<IContextMenu*>(this);
		} else {
			*ppv = NULL;
			return E_NOINTERFACE;
		}
		AddRef();
		return S_OK;
	}
	IFACEMETHODIMP_(ULONG) AddRef() override
	{
		return InterlockedIncrement(&m_cRef);
	}
	IFACEMETHODIMP_(ULONG) Release() override
	{
		ULONG c = InterlockedDecrement(&m_cRef);
		if (c == 0) delete this;
		return c;
	}

	// IShellExtInit
	IFACEMETHODIMP Initialize(LPCITEMIDLIST, IDataObject* pDataObj, HKEY) override
	{
		m_files.clear();
		if (pDataObj == NULL) return E_INVALIDARG;

		FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		STGMEDIUM stg = { 0 };
		if (FAILED(pDataObj->GetData(&fmt, &stg))) return E_INVALIDARG;

		HDROP hDrop = (HDROP)GlobalLock(stg.hGlobal);
		if (hDrop == NULL) { ReleaseStgMedium(&stg); return E_INVALIDARG; }

		UINT n = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);
		for (UINT i = 0; i < n; ++i) {
			WCHAR sz[MAX_PATH];
			if (DragQueryFileW(hDrop, i, sz, MAX_PATH) > 0)
				m_files.push_back(sz);
		}

		GlobalUnlock(stg.hGlobal);
		ReleaseStgMedium(&stg);
		return m_files.empty() ? E_INVALIDARG : S_OK;
	}

	// IContextMenu
	IFACEMETHODIMP QueryContextMenu(HMENU hMenu, UINT idxMenu,
	                                UINT idCmdFirst, UINT /*idCmdLast*/,
	                                UINT uFlags) override
	{
		if (uFlags & CMF_DEFAULTONLY)
			return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);

		LANGID lang = GetUserDefaultUILanguage();
		LPCWSTR pszLabel = (PRIMARYLANGID(lang) == LANG_KOREAN)
			? L"크림슨에디터로 편집(&C)..."
			: L"Edit with &Crimson Editor...";

		if (!InsertMenuW(hMenu, idxMenu, MF_BYPOSITION | MF_STRING,
		                 idCmdFirst, pszLabel)) {
			return HRESULT_FROM_WIN32(GetLastError());
		}

		// Attach the cedt application icon to the menu item. Best-
		// effort — if bitmap creation failed for any reason (low
		// memory, theme quirk, ...) we just show the text-only item.
		HBITMAP hBmp = GetCachedMenuBitmap();
		if (hBmp) {
			MENUITEMINFOW mii = { sizeof(mii) };
			mii.fMask    = MIIM_BITMAP;
			mii.hbmpItem = hBmp;
			SetMenuItemInfoW(hMenu, idxMenu, TRUE, &mii);
		}

		// Tell Explorer we used one command ID (idCmdFirst + 0).
		return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 1);
	}

	IFACEMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO pici) override
	{
		if (pici == NULL) return E_INVALIDARG;

		// Only the verb id we registered (offset 0) is valid here.
		// A string verb is something else (drag-drop helpers etc.).
		if (HIWORD(pici->lpVerb) != 0) return E_FAIL;
		if (LOWORD(pici->lpVerb) != 0) return E_FAIL;

		WCHAR szExe[MAX_PATH];
		if (!ResolveCedtExe(szExe, MAX_PATH)) return E_FAIL;

		// Open every selected file with cedt; rely on cedt's single-
		// instance IPC (CmdLine.cpp) to merge them into one window.
		for (size_t i = 0; i < m_files.size(); ++i) {
			ShellExecuteW(NULL, L"open", szExe, m_files[i].c_str(),
			              NULL, SW_SHOWNORMAL);
		}
		return S_OK;
	}

	IFACEMETHODIMP GetCommandString(UINT_PTR idCmd, UINT uType,
	                                UINT* /*pReserved*/, CHAR* pszName,
	                                UINT cchMax) override
	{
		if (idCmd != 0) return E_INVALIDARG;
		if (uType == GCS_HELPTEXTW) {
			LPCWSTR p = L"Open with Crimson Editor";
			lstrcpynW((LPWSTR)pszName, p, cchMax);
			return S_OK;
		} else if (uType == GCS_HELPTEXTA) {
			lstrcpynA(pszName, "Open with Crimson Editor", cchMax);
			return S_OK;
		} else if (uType == GCS_VERBW) {
			lstrcpynW((LPWSTR)pszName, L"crimsonedit", cchMax);
			return S_OK;
		} else if (uType == GCS_VERBA) {
			lstrcpynA(pszName, "crimsonedit", cchMax);
			return S_OK;
		}
		return E_NOTIMPL;
	}

private:
	~CCrimsonContextMenu() { InterlockedDecrement(&g_cRefDll); }

	// Locate the cedt EXE the installer recorded in HKLM.
	//
	// InstallDir is, semantically, a machine property — "where is
	// Crimson Editor installed on THIS box" — so it lives in HKLM
	// only. cedt itself reads the same single source of truth
	// (src/app/cedtapp.cpp), so ShellExt and cedt always agree.
	//
	// The WOW6432Node mirror is tried as a fallback because the
	// legacy 32-bit ShellExt.dll from 3.70 caused the OS to redirect
	// its registry writes there, and we want a clean upgrade from
	// that install state to succeed.
	//
	// Returns TRUE and writes the full EXE path into pszOut on success.
	static BOOL ResolveCedtExe(LPWSTR pszOut, DWORD cchMax)
	{
		WCHAR szDir[MAX_PATH] = { 0 };
		DWORD cb = sizeof(szDir);

		if (!ReadInstallDirHKLM(0, szDir, &cb)) {
			cb = sizeof(szDir);
			if (!ReadInstallDirHKLM(KEY_WOW64_32KEY, szDir, &cb)) {
				return FALSE;
			}
		}

		// Trim trailing backslash if any.
		size_t n = wcslen(szDir);
		while (n > 0 && (szDir[n-1] == L'\\' || szDir[n-1] == L'/')) szDir[--n] = 0;

		// Prefer cedt_kr.exe; fall back to cedt_us.exe; fall back to
		// the legacy cedt.exe name in case a 3.70 install is reused.
		const wchar_t* names[] = { L"cedt_kr.exe", L"cedt_us.exe", L"cedt.exe" };
		for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); ++i) {
			swprintf_s(pszOut, cchMax, L"%s\\%s", szDir, names[i]);
			if (GetFileAttributesW(pszOut) != INVALID_FILE_ATTRIBUTES)
				return TRUE;
		}
		return FALSE;
	}

	// HKLM\Software\Crimson System\Crimson Editor  "InstallDir" = REG_SZ
	static BOOL ReadInstallDirHKLM(REGSAM samExtra, LPWSTR pszOut, DWORD* pcb)
	{
		HKEY hKey;
		if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
		                  L"Software\\Crimson System\\Crimson Editor",
		                  0, KEY_QUERY_VALUE | samExtra, &hKey) != ERROR_SUCCESS) {
			return FALSE;
		}
		DWORD type = 0;
		LSTATUS s = RegQueryValueExW(hKey, L"InstallDir", NULL, &type,
		                             (BYTE*)pszOut, pcb);
		RegCloseKey(hKey);
		return (s == ERROR_SUCCESS && (type == REG_SZ || type == REG_EXPAND_SZ));
	}

	LONG m_cRef;
	std::vector<std::wstring> m_files;
};

extern "C" HRESULT CreateCrimsonContextMenu(REFIID riid, void** ppv)
{
	CCrimsonContextMenu* p = new (std::nothrow) CCrimsonContextMenu();
	if (!p) return E_OUTOFMEMORY;
	HRESULT hr = p->QueryInterface(riid, ppv);
	p->Release();
	return hr;
}

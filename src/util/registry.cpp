#include "stdafx.h"
#include "registry.h"


BOOL GetRegKeyValue(HKEY hRoot, LPCTSTR lpszRegPath, LPCTSTR lpszValName, TCHAR * lpData, DWORD dwSize)
{
	DWORD dwType; HKEY hKey;
	if( RegOpenKeyEx(hRoot, lpszRegPath, 0, KEY_QUERY_VALUE, & hKey) != ERROR_SUCCESS ) return FALSE;
	if( RegQueryValueEx(hKey, lpszValName, 0, & dwType, (BYTE *)lpData, & dwSize) != ERROR_SUCCESS ) return FALSE;
	if( RegCloseKey(hKey) != ERROR_SUCCESS ) return FALSE;
	return TRUE;
}

BOOL GetRegKeyValue(HKEY hRoot, LPCTSTR lpszRegPath, LPCTSTR lpszValName, CString & szValue)
{
	// RegQueryValueEx is the W variant under _UNICODE: it writes the REG_SZ as
	// UTF-16 and reports dwSize in BYTES. The buffer must therefore be TCHAR
	// (not BYTE), and its capacity is given in bytes. The old code used a BYTE
	// buffer and `szValue = szBuf`, which made CStringW reinterpret the wide
	// bytes as ANSI and truncate at the first embedded NUL - e.g. the InstallDir
	// "C:\..." collapsed to "C", breaking every install-dir-relative lookup
	// (syntax specs, color scheme, templates, schemes, ...).
	DWORD dwType, dwSize = sizeof(TCHAR) * MAX_PATH; TCHAR szBuf[MAX_PATH]; HKEY hKey;
	szBuf[0] = _T('\0');
	if( RegOpenKeyEx(hRoot, lpszRegPath, 0, KEY_QUERY_VALUE, & hKey) != ERROR_SUCCESS ) return FALSE;
	LONG lResult = RegQueryValueEx(hKey, lpszValName, 0, & dwType, (BYTE *)szBuf, & dwSize);
	RegCloseKey(hKey);
	if( lResult != ERROR_SUCCESS ) return FALSE;
	// RegQueryValueEx does not guarantee the value is null-terminated.
	DWORD dwChars = dwSize / sizeof(TCHAR);
	if( dwChars >= MAX_PATH ) dwChars = MAX_PATH - 1;
	szBuf[dwChars] = _T('\0');
	szValue = szBuf;
	return TRUE;
}

BOOL SetRegKeyValue(HKEY hRoot, LPCTSTR lpszRegPath, LPCTSTR lpszValName, LPCTSTR lpszValue)
{
	DWORD dwType, dwDisposition; TCHAR szBuf[MAX_PATH]; HKEY hKey;
	dwType = REG_SZ; szBuf[0] = '\0';
	if( RegCreateKeyEx(hRoot, lpszRegPath, 0, szBuf, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, & hKey, & dwDisposition) != ERROR_SUCCESS ) return FALSE;
	// cbData is in BYTES; under _UNICODE each TCHAR is 2 bytes, so the +1 for
	// the null terminator must also be scaled or the value is written truncated.
	if( RegSetValueEx(hKey, lpszValName, 0, dwType, (const BYTE *)lpszValue, (DWORD)((_tcslen(lpszValue)+1) * sizeof(TCHAR))) != ERROR_SUCCESS ) return FALSE;
	if( RegCloseKey(hKey) != ERROR_SUCCESS ) return FALSE;
	return TRUE;
}

BOOL DeleteRegKey(HKEY hRoot, LPCTSTR lpszRegPath)
{
	if( RegDeleteKey(hRoot, lpszRegPath) != ERROR_SUCCESS ) TRUE; // return FALSE;
	return TRUE;
}

BOOL DeleteRegValue(HKEY hRoot, LPCTSTR lpszRegPath, LPCTSTR lpszValName)
{
	HKEY hKey;
	if( RegOpenKeyEx(hRoot, lpszRegPath, 0, KEY_WRITE, & hKey) != ERROR_SUCCESS ) return FALSE;
	if( RegDeleteValue(hKey, lpszValName) != ERROR_SUCCESS ) TRUE; //return FALSE;
	if( RegCloseKey(hKey) != ERROR_SUCCESS ) return FALSE;
	return TRUE;
}

BOOL RegisterInProcServer(LPCTSTR lpszClsID, LPCTSTR lpszProgID, LPCTSTR lpszServerPath)
{
	CString szRegPath;
	szRegPath.Format(_T("CLSID\\%s"), lpszClsID);
	if( ! SetRegKeyValue(HKEY_CLASSES_ROOT, szRegPath, _T(""), lpszProgID) ) return FALSE;
	szRegPath.Format(_T("CLSID\\%s\\InProcServer32"), lpszClsID);
	if( ! SetRegKeyValue(HKEY_CLASSES_ROOT, szRegPath, _T(""), lpszServerPath) ) return FALSE;
	if( ! SetRegKeyValue(HKEY_CLASSES_ROOT, szRegPath, _T("ThreadingModel"), _T("Apartment")) ) return FALSE;
	szRegPath.Format(_T("CLSID\\%s\\ProgID"), lpszClsID);
	if( ! SetRegKeyValue(HKEY_CLASSES_ROOT, szRegPath, _T(""), lpszProgID) ) return FALSE;
	szRegPath.Format(_T("%s"), lpszProgID);
	if( ! SetRegKeyValue(HKEY_CLASSES_ROOT, szRegPath, _T(""), lpszProgID) ) return FALSE;
	szRegPath.Format(_T("%s\\CLSID"), lpszProgID);
	if( ! SetRegKeyValue(HKEY_CLASSES_ROOT, szRegPath, _T(""), lpszClsID) ) return FALSE;
	return TRUE;
}

BOOL UnregisterInProcServer(LPCTSTR lpszClsID, LPCTSTR lpszProgID)
{
	CString szRegPath;
	szRegPath.Format(_T("%s\\CLSID"), lpszProgID);
	if( ! DeleteRegKey(HKEY_CLASSES_ROOT, szRegPath) ) return FALSE;
	szRegPath.Format(_T("%s"), lpszProgID);
	if( ! DeleteRegKey(HKEY_CLASSES_ROOT, szRegPath) ) return FALSE;
	szRegPath.Format(_T("CLSID\\%s\\ProgID"), lpszClsID);
	if( ! DeleteRegKey(HKEY_CLASSES_ROOT, szRegPath) ) return FALSE;
	szRegPath.Format(_T("CLSID\\%s\\InProcServer32"), lpszClsID);
	if( ! DeleteRegKey(HKEY_CLASSES_ROOT, szRegPath) ) return FALSE;
	szRegPath.Format(_T("CLSID\\%s"), lpszClsID);
	if( ! DeleteRegKey(HKEY_CLASSES_ROOT, szRegPath) ) return FALSE;
	return TRUE;
}


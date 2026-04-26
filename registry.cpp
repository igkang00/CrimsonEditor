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
	DWORD dwType, dwSize = MAX_PATH; BYTE szBuf[MAX_PATH]; HKEY hKey;
	if( RegOpenKeyEx(hRoot, lpszRegPath, 0, KEY_QUERY_VALUE, & hKey) != ERROR_SUCCESS ) return FALSE;
	if( RegQueryValueEx(hKey, lpszValName, 0, & dwType, szBuf, & dwSize) != ERROR_SUCCESS ) return FALSE;
	if( RegCloseKey(hKey) != ERROR_SUCCESS ) return FALSE;
	szValue = szBuf;
	return TRUE;
}

BOOL SetRegKeyValue(HKEY hRoot, LPCTSTR lpszRegPath, LPCTSTR lpszValName, LPCTSTR lpszValue)
{
	DWORD dwType, dwDisposition; TCHAR szBuf[MAX_PATH]; HKEY hKey;
	dwType = REG_SZ; szBuf[0] = '\0';
	if( RegCreateKeyEx(hRoot, lpszRegPath, 0, szBuf, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, & hKey, & dwDisposition) != ERROR_SUCCESS ) return FALSE;
	if( RegSetValueEx(hKey, lpszValName, 0, dwType, (const BYTE *)lpszValue, strlen(lpszValue)+1) != ERROR_SUCCESS ) return FALSE;
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
	szRegPath.Format("CLSID\\%s", lpszClsID);
	if( ! SetRegKeyValue(HKEY_CLASSES_ROOT, szRegPath, "", lpszProgID) ) return FALSE;
	szRegPath.Format("CLSID\\%s\\InProcServer32", lpszClsID);
	if( ! SetRegKeyValue(HKEY_CLASSES_ROOT, szRegPath, "", lpszServerPath) ) return FALSE;
	if( ! SetRegKeyValue(HKEY_CLASSES_ROOT, szRegPath, "ThreadingModel", "Apartment") ) return FALSE;
	szRegPath.Format("CLSID\\%s\\ProgID", lpszClsID);
	if( ! SetRegKeyValue(HKEY_CLASSES_ROOT, szRegPath, "", lpszProgID) ) return FALSE;
	szRegPath.Format("%s", lpszProgID);
	if( ! SetRegKeyValue(HKEY_CLASSES_ROOT, szRegPath, "", lpszProgID) ) return FALSE;
	szRegPath.Format("%s\\CLSID", lpszProgID);
	if( ! SetRegKeyValue(HKEY_CLASSES_ROOT, szRegPath, "", lpszClsID) ) return FALSE;
	return TRUE;
}

BOOL UnregisterInProcServer(LPCTSTR lpszClsID, LPCTSTR lpszProgID)
{
	CString szRegPath;
	szRegPath.Format("%s\\CLSID", lpszProgID);
	if( ! DeleteRegKey(HKEY_CLASSES_ROOT, szRegPath) ) return FALSE;
	szRegPath.Format("%s", lpszProgID);
	if( ! DeleteRegKey(HKEY_CLASSES_ROOT, szRegPath) ) return FALSE;
	szRegPath.Format("CLSID\\%s\\ProgID", lpszClsID);
	if( ! DeleteRegKey(HKEY_CLASSES_ROOT, szRegPath) ) return FALSE;
	szRegPath.Format("CLSID\\%s\\InProcServer32", lpszClsID);
	if( ! DeleteRegKey(HKEY_CLASSES_ROOT, szRegPath) ) return FALSE;
	szRegPath.Format("CLSID\\%s", lpszClsID);
	if( ! DeleteRegKey(HKEY_CLASSES_ROOT, szRegPath) ) return FALSE;
	return TRUE;
}


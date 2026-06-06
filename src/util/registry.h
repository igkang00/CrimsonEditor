#ifndef __REGISTRY_H_
#define __REGISTRY_H_


BOOL GetRegKeyValue(HKEY hRoot, LPCTSTR lpszRegPath, LPCTSTR lpszValName, TCHAR * lpData, DWORD dwSize);
BOOL GetRegKeyValue(HKEY hRoot, LPCTSTR lpszRegPath, LPCTSTR lpszValName, CString & szValue);
BOOL SetRegKeyValue(HKEY hRoot, LPCTSTR lpszRegPath, LPCTSTR lpszValName, LPCTSTR lpszValue);

BOOL DeleteRegKey(HKEY hRoot, LPCTSTR lpszRegPath);
BOOL DeleteRegValue(HKEY hRoot, LPCTSTR lpszRegPath, LPCTSTR lpszValName);

BOOL RegisterInProcServer(LPCTSTR lpszClsID, LPCTSTR lpszProgID, LPCTSTR lpszServerPath);
BOOL UnregisterInProcServer(LPCTSTR lpszClsID, LPCTSTR lpszProgID);


#endif // __REGISTRY_H_

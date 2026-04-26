#ifndef __PATH_NAME_
#define __PATH_NAME_


BOOL ParseFileFilter(CStringArray & arrDescription, CStringArray & arrExtension, LPCTSTR lpszFilter);
BOOL MatchFileFilter(LPCTSTR lpszPathName, LPCTSTR lpszFilter);

BOOL VerifyPathName(LPCTSTR lpszPathName); // verify file path or directory path
BOOL VerifyFilePath(LPCTSTR lpszPathName); // verify only file path
BOOL FindAllFilePath(CStringArray & arrPathName, LPCTSTR lpszPathName);

CString QuotePathName(LPCTSTR lpszPathName); // quote pathname
CString ChopDirectory(LPCTSTR lpszDirectory); // remove trailing backslash

CString RemotePathToLocalPath(LPCTSTR lpszPathName); // convert remote pathname to local pathname
CString LocalPathToRemotePath(LPCTSTR lpszPathName); // convert local pathname to remote pathname

CString GetFileDirectory(LPCTSTR lpszPathName);
CString GetFileName(LPCTSTR lpszPathName);
CString GetFileTitle(LPCTSTR lpszPathName);
CString GetFileExtension(LPCTSTR lpszPathName);

CString GetShortPathName(LPCTSTR lpszPathName);
CString GetLongPathName(LPCTSTR lpszPathName);

BOOL TouchFile(LPCTSTR lpszPathName);


#endif // __PATH_NAME_
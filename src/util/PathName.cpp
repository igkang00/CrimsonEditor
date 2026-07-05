#include "stdafx.h"
#include "PathName.h"


BOOL ParseFileFilter(CStringArray & arrDescription, CStringArray & arrExtension, LPCTSTR lpszFilter)
{
	TCHAR szFilter[4096]; _tcscpy( szFilter, lpszFilter );
	if( szFilter[_tcslen(szFilter)-1] != '|' ) _tcscat( szFilter, _T("|") );

	arrDescription.RemoveAll(); arrExtension.RemoveAll();
	TCHAR * pFilter = szFilter; INT nLen = (INT)_tcslen(szFilter);
	BOOL bDescription = TRUE; CString szDescription;

	for(INT i = 0; i < nLen; i++) {
		if( szFilter[i] == '|' ) {
			szFilter[i] = '\0';

			if( bDescription ) {
				szDescription = pFilter;
				bDescription = FALSE;
			} else {
				if( _tcslen(szDescription) && _tcslen(pFilter) ) {
					arrDescription.Add( szDescription );
					arrExtension.Add( pFilter ); 
				}
				bDescription = TRUE;
			}

			pFilter += (INT)_tcslen(pFilter) + 1;
		}
	}

	return TRUE;
}

BOOL MatchFileFilter(LPCTSTR lpszPath, LPCTSTR lpszFilter)
{
	// Allocate on the heap to fit any filter length plus the trailing _T(";\0").
	// The previous 256-byte stack buffer would overrun on long user-defined
	// filters (no upper bound exists on the filter input).
	INT nFilterLen = lstrlen(lpszFilter);
	TCHAR * szFilter = new TCHAR[nFilterLen + 2];
	_tcscpy( szFilter, lpszFilter );
	if( nFilterLen == 0 || szFilter[nFilterLen - 1] != ';' ) _tcscat( szFilter, _T(";") );

	TCHAR * pFilter = szFilter; INT nLen = (INT)_tcslen(szFilter);
	BOOL bMatch = FALSE;

	for(INT i = 0; i < nLen; i++) {
		if( szFilter[i] == ';' ) {
			szFilter[i] = '\0';
			INT nFilter = (INT)_tcslen( pFilter );
			INT nPath = (INT)_tcslen( lpszPath );

			if( ! _tcscmp(pFilter, _T("*.*")) ) { bMatch = TRUE; break; }
			if( ! _tcsncmp(pFilter, _T("*."), 2) && (nPath >= nFilter-1)
				&& ! _tcsicmp(pFilter+1, lpszPath+nPath-(nFilter-1)) ) { bMatch = TRUE; break; }

			pFilter += (INT)_tcslen(pFilter) + 1;
		}
	}

	delete [] szFilter;
	return bMatch;
}

BOOL VerifyPathName(LPCTSTR lpszPath)
{
	CFileFind find;
	return find.FindFile(lpszPath);
}

BOOL VerifyFilePath(LPCTSTR lpszPath)
{
	CFileFind find;
	BOOL bFound = find.FindFile(lpszPath);

	while( bFound ) {
		bFound = find.FindNextFile();
		if( ! find.IsDirectory() ) return TRUE;
	}
	return FALSE;
}

BOOL FindAllFilePath(CStringArray & arrPath, LPCTSTR lpszPath)
{
	CFileFind find; BOOL bResult = FALSE;
	BOOL bFound = find.FindFile(lpszPath);

	while( bFound ) {
		bFound = find.FindNextFile();
		if( ! find.IsDirectory() && ! find.IsHidden() ) { 
			arrPath.Add(find.GetFilePath());
			bResult = TRUE;
		}
	}
	return bResult;
}


CString QuotePathName(LPCTSTR lpszPathName)
{
	INT nLen = (INT)_tcslen(lpszPathName);
	CString szPathName;

	if( nLen >= 2 && lpszPathName[0] == _T('\"') && lpszPathName[nLen-1] == _T('\"') ) szPathName = lpszPathName;
	else szPathName.Format(_T("\"%s\""), lpszPathName);

	return szPathName;
}


CString ChopDirectory(LPCTSTR lpszDirectory)
{
	INT nLen = (INT)_tcslen(lpszDirectory);
	CString szDirectory = lpszDirectory;  

	if( nLen >= 1 && lpszDirectory[nLen-1] == '\\' ) return szDirectory.Mid(0, nLen-1);
	else return szDirectory;
}


CString RemotePathToLocalPath(LPCTSTR lpszPathName)
{
	CString szPathName = lpszPathName;

	szPathName.Replace( _T("%"), _T("%25") );	szPathName.Replace( '/', '\\'  );
	szPathName.Replace( _T("*"), _T("%2A") );	szPathName.Replace( _T("?"), _T("%3F") );

	return szPathName;
}


CString LocalPathToRemotePath(LPCTSTR lpszPathName)
{
	CString szPathName = lpszPathName;

	szPathName.Replace( _T("%2A"), _T("*") );	szPathName.Replace( _T("%3F"), _T("?") );
	szPathName.Replace( _T("%25"), _T("%") );	szPathName.Replace( '\\' , '/' );

	return szPathName;
}


CString GetFileDirectory(LPCTSTR lpszPath)
{
	CString szTemp = lpszPath; 
	INT nLen = szTemp.GetLength(); if( ! nLen ) return _T("");
	if( szTemp[nLen-1] == '\\' ) szTemp.SetAt(nLen-1, '\0');
	if( szTemp[nLen-1] == '/'  ) szTemp.SetAt(nLen-1, '\0');

	INT nPos = szTemp.ReverseFind( '\\' );
	if( nPos <  0 ) nPos = szTemp.ReverseFind( '/' );

	if( nPos >= 0 ) return szTemp.Mid( 0, nPos );
	return _T("");
}

CString GetFileName(LPCTSTR lpszPath)
{
	CString szTemp = lpszPath; 
	INT nLen = szTemp.GetLength(); if( ! nLen ) return _T("");
	if( szTemp[nLen-1] == '\\' ) szTemp.SetAt(nLen-1, '\0');
	if( szTemp[nLen-1] == '/'  ) szTemp.SetAt(nLen-1, '\0');

	INT nPos = szTemp.ReverseFind( '\\' );
	if( nPos <  0 ) nPos = szTemp.ReverseFind( '/' );

	if( nPos >= 0 ) return szTemp.Mid( nPos + 1 );
	return lpszPath;
}

CString GetFileTitle(LPCTSTR lpszPath)
{
	CString szTemp = GetFileName(lpszPath);
	INT nPos = szTemp.ReverseFind( '.' );
	if( nPos >= 0 ) return szTemp.Mid( 0, nPos );
	else return szTemp;
}

CString GetFileExtension(LPCTSTR lpszPath)
{
	CString szTemp = GetFileName(lpszPath);
	INT nPos = szTemp.ReverseFind( '.' );
	if( nPos >= 0 ) return szTemp.Mid( nPos );
	else return _T("");
}

CString GetShortPathName(LPCTSTR lpszPath)
{
	TCHAR szShortPath[2048]; szShortPath[0] = '\0';
	GetShortPathName(lpszPath, szShortPath, 2048);
	return szShortPath;
}

/* not compatable in Win95 & WinNT
CString GetLongPathName(LPCTSTR lpszPath)
{
	TCHAR szLongPath[2048]; szLongPath[0] = '\0';
	GetLongPathName(lpszPath, szLongPath, 2048);
	return szLongPath;
} */

CString GetLongPathName(LPCTSTR lpszPath)
{
	CString szTemp, szFile, szPath = lpszPath, szLongPath;
	WIN32_FIND_DATA findData; HANDLE hFind; BOOL bDir;

	INT nFwd, nIdx, nLen = szPath.GetLength();
	if( nLen < 3 ) return szPath;

	if( szPath[0] == '\\' ) { // it's UNC path name
		nIdx = 2; // skip first two backslashes
		nIdx = szPath.Find('\\', nIdx) + 1; // skip UNC server name
		nIdx = szPath.Find('\\', nIdx) + 1; // skip UNC share name
	} else { // it's normal path name
		nIdx = 3; // skip drive letter
	}

	// get UNC share name or drive name 
	szLongPath = szPath.Left(nIdx);

	while( nIdx < nLen ) {
		nFwd = nIdx; while( nFwd < nLen && szPath[nFwd] != '\\' ) nFwd++;

		szTemp = szPath.Left(nFwd);
		szFile = szPath.Mid(nIdx, nFwd-nIdx);

		if( nFwd < nLen && szPath[nFwd] == '\\' ) { bDir = TRUE; nIdx = nFwd+1; }
		else { bDir = FALSE; nIdx = nFwd; }

		if( ! szFile.Compare(_T(".") ) ) {
			// do nothing
		} else if( ! szFile.Compare(_T("..")) ) {
			INT nPre = szLongPath.GetLength() - 2; // skip last backslash
			while( nPre >= 0 && szLongPath[nPre] != '\\' ) nPre--;
			szLongPath = szLongPath.Left(nPre) + _T("\\");
		} else {
			hFind = FindFirstFile( szTemp, & findData );
			if( hFind == INVALID_HANDLE_VALUE ) return szPath;
			FindClose( hFind );

			szLongPath += findData.cFileName;
			if( bDir ) szLongPath += _T("\\");
		}
	}

	return szLongPath;
}

BOOL TouchFile(LPCTSTR lpszPath)
{
	CString szTemp, szFile, szPath = lpszPath;
	WIN32_FIND_DATA findData; HANDLE hFind; BOOL bDir;

	INT nFwd, nIdx, nLen = szPath.GetLength();
	if( nLen < 3 ) return FALSE;

	if( szPath[0] == '\\' ) { // it's UNC path name
		nIdx = 2; // skip first two backslashes
		nIdx = szPath.Find('\\', nIdx) + 1; // skip UNC server name
		nIdx = szPath.Find('\\', nIdx) + 1; // skip UNC share name
	} else { // it's normal path name
		nIdx = 3; // skip drive letter
	}

	while( nIdx < nLen ) {
		nFwd = nIdx; while( nFwd < nLen && szPath[nFwd] != '\\' ) nFwd++;

		szTemp = szPath.Left(nFwd);
		szFile = szPath.Mid(nIdx, nFwd-nIdx);

		if( nFwd < nLen && szPath[nFwd] == '\\' ) { bDir = TRUE; nIdx = nFwd+1; }
		else { bDir = FALSE; nIdx = nFwd; }

		hFind = FindFirstFile( szTemp, & findData );
		if( hFind == INVALID_HANDLE_VALUE ) {
			if( ! bDir ) {
				HANDLE hFile = CreateFile( szTemp, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL );
				if( hFile == INVALID_HANDLE_VALUE ) return FALSE;
				CloseHandle( hFile );
			} else if( ! CreateDirectory( szTemp, NULL ) ) {
				return FALSE; 
			}
		} else FindClose( hFind );
	}

	return TRUE;
}


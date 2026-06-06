#include "stdafx.h"
#include "PathName.h"


BOOL ParseFileFilter(CStringArray & arrDescription, CStringArray & arrExtension, LPCTSTR lpszFilter)
{
	TCHAR szFilter[4096]; strcpy( szFilter, lpszFilter );
	if( szFilter[strlen(szFilter)-1] != '|' ) strcat( szFilter, "|" );

	arrDescription.RemoveAll(); arrExtension.RemoveAll();
	TCHAR * pFilter = szFilter; INT nLen = strlen(szFilter);
	BOOL bDescription = TRUE; CString szDescription;

	for(INT i = 0; i < nLen; i++) {
		if( szFilter[i] == '|' ) {
			szFilter[i] = '\0';

			if( bDescription ) {
				szDescription = pFilter;
				bDescription = FALSE;
			} else {
				if( strlen(szDescription) && strlen(pFilter) ) {
					arrDescription.Add( szDescription );
					arrExtension.Add( pFilter ); 
				}
				bDescription = TRUE;
			}

			pFilter += strlen(pFilter) + 1;
		}
	}

	return TRUE;
}

BOOL MatchFileFilter(LPCTSTR lpszPath, LPCTSTR lpszFilter)
{
	TCHAR szFilter[256]; strcpy( szFilter, lpszFilter );
	if( szFilter[strlen(szFilter)-1] != ';' ) strcat( szFilter, ";" );

	TCHAR * pFilter = szFilter; INT nLen = strlen(szFilter);

	for(INT i = 0; i < nLen; i++) {
		if( szFilter[i] == ';' ) {
			szFilter[i] = '\0';
			INT nFilter = strlen( pFilter );
			INT nPath = strlen( lpszPath );

			if( ! strcmp(pFilter, "*.*") ) return TRUE;
			if( ! strncmp(pFilter, "*.", 2) && (nPath >= nFilter-1)
				&& ! stricmp(pFilter+1, lpszPath+nPath-(nFilter-1)) ) return TRUE;

			pFilter += strlen(pFilter) + 1;
		}
	}

	return FALSE;
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
	INT nLen = strlen(lpszPathName);
	CString szPathName;

	if( nLen >= 2 && lpszPathName[0] == '\"' && lpszPathName[nLen-1] == '\"' ) szPathName = lpszPathName;
	else szPathName.Format("\"%s\"", lpszPathName);

	return szPathName;
}


CString ChopDirectory(LPCTSTR lpszDirectory)
{
	INT nLen = strlen(lpszDirectory);
	CString szDirectory = lpszDirectory;  

	if( nLen >= 1 && lpszDirectory[nLen-1] == '\\' ) return szDirectory.Mid(0, nLen-1);
	else return szDirectory;
}


CString RemotePathToLocalPath(LPCTSTR lpszPathName)
{
	CString szPathName = lpszPathName;

	szPathName.Replace( "%", "%25" );	szPathName.Replace( '/', '\\'  );
	szPathName.Replace( "*", "%2A" );	szPathName.Replace( "?", "%3F" );

	return szPathName;
}


CString LocalPathToRemotePath(LPCTSTR lpszPathName)
{
	CString szPathName = lpszPathName;

	szPathName.Replace( "%2A", "*" );	szPathName.Replace( "%3F", "?" );
	szPathName.Replace( "%25", "%" );	szPathName.Replace( '\\' , '/' );

	return szPathName;
}


CString GetFileDirectory(LPCTSTR lpszPath)
{
	CString szTemp = lpszPath; 
	INT nLen = szTemp.GetLength(); if( ! nLen ) return "";
	if( szTemp[nLen-1] == '\\' ) szTemp.SetAt(nLen-1, '\0');
	if( szTemp[nLen-1] == '/'  ) szTemp.SetAt(nLen-1, '\0');

	INT nPos = szTemp.ReverseFind( '\\' );
	if( nPos <  0 ) nPos = szTemp.ReverseFind( '/' );

	if( nPos >= 0 ) return szTemp.Mid( 0, nPos );
	return "";
}

CString GetFileName(LPCTSTR lpszPath)
{
	CString szTemp = lpszPath; 
	INT nLen = szTemp.GetLength(); if( ! nLen ) return "";
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
	else return "";
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

		if( ! szFile.Compare("." ) ) {
			// do nothing
		} else if( ! szFile.Compare("..") ) {
			INT nPre = szLongPath.GetLength() - 2; // skip last backslash
			while( nPre >= 0 && szLongPath[nPre] != '\\' ) nPre--;
			szLongPath = szLongPath.Left(nPre) + "\\";
		} else {
			hFind = FindFirstFile( szTemp, & findData );
			if( hFind == INVALID_HANDLE_VALUE ) return szPath;
			FindClose( hFind );

			szLongPath += findData.cFileName;
			if( bDir ) szLongPath += "\\";
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


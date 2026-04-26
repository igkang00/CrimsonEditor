#include "stdafx.h"
#include "cedtHeader.h"



BOOL CCedtDoc::DetectSyntaxType(LPCTSTR lpszPathName, LPCTSTR lpszFirstLine)
{
	CString szExtension = GetFileExtension(lpszPathName);
	CString szLinkFilePath = CCedtApp::m_szInstallDirectory + "\\link\\extension" + szExtension;

	if( szExtension.GetLength() && VerifyFilePath(szLinkFilePath) ) {
		return ReadExtensionLinkFile( szLinkFilePath );
	} else if( SearchLinkFileWithFirstLine(lpszFirstLine, szLinkFilePath) ) {
		return ReadExtensionLinkFile( szLinkFilePath );
	} else if( SearchLinkFileWithPathName(lpszPathName, szLinkFilePath) ) {
		return ReadExtensionLinkFile( szLinkFilePath );
	} else return FALSE;
}

BOOL CCedtDoc::SearchLinkFileWithFirstLine(LPCTSTR lpszFirstLine, CString & szLinkFilePath)
{
	CStringArray arrPathName; CString szFirstLine = lpszFirstLine;
	FindAllFilePath( arrPathName, CCedtApp::m_szInstallDirectory + "\\link\\firstline.*" );

	for( INT i = 0; i < arrPathName.GetSize(); i++ ) {
		CString szContain, szLinkPath = arrPathName.GetAt(i);
		if( GetContainsPartOfLinkFile( szLinkPath, szContain) ) {
			szFirstLine.MakeUpper(); szContain.MakeUpper();
			if( szFirstLine.Find( szContain ) >= 0 ) { szLinkFilePath = szLinkPath; return TRUE; }
		}
	}

	return FALSE;
}

BOOL CCedtDoc::SearchLinkFileWithPathName(LPCTSTR lpszPathName, CString & szLinkFilePath)
{
	CStringArray arrPathName; CString szPathName = lpszPathName;
	FindAllFilePath( arrPathName, CCedtApp::m_szInstallDirectory + "\\link\\pathname.*" );

	for( INT i = 0; i < arrPathName.GetSize(); i++ ) {
		CString szContain, szLinkPath = arrPathName.GetAt(i);
		if( GetContainsPartOfLinkFile( szLinkPath, szContain) ) {
			szPathName.MakeUpper(); szContain.MakeUpper();
			if( szPathName.Find( szContain ) >= 0 ) { szLinkFilePath = szLinkPath; return TRUE; }
		}
	}

	return FALSE;
}

BOOL CCedtDoc::GetContainsPartOfLinkFile(LPCTSTR lpszLinkFilePath, CString & szContains)
{
	ifstream fin( lpszLinkFilePath, ios::in | ios::nocreate);
	if( ! fin.is_open() ) return FALSE;

	TCHAR buff[1024], * ptr1, * ptr2;
	while( fin.good() ) {
		fin.getline(buff, 1024);
		if( ! strlen(buff) ) continue;

		ptr1 = strtok(buff, ":");
		ptr2 = strtok(NULL, "\n");

		if( ! _stricmp(ptr1, "CONTAINS") ) {
			szContains = ptr2; fin.close(); return TRUE;
		}
	}

	fin.close();
	return FALSE;
}

BOOL CCedtDoc::ReadExtensionLinkFile(LPCTSTR lpszLinkFilePath) 
{
	m_szLangSpecFile = m_szKeywordsFile = "";

	ifstream fin( lpszLinkFilePath, ios::in | ios::nocreate);
	if( ! fin.is_open() ) return FALSE;

	TCHAR buff[1024], * ptr1, * ptr2;
	while( fin.good() ) {
		fin.getline(buff, 1024);
		if( ! strlen(buff) ) continue;

		ptr1 = strtok(buff, ":");
		ptr2 = strtok(NULL, "\n");

		if( ! _stricmp(ptr1, "SPECIFICATION") ) {
			m_szLangSpecFile = ptr2;
		} else if( ! _stricmp(ptr1, "LANGSPEC") ) {
			m_szLangSpecFile = ptr2;
		} else if( ! _stricmp(ptr1, "KEYWORDS") ) {
			m_szKeywordsFile = ptr2;
		}
	}

	fin.close();
	return ( m_szLangSpecFile.GetLength() && m_szKeywordsFile.GetLength() );
}

BOOL CCedtDoc::LoadSyntaxInformation()
{
	if( m_szLangSpecFile.GetLength() && m_szKeywordsFile.GetLength() ) {
		CString szLangSpecPath = CCedtApp::m_szInstallDirectory + "\\spec\\" + m_szLangSpecFile;
		CString szKeywordsPath = CCedtApp::m_szInstallDirectory + "\\spec\\" + m_szKeywordsFile;

		m_clsLangSpec.FileLoad( szLangSpecPath );
		m_clsKeywords.FileLoad( szKeywordsPath, m_clsLangSpec.m_bCaseSensitive );

		return TRUE;
	} else return FALSE;
}

#include "stdafx.h"
#include "cedtHeader.h"
#include "FindInFilesDialog.h"



void CCedtApp::OnSearchFindInFiles() 
{
	// static dialog box to remember last settings...
	static CFindInFilesDialog dlg;

	CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pFrame );
	if( ! pFrame->CanUseOutputWindow() ) {
		AfxMessageBox(IDS_ERR_OUTPUT_WINDOW_OCCUPIED, MB_OK | MB_ICONSTOP); return;
	}

	CCedtView * pView = (CCedtView *)pFrame->MDIGetActiveView();
	CCedtDoc * pDoc = (pView) ? (CCedtDoc *)pView->GetDocument() : NULL;

	if( pDoc && pView ) {
		dlg.m_szFindString = pView->GetCurrentWord();
		if( pView->IsSelected() && ! pView->GetSelectedLineCount() ) dlg.m_szFindString = pView->GetSelectedString();

		CString szPathName = pDoc->GetPathName();
		if( szPathName.GetLength() && ! pDoc->IsRemoteFile() ) dlg.m_szFolder = GetFileDirectory( szPathName );
	}

	dlg.InitFileTypeList( GetComposedFileFilter() );
	if( dlg.DoModal() != IDOK ) return;

	CString szFindString = dlg.m_szFindString;
	CString szFileType = dlg.m_szFileType;
	CString szFolder = dlg.m_szFolder;

	BOOL bLookInSubfolders = dlg.m_bLookInSubfolders;
	UINT nOptions = COMPOSE_SEARCH_OPTION( dlg.m_bWholeWord, dlg.m_bMatchCase, dlg.m_bRegularExpression);

	CRegExp clsTestRegExp; // compile regular expression for test
	if( SEARCH_REG_EXP(nOptions) && ! clsTestRegExp.RegComp(szFindString) ) {
		CString szMessage; szMessage.Format(IDS_ERR_REG_COMP_FAILED, szFindString);
		AfxMessageBox(szMessage); return; // test failed
	}

	DoFindInFiles(szFindString, szFileType, szFolder, bLookInSubfolders, nOptions); 
}


BOOL CCedtApp::DoFindInFiles(LPCTSTR lpszFindString, LPCTSTR lpszFileType, LPCTSTR lpszFolder, BOOL bLookInSubfolders, UINT nOptions)
{
	CWaitCursor wait; CRegExp clsRegExp; 

	if( SEARCH_REG_EXP(nOptions) ) { // compile regular expression
		CString szExpression = lpszFindString; 			szExpression.Replace( "\\\\", "\x1B" );
		szExpression.Replace( "\\s" , "[ \t\r\n]" );	szExpression.Replace( "\\S" , "[^ \t\r\n]" );
		szExpression.Replace( "\\w" , "[A-Za-z0-9]" );	szExpression.Replace( "\\W" , "[^A-Za-z0-9]" );
		szExpression.Replace( "\\a" , "[A-Za-z]" );		szExpression.Replace( "\\A" , "[^A-Za-z]" );
		szExpression.Replace( "\\d" , "[0-9]" );		szExpression.Replace( "\\D" , "[^0-9]" );
		szExpression.Replace( "\\h" , "[A-Fa-f0-9]" );	szExpression.Replace( "\\H" , "[^A-Fa-f0-9]" );
		szExpression.Replace( "\\t" , "\t" );			szExpression.Replace( "\x1B", "\\\\" );

		if( ! SEARCH_MATCH_CASE(nOptions) ) szExpression.MakeLower();
		if( ! clsRegExp.RegComp( szExpression ) ) return FALSE;
	}

	CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pFrame );
	if( ! pFrame->IsOutputWindowVisible() ) pFrame->ShowOutputWindow(TRUE);

	pFrame->SetOutputWindowOccupied(TRUE);
	pFrame->EnableOutputWindowInput(FALSE);

	pFrame->ClearOutputWindowContents();

	CString szMessage; szMessage.LoadString(IDS_OUT_SEARCH_TITLE);
	pFrame->AddStringToOutputWindow( szMessage, RGB(0, 0, 0) );

	szMessage.Format(IDS_OUT_SEARCH_BEGIN, lpszFindString);
	pFrame->AddStringToOutputWindow( szMessage, RGB(0, 0, 128) );

	INT nFound = FindInFilesInFolder(lpszFindString, lpszFileType, lpszFolder, bLookInSubfolders, nOptions, clsRegExp);

	if( nFound ) szMessage.Format(IDS_OUT_SEARCH_RESULT, nFound);
	else szMessage.Format(IDS_OUT_SEARCH_NOT_FOUND, lpszFindString);
	pFrame->AddStringToOutputWindow( szMessage, RGB(0, 0, 128) );

	pFrame->SetOutputWindowOccupied(FALSE);
	pFrame->EnableOutputWindowInput(FALSE);

	return TRUE;
}

INT CCedtApp::FindInFilesInFolder(LPCTSTR lpszFindString, LPCTSTR lpszFileType, LPCTSTR lpszFolder, BOOL bLookInSubfolders, UINT nOptions, CRegExp & clsRegExp)
{
	CFileFind find; BOOL bFound; INT nFound = 0;

	CString szFolder = lpszFolder; INT nLen = strlen(lpszFolder);
	if( szFolder[nLen-1] != '\\' ) szFolder += "\\";

	bFound = find.FindFile(szFolder + "*.*");
	while( bFound ) {
		bFound = find.FindNextFile();
		if( ! find.IsDirectory() && ! find.IsDots() && ! find.IsHidden() ) {
			CString szFilePath = find.GetFilePath();
			if( ! MatchFileFilter(szFilePath, lpszFileType) ) continue;
			nFound += FindInFilesInFile(lpszFindString, szFilePath, nOptions, clsRegExp);
		}
	}

	// do not look in sub folders
	if( ! bLookInSubfolders ) return nFound;

	bFound = find.FindFile(szFolder + "*.*");
	while( bFound ) {
		bFound = find.FindNextFile();
		if( find.IsDirectory() && ! find.IsDots() && ! find.IsHidden() ) {
			CString szSubFolder = find.GetFilePath();
			nFound += FindInFilesInFolder(lpszFindString, lpszFileType, szSubFolder, bLookInSubfolders, nOptions, clsRegExp);
		}
	}

	// return total sum of the count
	return nFound;
}

INT CCedtApp::FindInFilesInFile(LPCTSTR lpszFindString, LPCTSTR lpszFilePath, UINT nOptions, CRegExp & clsRegExp)
{
	CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pFrame );
	CFile file; CString szMessage; INT nFound = 0;

	if( file.Open( lpszFilePath, CFile::modeRead | CFile::shareDenyNone ) ) {
		CArchive archive( & file, CArchive::load );
		CString szString; INT nIdxX, nIdxY = 0;

		while( archive.ReadString( szString ) ) {
			if( szString.FindOneOf( CONTROL_CHARS1 ) >= 0 ) {
			//	szMessage.Format(IDS_OUT_SEARCH_SKIP_BINARY, lpszFilePath);
			//	pFrame->AddStringToOutputWindow( szMessage, RGB(128, 0, 0) ); // do not display message
				break;
			} else if( szString.GetLength() >= MAX_STRING_SIZE ) {
				szMessage.Format(IDS_OUT_SEARCH_LINE_EXCEED, MAX_STRING_SIZE, lpszFilePath, nIdxY+1);
				pFrame->AddStringToOutputWindow( szMessage, RGB(128, 0, 0) );
			} else {
				if( ! SEARCH_REG_EXP(nOptions) ) nIdxX = ::ForwardFindString(szString, lpszFindString, 0, SEARCH_WHOLE_WORD(nOptions), SEARCH_MATCH_CASE(nOptions));
				else nIdxX = ::ForwardFindStringRegExp(szString, lpszFindString, clsRegExp, 0, SEARCH_WHOLE_WORD(nOptions), SEARCH_MATCH_CASE(nOptions));
				if( nIdxX >= 0 ) { // found it!
					szMessage.Format("%s(%d,%d): %s", lpszFilePath, nIdxY+1, nIdxX+1, szString);
					pFrame->AddStringToOutputWindow( szMessage, RGB(0, 0, 0) ); nFound++;
				}
			}
			nIdxY++;
		}

		archive.Close();
		file.Close();
	} else {
		szMessage.Format(IDS_OUT_SEARCH_CANNOT_OPEN, lpszFilePath);
		pFrame->AddStringToOutputWindow( szMessage, RGB(128, 0, 0) );
	}

	return nFound;
}


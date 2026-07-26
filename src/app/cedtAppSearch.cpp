#include "stdafx.h"
#include "cedtHeader.h"
#include "FindInFilesDialog.h"


// Find in Files runs the whole scan on the UI thread. Without pumping, a large folder — or a
// single huge file — leaves the window "Not Responding" until it finishes, so the user force-
// quits mid-search. Pump the pending messages periodically so paint and input keep working, and
// let Esc abort. A nested Find in Files cannot start during the pump: the output window is marked
// occupied (see DoFindInFiles) and OnSearchFindInFiles refuses while it is.
static BOOL _bAbortFindInFiles = FALSE;

static void _PumpFindInFilesMessages()
{
	MSG msg;
	while( ::PeekMessage( & msg, NULL, 0, 0, PM_REMOVE ) ) {
		if( msg.message == WM_QUIT ) {           // app is closing — stop and let it
			_bAbortFindInFiles = TRUE;
			::PostQuitMessage( (int)msg.wParam );
			return;
		}
		::TranslateMessage( & msg );
		::DispatchMessage( & msg );
	}
	if( ::GetAsyncKeyState( VK_ESCAPE ) & 0x8000 ) _bAbortFindInFiles = TRUE;
}


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
		CString szExpression = lpszFindString; 			szExpression.Replace( _T("\\\\"), _T("\x1B") );
		szExpression.Replace( _T("\\s") , _T("[ \t\r\n]") );	szExpression.Replace( _T("\\S") , _T("[^ \t\r\n]") );
		szExpression.Replace( _T("\\w") , _T("[A-Za-z0-9]") );	szExpression.Replace( _T("\\W") , _T("[^A-Za-z0-9]") );
		szExpression.Replace( _T("\\a") , _T("[A-Za-z]") );		szExpression.Replace( _T("\\A") , _T("[^A-Za-z]") );
		szExpression.Replace( _T("\\d") , _T("[0-9]") );		szExpression.Replace( _T("\\D") , _T("[^0-9]") );
		szExpression.Replace( _T("\\h") , _T("[A-Fa-f0-9]") );	szExpression.Replace( _T("\\H") , _T("[^A-Fa-f0-9]") );
		szExpression.Replace( _T("\\t") , _T("\t") );			szExpression.Replace( _T("\x1B"), _T("\\\\") );

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

	_bAbortFindInFiles = FALSE;
	INT nFound = FindInFilesInFolder(lpszFindString, lpszFileType, lpszFolder, bLookInSubfolders, nOptions, clsRegExp);

	if( _bAbortFindInFiles ) szMessage.Format(IDS_OUT_SEARCH_CANCELLED, nFound);
	else if( nFound ) szMessage.Format(IDS_OUT_SEARCH_RESULT, nFound);
	else szMessage.Format(IDS_OUT_SEARCH_NOT_FOUND, lpszFindString);
	pFrame->AddStringToOutputWindow( szMessage, RGB(0, 0, 128) );

	// The output window keeps only the most recent OUTPUT_MAX_LINE_COUNT lines. When a search
	// matches more than that, the earliest results have already scrolled out of the buffer, so
	// say so — otherwise the count above and the visible list silently disagree.
	if( pFrame->WereOutputWindowContentsTruncated() ) {
		szMessage.Format(IDS_OUT_SEARCH_TRUNCATED, OUTPUT_MAX_LINE_COUNT);
		pFrame->AddStringToOutputWindow( szMessage, RGB(128, 0, 0) );
	}

	pFrame->SetOutputWindowOccupied(FALSE);
	pFrame->EnableOutputWindowInput(FALSE);

	return TRUE;
}

INT CCedtApp::FindInFilesInFolder(LPCTSTR lpszFindString, LPCTSTR lpszFileType, LPCTSTR lpszFolder, BOOL bLookInSubfolders, UINT nOptions, CRegExp & clsRegExp)
{
	CFileFind find; BOOL bFound; INT nFound = 0;

	CString szFolder = lpszFolder; INT nLen = (INT)_tcslen(lpszFolder);
	if( szFolder[nLen-1] != _T('\\') ) szFolder += _T("\\");

	bFound = find.FindFile(szFolder + _T("*.*"));
	while( bFound ) {
		bFound = find.FindNextFile();
		if( _bAbortFindInFiles ) return nFound;
		if( ! find.IsDirectory() && ! find.IsDots() && ! find.IsHidden() ) {
			CString szFilePath = find.GetFilePath();
			if( ! MatchFileFilter(szFilePath, lpszFileType) ) continue;
			nFound += FindInFilesInFile(lpszFindString, szFilePath, nOptions, clsRegExp);
		}
	}

	// do not look in sub folders
	if( ! bLookInSubfolders || _bAbortFindInFiles ) return nFound;

	bFound = find.FindFile(szFolder + _T("*.*"));
	while( bFound ) {
		bFound = find.FindNextFile();
		if( _bAbortFindInFiles ) return nFound;
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
	CString szMessage; INT nFound = 0;

	// Read with the same encoding-aware, block-at-a-time loader the editor uses to open a file,
	// NOT CArchive::ReadString. In a Unicode build ReadString scans for L'\n' in sizeof(TCHAR)
	// units, so a byte-encoded file (ANSI / UTF-8 — most files) has no such unit and the whole
	// file comes back as one gigantic "line". That single call froze the app on a large file
	// (no pump point inside it), and the over-long line was then skipped, so big files could not
	// be searched at all. CMemText::FileLoad detects the encoding, splits lines correctly, and
	// returns fast, which also makes the periodic pump below effective.
	CMemText text;
	if( ! text.FileLoad( lpszFilePath ) ) {
		szMessage.Format(IDS_OUT_SEARCH_CANNOT_OPEN, lpszFilePath);
		pFrame->AddStringToOutputWindow( szMessage, RGB(128, 0, 0) );
		return 0;
	}

	INT nIdxX, nIdxY = 0;
	POSITION pos = text.GetHeadPosition();
	while( pos ) {
		// Keep the UI alive on a large file, and let Esc abort. The scan is fast now, but a file
		// of hundreds of thousands of lines still deserves a pump.
		if( (nIdxY & 0x0FFF) == 0 ) {
			_PumpFindInFilesMessages();
			if( _bAbortFindInFiles ) break;
		}

		const CString & szString = text.GetNext( pos );

		if( szString.FindOneOf( CONTROL_CHARS1 ) >= 0 ) {
			break;	// looks binary — stop scanning this file
		} else if( szString.GetLength() >= MAX_STRING_LENGTH ) {
			szMessage.Format(IDS_OUT_SEARCH_LINE_EXCEED, MAX_STRING_LENGTH, lpszFilePath, nIdxY+1);
			pFrame->AddStringToOutputWindow( szMessage, RGB(128, 0, 0) );
		} else {
			if( ! SEARCH_REG_EXP(nOptions) ) nIdxX = ::ForwardFindString(szString, lpszFindString, 0, SEARCH_WHOLE_WORD(nOptions), SEARCH_MATCH_CASE(nOptions));
			else nIdxX = ::ForwardFindStringRegExp(szString, lpszFindString, clsRegExp, 0, SEARCH_WHOLE_WORD(nOptions), SEARCH_MATCH_CASE(nOptions));
			if( nIdxX >= 0 ) { // found it!
				szMessage.Format(_T("%s(%d,%d): %s"), lpszFilePath, nIdxY+1, nIdxX+1, szString);
				pFrame->AddStringToOutputWindow( szMessage, RGB(0, 0, 0) ); nFound++;
			}
		}
		nIdxY++;
	}

	return nFound;
}


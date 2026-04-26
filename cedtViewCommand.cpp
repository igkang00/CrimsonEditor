#include "stdafx.h"
#include "cedtHeader.h"
#include "UserInputDialog.h"
#include "SortStringArray.h"
#include "FolderDialog.h"
#include "HtmlHelp.h"


BOOL CCedtView::RefreshUserCommandFilePathForMenu()
{
	for( INT i = 0; i < 8; i++ ) m_szUserCommandFilePath[i] = "";

	CSortStringArray arrPathName;
	CString szPathName = CCedtApp::m_szInstallDirectory + "\\tools\\*.tools";

	BOOL bFound = FindAllFilePath(arrPathName, szPathName);
	arrPathName.QuickSort();

	INT size = arrPathName.GetSize(); if( size > 8 ) size = 8;
	for( i = 0; i < size; i++ ) m_szUserCommandFilePath[i] = arrPathName[i];

	return TRUE;
}


CString CCedtView::GetCommandName(INT nCommand)
{
	CUserCommand & rCommand = m_clsUserCommand[nCommand];
	if( rCommand.m_szName.GetLength() ) return rCommand.m_szName;
	else return "- Empty -";
}

CString CCedtView::GetCommandHotKeyText(INT nCommand)
{
	CUserCommand & rCommand = m_clsUserCommand[nCommand];
	CString szHotKeyText = rCommand.GetHotKeyText();
	if( ! szHotKeyText.GetLength() ) szHotKeyText.Format("Ctrl+%d", nCommand % 10);
	return szHotKeyText;
}

BOOL CCedtView::SetChildInputString(LPCTSTR lpszInputString)
{
	m_arrChildInputString.Add( lpszInputString );
	return TRUE;
}

BOOL CCedtView::KillChildProcess()
{
	if( ! TerminateProcess( m_hChildProcess, 1 ) ) return FALSE;
	return TRUE;
}

BOOL CCedtView::TranslateMessageForUserCommand(MSG * pMsg)
{
	if( pMsg->message != WM_KEYDOWN && pMsg->message != WM_SYSKEYDOWN ) return FALSE;

	UINT nFlags = GetKeyState();
	BOOL bShift = (nFlags & KEYSTATE_SHIFT) ? TRUE : FALSE;
	BOOL bCtrl = (nFlags & KEYSTATE_CONTROL) ? TRUE : FALSE;
	BOOL bAlt = (nFlags & KEYSTATE_MENU) ? TRUE : FALSE;

	for( INT i = 1; i <= 10; i++ ) {
		CUserCommand & rCommand = m_clsUserCommand[i];
		BOOL bShift2 = (rCommand.m_wModifiers & HOTKEYF_SHIFT) ? TRUE : FALSE;
		BOOL bCtrl2 = (rCommand.m_wModifiers & HOTKEYF_CONTROL) ? TRUE : FALSE;
		BOOL bAlt2 = (rCommand.m_wModifiers & HOTKEYF_ALT) ? TRUE : FALSE;

		if( rCommand.m_wVirtualKeyCode != pMsg->wParam ) continue;
		if( ! rCommand.IsAssigned() ) continue;
		if( bShift != bShift2 || bCtrl != bCtrl2 || bAlt != bAlt2 ) continue;

		return PostMessage(WM_COMMAND, ID_COMMAND_EXECUTE0 + i, 0L);
	}

	return FALSE;
}

BOOL CCedtView::ExecuteCommand(INT nCommand)
{
	CString szCommand = m_clsUserCommand[nCommand].m_szCommand;
	CString szArgument = m_clsUserCommand[nCommand].m_szArgument;
	CString szDirectory = m_clsUserCommand[nCommand].m_szDirectory;

	BOOL bCloseOnExit = m_clsUserCommand[nCommand].m_bCloseOnExit;
	BOOL bUseShortFileName = m_clsUserCommand[nCommand].m_bUseShortFileName;
	BOOL bCaptureOutput = m_clsUserCommand[nCommand].m_bCaptureOutput;

	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	CCedtApp * pApp = (CCedtApp *)AfxGetApp();
	szCommand.TrimRight(); szCommand.TrimLeft();

	CString szPathName = pDoc->GetPathName();
	if( bUseShortFileName ) szPathName = GetShortPathName( szPathName );
	CString szProjectPath = pApp->GetProjectPathName();

	CString szLineNum; szLineNum.Format("%d", GetCurrentLineNumber());
	CString szCurrWord = GetCurrentWord();
	if( m_bSelected ) szCurrWord = GetSelectedString();

	CString szUserInput, szSelectPath1, szSelectPath2, szSelectDir1, szSelectDir2;
	if( szArgument.Find("$(UserInput)")    >= 0 ) { if( ! AskUserInputArguments( szUserInput ) ) return FALSE; }
	if( szArgument.Find("$(SelectPath1)")  >= 0 ) { if( ! AskUserSelectFilePath( pDoc->GetPathName(), szSelectPath1 ) ) return FALSE; }
	if( szArgument.Find("$(SelectPath2)")  >= 0 ) { if( ! AskUserSelectFilePath( pDoc->GetPathName(), szSelectPath2 ) ) return FALSE; }
	if( szArgument.Find("$(SelectDir1)")   >= 0 ) { if( ! AskUserSelectDirectory( pDoc->GetPathName(), szSelectDir1 ) ) return FALSE; }
	if( szArgument.Find("$(SelectDir2)")   >= 0 ) { if( ! AskUserSelectDirectory( pDoc->GetPathName(), szSelectDir2 ) ) return FALSE; }

	CMapStringToString clsVariables;
	SetDefaultShellVariables(clsVariables, szPathName, szProjectPath, szLineNum, szCurrWord, szUserInput, szSelectPath1, szSelectPath2, szSelectDir1, szSelectDir2);

	// replace shell variables
	if( szCommand.GetLength() ) ReplaceShellVariables( szCommand, clsVariables );
	TRACE1( "COMMAND EXECUTABLE: '%s'\n", szCommand );

	// replace program argument
	if( szArgument.GetLength() ) ReplaceShellVariables( szArgument, clsVariables );
	TRACE1( "COMMAND ARGUMENTS : '%s'\n", szArgument );

	// replace execute directory
	if( szDirectory.GetLength() ) ReplaceShellVariables( szDirectory, clsVariables );
	INT nLength = szDirectory.GetLength(); 
	if( nLength > 0 && szDirectory[nLength-1] != '\\' ) szDirectory += "\\";
	TRACE1( "COMMAND DIRECTORY : '%s'\n", szDirectory );
	
	// execute command!!!
	CString  szExtension = szCommand.Right(4);
	BOOL bResult = FALSE;
	
	if( ! szExtension.CompareNoCase(".exe") || ! szExtension.CompareNoCase(".com") ||
		! szExtension.CompareNoCase(".bat") || ! szExtension.CompareNoCase(".cmd") ) { // executable file
		bResult = ExecuteExecutable( szCommand, szArgument, szDirectory, bCloseOnExit, bCaptureOutput );
	} else if( ! szExtension.CompareNoCase(".hlp") ) { // windows help file
		bResult = ExecuteWinHelp( szCommand, szArgument );
	} else if( ! szExtension.CompareNoCase(".chm") ) { // html help file
		bResult = ExecuteHtmlHelp( szCommand, szArgument );
	}

	return bResult;
}

BOOL CCedtView::AskUserInputArguments(CString & szUserInput)
{
	CUserInputDialog dlg(IDS_ASKUSR_PROGRAM_ARGS);
	if( dlg.DoModal() != IDOK ) return FALSE;

	szUserInput = dlg.m_szUserInput;
	return TRUE;
}

BOOL CCedtView::AskUserSelectFilePath(LPCTSTR lpszPathName, CString & szSelectPath)
{
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	CString szFilter = CCedtApp::GetComposedFileFilter();
	CFileDialog dlg(TRUE, NULL, lpszPathName, dwFlags, szFilter);

	TCHAR szCurrentDirectory[MAX_PATH]; GetCurrentDirectory( MAX_PATH, szCurrentDirectory );
	TCHAR szInitialDirectory[MAX_PATH]; strcpy( szInitialDirectory, GetFileDirectory(lpszPathName) );

	CString szTitle; szTitle.LoadString(IDS_DLG_SELECT_FILE);
	dlg.m_ofn.lpstrTitle = szTitle; dlg.m_ofn.lpstrInitialDir = szInitialDirectory;
	dlg.m_ofn.nFilterIndex = CCedtApp::GetFilterIndexDialog() + 1;
	if( dlg.DoModal() != IDOK ) return FALSE;

//	GetCurrentDirectory( MAX_PATH, szInitialDirectory );
	SetCurrentDirectory( szCurrentDirectory );

	CCedtApp::SetFilterIndexDialog(dlg.m_ofn.nFilterIndex - 1);
	szSelectPath = dlg.GetPathName();

	return TRUE;
}

BOOL CCedtView::AskUserSelectDirectory(LPCTSTR lpszPathName, CString & szSelectDirectory)
{
	CString szText( (LPCTSTR)IDS_CHOOSE_DIRECTORY );
	CString szDirectory = GetFileDirectory( lpszPathName );

	CFolderDialog dlg(szText, szDirectory, NULL, this);
	if( dlg.DoModal() != IDOK ) return FALSE;

	szSelectDirectory = dlg.GetPathName();
	return TRUE;
}

BOOL CCedtView::SetDefaultShellVariables(CMapStringToString & clsVariables, LPCTSTR lpszPathName, LPCTSTR lpszProjectPath,
										 LPCTSTR lpszLineNum, LPCTSTR lpszCurrWord, LPCTSTR lpszUserInput, 
										 LPCTSTR lpszSelectPath1, LPCTSTR lpszSelectPath2, LPCTSTR lpszSelectDir1, LPCTSTR lpszSelectDir2)
{
	clsVariables.SetAt("FilePath", lpszPathName);
	clsVariables.SetAt("FileDir", GetFileDirectory(lpszPathName));
	clsVariables.SetAt("FileName", GetFileName(lpszPathName));
	clsVariables.SetAt("FileTitle", GetFileTitle(lpszPathName));

	clsVariables.SetAt("ProjectPath", lpszProjectPath);
	clsVariables.SetAt("ProjectDir", GetFileDirectory(lpszProjectPath));
	clsVariables.SetAt("ProjectName", GetFileName(lpszProjectPath));
	clsVariables.SetAt("ProjectTitle", GetFileTitle(lpszProjectPath));

	clsVariables.SetAt("LineNum", lpszLineNum);
	clsVariables.SetAt("CurrWord", lpszCurrWord);
	clsVariables.SetAt("UserInput", lpszUserInput);				

	clsVariables.SetAt("SelectPath1", lpszSelectPath1);
	clsVariables.SetAt("SelectPath2", lpszSelectPath2);
	clsVariables.SetAt("SelectDir1", lpszSelectDir1);
	clsVariables.SetAt("SelectDir2", lpszSelectDir2);

	return TRUE;
}

/* Removed by Ingyu Kang, 2004.09.20 --
BOOL CCedtView::ParseProgramArguments(CString & szArgu, LPCTSTR lpszPath, LPCTSTR lpszLineNum, LPCTSTR lpszCurrWord, LPCTSTR lpszUserInput, LPCTSTR lpszSelPath1, LPCTSTR lpszSelPath2, LPCTSTR lpszSelDir1, LPCTSTR lpszSelDir2)
{
	CString szPath = lpszPath;					CString szDir  = GetFileDirectory(lpszPath);
	CString szName = GetFileName(lpszPath);		CString szTtle = GetFileTitle(lpszPath);
	CString szLine = lpszLineNum;				CString szWord = lpszCurrWord;

	CString szUser = lpszUserInput;
	CString szSlP1 = lpszSelPath1;				CString szSlP2 = lpszSelPath2;
	CString szSlD1 = lpszSelDir1;				CString szSlD2 = lpszSelDir2;

	INT nFound = -1;

	while( (nFound = szArgu.Find("$(FilePath)"   )) >= 0 ) szArgu = szArgu.Left(nFound) + szPath + szArgu.Mid(nFound + 11);
	while( (nFound = szArgu.Find("$(FileDir)"    )) >= 0 ) szArgu = szArgu.Left(nFound) + szDir  + szArgu.Mid(nFound + 10);
	while( (nFound = szArgu.Find("$(FileName)"   )) >= 0 ) szArgu = szArgu.Left(nFound) + szName + szArgu.Mid(nFound + 11);
	while( (nFound = szArgu.Find("$(FileTitle)"  )) >= 0 ) szArgu = szArgu.Left(nFound) + szTtle + szArgu.Mid(nFound + 12);
	while( (nFound = szArgu.Find("$(LineNum)"    )) >= 0 ) szArgu = szArgu.Left(nFound) + szLine + szArgu.Mid(nFound + 10);
	while( (nFound = szArgu.Find("$(CurrWord)"   )) >= 0 ) szArgu = szArgu.Left(nFound) + szWord + szArgu.Mid(nFound + 11);

	while( (nFound = szArgu.Find("$(UserInput)"  )) >= 0 ) szArgu = szArgu.Left(nFound) + szUser + szArgu.Mid(nFound + 12);
	while( (nFound = szArgu.Find("$(SelectPath1)")) >= 0 ) szArgu = szArgu.Left(nFound) + szSlP1 + szArgu.Mid(nFound + 14);
	while( (nFound = szArgu.Find("$(SelectPath2)")) >= 0 ) szArgu = szArgu.Left(nFound) + szSlP2 + szArgu.Mid(nFound + 14);
	while( (nFound = szArgu.Find("$(SelectDir1)" )) >= 0 ) szArgu = szArgu.Left(nFound) + szSlD1 + szArgu.Mid(nFound + 13);
	while( (nFound = szArgu.Find("$(SelectDir2)" )) >= 0 ) szArgu = szArgu.Left(nFound) + szSlD2 + szArgu.Mid(nFound + 13);

	return TRUE;
}
-- Removed by Ingyu Kang, 2004.09.20 */

/* Removed by Ingyu Kang, 2004.09.20 --
BOOL CCedtView::ParseExecuteDirectory(CString & szArgu, LPCTSTR lpszPath)
{
	CString szDir = GetFileDirectory(lpszPath);

	INT nFound = -1;
	while( (nFound = szArgu.Find("$(FileDir)"    )) >= 0 ) szArgu = szArgu.Left(nFound) + szDir  + szArgu.Mid(nFound + 10);

	return TRUE;
}
-- Removed by Ingyu Kang, 2004.09.20 */

BOOL CCedtView::ReplaceShellVariables(CString & szArgu, CMapStringToString & clsVariables)
{
	TCHAR * pArgu = (TCHAR *)(LPCTSTR)szArgu;
	TCHAR * pChar = pArgu;

	TRACE1("BEGIN ReplaceShellVariables: %s\n", szArgu);
	while( * pChar && * pChar != '$' ) pChar++;

	while( * pChar ) {
		BOOL bEnclosed = FALSE;
		TCHAR * pSave = pChar;

		if( * pChar == '$' ) pChar++;
		if( * pChar == '(' ) { bEnclosed = TRUE; pChar++; }

		while( * pChar ) {
			if( bEnclosed && * pChar == ')' ) { pChar++; break; }
			if( ! bEnclosed && ! isalnum(* pChar) ) break;
			pChar++;
		}

		CString szVar0 = CString(pSave, pChar-pSave);
		CString szVar1 = bEnclosed ? CString(pSave+2, pChar-pSave-3) : CString(pSave+1, pChar-pSave-1);
		TRACE1("- Variable Found: '%s'\n", szVar0);

		CString szVar2 = szVar1, szExpn, szValu; INT nFound = -1;
		if( (nFound = szVar1.Find('=')) >= 0 ) { szVar2 = szVar1.Left(nFound); szExpn = szVar1.Mid(nFound); }
		if( (nFound = szVar1.Find(':')) >= 0 ) { szVar2 = szVar1.Left(nFound); szExpn = szVar1.Mid(nFound); }
		if( (nFound = szVar1.Find('#')) >= 0 ) { szVar2 = szVar1.Left(nFound); szExpn = szVar1.Mid(nFound); }
		if( (nFound = szVar1.Find('%')) >= 0 ) { szVar2 = szVar1.Left(nFound); szExpn = szVar1.Mid(nFound); }

		BOOL bFound = clsVariables.Lookup( szVar2, szValu );
		if( ! bFound ) szValu = getenv( szVar2 );
		TRACE2("- Original Value: '%s' -> '%s'\n", szVar2, szValu);

		if( szExpn.GetLength() ) ExpandShellVariable( szValu, szExpn );
		TRACE2("- Value Expanded: '%s' -> '%s'\n", szVar1, szValu);

		szArgu = szArgu.Left(pSave-pArgu) + szValu + szArgu.Mid(pChar-pArgu);
		pChar  = pArgu  = (TCHAR *)(LPCTSTR)szArgu;

		while( * pChar && * pChar != '$' ) pChar++;
	}

	TRACE1("ENDOF ReplaceShellVariables: %s\n", szArgu);
	return TRUE;
}

BOOL CCedtView::ExpandShellVariable(CString & szValue, LPCTSTR lpszExpand)
{
	INT nLength = szValue.GetLength();

	if( lpszExpand[0] == '=' ) { // assignment
		INT nExpand = strlen(lpszExpand+1);
		if( ! nLength && nExpand ) szValue = lpszExpand + 1;
	} else if( lpszExpand[0] == ':' ) { // substring
		TCHAR * pFound = strchr(lpszExpand+1, ',');
		INT nFirst = atoi( lpszExpand+1 ); if( nFirst > nLength ) nFirst = nLength;
		INT nCount = pFound ? atoi( pFound+1 ) : nLength-nFirst;
		szValue = szValue.Mid(nFirst, nCount);
	} else if( lpszExpand[0] == '#' ) { // beginning match delete
		INT nExpand = strlen(lpszExpand+1);
		if( nExpand && szValue.Find(lpszExpand+1) == 0 )
			szValue = szValue.Mid(nExpand);
	} else if( lpszExpand[0] == '%' ) { // trailing match delete
		INT nExpand = strlen(lpszExpand+1);
		if( nExpand && szValue.Find(lpszExpand+1) == nLength-nExpand )
			szValue = szValue.Left(nLength-nExpand);
	}

	return TRUE;
}

BOOL CCedtView::ExecuteWinHelp(LPCTSTR lpszCommand, LPCTSTR lpszArgument)
{
//	HWND hWnd = ::GetDesktopWindow(); -- changed to release input focus
	HWND hWnd = AfxGetMainWnd()->m_hWnd;

	return ::WinHelp( hWnd, lpszCommand, HELP_PARTIALKEY, (DWORD)lpszArgument );
}

BOOL CCedtView::ExecuteHtmlHelp(LPCTSTR lpszCommand, LPCTSTR lpszArgument)
{
	HH_AKLINK link; memset( & link, 0x00, sizeof(HH_AKLINK) );
	link.cbStruct = sizeof(HH_AKLINK);
	link.pszKeywords = lpszArgument;
	link.fIndexOnFail = TRUE;

	if( ! CCedtApp::m_bHtmlHelpInitialized ) {
		::HtmlHelp(NULL, NULL, HH_INITIALIZE, (DWORD)&CCedtApp::m_dwHtmlHelpCookie);
		CCedtApp::m_bHtmlHelpInitialized = TRUE;
	} // HtmlHelp will be uninitialized in CCedtApp::ExitInstance()

//	HWND hWnd = ::GetDesktopWindow(); -- changed to release input forcus
	HWND hWnd = AfxGetMainWnd()->m_hWnd;

//	HWND hHlp = ::HtmlHelp( hWnd, lpszCommand, HH_DISPLAY_TOC, NULL);
	HWND hHlp = ::HtmlHelp( hWnd, lpszCommand, HH_KEYWORD_LOOKUP, (DWORD)(& link) );

	return hHlp ? TRUE : FALSE;
}

BOOL CCedtView::ExecuteExecutable(LPCTSTR lpszCommand, LPCTSTR lpszArgument, LPCTSTR lpszDirectory, BOOL bCloseOnExit, BOOL bCaptureOutput)
{
	TCHAR szCommandLine[2048];
	if( ! bCaptureOutput && ! bCloseOnExit ) {
		CString szLauncher = CCedtApp::m_szInstallDirectory + "\\launch.exe";
		CString szShortPath = GetShortPathName( lpszCommand );
		sprintf(szCommandLine, "\"%s\" %s %s", szLauncher, szShortPath, lpszArgument);
	} else {
		sprintf(szCommandLine, "\"%s\" %s", lpszCommand, lpszArgument);
	}

	HANDLE hChildStdinWr, hChildStdinRd;
	HANDLE hChildStdoutWr, hChildStdoutRd;
	if( bCaptureOutput ) { // create redirect pipe
		SECURITY_ATTRIBUTES saAttr; 
		saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
		saAttr.bInheritHandle = TRUE; 
		saAttr.lpSecurityDescriptor = NULL; 

		HANDLE hProcess = GetCurrentProcess();

		if( ! CreatePipe( & hChildStdinRd, & hChildStdinWr, & saAttr, 0 ) ) return FALSE;
 		if( ! DuplicateHandle(hProcess, hChildStdinWr, hProcess, & m_hChildStdinWrDup, 0, FALSE, DUPLICATE_SAME_ACCESS) ) return FALSE;
 		if( ! CloseHandle( hChildStdinWr ) ) return FALSE;

		if( ! CreatePipe( & hChildStdoutRd, & hChildStdoutWr, & saAttr, 0 ) ) return FALSE;
		if( ! DuplicateHandle(hProcess, hChildStdoutRd, hProcess, & m_hChildStdoutRdDup, 0, FALSE, DUPLICATE_SAME_ACCESS) ) return FALSE;
		if( ! CloseHandle( hChildStdoutRd ) ) return FALSE;
	}

	STARTUPINFO si; ZeroMemory( & si, sizeof(si) ); si.cb = sizeof(si);
	if( bCaptureOutput ) {
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		si.wShowWindow = SW_HIDE;
		si.hStdInput = hChildStdinRd;
		si.hStdOutput = hChildStdoutWr;
		si.hStdError = hChildStdoutWr;
	} else {
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOWNORMAL;
	}

	LPCTSTR lpCurrentDirectory = NULL;
	if( strlen(lpszDirectory) ) lpCurrentDirectory = lpszDirectory;

	PROCESS_INFORMATION pi; ZeroMemory( & pi, sizeof(pi) );
	BOOL bResult = ::CreateProcess( NULL, szCommandLine, NULL, NULL, TRUE, 0, NULL, lpCurrentDirectory, & si, & pi);

	m_hChildProcess = pi.hProcess;
	m_hChildThread = pi.hThread;

	if( bCaptureOutput ) {
		if( ! CloseHandle(hChildStdinRd) ) return FALSE;
		if( ! CloseHandle(hChildStdoutWr) ) return FALSE;

		CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();
		if( ! pFrame->IsOutputWindowVisible() ) pFrame->ShowOutputWindow(TRUE);

		pFrame->SetOutputWindowOccupied(TRUE);
		pFrame->EnableOutputWindowInput(TRUE);

		pFrame->ClearOutputWindowContents();

		CString szMessage; szMessage.LoadString(IDS_OUT_COMMAND_TITLE);
		pFrame->AddStringToOutputWindow( szMessage, RGB(0, 0, 0) );

		szMessage.Format(IDS_OUT_COMMAND_COMMAND, szCommandLine);
		pFrame->AddStringToOutputWindow( szMessage, RGB(0, 0, 128) );

		m_bChildOutputLineContinue = FALSE;
		m_arrChildInputString.RemoveAll();

		SetTimer( ID_TIMER_CAPTURE_OUTPUT, 100, NULL );
	}

	return bResult;
}

void CCedtView::OnTimerCaptureOutput()
{
	static DWORD dwSave = 0; 
	static TCHAR szReadBuffer[4096], szWriteBuffer[4096];

	BOOL bResult; DWORD dwExitCode = 0;
	bResult = GetExitCodeProcess( m_hChildProcess, & dwExitCode );

	while( dwExitCode == STILL_ACTIVE && m_arrChildInputString.GetSize() ) {
		CString & szChildInputString = m_arrChildInputString.ElementAt(0);
		DWORD dwWritten, dwWrite = szChildInputString.GetLength();

		strcpy( szWriteBuffer, szChildInputString );
		szWriteBuffer[dwWrite] = '\n'; szWriteBuffer[dwWrite+1] = '\0';

		bResult = WriteFile( m_hChildStdinWrDup, szWriteBuffer, dwWrite+1, & dwWritten, NULL );
	//	bResult = FlushFileBuffers( m_hChildStdinWrDup ); -- flush will hang the child process

		// copy input string to output buffer for local echo effect
		if( COutputWindow::m_bLocalEcho ) { strcpy( szReadBuffer + dwSave, szWriteBuffer ); dwSave += dwWrite + 1; }

		INT nArraySize = m_arrChildInputString.GetSize();
		m_arrChildInputString.RemoveAt( nArraySize - 1 );
	}

	DWORD dwAvail = 0, dwRead = 0;
	if( dwExitCode == STILL_ACTIVE ) {
		bResult = PeekNamedPipe( m_hChildStdoutRdDup, NULL, 0, NULL, & dwAvail, NULL );
		if( dwAvail ) {
			bResult = ReadFile( m_hChildStdoutRdDup, szReadBuffer + dwSave, dwAvail, & dwRead, NULL );
			dwSave += dwRead; szReadBuffer[dwSave] = '\0';
		}
	} else {
		bResult = ReadFile( m_hChildStdoutRdDup, szReadBuffer + dwSave, 4096 - dwSave, & dwRead, NULL );
		dwSave += dwRead; szReadBuffer[dwSave] = '\0';
		if( dwSave ) {
			dwSave++; szReadBuffer[dwSave] = '\0';
		}
	}

	DWORD i = 0, j = 0;
	while( i < dwSave ) {
		if( szReadBuffer[i] != '\n' ) { i++; continue; }

		if( i > 0 && szReadBuffer[i-1] == '\r' ) szReadBuffer[i-1] = '\0';
		else szReadBuffer[i] = '\0';

		CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();
		if( m_bChildOutputLineContinue ) {
			CString szCompleteLine; pFrame->GetStringOfOutputWindow( szCompleteLine );
			szCompleteLine += szReadBuffer;

			pFrame->ReplaceStringOfOutputWindow( szCompleteLine, RGB(0, 0, 0) );
			m_bChildOutputLineContinue = FALSE;
		} else {
			pFrame->AddStringToOutputWindow( szReadBuffer, RGB(0, 0, 0) );
		}

		for( j = i + 1; j <= dwSave; j++ ) szReadBuffer[j-i-1] = szReadBuffer[j];
		dwSave = dwSave - (i + 1); i = 0;
	}

	if( i = dwSave ) {
		if( i > 0 && szReadBuffer[i-1] == '\r' ) szReadBuffer[i-1] = '\0';
		else szReadBuffer[i] = '\0';

		CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();
		pFrame->AddStringToOutputWindow( szReadBuffer, RGB(0, 0, 0) );

		dwSave = 0; szReadBuffer[0] = '\0';
		m_bChildOutputLineContinue = TRUE;
	}

	if( dwExitCode != STILL_ACTIVE ) {
		KillTimer( ID_TIMER_CAPTURE_OUTPUT );

		CloseHandle(m_hChildStdinWrDup);
		CloseHandle(m_hChildStdoutRdDup);
		m_hChildStdinWrDup = m_hChildStdoutRdDup = NULL;

		dwSave = 0;
		szReadBuffer[dwSave] = '\0';

		CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();

		CString szMessage; szMessage.Format(IDS_OUT_COMMAND_TERMINATED, dwExitCode);
		pFrame->AddStringToOutputWindow( szMessage, RGB(0, 0, 128) );

		pFrame->SetOutputWindowOccupied(FALSE);
		pFrame->EnableOutputWindowInput(FALSE);

		// check if files modified outside
		if( CCedtApp::m_bCheckIfFilesModifiedOutside ) pFrame->PostMessage(WM_COMMAND, ID_FILE_CHECK_MODIFIED, 0L);
	}
}

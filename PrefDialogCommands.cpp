#include "stdafx.h"
#include "cedtHeader.h"
#include "PrefDialog.h"
#include "FolderDialog.h"


void CPreferenceDialog::InitCommandsPage()
{
	CString szText;

	m_lstCommandList.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	szText.LoadString( IDS_PREF_USER_TOOLS_NAME );
	m_lstCommandList.InsertColumn( 0, szText, LVCFMT_LEFT, 159 );
	szText.LoadString( IDS_PREF_USER_TOOLS_HOT_KEY );
	m_lstCommandList.InsertColumn( 1, szText, LVCFMT_LEFT, 60 );

	m_btnCommandMoveUp.SetIcon( m_lstButtonImage.ExtractIcon(0) );
	m_btnCommandMoveDown.SetIcon( m_lstButtonImage.ExtractIcon(1) );
	m_btnCommandArgument.SetIcon( m_lstButtonImage.ExtractIcon(2) );
	m_btnCommandDirectory.SetIcon( m_lstButtonImage.ExtractIcon(2) );
}

void CPreferenceDialog::SizeCommandsPage()
{
	INT nPosY;

	nPosY  = 26; m_stcCommandSettingsBox.MoveWindow(170, nPosY, 360, 125);
	nPosY += 10; m_lstCommandList.MoveWindow(180, nPosY, 240, 100);			m_btnCommandRemove.MoveWindow(430, nPosY, 70, 18);
	nPosY += 20; m_btnCommandMoveUp.MoveWindow(430, nPosY, 34, 18);			m_btnCommandMoveDown.MoveWindow(466, nPosY, 34, 18);

	nPosY  = 26  + 100;
	nPosY += 35; m_stcCommandItemBox.MoveWindow(170, nPosY, 360, 165);
	nPosY += 10; m_stcCommandText.MoveWindow(180, nPosY, 80, 14);			m_edtCommandText.MoveWindow(270, nPosY-3, 230, 18);		
	nPosY += 20; m_stcCommandCommand.MoveWindow(180, nPosY, 80, 14);		m_edtCommandCommand.MoveWindow(270, nPosY-3, 230, 18);		m_btnCommandCommand.MoveWindow(505, nPosY-3, 20, 18);
	nPosY += 20; m_stcCommandArgument.MoveWindow(180, nPosY, 80, 14);		m_edtCommandArgument.MoveWindow(270, nPosY-3, 230, 18);		m_btnCommandArgument.MoveWindow(505, nPosY-3, 20, 18);
	nPosY += 20; m_stcCommandDirectory.MoveWindow(180, nPosY, 80, 14);		m_edtCommandDirectory.MoveWindow(270, nPosY-3, 230, 18);	m_btnCommandDirectory.MoveWindow(505, nPosY-3, 20, 18);
	nPosY += 20; m_stcCommandHotKey.MoveWindow(180, nPosY, 80, 14);			m_hkyCommandHotKey.MoveWindow(270, nPosY-3, 230, 18);

	nPosY += 30; m_chkCommandCloseOnExit.MoveWindow(180, nPosY, 150, 14);	m_chkCommandShortFileName.MoveWindow(340, nPosY, 170, 14);
	nPosY += 20; m_chkCommandCaptureOutput.MoveWindow(180, nPosY, 150, 14);	m_chkCommandSaveBefore.MoveWindow(340, nPosY, 170, 14);

	nPosY += 35; m_btnCommandLoadTools.MoveWindow(170, nPosY, 170, 20);		m_btnCommandSaveTools.MoveWindow(360, nPosY, 170, 20);
}

void CPreferenceDialog::ShowCommandsPage()
{
	INT nCmdShow = (m_nActiveCategory == PREF_CATEGORY_COMMANDS) ? SW_SHOW : SW_HIDE;

	m_stcCommandSettingsBox.ShowWindow(nCmdShow);
	m_lstCommandList.ShowWindow(nCmdShow);			m_btnCommandRemove.ShowWindow(nCmdShow);
	m_btnCommandMoveUp.ShowWindow(nCmdShow);		m_btnCommandMoveDown.ShowWindow(nCmdShow);

	m_stcCommandItemBox.ShowWindow(nCmdShow);
	m_stcCommandText.ShowWindow(nCmdShow);			m_edtCommandText.ShowWindow(nCmdShow);
	m_stcCommandCommand.ShowWindow(nCmdShow);		m_edtCommandCommand.ShowWindow(nCmdShow);		m_btnCommandCommand.ShowWindow(nCmdShow);
	m_stcCommandArgument.ShowWindow(nCmdShow);		m_edtCommandArgument.ShowWindow(nCmdShow);		m_btnCommandArgument.ShowWindow(nCmdShow);
	m_stcCommandDirectory.ShowWindow(nCmdShow);		m_edtCommandDirectory.ShowWindow(nCmdShow);		m_btnCommandDirectory.ShowWindow(nCmdShow);
	m_stcCommandHotKey.ShowWindow(nCmdShow);		m_hkyCommandHotKey.ShowWindow(nCmdShow);

	m_chkCommandCloseOnExit.ShowWindow(nCmdShow);	m_chkCommandShortFileName.ShowWindow(nCmdShow);
	m_chkCommandCaptureOutput.ShowWindow(nCmdShow);	m_chkCommandSaveBefore.ShowWindow(nCmdShow);

	m_btnCommandLoadTools.ShowWindow(nCmdShow);		m_btnCommandSaveTools.ShowWindow(nCmdShow);		
}


BOOL CPreferenceDialog::LoadUserCommands()
{
	for(INT i = 0; i < 11; i++) {
		m_clsUserCommand[i].CopyContents(CCedtView::m_clsUserCommand[i]);
		if( i > 0 ) {
			m_lstCommandList.InsertItem(i-1, "");
			DispCommandText(i-1);
		}
	}

	m_lstCommandList.SetItemState(m_nActiveUserCommand, LVIS_SELECTED, LVIS_SELECTED);
	m_lstCommandList.EnsureVisible(m_nActiveUserCommand, FALSE);

	return TRUE;
}

BOOL CPreferenceDialog::SaveUserCommands()
{
	for(INT i = 0; i < 11; i++) {
		CCedtView::m_clsUserCommand[i].CopyContents(m_clsUserCommand[i]);
	}

	return TRUE;
}

BOOL CPreferenceDialog::FileLoadUserCommands(LPCTSTR lpszPathName)
{
	ifstream fin(lpszPathName, ios::in | ios::nocreate | ios::binary);
	if( ! fin.is_open() ) return FALSE;

	TCHAR szBuffer[2048]; INT nLength = strlen(STRING_USERTOOLSVER); fin.read((char *)szBuffer, nLength); szBuffer[nLength] = '\0';
	if( strcmp(szBuffer, STRING_USERTOOLSVER) ) { fin.close(); return FALSE; }

	for(INT i = 0; i < 11; i++) {
		if( ! m_clsUserCommand[i].StreamLoad(fin) ) { fin.close(); return FALSE; }
	}

	fin.close();
	return TRUE;
}

BOOL CPreferenceDialog::FileSaveUserCommands(LPCTSTR lpszPathName)
{
	ofstream fout(lpszPathName, ios::out | ios::binary);
	if( ! fout.is_open() ) return FALSE;

	INT nLength = strlen(STRING_USERTOOLSVER);
	fout.write((const char *)STRING_USERTOOLSVER, nLength);

	for( INT i = 0; i < 11; i++ ) {
		if( ! m_clsUserCommand[i].StreamSave(fout) ) { fout.close(); return FALSE; }
	}

	fout.close();
	return TRUE;
}

BOOL CPreferenceDialog::DispCommandText(INT nCommand)
{
	m_lstCommandList.SetItemText(nCommand, 0, GetCommandName(nCommand+1));
	m_lstCommandList.SetItemText(nCommand, 1, GetCommandHotKeyText(nCommand+1));
	return TRUE;
}

CString CPreferenceDialog::GetCommandName(INT nCommand)
{
	CUserCommand & rCommand = m_clsUserCommand[nCommand];
	if( rCommand.m_szName.GetLength() ) return rCommand.m_szName;
	else return "- Empty -";
}

CString CPreferenceDialog::GetCommandHotKeyText(INT nCommand)
{
	CUserCommand & rCommand = m_clsUserCommand[nCommand];
	CString szHotKeyText = rCommand.GetHotKeyText();
	if( ! szHotKeyText.GetLength() ) szHotKeyText.Format("Ctrl+%d", nCommand % 10);
	return szHotKeyText;
}

void CPreferenceDialog::OnItemchangedCommandList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if( (pNMListView->iItem >= 0) && (pNMListView->uNewState & LVIS_SELECTED) ) {
		INT nCommand = pNMListView->iItem; m_nActiveUserCommand = nCommand;
		CUserCommand & rCommand = m_clsUserCommand[nCommand+1];

		m_edtCommandText.SetWindowText( rCommand.m_szName );
		m_edtCommandCommand.SetWindowText( rCommand.m_szCommand );
		m_edtCommandArgument.SetWindowText( rCommand.m_szArgument );
		m_edtCommandDirectory.SetWindowText( rCommand.m_szDirectory );
		m_hkyCommandHotKey.SetHotKey( rCommand.m_wVirtualKeyCode, rCommand.m_wModifiers );

		m_chkCommandCloseOnExit.SetCheck( rCommand.m_bCloseOnExit );
		m_chkCommandShortFileName.SetCheck( rCommand.m_bUseShortFileName );
		m_chkCommandCaptureOutput.SetCheck( rCommand.m_bCaptureOutput );
		m_chkCommandSaveBefore.SetCheck( rCommand.m_bSaveBeforeExecute );

		m_chkCommandCloseOnExit.EnableWindow( ! rCommand.m_bCaptureOutput );
	}
	*pResult = 0;
}

void CPreferenceDialog::OnCommandLoadTools() 
{
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	CString szFilter; szFilter.LoadString(IDS_FILTER_USER_TOOL);
	CFileDialog dlg(TRUE, NULL, NULL, dwFlags, szFilter);

	CString szTitle; szTitle.LoadString(IDS_DLG_LOAD_USER_TOOLS);
	CString szInitialDirectory = CCedtApp::m_szInstallDirectory + "\\tools";

	TCHAR szCurrentDirectory[MAX_PATH];
	GetCurrentDirectory( MAX_PATH, szCurrentDirectory );

	dlg.m_ofn.lpstrTitle = szTitle; dlg.m_ofn.lpstrInitialDir = szInitialDirectory;
	if( dlg.DoModal() != IDOK ) return;

	SetCurrentDirectory( szCurrentDirectory );
	FileLoadUserCommands( dlg.GetPathName() );

	for( INT i = 1; i < 11; i++ ) DispCommandText(i-1);
}

void CPreferenceDialog::OnCommandSaveTools() 
{
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CString szFilter; szFilter.LoadString(IDS_FILTER_USER_TOOL);
	CFileDialog dlg(FALSE, ".tools", NULL, dwFlags, szFilter);

	CString szTitle; szTitle.LoadString(IDS_DLG_SAVE_USER_TOOLS);
	CString szInitialDirectory = CCedtApp::m_szInstallDirectory + "\\tools";

	TCHAR szCurrentDirectory[MAX_PATH];
	GetCurrentDirectory( MAX_PATH, szCurrentDirectory );

	dlg.m_ofn.lpstrTitle = szTitle; dlg.m_ofn.lpstrInitialDir = szInitialDirectory;
	if( dlg.DoModal() != IDOK ) return;

	SetCurrentDirectory( szCurrentDirectory );
	FileSaveUserCommands( dlg.GetPathName() );
}

void CPreferenceDialog::OnCommandRemove() 
{
	INT nCommand = m_nActiveUserCommand;
	CUserCommand & rCommand = m_clsUserCommand[nCommand+1];

	rCommand.DeleteContents();
	DispCommandText( nCommand );

	m_edtCommandText.SetWindowText( rCommand.m_szName );
	m_edtCommandCommand.SetWindowText( rCommand.m_szCommand );
	m_edtCommandArgument.SetWindowText( rCommand.m_szArgument );
	m_edtCommandDirectory.SetWindowText( rCommand.m_szDirectory );
	m_hkyCommandHotKey.SetHotKey( rCommand.m_wVirtualKeyCode, rCommand.m_wModifiers );

	m_chkCommandCloseOnExit.SetCheck( rCommand.m_bCloseOnExit );
	m_chkCommandShortFileName.SetCheck( rCommand.m_bUseShortFileName );
	m_chkCommandCaptureOutput.SetCheck( rCommand.m_bCaptureOutput );
	m_chkCommandSaveBefore.SetCheck( rCommand.m_bSaveBeforeExecute );
}

void CPreferenceDialog::OnCommandMoveUp() 
{
	INT nCommand = m_nActiveUserCommand;
	CUserCommand & rCommand0 = m_clsUserCommand[         0];
	CUserCommand & rCommand1 = m_clsUserCommand[nCommand+0];
	CUserCommand & rCommand2 = m_clsUserCommand[nCommand+1];

	if( nCommand > 0 ) {
		rCommand0.CopyContents( rCommand1 );
		rCommand1.CopyContents( rCommand2 );
		rCommand2.CopyContents( rCommand0 );

		DispCommandText( nCommand-1 );
		DispCommandText( nCommand+0 );

		m_lstCommandList.SetItemState( nCommand-1, LVIS_SELECTED, LVIS_SELECTED );
		m_lstCommandList.EnsureVisible( nCommand-1, FALSE );
	}
}

void CPreferenceDialog::OnCommandMoveDown() 
{
	INT nCommand = m_nActiveUserCommand;
	CUserCommand & rCommand0 = m_clsUserCommand[         0];
	CUserCommand & rCommand1 = m_clsUserCommand[nCommand+1];
	CUserCommand & rCommand2 = m_clsUserCommand[nCommand+2];

	if( nCommand < 9 ) {
		rCommand0.CopyContents( rCommand1 );
		rCommand1.CopyContents( rCommand2 );
		rCommand2.CopyContents( rCommand0 );

		DispCommandText( nCommand+0 );
		DispCommandText( nCommand+1 );

		m_lstCommandList.SetItemState( nCommand+1, LVIS_SELECTED, LVIS_SELECTED );
		m_lstCommandList.EnsureVisible( nCommand+1, FALSE );
	}
}

void CPreferenceDialog::OnChangeCommandText() 
{
	INT nCommand = m_nActiveUserCommand; 
	CUserCommand & rCommand = m_clsUserCommand[nCommand+1];
	m_edtCommandText.GetWindowText( rCommand.m_szName );
	DispCommandText( nCommand );
}

void CPreferenceDialog::OnChangeCommandCommand() 
{
	INT nCommand = m_nActiveUserCommand;
	CUserCommand & rCommand = m_clsUserCommand[nCommand+1];
	m_edtCommandCommand.GetWindowText( rCommand.m_szCommand );
}

void CPreferenceDialog::OnChangeCommandArgument() 
{
	INT nCommand = m_nActiveUserCommand;
	CUserCommand & rCommand = m_clsUserCommand[nCommand+1];
	m_edtCommandArgument.GetWindowText( rCommand.m_szArgument );
}

void CPreferenceDialog::OnChangeCommandDirectory() 
{
	INT nCommand = m_nActiveUserCommand;
	CUserCommand & rCommand = m_clsUserCommand[nCommand+1];
	m_edtCommandDirectory.GetWindowText( rCommand.m_szDirectory );
}

void CPreferenceDialog::OnChangeCommandHotKey()
{
	INT nCommand = m_nActiveUserCommand;
	CUserCommand & rCommand = m_clsUserCommand[nCommand+1];
	m_hkyCommandHotKey.GetHotKey( rCommand.m_wVirtualKeyCode, rCommand.m_wModifiers );
	DispCommandText( nCommand );
}

void CPreferenceDialog::OnCommandCloseOnExit() 
{
	INT nCommand = m_nActiveUserCommand;
	CUserCommand & rCommand = m_clsUserCommand[nCommand+1];
	rCommand.m_bCloseOnExit = m_chkCommandCloseOnExit.GetCheck();
}

void CPreferenceDialog::OnCommandShortFileName() 
{
	INT nCommand = m_nActiveUserCommand;
	CUserCommand & rCommand = m_clsUserCommand[nCommand+1];
	rCommand.m_bUseShortFileName = m_chkCommandShortFileName.GetCheck();
}

void CPreferenceDialog::OnCommandCaptureOutput() 
{
	INT nCommand = m_nActiveUserCommand;
	CUserCommand & rCommand = m_clsUserCommand[nCommand+1];
	rCommand.m_bCaptureOutput = m_chkCommandCaptureOutput.GetCheck();
	m_chkCommandCloseOnExit.EnableWindow( ! m_chkCommandCaptureOutput.GetCheck() );
}

void CPreferenceDialog::OnCommandSaveBefore() 
{
	INT nCommand = m_nActiveUserCommand;
	CUserCommand & rCommand = m_clsUserCommand[nCommand+1];
	rCommand.m_bSaveBeforeExecute = m_chkCommandSaveBefore.GetCheck();
}

void CPreferenceDialog::OnCommandCommandBrowse() 
{
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	CString szFilter; szFilter.LoadString(IDS_FILTER_COMMAND_BROWSE);
	CString szPathName; m_edtCommandCommand.GetWindowText( szPathName );
	CFileDialog dlg(TRUE, NULL, szPathName, dwFlags, szFilter);

	CString szTitle; szTitle.LoadString(IDS_DLG_SELECT_FILE);
	TCHAR szCurrentDirectory[MAX_PATH]; GetCurrentDirectory( MAX_PATH, szCurrentDirectory );

	dlg.m_ofn.lpstrTitle = szTitle; dlg.m_ofn.lpstrInitialDir = szCurrentDirectory;
	if( dlg.DoModal() != IDOK ) return;

	m_edtCommandCommand.SetWindowText( dlg.GetPathName() );
	SetCurrentDirectory( szCurrentDirectory );
}

void CPreferenceDialog::OnCommandArgumentMenu() 
{
	CMenu * pMenu, context; context.LoadMenu(IDR_PREF_DIALOG);
	pMenu = context.GetSubMenu(0);

	CRect rect; m_btnCommandArgument.GetWindowRect( & rect );
	CPoint point(rect.right, rect.top);

	UINT nFlags = TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON;
	pMenu->TrackPopupMenu(nFlags, point.x, point.y, this);
}

void CPreferenceDialog::OnCommandDirectoryMenu() 
{
	CMenu * pMenu, context; context.LoadMenu(IDR_PREF_DIALOG);
	pMenu = context.GetSubMenu(1);

	CRect rect; m_btnCommandDirectory.GetWindowRect( & rect );
	CPoint point(rect.right, rect.top);

	UINT nFlags = TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON;
	pMenu->TrackPopupMenu(nFlags, point.x, point.y, this);
}

void CPreferenceDialog::OnCmdArgFilePath()		{ m_edtCommandArgument.ReplaceSel("$(FilePath)"); }
void CPreferenceDialog::OnCmdArgFileDir()		{ m_edtCommandArgument.ReplaceSel("$(FileDir)"); }
void CPreferenceDialog::OnCmdArgFileName()		{ m_edtCommandArgument.ReplaceSel("$(FileName)"); }
void CPreferenceDialog::OnCmdArgFileTitle()		{ m_edtCommandArgument.ReplaceSel("$(FileTitle)"); }

void CPreferenceDialog::OnCmdArgProjectPath()	{ m_edtCommandArgument.ReplaceSel("$(ProjectPath)"); } 
void CPreferenceDialog::OnCmdArgProjectDir()	{ m_edtCommandArgument.ReplaceSel("$(ProjectDir)"); }
void CPreferenceDialog::OnCmdArgProjectTitle()	{ m_edtCommandArgument.ReplaceSel("$(ProjectTitle)"); }

void CPreferenceDialog::OnCmdArgLineNumber()	{ m_edtCommandArgument.ReplaceSel("$(LineNum)"); }
void CPreferenceDialog::OnCmdArgCurrentWord()	{ m_edtCommandArgument.ReplaceSel("$(CurrWord)"); }

void CPreferenceDialog::OnCmdArgUserInput()		{ m_edtCommandArgument.ReplaceSel("$(UserInput)"); }
void CPreferenceDialog::OnCmdArgSelectPath1()	{ m_edtCommandArgument.ReplaceSel("$(SelectPath1)"); }
void CPreferenceDialog::OnCmdArgSelectPath2()	{ m_edtCommandArgument.ReplaceSel("$(SelectPath2)"); }
void CPreferenceDialog::OnCmdArgSelectDir1()	{ m_edtCommandArgument.ReplaceSel("$(SelectDir1)"); }
void CPreferenceDialog::OnCmdArgSelectDir2()	{ m_edtCommandArgument.ReplaceSel("$(SelectDir2)"); }

void CPreferenceDialog::OnCmdDirFileDir()		{ m_edtCommandDirectory.SetWindowText("$(FileDir)"); }
void CPreferenceDialog::OnCmdDirProjectDir()	{ m_edtCommandDirectory.SetWindowText("$(ProjectDir)"); }
void CPreferenceDialog::OnCmdDirBrowse() 
{
	CString szText( (LPCTSTR)IDS_CHOOSE_DIRECTORY );
	CString szDirectory; m_edtCommandDirectory.GetWindowText( szDirectory );
	CFolderDialog dlg(szText, szDirectory, NULL, this);
	if( dlg.DoModal() != IDOK ) return;
	m_edtCommandDirectory.SetWindowText(dlg.GetPathName());
}


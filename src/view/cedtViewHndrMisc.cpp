#include "stdafx.h"
#include "cedtHeader.h"

#include "MacroDefineDialog.h"
#include "UserInputDialog.h"


/////////////////////////////////////////////////////////////////////////////
// Misc Menu

void CCedtView::OnMoveCaretLeft() // not used now
{
	UINT nFlags = GetKeyState() & ~KEYSTATE_CONTROL;
	OnMoveKeyDown(VK_LEFT, nFlags);
}

void CCedtView::OnMoveCaretRight() // not used now
{
	UINT nFlags = GetKeyState() & ~KEYSTATE_CONTROL;
	OnMoveKeyDown(VK_RIGHT, nFlags);
}

void CCedtView::OnMoveCaretUp() 
{
	UINT nFlags = GetKeyState() & ~KEYSTATE_CONTROL;
	OnMoveKeyDown(VK_UP, nFlags);
}

void CCedtView::OnMoveCaretDown() 
{
	UINT nFlags = GetKeyState() & ~KEYSTATE_CONTROL;
	OnMoveKeyDown(VK_DOWN, nFlags);
}

void CCedtView::OnMoveCaretHome() // not used now
{
	UINT nFlags = GetKeyState() & ~(KEYSTATE_CONTROL | KEYSTATE_MENU);
	OnMoveKeyDown(VK_HOME, nFlags);
}

void CCedtView::OnMoveCaretEnd() // not used now
{
	UINT nFlags = GetKeyState() & ~(KEYSTATE_CONTROL | KEYSTATE_MENU);
	OnMoveKeyDown(VK_END, nFlags);
}

void CCedtView::OnMoveCaretPrior() // not used now
{
	UINT nFlags = GetKeyState() & ~(KEYSTATE_CONTROL | KEYSTATE_MENU);
	OnMoveKeyDown(VK_PRIOR, nFlags);
}

void CCedtView::OnMoveCaretNext() // not used now
{
	UINT nFlags = GetKeyState() & ~(KEYSTATE_CONTROL | KEYSTATE_MENU);
	OnMoveKeyDown(VK_NEXT, nFlags);
}

void CCedtView::OnScrollScreenTop() 
{
	SetScrollPosToMakeCaretTop();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnScrollScreenBottom() 
{
	SetScrollPosToMakeCaretBottom();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnScrollScreenCenter() 
{
	SetScrollPosToMakeCaretCenter();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

/////////////////////////////////////////////////////////////////////////////
// Edit Menu

void CCedtView::OnUpdateEditCut(CCmdUI* pCmdUI) { /* pCmdUI->Enable( m_bSelected ); */ }
void CCedtView::OnUpdateEditCopy(CCmdUI* pCmdUI) { /* pCmdUI->Enable( m_bSelected ); */ }
void CCedtView::OnUpdateEditPaste(CCmdUI* pCmdUI) { /* pCmdUI->Enable( HasClipboardData() ); */ }

void CCedtView::OnUpdateEditUndo(CCmdUI* pCmdUI) { pCmdUI->Enable( GetUndoBufferCount() ); }
void CCedtView::OnUpdateEditRedo(CCmdUI* pCmdUI) { pCmdUI->Enable( GetRedoBufferCount() ); }


/////////////////////////////////////////////////////////////////////////////
// View Menu

void CCedtView::OnViewWordWrap() 
{
	CChildFrame * pChild = (CChildFrame *)GetParentFrame();

	if( ! m_bLocalWordWrap && (m_bColumnMode || pChild->GetColumnCount() > 1 ) ) {
		AfxMessageBox(IDS_ERR_SWITCH_WORD_WRAP, MB_OK); 
		return;
	}

	m_bLocalWordWrap = ! m_bLocalWordWrap;

	SaveCaretAndAnchorPos();
	FormatScreenText(); 
	RestoreCaretAndAnchorPos();

	Invalidate(); UpdateWindow();
}

void CCedtView::OnViewSpellCheck() 
{
	m_bLocalSpellCheck = ! m_bLocalSpellCheck;
	if( ! CCedtDoc::IsDictionaryLoaded() ) CCedtDoc::LoadDictionary();

	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->AnalyzeText();

	SaveCaretAndAnchorPos();
	FormatScreenText(); 
	RestoreCaretAndAnchorPos();

	Invalidate(); UpdateWindow();
}

void CCedtView::OnUpdateViewWordWrap(CCmdUI* pCmdUI) { pCmdUI->SetCheck( m_bLocalWordWrap ); }
void CCedtView::OnUpdateViewSpellCheck(CCmdUI* pCmdUI) { pCmdUI->SetCheck( m_bLocalSpellCheck ); }


/////////////////////////////////////////////////////////////////////////////
// Search Menu
void CCedtView::OnUpdateSearchFindNext(CCmdUI* pCmdUI) { pCmdUI->Enable( m_szFindString.GetLength() || (m_bSelected && ! GetSelectedLineCount()) ); } 
void CCedtView::OnUpdateSearchFindPrev(CCmdUI* pCmdUI) { pCmdUI->Enable( m_szFindString.GetLength() || (m_bSelected && ! GetSelectedLineCount()) ); } 


/////////////////////////////////////////////////////////////////////////////
// Tool Menu
void CCedtView::OnToolEvaluateLine() 
{
	BeginActionRecording(TRUE);

	EventEvaluateLine(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_TOOL_EVALUATE_LINE);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnToolMsDosShell() 
{
	CString szShellPath = GetMsDosShellPath();
	WinExec( szShellPath, SW_SHOWNORMAL );
}

void CCedtView::OnToolViewInBrowser() 
{
	SendMessage(WM_COMMAND, ID_FILE_SAVE, 0L); // save before execute
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();

	CString szBrowserPath = GetDefaultBrowserPath();
	if( ! szBrowserPath.GetLength() ) return;

	CString szPathName = GetShortPathName( pDoc->GetPathName() );
	if( ! szPathName.GetLength() ) return;

	CString szCommand = szBrowserPath + " " + szPathName;
	WinExec( szCommand, SW_SHOWNORMAL );
}

void CCedtView::OnCommandExecute(INT nCommand)
{
	if( ! IsCommandAssigned(nCommand) ) return;
	CString szCommand = m_clsUserCommand[nCommand].m_szCommand;
	szCommand.TrimRight(); szCommand.TrimLeft();

	CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pFrame );
	BOOL bCaptureOutput = m_clsUserCommand[nCommand].m_bCaptureOutput;

	if( m_hChildStdinWrDup || m_hChildStdoutRdDup ) {
		AfxMessageBox(IDS_ERR_COMMAND_RUN_ALREADY, MB_OK | MB_ICONSTOP); return; 
	} else if( bCaptureOutput && ! pFrame->CanUseOutputWindow() ) {
		AfxMessageBox(IDS_ERR_OUTPUT_WINDOW_OCCUPIED, MB_OK | MB_ICONSTOP); return;
	} else if( ReplaceShellVariables(szCommand, CMapStringToString()) && ! VerifyFilePath(szCommand) ) {
		CString szMessage; szMessage.Format(IDS_ERR_FILE_NOT_FOUND, szCommand);
		AfxMessageBox(szMessage, MB_OK | MB_ICONSTOP); return;
	}

	// check save_before_execute flag
	BOOL bSaveBeforeExecute = m_clsUserCommand[nCommand].m_bSaveBeforeExecute;
	if( bSaveBeforeExecute ) SendMessage(WM_COMMAND, ID_FILE_SAVE_ALL, 0L); 
//	CCedtDoc * pDoc = (CCedtDoc *)GetDocument(); ASSERT( pDoc );
//	if( bSaveBeforeExecute && pDoc->IsModified() ) SendMessage(WM_COMMAND, ID_FILE_SAVE, 0L); 

	// execute command!
	ExecuteCommand(nCommand);
}

void CCedtView::OnCommandExecute1() { OnCommandExecute( 1); }
void CCedtView::OnCommandExecute2() { OnCommandExecute( 2); }
void CCedtView::OnCommandExecute3() { OnCommandExecute( 3); }
void CCedtView::OnCommandExecute4() { OnCommandExecute( 4); }
void CCedtView::OnCommandExecute5() { OnCommandExecute( 5); }
void CCedtView::OnCommandExecute6() { OnCommandExecute( 6); }
void CCedtView::OnCommandExecute7() { OnCommandExecute( 7); }
void CCedtView::OnCommandExecute8() { OnCommandExecute( 8); }
void CCedtView::OnCommandExecute9() { OnCommandExecute( 9); }
void CCedtView::OnCommandExecute0() { OnCommandExecute(10); }

void CCedtView::OnCommandSendInput() 
{
	if( ! IsCommandRunningInCaptureOutputMode() ) return;

	CUserInputDialog dlg(IDS_ASKUSR_TEXT_TO_SEND);
	if( dlg.DoModal() != IDOK ) return;

	SetChildInputString( dlg.m_szUserInput );
}

void CCedtView::OnUpdateCommandSendInput(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( IsCommandRunningInCaptureOutputMode() );
}

void CCedtView::OnCommandKillProcess() 
{
	if( ! IsCommandRunningInCaptureOutputMode() ) return;
	KillChildProcess();
}

void CCedtView::OnUpdateCommandKillProcess(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( IsCommandRunningInCaptureOutputMode() );
}

void CCedtView::OnUpdateCommandExecute(INT nCommand, CCmdUI* pCmdUI)
{
	if( nCommand != 0 && pCmdUI->m_pMenu ) {
		CString szMenuText; szMenuText.Format("&%d %s\t%s", nCommand % 10, GetCommandName(nCommand), GetCommandHotKeyText(nCommand));
		pCmdUI->SetText(szMenuText);
		pCmdUI->Enable( IsCommandAssigned(nCommand) ); 
	}
}

void CCedtView::OnUpdateCommandExecute1(CCmdUI* pCmdUI) { OnUpdateCommandExecute( 1, pCmdUI); }
void CCedtView::OnUpdateCommandExecute2(CCmdUI* pCmdUI) { OnUpdateCommandExecute( 2, pCmdUI); }
void CCedtView::OnUpdateCommandExecute3(CCmdUI* pCmdUI) { OnUpdateCommandExecute( 3, pCmdUI); }
void CCedtView::OnUpdateCommandExecute4(CCmdUI* pCmdUI) { OnUpdateCommandExecute( 4, pCmdUI); }
void CCedtView::OnUpdateCommandExecute5(CCmdUI* pCmdUI) { OnUpdateCommandExecute( 5, pCmdUI); }
void CCedtView::OnUpdateCommandExecute6(CCmdUI* pCmdUI) { OnUpdateCommandExecute( 6, pCmdUI); }
void CCedtView::OnUpdateCommandExecute7(CCmdUI* pCmdUI) { OnUpdateCommandExecute( 7, pCmdUI); }
void CCedtView::OnUpdateCommandExecute8(CCmdUI* pCmdUI) { OnUpdateCommandExecute( 8, pCmdUI); }
void CCedtView::OnUpdateCommandExecute9(CCmdUI* pCmdUI) { OnUpdateCommandExecute( 9, pCmdUI); }
void CCedtView::OnUpdateCommandExecute0(CCmdUI* pCmdUI) { OnUpdateCommandExecute(10, pCmdUI); }


/////////////////////////////////////////////////////////////////////////////
// Macro Menu

void CCedtView::OnMacroToggleRecording() 
{
	if( m_bMacroRecording ) OnMacroEndRecording();
	else OnMacroBeginRecording();
}

void CCedtView::OnMacroBeginRecording() 
{
	CMacroDefineDialog dlg;

	for(INT i = 1; i <= 10; i++) dlg.m_szMacroName[i] = GetMacroName(i);
	if( dlg.DoModal() != IDOK ) return;

	INT nMacroNumber = dlg.GetSelectedMacroNumber();
	LPCTSTR lpszMacroName = dlg.m_szMacroName[nMacroNumber];

	BeginMacroRecording(nMacroNumber, lpszMacroName);
}

void CCedtView::OnMacroEndRecording() 
{
	EndMacroRecording();

	CCedtApp::SaveMacroBuffers(CCedtApp::m_szAppDataDirectory + "\\cedt.macro");
}

void CCedtView::OnUpdateMacroBeginRecording(CCmdUI* pCmdUI) { pCmdUI->Enable( ! IsMacroRecording() ); }
void CCedtView::OnUpdateMacroEndRecording(CCmdUI* pCmdUI) { pCmdUI->Enable( IsMacroRecording() ); }


void CCedtView::OnMacroReplay(INT nMacro) 
{
	if( IsMacroRecording() ) { 
		CString szMessage; szMessage.Format(IDS_ERR_ILLEGAL_MACRO_DEF, "Replay Macro");
		AfxMessageBox(szMessage, MB_OK | MB_ICONSTOP); CancelMacroRecording(); return;
	} else if( ! GetMacroBufferCount(nMacro) ) return;

	BeginActionRecording(TRUE);

	EventReplayMacro(nMacro, 1, FALSE);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnMacroReplay () { OnMacroReplay( 0); }
void CCedtView::OnMacroReplay1() { OnMacroReplay( 1); }
void CCedtView::OnMacroReplay2() { OnMacroReplay( 2); }
void CCedtView::OnMacroReplay3() { OnMacroReplay( 3); }
void CCedtView::OnMacroReplay4() { OnMacroReplay( 4); }
void CCedtView::OnMacroReplay5() { OnMacroReplay( 5); }
void CCedtView::OnMacroReplay6() { OnMacroReplay( 6); }
void CCedtView::OnMacroReplay7() { OnMacroReplay( 7); }
void CCedtView::OnMacroReplay8() { OnMacroReplay( 8); }
void CCedtView::OnMacroReplay9() { OnMacroReplay( 9); }
void CCedtView::OnMacroReplay0() { OnMacroReplay(10); }


void CCedtView::OnUpdateMacroReplay(INT nMacro, CCmdUI* pCmdUI) {
	if( nMacro != 0 && pCmdUI->m_pMenu ) {
		CString szMenuText; szMenuText.Format("&%d %s\t%s", nMacro % 10, GetMacroName(nMacro), GetMacroHotKeyText(nMacro));
		pCmdUI->SetText(szMenuText);
	}

	pCmdUI->Enable( ! IsMacroRecording() && GetMacroBufferCount(nMacro) ); 
}

void CCedtView::OnUpdateMacroReplay (CCmdUI* pCmdUI) { OnUpdateMacroReplay( 0, pCmdUI); }
void CCedtView::OnUpdateMacroReplay1(CCmdUI* pCmdUI) { OnUpdateMacroReplay( 1, pCmdUI); }
void CCedtView::OnUpdateMacroReplay2(CCmdUI* pCmdUI) { OnUpdateMacroReplay( 2, pCmdUI); }
void CCedtView::OnUpdateMacroReplay3(CCmdUI* pCmdUI) { OnUpdateMacroReplay( 3, pCmdUI); }
void CCedtView::OnUpdateMacroReplay4(CCmdUI* pCmdUI) { OnUpdateMacroReplay( 4, pCmdUI); }
void CCedtView::OnUpdateMacroReplay5(CCmdUI* pCmdUI) { OnUpdateMacroReplay( 5, pCmdUI); }
void CCedtView::OnUpdateMacroReplay6(CCmdUI* pCmdUI) { OnUpdateMacroReplay( 6, pCmdUI); }
void CCedtView::OnUpdateMacroReplay7(CCmdUI* pCmdUI) { OnUpdateMacroReplay( 7, pCmdUI); }
void CCedtView::OnUpdateMacroReplay8(CCmdUI* pCmdUI) { OnUpdateMacroReplay( 8, pCmdUI); }
void CCedtView::OnUpdateMacroReplay9(CCmdUI* pCmdUI) { OnUpdateMacroReplay( 9, pCmdUI); }
void CCedtView::OnUpdateMacroReplay0(CCmdUI* pCmdUI) { OnUpdateMacroReplay(10, pCmdUI); }


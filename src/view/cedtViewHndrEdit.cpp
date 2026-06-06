#include "stdafx.h"
#include "cedtHeader.h"

#include "FindDialog.h"
#include "ReplaceDialog.h"
#include "AskReplaceDialog.h"
#include "GoToDialog.h"


/////////////////////////////////////////////////////////////////////////////
// CARET MOVEMENT
void CCedtView::OnMoveKeyDown(UINT nChar, UINT nFlags)
{
	BOOL bRedraw = EventMoveKeyDown(nChar, nFlags, FALSE);
	if( IsMacroRecording() ) MacroRecordMove(nChar, nFlags);

	SetScrollPosToMakeCaretVisible();
	if( bRedraw ) { Invalidate(); UpdateWindow(); UpdateAllViews(); }
	else UpdateCaretPosition();
}


/////////////////////////////////////////////////////////////////////////////
// BASIC EDITING
void CCedtView::OnCharKeyDown(UINT nChar)
{
	BeginActionRecording(TRUE);

	EventInsertChar(nChar, FALSE);
	if( IsMacroRecording() ) MacroRecordChar(nChar);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnDBCharKeyDown(UINT nCH1, UINT nCH2)
{
	TCHAR szBuf[3]; szBuf[0] = nCH1; szBuf[1] = nCH2; szBuf[2] = '\0';

	BeginActionRecording(TRUE);

	EventInsertString(szBuf, FALSE);
	if( IsMacroRecording() ) MacroRecordString(szBuf);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnImeCompositionStart()
{
	EventCompositionStart(FALSE);
}

void CCedtView::OnImeCompositionEnd(BOOL bRedraw)
{
	BeginActionRecording(TRUE);

	EventCompositionEnd(FALSE);
//	if( IsMacroRecording() ) MacroRecordNull();

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	if( bRedraw ) { Invalidate(); UpdateWindow(); UpdateAllViews(); }
}

void CCedtView::OnImeCompositionCompose(LPCTSTR lpszString)
{
	BeginActionRecording(TRUE);

	EventCompositionCompose(lpszString, FALSE);
//	if( IsMacroRecording() ) MacroRecordNull();

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnImeCompositionResult(LPCTSTR lpszString)
{
	BeginActionRecording(TRUE);

	EventCompositionResult(lpszString, FALSE);
	if( IsMacroRecording() ) MacroRecordString(lpszString);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditEscape() 
{
	EventCommandEscape(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_ESCAPE);

	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditReturn() 
{
	BeginActionRecording(TRUE);

	EventCommandReturn(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_RETURN);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditBack() 
{
	BeginActionRecording(TRUE);

	EventCommandBack(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_BACK);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditDelete() 
{
	BeginActionRecording(TRUE);

	EventCommandDelete(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_DELETE);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditTab() 
{
	BeginActionRecording(TRUE);

	EventCommandTab(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_TAB);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditDetab() 
{
	BeginActionRecording(TRUE);

	EventCommandDetab(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_DETAB);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

/////////////////////////////////////////////////////////////////////////////
// ADVANCED EDITING
void CCedtView::OnEditIncreaseIndent() 
{
	BeginActionRecording(TRUE);

	EventIncreaseIndent(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_INCREASE_INDENT);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditDecreaseIndent() 
{
	BeginActionRecording(TRUE);

	EventDecreaseIndent(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_DECREASE_INDENT);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditMakeComment() 
{
	BeginActionRecording(TRUE);

	EventMakeComment(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_ADD_COMMENT);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditReleaseComment() 
{
	BeginActionRecording(TRUE);

	EventReleaseComment(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_REMOVE_COMMENT);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditJoinLines() 
{
	BeginActionRecording(TRUE);

	EventJoinLines(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_JOIN_LINES);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditSplitLine() 
{
	BeginActionRecording(TRUE);

	EventSplitLine(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_SPLIT_LINE);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditDeleteWord() 
{
	BeginActionRecording(TRUE);

	EventDeleteWord(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_DELETE_WORD);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditDeletePrevWord() 
{
	BeginActionRecording(TRUE);

	EventDeletePrevWord(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_DELETE_PREV_WORD);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditDeleteToEndOfLine() 
{
	BeginActionRecording(TRUE);

	EventDeleteToEndOfLine(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_DELETE_TO_END_OF_LINE);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditDeleteToBeginOfLine() 
{
	BeginActionRecording(TRUE);

	EventDeleteToBeginOfLine(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_DELETE_TO_BEGIN_OF_LINE);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditDeleteLine() 
{
	BeginActionRecording(TRUE);

	EventDeleteLine(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_DELETE_LINE);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditDuplicateLine() 
{
	BeginActionRecording(TRUE);

	EventDuplicateLine(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_DUPLICATE_LINE);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

/////////////////////////////////////////////////////////////////////////////
// COPY & PASTE
void CCedtView::OnEditCut() 
{
	BeginActionRecording(TRUE);

	EventCommandCut(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_CUT);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditCopy() 
{
	EventCommandCopy(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_COPY);
}

void CCedtView::OnEditCopyFilePath() 
{
	EventCommandCopyFilePath(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_COPY_FILE_PATH);
}

void CCedtView::OnEditCutAppend() 
{
	BeginActionRecording(TRUE);

	EventCommandCutAppend(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_CUT_APPEND);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditCopyAppend() 
{
	EventCommandCopyAppend(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_COPY_APPEND);
}

void CCedtView::OnEditPaste() 
{
	BeginActionRecording(TRUE);

	EventCommandPaste(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_PASTE);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

/////////////////////////////////////////////////////////////////////////////
// DRAG & DROP
void CCedtView::OnDragCancel()
{
	EventDragCancel(FALSE);
//	if( IsMacroRecording() ) MacroRecordCommand(ID_DRAG_CANCEL);

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnDragAndDrop(HGLOBAL hMemory)
{
	BeginActionRecording(TRUE);

	EventDragDelete(FALSE);
	EventDragAdjust(FALSE);
	EventDropPaste(hMemory, FALSE);
//	if( IsMacroRecording() ) MacroRecordCommand(ID_DRAG_AND_DROP);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnDragDelete()
{
	BeginActionRecording(TRUE);

	EventDragDelete(FALSE);
//	if( IsMacroRecording() ) MacroRecordCommand(ID_DRAG_DELETE);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnDropCancel()
{
	EventDropCancel(FALSE);
//	if( IsMacroRecording() ) MacroRecordCommand(ID_DROP_CANCEL);

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnDropPaste(HGLOBAL hMemory)
{
	BeginActionRecording(TRUE);

	EventDropPaste(hMemory, FALSE);
//	if( IsMacroRecording() ) MacroRecordCommand(ID_DROP_PASTE);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

/////////////////////////////////////////////////////////////////////////////
// INSERT TEXT
void CCedtView::OnEditInsertDate() 
{
	BeginActionRecording(TRUE);

	EventInsertString(GetCurrentDate() + CString(" "), FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_INSERT_DATE);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditInsertTime() 
{
	BeginActionRecording(TRUE);

	EventInsertString(GetCurrentTime() + CString(" "), FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_INSERT_TIME);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditInsertFile() 
{
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	CString szFilter = CCedtApp::GetComposedFileFilter();
	CFileDialog dlg(TRUE, NULL, NULL, dwFlags, szFilter);

	TCHAR szCurrentDirectory[MAX_PATH];
	GetCurrentDirectory( MAX_PATH, szCurrentDirectory );

	static TCHAR szInitialDirectory[MAX_PATH] = "";
	if( ! strlen( szInitialDirectory ) ) strcpy( szInitialDirectory, szCurrentDirectory );

	CString szTitle; szTitle.LoadString(IDS_DLG_INSERT_FILE); 
	dlg.m_ofn.lpstrTitle = szTitle; dlg.m_ofn.lpstrInitialDir = szInitialDirectory;
	dlg.m_ofn.nFilterIndex = CCedtApp::GetFilterIndexDialog() + 1;
	if( dlg.DoModal() != IDOK ) return;

	GetCurrentDirectory( MAX_PATH, szInitialDirectory );
	SetCurrentDirectory( szCurrentDirectory );

	CCedtApp::SetFilterIndexDialog(dlg.m_ofn.nFilterIndex - 1);
	CString szPathName = dlg.GetPathName();



	BeginActionRecording(TRUE);

	EventInsertFile(szPathName, FALSE);
	if( IsMacroRecording() ) MacroRecordFile(szPathName);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

/////////////////////////////////////////////////////////////////////////////
// SELECTING
void CCedtView::OnEditSelectAll() 
{
	EventSelectAll(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_SELECT_ALL);

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditSelectLine() 
{
	EventSelectLine(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_SELECT_LINE);

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditSelectWord() 
{
	EventSelectWord(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_SELECT_WORD);

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditSelectBlock() 
{
	EventSelectBlock(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_SELECT_BLOCK);

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

/////////////////////////////////////////////////////////////////////////////
// CHANGE CASE
void CCedtView::OnEditUpperCase() 
{
	BeginActionRecording(TRUE);

	EventUpperCase(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_UPPER_CASE);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditLowerCase() 
{
	BeginActionRecording(TRUE);

	EventLowerCase(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_LOWER_CASE);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditCapitalize() 
{
	BeginActionRecording(TRUE);

	EventCapitalize(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_CAPITALIZE);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditInvertCase() 
{
	BeginActionRecording(TRUE);

	EventInvertCase(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_INVERT_CASE);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

/////////////////////////////////////////////////////////////////////////////
// DOCUMENT EDIT
void CCedtView::OnEditConvertTabsToSpaces() 
{
	BeginActionRecording(TRUE);

	EventConvertTabsToSpaces(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_CONVERT_TABS_TO_SPACES);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

//	SetScrollPosToMakeCaretVisible(); -- Do not reposition the cursor
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditConvertSpacesToTabs() 
{
	BeginActionRecording(TRUE);

	EventConvertSpacesToTabs(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_CONVERT_SPACES_TO_TABS);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

//	SetScrollPosToMakeCaretVisible(); -- Do not reposition the cursor
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditLeadingSpacesToTabs() 
{
	BeginActionRecording(TRUE);

	EventLeadingSpacesToTabs(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_LEADING_SPACES_TO_TABS);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

//	SetScrollPosToMakeCaretVisible(); -- Do not reposition the cursor
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditRemoveTrailingSpaces() 
{
	BeginActionRecording(TRUE);

	EventRemoveTrailingSpaces(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_EDIT_REMOVE_TRAILING_SPACES);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

//	SetScrollPosToMakeCaretVisible(); -- Do not reposition the cursor
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

/////////////////////////////////////////////////////////////////////////////
// UNDO & REDO
void CCedtView::OnEditUndo() 
{
	if( IsMacroRecording() ) { 
		CString szMessage; szMessage.Format(IDS_ERR_ILLEGAL_MACRO_DEF, "Undo");
		AfxMessageBox(szMessage); CancelMacroRecording(); return;
	} else if( ! GetUndoBufferCount() ) return;

	BeginActionRecording(FALSE);

	EventUndoLastAction(FALSE);

	EndActionRecording();
	CheckIfAllActionsAreUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnEditRedo() 
{
	if( IsMacroRecording() ) { 
		CString szMessage; szMessage.Format(IDS_ERR_ILLEGAL_MACRO_DEF, "Redo");
		AfxMessageBox(szMessage); CancelMacroRecording(); return;
	} else if( ! GetRedoBufferCount() ) return;

	BeginActionRecording(TRUE);

	EventRedoLastUndo(FALSE);

	EndActionRecording();
	CheckIfAllActionsAreUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

/////////////////////////////////////////////////////////////////////////////
// SEARCH & REPLACE
void CCedtView::OnSearchFind() 
{
	// static dialog box to remember last settings...
	static CFindDialog dlg;

	dlg.m_szFindString = GetCurrentWord();
	if( m_bSelected && ! GetSelectedLineCount() ) dlg.m_szFindString = GetSelectedString();

	if( dlg.DoModal() != IDOK ) return;

	CString szFindString = dlg.m_szFindString;
	UINT nOptions = COMPOSE_SEARCH_OPTION(dlg.m_bWholeWord, dlg.m_bMatchCase, dlg.m_bRegularExpression);

	CRegExp clsTestRegExp; // compile regular expression for test
	if( SEARCH_REG_EXP(nOptions) && ! clsTestRegExp.RegComp(szFindString) ) { 
		CString szMessage; szMessage.Format(IDS_ERR_REG_COMP_FAILED, szFindString);
		AfxMessageBox(szMessage); return; // test failed
	}

	EventFindString(szFindString, nOptions, FALSE);
	if( IsMacroRecording() ) MacroRecordFind(szFindString, nOptions);

	if( dlg.m_nDirection == 1 /* forward */ ) {
		PostMessage(WM_COMMAND, ID_SRCH_NEXT_OCCURRENCE, 0L);
	} else {
		PostMessage(WM_COMMAND, ID_SRCH_PREV_OCCURRENCE, 0L);
	}
}

void CCedtView::OnSearchReplace() 
{
	// static dialog box to remember last settings...
	static CReplaceDialog dlg;

//	dlg.m_szFindString = GetCurrentWord();
	if( m_bSelected && ! GetSelectedLineCount() ) dlg.m_szFindString = GetSelectedString();

	if( ! m_bColumnMode && m_bSelected && GetSelectedLineCount() ) dlg.m_nRange = 0;
	else dlg.m_nRange = 1;

	if( dlg.DoModal() != IDOK ) return;

	CString szFindString = dlg.m_szFindString;
	CString szReplaceString = dlg.m_szReplaceString;
	UINT nOptions = COMPOSE_SEARCH_OPTION(dlg.m_bWholeWord, dlg.m_bMatchCase, dlg.m_bRegularExpression);

	CRegExp clsTestRegExp; // compile regular expression for test
	if( SEARCH_REG_EXP(nOptions) && ! clsTestRegExp.RegComp(szFindString) ) { 
		CString szMessage; szMessage.Format(IDS_ERR_REG_COMP_FAILED, szFindString);
		AfxMessageBox(szMessage); return; // test failed
	}

	EventReplaceString(szFindString, szReplaceString, nOptions, FALSE);
	if( IsMacroRecording() ) MacroRecordReplace(szFindString, szReplaceString, nOptions);

	if( dlg.m_bReplaceAll ) {
		switch( dlg.m_nRange ) {
		case 0: // replace all in selection
			PostMessage(WM_COMMAND, ID_SRCH_REPALL_IN_SELECTION, 0L);
			break;
		case 1: // replace all in file
			PostMessage(WM_COMMAND, ID_SRCH_REPALL_IN_FILE, 0L);
			break;
		case 2: // replace all in open files
			PostMessage(WM_COMMAND, ID_SRCH_REPALL_IN_OPEN_FILES, 0L);
			break;
		}
	} else  PostMessage(WM_COMMAND, ID_SRCH_NEXT_OCCURRENCE, 0L);
}

void CCedtView::OnSearchNextWord() 
{
	EventFindCurrentString(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_SRCH_NEXT_WORD);

	PostMessage(WM_COMMAND, ID_SRCH_NEXT_OCCURRENCE, 0L);
}

void CCedtView::OnSearchPrevWord() 
{
	EventFindCurrentString(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_SRCH_PREV_WORD);

	PostMessage(WM_COMMAND, ID_SRCH_PREV_OCCURRENCE, 0L);
}

void CCedtView::OnSearchFindNext() 
{
	EventFindSelectedString(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_SRCH_FIND_NEXT);

	PostMessage(WM_COMMAND, ID_SRCH_NEXT_OCCURRENCE, 0L);
}

void CCedtView::OnSearchFindPrev() 
{
	EventFindSelectedString(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_SRCH_FIND_PREV);

	PostMessage(WM_COMMAND, ID_SRCH_PREV_OCCURRENCE, 0L);
}

void CCedtView::OnSearchNextOccurrence() 
{
	BOOL bFound = EventSearchNextOccurrence(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_SRCH_NEXT_OCCURRENCE);

	if( bFound && m_bReplaceSearch ) PostMessage(WM_COMMAND, ID_SRCH_ASK_REPLACE, 0L);

	if( bFound ) {
		if( ! IsCaretVisible() ) SetScrollPosToMakeCaretCenter();
		Invalidate(); UpdateWindow(); UpdateAllViews();
	} else {
		CString szMessage; szMessage.Format(IDS_MSG_SEARCH_NOT_FOUND, m_szFindString);
		AfxMessageBox(szMessage);
	}
}

void CCedtView::OnSearchPrevOccurrence() 
{
	BOOL bFound = EventSearchPrevOccurrence(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_SRCH_PREV_OCCURRENCE);

	if( bFound && m_bReplaceSearch ) PostMessage(WM_COMMAND, ID_SRCH_ASK_REPLACE, 0L);

	if( bFound ) {
		if( ! IsCaretVisible() ) SetScrollPosToMakeCaretCenter();
		Invalidate(); UpdateWindow(); UpdateAllViews();
	} else {
		CString szMessage; szMessage.Format(IDS_MSG_SEARCH_NOT_FOUND, m_szFindString);
		AfxMessageBox(szMessage);
	}
}

void CCedtView::OnSearchAskReplace() 
{
	if( ! m_bSelected || GetSelectedLineCount() ) return;
	CAskReplaceDialog dlg;

	INT nBegX, nBegY, nEndX, nEndY; GetSelectedPosition( nBegX, nBegY, nEndX, nEndY );
	CPoint point( nBegX-m_nScrollPosX+GetLeftMargin(), nBegY-m_nScrollPosY+GetCharHeight()+1 ); 
	ClientToScreen( & point ); dlg.SetInitialPos( point );

	if( dlg.DoModal() == IDOK ) PostMessage(WM_COMMAND, ID_SRCH_REPLACE_THIS, 0L);
	if( dlg.m_bReplaceAll     ) PostMessage(WM_COMMAND, ID_SRCH_REPALL_IN_FILE, 0L);
	if( dlg.m_bSearchNext     ) PostMessage(WM_COMMAND, ID_SRCH_NEXT_OCCURRENCE, 0L);
	if( dlg.m_bSearchPrev     ) PostMessage(WM_COMMAND, ID_SRCH_PREV_OCCURRENCE, 0L);
}

void CCedtView::OnSearchReplaceThisOccurrence() 
{
	BeginActionRecording(TRUE);

	INT nReplaced = EventReplaceThisOccurrence(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_SRCH_REPLACE_THIS);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow(); UpdateAllViews();
}

void CCedtView::OnSearchReplaceAllInSelection() 
{
	BeginActionRecording(TRUE);

	INT nReplaced = EventReplaceAllInSelection(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_SRCH_REPALL_IN_SELECTION);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	if( nReplaced ) {
		SetScrollPosToMakeCaretVisible();
		Invalidate(); UpdateWindow(); UpdateAllViews();

		CString szMessage; szMessage.Format(IDS_MSG_REPLACE_RESULT, nReplaced);
		AfxMessageBox(szMessage);
	} else {
		CString szMessage; szMessage.Format(IDS_MSG_SEARCH_NOT_FOUND, m_szFindString);
		AfxMessageBox(szMessage);
	}
}

void CCedtView::OnSearchReplaceAllInFile() 
{
	BeginActionRecording(TRUE);

	INT nReplaced = EventReplaceAllInFile(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_SRCH_REPALL_IN_FILE);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	if( nReplaced ) {
		SetScrollPosToMakeCaretVisible();
		Invalidate(); UpdateWindow(); UpdateAllViews();

		CString szMessage; szMessage.Format(IDS_MSG_REPLACE_RESULT, nReplaced);
		AfxMessageBox(szMessage);
	} else {
		CString szMessage; szMessage.Format(IDS_MSG_SEARCH_NOT_FOUND, m_szFindString);
		AfxMessageBox(szMessage);
	}
}

void CCedtView::OnSearchReplaceAllInOpenFiles() 
{
	if( IsMacroRecording() ) { 
		CString szMessage; szMessage.Format(IDS_ERR_ILLEGAL_MACRO_DEF, "Replace All In Open Files");
		AfxMessageBox(szMessage); CancelMacroRecording(); return;
	}

	CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();
	if( ! pFrame->CanUseOutputWindow() ) {
		AfxMessageBox(IDS_ERR_OUTPUT_WINDOW_OCCUPIED, MB_OK | MB_ICONSTOP); return;
	}

	if( ! pFrame->IsOutputWindowVisible() ) pFrame->ShowOutputWindow(TRUE);

	pFrame->SetOutputWindowOccupied(TRUE);
	pFrame->EnableOutputWindowInput(FALSE);

	pFrame->ClearOutputWindowContents();

	CString szMessage; szMessage.LoadString(IDS_OUT_REPLACE_TITLE);
	pFrame->AddStringToOutputWindow( szMessage, RGB(0, 0, 0) );

	CCedtApp * pApp = (CCedtApp *)AfxGetApp();
	POSITION posDoc = pApp->GetFirstDocPosition();
	while( posDoc ) {
		CCedtDoc * pDoc = (CCedtDoc *)pApp->GetNextDoc( posDoc );
		CCedtView * pView = (CCedtView *)pDoc->GetFirstView();

		pView->SendMessage(WM_COMMAND, ID_SRCH_REPALL_IN_FILE_OUTPUT, 0L);
	}

	pFrame->SetOutputWindowOccupied(FALSE);
	pFrame->EnableOutputWindowInput(FALSE);
}

void CCedtView::OnSearchReplaceAllInFileOutputWindow() 
{
	CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pFrame );
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument(); ASSERT( pDoc );

	BeginActionRecording(TRUE);

	INT nReplaced = EventReplaceAllInFile(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_SRCH_REPALL_IN_FILE);

	EndActionRecording();
	EmptyRedoBuffer(); CheckIfAllActionsCanBeUndone();

	if( nReplaced ) {
		SetScrollPosToMakeCaretVisible();
		Invalidate(); UpdateWindow(); UpdateAllViews();

		CString szMessage; szMessage.Format(IDS_OUT_REPLACE_RESULT, pDoc->GetTitle(), nReplaced);
		pFrame->AddStringToOutputWindow( szMessage, RGB(0, 0, 0) );
	} else {
		CString szMessage; szMessage.Format(IDS_OUT_REPLACE_NOT_FOUND, pDoc->GetTitle(), m_szFindString);
		pFrame->AddStringToOutputWindow( szMessage, RGB(128, 128, 128) );
	}
}

void CCedtView::OnSearchGoTo() 
{
	CGoToDialog dlg;

	dlg.m_szRange.Format(IDS_CTRL_GO_TO_DIALOG, 1, GetLastIdxY() + 1);
	dlg.m_nLineNumber = GetIdxYFromPosY( m_nCaretPosY ) + 1;

	if( dlg.DoModal() == IDOK ) OnSearchGoTo( dlg.m_nLineNumber );
}

void CCedtView::OnSearchGoTo(INT nLineNumber)
{
	BOOL bRedraw = EventGoToLine(nLineNumber, FALSE);
	if( IsMacroRecording() ) MacroRecordGoTo(nLineNumber);

	if( ! IsCaretVisible() ) { 
		SetScrollPosToMakeCaretCenter(); 
		Invalidate(); UpdateWindow(); UpdateAllViews();
	} else if( bRedraw ) {
		Invalidate(); UpdateWindow(); UpdateAllViews();
	} else UpdateCaretPosition();
}

void CCedtView::OnSearchToggleBookmark() 
{
	BOOL bRedraw = EventToggleBookmark(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_SRCH_TOGGLE_BOOKMARK);

	SetScrollPosToMakeCaretVisible();
	if( bRedraw ) {
		Invalidate(); UpdateWindow(); UpdateAllViews();
	} else UpdateCaretPosition();
}

void CCedtView::OnSearchNextBookmark() 
{
	BOOL bRedraw = EventNextBookmark(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_SRCH_NEXT_BOOKMARK);

	if( ! IsCaretVisible() ) { 
		SetScrollPosToMakeCaretCenter(); 
		Invalidate(); UpdateWindow(); UpdateAllViews();
	} else if( bRedraw ) {
		Invalidate(); UpdateWindow(); UpdateAllViews();
	} else UpdateCaretPosition();
}

void CCedtView::OnSearchPrevBookmark() 
{
	BOOL bRedraw = EventPrevBookmark(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_SRCH_PREV_BOOKMARK);

	if( ! IsCaretVisible() ) {
		SetScrollPosToMakeCaretCenter();
		Invalidate(); UpdateWindow(); UpdateAllViews();
	} else if( bRedraw ) {
		Invalidate(); UpdateWindow(); UpdateAllViews();
	} else UpdateCaretPosition();
}

void CCedtView::OnSearchPrevEdit() 
{
	if( ! GetUndoBufferCount() ) return;

	BOOL bRedraw = EventPrevEditingPosition(FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_SRCH_PREV_EDIT);

	if( ! IsCaretVisible() ) {
		SetScrollPosToMakeCaretCenter();
		Invalidate(); UpdateWindow(); UpdateAllViews();
	} else if( bRedraw ) {
		Invalidate(); UpdateWindow(); UpdateAllViews();
	} else UpdateCaretPosition();
}

void CCedtView::OnSearchPairsBegin() 
{
	BOOL bRedraw = EventPairsBeginPosition(0x00, FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_SRCH_PAIRS_BEGIN);

	if( ! IsCaretVisible() ) {
		SetScrollPosToMakeCaretCenter();
		Invalidate(); UpdateWindow(); UpdateAllViews();
	} else if( bRedraw ) {
		Invalidate(); UpdateWindow(); UpdateAllViews();
	} else UpdateCaretPosition();
}

void CCedtView::OnSearchPairsEnd() 
{
	BOOL bRedraw = EventPairsEndPosition(0x00, FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_SRCH_PAIRS_END);

	if( ! IsCaretVisible() ) {
		SetScrollPosToMakeCaretCenter();
		Invalidate(); UpdateWindow(); UpdateAllViews();
	} else if( bRedraw ) {
		Invalidate(); UpdateWindow(); UpdateAllViews();
	} else UpdateCaretPosition();
}

void CCedtView::OnSearchPairsBeginSelect() 
{
	BOOL bRedraw = EventPairsBeginPosition(KEYSTATE_SHIFT, FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_SRCH_PAIRS_BEGIN_SELECT);

	if( ! IsCaretVisible() ) {
		SetScrollPosToMakeCaretCenter();
		Invalidate(); UpdateWindow(); UpdateAllViews();
	} else if( bRedraw ) {
		Invalidate(); UpdateWindow(); UpdateAllViews();
	} else UpdateCaretPosition();
}

void CCedtView::OnSearchPairsEndSelect() 
{
	BOOL bRedraw = EventPairsEndPosition(KEYSTATE_SHIFT, FALSE);
	if( IsMacroRecording() ) MacroRecordCommand(ID_SRCH_PAIRS_END_SELECT);

	if( ! IsCaretVisible() ) {
		SetScrollPosToMakeCaretCenter();
		Invalidate(); UpdateWindow(); UpdateAllViews();
	} else if( bRedraw ) {
		Invalidate(); UpdateWindow(); UpdateAllViews();
	} else UpdateCaretPosition();
}

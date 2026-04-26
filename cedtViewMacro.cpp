#include "stdafx.h"
#include "cedtHeader.h"
#include "SortStringArray.h"



BOOL CCedtView::RefreshMacroBufferFilePathForMenu()
{
	for( INT i = 0; i < 8; i++ ) m_szMacroBufferFilePath[i] = "";

	CSortStringArray arrPathName;
	CString szPathName = CCedtApp::m_szInstallDirectory + "\\tools\\*.macro";

	BOOL bFound = FindAllFilePath(arrPathName, szPathName);
	arrPathName.QuickSort();

	INT size = arrPathName.GetSize(); if( size > 8 ) size = 8;
	for( i = 0; i < size; i++ ) m_szMacroBufferFilePath[i] = arrPathName[i];

	return TRUE;
}

CString CCedtView::GetMacroName(INT nMacro)
{
	CMacroBuffer & rBuffer = m_clsMacroBuffer[nMacro];
	if( rBuffer.GetBufferCount() ) {
		if( rBuffer.m_szName.GetLength() ) return rBuffer.m_szName;
		else return "[Noname]";
	} else return "- Empty -";
}

CString CCedtView::GetMacroHotKeyText(INT nMacro)
{
	CMacroBuffer & rBuffer = m_clsMacroBuffer[nMacro];
	CString szHotKeyText = rBuffer.GetHotKeyText();
	if( ! szHotKeyText.GetLength() ) szHotKeyText.Format("Alt+%d", nMacro % 10);
	return szHotKeyText;
}


BOOL CCedtView::TranslateMessageForMacroBuffer(MSG * pMsg)
{
	if( pMsg->message != WM_KEYDOWN && pMsg->message != WM_SYSKEYDOWN ) return FALSE;

	UINT nFlags = GetKeyState();
	BOOL bShift = (nFlags & KEYSTATE_SHIFT) ? TRUE : FALSE;
	BOOL bCtrl = (nFlags & KEYSTATE_CONTROL) ? TRUE : FALSE;
	BOOL bAlt = (nFlags & KEYSTATE_MENU) ? TRUE : FALSE;

	for( INT i = 1; i <= 10; i++ ) {
		CMacroBuffer & rBuffer = m_clsMacroBuffer[i];
		BOOL bShift2 = (rBuffer.m_wModifiers & HOTKEYF_SHIFT) ? TRUE : FALSE;
		BOOL bCtrl2 = (rBuffer.m_wModifiers & HOTKEYF_CONTROL) ? TRUE : FALSE;
		BOOL bAlt2 = (rBuffer.m_wModifiers & HOTKEYF_ALT) ? TRUE : FALSE;

		if( rBuffer.m_wVirtualKeyCode != pMsg->wParam ) continue;
		if( ! rBuffer.GetBufferCount() ) continue;
		if( bShift != bShift2 || bCtrl != bCtrl2 || bAlt != bAlt2 ) continue;

		return PostMessage(WM_COMMAND, ID_MACRO_REPLAY0 + i, 0L);
	}

	return FALSE;
}

void CCedtView::ActionReplayMacro(INT nMacro, INT nTimes)
{
	POSITION posAction, posParam, posFlags, posString;
	INT nAction; UINT nParam, nFlags, nOptions; 
	CString szString, szReplace;

	CMacroBuffer & rBuffer = m_clsMacroBuffer[nMacro];

	while( nTimes-- ) {
		posAction = rBuffer.m_lstAction.GetHeadPosition();
		posParam = rBuffer.m_lstParam.GetHeadPosition();
		posFlags = rBuffer.m_lstFlags.GetHeadPosition();
		posString = rBuffer.m_lstString.GetHeadPosition();

		while( posAction ) {
			nAction = rBuffer.m_lstAction.GetNext( posAction );

			switch( nAction ) {
			case MACRO_MOVE:
				nParam = rBuffer.m_lstParam.GetNext( posParam );
				nFlags = rBuffer.m_lstFlags.GetNext( posFlags );
				EventMoveKeyDown(nParam, nFlags, TRUE);
				break;

			case MACRO_CHAR:
				nParam = rBuffer.m_lstParam.GetNext( posParam );
				EventInsertChar(nParam, TRUE);
				break;

			case MACRO_STRING:
				szString = rBuffer.m_lstString.GetNext( posString );
				EventInsertString(szString, TRUE);
				break;

			case MACRO_FILE:
				szString = rBuffer.m_lstString.GetNext( posString );
				EventInsertFile(szString, TRUE);
				break;

			case MACRO_FIND:
				szString = rBuffer.m_lstString.GetNext( posString );
				nOptions = rBuffer.m_lstParam.GetNext( posParam );
				EventFindString(szString, nOptions, TRUE);
				break;

			case MACRO_REPLACE:
				szString = rBuffer.m_lstString.GetNext( posString );
				szReplace = rBuffer.m_lstString.GetNext( posString );
				nOptions = rBuffer.m_lstParam.GetNext( posParam );
				EventReplaceString(szString, szReplace, nOptions, TRUE);
				break;

			case MACRO_GOTO:
				nParam = rBuffer.m_lstParam.GetNext( posParam );
				EventGoToLine(nParam, TRUE);
				break;

			case MACRO_COMMAND:
				nParam = rBuffer.m_lstParam.GetNext( posParam );

				switch( nParam ) {
				case ID_EDIT_ESCAPE:						EventCommandEscape(TRUE);				break;
				case ID_EDIT_RETURN:						EventCommandReturn(TRUE);				break;
				case ID_EDIT_BACK: 							EventCommandBack(TRUE); 				break; 
				case ID_EDIT_DELETE: 						EventCommandDelete(TRUE); 				break;
				case ID_EDIT_TAB: 							EventCommandTab(TRUE);					break;
				case ID_EDIT_DETAB:							EventCommandDetab(TRUE);				break;
				case ID_EDIT_INCREASE_INDENT:				EventIncreaseIndent(TRUE);				break;
				case ID_EDIT_DECREASE_INDENT:				EventDecreaseIndent(TRUE);				break;
				case ID_EDIT_MAKE_COMMENT:					EventMakeComment(TRUE);					break;
				case ID_EDIT_RELEASE_COMMENT:				EventReleaseComment(TRUE);				break;
				case ID_EDIT_JOIN_LINES:					EventJoinLines(TRUE);					break;
				case ID_EDIT_SPLIT_LINE:					EventSplitLine(TRUE);					break;
				case ID_EDIT_DELETE_WORD:					EventDeleteWord(TRUE);					break;
				case ID_EDIT_DELETE_PREV_WORD:				EventDeletePrevWord(TRUE);				break;
				case ID_EDIT_DELETE_TO_END_OF_LINE:			EventDeleteToEndOfLine(TRUE);			break;
				case ID_EDIT_DELETE_TO_BEGIN_OF_LINE:		EventDeleteToBeginOfLine(TRUE);			break;
				case ID_EDIT_DELETE_LINE:					EventDeleteLine(TRUE);					break;
				case ID_EDIT_DUPLICATE_LINE:				EventDuplicateLine(TRUE);				break;
				case ID_EDIT_CUT:							EventCommandCut(TRUE);					break;
				case ID_EDIT_COPY:							EventCommandCopy(TRUE);					break;
				case ID_EDIT_COPY_FILE_PATH:				EventCommandCopyFilePath(TRUE);			break;
				case ID_EDIT_CUT_APPEND:					EventCommandCutAppend(TRUE);			break;
				case ID_EDIT_COPY_APPEND:					EventCommandCopyAppend(TRUE);			break;
				case ID_EDIT_PASTE:							EventCommandPaste(TRUE);				break;
				case ID_EDIT_INSERT_DATE:					EventInsertString(GetCurrentDate() + CString(" "), TRUE);	break;
				case ID_EDIT_INSERT_TIME:					EventInsertString(GetCurrentTime() + CString(" "), TRUE);	break;
				case ID_EDIT_SELECT_ALL:					EventSelectAll(TRUE);					break;
				case ID_EDIT_SELECT_LINE:					EventSelectLine(TRUE);					break;
				case ID_EDIT_SELECT_WORD:					EventSelectWord(TRUE);					break;
				case ID_EDIT_SELECT_BLOCK:					EventSelectBlock(TRUE);					break;
				case ID_EDIT_UPPER_CASE:					EventUpperCase(TRUE);					break;
				case ID_EDIT_LOWER_CASE:					EventLowerCase(TRUE);					break;
				case ID_EDIT_CAPITALIZE:					EventCapitalize(TRUE);					break;
				case ID_EDIT_INVERT_CASE:					EventInvertCase(TRUE);					break;
				case ID_EDIT_CONVERT_TABS_TO_SPACES:		EventConvertTabsToSpaces(TRUE);			break;
				case ID_EDIT_CONVERT_SPACES_TO_TABS:		EventConvertSpacesToTabs(TRUE);			break;
				case ID_EDIT_LEADING_SPACES_TO_TABS:		EventLeadingSpacesToTabs(TRUE);			break;
				case ID_EDIT_REMOVE_TRAILING_SPACES:		EventRemoveTrailingSpaces(TRUE);		break;
				case ID_EDIT_UNDO:							/* cannot be included in a macro */		break;
				case ID_EDIT_REDO:							/* cannot be included in a macro */		break;
				case ID_TOOL_EVALUATE_LINE:					EventEvaluateLine(TRUE);				break;
				case ID_MACRO_REPLAY:						/* cannot be included in a macro */		break;
				case ID_SRCH_NEXT_WORD:						EventFindCurrentString(TRUE);			break;
				case ID_SRCH_PREV_WORD:						EventFindCurrentString(TRUE);			break;
				case ID_SRCH_FIND_NEXT:						EventFindSelectedString(TRUE);			break;
				case ID_SRCH_FIND_PREV:						EventFindSelectedString(TRUE);			break;
				case ID_SRCH_NEXT_OCCURRENCE:				EventSearchNextOccurrence(TRUE);		break;
				case ID_SRCH_PREV_OCCURRENCE:				EventSearchPrevOccurrence(TRUE);		break;
				case ID_SRCH_REPLACE_THIS:					EventReplaceThisOccurrence(TRUE);		break;
				case ID_SRCH_REPALL_IN_SELECTION:			EventReplaceAllInSelection(TRUE);		break;
				case ID_SRCH_REPALL_IN_FILE:				EventReplaceAllInFile(TRUE);			break;
				case ID_SRCH_REPALL_IN_OPEN_FILES:			/* cannot be included in a macro */		break;
				case ID_SRCH_TOGGLE_BOOKMARK:				EventToggleBookmark(TRUE);				break;
				case ID_SRCH_NEXT_BOOKMARK:					EventNextBookmark(TRUE);				break;
				case ID_SRCH_PREV_BOOKMARK:					EventPrevBookmark(TRUE);				break;
				case ID_SRCH_PREV_EDIT:						EventPrevEditingPosition(TRUE);			break;
				case ID_SRCH_PAIRS_BEGIN:					EventPairsBeginPosition(0x00, TRUE);	break;
				case ID_SRCH_PAIRS_END:						EventPairsEndPosition(0x00, TRUE);		break;
				case ID_SRCH_PAIRS_BEGIN_SELECT:			EventPairsBeginPosition(KEYSTATE_SHIFT, TRUE);	break;
				case ID_SRCH_PAIRS_END_SELECT:				EventPairsEndPosition(KEYSTATE_SHIFT, TRUE);	break;
				}
				break;
			}

		//	if( m_bPairHighlighted[0] || m_bPairHighlighted[1] ) UnhighlightPairs();
			if( m_bPairMatched ) UnhighlightPairs();

		//	if( m_bEnablePairsMatching && ! m_bSelected ) HighlightMatchingPairs();
			if( m_bEnablePairsMatching ) HighlightMatchingPairs();
		}
	}
}

void CCedtView::BeginMacroRecording(INT nMacroRecordingNumber, LPCTSTR lpszMacroName)
{
	m_nMacroRecordingNumber = nMacroRecordingNumber;
	m_szMacroName = lpszMacroName;

	EmptyMacroBuffer(0);
	m_bMacroRecording = TRUE;

	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	pMainFrame->SetMacroRecordingFlag(TRUE);
}

void CCedtView::EndMacroRecording()
{
	m_clsMacroBuffer[m_nMacroRecordingNumber].CopyContents(m_clsMacroBuffer[0]);
	m_clsMacroBuffer[m_nMacroRecordingNumber].m_szName = m_szMacroName;

	m_bMacroRecording = FALSE;

	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	pMainFrame->SetMacroRecordingFlag(FALSE);
}

void CCedtView::CancelMacroRecording()
{
	EmptyMacroBuffer(0);
	m_bMacroRecording = FALSE;

	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	pMainFrame->SetMacroRecordingFlag(FALSE);
}

void CCedtView::MacroRecordMove(UINT nChar, UINT nFlags)
{
	m_clsMacroBuffer[0].m_lstAction.AddTail(MACRO_MOVE);
	m_clsMacroBuffer[0].m_lstParam.AddTail(nChar);
	m_clsMacroBuffer[0].m_lstFlags.AddTail(nFlags);
}

void CCedtView::MacroRecordChar(UINT nChar)
{
	m_clsMacroBuffer[0].m_lstAction.AddTail(MACRO_CHAR);
	m_clsMacroBuffer[0].m_lstParam.AddTail(nChar);
}

void CCedtView::MacroRecordString(LPCTSTR lpszString)
{
	m_clsMacroBuffer[0].m_lstAction.AddTail(MACRO_STRING);
	m_clsMacroBuffer[0].m_lstString.AddTail(lpszString);
}

void CCedtView::MacroRecordFile(LPCTSTR lpszPathName)
{
	m_clsMacroBuffer[0].m_lstAction.AddTail(MACRO_FILE);
	m_clsMacroBuffer[0].m_lstString.AddTail(lpszPathName);
}

void CCedtView::MacroRecordFind(LPCTSTR lpszFindString, UINT nOptions)
{
	m_clsMacroBuffer[0].m_lstAction.AddTail(MACRO_FIND);
	m_clsMacroBuffer[0].m_lstString.AddTail(lpszFindString);
	m_clsMacroBuffer[0].m_lstParam.AddTail(nOptions);
}

void CCedtView::MacroRecordReplace(LPCTSTR lpszFindString, LPCTSTR lpszReplaceString, UINT nOptions)
{
	m_clsMacroBuffer[0].m_lstAction.AddTail(MACRO_REPLACE);
	m_clsMacroBuffer[0].m_lstString.AddTail(lpszFindString);
	m_clsMacroBuffer[0].m_lstString.AddTail(lpszReplaceString);
	m_clsMacroBuffer[0].m_lstParam.AddTail(nOptions);
}

void CCedtView::MacroRecordGoTo(UINT nLineNumber)
{
	m_clsMacroBuffer[0].m_lstAction.AddTail(MACRO_GOTO);
	m_clsMacroBuffer[0].m_lstParam.AddTail(nLineNumber);
}

void CCedtView::MacroRecordCommand(UINT nCommand)
{
	m_clsMacroBuffer[0].m_lstAction.AddTail(MACRO_COMMAND);
	m_clsMacroBuffer[0].m_lstParam.AddTail(nCommand);
}

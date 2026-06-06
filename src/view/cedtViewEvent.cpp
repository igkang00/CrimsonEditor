#include "stdafx.h"
#include "cedtHeader.h"


BOOL CCedtView::EventMoveKeyDown(UINT nChar, UINT nFlags, BOOL bMacro)
{
	BOOL bShift = nFlags & KEYSTATE_SHIFT;
	BOOL bRedraw = FALSE;

	if( ! m_bSelected && bShift ) {
		m_nAnchorPosX = m_nCaretPosX; 
		m_nAnchorPosY = m_nCaretPosY;
	}

	INT nBegX, nBegY, nEndX, nEndY;
	if( m_bSelected ) GetSelectedPosition( nBegX, nBegY, nEndX, nEndY );

	switch( nChar ) {
	case VK_LEFT:
		if( m_bSelected && ! bShift ) { SetCaretPosY( nBegY ); SetCaretPosX( nBegX ); }
		else bRedraw = ActionLeftKey(nFlags);
		break;

	case VK_RIGHT:
		if( m_bSelected && ! bShift ) { SetCaretPosY( nEndY ); SetCaretPosX( nEndX ); }
		else bRedraw = ActionRightKey(nFlags);
		break;

	case VK_UP:
		if( m_bSelected && ! bShift ) { SetCaretPosY( nBegY ); SetCaretPosX( nBegX ); }
		bRedraw = ActionUpKey(nFlags);
		break;

	case VK_DOWN:
		if( m_bSelected && ! bShift ) { SetCaretPosY( nEndY ); SetCaretPosX( nEndX ); }
		bRedraw = ActionDownKey(nFlags);
		break;

	case VK_HOME:
		if( m_bSelected && ! bShift ) { SetCaretPosY( nBegY ); SetCaretPosX( nBegX ); }
		bRedraw = ActionHomeKey(nFlags);
		break;

	case VK_END:
		if( m_bSelected && ! bShift ) { SetCaretPosY( nEndY ); SetCaretPosX( nEndX ); }
		bRedraw = ActionEndKey(nFlags);
		break;

	case VK_PRIOR:
		if( m_bSelected && ! bShift ) { SetCaretPosY( nBegY ); SetCaretPosX( nBegX ); }
		bRedraw = ActionPriorKey(nFlags);
		break;

	case VK_NEXT:
		if( m_bSelected && ! bShift ) { SetCaretPosY( nEndY ); SetCaretPosX( nEndX ); }
		bRedraw = ActionNextKey(nFlags);
		break;
	}

	if( bShift ) {
		m_bSelected = ( (m_nAnchorPosY != m_nCaretPosY) || (m_nAnchorPosX != m_nCaretPosX) );
		bRedraw = TRUE;
	} else if( m_bSelected ) {
		m_bSelected = FALSE;
		bRedraw = TRUE;
	}

	return bRedraw;
}

void CCedtView::EventInsertChar(UINT nChar, BOOL bMacro)
{
	if( m_bColumnMode ) {
		if( m_bSelected && m_nCaretPosX != m_nAnchorPosX ) { ActionDeleteColumnSelection(); m_bSelected = (m_nCaretPosY != m_nAnchorPosY); }
		if( m_bSelected ) ActionInsertColumnChar( nChar );
		else ActionInsertChar( nChar );
	} else {
		if( m_bSelected ) { ActionDeleteLineSelection(); m_bSelected = FALSE; }
		ActionInsertChar( nChar );
	}
}

void CCedtView::EventInsertString(LPCTSTR lpszString, BOOL bMacro)
{
	if( m_bColumnMode ) {
		if( m_bSelected ) {
			if( m_nCaretPosX != m_nAnchorPosX ) ActionDeleteColumnSelection();
			m_bSelected = FALSE;
		}
		ActionInsertString( lpszString );
	} else {
		if( m_bSelected ) { ActionDeleteLineSelection(); m_bSelected = FALSE; }
		ActionInsertString( lpszString );
	}
}

void CCedtView::EventInsertFile(LPCTSTR lpszPathName, BOOL bMacro)
{
	CMemText Block; Block.FileLoad( lpszPathName );

	if( m_bColumnMode ) {
		if( m_bSelected ) {
			if( m_nCaretPosX != m_nAnchorPosX ) ActionDeleteColumnSelection();
			m_bSelected = FALSE;
		}
		ActionPasteColumnSelection( Block );
	} else {
		if( m_bSelected ) { ActionDeleteLineSelection(); m_bSelected = FALSE; }
		ActionPasteLineSelection( Block );
	}
}

void CCedtView::EventCompositionStart(BOOL bMacro)
{
	ActionCompositionStart();
}

void CCedtView::EventCompositionEnd(BOOL bMacro)
{
	ActionCompositionEnd();
}

void CCedtView::EventCompositionCompose(LPCTSTR lpszString, BOOL bMacro)
{
	if( m_bColumnMode ) {
		if( m_bSelected ) {
			if( m_nCaretPosX != m_nAnchorPosX ) ActionDeleteColumnSelection();
			m_bSelected = FALSE;
		}
		ActionCompositionCompose(lpszString);
	} else {
		if( m_bSelected ) { ActionDeleteLineSelection(); m_bSelected = FALSE; }
		ActionCompositionCompose(lpszString);
	}
}

void CCedtView::EventCompositionResult(LPCTSTR lpszString, BOOL bMacro)
{
	// there should no selection before this function call
	ActionCompositionResult(lpszString);
}

void CCedtView::EventCommandEscape(BOOL bMacro)
{
	// just release current selection
	if( m_bSelected ) m_bSelected = FALSE;
}

void CCedtView::EventCommandReturn(BOOL bMacro)
{
	if( m_bColumnMode ) {
		if( m_bSelected ) {
			if( m_nCaretPosX == m_nAnchorPosX ) { ActionWrongOperation(! bMacro); m_bSelected = TRUE; }
			else { ActionDeleteColumnSelection(); m_bSelected = (m_nCaretPosY != m_nAnchorPosY); }
		} else ActionCarrigeReturn();
	} else {
		if( m_bSelected ) { ActionDeleteLineSelection(); m_bSelected = FALSE; }
		ActionCarrigeReturn();
	}
}

void CCedtView::EventCommandBack(BOOL bMacro)
{
	if( m_bColumnMode ) {
		if( m_bSelected ) {
			if( m_nCaretPosX == m_nAnchorPosX ) { ActionDeleteColumnPrevChar(); m_bSelected = TRUE; }
			else { ActionDeleteColumnSelection(); m_bSelected = (m_nCaretPosY != m_nAnchorPosY); }
		} else ActionBackspace();
	} else {
		if( m_bSelected ) { ActionDeleteLineSelection(); m_bSelected = FALSE; }
		else ActionBackspace();
	}
}

void CCedtView::EventCommandDelete(BOOL bMacro)
{
	if( m_bColumnMode ) {
		if( m_bSelected ) {
			if( m_nCaretPosX == m_nAnchorPosX ) { ActionDeleteColumnChar(); m_bSelected = TRUE; }
			else { ActionDeleteColumnSelection(); m_bSelected = (m_nCaretPosY != m_nAnchorPosY); }
		} else ActionDeleteChar();
	} else {
		if( m_bSelected ) { ActionDeleteLineSelection(); m_bSelected = FALSE; }
		else ActionDeleteChar();
	}
}

void CCedtView::EventCommandTab(BOOL bMacro)
{
	if( m_bColumnMode ) {
		if( m_bSelected ) { 
			if( m_nCaretPosX != m_nAnchorPosX ) ActionDeleteColumnSelection();
			if( m_bUseSpacesInPlaceOfTab ) ActionInsertColumnSpacesInPlaceOfTab();
			else ActionInsertColumnChar('\t'); 
			m_bSelected = (m_nCaretPosY != m_nAnchorPosY); 
		} else {
			if( m_bUseSpacesInPlaceOfTab ) ActionInsertSpacesInPlaceOfTab();
			else ActionInsertChar('\t');
		}
	} else {
		if( ! GetSelectedLineCount() ) {
			if( m_bSelected ) { ActionDeleteLineSelection(); m_bSelected = FALSE; }
			if( m_bUseSpacesInPlaceOfTab ) ActionInsertSpacesInPlaceOfTab();
			else ActionInsertChar('\t');
		} else ActionIndentLineSelection();
	}
}

void CCedtView::EventCommandDetab(BOOL bMacro)
{
	if( m_bColumnMode ) {
		if( m_bSelected ) m_bSelected = FALSE;
		ActionDetabCaret();
	} else {
		if( ! GetSelectedLineCount() ) {
			if( m_bSelected ) m_bSelected = FALSE;
			ActionDetabCaret();
		} else ActionUnindentLineSelection();
	}
}

void CCedtView::EventIncreaseIndent(BOOL bMacro)
{
	if( ! m_bColumnMode ) {
		if( ! GetSelectedLineCount() ) {
			if( m_bSelected ) m_bSelected = FALSE;
			ActionIndentLine();
		} else ActionIndentLineSelection();
	} else ActionWrongOperation(! bMacro);
}

void CCedtView::EventDecreaseIndent(BOOL bMacro)
{
	if( ! m_bColumnMode ) {
		if( ! GetSelectedLineCount() ) {
			if( m_bSelected ) m_bSelected = FALSE;
			ActionUnindentLine();
		} else ActionUnindentLineSelection();
	} else ActionWrongOperation(! bMacro);
}

void CCedtView::EventMakeComment(BOOL bMacro)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();

	if( pDoc->HasLineCommentDelimiter() ) {
		if( ! m_bColumnMode ) {
			if( ! GetSelectedLineCount() ) {
				if( m_bSelected ) m_bSelected = FALSE;
				ActionMakeCommentLine();
			} else ActionMakeCommentLineSelection();
		} else ActionWrongOperation(! bMacro);
	} else ActionWrongOperation(! bMacro);
}

void CCedtView::EventReleaseComment(BOOL bMacro)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();

	if( pDoc->HasLineCommentDelimiter() ) {
		if( ! m_bColumnMode ) {
			if( ! GetSelectedLineCount() ) {
				if( m_bSelected ) m_bSelected = FALSE;
				ActionReleaseCommentLine();
			} else ActionReleaseCommentLineSelection();
		} else ActionWrongOperation(! bMacro);
	} else ActionWrongOperation(! bMacro);
}

void CCedtView::EventJoinLines(BOOL bMacro)
{
	// we need to expand this function to join multiple lines
	if( m_bSelected ) m_bSelected = FALSE;
	ActionJoinLines();
}

void CCedtView::EventSplitLine(BOOL bMacro)
{
	// we need to expand this function to split multiple lines
	if( m_bSelected ) m_bSelected = FALSE;
	ActionSplitLine();
}

void CCedtView::EventDeleteWord(BOOL bMacro)
{
	if( m_bColumnMode ) {
		if( m_bSelected ) {
			if( m_nCaretPosX == m_nAnchorPosX ) { ActionDeleteColumnChar(); m_bSelected = TRUE; }
			else { ActionDeleteColumnSelection(); m_bSelected = (m_nCaretPosY != m_nAnchorPosY); }
		} else ActionDeleteWord();
	} else {
		if( m_bSelected ) { ActionDeleteLineSelection(); m_bSelected = FALSE; }
		else ActionDeleteWord();
	}
}

void CCedtView::EventDeletePrevWord(BOOL bMacro)
{
	if( m_bColumnMode ) {
		if( m_bSelected ) {
			if( m_nCaretPosX == m_nAnchorPosX ) { ActionDeleteColumnPrevChar(); m_bSelected = TRUE; }
			else { ActionDeleteColumnSelection(); m_bSelected = (m_nCaretPosY != m_nAnchorPosY); }
		} else ActionDeletePrevWord();
	} else {
		if( m_bSelected ) { ActionDeleteLineSelection(); m_bSelected = FALSE; }
		else ActionDeletePrevWord();
	}
}

void CCedtView::EventDeleteToEndOfLine(BOOL bMacro)
{
	if( m_bColumnMode ) {
		if( m_bSelected ) {
			if( m_nCaretPosX == m_nAnchorPosX ) { ActionDeleteColumnToEndOfLine(); m_bSelected = TRUE; }
			else { ActionDeleteColumnSelection(); m_bSelected = (m_nCaretPosY != m_nAnchorPosY); }
		} else ActionDeleteToEndOfLine();
	} else {
		if( m_bSelected ) { ActionDeleteLineSelection(); m_bSelected = FALSE; }
		else ActionDeleteToEndOfLine();
	}
}

void CCedtView::EventDeleteToBeginOfLine(BOOL bMacro)
{
	if( m_bColumnMode ) {
		if( m_bSelected ) {
			if( m_nCaretPosX == m_nAnchorPosX ) { ActionDeleteColumnToBeginOfLine(); m_bSelected = TRUE; }
			else { ActionDeleteColumnSelection(); m_bSelected = (m_nCaretPosY != m_nAnchorPosY); }
		} else ActionDeleteToBeginOfLine();
	} else {
		if( m_bSelected ) { ActionDeleteLineSelection(); m_bSelected = FALSE; }
		else ActionDeleteToBeginOfLine();
	}
}

void CCedtView::EventDeleteLine(BOOL bMacro)
{
	if( m_bColumnMode ) {
		if( m_bSelected ) {
			if( m_nCaretPosX == m_nAnchorPosX ) ActionWrongOperation(! bMacro);
			else { ActionDeleteColumnSelection(); m_bSelected = (m_nCaretPosY != m_nAnchorPosY); }
		} else ActionDeleteLine();
	} else {
		if( m_bSelected ) { ActionDeleteLineSelection(); m_bSelected = FALSE; }
		else ActionDeleteLine();
	}
}

void CCedtView::EventDuplicateLine(BOOL bMacro)
{
	if( m_bSelected ) m_bSelected = FALSE;
	ActionDuplicateLine();
}

void CCedtView::EventCommandCut(BOOL bMacro)
{
	if( m_bColumnMode ) {
		if( m_bSelected ) {
			if( m_nCaretPosX != m_nAnchorPosX ) {
				CMemText Block; ActionCopyColumnSelection( Block ); 
				ActionDeleteColumnSelection(); 
				SetClipboardData( Block );
				m_bSelected = (m_nCaretPosY != m_nAnchorPosY); 
			} else ActionWrongOperation(! bMacro);
		} else ActionWrongOperation(! bMacro);
	} else {
		if( m_bSelected ) { 
			CMemText Block; ActionCopyLineSelection( Block ); 
			ActionDeleteLineSelection(); 
			SetClipboardData( Block );
			m_bSelected = FALSE; 
		} else if( m_bCopyCurrentLineIfNothingSelected ) {
			CMemText Block; ActionCopyLine( Block ); 
			ActionDeleteLine(); 
			SetClipboardData( Block );
		} else ActionWrongOperation(! bMacro);
	}
}

void CCedtView::EventCommandCopy(BOOL bMacro)
{
	if( m_bColumnMode ) {
		if( m_bSelected ) {
			if( m_nCaretPosX != m_nAnchorPosX ) {
				CMemText Block; ActionCopyColumnSelection( Block );
				SetClipboardData( Block );
			} else ActionWrongOperation(! bMacro);
		} else ActionWrongOperation(! bMacro);
	} else {
		if( m_bSelected ) {
			CMemText Block; ActionCopyLineSelection( Block );
			SetClipboardData( Block );
		} else if( m_bCopyCurrentLineIfNothingSelected ) {
			CMemText Block; ActionCopyLine( Block );
			SetClipboardData( Block );
		} else ActionWrongOperation(! bMacro);
	}
}

void CCedtView::EventCommandCopyFilePath(BOOL bMacro)
{
	CMemText Block; ActionCopyFilePath( Block );
	SetClipboardData( Block );
}

void CCedtView::EventCommandCutAppend(BOOL bMacro)
{
	CMemText Block; GetClipboardData( Block );

	if( ! m_bColumnMode ) {
		if( m_bSelected ) { 
			CMemText Blck2; ActionCopyLineSelection( Blck2 ); 
			ActionDeleteLineSelection(); 
			Block.AppendText( Blck2 );
			SetClipboardData( Block );
			m_bSelected = FALSE; 
		} else if( m_bCopyCurrentLineIfNothingSelected ) {
			CMemText Blck2; ActionCopyLine( Blck2 ); 
			ActionDeleteLine(); 
			Block.AppendText( Blck2 );
			SetClipboardData( Block );
		} else ActionWrongOperation(! bMacro);
	} else ActionWrongOperation(! bMacro);
}

void CCedtView::EventCommandCopyAppend(BOOL bMacro)
{
	CMemText Block; GetClipboardData( Block );

	if( ! m_bColumnMode ) {
		if( m_bSelected ) {
			CMemText Blck2; ActionCopyLineSelection( Blck2 );
			Block.AppendText( Blck2 );
			SetClipboardData( Block );
		} else if( m_bCopyCurrentLineIfNothingSelected ) {
			CMemText Blck2; ActionCopyLine( Blck2 );
			Block.AppendText( Blck2 );
			SetClipboardData( Block );
		} else ActionWrongOperation(! bMacro);
	} else ActionWrongOperation(! bMacro);
}

void CCedtView::EventCommandPaste(BOOL bMacro)
{
	CMemText Block; GetClipboardData( Block );

	if( m_bColumnMode ) {
		if( m_bSelected ) {
			if( m_nCaretPosX != m_nAnchorPosX ) { 
				ActionDeleteColumnSelection(); 
				m_bSelected = FALSE; 
			} else { 
				if( m_nCaretPosY > m_nAnchorPosY ) m_nCaretPosY = m_nAnchorPosY; 
				m_bSelected = FALSE; 
			}
		}
		ActionPasteColumnSelection( Block );
	} else {
		if( m_bSelected ) { 
			ActionDeleteLineSelection(); 
			m_bSelected = FALSE; 
		}
		ActionPasteLineSelection( Block );
	}
}

void CCedtView::EventDragCancel(BOOL bMacro)
{
	// just release current selection
	if( m_bSelected ) m_bSelected = FALSE;
}

void CCedtView::EventDragDelete(BOOL bMacro)
{
	m_nCaretPosX = m_nSaveSelBegX; m_nAnchorPosX = m_nSaveSelEndX;
	m_nCaretPosY = m_nSaveSelBegY; m_nAnchorPosY = m_nSaveSelEndY;

	if( m_bColumnMode ) {
		ActionDeleteColumnSelection(); 
		m_bSelected = FALSE; 
	} else {
		ActionDeleteLineSelection(); 
		m_bSelected = FALSE; 
	}
}

void CCedtView::EventDragAdjust(BOOL bMacro)
{
	INT nRelativePos = RelativePosToSavedSelection(m_nDragPosX, m_nDragPosY);

	if( m_bColumnMode ) {
		switch( nRelativePos ) {
		case  1: m_nDragPosX -= m_nSaveSelEndX - m_nSaveSelBegX; break;
		}
	} else {
		switch( nRelativePos ) {
		case  1: m_nDragPosX -= m_nSaveSelEndX - m_nSaveSelBegX;
				 m_nDragPosY -= m_nSaveSelEndY - m_nSaveSelBegY; break;
		case  2: m_nDragPosY -= m_nSaveSelEndY - m_nSaveSelBegY; break;
		}
	}
}

void CCedtView::EventDropCancel(BOOL bMacro)
{
	m_nCaretPosX = m_nDragPosX;
	m_nCaretPosY = m_nDragPosY;

	// just release current selection
	if( m_bSelected ) m_bSelected = FALSE;
}

void CCedtView::EventDropPaste(HGLOBAL hMemory, BOOL bMacro)
{
	m_nCaretPosX = m_nDragPosX;
	m_nCaretPosY = m_nDragPosY;

	CMemText Block; GetGlobalMemoryText( hMemory, Block );

	if( m_bColumnMode ) {
		if( m_bSelected ) m_bSelected = FALSE;
		ActionPasteColumnSelection( Block );
	} else {
		if( m_bSelected ) m_bSelected = FALSE;
		ActionPasteLineSelection( Block );
	}
}

void CCedtView::EventSelectAll(BOOL bMacro)
{
	if( m_bColumnMode ) ActionWrongOperation(! bMacro);
	else m_bSelected = ActionSelectAll();
}

void CCedtView::EventSelectLine(BOOL bMacro)
{
	// select line can be applied to both column editing mode and line editing mode
	m_bSelected = ActionSelectLine();
}

void CCedtView::EventSelectWord(BOOL bMacro)
{
	// select word can be applied to both column editing mode and line editing mode
	m_bSelected = ActionSelectWord();
}

void CCedtView::EventSelectBlock(BOOL bMacro)
{
	if( m_bColumnMode ) ActionWrongOperation(! bMacro);
	else m_bSelected = ActionSelectBlock();
}

void CCedtView::EventUpperCase(BOOL bMacro)
{
	if( m_bColumnMode ) {
		if( m_bSelected ) {
			if( m_nCaretPosX != m_nAnchorPosX ) { 
				CMemText Block; ActionCopyColumnSelection( Block );
				Block.MakeUpperCase();
				ActionChangeColumnSelection( Block );
			} else ActionWrongOperation(! bMacro);
		} else ActionWrongOperation(! bMacro);
	} else {
		if( m_bSelected ) {
			CMemText Block; ActionCopyLineSelection( Block );
			Block.MakeUpperCase();
			ActionChangeLineSelection( Block );
		} else ActionWrongOperation(! bMacro);
	}
}

void CCedtView::EventLowerCase(BOOL bMacro)
{
	if( m_bColumnMode ) {
		if( m_bSelected ) {
			if( m_nCaretPosX != m_nAnchorPosX ) { 
				CMemText Block; ActionCopyColumnSelection( Block );
				Block.MakeLowerCase();
				ActionChangeColumnSelection( Block );
			} else ActionWrongOperation(! bMacro);
		} else ActionWrongOperation(! bMacro);
	} else {
		if( m_bSelected ) {
			CMemText Block; ActionCopyLineSelection( Block );
			Block.MakeLowerCase();
			ActionChangeLineSelection( Block );
		} else ActionWrongOperation(! bMacro);
	}
}

void CCedtView::EventCapitalize(BOOL bMacro)
{
	if( m_bColumnMode ) {
		if( m_bSelected ) {
			if( m_nCaretPosX != m_nAnchorPosX ) { 
				CMemText Block; ActionCopyColumnSelection( Block );
				Block.MakeCapitalize();
				ActionChangeColumnSelection( Block );
			} else ActionWrongOperation(! bMacro);
		} else ActionWrongOperation(! bMacro);
	} else {
		if( m_bSelected ) {
			CMemText Block; ActionCopyLineSelection( Block );
			Block.MakeCapitalize();
			ActionChangeLineSelection( Block );
		} else ActionWrongOperation(! bMacro);
	}
}

void CCedtView::EventInvertCase(BOOL bMacro)
{
	if( m_bColumnMode ) {
		if( m_bSelected ) {
			if( m_nCaretPosX != m_nAnchorPosX ) { 
				CMemText Block; ActionCopyColumnSelection( Block );
				Block.MakeInvertCase();
				ActionChangeColumnSelection( Block );
			} else ActionWrongOperation(! bMacro);
		} else ActionWrongOperation(! bMacro);
	} else {
		if( m_bSelected ) {
			CMemText Block; ActionCopyLineSelection( Block );
			Block.MakeInvertCase();
			ActionChangeLineSelection( Block );
		} else ActionWrongOperation(! bMacro);
	}
}

void CCedtView::EventConvertTabsToSpaces(BOOL bMacro)
{
	ActionConvertTabsToSpaces();
}

void CCedtView::EventConvertSpacesToTabs(BOOL bMacro)
{
	ActionConvertSpacesToTabs();
}

void CCedtView::EventLeadingSpacesToTabs(BOOL bMacro)
{
	ActionLeadingSpacesToTabs();
}

void CCedtView::EventRemoveTrailingSpaces(BOOL bMacro)
{
	ActionRemoveTrailingSpaces();
}

void CCedtView::EventUndoLastAction(BOOL bMacro)
{
	if( m_bSelected ) m_bSelected = FALSE;
	ActionUndoLastAction();
}

void CCedtView::EventRedoLastUndo(BOOL bMacro)
{
	if( m_bSelected ) m_bSelected = FALSE;
	ActionRedoLastUndo();
}

void CCedtView::EventEvaluateLine(BOOL bMacro)
{
	if( m_bSelected ) m_bSelected = FALSE;
	ActionEvaluateLine();
}

void CCedtView::EventReplayMacro(INT nMacro, INT nTimes, BOOL bMacro)
{
//	if( m_bSelected ) m_bSelected = FALSE; - selection should be taken into account
	ActionReplayMacro(nMacro, nTimes);
}

BOOL CCedtView::EventFindString(LPCTSTR lpszFindString, UINT nOptions, BOOL bMacro)
{
	if( ! strlen(lpszFindString) ) return FALSE;

	if( SEARCH_REG_EXP(nOptions) ) { // compile regular expression
		CString szExpression = lpszFindString; 			szExpression.Replace( "\\\\", "\x1B" );
		szExpression.Replace( "\\s" , "[ \t\r\n]" );	szExpression.Replace( "\\S" , "[^ \t\r\n]" );
		szExpression.Replace( "\\w" , "[A-Za-z0-9]" );	szExpression.Replace( "\\W" , "[^A-Za-z0-9]" );
		szExpression.Replace( "\\a" , "[A-Za-z]" );		szExpression.Replace( "\\A" , "[^A-Za-z]" );
		szExpression.Replace( "\\d" , "[0-9]" );		szExpression.Replace( "\\D" , "[^0-9]" );
		szExpression.Replace( "\\h" , "[A-Fa-f0-9]" );	szExpression.Replace( "\\H" , "[^A-Fa-f0-9]" );
		szExpression.Replace( "\\t" , "\t" );			szExpression.Replace( "\x1B", "\\\\" );

		if( ! SEARCH_MATCH_CASE(nOptions) ) szExpression.MakeLower();
		if( ! m_clsRegExp.RegComp( szExpression ) ) return FALSE;
	}

	m_bReplaceSearch = FALSE;
	m_szFindString = lpszFindString;
	m_nSearchOptions = nOptions;

	return TRUE;
}

BOOL CCedtView::EventReplaceString(LPCTSTR lpszFindString, LPCTSTR lpszReplaceString, UINT nOptions, BOOL bMacro)
{
	if( ! strlen(lpszFindString) ) return FALSE;

	if( SEARCH_REG_EXP(nOptions) ) { // compile regular expression
		CString szExpression = lpszFindString; 			szExpression.Replace( "\\\\", "\x1B" );
		szExpression.Replace( "\\s" , "[ \t\r\n]" );	szExpression.Replace( "\\S" , "[^ \t\r\n]" );
		szExpression.Replace( "\\w" , "[A-Za-z0-9]" );	szExpression.Replace( "\\W" , "[^A-Za-z0-9]" );
		szExpression.Replace( "\\a" , "[A-Za-z]" );		szExpression.Replace( "\\A" , "[^A-Za-z]" );
		szExpression.Replace( "\\d" , "[0-9]" );		szExpression.Replace( "\\D" , "[^0-9]" );
		szExpression.Replace( "\\h" , "[A-Fa-f0-9]" );	szExpression.Replace( "\\H" , "[^A-Fa-f0-9]" );
		szExpression.Replace( "\\t" , "\t" );			szExpression.Replace( "\x1B", "\\\\" );

		if( ! SEARCH_MATCH_CASE(nOptions) ) szExpression.MakeLower();
		if( ! m_clsRegExp.RegComp( szExpression ) ) return FALSE;
	}

	m_bReplaceSearch = TRUE;
	m_szFindString = lpszFindString;
	m_szReplaceString = lpszReplaceString;
	m_nSearchOptions = nOptions;

	return TRUE;
}

BOOL CCedtView::EventFindCurrentString(BOOL bMacro)
{
	CString szFindString = GetCurrentWord();
	if( m_bSelected && ! GetSelectedLineCount() ) szFindString = GetSelectedString();

	// if current string has zero length then continue previous search
	if( ! szFindString.GetLength() ) return FALSE;

	m_bReplaceSearch = FALSE;
	m_szFindString = szFindString;
	m_nSearchOptions = COMPOSE_SEARCH_OPTION(FALSE, FALSE, FALSE);

	return TRUE;
}

BOOL CCedtView::EventFindSelectedString(BOOL bMacro)
{
	// check if there is selected word if not then continue previous search
	if( ! m_bSelected || GetSelectedLineCount() ) return FALSE;

	CString szFoundString = m_szFindString;
	if( SEARCH_REG_EXP(m_nSearchOptions) ) m_clsRegExp.GetFoundString(szFoundString);

	CString szSelectedString = GetSelectedString();
	if( ! SEARCH_MATCH_CASE(m_nSearchOptions) ) { szFoundString.MakeLower(); szSelectedString.MakeLower(); }

	// if selected text is same as previous search result then continue previous search
	if( ! szFoundString.Compare(szSelectedString) ) return FALSE;

	m_bReplaceSearch = FALSE;
	m_szFindString = szSelectedString;
	m_nSearchOptions = COMPOSE_SEARCH_OPTION(FALSE, FALSE, FALSE);

	return TRUE;
}

BOOL CCedtView::EventSearchNextOccurrence(BOOL bMacro)
{
	BOOL bFound, bSearchWrap = FALSE;

	bFound = ActionForwardFindString(m_szFindString, m_nSearchOptions, m_clsRegExp, & bSearchWrap);
	if( bFound ) m_bSelected = ((m_nAnchorPosY != m_nCaretPosY) || (m_nAnchorPosX != m_nCaretPosX));

	if( bFound && bSearchWrap ) {
		CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();
		pFrame->SetSplashMessage(IDS_MSG_SEARCH_WRAP, 1000);
	}

	return bFound;
}

BOOL CCedtView::EventSearchPrevOccurrence(BOOL bMacro)
{
	BOOL bFound, bSearchWrap = FALSE;

	bFound = ActionReverseFindString(m_szFindString, m_nSearchOptions, m_clsRegExp, & bSearchWrap);
	if( bFound ) m_bSelected = ((m_nAnchorPosY != m_nCaretPosY) || (m_nAnchorPosX != m_nCaretPosX));

	if( bFound && bSearchWrap ) {
		CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();
		pFrame->SetSplashMessage(IDS_MSG_SEARCH_WRAP, 1000);
	}

	return bFound;
}

INT CCedtView::EventReplaceThisOccurrence(BOOL bMacro)
{
	INT nReplaced = 0;

	if( m_bSelected && ! GetSelectedLineCount() ) {
		nReplaced = ActionReplaceThisOccurrence(m_szReplaceString, m_nSearchOptions, m_clsRegExp);
		m_bSelected = FALSE;
	} 

	return nReplaced;
}

INT CCedtView::EventReplaceAllInSelection(BOOL bMacro)
{
	INT nReplaced = 0;

	if( ! m_bColumnMode && m_bSelected && GetSelectedLineCount() ) {
		nReplaced = ActionReplaceAllInSelection(m_szFindString, m_szReplaceString, m_nSearchOptions, m_clsRegExp);
		m_bSelected = ((m_nAnchorPosY != m_nCaretPosY) || (m_nAnchorPosX != m_nCaretPosX));
	}

	return nReplaced;
}

INT CCedtView::EventReplaceAllInFile(BOOL bMacro)
{
	INT nReplaced = ActionReplaceAllInFile(m_szFindString, m_szReplaceString, m_nSearchOptions, m_clsRegExp);
	if( m_bSelected ) m_bSelected = FALSE;

	return nReplaced;
}

BOOL CCedtView::EventGoToLine(INT nLineNumber, BOOL bMacro)
{
	BOOL bRedraw = ActionGoToLine(nLineNumber-1);
	if( m_bSelected ) { m_bSelected = FALSE; bRedraw = TRUE; }
	return bRedraw;
}

BOOL CCedtView::EventToggleBookmark(BOOL bMacro)
{
	BOOL bRedraw = ActionToggleBookmark();
	if( m_bSelected ) { m_bSelected = FALSE; bRedraw = TRUE; }
	return bRedraw;
}

BOOL CCedtView::EventNextBookmark(BOOL bMacro)
{
	BOOL bRedraw = ActionNextBookmark();
	if( m_bSelected ) { m_bSelected = FALSE; bRedraw = TRUE; }
	return bRedraw;
}

BOOL CCedtView::EventPrevBookmark(BOOL bMacro)
{
	BOOL bRedraw = ActionPrevBookmark();
	if( m_bSelected ) { m_bSelected = FALSE; bRedraw = TRUE; }
	return bRedraw;
}

BOOL CCedtView::EventPrevEditingPosition(BOOL bMacro)
{
	BOOL bRedraw = ActionPrevEditingPosition();
	if( m_bSelected ) { m_bSelected = FALSE; bRedraw = TRUE; }
	return bRedraw;
}

BOOL CCedtView::EventPairsBeginPosition(UINT nFlags, BOOL bMacro)
{
	BOOL bShift = nFlags & KEYSTATE_SHIFT;
	BOOL bRedraw = FALSE;

	if( ! m_bSelected && bShift ) {
		m_nAnchorPosX = m_nCaretPosX; 
		m_nAnchorPosY = m_nCaretPosY;
	}

	INT nBegX, nBegY, nEndX, nEndY;
	if( m_bSelected ) GetSelectedPosition( nBegX, nBegY, nEndX, nEndY );

	if( m_bSelected && ! bShift ) { SetCaretPosY( nBegY ); SetCaretPosX( nBegX ); }
	bRedraw = ActionPairsBeginPosition();

	if( bShift ) {
		m_bSelected = ( (m_nAnchorPosY != m_nCaretPosY) || (m_nAnchorPosX != m_nCaretPosX) );
		bRedraw = TRUE;
	} else if( m_bSelected ) {
		m_bSelected = FALSE;
		bRedraw = TRUE;
	}

	return bRedraw;
}

BOOL CCedtView::EventPairsEndPosition(UINT nFlags, BOOL bMacro)
{
	BOOL bShift = nFlags & KEYSTATE_SHIFT;
	BOOL bRedraw = FALSE;

	if( ! m_bSelected && bShift ) {
		m_nAnchorPosX = m_nCaretPosX; 
		m_nAnchorPosY = m_nCaretPosY;
	}

	INT nBegX, nBegY, nEndX, nEndY;
	if( m_bSelected ) GetSelectedPosition( nBegX, nBegY, nEndX, nEndY );

	if( m_bSelected && ! bShift ) { SetCaretPosY( nEndY ); SetCaretPosX( nEndX ); }
	bRedraw = ActionPairsEndPosition();

	if( bShift ) {
		m_bSelected = ( (m_nAnchorPosY != m_nCaretPosY) || (m_nAnchorPosX != m_nCaretPosX) );
		bRedraw = TRUE;
	} else if( m_bSelected ) {
		m_bSelected = FALSE;
		bRedraw = TRUE;
	}

	return bRedraw;
}


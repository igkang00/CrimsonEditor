#include "stdafx.h"
#include "cedtHeader.h"


BOOL CCedtView::ActionLeftKey(UINT nFlags) 
{
	if( nFlags & KEYSTATE_CONTROL ) {
		CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );

		if( m_nCaretPosX > GetLastPosX( rLine ) ) {
			MoveCaretLineEnd();
			MoveCaretWordLeft();
		} else if( m_nCaretPosX > GetFirstPosX( rLine ) ) {
			MoveCaretWordLeft();
		} else if( m_nCaretPosY > 0 ) {
			MoveCaretUp(); MoveCaretLineEnd();
			MoveCaretWordLeft();
		}

		return ! IsCaretVisible();

	} else if( nFlags & KEYSTATE_MENU ) {
		MoveCaretLineBegin();
		return ! IsCaretVisible();

	} else { // no special key pressed 
		CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );

		if( ! m_bColumnMode ) {
			if( m_nCaretPosX > GetLastPosX( rLine ) ) {
				MoveCaretLineEnd();
				MoveCaretLeft();
			} else if( m_nCaretPosX > GetFirstPosX( rLine ) ) {
				MoveCaretLeft();
			} else if( m_nCaretPosY > 0 ) {
				MoveCaretUp(); MoveCaretLineEnd();
				if( rLine.m_siSplitIndex ) MoveCaretLeft();
			}
		} else { // in column mode
			if( m_nCaretPosX > GetFirstPosX( rLine ) ) MoveCaretLeft();
		}

		return ! IsCaretVisible();
	}
}

BOOL CCedtView::ActionRightKey(UINT nFlags) 
{
	if( nFlags & KEYSTATE_CONTROL ) {
		CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );

		if( m_nCaretPosX < GetLastPosX( rLine ) ) {
			MoveCaretWordRight();
			if( m_nCaretPosX == GetLastPosX( rLine ) && m_nCaretPosY < GetLastPosY() ) { MoveCaretDown(); MoveCaretLineBegin(); }
		} else if( m_nCaretPosY < GetLastPosY() ) {
			MoveCaretDown(); MoveCaretLineBegin();
			if( rLine.m_bLineBreak ) MoveCaretWordRight();
		}

		return ! IsCaretVisible();

	} else if( nFlags & KEYSTATE_MENU ) {
		MoveCaretLineEnd();
		return ! IsCaretVisible();

	} else { // no special key pressed
		CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );

		if( ! m_bColumnMode ) {
			if( m_nCaretPosX < GetLastPosX( rLine ) ) {
				MoveCaretRight();
				if( m_nCaretPosX == GetLastPosX( rLine ) && m_nCaretPosY < GetLastPosY() && rLine.m_bLineBreak ) { MoveCaretDown(); MoveCaretLineBegin(); }
			} else if( m_nCaretPosY < GetLastPosY() ) {
				MoveCaretDown(); MoveCaretLineBegin();
				if( rLine.m_bLineBreak ) MoveCaretRight();
			}
		} else { // in column mode
			MoveCaretRight();
		}

		return ! IsCaretVisible();
	}
}

BOOL CCedtView::ActionUpKey(UINT nFlags) 
{
	if( nFlags & KEYSTATE_CONTROL && nFlags & KEYSTATE_SHIFT ) {
		if( m_nScrollPosY > 0 ) { ScrollScreenUpLimited(); return TRUE; }
		else return FALSE;

	} else if( nFlags & KEYSTATE_CONTROL ) {
		if( m_nScrollPosY > 0 ) { ScrollScreenUp(); return TRUE; }
		else return FALSE;

	} else if( nFlags & KEYSTATE_MENU ) {
		MoveCaretScreenTop();
		return ! IsCaretVisible();

	} else /* no special key pressed */ {
		if( m_nCaretPosY  > 0 ) MoveCaretUp(); 
		return ! IsCaretVisible();
	}
}

BOOL CCedtView::ActionDownKey(UINT nFlags) 
{
	if( nFlags & KEYSTATE_CONTROL && nFlags & KEYSTATE_SHIFT ) {
		if( m_nScrollPosY < GetLastPosY() ) { ScrollScreenDownLimited(); return TRUE; }
		else return FALSE;

	} else if( nFlags & KEYSTATE_CONTROL ) {
		if( m_nScrollPosY < GetLastPosY() ) { ScrollScreenDown(); return TRUE; }
		else return FALSE;

	} else if( nFlags & KEYSTATE_MENU ) {
		MoveCaretScreenBottom(); 
		return ! IsCaretVisible();

	} else /* no special key pressed */ {
		if( m_nCaretPosY  < GetLastPosY() ) MoveCaretDown(); 
		return ! IsCaretVisible();
	}
}

BOOL CCedtView::ActionHomeKey(UINT nFlags)
{
	if( nFlags & KEYSTATE_CONTROL ) {
		MoveCaretDocumentBegin();
		return ! IsCaretVisible();

	} else if( nFlags & KEYSTATE_MENU ) {
		MoveCaretParaBegin();
		return ! IsCaretVisible();

	} else /* no special key pressed */ {
		CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
		if( ! m_bHomeKeyGoesToFirstPosition && m_nCaretPosX != GetFirstNonBlankPosX(rLine) ) MoveCaretLineBeginNonBlank();
		else MoveCaretLineBegin();
		return ! IsCaretVisible();
	}
}

BOOL CCedtView::ActionEndKey(UINT nFlags)
{
	if( nFlags & KEYSTATE_CONTROL ) {
		MoveCaretDocumentEnd();
		return ! IsCaretVisible();

	} else if( nFlags & KEYSTATE_MENU ) {
		MoveCaretParaEnd();
		return ! IsCaretVisible(); 

	} else /* no special key pressed */ {
		MoveCaretLineEnd();
		return ! IsCaretVisible();
	}
}

BOOL CCedtView::ActionPriorKey(UINT nFlags)
{
	if( nFlags & KEYSTATE_CONTROL ) {
		/* do nothing here */
		return FALSE;

	} else if( nFlags & KEYSTATE_MENU ) {
		if( m_nCaretPosY > 0 ) { MoveCaretHalfPageUp(); return TRUE; }
		else return FALSE;

	} else /* no special key pressed */ {
		if( m_nCaretPosY > 0 ) { MoveCaretPageUp(); return TRUE; }
		else return FALSE;
	}
}

BOOL CCedtView::ActionNextKey(UINT nFlags)
{
	if( nFlags & KEYSTATE_CONTROL ) {
		/* do nothing here */
		return FALSE;

	} else if( nFlags & KEYSTATE_MENU ) {
		if( m_nCaretPosY < GetLastPosY() ) { MoveCaretHalfPageDown(); return TRUE; }
		else return FALSE;

	} else /* no special key pressed */ {
		if( m_nCaretPosY < GetLastPosY() ) { MoveCaretPageDown(); return TRUE; }
		else return FALSE;
	}
}


void CCedtView::MoveCaretLeft()
{
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );

	if( m_nCaretPosX > GetLastPosX( rLine ) ) {
		INT nAveCharWidth = GetAveCharWidth();
		INT nIdxX = m_nCaretPosX / nAveCharWidth;

		if( m_bColumnMode ) SetCaretPosX( (nIdxX-1) * nAveCharWidth );
		else SetCaretPosX( GetLastPosX( rLine ) );

	} else if( m_nCaretPosX > GetFirstPosX( rLine ) ) {
		INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX );
		FORMATEDWORD & rWord = GetWordFromIdxX( rLine, nIdxX-1 );

		if( IS_DBCHAR(rWord) ) SetCaretPosX( GetPosXFromIdxX( rLine, nIdxX-2 ) );
		else SetCaretPosX( GetPosXFromIdxX( rLine, nIdxX-1 ) );

	} else SetCaretPosX( GetFirstPosX( rLine ) );
}

void CCedtView::MoveCaretRight()
{
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );

	if( m_nCaretPosX >= GetLastPosX( rLine ) ) {
		INT nAveCharWidth = GetAveCharWidth();
		INT nIdxX = m_nCaretPosX / nAveCharWidth;

		if( m_bColumnMode ) SetCaretPosX( (nIdxX+1) * nAveCharWidth );
		else SetCaretPosX( GetLastPosX( rLine ) );

	} else if( m_nCaretPosX >= GetFirstPosX( rLine ) ) {
		INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX );
		FORMATEDWORD & rWord = GetWordFromIdxX( rLine, nIdxX );

		if( IS_DBCHAR(rWord) ) SetCaretPosX( GetPosXFromIdxX( rLine, nIdxX+2 ) );
		else SetCaretPosX( GetPosXFromIdxX( rLine, nIdxX+1 ) );

	} else SetCaretPosX( GetFirstPosX( rLine ) );
}

void CCedtView::MoveCaretWordLeft()
{
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX );

	if( nIdxX > 0 ) nIdxX = GetPrevWordIdxX( rLine, nIdxX-1 );
	SetCaretPosX( GetPosXFromIdxX( rLine, nIdxX ) );
}

void CCedtView::MoveCaretWordRight()
{
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX );

	nIdxX = GetNextWordIdxX( rLine, nIdxX );
	SetCaretPosX( GetPosXFromIdxX( rLine, nIdxX ) );
}

void CCedtView::MoveCaretLineBegin()
{
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetFirstPosX( rLine ) );
}

void CCedtView::MoveCaretLineEnd()
{
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetLastPosX( rLine ) );
}

void CCedtView::MoveCaretLineBeginNonBlank()
{
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetFirstNonBlankPosX( rLine ) );
}

void CCedtView::MoveCaretParaBegin()
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CAnalyzedString & rLine = GetLineFromIdxY( nIdxY );
	INT nIdxX = GetFirstIdxX( rLine );

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX ) );
}

void CCedtView::MoveCaretParaEnd()
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CAnalyzedString & rLine = GetLineFromIdxY( nIdxY );
	INT nIdxX = GetLastIdxX( rLine );

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX ) );
}

void CCedtView::MoveCaretUp()
{
	SetCaretPosY( m_nCaretPosY - GetLineHeight() );
	SetCaretPosX( m_nCaretPosX, FALSE );
}

void CCedtView::MoveCaretDown()
{
	SetCaretPosY( m_nCaretPosY + GetLineHeight() );
	SetCaretPosX( m_nCaretPosX, FALSE );
}

void CCedtView::MoveCaretScreenTop()
{
	SetCaretPosY( m_nScrollPosY );
	SetCaretPosX( m_nCaretPosX, FALSE );
}

void CCedtView::MoveCaretScreenBottom()
{
	SetCaretPosY( m_nScrollPosY + (GetLinesPerPage() - 1) * GetLineHeight() );
	SetCaretPosX( m_nCaretPosX, FALSE );
}

void CCedtView::MoveCaretPageUp()
{
	SetScrollPosY( m_nScrollPosY - GetLinesPerPage() * GetLineHeight() );
	SetCaretPosY( m_nCaretPosY - GetLinesPerPage() * GetLineHeight() );
	SetCaretPosX( m_nCaretPosX, FALSE );
}

void CCedtView::MoveCaretPageDown()
{
	SetScrollPosY( m_nScrollPosY + GetLinesPerPage() * GetLineHeight() );

	SetCaretPosY( m_nCaretPosY + GetLinesPerPage() * GetLineHeight() );
	SetCaretPosX( m_nCaretPosX, FALSE );
}

void CCedtView::MoveCaretHalfPageUp()
{
	SetScrollPosY( m_nScrollPosY - (GetLinesPerPage() / 2) * GetLineHeight() );

	SetCaretPosY( m_nCaretPosY - (GetLinesPerPage() / 2) * GetLineHeight() );
	SetCaretPosX( m_nCaretPosX, FALSE );
}

void CCedtView::MoveCaretHalfPageDown()
{
	SetScrollPosY( m_nScrollPosY + (GetLinesPerPage() / 2) * GetLineHeight() );

	SetCaretPosY( m_nCaretPosY + (GetLinesPerPage() / 2) * GetLineHeight() );
	SetCaretPosX( m_nCaretPosX, FALSE );
}

void CCedtView::MoveCaretDocumentBegin()
{
	SetCaretPosY( 0 );

	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetFirstPosX( rLine ) );
}

void CCedtView::MoveCaretDocumentEnd()
{
	SetCaretPosY( GetLastPosY() );

	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetLastPosX( rLine ) );
}

void CCedtView::MoveCaretScreenBegin()
{
	SetCaretPosY( m_nScrollPosY );

	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetFirstPosX( rLine ) );
}

void CCedtView::MoveCaretScreenEnd()
{
	SetCaretPosY( m_nScrollPosY + (GetLinesPerPage() - 1) * GetLineHeight() );

	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetLastPosX( rLine ) );
}

void CCedtView::ScrollScreenUp()
{
	SetScrollPosY( m_nScrollPosY - GetLineHeight() );

	if( m_nCaretPosY > m_nScrollPosY + GetLinesPerPage() * GetLineHeight() - GetLineHeight() ) {
		SetCaretPosY( m_nScrollPosY + GetLinesPerPage() * GetLineHeight() - GetLineHeight() );
		SetCaretPosX( m_nCaretPosX, FALSE );
	}
}

void CCedtView::ScrollScreenDown()
{
	SetScrollPosY( m_nScrollPosY + GetLineHeight() );

	if( m_nCaretPosY < m_nScrollPosY ) {
		SetCaretPosY( m_nScrollPosY );
		SetCaretPosX( m_nCaretPosX, FALSE );
	}
}


void CCedtView::ScrollScreenUpLimited()
{
	INT nSavedScrollPosY = m_nScrollPosY;
	SetScrollPosY( m_nScrollPosY - GetLineHeight() );

	if( m_nCaretPosY > m_nScrollPosY + GetLinesPerPage() * GetLineHeight() - GetLineHeight() ) {
		SetScrollPosY( nSavedScrollPosY );
	}
}

void CCedtView::ScrollScreenDownLimited()
{
	INT nSavedScrollPosY = m_nScrollPosY;
	SetScrollPosY( m_nScrollPosY + GetLineHeight() );

	if( m_nCaretPosY < m_nScrollPosY ) {
		SetScrollPosY( nSavedScrollPosY );
	}
}




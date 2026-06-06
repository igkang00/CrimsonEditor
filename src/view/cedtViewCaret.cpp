#include "stdafx.h"
#include "cedtHeader.h"


INT CCedtView::GetSelectedLineCount()
{
	if( ! m_bSelected ) return 0;
	INT nBegX, nBegY, nEndX, nEndY;
	GetSelectedIndex(nBegX, nBegY, nEndX, nEndY);
	return (nEndY - nBegY);
}

void CCedtView::SaveCaretAndAnchorPos()
{
	m_nCaretIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	m_nCaretIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX );

	m_nAnchorIdxY = GetIdxYFromPosY( m_nAnchorPosY );
	CFormatedString & rLne2 = GetLineFromPosY( m_nAnchorPosY );
	m_nAnchorIdxX = GetIdxXFromPosX( rLne2, m_nAnchorPosX );

	m_nScrollIdxY = GetIdxYFromPosY( m_nScrollPosY );
	CFormatedString & rLne3 = GetLineFromPosY( m_nScrollPosY );
	m_nScrollIdxX = 0;
}

void CCedtView::RestoreCaretAndAnchorPos()
{
	SetCaretPosY( GetPosYFromIdxY( m_nCaretIdxX, m_nCaretIdxY ) );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLine, m_nCaretIdxX ) );

	m_nAnchorPosY = GetPosYFromIdxY( m_nAnchorIdxX, m_nAnchorIdxY );
	CFormatedString & rLne2 = GetLineFromPosY( m_nAnchorPosY );
	m_nAnchorPosX = GetPosXFromIdxX( rLne2, m_nAnchorIdxX );

	SetScrollPosY( GetPosYFromIdxY( 0, m_nScrollIdxY ) );
	CFormatedString & rLne3 = GetLineFromPosY( m_nScrollPosY );
	SetScrollPosX( GetPosXFromIdxX( rLne3, m_nScrollIdxX ) );
}


void CCedtView::GetSelectedPosition(INT & nBegX, INT & nBegY, INT & nEndX, INT & nEndY)
{
	if( m_bColumnMode && m_bSelected ) { // column select mode
		if( m_nCaretPosY < m_nAnchorPosY ) {
			nBegY = m_nCaretPosY;	nEndY = m_nAnchorPosY;
		} else {
			nBegY = m_nAnchorPosY;	nEndY = m_nCaretPosY;	
		}

		if( m_nCaretPosX < m_nAnchorPosX ) {
			nBegX = m_nCaretPosX;	nEndX = m_nAnchorPosX;
		} else {
			nBegX = m_nAnchorPosX;	nEndX = m_nCaretPosX;	
		}

	} else if( m_bSelected ) { // line select mode
		if( m_nCaretPosY > m_nAnchorPosY ) {
			nBegX = m_nAnchorPosX;	nBegY = m_nAnchorPosY;
			nEndX = m_nCaretPosX;	nEndY = m_nCaretPosY;

			if( m_bLineSelect ) { // line select mouse move
				if( m_nCaretPosY < GetLastPosY() ) {
					nEndY = m_nCaretPosY + GetLineHeight();
					CFormatedString & rLine = GetLineFromPosY( nEndY );
					nEndX = GetFirstPosX( rLine );
				} else {
					CFormatedString & rLine = GetLineFromPosY( nEndY );
					nEndX = GetLastPosX( rLine ); 
				}
			}
		} else if( m_nCaretPosY < m_nAnchorPosY ) {
			nBegX = m_nCaretPosX;	nBegY = m_nCaretPosY;
			nEndX = m_nAnchorPosX;	nEndY = m_nAnchorPosY;
		} else if( m_nCaretPosX > m_nAnchorPosX ) {
			nBegX = m_nAnchorPosX;	nBegY = m_nAnchorPosY;
			nEndX = m_nCaretPosX;	nEndY = m_nCaretPosY;
		} else {
			nBegX = m_nCaretPosX;	nBegY = m_nCaretPosY;
			nEndX = m_nAnchorPosX;	nEndY = m_nAnchorPosY;
		}

	} else if( m_bComposition ) { // multi-byte character composition
		nBegX = m_nAnchorPosX;	nBegY = m_nAnchorPosY;
		nEndX = m_nCaretPosX;	nEndY = m_nCaretPosY;

	} else {
		nBegX = nBegY = nEndX = nEndY = 0;
	}
}

void CCedtView::GetSelectedIndex(INT & nBegX, INT & nBegY, INT & nEndX, INT & nEndY)
{
	GetSelectedPosition(nBegX, nBegY, nEndX, nEndY);

	CFormatedString & rLine = GetLineFromPosY( nBegY );
	nBegY = GetIdxYFromPosY( nBegY );
	nBegX = GetIdxXFromPosX( rLine, nBegX );

	CFormatedString & rLne2 = GetLineFromPosY( nEndY );
	nEndY = GetIdxYFromPosY( nEndY );
	nEndX = GetIdxXFromPosX( rLne2, nEndX );
}


void CCedtView::SetCaretPosY(INT nPosY)
{
	INT nLineHeight = GetLineHeight();
	INT nLastPosY = GetLastPosY();

	if( nPosY > nLastPosY ) m_nCaretPosY = nLastPosY;
	else if( nPosY > 0 ) m_nCaretPosY = (nPosY / nLineHeight) * nLineHeight;
	else m_nCaretPosY = 0;
}

void CCedtView::SetCaretPosX(INT nPosX, BOOL bFix)
{
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nLastPosX, nFrstPosX;

	if( ! bFix ) nPosX = m_nCaretPosXFixed;

	if( nPosX > ( nLastPosX = GetLastPosX(rLine) ) ) {
		if( m_bColumnMode ) { // is using fixed pitch font
			INT nCharWidth = GetAveCharWidth();
			m_nCaretPosX = (nPosX / nCharWidth) * nCharWidth;
		} else m_nCaretPosX = nLastPosX;
	} else if( nPosX > ( nFrstPosX = GetFirstPosX(rLine) ) ) {
		INT nIdxX = GetIdxXFromPosX( rLine, nPosX );
		m_nCaretPosX = GetPosXFromIdxX( rLine, nIdxX );
	} else { // nPosX <= nFrstPosX
		m_nCaretPosX = nFrstPosX;
	}

	if( bFix ) m_nCaretPosXFixed = m_nCaretPosX;
}

void CCedtView::SetCaretPosFromMouse(CPoint point)
{
	TRACE0("SetCaretPosFromMouse\n");
	SetCaretPosY( point.y + m_nScrollPosY );

	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nPosX = point.x + m_nScrollPosX - GetLeftMargin();
	INT nLastPosX, nFrstPosX;

	if( nPosX > ( nLastPosX = GetLastPosX(rLine) ) ) {
		if( m_bColumnMode ) {
			INT nCharWidth = GetAveCharWidth();
			INT nPos1 = (nPosX / nCharWidth) * nCharWidth;
			INT nPos2 = (nPosX / nCharWidth + 1) * nCharWidth;
			SetCaretPosX( (nPosX-nPos1 < nPos2-nPosX) ? nPos1 : nPos2 );
		} else SetCaretPosX( nLastPosX );
	} else if( nPosX > ( nFrstPosX = GetFirstPosX(rLine) ) ) {
		INT nIdxX = GetIdxXFromPosX( rLine, nPosX );
		FORMATEDWORD & rWord = GetWordFromIdxX( rLine, nIdxX );
		INT nIncX = IS_DBCHAR(rWord) ? 2 : 1;

		INT nPos1 = GetPosXFromIdxX( rLine, nIdxX );
		INT nPos2 = GetPosXFromIdxX( rLine, nIdxX + nIncX );
		SetCaretPosX( (nPosX-nPos1 < nPos2-nPosX) ? nPos1 : nPos2 );
	} else {
		SetCaretPosX( nFrstPosX );
	}
}

void CCedtView::SetDragPosY(INT nPosY)
{
	INT nLineHeight = GetLineHeight();
	INT nLastPosY = GetLastPosY();

	if( nPosY > nLastPosY ) m_nDragPosY = nLastPosY;
	else if( nPosY > 0 ) m_nDragPosY = (nPosY / nLineHeight) * nLineHeight;
	else m_nDragPosY = 0;
}

void CCedtView::SetDragPosX(INT nPosX)
{
	CFormatedString & rLine = GetLineFromPosY( m_nDragPosY );
	INT nLastPosX, nFrstPosX;

	if( nPosX > ( nLastPosX = GetLastPosX(rLine) ) ) {
		if( m_bColumnMode ) { // is using fixed pitch font
			INT nCharWidth = GetAveCharWidth();
			m_nDragPosX = (nPosX / nCharWidth) * nCharWidth;
		} else m_nDragPosX = nLastPosX;
	} else if( nPosX > ( nFrstPosX = GetFirstPosX(rLine) ) ) {
		INT nIdxX = GetIdxXFromPosX( rLine, nPosX );
		m_nDragPosX = GetPosXFromIdxX( rLine, nIdxX );
	} else { // nPosX <= nFrstPosX
		m_nDragPosX = nFrstPosX;
	}
}

void CCedtView::SetDragPosFromMouse(CPoint point)
{
	SetDragPosY( point.y + m_nScrollPosY );

	CFormatedString & rLine = GetLineFromPosY( m_nDragPosY );
	INT nPosX = point.x + m_nScrollPosX - GetLeftMargin();
	INT nLastPosX, nFrstPosX;

	if( nPosX > ( nLastPosX = GetLastPosX(rLine) ) ) {
		if( m_bColumnMode ) {
			INT nCharWidth = GetAveCharWidth();
			INT nPos1 = (nPosX / nCharWidth) * nCharWidth;
			INT nPos2 = (nPosX / nCharWidth + 1) * nCharWidth;
			SetDragPosX( (nPosX-nPos1 < nPos2-nPosX) ? nPos1 : nPos2 );
		} else SetDragPosX( nLastPosX );
	} else if( nPosX > ( nFrstPosX = GetFirstPosX(rLine) ) ) {
		INT nIdxX = GetIdxXFromPosX( rLine, nPosX );
		FORMATEDWORD & rWord = GetWordFromIdxX( rLine, nIdxX );
		INT nIncX = IS_DBCHAR(rWord) ? 2 : 1;

		INT nPos1 = GetPosXFromIdxX( rLine, nIdxX );
		INT nPos2 = GetPosXFromIdxX( rLine, nIdxX + nIncX );
		SetDragPosX( (nPosX-nPos1 < nPos2-nPosX) ? nPos1 : nPos2 );
	} else {
		SetDragPosX( nFrstPosX );
	}
}

BOOL CCedtView::IsCaretVisible()
{
	RECT rect; GetClientRect( & rect );
	INT nLeftMargin = GetLeftMargin();	INT nMaxCharWidth = GetMaxCharWidth();
	INT nLineHeight = GetLineHeight();	INT nLinesPerPage = GetLinesPerPage();

	INT nPosY = m_nCaretPosY - m_nScrollPosY;
	if( nPosY < 0 || nPosY > (nLinesPerPage - 1) * nLineHeight ) return FALSE;

	INT nPosX = m_nCaretPosX - m_nScrollPosX + nLeftMargin;
	if( nPosX < nLeftMargin || nPosX > rect.right - nMaxCharWidth ) return FALSE;

	return TRUE;
}

BOOL CCedtView::IsPointInSelection(CPoint point)
{
	INT nLeftMargin = GetLeftMargin();

	INT nPosX = point.x + m_nScrollPosX - nLeftMargin;
	INT nPosY = point.y + m_nScrollPosY;

	return IsPositionInSelection(nPosX, nPosY);
}

BOOL CCedtView::IsPositionInSelection(INT nPosX, INT nPosY)
{
	INT nLineHeight = GetLineHeight(); 
	INT nBegX, nBegY, nEndX, nEndY; GetSelectedPosition(nBegX, nBegY, nEndX, nEndY);

	if( m_bColumnMode ) { /* column editing mode */
		if( nPosY < nBegY ) {
			return FALSE;
		} else if( nPosY < nEndY + nLineHeight ) {
			if( nPosX <  nBegX  ) return FALSE;
			if( nPosX <  nEndX  ) return TRUE;
			/*  nPosX >= nEndX */ return FALSE;
		} else /* nPosY >= nEndY + nLineHeight */ {
			return FALSE;
		}
	} else { /* line editing mode */
		if( nPosY >= nBegY && nPosY < nBegY + nLineHeight && nBegY == nEndY ) {
			if( nPosX <  nBegX  ) return FALSE;
			if( nPosX <  nEndX  ) return TRUE;
			/*  nPosX >= nEndX */ return FALSE;
		} else {
			if( nPosY <  nBegY  ) return FALSE;
			if( nPosY <  nBegY + nLineHeight  ) return (nPosX <  nBegX) ? FALSE : TRUE;
			if( nPosY <  nEndY  ) return TRUE;
			if( nPosY <  nEndY + nLineHeight  ) return (nPosX <  nEndX) ? TRUE : FALSE;
			/*  nPosY >= nEndY + nLineHeight */ return FALSE;
		}
	}

	return FALSE;
}

INT CCedtView::RelativePosToSavedSelection(INT nPosX, INT nPosY)
{
	if( m_bColumnMode ) { /* column editing mode */
		if( nPosY < m_nSaveSelBegY ) {
			if( nPosX <  m_nSaveSelBegX  ) return -4;
			if( nPosX <  m_nSaveSelEndX  ) return -3;
			/*  nPosX >= m_nSaveSelEndX */ return -2;
		} else if( nPosY < m_nSaveSelEndY ) {
			if( nPosX <  m_nSaveSelBegX  ) return -1;
			if( nPosX <  m_nSaveSelEndX  ) return  0;
			/*  nPosX >= m_nSaveSelEndX */ return  1;
		} else /* nPosY > m_nSaveSelEndY */ {
			if( nPosX <  m_nSaveSelBegX  ) return  2;
			if( nPosX <  m_nSaveSelEndX  ) return  3;
			/*  nPosX >= m_nSaveSelEndX */ return  4;
		}
	} else { /* line editing mode */
		if( nPosY == m_nSaveSelBegY && m_nSaveSelBegY == m_nSaveSelEndY ) {
			if( nPosX <  m_nSaveSelBegX  ) return -1;
			if( nPosX <  m_nSaveSelEndX  ) return  0;
			/*  nPosX >= m_nSaveSelEndX */ return  1;
		} else {
			if( nPosY <  m_nSaveSelBegY  ) return -2;
			if( nPosY == m_nSaveSelBegY  ) return (nPosX <  m_nSaveSelBegX) ? -1 : 0;
			if( nPosY <  m_nSaveSelEndY  ) return  0;
			if( nPosY == m_nSaveSelEndY  ) return (nPosX <  m_nSaveSelEndX) ?  0 : 1;
			/*  nPosY >  m_nSaveSelEndY */ return  2;
		}
	}
}

void CCedtView::AdjustCaretAndAnchorPosRelativeToSavedSelection()
{
	INT nRelativePos = RelativePosToSavedSelection(m_nCaretPosX, m_nCaretPosY);

	if( m_bColumnMode ) { // column editing mode
		switch( nRelativePos ) {
		case -4: m_nAnchorPosY = m_nSaveSelEndY; m_nAnchorPosX = m_nSaveSelEndX; break;
		case -3: m_nAnchorPosY = m_nSaveSelEndY; m_nAnchorPosX = m_nSaveSelEndX; m_nCaretPosX = m_nSaveSelBegX; break;
		case -2: m_nAnchorPosY = m_nSaveSelEndY; m_nAnchorPosX = m_nSaveSelBegX; break;
		case -1: m_nAnchorPosY = m_nSaveSelEndY; m_nAnchorPosX = m_nSaveSelEndX; m_nCaretPosY = m_nSaveSelBegY; break;
		case  0: m_nAnchorPosY = m_nSaveSelEndY; m_nAnchorPosX = m_nSaveSelEndX; m_nCaretPosY = m_nSaveSelBegY; m_nCaretPosX = m_nSaveSelBegX; break;
		case  1: m_nAnchorPosY = m_nSaveSelBegY; m_nAnchorPosX = m_nSaveSelBegX; m_nCaretPosY = m_nSaveSelEndY; break;
		case  2: m_nAnchorPosY = m_nSaveSelBegY; m_nAnchorPosX = m_nSaveSelEndX; break;
		case  3: m_nAnchorPosY = m_nSaveSelBegY; m_nAnchorPosX = m_nSaveSelBegX; m_nCaretPosX = m_nSaveSelEndX; break;
		case  4: m_nAnchorPosY = m_nSaveSelBegY; m_nAnchorPosX = m_nSaveSelBegX; break;
		}
	} else { // line editing mode
		switch( nRelativePos ) {
		case -2: m_nAnchorPosY = m_nSaveSelEndY; m_nAnchorPosX = m_nSaveSelEndX; break;
		case -1: m_nAnchorPosY = m_nSaveSelEndY; m_nAnchorPosX = m_nSaveSelEndX; break;
		case  0: m_nAnchorPosY = m_nSaveSelEndY; m_nAnchorPosX = m_nSaveSelEndX; m_nCaretPosY = m_nSaveSelBegY; m_nCaretPosX = m_nSaveSelBegX; break;
		case  1: m_nAnchorPosY = m_nSaveSelBegY; m_nAnchorPosX = m_nSaveSelBegX; break;
		case  2: m_nAnchorPosY = m_nSaveSelBegY; m_nAnchorPosX = m_nSaveSelBegX; break;
		}
	}
}

void CCedtView::AdjustCaretPosAfterLineSelectMouseMove()
{
	if( m_nCaretPosY > m_nAnchorPosY ) {
		if( m_nCaretPosY < GetLastPosY() ) {
			SetCaretPosY( m_nCaretPosY + GetLineHeight() );
			CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
			SetCaretPosX( GetFirstPosX( rLine ) );
		} else {
			CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
			SetCaretPosX( GetLastPosX( rLine ) ); 
		}
	}
}

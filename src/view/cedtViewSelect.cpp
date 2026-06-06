#include "stdafx.h"
#include "cedtHeader.h"


BOOL CCedtView::ActionSelectAll()
{
	SetCaretPosY( 0 );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetFirstPosX( rLine ) );

	m_nAnchorPosY = GetLastPosY();
	CFormatedString & rLne2 = GetLineFromPosY( m_nAnchorPosY );
	m_nAnchorPosX = GetLastPosX( rLne2 );

	return ( (m_nAnchorPosY != m_nCaretPosY) || (m_nAnchorPosX != m_nCaretPosX) );
}

BOOL CCedtView::ActionSelectLine()
{
	if( m_nCaretPosY == GetLastPosY() || m_bColumnMode ) {
		CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
		SetCaretPosX( GetFirstPosX( rLine ) );

		m_nAnchorPosY = m_nCaretPosY;
		m_nAnchorPosX = GetLastPosX( rLine );
	} else {
		CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
		SetCaretPosX( GetFirstPosX( rLine ) );

		m_nAnchorPosY = m_nCaretPosY + GetLineHeight();
		CFormatedString & rLne2 = GetLineFromPosY( m_nAnchorPosY );
		m_nAnchorPosX = GetFirstPosX( rLne2 );
	}

	return ( (m_nAnchorPosY != m_nCaretPosY) || (m_nAnchorPosX != m_nCaretPosX) );
}

BOOL CCedtView::ActionSelectWord()
{
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX );

	INT nBegX, nEndX; GetCurrWordRange( rLine, nIdxX, nBegX, nEndX );
	if( nEndX - nBegX > 0 ) {
		m_nCaretPosX  = GetPosXFromIdxX( rLine, nBegX );
		m_nAnchorPosY = m_nCaretPosY;
		m_nAnchorPosX = GetPosXFromIdxX( rLine, nEndX );
	} else {
		m_nAnchorPosY = m_nCaretPosY;
		m_nAnchorPosX = m_nCaretPosX;
	}

	return ( (m_nAnchorPosY != m_nCaretPosY) || (m_nAnchorPosX != m_nCaretPosX) );
}

BOOL CCedtView::ActionSelectBlock()
{
	if( m_bPairMatched ) { // pair already matched
		INT nBegX, nBegY, nEndX, nEndY;
		if( m_bBeginningPair ) { nBegX = m_nPairPosX[0]; nBegY = m_nPairPosY[0]; nEndX = m_nPairPosX[1]; nEndY = m_nPairPosY[1]; }
		else { nBegX = m_nPairPosX[1]; nBegY = m_nPairPosY[1]; nEndX = m_nPairPosX[0]; nEndY = m_nPairPosY[0]; }
		SetCaretPosY(nBegY); SetCaretPosX(nBegX);
		INT nIdxX, nIdxY; PositionToIndex(nEndX, nEndY, nIdxX, nIdxY); 
		IndexToPosition(nIdxX+1, nIdxY, nEndX, nEndY);
		m_nAnchorPosY = nEndY; m_nAnchorPosX = nEndX;
	} else { // search new pairs position
		INT nBegX, nBegY; PositionToIndex(m_nCaretPosX, m_nCaretPosY, nBegX, nBegY);
		if( ReverseFindBeginningPair(nBegX, nBegY) ) {
			BOOL bBeginning; INT nEndY = nBegY, nEndX = nBegX;
			if( IsThisOneOfPairs(nEndX, nEndY, bBeginning) && FindAnotherOneOfPairs(nEndX, nEndY) ) {
				INT nPosX, nPosY; IndexToPosition(nBegX, nBegY, nPosX, nPosY);
				SetCaretPosY(nPosY); SetCaretPosX(nPosX);
				IndexToPosition(nEndX+1, nEndY, nPosX, nPosY);
				m_nAnchorPosY = nPosY; m_nAnchorPosX = nPosX;
			} else return FALSE;
		} else return FALSE;
	}

	return ( (m_nAnchorPosY != m_nCaretPosY) || (m_nAnchorPosX != m_nCaretPosX) );
}

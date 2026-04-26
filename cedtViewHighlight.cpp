#include "stdafx.h"
#include "cedtHeader.h"


void CCedtView::HighlightActiveLine()
{
	RECT rect; GetClientRect( & rect );
	INT nCharHeight = GetCharHeight();
	INT nLeftMargin = GetLeftMargin();
	CClientDC dc(this); INT nPosY = m_nCaretPosY - m_nScrollPosY;

	// highlight caret line
	if( nPosY >= 0 && nPosY <= rect.bottom ) {
		if( ! m_bActiveLineHighlighted || nPosY != m_nActiveLineHighlightedPosY ) {
			INT nLineWidth  = rect.right - nLeftMargin;
		//	INT nLineHeight = GetLineHeight();

			m_dcActiveLine.BitBlt(nLeftMargin, nCharHeight+1, nLineWidth, nCharHeight+1, & dc, nLeftMargin, nPosY, SRCCOPY);

			m_dcActiveLine.FillSolidRect(nLeftMargin, 0, nLineWidth, nCharHeight, m_crBkgrColor[0]);
			m_dcActiveLine.BitBlt(nLeftMargin, nCharHeight+1, nLineWidth, nCharHeight+1, & m_dcActiveLine, nLeftMargin, 0, SRCINVERT);
			m_dcActiveLine.FillSolidRect(nLeftMargin, 0, nLineWidth, nCharHeight, m_crBkgrColor[2]);
			m_dcActiveLine.BitBlt(nLeftMargin, nCharHeight+1, nLineWidth, nCharHeight+1, & m_dcActiveLine, nLeftMargin, 0, SRCINVERT);

			if( m_bHighlightActiveLine ) {
				for(INT i = nLeftMargin; i < rect.right; i += 2) {
					m_dcActiveLine.SetPixelV(i, nCharHeight+1, m_crTextColor[WT_IDENTIFIER]);
					m_dcActiveLine.SetPixelV(i, nCharHeight+1+nCharHeight, m_crTextColor[WT_IDENTIFIER]);
				}
			}

			m_dcActiveLine.BitBlt(nLeftMargin, 0, nLineWidth, nCharHeight+1, & dc, nLeftMargin, nPosY, SRCCOPY);
			dc.BitBlt(nLeftMargin, nPosY, nLineWidth, nCharHeight+1, & m_dcActiveLine, nLeftMargin, nCharHeight+1, SRCCOPY);
		}

		m_bActiveLineHighlighted = TRUE;
		m_nActiveLineHighlightedPosY = nPosY;
	} else {
		m_bActiveLineHighlighted = FALSE;
		m_nActiveLineHighlightedPosY = 0;
	}
}

void CCedtView::UnhighlightActiveLine()
{
	RECT rect; GetClientRect( & rect );
	INT nCharHeight = GetCharHeight();
	INT nLeftMargin = GetLeftMargin();
	CClientDC dc(this); INT nPosY = m_nActiveLineHighlightedPosY;

	// remove highlight
	if( m_bActiveLineHighlighted && nPosY != m_nCaretPosY - m_nScrollPosY ) {
		INT nLineWidth  = rect.right - nLeftMargin;
	//	INT nLineHeight = GetLineHeight();

		dc.BitBlt(nLeftMargin, nPosY, nLineWidth, nCharHeight+1, & m_dcActiveLine, nLeftMargin, 0, SRCCOPY);

		m_bActiveLineHighlighted = FALSE;
		m_nActiveLineHighlightedPosY = 0;
	}
}

void CCedtView::HighlightColumnMarker()
{
	RECT rect; GetClientRect( & rect );
	INT nLeftMargin = GetLeftMargin();
	INT nAveCharWidth = GetAveCharWidth();
	CClientDC dc(this); INT nPosX, nPosY;

	nPosX = nLeftMargin + m_nColumnMarker1Pos * nAveCharWidth - m_nScrollPosX;
	if( m_bShowColumnMarker1 && (nPosX >= nLeftMargin && nPosX <= rect.right) ) {
		for( nPosY = 0; nPosY < rect.bottom; nPosY += 2 ) dc.SetPixelV(nPosX, nPosY, m_crTextColor[WT_IDENTIFIER]);
	}

	nPosX = nLeftMargin + m_nColumnMarker2Pos * nAveCharWidth - m_nScrollPosX;
	if( m_bShowColumnMarker2 && (nPosX >= nLeftMargin && nPosX <= rect.right) ) {
		for( nPosY = 0; nPosY < rect.bottom; nPosY += 2 ) dc.SetPixelV(nPosX, nPosY, m_crTextColor[WT_IDENTIFIER]);
	}
}

void CCedtView::HighlightMatchingPairs()
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );

	if( m_nCaretPosX > GetLastPosX( rLine ) ) { m_bPairMatched = FALSE; return; }
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX );

	BOOL bBeginning, bIsPair = IsThisOneOfPairs(nIdxX, nIdxY, bBeginning);
	if( ! bIsPair && nIdxX > 0 ) { nIdxX--; bIsPair = IsThisOneOfPairs(nIdxX, nIdxY, bBeginning); }
	if( ! bIsPair ) { m_bPairMatched = FALSE; return; }

	INT nIxY2 = nIdxY, nIxX2 = nIdxX;
	BOOL bIsMatched = FindAnotherOneOfPairs(nIxX2, nIxY2);
	if( ! bIsMatched ) { m_bPairMatched = FALSE; return; }

	IndexToPosition(nIdxX, nIdxY, m_nPairPosX[0], m_nPairPosY[0]);
	IndexToPosition(nIxX2, nIxY2, m_nPairPosX[1], m_nPairPosY[1]);

	CFormatedString & rLne2 = GetLineFromPosY( m_nPairPosY[0] );
	FORMATEDWORD & rWord = GetWordFromPosX( rLne2, m_nPairPosX[0] );
	m_nPairWidth = rWord.m_nWidth;

	RECT rect; GetClientRect( & rect );
	INT nCharHeight = GetCharHeight();
	INT nLeftMargin = GetLeftMargin();
	CClientDC dc(this); RECT rectInvert; INT nPosX, nPosY;

	nPosX = m_nPairPosX[0] - m_nScrollPosX + nLeftMargin;
	nPosY = m_nPairPosY[0] - m_nScrollPosY;
	if( ! m_bSelected && nPosX >= nLeftMargin && nPosX <= rect.right && nPosY >= 0 && nPosY <= rect.bottom ) {
		rectInvert.left = nPosX; rectInvert.right = nPosX + m_nPairWidth;
		rectInvert.top = nPosY + 7 * nCharHeight / 8; rectInvert.bottom = nPosY + nCharHeight;
		dc.InvertRect( & rectInvert ); m_bPairHighlighted[0] = TRUE;
	} else m_bPairHighlighted[0] = FALSE;

	nPosX = m_nPairPosX[1] - m_nScrollPosX + nLeftMargin;
	nPosY = m_nPairPosY[1] - m_nScrollPosY;
	if( ! m_bSelected && nPosX >= nLeftMargin && nPosX <= rect.right && nPosY >= 0 && nPosY <= rect.bottom ) {
		rectInvert.left = nPosX; rectInvert.right = nPosX + m_nPairWidth;
		rectInvert.top = nPosY + 7 * nCharHeight / 8; rectInvert.bottom = nPosY + nCharHeight;
		dc.InvertRect( & rectInvert ); m_bPairHighlighted[1] = TRUE;
	} else m_bPairHighlighted[1] = FALSE;

	m_bBeginningPair = bBeginning;
	m_bPairMatched = TRUE;
}

void CCedtView::UnhighlightPairs()
{
	INT nCharHeight = GetCharHeight();
	INT nLeftMargin = GetLeftMargin();
	CClientDC dc(this); RECT rectInvert; INT nPosX, nPosY;

	if( m_bPairHighlighted[0] ) {
		nPosX = m_nPairPosX[0] - m_nScrollPosX + nLeftMargin;
		nPosY = m_nPairPosY[0] - m_nScrollPosY;
		rectInvert.left = nPosX; rectInvert.right = nPosX + m_nPairWidth;
		rectInvert.top = nPosY + 7 * nCharHeight / 8; rectInvert.bottom = nPosY + nCharHeight;
		dc.InvertRect( & rectInvert ); m_bPairHighlighted[0] = FALSE;
	}

	if( m_bPairHighlighted[1] ) {
		nPosX = m_nPairPosX[1] - m_nScrollPosX + nLeftMargin;
		nPosY = m_nPairPosY[1] - m_nScrollPosY;
		rectInvert.left = nPosX; rectInvert.right = nPosX + m_nPairWidth;
		rectInvert.top = nPosY + 7 * nCharHeight / 8; rectInvert.bottom = nPosY + nCharHeight;
		dc.InvertRect( & rectInvert ); m_bPairHighlighted[1] = FALSE;
	}

	m_bPairMatched = FALSE;
}


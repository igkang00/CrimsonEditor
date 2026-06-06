#include "stdafx.h"
#include "cedtHeader.h"



INT CCedtView::GetLogPixelsX(CDC * pDC)
{
	if( pDC ) return pDC->GetDeviceCaps(LOGPIXELSX);
	else return m_dcScreen.GetDeviceCaps(LOGPIXELSX);
}

INT CCedtView::GetLogPixelsY(CDC * pDC)
{
	if( pDC ) return pDC->GetDeviceCaps(LOGPIXELSY);
	else return m_dcScreen.GetDeviceCaps(LOGPIXELSY);
}


BOOL CCedtView::IsUsingFixedPitchFont(CDC * pDC)
{
	static INT nSequence = -1;
	static BOOL bFixed = FALSE;

	// TEXTMETRIC.tmPitchAndFamily 
	//
	// TMPF_FIXED_PITCH: If this bit is set the font is a variable pitch font. 
	// If this bit is clear the font is a fixed pitch font. Note very carefully 
	// that those meanings are the opposite of what the constant name implies.

	if( pDC ) {
		TEXTMETRIC tm; pDC->GetTextMetrics( & tm );
		return ! (tm.tmPitchAndFamily & TMPF_FIXED_PITCH);
	} else if( nSequence != m_nScreenFontChangeSequence ) {
		TEXTMETRIC tm; m_dcScreen.GetTextMetrics( & tm );
		nSequence = m_nScreenFontChangeSequence;
		return bFixed = ! (tm.tmPitchAndFamily & TMPF_FIXED_PITCH);
	} else return bFixed;
}

INT CCedtView::GetCharHeight(CDC * pDC)
{
	static INT nSequence = -1, nHeight = 0;

	if( pDC ) {
		TEXTMETRIC tm; pDC->GetTextMetrics( & tm ); 
		return tm.tmHeight; 
	} else if( nSequence != m_nScreenFontChangeSequence ) {
		TEXTMETRIC tm; m_dcScreen.GetTextMetrics( & tm );
		nSequence = m_nScreenFontChangeSequence; 
		return nHeight = tm.tmHeight; 
	} else return nHeight;
}

INT CCedtView::GetSpaceWidth(CDC * pDC)
{
	static INT nSequence = -1, nWidth = 0;

	if( pDC ) {
		CSize size = pDC->GetTextExtent(" ", 1);
		return size.cx; 
	} else if( nSequence != m_nScreenFontChangeSequence ) {
		CSize size = m_dcScreen.GetTextExtent(" ", 1);
		nSequence = m_nScreenFontChangeSequence;
		return nWidth = size.cx;
	} else return nWidth;
}

INT CCedtView::GetNumberWidth(CDC * pDC)
{
	static INT nSequence = -1, nWidth = 0;

	if( pDC ) {
		CSize size = pDC->GetTextExtent("0", 1);
		return size.cx; 
	} else if( nSequence != m_nScreenFontChangeSequence ) {
		CSize size = m_dcScreen.GetTextExtent("0", 1);
		nSequence = m_nScreenFontChangeSequence;
		return nWidth = size.cx;
	} else return nWidth;
}

INT CCedtView::GetAveCharWidth(CDC * pDC)
{
	static INT nSequence = -1, nWidth = 0;

	if( pDC ) {
		TEXTMETRIC tm; pDC->GetTextMetrics( & tm ); 
		return tm.tmAveCharWidth; 
	} else if( nSequence != m_nScreenFontChangeSequence ) {
		TEXTMETRIC tm; m_dcScreen.GetTextMetrics( & tm );
		nSequence = m_nScreenFontChangeSequence; 
		return nWidth = tm.tmAveCharWidth; 
	} else return nWidth;
}

INT CCedtView::GetMaxCharWidth(CDC * pDC)
{
	static INT nSequence = -1, nWidth = 0;

	if( pDC ) {
		TEXTMETRIC tm; pDC->GetTextMetrics( & tm ); 
		return tm.tmMaxCharWidth; 
	} else if( nSequence != m_nScreenFontChangeSequence ) {
		TEXTMETRIC tm; m_dcScreen.GetTextMetrics( & tm );
		nSequence = m_nScreenFontChangeSequence; 
		return nWidth = tm.tmMaxCharWidth; 
	} else return nWidth;
}

INT CCedtView::GetLeftMargin(CDC * pDC)
{
	if( pDC ) {
		INT nNumberWidth = GetNumberWidth( pDC  );
		return m_bPrintLineNumbers ? (4 * m_nLineNumberDigits + 8) * nNumberWidth / 4 : 0;
	} else {
		INT nNumberWidth = GetNumberWidth( NULL );
		INT nExtraMargin = m_bShowSelectionMargin ? 10 * nNumberWidth / 4 : 1;
		return m_bShowLineNumbers  ? (4 * m_nLineNumberDigits + 2) * nNumberWidth / 4 : nExtraMargin;
	}
}

INT CCedtView::GetLineHeight(CDC * pDC)
{
	static INT nSequence = -1, nHeight = 0;

	if( pDC ) {
		TEXTMETRIC tm; pDC->GetTextMetrics( & tm ); 
		return tm.tmHeight * m_nLineSpacing / 100;
	} else if( nSequence != m_nScreenFontChangeSequence ) {
		TEXTMETRIC tm; m_dcScreen.GetTextMetrics( & tm );
		nSequence = m_nScreenFontChangeSequence; nHeight = tm.tmHeight;
		return (nHeight = tm.tmHeight) * m_nLineSpacing / 100;
	} else return nHeight * m_nLineSpacing / 100;
}


INT CCedtView::GetCharWidth()
{
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	FORMATEDWORD & rWord = GetWordFromPosX( rLine, m_nCaretPosX );
	if( rWord.m_siLength == 1 ) return rWord.m_nWidth;

	INT nIdxX = GetIdxXFromPosX(rLine, rWord, m_nCaretPosX );
	INT nLength = IS_DBCHAR(rWord) ? 2 : 1;

	CSize size = m_dcScreen.GetTextExtent((LPCTSTR)rLine + nIdxX, nLength);
	return size.cx;
}

INT CCedtView::GetLinesPerPage()
{
	CRect rect; GetClientRect( & rect );
	INT nLineHeight = GetLineHeight( NULL );

	return rect.Height() / nLineHeight;
}

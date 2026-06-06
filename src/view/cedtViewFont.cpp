#include "stdafx.h"
#include "cedtHeader.h"


BOOL CCedtView::IsFixedPitchScreenFont(INT nFont)
{
	// The two low-order bits specify the pitch of the font and can be one of the following values: 
	// DEFAULT_PITCH, FIXED_PITCH, VARIABLE_PITCH 

	BYTE byPitch = m_lfScreen[nFont].lfPitchAndFamily & 0x03;
	return (byPitch == FIXED_PITCH); 
}

void CCedtView::CreateScreenFontObject()
{
	if( m_fontScreen.m_hObject ) m_fontScreen.DeleteObject();
	if( m_fontScreenIt.m_hObject ) m_fontScreenIt.DeleteObject();

	LOGFONT lf; memcpy( & lf, & m_lfScreen[m_nCurrentScreenFont], sizeof(LOGFONT) );
	if( m_bUsingColumnModeFont ) memcpy( & lf, & m_lfMiscel[0x00], sizeof(LOGFONT) );
	lf.lfHeight = -MulDiv( lf.lfHeight, m_dcScreen.GetDeviceCaps(LOGPIXELSY), 720 );
	m_fontScreen.CreateFontIndirect( & lf ); 

	lf.lfItalic = TRUE;
	m_fontScreenIt.CreateFontIndirect( & lf );
}

void CCedtView::CreatePrinterFontObject(CDC * pDC)
{
	if( m_fontPrinter.m_hObject ) m_fontPrinter.DeleteObject();
	if( m_fontPrinterIt.m_hObject ) m_fontPrinterIt.DeleteObject();

	LOGFONT lf; memcpy( & lf, & m_lfPrinter[m_nCurrentPrinterFont], sizeof(LOGFONT) );
	m_fontPrinter.CreatePointFontIndirect( & lf, pDC );

	lf.lfItalic = TRUE;
	m_fontPrinterIt.CreatePointFontIndirect( & lf, pDC );
}

CFont * CCedtView::ApplyCurrentScreenFont()
{
	CFont * pFontOld = m_dcScreen.SelectObject( & m_fontScreen );
	m_nScreenFontChangeSequence++; // flag to inform that there is a change in screen font

	CCedtApp * pApp = (CCedtApp *)AfxGetApp();
	POSITION posDoc = pApp->GetFirstDocPosition();
	while( posDoc ) {
		CCedtDoc * pDoc = (CCedtDoc *)pApp->GetNextDoc( posDoc );
		POSITION posView = pDoc->GetFirstViewPosition();
		while( posView ) {
			CCedtView * pView = (CCedtView *)pDoc->GetNextView( posView );
			pView->OnScreenFontChange();
		}
	}

	return pFontOld;
}


CFont * CCedtView::ApplyCurrentPrinterFont(CDC * pDC)
{
	CFont * pFontOld = pDC->SelectObject( & m_fontPrinter );
	return pFontOld;
}


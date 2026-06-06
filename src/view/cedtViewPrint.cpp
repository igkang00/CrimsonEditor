#include "stdafx.h"
#include "cedtHeader.h"


static CRect _rectDraw;


BOOL CCedtView::OnPreparePrinting(CPrintInfo* pInfo)
{
	if( GetSelectedLineCount() ) { // Enable Print in Selection
		CPrintDialog * pDlg = pInfo->m_pPD;
		pDlg->m_pd.Flags &= ~PD_NOSELECTION;
		pDlg->m_pd.Flags |=  PD_SELECTION; 
	}

	return DoPreparePrinting(pInfo);
}

void CCedtView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	pDC->SetMapMode( MM_LOMETRIC );
	pDC->SetBkMode( TRANSPARENT );

	_rectDraw.left = m_rectPageMargin.left;
	_rectDraw.top = m_rectPageMargin.top;
	_rectDraw.right = 10 * pDC->GetDeviceCaps(HORZSIZE) - m_rectPageMargin.right;
	_rectDraw.bottom = 10 * pDC->GetDeviceCaps(VERTSIZE) - m_rectPageMargin.bottom;

	CreatePrinterFontObject(pDC);
	ApplyCurrentPrinterFont(pDC);

	INT nLineHeight = GetLineHeight( pDC );
	INT nLinesPerPage = _rectDraw.Height() / nLineHeight;
	if( m_bPrintHeader ) nLinesPerPage = nLinesPerPage - 2;
	if( m_bPrintFooter ) nLinesPerPage = nLinesPerPage - 2;

	CPrintDialog * pDlg = pInfo->m_pPD;
	if( pDlg->m_pd.Flags & PD_SELECTION ) {
		INT nBegX, nBegY, nEndX, nEndY; GetSelectedIndex( nBegX, nBegY, nEndX, nEndY );
		INT nCount = nEndY - nBegY + 1; if( ! nEndX ) nCount = nEndY - nBegY;

		FormatPrintText( pDC, _rectDraw, nBegY, nCount );
		m_nFormatedPrintTextStartIdxY = nBegY;
	} else {
		FormatPrintText( pDC, _rectDraw );
		m_nFormatedPrintTextStartIdxY = 0;
	}

	INT nLineCount = m_clsFormatedPrintText.GetCount();

	pInfo->SetMinPage(1);
	pInfo->SetMaxPage( (nLineCount / nLinesPerPage) + 1 );
	if( nLineCount % nLinesPerPage == 0 ) pInfo->SetMaxPage( nLineCount / nLinesPerPage );
}

void CCedtView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	pDC->SetMapMode( MM_LOMETRIC );
	pDC->SetBkMode( TRANSPARENT );

	CreatePrinterFontObject(pDC);
	ApplyCurrentPrinterFont(pDC);

	ParsePageHeaderAndFooter(pInfo->m_nCurPage, pInfo->GetMaxPage());

	DrawPrintPageOutline( pDC, _rectDraw, pInfo->m_nCurPage );
	DrawPrintPageBackgroundAndText( pDC, _rectDraw, pInfo->m_nCurPage );
}

void CCedtView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	m_clsFormatedPrintText.RemoveAll();
	m_nFormatedPrintTextStartIdxY = 0;
}

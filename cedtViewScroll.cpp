#include "stdafx.h"
#include "cedtHeader.h"


void CCedtView::SetScrollPosY(INT nPosY)
{
	INT nLineHeight = GetLineHeight();
	INT nLastPosY = GetLastPosY();

	if( nPosY > nLastPosY ) m_nScrollPosY = nLastPosY;
	else if( nPosY > 0 ) m_nScrollPosY = (nPosY / nLineHeight) * nLineHeight;
	else m_nScrollPosY = 0;
}

void CCedtView::SetScrollPosX(INT nPosX)
{
	if( nPosX > 0 ) m_nScrollPosX = nPosX;
	else m_nScrollPosX = 0;
}

void CCedtView::SetScrollPosSyncAllPanes(INT nPosX, INT nPosY)
{
	CChildFrame * pChild = (CChildFrame *)GetParentFrame(); ASSERT( pChild );
	pChild->SetScrollPosSyncAllPanes(this, nPosX, nPosY);
}

void CCedtView::SetScrollPosToMakeCaretVisible()
{
	RECT rect; GetClientRect( & rect );
	INT nLeftMargin = GetLeftMargin();	INT nMaxCharWidth = GetMaxCharWidth();
	INT nLineHeight = GetLineHeight();	INT nLinesPerPage = GetLinesPerPage();

	INT nIncrementX = 16 * GetAveCharWidth();
	INT nPosX = m_nScrollPosX;			INT nPosY = m_nScrollPosY;

	if( m_nCaretPosX < m_nScrollPosX ) nPosX = m_nCaretPosX - nIncrementX;
	if( m_nCaretPosX > m_nScrollPosX - nLeftMargin + rect.right - nMaxCharWidth ) nPosX = m_nCaretPosX + nLeftMargin - rect.right + nIncrementX;
	if( m_nCaretPosY < m_nScrollPosY ) nPosY = m_nCaretPosY;
	if( m_nCaretPosY > m_nScrollPosY + (nLinesPerPage - 1) * nLineHeight ) nPosY = m_nCaretPosY - (nLinesPerPage - 1) * nLineHeight;

	SetScrollPosSyncAllPanes(nPosX, nPosY);
}

void CCedtView::SetScrollPosToMakeAutoScroll()
{
	RECT rect; GetClientRect( & rect );
	INT nLeftMargin = GetLeftMargin();	INT nMaxCharWidth = GetMaxCharWidth();
	INT nLineHeight = GetLineHeight();	INT nLinesPerPage = GetLinesPerPage();

	INT nPosX = m_nScrollPosX;			INT nPosY = m_nScrollPosY;

	if( m_nCaretPosX < m_nScrollPosX ) nPosX = m_nCaretPosX;
	if( m_nCaretPosX > m_nScrollPosX - nLeftMargin + rect.right - nMaxCharWidth ) nPosX = m_nCaretPosX + nLeftMargin - rect.right + nMaxCharWidth;
	if( m_nCaretPosY < m_nScrollPosY ) nPosY = m_nCaretPosY;;
	if( m_nCaretPosY > m_nScrollPosY + (nLinesPerPage - 1) * nLineHeight ) nPosY = m_nCaretPosY - (nLinesPerPage - 1) * nLineHeight;

	SetScrollPosSyncAllPanes(nPosX, nPosY);
}

void CCedtView::SetScrollPosToMakeDragScroll()
{
	RECT rect; GetClientRect( & rect );
	INT nLeftMargin = GetLeftMargin();	INT nMaxCharWidth = GetMaxCharWidth();
	INT nLineHeight = GetLineHeight();	INT nLinesPerPage = GetLinesPerPage();

	INT nSkip = (rect.bottom % nLineHeight < nLineHeight / 2) ? 2 : 1;
	INT nPosX = m_nScrollPosX;			INT nPosY = m_nScrollPosY;

	if( m_nDragPosX < m_nScrollPosX ) nPosX = m_nDragPosX;
	if( m_nDragPosX > m_nScrollPosX - nLeftMargin + rect.right - nMaxCharWidth ) nPosX = m_nDragPosX + nLeftMargin - rect.right + nMaxCharWidth;
	if( m_nDragPosY < m_nScrollPosY + nLineHeight ) nPosY = m_nDragPosY - nLineHeight;
	if( m_nDragPosY > m_nScrollPosY + (nLinesPerPage - nSkip) * nLineHeight ) nPosY = m_nDragPosY - (nLinesPerPage - nSkip) * nLineHeight;

	SetScrollPosSyncAllPanes(nPosX, nPosY);
}

void CCedtView::SetScrollPosToMakeCaretTop()
{
	SetScrollPosSyncAllPanes( m_nScrollPosX, m_nCaretPosY );
	SetScrollPosToMakeCaretVisible();
}

void CCedtView::SetScrollPosToMakeCaretBottom()
{
	SetScrollPosSyncAllPanes( m_nScrollPosX, m_nCaretPosY - (GetLinesPerPage() - 1) * GetLineHeight() );
	SetScrollPosToMakeCaretVisible();
}

void CCedtView::SetScrollPosToMakeCaretCenter()
{
//	RECT rect; GetClientRect( & rect );
	SetScrollPosSyncAllPanes( m_nScrollPosX, m_nCaretPosY - (GetLinesPerPage() - 1) / 2 * GetLineHeight() );
	SetScrollPosToMakeCaretVisible();
}


#include "stdafx.h"
#include "cedtHeader.h"



void CCedtView::SetClipboardData(CMemText & rBlock)
{
	CWnd * pWnd = AfxGetMainWnd();
	::OpenClipboard( pWnd->m_hWnd );
	::EmptyClipboard();

	HGLOBAL hMemory = SetGlobalMemoryText( rBlock );
	::SetClipboardData(CF_TEXT, hMemory);
	::CloseClipboard();
}

BOOL CCedtView::GetClipboardData(CMemText & rBlock)
{
	CWnd * pWnd = AfxGetMainWnd();
	::OpenClipboard( pWnd->m_hWnd );
	HGLOBAL hMemory = ::GetClipboardData(CF_TEXT);

	if( ! GetGlobalMemoryText( hMemory, rBlock ) ) { ::CloseClipboard(); return FALSE; }
	::CloseClipboard();
	return TRUE;
}

BOOL CCedtView::HasClipboardData()
{
	CWnd * pWnd = AfxGetMainWnd();
	::OpenClipboard( pWnd->m_hWnd );
	HGLOBAL hMemory = ::GetClipboardData(CF_TEXT);

	::CloseClipboard();
	return hMemory ? TRUE : FALSE;
}

BOOL CCedtView::GetOleDataObjectData(COleDataObject * pDataObject, CMemText & rBlock)
{
	HGLOBAL hMemory = pDataObject->GetGlobalData(CF_TEXT);
	return GetGlobalMemoryText( hMemory, rBlock );
}

BOOL CCedtView::GetGlobalMemoryText(HGLOBAL hMemory, CMemText & rBlock)
{
	if( ! hMemory ) return FALSE;
	INT size = ::GlobalSize(hMemory);
	void * pMemory = ::GlobalLock(hMemory);
	rBlock.MemoryLoad((char *)pMemory, size);
	::GlobalUnlock(hMemory);
	return TRUE;
}

HGLOBAL CCedtView::SetGlobalMemoryText(CMemText & rBlock)
{
	INT size = rBlock.MemorySize();
	HGLOBAL hMemory = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, size);
	void * pMemory = ::GlobalLock(hMemory);
	rBlock.MemorySave((char *)pMemory, size);
	::GlobalUnlock(hMemory);
	return hMemory;
}

BOOL CCedtView::MultiLineStringConstant()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->MultiLineStringConstant();
}

BOOL CCedtView::VariableHighlightInString()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->VariableHighlightInString();
}

BOOL CCedtView::HasLineCommentDelimiter()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->HasLineCommentDelimiter();
}

BOOL CCedtView::HasBlockCommentDelimiter()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->HasBlockCommentDelimiter();
}


UINT CCedtView::GetKeyState()
{
	UINT nFlags = 0x00;
	if( ::GetKeyState(VK_CONTROL) & 0xF0 ) nFlags |= KEYSTATE_CONTROL;
	if( ::GetKeyState(VK_SHIFT) & 0xF0 ) nFlags |= KEYSTATE_SHIFT;
	if( ::GetKeyState(VK_MENU) & 0xF0 ) nFlags |= KEYSTATE_MENU;
	return nFlags;
}

CString CCedtView::GetCurrentDate()
{
	CTime time = CTime::GetCurrentTime();
	return time.Format("%B %d, %Y");
}

CString CCedtView::GetCurrentTime()
{
	CTime time = CTime::GetCurrentTime();
	return time.Format("%I:%M:%S %p");
}


// MainFrameDropTarget.cpp : implementation file
//

#include "stdafx.h"
#include "cedtHeader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrameDropTarget

CMainFrameDropTarget::CMainFrameDropTarget()
{
}

CMainFrameDropTarget::~CMainFrameDropTarget()
{
}


DROPEFFECT CMainFrameDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	CMainFrame * pMainFrame = (CMainFrame *)pWnd;
	return pMainFrame->OnDragEnter(pDataObject, dwKeyState, point);
}

DROPEFFECT CMainFrameDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	CMainFrame * pMainFrame = (CMainFrame *)pWnd;
	return pMainFrame->OnDragOver(pDataObject, dwKeyState, point);
}

void CMainFrameDropTarget::OnDragLeave(CWnd* pWnd)
{
	CMainFrame * pMainFrame = (CMainFrame *)pWnd;
	pMainFrame->OnDragLeave();
}


BOOL CMainFrameDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	CMainFrame * pMainFrame = (CMainFrame *)pWnd;
	return pMainFrame->OnDrop(pDataObject, dropEffect, point);
}

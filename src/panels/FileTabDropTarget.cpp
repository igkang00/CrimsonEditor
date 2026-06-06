// FileTabDropTarget.cpp : implementation file
//

#include "stdafx.h"
#include "cedtHeader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMDIFileTabDropTarget

CMDIFileTabDropTarget::CMDIFileTabDropTarget()
{
}

CMDIFileTabDropTarget::~CMDIFileTabDropTarget()
{
}


DROPEFFECT CMDIFileTabDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	CMDIFileTab * pMDIFileTab = (CMDIFileTab *)pWnd;
	return pMDIFileTab->OnDragEnter(pDataObject, dwKeyState, point);
}

DROPEFFECT CMDIFileTabDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	CMDIFileTab * pMDIFileTab = (CMDIFileTab *)pWnd;
	return pMDIFileTab->OnDragOver(pDataObject, dwKeyState, point);
}

void CMDIFileTabDropTarget::OnDragLeave(CWnd* pWnd)
{
	CMDIFileTab * pMDIFileTab = (CMDIFileTab *)pWnd;
	pMDIFileTab->OnDragLeave();
}

BOOL CMDIFileTabDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	CMDIFileTab * pMDIFileTab = (CMDIFileTab *)pWnd;
	return pMDIFileTab->OnDrop(pDataObject, dropEffect, point);
}

// FileWindowDropTarget.cpp : implementation file
//

#include "stdafx.h"
#include "cedtHeader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileWindowDropTarget

CFileWindowDropTarget::CFileWindowDropTarget()
{
}

CFileWindowDropTarget::~CFileWindowDropTarget()
{
}


DROPEFFECT CFileWindowDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	CFileWindow * pFileWindow = (CFileWindow *)pWnd;
	return pFileWindow->OnDragEnter(pDataObject, dwKeyState, point);
}

DROPEFFECT CFileWindowDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	CFileWindow * pFileWindow = (CFileWindow *)pWnd;
	return pFileWindow->OnDragOver(pDataObject, dwKeyState, point);
}

void CFileWindowDropTarget::OnDragLeave(CWnd* pWnd)
{
	CFileWindow * pFileWindow = (CFileWindow *)pWnd;
	pFileWindow->OnDragLeave();
}


BOOL CFileWindowDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	CFileWindow * pFileWindow = (CFileWindow *)pWnd;
	return pFileWindow->OnDrop(pDataObject, dropEffect, point);
}

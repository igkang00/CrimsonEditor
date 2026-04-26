#if !defined(AFX_FILEWINDOWDROPTARGET_H__8B34C2DB_6E0C_473F_BE22_958E7446EC7C__INCLUDED_)
#define AFX_FILEWINDOWDROPTARGET_H__8B34C2DB_6E0C_473F_BE22_958E7446EC7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CFileWindowDropTarget

class CFileWindowDropTarget : public COleDropTarget
{
public:
    CFileWindowDropTarget();
	virtual ~CFileWindowDropTarget();

public:
	DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	void OnDragLeave(CWnd* pWnd);
	BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEWINDOWDROPTARGET_H__8B34C2DB_6E0C_473F_BE22_958E7446EC7C__INCLUDED_)

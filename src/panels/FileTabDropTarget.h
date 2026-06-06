#if !defined(AFX_FILETABDROPTARGET_H__46519441_1434_49E4_B50C_EC5DFE8832C6__INCLUDED_)
#define AFX_FILETABDROPTARGET_H__46519441_1434_49E4_B50C_EC5DFE8832C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CMDIFileTabDropTarget command target

class CMDIFileTabDropTarget : public COleDropTarget
{
public:
    CMDIFileTabDropTarget();
	virtual ~CMDIFileTabDropTarget();

public:
	DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	void OnDragLeave(CWnd* pWnd);
	BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILETABDROPTARGET_H__46519441_1434_49E4_B50C_EC5DFE8832C6__INCLUDED_)

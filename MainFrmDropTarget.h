#if !defined(AFX_MAINFRAMEDROPTARGET_H__DF80E209_D9B9_445F_88AF_C71E37B3DAB1__INCLUDED_)
#define AFX_MAINFRAMEDROPTARGET_H__DF80E209_D9B9_445F_88AF_C71E37B3DAB1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CMainFrameDropTarget command target

class CMainFrameDropTarget : public COleDropTarget
{
public:
	CMainFrameDropTarget();
	virtual ~CMainFrameDropTarget();

public:
	DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	void OnDragLeave(CWnd* pWnd);
	BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRAMEDROPTARGET_H__DF80E209_D9B9_445F_88AF_C71E37B3DAB1__INCLUDED_)

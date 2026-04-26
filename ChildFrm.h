// ChildFrm.h : interface of the CChildFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDFRM_H__FFCA2B8A_F9C5_11D4_A6F1_0050CE184C9B__INCLUDED_)
#define AFX_CHILDFRM_H__FFCA2B8A_F9C5_11D4_A6F1_0050CE184C9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)

public:
	CChildFrame();


protected: // Attributes
	CSplitterWndEx m_wndSplitter;


public: // Operations
	INT  GetRowCount() { return m_wndSplitter.GetRowCount(); }
	INT  GetColumnCount() { return m_wndSplitter.GetColumnCount(); }
	INT  GetPaneCount() { return GetRowCount() * GetColumnCount(); }

	CWnd * GetNeighborPane(CWnd * pWnd);
	void SetScrollPosSyncAllPanes(CWnd * pWnd, INT nPosX, INT nPosY);

	void UpdateAllPanesInTheSameRow(CWnd * pWnd);
	void UpdateAllPanesSharingScrollBar(CWnd * pWnd);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrame)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CChildFrame)
	afx_msg void OnDestroy();
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRM_H__FFCA2B8A_F9C5_11D4_A6F1_0050CE184C9B__INCLUDED_)

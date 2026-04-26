#if !defined(AFX_FILETAB_H__CF314FB5_538C_11D3_978A_00104B151BA8__INCLUDED_)
#define AFX_FILETAB_H__CF314FB5_538C_11D3_978A_00104B151BA8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FileTab.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMDIFileTab window

class CMDIFileTab : public CDialogBar
{
// Construction
public:
	CMDIFileTab();


protected: // Attributes
	CImageList m_lstImage;
	CXPTabCtrl m_ctlXPTab;
	CToolTipCtrl m_ctlToolTip;

protected: // drag and drop
	CMDIFileTabDropTarget m_oleDropTarget;
	COleDataSource m_oleDataSource;

	INT m_nDragObjectType, m_nDragItemCount;
	INT m_nDragCurSel, m_nDropPosition;


protected: // Operations
	void OnLButtonDownTabCtrl(UINT nFlags, CPoint point);
	void OnLButtonDblClkTabCtrl(UINT nFlags, CPoint point);
	void OnMButtonDownTabCtrl(UINT nFlags, CPoint point);
	void OnMButtonUpTabCtrl(UINT nFlags, CPoint point);

	void OnBegindragTabCtrl(UINT nFlags, CPoint point);

public: // drag and drop
	DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);

	void OnDragLeave();
	BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);


public:
	void InsertMDIFileTab(CMDIChildWnd * pChild, INT nTab = -1);
	void DeleteMDIFileTab(CMDIChildWnd * pChild);

	void UpdateMDIFileTab(CMDIChildWnd * pChild);
	void SetActiveFileTab(CMDIChildWnd * pChild);

public:
	INT  GetMDIFileTabCount();
	CMDIChildWnd * GetFileTabItem(INT nTab);

	CMDIChildWnd * GetNextFileTabItem();
	CMDIChildWnd * GetPrevFileTabItem();


protected:
	void AdjustTabString(CString & szFileName);

	int  FindTabIndex(CMDIChildWnd * pChild);
	int  FindTabIndex(CPoint point);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMDIFileTab)
	public:
	virtual BOOL Create(CWnd * pParentWnd);
    virtual CSize CalcFixedLayout( BOOL bStretch, BOOL bHorz );
    virtual CSize CalcDynamicLayout( int nLength, DWORD dwMode );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMDIFileTab();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMDIFileTab)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSelchangeFileTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnNeedtextFileTab(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFileTabRefresh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILETAB_H__CF314FB5_538C_11D3_978A_00104B151BA8__INCLUDED_)

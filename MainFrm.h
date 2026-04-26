// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__FFCA2B88_F9C5_11D4_A6F1_0050CE184C9B__INCLUDED_)
#define AFX_MAINFRM_H__FFCA2B88_F9C5_11D4_A6F1_0050CE184C9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)

public:
	CMainFrame();


protected: // Attributes
	CList<CMDIChildWnd *, CMDIChildWnd *> m_lstChildFrame;
	BOOL m_bSwitchingChildFrame;
	BOOL m_bPrintPreviewMode;

protected: // drag and drop
	CMainFrameDropTarget m_oleDropTarget;
	INT  m_nDragObjectType;

protected: // control bar embedded members
	CStatusBarEx	m_wndStatusBar;
	CToolBar		m_wndToolBar;
	CMDIFileTab		m_wndFileTab;

	CFileWindow		m_wndFileWindow;
	COutputWindow	m_wndOutputWindow;


protected: // micellaneous
	void OnUpdateFrameTitle(BOOL bAddToTitle);
	void OnSizeMainWindow(int cx, int cy);


public: // drag and drop
	DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);

	void OnDragLeave();
	BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);

protected:
	BOOL IsPointInControlBars(CPoint point);

public: // menu item refresh
	BOOL RefreshSyntaxTypeMenu();
	BOOL RefreshLoadUserToolsMenu();
	BOOL RefreshLoadUserMacrosMenu();

protected:
	CMenu * FindParentMenuContaing(CMenu * pParentMenu, UINT nFindMenuID);


public: // window placement
	BOOL SaveWindowPlacement(LPCTSTR lpszProfileName);
	BOOL LoadWindowPlacement(LPCTSTR lpszProfileName);


public: // status bar
	void BeginProgress(LPCTSTR lpszMessage) { m_wndStatusBar.BeginProgress(lpszMessage); }
	void EndProgress() { m_wndStatusBar.EndProgress(); }
	void SetProgress(INT nPercent) { m_wndStatusBar.SetProgress(nPercent); }

	void SetSplashMessage(UINT nID, UINT nElapse) { m_wndStatusBar.SetSplashMessage(nID, nElapse); }
	void SetSplashMessage(LPCTSTR lpszMessage, UINT nElapse) { m_wndStatusBar.SetSplashMessage(lpszMessage, nElapse); }

	void SetCaretPositionInfo(INT nLine, INT nCol, INT nMax);
	void SetFileStatusInfo(INT nEncodingType, INT nFileFormat, BOOL bReadOnly);

	void SetMacroRecordingFlag(BOOL bRecording);
	void SetColumnModeFlag(BOOL bColumnMode);
	void SetOverwriteFlag(BOOL bOverwrite);


public: // MDI file tab
	void InsertMDIFileTab(CMDIChildWnd * pChild, INT nTab = -1);
	void DeleteMDIFileTab(CMDIChildWnd * pChild);

	void UpdateMDIFileTab(CMDIChildWnd * pChild);
	void SetActiveFileTab(CMDIChildWnd * pChild);


public: // file window
	CWnd * GetFileWindow() { return & m_wndFileWindow; }
	BOOL IsFileWindowVisible() { return m_wndFileWindow.IsWindowVisible(); }
	void ShowFileWindow(BOOL bShow) { ShowControlBar( & m_wndFileWindow, bShow, FALSE ); }

	void ShowFileWindowCategory(INT nCategory);
	void ToggleFileWindowCategory(INT nCategory);


public: // output window
	CWnd * GetOutputWindow() { return & m_wndOutputWindow; }
	BOOL IsOutputWindowVisible() { return m_wndOutputWindow.IsWindowVisible(); }
	void ShowOutputWindow(BOOL bShow) { ShowControlBar( & m_wndOutputWindow, bShow, FALSE ); }

	void ApplyOutputWindowFont(BOOL bRedraw = TRUE) { m_wndOutputWindow.ApplyOutputFont(bRedraw); }
	void EnableOutputWindowInput(BOOL bEnable) { m_wndOutputWindow.EnableInputConsole(bEnable); }

	void SetOutputWindowOccupied(BOOL bOccupy) { m_wndOutputWindow.SetOccupied(bOccupy); }
	BOOL CanUseOutputWindow() { return m_wndOutputWindow.CanUseNow(); }

	void ClearOutputWindowContents() { m_wndOutputWindow.SendMessage(WM_COMMAND, ID_OUTPUT_WINDOW_CLEAR, 0L); }
	void CopyAllOutputWindowContents() { m_wndOutputWindow.SendMessage(WM_COMMAND, ID_OUTPUT_WINDOW_COPY_ALL, 0L); }

	BOOL AddStringToOutputWindow(LPCTSTR lpszString, COLORREF crTextColor) { return m_wndOutputWindow.AddStringToTheLast(lpszString, crTextColor); }
	BOOL ReplaceStringOfOutputWindow(LPCTSTR lpszString, COLORREF crTextColor) { return m_wndOutputWindow.ReplaceTheLastString(lpszString, crTextColor); }
	BOOL GetStringOfOutputWindow(CString & szString) { return m_wndOutputWindow.GetTheLastString(szString); }


public: // child window
	INT  GetChildFrameCount() { return m_wndFileTab.GetMDIFileTabCount(); }
	CMDIChildWnd * GetChildFrame(INT nTab) { return m_wndFileTab.GetFileTabItem(nTab); }

	CMDIChildWnd * GetNextChildFrame() { return m_wndFileTab.GetNextFileTabItem(); }
	CMDIChildWnd * GetPrevChildFrame() { return m_wndFileTab.GetPrevFileTabItem(); }

	CDocument * MDIGetActiveDocument();
	CView * MDIGetActiveView();

protected:
	void ActivateNextChildFrame();
	void ActivatePrevChildFrame();
	void ActivateChildFrameInZOrder(INT nIndex);
	void BringChildFrameToTop(CMDIChildWnd * pChild);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	virtual void OnSetPreviewMode(BOOL bPreview, CPrintPreviewState* pState);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewFileTab();
	afx_msg void OnUpdateViewFileTab(CCmdUI* pCmdUI);
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
	afx_msg void OnClose();
	afx_msg void OnViewDirectoryWindow();
	afx_msg void OnUpdateViewDirectoryWindow(CCmdUI* pCmdUI);
	afx_msg void OnViewProjectWindow();
	afx_msg void OnUpdateViewProjectWindow(CCmdUI* pCmdUI);
	afx_msg void OnViewOutputWindow();
	afx_msg void OnUpdateViewOutputWindow(CCmdUI* pCmdUI);
	afx_msg void OnWindowLastVisited();
	afx_msg void OnWindowNext();
	afx_msg void OnWindowPrev();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileWindowHide();
	afx_msg void OnFileWindowSync();
	afx_msg void OnOutputWindowHide();
	afx_msg void OnOutputWindowClear();
	afx_msg void OnOutputWindowCopyAll();
	afx_msg void OnOutputWindowCopy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__FFCA2B88_F9C5_11D4_A6F1_0050CE184C9B__INCLUDED_)

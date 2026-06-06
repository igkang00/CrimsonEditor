#if !defined(AFX_OUTPUTWINDOW_H__0217A261_BFCA_11D5_B61D_00E0298ED004__INCLUDED_)
#define AFX_OUTPUTWINDOW_H__0217A261_BFCA_11D5_B61D_00E0298ED004__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define OUTPUT_MAX_LINE_COUNT	1024


class COutputWindow : public CSizingControlBar
{
public: // Construction
	COutputWindow();

public: // general preferences
	static COutputPattern m_clsPattern[11];
	static CRegExp m_clsRegExp[11];
	static BOOL m_bLocalEcho;
	static INT m_nMaxLineCount;

	static COLORREF m_crBkgrColor;
	static COLORREF m_crTextColor;
	static COLORREF m_crGrayColor;
	static COLORREF m_crInputText;
	static COLORREF m_crErrorText;

protected: // Resources & Controls
	TBBUTTON m_tbiToolbarOutput[10];
	TBBUTTON m_tbiWinButtons[10];

	CImageList m_imgToolbarOutput;
	CImageList m_imgWinButtons;

	CToolBarCtrl m_btnToolbarOutput;
	CToolBarCtrl m_btnWinButtons;

	CVerticalStatic m_stcCaptionOutput;
	CFont m_fontControl, m_fontOutput;

	HACCEL m_hAccel;
	BOOL m_bOccupied;

protected: // output window controls
	CColorListBox m_lstConsoleOutput;
	CEdit m_edtConsoleInput;

// Operations
public:
	void ApplyOutputFont(BOOL bRedraw = TRUE);
	void EnableInputConsole(BOOL bEnable);

	void SetOccupied(BOOL bOccupy) { m_bOccupied = bOccupy; }
	BOOL CanUseNow() { return ! m_bOccupied; }

	BOOL AddStringToTheLast(LPCTSTR lpszString, COLORREF crTextColor);
	BOOL ReplaceTheLastString(LPCTSTR lpszString, COLORREF crTextColor);
	BOOL GetTheLastString(CString & szString);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputWindow)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COutputWindow();

	// Generated message map functions
protected:
	//{{AFX_MSG(COutputWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDblclkOutput();
	afx_msg void OnOutputWindowClear();
	afx_msg void OnOutputWindowCopyAll();
	afx_msg void OnOutputWindowCopy();
	afx_msg void OnOutputWindowSendInput();
	afx_msg void OnOutputWindowKillProcess();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTPUTWINDOW_H__0217A261_BFCA_11D5_B61D_00E0298ED004__INCLUDED_)

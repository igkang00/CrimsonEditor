#if !defined(AFX_CEDTSTATUSBAR_H__14946526_5B6B_11D3_978B_00104B151BA8__INCLUDED_)
#define AFX_CEDTSTATUSBAR_H__14946526_5B6B_11D3_978B_00104B151BA8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CedtStatusBar.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CCedtStatusBar window
class CStatusBarEx : public CStatusBar
{
// Construction
public:
	CStatusBarEx();

// Attributes
protected:
	CDC			m_dcPane;
	CBitmap		m_bmpPane;
	CFont		m_fontPane1, m_fontPane2;

	CString		m_szPaneText;

	CString		m_szSplashMessage;
	BOOL		m_bSplashMessage;

	COLORREF	m_crPaneTextColors[256];
	BOOL		m_bPaneHighlight[256];

// Operations
public:
	void BeginProgress(LPCTSTR lpszMessage);
	void EndProgress();
	void SetProgress(INT nPercent);

	void SetSplashMessage(UINT nID, UINT nElapse);
	void SetSplashMessage(LPCTSTR lpszMessage, UINT nElapse);

	void SetPaneTextColor(INT nIndex, COLORREF crTextColor);
	void SetPaneHighlight(INT nIndex, BOOL bHighlight);

protected:
	void DrawPaneBorder(CDC * pDC, CRect rect);
	void DrawPaneText(CDC * pDC, CRect rect, LPCTSTR lpszPaneText, COLORREF crTextColor, BOOL bAlignCenter);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatusBarEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStatusBarEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStatusBarEx)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CEDTSTATUSBAR_H__14946526_5B6B_11D3_978B_00104B151BA8__INCLUDED_)

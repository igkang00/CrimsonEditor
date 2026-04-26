#if !defined(AFX_GOTODIALOG_H__14C957E1_0B26_11D5_A6F1_0050CE184C9B__INCLUDED_)
#define AFX_GOTODIALOG_H__14C957E1_0B26_11D5_A6F1_0050CE184C9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GoToDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGoToDialog dialog

class CGoToDialog : public CDialog
{
// Construction
public:
	CGoToDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGoToDialog)
	enum { IDD = IDD_GO_TO_DIALOG };
	CString	m_szRange;
	UINT	m_nLineNumber;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGoToDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGoToDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GOTODIALOG_H__14C957E1_0B26_11D5_A6F1_0050CE184C9B__INCLUDED_)

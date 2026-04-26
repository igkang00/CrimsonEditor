#if !defined(AFX_ABOUTDIALOG_H__0FC28BC1_0C2C_11D5_A6F1_0050CE184C9B__INCLUDED_)
#define AFX_ABOUTDIALOG_H__0FC28BC1_0C2C_11D5_A6F1_0050CE184C9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AboutDialog.h : header file
//

#include "HyperLink.h"


/////////////////////////////////////////////////////////////////////////////
// CAboutDialog dialog

class CAboutDialog : public CDialog
{
// Construction
public:
	CAboutDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAboutDialog)
	enum { IDD = IDD_HELP_ABOUT };
	CStatic	m_stcLogo;
	CEdit	m_edtCredits;
	CEdit	m_edtCopyright;
	CButton		m_btnOK;
	CHyperLink	m_lnkHomePage;
	CHyperLink	m_lnkFeedBack;
	CHyperLink	m_lnkSponsoring;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAboutDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABOUTDIALOG_H__0FC28BC1_0C2C_11D5_A6F1_0050CE184C9B__INCLUDED_)

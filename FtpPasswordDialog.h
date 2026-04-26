#if !defined(AFX_FTPPASSWORDDIALOG_H__9B8BD1C1_C568_11D5_BC5E_0050CE184C9B__INCLUDED_)
#define AFX_FTPPASSWORDDIALOG_H__9B8BD1C1_C568_11D5_BC5E_0050CE184C9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FtpPasswordDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFtpPasswordDialog dialog

class CFtpPasswordDialog : public CDialog
{
// Construction
public:
	CFtpPasswordDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFtpPasswordDialog)
	enum { IDD = IDD_FTP_PASSWORD };
	CString	m_szPassword;
	CString	m_szAccountInfo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFtpPasswordDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFtpPasswordDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FTPPASSWORDDIALOG_H__9B8BD1C1_C568_11D5_BC5E_0050CE184C9B__INCLUDED_)

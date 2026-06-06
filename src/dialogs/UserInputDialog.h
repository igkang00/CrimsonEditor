#if !defined(AFX_USERINPUTDIALOG_H__202C0E83_BE35_11D5_B61D_00E0298ED004__INCLUDED_)
#define AFX_USERINPUTDIALOG_H__202C0E83_BE35_11D5_B61D_00E0298ED004__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UserInputDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUserInputDialog dialog

class CUserInputDialog : public CDialog
{
protected:
	CString m_szWindowTitle;

// Construction
public:
	CUserInputDialog(LPCTSTR lpszWindowTitle, CWnd* pParent = NULL);   // standard constructor
	CUserInputDialog(UINT nIDWindowTitle, CWnd* pParent = NULL);   // standard constructor 2

// Dialog Data
	//{{AFX_DATA(CUserInputDialog)
	enum { IDD = IDD_USER_INPUT };
	CString	m_szUserInput;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserInputDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUserInputDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERINPUTDIALOG_H__202C0E83_BE35_11D5_B61D_00E0298ED004__INCLUDED_)

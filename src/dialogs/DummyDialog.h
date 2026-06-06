#if !defined(AFX_DUMMYDIALOG_H__1D3D8314_1621_11D5_A6F1_0050CE184C9B__INCLUDED_)
#define AFX_DUMMYDIALOG_H__1D3D8314_1621_11D5_A6F1_0050CE184C9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DummyDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDummyDialog dialog

class CDummyDialog : public CDialog
{
// Construction
public:
	CDummyDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDummyDialog)
	enum { IDD = IDD_DUMMY };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDummyDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDummyDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DUMMYDIALOG_H__1D3D8314_1621_11D5_A6F1_0050CE184C9B__INCLUDED_)

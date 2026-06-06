#if !defined(AFX_RELOADASDIALOG_H__3B8C0FB3_C898_40FA_81C2_4499B263B754__INCLUDED_)
#define AFX_RELOADASDIALOG_H__3B8C0FB3_C898_40FA_81C2_4499B263B754__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReloadAsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReloadAsDialog dialog

class CReloadAsDialog : public CDialog
{
// Construction
public:
	CReloadAsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CReloadAsDialog)
	enum { IDD = IDD_RELOAD_AS };
	int		m_nEncodingType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReloadAsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CReloadAsDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RELOADASDIALOG_H__3B8C0FB3_C898_40FA_81C2_4499B263B754__INCLUDED_)

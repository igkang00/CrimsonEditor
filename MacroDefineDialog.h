#if !defined(AFX_MACRODEFINEDIALOG_H__2AB69422_0CFF_11D5_A6F1_0050CE184C9B__INCLUDED_)
#define AFX_MACRODEFINEDIALOG_H__2AB69422_0CFF_11D5_A6F1_0050CE184C9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MacroDefineDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMacroDefineDialog dialog

class CMacroDefineDialog : public CDialog
{
// Construction
public:
	CMacroDefineDialog(CWnd* pParent = NULL);   // standard constructor

public:
	CString m_szMacroName[11];
	INT m_nSelected;

	INT GetSelectedMacroNumber() { return m_nSelected; }

// Dialog Data
	//{{AFX_DATA(CMacroDefineDialog)
	enum { IDD = IDD_MACRO_DEFINE };
	CEdit	m_edtMacroName;
	CListCtrl	m_lstMacro;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMacroDefineDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMacroDefineDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnItemchangedMacroList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MACRODEFINEDIALOG_H__2AB69422_0CFF_11D5_A6F1_0050CE184C9B__INCLUDED_)

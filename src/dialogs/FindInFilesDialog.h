#if !defined(AFX_FINDINFILESDIALOG_H__7A275FC1_C038_11D5_BC5E_0050CE184C9B__INCLUDED_)
#define AFX_FINDINFILESDIALOG_H__7A275FC1_C038_11D5_BC5E_0050CE184C9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindInFilesDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindInFilesDialog dialog

class CFindInFilesDialog : public CDialog
{
protected:
	CImageList m_lstButtonImage;
	CStringList m_lstFindString;
	CStringList m_lstFileType;
	CStringList m_lstFolder;

	BOOL m_bLogFindSelection;
	INT  m_nFindSelBeg, m_nFindSelEnd;

public:
	CString m_szFindString;
	CString m_szFileType;
	CString m_szFolder;

public: // Construction
	CFindInFilesDialog(CWnd* pParent = NULL);   // standard constructor
	BOOL InitFileTypeList(LPCTSTR lpszComposedFilter);

protected:
	void ReplaceFindSelection(LPCTSTR lpszString, INT nIncrement);

public:
// Dialog Data
	//{{AFX_DATA(CFindInFilesDialog)
	enum { IDD = IDD_FIND_IN_FILES };
	CButton	m_btnFind;
	CButton	m_btnFindString;
	CComboBox	m_cmbFileType;
	CComboBox	m_cmbFolder;
	CComboBox	m_cmbFindString;
	BOOL	m_bRegularExpression;
	BOOL	m_bWholeWord;
	BOOL	m_bMatchCase;
	BOOL	m_bLookInSubfolders;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindInFilesDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFindInFilesDialog)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnFind();
	afx_msg void OnBrowse();
	afx_msg void OnSetfocusFindString();
	afx_msg void OnKillfocusFindString();
	afx_msg void OnFindStringMenu();
	afx_msg void OnFindTextTabChar();
	afx_msg void OnFindTextAnyChar();
	afx_msg void OnFindTextBeginOfLine();
	afx_msg void OnFindTextEndOfLine();
	afx_msg void OnFindTextZeroOrMore();
	afx_msg void OnFindTextOneOrMore();
	afx_msg void OnFindTextZeroOrOne();
	afx_msg void OnFindTextOr();
	afx_msg void OnFindTextInRange();
	afx_msg void OnFindTextNotInRange();
	afx_msg void OnFindTextWhiteSpace();
	afx_msg void OnFindTextAlphaChar();
	afx_msg void OnFindTextAlnumChar();
	afx_msg void OnFindTextDecDigit();
	afx_msg void OnFindTextHexDigit();
	afx_msg void OnFindTextTaggedExp();
	afx_msg void OnEditchangeFindString();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDINFILESDIALOG_H__7A275FC1_C038_11D5_BC5E_0050CE184C9B__INCLUDED_)

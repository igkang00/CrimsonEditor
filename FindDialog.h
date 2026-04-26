#if !defined(AFX_FINDDIALOG_H__9B993EA4_671A_11D3_978C_00104B151BA8__INCLUDED_)
#define AFX_FINDDIALOG_H__9B993EA4_671A_11D3_978C_00104B151BA8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FindDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindDialog dialog
class CFindDialog : public CDialog
{
protected:
	CImageList m_lstButtonImage;
	CStringList m_lstFindString;

	BOOL m_bLogFindSelection;
	INT  m_nFindSelBeg, m_nFindSelEnd;

public:
	CString m_szFindString;

public: // Construction
	CFindDialog(CWnd* pParent = NULL);   // standard constructor

protected:
	void ReplaceFindSelection(LPCTSTR lpszString, INT nIncrement);

public:
// Dialog Data
	//{{AFX_DATA(CFindDialog)
	enum { IDD = IDD_FIND_DIALOG };
	CButton	m_btnFind;
	CButton	m_btnFindString;
	CComboBox	m_cmbFindString;
	BOOL	m_bMatchCase;
	BOOL	m_bRegularExpression;
	BOOL	m_bWholeWord;
	int		m_nDirection;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFindDialog)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnFind();
	afx_msg void OnFindStringMenu();
	afx_msg void OnFindTextAlnumChar();
	afx_msg void OnFindTextAlphaChar();
	afx_msg void OnFindTextAnyChar();
	afx_msg void OnFindTextBeginOfLine();
	afx_msg void OnFindTextDecDigit();
	afx_msg void OnFindTextEndOfLine();
	afx_msg void OnFindTextHexDigit();
	afx_msg void OnFindTextInRange();
	afx_msg void OnFindTextNotInRange();
	afx_msg void OnFindTextWhiteSpace();
	afx_msg void OnFindTextOneOrMore();
	afx_msg void OnFindTextOr();
	afx_msg void OnFindTextTabChar();
	afx_msg void OnFindTextTaggedExp();
	afx_msg void OnFindTextZeroOrMore();
	afx_msg void OnFindTextZeroOrOne();
	afx_msg void OnSetfocusFindString();
	afx_msg void OnKillfocusFindString();
	afx_msg void OnEditchangeFindString();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDDIALOG_H__9B993EA4_671A_11D3_978C_00104B151BA8__INCLUDED_)

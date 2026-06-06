#if !defined(AFX_REPLACEDIALOG_H__9B993EA5_671A_11D3_978C_00104B151BA8__INCLUDED_)
#define AFX_REPLACEDIALOG_H__9B993EA5_671A_11D3_978C_00104B151BA8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ReplaceDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReplaceDialog dialog

class CReplaceDialog : public CDialog
{
protected:
	CImageList m_lstButtonImage;
	CStringList m_lstFindString;
	CStringList m_lstReplaceString;

	BOOL m_bLogFindSelection;
	INT  m_nFindSelBeg, m_nFindSelEnd;

	BOOL m_bLogReplaceSelection;
	INT  m_nReplaceSelBeg, m_nReplaceSelEnd;

public:
	CString m_szFindString;
	CString m_szReplaceString;
	BOOL	m_bReplaceAll;

public: // Construction
	CReplaceDialog(CWnd* pParent = NULL);   // standard constructor

protected:
	void ReplaceFindSelection(LPCTSTR lpszString, INT nIncrement);
	void ReplaceReplaceSelection(LPCTSTR lpszString, INT nIncrement);

public:
// Dialog Data
	//{{AFX_DATA(CReplaceDialog)
	enum { IDD = IDD_REPLACE_DIALOG };
	CButton	m_btnReplaceAll;
	CButton	m_btnFind;
	CButton	m_btnReplaceString;
	CButton	m_btnFindString;
	CComboBox	m_cmbReplaceString;
	CComboBox	m_cmbFindString;
	BOOL	m_bWholeWord;
	BOOL	m_bRegularExpression;
	BOOL	m_bMatchCase;
	int		m_nRange;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReplaceDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CReplaceDialog)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnFind();
	afx_msg void OnReplaceAll();
	afx_msg void OnSetfocusFindString();
	afx_msg void OnKillfocusFindString();
	afx_msg void OnSetfocusReplaceString();
	afx_msg void OnKillfocusReplaceString();
	afx_msg void OnFindStringMenu();
	afx_msg void OnReplaceStringMenu();
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
	afx_msg void OnReplaceTextTabChar();
	afx_msg void OnReplaceTextTaggedExp0();
	afx_msg void OnReplaceTextTaggedExp1();
	afx_msg void OnReplaceTextTaggedExp2();
	afx_msg void OnReplaceTextTaggedExp3();
	afx_msg void OnReplaceTextTaggedExp4();
	afx_msg void OnReplaceTextTaggedExp5();
	afx_msg void OnReplaceTextTaggedExp6();
	afx_msg void OnReplaceTextTaggedExp7();
	afx_msg void OnReplaceTextTaggedExp8();
	afx_msg void OnReplaceTextTaggedExp9();
	afx_msg void OnEditchangeFindString();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPLACEDIALOG_H__9B993EA5_671A_11D3_978C_00104B151BA8__INCLUDED_)

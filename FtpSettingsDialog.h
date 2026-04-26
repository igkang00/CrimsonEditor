#if !defined(AFX_FTPSETTINGSDIALOG_H__63818BA5_2C14_11D5_A6F1_0050CE184C9B__INCLUDED_)
#define AFX_FTPSETTINGSDIALOG_H__63818BA5_2C14_11D5_A6F1_0050CE184C9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FtpSettingsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFtpSettingsDialog dialog

class CFtpSettingsDialog : public CDialog
{
// Construction
public:
	CFtpSettingsDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFtpSettingsDialog();

// Dialog Data
	//{{AFX_DATA(CFtpSettingsDialog)
	enum { IDD = IDD_FTP_SETTINGS };
	CEdit	m_edtDescription;
	CButton	m_chkSavePassword;
	CEdit	m_edtDirectory;
	CEdit	m_edtPassword;
	CEdit	m_edtUserName;
	CEdit	m_edtFtpServer;
	CListCtrl	m_lstFtpAccounts;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFtpSettingsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	INT m_nAccountCount;
	CFtpAccount * m_pFtpAccounts;

public:
	void SetFtpAccounts(INT nCount, CFtpAccount * pFtpAccounts);
	void GetFtpAccounts(INT nCount, CFtpAccount * pFtpAccounts);

protected:
	CImageList m_imgButtons;
	INT m_nCurrentAccount;

	// Generated message map functions
	//{{AFX_MSG(CFtpSettingsDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedFtpAccounts(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeFtpServer();
	afx_msg void OnChangeUserName();
	afx_msg void OnChangePassword();
	afx_msg void OnChangeDirectory();
	afx_msg void OnRemove();
	afx_msg void OnMoveUp();
	afx_msg void OnMoveDown();
	afx_msg void OnSavePassword();
	afx_msg void OnAdvanced();
	afx_msg void OnChangeDescription();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FTPSETTINGSDIALOG_H__63818BA5_2C14_11D5_A6F1_0050CE184C9B__INCLUDED_)

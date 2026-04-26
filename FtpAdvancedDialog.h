#if !defined(AFX_FTPADVANCEDDIALOG_H__4951CB84_B758_11D5_BC5E_0050CE184C9B__INCLUDED_)
#define AFX_FTPADVANCEDDIALOG_H__4951CB84_B758_11D5_BC5E_0050CE184C9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FtpAdvancedDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFtpAdvancedDialog dialog

class CFtpAdvancedDialog : public CDialog
{
// Construction
public:
	CFtpAdvancedDialog(CWnd* pParent = NULL);   // standard constructor

public:
	BOOL m_bPassiveMode;
	BOOL m_bBinaryType;
	BOOL m_bUseWinInet;

	INT  m_nServerType;
	INT  m_nPortNumber;

// Dialog Data
	//{{AFX_DATA(CFtpAdvancedDialog)
	enum { IDD = IDD_FTP_ADVANCED };
	CButton	m_btnTransferType2;
	CButton	m_btnTransferType1;
	CButton	m_chkDefaultPort;
	CEdit	m_edtPortNumber;
	CButton	m_chkPassiveMode;
	CButton	m_chkUseWinInet;
	CComboBox	m_cmbServerType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFtpAdvancedDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFtpAdvancedDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeServerType();
	afx_msg void OnUseWininet();
	afx_msg void OnPassiveMode();
	afx_msg void OnChangePortNumber();
	afx_msg void OnDefaultPort();
	afx_msg void OnTransferType1();
	afx_msg void OnTransferType2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FTPADVANCEDDIALOG_H__4951CB84_B758_11D5_BC5E_0050CE184C9B__INCLUDED_)

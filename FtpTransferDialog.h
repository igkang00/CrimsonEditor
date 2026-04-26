#if !defined(AFX_FTPTRANSFERDIALOG_H__C979611F_0310_4951_B9CF_F82BD3DE99F1__INCLUDED_)
#define AFX_FTPTRANSFERDIALOG_H__C979611F_0310_4951_B9CF_F82BD3DE99F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FtpTransferDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFtpTransferDialog dialog

class CFtpTransferDialog : public CDialog
{
// Construction
public:
	CFtpTransferDialog(BOOL bDownload, CFtpAccount & rAccount, LPCTSTR lpszRemoteFile, LPCTSTR lpszLocalFile, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFtpTransferDialog)
	enum { IDD = IDD_FTP_TRANSFER };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFtpTransferDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	static BOOL m_bDownload;
	static CFtpAccount m_clsFtpAccount;
	static CString m_szRemoteFile;
	static CString m_szLocalFile;

	static BOOL m_bTransferCanceledByUser;
	static BOOL m_bTransferCompleted;
	static CFtpTransferDialog * m_pActiveDialog;
	static CProgressCtrl * m_pActiveProgressCtrl;
	static CWinThread * m_pRunningWinThread;

public:
	static BOOL IsTransferCompleted() { return m_bTransferCompleted; }

protected:
	static UINT TransferThreadMain(LPVOID pParam);
	static BOOL TransferProgress(UINT nPercentage);

protected:
	// Generated message map functions
	//{{AFX_MSG(CFtpTransferDialog)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FTPTRANSFERDIALOG_H__C979611F_0310_4951_B9CF_F82BD3DE99F1__INCLUDED_)

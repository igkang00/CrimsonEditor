#if !defined(AFX_OPENREMOTEDIALOG_H__1C953025_26AB_11D5_A6F1_0050CE184C9B__INCLUDED_)
#define AFX_OPENREMOTEDIALOG_H__1C953025_26AB_11D5_A6F1_0050CE184C9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OpenRemoteDialog.h : header file
//

#include <afxinet.h>

#define REMOTE_ITEM_PARENT		0x00
#define REMOTE_ITEM_LINK		0x01
#define REMOTE_ITEM_DIRECTORY	0x02
#define REMOTE_ITEM_FILE		0x03

/////////////////////////////////////////////////////////////////////////////
// COpenRemoteDialog dialog

class COpenRemoteDialog : public CDialog
{
// Construction
public:
	COpenRemoteDialog(BOOL bOpenFileDialog, LPCTSTR lpszFileName, LPCTSTR lpszFilter, CWnd* pParent = NULL);   // standard constructor
	virtual ~COpenRemoteDialog();

// Dialog Data
	//{{AFX_DATA(COpenRemoteDialog)
	enum { IDD = IDD_OPEN_REMOTE };
	CButton	m_btnFtpSettings;
	CButton	m_btnOK;
	CComboBox	m_cmbFileType;
	CEdit	m_edtFileName;
	CButton	m_btnParentDirectory;
	CButton	m_btnCreateDirectory;
	CEdit	m_edtDirectory;
	CListCtrl	m_lstFiles;
	CComboBox	m_cmbFtpAccount;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenRemoteDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_bOpenFileDialog;
	CString m_szFileName;
	CString m_szFileFilter;

	INT m_nAccountCount;
	CFtpAccount * m_pFtpAccounts;

	CImageList m_imgButtons;
	CImageList m_imgFileList;

	CStringArray m_arrFilterDescription;
	CStringArray m_arrFilterExtensions;

	// store current ftp account & filter index
	INT m_nCurrentFtpAccount;
	INT m_nCurrentFilterIndex;

	// store selected path names
	CStringList  m_lstSelectedFileInfo;

public:
	void SetFtpAccounts(INT nCount, CFtpAccount * pFtpAccounts);
	void GetFtpAccounts(INT nCount, CFtpAccount * pFtpAccounts);

	void SetCurrentFtpAccount(INT nFtpAccount) { m_nCurrentFtpAccount = nFtpAccount; }
	INT  GetCurrentFtpAccount() { return m_nCurrentFtpAccount; }

	void SetCurrentFilterIndex(INT nFilterIndex) { m_nCurrentFilterIndex = nFilterIndex; }
	INT  GetCurrentFilterIndex() { return m_nCurrentFilterIndex; }

	POSITION GetFirstFileInfoPosition() { return m_lstSelectedFileInfo.GetHeadPosition(); }
	CString  GetNextFileInfo(POSITION & pos) { return m_lstSelectedFileInfo.GetNext(pos); }

	CString GetPathName();
	DWORD   GetFileSize();

protected:
	void InitFtpAccounts();
	void InitFileFilters();

	void ChangeDirectory(LPCTSTR lpszDirectory);
	BOOL RefreshFileList();

protected:
	// Generated message map functions
	//{{AFX_MSG(COpenRemoteDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnFtpSettings();
	afx_msg void OnConnect();
	afx_msg void OnDblclkFileList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedFileList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeFileType();
	virtual void OnOK();
	afx_msg void OnCreateDirectory();
	afx_msg void OnParentDirectory();
	afx_msg void OnSelchangeFtpAccount();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPENREMOTEDIALOG_H__1C953025_26AB_11D5_A6F1_0050CE184C9B__INCLUDED_)

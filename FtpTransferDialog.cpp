// FtpTransferDialog.cpp : implementation file
//

#include "stdafx.h"
#include "cedtHeader.h"
#include "RemoteFile.h"
#include "FtpTransferDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BOOL CFtpTransferDialog::m_bDownload = TRUE;
CFtpAccount CFtpTransferDialog::m_clsFtpAccount;
CString CFtpTransferDialog::m_szRemoteFile;
CString CFtpTransferDialog::m_szLocalFile;

BOOL CFtpTransferDialog::m_bTransferCanceledByUser = FALSE;
BOOL CFtpTransferDialog::m_bTransferCompleted = FALSE;
CFtpTransferDialog * CFtpTransferDialog::m_pActiveDialog = NULL;
CProgressCtrl * CFtpTransferDialog::m_pActiveProgressCtrl = NULL;
CWinThread * CFtpTransferDialog::m_pRunningWinThread = NULL;

/////////////////////////////////////////////////////////////////////////////
// CFtpTransferDialog dialog

CFtpTransferDialog::CFtpTransferDialog(BOOL bDownload, CFtpAccount & rAccount, LPCTSTR lpszRemoteFile, LPCTSTR lpszLocalFile, CWnd* pParent /*=NULL*/)
	: CDialog(CFtpTransferDialog::IDD, pParent)
{
	m_bDownload = bDownload;
	m_clsFtpAccount.CopyContents( rAccount );
	m_szRemoteFile = lpszRemoteFile;
	m_szLocalFile = lpszLocalFile;

	m_bTransferCanceledByUser = FALSE;
	m_bTransferCompleted = FALSE;
	m_pActiveDialog = NULL;
	m_pActiveProgressCtrl = NULL;
	m_pRunningWinThread = NULL;

	//{{AFX_DATA_INIT(CFtpTransferDialog)
	//}}AFX_DATA_INIT
}


void CFtpTransferDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFtpTransferDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFtpTransferDialog, CDialog)
	//{{AFX_MSG_MAP(CFtpTransferDialog)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFtpTransferDialog message handlers

BOOL CFtpTransferDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_bTransferCanceledByUser = FALSE;
	m_bTransferCompleted = FALSE;

	m_pActiveDialog = this;

	m_pActiveProgressCtrl = (CProgressCtrl *)GetDlgItem(IDC_PROGRESS);
	m_pActiveProgressCtrl->SetRange(0, 100);

	m_pRunningWinThread = AfxBeginThread(TransferThreadMain, NULL);

	return TRUE;
}

void CFtpTransferDialog::OnCancel() 
{
	m_bTransferCanceledByUser = TRUE;
	CDialog::OnCancel();
}

void CFtpTransferDialog::OnDestroy() 
{
	m_pActiveDialog = NULL;
	m_pActiveProgressCtrl = NULL;
	CDialog::OnDestroy();
}

UINT CFtpTransferDialog::TransferThreadMain(LPVOID pParam)
{
	if( m_bDownload ) {
		m_bTransferCompleted = DownloadRemoteFile(m_clsFtpAccount, m_szRemoteFile, m_szLocalFile, TransferProgress);
	} else {
		m_bTransferCompleted = UploadLocalFile(m_clsFtpAccount, m_szLocalFile, m_szRemoteFile, TransferProgress);
	}

	// close dialog box
	if( m_pActiveDialog ) m_pActiveDialog->PostMessage(WM_COMMAND, IDOK, 0L);

	return 0;
}

BOOL CFtpTransferDialog::TransferProgress(UINT nPercentage)
{
	ASSURE_BOUND_VALUE(nPercentage, 0U, 100U)
	if( m_pActiveProgressCtrl ) m_pActiveProgressCtrl->SetPos(nPercentage);
	return ! m_bTransferCanceledByUser;
}

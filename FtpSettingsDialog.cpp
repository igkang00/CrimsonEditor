// FtpSettingsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include <fstream.h>
#include "cedtElement.h"
#include "FtpSettingsDialog.h"
#include "FtpAdvancedDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFtpSettingsDialog dialog


CFtpSettingsDialog::CFtpSettingsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFtpSettingsDialog::IDD, pParent)
{
	m_nAccountCount = 0;
	m_pFtpAccounts = NULL;

	//{{AFX_DATA_INIT(CFtpSettingsDialog)
	//}}AFX_DATA_INIT
}

CFtpSettingsDialog::~CFtpSettingsDialog()
{
	delete [] m_pFtpAccounts;
	m_pFtpAccounts = NULL;
}

void CFtpSettingsDialog::SetFtpAccounts(INT nCount, CFtpAccount * pFtpAccounts)
{
	m_nAccountCount = nCount;

	delete [] m_pFtpAccounts;
	m_pFtpAccounts = new CFtpAccount[nCount];

	for(INT i = 0; i < nCount; i++) {
		m_pFtpAccounts[i].CopyContents( pFtpAccounts[i] );
	}
}

void CFtpSettingsDialog::GetFtpAccounts(INT nCount, CFtpAccount * pFtpAccounts)
{
	if( nCount > m_nAccountCount ) nCount = m_nAccountCount;

	for(INT i = 0; i < nCount; i++) {
		pFtpAccounts[i].CopyContents( m_pFtpAccounts[i] );
	}
}

void CFtpSettingsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFtpSettingsDialog)
	DDX_Control(pDX, IDC_DESCRIPTION, m_edtDescription);
	DDX_Control(pDX, IDC_SAVE_PASSWORD, m_chkSavePassword);
	DDX_Control(pDX, IDC_DIRECTORY, m_edtDirectory);
	DDX_Control(pDX, IDC_PASSWORD, m_edtPassword);
	DDX_Control(pDX, IDC_USER_NAME, m_edtUserName);
	DDX_Control(pDX, IDC_FTP_SERVER, m_edtFtpServer);
	DDX_Control(pDX, IDC_FTP_ACCOUNTS, m_lstFtpAccounts);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFtpSettingsDialog, CDialog)
	//{{AFX_MSG_MAP(CFtpSettingsDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FTP_ACCOUNTS, OnItemchangedFtpAccounts)
	ON_EN_CHANGE(IDC_FTP_SERVER, OnChangeFtpServer)
	ON_EN_CHANGE(IDC_USER_NAME, OnChangeUserName)
	ON_EN_CHANGE(IDC_PASSWORD, OnChangePassword)
	ON_EN_CHANGE(IDC_DIRECTORY, OnChangeDirectory)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_MOVE_UP, OnMoveUp)
	ON_BN_CLICKED(IDC_MOVE_DOWN, OnMoveDown)
	ON_BN_CLICKED(IDC_SAVE_PASSWORD, OnSavePassword)
	ON_BN_CLICKED(IDC_ADVANCED, OnAdvanced)
	ON_EN_CHANGE(IDC_DESCRIPTION, OnChangeDescription)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFtpSettingsDialog message handlers

BOOL CFtpSettingsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_imgButtons.Create(IDB_GENERAL_BUTTONS, 9, 0, RGB(255, 0, 255));

	m_lstFtpAccounts.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_lstFtpAccounts.InsertColumn(0, "FTP Accounts", LVCFMT_LEFT, 268);
	
	CButton * pButton0 = (CButton *)GetDlgItem(IDC_MOVE_UP);
	pButton0->SetIcon( m_imgButtons.ExtractIcon(0) );
	CButton * pButton1 = (CButton *)GetDlgItem(IDC_MOVE_DOWN);
	pButton1->SetIcon( m_imgButtons.ExtractIcon(1) );

	for(INT i = 0; i < m_nAccountCount; i++) m_lstFtpAccounts.InsertItem( i, m_pFtpAccounts[i].GetDisplayName() );
	m_lstFtpAccounts.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);

	return TRUE;
}

void CFtpSettingsDialog::OnItemchangedFtpAccounts(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if( (pNMListView->iItem >= 0) && (pNMListView->uNewState & LVIS_SELECTED) ) {
		INT nAccount = pNMListView->iItem; m_nCurrentAccount = nAccount;

		m_edtDescription.SetWindowText( m_pFtpAccounts[nAccount].m_szDescription );
		m_edtFtpServer.SetWindowText( m_pFtpAccounts[nAccount].m_szServerName );
		m_edtUserName.SetWindowText( m_pFtpAccounts[nAccount].m_szUserName );
		m_edtPassword.SetWindowText( m_pFtpAccounts[nAccount].m_szPassword );
		m_edtDirectory.SetWindowText( m_pFtpAccounts[nAccount].m_szSubDirectory );

		m_chkSavePassword.SetCheck( m_pFtpAccounts[nAccount].m_bSavePassword );
	}
	
	*pResult = 0;
}

void CFtpSettingsDialog::OnChangeDescription() 
{
	INT nAccount = m_nCurrentAccount; CString szText; 
	m_edtDescription.GetWindowText( szText ); szText.TrimLeft(); szText.TrimRight();
	m_pFtpAccounts[nAccount].m_szDescription = szText;
	m_lstFtpAccounts.SetItemText( nAccount, 0, m_pFtpAccounts[nAccount].GetDisplayName() );
}

void CFtpSettingsDialog::OnChangeFtpServer() 
{
	INT nAccount = m_nCurrentAccount; CString szText;
	m_edtFtpServer.GetWindowText( szText ); szText.TrimLeft(); szText.TrimRight();
	m_pFtpAccounts[nAccount].m_szServerName = szText;
	m_lstFtpAccounts.SetItemText( nAccount, 0, m_pFtpAccounts[nAccount].GetDisplayName() );
}

void CFtpSettingsDialog::OnChangeUserName() 
{
	INT nAccount = m_nCurrentAccount; CString szText;
	m_edtUserName.GetWindowText( szText ); szText.TrimLeft(); szText.TrimRight();
	m_pFtpAccounts[nAccount].m_szUserName = szText;
	m_lstFtpAccounts.SetItemText( nAccount, 0, m_pFtpAccounts[nAccount].GetDisplayName() );
}

void CFtpSettingsDialog::OnChangePassword() 
{
	INT nAccount = m_nCurrentAccount;
	m_edtPassword.GetWindowText( m_pFtpAccounts[nAccount].m_szPassword );
}

void CFtpSettingsDialog::OnSavePassword() 
{
	INT nAccount = m_nCurrentAccount;
	m_pFtpAccounts[nAccount].m_bSavePassword = m_chkSavePassword.GetCheck();
}

void CFtpSettingsDialog::OnChangeDirectory() 
{
	INT nAccount = m_nCurrentAccount; CString szText;
	m_edtDirectory.GetWindowText( szText ); szText.TrimLeft(); szText.TrimRight();
	m_pFtpAccounts[nAccount].m_szSubDirectory = szText;
}

void CFtpSettingsDialog::OnRemove() 
{
	INT nAccount = m_nCurrentAccount;
	m_pFtpAccounts[nAccount].DeleteContents();
	m_lstFtpAccounts.SetItemText(nAccount, 0, m_pFtpAccounts[nAccount].GetDisplayName() );

	m_edtDescription.SetWindowText( m_pFtpAccounts[nAccount].m_szDescription );
	m_edtFtpServer.SetWindowText( m_pFtpAccounts[nAccount].m_szServerName );
	m_edtUserName.SetWindowText( m_pFtpAccounts[nAccount].m_szUserName );
	m_edtPassword.SetWindowText( m_pFtpAccounts[nAccount].m_szPassword );
	m_edtDirectory.SetWindowText( m_pFtpAccounts[nAccount].m_szSubDirectory );

	m_chkSavePassword.SetCheck( m_pFtpAccounts[nAccount].m_bSavePassword );
}

void CFtpSettingsDialog::OnMoveUp() 
{
	INT nAccount = m_nCurrentAccount;
	if( nAccount > 0 ) {
		CFtpAccount clsFtpAccount; clsFtpAccount.CopyContents( m_pFtpAccounts[nAccount] );
		m_pFtpAccounts[nAccount+0].CopyContents( m_pFtpAccounts[nAccount-1] );
		m_pFtpAccounts[nAccount-1].CopyContents( clsFtpAccount );

		m_lstFtpAccounts.SetItemText(nAccount-1, 0, m_pFtpAccounts[nAccount-1].GetDisplayName() );
		m_lstFtpAccounts.SetItemText(nAccount+0, 0, m_pFtpAccounts[nAccount+0].GetDisplayName() );

		m_lstFtpAccounts.SetItemState( nAccount-1, LVIS_SELECTED, LVIS_SELECTED );
		m_lstFtpAccounts.EnsureVisible( nAccount-1, FALSE );
	}
}

void CFtpSettingsDialog::OnMoveDown() 
{
	INT nAccount = m_nCurrentAccount;
	if( nAccount < 9 ) {
		CFtpAccount clsFtpAccount; clsFtpAccount.CopyContents( m_pFtpAccounts[nAccount] );
		m_pFtpAccounts[nAccount+0].CopyContents( m_pFtpAccounts[nAccount+1] );
		m_pFtpAccounts[nAccount+1].CopyContents( clsFtpAccount );

		m_lstFtpAccounts.SetItemText(nAccount+0, 0, m_pFtpAccounts[nAccount+0].GetDisplayName() );
		m_lstFtpAccounts.SetItemText(nAccount+1, 0, m_pFtpAccounts[nAccount+1].GetDisplayName() );

		m_lstFtpAccounts.SetItemState( nAccount+1, LVIS_SELECTED, LVIS_SELECTED );
		m_lstFtpAccounts.EnsureVisible( nAccount+1, FALSE );
	}
}

void CFtpSettingsDialog::OnAdvanced() 
{
	INT nAccount = m_nCurrentAccount;
	CFtpAdvancedDialog dlg;

	dlg.m_bPassiveMode = m_pFtpAccounts[nAccount].m_bPassiveMode;
	dlg.m_bUseWinInet = m_pFtpAccounts[nAccount].m_bUseWinInet;
	dlg.m_bBinaryType = m_pFtpAccounts[nAccount].m_bBinaryType;
	dlg.m_nServerType = m_pFtpAccounts[nAccount].m_nServerType;
	dlg.m_nPortNumber = m_pFtpAccounts[nAccount].m_nPortNumber;

	if( dlg.DoModal() == IDOK ) {
		m_pFtpAccounts[nAccount].m_bPassiveMode = dlg.m_bPassiveMode;
		m_pFtpAccounts[nAccount].m_bUseWinInet = dlg.m_bUseWinInet;
		m_pFtpAccounts[nAccount].m_bBinaryType = dlg.m_bBinaryType;
		m_pFtpAccounts[nAccount].m_nServerType = dlg.m_nServerType;
		m_pFtpAccounts[nAccount].m_nPortNumber = dlg.m_nPortNumber;
	}
}

// FtpAdvancedDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include <fstream.h>
#include "cedtElement.h"
#include "FtpAdvancedDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFtpAdvancedDialog dialog


CFtpAdvancedDialog::CFtpAdvancedDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFtpAdvancedDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFtpAdvancedDialog)
	//}}AFX_DATA_INIT
}


void CFtpAdvancedDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFtpAdvancedDialog)
	DDX_Control(pDX, IDC_TRANSFER_TYPE2, m_btnTransferType2);
	DDX_Control(pDX, IDC_TRANSFER_TYPE1, m_btnTransferType1);
	DDX_Control(pDX, IDC_DEFAULT_PORT, m_chkDefaultPort);
	DDX_Control(pDX, IDC_PORT_NUMBER, m_edtPortNumber);
	DDX_Control(pDX, IDC_PASSIVE_MODE, m_chkPassiveMode);
	DDX_Control(pDX, IDC_USE_WININET, m_chkUseWinInet);
	DDX_Control(pDX, IDC_SERVER_TYPE, m_cmbServerType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFtpAdvancedDialog, CDialog)
	//{{AFX_MSG_MAP(CFtpAdvancedDialog)
	ON_CBN_SELCHANGE(IDC_SERVER_TYPE, OnSelchangeServerType)
	ON_BN_CLICKED(IDC_USE_WININET, OnUseWininet)
	ON_BN_CLICKED(IDC_PASSIVE_MODE, OnPassiveMode)
	ON_EN_CHANGE(IDC_PORT_NUMBER, OnChangePortNumber)
	ON_BN_CLICKED(IDC_DEFAULT_PORT, OnDefaultPort)
	ON_BN_CLICKED(IDC_TRANSFER_TYPE1, OnTransferType1)
	ON_BN_CLICKED(IDC_TRANSFER_TYPE2, OnTransferType2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFtpAdvancedDialog message handlers

BOOL CFtpAdvancedDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_cmbServerType.InsertString(0, "Generic FTP Server");
//	m_cmbServerType.InsertString(1, "Hewlett-Packard Server");
//	m_cmbServerType.InsertString(2, "Sun Microsystems Server");

	m_cmbServerType.SetCurSel( m_nServerType );
	m_cmbServerType.EnableWindow( ! m_bUseWinInet );

	m_chkUseWinInet.SetCheck( m_bUseWinInet );
	m_chkPassiveMode.SetCheck( m_bPassiveMode );

	CString szPortNumber; szPortNumber.Format("%d", m_nPortNumber );
	m_edtPortNumber.SetWindowText( szPortNumber );

	BOOL bDefaultPort = ( m_nPortNumber == 21 );
	m_edtPortNumber.EnableWindow( ! bDefaultPort );
	m_chkDefaultPort.SetCheck( bDefaultPort );

	m_btnTransferType1.SetCheck( ! m_bBinaryType );
	m_btnTransferType2.SetCheck( m_bBinaryType );

	return TRUE;
}

void CFtpAdvancedDialog::OnSelchangeServerType() 
{
	m_nServerType = m_cmbServerType.GetCurSel();
}

void CFtpAdvancedDialog::OnUseWininet() 
{
	m_bUseWinInet = m_chkUseWinInet.GetCheck();
	if( m_bUseWinInet ) m_cmbServerType.SetCurSel( m_nServerType = 0 );
	m_cmbServerType.EnableWindow( ! m_bUseWinInet );
}

void CFtpAdvancedDialog::OnPassiveMode() 
{
	m_bPassiveMode = m_chkPassiveMode.GetCheck();
}

void CFtpAdvancedDialog::OnChangePortNumber() 
{
	CString szPortNumber; m_edtPortNumber.GetWindowText( szPortNumber );
	m_nPortNumber = atoi( szPortNumber );
}

void CFtpAdvancedDialog::OnDefaultPort() 
{
	BOOL bDefaultPort = m_chkDefaultPort.GetCheck();
	if( bDefaultPort ) { m_nPortNumber = 21; m_edtPortNumber.SetWindowText( "21" ); }
	m_edtPortNumber.EnableWindow( ! bDefaultPort );
}

void CFtpAdvancedDialog::OnTransferType1() 
{
	m_bBinaryType = ! m_btnTransferType1.GetCheck();
	m_btnTransferType2.SetCheck( m_bBinaryType );
}

void CFtpAdvancedDialog::OnTransferType2() 
{
	m_bBinaryType = m_btnTransferType2.GetCheck();
	m_btnTransferType1.SetCheck( ! m_bBinaryType );
}

// FtpPasswordDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "FtpPasswordDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFtpPasswordDialog dialog


CFtpPasswordDialog::CFtpPasswordDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFtpPasswordDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFtpPasswordDialog)
	m_szPassword = _T("");
	m_szAccountInfo = _T("");
	//}}AFX_DATA_INIT
}


void CFtpPasswordDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFtpPasswordDialog)
	DDX_Text(pDX, IDC_PASSWORD, m_szPassword);
	DDX_Text(pDX, IDC_ACCOUNT_INFO, m_szAccountInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFtpPasswordDialog, CDialog)
	//{{AFX_MSG_MAP(CFtpPasswordDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFtpPasswordDialog message handlers

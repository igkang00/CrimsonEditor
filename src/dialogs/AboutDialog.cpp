// AboutDialog.cpp : implementation file
//

#include "stdafx.h"
#include "cedtHeader.h"
#include "AboutDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDialog dialog


CAboutDialog::CAboutDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAboutDialog)
	//}}AFX_DATA_INIT
}


void CAboutDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDialog)
	DDX_Control(pDX, IDC_SPONSORING, m_lnkSponsoring);
	DDX_Control(pDX, IDC_LOGO, m_stcLogo);
	DDX_Control(pDX, IDC_CREDITS, m_edtCredits);
	DDX_Control(pDX, IDC_COPYRIGHT, m_edtCopyright);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_HOMEPAGE, m_lnkHomePage);
	DDX_Control(pDX, IDC_FEEDBACK, m_lnkFeedBack);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAboutDialog, CDialog)
	//{{AFX_MSG_MAP(CAboutDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAboutDialog message handlers

BOOL CAboutDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect rect; GetClientRect( & rect );
	m_stcLogo.MoveWindow( rect.right - 236 - 14 - 2, 14 + 2, 236 + 2, 31 + 2);

	CString szCopyright; szCopyright.LoadString(IDS_TEXT_COPYRIGHT);
	m_edtCopyright.SetWindowText(szCopyright);

	CString szCredits; szCredits.LoadString(IDS_TEXT_CREDITS);
	m_edtCredits.SetWindowText(szCredits);

	CHyperLink::SetColors(RGB(0, 0, 128), RGB(0, 0, 128), RGB(0, 0, 128), RGB(0, 0, 255));

	m_lnkHomePage.SetURL(STRING_HOMEPAGEURL);
	m_lnkHomePage.ModifyLinkStyle(0, CHyperLink::StyleUseHover);

	m_lnkFeedBack.SetURL(STRING_EMAILADDRESS);
	m_lnkFeedBack.ModifyLinkStyle(0, CHyperLink::StyleUseHover);
	
	m_lnkSponsoring.SetURL(STRING_SPONSORURL);
	m_lnkSponsoring.ModifyLinkStyle(0, CHyperLink::StyleUseHover);

	return TRUE;
}

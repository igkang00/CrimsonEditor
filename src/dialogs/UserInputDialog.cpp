// UserInputDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "UserInputDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUserInputDialog dialog


CUserInputDialog::CUserInputDialog(LPCTSTR lpszWindowTitle, CWnd* pParent /*=NULL*/)
	: CDialog(CUserInputDialog::IDD, pParent)
{
	m_szWindowTitle = lpszWindowTitle;

	//{{AFX_DATA_INIT(CUserInputDialog)
	m_szUserInput = _T("");
	//}}AFX_DATA_INIT
}


CUserInputDialog::CUserInputDialog(UINT nIDWindowTitle, CWnd* pParent /*=NULL*/)
	: CDialog(CUserInputDialog::IDD, pParent)
{
	m_szWindowTitle.LoadString(nIDWindowTitle);

	//{{AFX_DATA_INIT(CUserInputDialog)
	m_szUserInput = _T("");
	//}}AFX_DATA_INIT
}


void CUserInputDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserInputDialog)
	DDX_Text(pDX, IDC_USER_INPUT, m_szUserInput);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserInputDialog, CDialog)
	//{{AFX_MSG_MAP(CUserInputDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserInputDialog message handlers

BOOL CUserInputDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText( m_szWindowTitle );
	
	return TRUE;
}

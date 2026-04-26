// GoToDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "GoToDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGoToDialog dialog


CGoToDialog::CGoToDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CGoToDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGoToDialog)
	m_szRange = _T("");
	m_nLineNumber = 0;
	//}}AFX_DATA_INIT
}


void CGoToDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGoToDialog)
	DDX_Text(pDX, IDC_RANGE, m_szRange);
	DDX_Text(pDX, IDC_LINE_NUMBER, m_nLineNumber);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGoToDialog, CDialog)
	//{{AFX_MSG_MAP(CGoToDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGoToDialog message handlers

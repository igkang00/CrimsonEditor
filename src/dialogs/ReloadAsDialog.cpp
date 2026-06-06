// ReloadAsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ReloadAsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReloadAsDialog dialog


CReloadAsDialog::CReloadAsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CReloadAsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReloadAsDialog)
	m_nEncodingType = -1;
	//}}AFX_DATA_INIT
}


void CReloadAsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReloadAsDialog)
	DDX_CBIndex(pDX, IDC_ENCODING_TYPE, m_nEncodingType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReloadAsDialog, CDialog)
	//{{AFX_MSG_MAP(CReloadAsDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReloadAsDialog message handlers

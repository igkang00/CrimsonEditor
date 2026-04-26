// DummyDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DummyDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDummyDialog dialog


CDummyDialog::CDummyDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CDummyDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDummyDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDummyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDummyDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDummyDialog, CDialog)
	//{{AFX_MSG_MAP(CDummyDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDummyDialog message handlers

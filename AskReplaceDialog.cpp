// AskReplaceDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AskReplaceDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAskReplaceDialog dialog


CAskReplaceDialog::CAskReplaceDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAskReplaceDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAskReplaceDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAskReplaceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAskReplaceDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAskReplaceDialog, CDialog)
	//{{AFX_MSG_MAP(CAskReplaceDialog)
	ON_BN_CLICKED(IDYES, OnYes)
	ON_BN_CLICKED(IDNO, OnNo)
	ON_BN_CLICKED(ID_ALL, OnAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAskReplaceDialog message handlers

BOOL CAskReplaceDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	RECT rect; GetWindowRect( & rect );
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	rect.left = m_ptInitialPos.x;
	rect.top = m_ptInitialPos.y;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;
	MoveWindow( & rect );
	
	return TRUE;
}

void CAskReplaceDialog::OnYes() 
{
	m_bReplaceAll = FALSE;
	m_bSearchNext = TRUE;
	m_bSearchPrev = FALSE;
	CDialog::OnOK();
}

void CAskReplaceDialog::OnNo() 
{
	m_bReplaceAll = FALSE;
	m_bSearchNext = TRUE;
	m_bSearchPrev = FALSE;
	CDialog::OnCancel();
}

void CAskReplaceDialog::OnAll() 
{
	m_bReplaceAll = TRUE;
	m_bSearchNext = FALSE;
	m_bSearchPrev = FALSE;
	CDialog::OnCancel();
}

void CAskReplaceDialog::OnOK() 
{
	m_bReplaceAll = FALSE;
	m_bSearchNext = FALSE;
	m_bSearchPrev = FALSE;
	CDialog::OnOK();
}

void CAskReplaceDialog::OnCancel() 
{
	m_bReplaceAll = FALSE;
	m_bSearchNext = FALSE;
	m_bSearchPrev = FALSE;
	CDialog::OnCancel();
}

BOOL CAskReplaceDialog::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F3 ) {

		if( GetKeyState(VK_SHIFT) & 0xF0 ) { // Shift + F3
			m_bReplaceAll = FALSE;
			m_bSearchNext = FALSE;
			m_bSearchPrev = TRUE;
			CDialog::OnCancel();

		} else { // F3
			m_bReplaceAll = FALSE;
			m_bSearchNext = TRUE;
			m_bSearchPrev = FALSE;
			CDialog::OnCancel();
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

// MacroDefineDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MacroDefineDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMacroDefineDialog dialog


CMacroDefineDialog::CMacroDefineDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CMacroDefineDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMacroDefineDialog)
	//}}AFX_DATA_INIT
}


void CMacroDefineDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMacroDefineDialog)
	DDX_Control(pDX, IDC_MACRO_NAME, m_edtMacroName);
	DDX_Control(pDX, IDC_MACRO_LIST, m_lstMacro);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMacroDefineDialog, CDialog)
	//{{AFX_MSG_MAP(CMacroDefineDialog)
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_MACRO_LIST, OnItemchangedMacroList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMacroDefineDialog message handlers

BOOL CMacroDefineDialog::OnInitDialog() 
{
	CString szText;
	CDialog::OnInitDialog();

	szText.LoadString(IDS_CTRL_DEFINE_MACRO_NO);
	m_lstMacro.InsertColumn(0, szText, LVCFMT_LEFT, 40);
	szText.LoadString(IDS_CTRL_DEFINE_MACRO_NAME);
	m_lstMacro.InsertColumn(1, szText, LVCFMT_LEFT, 145);

	m_lstMacro.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	for(INT i = 0; i < 10; i++) {
		CString szText; szText.Format("%d", (i+1) % 10);
		m_lstMacro.InsertItem(i, szText);
		m_lstMacro.SetItemText(i, 1, m_szMacroName[i+1]);
	}

	m_lstMacro.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
	m_nSelected = 1;

	CString szMacroName; szMacroName.LoadString(IDS_CTRL_DEFINE_MACRO_NEW);
	m_edtMacroName.SetWindowText(szMacroName);

	return TRUE;
}

void CMacroDefineDialog::OnDestroy() 
{
	CString szMacroName; m_edtMacroName.GetWindowText( szMacroName );

	for(INT i = 0; i < 10; i++) {
		if( m_lstMacro.GetItemState(i, LVIS_SELECTED) ) {
			m_szMacroName[i+1] = szMacroName;
			m_nSelected = i+1;
		}
	}

	CDialog::OnDestroy();
}

void CMacroDefineDialog::OnItemchangedMacroList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if( (pNMListView->iItem >= 0) && (pNMListView->uNewState & LVIS_SELECTED) ) {
		INT nMacro = pNMListView->iItem;
		CString szMacroName = m_lstMacro.GetItemText( nMacro, 1 );
		if( szMacroName == "- Empty -" ) szMacroName = "";
		if( szMacroName == "[Noname]" ) szMacroName = "";
		m_edtMacroName.SetWindowText( szMacroName );
	}
	*pResult = 0;
}

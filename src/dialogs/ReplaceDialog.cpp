// ReplaceDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ReplaceDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REGKEY_SEARCH_DIALOG	"Search Dialog"

/////////////////////////////////////////////////////////////////////////////
// CReplaceDialog dialog


CReplaceDialog::CReplaceDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CReplaceDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReplaceDialog)
	m_bWholeWord = FALSE;
	m_bRegularExpression = FALSE;
	m_bMatchCase = FALSE;
	m_nRange = -1;
	//}}AFX_DATA_INIT

	m_bLogFindSelection = FALSE;
	m_nFindSelBeg = m_nFindSelEnd = 0;

	m_bLogReplaceSelection = FALSE;
	m_nReplaceSelBeg = m_nReplaceSelEnd = 0;
}


void CReplaceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReplaceDialog)
	DDX_Control(pDX, IDC_REPLACE_ALL, m_btnReplaceAll);
	DDX_Control(pDX, IDC_FIND, m_btnFind);
	DDX_Control(pDX, IDC_REPLACE_STRING_MENU, m_btnReplaceString);
	DDX_Control(pDX, IDC_FIND_STRING_MENU, m_btnFindString);
	DDX_Control(pDX, IDC_REPLACE_STRING, m_cmbReplaceString);
	DDX_Control(pDX, IDC_FIND_STRING, m_cmbFindString);
	DDX_Check(pDX, IDC_WHOLE_WORD, m_bWholeWord);
	DDX_Check(pDX, IDC_REGULAR_EXPRESSION, m_bRegularExpression);
	DDX_Check(pDX, IDC_MATCH_CASE, m_bMatchCase);
	DDX_Radio(pDX, IDC_REPLACE_IN0, m_nRange);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReplaceDialog, CDialog)
	//{{AFX_MSG_MAP(CReplaceDialog)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_FIND, OnFind)
	ON_BN_CLICKED(IDC_REPLACE_ALL, OnReplaceAll)
	ON_CBN_SETFOCUS(IDC_FIND_STRING, OnSetfocusFindString)
	ON_CBN_KILLFOCUS(IDC_FIND_STRING, OnKillfocusFindString)
	ON_CBN_SETFOCUS(IDC_REPLACE_STRING, OnSetfocusReplaceString)
	ON_CBN_KILLFOCUS(IDC_REPLACE_STRING, OnKillfocusReplaceString)
	ON_BN_CLICKED(IDC_FIND_STRING_MENU, OnFindStringMenu)
	ON_BN_CLICKED(IDC_REPLACE_STRING_MENU, OnReplaceStringMenu)
	ON_COMMAND(ID_SRCH_FIND_TAB_CHAR, OnFindTextTabChar)
	ON_COMMAND(ID_SRCH_FIND_ANY_CHAR, OnFindTextAnyChar)
	ON_COMMAND(ID_SRCH_FIND_BEGIN_OF_LINE, OnFindTextBeginOfLine)
	ON_COMMAND(ID_SRCH_FIND_END_OF_LINE, OnFindTextEndOfLine)
	ON_COMMAND(ID_SRCH_FIND_ZERO_OR_MORE, OnFindTextZeroOrMore)
	ON_COMMAND(ID_SRCH_FIND_ONE_OR_MORE, OnFindTextOneOrMore)
	ON_COMMAND(ID_SRCH_FIND_ZERO_OR_ONE, OnFindTextZeroOrOne)
	ON_COMMAND(ID_SRCH_FIND_OR, OnFindTextOr)
	ON_COMMAND(ID_SRCH_FIND_IN_RANGE, OnFindTextInRange)
	ON_COMMAND(ID_SRCH_FIND_NOT_IN_RANGE, OnFindTextNotInRange)
	ON_COMMAND(ID_SRCH_FIND_WHITE_SPACE, OnFindTextWhiteSpace)
	ON_COMMAND(ID_SRCH_FIND_ALPHA_CHAR, OnFindTextAlphaChar)
	ON_COMMAND(ID_SRCH_FIND_ALNUM_CHAR, OnFindTextAlnumChar)
	ON_COMMAND(ID_SRCH_FIND_DEC_DIGIT, OnFindTextDecDigit)
	ON_COMMAND(ID_SRCH_FIND_HEX_DIGIT, OnFindTextHexDigit)
	ON_COMMAND(ID_SRCH_FIND_TAGGED_EXP, OnFindTextTaggedExp)
	ON_COMMAND(ID_SRCH_REPL_TAB_CHAR, OnReplaceTextTabChar)
	ON_COMMAND(ID_SRCH_REPL_TAGGED_EXP0, OnReplaceTextTaggedExp0)
	ON_COMMAND(ID_SRCH_REPL_TAGGED_EXP1, OnReplaceTextTaggedExp1)
	ON_COMMAND(ID_SRCH_REPL_TAGGED_EXP2, OnReplaceTextTaggedExp2)
	ON_COMMAND(ID_SRCH_REPL_TAGGED_EXP3, OnReplaceTextTaggedExp3)
	ON_COMMAND(ID_SRCH_REPL_TAGGED_EXP4, OnReplaceTextTaggedExp4)
	ON_COMMAND(ID_SRCH_REPL_TAGGED_EXP5, OnReplaceTextTaggedExp5)
	ON_COMMAND(ID_SRCH_REPL_TAGGED_EXP6, OnReplaceTextTaggedExp6)
	ON_COMMAND(ID_SRCH_REPL_TAGGED_EXP7, OnReplaceTextTaggedExp7)
	ON_COMMAND(ID_SRCH_REPL_TAGGED_EXP8, OnReplaceTextTaggedExp8)
	ON_COMMAND(ID_SRCH_REPL_TAGGED_EXP9, OnReplaceTextTaggedExp9)
	ON_CBN_EDITCHANGE(IDC_FIND_STRING, OnEditchangeFindString)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReplaceDialog message handlers

int CReplaceDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if( CDialog::OnCreate(lpCreateStruct) == -1 ) return -1;
	
	TRACE0("CReplaceDialog::OnCreate\n");
	m_lstButtonImage.Create(IDB_GENERAL_BUTTONS, 9, 0, RGB(255, 0, 255));
	
	// load replace preferences from profile
	CWinApp * pApp = AfxGetApp(); ASSERT( pApp );

	m_bWholeWord = pApp->GetProfileInt( REGKEY_SEARCH_DIALOG, "WholeWord", FALSE );
	m_bMatchCase = pApp->GetProfileInt( REGKEY_SEARCH_DIALOG, "MatchCase", FALSE );
	m_bRegularExpression = pApp->GetProfileInt( REGKEY_SEARCH_DIALOG, "RegularExpression", FALSE );
	
	m_lstFindString.RemoveAll();
	INT nCount = pApp->GetProfileInt( REGKEY_SEARCH_DIALOG, "FindStringCount", 0 );

	for( INT i = 0; i < nCount; i++ ) {
		CString szEntry; szEntry.Format( "FindString%d", i );
		CString szFindString = pApp->GetProfileString( REGKEY_SEARCH_DIALOG, szEntry, "" );
		m_lstFindString.AddTail( szFindString );
	}
	
	m_lstReplaceString.RemoveAll();
	INT nCoun2 = pApp->GetProfileInt( REGKEY_SEARCH_DIALOG, "ReplaceStringCount", 0 );

	for( INT j = 0; j < nCoun2; j++ ) {
		CString szEntry; szEntry.Format( "ReplaceString%d", j );
		CString szReplaceString = pApp->GetProfileString( REGKEY_SEARCH_DIALOG, szEntry, "" );
		m_lstReplaceString.AddTail( szReplaceString );
	}

	return 0;
}

void CReplaceDialog::OnDestroy() 
{
	CDialog::OnDestroy();
	
	TRACE0("CReplaceDialog::OnDestroy\n");
	m_lstButtonImage.Detach();

	// save find preferences to profile
	CWinApp * pApp = AfxGetApp(); ASSERT( pApp );

	pApp->WriteProfileInt( REGKEY_SEARCH_DIALOG, "WholeWord", m_bWholeWord );
	pApp->WriteProfileInt( REGKEY_SEARCH_DIALOG, "MatchCase", m_bMatchCase );
	pApp->WriteProfileInt( REGKEY_SEARCH_DIALOG, "RegularExpression", m_bRegularExpression );

	INT nCount = m_lstFindString.GetCount();
	pApp->WriteProfileInt( REGKEY_SEARCH_DIALOG, "FindStringCount", nCount );

	POSITION pos = m_lstFindString.GetHeadPosition();
	for( INT i = 0; i < nCount; i++ ) {
		CString szEntry; szEntry.Format( "FindString%d", i );
		CString szFindString = m_lstFindString.GetNext( pos );
		pApp->WriteProfileString( REGKEY_SEARCH_DIALOG, szEntry, szFindString );
	}

	INT nCoun2 = m_lstReplaceString.GetCount();
	pApp->WriteProfileInt( REGKEY_SEARCH_DIALOG, "ReplaceStringCount", nCoun2 );

	POSITION po2 = m_lstReplaceString.GetHeadPosition();
	for( INT j = 0; j < nCoun2; j++ ) {
		CString szEntry; szEntry.Format( "ReplaceString%d", j );
		CString szReplaceString = m_lstReplaceString.GetNext( po2 );
		pApp->WriteProfileString( REGKEY_SEARCH_DIALOG, szEntry, szReplaceString );
	}
}

BOOL CReplaceDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	TRACE0("CReplaceDialog::OnInitDialog\n");

	m_btnFindString.SetIcon( m_lstButtonImage.ExtractIcon(2) );
	m_btnReplaceString.SetIcon( m_lstButtonImage.ExtractIcon(2) );

	POSITION pos = m_lstFindString.GetHeadPosition();
	while( pos ) m_cmbFindString.AddString( m_lstFindString.GetNext(pos) );

	if( ! m_szFindString.GetLength() && m_lstFindString.GetCount() ) m_szFindString = m_lstFindString.GetHead();
	m_cmbFindString.SetWindowText( m_szFindString );

	m_btnFind.EnableWindow( m_szFindString.GetLength() );
	m_btnReplaceAll.EnableWindow( m_szFindString.GetLength() );

	POSITION po2 = m_lstReplaceString.GetHeadPosition();
	while( po2 ) m_cmbReplaceString.AddString( m_lstReplaceString.GetNext(po2) );

	if( ! m_szReplaceString.GetLength() && m_lstReplaceString.GetCount() ) m_szReplaceString = m_lstReplaceString.GetHead();
	m_cmbReplaceString.SetWindowText( m_szReplaceString );

	if( m_nRange == 0 ) { // replace in selection
		UINT nStyle; SetDefID(IDC_REPLACE_ALL);

		nStyle = m_btnReplaceAll.GetButtonStyle();
		m_btnReplaceAll.SetButtonStyle( nStyle | BS_DEFPUSHBUTTON );

		nStyle = m_btnFind.GetButtonStyle();
		m_btnFind.SetButtonStyle( nStyle & ~BS_DEFPUSHBUTTON );

	} else { // replace in all file
		UINT nStyle; SetDefID(IDC_FIND);

		CButton * pButton = (CButton *)GetDlgItem(IDC_REPLACE_IN0);
		pButton->EnableWindow(FALSE);

		nStyle = m_btnFind.GetButtonStyle();
		m_btnFind.SetButtonStyle( nStyle | BS_DEFPUSHBUTTON );

		nStyle = m_btnReplaceAll.GetButtonStyle();
		m_btnReplaceAll.SetButtonStyle( nStyle & ~BS_DEFPUSHBUTTON );
	}

	return TRUE;
}

void CReplaceDialog::OnFind() 
{
	m_cmbFindString.GetWindowText( m_szFindString );
	m_cmbReplaceString.GetWindowText( m_szReplaceString );

	if( ! m_szFindString.GetLength() ) return;

	POSITION pos = m_lstFindString.Find( m_szFindString );
	if( pos ) m_lstFindString.RemoveAt( pos );
	while( m_lstFindString.GetCount() >= 16 ) m_lstFindString.RemoveTail();
	m_lstFindString.AddHead( m_szFindString );

	POSITION po2 = m_lstReplaceString.Find( m_szReplaceString );
	if( po2 ) m_lstReplaceString.RemoveAt( po2 );
	while( m_lstReplaceString.GetCount() >= 16 ) m_lstReplaceString.RemoveTail();
	if( m_szReplaceString.GetLength() ) m_lstReplaceString.AddHead( m_szReplaceString );

	m_bReplaceAll = FALSE;
	CDialog::OnOK();
}

void CReplaceDialog::OnReplaceAll() 
{
	m_cmbFindString.GetWindowText( m_szFindString );
	m_cmbReplaceString.GetWindowText( m_szReplaceString );

	if( ! m_szFindString.GetLength() ) return;

	POSITION pos = m_lstFindString.Find( m_szFindString );
	if( pos ) m_lstFindString.RemoveAt( pos );
	while( m_lstFindString.GetCount() >= 16 ) m_lstFindString.RemoveTail();
	m_lstFindString.AddHead( m_szFindString );

	POSITION po2 = m_lstReplaceString.Find( m_szReplaceString );
	if( po2 ) m_lstReplaceString.RemoveAt( po2 );
	while( m_lstReplaceString.GetCount() >= 16 ) m_lstReplaceString.RemoveTail();
	if( m_szReplaceString.GetLength() ) m_lstReplaceString.AddHead( m_szReplaceString );

	m_bReplaceAll = TRUE;
	CDialog::OnOK();
}


void CReplaceDialog::OnSetfocusFindString() 
{
	m_bLogFindSelection = TRUE;
}

void CReplaceDialog::OnKillfocusFindString() 
{
	m_bLogFindSelection = FALSE;
}

void CReplaceDialog::OnSetfocusReplaceString() 
{
	m_bLogReplaceSelection = TRUE;
}

void CReplaceDialog::OnKillfocusReplaceString() 
{
	m_bLogReplaceSelection = FALSE;
}

BOOL CReplaceDialog::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYUP || pMsg->message == WM_LBUTTONUP ) {
		if( m_bLogFindSelection ) {
			DWORD dwSelect = m_cmbFindString.GetEditSel();
			m_nFindSelBeg = LOWORD(dwSelect);
			m_nFindSelEnd = HIWORD(dwSelect);
		}
		if( m_bLogReplaceSelection ) {
			DWORD dwSelect = m_cmbReplaceString.GetEditSel();
			m_nReplaceSelBeg = LOWORD(dwSelect);
			m_nReplaceSelEnd = HIWORD(dwSelect);
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CReplaceDialog::OnEditchangeFindString() 
{
	CString szFindString; m_cmbFindString.GetWindowText( szFindString );
	m_btnFind.EnableWindow( szFindString.GetLength() );
	m_btnReplaceAll.EnableWindow( szFindString.GetLength() );
}

void CReplaceDialog::OnFindStringMenu() 
{
	CMenu * pMenu, context; context.LoadMenu(IDR_SEARCH_DIALOG);
	pMenu = context.GetSubMenu(0);

	CRect rect; m_btnFindString.GetWindowRect( & rect );
	CPoint point(rect.right, rect.top);

	UINT nFlags = TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON;
	pMenu->TrackPopupMenu(nFlags, point.x, point.y, this);
}

void CReplaceDialog::OnReplaceStringMenu() 
{
	CMenu * pMenu, context; context.LoadMenu(IDR_SEARCH_DIALOG);
	pMenu = context.GetSubMenu(1);

	CRect rect; m_btnReplaceString.GetWindowRect( & rect );
	CPoint point(rect.right, rect.top);

	UINT nFlags = TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON;
	pMenu->TrackPopupMenu(nFlags, point.x, point.y, this);
}

/////////////////////////////////////////////////////////////////////////////
// Context Menu Handlers

void CReplaceDialog::ReplaceFindSelection(LPCTSTR lpszString, INT nIncrement)
{
	CString szText; m_cmbFindString.GetWindowText( szText );
	szText = szText.Left( m_nFindSelBeg ) + CString(lpszString) + szText.Mid( m_nFindSelEnd );
	m_cmbFindString.SetWindowText( szText ); m_cmbFindString.SetFocus();
	m_cmbFindString.SetEditSel( m_nFindSelBeg + nIncrement, m_nFindSelBeg + nIncrement );

	((CButton *)GetDlgItem(IDC_REGULAR_EXPRESSION))->SetCheck(1);
	m_btnFind.EnableWindow( TRUE );
	m_btnReplaceAll.EnableWindow( TRUE );
}

void CReplaceDialog::ReplaceReplaceSelection(LPCTSTR lpszString, INT nIncrement)
{
	CString szText; m_cmbReplaceString.GetWindowText( szText );
	szText = szText.Left( m_nReplaceSelBeg ) + CString(lpszString) + szText.Mid( m_nReplaceSelEnd );
	m_cmbReplaceString.SetWindowText( szText ); m_cmbReplaceString.SetFocus();
	m_cmbReplaceString.SetEditSel( m_nReplaceSelBeg + nIncrement, m_nReplaceSelBeg + nIncrement );

	((CButton *)GetDlgItem(IDC_REGULAR_EXPRESSION))->SetCheck(1);
//	m_btnFind.EnableWindow( TRUE );
//	m_btnReplaceAll.EnableWindow( TRUE );
}

void CReplaceDialog::OnFindTextTabChar() 
{
	ReplaceFindSelection( "\\t", 2 );
}

void CReplaceDialog::OnFindTextAnyChar() 
{
	ReplaceFindSelection( ".", 1 );
}

void CReplaceDialog::OnFindTextBeginOfLine() 
{
	ReplaceFindSelection( "^", 1 );
}

void CReplaceDialog::OnFindTextEndOfLine() 
{
	ReplaceFindSelection( "$", 1 );
}

void CReplaceDialog::OnFindTextZeroOrMore() 
{
	ReplaceFindSelection( "*", 1 );
}

void CReplaceDialog::OnFindTextOneOrMore() 
{
	ReplaceFindSelection( "+", 1 );
}

void CReplaceDialog::OnFindTextZeroOrOne() 
{
	ReplaceFindSelection( "?", 1 );
}

void CReplaceDialog::OnFindTextOr() 
{
	ReplaceFindSelection( "|", 1 );
}

void CReplaceDialog::OnFindTextInRange() 
{
	ReplaceFindSelection( "[]", 1 );
}

void CReplaceDialog::OnFindTextNotInRange() 
{
	ReplaceFindSelection( "[^]", 2 );
}

void CReplaceDialog::OnFindTextWhiteSpace() 
{
	ReplaceFindSelection( "\\s", 2 );
}

void CReplaceDialog::OnFindTextAlnumChar() 
{
	ReplaceFindSelection( "\\w", 2 );
}

void CReplaceDialog::OnFindTextAlphaChar() 
{
	ReplaceFindSelection( "\\a", 2 );
}

void CReplaceDialog::OnFindTextDecDigit() 
{
	ReplaceFindSelection( "\\d", 2 );
}

void CReplaceDialog::OnFindTextHexDigit() 
{
	ReplaceFindSelection( "\\h", 2 );
}

void CReplaceDialog::OnFindTextTaggedExp() 
{
	ReplaceFindSelection( "()", 1 );
}


void CReplaceDialog::OnReplaceTextTabChar() 
{
	ReplaceReplaceSelection( "\\t", 2 );
}

void CReplaceDialog::OnReplaceTextTaggedExp0() 
{
	ReplaceReplaceSelection( "\\0", 2 );
}

void CReplaceDialog::OnReplaceTextTaggedExp1() 
{
	ReplaceReplaceSelection( "\\1", 2 );
}

void CReplaceDialog::OnReplaceTextTaggedExp2() 
{
	ReplaceReplaceSelection( "\\2", 2 );
}

void CReplaceDialog::OnReplaceTextTaggedExp3() 
{
	ReplaceReplaceSelection( "\\3", 2 );
}

void CReplaceDialog::OnReplaceTextTaggedExp4() 
{
	ReplaceReplaceSelection( "\\4", 2 );
}

void CReplaceDialog::OnReplaceTextTaggedExp5() 
{
	ReplaceReplaceSelection( "\\5", 2 );
}

void CReplaceDialog::OnReplaceTextTaggedExp6() 
{
	ReplaceReplaceSelection( "\\6", 2 );
}

void CReplaceDialog::OnReplaceTextTaggedExp7() 
{
	ReplaceReplaceSelection( "\\7", 2 );
}

void CReplaceDialog::OnReplaceTextTaggedExp8() 
{
	ReplaceReplaceSelection( "\\8", 2 );
}

void CReplaceDialog::OnReplaceTextTaggedExp9() 
{
	ReplaceReplaceSelection( "\\9", 2 );
}

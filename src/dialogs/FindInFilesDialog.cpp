// FindInFilesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "FindInFilesDialog.h"
#include "FolderDialog.h"
#include "PathName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REGKEY_SEARCH_DIALOG	"Search Dialog"

/////////////////////////////////////////////////////////////////////////////
// CFindInFilesDialog dialog


CFindInFilesDialog::CFindInFilesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFindInFilesDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindInFilesDialog)
	m_bRegularExpression = FALSE;
	m_bWholeWord = FALSE;
	m_bMatchCase = FALSE;
	m_bLookInSubfolders = FALSE;
	//}}AFX_DATA_INIT

	m_bLogFindSelection = FALSE;
	m_nFindSelBeg = m_nFindSelEnd = 0;
}


void CFindInFilesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindInFilesDialog)
	DDX_Control(pDX, IDC_FIND, m_btnFind);
	DDX_Control(pDX, IDC_FIND_STRING_MENU, m_btnFindString);
	DDX_Control(pDX, IDC_FILE_TYPE, m_cmbFileType);
	DDX_Control(pDX, IDC_FOLDER, m_cmbFolder);
	DDX_Control(pDX, IDC_FIND_STRING, m_cmbFindString);
	DDX_Check(pDX, IDC_REGULAR_EXPRESSION, m_bRegularExpression);
	DDX_Check(pDX, IDC_WHOLE_WORD, m_bWholeWord);
	DDX_Check(pDX, IDC_MATCH_CASE, m_bMatchCase);
	DDX_Check(pDX, IDC_LOOK_SUBFOLDERS, m_bLookInSubfolders);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindInFilesDialog, CDialog)
	//{{AFX_MSG_MAP(CFindInFilesDialog)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_FIND, OnFind)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_CBN_SETFOCUS(IDC_FIND_STRING, OnSetfocusFindString)
	ON_CBN_KILLFOCUS(IDC_FIND_STRING, OnKillfocusFindString)
	ON_BN_CLICKED(IDC_FIND_STRING_MENU, OnFindStringMenu)
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
	ON_CBN_EDITCHANGE(IDC_FIND_STRING, OnEditchangeFindString)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindInFilesDialog message handlers

int CFindInFilesDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if( CDialog::OnCreate(lpCreateStruct) == -1 ) return -1;
	
	TRACE0("CFindInFilesDialog::OnCreate\n");
	m_lstButtonImage.Create(IDB_GENERAL_BUTTONS, 9, 0, RGB(255, 0, 255));

	// load find preferences from profile
	CWinApp * pApp = AfxGetApp(); ASSERT( pApp );

	m_bWholeWord = pApp->GetProfileInt( REGKEY_SEARCH_DIALOG, "WholeWord", FALSE );
	m_bMatchCase = pApp->GetProfileInt( REGKEY_SEARCH_DIALOG, "MatchCase", FALSE );
	m_bRegularExpression = pApp->GetProfileInt( REGKEY_SEARCH_DIALOG, "RegularExpression", FALSE );
	m_bLookInSubfolders = pApp->GetProfileInt( REGKEY_SEARCH_DIALOG, "LookInSubfolders", FALSE );

	m_lstFindString.RemoveAll();
	INT nCount = pApp->GetProfileInt( REGKEY_SEARCH_DIALOG, "FindStringCount", 0 );

	for( INT i = 0; i < nCount; i++ ) {
		CString szEntry; szEntry.Format( "FindString%d", i );
		CString szFindString = pApp->GetProfileString( REGKEY_SEARCH_DIALOG, szEntry, "" );
		m_lstFindString.AddTail( szFindString );
	}

	// load last used file type
	m_szFileType = pApp->GetProfileString( REGKEY_SEARCH_DIALOG, "FIF_FileType", "");

	m_lstFolder.RemoveAll();
	INT nCoun3 = pApp->GetProfileInt( REGKEY_SEARCH_DIALOG, "FIF_FolderCount", 0 );

	for( INT k = 0; k < nCoun3; k++ ) {
		CString szEntry; szEntry.Format( "FIF_Folder%d", k );
		CString szFolder = pApp->GetProfileString( REGKEY_SEARCH_DIALOG, szEntry, "" );
		m_lstFolder.AddTail( szFolder );
	}
	
	return 0;
}

void CFindInFilesDialog::OnDestroy() 
{
	CDialog::OnDestroy();
	
	TRACE0("CFindInFilesDialog::OnDestroy\n");
	m_lstButtonImage.Detach();

	// save find preferences to profile
	CWinApp * pApp = AfxGetApp(); ASSERT( pApp );

	pApp->WriteProfileInt( REGKEY_SEARCH_DIALOG, "WholeWord", m_bWholeWord );
	pApp->WriteProfileInt( REGKEY_SEARCH_DIALOG, "MatchCase", m_bMatchCase );
	pApp->WriteProfileInt( REGKEY_SEARCH_DIALOG, "RegularExpression", m_bRegularExpression );
	pApp->WriteProfileInt( REGKEY_SEARCH_DIALOG, "LookInSubfolders", m_bLookInSubfolders );

	INT nCount = m_lstFindString.GetCount();
	pApp->WriteProfileInt( REGKEY_SEARCH_DIALOG, "FindStringCount", nCount );

	POSITION pos = m_lstFindString.GetHeadPosition();
	for( INT i = 0; i < nCount; i++ ) {
		CString szEntry; szEntry.Format( "FindString%d", i );
		CString szFindString = m_lstFindString.GetNext( pos );
		pApp->WriteProfileString( REGKEY_SEARCH_DIALOG, szEntry, szFindString );
	}

	// save last used file type
	pApp->WriteProfileString( REGKEY_SEARCH_DIALOG, "FIF_FileType", m_szFileType );

	INT nCoun3 = m_lstFolder.GetCount();
	pApp->WriteProfileInt( REGKEY_SEARCH_DIALOG, "FIF_FolderCount", nCoun3 );

	POSITION po3 = m_lstFolder.GetHeadPosition();
	for( INT k = 0; k < nCoun3; k++ ) {
		CString szEntry; szEntry.Format( "FIF_Folder%d", k );
		CString szFolder = m_lstFolder.GetNext( po3 );
		pApp->WriteProfileString( REGKEY_SEARCH_DIALOG, szEntry, szFolder );
	}
}

BOOL CFindInFilesDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	TRACE0("CFindInFilesDialog::OnInitDialog\n");

	m_btnFindString.SetIcon( m_lstButtonImage.ExtractIcon(2) );
	
	POSITION pos = m_lstFindString.GetHeadPosition();
	while( pos ) m_cmbFindString.AddString( m_lstFindString.GetNext(pos) );

	if( ! m_szFindString.GetLength() && m_lstFindString.GetCount() ) m_szFindString = m_lstFindString.GetHead();
	m_cmbFindString.SetWindowText( m_szFindString );

	m_btnFind.EnableWindow( m_szFindString.GetLength() );

	POSITION po2 = m_lstFileType.GetHeadPosition();
	while( po2 ) m_cmbFileType.AddString( m_lstFileType.GetNext(po2) );

	if( ! m_szFileType.GetLength() ) m_szFileType = "*.*";
	m_cmbFileType.SetWindowText( m_szFileType );

	POSITION po3 = m_lstFolder.GetHeadPosition();
	while( po3 ) m_cmbFolder.AddString( m_lstFolder.GetNext(po3) );

	TCHAR szCurrentDirectory[MAX_PATH];
	GetCurrentDirectory( MAX_PATH, szCurrentDirectory );

	if( ! m_szFolder.GetLength() && m_lstFolder.GetCount() ) m_szFolder = m_lstFolder.GetHead();
	else if( ! m_szFolder.GetLength() ) m_szFolder = szCurrentDirectory;
	m_cmbFolder.SetWindowText( m_szFolder );

	return TRUE;
}

void CFindInFilesDialog::OnFind() 
{
	m_cmbFindString.GetWindowText( m_szFindString );
	m_cmbFileType.GetWindowText( m_szFileType );
	m_cmbFolder.GetWindowText( m_szFolder );

	if( ! m_szFindString.GetLength() ) return;
	if( ! m_szFileType.GetLength() ) return;
	if( ! m_szFolder.GetLength() ) return;

	POSITION pos = m_lstFindString.Find( m_szFindString );
	if( pos ) m_lstFindString.RemoveAt( pos );
	while( m_lstFindString.GetCount() >= 16 ) m_lstFindString.RemoveTail();
	m_lstFindString.AddHead( m_szFindString );

	POSITION po3 = m_lstFolder.Find( m_szFolder );
	if( po3 ) m_lstFolder.RemoveAt( po3 );
	if( m_lstFolder.GetCount() >= 16 ) m_lstFolder.RemoveTail();
	m_lstFolder.AddHead( m_szFolder );

	CDialog::OnOK();
}

void CFindInFilesDialog::OnSetfocusFindString() 
{
	m_bLogFindSelection = TRUE;
}

void CFindInFilesDialog::OnKillfocusFindString() 
{
	m_bLogFindSelection = FALSE;
}

BOOL CFindInFilesDialog::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYUP || pMsg->message == WM_LBUTTONUP ) {
		if( m_bLogFindSelection ) {
			DWORD dwSelect = m_cmbFindString.GetEditSel();
			m_nFindSelBeg = LOWORD(dwSelect);
			m_nFindSelEnd = HIWORD(dwSelect);
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CFindInFilesDialog::OnFindStringMenu() 
{
	CMenu * pMenu, context; context.LoadMenu(IDR_SEARCH_DIALOG);
	pMenu = context.GetSubMenu(0);

	CRect rect; m_btnFindString.GetWindowRect( & rect );
	CPoint point(rect.right, rect.top);

	UINT nFlags = TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON;
	pMenu->TrackPopupMenu(nFlags, point.x, point.y, this);
}

void CFindInFilesDialog::OnBrowse() 
{
	CString szDirectory; m_cmbFolder.GetWindowText( szDirectory );
	CString szText( LPCTSTR(IDS_CHOOSE_DIR_TO_SEARCH) );
	CFolderDialog dlg(szText, szDirectory, NULL, this);
	if( dlg.DoModal() != IDOK ) return;
	m_cmbFolder.SetWindowText( dlg.GetPathName() );
}

void CFindInFilesDialog::OnEditchangeFindString() 
{
	CString szFindString; m_cmbFindString.GetWindowText( szFindString );
	m_btnFind.EnableWindow( szFindString.GetLength() );
}

/////////////////////////////////////////////////////////////////////////////
// Context Menu Handlers
BOOL CFindInFilesDialog::InitFileTypeList(LPCTSTR lpszComposedFilter)
{
	CStringArray arrFilterDescription, arrFilterExtensions;
	ParseFileFilter(arrFilterDescription, arrFilterExtensions, lpszComposedFilter);

	m_lstFileType.RemoveAll();
	INT nCount = arrFilterExtensions.GetSize();

	for( INT i = 0; i < nCount; i++ ) {
		m_lstFileType.AddTail( arrFilterExtensions[i] );
	}

	return TRUE;
}

void CFindInFilesDialog::ReplaceFindSelection(LPCTSTR lpszString, INT nIncrement)
{
	CString szText; m_cmbFindString.GetWindowText( szText );
	szText = szText.Left( m_nFindSelBeg ) + CString(lpszString) + szText.Mid( m_nFindSelEnd );
	m_cmbFindString.SetWindowText( szText ); m_cmbFindString.SetFocus();
	m_cmbFindString.SetEditSel( m_nFindSelBeg + nIncrement, m_nFindSelBeg + nIncrement );

	((CButton *)GetDlgItem(IDC_REGULAR_EXPRESSION))->SetCheck(1);
	m_btnFind.EnableWindow( TRUE );
}

void CFindInFilesDialog::OnFindTextTabChar() 
{
	ReplaceFindSelection( "\\t", 2 );
}

void CFindInFilesDialog::OnFindTextAnyChar() 
{
	ReplaceFindSelection( ".", 1 );
}

void CFindInFilesDialog::OnFindTextBeginOfLine() 
{
	ReplaceFindSelection( "^", 1 );
}

void CFindInFilesDialog::OnFindTextEndOfLine() 
{
	ReplaceFindSelection( "$", 1 );
}

void CFindInFilesDialog::OnFindTextZeroOrMore() 
{
	ReplaceFindSelection( "*", 1 );
}

void CFindInFilesDialog::OnFindTextOneOrMore() 
{
	ReplaceFindSelection( "+", 1 );
}

void CFindInFilesDialog::OnFindTextZeroOrOne() 
{
	ReplaceFindSelection( "?", 1 );
}

void CFindInFilesDialog::OnFindTextOr() 
{
	ReplaceFindSelection( "|", 1 );
}

void CFindInFilesDialog::OnFindTextInRange() 
{
	ReplaceFindSelection( "[]", 1 );
}

void CFindInFilesDialog::OnFindTextNotInRange() 
{
	ReplaceFindSelection( "[^]", 2 );
}

void CFindInFilesDialog::OnFindTextWhiteSpace() 
{
	ReplaceFindSelection( "\\s", 2 );
}

void CFindInFilesDialog::OnFindTextAlnumChar() 
{
	ReplaceFindSelection( "\\w", 2 );
}

void CFindInFilesDialog::OnFindTextAlphaChar() 
{
	ReplaceFindSelection( "\\a", 2 );
}

void CFindInFilesDialog::OnFindTextDecDigit() 
{
	ReplaceFindSelection( "\\d", 2 );
}

void CFindInFilesDialog::OnFindTextHexDigit() 
{
	ReplaceFindSelection( "\\h", 2 );
}

void CFindInFilesDialog::OnFindTextTaggedExp() 
{
	ReplaceFindSelection( "()", 1 );
}

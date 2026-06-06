// FileWindow.cpp : implementation file
//

#include "stdafx.h"
#include "cedtHeader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CFileWindow::CFileWindow()
{
	memset( m_tbiToolbarDirectory, 0x00, sizeof(m_tbiToolbarDirectory) );
	memset( m_tbiToolbarProject, 0x00, sizeof(m_tbiToolbarProject) );
	memset( m_tbiWinButtons, 0x00, sizeof(m_tbiWinButtons) );

	m_tbiToolbarDirectory[0].iBitmap = 0;
	m_tbiToolbarDirectory[0].idCommand = ID_DIR_ITEM_REFRESH;
	m_tbiToolbarDirectory[0].fsState = TBSTATE_ENABLED;
	m_tbiToolbarDirectory[0].fsStyle = TBSTYLE_BUTTON;

	m_tbiToolbarDirectory[1].fsState = TBSTATE_ENABLED;
	m_tbiToolbarDirectory[1].fsStyle = TBSTYLE_SEP;

	m_tbiToolbarDirectory[2].iBitmap = 1;
	m_tbiToolbarDirectory[2].idCommand = ID_DIR_ITEM_COPY;
	m_tbiToolbarDirectory[2].fsState = TBSTATE_ENABLED;
	m_tbiToolbarDirectory[2].fsStyle = TBSTYLE_BUTTON;

	m_tbiToolbarDirectory[3].iBitmap = 2;
	m_tbiToolbarDirectory[3].idCommand = ID_DIR_ITEM_MOVE;
	m_tbiToolbarDirectory[3].fsState = TBSTATE_ENABLED;
	m_tbiToolbarDirectory[3].fsStyle = TBSTYLE_BUTTON;

	m_tbiToolbarDirectory[4].iBitmap = 3;
	m_tbiToolbarDirectory[4].idCommand = ID_DIR_ITEM_DELETE;
	m_tbiToolbarDirectory[4].fsState = TBSTATE_ENABLED;
	m_tbiToolbarDirectory[4].fsStyle = TBSTYLE_BUTTON;

	m_tbiToolbarDirectory[5].fsState = TBSTATE_ENABLED;
	m_tbiToolbarDirectory[5].fsStyle = TBSTYLE_SEP;


	m_tbiToolbarProject[0].iBitmap = 0;
	m_tbiToolbarProject[0].idCommand = ID_PRJ_NEW_CATEGORY;
	m_tbiToolbarProject[0].fsState = TBSTATE_ENABLED;
	m_tbiToolbarProject[0].fsStyle = TBSTYLE_BUTTON;

	m_tbiToolbarProject[1].fsState = TBSTATE_ENABLED;
	m_tbiToolbarProject[1].fsStyle = TBSTYLE_SEP;

	m_tbiToolbarProject[2].iBitmap = 1;
	m_tbiToolbarProject[2].idCommand = ID_PRJ_ADD_FILES_TO;
	m_tbiToolbarProject[2].fsState = TBSTATE_ENABLED;
	m_tbiToolbarProject[2].fsStyle = TBSTYLE_BUTTON;

	m_tbiToolbarProject[3].iBitmap = 2;
	m_tbiToolbarProject[3].idCommand = ID_PRJ_ADD_ACTIVE_FILE;
	m_tbiToolbarProject[3].fsState = TBSTATE_ENABLED;
	m_tbiToolbarProject[3].fsStyle = TBSTYLE_BUTTON;

	m_tbiToolbarProject[4].iBitmap = 3;
	m_tbiToolbarProject[4].idCommand = ID_PRJ_ADD_OPEN_FILES;
	m_tbiToolbarProject[4].fsState = TBSTATE_ENABLED;
	m_tbiToolbarProject[4].fsStyle = TBSTYLE_BUTTON;

	m_tbiToolbarProject[5].fsState = TBSTATE_ENABLED;
	m_tbiToolbarProject[5].fsStyle = TBSTYLE_SEP;

	m_tbiToolbarProject[6].iBitmap = 4;
	m_tbiToolbarProject[6].idCommand = ID_PRJ_ITEM_REMOVE;
	m_tbiToolbarProject[6].fsState = TBSTATE_ENABLED;
	m_tbiToolbarProject[6].fsStyle = TBSTYLE_BUTTON;

	m_tbiToolbarProject[7].fsState = TBSTATE_ENABLED;
	m_tbiToolbarProject[7].fsStyle = TBSTYLE_SEP;


	m_tbiWinButtons[0].iBitmap = 0;
	m_tbiWinButtons[0].idCommand = ID_FILE_WINDOW_SYNC;
	m_tbiWinButtons[0].fsState = TBSTATE_ENABLED;
	m_tbiWinButtons[0].fsStyle = TBSTYLE_BUTTON;

	m_tbiWinButtons[1].iBitmap = 1;
	m_tbiWinButtons[1].idCommand = ID_FILE_WINDOW_HIDE;
	m_tbiWinButtons[1].fsState = TBSTATE_ENABLED;
	m_tbiWinButtons[1].fsStyle = TBSTYLE_BUTTON;

	m_fcnCallbackSelchangeFileFilter = NULL;
}

CFileWindow::~CFileWindow()
{
	m_imgToolbarDirectory.Detach();
	m_imgToolbarProject.Detach();

	m_imgWinButtons.Detach();
	m_imgCategoryTab.Detach();

	m_imgDirectoryTree.Detach();
	m_imgProjectTree.Detach();
}


BEGIN_MESSAGE_MAP(CFileWindow, CSizingControlBar)
	//{{AFX_MSG_MAP(CFileWindow)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_FWIN_CATEGORY_TAB, OnSelchangeCategoryTab)
	ON_CBN_DROPDOWN(IDC_FWIN_LOCAL_DRIVE, OnDropdownLocalDrive)
	ON_CBN_SELCHANGE(IDC_FWIN_LOCAL_DRIVE, OnSelchangeLocalDrive)
	ON_CBN_DROPDOWN(IDC_FWIN_FILE_FILTER, OnDropdownFileFilter)
	ON_CBN_SELCHANGE(IDC_FWIN_FILE_FILTER, OnSelchangeFileFilter)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_FWIN_DIRECTORY_TREE, OnItemexpandingDirectoryTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_FWIN_DIRECTORY_TREE, OnSelchangedDirectoryTree)
	ON_NOTIFY(NM_CLICK, IDC_FWIN_DIRECTORY_TREE, OnClickDirectoryTree)
	ON_NOTIFY(NM_DBLCLK, IDC_FWIN_DIRECTORY_TREE, OnDblclkDirectoryTree)
	ON_NOTIFY(NM_RCLICK, IDC_FWIN_DIRECTORY_TREE, OnRclickDirectoryTree)
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_FWIN_DIRECTORY_TREE, OnBeginlabeleditDirectoryTree)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_FWIN_DIRECTORY_TREE, OnEndlabeleditDirectoryTree)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_FWIN_DIRECTORY_TREE, OnBegindragDirectoryTree)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_FWIN_PROJECT_TREE, OnItemexpandingProjectTree)
	ON_NOTIFY(NM_CLICK, IDC_FWIN_PROJECT_TREE, OnClickProjectTree)
	ON_NOTIFY(NM_DBLCLK, IDC_FWIN_PROJECT_TREE, OnDblclkProjectTree)
	ON_NOTIFY(NM_RCLICK, IDC_FWIN_PROJECT_TREE, OnRclickProjectTree)
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_FWIN_PROJECT_TREE, OnBeginlabeleditProjectTree)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_FWIN_PROJECT_TREE, OnEndlabeleditProjectTree)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_FWIN_PROJECT_TREE, OnBegindragProjectTree)
	ON_COMMAND(ID_FILE_WINDOW_OPEN, OnFileWindowOpen)
	ON_COMMAND(ID_FILE_WINDOW_DELETE, OnFileWindowDelete)
	ON_COMMAND(ID_FILE_WINDOW_RENAME, OnFileWindowRename)
	ON_COMMAND(ID_FILE_WINDOW_REFRESH, OnFileWindowRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFileWindow message handlers

int CFileWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CRect rect(0, 0, 1, 1); DWORD dwStyle;
	if (CSizingControlBar::OnCreate(lpCreateStruct) == -1) return -1;

	// register this window as drop target
	m_oleDropTarget.Register(this);

	// create controls
	dwStyle = WS_VISIBLE | WS_CHILD | CCS_NORESIZE | CCS_NODIVIDER | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS;
	m_btnToolbarDirectory.Create(dwStyle, rect, this, IDC_FWIN_TOOLBAR_DIRECTORY);
	m_btnToolbarProject.Create(dwStyle, rect, this, IDC_FWIN_TOOLBAR_PROJECT);
	m_btnWinButtons.Create(dwStyle, rect, this, IDC_FWIN_TOOLBAR_BUTTONS);

	// directory controls
	dwStyle = WS_VISIBLE | WS_CHILD | WS_VSCROLL | CBS_DROPDOWNLIST;
	m_cmbLocalDrive.Create(dwStyle, rect, this, IDC_FWIN_LOCAL_DRIVE);

	dwStyle = WS_VISIBLE | WS_CHILD /* | TVS_HASLINES */ | TVS_EDITLABELS /* | TVS_LINESATROOT */ | TVS_HASBUTTONS | TVS_SHOWSELALWAYS;
	m_treDirectoryTree.Create(dwStyle, rect, this, IDC_FWIN_DIRECTORY_TREE);
	m_treDirectoryTree.ModifyStyleEx(0, WS_EX_CLIENTEDGE, 0);

	dwStyle = WS_VISIBLE | WS_CHILD | WS_VSCROLL | CBS_DROPDOWNLIST;
	m_cmbFileFilter.Create(dwStyle, rect, this, IDC_FWIN_FILE_FILTER);

	// remote controls
	dwStyle = WS_VISIBLE | WS_CHILD | WS_VSCROLL | CBS_DROPDOWNLIST;
	m_cmbFtpAccount.Create(dwStyle, rect, this, IDC_FWIN_FTP_ACCOUNT);

	dwStyle = WS_VISIBLE | WS_CHILD | TVS_HASLINES /* | TVS_EDITLABELS | TVS_LINESATROOT */ | TVS_HASBUTTONS | TVS_SHOWSELALWAYS;
	m_treRemoteTree.Create(dwStyle, rect, this, IDC_FWIN_REMOTE_TREE);
	m_treRemoteTree.ModifyStyleEx(0, WS_EX_CLIENTEDGE, 0);

	// project controls
	dwStyle = WS_VISIBLE | WS_CHILD /* | TVS_HASLINES */ | TVS_EDITLABELS /* | TVS_LINESATROOT */ | TVS_HASBUTTONS | TVS_SHOWSELALWAYS;
	m_treProjectTree.Create(dwStyle, rect, this, IDC_FWIN_PROJECT_TREE);
	m_treProjectTree.ModifyStyleEx(0, WS_EX_CLIENTEDGE, 0);

	// category
	dwStyle = WS_VISIBLE | WS_CHILD | TCS_FOCUSNEVER | TCS_BOTTOM | TCS_HOTTRACK;
	m_tabCategory.Create(dwStyle, rect, this, IDC_FWIN_CATEGORY_TAB);
//	m_tabCategory.ModifyStyleEx(0, WS_EX_STATICEDGE, 0);

	// set image list
	m_imgToolbarDirectory.Create(IDB_FWIN_TOOLBAR_DIRECTORY, 16, 0, RGB(255, 0, 255));
	m_btnToolbarDirectory.SetImageList( & m_imgToolbarDirectory );

	m_imgToolbarProject.Create(IDB_FWIN_TOOLBAR_PROJECT, 16, 0, RGB(255, 0, 255));
	m_btnToolbarProject.SetImageList( & m_imgToolbarProject );

	m_imgWinButtons.Create(IDB_WIN_BUTTONS, 12, 0, RGB(255, 0, 255));
	m_btnWinButtons.SetImageList( & m_imgWinButtons );

	m_imgCategoryTab.Create(IDB_FWIN_CATEGORY_TAB, 16, 0, RGB(255, 0, 255));
	m_tabCategory.SetImageList( & m_imgCategoryTab );

	InitDirectoryImageList( m_imgDirectoryTree );
	m_treDirectoryTree.SetImageList( & m_imgDirectoryTree, TVSIL_NORMAL );

	m_imgProjectTree.Create(IDB_PROJECT_ITEM, 16, 0, RGB(255, 0, 255));
	m_treProjectTree.SetImageList( & m_imgProjectTree, TVSIL_NORMAL );

	// set control fonts
	CFont * pFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	LOGFONT lf; pFont->GetLogFont( & lf ); m_fontControl.CreateFontIndirect( & lf );

	m_cmbLocalDrive.SetFont( & m_fontControl, FALSE );	m_treDirectoryTree.SetFont( & m_fontControl, FALSE );
	m_cmbFileFilter.SetFont( & m_fontControl, FALSE );	m_tabCategory.SetFont( & m_fontControl, FALSE );

	m_cmbFtpAccount.SetFont( & m_fontControl, FALSE );	m_treRemoteTree.SetFont( & m_fontControl, FALSE );
	m_treProjectTree.SetFont( & m_fontControl, FALSE );

	// initialize toolbar
	m_btnToolbarDirectory.AddButtons( 6, m_tbiToolbarDirectory );
	m_btnToolbarProject.AddButtons( 8, m_tbiToolbarProject );
	m_btnWinButtons.AddButtons( 2, m_tbiWinButtons );

	// initialize tab control
	CString szCategoryName;
	szCategoryName.LoadString( IDS_FILE_WINDOW_DIRECTORY ); 
	m_tabCategory.InsertItem( FILE_WINDOW_DIRECTORY, szCategoryName, 0 );
//	szCategoryName.LoadString( IDS_FILE_WINDOW_REMOTE );
//	m_tabCategory.InsertItem( FILE_WINDOW_REMOTE, szCategoryName, 1 );
	szCategoryName.LoadString( IDS_FILE_WINDOW_PROJECT );
	m_tabCategory.InsertItem( FILE_WINDOW_PROJECT, szCategoryName, 2 );

	// set active category
	SetActiveCategory( FILE_WINDOW_DIRECTORY );

	// load accelerators
	CCedtApp * pApp = (CCedtApp *)AfxGetApp();
	m_hAccel = LoadAccelerators(pApp->m_hInstance, MAKEINTRESOURCE(IDR_FILE_WINDOW));

	// set label editing flag
	m_bLabelEditing = FALSE;

	return 0;
}

void CFileWindow::OnDestroy() 
{
	CSizingControlBar::OnDestroy();

	// free memories allocated to each project item
	RemoveAllProjectItems();
}

void CFileWindow::OnSize(UINT nType, int cx, int cy) 
{
	CSizingControlBar::OnSize(nType, cx, cy);

	INT nBegY = 0, nEndY = cy - 6;

	// button width = 22, seperator width = 8
	nBegY +=  2; m_btnToolbarDirectory.MoveWindow(2, nBegY, 106, 22, FALSE);
	nBegY +=  0; m_btnToolbarProject.MoveWindow(2, nBegY, 136, 22, FALSE);

	INT nWidth = (m_tabCategory.GetCurSel() == FILE_WINDOW_DIRECTORY) ? 38 : 19;
	nBegY +=  2; m_btnWinButtons.MoveWindow(cx-nWidth-2, nBegY, nWidth, 18, FALSE);

	nBegY += 20; m_cmbLocalDrive.MoveWindow(2, nBegY, cx-4, 150, FALSE);
	nBegY +=  0; m_cmbFtpAccount.MoveWindow(2, nBegY, cx-4, 150, FALSE);

	nEndY -=  2; m_tabCategory.MoveWindow(2, nEndY-27, cx-2, 27, FALSE);
	nEndY -= 29; m_cmbFileFilter.MoveWindow(2, nEndY-20, cx-4, 150, FALSE);

	nBegY += 22; nEndY -= 22; m_treDirectoryTree.MoveWindow(2, nBegY, cx-4, nEndY-nBegY, FALSE);
	nBegY +=  0; nEndY -=  0; m_treRemoteTree.MoveWindow(2, nBegY, cx-4, nEndY-nBegY, FALSE);
	nBegY -= 22; nEndY += 22; m_treProjectTree.MoveWindow(2, nBegY, cx-4, nEndY-nBegY, FALSE);

	Invalidate();
}

void CFileWindow::OnSelchangeCategoryTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	INT nSelect = m_tabCategory.GetCurSel();

	ShowOrHideDirectoryControls( nSelect );
	ShowOrHideRemoteControls( nSelect );
	ShowOrHideProjectControls( nSelect );
	ShowOrHideCommonControls( nSelect );

	*pResult = 0;
}

DROPEFFECT CFileWindow::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	TRACE0("CFileWindow::OnDragEnter\n");
	m_nDragCategory = GetActiveCategory();

	m_tabCategory.GetWindowRect( & m_rectDragCategory );
	ScreenToClient( & m_rectDragCategory );

	if( pDataObject->IsDataAvailable(CF_HDROP) ) m_nDragObjectType = DRAG_OBJECT_HDROP;
	else if( pDataObject->IsDataAvailable(g_uClipbrdFormatProjectItem) ) m_nDragObjectType = DRAG_OBJECT_PROJECT_ITEM;
	else if( pDataObject->IsDataAvailable(g_uClipbrdFormatFileTabItem) ) m_nDragObjectType = DRAG_OBJECT_FILETAB_ITEM;
	else m_nDragObjectType = DRAG_OBJECT_UNKNOWN;

	if( m_nDragCategory == FILE_WINDOW_DIRECTORY ) {
		m_treDirectoryTree.GetWindowRect( & m_rectDragBound );
		ScreenToClient( & m_rectDragBound );

		if( m_rectDragBound.PtInRect( point ) ) {
			return OnDragOverDirectoryTree(pDataObject, dwKeyState, point);
		} else return DROPEFFECT_NONE;

	} else if( m_nDragCategory == FILE_WINDOW_PROJECT ) {
		m_treProjectTree.GetWindowRect( & m_rectDragBound );
		ScreenToClient( & m_rectDragBound );

		if( m_rectDragBound.PtInRect( point ) ) {
			return OnDragOverProjectTree(pDataObject, dwKeyState, point);
		} else return DROPEFFECT_NONE;

	} else return DROPEFFECT_NONE;
}


DROPEFFECT CFileWindow::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
//	TRACE0("CFileWindow::OnDragOver\n");

	if( m_rectDragCategory.PtInRect(point) ) {

		if( m_nDragCategory == FILE_WINDOW_DIRECTORY ) {
			m_treDirectoryTree.SelectDropTarget(NULL);
		} else if( m_nDragCategory == FILE_WINDOW_PROJECT ) {
			m_treProjectTree.SelectDropTarget(NULL);
		}

		if( m_nDragObjectType != DRAG_OBJECT_HDROP &&
			m_nDragObjectType != DRAG_OBJECT_PROJECT_ITEM &&
			m_nDragObjectType != DRAG_OBJECT_FILETAB_ITEM ) return DROPEFFECT_NONE;

		TC_HITTESTINFO info; info.pt = point; 
		ClientToScreen( & info.pt ); m_tabCategory.ScreenToClient( & info.pt );

		INT nTab = m_tabCategory.HitTest( & info );
		if( nTab == -1 || nTab == m_nDragCategory ) return DROPEFFECT_NONE;

		SetActiveCategory(nTab);
		m_nDragCategory = nTab;

		if( m_nDragCategory == FILE_WINDOW_DIRECTORY ) {
			m_treDirectoryTree.GetWindowRect( & m_rectDragBound );
			ScreenToClient( & m_rectDragBound );
		} else if( m_nDragCategory == FILE_WINDOW_PROJECT ) {
			m_treProjectTree.GetWindowRect( & m_rectDragBound );
			ScreenToClient( & m_rectDragBound );
		}

		return DROPEFFECT_NONE;

	} else if( m_nDragCategory == FILE_WINDOW_DIRECTORY ) {
		if( m_rectDragBound.PtInRect( point ) ) {
			return OnDragOverDirectoryTree(pDataObject, dwKeyState, point);
		} else {
			m_treDirectoryTree.SelectDropTarget(NULL); 
			return DROPEFFECT_NONE;
		}

	} else if( m_nDragCategory == FILE_WINDOW_PROJECT ) {
		if( m_rectDragBound.PtInRect( point ) ) {
			return OnDragOverProjectTree(pDataObject, dwKeyState, point);
		} else {
			m_treProjectTree.SelectDropTarget(NULL); 
			return DROPEFFECT_NONE;
		}

	} else return DROPEFFECT_NONE;
}

void CFileWindow::OnDragLeave()
{
	TRACE0("CFileWindow::OnDragLeave\n");

	if( m_nDragCategory == FILE_WINDOW_DIRECTORY ) {
		m_treDirectoryTree.SelectDropTarget(NULL);
	} else if( m_nDragCategory == FILE_WINDOW_PROJECT ) {
		m_treProjectTree.SelectDropTarget(NULL);
	}
}

BOOL CFileWindow::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	TRACE0("CFileWindow::OnDrop\n");
	BOOL bResult = FALSE;

	if( m_nDragCategory == FILE_WINDOW_DIRECTORY ) {
		bResult = OnDropDirectoryTree(pDataObject, dropEffect, point);
		m_treDirectoryTree.SelectDropTarget(NULL);
	} else if( m_nDragCategory == FILE_WINDOW_PROJECT ) {
		bResult = OnDropProjectTree(pDataObject, dropEffect, point);
		m_treProjectTree.SelectDropTarget(NULL);
	} else bResult = FALSE;

	return bResult;
}


/////////////////////////////////////////////////////////////////////////////
// CFileWindow member functions

INT CFileWindow::GetActiveCategory()
{
	return m_tabCategory.GetCurSel();
}

void CFileWindow::SetActiveCategory(INT nSelect)
{
	m_tabCategory.SetCurSel( nSelect );

	ShowOrHideDirectoryControls( nSelect );
	ShowOrHideRemoteControls( nSelect );
	ShowOrHideProjectControls( nSelect );
	ShowOrHideCommonControls( nSelect );
}

void CFileWindow::ShowOrHideCommonControls(INT nSelect)
{
	CRect rect; GetClientRect( & rect );

	BOOL bShow = (nSelect == FILE_WINDOW_DIRECTORY) ? TRUE : FALSE;
	m_btnWinButtons.HideButton( ID_FILE_WINDOW_SYNC, ! bShow );

	INT nWidth = (nSelect == FILE_WINDOW_DIRECTORY) ? 38 : 19;
	m_btnWinButtons.MoveWindow(rect.right-nWidth-2, 4, nWidth, 18, TRUE);

	INT nCmdShow = ( (nSelect == FILE_WINDOW_DIRECTORY) || (nSelect == FILE_WINDOW_REMOTE) ) ? SW_SHOW : SW_HIDE;
	m_cmbFileFilter.ShowWindow( nCmdShow );
}

void CFileWindow::ShowOrHideDirectoryControls(INT nSelect)
{
	INT nCmdShow = (nSelect == FILE_WINDOW_DIRECTORY) ? SW_SHOW : SW_HIDE;

	m_btnToolbarDirectory.ShowWindow( nCmdShow );
	m_cmbLocalDrive.ShowWindow( nCmdShow );
	m_treDirectoryTree.ShowWindow( nCmdShow );
}

void CFileWindow::ShowOrHideRemoteControls(INT nSelect)
{
	INT nCmdShow = (nSelect == FILE_WINDOW_REMOTE) ? SW_SHOW : SW_HIDE;

	m_cmbFtpAccount.ShowWindow( nCmdShow );
	m_treRemoteTree.ShowWindow( nCmdShow );
}

void CFileWindow::ShowOrHideProjectControls(INT nSelect)
{
	INT nCmdShow = (nSelect == FILE_WINDOW_PROJECT) ? SW_SHOW : SW_HIDE;

	m_btnToolbarProject.ShowWindow( nCmdShow );
	m_treProjectTree.ShowWindow( nCmdShow );
}

BOOL CFileWindow::InitDirectoryImageList( CImageList & imglst )
{
	SHFILEINFO shFinfo; HIMAGELIST hImageList = NULL;
	hImageList = (HIMAGELIST)SHGetFileInfo("", 0, &shFinfo, sizeof(shFinfo), SHGFI_SMALLICON | SHGFI_SYSICONINDEX);
	if ( ! hImageList ) return FALSE;

	if( imglst.Detach() ) return FALSE;
	imglst.Attach( hImageList );

	return TRUE;
}


BOOL CFileWindow::PreTranslateMessage(MSG* pMsg) 
{
	// do not call CSizingControlBar::PreTranslateMessage()
	// file window should have separate message processing routine

	if( m_bLabelEditing || ! TranslateAccelerator(m_hWnd, m_hAccel, pMsg) ) {
		TranslateMessage( pMsg );
		DispatchMessage( pMsg );
	}

	return TRUE;
}

void CFileWindow::OnFileWindowOpen() 
{
	switch( GetActiveCategory() ) {
	case FILE_WINDOW_DIRECTORY:
		PostMessage( WM_COMMAND, ID_DIR_ITEM_OPEN, 0L );
		break;
	case FILE_WINDOW_PROJECT:
		PostMessage( WM_COMMAND, ID_PRJ_ITEM_OPEN, 0L );
		break;
	}
}

void CFileWindow::OnFileWindowDelete() 
{
	switch( GetActiveCategory() ) {
	case FILE_WINDOW_DIRECTORY:
		PostMessage( WM_COMMAND, ID_DIR_ITEM_DELETE, 0L );
		break;
	case FILE_WINDOW_PROJECT:
		PostMessage( WM_COMMAND, ID_PRJ_ITEM_REMOVE, 0L );
		break;
	}
}

void CFileWindow::OnFileWindowRename() 
{
	switch( GetActiveCategory() ) {
	case FILE_WINDOW_DIRECTORY:
		PostMessage( WM_COMMAND, ID_DIR_ITEM_RENAME, 0L );
		break;
	case FILE_WINDOW_PROJECT:
		PostMessage( WM_COMMAND, ID_PRJ_ITEM_RENAME, 0L );
		break;
	}
}

void CFileWindow::OnFileWindowRefresh() 
{
	switch( GetActiveCategory() ) {
	case FILE_WINDOW_DIRECTORY:
		PostMessage( WM_COMMAND, ID_DIR_ITEM_REFRESH, 0L );
		break;
	}
}

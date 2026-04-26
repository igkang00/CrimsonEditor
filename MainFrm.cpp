// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "cedtHeader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_FILE_TAB, OnViewFileTab)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FILE_TAB, OnUpdateViewFileTab)
	ON_WM_ACTIVATEAPP()
	ON_WM_CLOSE()
	ON_COMMAND(ID_VIEW_DIRECTORY_WINDOW, OnViewDirectoryWindow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DIRECTORY_WINDOW, OnUpdateViewDirectoryWindow)
	ON_COMMAND(ID_VIEW_PROJECT_WINDOW, OnViewProjectWindow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROJECT_WINDOW, OnUpdateViewProjectWindow)
	ON_COMMAND(ID_VIEW_OUTPUT_WINDOW, OnViewOutputWindow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUT_WINDOW, OnUpdateViewOutputWindow)
	ON_COMMAND(ID_WINDOW_LAST_VISITED, OnWindowLastVisited)
	ON_COMMAND(ID_WINDOW_NEXT, OnWindowNext)
	ON_COMMAND(ID_WINDOW_PREV, OnWindowPrev)
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_WINDOW_HIDE, OnFileWindowHide)
	ON_COMMAND(ID_FILE_WINDOW_SYNC, OnFileWindowSync)
	ON_COMMAND(ID_OUTPUT_WINDOW_HIDE, OnOutputWindowHide)
	ON_COMMAND(ID_OUTPUT_WINDOW_CLEAR, OnOutputWindowClear)
	ON_COMMAND(ID_OUTPUT_WINDOW_COPY_ALL, OnOutputWindowCopyAll)
	ON_COMMAND(ID_OUTPUT_WINDOW_COPY, OnOutputWindowCopy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators0[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_POS,
	ID_INDICATOR_MAX,
	ID_INDICATOR_FORMAT,
	ID_INDICATOR_READONLY,
	ID_INDICATOR_REC,
	ID_INDICATOR_COL,
	ID_INDICATOR_OVR,
};

static UINT indicators1[] =
{
	ID_SEPARATOR,           // status line indicator
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_bSwitchingChildFrame = FALSE;
	m_bPrintPreviewMode = FALSE;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CString szText;

	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// register this window as drop target
	m_oleDropTarget.Register(this);

	if ( ! m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		 /* | CBRS_GRIPPER */ | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		 ! m_wndToolBar.LoadToolBar(IDR_MAINFRAME) ) {
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if ( ! m_wndStatusBar.Create(this) ||
		 ! m_wndStatusBar.SetIndicators(indicators0, sizeof(indicators0)/sizeof(UINT)) ) {
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	szText.LoadString(IDS_TOOL_TOOLBAR);
	m_wndToolBar.SetWindowText(szText);

	// File Selector
	if ( ! m_wndFileTab.Create(this) ) {
		TRACE0("Failed to create file tab\n");
		return -1;
	}

	m_wndFileTab.EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM);
	DockControlBar(&m_wndFileTab, AFX_IDW_DOCKBAR_TOP);
	szText.LoadString(IDS_TOOL_FILE_TAB);
	m_wndFileTab.SetWindowText(szText);

	// File Window
	if ( ! m_wndFileWindow.Create(this, CSize(200, 480), ID_FILE_WINDOW, WS_CHILD | CBRS_LEFT) ) {
		TRACE0("Failed to create file window\n");
		return -1;
	}

	m_wndFileWindow.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
	DockControlBar( & m_wndFileWindow, AFX_IDW_DOCKBAR_LEFT );
	szText.LoadString(IDS_TOOL_FILE_WINDOW);
	m_wndFileWindow.SetWindowText(szText);

	// Output Window
	if ( ! m_wndOutputWindow.Create(this, CSize(640, 100), ID_OUTPUT_WINDOW, WS_CHILD | CBRS_BOTTOM) ) {
		TRACE0("Failed to create output window\n");
		return -1;
	}

	m_wndOutputWindow.EnableDocking(CBRS_ALIGN_BOTTOM | CBRS_ALIGN_TOP);
	DockControlBar( & m_wndOutputWindow, AFX_IDW_DOCKBAR_BOTTOM );
	szText.LoadString(IDS_TOOL_OUTPUT_WINDOW);
	m_wndOutputWindow.SetWindowText(szText);

	// Status Bar
	m_wndStatusBar.SetPaneStyle(1, SBPS_DISABLED); // OWNER DRAW PANE
	m_wndStatusBar.SetPaneStyle(2, SBPS_DISABLED); // OWNER DRAW PANE
	m_wndStatusBar.SetPaneStyle(3, SBPS_DISABLED); // OWNER DRAW PANE
	m_wndStatusBar.SetPaneStyle(4, SBPS_DISABLED); // OWNER DRAW PANE
	m_wndStatusBar.SetPaneStyle(5, SBPS_DISABLED); // OWNER DRAW PANE
	m_wndStatusBar.SetPaneStyle(6, SBPS_DISABLED); // OWNER DRAW PANE
	m_wndStatusBar.SetPaneStyle(7, SBPS_DISABLED); // OWNER DRAW PANE

	m_wndStatusBar.SetPaneTextColor(1, RGB(  0,   0, 128)); // POS
	m_wndStatusBar.SetPaneTextColor(2, RGB(  0,   0, 128)); // MAX
	m_wndStatusBar.SetPaneTextColor(3, RGB(  0,   0, 128)); // FORMAT
	m_wndStatusBar.SetPaneTextColor(4, RGB(128, 128, 128)); // READONLY
	m_wndStatusBar.SetPaneTextColor(5, RGB(160,  64,  64)); // REC
	m_wndStatusBar.SetPaneTextColor(6, RGB( 64, 160,  64)); // COL
	m_wndStatusBar.SetPaneTextColor(7, RGB( 64,  64, 160)); // OVR

	SetCaretPositionInfo(0, 0, 0);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return; // leave it alone!

	UpdateFrameTitleForDocument(NULL);
}

void CMainFrame::OnSizeMainWindow(int cx, int cy)
{
	// when print preview is closed direct call to CCedtView::OnSizeMainWindow()
	// causes program to crash...
	if( ! m_bPrintPreviewMode ) CCedtView::OnSizeMainWindow(cx, cy);

	// we need to update all the views again...
	CCedtApp * pApp = (CCedtApp *)AfxGetApp(); ASSERT( pApp );
	pApp->UpdateAllViews();
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIFrameWnd::OnSize(nType, cx, cy);

	// only call when it is not in print preview mode to avoid program crash
	if( ! m_bPrintPreviewMode ) CCedtView::OnSizeMainWindow(cx, cy);
}

void CMainFrame::OnSetPreviewMode(BOOL bPreview, CPrintPreviewState* pState) 
{
	CMDIFrameWnd::OnSetPreviewMode(bPreview, pState);
	m_bPrintPreviewMode = bPreview;

	if( bPreview ) {
		m_wndStatusBar.SetIndicators(indicators1, sizeof(indicators1)/sizeof(UINT));
	} else {
		m_wndStatusBar.SetIndicators(indicators0, sizeof(indicators0)/sizeof(UINT));

		CRect rect; GetWindowRect( & rect );
		PostMessage(WM_SIZE_MAIN_WINDOW, 0, MAKELONG(rect.Width(), rect.Height()));
	}
}

void CMainFrame::OnClose() 
{
	// user can *CANCEL* termination of mainframe window if there is any 
	// unsaved document and select *CANCEL* from dialog box
	// so, don't call any termination procedure in this function.
	if( ! m_bPrintPreviewMode ) {
		// Save window placement & bar state
		SaveBarState(REGKEY_BAR_STATE);
		SaveWindowPlacement(REGKEY_WINDOW_PLACEMENT);

		CCedtApp::SaveUserConfiguration(CCedtApp::m_szAppDataDirectory + "\\cedt.conf");

		CCedtApp * pApp = (CCedtApp *)AfxGetApp(); // get application pointer
		pApp->SaveBrowsingDirectory(REGKEY_BROWSING_DIRECTORY);

		// current working directory might differ from browsing directory
		pApp->SaveWorkingDirectory(REGKEY_WORKING_DIRECTORY);

		if( CCedtApp::m_bFirstInstance ) {
			// save workspace file path
			pApp->SaveWorkspaceFilePath(REGKEY_LAST_WORKSPACE);

			// save working file list and user configuration
			if( pApp->IsProjectLoaded() ) pApp->SaveProjectWorkspace();
			else pApp->SaveRegularWorkspace();
		}
	}

	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnActivateApp(BOOL bActive, HTASK hTask) 
{
	CMDIFrameWnd::OnActivateApp(bActive, hTask);

	// check if files modified outside
	if( bActive && CCedtApp::m_bCheckIfFilesModifiedOutside ) PostMessage(WM_COMMAND, ID_FILE_CHECK_MODIFIED, 0L);
}

void CMainFrame::RecalcLayout(BOOL bNotify) 
{
	CMDIFrameWnd::RecalcLayout(bNotify);
	CMDIFrameWnd::RecalcLayout(bNotify);
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	static INT nCount;

	switch( pMsg->message ) {
	case WM_KEYDOWN:
		switch( pMsg->wParam ) {
		case VK_CONTROL:
			m_bSwitchingChildFrame = TRUE;
			nCount = 0;
			break;

		case VK_TAB:
			if( GetKeyState(VK_CONTROL) & 0xF0 ) {
				ActivateChildFrameInZOrder(++nCount);
				return TRUE;
			}
			break;

		case VK_NEXT:
			if( GetKeyState(VK_CONTROL) & 0xF0 ) {
				ActivateNextChildFrame();
				return TRUE;
			}
			break;

		case VK_PRIOR:
			if( GetKeyState(VK_CONTROL) & 0xF0 ) {
				ActivatePrevChildFrame();
				return TRUE;
			}
			break;

		case VK_F5:
			if( GetKeyState(VK_CONTROL) & 0xF0 ) {
				if( GetKeyState(VK_SHIFT) & 0xF0 ) {
					ActivateNextChildFrame();
					return TRUE;
				} else {
					ActivatePrevChildFrame();
					return TRUE;
				} 
			} 
			break;

		case VK_F6:
			if( GetKeyState(VK_CONTROL) & 0xF0 ) {
				if( GetKeyState(VK_SHIFT) & 0xF0 ) {
					ActivatePrevChildFrame();
					return TRUE;
				} else {
					ActivateNextChildFrame();
					return TRUE;
				} 
			} 
			break;
		}
		break;

	case WM_KEYUP:
		switch( pMsg->wParam ) {
		case VK_CONTROL:
			m_bSwitchingChildFrame = FALSE;
			BringChildFrameToTop(NULL);
			break;
		}
		break;

	case WM_SIZE_MAIN_WINDOW:
		OnSizeMainWindow(LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
		return TRUE;
	}
	
	return CMDIFrameWnd::PreTranslateMessage(pMsg);
}


// drag and drop
DROPEFFECT CMainFrame::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	TRACE0("CMainFrame::OnDragEnter\n");

	if( pDataObject->IsDataAvailable(CF_TEXT) ) {
		// set drag object type
		m_nDragObjectType = DRAG_OBJECT_TEXT;

		CRect rect; GetClientRect( & rect );
		if( rect.PtInRect( point ) && ! IsPointInControlBars( point ) ) {
			if( dwKeyState & MK_CONTROL ) return DROPEFFECT_COPY;
			else return DROPEFFECT_MOVE;
		} else return DROPEFFECT_NONE;

	} else if( pDataObject->IsDataAvailable(CF_HDROP) ) {
		// set drag object type
		m_nDragObjectType = DRAG_OBJECT_HDROP;

		return DROPEFFECT_COPY;

	} else if( pDataObject->IsDataAvailable(g_uClipbrdFormatProjectItem) ) {
		// set drag object type
		m_nDragObjectType = DRAG_OBJECT_PROJECT_ITEM;

		return DROPEFFECT_COPY;

	} else {
		// set drag object type
		m_nDragObjectType = DRAG_OBJECT_UNKNOWN;

		return DROPEFFECT_NONE;
	}
}

DROPEFFECT CMainFrame::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
//	TRACE0("CMainFrame::OnDragOver\n");

	if( m_nDragObjectType == DRAG_OBJECT_TEXT ) {
		CRect rect; GetClientRect( & rect );
		if( rect.PtInRect( point ) && ! IsPointInControlBars( point ) ) {
			if( dwKeyState & MK_CONTROL ) return DROPEFFECT_COPY;
			else return DROPEFFECT_MOVE;
		} else return DROPEFFECT_NONE;

	} else if( m_nDragObjectType == DRAG_OBJECT_HDROP ) {
		return DROPEFFECT_COPY;

	} else if( m_nDragObjectType == DRAG_OBJECT_PROJECT_ITEM ) {
		return DROPEFFECT_COPY;

	} else {
		return DROPEFFECT_NONE;
	}
}

void CMainFrame::OnDragLeave()
{
	TRACE0("CMainFrame::OnDragLeave\n");
}

BOOL CMainFrame::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	TRACE0("CMainFrame::OnDrop\n");

	if( pDataObject->IsDataAvailable(CF_TEXT) ) {

		if( dropEffect == DROPEFFECT_COPY || dropEffect == DROPEFFECT_MOVE ) {
			HGLOBAL hMemory = pDataObject->GetGlobalData(CF_TEXT);
			if( ! hMemory ) return FALSE;

			if( ! CCedtView::GetGlobalMemoryText(hMemory, CCedtApp::m_clsMemText) ) return FALSE;
			CCedtApp::m_bOpenMemText = TRUE;

			PostMessage( WM_COMMAND, ID_FILE_NEW );
			::GlobalFree(hMemory);

			return TRUE;

		} return FALSE;

	} else if( pDataObject->IsDataAvailable(CF_HDROP) ) {

		if( dropEffect == DROPEFFECT_COPY ) {
			HGLOBAL hMemory = pDataObject->GetGlobalData(CF_HDROP);
			if( ! hMemory ) return FALSE;

			HDROP hDrop = (HDROP)::GlobalLock(hMemory);
			if( ! hDrop ) { ::GlobalUnlock(hMemory); return FALSE; }

			SetActiveWindow(); // activate us first !
			CWinApp * pApp = AfxGetApp(); ASSERT(pApp);

			TCHAR szNextFile[MAX_PATH];
			UINT uNumFiles = DragQueryFile( hDrop, (UINT)-1, NULL, 0 );
			for( UINT uFile = 0; uFile < uNumFiles; uFile++ ) {
				if( DragQueryFile( hDrop, uFile, szNextFile, MAX_PATH ) > 0 ) {
					pApp->OpenDocumentFile(szNextFile);
				}
			}

			::GlobalUnlock(hMemory);
			::GlobalFree(hMemory);

			return TRUE;

		} else return FALSE;

	} else if( pDataObject->IsDataAvailable(g_uClipbrdFormatProjectItem) ) {

		if( dropEffect == DROPEFFECT_COPY ) {
			HGLOBAL hMemory = pDataObject->GetGlobalData(g_uClipbrdFormatProjectItem);
			if( ! hMemory ) return FALSE;

			LPPROJECTITEMINFO lpInfo = (LPPROJECTITEMINFO)::GlobalLock(hMemory);
			if( ! lpInfo ) { ::GlobalUnlock(hMemory); return FALSE; }

			if( lpInfo->nItemType == PROJECT_ITEM_LOCAL_FILE ) {
				SetActiveWindow(); // activate us first !

				CCedtApp * pApp = (CCedtApp *)AfxGetApp(); ASSERT(pApp);
				pApp->OpenDocumentFile(lpInfo->szPathName, 0, NULL);

			} else if( lpInfo->nItemType == PROJECT_ITEM_REMOTE_FILE ) {
				SetActiveWindow(); // activate us first !

				CCedtApp * pApp = (CCedtApp *)AfxGetApp(); ASSERT(pApp);
				pApp->OpenRemoteDocumentFile(lpInfo->nFtpAccount, lpInfo->szPathName, 0, NULL);
			}

			::GlobalUnlock(hMemory);
			::GlobalFree(hMemory);

			return TRUE;

		} else return FALSE;

	} else return FALSE;
}

BOOL CMainFrame::IsPointInControlBars(CPoint point)
{
	CRect rect;

	if( m_wndStatusBar.IsWindowVisible() ) { 
		m_wndStatusBar.GetWindowRect( & rect ); ScreenToClient( & rect );
		if( rect.PtInRect(point) ) return TRUE;
	}
	
	if( m_wndToolBar.IsWindowVisible() ) {
		m_wndToolBar.GetWindowRect( & rect ); ScreenToClient( & rect );
		if( rect.PtInRect( point ) ) return TRUE;
	}
	
	if( m_wndFileTab.IsWindowVisible() ) {
		m_wndFileTab.GetWindowRect( & rect ); ScreenToClient( & rect );
		if( rect.PtInRect( point ) ) return TRUE;
	}
	
	if( m_wndFileWindow.IsWindowVisible() ) {
		m_wndFileWindow.GetWindowRect( & rect ); ScreenToClient( & rect );
		if( rect.PtInRect( point ) ) return TRUE;
	}
	
	if( m_wndOutputWindow.IsWindowVisible() ) {
		m_wndOutputWindow.GetWindowRect( & rect ); ScreenToClient( & rect );
		if( rect.PtInRect( point ) ) return TRUE;
	}

	return FALSE;
}

// menu item refresh
BOOL CMainFrame::RefreshSyntaxTypeMenu()
{
	CMenu * pMainMenu = GetMenu(); if( ! pMainMenu ) return FALSE;
	CMenu * pParentMenu = FindParentMenuContaing(pMainMenu, ID_DOCU_SYNTAX_AUTO); if( ! pParentMenu ) return FALSE;

	INT nInserted = 0, nMenuCount = pParentMenu->GetMenuItemCount();
	for( INT i = nMenuCount-3; i >= 3; i-- ) pParentMenu->DeleteMenu(i, MF_BYPOSITION);

	for( INT j = 0; j < MAX_SYNTAX_TYPE; j++ ) {
		CString & szMenuText = CCedtDoc::m_clsSyntaxTypes[j].m_szDescription;
		if( szMenuText.GetLength() ) {
			pParentMenu->InsertMenu(3 + nInserted, MF_BYPOSITION, ID_DOCU_SYNTAX_TYPE0 + j, szMenuText);
			nInserted++;
		}
	}

	return TRUE;
}

BOOL CMainFrame::RefreshLoadUserToolsMenu()
{
	CMenu * pMainMenu = GetMenu(); if( ! pMainMenu ) return FALSE;
	CMenu * pParentMenu = FindParentMenuContaing(pMainMenu, ID_COMMAND_USER_FILE0); if( ! pParentMenu ) return FALSE;

	INT nMenuCount = pParentMenu->GetMenuItemCount();
	for( INT i = nMenuCount-1; i >= 1; i-- ) pParentMenu->DeleteMenu(i, MF_BYPOSITION);

	for( INT j = 0; j < 8; j++ ) {
		CString szMenuText = GetFileTitle(CCedtView::m_szUserCommandFilePath[j]);
		if( j == 0 ) {
			if( ! szMenuText.GetLength() ) szMenuText.LoadString(IDS_MENU_NO_FILE_AVAILABLE);
			pParentMenu->ModifyMenu(j, MF_BYPOSITION, ID_COMMAND_USER_FILE0, szMenuText);
		} else { 
			if( ! szMenuText.GetLength() ) continue;
			pParentMenu->InsertMenu(j, MF_BYPOSITION, ID_COMMAND_USER_FILE0 + j, szMenuText);
		}
	}

	return TRUE;
}

BOOL CMainFrame::RefreshLoadUserMacrosMenu()
{
	CMenu * pMainMenu = GetMenu(); if( ! pMainMenu ) return FALSE;
	CMenu * pParentMenu = FindParentMenuContaing(pMainMenu, ID_MACRO_USER_FILE0); if( ! pParentMenu ) return FALSE;

	INT nMenuCount = pParentMenu->GetMenuItemCount();
	for( INT i = nMenuCount-1; i >= 1; i-- ) pParentMenu->DeleteMenu(i, MF_BYPOSITION);

	for( INT j = 0; j < 8; j++ ) {
		CString szMenuText = GetFileTitle(CCedtView::m_szMacroBufferFilePath[j]);
		if( j == 0 ) {
			if( ! szMenuText.GetLength() ) szMenuText.LoadString(IDS_MENU_NO_FILE_AVAILABLE);
			pParentMenu->ModifyMenu(j, MF_BYPOSITION, ID_MACRO_USER_FILE0, szMenuText);
		} else {
			if( ! szMenuText.GetLength() ) continue;
			pParentMenu->InsertMenu(j, MF_BYPOSITION, ID_MACRO_USER_FILE0 + j, szMenuText);
		}
	}

	return TRUE;
}

CMenu * CMainFrame::FindParentMenuContaing(CMenu * pParentMenu, UINT nFindMenuID)
{
	INT nMenuCount = pParentMenu->GetMenuItemCount();
	for(INT i = 0; i < nMenuCount; i++) {
		UINT nMenuID = pParentMenu->GetMenuItemID(i);
		if( nMenuID == nFindMenuID ) return pParentMenu;

		if( (INT)nMenuID == -1 ) { // pop-up menu
			CMenu * pMenu = FindParentMenuContaing( pParentMenu->GetSubMenu(i), nFindMenuID );
			if( pMenu ) return pMenu;
		}
	}

	return NULL;
}


// window placement
BOOL CMainFrame::SaveWindowPlacement(LPCTSTR lpszProfileName)
{
	CCedtApp * pApp = (CCedtApp *)AfxGetApp();

	WINDOWPLACEMENT wndpl; wndpl.length = sizeof(wndpl);
	GetWindowPlacement( & wndpl );
	pApp->WriteProfileInt(lpszProfileName, "flags", (INT)wndpl.flags);
	pApp->WriteProfileInt(lpszProfileName, "showCmd", (INT)wndpl.showCmd);
	pApp->WriteProfileInt(lpszProfileName, "minX", wndpl.ptMinPosition.x);
	pApp->WriteProfileInt(lpszProfileName, "minY", wndpl.ptMinPosition.y);
	pApp->WriteProfileInt(lpszProfileName, "maxX", wndpl.ptMaxPosition.x);
	pApp->WriteProfileInt(lpszProfileName, "maxY", wndpl.ptMaxPosition.y);
	pApp->WriteProfileInt(lpszProfileName, "left", wndpl.rcNormalPosition.left);
	pApp->WriteProfileInt(lpszProfileName, "top", wndpl.rcNormalPosition.top);
	pApp->WriteProfileInt(lpszProfileName, "right", wndpl.rcNormalPosition.right);
	pApp->WriteProfileInt(lpszProfileName, "bottom", wndpl.rcNormalPosition.bottom);

	SIZE szHorz, szVert, szFloat;
	szHorz = m_wndFileWindow.GetHorzSize();
	pApp->WriteProfileInt(lpszProfileName, "auxhcx", szHorz.cx);
	pApp->WriteProfileInt(lpszProfileName, "auxhcy", szHorz.cy);
	szVert = m_wndFileWindow.GetVertSize();
	pApp->WriteProfileInt(lpszProfileName, "auxvcx", szVert.cx);
	pApp->WriteProfileInt(lpszProfileName, "auxvcy", szVert.cy);
	szFloat = m_wndFileWindow.GetFloatSize();
	pApp->WriteProfileInt(lpszProfileName, "auxfcx", szFloat.cx);
	pApp->WriteProfileInt(lpszProfileName, "auxfcy", szFloat.cy);

	szHorz = m_wndOutputWindow.GetHorzSize();
	pApp->WriteProfileInt(lpszProfileName, "outhcx", szHorz.cx);
	pApp->WriteProfileInt(lpszProfileName, "outhcy", szHorz.cy);
	szVert = m_wndOutputWindow.GetVertSize();
	pApp->WriteProfileInt(lpszProfileName, "outvcx", szVert.cx);
	pApp->WriteProfileInt(lpszProfileName, "outvcy", szVert.cy);
	szFloat = m_wndOutputWindow.GetFloatSize();
	pApp->WriteProfileInt(lpszProfileName, "outfcx", szFloat.cx);
	pApp->WriteProfileInt(lpszProfileName, "outfcy", szFloat.cy);

	pApp->WriteProfileInt(lpszProfileName, "saved", 1);

	return TRUE;
}

BOOL CMainFrame::LoadWindowPlacement(LPCTSTR lpszProfileName)
{
	CCedtApp * pApp = (CCedtApp *)AfxGetApp();

	if( ! pApp->GetProfileInt(lpszProfileName, "saved", 0) ) return FALSE;

	SIZE szHorz, szVert, szFloat;
	szHorz.cx = pApp->GetProfileInt(lpszProfileName, "auxhcx", 200);
	szHorz.cy = pApp->GetProfileInt(lpszProfileName, "auxhcy", 480);
	m_wndFileWindow.SetHorzSize(szHorz);
	szVert.cx = pApp->GetProfileInt(lpszProfileName, "auxvcx", 200);
	szVert.cy = pApp->GetProfileInt(lpszProfileName, "auxvcy", 480);
	m_wndFileWindow.SetVertSize(szVert);
	szFloat.cx = pApp->GetProfileInt(lpszProfileName, "auxfcx", 200);
	szFloat.cy = pApp->GetProfileInt(lpszProfileName, "auxfcy", 480);
	m_wndFileWindow.SetFloatSize(szFloat);

	szHorz.cx = pApp->GetProfileInt(lpszProfileName, "outhcx", 640);
	szHorz.cy = pApp->GetProfileInt(lpszProfileName, "outhcy",  80);
	m_wndOutputWindow.SetHorzSize(szHorz);
	szVert.cx = pApp->GetProfileInt(lpszProfileName, "outvcx", 640);
	szVert.cy = pApp->GetProfileInt(lpszProfileName, "outvcy",  80);
	m_wndOutputWindow.SetVertSize(szVert);
	szFloat.cx = pApp->GetProfileInt(lpszProfileName, "outfcx", 640);
	szFloat.cy = pApp->GetProfileInt(lpszProfileName, "outfcy",  80);
	m_wndOutputWindow.SetFloatSize(szFloat);

	WINDOWPLACEMENT wndpl; wndpl.length = sizeof(wndpl);
	wndpl.flags = pApp->GetProfileInt(lpszProfileName, "flags", 0);
	wndpl.showCmd = pApp->GetProfileInt(lpszProfileName, "showCmd", 0);
	wndpl.ptMinPosition.x = pApp->GetProfileInt(lpszProfileName, "minX", 0);
	wndpl.ptMinPosition.y = pApp->GetProfileInt(lpszProfileName, "minY", 0);
	wndpl.ptMaxPosition.x = pApp->GetProfileInt(lpszProfileName, "maxX", 0);
	wndpl.ptMaxPosition.y = pApp->GetProfileInt(lpszProfileName, "maxY", 0);
	wndpl.rcNormalPosition.left = pApp->GetProfileInt(lpszProfileName, "left", 0);
	wndpl.rcNormalPosition.top = pApp->GetProfileInt(lpszProfileName, "top", 0);
	wndpl.rcNormalPosition.right = pApp->GetProfileInt(lpszProfileName, "right", 0);
	wndpl.rcNormalPosition.bottom = pApp->GetProfileInt(lpszProfileName, "bottom", 0);
	SetWindowPlacement( & wndpl );

	return TRUE;
}


// status bar
void CMainFrame::SetCaretPositionInfo(INT nLine, INT nCol, INT nMax)
{
	if( m_bPrintPreviewMode ) return;

	CString szPaneText; szPaneText.Format("Ln %d,  Ch %d", nLine, nCol);
	m_wndStatusBar.SetPaneText(1, szPaneText);

	szPaneText.Format("%d", nMax);
	m_wndStatusBar.SetPaneText(2, szPaneText);
}

void CMainFrame::SetFileStatusInfo(INT nEncodingType, INT nFileFormat, BOOL bReadOnly)
{
	if( m_bPrintPreviewMode ) return;

	CString szPaneText = ENCODING_TYPE_DESCRIPTION_SHORT[nEncodingType];
	szPaneText += ", ";
	szPaneText += FILE_FORMAT_DESCRIPTION_SHORT[nFileFormat];

	m_wndStatusBar.SetPaneText(3, szPaneText);
	m_wndStatusBar.SetPaneHighlight(4, bReadOnly);
}

void CMainFrame::SetMacroRecordingFlag(BOOL bRecording)
{
	if( m_bPrintPreviewMode ) return;
	m_wndStatusBar.SetPaneHighlight(5, bRecording);
}

void CMainFrame::SetColumnModeFlag(BOOL bColumnMode)
{
	if( m_bPrintPreviewMode ) return;
	m_wndStatusBar.SetPaneHighlight(6, bColumnMode);
}

void CMainFrame::SetOverwriteFlag(BOOL bOverwrite)
{
	if( m_bPrintPreviewMode ) return;
	m_wndStatusBar.SetPaneHighlight(7, bOverwrite);
}


// MDI file tab
void CMainFrame::InsertMDIFileTab(CMDIChildWnd * pChild, INT nTab)
{
	m_wndFileTab.InsertMDIFileTab(pChild, nTab);
	m_lstChildFrame.AddHead(pChild);
}

void CMainFrame::DeleteMDIFileTab(CMDIChildWnd * pChild)
{
	m_wndFileTab.DeleteMDIFileTab(pChild);
	POSITION pos = m_lstChildFrame.Find(pChild);
	if( pos ) m_lstChildFrame.RemoveAt(pos);
}

void CMainFrame::UpdateMDIFileTab(CMDIChildWnd * pChild)
{
	m_wndFileTab.UpdateMDIFileTab(pChild);
}

void CMainFrame::SetActiveFileTab(CMDIChildWnd * pChild)
{
	m_wndFileTab.SetActiveFileTab(pChild);
	if( ! m_bSwitchingChildFrame ) BringChildFrameToTop(pChild);
}


// file window
void CMainFrame::ShowFileWindowCategory(INT nCategory)
{
	INT nActivCat = m_wndFileWindow.GetActiveCategory();
	BOOL bVisible = m_wndFileWindow.IsWindowVisible();

	if( nActivCat == nCategory ) {
		if( ! bVisible ) ShowControlBar( & m_wndFileWindow, TRUE, FALSE );
	} else {
		if( ! bVisible ) ShowControlBar( & m_wndFileWindow, TRUE, FALSE );
		m_wndFileWindow.SetActiveCategory( nCategory );
	}
}

void CMainFrame::ToggleFileWindowCategory(INT nCategory)
{
	INT nActivCat = m_wndFileWindow.GetActiveCategory();
	BOOL bVisible = m_wndFileWindow.IsWindowVisible();

	if( nActivCat == nCategory ) {
		ShowControlBar( & m_wndFileWindow, ! bVisible, FALSE );
	} else {
		if( ! bVisible ) ShowControlBar( & m_wndFileWindow, TRUE, FALSE );
		m_wndFileWindow.SetActiveCategory( nCategory );
	}
}


// output window
/* All output window related functions are inline functions */


// child window
CDocument * CMainFrame::MDIGetActiveDocument()
{
	CView * pView = MDIGetActiveView();
	return pView ? pView->GetDocument() : NULL;
}

CView * CMainFrame::MDIGetActiveView()
{
	CMDIChildWnd * pChild = (CMDIChildWnd *)GetActiveFrame();
	return pChild ? pChild->GetActiveView() : NULL;
}

void CMainFrame::ActivateNextChildFrame()
{
	CMDIChildWnd * pChild = m_wndFileTab.GetNextFileTabItem();
	if( pChild ) pChild->MDIActivate();
}

void CMainFrame::ActivatePrevChildFrame()
{
	CMDIChildWnd * pChild = m_wndFileTab.GetPrevFileTabItem();
	if( pChild ) pChild->MDIActivate();
}

void CMainFrame::ActivateChildFrameInZOrder(INT nIndex)
{
	INT nCount = m_lstChildFrame.GetCount();
	if( nCount == 0 ) return;

	nIndex = nIndex % nCount;
	if( nIndex < 0 || nIndex > nCount-1 ) return;

	POSITION pos = m_lstChildFrame.FindIndex(nIndex);
	if( ! pos ) return;

	CMDIChildWnd * pChild = m_lstChildFrame.GetAt(pos);
	if( pChild ) pChild->MDIActivate();
}

void CMainFrame::BringChildFrameToTop(CMDIChildWnd * pChild)
{
	if( pChild == NULL ) pChild = MDIGetActive();

	POSITION pos = m_lstChildFrame.Find(pChild);
	if( ! pos ) return;

	m_lstChildFrame.RemoveAt(pos);
	m_lstChildFrame.AddHead(pChild);
}


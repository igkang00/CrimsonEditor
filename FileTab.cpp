// FileTab.cpp : implementation file
//

#include "stdafx.h"
#include "cedtHeader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMDIFileTab

CMDIFileTab::CMDIFileTab()
{
}

CMDIFileTab::~CMDIFileTab()
{
}


BEGIN_MESSAGE_MAP(CMDIFileTab, CDialogBar)
	//{{AFX_MSG_MAP(CMDIFileTab)
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(TCN_SELCHANGE, IDC_FILE_TAB, OnSelchangeFileTab)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnNeedtextFileTab)
	ON_COMMAND(ID_FILE_TAB_REFRESH, OnFileTabRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMDIFileTab message handlers

BOOL CMDIFileTab::Create(CWnd * pParentWnd)
{
	CDialogBar::Create(pParentWnd, IDD_MDI_FILE_TAB, WS_CHILD | WS_VISIBLE | CBRS_TOP
		/* | CBRS_GRIPPER */ | CBRS_SIZE_DYNAMIC, IDD_MDI_FILE_TAB);

	// register this window as drop target
	m_oleDropTarget.Register(this);

	m_lstImage.Create(IDB_MDI_FILE_TAB, 16, 0, RGB(255, 0, 255));
	m_ctlXPTab.SubclassDlgItem(IDC_FILE_TAB, this);

	m_ctlToolTip.Create( & m_ctlXPTab, TTS_NOPREFIX );
	m_ctlXPTab.SetToolTips( & m_ctlToolTip );

	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB);
	if( pTabCtrl ) pTabCtrl->SetImageList( & m_lstImage );

	return TRUE;
}

CSize CMDIFileTab::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	return CDialogBar::CalcFixedLayout(bStretch, bHorz);
}

CSize CMDIFileTab::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	CRect rect; AfxGetMainWnd()->GetWindowRect(& rect);
	return CSize(rect.Width()-4, 30);
}

void CMDIFileTab::OnSize(UINT nType, int cx, int cy) 
{
	RECT rect; GetClientRect( & rect );

	rect.left += 2; rect.right -= 0;
	rect.top += 2; rect.bottom -= 0;

	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB);
	if( pTabCtrl ) pTabCtrl->MoveWindow( & rect, FALSE );
	if( pTabCtrl ) pTabCtrl->Invalidate();

	CDialogBar::OnSize(nType, cx, cy);
}

void CMDIFileTab::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );
	CRect rect; pTabCtrl->GetWindowRect( & rect ); if( ! rect.PtInRect( point ) ) return;

	TC_HITTESTINFO info; info.pt = point; pTabCtrl->ScreenToClient( & info.pt );
	INT nTab = pTabCtrl->HitTest( & info );
	
	if( nTab >= 0 ) {
		TCITEM item; item.mask = TCIF_PARAM;
		pTabCtrl->GetItem(nTab, & item);

		CMDIChildWnd * pChild = (CMDIChildWnd *)item.lParam;
		pChild->MDIActivate();
	}

	CMenu * pMenu, context; context.LoadMenu(IDR_FILE_TAB);
	if( nTab < 0 ) pMenu = context.GetSubMenu(0);
	else pMenu = context.GetSubMenu(1);

	UINT nFlags = TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON;
	pMenu->TrackPopupMenu(nFlags, point.x, point.y, AfxGetMainWnd()); 
}

void CMDIFileTab::OnSelchangeFileTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );
	INT nTab = pTabCtrl->GetCurSel(); if( nTab < 0 ) return;

	TCITEM item; item.mask = TCIF_PARAM;
	pTabCtrl->GetItem(nTab, & item);

	CMDIChildWnd * pChild = (CMDIChildWnd *)item.lParam;
	pChild->MDIActivate();

	*pResult = 0;
}

BOOL CMDIFileTab::OnNeedtextFileTab(UINT id, NMHDR * pNMHDR, LRESULT * pResult)
{
    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
	if( pTTT->uFlags & TTF_IDISHWND ) return FALSE;

	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );
    UINT nTab = pNMHDR->idFrom;

	TCITEM item; item.mask = TCIF_PARAM;
	pTabCtrl->GetItem(nTab, & item);

	CMDIChildWnd * pChild = (CMDIChildWnd *)item.lParam;
	static TCHAR szText[MAX_PATH]; pChild->GetWindowText(szText, MAX_PATH);

	CCedtDoc * pDoc = (CCedtDoc *)pChild->GetActiveDocument();
	if( ! strlen(szText) ) strcpy(szText, pDoc->GetTitle());

	pTTT->lpszText = szText;
	pTTT->hinst = NULL;

    return TRUE;
}

void CMDIFileTab::OnFileTabRefresh() 
{
	TRACE0("CMDIFileTab::OnFileTabRefresh()\n");

	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );
	INT nCount = pTabCtrl->GetItemCount();

	for( INT nTab = 0; nTab < nCount; nTab++ ) {
		TCITEM item; item.mask = TCIF_PARAM;
		pTabCtrl->GetItem(nTab, & item);

		// update each tab item...
		UpdateMDIFileTab((CMDIChildWnd *)item.lParam);
	}

	// call Invalidate() to make it draw spin buttons
	Invalidate();
}

BOOL CMDIFileTab::PreTranslateMessage(MSG* pMsg) 
{
	CRect rect; CPoint point;
	CTabCtrl * pTabCtrl = NULL;

	switch( pMsg->message ) {
	case WM_LBUTTONDOWN:
		pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );
		pTabCtrl->GetWindowRect( & rect ); ScreenToClient( & rect );

		point.x = LOWORD(pMsg->lParam); point.y = HIWORD(pMsg->lParam);
		if( rect.PtInRect( point ) ) OnLButtonDownTabCtrl( pMsg->wParam, point );
		break;

	case WM_LBUTTONDBLCLK:
		pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );
		pTabCtrl->GetWindowRect( & rect ); ScreenToClient( & rect );

		point.x = LOWORD(pMsg->lParam); point.y = HIWORD(pMsg->lParam);
		if( rect.PtInRect( point ) ) OnLButtonDblClkTabCtrl( pMsg->wParam, point );
		break;

	case WM_MBUTTONDOWN:
		pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );
		pTabCtrl->GetWindowRect( & rect ); ScreenToClient( & rect );

		point.x = LOWORD(pMsg->lParam); point.y = HIWORD(pMsg->lParam);
		if( rect.PtInRect( point ) ) OnMButtonDownTabCtrl( pMsg->wParam, point );
		break;

	case WM_MBUTTONUP:
		pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );
		pTabCtrl->GetWindowRect( & rect ); ScreenToClient( & rect );

		point.x = LOWORD(pMsg->lParam); point.y = HIWORD(pMsg->lParam);
		if( rect.PtInRect( point ) ) OnMButtonUpTabCtrl( pMsg->wParam, point );
		break;

	case WM_BEGINDRAG_TAB_CTRL:
		pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );
		pTabCtrl->GetWindowRect( & rect ); ScreenToClient( & rect );

		point.x = LOWORD(pMsg->lParam); point.y = HIWORD(pMsg->lParam);
		if( rect.PtInRect( point ) ) OnBegindragTabCtrl( pMsg->wParam, point );
		break;
	}
	
	return CDialogBar::PreTranslateMessage(pMsg);
}

void CMDIFileTab::OnLButtonDownTabCtrl(UINT nFlags, CPoint point) 
{
	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );
	CMDIFrameWnd * pFrame = (CMDIFrameWnd *)AfxGetMainWnd();

	TC_HITTESTINFO info; info.pt = point;
	ClientToScreen( & info.pt ); pTabCtrl->ScreenToClient( & info.pt );
	INT nTab = pTabCtrl->HitTest( & info );

	if( nTab >= 0 && nTab == pTabCtrl->GetCurSel() ) {
		PostMessage( WM_BEGINDRAG_TAB_CTRL, nFlags, MAKELONG(point.x, point.y) );
	}
}

void CMDIFileTab::OnLButtonDblClkTabCtrl(UINT nFlags, CPoint point)
{
	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );
	CMDIFrameWnd * pFrame = (CMDIFrameWnd *)AfxGetMainWnd();

	TC_HITTESTINFO info; info.pt = point;
	ClientToScreen( & info.pt ); pTabCtrl->ScreenToClient( & info.pt );
	INT nTab = pTabCtrl->HitTest( & info );

	if( nTab >= 0 ) {
		TCITEM item; item.mask = TCIF_PARAM;
		pTabCtrl->GetItem(nTab, & item);

		if( CCedtApp::m_bCloseTabUsingDoubleClick ) {
			CMDIChildWnd * pChild = (CMDIChildWnd *)item.lParam;
			pChild->PostMessage(WM_CLOSE, 0, 0L);

		} else {
			CMDIChildWnd * pChild = (CMDIChildWnd *)item.lParam;
			CWnd * pParent = pChild->GetParent();

			BOOL bMaximized = FALSE;
			pFrame->MDIGetActive( & bMaximized );

			if( bMaximized ) {
				pParent->PostMessage(WM_MDIRESTORE, (WPARAM)pChild->m_hWnd, 0L);
			} else {
				pParent->PostMessage(WM_MDIMAXIMIZE, (WPARAM)pChild->m_hWnd, 0L);
			}
		}
	}
}

void CMDIFileTab::OnMButtonDownTabCtrl(UINT nFlags, CPoint point)
{
	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );
	CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();

	TC_HITTESTINFO info; info.pt = point;
	ClientToScreen( & info.pt ); pTabCtrl->ScreenToClient( & info.pt );
	INT nTab = pTabCtrl->HitTest( & info );

	if( nTab >= 0 ) {
		TCITEM item; item.mask = TCIF_PARAM;
		pTabCtrl->GetItem(nTab, & item);

		CMDIChildWnd * pChild = (CMDIChildWnd *)item.lParam;
		pFrame->SetActiveFileTab(pChild);
	}
}

void CMDIFileTab::OnMButtonUpTabCtrl(UINT nFlags, CPoint point)
{
	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );
	CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();

	TC_HITTESTINFO info; info.pt = point;
	ClientToScreen( & info.pt ); pTabCtrl->ScreenToClient( & info.pt );
	INT nTab = pTabCtrl->HitTest( & info );

	if( nTab >= 0 ) {
	/*	TCITEM item; item.mask = TCIF_PARAM;
		pTabCtrl->GetItem(nTab, & item);

		CMDIChildWnd * pChild = (CMDIChildWnd *)item.lParam;
		pChild->PostMessage(WM_CLOSE, 0, 0L); */
	}
}

void CMDIFileTab::OnBegindragTabCtrl(UINT nFlags, CPoint point)
{
	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );
	INT nSel = pTabCtrl->GetCurSel(); if( nSel < 0 ) return;

	TC_HITTESTINFO info; info.pt = point;
	ClientToScreen( & info.pt ); pTabCtrl->ScreenToClient( & info.pt );
	INT nTab = pTabCtrl->HitTest( & info ); if( nTab < 0 ) return;

	TCITEM item; item.mask = TCIF_PARAM; pTabCtrl->GetItem(nTab, & item);
	UINT size = sizeof(TCITEM);

	HGLOBAL hMemory = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT, size);
	if( ! hMemory ) return;

	TCITEM * pItem = (TCITEM *)::GlobalLock(hMemory);
	if( ! pItem ) { ::GlobalFree(hMemory); return; }

	CopyMemory( pItem, & item, size );
	::GlobalUnlock(hMemory);

	FORMATETC etc = { g_uClipbrdFormatFileTabItem, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	m_oleDataSource.CacheGlobalData( g_uClipbrdFormatFileTabItem, hMemory, & etc );

	DROPEFFECT eff = m_oleDataSource.DoDragDrop(DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_SCROLL);

	if( eff == DROPEFFECT_MOVE ) {
		// item is to be moved
		CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();
		pFrame->DeleteMDIFileTab((CMDIChildWnd *)pItem->lParam);
		pFrame->InsertMDIFileTab((CMDIChildWnd *)pItem->lParam, m_nDropPosition);
		pFrame->UpdateMDIFileTab((CMDIChildWnd *)pItem->lParam);
		pFrame->SetActiveFileTab((CMDIChildWnd *)pItem->lParam);
	} else if( eff == DROPEFFECT_NONE ) {
		// drag and drop has been canceled
		::GlobalFree(hMemory);
	}

	m_oleDataSource.Empty();
}

DROPEFFECT CMDIFileTab::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	TRACE0("CMDIFileTab::OnDragEnter\n");

	if( pDataObject->IsDataAvailable(g_uClipbrdFormatFileTabItem) ) {
		// set drag object type
		m_nDragObjectType = DRAG_OBJECT_FILETAB_ITEM;

		CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );
		m_nDragItemCount = pTabCtrl->GetItemCount();
		m_nDragCurSel = pTabCtrl->GetCurSel();
		m_nDropPosition = m_nDragCurSel;

		return DROPEFFECT_NONE;

	} else if( pDataObject->IsDataAvailable(CF_HDROP) ) {
		// set drag object type
		m_nDragObjectType = DRAG_OBJECT_HDROP;

		CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pFrame );
		return pFrame->OnDragEnter(pDataObject, dwKeyState, point);

	} else if( pDataObject->IsDataAvailable(g_uClipbrdFormatProjectItem) ) {
		// set drag object type
		m_nDragObjectType = DRAG_OBJECT_PROJECT_ITEM;

		CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pFrame );
		return pFrame->OnDragEnter(pDataObject, dwKeyState, point);

	} else {
		// set drag object type
		m_nDragObjectType = DRAG_OBJECT_UNKNOWN;

		return DROPEFFECT_NONE;
	}
}

DROPEFFECT CMDIFileTab::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
//	TRACE0("CMDIFileTab::OnDragOver\n");

	if( m_nDragObjectType == DRAG_OBJECT_FILETAB_ITEM ) {
		CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );

		TCHITTESTINFO info; info.pt = point;
		ClientToScreen( & info.pt ); pTabCtrl->ScreenToClient( & info.pt );
		INT nTab = pTabCtrl->HitTest( & info );

		for( INT i = 0; i < m_nDragItemCount; i++ ) pTabCtrl->HighlightItem(i, (i == nTab) ? TRUE : FALSE);

		if( nTab >= 0 && nTab != m_nDragCurSel ) return DROPEFFECT_MOVE;
		else return DROPEFFECT_NONE;

	} else if( m_nDragObjectType == DRAG_OBJECT_HDROP ) {
		CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pFrame );
		return pFrame->OnDragOver(pDataObject, dwKeyState, point);

	} else if( m_nDragObjectType == DRAG_OBJECT_PROJECT_ITEM ) {
		CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pFrame );
		return pFrame->OnDragOver(pDataObject, dwKeyState, point);

	} else {
		return DROPEFFECT_NONE;
	}
}

void CMDIFileTab::OnDragLeave()
{
	if( m_nDragObjectType == DRAG_OBJECT_FILETAB_ITEM ) {
		CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );
		for( INT i = 0; i < m_nDragItemCount; i++ ) pTabCtrl->HighlightItem(i, FALSE);

	} else if( m_nDragObjectType == DRAG_OBJECT_HDROP ) {
		CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pFrame );
		pFrame->OnDragLeave();

	} else if( m_nDragObjectType == DRAG_OBJECT_PROJECT_ITEM ) {
		CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pFrame );
		pFrame->OnDragLeave();
	}

	TRACE0("CMDIFileTab::OnDragLeave\n");
}

BOOL CMDIFileTab::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	TRACE0("CMDIFileTab::OnDrop\n");

	if( pDataObject->IsDataAvailable(g_uClipbrdFormatFileTabItem) ) {
		CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );
		for( INT i = 0; i < m_nDragItemCount; i++ ) pTabCtrl->HighlightItem(i, FALSE);

		TCHITTESTINFO info; info.pt = point;
		ClientToScreen( & info.pt ); pTabCtrl->ScreenToClient( & info.pt );
		INT nTab = pTabCtrl->HitTest( & info );

		if( nTab >= 0 && nTab != m_nDragCurSel && (dropEffect == DROPEFFECT_MOVE) ) {
			HGLOBAL hMemory = pDataObject->GetGlobalData(g_uClipbrdFormatFileTabItem);
			if( ! hMemory ) return FALSE;

			TCITEM * pItem = (TCITEM *)::GlobalLock(hMemory);
			if( ! pItem ) { ::GlobalUnlock(hMemory); return FALSE; }

			m_nDropPosition = nTab;

			::GlobalUnlock(hMemory);
			::GlobalFree(hMemory);

			return TRUE;

		} else return FALSE;

	} else if( pDataObject->IsDataAvailable(CF_HDROP) ) {
		CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pFrame );
		return pFrame->OnDrop(pDataObject, dropEffect, point);

	} else if( pDataObject->IsDataAvailable(g_uClipbrdFormatProjectItem) ) {
		CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pFrame );
		return pFrame->OnDrop(pDataObject, dropEffect, point);

	} else {
		return FALSE;
	}
}


void CMDIFileTab::InsertMDIFileTab(CMDIChildWnd * pChild, INT nTab)
{
	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );

	TCITEM item; item.mask = TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM;
	item.pszText = ""; item.iImage = 0; item.lParam = (LPARAM)pChild;

	if( nTab < 0 ) nTab = pTabCtrl->GetItemCount();
	pTabCtrl->InsertItem(nTab, & item); PostMessage(WM_COMMAND, ID_FILE_TAB_REFRESH, 0L);
}

void CMDIFileTab::DeleteMDIFileTab(CMDIChildWnd * pChild)
{
	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );
	INT nTab = FindTabIndex(pChild); if(nTab < 0) return;
	pTabCtrl->DeleteItem(nTab); PostMessage(WM_COMMAND, ID_FILE_TAB_REFRESH, 0L);
}

void CMDIFileTab::UpdateMDIFileTab(CMDIChildWnd * pChild)
{
	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );

	CCedtDoc * pDoc = (CCedtDoc *)pChild->GetActiveDocument();
	CString szText; pChild->GetWindowText( szText );

	// check string size of window text (can have zero length if it is not visible)
	if( ! szText.GetLength() ) szText = pDoc->GetTitle();

	// process window text to adjust into tab string
	szText = GetFileName(szText); AdjustTabString(szText); 
	szText.Replace("&", "&&"); szText += " ";

	TCITEM item; item.mask = TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM;
	item.pszText = (LPTSTR)(LPCTSTR)szText;
	item.iImage = pDoc->IsModified() ? 1 : 0;
	item.lParam = (LPARAM)pChild;

	TCHAR szTxt2[MAX_PATH]; 
	TCITEM itm2; itm2.mask = TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM;
	itm2.pszText = szTxt2; itm2.cchTextMax = sizeof(szTxt2);

	INT nTab = FindTabIndex(pChild); if(nTab < 0) return;
	pTabCtrl->GetItem(nTab, & itm2);

	// update tab item only if it is different from the current status
	if( strcmp(item.pszText, itm2.pszText) || item.iImage != itm2.iImage ) {
		pTabCtrl->SetItem(nTab, & item);
	}
}

void CMDIFileTab::SetActiveFileTab(CMDIChildWnd * pChild)
{
	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );
	INT nTab = FindTabIndex(pChild); if(nTab < 0) return;
	pTabCtrl->SetCurSel(nTab);
}

INT CMDIFileTab::GetMDIFileTabCount()
{
	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );
	return pTabCtrl->GetItemCount();
}

CMDIChildWnd * CMDIFileTab::GetFileTabItem(INT nTab)
{
	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );

	TCITEM item; item.mask = TCIF_PARAM;
	pTabCtrl->GetItem(nTab, & item);

	return (CMDIChildWnd *)item.lParam;
}

CMDIChildWnd * CMDIFileTab::GetNextFileTabItem()
{
	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );

	INT nCount = pTabCtrl->GetItemCount();
	if( nCount == 0 ) return NULL;

	INT nTab = pTabCtrl->GetCurSel();
	if( nTab < 0 ) return NULL;

	INT nNext = nTab + 1;
	if( nNext > nCount-1 ) nNext = 0;

	TCITEM item; item.mask = TCIF_PARAM;
	pTabCtrl->GetItem(nNext, & item);

	return (CMDIChildWnd *)item.lParam;
}

CMDIChildWnd * CMDIFileTab::GetPrevFileTabItem()
{
	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );

	INT nCount = pTabCtrl->GetItemCount();
	if( nCount == 0 ) return NULL;

	INT nTab = pTabCtrl->GetCurSel();
	if( nTab < 0 ) return NULL;

	INT nPrev = nTab - 1;
	if( nPrev < 0 ) nPrev = nCount-1;

	TCITEM item; item.mask = TCIF_PARAM;
	pTabCtrl->GetItem(nPrev, & item);

	return (CMDIChildWnd *)item.lParam;
}

void CMDIFileTab::AdjustTabString(CString & szFileName)
{
	INT nFileTabLength = CCedtView::m_nFileTabLength;
	INT nLength = szFileName.GetLength();

	if( nLength > CCedtView::m_nFileTabLength ) {
		INT nFound = szFileName.ReverseFind('.');

		if( nFound >= 0 ) nFound = nFound+1; // ignore '.' character
		else nFound = szFileName.Find(':'); // try to get view counter

		if( nFound >= 0 ) {
			CString szExtension = szFileName.Mid(nFound);
			szFileName = szFileName.Mid(0, nFileTabLength-(nLength-nFound)-3);
			szFileName += CString("...") + szExtension;
		} else {
			szFileName = szFileName.Mid(0, nFileTabLength-3);
			szFileName += "...";
		}
	}
}

int CMDIFileTab::FindTabIndex(CMDIChildWnd * pChild)
{
	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );

	TCITEM item; item.mask = TCIF_PARAM;
	INT nCount = pTabCtrl->GetItemCount();

	for(INT i = 0; i < nCount; i++) {
		pTabCtrl->GetItem(i, & item);
		if( item.lParam == (LPARAM)pChild ) return i;
	}

	return -1;
}

/* use HitTest instead of calling this function */
int CMDIFileTab::FindTabIndex(CPoint point)
{
	CTabCtrl * pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_FILE_TAB); ASSERT( pTabCtrl );

	CRect rect;
	INT nCount = pTabCtrl->GetItemCount();

	for(INT i = 0; i < nCount; i++) {
		pTabCtrl->GetItemRect(i, & rect);
		if( rect.PtInRect(point) ) return i;
	}

	return -1;
}

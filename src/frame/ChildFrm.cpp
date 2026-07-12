// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "cedtHeader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_MDIACTIVATE()
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_CLOSE, OnWindowClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
}

CChildFrame::~CChildFrame()
{
}


/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
//	return CMDIChildWnd::OnCreateClient(lpcs, pContext);
	return m_wndSplitter.Create(this, 2, 2, CSize(10, 10), pContext);
}

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if(CMDIChildWnd::OnCreate(lpCreateStruct) == -1) return -1;

	// insert into file selection tab
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	pMainFrame->InsertMDIFileTab(this);

	return 0;
}

// Closing a MAXIMIZED MDI child laid out the whole document again on the way out. With word
// wrap on and a 900,000-line file, you watch the progress bar spend a second doing it.
//
// Nobody asked for it. DefMDIChildProc un-maximizes the child before it destroys it, so the
// view gets a WM_SIZE with a smaller width -- and a width change is a real wrap-width
// change, so CCedtView::OnSize does what it must and reformats every line. Lines that are
// about to be deleted. Closing from the File menu never un-maximizes, which is why only the
// MDI close button showed it.
//
// The view cannot tell, from a WM_SIZE alone, that it is on its way out. So it is told,
// from each of the three doors out of a document -- here (WM_CLOSE: the file tab, when the
// close-on-double-click preference is on), OnWindowClose (the MDI close button), and
// CCedtDoc::OnCloseDocument (the File menu, and shutdown).
void CChildFrame::OnClose()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetActiveDocument();
	HWND hWnd = m_hWnd;

	if( pDoc ) pDoc->SetClosing(TRUE);

	CMDIChildWnd::OnClose();   // if this closes us, `this` and pDoc are gone when it returns

	// Still a window? Then the close was refused -- at the "save changes?" prompt, say --
	// and the frame and its document both survive. Take the flag back.
	if( pDoc && ::IsWindow(hWnd) ) pDoc->SetClosing(FALSE);
}

void CChildFrame::OnDestroy()
{
	// delete from file tab
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	pMainFrame->DeleteMDIFileTab(this);

	CMDIChildWnd::OnDestroy();
}

void CChildFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd) 
{
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

	if( bActivate ) {
		// set this active file tab
		CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
		pMainFrame->SetActiveFileTab(this);
	}
}

// The MDI close button lands here, not on WM_CLOSE -- MDIDestroy goes straight to
// WM_MDIDESTROY. Same story as OnClose: DefMDIChildProc un-maximizes the child on its way
// to destroying it, and that width change would reformat the whole document. Say we are
// leaving before we start leaving.
void CChildFrame::OnWindowClose()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetActiveDocument(); ASSERT( pDoc );
	if( ! pDoc->CanCloseFrame(this) ) return;   // the user said no; nothing to undo

	pDoc->SetClosing(TRUE);
	MDIDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// CChildFrame operations

CWnd * CChildFrame::GetNeighborPane(CWnd * pWnd)
{
	BOOL bChild; INT nRowIndex, nColIndex;
	bChild = m_wndSplitter.IsChildPane(pWnd, &nRowIndex, &nColIndex); ASSERT( bChild );

	if( nColIndex > 0 ) return m_wndSplitter.GetPane( nRowIndex, nColIndex-1 );
	if( nRowIndex > 0 ) return m_wndSplitter.GetPane( nRowIndex-1, nColIndex );

	return pWnd;
}

void CChildFrame::SetScrollPosSyncAllPanes(CWnd * pWnd, INT nPosX, INT nPosY)
{
	BOOL bChild; INT i, j, nRowIndex, nColIndex, nRowCount, nColCount;
	bChild = m_wndSplitter.IsChildPane(pWnd, &nRowIndex, &nColIndex); ASSERT( bChild );

	nRowCount = m_wndSplitter.GetRowCount();
	for( i = 0; i < nRowCount; i++ ) {
		CCedtView * pView = (CCedtView *)m_wndSplitter.GetPane(i, nColIndex);
		pView->SetScrollPosX( nPosX ); 
	}

	nColCount = m_wndSplitter.GetColumnCount();
	for( j = 0; j < nColCount; j++ ) {
		CCedtView * pView = (CCedtView *)m_wndSplitter.GetPane(nRowIndex, j);
		pView->SetScrollPosY( nPosY );
	}
}

void CChildFrame::UpdateAllPanesInTheSameRow(CWnd * pWnd)
{
	BOOL bChild; INT j, nRowIndex, nColIndex, nColCount;
	bChild = m_wndSplitter.IsChildPane(pWnd, &nRowIndex, &nColIndex); ASSERT( bChild );

	nColCount = m_wndSplitter.GetColumnCount();
	for( j = 0; j < nColCount; j++ ) {
		CCedtView * pView = (CCedtView *)m_wndSplitter.GetPane(nRowIndex, j);
		if( pView != pWnd ) pView->Invalidate();
	}
}

void CChildFrame::UpdateAllPanesSharingScrollBar(CWnd * pWnd)
{
	BOOL bChild; INT i, j, nRowIndex, nColIndex, nRowCount, nColCount;
	bChild = m_wndSplitter.IsChildPane(pWnd, &nRowIndex, &nColIndex); ASSERT( bChild );

	nRowCount = m_wndSplitter.GetRowCount();
	for( i = 0; i < nRowCount; i++ ) {
		CCedtView * pView = (CCedtView *)m_wndSplitter.GetPane(i, nColIndex);
		if( pView != pWnd ) pView->Invalidate();
	}

	nColCount = m_wndSplitter.GetColumnCount();
	for( j = 0; j < nColCount; j++ ) {
		CCedtView * pView = (CCedtView *)m_wndSplitter.GetPane(nRowIndex, j);
		if( pView != pWnd ) pView->Invalidate();
	}
}


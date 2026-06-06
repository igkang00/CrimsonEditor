// OutputWindow.cpp : implementation file
//

#include "stdafx.h"
#include "cedtHeader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// COutputWindow static member variables

COutputPattern COutputWindow::m_clsPattern[11];
CRegExp COutputWindow::m_clsRegExp[11];
BOOL COutputWindow::m_bLocalEcho = TRUE;
INT COutputWindow::m_nMaxLineCount = OUTPUT_MAX_LINE_COUNT;

COLORREF COutputWindow::m_crBkgrColor;
COLORREF COutputWindow::m_crTextColor;
COLORREF COutputWindow::m_crGrayColor;
COLORREF COutputWindow::m_crInputText;
COLORREF COutputWindow::m_crErrorText;


/////////////////////////////////////////////////////////////////////////////
// COutputWindow

COutputWindow::COutputWindow()
{
	memset( m_tbiToolbarOutput, 0x00, sizeof(m_tbiToolbarOutput) );
	memset( m_tbiWinButtons, 0x00, sizeof(m_tbiWinButtons) );

	m_tbiToolbarOutput[0].iBitmap = 0;
	m_tbiToolbarOutput[0].idCommand = ID_DIR_ITEM_REFRESH;
	m_tbiToolbarOutput[0].fsState = TBSTATE_ENABLED | TBSTATE_WRAP;
	m_tbiToolbarOutput[0].fsStyle = TBSTYLE_BUTTON;

	m_tbiToolbarOutput[1].fsState = TBSTATE_ENABLED | TBSTATE_WRAP;
	m_tbiToolbarOutput[1].fsStyle = TBSTYLE_SEP;

	m_tbiToolbarOutput[2].iBitmap = 1;
	m_tbiToolbarOutput[2].idCommand = ID_DIR_ITEM_COPY;
	m_tbiToolbarOutput[2].fsState = TBSTATE_ENABLED | TBSTATE_WRAP;
	m_tbiToolbarOutput[2].fsStyle = TBSTYLE_BUTTON;

	m_tbiToolbarOutput[3].iBitmap = 2;
	m_tbiToolbarOutput[3].idCommand = ID_DIR_ITEM_MOVE;
	m_tbiToolbarOutput[3].fsState = TBSTATE_ENABLED | TBSTATE_WRAP;
	m_tbiToolbarOutput[3].fsStyle = TBSTYLE_BUTTON;

	m_tbiToolbarOutput[4].iBitmap = 3;
	m_tbiToolbarOutput[4].idCommand = ID_DIR_ITEM_DELETE;
	m_tbiToolbarOutput[4].fsState = TBSTATE_ENABLED | TBSTATE_WRAP;
	m_tbiToolbarOutput[4].fsStyle = TBSTYLE_BUTTON;

	m_tbiToolbarOutput[5].fsState = TBSTATE_ENABLED | TBSTATE_WRAP;
	m_tbiToolbarOutput[5].fsStyle = TBSTYLE_SEP;

	m_tbiWinButtons[0].iBitmap = 1;
	m_tbiWinButtons[0].idCommand = ID_OUTPUT_WINDOW_HIDE;
	m_tbiWinButtons[0].fsState = TBSTATE_ENABLED | TBSTATE_WRAP;
	m_tbiWinButtons[0].fsStyle = TBSTYLE_BUTTON;
}

COutputWindow::~COutputWindow()
{
	m_imgToolbarOutput.Detach();
	m_imgWinButtons.Detach();
}


BEGIN_MESSAGE_MAP(COutputWindow, CSizingControlBar)
	//{{AFX_MSG_MAP(COutputWindow)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_LBN_DBLCLK(IDC_OUTPUT, OnDblclkOutput)
	ON_COMMAND(ID_OUTPUT_WINDOW_CLEAR, OnOutputWindowClear)
	ON_COMMAND(ID_OUTPUT_WINDOW_COPY_ALL, OnOutputWindowCopyAll)
	ON_COMMAND(ID_OUTPUT_WINDOW_COPY, OnOutputWindowCopy)
	ON_COMMAND(ID_OUTPUT_WINDOW_SEND_INPUT, OnOutputWindowSendInput)
	ON_COMMAND(ID_OUTPUT_WINDOW_KILL_PROCESS, OnOutputWindowKillProcess)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COutputWindow message handlers

int COutputWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CRect rect(0, 0, 1, 1); DWORD dwStyle;
	if (CSizingControlBar::OnCreate(lpCreateStruct) == -1) return -1;

	dwStyle = WS_VISIBLE | WS_CHILD; CString szText( (LPCTSTR)IDS_CTRL_OUTPUT_WINDOW );
	m_stcCaptionOutput.Create(szText, dwStyle, rect, this, IDC_OUTPUT_CAPTION);

	dwStyle = WS_VISIBLE | WS_CHILD | CCS_NORESIZE | CCS_NODIVIDER | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS;
	m_btnWinButtons.Create(dwStyle, rect, this, IDC_OUTPUT_TOOLBAR);

	dwStyle = WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_HSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | LBS_USETABSTOPS | LBS_HASSTRINGS | LBS_OWNERDRAWFIXED;
	m_lstConsoleOutput.Create(dwStyle, rect, this, IDC_OUTPUT_LIST);
	m_lstConsoleOutput.ModifyStyleEx(0, WS_EX_CLIENTEDGE, 0);

	m_lstConsoleOutput.SetHorizontalExtent(2560);
	m_lstConsoleOutput.SetTabStops(32); // dialog based unit (approximately 4 character)

	dwStyle = WS_VISIBLE | WS_CHILD;
	m_edtConsoleInput.Create(dwStyle, rect, this, IDC_OUTPUT_EDIT);
	m_edtConsoleInput.ModifyStyleEx(0, WS_EX_CLIENTEDGE, 0);
	m_edtConsoleInput.SetReadOnly(TRUE);

	// set image list
//	m_imgToolbarOutput.Create(IDB_OUTPUT_TOOLBAR, 16, 0, RGB(255, 0, 255));
//	m_btnToolbarOutput.SetImageList( & m_imgToolbarOutput );

	m_imgWinButtons.Create(IDB_WIN_BUTTONS, 12, 0, RGB(255, 0, 255));
	m_btnWinButtons.SetImageList( & m_imgWinButtons );

	// set control font
	CFont * pFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	LOGFONT lf; pFont->GetLogFont( & lf ); m_fontControl.CreateFontIndirect( & lf );
	m_stcCaptionOutput.SetFont( & m_fontControl, FALSE );

	// set output font
	ApplyOutputFont( FALSE );

	// initialize toolbar
//	m_btnToolbarOutput.AddButtons( 6, m_tbiToolbarOutput );
	m_btnWinButtons.AddButtons( 1, m_tbiWinButtons );

	// load accelerators
	CCedtApp * pApp = (CCedtApp *)AfxGetApp();
	m_hAccel = LoadAccelerators(pApp->m_hInstance, MAKEINTRESOURCE(IDR_OUTPUT_WINDOW));
	
	// set occupied flag
	m_bOccupied = FALSE;

	return 0;
}

void COutputWindow::OnSize(UINT nType, int cx, int cy) 
{
	CSizingControlBar::OnSize(nType, cx, cy);

	INT nBegX = 2, nEndX = cx - 4;

//	nBegX  =  2; m_btnToolbarOutput.MoveWindow(nBegX, 2, 22, 106);
	nBegX +=  0; m_stcCaptionOutput.MoveWindow(nBegX, 24, 18, cy-26);
	nBegX +=  2; m_btnWinButtons.MoveWindow(nBegX, 4, 19, 18);
	nBegX += 20; m_lstConsoleOutput.MoveWindow(nBegX, 2, nEndX-nBegX, cy-24);
	nBegX +=  0; m_edtConsoleInput.MoveWindow(nBegX, cy-22, nEndX-nBegX, 20);
}

void COutputWindow::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CRect rect; m_lstConsoleOutput.GetWindowRect( & rect );
	if( rect.PtInRect(point) ) {
		CMenu * pMenu, context; context.LoadMenu(IDR_OUTPUT_WINDOW);
		pMenu = context.GetSubMenu(0);

		UINT nFlags = TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON;
		pMenu->TrackPopupMenu(nFlags, point.x, point.y, AfxGetMainWnd());
	}
}

void COutputWindow::OnDblclkOutput() 
{
	INT nSelect = m_lstConsoleOutput.GetCurSel(); 
	if( nSelect == LB_ERR ) return;

	CString szString; m_lstConsoleOutput.GetText(nSelect, szString);
	if( ! szString.GetLength() ) return;

	INT nIndex1 = szString.Find('('); if( nIndex1 < 0 ) return;
	INT nIndex2 = szString.Find(')'); if( nIndex2 < 0 ) return;
	if( nIndex2 < nIndex1 ) return;

	CString szPathName = szString.Left(nIndex1);
	if( ! VerifyFilePath(szPathName) ) return;

	CString szLineNum  = szString.Mid(nIndex1+1, nIndex2-nIndex1-1);
	INT nLineNum = atoi(szLineNum); if( ! nLineNum ) return;

	CCedtApp * pApp = (CCedtApp *)AfxGetApp(); if( ! pApp ) return;
	pApp->PostOpenDocumentFile( szPathName, nLineNum );
}

/////////////////////////////////////////////////////////////
// Operations
void COutputWindow::ApplyOutputFont(BOOL bRedraw)
{
	if( m_fontOutput.m_hObject ) m_fontOutput.DeleteObject();
	LOGFONT lf; memcpy( & lf, & CCedtView::m_lfMiscel[0x01], sizeof(LOGFONT) );

	CDC * pDC = m_lstConsoleOutput.GetDC();
	lf.lfHeight = -MulDiv( lf.lfHeight, pDC->GetDeviceCaps(LOGPIXELSY), 720 );
	m_lstConsoleOutput.ReleaseDC( pDC );
	m_fontOutput.CreateFontIndirect( & lf );

	m_lstConsoleOutput.SetFont( & m_fontOutput, bRedraw );
	m_edtConsoleInput.SetFont( & m_fontOutput, bRedraw );
}

void COutputWindow::EnableInputConsole(BOOL bEnable)
{
	m_edtConsoleInput.SetWindowText("");
	m_edtConsoleInput.SetReadOnly(!bEnable);
}

BOOL COutputWindow::AddStringToTheLast(LPCTSTR lpszString, COLORREF crTextColor)
{
	if( m_lstConsoleOutput.GetCount() >= OUTPUT_MAX_LINE_COUNT ) {
		m_lstConsoleOutput.SetRedraw(FALSE);
		m_lstConsoleOutput.DeleteString(0);
		m_lstConsoleOutput.SetRedraw(TRUE);
	}

	INT nIndex = m_lstConsoleOutput.AddString(lpszString, crTextColor);
	if( nIndex == LB_ERR || nIndex == LB_ERRSPACE ) return FALSE;

	CRect rectClient; m_lstConsoleOutput.GetClientRect( & rectClient );
	CRect rectItem; m_lstConsoleOutput.GetItemRect(nIndex, & rectItem);

	INT nLineCount = rectClient.Height() / rectItem.Height();
	INT nTop = m_lstConsoleOutput.GetTopIndex();
	if( nIndex - nTop + 1 > nLineCount ) m_lstConsoleOutput.SetTopIndex( nIndex - nLineCount + 1);

	return TRUE;
}

BOOL COutputWindow::ReplaceTheLastString(LPCTSTR lpszString, COLORREF crTextColor)
{
	INT nCount = m_lstConsoleOutput.GetCount();
	if( nCount > 0 ) m_lstConsoleOutput.DeleteString(nCount-1);

	INT nIndex = m_lstConsoleOutput.AddString(lpszString, crTextColor);
	if( nIndex == LB_ERR || nIndex == LB_ERRSPACE ) return FALSE;

	CRect rectClient; m_lstConsoleOutput.GetClientRect( & rectClient );
	CRect rectItem; m_lstConsoleOutput.GetItemRect(nIndex, & rectItem);

	INT nLineCount = rectClient.Height() / rectItem.Height();
	INT nTop = m_lstConsoleOutput.GetTopIndex();
	if( nIndex - nTop + 1 > nLineCount ) m_lstConsoleOutput.SetTopIndex( nIndex - nLineCount + 1);

	return TRUE;
}

BOOL COutputWindow::GetTheLastString(CString & szString)
{
	INT nCount = m_lstConsoleOutput.GetCount();
	if( nCount > 0 ) m_lstConsoleOutput.GetText(nCount-1, szString);
	else szString = "";

	return TRUE;
}

BOOL COutputWindow::PreTranslateMessage(MSG* pMsg) 
{
	// do not call CSizingControlBar::PreTranslateMessage()
	// output window should have separate message processing routine

	if( ! TranslateAccelerator(m_hWnd, m_hAccel, pMsg) ) {
		TranslateMessage( pMsg );
		DispatchMessage( pMsg );
	}

	return TRUE;
}

void COutputWindow::OnOutputWindowClear() 
{
	m_lstConsoleOutput.ResetContent();
}

void COutputWindow::OnOutputWindowCopyAll() 
{
	CMemText Block; TCHAR szText[2048];
	INT nCount = m_lstConsoleOutput.GetCount();

	for(INT i = 0; i < nCount; i++) {
		m_lstConsoleOutput.GetText(i, szText);
		Block.AddTail( szText );
	}

	CCedtView::SetClipboardData( Block );
}

void COutputWindow::OnOutputWindowCopy() 
{
	CMemText Block; TCHAR szText[2048];
	INT nSelect = m_lstConsoleOutput.GetCurSel(); if( nSelect == LB_ERR ) return;

	m_lstConsoleOutput.GetText(nSelect, szText);
	Block.AddTail( szText ); Block.AddTail("");

	CCedtView::SetClipboardData( Block );
}

void COutputWindow::OnOutputWindowSendInput() 
{
	if( m_edtConsoleInput.GetStyle() & ES_READONLY ) return;
//	PostMessage( WM_COMMAND, ID_COMMAND_SEND_INPUT, 0L );

	TCHAR szInputString[4096];
	m_edtConsoleInput.GetLine(0, szInputString, 4096);

	CCedtView::SetChildInputString( szInputString );
	m_edtConsoleInput.SetWindowText(""); // clear input console
}

void COutputWindow::OnOutputWindowKillProcess() 
{
	if( m_edtConsoleInput.GetStyle() & ES_READONLY ) return;
//	PostMessage( WM_COMMAND, ID_COMMAND_KILL_PROCESS, 0L );

	CCedtView::KillChildProcess();
}

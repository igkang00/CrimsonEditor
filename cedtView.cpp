// cedtView.cpp : implementation of the CCedtView class
//

#include "stdafx.h"
#include "cedtHeader.h"
#include <imm.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCedtView

IMPLEMENT_DYNCREATE(CCedtView, CView)

BEGIN_MESSAGE_MAP(CCedtView, CView)
	//{{AFX_MSG_MAP(CCedtView)
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_COMMAND(ID_EDIT_RETURN, OnEditReturn)
	ON_COMMAND(ID_EDIT_BACK, OnEditBack)
	ON_COMMAND(ID_EDIT_ESCAPE, OnEditEscape)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_EDIT_SELECT_LINE, OnEditSelectLine)
	ON_COMMAND(ID_EDIT_SELECT_WORD, OnEditSelectWord)
	ON_COMMAND(ID_VIEW_WORD_WRAP, OnViewWordWrap)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WORD_WRAP, OnUpdateViewWordWrap)
	ON_COMMAND(ID_EDIT_INSERT_DATE, OnEditInsertDate)
	ON_COMMAND(ID_EDIT_INSERT_TIME, OnEditInsertTime)
	ON_COMMAND(ID_EDIT_INSERT_FILE, OnEditInsertFile)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_CUT_APPEND, OnEditCutAppend)
	ON_COMMAND(ID_EDIT_COPY_APPEND, OnEditCopyAppend)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_TAB, OnEditTab)
	ON_COMMAND(ID_EDIT_DETAB, OnEditDetab)
	ON_COMMAND(ID_EDIT_JOIN_LINES, OnEditJoinLines)
	ON_COMMAND(ID_EDIT_SPLIT_LINE, OnEditSplitLine)
	ON_COMMAND(ID_EDIT_DELETE_LINE, OnEditDeleteLine)
	ON_COMMAND(ID_EDIT_DUPLICATE_LINE, OnEditDuplicateLine)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_SRCH_FIND, OnSearchFind)
	ON_COMMAND(ID_SRCH_REPLACE, OnSearchReplace)
	ON_COMMAND(ID_SRCH_GO_TO, OnSearchGoTo)
	ON_COMMAND(ID_SRCH_ASK_REPLACE, OnSearchAskReplace)
	ON_COMMAND(ID_SRCH_TOGGLE_BOOKMARK, OnSearchToggleBookmark)
	ON_COMMAND(ID_SRCH_NEXT_BOOKMARK, OnSearchNextBookmark)
	ON_COMMAND(ID_SRCH_PREV_BOOKMARK, OnSearchPrevBookmark)
	ON_COMMAND(ID_MACRO_TOGGLE_RECORDING, OnMacroToggleRecording)
	ON_COMMAND(ID_MACRO_BEGIN_RECORDING, OnMacroBeginRecording)
	ON_COMMAND(ID_MACRO_END_RECORDING, OnMacroEndRecording)
	ON_UPDATE_COMMAND_UI(ID_MACRO_BEGIN_RECORDING, OnUpdateMacroBeginRecording)
	ON_UPDATE_COMMAND_UI(ID_MACRO_END_RECORDING, OnUpdateMacroEndRecording)
	ON_COMMAND(ID_MACRO_REPLAY, OnMacroReplay)
	ON_COMMAND(ID_MACRO_REPLAY1, OnMacroReplay1)
	ON_COMMAND(ID_MACRO_REPLAY2, OnMacroReplay2)
	ON_COMMAND(ID_MACRO_REPLAY3, OnMacroReplay3)
	ON_COMMAND(ID_MACRO_REPLAY4, OnMacroReplay4)
	ON_COMMAND(ID_MACRO_REPLAY5, OnMacroReplay5)
	ON_COMMAND(ID_MACRO_REPLAY6, OnMacroReplay6)
	ON_COMMAND(ID_MACRO_REPLAY7, OnMacroReplay7)
	ON_COMMAND(ID_MACRO_REPLAY8, OnMacroReplay8)
	ON_COMMAND(ID_MACRO_REPLAY9, OnMacroReplay9)
	ON_COMMAND(ID_MACRO_REPLAY0, OnMacroReplay0)
	ON_UPDATE_COMMAND_UI(ID_MACRO_REPLAY, OnUpdateMacroReplay)
	ON_UPDATE_COMMAND_UI(ID_MACRO_REPLAY1, OnUpdateMacroReplay1)
	ON_UPDATE_COMMAND_UI(ID_MACRO_REPLAY2, OnUpdateMacroReplay2)
	ON_UPDATE_COMMAND_UI(ID_MACRO_REPLAY3, OnUpdateMacroReplay3)
	ON_UPDATE_COMMAND_UI(ID_MACRO_REPLAY4, OnUpdateMacroReplay4)
	ON_UPDATE_COMMAND_UI(ID_MACRO_REPLAY5, OnUpdateMacroReplay5)
	ON_UPDATE_COMMAND_UI(ID_MACRO_REPLAY6, OnUpdateMacroReplay6)
	ON_UPDATE_COMMAND_UI(ID_MACRO_REPLAY7, OnUpdateMacroReplay7)
	ON_UPDATE_COMMAND_UI(ID_MACRO_REPLAY8, OnUpdateMacroReplay8)
	ON_UPDATE_COMMAND_UI(ID_MACRO_REPLAY9, OnUpdateMacroReplay9)
	ON_UPDATE_COMMAND_UI(ID_MACRO_REPLAY0, OnUpdateMacroReplay0)
	ON_COMMAND(ID_VIEW_SPELL_CHECK, OnViewSpellCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SPELL_CHECK, OnUpdateViewSpellCheck)
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_COMMAND_EXECUTE0, OnCommandExecute0)
	ON_COMMAND(ID_COMMAND_EXECUTE1, OnCommandExecute1)
	ON_COMMAND(ID_COMMAND_EXECUTE2, OnCommandExecute2)
	ON_COMMAND(ID_COMMAND_EXECUTE3, OnCommandExecute3)
	ON_COMMAND(ID_COMMAND_EXECUTE4, OnCommandExecute4)
	ON_COMMAND(ID_COMMAND_EXECUTE5, OnCommandExecute5)
	ON_COMMAND(ID_COMMAND_EXECUTE6, OnCommandExecute6)
	ON_COMMAND(ID_COMMAND_EXECUTE7, OnCommandExecute7)
	ON_COMMAND(ID_COMMAND_EXECUTE8, OnCommandExecute8)
	ON_COMMAND(ID_COMMAND_EXECUTE9, OnCommandExecute9)
	ON_UPDATE_COMMAND_UI(ID_COMMAND_EXECUTE0, OnUpdateCommandExecute0)
	ON_UPDATE_COMMAND_UI(ID_COMMAND_EXECUTE1, OnUpdateCommandExecute1)
	ON_UPDATE_COMMAND_UI(ID_COMMAND_EXECUTE2, OnUpdateCommandExecute2)
	ON_UPDATE_COMMAND_UI(ID_COMMAND_EXECUTE3, OnUpdateCommandExecute3)
	ON_UPDATE_COMMAND_UI(ID_COMMAND_EXECUTE4, OnUpdateCommandExecute4)
	ON_UPDATE_COMMAND_UI(ID_COMMAND_EXECUTE5, OnUpdateCommandExecute5)
	ON_UPDATE_COMMAND_UI(ID_COMMAND_EXECUTE6, OnUpdateCommandExecute6)
	ON_UPDATE_COMMAND_UI(ID_COMMAND_EXECUTE7, OnUpdateCommandExecute7)
	ON_UPDATE_COMMAND_UI(ID_COMMAND_EXECUTE8, OnUpdateCommandExecute8)
	ON_UPDATE_COMMAND_UI(ID_COMMAND_EXECUTE9, OnUpdateCommandExecute9)
	ON_COMMAND(ID_EDIT_UPPER_CASE, OnEditUpperCase)
	ON_COMMAND(ID_EDIT_LOWER_CASE, OnEditLowerCase)
	ON_COMMAND(ID_EDIT_CAPITALIZE, OnEditCapitalize)
	ON_COMMAND(ID_EDIT_INVERT_CASE, OnEditInvertCase)
	ON_COMMAND(ID_SRCH_PREV_EDIT, OnSearchPrevEdit)
	ON_COMMAND(ID_SRCH_PAIRS_BEGIN, OnSearchPairsBegin)
	ON_COMMAND(ID_SRCH_PAIRS_END, OnSearchPairsEnd)
	ON_COMMAND(ID_SRCH_PAIRS_BEGIN_SELECT, OnSearchPairsBeginSelect)
	ON_COMMAND(ID_SRCH_PAIRS_END_SELECT, OnSearchPairsEndSelect)
	ON_COMMAND(ID_EDIT_MAKE_COMMENT, OnEditMakeComment)
	ON_COMMAND(ID_EDIT_RELEASE_COMMENT, OnEditReleaseComment)
	ON_COMMAND(ID_MOVE_CARET_LEFT, OnMoveCaretLeft)
	ON_COMMAND(ID_MOVE_CARET_RIGHT, OnMoveCaretRight)
	ON_COMMAND(ID_MOVE_CARET_UP, OnMoveCaretUp)
	ON_COMMAND(ID_MOVE_CARET_DOWN, OnMoveCaretDown)
	ON_COMMAND(ID_TOOL_MS_DOS_SHELL, OnToolMsDosShell)
	ON_COMMAND(ID_TOOL_VIEW_IN_BROWSER, OnToolViewInBrowser)
	ON_COMMAND(ID_EDIT_INCREASE_INDENT, OnEditIncreaseIndent)
	ON_COMMAND(ID_EDIT_DECREASE_INDENT, OnEditDecreaseIndent)
	ON_COMMAND(ID_EDIT_SELECT_BLOCK, OnEditSelectBlock)
	ON_COMMAND(ID_EDIT_CONVERT_TABS_TO_SPACES, OnEditConvertTabsToSpaces)
	ON_COMMAND(ID_EDIT_REMOVE_TRAILING_SPACES, OnEditRemoveTrailingSpaces)
	ON_COMMAND(ID_EDIT_DELETE_WORD, OnEditDeleteWord)
	ON_COMMAND(ID_EDIT_DELETE_TO_END_OF_LINE, OnEditDeleteToEndOfLine)
	ON_COMMAND(ID_EDIT_DELETE_PREV_WORD, OnEditDeletePrevWord)
	ON_COMMAND(ID_COMMAND_SEND_INPUT, OnCommandSendInput)
	ON_COMMAND(ID_COMMAND_KILL_PROCESS, OnCommandKillProcess)
	ON_COMMAND(ID_TOOL_EVALUATE_LINE, OnToolEvaluateLine)
	ON_COMMAND(ID_SRCH_NEXT_WORD, OnSearchNextWord)
	ON_COMMAND(ID_SRCH_PREV_WORD, OnSearchPrevWord)
	ON_COMMAND(ID_SRCH_REPALL_IN_SELECTION, OnSearchReplaceAllInSelection)
	ON_COMMAND(ID_SRCH_REPALL_IN_FILE, OnSearchReplaceAllInFile)
	ON_COMMAND(ID_SRCH_REPALL_IN_OPEN_FILES, OnSearchReplaceAllInOpenFiles)
	ON_COMMAND(ID_SRCH_REPALL_IN_FILE_OUTPUT, OnSearchReplaceAllInFileOutputWindow)
	ON_COMMAND(ID_SRCH_REPLACE_THIS, OnSearchReplaceThisOccurrence)
	ON_COMMAND(ID_EDIT_CONVERT_SPACES_TO_TABS, OnEditConvertSpacesToTabs)
	ON_COMMAND(ID_EDIT_LEADING_SPACES_TO_TABS, OnEditLeadingSpacesToTabs)
	ON_COMMAND(ID_EDIT_COPY_FILE_PATH, OnEditCopyFilePath)
	ON_COMMAND(ID_MOVE_CARET_HOME, OnMoveCaretHome)
	ON_COMMAND(ID_MOVE_CARET_END, OnMoveCaretEnd)
	ON_COMMAND(ID_MOVE_CARET_PRIOR, OnMoveCaretPrior)
	ON_COMMAND(ID_MOVE_CARET_NEXT, OnMoveCaretNext)
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_EDIT_DELETE_TO_BEGIN_OF_LINE, OnEditDeleteToBeginOfLine)
	ON_COMMAND(ID_SRCH_NEXT_OCCURRENCE, OnSearchNextOccurrence)
	ON_COMMAND(ID_SRCH_PREV_OCCURRENCE, OnSearchPrevOccurrence)
	ON_COMMAND(ID_SRCH_FIND_NEXT, OnSearchFindNext)
	ON_COMMAND(ID_SRCH_FIND_PREV, OnSearchFindPrev)
	ON_UPDATE_COMMAND_UI(ID_SRCH_FIND_NEXT, OnUpdateSearchFindNext)
	ON_UPDATE_COMMAND_UI(ID_SRCH_FIND_PREV, OnUpdateSearchFindPrev)
	ON_COMMAND(ID_SCRL_SCREEN_TOP, OnScrollScreenTop)
	ON_COMMAND(ID_SCRL_SCREEN_BOTTOM, OnScrollScreenBottom)
	ON_COMMAND(ID_SCRL_SCREEN_CENTER, OnScrollScreenCenter)
	ON_WM_MOUSEWHEEL()
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_COMMAND_SEND_INPUT, OnUpdateCommandSendInput)
	ON_UPDATE_COMMAND_UI(ID_COMMAND_KILL_PROCESS, OnUpdateCommandKillProcess)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCedtView static member variables

// general preferences
COLORREF CCedtView::m_crBkgrColor[10], CCedtView::m_crTextColor[40];
LOGFONT  CCedtView::m_lfScreen[10], CCedtView::m_lfPrinter[10];
LOGFONT  CCedtView::m_lfMiscel[10];
INT  CCedtView::m_nCurrentScreenFont, CCedtView::m_nCurrentPrinterFont;
BOOL CCedtView::m_bUsingColumnModeFont = FALSE;

BOOL CCedtView::m_bShowLineNumbers, CCedtView::m_bSyntaxHighlight;
BOOL CCedtView::m_bEmboldenKeywords, CCedtView::m_bItalicizeComment;
BOOL CCedtView::m_bHighlightActiveLine, CCedtView::m_bEnablePairsMatching;
BOOL CCedtView::m_bEnableAutoIndent, CCedtView::m_bShowSelectionMargin;

BOOL CCedtView::m_bHomeKeyGoesToFirstPosition, CCedtView::m_bSearchWrapAtEndOfFile;
BOOL CCedtView::m_bEnableDragAndDropEditing, CCedtView::m_bCopyCurrentLineIfNothingSelected = FALSE;
BOOL CCedtView::m_bShowSpaces, CCedtView::m_bShowTabChars, CCedtView::m_bShowLineBreak;
BOOL CCedtView::m_bUseSpacesInPlaceOfTab, CCedtView::m_bOpenDocumentWordWrapped;

INT  CCedtView::m_nTabSize, CCedtView::m_nIndentationSize;
INT  CCedtView::m_nFixedWrapWidth, CCedtView::m_nWrapIndentation;
INT  CCedtView::m_nLineSpacing, CCedtView::m_nLineNumberDigits;
INT  CCedtView::m_nCaretWidthRatio, CCedtView::m_nFileTabLength;

BOOL CCedtView::m_bShowColumnMarker1, CCedtView::m_bShowColumnMarker2;
INT  CCedtView::m_nColumnMarker1Pos, CCedtView::m_nColumnMarker2Pos;

// printing preferences
RECT CCedtView::m_rectPageMargin;
BOOL CCedtView::m_bPrintLineNumbers, CCedtView::m_bPrintSyntaxHighlight;
BOOL CCedtView::m_bPrintHeader, CCedtView::m_bPrintFooter;
CString CCedtView::m_szHeaderFormat[3], CCedtView::m_szFooterFormat[3];
CString CCedtView::m_szHeaderString[3], CCedtView::m_szFooterString[3];

// global view mode
BOOL CCedtView::m_bOverwriteMode = FALSE;
BOOL CCedtView::m_bColumnMode = FALSE;

// search related variables
BOOL CCedtView::m_bReplaceSearch = FALSE;
CString CCedtView::m_szFindString, CCedtView::m_szReplaceString;
UINT CCedtView::m_nSearchOptions;
CRegExp CCedtView::m_clsRegExp;

// user commands
CUserCommand CCedtView::m_clsUserCommand[11];
CString CCedtView::m_szUserCommandFilePath[8];

// user command input & output
BOOL CCedtView::m_bChildOutputLineContinue = FALSE;
CStringArray CCedtView::m_arrChildInputString;

// redirect handles
HANDLE CCedtView::m_hChildStdinWrDup = NULL, CCedtView::m_hChildStdoutRdDup = NULL;
HANDLE CCedtView::m_hChildProcess = NULL, CCedtView::m_hChildThread = NULL;

// user macro buffers
CMacroBuffer CCedtView::m_clsMacroBuffer[11];
CString CCedtView::m_szMacroBufferFilePath[8];

// macro recording variables
BOOL CCedtView::m_bMacroRecording = FALSE;
INT CCedtView::m_nMacroRecordingNumber;
CString CCedtView::m_szMacroName;

// global screen resources
CDC CCedtView::m_dcScreen;
CBitmap CCedtView::m_bmScreen;
CFont CCedtView::m_fontScreen, CCedtView::m_fontScreenIt;
CFont CCedtView::m_fontPrinter, CCedtView::m_fontPrinterIt;
INT CCedtView::m_nScreenFontChangeSequence = 0;

// window placement
WINDOWPLACEMENT * CCedtView::m_lpWindowPlacement = NULL;

/////////////////////////////////////////////////////////////////////////////
// CCedtView construction/destruction

CCedtView::CCedtView()
{
	// view status
	m_bHaveFocus = m_bAutoScroll = m_bLineSelect = FALSE;
	m_szPrevClientSize = CSize(0, 0);
	m_ptRButtonUpPoint = CPoint(0, 0);

	// local display options
	m_bLocalWordWrap = m_bLocalSpellCheck = FALSE;

	// Drag and Drop
	m_bDragDataSource = m_bDragDropOccured = FALSE;
}

CCedtView::~CCedtView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CCedtView drawing

BOOL CCedtView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CCedtView::OnDraw(CDC* pDC)
{
	// check if this view is active
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CChildFrame * pChild = (CChildFrame *)GetParentFrame();
	BOOL bActiveView = ( pMainFrame->MDIGetActive() == pChild );

	// hide caret before drawing screen
	if( bActiveView && m_bHaveFocus ) HideCaret();

	// update scroll bar
	UpdateScrollBars();

	// prepare screen resources for drawing
	m_dcScreen.SetMapMode( MM_TEXT );
	m_dcScreen.SetBkMode( TRANSPARENT );

	// draw screen !
	DrawScreenBackgroundAndText();
	if( m_bSelected || m_bComposition ) InvertScreenSelected();

	m_bActiveLineHighlighted = FALSE;
	if( m_bHighlightActiveLine || m_crBkgrColor[0] != m_crBkgrColor[2] ) DrawActiveLineHighlight();
	if( m_bShowColumnMarker1 || m_bShowColumnMarker2 ) DrawColumnMarkerHighlight();

	RECT rect; GetClientRect( & rect );
	pDC->BitBlt(0, 0, rect.right, rect.bottom, & m_dcScreen, 0, 0, SRCCOPY);

	// there is no pairs highlighted
	m_bPairHighlighted[0] = m_bPairHighlighted[1] = FALSE;

	if( bActiveView && m_bHaveFocus ) {
		ShowCaret(); // make caret visible
		UpdateCaretPosition(); // create new caret and update caret position
	}

	if( bActiveView ) UpdateFileStatusPane(); // update file format & read only attribute
	UpdateMDIFileTabOfTheDocument(); // update file selection tab
}


/////////////////////////////////////////////////////////////////////////////
// CCedtView diagnostics

#ifdef _DEBUG
void CCedtView::AssertValid() const
{
	CView::AssertValid();
}

void CCedtView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCedtView member functions

// this function is called from CMainFrame::OnSize()
void CCedtView::OnSizeMainWindow(int cx, int cy)
{
	CClientDC dc(AfxGetMainWnd());

	if( m_dcScreen.m_hDC ) {
		m_bmScreen.DeleteObject();
		m_dcScreen.DeleteDC();
	}

	m_dcScreen.CreateCompatibleDC( & dc );
	m_bmScreen.CreateCompatibleBitmap( & dc, cx, cy );
	m_dcScreen.SelectObject( & m_bmScreen );

	CreateScreenFontObject();
	ApplyCurrentScreenFont();
}

void CCedtView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	CCedtApp * pApp = (CCedtApp *)AfxGetApp(); ASSERT( pApp );
	INT nTotalViewCount = pApp->GetViewCount();

	CCedtDoc * pDoc = (CCedtDoc *)GetDocument(); ASSERT( pDoc );
	INT nViewCount = pDoc->GetViewCount();

	CChildFrame * pChild = (CChildFrame *)GetParentFrame(); ASSERT( pChild );
	INT nPaneCount = pChild->GetPaneCount();

	// set window placement and maximize child frame if this is the first view
	if( m_lpWindowPlacement ) pChild->SetWindowPlacement( m_lpWindowPlacement );
	else if( nTotalViewCount < 2 ) pChild->MDIMaximize();

	CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pFrame );
	CCedtView * pClone = NULL;

	if( ! pClone && nPaneCount > 1 ) pClone = (CCedtView *)pChild->GetNeighborPane(this);
	if( ! pClone && nViewCount > 1 ) pClone = (CCedtView *)pFrame->MDIGetActiveView();

	// check if pClone has same document object, otherwise ignore it.
	if( pClone && pClone->GetDocument() != pDoc ) pClone = NULL; 

	// caret position
	if( pClone ) { 
		m_nScrollPosX = pClone->m_nScrollPosX;	m_nScrollPosY = pClone->m_nScrollPosY;
		m_nAnchorPosX = pClone->m_nAnchorPosX;	m_nAnchorPosY = pClone->m_nAnchorPosY;
		m_nCaretPosX  = pClone->m_nCaretPosX;	m_nCaretPosY  = pClone->m_nCaretPosY;
		m_nCaretPosXFixed = pClone->m_nCaretPosXFixed;
	} else {
		m_nScrollPosX = m_nScrollPosY = 0;
		m_nAnchorPosX = m_nAnchorPosY = 0;
		m_nCaretPosX  = m_nCaretPosY  = 0;
		m_nCaretPosXFixed = 0;
	}

	// display options
	if( pClone ) {
		m_bLocalWordWrap = pClone->m_bLocalWordWrap;
		m_bLocalSpellCheck = pClone->m_bLocalSpellCheck;
	} else {
		m_bLocalWordWrap = m_bColumnMode ? FALSE : m_bOpenDocumentWordWrapped;
		m_bLocalSpellCheck = FALSE;
	}

	// active line highlight
	m_bActiveLineHighlighted = FALSE;
	m_nActiveLineHighlightedPosY = 0;

	// pairs highlight
	m_bPairMatched = FALSE; m_bBeginningPair = FALSE;
	m_nPairPosX[0] = m_nPairPosX[1] = m_nPairPosY[0] = m_nPairPosY[1] = 0;
	m_bPairHighlighted[0] = m_bPairHighlighted[1] = FALSE;

	// imm composition
	m_bComposition = FALSE;


	// saved index (save & restore caret and scroll pos)
	m_nCaretIdxX = m_nCaretIdxY = 0;
	m_nAnchorIdxX = m_nAnchorIdxY = 0;
	m_nScrollIdxX = m_nScrollIdxY = 0;

	// saved selection
	m_nSaveSelBegX = m_nSaveSelBegY = 0;
	m_nSaveSelEndX = m_nSaveSelEndY = 0;
	m_bSaveSelected = m_bSelected = FALSE;

	// update file selection tab
	UpdateMDIFileTabOfTheDocument();

	// initial formatting
	FormatScreenText();
}

void CCedtView::Reinitialize() 
{
	// caret position
	m_nScrollPosX = m_nScrollPosY = 0;
	m_nAnchorPosX = m_nAnchorPosY = 0;
	m_nCaretPosX  = m_nCaretPosY  = 0;
	m_nCaretPosXFixed = 0;

	// display options
//	m_bLocalWordWrap = m_bColumnMode ? FALSE : m_bWordWrap;
//	m_bLocalSpellCheck = FALSE;

	// active line highlight
	m_bActiveLineHighlighted = FALSE;
	m_nActiveLineHighlightedPosY = 0;

	// pairs highlight
	m_bPairMatched = FALSE; m_bBeginningPair = FALSE;
	m_nPairPosX[0] = m_nPairPosX[1] = m_nPairPosY[0] = m_nPairPosY[1] = 0;
	m_bPairHighlighted[0] = m_bPairHighlighted[1] = FALSE;

	// imm composition
	m_bComposition = FALSE;

	// saved index (save & restore caret and scroll pos)
	m_nCaretIdxX = m_nCaretIdxY = 0;
	m_nAnchorIdxX = m_nAnchorIdxY = 0;
	m_nScrollIdxX = m_nScrollIdxY = 0;

	// saved selection
	m_nSaveSelBegX = m_nSaveSelBegY = 0;
	m_nSaveSelEndX = m_nSaveSelEndY = 0;
	m_bSaveSelected = m_bSelected = FALSE;

	// update file selection tab
	UpdateMDIFileTabOfTheDocument();

	// initial formatting
	FormatScreenText();
}

void CCedtView::UpdateScrollBars()
{
	INT nLineCount = m_clsFormatedScreenText.GetCount();
	if( ! nLineCount ) return;

	RECT rect; GetClientRect( & rect );
	INT nLineHeight = GetLineHeight();
	INT nLeftMargin = GetLeftMargin();
	INT nAveCharWidth = GetAveCharWidth();

	if( m_nScrollPosY < 0 ) m_nScrollPosY = 0;
	if( m_nScrollPosY > (nLineCount-1) * nLineHeight ) m_nScrollPosY = (nLineCount-1) * nLineHeight;

	UINT nScalingFactor = 32; // maximum scaling factor
	if     ( nLineCount <  32768 ) nScalingFactor =  1;
	else if( nLineCount <  65536 ) nScalingFactor =  2;
	else if( nLineCount < 131072 ) nScalingFactor =  4;
	else if( nLineCount < 262144 ) nScalingFactor =  8;
	else if( nLineCount < 524288 ) nScalingFactor = 16;

	UINT nPage = rect.bottom / nLineHeight / nScalingFactor;
	INT nMax = (nLineCount - 1) / nScalingFactor + nPage - 1;
	INT nPos = m_nScrollPosY / nLineHeight / nScalingFactor;

	SCROLLINFO vsi; GetScrollInfo( SB_VERT, & vsi );
	vsi.fMask |= SIF_DISABLENOSCROLL;
	if( vsi.nMax != nMax || vsi.nPage != nPage ) {
		vsi.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
		vsi.nMin = 0; vsi.nMax = nMax;
		vsi.nPage = nPage; vsi.nPos = nPos;
		SetScrollInfo( SB_VERT, & vsi );
	} else if( vsi.nPos != nPos ) SetScrollPos( SB_VERT, nPos );

	INT nMaxCharCount = 4096;
	if( m_nScrollPosX < 0 ) m_nScrollPosX = 0;

	nPage = (rect.right - nLeftMargin) / nAveCharWidth;
	nMax = nMaxCharCount + nPage - 2;
	nPos = m_nScrollPosX / nAveCharWidth;

	SCROLLINFO hsi; GetScrollInfo( SB_HORZ, & hsi );
	if( hsi.nMax != nMax || hsi.nPage != nPage ) {
		hsi.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
		hsi.nMin = 0; hsi.nMax = nMax;
		hsi.nPage = nPage; hsi.nPos = nPos;
		SetScrollInfo( SB_HORZ, & hsi );
	} else if( hsi.nPos != nPos ) SetScrollPos( SB_HORZ, nPos );
}

void CCedtView::UpdateCaretPosition()
{
	// check if this view is ready
	if( ! m_clsFormatedScreenText.GetCount() ) return;

	// hide and destroy caret
	HideCaret();
	DestroyCaret();

//	if( m_bPairHighlighted[0] || m_bPairHighlighted[1] ) UnhighlightPairs();
	if( m_bPairMatched ) UnhighlightPairs();
	if( m_bActiveLineHighlighted ) UnhighlightActiveLine();

	if( m_bShowColumnMarker1 || m_bShowColumnMarker2 ) HighlightColumnMarker();
	if( m_bHighlightActiveLine || m_crBkgrColor[0] != m_crBkgrColor[2] ) HighlightActiveLine();
//	if( m_bEnablePairsMatching && ! m_bSelected ) HighlightMatchingPairs();
	if( m_bEnablePairsMatching ) HighlightMatchingPairs();

	RECT rect; GetClientRect( & rect );
	INT nLeftMargin = GetLeftMargin();

	CFormatedString & rLine = GetLineFromPosY(m_nCaretPosY);
	INT nIdxY = GetIdxYFromPosY(m_nCaretPosY);
	INT nIdxX = GetIdxXFromPosX(rLine, m_nCaretPosX);
	INT nMaxY = GetLastIdxY();

	// create new caret
	INT nCaretHeight = GetCharHeight() - 1;
	INT nCaretWidth = m_nCaretWidthRatio * GetAveCharWidth() / 100;
	if( m_bOverwriteMode && ! m_bSelected ) nCaretWidth = GetCharWidth();
	CreateSolidCaret(nCaretWidth, nCaretHeight);

	// set caret position & show caret
	INT nPosX = m_nCaretPosX - m_nScrollPosX + nLeftMargin;
	INT nPosY = m_nCaretPosY - m_nScrollPosY;

	if( nPosX < nLeftMargin || nPosX > rect.right || nPosY < 0 || nPosY > rect.bottom 
		|| m_bComposition ) ::SetCaretPos(-10000, -10000);
	else ::SetCaretPos(nPosX, nPosY+1);

	ShowCaret();

	// update status bar caret information
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	pMainFrame->SetCaretPositionInfo(nIdxY+1, nIdxX+1, nMaxY+1);
}

void CCedtView::UpdateDragPosition()
{
	// check if this view is ready
	if( ! m_clsFormatedScreenText.GetCount() ) return;

	// hide and destroy caret
	HideCaret();
	DestroyCaret();

	RECT rect; GetClientRect( & rect );
	INT nLeftMargin = GetLeftMargin();

	CFormatedString & rLine = GetLineFromPosY(m_nCaretPosY);
	INT nIdxY = GetIdxYFromPosY(m_nCaretPosY);
	INT nIdxX = GetIdxXFromPosX(rLine, m_nCaretPosX);
	INT nMaxY = GetLastIdxY();

	// create new caret
	INT nCaretHeight = GetCharHeight() - 1;
	INT nCaretWidth = 25 * GetAveCharWidth() / 100;
	CreateGrayCaret(nCaretWidth, nCaretHeight);

	// set caret position & show caret
	INT nPosX = m_nDragPosX - m_nScrollPosX + nLeftMargin;
	INT nPosY = m_nDragPosY - m_nScrollPosY;

	if( nPosX < nLeftMargin || nPosX > rect.right || nPosY < 0 || nPosY > rect.bottom 
		|| m_bComposition ) ::SetCaretPos(-10000, -10000);
	else ::SetCaretPos(nPosX, nPosY+1);

	ShowCaret();
}

void CCedtView::UpdateFileStatusPane()
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pMainFrame->SetFileStatusInfo( pDoc->m_nEncodingType, pDoc->m_nFileFormat, pDoc->IsReadOnlyFile() );
}

void CCedtView::UpdateMDIFileTabOfTheDocument()
{
//	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
//	CChildFrame * pChild = (CChildFrame *)GetParentFrame();
//	pMainFrame->UpdateFileTab( pChild );

	CCedtDoc * pDoc = (CCedtDoc *)GetDocument(); ASSERT( pDoc );
	pDoc->UpdateMDIFileTab();
}

INT CCedtView::GetCompositionString(DWORD dwIndex, CString & szString)
{
	HIMC hImc = ImmGetContext( m_hWnd ); CHAR buf[1024];
	INT nSize = ImmGetCompositionString( hImc, dwIndex, buf, 1024 );
	ImmReleaseContext( m_hWnd, hImc ); 
	buf[nSize] = '\0'; szString = buf;
	return nSize;
}

INT CCedtView::GetCurrentLineNumber()
{
	return GetIdxYFromPosY(m_nCaretPosY) + 1;
}

CString CCedtView::GetCurrentWord()
{
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX );

	INT nBegX, nEndX; GetCurrWordRange( rLine, nIdxX, nBegX, nEndX );
	if( nEndX - nBegX > 0 ) {
		INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
		CAnalyzedString & rString = GetLineFromIdxY( nIdxY );
		return rString.Mid( nBegX, nEndX-nBegX );
	} else return "";
}

CString CCedtView::GetSelectedString()
{
	INT nBegX, nBegY, nEndX, nEndY; GetSelectedIndex(nBegX, nBegY, nEndX, nEndY);
	if( nBegY == nEndY ) {
		CAnalyzedString & rString = GetLineFromIdxY( nBegY );
		return rString.Mid( nBegX, nEndX-nBegX );
	} else return "";
}

BOOL CCedtView::TurnOffWordWrapMode()
{
	if( m_bLocalWordWrap ) {
		SendMessage(WM_COMMAND, ID_VIEW_WORD_WRAP, 0L);
		return TRUE;
	} else return FALSE;
}

void CCedtView::UpdateAllPanesInTheSameRow()
{
	CChildFrame * pChild = (CChildFrame *)GetParentFrame(); ASSERT( pChild );
	pChild->UpdateAllPanesInTheSameRow( this );
}

void CCedtView::UpdateAllPanesSharingScrollBar()
{
	CChildFrame * pChild = (CChildFrame *)GetParentFrame(); ASSERT( pChild );
	pChild->UpdateAllPanesSharingScrollBar( this );
}

void CCedtView::UpdateAllViews()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument(); ASSERT( pDoc );
	pDoc->UpdateAllViews( this );
}


/////////////////////////////////////////////////////////////////////////////
// CCedtView message handlers
int CCedtView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1) return -1;

	// register this view as drop target
	m_oleDropTarget.Register(this);
	
	return 0;
}

BOOL CCedtView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if( nHitTest == HTCLIENT ) {
		CPoint point; GetCursorPos( & point ); ScreenToClient( & point );

		if( m_bEnableDragAndDropEditing && m_bSelected && ! m_bAutoScroll ) {
			if( point.x < GetLeftMargin() ) SetCursor( CCedtApp::m_hCursorRightArrow );
			else if( IsPointInSelection(point) ) SetCursor( CCedtApp::m_hCursorArrow );
			else if( IsMacroRecording() ) SetCursor( CCedtApp::m_hCursorIBeamMacro );
			else SetCursor( CCedtApp::m_hCursorIBeam );
		} else {
			if( point.x < GetLeftMargin() ) SetCursor( CCedtApp::m_hCursorRightArrow );
			else if( IsMacroRecording() ) SetCursor( CCedtApp::m_hCursorIBeamMacro );
			else SetCursor( CCedtApp::m_hCursorIBeam );
		}

		return TRUE;
	}

	return CView::OnSetCursor(pWnd, nHitTest, message);
}

void CCedtView::OnSetFocus(CWnd* pOldWnd) 
{
	CView::OnSetFocus(pOldWnd);

	ShowCaret();
	m_bHaveFocus = TRUE;

	UpdateCaretPosition(); // create new caret and update caret position
	UpdateFileStatusPane(); // update file format & read only attribute
}

void CCedtView::OnKillFocus(CWnd* pNewWnd) 
{
	HideCaret();
	m_bHaveFocus = FALSE;

	CView::OnKillFocus(pNewWnd);
}

void CCedtView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	CClientDC dc( this );

	if( m_dcActiveLine.m_hDC ) {
		m_bmActiveLine.DeleteObject();
		m_dcActiveLine.DeleteDC();
	}

	m_dcActiveLine.CreateCompatibleDC( & dc );
	m_bmActiveLine.CreateCompatibleBitmap( & dc, cx, 2 * GetCharHeight() + 2 );
	m_dcActiveLine.SelectObject( & m_bmActiveLine ); 

	// check if not initialized yet
	if( ! m_clsFormatedScreenText.GetCount() ) return; 

	if( m_szPrevClientSize.cx != cx && m_bLocalWordWrap && ! m_nFixedWrapWidth ) {
		SaveCaretAndAnchorPos();
		FormatScreenText();
		RestoreCaretAndAnchorPos();
	}

	m_szPrevClientSize.cx = cx;
	m_szPrevClientSize.cy = cy;
}

// this function is called when screen font is changed
void CCedtView::OnScreenFontChange()
{
	CRect rect; GetClientRect( & rect );
	CClientDC dc( this );

	if( m_dcActiveLine.m_hDC ) {
		m_bmActiveLine.DeleteObject();
		m_dcActiveLine.DeleteDC();
	}

	m_dcActiveLine.CreateCompatibleDC( & dc );
	m_bmActiveLine.CreateCompatibleBitmap( & dc, rect.Width(), 2 * (GetLineHeight() + 1) );
	m_dcActiveLine.SelectObject( & m_bmActiveLine ); 
}

void CCedtView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	INT nLineCount = m_clsFormatedScreenText.GetCount();
	if( ! nLineCount ) return; // this view is not ready...

	RECT rect; GetClientRect( & rect );
	INT nLineHeight = GetLineHeight();
	INT nPageHeight = (rect.bottom / nLineHeight) * nLineHeight;

	UINT nScalingFactor = 32; // maximum scaling factor
	if     ( nLineCount <  32768 ) nScalingFactor =  1;
	else if( nLineCount <  65536 ) nScalingFactor =  2;
	else if( nLineCount < 131072 ) nScalingFactor =  4;
	else if( nLineCount < 262144 ) nScalingFactor =  8;
	else if( nLineCount < 524288 ) nScalingFactor = 16;

	switch( nSBCode ) {
	case SB_LINEUP:		SetScrollPosY(m_nScrollPosY - nLineHeight); break;
	case SB_LINEDOWN:	SetScrollPosY(m_nScrollPosY + nLineHeight); break;
	case SB_PAGEUP:		SetScrollPosY(m_nScrollPosY - nPageHeight); break;
	case SB_PAGEDOWN:	SetScrollPosY(m_nScrollPosY + nPageHeight); break;
	case SB_THUMBTRACK:	SetScrollPosY(nPos * nScalingFactor * nLineHeight); break;
	}

	Invalidate(); UpdateWindow();
}

void CCedtView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	INT nLineCount = m_clsFormatedScreenText.GetCount();
	if( ! nLineCount ) return; // this view is not ready

	RECT rect; GetClientRect( & rect );
	INT nLeftMargin = GetLeftMargin();
	INT nAveCharWidth = GetAveCharWidth();
	INT nPageWidth = ((rect.right - nLeftMargin) / nAveCharWidth) * nAveCharWidth;

	switch( nSBCode ) {
	case SB_LINELEFT:	SetScrollPosX(m_nScrollPosX - nAveCharWidth); break;
	case SB_LINERIGHT:	SetScrollPosX(m_nScrollPosX + nAveCharWidth); break;
	case SB_PAGELEFT:	SetScrollPosX(m_nScrollPosX - nPageWidth); break;
	case SB_PAGERIGHT:	SetScrollPosX(m_nScrollPosX + nPageWidth); break;
	case SB_THUMBTRACK:	SetScrollPosX(nPos * nAveCharWidth); break;
	}

	Invalidate(); UpdateWindow();
}

BOOL CCedtView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	INT nLineCount = m_clsFormatedScreenText.GetCount();
	if( ! nLineCount ) return TRUE; // this view is not ready

	INT nLineHeight = GetLineHeight();
	INT nDelta = 3 * abs(zDelta) / WHEEL_DELTA;

	if( zDelta > 0 ) SetScrollPosSyncAllPanes(m_nScrollPosX, m_nScrollPosY - nDelta * nLineHeight);
	else SetScrollPosSyncAllPanes(m_nScrollPosX, m_nScrollPosY + nDelta * nLineHeight);

	Invalidate(); UpdateWindow();
	UpdateAllPanesInTheSameRow();

	return TRUE;
}

void CCedtView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if( m_bSelected && point.x >= GetLeftMargin() && IsPointInSelection(point) ) {
		// do not set caret position the cursor is on the selection
	} else {
		// set caret position from mouse pointer
		SetCaretPosFromMouse(point);

		if( ! IsCaretVisible() ) {
			m_bSelected = FALSE;
			SetScrollPosToMakeCaretVisible();
			Invalidate(); UpdateWindow();
			UpdateAllPanesSharingScrollBar();
		} else if( m_bSelected ) {
			m_bSelected = FALSE;
			Invalidate(); UpdateWindow();
		} else {
			UpdateCaretPosition();
		}
	}

	CView::OnRButtonDown(nFlags, point);
}

void CCedtView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// save point to check context menu is called from mouse or keyboard
	m_ptRButtonUpPoint = point; 

	CView::OnRButtonUp(nFlags, point);
}

void CCedtView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu context, * pMenu = NULL; context.LoadMenu(IDR_VIEW_CONTEXT);
	ScreenToClient( & point );

	if( point != m_ptRButtonUpPoint ) { // not called from mouse but from keyboard
		INT nCaretWidth = m_nCaretWidthRatio * GetAveCharWidth() / 100;
		if( m_bOverwriteMode && ! m_bSelected ) nCaretWidth = GetCharWidth();

		point.x = m_nCaretPosX - m_nScrollPosX + GetLeftMargin() + nCaretWidth / 2; 
		point.y = m_nCaretPosY - m_nScrollPosY + GetLineHeight() / 2;
	}

	if( point.x < GetLeftMargin() ) pMenu = context.GetSubMenu(0);
	else if( ! m_bSelected ) pMenu = context.GetSubMenu(1);
	else pMenu = context.GetSubMenu(2);

	UINT nFlags = TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON;
	ClientToScreen( & point ); 
	pMenu->TrackPopupMenu(nFlags, point.x, point.y, AfxGetMainWnd());
}

void CCedtView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	TRACE0("OnLButtonDown\n");

	if( m_bEnableDragAndDropEditing && ! (nFlags & MK_SHIFT) && m_bSelected && m_bSaveSelected
		&& point.x >= GetLeftMargin() && IsPointInSelection(point) ) {

		CMemText Block; INT nBegX, nBegY, nEndX, nEndY;
		GetSelectedIndex(nBegX, nBegY, nEndX, nEndY);

		if( nBegY == nEndY ) { Block.AddTail(""); CopyToString(Block.GetTail(), nBegX, nBegY, nEndX-nBegX); }
		else if( ! m_bColumnMode ) CopyToLineSelection(Block, nBegX, nBegY, nEndX, nEndY);
		else { GetSelectedPosition(nBegX, nBegY, nEndX, nEndY); CopyToColumnSelection(Block, nBegX, nBegY, nEndX, nEndY); }
		INT size = Block.MemorySize(); 

		HGLOBAL hMemory = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, size); 
		if( ! hMemory ) return;

		void * pMemory = ::GlobalLock(hMemory); 
		if( ! pMemory ) { ::GlobalFree(hMemory); return; }
		
		Block.MemorySave((char *)pMemory, size); 
		::GlobalUnlock(hMemory);

		FORMATETC etc = { CF_TEXT, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		m_oleDataSource.CacheGlobalData(CF_TEXT, hMemory, & etc);

		m_bDragDataSource = TRUE; m_bDragDropOccured = FALSE;
		TRACE0("Before DoDragDrop\n");

		DROPEFFECT eff = m_oleDataSource.DoDragDrop(DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_SCROLL);
		m_bDragDataSource = FALSE;
		TRACE0("After DoDragDrop\n");

		if( eff == DROPEFFECT_NONE && ! m_bDragDropOccured ) {
			SetCaretPosFromMouse(point); OnDragCancel();
			::GlobalFree(hMemory);
		} else if( eff == DROPEFFECT_MOVE && m_bDragDropOccured ) {
			OnDragAndDrop(hMemory);
			::GlobalFree(hMemory);
		} else if( eff == DROPEFFECT_NONE ) {
			// drag and drop has been canceled
			::GlobalFree(hMemory);
		} else if( eff == DROPEFFECT_MOVE ) {
			// data has been moved - delete selection
			OnDragDelete();
		}

		m_oleDataSource.Empty();

	} else {
		// shift + click: select text enclosed from prev caret position and current mouse point
		if( (nFlags & MK_SHIFT) && ! m_bSelected ) { m_nAnchorPosY = m_nCaretPosY; m_nAnchorPosX = m_nCaretPosX; }

		// set caret position from mouse pointer
		SetCaretPosFromMouse(point); 

		// adjust caret position and anchor position relative to saved selection
		if( (nFlags & MK_SHIFT) && m_bSaveSelected ) AdjustCaretAndAnchorPosRelativeToSavedSelection();

		if( point.x < GetLeftMargin() ) {
			if( nFlags & MK_SHIFT ) m_bSelected = (m_nCaretPosY != m_nAnchorPosY || m_nCaretPosX != m_nAnchorPosX);
			else if( ! m_bColumnMode ) m_bSelected = ActionSelectLine();
			else { m_nAnchorPosY = m_nCaretPosY; m_nAnchorPosX = m_nCaretPosX; m_bSelected = FALSE; }
		} else {
			if( nFlags & MK_SHIFT ) m_bSelected = (m_nCaretPosY != m_nAnchorPosY || m_nCaretPosX != m_nAnchorPosX);
			else if( nFlags & MK_CONTROL ) m_bSelected = ActionSelectWord();
			else { m_nAnchorPosY = m_nCaretPosY; m_nAnchorPosX = m_nCaretPosX; m_bSelected = FALSE; }
		}

		if( m_bSelected ) { // set save selected field
			GetSelectedPosition(m_nSaveSelBegX, m_nSaveSelBegY, m_nSaveSelEndX, m_nSaveSelEndY);
			m_bSaveSelected = TRUE;
		} else m_bSaveSelected = FALSE;

		// check if this is line select mouse move
		if( point.x < GetLeftMargin() && ! m_bColumnMode ) m_bLineSelect = TRUE;
		else m_bLineSelect = FALSE;

		SetScrollPosToMakeCaretVisible();
		Invalidate(); UpdateWindow();
		UpdateAllPanesSharingScrollBar();

		// calculate auto non-scroll boundary...
		RECT rect; GetClientRect( & rect );
		INT  nLineHeight = GetLineHeight(), nLinesPerPage = GetLinesPerPage();
		INT  nLeftMargin = GetLeftMargin(), nMaxCharWidth = GetMaxCharWidth();

		m_rectNonScroll.top = rect.top;			m_rectNonScroll.bottom = nLinesPerPage * nLineHeight;
		m_rectNonScroll.left = rect.left;		m_rectNonScroll.right = rect.right - nMaxCharWidth;

		// set timer for auto scroll
		SetTimer( ID_TIMER_AUTO_SCROLL, 100, NULL );
		SetCapture(); m_bAutoScroll = TRUE;
	}

	CView::OnLButtonDown(nFlags, point);
}

void CCedtView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	TRACE0("OnLButtonDblClk\n");

	// set caret position from mouse pointer
	SetCaretPosFromMouse(point);

	if( point.x < GetLeftMargin() ) {
		OnSearchToggleBookmark(); 
		m_bSelected = ActionSelectLine();
	} else {
		m_bSelected = ActionSelectWord();
	}

	if( m_bSelected ) { // set save selected field
		GetSelectedPosition(m_nSaveSelBegX, m_nSaveSelBegY, m_nSaveSelEndX, m_nSaveSelEndY);
		m_bSaveSelected = TRUE;
	} else m_bSaveSelected = FALSE;

	// check if this is line select mouse move
	if( point.x < GetLeftMargin() && ! m_bColumnMode ) m_bLineSelect = TRUE;
	else m_bLineSelect = FALSE;

	SetScrollPosToMakeCaretVisible();
	Invalidate(); UpdateWindow();
	UpdateAllPanesSharingScrollBar();

	// calculate auto non-scroll boundary...
	RECT rect; GetClientRect( & rect );
	INT  nLineHeight = GetLineHeight(), nLinesPerPage = GetLinesPerPage();
	INT  nLeftMargin = GetLeftMargin(), nMaxCharWidth = GetMaxCharWidth();

	m_rectNonScroll.top = rect.top;			m_rectNonScroll.bottom = nLinesPerPage * nLineHeight;
	m_rectNonScroll.left = rect.left;		m_rectNonScroll.right = rect.right - nMaxCharWidth;

	// set timer for auto scroll
	SetTimer( ID_TIMER_AUTO_SCROLL, 100, NULL );
	SetCapture(); m_bAutoScroll = TRUE;

	CView::OnLButtonDblClk(nFlags, point);
}

void CCedtView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	TRACE0("OnLButtonUp\n");

	if( m_bAutoScroll ) {
		KillTimer( ID_TIMER_AUTO_SCROLL );
		ReleaseCapture(); m_bAutoScroll = FALSE;

		if( m_bLineSelect ) {
			AdjustCaretPosAfterLineSelectMouseMove();
			m_bLineSelect = FALSE;
			
			SetScrollPosToMakeCaretVisible();
			Invalidate(); UpdateWindow();
			UpdateAllPanesSharingScrollBar();
		}

		if( m_bSelected ) { // set save selected field
			GetSelectedPosition(m_nSaveSelBegX, m_nSaveSelBegY, m_nSaveSelEndX, m_nSaveSelEndY);
			m_bSaveSelected = TRUE;
		} else m_bSaveSelected = FALSE;
	}

	CView::OnLButtonUp(nFlags, point);
}

void CCedtView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( m_bAutoScroll && (nFlags & MK_LBUTTON) ) {
		if( PtInRect( & m_rectNonScroll, point ) ) {
			// set caret position from mouse pointer
			SetCaretPosFromMouse(point);

			// adjust caret position and anchor position relative to saved selection
			if( m_bSaveSelected ) AdjustCaretAndAnchorPosRelativeToSavedSelection();
			m_bSelected = ( (m_nAnchorPosY != m_nCaretPosY) || (m_nAnchorPosX != m_nCaretPosX) );

			// check if this is line select mouse move
			if( point.x < GetLeftMargin() && ! m_bColumnMode ) m_bLineSelect = TRUE;
			else m_bLineSelect = FALSE;

			Invalidate(); UpdateWindow();
		}

		// set cursor when cursor is moving into the line selection margin.
		OnSetCursor(this, HTCLIENT, 0);
	}
	
	CView::OnMouseMove(nFlags, point);
}

void CCedtView::OnTimerAutoScroll()
{
	if( m_bAutoScroll ) {
		CPoint point; GetCursorPos( & point ); ScreenToClient( & point );

		if( ! PtInRect( & m_rectNonScroll, point ) ) {
			// set caret position from mouse pointer
			SetCaretPosFromMouse(point);

			// adjust caret position and anchor position relative to saved selection
			if( m_bSaveSelected ) AdjustCaretAndAnchorPosRelativeToSavedSelection();
			m_bSelected = ( (m_nAnchorPosY != m_nCaretPosY) || (m_nAnchorPosX != m_nCaretPosX) );

			// check if this is line select mouse move
			if( point.x < GetLeftMargin() && ! m_bColumnMode ) m_bLineSelect = TRUE;
			else m_bLineSelect = FALSE;

			SetScrollPosToMakeAutoScroll();
			Invalidate(); UpdateWindow();
			UpdateAllPanesSharingScrollBar();
		}

		// don't need to set cursor here
		// OnSetCursor(this, HTCLIENT, 0);
	}
}

DROPEFFECT CCedtView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	TRACE0("CCedtView::OnDragEnter\n");

	if( pDataObject->IsDataAvailable(CF_TEXT) ) {
		CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pFrame );
		CChildFrame * pChild = (CChildFrame *)GetParentFrame(); ASSERT( pChild );

		// activate this child frame if it is not active
		if( pFrame->MDIGetActive() != pChild ) pFrame->MDIActivate(pChild);

		// set drag object type
		m_nDragObjectType = DRAG_OBJECT_TEXT;

		// calculate drag non-scroll boundary...
		RECT rect; GetClientRect( & rect );
		INT nLineHeight = GetLineHeight(), nLinesPerPage = GetLinesPerPage();
		INT nLeftMargin = GetLeftMargin(), nMaxCharWidth = GetMaxCharWidth();

		m_rectNonScroll.top = nLineHeight / 2;	m_rectNonScroll.bottom = nLinesPerPage * nLineHeight;
		m_rectNonScroll.left = nLeftMargin;		m_rectNonScroll.right = rect.right - nMaxCharWidth;
		if( rect.bottom % nLineHeight < nLineHeight ) m_rectNonScroll.bottom = nLinesPerPage * nLineHeight - nLineHeight / 2;

		// set drag position from mouse pointer
		SetDragPosFromMouse(point);

		// update drag position
		UpdateDragPosition();

		if( dwKeyState & MK_CONTROL ) return DROPEFFECT_COPY;
		else return DROPEFFECT_MOVE;

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

		return CView::OnDragEnter(pDataObject, dwKeyState, point);
	}
}

DROPEFFECT CCedtView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
//	TRACE0("CCedtView::OnDragOver\n");

	if( m_nDragObjectType == DRAG_OBJECT_TEXT ) {
		// set drag position from mouse pointer
		SetDragPosFromMouse(point);

		// determine whether the point is in the scrolling region, instead of using OnDragScroll()
		if( ! PtInRect( & m_rectNonScroll, point ) ) {
			SetScrollPosToMakeDragScroll();
			Invalidate(); UpdateWindow();
			UpdateAllPanesSharingScrollBar();
		}
		
		// update drag position
		UpdateDragPosition();

		if( dwKeyState & MK_CONTROL ) return DROPEFFECT_COPY;
		else return DROPEFFECT_MOVE;

	} else if( m_nDragObjectType == DRAG_OBJECT_HDROP ) {
		CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pFrame );
		return pFrame->OnDragOver(pDataObject, dwKeyState, point);

	} else if( m_nDragObjectType == DRAG_OBJECT_PROJECT_ITEM ) {
		CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pFrame );
		return pFrame->OnDragOver(pDataObject, dwKeyState, point);

	} else {
		return CView::OnDragOver(pDataObject, dwKeyState, point);
	}
}

void CCedtView::OnDragLeave() 
{
	if( m_nDragObjectType == DRAG_OBJECT_HDROP ) {
		CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pFrame );
		pFrame->OnDragLeave();

	} else if( m_nDragObjectType == DRAG_OBJECT_PROJECT_ITEM ) {
		CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pFrame );
		pFrame->OnDragLeave();
	}

	TRACE0("CCedtView::OnDragLeave\n");
	CView::OnDragLeave();
}

BOOL CCedtView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	TRACE0("CCedtView::OnDrop\n");
	m_bDragDropOccured = TRUE;

	if( pDataObject->IsDataAvailable(CF_TEXT) ) {
		// set drag position from mouse pointer
		SetDragPosFromMouse(point);

		if( m_bDragDataSource && IsPositionInSelection(m_nDragPosX, m_nDragPosY) ) {
			OnDropCancel();
			return FALSE;
		} else if( dropEffect == DROPEFFECT_MOVE && m_bDragDataSource ) {
		/*	Drag and Drop action will be executed when DoDragDrop() is finished */
			return TRUE;
		} else if( dropEffect == DROPEFFECT_COPY ) {
			HGLOBAL hMemory = pDataObject->GetGlobalData(CF_TEXT);
			OnDropPaste(hMemory); ::GlobalFree(hMemory); return TRUE;
		} else if( dropEffect == DROPEFFECT_MOVE ) {
			HGLOBAL hMemory = pDataObject->GetGlobalData(CF_TEXT);
			OnDropPaste(hMemory); ::GlobalFree(hMemory); return TRUE;
		} else return FALSE;

	} else if( pDataObject->IsDataAvailable(CF_HDROP) ) {
		CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pFrame );
		return pFrame->OnDrop(pDataObject, dropEffect, point);

	} else if( pDataObject->IsDataAvailable(g_uClipbrdFormatProjectItem) ) {
		CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pFrame );
		return pFrame->OnDrop(pDataObject, dropEffect, point);

	} else {
		return CView::OnDrop(pDataObject, dropEffect, point);
	}
}

void CCedtView::OnTimer(UINT nIDEvent) 
{
	switch( nIDEvent ) {
	case ID_TIMER_AUTO_SCROLL:
		OnTimerAutoScroll();
		break;
	case ID_TIMER_CAPTURE_OUTPUT:
		OnTimerCaptureOutput();
		break;
	}
	CView::OnTimer(nIDEvent);
}

BOOL CCedtView::PreTranslateMessage(MSG* pMsg) 
{
	static CString szCompositionString;
	static TCHAR szByte[3], cLeadByte = 0x00;
	static UINT nChar, nFlags;

	// translate message for command hot key
	if( TranslateMessageForUserCommand(pMsg) ) return TRUE;

	// translate message for macro hot key
	if( TranslateMessageForMacroBuffer(pMsg) ) return TRUE;


	switch( pMsg->message ) {

	case WM_KEYDOWN: case WM_SYSKEYDOWN:
		switch( pMsg->wParam ) {
		case VK_LEFT:	case VK_RIGHT:	case VK_UP:		case VK_DOWN:
		case VK_HOME:	case VK_END:	case VK_PRIOR:	case VK_NEXT:
			nChar = pMsg->wParam; nFlags = GetKeyState();
			if( (nFlags & KEYSTATE_CONTROL) && m_bSelected && (nChar == VK_UP || nChar == VK_DOWN) ) nFlags |= KEYSTATE_SHIFT;
			OnMoveKeyDown( nChar, nFlags ); 
			break; // do not return
		}
		break;

	case WM_CHAR:
		if( m_bComposition ) { // CHAR during IME composition
			TRACE0("END3\n"); m_bComposition = FALSE;
			OnImeCompositionEnd(FALSE);
		}

		if( g_bDoubleByteCharacterSet && cLeadByte ) {
			szByte[0] = cLeadByte; szByte[1] = pMsg->wParam; szByte[2] = '\0'; cLeadByte = 0x00;
			TRACE1("DBCHAR: [%s]\n", szByte); OnDBCharKeyDown( szByte[0], szByte[1] );
		} else if( g_bDoubleByteCharacterSet && IsDBCSLeadByte( pMsg->wParam ) ) {
			cLeadByte = pMsg->wParam;
		} else if( pMsg->wParam == 0x0D ) {
			OnEditReturn();
		} else if( pMsg->wParam == 0x1B ) {
			OnEditEscape();
		} else {
			OnCharKeyDown( pMsg->wParam );
		}

		break;

	case WM_IME_CHAR:
		if( m_bComposition ) { // CHAR during IME composition
			TRACE0("END4\n"); m_bComposition = FALSE;
			OnImeCompositionEnd(FALSE);
		}

		szByte[0] = (TCHAR) pMsg->wParam & 0xFF; szByte[1] = (TCHAR) pMsg->wParam >> 8; szByte[2] = '\0';
		TRACE1("DBCHAR: [%s]\n", szByte); OnDBCharKeyDown( szByte[0], szByte[1] );

		return TRUE;

	case WM_IME_STARTCOMPOSITION:
		TRACE0("BEG1 "); m_bComposition = TRUE;
		OnImeCompositionStart();
		return TRUE;

	case WM_IME_ENDCOMPOSITION:
		TRACE0("END1\n"); m_bComposition = FALSE;
		OnImeCompositionEnd(TRUE);
		return TRUE;

	case WM_IME_COMPOSITION:
		if( ! m_bComposition && pMsg->lParam & GCS_COMPSTR ) {
			TRACE0("BEG2 "); m_bComposition = TRUE;
			OnImeCompositionStart();
		} else if( m_bComposition && pMsg->lParam & GCS_RESULTSTR ) {
			TRACE0("END2\n"); m_bComposition = FALSE;
			OnImeCompositionEnd(FALSE);
		}

		if( pMsg->lParam & GCS_RESULTSTR ) {
			INT nSize = GetCompositionString( GCS_RESULTSTR, szCompositionString );
			TRACE1("RESULT: [%s]\n", szCompositionString); OnImeCompositionResult( szCompositionString );
		} else if( pMsg->lParam & GCS_COMPSTR ) {
			INT nSize = GetCompositionString( GCS_COMPSTR, szCompositionString );
			TRACE1("(%s) ", szCompositionString); OnImeCompositionCompose( szCompositionString );
		}

		return TRUE;
	}

	return CView::PreTranslateMessage(pMsg);
}

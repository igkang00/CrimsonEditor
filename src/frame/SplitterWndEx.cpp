// SplitterWndEx.cpp : implementation file
//

#include "stdafx.h"
#include "cedtHeader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplitterWndEx

CSplitterWndEx::CSplitterWndEx()
{
}

CSplitterWndEx::~CSplitterWndEx()
{
}


BEGIN_MESSAGE_MAP(CSplitterWndEx, CSplitterWnd)
	//{{AFX_MSG_MAP(CSplitterWndEx)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSplitterWndEx message handlers

BOOL CSplitterWndEx::SplitColumn(int cxBefore)
{
	for( INT i = 0; i < GetRowCount(); i++ ) {
		CCedtView * pView = (CCedtView *)GetPane(i, 0);
		if( pView->IsWordWrapOn() ) {
			AfxMessageBox(IDS_ERR_SPLIT_WINDOW_COLUMN, MB_OK);
			return FALSE;
		}
	}

	return CSplitterWnd::SplitColumn(cxBefore);
}

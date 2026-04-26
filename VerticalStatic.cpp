// VerticalStatic.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "VerticalStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVerticalStatic

CVerticalStatic::CVerticalStatic()
{
}

CVerticalStatic::~CVerticalStatic()
{
}


BEGIN_MESSAGE_MAP(CVerticalStatic, CStatic)
	//{{AFX_MSG_MAP(CVerticalStatic)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVerticalStatic message handlers

void CVerticalStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	RECT rect; GetClientRect( & rect );
	CString szText; GetWindowText(szText);

	LOGFONT lf; CFont * pFont = GetFont();
	pFont->GetLogFont( & lf ); lf.lfEscapement = 900;
	CFont font; font.CreateFontIndirect( & lf );

	dc.IntersectClipRect( & rect );
	CFont * pFontSave = dc.SelectObject( & font );
	dc.SetBkMode( TRANSPARENT );
	CSize size = dc.GetTextExtent( szText );
	dc.FillSolidRect( & rect, GetSysColor(COLOR_BTNFACE) );
	dc.SetTextColor( GetSysColor(COLOR_BTNTEXT) );
	dc.TextOut( rect.left + 4, rect.bottom - 2, szText );
	dc.SelectObject(pFontSave);
}

void CVerticalStatic::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
	Invalidate(TRUE);
}

BOOL CVerticalStatic::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

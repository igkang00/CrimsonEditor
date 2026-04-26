#include "stdafx.h"
#include "cedtHeader.h"


CView * CCedtDoc::GetFirstView()
{
	POSITION pos = GetFirstViewPosition();
	if( pos ) return GetNextView( pos );
	else return NULL;
}

INT CCedtDoc::GetViewCount()
{
	INT nCount = 0; POSITION pos = GetFirstViewPosition();
	while( pos ) { GetNextView( pos ); nCount++; }
	return nCount;
}

void CCedtDoc::ReinitializeAllViews()
{
	POSITION pos = GetFirstViewPosition();
	while( pos ) {
		CCedtView * pView = (CCedtView *)GetNextView( pos );
		pView->Reinitialize();
	}
}

void CCedtDoc::UpdateMDIFileTab()
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	POSITION pos = GetFirstViewPosition();
	while( pos ) {
		CCedtView * pView = (CCedtView *)GetNextView( pos );
		CChildFrame * pChild = (CChildFrame *)pView->GetParentFrame();
		pMainFrame->UpdateMDIFileTab( pChild );
	}
}

void CCedtDoc::FormatScreenText()
{
	POSITION pos = GetFirstViewPosition();
	while( pos ) {
		CCedtView * pView = (CCedtView *)GetNextView( pos );
		pView->FormatScreenText();
	}
}

void CCedtDoc::FormatScreenText(INT nIndex, INT nCount)
{
	POSITION pos = GetFirstViewPosition();
	while( pos ) {
		CCedtView * pView = (CCedtView *)GetNextView( pos );
		pView->FormatScreenText(nIndex, nCount);
	}
}

void CCedtDoc::RemoveScreenText(INT nIndex, INT nCount)
{
	POSITION pos = GetFirstViewPosition();
	while( pos ) {
		CCedtView * pView = (CCedtView *)GetNextView( pos );
		pView->RemoveScreenText(nIndex, nCount);
	}
}

void CCedtDoc::InsertScreenText(INT nIndex, INT nCount)
{
	POSITION pos = GetFirstViewPosition();
	while( pos ) {
		CCedtView * pView = (CCedtView *)GetNextView( pos );
		pView->InsertScreenText(nIndex, nCount);
	}
}

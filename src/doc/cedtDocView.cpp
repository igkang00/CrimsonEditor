#include "stdafx.h"
#include "cedtHeader.h"


// Not every view attached to this document is one of ours. MFC's print preview builds a
// CPreviewView with a CCreateContext naming this document, and CView::OnCreate adds it to
// the document's view list -- so for as long as preview is up, the list holds a view that
// is not a CCedtView. Casting it to one and touching a CCedtView member reads far past the
// end of that object: garbage that is then used as a GDI handle.
//
// So the list is walked through here and nowhere else, and the ones that are not ours are
// skipped. Returns NULL when there are no more.
CCedtView * CCedtDoc::GetNextCedtView(POSITION & pos)
{
	while( pos ) {
		CCedtView * pView = DYNAMIC_DOWNCAST(CCedtView, GetNextView( pos ));
		if( pView ) return pView;
	}
	return NULL;
}

CCedtView * CCedtDoc::GetFirstView()
{
	POSITION pos = GetFirstViewPosition();
	return GetNextCedtView( pos );
}

INT CCedtDoc::GetViewCount()
{
	INT nCount = 0; POSITION pos = GetFirstViewPosition();
	while( GetNextCedtView( pos ) ) nCount++;
	return nCount;
}

void CCedtDoc::ReinitializeAllViews()
{
	POSITION pos = GetFirstViewPosition(); CCedtView * pView;
	while( (pView = GetNextCedtView( pos )) != NULL ) {
		pView->Reinitialize();
	}
}

void CCedtDoc::UpdateMDIFileTab()
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	POSITION pos = GetFirstViewPosition(); CCedtView * pView;
	while( (pView = GetNextCedtView( pos )) != NULL ) {
		CChildFrame * pChild = (CChildFrame *)pView->GetParentFrame();
		pMainFrame->UpdateMDIFileTab( pChild );
	}
}

void CCedtDoc::FormatScreenText()
{
	POSITION pos = GetFirstViewPosition(); CCedtView * pView;
	while( (pView = GetNextCedtView( pos )) != NULL ) {
		pView->FormatScreenText();
	}
}

void CCedtDoc::FormatScreenText(INT nIndex, INT nCount)
{
	POSITION pos = GetFirstViewPosition(); CCedtView * pView;
	while( (pView = GetNextCedtView( pos )) != NULL ) {
		pView->FormatScreenText(nIndex, nCount);
	}
}

void CCedtDoc::RemoveScreenText(INT nIndex, INT nCount)
{
	POSITION pos = GetFirstViewPosition(); CCedtView * pView;
	while( (pView = GetNextCedtView( pos )) != NULL ) {
		pView->RemoveScreenText(nIndex, nCount);
	}
}

void CCedtDoc::InsertScreenText(INT nIndex, INT nCount)
{
	POSITION pos = GetFirstViewPosition(); CCedtView * pView;
	while( (pView = GetNextCedtView( pos )) != NULL ) {
		pView->InsertScreenText(nIndex, nCount);
	}
}

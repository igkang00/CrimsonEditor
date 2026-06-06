#include "stdafx.h"
#include "cedtHeader.h"



void CCedtApp::ApplyPreferencesToAllViews()
{
	INT nFont = CCedtView::m_nCurrentScreenFont;
	if( CCedtView::m_bColumnMode ) { // check if it need to use column mode font
		if( CCedtView::IsFixedPitchScreenFont(nFont) ) CCedtView::m_bUsingColumnModeFont = FALSE;
		else CCedtView::m_bUsingColumnModeFont = TRUE;
	} else CCedtView::m_bUsingColumnModeFont = FALSE;

	// apply preferences to all views
	SaveCaretAndAnchorPosAllViews();

	CCedtView::CreateScreenFontObject();
	CCedtView::ApplyCurrentScreenFont();
	FormatScreenTextAllViews();

	RestoreCaretAndAnchorPosAllViews();
	UpdateAllViews();
}

void CCedtApp::UpdateAllViews()
{
	POSITION posDoc = GetFirstDocPosition();
	while( posDoc ) {
		CCedtDoc * pDoc = (CCedtDoc *)GetNextDoc( posDoc );
		POSITION posView = pDoc->GetFirstViewPosition();
		while( posView ) {
			CCedtView * pView = (CCedtView *)pDoc->GetNextView( posView );
			pView->Invalidate();
		}
	}
}

void CCedtApp::SaveCaretAndAnchorPosAllViews()
{
	POSITION posDoc = GetFirstDocPosition();
	while( posDoc ) {
		CCedtDoc * pDoc = (CCedtDoc *)GetNextDoc( posDoc );
		POSITION posView = pDoc->GetFirstViewPosition();
		while( posView ) {
			CCedtView * pView = (CCedtView *)pDoc->GetNextView( posView );
			pView->SaveCaretAndAnchorPos();
		}
	}
}

void CCedtApp::RestoreCaretAndAnchorPosAllViews()
{
	POSITION posDoc = GetFirstDocPosition();
	while( posDoc ) {
		CCedtDoc * pDoc = (CCedtDoc *)GetNextDoc( posDoc );
		POSITION posView = pDoc->GetFirstViewPosition();
		while( posView ) {
			CCedtView * pView = (CCedtView *)pDoc->GetNextView( posView );
			pView->RestoreCaretAndAnchorPos();
		}
	}
}

void CCedtApp::FormatScreenTextAllViews()
{
	POSITION posDoc = GetFirstDocPosition();
	while( posDoc ) {
		CCedtDoc * pDoc = (CCedtDoc *)GetNextDoc( posDoc );
		POSITION posView = pDoc->GetFirstViewPosition();
		while( posView ) {
			CCedtView * pView = (CCedtView *)pDoc->GetNextView( posView );
			pView->FormatScreenText();
		}
	}
}

void CCedtApp::TurnOffWordWrapModeAllViews()
{
	POSITION posDoc = GetFirstDocPosition();
	while( posDoc ) {
		CCedtDoc * pDoc = (CCedtDoc *)GetNextDoc( posDoc );
		POSITION posView = pDoc->GetFirstViewPosition();
		while( posView ) {
			CCedtView * pView = (CCedtView *)pDoc->GetNextView( posView );
			if( pView->IsWordWrapOn() ) pView->TurnOffWordWrapMode();
		}
	}
}


void CCedtApp::OnEditColumnMode() 
{
	CCedtView::m_bColumnMode = ! CCedtView::m_bColumnMode;

	// turn off word wrap mode of all views
	if( CCedtView::m_bColumnMode ) TurnOffWordWrapModeAllViews();

	BOOL bNeedFormat = FALSE;
	if( CCedtView::m_bColumnMode ) { // check if it need to use column mode font
		if( CCedtView::IsUsingFixedPitchFont() ) CCedtView::m_bUsingColumnModeFont = FALSE;
		else { bNeedFormat = TRUE; CCedtView::m_bUsingColumnModeFont = TRUE; }
	} else {
		if( CCedtView::m_bUsingColumnModeFont ) bNeedFormat = TRUE;
		CCedtView::m_bUsingColumnModeFont = FALSE;
	}

	// create screen font object and apply to all views
	SaveCaretAndAnchorPosAllViews();

	CCedtView::CreateScreenFontObject();
	CCedtView::ApplyCurrentScreenFont();
	if( bNeedFormat ) FormatScreenTextAllViews();

	RestoreCaretAndAnchorPosAllViews();
	UpdateAllViews();

	// set indicators of main frame window
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	pMainFrame->SetColumnModeFlag(CCedtView::m_bColumnMode);

	// we need to update caret position info of main frame window
	CCedtView * pView = (CCedtView *)pMainFrame->MDIGetActiveView();
	if( pView ) pView->UpdateCaretPosition();
}

void CCedtApp::OnUpdateEditColumnMode(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(CCedtView::m_bColumnMode); 
}

void CCedtApp::OnIndicatorOvr() 
{
	CCedtView::m_bOverwriteMode = ! CCedtView::m_bOverwriteMode;

	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	pMainFrame->SetOverwriteFlag(CCedtView::m_bOverwriteMode);

	CCedtView * pView = (CCedtView *)pMainFrame->MDIGetActiveView();
	if( pView ) pView->UpdateCaretPosition();
}


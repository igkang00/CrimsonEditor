#include "stdafx.h"
#include "cedtHeader.h"
#include "PrefDialog.h"


void CPreferenceDialog::InitGeneralPage()
{
}

void CPreferenceDialog::SizeGeneralPage()
{
	INT nPosY;

	nPosY  = 26; m_stcTabSizeBox.MoveWindow(170, nPosY, 360, 55);
	nPosY += 10; m_stcTabSize.MoveWindow(180, nPosY, 240, 14);			m_edtTabSize.MoveWindow(430, nPosY-3, 70, 18);
	nPosY += 20; m_chkUseSpacesInPlaceOfTab.MoveWindow(180, nPosY, 320, 14);

	nPosY += 35; m_stcIndentationSizeBox.MoveWindow(170, nPosY, 360, 55);
	nPosY += 10; m_stcIndentationSize.MoveWindow(180, nPosY, 240, 14);	m_edtIndentationSize.MoveWindow(430, nPosY-3, 70, 18);
	nPosY += 20; m_chkUseTabAsIndentation.MoveWindow(180, nPosY, 320, 14);

	nPosY += 35; m_stcWordWrapBox.MoveWindow(170, nPosY, 360, 75);
	nPosY += 10; m_stcWrapIndentation.MoveWindow(180, nPosY, 240, 14);	m_edtWrapIndentation.MoveWindow(430, nPosY-3, 70, 18);
	nPosY += 20; m_chkFixedWrapWidth.MoveWindow(180, nPosY, 240, 14);	m_edtFixedWrapWidth.MoveWindow(430, nPosY-3, 70, 18);
	nPosY += 20; m_chkOpenDocumentWordWrapped.MoveWindow(180, nPosY, 320, 14);

	nPosY += 35; m_stcGeneralOptionsBox.MoveWindow(170, nPosY, 360, 115);
	nPosY += 10; m_chkEnableAutoIndent.MoveWindow(180, nPosY, 320, 14);
	nPosY += 20; m_chkHomeKeyToFirstPosition.MoveWindow(180, nPosY, 320, 14);
	nPosY += 20; m_chkSearchWrapAtEndOfFile.MoveWindow(180, nPosY, 320, 14);
	nPosY += 20; m_chkDragAndDropEditing.MoveWindow(180, nPosY, 320, 14);
	nPosY += 20; m_chkCopyLineNothingSelected.MoveWindow(180, nPosY, 320, 14);
}

void CPreferenceDialog::ShowGeneralPage()
{
	INT nCmdShow = (m_nActiveCategory == PREF_CATEGORY_GENERAL) ? SW_SHOW : SW_HIDE;

	m_stcTabSizeBox.ShowWindow(nCmdShow);
	m_stcTabSize.ShowWindow(nCmdShow);				m_edtTabSize.ShowWindow(nCmdShow);
	m_chkUseSpacesInPlaceOfTab.ShowWindow(nCmdShow);

	m_stcIndentationSizeBox.ShowWindow(nCmdShow);
	m_stcIndentationSize.ShowWindow(nCmdShow);		m_edtIndentationSize.ShowWindow(nCmdShow);
	m_chkUseTabAsIndentation.ShowWindow(nCmdShow);

	m_stcWordWrapBox.ShowWindow(nCmdShow);
	m_stcWrapIndentation.ShowWindow(nCmdShow);		m_edtWrapIndentation.ShowWindow(nCmdShow);
	m_chkFixedWrapWidth.ShowWindow(nCmdShow);		m_edtFixedWrapWidth.ShowWindow(nCmdShow);
	m_chkOpenDocumentWordWrapped.ShowWindow(nCmdShow);

	m_stcGeneralOptionsBox.ShowWindow(nCmdShow);
	m_chkEnableAutoIndent.ShowWindow(nCmdShow);
	m_chkHomeKeyToFirstPosition.ShowWindow(nCmdShow);
	m_chkSearchWrapAtEndOfFile.ShowWindow(nCmdShow);
	m_chkDragAndDropEditing.ShowWindow(nCmdShow);
	m_chkCopyLineNothingSelected.ShowWindow(nCmdShow);
}

BOOL CPreferenceDialog::LoadGeneralSettings()
{
	CString str;

	str.Format("%d", CCedtView::m_nTabSize);
	m_edtTabSize.SetWindowText(str);
	m_chkUseSpacesInPlaceOfTab.SetCheck(CCedtView::m_bUseSpacesInPlaceOfTab ? 1 : 0);

	if( CCedtView::m_nIndentationSize ) {
		str.Format("%d", CCedtView::m_nIndentationSize);
		m_edtIndentationSize.SetWindowText(str);
		m_chkUseTabAsIndentation.SetCheck(FALSE);
	} else {
		m_edtIndentationSize.SetWindowText(str); // CCedtView::m_nTabSize
		m_edtIndentationSize.EnableWindow(FALSE);
		m_chkUseTabAsIndentation.SetCheck(TRUE);
	}

	str.Format("%d", CCedtView::m_nWrapIndentation);
	m_edtWrapIndentation.SetWindowText(str);
	m_chkFixedWrapWidth.SetCheck(CCedtView::m_nFixedWrapWidth ? 1 : 0);
	if( CCedtView::m_nFixedWrapWidth ) {
		str.Format("%d", CCedtView::m_nFixedWrapWidth);
		m_edtFixedWrapWidth.SetWindowText(str);
	} else {
		m_edtFixedWrapWidth.SetWindowText("80");
		m_edtFixedWrapWidth.EnableWindow(FALSE);
	}
	m_chkOpenDocumentWordWrapped.SetCheck(CCedtView::m_bOpenDocumentWordWrapped ? 1 : 0);

	m_chkEnableAutoIndent.SetCheck(CCedtView::m_bEnableAutoIndent ? 1 : 0);

	m_chkHomeKeyToFirstPosition.SetCheck(CCedtView::m_bHomeKeyGoesToFirstPosition ? 1 : 0);
	m_chkSearchWrapAtEndOfFile.SetCheck(CCedtView::m_bSearchWrapAtEndOfFile ? 1 : 0);
	m_chkDragAndDropEditing.SetCheck(CCedtView::m_bEnableDragAndDropEditing ? 1 : 0);

	m_chkCopyLineNothingSelected.SetCheck(CCedtView::m_bCopyCurrentLineIfNothingSelected ? 1 : 0);

	return TRUE;
}

BOOL CPreferenceDialog::SaveGeneralSettings()
{
	TCHAR buf[1024];

	m_edtTabSize.GetWindowText(buf, 1024); CCedtView::m_nTabSize = atoi(buf);
	ASSURE_BOUND_VALUE(CCedtView::m_nTabSize, 1, 64);
	CCedtView::m_bUseSpacesInPlaceOfTab = m_chkUseSpacesInPlaceOfTab.GetCheck();

	if( ! m_chkUseTabAsIndentation.GetCheck() ) {
		m_edtIndentationSize.GetWindowText(buf, 1024); CCedtView::m_nIndentationSize = atoi(buf);
		ASSURE_BOUND_VALUE(CCedtView::m_nIndentationSize, 1, _MY_MIN(8, CCedtView::m_nTabSize));
	} else CCedtView::m_nIndentationSize = 0;

	m_edtWrapIndentation.GetWindowText(buf, 1024); CCedtView::m_nWrapIndentation = atoi(buf);
	ASSURE_BOUND_VALUE(CCedtView::m_nWrapIndentation, 0, 16);
	if( m_chkFixedWrapWidth.GetCheck() ) {
		m_edtFixedWrapWidth.GetWindowText(buf, 1024); CCedtView::m_nFixedWrapWidth = atoi(buf);
		ASSURE_BOUND_VALUE(CCedtView::m_nFixedWrapWidth, 16, 160);
	} else CCedtView::m_nFixedWrapWidth = 0;
	CCedtView::m_bOpenDocumentWordWrapped = m_chkOpenDocumentWordWrapped.GetCheck();

	CCedtView::m_bEnableAutoIndent = m_chkEnableAutoIndent.GetCheck();

	CCedtView::m_bHomeKeyGoesToFirstPosition = m_chkHomeKeyToFirstPosition.GetCheck();
	CCedtView::m_bSearchWrapAtEndOfFile = m_chkSearchWrapAtEndOfFile.GetCheck();
	CCedtView::m_bEnableDragAndDropEditing = m_chkDragAndDropEditing.GetCheck();

	CCedtView::m_bCopyCurrentLineIfNothingSelected = m_chkCopyLineNothingSelected.GetCheck();

	return TRUE;
}

void CPreferenceDialog::OnUseTabAsIndentation() 
{
	m_edtIndentationSize.EnableWindow( ! m_chkUseTabAsIndentation.GetCheck() );
}

void CPreferenceDialog::OnFixedWrapWidthCheck() 
{
	m_edtFixedWrapWidth.EnableWindow( m_chkFixedWrapWidth.GetCheck() );
}

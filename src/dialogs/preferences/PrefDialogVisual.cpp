#include "stdafx.h"
#include "cedtHeader.h"
#include "PrefDialog.h"



void CPreferenceDialog::InitVisualPage()
{
}

void CPreferenceDialog::SizeVisualPage()
{
	INT nPosY;

	nPosY  = 26; m_stcLineSpacingBox.MoveWindow(170, nPosY, 360, 75);
	nPosY += 10; m_stcLineSpacing.MoveWindow(180, nPosY, 240, 14);			m_edtLineSpacing.MoveWindow(430, nPosY-3, 70, 18);
	nPosY += 20; m_stcCaretWidth.MoveWindow(180, nPosY, 240, 14);			m_edtCaretWidth.MoveWindow(430, nPosY-3, 70, 18);
	nPosY += 20; m_stcCaretWidthDesc.MoveWindow(180, nPosY, 320, 14);

	nPosY += 35; m_stcLineNumberBox.MoveWindow(170, nPosY, 360, 55);
	nPosY += 10; m_stcLineNumberDigits.MoveWindow(180, nPosY, 240, 14);		m_edtLineNumberDigits.MoveWindow(430, nPosY-3, 70, 18);
	nPosY += 20; m_stcFileTabLength.MoveWindow(180, nPosY, 240, 14);		m_edtFileTabLength.MoveWindow(430, nPosY-3, 70, 18);

	nPosY += 35; m_stcColumnMarkersBox.MoveWindow(170, nPosY, 360, 55);
	nPosY += 10; m_chkColumnMarker1.MoveWindow(180, nPosY, 240, 14);		m_edtColumnMarker1.MoveWindow(430, nPosY-3, 70, 18);
	nPosY += 20; m_chkColumnMarker2.MoveWindow(180, nPosY, 240, 14);		m_edtColumnMarker2.MoveWindow(430, nPosY-3, 70, 18);

	nPosY += 35; m_stcVisualOptionsBox.MoveWindow(170, nPosY, 360, 100);
	nPosY += 10; m_chkShowLineNumbers.MoveWindow(180, nPosY, 150, 14);		m_chkShowSelectionMargin.MoveWindow(340, nPosY, 160, 14);
	nPosY += 20; m_chkHighlightActiveLine.MoveWindow(180, nPosY, 150, 14);	m_chkEnablePairsMatching.MoveWindow(340, nPosY, 160, 14);

	nPosY += 25; m_chkEmboldenKeywords.MoveWindow(180, nPosY, 150, 14);		m_chkItalicizeComment.MoveWindow(340, nPosY, 160, 14);
	nPosY += 20; m_chkShowTabChars.MoveWindow(180, nPosY, 150, 14);			m_chkShowLineBreak.MoveWindow(340, nPosY, 160, 14);
}

void CPreferenceDialog::ShowVisualPage()
{
	INT nCmdShow = (m_nActiveCategory == PREF_CATEGORY_VISUAL) ? SW_SHOW : SW_HIDE;

	m_stcLineSpacingBox.ShowWindow(nCmdShow);
	m_stcLineSpacing.ShowWindow(nCmdShow);			m_edtLineSpacing.ShowWindow(nCmdShow);
	m_stcCaretWidth.ShowWindow(nCmdShow);			m_edtCaretWidth.ShowWindow(nCmdShow);
	m_stcCaretWidthDesc.ShowWindow(nCmdShow);

	m_stcLineNumberBox.ShowWindow(nCmdShow);
	m_stcLineNumberDigits.ShowWindow(nCmdShow);		m_edtLineNumberDigits.ShowWindow(nCmdShow);
	m_stcFileTabLength.ShowWindow(nCmdShow);		m_edtFileTabLength.ShowWindow(nCmdShow);

	m_stcColumnMarkersBox.ShowWindow(nCmdShow);
	m_chkColumnMarker1.ShowWindow(nCmdShow);		m_edtColumnMarker1.ShowWindow(nCmdShow);
	m_chkColumnMarker2.ShowWindow(nCmdShow);		m_edtColumnMarker2.ShowWindow(nCmdShow);

	m_stcVisualOptionsBox.ShowWindow(nCmdShow);
	m_chkShowLineNumbers.ShowWindow(nCmdShow);		m_chkShowSelectionMargin.ShowWindow(nCmdShow);
	m_chkHighlightActiveLine.ShowWindow(nCmdShow);	m_chkEnablePairsMatching.ShowWindow(nCmdShow);

	m_chkEmboldenKeywords.ShowWindow(nCmdShow);		m_chkItalicizeComment.ShowWindow(nCmdShow);
	m_chkShowTabChars.ShowWindow(nCmdShow);			m_chkShowLineBreak.ShowWindow(nCmdShow);
}


BOOL CPreferenceDialog::LoadVisualSettings()
{
	CString str; 
	
	str.Format("%d", CCedtView::m_nLineSpacing);
	m_edtLineSpacing.SetWindowText(str);
	str.Format("%d", CCedtView::m_nCaretWidthRatio);
	m_edtCaretWidth.SetWindowText(str);

	str.Format("%d", CCedtView::m_nLineNumberDigits);
	m_edtLineNumberDigits.SetWindowText(str);
	str.Format("%d", CCedtView::m_nFileTabLength);
	m_edtFileTabLength.SetWindowText(str);

	m_chkColumnMarker1.SetCheck( CCedtView::m_bShowColumnMarker1 ? 1 : 0 );
	m_edtColumnMarker1.EnableWindow( CCedtView::m_bShowColumnMarker1 );
	str.Format("%d", CCedtView::m_nColumnMarker1Pos); 
	m_edtColumnMarker1.SetWindowText( str );

	m_chkColumnMarker2.SetCheck( CCedtView::m_bShowColumnMarker2 ? 1 : 0 );
	m_edtColumnMarker2.EnableWindow( CCedtView::m_bShowColumnMarker2 );
	str.Format("%d", CCedtView::m_nColumnMarker2Pos); 
	m_edtColumnMarker2.SetWindowText( str );

	m_chkShowLineNumbers.SetCheck(CCedtView::m_bShowLineNumbers ? 1 : 0);
	m_chkShowSelectionMargin.SetCheck(CCedtView::m_bShowSelectionMargin ? 1 : 0);
	m_chkHighlightActiveLine.SetCheck(CCedtView::m_bHighlightActiveLine ? 1 : 0);
	m_chkEnablePairsMatching.SetCheck(CCedtView::m_bEnablePairsMatching ? 1 : 0);
	m_chkShowSelectionMargin.EnableWindow( ! CCedtView::m_bShowLineNumbers );

	m_chkEmboldenKeywords.SetCheck(CCedtView::m_bEmboldenKeywords ? 1 : 0);
	m_chkItalicizeComment.SetCheck(CCedtView::m_bItalicizeComment ? 1 : 0);
	m_chkShowTabChars.SetCheck(CCedtView::m_bShowTabChars ? 1 : 0);
	m_chkShowLineBreak.SetCheck(CCedtView::m_bShowLineBreak ? 1 : 0);

	return TRUE;
}


BOOL CPreferenceDialog::SaveVisualSettings()
{
	TCHAR buf[1024];

	m_edtLineSpacing.GetWindowText(buf, 1024); CCedtView::m_nLineSpacing = atoi(buf);
	ASSURE_BOUND_VALUE(CCedtView::m_nLineSpacing, 50, 200);
	m_edtCaretWidth.GetWindowText(buf, 1024); CCedtView::m_nCaretWidthRatio = atoi(buf);
	ASSURE_BOUND_VALUE(CCedtView::m_nCaretWidthRatio, 1, 100);

	m_edtLineNumberDigits.GetWindowText(buf, 1024); CCedtView::m_nLineNumberDigits = atoi(buf);
	ASSURE_BOUND_VALUE(CCedtView::m_nLineNumberDigits, 2, 8);
	m_edtFileTabLength.GetWindowText(buf, 1024); CCedtView::m_nFileTabLength = atoi(buf);
	ASSURE_BOUND_VALUE(CCedtView::m_nFileTabLength, 12, 60);

	CCedtView::m_bShowColumnMarker1 = m_chkColumnMarker1.GetCheck();
	m_edtColumnMarker1.GetWindowText(buf, 1024); CCedtView::m_nColumnMarker1Pos = atoi(buf);
	ASSURE_BOUND_VALUE(CCedtView::m_nColumnMarker1Pos, 0, 160);

	CCedtView::m_bShowColumnMarker2 = m_chkColumnMarker2.GetCheck();
	m_edtColumnMarker2.GetWindowText(buf, 1024); CCedtView::m_nColumnMarker2Pos = atoi(buf);
	ASSURE_BOUND_VALUE(CCedtView::m_nColumnMarker2Pos, 0, 160);

	CCedtView::m_bShowLineNumbers = m_chkShowLineNumbers.GetCheck();
	CCedtView::m_bShowSelectionMargin = m_chkShowSelectionMargin.GetCheck();
	CCedtView::m_bHighlightActiveLine = m_chkHighlightActiveLine.GetCheck();
	CCedtView::m_bEnablePairsMatching = m_chkEnablePairsMatching.GetCheck();

	CCedtView::m_bEmboldenKeywords = m_chkEmboldenKeywords.GetCheck();
	CCedtView::m_bItalicizeComment = m_chkItalicizeComment.GetCheck();
	CCedtView::m_bShowTabChars = m_chkShowTabChars.GetCheck();
	CCedtView::m_bShowLineBreak = m_chkShowLineBreak.GetCheck();

	return TRUE;
}

void CPreferenceDialog::OnColumnMarker1Check() 
{
	m_edtColumnMarker1.EnableWindow( m_chkColumnMarker1.GetCheck() );
}

void CPreferenceDialog::OnColumnMarker2Check() 
{
	m_edtColumnMarker2.EnableWindow( m_chkColumnMarker2.GetCheck() );
}

void CPreferenceDialog::OnShowLineNumbers() 
{
	BOOL bShowLineNumbers = m_chkShowLineNumbers.GetCheck();
	m_chkShowSelectionMargin.EnableWindow( ! bShowLineNumbers );
}

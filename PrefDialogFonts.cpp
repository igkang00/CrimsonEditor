#include "stdafx.h"
#include "cedtHeader.h"
#include "PrefDialog.h"


void CPreferenceDialog::InitFontsPage()
{
	CString szText( (LPCTSTR)IDS_PREF_SCREEN_FONTS );
	m_tabFontsPannel.InsertItem( FONTS_PANNEL_SCREEN,  szText, 0 );
	szText.LoadString( IDS_PREF_PRINTER_FONTS );
	m_tabFontsPannel.InsertItem( FONTS_PANNEL_PRINTER, szText, 0 );
	szText.LoadString( IDS_PREF_MISC_FONTS );
	m_tabFontsPannel.InsertItem( FONTS_PANNEL_MISC,    szText, 0 );
	m_tabFontsPannel.SetCurSel( m_nActiveFontsPannel );

	m_lstScreenFonts.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	szText.LoadString( IDS_PREF_FONTS_AREA );
	m_lstScreenFonts.InsertColumn( 0, szText, LVCFMT_LEFT, 100 );
	szText.LoadString( IDS_PREF_FONTS_FACE_NAME );
	m_lstScreenFonts.InsertColumn( 1, szText, LVCFMT_LEFT, 150 );
	szText.LoadString( IDS_PREF_FONTS_SIZE );
	m_lstScreenFonts.InsertColumn( 2, szText, LVCFMT_LEFT, 65  );

	m_lstPrinterFonts.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	szText.LoadString( IDS_PREF_FONTS_AREA );
	m_lstPrinterFonts.InsertColumn( 0, szText, LVCFMT_LEFT, 100 );
	szText.LoadString( IDS_PREF_FONTS_FACE_NAME );
	m_lstPrinterFonts.InsertColumn( 1, szText, LVCFMT_LEFT, 150 );
	szText.LoadString( IDS_PREF_FONTS_SIZE );
	m_lstPrinterFonts.InsertColumn( 2, szText, LVCFMT_LEFT, 65  );

	m_lstMiscelFonts.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	szText.LoadString( IDS_PREF_FONTS_AREA );
	m_lstMiscelFonts.InsertColumn( 0, szText, LVCFMT_LEFT, 100 );
	szText.LoadString( IDS_PREF_FONTS_FACE_NAME );
	m_lstMiscelFonts.InsertColumn( 1, szText, LVCFMT_LEFT, 150 );
	szText.LoadString( IDS_PREF_FONTS_SIZE );
	m_lstMiscelFonts.InsertColumn( 2, szText, LVCFMT_LEFT, 65  );
}

void CPreferenceDialog::SizeFontsPage()
{
	INT nPosY;

	nPosY  =  26; m_tabFontsPannel.MoveWindow(170, nPosY, 360, 200);

	nPosY  =  66; m_lstScreenFonts.MoveWindow(180, nPosY, 320, 120);
	nPosY += 130; m_stcScreenFonts.MoveWindow(185, nPosY, 300, 14);

	nPosY  =  66; m_lstPrinterFonts.MoveWindow(180, nPosY, 320, 120);
	nPosY += 130; m_stcPrinterFonts.MoveWindow(185, nPosY, 300, 14);

	nPosY  =  66; m_lstMiscelFonts.MoveWindow(180, nPosY, 320, 120);
	nPosY += 130; m_stcMiscelFonts.MoveWindow(185, nPosY, 300, 14);
}

void CPreferenceDialog::ShowFontsPage()
{
	INT nCmdShow = (m_nActiveCategory == PREF_CATEGORY_FONTS) ? SW_SHOW : SW_HIDE;
	m_tabFontsPannel.ShowWindow(nCmdShow);

	nCmdShow = (m_nActiveCategory == PREF_CATEGORY_FONTS && m_nActiveFontsPannel == 0) ? SW_SHOW : SW_HIDE;
	m_stcScreenFonts.ShowWindow(nCmdShow);		m_lstScreenFonts.ShowWindow(nCmdShow);	

	nCmdShow = (m_nActiveCategory == PREF_CATEGORY_FONTS && m_nActiveFontsPannel == 1) ? SW_SHOW : SW_HIDE;
	m_stcPrinterFonts.ShowWindow(nCmdShow);		m_lstPrinterFonts.ShowWindow(nCmdShow);

	nCmdShow = (m_nActiveCategory == PREF_CATEGORY_FONTS && m_nActiveFontsPannel == 2) ? SW_SHOW : SW_HIDE;
	m_stcMiscelFonts.ShowWindow(nCmdShow);		m_lstMiscelFonts.ShowWindow(nCmdShow);
}


BOOL CPreferenceDialog::LoadFontSettings()
{
	INT i; CString szText;
	
	memcpy((void *)m_lfScreen, (const void *)CCedtView::m_lfScreen, sizeof(m_lfScreen));
	m_nActiveScreenFont = CCedtView::m_nCurrentScreenFont;

	for(i = 0; i < 6; i++) {
		if( i == 0 ) szText.Format(IDS_PREF_FONTS_AREA0);
		else szText.Format(IDS_PREF_FONTS_AREA1, i);
		m_lstScreenFonts.InsertItem(i, szText);
		DispScreenFontText(i);
	}

	memcpy((void *)m_lfPrinter, (const void *)CCedtView::m_lfPrinter, sizeof(m_lfPrinter));
	m_nActivePrinterFont = CCedtView::m_nCurrentPrinterFont;

	for(i = 0; i < 4; i++) {
		if( i == 0 ) szText.Format(IDS_PREF_FONTS_AREA0);
		else szText.Format(IDS_PREF_FONTS_AREA1, i);
		m_lstPrinterFonts.InsertItem(i, szText);
		DispPrinterFontText(i);
	}

	memcpy((void *)m_lfMiscel, (const void *)CCedtView::m_lfMiscel, sizeof(m_lfMiscel));
//	m_nActiveMiscelFont = 0x00;

	szText.Format(IDS_PREF_FONTS_MISC0);
	m_lstMiscelFonts.InsertItem(0, szText);
	DispMiscelFontText(0);

	szText.Format(IDS_PREF_FONTS_MISC1);
	m_lstMiscelFonts.InsertItem(1, szText);
	DispMiscelFontText(1);

	m_lstScreenFonts.SetItemState(m_nActiveScreenFont, LVIS_SELECTED, LVIS_SELECTED);
	m_lstPrinterFonts.SetItemState(m_nActivePrinterFont, LVIS_SELECTED, LVIS_SELECTED);
	m_lstMiscelFonts.SetItemState(m_nActiveMiscelFont, LVIS_SELECTED, LVIS_SELECTED);

	return TRUE;
}


BOOL CPreferenceDialog::SaveFontSettings()
{
	memcpy((void *)CCedtView::m_lfScreen, (const void *)m_lfScreen, sizeof(m_lfScreen));
	CCedtView::m_nCurrentScreenFont = m_nActiveScreenFont;

	memcpy((void *)CCedtView::m_lfPrinter, (const void *)m_lfPrinter, sizeof(m_lfPrinter));
	CCedtView::m_nCurrentPrinterFont = m_nActivePrinterFont;

	memcpy((void *)CCedtView::m_lfMiscel, (const void *)m_lfMiscel, sizeof(m_lfMiscel));

	return TRUE;
}

void CPreferenceDialog::DispScreenFontText(INT nScreenFont)
{
	m_lstScreenFonts.SetItemText(nScreenFont, 1, m_lfScreen[nScreenFont].lfFaceName);
	CString szSize; szSize.Format("%d", m_lfScreen[nScreenFont].lfHeight / 10);
	m_lstScreenFonts.SetItemText(nScreenFont, 2, szSize);
}

void CPreferenceDialog::DispPrinterFontText(INT nPrinterFont)
{
	m_lstPrinterFonts.SetItemText(nPrinterFont, 1, m_lfPrinter[nPrinterFont].lfFaceName);
	CString szSize; szSize.Format("%d", m_lfPrinter[nPrinterFont].lfHeight / 10);
	m_lstPrinterFonts.SetItemText(nPrinterFont, 2, szSize);
}

void CPreferenceDialog::DispMiscelFontText(INT nMiscelFont)
{
	m_lstMiscelFonts.SetItemText(nMiscelFont, 1, m_lfMiscel[nMiscelFont].lfFaceName);
	CString szSize; szSize.Format("%d", m_lfMiscel[nMiscelFont].lfHeight / 10);
	m_lstMiscelFonts.SetItemText(nMiscelFont, 2, szSize);
}

void CPreferenceDialog::OnSelchangeFontsPannel(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_nActiveFontsPannel = m_tabFontsPannel.GetCurSel();
	ShowAllPrefPages();
	*pResult = 0;
}

void CPreferenceDialog::OnItemchangedScreenFonts(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if( (pNMListView->iItem >= 0) && (pNMListView->uNewState & LVIS_SELECTED) ) 
		m_nActiveScreenFont = pNMListView->iItem;
	*pResult = 0;
}

void CPreferenceDialog::OnDblclkScreenFonts(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if( pNMListView->iItem >= 0 ) {
		m_nActiveScreenFont = pNMListView->iItem;
		OnScreenFontChange();
	}
	*pResult = 0;
}

void CPreferenceDialog::OnScreenFontChange() 
{
	LOGFONT lf; memcpy( & lf, & m_lfScreen[m_nActiveScreenFont], sizeof(LOGFONT) );
	lf.lfHeight = -MulDiv( lf.lfHeight, CCedtView::GetLogPixelsY(), 720);

	CFontDialog dlg( & lf, CF_SCREENFONTS | CF_EFFECTS );
	if( dlg.DoModal() == IDOK ) {
		memcpy( & m_lfScreen[m_nActiveScreenFont], & lf, sizeof(LOGFONT) );
		m_lfScreen[m_nActiveScreenFont].lfHeight = dlg.GetSize();
		DispScreenFontText(m_nActiveScreenFont);
	}
}

void CPreferenceDialog::OnItemchangedPrinterFonts(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if( (pNMListView->iItem >= 0) && (pNMListView->uNewState & LVIS_SELECTED) ) 
		m_nActivePrinterFont = pNMListView->iItem;
	*pResult = 0;
}

void CPreferenceDialog::OnDblclkPrinterFonts(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if( pNMListView->iItem >= 0 ) {
		m_nActivePrinterFont = pNMListView->iItem;
		OnPrinterFontChange();
	}
	*pResult = 0;
}

void CPreferenceDialog::OnPrinterFontChange() 
{
	LOGFONT lf; memcpy( & lf, & m_lfPrinter[m_nActivePrinterFont], sizeof(LOGFONT) );
	lf.lfHeight = -MulDiv(lf.lfHeight, CCedtView::GetLogPixelsY(), 720);

	CFontDialog dlg( & lf, CF_BOTH | CF_EFFECTS );
	if( dlg.DoModal() == IDOK ) {
		memcpy( & m_lfPrinter[m_nActivePrinterFont], & lf, sizeof(LOGFONT) );
		m_lfPrinter[m_nActivePrinterFont].lfHeight = dlg.GetSize();
		DispPrinterFontText(m_nActivePrinterFont);
	}
}

void CPreferenceDialog::OnItemchangedMiscelFonts(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if( (pNMListView->iItem >= 0) && (pNMListView->uNewState & LVIS_SELECTED) ) 
		m_nActiveMiscelFont = pNMListView->iItem;
	*pResult = 0;
}

void CPreferenceDialog::OnDblclkMiscelFonts(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if( pNMListView->iItem >= 0 ) {
		m_nActiveMiscelFont = pNMListView->iItem;
		OnMiscelFontChange();
	}
	*pResult = 0;
}

void CPreferenceDialog::OnMiscelFontChange() 
{
	LOGFONT lf; memcpy( & lf, & m_lfMiscel[m_nActiveMiscelFont], sizeof(LOGFONT) );
	lf.lfHeight = -MulDiv(lf.lfHeight, CCedtView::GetLogPixelsY(), 720);

	DWORD dwFlags = CF_SCREENFONTS | CF_EFFECTS;
	if( m_nActiveMiscelFont == 0x00 ) dwFlags |= CF_FIXEDPITCHONLY;

	CFontDialog dlg( & lf, dwFlags );
	if( dlg.DoModal() == IDOK ) {
		memcpy( & m_lfMiscel[m_nActiveMiscelFont], & lf, sizeof(LOGFONT) );
		m_lfMiscel[m_nActiveMiscelFont].lfHeight = dlg.GetSize();
		DispMiscelFontText(m_nActiveMiscelFont);
	}
}

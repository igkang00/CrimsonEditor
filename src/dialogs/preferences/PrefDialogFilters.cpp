#include "stdafx.h"
#include "cedtHeader.h"
#include "PrefDialog.h"


void CPreferenceDialog::InitFiltersPage()
{
	m_lstFileFilters.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	CString szText( (LPCTSTR)IDS_PREF_FILTERS_FILE_TYPE );
	m_lstFileFilters.InsertColumn(0, szText, LVCFMT_LEFT, 219);

	m_btnFilterMoveUp.SetIcon( m_lstButtonImage.ExtractIcon(0) );
	m_btnFilterMoveDown.SetIcon( m_lstButtonImage.ExtractIcon(1) );
}

void CPreferenceDialog::SizeFiltersPage()
{
	INT nPosY;

	nPosY  = 26; m_stcFilterSettingsBox.MoveWindow(170, nPosY, 360, 175);
	nPosY += 10; m_lstFileFilters.MoveWindow(180, nPosY, 240, 152);		m_btnFilterRemove.MoveWindow(430, nPosY, 70, 18);
	nPosY += 20; m_btnFilterMoveUp.MoveWindow(430, nPosY, 34, 18);		m_btnFilterMoveDown.MoveWindow(466, nPosY, 34, 18);

	nPosY  = 26 + 150;
	nPosY += 35; m_stcFilterItemBox.MoveWindow(170, nPosY, 360, 75);
	nPosY += 10; m_stcFilterDescription.MoveWindow(180, nPosY, 80, 14);	m_edtFilterDescription.MoveWindow(270, nPosY-3, 230, 18);
	nPosY += 20; m_stcFilterExtensions.MoveWindow(180, nPosY, 80, 14);	m_edtFilterExtensions.MoveWindow(270, nPosY-3, 230, 18);
	nPosY += 20; m_stcFilterDefaultExt.MoveWindow(180, nPosY, 80, 14);	m_edtFilterDefaultExt.MoveWindow(270, nPosY-3, 230, 18);
}

void CPreferenceDialog::ShowFiltersPage()
{
	INT nCmdShow = (m_nActiveCategory == PREF_CATEGORY_FILTERS) ? SW_SHOW : SW_HIDE;

	m_stcFilterSettingsBox.ShowWindow(nCmdShow);
	m_lstFileFilters.ShowWindow(nCmdShow);			m_btnFilterRemove.ShowWindow(nCmdShow);
	m_btnFilterMoveUp.ShowWindow(nCmdShow);			m_btnFilterMoveDown.ShowWindow(nCmdShow);

	m_stcFilterItemBox.ShowWindow(nCmdShow);
	m_stcFilterDescription.ShowWindow(nCmdShow);	m_edtFilterDescription.ShowWindow(nCmdShow);
	m_stcFilterExtensions.ShowWindow(nCmdShow);		m_edtFilterExtensions.ShowWindow(nCmdShow);
	m_stcFilterDefaultExt.ShowWindow(nCmdShow);		m_edtFilterDefaultExt.ShowWindow(nCmdShow);
}


BOOL CPreferenceDialog::LoadFileFilters()
{
	for( INT i = 0; i < MAX_FILE_FILTER; i++ ) {
		m_lstFileFilters.InsertItem(i, ""); // make room for filters
		m_clsFileFilters[i].CopyContents( CCedtApp::m_clsFileFilters[i] );
		DispFileFilterText(i);
	}

	m_lstFileFilters.SetItemState(m_nActiveFileFilter, LVIS_SELECTED, LVIS_SELECTED);
	m_lstFileFilters.EnsureVisible(m_nActiveFileFilter, FALSE);

	return TRUE;
}

BOOL CPreferenceDialog::SaveFileFilters()
{
	for( INT i = 0; i < MAX_FILE_FILTER; i++ ) {
		CString & szDescription = m_clsFileFilters[i].m_szDescription;
		CCedtApp::m_clsFileFilters[i].CopyContents( m_clsFileFilters[i] );
	}

	INT nIndexDialog = CCedtApp::m_nFilterIndexDialog;
	if( ! m_clsFileFilters[nIndexDialog].IsValid() ) CCedtApp::m_nFilterIndexDialog = 0;

	INT nIndexPannel = CCedtApp::m_nFilterIndexPannel;
	if( ! m_clsFileFilters[nIndexPannel].IsValid() ) CCedtApp::m_nFilterIndexPannel = 0;

	return TRUE;
}

BOOL CPreferenceDialog::DispFileFilterText(INT nFilter)
{
	CString szText;
	CString & szDescription = m_clsFileFilters[nFilter].m_szDescription;
	CString & szExtensions  = m_clsFileFilters[nFilter].m_szExtensions;
	if( szDescription.GetLength() ) {
		if( szExtensions.GetLength() ) szText.Format("%s (%s)", szDescription, szExtensions);
		else szText = szDescription;
	} else szText = "- Empty -";
	m_lstFileFilters.SetItemText(nFilter, 0, szText);
	return TRUE;
}

void CPreferenceDialog::OnItemchangedFilters(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if( (pNMListView->iItem >= 0) && (pNMListView->uNewState & LVIS_SELECTED) ) {
		INT nFilter = pNMListView->iItem; m_nActiveFileFilter = nFilter;
		m_edtFilterDescription.SetWindowText( m_clsFileFilters[nFilter].m_szDescription );
		m_edtFilterExtensions.SetWindowText( m_clsFileFilters[nFilter].m_szExtensions );
		m_edtFilterDefaultExt.SetWindowText( m_clsFileFilters[nFilter].m_szDefaultExt );
	}
	*pResult = 0;
}

void CPreferenceDialog::OnFilterRemove() 
{
	INT nFilter = m_nActiveFileFilter;
	m_clsFileFilters[nFilter].DeleteContents();
	DispFileFilterText(nFilter);

	m_edtFilterDescription.SetWindowText( m_clsFileFilters[nFilter].m_szDescription );
	m_edtFilterExtensions.SetWindowText( m_clsFileFilters[nFilter].m_szExtensions );
	m_edtFilterDefaultExt.SetWindowText( m_clsFileFilters[nFilter].m_szDefaultExt );
}

void CPreferenceDialog::OnFilterMoveUp() 
{
	INT nFilter = m_nActiveFileFilter;
	if( nFilter > 0 ) {
		CFileFilter clsFileFilter; clsFileFilter.CopyContents( m_clsFileFilters[nFilter+0] );
		m_clsFileFilters[nFilter+0].CopyContents( m_clsFileFilters[nFilter-1] );
		m_clsFileFilters[nFilter-1].CopyContents( clsFileFilter );

		DispFileFilterText(nFilter-1);
		DispFileFilterText(nFilter+0);

		m_lstFileFilters.SetItemState( nFilter-1, LVIS_SELECTED, LVIS_SELECTED );
		m_lstFileFilters.EnsureVisible( nFilter-1, FALSE );
	}
}

void CPreferenceDialog::OnFilterMoveDown() 
{
	INT nFilter = m_nActiveFileFilter;
	if( nFilter < MAX_FILE_FILTER-1 ) {
		CFileFilter clsFileFilter; clsFileFilter.CopyContents( m_clsFileFilters[nFilter+0] );
		m_clsFileFilters[nFilter+0].CopyContents( m_clsFileFilters[nFilter+1] );
		m_clsFileFilters[nFilter+1].CopyContents( clsFileFilter );

		DispFileFilterText(nFilter+0);
		DispFileFilterText(nFilter+1);

		m_lstFileFilters.SetItemState( nFilter+1, LVIS_SELECTED, LVIS_SELECTED );
		m_lstFileFilters.EnsureVisible( nFilter+1, FALSE );
	}
}

void CPreferenceDialog::OnChangeFilterDescription() 
{
	INT nFilter = m_nActiveFileFilter;
	m_edtFilterDescription.GetWindowText( m_clsFileFilters[nFilter].m_szDescription );
	DispFileFilterText(nFilter);
}

void CPreferenceDialog::OnChangeFilterExtension() 
{
	INT nFilter = m_nActiveFileFilter;
	m_edtFilterExtensions.GetWindowText( m_clsFileFilters[nFilter].m_szExtensions );
	DispFileFilterText(nFilter);
}

void CPreferenceDialog::OnChangeFilterDefaultExt() 
{
	INT nFilter = m_nActiveFileFilter;
	m_edtFilterDefaultExt.GetWindowText( m_clsFileFilters[nFilter].m_szDefaultExt );
	DispFileFilterText(nFilter);
}

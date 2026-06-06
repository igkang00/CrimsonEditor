#include "stdafx.h"
#include "cedtHeader.h"
#include "PrefDialog.h"


void CPreferenceDialog::InitSyntaxPage()
{
	m_lstSyntaxTypes.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	CString szText( (LPCTSTR)IDS_PREF_SYNTAX_TYPE );
	m_lstSyntaxTypes.InsertColumn(0, szText, LVCFMT_LEFT, 219);

	m_btnSyntaxMoveUp.SetIcon( m_lstButtonImage.ExtractIcon(0) );
	m_btnSyntaxMoveDown.SetIcon( m_lstButtonImage.ExtractIcon(1) );
}

void CPreferenceDialog::SizeSyntaxPage()
{
	INT nPosY;

	nPosY  = 26; m_stcSyntaxSettingsBox.MoveWindow(170, nPosY, 360, 175);
	nPosY += 10; m_lstSyntaxTypes.MoveWindow(180, nPosY, 240, 152);		m_btnSyntaxRemove.MoveWindow(430, nPosY, 70, 18);
	nPosY += 20; m_btnSyntaxMoveUp.MoveWindow(430, nPosY, 34, 18);		m_btnSyntaxMoveDown.MoveWindow(466, nPosY, 34, 18);

	nPosY  = 26  + 150;
	nPosY += 35; m_stcSyntaxItemBox.MoveWindow(170, nPosY, 360, 75);
	nPosY += 10; m_stcSyntaxDescription.MoveWindow(180, nPosY, 80, 14);	m_edtSyntaxDescription.MoveWindow(270, nPosY-3, 230, 18);		
	nPosY += 20; m_stcSyntaxLangSpec.MoveWindow(180, nPosY, 80, 14);	m_edtSyntaxLangSpec.MoveWindow(270, nPosY-3, 230, 18);		m_btnSyntaxLangSpec.MoveWindow(505, nPosY-3, 20, 18);		
	nPosY += 20; m_stcSyntaxKeywords.MoveWindow(180, nPosY, 80, 14);	m_edtSyntaxKeywords.MoveWindow(270, nPosY-3, 230, 18);		m_btnSyntaxKeywords.MoveWindow(505, nPosY-3, 20, 18);		
}

void CPreferenceDialog::ShowSyntaxPage()
{
	INT nCmdShow = (m_nActiveCategory == PREF_CATEGORY_SYNTAX) ? SW_SHOW : SW_HIDE;

	m_stcSyntaxSettingsBox.ShowWindow(nCmdShow);
	m_lstSyntaxTypes.ShowWindow(nCmdShow);			m_btnSyntaxRemove.ShowWindow(nCmdShow);
	m_btnSyntaxMoveUp.ShowWindow(nCmdShow);			m_btnSyntaxMoveDown.ShowWindow(nCmdShow);

	m_stcSyntaxItemBox.ShowWindow(nCmdShow);
	m_stcSyntaxDescription.ShowWindow(nCmdShow);	m_edtSyntaxDescription.ShowWindow(nCmdShow);
	m_stcSyntaxLangSpec.ShowWindow(nCmdShow);		m_edtSyntaxLangSpec.ShowWindow(nCmdShow);		m_btnSyntaxLangSpec.ShowWindow(nCmdShow);
	m_stcSyntaxKeywords.ShowWindow(nCmdShow);		m_edtSyntaxKeywords.ShowWindow(nCmdShow);		m_btnSyntaxKeywords.ShowWindow(nCmdShow);
}

BOOL CPreferenceDialog::LoadSyntaxTypes()
{
	for(INT i = 0; i < MAX_SYNTAX_TYPE; i++) {
		m_clsSyntaxTypes[i].CopyContents( CCedtDoc::m_clsSyntaxTypes[i] );
		m_lstSyntaxTypes.InsertItem(i, "");
		DispSyntaxType(i);
	}

	m_lstSyntaxTypes.SetItemState(m_nActiveSyntaxType, LVIS_SELECTED, LVIS_SELECTED);
	m_lstSyntaxTypes.EnsureVisible(m_nActiveSyntaxType, FALSE);

	return TRUE;
}

BOOL CPreferenceDialog::SaveSyntaxTypes()
{
	for(INT i = 0; i < MAX_SYNTAX_TYPE; i++) {
		CCedtDoc::m_clsSyntaxTypes[i].CopyContents( m_clsSyntaxTypes[i] );
	}

	return TRUE;
}

BOOL CPreferenceDialog::DispSyntaxType(INT nSyntaxType)
{
	CString szText = m_clsSyntaxTypes[nSyntaxType].m_szDescription;
	if( ! szText.GetLength() ) szText = "- Empty -";
	m_lstSyntaxTypes.SetItemText(nSyntaxType, 0, szText);
	return TRUE;
}

void CPreferenceDialog::OnItemchangedSyntaxTypes(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if( (pNMListView->iItem >= 0) && (pNMListView->uNewState & LVIS_SELECTED) ) {
		INT nSyntax = pNMListView->iItem; m_nActiveSyntaxType = nSyntax;
		m_edtSyntaxDescription.SetWindowText( m_clsSyntaxTypes[nSyntax].m_szDescription );
		m_edtSyntaxLangSpec.SetWindowText( m_clsSyntaxTypes[nSyntax].m_szLangSpecFile );
		m_edtSyntaxKeywords.SetWindowText( m_clsSyntaxTypes[nSyntax].m_szKeywordsFile );
	}
	*pResult = 0;
}

void CPreferenceDialog::OnSyntaxRemove() 
{
	INT nSyntaxType = m_nActiveSyntaxType;
	CSyntaxType & rSyntaxType = m_clsSyntaxTypes[nSyntaxType];

	rSyntaxType.DeleteContents();
	DispSyntaxType( nSyntaxType );

	m_edtSyntaxDescription.SetWindowText( rSyntaxType.m_szDescription );
	m_edtSyntaxLangSpec.SetWindowText( rSyntaxType.m_szLangSpecFile );
	m_edtSyntaxKeywords.SetWindowText( rSyntaxType.m_szKeywordsFile );
}

void CPreferenceDialog::OnSyntaxMoveUp() 
{
	INT nSyntaxType = m_nActiveSyntaxType;

	if( nSyntaxType > 0 ) {
		CSyntaxType & rSyntaxType0 = CSyntaxType();
		CSyntaxType & rSyntaxType1 = m_clsSyntaxTypes[nSyntaxType-1];
		CSyntaxType & rSyntaxType2 = m_clsSyntaxTypes[nSyntaxType+0];

		rSyntaxType0.CopyContents( rSyntaxType1 );
		rSyntaxType1.CopyContents( rSyntaxType2 );
		rSyntaxType2.CopyContents( rSyntaxType0 );

		DispSyntaxType( nSyntaxType-1 );
		DispSyntaxType( nSyntaxType+0 );

		m_lstSyntaxTypes.SetItemState( nSyntaxType-1, LVIS_SELECTED, LVIS_SELECTED );
		m_lstSyntaxTypes.EnsureVisible( nSyntaxType-1, FALSE );
	}
}

void CPreferenceDialog::OnSyntaxMoveDown() 
{
	INT nSyntaxType = m_nActiveSyntaxType;

	if( nSyntaxType < m_lstSyntaxTypes.GetItemCount()-1 ) {
		CSyntaxType & rSyntaxType0 = CSyntaxType();
		CSyntaxType & rSyntaxType1 = m_clsSyntaxTypes[nSyntaxType+0];
		CSyntaxType & rSyntaxType2 = m_clsSyntaxTypes[nSyntaxType+1];

		rSyntaxType0.CopyContents( rSyntaxType1 );
		rSyntaxType1.CopyContents( rSyntaxType2 );
		rSyntaxType2.CopyContents( rSyntaxType0 );

		DispSyntaxType( nSyntaxType+0 );
		DispSyntaxType( nSyntaxType+1 );

		m_lstSyntaxTypes.SetItemState( nSyntaxType+1, LVIS_SELECTED, LVIS_SELECTED );
		m_lstSyntaxTypes.EnsureVisible( nSyntaxType+1, FALSE );
	}
}

void CPreferenceDialog::OnChangeSyntaxDescription() 
{
	INT nSyntaxType = m_nActiveSyntaxType;
	CSyntaxType & rSyntaxType = m_clsSyntaxTypes[nSyntaxType];
	m_edtSyntaxDescription.GetWindowText( rSyntaxType.m_szDescription );
	DispSyntaxType(nSyntaxType);
}

void CPreferenceDialog::OnChangeSyntaxLangSpec() 
{
	INT nSyntaxType = m_nActiveSyntaxType;
	CSyntaxType & rSyntaxType = m_clsSyntaxTypes[nSyntaxType];
	m_edtSyntaxLangSpec.GetWindowText( rSyntaxType.m_szLangSpecFile );
}

void CPreferenceDialog::OnChangeSyntaxKeywords() 
{
	INT nSyntaxType = m_nActiveSyntaxType;
	CSyntaxType & rSyntaxType = m_clsSyntaxTypes[nSyntaxType];
	m_edtSyntaxKeywords.GetWindowText( rSyntaxType.m_szKeywordsFile );
}

void CPreferenceDialog::OnSyntaxLangSpecBrowse() 
{
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	CString szFilter; szFilter.LoadString(IDS_FILTER_LANGSPEC_BROWSE);
	CString szFileName; m_edtSyntaxLangSpec.GetWindowText( szFileName );
	CFileDialog dlg(TRUE, NULL, szFileName, dwFlags, szFilter);

	CString szTitle; szTitle.LoadString(IDS_DLG_SELECT_FILE);
	CString szInitialDirectory = CCedtApp::m_szInstallDirectory + "\\spec";

	TCHAR szCurrentDirectory[MAX_PATH];
	GetCurrentDirectory( MAX_PATH, szCurrentDirectory );

	dlg.m_ofn.lpstrTitle = szTitle; dlg.m_ofn.lpstrInitialDir = szInitialDirectory;
	if( dlg.DoModal() != IDOK ) return;

	szFileName = GetFileName( dlg.GetPathName() ); szFileName.MakeUpper();
	m_edtSyntaxLangSpec.SetWindowText( szFileName );
	SetCurrentDirectory( szCurrentDirectory );
}

void CPreferenceDialog::OnSyntaxKeywordsBrowse() 
{
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	CString szFilter; szFilter.LoadString(IDS_FILTER_KEYWORD_BROWSE);
	CString szFileName; m_edtSyntaxKeywords.GetWindowText( szFileName );
	CFileDialog dlg(TRUE, NULL, szFileName, dwFlags, szFilter);

	CString szTitle; szTitle.LoadString(IDS_DLG_SELECT_FILE);
	CString szInitialDirectory = CCedtApp::m_szInstallDirectory + "\\spec";

	TCHAR szCurrentDirectory[MAX_PATH];
	GetCurrentDirectory( MAX_PATH, szCurrentDirectory );

	dlg.m_ofn.lpstrTitle = szTitle; dlg.m_ofn.lpstrInitialDir = szInitialDirectory;
	if( dlg.DoModal() != IDOK ) return;

	szFileName = GetFileName( dlg.GetPathName() ); szFileName.MakeUpper();
	m_edtSyntaxKeywords.SetWindowText( szFileName );
	SetCurrentDirectory( szCurrentDirectory );
}

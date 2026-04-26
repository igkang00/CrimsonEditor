#include "stdafx.h"
#include "cedtHeader.h"
#include "PrefDialog.h"


void CPreferenceDialog::InitMacrosPage()
{
	CString szText;

	m_lstMacroList.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	szText.LoadString( IDS_PREF_MACROS_NAME );
	m_lstMacroList.InsertColumn( 0, szText, LVCFMT_LEFT, 159 );
	szText.LoadString( IDS_PREF_MACROS_HOT_KEY );
	m_lstMacroList.InsertColumn( 1, szText, LVCFMT_LEFT, 60  );

	m_btnMacroMoveUp.SetIcon( m_lstButtonImage.ExtractIcon(0) );
	m_btnMacroMoveDown.SetIcon( m_lstButtonImage.ExtractIcon(1) );
}

void CPreferenceDialog::SizeMacrosPage()
{
	INT nPosY;

	nPosY  = 26; m_stcMacroSettingsBox.MoveWindow(170, nPosY, 360, 175);
	nPosY += 10; m_lstMacroList.MoveWindow(180, nPosY, 240, 152);			m_btnMacroRemove.MoveWindow(430, nPosY, 70, 18);
	nPosY += 20; m_btnMacroMoveUp.MoveWindow(430, nPosY, 34, 18);			m_btnMacroMoveDown.MoveWindow(466, nPosY, 34, 18);

	nPosY  = 26 + 150;
	nPosY += 35; m_stcMacroItemBox.MoveWindow(170, nPosY, 360, 55);
	nPosY += 10; m_stcMacroText.MoveWindow(180, nPosY, 80, 14);				m_edtMacroText.MoveWindow(270, nPosY-3, 230, 18);
	nPosY += 20; m_stcMacroHotKey.MoveWindow(180, nPosY, 80, 14);			m_hkyMacroHotKey.MoveWindow(270, nPosY-3, 230, 18);

	nPosY += 35; m_btnMacroLoadMacros.MoveWindow(170, nPosY, 170, 20);		m_btnMacroSaveMacros.MoveWindow(360, nPosY, 170, 20);
}

void CPreferenceDialog::ShowMacrosPage()
{
	INT nCmdShow = (m_nActiveCategory == PREF_CATEGORY_MACROS) ? SW_SHOW : SW_HIDE;

	m_stcMacroSettingsBox.ShowWindow(nCmdShow);
	m_lstMacroList.ShowWindow(nCmdShow);			m_btnMacroRemove.ShowWindow(nCmdShow);
	m_btnMacroMoveUp.ShowWindow(nCmdShow);			m_btnMacroMoveDown.ShowWindow(nCmdShow);

	m_stcMacroItemBox.ShowWindow(nCmdShow);
	m_stcMacroText.ShowWindow(nCmdShow);			m_edtMacroText.ShowWindow(nCmdShow);
	m_stcMacroHotKey.ShowWindow(nCmdShow);			m_hkyMacroHotKey.ShowWindow(nCmdShow);

	m_btnMacroLoadMacros.ShowWindow(nCmdShow);		m_btnMacroSaveMacros.ShowWindow(nCmdShow);
}


BOOL CPreferenceDialog::LoadMacroBuffers()
{
	for(INT i = 0; i < 11; i++) {
		m_clsMacroBuffer[i].CopyContents(CCedtView::m_clsMacroBuffer[i]);
		if( i > 0 ) {
			m_lstMacroList.InsertItem(i-1, "");
			DispMacroText(i-1);
		}
	}

	m_lstMacroList.SetItemState(m_nActiveMacroBuffer, LVIS_SELECTED, LVIS_SELECTED);
	m_lstMacroList.EnsureVisible(m_nActiveMacroBuffer, FALSE);

	return TRUE;
}

BOOL CPreferenceDialog::SaveMacroBuffers()
{
	for(INT i = 0; i < 11; i++) {
		CCedtView::m_clsMacroBuffer[i].CopyContents(m_clsMacroBuffer[i]);
	}

	return TRUE;
}

BOOL CPreferenceDialog::FileLoadMacroBuffers(LPCTSTR lpszPathName)
{
	ifstream fin(lpszPathName, ios::in | ios::nocreate | ios::binary);
	if( ! fin.is_open() ) return FALSE;

	TCHAR szBuffer[2048]; INT nLength = strlen(STRING_USERMACROVER); fin.read((char *)szBuffer, nLength); szBuffer[nLength] = '\0';
	if( strcmp(szBuffer, STRING_USERMACROVER) ) { fin.close(); return FALSE; }

	for(INT i = 0; i < 11; i++) {
		if( ! m_clsMacroBuffer[i].StreamLoad(fin) ) { fin.close(); return FALSE; }
	}

	fin.close();
	return TRUE;
}

BOOL CPreferenceDialog::FileSaveMacroBuffers(LPCTSTR lpszPathName)
{
	ofstream fout(lpszPathName, ios::out | ios::binary);
	if( ! fout.is_open() ) return FALSE;

	INT nLength = strlen(STRING_USERMACROVER);
	fout.write((const char *)STRING_USERMACROVER, nLength);

	for( INT i = 0; i < 11; i++ ) {
		if( ! m_clsMacroBuffer[i].StreamSave(fout) ) { fout.close(); return FALSE; }
	}

	fout.close();
	return TRUE;
}

BOOL CPreferenceDialog::DispMacroText(INT nMacro)
{
	m_lstMacroList.SetItemText(nMacro, 0, GetMacroName(nMacro+1));
	m_lstMacroList.SetItemText(nMacro, 1, GetMacroHotKeyText(nMacro+1));
	return TRUE;
}

CString CPreferenceDialog::GetMacroName(INT nMacro)
{
	CMacroBuffer & rBuffer = m_clsMacroBuffer[nMacro];
	if( rBuffer.GetBufferCount() ) {
		if( rBuffer.m_szName.GetLength() ) return rBuffer.m_szName;
		else return "[Noname]";
	} else return "- Empty -";
}

CString CPreferenceDialog::GetMacroHotKeyText(INT nMacro)
{
	CMacroBuffer & rBuffer = m_clsMacroBuffer[nMacro];
	CString szHotKeyText = rBuffer.GetHotKeyText();
	if( ! szHotKeyText.GetLength() ) szHotKeyText.Format("Alt+%d", nMacro % 10);
	return szHotKeyText;
}

void CPreferenceDialog::OnItemchangedMacroList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if( (pNMListView->iItem >= 0) && (pNMListView->uNewState & LVIS_SELECTED) ) {
		INT nMacro = pNMListView->iItem; m_nActiveMacroBuffer = nMacro;
		CMacroBuffer & rBuffer = m_clsMacroBuffer[nMacro+1];

		m_edtMacroText.SetWindowText( rBuffer.m_szName );
		m_hkyMacroHotKey.SetHotKey( rBuffer.m_wVirtualKeyCode, rBuffer.m_wModifiers );

		m_edtMacroText.EnableWindow( rBuffer.GetBufferCount() );
		m_hkyMacroHotKey.EnableWindow( rBuffer.GetBufferCount() );
	}
	*pResult = 0;
}

void CPreferenceDialog::OnMacroLoadMacros() 
{
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	CString szFilter; szFilter.LoadString(IDS_FILTER_MACRO_BUFFER);
	CFileDialog dlg(TRUE, NULL, NULL, dwFlags, szFilter);

	CString szTitle; szTitle.LoadString(IDS_DLG_LOAD_MACRO_BUFFERS);
	CString szInitialDirectory = CCedtApp::m_szInstallDirectory + "\\tools";

	TCHAR szCurrentDirectory[MAX_PATH];
	GetCurrentDirectory( MAX_PATH, szCurrentDirectory );

	dlg.m_ofn.lpstrTitle = szTitle; dlg.m_ofn.lpstrInitialDir = szInitialDirectory;
	if( dlg.DoModal() != IDOK ) return;

	SetCurrentDirectory( szCurrentDirectory );
	FileLoadMacroBuffers( dlg.GetPathName() );

	for( INT i = 1; i < 11; i++ ) DispMacroText(i-1);
}

void CPreferenceDialog::OnMacroSaveMacros() 
{
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CString szFilter; szFilter.LoadString(IDS_FILTER_MACRO_BUFFER);
	CFileDialog dlg(FALSE, ".macro", NULL, dwFlags, szFilter);

	CString szTitle; szTitle.LoadString(IDS_DLG_SAVE_MACRO_BUFFERS);
	CString szInitialDirectory = CCedtApp::m_szInstallDirectory + "\\tools";

	TCHAR szCurrentDirectory[MAX_PATH];
	GetCurrentDirectory( MAX_PATH, szCurrentDirectory );

	dlg.m_ofn.lpstrTitle = szTitle; dlg.m_ofn.lpstrInitialDir = szInitialDirectory;
	if( dlg.DoModal() != IDOK ) return;

	SetCurrentDirectory( szCurrentDirectory );
	FileSaveMacroBuffers( dlg.GetPathName() );
}

void CPreferenceDialog::OnMacroRemove() 
{
	INT nMacro = m_nActiveMacroBuffer;
	CMacroBuffer & rBuffer = m_clsMacroBuffer[nMacro+1];

	rBuffer.DeleteContents();
	DispMacroText( nMacro );

	m_edtMacroText.SetWindowText( rBuffer.m_szName );
	m_hkyMacroHotKey.SetHotKey( rBuffer.m_wVirtualKeyCode, rBuffer.m_wModifiers );

	m_edtMacroText.EnableWindow( rBuffer.GetBufferCount() );
	m_hkyMacroHotKey.EnableWindow( rBuffer.GetBufferCount() );
}

void CPreferenceDialog::OnMacroMoveUp() 
{
	INT nMacro = m_nActiveMacroBuffer;
	CMacroBuffer & rBuffer0 = m_clsMacroBuffer[       0];
	CMacroBuffer & rBuffer1 = m_clsMacroBuffer[nMacro+0];
	CMacroBuffer & rBuffer2 = m_clsMacroBuffer[nMacro+1];

	if( nMacro > 0 ) {
		rBuffer0.CopyContents( rBuffer1 );
		rBuffer1.CopyContents( rBuffer2 );
		rBuffer2.CopyContents( rBuffer0 );

		DispMacroText( nMacro-1 );
		DispMacroText( nMacro+0 );

		m_lstMacroList.SetItemState( nMacro-1, LVIS_SELECTED, LVIS_SELECTED );
		m_lstMacroList.EnsureVisible( nMacro-1, FALSE );
	}
}

void CPreferenceDialog::OnMacroMoveDown() 
{
	INT nMacro = m_nActiveMacroBuffer;
	CMacroBuffer & rBuffer0 = m_clsMacroBuffer[       0];
	CMacroBuffer & rBuffer1 = m_clsMacroBuffer[nMacro+1];
	CMacroBuffer & rBuffer2 = m_clsMacroBuffer[nMacro+2];

	if( nMacro < 9 ) {
		rBuffer0.CopyContents( rBuffer1 );
		rBuffer1.CopyContents( rBuffer2 );
		rBuffer2.CopyContents( rBuffer0 );

		DispMacroText( nMacro+0 );
		DispMacroText( nMacro+1 );

		m_lstMacroList.SetItemState( nMacro+1, LVIS_SELECTED, LVIS_SELECTED );
		m_lstMacroList.EnsureVisible( nMacro+1, FALSE );
	}
}


void CPreferenceDialog::OnChangeMacroText() 
{
	INT nMacro = m_nActiveMacroBuffer;
	CMacroBuffer & rBuffer = m_clsMacroBuffer[nMacro+1];
	m_edtMacroText.GetWindowText( rBuffer.m_szName );
	DispMacroText( nMacro );
}

void CPreferenceDialog::OnChangeMacroHotKey()
{
	INT nMacro = m_nActiveMacroBuffer;
	CMacroBuffer & rBuffer = m_clsMacroBuffer[nMacro+1];
	m_hkyMacroHotKey.GetHotKey( rBuffer.m_wVirtualKeyCode, rBuffer.m_wModifiers );
	DispMacroText( nMacro );
}

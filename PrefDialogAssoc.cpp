#include "stdafx.h"
#include "cedtHeader.h"
#include "PrefDialog.h"
#include "registry.h"


void CPreferenceDialog::InitAssocPage()
{
	m_bAssocInfoLoaded = FALSE;
}

void CPreferenceDialog::SizeAssocPage()
{
	INT nPosY;

	nPosY  = 26; m_stcAssocSettingsBox.MoveWindow(170, nPosY, 360, 175);
	nPosY += 10; m_lstAssocExtensions.MoveWindow(180, nPosY, 120, 152);
	nPosY +=  0; m_edtAssocAssociate.MoveWindow(320, nPosY, 100, 18);	 m_btnAssocAssociate.MoveWindow(430, nPosY, 70, 18);

	nPosY += 30; m_stcAssocAssociated.MoveWindow(320, nPosY, 200, 14);
	nPosY += 20; m_lstAssocAssociated.MoveWindow(320, nPosY, 100, 102);	 m_btnAssocRestore.MoveWindow(430, nPosY, 70, 18);

	nPosY  = 26 + 150;
	nPosY += 35; m_stcAssocItemBox.MoveWindow(170, nPosY, 360, 75);
	nPosY += 10; m_stcAssocDescription.MoveWindow(180, nPosY, 80, 14);	 m_edtAssocDescription.MoveWindow(270, nPosY-3, 230, 18);
	nPosY += 20; m_stcAssocProgram.MoveWindow(180, nPosY, 80, 14);		 m_edtAssocProgram.MoveWindow(270, nPosY-3, 230, 18);		 m_btnAssocProgram.MoveWindow(505, nPosY-3, 20, 18);
	nPosY += 20; m_stcAssocDefaultIcon.MoveWindow(180, nPosY, 80, 14);	 m_edtAssocDefaultIcon.MoveWindow(270, nPosY-3, 230, 18);	 m_btnAssocDefaultIcon.MoveWindow(505, nPosY-3, 20, 18);
}

void CPreferenceDialog::ShowAssocPage()
{
	INT nCmdShow = (m_nActiveCategory == PREF_CATEGORY_ASSOC) ? SW_SHOW : SW_HIDE;

	m_stcAssocSettingsBox.ShowWindow(nCmdShow);
	m_lstAssocExtensions.ShowWindow(nCmdShow);
	m_edtAssocAssociate.ShowWindow(nCmdShow);		m_btnAssocAssociate.ShowWindow(nCmdShow);

	m_stcAssocAssociated.ShowWindow(nCmdShow);
	m_lstAssocAssociated.ShowWindow(nCmdShow);		m_btnAssocRestore.ShowWindow(nCmdShow);

	m_stcAssocItemBox.ShowWindow(nCmdShow);
	m_stcAssocDescription.ShowWindow(nCmdShow);		m_edtAssocDescription.ShowWindow(nCmdShow);
	m_stcAssocProgram.ShowWindow(nCmdShow);			m_edtAssocProgram.ShowWindow(nCmdShow);			m_btnAssocProgram.ShowWindow(nCmdShow);
	m_stcAssocDefaultIcon.ShowWindow(nCmdShow);		m_edtAssocDefaultIcon.ShowWindow(nCmdShow);		m_btnAssocDefaultIcon.ShowWindow(nCmdShow);
}


BOOL CPreferenceDialog::LoadAssocSettings()
{
	if( m_bAssocInfoLoaded ) return TRUE;

	TCHAR szKeyName[2048]; CString szProgID, szExtension;
	DWORD dwSize = 2048; INT nCount = 0; CWaitCursor wait;
	
	while( ::RegEnumKeyEx(HKEY_CLASSES_ROOT, nCount, szKeyName, & dwSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS ) {
		if( szKeyName[0] == '.' && GetRegKeyValue(HKEY_CLASSES_ROOT, szKeyName, "", szProgID) ) {
			szExtension = szKeyName; szExtension.MakeLower();
			m_lstAssocExtensions.AddString(szExtension);

			if( ! strncmp(szProgID, PROGID_FILEASSOC_CRIMSONEDITOR, strlen(PROGID_FILEASSOC_CRIMSONEDITOR)) ) {
				m_lstAssocAssociated.AddString(szExtension);
			}
		}

		dwSize = 2048; nCount++; 
	}

	if( ! m_szActiveAssocExtension.GetLength() ) m_lstAssocExtensions.GetText( 0, m_szActiveAssocExtension );

	SetAssocExtensionsCurSel( m_szActiveAssocExtension );
	m_edtAssocAssociate.SetWindowText( m_szActiveAssocExtension );
	SetAssocAssociatedCurSel( m_szActiveAssocExtension );
	DispAssociationInfo( m_szActiveAssocExtension );

	m_bAssocInfoLoaded = TRUE;

	return TRUE;
}

BOOL CPreferenceDialog::SaveAssocSettings()
{
	return TRUE;
}

void CPreferenceDialog::DispAssociationInfo(LPCTSTR lpszExtension)
{
	CString szProgID, szRegPath1, szRegPath2;
	GetRegKeyValue(HKEY_CLASSES_ROOT, lpszExtension, "", szProgID);
	szRegPath1 = szProgID + "\\Shell\\Open\\Command";
	szRegPath2 = szProgID + "\\DefaultIcon";

	CString szDescription, szProgram, szDefaultIcon;
	GetRegKeyValue(HKEY_CLASSES_ROOT, szProgID, "", szDescription);
	GetRegKeyValue(HKEY_CLASSES_ROOT, szRegPath1, "", szProgram);
	GetRegKeyValue(HKEY_CLASSES_ROOT, szRegPath2, "", szDefaultIcon);

	m_edtAssocDescription.SetWindowText( szDescription );
	m_edtAssocProgram.SetWindowText( szProgram );
	m_edtAssocDefaultIcon.SetWindowText( szDefaultIcon );
}

void CPreferenceDialog::UpdateAssociationInfo(LPCTSTR lpszExtension)
{
	CString szProgID, szRegPath1, szRegPath2;
	GetRegKeyValue(HKEY_CLASSES_ROOT, lpszExtension, "", szProgID);
	szRegPath1 = szProgID + "\\Shell\\Open\\Command";
	szRegPath2 = szProgID + "\\DefaultIcon";

	CString szDescription, szProgram, szDefaultIcon;
	GetRegKeyValue(HKEY_CLASSES_ROOT, szProgID, "", szDescription);
	GetRegKeyValue(HKEY_CLASSES_ROOT, szRegPath1, "", szProgram);
	GetRegKeyValue(HKEY_CLASSES_ROOT, szRegPath2, "", szDefaultIcon);

	CString szDescription2, szProgram2, szDefaultIcon2;
	m_edtAssocDescription.GetWindowText( szDescription2 );
	m_edtAssocProgram.GetWindowText( szProgram2 );
	m_edtAssocDefaultIcon.GetWindowText( szDefaultIcon2 );

	if( szDescription.Compare( szDescription2 ) )
		SetRegKeyValue(HKEY_CLASSES_ROOT, szProgID, "", szDescription2);
	if( szProgram.Compare( szProgram2 ) )
		SetRegKeyValue(HKEY_CLASSES_ROOT, szRegPath1, "", szProgram2);
	if( szDefaultIcon.Compare( szDefaultIcon2 ) )
		SetRegKeyValue(HKEY_CLASSES_ROOT, szRegPath2, "", szDefaultIcon2);
}

void CPreferenceDialog::SetAssocExtensionsCurSel(LPCTSTR lpszExtension)
{
	INT nFound = m_lstAssocExtensions.FindStringExact(-1, lpszExtension);
	if( nFound == LB_ERR) nFound = -1;
	m_lstAssocExtensions.SetCurSel( nFound );
}

void CPreferenceDialog::SetAssocAssociatedCurSel(LPCTSTR lpszExtension)
{
	INT nFound = m_lstAssocAssociated.FindStringExact(-1, lpszExtension);
	if( nFound == LB_ERR) nFound = -1;
	m_lstAssocAssociated.SetCurSel( nFound );
}

BOOL CPreferenceDialog::AssociateExtension(LPCTSTR lpszExtension)
{
	// find existing progID
	BOOL bOldProgID; CString szOldProgID;
	bOldProgID = GetRegKeyValue(HKEY_CLASSES_ROOT, lpszExtension, "", szOldProgID);

	// new progID
	CString szNewProgID = PROGID_FILEASSOC_CRIMSONEDITOR;
	szNewProgID += lpszExtension;

	// get description
	CString szDescription;
	if( bOldProgID ) GetRegKeyValue(HKEY_CLASSES_ROOT, szOldProgID, "", szDescription);

	if( ! szDescription.GetLength() ) {
		szDescription = LPCTSTR(lpszExtension + 1);
		szDescription.MakeUpper(); szDescription += " File";
	}

	// register new progID
	if( ! SetRegKeyValue(HKEY_CLASSES_ROOT, szNewProgID, "", szDescription) ) return FALSE;

	// register existing progID to restore association
	if( ! SetRegKeyValue(HKEY_CLASSES_ROOT, szNewProgID, "backup", szOldProgID) ) return FALSE;

	// register default icon
	CString szRegPath1 = szNewProgID + "\\DefaultIcon";
	CString szDefaultIcon = CCedtApp::m_szInstallDirectory + "\\cedttype.ico";
	if( ! SetRegKeyValue(HKEY_CLASSES_ROOT, szRegPath1, "", szDefaultIcon) ) return FALSE;

	// register shell/open/command
	CString szRegPath2 = szNewProgID + "\\shell\\open\\command";
	CString szProgram = CCedtApp::m_szInstallDirectory + "\\cedt.exe \"%1\"";
	if( ! SetRegKeyValue(HKEY_CLASSES_ROOT, szRegPath2, "", szProgram) ) return FALSE;

	// associate extension
	if( ! SetRegKeyValue(HKEY_CLASSES_ROOT, lpszExtension, "", szNewProgID) ) return FALSE;

	return TRUE;
}


BOOL CPreferenceDialog::RestoreAssociation(LPCTSTR lpszExtension)
{
	// new progID
	CString szNewProgID = PROGID_FILEASSOC_CRIMSONEDITOR;
	szNewProgID += lpszExtension;

	// find old progID
	BOOL bOldProgID; CString szOldProgID;
	bOldProgID = GetRegKeyValue(HKEY_CLASSES_ROOT, szNewProgID, "backup", szOldProgID);

	// delete default icon
	CString szRegPath1 = szNewProgID + "\\DefaultIcon";
	if( ! DeleteRegKey(HKEY_CLASSES_ROOT, szRegPath1) ) return FALSE;

	// delete shell/open/command
	CString szRegPath2 = szNewProgID + "\\shell\\open\\command";
	if( ! DeleteRegKey(HKEY_CLASSES_ROOT, szRegPath2) ) return FALSE;

	// delete new progID
	if( ! DeleteRegKey(HKEY_CLASSES_ROOT, szNewProgID) ) return FALSE;

	if( szOldProgID.GetLength() ) { // restore association
		if( ! SetRegKeyValue(HKEY_CLASSES_ROOT, lpszExtension, "", szOldProgID) ) return FALSE;
	} else { // delete registered extension
		if( ! DeleteRegKey(HKEY_CLASSES_ROOT, lpszExtension) ) return FALSE;
	}

	return TRUE;
}

void CPreferenceDialog::OnSelchangeAssocExtensions() 
{
	INT nSelect = m_lstAssocExtensions.GetCurSel(); if( nSelect == LB_ERR) return;
	m_lstAssocExtensions.GetText(nSelect, m_szActiveAssocExtension);

	m_edtAssocAssociate.SetWindowText( m_szActiveAssocExtension );
	SetAssocAssociatedCurSel( m_szActiveAssocExtension );
	DispAssociationInfo( m_szActiveAssocExtension );
}

void CPreferenceDialog::OnSelchangeAssocAssociated() 
{
	INT nSelect = m_lstAssocAssociated.GetCurSel(); if( nSelect == LB_ERR ) return;
	m_lstAssocAssociated.GetText(nSelect, m_szActiveAssocExtension);

	m_edtAssocAssociate.SetWindowText( m_szActiveAssocExtension );
	SetAssocExtensionsCurSel( m_szActiveAssocExtension );
	DispAssociationInfo( m_szActiveAssocExtension );
}

void CPreferenceDialog::OnAssocAssociate() 
{
	CString szExtension; m_edtAssocAssociate.GetWindowText( szExtension );
	szExtension.TrimLeft(); szExtension.TrimRight(); szExtension.MakeLower();
	if( szExtension[0] != '.' ) szExtension = CString(".") + szExtension;
	if( szExtension.GetLength() < 2 ) return; // null extension

	INT nFound = m_lstAssocAssociated.FindStringExact(-1, szExtension);
	if( nFound != LB_ERR ) return; // already associated

	if( ! AssociateExtension( szExtension ) ) {
		AfxMessageBox(IDS_ERR_PREF_ASSOC_FAILED, MB_OK | MB_ICONSTOP);
		return; // association failed 
	}
	m_lstAssocAssociated.AddString( szExtension );

	nFound = m_lstAssocExtensions.FindStringExact(-1, szExtension);
	if( nFound == LB_ERR ) m_lstAssocExtensions.AddString( szExtension );

	SetAssocExtensionsCurSel( szExtension );
	SetAssocAssociatedCurSel( szExtension );
	DispAssociationInfo( szExtension );
}

void CPreferenceDialog::OnAssocRestore() 
{
	INT nSelect = m_lstAssocAssociated.GetCurSel(); if( nSelect == LB_ERR ) return;
	CString szExtension; m_lstAssocAssociated.GetText(nSelect, szExtension);

	if( ! RestoreAssociation( szExtension ) ) {
		AfxMessageBox(IDS_ERR_PREF_ASSOC_FAILED, MB_OK | MB_ICONSTOP);
		return; // restore failed
	}
	m_lstAssocAssociated.DeleteString( nSelect );

	CString szProgID;
	if( ! GetRegKeyValue(HKEY_CLASSES_ROOT, szExtension, "", szProgID) ) {
		INT nFound = m_lstAssocExtensions.FindStringExact(-1, szExtension);
		if( nFound != LB_ERR ) m_lstAssocExtensions.DeleteString( nFound );
	}

	m_lstAssocExtensions.GetText( 0, szExtension );
	SetAssocExtensionsCurSel( szExtension );
	SetAssocAssociatedCurSel( szExtension );
	DispAssociationInfo( szExtension );
}

void CPreferenceDialog::OnKillfocusAssocDescription() 
{
	INT nSelect = m_lstAssocExtensions.GetCurSel(); if( nSelect == LB_ERR) return;
	CString szExtension; m_lstAssocExtensions.GetText(nSelect, szExtension);

	UpdateAssociationInfo( szExtension );
}

void CPreferenceDialog::OnKillfocusAssocProgram() 
{
	INT nSelect = m_lstAssocExtensions.GetCurSel(); if( nSelect == LB_ERR) return;
	CString szExtension; m_lstAssocExtensions.GetText(nSelect, szExtension);

	UpdateAssociationInfo( szExtension );
}

void CPreferenceDialog::OnKillfocusAssocDefaulticon() 
{
	INT nSelect = m_lstAssocExtensions.GetCurSel(); if( nSelect == LB_ERR) return;
	CString szExtension; m_lstAssocExtensions.GetText(nSelect, szExtension);

	UpdateAssociationInfo( szExtension );
}


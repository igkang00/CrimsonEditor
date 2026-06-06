#include "stdafx.h"
#include "cedtHeader.h"
#include "PrefDialog.h"
#include "FolderDialog.h"


void CPreferenceDialog::InitBackupPage()
{
}

void CPreferenceDialog::SizeBackupPage()
{
	INT nPosY;

	nPosY  = 26; m_stcBackupSettingsBox.MoveWindow(170, nPosY, 360, 75);
	nPosY += 10; m_btnMakeBackupFile0.MoveWindow(180, nPosY, 300, 14);
	nPosY += 20; m_btnMakeBackupFile1.MoveWindow(180, nPosY, 300, 14);
	nPosY += 20; m_btnMakeBackupFile2.MoveWindow(180, nPosY, 300, 14);

	nPosY += 35; m_stcBackupExtensionBox.MoveWindow(170, nPosY, 360, 75);
	nPosY += 10; m_stcBackupExtension.MoveWindow(180, nPosY, 240, 14);		m_edtBackupExtension.MoveWindow(430, nPosY-3, 70, 18);
	nPosY += 20; m_stcBackupExtensionDesc1.MoveWindow(180, nPosY, 300, 14);
	nPosY += 20; m_stcBackupExtensionDesc2.MoveWindow(180, nPosY, 300, 14);
	
	nPosY += 35; m_stcBackupMethodBox.MoveWindow(170, nPosY, 360, 75);
	nPosY += 10; m_stcBackupMethod.MoveWindow(180, nPosY, 300, 14);
	nPosY += 20; m_btnBackupMethod1.MoveWindow(180, nPosY, 300, 14);
	nPosY += 20; m_btnBackupMethod2.MoveWindow(180, nPosY, 300, 14);

	nPosY += 35; m_stcBackupDirectoryBox.MoveWindow(170, nPosY, 360, 75);
	nPosY += 10; m_stcBackupDirectory.MoveWindow(180, nPosY, 300, 14);
	nPosY += 20; m_edtBackupDirectory.MoveWindow(180, nPosY-3, 320, 18);	m_btnBackupDirectory.MoveWindow(505, nPosY-3, 20, 18);
	nPosY += 20; m_stcBackupDirectoryDesc.MoveWindow(180, nPosY, 300, 14);
}

void CPreferenceDialog::ShowBackupPage()
{
	INT nCmdShow = (m_nActiveCategory == PREF_CATEGORY_BACKUP) ? SW_SHOW : SW_HIDE;

	m_stcBackupSettingsBox.ShowWindow(nCmdShow);
	m_btnMakeBackupFile0.ShowWindow(nCmdShow);
	m_btnMakeBackupFile1.ShowWindow(nCmdShow);
	m_btnMakeBackupFile2.ShowWindow(nCmdShow);

	m_stcBackupExtensionBox.ShowWindow(nCmdShow);
	m_stcBackupExtension.ShowWindow(nCmdShow);			m_edtBackupExtension.ShowWindow(nCmdShow);
	m_stcBackupExtensionDesc1.ShowWindow(nCmdShow);
	m_stcBackupExtensionDesc2.ShowWindow(nCmdShow);
	
	m_stcBackupMethodBox.ShowWindow(nCmdShow);
	m_stcBackupMethod.ShowWindow(nCmdShow);
	m_btnBackupMethod1.ShowWindow(nCmdShow);
	m_btnBackupMethod2.ShowWindow(nCmdShow);

	m_stcBackupDirectoryBox.ShowWindow(nCmdShow);
	m_stcBackupDirectory.ShowWindow(nCmdShow);
	m_edtBackupDirectory.ShowWindow(nCmdShow);			m_btnBackupDirectory.ShowWindow(nCmdShow);
	m_stcBackupDirectoryDesc.ShowWindow(nCmdShow);
}


BOOL CPreferenceDialog::LoadBackupSettings()
{
	m_btnMakeBackupFile0.SetCheck( CCedtDoc::m_nMakeBackupFile == BACKUP_DONT_BACKUP ); 
	m_btnMakeBackupFile1.SetCheck( CCedtDoc::m_nMakeBackupFile == BACKUP_BEFORE_SAVE ); 
	m_btnMakeBackupFile2.SetCheck( CCedtDoc::m_nMakeBackupFile == BACKUP_BEFORE_OPEN ); 

	m_edtBackupExtension.SetWindowText( CCedtDoc::m_szBackupExtension );
	m_edtBackupExtension.EnableWindow( CCedtDoc::m_nMakeBackupFile != BACKUP_DONT_BACKUP );

	m_btnBackupMethod1.SetCheck( CCedtDoc::m_nBackupMethod == BACKUP_METHOD01 );
	m_btnBackupMethod1.EnableWindow( CCedtDoc::m_nMakeBackupFile != BACKUP_DONT_BACKUP );
	m_btnBackupMethod2.SetCheck( CCedtDoc::m_nBackupMethod == BACKUP_METHOD02 );
	m_btnBackupMethod2.EnableWindow( CCedtDoc::m_nMakeBackupFile != BACKUP_DONT_BACKUP );

	m_edtBackupDirectory.SetWindowText( CCedtDoc::m_szBackupDirectory );
	m_edtBackupDirectory.EnableWindow( CCedtDoc::m_nMakeBackupFile != BACKUP_DONT_BACKUP );

	return TRUE;
}

BOOL CPreferenceDialog::SaveBackupSettings()
{
	if( m_btnMakeBackupFile0.GetCheck() ) CCedtDoc::m_nMakeBackupFile = BACKUP_DONT_BACKUP;
	if( m_btnMakeBackupFile1.GetCheck() ) CCedtDoc::m_nMakeBackupFile = BACKUP_BEFORE_SAVE;
	if( m_btnMakeBackupFile2.GetCheck() ) CCedtDoc::m_nMakeBackupFile = BACKUP_BEFORE_OPEN;

	m_edtBackupExtension.GetWindowText( CCedtDoc::m_szBackupExtension );

	if( m_btnBackupMethod1.GetCheck() ) CCedtDoc::m_nBackupMethod = BACKUP_METHOD01;
	if( m_btnBackupMethod2.GetCheck() ) CCedtDoc::m_nBackupMethod = BACKUP_METHOD02;

	m_edtBackupDirectory.GetWindowText( CCedtDoc::m_szBackupDirectory );

	return TRUE;
}

void CPreferenceDialog::OnMakeBackupFile0() 
{
	m_btnMakeBackupFile0.SetCheck( 1 );
	m_btnMakeBackupFile1.SetCheck( 0 );
	m_btnMakeBackupFile2.SetCheck( 0 );

	m_edtBackupExtension.EnableWindow( FALSE );
	m_btnBackupMethod1.EnableWindow( FALSE );
	m_btnBackupMethod2.EnableWindow( FALSE );
	m_edtBackupDirectory.EnableWindow( FALSE );
}

void CPreferenceDialog::OnMakeBackupFile1() 
{
	m_btnMakeBackupFile0.SetCheck( 0 );
	m_btnMakeBackupFile1.SetCheck( 1 );
	m_btnMakeBackupFile2.SetCheck( 0 );

	m_edtBackupExtension.EnableWindow( TRUE );
	m_btnBackupMethod1.EnableWindow( TRUE );
	m_btnBackupMethod2.EnableWindow( TRUE );
	m_edtBackupDirectory.EnableWindow( TRUE );
}

void CPreferenceDialog::OnMakeBackupFile2() 
{
	m_btnMakeBackupFile0.SetCheck( 0 );
	m_btnMakeBackupFile1.SetCheck( 0 );
	m_btnMakeBackupFile2.SetCheck( 1 );

	m_edtBackupExtension.EnableWindow( TRUE );
	m_btnBackupMethod1.EnableWindow( TRUE );
	m_btnBackupMethod2.EnableWindow( TRUE );
	m_edtBackupDirectory.EnableWindow( TRUE );
}

void CPreferenceDialog::OnBackupMethod1() 
{
	m_btnBackupMethod1.SetCheck( 1 );
	m_btnBackupMethod2.SetCheck( 0 );
}

void CPreferenceDialog::OnBackupMethod2() 
{
	m_btnBackupMethod1.SetCheck( 0 );
	m_btnBackupMethod2.SetCheck( 1 );
}

void CPreferenceDialog::OnBackupDirectoryBrowse() 
{
	CString szDirectory; m_edtBackupDirectory.GetWindowText( szDirectory );
	CString szText( (LPCTSTR)IDS_CHOOSE_DIRECTORY );
	CFolderDialog dlg(szText, szDirectory, NULL, this);
	if( dlg.DoModal() != IDOK ) return;
	m_edtBackupDirectory.SetWindowText(dlg.GetPathName());
}

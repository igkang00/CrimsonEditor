#include "stdafx.h"
#include "cedtHeader.h"
#include "PrefDialog.h"
#include "FolderDialog.h"


void CPreferenceDialog::InitDirectoryPage()
{
}

void CPreferenceDialog::SizeDirectoryPage()
{
	INT nPosY;

	nPosY  = 26; m_stcWorkingDirectoryBox.MoveWindow(170, nPosY, 360, 55);
	nPosY += 10; m_stcWorkingDirectory.MoveWindow(180, nPosY, 320, 14);
	nPosY += 20; m_edtWorkingDirectory.MoveWindow(180, nPosY-3, 320, 18);	m_btnWorkingDirectory.MoveWindow(505, nPosY-3, 20, 18);

	nPosY += 35; m_stcRemoteDirectoryBox.MoveWindow(170, nPosY, 360, 55);
	nPosY += 10; m_stcRemoteDirectory.MoveWindow(180, nPosY, 320, 14);
	nPosY += 20; m_edtRemoteDirectory.MoveWindow(180, nPosY-3, 320, 18);	m_btnRemoteDirectory.MoveWindow(505, nPosY-3, 20, 18);
}

void CPreferenceDialog::ShowDirectoryPage()
{
	INT nCmdShow = (m_nActiveCategory == PREF_CATEGORY_DIRECTORY) ? SW_SHOW : SW_HIDE;

	m_stcWorkingDirectoryBox.ShowWindow(nCmdShow);
	m_stcWorkingDirectory.ShowWindow(nCmdShow);	
	m_edtWorkingDirectory.ShowWindow(nCmdShow);		m_btnWorkingDirectory.ShowWindow(nCmdShow);

	m_stcRemoteDirectoryBox.ShowWindow(nCmdShow);
	m_stcRemoteDirectory.ShowWindow(nCmdShow);
	m_edtRemoteDirectory.ShowWindow(nCmdShow);		m_btnRemoteDirectory.ShowWindow(nCmdShow);
}

BOOL CPreferenceDialog::LoadDirectorySettings()
{
	m_edtWorkingDirectory.SetWindowText( CCedtApp::m_szInitialWorkingDirectory );
	m_edtRemoteDirectory.SetWindowText( CCedtApp::m_szRemoteBackupDirectory );

	return TRUE;
}

BOOL CPreferenceDialog::SaveDirectorySettings()
{
	m_edtWorkingDirectory.GetWindowText( CCedtApp::m_szInitialWorkingDirectory );
	m_edtRemoteDirectory.GetWindowText( CCedtApp::m_szRemoteBackupDirectory );

	return TRUE;
}

void CPreferenceDialog::OnWorkingDirectoryBrowse() 
{
	CString szDirectory; m_edtWorkingDirectory.GetWindowText( szDirectory );
	CString szText( (LPCTSTR)IDS_CHOOSE_DIRECTORY );
	CFolderDialog dlg(szText, szDirectory, NULL, this);
	if( dlg.DoModal() != IDOK ) return;
	m_edtWorkingDirectory.SetWindowText( dlg.GetPathName() );
}


void CPreferenceDialog::OnRemoteDirectoryBrowse() 
{
	CString szDirectory; m_edtRemoteDirectory.GetWindowText( szDirectory );
	CString szText( (LPCTSTR)IDS_CHOOSE_DIRECTORY );
	CFolderDialog dlg(szText, szDirectory, NULL, this);
	if( dlg.DoModal() != IDOK ) return;
	m_edtRemoteDirectory.SetWindowText( dlg.GetPathName() );
}

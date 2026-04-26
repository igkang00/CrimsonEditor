#include "stdafx.h"
#include "cedtHeader.h"
#include "PrefDialog.h"


void CPreferenceDialog::InitFilePage()
{
}

void CPreferenceDialog::SizeFilePage()
{
	INT nPosY;

	nPosY  = 26; m_stcFileSettingsBox.MoveWindow(170, nPosY, 360, 100);
	nPosY += 10; m_chkAllowMultiInstances.MoveWindow(180, nPosY, 300, 14);
	nPosY += 20; m_chkCheckFileModOutside.MoveWindow(180, nPosY, 300, 14);

	nPosY += 25; m_chkCreateNewDocument.MoveWindow(180, nPosY, 300, 14);
	nPosY += 20; m_chkReloadWorkingFiles.MoveWindow(180, nPosY, 300, 14);

	nPosY += 35; m_stcSaveSettingsBox.MoveWindow(170, nPosY, 360, 100);
	nPosY += 10; m_chkConvertTabsToSpaces.MoveWindow(180, nPosY, 300, 14);
	nPosY += 20; m_chkRemoveTrailingSpaces.MoveWindow(180, nPosY, 300, 14);

	nPosY += 25; m_chkSaveFilesUnix.MoveWindow(180, nPosY, 300, 14);
	nPosY += 20; m_chkSaveRemoteFilesUnix.MoveWindow(180, nPosY, 300, 14);

	nPosY += 35; m_stcDefDoctypeBox.MoveWindow(170, nPosY, 360, 75);
	nPosY += 10; m_stcDefDoctypeTitle.MoveWindow(180, nPosY, 300, 14);
	nPosY += 20; m_stcDefEncodingType.MoveWindow(180, nPosY, 80, 14);		m_cmbDefEncodingType.MoveWindow(270, nPosY-3, 230, 100);
	nPosY += 20; m_stcDefFileFormat.MoveWindow(180, nPosY, 80, 14);			m_cmbDefFileFormat.MoveWindow(270, nPosY-3, 230, 100);
}

void CPreferenceDialog::ShowFilePage()
{
	INT nCmdShow = (m_nActiveCategory == PREF_CATEGORY_FILE) ? SW_SHOW : SW_HIDE;

	m_stcFileSettingsBox.ShowWindow(nCmdShow);
	m_chkAllowMultiInstances.ShowWindow(nCmdShow);
	m_chkCheckFileModOutside.ShowWindow(nCmdShow);

	m_chkCreateNewDocument.ShowWindow(nCmdShow);
	m_chkReloadWorkingFiles.ShowWindow(nCmdShow);

	m_stcSaveSettingsBox.ShowWindow(nCmdShow);
	m_chkConvertTabsToSpaces.ShowWindow(nCmdShow);
	m_chkRemoveTrailingSpaces.ShowWindow(nCmdShow);

	m_chkSaveFilesUnix.ShowWindow(nCmdShow);
	m_chkSaveRemoteFilesUnix.ShowWindow(nCmdShow);

	m_stcDefDoctypeBox.ShowWindow(nCmdShow);
	m_stcDefDoctypeTitle.ShowWindow(nCmdShow);
	m_stcDefEncodingType.ShowWindow(nCmdShow);		m_cmbDefEncodingType.ShowWindow(nCmdShow);
	m_stcDefFileFormat.ShowWindow(nCmdShow);		m_cmbDefFileFormat.ShowWindow(nCmdShow);
}


BOOL CPreferenceDialog::LoadFileSettings()
{
	m_chkAllowMultiInstances.SetCheck(CCedtApp::m_bAllowMultiInstances ? 1 : 0);
	m_chkCheckFileModOutside.SetCheck(CCedtApp::m_bCheckIfFilesModifiedOutside ? 1 : 0);

	m_chkCreateNewDocument.SetCheck( CCedtApp::m_bCreateNewDocumentOnStartup ? 1 : 0 );
	m_chkReloadWorkingFiles.SetCheck( CCedtApp::m_bReloadWorkingFilesOnStartup ? 1 : 0 );

	m_chkConvertTabsToSpaces.SetCheck( CCedtDoc::m_bConvertTabsToSpacesBeforeSaving ? 1 : 0 );
	m_chkRemoveTrailingSpaces.SetCheck( CCedtDoc::m_bRemoveTrailingSpacesBeforeSaving ? 1 : 0 );

	m_chkSaveFilesUnix.SetCheck( CCedtDoc::m_bSaveFilesInUnixFormat ? 1 : 0 );
	m_chkSaveRemoteFilesUnix.SetCheck( CCedtDoc::m_bSaveRemoteFilesInUnixFormat ? 1 : 0 );

	m_cmbDefEncodingType.SetCurSel(CCedtDoc::m_nDefaultEncodingType - ENCODING_TYPE_ASCII);
	m_cmbDefFileFormat.SetCurSel(CCedtDoc::m_nDefaultFileFormat - FILE_FORMAT_DOS);

	return TRUE;
}

BOOL CPreferenceDialog::SaveFileSettings()
{
	CCedtApp::m_bAllowMultiInstances = m_chkAllowMultiInstances.GetCheck();
	CCedtApp::m_bCheckIfFilesModifiedOutside = m_chkCheckFileModOutside.GetCheck();

	CCedtApp::m_bCreateNewDocumentOnStartup = m_chkCreateNewDocument.GetCheck();
	CCedtApp::m_bReloadWorkingFilesOnStartup = m_chkReloadWorkingFiles.GetCheck();

	CCedtDoc::m_bConvertTabsToSpacesBeforeSaving = m_chkConvertTabsToSpaces.GetCheck();
	CCedtDoc::m_bRemoveTrailingSpacesBeforeSaving = m_chkRemoveTrailingSpaces.GetCheck();

	CCedtDoc::m_bSaveFilesInUnixFormat = m_chkSaveFilesUnix.GetCheck();
	CCedtDoc::m_bSaveRemoteFilesInUnixFormat = m_chkSaveRemoteFilesUnix.GetCheck();

	CCedtDoc::m_nDefaultEncodingType = m_cmbDefEncodingType.GetCurSel() + ENCODING_TYPE_ASCII;
	CCedtDoc::m_nDefaultFileFormat = m_cmbDefFileFormat.GetCurSel() + FILE_FORMAT_DOS;

	return TRUE;
}

#include "stdafx.h"
#include "cedtHeader.h"
#include "PrefDialog.h"


void CPreferenceDialog::InitToolsPage()
{
}

void CPreferenceDialog::SizeToolsPage()
{
	INT nPosY;

	nPosY  = 26; m_stcToolSettingsBox.MoveWindow(170, nPosY, 360, 70);
	nPosY += 10; m_chkUseInInternetExplorer.MoveWindow(180, nPosY, 340, 14);
	nPosY += 20; m_chkAddToRightMouseButton.MoveWindow(180, nPosY, 340, 14);
	nPosY += 15; m_stcAddToRightMouseButton.MoveWindow(198, nPosY, 320, 14);
}

void CPreferenceDialog::ShowToolsPage()
{
	INT nCmdShow = (m_nActiveCategory == PREF_CATEGORY_TOOLS) ? SW_SHOW : SW_HIDE;

	m_stcToolSettingsBox.ShowWindow(nCmdShow);
	m_chkUseInInternetExplorer.ShowWindow(nCmdShow);
	m_chkAddToRightMouseButton.ShowWindow(nCmdShow);
	m_stcAddToRightMouseButton.ShowWindow(nCmdShow);
}


BOOL CPreferenceDialog::LoadToolsSettings()
{
	m_chkUseInInternetExplorer.SetCheck( CCedtApp::IsUsedInInternetExplorer() );
	m_chkAddToRightMouseButton.SetCheck( CCedtApp::IsAddedToRightMouseButton() );

	return TRUE;
}

BOOL CPreferenceDialog::SaveToolsSettings()
{
	return TRUE;
}

void CPreferenceDialog::OnUseInInternetExplorer() 
{
	if( ! CCedtApp::UseInInternetExplorer( m_chkUseInInternetExplorer.GetCheck() ) ) {
		AfxMessageBox(IDS_ERR_PREF_TOOLS_APPLY, MB_OK | MB_ICONSTOP);
		m_chkUseInInternetExplorer.SetCheck( CCedtApp::IsUsedInInternetExplorer() );
	}
}

void CPreferenceDialog::OnAddToRightMouseButton() 
{
	if( ! CCedtApp::AddToRightMouseButton( m_chkAddToRightMouseButton.GetCheck() ) ) {
		AfxMessageBox(IDS_ERR_PREF_TOOLS_APPLY, MB_OK | MB_ICONSTOP);
		m_chkAddToRightMouseButton.SetCheck( CCedtApp::IsAddedToRightMouseButton() );
	}
}

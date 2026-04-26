#include "stdafx.h"
#include "cedtHeader.h"
#include "registry.h"


BOOL CCedtApp::SaveMultiInstancesFlag(LPCTSTR lpszProfileName)
{
	CString szAllowMultiReg = GetProfileString(lpszProfileName, "", NULL);
	CString szAllowMultiNow = ( m_bAllowMultiInstances ) ? "yes" : "no";

	if( ! szAllowMultiReg.CompareNoCase(szAllowMultiNow) ) return TRUE; // no need to save
	if( ! WriteProfileString(lpszProfileName, "", szAllowMultiNow) ) return FALSE;

	return TRUE;
}

BOOL CCedtApp::LoadMultiInstancesFlag(LPCTSTR lpszProfileName)
{
	CString szAllowMulti;
	m_bAllowMultiInstances = FALSE;

	szAllowMulti = GetProfileString(lpszProfileName, "", NULL);
	if( ! szAllowMulti.CompareNoCase("yes") ) m_bAllowMultiInstances = TRUE;

	return TRUE;
}

BOOL CCedtApp::SaveBrowsingDirectory(LPCTSTR lpszProfileName)
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();
	CString szDirectory; pFileWindow->GetBrowsingDirectory( szDirectory );
	WriteProfileString(lpszProfileName, "", szDirectory);

	return TRUE;
}

BOOL CCedtApp::LoadBrowsingDirectory(LPCTSTR lpszProfileName)
{
	TCHAR szCurrentDirectory[MAX_PATH]; GetCurrentDirectory(MAX_PATH, szCurrentDirectory);
	CString szDirectory = GetProfileString(lpszProfileName, "", NULL);

	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();
	if( szDirectory.GetLength() ) pFileWindow->SetBrowsingDirectory( szDirectory );
	else pFileWindow->SetBrowsingDirectory( szCurrentDirectory );

	return TRUE;
}

BOOL CCedtApp::SaveWorkingDirectory(LPCTSTR lpszProfileName)
{
	TCHAR szDirectory[MAX_PATH]; GetCurrentDirectory(MAX_PATH, szDirectory);
	WriteProfileString(lpszProfileName, "", szDirectory);
	return TRUE;
}

BOOL CCedtApp::LoadWorkingDirectory(LPCTSTR lpszProfileName)
{
	CString szDirectory = GetProfileString(lpszProfileName, "", NULL);
	if( szDirectory.GetLength() ) SetCurrentDirectory(szDirectory);
	return TRUE;
}

BOOL CCedtApp::SaveWorkspaceFilePath(LPCTSTR lpszProfileName)
{
	CString szPathName = m_szAppDataDirectory + "\\cedt.wks";
	if( m_bProjectLoaded ) szPathName = m_szProjectPathName;
	WriteProfileString(lpszProfileName, "", szPathName);

	return TRUE;
}

BOOL CCedtApp::LoadWorkspaceFilePath(LPCTSTR lpszProfileName)
{
	CString szPathName = GetProfileString(lpszProfileName, "", NULL);
	if( szPathName.GetLength() ) m_szPrevWorkspacePathName = szPathName;
	else m_szPrevWorkspacePathName = "";

	return TRUE;
}

BOOL CCedtApp::IsUsedInInternetExplorer()
{
	CString szRegValue;
	if( ! GetRegKeyValue(HKEY_LOCAL_MACHINE, REGPATH_USEININTERNETEXPLORER, "", szRegValue) ) return FALSE;
	return ! szRegValue.CompareNoCase(m_szInstallDirectory + "\\notepad.exe");
}

BOOL CCedtApp::UseInInternetExplorer(BOOL bUse)
{
	BOOL bReg = IsUsedInInternetExplorer();
	if( (bUse && bReg) || (! bUse && ! bReg) ) return TRUE;

	if( bUse ) {
		CString szRegValue = m_szInstallDirectory + "\\notepad.exe";
		if( ! SetRegKeyValue(HKEY_LOCAL_MACHINE, REGPATH_USEININTERNETEXPLORER, "", szRegValue) ) return FALSE;
	} else {
		if( ! DeleteRegKey(HKEY_LOCAL_MACHINE, REGPATH_USEININTERNETEXPLORER) ) return FALSE;
	}

	return TRUE;
}

BOOL CCedtApp::IsAddedToRightMouseButton()
{
	CString szRegValue;
	if( ! GetRegKeyValue(HKEY_CLASSES_ROOT, REGPATH_ADDTORIGHTMOUSEBUTTON, "", szRegValue) ) return FALSE;
	return ! szRegValue.CompareNoCase(CLSID_SHELLEXT_CRIMSONEDITOR);
}

BOOL CCedtApp::AddToRightMouseButton(BOOL bAdd)
{
	BOOL bReg = IsAddedToRightMouseButton();
	if( (bAdd && bReg) || (! bAdd && ! bReg) ) return TRUE;

	if( bAdd ) {
		// register install directory first. This information will be used in ShellExt.dll
		if( ! SetRegKeyValue(HKEY_LOCAL_MACHINE, REGPATH_INSTALL_DIRECTORY, "InstallDir", m_szInstallDirectory) ) return FALSE;

		CString szRegValue = m_szInstallDirectory + "\\ShellExt.dll";
		if( ! RegisterInProcServer(CLSID_SHELLEXT_CRIMSONEDITOR, PROGID_SHELLEXT_CRIMSONEDITOR, szRegValue) ) return FALSE;
		if( ! SetRegKeyValue(HKEY_LOCAL_MACHINE, REGPATH_SHELLEXTENSIONAPPROVED, CLSID_SHELLEXT_CRIMSONEDITOR, PROGID_SHELLEXT_CRIMSONEDITOR) ) return FALSE;
		if( ! SetRegKeyValue(HKEY_CLASSES_ROOT, REGPATH_ADDTORIGHTMOUSEBUTTON, "", CLSID_SHELLEXT_CRIMSONEDITOR) ) return FALSE;
	} else {
		if( ! UnregisterInProcServer(CLSID_SHELLEXT_CRIMSONEDITOR, PROGID_SHELLEXT_CRIMSONEDITOR) ) return FALSE;
		if( ! DeleteRegValue(HKEY_LOCAL_MACHINE, REGPATH_SHELLEXTENSIONAPPROVED, CLSID_SHELLEXT_CRIMSONEDITOR) ) return FALSE;
		if( ! DeleteRegKey(HKEY_CLASSES_ROOT, REGPATH_ADDTORIGHTMOUSEBUTTON) ) return FALSE;
	}

	return TRUE;
}


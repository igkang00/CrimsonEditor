#include "stdafx.h"
#include "cedtHeader.h"
#include "ReloadAsDialog.h"
#include "OpenRemoteDialog.h"
#include "FtpPasswordDialog.h"
#include "FtpTransferDialog.h"


void CCedtDoc::OnFileReload() 
{
	// check if it is a saved file in the disk
	if( IsNewFileNotSaved() ) return;

	if( IsModified() ) { // confirm user to discard changes
		CString szPathName = GetPathName();
		if( IsRemoteFile() ) szPathName = GetFullRemotePathName();

		CString szPrompt; szPrompt.Format(IDS_MSG_CONFIRM_TO_RELOAD, szPathName);
		if( AfxMessageBox(szPrompt, MB_YESNO | MB_ICONEXCLAMATION) != IDYES ) return;
	} 
	
	FileReload( m_nEncodingType );
}

void CCedtDoc::OnFileReloadAs() 
{
	// check if it is a saved file in the disk
	if( IsNewFileNotSaved() ) return;

	CReloadAsDialog dlg;
	dlg.m_nEncodingType = ENCODING_TYPE_UNKNOWN;

	if( dlg.DoModal() != IDOK ) return;

	if( IsModified() ) { // confirm user to discard changes
		CString szPathName = GetPathName();
		if( IsRemoteFile() ) szPathName = GetFullRemotePathName();

		CString szPrompt; szPrompt.Format(IDS_MSG_CONFIRM_TO_RELOAD, szPathName);
		if( AfxMessageBox(szPrompt, MB_YESNO | MB_ICONEXCLAMATION) != IDYES ) return;
	}

	FileReload( dlg.m_nEncodingType );
}

void CCedtDoc::OnFileSave() 
{
	FileSave();
}

void CCedtDoc::OnFileSaveAs() 
{
	FileSaveAs();
}

void CCedtDoc::OnFileSaveAsRemote() 
{
	FileSaveAsRemote();
}

void CCedtDoc::OnFileSaveAll() 
{
	CCedtApp * pApp = (CCedtApp *)AfxGetApp();
	POSITION posDoc = pApp->GetFirstDocPosition();
	while( posDoc ) {
		CCedtDoc * pDoc = (CCedtDoc *)pApp->GetNextDoc( posDoc );
		if( pDoc->IsModified() ) pDoc->FileSave();
	}
}

BOOL CCedtDoc::SaveModified() 
{
	if( ! IsModified() ) return TRUE;

	CString szPathName = GetPathName();
	if( IsRemoteFile() ) szPathName = GetFullRemotePathName();
	if( szPathName.IsEmpty() ) szPathName = GetTitle();

	CString szPrompt; AfxFormatString1(szPrompt, AFX_IDP_ASK_TO_SAVE, szPathName);
	INT nResult = AfxMessageBox(szPrompt, MB_YESNOCANCEL, AFX_IDP_ASK_TO_SAVE);

	if( nResult == IDCANCEL ) return FALSE;
	else if( nResult == IDNO ) return TRUE;

	return FileSave();
}

BOOL CCedtDoc::FileReload(INT nEncodingType)
{
	if( IsNewFileNotSaved() ) return TRUE;

	CCedtView * pView = (CCedtView *)GetFirstView();
	INT nLineNum = pView->GetCurrentLineNumber();

	if( IsRemoteFile() ) {
		INT nAccount = m_nFtpAccount; CString szPathName = m_szRemotePathName;
		return ReloadRemoteDocumentFile( nAccount, szPathName, nLineNum, nEncodingType );
	} else {
		CString szPathName = GetPathName();
		return ReloadDocumentFile( szPathName, nLineNum, nEncodingType );
	}
}

BOOL CCedtDoc::FileSave()
{
	if( IsNewFileNotSaved() ) return FileSaveAs();

	m_dwFileAttribute = GetFileAttributes( GetPathName() ); // get attribute again just before
	if( IsReadOnlyFile() || IsHiddenFile() || IsSystemFile() ) return FileSaveAs();

	if( IsRemoteFile() ) {
		// we need to copy these, because m_szRemotePathName will be changed in OnDocumentSave()
		INT nFtpAccount = m_nFtpAccount; CString szPathName = m_szRemotePathName;
		return SaveRemoteDocumentFile( nFtpAccount, szPathName );
	} else {
		CString szPathName = GetPathName();
		return SaveDocumentFile( szPathName );
	}
}

BOOL CCedtDoc::FileSaveAs()
{
	CString szPathName = GetTitle();
	if( IsRemoteFile() ) szPathName = GetFileName( GetRemotePathName() );

	CCedtApp * pApp = (CCedtApp *)AfxGetApp();
	if( pApp->PromptSaveFileName( szPathName ) ) {
		return SaveDocumentFile( szPathName );
	} else return FALSE;
}

BOOL CCedtDoc::FileSaveAsRemote()
{
	INT nFtpAccount = IsRemoteFile() ? m_nFtpAccount : 0;
	CString szPathName = GetFileName( GetTitle() );

	CCedtApp * pApp = (CCedtApp *)AfxGetApp();
	if( pApp->PromptSaveRemoteFileName( nFtpAccount, szPathName ) ) {
		return SaveRemoteDocumentFile( nFtpAccount, szPathName );
	} else return FALSE;
}

BOOL CCedtDoc::SaveDocumentFile(LPCTSTR lpszPathName)
{
	m_nCurrentFtpAccount = -1;
	m_szCurrentRemotePathName = "";

	if( ! OnSaveDocument( lpszPathName ) ) return FALSE;
	SetPathName( lpszPathName );

	return TRUE;
}

BOOL CCedtDoc::SaveRemoteDocumentFile(INT nAccount, LPCTSTR lpszPathName)
{
	CString szLocalPath = ChopDirectory(CCedtApp::m_szAppDataDirectory) + "\\";
	if( CCedtApp::m_szRemoteBackupDirectory.GetLength() ) szLocalPath = ChopDirectory(CCedtApp::m_szRemoteBackupDirectory) + "\\";
	szLocalPath += CCedtApp::m_clsFtpAccounts[nAccount].GetShortAccountName();
	szLocalPath += RemotePathToLocalPath(lpszPathName);

	if( ! TouchFile( szLocalPath ) ) {
		CString szMessage; szMessage.Format(IDS_ERR_BACKUP_REMOTE_FILE, szLocalPath);
		AfxMessageBox(szMessage, MB_OK | MB_ICONSTOP); return FALSE;
	}

	BOOL bModified = IsModified();
	BOOL bDocumentSaved = m_bDocumentSaved;
	INT  nSavedUndoCount = m_nSavedUndoCount;
	
	m_nCurrentFtpAccount = nAccount;
	m_szCurrentRemotePathName = lpszPathName;

	if( ! OnSaveDocument( szLocalPath ) ) return FALSE;
	SetPathName( szLocalPath );

	CFtpAccount & rFtpAccount = CCedtApp::m_clsFtpAccounts[nAccount];
	if( ! rFtpAccount.IsValid() ) return FALSE;

	// if account password is not given, ask user
	if( ! rFtpAccount.m_bSavePassword && ! rFtpAccount.m_bPasswordVerified ) {
		CFtpPasswordDialog dlg; dlg.m_szAccountInfo = rFtpAccount.GetShortAccountName();
		if( dlg.DoModal() != IDOK ) return NULL;
		rFtpAccount.m_szPassword = dlg.m_szPassword;
	}

	// upload the local file
	CFtpTransferDialog dlg(FALSE, rFtpAccount, lpszPathName, szLocalPath);

	if( dlg.DoModal() != IDOK ) { // operation is canceled by the user
		m_bDocumentSaved = bDocumentSaved; m_nSavedUndoCount = nSavedUndoCount; // restore original flag
		SetModifiedFlag( bModified ); UpdateAllViews(NULL);
		CString szMessage; szMessage.Format(IDS_ERR_OPEN_REMOTE_CANCEL);
		AfxMessageBox(szMessage, MB_OK | MB_ICONSTOP); return FALSE;
	} else if( ! dlg.IsTransferCompleted() ) {
		m_bDocumentSaved = bDocumentSaved; m_nSavedUndoCount = nSavedUndoCount; // restore original flag
		SetModifiedFlag( bModified ); UpdateAllViews(NULL);
		CString szMessage; szMessage.Format(IDS_ERR_SAVE_REMOTE_FILE, lpszPathName);
		AfxMessageBox(szMessage, MB_OK | MB_ICONSTOP); return FALSE;
	}

	// operation successful, password verified
	if( ! rFtpAccount.m_bSavePassword ) rFtpAccount.m_bPasswordVerified = TRUE;

	return TRUE;
}

BOOL CCedtDoc::ReloadDocumentFile(LPCTSTR lpszPathName, INT nLineNum, INT nEncodingType)
{
	CCedtDoc::m_nCurrentFtpAccount = -1;
	CCedtDoc::m_szCurrentRemotePathName = "";

	if( ! OnReloadDocument(lpszPathName, nEncodingType) ) return FALSE;
	if( nLineNum ) GoToLineNumber( nLineNum );

	if( HaveAnyOverflowLine() ) {
		CString szMessage; szMessage.Format(IDS_MSG_HAVE_LINE_OVERFLOW, MAX_STRING_SIZE);
		AfxMessageBox(szMessage, MB_OK | MB_ICONINFORMATION);
	}

	return TRUE;
}

BOOL CCedtDoc::ReloadRemoteDocumentFile(INT nAccount, LPCTSTR lpszPathName, INT nLineNum, INT nEncodingType)
{
	LPCTSTR szLocalPath = GetPathName();
	CCedtDoc::m_nCurrentFtpAccount = nAccount;
	CCedtDoc::m_szCurrentRemotePathName = lpszPathName;

	CFtpAccount & rFtpAccount = CCedtApp::m_clsFtpAccounts[nAccount];
	if( ! rFtpAccount.IsValid() ) return FALSE;

	// if account password is not given, ask user
	if( ! rFtpAccount.m_bSavePassword && ! rFtpAccount.m_bPasswordVerified ) {
		CFtpPasswordDialog dlg; dlg.m_szAccountInfo = rFtpAccount.GetShortAccountName();
		if( dlg.DoModal() != IDOK ) return NULL;
		rFtpAccount.m_szPassword = dlg.m_szPassword;
	}

	// download the remote file
	CFtpTransferDialog dlg(TRUE, rFtpAccount, lpszPathName, szLocalPath);

	if( dlg.DoModal() != IDOK ) { // operation is canceled by the user
		CString szMessage; szMessage.Format(IDS_ERR_OPEN_REMOTE_CANCEL);
		AfxMessageBox(szMessage, MB_OK | MB_ICONSTOP); return FALSE;
	} else if( ! dlg.IsTransferCompleted() ) {
		CString szMessage; szMessage.Format(IDS_ERR_RELOAD_REMOTE_FILE, m_szRemotePathName);
		AfxMessageBox(szMessage, MB_OK | MB_ICONSTOP); return FALSE;
	}

	// operation successful, password verified
	if( ! rFtpAccount.m_bSavePassword ) rFtpAccount.m_bPasswordVerified = TRUE;

	if( ! OnReloadDocument(szLocalPath, nEncodingType) ) return FALSE;
	if( nLineNum ) GoToLineNumber( nLineNum );

	if( HaveAnyOverflowLine() ) {
		CString szMessage; szMessage.Format(IDS_MSG_HAVE_LINE_OVERFLOW, MAX_STRING_SIZE);
		AfxMessageBox(szMessage, MB_OK | MB_ICONINFORMATION);
	}

	return TRUE;
}

BOOL CCedtDoc::UpdateFileStatus()
{
	CString szPathName = GetPathName();
	if( ! szPathName.GetLength() ) return FALSE;
	return CFile::GetStatus(szPathName, m_clsFileStatus);
}

BOOL CCedtDoc::BackupDocument(LPCTSTR lpszPathName)
{
	CString szBackupDirectory = GetFileDirectory( lpszPathName );
	if( m_szBackupDirectory.GetLength() ) {
		CString szFileDirectory = GetFileDirectory( lpszPathName );
		if( IsRemoteFile() ) {
			CString szLocalPath = ChopDirectory(CCedtApp::m_szAppDataDirectory) + "\\";
			if( CCedtApp::m_szRemoteBackupDirectory.GetLength() ) szLocalPath = ChopDirectory(CCedtApp::m_szRemoteBackupDirectory) + "\\";
			CString szRemainDir = szFileDirectory; szRemainDir.Replace( szLocalPath, "" );
			szBackupDirectory = ChopDirectory(m_szBackupDirectory) + "\\" + szRemainDir;
		} else {
			CString szDriveName = szFileDirectory.Left(2);
			CString szRemainDir = szFileDirectory.Mid(2);
			if( ! szDriveName.Compare("\\\\") ) szDriveName = ""; // "Network\\";
			else if( szDriveName[1] == ':' ) szDriveName.Format("%c drive", szDriveName[0]);
			szBackupDirectory = ChopDirectory(m_szBackupDirectory) + "\\" + szDriveName + szRemainDir;
		}
	}

	INT nLength = szBackupDirectory.GetLength(); // chop directory
	if( szBackupDirectory[nLength-1] == '\\' ) szBackupDirectory = szBackupDirectory.Mid(0, nLength-1);

	CString szBackupExtension = m_szBackupExtension;
	if( ! szBackupExtension.GetLength() ) szBackupExtension = "bak";

	CString szBackupFilePath;
	if( m_nBackupMethod == BACKUP_METHOD01 ) { // filename.ext.bak
		CString szFileName = GetFileName( lpszPathName );
		szBackupFilePath = szBackupDirectory + "\\" + szFileName + "." + szBackupExtension;
	} else if( m_nBackupMethod == BACKUP_METHOD02 ) { // filename_bak.ext
		CString szFileTitle = GetFileTitle( lpszPathName );
		CString szExtension = GetFileExtension( lpszPathName );
		szBackupFilePath = szBackupDirectory + "\\" + szFileTitle + "_" + szBackupExtension + szExtension;
	} else { // otherwise filename.bak
		CString szFileTitle = GetFileTitle( lpszPathName );
		szBackupFilePath = szBackupDirectory + "\\" + szFileTitle + "." + szBackupExtension;
	}

	if( ! szBackupExtension.Compare("$$$") ) { // incremental number
		CString szTestFilePath, szIncrement;
		INT nIncrement = 0;

		do { // test until there is no such file path
			szTestFilePath = szBackupFilePath;
			szIncrement.Format("%03d", nIncrement++);
			szTestFilePath.Replace("$$$", szIncrement);
		} while( VerifyFilePath(szTestFilePath) );

		szBackupFilePath = szTestFilePath;
	}

	TRACE1( "CREATING BACKUP FILE: %s\n", szBackupFilePath );
	if( ! TouchFile( szBackupFilePath ) ) return FALSE;
	if( ! CopyFile( lpszPathName, szBackupFilePath, FALSE ) ) return FALSE;

	return TRUE;
}

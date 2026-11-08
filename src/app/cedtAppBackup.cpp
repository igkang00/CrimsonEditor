// cedtAppBackup.cpp : bringing unsaved documents back at startup (see docs/hot-exit.md)
//
// The restore runs in two halves around the workspace reload in InitInstance:
//
//  - FTP documents first. Left to the workspace, each would be downloaded again (and might ask
//    for a password) only to have its text replaced; opened from the local cache beforehand,
//    the workspace finds them open and leaves them be.
//  - Local and untitled documents after. The workspace reopens files through
//    SpawnDocumentFile, which opens a SECOND window onto a document that is already open, so
//    these must not be open before it runs. Afterwards, a document the workspace opened just
//    has its text replaced; anything else is opened or created here.
//
// It runs whenever backups are on disk, even with "Reload working files on startup" now off:
// they hold the user's unsaved work.

#include "stdafx.h"
#include "cedtHeader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CString CCedtApp::GetRemoteCachePathName(INT nAccount, LPCTSTR lpszRemotePathName)
{
	CString szLocalPath = ChopDirectory(m_szAppDataDirectory) + _T("\\");
	if( m_szRemoteBackupDirectory.GetLength() ) szLocalPath = ChopDirectory(m_szRemoteBackupDirectory) + _T("\\");
	szLocalPath += m_clsFtpAccounts[nAccount].GetShortAccountName();
	szLocalPath += RemotePathToLocalPath(lpszRemotePathName);
	return szLocalPath;
}

CDocument * CCedtApp::GetRemoteDocument(INT nAccount, LPCTSTR lpszRemotePathName)
{
	POSITION posDoc = GetFirstDocPosition();
	while( posDoc ) {
		CCedtDoc * pDoc = (CCedtDoc *)GetNextDoc( posDoc );
		if( pDoc->IsRemoteFile() && pDoc->GetFtpAccountNumber() == nAccount
			&& pDoc->GetRemotePathName() == lpszRemotePathName ) return pDoc;
	}
	return NULL;
}

// A new, empty document in its own window, as File > New makes.
CCedtDoc * CCedtApp::CreateEmptyDocument()
{
	POSITION pos = GetFirstDocTemplatePosition(); if( ! pos ) return NULL;
	CDocTemplate * pTemplate = GetNextDocTemplate(pos);
	return (CCedtDoc *)pTemplate->OpenDocumentFile(NULL);
}

INT CCedtApp::RestoreRemoteBackups()
{
	CArray<CBackupEntry, CBackupEntry &> arrFound;
	FindBackupFiles(CCedtDoc::GetBackupDirectory(), arrFound);

	INT nRestored = 0;
	for( INT i = 0; i < arrFound.GetSize(); i++ ) {
		CBackupEntry & entry = arrFound[i];
		if( entry.info.nKind != BACKUP_KIND_REMOTE ) continue;

		// An account removed since: nowhere to save it back to. The backup stays on disk.
		INT nAccount = entry.info.nAccount;
		if( nAccount < 0 || nAccount >= MAX_FTP_ACCOUNT || ! m_clsFtpAccounts[nAccount].IsValid() ) continue;

		CCedtDoc * pDoc = (CCedtDoc *)GetRemoteDocument(nAccount, entry.info.szPath);
		if( ! pDoc ) {
			CString szLocalPath = GetRemoteCachePathName(nAccount, entry.info.szPath);
			if( ! TouchFile(szLocalPath) ) continue;

			CCedtDoc::m_nCurrentFtpAccount = nAccount;
			CCedtDoc::m_szCurrentRemotePathName = entry.info.szPath;
			pDoc = (CCedtDoc *)CWinApp::OpenDocumentFile(szLocalPath);
			CCedtDoc::m_nCurrentFtpAccount = -1;
			CCedtDoc::m_szCurrentRemotePathName = _T("");
		}

		if( pDoc && pDoc->RestoreFromBackup(entry) ) nRestored++;
	}

	return nRestored;
}

// Put the tabs back in the order session.xml recorded at exit, and activate the tab that was
// active. Without it the reopened files come in workspace order and every restored document
// trails after them. The file is read once and deleted: after a crash there is none, and a
// stale one would impose an old order.
void CCedtApp::ApplyBackupSession()
{
	CStringArray arrSession; INT nActive = -1;
	BOOL bRead = ReadBackupSession(CCedtDoc::GetSessionPathName(), arrSession, nActive);
	::DeleteFile(CCedtDoc::GetSessionPathName());
	if( ! bRead ) return;

	CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();
	INT nCount = pFrame->GetChildFrameCount();

	CArray<CMDIChildWnd *, CMDIChildWnd *> arrChild; CStringArray arrCurrent;
	for( INT nTab = 0; nTab < nCount; nTab++ ) {
		CMDIChildWnd * pChild = pFrame->GetChildFrame(nTab);
		CCedtDoc * pDoc = pChild ? (CCedtDoc *)pChild->GetActiveDocument() : NULL;
		arrChild.Add(pChild);
		arrCurrent.Add(pDoc ? pDoc->GetBackupKey() : CString());
	}

	CArray<INT, INT> arrOrder;
	OrderBySession(arrCurrent, arrSession, arrOrder);

	// Re-append every tab in the new order -- the same moves a tab drag makes.
	for( INT i = 0; i < arrOrder.GetSize(); i++ ) {
		CMDIChildWnd * pChild = arrChild[ arrOrder[i] ];
		if( ! pChild ) continue;
		pFrame->DeleteMDIFileTab(pChild);
		pFrame->InsertMDIFileTab(pChild);
		pFrame->UpdateMDIFileTab(pChild);
	}

	if( nActive >= 0 && nActive < arrSession.GetSize() ) {
		for( INT nTab = 0; nTab < nCount; nTab++ ) {
			if( arrCurrent[nTab] == arrSession[nActive] && arrChild[nTab] ) { arrChild[nTab]->MDIActivate(); break; }
		}
	}
}

INT CCedtApp::RestoreLocalBackups()
{
	CArray<CBackupEntry, CBackupEntry &> arrFound;
	FindBackupFiles(CCedtDoc::GetBackupDirectory(), arrFound);

	INT nRestored = 0;
	for( INT i = 0; i < arrFound.GetSize(); i++ ) {
		CBackupEntry & entry = arrFound[i];
		CCedtDoc * pDoc = NULL;

		if( entry.info.nKind == BACKUP_KIND_FILE ) {
			pDoc = (CCedtDoc *)GetDocumentFromLocalPathName(entry.info.szPath);
			if( ! pDoc ) {
				if( VerifyFilePath(entry.info.szPath) ) {
					pDoc = (CCedtDoc *)OpenDocumentFile(entry.info.szPath, 0, NULL);
				} else {
					// deleted while the editor was closed: back as a document with that path,
					// which saving recreates
					pDoc = CreateEmptyDocument();
					if( pDoc ) pDoc->SetPathName(entry.info.szPath, FALSE);
				}
			}
		} else if( entry.info.nKind == BACKUP_KIND_UNTITLED ) {
			pDoc = CreateEmptyDocument();
		} else continue;	// remote: restored before the workspace

		if( pDoc && pDoc->RestoreFromBackup(entry) ) nRestored++;
	}

	return nRestored;
}

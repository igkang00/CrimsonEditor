// cedtDocBackup.cpp : the document's side of hot-exit backups (see docs/hot-exit.md)
//
// The file format and the timing rules live in cedtBackup.cpp, where they are unit-tested.
// This file only connects them to the document: every edit arms the backup, the main
// frame's timer writes it when it falls due, and anything that makes the document clean
// again -- save, reload, undo back to the saved state, close -- deletes it.

#include "stdafx.h"
#include "cedtHeader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CString CCedtDoc::GetBackupDirectory()
{
	return CCedtApp::m_szAppDataDirectory + _T("\\backup");
}

CString CCedtDoc::GetSessionPathName()
{
	return GetBackupDirectory() + _T("\\session.xml");
}

// How session.xml names this document: the same key its backup gets, so a tab recorded at exit
// and the document restored from its backup match.
CString CCedtDoc::GetBackupKey()
{
	CBackupInfo info; FillBackupInfo(info);
	return ::GetBackupKey(info);
}

// Every edit in cedtDocEdit.cpp ends here with TRUE; open, new, reload and save end here with
// FALSE, and so does undoing back to the saved state. That makes this the one place to tell
// "has unsaved changes" from "matches the disk".
void CCedtDoc::SetModifiedFlag(BOOL bModified)
{
	CDocument::SetModifiedFlag(bModified);

	if( bModified ) m_clsBackupState.Changed(::GetTickCount64());
	else DiscardBackup();
}

BOOL CCedtDoc::WriteBackupIfDue(ULONGLONG nNow)
{
	if( ! IsModified() || ! m_clsBackupState.IsDue(nNow) ) return FALSE;
	return WriteBackup(nNow);
}

// A failed write leaves the state pending, so the next tick tries again; the previous
// backup, if any, is still on disk meanwhile.
BOOL CCedtDoc::WriteBackup(ULONGLONG nNow)
{
	CBackupInfo info; FillBackupInfo(info);
	CString szPath = GetBackupDirectory() + _T("\\") + GetBackupRelativePath(info);

	ULONGLONG nBytes = 0;
	if( ! WriteBackupFile(szPath, info, m_clsAnalyzedText, & nBytes) ) return FALSE;

	CString szStale = m_clsBackupState.Written(nNow, nBytes, szPath);
	if( ! szStale.IsEmpty() ) ::DeleteFile(szStale);	// renamed by Save As since the last backup
	return TRUE;
}

void CCedtDoc::DiscardBackup()
{
	CString szPath = m_clsBackupState.Discard();
	if( ! szPath.IsEmpty() ) ::DeleteFile(szPath);
}

// Replace the text with a backup's from an earlier session and mark the document modified.
// The document takes the backup file over (Adopt): a save deletes it, an edit rewrites it.
BOOL CCedtDoc::RestoreFromBackup(const CBackupEntry & entry)
{
	CBackupInfo info;
	if( ! ReadBackupFile(entry.szPath, info, m_clsAnalyzedText) ) {
		// The header was fine when scanned, so this is rare; put back what the disk holds.
		if( ! IsNewFileNotSaved() && VerifyFilePath(GetPathName()) ) OnReloadDocument(GetPathName(), m_nEncodingType);
		return FALSE;
	}

	EmptyUndoBuffer();
	EmptyRedoBuffer();
	m_bRecordingUndo = TRUE;
	m_nRecordingCount = 0;

	// No undo step leads back to what is on disk: without this, editing and undoing once
	// would count as "back to saved", clear the modified flag and delete the backup.
	m_nSavedUndoCount = -1;

	if( info.nEncodingType != ENCODING_TYPE_UNKNOWN ) m_nEncodingType = info.nEncodingType;
	if( info.nFileFormat != FILE_FORMAT_UNKNOWN ) m_nFileFormat = info.nFileFormat;

	if( info.nKind == BACKUP_KIND_FILE || info.nKind == BACKUP_KIND_REMOTE ) {
		// The file's status when the edits began, so a file changed on disk since then is
		// caught by the ordinary "modified outside" check.
		m_clsFileStatus.m_mtime = CTime(info.nModifiedTime);
		m_clsFileStatus.m_size = info.nFileSize;
	}

	if( info.nKind == BACKUP_KIND_UNTITLED ) {
		m_szUntitledBackupId = info.szId;
		if( ! info.szTitle.IsEmpty() ) SetTitle(info.szTitle);
	}

	// A file deleted while the editor was closed comes back as a new document with its path;
	// give it the syntax colouring opening it would have.
	if( info.nKind == BACKUP_KIND_FILE && m_szLangSpecFile.IsEmpty() && m_clsAnalyzedText.GetCount() ) {
		if( DetectSyntaxType(info.szPath, m_clsAnalyzedText.GetHead()) ) LoadSyntaxInformation();
	}

	AnalyzeText();

	// The bookmarks the backup carried belong to its text; they replace any bookmarks.xml put
	// on the file as it is on disk.
	SetBookmarkLines(info.bookmarks);
	m_bBookmarksReady = TRUE;

	ReinitializeAllViews();
	UpdateAllViews(NULL);

	CDocument::SetModifiedFlag(TRUE);	// not ours: that would schedule a rewrite of a current backup
	m_clsBackupState.Adopt(entry.szPath);

	GoToLineNumber(info.nLineNum);
	return TRUE;
}

void CCedtDoc::FillBackupInfo(CBackupInfo & info)
{
	if( IsRemoteFile() ) {
		info.nKind = BACKUP_KIND_REMOTE;
		info.nAccount = m_nFtpAccount;
		info.szPath = m_szRemotePathName;
	} else if( IsNewFileNotSaved() ) {
		info.nKind = BACKUP_KIND_UNTITLED;
		if( m_szUntitledBackupId.IsEmpty() ) {
			// A GUID, so an untitled document restored from an earlier session can never
			// collide with one created in this one.
			GUID guid; WCHAR szGuid[64];
			if( SUCCEEDED(::CoCreateGuid(& guid)) && ::StringFromGUID2(guid, szGuid, 64) ) {
				m_szUntitledBackupId = szGuid; m_szUntitledBackupId.Trim(_T("{}"));
			} else {
				m_szUntitledBackupId.Format(_T("%I64u-%u"), ::GetTickCount64(), ::GetCurrentProcessId());
			}
		}
		info.szId = m_szUntitledBackupId;
		info.szTitle = GetTitle();
	} else {
		info.nKind = BACKUP_KIND_FILE;
		info.szPath = GetPathName();
	}

	info.nEncodingType = m_nEncodingType;
	info.nFileFormat = m_nFileFormat;
	info.nModifiedTime = m_clsFileStatus.m_mtime.GetTime();
	info.nFileSize = m_clsFileStatus.m_size;

	CCedtView * pView = GetFirstView();
	info.nLineNum = pView ? pView->GetCurrentLineNumber() : 1;

	// the unsaved text's bookmarks travel with it (docs/bookmarks.md)
	info.bookmarks = GetBookmarkLines();
}

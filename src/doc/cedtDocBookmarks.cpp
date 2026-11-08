// cedtDocBookmarks.cpp : the document's side of persistent bookmarks (see docs/bookmarks.md)
//
// The store, the line hash and re-finding live in cedtBookmarks.cpp, where they are unit-tested.
// This file connects them to the document: a file's bookmarks come back when it opens and
// survive a reload, and are recorded when it is saved, reloaded or closed.

#include "stdafx.h"
#include "cedtHeader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CBookmarkStore CCedtDoc::m_clsBookmarkStore;

CString CCedtDoc::GetBookmarkStorePathName()
{
	return CCedtApp::m_szAppDataDirectory + _T("\\bookmarks.xml");
}

void CCedtDoc::LoadBookmarkStore()
{
	m_clsBookmarkStore.Load(GetBookmarkStorePathName());
}

// Every bookmarked line: its index, and the hash that finds it again once the file changed.
std::vector<CBookmark> CCedtDoc::CollectBookmarks()
{
	std::vector<CBookmark> marks;
	INT nLine = 0; POSITION pos = m_clsAnalyzedText.GetHeadPosition();
	while( pos ) {
		CAnalyzedString & rLine = m_clsAnalyzedText.GetNext(pos);
		if( rLine.m_usLineInfo & LI_HAVEBOOKMARK ) {
			CBookmark mark; mark.nLine = nLine; mark.nHash = HashBookmarkLine(rLine);
			marks.push_back(mark);
		}
		nLine++;
	}
	return marks;
}

std::vector<INT> CCedtDoc::GetBookmarkLines()
{
	std::vector<INT> lines;
	INT nLine = 0; POSITION pos = m_clsAnalyzedText.GetHeadPosition();
	while( pos ) {
		if( m_clsAnalyzedText.GetNext(pos).m_usLineInfo & LI_HAVEBOOKMARK ) lines.push_back(nLine);
		nLine++;
	}
	return lines;
}

// Replace the bookmarks with exactly these lines. The caller repaints.
void CCedtDoc::SetBookmarkLines(const std::vector<INT> & lines)
{
	POSITION pos = m_clsAnalyzedText.GetHeadPosition();
	while( pos ) m_clsAnalyzedText.GetNext(pos).m_usLineInfo &= ~LI_HAVEBOOKMARK;

	INT nCount = GetLineCount();
	for( INT nLine : lines ) {
		if( nLine >= 0 && nLine < nCount ) GetLineFromIdxY(nLine).m_usLineInfo |= LI_HAVEBOOKMARK;
	}
}

std::vector<ULONGLONG> CCedtDoc::HashAllLines()
{
	std::vector<ULONGLONG> hashes; hashes.reserve(GetLineCount());
	POSITION pos = m_clsAnalyzedText.GetHeadPosition();
	while( pos ) hashes.push_back(HashBookmarkLine(m_clsAnalyzedText.GetNext(pos)));
	return hashes;
}

// The key bookmarks.xml files this document under. Built from the path passed in rather than from
// GetPathName(): MFC sets the document's path only AFTER OnOpenDocument returns, so during the
// open the document still looks untitled. An FTP document is keyed by its remote path, which
// OnOpenDocument has already set.
CString CCedtDoc::GetBookmarkKey(LPCTSTR lpszPathName)
{
	if( IsRemoteFile() ) return GetBackupKey();

	CBackupInfo info; info.nKind = BACKUP_KIND_FILE; info.szPath = lpszPathName;
	return ::GetBackupKey(info);
}

// Put back the bookmarks bookmarks.xml has for the file just opened from lpszPathName, on the
// same lines if the file is as it was when they were recorded, re-found by content if it is not.
void CCedtDoc::RestoreBookmarks(LPCTSTR lpszPathName)
{
	CBookmarkEntry entry;
	if( m_clsBookmarkStore.Lookup(GetBookmarkKey(lpszPathName), entry) ) {
		BOOL bUnchanged = ! IsRemoteFile() && entry.nModifiedTime != 0
			&& entry.nModifiedTime == m_clsFileStatus.m_mtime.GetTime()
			&& entry.nFileSize == (ULONGLONG)m_clsFileStatus.m_size;
		SetBookmarkLines(RefindBookmarks(entry.marks, HashAllLines(), bUnchanged));
	}

	m_bBookmarksReady = TRUE;
}

// Write this file's bookmarks to bookmarks.xml -- or remove its entry, if it had one and has no
// bookmarks left.
void CCedtDoc::RecordBookmarks()
{
	// Only a document that really loaded: one closing after a failed open must not wipe the
	// entry of a file that was merely missing for a moment.
	if( ! m_bBookmarksReady || IsNewFileNotSaved() ) return;

	CBookmarkEntry entry; entry.szKey = GetBookmarkKey(GetPathName());
	entry.marks = CollectBookmarks();

	CBookmarkEntry previous;
	if( entry.marks.empty() && ! m_clsBookmarkStore.Lookup(entry.szKey, previous) ) return;	// nothing to write

	if( IsModified() || IsRemoteFile() ) {
		// The lines are those of unsaved text, or of the server's copy: no file status speaks
		// for them, so the next open always re-finds them by content.
		entry.nModifiedTime = 0; entry.nFileSize = 0;
	} else {
		entry.nModifiedTime = m_clsFileStatus.m_mtime.GetTime();
		entry.nFileSize = m_clsFileStatus.m_size;
	}

	m_clsBookmarkStore.Record(entry);
	m_clsBookmarkStore.Save(GetBookmarkStorePathName());
}

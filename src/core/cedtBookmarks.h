// cedtBookmarks.h : persistent bookmarks (see docs/bookmarks.md)
//
// A bookmark is kept as a line index plus a hash of the line's text: the index is enough while
// the file is unchanged, the hash finds the line again once it has changed. Kept free of
// CCedtDoc so it can be tested on its own.

#if !defined(_CEDT_BOOKMARKS_H_)
#define _CEDT_BOOKMARKS_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <vector>

#define BOOKMARK_FORMAT_VERSION		1
#define BOOKMARK_MAX_FILES			100		// most recently used files kept in bookmarks.xml

class CBookmark
{
public:
	INT       nLine;		// 0-based line index
	ULONGLONG nHash;		// HashBookmarkLine of the line's text
};

// Hash of a line's text with its leading and trailing blanks (spaces, tabs) trimmed, so that
// re-indenting a line does not lose its bookmark. 64-bit FNV-1a: stable across runs and builds.
ULONGLONG HashBookmarkLine(LPCTSTR lpszLine);

// Where recorded bookmarks belong in a document whose lines hash to arrLineHashes. Unchanged:
// each goes back on its index (dropped if past the end). Changed: each goes to the nearest line
// with the same hash, the earlier one on a tie, or is dropped. Sorted, no duplicates.
std::vector<INT> RefindBookmarks(const std::vector<CBookmark> & marks, const std::vector<ULONGLONG> & arrLineHashes, BOOL bUnchanged);

// One file's bookmarks, as bookmarks.xml keeps them.
class CBookmarkEntry
{
public:
	CString   szKey;			// document key: file:<lower-cased path> or remote:<account>:<path>
	__int64   nModifiedTime;	// the file's last-write time when recorded, CTime seconds
	ULONGLONG nFileSize;		// the file's size when recorded
	std::vector<CBookmark> marks;

	CBookmarkEntry() { nModifiedTime = 0; nFileSize = 0; }
};

// bookmarks.xml: the files with bookmarks, most recently used first, at most BOOKMARK_MAX_FILES.
class CBookmarkStore
{
public:
	BOOL Load(LPCTSTR lpszPathName);		// FALSE (and empty) when missing or not ours
	BOOL Save(LPCTSTR lpszPathName);		// written to a .tmp and renamed into place

	BOOL Lookup(LPCTSTR lpszKey, CBookmarkEntry & entry) const;

	// Replace the file's entry and make it the most recent; an entry without bookmarks is removed.
	// Past BOOKMARK_MAX_FILES the least recently used goes.
	void Record(const CBookmarkEntry & entry);
	void Remove(LPCTSTR lpszKey);

	INT GetCount() const { return (INT)m_arrEntries.size(); }
	const CBookmarkEntry & GetAt(INT nIndex) const { return m_arrEntries[nIndex]; }

protected:
	std::vector<CBookmarkEntry> m_arrEntries;
};

#endif // !defined(_CEDT_BOOKMARKS_H_)

// cedtBookmarks.cpp : persistent bookmarks (see docs/bookmarks.md)

#include "stdafx.h"
#include "cedtBackup.h"
#include "cedtBookmarks.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define BOOKMARK_TAG		_T("<cedtbookmarks")
#define BOOKMARK_FILE_TAG	_T("<file")


ULONGLONG HashBookmarkLine(LPCTSTR lpszLine)
{
	// trim blanks (spaces and tabs) at both ends: indentation is not content
	LPCTSTR pBeg = lpszLine;
	while( * pBeg == _T(' ') || * pBeg == _T('\t') ) pBeg++;
	LPCTSTR pEnd = pBeg + _tcslen(pBeg);
	while( pEnd > pBeg && ( pEnd[-1] == _T(' ') || pEnd[-1] == _T('\t') ) ) pEnd--;

	// 64-bit FNV-1a over the UTF-16 code units
	ULONGLONG h = 14695981039346656037ULL;
	for( LPCTSTR p = pBeg; p < pEnd; p++ ) {
		WORD w = (WORD)* p;
		h ^= (BYTE)(w & 0xFF);	h *= 1099511628211ULL;
		h ^= (BYTE)(w >> 8);	h *= 1099511628211ULL;
	}
	return h;
}

std::vector<INT> RefindBookmarks(const std::vector<CBookmark> & marks, const std::vector<ULONGLONG> & arrLineHashes, BOOL bUnchanged)
{
	INT nLines = (INT)arrLineHashes.size();
	std::vector<INT> found;

	for( const CBookmark & mark : marks ) {
		if( bUnchanged ) {
			if( mark.nLine >= 0 && mark.nLine < nLines ) found.push_back(mark.nLine);
			continue;
		}

		// the nearest line with the same content; on an even tie the earlier one
		INT nBest = -1, nBestDistance = INT_MAX;
		for( INT i = 0; i < nLines; i++ ) {
			if( arrLineHashes[i] != mark.nHash ) continue;
			INT nDistance = abs(i - mark.nLine);
			if( nDistance < nBestDistance ) { nBest = i; nBestDistance = nDistance; }
		}
		if( nBest >= 0 ) found.push_back(nBest);
	}

	std::sort(found.begin(), found.end());
	found.erase(std::unique(found.begin(), found.end()), found.end());
	return found;
}


BOOL CBookmarkStore::Load(LPCTSTR lpszPathName)
{
	m_arrEntries.clear();

	CString szText;
	if( ! ReadUtf8TextFile(lpszPathName, szText) ) return FALSE;

	CMapStringToString mapAttr; CString szValue;
	INT nStart = 0, nLine = 0;
	while( nStart < szText.GetLength() ) {
		INT nEnd = szText.Find(_T('\n'), nStart); if( nEnd < 0 ) nEnd = szText.GetLength();
		CString szLine = szText.Mid(nStart, nEnd - nStart); nStart = nEnd + 1;
		if( szLine.Trim().IsEmpty() ) continue;

		if( nLine++ == 0 ) {	// the header: right tag, a version this build reads
			if( ! ParseTagLine(szLine, BOOKMARK_TAG, mapAttr) ) return FALSE;
			INT nVersion = mapAttr.Lookup(_T("version"), szValue) ? _ttoi(szValue) : 0;
			if( nVersion < 1 || nVersion > BOOKMARK_FORMAT_VERSION ) return FALSE;
			continue;
		}

		if( ! ParseTagLine(szLine, BOOKMARK_FILE_TAG, mapAttr) || ! mapAttr.Lookup(_T("key"), szValue) ) continue;
		CBookmarkEntry entry; entry.szKey = szValue;
		if( mapAttr.Lookup(_T("mtime"), szValue) ) entry.nModifiedTime = _ttoi64(szValue);
		if( mapAttr.Lookup(_T("size"), szValue) ) entry.nFileSize = _tcstoui64(szValue, NULL, 10);

		// marks="<line>:<hash>,<line>:<hash>,..."
		if( mapAttr.Lookup(_T("marks"), szValue) ) {
			INT nPos = 0; CString szToken = szValue.Tokenize(_T(","), nPos);
			while( ! szToken.IsEmpty() ) {
				INT nColon = szToken.Find(_T(':'));
				if( nColon > 0 ) {
					CBookmark mark;
					mark.nLine = _ttoi(szToken.Left(nColon));
					mark.nHash = _tcstoui64(szToken.Mid(nColon + 1), NULL, 16);
					entry.marks.push_back(mark);
				}
				szToken = szValue.Tokenize(_T(","), nPos);
			}
		}

		if( ! entry.marks.empty() && (INT)m_arrEntries.size() < BOOKMARK_MAX_FILES ) m_arrEntries.push_back(entry);
	}

	if( nLine == 0 ) return FALSE;	// empty file: not ours either
	return TRUE;
}

BOOL CBookmarkStore::Save(LPCTSTR lpszPathName)
{
	CString szText, szAttr;
	szText.Format(_T("%s version=\"%d\" />\n"), BOOKMARK_TAG, BOOKMARK_FORMAT_VERSION);

	for( const CBookmarkEntry & entry : m_arrEntries ) {
		szText += CString(BOOKMARK_FILE_TAG) + _T(" key=\"") + EscapeTagValue(entry.szKey) + _T("\"");
		szAttr.Format(_T(" mtime=\"%I64d\" size=\"%I64u\" marks=\""), entry.nModifiedTime, entry.nFileSize);
		szText += szAttr;
		for( size_t i = 0; i < entry.marks.size(); i++ ) {
			szAttr.Format(i ? _T(",%d:%016I64x") : _T("%d:%016I64x"), entry.marks[i].nLine, entry.marks[i].nHash);
			szText += szAttr;
		}
		szText += _T("\" />\n");
	}

	return WriteUtf8TextFile(lpszPathName, szText);
}

BOOL CBookmarkStore::Lookup(LPCTSTR lpszKey, CBookmarkEntry & entry) const
{
	for( const CBookmarkEntry & e : m_arrEntries ) {
		if( e.szKey == lpszKey ) { entry = e; return TRUE; }
	}
	return FALSE;
}

void CBookmarkStore::Record(const CBookmarkEntry & entry)
{
	Remove(entry.szKey);
	if( entry.marks.empty() ) return;		// no bookmarks left: no entry

	m_arrEntries.insert(m_arrEntries.begin(), entry);	// most recent first
	if( (INT)m_arrEntries.size() > BOOKMARK_MAX_FILES ) m_arrEntries.resize(BOOKMARK_MAX_FILES);
}

void CBookmarkStore::Remove(LPCTSTR lpszKey)
{
	for( size_t i = 0; i < m_arrEntries.size(); i++ ) {
		if( m_arrEntries[i].szKey == lpszKey ) { m_arrEntries.erase(m_arrEntries.begin() + i); return; }
	}
}

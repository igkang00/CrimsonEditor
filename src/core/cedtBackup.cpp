// cedtBackup.cpp : hot-exit backup files (see docs/hot-exit.md)

#include "stdafx.h"
#include "cedtElement.h"
#include "cedtBackup.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define BACKUP_TAG					_T("<cedtbackup")
#define BACKUP_HEADER_MAX_BYTES		(64 * 1024)		// a header longer than this is not ours

static LPCTSTR _KindName(INT nKind)
{
	switch( nKind ) {
	case BACKUP_KIND_FILE:		return _T("file");
	case BACKUP_KIND_REMOTE:	return _T("remote");
	case BACKUP_KIND_UNTITLED:	return _T("untitled");
	default:					return NULL;
	}
}

static INT _KindFromName(LPCTSTR lpszName)
{
	if( ! _tcscmp(lpszName, _T("file")) ) return BACKUP_KIND_FILE;
	if( ! _tcscmp(lpszName, _T("remote")) ) return BACKUP_KIND_REMOTE;
	if( ! _tcscmp(lpszName, _T("untitled")) ) return BACKUP_KIND_UNTITLED;
	return 0;
}

// Attribute values are escaped so a quote ends nothing early and a line break cannot split
// the header -- remote paths and untitled titles may hold either.
static CString _Escape(LPCTSTR lpszValue)
{
	CString szOut;
	for( LPCTSTR p = lpszValue; * p; p++ ) {
		switch( * p ) {
		case _T('&'):	szOut += _T("&amp;");	break;
		case _T('"'):	szOut += _T("&quot;");	break;
		case _T('<'):	szOut += _T("&lt;");	break;
		case _T('>'):	szOut += _T("&gt;");	break;
		case _T('\r'):	szOut += _T("&#13;");	break;
		case _T('\n'):	szOut += _T("&#10;");	break;
		default:		szOut += * p;			break;
		}
	}
	return szOut;
}

static BOOL _Unescape(const CString & szValue, CString & szOut)
{
	szOut.Empty();
	for( INT i = 0; i < szValue.GetLength(); i++ ) {
		TCHAR ch = szValue[i];
		if( ch != _T('&') ) { szOut += ch; continue; }

		INT nEnd = szValue.Find(_T(';'), i);
		if( nEnd < 0 ) return FALSE;
		CString szEntity = szValue.Mid(i + 1, nEnd - i - 1);

		if( szEntity == _T("amp") ) szOut += _T('&');
		else if( szEntity == _T("quot") ) szOut += _T('"');
		else if( szEntity == _T("lt") ) szOut += _T('<');
		else if( szEntity == _T("gt") ) szOut += _T('>');
		else if( szEntity.GetLength() > 1 && szEntity[0] == _T('#') ) szOut += (TCHAR)_ttoi(szEntity.Mid(1));
		else return FALSE;

		i = nEnd;
	}
	return TRUE;
}

static std::string _ToUtf8(LPCWSTR lpsz, INT nLength)
{
	std::string s;
	if( nLength <= 0 ) return s;
	INT nBytes = WideCharToMultiByte(CP_UTF8, 0, lpsz, nLength, NULL, 0, NULL, NULL);
	if( nBytes <= 0 ) return s;
	s.resize((size_t)nBytes);
	WideCharToMultiByte(CP_UTF8, 0, lpsz, nLength, & s[0], nBytes, NULL, NULL);
	return s;
}

// 64-bit FNV-1a over the UTF-16 code units: stable across runs and builds, which a
// pointer- or seed-based hash would not be.
static CString _HashKey(const CString & szKey)
{
	ULONGLONG h = 14695981039346656037ULL;
	for( INT i = 0; i < szKey.GetLength(); i++ ) {
		WORD w = (WORD)szKey[i];
		h ^= (BYTE)(w & 0xFF);	h *= 1099511628211ULL;
		h ^= (BYTE)(w >> 8);	h *= 1099511628211ULL;
	}
	CString szHash; szHash.Format(_T("%016I64x"), h);
	return szHash;
}


CBackupInfo::CBackupInfo()
{
	nKind = 0; nAccount = 0;
	nEncodingType = 0; nFileFormat = 0;
	nModifiedTime = 0; nFileSize = 0; nLineNum = 1;
}

CString FormatBackupHeader(const CBackupInfo & info)
{
	CString szHeader, szAttr;
	LPCTSTR lpszKind = _KindName(info.nKind);

	szHeader.Format(_T("%s version=\"%d\" kind=\"%s\""), BACKUP_TAG, BACKUP_FORMAT_VERSION, lpszKind ? lpszKind : _T(""));

	if( info.nKind == BACKUP_KIND_REMOTE ) {
		szAttr.Format(_T(" account=\"%d\""), info.nAccount); szHeader += szAttr;
	}
	if( info.nKind == BACKUP_KIND_FILE || info.nKind == BACKUP_KIND_REMOTE ) {
		szHeader += _T(" path=\"") + _Escape(info.szPath) + _T("\"");
	}
	if( info.nKind == BACKUP_KIND_UNTITLED ) {
		szHeader += _T(" id=\"") + _Escape(info.szId) + _T("\"");
		szHeader += _T(" title=\"") + _Escape(info.szTitle) + _T("\"");
	}

	szAttr.Format(_T(" encoding=\"%d\" format=\"%d\" mtime=\"%I64d\" size=\"%I64u\" linenum=\"%d\" />"),
		info.nEncodingType, info.nFileFormat, info.nModifiedTime, info.nFileSize, info.nLineNum);
	szHeader += szAttr;

	return szHeader;
}

BOOL ParseBackupHeader(LPCTSTR lpszLine, CBackupInfo & info)
{
	CString szLine = lpszLine; szLine.TrimRight();

	INT nTagLength = (INT)_tcslen(BACKUP_TAG);
	if( szLine.Left(nTagLength) != BACKUP_TAG ) return FALSE;
	if( szLine.GetLength() <= nTagLength || szLine[nTagLength] != _T(' ') ) return FALSE;
	if( szLine.Right(2) != _T("/>") ) return FALSE;

	CString szBody = szLine.Mid(nTagLength, szLine.GetLength() - nTagLength - 2);
	CBackupInfo result; INT nVersion = 0;

	INT i = 0, nLength = szBody.GetLength();
	for( ;; ) {
		while( i < nLength && szBody[i] == _T(' ') ) i++;
		if( i >= nLength ) break;

		INT nNameStart = i;
		while( i < nLength && _istalpha(szBody[i]) ) i++;
		CString szName = szBody.Mid(nNameStart, i - nNameStart);
		if( szName.IsEmpty() ) return FALSE;

		if( i + 1 >= nLength || szBody[i] != _T('=') || szBody[i+1] != _T('"') ) return FALSE;
		i += 2;

		INT nValueEnd = szBody.Find(_T('"'), i);
		if( nValueEnd < 0 ) return FALSE;
		CString szValue; if( ! _Unescape(szBody.Mid(i, nValueEnd - i), szValue) ) return FALSE;
		i = nValueEnd + 1;

		if( szName == _T("version") ) nVersion = _ttoi(szValue);
		else if( szName == _T("kind") ) result.nKind = _KindFromName(szValue);
		else if( szName == _T("path") ) result.szPath = szValue;
		else if( szName == _T("account") ) result.nAccount = _ttoi(szValue);
		else if( szName == _T("id") ) result.szId = szValue;
		else if( szName == _T("title") ) result.szTitle = szValue;
		else if( szName == _T("encoding") ) result.nEncodingType = _ttoi(szValue);
		else if( szName == _T("format") ) result.nFileFormat = _ttoi(szValue);
		else if( szName == _T("mtime") ) result.nModifiedTime = _ttoi64(szValue);
		else if( szName == _T("size") ) result.nFileSize = _tcstoui64(szValue, NULL, 10);
		else if( szName == _T("linenum") ) result.nLineNum = _ttoi(szValue);
		// anything else is from a later build and is ignored
	}

	if( nVersion < 1 || nVersion > BACKUP_FORMAT_VERSION ) return FALSE;
	if( result.nKind == BACKUP_KIND_FILE || result.nKind == BACKUP_KIND_REMOTE ) {
		if( result.szPath.IsEmpty() ) return FALSE;
	} else if( result.nKind == BACKUP_KIND_UNTITLED ) {
		if( result.szId.IsEmpty() ) return FALSE;
	} else return FALSE;

	info = result;
	return TRUE;
}

// Plain Win32 rather than CFile: a failure here is a return value, never an exception for
// someone to report in a message box -- this runs from a timer.
BOOL WriteBackupFile(LPCTSTR lpszPathName, const CBackupInfo & info, CAnalyzedText & rText, ULONGLONG * pBytesWritten)
{
	CString szTempPath = CString(lpszPathName) + _T(".tmp");

	HANDLE hFile = CreateFile(szTempPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile == INVALID_HANDLE_VALUE ) return FALSE;

	const size_t nFlushAt = 1 << 16;
	std::string buf; buf.reserve(nFlushAt + 4096);
	ULONGLONG nTotal = 0; BOOL bOK = TRUE;

	auto flush = [&]() {
		if( buf.empty() || ! bOK ) return;
		DWORD dwWritten = 0;
		if( ! ::WriteFile(hFile, buf.data(), (DWORD)buf.size(), & dwWritten, NULL) || dwWritten != buf.size() ) bOK = FALSE;
		nTotal += buf.size(); buf.clear();
	};

	CString szHeader = FormatBackupHeader(info);
	buf += _ToUtf8(szHeader, szHeader.GetLength()); buf += '\n';

	POSITION pos = rText.GetHeadPosition();
	while( pos && bOK ) {
		CAnalyzedString & rLine = rText.GetNext(pos);
		buf += _ToUtf8((LPCWSTR)rLine, rLine.GetLength());
		if( pos ) buf += '\n';
		if( buf.size() >= nFlushAt ) flush();
	}
	flush();

	CloseHandle(hFile);

	if( bOK ) bOK = MoveFileEx(szTempPath, lpszPathName, MOVEFILE_REPLACE_EXISTING);
	if( ! bOK ) { DeleteFile(szTempPath); return FALSE; }

	if( pBytesWritten ) * pBytesWritten = nTotal;
	return TRUE;
}

BOOL ReadBackupHeader(LPCTSTR lpszPathName, CBackupInfo & info)
{
	HANDLE hFile = CreateFile(lpszPathName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile == INVALID_HANDLE_VALUE ) return FALSE;

	std::string line; CHAR chunk[4096]; DWORD dwRead = 0; BOOL bFound = FALSE;
	while( ! bFound && line.size() < BACKUP_HEADER_MAX_BYTES && ::ReadFile(hFile, chunk, sizeof(chunk), & dwRead, NULL) && dwRead ) {
		for( DWORD k = 0; k < dwRead; k++ ) {
			if( chunk[k] == '\n' ) { bFound = TRUE; break; }
			line += chunk[k];
		}
	}
	CloseHandle(hFile);

	if( ! bFound ) return FALSE;

	INT nChars = MultiByteToWideChar(CP_UTF8, 0, line.data(), (INT)line.size(), NULL, 0);
	CString szLine;
	if( nChars > 0 ) {
		MultiByteToWideChar(CP_UTF8, 0, line.data(), (INT)line.size(), szLine.GetBuffer(nChars), nChars);
		szLine.ReleaseBuffer(nChars);
	}
	return ParseBackupHeader(szLine, info);
}

// The body is read by the ordinary UTF-8 loader -- the header is just its first line, and
// is dropped afterwards. The header is checked first, so a file that is not a backup never
// reaches the loader (whose own errors are reported in a message box).
BOOL ReadBackupFile(LPCTSTR lpszPathName, CBackupInfo & info, CAnalyzedText & rText)
{
	CBackupInfo header;
	if( ! ReadBackupHeader(lpszPathName, header) ) return FALSE;

	if( ! rText.FileLoad(lpszPathName, ENCODING_TYPE_UTF8_XBOM, FILE_FORMAT_UNIX) ) return FALSE;
	if( rText.GetCount() < 2 ) { rText.RemoveAll(); return FALSE; }	// a header is always followed by a body line
	rText.RemoveLines(0, 1);

	info = header;
	return TRUE;
}

CString GetBackupRelativePath(const CBackupInfo & info)
{
	switch( info.nKind ) {
	case BACKUP_KIND_FILE: {
		CString szKey = info.szPath;
		szKey.Replace(_T('/'), _T('\\')); szKey.MakeLower();
		return _T("file\\") + _HashKey(szKey) + _T(".bak"); }
	case BACKUP_KIND_REMOTE: {
		CString szKey; szKey.Format(_T("%d:%s"), info.nAccount, (LPCTSTR)info.szPath);
		return _T("remote\\") + _HashKey(szKey) + _T(".bak"); }
	case BACKUP_KIND_UNTITLED:
		return _T("untitled\\") + info.szId + _T(".bak");
	default:
		return _T("");
	}
}

BOOL IsBackupDue(ULONGLONG nNow, ULONGLONG nLastChange, ULONGLONG nLastBackup, ULONGLONG nLastBackupBytes, BOOL bChangedSinceBackup)
{
	if( ! bChangedSinceBackup ) return FALSE;
	if( nNow < nLastChange + BACKUP_IDLE_DELAY ) return FALSE;		// still typing

	// The size is known only from a previous backup, so a first backup always counts as small.
	BOOL bLarge = ( nLastBackup != 0 && nLastBackupBytes > BACKUP_LARGE_BYTES );
	if( bLarge && nNow < nLastBackup + BACKUP_LARGE_INTERVAL ) return FALSE;

	return TRUE;
}


void FindBackupFiles(LPCTSTR lpszBackupDir, CArray<CBackupEntry, CBackupEntry &> & arrFound)
{
	arrFound.RemoveAll();

	LPCTSTR kinds[] = { _T("file"), _T("remote"), _T("untitled") };
	for( INT k = 0; k < (INT)(sizeof(kinds) / sizeof(kinds[0])); k++ ) {
		CString szFolder = CString(lpszBackupDir) + _T("\\") + kinds[k];

		WIN32_FIND_DATA fd; HANDLE hFind = FindFirstFile(szFolder + _T("\\*"), & fd);
		if( hFind == INVALID_HANDLE_VALUE ) continue;

		do {
			if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) continue;

			// Match the extension exactly: a wildcard would also let 8.3 short names through.
			CString szName = fd.cFileName;
			if( szName.GetLength() < 5 || szName.Right(4).CompareNoCase(_T(".bak")) ) continue;

			CBackupEntry entry;
			entry.szPath = szFolder + _T("\\") + szName;
			entry.nWriteTime = ((ULONGLONG)fd.ftLastWriteTime.dwHighDateTime << 32) | fd.ftLastWriteTime.dwLowDateTime;
			if( ! ReadBackupHeader(entry.szPath, entry.info) ) continue;

			// insert in write-time order, oldest first; ties keep the order found
			INT nAt = (INT)arrFound.GetSize();
			while( nAt > 0 && arrFound[nAt-1].nWriteTime > entry.nWriteTime ) nAt--;
			arrFound.InsertAt(nAt, entry);
		} while( FindNextFile(hFind, & fd) );

		FindClose(hFind);
	}
}


CBackupState::CBackupState()
{
	m_nLastChange = m_nLastBackup = m_nLastBackupBytes = 0;
	m_bPending = FALSE;
}

void CBackupState::Changed(ULONGLONG nNow)
{
	m_nLastChange = nNow;
	m_bPending = TRUE;
}

BOOL CBackupState::IsDue(ULONGLONG nNow) const
{
	return IsBackupDue(nNow, m_nLastChange, m_nLastBackup, m_nLastBackupBytes, m_bPending);
}

CString CBackupState::Written(ULONGLONG nNow, ULONGLONG nBytes, LPCTSTR lpszPath)
{
	CString szStale;
	if( ! m_szPath.IsEmpty() && m_szPath.CompareNoCase(lpszPath) ) szStale = m_szPath;

	m_szPath = lpszPath;
	m_nLastBackup = nNow;
	m_nLastBackupBytes = nBytes;
	m_bPending = ( m_nLastChange > nNow );		// an edit stamped after the write began is still owed

	return szStale;
}

CString CBackupState::Discard()
{
	CString szPath = m_szPath;
	m_szPath.Empty();
	m_bPending = FALSE;
	return szPath;
}

void CBackupState::Adopt(LPCTSTR lpszPath)
{
	m_szPath = lpszPath;
	m_bPending = FALSE;
}

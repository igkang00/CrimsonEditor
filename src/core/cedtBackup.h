// cedtBackup.h : hot-exit backup files (see docs/hot-exit.md)
//
// One file per unsaved document: a single header line describing the document, then its
// text in UTF-8. Kept free of CCedtDoc so the format can be tested on its own; the document
// layer fills a CBackupInfo and hands over its CAnalyzedText.

#if !defined(_CEDT_BACKUP_H_)
#define _CEDT_BACKUP_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define BACKUP_FORMAT_VERSION			1

#define BACKUP_KIND_FILE				1	// a local file
#define BACKUP_KIND_REMOTE				2	// an FTP document
#define BACKUP_KIND_UNTITLED			3	// a new document that was never saved

// Timing (milliseconds, bytes). A backup waits for typing to pause; a document whose last
// backup was larger than BACKUP_LARGE_BYTES is also backed up no more often than every
// BACKUP_LARGE_INTERVAL. About 1.4 ms per MB is spent writing one.
#define BACKUP_IDLE_DELAY				1000
#define BACKUP_LARGE_BYTES				(1024 * 1024)
#define BACKUP_LARGE_INTERVAL			30000

class CAnalyzedText;

class CBackupInfo
{
public:
	INT       nKind;			// BACKUP_KIND_*
	CString   szPath;			// local path, or remote path for BACKUP_KIND_REMOTE
	INT       nAccount;			// FTP account number (remote only)
	CString   szTitle;			// tab title (untitled only)
	CString   szId;				// stable id, names the backup file (untitled only)
	INT       nEncodingType;	// ENCODING_TYPE_* the document saves with
	INT       nFileFormat;		// FILE_FORMAT_* the document saves with
	__int64   nModifiedTime;	// original file's last-write time, CTime seconds
	ULONGLONG nFileSize;		// original file's size in bytes
	INT       nLineNum;			// caret line, 1-based

	CBackupInfo();
};

// The header line, without a line break. Attribute values are escaped, so any path or
// title fits on one line.
CString FormatBackupHeader(const CBackupInfo & info);

// FALSE when the line is not a backup header this build understands: another tag, no kind,
// malformed, or a newer format version.
BOOL ParseBackupHeader(LPCTSTR lpszLine, CBackupInfo & info);

// Writes header and text to a temporary file beside lpszPathName, then renames it into place,
// so a failed write leaves the previous backup intact. Never shows UI; failure is the return
// value. pBytesWritten receives the file size on success.
BOOL WriteBackupFile(LPCTSTR lpszPathName, const CBackupInfo & info, CAnalyzedText & rText, ULONGLONG * pBytesWritten = NULL);

BOOL ReadBackupHeader(LPCTSTR lpszPathName, CBackupInfo & info);
BOOL ReadBackupFile(LPCTSTR lpszPathName, CBackupInfo & info, CAnalyzedText & rText);

// Where the backup lives under the backup directory: file\<hash>.bak, remote\<hash>.bak, or
// untitled\<id>.bak. Local paths are compared case- and slash-insensitively; remote paths
// are case-sensitive and per account.
CString GetBackupRelativePath(const CBackupInfo & info);

// Whether a document's backup should be written now. Times in milliseconds; nLastBackup == 0
// means never. nLastBackupBytes is the size of the previous backup (0 if none).
BOOL IsBackupDue(ULONGLONG nNow, ULONGLONG nLastChange, ULONGLONG nLastBackup, ULONGLONG nLastBackupBytes, BOOL bChangedSinceBackup);

// One backup found on disk by FindBackupFiles.
class CBackupEntry
{
public:
	CString     szPath;			// full path of the .bak file
	CBackupInfo info;			// its header
	ULONGLONG   nWriteTime;		// last-write time (FILETIME as 64 bits), for ordering
};

// Every readable backup under lpszBackupDir's file\, remote\ and untitled\ folders, oldest
// first. Unfinished writes (.tmp), other files, and files without a valid header are left
// out -- and left on disk.
void FindBackupFiles(LPCTSTR lpszBackupDir, CArray<CBackupEntry, CBackupEntry &> & arrFound);

// Tab order across a hot exit: backup\session.xml lists every tab by key, in order.
//
// The key names a document the same way before the exit (from its open tab) and after it
// (from its restored backup): file:<lower-cased path>, remote:<account>:<path>, untitled:<id>.
CString GetBackupKey(const CBackupInfo & info);

BOOL WriteBackupSession(LPCTSTR lpszPathName, const CStringArray & arrKeys, INT nActive);
BOOL ReadBackupSession(LPCTSTR lpszPathName, CStringArray & arrKeys, INT & nActive);	// FALSE: no usable session

// arrOrder[i] = which of arrCurrent goes to position i: tabs the session knows, in session
// order (first entry for a key wins); then the rest, in their current order.
void OrderBySession(const CStringArray & arrCurrent, const CStringArray & arrSession, CArray<INT, INT> & arrOrder);

// What one document remembers between backup-timer ticks. Pure bookkeeping -- the caller
// does the file work and passes in the time (milliseconds, e.g. GetTickCount64()).
class CBackupState
{
public:
	CBackupState();

	void Changed(ULONGLONG nNow);				// the document was edited
	BOOL IsDue(ULONGLONG nNow) const;

	// A backup was written to lpszPath. Returns the previous backup's path when it was a
	// different file (the document was renamed by Save As), which the caller deletes;
	// otherwise an empty string.
	CString Written(ULONGLONG nNow, ULONGLONG nBytes, LPCTSTR lpszPath);

	// The document needs no backup any more: saved, reloaded, or closed. Returns the backup
	// file to delete, or an empty string when there is none.
	CString Discard();

	// Take over a backup restored from a previous session; it is current as it stands.
	void Adopt(LPCTSTR lpszPath);

	BOOL HasBackup() const { return ! m_szPath.IsEmpty(); }

	// The backup on disk holds exactly what the editor holds: exit may skip the "save changes?"
	// prompt for this document. Deliberately independent of the modified flag.
	BOOL IsBackedUp() const { return HasBackup() && ! m_bPending; }

protected:
	ULONGLONG m_nLastChange;
	ULONGLONG m_nLastBackup;		// 0: never backed up in this session
	ULONGLONG m_nLastBackupBytes;
	BOOL      m_bPending;			// changed since the last backup
	CString   m_szPath;				// the backup file on disk, or empty
};

#endif // !defined(_CEDT_BACKUP_H_)

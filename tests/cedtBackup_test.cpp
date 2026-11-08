#include "StdAfx.h"
#include "cedtElement.h"
#include "cedtBackup.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>

// Hot-exit backups (docs/hot-exit.md).
//
// A backup is one file per unsaved document: a single header line describing the
// document, then its text in UTF-8. These tests pin down the parts that have to be
// right before any of it is wired into the document: the header survives every
// value a document can hold, the body survives every character, a failed write
// never disturbs the backup already on disk, and the timing rule decides "now"
// exactly where the design says.

namespace {

CString TempPath(LPCTSTR lpszName)
{
	TCHAR szDir[MAX_PATH]; GetTempPath(MAX_PATH, szDir);
	return CString(szDir) + lpszName;
}

std::vector<unsigned char> ReadBytes(LPCTSTR lpszPath)
{
	std::vector<unsigned char> v;
	FILE * fp = NULL;
	_tfopen_s(&fp, lpszPath, _T("rb"));
	if( ! fp ) return v;
	unsigned char buf[4096]; size_t n;
	while( (n = fread(buf, 1, sizeof(buf), fp)) > 0 ) v.insert(v.end(), buf, buf + n);
	fclose(fp);
	return v;
}

void WriteBytes(LPCTSTR lpszPath, const std::string & s)
{
	FILE * fp = NULL;
	_tfopen_s(&fp, lpszPath, _T("wb"));
	ASSERT_NE(nullptr, fp);
	if( s.size() ) fwrite(s.data(), 1, s.size(), fp);
	fclose(fp);
}

BOOL FileExists(LPCTSTR lpszPath)
{
	return GetFileAttributes(lpszPath) != INVALID_FILE_ATTRIBUTES;
}

void FillText(CAnalyzedText & text, const std::vector<CString> & lines)
{
	text.RemoveAll();
	for( size_t i = 0; i < lines.size(); i++ ) text.AddTail((LPCTSTR)lines[i]);
}

std::vector<CString> TextLines(CAnalyzedText & text)
{
	std::vector<CString> v;
	POSITION pos = text.GetHeadPosition();
	while( pos ) v.push_back(CString((LPCTSTR)text.GetNext(pos)));
	return v;
}

CBackupInfo SampleFileInfo()
{
	CBackupInfo info;
	info.nKind = BACKUP_KIND_FILE;
	info.szPath = _T("E:\\Work\\project dir\\\xD55C\xAE00.txt");	// a space and Korean in the path
	info.nEncodingType = ENCODING_TYPE_UTF8_WBOM;
	info.nFileFormat = FILE_FORMAT_UNIX;
	info.nModifiedTime = 1767225600;									// 2026-01-01, as CTime seconds
	info.nFileSize = 5120;
	info.nLineNum = 42;
	return info;
}

} // namespace


///////////////////////////////////////////////////////////////////////////////
// Header line

TEST(BackupHeaderTest, IsOneLineStartingWithTheTag)
{
	CString szHeader = FormatBackupHeader(SampleFileInfo());

	EXPECT_EQ(0, szHeader.Find(_T("<cedtbackup ")));
	EXPECT_EQ(-1, szHeader.Find(_T('\n')));
	EXPECT_EQ(-1, szHeader.Find(_T('\r')));
}

TEST(BackupHeaderTest, LocalFileRoundTrips)
{
	CBackupInfo in = SampleFileInfo(), out;
	ASSERT_TRUE(ParseBackupHeader(FormatBackupHeader(in), out));

	EXPECT_EQ(BACKUP_KIND_FILE, out.nKind);
	EXPECT_STREQ((LPCTSTR)in.szPath, (LPCTSTR)out.szPath);
	EXPECT_EQ(ENCODING_TYPE_UTF8_WBOM, out.nEncodingType);
	EXPECT_EQ(FILE_FORMAT_UNIX, out.nFileFormat);
	EXPECT_EQ(in.nModifiedTime, out.nModifiedTime);
	EXPECT_EQ(in.nFileSize, out.nFileSize);
	EXPECT_EQ(42, out.nLineNum);
}

TEST(BackupHeaderTest, RemoteFileRoundTrips)
{
	CBackupInfo in;
	in.nKind = BACKUP_KIND_REMOTE;
	in.nAccount = 3;
	in.szPath = _T("/home/user/my \"quoted\" dir/a&b <c>.txt");	// remote names may hold anything
	in.nEncodingType = ENCODING_TYPE_ASCII;
	in.nFileFormat = FILE_FORMAT_DOS;

	CBackupInfo out;
	ASSERT_TRUE(ParseBackupHeader(FormatBackupHeader(in), out));

	EXPECT_EQ(BACKUP_KIND_REMOTE, out.nKind);
	EXPECT_EQ(3, out.nAccount);
	EXPECT_STREQ((LPCTSTR)in.szPath, (LPCTSTR)out.szPath);
}

TEST(BackupHeaderTest, UntitledRoundTripsWithItsIdAndTitle)
{
	CBackupInfo in;
	in.nKind = BACKUP_KIND_UNTITLED;
	in.szId = _T("20261108-0001");
	in.szTitle = _T("Untitled2 \"draft\" & <notes>\r\n\xD55C\xAE00");	// characters that must be escaped

	CBackupInfo out;
	CString szHeader = FormatBackupHeader(in);
	EXPECT_EQ(-1, szHeader.Find(_T('\n')));		// the line break in the title is escaped too
	ASSERT_TRUE(ParseBackupHeader(szHeader, out));

	EXPECT_EQ(BACKUP_KIND_UNTITLED, out.nKind);
	EXPECT_STREQ(_T("20261108-0001"), (LPCTSTR)out.szId);
	EXPECT_STREQ((LPCTSTR)in.szTitle, (LPCTSTR)out.szTitle);
}

TEST(BackupHeaderTest, LargeTimeAndSizeSurvive)
{
	CBackupInfo in = SampleFileInfo(), out;
	in.nModifiedTime = 32503680000;					// year 3000: past 32 bits
	in.nFileSize = 5ULL * 1024 * 1024 * 1024;		// 5 GB: past 32 bits

	ASSERT_TRUE(ParseBackupHeader(FormatBackupHeader(in), out));
	EXPECT_EQ(in.nModifiedTime, out.nModifiedTime);
	EXPECT_EQ(in.nFileSize, out.nFileSize);
}

TEST(BackupHeaderTest, RejectsLinesThatAreNotBackupHeaders)
{
	CBackupInfo out;
	EXPECT_FALSE(ParseBackupHeader(_T(""), out));
	EXPECT_FALSE(ParseBackupHeader(_T("hello world"), out));
	EXPECT_FALSE(ParseBackupHeader(_T("<localfile path=\"C:\\a.txt\" />"), out));
	EXPECT_FALSE(ParseBackupHeader(_T("<cedtbackup version=\"1\" path=\"C:\\a.txt\" />"), out));	// no kind
	EXPECT_FALSE(ParseBackupHeader(_T("<cedtbackup version=\"1\" kind=\"file\" path=\"C:\\a.txt"), out));	// unterminated
}

TEST(BackupHeaderTest, RejectsANewerVersion)
{
	// A backup from a future build may mean something this build cannot honour;
	// leaving it alone is safer than restoring it wrongly.
	CBackupInfo out;
	EXPECT_FALSE(ParseBackupHeader(_T("<cedtbackup version=\"99\" kind=\"file\" path=\"C:\\a.txt\" />"), out));
}

TEST(BackupHeaderTest, IgnoresUnknownAttributes)
{
	CBackupInfo out;
	ASSERT_TRUE(ParseBackupHeader(_T("<cedtbackup version=\"1\" kind=\"file\" path=\"C:\\a.txt\" future=\"x\" />"), out));
	EXPECT_STREQ(_T("C:\\a.txt"), (LPCTSTR)out.szPath);
}


///////////////////////////////////////////////////////////////////////////////
// Backup file

TEST(BackupFileTest, TextRoundTripsExactly)
{
	std::vector<CString> lines = {
		_T("first line"),
		_T(""),													// an empty line in the middle
		_T("\xD55C\xAE00 Korean"),
		_T("emoji \xD83D\xDE00 here"),							// a surrogate pair
		_T("\ttabs and   spaces  "),							// trailing blanks kept
		_T(""),													// a trailing empty line
	};
	CAnalyzedText text; FillText(text, lines);

	CString szPath = TempPath(_T("cedt_backup_roundtrip.bak"));
	ASSERT_TRUE(WriteBackupFile(szPath, SampleFileInfo(), text));

	CBackupInfo out; CAnalyzedText back;
	ASSERT_TRUE(ReadBackupFile(szPath, out, back));

	EXPECT_STREQ((LPCTSTR)SampleFileInfo().szPath, (LPCTSTR)out.szPath);
	std::vector<CString> got = TextLines(back);
	ASSERT_EQ(lines.size(), got.size());
	for( size_t i = 0; i < lines.size(); i++ ) EXPECT_STREQ((LPCTSTR)lines[i], (LPCTSTR)got[i]) << "line " << i;

	_tremove(szPath);
}

TEST(BackupFileTest, AnEmptyDocumentRoundTrips)
{
	CAnalyzedText text; FillText(text, { _T("") });		// an empty document is one empty line

	CString szPath = TempPath(_T("cedt_backup_empty.bak"));
	ASSERT_TRUE(WriteBackupFile(szPath, SampleFileInfo(), text));

	CBackupInfo out; CAnalyzedText back;
	ASSERT_TRUE(ReadBackupFile(szPath, out, back));
	ASSERT_EQ(1, (int)back.GetCount());
	EXPECT_STREQ(_T(""), (LPCTSTR)back.GetHead());

	_tremove(szPath);
}

TEST(BackupFileTest, BodyIsUtf8WhateverTheDocumentEncoding)
{
	// An ANSI (CP949/CP1252) document can hold characters its own encoding cannot
	// write. The backup must not go through that encoding, or they turn into '?'.
	CBackupInfo info = SampleFileInfo();
	info.nEncodingType = ENCODING_TYPE_ASCII;

	CAnalyzedText text; FillText(text, { _T("\x20AC \xD83D\xDE00") });	// euro sign, emoji

	CString szPath = TempPath(_T("cedt_backup_utf8.bak"));
	ASSERT_TRUE(WriteBackupFile(szPath, info, text));

	std::vector<unsigned char> bytes = ReadBytes(szPath);
	std::string s(bytes.begin(), bytes.end());
	size_t nBody = s.find('\n');
	ASSERT_NE(std::string::npos, nBody);
	EXPECT_EQ("\xE2\x82\xAC \xF0\x9F\x98\x80", s.substr(nBody + 1));		// UTF-8, no BOM

	CBackupInfo out; CAnalyzedText back;
	ASSERT_TRUE(ReadBackupFile(szPath, out, back));
	EXPECT_EQ(ENCODING_TYPE_ASCII, out.nEncodingType);		// the document keeps its own encoding
	EXPECT_STREQ(_T("\x20AC \xD83D\xDE00"), (LPCTSTR)back.GetHead());

	_tremove(szPath);
}

TEST(BackupFileTest, RewritingReplacesAndLeavesNoTemporaryFile)
{
	CString szPath = TempPath(_T("cedt_backup_rewrite.bak"));
	CAnalyzedText text;

	FillText(text, { _T("old") });
	ASSERT_TRUE(WriteBackupFile(szPath, SampleFileInfo(), text));
	FillText(text, { _T("new") });
	ASSERT_TRUE(WriteBackupFile(szPath, SampleFileInfo(), text));

	CBackupInfo out; CAnalyzedText back;
	ASSERT_TRUE(ReadBackupFile(szPath, out, back));
	EXPECT_STREQ(_T("new"), (LPCTSTR)back.GetHead());
	EXPECT_FALSE(FileExists(szPath + _T(".tmp")));

	_tremove(szPath);
}

TEST(BackupFileTest, AFailedWriteReportsFalseQuietly)
{
	// Backups are written from a timer every second; a failure must come back as a
	// return value, never as a message box.
	CAnalyzedText text; FillText(text, { _T("x") });
	EXPECT_FALSE(WriteBackupFile(TempPath(_T("no_such_dir_cedt\\a.bak")), SampleFileInfo(), text));
}

TEST(BackupFileTest, AFailedWriteKeepsThePreviousBackup)
{
	CString szPath = TempPath(_T("cedt_backup_keep.bak"));
	CAnalyzedText text; FillText(text, { _T("kept") });
	ASSERT_TRUE(WriteBackupFile(szPath, SampleFileInfo(), text));

	// Hold the temporary file open with no sharing, so the next write cannot create it.
	HANDLE hBlock = CreateFile(szPath + _T(".tmp"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	ASSERT_NE(INVALID_HANDLE_VALUE, hBlock);

	FillText(text, { _T("lost") });
	EXPECT_FALSE(WriteBackupFile(szPath, SampleFileInfo(), text));

	CloseHandle(hBlock);
	_tremove(szPath + _T(".tmp"));

	CBackupInfo out; CAnalyzedText back;
	ASSERT_TRUE(ReadBackupFile(szPath, out, back));
	EXPECT_STREQ(_T("kept"), (LPCTSTR)back.GetHead());

	_tremove(szPath);
}

TEST(BackupFileTest, ReportsTheBytesWritten)
{
	CAnalyzedText text; FillText(text, { _T("abc") });
	CString szPath = TempPath(_T("cedt_backup_bytes.bak"));

	ULONGLONG nBytes = 0;
	ASSERT_TRUE(WriteBackupFile(szPath, SampleFileInfo(), text, & nBytes));
	EXPECT_EQ((ULONGLONG)ReadBytes(szPath).size(), nBytes);

	_tremove(szPath);
}

TEST(BackupFileTest, ReadingAMissingOrForeignFileFails)
{
	CBackupInfo out; CAnalyzedText back;
	EXPECT_FALSE(ReadBackupFile(TempPath(_T("cedt_backup_missing.bak")), out, back));

	CString szPath = TempPath(_T("cedt_backup_foreign.bak"));
	WriteBytes(szPath, "just some text\nsecond line");
	EXPECT_FALSE(ReadBackupFile(szPath, out, back));
	_tremove(szPath);
}

TEST(BackupFileTest, HeaderCanBeReadWithoutTheBody)
{
	// The restore pass scans every backup's header before deciding what to open.
	CString szPath = TempPath(_T("cedt_backup_headeronly.bak"));
	CAnalyzedText text; FillText(text, { _T("body") });
	ASSERT_TRUE(WriteBackupFile(szPath, SampleFileInfo(), text));

	CBackupInfo out;
	ASSERT_TRUE(ReadBackupHeader(szPath, out));
	EXPECT_STREQ((LPCTSTR)SampleFileInfo().szPath, (LPCTSTR)out.szPath);

	_tremove(szPath);
}


///////////////////////////////////////////////////////////////////////////////
// Where a backup lives, relative to the backup directory

TEST(BackupPathTest, LocalFilesGoUnderFileByHashedPath)
{
	CBackupInfo a; a.nKind = BACKUP_KIND_FILE; a.szPath = _T("C:\\Work\\a.txt");
	CString szRel = GetBackupRelativePath(a);

	EXPECT_EQ(0, szRel.Find(_T("file\\")));
	EXPECT_EQ(szRel.GetLength() - 4, szRel.Find(_T(".bak")));
	EXPECT_EQ(-1, szRel.Find(_T("a.txt")));		// a hash, not the name
}

TEST(BackupPathTest, LocalPathsAreCaseAndSlashInsensitive)
{
	CBackupInfo a; a.nKind = BACKUP_KIND_FILE; a.szPath = _T("C:\\Work\\A.txt");
	CBackupInfo b; b.nKind = BACKUP_KIND_FILE; b.szPath = _T("c:/work/a.TXT");
	EXPECT_STREQ((LPCTSTR)GetBackupRelativePath(a), (LPCTSTR)GetBackupRelativePath(b));
}

TEST(BackupPathTest, DifferentLocalPathsDiffer)
{
	CBackupInfo a; a.nKind = BACKUP_KIND_FILE; a.szPath = _T("C:\\Work\\a.txt");
	CBackupInfo b; b.nKind = BACKUP_KIND_FILE; b.szPath = _T("C:\\Work\\b.txt");
	EXPECT_STRNE((LPCTSTR)GetBackupRelativePath(a), (LPCTSTR)GetBackupRelativePath(b));
}

TEST(BackupPathTest, RemotePathsAreCaseSensitiveAndPerAccount)
{
	CBackupInfo a; a.nKind = BACKUP_KIND_REMOTE; a.nAccount = 0; a.szPath = _T("/home/a.txt");
	CBackupInfo b = a; b.szPath = _T("/home/A.txt");		// a different file on a Unix server
	CBackupInfo c = a; c.nAccount = 1;						// same path, another server

	EXPECT_EQ(0, GetBackupRelativePath(a).Find(_T("remote\\")));
	EXPECT_STRNE((LPCTSTR)GetBackupRelativePath(a), (LPCTSTR)GetBackupRelativePath(b));
	EXPECT_STRNE((LPCTSTR)GetBackupRelativePath(a), (LPCTSTR)GetBackupRelativePath(c));
}

TEST(BackupPathTest, UntitledUsesItsId)
{
	CBackupInfo a; a.nKind = BACKUP_KIND_UNTITLED; a.szId = _T("20261108-0001");
	EXPECT_STREQ(_T("untitled\\20261108-0001.bak"), (LPCTSTR)GetBackupRelativePath(a));
}


///////////////////////////////////////////////////////////////////////////////
// When a backup is due
//
// IsBackupDue(now, lastChange, lastBackup, lastBackupBytes, changedSinceBackup),
// all times in milliseconds; lastBackup == 0 means "never backed up".

TEST(BackupDueTest, NothingChangedMeansNothingToDo)
{
	EXPECT_FALSE(IsBackupDue(100000, 1000, 2000, 10, FALSE));
}

TEST(BackupDueTest, SmallDocumentWaitsOneSecondOfQuiet)
{
	ULONGLONG nChange = 50000;
	EXPECT_FALSE(IsBackupDue(nChange + 999,  nChange, 40000, 1000, TRUE));
	EXPECT_TRUE (IsBackupDue(nChange + 1000, nChange, 40000, 1000, TRUE));
}

TEST(BackupDueTest, OneMegabyteStillCountsAsSmall)
{
	ULONGLONG nChange = 50000;
	EXPECT_TRUE(IsBackupDue(nChange + 1000, nChange, nChange - 1, BACKUP_LARGE_BYTES, TRUE));
}

TEST(BackupDueTest, LargeDocumentWaitsThirtySecondsSinceItsLastBackup)
{
	ULONGLONG nBackup = 100000, nBytes = BACKUP_LARGE_BYTES + 1;
	EXPECT_FALSE(IsBackupDue(nBackup + 29999, nBackup + 5000, nBackup, nBytes, TRUE));	// quiet, but too soon
	EXPECT_TRUE (IsBackupDue(nBackup + 30000, nBackup + 5000, nBackup, nBytes, TRUE));
}

TEST(BackupDueTest, LargeDocumentStillWaitsForQuiet)
{
	ULONGLONG nBackup = 100000, nBytes = BACKUP_LARGE_BYTES + 1;
	EXPECT_FALSE(IsBackupDue(nBackup + 60000, nBackup + 59500, nBackup, nBytes, TRUE));	// still typing
	EXPECT_TRUE (IsBackupDue(nBackup + 60500, nBackup + 59500, nBackup, nBytes, TRUE));
}

TEST(BackupDueTest, FirstBackupOfAnyDocumentWaitsOnlyForQuiet)
{
	// The size is only known from a previous backup; the first one is written as if small.
	EXPECT_FALSE(IsBackupDue(5999, 5000, 0, 0, TRUE));
	EXPECT_TRUE (IsBackupDue(6000, 5000, 0, 0, TRUE));
}


///////////////////////////////////////////////////////////////////////////////
// Per-document backup state
//
// What a document remembers between timer ticks: when it last changed, when and
// how large its last backup was, and which file that backup went to -- so a Save As
// or a save can say exactly which file to delete.

TEST(BackupStateTest, AFreshDocumentHasNothingToDo)
{
	CBackupState state;
	EXPECT_FALSE(state.IsDue(100000));
	EXPECT_FALSE(state.HasBackup());
	EXPECT_STREQ(_T(""), (LPCTSTR)state.Discard());
}

TEST(BackupStateTest, AnEditBecomesDueAfterOneSecondOfQuiet)
{
	CBackupState state;
	state.Changed(5000);
	EXPECT_FALSE(state.IsDue(5999));
	EXPECT_TRUE (state.IsDue(6000));
}

TEST(BackupStateTest, WritingSettlesItUntilTheNextEdit)
{
	CBackupState state;
	state.Changed(5000);
	state.Written(6000, 100, _T("C:\\b\\file\\1.bak"));

	EXPECT_TRUE (state.HasBackup());
	EXPECT_FALSE(state.IsDue(60000));

	state.Changed(61000);
	EXPECT_TRUE (state.IsDue(62000));
}

TEST(BackupStateTest, AnEditDuringTheWriteIsNotLost)
{
	// The timer reads the time, writes, then reports. An edit stamped after that
	// time stays pending.
	CBackupState state;
	state.Changed(5000);
	state.Changed(6500);						// typed again while the write was running
	state.Written(6000, 100, _T("C:\\b\\file\\1.bak"));
	EXPECT_TRUE(state.IsDue(7500));
}

TEST(BackupStateTest, ALargeBackupSpacesTheNextOneThirtySeconds)
{
	CBackupState state;
	state.Changed(5000);
	state.Written(6000, BACKUP_LARGE_BYTES + 1, _T("C:\\b\\file\\1.bak"));

	state.Changed(7000);
	EXPECT_FALSE(state.IsDue(35999));
	EXPECT_TRUE (state.IsDue(36000));
}

TEST(BackupStateTest, DiscardHandsBackTheFileToDeleteAndClearsThePending)
{
	CBackupState state;
	state.Changed(5000);
	state.Written(6000, 100, _T("C:\\b\\file\\1.bak"));
	state.Changed(7000);						// edited, then saved before the next backup

	EXPECT_STREQ(_T("C:\\b\\file\\1.bak"), (LPCTSTR)state.Discard());
	EXPECT_FALSE(state.HasBackup());
	EXPECT_FALSE(state.IsDue(100000));			// saved: nothing left to back up
	EXPECT_STREQ(_T(""), (LPCTSTR)state.Discard());

	state.Changed(200000);						// edited again after the save
	EXPECT_TRUE(state.IsDue(201000));
}

TEST(BackupStateTest, WritingUnderANewNameHandsBackTheStaleFile)
{
	// Save As to another path, then more edits: the backup moves to a new name, and
	// the old one must not be left behind to be restored as a phantom next time.
	CBackupState state;
	state.Changed(5000);
	EXPECT_STREQ(_T(""), (LPCTSTR)state.Written(6000, 100, _T("C:\\b\\untitled\\x.bak")));

	state.Changed(7000);
	EXPECT_STREQ(_T(""), (LPCTSTR)state.Written(8000, 100, _T("C:\\b\\untitled\\x.bak")));	// same name: nothing stale

	state.Changed(9000);
	EXPECT_STREQ(_T("C:\\b\\untitled\\x.bak"), (LPCTSTR)state.Written(10000, 100, _T("C:\\b\\file\\2.bak")));
	EXPECT_STREQ(_T("C:\\b\\file\\2.bak"), (LPCTSTR)state.Discard());
}

TEST(BackupStateTest, AdoptingARestoredBackupKeepsItOnRecord)
{
	// After a restart the document comes back modified, its backup already on disk.
	// The state takes that file over, so a save deletes it and a further edit
	// rewrites it in place.
	CBackupState state;
	state.Adopt(_T("C:\\b\\file\\1.bak"));

	EXPECT_TRUE (state.HasBackup());
	EXPECT_FALSE(state.IsDue(100000));			// the file on disk is already current

	state.Changed(200000);
	EXPECT_TRUE(state.IsDue(201000));
	EXPECT_STREQ(_T(""), (LPCTSTR)state.Written(201000, 100, _T("C:\\b\\file\\1.bak")));
	EXPECT_STREQ(_T("C:\\b\\file\\1.bak"), (LPCTSTR)state.Discard());
}


///////////////////////////////////////////////////////////////////////////////
// IsBackedUp: may exit skip the "save changes?" prompt for this document?
//
// Only when the backup on disk holds exactly what is in the editor. The modified
// flag says nothing about that, so it is not consulted.

TEST(BackupStateTest, NotBackedUpBeforeAnyBackup)
{
	CBackupState state;
	EXPECT_FALSE(state.IsBackedUp());
	state.Changed(5000);
	EXPECT_FALSE(state.IsBackedUp());
}

TEST(BackupStateTest, BackedUpOnceWrittenUntilTheNextEdit)
{
	CBackupState state;
	state.Changed(5000);
	state.Written(6000, 100, _T("C:\\b\\file\\1.bak"));
	EXPECT_TRUE(state.IsBackedUp());

	state.Changed(7000);
	EXPECT_FALSE(state.IsBackedUp());			// the file on disk is now behind
}

TEST(BackupStateTest, NotBackedUpWhenEditedDuringTheWrite)
{
	CBackupState state;
	state.Changed(5000);
	state.Changed(6500);
	state.Written(6000, 100, _T("C:\\b\\file\\1.bak"));
	EXPECT_FALSE(state.IsBackedUp());
}

TEST(BackupStateTest, NotBackedUpOnceDiscarded)
{
	CBackupState state;
	state.Changed(5000);
	state.Written(6000, 100, _T("C:\\b\\file\\1.bak"));
	state.Discard();
	EXPECT_FALSE(state.IsBackedUp());
}

TEST(BackupStateTest, AnAdoptedBackupCountsAsBackedUp)
{
	CBackupState state;
	state.Adopt(_T("C:\\b\\file\\1.bak"));
	EXPECT_TRUE(state.IsBackedUp());
}


///////////////////////////////////////////////////////////////////////////////
// Finding the backups to restore

namespace {

// A scratch backup directory with the three kind folders, emptied on the way in.
CString MakeBackupDir(LPCTSTR lpszName)
{
	CString szDir = TempPath(lpszName);
	LPCTSTR kinds[] = { _T("file"), _T("remote"), _T("untitled") };
	CreateDirectory(szDir, NULL);
	for( LPCTSTR k : kinds ) {
		CString szSub = szDir + _T("\\") + k;
		CreateDirectory(szSub, NULL);
		WIN32_FIND_DATA fd; HANDLE h = FindFirstFile(szSub + _T("\\*"), & fd);
		if( h != INVALID_HANDLE_VALUE ) {
			do { if( !(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) DeleteFile(szSub + _T("\\") + fd.cFileName); }
			while( FindNextFile(h, & fd) );
			FindClose(h);
		}
	}
	return szDir;
}

CString WriteSample(const CString & szDir, const CBackupInfo & info, LPCTSTR lpszLine)
{
	CAnalyzedText text; FillText(text, { lpszLine });
	CString szPath = szDir + _T("\\") + GetBackupRelativePath(info);
	EXPECT_TRUE(WriteBackupFile(szPath, info, text));
	return szPath;
}

} // namespace

TEST(BackupScanTest, FindsEveryKindWithItsHeader)
{
	CString szDir = MakeBackupDir(_T("cedt_backup_scan1"));

	CBackupInfo f = SampleFileInfo();
	CBackupInfo r; r.nKind = BACKUP_KIND_REMOTE; r.nAccount = 1; r.szPath = _T("/srv/a.txt");
	CBackupInfo u; u.nKind = BACKUP_KIND_UNTITLED; u.szId = _T("abc"); u.szTitle = _T("Untitled1");
	WriteSample(szDir, f, _T("f")); WriteSample(szDir, r, _T("r")); WriteSample(szDir, u, _T("u"));

	CArray<CBackupEntry, CBackupEntry &> found;
	FindBackupFiles(szDir, found);

	ASSERT_EQ(3, (int)found.GetSize());
	int nFile = 0, nRemote = 0, nUntitled = 0;
	for( int i = 0; i < found.GetSize(); i++ ) {
		switch( found[i].info.nKind ) {
		case BACKUP_KIND_FILE:		nFile++;		EXPECT_STREQ((LPCTSTR)f.szPath, (LPCTSTR)found[i].info.szPath); break;
		case BACKUP_KIND_REMOTE:	nRemote++;		EXPECT_EQ(1, found[i].info.nAccount); break;
		case BACKUP_KIND_UNTITLED:	nUntitled++;	EXPECT_STREQ(_T("Untitled1"), (LPCTSTR)found[i].info.szTitle); break;
		}
		EXPECT_EQ(0, found[i].szPath.Find(szDir));		// full path to the backup file
	}
	EXPECT_EQ(1, nFile); EXPECT_EQ(1, nRemote); EXPECT_EQ(1, nUntitled);
}

TEST(BackupScanTest, SkipsTemporaryAndForeignFiles)
{
	CString szDir = MakeBackupDir(_T("cedt_backup_scan2"));
	WriteSample(szDir, SampleFileInfo(), _T("kept"));

	WriteBytes(szDir + _T("\\file\\half.bak.tmp"), "<cedtbackup version=\"1\" kind=\"file\" path=\"C:\\x\" />\nx");	// a write that never finished
	WriteBytes(szDir + _T("\\file\\notes.txt"), "not a backup");
	WriteBytes(szDir + _T("\\untitled\\broken.bak"), "garbage without a header");

	CArray<CBackupEntry, CBackupEntry &> found;
	FindBackupFiles(szDir, found);

	ASSERT_EQ(1, (int)found.GetSize());
	EXPECT_STREQ((LPCTSTR)SampleFileInfo().szPath, (LPCTSTR)found[0].info.szPath);
}

TEST(BackupScanTest, AnEmptyOrMissingDirectoryFindsNothing)
{
	CArray<CBackupEntry, CBackupEntry &> found;
	FindBackupFiles(MakeBackupDir(_T("cedt_backup_scan3")), found);
	EXPECT_EQ(0, (int)found.GetSize());

	FindBackupFiles(TempPath(_T("cedt_backup_no_such_dir")), found);
	EXPECT_EQ(0, (int)found.GetSize());
}

TEST(BackupScanTest, OlderBackupsComeFirst)
{
	// Without a session file to give the tab order, restored tabs follow the order the
	// backups were written in -- which is roughly the order the documents were edited.
	CString szDir = MakeBackupDir(_T("cedt_backup_scan4"));
	CBackupInfo a; a.nKind = BACKUP_KIND_UNTITLED; a.szId = _T("zzz-first");
	CBackupInfo b; b.nKind = BACKUP_KIND_UNTITLED; b.szId = _T("aaa-second");
	CString szA = WriteSample(szDir, a, _T("a"));
	CString szB = WriteSample(szDir, b, _T("b"));

	// Make the order unmistakable regardless of file-system timestamp resolution.
	HANDLE h = CreateFile(szA, FILE_WRITE_ATTRIBUTES, 0, NULL, OPEN_EXISTING, 0, NULL);
	FILETIME ft = { 0x10000000, 0x01D00000 }; SetFileTime(h, NULL, NULL, & ft); CloseHandle(h);

	CArray<CBackupEntry, CBackupEntry &> found;
	FindBackupFiles(szDir, found);
	ASSERT_EQ(2, (int)found.GetSize());
	EXPECT_STREQ(_T("zzz-first"), (LPCTSTR)found[0].info.szId);
	EXPECT_STREQ(_T("aaa-second"), (LPCTSTR)found[1].info.szId);
}

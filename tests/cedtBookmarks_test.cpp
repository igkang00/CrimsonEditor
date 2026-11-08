#include "StdAfx.h"
#include "cedtElement.h"
#include "cedtBookmarks.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>

// Persistent bookmarks (docs/bookmarks.md).
//
// A bookmark is kept as a line index plus a hash of the line's text. While the file is
// unchanged the index is enough; once it changed, the bookmark goes to the nearest line
// with the same content, or is dropped. These tests pin that down, and the store that
// keeps the 100 most recently used files' bookmarks in bookmarks.xml.

namespace {

CString TempPath(LPCTSTR lpszName)
{
	TCHAR szDir[MAX_PATH]; GetTempPath(MAX_PATH, szDir);
	return CString(szDir) + lpszName;
}

void WriteBytes(LPCTSTR lpszPath, const std::string & s)
{
	FILE * fp = NULL;
	_tfopen_s(&fp, lpszPath, _T("wb"));
	ASSERT_NE(nullptr, fp);
	if( s.size() ) fwrite(s.data(), 1, s.size(), fp);
	fclose(fp);
}

// The hashes of a document's lines, as the document side will compute them.
std::vector<ULONGLONG> Hashes(std::vector<LPCTSTR> lines)
{
	std::vector<ULONGLONG> v;
	for( LPCTSTR s : lines ) v.push_back(HashBookmarkLine(s));
	return v;
}

// Bookmarks on the given lines of a document.
std::vector<CBookmark> Marks(const std::vector<LPCTSTR> & lines, std::vector<INT> at)
{
	std::vector<CBookmark> v;
	for( INT i : at ) { CBookmark b; b.nLine = i; b.nHash = HashBookmarkLine(lines[i]); v.push_back(b); }
	return v;
}

std::vector<INT> Refind(const std::vector<CBookmark> & marks, std::vector<LPCTSTR> now, BOOL bUnchanged)
{
	return RefindBookmarks(marks, Hashes(now), bUnchanged);
}

CBookmarkEntry Entry(LPCTSTR lpszKey, std::vector<INT> lines = { 0 })
{
	CBookmarkEntry e; e.szKey = lpszKey; e.nModifiedTime = 1767225600; e.nFileSize = 100;
	for( INT i : lines ) { CBookmark b; b.nLine = i; b.nHash = 0x1234 + i; e.marks.push_back(b); }
	return e;
}

} // namespace


///////////////////////////////////////////////////////////////////////////////
// The line hash

TEST(BookmarkHashTest, SameTextSameHash)
{
	EXPECT_EQ(HashBookmarkLine(_T("int main()")), HashBookmarkLine(_T("int main()")));
	EXPECT_NE(HashBookmarkLine(_T("int main()")), HashBookmarkLine(_T("int main(void)")));
}

TEST(BookmarkHashTest, IndentationAndTrailingBlanksDoNotCount)
{
	// Re-indenting a block must not lose the bookmarks in it.
	ULONGLONG h = HashBookmarkLine(_T("return 0;"));
	EXPECT_EQ(h, HashBookmarkLine(_T("    return 0;")));
	EXPECT_EQ(h, HashBookmarkLine(_T("\t\treturn 0;  \t")));
}

TEST(BookmarkHashTest, BlanksInsideTheLineDoCount)
{
	EXPECT_NE(HashBookmarkLine(_T("a = b")), HashBookmarkLine(_T("a=b")));
}

TEST(BookmarkHashTest, KoreanAndEmojiHashLikeAnyText)
{
	EXPECT_EQ(HashBookmarkLine(_T("\xD55C\xAE00 \xD83D\xDE00")), HashBookmarkLine(_T("  \xD55C\xAE00 \xD83D\xDE00")));
	EXPECT_NE(HashBookmarkLine(_T("\xD55C\xAE00")), HashBookmarkLine(_T("\xD55C")));
}

TEST(BookmarkHashTest, BlankLinesAllHashAlike)
{
	EXPECT_EQ(HashBookmarkLine(_T("")), HashBookmarkLine(_T("   \t")));
}


///////////////////////////////////////////////////////////////////////////////
// Finding bookmarks again

TEST(BookmarkRefindTest, AnUnchangedFileKeepsEveryIndex)
{
	std::vector<LPCTSTR> doc = { _T("a"), _T("b"), _T("c"), _T("d") };
	EXPECT_EQ((std::vector<INT>{ 1, 3 }), Refind(Marks(doc, { 1, 3 }), doc, TRUE));
}

TEST(BookmarkRefindTest, AnUnchangedFileDropsIndexesPastTheEnd)
{
	std::vector<LPCTSTR> doc = { _T("a"), _T("b"), _T("c") };
	std::vector<CBookmark> marks = Marks(doc, { 1 });
	CBookmark past; past.nLine = 7; past.nHash = 0; marks.push_back(past);
	EXPECT_EQ((std::vector<INT>{ 1 }), Refind(marks, doc, TRUE));
}

TEST(BookmarkRefindTest, LinesInsertedAboveMoveTheBookmarkDown)
{
	std::vector<LPCTSTR> before = { _T("one"), _T("two"), _T("three") };
	std::vector<LPCTSTR> after  = { _T("new 1"), _T("new 2"), _T("one"), _T("two"), _T("three") };
	EXPECT_EQ((std::vector<INT>{ 3, 4 }), Refind(Marks(before, { 1, 2 }), after, FALSE));
}

TEST(BookmarkRefindTest, LinesDeletedAboveMoveTheBookmarkUp)
{
	std::vector<LPCTSTR> before = { _T("x"), _T("y"), _T("one"), _T("two") };
	std::vector<LPCTSTR> after  = { _T("one"), _T("two") };
	EXPECT_EQ((std::vector<INT>{ 1 }), Refind(Marks(before, { 3 }), after, FALSE));
}

TEST(BookmarkRefindTest, ABookmarkedLineThatWasEditedIsDropped)
{
	std::vector<LPCTSTR> before = { _T("one"), _T("two"), _T("three") };
	std::vector<LPCTSTR> after  = { _T("one"), _T("TWO!"), _T("three") };
	EXPECT_EQ((std::vector<INT>{ 2 }), Refind(Marks(before, { 1, 2 }), after, FALSE));
}

TEST(BookmarkRefindTest, ReindentedLinesKeepTheirBookmarks)
{
	std::vector<LPCTSTR> before = { _T("if( x )"), _T("return 1;") };
	std::vector<LPCTSTR> after  = { _T("if( x ) {"), _T("    return 1;"), _T("}") };
	EXPECT_EQ((std::vector<INT>{ 1 }), Refind(Marks(before, { 1 }), after, FALSE));
}

TEST(BookmarkRefindTest, ADuplicatedLineTakesTheNearestCopy)
{
	// "}" at 5 was bookmarked; after the change, copies sit at 2, 6 and 9.
	std::vector<LPCTSTR> before = { _T("a"), _T("b"), _T("c"), _T("d"), _T("e"), _T("}") };
	std::vector<LPCTSTR> after  = { _T("a"), _T("b"), _T("}"), _T("c"), _T("d"), _T("e"), _T("}"), _T("f"), _T("g"), _T("}") };
	EXPECT_EQ((std::vector<INT>{ 6 }), Refind(Marks(before, { 5 }), after, FALSE));
}

TEST(BookmarkRefindTest, AnEvenTieGoesToTheEarlierLine)
{
	std::vector<LPCTSTR> before = { _T("a"), _T("b"), _T("}") };	// "}" at 2
	std::vector<LPCTSTR> after  = { _T("}"), _T("x"), _T("y"), _T("z"), _T("}") };	// copies at 0 and 4
	EXPECT_EQ((std::vector<INT>{ 0 }), Refind(Marks(before, { 2 }), after, FALSE));
}

TEST(BookmarkRefindTest, TwoBookmarksOnOneLineBecomeOne)
{
	// Two bookmarked blank lines, and now only one blank line is left.
	std::vector<LPCTSTR> before = { _T("a"), _T(""), _T("b"), _T("") };
	std::vector<LPCTSTR> after  = { _T("a"), _T("b"), _T("") };
	EXPECT_EQ((std::vector<INT>{ 2 }), Refind(Marks(before, { 1, 3 }), after, FALSE));
}

TEST(BookmarkRefindTest, TheResultIsInLineOrder)
{
	// The bookmark recorded second now lies above the one recorded first.
	std::vector<LPCTSTR> before = { _T("alpha"), _T("beta") };
	std::vector<LPCTSTR> after  = { _T("beta"), _T("x"), _T("alpha") };
	EXPECT_EQ((std::vector<INT>{ 0, 2 }), Refind(Marks(before, { 0, 1 }), after, FALSE));
}

TEST(BookmarkRefindTest, NothingRecordedNothingFound)
{
	EXPECT_TRUE(Refind({ }, { _T("a") }, FALSE).empty());
	EXPECT_TRUE(Refind({ }, { _T("a") }, TRUE).empty());
}


///////////////////////////////////////////////////////////////////////////////
// The store: bookmarks.xml, 100 most recently used files

TEST(BookmarkStoreTest, RecordThenLookUp)
{
	CBookmarkStore store;
	store.Record(Entry(_T("file:c:\\a.c"), { 3, 9 }));

	CBookmarkEntry e;
	ASSERT_TRUE(store.Lookup(_T("file:c:\\a.c"), e));
	ASSERT_EQ(2u, e.marks.size());
	EXPECT_EQ(3, e.marks[0].nLine);
	EXPECT_EQ(9, e.marks[1].nLine);
	EXPECT_FALSE(store.Lookup(_T("file:c:\\other.c"), e));
}

TEST(BookmarkStoreTest, RecordingAgainReplacesAndMovesToTheFront)
{
	CBookmarkStore store;
	store.Record(Entry(_T("file:a"), { 1 }));
	store.Record(Entry(_T("file:b"), { 2 }));
	store.Record(Entry(_T("file:a"), { 5 }));

	ASSERT_EQ(2, store.GetCount());
	EXPECT_STREQ(_T("file:a"), (LPCTSTR)store.GetAt(0).szKey);	// most recent first
	EXPECT_STREQ(_T("file:b"), (LPCTSTR)store.GetAt(1).szKey);
	EXPECT_EQ(5, store.GetAt(0).marks[0].nLine);
}

TEST(BookmarkStoreTest, RecordingNoBookmarksRemovesTheEntry)
{
	CBookmarkStore store;
	store.Record(Entry(_T("file:a"), { 1 }));
	store.Record(Entry(_T("file:a"), { }));

	CBookmarkEntry e;
	EXPECT_FALSE(store.Lookup(_T("file:a"), e));
	EXPECT_EQ(0, store.GetCount());
}

TEST(BookmarkStoreTest, KeepsOnlyTheHundredMostRecent)
{
	CBookmarkStore store;
	for( INT i = 0; i < BOOKMARK_MAX_FILES + 5; i++ ) {
		CString szKey; szKey.Format(_T("file:%d"), i);
		store.Record(Entry(szKey));
	}
	EXPECT_EQ(BOOKMARK_MAX_FILES, store.GetCount());

	CBookmarkEntry e;
	EXPECT_FALSE(store.Lookup(_T("file:0"), e));		// the oldest five are gone
	EXPECT_FALSE(store.Lookup(_T("file:4"), e));
	EXPECT_TRUE (store.Lookup(_T("file:5"), e));
	EXPECT_TRUE (store.Lookup(_T("file:104"), e));
}

TEST(BookmarkStoreTest, SavesAndLoadsEverything)
{
	CBookmarkStore store;
	CBookmarkEntry a = Entry(_T("file:c:\\\xD55C\xAE00\\\"q\" & <x>.c"), { 0, 41 });	// Korean, characters to escape
	a.nModifiedTime = 32503680000;						// past 32 bits
	a.nFileSize = 5ULL * 1024 * 1024 * 1024;
	a.marks[1].nHash = 0xFEDCBA9876543210ULL;			// all 64 bits
	store.Record(Entry(_T("remote:0:/srv/b.txt"), { 7 }));
	store.Record(a);

	CString szPath = TempPath(_T("cedt_bookmarks.xml"));
	ASSERT_TRUE(store.Save(szPath));

	CBookmarkStore back;
	ASSERT_TRUE(back.Load(szPath));
	ASSERT_EQ(2, back.GetCount());

	const CBookmarkEntry & b = back.GetAt(0);			// order survives
	EXPECT_STREQ((LPCTSTR)a.szKey, (LPCTSTR)b.szKey);
	EXPECT_EQ(a.nModifiedTime, b.nModifiedTime);
	EXPECT_EQ(a.nFileSize, b.nFileSize);
	ASSERT_EQ(2u, b.marks.size());
	EXPECT_EQ(41, b.marks[1].nLine);
	EXPECT_EQ(0xFEDCBA9876543210ULL, b.marks[1].nHash);
	EXPECT_STREQ(_T("remote:0:/srv/b.txt"), (LPCTSTR)back.GetAt(1).szKey);

	EXPECT_EQ(INVALID_FILE_ATTRIBUTES, GetFileAttributes(szPath + _T(".tmp")));
	_tremove(szPath);
}

TEST(BookmarkStoreTest, AMissingOrForeignFileLoadsAsEmpty)
{
	CBookmarkStore store;
	store.Record(Entry(_T("file:a")));
	EXPECT_FALSE(store.Load(TempPath(_T("cedt_bookmarks_missing.xml"))));
	EXPECT_EQ(0, store.GetCount());

	CString szPath = TempPath(_T("cedt_bookmarks_foreign.xml"));
	WriteBytes(szPath, "<cedtsession version=\"1\" active=\"0\" />\n");
	EXPECT_FALSE(store.Load(szPath));
	EXPECT_EQ(0, store.GetCount());
	_tremove(szPath);
}

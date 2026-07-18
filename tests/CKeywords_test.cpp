#include "StdAfx.h"
#include "cedtElement.h"
#include <gtest/gtest.h>
#include <string>

namespace {

CString KeywordTempPath(LPCTSTR lpszName)
{
	TCHAR szDir[MAX_PATH]; GetTempPath(MAX_PATH, szDir);
	return CString(szDir) + lpszName;
}

void WriteAsciiFile(LPCTSTR lpszPath, const std::string & text)
{
	FILE * fp = NULL;
	_tfopen_s(&fp, lpszPath, _T("wb"));
	ASSERT_NE(nullptr, fp);
	if( ! text.empty() ) fwrite(text.data(), 1, text.size(), fp);
	fclose(fp);
}

} // namespace

TEST(CKeywordsTest, FreshlyConstructed_IsEmpty)
{
    CKeywords kw;
    EXPECT_EQ(0, kw.GetCount());
}

TEST(CKeywordsTest, SetAtAndLookup_RoundTrip)
{
    CKeywords kw;
    DWORD v1 = 0x12345678;
    DWORD v2 = 0xABCDEF01;
    kw.SetAt(_T("C:if"),    v1);
    kw.SetAt(_T("C:while"), v2);

    DWORD value = 0;
    EXPECT_TRUE(kw.Lookup(_T("C:if"), value));
    EXPECT_EQ(0x12345678u, value);

    EXPECT_TRUE(kw.Lookup(_T("C:while"), value));
    EXPECT_EQ(0xABCDEF01u, value);
}

TEST(CKeywordsTest, Lookup_MissingKey_ReturnsFalse)
{
    CKeywords kw;
    DWORD value = 0;
    EXPECT_FALSE(kw.Lookup(_T("never-set"), value));
}

TEST(CKeywordsTest, ResetContents_EmptiesTheMap)
{
    CKeywords kw;
    DWORD a = 1, b = 2;
    kw.SetAt(_T("foo"), a);
    kw.SetAt(_T("bar"), b);
    ASSERT_EQ(2, kw.GetCount());

    kw.ResetContents();
    EXPECT_EQ(0, kw.GetCount());
}

// A keyword longer than MAX_WORD_LENGTH is read with stream.width(), which truncates the token
// but leaves the remainder in the stream. Before the fix the remainder came back as the NEXT
// token and registered as a keyword of its own — a word nobody wrote, which then coloured in
// the editor. The tail must be discarded, and the token after it must still parse.
TEST(CKeywordsTest, FileLoad_OverlongKeyword_DoesNotRegisterItsTail)
{
    const std::string head(MAX_WORD_LENGTH, 'n');       // the part that fits
    const std::string tail(45, 'n');                    // what width() left behind

    std::string body = "[KEYWORDS0:GLOBAL]\r\n";
    body += head + tail + "\r\n";                       // one 300-character keyword
    body += "after_the_long_one\r\n";

    CString szPath = KeywordTempPath(_T("cedt_kw_overlong.key"));
    WriteAsciiFile(szPath, body);

    BOOL bCaseSensitive[4] = { TRUE, TRUE, TRUE, TRUE };    // case sensitive -> "C:" keys
    CKeywords kw;
    ASSERT_TRUE(kw.FileLoad(szPath, bCaseSensitive));

    DWORD value = 0;
    EXPECT_TRUE(kw.Lookup(CString(_T("C:")) + CString(head.c_str()), value))
        << "the truncated head should still register";
    EXPECT_FALSE(kw.Lookup(CString(_T("C:")) + CString(tail.c_str()), value))
        << "the discarded tail must not become a keyword";
    EXPECT_TRUE(kw.Lookup(_T("C:after_the_long_one"), value))
        << "the reader must not desync on the token after an over-long one";

    _tremove(szPath);
}

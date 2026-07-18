#include "StdAfx.h"
#include "cedtElement.h"
#include <gtest/gtest.h>
#include <string>

TEST(CDictionaryTest, FreshlyConstructed_IsEmpty)
{
    CDictionary dict;
    EXPECT_EQ(0, dict.GetCount());
}

TEST(CDictionaryTest, AddWord_NewWord_ReturnsTrue)
{
    CDictionary dict;
    EXPECT_TRUE(dict.AddWord(_T("hello")));
    EXPECT_EQ(1, dict.GetCount());
}

TEST(CDictionaryTest, AddWord_Duplicate_ReturnsFalse)
{
    CDictionary dict;
    ASSERT_TRUE(dict.AddWord(_T("hello")));
    EXPECT_FALSE(dict.AddWord(_T("hello")));
    EXPECT_EQ(1, dict.GetCount());
}

TEST(CDictionaryTest, AddWord_CaseInsensitive_TreatedAsSame)
{
    CDictionary dict;
    ASSERT_TRUE(dict.AddWord(_T("Hello")));
    EXPECT_FALSE(dict.AddWord(_T("HELLO")));
    EXPECT_FALSE(dict.AddWord(_T("hello")));
    EXPECT_EQ(1, dict.GetCount());
}

TEST(CDictionaryTest, LookupTable_FindsAddedWord_CaseInsensitive)
{
    CDictionary dict;
    dict.AddWord(_T("Crimson"));
    EXPECT_TRUE(dict.LookupTable(_T("crimson"), 7));
    EXPECT_TRUE(dict.LookupTable(_T("CRIMSON"), 7));
    EXPECT_FALSE(dict.LookupTable(_T("editor"), 6));
}

TEST(CDictionaryTest, AddWord_OverLongWord_ReturnsFalse)
{
    CDictionary dict;
    CString longWord('a', MAX_WORD_LENGTH + 5);
    EXPECT_FALSE(dict.AddWord(longWord));
    EXPECT_EQ(0, dict.GetCount());
}

// The same width() hazard as CKeywords::FileLoad: an over-long word is truncated, and its
// remainder used to come back as the next word — a spelling nobody put in the dictionary,
// which then made a real misspelling read as correct.
TEST(CDictionaryTest, FileLoad_OverlongWord_DoesNotRegisterItsTail)
{
    const std::string head(MAX_WORD_LENGTH, 'y');
    const std::string tail(45, 'y');

    std::string body = head + tail + "\r\nomega\r\n";     // one 300-character word, then a real one

    TCHAR szDir[MAX_PATH]; GetTempPath(MAX_PATH, szDir);
    CString szPath = CString(szDir) + _T("cedt_dic_overlong.dic");

    FILE * fp = NULL;
    _tfopen_s(&fp, szPath, _T("wb"));
    ASSERT_NE(nullptr, fp);
    fwrite(body.data(), 1, body.size(), fp);
    fclose(fp);

    CDictionary dict;
    ASSERT_TRUE(dict.FileLoad(szPath));

    UCHAR value = 0;
    EXPECT_TRUE(dict.Lookup(CString(head.c_str()), value))
        << "the truncated head should still register";
    EXPECT_FALSE(dict.Lookup(CString(tail.c_str()), value))
        << "the discarded tail must not become a dictionary word";
    EXPECT_TRUE(dict.Lookup(_T("omega"), value))
        << "the reader must not desync on the word after an over-long one";

    _tremove(szPath);
}

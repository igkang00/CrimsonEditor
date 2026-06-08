#include "StdAfx.h"
#include "cedtElement.h"
#include <gtest/gtest.h>

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

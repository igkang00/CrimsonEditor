#include "StdAfx.h"
#include "RegExp.h"
#include <gtest/gtest.h>

TEST(RegExpTest, LiteralMatch_Found)
{
    CRegExp re;
    ASSERT_NE(nullptr, re.RegComp(_T("hello")));
    EXPECT_NE(0, re.RegFind(_T("world hello there")));
}

TEST(RegExpTest, LiteralMatch_NotFound_ReturnsNegative)
{
    CRegExp re;
    re.RegComp(_T("xyz"));
    EXPECT_LT(re.RegFind(_T("hello world")), 0);
}

TEST(RegExpTest, GetFoundString_ReturnsMatchedSubstring)
{
    CRegExp re;
    re.RegComp(_T("[0-9]+"));
    ASSERT_GE(re.RegFind(_T("answer is 42 today")), 0);
    CString found;
    EXPECT_TRUE(re.GetFoundString(found));
    EXPECT_STREQ(_T("42"), (LPCTSTR)found);
}

TEST(RegExpTest, Metacharacters_DotPlus)
{
    CRegExp re;
    re.RegComp(_T("a.+c"));
    ASSERT_GE(re.RegFind(_T("xx abc yy")), 0);
    CString found;
    re.GetFoundString(found);
    EXPECT_STREQ(_T("abc"), (LPCTSTR)found);
}

TEST(RegExpTest, GroupCapture_AndReplace)
{
    CRegExp re;
    re.RegComp(_T("([0-9]+)-([0-9]+)"));
    ASSERT_GE(re.RegFind(_T("range 10-20 here")), 0);
    CString replaced;
    EXPECT_TRUE(re.GetReplaceString(_T("\\2-\\1"), replaced));
    EXPECT_STREQ(_T("20-10"), (LPCTSTR)replaced);
}

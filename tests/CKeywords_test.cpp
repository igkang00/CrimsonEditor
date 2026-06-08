#include "StdAfx.h"
#include "cedtElement.h"
#include <gtest/gtest.h>

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

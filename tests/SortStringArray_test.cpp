#include "StdAfx.h"
#include "SortStringArray.h"
#include <gtest/gtest.h>

TEST(SortStringArrayTest, QuickSort_CaseSensitive)
{
    CSortStringArray arr;
    arr.Add(_T("Banana"));
    arr.Add(_T("apple"));
    arr.Add(_T("Cherry"));
    arr.QuickSort(TRUE); // case-sensitive: uppercase letters come before lowercase

    EXPECT_STREQ(_T("Banana"), (LPCTSTR)arr[0]);
    EXPECT_STREQ(_T("Cherry"), (LPCTSTR)arr[1]);
    EXPECT_STREQ(_T("apple"),  (LPCTSTR)arr[2]);
}

TEST(SortStringArrayTest, QuickSort_CaseInsensitive)
{
    CSortStringArray arr;
    arr.Add(_T("Banana"));
    arr.Add(_T("apple"));
    arr.Add(_T("Cherry"));
    arr.Sort(); // default = case-insensitive

    EXPECT_STREQ(_T("apple"),  (LPCTSTR)arr[0]);
    EXPECT_STREQ(_T("Banana"), (LPCTSTR)arr[1]);
    EXPECT_STREQ(_T("Cherry"), (LPCTSTR)arr[2]);
}

TEST(SortStringArrayTest, Sort_EmptyArray_NoCrash)
{
    CSortStringArray arr;
    arr.Sort();
    EXPECT_EQ(0, arr.GetSize());
}

TEST(SortStringArrayTest, Sort_SingleElement_NoChange)
{
    CSortStringArray arr;
    arr.Add(_T("only"));
    arr.Sort();
    EXPECT_EQ(1, arr.GetSize());
    EXPECT_STREQ(_T("only"), (LPCTSTR)arr[0]);
}

TEST(SortStringArrayTest, BubbleSort_SameResultAsQuickSort)
{
    CSortStringArray a, b;
    const char * input[] = { "delta", "alpha", "echo", "bravo", "charlie" };
    for (auto s : input) { a.Add(s); b.Add(s); }
    a.QuickSort(TRUE);
    b.BubbleSort(TRUE);
    ASSERT_EQ(a.GetSize(), b.GetSize());
    for (INT i = 0; i < a.GetSize(); i++)
        EXPECT_STREQ((LPCTSTR)a[i], (LPCTSTR)b[i]);
}

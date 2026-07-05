#include "StdAfx.h"
#include "PathName.h"
#include <gtest/gtest.h>

TEST(PathNameTest, GetFileName_ReturnsLastSegment)
{
    EXPECT_STREQ(_T("file.txt"), (LPCTSTR)GetFileName(_T("C:\\folder\\file.txt")));
    EXPECT_STREQ(_T("file.txt"), (LPCTSTR)GetFileName(_T("file.txt")));
}

TEST(PathNameTest, GetFileTitle_StripsExtension)
{
    EXPECT_STREQ(_T("file"), (LPCTSTR)GetFileTitle(_T("C:\\folder\\file.txt")));
    EXPECT_STREQ(_T("archive.tar"), (LPCTSTR)GetFileTitle(_T("archive.tar.gz")));
}

TEST(PathNameTest, GetFileExtension_ReturnsExtWithDot)
{
    EXPECT_STREQ(_T(".txt"), (LPCTSTR)GetFileExtension(_T("C:\\folder\\file.txt")));
    EXPECT_STREQ(_T(""),     (LPCTSTR)GetFileExtension(_T("Makefile")));
}

TEST(PathNameTest, GetFileDirectory_StripsTrailingFile)
{
    EXPECT_STREQ(_T("C:\\folder"), (LPCTSTR)GetFileDirectory(_T("C:\\folder\\file.txt")));
}

TEST(PathNameTest, ChopDirectory_RemovesTrailingBackslash)
{
    EXPECT_STREQ(_T("C:\\folder"), (LPCTSTR)ChopDirectory(_T("C:\\folder\\")));
    EXPECT_STREQ(_T("C:\\folder"), (LPCTSTR)ChopDirectory(_T("C:\\folder")));
}

TEST(PathNameTest, QuotePathName_WrapsWithDoubleQuotes)
{
    CString quoted = QuotePathName(_T("file with spaces.txt"));
    EXPECT_EQ(_T('"'), quoted[0]);
    EXPECT_EQ(_T('"'), quoted[quoted.GetLength() - 1]);
}

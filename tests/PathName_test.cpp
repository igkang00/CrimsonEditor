#include "StdAfx.h"
#include "PathName.h"
#include <gtest/gtest.h>

TEST(PathNameTest, GetFileName_ReturnsLastSegment)
{
    EXPECT_STREQ("file.txt", (LPCTSTR)GetFileName("C:\\folder\\file.txt"));
    EXPECT_STREQ("file.txt", (LPCTSTR)GetFileName("file.txt"));
}

TEST(PathNameTest, GetFileTitle_StripsExtension)
{
    EXPECT_STREQ("file", (LPCTSTR)GetFileTitle("C:\\folder\\file.txt"));
    EXPECT_STREQ("archive.tar", (LPCTSTR)GetFileTitle("archive.tar.gz"));
}

TEST(PathNameTest, GetFileExtension_ReturnsExtWithDot)
{
    EXPECT_STREQ(".txt", (LPCTSTR)GetFileExtension("C:\\folder\\file.txt"));
    EXPECT_STREQ("",     (LPCTSTR)GetFileExtension("Makefile"));
}

TEST(PathNameTest, GetFileDirectory_StripsTrailingFile)
{
    EXPECT_STREQ("C:\\folder", (LPCTSTR)GetFileDirectory("C:\\folder\\file.txt"));
}

TEST(PathNameTest, ChopDirectory_RemovesTrailingBackslash)
{
    EXPECT_STREQ("C:\\folder", (LPCTSTR)ChopDirectory("C:\\folder\\"));
    EXPECT_STREQ("C:\\folder", (LPCTSTR)ChopDirectory("C:\\folder"));
}

TEST(PathNameTest, QuotePathName_WrapsWithDoubleQuotes)
{
    CString quoted = QuotePathName("file with spaces.txt");
    EXPECT_EQ('"', quoted[0]);
    EXPECT_EQ('"', quoted[quoted.GetLength() - 1]);
}

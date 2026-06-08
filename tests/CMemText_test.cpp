#include "StdAfx.h"
#include "cedtElement.h"
#include <gtest/gtest.h>

namespace {

void AddLines(CMemText & m, std::initializer_list<const char *> lines)
{
    for (auto s : lines) m.AddTail(CString(s));
}

} // namespace

TEST(CMemTextTest, MakeUpperCase_UppercasesAllLines)
{
    CMemText m;
    AddLines(m, { "hello", "World", "abc 123" });
    m.MakeUpperCase();

    POSITION pos = m.GetHeadPosition();
    EXPECT_STREQ(_T("HELLO"),   (LPCTSTR)m.GetNext(pos));
    EXPECT_STREQ(_T("WORLD"),   (LPCTSTR)m.GetNext(pos));
    EXPECT_STREQ(_T("ABC 123"), (LPCTSTR)m.GetNext(pos));
}

TEST(CMemTextTest, MakeLowerCase_LowercasesAllLines)
{
    CMemText m;
    AddLines(m, { "HELLO", "World" });
    m.MakeLowerCase();

    POSITION pos = m.GetHeadPosition();
    EXPECT_STREQ(_T("hello"), (LPCTSTR)m.GetNext(pos));
    EXPECT_STREQ(_T("world"), (LPCTSTR)m.GetNext(pos));
}

TEST(CMemTextTest, GetMaxLength_ReturnsLongestLine)
{
    CMemText m;
    AddLines(m, { "ab", "abcdefg", "abcd" });
    EXPECT_EQ(7, m.GetMaxLength());
}

TEST(CMemTextTest, GetMaxLength_EmptyReturnsZero)
{
    CMemText m;
    EXPECT_EQ(0, m.GetMaxLength());
}

TEST(CMemTextTest, MakeEqualLength_PadsShortLinesWithSpaces)
{
    CMemText m;
    AddLines(m, { "a", "abc", "ab" });
    m.MakeEqualLength();

    POSITION pos = m.GetHeadPosition();
    EXPECT_STREQ(_T("a  "), (LPCTSTR)m.GetNext(pos));
    EXPECT_STREQ(_T("abc"), (LPCTSTR)m.GetNext(pos));
    EXPECT_STREQ(_T("ab "), (LPCTSTR)m.GetNext(pos));
}

TEST(CMemTextTest, AssignmentOperator_DeepCopy)
{
    CMemText src;
    AddLines(src, { "one", "two" });
    CMemText dst;
    dst = src;

    src.RemoveAll();
    EXPECT_EQ(2, dst.GetCount());
    POSITION pos = dst.GetHeadPosition();
    EXPECT_STREQ(_T("one"), (LPCTSTR)dst.GetNext(pos));
    EXPECT_STREQ(_T("two"), (LPCTSTR)dst.GetNext(pos));
}

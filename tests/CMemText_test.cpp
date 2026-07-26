#include "StdAfx.h"
#include "cedtElement.h"
#include <gtest/gtest.h>
#include <vector>

namespace {

void AddLines(CMemText & m, std::initializer_list<const char *> lines)
{
    for (auto s : lines) m.AddTail(CString(s));
}

CString MemTextTempPath(LPCTSTR lpszName)
{
    TCHAR szDir[MAX_PATH]; GetTempPath(MAX_PATH, szDir);
    return CString(szDir) + lpszName;
}

void WriteBytesToFile(LPCTSTR lpszPath, const std::vector<unsigned char> & bytes)
{
    FILE * fp = NULL;
    _tfopen_s(&fp, lpszPath, _T("wb"));
    ASSERT_NE(nullptr, fp);
    if (!bytes.empty()) fwrite(bytes.data(), 1, bytes.size(), fp);
    fclose(fp);
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

// FileLoad — the Insert File path. It used to decode every file as CP_ACP, so a file in any
// other encoding inserted its Korean as mojibake. These use UTF-8-with-BOM and UTF-16LE, whose
// decode does not depend on the system code page, so they pin the behaviour on any machine.
//
// "한글" is U+D55C U+AE00.

TEST(CMemTextTest, FileLoad_Utf8Bom_DecodesKorean)
{
    // EF BB BF | 한(ED 95 9C) 글(EA B8 80)
    std::vector<unsigned char> bytes = {
        0xEF, 0xBB, 0xBF, 0xED, 0x95, 0x9C, 0xEA, 0xB8, 0x80
    };
    CString szPath = MemTextTempPath(_T("cedt_memtext_utf8bom.txt"));
    WriteBytesToFile(szPath, bytes);

    CMemText m;
    ASSERT_TRUE(m.FileLoad(szPath));
    ASSERT_EQ(1, m.GetCount());
    POSITION pos = m.GetHeadPosition();
    EXPECT_STREQ(_T("\xD55C\xAE00"), (LPCTSTR)m.GetNext(pos));

    _tremove(szPath);
}

TEST(CMemTextTest, FileLoad_Utf16LE_DecodesKorean)
{
    // FF FE | 한(5C D5) 글(00 AE)
    std::vector<unsigned char> bytes = {
        0xFF, 0xFE, 0x5C, 0xD5, 0x00, 0xAE
    };
    CString szPath = MemTextTempPath(_T("cedt_memtext_utf16le.txt"));
    WriteBytesToFile(szPath, bytes);

    CMemText m;
    ASSERT_TRUE(m.FileLoad(szPath));
    ASSERT_EQ(1, m.GetCount());
    POSITION pos = m.GetHeadPosition();
    EXPECT_STREQ(_T("\xD55C\xAE00"), (LPCTSTR)m.GetNext(pos));

    _tremove(szPath);
}

TEST(CMemTextTest, FileLoad_Utf8Bom_MultipleLinesAndCrLf)
{
    // BOM, then "한글\r\nabc" — CRLF must be stripped, both lines decoded.
    std::vector<unsigned char> bytes = {
        0xEF, 0xBB, 0xBF,
        0xED, 0x95, 0x9C, 0xEA, 0xB8, 0x80, 0x0D, 0x0A,
        0x61, 0x62, 0x63
    };
    CString szPath = MemTextTempPath(_T("cedt_memtext_utf8bom_2.txt"));
    WriteBytesToFile(szPath, bytes);

    CMemText m;
    ASSERT_TRUE(m.FileLoad(szPath));
    ASSERT_EQ(2, m.GetCount());
    POSITION pos = m.GetHeadPosition();
    EXPECT_STREQ(_T("\xD55C\xAE00"), (LPCTSTR)m.GetNext(pos));
    EXPECT_STREQ(_T("abc"),          (LPCTSTR)m.GetNext(pos));

    _tremove(szPath);
}

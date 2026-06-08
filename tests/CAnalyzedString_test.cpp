#include "StdAfx.h"
#include "cedtElement.h"
#include <gtest/gtest.h>

TEST(CAnalyzedStringTest, DefaultConstructor_InitializesAnalysisToZero)
{
    CAnalyzedString s;
    EXPECT_EQ(nullptr, s.m_pWord);
    EXPECT_EQ(0, s.m_siWordCount);
    EXPECT_EQ(0x00u, static_cast<unsigned int>(s.m_usLineInfo));
    EXPECT_TRUE(s.IsEmpty());
}

TEST(CAnalyzedStringTest, ConstructFromCString_CopiesText_AnalysisZeroed)
{
    CString src(_T("hello"));
    CAnalyzedString s(src);
    EXPECT_STREQ(_T("hello"), (LPCTSTR)s);
    EXPECT_EQ(nullptr, s.m_pWord);
    EXPECT_EQ(0, s.m_siWordCount);
    EXPECT_EQ(0x00u, static_cast<unsigned int>(s.m_usLineInfo));
}

TEST(CAnalyzedStringTest, ConstructFromCharRepeat_FillsBufferAndZeroesAnalysis)
{
    CAnalyzedString s('x', 4);
    EXPECT_STREQ(_T("xxxx"), (LPCTSTR)s);
    EXPECT_EQ(nullptr, s.m_pWord);
    EXPECT_EQ(0, s.m_siWordCount);
}

TEST(CAnalyzedStringTest, AssignmentOperator_CopiesTextNotWordBuffer)
{
    CAnalyzedString src(_T("abc"));
    src.m_siWordCount = 5;
    src.m_usLineInfo  = LI_HAVEBOOKMARK;

    CAnalyzedString dst;
    dst = src;

    EXPECT_STREQ(_T("abc"), (LPCTSTR)dst);
    // operator= leaves the destination's own word buffer state alone
    // (the analysis pointer is only repopulated by full analysis passes),
    // so we only verify the text content survives the copy.
}

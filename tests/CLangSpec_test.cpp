#include "StdAfx.h"
#include "cedtElement.h"
#include <gtest/gtest.h>

TEST(CLangSpecTest, DefaultConstructor_CallsResetContents)
{
    CLangSpec spec;
    // Defaults set by ResetContents:
    EXPECT_TRUE(spec.m_bCaseSensitive[0]);
    EXPECT_TRUE(spec.m_bCaseSensitive[1]);
    EXPECT_TRUE(spec.m_bCaseSensitive[2]);
    EXPECT_FALSE(spec.m_bVariableHighlightInString);
    EXPECT_FALSE(spec.m_bMultiLineStringConstant);
}

TEST(CLangSpecTest, ResetContents_DelimitersToBuiltinDefault)
{
    CLangSpec spec;
    spec.m_szDelimiters = _T("xxx");
    spec.ResetContents();
    // The default delimiter set (note: omits '_')
    EXPECT_STREQ(_T("(){}[]<>+-*/%=\"'~!@#$^&|\\?:;,."),
                 (LPCTSTR)spec.m_szDelimiters);
}

TEST(CLangSpecTest, ResetContents_ClearsCharAndStringMembers)
{
    CLangSpec spec;
    spec.m_chQuotationMark1 = '"';
    spec.m_szLineComment1   = _T("//");
    spec.m_szBlockComment1On  = _T("/*");
    spec.m_szBlockComment1Off = _T("*/");

    spec.ResetContents();

    EXPECT_EQ(0x00, spec.m_chQuotationMark1);
    EXPECT_TRUE(spec.m_szLineComment1.IsEmpty());
    EXPECT_TRUE(spec.m_szBlockComment1On.IsEmpty());
    EXPECT_TRUE(spec.m_szBlockComment1Off.IsEmpty());
}

TEST(CLangSpecTest, ResetContents_RangesToGlobal)
{
    CLangSpec spec;
    spec.m_ucQuotationMark1Range = RT_RANGE1;
    spec.m_ucLineComment1Range   = RT_RANGE2;
    spec.m_ucBlockComment1Range  = RT_R1ORR2;
    spec.ResetContents();
    EXPECT_EQ(RT_GLOBAL, spec.m_ucQuotationMark1Range);
    EXPECT_EQ(RT_GLOBAL, spec.m_ucLineComment1Range);
    EXPECT_EQ(RT_GLOBAL, spec.m_ucBlockComment1Range);
}

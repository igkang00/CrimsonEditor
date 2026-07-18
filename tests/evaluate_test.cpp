#include "StdAfx.h"
#include "evaluate.h"
#include <gtest/gtest.h>

namespace {

double Eval(const TCHAR * expr, INT expectedError = EVAL_ERROR_SUCCESSFUL)
{
    double value = 0.0;
    INT error = 0;
    EVAL::Evaluate(const_cast<TCHAR *>(expr), &value, &error);
    EXPECT_EQ(expectedError, error) << "expression: " << expr;
    return value;
}

} // namespace

TEST(EvaluateTest, BasicArithmetic_AddSub)
{
    EXPECT_DOUBLE_EQ(3.0, Eval(_T("1 + 2")));
    EXPECT_DOUBLE_EQ(-1.0, Eval(_T("2 - 3")));
}

TEST(EvaluateTest, BasicArithmetic_MulDiv)
{
    EXPECT_DOUBLE_EQ(12.0, Eval(_T("3 * 4")));
    EXPECT_DOUBLE_EQ(5.0, Eval(_T("10 / 2")));
}

TEST(EvaluateTest, OperatorPrecedence)
{
    EXPECT_DOUBLE_EQ(14.0, Eval(_T("2 + 3 * 4")));
    EXPECT_DOUBLE_EQ(20.0, Eval(_T("(2 + 3) * 4")));
}

TEST(EvaluateTest, BuiltinFunctions_AbsMinMax)
{
    EXPECT_DOUBLE_EQ(5.0, Eval(_T("abs(-5)")));
    EXPECT_DOUBLE_EQ(3.0, Eval(_T("min(3, 7)")));
    EXPECT_DOUBLE_EQ(7.0, Eval(_T("max(3, 7)")));
}

TEST(EvaluateTest, UnknownFunction_ReturnsError)
{
    double value = 0.0;
    INT error = 0;
    EVAL::Evaluate(const_cast<TCHAR *>(_T("nonexistent_func(1)")), &value, &error);
    EXPECT_EQ(EVAL_ERROR_FUNCTION_NOT_DEFINED, error);
}

// A single token longer than the evaluator's 2048-char scratch buffer must not overrun the
// stack (it used to crash). It is now reported as an error rather than truncated into a wrong
// value, so the caller can beep and show the error.
TEST(EvaluateTest, OverLongNumberToken_ReportsErrorNotOverrun)
{
    CString s = _T("1+");
    for (int i = 0; i < 5000; i++) s += _T("9");
    double value = 0.0; INT error = 0;
    EVAL::Evaluate(const_cast<TCHAR *>((LPCTSTR)s), &value, &error);
    EXPECT_EQ(EVAL_ERROR_TOKEN_TOO_LONG, error);
}

TEST(EvaluateTest, OverLongVariableToken_ReportsErrorNotOverrun)
{
    CString s = _T("$");
    for (int i = 0; i < 5000; i++) s += _T("a");
    double value = 0.0; INT error = 0;
    EVAL::Evaluate(const_cast<TCHAR *>((LPCTSTR)s), &value, &error);
    EXPECT_EQ(EVAL_ERROR_TOKEN_TOO_LONG, error);
}

TEST(EvaluateTest, OverLongFunctionToken_ReportsErrorNotOverrun)
{
    CString s;
    for (int i = 0; i < 5000; i++) s += _T("a");
    s += _T("(1)");
    double value = 0.0; INT error = 0;
    EVAL::Evaluate(const_cast<TCHAR *>((LPCTSTR)s), &value, &error);
    EXPECT_EQ(EVAL_ERROR_TOKEN_TOO_LONG, error);
}

// A long expression must be read in full, not truncated at any scratch-buffer size — a wrong
// sum would reveal silent truncation (the §A7 concern).
TEST(EvaluateTest, LongSumEvaluatesFully)
{
    CString s;
    for (int i = 0; i < 2000; i++) { if (i) s += _T("+"); s += _T("1"); }
    double value = 0.0; INT error = 0;
    EVAL::Evaluate(const_cast<TCHAR *>((LPCTSTR)s), &value, &error);
    EXPECT_EQ(EVAL_ERROR_SUCCESSFUL, error);
    EXPECT_DOUBLE_EQ(2000.0, value);
}

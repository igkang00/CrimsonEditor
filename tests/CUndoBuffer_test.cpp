#include "StdAfx.h"
#include "cedtElement.h"
#include <gtest/gtest.h>

TEST(CUndoBufferTest, FreshlyConstructed_IsEmpty)
{
    CUndoBuffer ub;
    EXPECT_EQ(0, ub.GetCount());
    EXPECT_EQ(0, ub.m_lstAction.GetCount());
    EXPECT_EQ(0, ub.m_lstIdxX.GetCount());
    EXPECT_EQ(0, ub.m_lstIdxY.GetCount());
    EXPECT_EQ(0, ub.m_lstParam.GetCount());
    EXPECT_EQ(0, ub.m_lstChar.GetCount());
    EXPECT_EQ(0, ub.m_lstString.GetCount());
    EXPECT_EQ(0, ub.m_lstBlock.GetCount());
}

TEST(CUndoBufferTest, EmptyBuffer_ClearsAllLists)
{
    CUndoBuffer ub;
    ub.AddTail(AT_INSERTCHAR);
    ub.m_lstAction.AddTail(AT_INSERTCHAR);
    ub.m_lstIdxX.AddTail(5);
    ub.m_lstIdxY.AddTail(7);
    ub.m_lstParam.AddTail(0);
    ub.m_lstChar.AddTail('A');
    ub.m_lstString.AddTail(CString(_T("hello")));

    ub.EmptyBuffer();

    EXPECT_EQ(0, ub.GetCount());
    EXPECT_EQ(0, ub.m_lstAction.GetCount());
    EXPECT_EQ(0, ub.m_lstIdxX.GetCount());
    EXPECT_EQ(0, ub.m_lstIdxY.GetCount());
    EXPECT_EQ(0, ub.m_lstParam.GetCount());
    EXPECT_EQ(0, ub.m_lstChar.GetCount());
    EXPECT_EQ(0, ub.m_lstString.GetCount());
    EXPECT_EQ(0, ub.m_lstBlock.GetCount());
}

TEST(CUndoBufferTest, GetRecentIndex_ReturnsHeadValues)
{
    CUndoBuffer ub;
    // GetRecentIndex returns early if the base list is empty,
    // so we add one item there as well.
    ub.AddTail(0);
    ub.m_lstIdxX.AddHead(10); // newest action is pushed to the head
    ub.m_lstIdxY.AddHead(30);

    INT x = -1, y = -1;
    ub.GetRecentIndex(x, y);
    EXPECT_EQ(10, x);
    EXPECT_EQ(30, y);
}

TEST(CUndoBufferTest, GetRecentIndex_OnEmptyBuffer_LeavesArgsUntouched)
{
    CUndoBuffer ub;
    INT x = 42, y = 99;
    ub.GetRecentIndex(x, y);
    EXPECT_EQ(42, x);
    EXPECT_EQ(99, y);
}

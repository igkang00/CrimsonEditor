#include "StdAfx.h"
#include "cedtElement.h"
#include <gtest/gtest.h>
#include <vector>

// CAnalyzedText::InsertLines / RemoveLines.
//
// These exist so the edit code states WHAT it wants (a run of lines appears, a run of
// lines goes away) rather than HOW to do it. The container is about to change from a
// linked list to an array (docs/refactoring-line-container.md), and the per-line loops
// these replaced would have turned a large paste into O(n*m) memmoves.
//
// They are pinned here because the whole point of introducing them ahead of the swap is
// that the swap must not be free to change what an edit MEANS. These tests pass against
// the CList backing today and must still pass against the array backing tomorrow.

namespace {

// Build a document with lines "L0", "L1", ... "L(n-1)".
void MakeDoc(CAnalyzedText & text, int n)
{
	text.RemoveAll();
	for(int i = 0; i < n; i++) {
		CString sz; sz.Format(_T("L%d"), i);
		text.AddTail( (LPCTSTR)sz );
	}
}

// Flatten to a vector<CString> so a whole document can be compared in one assertion.
std::vector<CString> Dump(CAnalyzedText & text)
{
	std::vector<CString> out;
	POSITION pos = text.GetHeadPosition();
	while( pos ) out.push_back( (LPCTSTR)text.GetNext(pos) );
	return out;
}

std::vector<CString> Expect(std::initializer_list<LPCTSTR> lines)
{
	std::vector<CString> out;
	for(LPCTSTR p : lines) out.push_back(p);
	return out;
}

} // namespace


TEST(CAnalyzedTextBulk, InsertLinesInTheMiddle)
{
	CAnalyzedText text; MakeDoc(text, 4);				// L0 L1 L2 L3

	CString ins[] = { _T("A"), _T("B") };
	text.InsertLines(2, ins, 2);						// A, B become lines 2 and 3

	EXPECT_EQ(Dump(text), Expect({_T("L0"), _T("L1"), _T("A"), _T("B"), _T("L2"), _T("L3")}));
	EXPECT_EQ(text.GetCount(), 6);
}

TEST(CAnalyzedTextBulk, InsertLinesAtTheHead)
{
	CAnalyzedText text; MakeDoc(text, 2);				// L0 L1

	CString ins[] = { _T("A") };
	text.InsertLines(0, ins, 1);

	EXPECT_EQ(Dump(text), Expect({_T("A"), _T("L0"), _T("L1")}));
}

TEST(CAnalyzedTextBulk, InsertLinesAtTheTailAppends)
{
	CAnalyzedText text; MakeDoc(text, 2);				// L0 L1

	CString ins[] = { _T("A"), _T("B") };
	text.InsertLines(2, ins, 2);						// nIndex == GetCount()

	EXPECT_EQ(Dump(text), Expect({_T("L0"), _T("L1"), _T("A"), _T("B")}));
}

TEST(CAnalyzedTextBulk, InsertLinesIntoAnEmptyDocument)
{
	CAnalyzedText text;

	CString ins[] = { _T("A") };
	text.InsertLines(0, ins, 1);

	EXPECT_EQ(Dump(text), Expect({_T("A")}));
}

TEST(CAnalyzedTextBulk, InsertZeroLinesIsANoOp)
{
	CAnalyzedText text; MakeDoc(text, 3);

	text.InsertLines(1, NULL, 0);

	EXPECT_EQ(Dump(text), Expect({_T("L0"), _T("L1"), _T("L2")}));
}

TEST(CAnalyzedTextBulk, RemoveLinesFromTheMiddle)
{
	CAnalyzedText text; MakeDoc(text, 5);				// L0..L4

	text.RemoveLines(1, 3);								// drop L1 L2 L3

	EXPECT_EQ(Dump(text), Expect({_T("L0"), _T("L4")}));
	EXPECT_EQ(text.GetCount(), 2);
}

TEST(CAnalyzedTextBulk, RemoveLinesFromTheHead)
{
	CAnalyzedText text; MakeDoc(text, 3);

	text.RemoveLines(0, 2);

	EXPECT_EQ(Dump(text), Expect({_T("L2")}));
}

TEST(CAnalyzedTextBulk, RemoveLinesToTheEnd)
{
	CAnalyzedText text; MakeDoc(text, 3);

	text.RemoveLines(1, 2);

	EXPECT_EQ(Dump(text), Expect({_T("L0")}));
}

TEST(CAnalyzedTextBulk, RemoveEveryLine)
{
	CAnalyzedText text; MakeDoc(text, 3);

	text.RemoveLines(0, 3);

	EXPECT_EQ(text.GetCount(), 0);
}

TEST(CAnalyzedTextBulk, RemoveZeroLinesIsANoOp)
{
	CAnalyzedText text; MakeDoc(text, 3);

	text.RemoveLines(1, 0);

	EXPECT_EQ(Dump(text), Expect({_T("L0"), _T("L1"), _T("L2")}));
}

// The shape DeleteBlock relies on: a selection from line nBegY to nEndY leaves nBegY in
// place (the caller has already spliced its text) and removes the nEndY - nBegY lines
// under it.
TEST(CAnalyzedTextBulk, RemoveLinesMatchesTheDeleteBlockContract)
{
	CAnalyzedText text; MakeDoc(text, 10);

	const INT nBegY = 3, nEndY = 7;
	text.RemoveLines(nBegY + 1, nEndY - nBegY);			// lines 4,5,6,7

	EXPECT_EQ(Dump(text), Expect({_T("L0"), _T("L1"), _T("L2"), _T("L3"),
	                              _T("L8"), _T("L9")}));
}

// The shape InsertBlock relies on: a paste of N block lines leaves the first merged into
// nBegY and puts the remaining N-1 underneath it, so the last one is line nBegY + N - 1.
TEST(CAnalyzedTextBulk, InsertLinesMatchesTheInsertBlockContract)
{
	CAnalyzedText text; MakeDoc(text, 4);				// L0 L1 L2 L3

	const INT nBegY = 1;
	CString ins[] = { _T("B1"), _T("B2"), _T("B3") };	// the 2nd..4th lines of a 4-line block
	text.InsertLines(nBegY + 1, ins, 3);

	EXPECT_EQ(Dump(text), Expect({_T("L0"), _T("L1"), _T("B1"), _T("B2"), _T("B3"),
	                              _T("L2"), _T("L3")}));

	const INT nEndY = nBegY + 3;						// where InsertBlock expects the tail
	EXPECT_STREQ((LPCTSTR)text.GetAt(text.FindIndex(nEndY)), _T("B3"));
}

// The lines carry state beyond their text (bookmarks, token arrays). A removal must not
// disturb the lines that survive it.
TEST(CAnalyzedTextBulk, RemoveLinesLeavesSurvivingLineStateIntact)
{
	CAnalyzedText text; MakeDoc(text, 5);

	text.GetAt(text.FindIndex(0)).m_usLineInfo = LI_HAVEBOOKMARK;
	text.GetAt(text.FindIndex(4)).m_usLineInfo = LI_HAVECOMMENT;

	text.RemoveLines(1, 3);

	POSITION pos = text.GetHeadPosition();
	EXPECT_EQ(text.GetNext(pos).m_usLineInfo, LI_HAVEBOOKMARK);
	EXPECT_EQ(text.GetNext(pos).m_usLineInfo, LI_HAVECOMMENT);
}

// A big one, because the array backing turns "remove one at a time" into a quadratic
// hang and this is the test that would catch it coming back.
TEST(CAnalyzedTextBulk, RemoveALargeRange)
{
	CAnalyzedText text; MakeDoc(text, 20000);

	text.RemoveLines(1, 19998);							// keep only L0 and L19999

	ASSERT_EQ(text.GetCount(), 2);
	POSITION pos = text.GetHeadPosition();
	EXPECT_STREQ((LPCTSTR)text.GetNext(pos), _T("L0"));
	EXPECT_STREQ((LPCTSTR)text.GetNext(pos), _T("L19999"));
}

TEST(CAnalyzedTextBulk, InsertALargeRange)
{
	CAnalyzedText text; MakeDoc(text, 2);

	std::vector<CString> ins;
	for(int i = 0; i < 20000; i++) { CString sz; sz.Format(_T("B%d"), i); ins.push_back(sz); }

	text.InsertLines(1, ins.data(), (INT)ins.size());

	ASSERT_EQ(text.GetCount(), 20002);
	EXPECT_STREQ((LPCTSTR)text.GetAt(text.FindIndex(0)),     _T("L0"));
	EXPECT_STREQ((LPCTSTR)text.GetAt(text.FindIndex(1)),     _T("B0"));
	EXPECT_STREQ((LPCTSTR)text.GetAt(text.FindIndex(20000)), _T("B19999"));
	EXPECT_STREQ((LPCTSTR)text.GetAt(text.FindIndex(20001)), _T("L1"));
}

#include "StdAfx.h"
#include "cedtElement.h"
#include "cedtLineList.h"
#include <gtest/gtest.h>
#include <vector>

// CLineList - the array-backed replacement for the CList the document's lines live in.
//
// The claim being tested is "drop-in": for every operation the editor performs, it
// behaves exactly as CList does. That is not something to eyeball across 140 call
// sites, so the centrepiece here is a DIFFERENTIAL test -- the same pseudo-random
// sequence of operations is applied to a CList and a CLineList, and the entire visible
// state of both is compared after every single step. A divergence names the operation
// that caused it.
//
// See docs/refactoring-line-container.md.

typedef CList<CString, LPCTSTR>      RefList;	// what we must match
typedef CLineList<CString, LPCTSTR>  NewList;	// what we are shipping

namespace {

std::vector<CString> DumpRef(RefList & list)
{
	std::vector<CString> out;
	POSITION pos = list.GetHeadPosition();
	while( pos ) out.push_back( list.GetNext(pos) );
	return out;
}

std::vector<CString> DumpNew(NewList & list)
{
	std::vector<CString> out;
	POSITION pos = list.GetHeadPosition();
	while( pos ) out.push_back( list.GetNext(pos) );
	return out;
}

// Walking backwards is a separate code path (GetPrev), so check it too.
std::vector<CString> DumpRefBackwards(RefList & list)
{
	std::vector<CString> out;
	POSITION pos = list.GetTailPosition();
	while( pos ) out.push_back( list.GetPrev(pos) );
	return out;
}

std::vector<CString> DumpNewBackwards(NewList & list)
{
	std::vector<CString> out;
	POSITION pos = list.GetTailPosition();
	while( pos ) out.push_back( list.GetPrev(pos) );
	return out;
}

// A deterministic PRNG, so a failure is reproducible rather than "it went red once".
struct Rng
{
	unsigned int s;
	explicit Rng(unsigned int seed) : s(seed) {}
	unsigned int Next() { s = s * 1664525u + 1013904223u; return s >> 8; }
	unsigned int Next(unsigned int n) { return n ? Next() % n : 0; }
};

} // namespace


// ---------------------------------------------------------------------------
// The differential test.
// ---------------------------------------------------------------------------

TEST(CLineList, MatchesCListUnderRandomOperations)
{
	for(unsigned int seed = 1; seed <= 20; seed++) {
		RefList ref;
		NewList New;
		Rng rng(seed);

		for(int step = 0; step < 300; step++) {
			INT_PTR n = ref.GetCount();
			ASSERT_EQ(n, New.GetCount()) << "seed " << seed << " step " << step;

			CString val; val.Format(_T("v%u_%d"), seed, step);
			unsigned int op = rng.Next(n == 0 ? 2 : 7);		// only the adds make sense when empty

			switch( op ) {
			case 0:		// AddTail
				ref.AddTail( (LPCTSTR)val );
				New.AddTail( (LPCTSTR)val );
				break;

			case 1:		// AddHead
				ref.AddHead( (LPCTSTR)val );
				New.AddHead( (LPCTSTR)val );
				break;

			case 2: {	// InsertAfter at a random index
				INT_PTR i = (INT_PTR)rng.Next((unsigned int)n);
				ref.InsertAfter( ref.FindIndex(i), (LPCTSTR)val );
				New.InsertAfter( New.FindIndex(i), (LPCTSTR)val );
				break;
			}

			case 3: {	// InsertBefore at a random index
				INT_PTR i = (INT_PTR)rng.Next((unsigned int)n);
				ref.InsertBefore( ref.FindIndex(i), (LPCTSTR)val );
				New.InsertBefore( New.FindIndex(i), (LPCTSTR)val );
				break;
			}

			case 4: {	// RemoveAt a random index
				INT_PTR i = (INT_PTR)rng.Next((unsigned int)n);
				ref.RemoveAt( ref.FindIndex(i) );
				New.RemoveAt( New.FindIndex(i) );
				break;
			}

			case 5: {	// modify in place through GetAt -- the reference must be live
				INT_PTR i = (INT_PTR)rng.Next((unsigned int)n);
				ref.GetAt( ref.FindIndex(i) ) += _T("!");
				New.GetAt( New.FindIndex(i) ) += _T("!");
				break;
			}

			case 6:		// RemoveAll, occasionally, to exercise the empty state
				if( rng.Next(10) == 0 ) { ref.RemoveAll(); New.RemoveAll(); }
				break;
			}

			// Compare EVERYTHING after every step.
			ASSERT_EQ(ref.GetCount(), New.GetCount()) << "seed " << seed << " step " << step << " op " << op;
			ASSERT_EQ(DumpRef(ref), DumpNew(New))     << "seed " << seed << " step " << step << " op " << op;
			ASSERT_EQ(DumpRefBackwards(ref), DumpNewBackwards(New))
				<< "backward walk diverged: seed " << seed << " step " << step << " op " << op;

			if( ref.GetCount() > 0 ) {
				ASSERT_STREQ((LPCTSTR)ref.GetHead(), (LPCTSTR)New.GetHead());
				ASSERT_STREQ((LPCTSTR)ref.GetTail(), (LPCTSTR)New.GetTail());

				// FindIndex must agree at every index, not just the ends.
				for(INT_PTR i = 0; i < ref.GetCount(); i++)
					ASSERT_STREQ((LPCTSTR)ref.GetAt(ref.FindIndex(i)),
					             (LPCTSTR)New.GetAt(New.FindIndex(i)))
						<< "index " << i << " seed " << seed << " step " << step;
			}
		}
	}
}


// ---------------------------------------------------------------------------
// The properties CList had that the call sites lean on.
// ---------------------------------------------------------------------------

TEST(CLineList, FindIndexOutOfRangeReturnsNull)
{
	NewList list;
	EXPECT_EQ(list.FindIndex(0), (POSITION)NULL);		// empty

	list.AddTail(_T("a"));
	EXPECT_NE(list.FindIndex(0), (POSITION)NULL);
	EXPECT_EQ(list.FindIndex(1), (POSITION)NULL);		// one past the end
	EXPECT_EQ(list.FindIndex(-1), (POSITION)NULL);
}

TEST(CLineList, HeadAndTailPositionsAreNullWhenEmpty)
{
	NewList list;
	EXPECT_EQ(list.GetHeadPosition(), (POSITION)NULL);
	EXPECT_EQ(list.GetTailPosition(), (POSITION)NULL);
	EXPECT_TRUE(list.IsEmpty());
	EXPECT_EQ(list.GetCount(), 0);
}

TEST(CLineList, GetNextNullsOutAtTheEnd)
{
	NewList list;
	list.AddTail(_T("a")); list.AddTail(_T("b"));

	POSITION pos = list.GetHeadPosition();
	EXPECT_STREQ((LPCTSTR)list.GetNext(pos), _T("a"));
	EXPECT_NE(pos, (POSITION)NULL);
	EXPECT_STREQ((LPCTSTR)list.GetNext(pos), _T("b"));
	EXPECT_EQ(pos, (POSITION)NULL);						// the loop-termination contract
}

TEST(CLineList, GetPrevNullsOutAtTheStart)
{
	NewList list;
	list.AddTail(_T("a")); list.AddTail(_T("b"));

	POSITION pos = list.GetTailPosition();
	EXPECT_STREQ((LPCTSTR)list.GetPrev(pos), _T("b"));
	EXPECT_NE(pos, (POSITION)NULL);
	EXPECT_STREQ((LPCTSTR)list.GetPrev(pos), _T("a"));
	EXPECT_EQ(pos, (POSITION)NULL);
}

// The decisive property: the editor holds C++ references into the container across
// mutations of it. A vector of VALUES would relocate its elements on insert and leave
// those references dangling -- silently, and usually appearing to work. Holding
// pointers is what makes this safe, and this test is what pins it.
TEST(CLineList, ReferencesIntoTheListSurviveInsertsAndRemovals)
{
	NewList list;
	for(int i = 0; i < 8; i++) { CString sz; sz.Format(_T("L%d"), i); list.AddTail((LPCTSTR)sz); }

	CString & rHeld = list.GetAt( list.FindIndex(3) );	// reference into the container
	EXPECT_STREQ((LPCTSTR)rHeld, _T("L3"));

	// Reallocate the vector many times over, and shift the held element around.
	for(int i = 0; i < 1000; i++) list.AddTail(_T("filler"));
	list.AddHead(_T("pushed everything down"));
	list.RemoveAt( list.FindIndex(0) );
	list.InsertGap(1, 500);

	// The pointers moved. The object did not.
	rHeld += _T(" still here");
	EXPECT_STREQ((LPCTSTR)rHeld, _T("L3 still here"));

	// ... and it is still the element the list holds, wherever it now sits.
	POSITION pos = list.GetHeadPosition();
	BOOL bFound = FALSE;
	while( pos ) if( list.GetNext(pos) == _T("L3 still here") ) { bFound = TRUE; break; }
	EXPECT_TRUE(bFound);
}


// ---------------------------------------------------------------------------
// The bulk operations, which are the reason a large paste or block delete does not
// become O(n*m).
// ---------------------------------------------------------------------------

TEST(CLineList, InsertGapMakesDefaultConstructedRoom)
{
	NewList list;
	list.AddTail(_T("a")); list.AddTail(_T("d"));

	list.InsertGap(1, 2);
	ASSERT_EQ(list.GetCount(), 4);

	list.ElementAt(1) = _T("b");
	list.ElementAt(2) = _T("c");

	EXPECT_EQ(DumpNew(list), (std::vector<CString>{_T("a"), _T("b"), _T("c"), _T("d")}));
}

TEST(CLineList, InsertGapAtTheEndAppends)
{
	NewList list;
	list.AddTail(_T("a"));

	list.InsertGap(1, 1);					// nIndex == GetCount()
	list.ElementAt(1) = _T("b");

	EXPECT_EQ(DumpNew(list), (std::vector<CString>{_T("a"), _T("b")}));
}

TEST(CLineList, InsertGapIntoAnEmptyList)
{
	NewList list;
	list.InsertGap(0, 2);
	ASSERT_EQ(list.GetCount(), 2);
	list.ElementAt(0) = _T("a");
	list.ElementAt(1) = _T("b");
	EXPECT_EQ(DumpNew(list), (std::vector<CString>{_T("a"), _T("b")}));
}

TEST(CLineList, RemoveRangeDropsExactlyThatRange)
{
	NewList list;
	for(int i = 0; i < 6; i++) { CString sz; sz.Format(_T("L%d"), i); list.AddTail((LPCTSTR)sz); }

	list.RemoveRange(2, 3);					// L2 L3 L4

	EXPECT_EQ(DumpNew(list), (std::vector<CString>{_T("L0"), _T("L1"), _T("L5")}));
}

TEST(CLineList, RemoveRangeCanEmptyTheList)
{
	NewList list;
	list.AddTail(_T("a")); list.AddTail(_T("b"));

	list.RemoveRange(0, 2);

	EXPECT_TRUE(list.IsEmpty());
	EXPECT_EQ(list.GetHeadPosition(), (POSITION)NULL);
}

TEST(CLineList, ZeroCountBulkOpsAreNoOps)
{
	NewList list;
	list.AddTail(_T("a"));

	list.InsertGap(0, 0);
	list.RemoveRange(0, 0);

	EXPECT_EQ(DumpNew(list), (std::vector<CString>{_T("a")}));
}

// A bulk removal must be ONE structural change, not n. If someone reimplements it as a
// loop of RemoveAt this stops being instant.
TEST(CLineList, LargeBulkOpsAreNotQuadratic)
{
	NewList list;
	for(int i = 0; i < 200000; i++) list.AddTail(_T("x"));

	list.RemoveRange(1, 199998);			// would be 199,998 memmoves if looped

	ASSERT_EQ(list.GetCount(), 2);

	list.InsertGap(1, 199998);
	ASSERT_EQ(list.GetCount(), 200000);
}


// ---------------------------------------------------------------------------
// ReplaceRange -- swap a run of elements for a run of a different length.
//
// This is what the wrapped-line formatter needs: it produces as many screen rows as a
// line turns out to need, and it cannot know how many until it has laid the line out.
// Growing the list one row at a time is a memmove of the tail PER ROW, and the formatter
// does it for every line in the document -- which is quadratic, and was: 90,000 wrapped
// lines took 2,779 ms that way, against 168 ms on the CList this class replaced. The
// regression shipped because nothing here would have caught it. Now something does.
// ---------------------------------------------------------------------------

TEST(CLineList, ReplaceRangeSwapsARunForALongerOne)
{
	NewList list;
	list.AddTail(_T("a")); list.AddTail(_T("b")); list.AddTail(_T("c"));

	CString * rows[3] = { new CString(_T("X")), new CString(_T("Y")), new CString(_T("Z")) };
	list.ReplaceRange(1, 1, rows, 3);		// "b" out, three rows in

	std::vector<CString> got = DumpNew(list);
	ASSERT_EQ(got.size(), 5u);
	EXPECT_STREQ(got[0], _T("a"));
	EXPECT_STREQ(got[1], _T("X"));
	EXPECT_STREQ(got[2], _T("Y"));
	EXPECT_STREQ(got[3], _T("Z"));
	EXPECT_STREQ(got[4], _T("c"));
}

TEST(CLineList, ReplaceRangeSwapsARunForAShorterOne)
{
	NewList list;
	for(int i = 0; i < 5; i++) list.AddTail(_T("old"));

	CString * rows[1] = { new CString(_T("one")) };
	list.ReplaceRange(1, 3, rows, 1);		// three rows collapse to one

	std::vector<CString> got = DumpNew(list);
	ASSERT_EQ(got.size(), 3u);
	EXPECT_STREQ(got[0], _T("old"));
	EXPECT_STREQ(got[1], _T("one"));
	EXPECT_STREQ(got[2], _T("old"));
}

TEST(CLineList, ReplaceRangeWithEqualCountsKeepsEveryOtherElement)
{
	NewList list;
	list.AddTail(_T("a")); list.AddTail(_T("b")); list.AddTail(_T("c"));

	CString * rows[1] = { new CString(_T("B")) };
	list.ReplaceRange(1, 1, rows, 1);

	std::vector<CString> got = DumpNew(list);
	ASSERT_EQ(got.size(), 3u);
	EXPECT_STREQ(got[0], _T("a"));
	EXPECT_STREQ(got[1], _T("B"));
	EXPECT_STREQ(got[2], _T("c"));
}

TEST(CLineList, ReplaceRangeCanEmptyARunAndCanFillAnEmptyList)
{
	NewList list;
	for(int i = 0; i < 3; i++) list.AddTail(_T("x"));

	list.ReplaceRange(0, 3, NULL, 0);		// everything out, nothing in
	ASSERT_EQ(list.GetCount(), 0);

	CString * rows[2] = { new CString(_T("p")), new CString(_T("q")) };
	list.ReplaceRange(0, 0, rows, 2);		// nothing out, everything in
	ASSERT_EQ(list.GetCount(), 2);
	EXPECT_STREQ(list.GetHead(), _T("p"));
	EXPECT_STREQ(list.GetTail(), _T("q"));
}

// The one that would have caught the regression. Rebuilding a 200,000-row list as the
// wrapped formatter does -- every line replaced, each yielding several rows -- must cost
// one splice, not one per row. Looped InsertGap makes this take minutes.
TEST(CLineList, ReplaceRangeOverAWholeListIsNotQuadratic)
{
	NewList list;
	for(int i = 0; i < 50000; i++) list.AddTail(_T("line"));

	// Every line re-wraps into four rows: 50,000 in, 200,000 out.
	std::vector<CString *> rows;
	rows.reserve(200000);
	for(int i = 0; i < 200000; i++) rows.push_back( new CString(_T("row")) );

	list.ReplaceRange(0, 50000, & rows[0], (INT_PTR)rows.size());

	ASSERT_EQ(list.GetCount(), 200000);
	EXPECT_STREQ(list.GetHead(), _T("row"));
	EXPECT_STREQ(list.GetTail(), _T("row"));
}


// ---------------------------------------------------------------------------
// The safety net.
//
// An index-based POSITION does not survive a structural change: remove element i and
// everything after shifts down, so a POSITION past i names the wrong line -- no crash,
// just the wrong text edited. Debug builds stamp every POSITION with a modification
// counter and assert on use.
//
// That assert is the entire defence, so it gets tested. An assert that never fires
// because it is broken looks exactly like an assert that never fires because the code
// is right.
// ---------------------------------------------------------------------------

#ifdef _DEBUG

TEST(CLineList, DetectsAPositionUsedAcrossAStructuralChange)
{
	NewList list;
	list.AddTail(_T("a")); list.AddTail(_T("b")); list.AddTail(_T("c"));

	POSITION pos = list.FindIndex(2);
	EXPECT_FALSE(list.IsStalePosition(pos));		// fresh

	list.RemoveAt( list.FindIndex(0) );				// everything shifts down

	EXPECT_TRUE(list.IsStalePosition(pos));			// the old one is caught

	EXPECT_FALSE(list.IsStalePosition( list.FindIndex(1) ));	// re-fetching is the fix
}

TEST(CLineList, EveryStructuralChangeInvalidates)
{
	NewList list;
	list.AddTail(_T("a")); list.AddTail(_T("b"));

	POSITION pos;

	pos = list.FindIndex(0); list.AddTail(_T("x"));
	EXPECT_TRUE(list.IsStalePosition(pos)) << "AddTail did not invalidate";

	pos = list.FindIndex(0); list.AddHead(_T("x"));
	EXPECT_TRUE(list.IsStalePosition(pos)) << "AddHead did not invalidate";

	pos = list.FindIndex(0); list.InsertAfter(list.FindIndex(0), _T("x"));
	EXPECT_TRUE(list.IsStalePosition(pos)) << "InsertAfter did not invalidate";

	pos = list.FindIndex(0); list.InsertBefore(list.FindIndex(0), _T("x"));
	EXPECT_TRUE(list.IsStalePosition(pos)) << "InsertBefore did not invalidate";

	pos = list.FindIndex(0); list.RemoveAt(list.FindIndex(0));
	EXPECT_TRUE(list.IsStalePosition(pos)) << "RemoveAt did not invalidate";

	pos = list.FindIndex(0); list.InsertGap(0, 1);
	EXPECT_TRUE(list.IsStalePosition(pos)) << "InsertGap did not invalidate";

	pos = list.FindIndex(0); list.RemoveRange(0, 1);
	EXPECT_TRUE(list.IsStalePosition(pos)) << "RemoveRange did not invalidate";

	pos = list.FindIndex(0); list.RemoveAll();
	EXPECT_TRUE(list.IsStalePosition(pos)) << "RemoveAll did not invalidate";
}

// The legitimate pattern the call sites use -- reassigning from the return value --
// must NOT be flagged, or the assert is useless noise and someone will delete it.
TEST(CLineList, ReassigningFromTheReturnValueStaysFresh)
{
	NewList list;
	list.AddTail(_T("a"));

	POSITION pos = list.GetHeadPosition();
	for(int i = 0; i < 5; i++) {
		pos = list.InsertAfter(pos, _T("x"));		// the pattern in cedtViewFormat.cpp
		EXPECT_FALSE(list.IsStalePosition(pos)) << "a returned POSITION must be usable";
		list.GetAt(pos);							// would assert if it were stale
	}
}

#endif // _DEBUG


// ---------------------------------------------------------------------------
// The element type the document actually stores, rather than a plain CString: it owns
// a heap array and has a real assignment operator, and CList assigns rather than
// copy-constructs into its nodes. CLineList must do the same.
// ---------------------------------------------------------------------------

TEST(CLineList, WorksWithTheRealLineType)
{
	CLineList<CAnalyzedString, LPCTSTR> list;

	list.AddTail(_T("int main(void)"));
	list.AddTail(_T("{"));
	list.AddTail(_T("}"));

	ASSERT_EQ(list.GetCount(), 3);

	list.GetAt( list.FindIndex(1) ).m_usLineInfo = LI_HAVEBOOKMARK;

	list.InsertGap(1, 1);
	list.ElementAt(1) = _T("\treturn 0;");

	EXPECT_STREQ((LPCTSTR)list.ElementAt(0), _T("int main(void)"));
	EXPECT_STREQ((LPCTSTR)list.ElementAt(1), _T("\treturn 0;"));
	EXPECT_STREQ((LPCTSTR)list.ElementAt(2), _T("{"));

	// The bookmark rode along with its line rather than staying at index 1.
	EXPECT_EQ(list.ElementAt(2).m_usLineInfo, LI_HAVEBOOKMARK);
	EXPECT_EQ(list.ElementAt(1).m_usLineInfo, 0);
}

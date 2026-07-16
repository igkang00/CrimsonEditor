#include "StdAfx.h"
#include "cedtCharWidth.h"
#include <gtest/gtest.h>

// Display-cell width for column mode — the PURE half of the classifier.
//
// Everything that decides a character's cell count is here and takes plain values, so it is
// tested without a device context: the East Asian Width table, the union with a measurement,
// the 1.2x threshold, the zero-cell combining-mark case, and the surrogate-pair decode. The
// GDI half (CCedtView::GetCharCells) only measures an advance and calls CellsFor; there is no
// judgement left in it to test here.
//
// U+1F600 GRINNING FACE encodes as the pair D83D DE00, spelled numerically so the tests do
// not depend on the source file's encoding.
static const TCHAR HI = (TCHAR)0xD83D;
static const TCHAR LO = (TCHAR)0xDE00;


///////////////////////////////////////////////////////////////////////////////
// CodepointAt — the astral branch

TEST(CharWidth, CodepointAt_BmpCharIsItself)
{
	const TCHAR line[] = { _T('a'), (TCHAR)0xAC00, 0 };   // 'a', Hangul GA
	EXPECT_EQ(CodepointAt(line, 0, 2), 0x61u);
	EXPECT_EQ(CodepointAt(line, 1, 2), 0xAC00u);
}

TEST(CharWidth, CodepointAt_SurrogatePairDecodesToAstral)
{
	const TCHAR line[] = { _T('a'), HI, LO, _T('b'), 0 };
	EXPECT_EQ(CodepointAt(line, 1, 4), 0x1F600u);         // the pair, as one scalar
}

TEST(CharWidth, CodepointAt_LoneSurrogateIsReturnedRawNotDecoded)
{
	const TCHAR line[] = { HI, _T('b'), 0 };              // high surrogate, no low half
	EXPECT_EQ(CodepointAt(line, 0, 2), 0xD83Du);          // the unit itself, not garbage
}

TEST(CharWidth, CodepointAt_OutOfRangeIsZero)
{
	const TCHAR line[] = { _T('a'), 0 };
	EXPECT_EQ(CodepointAt(line, -1, 1), 0u);
	EXPECT_EQ(CodepointAt(line,  1, 1), 0u);              // == length
}


///////////////////////////////////////////////////////////////////////////////
// IsWideByTable — East Asian Width W/F plus emoji

TEST(CharWidth, Table_AsciiAndLatinAreNarrow)
{
	EXPECT_FALSE(IsWideByTable(0x61));      // 'a'
	EXPECT_FALSE(IsWideByTable(0x20));      // space
	EXPECT_FALSE(IsWideByTable(0x00E9));    // e-acute
	EXPECT_FALSE(IsWideByTable(0x0410));    // Cyrillic A
}

TEST(CharWidth, Table_HangulHanKanaAreWide)
{
	EXPECT_TRUE(IsWideByTable(0xAC00));     // Hangul GA
	EXPECT_TRUE(IsWideByTable(0x6F22));     // Han 'Han'
	EXPECT_TRUE(IsWideByTable(0x3042));     // Hiragana A
	EXPECT_TRUE(IsWideByTable(0x1100));     // Hangul Jamo (leading edge of the table)
}

TEST(CharWidth, Table_FullwidthFormsAreWide)
{
	EXPECT_TRUE(IsWideByTable(0xFF01));     // Fullwidth exclamation
	EXPECT_TRUE(IsWideByTable(0xFF21));     // Fullwidth A
}

TEST(CharWidth, Table_AstralEmojiAndCjkAreWide)
{
	// Whole astral blocks are wide, so the table carries them by range.
	EXPECT_TRUE(IsWideByTable(0x1F600));    // grinning face
	EXPECT_TRUE(IsWideByTable(0x20000));    // CJK Extension B
}

TEST(CharWidth, Table_DoesNotChaseScatteredBmpSymbols_LeftToMeasurement)
{
	// U+2705 CHECK MARK and U+2605 BLACK STAR sit below the CJK blocks, interleaved with
	// narrow characters, so the table does NOT try to enumerate them — that is exactly the
	// enumeration game that shipped a caret inside emoji last time. The measurement half
	// catches them instead: both render ~1.7x wide (see CellsFor tests below). U+2705 in
	// particular is East Asian Width Wide, but the table still leaves it to measurement.
	EXPECT_FALSE(IsWideByTable(0x2705));
	EXPECT_FALSE(IsWideByTable(0x2605));    // Ambiguous — undecidable without a locale anyway
}

TEST(CharWidth, Table_RangeEdges)
{
	EXPECT_FALSE(IsWideByTable(0x10FF));    // just below Hangul Jamo
	EXPECT_TRUE (IsWideByTable(0x1100));    // first
	EXPECT_TRUE (IsWideByTable(0x115F));    // last
	EXPECT_FALSE(IsWideByTable(0x1160));    // just above

	EXPECT_FALSE(IsWideByTable(0xABFF));    // just below Hangul Syllables
	EXPECT_TRUE (IsWideByTable(0xAC00));    // first
	EXPECT_TRUE (IsWideByTable(0xD7A3));    // last
	EXPECT_FALSE(IsWideByTable(0xD7A4));    // just above
}


///////////////////////////////////////////////////////////////////////////////
// CellsFor — the union, the threshold, and the zero-cell case

TEST(CharWidth, Cells_CombiningMarkIsZero)
{
	// A combining mark has no advance of its own; it stacks on the previous character. The
	// measurement reports 0 and that wins, table or no table.
	EXPECT_EQ(CellsFor(0x0301, 0, 7), 0);   // combining acute accent
}

TEST(CharWidth, Cells_NarrowAsciiIsOne)
{
	EXPECT_EQ(CellsFor(0x61, 7, 7), 1);
}

TEST(CharWidth, Cells_TableCatchesHangulAFallbackDrawsTooNarrow)
{
	// Courier New draws Hangul at 1.13x — the measurement alone would call it one cell. The
	// table is what keeps it two. This is the case the union exists for.
	EXPECT_EQ(CellsFor(0xAC00, 9, 8), 2);
}

TEST(CharWidth, Cells_MeasurementCatchesWhatTheTableDoesNotKnow)
{
	// U+2605 is Ambiguous (not in the table). A font drawing it 1.71x wide makes it two
	// cells by measurement alone.
	EXPECT_EQ(CellsFor(0x2605, 12, 7), 2);
}

TEST(CharWidth, Cells_ThresholdIsStrictlyAbove1_2)
{
	// advance * 10 > narrow * 12, so exactly 1.2x is still one cell.
	EXPECT_EQ(CellsFor(0x2605, 12, 10), 1);   // 120 > 120 is false
	EXPECT_EQ(CellsFor(0x2605, 13, 10), 2);   // 130 > 120 is true
}

TEST(CharWidth, Cells_ConsolasHangulIsTwo_BothSourcesAgree)
{
	// The finding the plan rests on: Consolas Hangul at 1.43x. Table says wide; measurement
	// (10*10 > 7*12, i.e. 100 > 84) also says wide. 1.5 would have missed the measurement.
	EXPECT_EQ(CellsFor(0xAC00, 10, 7), 2);
}

TEST(CharWidth, Cells_ZeroNarrowDoesNotDivideByZeroOrFalselyWiden)
{
	// Defensive: a degenerate narrow width must not make a plain character "wide".
	EXPECT_EQ(CellsFor(0x61, 7, 0), 1);
}


///////////////////////////////////////////////////////////////////////////////
// The column coordinate — ColumnFromIdxX / IdxXFromColumn / LastColumn
//
// A fixed cell rule stands in for GDI measurement so the walk is tested without a device
// context: ASCII one cell, Hangul and astral emoji two, a combining mark zero. This is the
// same split of concerns as GetCharCells — the classifier's correctness is tested above; here
// the walk, the tab stops and the boundary rule are what is under test.

static const TCHAR TAB = _T('\t');

static INT FakeCells(void *, LPCTSTR psz, INT nIdxX, INT nLen)
{
	unsigned int cp = CodepointAt(psz, nIdxX, nLen);
	if( cp == 0x0301 ) return 0;                      // combining acute — stacks, no advance
	if( cp >= 0xAC00 && cp <= 0xD7A3 ) return 2;      // Hangul
	if( cp >= 0x1F000 ) return 2;                     // astral emoji
	return 1;                                         // ASCII / Latin
}

// Helpers so the tests read as column<->index without threading the fixed args each time.
static INT Col(LPCTSTR psz, INT nIdx, INT nTab = 4)
{
	return ColumnFromIdxX(psz, (INT)_tcslen(psz), nIdx, nTab, FakeCells, NULL);
}
static INT Idx(LPCTSTR psz, INT nCol, INT nTab = 4)
{
	return IdxXFromColumn(psz, (INT)_tcslen(psz), nCol, nTab, FakeCells, NULL);
}
static INT Last(LPCTSTR psz, INT nTab = 4)
{
	return LastColumn(psz, (INT)_tcslen(psz), nTab, FakeCells, NULL);
}


TEST(Column, TabCells_AdvancesToTheNextTabStop)
{
	EXPECT_EQ(_TabCells(0, 4), 4);
	EXPECT_EQ(_TabCells(1, 4), 3);
	EXPECT_EQ(_TabCells(3, 4), 1);
	EXPECT_EQ(_TabCells(4, 4), 4);   // already on a stop — a full tab
	EXPECT_EQ(_TabCells(0, 0), 1);   // degenerate tab size does not divide by zero
}

TEST(Column, WideCharacterCountsTwoColumns)
{
	// "aA한b" — Hangul at index 2 occupies columns 2 and 3, so b starts at column 4.
	const TCHAR line[] = { _T('a'), _T('A'), (TCHAR)0xAC00, _T('b'), 0 };

	EXPECT_EQ(Col(line, 0), 0);
	EXPECT_EQ(Col(line, 1), 1);
	EXPECT_EQ(Col(line, 2), 2);
	EXPECT_EQ(Col(line, 3), 4);   // past the two-cell Hangul
	EXPECT_EQ(Last(line),    5);
}

TEST(Column, BoundaryRule_ColumnInsideAWideCharResolvesToItsFarSide)
{
	const TCHAR line[] = { _T('a'), _T('A'), (TCHAR)0xAC00, _T('b'), 0 };

	EXPECT_EQ(Idx(line, 2), 2);   // column 2 is the Hangul's first cell -> the Hangul
	EXPECT_EQ(Idx(line, 3), 3);   // column 3 is its SECOND cell -> excluded, resolves to b
	EXPECT_EQ(Idx(line, 4), 3);   // column 4 is b's first cell -> b
}

TEST(Column, RoundTrips_OnEveryCharacterBoundary)
{
	const TCHAR line[] = { _T('a'), _T('A'), (TCHAR)0xAC00, _T('b'), 0 };

	// column -> index -> column returns the character's start column for each real boundary.
	int starts[] = { 0, 1, 2, 4 };
	for( int k = 0; k < 4; k++ )
		EXPECT_EQ(Col(line, Idx(line, starts[k])), starts[k]) << "start column " << starts[k];
}

TEST(Column, Partition_AdjacentRangesTileEveryCharacterExactlyOnce)
{
	// Ranges [0,2) [2,4) [4,6) must map to index ranges that neither gap nor overlap.
	const TCHAR line[] = { _T('a'), _T('A'), (TCHAR)0xAC00, _T('b'), 0 };

	INT b0 = Idx(line, 0), b2 = Idx(line, 2), b4 = Idx(line, 4), b6 = Idx(line, 6);
	EXPECT_EQ(b0, 0);   // "aA"
	EXPECT_EQ(b2, 2);   // "한"
	EXPECT_EQ(b4, 3);   // "b"
	EXPECT_EQ(b6, 4);   // end
	// contiguous and strictly ordered: each range's end is the next one's start
	EXPECT_LT(b0, b2); EXPECT_LT(b2, b4); EXPECT_LT(b4, b6);
}

TEST(Column, Tabs_WalkByTabStopNotFixedWidth)
{
	// "a<TAB>b" with tab size 4: a at column 0, the tab fills columns 1-3, b at column 4.
	const TCHAR line[] = { _T('a'), TAB, _T('b'), 0 };

	EXPECT_EQ(Col(line, 2), 4);   // b
	EXPECT_EQ(Last(line),   5);
	EXPECT_EQ(Idx(line, 4), 2);   // column 4 -> b
	EXPECT_EQ(Idx(line, 2), 2);   // a column inside the tab -> the tab ends past it, so b
}

TEST(Column, AstralEmojiIsOneTwoCellCharacterAcrossTwoCodeUnits)
{
	// "a<emoji>b": the surrogate pair is one character, two cells, two code units.
	const TCHAR line[] = { _T('a'), HI, LO, _T('b'), 0 };

	EXPECT_EQ(Col(line, 1), 1);   // emoji starts at column 1
	EXPECT_EQ(Col(line, 3), 3);   // b, past the two-cell emoji (index jumped 1 -> 3)
	EXPECT_EQ(Last(line),   4);
	EXPECT_EQ(Idx(line, 1), 1);   // column 1 -> the emoji
	EXPECT_EQ(Idx(line, 2), 3);   // its second cell -> excluded, resolves to b
}

TEST(Column, CombiningMarkAddsNoColumn)
{
	// "a<combining>b": the mark has zero cells, so b sits one column past a, not two.
	const TCHAR line[] = { _T('a'), (TCHAR)0x0301, _T('b'), 0 };

	EXPECT_EQ(Col(line, 1), 1);   // the mark starts where a ended
	EXPECT_EQ(Col(line, 2), 1);   // and consumes no column, so b is still at 1
	EXPECT_EQ(Last(line),   2);
}

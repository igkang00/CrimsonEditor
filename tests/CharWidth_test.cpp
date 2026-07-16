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

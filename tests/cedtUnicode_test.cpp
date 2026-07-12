#include "StdAfx.h"
#include "cedtUnicode.h"
#include "cedtElement.h"
#include <gtest/gtest.h>

// UTF-16 surrogate-pair helpers.
//
// U+1F600 GRINNING FACE encodes as the pair D83D DE00. The halves are spelled
// out numerically rather than pasted as a literal emoji so the tests do not
// depend on the source file's encoding.
static const TCHAR HI = (TCHAR)0xD83D;   // high surrogate of U+1F600
static const TCHAR LO = (TCHAR)0xDE00;   // low  surrogate of U+1F600
static const TCHAR RC = (TCHAR)0xFFFD;   // U+FFFD REPLACEMENT CHARACTER

// "a<emoji>b" — indices: 0='a', 1=HI, 2=LO, 3='b', length 4
static const TCHAR A_EMOJI_B[] = { _T('a'), HI, LO, _T('b'), 0 };
static const INT   A_EMOJI_B_LEN = 4;


///////////////////////////////////////////////////////////////////////////////
// Classification

TEST(CedtUnicodeTest, IsHighSurrogate_MatchesOnlyD800ToDBFF)
{
    EXPECT_TRUE(IsHighSurrogate((TCHAR)0xD800));
    EXPECT_TRUE(IsHighSurrogate((TCHAR)0xDBFF));
    EXPECT_TRUE(IsHighSurrogate(HI));

    EXPECT_FALSE(IsHighSurrogate((TCHAR)0xD7FF));   // just below the range
    EXPECT_FALSE(IsHighSurrogate((TCHAR)0xDC00));   // that's a LOW surrogate
    EXPECT_FALSE(IsHighSurrogate(_T('a')));
    EXPECT_FALSE(IsHighSurrogate((TCHAR)0xD55C));   // Hangul U+D55C, a BMP char
}

TEST(CedtUnicodeTest, IsLowSurrogate_MatchesOnlyDC00ToDFFF)
{
    EXPECT_TRUE(IsLowSurrogate((TCHAR)0xDC00));
    EXPECT_TRUE(IsLowSurrogate((TCHAR)0xDFFF));
    EXPECT_TRUE(IsLowSurrogate(LO));

    EXPECT_FALSE(IsLowSurrogate((TCHAR)0xDBFF));    // that's a HIGH surrogate
    EXPECT_FALSE(IsLowSurrogate((TCHAR)0xE000));    // just above the range
    EXPECT_FALSE(IsLowSurrogate(_T('a')));
}

// The whole point of UTF-16 over MBCS: a code unit's role is decidable from the
// unit alone, with no context and no backward scanning.
TEST(CedtUnicodeTest, IsSurrogate_RecognizesEitherHalf)
{
    EXPECT_TRUE(IsSurrogate(HI));
    EXPECT_TRUE(IsSurrogate(LO));
    EXPECT_FALSE(IsSurrogate((TCHAR)0xD55C));       // Hangul: BMP, not a surrogate
    EXPECT_FALSE(IsSurrogate(_T('Z')));
}


///////////////////////////////////////////////////////////////////////////////
// CharUnitsAt

TEST(CedtUnicodeTest, CharUnitsAt_PairCountsTwo_EverythingElseOne)
{
    EXPECT_EQ(1, CharUnitsAt(A_EMOJI_B, 0, A_EMOJI_B_LEN));   // 'a'
    EXPECT_EQ(2, CharUnitsAt(A_EMOJI_B, 1, A_EMOJI_B_LEN));   // the pair
    EXPECT_EQ(1, CharUnitsAt(A_EMOJI_B, 3, A_EMOJI_B_LEN));   // 'b'
}

// A lone surrogate reports 1 on purpose: we step over the damage one unit at a
// time rather than running past the end of the line.
TEST(CedtUnicodeTest, CharUnitsAt_LoneSurrogateCountsOne)
{
    const TCHAR szLoneHigh[] = { _T('a'), HI, 0 };
    EXPECT_EQ(1, CharUnitsAt(szLoneHigh, 1, 2));    // high with nothing after it

    const TCHAR szLoneLow[] = { _T('a'), LO, 0 };
    EXPECT_EQ(1, CharUnitsAt(szLoneLow, 1, 2));     // low with no high before it

    // The low half of a real pair, addressed directly, is also just 1 unit.
    EXPECT_EQ(1, CharUnitsAt(A_EMOJI_B, 2, A_EMOJI_B_LEN));
}

TEST(CedtUnicodeTest, CharUnitsAt_OutOfRangeIsSafe)
{
    EXPECT_EQ(1, CharUnitsAt(A_EMOJI_B, A_EMOJI_B_LEN, A_EMOJI_B_LEN));  // at end
    EXPECT_EQ(1, CharUnitsAt(A_EMOJI_B, 99, A_EMOJI_B_LEN));             // past end
    EXPECT_EQ(1, CharUnitsAt(A_EMOJI_B, -1, A_EMOJI_B_LEN));             // negative
}


///////////////////////////////////////////////////////////////////////////////
// IsCharBoundary / SnapIdxX

TEST(CedtUnicodeTest, IsCharBoundary_FalseOnlyInsideAPair)
{
    EXPECT_TRUE(IsCharBoundary(A_EMOJI_B, 0));      // start of 'a'
    EXPECT_TRUE(IsCharBoundary(A_EMOJI_B, 1));      // start of the emoji
    EXPECT_FALSE(IsCharBoundary(A_EMOJI_B, 2));     // INSIDE the emoji
    EXPECT_TRUE(IsCharBoundary(A_EMOJI_B, 3));      // start of 'b'
    EXPECT_TRUE(IsCharBoundary(A_EMOJI_B, 4));      // end of string
}

TEST(CedtUnicodeTest, SnapIdxX_PullsMidPairIndexBackOntoTheHighHalf)
{
    EXPECT_EQ(1, SnapIdxX(A_EMOJI_B, 2));           // mid-pair snaps down to the pair start
    EXPECT_EQ(1, SnapIdxX(A_EMOJI_B, 1));           // already a boundary: unchanged
    EXPECT_EQ(0, SnapIdxX(A_EMOJI_B, 0));
    EXPECT_EQ(3, SnapIdxX(A_EMOJI_B, 3));
    EXPECT_EQ(4, SnapIdxX(A_EMOJI_B, 4));
}


///////////////////////////////////////////////////////////////////////////////
// NextIdxX / PrevIdxX — one arrow-key press moves one CHARACTER

TEST(CedtUnicodeTest, NextIdxX_StepsOverAWholePair)
{
    EXPECT_EQ(1, NextIdxX(A_EMOJI_B, 0, A_EMOJI_B_LEN));   // 'a'   -> emoji
    EXPECT_EQ(3, NextIdxX(A_EMOJI_B, 1, A_EMOJI_B_LEN));   // emoji -> 'b'  (skips 2 units)
    EXPECT_EQ(4, NextIdxX(A_EMOJI_B, 3, A_EMOJI_B_LEN));   // 'b'   -> end
}

TEST(CedtUnicodeTest, NextIdxX_FromMidPairSnapsFirst)
{
    // Handed a mid-pair index, we must still land on the NEXT boundary (3),
    // never on the low half (2).
    EXPECT_EQ(3, NextIdxX(A_EMOJI_B, 2, A_EMOJI_B_LEN));
}

TEST(CedtUnicodeTest, PrevIdxX_StepsBackOverAWholePair)
{
    EXPECT_EQ(3, PrevIdxX(A_EMOJI_B, 4));   // end   -> 'b'
    EXPECT_EQ(1, PrevIdxX(A_EMOJI_B, 3));   // 'b'   -> emoji (skips 2 units)
    EXPECT_EQ(0, PrevIdxX(A_EMOJI_B, 1));   // emoji -> 'a'
    EXPECT_EQ(0, PrevIdxX(A_EMOJI_B, 0));   // clamped at the start
}

// Round-trip: walking right across the line then back must visit the same
// boundaries. This is the property that makes Left/Right symmetric.
TEST(CedtUnicodeTest, NextAndPrevIdxX_RoundTrip)
{
    INT nIdx = 0;
    for(INT i = 0; i < 3; i++) nIdx = NextIdxX(A_EMOJI_B, nIdx, A_EMOJI_B_LEN);
    EXPECT_EQ(A_EMOJI_B_LEN, nIdx);         // 3 characters -> at the end

    for(INT i = 0; i < 3; i++) nIdx = PrevIdxX(A_EMOJI_B, nIdx);
    EXPECT_EQ(0, nIdx);                     // and back to the start
}


///////////////////////////////////////////////////////////////////////////////
// SpanToCharBoundary — overwrite mode must never cut a pair in half

TEST(CedtUnicodeTest, SpanToCharBoundary_RoundsUpToWholeCharacters)
{
    // Asking to overwrite 1 unit starting at the emoji has to consume both of
    // its units, or the low half survives as a lone surrogate.
    EXPECT_EQ(2, SpanToCharBoundary(A_EMOJI_B, 1, 1, A_EMOJI_B_LEN));
    EXPECT_EQ(2, SpanToCharBoundary(A_EMOJI_B, 1, 2, A_EMOJI_B_LEN));

    // 3 units from the emoji = the pair plus 'b'.
    EXPECT_EQ(3, SpanToCharBoundary(A_EMOJI_B, 1, 3, A_EMOJI_B_LEN));

    // Plain BMP text is unaffected.
    EXPECT_EQ(1, SpanToCharBoundary(A_EMOJI_B, 0, 1, A_EMOJI_B_LEN));
}

TEST(CedtUnicodeTest, SpanToCharBoundary_StopsAtEndOfLine)
{
    EXPECT_EQ(1, SpanToCharBoundary(A_EMOJI_B, 3, 99, A_EMOJI_B_LEN));   // only 'b' left
    EXPECT_EQ(0, SpanToCharBoundary(A_EMOJI_B, A_EMOJI_B_LEN, 5, A_EMOJI_B_LEN));
}


///////////////////////////////////////////////////////////////////////////////
// ScrubLoneSurrogates — the load-time invariant

TEST(CedtUnicodeTest, ScrubLoneSurrogates_ReplacesUnpairedHalvesWithFFFD)
{
    const TCHAR szGood[]   = { _T('a'), HI, LO, _T('b'), 0 };   // well-formed pair
    const TCHAR szLoneHi[] = { _T('x'), HI, _T('y'), 0 };       // high, no low after
    const TCHAR szLoneLo[] = { _T('p'), LO, _T('q'), 0 };       // low, no high before

    CAnalyzedText text;
    text.AddTail(szGood);
    text.AddTail(szLoneHi);
    text.AddTail(szLoneLo);

    text.ScrubLoneSurrogates();

    POSITION pos = text.GetHeadPosition();
    CAnalyzedString & rGood   = text.GetNext(pos);
    CAnalyzedString & rLoneHi = text.GetNext(pos);
    CAnalyzedString & rLoneLo = text.GetNext(pos);

    // A real pair must survive untouched — scrubbing must not eat valid emoji.
    EXPECT_EQ(HI, rGood[1]);
    EXPECT_EQ(LO, rGood[2]);
    EXPECT_EQ(4,  rGood.GetLength());

    // Half characters become U+FFFD, which is what a conforming encoder would
    // have written. Length is preserved; only the bad unit changes.
    EXPECT_EQ(RC, rLoneHi[1]);
    EXPECT_EQ(_T('y'), rLoneHi[2]);

    EXPECT_EQ(RC, rLoneLo[1]);
    EXPECT_EQ(_T('q'), rLoneLo[2]);
}

TEST(CedtUnicodeTest, ScrubLoneSurrogates_LeavesPlainTextAlone)
{
    CAnalyzedText text;
    text.AddTail(_T("int main(void) { return 0; }"));

    text.ScrubLoneSurrogates();

    EXPECT_STREQ(_T("int main(void) { return 0; }"), (LPCTSTR)text.GetHead());
}


///////////////////////////////////////////////////////////////////////////////
// UTF-8 round-trip — the regression test for the actual data corruption
//
// UTF-8 has no surrogates: U+1F600 is simply the 4 bytes F0 9F 98 80. The whole
// point of this work is that a surrogate PAIR reaches the encoder intact, so it
// goes out as those 4 bytes. If an editing path had left half a pair behind,
// WideCharToMultiByte could not encode the orphan and would emit U+FFFD
// (EF BF BD) instead — silently destroying the character on disk.

TEST(CedtUnicodeTest, Utf8Save_EmojiBecomesFourBytes_NotReplacementChar)
{
    TCHAR szDir[MAX_PATH]; GetTempPath(MAX_PATH, szDir);
    CString szPath = CString(szDir) + _T("cedt_astral_roundtrip.txt");

    const TCHAR szLine[] = { _T('a'), HI, LO, _T('b'), 0 };

    CAnalyzedText text;
    text.AddTail(szLine);
    ASSERT_TRUE(text.FileSave(szPath, ENCODING_TYPE_UTF8_XBOM, FILE_FORMAT_UNIX));

    FILE * fp = NULL;
    _tfopen_s(&fp, szPath, _T("rb"));
    ASSERT_NE(nullptr, fp);

    unsigned char buf[64] = { 0 };
    size_t n = fread(buf, 1, sizeof(buf), fp);
    fclose(fp);
    _tremove(szPath);

    ASSERT_GE(n, (size_t)6);

    EXPECT_EQ(0x61, buf[0]);            // 'a'
    EXPECT_EQ(0xF0, buf[1]);            // U+1F600, 4-byte UTF-8
    EXPECT_EQ(0x9F, buf[2]);
    EXPECT_EQ(0x98, buf[3]);
    EXPECT_EQ(0x80, buf[4]);
    EXPECT_EQ(0x62, buf[5]);            // 'b'

    for(size_t i = 0; i + 2 < n; i++) {
        EXPECT_FALSE(buf[i] == 0xEF && buf[i+1] == 0xBF && buf[i+2] == 0xBD)
            << "U+FFFD at offset " << i << " - the surrogate pair was destroyed on save";
    }
}

// A lone surrogate that somehow reached the buffer must NOT be silently mangled
// into something unpredictable: the load-time scrub turns it into U+FFFD, which
// then encodes as the well-formed 3-byte EF BF BD.
TEST(CedtUnicodeTest, Utf8Save_ScrubbedLoneSurrogateBecomesWellFormedFFFD)
{
    TCHAR szDir[MAX_PATH]; GetTempPath(MAX_PATH, szDir);
    CString szPath = CString(szDir) + _T("cedt_lone_roundtrip.txt");

    const TCHAR szLine[] = { _T('a'), HI, _T('b'), 0 };   // lone high surrogate

    CAnalyzedText text;
    text.AddTail(szLine);
    text.ScrubLoneSurrogates();
    ASSERT_TRUE(text.FileSave(szPath, ENCODING_TYPE_UTF8_XBOM, FILE_FORMAT_UNIX));

    FILE * fp = NULL;
    _tfopen_s(&fp, szPath, _T("rb"));
    ASSERT_NE(nullptr, fp);

    unsigned char buf[64] = { 0 };
    size_t n = fread(buf, 1, sizeof(buf), fp);
    fclose(fp);
    _tremove(szPath);

    ASSERT_GE(n, (size_t)5);

    EXPECT_EQ(0x61, buf[0]);            // 'a'
    EXPECT_EQ(0xEF, buf[1]);            // U+FFFD, 3-byte UTF-8
    EXPECT_EQ(0xBF, buf[2]);
    EXPECT_EQ(0xBD, buf[3]);
    EXPECT_EQ(0x62, buf[4]);            // 'b'
}

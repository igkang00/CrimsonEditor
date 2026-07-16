// Crimson Editor — display-cell width for column (block) mode
//
// Column mode places text on a grid: a character is one cell wide or two, and the editor
// draws it at that cell whatever the font's own advance would have been (see
// docs/refactoring-column-mode.md). This header answers "how many cells?" for one character.
//
// The answer is the UNION of two sources, because neither is complete on its own:
//
//   * a TABLE of East Asian Width W/F ranges plus emoji — font-independent, so the same
//     document has the same columns on every machine; but hand-maintained, so it lags the
//     Unicode that adds wide characters every version.
//   * a MEASUREMENT — the glyph's real advance is more than 1.2x the narrow cell. This
//     catches what the table has not heard of (a new emoji), and misses what a bad fallback
//     font draws too narrow (Courier New draws Hangul at 1.13x). Each covers the other's gap.
//
// Why 1.2 and not something rounder: Consolas font-links Hangul at 1.43x the Latin advance,
// so a 1.5 threshold would miss the single most important wide character in the font that is
// column mode's current default. See the sorted ratios in the design doc, and re-measure with
// analysis/font-cell-width.ps1.
//
// Why lean wide when the two disagree: under forced placement, calling a NARROW character
// wide costs a little air around the glyph, while calling a WIDE one narrow makes it overlap
// its neighbour. The harmless error and the visible one are not symmetric, so the union takes
// either "yes".
//
// EVERYTHING INTERESTING IS PURE, and lives here so it can be unit-tested without a GDI
// device context: the table, the union, the 1.2x rule, the zero-cell combining-mark case, and
// the surrogate-pair decode all take their inputs as plain values. The GDI half —
// CCedtView::GetCharCells — is a thin shell that measures the advance and calls CellsFor.

#ifndef __CEDT_CHARWIDTH_H_
#define __CEDT_CHARWIDTH_H_

#include "cedtUnicode.h"    // CharUnitsAt, IsHighSurrogate, IsLowSurrogate


// The Unicode scalar value of the character starting at nIdxX. A well-formed surrogate pair
// becomes its astral code point; anything else (BMP char, or a lone surrogate we refuse to
// choke on) is returned as-is. This is what lets the table below carry emoji ranges above
// U+FFFF — a pair is one character, and the table is keyed on the character, not the unit.
inline unsigned int CodepointAt(LPCTSTR lpszLine, INT nIdxX, INT nLength)
{
	if( nIdxX < 0 || nIdxX >= nLength ) return 0;

	if( CharUnitsAt(lpszLine, nIdxX, nLength) == 2 ) {
		unsigned int hi = (unsigned int)(unsigned short)lpszLine[nIdxX];
		unsigned int lo = (unsigned int)(unsigned short)lpszLine[nIdxX + 1];
		return 0x10000 + ((hi - 0xD800) << 10) + (lo - 0xDC00);
	}

	return (unsigned int)(unsigned short)lpszLine[nIdxX];
}


// East Asian Width W (Wide) and F (Fullwidth), plus the astral emoji blocks.
//
// Ranges follow Unicode's EastAsianWidth.txt (W and F classes). This is the hand-maintained
// half, and it is ALLOWED to be incomplete: the measurement half of the classifier is the
// safety net for a character this table has not heard of. It exists to be right about the
// characters a bad fallback font would otherwise draw too narrow — chiefly Hangul and Han —
// not to be a complete census of wide Unicode.
//
// In particular it does NOT chase the emoji and symbols scattered through the BMP below the
// CJK blocks (U+2600 CHECK MARK, U+2B50 STAR, and their neighbours), which are interleaved
// with narrow characters and cannot be captured as clean ranges. Trying to enumerate them is
// precisely what shipped a caret inside an emoji last time — the deleted _CharColumnWidth
// listed some and missed U+2705. A colour-emoji font draws them ~1.7x wide, so the
// measurement catches them; the table does not pretend to.
inline BOOL IsWideByTable(unsigned int cp)
{
	// Fast out for ASCII and Latin-1, which is the overwhelming majority of source text.
	if( cp < 0x1100 ) return FALSE;

	return (
	    // --- BMP: East Asian Width W / F ---
	    (cp >= 0x1100 && cp <= 0x115F) ||   // Hangul Jamo
	    (cp >= 0x2E80 && cp <= 0x2EFF) ||   // CJK Radicals Supplement
	    (cp >= 0x2F00 && cp <= 0x2FDF) ||   // Kangxi Radicals
	    (cp >= 0x2FF0 && cp <= 0x2FFF) ||   // Ideographic Description Characters
	    (cp >= 0x3000 && cp <= 0x303E) ||   // CJK Symbols and Punctuation (U+3000 is F)
	    (cp >= 0x3041 && cp <= 0x33FF) ||   // Hiragana .. CJK Compatibility
	    (cp >= 0x3400 && cp <= 0x4DBF) ||   // CJK Unified Ideographs Extension A
	    (cp >= 0x4E00 && cp <= 0x9FFF) ||   // CJK Unified Ideographs
	    (cp >= 0xA000 && cp <= 0xA4CF) ||   // Yi Syllables / Yi Radicals
	    (cp >= 0xA960 && cp <= 0xA97F) ||   // Hangul Jamo Extended-A
	    (cp >= 0xAC00 && cp <= 0xD7A3) ||   // Hangul Syllables
	    (cp >= 0xF900 && cp <= 0xFAFF) ||   // CJK Compatibility Ideographs
	    (cp >= 0xFE10 && cp <= 0xFE19) ||   // Vertical Forms
	    (cp >= 0xFE30 && cp <= 0xFE4F) ||   // CJK Compatibility Forms
	    (cp >= 0xFF00 && cp <= 0xFF60) ||   // Fullwidth Forms
	    (cp >= 0xFFE0 && cp <= 0xFFE6) ||   // Fullwidth Signs

	    // --- emoji and astral CJK ---
	    (cp >= 0x1F000 && cp <= 0x1FAFF) || // Mahjong .. Symbols and Pictographs Extended-A
	    (cp >= 0x20000 && cp <= 0x3FFFD)    // CJK Unified Ideographs Extension B and beyond
	) ? TRUE : FALSE;
}


// How many cells the character occupies, from its code point and its measured advance.
//
//   advance == 0   -> 0 cells. A combining mark has no advance of its own; it stacks on the
//                     previous character. This falls out of the measurement with no special
//                     table entry, which is the point of measuring.
//   table OR wide  -> 2 cells. Either source saying "wide" wins (see the header note on why
//                     leaning wide is the safe error under forced placement).
//   otherwise      -> 1 cell.
//
// nNarrow is the width of a space in the current font. The 1.2x test is written as integer
// cross-multiplication so it neither rounds nor overflows.
inline INT CellsFor(unsigned int cp, INT nAdvance, INT nNarrow)
{
	if( nAdvance <= 0 ) return 0;

	if( IsWideByTable(cp) ) return 2;
	if( nNarrow > 0 && nAdvance * 10 > nNarrow * 12 ) return 2;

	return 1;
}


// ---------------------------------------------------------------------------
// The column coordinate.
//
// A display column is a running count of cells from the start of the line. These three
// functions are the whole of it: character index -> column, column -> character index, and
// the line's total width. Column mode's caret, selection and block geometry are all rebuilt
// on them (Phase 2+).
//
// Cells come from a caller-supplied function so this logic is testable without a device
// context — the GDI wrapper passes one backed by GetCharCells; a test passes a fixed rule.
// TAB is the one thing handled here rather than by that function: a tab advances to the next
// tab stop, so its width depends on the column it starts at, which only this walk knows.
// ---------------------------------------------------------------------------

// Cells of the (non-tab) character at psz[nIdxX]. nLen is the line length in code units.
typedef INT (*PFN_CHARCELLS)(void * pCtx, LPCTSTR psz, INT nIdxX, INT nLen);

// Cells a tab occupies, starting from nColumn. Whole tab stops, so the grid stays integer.
inline INT _TabCells(INT nColumn, INT nTabSize)
{
	if( nTabSize <= 0 ) return 1;
	return nTabSize - (nColumn % nTabSize);
}

// The display column at which the character at nIdxX begins — i.e. the cells before it.
inline INT ColumnFromIdxX(LPCTSTR psz, INT nLen, INT nIdxX, INT nTabSize,
                          PFN_CHARCELLS pfnCells, void * pCtx)
{
	INT nCol = 0, i = 0;
	while( i < nIdxX && i < nLen ) {
		if( psz[i] == _T('\t') ) {
			nCol += _TabCells(nCol, nTabSize);
			i += 1;
		} else {
			nCol += pfnCells(pCtx, psz, i, nLen);
			i += CharUnitsAt(psz, i, nLen);
		}
	}
	return nCol;
}

// The index of the first character whose START column is >= nColumn. This IS the boundary
// rule — a character belongs to the range holding its first cell — and it has no edge
// parameter because both edges of a block ask it the same question. A column that lands
// inside a wide character therefore resolves to that character's far side (its next
// boundary), which is what makes adjacent ranges tile the text with no gap and no overlap.
// Past the end of the line it returns nLen; virtual space is the caller's business.
inline INT IdxXFromColumn(LPCTSTR psz, INT nLen, INT nColumn, INT nTabSize,
                          PFN_CHARCELLS pfnCells, void * pCtx)
{
	INT nCol = 0, i = 0;
	while( i < nLen ) {
		if( nCol >= nColumn ) return i;
		if( psz[i] == _T('\t') ) {
			nCol += _TabCells(nCol, nTabSize);
			i += 1;
		} else {
			nCol += pfnCells(pCtx, psz, i, nLen);
			i += CharUnitsAt(psz, i, nLen);
		}
	}
	return nLen;
}

// The line's width in cells.
inline INT LastColumn(LPCTSTR psz, INT nLen, INT nTabSize, PFN_CHARCELLS pfnCells, void * pCtx)
{
	return ColumnFromIdxX(psz, nLen, nLen, nTabSize, pfnCells, pCtx);
}


#endif // __CEDT_CHARWIDTH_H_

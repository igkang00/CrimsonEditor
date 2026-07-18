#include "stdafx.h"
#include "cedtHeader.h"

static FORMATEDWORD _words[MAX_WORDS_COUNT+1];

// width setting global variables
static BOOL  _bFixedPitch;
static INT   _nSpaceWidth, _nTabWidth, _nTabMargin;
static INT   _nWrapWidth, _nIndentWidth, _nMinWidth;

// TRUE while laying out for the column-mode grid — runs measure to cells x narrow rather
// than their real advance. Distinct from CCedtView::m_bColumnMode (which is the editing
// mode, always on while column mode is): this is the LAYOUT choice, and it is off for the
// printer path, which measures for real, even when column mode happens to be on.
static BOOL  _bGridLayout;

// temporary global variables for format text
static BOOL  _bWordSplit; 
static INT   _nLeadingSpaceWidth;
static SHORT _siSplitIndex, _siWordIndex;

static UCHAR _ucType[3], _ucInfo;
static SHORT _siIndex, _siLength;



// How many screen rows does the logical line whose first row is nRow occupy? One, plus
// its continuation rows — the ones word wrap pushed onto the next display line, which
// carry a non-zero m_siSplitIndex.
static INT _ParagraphRowCount(CFormatedText & rRows, INT nRow, INT nTotal)
{
	INT nRows = 1;
	while( nRow + nRows < nTotal && rRows.ElementAt(nRow + nRows).m_siSplitIndex != 0 ) nRows++;
	return nRows;
}

static INT _GetLeadingSpaceWidth(CAnalyzedString & rLine, CDC * pDC)
{
	INT nPosition = 0;

	for(SHORT i = 0; i < rLine.m_siWordCount; i++) {
		ANALYZEDWORD & rWord = rLine.m_pWord[i];
		UCHAR ucType = rWord.m_ucType[0]; SHORT siLength = rWord.m_siLength;

		if( ucType == WT_TAB ) {
			nPosition += ((nPosition + _nSpaceWidth - _nTabMargin) / _nTabWidth + 1) * _nTabWidth - nPosition;
		} else if( ucType == WT_SPACE ) {
			nPosition += _nSpaceWidth * siLength;
		} else break;
	}

	return nPosition;
}

// Can this word's width be computed with plain arithmetic, or do we have to ask
// GDI what it will actually draw?
//
// The fixed-pitch fast path (width = _nSpaceWidth * length) is only valid when
// EVERY character is exactly one narrow cell. Under MBCS that was implicit:
// CP949 stored CJK as two bytes, the code counted bytes, and the arithmetic
// happened to line up with the two-column glyph. Under Unicode a character's
// advance no longer follows from its storage size at all.
//
// This used to test against a hand-written table of East Asian Width ranges, and
// that was the wrong shape of solution: enumerating the wide characters is a
// losing game. It missed BMP emoji entirely — U+2705 (white heavy check mark)
// and U+2B50 (white medium star) look like ordinary BMP code points, are in none
// of the CJK ranges, yet render roughly two cells wide. They took the fast path,
// were billed one cell, and were then drawn two cells wide: the caret landed in
// the middle of the glyph and everything after them on the line was shifted.
// Unicode adds more such characters every version, so the table could only ever
// be behind.
//
// So invert the test. Only plain ASCII in a fixed-pitch font is GUARANTEED to be
// one narrow cell. Everything else — CJK, surrogate pairs, BMP emoji, symbols,
// anything font-linking substitutes from a fallback face — goes to GDI, which by
// definition reports the width that will actually be rendered. Pure-ASCII source
// (the overwhelmingly common case) still takes the fast path.
static BOOL _NeedsGdiMeasure(LPCTSTR pWord, SHORT siLength)
{
#ifdef _UNICODE
	for(SHORT i = 0; i < siLength; i++) {
		if( (unsigned int)(unsigned short)pWord[i] >= 0x0080 ) return TRUE;
	}
#else
	(void)pWord; (void)siLength;
#endif
	return FALSE;
}

// ---------------------------------------------------------------------------
// Per-character advance cache
//
// Measuring is what makes formatting expensive. On a 900,000-line Korean file with
// word wrap on, laying the document out called GetTextExtent 3.6 million times, and
// those calls were ~90 % of the 12 s it took. Only 13 % of words needed GDI at all —
// the pure-ASCII ones already take the arithmetic fast path — but each of those calls
// is a round trip that has to font-link, load the fallback face, and shape.
//
// The advance of a character does not depend on what surrounds it: GDI lays text out
// by summing per-glyph advances, and TextOut — the call that eventually DRAWS this
// text — does the same. So the width of a word is the sum of the widths of its
// characters, and a character's width can be measured once and remembered. A Korean
// source file has a few thousand distinct characters, not 3.6 million.
//
// (This equivalence was verified against the real files rather than assumed: an
// instrumented build compared cache-summed widths with GetTextExtent for every word
// of the 100 MB Korean fixture and the astral/emoji fixture, and they agreed exactly.
// It holds because GDI applies no kerning and no cross-character shaping here. It
// would NOT hold for a shaping engine like DirectWrite/Uniscribe, so if the draw path
// ever moves to DirectWrite — see the colour-emoji roadmap item — this cache must move
// with it, not be left behind.)
//
// The cache is keyed on the font actually selected into the DC. That covers a font
// change, italics, and the printer DC (a different font entirely) sharing these same
// helpers — any of them invalidates the cache instead of silently returning widths
// measured against some other face.
static INT   _charWidth[0x10000];
static HFONT _hCacheFont = NULL;

static void _ResetWidthCacheIfFontChanged(CDC * pDC)
{
	HFONT hFont = (HFONT)::GetCurrentObject(pDC->GetSafeHdc(), OBJ_FONT);
	if( hFont == _hCacheFont ) return;

	_hCacheFont = hFont;
	for(INT i = 0; i < 0x10000; i++) _charWidth[i] = -1;	// -1 = not measured yet
}

// Advance of one BMP character, measured on first use.
static inline INT _CharAdvance(CDC * pDC, TCHAR ch)
{
	INT & rWidth = _charWidth[(unsigned short)ch];
	if( rWidth < 0 ) {
		CSize size = pDC->GetTextExtent( & ch, 1);
		rWidth = size.cx;
	}
	return rWidth;
}

// Width of a run, summed from the per-character cache.
//
// Surrogate pairs are measured as a pair and NOT cached: they are one character made
// of two code units, so neither half has a meaningful width on its own. They are rare
// enough in source that a GDI call each is fine.
static INT _MeasureRun(CDC * pDC, LPCTSTR pWord, SHORT siLength)
{
	INT nTotal = 0;

	for(SHORT i = 0; i < siLength; ) {
		SHORT siUnits = (SHORT)CharUnitsAt(pWord, i, siLength);

		if( siUnits == 2 ) {
			CSize size = pDC->GetTextExtent(pWord + i, 2);
			nTotal += size.cx;
		} else {
			nTotal += _CharAdvance(pDC, pWord[i]);
		}

		i = (SHORT)( i + siUnits );
	}

	return nTotal;
}

// Display cells of one character (0/1/2), from its code point and its measured advance.
// Static so the static formatters below can reach it; the cache reset is the caller's job
// (formatting resets once per batch). The narrow cell is the cached space advance.
static INT _CharCells(CDC * pDC, LPCTSTR psz, INT nIdxX, INT nLen)
{
	SHORT nUnits = (SHORT)CharUnitsAt(psz, nIdxX, nLen);
	INT nAdvance = _MeasureRun(pDC, psz + nIdxX, nUnits);
	unsigned int cp = CodepointAt(psz, nIdxX, nLen);
	return CellsFor(cp, nAdvance, _CharAdvance(pDC, _T(' ')));
}

// The grid width of a run: the sum of its characters' cells, times the narrow cell. This is
// what a word measures to in column mode — not its real pixel advance, which for CJK or emoji
// is not a whole number of cells. Padding and every position downstream are multiples of the
// cell, so the run must be too.
static INT _WordCellWidth(CDC * pDC, LPCTSTR pWord, SHORT siLength)
{
	INT nCells = 0;
	for(SHORT i = 0; i < siLength; ) {
		SHORT nUnits = (SHORT)CharUnitsAt(pWord, i, siLength);
		nCells += _CharCells(pDC, pWord, i, siLength);
		i = (SHORT)( i + nUnits );
	}
	return nCells * _nSpaceWidth;
}

static INT _GetWordWidth(LPCTSTR pWord, SHORT siLength, INT nPosition, UCHAR cType, CDC * pDC)
{
	if( cType == WT_TAB ) {
		return ((nPosition + _nSpaceWidth - _nTabMargin) / _nTabWidth + 1) * _nTabWidth - nPosition;
	} else if( cType == WT_SPACE ) {
		return _nSpaceWidth * siLength;
	} else if( _bFixedPitch && ! _NeedsGdiMeasure(pWord, siLength) ) {
		// Pure ASCII in a fixed-pitch font: fast path, no measuring at all. Already one cell
		// per character, so column mode needs nothing extra here.
		return _nSpaceWidth * siLength;
	} else if( _bGridLayout ) {
		// Column mode forces the run onto the grid: cells x narrow, not the real advance.
		// A font-linked CJK glyph drawn at 1.43x the Latin cell becomes a clean 2 cells, and
		// the caret, the selection and the padding all agree on where it ends.
		return _WordCellWidth(pDC, pWord, siLength);
	} else {
		// Anything non-ASCII has to be measured even when the base font is declared
		// fixed-pitch. Windows font-linking renders missing glyphs (CJK, emoji) from
		// a fallback face whose actual pixel width is NOT necessarily a whole
		// multiple of _nSpaceWidth. Sum the real per-character advances.
		return (SHORT)_MeasureRun(pDC, pWord, siLength);
	}
}

static SHORT _GetWordIndex(LPCTSTR pWord, SHORT siLength, INT nWidth, CDC * pDC)
{
	SHORT siIndex = 0;

	if( _bFixedPitch && ! _NeedsGdiMeasure(pWord, siLength) ) {
		// Pure ASCII fast path — every char is exactly _nSpaceWidth, and there
		// can be no surrogate pair here, so stepping one unit at a time is safe.
		for(SHORT i = 0; i <= siLength; i++) {
			if( _nSpaceWidth * i <= nWidth ) siIndex = i;
			else break;
		}
	} else {
		// Word contains non-ASCII (CJK, emoji, astral) or the font is variable-pitch.
		// Find the last character boundary whose prefix still fits in nWidth, using
		// the same per-character advances the width calculation uses, so the split
		// lands exactly where the text will be drawn.
		//
		// This used to re-measure the whole prefix from the start of the word on every
		// step — GetTextExtent(pWord, 1), then (pWord, 2), then (pWord, 3) — which is
		// quadratic in GDI calls. Advances add up, so keep a running total instead.
		//
		// Walk CHARACTER boundaries, not code units: this index is used both for caret
		// placement (via GetIdxXFromPosX) and as the word-wrap split point, so stepping
		// one code unit at a time would let the caret sit between the halves of a
		// surrogate pair and let word wrap tear an emoji across two display rows.
		INT nRunning = 0;

		for(SHORT i = 0; i <= siLength; ) {
			if( nRunning <= nWidth ) siIndex = i;
			else break;

			if( i == siLength ) break;

			SHORT siUnits = (SHORT)CharUnitsAt(pWord, i, siLength);
			// Advance in the same units _GetWordWidth used, so the split lands where the text
			// is actually drawn: grid cells in column mode, real pixels otherwise.
			if( _bGridLayout ) nRunning += _CharCells(pDC, pWord, i, siLength) * _nSpaceWidth;
			else               nRunning += _MeasureRun(pDC, pWord + i, siUnits);
			i = (SHORT)( i + siUnits );
		}
	}

	// Defensive: never hand back an index sitting on the low half of a pair.
	// (The loop above already guarantees this; the ASCII fast path can't see a
	// surrogate at all. This is here so a future edit can't silently regress.)
	if( siIndex < siLength ) siIndex = (SHORT)SnapIdxX(pWord, siIndex);

	return siIndex;
}


static void _WordFound(SHORT windex, UCHAR ucType, UCHAR ucInfo, SHORT siIndex, SHORT siLength, INT nPosition, INT nWidth)
{
	_words[windex].m_ucType[0] = ucType;
	_words[windex].m_ucType[1] = ucType;
	_words[windex].m_ucType[2] = ucType;
	_words[windex].m_ucInfo = ucInfo;
	_words[windex].m_siIndex = siIndex;
	_words[windex].m_siLength = siLength;
	_words[windex].m_nPosition = nPosition;
	_words[windex].m_nWidth = nWidth;
}

static void _WordFoundExtended(SHORT windex, UCHAR ucType[], UCHAR ucInfo, SHORT siIndex, SHORT siLength, INT nPosition, INT nWidth)
{
	_words[windex].m_ucType[0] = ucType[0];
	_words[windex].m_ucType[1] = ucType[1];
	_words[windex].m_ucType[2] = ucType[2];
	_words[windex].m_ucInfo = ucInfo;
	_words[windex].m_siIndex = siIndex;
	_words[windex].m_siLength = siLength;
	_words[windex].m_nPosition = nPosition;
	_words[windex].m_nWidth = nWidth;
}

static void _FinishLine(SHORT wcount, SHORT siSplitIndex, BOOL bLineBreak, CFormatedString & rFmtLine, CAnalyzedString & rLine)
{
	delete [] rFmtLine.m_pWord; rFmtLine.m_pWord = new FORMATEDWORD[wcount];
	memcpy(rFmtLine.m_pWord, _words, wcount * sizeof(FORMATEDWORD));

	rFmtLine.m_pString = (LPCTSTR)rLine;
	rFmtLine.m_siWordCount = wcount;
	rFmtLine.m_siSplitIndex = siSplitIndex;
	rFmtLine.m_bLineBreak = bLineBreak;
	rFmtLine.m_usLineInfo = rLine.m_usLineInfo;
	rFmtLine.m_bFormatted = TRUE;

	// m_usLineFlag is deliberately NOT reset here.
	//
	// It used to be zeroed, which was dead code: every caller formats a row and
	// then immediately calls _CheckLineFlag on it, and that overwrites the flag.
	// (True on all four paths — FormatScreenText no-wrap and wrap, and both
	// FormatPrintText loops.)
	//
	// It matters now because rows are laid out lazily: the carry-over syntax
	// state is seeded onto every row at load time, and formatting a row later —
	// when the user finally scrolls to it — must not throw that state away, or
	// the row would forget it is inside a block comment and render the wrong
	// colour.
}


static BOOL _FormatLineWrap(CFormatedString & rFmtLine, CAnalyzedString & rLine, CDC * pDC)
{
	
	SHORT i, wcount, siIndex, siLength, siSplit; 
	INT nPosition, nWidth; UCHAR ucType[3], ucInfo;

	_nLeadingSpaceWidth = _GetLeadingSpaceWidth(rLine, pDC);
	_siSplitIndex = 0;

	wcount = nPosition = 0;
	BOOL bLineBreak = FALSE;

	for(i = 0; i < rLine.m_siWordCount; i++) {
		ANALYZEDWORD & rWord = rLine.m_pWord[i];
		ucType[0] = rWord.m_ucType[0]; ucType[1] = rWord.m_ucType[1]; 
		ucType[2] = rWord.m_ucType[2]; ucInfo = rWord.m_ucInfo;
		siIndex = rWord.m_siIndex; siLength = rWord.m_siLength;

		nWidth = _GetWordWidth((LPCTSTR)rLine + siIndex, siLength, nPosition, ucType[0], pDC);

		if( _nWrapWidth <= _nLeadingSpaceWidth + _nIndentWidth + _nMinWidth ) {
			// not enough space - add in the same line
			_WordFoundExtended(wcount++, ucType, ucInfo, siIndex, siLength, nPosition, nWidth);
			nPosition += nWidth; _bWordSplit = FALSE;
		} else if( nPosition + nWidth <= _nWrapWidth || (ucType[0] == WT_RETURN || ucType[0] == WT_TAB || ucType[0] == WT_SPACE) ) {
			// add in the same line
			_WordFoundExtended(wcount++, ucType, ucInfo, siIndex, siLength, nPosition, nWidth);
			nPosition += nWidth; _bWordSplit = FALSE;
		} else if( _nLeadingSpaceWidth + _nIndentWidth + nWidth <= _nWrapWidth ) {
			// go next line
			_WordFound(wcount++, WT_LINEBREAK, RT_GLOBAL, siIndex, 0, nPosition, 0);
			_bWordSplit = FALSE; _siWordIndex = i; bLineBreak = TRUE; break;
		} else {
			// split word - go next line
			siSplit = _GetWordIndex((LPCTSTR)rLine + siIndex, siLength, _nWrapWidth - nPosition, pDC);
			nWidth = _GetWordWidth((LPCTSTR)rLine + siIndex, siSplit, nPosition, ucType[0], pDC);

			_WordFoundExtended(wcount++, ucType, ucInfo, siIndex, siSplit, nPosition, nWidth);
			siIndex += siSplit; siLength -= siSplit; nPosition += nWidth;

			_ucType[0] = ucType[0]; _ucType[1] = ucType[1]; 
			_ucType[2] = ucType[2]; _ucInfo = ucInfo;
			_siIndex = siIndex; _siLength = siLength; 

			_WordFound(wcount++, WT_LINEBREAK, RT_GLOBAL, siIndex, 0, nPosition, 0);
			_bWordSplit = TRUE; _siWordIndex = i; bLineBreak = TRUE; break;
		}
	}

	_FinishLine(wcount, _siSplitIndex++, bLineBreak, rFmtLine, rLine);
	return bLineBreak;
}


static BOOL _FormatLineWrapContinue(CFormatedString & rFmtLine, CAnalyzedString & rLine, CDC * pDC)
{
	SHORT i, wcount, siIndex, siLength, sSplit;
	INT nPosition, nWidth; UCHAR ucType[3], ucInfo;

	wcount = 0; nPosition = _nLeadingSpaceWidth + _nIndentWidth;
	BOOL bLineBreak = FALSE;

	for(i = _siWordIndex; i < rLine.m_siWordCount; i++) {
		if( _bWordSplit ) {
			ucType[0] = _ucType[0]; ucType[1] = _ucType[1];
			ucType[2] = _ucType[2]; ucInfo = _ucInfo; 
			siIndex = _siIndex; siLength = _siLength;
		} else {
			ANALYZEDWORD & rWord = rLine.m_pWord[i];
			ucType[0] = rWord.m_ucType[0]; ucType[1] = rWord.m_ucType[1]; 
			ucType[2] = rWord.m_ucType[2]; ucInfo = rWord.m_ucInfo;
			siIndex = rWord.m_siIndex; siLength = rWord.m_siLength;
		}

		nWidth = _GetWordWidth((LPCTSTR)rLine + siIndex, siLength, nPosition, ucType[0], pDC);

		if( nPosition + nWidth <= _nWrapWidth || (ucType[0] == WT_RETURN || ucType[0] == WT_TAB || ucType[0] == WT_SPACE) ) {
			// add in the same line
			_WordFoundExtended(wcount++, ucType, ucInfo, siIndex, siLength, nPosition, nWidth);
			nPosition += nWidth; _bWordSplit = FALSE;
		} else if( _nLeadingSpaceWidth + _nIndentWidth + nWidth <= _nWrapWidth ) {
			// go next line
			_WordFound(wcount++, WT_LINEBREAK, RT_GLOBAL, siIndex, 0, nPosition, 0);
			_bWordSplit = FALSE; _siWordIndex = i; bLineBreak = TRUE; break;
		} else {
			// split word - go next line
			sSplit = _GetWordIndex((LPCTSTR)rLine + siIndex, siLength, _nWrapWidth - nPosition, pDC);
			nWidth = _GetWordWidth((LPCTSTR)rLine + siIndex, sSplit, nPosition, ucType[0], pDC);

			_WordFoundExtended(wcount++, ucType, ucInfo, siIndex, sSplit, nPosition, nWidth);
			siIndex += sSplit; siLength -= sSplit; nPosition += nWidth;

			_ucType[0] = ucType[0]; _ucType[1] = ucType[1]; 
			_ucType[2] = ucType[2]; _ucInfo = ucInfo;
			_siIndex = siIndex; _siLength = siLength; 

			_WordFound(wcount++, WT_LINEBREAK, RT_GLOBAL, siIndex, 0, nPosition, 0);
			_bWordSplit = TRUE; _siWordIndex = i; bLineBreak = TRUE; break;
		}
	}

	_FinishLine(wcount, _siSplitIndex++, bLineBreak, rFmtLine, rLine);
	return bLineBreak;
}


static BOOL _FormatLineNoWrap(CFormatedString & rFmtLine, CAnalyzedString & rLine, CDC * pDC)
{
	SHORT i, wcount, siIndex, siLength;
	INT nPosition, nWidth; UCHAR ucType[3], ucInfo;

	wcount = nPosition = 0;

	for(i = 0; i < rLine.m_siWordCount; i++) {
		ANALYZEDWORD & rWord = rLine.m_pWord[i];
		ucType[0] = rWord.m_ucType[0]; ucType[1] = rWord.m_ucType[1]; 
		ucType[2] = rWord.m_ucType[2]; ucInfo = rWord.m_ucInfo;
		siIndex = rWord.m_siIndex; siLength = rWord.m_siLength;

		nWidth = _GetWordWidth((LPCTSTR)rLine + siIndex, siLength, nPosition, ucType[0], pDC);

		_WordFoundExtended(wcount++, ucType, ucInfo, siIndex, siLength, nPosition, nWidth);
		nPosition += nWidth;
	}

	_FinishLine(wcount, 0, FALSE, rFmtLine, rLine);
	return FALSE;
}


// Advance the carry-over syntax state (am I inside a block comment / a multi-line
// string / a here-document?) across one word.
//
// Takes the word's raw fields rather than a FORMATEDWORD, because it never needed
// anything more: m_nPosition and m_nWidth are the only two fields FORMATEDWORD adds
// over ANALYZEDWORD, and neither is read here. That lets the same function drive
// the carry state straight off the ANALYZER's output — no device context, no pixel
// measurement, no per-line allocation — which is what makes the state cheap enough
// to compute eagerly for every line while the layout itself stays lazy.
static void _CheckWordType(LPCTSTR pString, UCHAR ucType, UCHAR ucRange,
                           SHORT siIndex, SHORT siLength,
                           USHORT & usFlag, CString & szTerminator)
{
	switch( ucType ) {
	case WT_QUOTATION0:   if( ! CHECK_QUOTE_EX0(usFlag) && ! CHECK_COMMENT(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) usFlag ^=  LF_QUOTATION0;    break;
	case WT_QUOTATION1:   if( ! CHECK_QUOTE_EX1(usFlag) && ! CHECK_COMMENT(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) usFlag ^=  LF_QUOTATION1;    break;
	case WT_QUOTATION2:   if( ! CHECK_QUOTE_EX2(usFlag) && ! CHECK_COMMENT(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) usFlag ^=  LF_QUOTATION2;    break;
	case WT_QUOTATION3:   if( ! CHECK_QUOTE_EX3(usFlag) && ! CHECK_COMMENT(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) usFlag ^=  LF_QUOTATION3;    break;
	case WT_HEREDOCUMENT: if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) usFlag |=  LF_HEREDOCUMENT;  break;
	case WT_LINECOMMENT:  if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) usFlag |=  LF_LINECOMMENT;   break;
	case WT_COMMENT1ON:   if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) usFlag |=  LF_BLOCKCOMMENT1; break;
	case WT_COMMENT1OFF:  if( ! CHECK_QUOTATION(usFlag) &&   CHECK_CMT_BL1(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) usFlag &= ~LF_BLOCKCOMMENT1; break;
	case WT_COMMENT2ON:   if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) usFlag |=  LF_BLOCKCOMMENT2; break;
	case WT_COMMENT2OFF:  if( ! CHECK_QUOTATION(usFlag) &&   CHECK_CMT_BL2(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) usFlag &= ~LF_BLOCKCOMMENT2; break;

	case WT_SHADOWON:     if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) ) usFlag |=  LF_SHADOW;    break;
	case WT_SHADOWOFF:    if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) ) usFlag &= ~LF_SHADOW;    break;
	case WT_HIGHLIGHTON:  if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) ) usFlag |=  LF_HIGHLIGHT; break;
	case WT_HIGHLIGHTOFF: if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) ) usFlag &= ~LF_HIGHLIGHT; break;
	case WT_RANGE1BEG:    if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) && ! CHECK_RANGE2(usFlag) ) usFlag |=  LF_RANGE1; break;
	case WT_RANGE1END:    if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) && ! CHECK_RANGE2(usFlag) ) usFlag &= ~LF_RANGE1; break;
	case WT_RANGE2BEG:    if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) ) usFlag |=  LF_RANGE2; break;
	case WT_RANGE2END:    if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) ) usFlag &= ~LF_RANGE2; break;

	case WT_IDENTIFIER:
		if( CHECK_THIS(usFlag, LF_HEREDOCUMENT) ) {
			if( ! szTerminator.GetLength() ) {
				szTerminator = CString(pString + siIndex, siLength);
			}
		}
		if( CHECK_THIS(usFlag, LF_QUOTATIONH) ) {
			if( ! szTerminator.Compare(pString + siIndex) ) {
				szTerminator = _T("");
				usFlag &= ~LF_QUOTATIONH;
			}
		}
		break;

	case WT_RETURN:
		if( CHECK_THIS(usFlag, LF_HEREDOCUMENT) ) { 
			usFlag &= ~LF_HEREDOCUMENT;
			if( szTerminator.GetLength() ) usFlag |= LF_QUOTATIONH;
		}
		break;
	}
}


static BOOL _CheckLineFlag(CFormatedString & rFmtLine, USHORT & usLineFlag, CString & szTerminator)
{
	BOOL bCanStopProcessing = ( (rFmtLine.m_usLineFlag == usLineFlag) &&
		! rFmtLine.m_szHereDocumentTerminator.Compare(szTerminator) );

	if( ! bCanStopProcessing ) {
		rFmtLine.m_usLineFlag = usLineFlag;
		rFmtLine.m_szHereDocumentTerminator = szTerminator;
	}

	if( rFmtLine.m_usLineInfo & LI_HAVEHEREDOCUMENT || CHECK_THIS(usLineFlag, LF_QUOTATIONH) ) {
		for(SHORT i = 0; i < rFmtLine.m_siWordCount; i++ ) {
			FORMATEDWORD & rWord = rFmtLine.m_pWord[i];
			_CheckWordType(rFmtLine.m_pString, rWord.m_ucType[0], rWord.m_ucInfo,
			               rWord.m_siIndex, rWord.m_siLength, usLineFlag, szTerminator);
		}
	} else if( rFmtLine.m_usLineInfo & (LI_HAVERANGE | LI_HAVEHIGHLIGHT | LI_HAVECOMMENT | LI_HAVEQUOTATION) ) {
		for(SHORT i = 0; i < rFmtLine.m_siWordCount; i++ ) {
			FORMATEDWORD & rWord = rFmtLine.m_pWord[i];
			UCHAR ucType = rWord.m_ucType[0];
			if( _IS_BET(WT_RANGE1BEG, ucType, WT_RANGE2END) || _IS_BET(WT_SHADOWON, ucType, WT_HIGHLIGHTOFF) ||
				_IS_BET(WT_LINECOMMENT, ucType, WT_COMMENT2OFF) || _IS_BET(WT_QUOTATION0, ucType, WT_QUOTATION3) )
				_CheckWordType(rFmtLine.m_pString, rWord.m_ucType[0], rWord.m_ucInfo,
				               rWord.m_siIndex, rWord.m_siLength, usLineFlag, szTerminator);
		}
	}

	return bCanStopProcessing;
}

// Same carry-state step, driven off the ANALYZED line instead of the formatted row.
// This is the only producer of carry state once layout is lazy, and it has to be:
// an unformatted row has m_usLineInfo == 0 and m_siWordCount == 0, so the version
// above would look at it, see "nothing interesting on this line", and pass the state
// straight through — silently missing the /* that actually IS there. The analyzer
// knows; the empty row does not.
//
// Writes the INCOMING state onto rFmt (that is what a row's m_usLineFlag means: the
// state at the START of the row) and advances usLineFlag/szTerminator over the line.
// No device context, no allocation.
static BOOL _CheckLineFlagAnalyzed(CAnalyzedString & rAna, CFormatedString & rFmt,
                                   USHORT & usLineFlag, CString & szTerminator)
{
	BOOL bCanStopProcessing = ( (rFmt.m_usLineFlag == usLineFlag) &&
		! rFmt.m_szHereDocumentTerminator.Compare(szTerminator) );

	if( ! bCanStopProcessing ) {
		rFmt.m_usLineFlag = usLineFlag;
		rFmt.m_szHereDocumentTerminator = szTerminator;
	}

	LPCTSTR pString = (LPCTSTR)rAna;

	if( rAna.m_usLineInfo & LI_HAVEHEREDOCUMENT || CHECK_THIS(usLineFlag, LF_QUOTATIONH) ) {
		for(SHORT i = 0; i < rAna.m_siWordCount; i++ ) {
			ANALYZEDWORD & rWord = rAna.m_pWord[i];
			_CheckWordType(pString, rWord.m_ucType[0], rWord.m_ucInfo,
			               rWord.m_siIndex, rWord.m_siLength, usLineFlag, szTerminator);
		}
	} else if( rAna.m_usLineInfo & (LI_HAVERANGE | LI_HAVEHIGHLIGHT | LI_HAVECOMMENT | LI_HAVEQUOTATION) ) {
		for(SHORT i = 0; i < rAna.m_siWordCount; i++ ) {
			ANALYZEDWORD & rWord = rAna.m_pWord[i];
			UCHAR ucType = rWord.m_ucType[0];
			if( _IS_BET(WT_RANGE1BEG, ucType, WT_RANGE2END) || _IS_BET(WT_SHADOWON, ucType, WT_HIGHLIGHTOFF) ||
				_IS_BET(WT_LINECOMMENT, ucType, WT_COMMENT2OFF) || _IS_BET(WT_QUOTATION0, ucType, WT_QUOTATION3) )
				_CheckWordType(pString, rWord.m_ucType[0], rWord.m_ucInfo,
				               rWord.m_siIndex, rWord.m_siLength, usLineFlag, szTerminator);
		}
	}
	// Ordinary lines — no quotes, comments, ranges or heredoc — hit neither branch:
	// the state provably cannot change, so they cost one USHORT test.

	return bCanStopProcessing;
}


// Seed the carry-over syntax state onto every screen row, WITHOUT laying any of
// them out.
//
// Row N's colour depends on every row before it (a /* on line 5 colours line 5,000),
// so this part genuinely has to be eager and sequential. It is also cheap: it walks
// the analyzed lines, and a line with no quote, comment, range or heredoc token in it
// — which is most lines — costs a single USHORT test. No device context, no
// GetTextExtent, no per-line allocation.
//
// The two lists are walked in LOCKSTEP with GetNext. Indexing into a CList is a walk
// from the head, so doing this per row would be O(n^2).
void CCedtView::SeedScreenTextFlags()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument(); ASSERT( pDoc );

	USHORT usLineFlag = 0x0000;
	CString szTerminator;
	BOOL bMultiLineStringConstant = MultiLineStringConstant();

	POSITION po1 = pDoc->m_clsAnalyzedText.GetHeadPosition();
	POSITION po2 = m_clsFormatedScreenText.GetHeadPosition();

	while( po1 && po2 ) {
		// Same per-logical-line reset the formatter does: a line comment never
		// survives the newline, and quotations only survive if the language says so.
		usLineFlag &= ~LF_LINECOMMENT;
		if( ! bMultiLineStringConstant ) usLineFlag &= ~(LF_QUOTATION1 | LF_QUOTATION2 | LF_QUOTATION3);

		CAnalyzedString & rAna = pDoc->m_clsAnalyzedText.GetNext(po1);
		CFormatedString & rFmt = m_clsFormatedScreenText.GetNext(po2);

		_CheckLineFlagAnalyzed( rAna, rFmt, usLineFlag, szTerminator );
	}
}

void CCedtView::FormatScreenText()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	INT nLineCount = pDoc->GetLineCount();

	// One blank row per line, in one go. This used to AddTail a copy of a dummy row
	// 900,000 times, and each of those ran CFormatedString::operator=.
	m_clsFormatedScreenText.RemoveAll();
	m_clsFormatedScreenText.InsertGap(0, nLineCount);

	if( m_bLocalWordWrap ) {
		// Wrapped: a logical line can occupy several rows, and how many depends on
		// the pixel width of every word in it. The vertical scroll bar is sized from
		// the ROW count, so there is no way to know it without laying the whole
		// document out. Stays eager.
		FormatScreenText(0, nLineCount);
	} else {
		// Not wrapped: one row per line, so the row count is already known and
		// nothing but the visible rows needs a layout. Seed the syntax carry state
		// and stop — rows are laid out by EnsureFormattedRange when something
		// actually looks at them.
		SeedScreenTextFlags();
	}
}


// Publish the font/wrap metrics the _Format* helpers read out of file-static
// globals. Must be re-run before every batch of formatting: GetWordWidth /
// GetWordIndex (below) overwrite _nTabMargin for their own purposes, so the
// globals cannot be assumed to survive between calls.
void CCedtView::PrepareFormatMetrics()
{
	// The width cache belongs to whatever font is selected in the DC we are about to
	// measure with. Checking it here — once per batch — rather than per word keeps it
	// off the hot path while still catching a font change, an italic switch, or the
	// printer DC borrowing these same helpers.
	_ResetWidthCacheIfFontChanged( & m_dcScreen );

	CRect rect; GetClientRect( & rect );
	INT nLeftMargin   = GetLeftMargin();
	INT nAveCharWidth = GetAveCharWidth();
	INT nMaxCharWidth = GetMaxCharWidth();

	// save width setting to global variables
	_bFixedPitch = IsUsingFixedPitchFont();
	_bGridLayout = m_bColumnMode;			// screen layout: grid when in column mode
	_nSpaceWidth = GetSpaceWidth();
	_nTabWidth   = m_nTabSize * _nSpaceWidth;
	_nTabMargin  = 1;

	// save wrap setting to gloabl variables
	if( m_nFixedWrapWidth ) _nWrapWidth = m_nFixedWrapWidth * nAveCharWidth;
	else _nWrapWidth = rect.Width() - nLeftMargin - nMaxCharWidth;
	_nIndentWidth = m_nWrapIndentation * _nSpaceWidth;
	_nMinWidth = 8 * nAveCharWidth;
}

// Lay out nCount screen rows starting at nRow, if they are not laid out already.
//
// This is where the expensive work — GetTextExtent per word, new FORMATEDWORD[] per
// row — finally happens, for the handful of rows something is actually about to look
// at. Opening a 900,000-line file lays out the ~50 rows on screen; the rest stay
// blank until the user scrolls to them.
//
// Both lists are indexed ONCE and then advanced in lockstep. Calling this per row
// instead would index a CList per row, and CList indexing walks from the head — with
// the caret at line 800,000 that would be 50 x 800,000 hops per paint, far worse than
// the eager formatting it replaces.
//
// The carry-over syntax state on each row (seeded by SeedScreenTextFlags) survives
// this: _FinishLine no longer clears m_usLineFlag.
void CCedtView::EnsureFormattedRange(INT nRow, INT nCount)
{
	if( m_bLocalWordWrap ) return;		// wrapped documents are formatted eagerly

	INT nTotal = (INT)m_clsFormatedScreenText.GetCount();
	if( nRow < 0 ) { nCount += nRow; nRow = 0; }
	if( nRow >= nTotal || nCount <= 0 ) return;
	if( nRow + nCount > nTotal ) nCount = nTotal - nRow;

	CCedtDoc * pDoc = (CCedtDoc *)GetDocument(); ASSERT( pDoc );

	// Cheap pre-check: the common case is that everything asked for is already laid
	// out (the caret sits on a row that was painted), and then we must not touch the
	// analyzed list at all.
	BOOL bAnyMissing = FALSE;
	POSITION posPeek = m_clsFormatedScreenText.FindIndex(nRow);
	for(INT i = 0; i < nCount && posPeek; i++) {
		if( ! m_clsFormatedScreenText.GetNext(posPeek).m_bFormatted ) { bAnyMissing = TRUE; break; }
	}
	if( ! bAnyMissing ) return;

	PrepareFormatMetrics();

	POSITION po1 = pDoc->m_clsAnalyzedText.FindIndex(nRow);
	POSITION po2 = m_clsFormatedScreenText.FindIndex(nRow);

	for(INT i = 0; i < nCount && po1 && po2; i++) {
		CAnalyzedString & rAna = pDoc->m_clsAnalyzedText.GetNext(po1);
		CFormatedString & rFmt = m_clsFormatedScreenText.GetNext(po2);

		if( rFmt.m_bFormatted ) continue;
		_FormatLineNoWrap( rFmt, rAna, & m_dcScreen );	// sets m_bFormatted
	}
}

void CCedtView::EnsureFormattedAt(INT nRow)
{
	EnsureFormattedRange(nRow, 1);
}


void CCedtView::FormatScreenText(INT nIndex, INT nCount)
{
	PrepareFormatMetrics();

	CCedtDoc * pDoc = (CCedtDoc *)GetDocument(); ASSERT( pDoc );
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pMainFrame );
	INT nProcess = 0; CWaitCursor * pWait = NULL;

	if( nCount > LARGE_FILE_LINE_COUNT ) {
		pWait = new CWaitCursor;
		pMainFrame->BeginProgress(_T("Formatting..."));
	}

	// now start formatting text
	//
	// The analyzed list is only read here, so a POSITION into it is safe to carry. The
	// ROW list is replaced wholesale below, so it is worked in indices — a POSITION would
	// not survive that.
	POSITION po1 = pDoc->m_clsAnalyzedText.FindIndex(nIndex);
	INT nRow = FindScreenTextRow(nIndex);
	if( nRow < 0 ) { if( pWait ) delete pWait; if( nCount > LARGE_FILE_LINE_COUNT ) pMainFrame->EndProgress(); return; }

	USHORT usLineFlag = m_clsFormatedScreenText.ElementAt(nRow).m_usLineFlag;
	CString szTerminator = m_clsFormatedScreenText.ElementAt(nRow).m_szHereDocumentTerminator;
	BOOL bMultiLineStringConstant = MultiLineStringConstant();

	if( m_bLocalWordWrap ) {
		// A wrapped line yields as many rows as it turns out to need, and we do not know
		// how many until we have laid it out. Growing the row list one row at a time is a
		// memmove of every row below it, per row -- quadratic over the document, and
		// measurably so: 2,779 ms for 90,000 wrapped lines against 168 ms on the linked
		// list this array replaced. So the whole run is built off to the side and handed
		// over in one ReplaceRange.
		INT nRowBeg = nRow;
		INT nRowEnd = FindRowPastLines(nRowBeg, nCount);   // rows the old wrapping used

		std::vector<CFormatedString *> vecRows;
		vecRows.reserve( (size_t)(nRowEnd - nRowBeg) );

		while( po1 && nProcess < nCount ) {
			usLineFlag &= ~LF_LINECOMMENT;
			if( ! bMultiLineStringConstant ) usLineFlag &= ~(LF_QUOTATION1 | LF_QUOTATION2 | LF_QUOTATION3);

			CAnalyzedString & rLine = pDoc->m_clsAnalyzedText.GetNext(po1);

			CFormatedString * pRow = new CFormatedString;
			BOOL bContinue = _FormatLineWrap( * pRow, rLine, & m_dcScreen );
			_CheckLineFlag( * pRow, usLineFlag, szTerminator );
			vecRows.push_back( pRow );

			while( bContinue ) {
				pRow = new CFormatedString;
				bContinue = _FormatLineWrapContinue( * pRow, rLine, & m_dcScreen );
				_CheckLineFlag( * pRow, usLineFlag, szTerminator );
				vecRows.push_back( pRow );
			}

			if( nCount > LARGE_FILE_LINE_COUNT && ! (nProcess % 20) ) pMainFrame->SetProgress( 100 * nProcess / nCount );
			nProcess++;
		}

		m_clsFormatedScreenText.ReplaceRange( nRowBeg, nRowEnd - nRowBeg,
		                                      vecRows.empty() ? NULL : & vecRows[0],
		                                      (INT_PTR)vecRows.size() );
		nRow = nRowBeg + (INT)vecRows.size();

	} else {
		// No wrap: one row per line, always. Nothing is inserted or removed, so the rows
		// are laid out where they already sit.
		while( po1 && nProcess < nCount ) {
			usLineFlag &= ~LF_LINECOMMENT;
			if( ! bMultiLineStringConstant ) usLineFlag &= ~(LF_QUOTATION1 | LF_QUOTATION2 | LF_QUOTATION3);

			CAnalyzedString & rLine = pDoc->m_clsAnalyzedText.GetNext(po1);
			_FormatLineNoWrap( m_clsFormatedScreenText.ElementAt(nRow), rLine, & m_dcScreen );
			_CheckLineFlag( m_clsFormatedScreenText.ElementAt(nRow), usLineFlag, szTerminator );
			nRow++;

			if( nCount > LARGE_FILE_LINE_COUNT && ! (nProcess % 20) ) pMainFrame->SetProgress( 100 * nProcess / nCount );
			nProcess++;
		}
	}

	if( nCount > LARGE_FILE_LINE_COUNT ) {
		if( pWait ) delete pWait;
		pMainFrame->EndProgress();
	}

	// Post-process the carry-over state past the edited region, until it re-converges
	// with what the rows already hold. Typing "/*" has to repaint everything below it;
	// typing the matching "*/" has to stop repainting at that line.
	//
	// Nothing below changes the row list, so the count is settled now.
	BOOL bCanStopProcessing = FALSE;
	INT nRowCount = (INT)m_clsFormatedScreenText.GetCount();

	while( nRow < nRowCount && ! bCanStopProcessing ) {
		usLineFlag &= ~LF_LINECOMMENT;
		if( ! bMultiLineStringConstant ) usLineFlag &= ~(LF_QUOTATION1 | LF_QUOTATION2 | LF_QUOTATION3);

		if( m_bLocalWordWrap ) {
			BOOL bContinue = m_clsFormatedScreenText.ElementAt(nRow).m_bLineBreak;
			bCanStopProcessing = _CheckLineFlag( m_clsFormatedScreenText.ElementAt(nRow), usLineFlag, szTerminator );
			nRow++;

			while( bContinue && nRow < nRowCount ) {
				bContinue = m_clsFormatedScreenText.ElementAt(nRow).m_bLineBreak;
				bCanStopProcessing = _CheckLineFlag( m_clsFormatedScreenText.ElementAt(nRow), usLineFlag, szTerminator );
				nRow++;
			}

		} else {
			// Walk the ANALYZED line alongside the row (po1 was advanced in lockstep
			// by the loop above, so it already points here).
			//
			// The rows down here are almost certainly NOT laid out — that is the whole
			// point of lazy layout — and an unformatted row carries no words. Reading
			// the state off the row would therefore see "nothing interesting on this
			// line", pass the state through untouched, never find the */ that closes
			// the comment, and paint the remaining 900,000 lines as one block comment.
			// No crash; just silently wrong colour, on every edit. Read the analyzed
			// line instead, which always has its words.
			if( ! po1 ) break;

			CAnalyzedString & rAna = pDoc->m_clsAnalyzedText.GetNext(po1);
			CFormatedString & rFmt = m_clsFormatedScreenText.ElementAt(nRow);
			nRow++;

			bCanStopProcessing = _CheckLineFlagAnalyzed( rAna, rFmt, usLineFlag, szTerminator );
		}
	}
}


void CCedtView::FormatPrintText(CDC * pDC, RECT rectDraw)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	FormatPrintText(pDC, rectDraw, 0, pDoc->GetLineCount());
}


void CCedtView::FormatPrintText(CDC * pDC, RECT rectDraw, INT nIndex, INT nCount)
{
	// Printing measures with the printer DC and the printer font, at printer
	// resolution — nothing like the screen. Re-key the width cache to it, or every
	// width here would come back measured against the screen font.
	_ResetWidthCacheIfFontChanged( pDC );

	CRect rect( rectDraw );
	INT nLeftMargin   = GetLeftMargin( pDC );
	INT nAveCharWidth = GetAveCharWidth( pDC );
	INT nMaxCharWidth = GetMaxCharWidth( pDC );

	// save width setting to global variables
	_bFixedPitch = IsUsingFixedPitchFont( pDC );
	_bGridLayout = FALSE;			// the printer measures for real, even in column mode
	_nSpaceWidth = GetSpaceWidth( pDC );
	_nTabWidth   = m_nTabSize * _nSpaceWidth;
	_nTabMargin  = _MY_MAX(_nSpaceWidth / 10, 1);

	// save wrap setting to gloabl variables
	_nWrapWidth = rect.Width() - nLeftMargin - nMaxCharWidth;
	_nIndentWidth = m_nWrapIndentation * _nSpaceWidth;
	_nMinWidth = 8 * nAveCharWidth;


	m_clsFormatedPrintText.RemoveAll();

	CCedtDoc * pDoc = (CCedtDoc *)GetDocument(); ASSERT( pDoc );
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pMainFrame );
	INT nProcess = 0; CWaitCursor * pWait = NULL;

	if( nCount > 100 ) {
		pWait = new CWaitCursor;
		pMainFrame->BeginProgress(_T("Formatting..."));
	}

	// now start formatting text
	//
	// Built off to the side and handed over in one go, for the reason FormatScreenText
	// spells out: a wrapped line needs as many rows as it needs, and pushing them into the
	// list one at a time is a memmove per row.
	POSITION po1 = pDoc->m_clsAnalyzedText.FindIndex(nIndex);

	std::vector<CFormatedString *> vecRows;
	vecRows.reserve( (size_t)nCount );

	// The carry-over syntax state to start from is whatever the SCREEN rows already
	// worked out for this line — printing does not re-derive it from the top of the file.
	INT nScreenRow = FindScreenTextRow(nIndex);
	USHORT usLineFlag = 0x0000;
	CString szTerminator;

	if( nScreenRow >= 0 ) {
		usLineFlag = m_clsFormatedScreenText.ElementAt(nScreenRow).m_usLineFlag;
		szTerminator = m_clsFormatedScreenText.ElementAt(nScreenRow).m_szHereDocumentTerminator;
	}

	BOOL bMultiLineStringConstant = MultiLineStringConstant();

	while( po1 && nProcess < nCount ) {
		usLineFlag &= ~LF_LINECOMMENT;
		if( ! bMultiLineStringConstant ) usLineFlag &= ~(LF_QUOTATION1 | LF_QUOTATION2 | LF_QUOTATION3);

		CAnalyzedString & rLine = pDoc->m_clsAnalyzedText.GetNext(po1);

		CFormatedString * pRow = new CFormatedString;
		BOOL bContinue = _FormatLineWrap( * pRow, rLine, pDC );
		_CheckLineFlag( * pRow, usLineFlag, szTerminator );
		vecRows.push_back( pRow );

		while( bContinue ) {
			pRow = new CFormatedString;
			bContinue = _FormatLineWrapContinue( * pRow, rLine, pDC );
			_CheckLineFlag( * pRow, usLineFlag, szTerminator );
			vecRows.push_back( pRow );
		}

		if( nCount > 100 && ! (nProcess % 2) ) pMainFrame->SetProgress( 100 * nProcess / nCount );
		nProcess++;
	}

	m_clsFormatedPrintText.ReplaceRange( 0, 0, vecRows.empty() ? NULL : & vecRows[0],
	                                     (INT_PTR)vecRows.size() );

	if( nCount > 100 ) {
		if( pWait ) delete pWait;
		pMainFrame->EndProgress();
	}
}


// Drop nCount logical lines, starting at line nIndex, from the screen rows. The carry-over
// syntax flag from the first of them survives onto whatever now sits in its place.
void CCedtView::RemoveScreenText(INT nIndex, INT nCount)
{
	if( nCount <= 0 ) return;

	INT nRow = FindScreenTextRow( nIndex );
	if( nRow < 0 ) return;

	USHORT usLineFlag = m_clsFormatedScreenText.ElementAt( nRow ).m_usLineFlag;

	// Count the rows those nCount lines occupy — one each, plus their continuation rows
	// when wrapped — and remove them in ONE call. Removing a line at a time was free on
	// the linked list; on the array it is a memmove each, so deleting a large selection
	// would have gone quadratic.
	INT nTotal = (INT)m_clsFormatedScreenText.GetCount();
	INT nRows = 0;

	for(INT i = 0; i < nCount && nRow + nRows < nTotal; i++)
		nRows += _ParagraphRowCount(m_clsFormatedScreenText, nRow + nRows, nTotal);

	m_clsFormatedScreenText.RemoveRange(nRow, nRows);

	if( nRow < (INT)m_clsFormatedScreenText.GetCount() )
		m_clsFormatedScreenText.ElementAt( nRow ).m_usLineFlag = usLineFlag;
}

// Make room for nCount new logical lines after line nIndex. They are laid out later, by
// FormatScreenText; blank rows are all that is needed here.
void CCedtView::InsertScreenText(INT nIndex, INT nCount)
{
	if( nCount <= 0 ) return;

	// nIndex < 0 means "before the first line".
	INT nRow = 0;
	if( nIndex >= 0 ) {
		INT nFound = FindScreenTextRow( nIndex );
		nRow = ( nFound < 0 ) ? 0 : nFound + 1;
	}

	m_clsFormatedScreenText.InsertGap(nRow, nCount);		// one structural change
}


// Display cells (0, 1 or 2) of the character starting at nIdxX — the column-mode grid unit.
// See docs/refactoring-column-mode.md. This is the GDI half of the classifier: it measures
// the advance and hands it, with the code point and the narrow cell, to the pure CellsFor.
// One _MeasureRun call, which reads the per-character advance cache built for formatting.
//
// The grid is only meaningful in a fixed-pitch font — a proportional one has no single narrow
// cell to be a multiple of. Column mode already forces fixed pitch (CreateScreenFontObject
// substitutes the Column Mode font); the assert records that the model now depends on it.
INT CCedtView::GetCharCells(LPCTSTR lpszLine, INT nIdxX, INT nLength, CDC * pDC)
{
	if( pDC == NULL ) pDC = & m_dcScreen;
	ASSERT( IsUsingFixedPitchFont(pDC) );

	_ResetWidthCacheIfFontChanged( pDC );
	return _CharCells(pDC, lpszLine, nIdxX, nLength);
}

INT CCedtView::GetWordWidth(LPCTSTR lpWord, SHORT siLength, INT nPosition, UCHAR ucType, CDC * pDC)
{
	// save width setting to global variables
	_bFixedPitch = IsUsingFixedPitchFont( pDC );
	_bGridLayout = m_bColumnMode && ( pDC == NULL || pDC == & m_dcScreen );	// screen only
	_nSpaceWidth = GetSpaceWidth( pDC );
	_nTabWidth   = m_nTabSize * _nSpaceWidth;
	_nTabMargin  = _MY_MAX(_nSpaceWidth / 10, 1);

	if( pDC ) return _GetWordWidth(lpWord, siLength, nPosition, ucType, pDC);
	else return _GetWordWidth(lpWord, siLength, nPosition, ucType, & m_dcScreen);
}

SHORT CCedtView::GetWordIndex(LPCTSTR lpWord, SHORT siLength, INT nWidth, CDC * pDC)
{
	// save width setting to global variables
	_bFixedPitch = IsUsingFixedPitchFont( pDC );
	_bGridLayout = m_bColumnMode && ( pDC == NULL || pDC == & m_dcScreen );	// screen only
	_nSpaceWidth = GetSpaceWidth( pDC );
	_nTabWidth   = m_nTabSize * _nSpaceWidth;
	_nTabMargin  = _MY_MAX(_nSpaceWidth / 10, 1);

	if( pDC ) return _GetWordIndex(lpWord, siLength, nWidth, pDC);
	else return _GetWordIndex(lpWord, siLength, nWidth, & m_dcScreen);
}


// Logical line -> POSITION of its first screen row.
//
// With word wrap OFF that is just row nIndex. FindIndex still walks the list, but
// it walks it WITHOUT READING the rows — which is what matters here, because with
// lazy layout most of the rows it passes have no m_pWord yet.
// These used to hand POSITIONs around, and two of them removed rows WHILE holding a
// POSITION into the list they were removing from. On the linked list that was fine — a
// POSITION was a node pointer, so deleting some other node left it alone. On the array
// underneath now it is not: remove row i and everything after shifts down, so the held
// POSITION names the wrong row. No crash, just the wrong line rendered or edited.
//
// So they work in ROW INDICES, which is what they were really doing all along, and they
// remove a run of rows in one call rather than one at a time.


// Row index of the first screen row of logical line nIndex. -1 if there are no rows.
//
// With word wrap off, row == line. With it on, a line owns its first row (m_siSplitIndex
// == 0) plus however many continuation rows follow it.
INT CCedtView::FindScreenTextRow(INT nIndex)
{
	INT nTotal = (INT)m_clsFormatedScreenText.GetCount();
	if( nTotal == 0 ) return -1;

	if( ! m_bLocalWordWrap ) {
		if( nIndex < 0 ) return 0;
		return ( nIndex < nTotal ) ? nIndex : nTotal - 1;
	}

	INT nParaCount = 0;
	for(INT nRow = 0; nRow < nTotal; nRow++) {
		if( m_clsFormatedScreenText.ElementAt(nRow).m_siSplitIndex == 0 ) nParaCount++;
		if( nParaCount - 1 == nIndex ) return nRow;
	}

	return nTotal - 1;		// past the end: the old code returned the last row too
}

// The row just past the nCount logical lines that begin at row nRowBeg -- i.e. how many
// rows those lines currently occupy. nRowBeg must be a line's FIRST row.
//
// With wrap off that is nRowBeg + nCount. With it on, a line owns its first row plus its
// continuation rows, so they have to be counted.
INT CCedtView::FindRowPastLines(INT nRowBeg, INT nCount)
{
	INT nTotal = (INT)m_clsFormatedScreenText.GetCount();
	if( ! m_bLocalWordWrap ) return _MY_MIN(nRowBeg + nCount, nTotal);

	INT nRow = nRowBeg, nSeen = 0;

	while( nRow < nTotal ) {
		if( m_clsFormatedScreenText.ElementAt(nRow).m_siSplitIndex == 0 ) {
			if( nSeen == nCount ) break;   // the row that starts the line after our last
			nSeen++;
		}
		nRow++;
	}

	return nRow;
}


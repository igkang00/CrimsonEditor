#include "stdafx.h"
#include "cedtHeader.h"



INT CCedtView::GetLastIdxY()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->GetLastIdxY();
}

INT CCedtView::GetLastPosY()
{
	INT nLineHeight = GetLineHeight();
	return nLineHeight * (INT)(m_clsFormatedScreenText.GetCount()-1);
}

INT CCedtView::GetFirstIdxX(CAnalyzedString & rLine)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->GetFirstIdxX( rLine );
}

INT CCedtView::GetFirstIdxX(CFormatedString & rLine)
{
	FORMATEDWORD & rWord = rLine.m_pWord[0];
	return rWord.m_siIndex;
}

INT CCedtView::GetLastIdxX(CAnalyzedString & rLine)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->GetLastIdxX( rLine );
}

INT CCedtView::GetLastIdxX(CFormatedString & rLine)
{
	FORMATEDWORD & rWord = rLine.m_pWord[rLine.m_siWordCount-1];
	return rWord.m_siIndex + rWord.m_siLength;
}

INT CCedtView::GetFirstNonBlankIdxX(CAnalyzedString & rLine)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->GetFirstNonBlankIdxX( rLine );
}


// --- the column coordinate (column mode) ------------------------------------
//
// Thin GDI-side wrappers over the pure walk in cedtCharWidth.h. The line length is taken
// from the string itself, not GetLastIdxX, so these do not depend on the row being laid out.

// Static, so it can be a plain function pointer; recovers the view from the context and asks
// it to measure. GetCharCells asserts fixed pitch — the walk only calls it off tab characters.
INT CCedtView::CellCallback(void * pCtx, LPCTSTR psz, INT nIdxX, INT nLen)
{
	return ((CCedtView *)pCtx)->GetCharCells(psz, nIdxX, nLen);
}

INT CCedtView::GetColumnFromIdxX(CFormatedString & rLine, INT nIdxX)
{
	LPCTSTR psz = (LPCTSTR)rLine; if( psz == NULL ) return 0;
	return ColumnFromIdxX(psz, (INT)_tcslen(psz), nIdxX, m_nTabSize, CellCallback, this);
}

INT CCedtView::GetIdxXFromColumn(CFormatedString & rLine, INT nColumn)
{
	LPCTSTR psz = (LPCTSTR)rLine; if( psz == NULL ) return 0;
	return IdxXFromColumn(psz, (INT)_tcslen(psz), nColumn, m_nTabSize, CellCallback, this);
}

INT CCedtView::GetLastColumn(CFormatedString & rLine)
{
	LPCTSTR psz = (LPCTSTR)rLine; if( psz == NULL ) return 0;
	return LastColumn(psz, (INT)_tcslen(psz), m_nTabSize, CellCallback, this);
}

// Display width of a bare string, in cells. For the block clipboard, whose lines are plain
// CStrings rather than rows. Tabs are measured from column 0 — the block was cut out of its
// original tab stops, and it is about to land somewhere else anyway.
INT CCedtView::GetStringColumns(LPCTSTR psz)
{
	if( psz == NULL ) return 0;
	return LastColumn(psz, (INT)_tcslen(psz), m_nTabSize, CellCallback, this);
}

INT CCedtView::GetTrailingBlankIdxX(CAnalyzedString & rLine)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->GetTrailingBlankIdxX( rLine );
}


INT CCedtView::GetNextTabPosition(INT nPosX)
{
	INT nSpaceWidth = GetSpaceWidth();
	INT nTabWidth = m_nTabSize * nSpaceWidth;

	INT nNextPosX = ((nPosX + nSpaceWidth - 1) / nTabWidth + 1) * nTabWidth;
	return nNextPosX;
}

INT CCedtView::GetPrevTabPosition(INT nPosX)
{
	INT nSpaceWidth = GetSpaceWidth();
	INT nTabWidth = m_nTabSize * nSpaceWidth;

	INT nPrevPosX = ((nPosX - 1) / nTabWidth) * nTabWidth;
	return (nPrevPosX > 0) ? nPrevPosX : 0;
}


INT CCedtView::GetFirstPosX(CFormatedString & rLine) 
{
	FORMATEDWORD & rWord = rLine.m_pWord[0];
	return rWord.m_nPosition;
}

INT CCedtView::GetLastPosX(CFormatedString & rLine) 
{
	FORMATEDWORD & rWord = rLine.m_pWord[rLine.m_siWordCount-1];
	return rWord.m_nPosition + rWord.m_nWidth;
}

INT CCedtView::GetFirstNonBlankPosX(CFormatedString & rLine) 
{
	SHORT i, siWordCount = rLine.m_siWordCount;
	for(i = 0; i < siWordCount; i++) {
		FORMATEDWORD & rWord = rLine.m_pWord[i]; 
		if( ! IS_WSPACE(rWord) ) return rWord.m_nPosition;
	}
	return GetLastPosX(rLine);
}

INT CCedtView::GetTrailingBlankPosX(CFormatedString & rLine)
{
	SHORT i, siWordCount = rLine.m_siWordCount;
	for(i = siWordCount-1; i >= 0; i--) {
		FORMATEDWORD & rWord = rLine.m_pWord[i];
		if( ! IS_WSPACE(rWord) ) return rWord.m_nPosition + rWord.m_nWidth;
	}
	return GetFirstPosX(rLine);
}

CAnalyzedString & CCedtView::GetLineFromIdxY(INT nIdxY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->GetLineFromIdxY(nIdxY);
}

// THE gateway: every caret, edit, move, search, highlight and metric consumer gets
// its CFormatedString from here. Laying the row out on the way through is therefore
// all it takes to make lazy layout invisible to the rest of the editor.
CFormatedString & CCedtView::GetLineFromPosY(INT nPosY)
{
	INT nLineIndex = nPosY / GetLineHeight();

	EnsureFormattedAt( nLineIndex );

	POSITION pos = m_clsFormatedScreenText.FindIndex(nLineIndex);
	if( pos ) return m_clsFormatedScreenText.GetAt(pos);
	return m_clsFormatedScreenText.GetTail();
}

ANALYZEDWORD & CCedtView::GetWordFromIdxX(CAnalyzedString & rLine, INT nIdxX)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->GetWordFromIdxX(rLine, nIdxX);
}

FORMATEDWORD & CCedtView::GetWordFromPosX(CFormatedString & rLine, INT nPosX)
{
	SHORT i, siWordCount = rLine.m_siWordCount;
	for(i = 0; i < siWordCount; i++) {
		FORMATEDWORD & rWord = rLine.m_pWord[i];
		if( rWord.m_nPosition + rWord.m_nWidth > nPosX ) return rLine.m_pWord[i];
	}
	return rLine.m_pWord[siWordCount-1];
}

FORMATEDWORD & CCedtView::GetWordFromIdxX(CFormatedString & rLine, INT nIdxX)
{
	SHORT i, siWordCount = rLine.m_siWordCount;
	for(i = 0; i < siWordCount; i++) {
		FORMATEDWORD & rWord = rLine.m_pWord[i];
		if( rWord.m_siIndex + rWord.m_siLength > nIdxX ) return rLine.m_pWord[i];
	}
	return rLine.m_pWord[siWordCount-1];
}

// Screen row -> logical line.
//
// With word wrap OFF a logical line is exactly one screen row, so this is the
// identity and can be answered by arithmetic. That is worth doing: the loop below
// walks the row list from the head, and it is called on every paint and every
// caret move — on a 900,000-line file that is a 900,000-node walk per keystroke.
// It is also the walk that would touch rows that have not been laid out yet.
INT CCedtView::GetIdxYFromPosY(INT nPosY)
{
	INT nLineIndex = nPosY / GetLineHeight();

	if( ! m_bLocalWordWrap ) {
		INT nCount = (INT)m_clsFormatedScreenText.GetCount();
		if( nLineIndex < 0 || nLineIndex >= nCount ) return nCount - 1;	// same clamp as the loop
		return nLineIndex;
	}

	INT nParaCount = 0, nLineCount = 0;

	POSITION pos = m_clsFormatedScreenText.GetHeadPosition();
	while( pos ) {
		CFormatedString & rLine = m_clsFormatedScreenText.GetNext( pos );
		nLineCount++; if( rLine.m_siSplitIndex == 0 ) nParaCount++;
		if( nLineIndex == nLineCount-1 ) return nParaCount-1;
	}
	return nParaCount-1;
}

INT CCedtView::GetIdxXFromPosX(CFormatedString & rLine, INT nPosX, BOOL bAdjust)
{
	INT nFirstPosX, nLastPosX;

	if( nPosX < (nFirstPosX = GetFirstPosX(rLine) ) ) {
		return GetFirstIdxX( rLine );
	} else if( nPosX < (nLastPosX = GetLastPosX(rLine) ) ) {
		FORMATEDWORD & rWord = GetWordFromPosX(rLine, nPosX);
		return GetIdxXFromPosX(rLine, rWord, nPosX, bAdjust);
	} else {
		if( bAdjust ) return GetLastIdxX(rLine);
		else return GetLastIdxX(rLine) + (nPosX - nLastPosX) / GetAveCharWidth();
	}
}

INT CCedtView::GetIdxXFromPosX(CFormatedString & rLine, FORMATEDWORD & rWord, INT nPosX, BOOL bAdjust)
{
	if( rWord.m_nPosition + rWord.m_nWidth <= nPosX ) return rWord.m_siIndex + rWord.m_siLength;
	if( IS_SINGLE(rWord) || rWord.m_nPosition >= nPosX ) return rWord.m_siIndex;
	return rWord.m_siIndex + GetWordIndex((LPCTSTR)rLine + rWord.m_siIndex, rWord.m_siLength, nPosX - rWord.m_nPosition);
}

// Logical line -> screen Y.
//
// With word wrap OFF row index == line index, so the answer is nIdxY * height for
// any in-range line, whatever nIdxX is: the loop below only consults nIdxX to pick
// WHICH wrapped row of the line the caret sits on, and there is only one.
//
// Taking the arithmetic path also removes the GetLastIdxX(rLine) call, which reads
// rLine.m_pWord[m_siWordCount-1] on every row it merely walks past. That would
// dereference NULL[-1] on a row that has not been laid out yet — it is the single
// reason lazy layout could not simply be dropped in.
INT CCedtView::GetPosYFromIdxY(INT nIdxX, INT nIdxY, BOOL bAdjust)
{
	INT nLineHeight = GetLineHeight();

	if( ! m_bLocalWordWrap ) {
		INT nCount = (INT)m_clsFormatedScreenText.GetCount();
		if( nIdxY < 0 ) return 0;
		if( nIdxY >= nCount ) return (nCount - 1) * nLineHeight;
		return nIdxY * nLineHeight;
	}

	INT nParaCount = 0, nLineCount = 0;

	POSITION pos = m_clsFormatedScreenText.GetHeadPosition();
	while( pos ) {
		CFormatedString & rLine = m_clsFormatedScreenText.GetNext( pos );
		nLineCount++; if( rLine.m_siSplitIndex == 0 ) nParaCount++;
		if( nIdxY <  nParaCount-1 ) return (nLineCount-2) * nLineHeight;
		if( nIdxY == nParaCount-1 && GetLastIdxX( rLine ) > nIdxX ) return (nLineCount-1) * nLineHeight;
	}
	return (nLineCount-1) * nLineHeight;
}

INT CCedtView::GetPosXFromIdxX(CFormatedString & rLine, INT nIdxX, BOOL bAdjust)
{
	INT nFirstIdxX, nLastIdxX;

	if( nIdxX < (nFirstIdxX = GetFirstIdxX(rLine) ) ) {
		return GetFirstPosX( rLine );
	} else if( nIdxX < (nLastIdxX = GetLastIdxX(rLine) ) ) {
		FORMATEDWORD & rWord = GetWordFromIdxX(rLine, nIdxX);
		return GetPosXFromIdxX(rLine, rWord, nIdxX, bAdjust);
	} else {
		if( bAdjust ) return GetLastPosX(rLine);
		else return GetLastPosX(rLine) + (nIdxX - nLastIdxX) * GetAveCharWidth();
	}
}

INT CCedtView::GetPosXFromIdxX(CFormatedString & rLine, FORMATEDWORD & rWord, INT nIdxX, BOOL bAdjust)
{
	// Defensive: a stray mid-pair index would make GetWordWidth below measure a
	// prefix ending on a lone high surrogate, which GDI sizes as .notdef.
	nIdxX = SnapIdxX((LPCTSTR)rLine, nIdxX);

	if( rWord.m_siIndex + rWord.m_siLength <= nIdxX ) return rWord.m_nPosition + rWord.m_nWidth;
	if( IS_SINGLE(rWord) || rWord.m_siIndex >= nIdxX ) return rWord.m_nPosition;
	return rWord.m_nPosition + GetWordWidth((LPCTSTR)rLine + rWord.m_siIndex, nIdxX - rWord.m_siIndex, rWord.m_nPosition, rWord.m_ucType[0]);
}


BOOL CCedtView::IsBlankLine(CAnalyzedString & rLine)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->IsBlankLine(rLine);
}

BOOL CCedtView::IsBlankLineFromIdxY(INT nIdxY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->IsBlankLineFromIdxY(nIdxY);
}

BOOL CCedtView::IsBlankLine(CFormatedString & rLine)
{
	SHORT i, siWordCount = rLine.m_siWordCount;
	for(i = 0; i < siWordCount; i++) {
		FORMATEDWORD & rWord = rLine.m_pWord[i];
		if( ! IS_WSPACE(rWord) ) return FALSE;
	}
	return TRUE;
}

BOOL CCedtView::IsBlankLineFromPosY(INT nPosY)
{
	CFormatedString & rLine = GetLineFromPosY(nPosY);
	return IsBlankLine(rLine);
}


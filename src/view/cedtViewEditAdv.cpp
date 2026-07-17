#include "stdafx.h"
#include "cedtHeader.h"



void CCedtView::ActionDeleteLineSelection()
{
	INT nBegX, nBegY, nEndX, nEndY; GetSelectedIndex(nBegX, nBegY, nEndX, nEndY);

	if( nBegY != nEndY ) DeleteLineSelection(nBegX, nBegY, nEndX, nEndY);
	else DeleteString(nBegX, nBegY, nEndX-nBegX);

	SetCaretPosY( GetPosYFromIdxY( nBegX, nBegY ) );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLine, nBegX ) );
}

void CCedtView::ActionDeleteColumnSelection()
{
	INT nBegX, nBegY, nEndX, nEndY; GetSelectedPosition(nBegX, nBegY, nEndX, nEndY);

	DeleteColumnSelection(nBegX, nBegY, nEndX, nEndY);

	SetCaretPosY( nBegY ); m_nAnchorPosY = nEndY;
	SetCaretPosX( nBegX ); m_nAnchorPosX = nBegX;
}

void CCedtView::ActionDeleteColumnChar()
{
	INT nLineHeight = GetLineHeight();
	INT nBegX, nBegY, nEndX, nEndY; GetSelectedPosition(nBegX, nBegY, nEndX, nEndY);

	for(INT nPosY = nBegY; nPosY <= nEndY; nPosY += nLineHeight ) {
		CFormatedString & rLine = GetLineFromPosY( nPosY );
		INT nIdxY = GetIdxYFromPosY( nPosY ), nLstX = GetLastPosX( rLine );

		if( nBegX < nLstX ) {
			INT nIdxX = GetIdxXFromBlockEdge( rLine, nBegX );
			DeleteChar(nIdxX, nIdxY);
		}
	}

	SetCaretPosY( nBegY ); m_nAnchorPosY = nEndY;
	SetCaretPosX( nBegX ); m_nAnchorPosX = nBegX;
}

void CCedtView::ActionDeleteColumnPrevChar()
{
	INT nLineHeight = GetLineHeight();
	INT nBegX, nBegY, nEndX, nEndY; GetSelectedPosition(nBegX, nBegY, nEndX, nEndY);

	// The block steps back exactly one COLUMN, and each row then deletes one whole character.
	// A row whose character was two cells wide shrinks by two, one whose character was narrow
	// shrinks by one — so the rows end up different lengths, and that is right: what was
	// deleted really was of different widths. The block is a column, not a property of the
	// text, and it owes the text no alignment after the text has changed.
	if( nBegX <= 0 ) return;
	else nBegX = nEndX = nBegX - GetSpaceWidth();

	for(INT nPosY = nBegY; nPosY <= nEndY; nPosY += nLineHeight ) {
		CFormatedString & rLine = GetLineFromPosY( nPosY );
		INT nIdxY = GetIdxYFromPosY( nPosY ), nLstX = GetLastPosX( rLine );

		if( nBegX < nLstX ) {
			INT nIdxX = GetIdxXFromBlockEdge( rLine, nBegX );
			DeleteChar(nIdxX, nIdxY);
		}
	}

	SetCaretPosY( nBegY ); m_nAnchorPosY = nEndY;
	SetCaretPosX( nBegX ); m_nAnchorPosX = nBegX;
}

void CCedtView::ActionDeleteColumnToEndOfLine()
{
	INT nLineHeight = GetLineHeight();
	INT nBegX, nBegY, nEndX, nEndY; GetSelectedPosition(nBegX, nBegY, nEndX, nEndY);

	for(INT nPosY = nBegY; nPosY <= nEndY; nPosY += nLineHeight ) {
		CFormatedString & rLine = GetLineFromPosY( nPosY );
		INT nIdxY = GetIdxYFromPosY( nPosY ), nLstX = GetLastPosX( rLine );

		if( nBegX < nLstX ) {
			INT nIdxX = GetIdxXFromBlockEdge( rLine, nBegX );
			INT nLdxX = GetIdxXFromPosX( rLine, nLstX, TRUE );
			DeleteString(nIdxX, nIdxY, nLdxX - nIdxX);
		}
	}

	SetCaretPosY( nBegY ); m_nAnchorPosY = nEndY;
	SetCaretPosX( nBegX ); m_nAnchorPosX = nBegX;
}

void CCedtView::ActionDeleteColumnToBeginOfLine()
{
	INT nLineHeight = GetLineHeight();
	INT nBegX, nBegY, nEndX, nEndY; GetSelectedPosition(nBegX, nBegY, nEndX, nEndY);

	if( nBegX <= 0 ) return;

	for(INT nPosY = nBegY; nPosY <= nEndY; nPosY += nLineHeight ) {
		CFormatedString & rLine = GetLineFromPosY( nPosY );
		INT nIdxY = GetIdxYFromPosY( nPosY ), nLstX = GetLastPosX( rLine );

		INT nIdxX = ( nBegX < nLstX )
			? GetIdxXFromBlockEdge( rLine, nBegX )
			: GetIdxXFromPosX( rLine, nLstX, TRUE );
		DeleteString(0, nIdxY, nIdxX);
	}

	nBegX = nEndX = 0;
	SetCaretPosY( nBegY ); m_nAnchorPosY = nEndY;
	SetCaretPosX( nBegX ); m_nAnchorPosX = nBegX;
}

void CCedtView::ActionCopyLineSelection(CMemText & rBlock)
{
	rBlock.RemoveAll(); INT nBegX, nBegY, nEndX, nEndY; 
	GetSelectedIndex(nBegX, nBegY, nEndX, nEndY);

	if( nBegY != nEndY ) CopyToLineSelection(rBlock, nBegX, nBegY, nEndX, nEndY);
	else { rBlock.AddTail(_T("")); CopyToString(rBlock.GetTail(), nBegX, nBegY, nEndX-nBegX); }
}

void CCedtView::ActionCopyColumnSelection(CMemText & rBlock)
{
	rBlock.RemoveAll(); INT nBegX, nBegY, nEndX, nEndY; 
	GetSelectedPosition(nBegX, nBegY, nEndX, nEndY);

	CopyToColumnSelection(rBlock, nBegX, nBegY, nEndX, nEndY);
}

void CCedtView::ActionCopyLine(CMemText & rBlock)
{
	rBlock.RemoveAll(); INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );

	rBlock.AddTail( GetLineFromIdxY( nIdxY ) );
	rBlock.AddTail( _T("") );
}

void CCedtView::ActionCopyFilePath(CMemText & rBlock)
{
	rBlock.RemoveAll(); CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	CString szPathName = pDoc->GetTitle();

	rBlock.AddTail(szPathName);
}

void CCedtView::ActionPasteLineSelection(CMemText & rBlock)
{
	INT nBegX, nBegY; PositionToIndex( m_nCaretPosX, m_nCaretPosY, nBegX, nBegY );
	INT nEndX = nBegX, nEndY = nBegY;

	if( rBlock.GetCount() == 1 ) {
		LPCTSTR lpszString = rBlock.GetTail();
		InsertString(nBegX, nBegY, lpszString);
		nEndX = nBegX + (INT)_tcslen(lpszString);
	} else if( rBlock.GetCount() > 1 ) InsertLineSelection(nBegX, nBegY, nEndX, nEndY, rBlock);

	SetCaretPosY( GetPosYFromIdxY( nEndX, nEndY ) );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLine, nEndX ) );
}

void CCedtView::ActionPasteColumnSelection(CMemText & rBlock)
{
	INT nBegX = m_nCaretPosX, nBegY = m_nCaretPosY;
	INT nEndX = nBegX, nEndY = nBegY;

	InsertColumnSelection(nBegX, nBegY, nEndX, nEndY, rBlock);

	SetCaretPosY( nBegY ); m_nAnchorPosY = nBegY;
	SetCaretPosX( nEndX ); m_nAnchorPosX = nEndX;
}


void CCedtView::ActionChangeLineSelection(CMemText & rBlock)
{
	INT nBegX, nBegY, nEndX, nEndY;
	GetSelectedIndex(nBegX, nBegY, nEndX, nEndY);

	if( nBegY != nEndY ) DeleteLineSelection(nBegX, nBegY, nEndX, nEndY);
	else DeleteString(nBegX, nBegY, nEndX-nBegX);

	if( nBegY != nEndY ) InsertLineSelection(nBegX, nBegY, nEndX, nEndY, rBlock);
	else InsertString(nBegX, nBegY, rBlock.GetTail());
}

void CCedtView::ActionChangeColumnSelection(CMemText & rBlock)
{
	INT nBegX, nBegY, nEndX, nEndY;
	GetSelectedPosition(nBegX, nBegY, nEndX, nEndY);

	DeleteColumnSelection(nBegX, nBegY, nEndX, nEndY);
	InsertColumnSelection(nBegX, nBegY, nEndX, nEndY, rBlock);
}


void CCedtView::ActionIndentLineSelection()
{
	INT nBegX, nBegY, nEndX, nEndY;
	GetSelectedIndex(nBegX, nBegY, nEndX, nEndY);

	ArrangeLineSelection(nBegX, nBegY, nEndX, nEndY);
	IndentLineSelection(nBegX, nBegY, nEndX, nEndY);

	SetCaretPosY( GetPosYFromIdxY( nBegX, nBegY ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nBegX ) );

	m_nAnchorPosY = GetPosYFromIdxY( nEndX, nEndY );
	CFormatedString & rLne3 = GetLineFromPosY( m_nAnchorPosY );
	m_nAnchorPosX = GetPosXFromIdxX( rLne3, nEndX );
}

void CCedtView::ActionUnindentLineSelection()
{
	INT nBegX, nBegY, nEndX, nEndY;
	GetSelectedIndex(nBegX, nBegY, nEndX, nEndY);

	ArrangeLineSelection(nBegX, nBegY, nEndX, nEndY);
	UnindentLineSelection(nBegX, nBegY, nEndX, nEndY);

	SetCaretPosY( GetPosYFromIdxY( nBegX, nBegY ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nBegX ) );

	m_nAnchorPosY = GetPosYFromIdxY( nEndX, nEndY );
	CFormatedString & rLne3 = GetLineFromPosY( m_nAnchorPosY );
	m_nAnchorPosX = GetPosXFromIdxX( rLne3, nEndX );
}

// Comment out a column block by wrapping what it covers on each row in the BLOCK comment
// delimiters — /* ... */ — rather than putting a line comment at column 0.
//
// A line comment kills the whole line, which makes the block's columns meaningless: the user
// picked 3..7 and would get all of it commented. Block delimiters are the tool that matches
// what a column block means, because commenting out part of a line is exactly what they are
// for. A language without them (Python) cannot express this, and the caller beeps rather than
// pretending — see EventMakeComment.
//
// The block grows to cover the delimiters it just added, so Uncomment right after Comment
// finds what it needs and round-trips.
void CCedtView::ActionMakeCommentColumnSelection()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();

	// A space inside each delimiter, the way FastMakeCommentLine writes "// " — same reason,
	// and the two commands should not disagree about it.
	CString szOn  = pDoc->GetBlockCommentOn() + _T(" ");
	CString szOff = _T(" ") + pDoc->GetBlockCommentOff();

	INT nLineHeight = GetLineHeight(), nSpaceWidth = GetSpaceWidth();
	INT nBegX, nBegY, nEndX, nEndY; GetSelectedPosition(nBegX, nBegY, nEndX, nEndY);

	for(INT nPosY = nBegY; nPosY <= nEndY; nPosY += nLineHeight ) {
		INT nIdxY = GetIdxYFromPosY( nPosY );

		CFormatedString & rLine = GetLineFromPosY( nPosY );
		INT nLstX = GetLastPosX( rLine );
		INT nIdx1 = GetIdxXFromBlockEdge( rLine, nBegX );
		INT nIdx2 = GetIdxXFromBlockEdge( rLine, nEndX );
		if( nIdx1 >= nIdx2 ) continue;		// nothing of this row is inside the block

		// A row that stops short of the block's right edge gets padded out to it, so the
		// closing delimiter stands on the edge and the comment is the same rectangle the
		// block is. Without this the delimiters land ragged, wherever each row happens to end.
		if( nEndX > nLstX ) {
			InsertString(nIdx2, nIdxY, CString(_T(' '), (nEndX - nLstX) / nSpaceWidth));
			CFormatedString & rPadded = GetLineFromPosY( nPosY );	// re-fetch: the row changed
			nIdx2 = GetIdxXFromBlockEdge( rPadded, nEndX );
		}

		// Back to front: inserting at nIdx2 first leaves nIdx1 where it was.
		InsertString(nIdx2, nIdxY, szOff);
		InsertString(nIdx1, nIdxY, szOn);
	}

	nEndX = nEndX + ( GetStringColumns(szOn) + GetStringColumns(szOff) ) * nSpaceWidth;

	SetCaretPosY( nBegY ); m_nAnchorPosY = nEndY;
	SetCaretPosX( nBegX ); m_nAnchorPosX = nEndX;
}

// The inverse: on each row, if the block starts with /* and ends with */, take both away.
//
// A row that does not match is left alone silently — no beep. A block often covers rows where
// only some are commented, and refusing the whole operation because of one of them would be
// worse than doing the part that makes sense.
// How many characters at psz match lpsz, trying the spaced form first and falling back to the
// bare one — "/* " then "/*". Same tolerance FastReleaseCommentLine shows for "// " and "//",
// and for the same reason: we write the spaced form, but a human may have typed either.
// Returns 0 for no match.
static INT _MatchDelimiter(LPCTSTR psz, LPCTSTR lpszSpaced, LPCTSTR lpszBare)
{
	INT nSpaced = (INT)_tcslen(lpszSpaced), nBare = (INT)_tcslen(lpszBare);

	if( _tcsncmp(psz, lpszSpaced, nSpaced) == 0 ) return nSpaced;
	if( _tcsncmp(psz, lpszBare,   nBare  ) == 0 ) return nBare;
	return 0;
}

void CCedtView::ActionReleaseCommentColumnSelection()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();

	CString szOnBare = pDoc->GetBlockCommentOn(), szOffBare = pDoc->GetBlockCommentOff();
	CString szOn = szOnBare + _T(" "), szOff = _T(" ") + szOffBare;

	INT nLineHeight = GetLineHeight(), nSpaceWidth = GetSpaceWidth();
	INT nBegX, nBegY, nEndX, nEndY; GetSelectedPosition(nBegX, nBegY, nEndX, nEndY);
	INT nReleased = 0;

	for(INT nPosY = nBegY; nPosY <= nEndY; nPosY += nLineHeight ) {
		INT nIdxY = GetIdxYFromPosY( nPosY );

		CFormatedString & rLine = GetLineFromPosY( nPosY );
		INT nIdx1 = GetIdxXFromBlockEdge( rLine, nBegX );
		INT nIdx2 = GetIdxXFromBlockEdge( rLine, nEndX );
		if( nIdx1 >= nIdx2 ) continue;

		CAnalyzedString & rText = GetLineFromIdxY( nIdxY );

		INT nLenOn = _MatchDelimiter((LPCTSTR)rText + nIdx1, szOn, szOnBare);
		if( ! nLenOn ) continue;

		// The closing delimiter's length decides where it starts, so try each candidate at
		// its own offset rather than picking a length first.
		INT nLenOff = 0;
		if( nIdx2 - szOff.GetLength() >= nIdx1 &&
		    _tcsncmp((LPCTSTR)rText + nIdx2 - szOff.GetLength(), szOff, szOff.GetLength()) == 0 )
			nLenOff = szOff.GetLength();
		else if( nIdx2 - szOffBare.GetLength() >= nIdx1 &&
		         _tcsncmp((LPCTSTR)rText + nIdx2 - szOffBare.GetLength(), szOffBare, szOffBare.GetLength()) == 0 )
			nLenOff = szOffBare.GetLength();
		if( ! nLenOff ) continue;

		if( nIdx2 - nIdx1 < nLenOn + nLenOff ) continue;	// they would overlap

		DeleteString(nIdx2 - nLenOff, nIdxY, nLenOff);		// back to front again
		DeleteString(nIdx1, nIdxY, nLenOn);
		nReleased++;
	}

	// Only narrow the block if something actually came out of it. A block over rows that were
	// not commented used to shrink anyway, walking its right edge left on every attempt.
	if( nReleased ) {
		INT nShrunk = ( GetStringColumns(szOn) + GetStringColumns(szOff) ) * nSpaceWidth;
		nEndX = _MY_MAX(nBegX, nEndX - nShrunk);
	}

	SetCaretPosY( nBegY ); m_nAnchorPosY = nEndY;
	SetCaretPosX( nBegX ); m_nAnchorPosX = nEndX;
}

void CCedtView::ActionMakeCommentLineSelection()
{
	INT nBegX, nBegY, nEndX, nEndY;
	GetSelectedIndex(nBegX, nBegY, nEndX, nEndY);

	ArrangeLineSelection(nBegX, nBegY, nEndX, nEndY);
	MakeCommentLineSelection(nBegX, nBegY, nEndX, nEndY);

	SetCaretPosY( GetPosYFromIdxY( nBegX, nBegY ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nBegX ) );

	m_nAnchorPosY = GetPosYFromIdxY( nEndX, nEndY );
	CFormatedString & rLne3 = GetLineFromPosY( m_nAnchorPosY );
	m_nAnchorPosX = GetPosXFromIdxX( rLne3, nEndX );
}

void CCedtView::ActionReleaseCommentLineSelection()
{
	INT nBegX, nBegY, nEndX, nEndY;
	GetSelectedIndex(nBegX, nBegY, nEndX, nEndY);

	ArrangeLineSelection(nBegX, nBegY, nEndX, nEndY);
	ReleaseCommentLineSelection(nBegX, nBegY, nEndX, nEndY);

	SetCaretPosY( GetPosYFromIdxY( nBegX, nBegY ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nBegX ) );

	m_nAnchorPosY = GetPosYFromIdxY( nEndX, nEndY );
	CFormatedString & rLne3 = GetLineFromPosY( m_nAnchorPosY );
	m_nAnchorPosX = GetPosXFromIdxX( rLne3, nEndX );
}

void CCedtView::ActionConvertTabsToSpaces()
{
	INT nIdxX, nIdxY; PositionToIndex( m_nCaretPosX, m_nCaretPosY, nIdxX, nIdxY );
	INT nAncX, nAncY; PositionToIndex( m_nAnchorPosX, m_nAnchorPosY, nAncX, nAncY );

	ConvertTabsToSpacesDocument();

	IndexToPosition( nIdxX, nIdxY, m_nCaretPosX, m_nCaretPosY );
	IndexToPosition( nAncX, nAncY, m_nAnchorPosX, m_nAnchorPosY );
}

void CCedtView::ActionConvertSpacesToTabs()
{
	INT nIdxX, nIdxY; PositionToIndex( m_nCaretPosX, m_nCaretPosY, nIdxX, nIdxY );
	INT nAncX, nAncY; PositionToIndex( m_nAnchorPosX, m_nAnchorPosY, nAncX, nAncY );

	ConvertSpacesToTabsDocument();

	IndexToPosition( nIdxX, nIdxY, m_nCaretPosX, m_nCaretPosY );
	IndexToPosition( nAncX, nAncY, m_nAnchorPosX, m_nAnchorPosY );
}

void CCedtView::ActionLeadingSpacesToTabs()
{
	INT nIdxX, nIdxY; PositionToIndex( m_nCaretPosX, m_nCaretPosY, nIdxX, nIdxY );
	INT nAncX, nAncY; PositionToIndex( m_nAnchorPosX, m_nAnchorPosY, nAncX, nAncY );

	LeadingSpacesToTabsDocument();

	IndexToPosition( nIdxX, nIdxY, m_nCaretPosX, m_nCaretPosY );
	IndexToPosition( nAncX, nAncY, m_nAnchorPosX, m_nAnchorPosY );
}

void CCedtView::ActionRemoveTrailingSpaces()
{
	INT nIdxX, nIdxY; PositionToIndex( m_nCaretPosX, m_nCaretPosY, nIdxX, nIdxY );
	INT nAncX, nAncY; PositionToIndex( m_nAnchorPosX, m_nAnchorPosY, nAncX, nAncY );

	DeleteTrailingSpacesDocument();

	IndexToPosition( nIdxX, nIdxY, m_nCaretPosX, m_nCaretPosY );
	IndexToPosition( nAncX, nAncY, m_nAnchorPosX, m_nAnchorPosY );
}


////////////////////////////////////////////////
// BASIC EDITING FUNCTIONS
void CCedtView::CopyToLineSelection(CMemText & rBlock, INT nBegX, INT nBegY, INT nEndX, INT nEndY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->CopyToBlock(rBlock, nBegX, nBegY, nEndX, nEndY);
}

void CCedtView::InsertLineSelection(INT nBegX, INT nBegY, INT & nEndX, INT & nEndY, CMemText & rBlock)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->InsertBlock(nBegX, nBegY, nEndX, nEndY, rBlock); pDoc->InsertScreenText(nBegY, nEndY-nBegY);

	pDoc->AnalyzeText(nBegY, nEndY-nBegY+1);
	pDoc->FormatScreenText(nBegY, nEndY-nBegY+1);
}

void CCedtView::DeleteLineSelection(INT nBegX, INT nBegY, INT nEndX, INT nEndY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->DeleteBlock(nBegX, nBegY, nEndX, nEndY); pDoc->RemoveScreenText(nBegY, nEndY-nBegY);

	pDoc->AnalyzeText(nBegY, 1);
	pDoc->FormatScreenText(nBegY, 1);
}

void CCedtView::ArrangeLineSelection(INT & nBegX, INT & nBegY, INT & nEndX, INT & nEndY)
{
	if( nBegX != 0 ) nBegX = 0;
	if( nEndX != 0 ) {
		if( nEndY == GetLastIdxY() ) {
			CAnalyzedString & rLine = GetLineFromIdxY( nEndY );
			nEndX = GetLastIdxX( rLine );
		} else { nEndY = nEndY + 1; nEndX = 0; }
	}
}

void CCedtView::IndentLineSelection(INT nBegX, INT nBegY, INT nEndX, INT nEndY)
{
	BOOL bUseTab = ( (m_nIndentationSize == 0) && (! m_bUseSpacesInPlaceOfTab) );
	INT  nIndentSize = (m_nIndentationSize == 0) ? m_nTabSize : m_nIndentationSize;

	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->IndentBlock(nBegX, nBegY, nEndX, nEndY, bUseTab, nIndentSize);

	pDoc->AnalyzeText(nBegY, nEndY-nBegY+1);
	pDoc->FormatScreenText(nBegY, nEndY-nBegY+1);
}

void CCedtView::UnindentLineSelection(INT nBegX, INT nBegY, INT nEndX, INT nEndY)
{
	BOOL bUseTab = ( (m_nIndentationSize == 0) && (! m_bUseSpacesInPlaceOfTab) );
	INT  nIndentSize = (m_nIndentationSize == 0) ? m_nTabSize : m_nIndentationSize;

	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->UnindentBlock(nBegX, nBegY, nEndX, nEndY, m_nTabSize, nIndentSize);

	pDoc->AnalyzeText(nBegY, nEndY-nBegY+1);
	pDoc->FormatScreenText(nBegY, nEndY-nBegY+1);
}

void CCedtView::MakeCommentLineSelection(INT nBegX, INT nBegY, INT nEndX, INT nEndY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->MakeCommentBlock(nBegX, nBegY, nEndX, nEndY);

	pDoc->AnalyzeText(nBegY, nEndY-nBegY+1);
	pDoc->FormatScreenText(nBegY, nEndY-nBegY+1);
}

void CCedtView::ReleaseCommentLineSelection(INT nBegX, INT nBegY, INT nEndX, INT nEndY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->ReleaseCommentBlock(nBegX, nBegY, nEndX, nEndY);

	pDoc->AnalyzeText(nBegY, nEndY-nBegY+1);
	pDoc->FormatScreenText(nBegY, nEndY-nBegY+1);
}

void CCedtView::CopyToColumnSelection(CMemText & rBlock, INT nBegX, INT nBegY, INT nEndX, INT nEndY)
{
	rBlock.RemoveAll(); INT nLineHeight = GetLineHeight();

	for(INT nPosY = nBegY; nPosY <= nEndY; nPosY += nLineHeight ) {
		rBlock.AddTail(_T("")); CString & rString = rBlock.GetTail();
		CFormatedString & rLine = GetLineFromPosY( nPosY );
		INT nIdxY = GetIdxYFromPosY( nPosY ), nLstX = GetLastPosX( rLine );

		if( nLstX > nEndX ) {
			INT nIdxX1 = GetIdxXFromBlockEdge( rLine, nBegX );
			INT nIdxX2 = GetIdxXFromBlockEdge( rLine, nEndX );
			CopyToString(rString, nIdxX1, nIdxY, nIdxX2 - nIdxX1);
		} else if( nLstX > nBegX ) {
			INT nIdxX1 = GetIdxXFromBlockEdge( rLine, nBegX );
			INT nIdxX2 = GetIdxXFromPosX( rLine, nLstX, TRUE );
			CopyToString( rString, nIdxX1, nIdxY, nIdxX2 - nIdxX1 );
		}
	}
}

void CCedtView::InsertColumnSelection(INT nBegX, INT nBegY, INT & nEndX, INT & nEndY, CMemText & rBlock)
{
	INT nLineHeight = GetLineHeight();
	INT nLastPosY = GetLastPosY();

	// Square the block off by DISPLAY WIDTH, not character count.
	//
	// CMemText::MakeEqualLength pads to an equal number of characters, and GetMaxLength
	// counts characters — which is not the block's width the moment a line holds anything
	// wide. A Hangul syllable is one character and two cells, so a block whose lines mix
	// scripts came out ragged, and nEndX (chars x cell) landed short of where the text
	// actually ends. That is a counting error, not a metrics one: it was wrong even in a
	// perfect dual-width font.
	//
	// CMemText is a general container and has no business knowing about display width, so
	// the padding is done here — the block is a column-mode artefact and its width rule
	// belongs to column mode.
	INT nMaxCells = 0;
	POSITION posPad = rBlock.GetHeadPosition();
	while( posPad ) {
		INT nCells = GetStringColumns( rBlock.GetNext(posPad) );
		if( nCells > nMaxCells ) nMaxCells = nCells;
	}

	posPad = rBlock.GetHeadPosition();
	while( posPad ) {
		CString & rPad = rBlock.GetNext(posPad);
		INT nCells = GetStringColumns( rPad );
		if( nCells < nMaxCells ) rPad += CString(_T(' '), nMaxCells - nCells);
	}

	INT nCount = (INT)rBlock.GetCount();

	nEndX = nBegX + nMaxCells * GetSpaceWidth();
	nEndY = nBegY + (nCount - 1) * nLineHeight;

	POSITION pos = rBlock.GetHeadPosition();

	for( INT nPosY = nBegY; nPosY <= nEndY; nPosY += nLineHeight ) {
		if( nPosY > nLastPosY ) { // append new empty line at the end of the document
			INT nPrevIdxY = GetIdxYFromPosY( nPosY - nLineHeight );
			CFormatedString & rPrevLine = GetLineFromPosY( nPosY - nLineHeight );
			SplitLine( GetLastIdxX(rPrevLine), nPrevIdxY );
		}

		CFormatedString & rLine = GetLineFromPosY( nPosY );
		INT nIdxY = GetIdxYFromPosY( nPosY ), nLstX = GetLastPosX( rLine );

		CString & rString = rBlock.GetNext( pos );
		INT nLength = rString.GetLength();

		if( nLstX < nBegX ) { // append blank spaces
			INT nIdxX = GetIdxXFromPosX( rLine, nLstX, TRUE );
			InsertString( nIdxX, nIdxY, CString( _T(' '), (nBegX - nLstX) / GetSpaceWidth() ) );
		}

		// now insert text block
		INT nIdxX = GetIdxXFromBlockEdge( rLine, nBegX );
		InsertString( nIdxX, nIdxY, rString );
	}
}

void CCedtView::DeleteColumnSelection(INT nBegX, INT nBegY, INT nEndX, INT nEndY)
{
	INT nLineHeight = GetLineHeight();

	for(INT nPosY = nBegY; nPosY <= nEndY; nPosY += nLineHeight ) {
		CFormatedString & rLine = GetLineFromPosY( nPosY );
		INT nIdxY = GetIdxYFromPosY( nPosY ), nLstX = GetLastPosX( rLine );

		if( nLstX > nEndX ) {
			INT nIdxX1 = GetIdxXFromBlockEdge( rLine, nBegX );
			INT nIdxX2 = GetIdxXFromBlockEdge( rLine, nEndX );
			DeleteString(nIdxX1, nIdxY, nIdxX2 - nIdxX1);
		} else if( nLstX > nBegX ) {
			INT nIdxX1 = GetIdxXFromBlockEdge( rLine, nBegX );
			INT nIdxX2 = GetIdxXFromPosX( rLine, nLstX, TRUE );
			DeleteString(nIdxX1, nIdxY, nIdxX2 - nIdxX1);
		}
	}
}

void CCedtView::ConvertTabsToSpacesDocument()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->ConvertTabsToSpacesDocument();

	pDoc->AnalyzeText();
	pDoc->FormatScreenText();
}

void CCedtView::ConvertSpacesToTabsDocument()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->ConvertSpacesToTabsDocument();

	pDoc->AnalyzeText();
	pDoc->FormatScreenText();
}

void CCedtView::LeadingSpacesToTabsDocument()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->LeadingSpacesToTabsDocument();

	pDoc->AnalyzeText();
	pDoc->FormatScreenText();
}

void CCedtView::DeleteTrailingSpacesDocument()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->DeleteTrailingSpacesDocument();

	pDoc->AnalyzeText();
	pDoc->FormatScreenText();
}

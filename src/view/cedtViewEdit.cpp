#include "stdafx.h"
#include "cedtHeader.h"


void CCedtView::ActionInsertChar(UINT nChar)
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX, ! m_bColumnMode );

	CAnalyzedString & rString = GetLineFromIdxY( nIdxY );
	INT nLstX = GetLastIdxX( rString );

	if( nIdxX < nLstX && m_bOverwriteMode ) {
		// Overwrite consumes the whole character under the caret. Removing one
		// code unit would leave half of an emoji behind.
		DeleteCharacter(nIdxX, nIdxY);
	} else if( nIdxX > nLstX ) {
		CString szInsert(' ', nIdxX - nLstX );
		InsertString(nLstX, nIdxY, szInsert);
	}

	InsertChar(nIdxX, nIdxY, nChar);
	nIdxX = nIdxX + 1;

	// enhanced auto indent feature, 2004.12.06
	if( m_bEnableAutoIndent && ! m_bOverwriteMode ) {
		CAnalyzedString & rString = GetLineFromIdxY( nIdxY );
		INT nFstX = GetFirstNonBlankIdxX(rString);
		BOOL bAutoIndent = (nIdxX-1 == nFstX) && IsThisIndentOffChar(nFstX, nIdxY);

		BOOL bBeginning; INT nTmpX = nFstX, nTmpY = nIdxY;
		if( bAutoIndent ) bAutoIndent = IsThisOneOfPairs(nTmpX, nTmpY, bBeginning);
		if( bAutoIndent ) bAutoIndent = FindAnotherOneOfPairs(nTmpX, nTmpY);

		if( bAutoIndent ) {
			CAnalyzedString & rString = GetLineFromIdxY(nTmpY);
			CString szLeadingSpace; nTmpX = GetFirstNonBlankIdxX(rString);
			if( nTmpX ) CopyToString(szLeadingSpace, 0, nTmpY, nTmpX);

			DeleteLeadingSpaces(nIdxY); nIdxX = 1;
			INT nLength = szLeadingSpace.GetLength(); // insert previous leading spaces
			if( nLength ) { InsertString(0, nIdxY, szLeadingSpace); nIdxX += nLength; }
		}
	}

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY, ! m_bColumnMode ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX, ! m_bColumnMode ) );
}

void CCedtView::ActionInsertSpacesInPlaceOfTab()
{
	INT nSpaceWidth = GetSpaceWidth();
	INT nNextPosX = GetNextTabPosition(m_nCaretPosX);
	INT nSpaceCount = (nNextPosX - m_nCaretPosX) / nSpaceWidth;

	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX, ! m_bColumnMode );

	CAnalyzedString & rString = GetLineFromIdxY( nIdxY );
	INT nLstX = GetLastIdxX( rString );

	if( nIdxX < nLstX && m_bOverwriteMode ) {
		// Overwrite consumes the whole character — see ActionInsertChar.
		DeleteCharacter(nIdxX, nIdxY);
	} else if( nIdxX > nLstX ) {
		CString szInsert(' ', nIdxX - nLstX );
		InsertString(nLstX, nIdxY, szInsert);
	}

	if( nSpaceCount == 1 ) InsertChar(nIdxX, nIdxY, ' ');
	else InsertString(nIdxX, nIdxY, CString(' ', nSpaceCount));

	nIdxX = nIdxX + nSpaceCount;

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY, ! m_bColumnMode ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX, ! m_bColumnMode ) );
}

void CCedtView::ActionInsertColumnChar(UINT nChar)
{
	TCHAR szChar[2] = { (TCHAR)nChar, 0 };
	ActionInsertColumnString( szChar );
}

// Insert into every row of the block.
//
// A keystroke and an IME result go through the same loop on purpose: they used to be
// different code paths, and that is exactly why typing Hangul into a multi-row block only
// ever reached one row — the character path grew the multi-row loop and the composition path
// never did.
//
// The string, rather than a character, is the honest unit. A Hangul syllable is one TCHAR now
// that the editor is Unicode, so the character form would very nearly do; but "very nearly"
// is not a contract. IME delivers a STRING (WM_IME_COMPOSITION / GCS_RESULTSTR), and it can
// hold more than one character — a Hanja conversion, or an IME committing several syllables
// at once — while an astral character is two code units. Taking only the first would drop the
// rest silently.
//
// (InsertChar vs InsertString differ only in recording undo as AT_INSERTCHAR against
// AT_INSERTSTRING with a length of one. Undoing either removes the same text, so there is no
// reason to spell this loop twice.)
void CCedtView::ActionInsertColumnString(LPCTSTR lpszString)
{
	INT nLineHeight = GetLineHeight(), nSpaceWidth = GetSpaceWidth();
	INT nSize = (INT)_tcslen( lpszString ); if( nSize <= 0 ) return;

	INT nBegX, nBegY, nEndX, nEndY; GetSelectedPosition(nBegX, nBegY, nEndX, nEndY);

	for(INT nPosY = nBegY; nPosY <= nEndY; nPosY += nLineHeight ) {
		CFormatedString & rLine = GetLineFromPosY( nPosY );
		INT nIdxX, nIdxY = GetIdxYFromPosY( nPosY ), nLstX = GetLastPosX( rLine );

		if( nBegX > nLstX ) {	// pad the virtual space with spaces — a cell IS a space
			nIdxX = GetIdxXFromPosX( rLine, nLstX, TRUE );
			CString szInsert(_T(' '), (nBegX - nLstX) / nSpaceWidth);
			InsertString(nIdxX, nIdxY, szInsert);
		}

		nIdxX = GetIdxXFromBlockEdge( rLine, nBegX );
		InsertString(nIdxX, nIdxY, lpszString);
	}

	INT nIdxX, nIdxY; PositionToIndex( nBegX, nBegY, nIdxX, nIdxY );
	nIdxX = nIdxX + nSize; IndexToPosition( nIdxX, nIdxY, nBegX, nBegY );

	SetCaretPosY( nBegY ); m_nAnchorPosY = nEndY;
	SetCaretPosX( nBegX ); m_nAnchorPosX = nBegX;
}

void CCedtView::ActionInsertColumnSpacesInPlaceOfTab()
{
	INT nSpaceWidth = GetSpaceWidth();
	INT nNextPosX = GetNextTabPosition(m_nCaretPosX);
	INT nSpaceCount = (nNextPosX - m_nCaretPosX) / nSpaceWidth;

	INT nLineHeight = GetLineHeight();
	INT nBegX, nBegY, nEndX, nEndY; GetSelectedPosition(nBegX, nBegY, nEndX, nEndY);

	for(INT nPosY = nBegY; nPosY <= nEndY; nPosY += nLineHeight ) {
		CFormatedString & rLine = GetLineFromPosY( nPosY );
		INT nIdxX, nIdxY = GetIdxYFromPosY( nPosY ), nLstX = GetLastPosX( rLine );

		if( nBegX > nLstX ) {	// pad the virtual space with spaces — a cell IS a space
			nIdxX = GetIdxXFromPosX( rLine, nLstX, TRUE );
			CString szInsert(_T(' '), (nBegX - nLstX) / nSpaceWidth);
			InsertString(nIdxX, nIdxY, szInsert);
		}

		nIdxX = GetIdxXFromBlockEdge( rLine, nBegX );
		if( nSpaceCount == 1 ) InsertChar(nIdxX, nIdxY, ' ');
		else InsertString(nIdxX, nIdxY, CString(' ', nSpaceCount));
	}

	INT nIdxX, nIdxY; PositionToIndex( nBegX, nBegY, nIdxX, nIdxY );
	nIdxX = nIdxX + nSpaceCount; IndexToPosition( nIdxX, nIdxY, nBegX, nBegY );

	SetCaretPosY( nBegY ); m_nAnchorPosY = nEndY;
	SetCaretPosX( nBegX ); m_nAnchorPosX = nBegX;
}

void CCedtView::ActionInsertString(LPCTSTR lpszString)
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX, ! m_bColumnMode );

	INT nSize = (INT)_tcslen( lpszString );

	CAnalyzedString & rString = GetLineFromIdxY( nIdxY );
	INT nLstX = GetLastIdxX( rString );

	if( nSize > 0 && nIdxX < nLstX && m_bOverwriteMode ) {
		// Overwrite eats as many code units as we are about to insert — but the
		// cut has to land on a character boundary, otherwise the trailing half
		// of a surrogate pair survives as a lone surrogate.
		INT nSpan = SpanToCharBoundary((LPCTSTR)rString, nIdxX, nSize, nLstX);

		if( nLstX - nIdxX > nSpan ) {
			DeleteString(nIdxX, nIdxY, nSpan);
			InsertString(nIdxX, nIdxY, lpszString);
		} else {
			DeleteString(nIdxX, nIdxY, nLstX - nIdxX);
			InsertString(nIdxX, nIdxY, lpszString);
		}
	} else if( nSize > 0 && nIdxX > nLstX ) {
		CString szInsert(' ', nIdxX - nLstX);
		InsertString(nLstX, nIdxY, szInsert);
		InsertString(nIdxX, nIdxY, lpszString);
	} else if( nSize > 0 ) {
		InsertString(nIdxX, nIdxY, lpszString);
	}

	nIdxX = nIdxX + nSize;

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY, ! m_bColumnMode ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX, ! m_bColumnMode ) );
}


void CCedtView::ActionCarrigeReturn()
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX, TRUE );

	if( m_bOverwriteMode && nIdxY < GetLastIdxY() ) {
		// overwrite mode && not in last line => do not split line
	} else SplitLine(nIdxX, nIdxY);

	nIdxY = nIdxY + 1; nIdxX = 0;

	// enhanced auto indent feature, 2004.12.06
	if( m_bEnableAutoIndent && ! m_bOverwriteMode ) {
		CAnalyzedString & rString = GetLineFromIdxY( nIdxY );
		INT nFstX = GetFirstNonBlankIdxX(rString);
		BOOL bAutoIndent0 = IsThisIndentOffChar(nFstX, nIdxY);

		BOOL bBeginning; INT nTmpX = nFstX, nTmpY = nIdxY;
		if( bAutoIndent0 ) bAutoIndent0 = IsThisOneOfPairs(nTmpX, nTmpY, bBeginning);
		if( bAutoIndent0 ) bAutoIndent0 = FindAnotherOneOfPairs(nTmpX, nTmpY);

		if( bAutoIndent0 ) {
			CAnalyzedString & rString = GetLineFromIdxY(nTmpY);
			CString szLeadingSpace; INT nTmpX = GetFirstNonBlankIdxX(rString);
			if( nTmpX ) CopyToString(szLeadingSpace, 0, nTmpY, nTmpX);

			DeleteLeadingSpaces(nIdxY); nIdxX = 0;
			INT nLength = szLeadingSpace.GetLength(); // insert previous leading spaces
			if( nLength ) { InsertString(0, nIdxY, szLeadingSpace); nIdxX += nLength; }

		} else { // insert previous leading spaces
			for(INT nTmpY = nIdxY-1; nTmpY >= 0; nTmpY--) { if( ! IsBlankLineFromIdxY( nTmpY ) ) break; }
			BOOL bAutoIndent1 = ( nTmpY >= 0 ); // find previous non blank line

			if( bAutoIndent1 ) { // insert previous leading spaces
				CAnalyzedString & rString = GetLineFromIdxY(nTmpY); 
				CString szLeadingSpace; INT nTmpX = GetFirstNonBlankIdxX(rString);
				if( nTmpX ) CopyToString(szLeadingSpace, 0, nTmpY, nTmpX);

				DeleteLeadingSpaces(nIdxY); nIdxX = 0;
				INT nLength = szLeadingSpace.GetLength(); // insert previous leading spaces
				if( nLength ) { InsertString(0, nIdxY, szLeadingSpace); nIdxX += nLength; }
			}

			if( bAutoIndent1 ) { // append extra indentation
				CAnalyzedString & rString = GetLineFromIdxY(nTmpY);
				INT nLstX = GetTrailingBlankIdxX(rString);
				BOOL bAutoIndent2 = (nLstX > 0) && IsThisIndentOnChar(nLstX-1, nTmpY);

				BOOL bBeginning; INT nTmpX = nLstX-1;
				if( bAutoIndent2 ) bAutoIndent2 = IsThisOneOfPairs(nTmpX, nTmpY, bBeginning);
				if( bAutoIndent2 ) nIdxX += IndentLine(nIdxY);
			}
		}

	} else if( m_bEnableAutoIndent && m_bOverwriteMode ) { // go to first nonblank position
		CAnalyzedString & rString = GetLineFromIdxY(nIdxY);
		nIdxX = GetFirstNonBlankIdxX(rString);
	}

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY, ! m_bColumnMode ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX, ! m_bColumnMode ) );
}

void CCedtView::ActionBackspace()
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX, ! m_bColumnMode );

	CAnalyzedString & rString = GetLineFromIdxY( nIdxY );
	INT nLstX = GetLastIdxX( rString );

	if( nIdxX > nLstX ) {
		// move caret to the left (virtual space past end of line — no text here)
		nIdxX = nIdxX - 1;
	} else if( nIdxX > 0 ) {
		// Step back one CHARACTER and delete it whole. A bare nIdxX-1 would
		// remove only the low half of a surrogate pair, stranding the high half
		// as a lone surrogate that UTF-8 save then destroys.
		nIdxX = PrevIdxX((LPCTSTR)rString, nIdxX); DeleteCharacter(nIdxX, nIdxY);
	} else if( nIdxY > 0 ) {
		CAnalyzedString & rStrn2 = GetLineFromIdxY( nIdxY-1 );
		nIdxY = nIdxY - 1; nIdxX = GetLastIdxX( rStrn2 );
		JoinLines(nIdxX, nIdxY);
	}

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY, ! m_bColumnMode ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX, ! m_bColumnMode ) );
}

void CCedtView::ActionDeleteChar()
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX, ! m_bColumnMode );

	CAnalyzedString & rString = GetLineFromIdxY( nIdxY );
	INT nLstX = GetLastIdxX( rString );

	if( nIdxX < nLstX ) {
		// Delete the whole character at the caret, pair included.
		DeleteCharacter(nIdxX, nIdxY);
	} else if( nIdxY < GetLastIdxY() ) {
		if( nIdxX > nLstX ) {
			CString szInsert(' ', nIdxX - nLstX);
			InsertString(nLstX, nIdxY, szInsert);
		}
		JoinLines(nIdxX, nIdxY);
	}

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY, ! m_bColumnMode ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX, ! m_bColumnMode ) );
}

void CCedtView::ActionDetabCaret()
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX, ! m_bColumnMode );

	INT nPosX = GetPrevTabPosition( m_nCaretPosX );
	if( nPosX < GetFirstPosX( rLine ) ) nPosX = GetFirstPosX(rLine);
	nIdxX = GetIdxXFromPosX( rLine, nPosX, ! m_bColumnMode );

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY, ! m_bColumnMode ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX, ! m_bColumnMode ) );
}

void CCedtView::ActionJoinLines()
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX, TRUE );

	if( nIdxY < GetLastIdxY() ) {
		DeleteTrailingSpaces( nIdxY );
		DeleteLeadingSpaces( nIdxY + 1 );

		CAnalyzedString & rString = GetLineFromIdxY( nIdxY );
		nIdxX = GetLastIdxX( rString );

		JoinLines(nIdxX, nIdxY);
		InsertChar(nIdxX, nIdxY, ' ');
	}

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY, ! m_bColumnMode ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX, ! m_bColumnMode ) );
}

void CCedtView::ActionSplitLine()
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX, ! m_bColumnMode );

	CAnalyzedString & rString = GetLineFromIdxY( nIdxY );
	INT nLstX = GetLastIdxX( rString );

	if( nIdxX > nLstX )
		SplitLine(nLstX, nIdxY);
	else
		SplitLine(nIdxX, nIdxY);

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY, ! m_bColumnMode ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX, ! m_bColumnMode ) );
}

void CCedtView::ActionDeleteWord()
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX, ! m_bColumnMode );

	CAnalyzedString & rString = GetLineFromIdxY( nIdxY );
	INT nLstX = GetLastIdxX( rString );

	if( nIdxX < nLstX ) {
		INT nEndX = GetNextWordIdxX(rLine, nIdxX);
		DeleteString(nIdxX, nIdxY, nEndX-nIdxX);
	} else if( nIdxY < GetLastIdxY() ) {
		if( nIdxX > nLstX ) {
			CString szInsert(' ', nIdxX - nLstX);
			InsertString(nLstX, nIdxY, szInsert);
		}
		JoinLines(nIdxX, nIdxY);
	}

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY, ! m_bColumnMode ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX, ! m_bColumnMode ) );
}

void CCedtView::ActionDeletePrevWord()
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX, ! m_bColumnMode );

	CAnalyzedString & rString = GetLineFromIdxY( nIdxY );
	INT nLstX = GetLastIdxX( rString );

	if( nIdxX > nLstX ) {
		nIdxX = GetTrailingBlankIdxX( rString );
		if( nIdxX < nLstX ) DeleteString(nIdxX, nIdxY, nLstX-nIdxX);
	} else if( nIdxX > 0 ) {
		INT nBegX = GetPrevWordIdxX(rLine, nIdxX-1);
		DeleteString(nBegX, nIdxY, nIdxX-nBegX); nIdxX = nBegX;
	} else if( nIdxY > 0 ) {
		CAnalyzedString & rStrn2 = GetLineFromIdxY( nIdxY-1 );
		nIdxY = nIdxY - 1; nIdxX = GetLastIdxX( rStrn2 );
		JoinLines(nIdxX, nIdxY);
	}

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY, ! m_bColumnMode ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX, ! m_bColumnMode ) );
}

void CCedtView::ActionDeleteToEndOfLine()
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX, ! m_bColumnMode );

	CAnalyzedString & rString = GetLineFromIdxY( nIdxY );
	INT nLstX = GetLastIdxX( rString );

	if( nIdxX < nLstX ) {
		DeleteString(nIdxX, nIdxY, nLstX-nIdxX);
	} else if( nIdxY < GetLastIdxY() ) {
		if( nIdxX > nLstX ) {
			CString szInsert(' ', nIdxX - nLstX);
			InsertString(nLstX, nIdxY, szInsert);
		}
		JoinLines(nIdxX, nIdxY);
	}

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY, ! m_bColumnMode ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX, ! m_bColumnMode ) );
}

void CCedtView::ActionDeleteToBeginOfLine()
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX, ! m_bColumnMode );

	CAnalyzedString & rString = GetLineFromIdxY( nIdxY );
	INT nLstX = GetLastIdxX( rString );

	if( nIdxX > nLstX ) {
		if( nLstX > 0 ) DeleteString(0, nIdxY, nLstX); 
		nIdxX = 0;
	} else if( nIdxX > 0 ) {
		DeleteString(0, nIdxY, nIdxX); 
		nIdxX = 0;
	} else if( nIdxY > 0 ) {
		CAnalyzedString & rStrn2 = GetLineFromIdxY( nIdxY-1 );
		nIdxY = nIdxY - 1; nIdxX = GetLastIdxX( rStrn2 );
		JoinLines(nIdxX, nIdxY);
	}

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY, ! m_bColumnMode ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX, ! m_bColumnMode ) );
}

void CCedtView::ActionDeleteLine()
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX, ! m_bColumnMode );

	CAnalyzedString & rString = GetLineFromIdxY( nIdxY );
	INT nLstX = GetLastIdxX( rString );

	if( nIdxY < GetLastIdxY() )
		DeleteLineSelection(0, nIdxY, 0, nIdxY+1);
	else
		DeleteString(0, nIdxY, nLstX);

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY, ! m_bColumnMode ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX, ! m_bColumnMode ) );
}

void CCedtView::ActionDuplicateLine()
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX, ! m_bColumnMode );

	CAnalyzedString & rString = GetLineFromIdxY( nIdxY );
	INT nLstX = GetLastIdxX( rString );

	SplitLine(nLstX, nIdxY);
	InsertString(0, nIdxY+1, rString);
	nIdxY = nIdxY + 1;

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY, ! m_bColumnMode ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX, ! m_bColumnMode ) );
}

void CCedtView::ActionIndentLine()
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX, ! m_bColumnMode );

	nIdxX = nIdxX + IndentLine(nIdxY);

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY, ! m_bColumnMode ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX, ! m_bColumnMode ) );
}

void CCedtView::ActionUnindentLine()
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX, ! m_bColumnMode );

	nIdxX = nIdxX - UnindentLine(nIdxY);

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY, ! m_bColumnMode ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX, ! m_bColumnMode ) );
}

void CCedtView::ActionMakeCommentLine()
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX, ! m_bColumnMode );

	nIdxX = nIdxX + MakeCommentLine(nIdxY);

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY, ! m_bColumnMode ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX, ! m_bColumnMode ) );
}

void CCedtView::ActionReleaseCommentLine()
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX, ! m_bColumnMode );

	nIdxX = nIdxX - ReleaseCommentLine(nIdxY);

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY, ! m_bColumnMode ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX, ! m_bColumnMode ) );
}

////////////////////////////////////////////////
// BASIC EDITING FUNCTIONS
void CCedtView::InsertChar(INT nIdxX, INT nIdxY, UINT nChar)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->InsertChar(nIdxX, nIdxY, nChar);

	pDoc->AnalyzeText(nIdxY, 1);
	pDoc->FormatScreenText(nIdxY, 1);
}

void CCedtView::DeleteChar(INT nIdxX, INT nIdxY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->DeleteChar(nIdxX, nIdxY);

	pDoc->AnalyzeText(nIdxY, 1);
	pDoc->FormatScreenText(nIdxY, 1);
}

void CCedtView::DeleteCharacter(INT nIdxX, INT nIdxY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->DeleteCharacter(nIdxX, nIdxY);

	pDoc->AnalyzeText(nIdxY, 1);
	pDoc->FormatScreenText(nIdxY, 1);
}

void CCedtView::CopyToString(CString & rString, INT nIdxX, INT nIdxY, INT nLength)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->CopyToString(rString, nIdxX, nIdxY, nLength);
}

void CCedtView::InsertString(INT nIdxX, INT nIdxY, LPCTSTR lpszString)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->InsertString(nIdxX, nIdxY, lpszString);

	pDoc->AnalyzeText(nIdxY, 1);
	pDoc->FormatScreenText(nIdxY, 1);
}

void CCedtView::DeleteString(INT nIdxX, INT nIdxY, INT nLength)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->DeleteString(nIdxX, nIdxY, nLength);

	pDoc->AnalyzeText(nIdxY, 1);
	pDoc->FormatScreenText(nIdxY, 1);
}

void CCedtView::SplitLine(INT nIdxX, INT nIdxY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->SplitLine(nIdxX, nIdxY); pDoc->InsertScreenText(nIdxY, 1);

	pDoc->AnalyzeText(nIdxY, 2);
	pDoc->FormatScreenText(nIdxY, 2);
}

void CCedtView::JoinLines(INT nIdxX, INT nIdxY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->JoinLines(nIdxX, nIdxY); pDoc->RemoveScreenText(nIdxY+1, 1);

	pDoc->AnalyzeText(nIdxY, 1);
	pDoc->FormatScreenText(nIdxY, 1);
}

INT CCedtView::IndentLine(INT nIdxY)
{
	BOOL bUseTab = ( (m_nIndentationSize == 0) && (! m_bUseSpacesInPlaceOfTab) );
	INT  nIndentSize = (m_nIndentationSize == 0) ? m_nTabSize : m_nIndentationSize;

	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	INT nLen = pDoc->IndentLine(nIdxY, bUseTab, nIndentSize);

	pDoc->AnalyzeText(nIdxY, 1);
	pDoc->FormatScreenText(nIdxY, 1); 

	return nLen;
}

INT CCedtView::UnindentLine(INT nIdxY)
{
	BOOL bUseTab = ( (m_nIndentationSize == 0) && (! m_bUseSpacesInPlaceOfTab) );
	INT  nIndentSize = (m_nIndentationSize == 0) ? m_nTabSize : m_nIndentationSize;

	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	INT nLen = pDoc->UnindentLine(nIdxY, m_nTabSize, nIndentSize);

	pDoc->AnalyzeText(nIdxY, 1);
	pDoc->FormatScreenText(nIdxY, 1);
	
	return nLen;
}

INT CCedtView::MakeCommentLine(INT nIdxY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	INT nLen = pDoc->MakeCommentLine(nIdxY); if( ! nLen ) return 0;

	pDoc->AnalyzeText(nIdxY, 1);
	pDoc->FormatScreenText(nIdxY, 1);
	
	return nLen;
}

INT CCedtView::ReleaseCommentLine(INT nIdxY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	INT nLen = pDoc->ReleaseCommentLine(nIdxY); if( ! nLen ) return 0;

	pDoc->AnalyzeText(nIdxY, 1);
	pDoc->FormatScreenText(nIdxY, 1);
	
	return nLen;
}

INT CCedtView::ConvertTabsToSpaces(INT nIdxY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	INT nTab = pDoc->ConvertTabsToSpaces(nIdxY); if( ! nTab ) return 0;

	pDoc->AnalyzeText(nIdxY, 1);
	pDoc->FormatScreenText(nIdxY, 1); 
	
	return nTab;
}

INT CCedtView::ConvertSpacesToTabs(INT nIdxY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	INT nTab = pDoc->ConvertSpacesToTabs(nIdxY); if( ! nTab ) return 0;

	pDoc->AnalyzeText(nIdxY, 1);
	pDoc->FormatScreenText(nIdxY, 1);
	
	return nTab;
}

INT CCedtView::LeadingSpacesToTabs(INT nIdxY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	INT nTab = pDoc->LeadingSpacesToTabs(nIdxY); if( ! nTab ) return 0;

	pDoc->AnalyzeText(nIdxY, 1);
	pDoc->FormatScreenText(nIdxY, 1);
	
	return nTab;
}

INT CCedtView::DeleteLeadingSpaces(INT nIdxY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	INT nLen = pDoc->DeleteLeadingSpaces(nIdxY); if( ! nLen ) return 0;

	pDoc->AnalyzeText(nIdxY, 1);
	pDoc->FormatScreenText(nIdxY, 1);
	
	return nLen;
}

INT CCedtView::DeleteTrailingSpaces(INT nIdxY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	INT nLen = pDoc->DeleteTrailingSpaces(nIdxY); if( ! nLen ) return 0;

	pDoc->AnalyzeText(nIdxY, 1);
	pDoc->FormatScreenText(nIdxY, 1);
	
	return nLen;
}




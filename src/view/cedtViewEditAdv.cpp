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
			INT nIdxX = GetIdxXFromPosX( rLine, nBegX, TRUE );
			FORMATEDWORD & rWord = GetWordFromIdxX( rLine, nIdxX );
			if( IS_DBCHAR(rWord) ) {
				DeleteString(nIdxX, nIdxY, 2);
				InsertChar(nIdxX, nIdxY, ' ');
			} else DeleteChar(nIdxX, nIdxY);
		}
	}

	SetCaretPosY( nBegY ); m_nAnchorPosY = nEndY;
	SetCaretPosX( nBegX ); m_nAnchorPosX = nBegX;
}

void CCedtView::ActionDeleteColumnPrevChar()
{
	INT nLineHeight = GetLineHeight(), nAveCharWidth = GetAveCharWidth();
	INT nBegX, nBegY, nEndX, nEndY; GetSelectedPosition(nBegX, nBegY, nEndX, nEndY);

	if( nBegX <= 0 ) return;
	else nBegX = nEndX = nBegX - nAveCharWidth;

	for(INT nPosY = nBegY; nPosY <= nEndY; nPosY += nLineHeight ) {
		CFormatedString & rLine = GetLineFromPosY( nPosY );
		INT nIdxY = GetIdxYFromPosY( nPosY ), nLstX = GetLastPosX( rLine );

		if( nBegX < nLstX ) {
			INT nIdxX = GetIdxXFromPosX( rLine, nBegX, TRUE );
			FORMATEDWORD & rWord = GetWordFromIdxX( rLine, nIdxX );
			if( IS_DBCHAR(rWord) ) {
				DeleteString(nIdxX, nIdxY, 2);
				InsertChar(nIdxX, nIdxY, ' ');
			} else DeleteChar(nIdxX, nIdxY);
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
			INT nIdxX = GetIdxXFromPosX( rLine, nBegX, TRUE );
			FORMATEDWORD & rWord = GetWordFromIdxX( rLine, nIdxX );
			INT nLdxX = GetIdxXFromPosX( rLine, nLstX, TRUE );
			if( IS_DBCHAR(rWord) && rWord.m_nPosition < nBegX ) {
				DeleteString(nIdxX, nIdxY, nLdxX - nIdxX);
				InsertChar(nIdxX, nIdxY, ' ');
			} else DeleteString(nIdxX, nIdxY, nLdxX - nIdxX);
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

		if( nBegX < nLstX ) {
			INT nIdxX = GetIdxXFromPosX( rLine, nBegX, TRUE );
			FORMATEDWORD & rWord = GetWordFromIdxX( rLine, nIdxX );
			if( IS_DBCHAR(rWord) && rWord.m_nPosition < nBegX ) {
				DeleteString(0, nIdxY, nIdxX + 2);
				InsertChar(0, nIdxY, ' ');
			} else DeleteString(0, nIdxY, nIdxX);
		} else {
			INT nIdxX = GetIdxXFromPosX( rLine, nLstX, TRUE );
			DeleteString(0, nIdxY, nIdxX);
		}
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
	else { rBlock.AddTail(""); CopyToString(rBlock.GetTail(), nBegX, nBegY, nEndX-nBegX); }
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
	rBlock.AddTail( "" );
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
		nEndX = nBegX + strlen(lpszString);
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
		rBlock.AddTail(""); CString & rString = rBlock.GetTail();
		CFormatedString & rLine = GetLineFromPosY( nPosY );
		INT nIdxY = GetIdxYFromPosY( nPosY ), nLstX = GetLastPosX( rLine );

		if( nLstX > nEndX ) {
			INT nIdxX1 = GetIdxXFromPosX( rLine, nBegX, TRUE );
			INT nIdxX2 = GetIdxXFromPosX( rLine, nEndX, TRUE );

			FORMATEDWORD & rWord1 = GetWordFromIdxX( rLine, nIdxX1 );
			FORMATEDWORD & rWord2 = GetWordFromIdxX( rLine, nIdxX2 );

			BOOL bHalf1 = FALSE, bHalf2 = FALSE;
			if( IS_DBCHAR(rWord1) && rWord1.m_nPosition < nBegX ) bHalf1 = TRUE;
			if( IS_DBCHAR(rWord2) && rWord2.m_nPosition < nEndX ) bHalf2 = TRUE;

			if( bHalf1 && bHalf2 ) {
				CopyToString(rString, nIdxX1 + 2, nIdxY, nIdxX2 - nIdxX1 - 2);
				rString = CString(" ") + rString + CString(" ");
			} else if( bHalf1 ) {
				CopyToString(rString, nIdxX1 + 2, nIdxY, nIdxX2 - nIdxX1 - 2);
				rString = CString(" ") + rString;
			} else if( bHalf2 ) {
				CopyToString(rString, nIdxX1, nIdxY, nIdxX2 - nIdxX1);
				rString = rString + CString(" ");
			} else {
				CopyToString(rString, nIdxX1, nIdxY, nIdxX2 - nIdxX1);
			}
		} else if( nLstX > nBegX ) {
			INT nIdxX1 = GetIdxXFromPosX( rLine, nBegX, TRUE );
			INT nIdxX2 = GetIdxXFromPosX( rLine, nLstX, TRUE );

			FORMATEDWORD & rWord = GetWordFromIdxX( rLine, nIdxX1 );
			BOOL bHalf = ( IS_DBCHAR(rWord) && rWord.m_nPosition < nBegX );

			if( bHalf ) {
				CopyToString( rString, nIdxX1 + 2, nIdxY, nIdxX2 - nIdxX1 - 2 );
				rString = CString(" ") + rString;
			} else {
				CopyToString( rString, nIdxX1, nIdxY, nIdxX2 - nIdxX1 );
			}
		}
	}
}

void CCedtView::InsertColumnSelection(INT nBegX, INT nBegY, INT & nEndX, INT & nEndY, CMemText & rBlock)
{
	INT nLineHeight = GetLineHeight(), nAveCharWidth = GetAveCharWidth();
	INT nLastPosY = GetLastPosY();

	rBlock.MakeEqualLength();
	INT nCount = rBlock.GetCount(), nMaxLen = rBlock.GetMaxLength();

	nEndX = nBegX + nMaxLen * nAveCharWidth;
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
			InsertString( nIdxX, nIdxY, CString( ' ', (nBegX - nLstX) / nAveCharWidth ) );
		} else { // check if double byte character need to be splitted
			INT nIdxX = GetIdxXFromPosX( rLine, nBegX, TRUE );
			FORMATEDWORD & rWord = GetWordFromIdxX( rLine, nIdxX );
			if ( IS_DBCHAR(rWord) && rWord.m_nPosition < nBegX ) {
				DeleteString( nIdxX, nIdxY, 2 );
				InsertString( nIdxX, nIdxY, "  " );
			}
		}

		// now insert text block
		INT nIdxX = GetIdxXFromPosX( rLine, nBegX, TRUE );
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
			INT nIdxX1 = GetIdxXFromPosX( rLine, nBegX, TRUE );
			INT nIdxX2 = GetIdxXFromPosX( rLine, nEndX, TRUE );

			FORMATEDWORD & rWord1 = GetWordFromIdxX( rLine, nIdxX1 );
			FORMATEDWORD & rWord2 = GetWordFromIdxX( rLine, nIdxX2 );

			BOOL bHalf1 = FALSE, bHalf2 = FALSE;
			if( IS_DBCHAR(rWord1) && rWord1.m_nPosition < nBegX ) bHalf1 = TRUE;
			if( IS_DBCHAR(rWord2) && rWord2.m_nPosition < nEndX ) bHalf2 = TRUE;

			if( bHalf1 && bHalf2 ) {
				DeleteString(nIdxX1, nIdxY, nIdxX2 - nIdxX1 + 2);
				InsertString(nIdxX1, nIdxY, "  ");
			} else if( bHalf1 ) {
				DeleteString(nIdxX1, nIdxY, nIdxX2 - nIdxX1);
				InsertChar(nIdxX1, nIdxY, ' ');
			} else if( bHalf2 ) {
				DeleteString(nIdxX1, nIdxY, nIdxX2 - nIdxX1 + 2);
				InsertChar(nIdxX1, nIdxY, ' ');
			} else {
				DeleteString(nIdxX1, nIdxY, nIdxX2 - nIdxX1);
			}
		} else if( nLstX > nBegX ) {
			INT nIdxX1 = GetIdxXFromPosX( rLine, nBegX, TRUE );
			INT nIdxX2 = GetIdxXFromPosX( rLine, nLstX, TRUE );

			FORMATEDWORD & rWord = GetWordFromIdxX( rLine, nIdxX1 );
			BOOL bHalf = ( IS_DBCHAR(rWord) && rWord.m_nPosition < nBegX );

			if( bHalf ) {
				DeleteString(nIdxX1, nIdxY, nIdxX2 - nIdxX1);
				InsertChar(nIdxX1, nIdxY, ' ');
			} else {
				DeleteString(nIdxX1, nIdxY, nIdxX2 - nIdxX1);
			}
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

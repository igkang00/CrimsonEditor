#include "stdafx.h"
#include "cedtHeader.h"


BOOL CCedtView::ActionForwardFindString(LPCTSTR lpszFindString, UINT nOptions, CRegExp & clsRegExp, BOOL * pSearchWrap)
{
	INT nIdxX, nIdxY, nBegX, nBegY;

	if( m_bSelected ) GetSelectedIndex(nBegX, nBegY, nIdxX, nIdxY);
	else PositionToIndex(m_nCaretPosX, m_nCaretPosY, nIdxX, nIdxY);

	BOOL bFound = ForwardFindString(nIdxX, nIdxY, lpszFindString, nOptions, clsRegExp, pSearchWrap);
	INT nLength = SEARCH_REG_EXP(nOptions) ? clsRegExp.GetFoundLength() : strlen(lpszFindString);

	if( bFound ) {
		SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY ) );
		CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
		SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX ) );

		m_nAnchorPosY = GetPosYFromIdxY( nIdxX + nLength, nIdxY );
		CFormatedString & rLne3 = GetLineFromPosY( m_nAnchorPosY );
		m_nAnchorPosX = GetPosXFromIdxX( rLne3, nIdxX + nLength );
	}

	return bFound;
}

BOOL CCedtView::ActionReverseFindString(LPCTSTR lpszFindString, UINT nOptions, CRegExp & clsRegExp, BOOL * pSearchWrap)
{
	INT nIdxX, nIdxY, nEndX, nEndY;

	if( m_bSelected ) GetSelectedIndex(nIdxX, nIdxY, nEndX, nEndY);
	else PositionToIndex(m_nCaretPosX, m_nCaretPosY, nIdxX, nIdxY);

	BOOL bFound = ReverseFindString(nIdxX, nIdxY, lpszFindString, nOptions, clsRegExp, pSearchWrap);
	INT nLength = SEARCH_REG_EXP(nOptions) ? clsRegExp.GetFoundLength() : strlen(lpszFindString);

	if( bFound ) {
		SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY ) );
		CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
		SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX ) );

		m_nAnchorPosY = GetPosYFromIdxY( nIdxX + nLength, nIdxY );
		CFormatedString & rLne3 = GetLineFromPosY( m_nAnchorPosY );
		m_nAnchorPosX = GetPosXFromIdxX( rLne3, nIdxX + nLength );
	}

	return bFound;
}

INT CCedtView::ActionReplaceAllInSelection(LPCTSTR lpszFindString, LPCTSTR lpszReplaceString, UINT nOptions, CRegExp & clsRegExp)
{
	INT nBegX, nBegY, nEndX, nEndY;
	GetSelectedIndex( nBegX, nBegY, nEndX, nEndY );

	INT nReplaced = ReplaceAllInSelection(nBegX, nBegY, nEndX, nEndY, lpszFindString, lpszReplaceString, nOptions, clsRegExp);

	if( nReplaced ) {
		SetCaretPosY( GetPosYFromIdxY( nBegX, nBegY ) );
		CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
		SetCaretPosX( GetPosXFromIdxX( rLne2, nBegX ) );

		m_nAnchorPosY = GetPosYFromIdxY( nEndX, nEndY );
		CFormatedString & rLne3 = GetLineFromPosY( m_nAnchorPosY );
		m_nAnchorPosX = GetPosXFromIdxX( rLne3, nEndX );
	}

	return nReplaced;
}

INT CCedtView::ActionReplaceAllInFile(LPCTSTR lpszFindString, LPCTSTR lpszReplaceString, UINT nOptions, CRegExp & clsRegExp)
{
	INT nIdxX, nIdxY;
	PositionToIndex(m_nCaretPosX, m_nCaretPosY, nIdxX, nIdxY);

	INT nReplaced = ReplaceAllInFile(lpszFindString, lpszReplaceString, nOptions, clsRegExp);

	if( nReplaced ) {
		SetCaretPosY( GetPosYFromIdxY( 0, nIdxY ) );
		CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
		SetCaretPosX( GetPosXFromIdxX( rLne2, 0 ) );
	}

	return nReplaced;
}

INT CCedtView::ActionReplaceThisOccurrence(LPCTSTR lpszReplaceString, UINT nOptions, CRegExp & clsRegExp)
{
	INT nBegX, nBegY, nEndX, nEndY;
	GetSelectedIndex( nBegX, nBegY, nEndX, nEndY );

	INT nReplaced = ReplaceThisOccurrence(nBegX, nBegY, nEndX, nEndY, lpszReplaceString, nOptions, clsRegExp);

	SetCaretPosY( GetPosYFromIdxY( nEndX, nEndY ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nEndX ) );

	return nReplaced;
}

BOOL CCedtView::ActionGoToLine(INT nIdxY)
{
	if( nIdxY < 0 ) nIdxY = 0;
	if( nIdxY > GetLastIdxY() ) nIdxY = GetLastIdxY();

	SetCaretPosY( GetPosYFromIdxY( 0, nIdxY ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, 0 ) );

	return ! IsCaretVisible();
}

BOOL CCedtView::ActionToggleBookmark()
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	ToggleBookmark(nIdxY);

	return TRUE;
}

BOOL CCedtView::ActionNextBookmark()
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );

	if( FindNextBookmark(nIdxY) ) {
		SetCaretPosY( GetPosYFromIdxY( 0, nIdxY ) );
		CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
		SetCaretPosX( GetPosXFromIdxX( rLne2, 0 ) );
	}

	return ! IsCaretVisible();
}

BOOL CCedtView::ActionPrevBookmark()
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );

	if( FindPrevBookmark(nIdxY) ) {
		SetCaretPosY( GetPosYFromIdxY( 0, nIdxY ) );
		CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
		SetCaretPosX( GetPosXFromIdxX( rLne2, 0 ) );
	}

	return ! IsCaretVisible();
}


BOOL CCedtView::ActionPrevEditingPosition()
{
	INT nIdxX, nIdxY;
	GetLastEditingIndex( nIdxX, nIdxY );

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX ) );

	return ! IsCaretVisible();
}

BOOL CCedtView::ActionPairsBeginPosition()
{
	if( m_bPairMatched ) { // pairs matched already
		if( ! m_bBeginningPair ) { // in pairs ending position
			SetCaretPosY( m_nPairPosY[1] ); SetCaretPosX( m_nPairPosX[1] );
		}
	} else { // search new pairs position
		INT nIdxX, nIdxY; PositionToIndex( m_nCaretPosX, m_nCaretPosY, nIdxX, nIdxY );
		if( ReverseFindBeginningPair(nIdxX, nIdxY) ) {
			INT nPosX, nPosY; IndexToPosition( nIdxX, nIdxY, nPosX, nPosY );
			SetCaretPosY( nPosY ); SetCaretPosX( nPosX );
		}
	}

	return ! IsCaretVisible();
}

BOOL CCedtView::ActionPairsEndPosition()
{
	if( m_bPairMatched ) { // pairs matched already
		if( m_bBeginningPair ) { // in pairs beginning position
			SetCaretPosY( m_nPairPosY[1] ); SetCaretPosX( m_nPairPosX[1] );
		}
	} else { // search new pairs position
		INT nIdxX, nIdxY; PositionToIndex( m_nCaretPosX, m_nCaretPosY, nIdxX, nIdxY );
		TRACE2("%d, %d\n", nIdxX, nIdxY);
		if( ForwardFindEndingPair(nIdxX, nIdxY) ) {
			INT nPosX, nPosY; IndexToPosition( nIdxX, nIdxY, nPosX, nPosY );
			SetCaretPosY( nPosY ); SetCaretPosX( nPosX );
		}
	}

	return ! IsCaretVisible();
}


BOOL CCedtView::OnePassFindString(INT & nIdxX, INT & nIdxY, LPCTSTR lpszFindString, UINT nOptions, CRegExp & clsRegExp)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->OnePassFindString(nIdxX, nIdxY, lpszFindString, nOptions, clsRegExp);
}

BOOL CCedtView::ForwardFindString(INT & nIdxX, INT & nIdxY, LPCTSTR lpszFindString, UINT nOptions, CRegExp & clsRegExp, BOOL * pSearchWrap)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->ForwardFindString(nIdxX, nIdxY, lpszFindString, nOptions, clsRegExp, pSearchWrap);
}

BOOL CCedtView::ReverseFindString(INT & nIdxX, INT & nIdxY, LPCTSTR lpszFindString, UINT nOptions, CRegExp & clsRegExp, BOOL * pSearchWrap)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->ReverseFindString(nIdxX, nIdxY, lpszFindString, nOptions, clsRegExp, pSearchWrap);
}

INT CCedtView::ReplaceAllInSelection(INT & nBegX, INT & nBegY, INT & nEndX, INT & nEndY, LPCTSTR lpszFindString, LPCTSTR lpszReplaceString, UINT nOptions, CRegExp & clsRegExp)
{
	INT nIdxX = nBegX, nIdxY = nBegY, nReplaced = 0;
	CString szReplaceString = lpszReplaceString;

	while( OnePassFindString(nIdxX, nIdxY, lpszFindString, nOptions, clsRegExp) ) {
		if( SEARCH_REG_EXP(nOptions) ) {
			INT nFound; CString szExpression = lpszReplaceString;
			while( (nFound = szExpression.Find("\\t", 0)) >= 0 ) { szExpression = szExpression.Left(nFound) + "\t" + szExpression.Mid(nFound+2); }
			clsRegExp.GetReplaceString( szExpression, szReplaceString );
		}

		INT nLen = SEARCH_REG_EXP(nOptions) ? clsRegExp.GetFoundLength() : strlen(lpszFindString);
		INT nLe2 = strlen(szReplaceString);

		if( ! IsStringInSelection(nIdxX, nIdxY, nLen, nBegX, nBegY, nEndX, nEndY) ) break;

		if( nLen ) DeleteString(nIdxX, nIdxY, nLen);
		if( nLe2 ) InsertString(nIdxX, nIdxY, szReplaceString);

		// if it is the last line of selection then adjust selection range
		if( nIdxY == nEndY ) nEndX = nEndX + nLe2 - nLen;

		// increment nIdxX to make search begin next to the current search result
		nIdxX = nIdxX + nLe2; nReplaced++;

		// the following code segment will fix the bug that happens when replacing end of line to something
		if( nIdxX == GetLastIdxX( GetLineFromIdxY(nIdxY) ) ) { nIdxX = 0; nIdxY += 1; }
	}

	return nReplaced;
}

INT CCedtView::ReplaceAllInFile(LPCTSTR lpszFindString, LPCTSTR lpszReplaceString, UINT nOptions, CRegExp & clsRegExp)
{
	INT nIdxX = 0, nIdxY = 0, nReplaced = 0;
	CString szReplaceString = lpszReplaceString;

	while( OnePassFindString(nIdxX, nIdxY, lpszFindString, nOptions, clsRegExp) ) {
		if( SEARCH_REG_EXP(nOptions) ) {
			INT nFound; CString szExpression = lpszReplaceString;
			while( (nFound = szExpression.Find("\\t", 0)) >= 0 ) { szExpression = szExpression.Left(nFound) + "\t" + szExpression.Mid(nFound+2); }
			clsRegExp.GetReplaceString( szExpression, szReplaceString );
		}

		INT nLen = SEARCH_REG_EXP(nOptions) ? clsRegExp.GetFoundLength() : strlen(lpszFindString);
		INT nLe2 = strlen(szReplaceString);

		if( nLen ) DeleteString(nIdxX, nIdxY, nLen);
		if( nLe2 ) InsertString(nIdxX, nIdxY, szReplaceString);

		// increment nIdxX to make search begin next to the current search result
		nIdxX = nIdxX + nLe2; nReplaced++;

		// the following code segment will fix the bug that happens when replacing end of line to something
		if( nIdxX == GetLastIdxX( GetLineFromIdxY(nIdxY) ) ) { nIdxX = 0; nIdxY += 1; }
	}

	return nReplaced;
}

INT CCedtView::ReplaceThisOccurrence(INT & nBegX, INT & nBegY, INT & nEndX, INT & nEndY, LPCTSTR lpszReplaceString, UINT nOptions, CRegExp & clsRegExp)
{
	CString szReplaceString = lpszReplaceString;

	if( SEARCH_REG_EXP(nOptions) ) {
		INT nTab; CString szExpression = lpszReplaceString;
		while( (nTab = szExpression.Find("\\t", 0)) >= 0 ) { szExpression = szExpression.Left(nTab) + "\t" + szExpression.Mid(nTab+2); }
		clsRegExp.GetReplaceString( szExpression, szReplaceString );
	}

	if( nBegY != nEndY ) DeleteLineSelection(nBegX, nBegY, nEndX, nEndY);
	else DeleteString(nBegX, nBegY, nEndX-nBegX);

	InsertString(nBegX, nBegY, szReplaceString);
	nEndY = nBegY; nEndX = nBegX + strlen(szReplaceString);

	return 1;
}

void CCedtView::ToggleBookmark(INT nIdxY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->ToggleBookmark(nIdxY);
	pDoc->FormatScreenText(nIdxY, 1);
}

BOOL CCedtView::FindNextBookmark(INT & nIdxY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->FindNextBookmark(nIdxY);
}

BOOL CCedtView::FindPrevBookmark(INT & nIdxY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->FindPrevBookmark(nIdxY);
}

BOOL CCedtView::IsThisIndentOnChar(INT nIdxX, INT nIdxY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->IsThisIndentOnChar(nIdxX, nIdxY);
}

BOOL CCedtView::IsThisIndentOffChar(INT nIdxX, INT nIdxY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->IsThisIndentOffChar(nIdxX, nIdxY);
}

BOOL CCedtView::ForwardFindEndingPair(INT & nIdxX, INT & nIdxY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->ForwardFindEndingPair(nIdxX, nIdxY);
}

BOOL CCedtView::ReverseFindBeginningPair(INT & nIdxX, INT & nIdxY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->ReverseFindBeginningPair(nIdxX, nIdxY);
}

BOOL CCedtView::IsThisOneOfPairs(INT nIdxX, INT nIdxY, BOOL & bBeginning)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->IsThisOneOfPairs(nIdxX, nIdxY, bBeginning);
}

BOOL CCedtView::FindAnotherOneOfPairs(INT & nIdxX, INT & nIdxY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->FindAnotherOneOfPairs(nIdxX, nIdxY);
}

BOOL CCedtView::IsStringInSelection(INT nIdxX, INT nIdxY, INT nLength, INT nBegX, INT nBegY, INT nEndX, INT nEndY)
{
	if( nBegY == nEndY ) {
		if( nIdxY == nBegY && nIdxX >= nBegX && nIdxX + nLength <= nEndX ) return TRUE;
		else return FALSE;
	} else {
		if( nIdxY <  nBegY ) return FALSE;
		if( nIdxY == nBegY ) return (nIdxX < nBegX) ? FALSE : TRUE;
		if( nIdxY <  nEndY ) return TRUE;
		if( nIdxY == nEndY ) return (nEndX > 0 && nIdxX + nLength <= nEndX) ? TRUE : FALSE;
		return FALSE;
	}
}

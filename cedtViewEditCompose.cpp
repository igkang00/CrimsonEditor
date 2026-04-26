#include "stdafx.h"
#include "cedtHeader.h"


static INT _nIdxX, _nIdxY;


void CCedtView::ActionCompositionStart()
{
	EmptySavedCompositionString();
}

void CCedtView::ActionCompositionEnd()
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX, ! m_bColumnMode );

	if( ! IsCompositionStringSaved() ) {
	//	in case of SE-BEUL-SIK
	//	do nothing...
	} else {
		RestoreCurrentCompositionString( nIdxY );
		EmptySavedCompositionString();
	}

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY, ! m_bColumnMode ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX, ! m_bColumnMode ) );
}

void CCedtView::ActionCompositionCompose(LPCTSTR lpszString)
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX, ! m_bColumnMode );

	if( ! IsCompositionStringSaved() ) {
	//	begin composition...
		SaveCurrentCompositionString( nIdxY );
	} else {
	//	during composition...
		RestoreCurrentCompositionString( nIdxY );
	}

	INT nSize = strlen( lpszString );
	INT nAncX = nIdxX + nSize;

	CAnalyzedString & rString = GetLineFromIdxY( nIdxY );
	INT nLstX = GetLastIdxX( rString );

	if( nSize > 0 && nIdxX < nLstX && m_bOverwriteMode ) {
		if( nLstX - nIdxX > nSize ) {
			FORMATEDWORD & rWord = GetWordFromIdxX( rLine, nIdxX+nSize-1 );
			if( IS_DBCHAR(rWord) && nIdxX+nSize-1 == rWord.m_siIndex ) {
				CString szInsert(lpszString); szInsert += ' ';
				DeleteCompositionString(nIdxX, nIdxY, nSize+1);
				InsertCompositionString(nIdxX, nIdxY, szInsert);
			} else {
				DeleteCompositionString(nIdxX, nIdxY, nSize);
				InsertCompositionString(nIdxX, nIdxY, lpszString);
			}
		} else {
			DeleteCompositionString(nIdxX, nIdxY, nLstX - nIdxX);
			InsertCompositionString(nIdxX, nIdxY, lpszString);
		}
	} else if( nSize > 0 && nIdxX > nLstX ) {
		CString szInsert(' ', nIdxX - nLstX);
		InsertCompositionString(nLstX, nIdxY, szInsert);
		InsertCompositionString(nIdxX, nIdxY, lpszString);
	} else if( nSize > 0 ) {
		InsertCompositionString(nIdxX, nIdxY, lpszString);
	}

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY, ! m_bColumnMode ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX, ! m_bColumnMode ) );

	m_nAnchorPosY = GetPosYFromIdxY( nAncX, nIdxY );
	CFormatedString & rLne3 = GetLineFromPosY( m_nAnchorPosY );
	m_nAnchorPosX = GetPosXFromIdxX( rLne3, nAncX );
}

void CCedtView::ActionCompositionResult(LPCTSTR lpszString)
{
	INT nIdxY = GetIdxYFromPosY( m_nCaretPosY );
	CFormatedString & rLine = GetLineFromPosY( m_nCaretPosY );
	INT nIdxX = GetIdxXFromPosX( rLine, m_nCaretPosX, ! m_bColumnMode );

	if( ! IsCompositionStringSaved() ) {
	//	in case of SE-BEUL-SIK
	//	do nothing...
	} else {
		RestoreCurrentCompositionString( nIdxY );
		EmptySavedCompositionString();
	}

	INT nSize = strlen( lpszString );

	CAnalyzedString & rString = GetLineFromIdxY( nIdxY );
	INT nLstX = GetLastIdxX( rString );

	if( nSize > 0 && nIdxX < nLstX && m_bOverwriteMode ) {
		if( nLstX - nIdxX > nSize ) {
			FORMATEDWORD & rWord = GetWordFromIdxX( rLine, nIdxX+nSize-1 );
			if( IS_DBCHAR(rWord) && nIdxX+nSize-1 == rWord.m_siIndex ) {
				CString szInsert(lpszString); szInsert += ' ';
				DeleteString(nIdxX, nIdxY, nSize+1);
				InsertString(nIdxX, nIdxY, szInsert);
			} else {
				DeleteString(nIdxX, nIdxY, nSize);
				InsertString(nIdxX, nIdxY, lpszString);
			}
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

	m_nAnchorPosY = m_nCaretPosY;
	m_nAnchorPosX = m_nCaretPosX;
}


////////////////////////////////////////////////
// BASIC EDITING FUNCTIONS
BOOL CCedtView::IsCompositionStringSaved()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->IsCompositionStringSaved();
}

void CCedtView::EmptySavedCompositionString()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->EmptySavedCompositionString();
}

void CCedtView::SaveCurrentCompositionString(INT nIdxY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->SaveCurrentCompositionString(nIdxY);
}

void CCedtView::RestoreCurrentCompositionString(INT nIdxY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->RestoreCurrentCompositionString(nIdxY);

	pDoc->AnalyzeText(nIdxY, 1);
	pDoc->FormatScreenText(nIdxY, 1);
}

void CCedtView::InsertCompositionString(INT nIdxX, INT nIdxY, LPCTSTR lpszString)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->InsertCompositionString(nIdxX, nIdxY, lpszString);

	pDoc->AnalyzeText(nIdxY, 1);
	pDoc->FormatScreenText(nIdxY, 1);
}

void CCedtView::DeleteCompositionString(INT nIdxX, INT nIdxY, INT nLength)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->DeleteCompositionString(nIdxX, nIdxY, nLength);

	pDoc->AnalyzeText(nIdxY, 1);
	pDoc->FormatScreenText(nIdxY, 1);
}


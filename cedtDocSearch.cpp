#include "stdafx.h"
#include "cedtHeader.h"



static TCHAR _nPairCancelWith, _nPairLookingFor;
static BOOL _bPairFindForward;


BOOL CCedtDoc::OnePassFindString(INT & nIdxX, INT & nIdxY, LPCTSTR lpszFindString, UINT nOptions, CRegExp & clsRegExp)
{
	INT nLine = nIdxY; POSITION pos = m_clsAnalyzedText.FindIndex(nIdxY);

	while( pos ) {
		CAnalyzedString & rLine = m_clsAnalyzedText.GetNext(pos);
		INT nFound, nFrom = (nLine == nIdxY) ? nIdxX : -1;

		if( ! SEARCH_REG_EXP(nOptions) ) nFound = ::ForwardFindString(rLine, lpszFindString, nFrom, SEARCH_WHOLE_WORD(nOptions), SEARCH_MATCH_CASE(nOptions));
		else nFound = ::ForwardFindStringRegExp(rLine, lpszFindString, clsRegExp, nFrom, SEARCH_WHOLE_WORD(nOptions), SEARCH_MATCH_CASE(nOptions));

		if( nFound >= 0 ) { nIdxY = nLine; nIdxX = nFound; return TRUE; }
		else nLine++;
	}

	return FALSE;
}

BOOL CCedtDoc::ForwardFindString(INT & nIdxX, INT & nIdxY, LPCTSTR lpszFindString, UINT nOptions, CRegExp & clsRegExp, BOOL * pSearchWrap)
{
	if( pSearchWrap ) * pSearchWrap = FALSE;
	INT nLine = nIdxY; POSITION pos = m_clsAnalyzedText.FindIndex(nIdxY);

	while( pos ) {
		CAnalyzedString & rLine = m_clsAnalyzedText.GetNext(pos);
		INT nFound, nFrom = (nLine == nIdxY) ? nIdxX : -1;

		if( ! SEARCH_REG_EXP(nOptions) ) nFound = ::ForwardFindString(rLine, lpszFindString, nFrom, SEARCH_WHOLE_WORD(nOptions), SEARCH_MATCH_CASE(nOptions));
		else nFound = ::ForwardFindStringRegExp(rLine, lpszFindString, clsRegExp, nFrom, SEARCH_WHOLE_WORD(nOptions), SEARCH_MATCH_CASE(nOptions));

		if( nFound >= 0 ) { nIdxY = nLine; nIdxX = nFound; return TRUE; }
		else nLine++;
	}

	if( ! CCedtView::m_bSearchWrapAtEndOfFile ) return FALSE;

	if( pSearchWrap ) * pSearchWrap = TRUE;
	nLine = 0; pos = m_clsAnalyzedText.GetHeadPosition();

	while( pos ) {
		CAnalyzedString & rLine = m_clsAnalyzedText.GetNext(pos);
		INT nFound, nFrom = -1;

		if( ! SEARCH_REG_EXP(nOptions) ) nFound = ::ForwardFindString(rLine, lpszFindString, nFrom, SEARCH_WHOLE_WORD(nOptions), SEARCH_MATCH_CASE(nOptions));
		else nFound = ::ForwardFindStringRegExp(rLine, lpszFindString, clsRegExp, nFrom, SEARCH_WHOLE_WORD(nOptions), SEARCH_MATCH_CASE(nOptions));

		if( nFound >= 0 ) { nIdxY = nLine; nIdxX = nFound; return TRUE; }
		else nLine++;
	}

	return FALSE;
}

BOOL CCedtDoc::ReverseFindString(INT & nIdxX, INT & nIdxY, LPCTSTR lpszFindString, UINT nOptions, CRegExp & clsRegExp, BOOL * pSearchWrap)
{
	if( pSearchWrap ) * pSearchWrap = FALSE;
	INT nLine = nIdxY; POSITION pos = m_clsAnalyzedText.FindIndex(nIdxY);

	while( pos ) {
		CAnalyzedString & rLine = m_clsAnalyzedText.GetPrev(pos);
		INT nFound, nFrom = (nLine == nIdxY) ? nIdxX : -1;

		if( ! SEARCH_REG_EXP(nOptions) ) nFound = ::ReverseFindString(rLine, lpszFindString, nFrom, SEARCH_WHOLE_WORD(nOptions), SEARCH_MATCH_CASE(nOptions));
		else nFound = ::ReverseFindStringRegExp(rLine, lpszFindString, clsRegExp, nFrom, SEARCH_WHOLE_WORD(nOptions), SEARCH_MATCH_CASE(nOptions));

		if( nFound >= 0 ) { nIdxY = nLine; nIdxX = nFound; return TRUE; }
		else nLine--;
	}

	if( ! CCedtView::m_bSearchWrapAtEndOfFile ) return FALSE;

	if( pSearchWrap ) * pSearchWrap = TRUE;
	nLine = m_clsAnalyzedText.GetCount()-1; pos = m_clsAnalyzedText.GetTailPosition();

	while( pos ) {
		CAnalyzedString & rLine = m_clsAnalyzedText.GetPrev(pos);
		INT nFound, nFrom = -1;

		if( ! SEARCH_REG_EXP(nOptions) ) nFound = ::ReverseFindString(rLine, lpszFindString, nFrom, SEARCH_WHOLE_WORD(nOptions), SEARCH_MATCH_CASE(nOptions));
		else nFound = ::ReverseFindStringRegExp(rLine, lpszFindString, clsRegExp, nFrom, SEARCH_WHOLE_WORD(nOptions), SEARCH_MATCH_CASE(nOptions));

		if( nFound >= 0 ) { nIdxY = nLine; nIdxX = nFound; return TRUE; }
		else nLine--;
	}

	return FALSE;
}

void CCedtDoc::ToggleBookmark(INT nIdxY)
{
	CAnalyzedString & rLine = GetLineFromIdxY(nIdxY);

	if( rLine.m_usLineInfo & LI_HAVEBOOKMARK ) rLine.m_usLineInfo &= ~LI_HAVEBOOKMARK;
	else rLine.m_usLineInfo |= LI_HAVEBOOKMARK;
}

BOOL CCedtDoc::FindNextBookmark(INT & nIdxY)
{
	INT nLine = nIdxY+1; POSITION pos = m_clsAnalyzedText.FindIndex(nIdxY+1);

	while( pos ) {
		CAnalyzedString & rLine = m_clsAnalyzedText.GetNext(pos);
		if( rLine.m_usLineInfo & LI_HAVEBOOKMARK ) { nIdxY = nLine; return TRUE; }
		else nLine++;
	}

	nLine = 0; pos = m_clsAnalyzedText.GetHeadPosition();

	while( pos ) {
		CAnalyzedString & rLine = m_clsAnalyzedText.GetNext(pos);
		if( rLine.m_usLineInfo & LI_HAVEBOOKMARK ) { nIdxY = nLine; return TRUE; }
		else nLine++;
	}

	return FALSE;
}

BOOL CCedtDoc::FindPrevBookmark(INT & nIdxY)
{
	INT nLine = nIdxY-1; POSITION pos = m_clsAnalyzedText.FindIndex(nIdxY-1);

	while( pos ) {
		CAnalyzedString & rLine = m_clsAnalyzedText.GetPrev(pos);
		if( rLine.m_usLineInfo & LI_HAVEBOOKMARK ) { nIdxY = nLine; return TRUE; }
		else nLine--;
	}

	nLine = m_clsAnalyzedText.GetCount()-1; pos = m_clsAnalyzedText.GetTailPosition();

	while( pos ) {
		CAnalyzedString & rLine = m_clsAnalyzedText.GetPrev(pos);
		if( rLine.m_usLineInfo & LI_HAVEBOOKMARK ) { nIdxY = nLine; return TRUE; }
		else nLine--;
	}

	return FALSE;
}


BOOL CCedtDoc::IsThisIndentOnChar(INT nIdxX, INT nIdxY)
{
	POSITION pos = m_clsAnalyzedText.FindIndex(nIdxY);
	CAnalyzedString & rLine = m_clsAnalyzedText.GetAt(pos);
	ANALYZEDWORD & rWord = GetWordFromIdxX( rLine, nIdxX );

	if( ! IS_DELIMITER(rWord) ) return FALSE;
	TCHAR nChar = rLine[(INT)rWord.m_siIndex];

	if( nChar == m_clsLangSpec.m_chIndentationOn ) return TRUE;
	return FALSE;
}

BOOL CCedtDoc::IsThisIndentOffChar(INT nIdxX, INT nIdxY)
{
	POSITION pos = m_clsAnalyzedText.FindIndex(nIdxY);
	CAnalyzedString & rLine = m_clsAnalyzedText.GetAt(pos);
	ANALYZEDWORD & rWord = GetWordFromIdxX( rLine, nIdxX );

	if( ! IS_DELIMITER(rWord) ) return FALSE;
	TCHAR nChar = rLine[(INT)rWord.m_siIndex];

	if( nChar == m_clsLangSpec.m_chIndentationOff ) return TRUE;
	return FALSE;
}


BOOL CCedtDoc::ForwardFindEndingPair(INT & nIdxX, INT & nIdxY)
{
	POSITION pos = m_clsAnalyzedText.FindIndex(nIdxY);
	SHORT i, siWordCount, siBegin; INT j, nLine = nIdxY;

	LPCTSTR PAIRS[3]; INT nPairs[3]; INT nDepth[3];
	PAIRS[0] = m_clsLangSpec.m_szPairs1; nPairs[0] = strlen(PAIRS[0]); nDepth[0] = 0;
	PAIRS[1] = m_clsLangSpec.m_szPairs2; nPairs[1] = strlen(PAIRS[1]); nDepth[1] = 0;
	PAIRS[2] = m_clsLangSpec.m_szPairs3; nPairs[2] = strlen(PAIRS[2]); nDepth[2] = 0;

	while( pos ) {
		CAnalyzedString & rLine = m_clsAnalyzedText.GetNext(pos);
		siWordCount = rLine.m_siWordCount;

		if( nLine == nIdxY ) {
			for(siBegin = siWordCount-1, i = 0; i < siWordCount; i++) {
				ANALYZEDWORD & rWord = rLine.m_pWord[i];
				if( rWord.m_siIndex + rWord.m_siLength > nIdxX ) { siBegin = i; break; }
			}
		} else siBegin = 0;

		for(i = siBegin; i < siWordCount; i++) {
			ANALYZEDWORD & rWord = rLine.m_pWord[i];
		//	if( ! IS_DELIMITER(rWord) ) continue;
			if( rWord.m_siLength != 1 ) continue;

			for(j = 0; j < 3; j++) {
				if( nPairs[j] != 2 ) continue;
				if( rLine[(INT)rWord.m_siIndex] == PAIRS[j][0] ) nDepth[j]++;
			}

			for(j = 0; j < 3; j++) {
				if( nPairs[j] != 2 ) continue;
				if( rLine[(INT)rWord.m_siIndex] == PAIRS[j][1] ) nDepth[j]--;

				if( nDepth[j] == -1 ) {
					nIdxY = nLine;
					nIdxX = rWord.m_siIndex;
					return TRUE;
				}
			}
		}
		nLine++;
	}

	return FALSE;
}

BOOL CCedtDoc::ReverseFindBeginningPair(INT & nIdxX, INT & nIdxY)
{
	POSITION pos = m_clsAnalyzedText.FindIndex(nIdxY);
	SHORT i, siWordCount, siBegin; INT j, nLine = nIdxY;

	LPCTSTR PAIRS[3]; INT nPairs[3]; INT nDepth[3];
	PAIRS[0] = m_clsLangSpec.m_szPairs1; nPairs[0] = strlen(PAIRS[0]); nDepth[0] = 0;
	PAIRS[1] = m_clsLangSpec.m_szPairs2; nPairs[1] = strlen(PAIRS[1]); nDepth[1] = 0;
	PAIRS[2] = m_clsLangSpec.m_szPairs3; nPairs[2] = strlen(PAIRS[2]); nDepth[2] = 0;

	while( pos ) {
		CAnalyzedString & rLine = m_clsAnalyzedText.GetPrev(pos);
		siWordCount = rLine.m_siWordCount;

		if( nLine == nIdxY ) {
			for(siBegin = siWordCount-1, i = 0; i < siWordCount; i++) {
				ANALYZEDWORD & rWord = rLine.m_pWord[i];
				if( rWord.m_siIndex + rWord.m_siLength > nIdxX ) { siBegin = i; break; }
			}
		} else siBegin = siWordCount-1;

		for(i = siBegin; i >= 0; i--) {
			ANALYZEDWORD & rWord = rLine.m_pWord[i];
		//	if( ! IS_DELIMITER(rWord) ) continue;
			if( rWord.m_siLength != 1 ) continue;

			for(j = 0; j < 3; j++) {
				if( nPairs[j] != 2 ) continue;
				if( rLine[(INT)rWord.m_siIndex] == PAIRS[j][1] ) nDepth[j]++;
			}

			for(j = 0; j < 3; j++) {
				if( nPairs[j] != 2 ) continue;
				if( rLine[(INT)rWord.m_siIndex] == PAIRS[j][0] ) nDepth[j]--;

				if( nDepth[j] == -1 ) {
					nIdxY = nLine;
					nIdxX = rWord.m_siIndex;
					return TRUE;
				}
			}
		}
		nLine--;
	}

	return FALSE;
}


BOOL CCedtDoc::IsThisOneOfPairs(INT nIdxX, INT nIdxY, BOOL & bBeginning)
{
	POSITION pos = m_clsAnalyzedText.FindIndex(nIdxY);
	CAnalyzedString & rLine = m_clsAnalyzedText.GetAt(pos);
	ANALYZEDWORD & rWord = GetWordFromIdxX( rLine, nIdxX );

//	if( ! IS_DELIMITER(rWord) ) return FALSE;
	if( rWord.m_siLength != 1 ) return FALSE;
	TCHAR nChar = rLine.GetAt(rWord.m_siIndex);

	LPCTSTR PAIRS[3];
	PAIRS[0] = m_clsLangSpec.m_szPairs1;
	PAIRS[1] = m_clsLangSpec.m_szPairs2;
	PAIRS[2] = m_clsLangSpec.m_szPairs3;

	for(INT i = 0; i < 3; i++) {
		if( strlen(PAIRS[i]) != 2 ) continue;

		if( nChar == PAIRS[i][0] ) {
			_nPairCancelWith = PAIRS[i][0];	_nPairLookingFor = PAIRS[i][1];
			_bPairFindForward = TRUE;		bBeginning = TRUE;
			return TRUE;
		} 
		
		if( nChar == PAIRS[i][1] ) {
			_nPairCancelWith = PAIRS[i][1];	_nPairLookingFor = PAIRS[i][0];
			_bPairFindForward = FALSE;		bBeginning = FALSE;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CCedtDoc::FindAnotherOneOfPairs(INT & nIdxX, INT & nIdxY)
{
	if( _bPairFindForward ) 
		return ForwardFindAnotherOneOfPairs(nIdxX, nIdxY);
	else 
		return ReverseFindAnotherOneOfPairs(nIdxX, nIdxY);
}


BOOL CCedtDoc::ForwardFindAnotherOneOfPairs(INT & nIdxX, INT & nIdxY)
{
	POSITION pos = m_clsAnalyzedText.FindIndex(nIdxY);
	SHORT i, siWordCount, siBegin;
	INT nLine = nIdxY, nDepth = 0;

	while( pos ) {
		CAnalyzedString & rLine = m_clsAnalyzedText.GetNext(pos);
		siWordCount = rLine.m_siWordCount;

		if( nLine == nIdxY ) {
			for(siBegin = siWordCount-1, i = 0; i < siWordCount; i++) {
				ANALYZEDWORD & rWord = rLine.m_pWord[i];
				if( rWord.m_siIndex + rWord.m_siLength > nIdxX ) { siBegin = i; break; }
			}
		} else siBegin = 0;

		for(i = siBegin; i < siWordCount; i++) {
			ANALYZEDWORD & rWord = rLine.m_pWord[i];
		//	if( ! IS_DELIMITER(rWord) ) continue;
			if( rWord.m_siLength != 1 ) continue;

			if( rLine[(INT)rWord.m_siIndex] == _nPairCancelWith ) nDepth++;
			if( rLine[(INT)rWord.m_siIndex] == _nPairLookingFor ) nDepth--;

			if( nDepth == 0 ) {
				nIdxY = nLine;
				nIdxX = rWord.m_siIndex;
				return TRUE;
			}
		}
		nLine++;
	}

	return FALSE;
}

BOOL CCedtDoc::ReverseFindAnotherOneOfPairs(INT & nIdxX, INT & nIdxY)
{
	POSITION pos = m_clsAnalyzedText.FindIndex(nIdxY);
	SHORT i, siWordCount, siBegin;
	INT nLine = nIdxY, nDepth = 0;

	while( pos ) {
		CAnalyzedString & rLine = m_clsAnalyzedText.GetPrev(pos);
		siWordCount = rLine.m_siWordCount;

		if( nLine == nIdxY ) {
			for(siBegin = siWordCount-1, i = 0; i < siWordCount; i++) {
				ANALYZEDWORD & rWord = rLine.m_pWord[i];
				if( rWord.m_siIndex + rWord.m_siLength > nIdxX ) { siBegin = i; break; }
			}
		} else siBegin = siWordCount-1;

		for(i = siBegin; i >= 0; i--) {
			ANALYZEDWORD & rWord = rLine.m_pWord[i];
		//	if( ! IS_DELIMITER(rWord) ) continue;
			if( rWord.m_siLength != 1 ) continue;

			if( rLine[(INT)rWord.m_siIndex] == _nPairCancelWith ) nDepth++;
			if( rLine[(INT)rWord.m_siIndex] == _nPairLookingFor ) nDepth--;

			if( nDepth == 0 ) {
				nIdxY = nLine;
				nIdxX = rWord.m_siIndex;
				return TRUE;
			}
		}
		nLine--;
	}

	return FALSE;
}

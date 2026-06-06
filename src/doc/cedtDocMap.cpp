#include "stdafx.h"
#include "cedtHeader.h"


#define IS_COUNTABLE_WORD(rWord) ( \
  ( (rWord.m_ucType[0]) >= WT_KEYWORD0   && (rWord.m_ucType[0]) <= WT_KEYWORD9 ) || \
	(rWord.m_ucType[0]) == WT_CONSTANT   || (rWord.m_ucType[0]) == WT_VARIABLE   || \
	(rWord.m_ucType[0]) == WT_IDENTIFIER || (rWord.m_ucType[0]) == WT_WRONGWORD  || \
	(rWord.m_ucType[0]) == WT_DBCHAR \
)

INT CCedtDoc::GetWordCount()
{
	INT nWordCount = 0;
	POSITION pos = m_clsAnalyzedText.GetHeadPosition();

	while( pos ) {
		CAnalyzedString & rLine = m_clsAnalyzedText.GetNext( pos );
		for(SHORT i = 0; i < rLine.m_siWordCount; i++) {
			ANALYZEDWORD & rWord = rLine.m_pWord[i];
			if( IS_COUNTABLE_WORD(rWord) ) nWordCount++;
		}
	}

	return nWordCount;
}

INT CCedtDoc::GetByteCount()
{
	INT nByteCount = 0, nLineFeed = 2;
	if( m_nFileFormat != FILE_FORMAT_DOS ) nLineFeed = 1;

	POSITION pos = m_clsAnalyzedText.GetHeadPosition();

	while( pos ) {
		CAnalyzedString & rLine = m_clsAnalyzedText.GetNext( pos );
		nByteCount += rLine.GetLength();
		if( pos ) nByteCount += nLineFeed;
	}

	return nByteCount;
}

INT CCedtDoc::GetFirstIdxX(CAnalyzedString & rLine)
{
	ANALYZEDWORD & rWord = rLine.m_pWord[0];
	return rWord.m_siIndex;
}

INT CCedtDoc::GetLastIdxX(CAnalyzedString & rLine)
{
	ANALYZEDWORD & rWord = rLine.m_pWord[rLine.m_siWordCount-1];
	return rWord.m_siIndex + rWord.m_siLength;
}

INT CCedtDoc::GetFirstNonBlankIdxX(CAnalyzedString & rLine)
{
	SHORT i, sCount = rLine.m_siWordCount;
	for(i = 0; i < sCount; i++) {
		ANALYZEDWORD & rWord = rLine.m_pWord[i];
		if( ! IS_WSPACE(rWord) ) return rWord.m_siIndex;
	}
	return GetLastIdxX(rLine);
}

INT CCedtDoc::GetTrailingBlankIdxX(CAnalyzedString & rLine)
{
	SHORT i, sCount = rLine.m_siWordCount;
	for(i = sCount-1; i >= 0; i--) {
		ANALYZEDWORD & rWord = rLine.m_pWord[i]; 
		if( ! IS_WSPACE(rWord) ) return rWord.m_siIndex + rWord.m_siLength;
	}
	return GetFirstIdxX(rLine);
}

CAnalyzedString & CCedtDoc::GetLineFromIdxY(INT nIdxY)
{
	POSITION pos = m_clsAnalyzedText.FindIndex(nIdxY);
	if( pos ) return m_clsAnalyzedText.GetAt(pos);
	return m_clsAnalyzedText.GetTail();
}

ANALYZEDWORD & CCedtDoc::GetWordFromIdxX(CAnalyzedString & rLine, INT nIdxX)
{
	SHORT i, siCount = rLine.m_siWordCount;
	for(i = 0; i < siCount; i++) {
		ANALYZEDWORD & rWord = rLine.m_pWord[i];
		if( rWord.m_siIndex + rWord.m_siLength > nIdxX ) return rLine.m_pWord[i];
	}
	return rLine.m_pWord[siCount-1];
}

BOOL CCedtDoc::IsBlankLine(CAnalyzedString & rLine)
{
	SHORT i, siCount = rLine.m_siWordCount;
	for(i = 0; i < siCount; i++) {
		ANALYZEDWORD & rWord = rLine.m_pWord[i];
		if( ! IS_WSPACE(rWord) ) return FALSE;
	}
	return TRUE;
}

BOOL CCedtDoc::IsBlankLineFromIdxY(INT nIdxY)
{
	CAnalyzedString & rLine = GetLineFromIdxY( nIdxY );
	return IsBlankLine( rLine );
}

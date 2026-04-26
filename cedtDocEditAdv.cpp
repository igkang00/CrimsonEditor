#include "stdafx.h"
#include "cedtHeader.h"



INT CCedtDoc::IndentLine(INT nIdxY, BOOL bUseTab, INT nIndentSize) 
{
	CAnalyzedString & rLine = GetLineFromIdxY(nIdxY);
	return FastIndentLine(rLine, nIdxY, bUseTab, nIndentSize);
}

INT CCedtDoc::UnindentLine(INT nIdxY, INT nTabSize, INT nIndentSize)
{
	CAnalyzedString & rLine = GetLineFromIdxY(nIdxY);
	return FastUnindentLine(rLine, nIdxY, nTabSize, nIndentSize);
}

INT CCedtDoc::MakeCommentLine(INT nIdxY) 
{
	CAnalyzedString & rLine = GetLineFromIdxY(nIdxY);
	return FastMakeCommentLine(rLine, nIdxY);
}

INT CCedtDoc::ReleaseCommentLine(INT nIdxY) 
{
	CAnalyzedString & rLine = GetLineFromIdxY(nIdxY);
	return FastReleaseCommentLine(rLine, nIdxY);
}

INT CCedtDoc::ConvertTabsToSpaces(INT nIdxY)
{
	CAnalyzedString & rLine = GetLineFromIdxY(nIdxY);
	return FastConvertTabsToSpaces(rLine, nIdxY);
}

INT CCedtDoc::ConvertSpacesToTabs(INT nIdxY)
{
	CAnalyzedString & rLine = GetLineFromIdxY(nIdxY);
	return FastConvertSpacesToTabs(rLine, nIdxY);
}

INT CCedtDoc::LeadingTabsToSpaces(INT nIdxY)
{
	CAnalyzedString & rLine = GetLineFromIdxY(nIdxY);
	return FastLeadingTabsToSpaces(rLine, nIdxY);
}

INT CCedtDoc::LeadingSpacesToTabs(INT nIdxY)
{
	CAnalyzedString & rLine = GetLineFromIdxY(nIdxY);
	return FastLeadingSpacesToTabs(rLine, nIdxY);
}

INT CCedtDoc::DeleteLeadingSpaces(INT nIdxY)
{
	CAnalyzedString & rLine = GetLineFromIdxY(nIdxY);
	return FastDeleteLeadingSpaces(rLine, nIdxY);
}

INT CCedtDoc::DeleteTrailingSpaces(INT nIdxY)
{
	CAnalyzedString & rLine = GetLineFromIdxY(nIdxY);
	return FastDeleteTrailingSpaces(rLine, nIdxY);
}

void CCedtDoc::IndentBlock(INT nBegX, INT nBegY, INT nEndX, INT nEndY, BOOL bUseTab, INT nIndentSize)
{
	POSITION pos = m_clsAnalyzedText.FindIndex(nBegY);
	for(INT nIdxY = nBegY; nIdxY <= nEndY; nIdxY++) {
		CAnalyzedString & rLine = m_clsAnalyzedText.GetNext(pos);
		if( nIdxY == nEndY && nEndX == 0 ) continue;
		FastIndentLine(rLine, nIdxY, bUseTab, nIndentSize);
	}
}

void CCedtDoc::UnindentBlock(INT nBegX, INT nBegY, INT nEndX, INT nEndY, INT nTabSize, INT nIndentSize)
{
	POSITION pos = m_clsAnalyzedText.FindIndex(nBegY);
	for(INT nIdxY = nBegY; nIdxY <= nEndY; nIdxY++) {
		CAnalyzedString & rLine = m_clsAnalyzedText.GetNext(pos);
		if( nIdxY == nEndY && nEndX == 0 ) continue;
		FastUnindentLine(rLine, nIdxY, nTabSize, nIndentSize);
	}
}

void CCedtDoc::MakeCommentBlock(INT nBegX, INT nBegY, INT nEndX, INT nEndY)
{
	POSITION pos = m_clsAnalyzedText.FindIndex(nBegY);
	for(INT nIdxY = nBegY; nIdxY <= nEndY; nIdxY++) {
		CAnalyzedString & rLine = m_clsAnalyzedText.GetNext(pos);
		if( nIdxY == nEndY && nEndX == 0 ) continue;
		FastMakeCommentLine(rLine, nIdxY);
	}
}

void CCedtDoc::ReleaseCommentBlock(INT nBegX, INT nBegY, INT nEndX, INT nEndY)
{
	POSITION pos = m_clsAnalyzedText.FindIndex(nBegY);
	for(INT nIdxY = nBegY; nIdxY <= nEndY; nIdxY++) {
		CAnalyzedString & rLine = m_clsAnalyzedText.GetNext(pos);
		if( nIdxY == nEndY && nEndX == 0 ) continue;
		FastReleaseCommentLine(rLine, nIdxY);
	}
}

void CCedtDoc::ConvertTabsToSpacesDocument()
{
	POSITION pos = m_clsAnalyzedText.GetHeadPosition();
	INT nCount = m_clsAnalyzedText.GetCount();
	for(INT nIdxY = 0; nIdxY < nCount; nIdxY++) {
		CAnalyzedString & rLine = m_clsAnalyzedText.GetNext(pos);
		FastConvertTabsToSpaces(rLine, nIdxY);
	}
}

void CCedtDoc::ConvertSpacesToTabsDocument()
{
	POSITION pos = m_clsAnalyzedText.GetHeadPosition();
	INT nCount = m_clsAnalyzedText.GetCount();
	for(INT nIdxY = 0; nIdxY < nCount; nIdxY++) {
		CAnalyzedString & rLine = m_clsAnalyzedText.GetNext(pos);
		FastConvertSpacesToTabs(rLine, nIdxY);
	}
}

void CCedtDoc::LeadingSpacesToTabsDocument()
{
	POSITION pos = m_clsAnalyzedText.GetHeadPosition();
	INT nCount = m_clsAnalyzedText.GetCount();
	for(INT nIdxY = 0; nIdxY < nCount; nIdxY++) {
		CAnalyzedString & rLine = m_clsAnalyzedText.GetNext(pos);
		FastLeadingSpacesToTabs(rLine, nIdxY);
	}
}

void CCedtDoc::DeleteTrailingSpacesDocument()
{
	POSITION pos = m_clsAnalyzedText.GetHeadPosition();
	INT nCount = m_clsAnalyzedText.GetCount();
	for(INT nIdxY = 0; nIdxY < nCount; nIdxY++) {
		CAnalyzedString & rLine = m_clsAnalyzedText.GetNext(pos);
		FastDeleteTrailingSpaces(rLine, nIdxY);
	}
}

//////////////////////////////////////////////////////////////////////
// private editing functions
INT CCedtDoc::FastIndentLine(CAnalyzedString & rLine, INT nIdxY, BOOL bUseTab, INT nIndentSize)
{
	if( bUseTab ) {
		FastInsertChar(rLine, 0, nIdxY, '\t'); 
		return 1;
	} else {
		FastLeadingTabsToSpaces(rLine, nIdxY); // convert leading tabs to spaces
		FastInsertString(rLine, 0, nIdxY, CString(' ', nIndentSize)); 
		return nIndentSize;
	}
}

INT CCedtDoc::FastUnindentLine(CAnalyzedString & rLine, INT nIdxY, INT nTabSize, INT nIndentSize)
{
	INT nLen = rLine.GetLength(); if( ! nLen ) return 0;

	if( nTabSize == nIndentSize ) {
		if( rLine[0] == '\t' ) {
			FastDeleteChar(rLine, 0, nIdxY);
			return 1;
		} else if( rLine[0] == ' ' ) {
			INT j = 0; while( j < nLen && rLine[j] == ' ' && j < nIndentSize ) j++;
			if( j < nLen && rLine[j] == '\t' && j < nIndentSize ) j++;
			FastDeleteString(rLine, 0, nIdxY, j); 
			return j;
		} else return 0;
	} else {
		FastLeadingTabsToSpaces(rLine, nIdxY); // convert leading tabs to spaces
		INT j = 0; while( j < nLen && rLine[j] == ' ' && j < nIndentSize ) j++;
		FastDeleteString(rLine, 0, nIdxY, j); 
		return j;
	}
}

INT CCedtDoc::FastMakeCommentLine(CAnalyzedString & rLine, INT nIdxY)
{
	LPCTSTR LCS = m_clsLangSpec.m_szLineComment1;
	INT  lenLCS = strlen(LCS); if( ! lenLCS ) return 0;

	if( rLine.GetLength() ) {
		CString szString = LCS; szString += " ";
		FastInsertString(rLine, 0, nIdxY, szString); return lenLCS+1;
	} else return 0;
}

INT CCedtDoc::FastReleaseCommentLine(CAnalyzedString & rLine, INT nIdxY)
{
	LPCTSTR LCS = m_clsLangSpec.m_szLineComment1; 
	INT  lenLCS = strlen(LCS); if( ! lenLCS ) return 0;

	if( rLine.GetLength() ) {
		CString szString = LCS; szString += " ";
		if( ! strnicmp(rLine, szString, lenLCS+1) ) {
			FastDeleteString(rLine, 0, nIdxY, lenLCS+1); return lenLCS+1;
		} else if( ! strnicmp(rLine, LCS, lenLCS) ) {
			FastDeleteString(rLine, 0, nIdxY, lenLCS); return lenLCS;
		} else return 0;
	} else return 0;
}

INT CCedtDoc::FastConvertTabsToSpaces(CAnalyzedString & rLine, INT nIdxY)
{
	INT nLen = rLine.GetLength(); if( ! nLen ) return 0;
	INT nTabsConverted = 0, nTabSize = CCedtView::m_nTabSize;

	for( INT j = 0; j < nLen; j++ ) {
		if( rLine[j] == '\t' ) {
			INT nSpaces = nTabSize - (j % nTabSize);
			FastDeleteChar(rLine, j, nIdxY);
			FastInsertString(rLine, j, nIdxY, CString(' ', nSpaces));
			nTabsConverted++; nLen += nSpaces - 1;
		}
	}

	return nTabsConverted;
}

INT CCedtDoc::FastConvertSpacesToTabs(CAnalyzedString & rLine, INT nIdxY)
{
	INT nTabsDeleted = FastConvertTabsToSpaces(rLine, nIdxY);

	INT nLen = rLine.GetLength(); if( ! nLen ) return 0;
	INT nTabsConverted = 0, nTabSize = CCedtView::m_nTabSize;

	for( INT j = nLen / nTabSize * nTabSize; j >= nTabSize; j -= nTabSize ) {
		INT i = j; INT nSpaces = 0;
		while( i > j - nTabSize && rLine[i-1] == ' ' ) { i--; nSpaces++; }
		if( nSpaces >= 2 ) {
			FastDeleteString(rLine, i, nIdxY, nSpaces);
			FastInsertChar(rLine, i, nIdxY, '\t');
			nTabsConverted++; nLen += 1 - nSpaces;
		}
	}

	return nTabsConverted;
}

INT CCedtDoc::FastLeadingTabsToSpaces(CAnalyzedString & rLine, INT nIdxY)
{
	INT nLen = rLine.GetLength(); if( ! nLen ) return 0;
	INT nTabsConverted = 0, nTabSize = CCedtView::m_nTabSize;

	for( INT j = 0; j < nLen; j++ ) {
		if( rLine[j] == '\t' ) {
			INT nSpaces = nTabSize - (j % nTabSize);
			FastDeleteChar(rLine, j, nIdxY);
			FastInsertString(rLine, j, nIdxY, CString(' ', nSpaces));
			nTabsConverted++; nLen += nSpaces - 1;
		} else if( rLine[j] != ' ' ) break;
	}

	return nTabsConverted;
}

INT CCedtDoc::FastLeadingSpacesToTabs(CAnalyzedString & rLine, INT nIdxY)
{
	INT nLen = rLine.GetLength(); if( ! nLen ) return 0;
	INT nTabsConverted = 0, nTabSize = CCedtView::m_nTabSize;

	INT j = 0; while( j < nLen && rLine[j] == ' ' ) j++;
	INT nSpaces = (j / nTabSize) * nTabSize;
	nTabsConverted = nSpaces / nTabSize;

	if( nTabsConverted ) {
		FastDeleteString(rLine, 0, nIdxY, nSpaces);
		FastInsertString(rLine, 0, nIdxY, CString('\t', nTabsConverted));
	}

	return nTabsConverted;
}

INT CCedtDoc::FastDeleteLeadingSpaces(CAnalyzedString & rLine, INT nIdxY)
{
	INT nLen = rLine.GetLength(); if( ! nLen ) return 0;

	if( rLine[0] == ' ' || rLine[0] == '\t' ) {
		INT j = 0; while( j < nLen && (rLine[j] == ' ' || rLine[j] == '\t') ) j++;
		FastDeleteString(rLine, 0, nIdxY, j); return j;
	} else return 0;
}

INT CCedtDoc::FastDeleteTrailingSpaces(CAnalyzedString & rLine, INT nIdxY)
{
	INT nLen = rLine.GetLength(); if( ! nLen ) return 0;

	if( rLine[nLen-1] == ' ' || rLine[nLen-1] == '\t' ) {
		INT j = 0; while( j < nLen && (rLine[nLen-j-1] == ' ' || rLine[nLen-j-1] == '\t') ) j++;
		FastDeleteString(rLine, nLen-j, nIdxY, j); return j;
	} else return 0;
}

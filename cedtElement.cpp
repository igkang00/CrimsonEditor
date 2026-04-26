#include "stdafx.h"
#include <fstream.h>
#include <strstrea.h>
#include <ctype.h>
#include "Encode.h"
#include "RegExp.h"
#include "Utility.h"
#include "cedtElement.h"


#define _SWAP_UCHAR(A, B)				{ _uchar_temp = (A); (A) = (B); (B) = _uchar_temp; }
static UCHAR _uchar_temp;

#define _INITIAL_TYPES(types, atype)	{ types[0] = types[1] = types[2] = atype; types[3] = 0x00; }
#define _COMPOSE_TYPES(types)			( MAKELONG( MAKEWORD(types[0], types[1]), MAKEWORD(types[2], types[3]) ) )
#define _EXTRACT_TYPES(types, dword)	{ \
	_lword_temp = LOWORD(dword);		_hword_temp = HIWORD(dword); \
	types[0] = LOBYTE(_lword_temp);		types[1] = HIBYTE(_lword_temp); \
	types[2] = LOBYTE(_hword_temp);		types[3] = HIBYTE(_hword_temp); }
static WORD _lword_temp, _hword_temp;


CString ENCODING_TYPE_DESCRIPTION_FULL[] = {
	/* ENCODING_TYPE_UNKNOWN */			"Unknown Encoding",
	/* ENCODING_TYPE_ASCII */			"ASCII Encoding",
	/* ENCODING_TYPE_UNICODE_LE */		"Unicode Encoding (Little Endian)",
	/* ENCODING_TYPE_UNICODE_BE */		"Unicode Encoding (Big Endian)",
	/* ENCODING_TYPE_UTF8_WBOM */		"UTF-8 Encoding (with BOM)",
	/* ENDOCING_TYPE_UTF8_XBOM */		"UTF-8 Encoding (w/o BOM)",
};

CString ENCODING_TYPE_DESCRIPTION_SHORT[] = {
	/* ENCODING_TYPE_UNKNOWN */			"N.A.",
	/* ENCODING_TYPE_ASCII */			"ASCII",
	/* ENCODING_TYPE_UNICODE_LE */		"Unicode",
	/* ENCODING_TYPE_UNICODE_BE */		"Unicode",
	/* ENCODING_TYPE_UTF8_WBOM */		"UTF-8",
	/* ENCODING_TYPE_UTF8_XBOM */		"UTF-8",
};

CString FILE_FORMAT_DESCRIPTION_FULL[] = {
	/* FILE_FORMAT_UNKNOWN */			"Unknown Format",
	/* FILE_FORMAT_DOS     */			"DOS Format",
	/* FILE_FORMAT_UNIX    */			"UNIX Format",
	/* FILE_FORMAT_MAC     */			"MAC Format",
};

CString FILE_FORMAT_DESCRIPTION_SHORT[] = {
	/* FILE_FORMAT_UNKNOWN */			"N.A.",
	/* FILE_FORMAT_DOS     */			"DOS",
	/* FILE_FORMAT_UNIX    */			"UNIX",
	/* FILE_FORMAT_MAC     */			"MAC",
};


static UCHAR _GetRangeType1(LPCTSTR lpszRange)
{
	if     ( ! _strnicmp(lpszRange, "GLOBAL", 6) ) return RT_GLOBAL;
	else if( ! _strnicmp(lpszRange, "RANGE1", 6) ) return RT_RANGE1;
	else if( ! _strnicmp(lpszRange, "RANGE2", 6) ) return RT_RANGE2;
	else if( ! _strnicmp(lpszRange, "R1||R2", 6) ) return RT_R1ORR2;
	return RT_GLOBAL;
}

static UCHAR _GetRangeType2(LPCTSTR lpszRange)
{
	if     ( ! _strnicmp(lpszRange, "GLOBAL", 6) ) return RT_GLOBAL;
	else if( ! _strnicmp(lpszRange, "RANGE1", 6) ) return RT_RANGE1;
	else if( ! _strnicmp(lpszRange, "RANGE2", 6) ) return RT_RANGE2;
	return RT_GLOBAL;
}


BOOL DetectEncodingTypeAndFileFormat(LPCTSTR lpszPathName, UINT & nEncodingType, UINT & nFileFormat)
{
	try {
		CFile file(lpszPathName, CFile::modeRead | CFile::typeBinary);
		UCHAR szBuffer[4096]; INT nCount = file.Read( szBuffer, 4096 );
		file.Close();

		DetectEncodingType(szBuffer, nCount, nEncodingType);
		DetectFileFormat(szBuffer, nCount, nFileFormat);
		TRACE2("DetectEncodingTypeAndFileFormat: %s, %s\n", ENCODING_TYPE_DESCRIPTION_FULL[nEncodingType], FILE_FORMAT_DESCRIPTION_FULL[nFileFormat]);

		return TRUE;

	} catch( CException * ex ) {
		nEncodingType = ENCODING_TYPE_UNKNOWN;
		nFileFormat = FILE_FORMAT_UNKNOWN;

		ex->Delete(); 
		return FALSE;
	}
}

BOOL DetectEncodingType(LPVOID lpContents, INT nLength, UINT & nEncodingType)
{
	LPBYTE lpBuffer = (LPBYTE)lpContents;

	if( nLength >= 2 && lpBuffer[0] == 0xFF && lpBuffer[1] == 0xFE ) { nEncodingType = ENCODING_TYPE_UNICODE_LE; return TRUE; }
	if( nLength >= 2 && lpBuffer[0] == 0xFE && lpBuffer[1] == 0xFF ) { nEncodingType = ENCODING_TYPE_UNICODE_BE; return TRUE; }
	if( nLength >= 3 && lpBuffer[0] == 0xEF && lpBuffer[1] == 0xBB && lpBuffer[2] == 0xBF ) { nEncodingType = ENCODING_TYPE_UTF8_WBOM; return TRUE; }

	nEncodingType = ENCODING_TYPE_UNKNOWN; // default encoding type
	return FALSE;
}

BOOL DetectFileFormat(LPVOID lpContents, INT nLength, UINT & nFileFormat)
{
	LPBYTE lpBuffer = (LPBYTE)lpContents;
	BOOL bHasCR = FALSE, bHasLF = FALSE;

	for( INT i = 0; i < nLength; i++ ) {
		if( lpBuffer[i] == '\r' ) bHasCR = TRUE;
		if( lpBuffer[i] == '\n' ) bHasLF = TRUE;
		if( bHasCR && bHasLF ) { nFileFormat = FILE_FORMAT_DOS; return TRUE; }
	}

	if( ! bHasCR && bHasLF ) { nFileFormat = FILE_FORMAT_UNIX; return TRUE; }
	if( bHasCR && ! bHasLF ) { nFileFormat = FILE_FORMAT_MAC; return TRUE; }

	nFileFormat = FILE_FORMAT_UNKNOWN; // default file format
	return FALSE;
}


// CLangSpec
void CLangSpec::ResetContents() 
{
	m_bCaseSensitive[0] = m_bCaseSensitive[1] = m_bCaseSensitive[2] = TRUE; 

	m_bVariableHighlightInString = FALSE;
	m_szDelimiters = "(){}[]<>+-*/%=\"'~!@#$^&|\\?:;,."; // Omited '_' from default delimiters 2004.08.08
	m_szHexaDecimalMark = m_szKeywordPrefix = m_szVariablePrefix = "";
	m_szVariableOptionallyEnclosedBy = m_szSpecialVariableChars = "";
	m_chVariableQuotation = 0x00;

	m_bMultiLineStringConstant = FALSE;
	m_chEscapeChar = m_chMultiLineQuotationMark = 0x00;
	m_chQuotationMark1 = m_chQuotationMark2 = m_chQuotationMark3 = 0x00;
	m_ucMultiLineQuotationMarkRange = RT_GLOBAL;
	m_ucQuotationMark1Range = m_ucQuotationMark2Range = m_ucQuotationMark3Range = RT_GLOBAL;
	m_szHereDocument = "";

	m_szLineComment1OnFirstPosition = m_szLineComment2OnFirstPosition = "";
	m_szLineComment1 = m_szLineComment2 = "";
	m_ucLineComment1Range = m_ucLineComment2Range = RT_GLOBAL;

	m_szBlockComment1On = m_szBlockComment1Off = "";
	m_szBlockComment2On = m_szBlockComment2Off = "";
	m_ucBlockComment1Range = m_ucBlockComment2Range = RT_GLOBAL;

	m_szShadowOn = m_szShadowOff = "";
	m_szHighlightOn = m_szHighlightOff = "";
	m_szRange1Beg = m_szRange1End = "";
	m_szRange2Beg = m_szRange2End = "";

	m_chIndentationOn = m_chIndentationOff = 0x00;
	m_szPairs1 = m_szPairs2 = m_szPairs3 = "";
}

BOOL CLangSpec::FileLoad(LPCTSTR lpszPathName) 
{
	TCHAR szLine[4096];
	ResetContents(); // clear previous settings

	ifstream fin(lpszPathName, ios::in | ios::nocreate);
	if( ! fin.is_open() ) return FALSE;

	while( fin.good() ) {
		fin.getline(szLine, 4096);
		if( szLine[0] != '$' ) continue;

		TCHAR * ptr1 = strtok(szLine, "=");
		TCHAR * ptr2 = strtok(NULL, "\n");

		if     ( ptr2 && ! _stricmp(ptr1, "$CASESENSITIVE") ) { m_bCaseSensitive[0] = m_bCaseSensitive[1] = m_bCaseSensitive[2] = (ptr2[0] == 'Y' || ptr2[0] == 'y') ? TRUE : FALSE; }
		else if( ptr2 && ! _stricmp(ptr1, "$CASESENSITIVEINGLOBAL") ) { m_bCaseSensitive[0] = (ptr2[0] == 'Y' || ptr2[0] == 'y') ? TRUE : FALSE; }
		else if( ptr2 && ! _stricmp(ptr1, "$CASESENSITIVEINRANGE1") ) { m_bCaseSensitive[1] = (ptr2[0] == 'Y' || ptr2[0] == 'y') ? TRUE : FALSE; }
		else if( ptr2 && ! _stricmp(ptr1, "$CASESENSITIVEINRANGE2") ) { m_bCaseSensitive[2] = (ptr2[0] == 'Y' || ptr2[0] == 'y') ? TRUE : FALSE; }

		else if( ptr2 && ! _stricmp(ptr1, "$VARIABLEHIGHLIGHTINSTRING") ) { m_bVariableHighlightInString = (ptr2[0] == 'Y' || ptr2[0] == 'y') ? TRUE : FALSE; }
		else if( ptr2 && ! _stricmp(ptr1, "$DELIMITERS") ) { m_szDelimiters = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$HEXADECIMALMARK") ) { m_szHexaDecimalMark = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$PREFIX") ) { m_szKeywordPrefix = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$KEYWORDPREFIX") ) { m_szKeywordPrefix = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$VARIABLEPREFIX") ) { m_szVariablePrefix = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$VARIABLEENCLOSEDBY") ) { m_szVariableOptionallyEnclosedBy = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$VARIABLEOPTIONALLYENCLOSEDBY") ) { m_szVariableOptionallyEnclosedBy = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$SPECIALVARIABLECHARS") ) { m_szSpecialVariableChars = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$VARIABLEQUOTATION") ) { m_chVariableQuotation = ptr2[0]; }

		else if( ptr2 && ! _stricmp(ptr1, "$MULTILINESTRINGCONSTANT") ) { m_bMultiLineStringConstant = (ptr2[0] == 'Y' || ptr2[0] == 'y') ? TRUE : FALSE; }
		else if( ptr2 && ! _stricmp(ptr1, "$ESCAPECHAR") ) { m_chEscapeChar = ptr2[0]; }
		else if( ptr2 && ! _stricmp(ptr1, "$MULTILINEQUOTATIONMARK") ) { m_chMultiLineQuotationMark = ptr2[0]; }
		else if( ptr2 && ! _stricmp(ptr1, "$QUOTATIONMARK1") ) { m_chQuotationMark1 = ptr2[0]; }
		else if( ptr2 && ! _stricmp(ptr1, "$QUOTATIONMARK2") ) { m_chQuotationMark2 = ptr2[0]; }
		else if( ptr2 && ! _stricmp(ptr1, "$QUOTATIONMARK3") ) { m_chQuotationMark3 = ptr2[0]; }
		else if( ptr2 && ! _stricmp(ptr1, "$QUOTATIONMARKRANGE") ) { m_ucMultiLineQuotationMarkRange = m_ucQuotationMark1Range = m_ucQuotationMark2Range = m_ucQuotationMark3Range = _GetRangeType1(ptr2); }
		else if( ptr2 && ! _stricmp(ptr1, "$MULTILINEQUOTATIONMARKRANGE") ) { m_ucMultiLineQuotationMarkRange = _GetRangeType1(ptr2); }
		else if( ptr2 && ! _stricmp(ptr1, "$QUOTATIONMARK1RANGE") ) { m_ucQuotationMark1Range = _GetRangeType1(ptr2); }
		else if( ptr2 && ! _stricmp(ptr1, "$QUOTATIONMARK2RANGE") ) { m_ucQuotationMark2Range = _GetRangeType1(ptr2); }
		else if( ptr2 && ! _stricmp(ptr1, "$QUOTATIONMARK3RANGE") ) { m_ucQuotationMark3Range = _GetRangeType1(ptr2); }
		else if( ptr2 && ! _stricmp(ptr1, "$HEREDOCUMENT") ) { m_szHereDocument = ptr2; }

		else if( ptr2 && ! _stricmp(ptr1, "$LINECOMMENTONFIRSTPOSITION" ) ) { m_szLineComment1OnFirstPosition = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$LINECOMMENT1ONFIRSTPOSITION") ) { m_szLineComment1OnFirstPosition = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$LINECOMMENT2ONFIRSTPOSITION") ) { m_szLineComment2OnFirstPosition = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$LINECOMMENT" ) ) { m_szLineComment1 = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$LINECOMMENT1") ) { m_szLineComment1 = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$LINECOMMENT2") ) { m_szLineComment2 = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$LINECOMMENTRANGE"  ) ) { m_ucLineComment1Range = m_ucLineComment2Range = _GetRangeType1(ptr2); }
		else if( ptr2 && ! _stricmp(ptr1, "$LINECOMMENT1RANGE"  ) ) { m_ucLineComment1Range = _GetRangeType1(ptr2); }
		else if( ptr2 && ! _stricmp(ptr1, "$LINECOMMENT2RANGE"  ) ) { m_ucLineComment2Range = _GetRangeType1(ptr2); }

		else if( ptr2 && ! _stricmp(ptr1, "$BLOCKCOMMENTON"  ) ) { m_szBlockComment1On  = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$BLOCKCOMMENTOFF" ) ) { m_szBlockComment1Off = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$BLOCKCOMMENT1ON" ) ) { m_szBlockComment1On  = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$BLOCKCOMMENT1OFF") ) { m_szBlockComment1Off = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$BLOCKCOMMENT2ON" ) ) { m_szBlockComment2On  = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$BLOCKCOMMENT2OFF") ) { m_szBlockComment2Off = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$BLOCKCOMMENTRANGE" ) ) { m_ucBlockComment1Range = m_ucBlockComment2Range = _GetRangeType1(ptr2); }
		else if( ptr2 && ! _stricmp(ptr1, "$BLOCKCOMMENT1RANGE" ) ) { m_ucBlockComment1Range = _GetRangeType1(ptr2); }
		else if( ptr2 && ! _stricmp(ptr1, "$BLOCKCOMMENT2RANGE" ) ) { m_ucBlockComment2Range = _GetRangeType1(ptr2); }

		else if( ptr2 && ! _stricmp(ptr1, "$SHADOWON" ) ) { m_szShadowOn  = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$SHADOWOFF") ) { m_szShadowOff = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$HIGHLIGHTON" ) ) { m_szHighlightOn  = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$HIGHLIGHTOFF") ) { m_szHighlightOff = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$RANGE1BEG") ) { m_szRange1Beg = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$RANGE1END") ) { m_szRange1End = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$RANGE2BEG") ) { m_szRange2Beg = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$RANGE2END") ) { m_szRange2End = ptr2; }

		else if( ptr2 && ! _stricmp(ptr1, "$INDENTATIONON" ) ) { m_chIndentationOn  = ptr2[0]; }
		else if( ptr2 && ! _stricmp(ptr1, "$INDENTATIONOFF") ) { m_chIndentationOff = ptr2[0]; }
		else if( ptr2 && ! _stricmp(ptr1, "$PAIRS1") ) { m_szPairs1 = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$PAIRS2") ) { m_szPairs2 = ptr2; }
		else if( ptr2 && ! _stricmp(ptr1, "$PAIRS3") ) { m_szPairs3 = ptr2; }
	}

	fin.close();
	return TRUE;
}


// CKeywords
BOOL CKeywords::FileLoad(LPCTSTR lpszPathName, BOOL bCaseSensitive[])
{
	TCHAR szLine[MAX_STRING_SIZE+1], szWord[MAX_WORD_LENGTH+1], szBuffer[MAX_WORD_LENGTH+3];
	UCHAR ucType = 0x00, ucRange = RT_GLOBAL; BOOL bIgnoreCase, bNoEmbolden; UCHAR ucTypes[4]; DWORD dwValue;

	RemoveAll(); // clear hash table first

	ifstream fin(lpszPathName, ios::in | ios::nocreate);
	if( ! fin.is_open() ) return FALSE;

	while( fin.good() ) {
		fin.getline(szLine, MAX_STRING_SIZE);
		if( szLine[0] == '#' ) continue;

		if( ! _strnicmp(szLine, "[-COMMENT", 9) ) {
			ucType  = 0x00;
			ucRange = RT_GLOBAL;

		} else if( ! _strnicmp(szLine, "[#COMMENT", 9) ) {
			ucType  = 0x00;
			ucRange = RT_GLOBAL;

		} else if( ! _strnicmp(szLine, "[KEYWORDS", 9) ) {
			TCHAR * ptr1 = strtok(szLine, ":");
			TCHAR * ptr2 = strtok(NULL, "\n");

			ucType  = WT_KEYWORD0 + (ptr1[9] - '0');
			ucRange = _GetRangeType2(ptr2);

			bIgnoreCase = ! bCaseSensitive[ucRange];
			bNoEmbolden = FALSE; _strupr(ptr2);

			if( strstr(ptr2, "IGNORECASE") ) bIgnoreCase = TRUE;
			if( strstr(ptr2, "NOEMBOLDEN") ) bNoEmbolden = TRUE;

		} else if( ucType ) {
			istrstream sin(szLine);
			while( sin.good() ) {
				sin.eatwhite(); if( ! sin.good() ) break;
				sin >> szWord;  if( szWord[0] == '\0' ) break;

				if( bIgnoreCase ) { _strlwr(szWord); sprintf(szBuffer, "I:%s", szWord); }
				else sprintf(szBuffer, "C:%s", szWord);

				if( Lookup(szBuffer, dwValue) ) {
					_EXTRACT_TYPES(ucTypes, dwValue); if( ucTypes[ucRange] != WT_IDENTIFIER ) continue;
					ucTypes[ucRange] = ucType; if( ! bNoEmbolden ) ucTypes[3] |= 0x01 << ucRange;
					dwValue = _COMPOSE_TYPES(ucTypes); SetAt(szBuffer, dwValue);
				} else {
					_INITIAL_TYPES(ucTypes, WT_IDENTIFIER);
					ucTypes[ucRange] = ucType; if( ! bNoEmbolden ) ucTypes[3] |= 0x01 << ucRange;
					dwValue = _COMPOSE_TYPES(ucTypes); SetAt(szBuffer, dwValue);
				}
			}
		}
	}

	fin.close();
	return TRUE;
}

BOOL CKeywords::LookupTable(UCHAR ucType[], UCHAR & ucRange, LPCTSTR lpszWord, SHORT siLength)
{
	static TCHAR szWord[MAX_WORD_LENGTH+1], szBuffer[MAX_WORD_LENGTH+3];
	if( siLength > MAX_WORD_LENGTH ) return FALSE;

	BOOL bFoundC, bFoundI; DWORD dwValue; 
	UCHAR ucTypeC[4], ucTypeI[4];

	_INITIAL_TYPES(ucTypeC, WT_IDENTIFIER);
	_INITIAL_TYPES(ucTypeI, WT_IDENTIFIER);

	strncpy(szWord, lpszWord, siLength); szWord[siLength] = '\0'; sprintf(szBuffer, "C:%s", szWord);
	if( bFoundC = Lookup(szBuffer, dwValue) ) _EXTRACT_TYPES(ucTypeC, dwValue);

	_strlwr(szWord); sprintf(szBuffer, "I:%s", szWord);
	if( bFoundI = Lookup(szBuffer, dwValue) ) _EXTRACT_TYPES(ucTypeI, dwValue);

	ucType[0] = (ucTypeC[0] != WT_IDENTIFIER) ? ucTypeC[0] : ucTypeI[0];
	ucType[1] = (ucTypeC[1] != WT_IDENTIFIER) ? ucTypeC[1] : ucTypeI[1];
	ucType[2] = (ucTypeC[2] != WT_IDENTIFIER) ? ucTypeC[2] : ucTypeI[2];
	ucRange   = (ucTypeC[3] |  ucTypeI[3]); // bitwise or

	return (bFoundC || bFoundI);
}


// CDictionary
BOOL CDictionary::FileLoad(LPCTSTR lpszPathName, CALLBACK_FUNCTION fcnCallback)
{
	TCHAR szWord[MAX_WORD_LENGTH+1]; UCHAR ucValue; UINT nCount = 0;

	ifstream fin(lpszPathName, ios::in | ios::nocreate);
	if( ! fin.is_open() ) return FALSE;

	while( fin.good() ) {
		fin.eatwhite(); if( fin.eof() ) break;
		fin >> szWord; _strlwr(szWord);

		if( ! Lookup( szWord, ucValue ) ) {
			SetAt( szWord, ucValue = WT_IDENTIFIER );
			nCount++;
		}

		// call a callback function if exist
		if( ! (nCount % 100) && fcnCallback ) fcnCallback( fin.tellg() );
	}

	fin.close();
	return TRUE;
}

BOOL CDictionary::LookupTable(LPCTSTR lpszWord, SHORT siLength)
{
	static TCHAR szBuffer[MAX_WORD_LENGTH+1];
	if( siLength > MAX_WORD_LENGTH ) return FALSE;

	strncpy(szBuffer, lpszWord, siLength); szBuffer[siLength] = '\0';
	_strlwr(szBuffer);

	BOOL bFound; UCHAR ucValue = 0x00;
	bFound = Lookup(szBuffer, ucValue);

	return bFound;
}

BOOL CDictionary::AddWord(LPCTSTR lpszWord)
{
	TCHAR szWord[MAX_WORD_LENGTH+1]; UCHAR ucValue;
	if( strlen(lpszWord) > MAX_WORD_LENGTH ) return FALSE;
	
	strcpy(szWord, lpszWord); _strlwr(szWord);
	if( ! Lookup( szWord, ucValue ) ) {
		SetAt( szWord, ucValue = WT_IDENTIFIER );
	} else return FALSE;
	
	return TRUE;
}


// CAnalyzedString
CAnalyzedString & CAnalyzedString::operator=(const CAnalyzedString & stringSrc) {
	CString::operator=(stringSrc);
	delete [] m_pWord; m_pWord = NULL; 
	m_siWordCount = 0;
//	m_usLineInfo = m_usLineFlag = 0x00;
	return * this;
}


// CFormatedString
CFormatedString & CFormatedString::operator=(const CFormatedString & stringSrc) {
	m_pString = stringSrc.m_pString;
	delete [] m_pWord; m_pWord = NULL; 
	m_siWordCount = m_siSplitIndex = 0; m_bLineBreak = FALSE;
	m_usLineInfo = m_usLineFlag = 0x00;
	m_szHereDocumentTerminator = "";
	return * this;
}


// CMemText
BOOL CMemText::FileLoad(LPCTSTR lpszPathName)
{
	try {
		CFile file(lpszPathName, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone);
		RemoveAll(); AddTail(""); // initialize contents

		CHAR szBuffer[FILE_READ_BUFFER_SIZE+1];
		int i, nCount, nTotal = 0; BOOL bDelimFount = FALSE;

		while( nCount = file.Read( szBuffer, FILE_READ_BUFFER_SIZE ) ) { // read file contents
			for( bDelimFount = FALSE, i = 0; i <= nCount-1; i++ ) {
				if( szBuffer[i] == '\n' ) { bDelimFount = TRUE; i++; break; }
			}

			nCount = i; nTotal += nCount; 
			szBuffer[nCount] = 0x00;

			if( nCount >= 1 && szBuffer[nCount-1] == '\n' ) { szBuffer[nCount-1] = 0x00; nCount--; }
			if( nCount >= 1 && szBuffer[nCount-1] == '\r' ) { szBuffer[nCount-1] = 0x00; nCount--; }

			GetTail() += szBuffer;
			if( bDelimFount ) AddTail("");

			file.Seek(nTotal, CFile::begin);
		}

		file.Close();
	} catch( CException * ex ) {
		ex->ReportError( MB_OK | MB_ICONSTOP );
		ex->Delete(); return FALSE;
	}

	return TRUE;
}

BOOL CMemText::FileSave(LPCTSTR lpszPathName)
{
	try {
		CFile file(lpszPathName, CFile::modeReadWrite | CFile::typeBinary | CFile::shareDenyWrite);
		POSITION pos = GetHeadPosition();
		while( pos ) {
			CString & rString = GetNext(pos);
			INT nLength = rString.GetLength();
			file.Write( rString, nLength );
			if( pos ) file.Write( "\r\n", 2 );
		}
		file.Close();
	} catch( CException * ex ) {
		ex->ReportError( MB_OK | MB_ICONSTOP );
		ex->Delete(); return FALSE;
	}

	return TRUE;
}

void CMemText::AppendText(CMemText & rBlock)
{
	POSITION posBeg = rBlock.GetHeadPosition();
	if( ! posBeg ) { return; }
	POSITION posEnd = GetTailPosition();
	if( ! posEnd ) { AddTail( & rBlock ); return; }
	GetAt(posEnd) += rBlock.GetNext(posBeg);
	for(INT i = 1; i < rBlock.GetCount(); i++) AddTail( rBlock.GetNext(posBeg) );
}

CMemText & CMemText::operator=(const CMemText & rBlock) {
	RemoveAll();
	POSITION pos = rBlock.GetHeadPosition();
	while( pos ) AddTail( rBlock.GetNext(pos) );
	return * this;
}

void CMemText::MakeUpperCase()
{
	POSITION pos = GetHeadPosition();
	while( pos ) {
		CString & rString = GetNext( pos );
		rString.MakeUpper();
	}
}

void CMemText::MakeLowerCase()
{
	POSITION pos = GetHeadPosition();
	while( pos ) {
		CString & rString = GetNext( pos );
		rString.MakeLower();
	}
}

void CMemText::MakeInvertCase()
{
	POSITION pos = GetHeadPosition();
	while( pos ) {
		CString & rString = GetNext( pos );
		::MakeInvertCase( rString );
	}
}

void CMemText::MakeCapitalize()
{
	POSITION pos = GetHeadPosition();
	while( pos ) {
		CString & rString = GetNext( pos );
		::MakeCapitalize( rString );
	}
}

INT CMemText::GetMaxLength()
{
	INT nLen, nMaxLen = 0;
	POSITION pos = GetHeadPosition();
	while( pos ) {
		CString & rString = GetNext( pos );
		nLen = rString.GetLength();
		if( nLen > nMaxLen ) nMaxLen = nLen;
	}
	return nMaxLen;
}

void CMemText::MakeEqualLength()
{
	INT nLen, nMaxLen = GetMaxLength();

	POSITION pos = GetHeadPosition();
	while( pos ) {
		CString & rString = GetNext( pos );
		nLen = rString.GetLength();
		if( nLen < nMaxLen ) rString += CString(' ', nMaxLen - nLen);
	}
}

INT CMemText::MemorySize()
{
	INT size = 0;
	POSITION pos = GetHeadPosition();
	while( pos ) {
		CString & rString = GetNext( pos );
		size += rString.GetLength() + 2;
	}
	return size;
}

void CMemText::MemoryLoad(CHAR * pMem, INT size)
{
	RemoveAll(); CHAR * beg, * fwd = pMem;
	while( fwd - pMem < size ) {
		beg = fwd;
		while( fwd - pMem < size && * fwd != '\r' && * fwd != '\n' && * fwd != '\0' ) fwd++;
		AddTail( CString(beg, fwd-beg) );
		if( * fwd != '\0' ) { if(* fwd == '\r') fwd++; if(* fwd == '\n') fwd++; }
		else break;
	}
}

void CMemText::MemorySave(CHAR * pMem, INT size)
{
	CHAR * tmp = pMem; POSITION pos = GetHeadPosition();
	while( pos ) {
		CString & rString = GetNext(pos);
		INT nLength = rString.GetLength();
		memcpy(tmp, rString, nLength); tmp += nLength;
		if( pos ) { * tmp++ = '\r'; * tmp++ = '\n'; }
		else { * tmp++ = '\0'; * tmp++ = '\0'; }
	}
}


// CAnalyzedTextFile

BOOL CAnalyzedText::FileLoad(LPCTSTR lpszPathName, INT nEncodingType, INT nFileFormat)
{
	INT chDelim = '\n', chKill = '\r'; // FILE_FORMAT_DOS & FILE_FORMAT_UNIX
	if( nFileFormat == FILE_FORMAT_MAC ) { chDelim = '\r'; chKill = '\0'; }
	
	try {
		CFile file(lpszPathName, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone);
		RemoveAll(); AddTail(""); // initialize contents

		CHAR szBuffer[FILE_READ_BUFFER_SIZE + 1]; UCHAR szWideBuffer[2 * FILE_READ_BUFFER_SIZE + 2]; 
		INT i, nCount, nTotal = 0; BOOL bDelimFound = FALSE;

		if( nEncodingType == ENCODING_TYPE_UNICODE_LE ) {
			// extract byte-order mark
			nCount = file.Read( szWideBuffer, 2 ); 

			if( nCount >= 2 && szWideBuffer[0] == 0xFF && szWideBuffer[1] == 0xFE ) nTotal = 2;
			file.Seek(nTotal, CFile::begin);

			while( ( nCount = file.Read( szWideBuffer, FILE_READ_BUFFER_SIZE ) ) >= 2 ) { // read file contents
				for( bDelimFound = FALSE, i = 0; i <= nCount-2; i += 2 ) { 
					if( szWideBuffer[i] == chDelim && szWideBuffer[i+1] == 0x00 ) { bDelimFound = TRUE; i += 2; break; }
				}

				nCount = i; nTotal += nCount; 
				szWideBuffer[nCount] = szWideBuffer[nCount+1] = 0x00;

				if( nCount >= 2 && szWideBuffer[nCount-2] == chDelim && szWideBuffer[nCount-1] == 0x00 ) { szWideBuffer[nCount-2] = szWideBuffer[nCount-1] = 0x00; nCount -= 2; }
				if( nCount >= 2 && szWideBuffer[nCount-2] == chKill  && szWideBuffer[nCount-1] == 0x00 ) { szWideBuffer[nCount-2] = szWideBuffer[nCount-1] = 0x00; nCount -= 2; }

				nCount = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)szWideBuffer, -1, szBuffer, FILE_READ_BUFFER_SIZE + 1, NULL, NULL);
				nCount = lstrlen( szBuffer );

				GetTail() += szBuffer;
				if( bDelimFound ) AddTail("");

				file.Seek(nTotal, CFile::begin);
			}
		} else if( nEncodingType == ENCODING_TYPE_UNICODE_BE ) {
			// extract byte-order mark
			nCount = file.Read( szWideBuffer, 2 ); 

			if( nCount >= 2 && szWideBuffer[0] == 0xFE && szWideBuffer[1] == 0xFF ) nTotal = 2;
			file.Seek(nTotal, CFile::begin);

			while( ( nCount = file.Read( szWideBuffer, FILE_READ_BUFFER_SIZE ) ) >= 2 ) { // read file contents
				for( bDelimFound = FALSE, i = 0; i <= nCount-2; i += 2 ) { 
					if( szWideBuffer[i] == 0x00 && szWideBuffer[i+1] == chDelim ) { bDelimFound = TRUE; i += 2; break; }
				}

				nCount = i; nTotal += nCount; 
				szWideBuffer[nCount] = szWideBuffer[nCount+1] = 0x00;

				if( nCount >= 2 && szWideBuffer[nCount-2] == 0x00 && szWideBuffer[nCount-1] == chDelim ) { szWideBuffer[nCount-2] = szWideBuffer[nCount-1] = 0x00; nCount -= 2; }
				if( nCount >= 2 && szWideBuffer[nCount-2] == 0x00 && szWideBuffer[nCount-1] == chKill  ) { szWideBuffer[nCount-2] = szWideBuffer[nCount-1] = 0x00; nCount -= 2; }

				for( i = 0; i < nCount; i += 2 ) _SWAP_UCHAR( szWideBuffer[i], szWideBuffer[i+1] );
				nCount = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)szWideBuffer, -1, szBuffer, FILE_READ_BUFFER_SIZE + 1, NULL, NULL);
				nCount = lstrlen( szBuffer );

				GetTail() += szBuffer;
				if( bDelimFound ) AddTail("");

				file.Seek(nTotal, CFile::begin);
			}
		} else if( nEncodingType == ENCODING_TYPE_UTF8_WBOM || nEncodingType == ENCODING_TYPE_UTF8_XBOM ) {
			// extract byte-order mark
			nCount = file.Read( szWideBuffer, 4 ); 

			if( nCount >= 3 && szWideBuffer[0] == 0xEF && szWideBuffer[1] == 0xBB && szWideBuffer[2] == 0xBF ) nTotal = 3;
			file.Seek(nTotal, CFile::begin);

			while( nCount = file.Read( szBuffer, FILE_READ_BUFFER_SIZE ) ) { // read file contents
				for( bDelimFound = FALSE, i = 0; i <= nCount-1; i++ ) { 
					if( szBuffer[i] == chDelim ) { bDelimFound = TRUE; i++; break; } 
				}

				nCount = i; nTotal += nCount; 
				szBuffer[nCount] = 0x00;

				if( nCount >= 1 && szBuffer[nCount-1] == chDelim ) { szBuffer[nCount-1] = 0x00; nCount--; }
				if( nCount >= 1 && szBuffer[nCount-1] == chKill  ) { szBuffer[nCount-1] = 0x00; nCount--; }

				nCount = MultiByteToWideChar(CP_UTF8, 0, szBuffer, -1, (LPWSTR)szWideBuffer, FILE_READ_BUFFER_SIZE + 1);
				nCount = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)szWideBuffer, -1, szBuffer, FILE_READ_BUFFER_SIZE + 1, NULL, NULL);

				GetTail() += szBuffer; 
				if( bDelimFound ) AddTail("");

				file.Seek(nTotal, CFile::begin);
			}
		} else { /* nEncodingType == ENCODING_TYPE_ASCII */
			while( nCount = file.Read( szBuffer, FILE_READ_BUFFER_SIZE ) ) { // read file contents
				for( bDelimFound = FALSE, i = 0; i <= nCount-1; i++ ) { 
					if( szBuffer[i] == chDelim ) { bDelimFound = TRUE; i++; break; } 
				}

				nCount = i; nTotal += nCount; 
				szBuffer[nCount] = 0x00;

				if( nCount >= 1 && szBuffer[nCount-1] == chDelim ) { szBuffer[nCount-1] = 0x00; nCount--; }
				if( nCount >= 1 && szBuffer[nCount-1] == chKill  ) { szBuffer[nCount-1] = 0x00; nCount--; }

				GetTail() += szBuffer;
				if( bDelimFound ) AddTail("");

				file.Seek(nTotal, CFile::begin);
			}
		}

		file.Close();

	} catch( CException * ex ) {
		ex->ReportError( MB_OK | MB_ICONSTOP );
		ex->Delete(); return FALSE;
	}

	return TRUE;
}

BOOL CAnalyzedText::FileSave(LPCTSTR lpszPathName, INT nEncodingType, INT nFileFormat)
{
	CHAR szDelim[2]; lstrcpy(szDelim, "\r\n"); INT nDelimSize = 2; // FILE_FORMAT_DOS
	if( nFileFormat == FILE_FORMAT_UNIX ) { lstrcpy(szDelim, "\n"); nDelimSize = 1; }
	else if( nFileFormat == FILE_FORMAT_MAC ) { lstrcpy(szDelim, "\r"); nDelimSize = 1; }
	
	try {
		CFile file(lpszPathName, CFile::modeReadWrite | CFile::modeCreate | CFile::shareExclusive);
		POSITION pos = GetHeadPosition();

		INT nCount, nBufferSize = 0; CHAR * pBuffer = NULL;
		UCHAR szWideDelim[4], * pWideBuffer = NULL;

		if( nEncodingType == ENCODING_TYPE_UNICODE_LE ) {
			// write byte-order mark
			file.Write("\xFF\xFE", 2); 

			szWideDelim[0] = szDelim[0];	szWideDelim[1] = 0x00; 
			szWideDelim[2] = szDelim[1];	szWideDelim[3] = 0x00; 

			while( pos ) {
				CAnalyzedString & rLine = GetNext(pos);
				nCount = rLine.GetLength(); if( nCount < FILE_WRITE_BUFFER_SIZE ) nCount = FILE_WRITE_BUFFER_SIZE;

				if( nBufferSize < nCount ) {
					nBufferSize = nCount;
					delete [] pWideBuffer;	pWideBuffer = new UCHAR[2 * (nBufferSize + 1)];
				}

				nCount = MultiByteToWideChar(CP_ACP, 0, rLine, -1, (LPWSTR)pWideBuffer, nBufferSize + 1);
				INT nLength = wcslen( (LPCWSTR)pWideBuffer );

				file.Write( pWideBuffer, 2 * nLength );
				if( pos ) file.Write( szWideDelim, 2 * nDelimSize );
			}
		} else if( nEncodingType == ENCODING_TYPE_UNICODE_BE ) {
			// write byte-order mark
			file.Write("\xFE\xFF", 2); 

			szWideDelim[0] = 0x00; 	szWideDelim[1] = szDelim[0];
			szWideDelim[2] = 0x00; 	szWideDelim[3] = szDelim[1];

			while( pos ) {
				CAnalyzedString & rLine = GetNext(pos);
				nCount = rLine.GetLength(); if( nCount < FILE_WRITE_BUFFER_SIZE ) nCount = FILE_WRITE_BUFFER_SIZE;

				if( nBufferSize < nCount ) {
					nBufferSize = nCount;
					delete [] pWideBuffer;	pWideBuffer = new UCHAR[2 * (nBufferSize + 1)];
				}

				nCount = MultiByteToWideChar(CP_ACP, 0, rLine, -1, (LPWSTR)pWideBuffer, nBufferSize + 1);
				INT nLength = wcslen( (LPCWSTR)pWideBuffer );
				for( INT i = 0; i < nLength; i++ ) _SWAP_UCHAR( pWideBuffer[2*i], pWideBuffer[2*i+1] );

				file.Write( pWideBuffer, 2 * nLength );
				if( pos ) file.Write( szWideDelim, 2 * nDelimSize );
			}
		} else if( nEncodingType == ENCODING_TYPE_UTF8_WBOM || nEncodingType == ENCODING_TYPE_UTF8_XBOM ) {
			// write byte-order mark when it is not ENCODING_TYPE_UTF8_XBOM
			if( nEncodingType != ENCODING_TYPE_UTF8_XBOM ) file.Write("\xEF\xBB\xBF", 3); 

			while( pos ) {
				CAnalyzedString & rLine = GetNext(pos);
				nCount = rLine.GetLength(); if( nCount < FILE_WRITE_BUFFER_SIZE ) nCount = FILE_WRITE_BUFFER_SIZE;

				if( nBufferSize < nCount ) {
					nBufferSize = nCount;
					delete [] pBuffer;		pBuffer = new CHAR[3 * (nBufferSize + 1)];
					delete [] pWideBuffer;	pWideBuffer = new UCHAR[2 * (nBufferSize + 1)];
				}

				nCount = MultiByteToWideChar(CP_ACP, 0, rLine, -1, (LPWSTR)pWideBuffer, nBufferSize + 1);
				nCount = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pWideBuffer, -1, pBuffer, 3 * (nBufferSize + 1), NULL, NULL);
				INT nLength = strlen( pBuffer );

				file.Write( pBuffer, nLength );
				if( pos ) file.Write( szDelim, nDelimSize );
			}
		} else { /* nEncodingType == ENCODING_TYPE_ASCII */
			while( pos ) {
				CAnalyzedString & rLine = GetNext(pos);
				INT nLength = rLine.GetLength();

				file.Write( rLine, nLength );
				if( pos ) file.Write( szDelim, nDelimSize );
			}
		}

		delete [] pBuffer;
		delete [] pWideBuffer;

		file.Close();

	} catch( CException * ex ) {
		ex->ReportError( MB_OK | MB_ICONSTOP );
		ex->Delete(); return FALSE;
	}

	return TRUE;
}


BOOL CAnalyzedText::HaveAnyOverflowLine()
{
	POSITION pos = GetHeadPosition();
	while( pos ) {
		CAnalyzedString & rLine = GetNext(pos);
		if( rLine.m_usLineInfo & LI_HAVEOVERFLOW ) return TRUE;
	}
	return FALSE;
}


// CUndoBuffer
void CUndoBuffer::EmptyBuffer()
{
	m_lstAction.RemoveAll();
	m_lstIdxX.RemoveAll(); m_lstIdxY.RemoveAll();
	m_lstParam.RemoveAll();
	m_lstChar.RemoveAll();
	m_lstString.RemoveAll();
	m_lstBlock.RemoveAll();
	RemoveAll();
}

void CUndoBuffer::GetRecentIndex(INT & nIdxX, INT & nIdxY)
{
	if( ! GetCount() ) return;
	nIdxX = m_lstIdxX.GetHead();
	nIdxY = m_lstIdxY.GetHead();
}


// CUserCommand
CUserCommand::CUserCommand()
{
	m_wVirtualKeyCode = m_wModifiers = 0x00;
	m_szName = m_szCommand = "";
	m_szArgument = m_szDirectory = "";
	m_bCloseOnExit = TRUE; m_bUseShortFileName = FALSE;
	m_bCaptureOutput = FALSE; m_bSaveBeforeExecute = TRUE;
}

CString CUserCommand::GetHotKeyText()
{
	UINT nScanCode = MapVirtualKey( m_wVirtualKeyCode, 0 );
	LPARAM lParam = nScanCode << 16;
	TCHAR szKeyName[1024]; GetKeyNameText( lParam, szKeyName, 1024 );

	CString szHotKeyText = "";
	if( strlen(szKeyName) ) {
		if( m_wModifiers & HOTKEYF_CONTROL ) szHotKeyText += "Ctrl+";
		if( m_wModifiers & HOTKEYF_ALT ) szHotKeyText += "Alt+";
		if( m_wModifiers & HOTKEYF_SHIFT ) szHotKeyText += "Shift+";
		szHotKeyText += szKeyName;
	}
	return szHotKeyText;
}

BOOL CUserCommand::StreamSave(ofstream & fout)
{
	INT nLength;

	fout.write((const char *)(& m_wVirtualKeyCode), sizeof(WORD));
	fout.write((const char *)(& m_wModifiers), sizeof(WORD));

	nLength = m_szName.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	if( nLength ) fout.write((const char *)(LPCTSTR)m_szName, nLength);

	nLength = m_szCommand.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	if( nLength ) fout.write((const char *)(LPCTSTR)m_szCommand, nLength);

	nLength = m_szArgument.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	if( nLength ) fout.write((const char *)(LPCTSTR)m_szArgument, nLength);

	nLength = m_szDirectory.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	if( nLength ) fout.write((const char *)(LPCTSTR)m_szDirectory, nLength);

	fout.write((const char *)(& m_bCloseOnExit), sizeof(BOOL));
	fout.write((const char *)(& m_bUseShortFileName), sizeof(BOOL));
	fout.write((const char *)(& m_bCaptureOutput), sizeof(BOOL));
	fout.write((const char *)(& m_bSaveBeforeExecute), sizeof(BOOL));

	return TRUE;
}

BOOL CUserCommand::StreamLoad(ifstream & fin)
{
	INT nLength; TCHAR szBuffer[4096];

	fin.read((char *)(& m_wVirtualKeyCode), sizeof(WORD));
	fin.read((char *)(& m_wModifiers), sizeof(WORD));

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength ) fin.read((char *)(szBuffer), nLength); 
	szBuffer[nLength] = '\0'; m_szName = szBuffer;

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength ) fin.read((char *)(szBuffer), nLength); 
	szBuffer[nLength] = '\0'; m_szCommand = szBuffer;

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength ) fin.read((char *)(szBuffer), nLength); 
	szBuffer[nLength] = '\0'; m_szArgument = szBuffer;

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength ) fin.read((char *)(szBuffer), nLength); 
	szBuffer[nLength] = '\0'; m_szDirectory = szBuffer;

	fin.read((char *)(& m_bCloseOnExit), sizeof(BOOL));
	fin.read((char *)(& m_bUseShortFileName), sizeof(BOOL));
	fin.read((char *)(& m_bCaptureOutput), sizeof(BOOL));
	fin.read((char *)(& m_bSaveBeforeExecute), sizeof(BOOL));

	return TRUE;
}

void CUserCommand::DeleteContents()
{
	m_wVirtualKeyCode = m_wModifiers = 0x00;
	m_szName = m_szCommand = "";
	m_szArgument = m_szDirectory = "";
	m_bCloseOnExit = TRUE; m_bUseShortFileName = FALSE;
	m_bCaptureOutput = FALSE; m_bSaveBeforeExecute = TRUE;
}

void CUserCommand::CopyContents(CUserCommand & rCommand)
{
	m_wVirtualKeyCode = rCommand.m_wVirtualKeyCode; m_wModifiers = rCommand.m_wModifiers;
	m_szName = rCommand.m_szName; m_szCommand = rCommand.m_szCommand;
	m_szArgument = rCommand.m_szArgument; m_szDirectory = rCommand.m_szDirectory;
	m_bCloseOnExit = rCommand.m_bCloseOnExit; m_bUseShortFileName = rCommand.m_bUseShortFileName;
	m_bCaptureOutput = rCommand.m_bCaptureOutput; m_bSaveBeforeExecute = rCommand.m_bSaveBeforeExecute;
}


// CMacroBuffer
CMacroBuffer::CMacroBuffer()
{
	m_wVirtualKeyCode = 0x00; m_wModifiers = 0x00;
	m_szName = "";
	m_lstAction.RemoveAll();
	m_lstParam.RemoveAll();
	m_lstFlags.RemoveAll();
	m_lstString.RemoveAll();
}

CString CMacroBuffer::GetHotKeyText()
{
	UINT nScanCode = MapVirtualKey( m_wVirtualKeyCode, 0 );
	LPARAM lParam = nScanCode << 16;
	TCHAR szKeyName[1024]; GetKeyNameText( lParam, szKeyName, 1024 );

	CString szHotKeyText = "";
	if( strlen(szKeyName) ) {
		if( m_wModifiers & HOTKEYF_CONTROL ) szHotKeyText += "Ctrl+";
		if( m_wModifiers & HOTKEYF_ALT ) szHotKeyText += "Alt+";
		if( m_wModifiers & HOTKEYF_SHIFT ) szHotKeyText += "Shift+";
		szHotKeyText += szKeyName;
	}
	return szHotKeyText;
}

BOOL CMacroBuffer::StreamSave(ofstream & fout)
{
	INT nLength, nCount; POSITION pos;
	INT nAction; UINT nParam, nFlags; CString szString;

	fout.write((const char *)(& m_wVirtualKeyCode), sizeof(WORD));
	fout.write((const char *)(& m_wModifiers), sizeof(WORD));

	nLength = m_szName.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	if( nLength ) fout.write((const char *)(LPCTSTR)m_szName, nLength);

	nCount = m_lstAction.GetCount(); pos = m_lstAction.GetHeadPosition();
	fout.write((const char *)(& nCount), sizeof(nCount));
	while( pos ) { 
		nAction = m_lstAction.GetNext(pos); 
		fout.write((const char *)(& nAction), sizeof(nAction));
	}

	nCount = m_lstParam.GetCount(); pos = m_lstParam.GetHeadPosition();
	fout.write((const char *)(& nCount), sizeof(nCount));
	while( pos ) { 
		nParam = m_lstParam.GetNext(pos); 
		fout.write((const char *)(& nParam), sizeof(nParam));
	}

	nCount = m_lstFlags.GetCount(); pos = m_lstFlags.GetHeadPosition();
	fout.write((const char *)(& nCount), sizeof(nCount));
	while( pos ) { 
		nFlags = m_lstFlags.GetNext(pos); 
		fout.write((const char *)(& nFlags), sizeof(nFlags));
	}

	nCount = m_lstString.GetCount(); pos = m_lstString.GetHeadPosition();
	fout.write((const char *)(& nCount), sizeof(nCount));
	while( pos ) { 
		szString = m_lstString.GetNext(pos); nLength = szString.GetLength();
		fout.write((const char *)(& nLength), sizeof(nLength));
		fout.write((const char *)(LPCTSTR)szString, nLength);
	}

	return TRUE;
}

BOOL CMacroBuffer::StreamLoad(ifstream & fin)
{
	INT nLength, nCount; TCHAR szBuffer[4096];
	INT nAction; UINT nParam, nFlags; CString szString;

	fin.read((char *)(& m_wVirtualKeyCode), sizeof(WORD));
	fin.read((char *)(& m_wModifiers), sizeof(WORD));

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength ) fin.read((char *)(szBuffer), nLength); 
	szBuffer[nLength] = '\0'; m_szName = szBuffer;

	fin.read((char *)(& nCount), sizeof(nCount));
	m_lstAction.RemoveAll();
	while( nCount-- ) {
		fin.read((char *)(& nAction), sizeof(nAction));
		m_lstAction.AddTail(nAction);
	}

	fin.read((char *)(& nCount), sizeof(nCount));
	m_lstParam.RemoveAll();
	while( nCount-- ) {
		fin.read((char *)(& nParam), sizeof(nParam));
		m_lstParam.AddTail(nParam);
	}

	fin.read((char *)(& nCount), sizeof(nCount));
	m_lstFlags.RemoveAll();
	while( nCount-- ) {
		fin.read((char *)(& nFlags), sizeof(nFlags));
		m_lstFlags.AddTail(nFlags);
	}

	fin.read((char *)(& nCount), sizeof(nCount));
	m_lstString.RemoveAll();
	while( nCount-- ) {
		fin.read((char *)(& nLength), sizeof(nLength));
		fin.read((char *)(szBuffer), nLength); szBuffer[nLength] = '\0';
		m_lstString.AddTail(szBuffer);
	}

	return TRUE;
}

void CMacroBuffer::DeleteContents()
{
	m_wVirtualKeyCode = m_wModifiers = 0x00;
	m_szName = "";
	m_lstAction.RemoveAll();
	m_lstParam.RemoveAll();
	m_lstFlags.RemoveAll();
	m_lstString.RemoveAll();
}

void CMacroBuffer::CopyContents(CMacroBuffer & rBuffer)
{
	m_wVirtualKeyCode = rBuffer.m_wVirtualKeyCode; m_wModifiers = rBuffer.m_wModifiers;
	m_szName = rBuffer.m_szName;
	m_lstAction.RemoveAll(); m_lstAction.AddTail(& rBuffer.m_lstAction);
	m_lstParam.RemoveAll(); m_lstParam.AddTail(& rBuffer.m_lstParam);
	m_lstFlags.RemoveAll(); m_lstFlags.AddTail(& rBuffer.m_lstFlags);
	m_lstString.RemoveAll(); m_lstString.AddTail(& rBuffer.m_lstString);
}


// COutputPattern
BOOL COutputPattern::StreamSave(ofstream & fout)
{
	return FALSE;
}

BOOL COutputPattern::StreamLoad(ifstream & fin)
{
	return FALSE;
}


// CFileFilter
BOOL CFileFilter::StreamSave(ofstream & fout)
{
	INT nLength;

	nLength = m_szDescription.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	if( nLength ) fout.write((const char *)(LPCTSTR)(m_szDescription), nLength);

	nLength = m_szExtensions.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	if( nLength ) fout.write((const char *)(LPCTSTR)(m_szExtensions), nLength);

	nLength = m_szDefaultExt.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	if( nLength ) fout.write((const char *)(LPCTSTR)(m_szDefaultExt), nLength);

	return TRUE;
}

BOOL CFileFilter::StreamLoad(ifstream & fin)
{
	INT nLength; TCHAR szBuffer[4096];

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength < 0 || nLength >= 4096 ) return FALSE;
	if( nLength ) fin.read((char *)(szBuffer), nLength);
	szBuffer[nLength] = '\0'; m_szDescription = szBuffer;

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength < 0 || nLength >= 4096 ) return FALSE;
	if( nLength ) fin.read((char *)(szBuffer), nLength);
	szBuffer[nLength] = '\0'; m_szExtensions = szBuffer;

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength < 0 || nLength >= 4096 ) return FALSE;
	if( nLength ) fin.read((char *)(szBuffer), nLength);
	szBuffer[nLength] = '\0'; m_szDefaultExt = szBuffer;

	return TRUE;
}

void CFileFilter::AssignContents(LPCTSTR lpszDescription, LPCTSTR lpszExtensions, LPCTSTR lpszDefaultExt)
{
	m_szDescription = lpszDescription;
	m_szExtensions = lpszExtensions;
	m_szDefaultExt = lpszDefaultExt;
}

void CFileFilter::DeleteContents()
{
	m_szDescription = "";
	m_szExtensions = "";
	m_szDefaultExt = "";
}

void CFileFilter::CopyContents(CFileFilter & rFilter)
{
	m_szDescription = rFilter.m_szDescription;
	m_szExtensions = rFilter.m_szExtensions;
	m_szDefaultExt = rFilter.m_szDefaultExt;
}


// CSyntaxType
BOOL CSyntaxType::StreamSave(ofstream & fout)
{
	INT nLength;

	nLength = m_szDescription.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	if( nLength ) fout.write((const char *)(LPCTSTR)m_szDescription, nLength);

	nLength = m_szLangSpecFile.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	if( nLength ) fout.write((const char *)(LPCTSTR)m_szLangSpecFile, nLength);

	nLength = m_szKeywordsFile.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	if( nLength ) fout.write((const char *)(LPCTSTR)m_szKeywordsFile, nLength);

	return TRUE;
}

BOOL CSyntaxType::StreamLoad(ifstream & fin)
{
	INT nLength; TCHAR szBuffer[4096];

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength < 0 || nLength >= 4096 ) return FALSE;
	if( nLength ) fin.read((char *)(szBuffer), nLength); 
	szBuffer[nLength] = '\0'; m_szDescription = szBuffer;

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength < 0 || nLength >= 4096 ) return FALSE;
	if( nLength ) fin.read((char *)(szBuffer), nLength); 
	szBuffer[nLength] = '\0'; m_szLangSpecFile = szBuffer;

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength < 0 || nLength >= 4096 ) return FALSE;
	if( nLength ) fin.read((char *)(szBuffer), nLength); 
	szBuffer[nLength] = '\0'; m_szKeywordsFile = szBuffer;

	return TRUE;
}

void CSyntaxType::AssignContents(LPCTSTR lpszDescription, LPCTSTR lpszLangSpecFile, LPCTSTR lpszKeywordsFile)
{
	m_szDescription = lpszDescription;
	m_szLangSpecFile = lpszLangSpecFile;
	m_szKeywordsFile = lpszKeywordsFile;
}

void CSyntaxType::DeleteContents()
{
	m_szDescription = "";
	m_szLangSpecFile = "";
	m_szKeywordsFile = "";
}

void CSyntaxType::CopyContents(CSyntaxType & rSyntax)
{
	m_szDescription = rSyntax.m_szDescription;
	m_szLangSpecFile = rSyntax.m_szLangSpecFile;
	m_szKeywordsFile = rSyntax.m_szKeywordsFile;
}


// CFtpAccount
CFtpAccount::CFtpAccount()
{
	m_szDescription = m_szServerName = "";
	m_szUserName = m_szPassword = "";
	m_szSubDirectory = "";

	m_bSavePassword = m_bPassiveMode = m_bBinaryType = FALSE;
	m_bUseWinInet = m_bPasswordVerified = FALSE;

	m_nServerType = FTP_SERVER_GENERIC;
	m_nPortNumber = 21;
}

BOOL CFtpAccount::StreamSave(ofstream & fout)
{
	INT nLength;

	nLength = m_szDescription.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	fout.write((const char *)(LPCTSTR)m_szDescription, nLength);

	nLength = m_szServerName.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	fout.write((const char *)(LPCTSTR)m_szServerName, nLength);

	nLength = m_szUserName.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	fout.write((const char *)(LPCTSTR)m_szUserName, nLength);

	CString szEncodedPassword = "";
	if( m_bSavePassword ) szEncodedPassword = map_encode(m_szPassword);

	nLength = szEncodedPassword.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	fout.write((const char *)(LPCTSTR)szEncodedPassword, nLength);

	nLength = m_szSubDirectory.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	fout.write((const char *)(LPCTSTR)m_szSubDirectory, nLength);

	fout.write((const char *)(& m_bSavePassword), sizeof(m_bSavePassword));
	fout.write((const char *)(& m_bPassiveMode), sizeof(m_bPassiveMode));
	fout.write((const char *)(& m_bBinaryType), sizeof(m_bBinaryType));
	fout.write((const char *)(& m_bUseWinInet), sizeof(m_bUseWinInet));
//	fout.write((const char *)(& m_bPasswordVerified), sizeof(m_bPasswordVerified));

	fout.write((const char *)(& m_nServerType), sizeof(m_nServerType));
	fout.write((const char *)(& m_nPortNumber), sizeof(m_nPortNumber));

	return TRUE;
}

BOOL CFtpAccount::StreamLoad(ifstream & fin)
{
	INT nLength; TCHAR szBuffer[4096];

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength < 0 || nLength > 2048 ) return FALSE;
	fin.read((char *)(szBuffer), nLength); szBuffer[nLength] = '\0';
	m_szDescription = szBuffer;

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength < 0 || nLength > 2048 ) return FALSE;
	fin.read((char *)(szBuffer), nLength); szBuffer[nLength] = '\0';
	m_szServerName = szBuffer;

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength < 0 || nLength > 2048 ) return FALSE;
	fin.read((char *)(szBuffer), nLength); szBuffer[nLength] = '\0';
	m_szUserName = szBuffer;

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength < 0 || nLength > 2048 ) return FALSE;
	fin.read((char *)(szBuffer), nLength); szBuffer[nLength] = '\0';

	CString szEncodedPassword = szBuffer;
	m_szPassword = map_decode(szEncodedPassword);

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength < 0 || nLength > 2048 ) return FALSE;
	fin.read((char *)(szBuffer), nLength); szBuffer[nLength] = '\0';
	m_szSubDirectory = szBuffer;

	fin.read((char *)(& m_bSavePassword), sizeof(m_bSavePassword));
	fin.read((char *)(& m_bPassiveMode), sizeof(m_bPassiveMode));
	fin.read((char *)(& m_bUseWinInet), sizeof(m_bUseWinInet));
	fin.read((char *)(& m_bBinaryType), sizeof(m_bBinaryType));
	m_bPasswordVerified = FALSE;

	fin.read((char *)(& m_nServerType), sizeof(m_nServerType));
	fin.read((char *)(& m_nPortNumber), sizeof(m_nPortNumber));

	return TRUE;
}

CString CFtpAccount::GetDisplayName()
{
	CString szDisplayName;
	if( m_szDescription.GetLength() ) {
		if( m_szServerName.GetLength() ) {
			if( m_szUserName.GetLength() ) szDisplayName.Format("%s [ftp://%s@%s]", m_szDescription, m_szUserName, m_szServerName);
			else szDisplayName.Format("%s [ftp://%s]", m_szDescription, m_szServerName);
		} else szDisplayName = m_szDescription;
	} else szDisplayName = "- Empty -";
	return szDisplayName;
}

CString CFtpAccount::GetFullAccountName()
{
	CString szAccountName;
	if( m_szServerName.GetLength() ) {
		if( m_szUserName.GetLength() ) szAccountName.Format("ftp://%s@%s", m_szUserName, m_szServerName);
		else szAccountName.Format("ftp://%s", m_szServerName);
	} else szAccountName = "";
	return szAccountName;
}

CString CFtpAccount::GetShortAccountName()
{
	CString szAccountName;
	if( m_szServerName.GetLength() ) {
		if( m_szUserName.GetLength() ) szAccountName.Format("%s@%s", m_szUserName, m_szServerName);
		else szAccountName.Format("%s", m_szServerName);
	} else szAccountName = "";
	return szAccountName;
}

void CFtpAccount::DeleteContents()
{
	m_szDescription = m_szServerName = "";
	m_szUserName = m_szPassword = "";
	m_szSubDirectory = "";

	m_bSavePassword = m_bPassiveMode = m_bBinaryType = FALSE;
	m_bUseWinInet = m_bPasswordVerified = FALSE;

	m_nServerType = FTP_SERVER_GENERIC;
	m_nPortNumber = 21;
}

void CFtpAccount::CopyContents(CFtpAccount & rAccount)
{
	m_szDescription = rAccount.m_szDescription;		m_szServerName = rAccount.m_szServerName;
	m_szUserName = rAccount.m_szUserName;			m_szPassword = rAccount.m_szPassword;
	m_szSubDirectory = rAccount.m_szSubDirectory;

	m_bSavePassword = rAccount.m_bSavePassword;		m_bPassiveMode = rAccount.m_bPassiveMode;
	m_bBinaryType = rAccount.m_bBinaryType;
	m_bUseWinInet = rAccount.m_bUseWinInet;			m_bPasswordVerified = rAccount.m_bPasswordVerified;

	m_nServerType = rAccount.m_nServerType;
	m_nPortNumber = rAccount.m_nPortNumber;
}


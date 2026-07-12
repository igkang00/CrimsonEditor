#include "stdafx.h"
#include "fstream_compat.h"
#include <sstream>
#include <vector>
#include <ctype.h>
#include "cedtUnicode.h"
#include "Encode.h"
#include "RegExp.h"
#include "perflog.h"
#include "Utility.h"
#include "cedtElement.h"


#define _SWAP_UCHAR(A, B)				{ _uchar_temp = (A); (A) = (B); (B) = _uchar_temp; }
static UCHAR _uchar_temp;

// Under _UNICODE every CString char is 2 bytes on disk. The pre-Unicode
// StreamSave/StreamLoad passed GetLength() (char count) to write()/read()
// (byte count), which chopped every stored string in half. These helpers
// keep nLength as the char count on the wire and multiply by sizeof(TCHAR)
// at the byte boundary so the format is char-count + wide-bytes.
#define _WRITE_WIDE_STR(fout, ptr, nChars) \
	do { if( (nChars) > 0 ) (fout).write((const char *)(ptr), (nChars) * (INT)sizeof(TCHAR)); } while(0)
#define _READ_WIDE_STR(fin, buf, nChars) \
	do { if( (nChars) > 0 ) (fin).read((char *)(buf), (nChars) * (INT)sizeof(TCHAR)); } while(0)

#define _INITIAL_TYPES(types, atype)	{ types[0] = types[1] = types[2] = atype; types[3] = 0x00; }
#define _COMPOSE_TYPES(types)			( MAKELONG( MAKEWORD(types[0], types[1]), MAKEWORD(types[2], types[3]) ) )
#define _EXTRACT_TYPES(types, dword)	{ \
	_lword_temp = LOWORD(dword);		_hword_temp = HIWORD(dword); \
	types[0] = LOBYTE(_lword_temp);		types[1] = HIBYTE(_lword_temp); \
	types[2] = LOBYTE(_hword_temp);		types[3] = HIBYTE(_hword_temp); }
static WORD _lword_temp, _hword_temp;


// ENCODING_TYPE_ASCII is a legacy label — the disk bytes go through the
// system's ANSI code page (CP1252 on English Windows, CP949 on Korean),
// so pure-ASCII files look identical but anything above 0x7F is really
// ANSI/MBCS. The user-visible strings say "ANSI" so the menu, status
// bar, and document-summary dialog agree; the enum name stays for
// binary compatibility with existing cedt_kr.conf / cedt_us.conf files.
CString ENCODING_TYPE_DESCRIPTION_FULL[] = {
	/* ENCODING_TYPE_UNKNOWN */			_T("Unknown Encoding"),
	/* ENCODING_TYPE_ASCII */			_T("ANSI (system code page)"),
	/* ENCODING_TYPE_UNICODE_LE */		_T("UTF-16 (Little Endian)"),
	/* ENCODING_TYPE_UNICODE_BE */		_T("UTF-16 (Big Endian)"),
	/* ENCODING_TYPE_UTF8_WBOM */		_T("UTF-8 (with BOM)"),
	/* ENDOCING_TYPE_UTF8_XBOM */		_T("UTF-8 (w/o BOM)"),
};

// The status bar pane is narrow, so keep these to the family name
// (ANSI / UTF-8 / UTF-16). BOM presence and byte order are already
// visible in the menu and in the document summary dialog.
CString ENCODING_TYPE_DESCRIPTION_SHORT[] = {
	/* ENCODING_TYPE_UNKNOWN */			_T("N.A."),
	/* ENCODING_TYPE_ASCII */			_T("ANSI"),
	/* ENCODING_TYPE_UNICODE_LE */		_T("UTF-16"),
	/* ENCODING_TYPE_UNICODE_BE */		_T("UTF-16"),
	/* ENCODING_TYPE_UTF8_WBOM */		_T("UTF-8"),
	/* ENCODING_TYPE_UTF8_XBOM */		_T("UTF-8"),
};

CString FILE_FORMAT_DESCRIPTION_FULL[] = {
	/* FILE_FORMAT_UNKNOWN */			_T("Unknown Format"),
	/* FILE_FORMAT_DOS     */			_T("DOS Format"),
	/* FILE_FORMAT_UNIX    */			_T("UNIX Format"),
	/* FILE_FORMAT_MAC     */			_T("MAC Format"),
};

CString FILE_FORMAT_DESCRIPTION_SHORT[] = {
	/* FILE_FORMAT_UNKNOWN */			_T("N.A."),
	/* FILE_FORMAT_DOS     */			_T("DOS"),
	/* FILE_FORMAT_UNIX    */			_T("UNIX"),
	/* FILE_FORMAT_MAC     */			_T("MAC"),
};


static UCHAR _GetRangeType1(LPCTSTR lpszRange)
{
	if     ( ! _tcsnicmp(lpszRange, _T("GLOBAL"), 6) ) return RT_GLOBAL;
	else if( ! _tcsnicmp(lpszRange, _T("RANGE1"), 6) ) return RT_RANGE1;
	else if( ! _tcsnicmp(lpszRange, _T("RANGE2"), 6) ) return RT_RANGE2;
	else if( ! _tcsnicmp(lpszRange, _T("R1||R2"), 6) ) return RT_R1ORR2;
	return RT_GLOBAL;
}

static UCHAR _GetRangeType2(LPCTSTR lpszRange)
{
	if     ( ! _tcsnicmp(lpszRange, _T("GLOBAL"), 6) ) return RT_GLOBAL;
	else if( ! _tcsnicmp(lpszRange, _T("RANGE1"), 6) ) return RT_RANGE1;
	else if( ! _tcsnicmp(lpszRange, _T("RANGE2"), 6) ) return RT_RANGE2;
	return RT_GLOBAL;
}


BOOL DetectEncodingTypeAndFileFormat(LPCTSTR lpszPathName, UINT & nEncodingType, UINT & nFileFormat)
{
	// 64 KiB sample matches what uchardet / chardet use as their default
	// detection window — small enough that reading it is still constant-time
	// even on huge log files, large enough that source files with a long
	// ASCII header and non-ASCII text further down still reveal their
	// encoding within the sample.
	const INT SNIFF_SIZE = 65536;

	try {
		CFile file(lpszPathName, CFile::modeRead | CFile::typeBinary);
		UCHAR szBuffer[SNIFF_SIZE]; INT nCount = file.Read( szBuffer, SNIFF_SIZE );
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

	// 1) BOM-based detection — fast path, zero ambiguity.
	if( nLength >= 2 && lpBuffer[0] == 0xFF && lpBuffer[1] == 0xFE ) { nEncodingType = ENCODING_TYPE_UNICODE_LE; return TRUE; }
	if( nLength >= 2 && lpBuffer[0] == 0xFE && lpBuffer[1] == 0xFF ) { nEncodingType = ENCODING_TYPE_UNICODE_BE; return TRUE; }
	if( nLength >= 3 && lpBuffer[0] == 0xEF && lpBuffer[1] == 0xBB && lpBuffer[2] == 0xBF ) { nEncodingType = ENCODING_TYPE_UTF8_WBOM; return TRUE; }

	// 2) BOM-less UTF-8 heuristic.
	//
	// Modern editors (VS Code, GitHub's web UI, most CLI tools) save UTF-8
	// without a BOM by default, so relying purely on the BOM misdetects the
	// majority of new text files as ASCII/MBCS (CP949 on a Korean Windows)
	// and corrupts every non-ASCII byte on load. Add a byte-sequence check —
	// the same idea Notepad2/Notepad++ community proposals use and what
	// uchardet/chardet do under the hood.
	//
	// Rules for a valid UTF-8 multi-byte sequence:
	//   110xxxxx + 10xxxxxx                      (2 bytes)
	//   1110xxxx + 10xxxxxx*2                    (3 bytes)
	//   11110xxx + 10xxxxxx*3                    (4 bytes)
	// A standalone continuation byte (10xxxxxx) or a leading byte in the
	// 11111xxx range means the file is not UTF-8 but still has bytes above
	// 0x7F, so it is a legacy MBCS/ANSI file — classified as
	// ENCODING_TYPE_ASCII (see the two branches below), never UNKNOWN. Only a
	// file with no non-ASCII byte at all stays UNKNOWN.
	//
	// A file that reaches the end of the sample with at least one fully-
	// validated multi-byte sequence and zero rule violations is treated as
	// UTF-8 without BOM (ENCODING_TYPE_UTF8_XBOM). Pure ASCII stays UNKNOWN
	// because ASCII is a valid subset of both UTF-8 and MBCS — no need to
	// commit to one over the other.
	//
	// Trade-off: a very short CP949 file whose few bytes happen to satisfy
	// UTF-8 leading/continuation ranges could be misclassified as UTF-8. In
	// practice the probability drops quickly with each additional character,
	// and the flip side — misdetecting BOM-less UTF-8 as CP949 — was the
	// more common day-to-day problem.
	BOOL bHasValidatedNonAscii = FALSE;
	INT i = 0;
	while( i < nLength ) {
		BYTE b = lpBuffer[i];
		if( b < 0x80 ) { i++; continue; }

		INT nContinuation;
		if     ( (b & 0xE0) == 0xC0 ) nContinuation = 1; // 110xxxxx
		else if( (b & 0xF0) == 0xE0 ) nContinuation = 2; // 1110xxxx
		else if( (b & 0xF8) == 0xF0 ) nContinuation = 3; // 11110xxx
		// A byte above 0x7F that is not a valid UTF-8 lead byte (a bare
		// continuation byte, or an 11111xxx byte) means this is a legacy
		// MBCS/ANSI file (CP949 on a Korean host), not UTF-8. Classify it as
		// ANSI — NOT UNKNOWN — so the caller does not replace UNKNOWN with the
		// default encoding (UTF-8 w/o BOM since v3.90) and decode every CP949
		// byte as broken UTF-8.
		else { nEncodingType = ENCODING_TYPE_ASCII; return TRUE; }

		// If the multi-byte sequence is cut off at the sample boundary, be
		// conservative and stop scanning — don't commit either way based on
		// a partial sequence we can't finish validating.
		if( i + nContinuation >= nLength ) break;

		for( INT k = 1; k <= nContinuation; k++ ) {
			if( (lpBuffer[i + k] & 0xC0) != 0x80 ) {
				// UTF-8 lead byte without its continuation byte(s): again a
				// legacy MBCS/ANSI file, so commit to ANSI (see above).
				nEncodingType = ENCODING_TYPE_ASCII;
				return TRUE;
			}
		}
		bHasValidatedNonAscii = TRUE;
		i += nContinuation + 1;
	}
	if( bHasValidatedNonAscii ) {
		nEncodingType = ENCODING_TYPE_UTF8_XBOM;
		return TRUE;
	}

	nEncodingType = ENCODING_TYPE_UNKNOWN; // pure ASCII / undetectable
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
	m_szDelimiters = _T("(){}[]<>+-*/%=\"'~!@#$^&|\\?:;,."); // Omited '_' from default delimiters 2004.08.08
	m_szHexaDecimalMark = m_szKeywordPrefix = m_szVariablePrefix = _T("");
	m_szVariableOptionallyEnclosedBy = m_szSpecialVariableChars = _T("");
	m_chVariableQuotation = 0x00;

	m_bMultiLineStringConstant = FALSE;
	m_chEscapeChar = m_chMultiLineQuotationMark = 0x00;
	m_chQuotationMark1 = m_chQuotationMark2 = m_chQuotationMark3 = 0x00;
	m_ucMultiLineQuotationMarkRange = RT_GLOBAL;
	m_ucQuotationMark1Range = m_ucQuotationMark2Range = m_ucQuotationMark3Range = RT_GLOBAL;
	m_szHereDocument = _T("");

	m_szLineComment1OnFirstPosition = m_szLineComment2OnFirstPosition = _T("");
	m_szLineComment1 = m_szLineComment2 = _T("");
	m_ucLineComment1Range = m_ucLineComment2Range = RT_GLOBAL;

	m_szBlockComment1On = m_szBlockComment1Off = _T("");
	m_szBlockComment2On = m_szBlockComment2Off = _T("");
	m_ucBlockComment1Range = m_ucBlockComment2Range = RT_GLOBAL;

	m_szShadowOn = m_szShadowOff = _T("");
	m_szHighlightOn = m_szHighlightOff = _T("");
	m_szRange1Beg = m_szRange1End = _T("");
	m_szRange2Beg = m_szRange2End = _T("");

	m_chIndentationOn = m_chIndentationOff = 0x00;
	m_szPairs1 = m_szPairs2 = m_szPairs3 = _T("");
}

BOOL CLangSpec::FileLoad(LPCTSTR lpszPathName) 
{
	TCHAR szLine[4096];
	ResetContents(); // clear previous settings

	wifstream fin(lpszPathName, ios::in);
	if( ! fin.is_open() ) return FALSE;

	while( fin.good() ) {
		fin.getline(szLine, 4096);
		if( szLine[0] != '$' ) continue;

		TCHAR * ptr1 = _tcstok(szLine, _T("="));
		TCHAR * ptr2 = _tcstok(NULL, _T("\n"));

		if     ( ptr2 && ! _tcsicmp(ptr1, _T("$CASESENSITIVE")) ) { m_bCaseSensitive[0] = m_bCaseSensitive[1] = m_bCaseSensitive[2] = (ptr2[0] == 'Y' || ptr2[0] == 'y') ? TRUE : FALSE; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$CASESENSITIVEINGLOBAL")) ) { m_bCaseSensitive[0] = (ptr2[0] == 'Y' || ptr2[0] == 'y') ? TRUE : FALSE; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$CASESENSITIVEINRANGE1")) ) { m_bCaseSensitive[1] = (ptr2[0] == 'Y' || ptr2[0] == 'y') ? TRUE : FALSE; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$CASESENSITIVEINRANGE2")) ) { m_bCaseSensitive[2] = (ptr2[0] == 'Y' || ptr2[0] == 'y') ? TRUE : FALSE; }

		else if( ptr2 && ! _tcsicmp(ptr1, _T("$VARIABLEHIGHLIGHTINSTRING")) ) { m_bVariableHighlightInString = (ptr2[0] == 'Y' || ptr2[0] == 'y') ? TRUE : FALSE; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$DELIMITERS")) ) { m_szDelimiters = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$HEXADECIMALMARK")) ) { m_szHexaDecimalMark = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$PREFIX")) ) { m_szKeywordPrefix = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$KEYWORDPREFIX")) ) { m_szKeywordPrefix = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$VARIABLEPREFIX")) ) { m_szVariablePrefix = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$VARIABLEENCLOSEDBY")) ) { m_szVariableOptionallyEnclosedBy = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$VARIABLEOPTIONALLYENCLOSEDBY")) ) { m_szVariableOptionallyEnclosedBy = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$SPECIALVARIABLECHARS")) ) { m_szSpecialVariableChars = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$VARIABLEQUOTATION")) ) { m_chVariableQuotation = ptr2[0]; }

		else if( ptr2 && ! _tcsicmp(ptr1, _T("$MULTILINESTRINGCONSTANT")) ) { m_bMultiLineStringConstant = (ptr2[0] == 'Y' || ptr2[0] == 'y') ? TRUE : FALSE; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$ESCAPECHAR")) ) { m_chEscapeChar = ptr2[0]; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$MULTILINEQUOTATIONMARK")) ) { m_chMultiLineQuotationMark = ptr2[0]; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$QUOTATIONMARK1")) ) { m_chQuotationMark1 = ptr2[0]; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$QUOTATIONMARK2")) ) { m_chQuotationMark2 = ptr2[0]; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$QUOTATIONMARK3")) ) { m_chQuotationMark3 = ptr2[0]; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$QUOTATIONMARKRANGE")) ) { m_ucMultiLineQuotationMarkRange = m_ucQuotationMark1Range = m_ucQuotationMark2Range = m_ucQuotationMark3Range = _GetRangeType1(ptr2); }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$MULTILINEQUOTATIONMARKRANGE")) ) { m_ucMultiLineQuotationMarkRange = _GetRangeType1(ptr2); }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$QUOTATIONMARK1RANGE")) ) { m_ucQuotationMark1Range = _GetRangeType1(ptr2); }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$QUOTATIONMARK2RANGE")) ) { m_ucQuotationMark2Range = _GetRangeType1(ptr2); }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$QUOTATIONMARK3RANGE")) ) { m_ucQuotationMark3Range = _GetRangeType1(ptr2); }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$HEREDOCUMENT")) ) { m_szHereDocument = ptr2; }

		else if( ptr2 && ! _tcsicmp(ptr1, _T("$LINECOMMENTONFIRSTPOSITION") ) ) { m_szLineComment1OnFirstPosition = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$LINECOMMENT1ONFIRSTPOSITION")) ) { m_szLineComment1OnFirstPosition = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$LINECOMMENT2ONFIRSTPOSITION")) ) { m_szLineComment2OnFirstPosition = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$LINECOMMENT") ) ) { m_szLineComment1 = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$LINECOMMENT1")) ) { m_szLineComment1 = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$LINECOMMENT2")) ) { m_szLineComment2 = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$LINECOMMENTRANGE")  ) ) { m_ucLineComment1Range = m_ucLineComment2Range = _GetRangeType1(ptr2); }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$LINECOMMENT1RANGE")  ) ) { m_ucLineComment1Range = _GetRangeType1(ptr2); }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$LINECOMMENT2RANGE")  ) ) { m_ucLineComment2Range = _GetRangeType1(ptr2); }

		else if( ptr2 && ! _tcsicmp(ptr1, _T("$BLOCKCOMMENTON")  ) ) { m_szBlockComment1On  = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$BLOCKCOMMENTOFF") ) ) { m_szBlockComment1Off = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$BLOCKCOMMENT1ON") ) ) { m_szBlockComment1On  = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$BLOCKCOMMENT1OFF")) ) { m_szBlockComment1Off = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$BLOCKCOMMENT2ON") ) ) { m_szBlockComment2On  = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$BLOCKCOMMENT2OFF")) ) { m_szBlockComment2Off = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$BLOCKCOMMENTRANGE") ) ) { m_ucBlockComment1Range = m_ucBlockComment2Range = _GetRangeType1(ptr2); }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$BLOCKCOMMENT1RANGE") ) ) { m_ucBlockComment1Range = _GetRangeType1(ptr2); }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$BLOCKCOMMENT2RANGE") ) ) { m_ucBlockComment2Range = _GetRangeType1(ptr2); }

		else if( ptr2 && ! _tcsicmp(ptr1, _T("$SHADOWON") ) ) { m_szShadowOn  = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$SHADOWOFF")) ) { m_szShadowOff = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$HIGHLIGHTON") ) ) { m_szHighlightOn  = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$HIGHLIGHTOFF")) ) { m_szHighlightOff = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$RANGE1BEG")) ) { m_szRange1Beg = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$RANGE1END")) ) { m_szRange1End = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$RANGE2BEG")) ) { m_szRange2Beg = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$RANGE2END")) ) { m_szRange2End = ptr2; }

		else if( ptr2 && ! _tcsicmp(ptr1, _T("$INDENTATIONON") ) ) { m_chIndentationOn  = ptr2[0]; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$INDENTATIONOFF")) ) { m_chIndentationOff = ptr2[0]; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$PAIRS1")) ) { m_szPairs1 = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$PAIRS2")) ) { m_szPairs2 = ptr2; }
		else if( ptr2 && ! _tcsicmp(ptr1, _T("$PAIRS3")) ) { m_szPairs3 = ptr2; }
	}

	fin.close();
	return TRUE;
}


// CKeywords
BOOL CKeywords::FileLoad(LPCTSTR lpszPathName, BOOL bCaseSensitive[])
{
	TCHAR szLine[MAX_LINE_BUFFER_SIZE], szWord[MAX_WORD_LENGTH+1], szBuffer[MAX_WORD_LENGTH+3];
	UCHAR ucType = 0x00, ucRange = RT_GLOBAL; BOOL bIgnoreCase, bNoEmbolden; UCHAR ucTypes[4]; DWORD dwValue;

	RemoveAll(); // clear hash table first

	wifstream fin(lpszPathName, ios::in);
	if( ! fin.is_open() ) return FALSE;

	while( fin.good() ) {
		fin.getline(szLine, MAX_LINE_BUFFER_SIZE);
		if( szLine[0] == '#' ) continue;

		if( ! _tcsnicmp(szLine, _T("[-COMMENT"), 9) ) {
			ucType  = 0x00;
			ucRange = RT_GLOBAL;

		} else if( ! _tcsnicmp(szLine, _T("[#COMMENT"), 9) ) {
			ucType  = 0x00;
			ucRange = RT_GLOBAL;

		} else if( ! _tcsnicmp(szLine, _T("[KEYWORDS"), 9) ) {
			TCHAR * ptr1 = _tcstok(szLine, _T(":"));
			TCHAR * ptr2 = _tcstok(NULL, _T("\n"));

			ucType  = WT_KEYWORD0 + (ptr1[9] - '0');
			ucRange = _GetRangeType2(ptr2);

			bIgnoreCase = ! bCaseSensitive[ucRange];
			bNoEmbolden = FALSE; _tcsupr(ptr2);

			if( _tcsstr(ptr2, _T("IGNORECASE")) ) bIgnoreCase = TRUE;
			if( _tcsstr(ptr2, _T("NOEMBOLDEN")) ) bNoEmbolden = TRUE;

		} else if( ucType ) {
			std::wistringstream sin(szLine);
			while( sin.good() ) {
				sin >> std::ws; if( ! sin.good() ) break;
				sin.width(sizeof(szWord) / sizeof(TCHAR));   // bound the read to the buffer size
				sin >> szWord;  if( szWord[0] == '\0' ) break;

				if( bIgnoreCase ) { _tcslwr(szWord); _stprintf(szBuffer, _T("I:%s"), szWord); }
				else _stprintf(szBuffer, _T("C:%s"), szWord);

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

BOOL CKeywords::LookupTable(UCHAR ucType[], UCHAR & ucRange, LPCTSTR lpszWord, INT_PTR siLengthPtr)
{
	static TCHAR szWord[MAX_WORD_LENGTH+1], szBuffer[MAX_WORD_LENGTH+3];
	if( siLengthPtr > MAX_WORD_LENGTH ) return FALSE;
	SHORT siLength = (SHORT)siLengthPtr;

	BOOL bFoundC, bFoundI; DWORD dwValue; 
	UCHAR ucTypeC[4], ucTypeI[4];

	_INITIAL_TYPES(ucTypeC, WT_IDENTIFIER);
	_INITIAL_TYPES(ucTypeI, WT_IDENTIFIER);

	_tcsncpy(szWord, lpszWord, siLength); szWord[siLength] = '\0'; _stprintf(szBuffer, _T("C:%s"), szWord);
	if( bFoundC = Lookup(szBuffer, dwValue) ) _EXTRACT_TYPES(ucTypeC, dwValue);

	_tcslwr(szWord); _stprintf(szBuffer, _T("I:%s"), szWord);
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

	wifstream fin(lpszPathName, ios::in);
	if( ! fin.is_open() ) return FALSE;

	while( fin.good() ) {
		fin >> std::ws; if( fin.eof() ) break;
		fin.width(sizeof(szWord) / sizeof(TCHAR));   // bound the read to the buffer size
		fin >> szWord; _tcslwr(szWord);

		if( ! Lookup( szWord, ucValue ) ) {
			SetAt( szWord, ucValue = WT_IDENTIFIER );
			nCount++;
		}

		// call a callback function if exist
		if( ! (nCount % 100) && fcnCallback ) fcnCallback( (UINT)fin.tellg() );
	}

	fin.close();
	return TRUE;
}

BOOL CDictionary::LookupTable(LPCTSTR lpszWord, INT_PTR siLengthPtr)
{
	static TCHAR szBuffer[MAX_WORD_LENGTH+1];
	if( siLengthPtr > MAX_WORD_LENGTH ) return FALSE;
	SHORT siLength = (SHORT)siLengthPtr;

	_tcsncpy(szBuffer, lpszWord, siLength); szBuffer[siLength] = '\0';
	_tcslwr(szBuffer);

	BOOL bFound; UCHAR ucValue = 0x00;
	bFound = Lookup(szBuffer, ucValue);

	return bFound;
}

BOOL CDictionary::AddWord(LPCTSTR lpszWord)
{
	TCHAR szWord[MAX_WORD_LENGTH+1]; UCHAR ucValue;
	if( _tcslen(lpszWord) > MAX_WORD_LENGTH ) return FALSE;
	
	_tcscpy(szWord, lpszWord); _tcslwr(szWord);
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
// Note this RESETS rather than copies — CList::AddTail(dummyLine) relies on it to
// mint a blank row cheaply.
CFormatedString & CFormatedString::operator=(const CFormatedString & stringSrc) {
	m_pString = stringSrc.m_pString;
	delete [] m_pWord; m_pWord = NULL;
	m_siWordCount = m_siSplitIndex = 0; m_bLineBreak = FALSE;
	m_usLineInfo = m_usLineFlag = 0x00;
	m_szHereDocumentTerminator = _T("");
	m_bFormatted = FALSE;
	return * this;
}


// CMemText
BOOL CMemText::FileLoad(LPCTSTR lpszPathName)
{
	try {
		CFile file(lpszPathName, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone);
		RemoveAll(); AddTail(_T("")); // initialize contents

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
			if( bDelimFount ) AddTail(_T(""));

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
			if( pos ) file.Write( _T("\r\n"), 2 );
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
	// Return byte size (not TCHAR count) so callers that use this to
	// GlobalAlloc a clipboard block get the right size under both
	// Unicode (2 bytes/char) and MBCS (1 byte/char) builds.
	INT count = 0;
	POSITION pos = GetHeadPosition();
	while( pos ) {
		CString & rString = GetNext( pos );
		count += rString.GetLength() + 2;
	}
	return count * (INT)sizeof(TCHAR);
}

void CMemText::MemoryLoad(CHAR * pMem, INT size)
{
	// `size` and `pMem` are byte-oriented; the actual content is TCHAR.
	// Under Unicode we walk one TCHAR at a time and compare against
	// _T('\r') / _T('\n') / _T('\0') — the pre-Unicode code assumed
	// sizeof(TCHAR)==1 so it split every other byte as a delimiter.
	RemoveAll();
	TCHAR * base = (TCHAR *)pMem;
	INT total = size / (INT)sizeof(TCHAR);
	TCHAR * beg, * fwd = base;
	while( fwd - base < total ) {
		beg = fwd;
		while( fwd - base < total && * fwd != _T('\r') && * fwd != _T('\n') && * fwd != _T('\0') ) fwd++;
		AddTail( CString(beg, (int)(fwd - beg)) );
		if( * fwd != _T('\0') ) { if(* fwd == _T('\r')) fwd++; if(* fwd == _T('\n')) fwd++; }
		else break;
	}
}

void CMemText::MemorySave(CHAR * pMem, INT size)
{
	// Byte pointer in, but the payload is TCHAR-aligned. Copy each
	// string's TCHARs directly and place TCHAR-sized delimiters.
	TCHAR * tmp = (TCHAR *)pMem; POSITION pos = GetHeadPosition();
	while( pos ) {
		CString & rString = GetNext(pos);
		INT nLength = rString.GetLength();
		memcpy(tmp, (LPCTSTR)rString, nLength * sizeof(TCHAR)); tmp += nLength;
		if( pos ) { * tmp++ = _T('\r'); * tmp++ = _T('\n'); }
		else { * tmp++ = _T('\0'); * tmp++ = _T('\0'); }
	}
	(void)size;
}


// CAnalyzedTextFile

// Decode one COMPLETE line's raw bytes into a CString.
//
// Decoding a whole line at a time — rather than each fixed-size read buffer — is what
// keeps multi-byte characters intact. A line break never falls inside a UTF-8
// sequence, a CP949 lead/trail pair, or a UTF-16 code unit; a 512-byte read boundary
// happily does. The previous implementation converted per read chunk, so any such
// character that straddled a chunk boundary was corrupted (only reachable on lines
// longer than the buffer, which is why it went unnoticed).
static void _DecodeLine(CString & rLine, const CHAR * pBytes, INT nBytes, INT nEncodingType)
{
	rLine.Empty();
	if( nBytes <= 0 || pBytes == NULL ) return;

	if( nEncodingType == ENCODING_TYPE_UNICODE_LE || nEncodingType == ENCODING_TYPE_UNICODE_BE ) {
		INT nChars = nBytes / 2;
		if( nChars <= 0 ) return;

		LPTSTR pDst = rLine.GetBuffer(nChars + 1);
		if( nEncodingType == ENCODING_TYPE_UNICODE_LE ) {
			memcpy(pDst, pBytes, (size_t)nChars * 2);
		} else {
			const UCHAR * p = (const UCHAR *)pBytes;
			for(INT i = 0; i < nChars; i++) pDst[i] = (TCHAR)((p[2*i] << 8) | p[2*i + 1]);
		}
		pDst[nChars] = 0;
		rLine.ReleaseBuffer(nChars);
		return;
	}

	// UTF-8 -> UTF-16 directly, no CP_ACP round-trip: that round-trip is what used to
	// turn an em dash or a CJK character outside CP949 into '?'.
	// ANSI stays on CP_ACP, which is what preserves CP949 Korean in legacy files.
	UINT nCodePage = ( nEncodingType == ENCODING_TYPE_UTF8_WBOM ||
	                   nEncodingType == ENCODING_TYPE_UTF8_XBOM ) ? CP_UTF8 : CP_ACP;

	INT nChars = MultiByteToWideChar(nCodePage, 0, pBytes, nBytes, NULL, 0);
	if( nChars <= 0 ) return;

	LPTSTR pDst = rLine.GetBuffer(nChars + 1);
	MultiByteToWideChar(nCodePage, 0, pBytes, nBytes, pDst, nChars);
	pDst[nChars] = 0;
	rLine.ReleaseBuffer(nChars);
}

BOOL CAnalyzedText::FileLoad(LPCTSTR lpszPathName, INT nEncodingType, INT nFileFormat)
{
	INT chDelim = '\n', chKill = '\r'; // FILE_FORMAT_DOS & FILE_FORMAT_UNIX
	if( nFileFormat == FILE_FORMAT_MAC ) { chDelim = '\r'; chKill = '\0'; }

	BOOL bUnicode = ( nEncodingType == ENCODING_TYPE_UNICODE_LE ||
	                  nEncodingType == ENCODING_TYPE_UNICODE_BE );
	BOOL bLittleEndian = ( nEncodingType == ENCODING_TYPE_UNICODE_LE );

	LONGLONG _perf = CedtPerfNow(); // [profiling] stage 1

	try {
		CFile file(lpszPathName, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone);
		RemoveAll();

		// Read in large blocks and consume EVERY line in each one.
		//
		// The old loop read a buffer, took only the text up to the FIRST line break in
		// it, and then seeked back so the next read would start at the next line. That
		// is one read and one seek per LINE — a 900,000-line file paid 900,000 of each,
		// and re-read most of the file several times over.
		const INT nBufSize = 1 << 16;			// 64 KiB
		std::vector<CHAR> vecBuf( nBufSize );
		CHAR * pBuf = & vecBuf[0];

		std::vector<CHAR> vecLine;				// raw bytes of the line being assembled
		vecLine.reserve( 4096 );

		CString szLine;

		// Skip the byte-order mark, if the encoding has one.
		INT nSkip = 0;
		{
			UCHAR bom[4] = { 0, 0, 0, 0 };
			INT n = file.Read( bom, 4 );
			if( nEncodingType == ENCODING_TYPE_UNICODE_LE && n >= 2 && bom[0] == 0xFF && bom[1] == 0xFE ) nSkip = 2;
			else if( nEncodingType == ENCODING_TYPE_UNICODE_BE && n >= 2 && bom[0] == 0xFE && bom[1] == 0xFF ) nSkip = 2;
			else if( ( nEncodingType == ENCODING_TYPE_UTF8_WBOM || nEncodingType == ENCODING_TYPE_UTF8_XBOM ) &&
			         n >= 3 && bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF ) nSkip = 3;
			file.Seek( nSkip, CFile::begin );
		}

		INT nCarry = 0;		// a dangling odd byte of a UTF-16 unit, held over to the next read

		for( ;; ) {
			INT nRead = file.Read( pBuf + nCarry, nBufSize - nCarry );
			if( nRead <= 0 ) break;

			INT nAvail = nCarry + nRead;
			nCarry = 0;

			// UTF-16 is scanned in 2-byte units, so an odd trailing byte cannot be
			// looked at yet; it goes back to the front of the buffer for the next read.
			INT nUsable = bUnicode ? (nAvail & ~1) : nAvail;

			INT nStart = 0;
			for( INT i = 0; i < nUsable; i += (bUnicode ? 2 : 1) ) {
				if( bUnicode ) {
					const UCHAR * p = (const UCHAR *)(pBuf + i);
					TCHAR wc = bLittleEndian ? (TCHAR)( p[0] | (p[1] << 8) )
					                         : (TCHAR)( (p[0] << 8) | p[1] );
					if( wc != (TCHAR)chDelim ) continue;
				} else {
					if( pBuf[i] != (CHAR)chDelim ) continue;
				}

				vecLine.insert( vecLine.end(), pBuf + nStart, pBuf + i );

				// Drop the trailing kill character ('\r' before '\n'). Doing it on the
				// ASSEMBLED line rather than in the read buffer is what makes a "\r\n"
				// split across a read boundary work — the old code let that '\r' through.
				if( chKill ) {
					if( bUnicode ) {
						size_t n = vecLine.size();
						if( n >= 2 ) {
							const UCHAR * q = (const UCHAR *)( & vecLine[n-2] );
							TCHAR wk = bLittleEndian ? (TCHAR)( q[0] | (q[1] << 8) )
							                         : (TCHAR)( (q[0] << 8) | q[1] );
							if( wk == (TCHAR)chKill ) vecLine.resize( n - 2 );
						}
					} else {
						if( ! vecLine.empty() && vecLine.back() == (CHAR)chKill ) vecLine.pop_back();
					}
				}

				_DecodeLine( szLine, vecLine.empty() ? NULL : & vecLine[0], (INT)vecLine.size(), nEncodingType );
				AddTail( (LPCTSTR)szLine );

				vecLine.clear();
				nStart = i + (bUnicode ? 2 : 1);
			}

			// Whatever is left is the start of a line that continues into the next read.
			vecLine.insert( vecLine.end(), pBuf + nStart, pBuf + nUsable );

			if( nAvail > nUsable ) { pBuf[0] = pBuf[nAvail-1]; nCarry = 1; }
		}

		// The final line: the text after the last line break (empty if the file ended
		// with one — matching the old behavior, which always left a blank tail line).
		if( chKill && ! bUnicode && ! vecLine.empty() && vecLine.back() == (CHAR)chKill ) vecLine.pop_back();
		_DecodeLine( szLine, vecLine.empty() ? NULL : & vecLine[0], (INT)vecLine.size(), nEncodingType );
		AddTail( (LPCTSTR)szLine );

		file.Close();

		ScrubLoneSurrogates();

	} catch( CException * ex ) {
		ex->ReportError( MB_OK | MB_ICONSTOP );
		ex->Delete(); return FALSE;
	}

	if( GetCount() > 1000 ) CedtPerfLog(_T("1.FileLoad"), _perf, (INT)GetCount()); // [profiling] stage 1

	return TRUE;
}

// Establish the invariant every surrogate-aware helper relies on: the document
// never holds an *unpaired* surrogate.
//
// The editing paths can no longer create one, but a malformed file can arrive
// with one already in it — a truncated UTF-16 file, or CESU-8 bytes that some
// UTF-8 decoders let through. An unpaired surrogate is not a legal character in
// any encoding, so replace it with U+FFFD (the replacement character), which is
// exactly what a conforming encoder would have written anyway.
//
// This must run per completed LINE, never per read chunk: a chunk boundary can
// fall between the two halves of a pair, and scrubbing at that level would
// mistake a perfectly good high surrogate for a lone one and destroy it.
void CAnalyzedText::ScrubLoneSurrogates()
{
#ifdef _UNICODE
	POSITION pos = GetHeadPosition();

	while( pos ) {
		CAnalyzedString & rLine = GetNext(pos);
		INT nLength = rLine.GetLength();

		for(INT i = 0; i < nLength; i++) {
			TCHAR ch = rLine[i];
			if( ! IsSurrogate(ch) ) continue;

			// A high surrogate is fine only if a low one follows it, and a low
			// surrogate is fine only if a high one precedes it. Anything else
			// is a half character.
			if( IsHighSurrogate(ch) && i + 1 < nLength && IsLowSurrogate(rLine[i + 1]) ) {
				i++;	// well-formed pair — skip past both halves
				continue;
			}

			rLine.SetAt(i, (TCHAR)0xFFFD);
		}
	}
#endif
}

BOOL CAnalyzedText::FileSave(LPCTSTR lpszPathName, INT nEncodingType, INT nFileFormat)
{
	CHAR szDelim[3]; lstrcpyA(szDelim, "\r\n"); INT nDelimSize = 2; // FILE_FORMAT_DOS
	if( nFileFormat == FILE_FORMAT_UNIX ) { lstrcpyA(szDelim, "\n"); nDelimSize = 1; }
	else if( nFileFormat == FILE_FORMAT_MAC ) { lstrcpyA(szDelim, "\r"); nDelimSize = 1; }
	
	try {
		CFile file(lpszPathName, CFile::modeReadWrite | CFile::modeCreate | CFile::shareExclusive);
		POSITION pos = GetHeadPosition();

		INT nBufferSize = 0; CHAR * pBuffer = NULL;
		UCHAR szWideDelim[4], * pWideBuffer = NULL;

		if( nEncodingType == ENCODING_TYPE_UNICODE_LE ) {
			// write byte-order mark
			static const UCHAR bomLE[2] = { 0xFF, 0xFE };
			file.Write(bomLE, 2);

			szWideDelim[0] = szDelim[0];	szWideDelim[1] = 0x00;
			szWideDelim[2] = szDelim[1];	szWideDelim[3] = 0x00;

			while( pos ) {
				CAnalyzedString & rLine = GetNext(pos);
				INT nLength = rLine.GetLength();

				// rLine is already UTF-16 LE; write its wide-char buffer directly.
				if( nLength ) file.Write( (LPCWSTR)rLine, 2 * nLength );
				if( pos ) file.Write( szWideDelim, 2 * nDelimSize );
			}
		} else if( nEncodingType == ENCODING_TYPE_UNICODE_BE ) {
			// write byte-order mark
			static const UCHAR bomBE[2] = { 0xFE, 0xFF };
			file.Write(bomBE, 2);

			szWideDelim[0] = 0x00; 	szWideDelim[1] = szDelim[0];
			szWideDelim[2] = 0x00; 	szWideDelim[3] = szDelim[1];

			while( pos ) {
				CAnalyzedString & rLine = GetNext(pos);
				INT nLength = rLine.GetLength();

				if( nBufferSize < nLength ) {
					nBufferSize = nLength;
					delete [] pWideBuffer;	pWideBuffer = new UCHAR[2 * (nBufferSize + 1)];
				}

				// Copy the wide chars, then swap each pair to big-endian.
				if( nLength ) {
					memcpy(pWideBuffer, (LPCWSTR)rLine, 2 * nLength);
					for( INT i = 0; i < nLength; i++ ) _SWAP_UCHAR( pWideBuffer[2*i], pWideBuffer[2*i+1] );
					file.Write( pWideBuffer, 2 * nLength );
				}
				if( pos ) file.Write( szWideDelim, 2 * nDelimSize );
			}
		} else if( nEncodingType == ENCODING_TYPE_UTF8_WBOM || nEncodingType == ENCODING_TYPE_UTF8_XBOM ) {
			// write byte-order mark when it is not ENCODING_TYPE_UTF8_XBOM
			if( nEncodingType != ENCODING_TYPE_UTF8_XBOM ) {
				static const UCHAR bomUTF8[3] = { 0xEF, 0xBB, 0xBF };
				file.Write(bomUTF8, 3);
			}

			while( pos ) {
				CAnalyzedString & rLine = GetNext(pos);
				INT nLength = rLine.GetLength();

				if( nBufferSize < nLength ) {
					nBufferSize = nLength;
					delete [] pBuffer;		pBuffer = new CHAR[3 * (nBufferSize + 1) + 1];
				}

				// UTF-16 → UTF-8 directly, no CP_ACP round-trip. WideCharToMultiByte
				// asks for length via -1 only when the source is null-terminated,
				// which (LPCWSTR)rLine is; we count 3× worst-case bytes per char.
				INT nBytes = 0;
				if( nLength ) {
					nBytes = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)rLine, nLength,
						pBuffer, 3 * (nBufferSize + 1), NULL, NULL);
					if( nBytes > 0 ) file.Write( pBuffer, nBytes );
				}
				if( pos ) file.Write( szDelim, nDelimSize );
			}
		} else { /* nEncodingType == ENCODING_TYPE_ASCII */
			while( pos ) {
				CAnalyzedString & rLine = GetNext(pos);
				INT nLength = rLine.GetLength();

				if( nBufferSize < nLength ) {
					nBufferSize = nLength;
					delete [] pBuffer;		pBuffer = new CHAR[2 * (nBufferSize + 1) + 1];
				}

				// UTF-16 → CP_ACP for legacy MBCS on-disk format. Non-ANSI
				// characters get replaced by '?' by design — same behavior
				// as the pre-Unicode ASCII save path.
				if( nLength ) {
					INT nBytes = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)rLine, nLength,
						pBuffer, 2 * (nBufferSize + 1), NULL, NULL);
					if( nBytes > 0 ) file.Write( pBuffer, nBytes );
				}
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
	m_szName = m_szCommand = _T("");
	m_szArgument = m_szDirectory = _T("");
	m_bCloseOnExit = TRUE; m_bUseShortFileName = FALSE;
	m_bCaptureOutput = FALSE; m_bSaveBeforeExecute = TRUE;
}

CString CUserCommand::GetHotKeyText()
{
	// GetKeyNameText returns 0 when the scan code is invalid (default
	// m_wVirtualKeyCode = 0 hits this path). The pre-Unicode code left
	// szKeyName uninitialized and trusted _tcslen — which happened to
	// return 0 on MBCS stack patterns but returns garbage under Unicode.
	// Trust the API's return value instead.
	UINT nScanCode = MapVirtualKey( m_wVirtualKeyCode, 0 );
	LPARAM lParam = nScanCode << 16;
	TCHAR szKeyName[1024];
	INT nLen = GetKeyNameText( (LONG)lParam, szKeyName, 1024 );
	if( nLen <= 0 ) return _T("");

	CString szHotKeyText;
	if( m_wModifiers & HOTKEYF_CONTROL ) szHotKeyText += _T("Ctrl+");
	if( m_wModifiers & HOTKEYF_ALT ) szHotKeyText += _T("Alt+");
	if( m_wModifiers & HOTKEYF_SHIFT ) szHotKeyText += _T("Shift+");
	szHotKeyText += szKeyName;
	return szHotKeyText;
}

BOOL CUserCommand::StreamSave(ofstream & fout)
{
	INT nLength;

	fout.write((const char *)(& m_wVirtualKeyCode), sizeof(WORD));
	fout.write((const char *)(& m_wModifiers), sizeof(WORD));

	nLength = m_szName.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	_WRITE_WIDE_STR(fout, (LPCTSTR)m_szName, nLength);

	nLength = m_szCommand.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	_WRITE_WIDE_STR(fout, (LPCTSTR)m_szCommand, nLength);

	nLength = m_szArgument.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	_WRITE_WIDE_STR(fout, (LPCTSTR)m_szArgument, nLength);

	nLength = m_szDirectory.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	_WRITE_WIDE_STR(fout, (LPCTSTR)m_szDirectory, nLength);

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
	_READ_WIDE_STR(fin, szBuffer, nLength);
	szBuffer[nLength] = _T('\0'); m_szName = szBuffer;

	fin.read((char *)(& nLength), sizeof(nLength));
	_READ_WIDE_STR(fin, szBuffer, nLength);
	szBuffer[nLength] = _T('\0'); m_szCommand = szBuffer;

	fin.read((char *)(& nLength), sizeof(nLength));
	_READ_WIDE_STR(fin, szBuffer, nLength);
	szBuffer[nLength] = _T('\0'); m_szArgument = szBuffer;

	fin.read((char *)(& nLength), sizeof(nLength));
	_READ_WIDE_STR(fin, szBuffer, nLength);
	szBuffer[nLength] = _T('\0'); m_szDirectory = szBuffer;

	fin.read((char *)(& m_bCloseOnExit), sizeof(BOOL));
	fin.read((char *)(& m_bUseShortFileName), sizeof(BOOL));
	fin.read((char *)(& m_bCaptureOutput), sizeof(BOOL));
	fin.read((char *)(& m_bSaveBeforeExecute), sizeof(BOOL));

	return TRUE;
}

void CUserCommand::DeleteContents()
{
	m_wVirtualKeyCode = m_wModifiers = 0x00;
	m_szName = m_szCommand = _T("");
	m_szArgument = m_szDirectory = _T("");
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
	m_szName = _T("");
	m_lstAction.RemoveAll();
	m_lstParam.RemoveAll();
	m_lstFlags.RemoveAll();
	m_lstString.RemoveAll();
}

CString CMacroBuffer::GetHotKeyText()
{
	// Same uninitialized-szKeyName bug as CUserCommand::GetHotKeyText.
	UINT nScanCode = MapVirtualKey( m_wVirtualKeyCode, 0 );
	LPARAM lParam = nScanCode << 16;
	TCHAR szKeyName[1024];
	INT nLen = GetKeyNameText( (LONG)lParam, szKeyName, 1024 );
	if( nLen <= 0 ) return _T("");

	CString szHotKeyText;
	if( m_wModifiers & HOTKEYF_CONTROL ) szHotKeyText += _T("Ctrl+");
	if( m_wModifiers & HOTKEYF_ALT ) szHotKeyText += _T("Alt+");
	if( m_wModifiers & HOTKEYF_SHIFT ) szHotKeyText += _T("Shift+");
	szHotKeyText += szKeyName;
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
	_WRITE_WIDE_STR(fout, (LPCTSTR)m_szName, nLength);

	nCount = (INT)m_lstAction.GetCount(); pos = m_lstAction.GetHeadPosition();
	fout.write((const char *)(& nCount), sizeof(nCount));
	while( pos ) { 
		nAction = m_lstAction.GetNext(pos); 
		fout.write((const char *)(& nAction), sizeof(nAction));
	}

	nCount = (INT)m_lstParam.GetCount(); pos = m_lstParam.GetHeadPosition();
	fout.write((const char *)(& nCount), sizeof(nCount));
	while( pos ) { 
		nParam = m_lstParam.GetNext(pos); 
		fout.write((const char *)(& nParam), sizeof(nParam));
	}

	nCount = (INT)m_lstFlags.GetCount(); pos = m_lstFlags.GetHeadPosition();
	fout.write((const char *)(& nCount), sizeof(nCount));
	while( pos ) { 
		nFlags = m_lstFlags.GetNext(pos); 
		fout.write((const char *)(& nFlags), sizeof(nFlags));
	}

	nCount = (INT)m_lstString.GetCount(); pos = m_lstString.GetHeadPosition();
	fout.write((const char *)(& nCount), sizeof(nCount));
	while( pos ) { 
		szString = m_lstString.GetNext(pos); nLength = szString.GetLength();
		fout.write((const char *)(& nLength), sizeof(nLength));
		_WRITE_WIDE_STR(fout, (LPCTSTR)szString, nLength);
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
	_READ_WIDE_STR(fin, szBuffer, nLength);
	szBuffer[nLength] = _T('\0'); m_szName = szBuffer;

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
		_READ_WIDE_STR(fin, szBuffer, nLength);
		szBuffer[nLength] = _T('\0');
		m_lstString.AddTail(szBuffer);
	}

	return TRUE;
}

void CMacroBuffer::DeleteContents()
{
	m_wVirtualKeyCode = m_wModifiers = 0x00;
	m_szName = _T("");
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
	_WRITE_WIDE_STR(fout, (LPCTSTR)m_szDescription, nLength);

	nLength = m_szExtensions.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	_WRITE_WIDE_STR(fout, (LPCTSTR)m_szExtensions, nLength);

	nLength = m_szDefaultExt.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	_WRITE_WIDE_STR(fout, (LPCTSTR)m_szDefaultExt, nLength);

	return TRUE;
}

BOOL CFileFilter::StreamLoad(ifstream & fin)
{
	INT nLength; TCHAR szBuffer[4096];

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength < 0 || nLength >= 4096 ) return FALSE;
	_READ_WIDE_STR(fin, szBuffer, nLength);
	szBuffer[nLength] = _T('\0'); m_szDescription = szBuffer;

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength < 0 || nLength >= 4096 ) return FALSE;
	_READ_WIDE_STR(fin, szBuffer, nLength);
	szBuffer[nLength] = _T('\0'); m_szExtensions = szBuffer;

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength < 0 || nLength >= 4096 ) return FALSE;
	_READ_WIDE_STR(fin, szBuffer, nLength);
	szBuffer[nLength] = _T('\0'); m_szDefaultExt = szBuffer;

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
	m_szDescription = _T("");
	m_szExtensions = _T("");
	m_szDefaultExt = _T("");
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
	_WRITE_WIDE_STR(fout, (LPCTSTR)m_szDescription, nLength);

	nLength = m_szLangSpecFile.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	_WRITE_WIDE_STR(fout, (LPCTSTR)m_szLangSpecFile, nLength);

	nLength = m_szKeywordsFile.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	_WRITE_WIDE_STR(fout, (LPCTSTR)m_szKeywordsFile, nLength);

	return TRUE;
}

BOOL CSyntaxType::StreamLoad(ifstream & fin)
{
	INT nLength; TCHAR szBuffer[4096];

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength < 0 || nLength >= 4096 ) return FALSE;
	_READ_WIDE_STR(fin, szBuffer, nLength);
	szBuffer[nLength] = _T('\0'); m_szDescription = szBuffer;

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength < 0 || nLength >= 4096 ) return FALSE;
	_READ_WIDE_STR(fin, szBuffer, nLength);
	szBuffer[nLength] = _T('\0'); m_szLangSpecFile = szBuffer;

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength < 0 || nLength >= 4096 ) return FALSE;
	_READ_WIDE_STR(fin, szBuffer, nLength);
	szBuffer[nLength] = _T('\0'); m_szKeywordsFile = szBuffer;

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
	m_szDescription = _T("");
	m_szLangSpecFile = _T("");
	m_szKeywordsFile = _T("");
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
	m_szDescription = m_szServerName = _T("");
	m_szUserName = m_szPassword = _T("");
	m_szSubDirectory = _T("");

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
	_WRITE_WIDE_STR(fout, (LPCTSTR)m_szDescription, nLength);

	nLength = m_szServerName.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	_WRITE_WIDE_STR(fout, (LPCTSTR)m_szServerName, nLength);

	nLength = m_szUserName.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	_WRITE_WIDE_STR(fout, (LPCTSTR)m_szUserName, nLength);

	CString szEncodedPassword = _T("");
	if( m_bSavePassword ) szEncodedPassword = CString(CA2T(map_encode((LPCSTR)CT2A((LPCTSTR)m_szPassword))));

	nLength = szEncodedPassword.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	_WRITE_WIDE_STR(fout, (LPCTSTR)szEncodedPassword, nLength);

	nLength = m_szSubDirectory.GetLength();
	fout.write((const char *)(& nLength), sizeof(nLength));
	_WRITE_WIDE_STR(fout, (LPCTSTR)m_szSubDirectory, nLength);

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
	_READ_WIDE_STR(fin, szBuffer, nLength); szBuffer[nLength] = _T('\0');
	m_szDescription = szBuffer;

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength < 0 || nLength > 2048 ) return FALSE;
	_READ_WIDE_STR(fin, szBuffer, nLength); szBuffer[nLength] = _T('\0');
	m_szServerName = szBuffer;

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength < 0 || nLength > 2048 ) return FALSE;
	_READ_WIDE_STR(fin, szBuffer, nLength); szBuffer[nLength] = _T('\0');
	m_szUserName = szBuffer;

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength < 0 || nLength > 2048 ) return FALSE;
	_READ_WIDE_STR(fin, szBuffer, nLength); szBuffer[nLength] = _T('\0');

	CString szEncodedPassword = szBuffer;
	m_szPassword = CString(CA2T(map_decode((LPCSTR)CT2A((LPCTSTR)szEncodedPassword))));

	fin.read((char *)(& nLength), sizeof(nLength));
	if( nLength < 0 || nLength > 2048 ) return FALSE;
	_READ_WIDE_STR(fin, szBuffer, nLength); szBuffer[nLength] = _T('\0');
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
			if( m_szUserName.GetLength() ) szDisplayName.Format(_T("%s [ftp://%s@%s]"), m_szDescription, m_szUserName, m_szServerName);
			else szDisplayName.Format(_T("%s [ftp://%s]"), m_szDescription, m_szServerName);
		} else szDisplayName = m_szDescription;
	} else szDisplayName = _T("- Empty -");
	return szDisplayName;
}

CString CFtpAccount::GetFullAccountName()
{
	CString szAccountName;
	if( m_szServerName.GetLength() ) {
		if( m_szUserName.GetLength() ) szAccountName.Format(_T("ftp://%s@%s"), m_szUserName, m_szServerName);
		else szAccountName.Format(_T("ftp://%s"), m_szServerName);
	} else szAccountName = _T("");
	return szAccountName;
}

CString CFtpAccount::GetShortAccountName()
{
	CString szAccountName;
	if( m_szServerName.GetLength() ) {
		if( m_szUserName.GetLength() ) szAccountName.Format(_T("%s@%s"), m_szUserName, m_szServerName);
		else szAccountName.Format(_T("%s"), m_szServerName);
	} else szAccountName = _T("");
	return szAccountName;
}

void CFtpAccount::DeleteContents()
{
	m_szDescription = m_szServerName = _T("");
	m_szUserName = m_szPassword = _T("");
	m_szSubDirectory = _T("");

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


#include "stdafx.h"
#include "cedtHeader.h"


// temporary variables
static ANALYZEDWORD _words[MAX_WORDS_COUNT+1];


// global language settings
static CKeywords * _pKEY;
static CDictionary * _pDIC;


// global language specification
static TCHAR VQU, ESC, MQU, QU1, QU2, QU3;

static LPCTSTR	DEL, HEX, PRE, VAR, VEB, SVC, HRD;
static LPCTSTR	LF1, LF2, LC1, LC2, C1B, C1E, C2B, C2E;
static LPCTSTR	SDB, SDE, HLB, HLE, R1B, R1E, R2B, R2E;

static UCHAR rngMQU, rngQU1, rngQU2, rngQU3;
static UCHAR rngLC1, rngLC2, rngBC1, rngBC2;

static INT lenHEX, lenHRD;
static INT lenLF1, lenLF2, lenLC1, lenLC2, lenC1B, lenC1E, lenC2B, lenC2E;
static INT lenSDB, lenSDE, lenHLB, lenHLE, lenR1B, lenR1E, lenR2B, lenR2E;


// global language options
static BOOL bKEY, bDIC, bQUO, bCOM, bRNG, bPRE, bVAR;


// Character classification table, rebuilt from the language spec at the top of
// AnalyzeText.
//
// The tokenizer used to answer two questions the slow way, and it asked them a lot:
//
//   "is this character a delimiter?"  ->  _tcschr(DEL, ch), a linear scan of the
//   delimiter string, run for EVERY character of every identifier.
//
//   "does a comment / quote / range marker start here?"  ->  up to two dozen
//   _tcsnicmp calls, run at EVERY word boundary, almost always to conclude "no".
//
// Both collapse to one array lookup. The marker bit says "some marker in this
// language begins with this character" — if it is clear, the whole cascade of string
// compares in states 0x0200 / 0x0300 / 0x0350 provably cannot match, so it is skipped
// outright. Ordinary code is nearly all non-marker characters, so that is the common
// case.
// The classification bits also cover the CRT character tests. Under Unicode _istspace
// and friends are iswspace and friends: locale-aware CRT calls, not inlined, and the
// identifier scan runs one for EVERY character of every word — roughly 100 million
// calls to open a 900,000-line file. They are language-independent, so they are baked
// once; only the DELIM/MARKER bits, which come from the language spec, are rebuilt.
//
// (This does assume the C locale does not change under us mid-session. It does not:
// the app sets it up once at startup.)
#define _CT_DELIM		0x01
#define _CT_MARKER		0x02
#define _CT_SPACE		0x04
#define _CT_PRINT		0x08
#define _CT_ALPHA		0x10
#define _CT_DIGIT		0x20
#define _CT_XDIGIT		0x40

static UCHAR _charTable[0x10000];
static BOOL  _bCtypeBuilt = FALSE;

static void _BuildCtypeBitsOnce()
{
	if( _bCtypeBuilt ) return;
	_bCtypeBuilt = TRUE;

	for(INT i = 0; i < 0x10000; i++) {
		TCHAR c = (TCHAR)i;
		UCHAR f = 0;

		if( _istspace(c)  ) f |= _CT_SPACE;
		if( _istprint(c)  ) f |= _CT_PRINT;
		if( _istalpha(c)  ) f |= _CT_ALPHA;
		if( _istdigit(c)  ) f |= _CT_DIGIT;
		if( _istxdigit(c) ) f |= _CT_XDIGIT;

		_charTable[i] = f;
	}
}

static void _MarkFirstChar(LPCTSTR psz)
{
	if( psz && psz[0] ) _charTable[(unsigned short)psz[0]] |= _CT_MARKER;
}

static void _BuildCharTable()
{
	_BuildCtypeBitsOnce();

	// Clear only the language-dependent bits; the ctype bits above stay put.
	for(INT i = 0; i < 0x10000; i++) _charTable[i] &= ~(_CT_DELIM | _CT_MARKER);

	for(LPCTSTR p = DEL; p && * p; p++) _charTable[(unsigned short)(* p)] |= _CT_DELIM;

	if( ESC ) _charTable[(unsigned short)ESC] |= _CT_MARKER;
	if( MQU ) _charTable[(unsigned short)MQU] |= _CT_MARKER;
	if( QU1 ) _charTable[(unsigned short)QU1] |= _CT_MARKER;
	if( QU2 ) _charTable[(unsigned short)QU2] |= _CT_MARKER;
	if( QU3 ) _charTable[(unsigned short)QU3] |= _CT_MARKER;

	_MarkFirstChar(HRD);
	_MarkFirstChar(LF1);	_MarkFirstChar(LF2);
	_MarkFirstChar(LC1);	_MarkFirstChar(LC2);
	_MarkFirstChar(C1B);	_MarkFirstChar(C1E);
	_MarkFirstChar(C2B);	_MarkFirstChar(C2E);
	_MarkFirstChar(SDB);	_MarkFirstChar(SDE);
	_MarkFirstChar(HLB);	_MarkFirstChar(HLE);
	_MarkFirstChar(R1B);	_MarkFirstChar(R1E);
	_MarkFirstChar(R2B);	_MarkFirstChar(R2E);
}

// NUL is never in the table, so these are safe on the string terminator — which is
// more than _tcschr(DEL, 0) was, since that returns a pointer to DEL's own terminator.
#define _IS_DELIM(ch)	( _charTable[(unsigned short)(ch)] & _CT_DELIM )
#define _IS_MARKER(ch)	( _charTable[(unsigned short)(ch)] & _CT_MARKER )
#define _IS_SPACE(ch)	( _charTable[(unsigned short)(ch)] & _CT_SPACE )
#define _IS_PRINT(ch)	( _charTable[(unsigned short)(ch)] & _CT_PRINT )
#define _IS_ALPHA(ch)	( _charTable[(unsigned short)(ch)] & _CT_ALPHA )
#define _IS_DIGIT(ch)	( _charTable[(unsigned short)(ch)] & _CT_DIGIT )
#define _IS_XDIGIT(ch)	( _charTable[(unsigned short)(ch)] & _CT_XDIGIT )


// Callers pass pointer-arithmetic results (ptrdiff_t / INT_PTR on x64)
// for siIndex/siLength. The per-line analyzer caps line length at
// MAX_STRING_LENGTH (32767) so the values fit in a SHORT — the narrowing
// happens explicitly here at the boundary, not implicitly at every call site.
static void _WordFound(SHORT windex, UCHAR ucType, UCHAR ucInfo, INT_PTR siIndex, INT_PTR siLength)
{
	_words[windex].m_ucType[0] = ucType;
	_words[windex].m_ucType[1] = ucType;
	_words[windex].m_ucType[2] = ucType;
	_words[windex].m_ucInfo = ucInfo;
	_words[windex].m_siIndex = (SHORT)siIndex;
	_words[windex].m_siLength = (SHORT)siLength;

//	TRACE2("Word Found: %d, %d\n", windex, siLength);
}

static void _WordFoundExtended(SHORT windex, UCHAR ucType[], UCHAR ucInfo, INT_PTR siIndex, INT_PTR siLength)
{
	_words[windex].m_ucType[0] = ucType[0];
	_words[windex].m_ucType[1] = ucType[1];
	_words[windex].m_ucType[2] = ucType[2];
	_words[windex].m_ucInfo = ucInfo;
	_words[windex].m_siIndex = (SHORT)siIndex;
	_words[windex].m_siLength = (SHORT)siLength;

//	TRACE2("Word Found: %d, %d\n", windex, siLength);
}

static void _FinishLine(SHORT wcount, BOOL bOverflow, CAnalyzedString & rLine)
{
	BOOL bBookmark = rLine.m_usLineInfo & LI_HAVEBOOKMARK;

	delete [] rLine.m_pWord; rLine.m_pWord = new ANALYZEDWORD[wcount];
	memcpy(rLine.m_pWord, _words, wcount * sizeof(ANALYZEDWORD));

	rLine.m_siWordCount = wcount;
	rLine.m_usLineInfo = 0x0000;

	rLine.m_usLineInfo |= bBookmark ? LI_HAVEBOOKMARK : 0x0000;
	rLine.m_usLineInfo |= bOverflow ? LI_HAVEOVERFLOW : 0x0000;

	for(SHORT i = 0; i < wcount; i++) {
		switch( _words[i].m_ucType[0] ) {
		case WT_RANGE1BEG:		case WT_RANGE1END:		case WT_RANGE2BEG:		case WT_RANGE2END: 
			rLine.m_usLineInfo |= LI_HAVERANGE; break;
		case WT_SHADOWON:		case WT_SHADOWOFF:		case WT_HIGHLIGHTON:	case WT_HIGHLIGHTOFF:
			rLine.m_usLineInfo |= LI_HAVEHIGHLIGHT; break;
		case WT_LINECOMMENT:	case WT_COMMENT1ON:		case WT_COMMENT1OFF:	case WT_COMMENT2ON:		case WT_COMMENT2OFF:
			rLine.m_usLineInfo |= LI_HAVECOMMENT; break;
		case WT_QUOTATION0:		case WT_QUOTATION1:		case WT_QUOTATION2:		case WT_QUOTATION3:
			rLine.m_usLineInfo |= LI_HAVEQUOTATION; break;
		case WT_HEREDOCUMENT:
			rLine.m_usLineInfo |= LI_HAVEHEREDOCUMENT; break;
		}
	}
}


#define _NEXT_WORD(addr)		{ beg = fwd; state = (addr); }
#define _ROLL_BACK(addr)		{ fwd = beg; state = (addr); }
#define _JUMP_ADDR(addr)		{            state = (addr); }

#define _CHCK_SIZE(ptr, len)	( ptr - str < MAX_STRING_LENGTH - (len) )

static void _AnalyzeLine(CAnalyzedString & rLine) 
{
	INT is_finished = 0, state = 0; SHORT wcount = 0;
	UCHAR type[3], info; TCHAR * str, * beg, * fwd;

	str = beg = fwd = (TCHAR *)(LPCTSTR)rLine;

	while( ! is_finished ) {
		switch( state ) {
		
		case 0x0000: // CHECK EOS, OVERFLOW AND WHITE SPACE
			if( * fwd == '\0' ) {
				// end of line
				_WordFound(wcount++, WT_RETURN, RT_GLOBAL, beg-str, fwd-beg);
				is_finished =  1;
			} else if( fwd - str >= MAX_STRING_LENGTH-1 ) {
				// line overflow
				_WordFound(wcount++, WT_RETURN, RT_GLOBAL, beg-str, fwd-beg);
				is_finished = -1;
			} else if( * fwd == '\t' && _CHCK_SIZE(fwd, 1) ) {
				fwd++; 
				_WordFound(wcount++, WT_TAB, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( * fwd == ' ' && _CHCK_SIZE(fwd, 1) ) {
				fwd++; while( * fwd && * fwd == ' ' && _CHCK_SIZE(fwd, 1) ) fwd++;
				_WordFound(wcount++, WT_SPACE, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else {
				_ROLL_BACK(bQUO ? 0x0200 : (bCOM ? 0x0300 : (bRNG ? 0x0350 : 0x0400)));
			}
			break;


		case 0x0200: // CHECK QUOTATION MARK & ESCAPE CHARACTERS
			// Every alternative below starts with a character the table has flagged as
			// a marker, so if this one is not flagged, none of them can match.
			if( ! _IS_MARKER(* fwd) ) { _ROLL_BACK(bCOM ? 0x0300 : (bRNG ? 0x0350 : 0x0400)); break; }

			if( ESC && * fwd == ESC && * (fwd+1) && * (fwd+1) != '\t' && * (fwd+1) != ' ' && _CHCK_SIZE(fwd, 2) ) {
				// Consume the escape char plus the character it escapes. That
				// character may be astral (a surrogate pair = 2 code units), so
				// a blind fwd += 2 would swallow only its high half and leave
				// the low half to be tokenized as a separate word — tearing the
				// pair in two. Reading fwd[2] is safe: fwd[1] is known non-null.
				fwd += 1 + ( ( IsHighSurrogate(fwd[1]) && IsLowSurrogate(fwd[2]) ) ? 2 : 1 );
				_WordFound(wcount++, WT_CONSTANT, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( HRD[0] && ! _tcsnicmp(fwd, HRD, lenHRD) && _CHCK_SIZE(fwd, lenHRD) ) {
				fwd += lenHRD;
				_WordFound(wcount++, WT_HEREDOCUMENT, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( MQU && * fwd == MQU && _CHCK_SIZE(fwd, 1) ) {
				fwd++; 
				_WordFound(wcount++, WT_QUOTATION0, rngMQU, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( QU1 && * fwd == QU1 && _CHCK_SIZE(fwd, 1) ) {
				fwd++; 
				_WordFound(wcount++, WT_QUOTATION1, rngQU1, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( QU2 && * fwd == QU2 && _CHCK_SIZE(fwd, 1) ) {
				fwd++;
				_WordFound(wcount++, WT_QUOTATION2, rngQU2, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( QU3 && * fwd == QU3 && _CHCK_SIZE(fwd, 1) ) {
				fwd++;
				_WordFound(wcount++, WT_QUOTATION3, rngQU3, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else {
				_ROLL_BACK(bCOM ? 0x0300 : (bRNG ? 0x0350 : 0x0400));
			}
			break;


		case 0x0300: // CHECK COMMENT DELIMITERS
			// Same gate: no comment marker in this language begins with a character the
			// table has not flagged, so skip all eight string compares.
			if( ! _IS_MARKER(* fwd) ) { _ROLL_BACK(bRNG ? 0x0350 : 0x0400); break; }

			if( C1B[0] && ! _tcsnicmp(fwd, C1B, lenC1B) && _CHCK_SIZE(fwd, lenC1B) ) {
				fwd += lenC1B; 
				_WordFound(wcount++, WT_COMMENT1ON, rngBC1, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( C1E[0] && ! _tcsnicmp(fwd, C1E, lenC1E) && _CHCK_SIZE(fwd, lenC1E) ) {
				fwd += lenC1E; 
				_WordFound(wcount++, WT_COMMENT1OFF, rngBC1, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( C2B[0] && ! _tcsnicmp(fwd, C2B, lenC2B) && _CHCK_SIZE(fwd, lenC2B) ) {
				fwd += lenC2B; 
				_WordFound(wcount++, WT_COMMENT2ON, rngBC2, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( C2E[0] && ! _tcsnicmp(fwd, C2E, lenC2E) && _CHCK_SIZE(fwd, lenC2E) ) {
				fwd += lenC2E; 
				_WordFound(wcount++, WT_COMMENT2OFF, rngBC2, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( LC1[0] && _IS_ALPHA(LC1[0]) && ! _tcsnicmp(fwd, LC1, lenLC1) && ! _IS_ALPHA(fwd[lenLC1]) && _CHCK_SIZE(fwd, lenLC1) ) {
				fwd += lenLC1; 
				_WordFound(wcount++, WT_LINECOMMENT, rngLC1, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( LC1[0] && ! _IS_ALPHA(LC1[0]) && ! _tcsnicmp(fwd, LC1, lenLC1) && _CHCK_SIZE(fwd, lenLC1) ) {
				fwd += lenLC1; 
				_WordFound(wcount++, WT_LINECOMMENT, rngLC1, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( LC2[0] && _IS_ALPHA(LC2[0]) && ! _tcsnicmp(fwd, LC2, lenLC2) && ! _IS_ALPHA(fwd[lenLC2]) && _CHCK_SIZE(fwd, lenLC2) ) {
				fwd += lenLC2; 
				_WordFound(wcount++, WT_LINECOMMENT, rngLC2, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( LC2[0] && ! _IS_ALPHA(LC2[0]) && ! _tcsnicmp(fwd, LC2, lenLC2) && _CHCK_SIZE(fwd, lenLC2) ) {
				fwd += lenLC2; 
				_WordFound(wcount++, WT_LINECOMMENT, rngLC2, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( LF1[0] && fwd == str && ! _tcsnicmp(fwd, LF1, lenLF1) && _CHCK_SIZE(fwd, lenLF1) ) {
				fwd += lenLF1; 
				_WordFound(wcount++, WT_LINECOMMENT, rngLC1, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( LF2[0] && fwd == str && ! _tcsnicmp(fwd, LF2, lenLF2) && _CHCK_SIZE(fwd, lenLF2) ) {
				fwd += lenLF2; 
				_WordFound(wcount++, WT_LINECOMMENT, rngLC2, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else {
				_ROLL_BACK(bRNG ? 0x0350 : 0x0400);
			}
			break;

		case 0x0350: // CHECK VARIOUS DELIMITERS
			// Same gate for the highlight / shadow / range markers.
			if( ! _IS_MARKER(* fwd) ) { _ROLL_BACK(0x0400); break; }

			if( HLB[0] && ! _tcsnicmp(fwd, HLB, lenHLB) && _CHCK_SIZE(fwd, lenHLB) ) {
				fwd += lenHLB; 
				_WordFound(wcount++, WT_HIGHLIGHTON, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( HLE[0] && ! _tcsnicmp(fwd, HLE, lenHLE) && _CHCK_SIZE(fwd, lenHLE) ) {
				fwd += lenHLE; 
				_WordFound(wcount++, WT_HIGHLIGHTOFF, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( SDB[0] && ! _tcsnicmp(fwd, SDB, lenSDB) && _CHCK_SIZE(fwd, lenSDB) ) {
				fwd += lenSDB; 
				_WordFound(wcount++, WT_SHADOWON, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( SDE[0] && ! _tcsnicmp(fwd, SDE, lenSDE) && _CHCK_SIZE(fwd, lenSDE) ) {
				fwd += lenSDE; 
				_WordFound(wcount++, WT_SHADOWOFF, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( R2B[0] && ! _tcsnicmp(fwd, R2B, lenR2B) && _CHCK_SIZE(fwd, lenR2B) ) {
				fwd += lenR2B; 
				_WordFound(wcount++, WT_RANGE2BEG, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( R2E[0] && ! _tcsnicmp(fwd, R2E, lenR2E) && _CHCK_SIZE(fwd, lenR2E) ) {
				fwd += lenR2E; 
				_WordFound(wcount++, WT_RANGE2END, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( R1B[0] && ! _tcsnicmp(fwd, R1B, lenR1B) && _CHCK_SIZE(fwd, lenR1B) ) {
				fwd += lenR1B; 
				_WordFound(wcount++, WT_RANGE1BEG, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( R1E[0] && ! _tcsnicmp(fwd, R1E, lenR1E) && _CHCK_SIZE(fwd, lenR1E) ) {
				fwd += lenR1E; 
				_WordFound(wcount++, WT_RANGE1END, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else {
				_ROLL_BACK(0x0400);
			}
			break;


		case 0x0400: // CHECK HEXADECIMAL & DECIMAL NUMBERS AND FLOATING POINT NUMBERS
			if( HEX[0] && ! _tcsnicmp(fwd, HEX, lenHEX) && _CHCK_SIZE(fwd, lenHEX) ) {
				fwd += lenHEX; while( * fwd && _IS_XDIGIT(* fwd) && _CHCK_SIZE(fwd, 1) ) fwd++;
				_WordFound(wcount++, WT_CONSTANT, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( _IS_DIGIT(* fwd) && _CHCK_SIZE(fwd, 1) ) {
				fwd++; while( * fwd && (_IS_DIGIT(* fwd) || * fwd == '.') && _CHCK_SIZE(fwd, 1) ) fwd++;
				_JUMP_ADDR(0x0401);
			} else {
				_ROLL_BACK(bPRE ? 0x0500 : (bVAR ? 0x0600 : 0x0700));
			}
			break;

		case 0x0401:
			if( * fwd && (* fwd == 'E' || * fwd == 'e') && _CHCK_SIZE(fwd, 1) ) {
				fwd++; 
				_JUMP_ADDR(0x0402);
			} else {
				_WordFound(wcount++, WT_CONSTANT, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			}
			break;

		case 0x0402:
			if( * fwd && (* fwd == '+' || * fwd == '-' || _IS_DIGIT(* fwd)) && _CHCK_SIZE(fwd, 1) ) {
				fwd++; while( * fwd && (_IS_DIGIT(* fwd)) && _CHCK_SIZE(fwd, 1) ) fwd++;
				_WordFound(wcount++, WT_CONSTANT, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else {
				_WordFound(wcount++, WT_CONSTANT, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			}
			break;


		case 0x0500: // CHECK KEYWORD BEGINNING WITH PREFIX
			if( PRE[0] && _tcschr(PRE, * fwd) && _CHCK_SIZE(fwd, 1) ) {
				fwd++; 
				_JUMP_ADDR(0x0501);
			} else {
				_ROLL_BACK(bVAR ? 0x0600 : 0x0700);
			}
			break;

		case 0x0501:
			if( bKEY && _pKEY->LookupTable(type, info, beg, fwd-beg) ) {
				_WordFoundExtended(wcount++, type, info, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( * fwd && _tcschr( PRE, * fwd ) && _CHCK_SIZE(fwd, 1) ) {
				fwd++;
				_JUMP_ADDR(0x0501);
			} else {
				while( * fwd && ! _IS_SPACE(* fwd) && ! _IS_DELIM(* fwd) && _CHCK_SIZE(fwd, 1) ) fwd++;
				_JUMP_ADDR(0x0502);
			}
			break;

		case 0x0502:
			if( bKEY && _pKEY->LookupTable(type, info, beg, fwd-beg) ) {
				_WordFoundExtended(wcount++, type, info, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else {
				_ROLL_BACK(bVAR ? 0x0600 : 0x0700);
			}
			break;


		case 0x0600: // CHECK VARIABLE BEGINING WITH PREFIX
			if( VAR[0] && _tcschr(VAR, * fwd) && _CHCK_SIZE(fwd, 1) ) {
				fwd++;
				_JUMP_ADDR(0x0601);
			} else if( VQU && * fwd == VQU && _CHCK_SIZE(fwd, 1) ) {
				fwd++; while( * fwd && ! _IS_SPACE(* fwd) && * fwd != VQU && _CHCK_SIZE(fwd, 1) ) fwd++;
				_JUMP_ADDR(0x0604);
			} else {
				_ROLL_BACK(0x0700);
			}
			break;

		case 0x0601:
			if( * fwd && VEB[0] && * fwd == VEB[0] && _CHCK_SIZE(fwd, 1) ) {
				fwd++; while( * fwd && ! _IS_SPACE(* fwd) && ! _tcschr(VEB, * fwd) && _CHCK_SIZE(fwd, 1) ) fwd++;
				_JUMP_ADDR(0x0602);
			} else if( * fwd && SVC[0] && _tcschr(SVC, * fwd) && _CHCK_SIZE(fwd, 1) ) {
				fwd++; while( * fwd && _tcschr(SVC, * fwd) && _CHCK_SIZE(fwd, 1) ) fwd++;
				_JUMP_ADDR(0x0603);
			} else if( * fwd && ! _IS_SPACE(* fwd) && ! _IS_DELIM(* fwd) && _CHCK_SIZE(fwd, 1) ) {
				fwd++; while( * fwd && ! _IS_SPACE(* fwd) && ! _IS_DELIM(* fwd) && _CHCK_SIZE(fwd, 1) ) fwd++;
				_WordFound(wcount++, WT_VARIABLE, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else {
				_ROLL_BACK(0x0700);
			}
			break;

		case 0x0602:
			if( * fwd && VEB[1] && * fwd == VEB[1] && _CHCK_SIZE(fwd, 1) ) {
				fwd++;
				_WordFound(wcount++, WT_VARIABLE, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else {
				_ROLL_BACK(0x0700);
			}
			break;

		case 0x0603:
			if( * fwd && ! _IS_SPACE(* fwd) && ! _IS_DELIM(* fwd) && _CHCK_SIZE(fwd, 1) ) {
				fwd++; while( * fwd && ! _IS_SPACE(* fwd) && ! _IS_DELIM(* fwd) && _CHCK_SIZE(fwd, 1) ) fwd++;
				_WordFound(wcount++, WT_VARIABLE, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else {
				_WordFound(wcount++, WT_VARIABLE, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			}
			break;

		case 0x0604:
			if( * fwd && VQU && * fwd == VQU && _CHCK_SIZE(fwd, 1) ) {
				fwd++;
				_WordFound(wcount++, WT_VARIABLE, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else {
				_ROLL_BACK(0x0700);
			}
			break;


		case 0x0700: // CHECK IDENTIFIER
			if( ! _IS_DELIM(* fwd) && _CHCK_SIZE(fwd, 1) ) {
				fwd++; while( * fwd && ! _IS_SPACE(* fwd) && ! _IS_DELIM(* fwd) && _CHCK_SIZE(fwd, 1) ) fwd++;
				_JUMP_ADDR(0x0701);
			} else {
				_ROLL_BACK(0x0800);
			}
			break;

		case 0x0701:
			if( bKEY && _pKEY->LookupTable(type, info, beg, fwd-beg) ) {
				_WordFoundExtended(wcount++, type, info, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( bDIC && ! _pDIC->LookupTable(beg, fwd-beg) ) {
				_WordFound(wcount++, WT_WRONGWORD, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else {
				_WordFound(wcount++, WT_IDENTIFIER, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			}
			break;


		case 0x0800: // CHECK DELIMITERS
			if( _IS_PRINT(* fwd) && _IS_DELIM(* fwd) && _CHCK_SIZE(fwd, 1) ) {
				fwd++;
				_WordFound(wcount++, WT_DELIMITER, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else {
				// Advance a whole character. An emoji does not actually reach
				// this branch today (the identifier state above absorbs both
				// halves into one word), but a language spec that listed a
				// surrogate-range delimiter would land here and split the pair.
				fwd += ( IsHighSurrogate(fwd[0]) && IsLowSurrogate(fwd[1]) ) ? 2 : 1;
				_WordFound(wcount++, WT_GRAPH, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			}
			break;
		}
	}

	BOOL bOverflow = (is_finished < 0) ? TRUE : FALSE;
	_FinishLine(wcount, bOverflow, rLine);
}


// Deliberately does NOT use _charTable.
//
// The table is a global, built by AnalyzeText from the language spec of the document
// it is analyzing. This is called from the word walkers (Ctrl+arrow, double-click)
// against whichever document has focus, and several documents with different language
// specs can be open at once — so the table here could easily belong to a different
// language. The linear scan is correct, and this is nowhere near a hot path: it runs
// on a keystroke, over a word, not over a 900,000-line file.
INT CCedtDoc::GetCharType(TCHAR nChar)
{
	DEL = m_clsLangSpec.m_szDelimiters;

	if     ( _istspace(nChar) || ! nChar  ) return CH_WHITESPACE;
	else if( _istprint(nChar) && _tcschr(DEL, nChar) ) return CH_DELIMITER;
	else return CH_CHARACTER;
}


void CCedtDoc::AnalyzeText(INT nIndex, INT nCount)
{
	// save settings to global variables
	_pKEY = & m_clsKeywords;
	_pDIC = & m_clsDictionary;

	// global langage specification
	VQU = m_clsLangSpec.m_chVariableQuotation;				ESC = m_clsLangSpec.m_chEscapeChar;
	MQU = m_clsLangSpec.m_chMultiLineQuotationMark;			QU1 = m_clsLangSpec.m_chQuotationMark1;
	QU2 = m_clsLangSpec.m_chQuotationMark2;					QU3 = m_clsLangSpec.m_chQuotationMark3;

	DEL = m_clsLangSpec.m_szDelimiters;						HEX = m_clsLangSpec.m_szHexaDecimalMark;
	PRE = m_clsLangSpec.m_szKeywordPrefix;					VAR = m_clsLangSpec.m_szVariablePrefix;
	VEB = m_clsLangSpec.m_szVariableOptionallyEnclosedBy;	SVC = m_clsLangSpec.m_szSpecialVariableChars;	
	HRD = m_clsLangSpec.m_szHereDocument;

	LF1 = m_clsLangSpec.m_szLineComment1OnFirstPosition;	LF2 = m_clsLangSpec.m_szLineComment2OnFirstPosition;
	LC1 = m_clsLangSpec.m_szLineComment1;					LC2 = m_clsLangSpec.m_szLineComment2;
	C1B = m_clsLangSpec.m_szBlockComment1On;				C1E = m_clsLangSpec.m_szBlockComment1Off;
	C2B = m_clsLangSpec.m_szBlockComment2On;				C2E = m_clsLangSpec.m_szBlockComment2Off;

	SDB = m_clsLangSpec.m_szShadowOn;						SDE = m_clsLangSpec.m_szShadowOff;
	HLB = m_clsLangSpec.m_szHighlightOn;					HLE = m_clsLangSpec.m_szHighlightOff;
	R1B = m_clsLangSpec.m_szRange1Beg;						R1E = m_clsLangSpec.m_szRange1End;
	R2B = m_clsLangSpec.m_szRange2Beg;						R2E = m_clsLangSpec.m_szRange2End;

	rngMQU = m_clsLangSpec.m_ucMultiLineQuotationMarkRange;	rngQU1 = m_clsLangSpec.m_ucQuotationMark1Range;
	rngQU2 = m_clsLangSpec.m_ucQuotationMark2Range;			rngQU3 = m_clsLangSpec.m_ucQuotationMark3Range;
	rngLC1 = m_clsLangSpec.m_ucLineComment1Range;			rngLC2 = m_clsLangSpec.m_ucLineComment2Range;
	rngBC1 = m_clsLangSpec.m_ucBlockComment1Range;			rngBC2 = m_clsLangSpec.m_ucBlockComment2Range;

	lenHEX = (INT)_tcslen(HEX);		lenHRD = (INT)_tcslen(HRD);
	lenLF1 = (INT)_tcslen(LF1);		lenLF2 = (INT)_tcslen(LF2);		lenLC1 = (INT)_tcslen(LC1);		lenLC2 = (INT)_tcslen(LC2);
	lenC1B = (INT)_tcslen(C1B);		lenC1E = (INT)_tcslen(C1E);		lenC2B = (INT)_tcslen(C2B);		lenC2E = (INT)_tcslen(C2E);
	lenSDB = (INT)_tcslen(SDB);		lenSDE = (INT)_tcslen(SDE);		lenHLB = (INT)_tcslen(HLB);		lenHLE = (INT)_tcslen(HLE);
	lenR1B = (INT)_tcslen(R1B);		lenR1E = (INT)_tcslen(R1E);		lenR2B = (INT)_tcslen(R2B);		lenR2E = (INT)_tcslen(R2E);

	// Derive the character classification table from the spec we just loaded. Must
	// come after every DEL/marker global above is set, and before any line is walked.
	_BuildCharTable();

	// global language options
	bKEY = (BOOL)m_clsKeywords.GetCount();
	bDIC = m_bDictionaryLoaded;
	bQUO = MQU || QU1 || QU2 || QU3;
	bCOM = LF1[0] || LF2[0] || LC1[0] || LC2[0] || C1B[0] || C1E[0] || C2B[0] || C2E[0];
	bRNG = SDB[0] || SDE[0] || HLB[0] || HLE[0] || R1B[0] || R1E[0] || R2B[0] || R2E[0];
	bPRE = PRE[0];
	bVAR = VAR[0] || VQU;

	// now start analyzing text
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pMainFrame );
	INT nProcess = 0; CWaitCursor * pWait = NULL;

	if( nCount > LARGE_FILE_LINE_COUNT ) {
		pWait = new CWaitCursor;
		pMainFrame->BeginProgress(_T("Analyzing..."));
	}

	POSITION pos = m_clsAnalyzedText.FindIndex( nIndex );
	while( pos && nProcess < nCount ) {
		_AnalyzeLine( m_clsAnalyzedText.GetNext(pos) );

		// Asks for the same percentage many times over; CStatusBarEx::SetProgress drops
		// the repaints that would produce an identical bar.
		if( nCount > LARGE_FILE_LINE_COUNT && ! (nProcess % 20) ) pMainFrame->SetProgress(100 * nProcess / nCount);
		nProcess++;
	}

	if( nCount > LARGE_FILE_LINE_COUNT ) {
		if( pWait ) delete pWait;
		pMainFrame->EndProgress();
	}
}


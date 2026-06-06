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
static BOOL bKEY, bDIC, bDBC, bQUO, bCOM, bRNG, bPRE, bVAR;


static void _WordFound(SHORT windex, UCHAR ucType, UCHAR ucInfo, SHORT siIndex, SHORT siLength)
{
	_words[windex].m_ucType[0] = ucType;
	_words[windex].m_ucType[1] = ucType;
	_words[windex].m_ucType[2] = ucType;
	_words[windex].m_ucInfo = ucInfo;
	_words[windex].m_siIndex = siIndex;
	_words[windex].m_siLength = siLength;

//	TRACE2("Word Found: %d, %d\n", windex, siLength);
}

static void _WordFoundExtended(SHORT windex, UCHAR ucType[], UCHAR ucInfo, SHORT siIndex, SHORT siLength)
{
	_words[windex].m_ucType[0] = ucType[0];
	_words[windex].m_ucType[1] = ucType[1];
	_words[windex].m_ucType[2] = ucType[2];
	_words[windex].m_ucInfo = ucInfo;
	_words[windex].m_siIndex = siIndex;
	_words[windex].m_siLength = siLength;

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

#define _CHCK_DBCS(ptr)			( bDBC && IsDBCSLeadByte(* ptr) )
#define _CHCK_SIZE(ptr, len)	( ptr - str < MAX_STRING_SIZE - (len) )

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
			} else if( fwd - str >= MAX_STRING_SIZE-1 ) {
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
				_ROLL_BACK(bDBC ? 0x0100 : (bQUO ? 0x0200 : (bCOM ? 0x0300 : (bRNG ? 0x0350 : 0x0400))));
			}
			break;


		case 0x0100: // CHECK DOUBLE BYTE CHARACTERS
			if( bDBC && IsDBCSLeadByte(* fwd) && * (fwd+1) && _CHCK_SIZE(fwd, 2) ) {
				fwd += 2;
				_WordFound(wcount++, WT_DBCHAR, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else {
				_ROLL_BACK(bQUO ? 0x0200 : (bCOM ? 0x0300 : (bRNG ? 0x0350 : 0x0400)));
			}
			break;


		case 0x0200: // CHECK QUOTATION MARK & ESCAPE CHARACTERS
			if( ESC && * fwd == ESC && * (fwd+1) && * (fwd+1) != '\t' && * (fwd+1) != ' ' && _CHCK_SIZE(fwd, 2) ) {
				fwd += 2; 
				_WordFound(wcount++, WT_CONSTANT, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( HRD[0] && ! _strnicmp(fwd, HRD, lenHRD) && _CHCK_SIZE(fwd, lenHRD) ) {
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
			if( C1B[0] && ! _strnicmp(fwd, C1B, lenC1B) && _CHCK_SIZE(fwd, lenC1B) ) { 
				fwd += lenC1B; 
				_WordFound(wcount++, WT_COMMENT1ON, rngBC1, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( C1E[0] && ! _strnicmp(fwd, C1E, lenC1E) && _CHCK_SIZE(fwd, lenC1E) ) {
				fwd += lenC1E; 
				_WordFound(wcount++, WT_COMMENT1OFF, rngBC1, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( C2B[0] && ! _strnicmp(fwd, C2B, lenC2B) && _CHCK_SIZE(fwd, lenC2B) ) {
				fwd += lenC2B; 
				_WordFound(wcount++, WT_COMMENT2ON, rngBC2, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( C2E[0] && ! _strnicmp(fwd, C2E, lenC2E) && _CHCK_SIZE(fwd, lenC2E) ) {
				fwd += lenC2E; 
				_WordFound(wcount++, WT_COMMENT2OFF, rngBC2, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( LC1[0] && isalpha(LC1[0]) && ! _strnicmp(fwd, LC1, lenLC1) && ! isalpha(fwd[lenLC1]) && _CHCK_SIZE(fwd, lenLC1) ) {
				fwd += lenLC1; 
				_WordFound(wcount++, WT_LINECOMMENT, rngLC1, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( LC1[0] && ! isalpha(LC1[0]) && ! _strnicmp(fwd, LC1, lenLC1) && _CHCK_SIZE(fwd, lenLC1) ) {
				fwd += lenLC1; 
				_WordFound(wcount++, WT_LINECOMMENT, rngLC1, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( LC2[0] && isalpha(LC2[0]) && ! _strnicmp(fwd, LC2, lenLC2) && ! isalpha(fwd[lenLC2]) && _CHCK_SIZE(fwd, lenLC2) ) {
				fwd += lenLC2; 
				_WordFound(wcount++, WT_LINECOMMENT, rngLC2, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( LC2[0] && ! isalpha(LC2[0]) && ! _strnicmp(fwd, LC2, lenLC2) && _CHCK_SIZE(fwd, lenLC2) ) {
				fwd += lenLC2; 
				_WordFound(wcount++, WT_LINECOMMENT, rngLC2, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( LF1[0] && fwd == str && ! _strnicmp(fwd, LF1, lenLF1) && _CHCK_SIZE(fwd, lenLF1) ) {
				fwd += lenLF1; 
				_WordFound(wcount++, WT_LINECOMMENT, rngLC1, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( LF2[0] && fwd == str && ! _strnicmp(fwd, LF2, lenLF2) && _CHCK_SIZE(fwd, lenLF2) ) {
				fwd += lenLF2; 
				_WordFound(wcount++, WT_LINECOMMENT, rngLC2, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else {
				_ROLL_BACK(bRNG ? 0x0350 : 0x0400);
			}
			break;

		case 0x0350: // CHECK VARIOUS DELIMITERS
			if( HLB[0] && ! _strnicmp(fwd, HLB, lenHLB) && _CHCK_SIZE(fwd, lenHLB) ) {
				fwd += lenHLB; 
				_WordFound(wcount++, WT_HIGHLIGHTON, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( HLE[0] && ! _strnicmp(fwd, HLE, lenHLE) && _CHCK_SIZE(fwd, lenHLE) ) {
				fwd += lenHLE; 
				_WordFound(wcount++, WT_HIGHLIGHTOFF, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( SDB[0] && ! _strnicmp(fwd, SDB, lenSDB) && _CHCK_SIZE(fwd, lenSDB) ) {
				fwd += lenSDB; 
				_WordFound(wcount++, WT_SHADOWON, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( SDE[0] && ! _strnicmp(fwd, SDE, lenSDE) && _CHCK_SIZE(fwd, lenSDE) ) {
				fwd += lenSDE; 
				_WordFound(wcount++, WT_SHADOWOFF, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( R2B[0] && ! _strnicmp(fwd, R2B, lenR2B) && _CHCK_SIZE(fwd, lenR2B) ) {
				fwd += lenR2B; 
				_WordFound(wcount++, WT_RANGE2BEG, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( R2E[0] && ! _strnicmp(fwd, R2E, lenR2E) && _CHCK_SIZE(fwd, lenR2E) ) {
				fwd += lenR2E; 
				_WordFound(wcount++, WT_RANGE2END, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( R1B[0] && ! _strnicmp(fwd, R1B, lenR1B) && _CHCK_SIZE(fwd, lenR1B) ) {
				fwd += lenR1B; 
				_WordFound(wcount++, WT_RANGE1BEG, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( R1E[0] && ! _strnicmp(fwd, R1E, lenR1E) && _CHCK_SIZE(fwd, lenR1E) ) {
				fwd += lenR1E; 
				_WordFound(wcount++, WT_RANGE1END, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else {
				_ROLL_BACK(0x0400);
			}
			break;


		case 0x0400: // CHECK HEXADECIMAL & DECIMAL NUMBERS AND FLOATING POINT NUMBERS
			if( HEX[0] && ! _strnicmp(fwd, HEX, lenHEX) && _CHCK_SIZE(fwd, lenHEX) ) {
				fwd += lenHEX; while( * fwd && isxdigit(* fwd) && ! _CHCK_DBCS(fwd) && _CHCK_SIZE(fwd, 1) ) fwd++;
				_WordFound(wcount++, WT_CONSTANT, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else if( isdigit(* fwd) && _CHCK_SIZE(fwd, 1) ) {
				fwd++; while( * fwd && (isdigit(* fwd) || * fwd == '.') && ! _CHCK_DBCS(fwd) && _CHCK_SIZE(fwd, 1) ) fwd++;
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
			if( * fwd && (* fwd == '+' || * fwd == '-' || isdigit(* fwd)) && ! _CHCK_DBCS(fwd) && _CHCK_SIZE(fwd, 1) ) {
				fwd++; while( * fwd && (isdigit(* fwd)) && ! _CHCK_DBCS(fwd) && _CHCK_SIZE(fwd, 1) ) fwd++;
				_WordFound(wcount++, WT_CONSTANT, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else {
				_WordFound(wcount++, WT_CONSTANT, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			}
			break;


		case 0x0500: // CHECK KEYWORD BEGINNING WITH PREFIX
			if( PRE[0] && strchr(PRE, * fwd) && _CHCK_SIZE(fwd, 1) ) {
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
			} else if( * fwd && strchr( PRE, * fwd ) && ! _CHCK_DBCS(fwd) && _CHCK_SIZE(fwd, 1) ) {
				fwd++;
				_JUMP_ADDR(0x0501);
			} else {
				while( * fwd && ! isspace(* fwd) && ! strchr(DEL, * fwd) && ! _CHCK_DBCS(fwd) && _CHCK_SIZE(fwd, 1) ) fwd++;
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
			if( VAR[0] && strchr(VAR, * fwd) && _CHCK_SIZE(fwd, 1) ) {
				fwd++;
				_JUMP_ADDR(0x0601);
			} else if( VQU && * fwd == VQU && _CHCK_SIZE(fwd, 1) ) {
				fwd++; while( * fwd && ! isspace(* fwd) && * fwd != VQU && ! _CHCK_DBCS(fwd) && _CHCK_SIZE(fwd, 1) ) fwd++;
				_JUMP_ADDR(0x0604);
			} else {
				_ROLL_BACK(0x0700);
			}
			break;

		case 0x0601:
			if( * fwd && VEB[0] && * fwd == VEB[0] && _CHCK_SIZE(fwd, 1) ) {
				fwd++; while( * fwd && ! isspace(* fwd) && ! strchr(VEB, * fwd) && ! _CHCK_DBCS(fwd) && _CHCK_SIZE(fwd, 1) ) fwd++;
				_JUMP_ADDR(0x0602);
			} else if( * fwd && SVC[0] && strchr(SVC, * fwd) && ! _CHCK_DBCS(fwd) && _CHCK_SIZE(fwd, 1) ) {
				fwd++; while( * fwd && strchr(SVC, * fwd) && ! _CHCK_DBCS(fwd) && _CHCK_SIZE(fwd, 1) ) fwd++;
				_JUMP_ADDR(0x0603);
			} else if( * fwd && ! isspace(* fwd) && ! strchr(DEL, * fwd) && ! _CHCK_DBCS(fwd) && _CHCK_SIZE(fwd, 1) ) {
				fwd++; while( * fwd && ! isspace(* fwd) && ! strchr(DEL, * fwd) && ! _CHCK_DBCS(fwd) && _CHCK_SIZE(fwd, 1) ) fwd++;
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
			if( * fwd && ! isspace(* fwd) && ! strchr(DEL, * fwd) && ! _CHCK_DBCS(fwd) && _CHCK_SIZE(fwd, 1) ) {
				fwd++; while( * fwd && ! isspace(* fwd) && ! strchr(DEL, * fwd) && ! _CHCK_DBCS(fwd) && _CHCK_SIZE(fwd, 1) ) fwd++;
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
			if( ! strchr(DEL, * fwd) && ! _CHCK_DBCS(fwd) && _CHCK_SIZE(fwd, 1) ) {
				fwd++; while( * fwd && ! isspace(* fwd) && ! strchr(DEL, * fwd) && ! _CHCK_DBCS(fwd) && _CHCK_SIZE(fwd, 1) ) fwd++;
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
			if( isprint(* fwd) && strchr(DEL, * fwd) && _CHCK_SIZE(fwd, 1) ) {
				fwd++;
				_WordFound(wcount++, WT_DELIMITER, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			} else {
				fwd++;
				_WordFound(wcount++, WT_GRAPH, RT_GLOBAL, beg-str, fwd-beg);
				_NEXT_WORD(0x0000);
			}
			break;
		}
	}

	BOOL bOverflow = (is_finished < 0) ? TRUE : FALSE;
	_FinishLine(wcount, bOverflow, rLine);
}


INT CCedtDoc::GetCharType(TCHAR nChar)
{
	BOOL bDBCS = g_bDoubleByteCharacterSet;
	DEL = m_clsLangSpec.m_szDelimiters;

	if( bDBCS && IsDBCSLeadByte(nChar)  ) return CH_CHARACTER;
	else if( isspace(nChar) || ! nChar  ) return CH_WHITESPACE;
	else if( isprint(nChar) && strchr(DEL, nChar) ) return CH_DELIMITER;
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

	lenHEX = strlen(HEX);		lenHRD = strlen(HRD);
	lenLF1 = strlen(LF1);		lenLF2 = strlen(LF2);		lenLC1 = strlen(LC1);		lenLC2 = strlen(LC2);
	lenC1B = strlen(C1B);		lenC1E = strlen(C1E);		lenC2B = strlen(C2B);		lenC2E = strlen(C2E);
	lenSDB = strlen(SDB);		lenSDE = strlen(SDE);		lenHLB = strlen(HLB);		lenHLE = strlen(HLE);
	lenR1B = strlen(R1B);		lenR1E = strlen(R1E);		lenR2B = strlen(R2B);		lenR2E = strlen(R2E);

	// global language options
	bKEY = m_clsKeywords.GetCount();
	bDIC = m_bDictionaryLoaded;
	bDBC = g_bDoubleByteCharacterSet;
	bQUO = MQU || QU1 || QU2 || QU3;
	bCOM = LF1[0] || LF2[0] || LC1[0] || LC2[0] || C1B[0] || C1E[0] || C2B[0] || C2E[0];
	bRNG = SDB[0] || SDE[0] || HLB[0] || HLE[0] || R1B[0] || R1E[0] || R2B[0] || R2E[0];
	bPRE = PRE[0];
	bVAR = VAR[0] || VQU;

	// now start analyzing text
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pMainFrame );
	INT nProcess = 0; CWaitCursor * pWait = NULL;

	if( nCount > 1000 ) {
		pWait = new CWaitCursor;
		pMainFrame->BeginProgress("Analyzing...");
	}

	POSITION pos = m_clsAnalyzedText.FindIndex( nIndex );
	while( pos && nProcess < nCount ) {
		_AnalyzeLine( m_clsAnalyzedText.GetNext(pos) );

		if( nCount > 1000 && ! (nProcess % 20) ) pMainFrame->SetProgress(100 * nProcess / nCount);
		nProcess++; 
	}

	if( nCount > 1000 ) {
		if( pWait ) delete pWait;
		pMainFrame->EndProgress();
	}
}


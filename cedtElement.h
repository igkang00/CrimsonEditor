#ifndef __CEDT_ELEMENT_H_
#define __CEDT_ELEMENT_H_



// MAXIMUM CONSTANTS
#define MAX_STRING_SIZE					32767
#define MAX_WORD_LENGTH					255
#define MAX_WORDS_COUNT					32767


// RANGE TYPE (KEYWORD RANGE)
#define RT_GLOBAL						0x00
#define RT_RANGE1						0x01
#define RT_RANGE2						0x02
#define RT_R1ORR2						0x03


// CHARACTER TYPE
#define CH_WHITESPACE					0x00
#define CH_DELIMITER					0x01
#define CH_CHARACTER					0x02


// WORD TYPE
#define WT_RETURN						0x01
#define WT_LINEBREAK					0x02
#define WT_TAB							0x03
#define WT_SPACE						0x04
#define WT_CONSTANT						0x05
#define WT_QUOTATION0					0x06
#define WT_QUOTATION1					0x07
#define WT_QUOTATION2					0x08
#define WT_QUOTATION3					0x09
#define WT_HEREDOCUMENT					0x0A
#define WT_LINECOMMENT					0x0B
#define WT_COMMENT1ON					0x0C
#define WT_COMMENT1OFF					0x0D
#define WT_COMMENT2ON					0x0E
#define WT_COMMENT2OFF					0x0F
#define WT_SHADOWON						0x10
#define WT_SHADOWOFF					0x11
#define WT_HIGHLIGHTON					0x12
#define WT_HIGHLIGHTOFF					0x13
#define WT_RANGE1BEG					0x14
#define WT_RANGE1END					0x15
#define WT_RANGE2BEG					0x16
#define WT_RANGE2END					0x17
#define WT_VARIABLE						0x18
#define WT_KEYWORD0						0x19
#define WT_KEYWORD1						0x1A
#define WT_KEYWORD2						0x1B
#define WT_KEYWORD3						0x1C
#define WT_KEYWORD4						0x1D
#define WT_KEYWORD5						0x1E
#define WT_KEYWORD6						0x1F
#define WT_KEYWORD7						0x20
#define WT_KEYWORD8						0x21
#define WT_KEYWORD9						0x22
#define WT_IDENTIFIER					0x23
#define WT_WRONGWORD					0x24
#define WT_DELIMITER					0x25
#define WT_DBCHAR						0x26
#define WT_GRAPH						0x27


// WORD TYPE CHECK MACROS
#define IS_DELIMITER(rWord)				( (rWord).m_ucType[0] == WT_DELIMITER )
#define IS_DBCHAR(rWord)				( (rWord).m_ucType[0] == WT_DBCHAR )
#define IS_WSPACE(rWord)				( (rWord).m_ucType[0] == WT_RETURN || \
										  (rWord).m_ucType[0] == WT_LINEBREAK || \
										  (rWord).m_ucType[0] == WT_TAB || \
										  (rWord).m_ucType[0] == WT_SPACE )
#define IS_SINGLE(rWord)				( (rWord).m_ucType[0] == WT_RETURN || \
										  (rWord).m_ucType[0] == WT_LINEBREAK || \
										  (rWord).m_ucType[0] == WT_TAB || \
										  (rWord).m_ucType[0] == WT_DELIMITER || \
										  (rWord).m_ucType[0] == WT_GRAPH )


// LINE INFO
#define LI_HAVEOVERFLOW					0x0001
#define LI_HAVEBOOKMARK					0x0002

#define LI_HAVERANGE					0x0100
#define LI_HAVEHIGHLIGHT				0x0200
#define LI_HAVECOMMENT					0x0400
#define LI_HAVEQUOTATION				0x0800
#define LI_HAVEHEREDOCUMENT				0x1000

// LINE FLAG
#define LF_QUOTATION0					0x0001
#define LF_QUOTATION1					0x0002
#define LF_QUOTATION2					0x0004
#define LF_QUOTATION3					0x0008
#define LF_QUOTATIONH					0x0010
#define LF_LINECOMMENT					0x0020
#define LF_BLOCKCOMMENT1				0x0040
#define LF_BLOCKCOMMENT2				0x0080
#define LF_HIGHLIGHT					0x0100
#define LF_SHADOW						0x0200
#define LF_RANGE1						0x0400
#define LF_RANGE2						0x0800
#define LF_HEREDOCUMENT					0x1000


// LINE FLAG CHECK MACROS
#define CHECK_THIS(usFlag, some)		( (usFlag) & ( some ) )
#define CHECK_QUOTATION(usFlag)			( (usFlag) & ( LF_QUOTATION0 | LF_QUOTATION1 | LF_QUOTATION2 | LF_QUOTATION3 | LF_QUOTATIONH) )
#define CHECK_QUOTE_EX0(usFlag)			( (usFlag) & ( LF_QUOTATION1 | LF_QUOTATION2 | LF_QUOTATION3 | LF_QUOTATIONH ) )
#define CHECK_QUOTE_EX1(usFlag)			( (usFlag) & ( LF_QUOTATION0 | LF_QUOTATION2 | LF_QUOTATION3 | LF_QUOTATIONH ) )
#define CHECK_QUOTE_EX2(usFlag)			( (usFlag) & ( LF_QUOTATION0 | LF_QUOTATION1 | LF_QUOTATION3 | LF_QUOTATIONH ) )
#define CHECK_QUOTE_EX3(usFlag)			( (usFlag) & ( LF_QUOTATION0 | LF_QUOTATION1 | LF_QUOTATION2 | LF_QUOTATIONH ) )
#define CHECK_COMMENT(usFlag)			( (usFlag) & ( LF_BLOCKCOMMENT1 | LF_BLOCKCOMMENT2 | LF_LINECOMMENT ) )
#define CHECK_CMT_BLK(usFlag)			( (usFlag) & ( LF_BLOCKCOMMENT1 | LF_BLOCKCOMMENT2 ) )
#define CHECK_CMT_BL1(usFlag)			( (usFlag) & ( LF_BLOCKCOMMENT1 ) )
#define CHECK_CMT_BL2(usFlag)			( (usFlag) & ( LF_BLOCKCOMMENT2 ) )
#define CHECK_CMT_LNE(usFlag)			( (usFlag) & ( LF_LINECOMMENT ) )
#define CHECK_HILIGHT(usFlag)			( (usFlag) & ( LF_HIGHLIGHT | LF_SHADOW ) )
#define CHECK_RANGEA(usFlag)			( (usFlag) & ( LF_RANGE1 | LF_RANGE2 ) )
#define CHECK_RANGE1(usFlag)			( (usFlag) & ( LF_RANGE1 ) )
#define CHECK_RANGE2(usFlag)			( (usFlag) & ( LF_RANGE2 ) )

#define EFFECTIVE_RANGE(ucRange, usFlag)	( ( ( (ucRange) == RT_GLOBAL ) ) || \
		( ( (ucRange) == RT_RANGE1 ) && ( (usFlag) & LF_RANGE1 ) ) || \
		( ( (ucRange) == RT_RANGE2 ) && ( (usFlag) & LF_RANGE2 ) ) || \
		( ( (ucRange) == RT_R1ORR2 ) && ( (usFlag) & ( LF_RANGE1 | LF_RANGE2 ) ) ) )


// ACTION TYPE
#define AT_INSERTCHAR					0x01
#define AT_DELETECHAR					0x02
#define AT_INSERTSTRING					0x03
#define AT_DELETESTRING					0x04
#define AT_INSERTBLOCK					0x05
#define AT_DELETEBLOCK					0x06
#define AT_SPLITLINE					0x07
#define AT_JOINLINES					0x08


// MACRO TYPE
#define MACRO_MOVE						0x01
#define MACRO_CHAR						0x02
#define MACRO_STRING					0x03
#define MACRO_FILE						0x04
#define MACRO_FIND						0x05
#define MACRO_REPLACE					0x06
#define MACRO_GOTO						0x07
#define MACRO_COMMAND					0x08


// CONTROL CHARACTERS
#define CONTROL_CHARS1					"\x07\x08\x7F\xFF"
#define CONTROL_CHARS2					\
"\x01\x02\x03\x04\x05\x06\x07\x08\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F\x7F\xFF"


// SEARCH OPTIONS
#define OPTION_WHOLE_WORD				0x01
#define OPTION_MATCH_CASE				0x02
#define OPTION_REG_EXP					0x04

#define COMPOSE_SEARCH_OPTION(bWholeWord, bMatchCase, bRegExp)	( \
	(bWholeWord ? OPTION_WHOLE_WORD : 0x00) | \
	(bMatchCase ? OPTION_MATCH_CASE : 0x00) | \
	(bRegExp    ? OPTION_REG_EXP    : 0x00) )

#define SEARCH_WHOLE_WORD(nOptions)		( (nOptions) & OPTION_WHOLE_WORD )
#define SEARCH_MATCH_CASE(nOptions)		( (nOptions) & OPTION_MATCH_CASE )
#define SEARCH_REG_EXP(nOptions)		( (nOptions) & OPTION_REG_EXP    )


// KEY STATUS
#define KEYSTATE_CONTROL				0x01
#define KEYSTATE_SHIFT					0x02
#define KEYSTATE_MENU					0x04


// FILE BACKUP OPTION
#define BACKUP_DONT_BACKUP				0x00
#define BACKUP_BEFORE_SAVE				0x01
#define BACKUP_BEFORE_OPEN				0x02

#define BACKUP_METHOD01					0x01
#define BACKUP_METHOD02					0x02


// ENCODING TYPE
#define ENCODING_TYPE_UNKNOWN			0x00
#define ENCODING_TYPE_ASCII				0x01
#define ENCODING_TYPE_UNICODE_LE		0x02
#define ENCODING_TYPE_UNICODE_BE		0x03
#define ENCODING_TYPE_UTF8_WBOM			0x04
#define ENCODING_TYPE_UTF8_XBOM			0x05

extern CString ENCODING_TYPE_DESCRIPTION_FULL[];
extern CString ENCODING_TYPE_DESCRIPTION_SHORT[];


// FILE FORMAT
#define FILE_FORMAT_UNKNOWN				0x00
#define FILE_FORMAT_DOS					0x01
#define FILE_FORMAT_UNIX				0x02
#define FILE_FORMAT_MAC					0x03

extern CString FILE_FORMAT_DESCRIPTION_FULL[];
extern CString FILE_FORMAT_DESCRIPTION_SHORT[];


// FILE READ/WRITE BUFFER SIZE
#define FILE_READ_BUFFER_SIZE			512
#define FILE_WRITE_BUFFER_SIZE			512


// CALLBACK FUNCTION
typedef BOOL (* CALLBACK_FUNCTION)(UINT);


// GLOBAL FUNCTIONS
BOOL DetectEncodingTypeAndFileFormat(LPCTSTR lpszPathName, UINT & nEncodingType, UINT & nFileFormat);

BOOL DetectEncodingType(LPVOID lpContents, INT nLength, UINT & nEncodingType);
BOOL DetectFileFormat(LPVOID lpContents, INT nLength, UINT & nFileFormat);


// CLangSpec Class
class CLangSpec 
{
public:
	BOOL	m_bCaseSensitive[3]; // case sensitivity in each range

	BOOL	m_bVariableHighlightInString;
	CString m_szDelimiters, m_szHexaDecimalMark, m_szKeywordPrefix, m_szVariablePrefix;
	CString m_szVariableOptionallyEnclosedBy, m_szSpecialVariableChars;
	TCHAR	m_chVariableQuotation;
	
	BOOL	m_bMultiLineStringConstant;
	TCHAR	m_chEscapeChar, m_chMultiLineQuotationMark;
	TCHAR	m_chQuotationMark1, m_chQuotationMark2, m_chQuotationMark3;
	UCHAR	m_ucMultiLineQuotationMarkRange;
	UCHAR	m_ucQuotationMark1Range, m_ucQuotationMark2Range, m_ucQuotationMark3Range;
	CString m_szHereDocument;

	CString m_szLineComment1OnFirstPosition, m_szLineComment2OnFirstPosition;
	CString m_szLineComment1, m_szLineComment2;
	UCHAR	m_ucLineComment1Range, m_ucLineComment2Range;

	CString m_szBlockComment1On, m_szBlockComment1Off;
	CString m_szBlockComment2On, m_szBlockComment2Off;
	UCHAR	m_ucBlockComment1Range, m_ucBlockComment2Range;

	CString m_szShadowOn, m_szShadowOff;
	CString m_szHighlightOn, m_szHighlightOff;
	CString m_szRange1Beg, m_szRange1End;
	CString m_szRange2Beg, m_szRange2End;

	TCHAR	m_chIndentationOn, m_chIndentationOff;
	CString m_szPairs1, m_szPairs2, m_szPairs3;

public:
	CLangSpec() { ResetContents(); }
	virtual ~CLangSpec() {}

	void ResetContents();
	BOOL FileLoad(LPCTSTR lpszPathName);
};


// CKeywords Class
class CKeywords : public CMap<CString, LPCTSTR, DWORD, DWORD &>
{
public:
	CKeywords() { InitHashTable(1024); }
	virtual ~CKeywords() {}

	void ResetContents() { RemoveAll(); }
	BOOL FileLoad(LPCTSTR lpszPathName, BOOL bCaseSensitive[]);

	BOOL LookupTable(UCHAR ucType[], UCHAR & ucRange, LPCTSTR lpszWord, SHORT siLength);
};


// CDictionary Class
class CDictionary : public CMap<CString, LPCTSTR, UCHAR, UCHAR &> 
{
public:
	CDictionary() { InitHashTable(16384); }
	virtual ~CDictionary() {}

	void ResetContents() { RemoveAll(); }
	BOOL FileLoad(LPCTSTR lpszPathName, CALLBACK_FUNCTION fcnCallback = NULL);

	BOOL LookupTable(LPCTSTR lpszWord, SHORT siLength);
	BOOL AddWord(LPCTSTR lpszWord);
};


// ANALYZEDWORD Structure
typedef struct _ANALYZEDWORD {
	UCHAR	m_ucType[3], m_ucInfo;
	SHORT	m_siIndex, m_siLength;
} ANALYZEDWORD;


// CAnalyzedString Class
class CAnalyzedString : public CString 
{
public:
	ANALYZEDWORD * m_pWord;
	SHORT	m_siWordCount;
	USHORT	m_usLineInfo;
	
public:
	CAnalyzedString() : CString() { m_pWord = NULL; m_siWordCount = 0; m_usLineInfo = 0x00; }
	CAnalyzedString( const CString & stringSrc ) : CString(stringSrc) { m_pWord = NULL; m_siWordCount = 0; m_usLineInfo = 0x00; }
	CAnalyzedString( TCHAR ch, int nRepeat = 1 ) : CString(ch, nRepeat) { m_pWord = NULL; m_siWordCount = 0; m_usLineInfo = 0x00; }
	CAnalyzedString( LPCTSTR lpsz, int nLength ) : CString(lpsz, nLength) { m_pWord = NULL; m_siWordCount = 0; m_usLineInfo = 0x00; }
	CAnalyzedString( const unsigned char * psz ) : CString(psz) { m_pWord = NULL; m_siWordCount = 0; m_usLineInfo = 0x00; }
	CAnalyzedString( LPCWSTR lpsz ) : CString(lpsz) { m_pWord = NULL; m_siWordCount = 0; m_usLineInfo = 0x00; }
	CAnalyzedString( LPCSTR lpsz ) : CString(lpsz) { m_pWord = NULL; m_siWordCount = 0; m_usLineInfo = 0x00; }

	virtual ~CAnalyzedString() { delete [] m_pWord; }

	CAnalyzedString & operator=(const CAnalyzedString & stringSrc);
};


// FORMATEDWORD Structure
typedef struct _FORMATEDWORD {
	UCHAR	m_ucType[3], m_ucInfo;
	SHORT	m_siIndex, m_siLength;
	INT		m_nPosition, m_nWidth;
} FORMATEDWORD;


// CFormatedString Class
class CFormatedString 
{
public:
	LPCTSTR m_pString; FORMATEDWORD * m_pWord;
	SHORT	m_siWordCount, m_siSplitIndex; BOOL m_bLineBreak;
	USHORT	m_usLineInfo, m_usLineFlag;
	CString	m_szHereDocumentTerminator;
	
public:
	CFormatedString() { m_pString = NULL; m_pWord = NULL; m_siWordCount = m_siSplitIndex = 0; m_bLineBreak = FALSE; m_usLineInfo = m_usLineFlag = 0x00; }
	virtual ~CFormatedString() { delete [] m_pWord; }
	
	CFormatedString & operator=(const CFormatedString & stringSrc);
	operator LPCTSTR() const { return m_pString; }
	TCHAR operator[](INT nIndex) const { return m_pString[nIndex]; }
};


// CMemText Class
class CMemText : public CList<CString, LPCTSTR>
{
public:
	CMemText() : CList<CString, LPCTSTR>() {}
	CMemText(const CMemText & rBlock) { operator=(rBlock); }
	virtual ~CMemText() {}

	BOOL FileLoad(LPCTSTR lpszPathName);
	BOOL FileSave(LPCTSTR lpszPathName);

	void AppendText(CMemText & rBlock);
	CMemText & operator=(const CMemText & rBlock);

	void MakeUpperCase();
	void MakeLowerCase();
	void MakeInvertCase();
	void MakeCapitalize();

	INT  GetMaxLength();
	void MakeEqualLength();

	INT  MemorySize();
	void MemoryLoad(CHAR * pMem, INT size);
	void MemorySave(CHAR * pMem, INT size);
};


// CAnalyzedText Class
class CAnalyzedText : public CList<CAnalyzedString, LPCTSTR>
{
public:
	CAnalyzedText() : CList<CAnalyzedString, LPCTSTR>() {}
	virtual ~CAnalyzedText() {}

	BOOL FileLoad(LPCTSTR lpszPathName, INT nEncodingType, INT nFileFormat);
	BOOL FileSave(LPCTSTR lpszPathName, INT nEncodingType, INT nFileFormat);

	BOOL HaveAnyOverflowLine();
};


// CFormatedText Class
class CFormatedText : public CList<CFormatedString, CFormatedString &>
{
public:
	CFormatedText() : CList<CFormatedString, CFormatedString &>() {}
	virtual ~CFormatedText() {}
};


// CUndoBuffer Class
class CUndoBuffer : public CList<INT, INT> 
{
public:
	CList<INT, INT> m_lstAction;
	CList<INT, INT> m_lstIdxX, m_lstIdxY;
	CList<INT, INT> m_lstParam;
	CList<UINT, UINT> m_lstChar;
	CList<CString, LPCTSTR> m_lstString;
	CList<CMemText, CMemText &> m_lstBlock;

public:
	CUndoBuffer() {}
	virtual ~CUndoBuffer() {}

	void EmptyBuffer();
	void GetRecentIndex(INT & nIdxX, INT & nIdxY);
};


// CUserCommand Class
class CUserCommand 
{
public:
	WORD m_wVirtualKeyCode, m_wModifiers;
	CString m_szName, m_szCommand;
	CString m_szArgument, m_szDirectory;
	BOOL m_bCloseOnExit, m_bUseShortFileName;
	BOOL m_bCaptureOutput, m_bSaveBeforeExecute;

public:
	CUserCommand();
	virtual ~CUserCommand() {}

	INT IsAssigned() { return m_szCommand.GetLength(); }
	CString GetHotKeyText();

	BOOL StreamSave(ofstream & fout);
	BOOL StreamLoad(ifstream & fin);

	void DeleteContents();
	void CopyContents(CUserCommand & rCommand);
};


// CMacroBuffer Class
class CMacroBuffer 
{
public:
	WORD m_wVirtualKeyCode, m_wModifiers;
	CString m_szName;
	CList<INT, INT> m_lstAction;
	CList<UINT, UINT> m_lstParam;
	CList<UINT, UINT> m_lstFlags;
	CList<CString, LPCTSTR> m_lstString;

public:
	CMacroBuffer();
	virtual ~CMacroBuffer() {}

	INT GetBufferCount() { return m_lstAction.GetCount(); }
	CString GetHotKeyText();

	BOOL StreamSave(ofstream & fout);
	BOOL StreamLoad(ifstream & fin);

	void DeleteContents();
	void CopyContents(CMacroBuffer & rBuffer);
};


// COutputPattern Class
class COutputPattern
{
public:
	CString m_szPattern;
	CString m_szFileName;
	CString m_szLineNum;

public:
	COutputPattern() {}
	virtual ~COutputPattern() {}

	BOOL IsValid() { return m_szPattern.GetLength() && m_szFileName.GetLength(); }

	BOOL StreamSave(ofstream & fout);
	BOOL StreamLoad(ifstream & fin);
};


// CFileFilter Class
class CFileFilter 
{
public:
	CString m_szDescription;
	CString m_szExtensions;
	CString m_szDefaultExt;

public:
	CFileFilter() {}
	virtual ~CFileFilter() {}

	BOOL IsValid() { return (m_szDescription.GetLength() && m_szExtensions.GetLength()); }

	BOOL StreamSave(ofstream & fout);
	BOOL StreamLoad(ifstream & fin);

	void AssignContents(LPCTSTR lpszDescription, LPCTSTR lpszExtensions, LPCTSTR lpszDefaultExt);
	void DeleteContents();
	void CopyContents(CFileFilter & rFilter);
};


// CSyntaxType Class
class CSyntaxType 
{
public:
	CString m_szDescription;
	CString m_szLangSpecFile;
	CString m_szKeywordsFile;

public:
	CSyntaxType() {}
	virtual ~CSyntaxType() {}

	BOOL StreamSave(ofstream & fout);
	BOOL StreamLoad(ifstream & fin);

	void AssignContents(LPCTSTR lpszDescription, LPCTSTR lpszLangSpecFile, LPCTSTR lpszKeywordsFile);
	void DeleteContents();
	void CopyContents(CSyntaxType & rSyntax);
};


#define FTP_SERVER_GENERIC		0x00
#define FTP_SERVER_HPUNIX		0x01

// CFtpAccount Class
class CFtpAccount 
{
public:
	CString m_szDescription, m_szServerName;
	CString m_szUserName, m_szPassword;
	CString m_szSubDirectory;

	BOOL m_bSavePassword, m_bPassiveMode, m_bBinaryType;
	BOOL m_bUseWinInet, m_bPasswordVerified;
	INT  m_nServerType, m_nPortNumber;

public:
	CFtpAccount();
	virtual ~CFtpAccount() {}

	CString GetDisplayName();
	CString GetFullAccountName();
	CString GetShortAccountName();

	INT IsValid() { return (m_szDescription.GetLength() && m_szServerName.GetLength()); }

	BOOL StreamSave(ofstream & fout);
	BOOL StreamLoad(ifstream & fin);

	void DeleteContents();
	void CopyContents(CFtpAccount & rAccount);
};


#endif // __CEDT_ELEMENT_H_

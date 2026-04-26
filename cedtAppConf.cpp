#include "stdafx.h"
#include "cedtHeader.h"
#include "cedtColors.h"



BOOL CCedtApp::SaveFtpAccountInfo(LPCTSTR lpszPathName)
{
	if( ! TouchFile(lpszPathName) ) return FALSE;

	ofstream fout(lpszPathName, ios::out | ios::binary);
	if( ! fout.is_open() ) return FALSE;

	INT nLength = strlen(STRING_FTPACCOUNTVER);
	fout.write((const char *)STRING_FTPACCOUNTVER, nLength);

	for(INT i = 0; i < MAX_FTP_ACCOUNT; i++) {
		if( ! m_clsFtpAccounts[i].StreamSave(fout) ) { fout.close(); return FALSE; }
	}

	fout.close();
	return TRUE;
}

BOOL CCedtApp::LoadFtpAccountInfo(LPCTSTR lpszPathName) 
{
	ifstream fin(lpszPathName, ios::in | ios::nocreate | ios::binary);
	if( ! fin.is_open() ) return FALSE;

	TCHAR szBuffer[2048]; INT nLength = strlen(STRING_FTPACCOUNTVER); fin.read((char *)szBuffer, nLength); szBuffer[nLength] = '\0';
	if( strcmp(szBuffer, STRING_FTPACCOUNTVER) ) { fin.close(); return FALSE; }

	for(INT i = 0; i < MAX_FTP_ACCOUNT; i++) {
		if( ! m_clsFtpAccounts[i].StreamLoad(fin) ) { fin.close(); return FALSE; }
	}

	fin.close();
	return TRUE;
}

BOOL CCedtApp::SaveUserCommands(LPCTSTR lpszPathName)
{
	if( ! TouchFile(lpszPathName) ) return FALSE;

	ofstream fout(lpszPathName, ios::out | ios::binary);
	if( ! fout.is_open() ) return FALSE;

	INT nLength = strlen(STRING_USERTOOLSVER);
	fout.write((const char *)STRING_USERTOOLSVER, nLength);

	for(INT i = 0; i < 11; i++) {
		if( ! CCedtView::m_clsUserCommand[i].StreamSave(fout) ) { fout.close(); return FALSE; }
	}

	fout.close();
	return TRUE;
}

BOOL CCedtApp::LoadUserCommands(LPCTSTR lpszPathName)
{
	ifstream fin(lpszPathName, ios::in | ios::nocreate | ios::binary);
	if( ! fin.is_open() ) return FALSE;

	TCHAR szBuffer[2048]; INT nLength = strlen(STRING_USERTOOLSVER); fin.read((char *)szBuffer, nLength); szBuffer[nLength] = '\0';
	if( strcmp(szBuffer, STRING_USERTOOLSVER) ) { fin.close(); return FALSE; }

	for(INT i = 0; i < 11; i++) {
		if( ! CCedtView::m_clsUserCommand[i].StreamLoad(fin) ) { fin.close(); return FALSE; }
	}

	fin.close();
	return TRUE;
}

BOOL CCedtApp::SaveMacroBuffers(LPCTSTR lpszPathName)
{
	if( ! TouchFile(lpszPathName) ) return FALSE;

	ofstream fout(lpszPathName, ios::out | ios::binary);
	if( ! fout.is_open() ) return FALSE;

	INT nLength = strlen(STRING_USERMACROVER);
	fout.write((const char *)STRING_USERMACROVER, nLength);

	for(INT i = 0; i < 11; i++) {
		if( ! CCedtView::m_clsMacroBuffer[i].StreamSave(fout) ) { fout.close(); return FALSE; }
	}

	fout.close();
	return TRUE;
}

BOOL CCedtApp::LoadMacroBuffers(LPCTSTR lpszPathName)
{
	ifstream fin(lpszPathName, ios::in | ios::nocreate | ios::binary);
	if( ! fin.is_open() ) return FALSE;

	TCHAR szBuffer[2048]; INT nLength = strlen(STRING_USERMACROVER); fin.read((char *)szBuffer, nLength); szBuffer[nLength] = '\0';
	if( strcmp(szBuffer, STRING_USERMACROVER) ) { fin.close(); return FALSE; }

	for(INT i = 0; i < 11; i++) {
		if( ! CCedtView::m_clsMacroBuffer[i].StreamLoad(fin) ) { fin.close(); return FALSE; }
	}

	fin.close();
	return TRUE;
}

BOOL CCedtApp::SetPredefinedColorScheme(INT nScheme)
{
	switch( nScheme ) {
	case COLOR_SCHEME_DEFAULT:
		CCedtView::m_crBkgrColor[0x00]				= CR_DEFAULT_BACKGROUND;
		CCedtView::m_crBkgrColor[0x01]				= CR_DEFAULT_LEFTMARGIN;
		CCedtView::m_crBkgrColor[0x02]				= CR_DEFAULT_CURRENTLINE;
		CCedtView::m_crBkgrColor[0x03]				= CR_DEFAULT_RANGE1BKGR;
		CCedtView::m_crBkgrColor[0x04]				= CR_DEFAULT_RANGE2BKGR;

		CCedtView::m_crTextColor[WT_LINEBREAK]		= CR_DEFAULT_LINENUMBER;
		CCedtView::m_crTextColor[WT_CONSTANT]		= CR_DEFAULT_CONSTANT;
		CCedtView::m_crTextColor[WT_QUOTATION0]		= CR_DEFAULT_STRING;
		CCedtView::m_crTextColor[WT_LINECOMMENT]	= CR_DEFAULT_COMMENT;
		CCedtView::m_crTextColor[WT_VARIABLE]		= CR_DEFAULT_VARIABLE;
		CCedtView::m_crTextColor[WT_IDENTIFIER]		= CR_DEFAULT_WORD;
		CCedtView::m_crTextColor[WT_WRONGWORD]		= CR_DEFAULT_WORD;
		CCedtView::m_crTextColor[WT_DBCHAR]			= CR_DEFAULT_WORD;
		CCedtView::m_crTextColor[WT_GRAPH]			= CR_DEFAULT_WORD;
		CCedtView::m_crTextColor[WT_RETURN]			= CR_DEFAULT_DELIMITER;
		CCedtView::m_crTextColor[WT_TAB]			= CR_DEFAULT_DELIMITER;
		CCedtView::m_crTextColor[WT_SPACE]			= CR_DEFAULT_DELIMITER;
		CCedtView::m_crTextColor[WT_DELIMITER]		= CR_DEFAULT_DELIMITER;
		CCedtView::m_crTextColor[WT_HIGHLIGHTON]	= CR_DEFAULT_HIGHLIGHTED;
		CCedtView::m_crTextColor[WT_SHADOWON]		= CR_DEFAULT_SHADOWED;

		CCedtView::m_crTextColor[WT_KEYWORD0]		= CR_DEFAULT_KEYWORD0;
		CCedtView::m_crTextColor[WT_KEYWORD1]		= CR_DEFAULT_KEYWORD1;
		CCedtView::m_crTextColor[WT_KEYWORD2]		= CR_DEFAULT_KEYWORD2;
		CCedtView::m_crTextColor[WT_KEYWORD3]		= CR_DEFAULT_KEYWORD3;
		CCedtView::m_crTextColor[WT_KEYWORD4]		= CR_DEFAULT_KEYWORD4;
		CCedtView::m_crTextColor[WT_KEYWORD5]		= CR_DEFAULT_KEYWORD5;
		CCedtView::m_crTextColor[WT_KEYWORD6]		= CR_DEFAULT_KEYWORD6;
		CCedtView::m_crTextColor[WT_KEYWORD7]		= CR_DEFAULT_KEYWORD7;
		CCedtView::m_crTextColor[WT_KEYWORD8]		= CR_DEFAULT_KEYWORD8;
		CCedtView::m_crTextColor[WT_KEYWORD9]		= CR_DEFAULT_KEYWORD9;
		return TRUE;

	case COLOR_SCHEME_SIMPLIFIED:
		CCedtView::m_crBkgrColor[0x00]				= CR_SIMPLIFIED_BACKGROUND;
		CCedtView::m_crBkgrColor[0x01]				= CR_SIMPLIFIED_LEFTMARGIN;
		CCedtView::m_crBkgrColor[0x02]				= CR_SIMPLIFIED_CURRENTLINE;
		CCedtView::m_crBkgrColor[0x03]				= CR_SIMPLIFIED_RANGE1BKGR;
		CCedtView::m_crBkgrColor[0x04]				= CR_SIMPLIFIED_RANGE2BKGR;

		CCedtView::m_crTextColor[WT_LINEBREAK]		= CR_SIMPLIFIED_LINENUMBER;
		CCedtView::m_crTextColor[WT_CONSTANT]		= CR_SIMPLIFIED_CONSTANT;
		CCedtView::m_crTextColor[WT_QUOTATION0]		= CR_SIMPLIFIED_STRING;
		CCedtView::m_crTextColor[WT_LINECOMMENT]	= CR_SIMPLIFIED_COMMENT;
		CCedtView::m_crTextColor[WT_VARIABLE]		= CR_SIMPLIFIED_VARIABLE;
		CCedtView::m_crTextColor[WT_IDENTIFIER]		= CR_SIMPLIFIED_WORD;
		CCedtView::m_crTextColor[WT_WRONGWORD]		= CR_SIMPLIFIED_WORD;
		CCedtView::m_crTextColor[WT_DBCHAR]			= CR_SIMPLIFIED_WORD;
		CCedtView::m_crTextColor[WT_GRAPH]			= CR_SIMPLIFIED_WORD;
		CCedtView::m_crTextColor[WT_RETURN]			= CR_SIMPLIFIED_DELIMITER;
		CCedtView::m_crTextColor[WT_TAB]			= CR_SIMPLIFIED_DELIMITER;
		CCedtView::m_crTextColor[WT_SPACE]			= CR_SIMPLIFIED_DELIMITER;
		CCedtView::m_crTextColor[WT_DELIMITER]		= CR_SIMPLIFIED_DELIMITER;
		CCedtView::m_crTextColor[WT_HIGHLIGHTON]	= CR_SIMPLIFIED_HIGHLIGHTED;
		CCedtView::m_crTextColor[WT_SHADOWON]		= CR_SIMPLIFIED_SHADOWED;

		CCedtView::m_crTextColor[WT_KEYWORD0]		= CR_SIMPLIFIED_KEYWORD0;
		CCedtView::m_crTextColor[WT_KEYWORD1]		= CR_SIMPLIFIED_KEYWORD1;
		CCedtView::m_crTextColor[WT_KEYWORD2]		= CR_SIMPLIFIED_KEYWORD2;
		CCedtView::m_crTextColor[WT_KEYWORD3]		= CR_SIMPLIFIED_KEYWORD3;
		CCedtView::m_crTextColor[WT_KEYWORD4]		= CR_SIMPLIFIED_KEYWORD4;
		CCedtView::m_crTextColor[WT_KEYWORD5]		= CR_SIMPLIFIED_KEYWORD5;
		CCedtView::m_crTextColor[WT_KEYWORD6]		= CR_SIMPLIFIED_KEYWORD6;
		CCedtView::m_crTextColor[WT_KEYWORD7]		= CR_SIMPLIFIED_KEYWORD7;
		CCedtView::m_crTextColor[WT_KEYWORD8]		= CR_SIMPLIFIED_KEYWORD8;
		CCedtView::m_crTextColor[WT_KEYWORD9]		= CR_SIMPLIFIED_KEYWORD9;
		return TRUE;

	case COLOR_SCHEME_REVERSED:
		CCedtView::m_crBkgrColor[0x00]				= CR_REVERSED_BACKGROUND;
		CCedtView::m_crBkgrColor[0x01]				= CR_REVERSED_LEFTMARGIN;
		CCedtView::m_crBkgrColor[0x02]				= CR_REVERSED_CURRENTLINE;
		CCedtView::m_crBkgrColor[0x03]				= CR_REVERSED_RANGE1BKGR;
		CCedtView::m_crBkgrColor[0x04]				= CR_REVERSED_RANGE2BKGR;

		CCedtView::m_crTextColor[WT_LINEBREAK]		= CR_REVERSED_LINENUMBER;
		CCedtView::m_crTextColor[WT_CONSTANT]		= CR_REVERSED_CONSTANT;
		CCedtView::m_crTextColor[WT_QUOTATION0]		= CR_REVERSED_STRING;
		CCedtView::m_crTextColor[WT_LINECOMMENT]	= CR_REVERSED_COMMENT;
		CCedtView::m_crTextColor[WT_VARIABLE]		= CR_REVERSED_VARIABLE;
		CCedtView::m_crTextColor[WT_IDENTIFIER]		= CR_REVERSED_WORD;
		CCedtView::m_crTextColor[WT_WRONGWORD]		= CR_REVERSED_WORD;
		CCedtView::m_crTextColor[WT_DBCHAR]			= CR_REVERSED_WORD;
		CCedtView::m_crTextColor[WT_GRAPH]			= CR_REVERSED_WORD;
		CCedtView::m_crTextColor[WT_RETURN]			= CR_REVERSED_DELIMITER;
		CCedtView::m_crTextColor[WT_TAB]			= CR_REVERSED_DELIMITER;
		CCedtView::m_crTextColor[WT_SPACE]			= CR_REVERSED_DELIMITER;
		CCedtView::m_crTextColor[WT_DELIMITER]		= CR_REVERSED_DELIMITER;
		CCedtView::m_crTextColor[WT_HIGHLIGHTON]	= CR_REVERSED_HIGHLIGHTED;
		CCedtView::m_crTextColor[WT_SHADOWON]		= CR_REVERSED_SHADOWED;

		CCedtView::m_crTextColor[WT_KEYWORD0]		= CR_REVERSED_KEYWORD0;
		CCedtView::m_crTextColor[WT_KEYWORD1]		= CR_REVERSED_KEYWORD1;
		CCedtView::m_crTextColor[WT_KEYWORD2]		= CR_REVERSED_KEYWORD2;
		CCedtView::m_crTextColor[WT_KEYWORD3]		= CR_REVERSED_KEYWORD3;
		CCedtView::m_crTextColor[WT_KEYWORD4]		= CR_REVERSED_KEYWORD4;
		CCedtView::m_crTextColor[WT_KEYWORD5]		= CR_REVERSED_KEYWORD5;
		CCedtView::m_crTextColor[WT_KEYWORD6]		= CR_REVERSED_KEYWORD6;
		CCedtView::m_crTextColor[WT_KEYWORD7]		= CR_REVERSED_KEYWORD7;
		CCedtView::m_crTextColor[WT_KEYWORD8]		= CR_REVERSED_KEYWORD8;
		CCedtView::m_crTextColor[WT_KEYWORD9]		= CR_REVERSED_KEYWORD9;
		return TRUE;

	case COLOR_SCHEME_LIGHTGRAY:
		CCedtView::m_crBkgrColor[0x00]				= CR_LIGHTGRAY_BACKGROUND;
		CCedtView::m_crBkgrColor[0x01]				= CR_LIGHTGRAY_LEFTMARGIN;
		CCedtView::m_crBkgrColor[0x02]				= CR_LIGHTGRAY_CURRENTLINE;
		CCedtView::m_crBkgrColor[0x03]				= CR_LIGHTGRAY_RANGE1BKGR;
		CCedtView::m_crBkgrColor[0x04]				= CR_LIGHTGRAY_RANGE2BKGR;

		CCedtView::m_crTextColor[WT_LINEBREAK]		= CR_LIGHTGRAY_LINENUMBER;
		CCedtView::m_crTextColor[WT_CONSTANT]		= CR_LIGHTGRAY_CONSTANT;
		CCedtView::m_crTextColor[WT_QUOTATION0]		= CR_LIGHTGRAY_STRING;
		CCedtView::m_crTextColor[WT_LINECOMMENT]	= CR_LIGHTGRAY_COMMENT;
		CCedtView::m_crTextColor[WT_VARIABLE]		= CR_LIGHTGRAY_VARIABLE;
		CCedtView::m_crTextColor[WT_IDENTIFIER]		= CR_LIGHTGRAY_WORD;
		CCedtView::m_crTextColor[WT_WRONGWORD]		= CR_LIGHTGRAY_WORD;
		CCedtView::m_crTextColor[WT_DBCHAR]			= CR_LIGHTGRAY_WORD;
		CCedtView::m_crTextColor[WT_GRAPH]			= CR_LIGHTGRAY_WORD;
		CCedtView::m_crTextColor[WT_RETURN]			= CR_LIGHTGRAY_DELIMITER;
		CCedtView::m_crTextColor[WT_TAB]			= CR_LIGHTGRAY_DELIMITER;
		CCedtView::m_crTextColor[WT_SPACE]			= CR_LIGHTGRAY_DELIMITER;
		CCedtView::m_crTextColor[WT_DELIMITER]		= CR_LIGHTGRAY_DELIMITER;
		CCedtView::m_crTextColor[WT_HIGHLIGHTON]	= CR_LIGHTGRAY_HIGHLIGHTED;
		CCedtView::m_crTextColor[WT_SHADOWON]		= CR_LIGHTGRAY_SHADOWED;

		CCedtView::m_crTextColor[WT_KEYWORD0]		= CR_LIGHTGRAY_KEYWORD0;
		CCedtView::m_crTextColor[WT_KEYWORD1]		= CR_LIGHTGRAY_KEYWORD1;
		CCedtView::m_crTextColor[WT_KEYWORD2]		= CR_LIGHTGRAY_KEYWORD2;
		CCedtView::m_crTextColor[WT_KEYWORD3]		= CR_LIGHTGRAY_KEYWORD3;
		CCedtView::m_crTextColor[WT_KEYWORD4]		= CR_LIGHTGRAY_KEYWORD4;
		CCedtView::m_crTextColor[WT_KEYWORD5]		= CR_LIGHTGRAY_KEYWORD5;
		CCedtView::m_crTextColor[WT_KEYWORD6]		= CR_LIGHTGRAY_KEYWORD6;
		CCedtView::m_crTextColor[WT_KEYWORD7]		= CR_LIGHTGRAY_KEYWORD7;
		CCedtView::m_crTextColor[WT_KEYWORD8]		= CR_LIGHTGRAY_KEYWORD8;
		CCedtView::m_crTextColor[WT_KEYWORD9]		= CR_LIGHTGRAY_KEYWORD9;
		return TRUE;

	case COLOR_SCHEME_DARKBLUE:
		CCedtView::m_crBkgrColor[0x00]				= CR_DARKBLUE_BACKGROUND;
		CCedtView::m_crBkgrColor[0x01]				= CR_DARKBLUE_LEFTMARGIN;
		CCedtView::m_crBkgrColor[0x02]				= CR_DARKBLUE_CURRENTLINE;
		CCedtView::m_crBkgrColor[0x03]				= CR_DARKBLUE_RANGE1BKGR;
		CCedtView::m_crBkgrColor[0x04]				= CR_DARKBLUE_RANGE2BKGR;

		CCedtView::m_crTextColor[WT_LINEBREAK]		= CR_DARKBLUE_LINENUMBER;
		CCedtView::m_crTextColor[WT_CONSTANT]		= CR_DARKBLUE_CONSTANT;
		CCedtView::m_crTextColor[WT_QUOTATION0]		= CR_DARKBLUE_STRING;
		CCedtView::m_crTextColor[WT_LINECOMMENT]	= CR_DARKBLUE_COMMENT;
		CCedtView::m_crTextColor[WT_VARIABLE]		= CR_DARKBLUE_VARIABLE;
		CCedtView::m_crTextColor[WT_IDENTIFIER]		= CR_DARKBLUE_WORD;
		CCedtView::m_crTextColor[WT_WRONGWORD]		= CR_DARKBLUE_WORD;
		CCedtView::m_crTextColor[WT_DBCHAR]			= CR_DARKBLUE_WORD;
		CCedtView::m_crTextColor[WT_GRAPH]			= CR_DARKBLUE_WORD;
		CCedtView::m_crTextColor[WT_RETURN]			= CR_DARKBLUE_DELIMITER;
		CCedtView::m_crTextColor[WT_TAB]			= CR_DARKBLUE_DELIMITER;
		CCedtView::m_crTextColor[WT_SPACE]			= CR_DARKBLUE_DELIMITER;
		CCedtView::m_crTextColor[WT_DELIMITER]		= CR_DARKBLUE_DELIMITER;
		CCedtView::m_crTextColor[WT_HIGHLIGHTON]	= CR_DARKBLUE_HIGHLIGHTED;
		CCedtView::m_crTextColor[WT_SHADOWON]		= CR_DARKBLUE_SHADOWED;

		CCedtView::m_crTextColor[WT_KEYWORD0]		= CR_DARKBLUE_KEYWORD0;
		CCedtView::m_crTextColor[WT_KEYWORD1]		= CR_DARKBLUE_KEYWORD1;
		CCedtView::m_crTextColor[WT_KEYWORD2]		= CR_DARKBLUE_KEYWORD2;
		CCedtView::m_crTextColor[WT_KEYWORD3]		= CR_DARKBLUE_KEYWORD3;
		CCedtView::m_crTextColor[WT_KEYWORD4]		= CR_DARKBLUE_KEYWORD4;
		CCedtView::m_crTextColor[WT_KEYWORD5]		= CR_DARKBLUE_KEYWORD5;
		CCedtView::m_crTextColor[WT_KEYWORD6]		= CR_DARKBLUE_KEYWORD6;
		CCedtView::m_crTextColor[WT_KEYWORD7]		= CR_DARKBLUE_KEYWORD7;
		CCedtView::m_crTextColor[WT_KEYWORD8]		= CR_DARKBLUE_KEYWORD8;
		CCedtView::m_crTextColor[WT_KEYWORD9]		= CR_DARKBLUE_KEYWORD9;
		return TRUE;

	default:
		return FALSE;
	}
}

BOOL CCedtApp::LoadColorScheme(LPCTSTR lpszPathName)
{
	ifstream fin(lpszPathName, ios::in | ios::nocreate | ios::binary);
	if( ! fin.is_open() ) return FALSE;

	TCHAR szBuffer[2048]; INT nLength = strlen(STRING_COLORSETTINGSVER); fin.read((char *)szBuffer, nLength); szBuffer[nLength] = '\0';
	if( strcmp(szBuffer, STRING_COLORSETTINGSVER) ) { fin.close(); return FALSE; }

	fin.read((char *)CCedtView::m_crBkgrColor, sizeof(CCedtView::m_crBkgrColor));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)CCedtView::m_crTextColor, sizeof(CCedtView::m_crTextColor));
	if( ! fin.good() ) { fin.close(); return FALSE; }

	if( fin.peek() != EOF ) { fin.close(); return FALSE; }
	fin.close();

	return TRUE;
}

BOOL CCedtApp::SaveColorScheme(LPCTSTR lpszPathName)
{
	if( ! TouchFile(lpszPathName) ) return FALSE;

	ofstream fout(lpszPathName, ios::out | ios::binary);
	if( ! fout.is_open() ) return FALSE;

	INT nLength = strlen(STRING_COLORSETTINGSVER);
	fout.write((const char *)STRING_COLORSETTINGSVER, nLength);

	fout.write((const char *)CCedtView::m_crBkgrColor, sizeof(CCedtView::m_crBkgrColor));
	fout.write((const char *)CCedtView::m_crTextColor, sizeof(CCedtView::m_crTextColor));

	fout.close();

	return TRUE;
}


void CCedtApp::SetDefaultConfiguration()
{
	memset(CCedtView::m_lfScreen, 0x00, sizeof(CCedtView::m_lfScreen));
	strcpy(CCedtView::m_lfScreen[0].lfFaceName, "Courier New");
	strcpy(CCedtView::m_lfScreen[1].lfFaceName, "FixedSys");
	strcpy(CCedtView::m_lfScreen[2].lfFaceName, "Verdana");
	strcpy(CCedtView::m_lfScreen[3].lfFaceName, "Arial");
	strcpy(CCedtView::m_lfScreen[4].lfFaceName, "Lucida Console");
	strcpy(CCedtView::m_lfScreen[5].lfFaceName, "Terminal");
	CCedtView::m_lfScreen[0].lfHeight = 100;	CCedtView::m_lfScreen[1].lfHeight = 90;
	CCedtView::m_lfScreen[2].lfHeight = 100;	CCedtView::m_lfScreen[3].lfHeight = 100;
	CCedtView::m_lfScreen[4].lfHeight = 100;	CCedtView::m_lfScreen[5].lfHeight = 100;
	CCedtView::m_lfScreen[0].lfCharSet = CCedtView::m_lfScreen[1].lfCharSet = DEFAULT_CHARSET;
	CCedtView::m_lfScreen[2].lfCharSet = CCedtView::m_lfScreen[3].lfCharSet = DEFAULT_CHARSET;
	CCedtView::m_lfScreen[4].lfCharSet = CCedtView::m_lfScreen[5].lfCharSet = DEFAULT_CHARSET;
	CCedtView::m_lfScreen[0].lfPitchAndFamily = CCedtView::m_lfScreen[1].lfPitchAndFamily = 0x31;
	CCedtView::m_lfScreen[2].lfPitchAndFamily = CCedtView::m_lfScreen[3].lfPitchAndFamily = 0x22;
	CCedtView::m_lfScreen[4].lfPitchAndFamily = CCedtView::m_lfScreen[5].lfPitchAndFamily = 0x31;
	CCedtView::m_nCurrentScreenFont = 0;

	memset(CCedtView::m_lfPrinter, 0x00, sizeof(CCedtView::m_lfPrinter));
	strcpy(CCedtView::m_lfPrinter[0].lfFaceName, "Courier New");
	strcpy(CCedtView::m_lfPrinter[1].lfFaceName, "Verdana");
	strcpy(CCedtView::m_lfPrinter[2].lfFaceName, "Arial");
	strcpy(CCedtView::m_lfPrinter[3].lfFaceName, "Lucida Console");
	CCedtView::m_lfPrinter[0].lfHeight = 100;	CCedtView::m_lfPrinter[1].lfHeight = 100;
	CCedtView::m_lfPrinter[2].lfHeight = 100;	CCedtView::m_lfPrinter[3].lfHeight = 100;
	CCedtView::m_lfPrinter[0].lfCharSet = CCedtView::m_lfPrinter[1].lfCharSet = DEFAULT_CHARSET;
	CCedtView::m_lfPrinter[2].lfCharSet = CCedtView::m_lfPrinter[3].lfCharSet = DEFAULT_CHARSET;
	CCedtView::m_lfPrinter[0].lfPitchAndFamily = CCedtView::m_lfPrinter[3].lfPitchAndFamily = 0x31;
	CCedtView::m_lfPrinter[1].lfPitchAndFamily = CCedtView::m_lfPrinter[2].lfPitchAndFamily = 0x22;
	CCedtView::m_nCurrentPrinterFont = 0;

	memset(CCedtView::m_lfMiscel, 0x00, sizeof(CCedtView::m_lfMiscel));
	strcpy(CCedtView::m_lfMiscel[0].lfFaceName, "Courier New");
	strcpy(CCedtView::m_lfMiscel[1].lfFaceName, "Courier New");
	CCedtView::m_lfMiscel[0].lfHeight = 100;	CCedtView::m_lfMiscel[1].lfHeight = 100;
	CCedtView::m_lfMiscel[0].lfCharSet = CCedtView::m_lfMiscel[1].lfCharSet = DEFAULT_CHARSET;
	CCedtView::m_lfMiscel[0].lfPitchAndFamily = CCedtView::m_lfMiscel[1].lfPitchAndFamily = 0x31;

	CCedtView::m_bShowLineNumbers = FALSE;
	CCedtView::m_bSyntaxHighlight = TRUE;
	CCedtView::m_bEmboldenKeywords = FALSE;
	CCedtView::m_bItalicizeComment = FALSE;
	CCedtView::m_bHighlightActiveLine = TRUE;
	CCedtView::m_bEnablePairsMatching = TRUE;
	CCedtView::m_bEnableAutoIndent = TRUE;
	CCedtView::m_bShowSelectionMargin = FALSE;

	CCedtView::m_bHomeKeyGoesToFirstPosition = FALSE;
	CCedtView::m_bSearchWrapAtEndOfFile = TRUE;
	CCedtView::m_bEnableDragAndDropEditing = TRUE;
	CCedtView::m_bCopyCurrentLineIfNothingSelected = FALSE;

	CCedtView::m_bShowSpaces = FALSE;
	CCedtView::m_bShowTabChars = FALSE;
	CCedtView::m_bShowLineBreak = FALSE;

	CCedtView::m_bUseSpacesInPlaceOfTab = FALSE;
	CCedtView::m_bOpenDocumentWordWrapped = FALSE;

	CCedtView::m_nTabSize = 4;
	CCedtView::m_nIndentationSize = 0;
	CCedtView::m_nFixedWrapWidth = 0;
	CCedtView::m_nWrapIndentation = 4;

	CCedtView::m_nLineSpacing = 100;
	CCedtView::m_nLineNumberDigits = 4;
	CCedtView::m_nCaretWidthRatio = 25;
	CCedtView::m_nFileTabLength = 32;

	CCedtView::m_bShowColumnMarker1 = FALSE;
	CCedtView::m_bShowColumnMarker2 = FALSE;
	CCedtView::m_nColumnMarker1Pos = 6;
	CCedtView::m_nColumnMarker2Pos = 80;

	// print setup
	CCedtView::m_rectPageMargin.left = 100;
	CCedtView::m_rectPageMargin.top = 100;
	CCedtView::m_rectPageMargin.right = 100;
	CCedtView::m_rectPageMargin.bottom = 100;
	CCedtView::m_bPrintLineNumbers = FALSE;
	CCedtView::m_bPrintSyntaxHighlight = FALSE;
	CCedtView::m_bPrintHeader = TRUE;
	CCedtView::m_bPrintFooter = TRUE;
	CCedtView::m_szHeaderFormat[0] = "$(FileName)";
	CCedtView::m_szHeaderFormat[1] = "";
	CCedtView::m_szHeaderFormat[2] = "$(PageNumber) / $(TotalPage)";
	CCedtView::m_szFooterFormat[0] = "$(CurrDate)";
	CCedtView::m_szFooterFormat[1] = "";
	CCedtView::m_szFooterFormat[2] = "Crimson Editor";

	// document related
	CCedtDoc::m_bConvertTabsToSpacesBeforeSaving = FALSE;
	CCedtDoc::m_bRemoveTrailingSpacesBeforeSaving = FALSE;

	CCedtDoc::m_bSaveFilesInUnixFormat = FALSE;
	CCedtDoc::m_bSaveRemoteFilesInUnixFormat = TRUE;

	CCedtDoc::m_nDefaultEncodingType = ENCODING_TYPE_ASCII;
	CCedtDoc::m_nDefaultFileFormat = FILE_FORMAT_DOS;

	// backup options
	CCedtDoc::m_nMakeBackupFile = BACKUP_DONT_BACKUP;
	CCedtDoc::m_nBackupMethod = BACKUP_METHOD01;

	CCedtDoc::m_szBackupExtension = "bak";
	CCedtDoc::m_szBackupDirectory = "";

	// syntax types
	CString szSyntaxCount; szSyntaxCount.LoadString(IDS_CONF_SYNTAX_COUNT);
	INT nSyntaxCount = atoi(szSyntaxCount);

	for(INT i = 0; i < nSyntaxCount; i++) {
		CString szSyntaxType; szSyntaxType.LoadString(IDS_CONF_SYNTAX_TYPE0 + i);
		INT nFound; CString szDescription, szLangSpec, szKeywords;

		nFound = szSyntaxType.Find('|'); if( nFound < 0 ) { szSyntaxType = "NULL Document|NULL.SPC|NULL.KEY"; nFound = 13; }
		szDescription = szSyntaxType.Left(nFound); szSyntaxType = szSyntaxType.Mid(nFound + 1);
		nFound = szSyntaxType.Find('|'); if( nFound < 0 ) { szSyntaxType = "NULL.SPC|NULL.KEY", nFound = 8; }
		szLangSpec = szSyntaxType.Left(nFound); szKeywords = szSyntaxType.Mid(nFound + 1);

		CCedtDoc::m_clsSyntaxTypes[i].AssignContents(szDescription, szLangSpec, szKeywords);
	}

	if( nSyntaxCount < 1 ) { // in case there is no syntax type
		CString szDescription = "NULL Document";
		CString szLangSpec = "NULL.SPC";
		CString szKeywords = "NULL.KEY";
		CCedtDoc::m_clsSyntaxTypes[0].AssignContents(szDescription, szLangSpec, szKeywords);
	}

	// working directory and application settings
	CCedtApp::m_szInitialWorkingDirectory = "";
	CCedtApp::m_szRemoteBackupDirectory = "";

	CCedtApp::m_bCreateNewDocumentOnStartup = TRUE;
	CCedtApp::m_bReloadWorkingFilesOnStartup = TRUE;
	CCedtApp::m_bCheckIfFilesModifiedOutside = TRUE;
	CCedtApp::m_bCloseTabUsingDoubleClick = FALSE;

	// file filters
	CString szFilterCount; szFilterCount.LoadString(IDS_CONF_FILTER_COUNT);
	INT nFilterCount = atoi(szFilterCount);

	for(INT j = 0; j < nFilterCount; j++) {
		CString szFileFilter; szFileFilter.LoadString(IDS_CONF_FILE_FILTER0 + j);
		INT nFound; CString szDescription, szExtensions, szDefaultExt;

		nFound = szFileFilter.Find('|'); if( nFound < 0 ) { szFileFilter = "All Files|*.*|"; nFound = 9; }
		szDescription = szFileFilter.Left(nFound); szFileFilter = szFileFilter.Mid(nFound + 1);
		nFound = szFileFilter.Find('|'); if( nFound < 0 ) { szFileFilter = "*.*|"; nFound = 3; }
		szExtensions = szFileFilter.Left(nFound); szDefaultExt = szFileFilter.Mid(nFound + 1);

		CCedtApp::m_clsFileFilters[j].AssignContents(szDescription, szExtensions, szDefaultExt);
	}

	if( nFilterCount < 1 ) { // in case there is no file filter
		CString szDescription = "All Files";
		CString szExtensions = "*.*";
		CString szDefaultExt = "";
		CCedtApp::m_clsFileFilters[0].AssignContents(szDescription, szExtensions, szDefaultExt);
	}

	CCedtApp::m_nFilterIndexDialog = 0;
	CCedtApp::m_nFilterIndexPannel = 0;
}


BOOL CCedtApp::LoadUserConfiguration(LPCTSTR lpszPathName)
{
	INT i, nLength; TCHAR szBuffer[2048];

	ifstream fin(lpszPathName, ios::in | ios::nocreate | ios::binary);
	if( ! fin.is_open() ) return FALSE;

	nLength = strlen(STRING_CONFIGURATIONVER); fin.read((char *)szBuffer, nLength); szBuffer[nLength] = '\0';
	if( strcmp(szBuffer, STRING_CONFIGURATIONVER) ) { fin.close(); return FALSE; }

	fin.read((char *)CCedtView::m_lfScreen, sizeof(CCedtView::m_lfScreen));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_nCurrentScreenFont), sizeof(INT));
	if( ! fin.good() ) { fin.close(); return FALSE; }

	fin.read((char *)CCedtView::m_lfPrinter, sizeof(CCedtView::m_lfPrinter));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_nCurrentPrinterFont), sizeof(INT));
	if( ! fin.good() ) { fin.close(); return FALSE; }

	fin.read((char *)CCedtView::m_lfMiscel, sizeof(CCedtView::m_lfMiscel));
	if( ! fin.good() ) { fin.close(); return FALSE; }

	fin.read((char *)(& CCedtView::m_bShowLineNumbers), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_bSyntaxHighlight), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_bEmboldenKeywords), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_bItalicizeComment), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_bHighlightActiveLine), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_bEnablePairsMatching), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_bEnableAutoIndent), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_bShowSelectionMargin), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }

	fin.read((char *)(& CCedtView::m_bHomeKeyGoesToFirstPosition), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_bSearchWrapAtEndOfFile), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_bEnableDragAndDropEditing), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_bCopyCurrentLineIfNothingSelected), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }

	fin.read((char *)(& CCedtView::m_bShowSpaces), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_bShowTabChars), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_bShowLineBreak), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }

	fin.read((char *)(& CCedtView::m_bUseSpacesInPlaceOfTab), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_bOpenDocumentWordWrapped), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }

	fin.read((char *)(& CCedtView::m_nTabSize), sizeof(INT));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_nIndentationSize), sizeof(INT));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_nFixedWrapWidth), sizeof(INT));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_nWrapIndentation), sizeof(INT));
	if( ! fin.good() ) { fin.close(); return FALSE; }

	fin.read((char *)(& CCedtView::m_nLineSpacing), sizeof(INT));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_nLineNumberDigits), sizeof(INT));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_nCaretWidthRatio), sizeof(INT));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_nFileTabLength), sizeof(INT));
	if( ! fin.good() ) { fin.close(); return FALSE; }

	fin.read((char *)(& CCedtView::m_bShowColumnMarker1), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_bShowColumnMarker2), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }

	fin.read((char *)(& CCedtView::m_nColumnMarker1Pos), sizeof(INT));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_nColumnMarker2Pos), sizeof(INT));
	if( ! fin.good() ) { fin.close(); return FALSE; }

	// print setup
	fin.read((char *)(& CCedtView::m_rectPageMargin), sizeof(RECT));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_bPrintLineNumbers), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_bPrintSyntaxHighlight), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_bPrintHeader), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtView::m_bPrintFooter), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }

	// header format
	for(i = 0; i < 3; i++) {
		nLength = 0; fin.read((char *)(& nLength), sizeof(INT));
		if( nLength < 0 || nLength >= 2048 ) { fin.close(); return FALSE; }
		if( nLength ) fin.read((char *)szBuffer, nLength);
		if( ! fin.good() ) { fin.close(); return FALSE; }
		szBuffer[nLength] = '\0'; CCedtView::m_szHeaderFormat[i] = szBuffer;
	}

	// footer format
	for(i = 0; i < 3; i++) {
		nLength = 0; fin.read((char *)(& nLength), sizeof(INT));
		if( nLength < 0 || nLength >= 2048 ) { fin.close(); return FALSE; }
		if( nLength ) fin.read((char *)szBuffer, nLength);
		if( ! fin.good() ) { fin.close(); return FALSE; }
		szBuffer[nLength] = '\0'; CCedtView::m_szFooterFormat[i] = szBuffer;
	}

	// document related
	fin.read((char *)(& CCedtDoc::m_bConvertTabsToSpacesBeforeSaving), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtDoc::m_bRemoveTrailingSpacesBeforeSaving), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }

	fin.read((char *)(& CCedtDoc::m_bSaveFilesInUnixFormat), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtDoc::m_bSaveRemoteFilesInUnixFormat), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }

	fin.read((char *)(& CCedtDoc::m_nDefaultEncodingType), sizeof(UINT));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtDoc::m_nDefaultFileFormat), sizeof(UINT));
	if( ! fin.good() ) { fin.close(); return FALSE; }

	// backup options
	fin.read((char *)(& CCedtDoc::m_nMakeBackupFile), sizeof(UINT));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtDoc::m_nBackupMethod), sizeof(UINT));
	if( ! fin.good() ) { fin.close(); return FALSE; }

	nLength = 0; fin.read((char *)(& nLength), sizeof(INT));
	if( nLength < 0 || nLength >= 2048 ) { fin.close(); return FALSE; }
	if( nLength ) fin.read((char *)szBuffer, nLength);
	if( ! fin.good() ) { fin.close(); return FALSE; }
	szBuffer[nLength] = '\0'; CCedtDoc::m_szBackupExtension = szBuffer;

	nLength = 0; fin.read((char *)(& nLength), sizeof(INT));
	if( nLength < 0 || nLength >= 2048 ) { fin.close(); return FALSE; }
	if( nLength ) fin.read((char *)szBuffer, nLength);
	if( ! fin.good() ) { fin.close(); return FALSE; }
	szBuffer[nLength] = '\0'; CCedtDoc::m_szBackupDirectory = szBuffer;

	// syntax types
	for( i = 0; i < MAX_SYNTAX_TYPE; i++ ) {
		if( ! CCedtDoc::m_clsSyntaxTypes[i].StreamLoad(fin) ) { fin.close(); return FALSE; }
		if( ! fin.good() ) { fin.close(); return FALSE; }
	}

	// working directory
	nLength = 0; fin.read((char *)(& nLength), sizeof(INT));
	if( nLength < 0 || nLength >= 2048 ) { fin.close(); return FALSE; }
	if( nLength ) fin.read((char *)szBuffer, nLength);
	if( ! fin.good() ) { fin.close(); return FALSE; }
	szBuffer[nLength] = '\0'; CCedtApp::m_szInitialWorkingDirectory = szBuffer;

	// remote backup directory
	nLength = 0; fin.read((char *)(& nLength), sizeof(INT));
	if( nLength < 0 || nLength >= 2048 ) { fin.close(); return FALSE; }
	if( nLength ) fin.read((char *)szBuffer, nLength);
	if( ! fin.good() ) { fin.close(); return FALSE; }
	szBuffer[nLength] = '\0'; CCedtApp::m_szRemoteBackupDirectory = szBuffer;

	// application initialization options
	fin.read((char *)(& CCedtApp::m_bCreateNewDocumentOnStartup), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtApp::m_bReloadWorkingFilesOnStartup), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtApp::m_bCheckIfFilesModifiedOutside), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtApp::m_bCloseTabUsingDoubleClick), sizeof(BOOL));
	if( ! fin.good() ) { fin.close(); return FALSE; }

	// file filters and application settings
	for( i = 0; i < MAX_FILE_FILTER; i++ ) {
		if( ! CCedtApp::m_clsFileFilters[i].StreamLoad(fin) ) { fin.close(); return FALSE; }
		if( ! fin.good() ) { fin.close(); return FALSE; }
	}

	fin.read((char *)(& CCedtApp::m_nFilterIndexDialog), sizeof(INT));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)(& CCedtApp::m_nFilterIndexPannel), sizeof(INT));
	if( ! fin.good() ) { fin.close(); return FALSE; }

	if( fin.peek() != EOF ) { fin.close(); return FALSE; }

	fin.close();
	return TRUE;
}


BOOL CCedtApp::SaveUserConfiguration(LPCTSTR lpszPathName)
{
	INT i, nLength;
	if( ! TouchFile(lpszPathName) ) return FALSE;

	ofstream fout(lpszPathName, ios::out | ios::binary);
	if( ! fout.is_open() ) return FALSE;

	nLength = strlen(STRING_CONFIGURATIONVER);
	fout.write((const char *)STRING_CONFIGURATIONVER, nLength);

	fout.write((const char *)CCedtView::m_lfScreen, sizeof(CCedtView::m_lfScreen));
	fout.write((const char *)(& CCedtView::m_nCurrentScreenFont), sizeof(INT));

	fout.write((const char *)CCedtView::m_lfPrinter, sizeof(CCedtView::m_lfPrinter));
	fout.write((const char *)(& CCedtView::m_nCurrentPrinterFont), sizeof(INT));

	fout.write((const char *)CCedtView::m_lfMiscel, sizeof(CCedtView::m_lfMiscel));

	fout.write((const char *)(& CCedtView::m_bShowLineNumbers), sizeof(BOOL));
	fout.write((const char *)(& CCedtView::m_bSyntaxHighlight), sizeof(BOOL));
	fout.write((const char *)(& CCedtView::m_bEmboldenKeywords), sizeof(BOOL));
	fout.write((const char *)(& CCedtView::m_bItalicizeComment), sizeof(BOOL));
	fout.write((const char *)(& CCedtView::m_bHighlightActiveLine), sizeof(BOOL));
	fout.write((const char *)(& CCedtView::m_bEnablePairsMatching), sizeof(BOOL));
	fout.write((const char *)(& CCedtView::m_bEnableAutoIndent), sizeof(BOOL));
	fout.write((const char *)(& CCedtView::m_bShowSelectionMargin), sizeof(BOOL));

	fout.write((const char *)(& CCedtView::m_bHomeKeyGoesToFirstPosition), sizeof(BOOL));
	fout.write((const char *)(& CCedtView::m_bSearchWrapAtEndOfFile), sizeof(BOOL));
	fout.write((const char *)(& CCedtView::m_bEnableDragAndDropEditing), sizeof(BOOL));
	fout.write((const char *)(& CCedtView::m_bCopyCurrentLineIfNothingSelected), sizeof(BOOL));

	fout.write((const char *)(& CCedtView::m_bShowSpaces), sizeof(BOOL));
	fout.write((const char *)(& CCedtView::m_bShowTabChars), sizeof(BOOL));
	fout.write((const char *)(& CCedtView::m_bShowLineBreak), sizeof(BOOL));

	fout.write((const char *)(& CCedtView::m_bUseSpacesInPlaceOfTab), sizeof(BOOL));
	fout.write((const char *)(& CCedtView::m_bOpenDocumentWordWrapped), sizeof(BOOL));

	fout.write((const char *)(& CCedtView::m_nTabSize), sizeof(INT));
	fout.write((const char *)(& CCedtView::m_nIndentationSize), sizeof(INT));
	fout.write((const char *)(& CCedtView::m_nFixedWrapWidth), sizeof(INT));
	fout.write((const char *)(& CCedtView::m_nWrapIndentation), sizeof(INT));

	fout.write((const char *)(& CCedtView::m_nLineSpacing), sizeof(INT));
	fout.write((const char *)(& CCedtView::m_nLineNumberDigits), sizeof(INT));
	fout.write((const char *)(& CCedtView::m_nCaretWidthRatio), sizeof(INT));
	fout.write((const char *)(& CCedtView::m_nFileTabLength), sizeof(INT));

	fout.write((const char *)(& CCedtView::m_bShowColumnMarker1), sizeof(BOOL));
	fout.write((const char *)(& CCedtView::m_bShowColumnMarker2), sizeof(BOOL));
	fout.write((const char *)(& CCedtView::m_nColumnMarker1Pos), sizeof(INT));
	fout.write((const char *)(& CCedtView::m_nColumnMarker2Pos), sizeof(INT));

	// print setup
	fout.write((const char *)(& CCedtView::m_rectPageMargin), sizeof(RECT));
	fout.write((const char *)(& CCedtView::m_bPrintLineNumbers), sizeof(BOOL));
	fout.write((const char *)(& CCedtView::m_bPrintSyntaxHighlight), sizeof(BOOL));
	fout.write((const char *)(& CCedtView::m_bPrintHeader), sizeof(BOOL));
	fout.write((const char *)(& CCedtView::m_bPrintFooter), sizeof(BOOL));

	// header format
	for(i = 0; i < 3; i++) {
		nLength = CCedtView::m_szHeaderFormat[i].GetLength();
		fout.write((const char *)(& nLength), sizeof(INT));
		if( nLength ) fout.write((const char *)(CCedtView::m_szHeaderFormat[i]), nLength);
	}

	// footer format
	for(i = 0; i < 3; i++) {
		nLength = CCedtView::m_szFooterFormat[i].GetLength();
		fout.write((const char *)(& nLength), sizeof(INT));
		if( nLength ) fout.write((const char *)(CCedtView::m_szFooterFormat[i]), nLength);
	}

	// document related
	fout.write((const char *)(& CCedtDoc::m_bConvertTabsToSpacesBeforeSaving), sizeof(BOOL));
	fout.write((const char *)(& CCedtDoc::m_bRemoveTrailingSpacesBeforeSaving), sizeof(BOOL));

	fout.write((const char *)(& CCedtDoc::m_bSaveFilesInUnixFormat), sizeof(BOOL));
	fout.write((const char *)(& CCedtDoc::m_bSaveRemoteFilesInUnixFormat), sizeof(BOOL));

	fout.write((const char *)(& CCedtDoc::m_nDefaultEncodingType), sizeof(UINT));
	fout.write((const char *)(& CCedtDoc::m_nDefaultFileFormat), sizeof(UINT));

	// backup options
	fout.write((const char *)(& CCedtDoc::m_nMakeBackupFile), sizeof(UINT));
	fout.write((const char *)(& CCedtDoc::m_nBackupMethod), sizeof(UINT));

	nLength = CCedtDoc::m_szBackupExtension.GetLength();
	fout.write((const char *)(& nLength), sizeof(INT));
	if( nLength ) fout.write((const char *)(CCedtDoc::m_szBackupExtension), nLength);

	nLength = CCedtDoc::m_szBackupDirectory.GetLength();
	fout.write((const char *)(& nLength), sizeof(INT));
	if( nLength ) fout.write((const char *)(CCedtDoc::m_szBackupDirectory), nLength);

	// syntax types
	for( i = 0; i < MAX_SYNTAX_TYPE; i++ ) {
		CCedtDoc::m_clsSyntaxTypes[i].StreamSave(fout);
	}

	// working directory
	nLength = CCedtApp::m_szInitialWorkingDirectory.GetLength();
	fout.write((const char *)(& nLength), sizeof(INT));
	if( nLength ) fout.write((const char *)(CCedtApp::m_szInitialWorkingDirectory), nLength);

	// remote backup directory
	nLength = CCedtApp::m_szRemoteBackupDirectory.GetLength();
	fout.write((const char *)(& nLength), sizeof(INT));
	if( nLength ) fout.write((const char *)(CCedtApp::m_szRemoteBackupDirectory), nLength);

	// application initialization options
	fout.write((const char *)(& CCedtApp::m_bCreateNewDocumentOnStartup), sizeof(BOOL));
	fout.write((const char *)(& CCedtApp::m_bReloadWorkingFilesOnStartup), sizeof(BOOL));
	fout.write((const char *)(& CCedtApp::m_bCheckIfFilesModifiedOutside), sizeof(BOOL));
	fout.write((const char *)(& CCedtApp::m_bCloseTabUsingDoubleClick), sizeof(BOOL));

	// file filters
	for( i = 0; i < MAX_FILE_FILTER; i++ ) {
		CCedtApp::m_clsFileFilters[i].StreamSave(fout);
	}

	fout.write((const char *)(& CCedtApp::m_nFilterIndexDialog), sizeof(INT));
	fout.write((const char *)(& CCedtApp::m_nFilterIndexPannel), sizeof(INT));

	fout.close();
	return TRUE;
}


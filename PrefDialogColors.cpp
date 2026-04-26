#include "stdafx.h"
#include "cedtHeader.h"
#include "PrefDialog.h"
#include "cedtColors.h"


void CPreferenceDialog::InitColorsPage()
{
	CString szText; szText.LoadString( IDS_PREF_GENERAL_COLORS );
	m_tabColorsPannel.InsertItem(COLORS_PANNEL_GENERAL, szText, 0);
	szText.LoadString( IDS_PREF_KEYWORD_COLORS );
	m_tabColorsPannel.InsertItem(COLORS_PANNEL_KEYWORD, szText, 0);
	szText.LoadString( IDS_PREF_MISC_COLORS );
	m_tabColorsPannel.InsertItem(COLORS_PANNEL_MISC,    szText,  0);
	m_tabColorsPannel.SetCurSel( m_nActiveColorsPannel );

//	following color scheme codes are added in resource file
//	m_cmbColorSchemeList.InsertString(0, "default color scheme");
//	m_cmbColorSchemeList.InsertString(1, "light gray color scheme");
//	m_cmbColorSchemeList.InsertString(2, "simplified color scheme");
//	m_cmbColorSchemeList.InsertString(3, "reversed color scheme");
//	m_cmbColorSchemeList.InsertString(4, "saved color schemes..."); 

	m_cmbColorSchemeList.SetCurSel( m_nActiveColorScheme );
}

void CPreferenceDialog::SizeColorsPage()
{
	INT nPosY;

	nPosY  =  26; m_tabColorsPannel.MoveWindow(170, nPosY, 360, 200);

	nPosY  =  66; m_stcBackgroundColor.MoveWindow(180, nPosY, 300, 14);
	nPosY +=  22; m_btnBackgroundColor.MoveWindow(180, nPosY-3, 140, 18);	m_btnActiveLineColor.MoveWindow(360, nPosY-3, 140, 18);
	nPosY +=  22; m_stcTextColor.MoveWindow(180, nPosY, 300, 14);		
	nPosY +=  22; m_btnWordColor.MoveWindow(180, nPosY-3, 140, 18);			m_btnConstantColor.MoveWindow(360, nPosY-3, 140, 18);
	nPosY +=  22; m_btnCommentColor.MoveWindow(180, nPosY-3, 140, 18);		m_btnStringColor.MoveWindow(360, nPosY-3, 140, 18);
	nPosY +=  22; m_btnDelimiterColor.MoveWindow(180, nPosY-3, 140, 18);	m_btnVariableColor.MoveWindow(360, nPosY-3, 140, 18);

	nPosY  =  66; m_stcKeywordColor.MoveWindow(180, nPosY, 300, 14);
	nPosY +=  22; m_btnKeyword0Color.MoveWindow(180, nPosY-3, 140, 18);		m_btnKeyword1Color.MoveWindow(360, nPosY-3, 140, 18);
	nPosY +=  22; m_btnKeyword2Color.MoveWindow(180, nPosY-3, 140, 18);		m_btnKeyword3Color.MoveWindow(360, nPosY-3, 140, 18);
	nPosY +=  22; m_btnKeyword4Color.MoveWindow(180, nPosY-3, 140, 18);		m_btnKeyword5Color.MoveWindow(360, nPosY-3, 140, 18);
	nPosY +=  22; m_btnKeyword6Color.MoveWindow(180, nPosY-3, 140, 18);		m_btnKeyword7Color.MoveWindow(360, nPosY-3, 140, 18);
	nPosY +=  22; m_btnKeyword8Color.MoveWindow(180, nPosY-3, 140, 18);		m_btnKeyword9Color.MoveWindow(360, nPosY-3, 140, 18);

	nPosY  =  66; m_stcLeftMarginColor.MoveWindow(180, nPosY, 300, 14);
	nPosY +=  22; m_btnLeftMarginColor.MoveWindow(180, nPosY-3, 140, 18); 	m_btnLineNumberColor.MoveWindow(360, nPosY-3, 140, 18);
	nPosY +=  22; m_stcRangeBkgrColor.MoveWindow(180, nPosY, 300, 14);
	nPosY +=  22; m_btnRange1BkgrColor.MoveWindow(180, nPosY-3, 140, 18);	m_btnRange2BkgrColor.MoveWindow(360, nPosY-3, 140, 18);
	nPosY +=  22; m_stcHighlightedColor.MoveWindow(180, nPosY, 300, 14);
	nPosY +=  22; m_btnHighlightedColor.MoveWindow(180, nPosY-3, 140, 18);	m_btnShadowedColor.MoveWindow(360, nPosY-3, 140, 18);

	nPosY  =  30 + 200;
	nPosY +=  10; m_stcColorSchemeBox.MoveWindow(170, nPosY, 360, 80);
	nPosY +=  10; m_stcLoadColorScheme.MoveWindow(180, nPosY, 240, 14);
	nPosY +=  20; m_cmbColorSchemeList.MoveWindow(180, nPosY-3, 240, 96);	m_btnLoadColorScheme.MoveWindow(430, nPosY-3, 70, 18);
	nPosY +=  25; m_stcSaveColorScheme.MoveWindow(180, nPosY, 240, 14);		m_btnSaveColorScheme.MoveWindow(430, nPosY-3, 70, 18);
}

void CPreferenceDialog::ShowColorsPage()
{
	INT nCmdShow = (m_nActiveCategory == PREF_CATEGORY_COLORS) ? SW_SHOW : SW_HIDE;

	m_tabColorsPannel.ShowWindow(nCmdShow);
	INT nColorsPannel = m_tabColorsPannel.GetCurSel();

	nCmdShow = (m_nActiveCategory == PREF_CATEGORY_COLORS && nColorsPannel == 0) ? SW_SHOW : SW_HIDE;
	m_stcBackgroundColor.ShowWindow(nCmdShow);
	m_btnBackgroundColor.ShowWindow(nCmdShow);	
	m_btnActiveLineColor.ShowWindow(nCmdShow);
	m_stcTextColor.ShowWindow(nCmdShow);
	m_btnWordColor.ShowWindow(nCmdShow);		m_btnConstantColor.ShowWindow(nCmdShow);
	m_btnCommentColor.ShowWindow(nCmdShow);		m_btnStringColor.ShowWindow(nCmdShow);
	m_btnDelimiterColor.ShowWindow(nCmdShow);	m_btnVariableColor.ShowWindow(nCmdShow);

	nCmdShow = (m_nActiveCategory == PREF_CATEGORY_COLORS && nColorsPannel == 1) ? SW_SHOW : SW_HIDE;
	m_stcKeywordColor.ShowWindow(nCmdShow);
	m_btnKeyword0Color.ShowWindow(nCmdShow);	m_btnKeyword1Color.ShowWindow(nCmdShow);
	m_btnKeyword2Color.ShowWindow(nCmdShow);	m_btnKeyword3Color.ShowWindow(nCmdShow);
	m_btnKeyword4Color.ShowWindow(nCmdShow);	m_btnKeyword5Color.ShowWindow(nCmdShow);
	m_btnKeyword6Color.ShowWindow(nCmdShow);	m_btnKeyword7Color.ShowWindow(nCmdShow);
	m_btnKeyword8Color.ShowWindow(nCmdShow);	m_btnKeyword9Color.ShowWindow(nCmdShow);

	nCmdShow = (m_nActiveCategory == PREF_CATEGORY_COLORS && nColorsPannel == 2) ? SW_SHOW : SW_HIDE;
	m_stcLeftMarginColor.ShowWindow(nCmdShow);
	m_btnLeftMarginColor.ShowWindow(nCmdShow);	m_btnLineNumberColor.ShowWindow(nCmdShow);
	m_stcRangeBkgrColor.ShowWindow(nCmdShow);
	m_btnRange1BkgrColor.ShowWindow(nCmdShow);	m_btnRange2BkgrColor.ShowWindow(nCmdShow);
	m_stcHighlightedColor.ShowWindow(nCmdShow);
	m_btnHighlightedColor.ShowWindow(nCmdShow);	m_btnShadowedColor.ShowWindow(nCmdShow);

	nCmdShow = (m_nActiveCategory == PREF_CATEGORY_COLORS) ? SW_SHOW : SW_HIDE;
	m_stcColorSchemeBox.ShowWindow(nCmdShow);
	m_stcLoadColorScheme.ShowWindow(nCmdShow);
	m_cmbColorSchemeList.ShowWindow(nCmdShow);	m_btnLoadColorScheme.ShowWindow(nCmdShow);
	m_stcSaveColorScheme.ShowWindow(nCmdShow);	m_btnSaveColorScheme.ShowWindow(nCmdShow);
}


BOOL CPreferenceDialog::LoadColorSettings()
{
	memcpy((void *)m_crBkgrColor, (const void *)CCedtView::m_crBkgrColor, sizeof(m_crBkgrColor));
	memcpy((void *)m_crTextColor, (const void *)CCedtView::m_crTextColor, sizeof(m_crTextColor));

	return TRUE;
}

BOOL CPreferenceDialog::SaveColorSettings()
{
	memcpy((void *)CCedtView::m_crBkgrColor, (const void *)m_crBkgrColor, sizeof(m_crBkgrColor));
	memcpy((void *)CCedtView::m_crTextColor, (const void *)m_crTextColor, sizeof(m_crTextColor));

	return TRUE;
}

void CPreferenceDialog::InvalidateColorButtons()
{
	m_btnBackgroundColor.Invalidate();	m_btnActiveLineColor.Invalidate();

	m_btnWordColor.Invalidate();		m_btnConstantColor.Invalidate();
	m_btnCommentColor.Invalidate();		m_btnStringColor.Invalidate();
	m_btnDelimiterColor.Invalidate();	m_btnVariableColor.Invalidate();

	m_btnKeyword0Color.Invalidate();	m_btnKeyword1Color.Invalidate();
	m_btnKeyword2Color.Invalidate();	m_btnKeyword3Color.Invalidate();
	m_btnKeyword4Color.Invalidate();	m_btnKeyword5Color.Invalidate();
	m_btnKeyword6Color.Invalidate();	m_btnKeyword7Color.Invalidate();
	m_btnKeyword8Color.Invalidate();	m_btnKeyword9Color.Invalidate();

	m_btnLeftMarginColor.Invalidate();	m_btnLineNumberColor.Invalidate();
	m_btnHighlightedColor.Invalidate();	m_btnShadowedColor.Invalidate();
	m_btnRange1BkgrColor.Invalidate();	m_btnRange2BkgrColor.Invalidate();
}

BOOL CPreferenceDialog::SetPredefinedColorScheme(INT nScheme)
{
	switch( nScheme ) {
	case COLOR_SCHEME_DEFAULT:
		m_crBkgrColor[0x00]				= CR_DEFAULT_BACKGROUND;
		m_crBkgrColor[0x01]				= CR_DEFAULT_LEFTMARGIN;
		m_crBkgrColor[0x02]				= CR_DEFAULT_CURRENTLINE;
		m_crBkgrColor[0x03]				= CR_DEFAULT_RANGE1BKGR;
		m_crBkgrColor[0x04]				= CR_DEFAULT_RANGE2BKGR;

		m_crTextColor[WT_LINEBREAK]		= CR_DEFAULT_LINENUMBER;
		m_crTextColor[WT_CONSTANT]		= CR_DEFAULT_CONSTANT;
		m_crTextColor[WT_QUOTATION0]	= CR_DEFAULT_STRING;
		m_crTextColor[WT_LINECOMMENT]	= CR_DEFAULT_COMMENT;
		m_crTextColor[WT_VARIABLE]		= CR_DEFAULT_VARIABLE;
		m_crTextColor[WT_IDENTIFIER]	= CR_DEFAULT_WORD;
		m_crTextColor[WT_WRONGWORD]		= CR_DEFAULT_WORD;
		m_crTextColor[WT_DBCHAR]		= CR_DEFAULT_WORD;
		m_crTextColor[WT_GRAPH]			= CR_DEFAULT_WORD;
		m_crTextColor[WT_RETURN]		= CR_DEFAULT_DELIMITER;
		m_crTextColor[WT_TAB]			= CR_DEFAULT_DELIMITER;
		m_crTextColor[WT_SPACE]			= CR_DEFAULT_DELIMITER;
		m_crTextColor[WT_DELIMITER]		= CR_DEFAULT_DELIMITER;
		m_crTextColor[WT_HIGHLIGHTON]	= CR_DEFAULT_HIGHLIGHTED;
		m_crTextColor[WT_SHADOWON]		= CR_DEFAULT_SHADOWED;

		m_crTextColor[WT_KEYWORD0]		= CR_DEFAULT_KEYWORD0;
		m_crTextColor[WT_KEYWORD1]		= CR_DEFAULT_KEYWORD1;
		m_crTextColor[WT_KEYWORD2]		= CR_DEFAULT_KEYWORD2;
		m_crTextColor[WT_KEYWORD3]		= CR_DEFAULT_KEYWORD3;
		m_crTextColor[WT_KEYWORD4]		= CR_DEFAULT_KEYWORD4;
		m_crTextColor[WT_KEYWORD5]		= CR_DEFAULT_KEYWORD5;
		m_crTextColor[WT_KEYWORD6]		= CR_DEFAULT_KEYWORD6;
		m_crTextColor[WT_KEYWORD7]		= CR_DEFAULT_KEYWORD7;
		m_crTextColor[WT_KEYWORD8]		= CR_DEFAULT_KEYWORD8;
		m_crTextColor[WT_KEYWORD9]		= CR_DEFAULT_KEYWORD9;
		return TRUE;

	case COLOR_SCHEME_SIMPLIFIED:
		m_crBkgrColor[0x00]				= CR_SIMPLIFIED_BACKGROUND;
		m_crBkgrColor[0x01]				= CR_SIMPLIFIED_LEFTMARGIN;
		m_crBkgrColor[0x02]				= CR_SIMPLIFIED_CURRENTLINE;
		m_crBkgrColor[0x03]				= CR_SIMPLIFIED_RANGE1BKGR;
		m_crBkgrColor[0x04]				= CR_SIMPLIFIED_RANGE2BKGR;

		m_crTextColor[WT_LINEBREAK]		= CR_SIMPLIFIED_LINENUMBER;
		m_crTextColor[WT_CONSTANT]		= CR_SIMPLIFIED_CONSTANT;
		m_crTextColor[WT_QUOTATION0]	= CR_SIMPLIFIED_STRING;
		m_crTextColor[WT_LINECOMMENT]	= CR_SIMPLIFIED_COMMENT;
		m_crTextColor[WT_VARIABLE]		= CR_SIMPLIFIED_VARIABLE;
		m_crTextColor[WT_IDENTIFIER]	= CR_SIMPLIFIED_WORD;
		m_crTextColor[WT_WRONGWORD]		= CR_SIMPLIFIED_WORD;
		m_crTextColor[WT_DBCHAR]		= CR_SIMPLIFIED_WORD;
		m_crTextColor[WT_GRAPH]			= CR_SIMPLIFIED_WORD;
		m_crTextColor[WT_RETURN]		= CR_SIMPLIFIED_DELIMITER;
		m_crTextColor[WT_TAB]			= CR_SIMPLIFIED_DELIMITER;
		m_crTextColor[WT_SPACE]			= CR_SIMPLIFIED_DELIMITER;
		m_crTextColor[WT_DELIMITER]		= CR_SIMPLIFIED_DELIMITER;
		m_crTextColor[WT_HIGHLIGHTON]	= CR_SIMPLIFIED_HIGHLIGHTED;
		m_crTextColor[WT_SHADOWON]		= CR_SIMPLIFIED_SHADOWED;

		m_crTextColor[WT_KEYWORD0]		= CR_SIMPLIFIED_KEYWORD0;
		m_crTextColor[WT_KEYWORD1]		= CR_SIMPLIFIED_KEYWORD1;
		m_crTextColor[WT_KEYWORD2]		= CR_SIMPLIFIED_KEYWORD2;
		m_crTextColor[WT_KEYWORD3]		= CR_SIMPLIFIED_KEYWORD3;
		m_crTextColor[WT_KEYWORD4]		= CR_SIMPLIFIED_KEYWORD4;
		m_crTextColor[WT_KEYWORD5]		= CR_SIMPLIFIED_KEYWORD5;
		m_crTextColor[WT_KEYWORD6]		= CR_SIMPLIFIED_KEYWORD6;
		m_crTextColor[WT_KEYWORD7]		= CR_SIMPLIFIED_KEYWORD7;
		m_crTextColor[WT_KEYWORD8]		= CR_SIMPLIFIED_KEYWORD8;
		m_crTextColor[WT_KEYWORD9]		= CR_SIMPLIFIED_KEYWORD9;
		return TRUE;

	case COLOR_SCHEME_REVERSED:
		m_crBkgrColor[0x00]				= CR_REVERSED_BACKGROUND;
		m_crBkgrColor[0x01]				= CR_REVERSED_LEFTMARGIN;
		m_crBkgrColor[0x02]				= CR_REVERSED_CURRENTLINE;
		m_crBkgrColor[0x03]				= CR_REVERSED_RANGE1BKGR;
		m_crBkgrColor[0x04]				= CR_REVERSED_RANGE2BKGR;

		m_crTextColor[WT_LINEBREAK]		= CR_REVERSED_LINENUMBER;
		m_crTextColor[WT_CONSTANT]		= CR_REVERSED_CONSTANT;
		m_crTextColor[WT_QUOTATION0]	= CR_REVERSED_STRING;
		m_crTextColor[WT_LINECOMMENT]	= CR_REVERSED_COMMENT;
		m_crTextColor[WT_VARIABLE]		= CR_REVERSED_VARIABLE;
		m_crTextColor[WT_IDENTIFIER]	= CR_REVERSED_WORD;
		m_crTextColor[WT_WRONGWORD]		= CR_REVERSED_WORD;
		m_crTextColor[WT_DBCHAR]		= CR_REVERSED_WORD;
		m_crTextColor[WT_GRAPH]			= CR_REVERSED_WORD;
		m_crTextColor[WT_RETURN]		= CR_REVERSED_DELIMITER;
		m_crTextColor[WT_TAB]			= CR_REVERSED_DELIMITER;
		m_crTextColor[WT_SPACE]			= CR_REVERSED_DELIMITER;
		m_crTextColor[WT_DELIMITER]		= CR_REVERSED_DELIMITER;
		m_crTextColor[WT_HIGHLIGHTON]	= CR_REVERSED_HIGHLIGHTED;
		m_crTextColor[WT_SHADOWON]		= CR_REVERSED_SHADOWED;

		m_crTextColor[WT_KEYWORD0]		= CR_REVERSED_KEYWORD0;
		m_crTextColor[WT_KEYWORD1]		= CR_REVERSED_KEYWORD1;
		m_crTextColor[WT_KEYWORD2]		= CR_REVERSED_KEYWORD2;
		m_crTextColor[WT_KEYWORD3]		= CR_REVERSED_KEYWORD3;
		m_crTextColor[WT_KEYWORD4]		= CR_REVERSED_KEYWORD4;
		m_crTextColor[WT_KEYWORD5]		= CR_REVERSED_KEYWORD5;
		m_crTextColor[WT_KEYWORD6]		= CR_REVERSED_KEYWORD6;
		m_crTextColor[WT_KEYWORD7]		= CR_REVERSED_KEYWORD7;
		m_crTextColor[WT_KEYWORD8]		= CR_REVERSED_KEYWORD8;
		m_crTextColor[WT_KEYWORD9]		= CR_REVERSED_KEYWORD9;
		return TRUE;

	case COLOR_SCHEME_LIGHTGRAY:
		m_crBkgrColor[0x00]				= CR_LIGHTGRAY_BACKGROUND;
		m_crBkgrColor[0x01]				= CR_LIGHTGRAY_LEFTMARGIN;
		m_crBkgrColor[0x02]				= CR_LIGHTGRAY_CURRENTLINE;
		m_crBkgrColor[0x03]				= CR_LIGHTGRAY_RANGE1BKGR;
		m_crBkgrColor[0x04]				= CR_LIGHTGRAY_RANGE2BKGR;

		m_crTextColor[WT_LINEBREAK]		= CR_LIGHTGRAY_LINENUMBER;
		m_crTextColor[WT_CONSTANT]		= CR_LIGHTGRAY_CONSTANT;
		m_crTextColor[WT_QUOTATION0]	= CR_LIGHTGRAY_STRING;
		m_crTextColor[WT_LINECOMMENT]	= CR_LIGHTGRAY_COMMENT;
		m_crTextColor[WT_VARIABLE]		= CR_LIGHTGRAY_VARIABLE;
		m_crTextColor[WT_IDENTIFIER]	= CR_LIGHTGRAY_WORD;
		m_crTextColor[WT_WRONGWORD]		= CR_LIGHTGRAY_WORD;
		m_crTextColor[WT_DBCHAR]		= CR_LIGHTGRAY_WORD;
		m_crTextColor[WT_GRAPH]			= CR_LIGHTGRAY_WORD;
		m_crTextColor[WT_RETURN]		= CR_LIGHTGRAY_DELIMITER;
		m_crTextColor[WT_TAB]			= CR_LIGHTGRAY_DELIMITER;
		m_crTextColor[WT_SPACE]			= CR_LIGHTGRAY_DELIMITER;
		m_crTextColor[WT_DELIMITER]		= CR_LIGHTGRAY_DELIMITER;
		m_crTextColor[WT_HIGHLIGHTON]	= CR_LIGHTGRAY_HIGHLIGHTED;
		m_crTextColor[WT_SHADOWON]		= CR_LIGHTGRAY_SHADOWED;

		m_crTextColor[WT_KEYWORD0]		= CR_LIGHTGRAY_KEYWORD0;
		m_crTextColor[WT_KEYWORD1]		= CR_LIGHTGRAY_KEYWORD1;
		m_crTextColor[WT_KEYWORD2]		= CR_LIGHTGRAY_KEYWORD2;
		m_crTextColor[WT_KEYWORD3]		= CR_LIGHTGRAY_KEYWORD3;
		m_crTextColor[WT_KEYWORD4]		= CR_LIGHTGRAY_KEYWORD4;
		m_crTextColor[WT_KEYWORD5]		= CR_LIGHTGRAY_KEYWORD5;
		m_crTextColor[WT_KEYWORD6]		= CR_LIGHTGRAY_KEYWORD6;
		m_crTextColor[WT_KEYWORD7]		= CR_LIGHTGRAY_KEYWORD7;
		m_crTextColor[WT_KEYWORD8]		= CR_LIGHTGRAY_KEYWORD8;
		m_crTextColor[WT_KEYWORD9]		= CR_LIGHTGRAY_KEYWORD9;
		return TRUE;

	case COLOR_SCHEME_DARKBLUE:
		m_crBkgrColor[0x00]				= CR_DARKBLUE_BACKGROUND;
		m_crBkgrColor[0x01]				= CR_DARKBLUE_LEFTMARGIN;
		m_crBkgrColor[0x02]				= CR_DARKBLUE_CURRENTLINE;
		m_crBkgrColor[0x03]				= CR_DARKBLUE_RANGE1BKGR;
		m_crBkgrColor[0x04]				= CR_DARKBLUE_RANGE2BKGR;

		m_crTextColor[WT_LINEBREAK]		= CR_DARKBLUE_LINENUMBER;
		m_crTextColor[WT_CONSTANT]		= CR_DARKBLUE_CONSTANT;
		m_crTextColor[WT_QUOTATION0]	= CR_DARKBLUE_STRING;
		m_crTextColor[WT_LINECOMMENT]	= CR_DARKBLUE_COMMENT;
		m_crTextColor[WT_VARIABLE]		= CR_DARKBLUE_VARIABLE;
		m_crTextColor[WT_IDENTIFIER]	= CR_DARKBLUE_WORD;
		m_crTextColor[WT_WRONGWORD]		= CR_DARKBLUE_WORD;
		m_crTextColor[WT_DBCHAR]		= CR_DARKBLUE_WORD;
		m_crTextColor[WT_GRAPH]			= CR_DARKBLUE_WORD;
		m_crTextColor[WT_RETURN]		= CR_DARKBLUE_DELIMITER;
		m_crTextColor[WT_TAB]			= CR_DARKBLUE_DELIMITER;
		m_crTextColor[WT_SPACE]			= CR_DARKBLUE_DELIMITER;
		m_crTextColor[WT_DELIMITER]		= CR_DARKBLUE_DELIMITER;
		m_crTextColor[WT_HIGHLIGHTON]	= CR_DARKBLUE_HIGHLIGHTED;
		m_crTextColor[WT_SHADOWON]		= CR_DARKBLUE_SHADOWED;

		m_crTextColor[WT_KEYWORD0]		= CR_DARKBLUE_KEYWORD0;
		m_crTextColor[WT_KEYWORD1]		= CR_DARKBLUE_KEYWORD1;
		m_crTextColor[WT_KEYWORD2]		= CR_DARKBLUE_KEYWORD2;
		m_crTextColor[WT_KEYWORD3]		= CR_DARKBLUE_KEYWORD3;
		m_crTextColor[WT_KEYWORD4]		= CR_DARKBLUE_KEYWORD4;
		m_crTextColor[WT_KEYWORD5]		= CR_DARKBLUE_KEYWORD5;
		m_crTextColor[WT_KEYWORD6]		= CR_DARKBLUE_KEYWORD6;
		m_crTextColor[WT_KEYWORD7]		= CR_DARKBLUE_KEYWORD7;
		m_crTextColor[WT_KEYWORD8]		= CR_DARKBLUE_KEYWORD8;
		m_crTextColor[WT_KEYWORD9]		= CR_DARKBLUE_KEYWORD9;
		return TRUE;

	default:
		return FALSE;
	}
}

BOOL CPreferenceDialog::LoadColorScheme(LPCTSTR lpszPathName)
{
	ifstream fin(lpszPathName, ios::in | ios::nocreate | ios::binary);
	if( ! fin.is_open() ) return FALSE;

	TCHAR szBuffer[2048]; INT nLength = strlen(STRING_COLORSETTINGSVER); fin.read((char *)szBuffer, nLength); szBuffer[nLength] = '\0';
	if( strcmp(szBuffer, STRING_COLORSETTINGSVER) ) { fin.close(); return FALSE; }

	fin.read((char *)m_crBkgrColor, sizeof(m_crBkgrColor));
	if( ! fin.good() ) { fin.close(); return FALSE; }
	fin.read((char *)m_crTextColor, sizeof(m_crTextColor));
	if( ! fin.good() ) { fin.close(); return FALSE; }

	if( fin.peek() != EOF ) { fin.close(); return FALSE; }
	fin.close();

	return TRUE;
}

BOOL CPreferenceDialog::SaveColorScheme(LPCTSTR lpszPathName)
{
	ofstream fout(lpszPathName, ios::out | ios::binary);
	if( ! fout.is_open() ) return FALSE;

	INT nLength = strlen(STRING_COLORSETTINGSVER);
	fout.write((const char *)STRING_COLORSETTINGSVER, nLength);

	fout.write((const char *)m_crBkgrColor, sizeof(m_crBkgrColor));
	fout.write((const char *)m_crTextColor, sizeof(m_crTextColor));

	fout.close();

	return TRUE;
}

void CPreferenceDialog::OnSelchangeColorsPannel(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_nActiveColorsPannel = m_tabColorsPannel.GetCurSel();
	ShowAllPrefPages();
	*pResult = 0;
}

void CPreferenceDialog::OnSelchangeColorSchemeList() 
{
	m_nActiveColorScheme = m_cmbColorSchemeList.GetCurSel();
}

void CPreferenceDialog::OnBackgroundColor() 
{
	CColorDialog dlg(m_crBkgrColor[0x00]);
	if( dlg.DoModal() == IDOK ) {
		m_crBkgrColor[0x00] = dlg.GetColor();
		InvalidateColorButtons();
	}
}

void CPreferenceDialog::OnActiveLineColor() 
{
	CColorDialog dlg(m_crBkgrColor[0x02]);
	if( dlg.DoModal() == IDOK ) {
		m_crBkgrColor[0x02] = dlg.GetColor();
		InvalidateColorButtons();
	}
}

void CPreferenceDialog::OnWordColor() 
{
	CColorDialog dlg(m_crTextColor[WT_IDENTIFIER]);
	if( dlg.DoModal() == IDOK ) {
		m_crTextColor[WT_IDENTIFIER] = m_crTextColor[WT_WRONGWORD] = dlg.GetColor();
		m_crTextColor[WT_DBCHAR] = m_crTextColor[WT_GRAPH] = dlg.GetColor();
		InvalidateColorButtons();
	}
}

void CPreferenceDialog::OnConstantColor() 
{
	CColorDialog dlg(m_crTextColor[WT_CONSTANT]);
	if( dlg.DoModal() == IDOK ) {
		m_crTextColor[WT_CONSTANT] = dlg.GetColor();
		InvalidateColorButtons();
	}
}

void CPreferenceDialog::OnCommentColor() 
{
	CColorDialog dlg(m_crTextColor[WT_LINECOMMENT]);
	if( dlg.DoModal() == IDOK ) {
		m_crTextColor[WT_LINECOMMENT] = dlg.GetColor();
		InvalidateColorButtons();
	}
}

void CPreferenceDialog::OnStringColor() 
{
	CColorDialog dlg(m_crTextColor[WT_QUOTATION0]);
	if( dlg.DoModal() == IDOK ) {
		m_crTextColor[WT_QUOTATION0] = dlg.GetColor();
		InvalidateColorButtons();
	}
}

void CPreferenceDialog::OnDelimiterColor() 
{
	CColorDialog dlg(m_crTextColor[WT_DELIMITER]);
	if( dlg.DoModal() == IDOK ) {
		m_crTextColor[WT_RETURN] = m_crTextColor[WT_TAB] = dlg.GetColor();
		m_crTextColor[WT_SPACE] = m_crTextColor[WT_DELIMITER] = dlg.GetColor();
		InvalidateColorButtons();
	}
}

void CPreferenceDialog::OnVariableColor() 
{
	CColorDialog dlg(m_crTextColor[WT_VARIABLE]);
	if( dlg.DoModal() == IDOK ) {
		m_crTextColor[WT_VARIABLE] = dlg.GetColor();
		InvalidateColorButtons();
	}
}

void CPreferenceDialog::OnKeyword0Color() 
{
	CColorDialog dlg(m_crTextColor[WT_KEYWORD0]);
	if( dlg.DoModal() == IDOK ) {
		m_crTextColor[WT_KEYWORD0] = dlg.GetColor();
		InvalidateColorButtons();
	}
}

void CPreferenceDialog::OnKeyword1Color() 
{
	CColorDialog dlg(m_crTextColor[WT_KEYWORD1]);
	if( dlg.DoModal() == IDOK ) {
		m_crTextColor[WT_KEYWORD1] = dlg.GetColor();
		InvalidateColorButtons();
	}
}

void CPreferenceDialog::OnKeyword2Color() 
{
	CColorDialog dlg(m_crTextColor[WT_KEYWORD2]);
	if( dlg.DoModal() == IDOK ) {
		m_crTextColor[WT_KEYWORD2] = dlg.GetColor();
		InvalidateColorButtons();
	}
}

void CPreferenceDialog::OnKeyword3Color() 
{
	CColorDialog dlg(m_crTextColor[WT_KEYWORD3]);
	if( dlg.DoModal() == IDOK ) {
		m_crTextColor[WT_KEYWORD3] = dlg.GetColor();
		InvalidateColorButtons();
	}
}

void CPreferenceDialog::OnKeyword4Color() 
{
	CColorDialog dlg(m_crTextColor[WT_KEYWORD4]);
	if( dlg.DoModal() == IDOK ) {
		m_crTextColor[WT_KEYWORD4] = dlg.GetColor();
		InvalidateColorButtons();
	}
}

void CPreferenceDialog::OnKeyword5Color() 
{
	CColorDialog dlg(m_crTextColor[WT_KEYWORD5]);
	if( dlg.DoModal() == IDOK ) {
		m_crTextColor[WT_KEYWORD5] = dlg.GetColor();
		InvalidateColorButtons();
	}
}

void CPreferenceDialog::OnKeyword6Color() 
{
	CColorDialog dlg(m_crTextColor[WT_KEYWORD6]);
	if( dlg.DoModal() == IDOK ) {
		m_crTextColor[WT_KEYWORD6] = dlg.GetColor();
		InvalidateColorButtons();
	}
}

void CPreferenceDialog::OnKeyword7Color() 
{
	CColorDialog dlg(m_crTextColor[WT_KEYWORD7]);
	if( dlg.DoModal() == IDOK ) {
		m_crTextColor[WT_KEYWORD7] = dlg.GetColor();
		InvalidateColorButtons();
	}
}

void CPreferenceDialog::OnKeyword8Color() 
{
	CColorDialog dlg(m_crTextColor[WT_KEYWORD8]);
	if( dlg.DoModal() == IDOK ) {
		m_crTextColor[WT_KEYWORD8] = dlg.GetColor();
		InvalidateColorButtons();
	}
}

void CPreferenceDialog::OnKeyword9Color() 
{
	CColorDialog dlg(m_crTextColor[WT_KEYWORD9]);
	if( dlg.DoModal() == IDOK ) {
		m_crTextColor[WT_KEYWORD9] = dlg.GetColor();
		InvalidateColorButtons();
	}
}

void CPreferenceDialog::OnLeftMarginColor() 
{
	CColorDialog dlg(m_crBkgrColor[0x01]);
	if( dlg.DoModal() == IDOK ) {
		m_crBkgrColor[0x01] = dlg.GetColor();
		InvalidateColorButtons();
	}
}

void CPreferenceDialog::OnLineNumberColor() 
{
	CColorDialog dlg(m_crTextColor[WT_LINEBREAK]);
	if( dlg.DoModal() == IDOK ) {
		m_crTextColor[WT_LINEBREAK] = dlg.GetColor();
		InvalidateColorButtons();
	}
}

void CPreferenceDialog::OnRange1BkgrColor() 
{
	CColorDialog dlg(m_crBkgrColor[0x03]);
	if( dlg.DoModal() == IDOK ) {
		m_crBkgrColor[0x03] = dlg.GetColor();
		InvalidateColorButtons();
	}
}

void CPreferenceDialog::OnRange2BkgrColor() 
{
	CColorDialog dlg(m_crBkgrColor[0x04]);
	if( dlg.DoModal() == IDOK ) {
		m_crBkgrColor[0x04] = dlg.GetColor();
		InvalidateColorButtons();
	}
}

void CPreferenceDialog::OnHighlightedColor() 
{
	CColorDialog dlg(m_crTextColor[WT_HIGHLIGHTON]);
	if( dlg.DoModal() == IDOK ) {
		m_crTextColor[WT_HIGHLIGHTON] = dlg.GetColor();
		InvalidateColorButtons();
	}
}

void CPreferenceDialog::OnShadowedColor() 
{
	CColorDialog dlg(m_crTextColor[WT_SHADOWON]);
	if( dlg.DoModal() == IDOK ) {
		m_crTextColor[WT_SHADOWON] = dlg.GetColor();
		InvalidateColorButtons();
	}
}

void CPreferenceDialog::OnLoadColorScheme() 
{
	INT nScheme = m_cmbColorSchemeList.GetCurSel(); if( nScheme == CB_ERR ) return;

	if( nScheme == COLOR_SCHEME_SAVED ) { // saved color schemes
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
		CString szFilter; szFilter.LoadString(IDS_FILTER_COLOR_SCHEME);
		CFileDialog dlg(TRUE, NULL, NULL, dwFlags, szFilter);

		CString szTitle; szTitle.LoadString(IDS_DLG_LOAD_COLOR_SCHEME);
		CString szInitialDirectory = CCedtApp::m_szInstallDirectory + "\\schemes";

		TCHAR szCurrentDirectory[MAX_PATH];
		GetCurrentDirectory( MAX_PATH, szCurrentDirectory );

		dlg.m_ofn.lpstrTitle = szTitle; dlg.m_ofn.lpstrInitialDir = szInitialDirectory;
		INT nResult = dlg.DoModal();

		SetCurrentDirectory( szCurrentDirectory );
		if( nResult != IDOK ) return;

		LoadColorScheme( dlg.GetPathName() );

	} else SetPredefinedColorScheme( nScheme );

	InvalidateColorButtons();
}

void CPreferenceDialog::OnSaveColorScheme() 
{
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CString szFilter; szFilter.LoadString(IDS_FILTER_COLOR_SCHEME);
	CFileDialog dlg(FALSE, ".color", NULL, dwFlags, szFilter);

	CString szTitle; szTitle.LoadString(IDS_DLG_SAVE_COLOR_SCHEME);
	CString szInitialDirectory = CCedtApp::m_szInstallDirectory + "\\schemes";

	TCHAR szCurrentDirectory[MAX_PATH];
	GetCurrentDirectory( MAX_PATH, szCurrentDirectory );

	dlg.m_ofn.lpstrTitle = szTitle; dlg.m_ofn.lpstrInitialDir = szInitialDirectory;
	INT nResult = dlg.DoModal();

	SetCurrentDirectory( szCurrentDirectory );
	if( nResult != IDOK ) return;

	SaveColorScheme( dlg.GetPathName() );
}

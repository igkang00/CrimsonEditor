#include "stdafx.h"
#include "cedtHeader.h"
#include "DocumentSummary.h"


// ON UPDATE DOCUMENT SYNTAX TYPE
void CCedtDoc::OnUpdateDocumentSyntaxType0 (CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI,  0); }
void CCedtDoc::OnUpdateDocumentSyntaxType1 (CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI,  1); }
void CCedtDoc::OnUpdateDocumentSyntaxType2 (CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI,  2); }
void CCedtDoc::OnUpdateDocumentSyntaxType3 (CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI,  3); }
void CCedtDoc::OnUpdateDocumentSyntaxType4 (CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI,  4); }
void CCedtDoc::OnUpdateDocumentSyntaxType5 (CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI,  5); }
void CCedtDoc::OnUpdateDocumentSyntaxType6 (CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI,  6); }
void CCedtDoc::OnUpdateDocumentSyntaxType7 (CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI,  7); }
void CCedtDoc::OnUpdateDocumentSyntaxType8 (CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI,  8); }
void CCedtDoc::OnUpdateDocumentSyntaxType9 (CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI,  9); }
void CCedtDoc::OnUpdateDocumentSyntaxType10(CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI, 10); }
void CCedtDoc::OnUpdateDocumentSyntaxType11(CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI, 11); }
void CCedtDoc::OnUpdateDocumentSyntaxType12(CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI, 12); }
void CCedtDoc::OnUpdateDocumentSyntaxType13(CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI, 13); }
void CCedtDoc::OnUpdateDocumentSyntaxType14(CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI, 14); }
void CCedtDoc::OnUpdateDocumentSyntaxType15(CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI, 15); }
void CCedtDoc::OnUpdateDocumentSyntaxType16(CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI, 16); }
void CCedtDoc::OnUpdateDocumentSyntaxType17(CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI, 17); }
void CCedtDoc::OnUpdateDocumentSyntaxType18(CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI, 18); }
void CCedtDoc::OnUpdateDocumentSyntaxType19(CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI, 19); }
void CCedtDoc::OnUpdateDocumentSyntaxType20(CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI, 20); }
void CCedtDoc::OnUpdateDocumentSyntaxType21(CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI, 21); }
void CCedtDoc::OnUpdateDocumentSyntaxType22(CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI, 22); }
void CCedtDoc::OnUpdateDocumentSyntaxType23(CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI, 23); }
void CCedtDoc::OnUpdateDocumentSyntaxType24(CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI, 24); }
void CCedtDoc::OnUpdateDocumentSyntaxType25(CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI, 25); }
void CCedtDoc::OnUpdateDocumentSyntaxType26(CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI, 26); }
void CCedtDoc::OnUpdateDocumentSyntaxType27(CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI, 27); }
void CCedtDoc::OnUpdateDocumentSyntaxType28(CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI, 28); }
void CCedtDoc::OnUpdateDocumentSyntaxType29(CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI, 29); }
void CCedtDoc::OnUpdateDocumentSyntaxType30(CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI, 30); }
void CCedtDoc::OnUpdateDocumentSyntaxType31(CCmdUI* pCmdUI) { OnUpdateDocumentSyntaxType(pCmdUI, 31); }

void CCedtDoc::OnUpdateDocumentSyntaxAuto(CCmdUI* pCmdUI) 
{
	CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();
	pFrame->RefreshSyntaxTypeMenu();

	BOOL bFound = FALSE;
	for(INT i = 0; i < MAX_SYNTAX_TYPE; i++) {
		CSyntaxType & rSyntaxType = m_clsSyntaxTypes[i];
		if( rSyntaxType.m_szDescription.GetLength() && ! rSyntaxType.m_szLangSpecFile.CompareNoCase(m_szLangSpecFile)
			&& ! rSyntaxType.m_szKeywordsFile.CompareNoCase(m_szKeywordsFile) ) { bFound = TRUE; break; }
	}
	pCmdUI->SetRadio( m_szLangSpecFile.GetLength() && m_szKeywordsFile.GetLength() && ! bFound );
}

void CCedtDoc::OnUpdateDocumentSyntaxText(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio( ! m_szLangSpecFile.GetLength() || ! m_szKeywordsFile.GetLength() );
}

void CCedtDoc::OnUpdateDocumentSyntaxType(CCmdUI * pCmdUI, INT nSyntaxType)
{
	CSyntaxType & rSyntaxType = m_clsSyntaxTypes[nSyntaxType];
	pCmdUI->SetRadio( rSyntaxType.m_szDescription.GetLength() && ! rSyntaxType.m_szLangSpecFile.CompareNoCase(m_szLangSpecFile)
					  && ! rSyntaxType.m_szKeywordsFile.CompareNoCase(m_szKeywordsFile) );
}

// ON DOCUMENT SYNTAX TYPE
void CCedtDoc::OnDocumentSyntaxType0 () { OnDocumentSyntaxType( 0); }
void CCedtDoc::OnDocumentSyntaxType1 () { OnDocumentSyntaxType( 1); }
void CCedtDoc::OnDocumentSyntaxType2 () { OnDocumentSyntaxType( 2); }
void CCedtDoc::OnDocumentSyntaxType3 () { OnDocumentSyntaxType( 3); }
void CCedtDoc::OnDocumentSyntaxType4 () { OnDocumentSyntaxType( 4); }
void CCedtDoc::OnDocumentSyntaxType5 () { OnDocumentSyntaxType( 5); }
void CCedtDoc::OnDocumentSyntaxType6 () { OnDocumentSyntaxType( 6); }
void CCedtDoc::OnDocumentSyntaxType7 () { OnDocumentSyntaxType( 7); }
void CCedtDoc::OnDocumentSyntaxType8 () { OnDocumentSyntaxType( 8); }
void CCedtDoc::OnDocumentSyntaxType9 () { OnDocumentSyntaxType( 9); }
void CCedtDoc::OnDocumentSyntaxType10() { OnDocumentSyntaxType(10); }
void CCedtDoc::OnDocumentSyntaxType11() { OnDocumentSyntaxType(11); }
void CCedtDoc::OnDocumentSyntaxType12() { OnDocumentSyntaxType(12); }
void CCedtDoc::OnDocumentSyntaxType13() { OnDocumentSyntaxType(13); }
void CCedtDoc::OnDocumentSyntaxType14() { OnDocumentSyntaxType(14); }
void CCedtDoc::OnDocumentSyntaxType15() { OnDocumentSyntaxType(15); }
void CCedtDoc::OnDocumentSyntaxType16() { OnDocumentSyntaxType(16); }
void CCedtDoc::OnDocumentSyntaxType17() { OnDocumentSyntaxType(17); }
void CCedtDoc::OnDocumentSyntaxType18() { OnDocumentSyntaxType(18); }
void CCedtDoc::OnDocumentSyntaxType19() { OnDocumentSyntaxType(19); }
void CCedtDoc::OnDocumentSyntaxType20() { OnDocumentSyntaxType(20); }
void CCedtDoc::OnDocumentSyntaxType21() { OnDocumentSyntaxType(21); }
void CCedtDoc::OnDocumentSyntaxType22() { OnDocumentSyntaxType(22); }
void CCedtDoc::OnDocumentSyntaxType23() { OnDocumentSyntaxType(23); }
void CCedtDoc::OnDocumentSyntaxType24() { OnDocumentSyntaxType(24); }
void CCedtDoc::OnDocumentSyntaxType25() { OnDocumentSyntaxType(25); }
void CCedtDoc::OnDocumentSyntaxType26() { OnDocumentSyntaxType(26); }
void CCedtDoc::OnDocumentSyntaxType27() { OnDocumentSyntaxType(27); }
void CCedtDoc::OnDocumentSyntaxType28() { OnDocumentSyntaxType(28); }
void CCedtDoc::OnDocumentSyntaxType29() { OnDocumentSyntaxType(29); }
void CCedtDoc::OnDocumentSyntaxType30() { OnDocumentSyntaxType(30); }
void CCedtDoc::OnDocumentSyntaxType31() { OnDocumentSyntaxType(31); }

void CCedtDoc::OnDocumentSyntaxAuto() 
{
	m_szLangSpecFile = ""; m_clsLangSpec.ResetContents(); 
	m_szKeywordsFile = ""; m_clsKeywords.ResetContents();
	m_bAutomaticSyntaxType = TRUE;

	if( DetectSyntaxType(GetPathName(), m_clsAnalyzedText.GetHead()) ) LoadSyntaxInformation();
	AnalyzeText();

	FormatScreenText();
	UpdateAllViews(NULL);
}

void CCedtDoc::OnDocumentSyntaxText() 
{
	m_szLangSpecFile = ""; m_clsLangSpec.ResetContents(); 
	m_szKeywordsFile = ""; m_clsKeywords.ResetContents();
	m_bAutomaticSyntaxType = FALSE;

//	LoadSyntaxInformation();
	AnalyzeText();

	FormatScreenText();
	UpdateAllViews(NULL);
}

void CCedtDoc::OnDocumentSyntaxType(INT nSyntaxType)
{
	CSyntaxType & rSyntaxType = m_clsSyntaxTypes[nSyntaxType];
	if( rSyntaxType.m_szDescription.GetLength() && rSyntaxType.m_szLangSpecFile.GetLength() && rSyntaxType.m_szKeywordsFile.GetLength() ) {
		m_szLangSpecFile = rSyntaxType.m_szLangSpecFile; m_clsLangSpec.ResetContents(); 
		m_szKeywordsFile = rSyntaxType.m_szKeywordsFile; m_clsKeywords.ResetContents();
		m_bAutomaticSyntaxType = FALSE;

		LoadSyntaxInformation();
		AnalyzeText();

		FormatScreenText();
		UpdateAllViews(NULL);
	}
}

// ON UPDATE DOCUMENT FILE FORMAT
void CCedtDoc::OnUpdateDocumentFormatDos(CCmdUI* pCmdUI) { pCmdUI->SetRadio( m_nFileFormat == FILE_FORMAT_DOS ); }
void CCedtDoc::OnUpdateDocumentFormatUnix(CCmdUI* pCmdUI) { pCmdUI->SetRadio( m_nFileFormat == FILE_FORMAT_UNIX ); }
void CCedtDoc::OnUpdateDocumentFormatMac(CCmdUI* pCmdUI) { pCmdUI->SetRadio( m_nFileFormat == FILE_FORMAT_MAC ); }

// ON DOCUMENT FILE FORMAT
void CCedtDoc::OnDocumentFormatDos() { OnDocumentFileFormat(FILE_FORMAT_DOS); }
void CCedtDoc::OnDocumentFormatUnix() { OnDocumentFileFormat(FILE_FORMAT_UNIX); }
void CCedtDoc::OnDocumentFormatMac() { OnDocumentFileFormat(FILE_FORMAT_MAC); }

void CCedtDoc::OnDocumentFileFormat(UINT nFileFormat)
{
	if( m_nFileFormat == nFileFormat ) return;
	m_nFileFormat = nFileFormat; 

	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	pMainFrame->SetFileStatusInfo( m_nEncodingType, m_nFileFormat, IsReadOnlyFile() );
}

// ON UPDATE DOCUMENT ENCODING TYPE
void CCedtDoc::OnUpdateDocumentEncodingAscii(CCmdUI* pCmdUI) { pCmdUI->SetRadio( m_nEncodingType == ENCODING_TYPE_ASCII ); }
void CCedtDoc::OnUpdateDocumentEncodingUnicodeLE(CCmdUI* pCmdUI) { pCmdUI->SetRadio( m_nEncodingType == ENCODING_TYPE_UNICODE_LE ); }
void CCedtDoc::OnUpdateDocumentEncodingUnicodeBE(CCmdUI* pCmdUI) { pCmdUI->SetRadio( m_nEncodingType == ENCODING_TYPE_UNICODE_BE ); }
void CCedtDoc::OnUpdateDocumentEncodingUtf8WBOM(CCmdUI* pCmdUI) { pCmdUI->SetRadio( m_nEncodingType == ENCODING_TYPE_UTF8_WBOM ); }
void CCedtDoc::OnUpdateDocumentEncodingUtf8XBOM(CCmdUI* pCmdUI) { pCmdUI->SetRadio( m_nEncodingType == ENCODING_TYPE_UTF8_XBOM ); }

void CCedtDoc::OnDocumentEncodingAscii() { OnDocumentEncodingType(ENCODING_TYPE_ASCII); }
void CCedtDoc::OnDocumentEncodingUnicodeLE() { OnDocumentEncodingType(ENCODING_TYPE_UNICODE_LE); }
void CCedtDoc::OnDocumentEncodingUnicodeBE() { OnDocumentEncodingType(ENCODING_TYPE_UNICODE_BE); }
void CCedtDoc::OnDocumentEncodingUtf8WBOM() { OnDocumentEncodingType(ENCODING_TYPE_UTF8_WBOM); }
void CCedtDoc::OnDocumentEncodingUtf8XBOM() { OnDocumentEncodingType(ENCODING_TYPE_UTF8_XBOM); }

void CCedtDoc::OnDocumentEncodingType(UINT nEncodingType)
{
	if( m_nEncodingType == nEncodingType ) return;

	BOOL bFileReload = FALSE;

	if( ! IsNewFileNotSaved() ) { // if it is a disk file then ask user to reload this document
		INT nResult = AfxMessageBox(IDS_MSG_ASK_RELOAD_DOCUMENT, MB_YESNOCANCEL | MB_ICONQUESTION);
		if( nResult == IDCANCEL ) return;
		if( nResult == IDYES ) bFileReload = TRUE;
	}

	m_nEncodingType = nEncodingType;

	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	pMainFrame->SetFileStatusInfo( m_nEncodingType, m_nFileFormat, IsReadOnlyFile() );

	if( bFileReload ) pMainFrame->PostMessage(WM_COMMAND, ID_FILE_RELOAD, 0L);
}


// ON DOCUMENT SUMMARY & PROPERTIES
void CCedtDoc::OnDocumentSummary() 
{
	CDocumentSummary dlg;

	dlg.m_szPathName = GetTitle();

	dlg.m_szEncodingType = ENCODING_TYPE_DESCRIPTION_SHORT[m_nEncodingType];
	dlg.m_szFileFormat = FILE_FORMAT_DESCRIPTION_SHORT[m_nFileFormat];
	dlg.m_nFileSize = m_clsFileStatus.m_size;

	dlg.m_nLineCount = GetLineCount();
	dlg.m_nWordCount = GetWordCount();
	dlg.m_nByteCount = GetByteCount();

	dlg.m_bAttrReadOnly = m_dwFileAttribute & FILE_ATTRIBUTE_READONLY;
	dlg.m_bAttrHidden = m_dwFileAttribute & FILE_ATTRIBUTE_HIDDEN;
	dlg.m_bAttrSystem = m_dwFileAttribute & FILE_ATTRIBUTE_SYSTEM;

	dlg.m_szModDate = m_clsFileStatus.m_mtime.Format("%B %d, %Y   %I:%M:%S %p");

	dlg.DoModal();
}

void CCedtDoc::OnDocumentProperties() 
{
	if( IsRemoteFile() || IsNewFileNotSaved() ) return;
	CString szPath = GetPathName();

	SHELLEXECUTEINFO sei; ZeroMemory( & sei, sizeof(sei) );
	sei.cbSize = sizeof(sei);
	sei.lpFile = szPath;
	sei.lpVerb = _T("properties");
	sei.fMask  = SEE_MASK_INVOKEIDLIST;
	sei.nShow  = SW_SHOWNORMAL;

	ShellExecuteEx( & sei ); 
}

void CCedtDoc::OnUpdateDocumentSummary(CCmdUI* pCmdUI) 
{
}

void CCedtDoc::OnUpdateDocumentProperties(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( ! IsRemoteFile() && ! IsNewFileNotSaved() );
}

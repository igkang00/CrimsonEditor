#include "stdafx.h"
#include "cedtHeader.h"
#include "cedtColors.h"
#include "FtpSettingsDialog.h"
#include "PrefDialog.h"
#include "AboutDialog.h"



void CCedtApp::OnUpdateFileMruFile1(CCmdUI* pCmdUI) 
{
	if( pCmdUI->m_pSubMenu != NULL ) return;
	CWinApp::OnUpdateRecentFileMenu( pCmdUI );
}

void CCedtApp::OnFileFtpSettings() 
{
	CFtpSettingsDialog dlg;
	dlg.SetFtpAccounts( MAX_FTP_ACCOUNT, m_clsFtpAccounts );

	if( dlg.DoModal() == IDOK ) {
		dlg.GetFtpAccounts( MAX_FTP_ACCOUNT, m_clsFtpAccounts );
		SaveFtpAccountInfo(m_szAppDataDirectory + "\\cedt.ftp");
	}
}

void CCedtApp::OnViewColumnMarkers() 
{
	CPreferenceDialog dlg;
	dlg.DoModal(PREF_CATEGORY_VISUAL);
}

void CCedtApp::OnViewSetScreenFonts() 
{
	CPreferenceDialog dlg;
	dlg.DoModal(PREF_CATEGORY_FONTS, 0);
}

void CCedtApp::OnViewSetPrinterFonts() 
{
	CPreferenceDialog dlg;
	dlg.DoModal(PREF_CATEGORY_FONTS, 1);
}

void CCedtApp::OnViewSetLineSpacing() 
{
	CPreferenceDialog dlg;
	dlg.DoModal(PREF_CATEGORY_VISUAL);
}

void CCedtApp::OnViewSetTabSize() 
{
	CPreferenceDialog dlg;
	dlg.DoModal(PREF_CATEGORY_GENERAL);
}

void CCedtApp::OnViewSetColors() 
{
	CPreferenceDialog dlg;
	dlg.DoModal(PREF_CATEGORY_COLORS);
}


void CCedtApp::OnViewLineNumbers() 
{
	CCedtView::m_bShowLineNumbers = ! CCedtView::m_bShowLineNumbers;
	UpdateAllViews();
	
	SaveUserConfiguration(m_szAppDataDirectory + "\\cedt.conf");
}

void CCedtApp::OnUpdateViewLineNumbers(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( CCedtView::m_bShowLineNumbers );
}

void CCedtApp::OnViewLineSpacing100() { OnViewLineSpacing(100); }
void CCedtApp::OnViewLineSpacing110() { OnViewLineSpacing(110); }
void CCedtApp::OnViewLineSpacing120() { OnViewLineSpacing(120); }
void CCedtApp::OnViewLineSpacing150() { OnViewLineSpacing(150); }
void CCedtApp::OnViewLineSpacing200() { OnViewLineSpacing(200); }

void CCedtApp::OnViewLineSpacing(INT nSpacing)
{
	SaveCaretAndAnchorPosAllViews();

	CCedtView::m_nLineSpacing = nSpacing;

	RestoreCaretAndAnchorPosAllViews();
	UpdateAllViews();
	
	SaveUserConfiguration(m_szAppDataDirectory + "\\cedt.conf");
}

void CCedtApp::OnUpdateViewLineSpacing100(CCmdUI* pCmdUI) { OnUpdateViewLineSpacing(100, pCmdUI); }
void CCedtApp::OnUpdateViewLineSpacing110(CCmdUI* pCmdUI) { OnUpdateViewLineSpacing(110, pCmdUI); }
void CCedtApp::OnUpdateViewLineSpacing120(CCmdUI* pCmdUI) { OnUpdateViewLineSpacing(120, pCmdUI); }
void CCedtApp::OnUpdateViewLineSpacing150(CCmdUI* pCmdUI) { OnUpdateViewLineSpacing(150, pCmdUI); }
void CCedtApp::OnUpdateViewLineSpacing200(CCmdUI* pCmdUI) { OnUpdateViewLineSpacing(200, pCmdUI); }

void CCedtApp::OnUpdateViewLineSpacing(INT nSpacing, CCmdUI * pCmdUI)
{
	pCmdUI->SetRadio( CCedtView::m_nLineSpacing == nSpacing );
}

void CCedtApp::OnViewScreenFont0() { OnViewScreenFont(0); }
void CCedtApp::OnViewScreenFont1() { OnViewScreenFont(1); }
void CCedtApp::OnViewScreenFont2() { OnViewScreenFont(2); }
void CCedtApp::OnViewScreenFont3() { OnViewScreenFont(3); }
void CCedtApp::OnViewScreenFont4() { OnViewScreenFont(4); }
void CCedtApp::OnViewScreenFont5() { OnViewScreenFont(5); }

void CCedtApp::OnViewScreenFont(INT nFont)
{
	CCedtView::m_nCurrentScreenFont = nFont;
	if( CCedtView::m_bColumnMode ) { // check if it need to use column mode font
		if( CCedtView::IsFixedPitchScreenFont(nFont) ) CCedtView::m_bUsingColumnModeFont = FALSE;
		else CCedtView::m_bUsingColumnModeFont = TRUE;
	} else CCedtView::m_bUsingColumnModeFont = FALSE;

	// apply preferences to al views
	SaveCaretAndAnchorPosAllViews();

	CCedtView::CreateScreenFontObject();
	CCedtView::ApplyCurrentScreenFont();
	FormatScreenTextAllViews();

	RestoreCaretAndAnchorPosAllViews();
	UpdateAllViews();
	
	SaveUserConfiguration(m_szAppDataDirectory + "\\cedt.conf");
}

void CCedtApp::OnUpdateViewScreenFont0(CCmdUI* pCmdUI) { OnUpdateViewScreenFont(0, pCmdUI); }
void CCedtApp::OnUpdateViewScreenFont1(CCmdUI* pCmdUI) { OnUpdateViewScreenFont(1, pCmdUI); }
void CCedtApp::OnUpdateViewScreenFont2(CCmdUI* pCmdUI) { OnUpdateViewScreenFont(2, pCmdUI); }
void CCedtApp::OnUpdateViewScreenFont3(CCmdUI* pCmdUI) { OnUpdateViewScreenFont(3, pCmdUI); }
void CCedtApp::OnUpdateViewScreenFont4(CCmdUI* pCmdUI) { OnUpdateViewScreenFont(4, pCmdUI); }
void CCedtApp::OnUpdateViewScreenFont5(CCmdUI* pCmdUI) { OnUpdateViewScreenFont(5, pCmdUI); }

void CCedtApp::OnUpdateViewScreenFont(INT nFont, CCmdUI * pCmdUI)
{
	if( pCmdUI->m_pMenu ) {
		CString szMenuText; LOGFONT lf; memcpy( & lf, & CCedtView::m_lfScreen[nFont], sizeof(LOGFONT) );
		if( nFont == 0 ) szMenuText.Format(IDS_MENU_VIEW_FONT_DEFAULT, lf.lfFaceName, lf.lfHeight / 10);
		else szMenuText.Format(IDS_MENU_VIEW_FONT_CUSTOM, nFont, lf.lfFaceName, lf.lfHeight / 10);
		pCmdUI->SetText(szMenuText);
	}
	pCmdUI->SetRadio( CCedtView::m_nCurrentScreenFont == nFont );
}

void CCedtApp::OnViewPrinterFont0() { OnViewPrinterFont(0); }
void CCedtApp::OnViewPrinterFont1() { OnViewPrinterFont(1); }
void CCedtApp::OnViewPrinterFont2() { OnViewPrinterFont(2); }
void CCedtApp::OnViewPrinterFont3() { OnViewPrinterFont(3); }

void CCedtApp::OnViewPrinterFont(INT nFont) 
{
	CCedtView::m_nCurrentPrinterFont = nFont;
	UpdateAllViews();
	
	SaveUserConfiguration(m_szAppDataDirectory + "\\cedt.conf");
}

void CCedtApp::OnUpdateViewPrinterFont0(CCmdUI* pCmdUI) { OnUpdateViewPrinterFont(0, pCmdUI); }
void CCedtApp::OnUpdateViewPrinterFont1(CCmdUI* pCmdUI) { OnUpdateViewPrinterFont(1, pCmdUI); }
void CCedtApp::OnUpdateViewPrinterFont2(CCmdUI* pCmdUI) { OnUpdateViewPrinterFont(2, pCmdUI); }
void CCedtApp::OnUpdateViewPrinterFont3(CCmdUI* pCmdUI) { OnUpdateViewPrinterFont(3, pCmdUI); }

void CCedtApp::OnUpdateViewPrinterFont(INT nFont, CCmdUI* pCmdUI) 
{
	if( pCmdUI->m_pMenu ) {
		CString szMenuText; LOGFONT lf; memcpy( & lf, & CCedtView::m_lfPrinter[nFont], sizeof(LOGFONT) );
		if( nFont == 0 ) szMenuText.Format(IDS_MENU_VIEW_FONT_DEFAULT, lf.lfFaceName, lf.lfHeight / 10);
		else szMenuText.Format(IDS_MENU_VIEW_FONT_CUSTOM, nFont, lf.lfFaceName, lf.lfHeight / 10);
		pCmdUI->SetText(szMenuText);
	}
	pCmdUI->SetRadio( CCedtView::m_nCurrentPrinterFont == nFont );
}


void CCedtApp::OnViewTabSize02() { OnViewTabSize( 2); }
void CCedtApp::OnViewTabSize04() { OnViewTabSize( 4); }
void CCedtApp::OnViewTabSize08() { OnViewTabSize( 8); }
void CCedtApp::OnViewTabSize16() { OnViewTabSize(16); }

void CCedtApp::OnViewTabSize(INT nTabSize)
{
	SaveCaretAndAnchorPosAllViews();

	CCedtView::m_nTabSize = nTabSize;
	FormatScreenTextAllViews();

	RestoreCaretAndAnchorPosAllViews();
	UpdateAllViews();
	
	SaveUserConfiguration(m_szAppDataDirectory + "\\cedt.conf");
}

void CCedtApp::OnUpdateViewTabSize02(CCmdUI* pCmdUI) { OnUpdateViewTabSize( 2, pCmdUI); }
void CCedtApp::OnUpdateViewTabSize04(CCmdUI* pCmdUI) { OnUpdateViewTabSize( 4, pCmdUI); }
void CCedtApp::OnUpdateViewTabSize08(CCmdUI* pCmdUI) { OnUpdateViewTabSize( 8, pCmdUI); }
void CCedtApp::OnUpdateViewTabSize16(CCmdUI* pCmdUI) { OnUpdateViewTabSize(16, pCmdUI); }

void CCedtApp::OnUpdateViewTabSize(INT nTabSize, CCmdUI * pCmdUI)
{
	pCmdUI->SetRadio( CCedtView::m_nTabSize == nTabSize );
}


void CCedtApp::OnViewColorSchemeDefault() { OnViewColorScheme(COLOR_SCHEME_DEFAULT); }
void CCedtApp::OnViewColorSchemeSimplified() { OnViewColorScheme(COLOR_SCHEME_SIMPLIFIED); }
void CCedtApp::OnViewColorSchemeReversed() { OnViewColorScheme(COLOR_SCHEME_REVERSED); }
void CCedtApp::OnViewColorSchemeLightgray() { OnViewColorScheme(COLOR_SCHEME_LIGHTGRAY); }
void CCedtApp::OnViewColorSchemeDarkblue() { OnViewColorScheme(COLOR_SCHEME_DARKBLUE); }

void CCedtApp::OnViewColorScheme(INT nScheme)
{
	SetPredefinedColorScheme(nScheme);
	UpdateAllViews();
	SaveColorScheme(m_szAppDataDirectory + "\\cedt.color");
}

void CCedtApp::OnViewColorSchemeSaved() 
{
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
	UpdateAllViews();
	SaveColorScheme(m_szAppDataDirectory + "\\cedt.color");
}

void CCedtApp::OnViewEmboldenKeywords() 
{
	CCedtView::m_bEmboldenKeywords = ! CCedtView::m_bEmboldenKeywords;
	UpdateAllViews();
	SaveUserConfiguration(m_szAppDataDirectory + "\\cedt.conf");
}

void CCedtApp::OnViewItalicizeComment() 
{
	CCedtView::m_bItalicizeComment = ! CCedtView::m_bItalicizeComment;
	UpdateAllViews();
	SaveUserConfiguration(m_szAppDataDirectory + "\\cedt.conf");
}

void CCedtApp::OnViewShowSpaces() 
{
	CCedtView::m_bShowSpaces = ! CCedtView::m_bShowSpaces;
	UpdateAllViews();
	SaveUserConfiguration(m_szAppDataDirectory + "\\cedt.conf");
}

void CCedtApp::OnViewShowTabChars() 
{
	CCedtView::m_bShowTabChars = ! CCedtView::m_bShowTabChars;
	UpdateAllViews();
	SaveUserConfiguration(m_szAppDataDirectory + "\\cedt.conf");
}

void CCedtApp::OnViewShowLineBreak() 
{
	CCedtView::m_bShowLineBreak = ! CCedtView::m_bShowLineBreak;
	UpdateAllViews();
	SaveUserConfiguration(m_szAppDataDirectory + "\\cedt.conf");
}

void CCedtApp::OnViewShowAllWhite() 
{
	if( CCedtView::m_bShowSpaces && CCedtView::m_bShowTabChars && CCedtView::m_bShowLineBreak ) {
		CCedtView::m_bShowSpaces = FALSE;
		CCedtView::m_bShowTabChars = FALSE;
		CCedtView::m_bShowLineBreak = FALSE;
	} else {
		CCedtView::m_bShowSpaces = TRUE;
		CCedtView::m_bShowTabChars = TRUE;
		CCedtView::m_bShowLineBreak = TRUE;
	}

	UpdateAllViews();
	SaveUserConfiguration(m_szAppDataDirectory + "\\cedt.conf");
}

void CCedtApp::OnUpdateViewEmboldenKeywords(CCmdUI* pCmdUI) { pCmdUI->SetCheck( CCedtView::m_bEmboldenKeywords ); }
void CCedtApp::OnUpdateViewItalicizeComment(CCmdUI* pCmdUI) { pCmdUI->SetCheck( CCedtView::m_bItalicizeComment ); }
void CCedtApp::OnUpdateViewShowSpaces(CCmdUI* pCmdUI) { pCmdUI->SetCheck( CCedtView::m_bShowSpaces ); }
void CCedtApp::OnUpdateViewShowTabChars(CCmdUI* pCmdUI) { pCmdUI->SetCheck( CCedtView::m_bShowTabChars ); }
void CCedtApp::OnUpdateViewShowLineBreak(CCmdUI* pCmdUI) { pCmdUI->SetCheck( CCedtView::m_bShowLineBreak ); }
void CCedtApp::OnUpdateViewShowAllWhite(CCmdUI* pCmdUI) { pCmdUI->SetCheck( CCedtView::m_bShowSpaces && CCedtView::m_bShowTabChars && CCedtView::m_bShowLineBreak ); }


void CCedtApp::OnDocuSyntaxCustomize() 
{
	CPreferenceDialog dlg;
	dlg.DoModal(PREF_CATEGORY_SYNTAX);
}

void CCedtApp::OnToolPreferences() 
{
	CPreferenceDialog dlg;
	dlg.DoModal();
}

void CCedtApp::OnUpdateCommandUserFile0(CCmdUI* pCmdUI) 
{
	CCedtView::RefreshUserCommandFilePathForMenu();
	CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();
	pFrame->RefreshLoadUserToolsMenu();
	pCmdUI->Enable( CCedtView::m_szUserCommandFilePath[0].GetLength() );
}

void CCedtApp::OnCommandUserFile0() { LoadUserCommands(CCedtView::m_szUserCommandFilePath[0]); SaveUserCommands(m_szAppDataDirectory + "\\cedt.tools"); }
void CCedtApp::OnCommandUserFile1() { LoadUserCommands(CCedtView::m_szUserCommandFilePath[1]); SaveUserCommands(m_szAppDataDirectory + "\\cedt.tools"); }
void CCedtApp::OnCommandUserFile2() { LoadUserCommands(CCedtView::m_szUserCommandFilePath[2]); SaveUserCommands(m_szAppDataDirectory + "\\cedt.tools"); }
void CCedtApp::OnCommandUserFile3() { LoadUserCommands(CCedtView::m_szUserCommandFilePath[3]); SaveUserCommands(m_szAppDataDirectory + "\\cedt.tools"); }
void CCedtApp::OnCommandUserFile4() { LoadUserCommands(CCedtView::m_szUserCommandFilePath[4]); SaveUserCommands(m_szAppDataDirectory + "\\cedt.tools"); }
void CCedtApp::OnCommandUserFile5() { LoadUserCommands(CCedtView::m_szUserCommandFilePath[5]); SaveUserCommands(m_szAppDataDirectory + "\\cedt.tools"); }
void CCedtApp::OnCommandUserFile6() { LoadUserCommands(CCedtView::m_szUserCommandFilePath[6]); SaveUserCommands(m_szAppDataDirectory + "\\cedt.tools"); }
void CCedtApp::OnCommandUserFile7() { LoadUserCommands(CCedtView::m_szUserCommandFilePath[7]); SaveUserCommands(m_szAppDataDirectory + "\\cedt.tools"); }

void CCedtApp::OnCommandConfigure() 
{
	CPreferenceDialog dlg;
	dlg.DoModal(PREF_CATEGORY_COMMANDS);
}

void CCedtApp::OnUpdateMacroUserFile0(CCmdUI* pCmdUI) 
{
	CCedtView::RefreshMacroBufferFilePathForMenu();
	CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();
	pFrame->RefreshLoadUserMacrosMenu();
	pCmdUI->Enable( CCedtView::m_szMacroBufferFilePath[0].GetLength() );
}

void CCedtApp::OnMacroUserFile0() { LoadMacroBuffers(CCedtView::m_szMacroBufferFilePath[0]); SaveMacroBuffers(m_szAppDataDirectory + "\\cedt.macro"); }
void CCedtApp::OnMacroUserFile1() { LoadMacroBuffers(CCedtView::m_szMacroBufferFilePath[1]); SaveMacroBuffers(m_szAppDataDirectory + "\\cedt.macro"); }
void CCedtApp::OnMacroUserFile2() { LoadMacroBuffers(CCedtView::m_szMacroBufferFilePath[2]); SaveMacroBuffers(m_szAppDataDirectory + "\\cedt.macro"); }
void CCedtApp::OnMacroUserFile3() { LoadMacroBuffers(CCedtView::m_szMacroBufferFilePath[3]); SaveMacroBuffers(m_szAppDataDirectory + "\\cedt.macro"); }
void CCedtApp::OnMacroUserFile4() { LoadMacroBuffers(CCedtView::m_szMacroBufferFilePath[4]); SaveMacroBuffers(m_szAppDataDirectory + "\\cedt.macro"); }
void CCedtApp::OnMacroUserFile5() { LoadMacroBuffers(CCedtView::m_szMacroBufferFilePath[5]); SaveMacroBuffers(m_szAppDataDirectory + "\\cedt.macro"); }
void CCedtApp::OnMacroUserFile6() { LoadMacroBuffers(CCedtView::m_szMacroBufferFilePath[6]); SaveMacroBuffers(m_szAppDataDirectory + "\\cedt.macro"); }
void CCedtApp::OnMacroUserFile7() { LoadMacroBuffers(CCedtView::m_szMacroBufferFilePath[7]); SaveMacroBuffers(m_szAppDataDirectory + "\\cedt.macro"); }

void CCedtApp::OnMacroConfigure() 
{
	CPreferenceDialog dlg;
	dlg.DoModal(PREF_CATEGORY_MACROS);
}

void CCedtApp::OnHelpTopics() 
{
	GotoURL(m_szInstallDirectory + "\\docs\\index.html", SW_SHOW);
}

void CCedtApp::OnHelpVisitHomepage() 
{
	GotoURL(STRING_HOMEPAGEURL, SW_SHOW);
}

void CCedtApp::OnHelpFeedback() 
{
	GotoURL(STRING_EMAILADDRESS, SW_SHOW);
}

void CCedtApp::OnHelpSponsoring() 
{
	GotoURL(STRING_SPONSORURL, SW_SHOW);
}

void CCedtApp::OnAppAbout() 
{
	CAboutDialog dlg;
	dlg.DoModal();
}

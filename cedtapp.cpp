// cedt.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <afxmt.h>
#include "cedtHeader.h"
#include "cedtColors.h"
#include "registry.h"
#include "HtmlHelp.h"
#include "FolderDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// Shared data segment
#pragma data_seg("Shared")
DWORD	nFirstThreadID = 0;
HWND	hFirstWindow = NULL;
#pragma data_seg()
#pragma comment(linker, "/section:Shared,rws")


/////////////////////////////////////////////////////////////////////////////
// Global variables
UINT g_uClipbrdFormatProjectItem = 0U;
UINT g_uClipbrdFormatFileTabItem = 0U;
BOOL g_bDoubleByteCharacterSet = FALSE;


/////////////////////////////////////////////////////////////////////////////
// CCedtApp static member variables
CString CCedtApp::m_szLoadingDirectory;
CString CCedtApp::m_szInstallDirectory;
CString CCedtApp::m_szAppDataDirectory;

CString CCedtApp::m_szInitialWorkingDirectory;
CString CCedtApp::m_szRemoteBackupDirectory;

BOOL CCedtApp::m_bAllowMultiInstances = FALSE;
BOOL CCedtApp::m_bFirstInstance = TRUE;

BOOL CCedtApp::m_bCreateNewDocumentOnStartup = TRUE;
BOOL CCedtApp::m_bReloadWorkingFilesOnStartup = TRUE;
BOOL CCedtApp::m_bCheckIfFilesModifiedOutside = TRUE;
BOOL CCedtApp::m_bCloseTabUsingDoubleClick = TRUE;

// global cursor resources
HCURSOR CCedtApp::m_hCursorArrow, CCedtApp::m_hCursorIBeam, CCedtApp::m_hCursorCross; 
HCURSOR CCedtApp::m_hCursorRightArrow, CCedtApp::m_hCursorArrowMacro, CCedtApp::m_hCursorIBeamMacro;

// ftp accounts and file filters
CFtpAccount CCedtApp::m_clsFtpAccounts[MAX_FTP_ACCOUNT];
CFileFilter CCedtApp::m_clsFileFilters[MAX_FILE_FILTER];
INT CCedtApp::m_nFilterIndexDialog = 0;
INT CCedtApp::m_nFilterIndexPannel = 0;

// html help initialization
BOOL CCedtApp::m_bHtmlHelpInitialized = FALSE;
DWORD CCedtApp::m_dwHtmlHelpCookie = 0x00;

// open template
CString CCedtApp::m_szOpenTemplatePathName;
BOOL CCedtApp::m_bOpenTemplate;

// open memtext
CMemText CCedtApp::m_clsMemText;
BOOL CCedtApp::m_bOpenMemText;

/////////////////////////////////////////////////////////////////////////////
// CCedtApp

BEGIN_MESSAGE_MAP(CCedtApp, CWinApp)
	//{{AFX_MSG_MAP(CCedtApp)
	ON_UPDATE_COMMAND_UI(ID_FILE_MRU_FILE1, OnUpdateFileMruFile1)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_HELP_TOPICS, OnHelpTopics)
	ON_COMMAND(ID_HELP_VISIT_HOMEPAGE, OnHelpVisitHomepage)
	ON_COMMAND(ID_HELP_FEEDBACK, OnHelpFeedback)
	ON_COMMAND(ID_HELP_SPONSORING, OnHelpSponsoring)
	ON_COMMAND(ID_TOOL_PREFERENCES, OnToolPreferences)
	ON_COMMAND(ID_MACRO_CONFIGURE, OnMacroConfigure)
	ON_COMMAND(ID_COMMAND_CONFIGURE, OnCommandConfigure)
	ON_COMMAND(ID_VIEW_SET_COLORS, OnViewSetColors)
	ON_COMMAND(ID_FILE_FTP_SETTINGS, OnFileFtpSettings)
	ON_COMMAND(ID_FILE_OPEN_REMOTE, OnFileOpenRemote)
	ON_COMMAND(ID_FILE_OPEN_TEMPLATE, OnFileOpenTemplate)
	ON_COMMAND(ID_VIEW_SET_LINE_SPACING, OnViewSetLineSpacing)
	ON_COMMAND(ID_VIEW_SET_TAB_SIZE, OnViewSetTabSize)
	ON_COMMAND(ID_FILE_CLOSE_ALL, OnFileCloseAll)
	ON_COMMAND(ID_FILE_CHECK_MODIFIED, OnFileCheckModified)
	ON_COMMAND(ID_DOCU_SYNTAX_CUSTOMIZE, OnDocuSyntaxCustomize)
	ON_COMMAND(ID_VIEW_COLUMN_MARKERS, OnViewColumnMarkers)
	ON_COMMAND(ID_SRCH_FIND_IN_FILES, OnSearchFindInFiles)
	ON_COMMAND(ID_PRJ_NEW, OnProjectNew)
	ON_COMMAND(ID_PRJ_OPEN, OnProjectOpen)
	ON_COMMAND(ID_PRJ_CLOSE, OnProjectClose)
	ON_COMMAND(ID_PRJ_NEW_CATEGORY, OnProjectNewCategory)
	ON_COMMAND(ID_PRJ_ADD_FILES_TO, OnProjectAddFilesTo)
	ON_COMMAND(ID_PRJ_ADD_ACTIVE_FILE, OnProjectAddActiveFile)
	ON_COMMAND(ID_PRJ_ADD_OPEN_FILES, OnProjectAddOpenFiles)
	ON_UPDATE_COMMAND_UI(ID_PRJ_CLOSE, OnUpdateProjectClose)
	ON_UPDATE_COMMAND_UI(ID_PRJ_NEW_CATEGORY, OnUpdateProjectNewCategory)
	ON_UPDATE_COMMAND_UI(ID_PRJ_ADD_FILES_TO, OnUpdateProjectAddFilesTo)
	ON_UPDATE_COMMAND_UI(ID_PRJ_ADD_ACTIVE_FILE, OnUpdateProjectAddActiveFile)
	ON_UPDATE_COMMAND_UI(ID_PRJ_ADD_OPEN_FILES, OnUpdateProjectAddOpenFiles)
	ON_COMMAND(ID_VIEW_SET_SCREEN_FONTS, OnViewSetScreenFonts)
	ON_COMMAND(ID_VIEW_SET_PRINTER_FONTS, OnViewSetPrinterFonts)
	ON_UPDATE_COMMAND_UI(ID_COMMAND_USER_FILE0, OnUpdateCommandUserFile0)
	ON_UPDATE_COMMAND_UI(ID_MACRO_USER_FILE0, OnUpdateMacroUserFile0)
	ON_COMMAND(ID_COMMAND_USER_FILE0, OnCommandUserFile0)
	ON_COMMAND(ID_COMMAND_USER_FILE1, OnCommandUserFile1)
	ON_COMMAND(ID_COMMAND_USER_FILE2, OnCommandUserFile2)
	ON_COMMAND(ID_COMMAND_USER_FILE3, OnCommandUserFile3)
	ON_COMMAND(ID_COMMAND_USER_FILE4, OnCommandUserFile4)
	ON_COMMAND(ID_COMMAND_USER_FILE5, OnCommandUserFile5)
	ON_COMMAND(ID_COMMAND_USER_FILE6, OnCommandUserFile6)
	ON_COMMAND(ID_COMMAND_USER_FILE7, OnCommandUserFile7)
	ON_COMMAND(ID_MACRO_USER_FILE0, OnMacroUserFile0)
	ON_COMMAND(ID_MACRO_USER_FILE1, OnMacroUserFile1)
	ON_COMMAND(ID_MACRO_USER_FILE2, OnMacroUserFile2)
	ON_COMMAND(ID_MACRO_USER_FILE3, OnMacroUserFile3)
	ON_COMMAND(ID_MACRO_USER_FILE4, OnMacroUserFile4)
	ON_COMMAND(ID_MACRO_USER_FILE5, OnMacroUserFile5)
	ON_COMMAND(ID_MACRO_USER_FILE6, OnMacroUserFile6)
	ON_COMMAND(ID_MACRO_USER_FILE7, OnMacroUserFile7)
	ON_COMMAND(ID_EDIT_COLUMN_MODE, OnEditColumnMode)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COLUMN_MODE, OnUpdateEditColumnMode)
	ON_COMMAND(ID_INDICATOR_OVR, OnIndicatorOvr)
	ON_COMMAND(ID_VIEW_LINE_NUMBERS, OnViewLineNumbers)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LINE_NUMBERS, OnUpdateViewLineNumbers)
	ON_COMMAND(ID_VIEW_LINE_SPACING100, OnViewLineSpacing100)
	ON_COMMAND(ID_VIEW_LINE_SPACING110, OnViewLineSpacing110)
	ON_COMMAND(ID_VIEW_LINE_SPACING120, OnViewLineSpacing120)
	ON_COMMAND(ID_VIEW_LINE_SPACING150, OnViewLineSpacing150)
	ON_COMMAND(ID_VIEW_LINE_SPACING200, OnViewLineSpacing200)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LINE_SPACING100, OnUpdateViewLineSpacing100)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LINE_SPACING110, OnUpdateViewLineSpacing110)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LINE_SPACING120, OnUpdateViewLineSpacing120)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LINE_SPACING150, OnUpdateViewLineSpacing150)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LINE_SPACING200, OnUpdateViewLineSpacing200)
	ON_COMMAND(ID_VIEW_SCREEN_FONT0, OnViewScreenFont0)
	ON_COMMAND(ID_VIEW_SCREEN_FONT1, OnViewScreenFont1)
	ON_COMMAND(ID_VIEW_SCREEN_FONT2, OnViewScreenFont2)
	ON_COMMAND(ID_VIEW_SCREEN_FONT3, OnViewScreenFont3)
	ON_COMMAND(ID_VIEW_SCREEN_FONT4, OnViewScreenFont4)
	ON_COMMAND(ID_VIEW_SCREEN_FONT5, OnViewScreenFont5)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SCREEN_FONT0, OnUpdateViewScreenFont0)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SCREEN_FONT1, OnUpdateViewScreenFont1)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SCREEN_FONT2, OnUpdateViewScreenFont2)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SCREEN_FONT3, OnUpdateViewScreenFont3)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SCREEN_FONT4, OnUpdateViewScreenFont4)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SCREEN_FONT5, OnUpdateViewScreenFont5)
	ON_COMMAND(ID_VIEW_PRINTER_FONT0, OnViewPrinterFont0)
	ON_COMMAND(ID_VIEW_PRINTER_FONT1, OnViewPrinterFont1)
	ON_COMMAND(ID_VIEW_PRINTER_FONT2, OnViewPrinterFont2)
	ON_COMMAND(ID_VIEW_PRINTER_FONT3, OnViewPrinterFont3)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PRINTER_FONT0, OnUpdateViewPrinterFont0)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PRINTER_FONT1, OnUpdateViewPrinterFont1)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PRINTER_FONT2, OnUpdateViewPrinterFont2)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PRINTER_FONT3, OnUpdateViewPrinterFont3)
	ON_COMMAND(ID_VIEW_TAB_SIZE02, OnViewTabSize02)
	ON_COMMAND(ID_VIEW_TAB_SIZE04, OnViewTabSize04)
	ON_COMMAND(ID_VIEW_TAB_SIZE08, OnViewTabSize08)
	ON_COMMAND(ID_VIEW_TAB_SIZE16, OnViewTabSize16)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TAB_SIZE02, OnUpdateViewTabSize02)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TAB_SIZE04, OnUpdateViewTabSize04)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TAB_SIZE08, OnUpdateViewTabSize08)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TAB_SIZE16, OnUpdateViewTabSize16)
	ON_COMMAND(ID_VIEW_COLOR_SCHEME_DEFAULT, OnViewColorSchemeDefault)
	ON_COMMAND(ID_VIEW_COLOR_SCHEME_SIMPLIFIED, OnViewColorSchemeSimplified)
	ON_COMMAND(ID_VIEW_COLOR_SCHEME_REVERSED, OnViewColorSchemeReversed)
	ON_COMMAND(ID_VIEW_COLOR_SCHEME_LIGHTGRAY, OnViewColorSchemeLightgray)
	ON_COMMAND(ID_VIEW_COLOR_SCHEME_DARKBLUE, OnViewColorSchemeDarkblue)
	ON_COMMAND(ID_VIEW_COLOR_SCHEME_SAVED, OnViewColorSchemeSaved)
	ON_COMMAND(ID_VIEW_EMBOLDEN_KEYWORDS, OnViewEmboldenKeywords)
	ON_COMMAND(ID_VIEW_ITALICIZE_COMMENT, OnViewItalicizeComment)
	ON_COMMAND(ID_VIEW_SHOW_SPACES, OnViewShowSpaces)
	ON_COMMAND(ID_VIEW_SHOW_TAB_CHARS, OnViewShowTabChars)
	ON_COMMAND(ID_VIEW_SHOW_LINE_BREAK, OnViewShowLineBreak)
	ON_COMMAND(ID_VIEW_SHOW_ALL_WHITE, OnViewShowAllWhite)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EMBOLDEN_KEYWORDS, OnUpdateViewEmboldenKeywords)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ITALICIZE_COMMENT, OnUpdateViewItalicizeComment)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOW_SPACES, OnUpdateViewShowSpaces)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOW_TAB_CHARS, OnUpdateViewShowTabChars)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOW_LINE_BREAK, OnUpdateViewShowLineBreak)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOW_ALL_WHITE, OnUpdateViewShowAllWhite)
	ON_COMMAND(ID_DIR_ITEM_OPEN, OnDirectoryItemOpen)
	ON_COMMAND(ID_DIR_ITEM_EXECUTE, OnDirectoryItemExecute)
	ON_COMMAND(ID_DIR_ITEM_EXPLORE, OnDirectoryItemExplore)
	ON_COMMAND(ID_DIR_ITEM_FIND, OnDirectoryItemFind)
	ON_COMMAND(ID_DIR_ITEM_NEW_FOLDER, OnDirectoryItemNewFolder)
	ON_COMMAND(ID_DIR_ITEM_NEW_DOCUMENT, OnDirectoryItemNewDocument)
	ON_COMMAND(ID_DIR_ITEM_SETWORKDIR, OnDirectoryItemSetWorkdir)
	ON_COMMAND(ID_DIR_ITEM_MOVE, OnDirectoryItemMove)
	ON_COMMAND(ID_DIR_ITEM_COPY, OnDirectoryItemCopy)
	ON_COMMAND(ID_DIR_ITEM_DELETE, OnDirectoryItemDelete)
	ON_COMMAND(ID_DIR_ITEM_RENAME, OnDirectoryItemRename)
	ON_COMMAND(ID_DIR_ITEM_PROPERTY, OnDirectoryItemProperty)
	ON_COMMAND(ID_DIR_ITEM_REFRESH, OnDirectoryItemRefresh)
	ON_COMMAND(ID_PRJ_ITEM_OPEN, OnProjectItemOpen)
	ON_COMMAND(ID_PRJ_ITEM_EXECUTE, OnProjectItemExecute)
	ON_COMMAND(ID_PRJ_ITEM_REMOVE, OnProjectItemRemove)
	ON_COMMAND(ID_PRJ_ITEM_RENAME, OnProjectItemRename)
	ON_COMMAND(ID_PRJ_ITEM_PROPERTY, OnProjectItemProperty)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCedtApp construction
CCedtApp::CCedtApp()
{
	m_bPostOpenDocument = FALSE;
	m_nPostOpenFtpAccount = -1;
	m_szPostOpenPathName = "";
	m_nPostOpenLineNum = 0;

	m_szPrevWorkspacePathName = "";
	m_bProjectLoaded = FALSE;
	m_szProjectPathName = "";
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCedtApp object
CCedtApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CCedtApp initialization
BOOL CCedtApp::InitInstance()
{
	// Change the registry key under which our settings are stored.
	SetRegistryKey(STRING_COMPANYNAME);

	// get current working directory
	TCHAR szTemp[MAX_PATH]; GetCurrentDirectory(MAX_PATH, szTemp);
	m_szLoadingDirectory = ChopDirectory(szTemp);
	TRACE1("LoadingDirectory: \"%s\"\n", m_szLoadingDirectory);

	// get InstallDir from the registry
	m_szInstallDirectory = GetProfileString(REGKEY_INSTALL_DIRECTORY, "", NULL);
	if( ! m_szInstallDirectory.GetLength() ) {
		if( ! GetRegKeyValue(HKEY_LOCAL_MACHINE, REGPATH_INSTALL_DIRECTORY, "InstallDir", m_szInstallDirectory) ) {
			CString szText; szText.LoadString( IDS_CHOOSE_INSTALL_DIR );
			CString szDirectory = m_szLoadingDirectory;

			CFolderDialog dlg(szText, szDirectory, NULL, NULL);
			if( dlg.DoModal() != IDOK ) return FALSE;
			m_szInstallDirectory = dlg.GetPathName();
		}
		WriteProfileString(REGKEY_INSTALL_DIRECTORY, "", m_szInstallDirectory);
	}
	TRACE1("InstallDirectory: \"%s\"\n", m_szInstallDirectory);

	// get application data directory
	TCHAR szTmp2[MAX_PATH]; m_szAppDataDirectory = m_szInstallDirectory;
	BOOL bResul2 = SHGetSpecialFolderPath(NULL, szTmp2, CSIDL_APPDATA, TRUE);
	if( bResul2 ) m_szAppDataDirectory.Format("%s\\Crimson Editor", szTmp2);
	TRACE1("AppDataDirectory: \"%s\"\n", m_szAppDataDirectory);


	// load multi-instance flag
	m_bAllowMultiInstances = FALSE;
	if( ! LoadMultiInstancesFlag(REGKEY_ALLOW_MULTI_INSTANCES) ) return FALSE;

	// is it first instance ?
	m_bFirstInstance = TRUE; 
	if( nFirstThreadID ) m_bFirstInstance = FALSE;

	// check if it allow multi-instances
	if( ! m_bAllowMultiInstances && ! m_bFirstInstance ) {
		// do not allow multi-instances
		CMutex mutex(FALSE, MUTEX_NAME_CMDLINE);
		CSingleLock lock( & mutex ); lock.Lock();

		BOOL quote = ( strlen(m_lpCmdLine) && VerifyFilePath(m_lpCmdLine) );

		ofstream fout(m_szAppDataDirectory + "\\cmdline.txt", ios::out | ios::app);
		fout << "/D:\"" << m_szLoadingDirectory << "\"";
		fout << (quote ? " \"" : " ") << m_lpCmdLine << (quote ? "\"" : "") << endl;
		fout.close(); 

		lock.Unlock();

		if( hFirstWindow && IsIconic(hFirstWindow) ) ShowWindow(hFirstWindow, SW_RESTORE);
		if( hFirstWindow ) SetForegroundWindow(hFirstWindow);

		::PostThreadMessage(nFirstThreadID, WM_ANOTHER_INSTANCE, 0, 0L);
		return FALSE; // ghost exit here !!!

	} else {
		// otherwise save thread id for later use
		nFirstThreadID = m_nThreadID; 
	}


	// Initialize OLE 2.0 libraries
	if( ! AfxOleInit() ) { AfxMessageBox(IDS_ERR_OLE_INIT_FAILURE); return FALSE; }

	// Initialize common control library for XP Look
	InitCommonControls();

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif


	// register custom clipboard format
	g_uClipbrdFormatProjectItem = RegisterClipboardFormat(CLIPBRD_FORMAT_PROJECT_ITEM);
	g_uClipbrdFormatFileTabItem = RegisterClipboardFormat(CLIPBRD_FORMAT_FILETAB_ITEM);

	// check multi-byte language
	g_bDoubleByteCharacterSet = FALSE;
	if( GetSystemMetrics(SM_DBCSENABLED) ) g_bDoubleByteCharacterSet = TRUE;

	// load user configuration
	if( ! LoadUserConfiguration(m_szAppDataDirectory + "\\cedt.conf") ) {
		if( ! LoadUserConfiguration(m_szInstallDirectory + "\\cedt.conf") ) {
			AfxMessageBox(IDS_ERR_CORRUPT_CONFIG_FILE, MB_OK | MB_ICONEXCLAMATION);
			SetDefaultConfiguration();
		}
		SaveUserConfiguration(m_szAppDataDirectory + "\\cedt.conf");
	}

	// load color settings
	if( ! LoadColorScheme(m_szAppDataDirectory + "\\cedt.color") ) {
		if( ! LoadColorScheme(m_szInstallDirectory + "\\cedt.color") ) 
			SetPredefinedColorScheme(COLOR_SCHEME_DEFAULT);
		SaveColorScheme(m_szAppDataDirectory + "\\cedt.color");
	}

	// load FTP account information
	LoadFtpAccountInfo(m_szAppDataDirectory + "\\cedt.ftp");

	// load command & macro
	LoadUserCommands(m_szAppDataDirectory + "\\cedt.tools");
	LoadMacroBuffers(m_szAppDataDirectory + "\\cedt.macro");

//	SetRegistryKey(STRING_COMPANYNAME); // Change the registry key under which our settings are stored.
	LoadStdProfileSettings(8); // Load standard INI file options (including MRU)


	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_CEDTTYPE,
		RUNTIME_CLASS(CCedtDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CCedtView));
	AddDocTemplate(pDocTemplate);

	// Create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if ( ! pMainFrame->LoadFrame(IDR_MAINFRAME) ) return FALSE;
	m_pMainWnd = pMainFrame;

	// save first instance window handle 
	if( ! hFirstWindow ) hFirstWindow = pMainFrame->m_hWnd;

	// Load cursor resources
	m_hCursorArrow = LoadStandardCursor(IDC_ARROW);
	m_hCursorIBeam = LoadStandardCursor(IDC_IBEAM);
	m_hCursorCross = LoadStandardCursor(IDC_CROSS);
	m_hCursorRightArrow = LoadCursor(IDC_RIGHT_ARROW);
	m_hCursorArrowMacro = LoadCursor(IDC_ARROW_MACRO);
	m_hCursorIBeamMacro = LoadCursor(IDC_IBEAM_MACRO);


	// Accept files from drag and drop
	// Changed it to OLE drag and drop
	// pMainFrame->DragAcceptFiles(TRUE);

	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();

	// File window initialization 1
	pFileWindow->InitLocalDriveList(NULL);
	pFileWindow->InitFileFilterList( GetComposedFileFilter(), RecalcFilterIndexSkipNull(m_nFilterIndexPannel), CallbackFilterIndexPannel );

	// File window browsing directory & current working directory
	if( m_szInitialWorkingDirectory.GetLength() ) {
		pFileWindow->SetBrowsingDirectory( m_szInitialWorkingDirectory );
		// redundant code - SetBrowsingDirectory() will set current working directory
		::SetCurrentDirectory( m_szInitialWorkingDirectory );
	} else {
		LoadBrowsingDirectory(REGKEY_BROWSING_DIRECTORY);
		// current working directory might differ from browsing directory
		LoadWorkingDirectory(REGKEY_WORKING_DIRECTORY);
	}

	// File window initialization 2
	pFileWindow->InitProjectWorkspace();

	// Load prev workspace pathname
	m_szPrevWorkspacePathName = "";
	LoadWorkspaceFilePath(REGKEY_LAST_WORKSPACE);

	// Load window placement & bar state
	pMainFrame->LoadWindowPlacement(REGKEY_WINDOW_PLACEMENT);
	pMainFrame->LoadBarState(REGKEY_BAR_STATE);

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(SW_SHOW);
	pMainFrame->UpdateWindow();


	// Reload last working files
	BOOL bReloadLastWorkingFiles = ( m_bReloadWorkingFilesOnStartup && m_bFirstInstance );
	if( bReloadLastWorkingFiles ) ReloadLastWorkingFiles();

	// Parse command line for standard shell commands, DDE, file open
	if( strlen(m_lpCmdLine) && VerifyFilePath(m_lpCmdLine) ) {
		TRACE1("CmdLine: %s\n", m_lpCmdLine);
		OpenDocumentFile( GetLongPathName(m_lpCmdLine) );
	} else {
		CCmdLine cmdLine(__argc, __argv);
		ProcessShellCommand(cmdLine);
	}

	// Create new document on start up if nothing loaded
	if( m_bCreateNewDocumentOnStartup && ! GetFirstDocPosition() ) OnFileNew();

	return TRUE;
}

int CCedtApp::ExitInstance() 
{
	// User configuration is saved when mainframe is closed and whenever there is a change
	// SaveUserConfiguration(m_szAppDataDirectory + "\\cedt.conf");

	// Uninitialize HtmlHelp if it was initialized
	if( m_bHtmlHelpInitialized ) {
		::HtmlHelp(NULL, NULL, HH_UNINITIALIZE, (DWORD)&m_dwHtmlHelpCookie);
		m_bHtmlHelpInitialized = FALSE;
	}

	return CWinApp::ExitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// CCedtApp operations
void CCedtApp::OnAnotherInstance()
{
	TCHAR szBuffer[4096]; CStringArray arrCmdLine;

	CMutex mutex(FALSE, MUTEX_NAME_CMDLINE);
	CSingleLock lock( & mutex ); lock.Lock();

	ifstream fin(m_szAppDataDirectory + "\\cmdline.txt", ios::in | ios::nocreate);
	while( fin.good() ) {
		fin.getline(szBuffer, 4096);
		if( strlen(szBuffer) ) arrCmdLine.Add(szBuffer);
	}
	fin.close();

	ofstream fout(m_szAppDataDirectory + "\\cmdline.txt", ios::out );
	fout.close();

	lock.Unlock();

	for(INT i = 0; i < arrCmdLine.GetSize(); i++) {
		// Parse command line for standard shell commands
		CCmdLine cmdLine( arrCmdLine[i] );
		ProcessShellCommand(cmdLine);
	}
}

POSITION CCedtApp::GetFirstDocPosition()
{
	POSITION posTemplate = GetFirstDocTemplatePosition();
	CDocTemplate * pTemplate = GetNextDocTemplate( posTemplate );
	return pTemplate->GetFirstDocPosition();
}

CDocument * CCedtApp::GetNextDoc(POSITION & rPos)
{
	POSITION posTemplate = GetFirstDocTemplatePosition();
	CDocTemplate * pTemplate = GetNextDocTemplate( posTemplate );
	return pTemplate->GetNextDoc( rPos );
}

INT CCedtApp::GetDocumentCount()
{
	INT nCount = 0; POSITION posDoc = GetFirstDocPosition();
	while( posDoc ) { 
		CCedtDoc * pDoc = (CCedtDoc *)GetNextDoc( posDoc ); 
		nCount++; 
	}
	return nCount;
}

INT CCedtApp::GetViewCount()
{
	INT nCount = 0; POSITION posDoc = GetFirstDocPosition();
	while( posDoc ) {
		CCedtDoc * pDoc = (CCedtDoc *)GetNextDoc( posDoc ); 
		nCount += pDoc->GetViewCount();
	}
	return nCount;
}

/////////////////////////////////////////////////////////////////////////////
// CCedtApp message handlers
BOOL CCedtApp::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_ANOTHER_INSTANCE ) { 
		OnAnotherInstance(); 
		return TRUE;
	}
	
	return CWinApp::PreTranslateMessage(pMsg);
}


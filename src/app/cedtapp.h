// cedt.h : main header file for the CEDT application
//

#if !defined(AFX_CEDT_H__FFCA2B84_F9C5_11D4_A6F1_0050CE184C9B__INCLUDED_)
#define AFX_CEDT_H__FFCA2B84_F9C5_11D4_A6F1_0050CE184C9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


/////////////////////////////////////////////////////////////////////////////
// Global variables

extern UINT g_uClipbrdFormatProjectItem;
extern UINT g_uClipbrdFormatFileTabItem;
extern BOOL g_bDoubleByteCharacterSet;


/////////////////////////////////////////////////////////////////////////////
// CCedtApp:

class CCedtApp : public CWinApp
{
public:
	CCedtApp();

public: // general preferences
	static CString m_szLoadingDirectory;
	static CString m_szInstallDirectory;
	static CString m_szAppDataDirectory;

	static CString m_szInitialWorkingDirectory;
	static CString m_szRemoteBackupDirectory;

	static BOOL m_bAllowMultiInstances;
	static BOOL m_bFirstInstance;

	static BOOL m_bCreateNewDocumentOnStartup;
	static BOOL m_bReloadWorkingFilesOnStartup;
	static BOOL m_bCheckIfFilesModifiedOutside;
	static BOOL m_bCloseTabUsingDoubleClick;

public: // global cursor resources
	static HCURSOR m_hCursorArrow, m_hCursorIBeam, m_hCursorCross;
	static HCURSOR m_hCursorRightArrow, m_hCursorArrowMacro, m_hCursorIBeamMacro;

public: // ftp accounts and file filters
	static CFtpAccount m_clsFtpAccounts[MAX_FTP_ACCOUNT];
	static CFileFilter m_clsFileFilters[MAX_FILE_FILTER];
	static INT m_nFilterIndexDialog, m_nFilterIndexPannel;

public: // html help initialization
	static BOOL m_bHtmlHelpInitialized;
	static DWORD m_dwHtmlHelpCookie;

public: // open template
	static CString m_szOpenTemplatePathName;
	static BOOL m_bOpenTemplate;

public: // open memtext
	static CMemText m_clsMemText;
	static BOOL m_bOpenMemText;

protected: // post open document file
	BOOL	m_bPostOpenDocument;
	INT		m_nPostOpenFtpAccount;
	CString	m_szPostOpenPathName;
	INT		m_nPostOpenLineNum;

protected: // project management
	CString	m_szPrevWorkspacePathName;
	BOOL	m_bProjectLoaded;
	CString	m_szProjectPathName;
	CString m_szProjectInitialDirectory;


protected: // *** cedtApp.cpp ***
	void OnAnotherInstance();

public:
	POSITION GetFirstDocPosition();
	CDocument * GetNextDoc(POSITION & rPos);

	INT GetDocumentCount();
	INT GetViewCount();


public: // *** cedtAppFilter.cpp ***
	static INT  GetFilterIndexDialog() { return RecalcFilterIndexSkipNull(m_nFilterIndexDialog); }
	static void SetFilterIndexDialog(INT nFilter) { m_nFilterIndexDialog = RecalcFilterIndexCountAll(nFilter); }

	static CString GetComposedFileFilter();
	static CString GetDefaultFileExtension();

	static void ApplyFileFilterToFileWindow();

protected:
	static void CallbackFilterIndexDialog(INT nFilter);
	static void CallbackFilterIndexPannel(INT nFilter);

	static INT RecalcFilterIndexSkipNull(INT nFilter);
	static INT RecalcFilterIndexCountAll(INT nFilter);


public: // *** cedtAppFile.cpp ***
	BOOL ReloadLastWorkingFiles();
	BOOL ProcessShellCommand(CCmdLine & rCmdLine);

	BOOL PromptSaveFileName(CString & szPathName);
	BOOL PromptSaveRemoteFileName(INT & nFtpAccount, CString & szPathName);

	BOOL PostOpenDocumentFile(LPCTSTR lpszPathName, INT nLineNum); /* delayed version using message, because of input focus */
	BOOL PostOpenRemoteDocumentFile(INT nFtpAccount, LPCTSTR lpszPathName, INT nLineNum);

	CDocument * SpawnDocumentFile(LPCTSTR lpszPathName, INT nLineNum, WINDOWPLACEMENT * lpwndpl);
	CDocument * SpawnRemoteDocumentFile(INT nFtpAccount, LPCTSTR lpszPathName, INT nLineNum, WINDOWPLACEMENT * lpwndpl);

	CDocument * OpenDocumentFile(LPCTSTR lpszPathName) { return OpenDocumentFile(lpszPathName, 0, NULL); }
	CDocument * OpenDocumentFile(LPCTSTR lpszPathName, INT nLineNum, WINDOWPLACEMENT * lpwndpl);
	CDocument * OpenRemoteDocumentFile(INT nFtpAccount, LPCTSTR lpszPathName, INT nLineNum, WINDOWPLACEMENT * lpwndpl);

protected:
	CDocument * GetDocumentFromLocalPathName(LPCTSTR lpszPathName);


public: // *** cedtAppView.cpp ***
	void ApplyPreferencesToAllViews();
	void UpdateAllViews();

protected:
	void SaveCaretAndAnchorPosAllViews();
	void RestoreCaretAndAnchorPosAllViews();

	void FormatScreenTextAllViews();
	void TurnOffWordWrapModeAllViews();


protected: // *** cedtAppHndr.cpp ***
	void OnViewLineSpacing(INT nSpacing);
	void OnUpdateViewLineSpacing(INT nSpacing, CCmdUI * pCmdUI);

	void OnViewScreenFont(INT nFont);
	void OnUpdateViewScreenFont(INT nFont, CCmdUI * pCmdUI);

	void OnViewPrinterFont(INT nFont);
	void OnUpdateViewPrinterFont(INT nFont, CCmdUI * pCmdUI);

	void OnViewTabSize(INT nTabSize);
	void OnUpdateViewTabSize(INT nTabSize, CCmdUI * pCmdUI);

	void OnViewColorScheme(INT nScheme);


protected: // *** cedtAppSearch.cpp ***
	BOOL DoFindInFiles(LPCTSTR lpszFindString, LPCTSTR lpszFileType, LPCTSTR lpszFolder, BOOL bLookInSubfolders, UINT nOptions);
	INT  FindInFilesInFolder(LPCTSTR lpszFindString, LPCTSTR lpszFileType, LPCTSTR lpszFolder, BOOL bLookInSubfolders, UINT nOptions, CRegExp & clsRegExp);
	INT  FindInFilesInFile(LPCTSTR lpszFindString, LPCTSTR lpszFilePath, UINT nOptions, CRegExp & clsRegExp);


public: // *** cedtAppProject.cpp ***
	BOOL IsProjectLoaded() { return m_bProjectLoaded; }
	CString GetProjectPathName() { return m_szProjectPathName; }

	BOOL CloseProjectWorkspace(BOOL bCloseAll);
	BOOL SaveProjectWorkspace();
	BOOL SaveRegularWorkspace();

protected:
	BOOL NewProjectWorkspace(LPCTSTR lpszPathName);
	BOOL OpenProjectWorkspace(LPCTSTR lpszPathName);
	BOOL OpenRegularWorkspace(LPCTSTR lpszPathName);


public: // *** cedtAppRegistry.cpp ***
	BOOL SaveMultiInstancesFlag(LPCTSTR lpszProfileName);
	BOOL LoadMultiInstancesFlag(LPCTSTR lpszProfileName);

	BOOL SaveBrowsingDirectory(LPCTSTR lpszProfileName);
	BOOL LoadBrowsingDirectory(LPCTSTR lpszProfileName);

	BOOL SaveWorkingDirectory(LPCTSTR lpszProfileName);
	BOOL LoadWorkingDirectory(LPCTSTR lpszProfileName);

	BOOL SaveWorkspaceFilePath(LPCTSTR lpszProfileName);
	BOOL LoadWorkspaceFilePath(LPCTSTR lpszProfileName);

	static BOOL IsUsedInInternetExplorer();
	static BOOL UseInInternetExplorer(BOOL bUse);
	static BOOL IsAddedToRightMouseButton();
	static BOOL AddToRightMouseButton(BOOL bAdd);


public: // *** cedtAppConf.cpp ***
	static BOOL SaveFtpAccountInfo(LPCTSTR lpszPathName);
	static BOOL LoadFtpAccountInfo(LPCTSTR lpszPathName);

	static BOOL SaveUserCommands(LPCTSTR lpszPathName);
	static BOOL LoadUserCommands(LPCTSTR lpszPathName);
	static BOOL SaveMacroBuffers(LPCTSTR lpszPathName);
	static BOOL LoadMacroBuffers(LPCTSTR lpszPathName);

	static BOOL SetPredefinedColorScheme(INT nScheme);
	static BOOL LoadColorScheme(LPCTSTR lpszPathName);
	static BOOL SaveColorScheme(LPCTSTR lpszPathName);

	static void SetDefaultConfiguration();
	static BOOL LoadUserConfiguration(LPCTSTR lpszPathName);
	static BOOL SaveUserConfiguration(LPCTSTR lpszPathName);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCedtApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CCedtApp)
	afx_msg void OnUpdateFileMruFile1(CCmdUI* pCmdUI);
	afx_msg void OnFileOpen();
	afx_msg void OnAppAbout();
	afx_msg void OnHelpTopics();
	afx_msg void OnHelpVisitHomepage();
	afx_msg void OnHelpFeedback();
	afx_msg void OnHelpSponsoring();
	afx_msg void OnToolPreferences();
	afx_msg void OnMacroConfigure();
	afx_msg void OnCommandConfigure();
	afx_msg void OnViewSetColors();
	afx_msg void OnFileFtpSettings();
	afx_msg void OnFileOpenRemote();
	afx_msg void OnFileOpenTemplate();
	afx_msg void OnViewSetLineSpacing();
	afx_msg void OnViewSetTabSize();
	afx_msg void OnFileCloseAll();
	afx_msg void OnFileCheckModified();
	afx_msg void OnDocuSyntaxCustomize();
	afx_msg void OnViewColumnMarkers();
	afx_msg void OnSearchFindInFiles();
	afx_msg void OnProjectNew();
	afx_msg void OnProjectOpen();
	afx_msg void OnProjectClose();
	afx_msg void OnProjectNewCategory();
	afx_msg void OnProjectAddFilesTo();
	afx_msg void OnProjectAddActiveFile();
	afx_msg void OnProjectAddOpenFiles();
	afx_msg void OnUpdateProjectClose(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProjectNewCategory(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProjectAddFilesTo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProjectAddActiveFile(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProjectAddOpenFiles(CCmdUI* pCmdUI);
	afx_msg void OnViewSetScreenFonts();
	afx_msg void OnViewSetPrinterFonts();
	afx_msg void OnUpdateCommandUserFile0(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMacroUserFile0(CCmdUI* pCmdUI);
	afx_msg void OnCommandUserFile0();
	afx_msg void OnCommandUserFile1();
	afx_msg void OnCommandUserFile2();
	afx_msg void OnCommandUserFile3();
	afx_msg void OnCommandUserFile4();
	afx_msg void OnCommandUserFile5();
	afx_msg void OnCommandUserFile6();
	afx_msg void OnCommandUserFile7();
	afx_msg void OnMacroUserFile0();
	afx_msg void OnMacroUserFile1();
	afx_msg void OnMacroUserFile2();
	afx_msg void OnMacroUserFile3();
	afx_msg void OnMacroUserFile4();
	afx_msg void OnMacroUserFile5();
	afx_msg void OnMacroUserFile6();
	afx_msg void OnMacroUserFile7();
	afx_msg void OnEditColumnMode();
	afx_msg void OnUpdateEditColumnMode(CCmdUI* pCmdUI);
	afx_msg void OnIndicatorOvr();
	afx_msg void OnViewLineNumbers();
	afx_msg void OnUpdateViewLineNumbers(CCmdUI* pCmdUI);
	afx_msg void OnViewLineSpacing100();
	afx_msg void OnViewLineSpacing110();
	afx_msg void OnViewLineSpacing120();
	afx_msg void OnViewLineSpacing150();
	afx_msg void OnViewLineSpacing200();
	afx_msg void OnUpdateViewLineSpacing100(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewLineSpacing110(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewLineSpacing120(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewLineSpacing150(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewLineSpacing200(CCmdUI* pCmdUI);
	afx_msg void OnViewScreenFont0();
	afx_msg void OnViewScreenFont1();
	afx_msg void OnViewScreenFont2();
	afx_msg void OnViewScreenFont3();
	afx_msg void OnViewScreenFont4();
	afx_msg void OnViewScreenFont5();
	afx_msg void OnUpdateViewScreenFont0(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewScreenFont1(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewScreenFont2(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewScreenFont3(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewScreenFont4(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewScreenFont5(CCmdUI* pCmdUI);
	afx_msg void OnViewPrinterFont0();
	afx_msg void OnViewPrinterFont1();
	afx_msg void OnViewPrinterFont2();
	afx_msg void OnViewPrinterFont3();
	afx_msg void OnUpdateViewPrinterFont0(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewPrinterFont1(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewPrinterFont2(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewPrinterFont3(CCmdUI* pCmdUI);
	afx_msg void OnViewTabSize02();
	afx_msg void OnViewTabSize04();
	afx_msg void OnViewTabSize08();
	afx_msg void OnViewTabSize16();
	afx_msg void OnUpdateViewTabSize02(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTabSize04(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTabSize08(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTabSize16(CCmdUI* pCmdUI);
	afx_msg void OnViewColorSchemeDefault();
	afx_msg void OnViewColorSchemeSimplified();
	afx_msg void OnViewColorSchemeReversed();
	afx_msg void OnViewColorSchemeLightgray();
	afx_msg void OnViewColorSchemeDarkblue();
	afx_msg void OnViewColorSchemeSaved();
	afx_msg void OnViewEmboldenKeywords();
	afx_msg void OnViewItalicizeComment();
	afx_msg void OnViewShowSpaces();
	afx_msg void OnViewShowTabChars();
	afx_msg void OnViewShowLineBreak();
	afx_msg void OnViewShowAllWhite();
	afx_msg void OnUpdateViewEmboldenKeywords(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewItalicizeComment(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewShowSpaces(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewShowTabChars(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewShowLineBreak(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewShowAllWhite(CCmdUI* pCmdUI);
	afx_msg void OnDirectoryItemOpen();
	afx_msg void OnDirectoryItemExecute();
	afx_msg void OnDirectoryItemExplore();
	afx_msg void OnDirectoryItemFind();
	afx_msg void OnDirectoryItemNewFolder();
	afx_msg void OnDirectoryItemNewDocument();
	afx_msg void OnDirectoryItemSetWorkdir();
	afx_msg void OnDirectoryItemMove();
	afx_msg void OnDirectoryItemCopy();
	afx_msg void OnDirectoryItemDelete();
	afx_msg void OnDirectoryItemRename();
	afx_msg void OnDirectoryItemProperty();
	afx_msg void OnDirectoryItemRefresh();
	afx_msg void OnProjectItemOpen();
	afx_msg void OnProjectItemExecute();
	afx_msg void OnProjectItemRemove();
	afx_msg void OnProjectItemRename();
	afx_msg void OnProjectItemProperty();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CEDT_H__FFCA2B84_F9C5_11D4_A6F1_0050CE184C9B__INCLUDED_)

#if !defined(AFX_PREFERENCEDIALOG_H__CF938562_0FD5_11D5_A6F1_0050CE184C9B__INCLUDED_)
#define AFX_PREFERENCEDIALOG_H__CF938562_0FD5_11D5_A6F1_0050CE184C9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreferenceDialog.h : header file
//

#include "Separator.h"


#define PREF_CATEGORY_GENERAL		0x01
#define PREF_CATEGORY_VISUAL		0x02
#define PREF_CATEGORY_FONTS			0x03
#define PREF_CATEGORY_COLORS		0x04
#define PREF_CATEGORY_PRINT			0x05
#define PREF_CATEGORY_FILE			0x06
#define PREF_CATEGORY_DIRECTORY		0x07
#define PREF_CATEGORY_ASSOC		 	0x08
#define PREF_CATEGORY_BACKUP		0x09
#define PREF_CATEGORY_SYNTAX		0x0A
#define PREF_CATEGORY_FILTERS		0x0B
#define PREF_CATEGORY_TOOLS			0x0C
#define PREF_CATEGORY_COMMANDS		0x0D
#define PREF_CATEGORY_MACROS		0x0E
#define PREF_CATEGORY_OUTPUT		0x0F

#define FONTS_PANNEL_SCREEN			0x00
#define FONTS_PANNEL_PRINTER		0x01
#define FONTS_PANNEL_MISC			0x02

#define COLORS_PANNEL_GENERAL		0x00
#define COLORS_PANNEL_KEYWORD		0x01
#define COLORS_PANNEL_MISC			0x02


/////////////////////////////////////////////////////////////////////////////
// CPreferenceDialog dialog

class CPreferenceDialog : public CDialog
{
// Construction
public:
	CPreferenceDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPreferenceDialog)
	enum { IDD = IDD_PREFERENCE_DIALOG };
	CComboBox	m_cmbDefFileFormat;
	CComboBox	m_cmbDefEncodingType;
	CStatic	m_stcDefFileFormat;
	CStatic	m_stcDefEncodingType;
	CStatic	m_stcDefDoctypeTitle;
	CStatic	m_stcDefDoctypeBox;
	CStatic	m_stcSaveSettingsBox;
	CButton	m_chkUseTabAsIndentation;
	CButton	m_chkCheckFileModOutside;
	CButton	m_chkCopyLineNothingSelected;
	CEdit	m_edtIndentationSize;
	CStatic	m_stcIndentationSize;
	CStatic	m_stcIndentationSizeBox;
	CListCtrl	m_lstMiscelFonts;
	CStatic	m_stcMiscelFonts;
	CStatic	m_stcMacroItemBox;
	CStatic	m_stcMacroSettingsBox;
	CStatic	m_stcCommandItemBox;
	CStatic	m_stcCommandSettingsBox;
	CListCtrl	m_lstSyntaxTypes;
	CStatic	m_stcToolSettingsBox;
	CStatic	m_stcFilterItemBox;
	CStatic	m_stcFilterSettingsBox;
	CStatic	m_stcSyntaxItemBox;
	CStatic	m_stcSyntaxSettingsBox;
	CStatic	m_stcBackupMethodBox;
	CStatic	m_stcBackupDirectoryBox;
	CStatic	m_stcBackupExtensionBox;
	CStatic	m_stcBackupSettingsBox;
	CStatic	m_stcAssocItemBox;
	CStatic	m_stcAssocSettingsBox;
	CButton	m_chkSaveRemoteFilesUnix;
	CButton	m_chkSaveFilesUnix;
	CButton	m_chkReloadWorkingFiles;
	CButton	m_chkCreateNewDocument;
	CStatic	m_stcFileSettingsBox;
	CButton	m_btnRemoteDirectory;
	CEdit	m_edtRemoteDirectory;
	CStatic	m_stcRemoteDirectory;
	CStatic	m_stcRemoteDirectoryBox;
	CEdit	m_edtWorkingDirectory;
	CStatic	m_stcWorkingDirectory;
	CButton	m_btnWorkingDirectory;
	CStatic	m_stcWorkingDirectoryBox;
	CStatic	m_stcPageMarginTitle;
	CStatic	m_stcPrintOptionsBox;
	CStatic	m_stcPrintFooterBox;
	CStatic	m_stcPrintHeaderBox;
	CStatic	m_stcPageMarginBox;
	CStatic	m_stcColorSchemeBox;
	CStatic	m_stcVisualOptionsBox;
	CStatic	m_stcColumnMarkersBox;
	CStatic	m_stcLineNumberBox;
	CStatic	m_stcLineSpacingBox;
	CStatic	m_stcGeneralOptionsBox;
	CStatic	m_stcWordWrapBox;
	CStatic	m_stcTabSizeBox;
	CButton	m_btnMacroLoadMacros;
	CButton	m_btnCommandLoadTools;
	CButton	m_chkPrintLineNumbers;
	CButton	m_chkShowLineNumbers;
	CButton	m_chkShowSelectionMargin;
	CButton	m_chkSearchWrapAtEndOfFile;
	CButton	m_chkOpenDocumentWordWrapped;
	CButton	m_chkDragAndDropEditing;
	CButton	m_chkHomeKeyToFirstPosition;
	CButton	m_chkEnableAutoIndent;
	CButton	m_chkEnablePairsMatching;
	CButton	m_chkShowTabChars;
	CListCtrl	m_lstMacroList;
	CListCtrl	m_lstCommandList;
	CButton	m_btnMacroSaveMacros;
	CButton	m_btnCommandSaveTools;
	CEdit	m_edtAssocAssociate;
	CStatic	m_stcAssocDefaultIcon;
	CButton	m_btnAssocDefaultIcon;
	CEdit	m_edtAssocDefaultIcon;
	CButton	m_btnAssocRestore;
	CStatic	m_stcAssocProgram;
	CButton	m_btnAssocProgram;
	CEdit	m_edtAssocProgram;
	CListBox	m_lstAssocExtensions;
	CStatic	m_stcAssocDescription;
	CEdit	m_edtAssocDescription;
	CStatic	m_stcAssocAssociated;
	CListBox	m_lstAssocAssociated;
	CButton	m_btnAssocAssociate;
	CButton	m_btnSaveColorScheme;
	CStatic	m_stcSaveColorScheme;
	CEdit	m_edtFilterDefaultExt;
	CStatic	m_stcFilterDefaultExt;
	CButton	m_chkUseSpacesInPlaceOfTab;
	CButton	m_chkEmboldenKeywords;
	CEdit	m_edtLineNumberDigits;
	CStatic	m_stcLineNumberDigits;
	CStatic	m_stcHighlightedColor;
	CStatic	m_stcRangeBkgrColor;
	CStatic	m_stcLeftMarginColor;
	CTabCtrl	m_tabFontsPannel;
	CTabCtrl	m_tabColorsPannel;
	CStatic	m_btnStringColor;
	CButton	m_chkItalicizeComment;
	CStatic	m_stcFilterExtensions;
	CEdit	m_edtFilterExtensions;
	CListCtrl	m_lstFileFilters;
	CButton	m_btnSyntaxMoveUp;
	CButton	m_btnSyntaxKeywords;
	CEdit	m_edtSyntaxKeywords;
	CStatic	m_stcSyntaxKeywords;
	CButton	m_btnSyntaxLangSpec;
	CEdit	m_edtSyntaxLangSpec;
	CStatic	m_stcSyntaxLangSpec;
	CEdit	m_edtSyntaxDescription;
	CStatic	m_stcSyntaxDescription;
	CButton	m_btnSyntaxRemove;
	CButton	m_btnSyntaxMoveDown;
	CEdit	m_edtColumnMarker2;
	CEdit	m_edtColumnMarker1;
	CButton	m_chkColumnMarker2;
	CButton	m_chkColumnMarker1;
	CStatic	m_btnRange2BkgrColor;
	CStatic	m_btnRange1BkgrColor;
	CStatic	m_btnShadowedColor;
	CStatic	m_btnHighlightedColor;
	CComboBox	m_cmbColorSchemeList;
	CButton	m_btnLoadColorScheme;
	CStatic	m_stcLoadColorScheme;
	CStatic	m_btnActiveLineColor;
	CStatic	m_stcAddToRightMouseButton;
	CButton	m_chkAddToRightMouseButton;
	CStatic	m_stcTextColor;
	CStatic	m_stcKeywordColor;
	CStatic	m_stcBackgroundColor;
	CButton	m_btnBackupDirectory;
	CButton	m_btnBackupMethod2;
	CButton	m_btnBackupMethod1;
	CStatic	m_stcBackupMethod;
	CStatic	m_stcBackupDirectoryDesc;
	CEdit	m_edtBackupDirectory;
	CStatic	m_stcBackupDirectory;
	CStatic	m_stcBackupExtensionDesc2;
	CStatic	m_stcBackupExtensionDesc1;
	CEdit	m_edtBackupExtension;
	CStatic	m_stcBackupExtension;
	CButton	m_btnMakeBackupFile2;
	CButton	m_btnMakeBackupFile1;
	CButton	m_btnMakeBackupFile0;
	CStatic	m_btnKeyword9Color;
	CStatic	m_btnKeyword8Color;
	CStatic	m_btnKeyword7Color;
	CStatic	m_btnKeyword6Color;
	CStatic	m_btnKeyword5Color;
	CStatic	m_btnKeyword4Color;
	CStatic	m_btnKeyword3Color;
	CStatic	m_btnKeyword2Color;
	CStatic	m_btnKeyword1Color;
	CStatic	m_btnKeyword0Color;
	CStatic	m_btnLineNumberColor;
	CStatic	m_btnDelimiterColor;
	CStatic	m_btnConstantColor;
	CStatic	m_btnCommentColor;
	CStatic	m_btnVariableColor;
	CStatic	m_btnWordColor;
	CStatic	m_btnBackgroundColor;
	CStatic	m_btnLeftMarginColor;
	CButton	m_chkRemoveTrailingSpaces;
	CButton	m_chkConvertTabsToSpaces;
	CButton	m_chkShowLineBreak;
	CStatic	m_stcFileTabLength;
	CEdit	m_edtFileTabLength;
	CStatic	m_stcCaretWidthDesc;
	CEdit	m_edtCaretWidth;
	CStatic	m_stcCaretWidth;
	CStatic	m_stcMacroHotKey;
	CStatic	m_stcCommandHotKey;
	CHotKeyCtrl	m_hkyMacroHotKey;
	CHotKeyCtrl	m_hkyCommandHotKey;
	CStatic	m_stcWrapIndentation;
	CStatic	m_stcTabSize;
	CStatic	m_stcScreenFonts;
	CStatic	m_stcPrinterFonts;
	CStatic	m_stcPrintHeader2;
	CStatic	m_stcPrintHeader1;
	CStatic	m_stcPrintHeader0;
	CStatic	m_stcPrintFooter2;
	CStatic	m_stcPrintFooter1;
	CStatic	m_stcPrintFooter0;
	CStatic	m_stcPageMarginTop;
	CStatic	m_stcPageMarginRight;
	CStatic	m_stcPageMarginLeft;
	CStatic	m_stcPageMarginBottom;
	CStatic	m_stcMacroText;
	CStatic	m_stcLineSpacing;
	CStatic	m_stcCommandText;
	CStatic	m_stcCommandDirectory;
	CStatic	m_stcCommandCommand;
	CStatic	m_stcCommandArgument;
	CTreeCtrl	m_treCategories;
	CStatic	m_stcCategories;
	CButton	m_btnOK;
	CButton	m_btnCancel;
	CButton	m_btnApply;
	CButton	m_chkPrintSyntaxHighlight;
	CButton	m_chkCommandShortFileName;
	CButton	m_btnCommandDirectory;
	CStatic	m_stcFilterDescription;
	CEdit	m_edtFilterDescription;
	CButton	m_btnFilterMoveDown;
	CButton	m_btnFilterMoveUp;
	CButton	m_btnFilterRemove;
	CEdit	m_edtPrintHeader0;
	CButton	m_btnPrintFooter2;
	CButton	m_btnPrintFooter1;
	CButton	m_btnPrintFooter0;
	CEdit	m_edtPrintFooter2;
	CEdit	m_edtPrintFooter1;
	CEdit	m_edtPrintFooter0;
	CButton	m_btnPrintHeader2;
	CButton	m_btnPrintHeader1;
	CButton	m_btnPrintHeader0;
	CEdit	m_edtPrintHeader2;
	CEdit	m_edtPrintHeader1;
	CButton	m_chkPrintFooter;
	CButton	m_chkPrintHeader;
	CEdit	m_edtPageMarginBottom;
	CEdit	m_edtPageMarginTop;
	CEdit	m_edtPageMarginRight;
	CEdit	m_edtPageMarginLeft;
	CEdit	m_edtWrapIndentation;
	CEdit	m_edtTabSize;
	CEdit	m_edtMacroText;
	CEdit	m_edtLineSpacing;
	CEdit	m_edtFixedWrapWidth;
	CEdit	m_edtCommandText;
	CEdit	m_edtCommandDirectory;
	CEdit	m_edtCommandCommand;
	CEdit	m_edtCommandArgument;
	CButton	m_chkUseInInternetExplorer;
	CButton	m_chkHighlightActiveLine;
	CButton	m_btnCommandMoveDown;
	CButton	m_btnCommandMoveUp;
	CButton	m_btnMacroMoveDown;
	CButton	m_btnMacroMoveUp;
	CButton	m_btnCommandArgument;
	CButton	m_btnCommandCommand;
	CButton	m_chkCommandSaveBefore;
	CButton	m_chkCommandCloseOnExit;
	CButton	m_chkCommandCaptureOutput;
	CButton	m_btnCommandRemove;
	CButton	m_btnMacroRemove;
	CListCtrl	m_lstPrinterFonts;
	CListCtrl	m_lstScreenFonts;
	CButton	m_chkAllowMultiInstances;
	CButton	m_chkFixedWrapWidth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreferenceDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	INT DoModal() { return CDialog::DoModal(); }
	INT DoModal(INT nCategory) { m_nActiveCategory = nCategory; return CDialog::DoModal(); }
	INT DoModal(INT nCategory, INT nFontsPannel) { m_nActiveCategory = nCategory; m_nActiveFontsPannel = nFontsPannel; return CDialog::DoModal(); }

protected:
	CImageList m_lstButtonImage;
	CSeparator m_ctlSeparator;

	HTREEITEM m_hItemGeneral, m_hItemVisual, m_hItemFonts, m_hItemColors, m_hItemPrint;
	HTREEITEM m_hItemFile, m_hItemDirectory, m_hItemAssoc, m_hItemBackup, m_hItemSyntax, m_hItemFilters;
	HTREEITEM m_hItemTools, m_hItemCommands, m_hItemMacros, m_hItemOutput;
	static INT m_nActiveCategory;

	// fonts
	LOGFONT m_lfScreen[10], m_lfPrinter[10], m_lfMiscel[10];
	static INT m_nActiveScreenFont, m_nActivePrinterFont, m_nActiveMiscelFont;
	static INT m_nActiveFontsPannel;

	// colors
	COLORREF m_crBkgrColor[10];
	COLORREF m_crTextColor[40];
	static INT m_nActiveColorsPannel;
	static INT m_nActiveColorScheme;

	// file associations
	BOOL m_bAssocInfoLoaded;
	static CString m_szActiveAssocExtension;

	// syntax type
	CSyntaxType m_clsSyntaxTypes[MAX_SYNTAX_TYPE];
	static INT m_nActiveSyntaxType;

	// filters
	CFileFilter m_clsFileFilters[MAX_FILE_FILTER];
	static INT m_nActiveFileFilter;

	// commands
	CUserCommand m_clsUserCommand[11];
	static INT m_nActiveUserCommand;

	// macros
	CMacroBuffer m_clsMacroBuffer[11];
	static INT m_nActiveMacroBuffer;


protected: // *** PrefDialog.cpp ***
	void InitAllPrefPages();
	void SizeAllPrefPages();
	void ShowAllPrefPages();

	BOOL LoadAllPrefSettings();
	BOOL SaveAllPrefSettings(); // this function will apply pref settings also 

	HTREEITEM InsertCategoryItem(UINT nResourceID, HTREEITEM hParent, UINT nCategory);

protected: // *** PrefDialogGeneral.cpp ***
	void InitGeneralPage();
	void SizeGeneralPage();
	void ShowGeneralPage();

	BOOL LoadGeneralSettings();
	BOOL SaveGeneralSettings();

protected: // *** PrefDialogVisual.cpp ***
	void InitVisualPage();
	void SizeVisualPage();
	void ShowVisualPage();

	BOOL LoadVisualSettings();
	BOOL SaveVisualSettings();

protected: // *** PrefDialogFonts.cpp ***
	void InitFontsPage();
	void SizeFontsPage();
	void ShowFontsPage();

	BOOL LoadFontSettings();
	BOOL SaveFontSettings();

	void DispScreenFontText(INT nScreenFont);
	void DispPrinterFontText(INT nPrinterFont);
	void DispMiscelFontText(INT nMiscelFont);

	void OnScreenFontChange();
	void OnPrinterFontChange();
	void OnMiscelFontChange();

protected: // *** PrefDialogColors.cpp ***
	void InitColorsPage();
	void SizeColorsPage();
	void ShowColorsPage();

	BOOL LoadColorSettings();
	BOOL SaveColorSettings();

	void InvalidateColorButtons();

	BOOL SetPredefinedColorScheme(INT nScheme);
	BOOL LoadColorScheme(LPCTSTR lpszPathName);
	BOOL SaveColorScheme(LPCTSTR lpszPathName);

protected: // *** PrefDialogPrint.cpp ***
	void InitPrintPage();
	void SizePrintPage();
	void ShowPrintPage();

	BOOL LoadPrintSettings();
	BOOL SavePrintSettings();

protected: // *** PrefDialogFile.cpp ***
	void InitFilePage();
	void SizeFilePage();
	void ShowFilePage();

	BOOL LoadFileSettings();
	BOOL SaveFileSettings();

protected: // *** PrefDialogDirectory.cpp ***
	void InitDirectoryPage();
	void SizeDirectoryPage();
	void ShowDirectoryPage();

	BOOL LoadDirectorySettings();
	BOOL SaveDirectorySettings();

protected: // *** PrefDialogAssoc.cpp ***
	void InitAssocPage();
	void SizeAssocPage();
	void ShowAssocPage();

	BOOL LoadAssocSettings();
	BOOL SaveAssocSettings();

	void DispAssociationInfo(LPCTSTR lpszExtension);
	void UpdateAssociationInfo(LPCTSTR lpszExtension);

	void SetAssocExtensionsCurSel(LPCTSTR lpszExtension);
	void SetAssocAssociatedCurSel(LPCTSTR lpszExtension);

	BOOL AssociateExtension(LPCTSTR lpszExtension);
	BOOL RestoreAssociation(LPCTSTR lpszExtension);

protected: // *** PrefDialogBackup.cpp ***
	void InitBackupPage();
	void SizeBackupPage();
	void ShowBackupPage();

	BOOL LoadBackupSettings();
	BOOL SaveBackupSettings();

protected: // *** PrefDialogSyntax.cpp ***
	void InitSyntaxPage();
	void SizeSyntaxPage();
	void ShowSyntaxPage();

	BOOL LoadSyntaxTypes();
	BOOL SaveSyntaxTypes();
	BOOL DispSyntaxType(INT nSyntaxType);

protected: // *** PrefDialogFilters.cpp
	void InitFiltersPage();
	void SizeFiltersPage();
	void ShowFiltersPage();

	BOOL LoadFileFilters();
	BOOL SaveFileFilters();
	BOOL DispFileFilterText(INT nFilter);

protected: // *** PrefDialogTools.cpp
	void InitToolsPage();
	void SizeToolsPage();
	void ShowToolsPage();

	BOOL LoadToolsSettings();
	BOOL SaveToolsSettings();

protected: // *** PrefDialogCommands.cpp
	void InitCommandsPage();
	void SizeCommandsPage();
	void ShowCommandsPage();

	BOOL LoadUserCommands();
	BOOL SaveUserCommands();

	BOOL FileLoadUserCommands(LPCTSTR lpszPathName);
	BOOL FileSaveUserCommands(LPCTSTR lpszPathName);

	BOOL DispCommandText(INT nCommand);

	CString GetCommandName(INT nCommand);
	CString GetCommandHotKeyText(INT nCommand);

	void OnChangeCommandHotKey();

protected: // *** PrefDialogMacros.cpp
	void InitMacrosPage();
	void SizeMacrosPage();
	void ShowMacrosPage();

	BOOL LoadMacroBuffers();
	BOOL SaveMacroBuffers();

	BOOL FileLoadMacroBuffers(LPCTSTR lpszPathName);
	BOOL FileSaveMacroBuffers(LPCTSTR lpszPathName);

	BOOL DispMacroText(INT nMacro);

	CString GetMacroName(INT nMacro);
	CString GetMacroHotKeyText(INT nMacro);

	void OnChangeMacroHotKey();

protected: // *** PrefDialogOutput.cpp ***
	void InitOutputPage();
	void SizeOutputPage();
	void ShowOutputPage();

	BOOL LoadOutputSettings();
	BOOL SaveOutputSettings();


	// Generated message map functions
	//{{AFX_MSG(CPreferenceDialog)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedCategories(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkScreenFonts(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkPrinterFonts(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnApply();
	afx_msg void OnFixedWrapWidthCheck();
	afx_msg void OnItemchangedScreenFonts(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedPrinterFonts(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedCommandList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedMacroList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeMacroText();
	afx_msg void OnMacroRemove();
	afx_msg void OnMacroMoveUp();
	afx_msg void OnMacroMoveDown();
	afx_msg void OnChangeCommandText();
	afx_msg void OnChangeCommandCommand();
	afx_msg void OnChangeCommandArgument();
	afx_msg void OnChangeCommandDirectory();
	afx_msg void OnCommandCloseOnExit();
	afx_msg void OnCommandCaptureOutput();
	afx_msg void OnCommandSaveBefore();
	afx_msg void OnCommandRemove();
	afx_msg void OnCommandMoveUp();
	afx_msg void OnCommandMoveDown();
	afx_msg void OnCommandCommandBrowse();
	afx_msg void OnCommandArgumentMenu();
	afx_msg void OnCommandDirectoryMenu();
	afx_msg void OnCmdArgFilePath();
	afx_msg void OnCmdArgFileDir();
	afx_msg void OnCmdArgFileName();
	afx_msg void OnCmdArgFileTitle();
	afx_msg void OnCmdArgProjectPath();
	afx_msg void OnCmdArgProjectDir();
	afx_msg void OnCmdArgProjectTitle();
	afx_msg void OnCmdArgCurrentWord();
	afx_msg void OnCmdArgLineNumber();
	afx_msg void OnCmdArgUserInput();
	afx_msg void OnCmdArgSelectPath1();
	afx_msg void OnCmdArgSelectPath2();
	afx_msg void OnCmdArgSelectDir1();
	afx_msg void OnCmdArgSelectDir2();
	afx_msg void OnCmdDirFileDir();
	afx_msg void OnCmdDirProjectDir();
	afx_msg void OnCmdDirBrowse();
	afx_msg void OnPrintHeader();
	afx_msg void OnPrintFooter();
	afx_msg void OnPrintHeader0Menu();
	afx_msg void OnPrintHeader1Menu();
	afx_msg void OnPrintHeader2Menu();
	afx_msg void OnPrintFooter0Menu();
	afx_msg void OnPrintFooter1Menu();
	afx_msg void OnPrintFooter2Menu();
	afx_msg void OnPrintHeader0FilePath();
	afx_msg void OnPrintHeader0FileName();
	afx_msg void OnPrintHeader0PageNumber();
	afx_msg void OnPrintHeader0TotalPage();
	afx_msg void OnPrintHeader0CurrentDate();
	afx_msg void OnPrintHeader0CurrentTime();
	afx_msg void OnPrintHeader1FilePath();
	afx_msg void OnPrintHeader1FileName();
	afx_msg void OnPrintHeader1PageNumber();
	afx_msg void OnPrintHeader1TotalPage();
	afx_msg void OnPrintHeader1CurrentDate();
	afx_msg void OnPrintHeader1CurrentTime();
	afx_msg void OnPrintHeader2FilePath();
	afx_msg void OnPrintHeader2FileName();
	afx_msg void OnPrintHeader2PageNumber();
	afx_msg void OnPrintHeader2TotalPage();
	afx_msg void OnPrintHeader2CurrentDate();
	afx_msg void OnPrintHeader2CurrentTime();
	afx_msg void OnPrintFooter0FilePath();
	afx_msg void OnPrintFooter0FileName();
	afx_msg void OnPrintFooter0PageNumber();
	afx_msg void OnPrintFooter0TotalPage();
	afx_msg void OnPrintFooter0CurrentDate();
	afx_msg void OnPrintFooter0CurrentTime();
	afx_msg void OnPrintFooter1FilePath();
	afx_msg void OnPrintFooter1FileName();
	afx_msg void OnPrintFooter1PageNumber();
	afx_msg void OnPrintFooter1TotalPage();
	afx_msg void OnPrintFooter1CurrentDate();
	afx_msg void OnPrintFooter1CurrentTime();
	afx_msg void OnPrintFooter2FilePath();
	afx_msg void OnPrintFooter2FileName();
	afx_msg void OnPrintFooter2PageNumber();
	afx_msg void OnPrintFooter2TotalPage();
	afx_msg void OnPrintFooter2CurrentDate();
	afx_msg void OnPrintFooter2CurrentTime();
	afx_msg void OnWorkingDirectoryBrowse();
	afx_msg void OnItemchangedFilters(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeFilterDescription();
	afx_msg void OnChangeFilterExtension();
	afx_msg void OnFilterRemove();
	afx_msg void OnFilterMoveUp();
	afx_msg void OnFilterMoveDown();
	afx_msg void OnCommandShortFileName();
	afx_msg void OnBackgroundColor();
	afx_msg void OnLeftMarginColor();
	afx_msg void OnWordColor();
	afx_msg void OnVariableColor();
	afx_msg void OnCommentColor();
	afx_msg void OnConstantColor();
	afx_msg void OnDelimiterColor();
	afx_msg void OnLineNumberColor();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnKeyword0Color();
	afx_msg void OnKeyword1Color();
	afx_msg void OnKeyword2Color();
	afx_msg void OnKeyword3Color();
	afx_msg void OnKeyword4Color();
	afx_msg void OnKeyword5Color();
	afx_msg void OnKeyword6Color();
	afx_msg void OnKeyword7Color();
	afx_msg void OnKeyword8Color();
	afx_msg void OnKeyword9Color();
	afx_msg void OnMakeBackupFile0();
	afx_msg void OnMakeBackupFile1();
	afx_msg void OnMakeBackupFile2();
	afx_msg void OnBackupMethod1();
	afx_msg void OnBackupMethod2();
	afx_msg void OnBackupDirectoryBrowse();
	afx_msg void OnSelchangeAssocExtensions();
	afx_msg void OnSelchangeAssocAssociated();
	afx_msg void OnAssocAssociate();
	afx_msg void OnAssocRestore();
	afx_msg void OnKillfocusAssocDescription();
	afx_msg void OnKillfocusAssocProgram();
	afx_msg void OnKillfocusAssocDefaulticon();
	afx_msg void OnActiveLineColor();
	afx_msg void OnLoadColorScheme();
	afx_msg void OnHighlightedColor();
	afx_msg void OnShadowedColor();
	afx_msg void OnRange1BkgrColor();
	afx_msg void OnRange2BkgrColor();
	afx_msg void OnColumnMarker1Check();
	afx_msg void OnColumnMarker2Check();
	afx_msg void OnItemchangedSyntaxTypes(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeSyntaxDescription();
	afx_msg void OnChangeSyntaxLangSpec();
	afx_msg void OnChangeSyntaxKeywords();
	afx_msg void OnSyntaxRemove();
	afx_msg void OnSyntaxMoveUp();
	afx_msg void OnSyntaxMoveDown();
	afx_msg void OnSyntaxLangSpecBrowse();
	afx_msg void OnSyntaxKeywordsBrowse();
	afx_msg void OnStringColor();
	afx_msg void OnItemchangedMiscelFonts(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkMiscelFonts(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeColorsPannel(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeFontsPannel(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeFilterDefaultExt();
	afx_msg void OnSaveColorScheme();
	afx_msg void OnSelchangeColorSchemeList();
	afx_msg void OnCommandSaveTools();
	afx_msg void OnMacroSaveMacros();
	afx_msg void OnRemoteDirectoryBrowse();
	afx_msg void OnShowLineNumbers();
	afx_msg void OnCommandLoadTools();
	afx_msg void OnMacroLoadMacros();
	afx_msg void OnUseTabAsIndentation();
	afx_msg void OnUseInInternetExplorer();
	afx_msg void OnAddToRightMouseButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCEDIALOG_H__CF938562_0FD5_11D5_A6F1_0050CE184C9B__INCLUDED_)

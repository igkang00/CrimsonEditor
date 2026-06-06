// cedtDoc.cpp : implementation of the CCedtDoc class
//

#include "stdafx.h"
#include "cedtHeader.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCedtDoc
IMPLEMENT_DYNCREATE(CCedtDoc, CDocument)

BEGIN_MESSAGE_MAP(CCedtDoc, CDocument)
	//{{AFX_MSG_MAP(CCedtDoc)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_SAVE_ALL, OnFileSaveAll)
	ON_COMMAND(ID_FILE_SAVE_AS_REMOTE, OnFileSaveAsRemote)
	ON_COMMAND(ID_FILE_RELOAD, OnFileReload)
	ON_COMMAND(ID_FILE_RELOAD_AS, OnFileReloadAs)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_AUTO, OnUpdateDocumentSyntaxAuto)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TEXT, OnUpdateDocumentSyntaxText)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE0, OnUpdateDocumentSyntaxType0)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE1, OnUpdateDocumentSyntaxType1)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE2, OnUpdateDocumentSyntaxType2)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE3, OnUpdateDocumentSyntaxType3)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE4, OnUpdateDocumentSyntaxType4)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE5, OnUpdateDocumentSyntaxType5)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE6, OnUpdateDocumentSyntaxType6)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE7, OnUpdateDocumentSyntaxType7)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE8, OnUpdateDocumentSyntaxType8)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE9, OnUpdateDocumentSyntaxType9)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE10, OnUpdateDocumentSyntaxType10)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE11, OnUpdateDocumentSyntaxType11)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE12, OnUpdateDocumentSyntaxType12)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE13, OnUpdateDocumentSyntaxType13)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE14, OnUpdateDocumentSyntaxType14)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE15, OnUpdateDocumentSyntaxType15)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE16, OnUpdateDocumentSyntaxType16)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE17, OnUpdateDocumentSyntaxType17)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE18, OnUpdateDocumentSyntaxType18)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE19, OnUpdateDocumentSyntaxType19)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE20, OnUpdateDocumentSyntaxType20)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE21, OnUpdateDocumentSyntaxType21)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE22, OnUpdateDocumentSyntaxType22)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE23, OnUpdateDocumentSyntaxType23)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE24, OnUpdateDocumentSyntaxType24)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE25, OnUpdateDocumentSyntaxType25)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE26, OnUpdateDocumentSyntaxType26)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE27, OnUpdateDocumentSyntaxType27)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE28, OnUpdateDocumentSyntaxType28)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE29, OnUpdateDocumentSyntaxType29)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE30, OnUpdateDocumentSyntaxType30)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SYNTAX_TYPE31, OnUpdateDocumentSyntaxType31)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE0, OnDocumentSyntaxType0)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE1, OnDocumentSyntaxType1)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE2, OnDocumentSyntaxType2)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE3, OnDocumentSyntaxType3)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE4, OnDocumentSyntaxType4)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE5, OnDocumentSyntaxType5)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE6, OnDocumentSyntaxType6)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE7, OnDocumentSyntaxType7)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE8, OnDocumentSyntaxType8)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE9, OnDocumentSyntaxType9)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE10, OnDocumentSyntaxType10)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE11, OnDocumentSyntaxType11)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE12, OnDocumentSyntaxType12)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE13, OnDocumentSyntaxType13)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE14, OnDocumentSyntaxType14)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE15, OnDocumentSyntaxType15)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE16, OnDocumentSyntaxType16)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE17, OnDocumentSyntaxType17)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE18, OnDocumentSyntaxType18)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE19, OnDocumentSyntaxType19)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE20, OnDocumentSyntaxType20)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE21, OnDocumentSyntaxType21)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE22, OnDocumentSyntaxType22)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE23, OnDocumentSyntaxType23)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE24, OnDocumentSyntaxType24)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE25, OnDocumentSyntaxType25)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE26, OnDocumentSyntaxType26)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE27, OnDocumentSyntaxType27)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE28, OnDocumentSyntaxType28)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE29, OnDocumentSyntaxType29)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE30, OnDocumentSyntaxType30)
	ON_COMMAND(ID_DOCU_SYNTAX_TYPE31, OnDocumentSyntaxType31)
	ON_COMMAND(ID_DOCU_SYNTAX_AUTO, OnDocumentSyntaxAuto)
	ON_COMMAND(ID_DOCU_SYNTAX_TEXT, OnDocumentSyntaxText)
	ON_UPDATE_COMMAND_UI(ID_DOCU_FORMAT_DOS, OnUpdateDocumentFormatDos)
	ON_UPDATE_COMMAND_UI(ID_DOCU_FORMAT_UNIX, OnUpdateDocumentFormatUnix)
	ON_UPDATE_COMMAND_UI(ID_DOCU_FORMAT_MAC, OnUpdateDocumentFormatMac)
	ON_COMMAND(ID_DOCU_FORMAT_DOS, OnDocumentFormatDos)
	ON_COMMAND(ID_DOCU_FORMAT_UNIX, OnDocumentFormatUnix)
	ON_COMMAND(ID_DOCU_FORMAT_MAC, OnDocumentFormatMac)
	ON_UPDATE_COMMAND_UI(ID_DOCU_ENCODING_ASCII, OnUpdateDocumentEncodingAscii)
	ON_UPDATE_COMMAND_UI(ID_DOCU_ENCODING_UNICODE_LE, OnUpdateDocumentEncodingUnicodeLE)
	ON_UPDATE_COMMAND_UI(ID_DOCU_ENCODING_UNICODE_BE, OnUpdateDocumentEncodingUnicodeBE)
	ON_UPDATE_COMMAND_UI(ID_DOCU_ENCODING_UTF8_WBOM, OnUpdateDocumentEncodingUtf8WBOM)
	ON_UPDATE_COMMAND_UI(ID_DOCU_ENCODING_UTF8_XBOM, OnUpdateDocumentEncodingUtf8XBOM)
	ON_COMMAND(ID_DOCU_ENCODING_ASCII, OnDocumentEncodingAscii)
	ON_COMMAND(ID_DOCU_ENCODING_UNICODE_LE, OnDocumentEncodingUnicodeLE)
	ON_COMMAND(ID_DOCU_ENCODING_UNICODE_BE, OnDocumentEncodingUnicodeBE)
	ON_COMMAND(ID_DOCU_ENCODING_UTF8_WBOM, OnDocumentEncodingUtf8WBOM)
	ON_COMMAND(ID_DOCU_ENCODING_UTF8_XBOM, OnDocumentEncodingUtf8XBOM)
	ON_COMMAND(ID_DOCU_SUMMARY, OnDocumentSummary)
	ON_COMMAND(ID_DOCU_PROPERTIES, OnDocumentProperties)
	ON_UPDATE_COMMAND_UI(ID_DOCU_SUMMARY, OnUpdateDocumentSummary)
	ON_UPDATE_COMMAND_UI(ID_DOCU_PROPERTIES, OnUpdateDocumentProperties)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCedtDoc static member variables
BOOL CCedtDoc::m_bConvertTabsToSpacesBeforeSaving;
BOOL CCedtDoc::m_bRemoveTrailingSpacesBeforeSaving;

BOOL CCedtDoc::m_bSaveFilesInUnixFormat;
BOOL CCedtDoc::m_bSaveRemoteFilesInUnixFormat;

UINT CCedtDoc::m_nDefaultEncodingType;
UINT CCedtDoc::m_nDefaultFileFormat;

UINT CCedtDoc::m_nMakeBackupFile;
UINT CCedtDoc::m_nBackupMethod;

CString CCedtDoc::m_szBackupExtension;
CString CCedtDoc::m_szBackupDirectory;

CSyntaxType CCedtDoc::m_clsSyntaxTypes[MAX_SYNTAX_TYPE];

CDictionary CCedtDoc::m_clsDictionary;
BOOL CCedtDoc::m_bDictionaryLoaded = FALSE;

INT CCedtDoc::m_nCurrentFtpAccount = -1;
CString CCedtDoc::m_szCurrentRemotePathName = "";


/////////////////////////////////////////////////////////////////////////////
// CCedtDoc construction/destruction
CCedtDoc::CCedtDoc()
{
	m_szSavedCompositionString = "";
	m_bCompositionStringSaved = FALSE;
}

CCedtDoc::~CCedtDoc()
{
}


/////////////////////////////////////////////////////////////////////////////
// CCedtDoc diagnostics

#ifdef _DEBUG
void CCedtDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCedtDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Operations

BOOL CCedtDoc::IsModifiedOutside()
{
	CString szPathName = GetPathName();
	if( ! szPathName.GetLength() || IsRemoteFile() ) return FALSE;

	CFileStatus status;	
	if( ! CFile::GetStatus(szPathName, status) ) return FALSE;

	return( status.m_mtime != m_clsFileStatus.m_mtime );
}

void CCedtDoc::GoToLineNumber(INT nLineNumber)
{
	POSITION pos = GetFirstViewPosition();
	while( pos ) {
		CCedtView * pView = (CCedtView *)GetNextView( pos );
		pView->OnSearchGoTo(nLineNumber);
	}
}

void CCedtDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU) 
{
	m_strPathName = lpszPathName;
	ASSERT( ! m_strPathName.IsEmpty() );
	m_bEmbedded = FALSE;

	// set the document title based on path name
	if( ! IsRemoteFile() ) SetTitle( GetPathName() );
	else SetTitle( GetFullRemotePathName() );

	// add it to the file MRU list
	if( bAddToMRU && ! IsRemoteFile() )
		AfxGetApp()->AddToRecentFileList(lpszPathName);
}

CString CCedtDoc::GetFullRemotePathName() const
{
	CString szFullRemotePathName = CCedtApp::m_clsFtpAccounts[m_nFtpAccount].GetFullAccountName();
	szFullRemotePathName += m_szRemotePathName;
	return szFullRemotePathName;
}

/////////////////////////////////////////////////////////////////////////////
// CCedtDoc commands

BOOL CCedtDoc::OnNewDocument()
{
	m_nFtpAccount = -1;
	m_szRemotePathName = "";

	// This code segment replaces CDocument::OnNewDocument()
	if( IsModified() ) TRACE0("Warning: OnNewDocument replaces an unsaved document.\n");

	DeleteContents();
	m_strPathName.Empty();  // no path name yet
	SetModifiedFlag(FALSE); // make clean
	// End of code segment CDocument::OnNewDocument()

	m_bDocumentSaved = FALSE;
	m_nSavedUndoCount = 0;

	EmptyUndoBuffer();
	EmptyRedoBuffer();

	m_bRecordingUndo = TRUE;
	m_nRecordingCount = 0;

	// Set default encoding type and file format
	m_nEncodingType = m_nDefaultEncodingType;
	m_nFileFormat = m_nDefaultFileFormat;

	// Initialize file contents
	if( CCedtApp::m_bOpenMemText ) {
		POSITION pos = CCedtApp::m_clsMemText.GetHeadPosition();
		while( pos ) m_clsAnalyzedText.AddTail( CCedtApp::m_clsMemText.GetNext(pos) );
	} else if( CCedtApp::m_bOpenTemplate ) {
		m_clsAnalyzedText.FileLoad(CCedtApp::m_szOpenTemplatePathName, m_nEncodingType, m_nFileFormat);
	} else m_clsAnalyzedText.AddTail("");

	ZeroMemory( & m_clsFileStatus, sizeof(m_clsFileStatus) );
	m_dwFileAttribute = FILE_ATTRIBUTE_NORMAL;

	m_szLangSpecFile = ""; m_clsLangSpec.ResetContents(); 
	m_szKeywordsFile = ""; m_clsKeywords.ResetContents();
	m_bAutomaticSyntaxType = TRUE;

	if( CCedtApp::m_bOpenTemplate && DetectSyntaxType(CCedtApp::m_szOpenTemplatePathName, m_clsAnalyzedText.GetHead()) ) LoadSyntaxInformation();
	AnalyzeText();

	CCedtApp::m_bOpenTemplate = FALSE;
	CCedtApp::m_bOpenMemText = FALSE;

//	ReinitializeAllViews();
//	FormatScreenText();
//	UpdateAllViews(NULL);

	return TRUE;
}

BOOL CCedtDoc::OnReloadDocument(LPCTSTR lpszPathName, INT nEncodingType)
{
	if( ! strlen(lpszPathName) ) return FALSE;

//	m_nFtpAccount = m_nCurrentFtpAccount;
//	m_szRemotePathName = m_szCurrentRemotePathName;

	// This code segment imitate CDocument::OnOpenDocument(lpszPathName)
	if( IsModified() ) TRACE0("Warning: OnReloadDocument replaces an unsaved document.\n");

	CFileException fe;
	CFile * pFile = GetFile(lpszPathName, CFile::modeRead | CFile::shareDenyNone, &fe);
	if( pFile == NULL ) {
		ReportSaveLoadException(lpszPathName, &fe, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		return FALSE;
	}
	ReleaseFile(pFile, FALSE);

	DeleteContents();
	SetModifiedFlag(FALSE);
	// End of code segment CDocument::OnOpenDocument(lpszPathName)

	m_bDocumentSaved = FALSE;
	m_nSavedUndoCount = 0;

	EmptyUndoBuffer();
	EmptyRedoBuffer();

	m_bRecordingUndo = TRUE;
	m_nRecordingCount = 0;

	// Detect encoding type and file format
	DetectEncodingTypeAndFileFormat(lpszPathName, m_nEncodingType, m_nFileFormat);
	if( m_nEncodingType == ENCODING_TYPE_UNKNOWN ) m_nEncodingType = m_nDefaultEncodingType;
	if( m_nFileFormat == FILE_FORMAT_UNKNOWN ) m_nFileFormat = m_nDefaultFileFormat;

	// Check if encoding type is given
	if( nEncodingType != ENCODING_TYPE_UNKNOWN ) m_nEncodingType = nEncodingType;

	// Load file contents
	m_clsAnalyzedText.FileLoad(lpszPathName, m_nEncodingType, m_nFileFormat);

	CFile::GetStatus(lpszPathName, m_clsFileStatus);
	m_dwFileAttribute = GetFileAttributes(lpszPathName);

//	m_szLangSpecFileName = ""; m_clsLangSpec.Reset(); 
//	m_szKeywordsFileName = ""; m_clsKeywords.Reset();
//	m_bAutomaticSyntaxType = TRUE;

//	if( DetectSyntaxType(lpszPathName, m_clsAnalyzedText.GetHead()) ) LoadSyntaxInformation();
	AnalyzeText();

	ReinitializeAllViews();
//	FormatScreenText();
	UpdateAllViews(NULL);
	
	return TRUE;
}

BOOL CCedtDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	m_nFtpAccount = m_nCurrentFtpAccount;
	m_szRemotePathName = m_szCurrentRemotePathName;

	// make backup file
	if( m_nMakeBackupFile == 2 && ! IsRemoteFile() && VerifyFilePath(lpszPathName) ) {
		if( ! BackupDocument(lpszPathName) ) AfxMessageBox( IDS_ERR_FILE_BACKUP_FAILED );
	}

	// This code segment replaces CDocument::OnOpenDocument(lpszPathName)
	if( IsModified() ) TRACE0("Warning: OnOpenDocument replaces an unsaved document.\n");

	CFileException fe;
	CFile * pFile = GetFile(lpszPathName, CFile::modeRead | CFile::shareDenyNone, &fe);
	if( pFile == NULL ) {
		ReportSaveLoadException(lpszPathName, &fe, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		return FALSE;
	}
	ReleaseFile(pFile, FALSE);

	DeleteContents();
	SetModifiedFlag(FALSE);
	// End of code segment CDocument::OnOpenDocument(lpszPathName)

	m_bDocumentSaved = FALSE;
	m_nSavedUndoCount = 0;

	EmptyUndoBuffer();
	EmptyRedoBuffer();

	m_bRecordingUndo = TRUE;
	m_nRecordingCount = 0;

	// Detect encoding type and file format
	DetectEncodingTypeAndFileFormat(lpszPathName, m_nEncodingType, m_nFileFormat);
	if( m_nEncodingType == ENCODING_TYPE_UNKNOWN ) m_nEncodingType = m_nDefaultEncodingType;
	if( m_nFileFormat == FILE_FORMAT_UNKNOWN ) m_nFileFormat = m_nDefaultFileFormat;

	// Load file contents
	m_clsAnalyzedText.FileLoad(lpszPathName, m_nEncodingType, m_nFileFormat);

	CFile::GetStatus(lpszPathName, m_clsFileStatus);
	m_dwFileAttribute = GetFileAttributes(lpszPathName);

	m_szLangSpecFile = ""; m_clsLangSpec.ResetContents(); 
	m_szKeywordsFile = ""; m_clsKeywords.ResetContents();
	m_bAutomaticSyntaxType = TRUE;

	if( DetectSyntaxType(lpszPathName, m_clsAnalyzedText.GetHead()) ) LoadSyntaxInformation();
	AnalyzeText();

//	ReinitializeAllViews();
//	FormatScreenText();
//	UpdateAllViews(NULL);
	
	return TRUE;
}

BOOL CCedtDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// retrieve old path name
	CString szOldPathName = GetPathName();
	if( szOldPathName.IsEmpty() ) szOldPathName = GetTitle();

	// check if file can be written
	DWORD dwAttrib = 0x00000000;
	if( VerifyFilePath(lpszPathName) ) dwAttrib = GetFileAttributes(lpszPathName);

	if( dwAttrib == 0xFFFFFFFF ) { // GetFileAttributes() error
		INT nReturn = AfxMessageBox( IDS_ERR_FILE_ATTR_CHECK, MB_OKCANCEL );
		if( nReturn != IDOK ) return FALSE;
	} else if( (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) || (dwAttrib & FILE_ATTRIBUTE_READONLY) ) {
		CString szMessage; szMessage.Format( IDS_ERR_FILE_SAVE_DENIED1, lpszPathName );
		AfxMessageBox( szMessage, MB_OK | MB_ICONSTOP ); return FALSE;
	} else if( (dwAttrib & FILE_ATTRIBUTE_HIDDEN) || (dwAttrib & FILE_ATTRIBUTE_SYSTEM) ) {
		CString szMessage; szMessage.Format( IDS_ERR_FILE_SAVE_DENIED2, lpszPathName );
		AfxMessageBox( szMessage, MB_OK | MB_ICONSTOP ); return FALSE;
	}

	m_nFtpAccount = m_nCurrentFtpAccount;
	m_szRemotePathName = m_szCurrentRemotePathName;

	// make backup file
	if( m_nMakeBackupFile == 1 && VerifyFilePath(lpszPathName) ) {
		if( ! BackupDocument(lpszPathName) ) AfxMessageBox( IDS_ERR_FILE_BACKUP_FAILED );
	}

	// check if it need to remove trailing spaces
	if( m_bRemoveTrailingSpacesBeforeSaving ) {
		CCedtView * pView = (CCedtView *)GetFirstView();
		pView->SendMessage(WM_COMMAND, ID_EDIT_REMOVE_TRAILING_SPACES, 0L);
	}

	// check if it need to convert tabs to spaces
	if( m_bConvertTabsToSpacesBeforeSaving ) {
		CCedtView * pView = (CCedtView *)GetFirstView();
		pView->SendMessage(WM_COMMAND, ID_EDIT_CONVERT_TABS_TO_SPACES, 0L);
	}

	// This code segment replaces CDocument::OnSaveDocument(lpszPathName)
	CFileException fe;
	CFile * pFile = GetFile(lpszPathName, CFile::modeReadWrite | CFile::modeCreate | CFile::shareExclusive, &fe);
	if( pFile == NULL ) {
		ReportSaveLoadException(lpszPathName, &fe, TRUE, AFX_IDP_INVALID_FILENAME);
		return FALSE;
	}
	ReleaseFile(pFile, FALSE);

	SetModifiedFlag(FALSE); // back to unmodified
	// End of code segment CDocument::OnSaveDocument(lpszPathName)

	m_bDocumentSaved = TRUE;
	m_nSavedUndoCount = GetUndoBufferCount();

//	EmptyUndoBuffer(); - do not empty undo buffer
//	EmptyRedoBuffer(); - do not empty redo buffer

	m_bRecordingUndo = TRUE;
	m_nRecordingCount = 0;

	// Set file format if special option is checked
	if( IsRemoteFile() && m_bSaveRemoteFilesInUnixFormat ) m_nFileFormat = FILE_FORMAT_UNIX;
	else if( m_bSaveFilesInUnixFormat ) m_nFileFormat = FILE_FORMAT_UNIX;

	// Save file contents
	m_clsAnalyzedText.FileSave(lpszPathName, m_nEncodingType, m_nFileFormat);

	CFile::GetStatus(lpszPathName, m_clsFileStatus);
	m_dwFileAttribute = GetFileAttributes(lpszPathName);

	CString szOldExtension = GetFileExtension(szOldPathName);
	CString szNewExtension = GetFileExtension(lpszPathName);

	if( m_bAutomaticSyntaxType && szNewExtension != szOldExtension ) {
		m_szLangSpecFile = ""; m_clsLangSpec.ResetContents(); 
		m_szKeywordsFile = ""; m_clsKeywords.ResetContents();
		m_bAutomaticSyntaxType = TRUE;

		if( DetectSyntaxType(lpszPathName, m_clsAnalyzedText.GetHead()) ) LoadSyntaxInformation();
		AnalyzeText();

	//	ReinitializeAllViews();
		FormatScreenText();
		UpdateAllViews(NULL);
	} else {
	//	ReinitializeAllViews();
	//	FormatScreenText();
		UpdateAllViews(NULL);
	}

	return TRUE;
}

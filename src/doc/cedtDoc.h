// cedtDoc.h : interface of the CCedtDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CEDTDOC_H__FFCA2B8C_F9C5_11D4_A6F1_0050CE184C9B__INCLUDED_)
#define AFX_CEDTDOC_H__FFCA2B8C_F9C5_11D4_A6F1_0050CE184C9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CCedtDoc : public CDocument
{
protected: // create from serialization only
	CCedtDoc();
	DECLARE_DYNCREATE(CCedtDoc)

public: // Attributes
	static BOOL m_bConvertTabsToSpacesBeforeSaving;
	static BOOL m_bRemoveTrailingSpacesBeforeSaving;

	static BOOL m_bSaveFilesInUnixFormat;
	static BOOL m_bSaveRemoteFilesInUnixFormat;

	static UINT m_nDefaultEncodingType;
	static UINT m_nDefaultFileFormat;

public: // backup options
	static UINT m_nMakeBackupFile;
	static UINT m_nBackupMethod;

	static CString m_szBackupExtension;
	static CString m_szBackupDirectory;

public: // syntax types
	static CSyntaxType m_clsSyntaxTypes[MAX_SYNTAX_TYPE];

protected: // dictionary
	static CDictionary m_clsDictionary;
	static BOOL m_bDictionaryLoaded;

public: // file contents & status
	CAnalyzedText m_clsAnalyzedText;
	UINT m_nEncodingType; // ASCII/Unicode/UTF-8
	UINT m_nFileFormat;   // DOS/UNIX/MAC
	
	CFileStatus m_clsFileStatus;
	DWORD m_dwFileAttribute;

	BOOL m_bDocumentSaved;
	INT  m_nSavedUndoCount;

public: // shared remote path name
	static INT m_nCurrentFtpAccount;
	static CString m_szCurrentRemotePathName;

protected: // remote path name
	INT m_nFtpAccount;
	CString m_szRemotePathName;

public: // syntax type
	BOOL m_bAutomaticSyntaxType;
	CString m_szLangSpecFile;
	CString m_szKeywordsFile;

protected: // language specification
	CLangSpec m_clsLangSpec;
	CKeywords m_clsKeywords;

protected: // saved composition string
	CString m_szSavedCompositionString;
	BOOL m_bCompositionStringSaved;

protected: // undo & redo buffer
	CUndoBuffer m_clsUndoBuffer;
	CUndoBuffer m_clsRedoBuffer;

	BOOL m_bRecordingUndo;
	INT  m_nRecordingCount;


public: // *** cedtDoc.cpp ***
	BOOL IsModifiedOutside();
	void GoToLineNumber(INT nLineNumber);

	BOOL IsReadOnlyFile() const { return (m_dwFileAttribute & FILE_ATTRIBUTE_READONLY) ? TRUE : FALSE; }
	BOOL IsHiddenFile() const { return (m_dwFileAttribute & FILE_ATTRIBUTE_HIDDEN) ? TRUE : FALSE; }
	BOOL IsSystemFile() const { return (m_dwFileAttribute & FILE_ATTRIBUTE_SYSTEM) ? TRUE : FALSE; }

	BOOL IsRemoteFile() const { return (m_nFtpAccount >= 0) ? TRUE : FALSE; }
	BOOL IsNewFileNotSaved() const { return GetPathName().IsEmpty(); }

	BOOL HaveAnyOverflowLine() { return m_clsAnalyzedText.HaveAnyOverflowLine(); }
	BOOL MultiLineStringConstant() const { return m_clsLangSpec.m_bMultiLineStringConstant; }
	BOOL VariableHighlightInString() const { return m_clsLangSpec.m_bVariableHighlightInString; }

	BOOL HasLineCommentDelimiter() const { return m_clsLangSpec.m_szLineComment1.GetLength(); }
	BOOL HasBlockCommentDelimiter() const { return (m_clsLangSpec.m_szBlockComment1On.GetLength() && m_clsLangSpec.m_szBlockComment1Off.GetLength()); }

	INT GetFtpAccountNumber() const { return m_nFtpAccount; }
	CString GetRemotePathName() const { return m_szRemotePathName; }

	CString GetFullRemotePathName() const;
	LONG GetFileSize() const { return m_clsFileStatus.m_size; }

protected:
//	BOOL OnNewDocument();
	BOOL OnReloadDocument(LPCTSTR lpszPathName, INT nEncodingType);
//	BOOL OnOpenDocument(LPCTSTR lpszPathName);
//	BOOL OnSaveDocument(LPCTSTR lpszPathName);


public: // *** cedtDocView.cpp ***
	CView * GetFirstView();
	INT GetViewCount();

	void ReinitializeAllViews();
	void UpdateMDIFileTab();

	void FormatScreenText();
	void FormatScreenText(INT nIndex, INT nCount);

	void RemoveScreenText(INT nIndex, INT nCount);
	void InsertScreenText(INT nIndex, INT nCount);


public: // *** cedtDocFile.cpp ***
	BOOL FileSave();
	BOOL FileSaveAs();
	BOOL FileSaveAsRemote();
	BOOL FileReload(INT nEncodingType);

	BOOL SaveDocumentFile(LPCTSTR lpszPathName);
	BOOL SaveRemoteDocumentFile(INT nFtpAccount, LPCTSTR lpszPathName);

	BOOL ReloadDocumentFile(LPCTSTR lpszPathName, INT nLineNum, INT nEncodingType);
	BOOL ReloadRemoteDocumentFile(INT nFtpAccount, LPCTSTR lpszPathName, INT nLineNum, INT nEncodingType);

	BOOL UpdateFileStatus();
	BOOL BackupDocument(LPCTSTR lpszPathName);


public: // *** cedtDocSyntax.cpp ***
	BOOL DetectSyntaxType(LPCTSTR lpszPathName, LPCTSTR lpszFirstLine);
	BOOL SearchLinkFileWithFirstLine(LPCTSTR lpszFirstLine, CString & szLinkFilePath);
	BOOL SearchLinkFileWithPathName(LPCTSTR lpszPathName, CString & szLinkFilePath);
	BOOL GetContainsPartOfLinkFile(LPCTSTR lpszLinkFilePath, CString & szContains);
	BOOL ReadExtensionLinkFile(LPCTSTR lpszLinkFilePath);
	BOOL LoadSyntaxInformation();


public: // *** cedtDocDictionary.cpp ***
	static BOOL IsDictionaryLoaded() { return m_bDictionaryLoaded; }
	static BOOL LoadDictionary();
	static BOOL LoadingProgress(UINT nTotalRead);


public: // *** cedtDocAnal.cpp ***
	INT  GetCharType(TCHAR nChar);
	void AnalyzeText() { AnalyzeText(0, GetLineCount()); }
	void AnalyzeText(INT nIndex, INT nCount);


public: // *** cedtDocHndr.cpp ***
	void OnUpdateDocumentSyntaxType(CCmdUI * pCmdUI, INT nSyntaxType);
	void OnDocumentSyntaxType(INT nSyntaxType);

	void OnDocumentFileFormat(UINT nFileFormat);
	void OnDocumentEncodingType(UINT nEncodingType);


public: // *** cedtDocMap.cpp ***
	INT GetLineCount() { return m_clsAnalyzedText.GetCount(); }
	INT GetLastIdxY() { return m_clsAnalyzedText.GetCount()-1; }
	INT GetWordCount();
	INT GetByteCount();

	INT GetFirstIdxX(CAnalyzedString & rLine);
	INT GetLastIdxX(CAnalyzedString & rLine);
	INT GetFirstNonBlankIdxX(CAnalyzedString & rLine);
	INT GetTrailingBlankIdxX(CAnalyzedString & rLine);

	CAnalyzedString & GetLineFromIdxY(INT nIdxY);
	ANALYZEDWORD & GetWordFromIdxX(CAnalyzedString & rLine, INT nIdxX);

	BOOL IsBlankLine(CAnalyzedString & rLine);
	BOOL IsBlankLineFromIdxY(INT nIdxY);


public: // *** cedtDocEdit.cpp ***
	BOOL IsCompositionStringSaved() { return m_bCompositionStringSaved; }
	void EmptySavedCompositionString();
	void SaveCurrentCompositionString(INT nIdxY);
	void RestoreCurrentCompositionString(INT nIdxY);

	void InsertCompositionString(INT nIdxX, INT nIdxY, LPCTSTR lpszString);
	void DeleteCompositionString(INT nIdxX, INT nIdxY, INT nLength);

	void InsertChar(INT nIdxX, INT nIdxY, UINT nChar);
	void DeleteChar(INT nIdxX, INT nIdxY);
	void InsertString(INT nIdxX, INT nIdxY, LPCTSTR lpszString);
	void DeleteString(INT nIdxX, INT nIdxY, INT nLength);

	void InsertBlock(INT nBegX, INT nBegY, INT & nEndX, INT & nEndY, CMemText & rBlock);
	void DeleteBlock(INT nBegX, INT nBegY, INT nEndX, INT nEndY);
	void SplitLine(INT nIdxX, INT nIdxY);
	void JoinLines(INT nIdxX, INT nIdxY);

	void CopyToString(CString & rString, INT nBegX, INT nBegY, INT nLength);
	void CopyToBlock(CMemText & rBlock, INT nBegX, INT nBegY, INT nEndX, INT nEndY);

protected:
	void FastInsertChar(CAnalyzedString & rLine, INT nIdxX, INT nIdxY, UINT nChar);
	void FastDeleteChar(CAnalyzedString & rLine, INT nIdxX, INT nIdxY);
	void FastInsertString(CAnalyzedString & rLine, INT nIdxX, INT nIdxY, LPCTSTR lpszString);
	void FastDeleteString(CAnalyzedString & rLine, INT nIdxX, INT nIdxY, INT nLength);


public: // *** cedtDocEditAdv.cpp
	// return number of characters inserted or deleted
	INT  IndentLine(INT nIdxY, BOOL bUseTab, INT nIndentSize);
	INT  UnindentLine(INT nIdxY, INT nTabSize, INT nIndentSize);
	INT  MakeCommentLine(INT nIdxY);			INT  ReleaseCommentLine(INT nIdxY);

	// return number characters converted or deleted
	INT  ConvertTabsToSpaces(INT nIdxY);		INT  ConvertSpacesToTabs(INT nIdxY);
	INT  LeadingTabsToSpaces(INT nIdxY);		INT  LeadingSpacesToTabs(INT nIdxY);
	INT  DeleteLeadingSpaces(INT nIdxY);		INT  DeleteTrailingSpaces(INT nIdxY);

	void IndentBlock(INT nBegX, INT nBegY, INT nEndX, INT nEndY, BOOL bUseTab, INT nIndentSize);
	void UnindentBlock(INT nBegX, INT nBegY, INT nEndX, INT nEndY, INT nTabSize, INT nIndentSize);
	void MakeCommentBlock(INT nBegX, INT nBegY, INT nEndX, INT nEndY);
	void ReleaseCommentBlock(INT nBegX, INT nBegY, INT nEndX, INT nEndY);

	void ConvertTabsToSpacesDocument();			void ConvertSpacesToTabsDocument();
	void LeadingSpacesToTabsDocument();			void DeleteTrailingSpacesDocument();

protected:
	INT  FastIndentLine(CAnalyzedString & rLine, INT nIdxY, BOOL bUseTab, INT nIndentSize);
	INT  FastUnindentLine(CAnalyzedString & rLine, INT nIdxY, INT nTabSize, INT nIndentSize);
	INT  FastMakeCommentLine(CAnalyzedString & rLine, INT nIdxY);
	INT  FastReleaseCommentLine(CAnalyzedString & rLine, INT nIdxY);

	INT  FastConvertTabsToSpaces(CAnalyzedString & rLine, INT nIdxY);
	INT  FastConvertSpacesToTabs(CAnalyzedString & rLine, INT nIdxY);
	INT  FastLeadingTabsToSpaces(CAnalyzedString & rLine, INT nIdxY);
	INT  FastLeadingSpacesToTabs(CAnalyzedString & rLine, INT nIdxY);

	INT  FastDeleteLeadingSpaces(CAnalyzedString & rLine, INT nIdxY);
	INT  FastDeleteTrailingSpaces(CAnalyzedString & rLine, INT nIdxY);


public: // *** cedtDocUndo.cpp ***
	INT  GetUndoBufferCount() { return m_clsUndoBuffer.GetCount(); }
	INT  GetRedoBufferCount() { return m_clsRedoBuffer.GetCount(); }

	void EmptyUndoBuffer() { m_clsUndoBuffer.EmptyBuffer(); }
	void EmptyRedoBuffer() { m_clsRedoBuffer.EmptyBuffer(); }

	void GetLastEditingIndex(INT & nIdxX, INT & nIdxY) { m_clsUndoBuffer.GetRecentIndex(nIdxX, nIdxY); }
	void CheckIfAllActionsCanBeUndone() { if( m_nSavedUndoCount > GetUndoBufferCount() ) m_nSavedUndoCount = -1; }
	void CheckIfAllActionsAreUndone() { if( m_nSavedUndoCount == GetUndoBufferCount() ) SetModifiedFlag(FALSE); }

	void BeginActionRecording(BOOL bRecordingUndo);
	void EndActionRecording();

	void RecordInsertChar(INT nIdxX, INT nIdxY);
	void RecordDeleteChar(INT nIdxX, INT nIdxY, UINT nChar);
	void RecordInsertString(INT nIdxX, INT nIdxY, INT nLength);
	void RecordDeleteString(INT nIdxX, INT nIdxY, LPCTSTR lpszString);
	void RecordInsertBlock(INT nBegX, INT nBegY, INT nEndX, INT nEndY);
	void RecordDeleteBlock(INT nBegX, INT nBegY, CMemText & rBlock);
	void RecordSplitLine(INT nIdxX, INT nIdxY);
	void RecordJoinLines(INT nIdxX, INT nIdxY);

	INT PopUndoBuffer() { return m_clsUndoBuffer.RemoveHead(); }
	INT PopUndoAction() { return m_clsUndoBuffer.m_lstAction.RemoveHead(); }
	INT PopUndoIdxX() { return m_clsUndoBuffer.m_lstIdxX.RemoveHead(); }
	INT PopUndoIdxY() { return m_clsUndoBuffer.m_lstIdxY.RemoveHead(); }
	INT PopUndoParam() { return m_clsUndoBuffer.m_lstParam.RemoveHead(); }
	UINT PopUndoChar() { return m_clsUndoBuffer.m_lstChar.RemoveHead(); }
	CString PopUndoString() { return m_clsUndoBuffer.m_lstString.RemoveHead(); }
	CMemText PopUndoBlock() { return m_clsUndoBuffer.m_lstBlock.RemoveHead(); }

	INT PopRedoBuffer() { return m_clsRedoBuffer.RemoveHead(); }
	INT PopRedoAction() { return m_clsRedoBuffer.m_lstAction.RemoveHead(); }
	INT PopRedoIdxX() { return m_clsRedoBuffer.m_lstIdxX.RemoveHead(); }
	INT PopRedoIdxY() { return m_clsRedoBuffer.m_lstIdxY.RemoveHead(); }
	INT PopRedoParam() { return m_clsRedoBuffer.m_lstParam.RemoveHead(); }
	UINT PopRedoChar() { return m_clsRedoBuffer.m_lstChar.RemoveHead(); }
	CString PopRedoString() { return m_clsRedoBuffer.m_lstString.RemoveHead(); }
	CMemText PopRedoBlock() { return m_clsRedoBuffer.m_lstBlock.RemoveHead(); }


public: // *** cedtDocSearch.cpp ***
	BOOL OnePassFindString(INT & nIdxX, INT & nIdxY, LPCTSTR lpszFindString, UINT nOptions, CRegExp & clsRegExp);
	BOOL ForwardFindString(INT & nIdxX, INT & nIdxY, LPCTSTR lpszFindString, UINT nOptions, CRegExp & clsRegExp, BOOL * pSearchWrap);
	BOOL ReverseFindString(INT & nIdxX, INT & nIdxY, LPCTSTR lpszFindString, UINT nOptions, CRegExp & clsRegExp, BOOL * pSearchWrap);

	void ToggleBookmark(INT nIdxY);
	BOOL FindNextBookmark(INT & nIdxY);
	BOOL FindPrevBookmark(INT & nIdxY);

	BOOL IsThisIndentOnChar(INT nIdxX, INT nIdxY);
	BOOL IsThisIndentOffChar(INT nIdxX, INT nIdxY);

	BOOL ForwardFindEndingPair(INT & nIdxX, INT & nIdxY);
	BOOL ReverseFindBeginningPair(INT & nIdxX, INT & nIdxY);

	BOOL IsThisOneOfPairs(INT nIdxX, INT nIdxY, BOOL & bBeginning);
	BOOL FindAnotherOneOfPairs(INT & nIdxX, INT & nIdxY);

private:
	BOOL ForwardFindAnotherOneOfPairs(INT & nIdxX, INT & nIdxY);
	BOOL ReverseFindAnotherOneOfPairs(INT & nIdxX, INT & nIdxY);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCedtDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCedtDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CCedtDoc)
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileSaveAll();
	afx_msg void OnFileSaveAsRemote();
	afx_msg void OnFileReload();
	afx_msg void OnFileReloadAs();
	afx_msg void OnUpdateDocumentSyntaxAuto(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxText(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType0(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType1(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType2(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType3(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType4(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType5(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType6(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType7(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType8(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType9(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType10(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType11(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType12(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType13(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType14(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType15(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType16(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType17(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType18(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType19(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType20(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType21(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType22(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType23(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType24(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType25(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType26(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType27(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType28(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType29(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType30(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSyntaxType31(CCmdUI* pCmdUI);
	afx_msg void OnDocumentSyntaxType0();
	afx_msg void OnDocumentSyntaxType1();
	afx_msg void OnDocumentSyntaxType2();
	afx_msg void OnDocumentSyntaxType3();
	afx_msg void OnDocumentSyntaxType4();
	afx_msg void OnDocumentSyntaxType5();
	afx_msg void OnDocumentSyntaxType6();
	afx_msg void OnDocumentSyntaxType7();
	afx_msg void OnDocumentSyntaxType8();
	afx_msg void OnDocumentSyntaxType9();
	afx_msg void OnDocumentSyntaxType10();
	afx_msg void OnDocumentSyntaxType11();
	afx_msg void OnDocumentSyntaxType12();
	afx_msg void OnDocumentSyntaxType13();
	afx_msg void OnDocumentSyntaxType14();
	afx_msg void OnDocumentSyntaxType15();
	afx_msg void OnDocumentSyntaxType16();
	afx_msg void OnDocumentSyntaxType17();
	afx_msg void OnDocumentSyntaxType18();
	afx_msg void OnDocumentSyntaxType19();
	afx_msg void OnDocumentSyntaxType20();
	afx_msg void OnDocumentSyntaxType21();
	afx_msg void OnDocumentSyntaxType22();
	afx_msg void OnDocumentSyntaxType23();
	afx_msg void OnDocumentSyntaxType24();
	afx_msg void OnDocumentSyntaxType25();
	afx_msg void OnDocumentSyntaxType26();
	afx_msg void OnDocumentSyntaxType27();
	afx_msg void OnDocumentSyntaxType28();
	afx_msg void OnDocumentSyntaxType29();
	afx_msg void OnDocumentSyntaxType30();
	afx_msg void OnDocumentSyntaxType31();
	afx_msg void OnDocumentSyntaxAuto();
	afx_msg void OnDocumentSyntaxText();
	afx_msg void OnUpdateDocumentFormatDos(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentFormatUnix(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentFormatMac(CCmdUI* pCmdUI);
	afx_msg void OnDocumentFormatDos();
	afx_msg void OnDocumentFormatUnix();
	afx_msg void OnDocumentFormatMac();
	afx_msg void OnUpdateDocumentEncodingAscii(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentEncodingUnicodeLE(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentEncodingUnicodeBE(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentEncodingUtf8WBOM(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentEncodingUtf8XBOM(CCmdUI* pCmdUI);
	afx_msg void OnDocumentEncodingAscii();
	afx_msg void OnDocumentEncodingUnicodeLE();
	afx_msg void OnDocumentEncodingUnicodeBE();
	afx_msg void OnDocumentEncodingUtf8WBOM();
	afx_msg void OnDocumentEncodingUtf8XBOM();
	afx_msg void OnDocumentSummary();
	afx_msg void OnDocumentProperties();
	afx_msg void OnUpdateDocumentSummary(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentProperties(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CEDTDOC_H__FFCA2B8C_F9C5_11D4_A6F1_0050CE184C9B__INCLUDED_)

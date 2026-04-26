// cedtView.h : interface of the CCedtView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CEDTVIEW_H__FFCA2B8E_F9C5_11D4_A6F1_0050CE184C9B__INCLUDED_)
#define AFX_CEDTVIEW_H__FFCA2B8E_F9C5_11D4_A6F1_0050CE184C9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CCedtView : public CView
{
protected: // create from serialization only
	CCedtView();
	DECLARE_DYNCREATE(CCedtView)

public: // general preferences
	static COLORREF m_crBkgrColor[10], m_crTextColor[40];
	static LOGFONT  m_lfScreen[10], m_lfPrinter[10];
	static LOGFONT  m_lfMiscel[10]; // 0x00: Column Mode, 0x01: Output Window
	static INT  m_nCurrentScreenFont, m_nCurrentPrinterFont;
	static BOOL m_bUsingColumnModeFont;

	static BOOL m_bShowLineNumbers, m_bSyntaxHighlight;
	static BOOL m_bEmboldenKeywords, m_bItalicizeComment;
	static BOOL m_bHighlightActiveLine, m_bEnablePairsMatching;
	static BOOL m_bEnableAutoIndent, m_bShowSelectionMargin;

	static BOOL m_bHomeKeyGoesToFirstPosition, m_bSearchWrapAtEndOfFile;
	static BOOL m_bEnableDragAndDropEditing, m_bCopyCurrentLineIfNothingSelected;
	static BOOL m_bShowSpaces, m_bShowTabChars, m_bShowLineBreak;
	static BOOL m_bUseSpacesInPlaceOfTab, m_bOpenDocumentWordWrapped;

	static INT  m_nTabSize, m_nIndentationSize;
	static INT  m_nFixedWrapWidth, m_nWrapIndentation;
	static INT  m_nLineSpacing, m_nLineNumberDigits;
	static INT  m_nCaretWidthRatio, m_nFileTabLength;

	static BOOL m_bShowColumnMarker1, m_bShowColumnMarker2;
	static INT  m_nColumnMarker1Pos, m_nColumnMarker2Pos;

public: // printing preferences
	static RECT m_rectPageMargin; // in LOMETRIC
	static BOOL m_bPrintLineNumbers, m_bPrintSyntaxHighlight;
	static BOOL m_bPrintHeader, m_bPrintFooter;
	static CString m_szHeaderFormat[3], m_szFooterFormat[3];
	static CString m_szHeaderString[3], m_szFooterString[3];

public: // global view mode
	static BOOL m_bOverwriteMode;
	static BOOL m_bColumnMode;

protected: // search related variables
	static BOOL m_bReplaceSearch;
	static CString m_szFindString, m_szReplaceString;
	static UINT m_nSearchOptions; // WholeWord, MatchCase, RegExp
	static CRegExp m_clsRegExp;

public: // user commands
	static CUserCommand m_clsUserCommand[11];
	static CString m_szUserCommandFilePath[8];

protected: // user command input & output
	static BOOL m_bChildOutputLineContinue;
	static CStringArray m_arrChildInputString;

protected: // redirect handles
	static HANDLE m_hChildStdinWrDup, m_hChildStdoutRdDup;
	static HANDLE m_hChildProcess, m_hChildThread;

public: // user macros buffers
	static CMacroBuffer m_clsMacroBuffer[11];
	static CString m_szMacroBufferFilePath[8];

protected: // macro recording variables
	static BOOL m_bMacroRecording;
	static INT  m_nMacroRecordingNumber;
	static CString m_szMacroName;

protected: // global screen resources
	static CDC   m_dcScreen;
	static CBitmap m_bmScreen;
	static CFont m_fontScreen, m_fontScreenIt;
	static CFont m_fontPrinter, m_fontPrinterIt;
	static INT   m_nScreenFontChangeSequence;

public:
	static WINDOWPLACEMENT * m_lpWindowPlacement;

protected: // formated file contents
	CFormatedText m_clsFormatedScreenText;
	CFormatedText m_clsFormatedPrintText;
	INT  m_nFormatedPrintTextStartIdxY; // starting index of formated print text

protected: // view status
	BOOL   m_bHaveFocus, m_bAutoScroll, m_bLineSelect;
	CRect  m_rectNonScroll;		// non scroll boundary of view
	CSize  m_szPrevClientSize;	// to check text formatting is necessary
	CPoint m_ptRButtonUpPoint;	// to check if context menu is called from mouse

protected: // local display options
	BOOL m_bLocalWordWrap;
	BOOL m_bLocalSpellCheck;

protected: // scroll position & caret position
	INT  m_nScrollPosX, m_nScrollPosY;
	INT  m_nAnchorPosX, m_nAnchorPosY;
	INT  m_nCaretPosX,  m_nCaretPosY;
	INT  m_nCaretPosXFixed; // used to move up & down

protected: // block selection (copy & paste)
	BOOL m_bSaveSelected, m_bSelected;
	INT  m_nSaveSelBegX, m_nSaveSelBegY;
	INT  m_nSaveSelEndX, m_nSaveSelEndY;

protected: // saved index (save & restore caret and anchor pos)
	INT  m_nScrollIdxX, m_nScrollIdxY;
	INT  m_nAnchorIdxX, m_nAnchorIdxY;
	INT  m_nCaretIdxX,  m_nCaretIdxY;

protected: // active line highlight
	CDC  m_dcActiveLine; CBitmap m_bmActiveLine;
	BOOL m_bActiveLineHighlighted;
	INT  m_nActiveLineHighlightedPosY;

protected: // matching pairs highlight
	BOOL m_bPairMatched, m_bBeginningPair;
	INT  m_nPairPosX[2], m_nPairPosY[2], m_nPairWidth;
	BOOL m_bPairHighlighted[2];

protected: // IMM composition
	BOOL m_bComposition;

protected: // Drag And Drop
	COleDropTarget m_oleDropTarget;
	COleDataSource m_oleDataSource;

	BOOL m_bDragDataSource, m_bDragDropOccured;
	INT  m_nDragPosX, m_nDragPosY;
	INT  m_nDragObjectType;


public: // *** CCedtView.cpp ***
	static BOOL IsOverwriteMode() { return m_bOverwriteMode; }
	static BOOL IsColumnMode() { return m_bColumnMode; }

	static void OnSizeMainWindow(int cx, int cy);

	BOOL IsWordWrapOn() { return m_bLocalWordWrap; }
	BOOL IsSpellCheckOn() { return m_bLocalSpellCheck; }

//	void OnInitialUpdate();
	void Reinitialize();

	void UpdateScrollBars();
	void UpdateCaretPosition();
	void UpdateDragPosition();

	void UpdateFileStatusPane();
	void UpdateMDIFileTabOfTheDocument();

	BOOL TurnOffWordWrapMode();

protected:
	void UpdateAllPanesInTheSameRow();
	void UpdateAllPanesSharingScrollBar();

	void UpdateAllViews();

public:
	INT GetCompositionString(DWORD dwIndex, CString & szString);
	INT GetCurrentLineNumber();

	CString GetCurrentWord();
	CString GetSelectedString();

protected:
	void OnScreenFontChange();
	void OnTimerAutoScroll();


public: // *** cedtViewFont.cpp ***
	static BOOL IsFixedPitchScreenFont(INT nFont);

	static void CreateScreenFontObject();
	static void CreatePrinterFontObject(CDC * pDC);

	static CFont * ApplyCurrentScreenFont();
	static CFont * ApplyCurrentPrinterFont(CDC * pDC);



public: // *** cedtViewMetric.cpp ***
	static INT GetLogPixelsX(CDC * pDC = NULL);
	static INT GetLogPixelsY(CDC * pDC = NULL);

	static BOOL IsUsingFixedPitchFont(CDC * pDC = NULL);

protected:
	static INT GetCharHeight(CDC * pDC = NULL);
	static INT GetSpaceWidth(CDC * pDC = NULL);
	static INT GetNumberWidth(CDC * pDC = NULL);
	static INT GetAveCharWidth(CDC * pDC = NULL);
	static INT GetMaxCharWidth(CDC * pDC = NULL);

	static INT GetLeftMargin(CDC * pDC = NULL);
	static INT GetLineHeight(CDC * pDC = NULL);

	INT GetCharWidth();
	INT GetLinesPerPage();


public: // *** cedtViewScroll.cpp ***
	void SetScrollPosY(INT nPosY);
	void SetScrollPosX(INT nPosX);

protected:
	void SetScrollPosSyncAllPanes(INT nPosX, INT nPosY);
	void SetScrollPosToMakeCaretVisible();

	void SetScrollPosToMakeAutoScroll();
	void SetScrollPosToMakeDragScroll();

	void SetScrollPosToMakeCaretTop();
	void SetScrollPosToMakeCaretBottom();
	void SetScrollPosToMakeCaretCenter();


public: // *** cedtViewCaret.cpp ***
	BOOL IsSelected() { return m_bSelected; }
	INT  GetSelectedLineCount();

	void SaveCaretAndAnchorPos();
	void RestoreCaretAndAnchorPos();

	void GetSelectedPosition(INT & nBegX, INT & nBegY, INT & nEndX, INT & nEndY);
	void GetSelectedIndex(INT & nBegX, INT & nBegY, INT & nEndX, INT & nEndY);

protected:
	void SetCaretPosY(INT nPosY);				
	void SetCaretPosX(INT nPosX, BOOL bFix = TRUE);
	void SetCaretPosFromMouse(CPoint point);

	void SetDragPosY(INT nPosY);
	void SetDragPosX(INT nPosX);
	void SetDragPosFromMouse(CPoint point);

	BOOL IsCaretVisible();
	BOOL IsPointInSelection(CPoint point);
	BOOL IsPositionInSelection(INT nPosX, INT nPosY);

	INT  RelativePosToSavedSelection(INT nPosX, INT nPosY);
	void AdjustCaretAndAnchorPosRelativeToSavedSelection();
	void AdjustCaretPosAfterLineSelectMouseMove();


public: // *** cedtViewMisc.cpp ***
	static void SetClipboardData(CMemText & rBlock);
	static BOOL GetClipboardData(CMemText & rBlock);
	static BOOL HasClipboardData();

	static BOOL GetOleDataObjectData(COleDataObject * pDataObject, CMemText & rBlock);

	static BOOL GetGlobalMemoryText(HGLOBAL hMemory, CMemText & rBlock);
	static HGLOBAL SetGlobalMemoryText(CMemText & rBlock);

protected:
	BOOL MultiLineStringConstant();
	BOOL VariableHighlightInString();

	BOOL HasLineCommentDelimiter();
	BOOL HasBlockCommentDelimiter();

	static UINT GetKeyState();

	static CString GetCurrentDate();
	static CString GetCurrentTime();


protected: // *** cedtViewDraw.cpp ***
	void DrawScreenBackgroundAndText();
	void InvertScreenSelected();

	void DrawPrintPageOutline(CDC * pDC, RECT rectDraw, INT nCurPage);
	void DrawPrintPageBackgroundAndText(CDC * pDC, RECT rectDraw, INT nCurPage);

	void DrawActiveLineHighlight();
	void DrawColumnMarkerHighlight();

	void DrawScreenBookmark(INT nPosX, INT nPosY, INT nWidth, INT nHeight);
	void DrawScreenUnderbar(INT nPosX, INT nPosY, INT nWidth, INT nHeight);
	void DrawScreenSpaces(INT nPosX, INT nPosY, INT nWidth, INT nHeight, INT nCount);
	void DrawScreenTabChar(INT nPosX, INT nPosY, INT nWidth, INT nHeight);
	void DrawScreenLineBreak(INT nPosX, INT nPosY, INT nWidth, INT nHeight);

	void ParsePageHeaderAndFooter(INT nCurPage, INT nMaxPage);
	void ParsePageHeaderAndFooter(CString & szArgument, LPCTSTR lpszFilePath, LPCTSTR lpszFileName,
		 LPCTSTR lpszPageNumber, LPCTSTR lpszTotalPage, LPCTSTR lpszCurrDate, LPCTSTR lpszCurrTime);


protected: // *** cedtViewHighlight.cpp ***
	void HighlightActiveLine();
	void UnhighlightActiveLine();
	void HighlightColumnMarker();

	void HighlightMatchingPairs();
	void UnhighlightPairs();


public: // *** cedtViewFormat.cpp ***
	void FormatScreenText();
	void FormatScreenText(INT nIndex, INT nCount);

	void FormatPrintText(CDC * pDC, RECT rectDraw);
	void FormatPrintText(CDC * pDC, RECT rectDraw, INT nIndex, INT nCount);

	void RemoveScreenText(INT nIndex, INT nCount);
	void InsertScreenText(INT nIndex, INT nCount);

protected:
	INT   GetWordWidth(LPCTSTR lpWord, SHORT siLength, INT nPosition, UCHAR ucType, CDC * pDC = NULL);
	SHORT GetWordIndex(LPCTSTR lpWord, SHORT siLength, INT nWidth, CDC * pDC = NULL);

	POSITION FindScreenTextIndex(INT nIndex);
	POSITION FlattenScreenTextAt(POSITION pos);
	POSITION RemoveScreenTextAt(POSITION pos);


protected: // *** cedtViewMap.cpp ***
	INT GetLastIdxY();
	INT GetLastPosY();

	INT GetFirstIdxX(CAnalyzedString & rLine);
	INT GetFirstIdxX(CFormatedString & rLine);
	INT GetLastIdxX(CAnalyzedString & rLine);
	INT GetLastIdxX(CFormatedString & rLine);
	INT GetFirstNonBlankIdxX(CAnalyzedString & rLine);
	INT GetTrailingBlankIdxX(CAnalyzedString & rLine);

	INT GetNextTabPosition(INT nPosX);			
	INT GetPrevTabPosition(INT nPosX);
	INT GetFirstPosX(CFormatedString & rLine);
	INT GetLastPosX(CFormatedString & rLine);
	INT GetFirstNonBlankPosX(CFormatedString & rLine);
	INT GetTrailingBlankPosX(CFormatedString & rLine);

	CAnalyzedString & GetLineFromIdxY(INT nIdxY);
	CFormatedString & GetLineFromPosY(INT nPosY);

	ANALYZEDWORD & GetWordFromIdxX(CAnalyzedString & rLine, INT nIdxX);
	FORMATEDWORD & GetWordFromPosX(CFormatedString & rLine, INT nPosX);
	FORMATEDWORD & GetWordFromIdxX(CFormatedString & rLine, INT nIdxX);

	INT GetIdxYFromPosY(INT nPosY);
	INT GetIdxXFromPosX(CFormatedString & rLine, INT nPosX, BOOL bAdjust = TRUE);
	INT GetIdxXFromPosX(CFormatedString & rLine, FORMATEDWORD & rWord, INT nPosX, BOOL bAdjust = TRUE);

	INT GetPosYFromIdxY(INT nIdxX, INT nIdxY, BOOL bAdjust = TRUE);
	INT GetPosXFromIdxX(CFormatedString & rLine, INT nIdxX, BOOL bAdjust = TRUE);
	INT GetPosXFromIdxX(CFormatedString & rLine, FORMATEDWORD & rWord, INT nIdxX, BOOL bAdjust = TRUE);

	BOOL IsBlankLine(CAnalyzedString & rLine);
	BOOL IsBlankLineFromIdxY(INT nIdxY);

	BOOL IsBlankLine(CFormatedString & rLine);
	BOOL IsBlankLineFromPosY(INT nPosY);


public: // *** cedtViewMapAdv.cpp ***
	void PositionToIndex(INT nPosX, INT nPosY, INT & nIdxX, INT & nIdxY);
	void IndexToPosition(INT nIdxX, INT nIdxY, INT & nPosX, INT & nPosY);

	TCHAR GetCharFromPosX(CFormatedString & rLine, INT nPosX);
	TCHAR GetCharFromIdxX(CFormatedString & rLine, INT nIdxX);

	INT GetNextSegmentIdxX(CFormatedString & rLine, INT nIdxX);
	INT GetPrevSegmentIdxX(CFormatedString & rLine, INT nIdxX);

	INT GetPrevWordIdxX(CFormatedString & rLine, INT nIdxX);
	INT GetNextWordIdxX(CFormatedString & rLine, INT nIdxX);
	INT GetCurrWordRange(CFormatedString & rLine, INT nIdxX, INT & nBegX, INT & nEndX);


protected: // *** cedtViewHndrEdit.cpp ***
	void OnMoveKeyDown(UINT nChar, UINT nFlags);
	void OnCharKeyDown(UINT nChar);
	void OnDBCharKeyDown(UINT nCH1, UINT nCH2);

	void OnImeCompositionStart();
	void OnImeCompositionEnd(BOOL bRedraw = TRUE);
	void OnImeCompositionCompose(LPCTSTR lpszString);
	void OnImeCompositionResult(LPCTSTR lpszString);

	void OnDragCancel();
	void OnDragAndDrop(HGLOBAL hMemory);
	void OnDragDelete();
	void OnDropCancel();
	void OnDropPaste(HGLOBAL hMemory);

public:
	void OnCommandExecute(INT nCommand);
	void OnMacroReplay(INT nMacro);
	void OnSearchGoTo(INT nLineNumber);


protected: // *** cedtViewHndrMisc.cpp ***
	void OnUpdateCommandExecute(INT nCommand, CCmdUI * pCmdUI);
	void OnUpdateMacroReplay(INT nMacro, CCmdUI * pCmdUI);


protected: // *** cedtViewEvnt.cpp ***
	BOOL EventMoveKeyDown(UINT nChar, UINT nFlags, BOOL bMacro);

	void EventInsertChar(UINT nChar, BOOL bMacro);
	void EventInsertString(LPCTSTR lpszString, BOOL bMacro);
	void EventInsertFile(LPCTSTR lpszPathName, BOOL bMacro);

	void EventCompositionStart(BOOL bMacro);
	void EventCompositionEnd(BOOL bMacro);
	void EventCompositionCompose(LPCTSTR lpszString, BOOL bMacro);
	void EventCompositionResult(LPCTSTR lpszString, BOOL bMacro);

	void EventCommandEscape(BOOL bMacro);		void EventCommandReturn(BOOL bMacro);
	void EventCommandBack(BOOL bMacro);			void EventCommandDelete(BOOL bMacro);
	void EventCommandTab(BOOL bMacro);			void EventCommandDetab(BOOL bMacro);

	void EventIncreaseIndent(BOOL bMacro);		void EventDecreaseIndent(BOOL bMacro);
	void EventMakeComment(BOOL bMacro);			void EventReleaseComment(BOOL bMacro);
	void EventJoinLines(BOOL bMacro);			void EventSplitLine(BOOL bMacro);

	void EventDeleteWord(BOOL bMacro);			void EventDeletePrevWord(BOOL bMacro);
	void EventDeleteToEndOfLine(BOOL bMacro);	void EventDeleteToBeginOfLine(BOOL bMacro);
	void EventDeleteLine(BOOL bMacro);			void EventDuplicateLine(BOOL bMacro);

	void EventCommandCut(BOOL bMacro);			void EventCommandCopy(BOOL bMacro);
	void EventCommandCopyFilePath(BOOL bMacro);
	void EventCommandCutAppend(BOOL bMacro);	void EventCommandCopyAppend(BOOL bMacro);
	void EventCommandPaste(BOOL bMacro);

	void EventDragCancel(BOOL bMacro);
	void EventDragDelete(BOOL bMacro);
	void EventDragAdjust(BOOL bMacro);
	void EventDropCancel(BOOL bMacro);
	void EventDropPaste(HGLOBAL hMemory, BOOL bMacro);

	void EventSelectAll(BOOL bMacro);			void EventSelectLine(BOOL bMacro);
	void EventSelectWord(BOOL bMacro);			void EventSelectBlock(BOOL bMacro);

	void EventUpperCase(BOOL bMacro);			void EventLowerCase(BOOL bMacro);
	void EventCapitalize(BOOL bMacro);			void EventInvertCase(BOOL bMacro);

	void EventConvertTabsToSpaces(BOOL bMacro);	void EventConvertSpacesToTabs(BOOL bMacro);
	void EventLeadingSpacesToTabs(BOOL bMacro);	void EventRemoveTrailingSpaces(BOOL bMacro);

	void EventUndoLastAction(BOOL bMacro);		void EventRedoLastUndo(BOOL bMacro);
	void EventEvaluateLine(BOOL bMacro);		void EventReplayMacro(INT nMacro, INT nTimes, BOOL bMacro);

	BOOL EventFindString(LPCTSTR lpszFindString, UINT nOptions, BOOL bMacro);
	BOOL EventReplaceString(LPCTSTR lpszFindString, LPCTSTR lpszReplaceString, UINT nOptions, BOOL bMacro);

	BOOL EventFindCurrentString(BOOL bMacro);
	BOOL EventFindSelectedString(BOOL bMacro);

	BOOL EventSearchNextOccurrence(BOOL bMacro);			
	BOOL EventSearchPrevOccurrence(BOOL bMacro);

	INT  EventReplaceThisOccurrence(BOOL bMacro);
	INT  EventReplaceAllInSelection(BOOL bMacro);
	INT  EventReplaceAllInFile(BOOL bMacro);

	BOOL EventGoToLine(INT nLineNumber, BOOL bMacro);
	BOOL EventToggleBookmark(BOOL bMacro);
	BOOL EventNextBookmark(BOOL bMacro);
	BOOL EventPrevBookmark(BOOL bMacro);

	BOOL EventPrevEditingPosition(BOOL bMacro);
	BOOL EventPairsBeginPosition(UINT nFlags, BOOL bMacro);
	BOOL EventPairsEndPosition(UINT nFlags, BOOL bMacro);


protected: // *** cedtViewAction.cpp ***
	void ActionWrongOperation(BOOL bBeep = TRUE);
	void ActionEvaluateLine();


protected: // *** cedtViewMove.cpp ***
	BOOL ActionLeftKey(UINT nFlags);			BOOL ActionRightKey(UINT nFlags);
	BOOL ActionUpKey(UINT nFlags);				BOOL ActionDownKey(UINT nFlags);
	BOOL ActionHomeKey(UINT nFlags);			BOOL ActionEndKey(UINT nFlags);
	BOOL ActionPriorKey(UINT nFlags);			BOOL ActionNextKey(UINT nFlags);

protected:
	void MoveCaretLeft();						void MoveCaretRight();
	void MoveCaretWordLeft();					void MoveCaretWordRight();
	void MoveCaretLineBegin();					void MoveCaretLineEnd();
	void MoveCaretLineBeginNonBlank();
	void MoveCaretParaBegin();					void MoveCaretParaEnd();

	void MoveCaretUp();							void MoveCaretDown();
	void MoveCaretScreenTop();					void MoveCaretScreenBottom();
	void MoveCaretPageUp();						void MoveCaretPageDown();
	void MoveCaretHalfPageUp();					void MoveCaretHalfPageDown();

	void MoveCaretDocumentBegin();				void MoveCaretDocumentEnd();
	void MoveCaretScreenBegin();				void MoveCaretScreenEnd();
	void ScrollScreenUp();						void ScrollScreenDown();
	void ScrollScreenUpLimited();				void ScrollScreenDownLimited();


protected: // *** cedtViewSelect.cpp ***
	BOOL ActionSelectAll();						BOOL ActionSelectLine();
	BOOL ActionSelectWord();					BOOL ActionSelectBlock();


protected: // *** cedtViewEdit.cpp ***
	void ActionInsertChar(UINT nChar);			void ActionInsertColumnChar(UINT nChar);
	void ActionInsertSpacesInPlaceOfTab();		void ActionInsertColumnSpacesInPlaceOfTab();
	void ActionInsertString(LPCTSTR lpszString);

	void ActionCarrigeReturn();					void ActionBackspace();
	void ActionDeleteChar();					void ActionDetabCaret();

	void ActionJoinLines();						void ActionSplitLine();
	void ActionDeleteWord();					void ActionDeletePrevWord();
	void ActionDeleteToEndOfLine();				void ActionDeleteToBeginOfLine();
	void ActionDeleteLine();					void ActionDuplicateLine();

	void ActionIndentLine();					void ActionUnindentLine();
	void ActionMakeCommentLine();				void ActionReleaseCommentLine();

protected:
	void InsertChar(INT nIdxX, INT nIdxY, UINT nChar);
	void DeleteChar(INT nIdxX, INT nIdxY);

	void CopyToString(CString & rString, INT nIdxX, INT nIdxY, INT nLength);
	void InsertString(INT nIdxX, INT nIdxY, LPCTSTR lpszString);
	void DeleteString(INT nIdxX, INT nIdxY, INT nLength);

	void SplitLine(INT nIdxX, INT nIdxY);
	void JoinLines(INT nIdxX, INT nIdxY);

protected:
	INT  IndentLine(INT nIdxY);					INT  UnindentLine(INT nIdxY);
	INT  MakeCommentLine(INT nIdxY);			INT  ReleaseCommentLine(INT nIdxY);

	INT  ConvertTabsToSpaces(INT nIdxY);		INT  ConvertSpacesToTabs(INT nIdxY);
	INT  LeadingSpacesToTabs(INT nIdxY);
	INT  DeleteLeadingSpaces(INT nIdxY);		INT  DeleteTrailingSpaces(INT nIdxY);


protected: // *** cedtViewEditCompose.cpp ***
	void ActionCompositionStart();
	void ActionCompositionEnd();
	void ActionCompositionCompose(LPCTSTR lpszString);
	void ActionCompositionResult(LPCTSTR lpszString);

protected:
	BOOL IsCompositionStringSaved();
	void EmptySavedCompositionString();
	void SaveCurrentCompositionString(INT nIdxY);
	void RestoreCurrentCompositionString(INT nIdxY);

	void InsertCompositionString(INT nIdxX, INT nIdxY, LPCTSTR lpszString);
	void DeleteCompositionString(INT nIdxX, INT nIdxY, INT nLength);


protected: // *** cedtViewEditAdv.cpp ***
	void ActionDeleteLineSelection();			void ActionDeleteColumnSelection();
	void ActionDeleteColumnChar();				void ActionDeleteColumnPrevChar();
	void ActionDeleteColumnToEndOfLine();		void ActionDeleteColumnToBeginOfLine();

	void ActionCopyLineSelection(CMemText & rBlock);
	void ActionCopyColumnSelection(CMemText & rBlock);
	void ActionCopyLine(CMemText & rBlock);
	void ActionCopyFilePath(CMemText & rBlock);

	void ActionPasteLineSelection(CMemText & rBlock);
	void ActionPasteColumnSelection(CMemText & rBlock);

	void ActionChangeLineSelection(CMemText & rBlock);
	void ActionChangeColumnSelection(CMemText & rBlock);

	void ActionIndentLineSelection();			void ActionUnindentLineSelection();
	void ActionMakeCommentLineSelection();		void ActionReleaseCommentLineSelection();

	void ActionConvertTabsToSpaces();			void ActionConvertSpacesToTabs();
	void ActionLeadingSpacesToTabs();			void ActionRemoveTrailingSpaces();

protected:
	void CopyToLineSelection(CMemText & rBlock, INT nBegX, INT nBegY, INT nEndX, INT nEndY);
	void InsertLineSelection(INT nBegX, INT nBegY, INT & nEndX, INT & nEndY, CMemText & rBlock);
	void DeleteLineSelection(INT nBegX, INT nBegY, INT nEndX, INT nEndY);

	void ArrangeLineSelection(INT & nBegX, INT & nBegY, INT & nEndX, INT & nEndY);
	void IndentLineSelection(INT nBegX, INT nBegY, INT nEndX, INT nEndY);
	void UnindentLineSelection(INT nBegX, INT nBegY, INT nEndX, INT nEndY);
	void MakeCommentLineSelection(INT nBegX, INT nBegY, INT nEndX, INT nEndY);
	void ReleaseCommentLineSelection(INT nBegX, INT nBegY, INT nEndX, INT nEndY);

	void CopyToColumnSelection(CMemText & rBlock, INT nBegX, INT nBegY, INT nEndX, INT nEndY);
	void InsertColumnSelection(INT nBegX, INT nBegY, INT & nEndX, INT & nEndY, CMemText & rBlock);
	void DeleteColumnSelection(INT nBegX, INT nBegY, INT nEndX, INT nEndY);

	void ConvertTabsToSpacesDocument();			void ConvertSpacesToTabsDocument();
	void LeadingSpacesToTabsDocument();			void DeleteTrailingSpacesDocument();


protected: // *** cedtViewUndo.cpp ***
	void ActionUndoLastAction();				void ActionRedoLastUndo();

protected:
	INT  GetUndoBufferCount();					INT  GetRedoBufferCount();
	void EmptyUndoBuffer();						void EmptyRedoBuffer();

	void GetLastEditingIndex(INT & nIdxX, INT & nIdxY);
	void CheckIfAllActionsCanBeUndone();
	void CheckIfAllActionsAreUndone();

	void BeginActionRecording(BOOL bRecordingUndo);
	void EndActionRecording();


protected: // *** cedtViewSearch.cpp ***
	BOOL ActionForwardFindString(LPCTSTR lpszFindString, UINT nOptions, CRegExp & clsRegExp, BOOL * pSearchWrap);
	BOOL ActionReverseFindString(LPCTSTR lpszFindString, UINT nOptions, CRegExp & clsRegExp, BOOL * pSearchWrap);

	INT  ActionReplaceAllInSelection(LPCTSTR lpszFindString, LPCTSTR lpszReplaceString, UINT nOptions, CRegExp & clsRegExp);
	INT  ActionReplaceAllInFile(LPCTSTR lpszFindString, LPCTSTR lpszReplaceString, UINT nOptions, CRegExp & clsRegExp);
	INT  ActionReplaceThisOccurrence(LPCTSTR lpszReplaceString, UINT nOptions, CRegExp & clsRegExp);

	BOOL ActionGoToLine(INT nIdxY);
	BOOL ActionToggleBookmark();
	BOOL ActionNextBookmark();
	BOOL ActionPrevBookmark();

	BOOL ActionPrevEditingPosition();
	BOOL ActionPairsBeginPosition();
	BOOL ActionPairsEndPosition();

protected:
	BOOL OnePassFindString(INT & nIdxX, INT & nIdxY, LPCTSTR lpszFindString, UINT nOptions, CRegExp & clsRegExp);
	BOOL ForwardFindString(INT & nIdxX, INT & nIdxY, LPCTSTR lpszFindString, UINT nOptions, CRegExp & clsRegExp, BOOL * pSearchWrap);
	BOOL ReverseFindString(INT & nIdxX, INT & nIdxY, LPCTSTR lpszFindString, UINT nOptions, CRegExp & clsRegExp, BOOL * pSearchWrap);

	INT  ReplaceAllInSelection(INT & nBegX, INT & nBegY, INT & nEndX, INT & nEndY, LPCTSTR lpszFindString, LPCTSTR lpszReplaceString, UINT nOptions, CRegExp & clsRegExp);
	INT  ReplaceAllInFile(LPCTSTR lpszFindString, LPCTSTR lpszReplaceString, UINT nOptions, CRegExp & clsRegExp);
	INT  ReplaceThisOccurrence(INT & nBegX, INT & nBegY, INT & nEndX, INT & nEndY, LPCTSTR lpszReplaceString, UINT nOptions, CRegExp & clsRegExp);

	void ToggleBookmark(INT nIdxY);
	BOOL FindNextBookmark(INT & nIdxY);
	BOOL FindPrevBookmark(INT & nIdxY);

	BOOL IsThisIndentOnChar(INT nIdxX, INT nIdxY);
	BOOL IsThisIndentOffChar(INT nIdxX, INT nIdxY);

	BOOL ForwardFindEndingPair(INT & nIdxX, INT & nIdxY);
	BOOL ReverseFindBeginningPair(INT & nIdxX, INT & nIdxY);

	BOOL IsThisOneOfPairs(INT nIdxX, INT nIdxY, BOOL & bBeginning);
	BOOL FindAnotherOneOfPairs(INT & nIdxX, INT & nIdxY);

	BOOL IsStringInSelection(INT nIdxX, INT nIdxY, INT nLength, INT nBegX, INT nBegY, INT nEndX, INT nEndY);


public: // *** cedtViewCommand.cpp ***
	static BOOL IsCommandRunningInCaptureOutputMode() { return (m_hChildStdinWrDup || m_hChildStdoutRdDup); }
	static BOOL RefreshUserCommandFilePathForMenu();

	static CString GetCommandName(INT nCommand);
	static CString GetCommandHotKeyText(INT nCommand);

	static INT  IsCommandAssigned(INT nCommand) { return m_clsUserCommand[nCommand].IsAssigned(); }
	static void DeleteCommand(INT nCommand) { m_clsUserCommand[nCommand].DeleteContents(); }

	static BOOL SetChildInputString(LPCTSTR lpszInputString);
	static BOOL KillChildProcess();

protected:
	BOOL TranslateMessageForUserCommand(MSG * pMsg);
	BOOL ExecuteCommand(INT nCommand);

	BOOL AskUserInputArguments(CString & szUserInput);
	BOOL AskUserSelectFilePath(LPCTSTR lpszPathName, CString & szSelectPath);
	BOOL AskUserSelectDirectory(LPCTSTR lpszPathName, CString & szSelectDirectory);

	BOOL SetDefaultShellVariables(CMapStringToString & clsVariables, LPCTSTR lpszPathName, LPCTSTR lpszProjectPath, 
		 LPCTSTR lpszLineNum, LPCTSTR lpszCurrWord, LPCTSTR lpszUserInput, LPCTSTR lpszSelPath1, LPCTSTR lpszSelPath2, LPCTSTR lpszSelDir1, LPCTSTR lpszSelDir2);
	BOOL ReplaceShellVariables(CString & szArgument, CMapStringToString & clsVariables);
	BOOL ExpandShellVariable(CString & szVariable, LPCTSTR lpszExpand);

	BOOL ExecuteWinHelp(LPCTSTR lpszCommand, LPCTSTR lpszArgument);
	BOOL ExecuteHtmlHelp(LPCTSTR lpszCommand, LPCTSTR lpszArgument);
	BOOL ExecuteExecutable(LPCTSTR lpszCommand, LPCTSTR lpszArgument, LPCTSTR lpszDirectory, BOOL bCloseOnExit, BOOL bCaptureOutput);
	void OnTimerCaptureOutput();


public: // *** cedtViewMacro.cpp ***
	static BOOL RefreshMacroBufferFilePathForMenu();

	static CString GetMacroName(INT nMacro);
	static CString GetMacroHotKeyText(INT nMacro);

	static INT  GetMacroBufferCount(INT nMacro) { return m_clsMacroBuffer[nMacro].GetBufferCount(); }
	static void EmptyMacroBuffer(INT nMacro) { m_clsMacroBuffer[nMacro].DeleteContents(); }

protected:
	BOOL TranslateMessageForMacroBuffer(MSG * pMsg);
	void ActionReplayMacro(INT nMacro, INT nTimes);

	void BeginMacroRecording(INT nMacroRecordingNumber, LPCTSTR lpszMacroName);
	void EndMacroRecording();
	void CancelMacroRecording();
	BOOL IsMacroRecording() { return m_bMacroRecording; }

	void MacroRecordMove(UINT nChar, UINT nFlags);

	void MacroRecordChar(UINT nChar);
	void MacroRecordString(LPCTSTR lpszString);
	void MacroRecordFile(LPCTSTR lpszPathName);

	void MacroRecordFind(LPCTSTR lpszFindString, UINT nOptions);
	void MacroRecordReplace(LPCTSTR lpszFindString, LPCTSTR lpszReplaceString, UINT nOptions);
	void MacroRecordGoTo(UINT nLineNumber);

	void MacroRecordCommand(UINT nCommand);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCedtView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCedtView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CCedtView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnEditReturn();
	afx_msg void OnEditBack();
	afx_msg void OnEditEscape();
	afx_msg void OnEditDelete();
	afx_msg void OnEditSelectAll();
	afx_msg void OnEditSelectLine();
	afx_msg void OnEditSelectWord();
	afx_msg void OnViewWordWrap();
	afx_msg void OnUpdateViewWordWrap(CCmdUI* pCmdUI);
	afx_msg void OnEditInsertDate();
	afx_msg void OnEditInsertTime();
	afx_msg void OnEditInsertFile();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnEditCutAppend();
	afx_msg void OnEditCopyAppend();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditTab();
	afx_msg void OnEditDetab();
	afx_msg void OnEditJoinLines();
	afx_msg void OnEditSplitLine();
	afx_msg void OnEditDeleteLine();
	afx_msg void OnEditDuplicateLine();
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnSearchFind();
	afx_msg void OnSearchReplace();
	afx_msg void OnSearchGoTo();
	afx_msg void OnSearchAskReplace();
	afx_msg void OnSearchToggleBookmark();
	afx_msg void OnSearchNextBookmark();
	afx_msg void OnSearchPrevBookmark();
	afx_msg void OnMacroToggleRecording();
	afx_msg void OnMacroBeginRecording();
	afx_msg void OnMacroEndRecording();
	afx_msg void OnUpdateMacroBeginRecording(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMacroEndRecording(CCmdUI* pCmdUI);
	afx_msg void OnMacroReplay();
	afx_msg void OnMacroReplay1();
	afx_msg void OnMacroReplay2();
	afx_msg void OnMacroReplay3();
	afx_msg void OnMacroReplay4();
	afx_msg void OnMacroReplay5();
	afx_msg void OnMacroReplay6();
	afx_msg void OnMacroReplay7();
	afx_msg void OnMacroReplay8();
	afx_msg void OnMacroReplay9();
	afx_msg void OnMacroReplay0();
	afx_msg void OnUpdateMacroReplay(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMacroReplay1(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMacroReplay2(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMacroReplay3(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMacroReplay4(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMacroReplay5(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMacroReplay6(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMacroReplay7(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMacroReplay8(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMacroReplay9(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMacroReplay0(CCmdUI* pCmdUI);
	afx_msg void OnViewSpellCheck();
	afx_msg void OnUpdateViewSpellCheck(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCommandExecute0();
	afx_msg void OnCommandExecute1();
	afx_msg void OnCommandExecute2();
	afx_msg void OnCommandExecute3();
	afx_msg void OnCommandExecute4();
	afx_msg void OnCommandExecute5();
	afx_msg void OnCommandExecute6();
	afx_msg void OnCommandExecute7();
	afx_msg void OnCommandExecute8();
	afx_msg void OnCommandExecute9();
	afx_msg void OnUpdateCommandExecute0(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCommandExecute1(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCommandExecute2(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCommandExecute3(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCommandExecute4(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCommandExecute5(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCommandExecute6(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCommandExecute7(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCommandExecute8(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCommandExecute9(CCmdUI* pCmdUI);
	afx_msg void OnEditUpperCase();
	afx_msg void OnEditLowerCase();
	afx_msg void OnEditCapitalize();
	afx_msg void OnEditInvertCase();
	afx_msg void OnSearchPrevEdit();
	afx_msg void OnSearchPairsBegin();
	afx_msg void OnSearchPairsEnd();
	afx_msg void OnSearchPairsBeginSelect();
	afx_msg void OnSearchPairsEndSelect();
	afx_msg void OnEditMakeComment();
	afx_msg void OnEditReleaseComment();
	afx_msg void OnMoveCaretLeft();
	afx_msg void OnMoveCaretRight();
	afx_msg void OnMoveCaretUp();
	afx_msg void OnMoveCaretDown();
	afx_msg void OnToolMsDosShell();
	afx_msg void OnToolViewInBrowser();
	afx_msg void OnEditIncreaseIndent();
	afx_msg void OnEditDecreaseIndent();
	afx_msg void OnEditSelectBlock();
	afx_msg void OnEditConvertTabsToSpaces();
	afx_msg void OnEditRemoveTrailingSpaces();
	afx_msg void OnEditDeleteWord();
	afx_msg void OnEditDeleteToEndOfLine();
	afx_msg void OnEditDeletePrevWord();
	afx_msg void OnCommandSendInput();
	afx_msg void OnCommandKillProcess();
	afx_msg void OnToolEvaluateLine();
	afx_msg void OnSearchNextWord();
	afx_msg void OnSearchPrevWord();
	afx_msg void OnSearchReplaceAllInSelection();
	afx_msg void OnSearchReplaceAllInFile();
	afx_msg void OnSearchReplaceAllInOpenFiles();
	afx_msg void OnSearchReplaceAllInFileOutputWindow();
	afx_msg void OnSearchReplaceThisOccurrence();
	afx_msg void OnEditConvertSpacesToTabs();
	afx_msg void OnEditLeadingSpacesToTabs();
	afx_msg void OnEditCopyFilePath();
	afx_msg void OnMoveCaretHome();
	afx_msg void OnMoveCaretEnd();
	afx_msg void OnMoveCaretPrior();
	afx_msg void OnMoveCaretNext();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnEditDeleteToBeginOfLine();
	afx_msg void OnSearchNextOccurrence();
	afx_msg void OnSearchPrevOccurrence();
	afx_msg void OnSearchFindNext();
	afx_msg void OnSearchFindPrev();
	afx_msg void OnUpdateSearchFindNext(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSearchFindPrev(CCmdUI* pCmdUI);
	afx_msg void OnScrollScreenTop();
	afx_msg void OnScrollScreenBottom();
	afx_msg void OnScrollScreenCenter();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateCommandSendInput(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCommandKillProcess(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CEDTVIEW_H__FFCA2B8E_F9C5_11D4_A6F1_0050CE184C9B__INCLUDED_)

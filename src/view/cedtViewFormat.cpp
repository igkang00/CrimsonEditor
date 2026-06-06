#include "stdafx.h"
#include "cedtHeader.h"


static FORMATEDWORD _words[MAX_WORDS_COUNT+1];

// width setting global variables
static BOOL  _bFixedPitch;
static INT   _nSpaceWidth, _nTabWidth, _nTabMargin;
static INT   _nWrapWidth, _nIndentWidth, _nMinWidth;

// temporary global variables for format text
static BOOL  _bWordSplit; 
static INT   _nLeadingSpaceWidth;
static SHORT _siSplitIndex, _siWordIndex;

static UCHAR _ucType[3], _ucInfo;
static SHORT _siIndex, _siLength;



static INT _GetLeadingSpaceWidth(CAnalyzedString & rLine, CDC * pDC) 
{
	INT nPosition = 0;

	for(SHORT i = 0; i < rLine.m_siWordCount; i++) {
		ANALYZEDWORD & rWord = rLine.m_pWord[i];
		UCHAR ucType = rWord.m_ucType[0]; SHORT siLength = rWord.m_siLength;

		if( ucType == WT_TAB ) {
			nPosition += ((nPosition + _nSpaceWidth - _nTabMargin) / _nTabWidth + 1) * _nTabWidth - nPosition;
		} else if( ucType == WT_SPACE ) {
			nPosition += _nSpaceWidth * siLength;
		} else break;
	}

	return nPosition;
}

static INT _GetWordWidth(LPCTSTR pWord, SHORT siLength, INT nPosition, UCHAR cType, CDC * pDC)
{
	if( cType == WT_TAB ) {
		return ((nPosition + _nSpaceWidth - _nTabMargin) / _nTabWidth + 1) * _nTabWidth - nPosition;
	} else if( cType == WT_SPACE || _bFixedPitch ) {
		return _nSpaceWidth * siLength;
	} else {
		CSize size = pDC->GetTextExtent(pWord, siLength);
		return (SHORT)size.cx;
	}
}

static SHORT _GetWordIndex(LPCTSTR pWord, SHORT siLength, INT nWidth, CDC * pDC)
{
	SHORT siIndex; CSize size;

	for(SHORT i = 0; i <= siLength; i++) {
		if( _bFixedPitch ) size.cx = _nSpaceWidth * i;
		else size = pDC->GetTextExtent(pWord, i);

		if( size.cx <= nWidth ) siIndex = i;
		else break;
	}

	return siIndex;
}


static void _WordFound(SHORT windex, UCHAR ucType, UCHAR ucInfo, SHORT siIndex, SHORT siLength, INT nPosition, INT nWidth)
{
	_words[windex].m_ucType[0] = ucType;
	_words[windex].m_ucType[1] = ucType;
	_words[windex].m_ucType[2] = ucType;
	_words[windex].m_ucInfo = ucInfo;
	_words[windex].m_siIndex = siIndex;
	_words[windex].m_siLength = siLength;
	_words[windex].m_nPosition = nPosition;
	_words[windex].m_nWidth = nWidth;
}

static void _WordFoundExtended(SHORT windex, UCHAR ucType[], UCHAR ucInfo, SHORT siIndex, SHORT siLength, INT nPosition, INT nWidth)
{
	_words[windex].m_ucType[0] = ucType[0];
	_words[windex].m_ucType[1] = ucType[1];
	_words[windex].m_ucType[2] = ucType[2];
	_words[windex].m_ucInfo = ucInfo;
	_words[windex].m_siIndex = siIndex;
	_words[windex].m_siLength = siLength;
	_words[windex].m_nPosition = nPosition;
	_words[windex].m_nWidth = nWidth;
}

static void _FinishLine(SHORT wcount, SHORT siSplitIndex, BOOL bLineBreak, CFormatedString & rFmtLine, CAnalyzedString & rLine)
{
	delete [] rFmtLine.m_pWord; rFmtLine.m_pWord = new FORMATEDWORD[wcount];
	memcpy(rFmtLine.m_pWord, _words, wcount * sizeof(FORMATEDWORD));

	rFmtLine.m_pString = (LPCTSTR)rLine;
	rFmtLine.m_siWordCount = wcount;
	rFmtLine.m_siSplitIndex = siSplitIndex;
	rFmtLine.m_bLineBreak = bLineBreak;
	rFmtLine.m_usLineInfo = rLine.m_usLineInfo;
	rFmtLine.m_usLineFlag = 0x0000;
}


static BOOL _FormatLineWrap(CFormatedString & rFmtLine, CAnalyzedString & rLine, CDC * pDC)
{
	
	SHORT i, wcount, siIndex, siLength, siSplit; 
	INT nPosition, nWidth; UCHAR ucType[3], ucInfo;

	_nLeadingSpaceWidth = _GetLeadingSpaceWidth(rLine, pDC);
	_siSplitIndex = 0;

	wcount = nPosition = 0;
	BOOL bLineBreak = FALSE;

	for(i = 0; i < rLine.m_siWordCount; i++) {
		ANALYZEDWORD & rWord = rLine.m_pWord[i];
		ucType[0] = rWord.m_ucType[0]; ucType[1] = rWord.m_ucType[1]; 
		ucType[2] = rWord.m_ucType[2]; ucInfo = rWord.m_ucInfo;
		siIndex = rWord.m_siIndex; siLength = rWord.m_siLength;

		nWidth = _GetWordWidth((LPCTSTR)rLine + siIndex, siLength, nPosition, ucType[0], pDC);

		if( _nWrapWidth <= _nLeadingSpaceWidth + _nIndentWidth + _nMinWidth ) {
			// not enough space - add in the same line
			_WordFoundExtended(wcount++, ucType, ucInfo, siIndex, siLength, nPosition, nWidth);
			nPosition += nWidth; _bWordSplit = FALSE;
		} else if( nPosition + nWidth <= _nWrapWidth || (ucType[0] == WT_RETURN || ucType[0] == WT_TAB || ucType[0] == WT_SPACE) ) {
			// add in the same line
			_WordFoundExtended(wcount++, ucType, ucInfo, siIndex, siLength, nPosition, nWidth);
			nPosition += nWidth; _bWordSplit = FALSE;
		} else if( _nLeadingSpaceWidth + _nIndentWidth + nWidth <= _nWrapWidth ) {
			// go next line
			_WordFound(wcount++, WT_LINEBREAK, RT_GLOBAL, siIndex, 0, nPosition, 0);
			_bWordSplit = FALSE; _siWordIndex = i; bLineBreak = TRUE; break;
		} else {
			// split word - go next line
			siSplit = _GetWordIndex((LPCTSTR)rLine + siIndex, siLength, _nWrapWidth - nPosition, pDC);
			nWidth = _GetWordWidth((LPCTSTR)rLine + siIndex, siSplit, nPosition, ucType[0], pDC);

			_WordFoundExtended(wcount++, ucType, ucInfo, siIndex, siSplit, nPosition, nWidth);
			siIndex += siSplit; siLength -= siSplit; nPosition += nWidth;

			_ucType[0] = ucType[0]; _ucType[1] = ucType[1]; 
			_ucType[2] = ucType[2]; _ucInfo = ucInfo;
			_siIndex = siIndex; _siLength = siLength; 

			_WordFound(wcount++, WT_LINEBREAK, RT_GLOBAL, siIndex, 0, nPosition, 0);
			_bWordSplit = TRUE; _siWordIndex = i; bLineBreak = TRUE; break;
		}
	}

	_FinishLine(wcount, _siSplitIndex++, bLineBreak, rFmtLine, rLine);
	return bLineBreak;
}


static BOOL _FormatLineWrapContinue(CFormatedString & rFmtLine, CAnalyzedString & rLine, CDC * pDC)
{
	SHORT i, wcount, siIndex, siLength, sSplit;
	INT nPosition, nWidth; UCHAR ucType[3], ucInfo;

	wcount = 0; nPosition = _nLeadingSpaceWidth + _nIndentWidth;
	BOOL bLineBreak = FALSE;

	for(i = _siWordIndex; i < rLine.m_siWordCount; i++) {
		if( _bWordSplit ) {
			ucType[0] = _ucType[0]; ucType[1] = _ucType[1];
			ucType[2] = _ucType[2]; ucInfo = _ucInfo; 
			siIndex = _siIndex; siLength = _siLength;
		} else {
			ANALYZEDWORD & rWord = rLine.m_pWord[i];
			ucType[0] = rWord.m_ucType[0]; ucType[1] = rWord.m_ucType[1]; 
			ucType[2] = rWord.m_ucType[2]; ucInfo = rWord.m_ucInfo;
			siIndex = rWord.m_siIndex; siLength = rWord.m_siLength;
		}

		nWidth = _GetWordWidth((LPCTSTR)rLine + siIndex, siLength, nPosition, ucType[0], pDC);

		if( nPosition + nWidth <= _nWrapWidth || (ucType[0] == WT_RETURN || ucType[0] == WT_TAB || ucType[0] == WT_SPACE) ) {
			// add in the same line
			_WordFoundExtended(wcount++, ucType, ucInfo, siIndex, siLength, nPosition, nWidth);
			nPosition += nWidth; _bWordSplit = FALSE;
		} else if( _nLeadingSpaceWidth + _nIndentWidth + nWidth <= _nWrapWidth ) {
			// go next line
			_WordFound(wcount++, WT_LINEBREAK, RT_GLOBAL, siIndex, 0, nPosition, 0);
			_bWordSplit = FALSE; _siWordIndex = i; bLineBreak = TRUE; break;
		} else {
			// split word - go next line
			sSplit = _GetWordIndex((LPCTSTR)rLine + siIndex, siLength, _nWrapWidth - nPosition, pDC);
			nWidth = _GetWordWidth((LPCTSTR)rLine + siIndex, sSplit, nPosition, ucType[0], pDC);

			_WordFoundExtended(wcount++, ucType, ucInfo, siIndex, sSplit, nPosition, nWidth);
			siIndex += sSplit; siLength -= sSplit; nPosition += nWidth;

			_ucType[0] = ucType[0]; _ucType[1] = ucType[1]; 
			_ucType[2] = ucType[2]; _ucInfo = ucInfo;
			_siIndex = siIndex; _siLength = siLength; 

			_WordFound(wcount++, WT_LINEBREAK, RT_GLOBAL, siIndex, 0, nPosition, 0);
			_bWordSplit = TRUE; _siWordIndex = i; bLineBreak = TRUE; break;
		}
	}

	_FinishLine(wcount, _siSplitIndex++, bLineBreak, rFmtLine, rLine);
	return bLineBreak;
}


static BOOL _FormatLineNoWrap(CFormatedString & rFmtLine, CAnalyzedString & rLine, CDC * pDC)
{
	SHORT i, wcount, siIndex, siLength;
	INT nPosition, nWidth; UCHAR ucType[3], ucInfo;

	wcount = nPosition = 0;

	for(i = 0; i < rLine.m_siWordCount; i++) {
		ANALYZEDWORD & rWord = rLine.m_pWord[i];
		ucType[0] = rWord.m_ucType[0]; ucType[1] = rWord.m_ucType[1]; 
		ucType[2] = rWord.m_ucType[2]; ucInfo = rWord.m_ucInfo;
		siIndex = rWord.m_siIndex; siLength = rWord.m_siLength;

		nWidth = _GetWordWidth((LPCTSTR)rLine + siIndex, siLength, nPosition, ucType[0], pDC);

		_WordFoundExtended(wcount++, ucType, ucInfo, siIndex, siLength, nPosition, nWidth);
		nPosition += nWidth;
	}

	_FinishLine(wcount, 0, FALSE, rFmtLine, rLine);
	return FALSE;
}


static void _CheckWordType(CFormatedString & rLine, FORMATEDWORD & rWord, USHORT & usFlag, CString & szTerminator)
{
	UCHAR ucType = rWord.m_ucType[0];
	UCHAR ucRange = rWord.m_ucInfo;

	switch( ucType ) {
	case WT_QUOTATION0:   if( ! CHECK_QUOTE_EX0(usFlag) && ! CHECK_COMMENT(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) usFlag ^=  LF_QUOTATION0;    break;
	case WT_QUOTATION1:   if( ! CHECK_QUOTE_EX1(usFlag) && ! CHECK_COMMENT(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) usFlag ^=  LF_QUOTATION1;    break;
	case WT_QUOTATION2:   if( ! CHECK_QUOTE_EX2(usFlag) && ! CHECK_COMMENT(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) usFlag ^=  LF_QUOTATION2;    break;
	case WT_QUOTATION3:   if( ! CHECK_QUOTE_EX3(usFlag) && ! CHECK_COMMENT(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) usFlag ^=  LF_QUOTATION3;    break;
	case WT_HEREDOCUMENT: if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) usFlag |=  LF_HEREDOCUMENT;  break;
	case WT_LINECOMMENT:  if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) usFlag |=  LF_LINECOMMENT;   break;
	case WT_COMMENT1ON:   if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) usFlag |=  LF_BLOCKCOMMENT1; break;
	case WT_COMMENT1OFF:  if( ! CHECK_QUOTATION(usFlag) &&   CHECK_CMT_BL1(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) usFlag &= ~LF_BLOCKCOMMENT1; break;
	case WT_COMMENT2ON:   if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) usFlag |=  LF_BLOCKCOMMENT2; break;
	case WT_COMMENT2OFF:  if( ! CHECK_QUOTATION(usFlag) &&   CHECK_CMT_BL2(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) usFlag &= ~LF_BLOCKCOMMENT2; break;

	case WT_SHADOWON:     if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) ) usFlag |=  LF_SHADOW;    break;
	case WT_SHADOWOFF:    if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) ) usFlag &= ~LF_SHADOW;    break;
	case WT_HIGHLIGHTON:  if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) ) usFlag |=  LF_HIGHLIGHT; break;
	case WT_HIGHLIGHTOFF: if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) ) usFlag &= ~LF_HIGHLIGHT; break;
	case WT_RANGE1BEG:    if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) && ! CHECK_RANGE2(usFlag) ) usFlag |=  LF_RANGE1; break;
	case WT_RANGE1END:    if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) && ! CHECK_RANGE2(usFlag) ) usFlag &= ~LF_RANGE1; break;
	case WT_RANGE2BEG:    if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) ) usFlag |=  LF_RANGE2; break;
	case WT_RANGE2END:    if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) ) usFlag &= ~LF_RANGE2; break;

	case WT_IDENTIFIER:
		if( CHECK_THIS(usFlag, LF_HEREDOCUMENT) ) {
			if( ! szTerminator.GetLength() ) {
				szTerminator = CString(rLine.m_pString + rWord.m_siIndex, rWord.m_siLength);
			}
		}
		if( CHECK_THIS(usFlag, LF_QUOTATIONH) ) {
			if( ! szTerminator.Compare(rLine.m_pString + rWord.m_siIndex) ) {
				szTerminator = "";
				usFlag &= ~LF_QUOTATIONH;
			}
		}
		break;

	case WT_RETURN:
		if( CHECK_THIS(usFlag, LF_HEREDOCUMENT) ) { 
			usFlag &= ~LF_HEREDOCUMENT;
			if( szTerminator.GetLength() ) usFlag |= LF_QUOTATIONH;
		}
		break;
	}
}


static BOOL _CheckLineFlag(CFormatedString & rFmtLine, USHORT & usLineFlag, CString & szTerminator)
{
	BOOL bCanStopProcessing = ( (rFmtLine.m_usLineFlag == usLineFlag) &&
		! rFmtLine.m_szHereDocumentTerminator.Compare(szTerminator) );

	if( ! bCanStopProcessing ) {
		rFmtLine.m_usLineFlag = usLineFlag;
		rFmtLine.m_szHereDocumentTerminator = szTerminator;
	}

	if( rFmtLine.m_usLineInfo & LI_HAVEHEREDOCUMENT || CHECK_THIS(usLineFlag, LF_QUOTATIONH) ) {
		for(SHORT i = 0; i < rFmtLine.m_siWordCount; i++ ) {
			FORMATEDWORD & rWord = rFmtLine.m_pWord[i];
			_CheckWordType(rFmtLine, rWord, usLineFlag, szTerminator);
		}
	} else if( rFmtLine.m_usLineInfo & (LI_HAVERANGE | LI_HAVEHIGHLIGHT | LI_HAVECOMMENT | LI_HAVEQUOTATION) ) {
		for(SHORT i = 0; i < rFmtLine.m_siWordCount; i++ ) {
			FORMATEDWORD & rWord = rFmtLine.m_pWord[i];
			UCHAR ucType = rWord.m_ucType[0];
			if( _IS_BET(WT_RANGE1BEG, ucType, WT_RANGE2END) || _IS_BET(WT_SHADOWON, ucType, WT_HIGHLIGHTOFF) ||
				_IS_BET(WT_LINECOMMENT, ucType, WT_COMMENT2OFF) || _IS_BET(WT_QUOTATION0, ucType, WT_QUOTATION3) )
				_CheckWordType(rFmtLine, rWord, usLineFlag, szTerminator);
		}
	}

	return bCanStopProcessing;
}


void CCedtView::FormatScreenText() 
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	INT nLineCount = pDoc->GetLineCount();

	m_clsFormatedScreenText.RemoveAll(); CFormatedString dummyLine;
	for(INT i = 0; i < nLineCount; i++) m_clsFormatedScreenText.AddTail( dummyLine );

	FormatScreenText(0, nLineCount);
}


void CCedtView::FormatScreenText(INT nIndex, INT nCount) 
{
	CRect rect; GetClientRect( & rect );
	INT nLeftMargin   = GetLeftMargin();
	INT nAveCharWidth = GetAveCharWidth();
	INT nMaxCharWidth = GetMaxCharWidth();

	// save width setting to global variables
	_bFixedPitch = IsUsingFixedPitchFont();
	_nSpaceWidth = GetSpaceWidth();
	_nTabWidth   = m_nTabSize * _nSpaceWidth;
	_nTabMargin  = 1;

	// save wrap setting to gloabl variables
	if( m_nFixedWrapWidth ) _nWrapWidth = m_nFixedWrapWidth * nAveCharWidth;
	else _nWrapWidth = rect.Width() - nLeftMargin - nMaxCharWidth;
	_nIndentWidth = m_nWrapIndentation * _nSpaceWidth;
	_nMinWidth = 8 * nAveCharWidth;


	CCedtDoc * pDoc = (CCedtDoc *)GetDocument(); ASSERT( pDoc );
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pMainFrame );
	INT nProcess = 0; CWaitCursor * pWait = NULL;

	if( nCount > 1000 ) {
		pWait = new CWaitCursor;
		pMainFrame->BeginProgress("Formatting...");
	}

	// now start formatting text
	POSITION po1 = pDoc->m_clsAnalyzedText.FindIndex(nIndex);
	POSITION po2 = FindScreenTextIndex(nIndex);

	USHORT usLineFlag = m_clsFormatedScreenText.GetAt(po2).m_usLineFlag;
	CString szTerminator = m_clsFormatedScreenText.GetAt(po2).m_szHereDocumentTerminator;
	BOOL bMultiLineStringConstant = MultiLineStringConstant();

	while( po1 && nProcess < nCount ) {
		usLineFlag &= ~LF_LINECOMMENT;
		if( ! bMultiLineStringConstant ) usLineFlag &= ~(LF_QUOTATION1 | LF_QUOTATION2 | LF_QUOTATION3);

		if( m_bLocalWordWrap ) {
			CAnalyzedString & rLine = pDoc->m_clsAnalyzedText.GetNext(po1); FlattenScreenTextAt(po2);
			BOOL bContinue = _FormatLineWrap( m_clsFormatedScreenText.GetAt(po2), rLine, & m_dcScreen );
			_CheckLineFlag( m_clsFormatedScreenText.GetAt(po2), usLineFlag, szTerminator );

			while( bContinue ) {
				CFormatedString dummyLine; po2 = m_clsFormatedScreenText.InsertAfter(po2, dummyLine);
				bContinue = _FormatLineWrapContinue( m_clsFormatedScreenText.GetAt(po2), rLine, & m_dcScreen );
				_CheckLineFlag( m_clsFormatedScreenText.GetAt(po2), usLineFlag, szTerminator );
			}
			m_clsFormatedScreenText.GetNext(po2);

		} else {
			CAnalyzedString & rLine = pDoc->m_clsAnalyzedText.GetNext(po1); // FlattenScreenTextAt(po2);
			_FormatLineNoWrap( m_clsFormatedScreenText.GetAt(po2), rLine, & m_dcScreen );
			_CheckLineFlag( m_clsFormatedScreenText.GetAt(po2), usLineFlag, szTerminator );
			m_clsFormatedScreenText.GetNext(po2);
		}

		if( nCount > 1000 && ! (nProcess % 20) ) pMainFrame->SetProgress( 100 * nProcess / nCount );
		nProcess++;
	}

	if( nCount > 1000 ) {
		if( pWait ) delete pWait;
		pMainFrame->EndProgress();
	}

	// post processing line flag until we reach the same status line...
	BOOL bCanStopProcessing = FALSE;

	while( po2 && ! bCanStopProcessing ) {
		usLineFlag &= ~LF_LINECOMMENT;
		if( ! bMultiLineStringConstant ) usLineFlag &= ~(LF_QUOTATION1 | LF_QUOTATION2 | LF_QUOTATION3);

		if( m_bLocalWordWrap ) {
			BOOL bContinue = m_clsFormatedScreenText.GetAt(po2).m_bLineBreak;
			bCanStopProcessing = _CheckLineFlag( m_clsFormatedScreenText.GetNext(po2), usLineFlag, szTerminator );

			while( bContinue ) {
				bContinue = m_clsFormatedScreenText.GetAt(po2).m_bLineBreak;
				bCanStopProcessing = _CheckLineFlag( m_clsFormatedScreenText.GetNext(po2), usLineFlag, szTerminator );
			}

		} else {
			bCanStopProcessing = _CheckLineFlag( m_clsFormatedScreenText.GetNext(po2), usLineFlag, szTerminator );
		}
	}
}


void CCedtView::FormatPrintText(CDC * pDC, RECT rectDraw)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	FormatPrintText(pDC, rectDraw, 0, pDoc->GetLineCount());
}


void CCedtView::FormatPrintText(CDC * pDC, RECT rectDraw, INT nIndex, INT nCount)
{
	CRect rect( rectDraw );
	INT nLeftMargin   = GetLeftMargin( pDC );
	INT nAveCharWidth = GetAveCharWidth( pDC );
	INT nMaxCharWidth = GetMaxCharWidth( pDC );

	// save width setting to global variables
	_bFixedPitch = IsUsingFixedPitchFont( pDC );
	_nSpaceWidth = GetSpaceWidth( pDC );
	_nTabWidth   = m_nTabSize * _nSpaceWidth;
	_nTabMargin  = _MY_MAX(_nSpaceWidth / 10, 1);

	// save wrap setting to gloabl variables
	_nWrapWidth = rect.Width() - nLeftMargin - nMaxCharWidth;
	_nIndentWidth = m_nWrapIndentation * _nSpaceWidth;
	_nMinWidth = 8 * nAveCharWidth;


	m_clsFormatedPrintText.RemoveAll(); CFormatedString dummyLine;
	for(INT i = 0; i < nCount; i++) m_clsFormatedPrintText.AddTail( dummyLine );

	CCedtDoc * pDoc = (CCedtDoc *)GetDocument(); ASSERT( pDoc );
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pMainFrame );
	INT nProcess = 0; CWaitCursor * pWait = NULL;

	if( nCount > 100 ) {
		pWait = new CWaitCursor;
		pMainFrame->BeginProgress("Formatting...");
	}

	// now start formatting text
	POSITION po1 = pDoc->m_clsAnalyzedText.FindIndex(nIndex);
	POSITION po2 = m_clsFormatedPrintText.GetHeadPosition();
	POSITION po3 = FindScreenTextIndex(nIndex);

	USHORT usLineFlag = m_clsFormatedScreenText.GetAt(po3).m_usLineFlag;
	CString szTerminator = m_clsFormatedScreenText.GetAt(po3).m_szHereDocumentTerminator;
	BOOL bMultiLineStringConstant = MultiLineStringConstant();

	while( po1 && nProcess < nCount ) {
		usLineFlag &= ~LF_LINECOMMENT;
		if( ! bMultiLineStringConstant ) usLineFlag &= ~(LF_QUOTATION1 | LF_QUOTATION2 | LF_QUOTATION3);

		CAnalyzedString & rLine = pDoc->m_clsAnalyzedText.GetNext(po1);
		BOOL bContinue = _FormatLineWrap( m_clsFormatedPrintText.GetAt(po2), rLine, pDC );
		_CheckLineFlag( m_clsFormatedPrintText.GetAt(po2), usLineFlag, szTerminator );

		while( bContinue ) {
			CFormatedString dummyLine; po2 = m_clsFormatedPrintText.InsertAfter(po2, dummyLine);
			bContinue = _FormatLineWrapContinue( m_clsFormatedPrintText.GetAt(po2), rLine, pDC );
			_CheckLineFlag( m_clsFormatedPrintText.GetAt(po2), usLineFlag, szTerminator );
		}
		m_clsFormatedPrintText.GetNext(po2);

		if( nCount > 100 && ! (nProcess % 2) ) pMainFrame->SetProgress( 100 * nProcess / nCount );
		nProcess++;
	}

	if( nCount > 100 ) {
		if( pWait ) delete pWait;
		pMainFrame->EndProgress();
	}
}


void CCedtView::RemoveScreenText(INT nIndex, INT nCount)
{
	POSITION posRemove = FindScreenTextIndex( nIndex );
	USHORT usLineFlag = m_clsFormatedScreenText.GetAt( posRemove ).m_usLineFlag;

	while( nCount-- ) posRemove = RemoveScreenTextAt( posRemove );
	if( posRemove ) m_clsFormatedScreenText.GetAt( posRemove ).m_usLineFlag = usLineFlag;
}

void CCedtView::InsertScreenText(INT nIndex, INT nCount)
{
	POSITION posInsert = (nIndex < 0) ? NULL : FindScreenTextIndex( nIndex );
	CFormatedString dummyLine;

	if( posInsert ) while( nCount-- ) m_clsFormatedScreenText.InsertAfter( posInsert, dummyLine );
	else while( nCount-- ) m_clsFormatedScreenText.AddHead( dummyLine );
}


INT CCedtView::GetWordWidth(LPCTSTR lpWord, SHORT siLength, INT nPosition, UCHAR ucType, CDC * pDC)
{
	// save width setting to global variables
	_bFixedPitch = IsUsingFixedPitchFont( pDC );
	_nSpaceWidth = GetSpaceWidth( pDC );
	_nTabWidth   = m_nTabSize * _nSpaceWidth;
	_nTabMargin  = _MY_MAX(_nSpaceWidth / 10, 1);

	if( pDC ) return _GetWordWidth(lpWord, siLength, nPosition, ucType, pDC);
	else return _GetWordWidth(lpWord, siLength, nPosition, ucType, & m_dcScreen);
}

SHORT CCedtView::GetWordIndex(LPCTSTR lpWord, SHORT siLength, INT nWidth, CDC * pDC)
{
	// save width setting to global variables
	_bFixedPitch = IsUsingFixedPitchFont( pDC );
	_nSpaceWidth = GetSpaceWidth( pDC );
	_nTabWidth   = m_nTabSize * _nSpaceWidth;
	_nTabMargin  = _MY_MAX(_nSpaceWidth / 10, 1);

	if( pDC ) return _GetWordIndex(lpWord, siLength, nWidth, pDC);
	else return _GetWordIndex(lpWord, siLength, nWidth, & m_dcScreen);
}


POSITION CCedtView::FindScreenTextIndex(INT nIndex)
{
	INT nParaCount = 0;
	POSITION posFound, pos = m_clsFormatedScreenText.GetHeadPosition();
	while( pos ) {
		posFound = pos;
		CFormatedString & rLine = m_clsFormatedScreenText.GetNext( pos );
		if( rLine.m_siSplitIndex == 0 ) nParaCount++;
		if( nParaCount-1 == nIndex ) break;
	}
	return posFound;
}

POSITION CCedtView::FlattenScreenTextAt(POSITION pos)
{
	INT nParaCount = 0;
	POSITION posFlatten;
	while( pos ) {
		posFlatten = pos;
		CFormatedString & rLine = m_clsFormatedScreenText.GetNext( pos );
		if( rLine.m_siSplitIndex == 0 ) nParaCount++;
		if( nParaCount-1 == 0 ) {
			if( rLine.m_siSplitIndex ) m_clsFormatedScreenText.RemoveAt( posFlatten );
		} else break;
	}
	return posFlatten;
}

POSITION CCedtView::RemoveScreenTextAt(POSITION pos)
{
	INT nParaCount = 0;
	POSITION posRemove;
	while( pos ) {
		posRemove = pos;
		CFormatedString & rLine = m_clsFormatedScreenText.GetNext( pos );
		if( rLine.m_siSplitIndex == 0 ) nParaCount++;
		if( nParaCount-1 == 0 ) m_clsFormatedScreenText.RemoveAt( posRemove );
		else break;
	}
	return posRemove;
}

#include "stdafx.h"
#include "cedtHeader.h"


#define _MIX_COLOR(color1, color2)				RGB( ( GetRValue(color1) + GetRValue(color2) ) / 2 , \
													 ( GetGValue(color1) + GetGValue(color2) ) / 2 , \
													 ( GetBValue(color1) + GetBValue(color2) ) / 2 )

#define _REV_COLOR(color1)						RGB( ( 255 - GetRValue(color1) ) , \
													 ( 255 - GetGValue(color1) ) , \
													 ( 255 - GetBValue(color1) ) )

#define _SET_TEXT_COLOR(pDC, bRevert, color)	pDC->SetTextColor( bRevert ? _REV_COLOR(color) : (color) )


static UCHAR _CheckWordType(CFormatedString & rLine, FORMATEDWORD & rWord, USHORT & usFlag, CString & szTerminator)
{
	UCHAR ucType = rWord.m_ucType[0];
	UCHAR ucRange = rWord.m_ucInfo;

	switch( ucType ) {
	case WT_QUOTATION0:   if( ! CHECK_QUOTE_EX0(usFlag) && ! CHECK_COMMENT(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) { usFlag ^=  LF_QUOTATION0;    return WT_QUOTATION0;  } else return WT_DELIMITER;
	case WT_QUOTATION1:   if( ! CHECK_QUOTE_EX1(usFlag) && ! CHECK_COMMENT(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) { usFlag ^=  LF_QUOTATION1;    return WT_QUOTATION0;  } else return WT_DELIMITER;
	case WT_QUOTATION2:   if( ! CHECK_QUOTE_EX2(usFlag) && ! CHECK_COMMENT(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) { usFlag ^=  LF_QUOTATION2;    return WT_QUOTATION0;  } else return WT_DELIMITER;
	case WT_QUOTATION3:   if( ! CHECK_QUOTE_EX3(usFlag) && ! CHECK_COMMENT(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) { usFlag ^=  LF_QUOTATION3;    return WT_QUOTATION0;  } else return WT_DELIMITER;
	case WT_HEREDOCUMENT: if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) { usFlag |=  LF_HEREDOCUMENT;  return WT_DELIMITER;   } else return WT_DELIMITER;
	case WT_LINECOMMENT:  if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) { usFlag |=  LF_LINECOMMENT;   return WT_LINECOMMENT; } else return WT_DELIMITER;
	case WT_COMMENT1ON:   if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) { usFlag |=  LF_BLOCKCOMMENT1; return WT_LINECOMMENT; } else return WT_DELIMITER;
	case WT_COMMENT1OFF:  if( ! CHECK_QUOTATION(usFlag) &&   CHECK_CMT_BL1(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) { usFlag &= ~LF_BLOCKCOMMENT1; return WT_LINECOMMENT; } else return WT_DELIMITER;
	case WT_COMMENT2ON:   if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) { usFlag |=  LF_BLOCKCOMMENT2; return WT_LINECOMMENT; } else return WT_DELIMITER;
	case WT_COMMENT2OFF:  if( ! CHECK_QUOTATION(usFlag) &&   CHECK_CMT_BL2(usFlag) && EFFECTIVE_RANGE(ucRange, usFlag) ) { usFlag &= ~LF_BLOCKCOMMENT2; return WT_LINECOMMENT; } else return WT_DELIMITER;

	case WT_SHADOWON:     if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) ) usFlag |=  LF_SHADOW;    return WT_SHADOWON;
	case WT_SHADOWOFF:    if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) ) usFlag &= ~LF_SHADOW;    return WT_SHADOWON;
	case WT_HIGHLIGHTON:  if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) ) usFlag |=  LF_HIGHLIGHT; return WT_HIGHLIGHTON;
	case WT_HIGHLIGHTOFF: if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) ) usFlag &= ~LF_HIGHLIGHT; return WT_HIGHLIGHTON;
	case WT_RANGE1BEG:    if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) && ! CHECK_RANGE2(usFlag) ) usFlag |=  LF_RANGE1; return WT_DELIMITER;
	case WT_RANGE1END:    if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) && ! CHECK_RANGE2(usFlag) ) usFlag &= ~LF_RANGE1; return WT_DELIMITER;
	case WT_RANGE2BEG:    if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) ) usFlag |=  LF_RANGE2; return WT_DELIMITER;
	case WT_RANGE2END:    if( ! CHECK_QUOTATION(usFlag) && ! CHECK_COMMENT(usFlag) ) usFlag &= ~LF_RANGE2; return WT_DELIMITER;

	case WT_IDENTIFIER:
	/*	if( CHECK_THIS(usFlag, LF_HEREDOCUMENT) ) {
			if( ! szTerminator.GetLength() ) {
				szTerminator = CString(rLine.m_pString + rWord.m_siIndex, rWord.m_siLength);
			}
		} */ 
		if( CHECK_THIS(usFlag, LF_QUOTATIONH) ) {
			if( ! szTerminator.Compare(rLine.m_pString + rWord.m_siIndex) ) {
				szTerminator = "";
				usFlag &= ~LF_QUOTATIONH;
			}
		}
		return WT_IDENTIFIER;

	case WT_RETURN:
	/*	if( CHECK_THIS(usFlag, LF_HEREDOCUMENT) ) {
			usFlag &= ~LF_HEREDOCUMENT;
			if( szTerminator.GetLength() ) usFlag |= LF_QUOTATIONH;
		} */
		return WT_RETURN;

	default:
		return ucType;
	}
}

static UCHAR _KeywordType(UCHAR ucType[], USHORT usFlag)
{
	if( CHECK_RANGE2(usFlag) ) return ucType[2];
	if( CHECK_RANGE1(usFlag) ) return ucType[1];
	return ucType[0];
}

static BOOL _CanEmbolden(UCHAR ucInfo, USHORT usFlag)
{
	if( CHECK_RANGE2(usFlag) ) return ucInfo & 0x04;
	if( CHECK_RANGE1(usFlag) ) return ucInfo & 0x02;
	return ucInfo & 0x01;
}


void CCedtView::DrawScreenBackgroundAndText() 
{
	CRect rect; GetClientRect( & rect );
	INT nCharHeight = GetCharHeight();		INT nLineHeight = GetLineHeight();
	INT nLeftMargin = GetLeftMargin();		INT nAveCharWidth = GetAveCharWidth();

	INT nNumWidth = GetNumberWidth();
	INT nSpaceWidth = GetSpaceWidth();

	BOOL bVariableHighlightInString = VariableHighlightInString();
	INT nParaMax = GetLastIdxY(), nLineMax = GetLastPosY() / nLineHeight;
	USHORT usFlag; CString szTerminator; UCHAR ucType1, ucType2; INT nPosition, nWidth;

	// fill background
	m_dcScreen.SelectClipRgn( NULL, RGN_COPY );
	if( m_bShowLineNumbers || m_bShowSelectionMargin ) {
		m_dcScreen.FillSolidRect(0, 0, nLeftMargin-1, rect.bottom, m_crBkgrColor[1]);
		m_dcScreen.FillSolidRect(nLeftMargin-1, 0, rect.right-nLeftMargin+1, rect.bottom, m_crBkgrColor[0]);
	} else {
		m_dcScreen.FillSolidRect(0, 0, rect.right, rect.bottom, m_crBkgrColor[0]);
	}

	INT nLineCount = m_nScrollPosY / nLineHeight;
	INT nParaCount = GetIdxYFromPosY(m_nScrollPosY);

	POSITION pos = m_clsFormatedScreenText.FindIndex(nLineCount);
	while( pos ) {
		CFormatedString & rLine = m_clsFormatedScreenText.GetNext( pos );
		INT nPosY = nLineCount * nLineHeight - m_nScrollPosY;

		if( nPosY > rect.Height() ) break; // drawing finished...
		BOOL bLineVisible = (nPosY + nLineHeight > 0) && (nPosY <= rect.Height());

		if( bLineVisible ) {
			// draw line number
			if( m_bShowLineNumbers && rLine.m_siSplitIndex == 0 ) {
				CString szLineNumber; szLineNumber.Format("%d", nParaCount+1);
				CSize size = m_dcScreen.GetTextExtent(szLineNumber);
				INT nPosX = nLeftMargin - size.cx - nNumWidth / 2;
				m_dcScreen.SetTextColor( m_crTextColor[WT_LINEBREAK] );
				m_dcScreen.TextOut(nPosX, nPosY, szLineNumber);
			}

			// draw bookmark
			if( (rLine.m_usLineInfo & LI_HAVEBOOKMARK) && rLine.m_siSplitIndex == 0 ) {
				if( m_bShowLineNumbers || m_bShowSelectionMargin ) {
					DrawScreenBookmark(nLeftMargin-5*nNumWidth/2+1, nPosY+1, 2*nNumWidth-2, nCharHeight-2);
				} else {
					COLORREF clr = RGB(0, 128, 128) ^ m_crBkgrColor[0];
					m_dcScreen.FillSolidRect(nLeftMargin, nPosY, rect.right-nLeftMargin, nCharHeight, clr);
				}
			}

			// exclude left margin from drawing
			m_dcScreen.ExcludeClipRect(0, nPosY, nLeftMargin, nPosY + nLineHeight);

			// initialize drawing status
			usFlag = rLine.m_usLineFlag;
			szTerminator = rLine.m_szHereDocumentTerminator;

			// drawing background - the reason why we split from drawing background from drawing text is that
			//                      when the font is italic drawing next background will erase part of previous text
			for(SHORT siWordCount = 0; siWordCount < rLine.m_siWordCount; siWordCount++) {
				FORMATEDWORD & rWord = rLine.m_pWord[siWordCount];
				nPosition = rWord.m_nPosition; nWidth = rWord.m_nWidth;
				ucType1 = rWord.m_ucType[0];

				INT nPosX = nLeftMargin + nPosition - m_nScrollPosX;
				if( ucType1 == WT_RETURN || ucType1 == WT_LINEBREAK ) nWidth = rect.right - nPosX;

				BOOL bWordVisible = nPosX + nWidth >= nLeftMargin && nPosX <= rect.right;
				BOOL bPrevRange1 = CHECK_RANGE1(usFlag), bPrevRange2 = CHECK_RANGE2(usFlag);
				ucType2 = _CheckWordType(rLine, rWord, usFlag, szTerminator);

				if( m_bSyntaxHighlight && bWordVisible && ( CHECK_RANGE2(usFlag) || (bPrevRange2 && ucType1 == WT_RANGE2END) ) && m_crBkgrColor[0] != m_crBkgrColor[4] ) {
					if( siWordCount == 0 && nPosition != 0 ) m_dcScreen.FillSolidRect(nLeftMargin, nPosY-(nLineHeight-nCharHeight)/2, nPosX-nLeftMargin, nLineHeight, m_crBkgrColor[0x04]);
					m_dcScreen.FillSolidRect(nPosX, nPosY-(nLineHeight-nCharHeight)/2, nWidth, nLineHeight, m_crBkgrColor[0x04]);
				} else if( m_bSyntaxHighlight && bWordVisible && ( CHECK_RANGE1(usFlag) || (bPrevRange1 && ucType1 == WT_RANGE1END) ) && m_crBkgrColor[0] != m_crBkgrColor[3] ) {
					if( siWordCount == 0 && nPosition != 0 ) m_dcScreen.FillSolidRect(nLeftMargin, nPosY-(nLineHeight-nCharHeight)/2, nPosX-nLeftMargin, nLineHeight, m_crBkgrColor[0x03]);
					m_dcScreen.FillSolidRect(nPosX, nPosY-(nLineHeight-nCharHeight)/2, nWidth, nLineHeight, m_crBkgrColor[0x03]);
				}
			}

			// restore status of before drawing text
			usFlag = rLine.m_usLineFlag;
			szTerminator = rLine.m_szHereDocumentTerminator;

			// drawing text
			for(siWordCount = 0; siWordCount < rLine.m_siWordCount; siWordCount++) {
				FORMATEDWORD & rWord = rLine.m_pWord[siWordCount];
				nPosition = rWord.m_nPosition; nWidth = rWord.m_nWidth;
				ucType1 = rWord.m_ucType[0];

				INT nPosX = nLeftMargin + nPosition - m_nScrollPosX;
				if( ucType1 == WT_RETURN || ucType1 == WT_LINEBREAK ) nWidth = rect.right - nPosX;

				BOOL bWordVisible = nPosX + nWidth >= nLeftMargin && nPosX <= rect.right;
				ucType2 = _CheckWordType(rLine, rWord, usFlag, szTerminator);

				if( bWordVisible && ucType1 != WT_LINEBREAK ) {
					if( ! m_bSyntaxHighlight ) m_dcScreen.SetTextColor( m_crTextColor[WT_IDENTIFIER] );
					else if( CHECK_HILIGHT(usFlag) ) m_dcScreen.SetTextColor( m_crTextColor[CHECK_THIS(usFlag, LF_HIGHLIGHT) ? WT_HIGHLIGHTON : WT_SHADOWON] );
					else if( CHECK_COMMENT(usFlag) ) m_dcScreen.SetTextColor( m_crTextColor[WT_LINECOMMENT] );
					else if( CHECK_QUOTATION(usFlag) ) {
						if( bVariableHighlightInString && ucType2 == WT_VARIABLE ) m_dcScreen.SetTextColor( _MIX_COLOR(m_crTextColor[WT_VARIABLE], m_crTextColor[WT_QUOTATION0]) );
						else m_dcScreen.SetTextColor( m_crTextColor[WT_QUOTATION0] );
					} else if( _IS_BET(WT_KEYWORD0, ucType2, WT_KEYWORD9) || ucType2 == WT_IDENTIFIER ) {
						ucType2 = _KeywordType(rWord.m_ucType, usFlag);
						m_dcScreen.SetTextColor( m_crTextColor[ucType2] );
					} else m_dcScreen.SetTextColor( m_crTextColor[ucType2] );

					if( m_bShowLineBreak && ucType1 == WT_RETURN && nParaCount != nParaMax ) {
						DrawScreenLineBreak(nPosX, nPosY, nSpaceWidth, nLineHeight);
					} else if( m_bShowTabChars && ucType1 == WT_TAB ) {
						DrawScreenTabChar(nPosX, nPosY, nSpaceWidth, nLineHeight);
					} else if( m_bShowSpaces && ucType1 == WT_SPACE ) {
						DrawScreenSpaces(nPosX, nPosY, nSpaceWidth, nLineHeight, rWord.m_siLength);
					} else if( ucType1 != WT_RETURN && ucType1 != WT_TAB && ucType1 != WT_SPACE ) {
						// when italicize comment option is on and this part is comment then set font to italic
						if( m_bItalicizeComment && (CHECK_COMMENT(usFlag) || ucType2 == WT_LINECOMMENT) ) m_dcScreen.SelectObject( & m_fontScreenIt );

						// draw text string
						m_dcScreen.TextOut(nPosX, nPosY, (LPCTSTR)rLine + rWord.m_siIndex, rWord.m_siLength);

						if( m_bEmboldenKeywords && _IS_BET(WT_KEYWORD0, ucType2, WT_KEYWORD9) && _CanEmbolden(rWord.m_ucInfo, usFlag) && ! CHECK_HILIGHT(usFlag) && ! CHECK_COMMENT(usFlag) && ! CHECK_QUOTATION(usFlag) ) 
							m_dcScreen.TextOut(nPosX+1, nPosY, (LPCTSTR)rLine + rWord.m_siIndex, rWord.m_siLength);

						if( m_bLocalSpellCheck && ucType2 == WT_WRONGWORD /* && ! CHECK_HIGHLIGHT && ! CHECK_COMMENT && ! CHECK_QUOTATION */ ) 
							DrawScreenUnderbar(nPosX, nPosY+nCharHeight-1, rWord.m_nWidth, 1);

						// restore font from italic
						if( m_bItalicizeComment && (CHECK_COMMENT(usFlag) || ucType2 == WT_LINECOMMENT) ) m_dcScreen.SelectObject( & m_fontScreen );
					}
				}
			}
		}

		nLineCount++;
		if( rLine.m_siSplitIndex == 0 ) nParaCount++;
	}
}


void CCedtView::InvertScreenSelected()
{
	CRect rect; GetClientRect( & rect );
	INT nCharHeight = GetCharHeight();		INT nLineHeight = GetLineHeight();
	INT nLeftMargin = GetLeftMargin();		INT nAveCharWidth = GetAveCharWidth();

	INT nBegX, nBegY, nEndX, nEndY;
	GetSelectedPosition( nBegX, nBegY, nEndX, nEndY );

	INT nLineCount = m_nScrollPosY / nLineHeight;
	INT nParaCount = GetIdxYFromPosY(m_nScrollPosY);

	POSITION pos = m_clsFormatedScreenText.FindIndex(nLineCount);
	while( pos ) {
		CFormatedString & rLine = m_clsFormatedScreenText.GetNext( pos );
		INT nPosY = nLineCount * nLineHeight - m_nScrollPosY;

		if( nPosY > rect.Height() ) break; // drawing finished
		BOOL bLineVisible = (nPosY + nLineHeight > 0) && (nPosY <= rect.Height());

		if( bLineVisible ) {
			INT nCurY = nLineCount * nLineHeight;
			INT nLineBreak = rLine.m_bLineBreak ? 0 : nAveCharWidth;

			if( nCurY >= nBegY && nCurY <= nEndY ) { // in block
				RECT rectInvert; rectInvert.top = nPosY;
				rectInvert.bottom = nPosY + nCharHeight;

				if( m_bColumnMode ) {
					rectInvert.left = nBegX - m_nScrollPosX + nLeftMargin;
					if( nBegX == nEndX && nCurY != m_nCaretPosY ) {
						INT nCaretWidth = 25 * nAveCharWidth / 100; // default caret width
						rectInvert.right = rectInvert.left + nCaretWidth;
					} else rectInvert.right = nEndX - m_nScrollPosX + nLeftMargin;
				} else {
					if( nCurY == nBegY && nCurY == nEndY ) {
						rectInvert.left = nBegX - m_nScrollPosX + nLeftMargin;
						rectInvert.right = nEndX - m_nScrollPosX + nLeftMargin;
					} else if( nCurY == nBegY ) {
						rectInvert.left = nBegX - m_nScrollPosX + nLeftMargin;
						rectInvert.right = GetLastPosX(rLine) - m_nScrollPosX + nLeftMargin + nLineBreak;
					} else if( nCurY == nEndY ) {
						rectInvert.left = GetFirstPosX(rLine) - m_nScrollPosX + nLeftMargin;
						rectInvert.right = nEndX - m_nScrollPosX + nLeftMargin;
					} else {
						rectInvert.left = GetFirstPosX(rLine) - m_nScrollPosX + nLeftMargin;
						rectInvert.right = GetLastPosX(rLine) - m_nScrollPosX + nLeftMargin + nLineBreak;
					}
				}

				if( rectInvert.top    < rect.top    ) rectInvert.top    = rect.top;
				if( rectInvert.bottom > rect.bottom ) rectInvert.bottom = rect.bottom;
				if( rectInvert.left   < rect.left   ) rectInvert.left   = rect.left;
				if( rectInvert.right  > rect.right  ) rectInvert.right  = rect.right;

				m_dcScreen.InvertRect( & rectInvert );
			}
		}

		nLineCount++;
		if( rLine.m_siSplitIndex == 0 ) nParaCount++;
	}
}


void CCedtView::DrawPrintPageOutline(CDC * pDC, RECT rectDraw, INT nCurPage)
{
	CRect rect( rectDraw );
	INT nLineHeight = GetLineHeight( pDC );
	INT nPosX, nPosY; CSize size;

	CPen pen( PS_SOLID, 1, RGB(0, 0, 0) );
	CPen * pPenOld = pDC->SelectObject( & pen );
	pDC->SetTextColor( RGB(0, 0, 0) );

	if( m_bPrintHeader ) {
		nPosY = - rect.top;
		nPosX = rect.left;
		pDC->TextOut(nPosX, nPosY, m_szHeaderString[0]);

		size = pDC->GetTextExtent(m_szHeaderString[1]);
		nPosX = rect.left + (rect.Width() - size.cx) / 2;
		pDC->TextOut(nPosX, nPosY, m_szHeaderString[1]);

		size = pDC->GetTextExtent(m_szHeaderString[2]);
		nPosX = rect.right - size.cx;
		pDC->TextOut(nPosX, nPosY, m_szHeaderString[2]);

		nPosY = - rect.top - 4 * nLineHeight / 3;
		pDC->MoveTo(rect.left, nPosY);
		pDC->LineTo(rect.right, nPosY);
	}

	if( m_bPrintFooter ) {
		nPosY = - rect.bottom + 4 * nLineHeight / 3;
		pDC->MoveTo(rect.left, nPosY);
		pDC->LineTo(rect.right, nPosY);

		nPosY = - rect.bottom + nLineHeight;
		nPosX = rect.left;
		pDC->TextOut(nPosX, nPosY, m_szFooterString[0]);

		size = pDC->GetTextExtent(m_szFooterString[1]);
		nPosX = rect.left + (rect.Width() - size.cx) / 2;
		pDC->TextOut(nPosX, nPosY, m_szFooterString[1]);

		size = pDC->GetTextExtent(m_szFooterString[2]);
		nPosX = rect.right - size.cx;
		pDC->TextOut(nPosX, nPosY, m_szFooterString[2]);
	}

	pDC->SelectObject( pPenOld );
}


void CCedtView::DrawPrintPageBackgroundAndText(CDC * pDC, RECT rectDraw, INT nCurPage)
{
	CRect rect( rectDraw );
	INT nCharHeight = GetCharHeight(pDC);	INT nLineHeight = GetLineHeight(pDC);
	INT nLeftMargin = GetLeftMargin(pDC);	INT nAveCharWidth = GetAveCharWidth(pDC);

	INT nLinesPerPage = rect.Height() / nLineHeight;
	if( m_bPrintHeader ) { nLinesPerPage = nLinesPerPage - 2; rect.top = rect.top + 2 * nLineHeight; }
	if( m_bPrintFooter ) { nLinesPerPage = nLinesPerPage - 2; rect.bottom = rect.bottom - 2 * nLineHeight; }
	INT nScrollPosY = (nCurPage - 1) * nLinesPerPage * nLineHeight;

	INT nNumWidth = GetNumberWidth(pDC);
	INT nSpaceWidth = GetSpaceWidth(pDC);

	BOOL bRevert = FALSE; COLORREF crBkgr = m_crBkgrColor[0x00];
	if( GetRValue(crBkgr) + GetGValue(crBkgr) + GetBValue(crBkgr) < 3 * 128 ) bRevert = m_bPrintSyntaxHighlight;

	BOOL bVariableHighlightInString = VariableHighlightInString();
	INT nParaMax = GetLastIdxY(), nLineMax = GetLastPosY() / nLineHeight;
	USHORT usFlag; CString szTerminator; UCHAR ucType1, ucType2; INT nPosition, nWidth;

	INT nLineCount = 0;
	INT nParaCount = m_nFormatedPrintTextStartIdxY;

	POSITION pos = m_clsFormatedPrintText.GetHeadPosition();
	while( pos ) {
		CFormatedString & rLine = m_clsFormatedPrintText.GetNext( pos );
		INT nPosY = nLineCount * nLineHeight - nScrollPosY;

		if( nPosY > rect.Height()) break; // drawing finished...
		BOOL bLineVisible = (nPosY >= 0) && (nPosY + nLineHeight <= rect.Height());

		if( bLineVisible ) {
			// draw line number
			if( m_bPrintLineNumbers && rLine.m_siSplitIndex == 0 ) {
				CString szLineNumber; szLineNumber.Format("%d:", nParaCount+1);
				CSize size = pDC->GetTextExtent(szLineNumber);
				INT nPosX = nLeftMargin - size.cx - nNumWidth / 1;
				pDC->SetTextColor( RGB(0, 0, 0) );
				pDC->TextOut(rect.left+nPosX, -rect.top-nPosY, szLineNumber);
			}

			// initialize drawing status
			usFlag = rLine.m_usLineFlag;
			szTerminator = rLine.m_szHereDocumentTerminator;

			// drawing background - the reason why we split from drawing background from drawing text is that
			//                      when the font is italic drawing next background will erase part of previous text
			for(SHORT siWordCount = 0; siWordCount < rLine.m_siWordCount; siWordCount++) {
				FORMATEDWORD & rWord = rLine.m_pWord[siWordCount];
				nPosition = rWord.m_nPosition; nWidth = rWord.m_nWidth;
				ucType1 = rWord.m_ucType[0];

				INT nPosX = nLeftMargin + nPosition;
				if( ucType1 == WT_RETURN || ucType1 == WT_LINEBREAK ) nWidth = rect.right - rect.left - nPosX;

				BOOL bWordVisible = nPosX + nWidth >= nLeftMargin && nPosX <= rect.right;
				BOOL bPrevRange1 = CHECK_RANGE1(usFlag), bPrevRange2 = CHECK_RANGE2(usFlag);
				ucType2 = _CheckWordType(rLine, rWord, usFlag, szTerminator);

				if( m_bPrintSyntaxHighlight && bWordVisible && ( CHECK_RANGE2(usFlag) || (bPrevRange2 && ucType1 == WT_RANGE2END) ) && m_crBkgrColor[0] != m_crBkgrColor[4] ) {
					if( siWordCount == 0 && nPosition != 0 ) pDC->FillSolidRect(rect.left+nLeftMargin, -(rect.top+nPosY+nCharHeight)-(nLineHeight-nCharHeight)/2, nPosX-nLeftMargin, nLineHeight, bRevert ? _REV_COLOR(m_crBkgrColor[0x04]) : m_crBkgrColor[0x04]);
					pDC->FillSolidRect(rect.left+nPosX, -(rect.top+nPosY+nCharHeight)-(nLineHeight-nCharHeight)/2, nWidth, nLineHeight, bRevert ? _REV_COLOR(m_crBkgrColor[0x04]) : m_crBkgrColor[0x04]);
				} else if( m_bPrintSyntaxHighlight && bWordVisible && ( CHECK_RANGE1(usFlag) || (bPrevRange1 && ucType1 == WT_RANGE1END) ) && m_crBkgrColor[0] != m_crBkgrColor[3] ) {
					if( siWordCount == 0 && nPosition != 0 ) pDC->FillSolidRect(rect.left+nLeftMargin, -(rect.top+nPosY+nCharHeight)-(nLineHeight-nCharHeight)/2, nPosX-nLeftMargin, nLineHeight, bRevert ? _REV_COLOR(m_crBkgrColor[0x03]) : m_crBkgrColor[0x03]);
					pDC->FillSolidRect(rect.left+nPosX, -(rect.top+nPosY+nCharHeight)-(nLineHeight-nCharHeight)/2, nWidth, nLineHeight, bRevert ? _REV_COLOR(m_crBkgrColor[0x03]) : m_crBkgrColor[0x03]);
				}
			}

			// restore status of before drawing text
			usFlag = rLine.m_usLineFlag;
			szTerminator = rLine.m_szHereDocumentTerminator;

			// drawing text
			for(siWordCount = 0; siWordCount < rLine.m_siWordCount; siWordCount++) {
				FORMATEDWORD & rWord = rLine.m_pWord[siWordCount];
				nPosition = rWord.m_nPosition; nWidth = rWord.m_nWidth;
				ucType1 = rWord.m_ucType[0];

				INT nPosX = nLeftMargin + nPosition;
				if( ucType1 == WT_RETURN || ucType1 == WT_LINEBREAK ) nWidth = rect.right - rect.left - nPosX;

				BOOL bWordVisible = nPosX + nWidth >= nLeftMargin && nPosX <= rect.right;
				ucType2 = _CheckWordType(rLine, rWord, usFlag, szTerminator);

				if( bWordVisible && ucType1 != WT_LINEBREAK ) {
					if( ! m_bPrintSyntaxHighlight ) _SET_TEXT_COLOR(pDC, FALSE, RGB(0, 0, 0) );
					else if( CHECK_HILIGHT(usFlag) ) _SET_TEXT_COLOR(pDC, bRevert, m_crTextColor[CHECK_THIS(usFlag, LF_HIGHLIGHT) ? WT_HIGHLIGHTON : WT_SHADOWON] );
					else if( CHECK_COMMENT(usFlag) ) _SET_TEXT_COLOR(pDC, bRevert, m_crTextColor[WT_LINECOMMENT] );
					else if( CHECK_QUOTATION(usFlag) ) {
						if( bVariableHighlightInString && ucType2 == WT_VARIABLE ) _SET_TEXT_COLOR(pDC, bRevert, _MIX_COLOR(m_crTextColor[WT_VARIABLE], m_crTextColor[WT_QUOTATION0]));
						else _SET_TEXT_COLOR(pDC, bRevert, m_crTextColor[WT_QUOTATION0]);
					} else if( _IS_BET(WT_KEYWORD0, ucType2, WT_KEYWORD9) || ucType2 == WT_IDENTIFIER ) {
						ucType2 = _KeywordType(rWord.m_ucType, usFlag);
						_SET_TEXT_COLOR(pDC, bRevert, m_crTextColor[ucType2]);
					} else _SET_TEXT_COLOR(pDC, bRevert, m_crTextColor[ucType2]);

					if( ucType1 != WT_RETURN && ucType1 != WT_TAB && ucType1 != WT_SPACE ) {
						// when italicize comment option is on and this part is comment then set font to italic
						if( m_bItalicizeComment && (CHECK_COMMENT(usFlag) || ucType2 == WT_LINECOMMENT) ) pDC->SelectObject( & m_fontPrinterIt );

						// draw text string
						pDC->TextOut(rect.left+nPosX, -(rect.top+nPosY), (LPCTSTR)rLine + rWord.m_siIndex, rWord.m_siLength);

						if( m_bEmboldenKeywords && _IS_BET(WT_KEYWORD0, ucType2, WT_KEYWORD9) && _CanEmbolden(rWord.m_ucInfo, usFlag) && ! CHECK_HILIGHT(usFlag) && ! CHECK_COMMENT(usFlag) && ! CHECK_QUOTATION(usFlag) ) 
							pDC->TextOut(rect.left+nPosX+nAveCharWidth/9, -(rect.top+nPosY), (LPCTSTR)rLine + rWord.m_siIndex, rWord.m_siLength);

						// restore font from italic
						if( m_bItalicizeComment && (CHECK_COMMENT(usFlag) || ucType2 == WT_LINECOMMENT) ) pDC->SelectObject( & m_fontPrinter );
					}
				}
			}
		}

		nLineCount++;
		if( rLine.m_siSplitIndex == 0 ) nParaCount++;
	}
}


void CCedtView::DrawActiveLineHighlight()
{
	RECT rect; GetClientRect( & rect );
	INT nCharHeight = GetCharHeight();
	INT nLeftMargin = GetLeftMargin();
	INT nPosY = m_nCaretPosY - m_nScrollPosY;

	if( nPosY >= 0 && nPosY <= rect.bottom ) {
		INT nLineWidth  = rect.right - nLeftMargin;
	//	INT nLineHeight = GetLineHeight();

		m_dcActiveLine.BitBlt(nLeftMargin, nCharHeight+1, nLineWidth, nCharHeight+1, & m_dcScreen, nLeftMargin, nPosY, SRCCOPY);

		m_dcActiveLine.FillSolidRect(nLeftMargin, 0, nLineWidth, nCharHeight, m_crBkgrColor[0]);
		m_dcActiveLine.BitBlt(nLeftMargin, nCharHeight+1, nLineWidth, nCharHeight+1, & m_dcActiveLine, nLeftMargin, 0, SRCINVERT);
		m_dcActiveLine.FillSolidRect(nLeftMargin, 0, nLineWidth, nCharHeight, m_crBkgrColor[2]);
		m_dcActiveLine.BitBlt(nLeftMargin, nCharHeight+1, nLineWidth, nCharHeight+1, & m_dcActiveLine, nLeftMargin, 0, SRCINVERT);

		if( m_bHighlightActiveLine ) {
			for(INT i = nLeftMargin; i < rect.right; i += 2) {
				m_dcActiveLine.SetPixelV(i, nCharHeight+1, m_crTextColor[WT_IDENTIFIER]);
				m_dcActiveLine.SetPixelV(i, nCharHeight+1+nCharHeight, m_crTextColor[WT_IDENTIFIER]);
			}
		}

		m_dcActiveLine.BitBlt(nLeftMargin, 0, nLineWidth, nCharHeight+1, & m_dcScreen, nLeftMargin, nPosY, SRCCOPY);
		m_dcScreen.BitBlt(nLeftMargin, nPosY, nLineWidth, nCharHeight+1, & m_dcActiveLine, nLeftMargin, nCharHeight+1, SRCCOPY);

		m_bActiveLineHighlighted = TRUE;
		m_nActiveLineHighlightedPosY = nPosY;
	} else {
		m_bActiveLineHighlighted = FALSE;
		m_nActiveLineHighlightedPosY = 0;
	}
}

void CCedtView::DrawColumnMarkerHighlight()
{
	RECT rect; GetClientRect( & rect );
	INT nLeftMargin = GetLeftMargin();
	INT nAveCharWidth = GetAveCharWidth();
	INT nPosX, nPosY;

	nPosX = nLeftMargin + m_nColumnMarker1Pos * nAveCharWidth - m_nScrollPosX;
	if( m_bShowColumnMarker1 && (nPosX >= nLeftMargin && nPosX <= rect.right) ) {
		for( nPosY = 0; nPosY < rect.bottom; nPosY += 2 ) m_dcScreen.SetPixelV(nPosX, nPosY, m_crTextColor[WT_IDENTIFIER]);
	}

	nPosX = nLeftMargin + m_nColumnMarker2Pos * nAveCharWidth - m_nScrollPosX;
	if( m_bShowColumnMarker2 && (nPosX >= nLeftMargin && nPosX <= rect.right) ) {
		for( nPosY = 0; nPosY < rect.bottom; nPosY += 2 ) m_dcScreen.SetPixelV(nPosX, nPosY, m_crTextColor[WT_IDENTIFIER]);
	}
}

void CCedtView::DrawScreenBookmark(INT nPosX, INT nPosY, INT nWidth, INT nHeight)
{
	POINT points[4];
	points[0].x = nPosX;			points[0].y = nPosY;
	points[1].x = nPosX + nWidth;	points[1].y = nPosY + nHeight / 2;
	points[2].x = nPosX;			points[2].y = nPosY + nHeight;

	CPen * penOld, pen(PS_SOLID, 1, RGB(0, 0, 0));
	CBrush * brushOld, brush(RGB(0, 255, 0));
	penOld = m_dcScreen.SelectObject( & pen );
	brushOld = m_dcScreen.SelectObject( & brush );
	m_dcScreen.FillSolidRect(nPosX, nPosY, nWidth, nHeight, m_crBkgrColor[1]);
	m_dcScreen.Polygon( points, 3 );
	m_dcScreen.SelectObject( penOld );
	m_dcScreen.SelectObject( brushOld );
}

void CCedtView::DrawScreenUnderbar(INT nPosX, INT nPosY, INT nWidth, INT nHeight)
{
	CPen * penOld, pen(PS_SOLID, 1, RGB(255, 0, 0));
	penOld = m_dcScreen.SelectObject( & pen );

	if( nHeight == 0 ) nHeight = 1;
	INT i, nCount = nWidth / nHeight;

	m_dcScreen.MoveTo(nPosX, nPosY);
	for(i = 1; i <= nCount; i++) {
		m_dcScreen.LineTo(nPosX + i * nHeight, (i % 2) ? nPosY - nHeight : nPosY);
	}

	m_dcScreen.SelectObject( penOld );
}

void CCedtView::DrawScreenSpaces(INT nPosX, INT nPosY, INT nWidth, INT nHeight, INT nCount)
{
	for(INT i = 0; i < nCount; i++) {
		m_dcScreen.SetPixelV( nPosX + i * nWidth + nWidth / 2,     nPosY + nHeight / 2,     RGB(127, 127, 127) );
		m_dcScreen.SetPixelV( nPosX + i * nWidth + nWidth / 2,     nPosY + nHeight / 2 - 1, RGB(127, 127, 127) );
		m_dcScreen.SetPixelV( nPosX + i * nWidth + nWidth / 2 - 1, nPosY + nHeight / 2 - 1, RGB(127, 127, 127) );
		m_dcScreen.SetPixelV( nPosX + i * nWidth + nWidth / 2 - 1, nPosY + nHeight / 2,     RGB(127, 127, 127) );
	}
}

void CCedtView::DrawScreenTabChar(INT nPosX, INT nPosY, INT nWidth, INT nHeight)
{
	if( nWidth < 8 ) nWidth = 8;
	INT nCenY = nPosY + nHeight / 2;
	INT nDelY = 4 * nWidth / 8;

	CPen * penOld, pen(PS_SOLID, 1, RGB(127, 127, 127));
	penOld = m_dcScreen.SelectObject( & pen );

	m_dcScreen.MoveTo(nPosX + 4 * nWidth / 8, nCenY);
	m_dcScreen.LineTo(nPosX + 0 * nWidth / 8, nCenY - nDelY);
	m_dcScreen.MoveTo(nPosX + 4 * nWidth / 8, nCenY);
	m_dcScreen.LineTo(nPosX + 0 * nWidth / 8, nCenY + nDelY);

	m_dcScreen.MoveTo(nPosX + 7 * nWidth / 8, nCenY);
	m_dcScreen.LineTo(nPosX + 3 * nWidth / 8, nCenY - nDelY);
	m_dcScreen.MoveTo(nPosX + 7 * nWidth / 8, nCenY);
	m_dcScreen.LineTo(nPosX + 3 * nWidth / 8, nCenY + nDelY);

	m_dcScreen.SelectObject( penOld );
}

void CCedtView::DrawScreenLineBreak(INT nPosX, INT nPosY, INT nWidth, INT nHeight)
{
	if( nWidth < 8 ) nWidth = 8;

	CPen * penOld, pen(PS_SOLID, 1, RGB(127, 127, 127));
	penOld = m_dcScreen.SelectObject( & pen );

	m_dcScreen.Arc(nPosX +  1 * nWidth / 8, nPosY + 2 * nHeight / 10,
				   nPosX +  9 * nWidth / 8, nPosY + 6 * nHeight / 10,
				   nPosX +  6 * nWidth / 8, nPosY + 2 * nHeight / 10,
				   nPosX +  7 * nWidth / 8, nPosY + 6 * nHeight / 10);

	m_dcScreen.MoveTo(nPosX +  6 * nWidth / 8, nPosY + 2 * nHeight / 10);
	m_dcScreen.LineTo(nPosX +  6 * nWidth / 8, nPosY + 8 * nHeight / 10);
	m_dcScreen.MoveTo(nPosX +  8 * nWidth / 8, nPosY + 2 * nHeight / 10);
	m_dcScreen.LineTo(nPosX +  8 * nWidth / 8, nPosY + 8 * nHeight / 10);

	m_dcScreen.MoveTo(nPosX +  9 * nWidth / 8, nPosY + 2 * nHeight / 10);
	m_dcScreen.LineTo(nPosX +  6 * nWidth / 8, nPosY + 2 * nHeight / 10); 
	m_dcScreen.MoveTo(nPosX +  9 * nWidth / 8, nPosY + 8 * nHeight / 10);
	m_dcScreen.LineTo(nPosX +  3 * nWidth / 8, nPosY + 8 * nHeight / 10); 

	m_dcScreen.SelectObject( penOld );
}


void CCedtView::ParsePageHeaderAndFooter(INT nCurPage, INT nMaxPage)
{
	CString szPathName, szFileName, szPageNumber, szTotalPage, szCurrTime, szCurrDate;
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();

	szPathName = pDoc->GetTitle();
	szFileName = GetFileName( szPathName );

	szPageNumber.Format("%d", nCurPage );
	szTotalPage.Format("%d", nMaxPage );
	szCurrDate = GetCurrentDate();
	szCurrTime = GetCurrentTime();

	for(INT i = 0; i < 3; i++) {
		m_szHeaderString[i] = m_szHeaderFormat[i];
		m_szFooterString[i] = m_szFooterFormat[i];

		ParsePageHeaderAndFooter(m_szHeaderString[i], szPathName, szFileName, szPageNumber, szTotalPage, szCurrDate, szCurrTime);
		ParsePageHeaderAndFooter(m_szFooterString[i], szPathName, szFileName, szPageNumber, szTotalPage, szCurrDate, szCurrTime);
	}
}

void CCedtView::ParsePageHeaderAndFooter(CString & szArgument, LPCTSTR lpszFilePath, LPCTSTR lpszFileName, 
										 LPCTSTR lpszPageNumber, LPCTSTR lpszTotalPage, LPCTSTR lpszCurrDate, LPCTSTR lpszCurrTime)
{
	INT nFound;
	while( (nFound = szArgument.Find("$(FilePath)"  )) >= 0 ) szArgument = szArgument.Left(nFound) + lpszFilePath   + szArgument.Mid(nFound + 11);
	while( (nFound = szArgument.Find("$(FileName)"  )) >= 0 ) szArgument = szArgument.Left(nFound) + lpszFileName   + szArgument.Mid(nFound + 11);
	while( (nFound = szArgument.Find("$(PageNumber)")) >= 0 ) szArgument = szArgument.Left(nFound) + lpszPageNumber + szArgument.Mid(nFound + 13);
	while( (nFound = szArgument.Find("$(TotalPage)" )) >= 0 ) szArgument = szArgument.Left(nFound) + lpszTotalPage  + szArgument.Mid(nFound + 12);
	while( (nFound = szArgument.Find("$(CurrDate)"  )) >= 0 ) szArgument = szArgument.Left(nFound) + lpszCurrDate   + szArgument.Mid(nFound + 11);
	while( (nFound = szArgument.Find("$(CurrTime)"  )) >= 0 ) szArgument = szArgument.Left(nFound) + lpszCurrTime   + szArgument.Mid(nFound + 11);
}


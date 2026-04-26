#include "stdafx.h"
#include "cedtHeader.h"


void CCedtDoc::EmptySavedCompositionString()
{
	m_szSavedCompositionString = "";
	m_bCompositionStringSaved = FALSE;
}

void CCedtDoc::SaveCurrentCompositionString(INT nIdxY)
{
	CAnalyzedString & rLine = GetLineFromIdxY(nIdxY);
	m_szSavedCompositionString = rLine;
	m_bCompositionStringSaved = TRUE;
}

void CCedtDoc::RestoreCurrentCompositionString(INT nIdxY)
{
	if( ! m_bCompositionStringSaved ) return;
	CAnalyzedString & rLine = GetLineFromIdxY(nIdxY);
	rLine = m_szSavedCompositionString;
}

void CCedtDoc::InsertCompositionString(INT nIdxX, INT nIdxY, LPCTSTR lpszString)
{
	CAnalyzedString & rLine = GetLineFromIdxY(nIdxY);
	rLine.Insert(nIdxX, lpszString);
	SetModifiedFlag(TRUE);
}

void CCedtDoc::DeleteCompositionString(INT nIdxX, INT nIdxY, INT nLength)
{
	CAnalyzedString & rLine = GetLineFromIdxY(nIdxY);
	rLine.Delete(nIdxX, nLength);
	SetModifiedFlag(TRUE);
}

void CCedtDoc::InsertChar(INT nIdxX, INT nIdxY, UINT nChar)
{
	CAnalyzedString & rLine = GetLineFromIdxY(nIdxY);
	FastInsertChar(rLine, nIdxX, nIdxY, nChar);
}

void CCedtDoc::DeleteChar(INT nIdxX, INT nIdxY)
{
	CAnalyzedString & rLine = GetLineFromIdxY(nIdxY);
	FastDeleteChar(rLine, nIdxX, nIdxY);
}

void CCedtDoc::InsertString(INT nIdxX, INT nIdxY, LPCTSTR lpszString)
{
	CAnalyzedString & rLine = GetLineFromIdxY(nIdxY);
	FastInsertString(rLine, nIdxX, nIdxY, lpszString);
}

void CCedtDoc::DeleteString(INT nIdxX, INT nIdxY, INT nLength)
{
	CAnalyzedString & rLine = GetLineFromIdxY(nIdxY);
	FastDeleteString(rLine, nIdxX, nIdxY, nLength);
}


void CCedtDoc::InsertBlock(INT nBegX, INT nBegY, INT & nEndX, INT & nEndY, CMemText & rBlock)
{
	CAnalyzedString & rLineBeg = GetLineFromIdxY(nBegY);
	CString szSplit = rLineBeg.Mid(nBegX);
	POSITION pos = m_clsAnalyzedText.FindIndex(nBegY);
	POSITION po2 = rBlock.GetHeadPosition();
	rLineBeg = rLineBeg.Left(nBegX) + rBlock.GetNext(po2);
	for(INT i = 1; i < rBlock.GetCount(); i++) pos = m_clsAnalyzedText.InsertAfter(pos, rBlock.GetNext(po2));
	CAnalyzedString & rLineEnd = m_clsAnalyzedText.GetAt(pos);
	rLineEnd += szSplit;

	nEndY = nBegY + rBlock.GetCount() - 1;
	nEndX = rLineEnd.GetLength() - szSplit.GetLength();

	RecordInsertBlock(nBegX, nBegY, nEndX, nEndY);
	SetModifiedFlag(TRUE);
}

void CCedtDoc::DeleteBlock(INT nBegX, INT nBegY, INT nEndX, INT nEndY)
{
	CMemText Block; CopyToBlock(Block, nBegX, nBegY, nEndX, nEndY);

	CAnalyzedString & rLineBeg = GetLineFromIdxY(nBegY);
	CAnalyzedString & rLineEnd = GetLineFromIdxY(nEndY);
	rLineBeg = rLineBeg.Left(nBegX) + rLineEnd.Mid(nEndX);
	POSITION pos = m_clsAnalyzedText.FindIndex(nBegY+1);
	for(INT i = nBegY+1; i <= nEndY; i++) {
		POSITION posRemove = pos; m_clsAnalyzedText.GetNext(pos);
		m_clsAnalyzedText.RemoveAt(posRemove);
	}

	RecordDeleteBlock(nBegX, nBegY, Block);
	SetModifiedFlag(TRUE);
}

void CCedtDoc::SplitLine(INT nIdxX, INT nIdxY)
{
	CAnalyzedString & rLine = GetLineFromIdxY(nIdxY);
	CString szString = rLine.Mid(nIdxX); rLine = rLine.Left(nIdxX);
	POSITION pos = m_clsAnalyzedText.FindIndex(nIdxY);
	m_clsAnalyzedText.InsertAfter(pos, szString);

	RecordSplitLine(nIdxX, nIdxY);
	SetModifiedFlag(TRUE);
}

void CCedtDoc::JoinLines(INT nIdxX, INT nIdxY)
{
	CAnalyzedString & rLine = GetLineFromIdxY(nIdxY);
	rLine = rLine + GetLineFromIdxY(nIdxY+1);
	POSITION pos = m_clsAnalyzedText.FindIndex(nIdxY+1);
	m_clsAnalyzedText.RemoveAt(pos);

	RecordJoinLines(nIdxX, nIdxY);
	SetModifiedFlag(TRUE);
}


void CCedtDoc::CopyToString(CString & rString, INT nBegX, INT nBegY, INT nLength)
{
	CAnalyzedString & rLine = GetLineFromIdxY(nBegY);
	rString = rLine.Mid(nBegX, nLength);
}

void CCedtDoc::CopyToBlock(CMemText & rBlock, INT nBegX, INT nBegY, INT nEndX, INT nEndY)
{
	rBlock.RemoveAll();
	POSITION pos = m_clsAnalyzedText.FindIndex(nBegY);
	rBlock.AddTail( m_clsAnalyzedText.GetNext(pos).Mid(nBegX) );
	for(INT i = nBegY+1; i < nEndY; i++) rBlock.AddTail( m_clsAnalyzedText.GetNext(pos) );
	rBlock.AddTail( m_clsAnalyzedText.GetNext(pos).Left(nEndX) );
}


void CCedtDoc::FastInsertChar(CAnalyzedString & rLine, INT nIdxX, INT nIdxY, UINT nChar)
{
	rLine.Insert(nIdxX, nChar);
	RecordInsertChar(nIdxX, nIdxY);
	SetModifiedFlag(TRUE);
}

void CCedtDoc::FastDeleteChar(CAnalyzedString & rLine, INT nIdxX, INT nIdxY)
{
	INT nChar = rLine[nIdxX]; 
	rLine.Delete(nIdxX, 1);

	RecordDeleteChar(nIdxX, nIdxY, nChar);
	SetModifiedFlag(TRUE);
}

void CCedtDoc::FastInsertString(CAnalyzedString & rLine, INT nIdxX, INT nIdxY, LPCTSTR lpszString)
{
	INT nLength = strlen(lpszString);
	rLine.Insert(nIdxX, lpszString);

	RecordInsertString(nIdxX, nIdxY, nLength);
	SetModifiedFlag(TRUE);
}

void CCedtDoc::FastDeleteString(CAnalyzedString & rLine, INT nIdxX, INT nIdxY, INT nLength)
{
	CString szString = rLine.Mid(nIdxX, nLength);
	rLine.Delete(nIdxX, nLength);

	RecordDeleteString(nIdxX, nIdxY, szString);
	SetModifiedFlag(TRUE);
}

#include "stdafx.h"
#include "cedtHeader.h"



void CCedtView::ActionUndoLastAction()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	INT nAction, nIdxX, nIdxY, nEndX, nEndY;
	UINT nChar; INT nLength; CString szString; CMemText Block; 

	INT nCount = pDoc->PopUndoBuffer();
	while( nCount-- ) {
		nAction = pDoc->PopUndoAction();
		nIdxX = pDoc->PopUndoIdxX();
		nIdxY = pDoc->PopUndoIdxY();

		switch( nAction ) {
		case AT_INSERTCHAR:
			DeleteChar(nIdxX, nIdxY);
			break;

		case AT_DELETECHAR:
			nChar = pDoc->PopUndoChar();
			InsertChar(nIdxX, nIdxY, nChar);
			nIdxX = nIdxX + 1;
			break;

		case AT_INSERTSTRING:
			nLength = pDoc->PopUndoParam();
			DeleteString(nIdxX, nIdxY, nLength);
			break;

		case AT_DELETESTRING:
			szString = pDoc->PopUndoString();
			InsertString(nIdxX, nIdxY, szString);
			nIdxX = nIdxX + szString.GetLength();
			break;

		case AT_INSERTBLOCK:
			nEndX = pDoc->PopUndoIdxX();
			nEndY = pDoc->PopUndoIdxY();
			DeleteLineSelection(nIdxX, nIdxY, nEndX, nEndY);
			break;

		case AT_DELETEBLOCK:
			Block = pDoc->PopUndoBlock();
			InsertLineSelection(nIdxX, nIdxY, nEndX, nEndY, Block);
			nIdxX = nEndX; nIdxY = nEndY;
			break;

		case AT_SPLITLINE:
			JoinLines(nIdxX, nIdxY);
			break;

		case AT_JOINLINES:
			SplitLine(nIdxX, nIdxY);
			break;
		}
	}

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX ) );
}

void CCedtView::ActionRedoLastUndo()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	INT nAction, nIdxX, nIdxY, nEndX, nEndY;
	UINT nChar; INT nLength; CString szString; CMemText Block; 

	INT nCount = pDoc->PopRedoBuffer();
	while( nCount-- ) {
		nAction = pDoc->PopRedoAction();
		nIdxX = pDoc->PopRedoIdxX();
		nIdxY = pDoc->PopRedoIdxY();

		switch( nAction ) {
		case AT_INSERTCHAR:
			DeleteChar(nIdxX, nIdxY);
			break;

		case AT_DELETECHAR:
			nChar = pDoc->PopRedoChar();
			InsertChar(nIdxX, nIdxY, nChar);
			nIdxX = nIdxX + 1;
			break;

		case AT_INSERTSTRING:
			nLength = pDoc->PopRedoParam();
			DeleteString(nIdxX, nIdxY, nLength);
			break;

		case AT_DELETESTRING:
			szString = pDoc->PopRedoString();
			InsertString(nIdxX, nIdxY, szString);
			nIdxX = nIdxX + szString.GetLength();
			break;

		case AT_INSERTBLOCK:
			nEndX = pDoc->PopRedoIdxX();
			nEndY = pDoc->PopRedoIdxY();
			DeleteLineSelection(nIdxX, nIdxY, nEndX, nEndY);
			break;

		case AT_DELETEBLOCK:
			Block = pDoc->PopRedoBlock();
			InsertLineSelection(nIdxX, nIdxY, nEndX, nEndY, Block);
			nIdxX = nEndX; nIdxY = nEndY;
			break;

		case AT_SPLITLINE:
			JoinLines(nIdxX, nIdxY);
			break;

		case AT_JOINLINES:
			SplitLine(nIdxX, nIdxY);
			break;
		}
	}

	SetCaretPosY( GetPosYFromIdxY( nIdxX, nIdxY ) );
	CFormatedString & rLne2 = GetLineFromPosY( m_nCaretPosY );
	SetCaretPosX( GetPosXFromIdxX( rLne2, nIdxX ) );
}

INT CCedtView::GetUndoBufferCount()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->GetUndoBufferCount();
}

INT CCedtView::GetRedoBufferCount()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	return pDoc->GetRedoBufferCount();
}

void CCedtView::EmptyUndoBuffer()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->EmptyUndoBuffer();
}

void CCedtView::EmptyRedoBuffer()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->EmptyRedoBuffer();
}

void CCedtView::GetLastEditingIndex(INT & nIdxX, INT & nIdxY)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->GetLastEditingIndex(nIdxX, nIdxY);
}

void CCedtView::CheckIfAllActionsCanBeUndone()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->CheckIfAllActionsCanBeUndone();
}

void CCedtView::CheckIfAllActionsAreUndone()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->CheckIfAllActionsAreUndone();
}

void CCedtView::BeginActionRecording(BOOL bRecordingUndo)
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->BeginActionRecording(bRecordingUndo);
}

void CCedtView::EndActionRecording()
{
	CCedtDoc * pDoc = (CCedtDoc *)GetDocument();
	pDoc->EndActionRecording();
}


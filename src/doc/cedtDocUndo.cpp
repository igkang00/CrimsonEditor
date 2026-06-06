#include "stdafx.h"
#include "cedtHeader.h"



void CCedtDoc::BeginActionRecording(BOOL bRecordingUndo)
{
	m_nRecordingCount = 0;
	m_bRecordingUndo = bRecordingUndo;
}

void CCedtDoc::EndActionRecording()
{
	if( m_nRecordingCount == 0 ) return;
	if( m_bRecordingUndo ) m_clsUndoBuffer.AddHead( m_nRecordingCount );
	else m_clsRedoBuffer.AddHead( m_nRecordingCount );
}


void CCedtDoc::RecordInsertChar(INT nIdxX, INT nIdxY)
{
	if( m_bRecordingUndo ) {
		m_clsUndoBuffer.m_lstAction.AddHead( AT_INSERTCHAR );
		m_clsUndoBuffer.m_lstIdxX.AddHead( nIdxX );
		m_clsUndoBuffer.m_lstIdxY.AddHead( nIdxY );
	} else {
		m_clsRedoBuffer.m_lstAction.AddHead( AT_INSERTCHAR );
		m_clsRedoBuffer.m_lstIdxX.AddHead( nIdxX );
		m_clsRedoBuffer.m_lstIdxY.AddHead( nIdxY );
	}
	m_nRecordingCount++;
}

void CCedtDoc::RecordDeleteChar(INT nIdxX, INT nIdxY, UINT nChar)
{
	if( m_bRecordingUndo ) {
		m_clsUndoBuffer.m_lstAction.AddHead( AT_DELETECHAR );
		m_clsUndoBuffer.m_lstIdxX.AddHead( nIdxX );
		m_clsUndoBuffer.m_lstIdxY.AddHead( nIdxY );
		m_clsUndoBuffer.m_lstChar.AddHead( nChar );
	} else {
		m_clsRedoBuffer.m_lstAction.AddHead( AT_DELETECHAR );
		m_clsRedoBuffer.m_lstIdxX.AddHead( nIdxX );
		m_clsRedoBuffer.m_lstIdxY.AddHead( nIdxY );
		m_clsRedoBuffer.m_lstChar.AddHead( nChar );
	}
	m_nRecordingCount++;
}

void CCedtDoc::RecordInsertString(INT nIdxX, INT nIdxY, INT nLength)
{
	if( m_bRecordingUndo ) {
		m_clsUndoBuffer.m_lstAction.AddHead( AT_INSERTSTRING );
		m_clsUndoBuffer.m_lstIdxX.AddHead( nIdxX );
		m_clsUndoBuffer.m_lstIdxY.AddHead( nIdxY );
		m_clsUndoBuffer.m_lstParam.AddHead( nLength );
	} else {
		m_clsRedoBuffer.m_lstAction.AddHead( AT_INSERTSTRING );
		m_clsRedoBuffer.m_lstIdxX.AddHead( nIdxX );
		m_clsRedoBuffer.m_lstIdxY.AddHead( nIdxY );
		m_clsRedoBuffer.m_lstParam.AddHead( nLength );
	}
	m_nRecordingCount++;
}

void CCedtDoc::RecordDeleteString(INT nIdxX, INT nIdxY, LPCTSTR lpszString)
{
	if( m_bRecordingUndo ) {
		m_clsUndoBuffer.m_lstAction.AddHead( AT_DELETESTRING );
		m_clsUndoBuffer.m_lstIdxX.AddHead( nIdxX );
		m_clsUndoBuffer.m_lstIdxY.AddHead( nIdxY );
		m_clsUndoBuffer.m_lstString.AddHead( lpszString );
	} else {
		m_clsRedoBuffer.m_lstAction.AddHead( AT_DELETESTRING );
		m_clsRedoBuffer.m_lstIdxX.AddHead( nIdxX );
		m_clsRedoBuffer.m_lstIdxY.AddHead( nIdxY );
		m_clsRedoBuffer.m_lstString.AddHead( lpszString );
	}
	m_nRecordingCount++;
}

void CCedtDoc::RecordInsertBlock(INT nBegX, INT nBegY, INT nEndX, INT nEndY)
{
	if( m_bRecordingUndo ) {
		m_clsUndoBuffer.m_lstAction.AddHead( AT_INSERTBLOCK );
		m_clsUndoBuffer.m_lstIdxX.AddHead( nEndX );
		m_clsUndoBuffer.m_lstIdxY.AddHead( nEndY );
		m_clsUndoBuffer.m_lstIdxX.AddHead( nBegX );
		m_clsUndoBuffer.m_lstIdxY.AddHead( nBegY );
	} else {
		m_clsRedoBuffer.m_lstAction.AddHead( AT_INSERTBLOCK );
		m_clsRedoBuffer.m_lstIdxX.AddHead( nEndX );
		m_clsRedoBuffer.m_lstIdxY.AddHead( nEndY );
		m_clsRedoBuffer.m_lstIdxX.AddHead( nBegX );
		m_clsRedoBuffer.m_lstIdxY.AddHead( nBegY );
	}
	m_nRecordingCount++;
}

void CCedtDoc::RecordDeleteBlock(INT nBegX, INT nBegY, CMemText & rBlock)
{
	if( m_bRecordingUndo ) {
		m_clsUndoBuffer.m_lstAction.AddHead( AT_DELETEBLOCK );
		m_clsUndoBuffer.m_lstIdxX.AddHead( nBegX );
		m_clsUndoBuffer.m_lstIdxY.AddHead( nBegY );
		m_clsUndoBuffer.m_lstBlock.AddHead( rBlock );
	} else {
		m_clsRedoBuffer.m_lstAction.AddHead( AT_DELETEBLOCK );
		m_clsRedoBuffer.m_lstIdxX.AddHead( nBegX );
		m_clsRedoBuffer.m_lstIdxY.AddHead( nBegY );
		m_clsRedoBuffer.m_lstBlock.AddHead( rBlock );
	}
	m_nRecordingCount++;
}

void CCedtDoc::RecordSplitLine(INT nIdxX, INT nIdxY)
{
	if( m_bRecordingUndo ) {
		m_clsUndoBuffer.m_lstAction.AddHead( AT_SPLITLINE );
		m_clsUndoBuffer.m_lstIdxX.AddHead( nIdxX );
		m_clsUndoBuffer.m_lstIdxY.AddHead( nIdxY );
	} else {
		m_clsRedoBuffer.m_lstAction.AddHead( AT_SPLITLINE );
		m_clsRedoBuffer.m_lstIdxX.AddHead( nIdxX );
		m_clsRedoBuffer.m_lstIdxY.AddHead( nIdxY );
	}
	m_nRecordingCount++;
}

void CCedtDoc::RecordJoinLines(INT nIdxX, INT nIdxY)
{
	if( m_bRecordingUndo ) {
		m_clsUndoBuffer.m_lstAction.AddHead( AT_JOINLINES );
		m_clsUndoBuffer.m_lstIdxX.AddHead( nIdxX );
		m_clsUndoBuffer.m_lstIdxY.AddHead( nIdxY );
	} else {
		m_clsRedoBuffer.m_lstAction.AddHead( AT_JOINLINES );
		m_clsRedoBuffer.m_lstIdxX.AddHead( nIdxX );
		m_clsRedoBuffer.m_lstIdxY.AddHead( nIdxY );
	}
	m_nRecordingCount++;
}

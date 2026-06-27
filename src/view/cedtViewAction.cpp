#include "stdafx.h"
#include "cedtHeader.h"
#include "Evaluate.h"
#include <math.h>


void CCedtView::ActionWrongOperation(BOOL bBeep)
{
	if( bBeep ) MessageBeep(MB_ICONEXCLAMATION);
}

void CCedtView::ActionEvaluateLine()
{
	INT nIdxX, nIdxY; PositionToIndex( m_nCaretPosX, m_nCaretPosY, nIdxX, nIdxY );
	CAnalyzedString & rString = GetLineFromIdxY( nIdxY );
	// Pass the line straight to the evaluator instead of copying it into a
	// fixed-size stack buffer. Lines up to MAX_STRING_LENGTH (32767) are valid
	// in this editor, so a 2048-byte buffer would overrun on long lines.
	TCHAR * pFormula = const_cast<TCHAR *>((LPCTSTR)rString);

	CString szResult; double dValue; INT nError;
	TCHAR * pExpr = EVAL::Evaluate( pFormula, & dValue, & nError );

	if( nError == EVAL_ERROR_SUCCESSFUL ) {
		double dFraction, dInteger; dFraction = modf( dValue, & dInteger );
		if( dFraction == 0.0 ) szResult.Format("$ans = %.0f", dValue);
		else szResult.Format("$ans = %f", dValue);
	} else szResult.Format("error(%d): %s", pExpr - pFormula + 1, EVAL::GetErrorMessage(nError));

	SplitLine(GetLastIdxX(rString), nIdxY);
	nIdxY = nIdxY + 1; nIdxX = 0;
	InsertString(nIdxX, nIdxY, szResult);
	nIdxX = szResult.GetLength();

	INT nPosX, nPosY; IndexToPosition( nIdxX, nIdxY, nPosX, nPosY );
	SetCaretPosY( nPosY ); SetCaretPosX( nPosX );
}

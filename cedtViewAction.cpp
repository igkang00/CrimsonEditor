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
	TCHAR szFormula[2048]; strcpy(szFormula, rString);

	CString szResult; double dValue; INT nError;
	TCHAR * pExpr = EVAL::Evaluate( szFormula, & dValue, & nError );

	if( nError == EVAL_ERROR_SUCCESSFUL ) {
		double dFraction, dInteger; dFraction = modf( dValue, & dInteger );
		if( dFraction == 0.0 ) szResult.Format("$ans = %.0f", dValue);
		else szResult.Format("$ans = %f", dValue);
	} else szResult.Format("error(%d): %s", pExpr - szFormula + 1, EVAL::GetErrorMessage(nError));

	SplitLine(GetLastIdxX(rString), nIdxY);
	nIdxY = nIdxY + 1; nIdxX = 0;
	InsertString(nIdxX, nIdxY, szResult);
	nIdxX = szResult.GetLength();

	INT nPosX, nPosY; IndexToPosition( nIdxX, nIdxY, nPosX, nPosY );
	SetCaretPosY( nPosY ); SetCaretPosX( nPosX );
}

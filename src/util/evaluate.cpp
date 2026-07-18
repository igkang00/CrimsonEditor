#include "stdafx.h"
#include <math.h>
#include "date.h"
#include "evaluate.h"


#define EVAL_MAX_ARGUMENT_COUNT		16


/* evaluate variables */
#define EVAL_VARIABLE_PI			3.1415926535


/* evaluate functions */
#define EVAL_FUNCTION_ABS			0x01
#define EVAL_FUNCTION_MOD			0x02
#define EVAL_FUNCTION_CEIL			0x03
#define EVAL_FUNCTION_FLOOR			0x04
#define EVAL_FUNCTION_ROUND			0x05
#define EVAL_FUNCTION_MIN			0x06
#define EVAL_FUNCTION_MAX			0x07

#define EVAL_FUNCTION_ACOS			0x08
#define EVAL_FUNCTION_ASIN			0x09
#define EVAL_FUNCTION_ATAN			0x0A
#define EVAL_FUNCTION_ATAN2			0x0B
#define EVAL_FUNCTION_COS			0x0C
#define EVAL_FUNCTION_SIN			0x0D
#define EVAL_FUNCTION_TAN			0x0E
#define EVAL_FUNCTION_COSH			0x0F
#define EVAL_FUNCTION_SINH			0x10
#define EVAL_FUNCTION_TANH			0x11

#define EVAL_FUNCTION_EXP			0x12
#define EVAL_FUNCTION_LOG			0x13
#define EVAL_FUNCTION_LOG10			0x14
#define EVAL_FUNCTION_POW			0x15
#define EVAL_FUNCTION_SQR			0x16
#define EVAL_FUNCTION_SQRT			0x17

#define EVAL_FUNCTION_TODAY			0x18
#define EVAL_FUNCTION_YEARDAYS		0x19
#define EVAL_FUNCTION_MONTHDAYS		0x20
#define EVAL_FUNCTION_DATE2DAYS		0x21
#define EVAL_FUNCTION_DAYS2DATE		0x22
#define EVAL_FUNCTION_EOMDAY		0x23
#define EVAL_FUNCTION_EOMDATE		0x24
#define EVAL_FUNCTION_WEEKDAY		0x25

#define EVAL_FUNCTION_ISBIZDATE		0x26
#define EVAL_FUNCTION_NBIZDATE		0x27
#define EVAL_FUNCTION_PBIZDATE		0x28
#define EVAL_FUNCTION_ADDDAYS		0x29
#define EVAL_FUNCTION_ADDMONTHS		0x30
#define EVAL_FUNCTION_ADDTERMS		0x31

#define EVAL_FUNCTION_DAYS360		0x32
#define EVAL_FUNCTION_DAYS365		0x33
#define EVAL_FUNCTION_DAYSACT		0x34
#define EVAL_FUNCTION_DAYSBET		0x35
#define EVAL_FUNCTION_MONTHSBET		0x36
#define EVAL_FUNCTION_TERMSBET		0x37
#define EVAL_FUNCTION_TERMFRAC		0x38
#define EVAL_FUNCTION_YEARFRAC		0x39


/* evaluate macros */
#define EVAL_EAT_WHITE(p)		while( * p == ' ' || * p == '\t' ) p++;


namespace EVAL {
	TCHAR szErrorMessage[][64] = {
		_T("successful"),
		_T("internal error"),
		_T("wrong syntax"),
		_T("variable not defined"),
		_T("function not defined"),
		_T("function argument count"),
		_T("token too long"),
	};

	CMap<CString, LPCTSTR, double, double> hashVariables;
	CMap<CString, LPCTSTR, INT, INT> hashFunctions;

	void Initialize();

	TCHAR * EvalExpression(TCHAR * pExpr, double * pValue, INT * pError);
	TCHAR * EvalTerm(TCHAR * pExpr, double * pValue, INT * pError);
	TCHAR * EvalSignedFactor(TCHAR * pExpr, double * pValue, INT * pError);
	TCHAR * EvalFactor(TCHAR * pExpr, double * pValue, INT * pError);
	TCHAR * EvalConstant(TCHAR * pExpr, double * pValue, INT * pError);
	TCHAR * EvalParentheses(TCHAR * pExpr, double * pValue, INT * pError);
	TCHAR * EvalVariable(TCHAR * pExpr, double * pValue, INT * pError);
	TCHAR * EvalFunction(TCHAR * pExpr, double * pValue, INT * pError);

	BOOL EvalFunction(INT nFunction, INT argc, double * argv, double * pValue);
};


void EVAL::SetVariable(TCHAR * pVarName, double dValue)
{
}

void EVAL::GetVariable(TCHAR * pVarName, double * pValue, INT * pError)
{
	* pError = EVAL_ERROR_SUCCESSFUL;
}

TCHAR * EVAL::Evaluate(TCHAR * pExpr, double * pValue, INT * pError)
{
	static BOOL bInitialized = FALSE;

	if( ! bInitialized ) {
		Initialize();
		bInitialized = TRUE;
	}

	* pValue = 0.0;
	* pError = EVAL_ERROR_SUCCESSFUL;

	EVAL_EAT_WHITE( pExpr );

	pExpr = EvalExpression( pExpr, pValue, pError );

	if( * pError == EVAL_ERROR_SUCCESSFUL && * pExpr != '\0' ) * pError = EVAL_ERROR_WRONG_SYNTAX;
	if( * pError == EVAL_ERROR_SUCCESSFUL ) hashVariables.SetAt(_T("ans"), * pValue);

	return pExpr;
}

TCHAR * EVAL::GetErrorMessage(INT nError)
{
	return szErrorMessage[nError];
}

void EVAL::Initialize()
{
	hashVariables.InitHashTable(30);

	hashVariables.SetAt(_T("ans"),			0.0);
	hashVariables.SetAt(_T("pi"),			EVAL_VARIABLE_PI);

	hashVariables.SetAt(_T("annual"),		FRE_ANNUAL);
	hashVariables.SetAt(_T("semiannual"),	FRE_SEMIANNUAL);
	hashVariables.SetAt(_T("quarterly"),	FRE_QUARTERLY);
	hashVariables.SetAt(_T("bimonthly"),	FRE_BIMONTHLY);
	hashVariables.SetAt(_T("monthly"),		FRE_MONTHLY);

	hashVariables.SetAt(_T("actact"),		DCB_ACTACT);
	hashVariables.SetAt(_T("act360"),		DCB_ACT360);
	hashVariables.SetAt(_T("act365"),		DCB_ACT365);
	hashVariables.SetAt(_T("30360"),		DCB_30360);
	hashVariables.SetAt(_T("30e360"),		DCB_30E360);
	hashVariables.SetAt(_T("nl365"),		DCB_NL365);

	hashVariables.SetAt(_T("actual"),		EMR_ACTUAL);
	hashVariables.SetAt(_T("endofmonth"),	EMR_ENDOFMONTH);
	hashVariables.SetAt(_T("sunday"),		HDC_SUNDAY);
	hashVariables.SetAt(_T("sunsat"),		HDC_SUNSAT);

	hashFunctions.InitHashTable(100);

	hashFunctions.SetAt(_T("abs"),			EVAL_FUNCTION_ABS);
	hashFunctions.SetAt(_T("fabs"),			EVAL_FUNCTION_ABS);
	hashFunctions.SetAt(_T("mod"),			EVAL_FUNCTION_MOD);
	hashFunctions.SetAt(_T("fmod"),			EVAL_FUNCTION_MOD);
	hashFunctions.SetAt(_T("ceil"),			EVAL_FUNCTION_CEIL);
	hashFunctions.SetAt(_T("floor"),		EVAL_FUNCTION_FLOOR);
	hashFunctions.SetAt(_T("round"),		EVAL_FUNCTION_ROUND);
	hashFunctions.SetAt(_T("min"),			EVAL_FUNCTION_MIN);
	hashFunctions.SetAt(_T("max"),			EVAL_FUNCTION_MAX);

	hashFunctions.SetAt(_T("acos"),			EVAL_FUNCTION_ACOS);
	hashFunctions.SetAt(_T("asin"),			EVAL_FUNCTION_ASIN);
	hashFunctions.SetAt(_T("atan"),			EVAL_FUNCTION_ATAN);
	hashFunctions.SetAt(_T("atan2"),		EVAL_FUNCTION_ATAN2);
	hashFunctions.SetAt(_T("cos"),			EVAL_FUNCTION_COS);
	hashFunctions.SetAt(_T("sin"),			EVAL_FUNCTION_SIN);
	hashFunctions.SetAt(_T("tan"),			EVAL_FUNCTION_TAN);
	hashFunctions.SetAt(_T("cosh"),			EVAL_FUNCTION_COSH);
	hashFunctions.SetAt(_T("sinh"),			EVAL_FUNCTION_SINH);
	hashFunctions.SetAt(_T("tanh"),			EVAL_FUNCTION_TANH);

	hashFunctions.SetAt(_T("exp"),			EVAL_FUNCTION_EXP);
	hashFunctions.SetAt(_T("log"),			EVAL_FUNCTION_LOG);
	hashFunctions.SetAt(_T("log10"),		EVAL_FUNCTION_LOG10);
	hashFunctions.SetAt(_T("pow"),			EVAL_FUNCTION_POW);
	hashFunctions.SetAt(_T("sqr"),			EVAL_FUNCTION_SQR);
	hashFunctions.SetAt(_T("sqrt"),			EVAL_FUNCTION_SQRT);

	hashFunctions.SetAt(_T("today"),		EVAL_FUNCTION_TODAY);
	hashFunctions.SetAt(_T("yeardays"),		EVAL_FUNCTION_YEARDAYS);
	hashFunctions.SetAt(_T("monthdays"),	EVAL_FUNCTION_MONTHDAYS);
	hashFunctions.SetAt(_T("date2days"),	EVAL_FUNCTION_DATE2DAYS);
	hashFunctions.SetAt(_T("days2date"),	EVAL_FUNCTION_DAYS2DATE);
	hashFunctions.SetAt(_T("eomday"),		EVAL_FUNCTION_EOMDAY);
	hashFunctions.SetAt(_T("eomdate"),		EVAL_FUNCTION_EOMDATE);
	hashFunctions.SetAt(_T("weekday"),		EVAL_FUNCTION_WEEKDAY);

	hashFunctions.SetAt(_T("isbizdate"),	EVAL_FUNCTION_ISBIZDATE);
	hashFunctions.SetAt(_T("nbizdate"),		EVAL_FUNCTION_NBIZDATE);
	hashFunctions.SetAt(_T("pbizdate"),		EVAL_FUNCTION_PBIZDATE);
	hashFunctions.SetAt(_T("adddays"),		EVAL_FUNCTION_ADDDAYS);
	hashFunctions.SetAt(_T("addmonths"),	EVAL_FUNCTION_ADDMONTHS);
	hashFunctions.SetAt(_T("addterms"),		EVAL_FUNCTION_ADDTERMS);

	hashFunctions.SetAt(_T("days360"),		EVAL_FUNCTION_DAYS360);
	hashFunctions.SetAt(_T("days365"),		EVAL_FUNCTION_DAYS365);
	hashFunctions.SetAt(_T("daysact"),		EVAL_FUNCTION_DAYSACT);
	hashFunctions.SetAt(_T("daysbet"),		EVAL_FUNCTION_DAYSBET);
	hashFunctions.SetAt(_T("monthsbet"),	EVAL_FUNCTION_MONTHSBET);
	hashFunctions.SetAt(_T("termsbet"),		EVAL_FUNCTION_TERMSBET);
	hashFunctions.SetAt(_T("termfrac"),		EVAL_FUNCTION_TERMFRAC);
	hashFunctions.SetAt(_T("yearfrac"),		EVAL_FUNCTION_YEARFRAC);
}

TCHAR * EVAL::EvalExpression(TCHAR * pExpr, double * pValue, INT * pError)
{
	EVAL_EAT_WHITE( pExpr );
	pExpr = EvalTerm( pExpr, pValue, pError );
	if( * pError ) return pExpr;

	EVAL_EAT_WHITE( pExpr );
	TCHAR op = * pExpr;

	while( op == '+' || op == '-' ) {
		pExpr++; // skip operator
		double value;

		EVAL_EAT_WHITE( pExpr );
		pExpr = EvalTerm( pExpr, & value, pError );
		if( * pError ) return pExpr;

		if( op == '+' ) * pValue += value;
		if( op == '-' ) * pValue -= value;

		EVAL_EAT_WHITE( pExpr );
		op = * pExpr;
	}

	return pExpr;
}

TCHAR * EVAL::EvalTerm(TCHAR * pExpr, double * pValue, INT * pError)
{
	EVAL_EAT_WHITE( pExpr );
	pExpr = EvalSignedFactor( pExpr, pValue, pError );
	if( * pError ) return pExpr;

	EVAL_EAT_WHITE( pExpr );
	TCHAR op = * pExpr;

	while( op == '*' || op == '/' || op == '%' ) {
		pExpr++; // skip operator
		double value;

		EVAL_EAT_WHITE( pExpr );
		pExpr = EvalFactor( pExpr, & value, pError );
		if( * pError ) return pExpr;

		if( op == '*' ) * pValue *= value;
		if( op == '/' ) * pValue /= value;
		if( op == '%' ) * pValue  = fmod(* pValue, value);

		EVAL_EAT_WHITE( pExpr );
		op = * pExpr;
	}

	return pExpr;
}

TCHAR * EVAL::EvalSignedFactor(TCHAR * pExpr, double * pValue, INT * pError)
{
	EVAL_EAT_WHITE( pExpr );
	TCHAR op = * pExpr;

	if( op == '+' || op == '-' ) pExpr++; // skip operator

	EVAL_EAT_WHITE( pExpr );
	pExpr = EvalFactor( pExpr, pValue, pError );
	if( * pError ) return pExpr;

	if( op == '-' ) * pValue = - (* pValue);

	return pExpr;
}

TCHAR * EVAL::EvalFactor(TCHAR * pExpr, double * pValue, INT * pError)
{
	EVAL_EAT_WHITE( pExpr );

	// A factor is a required operand. Hitting end-of-string here means one is missing
	// (e.g. "1+", "1++", "1*", or empty input) — a syntax error, not a silent success.
	// Returning without an error used to leave *pValue as an uninitialised double that the
	// caller then added in, so "1++" came out as 1 by luck.
	if     ( * pExpr == '\0'  ) { * pError = EVAL_ERROR_WRONG_SYNTAX; return pExpr; }
	else if( _istdigit(* pExpr) ) return EvalConstant( pExpr, pValue, pError );
	else if( * pExpr == '('   ) return EvalParentheses( pExpr, pValue, pError );
	else if( * pExpr == '$'   ) return EvalVariable( pExpr, pValue, pError );
	else if( _istalpha(* pExpr) ) return EvalFunction( pExpr, pValue, pError );
	else   /* other cases    */ { * pError = EVAL_ERROR_WRONG_SYNTAX; return pExpr; }
}

TCHAR * EVAL::EvalConstant(TCHAR * pExpr, double * pValue, INT * pError)
{
	EVAL_EAT_WHITE( pExpr );

	TCHAR * pEnd = pExpr;
	while( _istdigit(* pEnd) || * pEnd == '.' ) pEnd++;

	TCHAR szNum[2048]; INT nLen = (INT)(pEnd - pExpr);
	if( nLen <= 0 ) { * pError = EVAL_ERROR_WRONG_SYNTAX; return pExpr; }
	// A token longer than the buffer would overrun the stack. Reject it as an error rather
	// than truncate and return a wrong value: the caller shows the error (and beeps).
	if( nLen > (INT)(sizeof(szNum)/sizeof(TCHAR)) - 1 ) { * pError = EVAL_ERROR_TOKEN_TOO_LONG; return pExpr; }
	_tcsncpy( szNum, pExpr, nLen ); szNum[nLen] = '\0';

	* pValue = _tstof( szNum );
	pExpr = pEnd;

	return pExpr;
}

TCHAR * EVAL::EvalParentheses(TCHAR * pExpr, double * pValue, INT * pError)
{
	EVAL_EAT_WHITE( pExpr );
	if( * pExpr == '(' ) pExpr++; // skip '('
	else { * pError = EVAL_ERROR_INTERNAL; return pExpr; }

	EVAL_EAT_WHITE( pExpr );
	pExpr = EvalExpression( pExpr, pValue, pError );
	if( * pError ) return pExpr;

	EVAL_EAT_WHITE( pExpr );
	if( * pExpr == ')' ) pExpr++; // skip ')'
	else { * pError = EVAL_ERROR_WRONG_SYNTAX; return pExpr; }

	return pExpr;
}

TCHAR * EVAL::EvalVariable(TCHAR * pExpr, double * pValue, INT * pError)
{
	EVAL_EAT_WHITE( pExpr );
	if( * pExpr == '$' ) pExpr++; // skip '$'
	else { * pError = EVAL_ERROR_INTERNAL; return pExpr; }

	TCHAR * pEnd = pExpr;
	while( _istalnum(* pEnd) ) pEnd++;

	TCHAR szVar[2048]; INT nLen = (INT)(pEnd - pExpr);
	if( nLen <= 0 ) { * pError = EVAL_ERROR_WRONG_SYNTAX; return pExpr; }
	// Reject an over-long token rather than overrun the buffer (see EvalConstant).
	if( nLen > (INT)(sizeof(szVar)/sizeof(TCHAR)) - 1 ) { * pError = EVAL_ERROR_TOKEN_TOO_LONG; return pExpr; }
	_tcsncpy( szVar, pExpr, nLen ); szVar[nLen] = '\0'; _tcslwr(szVar);

	double dValue;
	if( hashVariables.Lookup( szVar, dValue ) ) { * pValue = dValue; pExpr = pEnd; }
	else { * pError = EVAL_ERROR_VARIABLE_NOT_DEFINED; return pExpr; }

	return pExpr;
}

TCHAR * EVAL::EvalFunction(TCHAR * pExpr, double * pValue, INT * pError)
{
	EVAL_EAT_WHITE( pExpr );

	TCHAR * pEnd = pExpr;
	while( _istalnum(* pEnd) ) pEnd++;

	TCHAR szFun[2048]; INT nLen = (INT)(pEnd - pExpr);
	if( nLen <= 0 ) { * pError = EVAL_ERROR_WRONG_SYNTAX; return pExpr; }
	// Reject an over-long token rather than overrun the buffer (see EvalConstant).
	if( nLen > (INT)(sizeof(szFun)/sizeof(TCHAR)) - 1 ) { * pError = EVAL_ERROR_TOKEN_TOO_LONG; return pExpr; }
	_tcsncpy( szFun, pExpr, nLen ); szFun[nLen] = '\0'; _tcslwr(szFun);

	INT nFunction;
	if( hashFunctions.Lookup( szFun, nFunction ) ) { pExpr = pEnd; }
	else { * pError = EVAL_ERROR_FUNCTION_NOT_DEFINED; return pExpr; }


	INT argc = 0; double argv[EVAL_MAX_ARGUMENT_COUNT];

	EVAL_EAT_WHITE( pExpr );
	if( * pExpr == '(' ) pExpr++; // skip '('
	else { * pError = EVAL_ERROR_WRONG_SYNTAX; return pExpr; }

	EVAL_EAT_WHITE( pExpr );
	if( * pExpr != ')' ) { // not a void parameter function
		pExpr = EvalExpression( pExpr, & (argv[argc++]), pError );
		if( * pError ) return pExpr;
	}

	EVAL_EAT_WHITE( pExpr );
	TCHAR op = * pExpr;

	while( op == ',' ) {
		pExpr++; // skip commma operator

		EVAL_EAT_WHITE( pExpr );
		pExpr = EvalExpression( pExpr, & (argv[argc++]), pError );
		if( * pError ) return pExpr;

		EVAL_EAT_WHITE( pExpr );
		op = * pExpr;
	}

	EVAL_EAT_WHITE( pExpr );
	if( * pExpr == ')' ) pExpr++; // skip ')'
	else { * pError = EVAL_ERROR_WRONG_SYNTAX; return pExpr; }

	if( EvalFunction( nFunction, argc, argv, pValue ) ) { }
	else { * pError = EVAL_ERROR_FUNCTION_ARGUMENT_COUNT; return pExpr; }

	return pExpr;
}

BOOL EVAL::EvalFunction(INT nFunction, INT argc, double * argv, double * pValue)
{
	switch( nFunction ) {

	case EVAL_FUNCTION_ABS:
		if( argc == 1 ) { * pValue = fabs( argv[0] ); return TRUE; }
		break;

	case EVAL_FUNCTION_MOD:
		if( argc == 2 ) { * pValue = fmod( argv[0], argv[1] ); return TRUE; }
		break;

	case EVAL_FUNCTION_CEIL:
		if( argc == 1 ) { * pValue = ceil( argv[0] ); return TRUE; }
		break;

	case EVAL_FUNCTION_FLOOR:
		if( argc == 1 ) { * pValue = floor( argv[0] ); return TRUE; }
		break;

	case EVAL_FUNCTION_ROUND:
		if( argc == 1 ) { * pValue = floor( argv[0] + 0.5 ); return TRUE; }
		break;

	case EVAL_FUNCTION_MIN:
		if( argc == 2 ) { * pValue = argv[0] < argv[1] ? argv[0] : argv[1]; return TRUE; }
		break;

	case EVAL_FUNCTION_MAX:
		if( argc == 2 ) { * pValue = argv[0] > argv[1] ? argv[0] : argv[1]; return TRUE; }
		break;

	case EVAL_FUNCTION_ACOS:
		if( argc == 1 ) { * pValue = acos(argv[0]); return TRUE; }
		break;

	case EVAL_FUNCTION_ASIN:
		if( argc == 1 ) { * pValue = asin(argv[0]); return TRUE; }
		break;

	case EVAL_FUNCTION_ATAN:
		if( argc == 1 ) { * pValue = atan(argv[0]); return TRUE; }
		break;

	case EVAL_FUNCTION_ATAN2:
		if( argc == 2 ) { * pValue = atan2(argv[0], argv[1]); return TRUE; }
		break;

	case EVAL_FUNCTION_COS:
		if( argc == 1 ) { * pValue = cos(argv[0]); return TRUE; }
		break;

	case EVAL_FUNCTION_SIN:
		if( argc == 1 ) { * pValue = sin(argv[0]); return TRUE; }
		break;

	case EVAL_FUNCTION_TAN:
		if( argc == 1 ) { * pValue = tan(argv[0]); return TRUE; }
		break;

	case EVAL_FUNCTION_COSH:
		if( argc == 1 ) { * pValue = cosh(argv[0]); return TRUE; }
		break;

	case EVAL_FUNCTION_SINH:
		if( argc == 1 ) { * pValue = sinh(argv[0]); return TRUE; }
		break;

	case EVAL_FUNCTION_TANH:
		if( argc == 1 ) { * pValue = tanh(argv[0]); return TRUE; }
		break;

	case EVAL_FUNCTION_EXP:
		if( argc == 1 ) { * pValue = exp(argv[0]); return TRUE; }
		break;

	case EVAL_FUNCTION_LOG:
		if( argc == 1 ) { * pValue = log(argv[0]); return TRUE; }
		break;

	case EVAL_FUNCTION_LOG10:
		if( argc == 1 ) { * pValue = log10(argv[0]); return TRUE; }
		break;

	case EVAL_FUNCTION_POW:
		if( argc == 2 ) { * pValue = pow(argv[0], argv[1]); return TRUE; }
		break;

	case EVAL_FUNCTION_SQR:
		if( argc == 1 ) { * pValue = argv[0] * argv[0]; return TRUE; }
		break;

	case EVAL_FUNCTION_SQRT:
		if( argc == 1 ) { * pValue = sqrt(argv[0]); return TRUE; }
		break;

	case EVAL_FUNCTION_TODAY:
		if( argc == 0 ) { * pValue = (double)today(); return TRUE; }
		break;

	case EVAL_FUNCTION_YEARDAYS:
		if( argc == 1 ) { * pValue = (double)yeardays((int)argv[0]); return TRUE; }
		break;

	case EVAL_FUNCTION_MONTHDAYS:
		if( argc == 2 ) { * pValue = (double)monthdays((int)argv[0], (int)argv[1]); return TRUE; }
		break;

	case EVAL_FUNCTION_DATE2DAYS:
		if( argc == 1 ) { * pValue = (double)date2days((int)argv[0]); return TRUE; }
		break;

	case EVAL_FUNCTION_DAYS2DATE:
		if( argc == 1 ) { * pValue = (double)days2date((int)argv[0]); return TRUE; }
		break;

	case EVAL_FUNCTION_EOMDAY:
		if( argc == 2 ) { * pValue = (double)eomday((int)argv[0], (int)argv[1]); return TRUE; }
		break;

	case EVAL_FUNCTION_EOMDATE:
		if( argc == 2 ) { * pValue = (double)eomdate((int)argv[0], (int)argv[1]); return TRUE; }
		break;

	case EVAL_FUNCTION_WEEKDAY:
		if( argc == 1 ) { * pValue = (double)weekday((int)argv[0]); return TRUE; }
		break;

	case EVAL_FUNCTION_ISBIZDATE:
		if( argc == 1 ) { * pValue = (double)isbizdate((int)argv[0], HDC_SUNSAT  ); return TRUE; }
		if( argc == 2 ) { * pValue = (double)isbizdate((int)argv[0], (int)argv[1]); return TRUE; }
		break;

	case EVAL_FUNCTION_NBIZDATE:
		if( argc == 1 ) { * pValue = (double)nbizdate((int)argv[0], HDC_SUNSAT  ); return TRUE; }
		if( argc == 2 ) { * pValue = (double)nbizdate((int)argv[0], (int)argv[1]); return TRUE; }
		break;

	case EVAL_FUNCTION_PBIZDATE:
		if( argc == 1 ) { * pValue = (double)pbizdate((int)argv[0], HDC_SUNSAT  ); return TRUE; }
		if( argc == 2 ) { * pValue = (double)pbizdate((int)argv[0], (int)argv[1]); return TRUE; }
		break;

	case EVAL_FUNCTION_ADDDAYS:
		if( argc == 2 ) { * pValue = (double)adddays((int)argv[0], (int)argv[1]); return TRUE; }
		break;

	case EVAL_FUNCTION_ADDMONTHS:
		if( argc == 2 ) { * pValue = (double)addmonths((int)argv[0], (int)argv[1], EMR_ACTUAL  ); return TRUE; }
		if( argc == 3 ) { * pValue = (double)addmonths((int)argv[0], (int)argv[1], (int)argv[2]); return TRUE; }
		break;

	case EVAL_FUNCTION_ADDTERMS:
		if( argc == 3 ) { * pValue = (double)addterms((int)argv[0], (int)argv[1], (int)argv[2], EMR_ACTUAL  ); return TRUE; }
		if( argc == 4 ) { * pValue = (double)addterms((int)argv[0], (int)argv[1], (int)argv[2], (int)argv[3]); return TRUE; }
		break;

	case EVAL_FUNCTION_DAYS360:
		if( argc == 2 ) { * pValue = (double)days360((int)argv[0], (int)argv[1], 0 /* USA */ ); return TRUE; }
		if( argc == 3 ) { * pValue = (double)days360((int)argv[0], (int)argv[1], (int)argv[2]); return TRUE; }
		break;

	case EVAL_FUNCTION_DAYS365:
		if( argc == 2 ) { * pValue = (double)days365((int)argv[0], (int)argv[1]); return TRUE; }
		break;

	case EVAL_FUNCTION_DAYSACT:
		if( argc == 2 ) { * pValue = (double)daysact((int)argv[0], (int)argv[1]); return TRUE; }
		break;

	case EVAL_FUNCTION_DAYSBET:
		if( argc == 2 ) { * pValue = (double)daysbet((int)argv[0], (int)argv[1], DCB_ACTACT  ); return TRUE; }
		if( argc == 3 ) { * pValue = (double)daysbet((int)argv[0], (int)argv[1], (int)argv[2]); return TRUE; }
		break;

	case EVAL_FUNCTION_MONTHSBET:
		if( argc == 2 ) { * pValue = (double)monthsbet((int)argv[0], (int)argv[1]); return TRUE; }
		break;

	case EVAL_FUNCTION_TERMSBET:
		if( argc == 3 ) { * pValue = (double)termsbet((int)argv[0], (int)argv[1], (int)argv[2]); return TRUE; }
		break;

	case EVAL_FUNCTION_TERMFRAC:
		if( argc == 3 ) { * pValue = termfrac((int)argv[0], (int)argv[1], (int)argv[2], DCB_ACTACT,   EMR_ACTUAL  ); return TRUE; }
		if( argc == 4 ) { * pValue = termfrac((int)argv[0], (int)argv[1], (int)argv[2], (int)argv[3], EMR_ACTUAL  ); return TRUE; }
		if( argc == 5 ) { * pValue = termfrac((int)argv[0], (int)argv[1], (int)argv[2], (int)argv[3], (int)argv[4]); return TRUE; }
		break;

	case EVAL_FUNCTION_YEARFRAC:
		if( argc == 2 ) { * pValue = yearfrac((int)argv[0], (int)argv[1], DCB_ACTACT,   EMR_ACTUAL  ); return TRUE; }
		if( argc == 3 ) { * pValue = yearfrac((int)argv[0], (int)argv[1], (int)argv[2], EMR_ACTUAL  ); return TRUE; }
		if( argc == 4 ) { * pValue = yearfrac((int)argv[0], (int)argv[1], (int)argv[2], (int)argv[3]); return TRUE; }
		break;

	}

	return FALSE;
}


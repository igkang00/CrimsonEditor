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
		"successful",
		"internal error",
		"wrong syntax",
		"variable not defined",
		"function not defined",
		"function argument count",
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
	if( * pError == EVAL_ERROR_SUCCESSFUL ) hashVariables.SetAt("ans", * pValue);

	return pExpr;
}

TCHAR * EVAL::GetErrorMessage(INT nError)
{
	return szErrorMessage[nError];
}

void EVAL::Initialize()
{
	hashVariables.InitHashTable(30);

	hashVariables.SetAt("ans",			0.0);
	hashVariables.SetAt("pi",			EVAL_VARIABLE_PI);

	hashVariables.SetAt("annual",		FRE_ANNUAL);
	hashVariables.SetAt("semiannual",	FRE_SEMIANNUAL);
	hashVariables.SetAt("quarterly",	FRE_QUARTERLY);
	hashVariables.SetAt("bimonthly",	FRE_BIMONTHLY);
	hashVariables.SetAt("monthly",		FRE_MONTHLY);

	hashVariables.SetAt("actact",		DCB_ACTACT);
	hashVariables.SetAt("act360",		DCB_ACT360);
	hashVariables.SetAt("act365",		DCB_ACT365);
	hashVariables.SetAt("30360",		DCB_30360);
	hashVariables.SetAt("30e360",		DCB_30E360);
	hashVariables.SetAt("nl365",		DCB_NL365);

	hashVariables.SetAt("actual",		EMR_ACTUAL);
	hashVariables.SetAt("endofmonth",	EMR_ENDOFMONTH);
	hashVariables.SetAt("sunday",		HDC_SUNDAY);
	hashVariables.SetAt("sunsat",		HDC_SUNSAT);

	hashFunctions.InitHashTable(100);

	hashFunctions.SetAt("abs",			EVAL_FUNCTION_ABS);
	hashFunctions.SetAt("fabs",			EVAL_FUNCTION_ABS);
	hashFunctions.SetAt("mod",			EVAL_FUNCTION_MOD);
	hashFunctions.SetAt("fmod",			EVAL_FUNCTION_MOD);
	hashFunctions.SetAt("ceil",			EVAL_FUNCTION_CEIL);
	hashFunctions.SetAt("floor",		EVAL_FUNCTION_FLOOR);
	hashFunctions.SetAt("round",		EVAL_FUNCTION_ROUND);
	hashFunctions.SetAt("min",			EVAL_FUNCTION_MIN);
	hashFunctions.SetAt("max",			EVAL_FUNCTION_MAX);

	hashFunctions.SetAt("acos",			EVAL_FUNCTION_ACOS);
	hashFunctions.SetAt("asin",			EVAL_FUNCTION_ASIN);
	hashFunctions.SetAt("atan",			EVAL_FUNCTION_ATAN);
	hashFunctions.SetAt("atan2",		EVAL_FUNCTION_ATAN2);
	hashFunctions.SetAt("cos",			EVAL_FUNCTION_COS);
	hashFunctions.SetAt("sin",			EVAL_FUNCTION_SIN);
	hashFunctions.SetAt("tan",			EVAL_FUNCTION_TAN);
	hashFunctions.SetAt("cosh",			EVAL_FUNCTION_COSH);
	hashFunctions.SetAt("sinh",			EVAL_FUNCTION_SINH);
	hashFunctions.SetAt("tanh",			EVAL_FUNCTION_TANH);

	hashFunctions.SetAt("exp",			EVAL_FUNCTION_EXP);
	hashFunctions.SetAt("log",			EVAL_FUNCTION_LOG);
	hashFunctions.SetAt("log10",		EVAL_FUNCTION_LOG10);
	hashFunctions.SetAt("pow",			EVAL_FUNCTION_POW);
	hashFunctions.SetAt("sqr",			EVAL_FUNCTION_SQR);
	hashFunctions.SetAt("sqrt",			EVAL_FUNCTION_SQRT);

	hashFunctions.SetAt("today",		EVAL_FUNCTION_TODAY);
	hashFunctions.SetAt("yeardays",		EVAL_FUNCTION_YEARDAYS);
	hashFunctions.SetAt("monthdays",	EVAL_FUNCTION_MONTHDAYS);
	hashFunctions.SetAt("date2days",	EVAL_FUNCTION_DATE2DAYS);
	hashFunctions.SetAt("days2date",	EVAL_FUNCTION_DAYS2DATE);
	hashFunctions.SetAt("eomday",		EVAL_FUNCTION_EOMDAY);
	hashFunctions.SetAt("eomdate",		EVAL_FUNCTION_EOMDATE);
	hashFunctions.SetAt("weekday",		EVAL_FUNCTION_WEEKDAY);

	hashFunctions.SetAt("isbizdate",	EVAL_FUNCTION_ISBIZDATE);
	hashFunctions.SetAt("nbizdate",		EVAL_FUNCTION_NBIZDATE);
	hashFunctions.SetAt("pbizdate",		EVAL_FUNCTION_PBIZDATE);
	hashFunctions.SetAt("adddays",		EVAL_FUNCTION_ADDDAYS);
	hashFunctions.SetAt("addmonths",	EVAL_FUNCTION_ADDMONTHS);
	hashFunctions.SetAt("addterms",		EVAL_FUNCTION_ADDTERMS);

	hashFunctions.SetAt("days360",		EVAL_FUNCTION_DAYS360);
	hashFunctions.SetAt("days365",		EVAL_FUNCTION_DAYS365);
	hashFunctions.SetAt("daysact",		EVAL_FUNCTION_DAYSACT);
	hashFunctions.SetAt("daysbet",		EVAL_FUNCTION_DAYSBET);
	hashFunctions.SetAt("monthsbet",	EVAL_FUNCTION_MONTHSBET);
	hashFunctions.SetAt("termsbet",		EVAL_FUNCTION_TERMSBET);
	hashFunctions.SetAt("termfrac",		EVAL_FUNCTION_TERMFRAC);
	hashFunctions.SetAt("yearfrac",		EVAL_FUNCTION_YEARFRAC);
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

	if     ( * pExpr == '\0'  ) return pExpr;
	else if( isdigit(* pExpr) ) return EvalConstant( pExpr, pValue, pError );
	else if( * pExpr == '('   ) return EvalParentheses( pExpr, pValue, pError );
	else if( * pExpr == '$'   ) return EvalVariable( pExpr, pValue, pError );
	else if( isalpha(* pExpr) ) return EvalFunction( pExpr, pValue, pError );
	else   /* other cases    */ { * pError = EVAL_ERROR_WRONG_SYNTAX; return pExpr; }
}

TCHAR * EVAL::EvalConstant(TCHAR * pExpr, double * pValue, INT * pError)
{
	EVAL_EAT_WHITE( pExpr );

	TCHAR * pEnd = pExpr;
	while( isdigit(* pEnd) || * pEnd == '.' ) pEnd++;

	TCHAR szNum[2048]; INT nLen = pEnd - pExpr;
	if( nLen > 0 ) { strncpy( szNum, pExpr, nLen ); szNum[nLen] = '\0'; }
	else { * pError = EVAL_ERROR_WRONG_SYNTAX; return pExpr; }

	* pValue = atof( szNum );
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
	while( isalnum(* pEnd) ) pEnd++;

	TCHAR szVar[2048]; INT nLen = pEnd - pExpr;
	if( nLen > 0 ) { strncpy( szVar, pExpr, nLen ); szVar[nLen] = '\0'; strlwr(szVar); }
	else { * pError = EVAL_ERROR_WRONG_SYNTAX; return pExpr; }

	double dValue;
	if( hashVariables.Lookup( szVar, dValue ) ) { * pValue = dValue; pExpr = pEnd; }
	else { * pError = EVAL_ERROR_VARIABLE_NOT_DEFINED; return pExpr; }

	return pExpr;
}

TCHAR * EVAL::EvalFunction(TCHAR * pExpr, double * pValue, INT * pError)
{
	EVAL_EAT_WHITE( pExpr );

	TCHAR * pEnd = pExpr;
	while( isalnum(* pEnd) ) pEnd++;

	TCHAR szFun[2048]; INT nLen = pEnd - pExpr;
	if( nLen > 0 ) { strncpy( szFun, pExpr, nLen ); szFun[nLen] = '\0'; strlwr(szFun); }
	else { * pError = EVAL_ERROR_WRONG_SYNTAX; return pExpr; }

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


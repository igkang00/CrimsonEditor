#ifndef __EVALUATE_H_
#define __EVALUATE_H_


#define EVAL_ERROR_SUCCESSFUL					0x00
#define EVAL_ERROR_INTERNAL						0x01
#define EVAL_ERROR_WRONG_SYNTAX					0x02
#define EVAL_ERROR_VARIABLE_NOT_DEFINED			0x03
#define EVAL_ERROR_FUNCTION_NOT_DEFINED			0x04
#define EVAL_ERROR_FUNCTION_ARGUMENT_COUNT		0x05


namespace EVAL {
	void SetVariable(TCHAR * pVarName, double dValue);
	void GetVariable(TCHAR * pVarName, double * pValue, INT * pError);

	TCHAR * Evaluate(TCHAR * pExpr, double * pValue, INT * pError);
	TCHAR * GetErrorMessage(INT nError);
};


#endif // __EVALUATE_H_

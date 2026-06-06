#ifndef __CMDLINE_H_
#define __CMDLINE_H_


#define MAX_PARAM	1024


class CCmdLine {
protected:
	CStringArray m_arrArguments;
	CMapStringToString m_mapSwitchs;

public:
	CCmdLine(LPCTSTR lpszCmdLine);
	CCmdLine(INT argc, TCHAR ** argv);

	BOOL HasSwitch(LPCTSTR lpszSwitch, CString & szOption);
	INT GetArgumentCount();

	CString GetArgument(INT nIndex);
	CString GetFullArgument();

protected:
	INT SplitLine(LPCTSTR lpszCmdLine, TCHAR ** argv);
	INT ParseParam(INT argc, TCHAR ** argv); 
	void FreeParam(INT argc, TCHAR ** argv);

	BOOL IsSwitch(LPCTSTR lpszParam);
	BOOL ParseSwitch(LPCTSTR lpszParam, CString & szSwitch, CString & szOption);
};


#endif // __CMDLINE_H_

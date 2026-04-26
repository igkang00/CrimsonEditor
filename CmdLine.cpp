#include "stdafx.h"
#include "CmdLine.h"


#define _IS_SPACE(ch)		( ch == ' ' || ch == '\t' )
#define _EAT_WHITE(p)		while( * p == ' ' || * p == '\t' ) p++;


CCmdLine::CCmdLine(LPCTSTR lpszCmdLine)
{
	TCHAR * argv[MAX_PARAM];
	INT argc = SplitLine(lpszCmdLine, argv);
	ParseParam(argc, argv);
	FreeParam(argc, argv);
}

CCmdLine::CCmdLine(INT argc, TCHAR ** argv)
{
	ParseParam(argc, argv);
}

BOOL CCmdLine::HasSwitch(LPCTSTR lpszSwitch, CString & szOption)
{
	return m_mapSwitchs.Lookup(lpszSwitch, szOption);
}

INT CCmdLine::GetArgumentCount()
{
	return m_arrArguments.GetSize();
}

CString CCmdLine::GetArgument(INT nIndex)
{
	return m_arrArguments[nIndex];
}

CString CCmdLine::GetFullArgument()
{
	INT nSize = m_arrArguments.GetSize(); if( nSize < 1 ) return "";
	CString szFullArgument = m_arrArguments[0];
	for(INT i = 1; i < nSize; i++)
		szFullArgument += CString(" ") + m_arrArguments[i];
	return szFullArgument;
}

INT CCmdLine::SplitLine(LPCTSTR lpszCmdLine, TCHAR ** argv)
{
	INT i, j, argc = 0; 
	TCHAR * pSave, * pChar;
	BOOL bQuoted = FALSE;

	argv[argc] = new TCHAR [MAX_PATH];
	strcpy(argv[argc], "__program_name");
	argc++;

	pChar = (TCHAR *)lpszCmdLine;
	_EAT_WHITE(pChar);

	while( * pChar ) {
		pSave = pChar;

		while( * pChar ) {
			if( * pChar == '\\' && * (pChar+1) == '\"' && ! bQuoted ) pChar += 2;
			else if( * pChar == '\"' ) { bQuoted = ! bQuoted; pChar++; }
			else if( ! _IS_SPACE(* pChar) ) pChar++;
			else if( _IS_SPACE(* pChar) && bQuoted ) pChar++;
			else break;
		}

		argv[argc] = new TCHAR [pChar-pSave+1];
		i = j = 0;
		while( i < pChar - pSave ) {
			if( pSave[i] == '\\' && pSave[i+1] == '\"' ) { argv[argc][j++] = pSave[i+1]; i += 2; }
			else if( pSave[i] != '\"' ) argv[argc][j++] = pSave[i++];
			else i++;
		}
		argv[argc][j] = '\0';
		argc++;

		_EAT_WHITE(pChar);
	}

	return argc;
}

INT CCmdLine::ParseParam(INT argc, TCHAR ** argv)
{
	m_mapSwitchs.RemoveAll();
	m_arrArguments.RemoveAll();

	CString szSwitch, szOption, szArgument;
	for(INT i = 1; i < argc; i++) {
		if( IsSwitch(argv[i]) && ParseSwitch(argv[i], szSwitch, szOption) ) {
			szSwitch.MakeUpper(); m_mapSwitchs.SetAt(szSwitch, szOption);
		} else {
			m_arrArguments.Add(argv[i]);
		}
	}
	return argc;
}

void CCmdLine::FreeParam(INT argc, TCHAR ** argv)
{
	for(INT i = 0; i < argc; i++) delete [] argv[i];
}

BOOL CCmdLine::IsSwitch(LPCTSTR lpszParam)
{
	if( lpszParam == NULL ) return FALSE;
	INT nLen = strlen(lpszParam); if( nLen < 2 ) return FALSE;
	if( lpszParam[0] == '/' || lpszParam[0] == '-' ) return isalpha(lpszParam[1]);
	else return FALSE;
}

BOOL CCmdLine::ParseSwitch(LPCTSTR lpszParam, CString & szSwitch, CString & szOption)
{
	CString szParam(lpszParam); if( szParam.GetLength() < 2 ) return FALSE;
	INT nPos = szParam.Find(':');
	szSwitch = ( nPos > 0 ) ? szParam.Mid(1, nPos-1) : szParam.Mid(1);
	szOption = ( nPos > 0 ) ? szParam.Mid(nPos+1) : "";
	return TRUE;
}

#ifndef __UTILITY_H_
#define __UTILITY_H_


// invert case, capitalize
void MakeInvertCase( CString & rString );
void MakeCapitalize( CString & rString );

// find string
INT ForwardFindString(LPCTSTR lpszString, LPCTSTR lpszSubString, INT nFrom, BOOL bWholeWord, BOOL bMatchCase);
INT ReverseFindString(LPCTSTR lpszString, LPCTSTR lpszSubString, INT nFrom, BOOL bWholeWord, BOOL bMatchCase);

// regular expression
INT ForwardFindStringRegExp(LPCTSTR lpszString, LPCTSTR lpszRegExp, CRegExp & clsRegExp, INT nFrom, BOOL bWholeWord, BOOL bMatchCase);
INT ReverseFindStringRegExp(LPCTSTR lpszString, LPCTSTR lpszRegExp, CRegExp & clsRegExp, INT nFrom, BOOL bWholeWord, BOOL bMatchCase);

// system environment
CString GetMsDosShellPath();
CString GetDefaultBrowserPath();

// goto url
HINSTANCE GotoURL(LPCTSTR URL, INT nCmdShow);

// get sub menu
CMenu * GetSubMenuByText(CMenu * pMenu, LPCTSTR lpszText);


#endif // __UTILITY_H_

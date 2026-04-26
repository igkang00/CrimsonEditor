#include "stdafx.h"
#include <string.h>
#include "registry.h"
#include "RegExp.h"
#include "Utility.h"


// invert case, capitalize
void MakeInvertCase(CString & rString)
{
	INT nLen = rString.GetLength();
	for(INT i = 0; i < nLen; i++) {
		INT ch = rString[i];
		if( isalpha(ch) ) {
			if( isupper(ch) ) rString.SetAt(i, tolower(ch));
			else rString.SetAt(i, toupper(ch));
		}
	}
}

void MakeCapitalize(CString & rString)
{
	INT nLen = rString.GetLength(); BOOL bAlpha = FALSE;
	for(INT i = 0; i < nLen; i++) {
		INT ch = rString[i];
		if( isalpha(ch) ) {
			if( bAlpha ) rString.SetAt(i, tolower(ch));
			else rString.SetAt(i, toupper(ch));
			bAlpha = TRUE;
		} else bAlpha = FALSE;
	}
}

// find string
INT ForwardFindString(LPCTSTR lpszString, LPCTSTR lpszSubStr, INT nFrom, BOOL bWholeWord, BOOL bMatchCase)
{
	INT nLen1 = strlen(lpszString); if( ! nLen1 ) return -1;
	INT nLen2 = strlen(lpszSubStr); if( ! nLen2 ) return -1;
	if( nFrom < 0 ) nFrom = 0; if( nFrom > nLen1 ) return -1;

	TCHAR * pString = (TCHAR *)lpszString;
	TCHAR * pSubStr = (TCHAR *)lpszSubStr;
	if( ! bMatchCase ) {
		pString = new TCHAR[nLen1+1]; strcpy(pString, lpszString); _strlwr(pString);
		pSubStr = new TCHAR[nLen2+1]; strcpy(pSubStr, lpszSubStr); _strlwr(pSubStr);
	}

	INT nFound = -1; TCHAR * pResult, * pOffset = pString + nFrom;
	while( pOffset < pString + nLen1 && ( pResult = strstr(pOffset, pSubStr) ) ) {
		if( bWholeWord ) {
			UINT nBef = (pResult > pString) ? pResult[-1] : 0;
			UINT nAft = pResult[nLen2];
			if( ! __iscsym(nBef) && ! __iscsym(nAft) ) { nFound = pResult - pString; break; }
		} else { nFound = pResult - pString; break; }

		pOffset = pResult + 1; 
	}

	if( ! bMatchCase ) {
		delete [] pString;
		delete [] pSubStr;
	}

	return nFound;
}

INT ReverseFindString(LPCTSTR lpszString, LPCTSTR lpszSubStr, INT nFrom, BOOL bWholeWord, BOOL bMatchCase)
{
	INT nLen1 = strlen(lpszString); if( ! nLen1 ) return -1;
	INT nLen2 = strlen(lpszSubStr); if( ! nLen2 ) return -1;
	if( nFrom < 0 ) nFrom = nLen1; if( nFrom > nLen1 ) return -1;

	TCHAR * pString = (TCHAR *)lpszString;
	TCHAR * pSubStr = (TCHAR *)lpszSubStr;
	if( ! bMatchCase ) {
		pString = new TCHAR[nLen1+1]; strcpy(pString, lpszString); _strlwr(pString);
		pSubStr = new TCHAR[nLen2+1]; strcpy(pSubStr, lpszSubStr); _strlwr(pSubStr);
	}

	INT nFound = -1; TCHAR * pResult, * pOffset = pString;
	while( pOffset < pString + nLen1 && ( pResult = strstr(pOffset, pSubStr) ) ) {
		if( pResult + nLen2 > pString + nFrom ) {
			break; // stop search if we get here...
		} else if( bWholeWord ) {
			UINT nBef = (pResult > pString) ? pResult[-1] : 0;
			UINT nAft = pResult[nLen2];
			if( ! __iscsym(nBef) && ! __iscsym(nAft) ) nFound = pResult - pString;
		} else nFound = pResult - pString;

		pOffset = pResult + 1;
	}

	if( ! bMatchCase ) {
		delete [] pString;
		delete [] pSubStr;
	}

	return nFound;
}

// regular expression
INT ForwardFindStringRegExp(LPCTSTR lpszString, LPCTSTR lpszRegExp, CRegExp & clsRegExp, INT nFrom, BOOL bWholeWord, BOOL bMatchCase)
{
	INT nLen1 = strlen(lpszString); // if( ! nLen1 ) return -1; -- can be zero length string
	INT nLen2 = strlen(lpszRegExp); if( ! nLen2 ) return -1;
	if( nFrom < 0 ) nFrom = 0; if( nFrom > nLen1 ) return -1;

	TCHAR * pString = (TCHAR *)lpszString;
	TCHAR * pRegExp = (TCHAR *)lpszRegExp;
	if( ! bMatchCase ) {
		static CString szString; szString = lpszString; szString.MakeLower();
		pString = (TCHAR *)(LPCTSTR)szString;
	}

	INT nResult, nFound = -1; TCHAR * pOffset = pString + nFrom;
	while( pOffset <= pString + nLen1 && ( nResult = clsRegExp.RegFind(pOffset) ) >= 0 ) {
		INT nLen3 = clsRegExp.GetFoundLength();

		if( pRegExp[0] == '^' && pOffset - pString + nResult > 0 ) {
			break; // there is no more possibility
		} else if( pRegExp[nLen2-1] == '$' && pOffset - pString + nResult + nLen3 < nLen1 ) { 
		//	continue; skip this search result...
		} else if( bWholeWord ) {
			UINT nBef = (pOffset + nResult > pString) ? pOffset[nResult - 1] : 0;
			UINT nAft = pOffset[nResult + nLen3];
			if( ! __iscsym(nBef) && ! __iscsym(nAft) ) { nFound = pOffset - pString + nResult; break; }
		} else { nFound = pOffset - pString + nResult; break; }

		pOffset = pOffset + nResult + 1;
	}

	return nFound;
}

INT ReverseFindStringRegExp(LPCTSTR lpszString, LPCTSTR lpszRegExp, CRegExp & clsRegExp, INT nFrom, BOOL bWholeWord, BOOL bMatchCase)
{
	INT nLen1 = strlen(lpszString); // if( ! nLen1 ) return -1; -- can be zero length string
	INT nLen2 = strlen(lpszRegExp); if( ! nLen2 ) return -1;
	if( nFrom < 0 ) nFrom = nLen1; if( nFrom > nLen1 ) return -1;

	TCHAR * pString = (TCHAR *)lpszString;
	TCHAR * pRegExp = (TCHAR *)lpszRegExp;
	if( ! bMatchCase ) {
		static CString szString; szString = lpszString; szString.MakeLower();
		pString = (TCHAR *)(LPCTSTR)szString;
	}

	INT nResult, nFound = -1, nLength = 0; TCHAR * pOffset = pString;
	while( pOffset <= pString + nLen1 && ( nResult = clsRegExp.RegFind(pOffset) ) >= 0 ) {
		INT nLocal = -1, nLen3 = clsRegExp.GetFoundLength(); 

		if( pOffset + nResult + nLen3 > pString + nFrom ) {
			break; // stop search if we get here
		} else if( pRegExp[0] == '^' && pOffset - pString + nResult > 0 ) {
			break; // there is no more possibility
		} else if( pRegExp[nLen2-1] == '$' && pOffset - pString + nResult + nLen3 < nLen1 ) { 
		//	continue; skip this search result...
		} else if( bWholeWord ) {
			UINT nBef = (pOffset + nResult > pString) ? pOffset[nResult - 1] : 0;
			UINT nAft = pOffset[nResult + nLen3];
			if( ! __iscsym(nBef) && ! __iscsym(nAft) ) nLocal = pOffset - pString + nResult;
		} else nLocal = pOffset - pString + nResult;

		// greedy rule of regular expression
		if( nLocal >= 0 && nLocal + nLen3 != nFound + nLength ) { nFound = nLocal; nLength = nLen3; }

		pOffset = pOffset + nResult + 1;
	}

	// to retrieve information from search result later
	if( nFound >= 0 ) clsRegExp.RegFind(pString + nFound);

	return nFound;
}


// system environment
CString GetMsDosShellPath()
{
	TCHAR szShellPath[MAX_PATH];
	GetEnvironmentVariable("COMSPEC", szShellPath, MAX_PATH);
	return szShellPath;
}

CString GetDefaultBrowserPath()
{
	TCHAR szKey[MAX_PATH + MAX_PATH];
	if( ! GetRegKeyValue(HKEY_CLASSES_ROOT, ".htm", "", szKey, MAX_PATH + MAX_PATH) ) return "";

	lstrcat(szKey, "\\shell\\open\\command");
	if( ! GetRegKeyValue(HKEY_CLASSES_ROOT, szKey, "", szKey, MAX_PATH + MAX_PATH) ) return "";

    TCHAR * pos;
    pos = _tcsstr(szKey, _T("\"%1\""));
    if (pos == NULL) {                     // No quotes found
        pos = _tcsstr(szKey, _T("%1"));    // Check for %1, without quotes
        if (pos == NULL)                   // No parameter at all...
            pos = szKey+lstrlen(szKey)-1;
        else * pos = '\0';                 // Remove the parameter
    } else * pos = '\0';                   // Remove the parameter

	return szKey;
}

// goto url
HINSTANCE GotoURL(LPCTSTR URL, INT nCmdShow)
{
	// First try ShellExecute()
	HINSTANCE hResult = ShellExecute(NULL, _T("open"), URL, NULL, NULL, nCmdShow);

	// If it failed, get the .htm regkey and lookup the program
	if ((UINT)hResult <= HINSTANCE_ERROR) {
		CString szCommand = GetDefaultBrowserPath();

		if( szCommand.GetLength() ) {
			szCommand += " ";
			szCommand += URL;
            hResult = (HINSTANCE) WinExec(szCommand, nCmdShow);
		}
	}
	  
	return hResult;
}

// get sub menu
CMenu * GetSubMenuByText(CMenu * pMenu, LPCTSTR lpszText)
{
	CString szMenuText; INT nMenuCount = pMenu->GetMenuItemCount();
	for( INT i = 0; i < nMenuCount; i++ ) {
		pMenu->GetMenuString( i, szMenuText, MF_BYPOSITION );
		if( ! szMenuText.Compare(lpszText) ) return pMenu->GetSubMenu( i );
	}
	return NULL;
}

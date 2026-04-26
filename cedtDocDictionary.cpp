#include "stdafx.h"
#include "cedtHeader.h"


static UINT nTotalSize = 0U;


BOOL CCedtDoc::LoadDictionary()
{
	CString szCedtDicPath = CCedtApp::m_szInstallDirectory + "\\cedt.dic";
	CString szUserDicPath = CCedtApp::m_szInstallDirectory + "\\user.dic";

	CFile file(szCedtDicPath, CFile::modeRead); CWaitCursor wait;
	nTotalSize = file.GetLength(); file.Close();

	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	pMainFrame->BeginProgress("Loading Dictionary...");

	if( ! m_clsDictionary.FileLoad(szCedtDicPath, LoadingProgress) ) return FALSE;
	if( ! m_clsDictionary.FileLoad(szUserDicPath) ) return FALSE;

	pMainFrame->EndProgress();
	m_bDictionaryLoaded = TRUE;

	return TRUE;
}

BOOL CCedtDoc::LoadingProgress(UINT nTotalRead)
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	if( nTotalSize > 0 ) pMainFrame->SetProgress( 100U * nTotalRead / nTotalSize );
	else pMainFrame->SetProgress( 100 );

	return TRUE;
}

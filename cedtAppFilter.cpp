#include "stdafx.h"
#include "cedtHeader.h"



CString CCedtApp::GetComposedFileFilter()
{
	CString szFileFilter, szElement;
	for(INT i = 0; i < MAX_FILE_FILTER; i++) {
		if( ! m_clsFileFilters[i].IsValid() ) continue;

		CString & szDescription = m_clsFileFilters[i].m_szDescription;
		CString & szExtensions  = m_clsFileFilters[i].m_szExtensions;

		szElement.Format("%s (%s)|%s|", szDescription, szExtensions, szExtensions);
		szFileFilter += szElement;
	}

	if( ! szFileFilter.GetLength() ) szFileFilter = "All Files (*.*)|*.*|";
	szFileFilter += "|";

	return szFileFilter;
}

CString CCedtApp::GetDefaultFileExtension()
{
	INT nFilter = m_nFilterIndexDialog;
	return m_clsFileFilters[nFilter].m_szDefaultExt;
}

void CCedtApp::ApplyFileFilterToFileWindow()
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();
	pFileWindow->InitFileFilterList( GetComposedFileFilter(), RecalcFilterIndexSkipNull(m_nFilterIndexPannel), CallbackFilterIndexPannel );
}

void CCedtApp::CallbackFilterIndexDialog(INT nFilter)
{
	m_nFilterIndexDialog = RecalcFilterIndexCountAll(nFilter);
}

void CCedtApp::CallbackFilterIndexPannel(INT nFilter)
{
	m_nFilterIndexPannel = RecalcFilterIndexCountAll(nFilter);
}

INT CCedtApp::RecalcFilterIndexSkipNull(INT nFilter)
{
	INT nFilterIndex = 0;

	for( INT i = 0; i < nFilter; i++ ) {
		if( m_clsFileFilters[i].IsValid() ) nFilterIndex++;
	}

	return nFilterIndex;
}

INT CCedtApp::RecalcFilterIndexCountAll(INT nFilter)
{
	INT nFilterIndex = 0;

	for( INT i = 0; i < nFilter; i++ ) {
		if( m_clsFileFilters[i].IsValid() ) nFilterIndex++;
		else nFilterIndex += 2;
	}

	return nFilterIndex;
}

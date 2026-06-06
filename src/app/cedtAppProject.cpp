#include "stdafx.h"
#include "cedtHeader.h"
#include "UserInputDialog.h"



void CCedtApp::OnProjectNew() 
{
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CString szFilter; szFilter.LoadString(IDS_FILTER_PROJECT);
	CFileDialog dlg(FALSE, ".prj", NULL, dwFlags, szFilter);

	TCHAR szCurrentDirectory[MAX_PATH];
	GetCurrentDirectory( MAX_PATH, szCurrentDirectory );

	TCHAR szInitialDirectory[MAX_PATH]; strcpy( szInitialDirectory, m_szProjectInitialDirectory );
	if( ! strlen( szInitialDirectory ) ) strcpy( szInitialDirectory, szCurrentDirectory );

	CString szTitle; szTitle.LoadString(IDS_DLG_NEW_PROJECT);
	dlg.m_ofn.lpstrTitle = szTitle; dlg.m_ofn.lpstrInitialDir = szInitialDirectory;
	if( dlg.DoModal() != IDOK ) return;

	GetCurrentDirectory( MAX_PATH, szInitialDirectory ); m_szProjectInitialDirectory = szInitialDirectory;
	SetCurrentDirectory( szCurrentDirectory );

	// ask user to close workspace files
	if( m_bProjectLoaded ) { // project is open now
		BOOL bCloseAll = FALSE;

		if( GetDocumentCount() ) {
			INT nAnswer = AfxMessageBox(IDS_MSG_CLOSE_PROJECT_WORKSPACE, MB_YESNOCANCEL);
			if( nAnswer == IDCANCEL ) return;
			bCloseAll = ( nAnswer == IDYES ) ? TRUE : FALSE;
		} else bCloseAll = FALSE;

		if( ! CloseProjectWorkspace(bCloseAll) ) return;
	}

	// activate project tab
	CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();
	pFrame->ShowFileWindowCategory( FILE_WINDOW_PROJECT );

	CString szPathName = dlg.GetPathName();
	if( ! NewProjectWorkspace( szPathName ) ) return;
}

void CCedtApp::OnProjectOpen() 
{
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	CString szFilter; szFilter.LoadString(IDS_FILTER_PROJECT);
	CFileDialog dlg(TRUE, NULL, NULL, dwFlags, szFilter);

	TCHAR szCurrentDirectory[MAX_PATH];
	GetCurrentDirectory( MAX_PATH, szCurrentDirectory );

	TCHAR szInitialDirectory[MAX_PATH]; strcpy( szInitialDirectory, m_szProjectInitialDirectory );
	if( ! strlen( szInitialDirectory ) ) strcpy( szInitialDirectory, szCurrentDirectory );

	CString szTitle; szTitle.LoadString(IDS_DLG_OPEN_PROJECT);
	dlg.m_ofn.lpstrTitle = szTitle; dlg.m_ofn.lpstrInitialDir = szInitialDirectory;
	if( dlg.DoModal() != IDOK ) return;

	GetCurrentDirectory( MAX_PATH, szInitialDirectory ); m_szProjectInitialDirectory = szInitialDirectory;
	SetCurrentDirectory( szCurrentDirectory );

	// close all the workspace files
	if( m_bProjectLoaded ) { // project is open now
		// check if same project is open already
		if( ! m_szProjectPathName.CompareNoCase( dlg.GetPathName() ) ) return; 

		// close the current project workspace
		BOOL bCloseAll = TRUE;
		if( ! CloseProjectWorkspace(bCloseAll) ) return;

	} else { // no project available
		// save modified and close all the documents
		if( SaveAllModified() ) CloseAllDocuments(FALSE);
		else return;
	}

	// activate project tab
	CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();
	pFrame->ShowFileWindowCategory( FILE_WINDOW_PROJECT );

	CString szPathName = dlg.GetPathName();
	if( ! OpenProjectWorkspace( szPathName ) ) return;
}

void CCedtApp::OnProjectClose() 
{
	// ask user to close workspace files
	if( m_bProjectLoaded ) { // project is open now
		BOOL bCloseAll = FALSE;

		if( GetDocumentCount() ) {
			INT nAnswer = AfxMessageBox(IDS_MSG_CLOSE_PROJECT_WORKSPACE, MB_YESNOCANCEL);
			if( nAnswer == IDCANCEL ) return;
			bCloseAll = ( nAnswer == IDYES ) ? TRUE : FALSE;
		} else bCloseAll = FALSE;

		if( ! CloseProjectWorkspace(bCloseAll) ) return;
	}
}

void CCedtApp::OnProjectNewCategory() 
{
	CUserInputDialog dlg(IDS_ASKUSR_CATEGORY_NAME);
	if( dlg.DoModal() != IDOK ) return;

	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();

	pFileWindow->AddCategoryToProject(dlg.m_szUserInput);
	pFileWindow->SaveProjectWorkspace(m_szProjectPathName);
}

void CCedtApp::OnProjectAddFilesTo() 
{
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;
	CString szFilter = GetComposedFileFilter();
	CFileDialog dlg(TRUE, NULL, NULL, dwFlags, szFilter);

	TCHAR szCurrentDirectory[MAX_PATH];
	GetCurrentDirectory( MAX_PATH, szCurrentDirectory );

	TCHAR szBuffer[4096]; memset(szBuffer, 0x00, sizeof(szBuffer));
	CString szTitle; szTitle.LoadString(IDS_DLG_ADD_TO_PROJECT);
	dlg.m_ofn.lpstrFile = szBuffer; dlg.m_ofn.nMaxFile = 4096; 
	dlg.m_ofn.lpstrTitle = szTitle; dlg.m_ofn.lpstrInitialDir = szCurrentDirectory;
	dlg.m_ofn.nFilterIndex = GetFilterIndexDialog() + 1;
	if( dlg.DoModal() != IDOK ) return;

	SetFilterIndexDialog(dlg.m_ofn.nFilterIndex - 1);

	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();

	POSITION pos = dlg.GetStartPosition();
	while( pos ) {
		CString szPathName = dlg.GetNextPathName( pos );
		pFileWindow->AddLocalFileToProject( szPathName );
	}
	pFileWindow->SaveProjectWorkspace(m_szProjectPathName);
}

void CCedtApp::OnProjectAddActiveFile() 
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pMainFrame );
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();
	CCedtDoc * pDoc = (CCedtDoc *)pMainFrame->MDIGetActiveDocument();

	if( ! m_bProjectLoaded || ! pDoc ) return;
	if( pDoc->IsNewFileNotSaved() ) return;

	if( pDoc->IsRemoteFile() ) {
		pFileWindow->AddRemoteFileToProject( pDoc->GetFtpAccountNumber(), pDoc->GetRemotePathName(), pDoc->GetFileSize() );
	} else {
		pFileWindow->AddLocalFileToProject( pDoc->GetPathName() );
	}
	pFileWindow->SaveProjectWorkspace(m_szProjectPathName);
}

void CCedtApp::OnProjectAddOpenFiles() 
{
	POSITION posDoc = GetFirstDocPosition();
	if( ! m_bProjectLoaded || ! posDoc ) return;

	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();

	while( posDoc ) {
		CCedtDoc * pDoc = (CCedtDoc *)GetNextDoc( posDoc );
		if( pDoc->IsNewFileNotSaved() ) continue;

		if( pDoc->IsRemoteFile() ) {
			pFileWindow->AddRemoteFileToProject( pDoc->GetFtpAccountNumber(), pDoc->GetRemotePathName(), pDoc->GetFileSize() );
		} else {
			pFileWindow->AddLocalFileToProject( pDoc->GetPathName() );
		}
	}
	pFileWindow->SaveProjectWorkspace(m_szProjectPathName);
}

void CCedtApp::OnUpdateProjectClose(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_bProjectLoaded );
}

void CCedtApp::OnUpdateProjectNewCategory(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_bProjectLoaded );
}

void CCedtApp::OnUpdateProjectAddFilesTo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_bProjectLoaded );
}

void CCedtApp::OnUpdateProjectAddActiveFile(CCmdUI* pCmdUI) 
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT( pMainFrame );
	CCedtDoc * pDoc = (CCedtDoc *)pMainFrame->MDIGetActiveDocument();
	pCmdUI->Enable( m_bProjectLoaded && pDoc && ! pDoc->IsNewFileNotSaved() );
}

void CCedtApp::OnUpdateProjectAddOpenFiles(CCmdUI* pCmdUI) 
{
	POSITION posDoc = GetFirstDocPosition();
	pCmdUI->Enable( m_bProjectLoaded && posDoc );
}

void CCedtApp::OnProjectItemOpen() 
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();
	pFileWindow->OpenSelectedProjectItem();
}

void CCedtApp::OnProjectItemExecute() 
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();
	pFileWindow->ExecuteSelectedProjectItem();
}

void CCedtApp::OnProjectItemRemove() 
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();

	if( pFileWindow->IsSelectedProjectItemRoot() ) return;

	CString szItemText = pFileWindow->GetSelectedProjectItemText();
	CString szMessage; szMessage.Format( IDS_MSG_REMOVE_FROM_PROJECT, szItemText );
	if( AfxMessageBox(szMessage, MB_OKCANCEL) != IDOK ) return;

	pFileWindow->RemoveSelectedProjectItem();
	pFileWindow->SaveProjectWorkspace(m_szProjectPathName);
}

void CCedtApp::OnProjectItemRename() 
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();
	pFileWindow->RenameSelectedProjectItem();
}

void CCedtApp::OnProjectItemProperty() 
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();
	pFileWindow->ShowPropSelectedProjectItem();
}


BOOL CCedtApp::CloseProjectWorkspace(BOOL bCloseAll)
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();

	if( ! pFileWindow->SaveProjectWorkspace(m_szProjectPathName) ) return FALSE;

	if( bCloseAll ) {
		if( SaveAllModified() ) CloseAllDocuments(FALSE);
		else return FALSE;
	}

	if( ! pFileWindow->InitProjectWorkspace() ) return FALSE;

	m_bProjectLoaded = FALSE;
	m_szProjectPathName = "";

	return TRUE;
}

BOOL CCedtApp::SaveProjectWorkspace()
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();

	if( ! pFileWindow->SaveProjectWorkspace(m_szProjectPathName) ) return FALSE;

	return TRUE;
}

BOOL CCedtApp::SaveRegularWorkspace()
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();

	if( ! pFileWindow->SaveRegularWorkspace(m_szAppDataDirectory + "\\cedt.wks") ) return FALSE;

	return TRUE;
}

BOOL CCedtApp::NewProjectWorkspace(LPCTSTR lpszPathName)
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();

	if( ! pFileWindow->NewProjectWorkspace(lpszPathName) ) return FALSE;
	if( ! pFileWindow->SaveProjectWorkspace(lpszPathName) ) return FALSE;

	m_bProjectLoaded = TRUE;
	m_szProjectPathName = lpszPathName;

	return TRUE;
}

BOOL CCedtApp::OpenProjectWorkspace(LPCTSTR lpszPathName)
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();

	if( ! pFileWindow->OpenProjectWorkspace(lpszPathName) ) {
		pFileWindow->InitProjectWorkspace();
		return FALSE;
	}

	m_bProjectLoaded = TRUE;
	m_szProjectPathName = lpszPathName;

	return TRUE;
}

BOOL CCedtApp::OpenRegularWorkspace(LPCTSTR lpszPathName)
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();

	if( ! pFileWindow->OpenRegularWorkspace(lpszPathName) ) {
		pFileWindow->InitProjectWorkspace();
		return FALSE;
	}

	m_bProjectLoaded = FALSE;
	m_szProjectPathName = "";

	return TRUE;
}

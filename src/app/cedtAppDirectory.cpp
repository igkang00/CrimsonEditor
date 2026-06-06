#include "stdafx.h"
#include "cedtHeader.h"


void CCedtApp::OnDirectoryItemOpen() 
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();
	pFileWindow->OpenSelectedDirectoryItem();
}

void CCedtApp::OnDirectoryItemExecute() 
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();
	pFileWindow->ExecuteSelectedDirectoryItem();
}

void CCedtApp::OnDirectoryItemExplore() 
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();
	pFileWindow->ExploreSelectedDirectoryItem();
}

void CCedtApp::OnDirectoryItemFind() 
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();
	pFileWindow->FindInSelectedDirectoryItem();
}

void CCedtApp::OnDirectoryItemNewFolder() 
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();
	pFileWindow->CreateNewFolderInSelectedDirectoryItem();
}

void CCedtApp::OnDirectoryItemNewDocument() 
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();
	pFileWindow->CreateNewDocumentInSelectedDirectoryItem();
}

void CCedtApp::OnDirectoryItemSetWorkdir() 
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();
	pFileWindow->SetAsWorkingDirSelectedDirectoryItem();
	SaveUserConfiguration(m_szAppDataDirectory + "\\cedt.conf");
}

void CCedtApp::OnDirectoryItemMove() 
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();
	pFileWindow->MoveSelectedDirectoryItem();
}

void CCedtApp::OnDirectoryItemCopy() 
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();
	pFileWindow->CopySelectedDirectoryItem();
}

void CCedtApp::OnDirectoryItemDelete() 
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();
	pFileWindow->DeleteSelectedDirectoryItem();
}

void CCedtApp::OnDirectoryItemRename() 
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();
	pFileWindow->RenameSelectedDirectoryItem();
}

void CCedtApp::OnDirectoryItemProperty() 
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();
	pFileWindow->ShowPropSelectedDirectoryItem();
}

void CCedtApp::OnDirectoryItemRefresh() 
{
	CMainFrame * pMainFrame = (CMainFrame *)AfxGetMainWnd();
	CFileWindow * pFileWindow = (CFileWindow *)pMainFrame->GetFileWindow();
	pFileWindow->RefreshSelectedDirectoryItem();
}

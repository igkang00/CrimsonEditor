#include "stdafx.h"
#include "cedtHeader.h"


BOOL CFileWindow::InitProjectWorkspace()
{
	if( ! RemoveAllProjectItems() ) return FALSE;
	EnableAllProjectButtons(FALSE);

	CString szItemText; szItemText.LoadString(IDS_MENU_NO_PROJECT_AVAILABLE);
	if( ! InsertProjectTreeItem(TVI_ROOT, szItemText, PROJECT_ITEM_PROJECT, 0, "") ) return FALSE;

	EnableAllProjectButtons(FALSE);
	return TRUE;
}

BOOL CFileWindow::NewProjectWorkspace(LPCTSTR lpszPathName)
{
	if( ! RemoveAllProjectItems() ) return FALSE;
	EnableAllProjectButtons(FALSE);

	if( ! InsertProjectTreeItem(TVI_ROOT, GetFileName(lpszPathName), PROJECT_ITEM_PROJECT, 0, lpszPathName) ) return FALSE;

	EnableAllProjectButtons(TRUE);
	return TRUE;
}

BOOL CFileWindow::SaveProjectWorkspace(LPCTSTR lpszPathName)
{
	ofstream fout(lpszPathName, ios::out);
	if( ! fout.is_open() ) return FALSE;

	CString szContents;

	// save project
	szContents.Format("<project version=\"%s\">", STRING_PROJECTFILEVER);
	fout << szContents << endl;

	HTREEITEM hRoot = m_treProjectTree.GetRootItem();
	HTREEITEM hChild = m_treProjectTree.GetChildItem( hRoot );
	while( hChild ) { // recursive call to all child items
		if( ! SaveProjectItem(fout, 1, hChild) ) { fout.close(); return FALSE; }
		hChild = m_treProjectTree.GetNextSiblingItem( hChild );
	}

	fout << "</project>" << endl << endl;

	// save workspace
	szContents.Format("<workspace version=\"%s\">", STRING_PROJECTFILEVER);
	fout << szContents << endl;

	CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();
	INT nCount = pFrame->GetChildFrameCount();
	for( INT nTab = 0; nTab < nCount; nTab++ ) {
		CMDIChildWnd * pChild = pFrame->GetChildFrame(nTab);
		if( ! SaveWorkspaceItem(fout, 1, pChild) ) { fout.close(); return FALSE; }
	}

	fout << "</workspace>" << endl << endl;

	fout.close(); return TRUE;
}

BOOL CFileWindow::OpenProjectWorkspace(LPCTSTR lpszPathName)
{
	if( ! RemoveAllProjectItems() ) return FALSE;
	EnableAllProjectButtons(FALSE);

	// open file to load project workspace
	ifstream fin(lpszPathName, ios::in | ios::nocreate);

	CMapStringToString mapAttr; TCHAR szText[4096]; 
	fin >> szText; // get first token

	// load project
	if( ! _stricmp(szText, "<project") ) {
		fin.getline(szText, 4096, '>'); // get attributes
		if( ! ParseItemAttribute( szText, mapAttr ) ) { fin.close(); return FALSE; }

		CString szVersion; BOOL bLookup = mapAttr.Lookup("version", szVersion);
		if( ! bLookup ) { AfxMessageBox(IDS_ERR_WRONG_PRJ_FILE); fin.close(); return FALSE; }

		HTREEITEM hItem = InsertProjectTreeItem(TVI_ROOT, GetFileName(lpszPathName), PROJECT_ITEM_PROJECT, 0, lpszPathName);
		fin >> szText; // get next token

		while( _stricmp(szText, "</project>") ) {
			if( ! LoadProjectItem(fin, szText, hItem) ) { fin.close(); return FALSE; }
		}
		fin >> szText; // get next token

		// expand and select root item
		if( ! m_treProjectTree.Expand( hItem, TVE_EXPAND ) ) TRUE; // there could be only one root item;
		if( ! m_treProjectTree.SelectItem( hItem ) ) { fin.close(); return FALSE; }

	} else { AfxMessageBox(IDS_ERR_WRONG_PRJ_FILE); fin.close(); return FALSE; }

	// load workspace
	if( ! _stricmp(szText, "<workspace") ) {
		fin.getline(szText, 4096, '>'); // get attributes
		if( ! ParseItemAttribute( szText, mapAttr ) ) { fin.close(); return FALSE; }

		CString szVersion; BOOL bLookup = mapAttr.Lookup("version", szVersion);
		if( ! bLookup ) { AfxMessageBox(IDS_ERR_WRONG_PRJ_FILE); fin.close(); return FALSE; }

		CCedtApp * pApp = (CCedtApp *)AfxGetApp();
		fin >> szText; // get next token

		while( _stricmp(szText, "</workspace>") ) {
			if( ! LoadWorkspaceItem(fin, szText, pApp) ) { fin.close(); return FALSE; }
		}
		fin >> szText; // get next token

	} else { AfxMessageBox(IDS_ERR_WRONG_PRJ_FILE); fin.close(); return FALSE; }

	EnableAllProjectButtons(TRUE);
	fin.close(); return TRUE;
}

BOOL CFileWindow::SaveRegularWorkspace(LPCTSTR lpszPathName)
{
	ofstream fout(lpszPathName, ios::out);
	if( ! fout.is_open() ) return FALSE;

	CString szContents;

	// save workspace
	szContents.Format("<workspace version=\"%s\">", STRING_PROJECTFILEVER);
	fout << szContents << endl;

	CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();
	INT nCount = pFrame->GetChildFrameCount();
	for( INT nTab = 0; nTab < nCount; nTab++ ) {
		CMDIChildWnd * pChild = pFrame->GetChildFrame(nTab);
		if( ! SaveWorkspaceItem(fout, 1, pChild) ) { fout.close(); return FALSE; }
	}

	fout << "</workspace>" << endl << endl;

	fout.close(); return TRUE;
}

BOOL CFileWindow::OpenRegularWorkspace(LPCTSTR lpszPathName)
{
	if( ! RemoveAllProjectItems() ) return FALSE;
	EnableAllProjectButtons(FALSE);

	// init project workspace
	CString szItemText; szItemText.LoadString(IDS_MENU_NO_PROJECT_AVAILABLE);
	if( ! InsertProjectTreeItem(TVI_ROOT, szItemText, PROJECT_ITEM_PROJECT, 0, "") ) return FALSE;

	// open file to load regular workspace
	ifstream fin(lpszPathName, ios::in | ios::nocreate);

	CMapStringToString mapAttr; TCHAR szText[4096]; 
	fin >> szText; // get first token

	// load workspace
	if( ! _stricmp(szText, "<workspace") ) {
		fin.getline(szText, 4096, '>'); // get attributes
		if( ! ParseItemAttribute( szText, mapAttr ) ) { fin.close(); return FALSE; }

		CString szVersion; BOOL bLookup = mapAttr.Lookup("version", szVersion);
		if( ! bLookup ) { AfxMessageBox(IDS_ERR_WRONG_PRJ_FILE); fin.close(); return FALSE; }

		CCedtApp * pApp = (CCedtApp *)AfxGetApp();
		fin >> szText; // get next token

		while( _stricmp(szText, "</workspace>") ) {
			if( ! LoadWorkspaceItem(fin, szText, pApp) ) { fin.close(); return FALSE; }
		}
		fin >> szText; // get next token
	} else { AfxMessageBox(IDS_ERR_WRONG_PRJ_FILE); fin.close(); return FALSE; }

	EnableAllProjectButtons(FALSE);
	fin.close(); return TRUE;
}

BOOL CFileWindow::AddCategoryToProject(LPCTSTR lpszCategory)
{
	HTREEITEM hParent = m_treProjectTree.GetSelectedItem();
	if( ! hParent ) { AfxMessageBox(IDS_ERR_NO_PRJ_ITEM_SELECTED); return FALSE; }

	// if selected item is not a category, then get parent item
	INT nImage, nSelectedImage; m_treProjectTree.GetItemImage(hParent, nImage, nSelectedImage);
	if( nImage != PROJECT_ITEM_PROJECT && nImage != PROJECT_ITEM_CATEGORY ) hParent = m_treProjectTree.GetParentItem(hParent);

	// call a function to add item to project
	return AddCategoryToProject(hParent, lpszCategory);
}

BOOL CFileWindow::AddLocalFileToProject(LPCTSTR lpszPathName)
{
	HTREEITEM hParent = m_treProjectTree.GetSelectedItem();
	if( ! hParent ) { AfxMessageBox(IDS_ERR_NO_PRJ_ITEM_SELECTED); return FALSE; }

	// if selected item is not a category, then get parent item
	INT nImage, nSelectedImage; m_treProjectTree.GetItemImage(hParent, nImage, nSelectedImage);
	if( nImage != PROJECT_ITEM_PROJECT && nImage != PROJECT_ITEM_CATEGORY ) hParent = m_treProjectTree.GetParentItem(hParent);

	// call a function to add item to project
	return AddLocalFileToProject(hParent, lpszPathName);
}

BOOL CFileWindow::AddRemoteFileToProject(INT nAccount, LPCTSTR lpszPathName, DWORD dwSize)
{
	HTREEITEM hParent = m_treProjectTree.GetSelectedItem();
	if( ! hParent ) { AfxMessageBox(IDS_ERR_NO_PRJ_ITEM_SELECTED); return FALSE; }

	// if selected item is not a category, then get parent item
	INT nImage, nSelectedImage; m_treProjectTree.GetItemImage(hParent, nImage, nSelectedImage);
	if( nImage != PROJECT_ITEM_PROJECT && nImage != PROJECT_ITEM_CATEGORY ) hParent = m_treProjectTree.GetParentItem(hParent);

	// call a function to add item to project
	return AddRemoteFileToProject(hParent, nAccount, lpszPathName, dwSize);
}

BOOL CFileWindow::AddLocalDirectoryToProject(LPCTSTR lpszPathName)
{
	HTREEITEM hParent = m_treProjectTree.GetSelectedItem();
	if( ! hParent ) { AfxMessageBox(IDS_ERR_NO_PRJ_ITEM_SELECTED); return FALSE; }

	// if selected item is not a category, then get parent item
	INT nImage, nSelectedImage; m_treProjectTree.GetItemImage(hParent, nImage, nSelectedImage);
	if( nImage != PROJECT_ITEM_PROJECT && nImage != PROJECT_ITEM_CATEGORY ) hParent = m_treProjectTree.GetParentItem(hParent);

	// call a function to add item to project
	return AddLocalDirectoryToProject(hParent, lpszPathName);
}

BOOL CFileWindow::AddProjectCategoryToProject(LPPROJECTITEMINFO lpInfo)
{
	HTREEITEM hParent = m_treProjectTree.GetSelectedItem();
	if( ! hParent ) { AfxMessageBox(IDS_ERR_NO_PRJ_ITEM_SELECTED); return FALSE; }

	// if selected item is not a category, then get parent item
	INT nImage, nSelectedImage; m_treProjectTree.GetItemImage(hParent, nImage, nSelectedImage);
	if( nImage != PROJECT_ITEM_PROJECT && nImage != PROJECT_ITEM_CATEGORY ) hParent = m_treProjectTree.GetParentItem(hParent);

	// call a function to add item to project
	return AddProjectCategoryToProject(hParent, lpInfo);
}

BOOL CFileWindow::IsSelectedProjectItemRoot()
{
	HTREEITEM hItem = m_treProjectTree.GetSelectedItem();
	HTREEITEM hRoot = m_treProjectTree.GetRootItem();
	return (hItem == hRoot);
}

CString CFileWindow::GetSelectedProjectItemText()
{
	HTREEITEM hItem = m_treProjectTree.GetSelectedItem();
	if( ! hItem ) { AfxMessageBox(IDS_ERR_NO_PRJ_ITEM_SELECTED); return ""; }
	return m_treProjectTree.GetItemText(hItem);
}

BOOL CFileWindow::OpenSelectedProjectItem()
{
	HTREEITEM hItem = m_treProjectTree.GetSelectedItem();
	if( ! hItem ) { AfxMessageBox(IDS_ERR_NO_PRJ_ITEM_SELECTED); return FALSE; }

	INT nImage, nSelectedImage; m_treProjectTree.GetItemImage(hItem, nImage, nSelectedImage);
	if( nImage != PROJECT_ITEM_LOCAL_FILE && nImage != PROJECT_ITEM_REMOTE_FILE ) return FALSE;

	LPPROJECTITEMINFO lpInfo = GetProjectItemInfo(hItem);
	return OpenProjectItem(lpInfo);
}

BOOL CFileWindow::ExecuteSelectedProjectItem()
{
	HTREEITEM hItem = m_treProjectTree.GetSelectedItem();
	if( ! hItem ) { AfxMessageBox(IDS_ERR_NO_PRJ_ITEM_SELECTED); return FALSE; }

	INT nImage, nSelectedImage; m_treProjectTree.GetItemImage(hItem, nImage, nSelectedImage);
	if( nImage != PROJECT_ITEM_LOCAL_FILE ) return FALSE;

	LPPROJECTITEMINFO lpInfo = GetProjectItemInfo(hItem);
	return ExecuteProjectItem(lpInfo);
}

BOOL CFileWindow::ShowPropSelectedProjectItem()
{
	HTREEITEM hItem = m_treProjectTree.GetSelectedItem();
	if( ! hItem ) { AfxMessageBox(IDS_ERR_NO_PRJ_ITEM_SELECTED); return FALSE; }

	INT nImage, nSelectedImage; m_treProjectTree.GetItemImage(hItem, nImage, nSelectedImage);
	if( nImage != PROJECT_ITEM_LOCAL_FILE ) return FALSE;

	LPPROJECTITEMINFO lpInfo = GetProjectItemInfo(hItem);
	return ShowPropProjectItem(lpInfo);
}

BOOL CFileWindow::RemoveSelectedProjectItem()
{
	HTREEITEM hItem = m_treProjectTree.GetSelectedItem();
	if( ! hItem ) { AfxMessageBox(IDS_ERR_NO_PRJ_ITEM_SELECTED); return FALSE; }

	INT nImage, nSelectedImage; m_treProjectTree.GetItemImage(hItem, nImage, nSelectedImage);
	if( nImage == PROJECT_ITEM_PROJECT ) return FALSE;
	
	return RemoveProjectItem(hItem);
}

BOOL CFileWindow::RenameSelectedProjectItem()
{
	HTREEITEM hItem = m_treProjectTree.GetSelectedItem();
	if( ! hItem ) { AfxMessageBox(IDS_ERR_NO_PRJ_ITEM_SELECTED); return FALSE; }

	INT nImage, nSelectedImage; m_treProjectTree.GetItemImage(hItem, nImage, nSelectedImage);
	if( nImage == PROJECT_ITEM_PROJECT ) return FALSE;

	CEdit * pEdit = m_treProjectTree.EditLabel(hItem);
	return (pEdit != NULL);
}

/////////////////////////////////////////////////////////////////////////////
// Inner Functions
BOOL CFileWindow::SaveProjectItem(ostream & os, INT nLevel, HTREEITEM hItem)
{
	INT nImage, nSelectedImage; m_treProjectTree.GetItemImage( hItem, nImage, nSelectedImage );
	CString szContents, szIndent('\t', nLevel), szText = m_treProjectTree.GetItemText( hItem );
	CString szExpanded = (m_treProjectTree.GetItemState(hItem, TVIS_EXPANDED) & TVIS_EXPANDED) ? "yes" : "no";

	if( nImage == PROJECT_ITEM_CATEGORY ) {
		szContents.Format("<category name=\"%s\" expanded=\"%s\">", szText, szExpanded);
		os << szIndent << szContents << endl;

		HTREEITEM hChild = m_treProjectTree.GetChildItem( hItem );
		while( hChild ) { // recursive call to all child items
			SaveProjectItem(os, nLevel+1, hChild);
			hChild = m_treProjectTree.GetNextSiblingItem( hChild );
		}

		os << szIndent << "</category>" << endl;

	} else if( nImage == PROJECT_ITEM_LOCAL_FILE ) {
		CString szPath = GetProjectItemPathName( hItem );
		szContents.Format("<localfile path=\"%s\" />", szPath);

		os << szIndent << szContents << endl;

	} else if( nImage == PROJECT_ITEM_REMOTE_FILE ) {
		LPPROJECTITEMINFO lpInfo = GetProjectItemInfo( hItem );
		INT nAccount = lpInfo->nFtpAccount;
		CString szPath = lpInfo->szPathName;

		szContents.Format("<remotefile account=\"%d\" path=\"%s\" />", nAccount, szPath);

		os << szIndent << szContents << endl;

	} else return FALSE;

	return TRUE;
}

BOOL CFileWindow::LoadProjectItem(istream & is, TCHAR szText[], HTREEITEM hParent)
{
	CMapStringToString mapAttr; 

	if( ! _stricmp(szText, "<category") ) {
		is.getline(szText, 4096, '>'); // get attributes
		if( ! ParseItemAttribute( szText, mapAttr ) ) return FALSE;

		CString szName; BOOL bLookup = mapAttr.Lookup("name", szName);
		if( ! bLookup ) { AfxMessageBox(IDS_ERR_WRONG_PRJ_FILE); return FALSE; }

		CString szExpanded; bLookup = mapAttr.Lookup("expanded", szExpanded);
		if( ! bLookup ) { szExpanded = "no"; }

		HTREEITEM hItem = InsertProjectTreeItem(hParent, szName, PROJECT_ITEM_CATEGORY, 0, "");
		is >> szText; // get next token

		while( _stricmp(szText, "</category>") ) {
			if( ! LoadProjectItem(is, szText, hItem) ) return FALSE;
		}
		is >> szText; // get next token

		// expand category item if it is checked
		if( ! szExpanded.Compare("yes") ) m_treProjectTree.Expand( hItem, TVE_EXPAND );

	} else if( ! _stricmp(szText, "<localfile") ) {
		is.getline(szText, 4096, '>'); // get attributes
		INT nLen = strlen(szText); if( szText[nLen-1] == '/' ) szText[nLen-1] = '\0';
		if( ! ParseItemAttribute( szText, mapAttr ) ) return FALSE;

		CString szPath; BOOL bLookup = mapAttr.Lookup("path", szPath);
		if( ! bLookup ) { AfxMessageBox(IDS_ERR_WRONG_PRJ_FILE); return FALSE; }

		HTREEITEM hItem = InsertProjectTreeItem(hParent, GetFileName(szPath), PROJECT_ITEM_LOCAL_FILE, 0, szPath);
		is >> szText; // get next token

	} else if( ! _stricmp(szText, "<remotefile") ) {
		is.getline(szText, 4096, '>'); // get attributes
		INT nLen = strlen(szText); if( szText[nLen-1] == '/' ) szText[nLen-1] = '\0';
		if( ! ParseItemAttribute( szText, mapAttr ) ) return FALSE;

		CString szAccount; BOOL bLookup = mapAttr.Lookup("account", szAccount);
		if( ! bLookup ) { AfxMessageBox(IDS_ERR_WRONG_PRJ_FILE); return FALSE; }

		CString szPath; bLookup = mapAttr.Lookup("path", szPath);
		if( ! bLookup ) { AfxMessageBox(IDS_ERR_WRONG_PRJ_FILE); return FALSE; }

		HTREEITEM hItem = InsertProjectTreeItem(hParent, GetFileName(szPath), PROJECT_ITEM_REMOTE_FILE, atoi(szAccount), szPath);
		is >> szText; // get next token

	} else { // not recognized item
		is.getline(szText, 4096, '>'); // skip attributes
		is >> szText; // get next token
	}

	return TRUE;
}

BOOL CFileWindow::SaveWorkspaceItem(ostream & os, INT nLevel, CMDIChildWnd * pChild)
{
	CCedtDoc * pDoc = (CCedtDoc *)pChild->GetActiveDocument();
	if( pDoc->IsNewFileNotSaved() ) return TRUE;

	CCedtView * pView = (CCedtView *)pChild->GetActiveView();
	CString szContents, szIndent('\t', nLevel);

	if( pDoc->IsRemoteFile() ) {
		INT nAccount = pDoc->GetFtpAccountNumber();
		CString szPath = pDoc->GetRemotePathName();

		INT nLineNum = pView->GetCurrentLineNumber();
		WINDOWPLACEMENT wndpl; pChild->GetWindowPlacement( & wndpl );

		szContents.Format("<remotefile account=\"%d\" path=\"%s\" linenum=\"%d\" placement=\"%d:%d:%d:%d:%d:%d:%d:%d:%d:%d\" />",
			nAccount, szPath, nLineNum, (INT)wndpl.flags, (INT)wndpl.showCmd, 
			wndpl.ptMinPosition.x, wndpl.ptMinPosition.y, 
			wndpl.ptMaxPosition.x, wndpl.ptMaxPosition.y,
			wndpl.rcNormalPosition.left, wndpl.rcNormalPosition.top, 
			wndpl.rcNormalPosition.right, wndpl.rcNormalPosition.bottom);

		os << szIndent << szContents << endl;

	} else {
		CString szPath = pDoc->GetPathName();

		INT nLineNum = pView->GetCurrentLineNumber();
		INT nWindow = pChild->m_nWindow;

		WINDOWPLACEMENT wndpl; pChild->GetWindowPlacement( & wndpl );

		szContents.Format("<localfile path=\"%s\" linenum=\"%d\" placement=\"%d:%d:%d:%d:%d:%d:%d:%d:%d:%d\" />", 
			szPath, nLineNum, (INT)wndpl.flags, (INT)wndpl.showCmd, 
			wndpl.ptMinPosition.x, wndpl.ptMinPosition.y, 
			wndpl.ptMaxPosition.x, wndpl.ptMaxPosition.y,
			wndpl.rcNormalPosition.left, wndpl.rcNormalPosition.top, 
			wndpl.rcNormalPosition.right, wndpl.rcNormalPosition.bottom);

		os << szIndent << szContents << endl;
	}

	return TRUE;
}

BOOL CFileWindow::LoadWorkspaceItem(istream & is, TCHAR szText[], CWinApp * pApp)
{
	CCedtApp * pCedtApp = (CCedtApp *)pApp;
	CMapStringToString mapAttr;

	if( ! _stricmp(szText, "<remotefile") ) {
		is.getline(szText, 4096, '>'); // get attributes
		INT nLen = strlen(szText); if( szText[nLen-1] == '/' ) szText[nLen-1] = '\0';
		if( ! ParseItemAttribute( szText, mapAttr ) ) return FALSE;

		CString szAccount; BOOL bLookup = mapAttr.Lookup("account", szAccount);
		if( ! bLookup ) { AfxMessageBox(IDS_ERR_WRONG_PRJ_FILE); return FALSE; }

		CString szPath; bLookup = mapAttr.Lookup("path", szPath);
		if( ! bLookup ) { AfxMessageBox(IDS_ERR_WRONG_PRJ_FILE); return FALSE; }

		CString szLineNum; bLookup = mapAttr.Lookup("linenum", szLineNum);
		if( ! bLookup ) { AfxMessageBox(IDS_ERR_WRONG_PRJ_FILE); return FALSE; }

		CString szPlacement; bLookup = mapAttr.Lookup("placement", szPlacement);
		if( ! bLookup ) { szPlacement = ""; }

		WINDOWPLACEMENT * pwndpl = NULL;
		if( szPlacement.GetLength() ) {
			WINDOWPLACEMENT wndpl; wndpl.length = sizeof(wndpl);
			INT nFound = 0; pwndpl = & wndpl;

			wndpl.flags = atoi( szPlacement.Mid(nFound) ); nFound = szPlacement.Find(':', nFound) + 1;
			wndpl.showCmd = atoi( szPlacement.Mid(nFound) ); nFound = szPlacement.Find(':', nFound) + 1;
			wndpl.ptMinPosition.x = atoi( szPlacement.Mid(nFound) ); nFound = szPlacement.Find(':', nFound) + 1;
			wndpl.ptMinPosition.y = atoi( szPlacement.Mid(nFound) ); nFound = szPlacement.Find(':', nFound) + 1;
			wndpl.ptMaxPosition.x = atoi( szPlacement.Mid(nFound) ); nFound = szPlacement.Find(':', nFound) + 1;
			wndpl.ptMaxPosition.y = atoi( szPlacement.Mid(nFound) ); nFound = szPlacement.Find(':', nFound) + 1;
			wndpl.rcNormalPosition.left = atoi( szPlacement.Mid(nFound) ); nFound = szPlacement.Find(':', nFound) + 1;
			wndpl.rcNormalPosition.top = atoi( szPlacement.Mid(nFound) ); nFound = szPlacement.Find(':', nFound) + 1;
			wndpl.rcNormalPosition.right = atoi( szPlacement.Mid(nFound) ); nFound = szPlacement.Find(':', nFound) + 1;
			wndpl.rcNormalPosition.bottom = atoi( szPlacement.Mid(nFound) ); nFound = szPlacement.Find(':', nFound) + 1;
		}

		pCedtApp->SpawnRemoteDocumentFile(atoi(szAccount), szPath, atoi(szLineNum), pwndpl);
		is >> szText; // get next token

	} else if( ! _stricmp(szText, "<localfile") ) {
		is.getline(szText, 4096, '>'); // get attributes
		INT nLen = strlen(szText); if( szText[nLen-1] == '/' ) szText[nLen-1] = '\0';
		if( ! ParseItemAttribute( szText, mapAttr ) ) return FALSE;

		CString szPath; BOOL bLookup = mapAttr.Lookup("path", szPath);
		if( ! bLookup ) { AfxMessageBox(IDS_ERR_WRONG_PRJ_FILE); return FALSE; }

		CString szLineNum; bLookup = mapAttr.Lookup("linenum", szLineNum);
		if( ! bLookup ) { AfxMessageBox(IDS_ERR_WRONG_PRJ_FILE); return FALSE; }

		CString szPlacement; bLookup = mapAttr.Lookup("placement", szPlacement);
		if( ! bLookup ) { szPlacement = ""; }

		WINDOWPLACEMENT * pwndpl = NULL;
		if( szPlacement.GetLength() ) {
			WINDOWPLACEMENT wndpl; wndpl.length = sizeof(wndpl);
			INT nFound = 0; pwndpl = & wndpl;

			wndpl.flags = atoi( szPlacement.Mid(nFound) ); nFound = szPlacement.Find(':', nFound) + 1;
			wndpl.showCmd = atoi( szPlacement.Mid(nFound) ); nFound = szPlacement.Find(':', nFound) + 1;
			wndpl.ptMinPosition.x = atoi( szPlacement.Mid(nFound) ); nFound = szPlacement.Find(':', nFound) + 1;
			wndpl.ptMinPosition.y = atoi( szPlacement.Mid(nFound) ); nFound = szPlacement.Find(':', nFound) + 1;
			wndpl.ptMaxPosition.x = atoi( szPlacement.Mid(nFound) ); nFound = szPlacement.Find(':', nFound) + 1;
			wndpl.ptMaxPosition.y = atoi( szPlacement.Mid(nFound) ); nFound = szPlacement.Find(':', nFound) + 1;
			wndpl.rcNormalPosition.left = atoi( szPlacement.Mid(nFound) ); nFound = szPlacement.Find(':', nFound) + 1;
			wndpl.rcNormalPosition.top = atoi( szPlacement.Mid(nFound) ); nFound = szPlacement.Find(':', nFound) + 1;
			wndpl.rcNormalPosition.right = atoi( szPlacement.Mid(nFound) ); nFound = szPlacement.Find(':', nFound) + 1;
			wndpl.rcNormalPosition.bottom = atoi( szPlacement.Mid(nFound) ); nFound = szPlacement.Find(':', nFound) + 1;
		}

		pCedtApp->SpawnDocumentFile(szPath, atoi(szLineNum), pwndpl);
		is >> szText; // get next token

	} else { // not recognized item
		is.getline(szText, 4096, '>'); // skip attributes
		is >> szText; // get next token
	}

	return TRUE;
}

BOOL CFileWindow::ParseItemAttribute(LPCTSTR lpszText, CMapStringToString & mapAttr)
{
	TCHAR * pBeg, * pEnd = (TCHAR *)lpszText;
	CString szAttrName, szAttrValue;

	mapAttr.RemoveAll();
	while( * pEnd && isspace(* pEnd) ) pEnd++;

	while( * pEnd ) {
		pBeg = pEnd; while( * pEnd && ! isspace(* pEnd) && * pEnd != '=' ) pEnd++;
		szAttrName = CString(pBeg, pEnd-pBeg);

		while( * pEnd && isspace(* pEnd) ) pEnd++;

		if( * pEnd == '=' ) { pEnd++; } 
		else { AfxMessageBox(IDS_ERR_PARSE_PRJ_ATTR); return FALSE; }

		while( * pEnd && isspace(* pEnd) ) pEnd++;

		if( * pEnd == '"' ) { pEnd++; } 
		else { AfxMessageBox(IDS_ERR_PARSE_PRJ_ATTR); return FALSE; }

		pBeg = pEnd; while( * pEnd && * pEnd != '"' ) pEnd++;
		szAttrValue = CString(pBeg, pEnd-pBeg);

		if( * pEnd == '"' ) { pEnd++; } 
		else { AfxMessageBox(IDS_ERR_PARSE_PRJ_ATTR); return FALSE; }

		mapAttr.SetAt( szAttrName, szAttrValue );
		while( * pEnd && isspace(* pEnd) ) pEnd++;
	}

	return TRUE;
}

BOOL CFileWindow::AddCategoryToProject(HTREEITEM hParent, LPCTSTR lpszCategory, BOOL bExpand)
{
	HTREEITEM hFound = FindProjectTreeChildItem(hParent, lpszCategory);
	if( hFound ) { AfxMessageBox(IDS_ERR_DUPLICATE_PRJ_ITEM); return FALSE; }

	HTREEITEM hInsert = InsertProjectTreeItem(hParent, lpszCategory, PROJECT_ITEM_CATEGORY, 0, "");
	if( ! hInsert ) { AfxMessageBox(IDS_ERR_INSERT_PRJ_ITEM); return FALSE; }

	// expand parent category item
	if( bExpand ) m_treProjectTree.Expand( hParent, TVE_EXPAND );

	return TRUE;
}

BOOL CFileWindow::AddLocalFileToProject(HTREEITEM hParent, LPCTSTR lpszPathName, BOOL bExpand)
{
	HTREEITEM hFound = FindProjectTreeChildItem(hParent, GetFileName(lpszPathName));
	if( hFound ) { AfxMessageBox(IDS_ERR_DUPLICATE_PRJ_ITEM); return FALSE; }

	HTREEITEM hInsert = InsertProjectTreeItem(hParent, GetFileName(lpszPathName), PROJECT_ITEM_LOCAL_FILE, 0, lpszPathName);
	if( ! hInsert ) { AfxMessageBox(IDS_ERR_INSERT_PRJ_ITEM); return FALSE; }

	// expand parent category item
	if( bExpand ) m_treProjectTree.Expand( hParent, TVE_EXPAND );

	return TRUE;
}

BOOL CFileWindow::AddRemoteFileToProject(HTREEITEM hParent, INT nAccount, LPCTSTR lpszPathName, BOOL bExpand)
{
	HTREEITEM hFound = FindProjectTreeChildItem(hParent, GetFileName(lpszPathName));
	if( hFound ) { AfxMessageBox(IDS_ERR_DUPLICATE_PRJ_ITEM); return FALSE; }

	HTREEITEM hInsert = InsertProjectTreeItem(hParent, GetFileName(lpszPathName), PROJECT_ITEM_REMOTE_FILE, nAccount, lpszPathName);
	if( ! hInsert ) { AfxMessageBox(IDS_ERR_INSERT_PRJ_ITEM); return FALSE; }

	// expand parent category item
	if( bExpand ) m_treProjectTree.Expand( hParent, TVE_EXPAND );

	return TRUE;
}

BOOL CFileWindow::AddLocalDirectoryToProject(HTREEITEM hParent, LPCTSTR lpszPathName, BOOL bExpand)
{
	HTREEITEM hFound = FindProjectTreeChildItem(hParent, GetFileName(lpszPathName));
	if( hFound ) { AfxMessageBox(IDS_ERR_DUPLICATE_PRJ_ITEM); return FALSE; }

	HTREEITEM hInsert = InsertProjectTreeItem(hParent, GetFileName(lpszPathName), PROJECT_ITEM_CATEGORY, 0, "");
	if( ! hInsert ) { AfxMessageBox(IDS_ERR_INSERT_PRJ_ITEM); return FALSE; }

	// expand parent category item
	if( bExpand ) m_treProjectTree.Expand( hParent, TVE_EXPAND );

	// looking for sub items to add to project
	CString szPath = lpszPathName; INT nLength = szPath.GetLength();
	if( szPath[nLength-1] == '\\' ) szPath += "*.*";
	else szPath += "\\*.*";

	CFileFind find; BOOL bFound = find.FindFile(szPath);

	while( bFound ) {
		bFound = find.FindNextFile();
		CString szFilePath = find.GetFilePath();

		if( ! find.IsDirectory() && ! find.IsHidden() ) AddLocalFileToProject(hInsert, szFilePath, FALSE);
		else if( find.IsDirectory() && ! find.IsDots() && ! find.IsHidden() ) AddLocalDirectoryToProject(hInsert, szFilePath, FALSE);
	}

	return TRUE;
}

BOOL CFileWindow::AddProjectCategoryToProject(HTREEITEM hParent, LPPROJECTITEMINFO lpInfo, BOOL bExpand)
{
	HTREEITEM hFound = FindProjectTreeChildItem(hParent, lpInfo->szText);
	if( hFound ) { AfxMessageBox(IDS_ERR_DUPLICATE_PRJ_ITEM); return FALSE; }

	HTREEITEM hInsert = InsertProjectTreeItem(hParent, lpInfo->szText, PROJECT_ITEM_CATEGORY, 0, "");
	if( ! hInsert ) { AfxMessageBox(IDS_ERR_INSERT_PRJ_ITEM); return FALSE; }

	// expand parent category item
	if( bExpand ) m_treProjectTree.Expand( hParent, TVE_EXPAND );

	// looking for sub items to add to project
	HTREEITEM hChild = m_treProjectTree.GetChildItem(lpInfo->hItem);
	while( hChild ) {
		LPPROJECTITEMINFO lpInfo2 = GetProjectItemInfo(hChild);

		if( lpInfo2->nItemType == PROJECT_ITEM_REMOTE_FILE ) AddRemoteFileToProject(hInsert, lpInfo2->nFtpAccount, lpInfo2->szPathName, FALSE);
		if( lpInfo2->nItemType == PROJECT_ITEM_LOCAL_FILE ) AddLocalFileToProject(hInsert, lpInfo2->szPathName, FALSE);
		else if( lpInfo2->nItemType == PROJECT_ITEM_CATEGORY ) AddProjectCategoryToProject(hInsert, lpInfo2, FALSE);

		hChild = m_treProjectTree.GetNextSiblingItem(hChild);
	}

	return FALSE;
}

LPPROJECTITEMINFO CFileWindow::GetProjectItemInfo(HTREEITEM hItem)
{
	if( ! hItem ) return NULL;
	return (LPPROJECTITEMINFO)m_treProjectTree.GetItemData(hItem); 
}

CString CFileWindow::GetProjectItemPathName(HTREEITEM hItem)
{
	if( ! hItem ) return "";
	LPPROJECTITEMINFO lpInfo = GetProjectItemInfo(hItem);
	return lpInfo->szPathName;
}

BOOL CFileWindow::EnableAllProjectButtons(BOOL bEnable)
{
	m_btnToolbarProject.EnableButton(ID_PRJ_NEW_CATEGORY, bEnable);
	m_btnToolbarProject.EnableButton(ID_PRJ_ADD_FILES_TO, bEnable);
	m_btnToolbarProject.EnableButton(ID_PRJ_ADD_ACTIVE_FILE, bEnable);
	m_btnToolbarProject.EnableButton(ID_PRJ_ADD_OPEN_FILES, bEnable);
	m_btnToolbarProject.EnableButton(ID_PRJ_ITEM_REMOVE, bEnable);

	return TRUE;
}

INT CALLBACK CFileWindow::CompareProjectItem(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	LPPROJECTITEMINFO lpInfo1 = (LPPROJECTITEMINFO)lParam1;
	LPPROJECTITEMINFO lpInfo2 = (LPPROJECTITEMINFO)lParam2;

	if( lpInfo1->nItemType == lpInfo2->nItemType ) {
		return _stricmp(lpInfo1->szText, lpInfo2->szText);
	} else return (lpInfo1->nItemType - lpInfo2->nItemType);
}

HTREEITEM CFileWindow::GetPointedProjectTreeItem()
{
	CPoint point; GetCursorPos( & point ); 
	m_treProjectTree.ScreenToClient( & point );

	HTREEITEM hItem; UINT nFlags;
	hItem = m_treProjectTree.HitTest( point, & nFlags );

	if( nFlags & TVHT_ONITEM ) return hItem;
	return NULL;
}

HTREEITEM CFileWindow::InsertProjectTreeItem(HTREEITEM hParent, LPCTSTR lpszText, INT nType, INT nAccount, LPCTSTR lpszPathName)
{
	HTREEITEM hItem = m_treProjectTree.InsertItem( lpszText, nType, nType, hParent );

	LPPROJECTITEMINFO lpInfo = new PROJECTITEMINFO;
	lpInfo->hItem = hItem;		strcpy(lpInfo->szText, lpszText);
	lpInfo->nItemType = nType;	lpInfo->nFtpAccount = nAccount;
	strcpy(lpInfo->szPathName, lpszPathName);

	BOOL bReturn = m_treProjectTree.SetItemData( hItem, (DWORD)lpInfo );

	TVSORTCB sort; sort.hParent = hParent; sort.lpfnCompare = CompareProjectItem; sort.lParam = 0L;
	BOOL bSorted = m_treProjectTree.SortChildrenCB( & sort );

	return hItem;
}

HTREEITEM CFileWindow::FindProjectTreeChildItem(HTREEITEM hParent, LPCTSTR lpszText)
{
	HTREEITEM hFound = m_treProjectTree.GetChildItem(hParent);
	while( hFound ) {
		CString szText = m_treProjectTree.GetItemText( hFound );
		if( ! szText.CompareNoCase(lpszText) ) return hFound;
		hFound = m_treProjectTree.GetNextSiblingItem( hFound );
	}
	return NULL;
}

BOOL CFileWindow::RemoveAllProjectItems()
{
	HTREEITEM hRoot = m_treProjectTree.GetRootItem();
	if( hRoot ) return RemoveProjectItem( hRoot );
	else return TRUE; // nothing to delete
}

BOOL CFileWindow::RemoveProjectItem(HTREEITEM hItem)
{
	if( ! hItem ) return FALSE;

	HTREEITEM hChild = m_treProjectTree.GetChildItem( hItem );
	while( hChild ) { // delete childrens
		if( ! RemoveProjectItem( hChild ) ) return FALSE; // recursive call
		hChild = m_treProjectTree.GetChildItem( hItem );
	}

	delete GetProjectItemInfo( hItem ); // delete item data first
	return m_treProjectTree.DeleteItem( hItem );
}

BOOL CFileWindow::IsParentProjectItem(HTREEITEM hTest, HTREEITEM hItem)
{
	HTREEITEM hParent = m_treProjectTree.GetParentItem(hItem);
	while( hParent ) {
		if( hParent == hTest ) return TRUE;
		hParent = m_treProjectTree.GetParentItem(hParent);
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// Handlers
void CFileWindow::OnItemexpandingProjectTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;

	UINT uAction = pNMTreeView->action;
	HTREEITEM hRoot = m_treProjectTree.GetRootItem();

	// prevent root item to be collapsed
	if( uAction == TVE_COLLAPSE && hItem == hRoot ) * pResult = 1;
	else * pResult = 0;
}

void CFileWindow::OnClickProjectTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
}

void CFileWindow::OnDblclkProjectTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HTREEITEM hItem = GetPointedProjectTreeItem();
	if( ! hItem ) { * pResult = 0; return; }

	LPPROJECTITEMINFO lpInfo = GetProjectItemInfo(hItem);
	OpenProjectItem(lpInfo);

	*pResult = 0;
}

void CFileWindow::OnRclickProjectTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HTREEITEM hItem = GetPointedProjectTreeItem();
	if( hItem ) m_treProjectTree.SelectItem(hItem);

	CPoint point; GetCursorPos( & point ); 
	CMenu * pMenu, context; context.LoadMenu(IDR_FILE_WINDOW);

	if( hItem ) {
		INT nImage, nSelectedImage; m_treProjectTree.GetItemImage(hItem, nImage, nSelectedImage);
		CCedtApp * pApp = (CCedtApp *)AfxGetApp();

		if( ! IsSelectedProjectItemRoot() ) {
			switch( nImage ) {
			case PROJECT_ITEM_LOCAL_FILE:
				pMenu = GetSubMenuByText( & context, "PRJ_LOCAL"    ); break;
			case PROJECT_ITEM_REMOTE_FILE:
				pMenu = GetSubMenuByText( & context, "PRJ_REMOTE"   ); break;
			case PROJECT_ITEM_CATEGORY:
				pMenu = GetSubMenuByText( & context, "PRJ_CATEGORY" ); break;
			default: // this should not occur !!!
				pMenu = GetSubMenuByText( & context, "PRJ_NULL"     ); break;
			}
		} else if( pApp->IsProjectLoaded() ) {
			pMenu = GetSubMenuByText( & context, "PRJ_ROOT1" );
		} else pMenu = GetSubMenuByText( & context, "PRJ_ROOT0" );
	} else pMenu = GetSubMenuByText( & context, "PRJ_NULL" );

	UINT nFlags = TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON;
	pMenu->TrackPopupMenu(nFlags, point.x, point.y, AfxGetMainWnd());

	*pResult = 0;
}

void CFileWindow::OnBeginlabeleditProjectTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMTVDISPINFO* pNMTVDISPINFO = (NMTVDISPINFO*)pNMHDR;
	HTREEITEM hItem = pNMTVDISPINFO->item.hItem;

	LPPROJECTITEMINFO lpInfo = GetProjectItemInfo( hItem );
	if( lpInfo->nItemType == PROJECT_ITEM_CATEGORY ) m_bLabelEditing = TRUE;

	* pResult = m_bLabelEditing ? 0 : 1;
}

void CFileWindow::OnEndlabeleditProjectTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMTVDISPINFO* pNMTVDISPINFO = (NMTVDISPINFO*)pNMHDR;
	HTREEITEM hItem = pNMTVDISPINFO->item.hItem;

	CString szOldName = m_treProjectTree.GetItemText(hItem);
	CString szNewName = pNMTVDISPINFO->item.pszText;
	if( szNewName.GetLength() && szOldName.CompareNoCase(szNewName) ) {
		m_treProjectTree.SetItemText(hItem, szNewName);
	}

	m_bLabelEditing = FALSE;

	* pResult = 0;
}

void CFileWindow::OnBegindragProjectTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTREEVIEW* pNMTREEVIEW = (NMTREEVIEW*)pNMHDR;
	HTREEITEM hItem = pNMTREEVIEW->itemNew.hItem;

	// root item should not be allowed to drag
	if( hItem == m_treProjectTree.GetRootItem() ) { * pResult = 0; return; }

	LPPROJECTITEMINFO lpInfo = GetProjectItemInfo(hItem);
	UINT size = sizeof(PROJECTITEMINFO);

	HGLOBAL hMemory = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT, size);
	if( ! hMemory ) { * pResult = 0; return; }

	PROJECTITEMINFO * pInfo = (PROJECTITEMINFO *)::GlobalLock(hMemory);
	if( ! pInfo ) { ::GlobalFree(hMemory); * pResult = 0; return; }

	CopyMemory( pInfo, lpInfo, size );
	::GlobalUnlock(hMemory);

	FORMATETC etc = { g_uClipbrdFormatProjectItem, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	m_oleDataSource.CacheGlobalData( g_uClipbrdFormatProjectItem, hMemory, & etc );

	DROPEFFECT eff = m_oleDataSource.DoDragDrop(DROPEFFECT_COPY | DROPEFFECT_MOVE);

	if( eff == DROPEFFECT_MOVE ) {
		// item has been moved - delete tree item 
		RemoveProjectItem(hItem);
	} else if( eff == DROPEFFECT_NONE ) {
		// drag and drop has been canceled 
		::GlobalFree(hMemory);
	}

	m_oleDataSource.Empty(); 

	* pResult = 0;
}

DROPEFFECT CFileWindow::OnDragOverProjectTree(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	static CPoint ptPrev = CPoint(-1, -1);
	static INT nStayCount = 0;

	if( m_nDragObjectType == DRAG_OBJECT_HDROP || m_nDragObjectType == DRAG_OBJECT_PROJECT_ITEM || m_nDragObjectType == DRAG_OBJECT_FILETAB_ITEM ) {
		ClientToScreen( & point ); m_treProjectTree.ScreenToClient( & point );
		CRect rectNonScroll; m_treProjectTree.GetClientRect( & rectNonScroll );
		
		if( rectNonScroll.Height() > 2 * DRAG_SCROLL_REGION ) rectNonScroll.DeflateRect(0, DRAG_SCROLL_REGION);
		if( rectNonScroll.Width()  > 2 * DRAG_SCROLL_REGION ) rectNonScroll.DeflateRect(DRAG_SCROLL_REGION, 0);

		if( ! rectNonScroll.PtInRect(point) ) {
			if( point.y > rectNonScroll.bottom    ) m_treProjectTree.SendMessage(WM_VSCROLL, SB_LINEDOWN, 0L);
			else if( point.y < rectNonScroll.top  ) m_treProjectTree.SendMessage(WM_VSCROLL, SB_LINEUP,   0L);

			if( point.x > rectNonScroll.right     ) m_treProjectTree.SendMessage(WM_HSCROLL, SB_LINEDOWN, 0L);
			else if( point.x < rectNonScroll.left ) m_treProjectTree.SendMessage(WM_HSCROLL, SB_LINEUP,   0L);
		}

		UINT nFlags; HTREEITEM hItem = m_treProjectTree.HitTest( point, & nFlags );

		if( hItem ) {
			LPPROJECTITEMINFO lpInfo = GetProjectItemInfo(hItem);

			if( lpInfo->nItemType == PROJECT_ITEM_PROJECT || lpInfo->nItemType == PROJECT_ITEM_CATEGORY ) {
				m_treProjectTree.SelectDropTarget(hItem);
				UINT uState = m_treProjectTree.GetItemState(hItem, TVIS_EXPANDED);

				if( m_treProjectTree.ItemHasChildren(hItem) && ! (uState & TVIS_EXPANDED) ) {
					if( ptPrev == point ) nStayCount++;
					else { ptPrev = point; nStayCount = 0; }

					if( nStayCount >= DRAG_EXPAND_COUNT ) {
						m_treProjectTree.Expand(hItem, TVE_EXPAND);
						ptPrev = point; nStayCount = 0;
					}
				} else ptPrev = CPoint(-1, -1);

				if( m_nDragObjectType == DRAG_OBJECT_HDROP ) return DROPEFFECT_COPY;
				else if( m_nDragObjectType == DRAG_OBJECT_PROJECT_ITEM ) return (dwKeyState & MK_CONTROL) ? DROPEFFECT_COPY : DROPEFFECT_MOVE;
				else if( m_nDragObjectType == DRAG_OBJECT_FILETAB_ITEM ) return DROPEFFECT_COPY;
				else return DROPEFFECT_NONE;

			} else {
				m_treProjectTree.SelectDropTarget(NULL);
				ptPrev = CPoint(-1, -1); return DROPEFFECT_NONE;
			}

		} else {
			m_treProjectTree.SelectDropTarget(NULL);
			ptPrev = CPoint(-1, -1); return DROPEFFECT_NONE;
		}

	} else {
		m_treProjectTree.SelectDropTarget(NULL);
		ptPrev = CPoint(-1, -1); return DROPEFFECT_NONE;
	}
}

BOOL CFileWindow::OnDropProjectTree(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	if( pDataObject->IsDataAvailable(CF_HDROP) ) {
		ClientToScreen( & point ); m_treProjectTree.ScreenToClient( & point );
		UINT nFlags; HTREEITEM hItem = m_treProjectTree.HitTest( point, & nFlags );

		if( hItem && (dropEffect == DROPEFFECT_COPY) ) {
			LPPROJECTITEMINFO lpInfo = GetProjectItemInfo(hItem);

			if( lpInfo->nItemType == PROJECT_ITEM_PROJECT || lpInfo->nItemType == PROJECT_ITEM_CATEGORY ) {
				HGLOBAL hMemory = pDataObject->GetGlobalData(CF_HDROP);
				if( ! hMemory ) return FALSE;

				HDROP hDrop = (HDROP)::GlobalLock(hMemory);
				if( ! hDrop ) { ::GlobalUnlock(hMemory); return FALSE; }

				TCHAR szNextFile[MAX_PATH];
				UINT uNumFiles = DragQueryFile( hDrop, (UINT)-1, NULL, 0 );
				for( UINT uFile = 0; uFile < uNumFiles; uFile++ ) {
					if( DragQueryFile( hDrop, uFile, szNextFile, MAX_PATH ) > 0 ) {
						if( VerifyFilePath(szNextFile) ) AddLocalFileToProject(hItem, szNextFile);
						else AddLocalDirectoryToProject(hItem, szNextFile);
					}
				}

				::GlobalUnlock(hMemory);
				::GlobalFree(hMemory);

				return TRUE;

			} else return FALSE;

		} else return FALSE;

	} else if( pDataObject->IsDataAvailable(g_uClipbrdFormatProjectItem) ) {
		ClientToScreen( & point ); m_treProjectTree.ScreenToClient( & point );
		UINT nFlags; HTREEITEM hItem = m_treProjectTree.HitTest( point, & nFlags );

		if( hItem && (dropEffect == DROPEFFECT_COPY || dropEffect == DROPEFFECT_MOVE) ) {
			LPPROJECTITEMINFO lpInfo = GetProjectItemInfo(hItem);

			if( lpInfo->nItemType == PROJECT_ITEM_PROJECT || lpInfo->nItemType == PROJECT_ITEM_CATEGORY ) {
				HGLOBAL hMemory = pDataObject->GetGlobalData(g_uClipbrdFormatProjectItem);
				if( ! hMemory ) return FALSE;

				LPPROJECTITEMINFO lpInfo2 = (LPPROJECTITEMINFO)::GlobalLock(hMemory);
				if( ! lpInfo2 ) { ::GlobalUnlock(hMemory); return FALSE; }

				if( lpInfo2->hItem == hItem || GetParentProjectItem(lpInfo2->hItem) == hItem ) {
					UINT nFormatID = (dropEffect == DROPEFFECT_COPY) ? IDS_ERR_COPY_PRJ_ITEM1 : IDS_ERR_MOVE_PRJ_ITEM1;
					CString szMessage; szMessage.Format(nFormatID, lpInfo2->szText);
					AfxMessageBox(szMessage, MB_OK | MB_ICONSTOP); ::GlobalUnlock(hMemory); return FALSE; 
				} else if( IsParentProjectItem(lpInfo2->hItem, hItem) ) {
					UINT nFormatID = (dropEffect == DROPEFFECT_COPY) ? IDS_ERR_COPY_PRJ_ITEM2 : IDS_ERR_MOVE_PRJ_ITEM2;
					CString szMessage; szMessage.Format(nFormatID, lpInfo2->szText);
					AfxMessageBox(szMessage, MB_OK | MB_ICONSTOP); ::GlobalUnlock(hMemory); return FALSE; 
				}

				if( lpInfo2->nItemType == PROJECT_ITEM_REMOTE_FILE ) AddRemoteFileToProject(hItem, lpInfo2->nFtpAccount, lpInfo2->szPathName);
				else if( lpInfo2->nItemType == PROJECT_ITEM_LOCAL_FILE ) AddLocalFileToProject(hItem, lpInfo2->szPathName);
				else if( lpInfo2->nItemType == PROJECT_ITEM_CATEGORY ) AddProjectCategoryToProject(hItem, lpInfo2);

				::GlobalUnlock(hMemory);
				::GlobalFree(hMemory);

				return TRUE;

			} else return FALSE;

		} else return FALSE;

	} else if( pDataObject->IsDataAvailable(g_uClipbrdFormatFileTabItem) ) {
		ClientToScreen( & point ); m_treProjectTree.ScreenToClient( & point );
		UINT nFlags; HTREEITEM hItem = m_treProjectTree.HitTest( point, & nFlags );

		if( hItem && (dropEffect == DROPEFFECT_COPY) ) {
			LPPROJECTITEMINFO lpInfo = GetProjectItemInfo(hItem);

			if( lpInfo->nItemType == PROJECT_ITEM_PROJECT || lpInfo->nItemType == PROJECT_ITEM_CATEGORY ) {
				HGLOBAL hMemory = pDataObject->GetGlobalData(g_uClipbrdFormatFileTabItem);
				if( ! hMemory ) return FALSE;

				TCITEM * pItem = (TCITEM *)::GlobalLock(hMemory);
				if( ! pItem ) { ::GlobalUnlock(hMemory); return FALSE; }

				CMDIChildWnd * pChild = (CMDIChildWnd *)pItem->lParam;
				CCedtDoc * pDoc = (CCedtDoc *)pChild->GetActiveDocument();

				if( pDoc->IsNewFileNotSaved() ) { ::GlobalUnlock(hMemory); return FALSE; }

				if( pDoc->IsRemoteFile() ) AddRemoteFileToProject(hItem, pDoc->GetFtpAccountNumber(), pDoc->GetRemotePathName());
				else AddLocalFileToProject(hItem, pDoc->GetPathName());

				::GlobalUnlock(hMemory);
				::GlobalFree(hMemory);

				return TRUE;

			} else return FALSE;

		} else return FALSE;

	} else return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// Action Functions
BOOL CFileWindow::OpenProjectItem(LPPROJECTITEMINFO lpInfo)
{
	if( lpInfo->nItemType == PROJECT_ITEM_REMOTE_FILE ) { // remote file
		CCedtApp * pApp = (CCedtApp *)AfxGetApp(); if( ! pApp ) return FALSE;
		return pApp->PostOpenRemoteDocumentFile( lpInfo->nFtpAccount, lpInfo->szPathName, 0 );
	} else if( lpInfo->nItemType == PROJECT_ITEM_LOCAL_FILE ) { // local file
		if( ! VerifyFilePath( lpInfo->szPathName ) ) return FALSE;
		CCedtApp * pApp = (CCedtApp *)AfxGetApp(); if( ! pApp ) return FALSE;
		return pApp->PostOpenDocumentFile( lpInfo->szPathName, 0 );
	} else return FALSE;
}

BOOL CFileWindow::ExecuteProjectItem(LPPROJECTITEMINFO lpInfo)
{
	if( lpInfo->nItemType != PROJECT_ITEM_LOCAL_FILE ) return FALSE;

	CString szPath = lpInfo->szPathName;
	if( ! VerifyFilePath( szPath ) ) return FALSE;

	CWnd * pWnd = AfxGetMainWnd(); if( ! pWnd ) return FALSE;
	HINSTANCE hResult = ::ShellExecute(NULL, "open", szPath, NULL, NULL, SW_SHOWNORMAL);
	return ((UINT)hResult > 32) ? TRUE : FALSE;
}

BOOL CFileWindow::ShowPropProjectItem(LPPROJECTITEMINFO lpInfo)
{
	if( lpInfo->nItemType != PROJECT_ITEM_LOCAL_FILE ) return FALSE;

	CString szPath = lpInfo->szPathName;
	if( ! VerifyFilePath( szPath ) ) return FALSE;

	SHELLEXECUTEINFO sei; ZeroMemory( & sei, sizeof(sei) );
	sei.cbSize = sizeof(sei);
	sei.lpFile = szPath;
	sei.lpVerb = _T("properties");
	sei.fMask  = SEE_MASK_INVOKEIDLIST;
	sei.nShow  = SW_SHOWNORMAL;

	return ShellExecuteEx( & sei ); 
}

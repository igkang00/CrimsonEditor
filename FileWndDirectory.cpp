#include "stdafx.h"
#include "cedtHeader.h"
#include "SortStringArray.h"
#include "FolderDialog.h"


BOOL CFileWindow::InitLocalDriveList(LPCTSTR lpszInitialDriveName)
{
	TCHAR szBuffer[MAX_PATH]; // size MAX_PATH should be enough
	if( ! GetLogicalDriveStrings(sizeof(szBuffer), szBuffer) ) return FALSE;

	m_cmbLocalDrive.ResetContent();
	m_arrLocalDrive.RemoveAll();

	CString szDriveName; 
	TCHAR * pDrive = szBuffer;

	while( * pDrive ) {
		INT nType = GetDriveType(pDrive);

		if( nType == DRIVE_FIXED || nType == DRIVE_RAMDISK || nType == DRIVE_REMOTE ) {
			TCHAR szVolumeName[1024], szFileSystemName[1024];
			DWORD dwSerialNumber, dwMaxComponentLength, dwFileSystemFlags;

			if( GetVolumeInformation(pDrive, szVolumeName, sizeof(szVolumeName), & dwSerialNumber,
				& dwMaxComponentLength, & dwFileSystemFlags, szFileSystemName, sizeof(szFileSystemName)) ) {
				szDriveName.Format("[%c:] %s", toupper(pDrive[0]), szVolumeName);
			} else if( nType == DRIVE_FIXED ) {
				szDriveName.Format("[%c:] Local Drive", toupper(pDrive[0]));
			} else if( nType == DRIVE_RAMDISK ) {
				szDriveName.Format("[%c:] RAM Disk", toupper(pDrive[0]));
			} else if( nType == DRIVE_REMOTE ) {
				szDriveName.Format("[%c:] Network Drive", toupper(pDrive[0]));
			}
		} else { // other types of drive takes time to GetVolumeInformation()
			switch( nType ) {
			case DRIVE_CDROM: 
				szDriveName.Format("[%c:] CD-ROM", toupper(pDrive[0])); break;
			case DRIVE_REMOVABLE: 
				szDriveName.Format("[%c:] Removable", toupper(pDrive[0])); break;
			default: 
				szDriveName.Format("[%c:] Unknown Type", toupper(pDrive[0])); break;
			}
		}

		m_cmbLocalDrive.AddString( szDriveName );
		m_arrLocalDrive.Add( szDriveName.Mid(1, 2) );

		pDrive += strlen(pDrive) + 1;
	}

	// register Desktop directory to drive list
	if( SHGetSpecialFolderPath(NULL, szBuffer, CSIDL_DESKTOPDIRECTORY, TRUE) ) {
		szDriveName.LoadString(IDS_NAME_DESKTOP_DIR);
		m_cmbLocalDrive.AddString( szDriveName );
		m_arrLocalDrive.Add( szBuffer );
	}

	// register My Documents directory to drive list
	if( SHGetSpecialFolderPath(NULL, szBuffer, CSIDL_PERSONAL, TRUE) ) {
		szDriveName.LoadString(IDS_NAME_MY_DOCUMENTS);
		m_cmbLocalDrive.AddString( szDriveName );
		m_arrLocalDrive.Add( szBuffer );
	}

	if( lpszInitialDriveName ) {
	//	nDrive = m_cmbLocalDrive.FindString(-1, lpszInitialDriveName);
	//	if( nDrive != CB_ERR ) m_cmbLocalDrive.SetCurSel(nDrive);

		INT nCount = m_arrLocalDrive.GetSize();
		INT nDrive = CB_ERR;

		for( INT i = 0; i < nCount; i++ ) {
			if( ! m_arrLocalDrive[i].CompareNoCase( lpszInitialDriveName ) ) { nDrive = i; break; }
		}

		if( nDrive != CB_ERR ) {
			m_cmbLocalDrive.SetCurSel(nDrive);
			m_szPrevDriveName = lpszInitialDriveName;
		}
	}

	return TRUE;
}

BOOL CFileWindow::InitFileFilterList(LPCTSTR lpszComposedFilter, INT nInitialFilter, CALLBACK_SELCHANGE fcnCallback)
{
	m_cmbFileFilter.ResetContent(); if( nInitialFilter < 0 ) nInitialFilter = 0; 
	ParseFileFilter(m_arrFilterDescription, m_arrFilterExtensions, lpszComposedFilter);

	INT nSize = m_arrFilterDescription.GetSize();
	for(INT i = 0; i < nSize; i++) m_cmbFileFilter.InsertString( i, m_arrFilterDescription.GetAt(i) );

	m_cmbFileFilter.SetCurSel(nInitialFilter);
	m_nPrevFilterIndex = nInitialFilter;

	m_fcnCallbackSelchangeFileFilter = fcnCallback;

	return TRUE;
}

BOOL CFileWindow::GetBrowsingDirectory(CString & szDirectory)
{
	HTREEITEM hItem = m_treDirectoryTree.GetSelectedItem();
	CString szFullPath = GetDirectoryItemPathName( hItem );

	if( ! VerifyFilePath( szFullPath ) ) szDirectory = szFullPath;
	else szDirectory = GetFileDirectory( szFullPath );

	return TRUE;
}

BOOL CFileWindow::SetBrowsingDirectory(LPCTSTR lpszDirectory)
{
	CString szDriveName( lpszDirectory, 2 );
	INT nCount = m_arrLocalDrive.GetSize();
	for( INT i = nCount-1; i >= 0; i-- ) { // search in reverse order
		CString & szLocalDrive = m_arrLocalDrive[i];
		if( ! strnicmp(szLocalDrive, lpszDirectory, szLocalDrive.GetLength()) ) { szDriveName = szLocalDrive; break; }
	}
	BOOL bRemake = szDriveName.CompareNoCase( m_szPrevDriveName );

	if( bRemake ) SelectLocalDriveList( szDriveName );

	CWaitCursor wait;
	m_treDirectoryTree.SetRedraw(FALSE);

	HTREEITEM hRoot = m_treDirectoryTree.GetRootItem();
	if( ! hRoot || bRemake ) RemakeDirectoryTreeRoot(szDriveName);

	ExpandDirectoryTreePath(lpszDirectory);

	m_treDirectoryTree.SetRedraw(TRUE);
	return TRUE;
}

BOOL CFileWindow::IsSelectedDirectoryItemRoot()
{
	HTREEITEM hItem = m_treDirectoryTree.GetSelectedItem();
	HTREEITEM hRoot = m_treDirectoryTree.GetRootItem();
	return (hItem == hRoot);
}

CString CFileWindow::GetSelectedDirectoryItemText()
{
	HTREEITEM hItem = m_treDirectoryTree.GetSelectedItem();
	if( ! hItem ) return "";
	return m_treDirectoryTree.GetItemText(hItem);
}

BOOL CFileWindow::OpenSelectedDirectoryItem()
{
	HTREEITEM hItem = GetSelectedDirectoryTreeItem();
	if( ! hItem ) return FALSE;

	CString szPathName = GetDirectoryItemPathName(hItem);
	return OpenDirectoryItem(szPathName);
}

BOOL CFileWindow::ExecuteSelectedDirectoryItem()
{
	HTREEITEM hItem = GetSelectedDirectoryTreeItem();
	if( ! hItem ) return FALSE;

	CString szPathName = GetDirectoryItemPathName(hItem);
	return ExecuteDirectoryItem(szPathName);
}

BOOL CFileWindow::ExploreSelectedDirectoryItem()
{
	HTREEITEM hItem = GetSelectedDirectoryTreeItem();
	if( ! hItem ) return FALSE;

	CString szPathName = GetDirectoryItemPathName(hItem);
	return ExploreDirectoryItem(szPathName);
}

BOOL CFileWindow::FindInSelectedDirectoryItem()
{
	HTREEITEM hItem = GetSelectedDirectoryTreeItem();
	if( ! hItem ) return FALSE;

	CString szPathName = GetDirectoryItemPathName(hItem);
	return FindInDirectoryItem(szPathName);
}

BOOL CFileWindow::ShowPropSelectedDirectoryItem()
{
	HTREEITEM hItem = GetSelectedDirectoryTreeItem();
	if( ! hItem ) return FALSE;

	CString szPathName = GetDirectoryItemPathName(hItem);
	return ShowPropDirectoryItem(szPathName);
}

BOOL CFileWindow::MoveSelectedDirectoryItem()
{
	HTREEITEM hItem = GetSelectedDirectoryTreeItem();
	if( hItem && m_treDirectoryTree.GetParentItem(hItem) ) {
		CString szPathName = GetDirectoryItemPathName(hItem);
		CString szDirectory = GetFileDirectory(szPathName);
		if( ! AskDestinationDirectory( szDirectory ) ) return FALSE;
		if( MoveToDirectoryItem(szPathName, szDirectory) && ! VerifyPathName(szPathName) ) {
			m_treDirectoryTree.SelectItem( m_treDirectoryTree.GetParentItem(hItem) );
			m_treDirectoryTree.DeleteItem( hItem ); return TRUE;
		} else return FALSE;
	} else return FALSE;
}

BOOL CFileWindow::CopySelectedDirectoryItem()
{
	HTREEITEM hItem = GetSelectedDirectoryTreeItem();
	if( hItem && m_treDirectoryTree.GetParentItem(hItem) ) {
		CString szPathName = GetDirectoryItemPathName(hItem);
		CString szDirectory = GetFileDirectory(szPathName);
		if( ! AskDestinationDirectory(szDirectory) ) return FALSE;
		return CopyToDirectoryItem(szPathName, szDirectory);
	} else return FALSE;
}

BOOL CFileWindow::DeleteSelectedDirectoryItem()
{
	HTREEITEM hItem = GetSelectedDirectoryTreeItem();
	if( hItem && m_treDirectoryTree.GetParentItem(hItem) ) {
		CString szPathName = GetDirectoryItemPathName(hItem);
		SetAsCurrentDirectory( GetFileDirectory(szPathName) );
		if( DeleteDirectoryItem(szPathName) && ! VerifyPathName(szPathName) ) {
			m_treDirectoryTree.SelectItem( m_treDirectoryTree.GetParentItem(hItem) );
			m_treDirectoryTree.DeleteItem( hItem ); return TRUE;
		} else return FALSE;
	} else return FALSE;
}

BOOL CFileWindow::RenameSelectedDirectoryItem()
{
	HTREEITEM hItem = GetSelectedDirectoryTreeItem();
	if( hItem && m_treDirectoryTree.GetParentItem(hItem) ) {
		CEdit * pEdit = m_treDirectoryTree.EditLabel(hItem);
		return (pEdit != NULL);
	} else return FALSE;
}

BOOL CFileWindow::CreateNewFolderInSelectedDirectoryItem()
{
	HTREEITEM hItem = GetSelectedDirectoryTreeItem();
	if( ! hItem ) return FALSE;

	CString szFolderName, szPath = GetDirectoryItemPathName(hItem);
	UINT uState = m_treDirectoryTree.GetItemState(hItem, TVIS_EXPANDED);
	if( m_treDirectoryTree.ItemHasChildren(hItem) && ! (uState & TVIS_EXPANDED) ) {
		DeleteDirectoryTreeChildren(hItem);
		InsertDirectoryTreeChildren(hItem, szPath);
		m_treDirectoryTree.Expand(hItem, TVE_EXPAND);
	}

	if( CreateNewFolderInDirectory(szPath, szFolderName) ) {
		CString szTemp; szTemp.Format("%s\\%s", szPath, szFolderName);
		HTREEITEM hInsert = InsertDirectoryTreeItem(hItem, szTemp);
		m_treDirectoryTree.Expand(hItem, TVE_EXPAND);

		CEdit * pEdit = m_treDirectoryTree.EditLabel(hInsert);
		return (pEdit != NULL);
	} else return FALSE;
}

BOOL CFileWindow::CreateNewDocumentInSelectedDirectoryItem()
{
	HTREEITEM hItem = GetSelectedDirectoryTreeItem();
	if( ! hItem ) return FALSE;

	CString szFileName, szPath = GetDirectoryItemPathName(hItem);
	UINT uState = m_treDirectoryTree.GetItemState(hItem, TVIS_EXPANDED);
	if( m_treDirectoryTree.ItemHasChildren(hItem) && ! (uState & TVIS_EXPANDED) ) {
		DeleteDirectoryTreeChildren(hItem);
		InsertDirectoryTreeChildren(hItem, szPath);
		m_treDirectoryTree.Expand(hItem, TVE_EXPAND);
	}

	if( CreateNewDocumentInDirectory(szPath, szFileName) ) {
		CString szTemp; szTemp.Format("%s\\%s", szPath, szFileName);
		HTREEITEM hInsert = InsertDirectoryTreeItem(hItem, szTemp);
		m_treDirectoryTree.Expand(hItem, TVE_EXPAND);

		CEdit * pEdit = m_treDirectoryTree.EditLabel(hInsert);
		return (pEdit != NULL);
	} else return FALSE;
}

BOOL CFileWindow::SetAsWorkingDirSelectedDirectoryItem()
{
	HTREEITEM hItem = GetSelectedDirectoryTreeItem();
	if( ! hItem ) return FALSE;

	CString szPathName = GetDirectoryItemPathName(hItem);
	return SetAsWorkingDirectory(szPathName);
}

BOOL CFileWindow::RefreshSelectedDirectoryItem()
{
	CWaitCursor wait;
	m_treDirectoryTree.SetRedraw(FALSE);

	HTREEITEM hItem = GetSelectedDirectoryTreeItem();
	BOOL bResult = RefreshDirectoryItem(hItem);

	m_treDirectoryTree.SetRedraw(TRUE);
	return bResult;
}



/////////////////////////////////////////////////////////////////////////////
// Inner Functions
BOOL CFileWindow::SelectLocalDriveList(LPCTSTR lpszDriveName)
{
//	INT nDrive = m_cmbLocalDrive.FindString(-1, lpszDriveName);
//	if( nDrive == CB_ERR ) return FALSE;
//	m_cmbLocalDrive.SetCurSel(nDrive);
//	return TRUE;

	INT nCount = m_arrLocalDrive.GetSize();
	INT nDrive = CB_ERR;

	for( INT i = 0; i < nCount; i++ ) {
		if( ! m_arrLocalDrive[i].CompareNoCase( lpszDriveName ) ) { nDrive = i; break; }
	}

	if( nDrive != CB_ERR ) { 
		m_cmbLocalDrive.SetCurSel(nDrive);
		m_szPrevDriveName = lpszDriveName;
		return TRUE; 
	} else return FALSE;
}

CString CFileWindow::GetActiveLocalDriveName()
{
	INT nDrive = m_cmbLocalDrive.GetCurSel();
	if( nDrive == CB_ERR ) return "C:";

//	CString szDrive; m_cmbLocalDrive.GetLBText(nDrive, szDrive);
//	return szDrive.Mid(1, 2);

	return m_arrLocalDrive[nDrive];
}

INT CFileWindow::GetActiveFileFilterIndex()
{
	INT nFilter = m_cmbFileFilter.GetCurSel();
	if( nFilter == CB_ERR ) return 0;
	return nFilter;
}

CString CFileWindow::GetActiveFileFilterString()
{
	INT nFilter = m_cmbFileFilter.GetCurSel();
	if( nFilter == CB_ERR ) return "*.*";
	return m_arrFilterExtensions.GetAt(nFilter);
}


BOOL CFileWindow::DirectoryHasChildren(LPCTSTR lpszPath)
{
	CString szPath = lpszPath; INT nLength = szPath.GetLength();
	if( szPath[nLength-1] == '\\' ) szPath += "*.*";
	else szPath += "\\*.*";

	CString szFilter = GetActiveFileFilterString();
	CFileFind find; BOOL bFound = find.FindFile(szPath);

	while( bFound ) {
		bFound = find.FindNextFile();
		CString szFilePath = find.GetFilePath();

		if( ! find.IsDirectory() && ! find.IsHidden() && MatchFileFilter(szFilePath, szFilter) ) return TRUE;
		else if( find.IsDirectory() && ! find.IsDots() && ! find.IsHidden() ) return TRUE;
	}

	return FALSE;
}

CString CFileWindow::GetDirectoryItemPathName(HTREEITEM hItem)
{
	HTREEITEM hRoot = m_treDirectoryTree.GetRootItem();
	CString szTemp, szReturn = "";

	while( hItem ) {
		if( hItem == hRoot ) szTemp = GetActiveLocalDriveName();
		else szTemp = m_treDirectoryTree.GetItemText( hItem );

		if( ! szReturn.GetLength() ) szReturn = szTemp;
		else szReturn = szTemp + "\\" + szReturn;

		hItem = m_treDirectoryTree.GetParentItem( hItem );
	}

	TRACE1("GetDirectoryItemPathName: %s\n", szReturn);
	return szReturn;
}

BOOL CFileWindow::RemakeDirectoryTreeRoot(LPCTSTR lpszDriveName)
{
	m_treDirectoryTree.DeleteAllItems();
	HTREEITEM hRoot = InsertDirectoryTreeRoot(lpszDriveName);

	if( DirectoryHasChildren(lpszDriveName) ) {
		InsertDirectoryTreeChildren(hRoot, lpszDriveName);
		m_treDirectoryTree.Expand(hRoot, TVE_EXPAND);
	}

	m_treDirectoryTree.EnsureVisible(hRoot);
	m_treDirectoryTree.SelectItem(hRoot);

	return TRUE;
}

BOOL CFileWindow::ExpandDirectoryTreePath(LPCTSTR lpszPathName)
{
	TCHAR szPathName[MAX_PATH]; strcpy( szPathName, lpszPathName );
	if( szPathName[strlen(szPathName)-1] != '\\' ) strcat( szPathName, "\\" );

	HTREEITEM hItem = m_treDirectoryTree.GetRootItem();
	CString szRoot = GetActiveLocalDriveName();

	if( strnicmp(szPathName, szRoot, szRoot.GetLength()) ) return FALSE;
	TCHAR * pText = szPathName + szRoot.GetLength() + 1;

	INT nLen = strlen(szPathName);
	for(INT i = szRoot.GetLength() + 1; i < nLen; i++) {
		if( szPathName[i] == '\\' ) {
			szPathName[i] = '\0';

			hItem = FindDirectoryTreeChildItem(hItem, pText); 
			if( ! hItem ) break; // not found

			CString szPath = GetDirectoryItemPathName(hItem);
			UINT uState = m_treDirectoryTree.GetItemState(hItem, TVIS_EXPANDED);

			if( m_treDirectoryTree.ItemHasChildren(hItem) && ! (uState & TVIS_EXPANDED) ) {
				DeleteDirectoryTreeChildren(hItem);
				InsertDirectoryTreeChildren(hItem, szPath);
				m_treDirectoryTree.Expand(hItem, TVE_EXPAND);
			}

			pText += strlen(pText) + 1;
		}
	}

	if( hItem ) {
		m_treDirectoryTree.EnsureVisible(hItem);
		m_treDirectoryTree.SelectItem(hItem);
	}

	return hItem ? TRUE : FALSE;
}

BOOL CFileWindow::RefreshDirectoryItem(HTREEITEM hItem)
{
	if( ! hItem ) return FALSE;

	CString szTest = GetDirectoryItemPathName(hItem);
	CString szFind = "";

	if( hItem == m_treDirectoryTree.GetRootItem() ) /* skip test for root item */ ;
	else if( ! VerifyPathName(szTest) ) hItem = m_treDirectoryTree.GetParentItem(hItem);
	else if( VerifyFilePath(szTest) ) { hItem = m_treDirectoryTree.GetParentItem(hItem); szFind = GetFileName(szTest); }

	CString szPath = GetDirectoryItemPathName(hItem);
	UINT uState = m_treDirectoryTree.GetItemState(hItem, TVIS_EXPANDED);

	if( uState & TVIS_EXPANDED ) {
		DeleteDirectoryTreeChildren(hItem);
		InsertDirectoryTreeChildren(hItem, szPath);
		m_treDirectoryTree.Expand(hItem, TVE_EXPAND);
	} else {
		DeleteDirectoryTreeChildren(hItem);
		if( DirectoryHasChildren(szPath) ) m_treDirectoryTree.InsertItem("NULL", hItem);
	}

	if( szFind.GetLength() ) {
		HTREEITEM hFind = FindDirectoryTreeChildItem(hItem, szFind);
		if( hFind ) m_treDirectoryTree.SelectItem(hFind);
		else m_treDirectoryTree.SelectItem(hItem);
	} else m_treDirectoryTree.SelectItem(hItem);

	return TRUE;
}

BOOL CFileWindow::AskDestinationDirectory(CString & szDirectory)
{
	CString szText( LPCTSTR(IDS_CHOOSE_DIR_DESTINATION) );
	CFolderDialog dlg(szText, szDirectory, NULL, AfxGetMainWnd());
	if( dlg.DoModal() != IDOK ) return FALSE;
	szDirectory = dlg.GetPathName();
	return TRUE;
}


HTREEITEM CFileWindow::GetPointedDirectoryTreeItem()
{
	CPoint point; GetCursorPos( & point ); 
	m_treDirectoryTree.ScreenToClient( & point );

	HTREEITEM hItem; UINT nFlags;
	hItem = m_treDirectoryTree.HitTest( point, & nFlags );

	if( nFlags & TVHT_ONITEM ) return hItem;
	return NULL;
}


HTREEITEM CFileWindow::DeleteDirectoryTreeChildren(HTREEITEM hParent)
{
	HTREEITEM hChild = m_treDirectoryTree.GetChildItem( hParent );
	while( hChild ) {
		m_treDirectoryTree.DeleteItem( hChild );
		hChild = m_treDirectoryTree.GetChildItem( hParent );
	}

	return hParent;
}

HTREEITEM CFileWindow::InsertDirectoryTreeChildren(HTREEITEM hParent, LPCTSTR lpszPath)
{
	CString szPath = lpszPath; INT nLength = szPath.GetLength();
	if( szPath[nLength-1] == '\\' ) szPath += "*.*";
	else szPath += "\\*.*";

	CSortStringArray arrDirectories, arrFiles;
	INT i, nSize;

	CString szFilter = GetActiveFileFilterString();
	CFileFind find; BOOL bFound = find.FindFile(szPath);

	while( bFound ) {
		bFound = find.FindNextFile();
		CString szFilePath = find.GetFilePath();

		if( ! find.IsDirectory() && ! find.IsHidden() && MatchFileFilter(szFilePath, szFilter) ) arrFiles.Add( szFilePath );
		else if( find.IsDirectory() && ! find.IsDots() && ! find.IsHidden() ) arrDirectories.Add( szFilePath );
	}

	arrDirectories.Sort(); nSize = arrDirectories.GetSize();
	for(i = 0; i < nSize; i++) {
		szPath = arrDirectories.GetAt(i);
		HTREEITEM hItem = InsertDirectoryTreeItem(hParent, szPath);
		if( DirectoryHasChildren(szPath) ) m_treDirectoryTree.InsertItem("NULL", hItem);
	}

	arrFiles.Sort(); nSize = arrFiles.GetSize();
	for(i = 0; i < nSize; i++) {
		szPath = arrFiles.GetAt(i);
		HTREEITEM hItem = InsertDirectoryTreeItem(hParent, szPath);
	}

	return hParent;
}


HTREEITEM CFileWindow::InsertDirectoryTreeRoot(LPCTSTR lpszPath)
{
	TCHAR szTemp[MAX_PATH]; strcpy(szTemp, lpszPath); INT nLen = strlen(lpszPath);
	if( szTemp[nLen-1] != '\\' ) { szTemp[nLen] = '\\'; nLen++; szTemp[nLen] = '\0'; }

	SHFILEINFO shFinfo; // INT iIcon, iIconSel;
	if( ! SHGetFileInfo(szTemp, 0, &shFinfo, sizeof(shFinfo), SHGFI_DISPLAYNAME | SHGFI_SYSICONINDEX) ) return NULL;

//	if( szTemp[nLen-1] == '\\' ) { szTemp[nLen-1] = '\0'; }
	return m_treDirectoryTree.InsertItem( shFinfo.szDisplayName, shFinfo.iIcon, shFinfo.iIcon, TVI_ROOT );
}

HTREEITEM CFileWindow::InsertDirectoryTreeItem(HTREEITEM hParent, LPCTSTR lpszPath)
{
	TCHAR szTemp[MAX_PATH]; strcpy(szTemp, lpszPath); INT nLen = strlen(lpszPath);
	if( szTemp[nLen-1] != '\\' ) { szTemp[nLen] = '\\'; nLen++; szTemp[nLen] = '\0'; }

	SHFILEINFO shFinfo; // INT iIcon, iIconSel;
	if( ! SHGetFileInfo(szTemp, 0, &shFinfo, sizeof(shFinfo), SHGFI_SYSICONINDEX) ) return NULL;

	if( szTemp[nLen-1] == '\\' ) { szTemp[nLen-1] = '\0'; }
	return m_treDirectoryTree.InsertItem( GetFileName(szTemp), shFinfo.iIcon, shFinfo.iIcon, hParent );
}

HTREEITEM CFileWindow::FindDirectoryTreeChildItem(HTREEITEM hParent, LPCTSTR lpszText)
{
	HTREEITEM hFound = m_treDirectoryTree.GetChildItem(hParent);
	while( hFound ) {
		CString szText = m_treDirectoryTree.GetItemText( hFound );
		if( ! szText.CompareNoCase(lpszText) ) return hFound;
		hFound = m_treDirectoryTree.GetNextSiblingItem( hFound );
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// Handlers
void CFileWindow::OnDropdownLocalDrive() 
{
	CString szDriveName = GetActiveLocalDriveName();
	InitLocalDriveList(szDriveName);
}

void CFileWindow::OnSelchangeLocalDrive() 
{
	CString szDriveName = GetActiveLocalDriveName();
	if( ! szDriveName.Compare(m_szPrevDriveName) ) return;

	CWaitCursor wait;
	m_treDirectoryTree.SetRedraw(FALSE);

	HTREEITEM hRoot = m_treDirectoryTree.GetRootItem();
	RemakeDirectoryTreeRoot(szDriveName);

	m_treDirectoryTree.SetRedraw(TRUE);
	m_szPrevDriveName = szDriveName;
}

void CFileWindow::OnDropdownFileFilter()
{
	INT nFilterIndex = GetActiveFileFilterIndex();
//	m_nPrevFilterIndex = nFilterIndex;
}

void CFileWindow::OnSelchangeFileFilter() 
{
	INT nFilterIndex = GetActiveFileFilterIndex();
	if( nFilterIndex == m_nPrevFilterIndex ) return;

	HTREEITEM hItem = GetSelectedDirectoryTreeItem();

	CString szDriveName = GetActiveLocalDriveName();
	CString szPath; if( hItem ) szPath = GetDirectoryItemPathName(hItem);

	CWaitCursor wait;
	m_treDirectoryTree.SetRedraw(FALSE);

	RemakeDirectoryTreeRoot(szDriveName);
	if( hItem ) ExpandDirectoryTreePath(szPath);

	m_treDirectoryTree.SetRedraw(TRUE);
	m_nPrevFilterIndex = nFilterIndex;

	if( m_fcnCallbackSelchangeFileFilter ) m_fcnCallbackSelchangeFileFilter( nFilterIndex );
}

void CFileWindow::OnItemexpandingDirectoryTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;

	UINT uAction = pNMTreeView->action;
	CString szPath = GetDirectoryItemPathName( hItem );

	if( uAction == TVE_EXPAND ) { // expanding
		CWaitCursor wait;
		m_treDirectoryTree.SetRedraw(FALSE);

		DeleteDirectoryTreeChildren(hItem);
		InsertDirectoryTreeChildren(hItem, szPath);

		m_treDirectoryTree.SetRedraw(TRUE);
	}

	HTREEITEM hRoot = m_treDirectoryTree.GetRootItem();

	// prevent root item to be collapsed
	if( uAction == TVE_COLLAPSE && hItem == hRoot ) * pResult = 1;
	else * pResult = 0;
}

void CFileWindow::OnSelchangedDirectoryTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HTREEITEM hItem = GetSelectedDirectoryTreeItem();
	if( ! hItem ) { * pResult = 0; return; }

	CString szPathName = GetDirectoryItemPathName(hItem);
	SetAsCurrentDirectory(szPathName);

	*pResult = 0;
}

void CFileWindow::OnClickDirectoryTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	HTREEITEM hItem = GetPointedDirectoryItem();
	*pResult = 0;
}

void CFileWindow::OnDblclkDirectoryTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HTREEITEM hItem = GetPointedDirectoryTreeItem();
	if( ! hItem ) { * pResult = 0; return; }

	CString szPathName = GetDirectoryItemPathName(hItem);
	OpenDirectoryItem(szPathName);

	*pResult = 0;
}

void CFileWindow::OnRclickDirectoryTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HTREEITEM hItem = GetPointedDirectoryTreeItem();
	if( hItem ) m_treDirectoryTree.SelectItem(hItem);

	CPoint point; GetCursorPos( & point ); 
	CMenu * pMenu, context; context.LoadMenu(IDR_FILE_WINDOW);

	if( hItem ) {
		if( ! IsSelectedDirectoryItemRoot() ) {
			CString szPathName = GetDirectoryItemPathName(hItem);
			if( VerifyFilePath(szPathName) ) {
				pMenu = GetSubMenuByText( & context, "DIR_FILE" );
			} else pMenu = GetSubMenuByText( & context, "DIR_FOLDER" );
		} else pMenu = GetSubMenuByText( & context, "DIR_ROOT" );
	} else pMenu = GetSubMenuByText( & context, "DIR_NULL" );

	UINT nFlags = TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON;
	pMenu->TrackPopupMenu(nFlags, point.x, point.y, AfxGetMainWnd());

	*pResult = 0;
}

void CFileWindow::OnBeginlabeleditDirectoryTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMTVDISPINFO* pNMTVDISPINFO = (NMTVDISPINFO*)pNMHDR;
	HTREEITEM hItem = pNMTVDISPINFO->item.hItem;

	HTREEITEM hParent = m_treDirectoryTree.GetParentItem(hItem);
	m_bLabelEditing = hParent ? TRUE : FALSE;

	* pResult = m_bLabelEditing ? 0 : 1;
}

void CFileWindow::OnEndlabeleditDirectoryTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMTVDISPINFO* pNMTVDISPINFO = (NMTVDISPINFO*)pNMHDR;
	HTREEITEM hItem = pNMTVDISPINFO->item.hItem;

	CString szOldName = m_treDirectoryTree.GetItemText(hItem);
	CString szNewName = pNMTVDISPINFO->item.pszText;
	if( szNewName.GetLength() && szOldName.CompareNoCase(szNewName) ) {
		CString szPathName = GetDirectoryItemPathName(hItem);
		if( RenameDirectoryItem(szPathName, szNewName) && ! VerifyPathName(szPathName) ) {
			m_treDirectoryTree.SetItemText(hItem, szNewName);
		}
	}

	m_bLabelEditing = FALSE;

	* pResult = 0;
}

void CFileWindow::OnBegindragDirectoryTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTREEVIEW* pNMTREEVIEW = (NMTREEVIEW*)pNMHDR;
	HTREEITEM hItem = pNMTREEVIEW->itemNew.hItem;

	// root item should not be allowed to drag
	if( hItem == m_treDirectoryTree.GetRootItem() ) { * pResult = 0; return; }

	CString szPathName = GetDirectoryItemPathName(hItem);
	UINT size = sizeof(DROPFILES) + szPathName.GetLength() + 2;

	HGLOBAL hMemory = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT, size);
	if( ! hMemory ) { * pResult = 0; return; }

	DROPFILES * pDrop = (DROPFILES *)::GlobalLock(hMemory);
	if( ! pDrop ) { ::GlobalFree(hMemory); * pResult = 0; return; }

	pDrop->pFiles = sizeof(DROPFILES);
	TCHAR * pName = (TCHAR *)(LPBYTE(pDrop) + sizeof(DROPFILES));
	lstrcpy( pName, szPathName );
	::GlobalUnlock(hMemory);

	FORMATETC etc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	m_oleDataSource.CacheGlobalData( CF_HDROP, hMemory, & etc );

	DROPEFFECT eff = m_oleDataSource.DoDragDrop(DROPEFFECT_COPY | DROPEFFECT_MOVE);

	if( eff == DROPEFFECT_MOVE ) {
		// item has been moved - delete tree item
		m_treDirectoryTree.DeleteItem( hItem );
		TRACE0("Item has been moved by DROPEFFECT_MOVE\n");
	} else if( eff == DROPEFFECT_NONE ) {
		// check if item has been moved
		if( ! VerifyPathName( szPathName ) ) {
			// item has been moved - delete tree item
			m_treDirectoryTree.DeleteItem( hItem );
			TRACE0("Item has been moved by DROPEFFECT_NONE\n");
		} else {
			// drag and drop has been canceled
			::GlobalFree(hMemory);
			TRACE0("Drag and Drop has been canceled\n");
		}
	}

	m_oleDataSource.Empty(); 
	* pResult = 0;
}

DROPEFFECT CFileWindow::OnDragOverDirectoryTree(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	static CPoint ptPrev = CPoint(-1, -1);
	static INT nStayCount = 0;

	if( m_nDragObjectType == DRAG_OBJECT_HDROP ) {
		ClientToScreen( & point ); m_treDirectoryTree.ScreenToClient( & point );
		CRect rectNonScroll; m_treDirectoryTree.GetClientRect( & rectNonScroll );
		
		if( rectNonScroll.Height() > 2 * DRAG_SCROLL_REGION ) rectNonScroll.DeflateRect(0, DRAG_SCROLL_REGION);
		if( rectNonScroll.Width()  > 2 * DRAG_SCROLL_REGION ) rectNonScroll.DeflateRect(DRAG_SCROLL_REGION, 0);

		if( ! rectNonScroll.PtInRect(point) ) {
			if( point.y > rectNonScroll.bottom    ) m_treDirectoryTree.SendMessage(WM_VSCROLL, SB_LINEDOWN, 0L);
			else if( point.y < rectNonScroll.top  ) m_treDirectoryTree.SendMessage(WM_VSCROLL, SB_LINEUP,   0L);

			if( point.x > rectNonScroll.right     ) m_treDirectoryTree.SendMessage(WM_HSCROLL, SB_LINEDOWN, 0L);
			else if( point.x < rectNonScroll.left ) m_treDirectoryTree.SendMessage(WM_HSCROLL, SB_LINEUP,   0L);
		}

		UINT nFlags; HTREEITEM hItem = m_treDirectoryTree.HitTest( point, & nFlags );

		if( hItem ) {
			CString szPath = GetDirectoryItemPathName(hItem);

			if( ! VerifyFilePath(szPath) ) {
				m_treDirectoryTree.SelectDropTarget(hItem);
				UINT uState = m_treDirectoryTree.GetItemState(hItem, TVIS_EXPANDED);

				if( m_treDirectoryTree.ItemHasChildren(hItem) && ! (uState & TVIS_EXPANDED) ) {
					if( ptPrev == point ) nStayCount++;
					else { ptPrev = point; nStayCount = 0; }

					if( nStayCount >= DRAG_EXPAND_COUNT ) {
						CWaitCursor wait; 
						m_treDirectoryTree.SetRedraw(FALSE);

						DeleteDirectoryTreeChildren(hItem);
						InsertDirectoryTreeChildren(hItem, szPath);
						m_treDirectoryTree.Expand(hItem, TVE_EXPAND);

						m_treDirectoryTree.SetRedraw(TRUE);
						ptPrev = point; nStayCount = 0;
					}
				} else ptPrev = CPoint(-1, -1);

				if( dwKeyState & MK_CONTROL ) return DROPEFFECT_COPY;
				else return DROPEFFECT_MOVE;

			} else {
				m_treDirectoryTree.SelectDropTarget(NULL);
				ptPrev = CPoint(-1, -1); return DROPEFFECT_NONE;
			}

		} else {
			m_treDirectoryTree.SelectDropTarget(NULL);
			ptPrev = CPoint(-1, -1); return DROPEFFECT_NONE;
		}

	} else {
		m_treDirectoryTree.SelectDropTarget(NULL);
		ptPrev = CPoint(-1, -1); return DROPEFFECT_NONE;
	}
}

BOOL CFileWindow::OnDropDirectoryTree(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	if( pDataObject->IsDataAvailable(CF_HDROP) ) {
		ClientToScreen( & point ); m_treDirectoryTree.ScreenToClient( & point );
		UINT nFlags; HTREEITEM hItem = m_treDirectoryTree.HitTest( point, & nFlags );

		if( hItem && (dropEffect == DROPEFFECT_COPY || dropEffect == DROPEFFECT_MOVE) ) {
			CString szPath = GetDirectoryItemPathName(hItem);

			if( ! VerifyFilePath(szPath) ) {
				HGLOBAL hMemory = pDataObject->GetGlobalData(CF_HDROP);
				if( ! hMemory ) return FALSE;

				HDROP hDrop = (HDROP)::GlobalLock(hMemory);
				if( ! hDrop ) { ::GlobalUnlock(hMemory); return FALSE; }

				TCHAR szNextFile[MAX_PATH];
				UINT uNumFiles = DragQueryFile( hDrop, (UINT)-1, NULL, 0 );
				for( UINT uFile = 0; uFile < uNumFiles; uFile++ ) {
					if( DragQueryFile( hDrop, uFile, szNextFile, MAX_PATH ) > 0 ) {
						if( dropEffect == DROPEFFECT_COPY ) CopyToDirectoryItem(szNextFile, szPath);
						else MoveToDirectoryItem(szNextFile, szPath);
					}
				}

				::GlobalUnlock(hMemory);
				::GlobalFree(hMemory);

				RefreshDirectoryItem( hItem );

				if( dropEffect == DROPEFFECT_COPY ) return TRUE;
				else return FALSE;

			} else return FALSE;

		} else return FALSE;

	} else return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Action Functions
BOOL CFileWindow::SetAsCurrentDirectory(LPCTSTR lpszPathName)
{
	CString szDirName, szPathName = lpszPathName;
	if( ! VerifyFilePath(szPathName) ) {
		szDirName = szPathName;
		return ::SetCurrentDirectory(szDirName + "\\");
	} else {
		szDirName = GetFileDirectory(szPathName);
		return ::SetCurrentDirectory(szDirName + "\\");
	}
}

BOOL CFileWindow::SetAsWorkingDirectory(LPCTSTR lpszPathName)
{
	CString szPathName = lpszPathName;
	if( ! VerifyFilePath(szPathName) ) {
		CCedtApp::m_szInitialWorkingDirectory = szPathName;
		return TRUE;
	} else return FALSE;
}

BOOL CFileWindow::OpenDirectoryItem(LPCTSTR lpszPathName)
{
	CString szPath = lpszPathName;
	if( ! VerifyFilePath( szPath ) ) return FALSE;

	CCedtApp * pApp = (CCedtApp *)AfxGetApp(); if( ! pApp ) return FALSE;
	return pApp->PostOpenDocumentFile( szPath, 0 );
}

BOOL CFileWindow::ExecuteDirectoryItem(LPCTSTR lpszPathName)
{
	CString szPath = lpszPathName;
	if( ! VerifyFilePath( szPath ) ) return FALSE;

	CWnd * pWnd = AfxGetMainWnd(); if( ! pWnd ) return FALSE;
	HINSTANCE hResult = ::ShellExecute(NULL, "open", szPath, NULL, NULL, SW_SHOWNORMAL);
	return ((UINT)hResult > 32) ? TRUE : FALSE;
}

BOOL CFileWindow::ExploreDirectoryItem(LPCTSTR lpszPathName)
{
	CString szPath = lpszPathName;
	if( VerifyFilePath(szPath) ) return FALSE;

	CWnd * pWnd = AfxGetMainWnd(); if( ! pWnd ) return FALSE;
	HINSTANCE hResult = ::ShellExecute(NULL, "open", szPath, NULL, NULL, SW_SHOWNORMAL);
	return ((UINT)hResult > 32) ? TRUE : FALSE;
}

BOOL CFileWindow::FindInDirectoryItem(LPCTSTR lpszPathName)
{
	CString szPath = lpszPathName;
	if( VerifyFilePath(szPath) ) return FALSE;

	CWnd * pWnd = AfxGetMainWnd(); if( ! pWnd ) return FALSE;
	HINSTANCE hResult = ::ShellExecute(NULL, "find", szPath, NULL, NULL, SW_SHOWNORMAL);
	return ((UINT)hResult > 32) ? TRUE : FALSE;
}

BOOL CFileWindow::ShowPropDirectoryItem(LPCTSTR lpszPathName)
{
	CString szPath = lpszPathName;
//	if( ! VerifyFilePath( szPath ) ) return FALSE;

	SHELLEXECUTEINFO sei; ZeroMemory( & sei, sizeof(sei) );
	sei.cbSize = sizeof(sei);
	sei.lpFile = szPath;
	sei.lpVerb = _T("properties");
	sei.fMask  = SEE_MASK_INVOKEIDLIST;
	sei.nShow  = SW_SHOWNORMAL;

	return ShellExecuteEx( & sei ); 
}

BOOL CFileWindow::MoveToDirectoryItem(LPCTSTR lpszPathName, LPCTSTR lpszDestination)
{
	TCHAR szFrom[MAX_PATH]; memset(szFrom, 0x00, sizeof(szFrom)); strcpy(szFrom, lpszPathName);
	TCHAR szDest[MAX_PATH]; memset(szDest, 0x00, sizeof(szDest)); strcpy(szDest, lpszDestination);
	CWnd * pWnd = AfxGetMainWnd();

	SHFILEOPSTRUCT fo; memset( & fo, 0x00, sizeof(SHFILEOPSTRUCT) );
	fo.hwnd = pWnd->m_hWnd;		fo.wFunc = FO_MOVE;
	fo.pFrom = szFrom;			fo.pTo = szDest;
	fo.fFlags = FOF_ALLOWUNDO;

	if( ! SHFileOperation( & fo ) && ! fo.fAnyOperationsAborted ) return TRUE;
	else return FALSE;
}

BOOL CFileWindow::CopyToDirectoryItem(LPCTSTR lpszPathName, LPCTSTR lpszDestination)
{
	TCHAR szFrom[MAX_PATH]; memset(szFrom, 0x00, sizeof(szFrom)); strcpy(szFrom, lpszPathName);
	TCHAR szDest[MAX_PATH]; memset(szDest, 0x00, sizeof(szDest)); strcpy(szDest, lpszDestination);
	CWnd * pWnd = AfxGetMainWnd();

	SHFILEOPSTRUCT fo; memset( & fo, 0x00, sizeof(SHFILEOPSTRUCT) );
	fo.hwnd = pWnd->m_hWnd;		fo.wFunc = FO_COPY;
	fo.pFrom = szFrom;			fo.pTo = szDest;
	fo.fFlags = FOF_ALLOWUNDO;

	if( ! SHFileOperation( & fo ) && ! fo.fAnyOperationsAborted ) return TRUE;
	else return FALSE;
}

BOOL CFileWindow::DeleteDirectoryItem(LPCTSTR lpszPathName)
{
	TCHAR szFrom[MAX_PATH]; memset(szFrom, 0x00, sizeof(szFrom)); strcpy(szFrom, lpszPathName);
	CWnd * pWnd = AfxGetMainWnd();

	SHFILEOPSTRUCT fo; memset( & fo, 0x00, sizeof(SHFILEOPSTRUCT) );
	fo.hwnd = pWnd->m_hWnd;		fo.wFunc = FO_DELETE;
	fo.pFrom = szFrom;			fo.fFlags = FOF_ALLOWUNDO;

	if( ! SHFileOperation( & fo ) && ! fo.fAnyOperationsAborted ) return TRUE;
	else return FALSE;
}

BOOL CFileWindow::RenameDirectoryItem(LPCTSTR lpszPathName, LPCTSTR lpszNewName)
{
	TCHAR szFrom[MAX_PATH]; memset(szFrom, 0x00, sizeof(szFrom)); strcpy(szFrom, lpszPathName);
	TCHAR szDest[MAX_PATH]; memset(szDest, 0x00, sizeof(szDest)); strcpy(szDest, GetFileDirectory(szFrom) + "\\" + lpszNewName);
	CWnd * pWnd = AfxGetMainWnd();

	SHFILEOPSTRUCT fo; memset( & fo, 0x00, sizeof(SHFILEOPSTRUCT) );
	fo.hwnd = pWnd->m_hWnd;		fo.wFunc = FO_RENAME;
	fo.pFrom = szFrom;			fo.pTo = szDest;
	fo.fFlags = FOF_ALLOWUNDO;

	if( ! SHFileOperation( & fo ) && ! fo.fAnyOperationsAborted ) return TRUE;
	else return FALSE;
}

BOOL CFileWindow::CreateNewFolderInDirectory(LPCTSTR lpszPathName, CString & szFolderName)
{
	CString szTemp; INT nIncrement = 2;

	szFolderName.LoadString(IDS_NAME_NEW_FOLDER);
	szTemp.Format("%s\\%s", lpszPathName, szFolderName);

	while( VerifyPathName(szTemp) ) {
		szFolderName.Format(IDS_NAME_NEW_FOLDER2, nIncrement++);
		szTemp.Format("%s\\%s", lpszPathName, szFolderName);
	}

	return CreateDirectory( szTemp, NULL );
}

BOOL CFileWindow::CreateNewDocumentInDirectory(LPCTSTR lpszPathName, CString & szFileName)
{
	CString szTemp; INT nIncrement = 2;

	szFileName.LoadString(IDS_NAME_NEW_DOCUMENT);
	szTemp.Format("%s\\%s", lpszPathName, szFileName);

	while( VerifyPathName(szTemp) ) {
		szFileName.Format(IDS_NAME_NEW_DOCUMENT2, nIncrement++);
		szTemp.Format("%s\\%s", lpszPathName, szFileName);
	}

	HANDLE hFile = CreateFile( szTemp, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile == INVALID_HANDLE_VALUE ) return FALSE;
	CloseHandle( hFile ); return TRUE;
}

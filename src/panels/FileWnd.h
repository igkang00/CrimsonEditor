#if !defined(AFX_FILEWINDOW_H__EF837327_15B9_11D5_A6F1_0050CE184C9B__INCLUDED_)
#define AFX_FILEWINDOW_H__EF837327_15B9_11D5_A6F1_0050CE184C9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define FILE_WINDOW_DIRECTORY		0x00
#define FILE_WINDOW_REMOTE			0x02
#define FILE_WINDOW_PROJECT			0x01

#define PROJECT_ITEM_PROJECT		0x00
#define PROJECT_ITEM_CATEGORY		0x01
#define PROJECT_ITEM_LOCAL_FILE		0x02
#define PROJECT_ITEM_REMOTE_FILE	0x03

#define DRAG_EXPAND_COUNT			20
#define DRAG_SCROLL_REGION			20

typedef void (* CALLBACK_SELCHANGE)(INT);

typedef struct _PROJECTITEMINFO {
	HTREEITEM hItem;
	TCHAR szText[MAX_PATH];
	INT nItemType;
	INT nFtpAccount;
	TCHAR szPathName[MAX_PATH];
} PROJECTITEMINFO, * LPPROJECTITEMINFO;


class CFileWindow : public CSizingControlBar
{
public: // Construction
	CFileWindow();
	virtual ~CFileWindow();

protected: // Resources & Controls
	TBBUTTON m_tbiToolbarDirectory[10];
	TBBUTTON m_tbiToolbarProject[10];
	TBBUTTON m_tbiWinButtons[10];

	CImageList m_imgToolbarDirectory;
	CImageList m_imgToolbarProject;
	CImageList m_imgWinButtons;

	CToolBarCtrl m_btnToolbarDirectory;
	CToolBarCtrl m_btnToolbarProject;
	CToolBarCtrl m_btnWinButtons;

	CImageList m_imgCategoryTab;
	CImageList m_imgDirectoryTree;
	CImageList m_imgProjectTree;

	CFont m_fontControl;
	CXPTabCtrl m_tabCategory;

	HACCEL m_hAccel;
	BOOL m_bLabelEditing;

protected: // directory window variables
	CStringArray m_arrLocalDrive;

	CString m_szPrevDriveName;
	INT m_nPrevFilterIndex;

	CStringArray m_arrFilterDescription, m_arrFilterExtensions;
	CALLBACK_SELCHANGE m_fcnCallbackSelchangeFileFilter;

protected: // drag and drop
	CFileWindowDropTarget m_oleDropTarget;
	COleDataSource m_oleDataSource;

	INT m_nDragCategory, m_nDragObjectType;
	CRect m_rectDragCategory, m_rectDragBound;

protected: // directory window controls
	CComboBox m_cmbLocalDrive;
	CTreeCtrl m_treDirectoryTree;
	CComboBox m_cmbFileFilter;

	CComboBox m_cmbFtpAccount;
	CTreeCtrl m_treRemoteTree;

protected: // project window controls
	CTreeCtrl m_treProjectTree;


public: // drag and drop
	DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);

	void OnDragLeave();
	BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);

public:
	INT  GetActiveCategory();
	void SetActiveCategory(INT nSelect);

protected:
	void ShowOrHideCommonControls(INT nSelect);
	void ShowOrHideDirectoryControls(INT nSelect);
	void ShowOrHideRemoteControls(INT nSelect);
	void ShowOrHideProjectControls(INT nSelect);

protected: // initialize directory image list
	BOOL InitDirectoryImageList( CImageList & imglst );


public: // *** FileWndDirectory.cpp ***
	BOOL InitLocalDriveList(LPCTSTR lpszInitialDriveName);
	BOOL InitFileFilterList(LPCTSTR lpszComposedFilter, INT nInitialFilter, CALLBACK_SELCHANGE fcnCallback = NULL);

	BOOL GetBrowsingDirectory(CString & szDirectory);
	BOOL SetBrowsingDirectory(LPCTSTR lpszDirectory);

	BOOL IsSelectedDirectoryItemRoot();
	CString GetSelectedDirectoryItemText();

	BOOL OpenSelectedDirectoryItem();
	BOOL ExecuteSelectedDirectoryItem();
	BOOL ExploreSelectedDirectoryItem();
	BOOL FindInSelectedDirectoryItem();
	BOOL ShowPropSelectedDirectoryItem();

	BOOL MoveSelectedDirectoryItem();
	BOOL CopySelectedDirectoryItem();
	BOOL DeleteSelectedDirectoryItem();
	BOOL RenameSelectedDirectoryItem();

	BOOL CreateNewFolderInSelectedDirectoryItem();
	BOOL CreateNewDocumentInSelectedDirectoryItem();
	BOOL SetAsWorkingDirSelectedDirectoryItem();
	BOOL RefreshSelectedDirectoryItem();

protected: // inner functions
	BOOL SelectLocalDriveList(LPCTSTR lpszDriveName);
	CString GetActiveLocalDriveName();

	INT GetActiveFileFilterIndex();
	CString GetActiveFileFilterString();

	BOOL DirectoryHasChildren(LPCTSTR lpszPath);
	CString GetDirectoryItemPathName(HTREEITEM hItem);

	BOOL RemakeDirectoryTreeRoot(LPCTSTR lpszDriveName);
	BOOL ExpandDirectoryTreePath(LPCTSTR lpszPath);

	BOOL RefreshDirectoryItem(HTREEITEM hItem);
	BOOL AskDestinationDirectory(CString & szDirectory);

protected: // tree control handling
	HTREEITEM GetSelectedDirectoryTreeItem() { return m_treDirectoryTree.GetSelectedItem(); }
	HTREEITEM GetPointedDirectoryTreeItem();

	HTREEITEM DeleteDirectoryTreeChildren(HTREEITEM hParent);
	HTREEITEM InsertDirectoryTreeChildren(HTREEITEM hParent, LPCTSTR lpszPath);

	HTREEITEM InsertDirectoryTreeRoot(LPCTSTR lpszPath);
	HTREEITEM InsertDirectoryTreeItem(HTREEITEM hParent, LPCTSTR lpszPath);
	HTREEITEM FindDirectoryTreeChildItem(HTREEITEM hParent, LPCTSTR lpszText);

protected: // Handlers
	DROPEFFECT OnDragOverDirectoryTree(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	BOOL OnDropDirectoryTree(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);

protected: // action functions
	BOOL SetAsCurrentDirectory(LPCTSTR lpszPathName);
	BOOL SetAsWorkingDirectory(LPCTSTR lpszPathName);

	BOOL OpenDirectoryItem(LPCTSTR lpszPathName);
	BOOL ExecuteDirectoryItem(LPCTSTR lpszPathName);
	BOOL ExploreDirectoryItem(LPCTSTR lpszPathName);
	BOOL FindInDirectoryItem(LPCTSTR lpszPathName);
	BOOL ShowPropDirectoryItem(LPCTSTR lpszPathName);

	BOOL MoveToDirectoryItem(LPCTSTR lpszPathName, LPCTSTR lpszDestination);
	BOOL CopyToDirectoryItem(LPCTSTR lpszPathName, LPCTSTR lpszDestination);
	BOOL DeleteDirectoryItem(LPCTSTR lpszPathName);
	BOOL RenameDirectoryItem(LPCTSTR lpszPathName, LPCTSTR lpszNewName);

	BOOL CreateNewFolderInDirectory(LPCTSTR lpszPathName, CString & szFolderName);
	BOOL CreateNewDocumentInDirectory(LPCTSTR lpszPathName, CString & szFileName);


public: // *** FileWndRemote.cpp ***


public: // *** FileWndProject.cpp ***
	BOOL InitProjectWorkspace();

	BOOL NewProjectWorkspace(LPCTSTR lpszPathName);
	BOOL SaveProjectWorkspace(LPCTSTR lpszPathName);
	BOOL OpenProjectWorkspace(LPCTSTR lpszPathName);

	BOOL SaveRegularWorkspace(LPCTSTR lpszPathName);
	BOOL OpenRegularWorkspace(LPCTSTR lpszPathName);

	BOOL AddCategoryToProject(LPCTSTR lpszCategory);
	BOOL AddLocalFileToProject(LPCTSTR lpszPathName);
	BOOL AddRemoteFileToProject(INT nAccount, LPCTSTR lpszPathName, DWORD dwSize);

	BOOL AddLocalDirectoryToProject(LPCTSTR lpszPathName);
	BOOL AddProjectCategoryToProject(LPPROJECTITEMINFO lpInfo);

	BOOL IsSelectedProjectItemRoot();
	CString GetSelectedProjectItemText();

	BOOL OpenSelectedProjectItem();
	BOOL ExecuteSelectedProjectItem();
	BOOL ShowPropSelectedProjectItem();

	BOOL RemoveSelectedProjectItem();
	BOOL RenameSelectedProjectItem();

protected: // inner functions
	BOOL SaveProjectItem(ostream & os, INT nLevel, HTREEITEM hItem);
	BOOL LoadProjectItem(istream & is, TCHAR szText[], HTREEITEM hParent);

	BOOL SaveWorkspaceItem(ostream & os, INT nLevel, CMDIChildWnd * pChild);
	BOOL LoadWorkspaceItem(istream & is, TCHAR szText[], CWinApp * pApp);

	BOOL ParseItemAttribute(LPCTSTR lpszText, CMapStringToString & mapAttr);

	BOOL AddCategoryToProject(HTREEITEM hParent, LPCTSTR lpszCategory, BOOL bExpand = TRUE);
	BOOL AddLocalFileToProject(HTREEITEM hParent, LPCTSTR lpszPathName, BOOL bExpand = TRUE);
	BOOL AddRemoteFileToProject(HTREEITEM hParent, INT nAccount, LPCTSTR lpszPathName, BOOL bExpand = TRUE);

	BOOL AddLocalDirectoryToProject(HTREEITEM hParent, LPCTSTR lpszPathName, BOOL bExpand = TRUE);
	BOOL AddProjectCategoryToProject(HTREEITEM hParent, LPPROJECTITEMINFO lpInfo, BOOL bExpand = TRUE);

	LPPROJECTITEMINFO GetProjectItemInfo(HTREEITEM hItem);
	CString GetProjectItemPathName(HTREEITEM hItem);

	BOOL EnableAllProjectButtons(BOOL bEnable);
	static INT CALLBACK CompareProjectItem(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

protected: // tree control handling
	HTREEITEM GetSelectedProjectTreeItem() { return m_treProjectTree.GetSelectedItem(); }
	HTREEITEM GetPointedProjectTreeItem();

	HTREEITEM InsertProjectTreeItem(HTREEITEM hParent, LPCTSTR lpszText, INT nType, INT nAccount, LPCTSTR lpszPathName);
	HTREEITEM FindProjectTreeChildItem(HTREEITEM hParent, LPCTSTR lpszText);
	
	BOOL RemoveAllProjectItems();
	BOOL RemoveProjectItem(HTREEITEM hItem);

	HTREEITEM GetParentProjectItem(HTREEITEM hItem) { return m_treProjectTree.GetParentItem(hItem); }
	BOOL IsParentProjectItem(HTREEITEM hTest, HTREEITEM hItem);

protected: // Handlers
	DROPEFFECT OnDragOverProjectTree(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	BOOL OnDropProjectTree(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);

protected: // action functions
	BOOL OpenProjectItem(LPPROJECTITEMINFO lpInfo);
	BOOL ExecuteProjectItem(LPPROJECTITEMINFO lpInfo);
	BOOL ShowPropProjectItem(LPPROJECTITEMINFO lpInfo);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileWindow)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Attributes
protected:

	// Generated message map functions
protected:
	//{{AFX_MSG(CFileWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangeCategoryTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDropdownLocalDrive();
	afx_msg void OnSelchangeLocalDrive();
	afx_msg void OnDropdownFileFilter();
	afx_msg void OnSelchangeFileFilter();
	afx_msg void OnItemexpandingDirectoryTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedDirectoryTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickDirectoryTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkDirectoryTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickDirectoryTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeleditDirectoryTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditDirectoryTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBegindragDirectoryTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpandingProjectTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickProjectTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkProjectTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickProjectTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeleditProjectTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditProjectTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBegindragProjectTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFileWindowOpen();
	afx_msg void OnFileWindowDelete();
	afx_msg void OnFileWindowRename();
	afx_msg void OnFileWindowRefresh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEWINDOW_H__EF837327_15B9_11D5_A6F1_0050CE184C9B__INCLUDED_)

// OpenRemoteDialog.cpp : implementation file
//

#include "stdafx.h"
#include "cedtHeader.h"
#include "RemoteFile.h"
#include "FtpSettingsDialog.h"
#include "FtpPasswordDialog.h"
#include "OpenRemoteDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COpenRemoteDialog dialog


COpenRemoteDialog::COpenRemoteDialog(BOOL bOpenFileDialog, LPCTSTR lpszFileName, LPCTSTR lpszFilter, CWnd* pParent /*=NULL*/)
	: CDialog(COpenRemoteDialog::IDD, pParent)
{
	m_bOpenFileDialog = bOpenFileDialog;
	if( lpszFileName ) m_szFileName = lpszFileName;
	if( lpszFilter ) m_szFileFilter = lpszFilter;

	m_nAccountCount = 0;
	m_pFtpAccounts = NULL;

	m_nCurrentFilterIndex = 0;
	m_nCurrentFtpAccount = 0;

	//{{AFX_DATA_INIT(COpenRemoteDialog)
	//}}AFX_DATA_INIT
}

COpenRemoteDialog::~COpenRemoteDialog()
{
	delete [] m_pFtpAccounts;
	m_pFtpAccounts = NULL;
}

void COpenRemoteDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpenRemoteDialog)
	DDX_Control(pDX, IDC_FTP_SETTINGS, m_btnFtpSettings);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_FILE_TYPE, m_cmbFileType);
	DDX_Control(pDX, IDC_FILE_NAME, m_edtFileName);
	DDX_Control(pDX, IDC_PARENT_DIRECTORY, m_btnParentDirectory);
	DDX_Control(pDX, IDC_CREATE_DIRECTORY, m_btnCreateDirectory);
	DDX_Control(pDX, IDC_DIRECTORY, m_edtDirectory);
	DDX_Control(pDX, IDC_FILE_LIST, m_lstFiles);
	DDX_Control(pDX, IDC_FTP_ACCOUNT, m_cmbFtpAccount);
	//}}AFX_DATA_MAP
}

void COpenRemoteDialog::SetFtpAccounts(INT nCount, CFtpAccount * pFtpAccounts)
{
	m_nAccountCount = nCount;

	delete [] m_pFtpAccounts;
	m_pFtpAccounts = new CFtpAccount[nCount];

	for( INT i = 0; i < nCount; i++ ) {
		m_pFtpAccounts[i].CopyContents( pFtpAccounts[i] );
	}
}

void COpenRemoteDialog::GetFtpAccounts(INT nCount, CFtpAccount * pFtpAccounts)
{
	if( nCount > m_nAccountCount ) nCount = m_nAccountCount;

	for( INT i = 0; i < nCount; i++ ) {
		pFtpAccounts[i].CopyContents( m_pFtpAccounts[i] );
	}
}

CString COpenRemoteDialog::GetPathName()
{
	CString & szFileInfo = m_lstSelectedFileInfo.GetHead();
	INT nFound = szFileInfo.Find('\n'); ASSERT(nFound >= 0);
	return szFileInfo.Mid(0, nFound);
}

DWORD COpenRemoteDialog::GetFileSize()
{
	CString & szFileInfo = m_lstSelectedFileInfo.GetHead();
	INT nFound = szFileInfo.Find('\n'); ASSERT(nFound >= 0);
	return atoi( szFileInfo.Mid(nFound+1) );
}

void COpenRemoteDialog::InitFtpAccounts()
{
	INT nAccount = m_cmbFtpAccount.GetCurSel();
	if( nAccount == CB_ERR ) nAccount = 0;

	m_cmbFtpAccount.ResetContent();
	for( INT i = 0; i < m_nAccountCount; i++ ) m_cmbFtpAccount.InsertString( i, m_pFtpAccounts[i].GetDisplayName() );

	m_cmbFtpAccount.SetCurSel( nAccount );
}

void COpenRemoteDialog::InitFileFilters()
{
	INT nFilter = m_cmbFileType.GetCurSel();
	if( nFilter == CB_ERR ) nFilter = 0;

	m_cmbFileType.ResetContent();
	ParseFileFilter(m_arrFilterDescription, m_arrFilterExtensions, m_szFileFilter);

	INT nSize = m_arrFilterDescription.GetSize();
	for( INT i = 0; i < nSize; i++ ) m_cmbFileType.InsertString( i, m_arrFilterDescription.GetAt(i) );

	m_cmbFileType.SetCurSel( nFilter );
}

void COpenRemoteDialog::ChangeDirectory(LPCTSTR lpszDirectory)
{
	INT nAccount = m_cmbFtpAccount.GetCurSel();
	CString szCurrentDirectory = m_pFtpAccounts[nAccount].m_szSubDirectory;

/*	if( strcmp(lpszDirectory, "..") == 0 ) { 
		INT nFound = szCurrentDirectory.ReverseFind('/');
		szCurrentDirectory = szCurrentDirectory.Mid(0, nFound);
		if( szCurrentDirectory.GetLength() == 0 ) szCurrentDirectory = "/";
	} else {
		if( lpszDirectory[0] == '/' ) szCurrentDirectory = lpszDirectory;
		else if( szCurrentDirectory.Compare("/") == 0 ) szCurrentDirectory += lpszDirectory;
		else szCurrentDirectory += CString("/") + lpszDirectory;
	} */

	if( lpszDirectory[0] == '/' ) szCurrentDirectory = lpszDirectory;
	else if( szCurrentDirectory.Compare("/") == 0 ) szCurrentDirectory += lpszDirectory;
	else szCurrentDirectory += CString("/") + lpszDirectory;

	m_pFtpAccounts[nAccount].m_szSubDirectory = szCurrentDirectory;
}

BOOL COpenRemoteDialog::RefreshFileList()
{
	// get current ftp account
	INT nAccount = m_cmbFtpAccount.GetCurSel();
	if( nAccount == CB_ERR ) nAccount = 0;

	CFtpAccount & rFtpAccount = m_pFtpAccounts[nAccount];
	if( ! rFtpAccount.IsValid() ) return FALSE;

	// if account password is not given, ask user
	if( ! rFtpAccount.m_bSavePassword && ! rFtpAccount.m_bPasswordVerified ) {
		CFtpPasswordDialog dlg; dlg.m_szAccountInfo = rFtpAccount.GetShortAccountName();
		if( dlg.DoModal() != IDOK ) return FALSE;
		rFtpAccount.m_szPassword = dlg.m_szPassword;
	}

	// get current file filter
	INT nFilter = m_cmbFileType.GetCurSel();
	if( nFilter == CB_ERR ) nFilter = 0;

	CString szFilter = m_arrFilterExtensions.GetAt(nFilter);
	if( ! szFilter.GetLength() ) szFilter = "*.*";

	// get remote file list
	CSortStringArray arrFileInfo;
	if( ! GetRemoteFileList(arrFileInfo, rFtpAccount, szFilter) ) return FALSE;

	// operation successful, password verified
	if( ! rFtpAccount.m_bSavePassword ) rFtpAccount.m_bPasswordVerified = TRUE;

	// set directory window text
	CString szCurrentDirectory = rFtpAccount.m_szSubDirectory;
	m_edtDirectory.SetWindowText( szCurrentDirectory );

	// refresh file list
	INT i, nSize, nItemCount = 0;
	m_lstFiles.DeleteAllItems();

	if( szCurrentDirectory.Compare("/") ) { // not a root directory
		m_lstFiles.InsertItem(nItemCount, "..", REMOTE_ITEM_PARENT);
		m_lstFiles.SetItemData(nItemCount, 0); nItemCount++;
		m_btnParentDirectory.EnableWindow( TRUE );
	} else { // root directory
		m_btnParentDirectory.EnableWindow( FALSE );
	}

	arrFileInfo.Sort(TRUE); nSize = arrFileInfo.GetSize();
	for( i = 0; i < nSize; i++ ) {
		CString szFileInfo = arrFileInfo[i];
		INT nFound = szFileInfo.Find('/', 2);

		CString szName = szFileInfo.Mid(2, nFound - 2);
		CString szInfo = szFileInfo.Mid(nFound + 1);

		INT nImage = REMOTE_ITEM_FILE; DWORD dwSize = 0;
		if( szFileInfo[0] == 'D' ) {
			if( szInfo[0] == 'N' ) nImage = REMOTE_ITEM_DIRECTORY;
			else nImage = REMOTE_ITEM_LINK;
		} else dwSize = atoi( szInfo );

		m_lstFiles.InsertItem(nItemCount, szName, nImage);
		m_lstFiles.SetItemData(nItemCount, dwSize); nItemCount++;
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(COpenRemoteDialog, CDialog)
	//{{AFX_MSG_MAP(COpenRemoteDialog)
	ON_BN_CLICKED(IDC_FTP_SETTINGS, OnFtpSettings)
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_NOTIFY(NM_DBLCLK, IDC_FILE_LIST, OnDblclkFileList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FILE_LIST, OnItemchangedFileList)
	ON_CBN_SELCHANGE(IDC_FILE_TYPE, OnSelchangeFileType)
	ON_BN_CLICKED(IDC_CREATE_DIRECTORY, OnCreateDirectory)
	ON_BN_CLICKED(IDC_PARENT_DIRECTORY, OnParentDirectory)
	ON_CBN_SELCHANGE(IDC_FTP_ACCOUNT, OnSelchangeFtpAccount)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenRemoteDialog message handlers

BOOL COpenRemoteDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if( m_bOpenFileDialog ) { // Open Remote
		CString szText; m_lstFiles.ModifyStyle( LVS_SINGLESEL, 0, 0 );
		szText.LoadString( IDS_CTRL_OPEN_REMOTE_OK ); m_btnOK.SetWindowText(szText);
		szText.LoadString( IDS_DLG_OPEN_REMOTE ); SetWindowText(szText);
	} else { // Save As Remote
		CString szText; m_lstFiles.ModifyStyle( 0, LVS_SINGLESEL, 0 );
		szText.LoadString( IDS_CTRL_SAVE_REMOTE_OK ); m_btnOK.SetWindowText(szText);
		szText.LoadString( IDS_DLG_SAVE_REMOTE ); SetWindowText(szText);
	}

	m_edtFileName.SetWindowText( m_szFileName );

	m_imgButtons.Create(IDB_REMOTE_OPEN, 16, 0, RGB(255, 0, 255));
	m_btnParentDirectory.SetIcon(m_imgButtons.ExtractIcon(0));
	m_btnCreateDirectory.SetIcon(m_imgButtons.ExtractIcon(1));
	m_btnFtpSettings.SetIcon(m_imgButtons.ExtractIcon(2));

	m_imgFileList.Create(IDB_REMOTE_ITEM, 16, 0, RGB(255, 0, 255));
	m_lstFiles.SetImageList( & m_imgFileList, LVSIL_SMALL );
	
	// disable open or save button
	m_btnOK.EnableWindow(FALSE);

	// initialize ftp account combo box
	InitFtpAccounts();
	m_cmbFtpAccount.SetCurSel( m_nCurrentFtpAccount );
	OnSelchangeFtpAccount();

	// initialize file type combo box
	InitFileFilters();
	m_cmbFileType.SetCurSel( m_nCurrentFilterIndex );

	return TRUE;
}

void COpenRemoteDialog::OnSelchangeFtpAccount() 
{
	INT nAccount = m_cmbFtpAccount.GetCurSel();
	if( nAccount == CB_ERR ) nAccount = 0;
	m_nCurrentFtpAccount = nAccount;

	// set directory window text
	CFtpAccount & rFtpAccount = m_pFtpAccounts[nAccount];
	CString szCurrentDirectory = rFtpAccount.m_szSubDirectory;
	m_edtDirectory.SetWindowText( szCurrentDirectory );
}

void COpenRemoteDialog::OnSelchangeFileType() 
{
	INT nFilter = m_cmbFileType.GetCurSel();
	if( nFilter == CB_ERR ) nFilter = 0;
	m_nCurrentFilterIndex = nFilter;

	m_btnOK.EnableWindow( RefreshFileList() );
}

void COpenRemoteDialog::OnCreateDirectory() 
{
	AfxMessageBox("Sorry, not available now");
}

void COpenRemoteDialog::OnParentDirectory() 
{
	ChangeDirectory( ".." );
	m_btnOK.EnableWindow( RefreshFileList() );
}

void COpenRemoteDialog::OnFtpSettings() 
{
	CFtpSettingsDialog dlg;
	dlg.SetFtpAccounts( m_nAccountCount, m_pFtpAccounts );

	if( dlg.DoModal() == IDOK ) {
		dlg.GetFtpAccounts( m_nAccountCount, m_pFtpAccounts );
		InitFtpAccounts();
	}
}

void COpenRemoteDialog::OnConnect() 
{
	m_btnOK.EnableWindow( RefreshFileList() );
}

void COpenRemoteDialog::OnItemchangedFileList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	INT nCount = 0; CString szFileList; 
	LVITEM item; TCHAR szText[MAX_PATH];

	POSITION pos = m_lstFiles.GetFirstSelectedItemPosition();
	while( pos ) {
		memset( & item, 0x00, sizeof(LVITEM) );
		item.iItem = m_lstFiles.GetNextSelectedItem( pos );
		item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM; 
		item.pszText = szText; item.cchTextMax = MAX_PATH;
		m_lstFiles.GetItem( & item );

		if( item.iImage == REMOTE_ITEM_FILE ) { // append only file items to the file name edit control
			CString szQuoted; szQuoted.Format("\"%s\"", szText);
			szFileList = szQuoted + " " + szFileList; nCount++;
		}
	}

	if( nCount > 1 ) m_edtFileName.SetWindowText( szFileList );
	else if( nCount == 1 ) m_edtFileName.SetWindowText( szText );

	*pResult = 0;
}

void COpenRemoteDialog::OnDblclkFileList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	LVITEM item; TCHAR szText[MAX_PATH];

	if( pNMListView->iItem >= 0 ) {
		memset( & item, 0x00, sizeof(LVITEM) );
		item.iItem = pNMListView->iItem;
		item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		item.pszText = szText; item.cchTextMax = MAX_PATH;
		m_lstFiles.GetItem( & item );

		if( item.iImage == REMOTE_ITEM_PARENT ) {
			// if it is a parent directory item then change directory
			ChangeDirectory( ".." );
			m_btnOK.EnableWindow( RefreshFileList() );
		} else if( item.iImage == REMOTE_ITEM_LINK || item.iImage == REMOTE_ITEM_DIRECTORY ) {
			// if it is a directory item then change directory
			ChangeDirectory( szText );
			m_btnOK.EnableWindow( RefreshFileList() );
		} else if( item.iImage == REMOTE_ITEM_FILE ) {
			// if it is a file item then close dialog box and open this file
			OnOK(); 
		}
	}

	*pResult = 0;
}

void COpenRemoteDialog::OnOK() 
{
	CString szFileName; m_edtFileName.GetWindowText( szFileName );
	if( ! szFileName.GetLength() ) return;

	CString szDirectory; m_edtDirectory.GetWindowText( szDirectory );
	INT nLen = szDirectory.GetLength();
	if( nLen == 0 || szDirectory.GetAt(nLen-1) != '/' ) szDirectory += "/";

	LVITEM item; LVFINDINFO info; TCHAR szText[MAX_PATH];
	m_lstSelectedFileInfo.RemoveAll(); CString szFileInfo;

	POSITION pos = m_lstFiles.GetFirstSelectedItemPosition();
	while( pos ) {
		memset( & item, 0x00, sizeof(LVITEM) );
		item.iItem = m_lstFiles.GetNextSelectedItem( pos );
		item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM; 
		item.pszText = szText; item.cchTextMax = MAX_PATH;
		m_lstFiles.GetItem( & item );

		if( item.iImage == REMOTE_ITEM_FILE ) { // append only file items
			szFileInfo.Format("%s\n%d", szDirectory + szText, item.lParam);
			m_lstSelectedFileInfo.AddTail( szFileInfo );
		}
	}

	if( m_lstSelectedFileInfo.GetCount() == 0 ) m_lstSelectedFileInfo.AddTail("");

	if( m_lstSelectedFileInfo.GetCount() == 1 ) {
		// check if the file name specified in the edit control exist in the list control
		memset( & info, 0x00, sizeof(LVFINDINFO) );
		info.flags = LVFI_STRING; info.psz = szFileName;
		INT nItemFound = m_lstFiles.FindItem( & info );

		if( nItemFound >= 0 ) { // if the file name specified is found in the list
			memset( & item, 0x00, sizeof(LVITEM) );
			item.iItem = nItemFound;
			item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM; 
			item.pszText = szText; item.cchTextMax = MAX_PATH;
			m_lstFiles.GetItem( & item );
		}

		if( m_bOpenFileDialog ) { // if it is open dialog then check if the file name specified exists
			if( nItemFound < 0 ) { // if the file name specified is not found in the list
				CString szMessage; szMessage.Format("%s\nThere is no such file!", szFileName);
				AfxMessageBox(szMessage); return; 
			} else if( item.iImage == REMOTE_ITEM_PARENT || item.iImage == REMOTE_ITEM_LINK 
				|| item.iImage == REMOTE_ITEM_DIRECTORY ) { // if the file name specified is a directory
				ChangeDirectory( szFileName );
				m_btnOK.EnableWindow( RefreshFileList() ); return;
			} else {
				szFileInfo.Format("%s\n%d", szDirectory + szFileName, item.lParam);
				POSITION pos = m_lstSelectedFileInfo.GetHeadPosition();
				m_lstSelectedFileInfo.SetAt(pos, szFileInfo);
			}
		} else { // if it is save dialog then we could add file overwrite check here
			if( nItemFound < 0 ) { // if the file name specified is not found in the list
				szFileInfo.Format("%s\n%d", szDirectory + szFileName, 0);
				POSITION pos = m_lstSelectedFileInfo.GetHeadPosition();
				m_lstSelectedFileInfo.SetAt(pos, szFileInfo);
			} else if( item.iImage == REMOTE_ITEM_PARENT || item.iImage == REMOTE_ITEM_LINK 
				|| item.iImage == REMOTE_ITEM_DIRECTORY ) { // if the file name specifed is a directory
				ChangeDirectory( szFileName );
				m_btnOK.EnableWindow( RefreshFileList() ); return;
			} else {
				CString szPrompt; szPrompt.Format("%s\nOverwrite the existing file?", szFileName);
				if( AfxMessageBox(szPrompt, MB_YESNO | MB_ICONEXCLAMATION) != IDYES ) return;

				szFileInfo.Format("%s\n%d", szDirectory + szFileName, item.lParam);
				POSITION pos = m_lstSelectedFileInfo.GetHeadPosition();
				m_lstSelectedFileInfo.SetAt(pos, szFileInfo);
			}
		}

	}

	CDialog::OnOK();
}

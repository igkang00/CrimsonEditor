#include "stdafx.h";
#include "cedtHeader.h"
#include "FtpClnt.h"
#include <afxinet.h>
#include "RemoteFile.h"



static BOOL CheckIfWinSockInitialized();
static BOOL CleanRemoteFileListItem(CString & szItem);
static BOOL ParseRemoteFileListItem(CString & szMode, DWORD & dwSize, CString & szTime, CString & szLeft, LPCTSTR lpszItem);



BOOL DownloadRemoteFile(CFtpAccount & rFtpAccount, LPCTSTR lpszRemoteFile, LPCTSTR lpszLocalFile, CALLBACK_FUNCTION fcnCallback)
{
	if( rFtpAccount.m_bUseWinInet ) {
		return DownloadRemoteFileWinInet(rFtpAccount, lpszRemoteFile, lpszLocalFile, fcnCallback);
	} else {
		return DownloadRemoteFileFtpClnt(rFtpAccount, lpszRemoteFile, lpszLocalFile, fcnCallback);
	}
}


BOOL DownloadRemoteFileWinInet(CFtpAccount & rFtpAccount, LPCTSTR lpszRemoteFile, LPCTSTR lpszLocalFile, CALLBACK_FUNCTION fcnCallback)
{
	if( ! rFtpAccount.IsValid() ) return FALSE;

	CInternetSession clsInternetSession;
	CFtpConnection * pFtpConnection = NULL;

	try {
		pFtpConnection = clsInternetSession.GetFtpConnection( 
			rFtpAccount.m_szServerName, rFtpAccount.m_szUserName, rFtpAccount.m_szPassword,
			rFtpAccount.m_nPortNumber, rFtpAccount.m_bPassiveMode );
	} catch( CException * ex ) {
		ex->ReportError(MB_OK | MB_ICONSTOP);
		ex->Delete(); return FALSE;
	} 

	DWORD dwSize = 0; CFtpFileFind find(pFtpConnection);
	BOOL bFound = find.FindFile(lpszRemoteFile);
	if( bFound ) {
		find.FindNextFile();
		dwSize = find.GetLength();
	} else {
		AfxMessageBox(IDS_ERR_FTP_FIND_REMOTE, MB_OK | MB_ICONSTOP);
		pFtpConnection->Close(); delete pFtpConnection;
		return FALSE;
	}

//	BOOL bResult = pFtpConnection->GetFile(lpszRemoteFile, lpszLocalFile, FALSE);
	CInternetFile * pInternetFile = NULL;
	CFile localFile; TCHAR szBuffer[FTP_TRANSFER_BUFFER_SIZE];
	BOOL bTransferCompleted = FALSE;

	try {
		DWORD dwFlags = rFtpAccount.m_bBinaryType ? FTP_TRANSFER_TYPE_BINARY : FTP_TRANSFER_TYPE_ASCII;
		pInternetFile = pFtpConnection->OpenFile(lpszRemoteFile, GENERIC_READ, dwFlags);
	} catch( CException * ex ) {
		ex->ReportError(MB_OK | MB_ICONSTOP);
		pFtpConnection->Close(); delete pFtpConnection;
		ex->Delete(); return FALSE;
	}

	if( ! localFile.Open(lpszLocalFile, CFile::modeReadWrite | CFile::modeCreate) ) {
		AfxMessageBox(IDS_ERR_FTP_OPEN_LOCAL, MB_OK | MB_ICONSTOP);
		pInternetFile->Close(); delete pInternetFile;
		pFtpConnection->Close(); delete pFtpConnection;
		return FALSE;
	}

	BOOL bContinue = TRUE; UINT nRead, nTotalRead = 0;
	while( bContinue ) {
		try {
			nRead = pInternetFile->Read(szBuffer, FTP_TRANSFER_BUFFER_SIZE);
			if( nRead ) { localFile.Write(szBuffer, nRead); nTotalRead += nRead; }
		} catch( CException * ex ) {
			ex->ReportError(MB_OK | MB_ICONSTOP);
			localFile.Close(); // close local file
			pInternetFile->Close(); delete pInternetFile;
			pFtpConnection->Close(); delete pFtpConnection;
			ex->Delete(); return FALSE;
		}

		if( fcnCallback && dwSize ) bContinue = fcnCallback(100U * nTotalRead / dwSize);
		if( ! bContinue || nRead < FTP_TRANSFER_BUFFER_SIZE ) break;
	}
	bTransferCompleted = bContinue;

	localFile.Close(); // close local file
	pInternetFile->Close();	delete pInternetFile;
	pFtpConnection->Close(); delete pFtpConnection;

	return bTransferCompleted;
}


BOOL DownloadRemoteFileFtpClnt(CFtpAccount & rFtpAccount, LPCTSTR lpszRemoteFile, LPCTSTR lpszLocalFile, CALLBACK_FUNCTION fcnCallback)
{
	if( ! rFtpAccount.IsValid() || ! CheckIfWinSockInitialized() ) return FALSE;

	CFtpClient client; 

	if( ! client.Open(rFtpAccount.m_szServerName, rFtpAccount.m_nPortNumber) ) {
		CString szMessage; szMessage.Format(IDS_ERR_FTP_SERVER_CONNECT, client.GetResponseMessage());
		AfxMessageBox( szMessage, MB_OK | MB_ICONSTOP ); 
		return FALSE;
	}

	if( ! client.LogOn(rFtpAccount.m_szUserName, rFtpAccount.m_szPassword) ) {
		CString szMessage; szMessage.Format(IDS_ERR_FTP_SERVER_LOGON, client.GetResponseMessage());
		AfxMessageBox( szMessage, MB_OK | MB_ICONSTOP ); 
		client.Close(); return FALSE;
	}

	DWORD dwSize = 0;

/*	Do not use client.GetFileSize() function because SIZE command is not the standard of FTP protocol (RFC-959).
 	We will use response message of the OpenFile member function to get the file size instead... */

/*	if( ! client.GetFileSize(lpszRemoteFile, dwSize) ) {
		CString szMessage; szMessage.Format(IDS_ERR_FTP_FIND_REMOTE1, client.GetResponseMessage());
		AfxMessageBox( szMessage, MB_OK | MB_ICONSTOP );
		client.LogOff(); client.Close(); return FALSE;
	} */

	CFile localFile; TCHAR szBuffer[FTP_TRANSFER_BUFFER_SIZE];
	BOOL bTransferCompleted = FALSE;

	if( ! client.OpenFile(TRUE, lpszRemoteFile, rFtpAccount.m_bBinaryType, rFtpAccount.m_bPassiveMode) ) {
		CString szMessage; szMessage.Format(IDS_ERR_FTP_OPEN_REMOTE, client.GetResponseMessage());
		AfxMessageBox( szMessage, MB_OK | MB_ICONSTOP ); 
		client.LogOff(); client.Close(); return FALSE;
	}

	// Retrieve file size from the response message of the OpenFile()
	CString szResponse = client.GetResponseMessage();
	INT nFound = szResponse.Find('(');
	if( nFound >= 0 ) {
		dwSize = atoi( szResponse.Mid(nFound+1) );
	} else {
		AfxMessageBox(IDS_ERR_FTP_FIND_REMOTE, MB_OK | MB_ICONSTOP);
		client.LogOff(); client.Close(); return FALSE;
	}

	if( ! localFile.Open(lpszLocalFile, CFile::modeReadWrite | CFile::modeCreate) ) {
		AfxMessageBox(IDS_ERR_FTP_OPEN_LOCAL, MB_OK | MB_ICONSTOP);
		client.CloseFile(); client.LogOff(); client.Close(); return FALSE;
	}

	BOOL bContinue = TRUE; UINT nRead, nTotalRead = 0;
	while( bContinue ) {
		try {
			nRead = client.ReadFile(szBuffer, FTP_TRANSFER_BUFFER_SIZE);
			if( nRead ) { localFile.Write(szBuffer, nRead); nTotalRead += nRead; }
		} catch( CException * ex ) {
			CString szMessage; szMessage.Format(IDS_ERR_FTP_FILE_TRANSFER);
			AfxMessageBox( szMessage, MB_OK | MB_ICONSTOP ); 
			localFile.Close(); client.Close(); ex->Delete(); return FALSE;
		}

		if( fcnCallback && dwSize ) bContinue = fcnCallback(100U * nTotalRead / dwSize);
		if( ! bContinue || nRead < FTP_TRANSFER_BUFFER_SIZE ) break;
	}
	bTransferCompleted = bContinue;

	localFile.Close(); // close local file
	client.CloseFile(); client.LogOff(); client.Close();

	return bTransferCompleted;
}


BOOL UploadLocalFile(CFtpAccount & rFtpAccount, LPCTSTR lpszLocalFile, LPCTSTR lpszRemoteFile, CALLBACK_FUNCTION fcnCallback)
{
	if( rFtpAccount.m_bUseWinInet ) {
		return UploadLocalFileWinInet(rFtpAccount, lpszLocalFile, lpszRemoteFile, fcnCallback);
	} else {
		return UploadLocalFileFtpClnt(rFtpAccount, lpszLocalFile, lpszRemoteFile, fcnCallback);
	}
}


BOOL UploadLocalFileWinInet(CFtpAccount & rFtpAccount, LPCTSTR lpszLocalFile, LPCTSTR lpszRemoteFile, CALLBACK_FUNCTION fcnCallback)
{
	if( ! rFtpAccount.IsValid() ) return FALSE;

	CInternetSession clsInternetSession;
	CFtpConnection * pFtpConnection = NULL;

	try {
		pFtpConnection = clsInternetSession.GetFtpConnection( 
			rFtpAccount.m_szServerName, rFtpAccount.m_szUserName, rFtpAccount.m_szPassword,
			rFtpAccount.m_nPortNumber, rFtpAccount.m_bPassiveMode );
	} catch( CException * ex ) {
		ex->ReportError(MB_OK | MB_ICONSTOP);
		ex->Delete(); return FALSE;
	} 

	DWORD dwSize = 0; CFileStatus clsStatus; 
	if( CFile::GetStatus(lpszLocalFile, clsStatus) ) {
		dwSize = clsStatus.m_size;
	} else {
		AfxMessageBox(IDS_ERR_FTP_FIND_LOCAL, MB_OK | MB_ICONSTOP);
		pFtpConnection->Close(); delete pFtpConnection;
		return FALSE;
	}

//	BOOL bResult = pFtpConnection->PutFile(lpszLocalFile, lpszRemoteFile);
	CInternetFile * pInternetFile = NULL;
	CFile localFile; TCHAR szBuffer[FTP_TRANSFER_BUFFER_SIZE];
	BOOL bTransferCompleted = FALSE;

	try {
		DWORD dwFlags = rFtpAccount.m_bBinaryType ? FTP_TRANSFER_TYPE_BINARY : FTP_TRANSFER_TYPE_ASCII;
		pInternetFile = pFtpConnection->OpenFile(lpszRemoteFile, GENERIC_WRITE, dwFlags);
	} catch( CException * ex ) {
		ex->ReportError(MB_OK | MB_ICONSTOP);
		pFtpConnection->Close(); delete pFtpConnection;
		ex->Delete(); return FALSE;
	}

	if( ! localFile.Open(lpszLocalFile, CFile::modeRead ) ) {
		AfxMessageBox(IDS_ERR_FTP_OPEN_LOCAL, MB_OK | MB_ICONSTOP);
		pInternetFile->Close(); delete pInternetFile;
		pFtpConnection->Close(); delete pFtpConnection;
		return FALSE;
	}

	BOOL bContinue = TRUE; UINT nRead, nTotalRead = 0;
	while( bContinue ) {
		try {
			nRead = localFile.Read(szBuffer, FTP_TRANSFER_BUFFER_SIZE);
			if( nRead ) { pInternetFile->Write(szBuffer, nRead); nTotalRead += nRead; }
		} catch( CException * ex ) {
			ex->ReportError(MB_OK | MB_ICONSTOP);
			localFile.Close(); // close local file
			pInternetFile->Close(); delete pInternetFile;
			pFtpConnection->Close(); delete pFtpConnection;
			ex->Delete(); return FALSE;
		}

		if( fcnCallback && dwSize ) bContinue = fcnCallback(100U * nTotalRead / dwSize);
		if( ! bContinue || nRead < FTP_TRANSFER_BUFFER_SIZE ) break;
	}
	bTransferCompleted = bContinue;

	localFile.Close(); // close local file
	pInternetFile->Close(); delete pInternetFile;
	pFtpConnection->Close(); delete pFtpConnection;

	return bTransferCompleted;
}


BOOL UploadLocalFileFtpClnt(CFtpAccount & rFtpAccount, LPCTSTR lpszLocalFile, LPCTSTR lpszRemoteFile, CALLBACK_FUNCTION fcnCallback)
{
	if( ! rFtpAccount.IsValid() || ! CheckIfWinSockInitialized() ) return FALSE;

	CFtpClient client;

	if( ! client.Open(rFtpAccount.m_szServerName, rFtpAccount.m_nPortNumber) ) {
		CString szMessage; szMessage.Format(IDS_ERR_FTP_SERVER_CONNECT, client.GetResponseMessage());
		AfxMessageBox( szMessage, MB_OK | MB_ICONSTOP ); 
		return FALSE;
	}

	if( ! client.LogOn(rFtpAccount.m_szUserName, rFtpAccount.m_szPassword) ) {
		CString szMessage; szMessage.Format(IDS_ERR_FTP_SERVER_LOGON, client.GetResponseMessage());
		AfxMessageBox( szMessage, MB_OK | MB_ICONSTOP ); 
		client.Close(); return FALSE;
	}

	DWORD dwSize = 0; CFileStatus clsStatus; 
	if( CFile::GetStatus(lpszLocalFile, clsStatus) ) {
		dwSize = clsStatus.m_size;
	} else {
		AfxMessageBox(IDS_ERR_FTP_FIND_LOCAL, MB_OK | MB_ICONSTOP);
		client.LogOff(); client.Close(); return FALSE;
	}

	CFile localFile; TCHAR szBuffer[FTP_TRANSFER_BUFFER_SIZE];
	BOOL bTransferCompleted = FALSE;

	if( ! client.OpenFile(FALSE, lpszRemoteFile, rFtpAccount.m_bBinaryType, rFtpAccount.m_bPassiveMode) ) {
		CString szMessage; szMessage.Format(IDS_ERR_FTP_OPEN_REMOTE, client.GetResponseMessage());
		AfxMessageBox( szMessage, MB_OK | MB_ICONSTOP ); 
		client.LogOff(); client.Close(); return FALSE;
	}

	if( ! localFile.Open(lpszLocalFile, CFile::modeRead ) ) {
		AfxMessageBox(IDS_ERR_FTP_OPEN_LOCAL, MB_OK | MB_ICONSTOP);
		client.CloseFile(); client.LogOff(); client.Close(); return FALSE;
	}

	BOOL bContinue = TRUE; UINT nRead, nTotalRead = 0;
	while( bContinue ) {
		try {
			nRead = localFile.Read(szBuffer, FTP_TRANSFER_BUFFER_SIZE);
			if( nRead ) { client.WriteFile(szBuffer, nRead); nTotalRead += nRead; }
		} catch( CException * ex ) {
			CString szMessage; szMessage.Format(IDS_ERR_FTP_FILE_TRANSFER);
			AfxMessageBox( szMessage, MB_OK | MB_ICONSTOP ); 
			localFile.Close(); client.Close(); ex->Delete(); return FALSE;
		}

		if( fcnCallback && dwSize ) bContinue = fcnCallback(100L * nTotalRead / dwSize);
		if( ! bContinue || nRead < FTP_TRANSFER_BUFFER_SIZE ) break;
	}
	bTransferCompleted = bContinue;

	localFile.Close(); // close local file
	client.CloseFile(); client.LogOff(); client.Close();

	return bTransferCompleted;
}


BOOL GetRemoteFileList(CSortStringArray & arrFileInfo, CFtpAccount & rFtpAccount, LPCTSTR lpszFilter)
{
	if( rFtpAccount.m_bUseWinInet ) {
		return GetRemoteFileListWinInet(arrFileInfo, rFtpAccount, lpszFilter);
	} else {
		return GetRemoteFileListFtpClnt(arrFileInfo, rFtpAccount, lpszFilter);
	}
}


BOOL GetRemoteFileListWinInet(CSortStringArray & arrFileInfo, CFtpAccount & rFtpAccount, LPCTSTR lpszFilter)
{
	if( ! rFtpAccount.IsValid() ) return FALSE;

	CInternetSession clsInternetSession; CWaitCursor wait;
	CFtpConnection * pFtpConnection = NULL;

	try {
		pFtpConnection = clsInternetSession.GetFtpConnection( 
			rFtpAccount.m_szServerName, rFtpAccount.m_szUserName, rFtpAccount.m_szPassword,
			rFtpAccount.m_nPortNumber, rFtpAccount.m_bPassiveMode );
	} catch( CException * ex ) {
		ex->ReportError(MB_OK | MB_ICONSTOP);
		ex->Delete(); return FALSE;
	} 

	if( rFtpAccount.m_szSubDirectory.GetLength() ) {
		if( ! pFtpConnection->SetCurrentDirectory( rFtpAccount.m_szSubDirectory ) ) {
			CString szMessage; szMessage.Format(IDS_ERR_FTP_SERVER_CWD2, rFtpAccount.m_szSubDirectory, "Not a directory?");
			AfxMessageBox( szMessage, MB_OK | MB_ICONSTOP ); // return FALSE;
		}
	}

	pFtpConnection->GetCurrentDirectory( rFtpAccount.m_szSubDirectory );
	TRACE1("[PWD]: %s\n", rFtpAccount.m_szSubDirectory);

	CFtpFileFind find(pFtpConnection);
	BOOL bFound = find.FindFile(NULL); 

	arrFileInfo.RemoveAll();
	while( bFound ) {
		bFound = find.FindNextFile();
		CString szFileInfo;

		CString szName = find.GetFileName();
		DWORD dwSize = find.GetLength();
		TRACE2("RemoteFileList: %s (%d)\n", szName, dwSize);

		if( ! find.IsDirectory() && find.IsNormal() && MatchFileFilter(szName, lpszFilter) ) { szFileInfo.Format("F/%s/%d", szName, dwSize); arrFileInfo.Add(szFileInfo); }
		else if( ! find.IsDirectory() && ! find.IsNormal() ) { szFileInfo.Format("D/%s/L", szName); arrFileInfo.Add(szFileInfo); }
		else if( find.IsDirectory() && ! find.IsDots() ) { szFileInfo.Format("D/%s/N", szName); arrFileInfo.Add(szFileInfo); }
	}

	pFtpConnection->Close(); delete pFtpConnection;

	return TRUE;
}

BOOL GetRemoteFileListFtpClnt(CSortStringArray & arrFileInfo, CFtpAccount & rFtpAccount, LPCTSTR lpszFilter)
{
	if( ! rFtpAccount.IsValid() || ! CheckIfWinSockInitialized() ) return FALSE;

	CFtpClient client; CWaitCursor wait;

	if( ! client.Open(rFtpAccount.m_szServerName, rFtpAccount.m_nPortNumber) ) {
		CString szMessage; szMessage.Format(IDS_ERR_FTP_SERVER_CONNECT, client.GetResponseMessage());
		AfxMessageBox( szMessage, MB_OK | MB_ICONSTOP ); return FALSE;
	}

	if( ! client.LogOn(rFtpAccount.m_szUserName, rFtpAccount.m_szPassword) ) {
		CString szMessage; szMessage.Format(IDS_ERR_FTP_SERVER_LOGON, client.GetResponseMessage());
		AfxMessageBox( szMessage, MB_OK | MB_ICONSTOP ); client.Close(); return FALSE;
	}

	if( rFtpAccount.m_szSubDirectory.GetLength() ) {
		if( ! client.SetCurrentDirectory( rFtpAccount.m_szSubDirectory ) ) {
			CString szMessage; szMessage.Format(IDS_ERR_FTP_SERVER_CWD1, client.GetResponseMessage());
			AfxMessageBox( szMessage, MB_OK | MB_ICONSTOP ); // client.Close(); return FALSE;
		}
	}

	client.GetCurrentDirectory( rFtpAccount.m_szSubDirectory );
	TRACE1("[PWD]: %s\n", rFtpAccount.m_szSubDirectory);

	CStringArray arrList;
	client.ListDirectory(arrList, NULL, rFtpAccount.m_bPassiveMode);

	INT nSize = arrList.GetSize();
	CString szMode, szTime, szName; DWORD dwSize;

	arrFileInfo.RemoveAll();
	for(INT i = 0; i < nSize; i++) {
		CString szFileInfo, szListItem = arrList.GetAt(i);
		CleanRemoteFileListItem( szListItem );

		if( ! ParseRemoteFileListItem(szMode, dwSize, szTime, szName, szListItem) ) continue;
		TRACE2("RemoteFileList: %s (%d)\n", szName, dwSize);

		if( szMode[0] == '-' && MatchFileFilter(szName, lpszFilter) ) { szFileInfo.Format("F/%s/%d", szName, dwSize); arrFileInfo.Add(szFileInfo); }
		else if( szMode[0] == 'l' ) { szFileInfo.Format("D/%s/L", szName); arrFileInfo.Add(szFileInfo); }
		else if( szMode[0] == 'd' && szName.Compare("..") && szName.Compare(".") ) { szFileInfo.Format("D/%s/N", szName); arrFileInfo.Add(szFileInfo); }
	}

	client.LogOff(); client.Close();

	return TRUE;
}


static BOOL CheckIfWinSockInitialized()
{
	static BOOL bWinSockInitialized = FALSE;

	if( ! bWinSockInitialized ) {
		if( ! AfxSocketInit() ) {
			AfxMessageBox("Could not initialize Windows Sockets!");
			return FALSE;
		}
		bWinSockInitialized = TRUE;
	}

	return bWinSockInitialized;
}

static BOOL CleanRemoteFileListItem(CString & szItem)
{
	static CRegExp clsRegExp;
	static BOOL bCompiled = FALSE;

	// compile regular expression for Korean HP server
	if( ! bCompiled && clsRegExp.RegComp("([0-9]+)¿ù ([0-9]+)ÀÏ [0-9]+:[0-9]+") ) bCompiled = TRUE;

	INT nFound; CString szFormat, szReplace;
	if( bCompiled && (nFound = clsRegExp.RegFind(szItem)) >= 0 ) {
		INT nLength = clsRegExp.GetFoundLength();
		CTime time = CTime::GetCurrentTime();
		szFormat.Format("%s.\\1.\\2", time.Format("%Y"));
		clsRegExp.GetReplaceString(szFormat, szReplace);
		szItem = szItem.Mid(0, nFound) + szReplace + szItem.Mid(nFound + nLength);
	}

	if( (nFound = szItem.Find(" -> ")) >= 0 ) {
		szItem = szItem.Mid(0, nFound);
	}

	return TRUE;
}


static BOOL ParseRemoteFileListItem(CString & szMode, DWORD & dwSize, CString & szTime, CString & szLeft, LPCTSTR lpszItem)
{
	TCHAR * BEG = (TCHAR *)lpszItem, * FWD = (TCHAR *)lpszItem;

	// extract file mode
	while( * FWD && ! isspace(* FWD) ) FWD++;
	if( FWD == BEG ) return FALSE;
	szMode = CString(BEG, FWD-BEG); 

	// skip spaces
	while( * FWD && isspace(* FWD) ) FWD++;
	if( * FWD == '\0' ) return FALSE;

	// advance pointer
	BEG = FWD;

	// skip block size, file owner, file group
	for(INT i = 0; i < 3; i++) {
		// skip something
		while( * FWD && ! isspace(* FWD) ) FWD++;
		if( * FWD == '\0' ) return FALSE;

		// skip spaces
		while( * FWD && isspace(* FWD) ) FWD++;
		if( * FWD == '\0' ) return FALSE;
	}

	// advance pointer
	BEG = FWD;

	// extract file size
	while( * FWD && ! isspace(* FWD) ) FWD++;
	if( FWD == BEG ) return FALSE;
	dwSize = atoi(BEG);

	// skip spaces
	while( * FWD && isspace(* FWD) ) FWD++;
	if( * FWD == '\0' ) return FALSE;

	// advance pointer
	BEG = FWD;

	// extract file creation time
	while( * FWD &&   (* FWD != ' ') && (* FWD != '.')   ) FWD++;
	while( * FWD && ( (* FWD == ' ') || (* FWD == '.') ) ) FWD++;
	while( * FWD &&   (* FWD != ' ') && (* FWD != '.')   ) FWD++;
	while( * FWD && ( (* FWD == ' ') || (* FWD == '.') ) ) FWD++;
	while( * FWD &&   (* FWD != ' ') && (* FWD != '.')   ) FWD++;
	if( FWD == BEG ) return FALSE;
	szTime = CString(BEG, FWD-BEG); 

/*	// extract file creation time
	while( * FWD && ! isspace(* FWD) ) FWD++;
	if( FWD == BEG ) return FALSE;
	szTime = CString(BEG, FWD-BEG); */

	// skip spaces
	while( * FWD && isspace(* FWD) ) FWD++;
	if( * FWD == '\0' ) return FALSE;

	// skip spaces
	while( * FWD && isspace(* FWD) ) FWD++;
	if( * FWD == '\0' ) return FALSE;

	// advance pointer
	BEG = FWD;

	// extract file name
	szLeft = BEG;

	return TRUE;
}

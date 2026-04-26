#include "stdafx.h"
#include "FtpClnt.h"


CFtpClient::CFtpClient()
{
	m_pControlSocket = NULL;
	m_pControlSocketFile = NULL;
	m_pControlReadArchive = NULL;
	m_pControlWriteArchive = NULL;

	m_pServerSocket = NULL;

	m_pDataSocket = NULL;
	m_pDataSocketFile = NULL;
	m_pDataReadArchive = NULL;
	m_pDataWriteArchive = NULL;

	m_bIsDataChannelEstablished = FALSE;
	m_bPassiveModeDataChannel = FALSE;

	m_bIsRemoteFileOpen = FALSE;
	m_bOpenRemoteFileToGet = FALSE;

	m_nResponseCode = 0x00;
	m_szResponseMessage = "";
}

CFtpClient::~CFtpClient()
{
	try { if( m_pDataReadArchive ) delete m_pDataReadArchive; } catch( CException * ex ) { ex->Delete(); }
	try { if( m_pDataWriteArchive ) delete m_pDataWriteArchive; } catch( CException * ex ) { ex->Delete(); }
	try { if( m_pDataSocketFile ) delete m_pDataSocketFile; } catch( CException * ex ) { ex->Delete(); }
	try { if( m_pDataSocket ) delete m_pDataSocket; } catch( CException * ex ) { ex->Delete(); }

	try { if( m_pServerSocket ) delete m_pServerSocket; } catch( CException * ex ) { ex->Delete(); }

	try { if( m_pControlReadArchive ) delete m_pControlReadArchive; } catch( CException * ex ) { ex->Delete(); }
	try { if( m_pControlWriteArchive ) delete m_pControlWriteArchive; } catch( CException * ex ) { ex->Delete(); }
	try { if( m_pControlSocketFile ) delete m_pControlSocketFile; } catch( CException * ex ) { ex->Delete(); }
	try { if( m_pControlSocket ) delete m_pControlSocket; } catch( CException * ex ) { ex->Delete(); }
}

BOOL CFtpClient::Open(LPCTSTR lpszServer, INT nPortNum)
{
	if( ! OpenControlChannel(lpszServer, nPortNum) ) return FALSE;

	if( ! ReadFromControlChannel() ) return FALSE; // 220 xxx FTP server (Version x.xxx Wek Mon DD HH:MM:SS EST 1997) ready.
	if( m_nResponseCode < 200 || m_nResponseCode >= 300 ) return FALSE;

	return TRUE;
}

BOOL CFtpClient::Close()
{
	if( ! CloseControlChannel() ) return FALSE;
	return TRUE;
}

BOOL CFtpClient::LogOn(LPCTSTR lpszUser, LPCTSTR lpszPassword)
{
	CString szString;

	szString.Format("USER %s\r\n", lpszUser);
	if( ! WriteToControlChannel( szString ) ) return FALSE;

	if( ! ReadFromControlChannel() ) return FALSE; // 331 Password required for xxx.
	if( m_nResponseCode < 200 || m_nResponseCode >= 400 ) return FALSE;

	// check if this server does not need password...
	if( m_nResponseCode == 230 ) return TRUE;

	szString.Format("PASS %s\r\n", lpszPassword);
	if( ! WriteToControlChannel( szString ) ) return FALSE;

	if( ! ReadFromControlChannel() ) return FALSE; // 230 User xxx logged in.
	if( m_nResponseCode < 200 || m_nResponseCode >= 300 ) return FALSE;

	return TRUE;
}

BOOL CFtpClient::LogOff()
{
	if( ! WriteToControlChannel("QUIT\r\n") ) return FALSE;

	try {
		if( ! ReadFromControlChannel() ) return FALSE; // 221 Goodbye.
		if( m_nResponseCode < 200 || m_nResponseCode >= 300 ) return FALSE;

	} catch( CException * ex ) { 
		// in case there is a firewall working in WindowXP Service Pack 2
		ex->Delete();
	}

	return TRUE;
}

BOOL CFtpClient::GetCurrentDirectory(CString & szDirName)
{
	if( ! WriteToControlChannel("PWD\r\n") ) return FALSE;

	if( ! ReadFromControlChannel() ) return FALSE;
	if( m_nResponseCode < 200 || m_nResponseCode >= 300 ) return FALSE;

	INT i = m_szResponseMessage.Find('"'); if( i < 0 ) return FALSE;
	INT j = m_szResponseMessage.Find('"', i+1); if( j < 0 ) return FALSE;
	szDirName = m_szResponseMessage.Mid(i+1, (j-i)-1);

	return TRUE;
}

BOOL CFtpClient::SetCurrentDirectory(LPCTSTR lpszDirName)
{
	CString szMessage; szMessage.Format("CWD %s\r\n", lpszDirName);
	if( ! WriteToControlChannel(szMessage) ) return FALSE;

	if( ! ReadFromControlChannel() ) return FALSE;
	if( m_nResponseCode < 200 || m_nResponseCode >= 300 ) return FALSE;

	return TRUE;
}

BOOL CFtpClient::SendCommand(LPCTSTR lpszCommand)
{
	CString szMessage; szMessage.Format("%s\r\n", lpszCommand);
	if( ! WriteToControlChannel(szMessage) ) return FALSE;

	if( ! ReadFromControlChannel() ) return FALSE;
	if( m_nResponseCode < 200 || m_nResponseCode >= 300 ) return FALSE;

	return TRUE;
}

BOOL CFtpClient::SetFileTransferType(BOOL bBinary)
{
	CString szMessage;

	szMessage.Format(bBinary ? "TYPE I\r\n" : "TYPE A\r\n");
	if( ! WriteToControlChannel(szMessage) ) return FALSE;
	if( ! ReadFromControlChannel() ) return FALSE; // 200 Type set to I
	if( m_nResponseCode < 200 || m_nResponseCode >= 300 ) return FALSE;

	return TRUE;
}

BOOL CFtpClient::GetFileSize(LPCTSTR lpszRemoteFile, DWORD & dwFileSize)
{
	CString szMessage;

	szMessage.Format("SIZE %s\r\n", lpszRemoteFile);
	if( ! WriteToControlChannel(szMessage) ) return FALSE;
	if( ! ReadFromControlChannel() ) return FALSE;
	if( m_nResponseCode < 200 || m_nResponseCode >= 300 ) return FALSE;

	dwFileSize = atoi(m_szResponseMessage.Mid(4));

	return TRUE;
}

BOOL CFtpClient::RenameFile(LPCTSTR lpszExisting, LPCTSTR lpszNewName)
{
	CString szMessage;

	szMessage.Format("RNFR %s\r\n", lpszExisting);
	if( ! WriteToControlChannel(szMessage) ) return FALSE;
	if( ! ReadFromControlChannel() ) return FALSE; // 350 File exists, ready for destination name.
	if( m_nResponseCode < 300 || m_nResponseCode >= 400 ) return FALSE;

	szMessage.Format("RNTO %s\r\n", lpszNewName);
	if( ! WriteToControlChannel(szMessage) ) return FALSE;
	if( ! ReadFromControlChannel() ) return FALSE; // 250 RNTO command successful.
	if( m_nResponseCode < 200 || m_nResponseCode >= 300 ) return FALSE;

	return TRUE;
}

BOOL CFtpClient::ListDirectory(CStringArray & arrList, LPCTSTR lpszFilter, BOOL bPassive)
{
	CString szCommand = "LIST";
	if( lpszFilter ) szCommand.Format("LIST %s", lpszFilter);

	if( ! EstablishDataChannel(bPassive, szCommand) ) return FALSE;

	CString szString; arrList.RemoveAll();
	while( ReadFromDataChannel(szString) ) arrList.Add(szString);

	if( ! DestroyDataChannel() ) return FALSE;

	return TRUE;
}

BOOL CFtpClient::GetFile(LPCTSTR lpszRemoteFile, LPCTSTR lpszLocalFile, BOOL bBinary, BOOL bPassive)
{
	CFile localFile; TCHAR szBuffer[FTP_TRANSFER_BUFFER_SIZE];

	if( ! OpenFile(TRUE, lpszRemoteFile, bBinary, bPassive) ) return FALSE;
	if( ! localFile.Open(lpszLocalFile, CFile::modeReadWrite | CFile::modeCreate) ) return FALSE;

	UINT nRead, nTotalRead = 0;
	while( TRUE ) {
		try {
			nRead = ReadFromDataChannel(szBuffer, FTP_TRANSFER_BUFFER_SIZE);
			localFile.Write(szBuffer, nRead); nTotalRead += nRead;
		} catch( CException * ex ) {
			localFile.Close(); throw ex;
		}

		if( nRead < FTP_TRANSFER_BUFFER_SIZE ) break;
	}

	localFile.Close();
	if( ! CloseFile() ) return FALSE;

	return TRUE;
}

BOOL CFtpClient::PutFile(LPCTSTR lpszLocalFile, LPCTSTR lpszRemoteFile, BOOL bBinary, BOOL bPassive)
{
	CFile localFile; TCHAR szBuffer[FTP_TRANSFER_BUFFER_SIZE];

	if( ! OpenFile(FALSE, lpszRemoteFile, bBinary, bPassive) ) return FALSE;
	if( ! localFile.Open(lpszLocalFile, CFile::modeRead) ) return FALSE;

	UINT nRead, nTotalRead = 0;
	while( TRUE ) {
		try {
			nRead = localFile.Read(szBuffer, FTP_TRANSFER_BUFFER_SIZE);
			WriteToDataChannel(szBuffer, nRead); nTotalRead += nRead;
		} catch( CException * ex ) {
			localFile.Close(); throw ex;
		}

		if( nRead < FTP_TRANSFER_BUFFER_SIZE ) break;
	}

	localFile.Close();
	if( ! CloseFile() ) return FALSE;

	return TRUE;
}

BOOL CFtpClient::OpenFile(BOOL bGet, LPCTSTR lpszRemoteFile, BOOL bBinary, BOOL bPassive)
{
	CString szCommand;
	if( m_bIsRemoteFileOpen ) return FALSE; // remote file is open already

	if( bGet ) szCommand.Format("RETR %s", lpszRemoteFile);
	else szCommand.Format("STOR %s", lpszRemoteFile);

	// type set to binary
	if( ! SetFileTransferType(bBinary) ) return FALSE;

	if( ! EstablishDataChannel(bPassive, szCommand) ) return FALSE;

	m_bIsRemoteFileOpen = TRUE;
	m_bOpenRemoteFileToGet = bGet;

	return TRUE;
}

BOOL CFtpClient::CloseFile()
{
	if( ! m_bIsRemoteFileOpen ) return FALSE; // remote file is not open yet

	if( ! DestroyDataChannel() ) return FALSE;

	m_bIsRemoteFileOpen = FALSE;
	m_bOpenRemoteFileToGet = FALSE;

	return TRUE;
}

UINT CFtpClient::ReadFile(VOID * lpBuf, UINT nMax)
{
	if( ! m_bIsRemoteFileOpen || ! m_bOpenRemoteFileToGet ) return FALSE;

	return ReadFromDataChannel(lpBuf, nMax);
}

BOOL CFtpClient::WriteFile(VOID * lpBuf, UINT nMax)
{
	if( ! m_bIsRemoteFileOpen || m_bOpenRemoteFileToGet ) return FALSE;

	return WriteToDataChannel(lpBuf, nMax);
}


BOOL CFtpClient::SetSocketErrorMessage(INT nErrorCode, LPCTSTR lpszDefaultErrorMessage)
{
	switch( nErrorCode ) {
	case WSANOTINITIALISED: m_szResponseMessage = "A successful AfxSocketInit must occur before using this API."; break;
	case WSAENETDOWN: m_szResponseMessage = "The Windows Sockets implementation detected that the network subsystem failed."; break;
	case WSAEADDRINUSE: m_szResponseMessage = "The specified address is already in use."; break;
	case WSAEINPROGRESS: m_szResponseMessage = "A blocking Windows Sockets call is in progress."; break;
	case WSAEADDRNOTAVAIL: m_szResponseMessage = "The specified address is not available from the local machine."; break;
	case WSAEAFNOSUPPORT: m_szResponseMessage = "Addresses in the specified family cannot be used with this socket."; break;
	case WSAECONNREFUSED: m_szResponseMessage = "The attempt to connect was rejected."; break;
	case WSAEDESTADDRREQ: m_szResponseMessage = "A destination address is required."; break;
	case WSAEFAULT: m_szResponseMessage = "The nSockAddrLen argument is incorrect."; break;
	case WSAEINVAL: m_szResponseMessage = "Invalid host address."; break;
	case WSAEISCONN: m_szResponseMessage = "The socket is already connected."; break;
	case WSAEMFILE: m_szResponseMessage = "No more file descriptors are available."; break;
	case WSAENETUNREACH: m_szResponseMessage = "The network cannot be reached from this host at this time."; break;
	case WSAENOBUFS: m_szResponseMessage = "No buffer space is available. The socket cannot be connected."; break;
	case WSAENOTSOCK: m_szResponseMessage = "The descriptor is not a socket."; break;
	case WSAETIMEDOUT: m_szResponseMessage = "Attempt to connect timed out without establishing a connection."; break;
	case WSAEWOULDBLOCK: m_szResponseMessage = "The socket is marked as nonblocking and the connection cannot be completed immediately."; break;
	case WSAEPROTONOSUPPORT: m_szResponseMessage = "The specified port is not supported."; break;
	case WSAEPROTOTYPE: m_szResponseMessage = "The specified port is the wrong type for this socket."; break;
	case WSAESOCKTNOSUPPORT: m_szResponseMessage = "The specified socket type is not supported in this address family."; break;
	default: m_szResponseMessage = lpszDefaultErrorMessage; break;
	}
	return TRUE;
}

BOOL CFtpClient::EstablishDataChannel(BOOL bPassive, LPCTSTR lpszCommand)
{
	CString szMessage, szString, szAddress; INT nPortNum;
	if( m_bIsDataChannelEstablished ) return FALSE; // data channel is established already

	if( ! bPassive ) { // active mode

		if( ! ListenServer(szAddress, nPortNum) ) return FALSE;

		if( ! StringFromAddress( szString, szAddress, nPortNum ) ) return FALSE;
		szMessage.Format("PORT %s\r\n", szString);
		if( ! WriteToControlChannel(szMessage) ) return FALSE;

		if( ! ReadFromControlChannel() ) return FALSE; // 200 PORT command successful.
		if( m_nResponseCode < 200 || m_nResponseCode >= 300 ) return FALSE;

		szMessage.Format("%s\r\n", lpszCommand);
		if( ! WriteToControlChannel(szMessage) ) return FALSE;

		if( ! ReadFromControlChannel() ) return FALSE; // 150 Opening ASCII mode data connection for file list.
		if( m_nResponseCode < 100 || m_nResponseCode >= 200 ) return FALSE;

		if( ! AcceptDataChannel() ) return FALSE;

	} else { // passive mode

		if( ! WriteToControlChannel("PASV\r\n") ) return FALSE;

		if( ! ReadFromControlChannel() ) return FALSE; // 227 Entering Passive Mode (xxx,xxx,xxx,xxx,xxx,xxx).
		if( m_nResponseCode < 200 || m_nResponseCode >= 300 ) return FALSE;

		INT i = m_szResponseMessage.Find('('); if( i < 0 ) return FALSE;
		INT j = m_szResponseMessage.Find(')'); if( j < 0 ) return FALSE;
		szString = m_szResponseMessage.Mid(i+1, (j-i)-1);

		szMessage.Format("%s\r\n", lpszCommand);
		if( ! WriteToControlChannel(szMessage) ) return FALSE;

		if( ! AddressFromString(szAddress, nPortNum, szString) ) return FALSE;
		if( ! ConnectDataChannel(szAddress, nPortNum) ) return FALSE;

		if( ! ReadFromControlChannel() ) return FALSE; // 150 Opening ASCII mode data connection for file list.
		if( m_nResponseCode < 100 || m_nResponseCode >= 200 ) return FALSE;

	}

	m_bIsDataChannelEstablished = TRUE;
	m_bPassiveModeDataChannel = bPassive;

	return TRUE;
}

BOOL CFtpClient::DestroyDataChannel()
{
	if( ! m_bIsDataChannelEstablished ) return FALSE; // no data chennel is established yet

	if( ! m_bPassiveModeDataChannel ) { // active mode

		if( ! CloseDataChannel() ) return FALSE;
		if( ! CloseServer() ) return FALSE;

		if( ! ReadFromControlChannel() ) return FALSE; // 226 Transfer complete
		if( m_nResponseCode < 200 || m_nResponseCode >= 300 ) return FALSE;

	} else { // passive mode

		if( ! CloseDataChannel() ) return FALSE;

		if( ! ReadFromControlChannel() ) return FALSE; // 226 Transfer complete.
		if( m_nResponseCode < 200 || m_nResponseCode >= 300 ) return FALSE;

	}

	m_bIsDataChannelEstablished = FALSE;
	m_bPassiveModeDataChannel = FALSE;

	return TRUE;
}

BOOL CFtpClient::StringFromAddress(CString & rString, LPCTSTR lpszAddress, INT nPortNum)
{
	CString szAddress = lpszAddress;

	INT i, len = szAddress.GetLength();
	for( i = 0; i < len; i++ ) if( szAddress.GetAt(i) == '.' ) szAddress.SetAt(i, ',');
	rString.Format("%s,%d,%d", szAddress, nPortNum / 256, nPortNum % 256);

	return TRUE;
}

BOOL CFtpClient::AddressFromString(CString & rAddress, INT & rPortNum, LPCTSTR lpszString)
{
	INT i; CString szString = lpszString;
	i = szString.ReverseFind(',');
	rPortNum = atoi( szString.Mid(i+1) );

	szString = szString.Left(i);
	i = szString.ReverseFind(',');
	rPortNum += 256 * atoi( szString.Mid(i+1) );

	rAddress = szString.Left(i); INT len = rAddress.GetLength();
	for( i = 0; i < len; i++ ) if( rAddress.GetAt(i) == ',' ) rAddress.SetAt(i, '.');

	return TRUE;
}

BOOL CFtpClient::WriteToControlChannel(LPCTSTR lpszString)
{
	if( ! m_pControlWriteArchive ) return FALSE;

	m_pControlWriteArchive->WriteString(lpszString);
	m_pControlWriteArchive->Flush();

	// lpszString already contains linefeed
	TRACE1("FTP-SEND: %s", lpszString);

	return TRUE;
}

BOOL CFtpClient::ReadFromControlChannel()
{
	CString szResponseMessage, szCode;
	if( ! m_pControlReadArchive ) return FALSE;

	// The client can identify the last line of the response as follows:
	// it begins with three ASCII digits and a space;
	// previous lines do not. The three digits form a code.
	if( ! m_pControlReadArchive->ReadString(szResponseMessage) ) return FALSE;
	m_szResponseMessage = szResponseMessage; 
	szCode = szResponseMessage.Mid(0,3); m_nResponseCode = atoi(szCode);

	// Codes between 100 and 199 indicate marks;
	// codes between 200 and 399 indicate acceptance;
	// codes between 400 and 599 indicate rejection.
	while( szResponseMessage[3] != ' ' || szCode.Compare("100") < 0 || szCode.Compare("999") > 0 ) {
		if( ! m_pControlReadArchive->ReadString(szResponseMessage) ) return FALSE;
		m_szResponseMessage += CString("\n") + szResponseMessage;
		szCode = szResponseMessage.Mid(0,3); m_nResponseCode = atoi(szCode);
	}

	TRACE1("FTP-RECV: %s\n", m_szResponseMessage);

	return TRUE;
}

BOOL CFtpClient::ReadFromDataChannel(CString & rString)
{
	if( ! m_pDataReadArchive ) return FALSE;
	return m_pDataReadArchive->ReadString(rString);
}

UINT CFtpClient::ReadFromDataChannel(VOID * lpBuf, UINT nMax)
{
	if( ! m_pDataReadArchive ) return 0;
	return m_pDataReadArchive->Read(lpBuf, nMax);
}

BOOL CFtpClient::WriteToDataChannel(VOID * lpBuf, UINT nMax)
{
	if( ! m_pDataReadArchive ) return FALSE;
	m_pDataWriteArchive->Write(lpBuf, nMax); return TRUE;
}

BOOL CFtpClient::OpenControlChannel(LPCTSTR lpszServer, INT nPortNum)
{
	if( ! (m_pControlSocket = new CSocket) ) return FALSE;

	if( ! m_pControlSocket->Create() ) {
		SetSocketErrorMessage( m_pControlSocket->GetLastError(), "An error occured in creating socket object." );
		return FALSE;
	}
	if( ! m_pControlSocket->Connect(lpszServer, nPortNum) ) {
		SetSocketErrorMessage( m_pControlSocket->GetLastError(), "An error occured in connecting control channel." );
		return FALSE;
	}

	if( ! (m_pControlSocketFile = new CSocketFile(m_pControlSocket)) ) return FALSE;
	if( ! (m_pControlReadArchive = new CArchive(m_pControlSocketFile, CArchive::load)) ) return FALSE;
	if( ! (m_pControlWriteArchive = new CArchive(m_pControlSocketFile, CArchive::store)) ) return FALSE;

	return TRUE;
}

BOOL CFtpClient::CloseControlChannel()
{
	try {
		if( m_pControlReadArchive ) delete m_pControlReadArchive;
		m_pControlReadArchive = NULL;
		if( m_pControlWriteArchive ) delete m_pControlWriteArchive;
		m_pControlWriteArchive = NULL;
		if( m_pControlSocketFile ) delete m_pControlSocketFile;
		m_pControlSocketFile = NULL;
		if( m_pControlSocket ) delete m_pControlSocket;
		m_pControlSocket = NULL;
	} catch( CException * ex ) {
		ex->ReportError(MB_OK | MB_ICONSTOP);
		ex->Delete(); return FALSE;
	}

	return TRUE;
}

BOOL CFtpClient::ListenServer(CString & rAddress, INT & rPortNum)
{
	CString szAddress; UINT nPortNum;

	if( ! m_pControlSocket ) return FALSE;
	if( ! m_pControlSocket->GetSockName(rAddress, nPortNum) ) return FALSE;

	if( ! (m_pServerSocket = new CSocket) ) return FALSE;
	if( ! m_pServerSocket->Create() ) return FALSE;
	if( ! m_pServerSocket->Listen() ) return FALSE;

	if( ! m_pServerSocket->GetSockName(szAddress, nPortNum) ) return FALSE;
	rPortNum = (INT)nPortNum;

	return TRUE;
}

BOOL CFtpClient::CloseServer()
{
	try {
		if( m_pServerSocket ) delete m_pServerSocket;
		m_pServerSocket = NULL;
	} catch( CException * ex ) {
		ex->ReportError(MB_OK | MB_ICONSTOP);
		ex->Delete(); return FALSE;
	}

	return TRUE;
}

BOOL CFtpClient::AcceptDataChannel()
{
	if( ! m_pServerSocket || m_pDataSocket ) return FALSE;
	if( ! (m_pDataSocket = new CSocket) ) return FALSE;
	if( ! m_pServerSocket->Accept( * m_pDataSocket ) ) return FALSE;

	if( ! (m_pDataSocketFile = new CSocketFile(m_pDataSocket)) ) return FALSE;
	if( ! (m_pDataReadArchive = new CArchive(m_pDataSocketFile, CArchive::load)) ) return FALSE;
	if( ! (m_pDataWriteArchive = new CArchive(m_pDataSocketFile, CArchive::store)) ) return FALSE;

	return TRUE;
}

BOOL CFtpClient::ConnectDataChannel(LPCTSTR lpszServer, INT nPortNum)
{
	if( m_pDataSocket ) return FALSE;
	if( ! (m_pDataSocket = new CSocket) ) return FALSE;
	if( ! m_pDataSocket->Create() ) return FALSE;
	m_pDataSocket->Connect(lpszServer, nPortNum); /* WSAEWOULDBLOCK error for CAsyncSocket */

	if( ! (m_pDataSocketFile = new CSocketFile(m_pDataSocket)) ) return FALSE;
	if( ! (m_pDataReadArchive = new CArchive(m_pDataSocketFile, CArchive::load)) ) return FALSE;
	if( ! (m_pDataWriteArchive = new CArchive(m_pDataSocketFile, CArchive::store)) ) return FALSE;

	return TRUE;
}

BOOL CFtpClient::CloseDataChannel()
{
	try {
		if( m_pDataReadArchive ) delete m_pDataReadArchive;
		m_pDataReadArchive = NULL;
		if( m_pDataWriteArchive ) delete m_pDataWriteArchive;
		m_pDataWriteArchive = NULL;
		if( m_pDataSocketFile ) delete m_pDataSocketFile;
		m_pDataSocketFile = NULL;
		if( m_pDataSocket ) delete m_pDataSocket;
		m_pDataSocket = NULL;
	} catch( CException * ex ) {
		ex->ReportError(MB_OK | MB_ICONSTOP);
		ex->Delete(); return FALSE;
	}

	return TRUE;
}

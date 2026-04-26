#ifndef __FTP_CLNT_H_
#define __FTP_CLNT_H_


#include <afxsock.h>


// READ/WRITE BUFFER SIZE
#define FTP_TRANSFER_BUFFER_SIZE	512


class CFtpClient
{
public:
	CFtpClient();
	virtual ~CFtpClient();

	BOOL Open(LPCTSTR lpszServer, INT nPortNum = 21);
	BOOL Close();

	BOOL LogOn(LPCTSTR lpszUser, LPCTSTR lpszPassword);
	BOOL LogOff();

	BOOL GetCurrentDirectory(CString & szDirName);
	BOOL SetCurrentDirectory(LPCTSTR lpszDirName);

	BOOL SendCommand(LPCTSTR lpszCommand);
	BOOL SetFileTransferType(BOOL bBinary);

	BOOL GetFileSize(LPCTSTR lpszRemoteFile, DWORD & dwFileSize);
	BOOL RenameFile(LPCTSTR lpszExisting, LPCTSTR lpszNewName);

	BOOL ListDirectory(CStringArray & arrList, LPCTSTR lpszFilter, BOOL bPassive);

	BOOL GetFile(LPCTSTR lpszRemoteFile, LPCTSTR lpszLocalFile, BOOL bBinary, BOOL bPassive);
	BOOL PutFile(LPCTSTR lpszLocalFile, LPCTSTR lpszRemoteFile, BOOL bBinary, BOOL bPassive);

	BOOL OpenFile(BOOL bGet, LPCTSTR lpszRemoteFile, BOOL bBinary, BOOL bPassive);
	BOOL CloseFile();

	UINT ReadFile(VOID * lpBuf, UINT nMax);
	BOOL WriteFile(VOID * lpBuf, UINT nMax);

	INT GetResponseCode() { return m_nResponseCode; }
	LPCTSTR GetResponseMessage() { return m_szResponseMessage; }

private:
	CSocket * m_pControlSocket;
	CSocketFile * m_pControlSocketFile;
	CArchive * m_pControlReadArchive, * m_pControlWriteArchive;

	CSocket * m_pServerSocket, * m_pDataSocket;
	CSocketFile * m_pDataSocketFile;
	CArchive * m_pDataReadArchive, * m_pDataWriteArchive;

	BOOL m_bIsDataChannelEstablished, m_bPassiveModeDataChannel;
	BOOL m_bIsRemoteFileOpen, m_bOpenRemoteFileToGet;

	INT m_nResponseCode;
	CString m_szResponseMessage;

private:
	BOOL SetSocketErrorMessage(INT nErrorCode, LPCTSTR lpszDefaultErrorMessage);

	BOOL EstablishDataChannel(BOOL bPassive, LPCTSTR lpszCommand);
	BOOL DestroyDataChannel();

private:
	BOOL StringFromAddress(CString & rString, LPCTSTR lpszAddress, INT nPortNum);
	BOOL AddressFromString(CString & rAddress, INT & rPortNum, LPCTSTR lpszString);

	BOOL WriteToControlChannel(LPCTSTR lpszString);
	BOOL ReadFromControlChannel();

	BOOL ReadFromDataChannel(CString & rString);
	UINT ReadFromDataChannel(VOID * lpBuf, UINT nMax);
	BOOL WriteToDataChannel(VOID * lpBuf, UINT nMax);

	BOOL OpenControlChannel(LPCTSTR lpszServer, INT nPortNum);
	BOOL CloseControlChannel();

	BOOL ListenServer(CString & rAddress, INT & rPortNum);
	BOOL CloseServer();

	BOOL AcceptDataChannel();
	BOOL ConnectDataChannel(LPCTSTR lpszServer, INT nPortNum);
	BOOL CloseDataChannel();
};


#endif // __FTP_CLNT_H_

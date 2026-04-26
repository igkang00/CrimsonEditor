#ifndef __REMOTE_FILE_H_
#define __REMOTE_FILE_H_


BOOL DownloadRemoteFile(CFtpAccount & rFtpAccount, LPCTSTR lpszRemoteFile, LPCTSTR lpszLocalFile, CALLBACK_FUNCTION fcnCallback = NULL);
BOOL DownloadRemoteFileWinInet(CFtpAccount & rFtpAccount, LPCTSTR lpszRemoteFile, LPCTSTR lpszLocalFile, CALLBACK_FUNCTION fcnCallback = NULL);
BOOL DownloadRemoteFileFtpClnt(CFtpAccount & rFtpAccount, LPCTSTR lpszRemoteFile, LPCTSTR lpszLocalFile, CALLBACK_FUNCTION fcnCallback = NULL);

BOOL UploadLocalFile(CFtpAccount & rFtpAccount, LPCTSTR lpszLocalFile, LPCTSTR lpszRemoteFile, CALLBACK_FUNCTION fcnCallback = NULL);
BOOL UploadLocalFileWinInet(CFtpAccount & rFtpAccount, LPCTSTR lpszLocalFile, LPCTSTR lpszRemoteFile, CALLBACK_FUNCTION fcnCallback = NULL);
BOOL UploadLocalFileFtpClnt(CFtpAccount & rFtpAccount, LPCTSTR lpszLocalFile, LPCTSTR lpszRemoteFile, CALLBACK_FUNCTION fcnCallback = NULL);

// FileInfo: "FILE_TYPE\tFILE_NAME\tFILE_SIZE"
BOOL GetRemoteFileList(CSortStringArray & arrFileInfo, CFtpAccount & rFtpAccount, LPCTSTR lpszFilter);
BOOL GetRemoteFileListWinInet(CSortStringArray & arrFileInfo, CFtpAccount & rFtpAccount, LPCTSTR lpszFilter);
BOOL GetRemoteFileListFtpClnt(CSortStringArray & arrFileInfo, CFtpAccount & rFtpAccount, LPCTSTR lpszFilter);


#endif // __REMOTE_FILE_H_

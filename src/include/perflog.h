#pragma once

// ---------------------------------------------------------------------------
// Temporary profiling instrumentation for large-file open.
//
// Measures the three load stages -- 1.FileLoad, 2.AnalyzeText,
// 3.FormatScreenText -- with QueryPerformanceCounter and logs the elapsed
// milliseconds both to the debugger (OutputDebugString, viewable in DebugView)
// and appended to %TEMP%\cedt_perf.log so it can be read from a release build
// with no debugger attached. The call sites gate on line count, so this only
// fires for large documents. Remove once the profiling work is done.
// ---------------------------------------------------------------------------

inline LONGLONG CedtPerfNow()
{
	LARGE_INTEGER t; QueryPerformanceCounter( & t ); return t.QuadPart;
}

inline void CedtPerfLog(LPCTSTR lpszLabel, LONGLONG llStart, INT nLines)
{
	LARGE_INTEGER freq; QueryPerformanceFrequency( & freq );
	double ms = (double)(CedtPerfNow() - llStart) * 1000.0 / (double)freq.QuadPart;

	CString szMsg;
	szMsg.Format(_T("[CEDT-PERF] %-18s %10.1f ms  (%d lines)\r\n"), lpszLabel, ms, nLines);
	OutputDebugString( szMsg );

	TCHAR szDir[MAX_PATH]; DWORD n = GetTempPath(MAX_PATH, szDir);
	if( n == 0 || n >= MAX_PATH ) return;
	CString szPath = CString(szDir) + _T("cedt_perf.log");

	HANDLE hFile = CreateFile(szPath, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile == INVALID_HANDLE_VALUE ) return;
	CStringA szAnsi( szMsg );
	DWORD dwWritten = 0;
	WriteFile(hFile, (LPCSTR)szAnsi, (DWORD)szAnsi.GetLength(), & dwWritten, NULL);
	CloseHandle(hFile);
}

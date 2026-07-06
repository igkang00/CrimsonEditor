// launch.cpp — fire-and-forget child-process launcher used by Crimson Editor.
//
// Invoked by cedt.exe as:
//     launch.exe <command> [args...]
//
// We strip our own argv[0] from the command line, CreateProcess the rest
// with CREATE_NEW_CONSOLE + inherit=FALSE, then immediately close the
// child handles and return. cedt waits on us (very briefly), so by the
// time cedt is unblocked the child has no parent left — it survives a
// later cedt exit. This is the "Capture output OFF, Close on exit OFF"
// path in CCedtView::ExecuteExecutable (cedtViewCommand.cpp).

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// GetCommandLineW gives the raw, exactly-as-typed command line as
	// UTF-16, with our own argv[0] (possibly quoted) at the front. Use
	// the W variant so tool paths with Chinese / Japanese / emoji or any
	// character outside the current ANSI code page survive.
	LPWSTR p = GetCommandLineW();
	if (*p == L'"') {
		++p;
		while (*p && *p != L'"') ++p;
		if (*p == L'"') ++p;
	} else {
		while (*p && *p != L' ' && *p != L'\t') ++p;
	}
	while (*p == L' ' || *p == L'\t') ++p;

	if (!*p) return 1;   // No command was given.

	STARTUPINFOW si = { sizeof(si) };
	PROCESS_INFORMATION pi = { 0 };
	if (!CreateProcessW(NULL, p, NULL, NULL,
	                    FALSE,                 // do NOT inherit our handles
	                    CREATE_NEW_CONSOLE,    // give the tool its own window
	                    NULL, NULL,
	                    &si, &pi)) {
		return (int)GetLastError();
	}

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	return 0;
}

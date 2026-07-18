// VC6 <fstream.h> compatibility shim for modern toolsets.
#pragma once
#include <fstream>
#include <iostream>
#include <locale>
using std::ifstream;
using std::ofstream;
using std::fstream;
using std::istream;
using std::ostream;
using std::wifstream;
using std::wofstream;
using std::wfstream;
using std::wistream;
using std::wostream;
using std::ios;
using std::ios_base;
using std::streampos;
using std::streamoff;
using std::streamsize;
using std::endl;

// A wide stream, by default, converts each byte of the file to a wchar with the C locale — so
// it can only carry ASCII intact. Writing a Korean path character then puts the stream into a
// fail state mid-write, and the file is truncated there. (This is exactly how a workspace with
// a Korean file path ended up cut off after `path="C:\Temp\`.)
//
// Imbue this before opening a wide stream to make it read/write the file as UTF-8, using the
// Windows UTF-8 code page (65001). This avoids the deprecated <codecvt> header — the runtime's
// code-page locale does the same wchar (UTF-16) <-> UTF-8 conversion. No BOM is written or
// expected, so pure-ASCII files — every workspace ever saved before this — round-trip
// unchanged, and only the non-ASCII bytes of a path are now encoded rather than dropped.
inline std::locale Utf8FileLocale()
{
	return std::locale(".65001");
}

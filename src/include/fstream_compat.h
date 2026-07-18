// VC6 <fstream.h> compatibility shim for modern toolsets.
#pragma once
#include <fstream>
#include <iostream>
#include <locale>

#pragma warning(push)
#pragma warning(disable: 4996)		// <codecvt> is deprecated in C++17, not removed; see below
#include <codecvt>
#pragma warning(pop)

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
// Imbue this before opening a wide stream to make it read/write the file as UTF-8. Windows
// wchar_t is UTF-16, so codecvt_utf8_utf16 is the correct pairing — codecvt_utf8 assumes UCS-2
// and would mishandle anything astral. No BOM is written or expected, so pure-ASCII files —
// every workspace ever saved before this — round-trip unchanged, and only the non-ASCII bytes
// of a path are now encoded rather than dropped.
//
// This uses the codecvt FACET rather than a named code-page locale like `std::locale(".65001")`.
// The named locale depends on the CRT actually having that code page's locale data, which is
// not present on every Windows/UCRT version — where it is missing the constructor throws, and
// on startup that unhandled exception crashed the app on a VM while working on the dev machine.
// The facet is STL code and constructs the same everywhere.
#pragma warning(push)
#pragma warning(disable: 4996)
inline std::locale Utf8FileLocale()
{
	return std::locale(std::locale::classic(), new std::codecvt_utf8_utf16<wchar_t>);
}
#pragma warning(pop)

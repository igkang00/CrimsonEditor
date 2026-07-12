#include "StdAfx.h"
#include "cedtElement.h"
#include <gtest/gtest.h>
#include <vector>

// CAnalyzedText::FileLoad / FileSave.
//
// The loader reads the file in blocks and assembles lines from them. The hazard is
// the block boundary: a multi-byte character (a UTF-8 sequence, a CP949 lead/trail
// pair, a UTF-16 code unit) or a CRLF can straddle one. A line break never can, which
// is why the loader accumulates a line's raw bytes and decodes the LINE, not the
// block.
//
// These tests use lines far longer than any plausible read block so the boundary is
// actually crossed, with multi-byte characters sitting on top of it.

namespace {

const TCHAR HI = (TCHAR)0xD83D;			// U+1F600, as a surrogate pair
const TCHAR LO = (TCHAR)0xDE00;

CString TempPath(LPCTSTR lpszName)
{
	TCHAR szDir[MAX_PATH]; GetTempPath(MAX_PATH, szDir);
	return CString(szDir) + lpszName;
}

void WriteBytes(LPCTSTR lpszPath, const void * pData, size_t nBytes)
{
	FILE * fp = NULL;
	_tfopen_s(&fp, lpszPath, _T("wb"));
	ASSERT_NE(nullptr, fp);
	if( nBytes ) fwrite(pData, 1, nBytes, fp);
	fclose(fp);
}

std::vector<unsigned char> ReadBytes(LPCTSTR lpszPath)
{
	std::vector<unsigned char> v;
	FILE * fp = NULL;
	_tfopen_s(&fp, lpszPath, _T("rb"));
	if( ! fp ) return v;
	unsigned char buf[4096]; size_t n;
	while( (n = fread(buf, 1, sizeof(buf), fp)) > 0 ) v.insert(v.end(), buf, buf + n);
	fclose(fp);
	return v;
}

// A line long enough to span several read blocks, made of a 2-code-unit-in-UTF-8
// character so the boundary lands INSIDE a character rather than between two.
CString LongHangulLine()
{
	CString s;
	for(int i = 0; i < 400; i++) s += _T("\xD55C\xAE00");	// 한글
	return s;										// 800 chars, 2400 bytes in UTF-8
}

std::string ToUtf8(const CString & s)
{
	int n = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)s, s.GetLength(), NULL, 0, NULL, NULL);
	std::string out((size_t)n, '\0');
	if( n ) WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)s, s.GetLength(), &out[0], n, NULL, NULL);
	return out;
}

} // namespace


///////////////////////////////////////////////////////////////////////////////
// UTF-8

TEST(AnalyzedTextFileTest, Utf8_LongLineWithCjkSurvivesBlockBoundaries)
{
	CString szLong = LongHangulLine();
	ASSERT_EQ(800, szLong.GetLength());

	std::string body = "first\r\n" + ToUtf8(szLong) + "\r\nlast";
	CString szPath = TempPath(_T("cedt_load_utf8.txt"));
	WriteBytes(szPath, body.data(), body.size());

	CAnalyzedText text;
	ASSERT_TRUE(text.FileLoad(szPath, ENCODING_TYPE_UTF8_XBOM, FILE_FORMAT_DOS));

	ASSERT_EQ(3, (int)text.GetCount());

	POSITION pos = text.GetHeadPosition();
	EXPECT_STREQ(_T("first"), (LPCTSTR)text.GetNext(pos));

	// The whole 2400-byte line must come back intact — not a single mangled
	// character where a read block happened to end.
	CAnalyzedString & rLong = text.GetNext(pos);
	EXPECT_EQ(800, rLong.GetLength());
	EXPECT_STREQ((LPCTSTR)szLong, (LPCTSTR)rLong);

	EXPECT_STREQ(_T("last"), (LPCTSTR)text.GetNext(pos));

	_tremove(szPath);
}

TEST(AnalyzedTextFileTest, Utf8_BomIsSkipped)
{
	std::string body = "\xEF\xBB\xBF" "alpha\r\nbeta";
	CString szPath = TempPath(_T("cedt_load_utf8bom.txt"));
	WriteBytes(szPath, body.data(), body.size());

	CAnalyzedText text;
	ASSERT_TRUE(text.FileLoad(szPath, ENCODING_TYPE_UTF8_WBOM, FILE_FORMAT_DOS));

	ASSERT_EQ(2, (int)text.GetCount());
	EXPECT_STREQ(_T("alpha"), (LPCTSTR)text.GetHead());	// no stray U+FEFF

	_tremove(szPath);
}

TEST(AnalyzedTextFileTest, Utf8_EmojiSurvivesLoad)
{
	std::string body = "a" "\xF0\x9F\x98\x80" "b";		// a U+1F600 b
	CString szPath = TempPath(_T("cedt_load_emoji.txt"));
	WriteBytes(szPath, body.data(), body.size());

	CAnalyzedText text;
	ASSERT_TRUE(text.FileLoad(szPath, ENCODING_TYPE_UTF8_XBOM, FILE_FORMAT_DOS));

	CAnalyzedString & r = text.GetHead();
	ASSERT_EQ(4, r.GetLength());						// 'a' + surrogate PAIR + 'b'
	EXPECT_EQ(HI, r[1]);
	EXPECT_EQ(LO, r[2]);

	_tremove(szPath);
}


///////////////////////////////////////////////////////////////////////////////
// Line splitting

TEST(AnalyzedTextFileTest, TrailingNewlineLeavesABlankLastLine)
{
	std::string body = "one\r\ntwo\r\n";
	CString szPath = TempPath(_T("cedt_load_trailing.txt"));
	WriteBytes(szPath, body.data(), body.size());

	CAnalyzedText text;
	ASSERT_TRUE(text.FileLoad(szPath, ENCODING_TYPE_ASCII, FILE_FORMAT_DOS));

	ASSERT_EQ(3, (int)text.GetCount());					// "one", "two", ""
	EXPECT_TRUE(text.GetTail().IsEmpty());

	_tremove(szPath);
}

TEST(AnalyzedTextFileTest, NoTrailingNewlineKeepsTheLastLine)
{
	std::string body = "one\r\ntwo";
	CString szPath = TempPath(_T("cedt_load_notrailing.txt"));
	WriteBytes(szPath, body.data(), body.size());

	CAnalyzedText text;
	ASSERT_TRUE(text.FileLoad(szPath, ENCODING_TYPE_ASCII, FILE_FORMAT_DOS));

	ASSERT_EQ(2, (int)text.GetCount());
	EXPECT_STREQ(_T("two"), (LPCTSTR)text.GetTail());

	_tremove(szPath);
}

TEST(AnalyzedTextFileTest, EmptyFileIsOneBlankLine)
{
	CString szPath = TempPath(_T("cedt_load_empty.txt"));
	WriteBytes(szPath, "", 0);

	CAnalyzedText text;
	ASSERT_TRUE(text.FileLoad(szPath, ENCODING_TYPE_ASCII, FILE_FORMAT_DOS));

	ASSERT_EQ(1, (int)text.GetCount());
	EXPECT_TRUE(text.GetHead().IsEmpty());

	_tremove(szPath);
}

// The CR of a CRLF used to be able to survive into the line if the pair straddled a
// read block: the CR went in with one block and the LF was stripped from the next.
// Long lines are what make that reachable.
TEST(AnalyzedTextFileTest, CrLfIsStrippedEvenOnVeryLongLines)
{
	CString szLong = LongHangulLine();
	std::string body;
	for(int i = 0; i < 8; i++) body += ToUtf8(szLong) + "\r\n";

	CString szPath = TempPath(_T("cedt_load_crlf.txt"));
	WriteBytes(szPath, body.data(), body.size());

	CAnalyzedText text;
	ASSERT_TRUE(text.FileLoad(szPath, ENCODING_TYPE_UTF8_XBOM, FILE_FORMAT_DOS));

	ASSERT_EQ(9, (int)text.GetCount());					// 8 lines + blank tail

	POSITION pos = text.GetHeadPosition();
	for(int i = 0; i < 8; i++) {
		CAnalyzedString & r = text.GetNext(pos);
		EXPECT_EQ(800, r.GetLength()) << "line " << i << " kept a stray CR or lost a char";
		EXPECT_STREQ((LPCTSTR)szLong, (LPCTSTR)r);
	}

	_tremove(szPath);
}


///////////////////////////////////////////////////////////////////////////////
// UTF-16

TEST(AnalyzedTextFileTest, Utf16LE_LongLineRoundTrips)
{
	CString szLong = LongHangulLine();

	std::vector<unsigned char> body;
	body.push_back(0xFF); body.push_back(0xFE);			// BOM
	CString whole = _T("head\r\n") + szLong + _T("\r\ntail");
	for(int i = 0; i < whole.GetLength(); i++) {
		unsigned short u = (unsigned short)whole[i];
		body.push_back((unsigned char)(u & 0xFF));
		body.push_back((unsigned char)(u >> 8));
	}

	CString szPath = TempPath(_T("cedt_load_u16le.txt"));
	WriteBytes(szPath, body.data(), body.size());

	CAnalyzedText text;
	ASSERT_TRUE(text.FileLoad(szPath, ENCODING_TYPE_UNICODE_LE, FILE_FORMAT_DOS));

	ASSERT_EQ(3, (int)text.GetCount());
	POSITION pos = text.GetHeadPosition();
	EXPECT_STREQ(_T("head"), (LPCTSTR)text.GetNext(pos));
	EXPECT_STREQ((LPCTSTR)szLong, (LPCTSTR)text.GetNext(pos));
	EXPECT_STREQ(_T("tail"), (LPCTSTR)text.GetNext(pos));

	_tremove(szPath);
}

TEST(AnalyzedTextFileTest, Utf16BE_LongLineRoundTrips)
{
	CString szLong = LongHangulLine();

	std::vector<unsigned char> body;
	body.push_back(0xFE); body.push_back(0xFF);			// BOM
	CString whole = _T("head\r\n") + szLong + _T("\r\ntail");
	for(int i = 0; i < whole.GetLength(); i++) {
		unsigned short u = (unsigned short)whole[i];
		body.push_back((unsigned char)(u >> 8));		// big endian
		body.push_back((unsigned char)(u & 0xFF));
	}

	CString szPath = TempPath(_T("cedt_load_u16be.txt"));
	WriteBytes(szPath, body.data(), body.size());

	CAnalyzedText text;
	ASSERT_TRUE(text.FileLoad(szPath, ENCODING_TYPE_UNICODE_BE, FILE_FORMAT_DOS));

	ASSERT_EQ(3, (int)text.GetCount());
	POSITION pos = text.GetHeadPosition();
	EXPECT_STREQ(_T("head"), (LPCTSTR)text.GetNext(pos));
	EXPECT_STREQ((LPCTSTR)szLong, (LPCTSTR)text.GetNext(pos));
	EXPECT_STREQ(_T("tail"), (LPCTSTR)text.GetNext(pos));

	_tremove(szPath);
}


///////////////////////////////////////////////////////////////////////////////
// Save -> Load -> Save must reproduce the file byte for byte

TEST(AnalyzedTextFileTest, Utf8_SaveLoadSaveIsByteIdentical)
{
	CString szLong = LongHangulLine();

	CAnalyzedText a;
	a.AddTail(_T("first line"));
	a.AddTail((LPCTSTR)szLong);
	a.AddTail(_T("emoji \xD83D\xDE00 tail"));
	a.AddTail(_T(""));

	CString szPath1 = TempPath(_T("cedt_rt_1.txt"));
	CString szPath2 = TempPath(_T("cedt_rt_2.txt"));

	ASSERT_TRUE(a.FileSave(szPath1, ENCODING_TYPE_UTF8_XBOM, FILE_FORMAT_DOS));

	CAnalyzedText b;
	ASSERT_TRUE(b.FileLoad(szPath1, ENCODING_TYPE_UTF8_XBOM, FILE_FORMAT_DOS));
	ASSERT_EQ(a.GetCount(), b.GetCount());

	ASSERT_TRUE(b.FileSave(szPath2, ENCODING_TYPE_UTF8_XBOM, FILE_FORMAT_DOS));

	EXPECT_EQ(ReadBytes(szPath1), ReadBytes(szPath2));

	_tremove(szPath1); _tremove(szPath2);
}

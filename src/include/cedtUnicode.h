// Crimson Editor — UTF-16 surrogate-pair helpers
//
// The editor indexes text by UTF-16 code units (TCHAR == wchar_t). For every
// character in the Basic Multilingual Plane — which includes all of the Hangul,
// Kana and common Han a user actually types — one code unit is one character,
// and the old "1 index = 1 char" arithmetic is correct.
//
// It is NOT correct above U+FFFF. Emoji and rare CJK (Extension B and beyond)
// are stored as a *surrogate pair*: a high surrogate (0xD800-0xDBFF) followed
// by a low surrogate (0xDC00-0xDFFF). Two code units, one character.
//
// Without these helpers the caret can rest between the halves, and Backspace
// deletes only one of them — leaving a lone surrogate, which is not a legal
// character in any encoding. WideCharToMultiByte cannot encode it and writes
// U+FFFD instead, so saving the file destroys the data for good.
//
// Unlike the MBCS lead/trail-byte machinery this replaced, surrogates are
// unambiguous: a single code unit tells you what it is, by range, with no
// context and no backward scanning. Hence these are cheap local checks rather
// than a stateful scanner.
//
// Under MBCS (#ifndef _UNICODE) every helper degrades to the old one-unit
// behavior, so call sites need no #ifdef of their own.

#ifndef __CEDT_UNICODE_H_
#define __CEDT_UNICODE_H_


inline BOOL IsHighSurrogate(TCHAR ch)
{
#ifdef _UNICODE
	unsigned int u = (unsigned int)(unsigned short)ch;
	return ( u >= 0xD800 && u <= 0xDBFF ) ? TRUE : FALSE;
#else
	(void)ch; return FALSE;
#endif
}

inline BOOL IsLowSurrogate(TCHAR ch)
{
#ifdef _UNICODE
	unsigned int u = (unsigned int)(unsigned short)ch;
	return ( u >= 0xDC00 && u <= 0xDFFF ) ? TRUE : FALSE;
#else
	(void)ch; return FALSE;
#endif
}

inline BOOL IsSurrogate(TCHAR ch)
{
	return ( IsHighSurrogate(ch) || IsLowSurrogate(ch) ) ? TRUE : FALSE;
}


// How many TCHAR units the character starting at nIdxX occupies: 2 for a
// well-formed pair, 1 otherwise. A *lone* surrogate reports 1 on purpose — we
// step over the damage one unit at a time rather than running off the end.
inline INT CharUnitsAt(LPCTSTR lpszLine, INT nIdxX, INT nLength)
{
	if( nIdxX < 0 || nIdxX >= nLength ) return 1;

	if( IsHighSurrogate(lpszLine[nIdxX]) &&
	    nIdxX + 1 < nLength &&
	    IsLowSurrogate(lpszLine[nIdxX + 1]) ) return 2;

	return 1;
}

// TRUE when nIdxX is a legal place to put the caret / start a range: anywhere
// except the low half of a pair. Index 0 and the end-of-string index always
// qualify.
inline BOOL IsCharBoundary(LPCTSTR lpszLine, INT nIdxX)
{
	if( nIdxX <= 0 ) return TRUE;

	return ( IsLowSurrogate(lpszLine[nIdxX]) &&
	         IsHighSurrogate(lpszLine[nIdxX - 1]) ) ? FALSE : TRUE;
}

// Pull an index that landed on the low half of a pair back onto the high half.
// Snapping *down* (rather than up) keeps the whole character on the side the
// caller was already looking at, so a pixel->index conversion never gains a
// half character it did not ask for.
inline INT SnapIdxX(LPCTSTR lpszLine, INT nIdxX)
{
	if( nIdxX > 0 &&
	    IsLowSurrogate(lpszLine[nIdxX]) &&
	    IsHighSurrogate(lpszLine[nIdxX - 1]) ) return nIdxX - 1;

	return nIdxX;
}

// Step forward one whole character. Snaps first, so a caller that handed us a
// mid-pair index still lands on the next boundary rather than on the low half.
inline INT NextIdxX(LPCTSTR lpszLine, INT nIdxX, INT nLength)
{
	nIdxX = SnapIdxX(lpszLine, nIdxX);
	return nIdxX + CharUnitsAt(lpszLine, nIdxX, nLength);
}

// Step back one whole character.
inline INT PrevIdxX(LPCTSTR lpszLine, INT nIdxX)
{
	if( nIdxX <= 0 ) return 0;

	if( nIdxX >= 2 &&
	    IsLowSurrogate(lpszLine[nIdxX - 1]) &&
	    IsHighSurrogate(lpszLine[nIdxX - 2]) ) return nIdxX - 2;

	return nIdxX - 1;
}

// How many code units to remove, starting at nIdxX, so that at least nUnits are
// gone AND the cut lands on a character boundary. Overwrite mode replaces "as
// many units as I am inserting"; without this it could stop halfway through a
// surrogate pair and strand the other half.
inline INT SpanToCharBoundary(LPCTSTR lpszLine, INT nIdxX, INT nUnits, INT nLength)
{
	INT nSpan = 0;
	while( nSpan < nUnits && nIdxX + nSpan < nLength )
		nSpan += CharUnitsAt(lpszLine, nIdxX + nSpan, nLength);

	return nSpan;
}


#endif // __CEDT_UNICODE_H_

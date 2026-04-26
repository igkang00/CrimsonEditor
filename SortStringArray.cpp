// SortStringArray.cpp: Implementierung der Klasse CSortStringArray.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SortStringArray.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
void CSortStringArray::BubbleSort(BOOL bCase)
{
	INT nBound = GetUpperBound();
	if( nBound <= 0 ) return;

	if( bCase ) BubbleSortInner(0, nBound);
	else BubbleSortInnerNoCase(0, nBound);
}

void CSortStringArray::QuickSort(BOOL bCase)
{
	INT nBound = GetUpperBound();
	if( nBound <= 0 ) return;

	if( bCase ) QuickSortInner(0, nBound);
	else QuickSortInnerNoCase(0, nBound);
}

void CSortStringArray::BubbleSortInner(INT nLeft, INT nRight)
{
	BOOL bNotDone = TRUE;   
	while( bNotDone ) {
		bNotDone = FALSE;
		for( INT nPos = nLeft; nPos < nRight; nPos++ ) {
			bNotDone |= CompareAndSwap( nPos );   
		}
	}
}

void CSortStringArray::BubbleSortInnerNoCase(INT nLeft, INT nRight)
{
	BOOL bNotDone = TRUE;   
	while( bNotDone ) {
		bNotDone = FALSE;
		for( INT nPos = nLeft; nPos < nRight; nPos++ ) {
			bNotDone |= CompareAndSwapNoCase( nPos );   
		}
	}
}

void CSortStringArray::QuickSortInner(INT l, INT r)
{
	INT i = l, j = r;
	CString & szTest = GetAt( (l + r) / 2 );

	do {
		while( GetAt(i).Compare(szTest) < 0 ) i++;
		while( GetAt(j).Compare(szTest) > 0 ) j--;
		if( i <= j ) { SwapElements(i, j); i++; j--; }
	} while( i <= j );

	if( l < j ) QuickSortInner(l, j);
	if( i < r ) QuickSortInner(i, r);
}

void CSortStringArray::QuickSortInnerNoCase(INT l, INT r)
{
	INT i = l, j = r;
	CString & szTest = GetAt( (l + r) / 2 );

	do {
		while( GetAt(i).CompareNoCase(szTest) < 0 ) i++;
		while( GetAt(j).CompareNoCase(szTest) > 0 ) j--;
		if( i <= j ) { SwapElements(i, j); i++; j--; }
	} while( i <= j );

	if( l < j ) QuickSortInnerNoCase(l, j);
	if( i < r ) QuickSortInnerNoCase(i, r);
}

BOOL CSortStringArray::CompareAndSwap(INT nPos1)
{
	INT nPos2 = nPos1 + 1;

	if( GetAt(nPos1).Compare( GetAt(nPos2) ) > 0 ) {
		SwapElements(nPos1, nPos2); return TRUE;
	} else return FALSE;
}

BOOL CSortStringArray::CompareAndSwapNoCase(INT nPos1)
{
	INT nPos2 = nPos1 + 1;

	if( GetAt(nPos1).CompareNoCase(GetAt(nPos2)) > 0 ) {
		SwapElements(nPos1, nPos2); return TRUE;   
	} else return FALSE;
}

void CSortStringArray::SwapElements(INT nPos1, INT nPos2)
{
	static CString szTemp;
	szTemp = GetAt(nPos1);
	SetAt(nPos1, GetAt(nPos2));
	SetAt(nPos2, szTemp);
}

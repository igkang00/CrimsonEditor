// SortStringArray.h: Schnittstelle für die Klasse CSortStringArray.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SORTSTRINGARRAY_H__64C6B322_B44E_11D2_955E_204C4F4F5020__INCLUDED_)
#define AFX_SORTSTRINGARRAY_H__64C6B322_B44E_11D2_955E_204C4F4F5020__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CSortStringArray : public CStringArray  
{
public:
	void Sort(BOOL bCase = FALSE) { QuickSort(bCase); }

	void BubbleSort(BOOL bCase = FALSE);
	void QuickSort(BOOL bCase = FALSE);

private: 
	void BubbleSortInner(INT nLeft, INT nRight);
	void BubbleSortInnerNoCase(INT nLeft, INT nRight);

	void QuickSortInner(INT nLeft, INT nRight);
	void QuickSortInnerNoCase(INT nLeft, INT nRight);

	BOOL CompareAndSwap(INT nPos);
	BOOL CompareAndSwapNoCase(INT nPos);

	void SwapElements(INT nPos1, INT nPos2);
};

#endif // !defined(AFX_SORTSTRINGARRAY_H__64C6B322_B44E_11D2_955E_204C4F4F5020__INCLUDED_)

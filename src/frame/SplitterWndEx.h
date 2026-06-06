#if !defined(AFX_SPLITTERWNDEX_H__1C22F761_5D97_40ED_ACC6_2D99A90619FA__INCLUDED_)
#define AFX_SPLITTERWNDEX_H__1C22F761_5D97_40ED_ACC6_2D99A90619FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SplitterWndEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSplitterWndEx window

class CSplitterWndEx : public CSplitterWnd
{
// Construction
public:
	CSplitterWndEx();

// Attributes
public:

// Operations
public:
	virtual BOOL SplitColumn(int cxBefore);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitterWndEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSplitterWndEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSplitterWndEx)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPLITTERWNDEX_H__1C22F761_5D97_40ED_ACC6_2D99A90619FA__INCLUDED_)

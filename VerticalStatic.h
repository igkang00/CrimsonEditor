#if !defined(AFX_VERTICALSTATIC_H__2DA3F386_7347_11D6_BC5E_0050CE184C9B__INCLUDED_)
#define AFX_VERTICALSTATIC_H__2DA3F386_7347_11D6_BC5E_0050CE184C9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VerticalStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVerticalStatic window

class CVerticalStatic : public CStatic
{
// Construction
public:
	CVerticalStatic();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVerticalStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVerticalStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CVerticalStatic)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VERTICALSTATIC_H__2DA3F386_7347_11D6_BC5E_0050CE184C9B__INCLUDED_)

#if !defined(AFX_DOCUMENTSUMMARY_H__E28ADC87_ACCE_11D6_BC5E_0050CE184C9B__INCLUDED_)
#define AFX_DOCUMENTSUMMARY_H__E28ADC87_ACCE_11D6_BC5E_0050CE184C9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DocumentSummary.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDocumentSummary dialog

class CDocumentSummary : public CDialog
{
// Construction
public:
	CDocumentSummary(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDocumentSummary)
	enum { IDD = IDD_DOCUMENT_SUMMARY };
	CString	m_szPathName;
	CString	m_szFileFormat;
	long	m_nFileSize;
	int		m_nLineCount;
	int		m_nWordCount;
	CString	m_szModDate;
	BOOL	m_bAttrHidden;
	BOOL	m_bAttrReadOnly;
	BOOL	m_bAttrSystem;
	CString	m_szEncodingType;
	int		m_nByteCount;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocumentSummary)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDocumentSummary)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOCUMENTSUMMARY_H__E28ADC87_ACCE_11D6_BC5E_0050CE184C9B__INCLUDED_)

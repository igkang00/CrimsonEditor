// DocumentSummary.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DocumentSummary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Format an integer with a comma every three digits (e.g. 57644723 -> "57,644,723").
static CString GroupThousands(long n)
{
	CString raw; raw.Format(_T("%ld"), n);
	BOOL bNeg = ( ! raw.IsEmpty() && raw[0] == _T('-') );
	CString digits = bNeg ? raw.Mid(1) : raw;

	CString out; INT nGroup = 0;
	for( INT i = digits.GetLength() - 1; i >= 0; i-- ) {
		out.Insert(0, digits[i]);
		if( ++nGroup % 3 == 0 && i > 0 ) out.Insert(0, _T(','));
	}
	return ( bNeg ? CString(_T("-")) : CString(_T("")) ) + out;
}

/////////////////////////////////////////////////////////////////////////////
// CDocumentSummary dialog


CDocumentSummary::CDocumentSummary(CWnd* pParent /*=NULL*/)
	: CDialog(CDocumentSummary::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDocumentSummary)
	m_szPathName = _T("");
	m_szFileFormat = _T("");
	m_nFileSize = 0;
	m_nLineCount = 0;
	m_nWordCount = 0;
	m_szModDate = _T("");
	m_bAttrHidden = FALSE;
	m_bAttrReadOnly = FALSE;
	m_bAttrSystem = FALSE;
	m_szEncodingType = _T("");
	m_nByteCount = 0;
	m_nCharCount = 0;
	//}}AFX_DATA_INIT
}


void CDocumentSummary::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDocumentSummary)
	DDX_Text(pDX, IDC_PATHNAME, m_szPathName);
	DDX_Text(pDX, IDC_FILE_FORMAT, m_szFileFormat);
	{ CString sz = GroupThousands(m_nFileSize);        DDX_Text(pDX, IDC_FILE_SIZE, sz); }
	{ CString sz = GroupThousands((long)m_nLineCount); DDX_Text(pDX, IDC_LINE_COUNT, sz); }
	{ CString sz = GroupThousands((long)m_nWordCount); DDX_Text(pDX, IDC_WORD_COUNT, sz); }
	DDX_Text(pDX, IDC_LMOD_DATE, m_szModDate);
	DDX_Check(pDX, IDC_ATTR_HIDDEN, m_bAttrHidden);
	DDX_Check(pDX, IDC_ATTR_READ_ONLY, m_bAttrReadOnly);
	DDX_Check(pDX, IDC_ATTR_SYSTEM, m_bAttrSystem);
	DDX_Text(pDX, IDC_ENCODING_TYPE, m_szEncodingType);
	{ CString sz = GroupThousands((long)m_nCharCount); DDX_Text(pDX, IDC_CHAR_COUNT, sz); }
	//}}AFX_DATA_MAP
}


// The file-attribute checkboxes are read-outs, not settings. In the dialog resource the boxes
// carry no text and are WS_DISABLED (so they clearly cannot be toggled and still show their
// checked state), while the labels beside them are separate static text that stays readable —
// disabling a checkbox with built-in text would grey the label too.

BEGIN_MESSAGE_MAP(CDocumentSummary, CDialog)
	//{{AFX_MSG_MAP(CDocumentSummary)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocumentSummary message handlers

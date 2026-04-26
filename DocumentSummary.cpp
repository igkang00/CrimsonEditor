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
	//}}AFX_DATA_INIT
}


void CDocumentSummary::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDocumentSummary)
	DDX_Text(pDX, IDC_PATHNAME, m_szPathName);
	DDX_Text(pDX, IDC_FILE_FORMAT, m_szFileFormat);
	DDX_Text(pDX, IDC_FILE_SIZE, m_nFileSize);
	DDX_Text(pDX, IDC_LINE_COUNT, m_nLineCount);
	DDX_Text(pDX, IDC_WORD_COUNT, m_nWordCount);
	DDX_Text(pDX, IDC_LMOD_DATE, m_szModDate);
	DDX_Check(pDX, IDC_ATTR_HIDDEN, m_bAttrHidden);
	DDX_Check(pDX, IDC_ATTR_READ_ONLY, m_bAttrReadOnly);
	DDX_Check(pDX, IDC_ATTR_SYSTEM, m_bAttrSystem);
	DDX_Text(pDX, IDC_ENCODING_TYPE, m_szEncodingType);
	DDX_Text(pDX, IDC_BYTE_COUNT, m_nByteCount);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDocumentSummary, CDialog)
	//{{AFX_MSG_MAP(CDocumentSummary)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocumentSummary message handlers

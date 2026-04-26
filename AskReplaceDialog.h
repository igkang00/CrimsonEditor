#if !defined(AFX_ASKREPLACEDIALOG_H__05E092E3_67E1_11D3_978C_00104B151BA8__INCLUDED_)
#define AFX_ASKREPLACEDIALOG_H__05E092E3_67E1_11D3_978C_00104B151BA8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AskReplaceDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAskReplaceDialog dialog

class CAskReplaceDialog : public CDialog
{
private:
	CPoint	m_ptInitialPos;

public:
	BOOL m_bReplaceAll;
	BOOL m_bSearchNext;
	BOOL m_bSearchPrev;

public:
	void SetInitialPos(CPoint point) { m_ptInitialPos = point; }

// Construction
public:
	CAskReplaceDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAskReplaceDialog)
	enum { IDD = IDD_ASK_REPLACE_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAskReplaceDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAskReplaceDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnYes();
	afx_msg void OnNo();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASKREPLACEDIALOG_H__05E092E3_67E1_11D3_978C_00104B151BA8__INCLUDED_)

//{{AFX_INCLUDES()
#include "msflexgrid.h"
//}}AFX_INCLUDES
#if !defined(AFX_COUNTTINGRANGE_H__689C2881_208D_484D_9ED6_5F6285575595__INCLUDED_)
#define AFX_COUNTTINGRANGE_H__689C2881_208D_484D_9ED6_5F6285575595__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CounttingRange.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCounttingRange dialog

class CCounttingRange : public CDialog
{
// Construction
public:
	CCounttingRange(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCounttingRange)
	enum { IDD = IDD_COUNTTING_RANGE };
	CMSFlexGrid	m_ctrlDropCountRange;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCounttingRange)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCounttingRange)
	afx_msg void OnClickMsflexgridDropRange();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickCmdDropSave();
	afx_msg void OnClickCmdDropReturn();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COUNTTINGRANGE_H__689C2881_208D_484D_9ED6_5F6285575595__INCLUDED_)

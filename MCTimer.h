//{{AFX_INCLUDES()
#include "labelcontrol.h"
//}}AFX_INCLUDES
#if !defined(AFX_MCTIMER_H__ECA11A0E_6601_41DC_B5BB_0FE3001D9564__INCLUDED_)
#define AFX_MCTIMER_H__ECA11A0E_6601_41DC_B5BB_0FE3001D9564__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MCTimer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMCTimer dialog

class CMCTimer : public CDialog
{
// Construction
public:
	int m_nStatus;
	int m_nTimer;
	int m_nCount;
	void CloseWindow();
	CMCTimer(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMCTimer)
	enum { IDD = IDD_MC_TIMER };
	CLabelControl	m_ctrlLabTimer;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMCTimer)
	public:
	virtual BOOL Create(CWnd* pParentWnd);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMCTimer)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MCTIMER_H__ECA11A0E_6601_41DC_B5BB_0FE3001D9564__INCLUDED_)

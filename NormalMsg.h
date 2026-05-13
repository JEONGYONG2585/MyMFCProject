//{{AFX_INCLUDES()
#include "labelcontrol.h"
#include "commandbutton.h"
//}}AFX_INCLUDES
#if !defined(AFX_NORMALMSG_H__184B8C69_84D7_4A4C_AE05_160367B7F778__INCLUDED_)
#define AFX_NORMALMSG_H__184B8C69_84D7_4A4C_AE05_160367B7F778__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NormalMsg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNormalMsg dialog

class CNormalMsg : public CDialog
{
// Construction
public:
	CNormalMsg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNormalMsg();
	CString m_strMsg1 , m_strMsg2 , m_strTitle;

	BOOL m_bOrigin;
	BOOL m_bTimer;
	UINT m_nStep;
	int m_nEndMessage;
	UINT m_nSelHide;
	int m_nTimerCount;

// Dialog Data
	//{{AFX_DATA(CNormalMsg)
	enum { IDD = IDD_NORMAL_MSG };
	CCommandButton	m_ctrlTitle;
	CLabelControl	m_ctrlMsg1;
	CLabelControl	m_ctrlMsg2;
	BOOL	m_bCheckZServoOff;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNormalMsg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

//
	void SubTimerFunc0();
	void SubTimerFunc1();
	void SubButtonHide();
	// Generated message map functions
	//{{AFX_MSG(CNormalMsg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnOK();
	afx_msg void OnClickCmdTitle();
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NORMALMSG_H__184B8C69_84D7_4A4C_AE05_160367B7F778__INCLUDED_)

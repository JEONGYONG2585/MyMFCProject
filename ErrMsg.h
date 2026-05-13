//{{AFX_INCLUDES()
#include "labelcontrol.h"
#include "commandbutton.h"
//}}AFX_INCLUDES
#if !defined(AFX_ERRMSG_H__F16C8C4C_66A2_4C47_8E89_2BA929C36F29__INCLUDED_)
#define AFX_ERRMSG_H__F16C8C4C_66A2_4C47_8E89_2BA929C36F29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ErrMsg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CErrMsg dialog

class CErrMsg : public CDialog
{
// Construction
public:
	CErrMsg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CErrMsg();

	// 
	UINT m_nErrCode;
	UINT m_nErrCode2;
	UINT m_nEndMessae;
	UINT m_nSelectAlarm; // 0 : Warning		1: alarm
//
	CString m_strMsg1;
	CString m_strMsg2;

// Dialog Data
	//{{AFX_DATA(CErrMsg)
	enum { IDD = IDD_ERR_MSG };
	CLabelControl	m_ctrlTitle;
	CLabelControl	m_ctrlMsg1;
	CLabelControl	m_ctrlMsg2;
	CCommandButton	m_ctrlReturn;
	CCommandButton	m_ctrlSafety;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CErrMsg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void err_Message();
//
	UINT m_nTimerCount;	

	// Generated message map functions
	//{{AFX_MSG(CErrMsg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClickCmdDo();
	afx_msg void OnClickLabelErrmsgTitle();
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnClickCmdSafety();
	afx_msg void OnClickCmdBuzzerOff();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ERRMSG_H__F16C8C4C_66A2_4C47_8E89_2BA929C36F29__INCLUDED_)

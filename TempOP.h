//{{AFX_INCLUDES()
#include "commandbutton.h"
//}}AFX_INCLUDES
#if !defined(AFX_TEMPOP_H__2C5E4B06_F585_4EA6_A6D7_17D2260D28D1__INCLUDED_)
#define AFX_TEMPOP_H__2C5E4B06_F585_4EA6_A6D7_17D2260D28D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TempOP.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTempOP dialog

class CTempOP : public CDialog
{
// Construction
public:
	void SelectLanguage();
	BOOL m_bJogControl;
	BOOL m_bStop;
	BOOL m_bStart;
	BOOL m_bOK;
	BOOL m_bDown;
	BOOL m_bUp;
	BOOL m_bRight;
	BOOL m_bLeft;
	CTempOP(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTempOP)
	enum { IDD = IDD_OP };
	CCommandButton	m_ctrlStart;
	CCommandButton	m_ctrlStop;
	CCommandButton	m_ctrlOK;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTempOP)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTempOP)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickStart();
	afx_msg void OnClickOk();
	afx_msg void OnClickStop();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnCancel();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPOP_H__2C5E4B06_F585_4EA6_A6D7_17D2260D28D1__INCLUDED_)

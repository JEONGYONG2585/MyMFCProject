//{{AFX_INCLUDES()
#include "labelcontrol.h"
#include "commandbutton.h"
//}}AFX_INCLUDES
#if !defined(AFX_SPECCOUNT_H__EA235C6D_0A09_429B_A5F3_54908D36EBF5__INCLUDED_)
#define AFX_SPECCOUNT_H__EA235C6D_0A09_429B_A5F3_54908D36EBF5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpecCount.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSpecCount dialog

class CSpecCount : public CDialog
{
// Construction
public:
	BOOL m_bMoveSpec;
	void OnDispDuedate();
	BOOL m_bSel[MAX_NOZZLE];
	void SubTimerFunc();
	CString m_strDueDate;
	void SubDispTime();
	CString OnReturnTime();
	CString m_strTime_spec;
	int OnTimeCal();
	int m_nTime_Spec;
	int m_nCount_Spec[MAX_NOZZLE];
	int m_nRealCount_Spec[MAX_NOZZLE];
	void Doc2Value();
	void Valu2Doc();
	CSpecCount(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSpecCount)
	enum { IDD = IDD_SPEC_COUNT };
	CLabelControl	m_ctrlSetCount1;
	CLabelControl	m_ctrlSetCount10;
	CLabelControl	m_ctrlSetCount11;
	CLabelControl	m_ctrlSetCount12;
	CLabelControl	m_ctrlSetCount2;
	CLabelControl	m_ctrlSetCount3;
	CLabelControl	m_ctrlSetCount4;
	CLabelControl	m_ctrlSetCount5;
	CLabelControl	m_ctrlSetCount6;
	CLabelControl	m_ctrlSetCount7;
	CLabelControl	m_ctrlSetCount8;
	CLabelControl	m_ctrlSetCount9;
	CLabelControl	m_ctrlSetTime;
	CLabelControl	m_ctrlRemainTime;
	CLabelControl	m_ctrlDispDate;
	CCommandButton	m_ctrlSelect_Head;
	CCommandButton	m_ctrlReset;
	CLabelControl	m_ctrlCountDisp1;
	CLabelControl	m_ctrlCountDisp10;
	CLabelControl	m_ctrlCountDisp11;
	CLabelControl	m_ctrlCountDisp12;
	CLabelControl	m_ctrlCountDisp2;
	CLabelControl	m_ctrlCountDisp3;
	CLabelControl	m_ctrlCountDisp4;
	CLabelControl	m_ctrlCountDisp5;
	CLabelControl	m_ctrlCountDisp6;
	CLabelControl	m_ctrlCountDisp7;
	CLabelControl	m_ctrlCountDisp8;
	CLabelControl	m_ctrlCountDisp9;
	CLabelControl	m_ctrlHead1;
	CLabelControl	m_ctrlHead10;
	CLabelControl	m_ctrlHead11;
	CLabelControl	m_ctrlHead12;
	CLabelControl	m_ctrlHead2;
	CLabelControl	m_ctrlHead3;
	CLabelControl	m_ctrlHead4;
	CLabelControl	m_ctrlHead5;
	CLabelControl	m_ctrlHead6;
	CLabelControl	m_ctrlHead7;
	CLabelControl	m_ctrlHead8;
	CLabelControl	m_ctrlHead9;
	CCommandButton	m_ctrlSpecmove1;
	CLabelControl	m_ctrlHead13;
	CLabelControl	m_ctrlHead14;
	CLabelControl	m_ctrlHead15;
	CLabelControl	m_ctrlHead16;
	CLabelControl	m_ctrlSetCount13;
	CLabelControl	m_ctrlSetCount14;
	CLabelControl	m_ctrlSetCount15;
	CLabelControl	m_ctrlSetCount16;
	CLabelControl	m_ctrlCountDisp13;
	CLabelControl	m_ctrlCountDisp14;
	CLabelControl	m_ctrlCountDisp15;
	CLabelControl	m_ctrlCountDisp16;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpecCount)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSpecCount)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickSetCount1();
	afx_msg void OnClickSetCount2();
	afx_msg void OnClickSetCount3();
	afx_msg void OnClickSetCount4();
	afx_msg void OnClickSetCount5();
	afx_msg void OnClickSetCount6();
	afx_msg void OnClickSetCount7();
	afx_msg void OnClickSetCount8();
	afx_msg void OnClickSetCount9();
	afx_msg void OnClickSetCount10();
	afx_msg void OnClickSetCount11();
	afx_msg void OnClickSetCount12();
	afx_msg void OnClickSetTime();
	afx_msg void OnClickSelectHead();
	afx_msg void OnClickReset();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClickCancel();
	afx_msg void OnClickOk();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnClickSelHead1();
	afx_msg void OnClickSelHead2();
	afx_msg void OnClickSelHead3();
	afx_msg void OnClickSelHead4();
	afx_msg void OnClickSelHead5();
	afx_msg void OnClickSelHead6();
	afx_msg void OnClickSelHead7();
	afx_msg void OnClickSelHead8();
	afx_msg void OnClickSelHead9();
	afx_msg void OnClickSelHead10();
	afx_msg void OnClickSelHead11();
	afx_msg void OnClickSelHead12();
	afx_msg void OnClickMoving();
	afx_msg void OnClickSelHead13();
	afx_msg void OnClickSelHead14();
	afx_msg void OnClickSelHead15();
	afx_msg void OnClickSelHead16();
	afx_msg void OnClickSetCount13();
	afx_msg void OnClickSetCount14();
	afx_msg void OnClickSetCount15();
	afx_msg void OnClickSetCount16();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPECCOUNT_H__EA235C6D_0A09_429B_A5F3_54908D36EBF5__INCLUDED_)

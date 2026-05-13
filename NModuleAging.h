//{{AFX_INCLUDES()
#include "msflexgrid.h"
#include "commandbutton.h"
#include "labelcontrol.h"
//}}AFX_INCLUDES
#if !defined(AFX_NMODULEAGING_H__8CC70F96_140C_4227_895F_263AB2717947__INCLUDED_)
#define AFX_NMODULEAGING_H__8CC70F96_140C_4227_895F_263AB2717947__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NModuleAging.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNModuleAging dialog

class CNModuleAging : public CDialog
{
// Construction
public:
	BOOL WriteNparam(int nHeadNum);
	int m_nCount; 
	int m_nStatus; 
	int m_nRngHzMin;
	int m_nRngHzMax;
	int m_nTimeMin;
	int m_nTimeSec; 
	int m_nTimeCycle; 
	void InitFlexGrid();
	BOOL m_bConditionMode; // TRUE : MANUAL INPUT , FALSE : BY RECIPE
	CNModuleAging(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNModuleAging)
	enum { IDD = IDD_N_AGING_DLG };
	CMSFlexGrid	m_ctrlNAgingCondition;
	CCommandButton	m_ctrlConditionMode;
	CCommandButton	m_ctrlDummyPos;
	CCommandButton	m_DummyChgPos;
	CCommandButton	m_ctrlAgingStart;
	CLabelControl	m_ctrlTimeMin;
	CLabelControl	m_ctrlTimeSec;
	CLabelControl	m_ctrlTimeCyc;
	int		m_nHzMode;
	CLabelControl	m_ctrlRngHzMin;
	CLabelControl	m_ctrlRngHzMax;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNModuleAging)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNModuleAging)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClickConditionMode();
	afx_msg void OnClickReturn();
	afx_msg void OnClickPtpDummy();
	afx_msg void OnClickPtpDummychange();
	afx_msg void OnClickAgingStart();
	afx_msg void OnClickAgingTimeMin();
	afx_msg void OnClickAgingTimeSec();
	afx_msg void OnClickAgingTimeCyc();
	afx_msg void OnRADIOFixHz();
	afx_msg void OnRADIOAdjustHz();
	afx_msg void OnClickRnghzMin();
	afx_msg void OnClickRnghzMax();
	afx_msg void OnClickFlexgNAging();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NMODULEAGING_H__8CC70F96_140C_4227_895F_263AB2717947__INCLUDED_)

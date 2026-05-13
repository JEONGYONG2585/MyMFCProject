//{{AFX_INCLUDES()
#include "labelcontrol.h"
#include "commandbutton.h"
//}}AFX_INCLUDES
#if !defined(AFX_MANUWARMUP_H__578586FC_90A8_425D_BFFC_8DE96277139B__INCLUDED_)
#define AFX_MANUWARMUP_H__578586FC_90A8_425D_BFFC_8DE96277139B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ManuWarmUp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CManuWarmUp dialog

class CManuWarmUp : public CDialog
{
// Construction
public:
	void SelectLanguage();
	BOOL m_bIsTRfOnlyMode_Old; // by ckh 2009.02.16 , 워밍전 현재 반송 전용유무를 저장, 워밍창 종료시 값을 반환한다. 
	BOOL OnReadParam();
	void OnSaveParam();
	int m_nActTime;
	int m_nActTimeDisp;
	int m_nActTimeCur;
	int m_nActTimePre;
	CTime m_pActTime;
	void SubTimerActTime();
	CManuWarmUp(CWnd* pParent = NULL);   // standard constructor
	virtual ~CManuWarmUp();
	CButton* m_pAxisChoice[2];	// S-Master, K-Master 
	CButton* m_pKAxisStepMove; //150521 HSN PTP Warmup추가
	
// Dialog Data
	//{{AFX_DATA(CManuWarmUp)
	enum { IDD = IDD_MANU_WARMUP };
	CLabelControl	m_ctrlTableLoopCount;
	CCommandButton	m_ctrlWarmupMode1;
	CCommandButton	m_ctrlWarmupMode2;
	CCommandButton	m_ctrlReturn;
	CCommandButton	m_ctrlWarmupRun;
	CLabelControl	m_ctrlSAxisMoveSpeed;
	CLabelControl	m_ctrlYAxisMoveSpeed;
	CLabelControl	m_ctrlMessageWarmUp;
	CLabelControl	m_ctrlRecipeName;
	CCommandButton	m_ctrlWarmupMode3;
	CLabelControl	m_ctrlColAxisMoveSpeed;
	CLabelControl	m_ctrlActTime;
	CLabelControl	m_ctrlSettingTime;
	CLabelControl	m_ctrlKAxisStepMoveLength;
	CLabelControl	m_ctrlKAxisStepMoveTimes;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManuWarmUp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
//
	UINT m_nModeSelection ;
	// Generated message map functions
	//{{AFX_MSG(CManuWarmUp)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClickTableLoopCount();
	afx_msg void OnClickWarmupX();
	afx_msg void OnClickWarmupY();
	afx_msg void OnClickWarmupMode1();
	afx_msg void OnClickWarmupMode2();
	afx_msg void OnClickWarmupRun();
	afx_msg void OnClickCmdReturn();
	afx_msg void OnClickTableMoveSpeed2();
	afx_msg void OnClickWarmupX2();
	afx_msg void OnClickWarmupY2();
	afx_msg void OnClickWarmupMode3();
	afx_msg void OnClickColMoveSpeed();
	afx_msg void OnClickSettingTime();
	afx_msg void OnClickBuzzer();
	afx_msg void OnClickActTime();
	afx_msg void OnClickKStepMoveLength();
	afx_msg void OnClickKStepMoveTimes();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANUWARMUP_H__578586FC_90A8_425D_BFFC_8DE96277139B__INCLUDED_)

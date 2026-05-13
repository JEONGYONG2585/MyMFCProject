//{{AFX_INCLUDES()
#include "labelcontrol.h"
#include "msflexgrid.h"
#include "commandbutton.h"
//}}AFX_INCLUDES
#if !defined(AFX_ADJUSTINITIAL_H__AAA4F05F_01CB_4B6B_9272_0775EDCB96CC__INCLUDED_)
#define AFX_ADJUSTINITIAL_H__AAA4F05F_01CB_4B6B_9272_0775EDCB96CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdjustInitial.h : header file
//
#include "SettingAssemble.h"

/////////////////////////////////////////////////////////////////////////////
// CAdjustInitial dialog

class CAdjustInitial : public CDialog
{
// Construction
public:
	void SelectLanguage();
	void SPDDataDisplay();
	void SubColorControl();
	void OnButtonSet(BOOL i);
	int CheckIfOnlyOneHead();
	long m_lAdjustMotorStatus[MAX_NOZZLE];
	void SubTimerAdjustInitialLimitDisp();
	BOOL m_bMoveFlag;
	CAdjustInitial(CWnd* pParent = NULL);   // standard constructor
	CSettingAssemble *pSettingAssembleDlg;

	virtual ~CAdjustInitial();

// Dialog Data
	//{{AFX_DATA(CAdjustInitial)
	enum { IDD = IDD_ADJUST_INITIAL };
	CLabelControl	m_ctrlRecipeName;
	CCommandButton	m_ctrlReturn;
	CLabelControl	m_ctrlMessageAdjustInitial;
	CCommandButton	m_ctrlUseInterlock;
	CMSFlexGrid	m_ctrlAdjustInitialSet;
	CCommandButton	m_ctrlModeSel;
	CCommandButton	m_ctrlPistonClear;
	CCommandButton	m_ctrlDisjoint;
	CCommandButton	m_ctrlJoint1;
	CCommandButton	m_ctrlJoint2;
	CCommandButton	m_ctrlLcSupply;
	CCommandButton	m_ctrlBubbleRemoval;
	CLabelControl	m_ctrlDegasSucPos;
	CLabelControl	m_ctrlSuctionSpeed;
	CLabelControl	m_ctrlDegasTimes2;
	CCommandButton	m_ctrlGateHome;
	CCommandButton	m_ctrlGateOpen;
	CCommandButton	m_ctrlGateClose;
	CCommandButton	m_ctrlPumpHome;
	CCommandButton	m_ctrlPumpTeach;
	CCommandButton	m_ctrlPumpDataSave;
	CLabelControl	m_ctrlDegasTimes1;
	CLabelControl	m_ctrlDischargeSpeed;
	CCommandButton	m_ctrlInitSupplyJob;
	CCommandButton	m_ctrlNormalSupplyJob;
	CLabelControl	m_ctrlInitSupplyTime;
	CLabelControl	m_ctrlNormalSupplyTime;
	CLabelControl	m_ctrlManuCanWeight;
	CLabelControl	m_ctrlNDischargeSpeed1;
	CLabelControl	m_ctrlNDischargeSpeed2;
	CLabelControl	m_ctrlNDegasSucPos;
	CCommandButton	m_ctrlNDegas;
	CLabelControl	m_ctrlNDegasTimes;
	CLabelControl	m_ctrlNOverTime;
	CCommandButton	m_ctrlNAging;
	CLabelControl	m_ctrlNAgingTimes;
	CLabelControl	m_ctrlNAgingSupplyTime;
	CLabelControl	m_ctrlNAgingCycle;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdjustInitial)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	//
	int m_nTimerCount;
	//	func
	void SubDisplayRecipeName();
	void SubDisplayVolumnPosition();
	void SubTimerAdjustInitialFunc();
	void SubTimerAdjustInitialVolDispFunc();

	// Generated message map functions
	//{{AFX_MSG(CAdjustInitial)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClickCmdReturn();
	afx_msg void OnClickCmdInterlockusage();
	afx_msg void OnClickMsflexgridAdjustinitialSet();
	afx_msg void OnClickCmdModeSeclect();
	afx_msg void OnClickCmdPistonDown();
	afx_msg void OnClickCmdPumpDisjoint();
	afx_msg void OnClickCmdPumpJoint1();
	afx_msg void OnClickCmdPumpJoint2();
	afx_msg void OnClickCmdLcSupply();
	afx_msg void OnClickCmdBubbleRemoval1();
	afx_msg void OnClickDegasSuctionDistance();
	afx_msg void OnClickDegasSuctionSpeed();
	afx_msg void OnClickDegasDischargeSpeed();
	afx_msg void OnClickDegasTimes1();
	afx_msg void OnClickDegasTimes2();
	afx_msg void OnClickCmdGateClose();
	afx_msg void OnClickCmdGateHome();
	afx_msg void OnClickCmdGateOpen();
	afx_msg void OnClickCmdPumpHome();
	afx_msg void OnClickCmdSetting();
	afx_msg void OnClickCmdSpdSave();
	afx_msg void OnClickCmdLcInitSupplyJob();
	afx_msg void OnClickCmdLcNormalSupplyJob2();
	afx_msg void OnClickInitSupplyTime();
	afx_msg void OnClickNormalSupplyTime();
	afx_msg void OnClickManualAlarmWeight();
	afx_msg void OnClickLabelRecipe();
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnClickCmdNBubbleRemoval();
	afx_msg void OnClickDegasNSuctionDistance();
	afx_msg void OnClickDegasNDishcargeSpeed1();
	afx_msg void OnClickDegasNDishcargeSpeed2();
	afx_msg void OnClickNDegasTimes();
	afx_msg void OnClickNOverTime();
	afx_msg void OnClickCmdNAging();
	afx_msg void OnClickNAgingTimes();
	afx_msg void OnClickNAgingSupplyTime();
	afx_msg void OnClickNAgingCycle();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADJUSTINITIAL_H__AAA4F05F_01CB_4B6B_9272_0775EDCB96CC__INCLUDED_)

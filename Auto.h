//{{AFX_INCLUDES()
#include "msflexgrid.h"
#include "labelcontrol.h"
#include "commandbutton.h"
//}}AFX_INCLUDES
#if !defined(AFX_AUTO_H__63F646CB_F011_4EE3_883B_79A3B2F5B4A4__INCLUDED_)
#define AFX_AUTO_H__63F646CB_F011_4EE3_883B_79A3B2F5B4A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Auto.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAuto dialog

class CAuto : public CDialog
{
// Construction
public:	
	int nTempCount_S[MAX_NOZZLE];
	int nTempCount_K[2];
	void SubTimerAutoTPD();
	void SelectLanguage();
	CString m_strLog;
	BOOL m_bBuzzer;
	BOOL m_bStepNotSelError;
	void SubTimerSupplyFunc();
//	void SubTimerAutoSpecTime();
//	CString m_strDuedate;
//	CString OnCalDuedate();
	void DispLCQntyLabel(int nH, int nValue);
	void OnSubMeaHeadDispReturn(WPARAM wParam, LPARAM lParam);
	void SubAutoMeaHeadDisp(int nBalNo);
	CTime m_pTimeStepStart;
	CTime m_pTimeCurrent;
	CTime m_nCycleTimeDisp;
	int m_nCycleTimeCur;
	int m_nCycleTimePrev;

	int m_nMeaInterval;
	void SubTimerAutoCycleTimeDisplay();
	unsigned long int m_ulAutoCycletime;
	CAuto(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAuto();
	void Sub_Amount_Display(BOOL);
	void Sub_LC_Amount_Sensor_Disp();
	void SubAutoMeasuredDataDisplay();
	CString PanelInformationConvert(int Panel_Info);

	int m_nAutoTimerCount;
	int m_nAutoStepTimerCount;
	int m_nAutoTime[7];
	int m_nJobStep, m_nJobStepBK;
	int m_nLoopCountBK[MAX_NOZZLE];
	int m_nSupplyTimeCur;
	int m_nSupplyTimePrev;
	int m_nSupplyTimeCount;

	unsigned long m_nOldTempSpanTime;
	BOOL m_bTimeCheck;
	BOOL m_bAdjustErrRange;


//
	CProgressCtrl	*m_pQnty[MAX_NOZZLE];
// Dialog Data
	//{{AFX_DATA(CAuto)
	enum { IDD = IDD_AUTO };
	CButton	m_ctrlAdjustInAuto;
	CMSFlexGrid	m_ctrlAutoMeaData;
	CLabelControl	m_ctrlRecipeName;
	CCommandButton	m_ctrlAutoDrop;
	CCommandButton	m_ctrlAutoUnLoad;
	CCommandButton	m_ctrlAutoContinuous;
	CCommandButton	m_ctrlAutoScrap;
	CCommandButton	m_ctrlAutoStep;
	CCommandButton	m_ctrlReturn;
	CLabelControl	m_ctrlMessage;
	CLabelControl	m_ctrlProgress00;
	CLabelControl	m_ctrlProgress01;
	CLabelControl	m_ctrlProgress02;
	CLabelControl	m_ctrlProgress03;
	CLabelControl	m_ctrlProgress04;
	CCommandButton	m_ctrlAutoLoad;
	CCommandButton	m_ctrlAutoMeasure;
	CCommandButton	m_ctrlAutoDummyDrop;
	CLabelControl	m_ctrlPatternDispensing;
	CLabelControl	m_ctrlNgPanelSkip;
	CLabelControl	m_ctrlRecipeNumCompare;
	CMSFlexGrid	m_ctrlAutoWorkData;
	CMSFlexGrid	m_ctrlAutoDropCondition;
	CMSFlexGrid	m_ctrlDotNum;
	CMSFlexGrid	m_ctrlGlassProcess;
	CCommandButton	m_ctrlOffLineMode;
	CLabelControl	m_ctrlBalUse1;
	CLabelControl	m_ctrlBalUse2;
	CLabelControl	m_ctrlBalUse3;
	CLabelControl	m_ctrlBalUse4;
	CLabelControl	m_ctrlLCQntyDisp1;
	CLabelControl	m_ctrlLCQntyDisp10;
	CLabelControl	m_ctrlLCQntyDisp11;
	CLabelControl	m_ctrlLCQntyDisp12;
	CLabelControl	m_ctrlLCQntyDisp2;
	CLabelControl	m_ctrlLCQntyDisp3;
	CLabelControl	m_ctrlLCQntyDisp4;
	CLabelControl	m_ctrlLCQntyDisp5;
	CLabelControl	m_ctrlLCQntyDisp6;
	CLabelControl	m_ctrlLCQntyDisp7;
	CLabelControl	m_ctrlLCQntyDisp8;
	CLabelControl	m_ctrlLCQntyDisp9;
	CLabelControl	m_DropUse1;
	CLabelControl	m_DropUse2;
	CLabelControl	m_DropUse3;
	CLabelControl	m_DropUse4;
	CLabelControl	m_DropUse5;
	CLabelControl	m_DropUse6;
	CLabelControl	m_DropUse7;
	CLabelControl	m_DropUse8;
	CLabelControl	m_DropUse9;
	CLabelControl	m_DropUse10;
	CLabelControl	m_DropUse11;
	CLabelControl	m_DropUse12;
	CLabelControl	m_ctrlLCSensorDisp1;
	CLabelControl	m_ctrlLCSensorDisp2;
	CLabelControl	m_ctrlLCSensorDisp3;
	CLabelControl	m_ctrlLCSensorDisp4;
	CLabelControl	m_ctrlLCSensorDisp5;
	CLabelControl	m_ctrlLCSensorDisp6;
	CLabelControl	m_ctrlLCSensorDisp7;
	CLabelControl	m_ctrlLCSensorDisp8;
	CLabelControl	m_ctrlLCSensorDisp9;
	CLabelControl	m_ctrlLCSensorDisp10;
	CLabelControl	m_ctrlLCSensorDisp11;
	CLabelControl	m_ctrlLCSensorDisp12;
	CLabelControl	m_ctrlLCQuanRep;
	CLabelControl	m_ctrlLCQuanRep01;
	CLabelControl	m_ctrlSpecmove;
	CLabelControl	m_ctrlAddChange;
	CLabelControl	m_ctrlAddCompare;
	CLabelControl	m_ctrlLCQntyDisp13;
	CLabelControl	m_ctrlLCQntyDisp14;
	CLabelControl	m_ctrlLCQntyDisp15;
	CLabelControl	m_ctrlLCQntyDisp16;
	CLabelControl	m_DropUse13;
	CLabelControl	m_DropUse14;
	CLabelControl	m_DropUse15;
	CLabelControl	m_DropUse16;
	CLabelControl	m_ctrlBalUse5;
	CLabelControl	m_ctrlBalUse6;
	CLabelControl	m_ctrlLCSensorDisp13;
	CLabelControl	m_ctrlLCSensorDisp14;
	CLabelControl	m_ctrlLCSensorDisp15;
	CLabelControl	m_ctrlLCSensorDisp16;
	CLabelControl	m_ctrlMessage1;
	CLabelControl	m_ctrlProgress05;
	CMSFlexGrid	m_ctrlCanRemain;
	CCommandButton	m_ctrlGrease2;
	CCommandButton	m_ctrlGreaseMsg;
	CCommandButton	m_ctrlSpecialInfo;
	CCommandButton	m_ctrlGlassDataReq;
	CLabelControl	m_ctrlLCReMain;
	CCommandButton	m_ctrlAutoModeTitle;
	CLabelControl	m_ctrlCanisterUseDisplay;
	CLabelControl	m_ctrlBalUse7;
	CLabelControl	m_ctrlBalUse8;
	CLabelControl	m_ctrlMarkInsp;
	CLabelControl	m_ctrlInterDrop;
	CCommandButton	m_ctrlWarmUp;
	CLabelControl	m_ctrlRecipeInfo;
	CCommandButton	m_ctrlLoadSensorMode;
	CCommandButton	m_ctrlJobMovePos;
	CCommandButton	m_ctrlValcRun;
	CCommandButton	m_ctrlPdDownStatus;
	CLabelControl	m_ctrlvisionuse;
	CCommandButton	m_ctrlAutoVision;
	CLabelControl	m_ctrlProgress06;
	CCommandButton	m_ctrlLogWrite;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAuto)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
//
	CTime m_pTime;
	CString m_strTime;
	CTime m_pTimeDisplay;
	CTime m_pTimeSupply;

//
	void SubDotNumDisplay(int nCountMode);
	void SubDisplayRecipeName();
	// timer func
	void SubTimerAutoFunc();
	void SubAutoMsgDispFunc();
//	void SubAutoMeasuredDataDisplay();
	void SubTimeDisplay();
	void CalcPanelCount();
//
	CPictureHolder m_pictureholder;

	// Generated message map functions
	//{{AFX_MSG(CAuto)
	afx_msg long OnDotNumDisp(WPARAM wParam, LPARAM lParam);
	afx_msg long OnInfoDisp(WPARAM wParam, LPARAM lParam);
	afx_msg long OnAmountDisp(WPARAM wParam, LPARAM lParam);
	afx_msg long OnTargetWeightMMGDisp(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnPaint();
	afx_msg void OnClickAutoLoad();
	afx_msg void OnClickAutoDrop();
	afx_msg void OnClickAutoUnload();
	afx_msg void OnClickAutoStep();
	afx_msg void OnClickAutoContinuous();
	afx_msg void OnClickAutoScrap();
	afx_msg void OnClickCmdReturn();
	afx_msg void OnClickCmdAutointerface();
	afx_msg void OnClickAutoMeasure();
	afx_msg void OnClickCmdMeasureddataDraw();
	afx_msg void OnClickAutoDummydrop();
	afx_msg void OnAdjustInAuto();
	afx_msg void OnClickCmdGlsCntClear();
	afx_msg void OnClickMsflexgridGlassProcess();
	afx_msg void OnClickOnOffLinemode();
	afx_msg void OnClickMsflexgridAutoWorkdata();
	afx_msg void OnClickAutoErrorHistory();
	afx_msg void OnClickMsflexgridAutoMeadata();
	afx_msg void OnClickGlsDataChange();
	afx_msg void OnClickGrease2();
	afx_msg void OnClickGlsDataRequest();
	afx_msg void OnClickLcRemain();
	afx_msg void OnDblClickCmdAutoTitle(LPDISPATCH Cancel);
	afx_msg void OnClickAlignView();
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnClickCmdWarmmingUp();
	afx_msg void OnClickRecipeInfo();
	afx_msg void OnClickCmdLoadSensor();
	afx_msg void OnClickCmdJobMove();
	afx_msg void OnClickCMDValcRun();
	afx_msg void OnClickAutoVision();
	afx_msg void OnClickCmdLogWrite();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTO_H__63F646CB_F011_4EE3_883B_79A3B2F5B4A4__INCLUDED_)

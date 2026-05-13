//{{AFX_INCLUDES()
#include "msflexgrid.h"
#include "labelcontrol.h"
#include "commandbutton.h"
//}}AFX_INCLUDES
#if !defined(AFX_ADJUST_H__FDDCCEF4_581A_42EE_9B3B_F33E8CD8EDAF__INCLUDED_)
#define AFX_ADJUST_H__FDDCCEF4_581A_42EE_9B3B_F33E8CD8EDAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Adjust.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAdjust dialog
//
class CAdjust : public CDialog
{
// Construction
public:
	void SubSectionCalibResult(int r, int c);
	void SelectLanguage();
	void SubFlexibleResult(int r, int c);
	BOOL m_bMeasureInterlock;
	void SubTimerDropCountDisp();
	void SubTimerBalanceModeDispFunc();
	CString m_str1,m_str2,m_str3,m_str4,m_str5,m_str6,m_str7,m_str8;
	CAdjust(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAdjust();

	//	func
	void SubDisplayRecipeName();
	void SubDataProcess(int,int);
	void SubDisplayAdjustingCondition(int);
	void SubLoopCountProcess();
	void SubVolCalibResult(int r, int c);

	//	variable..
	int m_nMeasureJob; // 1:정지 상태,adjust,measure... 
					   //2: Column Inotial Move시...
					   //3: Dummy Drop시   
	BOOL m_bRun;
	int m_nTasJob; //by shin//2009.08.25//TAS추가 관련...//

// Dialog Data
	//{{AFX_DATA(CAdjust)
	enum { IDD = IDD_ADJUST };
	CButton	m_ctrlVolCalculate;
	CButton	m_ctrlPumpInitial;
	CLabelControl	m_ctrlRecipeName;
	CLabelControl	m_ctrlLoopCount1;
	CLabelControl	m_ctrlLoopCount2;
	CLabelControl	m_ctrlLoopCount3;
	CLabelControl	m_ctrlLoopCount4;
	CLabelControl	m_ctrlLoopCount5;
	CLabelControl	m_ctrlLoopCount6;
	CLabelControl	m_ctrlLoopCount7;
	CLabelControl	m_ctrlLoopCount8;
	CLabelControl	m_ctrlLoopCount9;
	CLabelControl	m_ctrlLoopCount10;
	CLabelControl	m_ctrlLoopCount11;
	CLabelControl	m_ctrlLoopCount12;
	CCommandButton	m_ctrlDummyDrop;
	CMSFlexGrid	m_ctrlAdjustingCondition;
	CMSFlexGrid	m_ctrlMeasuredData;
	CCommandButton	m_ctrlAdjust;
	CCommandButton	m_ctrlMeasure;
	CLabelControl	m_ctrlMessage1;
	CLabelControl	m_ctrlMessage2;
	CLabelControl	m_ctrlMessage3;
	CLabelControl	m_ctrlMessage4;
	CCommandButton	m_ctrlReturn;	
	CLabelControl	m_ctrlMessageDummyDrop;
	CMSFlexGrid	m_ctrlVolCalibResult1;
	CMSFlexGrid	m_ctrlVolCalibResult2;
	CCommandButton	m_ctrlVolCalib;
	CCommandButton	m_ctrlBalModeSet;
	CMSFlexGrid	m_ctrlMeasuredGapData;
	CLabelControl	m_ctrlDummyCnt;
	CLabelControl	m_ctrlLoopCount13;
	CLabelControl	m_ctrlLoopCount14;
	CLabelControl	m_ctrlLoopCount15;
	CLabelControl	m_ctrlLoopCount16;
	CLabelControl	m_ctrlMessage5;
	CLabelControl	m_ctrlMessage6;
	CCommandButton	m_ctrlDropCount;
	CCommandButton	m_ctrlApdReport;
	CLabelControl	m_ctrlAdjustError;
	CLabelControl	m_ctrlManuMeasureCount;
	CLabelControl	m_ctrlMessage7;
	CLabelControl	m_ctrlMessage8;
	CLabelControl	m_ctrlDummyCycle;
	CCommandButton	m_ctrlTargetMode;
	CCommandButton	m_ctrlTitle;
	CCommandButton	m_ctrlTargetSet;
	CCommandButton	m_ctrlCalibApply;
	CCommandButton	m_ctrlInitialMode;
	CCommandButton	m_ctrlNOffset;
//	CMSFlexGrid	m_ctrlVolCalibResult3;
	CMSFlexGrid	m_ctrlMeasuredData2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdjust)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// 현재선택되어있는 data 영역을 구분하기 위한 변수..
	int m_nR, m_nC;

	//
	UINT m_nTimerCount;
	// 
	void SubTimerAdjustFunc();
	void SubTimerDummyDropFunc();
	void SubTimerInitialMoveFunc();
	//
	void SubTimerAdjustMeasurdDataDispFunc();
	void SubTimerAdjustMsgDispFunc();

//
	// Generated message map functions
	//{{AFX_MSG(CAdjust)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBtnPrev();
	afx_msg void OnBtnNext();
	afx_msg void OnBtnUp();
	afx_msg void OnBtnDown();
	afx_msg void OnBtnPumpInitial();
	afx_msg void OnBtnSeqInitial();
	afx_msg void OnClickCmdAdjust();
	afx_msg void OnClickCmdMeasure();
	afx_msg void OnClickCmdDummyDrop();
	afx_msg void OnClickCmdReturn();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnClickInitialVolcalib();
	afx_msg void OnClickMsflexgridVolcalibResult();
	afx_msg void OnClickMsflexgridVolcalibResult2();
	afx_msg void OnInitialVolcalculate();
	afx_msg void OnClickBalModeSet();
	afx_msg void OnClickDummyCnt();
	afx_msg void OnDblClickMsflexgridAdjust();
	afx_msg void OnClickCmdDropCounting();
	afx_msg void OnClickApdReport();
	afx_msg void OnClickAdjustError();
	afx_msg void OnClickMeasureCount();
	afx_msg void OnClickLabelRecipe();
	afx_msg void OnDblClickBalanceCal(LPDISPATCH Cancel);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnClickDummyCycle();
	afx_msg void OnClickTargetMode();
	afx_msg void OnClickTargetSet();
	afx_msg void OnDblClickCalibrationApply(LPDISPATCH Cancel);
	afx_msg void OnClickCmdTitle();
	afx_msg void OnClickInitialOnoff();
	afx_msg void OnClickNOffset();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void SubTimerLoadCellDisp();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADJUST_H__FDDCCEF4_581A_42EE_9B3B_F33E8CD8EDAF__INCLUDED_)

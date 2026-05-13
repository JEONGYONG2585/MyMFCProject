//{{AFX_INCLUDES()
#include "commandbutton.h"
#include "labelcontrol.h"
//}}AFX_INCLUDES
#if !defined(AFX_MANUAL_H__E7777BB0_65E5_49CA_AF96_E93976990F9A__INCLUDED_)
#define AFX_MANUAL_H__E7777BB0_65E5_49CA_AF96_E93976990F9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Manual.h : header file
//
#include "NormalMsg.h"
/////////////////////////////////////////////////////////////////////////////
// CManual dialog

class CManual : public CDialog
{
// Construction
public:
	void SelectLanguage();
	BOOL GetStageVac();
	BOOL m_bDoorUsing[2];
	CManual(CWnd* pParent = NULL);   // standard constructor
	virtual ~CManual();

// Dialog Data
	//{{AFX_DATA(CManual)
	enum { IDD = IDD_MANU };
	CCommandButton	m_ctrlStageVacOn;
	CCommandButton	m_ctrlStageVacOff;
	CLabelControl	m_ctrlRecipeName;
	CCommandButton	m_ctrlIonizerON;
	CCommandButton	m_ctrlIonizerOFF;
	CCommandButton	m_ctrlCDA1;
	CCommandButton	m_ctrlCDA2;
	CCommandButton	m_ctrlPinch1;
	CCommandButton	m_ctrlPinch2;
	CCommandButton	m_ctrlNozzleForword1;
	CCommandButton	m_ctrlNozzleForword2;
	CCommandButton	m_ctrlSideDoorStatus;
	CCommandButton	m_ctrlSpyHead;
	CCommandButton	m_ctrlStagePurge;
	CCommandButton	m_ctrlSenDoor0;
	CCommandButton	m_ctrlSenDoor1;
	CCommandButton	m_ctrlSenDoor2;
	CCommandButton	m_ctrlSenDoor3;
	CCommandButton	m_ctrlSenRobot;
	CCommandButton	m_ctrlSenGlass;
	CCommandButton	m_ctrlSenEMO1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManual)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_nTimerCount;
	int m_nMoveCode;

	BOOL m_bCDA1;
	BOOL m_bCDA2;

	BOOL m_bPinch1;
	BOOL m_bPinch2;

	BOOL m_bNozzleForword1;
	BOOL m_bNozzleForword2;

//
	CNormalMsg m_NormalMsg;

	// Generated message map functions
	//{{AFX_MSG(CManual)
	virtual void OnManuMove(int);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClickManuVacuumOn();
	afx_msg void OnClickManuVacuumOff();
	afx_msg void OnClickCmdIo();
	afx_msg void OnClickCmdReturn();
	afx_msg void OnClickManuMove();
	afx_msg void OnClickManuWarmUp();
	afx_msg void OnClickManuDrop();
	afx_msg void OnClickManuDischarge();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClickIonizerOn();
	afx_msg void OnClickIonizerOff();
	afx_msg void OnClickCda1();
	afx_msg void OnClickCda2();
	afx_msg void OnClickPinch1();
	afx_msg void OnClickPinch2();
	afx_msg void OnClickSupplyNozzle1();
	afx_msg void OnClickSupplyNozzle2();
	afx_msg void OnClickManuMeasurePos();
	afx_msg void OnClickSpyHead();
	afx_msg void OnDblClickLabelLcSupplyGroup(LPDISPATCH Cancel);
	afx_msg void OnClickManuPurgeOn();
	afx_msg void OnClickManuMarkAlign();
	afx_msg void OnClickManuVision();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANUAL_H__E7777BB0_65E5_49CA_AF96_E93976990F9A__INCLUDED_)

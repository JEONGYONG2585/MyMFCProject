//{{AFX_INCLUDES()
#include "commandbutton.h"
#include "msflexgrid.h"
//}}AFX_INCLUDES
#if !defined(AFX_MANUIO_H__CC449F7A_64A9_486E_B70C_080578965AD8__INCLUDED_)
#define AFX_MANUIO_H__CC449F7A_64A9_486E_B70C_080578965AD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ManuIO.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CManuIO dialog

class CManuIO : public CDialog
{
// Construction
public:
	void SelectLanguage();
	void IOView();
	void PressureView();
	double m_dPress[6];
	BOOL m_bPressureView;
	CManuIO(CWnd* pParent = NULL);   // standard constructor
	virtual ~CManuIO();

// Dialog Data
	//{{AFX_DATA(CManuIO)
	enum { IDD = IDD_MANU_IO };
	CCommandButton	m_ctrlBdSel1;
	CCommandButton	m_ctrlBdSel2;
	CCommandButton	m_ctrlBdSel3;
	CCommandButton	m_ctrlBdSel4;
	CMSFlexGrid	m_ctrlInput;
	CMSFlexGrid	m_ctrlOutput;
	CMSFlexGrid	m_ctrlMotorSensor;
	CCommandButton	m_LabelMotorSensor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManuIO)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
//
	int m_nBdSel; //현재 선택된 bd
	
	// m_strIoCaptionSet[bd_no];
	IO_CAPTION_SET m_structIOCaptionSet[4];   //  

	// IO , Motor Sensor 읽은 값을 저장하기 위해서..
	BOOL m_bInputStatus[64];   BOOL m_bInputStatusBK[64];
	BOOL m_bOutputStatus[64];  BOOL m_bOutputStatusBK[64];
	BOOL m_bAxisStatus[16][5]; BOOL m_bAxisStatusBK[16][5];   //  Limit+ , Limit- , Org, bLim,bOrg( by ckh 2008.12.16 센서 유무 추가함.)

	// func
	void SubOpenIOStrFile();
	void SubDisplayIOStr();
	void SubBdSelBtnColor();

//
	// Generated message map functions
	//{{AFX_MSG(CManuIO)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickCmdBoardSel1();
	afx_msg void OnClickCmdBoardSel2();
	afx_msg void OnClickCmdBoardSel3();
	afx_msg void OnClickCmdBoardSel4();
	afx_msg void OnClickMsflexgridOutput();
	afx_msg void OnClickCmdReturn();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClickCmdMotorSensorTitle();
	afx_msg void OnClickPrees();
	afx_msg void OnClickPvg();
	afx_msg void OnClickCmdPiezoStatus();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANUIO_H__CC449F7A_64A9_486E_B70C_080578965AD8__INCLUDED_)

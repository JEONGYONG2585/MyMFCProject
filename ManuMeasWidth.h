//{{AFX_INCLUDES()
#include "labelcontrol.h"
#include "commandbutton.h"
#include "msflexgrid.h"
#include "ligeledisplay.h"
//}}AFX_INCLUDES
#if !defined(AFX_MANUMEASWIDTH_H__668BCE2C_EC5A_4A6D_B4F8_DA8C90F36A37__INCLUDED_)
#define AFX_MANUMEASWIDTH_H__668BCE2C_EC5A_4A6D_B4F8_DA8C90F36A37__INCLUDED_

#include "ThreadData.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ManuMeasWidth.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CManuMeasWidth dialog

class CManuMeasWidth : public CDialog
{
// Construction
public:
	void SelectLanguage();
	int m_nLightVal[MAX_NOZZLE];
	void SetCamViewr(int nHeadCam);
	BOOL m_bButExect; // 버튼에 의해 정지하기 위함. 현재 사용안함. (by ckh 2009.01.09 )
	void SetDisplayMarkData(int nHeadNum);
	BOOL SetSelCam(int nHeadCam);
	void SetBut_HeadColor(int nHead,long lColor);
	BOOL m_bLamp;
	int m_nColumn;
//	BOOL m_bCheckMeas[WIDTH_INDEX][MAX_HEAD];
	BOOL CheckInterlock();
	BOOL m_bMeasPosition;
	void MoveCameraPosition(int nPointNo, int nColumnNo);
	void MoveCameraPosition2(int nPointNo);
	void SetMSFlexGrid();

	int m_nViewMode;
	int m_nSelect1, m_nSelect2;
	BOOL m_bProcess;
	double m_dXPointCam[MAX_NOZZLE];
	double m_dYPointCam[MAX_NOZZLE];
	int m_nHeadSelectNo;
	double m_dDistanceCamX[MAX_NOZZLE];
	double m_dDistanceCamY[MAX_NOZZLE];

	CFont m_fontCombo;
	CManuMeasWidth(CWnd* pParent = NULL);   // standard constructor
	virtual ~CManuMeasWidth();

// Dialog Data
	//{{AFX_DATA(CManuMeasWidth)
	enum { IDD = IDD_MANU_MEAS_POS };
	CButton	m_ctrlFirstChk;
	CSpinButtonCtrl	m_ctrlSpenLightLevel;
	CEdit	m_ctrlLightLevel;
	CLigEleDisplay	m_ctrlDisplay0;
	CLigEleDisplay	m_ctrlDisplay1;
	CCommandButton	m_ctrlHead1;
	CCommandButton	m_ctrlHead10;
	CCommandButton	m_ctrlHead11;
	CCommandButton	m_ctrlHead12;
	CCommandButton	m_ctrlHead13;
	CCommandButton	m_ctrlHead14;
	CCommandButton	m_ctrlHead15;
	CCommandButton	m_ctrlHead16;
	CCommandButton	m_ctrlHead2;
	CCommandButton	m_ctrlHead3;
	CCommandButton	m_ctrlHead4;
	CCommandButton	m_ctrlHead5;
	CCommandButton	m_ctrlHead6;
	CCommandButton	m_ctrlHead7;
	CCommandButton	m_ctrlHead8;
	CCommandButton	m_ctrlHead9;
	CCommandButton	m_ctrlJog;
	CCommandButton	m_ctrlMovePos1;
	CCommandButton	m_ctrlMovePos2;
	CLabelControl	m_ctrlCenterPosX;
	CLabelControl	m_ctrlCenterPosY;
	CLabelControl	m_ctrlDistX;
	CLabelControl	m_ctrlDistY;
	BOOL	m_bFirstChk;
	CCommandButton	m_ctrlPosChk;
	CCommandButton	m_ctrlCmdSave;
	CCommandButton	m_ctrlMarkSearch;
	CCommandButton	m_ctrlLCSearch;
	CLabelControl	m_ctrlLabName;
	CLabelControl	m_ctrlLabLightLevel;
	CLabelControl	m_ctrlLabLightBright;
	CCommandButton	m_ctrlAlignSearch;
	CCommandButton	m_ctrlTeachData;
	CCommandButton	m_ctrlTeachingAling;
	CCommandButton	m_ctrlPMLoad;
	CCommandButton	m_ctrlPMSave;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManuMeasWidth)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CManuMeasWidth)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickCmdPrev();
	afx_msg void OnClickCmdMeas();
	afx_msg void OnClickCmdManualJog();
	afx_msg void OnClickCmdHead1();
	afx_msg void OnClickCmdHead2();
	afx_msg void OnClickCmdHead3();
	afx_msg void OnClickCmdHead4();
	afx_msg void OnClickCmdHead5();
	afx_msg void OnClickCmdHead6();
	afx_msg void OnClickCmdHead7();
	afx_msg void OnClickCmdHead8();
	afx_msg void OnClickCmdHead9();
	afx_msg void OnClickCmdHead10();
	afx_msg void OnClickCmdHead11();
	afx_msg void OnClickCmdHead12();
	afx_msg void OnClickCmdHead13();
	afx_msg void OnClickCmdHead14();
	afx_msg void OnClickCmdHead15();
	afx_msg void OnClickCmdHead16();
	afx_msg void OnClickCmdSave();
	afx_msg void OnClickCmdBlob();
	afx_msg void OnClickCmdMovePos2();
	afx_msg void OnClickCmdMovePos1();
	afx_msg void OnClickCmdPoschk();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnChkFirstchk();
	afx_msg void OnClickCmdMarkSearch();
	afx_msg void OnClickCmdLcSearch();
	afx_msg void OnDeltaposSpinlightlevel(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickLabLightlevel();
	afx_msg void OnClickCmdTeachingdata();
	afx_msg void OnClickCmdPoschkResult();
	afx_msg void OnClickCmdAlignSearch();
	afx_msg void OnClickCmdTeachingAlignmark();
	afx_msg void OnClickCmdPmload();
	afx_msg void OnClickCmdPmsave();
	afx_msg void OnClickCmdMarkAlign();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANUMEASWIDTH_H__668BCE2C_EC5A_4A6D_B4F8_DA8C90F36A37__INCLUDED_)

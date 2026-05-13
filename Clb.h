//{{AFX_INCLUDES()
#include "msflexgrid.h"
#include "labelcontrol.h"
#include "commandbutton.h"
//}}AFX_INCLUDES
#if !defined(AFX_CLB_H__FF7B0FA5_7DFD_46E7_B44E_89EFBC9FB88A__INCLUDED_)
#define AFX_CLB_H__FF7B0FA5_7DFD_46E7_B44E_89EFBC9FB88A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Clb.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CClb dialog

class CClb : public CDialog
{
// Construction
public:
	void SelectLanguage();
	BOOL m_bLowDetect[MAX_NOZZLE];//2010.07.05 by tskim
	void RefreshData();
	int m_nClickCount;
	void HeadOffsetSave();
	double HeadOffset[12];
	BOOL HeadOffsetRead();
	void Doc2Val();
	BOOL m_bRemain[MAX_NOZZLE];
	BOOL m_bDropCount[MAX_NOZZLE];
	CClb(CWnd* pParent = NULL);   // standard constructor
	virtual ~CClb();
//
	//func
	void SubDisplayRecipeName();
	void SubClbDataSave();
	CButton* m_pDropCountSensor[MAX_NOZZLE];

// Dialog Data
	//{{AFX_DATA(CClb)
	enum { IDD = IDD_CLB };
	CMSFlexGrid	m_ctrlOffset;
	CMSFlexGrid	m_ctrlCommon;
	CLabelControl	m_ctrlRecipeName;
	CMSFlexGrid	m_ctrlLCLotNo;
	CLabelControl	m_ctrlPumpNoLabel;
	CLabelControl	m_ctrlLCDetectedCountSet;
	int		m_nCheckType;
	CMSFlexGrid	m_ctrlOffset2;
	CMSFlexGrid	m_crtlDropCntErrorRange;
	CMSFlexGrid	m_ctrlPumpNo;
	CMSFlexGrid	m_ctrlLcSupplyBal;
	CLabelControl	m_ctrlLcBal1;
	CLabelControl	m_ctrlLcBal2;
	CLabelControl	m_ctrlLcLotName;
	CCommandButton	m_ctrlHeadDropInf;
	CCommandButton	m_ctrlRunDetectMode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClb)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// io
	// Generated message map functions
	//{{AFX_MSG(CClb)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClickMsflexgridOffset();
	afx_msg void OnClickMsflexgridCommon();
	afx_msg void OnClickCmdReturn();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnClickCmdRegist();
	afx_msg void OnClickMsflexgridLcLotNum();
	afx_msg void OnClickLcCountSet();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRADIODoor();
	afx_msg void OnRADIORemain();
	afx_msg void OnClickMsflexgridOffset2();
	afx_msg void OnClickInputBarcode();
	afx_msg void OnClickMsflexgridDropCountErrorRange();
	afx_msg void OnClickMsflexgridPumpNum();
	afx_msg void OnClickCmdInitialShot();
	afx_msg void OnClickCmdPumpInitial();
	afx_msg void OnClickMsflexgridLcSupply();
	afx_msg void OnClickLabelHead1();
	afx_msg void OnClickLabelHead2();
	afx_msg void OnClickCmdGrease();
	afx_msg void OnClickCmdDropCountingRange();
	afx_msg void OnClickCmdModuleInfo();
	afx_msg void OnClickCmdVisionDropCount();
	afx_msg void OnClickCmdDropcountRealtime();
	afx_msg void OnClickCmdMaterialChan();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLB_H__FF7B0FA5_7DFD_46E7_B44E_89EFBC9FB88A__INCLUDED_)

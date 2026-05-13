//{{AFX_INCLUDES()
#include "msflexgrid.h"
//}}AFX_INCLUDES
#if !defined(AFX_SETTINGASSEMBLE_H__ACD9C490_17C5_4B1A_9FCB_1C25045983D8__INCLUDED_)
#define AFX_SETTINGASSEMBLE_H__ACD9C490_17C5_4B1A_9FCB_1C25045983D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingAssemble.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSettingAssemble dialog

class CSettingAssemble : public CDialog
{
// Construction
public:
	void SetSupplyDegasCondition();
	void SetNozzleCleanGrid();
	void SetPumpLimitCheck();
	CSettingAssemble(CWnd* pParent = NULL);   // standard constructor

	int iAxis[MAX_NOZZLE];
	double dAbsPos[MAX_NOZZLE];
	double dIncPos[MAX_NOZZLE];
	double dVelocity[MAX_NOZZLE];
	WORD wNoOfAxis;
// Dialog Data
	//{{AFX_DATA(CSettingAssemble)
	enum { IDD = IDD_SETTING };
	CMSFlexGrid	m_ctrlSettingGrid;
	CMSFlexGrid	m_ctrlNozzleCleanGrid;
	CMSFlexGrid	m_GridCondition;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettingAssemble)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void SetSettingMSFlexGrid();

	int m_nLoopCount;

	// Generated message map functions
	//{{AFX_MSG(CSettingAssemble)
	afx_msg void OnClickCmdReturn();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickMsflexgridSetting();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClickCmdPumpPositionSave();
	afx_msg void OnClickGridCondition();
	afx_msg void OnClickMsflexgridNozzleClean();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTINGASSEMBLE_H__ACD9C490_17C5_4B1A_9FCB_1C25045983D8__INCLUDED_)

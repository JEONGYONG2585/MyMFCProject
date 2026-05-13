//{{AFX_INCLUDES()
#include "labelcontrol.h"
//}}AFX_INCLUDES
#if !defined(AFX_SAFETYMAP_H__74E957F0_162B_4A1B_99DF_D3DFF3C2B662__INCLUDED_)
#define AFX_SAFETYMAP_H__74E957F0_162B_4A1B_99DF_D3DFF3C2B662__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SafetyMap.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSafetyMap dialog

class CSafetyMap : public CDialog
{
// Construction
public:
	CSafetyMap(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSafetyMap)
	enum { IDD = IDD_SAFETYMAP };
	CLabelControl	m_ctrlRobotArm;
	CLabelControl	m_ctrlMaintDoor2;
	CLabelControl	m_ctrlMaintDoor1;
	CLabelControl	m_ctrlGlass;
//	CLabelControl	m_ctrlEms6;
	CLabelControl	m_ctrlEms5;
	CLabelControl	m_ctrlEms4;
	CLabelControl	m_ctrlEms3;
	CLabelControl	m_ctrlEms2;
	CLabelControl	m_ctrlEms1;
	CLabelControl	m_ctrlEms0;
	CLabelControl	m_ctrlEmo;
	CLabelControl	m_ctrlDoor2;
	CLabelControl	m_ctrlDoor3;
	CLabelControl	m_ctrlDoor1;
	CLabelControl	m_ctrlArea1;
	CLabelControl	m_ctrlArea2;
	CLabelControl	m_ctrlArea3;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSafetyMap)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSafetyMap)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAFETYMAP_H__74E957F0_162B_4A1B_99DF_D3DFF3C2B662__INCLUDED_)

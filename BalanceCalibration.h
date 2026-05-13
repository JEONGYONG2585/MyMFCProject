//{{AFX_INCLUDES()
#include "msflexgrid.h"
#include "commandbutton.h"
#include "labelcontrol.h"
//}}AFX_INCLUDES
#if !defined(AFX_BALANCECALIBRATION_H__BD53732D_4A0E_47D1_9369_826CB2855CC2__INCLUDED_)
#define AFX_BALANCECALIBRATION_H__BD53732D_4A0E_47D1_9369_826CB2855CC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BalanceCalibration.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBalanceCalibration dialog

class CBalanceCalibration : public CDialog
{
// Construction
public:
	void SubTimerCal();
	CBalanceCalibration(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBalanceCalibration)
	enum { IDD = IDD_BALANCE_CAL };
	CButton	m_ctrlOK;
	CMSFlexGrid	m_GridCal;
	CCommandButton	m_ctrlStart;
	CLabelControl	m_ctrlMsg;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBalanceCalibration)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	UINT m_nTimerCount;

	// Generated message map functions
	//{{AFX_MSG(CBalanceCalibration)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnClickGridCal();
	afx_msg void OnClickCalStart();
	afx_msg void OnClickMsg();
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BALANCECALIBRATION_H__BD53732D_4A0E_47D1_9369_826CB2855CC2__INCLUDED_)

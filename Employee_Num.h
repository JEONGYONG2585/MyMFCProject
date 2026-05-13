//{{AFX_INCLUDES()
#include "labelcontrol.h"
//}}AFX_INCLUDES
#if !defined(AFX_EMPLOYEE_NUM_H__B2E9BC5E_50CB_4A7B_B79D_3F0D56911455__INCLUDED_)
#define AFX_EMPLOYEE_NUM_H__B2E9BC5E_50CB_4A7B_B79D_3F0D56911455__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Employee_Num.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEmployee_Num dialog

class CEmployee_Num : public CDialog
{
// Construction
public:
	void SelectLanguage();
	BOOL OnInitDialog();
	CEmployee_Num(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEmployee_Num)
	enum { IDD = IDD_EMPLOYEE_NUM };
	CLabelControl	m_ctrlEmploy_Num;
	CLabelControl	m_ctrlTeam_Num;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEmployee_Num)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEmployee_Num)
	afx_msg void OnClickSendNum();
	afx_msg void OnClickLABELEmployeeNum();
	afx_msg void OnClickLABELTeamNum();
	afx_msg void OnClickCalcelBtn();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EMPLOYEE_NUM_H__B2E9BC5E_50CB_4A7B_B79D_3F0D56911455__INCLUDED_)

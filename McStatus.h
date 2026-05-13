//{{AFX_INCLUDES()
#include "msflexgrid.h"
//}}AFX_INCLUDES
#if !defined(AFX_MCSTATUS_H__B858CFAA_3CEF_4F4C_AE0F_FA7720A1F201__INCLUDED_)
#define AFX_MCSTATUS_H__B858CFAA_3CEF_4F4C_AE0F_FA7720A1F201__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// McStatus.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMcStatus dialog

class CMcStatus : public CDialog
{
// Construction
public:
	void CloseWindow();
	void InitMcStatus();
	CMcStatus(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMcStatus)
	enum { IDD = IDD_MC_STATUS };
	CMSFlexGrid	m_GridMcStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMcStatus)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();
	virtual BOOL Create(CWnd* pParentWnd);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMcStatus)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnOK();
	afx_msg void OnDblClickGrdiMcstatus();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MCSTATUS_H__B858CFAA_3CEF_4F4C_AE0F_FA7720A1F201__INCLUDED_)

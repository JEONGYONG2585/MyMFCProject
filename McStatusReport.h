//{{AFX_INCLUDES()
#include "labelcontrol.h"
#include "commandbutton.h"
//}}AFX_INCLUDES
#if !defined(AFX_MCSTATUSREPORT_H__DFB67027_405C_49E3_AE2D_6C8C6F9A2067__INCLUDED_)
#define AFX_MCSTATUSREPORT_H__DFB67027_405C_49E3_AE2D_6C8C6F9A2067__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// McStatusReport.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMcStatusReport dialog

class CMcStatusReport : public CDialog
{
// Construction
public:
	void SelectLanguage();
	BOOL ReadData();
	CMcStatusReport(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMcStatusReport)
	enum { IDD = IDD_MC_STATUS_REPORT };
	int		m_nStatusCode;
	CLabelControl	m_ctrlLabModel;
	CLabelControl	m_ctrlLabClean;
	CLabelControl	m_ctrlLabMaterial;
	CCommandButton	m_ctrlCmdSave;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMcStatusReport)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMcStatusReport)
	afx_msg void OnMcSel1();
	afx_msg void OnAbort();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickLabmodel();
	afx_msg void OnClickLabmaterial();
	afx_msg void OnClickLabclean();
	afx_msg void OnClickCmdSave();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MCSTATUSREPORT_H__DFB67027_405C_49E3_AE2D_6C8C6F9A2067__INCLUDED_)

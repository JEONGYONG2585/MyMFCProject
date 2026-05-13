//{{AFX_INCLUDES()
#include "msflexgrid.h"
//}}AFX_INCLUDES
#if !defined(AFX_MODULEDBREGISTRY_H__BB77FF97_0712_483D_9D74_2CDEDA2203E7__INCLUDED_)
#define AFX_MODULEDBREGISTRY_H__BB77FF97_0712_483D_9D74_2CDEDA2203E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModuleDBRegistry.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModuleDBRegistry dialog

class CModuleDBRegistry : public CDialog
{
// Construction
public:
	CString strModuleID;
	int nAmplitudeOffset_DB;
	int nNegOffsetOffset_DB;
	int  nFrequecyOffset_DB;
	double   nDutyOffset_DB;
	int nFirst_VolOffset_DB;
	int nFallingEdgeOffset_DB;
	void SetGrid_Initial();
	CModuleDBRegistry(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModuleDBRegistry)
	enum { IDD = IDD_MODULE_DB_REGISTRY };
	CMSFlexGrid	m_GridDBRegistry;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModuleDBRegistry)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModuleDBRegistry)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnClickGridDbRegistry();
	afx_msg void OnClickCmdReturn();
	afx_msg void OnClickCmdCreation();
	afx_msg void OnClickCmdRegistry();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODULEDBREGISTRY_H__BB77FF97_0712_483D_9D74_2CDEDA2203E7__INCLUDED_)

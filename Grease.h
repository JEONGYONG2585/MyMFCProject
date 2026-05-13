//{{AFX_INCLUDES()
#include "msflexgrid.h"
#include "commandbutton.h"
//}}AFX_INCLUDES
#if !defined(AFX_GREASE_H__8F8D45EC_D02F_4389_B3D3_E73F00FF8EC6__INCLUDED_)
#define AFX_GREASE_H__8F8D45EC_D02F_4389_B3D3_E73F00FF8EC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Grease.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGrease dialog

class CGrease : public CDialog
{
// Construction
public:
	void SelectLanguage();
	CGrease(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGrease)
	enum { IDD = IDD_GREASE };
	CMSFlexGrid	m_ctrlGrease;
	CCommandButton	m_ctrlGreaseSave;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGrease)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGrease)
	afx_msg void OnClickSave();
	afx_msg void OnClickOk();
	afx_msg void OnClickMsflexgridGrease();
	virtual BOOL OnInitDialog();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GREASE_H__8F8D45EC_D02F_4389_B3D3_E73F00FF8EC6__INCLUDED_)

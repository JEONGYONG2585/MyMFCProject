//{{AFX_INCLUDES()
#include "msflexgrid.h"
//}}AFX_INCLUDES
#if !defined(AFX_TARGET_H__FC2BAADA_A900_41C9_B69A_6E47975A0D9B__INCLUDED_)
#define AFX_TARGET_H__FC2BAADA_A900_41C9_B69A_6E47975A0D9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Target.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTarget dialog

class CTarget : public CDialog
{
// Construction
public:
	int nSetRow;
	void SetGrid();
	CTarget(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTarget)
	enum { IDD = IDD_TARGET };
	CMSFlexGrid	m_GridTarget;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTarget)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTarget)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickGridTarget();
	virtual void OnOK();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TARGET_H__FC2BAADA_A900_41C9_B69A_6E47975A0D9B__INCLUDED_)

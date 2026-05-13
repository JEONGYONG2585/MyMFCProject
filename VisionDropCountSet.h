//{{AFX_INCLUDES()
#include "msflexgrid.h"
//}}AFX_INCLUDES
#if !defined(AFX_VISIONDROPCOUNTSET_H__F8DCAAE9_11C9_4148_98D2_3E81E19BB2EA__INCLUDED_)
#define AFX_VISIONDROPCOUNTSET_H__F8DCAAE9_11C9_4148_98D2_3E81E19BB2EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VisionDropCountSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVisionDropCountSet dialog

class CVisionDropCountSet : public CDialog
{
// Construction
public:
	void Set_GridInfo();
	CVisionDropCountSet(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVisionDropCountSet)
	enum { IDD = IDD_VISION_DROP_COUNT };
	CMSFlexGrid	m_GridVisionRange;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVisionDropCountSet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVisionDropCountSet)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickGridRange();
	afx_msg void OnClickCmdSave();
	afx_msg void OnClickCmdReturn();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VISIONDROPCOUNTSET_H__F8DCAAE9_11C9_4148_98D2_3E81E19BB2EA__INCLUDED_)

//{{AFX_INCLUDES()
#include "msflexgrid.h"
//}}AFX_INCLUDES
#if !defined(AFX_TEACHINGPOSCHK_H__D695DBC2_875E_46D7_BB49_15BE13ED797D__INCLUDED_)
#define AFX_TEACHINGPOSCHK_H__D695DBC2_875E_46D7_BB49_15BE13ED797D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TeachingPosChk.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTeachingPosChk dialog

class CTeachingPosChk : public CDialog
{
// Construction
public:
	BOOL m_bPosChk;
	void SetGridData();
	CTeachingPosChk(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTeachingPosChk)
	enum { IDD = IDD_TEACHING_POSCHK };
	CMSFlexGrid	m_ctrlGridTeaching;
	CLabelControl	m_ctrlLabTitle;
	CCommandButton	m_ctrlTeaching;
	CCommandButton	m_ctrlDataSave;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTeachingPosChk)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTeachingPosChk)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickCmdDatasave();
	afx_msg void OnClickGridTeachingPoschk();
	afx_msg void OnClickCmdTeaching();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEACHINGPOSCHK_H__D695DBC2_875E_46D7_BB49_15BE13ED797D__INCLUDED_)

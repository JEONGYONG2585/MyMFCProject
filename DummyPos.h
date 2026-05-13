//{{AFX_INCLUDES()
#include "msflexgrid.h"
//}}AFX_INCLUDES
#if !defined(AFX_DUMMYPOS_H__36E6A020_5401_45DA_A062_30310A3E0628__INCLUDED_)
#define AFX_DUMMYPOS_H__36E6A020_5401_45DA_A062_30310A3E0628__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DummyPos.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDummyPos dialog

class CDummyPos : public CDialog
{
// Construction
public:
	void SetSettingMSFlexGrid();
	CDummyPos(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDummyPos)
	enum { IDD = IDD_TEACH2 };
	CMSFlexGrid	m_ctrlDummyTeach;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDummyPos)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDummyPos)
	afx_msg void OnClickCmdSaveData();
	afx_msg void OnClickCmdReturn();
	afx_msg void OnClickMsflexgridDummyTeach();
	virtual BOOL OnInitDialog();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DUMMYPOS_H__36E6A020_5401_45DA_A062_30310A3E0628__INCLUDED_)

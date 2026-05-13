//{{AFX_INCLUDES()
#include "msflexgrid.h"
//}}AFX_INCLUDES
#if !defined(AFX_VISION_RESULT_H__128931BC_B19F_4C16_966D_5FFED658CBA5__INCLUDED_)
#define AFX_VISION_RESULT_H__128931BC_B19F_4C16_966D_5FFED658CBA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "N_Vision.h"
// Vision_Result.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVision_Result dialog

class CVision_Result : public CDialog
{
// Construction
public:
	int m_nMisStart[MAX_CELL];
	int m_nOverStart[MAX_CELL];
	int m_nMisCount[MAX_CELL];
	int m_nOverCount[MAX_CELL];
	void OnGrid(int nRow, BOOL *bTitle);
	CVision_Result(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVision_Result)
	enum { IDD = IDD_VISION_RESULT };
	CMSFlexGrid	m_gridResult;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVision_Result)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVision_Result)
	afx_msg void OnClickCmdReturn();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickCmdCheck();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VISION_RESULT_H__128931BC_B19F_4C16_966D_5FFED658CBA5__INCLUDED_)

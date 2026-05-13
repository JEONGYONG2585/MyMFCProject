//{{AFX_INCLUDES()
#include "msflexgrid.h"
#include "labelcontrol.h"
#include "commandbutton.h"
//}}AFX_INCLUDES
#if !defined(AFX_REPAIREDIT_H__D771866E_9AB6_496F_9E7E_F625C663051E__INCLUDED_)
#define AFX_REPAIREDIT_H__D771866E_9AB6_496F_9E7E_F625C663051E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RepairEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRepairEdit dialog
#include "PatternWnd.h"

#define ID_PATTERN_NO_WND 21002

class CRepairEdit : public CDialog
{
// Construction
public:
	CArray<DROP_DATA, DROP_DATA> *m_arDropData_Repair;
	PATTERN_DATA m_structPatternData_Repair[MAX_PATTERN];
	DROP_DATA m_structDropData_Repair;
	void SetGrid(int nPatternNo);
	int m_nMinPoint;
	int m_nMaxPoint;
	int m_nMaxVer;
	int m_nMaxHor;
	void SetScrollBar();
	int m_nVPos;
	int m_nHPos;
	void SubDrawPattern();
	CRepairEdit(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRepairEdit();
	CButton* m_pHead[MAX_NOZZLE];
	int m_nCurrentPatternNo;
//	PATTERN_DATA m_structPatternData;
	CPatternWnd *m_pPatternWnd;
// Dialog Data
	//{{AFX_DATA(CRepairEdit)
	enum { IDD = IDD_REPAIR_EDIT };
	CScrollBar	m_ctlScrollVer;
	CScrollBar	m_ctlScrollHor;
	CButton	m_staticScreen;
	CLabelControl	m_ctrlCurPatNo;
	CMSFlexGrid	m_ctrlDropPointRange;
	CCommandButton	m_ctrlTextDisp;
	CMSFlexGrid	m_GridPoint;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRepairEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
//	CPatternWnd *m_pPatternWnd;
	// Generated message map functions
	//{{AFX_MSG(CRepairEdit)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioZoom1();
	afx_msg void OnRadioZoom2();
	afx_msg void OnRadioZoom4();
	afx_msg void OnRadioZoom7();
	afx_msg void OnRadioZoom10();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHead1Chk();
	afx_msg void OnClickCmdReturn();
	afx_msg void OnClickAllReset();
	afx_msg void OnClickAllSet();
	afx_msg void OnClickPointRange();
	afx_msg void OnClickCmdDropNumDisp();
	afx_msg void OnClickDropRange();
	afx_msg void OnClickPartConv();
	afx_msg void OnClickGridPoint();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPAIREDIT_H__D771866E_9AB6_496F_9E7E_F625C663051E__INCLUDED_)

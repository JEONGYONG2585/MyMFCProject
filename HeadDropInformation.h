//{{AFX_INCLUDES()
#include "msflexgrid.h"
#include "labelcontrol.h"
//}}AFX_INCLUDES
#if !defined(AFX_HEADDROPINFORMATION_H__913829AA_1E22_4D79_BBE2_D4A0F41D3B7B__INCLUDED_)
#define AFX_HEADDROPINFORMATION_H__913829AA_1E22_4D79_BBE2_D4A0F41D3B7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HeadDropInformation.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHeadDropInformation dialog

class CHeadDropInformation : public CDialog
{
// Construction
public:
	void SelectLanguage();
	double	m_dLCDiff[2];
	BOOL m_bLowDetect[MAX_NOZZLE];
	BOOL m_bRemain[MAX_NOZZLE];
	BOOL m_bDropCount[MAX_NOZZLE];
	void InforDisplay();
	CHeadDropInformation(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHeadDropInformation)
	enum { IDD = IDD_HEADDROP_INF };
	CMSFlexGrid	m_GridHeadSensor;
	CMSFlexGrid	m_GridDropCntRange;
	CMSFlexGrid	m_GridLcAmount;
	CLabelControl	m_ctrlLcCountSet;
	CMSFlexGrid	m_GridAmountDiff;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHeadDropInformation)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHeadDropInformation)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickGridHeadsensor();
	afx_msg void OnClickGridDropcntRange();
	afx_msg void OnClickGridLcAmount();
	afx_msg void OnClickSave();
	afx_msg void OnClickLcCountSet();
	afx_msg void OnClickGridAmountdiff();
	afx_msg void OnClickReturn();
	afx_msg void OnPaint();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HEADDROPINFORMATION_H__913829AA_1E22_4D79_BBE2_D4A0F41D3B7B__INCLUDED_)

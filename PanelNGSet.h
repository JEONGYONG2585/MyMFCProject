//{{AFX_INCLUDES()
#include "labelcontrol.h"
//}}AFX_INCLUDES
#if !defined(AFX_PANELNGSET_H__BEFC6B55_CEB8_4084_B601_D35CE02DF447__INCLUDED_)
#define AFX_PANELNGSET_H__BEFC6B55_CEB8_4084_B601_D35CE02DF447__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PanelNGSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPanelNGSet dialog

class CPanelNGSet : public CDialog
{
// Construction
public:
	int m_nValue;
	CString m_strCFInform;
	CString m_strTFTInform;

	CPanelNGSet(CWnd* pParent = NULL);   // standard constructor
	CPanelNGSet(CString strTFT, CString strCF, CWnd* pParent = NULL);   // standard constructor
	CPanelNGSet(CString strTFT, CString strCF, int Value, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPanelNGSet)
	enum { IDD = IDD_PANEL_NG_SET };
	CLabelControl	m_ctrlTFTInform;
	CLabelControl	m_ctrlCFInform;
	int		m_nDropJudge;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPanelNGSet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPanelNGSet)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRadioOn();
	afx_msg void OnRadioOff();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PANELNGSET_H__BEFC6B55_CEB8_4084_B601_D35CE02DF447__INCLUDED_)

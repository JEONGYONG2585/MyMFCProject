//{{AFX_INCLUDES()
#include "labelcontrol.h"
//}}AFX_INCLUDES
#if !defined(AFX_MODULEBARTEACHING_H__E548161B_35BE_459E_ABD5_B08B8FD14E6D__INCLUDED_)
#define AFX_MODULEBARTEACHING_H__E548161B_35BE_459E_ABD5_B08B8FD14E6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModuleBarTeaching.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModuleBarTeaching dialog

class CModuleBarTeaching : public CDialog
{
// Construction
public:
	BOOL OnInitDialog();
	CModuleBarTeaching(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModuleBarTeaching)
	enum { IDD = IDD_MODULE_DB_BAR_TEACH };
	CLabelControl	m_ctrlmodulehead1teach;
	CLabelControl	m_ctrlmodulehead2teach;
	CLabelControl	m_ctrlmodulehead10teach;
	CLabelControl	m_ctrlmodulehead11teach;
	CLabelControl	m_ctrlmodulehead12teach;
	CLabelControl	m_ctrlmodulehead13teach;
	CLabelControl	m_ctrlmodulehead14teach;
	CLabelControl	m_ctrlmodulehead15teach;
	CLabelControl	m_ctrlmodulehead16teach;
	CLabelControl	m_ctrlmodulecolteach;
	CLabelControl	m_ctrlmodulehead3teach;
	CLabelControl	m_ctrlmodulehead4teach;
	CLabelControl	m_ctrlmodulehead5teach;
	CLabelControl	m_ctrlmodulehead6teach;
	CLabelControl	m_ctrlmodulehead7teach;
	CLabelControl	m_ctrlmodulehead8teach;
	CLabelControl	m_ctrlmodulehead9teach;
	CLabelControl	m_ctrlmoduleoffsetteach;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModuleBarTeaching)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModuleBarTeaching)
	afx_msg void OnClickLabelModuleHead1();
	afx_msg void OnClickLabelModuleHead10();
	afx_msg void OnClickLabelModuleHead11();
	afx_msg void OnClickLabelModuleHead12();
	afx_msg void OnClickLabelModuleHead13();
	afx_msg void OnClickLabelModuleHead14();
	afx_msg void OnClickLabelModuleHead15();
	afx_msg void OnClickLabelModuleHead16();
	afx_msg void OnClickLabelModuleHead2();
	afx_msg void OnClickLabelModuleHead3();
	afx_msg void OnClickLabelModuleHead4();
	afx_msg void OnClickLabelModuleHead5();
	afx_msg void OnClickLabelModuleHead6();
	afx_msg void OnClickLabelModuleHead7();
	afx_msg void OnClickLabelModuleHead8();
	afx_msg void OnClickLabelModuleHead9();
	afx_msg void OnClickLabelModuleOffset();
	afx_msg void OnClickSAVEModuleTeach();
	afx_msg void OnClickCalcelBtn();
	afx_msg void OnClickLabelModuleCol();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODULEBARTEACHING_H__E548161B_35BE_459E_ABD5_B08B8FD14E6D__INCLUDED_)

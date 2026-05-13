//{{AFX_INCLUDES()
#include "labelcontrol.h"
//}}AFX_INCLUDES
#if !defined(AFX_BALMODESELECT_H__D7ED3D3D_9861_4D41_85DD_9F9B318B5180__INCLUDED_)
#define AFX_BALMODESELECT_H__D7ED3D3D_9861_4D41_85DD_9F9B318B5180__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BalModeSelect.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBalModeSelect dialog

class CBalModeSelect : public CDialog
{
// Construction
public:
	void SelectLanguage();
	CBalModeSelect(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBalModeSelect();
// Dialog Data
	//{{AFX_DATA(CBalModeSelect)
	enum { IDD = IDD_BAL_MODE };
	CButton	m_ctrlUseBal8;
	CButton	m_ctrlUseBal7;
	CButton	m_ctrlUseBal6;
	CButton	m_ctrlUseBal5;
	CButton	m_ctrlUseBal4;
	CButton	m_ctrlUseBal3;
	CButton	m_ctrlUseBal2;
	CButton	m_ctrlUseBal1;
	CButton	m_ctrlBalRifLevel1;
	CButton	m_ctrlBalRifLevel2;
	CButton	m_ctrlBalRifLevel3;
	int		m_nBalMode;
	int		m_nBalRifLevel;
	int		m_nPrevBalMode;
	int		m_nBal1_Type;
	int		m_nBal2_Type;
	int		m_nBal3_Type;
	int		m_nBal4_Type;
	int		m_nBal5_Type;
	int		m_nBal6_Type;
	int		m_nBal7_Type;
	int		m_nBal8_Type;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBalModeSelect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBalModeSelect)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickExe();
	afx_msg void OnClickCancel();
	afx_msg void OnRadioBalNormal();
	afx_msg void OnRadioAbnormal();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BALMODESELECT_H__D7ED3D3D_9861_4D41_85DD_9F9B318B5180__INCLUDED_)

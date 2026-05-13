//{{AFX_INCLUDES()
#include "labelcontrol.h"
#include "commandbutton.h"
//}}AFX_INCLUDES
#if !defined(AFX_GLASSDATACHANGE_H__390E39FE_E074_4BB8_AE09_A25D4489D0D6__INCLUDED_)
#define AFX_GLASSDATACHANGE_H__390E39FE_E074_4BB8_AE09_A25D4489D0D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GlassDataChange.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGlassDataChange dialog

class CGlassDataChange : public CDialog
{
// Construction
public:
	void SelectLanguage();
	CGlassDataChange(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGlassDataChange)
	enum { IDD = IDD_GLS_DATA };
	CLabelControl	m_labelHostRecipeNo;
	CLabelControl	m_labelLotNo;
	CLabelControl	m_labelSlotNo;
	CLabelControl	m_labelCode;
	CLabelControl	m_labelGlsID;
	int m_nSelNo;
	CCommandButton	m_ctrlSel1;
	CCommandButton	m_ctrlSel2;
	CLabelControl	m_labelGlsCode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGlassDataChange)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGlassDataChange)
	afx_msg void OnClickReciepe();
	afx_msg void OnClickLot();
	afx_msg void OnClickSlot();
	afx_msg void OnClickCode();
	afx_msg void OnClickId();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickOk();
	afx_msg void OnClickCancel();
	afx_msg void OnClickSel1();
	afx_msg void OnClickSel2();
	afx_msg void OnClickGlsCode();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLASSDATACHANGE_H__390E39FE_E074_4BB8_AE09_A25D4489D0D6__INCLUDED_)

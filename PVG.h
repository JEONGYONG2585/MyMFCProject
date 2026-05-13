//{{AFX_INCLUDES()
#include "labelcontrol.h"
//#include "mdccombo.h"
//}}AFX_INCLUDES
#if !defined(AFX_PVG_H__7A244416_E412_4C2D_804E_D073FC255A29__INCLUDED_)
#define AFX_PVG_H__7A244416_E412_4C2D_804E_D073FC255A29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PVG.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPVG dialog

class CPVG : public CDialog
{
// Construction
public:
	CString m_strTitle;
	BOOL m_bKAxis_M;
	BOOL m_bKAxis_P;
	BOOL m_bSAxis_M;
	BOOL m_bSAxis_P;
	BOOL m_bGAxis_C;
	BOOL m_bGAxis_O;
	BOOL m_bPAxis_M;
	BOOL m_bPAxis_P;
	int m_nTabSelectOld;
	int m_nHead;
	CFont *pFont;
	CBitmap m_ctrlBitmap_GC;
	CBitmap m_ctrlBitmap_GO;
	CBitmap m_ctrlBitmap_M;
	CBitmap m_ctrlBitmap_P;
	void LabelControl(int nSel);
	void ColStatus();
	CBitmap m_ctrlBitmap2;
	CBitmap m_ctrlBitmap;
	void Head2Status();
	void Head1Status();
	void SelectView(int nSel);
	int m_nTabSelect;
	CPVG(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPVG)
	enum { IDD = IDD_DIALOG_PVG };
	CComboBox	m_comboHead;
	CStatic	m_static_G_C;
	CStatic	m_static_G_O;
	CStatic	m_static_M;
	CStatic	m_static_P;
	CStatic	m_static_bitmap2;
	CStatic	m_static_bitmap;
	CTabCtrl	m_ctrlTabPVG;
	CLabelControl	m_ctrlLabel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPVG)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPVG)
	afx_msg void OnSelchangeTabPvg(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnChangeComboboxHead();
	afx_msg void OnSelchangeComboHead();
	virtual void OnOK();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PVG_H__7A244416_E412_4C2D_804E_D073FC255A29__INCLUDED_)

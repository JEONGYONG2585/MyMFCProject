//{{AFX_INCLUDES()
#include "msflexgrid.h"
#include "labelcontrol.h"
#include "commandbutton.h"
#include "Clb.h"	// Added by ClassView
//}}AFX_INCLUDES
#if !defined(AFX_BARCODE_H__207A02E6_5125_489E_951C_6DF3F542187C__INCLUDED_)
#define AFX_BARCODE_H__207A02E6_5125_489E_951C_6DF3F542187C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Barcode.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBarcode dialog

class CBarcode : public CDialog
{
// Construction
public:
	void SelectLanguage();
	BOOL m_bSelected_Canister[2];
	BOOL m_bSeleted_Head[MAX_NOZZLE];
	BOOL OnCompare_BarValue();
	CString m_strMessage;
	int nBarcodeStatus;
	CBarcode(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBarcode)
	enum { IDD = IDD_BARCODE };
	CEdit	m_ctrlEditType;
	CMSFlexGrid	m_ctrlLcType;
	BOOL	m_bBarEnable;
	CString	m_strBarcode;
	CLabelControl	m_ctrlLcTypeName;
	CLabelControl	m_ctrlExplain;
	BOOL	m_bBarAllSelect;
	CMSFlexGrid	m_ctrlCanisterLcType;
	CLabelControl	m_ctrlCanisterTypeName;
	int		m_nBarcode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBarcode)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBarcode)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickOk();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnClickBarcodeInput();
	afx_msg void OnClickBarcodeStop();
	afx_msg void OnClickMsflexgridBarcode();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClickSelectAuto();
	afx_msg void OnDblClickMsflexgridBarcode();
	afx_msg void OnClickExamine();
	afx_msg void OnClickMsflexgridCanisterType();
	afx_msg void OnClickLabelCanisterName();
	afx_msg void OnRadioHead();
	afx_msg void OnClickRfInformation();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BARCODE_H__207A02E6_5125_489E_951C_6DF3F542187C__INCLUDED_)

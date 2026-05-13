//{{AFX_INCLUDES()
#include "msflexgrid.h"
#include "labelcontrol.h"
//}}AFX_INCLUDES
#if !defined(AFX_PIEZOSTATUS_H__D028C1FC_F271_4D2C_8874_A6285B4D50CB__INCLUDED_)
#define AFX_PIEZOSTATUS_H__D028C1FC_F271_4D2C_8874_A6285B4D50CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PiezoStatus.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPiezoStatus dialog

class CPiezoStatus : public CDialog
{
// Construction
public:
	void Set_PiezoStatus(int nHead);
	int m_nHead;
	CFont *pFont;
	void SetGridData2(int nID);
	void SetGridData1(int nID);
	CPiezoStatus(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPiezoStatus)
	enum { IDD = IDD_PIEZO_STATUS };
	CComboBox	m_comboPiezoID;
	CMSFlexGrid	m_GridPara1;
	CMSFlexGrid	m_GridPara2;
	CLabelControl	m_ctrlRecipeName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPiezoStatus)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPiezoStatus)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboPiezoId();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PIEZOSTATUS_H__D028C1FC_F271_4D2C_8874_A6285B4D50CB__INCLUDED_)

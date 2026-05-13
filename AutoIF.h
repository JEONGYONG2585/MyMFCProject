//{{AFX_INCLUDES()
#include "msflexgrid.h"
//}}AFX_INCLUDES
#if !defined(AFX_AUTOIF_H__951BA021_8F8C_4A04_9EA3_58779D5ABA48__INCLUDED_)
#define AFX_AUTOIF_H__951BA021_8F8C_4A04_9EA3_58779D5ABA48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoIF.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAutoIF dialog

class CAutoIF : public CDialog
{
// Construction
public:
	CAutoIF(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAutoIF();

// Dialog Data
	//{{AFX_DATA(CAutoIF)
	enum { IDD = IDD_AUTO_INTERFACE };
	CMSFlexGrid	m_ctrlIF;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoIF)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// variables
	IF_CAPTION_SET m_structIFCaptionSet; 

	// interface bit 저장하기 위해서..
	BOOL m_bIFStatus[6][10]; //  [type][bit상태]
	BOOL m_bIFStatusBK[6][10]; //  [type][bit상태] backup

	WORD m_nIFStatus, m_nIFStatusTemp;
	// func
	void SubOpenIFStrFile();
	void SubDisplayIFStr();
//
	// Generated message map functions
	//{{AFX_MSG(CAutoIF)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOIF_H__951BA021_8F8C_4A04_9EA3_58779D5ABA48__INCLUDED_)

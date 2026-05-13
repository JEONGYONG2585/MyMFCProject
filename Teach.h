//{{AFX_INCLUDES()
#include "msflexgrid.h"
//}}AFX_INCLUDES
#if !defined(AFX_TEACH_H__9E34BCCB_609A_439A_94E0_D17F6C041419__INCLUDED_)
#define AFX_TEACH_H__9E34BCCB_609A_439A_94E0_D17F6C041419__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Teach.h : header file
//
#include "DummyPos.h"

/////////////////////////////////////////////////////////////////////////////
// CTeach dialog

class CTeach : public CDialog
{
// Construction
public:
	void DataBK();
	CTeach(CWnd* pParent = NULL);   // standard constructor
	CDummyPos *pDummyTeachDlg;
	virtual ~CTeach();

// Dialog Data
	//{{AFX_DATA(CTeach)
	enum { IDD = IDD_TEACH };
	CMSFlexGrid	m_ctrlTeach0;
	CMSFlexGrid	m_ctrlTeach1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTeach)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// variables
	CString *m_pCap;
	//
	TEACHING_CONFIG m_structTeachingConfig[40][3]; //"Hello".. 크게 늘이지 않아도 될 듯..

	// func
	void SubOpenStrFile();

	// Generated message map functions
	//{{AFX_MSG(CTeach)
	afx_msg void OnClickCmdReturn();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickMsflexgridTeach0();
	afx_msg void OnClickMsflexgridTeach1();
	afx_msg void OnClickCmdTeachdataSave();
	afx_msg void OnClickCmdDummyTeach();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEACH_H__9E34BCCB_609A_439A_94E0_D17F6C041419__INCLUDED_)

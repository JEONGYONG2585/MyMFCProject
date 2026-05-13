#if !defined(AFX_ERRLISTAUTO_H__0795E4DA_9AC7_4A5A_B50B_CD37EFDECFD5__INCLUDED_)
#define AFX_ERRLISTAUTO_H__0795E4DA_9AC7_4A5A_B50B_CD37EFDECFD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ErrListAuto.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CErrListAuto dialog

class CErrListAuto : public CDialog
{
// Construction
public:
	CString InsertContents(int nCode, int nCode2);
	void SetListData();
	void MakeListView();
	CErrListAuto(CWnd* pParent = NULL);   // standard constructor

	BOOL m_bStatusAutoManualFlag; 
	int m_nTempFlag; 

// Dialog Data
	//{{AFX_DATA(CErrListAuto)
	enum { IDD = IDD_ERR_LIST_AUTO };
	CListCtrl	m_ctrlErrListAuto;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CErrListAuto)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CErrListAuto)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickCommandbutton1();
	afx_msg void OnAutoErrlist();
	afx_msg void OnAutoManualErrlist();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ERRLISTAUTO_H__0795E4DA_9AC7_4A5A_B50B_CD37EFDECFD5__INCLUDED_)

//{{AFX_INCLUDES()
#include "labelcontrol.h"
//}}AFX_INCLUDES
#if !defined(AFX_ERRLIST_H__C751D44D_EE2C_4E5B_8E66_B45CDEFE548A__INCLUDED_)
#define AFX_ERRLIST_H__C751D44D_EE2C_4E5B_8E66_B45CDEFE548A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ErrList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CErrList dialog

class CErrList : public CDialog
{
// Construction
public:
	void SelectLanguage();
	CErrList(CWnd* pParent = NULL);   // standard constructor
	virtual ~CErrList();

	//
	void MakeListView(int);	
	void SetListData(int);

	//
	CString m_strFileName;
	int m_nTempFlag; 

//
// Dialog Data
	//{{AFX_DATA(CErrList)
	enum { IDD = IDD_ERR_LIST };
	CButton	m_ctrlClear;
	CButton	m_ctrlLog;
	CListCtrl	m_ctrlErrList;
	CLabelControl	m_ctrlDate;
	BOOL	m_bCheckLog;
	BOOL	m_bPreviousErrList;  // 090711 이전 로그 보기 위한 상태 변수 추가 	////////////////////090711 Errlist Manual/Auto 구분 적용
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CErrList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//
	CString InsertContents(int,int);

	// Generated message map functions
	//{{AFX_MSG(CErrList)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBtnLogOpen();
	afx_msg void OnCheckLog();
	afx_msg void OnClickBtnReturn();
	afx_msg void OnBtnClear();
	afx_msg void OnPreviousErrlistCheck();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ERRLIST_H__C751D44D_EE2C_4E5B_8E66_B45CDEFE548A__INCLUDED_)

//{{AFX_INCLUDES()
#include "labelcontrol.h"
//}}AFX_INCLUDES
#if !defined(AFX_PASSQA_H__E5EC28C6_E312_4FAE_B2A1_29ADBD870E42__INCLUDED_)
#define AFX_PASSQA_H__E5EC28C6_E312_4FAE_B2A1_29ADBD870E42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PassQa.h : header file
//
#include "PassQaInput.h"

/////////////////////////////////////////////////////////////////////////////
// CPassQa dialog

class CPassQa : public CDialog
{
// Construction
public:
	void SelectLanguage();
	CPassQa(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPassQa();

// Dialog Data
	//{{AFX_DATA(CPassQa)
	enum { IDD = IDD_PASS_QA };
	CString	m_strPassWd;
	CLabelControl	m_ctrlPasswdInputTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPassQa)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPassQaInput *m_pPassQaInput;

	// Generated message map functions
	//{{AFX_MSG(CPassQa)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSQA_H__E5EC28C6_E312_4FAE_B2A1_29ADBD870E42__INCLUDED_)

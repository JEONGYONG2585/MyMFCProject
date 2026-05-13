#if !defined(AFX_PASSQAINPUT_H__008EE7AC_7FB1_4248_8E78_1FCC147A679B__INCLUDED_)
#define AFX_PASSQAINPUT_H__008EE7AC_7FB1_4248_8E78_1FCC147A679B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PassQaInput.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPassQaInput dialog

class CPassQaInput : public CDialog
{
// Construction
public:
	CPassQaInput(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPassQaInput();

// Dialog Data
	//{{AFX_DATA(CPassQaInput)
	enum { IDD = IDD_PASS_QA_INPUT };
	int		m_nNum;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPassQaInput)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_strNewData;

	// Generated message map functions
	//{{AFX_MSG(CPassQaInput)
	virtual BOOL OnInitDialog();
	afx_msg void OnCmd0();
	afx_msg void OnCmdClr();
	afx_msg void OnCmdOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSQAINPUT_H__008EE7AC_7FB1_4248_8E78_1FCC147A679B__INCLUDED_)

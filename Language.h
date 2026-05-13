//{{AFX_INCLUDES()
#include "commandbutton.h"
//}}AFX_INCLUDES
#if !defined(AFX_LANGUAGE_H__E8F72B3B_D431_445A_898D_D9C75717EC42__INCLUDED_)
#define AFX_LANGUAGE_H__E8F72B3B_D431_445A_898D_D9C75717EC42__INCLUDED_

#include "commandbutton.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Language.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLanguage dialog

class CLanguage : public CDialog
{
// Construction
public:
	BOOL OnInitDialog();
	void SelectLanguage();
	CLanguage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLanguage)
	enum { IDD = IDD_LANGUAGE };
	CCommandButton	m_ctrlChina;
	CCommandButton	m_ctrlEnglish;
	CCommandButton	m_ctrlKorea;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLanguage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLanguage)
	afx_msg void OnClickCmdKorea();
	afx_msg void OnClickCmdEnglish();
	afx_msg void OnClickCmdChina();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LANGUAGE_H__E8F72B3B_D431_445A_898D_D9C75717EC42__INCLUDED_)

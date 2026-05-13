#if !defined(AFX_MAINT_H__5BE72F1F_0A28_4BAA_B9BC_806FDAEC4048__INCLUDED_)
#define AFX_MAINT_H__5BE72F1F_0A28_4BAA_B9BC_806FDAEC4048__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Maint.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMaint dialog

class CMaint : public CDialog
{
// Construction

public:
	void SelectLanguage();
	CMaint(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CMaint)
	enum { IDD = IDD_IDLE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMaint)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CTime m_pTime;
	CString m_strTime;
	CString m_strTime1;
	// Generated message map functions
	//{{AFX_MSG(CMaint)
	afx_msg void OnClickCmdPrev();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnClickCmdPvg();
	afx_msg void OnClickCmdBuzzerOff();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINT_H__5BE72F1F_0A28_4BAA_B9BC_806FDAEC4048__INCLUDED_)

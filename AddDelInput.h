#if !defined(AFX_ADDDELINPUT_H__8184108B_0BD6_42C6_BCA4_3E3097891087__INCLUDED_)
#define AFX_ADDDELINPUT_H__8184108B_0BD6_42C6_BCA4_3E3097891087__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddDelInput.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddDelInput dialog

class CAddDelInput : public CDialog
{
// Construction
public:
	void SetstrValue();
	int StrtoValue(CString str);
	CAddDelInput(CWnd* pParent = NULL);   // standard constructor
	CAddDelInput(CString strRecentString, CPoint pOrgPoint);
	virtual ~CAddDelInput();
	
	CString m_strOldData;		// 이전 데이타
	CString m_strNewData;		// 새로운 데이타


// Dialog Data
	//{{AFX_DATA(CAddDelInput)
	enum { IDD = IDD_STRING_INPUT };
	CSpinButtonCtrl	m_ctrlSpinCH;
	CComboBox	m_ctrlComCH;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddDelInput)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void AddText(CString);
	//
	CBrush m_dlg_brush;
	CPoint m_pOrgPoint;
	CFont *m_fontCombo;

	// Generated message map functions
	//{{AFX_MSG(CAddDelInput)
	afx_msg void OnRadioPlus();
	afx_msg void OnRadioMinus();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRadioZero();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDDELINPUT_H__8184108B_0BD6_42C6_BCA4_3E3097891087__INCLUDED_)

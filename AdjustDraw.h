#if !defined(AFX_ADJUSTDRAW_H__5F57BF28_5A7D_4A8D_8539_3BB68AF449D8__INCLUDED_)
#define AFX_ADJUSTDRAW_H__5F57BF28_5A7D_4A8D_8539_3BB68AF449D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdjustDraw.h : header file
//
#include "TrendWindow.h"

/////////////////////////////////////////////////////////////////////////////
// CAdjustDraw dialog

class CAdjustDraw : public CDialog
{
// Construction
public:
	CAdjustDraw(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAdjustDraw();

// Dialog Data
	//{{AFX_DATA(CAdjustDraw)
	enum { IDD = IDD_ADJUST_DRAW };
	int		m_nHeadSelected;
	int		m_nDataSel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdjustDraw)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAdjustDraw)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void OnOK();
	afx_msg void OnRadioGraphH1();
	afx_msg void OnRadioAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private :
	// trend window의 포인터를 선언한다.
	TrendWindow *mp_TrendView;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADJUSTDRAW_H__5F57BF28_5A7D_4A8D_8539_3BB68AF449D8__INCLUDED_)

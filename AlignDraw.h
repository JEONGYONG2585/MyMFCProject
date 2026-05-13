//{{AFX_INCLUDES()
#include "labelcontrol.h"
//}}AFX_INCLUDES
#if !defined(AFX_ALIGNDRAW_H__4212F29E_0A6C_4F02_B500_5736F353CFAC__INCLUDED_)
#define AFX_ALIGNDRAW_H__4212F29E_0A6C_4F02_B500_5736F353CFAC__INCLUDED_

#include "TrendWindow.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AlignDraw.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAlignDraw dialog

class CAlignDraw : public CDialog
{
// Construction
public:
	CAlignDraw(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAlignDraw)
	enum { IDD = IDD_ALIGN };
	CLabelControl	m_ctrlAlignData;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlignDraw)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAlignDraw)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	TrendWindow *mp_TrendView;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALIGNDRAW_H__4212F29E_0A6C_4F02_B500_5736F353CFAC__INCLUDED_)

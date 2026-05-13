#if !defined(AFX_MANUMEASPOSRESULT_H__0C5B616F_F7AC_463B_AB3E_5AD62006FE7D__INCLUDED_)
#define AFX_MANUMEASPOSRESULT_H__0C5B616F_F7AC_463B_AB3E_5AD62006FE7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ManuMeasPosResult.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CManuMeasPosResult dialog

class CManuMeasPosResult : public CDialog
{
// Construction
public:
	CManuMeasPosResult(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CManuMeasPosResult)
	enum { IDD = IDD_MANU_MEAS_RESULT };
	CString	m_strPosResult;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManuMeasPosResult)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CManuMeasPosResult)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANUMEASPOSRESULT_H__0C5B616F_F7AC_463B_AB3E_5AD62006FE7D__INCLUDED_)

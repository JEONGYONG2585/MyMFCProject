#if !defined(AFX_PANELINFORMINPUT_H__DD63CDA8_DDDB_459D_8CF8_1D7CE23B4ECC__INCLUDED_)
#define AFX_PANELINFORMINPUT_H__DD63CDA8_DDDB_459D_8CF8_1D7CE23B4ECC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PanelInformInput.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPanelInformInput dialog

class CPanelInformInput : public CDialog
{
// Construction
public:
	CString m_strCF;
	CString m_strTFT;
	int m_nIndex;
	CPanelInformInput(CWnd* pParent = NULL);   // standard constructor
	CPanelInformInput(int nIndex, char strTFT, char strCF, CWnd* pParent=NULL /*=NULL*/);

// Dialog Data
	//{{AFX_DATA(CPanelInformInput)
	enum { IDD = IDD_PANEL_INFORM };
	int		m_nPanelValueTFT;
	int		m_nPanelValueCF;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPanelInformInput)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPanelInformInput)
	virtual BOOL OnInitDialog();
	afx_msg void OnCFClick();
	virtual void OnOK();
	afx_msg void OnTFTClick();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PANELINFORMINPUT_H__DD63CDA8_DDDB_459D_8CF8_1D7CE23B4ECC__INCLUDED_)

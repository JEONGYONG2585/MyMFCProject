#if !defined(AFX_SCRAPMSG_H__F6226344_EE6B_4A04_97C0_2C51665E2FAC__INCLUDED_)
#define AFX_SCRAPMSG_H__F6226344_EE6B_4A04_97C0_2C51665E2FAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScrapMsg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScrapMsg dialog

class CScrapMsg : public CDialog
{
// Construction
public:
	void SelectLanguage();
	CScrapMsg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CScrapMsg();

// Dialog Data
	//{{AFX_DATA(CScrapMsg)
	enum { IDD = IDD_SCRAP_MSG };
	int		m_nScrapSelType;
	UINT	m_nRecipeNum;
	CString	m_strGlassID;
	UINT	m_nLotNum;
	UINT	m_nSlotNum;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScrapMsg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CScrapMsg)
	afx_msg void OnScrapSel();
	afx_msg void OnButtonGlassDataEdit();
	virtual BOOL OnInitDialog();
	afx_msg void OnNgScrap();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRAPMSG_H__F6226344_EE6B_4A04_97C0_2C51665E2FAC__INCLUDED_)

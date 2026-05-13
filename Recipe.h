//{{AFX_INCLUDES()
#include "labelcontrol.h"
//}}AFX_INCLUDES
#if !defined(AFX_RECIPE_H__F79165BA_CBCD_4D58_A313_52786141B4FC__INCLUDED_)
#define AFX_RECIPE_H__F79165BA_CBCD_4D58_A313_52786141B4FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Recipe.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRecipe dialog

class CRecipe : public CDialog
{
// Construction
public:
	void Read_ModuleData(CString strRecipeNo);
	CRecipe(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRecipe();
//
	CFont *m_fontList;
	CString m_strFilePath;
/*	CString m_strFileTitle;
	CString m_strFileExt;
	CString m_strFileName;
	CString m_strDataFileExt;
	CString m_strDataFileName;
*/
	void SubDisplayRecipeName();
	void MakeListView();
	int	 SetListData();
	BOOL AddItem(CString);
//
// Dialog Data
	//{{AFX_DATA(CRecipe)
	enum { IDD = IDD_RECIPE_CHANGE };
	CListCtrl	m_ctrlRecipeList;
	CLabelControl	m_ctrlRecipeName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecipe)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRecipe)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickCmdReturn();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnPaint();
	afx_msg void OnClickCmdRecipeLoad();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECIPE_H__F79165BA_CBCD_4D58_A313_52786141B4FC__INCLUDED_)

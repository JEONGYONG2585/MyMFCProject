//{{AFX_INCLUDES()
#include "commandbutton.h"
#include "labelcontrol.h"
//}}AFX_INCLUDES
#if !defined(AFX_FILEMNG_H__3EDE8BA9_8D94_4A90_8091_64B9EBDC28A8__INCLUDED_)
#define AFX_FILEMNG_H__3EDE8BA9_8D94_4A90_8091_64B9EBDC28A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileMng.h : header file
//
//#include "P8CA_LcDispDoc.h"
#include <afxtempl.h>
#include "LcDef.h"

/////////////////////////////////////////////////////////////////////////////
// CFileMng dialog

class CFileMng : public CDialog
{
// Construction
public:
	void SelectLanguage();
	CFileMng(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFileMng();

// Dialog Data
	//{{AFX_DATA(CFileMng)
	enum { IDD = IDD_FILE_MANAGE };
	CComboBox	m_ctrlRecipeList;
	CListCtrl	m_ctrlTerminal;
	CListCtrl	m_ctrlOrigin;
	CCommandButton	m_ctrlDo;
	CCommandButton	m_ctrlNameChange;
	CCommandButton	m_ctrlCopy;
	CLabelControl	m_ctrlRecipeName;
	CLabelControl	m_ctrlRecipeNameBack;
	CCommandButton	m_ctrlNewMake;
	CLabelControl	m_ctrlRecipeNO;
	CCommandButton	m_ctrlChange;
	CCommandButton	m_ctrlDelete;
	CCommandButton	m_ctrlRename;
	CLabelControl	m_ctrlRecipeNO2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileMng)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	UINT m_nSelMenu;
	// List control 관련
	void SubMakeListView();
	int  SubSetListData(UINT);
	BOOL SubAddItem(UINT , CString);
	void SubDisplayRecipeName();
	//
	void SubDo0();
	void SubDo1();
	void SubDo2();
//	void SubDo3();

	//
	CString m_strDataPath;
	BOOL FindRecipeData(CString);
	BOOL ReadRecipeFolder(CString);
//
	DATA_EDITOR m_structDataEditor;
	// Recipe Data관리변수
	RECIPE_DATA m_rd;
	CArray<RECIPE_DATA, RECIPE_DATA&> m_arRecipeData;
// 
	BOOL SaveKiban(int , CString);
	BOOL ReadKiban(int , CString);
//
	// combo선택 할때, 실제 Recipe번호를 판단한다..
	WORD m_nRecipeTemp[MAX_RECIPE];
//
	// Generated message map functions
	//{{AFX_MSG(CFileMng)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClickBtnCopy();
	afx_msg void OnClickBtnDo();
	afx_msg void OnClickBtnNameChange();
	afx_msg void OnClickCmdReturn();
	afx_msg void OnPaint();
	afx_msg void OnClickBtnNewmake();
	afx_msg void OnSelchangeComboTerminal();
	afx_msg void OnClickLabelRecipeno();
	afx_msg void OnClickChange();
	afx_msg void OnClickDelete();
	afx_msg void OnClickRename();
	afx_msg void OnClickLabelRecipeno2();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEMNG_H__3EDE8BA9_8D94_4A90_8091_64B9EBDC28A8__INCLUDED_)

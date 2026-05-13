//{{AFX_INCLUDES()
#include "msflexgrid.h"
#include "labelcontrol.h"
//}}AFX_INCLUDES
#if !defined(AFX_MODULEINFO_H__CA8F4990_0CD0_482D_AB7A_697C505CA419__INCLUDED_)
#define AFX_MODULEINFO_H__CA8F4990_0CD0_482D_AB7A_697C505CA419__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModuleInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModuleInfo dialog

class CModuleInfo : public CDialog
{
// Construction
public:
	void LoadModuleBarTeach();
	void SaveModuleBarTeach();
	void Load_Module_Before(CString strRecipeNo);
	void Save_Module_Before(CString strRecipeNo);
	void SelectLanguage();
	BOOL m_bReadStatus;
	void CaculateOffset();
	BOOL bModuleID; 
	int nHead;
	CString strModuleID;
	int nAmplitude_DB;
	int nNegOffset_DB;
	int nFrequecy_DB;
	double nDuty_DB;
	int nFirst_Vol_DB;
	int nFallingEdge_DB;
	void Get_ModuleInfo(int nModuleID , int nHead  , BOOL bInitial);
	void SetGrid_Initial();
	CModuleInfo(CWnd* pParent = NULL);   // standard constructor

	CString NowModuleID[MAX_NOZZLE];		//ehji Module DB를 불러오기 위해 저장.

// Dialog Data
	//{{AFX_DATA(CModuleInfo)
	enum { IDD = IDD_MODULE_INFO };
	CMSFlexGrid	m_GridModule;
	CLabelControl	m_strMsgDB;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModuleInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModuleInfo)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnClickGridModuleInfo();
	afx_msg void OnClickCmdReturn();
	afx_msg void OnClickCmdModuleDbRegistry();
	afx_msg void OnClickCmdRead();
	afx_msg void OnClickCmdApply();
	afx_msg void OnClickDbModify();
	afx_msg void OnClickCmdModuleDbBefore();
	afx_msg void OnDblClickCmdModuleDbMove(LPDISPATCH Cancel);
	afx_msg void OnClickCmdModuleDbTeaching();
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODULEINFO_H__CA8F4990_0CD0_482D_AB7A_697C505CA419__INCLUDED_)

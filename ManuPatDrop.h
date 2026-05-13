//{{AFX_INCLUDES()
#include "msflexgrid.h"
#include "labelcontrol.h"
#include "commandbutton.h"
//}}AFX_INCLUDES
#if !defined(AFX_MANUPATDROP_H__CB51A279_0FDF_46BE_9C32_16AF2D8CDA1E__INCLUDED_)
#define AFX_MANUPATDROP_H__CB51A279_0FDF_46BE_9C32_16AF2D8CDA1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ManuPatDrop.h : header file
//
#include "PatternWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CManuPatDrop dialog

class CManuPatDrop : public CDialog
{
// Construction
public:
	void SelectLanguage();
	BOOL m_bStepNotSelError;
	CString PanelInformM(int );

	void ReturnRecipeData();
	void PatDataTmpSave();
	void SubTimerManuDropCountDisp();
	void SavePanelMarkSet();
	CManuPatDrop(CWnd* pParent = NULL);   // standard constructor
	virtual ~CManuPatDrop();
	PATTERN_DATA m_structPatternData[MAX_PATTERN];
	CArray<DROP_DATA, DROP_DATA> *m_structDropData;

// Dialog Data
	//{{AFX_DATA(CManuPatDrop)
	enum { IDD = IDD_MANU_PATDROP };
	CComboBox	m_ctrlComboPattern;
	CButton	m_staticScreen;
	CMSFlexGrid	m_ctrlCimAdel;
	CLabelControl	m_ctrlSetAdel;
	CLabelControl	m_ctrlSetPanelInform;
	CLabelControl	m_ctrlRecipeName;
	CCommandButton	m_ctrlSeeAllPattern;
	CCommandButton	m_ctrlHead1;
	CLabelControl	m_ctrlMessageManuPatDrop;
	CCommandButton	m_ctrlReturn;
	CCommandButton	m_ctrlHead2;
	CCommandButton	m_ctrlHead3;
	CCommandButton	m_ctrlHead10;
	CCommandButton	m_ctrlHead11;
	CCommandButton	m_ctrlHead12;
	CCommandButton	m_ctrlHead4;
	CCommandButton	m_ctrlHead5;
	CCommandButton	m_ctrlHead6;
	CCommandButton	m_ctrlHead7;
	CCommandButton	m_ctrlHead8;
	CCommandButton	m_ctrlHead9;
	CCommandButton	m_ctrlRepairEdit;
	CCommandButton	m_ctrlDropClear;
	CCommandButton	m_ctrlDropCompleted;
	CMSFlexGrid	m_ctrlPanelMatrixSet;
	CLabelControl	m_ctrlHeadlabel1;
	CLabelControl	m_ctrlHeadlabel10;
	CLabelControl	m_ctrlHeadlabel11;
	CLabelControl	m_ctrlHeadlabel12;
	CLabelControl	m_ctrlHeadlabel2;
	CLabelControl	m_ctrlHeadlabel3;
	CLabelControl	m_ctrlHeadlabel4;
	CLabelControl	m_ctrlHeadlabel5;
	CLabelControl	m_ctrlHeadlabel6;
	CLabelControl	m_ctrlHeadlabel7;
	CLabelControl	m_ctrlHeadlabel8;
	CLabelControl	m_ctrlHeadlabel9;
	CCommandButton	m_ctrlHead13;
	CCommandButton	m_ctrlHead14;
	CCommandButton	m_ctrlHead15;
	CCommandButton	m_ctrlHead16;
	CLabelControl	m_ctrlHeadlabel13;
	CLabelControl	m_ctrlHeadlabel14;
	CLabelControl	m_ctrlHeadlabel15;
	CLabelControl	m_ctrlHeadlabel16;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManuPatDrop)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//
	CPatternWnd *m_pPatternWnd;
	int m_nCurrentPatternNo;
	BOOL m_bAllPattern;
	BOOL m_bStart; // window 처음그릴때는  두번째와는 다른 initial 설정이 필요하므로..
				   // 처음을 나타내기 위한 flag..
	int m_nTimerCount;
	int m_nSelectedSequence;

	//
	void SubDrawPattern();
	void SubSetControl(int);
	void SubTimerManuPatDropFunc();
	// Generated message map functions
	//{{AFX_MSG(CManuPatDrop)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelchangeComboPatternNo();
	afx_msg void OnClickCmdSeeAllPattern();
	afx_msg void OnClickLabelAdel();
	afx_msg void OnBtnChange();
	afx_msg void OnClickMsflexgridCimAdel();
	afx_msg void OnClickCmdReturn();
	afx_msg void OnPaint();
	afx_msg void OnClickDropCountReset();
	afx_msg void OnClickRepairEdit();
	afx_msg void OnClickGlsStatusClear();
	afx_msg void OnClickGlsStatusSet();
	afx_msg void OnClickMsflexgridPanelInformSet();
	afx_msg void OnClickLabelPanInform();
	afx_msg void OnPanelInformAllChg();
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANUPATDROP_H__CB51A279_0FDF_46BE_9C32_16AF2D8CDA1E__INCLUDED_)

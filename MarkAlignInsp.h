//{{AFX_INCLUDES()
#include "ligeledisplay.h"
#include "commandbutton.h"
#include "msflexgrid.h"
#include "labelcontrol.h"
//}}AFX_INCLUDES
#if !defined(AFX_MARKALIGNINSP_H__FBF48ED1_76C8_4E31_B420_28FF6485B78B__INCLUDED_)
#define AFX_MARKALIGNINSP_H__FBF48ED1_76C8_4E31_B420_28FF6485B78B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MarkAlignInsp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMarkAlignInsp dialog

class CMarkAlignInsp : public CDialog
{
// Construction
public:
	void SelectLanguage();
	int m_nCamSel;
	double m_dCurrPos_K;
	double m_dCurrPos_S[MAX_NOZZLE];
	int m_nTimerCount;
	void ButtonControl(BOOL bCtrl);
	WORD UP_OP;
	WORD DOWN_OP;
	WORD LEFT_OP;
	WORD RIGHT_OP;
	int m_nCamChannel;
	double m_nJogSpd;
	CFont *m_fontComboSpd;
	CString m_strAlignData;
	CString m_strCurrentData;
	CString m_strTeachingData;
	int m_nInfoDisplay;
	void SetGrid(int ViewType);
	CString m_strMsg;
	CMarkAlignInsp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMarkAlignInsp)
	enum { IDD = IDD_MARK_ALIGN };
	CComboBox	m_comboSpd;
	CLigEleDisplay	m_ctrlAlign1;
	CCommandButton	m_ctrlReset;
	CLigEleDisplay	m_ctrlAlign2;
	CMSFlexGrid	m_GridMarkInfo;
	CLabelControl	m_ctrlLightH1;
	CLabelControl	m_ctrlLightH16;
	CCommandButton	m_ctrlRegistMark;
	CCommandButton	m_ctrlMoveJog;
	CLigEleDisplay	m_ctrlDisplay0;
	CLigEleDisplay	m_ctrlDisplay1;
	CCommandButton	m_ctrlMarkSel;
	CCommandButton	m_ctrlMarkFind;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMarkAlignInsp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMarkAlignInsp)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickReturn();
	afx_msg void OnSelchangeComboSpd();
	afx_msg void OnClickRegistMark();
	afx_msg void OnClickMoveMarkPos();
	afx_msg void OnClickMarkAlign();
	afx_msg void OnClickMarkFind();
	afx_msg void OnClickMarkSave();
	afx_msg void OnClickMoveJog();
	afx_msg void OnClickLightH16();
	afx_msg void OnClickLightH1();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClickMarkSel();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MARKALIGNINSP_H__FBF48ED1_76C8_4E31_B420_28FF6485B78B__INCLUDED_)

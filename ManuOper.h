//{{AFX_INCLUDES()
#include "commandbutton.h"
#include "labelcontrol.h"
//}}AFX_INCLUDES
#if !defined(AFX_MANUOPER_H__5D647AC1_8924_42A7_8227_F673D306A401__INCLUDED_)
#define AFX_MANUOPER_H__5D647AC1_8924_42A7_8227_F673D306A401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ManuOper.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CManuOper dialog

class CManuOper : public CDialog
{
// Construction
public:
	void SelectLanguage();
	CString m_strAlignData;
	void SubTimerMotorStatusDisp();
	WORD m_nIoVal;
	WORD m_nAxis;
	int m_nBdID;
	int m_nTimerCount;

	CManuOper(CWnd* pParent = NULL);   // standard constructor
	virtual ~CManuOper();

// Dialog Data
	//{{AFX_DATA(CManuOper)
	enum { IDD = IDD_MANU_OPER };
	int		m_nY;
	int		m_nS;
	CCommandButton	m_ctrlJog;
	CCommandButton	m_ctrlOrgAll;
	CCommandButton	m_ctrlOrgSelected;
	int		m_nSpeed;
	CCommandButton	m_ctrlReturn;
	CLabelControl	m_ctrlLimitPlus;
	CLabelControl	m_ctrlLimitMinus;
	CLabelControl	m_ctrlHome;
	CLabelControl	m_ctrlMotorAlarm;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManuOper)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void SubButtonDisable();
	void SubButtonEnable();
	void SubPositionDisplay(int);

	// Generated message map functions
	//{{AFX_MSG(CManuOper)
	afx_msg void OnPaint();
	afx_msg void OnClickCmdReturn();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClickCmdJog();
	afx_msg void OnClickCmdOrgSelected();
	afx_msg void OnClickCmdOrgAll();
	afx_msg void OnCmdS1();
	afx_msg void OnCmdY();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSpeedLow();
	virtual BOOL OnInitDialog();
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANUOPER_H__5D647AC1_8924_42A7_8227_F673D306A401__INCLUDED_)

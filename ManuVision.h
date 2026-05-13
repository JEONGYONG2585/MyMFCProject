//{{AFX_INCLUDES()
#include "labelcontrol.h"
//}}AFX_INCLUDES
#if !defined(AFX_MANUVISION_H__71C4889B_8B7D_4CDA_BFD0_A2406D9260AF__INCLUDED_)
#define AFX_MANUVISION_H__71C4889B_8B7D_4CDA_BFD0_A2406D9260AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ManuVision.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CManuVision dialog

class CManuVision : public CDialog
{
// Construction
public:
	void SelectLanguage();
	int m_nCal_Head;
	double m_dS_Start;
	double m_dK_Start;
	double m_dK_End;
	int m_nS_Count;
	double m_dS_Dist;
	double m_dAmount;
	void Read_Cal();
	void Save_Cal();
	BOOL m_bRcvMsgTimeOver;
	CString m_strMsg;
	void OnBtn_Ctr(BOOL bOn);
	void Disp_Progress();
	void OnDisp_Param();
	CManuVision(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CManuVision)
	enum { IDD = IDD_MANU_VISION_SOLID };
	int		m_nMode;
	CLabelControl	m_ctrlH1;
	CLabelControl	m_ctrlH10;
	CLabelControl	m_ctrlH11;
	CLabelControl	m_ctrlH12;
	CLabelControl	m_ctrlH2;
	CLabelControl	m_ctrlH3;
	CLabelControl	m_ctrlH4;
	CLabelControl	m_ctrlH5;
	CLabelControl	m_ctrlH6;
	CLabelControl	m_ctrlH7;
	CLabelControl	m_ctrlH8;
	CLabelControl	m_ctrlH9;
	CLabelControl	m_ctrlHead;
	CLabelControl	m_ctrlS_Start;
	CLabelControl	m_ctrlK_Start;
	CLabelControl	m_ctrlK_End;
	CLabelControl	m_ctrlS_Count;
	CLabelControl	m_ctrlS_Dist;
	CLabelControl	m_ctrlAmount;
	CLabelControl	m_ctrlH13;
	CLabelControl	m_ctrlH14;
	CLabelControl	m_ctrlH15;
	CLabelControl	m_ctrlH16;
	CLabelControl	m_ctrlX1;
	CLabelControl	m_ctrlX10;
	CLabelControl	m_ctrlX11;
	CLabelControl	m_ctrlX12;
	CLabelControl	m_ctrlX13;
	CLabelControl	m_ctrlX14;
	CLabelControl	m_ctrlX15;
	CLabelControl	m_ctrlX16;
	CLabelControl	m_ctrlX2;
	CLabelControl	m_ctrlX3;
	CLabelControl	m_ctrlX4;
	CLabelControl	m_ctrlX5;
	CLabelControl	m_ctrlX6;
	CLabelControl	m_ctrlX7;
	CLabelControl	m_ctrlX8;
	CLabelControl	m_ctrlX9;
	CLabelControl	m_ctrlYL1;
	CLabelControl	m_ctrlYL10;
	CLabelControl	m_ctrlYL11;
	CLabelControl	m_ctrlYL12;
	CLabelControl	m_ctrlYL13;
	CLabelControl	m_ctrlYL14;
	CLabelControl	m_ctrlYL15;
	CLabelControl	m_ctrlYL16;
	CLabelControl	m_ctrlYL2;
	CLabelControl	m_ctrlYL3;
	CLabelControl	m_ctrlYL4;
	CLabelControl	m_ctrlYL5;
	CLabelControl	m_ctrlYL6;
	CLabelControl	m_ctrlYL7;
	CLabelControl	m_ctrlYL8;
	CLabelControl	m_ctrlYL9;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManuVision)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CManuVision)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickInitial();
	afx_msg void OnClickWritePath();
	afx_msg void OnClickWriteRecipe();
	afx_msg void OnClickWriteCell();
	afx_msg void OnClickMoving();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClickReturn();
	afx_msg void OnClickOffset1();
	afx_msg void OnClickOffset2();
	afx_msg void OnClickOffset3();
	afx_msg void OnClickOffset4();
	afx_msg void OnClickOffset5();
	afx_msg void OnClickOffset6();
	afx_msg void OnClickOffset7();
	afx_msg void OnClickOffset8();
	afx_msg void OnClickOffset9();
	afx_msg void OnClickOffset10();
	afx_msg void OnClickOffset11();
	afx_msg void OnClickOffset12();
	afx_msg void OnClickSaveOffset();
	afx_msg void OnDblClickTitle(LPDISPATCH Cancel);
	afx_msg void OnClickCalStart();
	afx_msg void OnClickHeadNo();
	afx_msg void OnClickSStart();
	afx_msg void OnClickKStartPos();
	afx_msg void OnClickKEndPos();
	afx_msg void OnClickSMoveCount();
	afx_msg void OnClickSMoveDist();
	afx_msg void OnClickDropAmount();
	afx_msg void OnClickOffset13();
	afx_msg void OnClickOffset14();
	afx_msg void OnClickOffset15();
	afx_msg void OnClickOffset16();
	afx_msg void OnClickOffsetX1();
	afx_msg void OnClickOffsetX2();
	afx_msg void OnClickOffsetX3();
	afx_msg void OnClickOffsetX4();
	afx_msg void OnClickOffsetX5();
	afx_msg void OnClickOffsetX6();
	afx_msg void OnClickOffsetX7();
	afx_msg void OnClickOffsetX8();
	afx_msg void OnClickOffsetX9();
	afx_msg void OnClickOffsetX10();
	afx_msg void OnClickOffsetX11();
	afx_msg void OnClickOffsetX12();
	afx_msg void OnClickOffsetX13();
	afx_msg void OnClickOffsetX14();
	afx_msg void OnClickOffsetX15();
	afx_msg void OnClickOffsetX16();
	afx_msg void OnClickOffsetYL1();
	afx_msg void OnClickOffsetYL2();
	afx_msg void OnClickOffsetYL3();
	afx_msg void OnClickOffsetYL4();
	afx_msg void OnClickOffsetYL5();
	afx_msg void OnClickOffsetYL6();
	afx_msg void OnClickOffsetYL7();
	afx_msg void OnClickOffsetYL8();
	afx_msg void OnClickOffsetYL9();
	afx_msg void OnClickOffsetYL10();
	afx_msg void OnClickOffsetYL11();
	afx_msg void OnClickOffsetYL12();
	afx_msg void OnClickOffsetYL13();
	afx_msg void OnClickOffsetYL14();
	afx_msg void OnClickOffsetYL15();
	afx_msg void OnClickOffsetYL16();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANUVISION_H__71C4889B_8B7D_4CDA_BFD0_A2406D9260AF__INCLUDED_)

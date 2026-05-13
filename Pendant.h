//{{AFX_INCLUDES()
#include "labelcontrol.h"
#include "mscomm.h"
#include "Serial.h"
#include "commandbutton.h"
//}}AFX_INCLUDES
#if !defined(AFX_PENDANT_H__49939BEE_993F_403F_9FF9_C16E053618E5__INCLUDED_)
#define AFX_PENDANT_H__49939BEE_993F_403F_9FF9_C16E053618E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Pendant.h : header file
//
#define PENDANT_INFO_FILE_PATH _T("D:\\TOP\\PendantInfo_LC.ini")

typedef enum _LC_AXIS_NO{
	STAGE_Y	= 0,
	STAGE_X,
	STAGE_TH,
	GANTRY_L,
	GANTRY_R,
	HEAD_S1,
	HEAD_S2,
	HEAD_S3,
	HEAD_S4,
	HEAD_S5,
	HEAD_S6,
	HEAD_S7,
	HEAD_S8,
	HEAD_S9,
	HEAD_S10,
	HEAD_S11,
	HEAD_S12,
	HEAD_S13,
	HEAD_S14,
	HEAD_S15,
	HEAD_S16,
	HEAD_P1,
	HEAD_P2,
	HEAD_P3,
	HEAD_P4,
	HEAD_P5,
	HEAD_P6,
	HEAD_P7,
	HEAD_P8,
	HEAD_P9,
	HEAD_P10,
	HEAD_P11,
	HEAD_P12,
	HEAD_P13,
	HEAD_P14,
	HEAD_P15,
	HEAD_P16,
	HEAD_G1,
	HEAD_G2,
	HEAD_G3,
	HEAD_G4,
	HEAD_G5,
	HEAD_G6,
	HEAD_G7,
	HEAD_G8,
	HEAD_G9,
	HEAD_G10,
	HEAD_G11,
	HEAD_G12,
	HEAD_G13,
	HEAD_G14,
	HEAD_G15,
	HEAD_G16,
	LC_MAX_AXIS,
}LC_AXIS_NO;

typedef struct AxisInfo
{
	BOOL bUse;
	BOOL bMotionType;
	int nAxisNo;
	WORD wUse_Address;
	WORD wSelectAxis_Address;
}AXIS_INFO;

#define PENDANT_TEST 0    // PENDANT TEST MODE 일경우 만 1 실제 설비에서는 0...

#define WM_MESSAGEBOX			WM_USER+100
#define TIMER_UI 1
#define TIMER_THREADCHECK 2

/////////////////////////////////////////////////////////////////////////////
// CPendant dialog


class CPendant : public CDialog
{
// Construction
public:
	CPendant(CWnd* pParent = NULL);   // standard constructor	
	AXIS_INFO m_structAxisData[LC_MAX_AXIS];	
	BOOL g_bThreadRunFlag;
	BOOL m_bEZRUse;
	int m_nMaxFasBoard;
	int m_nMaxFasAxis;	
	int m_nEZRStartPort;
	int m_nEZREndPort;
	int m_nEZRUseSlaveNo;
	CString m_strRxDataBuf;
	CString m_strMessageText;
	BOOL m_bReciveDataOn;
	BYTE m_szRcvBuf[200];
	BOOL m_bPendantThreadExitFlag;
	int m_nSelectAxis;
	int m_nSelectAxisBk;
	BOOL m_bSelectServoOn;
	BOOL m_bFlagSelectServoOnOff;
	double m_dSelectSpeed;
	BOOL m_bSelectAxisStatus_ServoOn;
	BOOL m_bSelectAxisStatus_ServoAlarm;
	BOOL m_bSelectAxisStatus_Plus_Limit;
	BOOL m_bSelectAxisStatus_Minus_Limit;
	BOOL m_bPendantJogMode;
	BOOL m_bHeartBitFlag;

	BOOL m_bConnectionCheck;
	
	int m_nPendantGripBit;
	int m_nJogPlusBit;
	int m_nJogMinusBit;
	int m_nConnectionBit;
	int m_nKeyLockBit;
	int m_nPowerOnBit;
	int m_nGantryL1AxisNo;
	int m_nGantryL2AxisNo;
	int m_nGantryR1AxisNo;
	int m_nGantryR2AxisNo;

	BOOL PendantPortOpen();
	void PendantPowerOnOff(BOOL bOn);
	void DoEvents();
	
	void SetPendantSingleCoil(DWORD wCoil, BOOL bOnOff);
	void SetPendantMultyCoil(WORD wStartCoilAddress, WORD wPointCoilNo, WORD wValue) ;
	void SetPendantSingleRegister(WORD wRegister, WORD wValue);
	int GetPendantCoil(WORD wRegisterStartAddress, WORD wAddressPointNo);
	int GetPendantRegister(WORD wRegisterStartAddress, WORD wAddressPointNo);	
	
	int INI_ReadKeyCount(CString strFileName, CString strSection);
	CString INI_ReadKeyName(CString strFileName, CString strSection, int nLine);
	int INI_ReadInt(CString strFileName, CString strSection, CString strKey);
	double INI_ReadDouble(CString strFileName, CString strSection, CString strKey);
	bool INI_ReadBool(CString strFileName, CString strSection, CString strKey);
	char* INI_ReadString(CString strFileName, CString strSection, CString strKey);
	void INI_WriteInt(CString strFileName, CString strSection, CString strKey, int iValue);
	void INI_WriteDouble(CString strFileName, CString strSection, CString strKey, double dValue);
	void INI_WriteDouble(CString strFileName, CString strSection, CString strKey, double dValue, int nPrecision);	//기존 WriteDouble함수에 자리수 인자 추가
	void INI_WriteBool(CString strFileName, CString strSection, CString strKey, bool bolValue);
	void INI_WriteString(CString strFileName, CString strSection, CString strKey, char* szValue);
	void INI_DeleteSection(CString strFileName, CString strSection);
	
	void SubTimerUIControll();
	void BtnEnable(BOOL bEnable);
	void AddStringMessageListBox(WPARAM wParam, LPARAM lParam);
	void AllAxisJogSpeedSet();
	void AllAxisServoOff();
	void ReadDataInfo();
	BOOL ReadPendantConnectionIO();
	BOOL ReadPendantKeyLockIO();
	void ReadSelectAxis();
	void ReadSelectAxisStatus();
	BOOL ReadPendantJogModeBit();
	int ReadPendantJogIO();  //
	void WriteSelectAxisStatus();
	void SetHeartBitOnOff();
	void SendData_UseAxisInfo(); 

	void KillThreadPendant();
	void RunThreadPendant();

	CWinThread *m_pThread;
	HANDLE m_hThread;
// Dialog Data
	//{{AFX_DATA(CPendant)
	enum { IDD = IDD_PENDANT };
	CListBox	m_ctrlListMessage;
	CLabelControl	m_ctrlConnection;
	CMSComm	m_ctrlCommPendant;
	CLabelControl	m_ctrlMessageBox;
	CLabelControl	m_ctrlPowerCheck;
	CLabelControl	m_ctrlSelectAxis;
	CLabelControl	m_ctrlAxisSpeed;
	CLabelControl	m_ctrlAxisServoOn;
	CLabelControl	m_ctrlAxisActualPulse;
	CCommandButton	m_ctrlInitial;
	CCommandButton	m_ctrlPowerOnOff;
	CCommandButton	m_ctrlDisconnect;
	CLabelControl	m_ctrlJogMode;
	CLabelControl	m_ctrlCommunication;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPendant)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	char m_szFileName[255];
	char szSection[255];
	char szKey[255];
	// Generated message map functions
	//{{AFX_MSG(CPendant)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDblClickPowerOn(LPDISPATCH Cancel);
	afx_msg void OnDblClickPendantInit(LPDISPATCH Cancel);
	afx_msg void OnOnCommMscommPendant();
	afx_msg void OnDblClickCmdDisconnect(LPDISPATCH Cancel);
	afx_msg void OnClickCmdReturn();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
typedef struct tagPDTTHREADPARAM
{
	CPendant	*pPdt;
}PDTTHREADPARAMS;
UINT ThreadIOFunc(LPVOID pParam);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PENDANT_H__49939BEE_993F_403F_9FF9_C16E053618E5__INCLUDED_)

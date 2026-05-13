// P8CA_LcDispView.h : interface of the CP8CA_LcDispView class
//
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INCLUDES()
#include "commandbutton.h"
#include "labelcontrol.h"
#include "mscomm.h"
#include "kovision.h"
#include "pmocx.h"
#include "ligeledisplay.h"
#include "milgrabber.h"
#include "actqj71e71tcp3.h"
//}}AFX_INCLUDES

#if !defined(AFX_P8CA_LcDispVIEW_H__57EEDCD8_7B2B_4A58_800B_F37845CDBE04__INCLUDED_)
#define AFX_P8CA_LcDispVIEW_H__57EEDCD8_7B2B_4A58_800B_F37845CDBE04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "P8CA_LcDispDoc.h"
#include "PassQa.h"
#include "Auto.h"
#include "Adjust.h"

#include "DevicePart.h"
#include "Serial.h"

#include "Maint.h"

#include "VisionCmds.h"

#include "TempOP.h"//원격 제어 
#include "PiezoDevice.h"  //by shin//2013.01.15//piezo add..

#include "Employee_Num.h"		//ehji 141110

////////////////////////////////////////////////////
////////////// extern variables
////////////////////////////////////////////////////
extern BOOL g_SuppyDropReport;		//ehji 150513
extern int g_nLcSupplyAllcount[MAX_NOZZLE];	//ehji 150505
extern BOOL g_bCalibrationStatus[MAX_NOZZLE]; //120419 by shlee
extern UINT g_nGlassCount;
extern int g_nGlassCountScanCycle;	//140918 ehji
extern BOOL g_Nmodulepara;		//ehji 150110
extern BOOL g_MeasureCountError;	//ehji 150527
extern BOOL g_bStatusError[MAX_NOZZLE];			//ehji 150529
extern int g_MeasureheadError;		//ehji 150527
extern BOOL g_DropSensorCheck;		//ehji 150322
extern BOOL g_WarmupPTPmode;		//ehji 150527
extern UINT g_nTotalGlassCount;	
extern long global_Sequence_Count;
extern BOOL g_bSpecCount;
extern long g_nDrop_Repeat_Count;
extern BOOL g_bCommunicationType;
extern BOOL g_bActiveTCP;
extern DROPINFO Drop_Info;
extern short BitTFReceiveIF;
extern short BitTFSendIF;
extern short nCIMPanelInfoPos;		
extern BOOL g_bReportWorkTest;
extern int g_nReportCount;
extern int g_nReportCountBK;
extern BOOL g_Is6LineMC; //lbg 6라인 구별 추가..

extern CIM_DATA CIM_Information;
extern THREADSTAGE ThreadStage;
extern THREADORIGIN ThreadOrigin;
//extern THREADBALANCE ThreadBalance1;
//extern THREADBALANCE ThreadBalance2;
//extern THREADBALANCE ThreadBalance3;
//extern THREADBALANCE ThreadBalance4;
//extern THREADBALANCE ThreadBalance5;
//extern THREADBALANCE ThreadBalance6;
//extern THREADBALANCE ThreadBalance7;
//extern THREADBALANCE ThreadBalance8;
extern THREADBALANCE ThBal[MAX_BAL];

extern THREADSUPPYBAL ThreadSupplyBal1;
extern THREADSUPPYBAL ThreadSupplyBal2;
extern ERROR_INFORM Error_Information[50];
extern BOOL g_bAdjCalBaseCompl[12];
extern THREADIO ThreadIO;
extern CIM_GLASS_INFORM_SAVE Glass_Inform[2][10];	// 0:TFT, 1:CF
extern int LinePattern_Seq_count;
extern UINT	g_nGlassAlignCount;//Mark Align Count 2010.03.22 by tskim

extern THREADVISION ThreadVisionDetect; //VISION DETECT OVERLAP

//2011.04.03 by tskim
extern BALSTATE Balstate;
/////////////////////////////////////////////////////
/////////////// end of extern.. 
/////////////////////////////////////////////////////
#include "actqj71e71tcp3.h"	// Added by ClassView
#include "MCTimer.h"	// Added by ClassView
#include "TempOP.h"	// Added by ClassView
#include "McStatus.h"	// Added by ClassView
#include "N_Function.h"	// Added by ClassView
class CP8CA_LcDispView : public CFormView
{
protected: // create from serialization only
	CP8CA_LcDispView();
	DECLARE_DYNCREATE(CP8CA_LcDispView)

public:
	//{{AFX_DATA(CP8CA_LcDispView)
	enum { IDD = IDD_P8CA_LcDisp_FORM };
	CButton	m_ctrlIonizerON;
	CCommandButton	m_ctrlEngMode;
	CLabelControl	m_ctrlRecipeName;
	CSerial	m_ctrlComm1;
	CSerial	m_ctrlComm2;
	CSerial	m_ctrlComm3;
	CSerial	m_ctrlComm4;
	CSerial	m_ctrlComm5;
	CSerial	m_ctrlComm6;
	CSerial	m_ctrlComm7;
	CSerial	m_ctrlComm10;
	CSerial	m_ctrlComm8;
	CSerial	m_ctrlComm9;
	CCommandButton	m_ctrlGlassDataReq;
	CCommandButton	m_ctrlErrorReset;
	CCommandButton	m_ctrlTasDataReport;
	CSerial	m_ctrlComm11;
	CSerial	m_ctrlComm12;
	CLabelControl	m_labelVersion;
	CActQJ71E71TCP3	m_actTCP1;
	CCommandButton	m_ctrlautorun;
	CCommandButton	m_ctrlmanual;
	CCommandButton	m_ctrlrecipe;
	CCommandButton	m_ctrldataadjust;
	CCommandButton	m_ctrldataeditor;
	CCommandButton	m_ctrlpass;
	CCommandButton	m_ctrlstatuscontinue;
	CCommandButton	m_ctrlpddownstate;
	CCommandButton	m_ctrlRMSReport;
	CCommandButton	m_ctrlMotorLoad;
	//}}AFX_DATA

// Attributes
public:
	CP8CA_LcDispDoc* GetDocument();

// Operations
public:
	void KillThread(int);
	void RunThread(int);
	CWinThread *m_pThread[THREAD_NO];
	HANDLE m_hThread[THREAD_NO];
	CVisionCmds		*pVision;
//


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CP8CA_LcDispView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	void OpenLockDisplay();
	int m_nPairInterlockResult;//2015.04.29 원재료 전산화Add Pair Interlock 
	CString m_strTempDummyDropResult;//2015.04.29 원재료 전산화Add Dummy Drop 보고
	int m_nErrPatternNum;
	HWND m_hWndMotorLoad;
	BOOL WriteEquipStatus(int ntype);
	BOOL MachineAddressSort(int MachineNo);
	BOOL ReadGlassData(int ntype);
	int m_nCalibUpDate3[MAX_NOZZLE][MAX_MEADATA_GETCOUNT3];//2015.01.24 by tskim Section Calibration
	BOOL bDropStart_Test;//2014.11.29 by tskim Data Tracing for Test
	double dDropCurPos_X_Test;
	double dDropCurPos_Y_Test;
	double dDropPos_X_Test;
	double dDropPos_Y_Test;
	double dDropVel_Test;
	HANDLE m_hTimeEvent_Test;
	CMMTimer m_mmTimer_Test;
	CString strLog_Test;
	WORD wDropQueue_Test;
	WORD wFreeArcQueue_Test;
	WORD wFreeArcMotionQueue_Test;
	WORD wFreeLinearQueue_Test;
	WORD wFreeContLinearQueue_Test;
	WORD wFreeCircleQueue_Test;
	WORD wFreeAxisQueue_Test1;
	WORD wFreeAxisQueue_Test2;
	void LoadEmployeeNum();
	void SaveEmployeeNum();
	BOOL ReadValcRun(); //ehji 141013 
	void AutoButtonSet(BOOL i);
	void OnVision_Init();
	void SaveDB_HistoryLog(int nModuleID,int nHeadNo,int nEventCase,int nAlarmNo, CString strDiscription);
	WORD wTempDropCountIO_RUN;
 	WORD wIODropCountResult1_RUN;
	BOOL bModuleInfoStatus;
	void LoadLanguage();
	void SaveLanguage();
	void SelectLanguage();
	CN_Function *pN_Func;
	BOOL m_bVision_TimeOver;
	int Read_Count_Result();
	void DropCountVariableInit();
	int Send_Msg_2_Vision(int nKind);
	CString m_strStageAutoLog;
	CMcStatus *m_pMcStatus;
	int m_nMCNaviStatus[11];
	void McNaviStatusReport();
	int Write_2_Vision(int nKind);
	BOOL m_bCalibUpDate2[MAX_NOZZLE][MAX_MEADATA_GETCOUNT2];
	BOOL m_bCalibUpDate1[MAX_NOZZLE][MAX_MEADATA_GETCOUNT1];
	double GetSuctionDist(int head, double dSuctionWeight);
	double FlexibleGetSuctionDist(int head, double dDropWeight, int nblock);
	CPiezoDevice *m_pPiezoDevice;
	void BalStateCheck();
	BOOL m_bAllHeadDisJointDone;//disjoint flag 2011.03.23 by tskim
	BOOL m_bHeadDisJointDone[MAX_NOZZLE];//disjoint flag 2011.03.23 by tskim
	float m_dCanCda_R;
	float m_dCanCda_L;
	float m_dStgVac;
	float m_dIonCda;
	float m_dHeadCda;
	BOOL m_bUseWarmUp;
	void RmsFileChange(CString strRecipeNo);
	void RmsFileRename(CString strRecipeNo, CString strRecipeNo1);
	void RmsFileDelete(CString strRecipeNo);
	void RmsFileBuilt(CString strRecipeNo);
	void Read_RmsInformation();
	void Read_RmsInformation_EC();
	void ReadData_Rms();
	CTempOP *pTempOp;
	int g_nSupplyHeadBk;
	BOOL bSamePanelLcSupDone;
	int LCSupHeadDone[MAX_NOZZLE];
	BOOL m_bSupHeadInterLock;
	int g_nSupplyHead_Mem;
	int LCSupHead[MAX_NOZZLE];
	BOOL bErrHead[MAX_NOZZLE];
	double m_dTempLcMin2;
	double m_dTempLcMin1;
	double m_dTempLcMax2;
	double m_dTempLcMax1;
	int m_nHeadLcMin2;
	int m_nHeadLcMin1;
	int m_nHeadLcMax2;
	int m_nHeadLcMax1;

	int m_nLanguage;  //ehji 140308
	CString EmployNumber;		//ehji 141031

	double ModuleTeachHead[MAX_NOZZLE+2];		//ehji 150227
	CString  SerialModuleValue[MAX_NOZZLE];				//ehji 150227
	CString SerialModule;								//ehji 150227

	BOOL m_bBalanceCalibDo;
	BOOL m_bCalBal_Complete[MAX_BAL];
	BOOL m_bSelectCalBal[MAX_BAL];
	void SaveMeasureData(int head);
	BOOL bCheckOK;
	BOOL AssembleCheck_New(BOOL bCheckOK);
	int m_nCurrentAssemHead;
	BOOL m_bAllHeadJointDone;
	BOOL m_bHeadJointDone[MAX_NOZZLE];
	BOOL m_bAssemCheckOK[MAX_NOZZLE];
	void SaveBalLog(int BalID,CString strText);
	double m_dblMarkX[2];
	double m_dblMarkY[2];
	double m_dblAngle[2];
	long m_lScore[2];
	BOOL FindMark(int nModel);
	void ReadMarkAlignData();
	void SaveRMSLog(CString strText);
	BOOL WriteTasMCData(int ntype, int nCnt, int nBit);
	BOOL ReadPairSpecific();
	BOOL PairGlassComplete();
	void ShowTimer();
	CMCTimer *m_pMCTimer;
	void SetOffLine(BOOL bSetOffLine); // by ckh 2009.03.18 
	// by ckh 밸런스 관련 로그함수. 알람 이사으로 인해 추가함. 
	//
	int m_nMachineStatus; // 생산 준비 상태 보고를 위해 추가함. by ckh 2009.02.04
	BOOL InformationAddDelBk(int nDataName,BOOL bRead);
	BOOL InformationAddDelBk(BOOL bRead);
	int LcSupplyStatusCheck2();
	int LcSupplyStatusCheck1();
	void LcSupplyInitial2();
	void LcSupplyInitial1();
	BOOL WriteTasData(int ntype, int nCnt, int nBit);
	void SetMotorAllStop();
	void AllRecipeReport(int ntype);
	void InitializeVision();
	void SubSPDAdjustFunc(double weight_val, int DropStepCounts, int head_num_index);
	BOOL AssembleCheck(int check);
	double SubRoundAdjust(double Value);
	int GateOpenCheck(int headno);
	int GateCloseCheck(int headno);
	void FileBk();
//2007.08.08 By tskim
//Auto창 Pattern Width, Drop Offset Display..	
	int m_nRefPatNum;
	
	void OnSetData_Error();
	CString m_strDuedate;
	void SubTimerAutoSpecTime();
	CString OnCalDuedate();
	BOOL CheckReadData();
	void ReadDataCIM();
	int	LevelMaxSearch(int nP, int nH, BOOL bAddORDel);
	void FtpFileSort(int nSaveCnt);
	BOOL m_bSafetyOpened;
	BOOL m_bMaintModeClick;
	BOOL Ftp_FileDownLoad();
	void Report_Panel_Judge();
	BOOL Panel_Drop_Judge(int nTFT, int nCF);
	CString FtpFileNameFun(CString Lot_Num);
	BOOL Ftp_MDS_True();
	// FTP 관련
	CString m_strFtpServerDirText;
	void FtpServerConnect();
	BOOL SocketInit();
	CFtpFileFind *m_pFileFind;		// 서버의 File을 찾는 Class
	CFtpConnection *m_pConnection;	// 서버와의 연결 관리 class
	CInternetSession m_Session;		// 접속할 서버에 대한 정보 관리 Class

	BOOL DoTheJob(int pat, int seq);
	void BalanceModeSet();
	void ActQJ71E71TCT_Open();
//
	void Write_Information(); 
	void Write_Inform_for_Worked();
	BOOL Write_GlassData(int ntype);

	void Write_Information1(); 
	void Write_Information2(); 
	void Write_Information3();
	void Write_Information4();
	void Report_Glass_PracticalWork(WPARAM wParam, LPARAM lParam);

	void Read_Information(); 
	void Read_InformationTFBK();
	void Read_InformationAdd();
	void Read_InformationDotCancel_TFT(); 
	void Read_InformationDotCancel_CF();

//
	BOOL Judgement(double, double, double); // error % 판단..
	BOOL JudgementPlus(double, double, double); // error % 판단..
	BOOL JudgementMinus(double, double, double); // error % 판단..
	void SubAdjustFunc(double weight_val, int head_num); // error % 판단..

	//func
	void SubDisplayRecipeName();
	void SubButtonShowOrHide();
	void SaveLog(int, CString);
	//
	// comm 관련
	void SendData(short port_num,CString str);
	BOOL InitializeComm();

	CString m_strRxDataBuf1;
	CString m_strRxDataBuf2;
	CString m_strRxDataBuf3;
	CString m_strRxDataBuf4;
	CString m_strRxDataBuf5;
	CString m_strRxDataBuf6;
	CString m_strRxDataBuf7;
	CString m_strRxDataBuf8;
	CString m_strRxDataBuf9;
	CString m_strRxDataBuf10;
	CString m_strRxDataBuf11;
	CString m_strRxDataBuf12;
	//
	// variables
	CPassQa *m_pPassQa;
	CDevicePart *m_pDevice;
	CAuto *m_pAuto;
	//
	CMaint *m_pMaintMode;

	// 장치상태를 나태내는 것들..
	BOOL m_bMesuredDataDisplayEnable;	//	측정data를 표시하기를 원하면 true
	BOOL m_bMesuredDataSaveCompleted;	// 측정 data 저장이 완료된 다음에 Display 할 필요가 있으므로...
	BOOL m_bBalanceModeDispEnable;		//  Balance Mode의 변화가 생기면 true
	CString m_strMachineNo;
	CString m_strEngPass;
	int m_nAddDelCompleted1;		// FTP File을 Download가 정상적으로 완료되면 1, 정상적이지 않거나 File이 없으면 2, Download를 기다리는 동안에는 0.
	BOOL m_bAddDelCompleted2;		// 유효 Drop수 계산이 완료되었을 경우 TRUE.
	BOOL m_bAddDelCompleted3;		// ADD/DEL LEVEL 정보가 File 내에 정상적으로 있는 경우 TRUE, 이상이면 FALSE.
	BOOL m_bReportCompleted;		// 실처리 보고가 완료되었을 경우 TRUE.
	int m_nMachineNo;
	//임시//
	int SupplyBalNo;
//	int m_nSupplyCount;
	
	// CIM Data
	CString ReceiveData;
	CString ReceiveData1;
	CString ReceiveData2;
	CString Glass_add_data;
	CString Glass_add_data1;
	CString Glass_add_data24;
	CString Glass_add_data5;
	CString Glass_add_data6;
	CString Glass_Judge;
	CString Glass_Specific_Data;
	CString Glass_history;		
	CString Recipe_Number;		
	CString Glass_Type;			
	CString Lot_number;			
	CString Glass_ID;			
	CString Pre_Process_Data1;
	CString Pre_Process_Data234;
	CString Glass_Process_Data;
	CString Glass_Routing_Data;
	CString Glass_Specific_Data_Dot_Limit;

	int m_nGlassDataReadStatus; //090527

	virtual ~CP8CA_LcDispView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:

	//{{AFX_MSG(CP8CA_LcDispView)
	afx_msg void OnPaint();
	afx_msg void OnClickCmdAppExit();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClickEngMode();
	afx_msg void OnClickCmdAuto();
	afx_msg void OnClickCmdManual();
	afx_msg void OnClickCmdRecipeSet();
	afx_msg void OnClickCmdCalib();
	afx_msg void OnClickCmdPass();
	afx_msg void OnClickCmdTeach();
	afx_msg void OnClickCmdDataEditor();
	afx_msg void OnClickCmdFileMng();
	afx_msg void OnClickCmdErrChk();
	afx_msg void OnClickCmdErrReset();
	afx_msg void OnCommMscomm1();
	afx_msg void OnCommMscomm2();
	afx_msg LONG OnPosChkMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnGetData(WPARAM wParam,LPARAM lParam);
	afx_msg LONG OnMeasuredDataSave(WPARAM wParam,LPARAM lParam);
	afx_msg long OnErrorDisp(WPARAM wParam, LPARAM lParam);
	afx_msg long OnGetCimData(WPARAM wParam, LPARAM lParam);
	afx_msg long OnGlassInformSave(WPARAM wParam, LPARAM lParam);	
	afx_msg long OnRunThread(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTasData(WPARAM wParam,LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCommMscomm3();
	afx_msg void OnCommMscomm4();
	afx_msg void OnDestroy();
	afx_msg void OnChkIonOn();
	afx_msg void OnClickMaintMode();
	afx_msg void OnOnCommMscomm5();
	afx_msg void OnOnCommMscomm6();
	afx_msg LONG OnCanisterDataSave(WPARAM wParam);
	afx_msg void OnOnCommMscomm7();
	afx_msg void OnOnCommMscomm8();
	afx_msg void OnOnCommMscomm9();
	afx_msg void OnOnCommMscomm10();
	afx_msg void OnClickCheckMachine();
	afx_msg void OnClickCheckMachine2();
	afx_msg void OnClickCmdErrorReset();
	afx_msg void OnDblClickCmdAppExit(LPDISPATCH Cancel);
	afx_msg void OnClickReportTas();
	afx_msg void OnOnCommMscomm11();
	afx_msg void OnOnCommMscomm12();
	afx_msg void OnDblClickLabelVersion(LPDISPATCH Cancel);
	afx_msg void OnDblClickCmdModeLanguage(LPDISPATCH Cancel);
	afx_msg void OnDblClickLabelModelName(LPDISPATCH Cancel);
	afx_msg void OnDblClickCmdModeStatuscontinue(LPDISPATCH Cancel);
	afx_msg void OnDblClickCmdModePddownstatemode(LPDISPATCH Cancel);
	afx_msg void OnClickCmdMotorload();
	afx_msg void OnClickCmdPendant();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in P8CA_LcDispView.cpp
inline CP8CA_LcDispDoc* CP8CA_LcDispView::GetDocument()
   { return (CP8CA_LcDispDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_P8CA_LcDispVIEW_H__57EEDCD8_7B2B_4A58_800B_F37845CDBE04__INCLUDED_)

#ifndef __THREADDEFINE_H__
#define __THREADDEFINE_H__

typedef struct tagTREADPARAM
{
	CP8CA_LcDispView	*pLcView;
	CP8CA_LcDispDoc *pLcDoc;
}THREADPARAMS;

CCriticalSection g_critical;
CCriticalSection g_critTCP;
//CEvent g_BalanceCheckEvent;
CCriticalSection g_critical1;
CCriticalSection g_critical2;
CCriticalSection g_critical3;

// 변수선언
THREADSTAGE ThreadStage;

THREADBALANCE ThBal[MAX_BAL];
//
THREADIO ThreadIO;
THREADORIGIN ThreadOrigin;
THREADSUPPYBAL ThreadSupplyBal1;
THREADSUPPYBAL ThreadSupplyBal2;

THREADVISION ThreadVisionDetect; //VISION DETECT OVERLAP

//2011.04.03 by tskim
BALSTATE Balstate;
//
//
DROPINFO Drop_Info;
// PLC
CIM_DATA CIM_Information;
//
CIM_GLASS_INFORM_SAVE Glass_Inform[2][10];	// 0:TFT, 1:CF
//
ERROR_INFORM Error_Information[50];

// 함수선언
UINT ThreadVisionDetectFunc(LPVOID pParam);
UINT ThreadStageFunc(LPVOID pParam);
UINT ThreadIOFunc(LPVOID pParam);
UINT ThreadOriginAllFunc(LPVOID pParam);

UINT ThreadBalanceFunc1(LPVOID pParam);
UINT ThreadBalanceFunc2(LPVOID pParam);
UINT ThreadBalanceFunc3(LPVOID pParam);
UINT ThreadBalanceFunc4(LPVOID pParam);
UINT ThreadBalanceFunc5(LPVOID pParam);
UINT ThreadBalanceFunc6(LPVOID pParam);
UINT ThreadBalanceFunc7(LPVOID pParam);
UINT ThreadBalanceFunc8(LPVOID pParam);
//
UINT ThreadCalibFunc1(LPVOID pParam);
UINT ThreadCalibFunc2(LPVOID pParam);
UINT ThreadCalibFunc3(LPVOID pParam);
UINT ThreadCalibFunc4(LPVOID pParam);
UINT ThreadCalibFunc5(LPVOID pParam);
UINT ThreadCalibFunc6(LPVOID pParam);
UINT ThreadCalibFunc7(LPVOID pParam);
UINT ThreadCalibFunc8(LPVOID pParam);
//
UINT ThreadSupplyBalFunc1(LPVOID pParam);
UINT ThreadSupplyBalFunc2(LPVOID pParam);

// ThreadStageFunc에서  2개의 함수로 구분한다.
void StageAuto(LPVOID pParam);
void StageManual(LPVOID pParam);
/// PosChk 스레드 /////////////////////////////////////////////////////
// by ckh 
UINT ThreadPositionCheck(LPVOID pParam);

UINT BalanceFunc(UINT BalID, LPVOID pParam);

UINT ThreadBalanceCalibration1(LPVOID pParam);
UINT ThreadBalanceCalibration2(LPVOID pParam);
UINT ThreadBalanceCalibration3(LPVOID pParam);
UINT ThreadBalanceCalibration4(LPVOID pParam);
UINT ThreadBalanceCalibration5(LPVOID pParam);
UINT ThreadBalanceCalibration6(LPVOID pParam);
UINT ThreadBalanceCalibration7(LPVOID pParam);
UINT ThreadBalanceCalibration8(LPVOID pParam);

UINT ThreadBalanceCalibrationCom(UINT BalID, LPVOID pParam);

UINT HeadCalibFunc(UINT BalID, LPVOID pParam);

//	필요한 전역변수 선언..
BOOL g_SuppyDropReport;		//ehji 150513
int g_nLcSupplyAllcount[MAX_NOZZLE];	//ehji 150505
long global_Sequence_Count;
BOOL g_bCalibrationStatus[MAX_NOZZLE]; //120419 by shlee
UINT g_nGlassCount;	
int g_nGlassCountScanCycle;		//ehji 140918
BOOL g_Nmodulepara;		//ehji 150110
BOOL g_MeasureCountError;	//ehji 150527
BOOL g_bStatusError[MAX_NOZZLE];			//ehji 150529
int g_MeasureheadError;		//ehji 150527
BOOL g_DropSensorCheck;		//ehji 150322
BOOL g_WarmupPTPmode;		//ehji 150527
UINT g_nTotalGlassCount;
BOOL g_bCommunicationType;
BOOL g_bActiveTCP;
BOOL g_bReportWorkTest;
int g_nReportCount;
int g_nReportCountBK;
BOOL g_bAdjCalBaseCompl[12];
long g_nDrop_Repeat_Count;
BOOL g_bSpecCount;
UINT g_nDoorOpen[5];//Door Cim 보고 관련... Door yamary
BOOL g_bIsGlass;
//lbg 6라인 구별..
BOOL g_Is6LineMC;
UINT	g_nGlassAlignCount;//Mark Align Count 2010.03.22 by tskim


// PLCAddr.DAT  : Disp.가 Read 하는 영역....
// 장비 A,B 호기에 따라 같은 영역에서 읽는 Address가 다르므로 변수 처리한다... 
short BitTFReceiveIF;	// A혹은 C
short BitTFSendIF;		// B혹은 D
short nCIMPanelInfoPos;		
//short BitProcKeyDataSendReq;	
//short BitPanelNormalInfo;		
//short BitPanelSpecialInfo;		

int LinePattern_Seq_count;

//
#endif

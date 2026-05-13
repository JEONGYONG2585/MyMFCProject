#ifndef __THREADDATA_H__
#define __THREADDATA_H__

#define MAX_PANEL		300

typedef struct tagDROP_INFO
{
	BOOL drop_onoff[MAX_NOZZLE]; // current drop on-off
	BOOL head_onoff[MAX_NOZZLE]; // current head on-off
	BOOL manu_head_job[MAX_NOZZLE]; // manual에서 이용되는 head on-off

	//by shin//2008.07.23//
	double m_dAdjustAbsPos[MAX_NOZZLE];
	double m_dBasePos[MAX_NOZZLE];
	double m_dAssemPos[MAX_NOZZLE];
	double m_dFullBasePos[MAX_NOZZLE];	
	double m_dRemoveBubblePos[MAX_NOZZLE];
	double m_dDistPerDrop[MAX_NOZZLE];	
	int m_nDropPerCycle[MAX_NOZZLE];
	int m_nSuctionSpeed[MAX_NOZZLE];
	int m_nDischargeSpeed[MAX_NOZZLE];
	double m_dMMPerVolume[MAX_NOZZLE];
	int m_nSuctionOffset[MAX_NOZZLE];
	int m_nGateOffset[MAX_NOZZLE];
	double m_dSucOffSpeed[MAX_NOZZLE];
	int m_nRemoveBubbleTimes1[MAX_NOZZLE];
	int m_nRemoveBubbleTimes2[MAX_NOZZLE];

	double m_dRemainPulse[MAX_NOZZLE];
	double m_dRemainPulse_Flex[MAX_NOZZLE][150];

	//150527 ehji
	double m_dMaxDistPerDrop;
	double m_dMinDistPerDrop;
	double m_dSumDistPerDrop;
	double m_dAverageDistPerDrop;
	double m_dDistPerDropPercent[MAX_NOZZLE];

	//by shin//2013.01.15//n-type add//
	double m_dNRemoveBubblePos[MAX_NOZZLE];
	double m_dNDischargeSpeed1[MAX_NOZZLE];
	double m_dNDischargeSpeed2[MAX_NOZZLE];
	int m_nNRemoveBubbleTimes[MAX_NOZZLE];
	int m_nNOverTime[MAX_NOZZLE];

	//by shin//2013.01.15//n-type add//
	//n-type 기포제가에만 사용하는 변수로 사용자가 직접 file로 저장해놓고..s/w에서는 읽어서 사용만 한다.
	//1변수로 12head 덮어쓴다...//
	//clean 동작은 총 4step으로 구분되며 각각 하기 파라미터를 이용한다.//배열로 하자...//
	//step1
	int m_nNCleanStep[4];
	int m_nNCleanStepHz[4];
	int m_nNCleanStepAmp[4];
	int m_nNCleanStepNegOffset[4];
	double m_dNCleanStepDuty[4];
	int m_nNCleanStepFallingEdge[4];
	double m_dNCleanStepSpeed[4];	

	double m_dDistPerDropAutoNtype[MAX_NOZZLE];
	double m_dDistPerDropAutoFlexibleNtype[MAX_NOZZLE][150];
	double m_dNtypeDischargeLineDropSpeed[MAX_NOZZLE][150];

//2014.10.18 by tskim 
	int m_nDistPerDropPanelNO[MAX_NOZZLE][150];

//2011.05.12 by tskim
	double m_dFlexibeDistPerDrop[MAX_NOZZLE];
	double m_dRealFlexibeDistPerDrop[MAX_NOZZLE][150];



	double m_dCalibInterlockPulse[MAX_NOZZLE];
	double m_dCalibInterlockFullBase[MAX_NOZZLE];
	int nMeasurePerDrop[MAX_NOZZLE];

	long lSupplyAcctime;
	long lSupplyDectime;
	long lSupplyStartSpeed;
	long lDegasAcctime;
	long lDegasDectime;
	long lDegasStartSpeed;

	BOOL m_bDegasNtypeStatus[MAX_NOZZLE];  // TRUE : OK , FALSE : NG// TRUE : OK , FALSE : NG

	BOOL bAdjustComplete[MAX_NOZZLE];
	BOOL bMeasureComplete[MAX_NOZZLE];//2011.12.23
	
	int m_nNAgingCount; //2015.08.29 by tskim N-Aging Count
	double m_dDistPerDrop_ForPulseCheck[MAX_NOZZLE][256]; //Target Pulse 비교용 2015.08.29 by tskim Max pattern 256
//2015.04.29 by tskim 원재료 전산화Add Dummy Drop 보고 
	int nDummyDropComplete[MAX_NOZZLE];
	int m_nNAgingCycle; //20210224 him n-aging cylce
} DROPINFO;

typedef struct tagTHREAD_STAGE
{
	int		JobStep;
	int		JobModeSelection;
	BOOL	JobOption; // step(0) , continuous(1)
	BOOL	StepStop;
	BOOL	ExitFlag;
	int		JobFlag; // auto(0) , manual(1)
	UINT	nPassTime;	//	pass mode에서 delay time
	UINT	nPassTimeCount; // pass mode에서 delay time counting..
	// manual
	char	ManualCode;
	int		JogAxis; // Y,K,A,S,Z
	int		ManualJobStep; // job stp 구분..
	int		AdjustInitCode; // adjust init에서 동작 구분..
	int		PtpMoveCode; // manual ptp에서 동작 구분..
	CString strMsg1;
	CString strMsgBK;
	int		MeasureMoveStep;
	int		nHeadSelected;	// Pump 기포제거에서 선택된 Head의 저장
//
	int		HomeSearchMotor;
	int		HomeSearchBD;
	int		TotalTime;

	BOOL	Recipe_Change;

	// warm up information
	double	WarmUpSpeedS;
	double	WarmUpSpeedY;
	double	WarmUpSpeedK;
	int		WarmUpTimes;
	BOOL	WarmUpAxis[2]; // 0:S축,1:K축
	BOOL    WarmUpStop;
	int     WarmUpSettingTime;
	BOOL    WarmUpDummyDrop;
	int     WarmUpCount;
	BOOL	WarmUpContiJob;

//2014.03.10 by tskim //150521 HSN PTP Warmup추가
	double  dWarmUp_KAxis_StepMove_Length;
	int		nWarmUp_KAxis_StepMove_Times;
	BOOL	bWarmUpAxisStepMove[2];//0:S축 1: K축

	// machine status information
	BOOL	bEmergencyStop;
	BOOL	bLimitDetect;
	UINT	nHomeEnableCount;
	BOOL	bMachineRunning;
	BOOL	bUseInterLock;
	BOOL	bDropCountDisp;
	int		nDropStatus;	// 0 : 토출전 Glass, 1: 토출 중 Glass, 2: 토출 완료 Glass
	BOOL	OffLineMode;

	BOOL	bGlassBothways;	// ture : 동시 취출, false : 단일 배출
	int		nLCDetectedCount[MAX_NOZZLE];
	BOOL	bLCDectectedMem[MAX_NOZZLE];
	int		nAlarmcode;
	int		nExistMMG;
	int		nDropDirection;//0:FRONT,1:REAR	
	int		nMoveHeadNum;//2010.03.04 by tskim;
	double  dAlignOffsetX;
	double  dAlignOffsetY;

	BOOL	bVisionDrection; //TRUE : TTB , FALSE : BTT
} THREADSTAGE;

typedef struct tagTHREAD_BALANCE
{
	int	JobStep;
	int ErrCode;
	int nLoopCount; // 반복횟수를 저장
	int nHeadSelected;	//  설정된 head번호를 저장
	CString strMsg1;
//
	int nBalanceStableCnt; // 0,1,2,99
	double dFirstData;
	double dSecondData;
	double dRecentData;
	BOOL bBalanceStableFirstFlag;
	BOOL bBalanceStableSecondFlag;
//
//	BOOL bBalanceEnd;
//	BOOL bBalanceNormalEnd;
	BOOL TimerOver;
	int nTimerCount;
//
	BOOL ExitFlag;
	BOOL bRunning;
	BOOL bAdjust;
	
	int nBalanceMode;
	int nBalanceRifLevel;
	int	nBalanceUse;

	BOOL bAutoMeaHeadDispCompl;
	CString strBalStatus;
	int MeasuringTimeCount;
//2011.04.27 by tskim
	int nAbnormalCnt;
	BOOL bAbnormalSpecOut;
	CString strMsg2;
	int nAbnormalDelayCnt;
	BOOL bNParaWriting;

	int nBalanceType;// 0: 메틀러, 1: 사토리우스
	CString strBalCmd;
//  
} THREADBALANCE;

typedef struct tagTHREAD_IO
{
	BOOL ExitFlag;
	int JobStep; 
	BOOL bBalDoorMoved[4];
} THREADIO;

typedef struct tagTHREAD_ORIGINALL
{
	BOOL	ExitFlag;
	BOOL	bRunning;
	int		JobStep; 
	int		nHide;
	int		nEndMessage;
	CString strMsg1;
	CString strMsg2;
} THREADORIGIN;

typedef struct tagTHREAD_SUPPLY_BAL
{
	int	JobStep;
	int ErrCode;
	int nLoopCount; // 반복횟수를 저장
	int nHeadSelected;	//  설정된 head번호를 저장
	CString strMsg1;
	int nBalanceStableCnt; // 0,1,2,99
	double dFirstData;
	double dSecondData;
	double dRecentData;
	BOOL bBalanceStableFirstFlag;
	BOOL bBalanceStableSecondFlag;
	BOOL TimerOver;
	int nTimerCount;
	BOOL ExitFlag;
	BOOL bRunning;
	BOOL bErrorFlag;
} THREADSUPPYBAL;

typedef struct tagTHREAD_VISION_DETECT
{
	int	JobStep;
	int ErrCode;
	CString strMsg1;
	BOOL TimerOver;
	int nTimerCount;
	BOOL ExitFlag;
	BOOL bRunning;
	BOOL bErrorFlag;

	BOOL bWritingFlag; 
	BOOL bLineDropping; 
	BOOL bResultCheckFlag;
	
	BOOL bPrepareBeforeLineDrop; 

	BOOL bWritingFinish; 
	BOOL bLineDroppingFinish; 
	BOOL bResultCheckFinish; 

	int nErrorCode; 

	BOOL bUseVisionThread; 

//161214 VISION PART SCAN //JYKIM 161219 PART SCAN 180K
	int nLineDropStatus; // 0 : START
					     // 1 : MOTIONNING
						 // 2 : FINISH
	
} THREADVISION;

// plc관련..
typedef struct tagCIM_DATA
{
	CString GlassDataEqui;
	CString GlassDataPair;
//
	CString Recipe_No;
	CString Recipe_Number;		// 0x21B0
	CString Glass_Type;			// 0x21B1
	CString Lot_number;			// 0x21B2
	CString Glass_ID;			// 0x21B3 ~ 0x21B9
	CString Glass_Judge_Code12;	// 0x21BA
	CString Glass_add_data1;	// 0x21BB
	CString Glass_add_data23;	// 0x21BC~0x21BD
//	CString Glass_history;		// 0x21BE~0x21BF
	CString Glass_history1;		// 0x21BE
	CString Glass_history2;		// 0x21BF

	CString Pair_Lot;
	CString Pair_Slot;
	CString Glass_Judge;
	CString Glass_Judge_ASC;
	CString GlassID_ASC;	
	CString Glass_Specific_Data;
	CString Pre_Process_Data1;
	CString Pre_Process_Data234;
	CString Glass_Process_Data;

	CString Glass_add_data24;
	CString Glass_add_data5;
	CString Glass_add_data6;

	CString Job_Start_Time; // Processing Time 
	CString Glass_Loading_Time;	// Loding 완료 
	CString Glass_UnLoading_Time; // UnLoading 완료 // by ckh 2009.02.23 추가함. 
	CString Drop_Start_Time;// Drop Start			// by ckh 2009.02.23 추가함. 
	CString Drop_End_Time;	// Drop End				// by ckh 2009.02.23 추가함. 
	int     nWorkTime; //Drop Tact Time

	CString Panel_Information[2];	// CIM으로 부터 오는 N,G data String. 0:TFT, 1:CF
	CString Panel_Inform_Convert[2];	// CIM으로 부터 오는 N,G data String. 0:TFT, 1:CF
//	int Panel_NG_Mark[2][88];		// 0:TFT,1:CF... CIM으로 부터 오는 N,G의 내용을 포함한 data로 치환한 배열. 0x2020 ~ 0x202B

//	int Panel_NG_Mark[2][100];			// 0:TFT,1:CF... CIM으로 부터 오는 N,G의 내용을 포함한 data로 치환한 배열. 0x2020 ~ 0x202B
//	int Panel_NG_Mark_TFT[2][100];			// 0:Main,1:Sub... CIM으로 부터 오는 N,G의 내용을 포함한 data로 치환한 배열. 0x2020 ~ 0x202B
//	int Panel_NG_Mark_CF[2][100];			// 0:Main,1:Sub... CIM으로 부터 오는 N,G의 내용을 포함한 data로 치환한 배열. 0x2020 ~ 0x202B
	
	int Panel_NG_Mark[2][MAX_PANEL];			// 0:TFT,1:CF... CIM으로 부터 오는 N,G의 내용을 포함한 data로 치환한 배열. 0x2020 ~ 0x202B
	int Panel_NG_Mark_TFT[2][MAX_PANEL];			// 0:Main,1:Sub... CIM으로 부터 오는 N,G의 내용을 포함한 data로 치환한 배열. 0x2020 ~ 0x202B
	int Panel_NG_Mark_CF[2][MAX_PANEL];			// 0:Main,1:Sub... CIM으로 부터 오는 N,G의 내용을 포함한 data로 치환한 배열. 0x2020 ~ 0x202B
	
	int Panel_NG_MarkSet[25];		// TFT와 CF Panel 정보에 대한 조합을 Drop을 할 것인가 말 것인가를 사용자 정의에 의해 설정한 값을 저장하는 배열.
	CString Panel_Inform_AllManual; // Panel 정보를 수동으로 입력받는 변수.
/******* 예제 **********
//	  |  G   Q   N   W  *
------+-----------------
//  G |  1   1   1   1  * <= Panel_NG_MarkSet[]
//	Q |  1   0   0   0  *
//	N |  1   0   0   0  *
//	W |  1   0   0   0  *
//**********************/

//
	CString Glass_JudgeCode_Main;	// Add/Del Data 저장. 0x2040 ~ 0x2057
	CString Glass_JudgeCode_Sub;	// Add/Del Data 저장. 0x2060 ~ 0x2077

//	int Add_Del_Number[88];				// CIM으로 부터 오는 Add/Del Number를 순차적으로 저장하는 변수. 
	int Add_Del_Number[MAX_PANEL];				// CIM으로 부터 오는 Add/Del Number를 순차적으로 저장하는 변수. 
	int Add_Del_Main[MAX_PANEL];
	int Add_Del_Sub[MAX_PANEL]; //2013.12.04 by tskim Main,sub를 각각300면취로....추후 MMG Model 운용시 Panel정보 영역 및 최대 면취(총600면취가 되므로..) 고려 해야 함..
	int Worked_Report_Add_Del[MAX_PANEL];		// 실처리된 ADD/DEL Level 값을 CIM의 특정 영역에 써 주기 위해 재배열 시킨 변수.
	int DropPoint[MAX_PANEL];					// CIM에 보고해야 할 Drop 수를 저장하는 순차적 배열.
	int Worked_Report_DropPoint[MAX_PANEL];	// CIM에 보고해야 할 Drop 수를 CIM의 특정 영역에 써 주기 위해 재배열 시킨 변수.	
//	int Worked_Report_Add_Del[88];		// 실처리된 ADD/DEL Level 값을 CIM의 특정 영역에 써 주기 위해 재배열 시킨 변수.
//	int DropPoint[88];					// CIM에 보고해야 할 Drop 수를 저장하는 순차적 배열.
//	int Worked_Report_DropPoint[88];	// CIM에 보고해야 할 Drop 수를 CIM의 특정 영역에 써 주기 위해 재배열 시킨 변수.	
	int Panal_Max_Num;
	int SubPanalNo_Start;

	CString LotSlot_No;
	CString LotSlot_No_Pair;
	CString LotSlot_Dec;

//
//	CString Recipe_Number_Pair;		// 0x21A0
//	CString Glass_Type_Pair;		// 0x21A1
	CString Lot_number_Pair;		// 0x21A2 //이전 코드//
	CString Glass_ID_Pair;			// 0x21A23 ~ 0x2029
//	CString Glass_Judge_Code12_Pair;// 0x21A3~0x21A9
//	CString Panel_Information_Pair;	// 0x21AB~0x21AF

	CString LC_Amount[MAX_NOZZLE][2];
	CString sBarcodeData[MAX_NOZZLE];		// 나중에 Barcode 추가하자..
	CString dMeasured1[MAX_NOZZLE];	
	CString dMeasured2[MAX_NOZZLE];

// 판정값 보고용.
	CString MeasuredResult[MAX_PANEL];	// Panel 별 "4" : (-), "5" : (정상), "5" : (+)
//	CString MeasuredResult[96];	// Panel 별 "4" : (-), "5" : (정상), "5" : (+)
	int MeasuringNum;			// 측정 번호 (측정값에 대한 ID로 보면 된다.)
	int nDotting_Grupe_Num;		// by ckh 2009.02.25 , 현재 Glass Dotting Grupe Num, 토출 전 측정시는 MeasuringNum와 같이 갱신되나, 토출 후 측정시에는 현재 값을 유지함. Glass 투입시 갱신함.
	int GlassSendCount;			// 현재 배출하고 난 후 측정값 보고를 하지 않은 Glass의 수를 저장한다.

// FTP 파일 이름
	CString FtpFileName;

//2007.08.09 By tskim
//Glass Slot Number 추가..
	CString Slot_number;
	CString Slot_number_pair;
	
	CString sAlarm_Code;
	int		nAlarm_Code;
	BOOL	bAlarm_ON;
	CString sWarning_Code;
	int		nWarning_Code;
	BOOL	bWarning_ON;
	CString sCanisterTypeData[2];
	int nRmsDropPoint[MAX_PANEL];					// RMS 보고시 사용하는 C00.
	int nRms_Worked_Report_DropPoint[MAX_PANEL];   // RMS 보고시 사용하는 C00.
//2009.11.23 by tskim
	BOOL bNGCell[MAX_PANEL];
	CString NGScrapData;
//	CString NGScrapData_TR;

	double dPStartPosData[50][MAX_NOZZLE];
	double dPEndPosData  [50][MAX_NOZZLE];
} CIM_DATA;


typedef struct tagGlassInform
{
	CString Glass_ID;
	CString Lot_number;
	CString Slot_number;
	CString Panel_Information;

} CIM_GLASS_INFORM_SAVE;

typedef struct tagFtpFileInform
{
	CString sFileName;
	unsigned long lAccessTime;
} FTP_FILE_INFORM;

typedef struct tagErrorInform
{
	int m_nCode[3];
	CString strTime;
} ERROR_INFORM;


//2011.04.03 by tskim Balance Status Check
typedef struct tagBalState
{
	BOOL bBalstatecheckDone;
	int nStateCheckDelaycnt;
	BOOL bBalNormalState[MAX_BAL];
	BOOL bAllBalStateNormal;
	int nMeasuringDelay;
} BALSTATE;

#endif
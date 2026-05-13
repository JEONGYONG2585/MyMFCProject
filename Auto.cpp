// Auto.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"
//
#include "Auto.h"
#include "NormalMsg.h"
#include "AutoIF.h"
#include "AdjustDraw.h"
#include "ScrapMsg.h"
#include "GlassID.h"
//
#include "ErrListAuto.h"
#include "SpecCount.h"
#include "ErrMsg.h"

#include "GlassDataChange.h"
#include "Grease.h"
#include "AlignDraw.h"
#include "N_Vision.h"
#include "Piezostatus.h"

//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//2007.07.17 By tskim
//Target값 측정값 다를 경우 알람...
extern BOOL _bSpecOver[MAX_NOZZLE];

//2007.09.13 by tskim
//강제 측정 주기 변수 추가.
//extern int m_nForcedMeasInterval;

//2007.11.09 By tskim
//반출 대기 시간 1000초 경과 할 경우 알람 발생...
extern BOOL m_bUnloadingTimeOver;
//
extern BOOL bSupplyComplete;
extern BOOL bSupplyStart;
extern BOOL bSupplyTimerFlag;
//extern BOOL bTimeCheck; 

extern short RobotSend;
extern short RobotReceive;
extern BOOL g_bGlassDataChange;
extern BOOL g_bLcSupply;
extern int g_nSupplyHead;
extern BOOL g_bMotorMoveView;
extern double g_dSupplyTime;
extern double m_dAdjustError;
extern BOOL g_bCanster1WeightAlarm;
extern BOOL g_bCanster2WeightAlarm;

extern BOOL g_bTimeOverT3;
extern BOOL g_bPumpOringFlag;
extern BOOL g_bDropStartLimit;
extern BOOL g_bDropStartLimitStatus;		//ehji 141205
extern BOOL g_bLoadUnLoadSensorMode; 
extern BOOL g_bJobMovePos; 
extern BOOL g_bLcSupplyUsed;
extern BOOL g_bValcRun;	// 141013 ehji 

extern BOOL m_bSWCheck;
extern int m_nGraphSel;
extern BOOL m_bNGScrap;//2010.04.05 by tskim NG Scrap
extern BOOL g_bRemoteControl;

extern CN_Vision _NVision;
extern BOOL g_bMultiTargetMeas;
extern BOOL g_bInitialShot;


extern BOOL g_bPdReqTimeOver;				//141026 PD DOWN : PDDOWN REQUESE 상태 변수
extern BOOL g_bPdDownReq;					//141026 PD DOWN : PD DOWN 상태 변수
extern BOOL g_bPdDownRelease;				//141026 PD DOWN : PD DOWN RELEASE 상태 변수

extern BOOL  g_bAceeptSettingStatus; //141113 GLOBAL 변수 선언 

extern UINT  g_nInitMeasureGlassCount;//2014.11.27 by tskim

//2014.11.27 by tskim  RMS Add
extern BOOL g_bRMS_AddData_ReportOnOff;//False : RMS Add off, TRUE : RMS Add On
//2014.11.27 by tskim Over Clock Use
extern BOOL g_OverClock_Use;//False : No use, True : use

extern double m_dPAxis_SuctionOffset[MAX_NOZZLE];//2014.12.01 by tskim Dummy Big Drop 방지 Suction 동작 테스트 용
extern double m_dPAxis_SuctionOffset_Temp[MAX_NOZZLE];//2014.12.01 by tskim Dummy Big Drop 방지 Suction 동작 테스트 용

extern BOOL g_bMotorLoad_Report;//false : Report off, true: Report on // 2015.02.10 by tskim Motor Load

extern BOOL g_bLogWriteOff; 


/////////////////////////////////////////////////////////////////////////////
// CAuto dialog


CAuto::CAuto(CWnd* pParent /*=NULL*/)
	: CDialog(CAuto::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAuto)
	//}}AFX_DATA_INIT
	ThreadStage.nPassTimeCount=0;
	ThreadStage.JobStep = 200; 
	global_Sequence_Count=0;
	m_nAutoTimerCount = 0;
	m_nAutoStepTimerCount=0;
	m_ulAutoCycletime=0;
	m_nJobStep = 0; m_nJobStepBK=200;
	m_nSupplyTimeCur = 0; m_nSupplyTimePrev = 0;
	m_nSupplyTimeCount = 0;
	m_nOldTempSpanTime = 0;
	m_bTimeCheck = FALSE;
	//`
	for(int i=0;i<7;i++) m_nAutoTime[i] = 0;
	m_pictureholder.CreateFromBitmap(IDB_SKIP);
	//
	for(i=0; i<MAX_NOZZLE;i++) m_nLoopCountBK[i]=0;
	m_strTime="";
	for(i=0; i<MAX_NOZZLE;i++) m_pQnty[i]=NULL;
	m_bAdjustErrRange = FALSE;
	LinePattern_Seq_count = 0;
	m_bSWCheck = false;

//2014.04.21 by tskim TPD
	for(i=0; i<MAX_NOZZLE;i++)
		nTempCount_S[i] = 100+i;
	nTempCount_K[0]=0;
	nTempCount_K[1]=50;
}
CAuto::~CAuto()
{
	//by shin//2009.06.16//
//	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
//	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

//	pDoc->m_bAutoManualFlag =FALSE; 
}

void CAuto::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAuto)
	DDX_Control(pDX, IDC_ADJUST_IN_AUTO, m_ctrlAdjustInAuto);
	DDX_Control(pDX, IDC_MSFLEXGRID_AUTO_MEADATA, m_ctrlAutoMeaData);
	DDX_Control(pDX, IDC_LABEL_RECIPE_NAME, m_ctrlRecipeName);
	DDX_Control(pDX, IDC_AUTO_DROP, m_ctrlAutoDrop);
	DDX_Control(pDX, IDC_AUTO_UNLOAD, m_ctrlAutoUnLoad);
	DDX_Control(pDX, IDC_AUTO_CONTINUOUS, m_ctrlAutoContinuous);
	DDX_Control(pDX, IDC_AUTO_SCRAP, m_ctrlAutoScrap);
	DDX_Control(pDX, IDC_AUTO_STEP, m_ctrlAutoStep);
	DDX_Control(pDX, IDC_CMD_RETURN, m_ctrlReturn);
	DDX_Control(pDX, IDC_LABEL_MESSAGE, m_ctrlMessage);
	DDX_Control(pDX, IDC_PROGRESS00, m_ctrlProgress00);
	DDX_Control(pDX, IDC_PROGRESS01, m_ctrlProgress01);
	DDX_Control(pDX, IDC_PROGRESS2, m_ctrlProgress02);
	DDX_Control(pDX, IDC_PROGRESS3, m_ctrlProgress03);
	DDX_Control(pDX, IDC_PROGRESS4, m_ctrlProgress04);
	DDX_Control(pDX, IDC_AUTO_LOAD, m_ctrlAutoLoad);
	DDX_Control(pDX, IDC_AUTO_MEASURE, m_ctrlAutoMeasure);
	DDX_Control(pDX, IDC_AUTO_DUMMYDROP, m_ctrlAutoDummyDrop);
	DDX_Control(pDX, IDC_LABEL_PATTERN_DISPENSING, m_ctrlPatternDispensing);
	DDX_Control(pDX, IDC_LABEL_NG_DROPPING, m_ctrlNgPanelSkip);
	DDX_Control(pDX, IDC_LABEL_RECIPENUM_COMPARE, m_ctrlRecipeNumCompare);
	DDX_Control(pDX, IDC_MSFLEXGRID_AUTO_WORKDATA, m_ctrlAutoWorkData);
	DDX_Control(pDX, IDC_MSFLEXGRID_DROP_CONDITION, m_ctrlAutoDropCondition);
	DDX_Control(pDX, IDC_MSFLEXGRID_DOTNUM, m_ctrlDotNum);
	DDX_Control(pDX, IDC_MSFLEXGRID_GLASS_PROCESS, m_ctrlGlassProcess);
	DDX_Control(pDX, IDC_ON_OFF_LINEMODE, m_ctrlOffLineMode);
	DDX_Control(pDX, IDC_LABEL_BAL_USE1, m_ctrlBalUse1);
	DDX_Control(pDX, IDC_LABEL_BAL_USE2, m_ctrlBalUse2);
	DDX_Control(pDX, IDC_LABEL_BAL_USE3, m_ctrlBalUse3);
	DDX_Control(pDX, IDC_LABEL_BAL_USE4, m_ctrlBalUse4);
	DDX_Control(pDX, IDC_LC_QUAN_H1, m_ctrlLCQntyDisp1);
	DDX_Control(pDX, IDC_LC_QUAN_H10, m_ctrlLCQntyDisp10);
	DDX_Control(pDX, IDC_LC_QUAN_H11, m_ctrlLCQntyDisp11);
	DDX_Control(pDX, IDC_LC_QUAN_H12, m_ctrlLCQntyDisp12);
	DDX_Control(pDX, IDC_LC_QUAN_H2, m_ctrlLCQntyDisp2);
	DDX_Control(pDX, IDC_LC_QUAN_H3, m_ctrlLCQntyDisp3);
	DDX_Control(pDX, IDC_LC_QUAN_H4, m_ctrlLCQntyDisp4);
	DDX_Control(pDX, IDC_LC_QUAN_H5, m_ctrlLCQntyDisp5);
	DDX_Control(pDX, IDC_LC_QUAN_H6, m_ctrlLCQntyDisp6);
	DDX_Control(pDX, IDC_LC_QUAN_H7, m_ctrlLCQntyDisp7);
	DDX_Control(pDX, IDC_LC_QUAN_H8, m_ctrlLCQntyDisp8);
	DDX_Control(pDX, IDC_LC_QUAN_H9, m_ctrlLCQntyDisp9);
	DDX_Control(pDX, IDC_LC_H1, m_DropUse1);
	DDX_Control(pDX, IDC_LC_H2, m_DropUse2);
	DDX_Control(pDX, IDC_LC_H3, m_DropUse3);
	DDX_Control(pDX, IDC_LC_H4, m_DropUse4);
	DDX_Control(pDX, IDC_LC_H5, m_DropUse5);
	DDX_Control(pDX, IDC_LC_H6, m_DropUse6);
	DDX_Control(pDX, IDC_LC_H7, m_DropUse7);
	DDX_Control(pDX, IDC_LC_H8, m_DropUse8);
	DDX_Control(pDX, IDC_LC_H9, m_DropUse9);
	DDX_Control(pDX, IDC_LC_H10, m_DropUse10);
	DDX_Control(pDX, IDC_LC_H11, m_DropUse11);
	DDX_Control(pDX, IDC_LC_H12, m_DropUse12);
	DDX_Control(pDX, IDC_ADD_GLASS_H1, m_ctrlLCSensorDisp1);
	DDX_Control(pDX, IDC_ADD_GLASS_H2, m_ctrlLCSensorDisp2);
	DDX_Control(pDX, IDC_ADD_GLASS_H3, m_ctrlLCSensorDisp3);
	DDX_Control(pDX, IDC_ADD_GLASS_H4, m_ctrlLCSensorDisp4);
	DDX_Control(pDX, IDC_ADD_GLASS_H5, m_ctrlLCSensorDisp5);
	DDX_Control(pDX, IDC_ADD_GLASS_H6, m_ctrlLCSensorDisp6);
	DDX_Control(pDX, IDC_ADD_GLASS_H7, m_ctrlLCSensorDisp7);
	DDX_Control(pDX, IDC_ADD_GLASS_H8, m_ctrlLCSensorDisp8);
	DDX_Control(pDX, IDC_ADD_GLASS_H9, m_ctrlLCSensorDisp9);
	DDX_Control(pDX, IDC_ADD_GLASS_H10, m_ctrlLCSensorDisp10);
	DDX_Control(pDX, IDC_ADD_GLASS_H11, m_ctrlLCSensorDisp11);
	DDX_Control(pDX, IDC_ADD_GLASS_H12, m_ctrlLCSensorDisp12);
	DDX_Control(pDX, IDC_LC_QUAN_REPORT, m_ctrlLCQuanRep);
	DDX_Control(pDX, IDC_LC_QUAN_REPORT_01, m_ctrlLCQuanRep01);
	DDX_Control(pDX, IDC_SPECMOVING, m_ctrlSpecmove);
	DDX_Control(pDX, IDC_LABEL_ADDDEL_CHANGE, m_ctrlAddChange);
	DDX_Control(pDX, IDC_LABEL_ADDDEL_COMPARE, m_ctrlAddCompare);
	DDX_Control(pDX, IDC_LC_QUAN_H13, m_ctrlLCQntyDisp13);
	DDX_Control(pDX, IDC_LC_QUAN_H14, m_ctrlLCQntyDisp14);
	DDX_Control(pDX, IDC_LC_QUAN_H15, m_ctrlLCQntyDisp15);
	DDX_Control(pDX, IDC_LC_QUAN_H16, m_ctrlLCQntyDisp16);
	DDX_Control(pDX, IDC_LC_H13, m_DropUse13);
	DDX_Control(pDX, IDC_LC_H14, m_DropUse14);
	DDX_Control(pDX, IDC_LC_H15, m_DropUse15);
	DDX_Control(pDX, IDC_LC_H16, m_DropUse16);
	DDX_Control(pDX, IDC_LABEL_BAL_USE5, m_ctrlBalUse5);
	DDX_Control(pDX, IDC_LABEL_BAL_USE6, m_ctrlBalUse6);
	DDX_Control(pDX, IDC_ADD_GLASS_H13, m_ctrlLCSensorDisp13);
	DDX_Control(pDX, IDC_ADD_GLASS_H14, m_ctrlLCSensorDisp14);
	DDX_Control(pDX, IDC_ADD_GLASS_H15, m_ctrlLCSensorDisp15);
	DDX_Control(pDX, IDC_ADD_GLASS_H16, m_ctrlLCSensorDisp16);
	DDX_Control(pDX, IDC_LABEL_MESSAGE2, m_ctrlMessage1);
	DDX_Control(pDX, IDC_PROGRESS5, m_ctrlProgress05);
	DDX_Control(pDX, IDC_MSFLEXGRID_CAN_REMAIN, m_ctrlCanRemain);
	DDX_Control(pDX, IDC_GREASE2, m_ctrlGrease2);
	DDX_Control(pDX, IDC_CMD_GREASE_MSG, m_ctrlGreaseMsg);
	DDX_Control(pDX, IDC_CMD_SPECIAL_INFO, m_ctrlSpecialInfo);
	DDX_Control(pDX, IDC_GLS_DATA_REQUEST, m_ctrlGlassDataReq);
	DDX_Control(pDX, IDC_LC_REMAIN, m_ctrlLCReMain);
	DDX_Control(pDX, IDC_CMD_AUTO_TITLE, m_ctrlAutoModeTitle);
	DDX_Control(pDX, IDC_CAN_WEIGHT, m_ctrlCanisterUseDisplay);
	DDX_Control(pDX, IDC_LABEL_BAL_USE7, m_ctrlBalUse7);
	DDX_Control(pDX, IDC_LABEL_BAL_USE8, m_ctrlBalUse8);
	DDX_Control(pDX, IDC_LABEL_MARK, m_ctrlMarkInsp);
	DDX_Control(pDX, IDC_LABEL_INTERDROP, m_ctrlInterDrop);
	DDX_Control(pDX, IDC_CMD_WARMMING_UP, m_ctrlWarmUp);
	DDX_Control(pDX, IDC_RECIPE_INFO, m_ctrlRecipeInfo);
	DDX_Control(pDX, IDC_CMD_LOAD_SENSOR, m_ctrlLoadSensorMode);
	DDX_Control(pDX, IDC_CMD_JOB_MOVE, m_ctrlJobMovePos);
	DDX_Control(pDX, IDC_CMD_Valc_Run, m_ctrlValcRun);
	DDX_Control(pDX, IDC_CMD_PD_DOWN_STATUS, m_ctrlPdDownStatus);
	DDX_Control(pDX, IDC_LABEL_VISION_USE, m_ctrlvisionuse);
	DDX_Control(pDX, IDC_AUTO_VISION, m_ctrlAutoVision);
	DDX_Control(pDX, IDC_PROGRESS6, m_ctrlProgress06);
	DDX_Control(pDX, IDC_CMD_LOG_WRITE, m_ctrlLogWrite);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAuto, CDialog)
	//{{AFX_MSG_MAP(CAuto)
	ON_MESSAGE(WM_DOTNUM_DISP, OnDotNumDisp)
	ON_MESSAGE(WM_INFODISP, OnInfoDisp)
	ON_MESSAGE(WM_AMOUNTDISP, OnAmountDisp)
	ON_MESSAGE(WM_TARGET_WEIGHT_MMGDISP, OnTargetWeightMMGDisp)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_ADJUST_IN_AUTO, OnAdjustInAuto)
	ON_MESSAGE(WM_AUTO_MEA_HEAD, OnSubMeaHeadDispReturn)
	//ON_MESSAGE(WM_VISION_INIT, OnVision_Init)	//ehji 140804
	ON_WM_RBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAuto message handlers

BOOL CAuto::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	SelectLanguage();

	int i = 0, j=0, k=0;
	CString str="";

// 	for( i=0 ; i<MAX_NOZZLE ;i++)
// 		pDoc->m_strRFDataLOTID_Full[i] = "CL-1110240001-ML-1103-1F-0002";

	BOOL bResult=FALSE;
	g_bReportWorkTest=FALSE;

	g_bLcSupplyUsed = TRUE;

//2014.12.01 by tskim Dummy Big Drop 방지 Suction 동작 테스트 용
	for( i=0 ; i<MAX_NOZZLE ;i++)
		m_dPAxis_SuctionOffset_Temp[i] = -20.0;

/**********************************************************************************************************/
/********************************************************************************************************************/

	//by shin//2009//auto thread 동작중인지로 구분한다...//
//	pDoc->m_bAutoManualFlag = TRUE; //090612 Auto 시 ErrMsg 처리 확인 변수 초기화 by shlee

//	m_nJobStep=300; //?
	//default job step //by shin
	m_nJobStep = 0;

	//121210
//	if ( !pDoc-> m_structDataEditor.m_bMMGLineDropMode )
//		GetDlgItem(IDC_CMD_MMG_LINEDROP)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );

	m_ctrlSpecmove.SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );

	m_nMeaInterval=pDoc->m_structAdjustCondition.nCheckingIntervalSheet;

	CalcPanelCount();	// Panel의 최대값과 Sub Panel의 시작 번호 등을 계산한다.

	if(ThreadStage.JobModeSelection == 0)
	{
		if(g_bDropStartLimit == TRUE)
		{
			if(pView->m_nLanguage == 0)
			m_ctrlAutoModeTitle.SetCaption("AUTO            (투입 제한)");
			else if(pView->m_nLanguage == 1)
				m_ctrlAutoModeTitle.SetCaption("AUTO         (Insert Limit)");
			else if(pView->m_nLanguage == 2)
				m_ctrlAutoModeTitle.SetCaption("AUTO        (投入限制)");

			m_ctrlAutoModeTitle.SetBackColor(RED);
		}
		else
		{
			if(pView->m_nLanguage == 0)
			m_ctrlAutoModeTitle.SetCaption("AUTO           (투입 제한 해제)");
			else if(pView->m_nLanguage == 1)
			m_ctrlAutoModeTitle.SetCaption("AUTO         (Insert Limit Clear)");
			else if(pView->m_nLanguage == 2)
			m_ctrlAutoModeTitle.SetCaption("AUTO           (投入限制解除)");

			m_ctrlAutoModeTitle.SetBackColor(BLUE);
		}

		if ( g_bLoadUnLoadSensorMode )
		{
			m_ctrlLoadSensorMode.SetBackColor(GREEN); 
			if(pView->m_nLanguage ==0)
			m_ctrlLoadSensorMode.SetCaption("반입 센서 모드 OFF"); 
			else if(pView->m_nLanguage ==1)
			m_ctrlLoadSensorMode.SetCaption("Loading Sensor Mode OFF"); 
			else if(pView->m_nLanguage ==2)
			m_ctrlLoadSensorMode.SetCaption("搬入Sensor Mode OFF"); 
		}
		else 
		{
			m_ctrlLoadSensorMode.SetBackColor(RED); 

			if(pView->m_nLanguage ==0)
			m_ctrlLoadSensorMode.SetCaption("반입 센서 모드 ON"); 
			else if(pView->m_nLanguage ==1)
			m_ctrlLoadSensorMode.SetCaption("Loading Sensor Mode ON"); 
			else if(pView->m_nLanguage ==2)
			m_ctrlLoadSensorMode.SetCaption("搬入Sensor Mode ON"); 
		}


		if ( !g_bLogWriteOff ) //200921 by shlee log 기록 off mode 
		{
			m_ctrlLogWrite.SetBackColor(GREEN); 
			if(pView->m_nLanguage == 0)
			m_ctrlLogWrite.SetCaption("LOG ON"); 

			else if(pView->m_nLanguage == 1)
			m_ctrlLogWrite.SetCaption("LOG ON");
			
			else if(pView->m_nLanguage == 2)
			m_ctrlLogWrite.SetCaption("LOG ON");
		}
		else 
		{		
			m_ctrlLogWrite.SetBackColor(RED); 
			if(pView->m_nLanguage == 0)
			m_ctrlLogWrite.SetCaption("LOG OFF"); 
		
			else if(pView->m_nLanguage == 1)
			m_ctrlLogWrite.SetCaption("LOG OFF"); 
		
			else if(pView->m_nLanguage == 2)
			m_ctrlLogWrite.SetCaption("LOG OFF"); 
		}	


		if ( !g_bJobMovePos )
		{
			m_ctrlJobMovePos.SetBackColor(RED); 

			if(pView->m_nLanguage ==0)
			m_ctrlJobMovePos.SetCaption("작업 위치 이동 OFF"); 
			else if(pView->m_nLanguage ==1)
			m_ctrlJobMovePos.SetCaption("Job POS Move OFF"); 
			else if(pView->m_nLanguage ==2)
			m_ctrlJobMovePos.SetCaption("作業位置移動 OFF"); 

			m_ctrlJobMovePos.EnableWindow(false);		//ehji 141125

		}
		else 
		{			
			m_ctrlJobMovePos.SetBackColor(GREEN);

			if(pView->m_nLanguage ==0)
			m_ctrlJobMovePos.SetCaption("작업 위치 이동 ON"); 
			else if(pView->m_nLanguage ==1)
			m_ctrlJobMovePos.SetCaption("Job POS Move ON"); 
			else if(pView->m_nLanguage ==2)
			m_ctrlJobMovePos.SetCaption("作業位置移動 ON"); 

			m_ctrlJobMovePos.EnableWindow(false);		//ehji 141125
		}

		if (!g_bValcRun)		//ehji 141013
		{
			m_ctrlValcRun.SetBackColor(RED);
			m_ctrlValcRun.SetCaption("Valc RUN OFF"); 
		}
		else 
		{
			m_ctrlValcRun.SetBackColor(GREEN);
			m_ctrlValcRun.SetCaption("Valc RUN ON"); 
		}

		
		if(pDoc->m_structAdjustCondition.nCheckingIntervalSheet != 0){
			m_ctrlAutoMeasure.SetPicture(0);
			m_ctrlAutoMeasure.EnableWindow(TRUE);
		}
		else {
			m_ctrlAutoMeasure.SetPicture(m_pictureholder.GetPictureDispatch());
			m_ctrlAutoMeasure.EnableWindow(FALSE);
		}

		m_ctrlWarmUp.SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	}
	else if(ThreadStage.JobModeSelection == 1)
	{
		if(pView->m_nLanguage == 0)
		{
			m_ctrlAutoModeTitle.SetCaption("반송 모드");
			m_ctrlAutoDrop.SetCaption("대기");
		}
		else if(pView->m_nLanguage == 1)
		{
			m_ctrlAutoModeTitle.SetCaption("PASS Mode");
			m_ctrlAutoDrop.SetCaption("Wating");
		}
		else if(pView->m_nLanguage == 2)
		{
			m_ctrlAutoModeTitle.SetCaption("搬送模式");
			m_ctrlAutoDrop.SetCaption("待機");
		}


		pView->m_bUseWarmUp = FALSE; 
		m_ctrlWarmUp.SetCaption("Warm Up X");
		m_ctrlWarmUp.SetBackColor(RED); 
		m_ctrlAutoScrap.EnableWindow(FALSE);
		m_ctrlAutoDummyDrop.SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
		m_ctrlAutoMeasure.SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
		m_ctrlProgress04.SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
		m_ctrlAutoVision.SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );		//ehji 150327
	}
	//임시
//	ThreadStage.nDropStatus = 0;
	if(ThreadStage.JobModeSelection != 1)
	{	// 반송모드가 아닌 경우에만...
		if(ThreadStage.nDropStatus==0)
		{
			if(pDoc->m_structDataEditor.m_bMarkAlignInsp)
			{
				if(pView->m_nLanguage == 0)
				m_ctrlAutoDrop.SetCaption("Mark Align  &  토출 요구");
				else if(pView->m_nLanguage == 1)
				m_ctrlAutoDrop.SetCaption("Mark Align  &  Dispensing Demand");
				else if(pView->m_nLanguage == 2)
				m_ctrlAutoDrop.SetCaption("Mark Align  &  吐出要求");
			}
			else
			{
				if(pView->m_nLanguage == 0)
				m_ctrlAutoDrop.SetCaption("토출 요구");
				else if(pView->m_nLanguage == 1)
				m_ctrlAutoDrop.SetCaption("Dispensing Demand");
				else if(pView->m_nLanguage == 2)
				m_ctrlAutoDrop.SetCaption("吐出要求");
			}
		}
		else if(ThreadStage.nDropStatus==1)
		{
			if(pView->m_nLanguage == 0)
			m_ctrlAutoDrop.SetCaption("토출 중");
			else if(pView->m_nLanguage == 1)
			m_ctrlAutoDrop.SetCaption("Dispensing");
			else if(pView->m_nLanguage == 2)
			m_ctrlAutoDrop.SetCaption("吐出中");
		}
		else if(ThreadStage.nDropStatus==2)
		{
			if(pView->m_nLanguage == 0)
			m_ctrlAutoDrop.SetCaption("토출 완료");
			else if(pView->m_nLanguage == 0)
			m_ctrlAutoDrop.SetCaption("Finish Dispensing");
			else if(pView->m_nLanguage == 0)
			m_ctrlAutoDrop.SetCaption("吐出完了");
		}
	}
	//
	SubDisplayRecipeName();
	
	// step / continuous
	if(ThreadStage.StepStop==TRUE)  { m_ctrlAutoStep.SetBackColor(GREEN);      m_ctrlAutoContinuous.SetBackColor(WHITEGRAY);	}
	else 							{ m_ctrlAutoStep.SetBackColor(WHITEGRAY);  m_ctrlAutoContinuous.SetBackColor(GREEN);		}

	// work data //m_ctrlAutoWorkData
	m_ctrlAutoWorkData.SetTextMatrix(0, 0, "Recipe No");
	m_ctrlAutoWorkData.SetTextMatrix(1, 0, "Glass ID");
	m_ctrlAutoWorkData.SetTextMatrix(2, 0, "Lot No");
	m_ctrlAutoWorkData.SetTextMatrix(3, 0, "Slot No");

	m_ctrlAutoWorkData.SetColWidth(0, 1150); //1500
	m_ctrlAutoWorkData.SetColWidth(1, 2250); //2800 

	for(i=0 ; i < 4 ; i++) m_ctrlAutoWorkData.SetRowHeight(i,270);

	//Canister 잔량 표기//
	for(i = 0; i<4; i++)
	{
		str.Format(" CAN#%d",i+1);
		m_ctrlCanRemain.SetTextMatrix(0, 3-i, str);
		m_ctrlCanRemain.SetColWidth(i, 870);//1070
	}

	for(i=0 ; i < 3 ; i++) m_ctrlCanRemain.SetRowHeight(i,240);
	
	m_ctrlGlassProcess.SetTextMatrix(0, 0, "Glass Count");

	if(pView->m_nLanguage == 0)
	m_ctrlGlassProcess.SetTextMatrix(1, 0, "측정 주기(장)");
	else if(pView->m_nLanguage == 1)
	m_ctrlGlassProcess.SetTextMatrix(1, 0, "M/M Count(S)");
	else if(pView->m_nLanguage == 2)
	m_ctrlGlassProcess.SetTextMatrix(1, 0, "測定週期（張）");

	if(ThreadStage.JobModeSelection == 1) {		// 반송모드
		if(pView->m_nLanguage == 0)
		m_ctrlGlassProcess.SetTextMatrix(2, 0, "반송지연시간 (초)");
		else if(pView->m_nLanguage == 1)
		m_ctrlGlassProcess.SetTextMatrix(2, 0, "Pass Delay Time (S)");
		else if(pView->m_nLanguage == 2)
		m_ctrlGlassProcess.SetTextMatrix(2, 0, "搬送延持時間 (秒)");
		
		str.Format("%d",ThreadStage.nPassTime);
		m_ctrlGlassProcess.SetTextMatrix(2, 1, str);	// 반송모드 지연시간을 Default로 10초로 한다.
		ThreadStage.nPassTime = atoi((char *)(LPCSTR)m_ctrlGlassProcess.GetTextMatrix(2, 1));
	}
	else//Autorun 모드
	{
//강제 측정 주기
//		m_ctrlGlassProcess.SetTextMatrix(2, 0, "강제 측정주기");
		m_ctrlGlassProcess.SetTextMatrix(2, 0, "Scan count"); //jykim scan_cycle 160530
		str.Format("%d/%d",g_nGlassCountScanCycle, pDoc->m_structN_Vision.m_nScan_Cycle); //jykim scan_cycle 160530
		m_ctrlGlassProcess.SetTextMatrix(2, 1, str); //jykim scan_cycle 160530
//		str.Format("%d",m_nForcedMeasInterval); 
//		m_ctrlGlassProcess.SetTextMatrix(2, 1, str);

	}
	for(i=0 ; i < 3 ; i++) m_ctrlGlassProcess.SetRowHeight(i,530);
	m_ctrlGlassProcess.SetColWidth(0, 1100);
	m_ctrlGlassProcess.SetColWidth(1, 950);
	for(i=0 ; i < 2 ; i++) m_ctrlGlassProcess.SetColAlignment(i,4);

	//
	// drop condition //m_ctrlAutoDropCondition
	if(pView->m_nLanguage == 0)
	{
		m_ctrlAutoDropCondition.SetTextMatrix(0, 0, "토출조건");
		m_ctrlAutoDropCondition.SetTextMatrix(1, 0, "K속도(mm/s)");
		m_ctrlAutoDropCondition.SetTextMatrix(2, 0, "S속도(mm/s)");
		m_ctrlAutoDropCondition.SetTextMatrix(3, 0, "G속도(rpm)");
		
		m_ctrlAutoDropCondition.SetTextMatrix(0, 3, "충진모드");
		m_ctrlAutoDropCondition.SetTextMatrix(1, 3, "P충진속도(Kpps)");
		m_ctrlAutoDropCondition.SetTextMatrix(2, 3, "P토출속도(Kpps)");
	//2009.10.20 by tskim
	//	m_ctrlAutoDropCondition.SetTextMatrix(3, 3, "-----------------");
	//	m_ctrlAutoDropCondition.SetTextMatrix(3, 4, "-----------------");
		m_ctrlAutoDropCondition.SetTextMatrix(3, 3, "Drop Mode");
	}
	else if(pView->m_nLanguage == 1)
	{
		m_ctrlAutoDropCondition.SetTextMatrix(0, 0, "Drop Mode");
		m_ctrlAutoDropCondition.SetTextMatrix(1, 0, "K Speed(mm/s)");
		m_ctrlAutoDropCondition.SetTextMatrix(2, 0, "S Speed(mm/s)");
		m_ctrlAutoDropCondition.SetTextMatrix(3, 0, "G Speed(rpm)");
		
		m_ctrlAutoDropCondition.SetTextMatrix(0, 3, "Suction Mode");
		m_ctrlAutoDropCondition.SetTextMatrix(1, 3, "P Suction Speed(Kpps)");
		m_ctrlAutoDropCondition.SetTextMatrix(2, 3, "P Discharge Speed(Kpps)");
	//2009.10.20 by tskim
	//	m_ctrlAutoDropCondition.SetTextMatrix(3, 3, "-----------------");
	//	m_ctrlAutoDropCondition.SetTextMatrix(3, 4, "-----------------");
		m_ctrlAutoDropCondition.SetTextMatrix(3, 3, "Drop Mode");
	}
	else if(pView->m_nLanguage == 2)
	{
		m_ctrlAutoDropCondition.SetTextMatrix(0, 0, "吐出條件");
		m_ctrlAutoDropCondition.SetTextMatrix(1, 0, "K速度(mm/s)");
		m_ctrlAutoDropCondition.SetTextMatrix(2, 0, "S速度(mm/s)");
		m_ctrlAutoDropCondition.SetTextMatrix(3, 0, "G速度(rpm)");
		
		m_ctrlAutoDropCondition.SetTextMatrix(0, 3, "充進模式");
		m_ctrlAutoDropCondition.SetTextMatrix(1, 3, "P充進速度(Kpps)");
		m_ctrlAutoDropCondition.SetTextMatrix(2, 3, "P吐出速度(Kpps)");
	//2009.10.20 by tskim
	//	m_ctrlAutoDropCondition.SetTextMatrix(3, 3, "-----------------");
	//	m_ctrlAutoDropCondition.SetTextMatrix(3, 4, "-----------------");
		m_ctrlAutoDropCondition.SetTextMatrix(3, 3, "Drop Mode");
	}

	m_ctrlAutoDropCondition.SetColWidth(0, 1300);
	m_ctrlAutoDropCondition.SetColWidth(1, 755);
	
	for(i=0 ; i < 4 ; i++) m_ctrlAutoDropCondition.SetRowHeight(i,400);
	for(i=0 ; i < 5 ; i++) m_ctrlAutoDropCondition.SetColAlignment(i,4);
//2007.08.08 by tskim
//Pattern 폭, Drop Offset Display
	m_ctrlAutoDropCondition.SetColWidth(2,1710);

	m_ctrlAutoDropCondition.SetCol(0);
	for(i=0 ; i < 4 ; i++)
	{
		m_ctrlAutoDropCondition.SetRow(i);
		m_ctrlAutoDropCondition.SetCellBackColor(GRID_COLOR);
	}

	m_ctrlAutoDropCondition.SetCol(2);
	m_ctrlAutoDropCondition.SetRow(0);
	m_ctrlAutoDropCondition.SetCellBackColor(GRID_COLOR);
	m_ctrlAutoDropCondition.SetTextMatrix(0,2,"Patten Width(X,Y)");
	str.Format("%.3f,%.3f",pDoc->m_structPatternData[pView->m_nRefPatNum].m_dWidthX,pDoc->m_structPatternData[pView->m_nRefPatNum].m_dWidthY);
	m_ctrlAutoDropCondition.SetTextMatrix(1,2,str);

	m_ctrlAutoDropCondition.SetCol(2);
	m_ctrlAutoDropCondition.SetRow(2);
	m_ctrlAutoDropCondition.SetCellBackColor(GRID_COLOR);
	m_ctrlAutoDropCondition.SetTextMatrix(2,2,"Drop Offset(mm)");
	str.Format("%.3f(mm)",pDoc->m_structPatternData[pView->m_nRefPatNum].m_dDropOffset);
	m_ctrlAutoDropCondition.SetTextMatrix(3,2,str);

	m_ctrlAutoDropCondition.SetColWidth(3, 1700);
	m_ctrlAutoDropCondition.SetColWidth(4, 730);
	m_ctrlAutoDropCondition.SetCol(3);
	for(i=0 ; i < 4 ; i++)
	{
		m_ctrlAutoDropCondition.SetRow(i);
		m_ctrlAutoDropCondition.SetCellBackColor(GRID_COLOR);
	}

	m_ctrlAutoDropCondition.SetCol(4);
	m_ctrlAutoDropCondition.SetRow(3);
	m_ctrlAutoDropCondition.SetCellBackColor(GRID_COLOR);

	// auto measured data
	m_ctrlAutoMeaData.SetColWidth(0, 1600);
	for(i=0 ; i < MAX_NOZZLE ; i++)
	{
		m_ctrlAutoMeaData.SetColWidth(i+1, 840);
		str.Format("H%d",i+1);
		m_ctrlAutoMeaData.SetTextMatrix(0, (MAX_NOZZLE+1)-(i+1), str);
	
		if(pDoc->m_bIsHeadSelected[i]==TRUE) 
		{
			m_ctrlAutoMeaData.SetRow(0);
			m_ctrlAutoMeaData.SetCol((MAX_NOZZLE+1)-(i+1));
			m_ctrlAutoMeaData.SetCellBackColor(GREEN);
		}

		//lbg 080415 측정횟수 표기..
		str.Format("%.3f",pDoc->m_structAdjustCondition.dTargetWeight[i]);
		//str.Format("0/%.3f",pDoc->m_structAdjustCondition.dTargetWeight[i]);
		m_ctrlAutoMeaData.SetTextMatrix(1,(MAX_NOZZLE+1)-(i+1),str);

		m_ctrlAutoMeaData.SetRow(1);
		m_ctrlAutoMeaData.SetCol((MAX_NOZZLE+1)-(i+1));
		if(pDoc->m_bIsHeadSelected[i]==TRUE)	
			m_ctrlAutoMeaData.SetCellBackColor(YELLOW);
		else	
			m_ctrlAutoMeaData.SetCellBackColor(WHITEGRAY);
	}

	for ( int nCount = 0; nCount < 8; nCount++ )
	{
		if ( nCount == 0 || nCount == 1)
			m_ctrlAutoMeaData.SetRowHeight(nCount,300);
		else
			m_ctrlAutoMeaData.SetRowHeight(nCount,250); 
	}
//2011.04.27 by tskim
	if(ThBal[BALID1].nBalanceMode == 0)
	{
// 		m_ctrlAutoMeaData.SetRow(0);
// 		m_ctrlAutoMeaData.SetCol(0);
// 		m_ctrlAutoMeaData.SetCellBackColor(WHITEGREEN);
		if(pView->m_nLanguage ==0)
		m_ctrlAutoMeaData.SetTextMatrix(0,0,"BAL 정상모드");
		else if(pView->m_nLanguage ==1)
		m_ctrlAutoMeaData.SetTextMatrix(0,0,"BAL Normal");
		else if(pView->m_nLanguage ==2)
		m_ctrlAutoMeaData.SetTextMatrix(0,0,"BAL 正常模式");
	}
	else
	{
// 		m_ctrlAutoMeaData.SetRow(0);
// 		m_ctrlAutoMeaData.SetCol(0);
// 		m_ctrlAutoMeaData.SetCellBackColor(RED);
		if(pView->m_nLanguage ==0)
		m_ctrlAutoMeaData.SetTextMatrix(0,0,"BAL 지진모드");
		else if(pView->m_nLanguage ==1)
		m_ctrlAutoMeaData.SetTextMatrix(0,0,"BAL abNormal");
		else if(pView->m_nLanguage ==2)
		m_ctrlAutoMeaData.SetTextMatrix(0,0,"BAL 地震模式");

	}
	if(ThBal[BALID1].nBalanceUse)	m_ctrlBalUse1.SetBackColor(GREEN);
	else m_ctrlBalUse1.SetBackColor(RED);

	if(ThBal[BALID2].nBalanceUse)	m_ctrlBalUse2.SetBackColor(GREEN);
	else m_ctrlBalUse2.SetBackColor(RED);

	if(ThBal[BALID3].nBalanceUse)	m_ctrlBalUse3.SetBackColor(GREEN);
	else m_ctrlBalUse3.SetBackColor(RED);

	if(ThBal[BALID4].nBalanceUse)	m_ctrlBalUse4.SetBackColor(GREEN);
	else m_ctrlBalUse4.SetBackColor(RED);

	if(ThBal[BALID5].nBalanceUse)	m_ctrlBalUse5.SetBackColor(GREEN);
	else m_ctrlBalUse5.SetBackColor(RED);

	if(ThBal[BALID6].nBalanceUse)	m_ctrlBalUse6.SetBackColor(GREEN);
	else m_ctrlBalUse6.SetBackColor(RED);

	if(ThBal[BALID7].nBalanceUse)	m_ctrlBalUse7.SetBackColor(GREEN);
	else m_ctrlBalUse7.SetBackColor(RED);

	if(ThBal[BALID8].nBalanceUse)	m_ctrlBalUse8.SetBackColor(GREEN);
	else m_ctrlBalUse8.SetBackColor(RED);

	m_ctrlAutoMeaData.SetRow(1); m_ctrlAutoMeaData.SetCol(0);
	m_ctrlAutoMeaData.SetCellBackColor(YELLOW);

	CString strTemp[7]={"Target (mg)","data1(mg)","data2(mg)","data3(mg)","data4(mg)","data5(mg)","data6(mg)"};
	for(i=0; i<7;i++) m_ctrlAutoMeaData.SetTextMatrix(i+1,0,strTemp[i]);
//
	if(pDoc->m_structDataEditor.m_bIsTrfOnlyMode!=TRUE)
	{
		if(pDoc->m_structDataEditor.m_bPatternDispensing==TRUE)  {	m_ctrlPatternDispensing.SetCaption("Dispensing ON");	 m_ctrlPatternDispensing.SetBackColor(WHITEGRAY);	}
		else													 {	m_ctrlPatternDispensing.SetCaption("Dispensing OFF");	 m_ctrlPatternDispensing.SetBackColor(RED);			}

		if(pDoc->m_structDataEditor.m_bNgDropping==TRUE)		 {	m_ctrlNgPanelSkip.SetCaption("NG Panel DROP");			 m_ctrlNgPanelSkip.SetBackColor(WHITEGRAY);			}
		else													 {	m_ctrlNgPanelSkip.SetCaption("NG Panel SKIP");			 m_ctrlNgPanelSkip.SetBackColor(RED);				}				

		if(pDoc->m_structDataEditor.m_bRecipeNoCompare==TRUE)	 {	m_ctrlRecipeNumCompare.SetCaption("Recipe Compare ON");  m_ctrlRecipeNumCompare.SetBackColor(WHITEGRAY);		}
		else													 {	m_ctrlRecipeNumCompare.SetCaption("Recipe Compare OFF"); m_ctrlRecipeNumCompare.SetBackColor(RED);			}


		if(pView->m_nLanguage == 0)
		{
			if(pDoc->m_structDataEditor.m_bLCQuanReportPlus==TRUE)	{	m_ctrlLCQuanRep.SetCaption("중량상한보고ON");  m_ctrlLCQuanRep.SetBackColor(WHITEGRAY);		}
			else													{	m_ctrlLCQuanRep.SetCaption("중량상한보고OFF"); m_ctrlLCQuanRep.SetBackColor(RED);			}

			if(pDoc->m_structDataEditor.m_bLCQuanReportMinus==TRUE)	{	m_ctrlLCQuanRep01.SetCaption("중량하한보고ON");  m_ctrlLCQuanRep01.SetBackColor(WHITEGRAY);		}
			else													{	m_ctrlLCQuanRep01.SetCaption("중량하한보고OFF"); m_ctrlLCQuanRep01.SetBackColor(RED);			}

			if(pDoc->m_structDataEditor.m_bAddDelChange==TRUE)		{	m_ctrlAddChange.SetCaption("A/D 변환 ON");  m_ctrlAddChange.SetBackColor(WHITEGRAY);		}
			else													{	m_ctrlAddChange.SetCaption("A/D 변환 OFF"); m_ctrlAddChange.SetBackColor(RED);			}

			if(pDoc->m_structDataEditor.m_bAddDelCompare==TRUE)		{	m_ctrlAddCompare.SetCaption("A/D 비교 ON");  m_ctrlAddCompare.SetBackColor(WHITEGRAY);		}
			else													{	m_ctrlAddCompare.SetCaption("A/D 비교 OFF"); m_ctrlAddCompare.SetBackColor(RED);			}
		}
		else if(pView->m_nLanguage == 1)
		{
			if(pDoc->m_structDataEditor.m_bLCQuanReportPlus==TRUE)	{	m_ctrlLCQuanRep.SetCaption("중량상한보고ON");  m_ctrlLCQuanRep.SetBackColor(WHITEGRAY);		}
			else													{	m_ctrlLCQuanRep.SetCaption("중량상한보고OFF"); m_ctrlLCQuanRep.SetBackColor(RED);			}

			if(pDoc->m_structDataEditor.m_bLCQuanReportMinus==TRUE)	{	m_ctrlLCQuanRep01.SetCaption("중량하한보고ON");  m_ctrlLCQuanRep01.SetBackColor(WHITEGRAY);		}
			else													{	m_ctrlLCQuanRep01.SetCaption("중량하한보고OFF"); m_ctrlLCQuanRep01.SetBackColor(RED);			}

			if(pDoc->m_structDataEditor.m_bAddDelChange==TRUE)		{	m_ctrlAddChange.SetCaption("A/D Change ON");  m_ctrlAddChange.SetBackColor(WHITEGRAY);		}
			else													{	m_ctrlAddChange.SetCaption("A/D Change OFF"); m_ctrlAddChange.SetBackColor(RED);			}

			if(pDoc->m_structDataEditor.m_bAddDelCompare==TRUE)		{	m_ctrlAddCompare.SetCaption("A/D Compare ON");  m_ctrlAddCompare.SetBackColor(WHITEGRAY);		}
			else													{	m_ctrlAddCompare.SetCaption("A/D Compare OFF"); m_ctrlAddCompare.SetBackColor(RED);			}
		}
		else if(pView->m_nLanguage == 2)
		{
			if(pDoc->m_structDataEditor.m_bLCQuanReportPlus==TRUE)	{	m_ctrlLCQuanRep.SetCaption("重量上限報告ON");  m_ctrlLCQuanRep.SetBackColor(WHITEGRAY);		}
			else													{	m_ctrlLCQuanRep.SetCaption("重量上限報告OFF"); m_ctrlLCQuanRep.SetBackColor(RED);			}

			if(pDoc->m_structDataEditor.m_bLCQuanReportMinus==TRUE)	{	m_ctrlLCQuanRep01.SetCaption("重量下限報告ON");  m_ctrlLCQuanRep01.SetBackColor(WHITEGRAY);		}
			else													{	m_ctrlLCQuanRep01.SetCaption("重量下限報告OFF"); m_ctrlLCQuanRep01.SetBackColor(RED);			}

			if(pDoc->m_structDataEditor.m_bAddDelChange==TRUE)		{	m_ctrlAddChange.SetCaption("A/D 變換 ON");  m_ctrlAddChange.SetBackColor(WHITEGRAY);		}
			else													{	m_ctrlAddChange.SetCaption("A/D 變換 OFF"); m_ctrlAddChange.SetBackColor(RED);			}

			if(pDoc->m_structDataEditor.m_bAddDelCompare==TRUE)		{	m_ctrlAddCompare.SetCaption("A/D 比較 ON");  m_ctrlAddCompare.SetBackColor(WHITEGRAY);		}
			else													{	m_ctrlAddCompare.SetCaption("A/D 比較 OFF"); m_ctrlAddCompare.SetBackColor(RED);			}
		}

		 
	}
	else
	{
		m_ctrlPatternDispensing.SetCaption("Dispensing OFF");	 m_ctrlPatternDispensing.SetBackColor(RED);			
		m_ctrlNgPanelSkip.SetCaption("NG Panel SKIP");			 m_ctrlNgPanelSkip.SetBackColor(RED);
		m_ctrlRecipeNumCompare.SetCaption("Recipe Compare OFF"); m_ctrlRecipeNumCompare.SetBackColor(RED);			
		m_ctrlLCQuanRep.SetCaption("중량상한보고OFF");			 m_ctrlLCQuanRep.SetBackColor(RED);
		m_ctrlLCQuanRep01.SetCaption("중량하한보고OFF");		 m_ctrlLCQuanRep01.SetBackColor(RED);
	}

//2009.11.23 by tskim 유효 Drop수 계산을 위한 NGCell 구분 
	for(i=0;i<MAX_PANEL;i++)
		CIM_Information.bNGCell[i] = FALSE;
//					if((m_pLcDoc->m_structDataEditor.m_bSelectDropMode == TRUE) && (ThreadStage.JobModeSelection == 0)) //Flexible Line Drop && Auto Drop
//					{
		for(j=0; j<pDoc->m_structDataEditor.m_nPatternNum;j++)
		{
//							if(LinePattern_Seq_count == m_pLcDoc->m_structPatternData[j].LinePatNum)
//							{
				if(pDoc->m_structPatternData[j].m_nSequence != 0)
				{						
					for(i=0; i<MAX_NOZZLE; i++)
					{
						if(pDoc->m_structPatternData[j].m_bIsOnHead[i] == TRUE)
						{
							// Drop when Pattern dispensing button is On. 
							if((pDoc->m_structDataEditor.m_bPatternDispensing==TRUE)&&(pDoc->m_structDataEditor.m_bIsTrfOnlyMode!=TRUE))
							{
								if(pDoc->m_structDataEditor.m_bNgDropping == TRUE)
								{
									CIM_Information.bNGCell[pDoc->m_structPatternData[j].m_nPanelNo[i]-1] = TRUE;
								}
								else if(pView->Panel_Drop_Judge(CIM_Information.Panel_NG_Mark[TFT][pDoc->m_structPatternData[j].m_nPanelNo[i]-1],
														CIM_Information.Panel_NG_Mark[CF][pDoc->m_structPatternData[j].m_nPanelNo[i]-1]))
								{
									CIM_Information.bNGCell[pDoc->m_structPatternData[j].m_nPanelNo[i]-1] = TRUE;
								}
								else
								{
									CIM_Information.bNGCell[pDoc->m_structPatternData[j].m_nPanelNo[i]-1] = FALSE;
								}//2009.11.23 by tskim
							}
							else
							{
								CIM_Information.bNGCell[pDoc->m_structPatternData[j].m_nPanelNo[i]-1] = FALSE;
							}
						}
						else
						{
							CIM_Information.bNGCell[pDoc->m_structPatternData[j].m_nPanelNo[i]-1] = FALSE;
						}
					}
				}
//							}
		}
//					}
/*****************************************************************************************************************************************/

	// Auto창이 처음열릴때에, add/del정보를 읽고서 실드랍수를 계산하고 표시한다.
	for(i = 0 ; i < pDoc->m_structDataEditor.m_nPatternNum ; i++)
	{
		if( pDoc->m_structPatternData[i].m_nSequence > 0 ) 
			SendMessage(WM_DOTNUM_DISP,0,i); // 0 :drop수, SetValidDropPoint(int nP)  
	}
	if(!pDoc->m_structDataEditor.m_bUse_Vision)
		SendMessage(WM_DOTNUM_DISP,1,1); // 1 :display   : Panel ID, Panel 일반정보, ADD/DEL Level, 유효 Drop 수, Drop Count Display.
	else
		SendMessage(WM_DOTNUM_DISP,1,2); // 1 :display   : Panel ID, Panel 일반정보, ADD/DEL Level, 유효 Drop 수, Drop Count Display.
	PostMessage(WM_INFODISP,2,0);		// Drop Condition display : 토출 조건, 테이블 속도, ... // 1번째 pattern에 대해서..//일단 첫번째 패턴에 대한것만 디스플레이...//
	PostMessage(WM_INFODISP,3,TRUE);	// CIM Data display  : Glass ID, Lot No, Slot No,..
//	PostMessage(WM_INFODISP,7,0); //Canister 잔량 표기//
	/*measured data display..*/
	pView->m_bMesuredDataDisplayEnable = TRUE;	  // 측정 Data 한 번 뿌려 주기.
	pView->m_bMesuredDataSaveCompleted = TRUE;
	SubAutoMeasuredDataDisplay();
	/*Lc quantity display*/
	Sub_Amount_Display(TRUE);           // Progress Control 표현해 주기.  => 우선 Range 설정. 
	Sub_Amount_Display(FALSE);			// Progress Control 표현해 주기.  => 현재 양 표시.

	if(pView->GetDocument()->m_bIsEngineerMode == TRUE)
	{
		m_ctrlOffLineMode.EnableWindow(TRUE);
		if(ThreadStage.OffLineMode==FALSE)
		{
			m_ctrlOffLineMode.SetWindowText("Online Mode");
			m_ctrlOffLineMode.SetBackColor(GREEN);	
		}
		else	
		{
			m_ctrlOffLineMode.SetWindowText("Offline Mode");
			m_ctrlOffLineMode.SetBackColor(RED);
		}
	}
	else
	{
		ThreadStage.OffLineMode = FALSE;
		m_ctrlOffLineMode.SetWindowText("Online Mode");
		m_ctrlOffLineMode.SetBackColor(GREEN);	
		m_ctrlOffLineMode.EnableWindow(FALSE);
		
		// by ckh 2009.03.20
		if(ThreadStage.OffLineMode==TRUE) //OFFLINE 이었을 경우. 
		{
			pView->m_pDevice->MNET_BitSet(DISP_BIT, true); 
			pView->SetOffLine(FALSE); // BM 해제 함. 
		}
	}
//#if EQ
	if(pDoc->m_structDataEditor.m_nLcSupplyType == 1) //Auto LC Supply
	{
		if((pDoc->m_nCanisterBalNo[0] != 0)||(pDoc->m_nCanisterBalNo[1] != 0)) //사용할 canister가 있을 경우...//
		{
			for(i = 0; i < MAX_NOZZLE; i++)
			{
				if(pDoc->m_bIsHeadSelected[i] == FALSE) continue;

				if((pDoc->m_structDataEditor.m_strBarcode[i] != CIM_Information.sCanisterTypeData[0])
					|| (pDoc->m_structDataEditor.m_strBarcode[i] != CIM_Information.sCanisterTypeData[1]))
				{
					CNormalMsg dlg;
					dlg.m_bTimer=FALSE;

					if(pView->m_nLanguage == 0)
					{
						dlg.m_strTitle = _T("확인");
						dlg.m_strMsg1 = " Canister의 LC Type과 Recipe의 LC Type이 일치하지 않습니다.";
						dlg.m_strMsg2 = " LC 중앙 공급을 진행 하지 않습니다.";
					}

					if(pView->m_nLanguage == 1)
					{
						dlg.m_strTitle = _T("Check");
						dlg.m_strMsg1 = " LC Type of Canister and Lc Type of Recipe is Different.";
						dlg.m_strMsg2 = " Machine will not do LC Center Supply progress.";
					}

					if(pView->m_nLanguage == 2)	//중국어
					{
						dlg.m_strTitle = _T("確認");
						dlg.m_strMsg1 = " Canister的 LC Type和 Recipe的 LC Type不一致.";
						dlg.m_strMsg2 = " LC 中央供給裝置不運行.";
					}


					if( dlg.DoModal() == IDOK )
					{
						if(pView->m_nLanguage == 0)
						{
							m_ctrlAutoUnLoad.SetCaption("반출");
							m_ctrlCanisterUseDisplay.SetCaption("LC 중앙 공급 사용 안함!!!");
						}
						else if(pView->m_nLanguage == 1)
						{
							m_ctrlAutoUnLoad.SetCaption("Unloading");
							m_ctrlCanisterUseDisplay.SetCaption("LC Center Supply Not USE!!!");
						}
						else if(pView->m_nLanguage == 2)
						{
							m_ctrlAutoUnLoad.SetCaption("搬出");
							m_ctrlCanisterUseDisplay.SetCaption("不使用LC中央供給!!!");
						}


						m_ctrlCanisterUseDisplay.SetBackColor(RED);
						g_bLcSupplyUsed = FALSE;
					}
					else 
					{
							if(pView->m_nLanguage == 0)
						{
							m_ctrlAutoUnLoad.SetCaption("반출");
							m_ctrlCanisterUseDisplay.SetCaption("LC 중앙 공급 사용 안함!!!");
						}
						else if(pView->m_nLanguage == 1)
						{
							m_ctrlAutoUnLoad.SetCaption("Unloading");
							m_ctrlCanisterUseDisplay.SetCaption("LC Center Supply Not USE!!!");
						}
						else if(pView->m_nLanguage == 2)
						{
							m_ctrlAutoUnLoad.SetCaption("搬出");
							m_ctrlCanisterUseDisplay.SetCaption("不使用LC中央供給!!!");
						}

						m_ctrlCanisterUseDisplay.SetBackColor(RED);
						g_bLcSupplyUsed = FALSE;					
					}
					break;
				}
			}
			if(g_bLcSupplyUsed == TRUE)
			{
				m_ctrlAutoUnLoad.SetCaption("액정 공급 & 반출");
			}
		}
		else
		{
					if(pView->m_nLanguage == 0)
						{
							m_ctrlAutoUnLoad.SetCaption("반출");
							m_ctrlCanisterUseDisplay.SetCaption("LC 중앙 공급 사용 안함!!!");
						}
						else if(pView->m_nLanguage == 1)
						{
							m_ctrlAutoUnLoad.SetCaption("Unloading");
							m_ctrlCanisterUseDisplay.SetCaption("LC Center Supply Not USE!!!");
						}
						else if(pView->m_nLanguage == 2)
						{
							m_ctrlAutoUnLoad.SetCaption("搬出");
							m_ctrlCanisterUseDisplay.SetCaption("不使用LC中央供給!!!");
						}

			m_ctrlCanisterUseDisplay.SetBackColor(RED);
			g_bLcSupplyUsed = FALSE;
		}
	}
	else
	{
					if(pView->m_nLanguage == 0)
						{
							m_ctrlAutoUnLoad.SetCaption("반출");
							m_ctrlCanisterUseDisplay.SetCaption("LC 중앙 공급 사용 안함!!!");
						}
						else if(pView->m_nLanguage == 1)
						{
							m_ctrlAutoUnLoad.SetCaption("Unloading");
							m_ctrlCanisterUseDisplay.SetCaption("LC Center Supply Not USE!!!");
						}
						else if(pView->m_nLanguage == 2)
						{
							m_ctrlAutoUnLoad.SetCaption("搬出");
							m_ctrlCanisterUseDisplay.SetCaption("不使用LC中央供給!!!");
						}
		m_ctrlCanisterUseDisplay.SetBackColor(RED);
		g_bLcSupplyUsed = FALSE;
	}
//#endif
	for(i = 0; i < 4; i++)
	{
		m_ctrlCanRemain.SetRow(0);
		m_ctrlCanRemain.SetCol(i);
		m_ctrlCanRemain.SetCellBackColor(LIGHTGRAY);
	}

	if(g_bLcSupplyUsed == TRUE)
	{
		if(pDoc->m_nCanisterBalNo[0]-SUPPLY_BAL1 == 0) //Can#1
		{
			m_ctrlCanRemain.SetRow(0);
			m_ctrlCanRemain.SetCol(3);
			m_ctrlCanRemain.SetCellBackColor(GREEN);
		}
		else if(pDoc->m_nCanisterBalNo[0]-SUPPLY_BAL1 == 1)//Can#2
		{
			m_ctrlCanRemain.SetRow(0);
			m_ctrlCanRemain.SetCol(2);
			m_ctrlCanRemain.SetCellBackColor(GREEN);
		}
		if(pDoc->m_nCanisterBalNo[1]-SUPPLY_BAL1 == 2) //Can#3
		{
			m_ctrlCanRemain.SetRow(0);
			m_ctrlCanRemain.SetCol(1);
			m_ctrlCanRemain.SetCellBackColor(GREEN);
		}
		else if(pDoc->m_nCanisterBalNo[1]-SUPPLY_BAL1 == 3)//Can#4
		{
			m_ctrlCanRemain.SetRow(0);
			m_ctrlCanRemain.SetCol(0);
			m_ctrlCanRemain.SetCellBackColor(GREEN);
		}
	}

	PostMessage(WM_INFODISP,7,0); //Canister 잔량 표기//

	// temp
	CRect rect;
	m_ctrlGreaseMsg.GetWindowRect(rect);

	//Column K 이동량이 경고량을 초과 할 경우
	if(pDoc->m_dMotorMoveDistance[0][0] > pDoc->m_dMotorMoveDistance[0][1])
	{
		m_ctrlGreaseMsg.SetCaption(" Column K축 구리스 주입!!!");
	} 
	//Head S 이동량이 경고량을 초과 할 경우
	else if(pDoc->m_dMotorMoveDistance[1][0] > pDoc->m_dMotorMoveDistance[1][1])
	{
		m_ctrlGreaseMsg.SetCaption(" Head S축 구리스 주입!!!");
	}
	else
	{
		m_ctrlGreaseMsg.MoveWindow(0,0,0,0);
	}

	//141026 PD DOWN : PD DOWN DISPLAY INITIALIZE
	m_ctrlPdDownStatus.GetWindowRect(rect);
	if ( g_bPdReqTimeOver )
	{
		m_ctrlPdDownStatus.SetCaption("PD DOWN TIME OVER");	
		m_ctrlPdDownStatus.SetBackColor(RED);
	}
	else if ( g_bPdDownReq )
	{
		m_ctrlPdDownStatus.SetCaption("PD DOWN");	
		m_ctrlPdDownStatus.SetBackColor(RED);
	}
	else if ( g_bPdDownRelease )
	{
		m_ctrlPdDownStatus.SetCaption("PD DOWN Release");	
		m_ctrlPdDownStatus.SetBackColor(BLUE);
	}

	m_ctrlSpecialInfo.GetWindowRect(rect);

	if(pDoc->m_structDataEditor.m_bSpecialInfo == FALSE)
	{
		m_ctrlSpecialInfo.SetCaption("ECS 특수 정보(Panel Level / Judge) OFF");
	} 
	else
	{
		m_ctrlSpecialInfo.MoveWindow(0,0,0,0);
	}

	if(pDoc->m_structDataEditor.m_nLcSupplyType == 1)
	{
		if(pDoc->m_nCanisterBalNo[0] != 0)
		{
			if(pDoc->m_dCanister[0][pDoc->m_nCanisterBalNo[0]-SUPPLY_BAL1] <= pDoc->m_dCanister[1][pDoc->m_nCanisterBalNo[0]-SUPPLY_BAL1]) //현재량 < 경고량
			{
				g_bCanster1WeightAlarm = TRUE;
			}
			else g_bCanster1WeightAlarm = FALSE;
		}
		else 
		{
			g_bCanster1WeightAlarm = FALSE;
		}

		if(pDoc->m_nCanisterBalNo[1] != 0)	
		{
			if(pDoc->m_dCanister[0][pDoc->m_nCanisterBalNo[1]-SUPPLY_BAL1] <= pDoc->m_dCanister[1][pDoc->m_nCanisterBalNo[1]-SUPPLY_BAL1]) //현재량 < 경고량
			{
				g_bCanster2WeightAlarm = TRUE;
			}
			else g_bCanster2WeightAlarm = FALSE;
		}
		else 
		{
			g_bCanster2WeightAlarm = FALSE;
		}
	}

//
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&//
//	WORD wIOResult2 = 0x0012;
//	pView->SendMessage(WM_ERROR,309,wIOResult2);
//2009.12.27 by tskim For Test//2010.06.07 by tskim
// 	ThreadStage.nHomeEnableCount = 3;
// 	ThreadStage.JobModeSelection == 0;
// 	ThreadStage.ExitFlag = FALSE;
// 	CIM_Information.Glass_ID = "Line Drop";
// 	CIM_Information.Panel_NG_Mark[TFT][0] = 1;
// 	CIM_Information.Panel_NG_Mark[CF][0] = 1;
// 	CIM_Information.Panel_NG_Mark[TFT][1] = 1;
// 	CIM_Information.Panel_NG_Mark[CF][1] = 1;
// 	CIM_Information.Panel_NG_Mark[TFT][2] = 1;
// 	CIM_Information.Panel_NG_Mark[CF][2] = 1;
// 	CIM_Information.Panel_NG_Mark[TFT][3] = 1;
// 	CIM_Information.Panel_NG_Mark[CF][3] = 1;
// 	CIM_Information.Panel_NG_Mark[TFT][4] = 1;
// 	CIM_Information.Panel_NG_Mark[CF][4] = 1;
// 	CIM_Information.Panel_NG_Mark[TFT][5] = 1;
// 	CIM_Information.Panel_NG_Mark[CF][5] = 1;
//  	CIM_Information.Add_Del_Number[0] = -10;
//  	CIM_Information.Add_Del_Number[1] = -11;
//  	CIM_Information.Add_Del_Number[2] = -12;
//  	CIM_Information.Add_Del_Number[3] = -13;
//  	CIM_Information.Add_Del_Number[4] = -14;
//  	CIM_Information.Add_Del_Number[5] = -15;
//  	CIM_Information.Add_Del_Number[6] = -16;
//  	CIM_Information.Add_Del_Number[7] = -17;
// 	CIM_Information.Add_Del_Number[2] = 0;
// 	CIM_Information.Add_Del_Number[3] = 0;
// 	CIM_Information.Add_Del_Number[4] = 0;
// 	CIM_Information.Add_Del_Number[5] = 0;
// //	ThreadStage.nDropDirection = 1;
// //#if PLC
// 	ThreadStage.JobStep = 0;
// 	g_bLcSupply = TRUE;
// 	g_nSupplyHead = 0;
// 	g_nTotalGlassCount =1;
// 	g_bLcSupplyUsed = TRUE;
// 	pView->KillThread(THREAD_IO);
// 	pView->KillThread(THREAD_ORIGINALL);
// 	pView->KillThread(STAGE_AUTO);
// 	pView->RunThread(STAGE_AUTO);
// 	g_bCanster1WeightAlarm=g_bCanster2WeightAlarm=FALSE;
// 	ThreadStage.JobModeSelection = 0;
// 	ThreadStage.nDropDirection = 1;
//	ThreadStage.ExitFlag = TRUE;
//#endif
/********************************************************************************************************************************************************/

	//
	if(PC_TYPE==TRUE) 
	{
		SetTimer(TIMER_AUTO,200,NULL); // SubTimerAutoFunc()
		SetTimer(TIMER_AUTO_CLOCK,1000,NULL); // SubTimerAutoFunc()
		SetTimer(TIMER_AUTO_CYCLE_TIME,500,NULL);  
//2014.04.21 by tskim TPD
//		SetTimer(TIMER_AUTO_TPD,500,NULL);

		//Unit Auto Mode만 살리자.
		pView->m_pDevice->MNET_BitSet(DISP_BIT+0x05,TRUE); //UnitAutoMode
		//Auto Mode 전환..
		pView->m_pDevice->MNET_WriteBit(DISP_OP_MODE,"0001");//AutoMode

//		pView->m_pDevice->MNET_BitSet(DISP_OP_MODE,TRUE); //AutoMode
//		pView->m_pDevice->MNET_BitSet(DISP_OP_MODE+0x01,FALSE);//Manual Mode
	
		// 장치자동Mode On
//		pView->m_pDevice->MNET_BitSet(BaseAddr_DISP+BitDispModeStatus+0x05, true);
		// Loop Count BackUp in Temp Area// memcpy( dest, source , count ) // auto run에서 이용되는 count
		memcpy(m_nLoopCountBK,pDoc->m_structAdjustCondition.nLoopCount ,sizeof(int)*MAX_NOZZLE);
	}

	//lbg 잠시
	//2007.07.17 By tskim
	//Target값과 측정Data값이 일치하지 않을 경우 Alarm
	/*
	double error_percentage = 0.0;
	for(int j =0 ; j <MAX_NOZZLE; j++) // 12개 Head 별로..
	{
		_bSpecOver[j] = FALSE;
		for(i =0 ; i <1 ; i ++) // 3개 비교
		{
			error_percentage = (pDoc->m_dMeasuredData[j][i] - pDoc->m_structAdjustCondition.dTargetWeight[j])*100/pDoc->m_structAdjustCondition.dTargetWeight[j];
			if(pDoc->m_bIsHeadSelected[j]==TRUE)
			{
				if(fabs(error_percentage) <= pDoc->m_structAdjustCondition.dTargetError[j])
				{
					_bSpecOver[j] = FALSE;
				}
				else
				{
					_bSpecOver[j] = TRUE;
					break;
				}
			}
			else
			{
				_bSpecOver[j] = FALSE;
			}
		}
	}

	for(i=0; i<MAX_NOZZLE;i++)
	{
		if(_bSpecOver[i])
		{
			CErrMsg dlg;
			dlg.m_nErrCode = 188;
			dlg.DoModal();
			return FALSE;
		}
	}*/

	// by ckh 2009.01.17
	m_bStepNotSelError = FALSE;
	m_bAdjustErrRange = FALSE;	
	// by ckh 
	m_ctrlDotNum.SetFontWidth(3);
	// by ckh 2009.03.09
	m_bBuzzer = TRUE;

	//중앙 공급 압력을 normal 상태로 변경...//0.2mpa//
	FAS_SetIoBit(3,false,SUPPLY_CHANGE_SOL,TRUE);
	Sleep(10);

	//by shin//2009.08.25//MC 관련 TAS 추가...//
	pView->WriteTasMCData(TAS_MC, 0, ALL_OFF); 
	
	GetDlgItem(IDC_LABEL_RECIPE)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );

//2010.03.10 by tskim
	if(pDoc->m_structDataEditor.m_bInteractiveDrop)
	{
		if(pView->m_nLanguage == 0)
		str.Format("양방향 Drop ON");
		else if(pView->m_nLanguage == 1)
		str.Format("Duplex Drop ON");
		else if(pView->m_nLanguage == 2)
		str.Format("兩方向 Drop ON");

		m_ctrlInterDrop.SetBackColor(BLUE);
	}
	else
	{
		if(pView->m_nLanguage == 0)
		str.Format("양방향 Drop OFF");
		else if(pView->m_nLanguage == 1)
		str.Format("Duplex Drop OFF");
		else if(pView->m_nLanguage == 2)
		str.Format("兩方向 Drop OFF");


		m_ctrlInterDrop.SetBackColor(RED);
	}
	m_ctrlInterDrop.SetCaption(str);
	if(pDoc->m_structDataEditor.m_bMarkAlignInsp)
	{
		str.Format("Mark Align ON");
		m_ctrlMarkInsp.SetBackColor(BLUE);
	}
	else
	{
		str.Format("Mark Align OFF");
		m_ctrlMarkInsp.SetBackColor(RED);
	}
	m_ctrlMarkInsp.SetCaption(str);

	if(pDoc->m_structDataEditor.m_bUse_Vision)		//ehji 141102
	{
		str.Format("Vision USE");
		m_ctrlvisionuse.SetBackColor(BLUE);
	}
	else
	{
		str.Format("Vision NOUSE");
		m_ctrlvisionuse.SetBackColor(RED);
	}
	m_ctrlvisionuse.SetCaption(str);

//2011.04.27 by tskim
	for(i=0;i<MAX_BAL;i++)
	{
		Balstate.bBalNormalState[i] = TRUE;
		ThBal[i].nBalanceMode = 0;
		ThBal[i].bAbnormalSpecOut = FALSE;

	}

//2011.04.27 by tskim
	SetTimer(TIMER_AUTO_BAL_MODE,1000,NULL);

	CString strTemp1;
	str.Empty();
	strTemp1.Empty();
	if(pDoc->m_structDataEditor.m_bSelectDropMode)
	{
		if ( pDoc->m_structDataEditor.m_bMMGLineDropMode )
		{
			strTemp1.Format("MMG Line Drop Mode / ");
			str += strTemp1;
		}
		else
		{
			strTemp1.Format("Line Drop Mode / ");
			str += strTemp1;
		}
	}
	else
	{
		strTemp1.Format("General Drop Mode / ");
		str += strTemp1;
	}
	if(pDoc->m_structDataEditor.m_nNzlMode == 0)
	{
		strTemp1.Format("1 Nozzle Mode\n");
		str += strTemp1;
	}
	else if(pDoc->m_structDataEditor.m_nNzlMode == 1)
	{
		strTemp1.Format("N-Type Mode\n");
		str += strTemp1;
	}
	else if(pDoc->m_structDataEditor.m_nNzlMode == 2)
	{
		strTemp1.Format("Multi Nozzle Mode\n");
		str += strTemp1;
	}
	if(pDoc->m_structDataEditor.m_nCalibrationMode == 0)// 0 : Adjust  1 : 3 point 2 : Full Point(For Flexible)
	{
		strTemp1.Format("Adjust Mode / ");
		str += strTemp1;
	}
	else if(pDoc->m_structDataEditor.m_nCalibrationMode == 1)
	{
		strTemp1.Format("Calibration<General> Mode / ");
		str += strTemp1;
	}
	else if(pDoc->m_structDataEditor.m_nCalibrationMode == 2)
	{
		strTemp1.Format("Calibration<Flexible> Mode / ");
		str += strTemp1;
	}
	if(pDoc->m_structDataEditor.m_bUse_Vision)
	{
		strTemp1.Format("Vision System Use / ");
		str += strTemp1;
	}
	else
	{
		strTemp1.Format("Vision System No Use / ");
		str += strTemp1;
	}
//2014.11.29 by tskim
// 	if(g_OverClock_Use == TRUE)
// 	{
// 		strTemp1.Format("Over Clock O / ");
// 		str += strTemp1;		
// 	}
// 	else
// 	{
// 		strTemp1.Format("Over Clock X / ");
// 		str += strTemp1;		
// 	}
	if(g_bRMS_AddData_ReportOnOff == TRUE)	
	{
		strTemp1.Format("RMS_ADD O");
		str += strTemp1;
	}
	else
	{
		strTemp1.Format("RMS_ADD X");
		str += strTemp1;
	}
	m_ctrlRecipeInfo.SetBackColor(MAGENTA);
	m_ctrlRecipeInfo.SetForeColor(YELLOW);
	m_ctrlRecipeInfo.SetCaption(str);

//	if(pDoc->m_structDataEditor.m_nNzlMode == 1)
//	{
//		m_ctrlAutoDummyDrop.SetEnabled(FALSE); 
//	}
		
	m_strLog.Format("AutoRun In Mode Select : %s",str);
	pView->SaveLog(0,m_strLog);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAuto::OnTimer(UINT nIDEvent) 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	// TODO: Add your message handler code here and/or call default

	switch(nIDEvent)
	{
	case TIMER_AUTO: // 0
		SubTimerAutoFunc();
		break;
	case TIMER_AUTO_TFDATAOUT:
/*		if((m_pLcView->m_pDevice->MNET_Read_A_Bit(BaseAddr_TF+BitTFSendIF, 1))==TRUE)
		{
			m_pLcView->m_pDevice->MNET_BitSet(BaseAddr_DISP+BitDispReceiveIF+0x05, true);
		}
*/		break;
	case TIMER_AUTO_CLOCK:
		m_pTime = CTime::GetCurrentTime();
		m_strTime.Format("%02d:%02d",m_pTime.GetHour(),m_pTime.GetMinute());
		SetDlgItemText(IDC_STATIC_TIME_AUTO,m_strTime);
		break;
	case TIMER_AUTO_CYCLE_TIME:
		SubTimerAutoCycleTimeDisplay();
		break;
	case TIMER_AUTO_BAL_MODE:
		if(m_nJobStep >= 100 && m_nJobStep <150)
		{
			if(ThBal[BALID1].nBalanceMode == 0)
			{
				m_ctrlAutoMeaData.SetRow(0);
				m_ctrlAutoMeaData.SetCol(0);
				if(m_ctrlAutoMeaData.GetCellBackColor() == LIGHTGRAY)
					m_ctrlAutoMeaData.SetCellBackColor(GREEN);	
				else 
					m_ctrlAutoMeaData.SetCellBackColor(LIGHTGRAY);
				m_ctrlAutoMeaData.SetTextMatrix(0,0,"BAL 정상모드");
			}	
			else
			{
				m_ctrlAutoMeaData.SetRow(0);
				m_ctrlAutoMeaData.SetCol(0);
				if(m_ctrlAutoMeaData.GetCellBackColor() == LIGHTGRAY)
					m_ctrlAutoMeaData.SetCellBackColor(RED);	
				else 
					m_ctrlAutoMeaData.SetCellBackColor(LIGHTGRAY);
				m_ctrlAutoMeaData.SetTextMatrix(0,0,"BAL 지진모드");
			}
		}
		break;
//2014.04.21 by tskim
	case TIMER_AUTO_TPD:
		if(g_bMotorLoad_Report == TRUE)
			SubTimerAutoTPD();
		break;
/*	case TIMER_AUTO_SPEC_TIME:
		SubTimerAutoSpecTime();
		break;*/
//	case TIMER_LC_SUPPLY:
//		SubTimerSupplyFunc();
//		break;
	default:
		AfxMessageBox("디버그code를 넣어요?");
		break;
	}// end of switch(nIDEvent)
//	
	CDialog::OnTimer(nIDEvent);
}

void CAuto::OnOK() 
{
	// TODO: Add extra validation here
	
//	CDialog::OnOK();
}

void CAuto::OnCancel() 
{
	// TODO: Add extra cleanup here
	
//	CDialog::OnCancel();
}

void CAuto::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CPen pen;
	pen.CreatePen(PS_SOLID,5,DARKBLUE);
	CPen *pOldPen = (CPen *)dc.SelectObject(&pen);

	dc.MoveTo(0,HEIGHT_YPOS);
	dc.LineTo(SCREEN_WIDTH,HEIGHT_YPOS);

	dc.MoveTo(0,SCREEN_HEIGHT - HEIGHT_YPOS);
	dc.LineTo(SCREEN_WIDTH,SCREEN_HEIGHT - HEIGHT_YPOS);

	dc.SelectObject(pOldPen);
	// Do not call CDialog::OnPaint() for painting messages
}

BEGIN_EVENTSINK_MAP(CAuto, CDialog)
    //{{AFX_EVENTSINK_MAP(CAuto)
	ON_EVENT(CAuto, IDC_AUTO_LOAD, -600 /* Click */, OnClickAutoLoad, VTS_NONE)
	ON_EVENT(CAuto, IDC_AUTO_DROP, -600 /* Click */, OnClickAutoDrop, VTS_NONE)
	ON_EVENT(CAuto, IDC_AUTO_UNLOAD, -600 /* Click */, OnClickAutoUnload, VTS_NONE)
	ON_EVENT(CAuto, IDC_AUTO_STEP, -600 /* Click */, OnClickAutoStep, VTS_NONE)
	ON_EVENT(CAuto, IDC_AUTO_CONTINUOUS, -600 /* Click */, OnClickAutoContinuous, VTS_NONE)
	ON_EVENT(CAuto, IDC_AUTO_SCRAP, -600 /* Click */, OnClickAutoScrap, VTS_NONE)
	ON_EVENT(CAuto, IDC_CMD_RETURN, -600 /* Click */, OnClickCmdReturn, VTS_NONE)
	ON_EVENT(CAuto, IDC_CMD_AUTOINTERFACE, -600 /* Click */, OnClickCmdAutointerface, VTS_NONE)
	ON_EVENT(CAuto, IDC_AUTO_MEASURE, -600 /* Click */, OnClickAutoMeasure, VTS_NONE)
	ON_EVENT(CAuto, IDC_CMD_MEASUREDDATA_DRAW, -600 /* Click */, OnClickCmdMeasureddataDraw, VTS_NONE)
	ON_EVENT(CAuto, IDC_AUTO_DUMMYDROP, -600 /* Click */, OnClickAutoDummydrop, VTS_NONE)
	ON_EVENT(CAuto, IDC_CMD_GLS_CNT_CLEAR, -600 /* Click */, OnClickCmdGlsCntClear, VTS_NONE)
	ON_EVENT(CAuto, IDC_MSFLEXGRID_GLASS_PROCESS, -600 /* Click */, OnClickMsflexgridGlassProcess, VTS_NONE)
	ON_EVENT(CAuto, IDC_ON_OFF_LINEMODE, -600 /* Click */, OnClickOnOffLinemode, VTS_NONE)
	ON_EVENT(CAuto, IDC_MSFLEXGRID_AUTO_WORKDATA, -600 /* Click */, OnClickMsflexgridAutoWorkdata, VTS_NONE)
	ON_EVENT(CAuto, IDC_AUTO_ERROR_HISTORY, -600 /* Click */, OnClickAutoErrorHistory, VTS_NONE)
	ON_EVENT(CAuto, IDC_MSFLEXGRID_AUTO_MEADATA, -600 /* Click */, OnClickMsflexgridAutoMeadata, VTS_NONE)
	ON_EVENT(CAuto, IDC_GLS_DATA_CHANGE, -600 /* Click */, OnClickGlsDataChange, VTS_NONE)
	ON_EVENT(CAuto, IDC_GREASE2, -600 /* Click */, OnClickGrease2, VTS_NONE)
	ON_EVENT(CAuto, IDC_GLS_DATA_REQUEST, -600 /* Click */, OnClickGlsDataRequest, VTS_NONE)
	ON_EVENT(CAuto, IDC_LC_REMAIN, -600 /* Click */, OnClickLcRemain, VTS_NONE)
	ON_EVENT(CAuto, IDC_CMD_AUTO_TITLE, -601 /* DblClick */, OnDblClickCmdAutoTitle, VTS_DISPATCH)
	ON_EVENT(CAuto, IDC_ALIGN_VIEW, -600 /* Click */, OnClickAlignView, VTS_NONE)
	ON_EVENT(CAuto, IDC_CMD_WARMMING_UP, -600 /* Click */, OnClickCmdWarmmingUp, VTS_NONE)
	ON_EVENT(CAuto, IDC_RECIPE_INFO, -600 /* Click */, OnClickRecipeInfo, VTS_NONE)
	ON_EVENT(CAuto, IDC_CMD_LOAD_SENSOR, -600 /* Click */, OnClickCmdLoadSensor, VTS_NONE)
	ON_EVENT(CAuto, IDC_CMD_JOB_MOVE, -600 /* Click */, OnClickCmdJobMove, VTS_NONE)
	ON_EVENT(CAuto, IDC_CMD_Valc_Run, -600 /* Click */, OnClickCMDValcRun, VTS_NONE)
	ON_EVENT(CAuto, IDC_AUTO_VISION, -600 /* Click */, OnClickAutoVision, VTS_NONE)
	ON_EVENT(CAuto, IDC_CMD_LOG_WRITE, -600 /* Click */, OnClickCmdLogWrite, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CAuto::OnClickAutoDummydrop() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(ThreadStage.ExitFlag == FALSE) return;
	ThreadStage.JobStep = 80;

	CString strTemp;		//log 추가  ehji 141123
	strTemp.Format("AutoDummyDrop Click");
	pView->SaveLog(0, strTemp);

}

void CAuto::OnClickAutoLoad() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(ThreadStage.ExitFlag == FALSE) return;
	ThreadStage.JobStep = 0;

		CString strTemp;		//log 추가  ehji 141123
	strTemp.Format("AutoLoad Click");
	pView->SaveLog(0, strTemp);
}

void CAuto::OnClickAutoDrop() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(ThreadStage.ExitFlag == FALSE) return;
	//ThreadStage.JobStep = 19;
	ThreadStage.JobStep = 19;

	CString strTemp;		//log 추가  ehji 141123
	strTemp.Format("AutoDrop Click");
	pView->SaveLog(0, strTemp);
}

void CAuto::OnClickAutoUnload() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	CString strTemp; 

			//log 추가  ehji 141123
	strTemp.Format("AutuUnload Click");
	pView->SaveLog(0, strTemp);

	if(ThreadStage.ExitFlag == FALSE) return;

	CNormalMsg dlg;
	dlg.m_bTimer=FALSE;

	if(pView->m_pDevice->Stage_glass_check())
	{
		if(pView->m_nLanguage == 0)
		{
			dlg.m_strTitle = _T("확인");
			dlg.m_strMsg1 = " GLASS 반출 전 토출 완료 상태인지 확인 하세요.";
			dlg.m_strMsg2 = " 토출 미완료 상태에서 확인 버튼 클릭 시 ALARM 이 발생합니다. ";
		}

		else if(pView->m_nLanguage == 1)
		{
			dlg.m_strTitle = _T("Check");
			dlg.m_strMsg1 = " Before Unloading GLASS, Check state of Dispenting finish.";
			dlg.m_strMsg2 = " If you Click 'Check Button' at state Charge not complete , ALARM Happen. ";
		}

		else if(pView->m_nLanguage == 2)
		{
			dlg.m_strTitle = _T("確認");
			dlg.m_strMsg1 = " GLASS搬出前請確認是否吐出完了.";
			dlg.m_strMsg2 = "  吐出未完了狀態下點擊確認按?時會發生ALARM. ";
		}

		if( dlg.DoModal() == IDOK )
		{
			ThreadStage.JobStep = 40;

			strTemp.Empty();
			strTemp.Format("GLASS 반출 ( DROP STATUS : %d )" , ThreadStage.nDropStatus ); 
			pView->SaveLog(0, strTemp);
		}
		else 	return; 

	}
	else 
	{
		if(pView->m_nLanguage == 0)
		{
			dlg.m_strTitle = _T("확인");
			dlg.m_strMsg1 = " GLASS 반출 전 토출 완료 상태인지 확인 하세요.";
			dlg.m_strMsg2 = " 토출 미완료 상태에서 확인 버튼 클릭 시 ALARM 이 발생합니다. ";
		}
		else if(pView->m_nLanguage == 1)
		{
			dlg.m_strTitle = _T("Check");
			dlg.m_strMsg1 = " Before Unloading GLASS, Check state of Dispenting finish.";
			dlg.m_strMsg2 = " If you Click 'Check Button' at state Charge not complete , ALARM Happen. ";
		}
		else if(pView->m_nLanguage == 2)
		{
			dlg.m_strTitle = _T("確認");
			dlg.m_strMsg1 = " GLASS搬出前請確認是否吐出完了.";
			dlg.m_strMsg2 = "  吐出未完了狀態下點擊確認按?時會發生ALARM. ";
		}


		dlg.DoModal();
		return; 
	}
}

void CAuto::OnClickAutoMeasure() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(ThreadStage.ExitFlag == FALSE) return;
	ThreadStage.AdjustInitCode=0;
	ThreadStage.JobStep = 100;

	CString strTemp; 
	strTemp.Format("AutoMeasure Click");
	pView->SaveLog(0, strTemp);
}

void CAuto::OnClickAutoStep() 
{
//	if((!ThreadStage.JobOption) || (ThreadStage.ExitFlag == FALSE)) return;
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(ThreadStage.StepStop==TRUE)	return;

	CString strTemp; 
	strTemp.Format("AutoStep Click");
	pView->SaveLog(0, strTemp);

	//by shin//2009.08.25//MC 관련 TAS 추가...//
	//STEP STEP 눌림...//
//	pView->WriteTasMCData(TAS_MC, 0, BIT_ON); 

	if(ThreadStage.JobStep == 63) //동시 취출 비교 부분
	{
		if((pView->m_pDevice->MNET_Read_A_Bit(RobotSend, GLASS_DATA_OUT))==FALSE) 	// TF가 Glass Data 출력을 보내지 않는 것을 확인하고 Step운전으로 전환한다.
		{
			m_ctrlAutoStep.EnableWindow(FALSE);
			m_ctrlAutoContinuous.EnableWindow(FALSE);
			Sleep(1000);
			if((pView->m_pDevice->MNET_Read_A_Bit(RobotSend, GLASS_DATA_OUT))==FALSE)
			{
				ThreadStage.StepStop = true;
				m_ctrlAutoStep.SetBackColor(LIGHTGREEN);
				m_ctrlAutoContinuous.SetBackColor(WHITEGRAY);	
			}
			m_ctrlAutoStep.EnableWindow(TRUE);
			m_ctrlAutoContinuous.EnableWindow(TRUE);
		}
	}
	else if(ThreadStage.JobStep != 63)
	{
		ThreadStage.StepStop = true;
		m_ctrlAutoStep.SetBackColor(LIGHTGREEN);
		m_ctrlAutoContinuous.SetBackColor(WHITEGRAY);	
	}
}

void CAuto::OnClickAutoContinuous() 
{
//if((!ThreadStage.JobOption) || (ThreadStage.ExitFlag == FALSE)) return;

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	//by shin//2009.08.25//MC 관련 TAS 추가...//
	//STEP STEP 해제...//
//	pView->WriteTasMCData(TAS_MC, 0, BIT_OFF); 

	CString strTemp; 
	strTemp.Format("AutoContinuous Click");
	pView->SaveLog(0, strTemp);

	ThreadStage.StepStop = false;
	m_ctrlAutoStep.SetBackColor(WHITEGRAY);
	m_ctrlAutoContinuous.SetBackColor(LIGHTGREEN);	
}

void CAuto::OnClickAutoScrap() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	CString temp_str;

   if(ThreadStage.OffLineMode == TRUE) //Offline
	{
//		m_nKindOfGlass = 0;
		pView->m_pDevice->MNET_BitSet(DISP_BIT+GLASS_PROCESSING, FALSE);
		pView->m_pDevice->MNET_BitSet(DISP_BIT+GLASS_BEING, FALSE);
		return;
	}


	if(PC_TYPE==TRUE) KillTimer(TIMER_AUTO); 
	
	CScrapMsg dlg;
	if(dlg.DoModal()==IDOK)
	{
		pView->SendMessage(WM_CIMINFO,3,0);//Write_GlassData()
		//# 보고코드 APD: 1, Scrap: 2, Recipe Body: 3
		pView->m_pDevice->MNET_WriteWord(APD_REPORT1, "0002", 1); 
		CString str="",strTemp;
		WORD Temp = 0x0000;
		int n=0;
//2010.04.05 by tskim NG Scrap		
		if(m_bNGScrap)
		{
			CIM_Information.NGScrapData = pView->m_pDevice->MNET_ReadWord(SELF_INFO+0x01, 1, TRUE);

			n = pView->m_pDevice->HexConvert(CIM_Information.NGScrapData.GetAt(0))*4096 + 
				pView->m_pDevice->HexConvert(CIM_Information.NGScrapData.GetAt(1))*256 + 
				pView->m_pDevice->HexConvert(CIM_Information.NGScrapData.GetAt(2))*16 + 
				pView->m_pDevice->HexConvert(CIM_Information.NGScrapData.GetAt(3));

			n |= 0x2100;
			
			str.Empty();
			str.Format("%04x", n);
			pView->m_pDevice->MNET_WriteWord(SELF_INFO+0x01, str, 1);
			str.Format("NG Glass SCrap코드 선택 됨!!");
			pView->SaveLog(0, str);
//////////////////////////////////////////////////////////////////////////////
			//# Scrap코드 
			//# 9100: Glass (Sub-Glass) Scrap, 9200: by Data Miss
			//# 9300: by op,     9400: by Eng, 9900: by Other Reason			
//			if(dlg.m_nScrapSelType ==0) str="9100";
//			else if(dlg.m_nScrapSelType ==1) str="9200";
//			else if(dlg.m_nScrapSelType ==2) str="9300";
//			else if(dlg.m_nScrapSelType ==3) str="9400";
//			else if(dlg.m_nScrapSelType ==4) str="9900";
//			else return;
			str="9900";
			temp_str = pView->m_pDevice->MakeHexDatatoPLC(str,4);		
			pView->m_pDevice->MNET_WriteWord(APD_REPORT1+0x01, temp_str, 2); 
///////////////////////////////////////////////////////////////////////////////////
			//# Scrap Report Bit
			//# T1초 trigger
			pView->m_pDevice->MNET_BitSet(DISP_REPORT+0x02, TRUE);
			Sleep((int)(pDoc->m_dTeachData[37][0]*1000.0));
			pView->m_pDevice->MNET_BitSet(DISP_REPORT+0x02, FALSE);

			//# 상류투입가능 ON, idle, Auto Mode, Online State
// 			pView->m_pDevice->MNET_WriteBit(DISP_BIT, "1221");
// 			//# Pos.#1 Glass Exist : Stage가 하나 이므로, 한 Bit만...
// 			pView->m_pDevice->MNET_BitSet(DISP_GLS_POS1, FALSE);		
// 			//# Pos.#1 Glass Exist : Stage가 하나 이므로, 한 Word만...
// 			pView->m_pDevice->MNET_WriteWord(POS_GLS_CODE, "0000", 1);
			
// 			//# Current Recipe No.
// 			//str = CIM_Information.Recipe_No;
// 			//# Glass Count In Unit
// 			str ="0000";
// 			//# Put Into Possible Count
// 			//str +="0001";
// 			pView->m_pDevice->MNET_WriteWord(DISP_WORD+0x0C, str, 2);
		}
		else
		{
//2015.08.24 by tskim SCrap Bit 구분 Start
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CIM_Information.NGScrapData = pView->m_pDevice->MNET_ReadWord(SELF_INFO+0x01, 1, TRUE);

			n = pView->m_pDevice->HexConvert(CIM_Information.NGScrapData.GetAt(0))*4096 + 
				pView->m_pDevice->HexConvert(CIM_Information.NGScrapData.GetAt(1))*256 + 
				pView->m_pDevice->HexConvert(CIM_Information.NGScrapData.GetAt(2))*16 + 
				pView->m_pDevice->HexConvert(CIM_Information.NGScrapData.GetAt(3));

			n |= 0x2000;
			
			str.Empty();
			str.Format("%04x", n);
			pView->m_pDevice->MNET_WriteWord(SELF_INFO+0x01, str, 1);
			str.Format("일반 SCrap 선택 됨!!");
			pView->SaveLog(0, str);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//2015.08.24 by tskim SCrap Bit 구분 End

			//# Scrap코드 
			//# 9100: Glass (Sub-Glass) Scrap, 9200: by Data Miss
			//# 9300: by op,     9400: by Eng, 9900: by Other Reason			
			if(dlg.m_nScrapSelType ==0) str="9100";
			else if(dlg.m_nScrapSelType ==1) str="9200";
			else if(dlg.m_nScrapSelType ==2) str="9300";
			else if(dlg.m_nScrapSelType ==3) str="9400";
			else if(dlg.m_nScrapSelType ==4) str="9900";
			else return;
			temp_str = pView->m_pDevice->MakeHexDatatoPLC(str,4);		
			pView->m_pDevice->MNET_WriteWord(APD_REPORT1+0x01, temp_str, 2); 

			//# Scrap Report Bit
			//# T1초 trigger
			pView->m_pDevice->MNET_BitSet(DISP_REPORT+0x02, TRUE);
			Sleep((int)(pDoc->m_dTeachData[37][0]*1000.0));
			pView->m_pDevice->MNET_BitSet(DISP_REPORT+0x02, FALSE);

			//# 상류투입가능 ON, idle, Auto Mode, Online State
			pView->m_pDevice->MNET_WriteBit(DISP_BIT, "0221");
			//# Pos.#1 Glass Exist : Stage가 하나 이므로, 한 Bit만...
			pView->m_pDevice->MNET_BitSet(DISP_GLS_POS1, FALSE);		
			//# Pos.#1 Glass Exist : Stage가 하나 이므로, 한 Word만...
			pView->m_pDevice->MNET_WriteWord(POS_GLS_CODE, "0000", 1);
			
			//# Current Recipe No.
			//str = CIM_Information.Recipe_No;
			//# Glass Count In Unit
			str ="0000";
			//# Put Into Possible Count
			str +="0001";
			pView->m_pDevice->MNET_WriteWord(DISP_WORD+0x0C, str, 2);	
		}

		/*
		CIM_Information.Recipe_Number = "0000";
		CIM_Information.Glass_ID = "";
		CIM_Information.Glass_add_data1 = "0000";
		CIM_Information.Glass_add_data23 = "00000000";
		CIM_Information.Glass_history1 = "0000";
		CIM_Information.Glass_history2 = "0000";
		CIM_Information.Glass_Type = "";
		CIM_Information.Glass_JudgeCode_Main = "";
		CIM_Information.Glass_JudgeCode_Sub = "";
		CIM_Information.Panel_Information[TFT] = "";
		CIM_Information.Panel_Information[CF] = "";
		CIM_Information.Lot_number = "0000";
		SendMessage(WM_INFODISP,3,0);
		*/

		// 스크랩 관련 log 작성.
		str="";
		str = "Glass 스크랩 : ";
		str += CIM_Information.Glass_ID;
		pView->SaveLog(0,str);
//		pView->m_pDevice->MNET_WriteWord(SELF_INFO+0x01, "00C8", 1);//반출시점으로 이동한다..
	}
	else
	{
		m_bNGScrap = FALSE;
	}
//
	if(PC_TYPE==TRUE) SetTimer(TIMER_AUTO,200,NULL); // SubTimerAutoFunc()
}

void CAuto::OnClickCmdReturn() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CString str="";

	g_bLcSupplyUsed = TRUE;

	//by shin//2009.06.16//
	pDoc->m_bAutoManualFlag = FALSE;

	g_bLogWriteOff = false; 

//
	if(PC_TYPE==TRUE) 
	{
/*	삭제 요청
		if(CIM_Information.GlassSendCount!=0)
		{

			str.Format("반출된 Glass 중 Panel 판정값이 보고되지 않은  Glass가 있습니다.Auto에서 측정을 실시하시겠습니까?");
			if(AfxMessageBox(str,MB_SYSTEMMODAL|MB_YESNO)==IDYES)
			{
				ThreadStage.StepStop=TRUE;
				ThreadStage.AdjustInitCode=0;
				ThreadStage.JobStep = 101;	
				m_ctrlAutoStep.SetBackColor(LIGHTGREEN);
				m_ctrlAutoContinuous.SetBackColor(WHITEGRAY);				
				return;
			}
		}
*/
		KillTimer(TIMER_AUTO); // SubTimerAutoFunc()
		KillTimer(TIMER_AUTO_CLOCK);
		KillTimer(TIMER_AUTO_CYCLE_TIME); 
//2011.04.27 by tskim
		KillTimer(TIMER_AUTO_BAL_MODE);
//2014.04.21 by tskim TPD
		KillTimer(TIMER_AUTO_TPD);

//2010.07.20 by tskim
/*		if(g_bSpecCount) KillTimer(TIMER_AUTO_SPEC_TIME);*/
		pView->SetTimer(ID_CLOCK_TIMER,1000,NULL);
		pView->SetTimer(ID_THREADCHECK_TIMER,500,NULL);
		pView->PostMessage(WM_TIMER,(WPARAM)ID_CLOCK_TIMER,(LPARAM)NULL);
		pView->SetTimer(ID_PDDOWN_CEHCK,1000,NULL);		//ehji 141118

//		if(pDoc->m_structDataEditor.m_nLcSupplyType == 1)
//		{
//			KillTimer(TIMER_LC_SUPPLY);
//		}
		
		//Unit Auto Mode만 죽이자.
		pView->m_pDevice->MNET_BitSet(DISP_BIT+0x05,FALSE); //UnitAutoMode

		//Manual Mode 전환..
		pView->m_pDevice->MNET_WriteBit(DISP_OP_MODE,"0002");//Manual Mode


//		pView->m_pDevice->MNET_BitSet(DISP_OP_MODE,FALSE); //AutoMode
//		pView->m_pDevice->MNET_BitSet(DISP_OP_MODE+0x01,TRUE);//Manual Mode


		// Loop Count Restore To Origin Area // memcpy( dest, source , count )
		memcpy(pDoc->m_structAdjustCondition.nLoopCount , m_nLoopCountBK, sizeof(int)*MAX_NOZZLE);

		pView->m_pDevice->Stop_button_onoff(false);
		pView->m_pDevice->Start_button_onoff(false);
		// auto mode off
//		pView->m_pDevice->MNET_BitSet(BaseAddr_DISP+BitDispModeStatus+0x05, false);
		pView->m_pDevice->MNET_WriteBit(DISP_SEND, "0000");
		pView->m_pDevice->MNET_WriteBit(DISP_RECEIVE, "0000");

		//TAS DATA 영역 초기화//
		for(int i = 0; i <= TAS_DUMMY_DROP; i++)
		{
			pView->WriteTasData(TAS_EXCHANGE+i, 0, ALL_OFF); 
			Sleep(30);
		}
	}
	pDoc->Save_SpecoutCount();
	//

	// by ckh 2009.03.15 부저 끄기
	pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispBuzzer,"0000");

	//by shin//2009.08.25//MC 관련 TAS 추가...//
	pView->WriteTasMCData(TAS_MC, 0, ALL_OFF); 

	if(pView->m_nMachineStatus == 0)
	{
		pView->m_pMcStatus->KillTimer(0);
		pView->m_pMcStatus->CloseWindow();
	}

	DestroyWindow();
}

//////////////////////////////////////////////////////////////////
//////////// 사용자 정의 function	//////////////////////////////
//////////////////////////////////////////////////////////////////
long CAuto::OnDotNumDisp(WPARAM wParam, LPARAM lParam)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	// drop수 계산 
	if( wParam == 0 )
		pDoc->SetValidDropPoint(lParam);
	// drop수 display
	else if(wParam == 1)
		pView->m_pAuto->SubDotNumDisplay(lParam);

	return TRUE;
}

long CAuto::OnTargetWeightMMGDisp(WPARAM wParam, LPARAM lParam)	// wParam : Head 번호
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CString str="";

	str.Format("%.3f",pDoc->m_structAdjustCondition.dTargetWeight[wParam-1]);
	m_ctrlAutoMeaData.SetTextMatrix(1,wParam,str);	

	return TRUE;
}

long CAuto::OnInfoDisp(WPARAM wParam, LPARAM lParam)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	CString val="";
	CString str="";
	CString str1="";

	int temp_number = 0,i=0,nstrLenth=0;;
	double vol_pos =0.0;
	switch(wParam)
	{
		case 1 : // spare
			/*
				str.Format("%2x",lParam);
				for(i=0; i<2 ; i++) CIM_Information.Lot_number.SetAt(i,str.GetAt(i)); 
				m_ctrlAutoWorkData.SetTextMatrix(2,1,CIM_Information.Lot_number);
				break;
				*/
		case 2 : // dropping condition display
				if(pDoc->m_structPatternData[lParam].m_nDispenseMode == 0) val = "Drop";
				else val = "Line";
				m_ctrlAutoDropCondition.SetTextMatrix(0,1,val);
				val.Format("%.3f",(double)(pDoc->m_structPatternData[lParam].m_nTableSpeed));
				m_ctrlAutoDropCondition.SetTextMatrix(1,1,val);
				val.Format("%d",(int)(pDoc->m_structPatternData[lParam].m_nSAxisSpeed));
				m_ctrlAutoDropCondition.SetTextMatrix(2,1,val);
				val.Format("%.0f",(pDoc->m_structAdjustCondition.dSpeed));
				m_ctrlAutoDropCondition.SetTextMatrix(3,1,val);

				if(pDoc->m_structPatternData[lParam].m_nSuctionMode == 0) val = "Chip";
				else val = "Line";
				m_ctrlAutoDropCondition.SetTextMatrix(0,4,val);
				val.Format("%d",(int)(pDoc->m_structPatternData[lParam].m_dSuctionSpeed));
				m_ctrlAutoDropCondition.SetTextMatrix(1,4,val);
				val.Format("%.3f",(double)(pDoc->m_structPatternData[lParam].m_dDischargeSpeed));
				m_ctrlAutoDropCondition.SetTextMatrix(2,4,val);
//2009.10.20 by tskim
				if(pDoc->m_structDataEditor.m_bSelectDropMode == TRUE)
				{
					m_ctrlAutoDropCondition.SetRow(3);
					m_ctrlAutoDropCondition.SetCol(4);
					m_ctrlAutoDropCondition.SetCellBackColor(RED);
					m_ctrlAutoDropCondition.SetTextMatrix(3,4, "L");	
				}
				else
				{
					m_ctrlAutoDropCondition.SetRow(3);
					m_ctrlAutoDropCondition.SetCol(4);
					m_ctrlAutoDropCondition.SetCellBackColor(LIGHTGREEN);
					m_ctrlAutoDropCondition.SetTextMatrix(3,4, "G");
				}
				break;
		case 3 : // CIM data display
				
			//	Recipe No
//				nstrLenth=CIM_Information.Recipe_Number.GetLength();
				// String의 길이를 먼저 비교하는 이유는 GetAt() 함수가 오류를 범하지 않게 하기 위함이다.
//				if(nstrLenth>=4)
//				{
//					temp_number=0;
//					for(i=0;i<2;i++)	temp_number += (pView->m_pDevice->HexConvert(CIM_Information.Recipe_Number.GetAt(i+2))*(int)pow(16,1-i));

//					val.Format("%03d",temp_number);
//					m_ctrlAutoWorkData.SetTextMatrix(0,1,val);
//				}
//				else
//				{
					m_ctrlAutoWorkData.SetTextMatrix(0,1,CIM_Information.Recipe_Number);

//					m_ctrlAutoWorkData.SetTextMatrix(0,1,"없음");
//				}

				
///////
			//	Glass ID
//				val = CIM_Information.Glass_ID;
				val = CIM_Information.GlassID_ASC;
				m_ctrlAutoWorkData.SetTextMatrix(1,1,val);

			//	Lot No
/*				nstrLenth=CIM_Information.Lot_number.GetLength();
				if(nstrLenth>=2)				
				{
					temp_number = 0;
					for(i=0;i<2;i++) temp_number += (pView->m_pDevice->HexConvert(CIM_Information.Lot_number.GetAt(i))*(int)pow(16,1-i));

					val.Format("%03d",temp_number);
					m_ctrlAutoWorkData.SetTextMatrix(2,1,val);
				}*/
//				else
//				{
					m_ctrlAutoWorkData.SetTextMatrix(2,1,CIM_Information.Lot_number);					
//					m_ctrlAutoWorkData.SetTextMatrix(2,1,"없음");
//				}
						
			//	Slot No
/*				nstrLenth=CIM_Information.Slot_number.GetLength();
				if(nstrLenth>=4)				
				{
					temp_number = 0;
					for(i=0;i<2;i++) temp_number += (pView->m_pDevice->HexConvert(CIM_Information.Lot_number.GetAt(i+2))*(int)pow(16,1-i));

					val.Format("%03d",temp_number);
					m_ctrlAutoWorkData.SetTextMatrix(3,1,val);
				}*/
//				else
//				{
					m_ctrlAutoWorkData.SetTextMatrix(3,1,CIM_Information.Slot_number);
//					m_ctrlAutoWorkData.SetTextMatrix(3,1,"없음");
//				}
				break;

		case 4 :
				break;

		case 5 : // Display the remained amount of each Cylinge
//				Amount_Display();
				break;
//2007.08.08 by tskim
//Auto화면 Pattern 폭, drop offset Display
		case 6:
			str.Format("%.3f,%.3f",pDoc->m_structPatternData[pView->m_nRefPatNum].m_dWidthX,pDoc->m_structPatternData[pView->m_nRefPatNum].m_dWidthY);
			m_ctrlAutoDropCondition.SetTextMatrix(1,2,str);
			
			str.Format("%.3f(mm)",pDoc->m_structPatternData[pView->m_nRefPatNum].m_dDropOffset);
			m_ctrlAutoDropCondition.SetTextMatrix(3,2,str);
			break;

		case 7:
			
			pFrame->DoEvents(); //090807 실시간 업데이트 위해 추가
			//Canister 잔량 표기//
			for(i = 0; i < 4; i++)
			{
				str.Format("%.3f",pDoc->m_dCanister[0][i]);
				str1.Format("%.0f",pDoc->m_dCanisterSupplyWeignt[i]);
				m_ctrlCanRemain.SetTextMatrix(1,3-i,str);
				m_ctrlCanRemain.SetTextMatrix(2,3-i,str1);
			}
 			pFrame->DoEvents(); //090807 실시간 업데이트 위해 추가

			break;

		default : break;
	}
	return 1L;
}

long CAuto::OnAmountDisp(WPARAM wParam, LPARAM lParam)
{
	Sub_Amount_Display(FALSE);
	return TRUE;
}
/////////////////// sub function	////////////////////////////
//2007.06.06 by tskim
//3mg add/del 대응 관련 display 부분 수정(변환 된 level값과 변환 된 유효 drop수를 display한다.
void CAuto::SubDotNumDisplay(int nCountMode)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	int i = 0, j = 0 ,pat = 0,head = 0, ValDropTot =0, DropCntTot=0, ValDropTot_Bf = 0 ; /*NoDropTot =0, NoDropDiff = 0;*/
	CString str = _T("");
	CString str1 = _T("");CString str2 = _T("");
	CString strTemp1=_T(""); CString strTemp2=_T("");
//2009.12.04 by tskim;
	int ValDropTot_Line = 0;;

	if(nCountMode == 1) // Auto창 처음열리면..
	{
		for(  i = 0 ; i < 20 ; i++)
			m_ctrlDotNum.SetColWidth(i,535);//472

		for(i = 0; i < 20 ; i++)
		{
			m_ctrlDotNum.SetCol(i);
			
			m_ctrlDotNum.SetRow(0);
			m_ctrlDotNum.SetCellBackColor(LIGHTGRAY);
			m_ctrlDotNum.SetRow(8);
			m_ctrlDotNum.SetCellBackColor(LIGHTGRAY);
			m_ctrlDotNum.SetRow(16);
			m_ctrlDotNum.SetCellBackColor(LIGHTGRAY);
			m_ctrlDotNum.SetRow(24);
			m_ctrlDotNum.SetCellBackColor(LIGHTGRAY);
			
			/*
			if(i%5)	
			{
				m_ctrlDotNum.SetRow(i);
				m_ctrlDotNum.SetCellBackColor(LIGHTGRAY);
			}
			*/
		}

		for( i = 0 ; i < 15 ; i++)	    // Panel 최대 개수 : 80개로 하자.//
		{
			for( j = 0 ; j < 20 ; j++)  // 4 * 20 = 80
			{
				ValDropTot =0; DropCntTot=0; ValDropTot_Bf = 0;		/*	NoDropTot =0;	NoDropDiff = 0;*/
				for( pat = 0 ; pat < pDoc->m_structDataEditor.m_nPatternNum ; pat++)
				{
					for( head = 0 ; head < MAX_NOZZLE ; head++)
					{
						// m_nPanelNo[head] 에 1,2,3..
						if((pDoc->m_structPatternData[pat].m_nSequence > 0) && (pDoc->m_structPatternData[pat].m_nPanelNo[head] == 20*i + j +1))
						{
							m_ctrlDotNum.SetTextMatrix(8*i,j,pDoc->m_strGlassIden[20*i+j]);

							str1.Format(PanelInformationConvert(CIM_Information.Panel_NG_Mark[TFT][20*i+j]));

							if(i < 5)
							{
								strTemp1+=str1;
							}

							str2.Format(PanelInformationConvert(CIM_Information.Panel_NG_Mark[CF][20*i+j]));

							if(i < 5)
							{
								strTemp2+=str2;
							}

							str.Format("%s%s",str1,str2);
							m_ctrlDotNum.SetTextMatrix(8*i+1,j,str);

							str.Format("%d",CIM_Information.Add_Del_Number[20*i+j]);	// 이제는 값(integer)으로 나온다.
							m_ctrlDotNum.SetTextMatrix(8*i+2,j,str);

							ValDropTot_Bf+=pDoc->m_nValidDropPoint_Conv[pat][head];  //
							str.Format("%d",ValDropTot_Bf);
							m_ctrlDotNum.SetTextMatrix(8*i+3,j,str);
//2007.06.06 BY tskim 
//add/del변환  display 관련 수정..(//3mg add/del 대응 관련 display 부분 수정(변환 된 level값과 변환 된 유효 drop수를 display한다.)
							str.Format("%d",pDoc->m_nConversonAddDel[20*i+j]);
							m_ctrlDotNum.SetTextMatrix(8*i+4,j,str);
							m_ctrlDotNum.SetRow(8*i+4);
							m_ctrlDotNum.SetCol(j);
							m_ctrlDotNum.SetCellBackColor(ORANGE);

							ValDropTot+=pDoc->m_nValidDropPoint[pat][head];
							str.Format("%d",ValDropTot);
							m_ctrlDotNum.SetTextMatrix(8*i+5,j,str);
							m_ctrlDotNum.SetRow(8*i+5);
							m_ctrlDotNum.SetCol(j);
							m_ctrlDotNum.SetCellBackColor(ORANGE);
							
//							DropCntTot += pDoc->m_nDropCount[pat][head];
							
							if((pDoc->m_structDataEditor.m_bSelectDropMode == TRUE) && (ThreadStage.JobModeSelection == 0)) //Flexible Line Drop && Auto Drop
							{
								//일단 유효드랍수로 디스플레이...//임시...//
//								ValDropTot+=pDoc->m_nValidDropPoint[pat][head];
//2009.12.04 by tskim 
/*								if(CIM_Information.bNGCell[pDoc->m_structPatternData[pat].m_nPanelNo[head]-1])
								{
									ValDropTot_Line+=pDoc->m_nValidDropPoint[pat][head];
								}
								else
								{
									//ValDropTot+=pDoc->m_nValidDropPoint[pat][head];
									ValDropTot_Line = 9999;
								}*/
								str.Format("%d",ValDropTot_Line);
								m_ctrlDotNum.SetTextMatrix(8*i+6,j,str);
								m_ctrlDotNum.SetRow(8*i+6);
								m_ctrlDotNum.SetCol(j);

								if(fabs(pDoc->m_nFlexibleDropCount[head] - pDoc->m_nTotalCount[pDoc->m_nFlexiblePatNo][head]) <= 3*pDoc->m_structDataEditor.m_nPatternNum)
									m_ctrlDotNum.SetCellBackColor(WHITE);
								else
									m_ctrlDotNum.SetCellBackColor(YELLOW);
							}
							else
							{
								DropCntTot += pDoc->m_nTotalCount[pat][head];
								if(fabs(DropCntTot - ValDropTot) <= 3) //3point정도는 랜덤하게 튄다..//
								{
									DropCntTot = ValDropTot;
								}
								str.Format("%d",DropCntTot);
								m_ctrlDotNum.SetTextMatrix(8*i+6,j,str);
								
								m_ctrlDotNum.SetRow(8*i+6);
								m_ctrlDotNum.SetCol(j);
								if(ValDropTot==DropCntTot)	
									m_ctrlDotNum.SetCellBackColor(WHITE);
								else
									m_ctrlDotNum.SetCellBackColor(YELLOW);
							}
//2015.07.14 by tskim
#if PTN_TARGET
							str.Format("%.3f",pDoc->m_structPatternData[pat].m_dTargetWeight_ByPattern[head]);
#else
							str.Format("%.3f",pDoc->m_structAdjustCondition.dTargetWeight[head]);
#endif
							m_ctrlDotNum.SetTextMatrix(8*i+7,j,str);
							m_ctrlDotNum.SetRow(8*i+7);
							m_ctrlDotNum.SetCol(j);
							m_ctrlDotNum.SetCellBackColor(LIGHTCYAN);
						}						
					}					
				}			
			}
		}
		CIM_Information.Panel_Inform_Convert[TFT].Format(strTemp1);
		CIM_Information.Panel_Inform_Convert[CF].Format(strTemp2);
	} // End  of if(bInit == TRUE)
	else if(nCountMode == 0)
	{
		for(  i = 0 ; i < 20 ; i++)
			m_ctrlDotNum.SetColWidth(i,535);//472

		for(i = 0; i < 20 ; i++)
		{
			m_ctrlDotNum.SetCol(i);
			
			m_ctrlDotNum.SetRow(0);
			m_ctrlDotNum.SetCellBackColor(LIGHTGRAY);
			m_ctrlDotNum.SetRow(8);
			m_ctrlDotNum.SetCellBackColor(LIGHTGRAY);
			m_ctrlDotNum.SetRow(16);
			m_ctrlDotNum.SetCellBackColor(LIGHTGRAY);
			m_ctrlDotNum.SetRow(24);
			m_ctrlDotNum.SetCellBackColor(LIGHTGRAY);
			
			/*
			if(i%5)	
			{
				m_ctrlDotNum.SetRow(i);
				m_ctrlDotNum.SetCellBackColor(LIGHTGRAY);
			}
			*/
		}

		for( i = 0 ; i < 15 ; i++)
		{
			for( j = 0 ; j < 20 ; j++)
			{
				ValDropTot =0, DropCntTot=0; ValDropTot_Bf = 0;	/*NoDropTot =0;	NoDropDiff = 0;*/
				for( pat = 0 ; pat < pDoc->m_structDataEditor.m_nPatternNum ; pat++)
				{
					for( head = 0 ; head < MAX_NOZZLE ; head++)
					{
						if((pDoc->m_structPatternData[pat].m_nSequence > 0) && (pDoc->m_structPatternData[pat].m_nPanelNo[head] == 20*i + j +1))
						{
							m_ctrlDotNum.SetTextMatrix(8*i,j,pDoc->m_strGlassIden[20*i+j]);

							str1.Format(PanelInformationConvert(CIM_Information.Panel_NG_Mark[TFT][20*i+j]));
							if(i < 5)
							{
								strTemp1+=str1;
							}

							str2.Format(PanelInformationConvert(CIM_Information.Panel_NG_Mark[CF][20*i+j]));
							if(i < 5)
							{
								strTemp2+=str2;
							}

							str.Format("%s%s",str1,str2);
							m_ctrlDotNum.SetTextMatrix(8*i+1,j,str);

							str.Format("%d",CIM_Information.Add_Del_Number[20*i+j]);	// 이제는 값(integer)으로 나온다.
							m_ctrlDotNum.SetTextMatrix(8*i+2,j,str);

							ValDropTot_Bf+=pDoc->m_nValidDropPoint_Conv[pat][head];
							str.Format("%d",ValDropTot_Bf);
							m_ctrlDotNum.SetTextMatrix(8*i+3,j,str);
//2007.06.06 BY tskim 
//add/del변환  display 관련 수정..(//3mg add/del 대응 관련 display 부분 수정(변환 된 level값과 변환 된 유효 drop수를 display한다.)
							str.Format("%d",pDoc->m_nConversonAddDel[20*i+j]);
							m_ctrlDotNum.SetTextMatrix(8*i+4,j,str);
							m_ctrlDotNum.SetRow(8*i+4);
							m_ctrlDotNum.SetCol(j);
							m_ctrlDotNum.SetCellBackColor(ORANGE);

							ValDropTot+=pDoc->m_nValidDropPoint[pat][head];
							str.Format("%d",ValDropTot);
							m_ctrlDotNum.SetTextMatrix(8*i+5,j,str);
							m_ctrlDotNum.SetRow(8*i+5);
							m_ctrlDotNum.SetCol(j);
							m_ctrlDotNum.SetCellBackColor(ORANGE);

//							DropCntTot += pDoc->m_nDropCount[pat][head];

							if((pDoc->m_structDataEditor.m_bSelectDropMode == TRUE) && (ThreadStage.JobModeSelection == 0)) //Flexible Line Drop && Auto Drop
							{
								//일단 유효드랍수로 디스플레이...//임시...//
								if(ThreadStage.nDropStatus == 0)
								{
									if(CIM_Information.bNGCell[pDoc->m_structPatternData[pat].m_nPanelNo[head]-1])
									{
										//ValDropTot_Line = pDoc->m_nTotalCount[pat/*LinePattern_Seq_count-1*/][head]; //pDoc->m_nValidDropPoint[pat][head];
										ValDropTot_Line = 0;
//										strLog.Format("Panel NO %d Drop수 %d ",pDoc->m_structPatternData[pat].m_nPanelNo[head],ValDropTot_Line);
									}
									else
									{
										//ValDropTot+=pDoc->m_nValidDropPoint[pat][head];
										ValDropTot_Line = 9999;
										//ValDropTot_Line = pDoc->m_nTotalCount[pat/*LinePattern_Seq_count-1*/][head];
//										strLog.Format("Panel NO %d Drop수 %d ",pDoc->m_structPatternData[pat].m_nPanelNo[head],ValDropTot_Line);
									}

								}
								else if(ThreadStage.nDropStatus == 2)
								{
									if(CIM_Information.bNGCell[pDoc->m_structPatternData[pat].m_nPanelNo[head]-1])
									{
										//ValDropTot_Line = pDoc->m_nTotalCount[pat/*LinePattern_Seq_count-1*/][head]; //pDoc->m_nValidDropPoint[pat][head];
										ValDropTot_Line = CIM_Information.DropPoint[pDoc->m_structPatternData[pat].m_nPanelNo[head]-1];
//										strLog.Format("Panel NO %d Drop수 %d ",pDoc->m_structPatternData[pat].m_nPanelNo[head],ValDropTot_Line);
									}
									else
									{
										//ValDropTot+=pDoc->m_nValidDropPoint[pat][head];
										ValDropTot_Line = 9999;
										ValDropTot_Line = pDoc->m_nTotalCount[pat/*LinePattern_Seq_count-1*/][head];
//										strLog.Format("Panel NO %d Drop수 %d ",pDoc->m_structPatternData[pat].m_nPanelNo[head],ValDropTot_Line);
									}
								}
//2009.12.04 by tskim 
// 								if(CIM_Information.bNGCell[pDoc->m_structPatternData[pat].m_nPanelNo[head]-1])
// 								{
// 									//ValDropTot_Line = pDoc->m_nTotalCount[pat/*LinePattern_Seq_count*/-1][head]; //pDoc->m_nValidDropPoint[pat][head];
// 									ValDropTot_Line = CIM_Information.DropPoint[pDoc->m_structPatternData[pat].m_nPanelNo[head]-1];
// 								}
// 								else
// 								{
// 									//ValDropTot+=pDoc->m_nValidDropPoint[pat][head];
// 									ValDropTot_Line = 9999;
// 								}
//2009.11.23 by tskim 여기 디스플레이 부분 어캐 해야 함...
//								str.Format("%d",ValDropTot);//2009.11.23 by tskim 이건 유효 drop수 일단 죽인다.//2009.11.23 by tskim

//								DropCntTot += pDoc->m_nTotalCount[LinePattern_Seq_count-1][head];
//								if(fabs(DropCntTot - ValDropTot) <= 3*pDoc->m_structDataEditor.m_nPatternNum) //3point정도는 랜덤하게 튄다..//
//								{
//									DropCntTot = ValDropTot;
//								}
//								str.Format("%d",DropCntTot);
//								ValDropTot = pDoc->m_nFlexibleDropCount[head];//pDoc->m_nTotalCount[pDoc->m_nFlexiblePatNo][head];
								str.Format("%d",ValDropTot_Line);
								m_ctrlDotNum.SetTextMatrix(8*i+6,j,str);
								m_ctrlDotNum.SetRow(8*i+6);
								m_ctrlDotNum.SetCol(j);
//2009.11.23 by tskim
								if(fabs(pDoc->m_nFlexibleDropCount[head] - pDoc->m_nTotalCount[LinePattern_Seq_count-1/*pDoc->m_nFlexiblePatNo*/][head]) <= 3*pDoc->m_structDataEditor.m_nPatternNum)
									m_ctrlDotNum.SetCellBackColor(WHITE);
								else
									m_ctrlDotNum.SetCellBackColor(YELLOW);
//2009.11.23 by tskim
//#if EQ
								if(pDoc->m_structDataEditor.m_nNzlMode != 1)//2015.08.29 by tskim
								{
									str.Empty();
									str.Format("FlexPattern %d head %d FlexDropCount %d TotalCount %d",LinePattern_Seq_count,head+1,pDoc->m_nFlexibleDropCount[head],pDoc->m_nTotalCount[pDoc->m_nFlexiblePatNo][head]);
									pView->SaveLog(0,str);
								}
//#endif
							}
							else
							{
								DropCntTot += pDoc->m_nTotalCount[pat][head];
								if(fabs(DropCntTot - ValDropTot) <= 3) //3point정도는 랜덤하게 튄다..//
								{
									DropCntTot = ValDropTot;
								}
								str.Format("%d",DropCntTot);
								m_ctrlDotNum.SetTextMatrix(8*i+6,j,str);

								m_ctrlDotNum.SetRow(7*i+6);
								m_ctrlDotNum.SetCol(j);
								if(ValDropTot==DropCntTot)	
									m_ctrlDotNum.SetCellBackColor(WHITE);
								else
									m_ctrlDotNum.SetCellBackColor(YELLOW);
							}
//2015.07.14 by tskim
#if PTN_TARGET
							str.Format("%.3f",pDoc->m_structPatternData[pat].m_dTargetWeight_ByPattern[head]);
#else
							str.Format("%.3f",pDoc->m_structAdjustCondition.dTargetWeight[head]);
#endif
							m_ctrlDotNum.SetTextMatrix(8*i+7,j,str);
							m_ctrlDotNum.SetRow(8*i+7);
							m_ctrlDotNum.SetCol(j);
							m_ctrlDotNum.SetCellBackColor(LIGHTCYAN);
						}
					}
				}
			}
		}
		CIM_Information.Panel_Inform_Convert[TFT].Format(strTemp1);
		CIM_Information.Panel_Inform_Convert[CF].Format(strTemp2);
	}
	else if(nCountMode == 2)//Vision Drop Count
	{
		for(  i = 0 ; i < 20 ; i++)
			m_ctrlDotNum.SetColWidth(i,535);//472

		for(i = 0; i < 20 ; i++)
		{
			m_ctrlDotNum.SetCol(i);
			
			m_ctrlDotNum.SetRow(0);
			m_ctrlDotNum.SetCellBackColor(LIGHTGRAY);
			m_ctrlDotNum.SetRow(8);
			m_ctrlDotNum.SetCellBackColor(LIGHTGRAY);
			m_ctrlDotNum.SetRow(16);
			m_ctrlDotNum.SetCellBackColor(LIGHTGRAY);
			m_ctrlDotNum.SetRow(24);
			m_ctrlDotNum.SetCellBackColor(LIGHTGRAY);
		}

		for( i = 0 ; i < 15 ; i++)
		{
			for( j = 0 ; j < 20 ; j++)
			{
				ValDropTot =0, DropCntTot=0; ValDropTot_Bf = 0;	/*NoDropTot =0;	NoDropDiff = 0;*/
				for( pat = 0 ; pat < pDoc->m_structDataEditor.m_nPatternNum ; pat++)
				{
					for( head = 0 ; head < MAX_NOZZLE ; head++)
					{
						if((pDoc->m_structPatternData[pat].m_nSequence > 0) && (pDoc->m_structPatternData[pat].m_nPanelNo[head] == 20*i + j +1))
						{
							m_ctrlDotNum.SetTextMatrix(8*i,j,pDoc->m_strGlassIden[20*i+j]);

							str1.Format(PanelInformationConvert(CIM_Information.Panel_NG_Mark[TFT][20*i+j]));
							if(i < 5)
							{
								strTemp1+=str1;
							}

							str2.Format(PanelInformationConvert(CIM_Information.Panel_NG_Mark[CF][20*i+j]));
							if(i < 5)
							{
								strTemp2+=str2;
							}

							str.Format("%s%s",str1,str2);
							m_ctrlDotNum.SetTextMatrix(8*i+1,j,str);

							str.Format("%d",CIM_Information.Add_Del_Number[20*i+j]);	// 이제는 값(integer)으로 나온다.
							m_ctrlDotNum.SetTextMatrix(8*i+2,j,str);

							ValDropTot_Bf+=pDoc->m_nValidDropPoint_Conv[pat][head];
							str.Format("%d",ValDropTot_Bf);
							m_ctrlDotNum.SetRow(8*i+3);m_ctrlDotNum.SetCol(j);
							m_ctrlDotNum.SetCellFontSize(6);
							m_ctrlDotNum.SetTextMatrix(8*i+3,j,str);
//2007.06.06 BY tskim 
//add/del변환  display 관련 수정..(//3mg add/del 대응 관련 display 부분 수정(변환 된 level값과 변환 된 유효 drop수를 display한다.)
							str.Format("%d",pDoc->m_nConversonAddDel[20*i+j]);
							m_ctrlDotNum.SetTextMatrix(8*i+4,j,str);
							m_ctrlDotNum.SetRow(8*i+4);
							m_ctrlDotNum.SetCol(j);
							m_ctrlDotNum.SetCellBackColor(ORANGE);

							if(pDoc->m_structDataEditor.m_nNzlMode == 1)
								ValDropTot+=(pDoc->m_nValidDropPoint[pat][head]);//*N_MAX_NOZZLE;
							else
								ValDropTot+=(pDoc->m_nValidDropPoint[pat][head]);								
							str.Format("%d",ValDropTot);
							m_ctrlDotNum.SetRow(8*i+5);m_ctrlDotNum.SetCol(j);
							m_ctrlDotNum.SetCellFontSize(6);
							m_ctrlDotNum.SetTextMatrix(8*i+5,j,str);
							m_ctrlDotNum.SetRow(8*i+5);
							m_ctrlDotNum.SetCol(j);
							m_ctrlDotNum.SetCellBackColor(ORANGE);

							if((pDoc->m_structDataEditor.m_bSelectDropMode == TRUE) && (ThreadStage.JobModeSelection == 0)) //Flexible Line Drop && Auto Drop
							{
								//일단 유효드랍수로 디스플레이...//임시...//
								if(ThreadStage.nDropStatus == 0)
								{
									if(CIM_Information.bNGCell[pDoc->m_structPatternData[pat].m_nPanelNo[head]-1])
									{
										ValDropTot_Line = 0;
									}
									else
									{
										ValDropTot_Line = 9999;
									}

								}
								else if(ThreadStage.nDropStatus == 2)
								{
									if(CIM_Information.bNGCell[pDoc->m_structPatternData[pat].m_nPanelNo[head]-1])
									{
										//ValDropTot_Line = _NVision.m_structCount_Result.m_nCell_Drop_Count[pDoc->m_structPatternData[pat].m_nPanelNo[head]-1]/N_MAX_NOZZLE;

										//140423 기준 - 미스  
//										ValDropTot_Line = ( _NVision.m_structCount_Result.m_nCell_Drop_Number[pDoc->m_structPatternData[pat].m_nPanelNo[head]-1] 
//														  - _NVision.m_structCount_Result.m_nMiss_Count[pDoc->m_structPatternData[pat].m_nPanelNo[head]-1] ) / N_MAX_NOZZLE;
										ValDropTot_Line = _NVision.m_structCount_Result.m_nMiss_Count[pDoc->m_structPatternData[pat].m_nPanelNo[head]-1] ;
									}
									else
									{
										ValDropTot_Line = 9999;										
									}
								}
								str.Format("%d",ValDropTot_Line);
								m_ctrlDotNum.SetRow(8*i+6);m_ctrlDotNum.SetCol(j);
								m_ctrlDotNum.SetCellFontSize(8);
								m_ctrlDotNum.SetTextMatrix(8*i+6,j,str);
								m_ctrlDotNum.SetRow(8*i+6);
								m_ctrlDotNum.SetCol(j);
//2009.11.23 by tskim
//								if(fabs(ValDropTot - ValDropTot_Line) <= 3*pDoc->m_structDataEditor.m_nPatternNum)

								//CELL별 MISS COUNT 만 DISPLAY
								if ( ValDropTot_Line < pDoc->m_structVision.m_nPanelMissShot[pDoc->m_structPatternData[pat].m_nPanelNo[head]-1][0] )
//								if ( ValDropTot_Line < pDoc->m_structVision.nCell_Miss )
									m_ctrlDotNum.SetCellBackColor(GREEN);
								else
									m_ctrlDotNum.SetCellBackColor(RED);
							}
							else if((pDoc->m_structDataEditor.m_bSelectDropMode == FALSE) && (ThreadStage.JobModeSelection == 0))
							{
								if(ThreadStage.nDropStatus == 0)
								{
									if(CIM_Information.bNGCell[pDoc->m_structPatternData[pat].m_nPanelNo[head]-1])
									{
										ValDropTot_Line = 0;
									}
									else
									{
										ValDropTot_Line = 9999;
									}

								}
								else if(ThreadStage.nDropStatus == 2)
								{
									if(CIM_Information.bNGCell[pDoc->m_structPatternData[pat].m_nPanelNo[head]-1])
									{
										//ValDropTot_Line = _NVision.m_structCount_Result.m_nCell_Drop_Count[pDoc->m_structPatternData[pat].m_nPanelNo[head]-1]/N_MAX_NOZZLE;

										//140423 기준 - 미스  
//										ValDropTot_Line = ( _NVision.m_structCount_Result.m_nCell_Drop_Number[pDoc->m_structPatternData[pat].m_nPanelNo[head]-1] 
//														  - _NVision.m_structCount_Result.m_nMiss_Count[pDoc->m_structPatternData[pat].m_nPanelNo[head]-1] ) / N_MAX_NOZZLE;
										ValDropTot_Line = _NVision.m_structCount_Result.m_nMiss_Count[pDoc->m_structPatternData[pat].m_nPanelNo[head]-1] ;
									}
									else
									{
										ValDropTot_Line = 9999;										
									}
								}

								str.Format("%d",ValDropTot_Line);
								m_ctrlDotNum.SetRow(8*i+6);m_ctrlDotNum.SetCol(j);
								m_ctrlDotNum.SetCellFontSize(8);
								m_ctrlDotNum.SetTextMatrix(8*i+6,j,str);
								m_ctrlDotNum.SetRow(8*i+6);
								m_ctrlDotNum.SetCol(j);
//2009.11.23 by tskim
//								if(fabs(ValDropTot - ValDropTot_Line) <= 3*pDoc->m_structDataEditor.m_nPatternNum)

								//CELL별 MISS COUNT 만 DISPLAY
								if ( ValDropTot_Line < pDoc->m_structVision.m_nPanelMissShot[pDoc->m_structPatternData[pat].m_nPanelNo[head]-1][0] )
//								if ( ValDropTot_Line < pDoc->m_structVision.nCell_Miss)
									m_ctrlDotNum.SetCellBackColor(GREEN);
								else
									m_ctrlDotNum.SetCellBackColor(RED);
								/*
								DropCntTot += _NVision.m_structCount_Result.m_nCell_Drop_Count[pDoc->m_structPatternData[pat].m_nPanelNo[head]-1];
								if(fabs(DropCntTot - ValDropTot) <= 3) //3point정도는 랜덤하게 튄다..//
								{
									DropCntTot = ValDropTot;
								}
								str.Format("%d",DropCntTot);
								m_ctrlDotNum.SetRow(7*i+6);m_ctrlDotNum.SetCol(j);
								m_ctrlDotNum.SetCellFontSize(6);
								m_ctrlDotNum.SetTextMatrix(7*i+6,j,str);

								m_ctrlDotNum.SetRow(7*i+6);
								m_ctrlDotNum.SetCol(j);
								if(ValDropTot==DropCntTot)	
									m_ctrlDotNum.SetCellBackColor(GREEN);
								else
									m_ctrlDotNum.SetCellBackColor(RED);
								*/
							 }
//2015.07.14 by tskim
#if PTN_TARGET
							str.Format("%.3f",pDoc->m_structPatternData[pat].m_dTargetWeight_ByPattern[head]);
#else
							str.Format("%.3f",pDoc->m_structAdjustCondition.dTargetWeight[head]);
#endif
							m_ctrlDotNum.SetTextMatrix(8*i+7,j,str);
							m_ctrlDotNum.SetRow(8*i+7);
							m_ctrlDotNum.SetCol(j);
							m_ctrlDotNum.SetCellBackColor(LIGHTCYAN);
						}
					}
				}
			}
		}
		CIM_Information.Panel_Inform_Convert[TFT].Format(strTemp1);
		CIM_Information.Panel_Inform_Convert[CF].Format(strTemp2);
// 		for(i = 0; i < m_ctrlDropInformation.GetRows()-1; i++)
// 		{
// 			str.Format("%d", i+1);
// 			m_ctrlDropInformation.SetTextMatrix(i+1, 0, str);
// 			m_ctrlDropInformation.SetTextMatrix(i+1, 1, pDoc->m_strGlassIden[i]);
// 			str.Format("%d", _NVision.m_structCount_Result.m_nCell_Drop_Number[i]);
// 			m_ctrlDropInformation.SetTextMatrix(i+1, 2, str);
// 			str.Format("%d", _NVision.m_structCount_Result.m_nCell_Drop_Count[i]);
// 			m_ctrlDropInformation.SetTextMatrix(i+1, 3, str);
// 			str.Format("%d / %d", _NVision.m_structCount_Result.m_nOver_Count[i], _NVision.m_structRecipe_Info_Lc.m_nCell_Permit_Over);
// 			m_ctrlDropInformation.SetTextMatrix(i+1, 4, str);
// 			str.Format("%d / %d", _NVision.m_structCount_Result.m_nMiss_Count[i], _NVision.m_structRecipe_Info_Lc.m_nCell_Permit_Miss);
// 			m_ctrlDropInformation.SetTextMatrix(i+1, 5, str);
// 			m_ctrlDropInformation.SetRow(i+1);
// 			m_ctrlDropInformation.SetCol(6);
// 			if(_NVision.m_structCount_Result.m_nCell_Drop_Number[i] == 0)
// 			{
// 				str = "";
// 				m_ctrlDropInformation.SetCellBackColor(WHITE);
// 			}
// 			else 
// 			{
// 				if(_NVision.m_structCount_Result.m_bCell_Judge[i])
// 				{	
// 					str = "OK";
// 					m_ctrlDropInformation.SetCellBackColor(GREEN);
// 				}
// 				else
// 				{
// 					str = "NG";
// 					m_ctrlDropInformation.SetCellBackColor(GREEN);
// 				}
// 			}
// 			m_ctrlDropInformation.SetTextMatrix(i+1, 6, str);
// 		}
	}
}

void CAuto::SubDisplayRecipeName()
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	CString str = "";

	// Recipe Name 설정
	str.Format("%s|%s", pDoc->m_structOperatorModeRecipeData.strFrontRecipeName, pDoc->m_structOperatorModeRecipeData.strBackRecipeName);

	m_ctrlRecipeName.SetCaption(str);
}

void CAuto::Sub_Amount_Display(BOOL bInit)   // 남아 있는 LC 양을 Progress Control 표현해 주고 사용하지 않는 Head의 것은 0으로 표현 해 주고 .
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	for(int i=0; i<MAX_NOZZLE ;i++)
		m_pQnty[i]=(CProgressCtrl*)GetDlgItem(IDC_PROGRESS_H1+i);   // 

	if(bInit==TRUE)
	{
		for(i=0; i<MAX_NOZZLE ;i++) m_pQnty[i]->SetRange(0,400);// 1000(ml)=1000(g) : 1000 g 짜리 병을 사용하지만 Display는 500 g 부터 하도록 요청

		if(pDoc->m_bIsHeadSelected[0]==TRUE)
		{
			m_DropUse1.SetEnabled(TRUE);
			m_ctrlLCQntyDisp1.SetEnabled(TRUE);

			if(pDoc->m_bDropCountSensorUSE[0]==TRUE) m_DropUse1.SetBackColor(0x00FF80FF);
			else m_DropUse1.SetBackColor(0x00C8D0D4);

			if(pDoc->m_bRemainSensorUSE[0]==TRUE) m_ctrlLCQntyDisp1.SetBackColor(0x00FF80FF);
			else m_ctrlLCQntyDisp1.SetBackColor(0x00C8D0D4);
		}
		else
		{
			m_DropUse1.SetEnabled(FALSE);
			m_ctrlLCQntyDisp1.SetEnabled(FALSE);
			m_DropUse1.SetBackColor(0x00C8D0D4);
			m_ctrlLCQntyDisp1.SetBackColor(0x00C8D0D4);
		}

		if(pDoc->m_bIsHeadSelected[1]==TRUE)
		{
			m_DropUse2.SetEnabled(TRUE);
			m_ctrlLCQntyDisp2.SetEnabled(TRUE);

			if(pDoc->m_bDropCountSensorUSE[1]==TRUE) m_DropUse2.SetBackColor(0x00FF80FF);
			else m_DropUse2.SetBackColor(0x00C8D0D4);

			if(pDoc->m_bRemainSensorUSE[1]==TRUE) m_ctrlLCQntyDisp2.SetBackColor(0x00FF80FF);
			else m_ctrlLCQntyDisp2.SetBackColor(0x00C8D0D4);
		}
		else
		{
			m_DropUse2.SetEnabled(FALSE);
			m_ctrlLCQntyDisp2.SetEnabled(FALSE);
			m_DropUse2.SetBackColor(0x00C8D0D4);
			m_ctrlLCQntyDisp2.SetBackColor(0x00C8D0D4);
		}

		if(pDoc->m_bIsHeadSelected[2]==TRUE)
		{
			m_DropUse3.SetEnabled(TRUE);
			m_ctrlLCQntyDisp3.SetEnabled(TRUE);

			if(pDoc->m_bDropCountSensorUSE[2]==TRUE) m_DropUse3.SetBackColor(0x00FF80FF);
			else m_DropUse3.SetBackColor(0x00C8D0D4);

			if(pDoc->m_bRemainSensorUSE[2]==TRUE) m_ctrlLCQntyDisp3.SetBackColor(0x00FF80FF);
			else m_ctrlLCQntyDisp3.SetBackColor(0x00C8D0D4);
		}
		else
		{
			m_DropUse3.SetEnabled(FALSE);
			m_ctrlLCQntyDisp3.SetEnabled(FALSE);
			m_DropUse3.SetBackColor(0x00C8D0D4);
			m_ctrlLCQntyDisp3.SetBackColor(0x00C8D0D4);
		}

		if(pDoc->m_bIsHeadSelected[3]==TRUE)
		{
			m_DropUse4.SetEnabled(TRUE);
			m_ctrlLCQntyDisp4.SetEnabled(TRUE);

			if(pDoc->m_bDropCountSensorUSE[3]==TRUE) m_DropUse4.SetBackColor(0x00FF80FF);
			else m_DropUse1.SetBackColor(0x00C8D0D4);

			if(pDoc->m_bRemainSensorUSE[3]==TRUE) m_ctrlLCQntyDisp4.SetBackColor(0x00FF80FF);
			else m_ctrlLCQntyDisp4.SetBackColor(0x00C8D0D4);
		}
		else
		{
			m_DropUse4.SetEnabled(FALSE);
			m_ctrlLCQntyDisp4.SetEnabled(FALSE);
			m_DropUse4.SetBackColor(0x00C8D0D4);
			m_ctrlLCQntyDisp4.SetBackColor(0x00C8D0D4);
		}

		if(pDoc->m_bIsHeadSelected[4]==TRUE)
		{
			m_DropUse5.SetEnabled(TRUE);
			m_ctrlLCQntyDisp5.SetEnabled(TRUE);

			if(pDoc->m_bDropCountSensorUSE[4]==TRUE) m_DropUse5.SetBackColor(0x00FF80FF);
			else m_DropUse5.SetBackColor(0x00C8D0D4);

			if(pDoc->m_bRemainSensorUSE[4]==TRUE) m_ctrlLCQntyDisp5.SetBackColor(0x00FF80FF);
			else m_ctrlLCQntyDisp5.SetBackColor(0x00C8D0D4);
		}
		else
		{
			m_DropUse5.SetEnabled(FALSE);
			m_ctrlLCQntyDisp5.SetEnabled(FALSE);
			m_DropUse5.SetBackColor(0x00C8D0D4);
			m_ctrlLCQntyDisp5.SetBackColor(0x00C8D0D4);
		}

		if(pDoc->m_bIsHeadSelected[5]==TRUE)
		{
			m_DropUse6.SetEnabled(TRUE);
			m_ctrlLCQntyDisp6.SetEnabled(TRUE);

			if(pDoc->m_bDropCountSensorUSE[5]==TRUE) m_DropUse6.SetBackColor(0x00FF80FF);
			else m_DropUse6.SetBackColor(0x00C8D0D4);

			if(pDoc->m_bRemainSensorUSE[5]==TRUE) m_ctrlLCQntyDisp6.SetBackColor(0x00FF80FF);
			else m_ctrlLCQntyDisp6.SetBackColor(0x00C8D0D4);
		}
		else
		{
			m_DropUse6.SetEnabled(FALSE);
			m_ctrlLCQntyDisp6.SetEnabled(FALSE);
			m_DropUse6.SetBackColor(0x00C8D0D4);
			m_ctrlLCQntyDisp6.SetBackColor(0x00C8D0D4);
		}

		if(pDoc->m_bIsHeadSelected[6]==TRUE)
		{
			m_DropUse7.SetEnabled(TRUE);
			m_ctrlLCQntyDisp7.SetEnabled(TRUE);

			if(pDoc->m_bDropCountSensorUSE[6]==TRUE) m_DropUse7.SetBackColor(0x00FF80FF);
			else m_DropUse7.SetBackColor(0x00C8D0D4);

			if(pDoc->m_bRemainSensorUSE[6]==TRUE) m_ctrlLCQntyDisp7.SetBackColor(0x00FF80FF);
			else m_ctrlLCQntyDisp7.SetBackColor(0x00C8D0D4);
		}
		else
		{
			m_DropUse7.SetEnabled(FALSE);
			m_ctrlLCQntyDisp7.SetEnabled(FALSE);
			m_DropUse7.SetBackColor(0x00C8D0D4);
			m_ctrlLCQntyDisp7.SetBackColor(0x00C8D0D4);
		}

		if(pDoc->m_bIsHeadSelected[7]==TRUE)
		{
			m_DropUse8.SetEnabled(TRUE);
			m_ctrlLCQntyDisp8.SetEnabled(TRUE);

			if(pDoc->m_bDropCountSensorUSE[7]==TRUE) m_DropUse8.SetBackColor(0x00FF80FF);
			else m_DropUse8.SetBackColor(0x00C8D0D4);

			if(pDoc->m_bRemainSensorUSE[7]==TRUE) m_ctrlLCQntyDisp8.SetBackColor(0x00FF80FF);
			else m_ctrlLCQntyDisp8.SetBackColor(0x00C8D0D4);
		}
		else
		{
			m_DropUse8.SetEnabled(FALSE);
			m_ctrlLCQntyDisp8.SetEnabled(FALSE);
			m_DropUse8.SetBackColor(0x00C8D0D4);
			m_ctrlLCQntyDisp8.SetBackColor(0x00C8D0D4);
		}

		if(pDoc->m_bIsHeadSelected[8]==TRUE)
		{
			m_DropUse9.SetEnabled(TRUE);
			m_ctrlLCQntyDisp9.SetEnabled(TRUE);

			if(pDoc->m_bDropCountSensorUSE[8]==TRUE) m_DropUse9.SetBackColor(0x00FF80FF);
			else m_DropUse9.SetBackColor(0x00C8D0D4);

			if(pDoc->m_bRemainSensorUSE[8]==TRUE) m_ctrlLCQntyDisp9.SetBackColor(0x00FF80FF);
			else m_ctrlLCQntyDisp9.SetBackColor(0x00C8D0D4);
		}
		else
		{
			m_DropUse9.SetEnabled(FALSE);
			m_ctrlLCQntyDisp9.SetEnabled(FALSE);
			m_DropUse9.SetBackColor(0x00C8D0D4);
			m_ctrlLCQntyDisp9.SetBackColor(0x00C8D0D4);
		}
		
		if(pDoc->m_bIsHeadSelected[9]==TRUE)
		{
			m_DropUse10.SetEnabled(TRUE);
			m_ctrlLCQntyDisp10.SetEnabled(TRUE);

			if(pDoc->m_bDropCountSensorUSE[9]==TRUE) m_DropUse10.SetBackColor(0x00FF80FF);
			else m_DropUse10.SetBackColor(0x00C8D0D4);

			if(pDoc->m_bRemainSensorUSE[9]==TRUE) m_ctrlLCQntyDisp10.SetBackColor(0x00FF80FF);
			else m_ctrlLCQntyDisp10.SetBackColor(0x00C8D0D4);
		}
		else
		{
			m_DropUse10.SetEnabled(FALSE);
			m_ctrlLCQntyDisp10.SetEnabled(FALSE);
			m_DropUse10.SetBackColor(0x00C8D0D4);
			m_ctrlLCQntyDisp10.SetBackColor(0x00C8D0D4);
		}

		if(pDoc->m_bIsHeadSelected[10]==TRUE)
		{
			m_DropUse11.SetEnabled(TRUE);
			m_ctrlLCQntyDisp11.SetEnabled(TRUE);

			if(pDoc->m_bDropCountSensorUSE[10]==TRUE) m_DropUse11.SetBackColor(0x00FF80FF);
			else m_DropUse11.SetBackColor(0x00C8D0D4);

			if(pDoc->m_bRemainSensorUSE[10]==TRUE) m_ctrlLCQntyDisp11.SetBackColor(0x00FF80FF);
			else m_ctrlLCQntyDisp11.SetBackColor(0x00C8D0D4);
		}
		else
		{
			m_DropUse11.SetEnabled(FALSE);
			m_ctrlLCQntyDisp11.SetEnabled(FALSE);
			m_DropUse11.SetBackColor(0x00C8D0D4);
			m_ctrlLCQntyDisp11.SetBackColor(0x00C8D0D4);
		}

		if(pDoc->m_bIsHeadSelected[11]==TRUE)
		{
			m_DropUse12.SetEnabled(TRUE);
			m_ctrlLCQntyDisp12.SetEnabled(TRUE);

			if(pDoc->m_bDropCountSensorUSE[11]==TRUE) m_DropUse12.SetBackColor(0x00FF80FF);
			else m_DropUse12.SetBackColor(0x00C8D0D4);

			if(pDoc->m_bRemainSensorUSE[11]==TRUE) m_ctrlLCQntyDisp12.SetBackColor(0x00FF80FF);
			else m_ctrlLCQntyDisp12.SetBackColor(0x00C8D0D4);
		}
		else
		{
			m_DropUse12.SetEnabled(FALSE);
			m_ctrlLCQntyDisp12.SetEnabled(FALSE);
			m_DropUse12.SetBackColor(0x00C8D0D4);
			m_ctrlLCQntyDisp12.SetBackColor(0x00C8D0D4);
		}
		
		if(pDoc->m_bIsHeadSelected[12]==TRUE)
		{
			m_DropUse13.SetEnabled(TRUE);
			m_ctrlLCQntyDisp13.SetEnabled(TRUE);

			if(pDoc->m_bDropCountSensorUSE[12]==TRUE) m_DropUse13.SetBackColor(0x00FF80FF);
			else m_DropUse13.SetBackColor(0x00C8D0D4);

			if(pDoc->m_bRemainSensorUSE[12]==TRUE) m_ctrlLCQntyDisp13.SetBackColor(0x00FF80FF);
			else m_ctrlLCQntyDisp13.SetBackColor(0x00C8D0D4);
		}
		else
		{
			m_DropUse13.SetEnabled(FALSE);
			m_ctrlLCQntyDisp13.SetEnabled(FALSE);
			m_DropUse13.SetBackColor(0x00C8D0D4);
			m_ctrlLCQntyDisp13.SetBackColor(0x00C8D0D4);
		}

		if(pDoc->m_bIsHeadSelected[13]==TRUE)
		{
			m_DropUse14.SetEnabled(TRUE);
			m_ctrlLCQntyDisp14.SetEnabled(TRUE);

			if(pDoc->m_bDropCountSensorUSE[13]==TRUE) m_DropUse14.SetBackColor(0x00FF80FF);
			else m_DropUse14.SetBackColor(0x00C8D0D4);

			if(pDoc->m_bRemainSensorUSE[13]==TRUE) m_ctrlLCQntyDisp14.SetBackColor(0x00FF80FF);
			else m_ctrlLCQntyDisp14.SetBackColor(0x00C8D0D4);
		}
		else
		{
			m_DropUse14.SetEnabled(FALSE);
			m_ctrlLCQntyDisp14.SetEnabled(FALSE);
			m_DropUse14.SetBackColor(0x00C8D0D4);
			m_ctrlLCQntyDisp14.SetBackColor(0x00C8D0D4);
		}

		if(pDoc->m_bIsHeadSelected[14]==TRUE)
		{
			m_DropUse15.SetEnabled(TRUE);
			m_ctrlLCQntyDisp15.SetEnabled(TRUE);

			if(pDoc->m_bDropCountSensorUSE[0]==TRUE) m_DropUse15.SetBackColor(0x00FF80FF);
			else m_DropUse15.SetBackColor(0x00C8D0D4);

			if(pDoc->m_bRemainSensorUSE[0]==TRUE) m_ctrlLCQntyDisp15.SetBackColor(0x00FF80FF);
			else m_ctrlLCQntyDisp15.SetBackColor(0x00C8D0D4);
		}
		else
		{
			m_DropUse15.SetEnabled(FALSE);
			m_ctrlLCQntyDisp15.SetEnabled(FALSE);
			m_DropUse15.SetBackColor(0x00C8D0D4);
			m_ctrlLCQntyDisp15.SetBackColor(0x00C8D0D4);
		}

		if(pDoc->m_bIsHeadSelected[15]==TRUE)
		{
			m_DropUse16.SetEnabled(TRUE);
			m_ctrlLCQntyDisp16.SetEnabled(TRUE);

			if(pDoc->m_bDropCountSensorUSE[15]==TRUE) m_DropUse16.SetBackColor(0x00FF80FF);
			else m_DropUse16.SetBackColor(0x00C8D0D4);

			if(pDoc->m_bRemainSensorUSE[15]==TRUE) m_ctrlLCQntyDisp16.SetBackColor(0x00FF80FF);
			else m_ctrlLCQntyDisp16.SetBackColor(0x00C8D0D4);
		}
		else
		{
			m_DropUse16.SetEnabled(FALSE);
			m_ctrlLCQntyDisp16.SetEnabled(FALSE);
			m_DropUse16.SetBackColor(0x00C8D0D4);
			m_ctrlLCQntyDisp16.SetBackColor(0x00C8D0D4);
		}
	}
	else
	{
		for(i=0; i<MAX_NOZZLE ; i++)
		{
			if(pDoc->m_bIsHeadSelected[i]==TRUE)
			{
				if(pDoc->m_dOffset[0][i]<=400)	m_pQnty[i]->SetPos((int)pDoc->m_dOffset[0][i]);   // 현재 남아 있는 LC 양을 Control로 표현해 준다.
				else	m_pQnty[i]->SetPos(400);

				if(pDoc->m_dOffset[0][i] <= 40) m_pQnty[i]->PostMessage(PBM_SETBARCOLOR, 0, RED);
				else if(pDoc->m_dOffset[0][i] <= 100)	m_pQnty[i]->PostMessage(PBM_SETBARCOLOR, 0, YELLOW);
			}
			else
			{
				m_pQnty[i]->SetPos(0);
			}
			DispLCQntyLabel(i,(int)pDoc->m_dOffset[0][i]);
		}
	}	
	Sub_LC_Amount_Sensor_Disp();

}

void CAuto::Sub_LC_Amount_Sensor_Disp()
{
	// 너무 나열식으로 한 것 같아 짜증난다...^^
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CString str="";
	BOOL bIoResult=FALSE;

	if(pDoc->m_bIsHeadSelected[0]==TRUE)
	{
		m_ctrlLCSensorDisp1.EnableWindow(TRUE);
		str.Format("%2d",ThreadStage.nLCDetectedCount[0]);
		m_ctrlLCSensorDisp1.SetCaption(str);
		FAS_GetIoBit(1,true,LC_LIMIT1,&bIoResult);			
		if(bIoResult==FALSE)
			m_ctrlLCSensorDisp1.SetBackColor(RED);		
		else
			m_ctrlLCSensorDisp1.SetBackColor(WHITEGRAY);
	}
	else
	{
		m_ctrlLCSensorDisp1.EnableWindow(FALSE);
		m_ctrlLCSensorDisp1.SetBackColor(WHITEGRAY);
		m_ctrlLCSensorDisp1.SetCaption("0");
	}
	if(pDoc->m_bIsHeadSelected[1]==TRUE)
	{
		m_ctrlLCSensorDisp2.EnableWindow(TRUE);
		str.Format("%2d",ThreadStage.nLCDetectedCount[1]);
		m_ctrlLCSensorDisp2.SetCaption(str);
		FAS_GetIoBit(1,true,LC_LIMIT2,&bIoResult);			
		if(bIoResult==FALSE)
			m_ctrlLCSensorDisp2.SetBackColor(RED);		
		else
			m_ctrlLCSensorDisp2.SetBackColor(WHITEGRAY);
	}
	else
	{
		m_ctrlLCSensorDisp2.EnableWindow(FALSE);
		m_ctrlLCSensorDisp2.SetBackColor(WHITEGRAY);
		m_ctrlLCSensorDisp2.SetCaption("0");
	}
	if(pDoc->m_bIsHeadSelected[2]==TRUE)
	{
		m_ctrlLCSensorDisp3.EnableWindow(TRUE);
		str.Format("%2d",ThreadStage.nLCDetectedCount[2]);
		m_ctrlLCSensorDisp3.SetCaption(str);
		FAS_GetIoBit(1,true,LC_LIMIT3,&bIoResult);			
		if(bIoResult==FALSE)
			m_ctrlLCSensorDisp3.SetBackColor(RED);		
		else
			m_ctrlLCSensorDisp3.SetBackColor(WHITEGRAY);
	}
	else
	{
		m_ctrlLCSensorDisp3.EnableWindow(FALSE);
		m_ctrlLCSensorDisp3.SetBackColor(WHITEGRAY);
		m_ctrlLCSensorDisp3.SetCaption("0");
	}
	if(pDoc->m_bIsHeadSelected[3]==TRUE)
	{
		m_ctrlLCSensorDisp4.EnableWindow(TRUE);
		str.Format("%2d",ThreadStage.nLCDetectedCount[3]);
		m_ctrlLCSensorDisp4.SetCaption(str);
		FAS_GetIoBit(1,true,LC_LIMIT4,&bIoResult);			
		if(bIoResult==FALSE)
			m_ctrlLCSensorDisp4.SetBackColor(RED);		
		else
			m_ctrlLCSensorDisp4.SetBackColor(WHITEGRAY);
	}
	else
	{
		m_ctrlLCSensorDisp4.EnableWindow(FALSE);
		m_ctrlLCSensorDisp4.SetBackColor(WHITEGRAY);
		m_ctrlLCSensorDisp4.SetCaption("0");
	}
	if(pDoc->m_bIsHeadSelected[4]==TRUE)
	{
		m_ctrlLCSensorDisp5.EnableWindow(TRUE);
		str.Format("%2d",ThreadStage.nLCDetectedCount[4]);
		m_ctrlLCSensorDisp5.SetCaption(str);
		FAS_GetIoBit(1,true,LC_LIMIT5,&bIoResult);			
		if(bIoResult==FALSE)
			m_ctrlLCSensorDisp5.SetBackColor(RED);		
		else
			m_ctrlLCSensorDisp5.SetBackColor(WHITEGRAY);
	}
	else
	{
		m_ctrlLCSensorDisp5.EnableWindow(FALSE);
		m_ctrlLCSensorDisp5.SetBackColor(WHITEGRAY);
		m_ctrlLCSensorDisp5.SetCaption("0");
	}
	if(pDoc->m_bIsHeadSelected[5]==TRUE)
	{
		m_ctrlLCSensorDisp6.EnableWindow(TRUE);
		str.Format("%2d",ThreadStage.nLCDetectedCount[5]);
		m_ctrlLCSensorDisp6.SetCaption(str);
		FAS_GetIoBit(1,true,LC_LIMIT6,&bIoResult);			
		if(bIoResult==FALSE)
			m_ctrlLCSensorDisp6.SetBackColor(RED);		
		else
			m_ctrlLCSensorDisp6.SetBackColor(WHITEGRAY);
	}
	else
	{
		m_ctrlLCSensorDisp6.EnableWindow(FALSE);
		m_ctrlLCSensorDisp6.SetBackColor(WHITEGRAY);
		m_ctrlLCSensorDisp6.SetCaption("0");
	}
	if(pDoc->m_bIsHeadSelected[6]==TRUE)
	{
		m_ctrlLCSensorDisp7.EnableWindow(TRUE);
		str.Format("%2d",ThreadStage.nLCDetectedCount[6]);
		m_ctrlLCSensorDisp7.SetCaption(str);
		FAS_GetIoBit(1,true,LC_LIMIT7,&bIoResult);			
		if(bIoResult==FALSE)
			m_ctrlLCSensorDisp7.SetBackColor(RED);		
		else
			m_ctrlLCSensorDisp7.SetBackColor(WHITEGRAY);
	}
	else
	{
		m_ctrlLCSensorDisp7.EnableWindow(FALSE);
		m_ctrlLCSensorDisp7.SetBackColor(WHITEGRAY);
		m_ctrlLCSensorDisp7.SetCaption("0");
	}
	if(pDoc->m_bIsHeadSelected[7]==TRUE)
	{
		m_ctrlLCSensorDisp8.EnableWindow(TRUE);
		str.Format("%2d",ThreadStage.nLCDetectedCount[7]);
		m_ctrlLCSensorDisp8.SetCaption(str);
		FAS_GetIoBit(1,true,LC_LIMIT8,&bIoResult);			
		if(bIoResult==FALSE)
			m_ctrlLCSensorDisp8.SetBackColor(RED);		
		else
			m_ctrlLCSensorDisp8.SetBackColor(WHITEGRAY);
	}
	else
	{
		m_ctrlLCSensorDisp8.EnableWindow(FALSE);
		m_ctrlLCSensorDisp8.SetBackColor(WHITEGRAY);
		m_ctrlLCSensorDisp8.SetCaption("0");
	}
	if(pDoc->m_bIsHeadSelected[8]==TRUE)
	{
		m_ctrlLCSensorDisp9.EnableWindow(TRUE);
		str.Format("%2d",ThreadStage.nLCDetectedCount[8]);
		m_ctrlLCSensorDisp9.SetCaption(str);
		FAS_GetIoBit(1,true,LC_LIMIT9,&bIoResult);			
		if(bIoResult==FALSE)
			m_ctrlLCSensorDisp9.SetBackColor(RED);		
		else
			m_ctrlLCSensorDisp9.SetBackColor(WHITEGRAY);
	}
	else
	{
		m_ctrlLCSensorDisp9.EnableWindow(FALSE);
		m_ctrlLCSensorDisp9.SetBackColor(WHITEGRAY);
		m_ctrlLCSensorDisp9.SetCaption("0");
	}
	if(pDoc->m_bIsHeadSelected[9]==TRUE)
	{
		m_ctrlLCSensorDisp10.EnableWindow(TRUE);
		str.Format("%2d",ThreadStage.nLCDetectedCount[9]);
		m_ctrlLCSensorDisp10.SetCaption(str);
		FAS_GetIoBit(1,true,LC_LIMIT10,&bIoResult);			
		if(bIoResult==FALSE)
			m_ctrlLCSensorDisp10.SetBackColor(RED);		
		else
			m_ctrlLCSensorDisp10.SetBackColor(WHITEGRAY);
	}
	else
	{
		m_ctrlLCSensorDisp10.EnableWindow(FALSE);
		m_ctrlLCSensorDisp10.SetBackColor(WHITEGRAY);
		m_ctrlLCSensorDisp10.SetCaption("0");
	}
	if(pDoc->m_bIsHeadSelected[10]==TRUE)
	{
		m_ctrlLCSensorDisp11.EnableWindow(TRUE);
		str.Format("%2d",ThreadStage.nLCDetectedCount[10]);
		m_ctrlLCSensorDisp11.SetCaption(str);
		FAS_GetIoBit(1,true,LC_LIMIT11,&bIoResult);			
		if(bIoResult==FALSE)
			m_ctrlLCSensorDisp11.SetBackColor(RED);		
		else
			m_ctrlLCSensorDisp11.SetBackColor(WHITEGRAY);
	}
	else
	{
		m_ctrlLCSensorDisp11.EnableWindow(FALSE);
		m_ctrlLCSensorDisp11.SetBackColor(WHITEGRAY);
		m_ctrlLCSensorDisp11.SetCaption("0");
	}
	if(pDoc->m_bIsHeadSelected[11]==TRUE)
	{
		m_ctrlLCSensorDisp12.EnableWindow(TRUE);
		str.Format("%2d",ThreadStage.nLCDetectedCount[11]);
		m_ctrlLCSensorDisp12.SetCaption(str);
		FAS_GetIoBit(1,true,LC_LIMIT12,&bIoResult);			
		if(bIoResult==FALSE)
			m_ctrlLCSensorDisp12.SetBackColor(RED);		
		else
			m_ctrlLCSensorDisp12.SetBackColor(WHITEGRAY);
	}
	else
	{
		m_ctrlLCSensorDisp12.EnableWindow(FALSE);
		m_ctrlLCSensorDisp12.SetBackColor(WHITEGRAY);
		m_ctrlLCSensorDisp12.SetCaption("0");
	}
	if(pDoc->m_bIsHeadSelected[12]==TRUE)
	{
		m_ctrlLCSensorDisp13.EnableWindow(TRUE);
		str.Format("%2d",ThreadStage.nLCDetectedCount[12]);
		m_ctrlLCSensorDisp13.SetCaption(str);
		FAS_GetIoBit(1,true,LC_LIMIT13,&bIoResult);			
		if(bIoResult==FALSE)
			m_ctrlLCSensorDisp13.SetBackColor(RED);		
		else
			m_ctrlLCSensorDisp13.SetBackColor(WHITEGRAY);
	}
	else
	{
		m_ctrlLCSensorDisp13.EnableWindow(FALSE);
		m_ctrlLCSensorDisp13.SetBackColor(WHITEGRAY);
		m_ctrlLCSensorDisp13.SetCaption("0");
	}
	if(pDoc->m_bIsHeadSelected[13]==TRUE)
	{
		m_ctrlLCSensorDisp14.EnableWindow(TRUE);
		str.Format("%2d",ThreadStage.nLCDetectedCount[13]);
		m_ctrlLCSensorDisp14.SetCaption(str);
		FAS_GetIoBit(1,true,LC_LIMIT14,&bIoResult);			
		if(bIoResult==FALSE)
			m_ctrlLCSensorDisp14.SetBackColor(RED);		
		else
			m_ctrlLCSensorDisp14.SetBackColor(WHITEGRAY);
	}
	else
	{
		m_ctrlLCSensorDisp14.EnableWindow(FALSE);
		m_ctrlLCSensorDisp14.SetBackColor(WHITEGRAY);
		m_ctrlLCSensorDisp14.SetCaption("0");
	}
	if(pDoc->m_bIsHeadSelected[14]==TRUE)
	{
		m_ctrlLCSensorDisp15.EnableWindow(TRUE);
		str.Format("%2d",ThreadStage.nLCDetectedCount[14]);
		m_ctrlLCSensorDisp15.SetCaption(str);
		FAS_GetIoBit(1,true,LC_LIMIT15,&bIoResult);			
		if(bIoResult==FALSE)
			m_ctrlLCSensorDisp15.SetBackColor(RED);		
		else
			m_ctrlLCSensorDisp15.SetBackColor(WHITEGRAY);
	}
	else
	{
		m_ctrlLCSensorDisp15.EnableWindow(FALSE);
		m_ctrlLCSensorDisp15.SetBackColor(WHITEGRAY);
		m_ctrlLCSensorDisp15.SetCaption("0");
	}
	if(pDoc->m_bIsHeadSelected[15]==TRUE)
	{
		m_ctrlLCSensorDisp16.EnableWindow(TRUE);
		str.Format("%2d",ThreadStage.nLCDetectedCount[15]);
		m_ctrlLCSensorDisp16.SetCaption(str);
		FAS_GetIoBit(1,true,LC_LIMIT16,&bIoResult);			
		if(bIoResult==FALSE)
			m_ctrlLCSensorDisp16.SetBackColor(RED);		
		else
			m_ctrlLCSensorDisp16.SetBackColor(WHITEGRAY);
	}
	else
	{
		m_ctrlLCSensorDisp16.EnableWindow(FALSE);
		m_ctrlLCSensorDisp16.SetBackColor(WHITEGRAY);
		m_ctrlLCSensorDisp16.SetCaption("0");
	}
}
// timer 관련 func
void CAuto::SubTimerAutoFunc()
{
//
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	DWORD dwIOResultF=0;
	WORD IoinizerStatus=0;
	BOOL bMiniDoorSafetyOK=FALSE;
	CString str="";
	BOOL bSupplyMinIo = TRUE;//			

	CString strLog;

	int nHead = 0, nNozzle =0;
	UINT nTotalCount = 0;
	CString strTemp1="",	strTemp2="" , strTemp3="";
	WORD wTempValue=0;


	m_nJobStep = ThreadStage.JobStep;
	SubAutoMsgDispFunc(); // message display

	// Auto에서 사용하는 Timer Count..
	if(m_nAutoTimerCount<0)	m_nAutoTimerCount = 0;
	else if(m_nAutoTimerCount<10000)	m_nAutoTimerCount++;
	else m_nAutoTimerCount = 0;

	// Measureing/Adjust 동작 중에 있는 head를 표시해 준다.
	if((m_nAutoTimerCount%4)==0)
	{
		if((ThBal[BALID1].bRunning==TRUE)&&(ThBal[BALID1].bAdjust==TRUE))	
			SubAutoMeaHeadDisp(BAL1);
		if((ThBal[BALID2].bRunning==TRUE)&&(ThBal[BALID2].bAdjust==TRUE))	
			SubAutoMeaHeadDisp(BAL2);
		if((ThBal[BALID3].bRunning==TRUE)&&(ThBal[BALID3].bAdjust==TRUE))	
			SubAutoMeaHeadDisp(BAL3);
		if((ThBal[BALID4].bRunning==TRUE)&&(ThBal[BALID4].bAdjust==TRUE))	
			SubAutoMeaHeadDisp(BAL4);
		if((ThBal[BALID5].bRunning==TRUE)&&(ThBal[BALID5].bAdjust==TRUE))	
			SubAutoMeaHeadDisp(BAL5);
		if((ThBal[BALID6].bRunning==TRUE)&&(ThBal[BALID6].bAdjust==TRUE))	
			SubAutoMeaHeadDisp(BAL6);
		if((ThBal[BALID7].bRunning==TRUE)&&(ThBal[BALID7].bAdjust==TRUE))	
			SubAutoMeaHeadDisp(BAL7);
		if((ThBal[BALID8].bRunning==TRUE)&&(ThBal[BALID8].bAdjust==TRUE))	
			SubAutoMeaHeadDisp(BAL8);
		//
	}

	if(pDoc->m_structDataEditor.m_bIsTrfOnlyMode==TRUE)
	{
		if((m_nAutoTimerCount%10)==0)		
		{
			str.Format("%s|Dummy Run 전용",pDoc->m_structOperatorModeRecipeData.strFrontRecipeName);
			m_ctrlRecipeName.SetCaption(str);
			m_ctrlRecipeName.SetBackColor(RED);	
		} 
		else if((m_nAutoTimerCount%5)==0)
		{			
			str.Format("%s|%s", pDoc->m_structOperatorModeRecipeData.strFrontRecipeName, pDoc->m_structOperatorModeRecipeData.strBackRecipeName);
			m_ctrlRecipeName.SetCaption(str);
			m_ctrlRecipeName.SetBackColor(YELLOW);	
		}		
	}

	//141026 PD DOWN : PD DOWN DISPLAY
	if ( g_bPdReqTimeOver )
	{
		m_ctrlPdDownStatus.SetCaption("PD DOWN TIME OVER");	

		if(( m_nAutoTimerCount%10 ) == 0)
		{
			m_ctrlPdDownStatus.SetBackColor(RED);	
		}
		else if(( m_nAutoTimerCount%5 ) == 0)
		{
			m_ctrlPdDownStatus.SetBackColor(YELLOW);
		}
	}
	else if ( g_bPdDownReq )
	{
		m_ctrlPdDownStatus.SetCaption("PD DOWN");	
		
		if(( m_nAutoTimerCount%10 ) == 0)
		{
			m_ctrlPdDownStatus.SetBackColor(RED);	
		}
		else if(( m_nAutoTimerCount%5 ) == 0)
		{
			m_ctrlPdDownStatus.SetBackColor(YELLOW);
		}
	}
	else if ( g_bPdDownRelease )
	{
		m_ctrlPdDownStatus.SetCaption("PD DOWN RELREASE");	
		
		if(( m_nAutoTimerCount%10 ) == 0)
		{
			m_ctrlPdDownStatus.SetBackColor(BLUE);	
		}
		else if(( m_nAutoTimerCount%5 ) == 0)
		{
			m_ctrlPdDownStatus.SetBackColor(YELLOW);
		}
	}

	//Column K 이동량이 경고량을 초과 할 경우
	if(pDoc->m_dMotorMoveDistance[0][0] > pDoc->m_dMotorMoveDistance[0][1])
	{
		str = " Column K축 구리스 주입!!!";
		m_ctrlGreaseMsg.SetCaption(str);
		if((m_nAutoTimerCount%10)==0)
		{
			m_ctrlGreaseMsg.SetBackColor(RED);	
		}
		else if((m_nAutoTimerCount%5)==0)
		{
			m_ctrlGreaseMsg.SetBackColor(YELLOW);
		}
//		pFrame->DoEvents();
		// by ckh 2009.01.17
		m_ctrlGreaseMsg.MoveWindow(11,74,220,24);

	} 
	//Head S 이동량이 경고량을 초과 할 경우
	else if(pDoc->m_dMotorMoveDistance[1][0] > pDoc->m_dMotorMoveDistance[1][1])
	{
		str = " Head S축 구리스 주입!!!";
		m_ctrlGreaseMsg.SetCaption(str);
		if((m_nAutoTimerCount%10)==0)
		{
			m_ctrlGreaseMsg.SetBackColor(RED);	
		}
		else if((m_nAutoTimerCount%5)==0)
		{
			m_ctrlGreaseMsg.SetBackColor(YELLOW);	
		}
//		pFrame->DoEvents();
		// by ckh 2009.01.17
		m_ctrlGreaseMsg.MoveWindow(11,74,220,24);
	}
	else
	{
		m_ctrlGreaseMsg.MoveWindow(0,0,0,0);
	}	
	
	if(pDoc->m_structDataEditor.m_bSpecialInfo == FALSE)
	{
		str = "ECS 특수 정보(Panel Level / Judge) OFF";
		m_ctrlSpecialInfo.SetCaption(str);
		if((m_nAutoTimerCount%10)==0)
		{
			m_ctrlSpecialInfo.SetBackColor(RED);	
		}
		else if((m_nAutoTimerCount%5)==0)
		{
			m_ctrlSpecialInfo.SetBackColor(YELLOW);	
		}
	}
	else
	{
		m_ctrlSpecialInfo.MoveWindow(0,0,0,0);
	}	


	if(g_bLcSupplyUsed == TRUE) //Canister Type이 일치하는 경우...
	{
		if(g_bCanster1WeightAlarm == TRUE)
		{
			if(pDoc->m_nCanisterBalNo[0]-SUPPLY_BAL1 == 0) //Can#1
			{
				if((m_nAutoTimerCount%10)==0)
				{
					m_ctrlCanRemain.SetRow(0);
					m_ctrlCanRemain.SetCol(3);
					m_ctrlCanRemain.SetCellBackColor(RED);
				}
				else if((m_nAutoTimerCount%5)==0)
				{
					m_ctrlCanRemain.SetRow(0);
					m_ctrlCanRemain.SetCol(3);
					m_ctrlCanRemain.SetCellBackColor(YELLOW);
				}
			}
			else //Can#2
			{
				if((m_nAutoTimerCount%10)==0)
				{
					m_ctrlCanRemain.SetRow(0);
					m_ctrlCanRemain.SetCol(2);
					m_ctrlCanRemain.SetCellBackColor(RED);
				}
				else if((m_nAutoTimerCount%5)==0)
				{
					m_ctrlCanRemain.SetRow(0);
					m_ctrlCanRemain.SetCol(2);
					m_ctrlCanRemain.SetCellBackColor(YELLOW);
				}
			}
		}
		if(g_bCanster2WeightAlarm == TRUE)
		{
			if(pDoc->m_nCanisterBalNo[1]-SUPPLY_BAL1 == 2) //Can#3
			{
				if((m_nAutoTimerCount%10)==0)
				{
					m_ctrlCanRemain.SetRow(0);
					m_ctrlCanRemain.SetCol(1);
					m_ctrlCanRemain.SetCellBackColor(RED);
				}
				else if((m_nAutoTimerCount%5)==0)
				{
					m_ctrlCanRemain.SetRow(0);
					m_ctrlCanRemain.SetCol(1);
					m_ctrlCanRemain.SetCellBackColor(YELLOW);
				}
			}
			else //Can#4
			{
				if((m_nAutoTimerCount%10)==0)
				{
					m_ctrlCanRemain.SetRow(0);
					m_ctrlCanRemain.SetCol(0);
					m_ctrlCanRemain.SetCellBackColor(RED);
				}
				else if((m_nAutoTimerCount%5)==0)
				{
					m_ctrlCanRemain.SetRow(0);
					m_ctrlCanRemain.SetCol(0);
					m_ctrlCanRemain.SetCellBackColor(YELLOW);
				}
			}
		}
		if((g_bCanster1WeightAlarm == FALSE)&&(g_bCanster2WeightAlarm == FALSE))
		{
			if(pDoc->m_nCanisterBalNo[0] != 0)
			{
				if(pDoc->m_nCanisterBalNo[0]-SUPPLY_BAL1 == 0) //Can#1
				{
					m_ctrlCanRemain.SetRow(0);
					m_ctrlCanRemain.SetCol(3);
					m_ctrlCanRemain.SetCellBackColor(GREEN);
				}
				else //Can#2
				{
					m_ctrlCanRemain.SetRow(0);
					m_ctrlCanRemain.SetCol(2);
					m_ctrlCanRemain.SetCellBackColor(GREEN);
				}
			}
			else
			{
				for(int i = 2; i < 4; i++)
				{
					m_ctrlCanRemain.SetRow(0);
					m_ctrlCanRemain.SetCol(i);
					m_ctrlCanRemain.SetCellBackColor(LIGHTGRAY);
				}
			}
			if(pDoc->m_nCanisterBalNo[1] != 0)
			{
				if(pDoc->m_nCanisterBalNo[1]-SUPPLY_BAL1 == 2) //Can#3
				{
					m_ctrlCanRemain.SetRow(0);
					m_ctrlCanRemain.SetCol(1);
					m_ctrlCanRemain.SetCellBackColor(GREEN);
				}
				else //Can#4
				{
					m_ctrlCanRemain.SetRow(0);
					m_ctrlCanRemain.SetCol(0);
					m_ctrlCanRemain.SetCellBackColor(GREEN);
				}
			}
			else
			{
				for(int i = 0; i < 2; i++)
				{
					m_ctrlCanRemain.SetRow(0);
					m_ctrlCanRemain.SetCol(i+2);
					m_ctrlCanRemain.SetCellBackColor(LIGHTGRAY);
				}
			}
//			for(int i = 0; i < 4; i++)
//			{
//				m_ctrlCanRemain.SetRow(0);
//				m_ctrlCanRemain.SetCol(i);
//				m_ctrlCanRemain.SetCellBackColor(LIGHTGRAY);
//			}
		}
	}

//	SubTimeDisplay(); // test
	// by ckh 2009.03.09 추가함. (반송 전용 혹은 패스 모드 제외)
// 매장마다 울리기 위해서는 이부분으로 하기->	if((m_bBuzzer == TRUE) && (pDoc->m_structDataEditor.m_bIsTrfOnlyMode!=TRUE) && (ThreadStage.JobModeSelection != 1))
	if((pDoc->m_structDataEditor.m_bIsTrfOnlyMode!=TRUE) && (ThreadStage.JobModeSelection != 1))
	{
		nHead = 0 ;
		nNozzle = MAX_NOZZLE;
		
		if(pDoc->m_structDataEditor.m_nLcSupplyType == 1)// 자동 공급 일 경우
		{
			if(g_bLcSupplyUsed == TRUE) //Canister Type이 일치하는 경우...
			{
				for(int i = 0; i < MAX_NOZZLE; i++)
				{
					if(pDoc->m_bIsHeadSelected[i]==TRUE)
					{				
						if(i < MAX_NOZZLE/2 && pDoc->m_nCanisterBalNo[0] == 0) // BAL 1,2
						{
							nHead = 0;
							if(pDoc->m_nCanisterBalNo[1] == 0) // BAL 3,4 사용 안할 경우.
								nNozzle = MAX_NOZZLE;
							else
								nNozzle = MAX_NOZZLE/2;
							break;
						}
						else if(pDoc->m_nCanisterBalNo[1] == 0) // BAL 3,4
						{
							nHead = MAX_NOZZLE/2;
							nNozzle = MAX_NOZZLE;
							break;
						}
						else // 밸런스 사용 할 경우.( 밸런스 양이 없을 경우)
						{
							nHead = MAX_NOZZLE ;
							nNozzle = MAX_NOZZLE;
							break;
						}
					}
				}
			}
		}

		for(int i = nHead; i < nNozzle; i++)
		{
			if(pDoc->m_bIsHeadSelected[i]==TRUE)
			{				
				//액정 공급 하한 센서 검사...//
				FAS_GetIoBit(2, TRUE, SUPPLY_MIN1+i, &bSupplyMinIo);
				
				if(bSupplyMinIo != TRUE)
				{
					if(m_bBuzzer == TRUE)
					{
// 						if(g_nTotalGlassCount != nTotalCount) // 매장마다 , Glass Count가 바뀔때 마다.
 						{
// 							m_bBuzzer = TRUE;
// 							nTotalCount = g_nTotalGlassCount;
//							pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispBuzzer,"0004"); //Warning소리
							//by shin//2009.04.03//알람 박스 추가...무조건 알람 소리...
							pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispBuzzer,"0008"); //Warning소리

							m_bBuzzer = FALSE;
							pView->SaveLog(0,"Auto 창 하한 센서 미감지 부저 On");
						}
					}
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					{
						strTemp1.Format("%d,",i+1);
						strTemp2+=strTemp1;
					}
				}
			}
		}
		if(bSupplyMinIo == FALSE)
		{
			str.Format( " Bottle %s 번\n액정 부족!! 액정 공급 필요.",strTemp2,0);
			if(m_bBuzzer == TRUE)
				str= " ☞부저 RESET☜" + str;
				
			m_ctrlLCReMain.SetCaption(str);
			if((m_nAutoTimerCount%10)==0)
			{
				m_ctrlLCReMain.SetBackColor(YELLOW);	
			}
			else if((m_nAutoTimerCount%5)==0)
			{
				m_ctrlLCReMain.SetBackColor(RED);	
			}
	//		pFrame->DoEvents();
			// by ckh 2009.01.17
			m_ctrlLCReMain.MoveWindow(488,69,544,199);
		}
		else
		{
			if(pView->m_nLanguage ==0)
			str = "☞부저 RESET☜ Bottle 액정 잔량(g)";
			else if(pView->m_nLanguage ==1)
			str = "☞Buzzer RESET☜ Bottle Remain(g)";
			else if(pView->m_nLanguage == 2)
			str = "☞Buzzer RESET☜ 液晶餘量(g)";

			m_ctrlLCReMain.SetCaption(str);
			m_ctrlLCReMain.SetBackColor(LIGHTMAGENTA);
			m_ctrlLCReMain.MoveWindow(560,79,400,14);
		}	
	}
	
	//
	
	if(ThreadStage.bMachineRunning == TRUE)
	{
		SubTimeDisplay(); // 소요 time display..
		SubAutoMeasuredDataDisplay(); // measured data display..

		if(pDoc->m_structDataEditor.m_nLcSupplyType == 1)
		{
			if((g_bLcSupplyUsed == TRUE) && (g_bLcSupply == TRUE)) //Canister Type이 일치하는 경우...////LC 공급할 Head가 존재 할 경우...//
			{
				SubTimerSupplyFunc();
			}
		}

		FAS_GetIo(1,false,&dwIOResultF);

		// 현재 Run 상태에서 Start 버튼을 On 시켜 놓는다.
		pView->m_pDevice->Start_button_onoff(true);
		pView->m_pDevice->Stop_button_onoff(false);

		////////////////////////////////////////////
		if(m_nJobStep>=100 && m_nJobStep<200) // only measure step, BAL reply check timer increase..
		{
			if(ThBal[BALID1].nTimerCount > -1) ThBal[BALID1].nTimerCount++; // ThreadBal1
			if(ThBal[BALID2].nTimerCount > -1) ThBal[BALID2].nTimerCount++; // ThreadBal2
			if(ThBal[BALID3].nTimerCount > -1) ThBal[BALID3].nTimerCount++; // ThreadBal3
			if(ThBal[BALID4].nTimerCount > -1) ThBal[BALID4].nTimerCount++; // ThreadBal4
			if(ThBal[BALID5].nTimerCount > -1) ThBal[BALID5].nTimerCount++; // ThreadBal5
			if(ThBal[BALID6].nTimerCount > -1) ThBal[BALID6].nTimerCount++; // ThreadBal6
			if(ThBal[BALID7].nTimerCount > -1) ThBal[BALID7].nTimerCount++; // ThreadBal5
			if(ThBal[BALID8].nTimerCount > -1) ThBal[BALID8].nTimerCount++; // ThreadBal6
		}
		
		if((m_nJobStep>=40 && m_nJobStep<80)&&(g_bLcSupply == TRUE))  // 중앙 공급시 timercount를 증가한다.//g_bLcSupply
		{
			if(ThreadSupplyBal1.nTimerCount > -1) ThreadSupplyBal1.nTimerCount++; // ThreadBal1
			if(ThreadSupplyBal2.nTimerCount > -1) ThreadSupplyBal2.nTimerCount++; // ThreadBal2
		}

		///////////////////////////////////////////
		if(pView->m_pDevice->SST_Check(STOP_SWITCH))
		{
			Sleep(100);
			int iIoCount=0;
			for(int i=0; i<30 ;i++)  	
			   if(pView->m_pDevice->SST_Check(STOP_SWITCH)) iIoCount++;
			//////////////////////////////////////////////
			if(iIoCount > 5) 
			{
										m_bSWCheck = false;//2010.01.19
				//AfxMessageBox("stop버튼 눌림");
				pView->SaveLog(0, "CAuto: STOP switch 눌림");
				pView->SaveLog(1, "CAuto: STOP switch 눌림");
			// 현찬 :	ThreadStage.OffLineMode = FALSE;
				if(m_nJobStep > 0 && m_nJobStep <=6)		// 반입 Step 영역 내에서..
				{
					if((pView->m_pDevice->MNET_Read_A_Bit(RobotSend, ROBOT_SENDING) == FALSE) && 
						(pView->m_pDevice->MNET_Read_A_Bit(RobotSend, GLASS_DATA_OUT)==FALSE)) // TF 배출 동작중이 아닌 경우에...		
					{
						Sleep(100);
						if((pView->m_pDevice->MNET_Read_A_Bit(RobotSend, ROBOT_SENDING) == FALSE)&& 
							(pView->m_pDevice->MNET_Read_A_Bit(RobotSend, GLASS_DATA_OUT)==FALSE)) // TF 배출 동작중이 아닌 경우에...		
						{
							pView->WriteTasData(TAS_LOADING, 3, BIT_OFF); //Glass Data On 대기

							pView->WriteTasData(TAS_LOADING, 2, BIT_OFF); //Glass Request Off
							//# GLS DATA 요구 Off
							pView->m_pDevice->MNET_BitSet(DISP_RECEIVE+GLASS_DATA_REQ, false);	
							
							pView->WriteTasData(TAS_LOADING, 5, BIT_OFF); //수취 간섭 없음 OFF
							//# Load Enable (간섭없음 OFF)
							pView->m_pDevice->MNET_BitSet(DISP_RECEIVE+OUT_OF_AREA, false);  

//							pView->m_pDevice->MNET_BitSet(BaseAddr_DISP+BitDispReceiveIF+0x00, false); 
//							pView->m_pDevice->MNET_BitSet(BaseAddr_DISP+BitDispReceiveIF+0x01, false);  //Glass Data요구 off
//							pView->m_pDevice->MNET_BitSet(BaseAddr_DISP+0x97, false);// 2007.01.19 제우스측 요청으로 추가..yamary


							//TAS DATA 영역 초기화//
							pView->WriteTasData(TAS_LOADING, 0, ALL_OFF); 

							ThreadStage.ExitFlag = true;
							ThreadStage.JobStep=200;
						}
					}
				}
				else if(m_nJobStep>=40 && m_nJobStep<=53)	// 반출 Step 영역내에서..
				{
					if((pView->m_pDevice->MNET_Read_A_Bit(RobotReceive, ROBOT_RECEIVEING))==FALSE) // TF 수취 동작중이 아닌 경우에...
					{
						Sleep(100);
						if((pView->m_pDevice->MNET_Read_A_Bit(RobotReceive, ROBOT_RECEIVEING))==FALSE) // TF 수취 동작중이 아닌 경우에...
						{
							pView->WriteTasData(TAS_EXCHANGE, 6, BIT_OFF); //배출 간섭 없음 OFF
							//# 배출위치 간섭없음 off
							pView->m_pDevice->MNET_BitSet(DISP_SEND+OUT_OF_AREA, false);  
//							pView->m_pDevice->MNET_BitSet(BaseAddr_DISP+BitDispSendIF+0x00, false); 
				
							//TAS DATA 영역 초기화//
							pView->WriteTasData(TAS_EXCHANGE, 0, ALL_OFF); 
							pView->WriteTasData(TAS_LC_SUPPLY, 0, ALL_OFF); 

							ThreadStage.ExitFlag = true;
							ThreadStage.JobStep=200;
						}
					}
				}
				else if(m_nJobStep==63)
				{
					if((pView->m_pDevice->MNET_Read_A_Bit(RobotSend, GLASS_DATA_OUT) == FALSE)&&//Glass Data 출력중
						(pView->m_pDevice->MNET_Read_A_Bit(RobotSend, ROBOT_SENDING) == FALSE)&&//배출 동작중
						(pView->m_pDevice->MNET_Read_A_Bit(RobotReceive, ROBOT_RECEIVEING) == FALSE))//수취동작중
					{
						Sleep(100);
						if((pView->m_pDevice->MNET_Read_A_Bit(RobotSend, GLASS_DATA_OUT) == FALSE)&&//Glass Data 출력중
							(pView->m_pDevice->MNET_Read_A_Bit(RobotSend, ROBOT_SENDING) == FALSE)&&//배출 동작중
							(pView->m_pDevice->MNET_Read_A_Bit(RobotReceive, ROBOT_RECEIVEING) == FALSE))//수취동작중
						{						
							pView->WriteTasData(TAS_EXCHANGE, 7, BIT_OFF); //Robt Glass Dtat 출력 중 대기
						
							pView->WriteTasData(TAS_EXCHANGE, 5, BIT_OFF); //Glass Data Request OFF
							//# GLS DATA 요구 Off
							pView->m_pDevice->MNET_BitSet(DISP_RECEIVE+GLASS_DATA_REQ, false);	
							
							pView->WriteTasData(TAS_EXCHANGE, 9, BIT_OFF); //수취 간섭 없음 OFF
							//# Load Enable (간섭없음 OFF)
							pView->m_pDevice->MNET_BitSet(DISP_RECEIVE+OUT_OF_AREA, false);  

							pView->WriteTasData(TAS_EXCHANGE, 6, BIT_OFF); //배출 간섭 없음 OFF
							//# 배출위치 간섭없음 off
							pView->m_pDevice->MNET_BitSet(DISP_SEND+OUT_OF_AREA, false);  


//							pView->m_pDevice->MNET_BitSet(BaseAddr_DISP+0x97, false);// 2007.01.19 제우스측 요청으로 추가..yamary

							//TAS DATA 영역 초기화//
							pView->WriteTasData(TAS_EXCHANGE, 0, ALL_OFF); 
							pView->WriteTasData(TAS_LC_SUPPLY, 0, ALL_OFF); 

							ThreadStage.ExitFlag = true;
							ThreadStage.JobStep=200;
						}
					}
				}
				else if(m_nJobStep>=100 && m_nJobStep<200)	// 측정 Step 영역 내에서..
				{
					ThBal[BALID1].ExitFlag=true; ThBal[BALID2].ExitFlag=true;
					ThBal[BALID3].ExitFlag=true; ThBal[BALID4].ExitFlag=true;
					ThBal[BALID5].ExitFlag=true; ThBal[BALID6].ExitFlag=true;
					ThBal[BALID7].ExitFlag=true; ThBal[BALID8].ExitFlag=true;
					
//					SendMessage(WM_AUTO_MEA_HEAD,0,BAL1);
//					SendMessage(WM_AUTO_MEA_HEAD,0,BAL2);
//					SendMessage(WM_AUTO_MEA_HEAD,0,BAL3);
//					SendMessage(WM_AUTO_MEA_HEAD,0,BAL4);
//					SendMessage(WM_AUTO_MEA_HEAD,0,BAL5);
//					SendMessage(WM_AUTO_MEA_HEAD,0,BAL6);

					//TAS DATA 영역 초기화//
					pView->WriteTasData(TAS_BALANCE_MEAS, 0, ALL_OFF); 

					ThreadStage.ExitFlag = true;
					ThreadStage.JobStep=200;
				}
				else
				{
					ThreadStage.StepStop = true;
					m_ctrlAutoStep.SetBackColor(LIGHTGREEN);
					m_ctrlAutoContinuous.SetBackColor(WHITEGRAY);	
				}

			}// end of if(iIoCount > 20)
		}
		
		// control
		pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispLamp,"0004");
//		m_ctrlReturn.SetEnabled(true);
		m_ctrlReturn.SetEnabled(false);
		m_ctrlAutoScrap.SetEnabled(false); 
		m_ctrlAdjustInAuto.EnableWindow(false);
		m_ctrlOffLineMode.EnableWindow(false);
//		m_ctrlGrease2.EnableWindow(false);
	}
	else		// 정지 상태에서 Start 버튼 입력을 기다리는 중....
	{
		if(pView->m_nLanguage == 0)
		ThreadStage.strMsg1="장치정지중..";
		else if(pView->m_nLanguage == 1)
		ThreadStage.strMsg1="Machine Stopping..";
		else if(pView->m_nLanguage == 2)
		ThreadStage.strMsg1="設備停止中..";
		
		// Stop 상태에서 
		// Stop 버튼이 On되도록 한다.
		pView->m_pDevice->Stop_button_onoff(true);
		// Start 버튼은 깜박거리게 한다.
		if((m_nAutoTimerCount%4)==0)	pView->m_pDevice->Start_button_onoff(false);
		else if((m_nAutoTimerCount%2)==0)	pView->m_pDevice->Start_button_onoff(true);

		// machine stop상태에서 interface관련 모든 bit는 reset : 이 부분은 View의 Timer로 바뀌었다.
		pView->m_pDevice->MNET_WriteBit(DISP_RECEIVE, "0000");  // JHC_C : Interface Signal all off..
		pView->m_pDevice->MNET_WriteBit(DISP_SEND, "0000");		

		if(pView->m_pDevice->SST_Check(START_SWITCH))
		{	
						m_bSWCheck = false;//2010.01.19
			//조정오차가 측정오차보다 클경우 알림 메시지...//
			for(int i = 0; i < MAX_NOZZLE; i++)
			{
				if(m_dAdjustError > pDoc->m_structAdjustCondition.dTargetError[i])
				{
					m_bAdjustErrRange = TRUE;		
					break;
				}
			}

			if(ThreadStage.JobModeSelection == 1) //반송모드 경우 인터락 해제
			{
				m_bAdjustErrRange = FALSE;
				g_bPumpOringFlag = FALSE;
				g_bAceeptSettingStatus = TRUE;		//ehji 150328
			}
///////////////////////////////////////////////////////////////////////////////////// ehji 150527

			int  nHeadCount = 0; 
			WORD wTempIO , wIOResult1 = 0; 
			Drop_Info.m_dSumDistPerDrop = 0; 
			Drop_Info.m_dMaxDistPerDrop = 0.0; 
			Drop_Info.m_dMinDistPerDrop = 300000.0; 
	
			if ( pDoc->m_structDataEditor.m_bSelectDropMode == TRUE ) 
			{
				for ( i = 0; i < MAX_NOZZLE; i++ )
				{
					if ( pDoc->m_structPatternData[0].m_bIsOnHead[i] )
					{
						if ( ( Drop_Info.m_dMaxDistPerDrop < Drop_Info.m_dFlexibeDistPerDrop[i] )
						  || ( Drop_Info.m_dMinDistPerDrop > Drop_Info.m_dFlexibeDistPerDrop[i] ))
						{
							if ( Drop_Info.m_dMaxDistPerDrop < Drop_Info.m_dFlexibeDistPerDrop[i] )
							{
								Drop_Info.m_dMaxDistPerDrop = Drop_Info.m_dFlexibeDistPerDrop[i];												
							}
						
							if ( Drop_Info.m_dMinDistPerDrop > Drop_Info.m_dFlexibeDistPerDrop[i] )
							{
								Drop_Info.m_dMinDistPerDrop = Drop_Info.m_dFlexibeDistPerDrop[i];												
							}
						}
						Drop_Info.m_dSumDistPerDrop += Drop_Info.m_dFlexibeDistPerDrop[i];
						nHeadCount++; 
					}		
				}

				Drop_Info.m_dSumDistPerDrop = Drop_Info.m_dSumDistPerDrop - ( Drop_Info.m_dMinDistPerDrop + Drop_Info.m_dMaxDistPerDrop );

				Drop_Info.m_dAverageDistPerDrop = Drop_Info.m_dSumDistPerDrop / (nHeadCount - 2);

				strLog.Format("<DistPerDrop 기준값> \t nHeadCount \t%d \t평균값 \t%.2f \t알람 기준 값 \t%.2f"
					 ,nHeadCount ,Drop_Info.m_dAverageDistPerDrop, pDoc->m_structAdjustCondition.m_dDistperdropEmergency);
				pView->SaveLog(0, strLog);
		
				wIOResult1 = 0x0000;

				for( i = 0; i < MAX_NOZZLE; i++)
				{
					if ( pDoc->m_structPatternData[0].m_bIsOnHead[i] )
					{
						Drop_Info.m_dDistPerDropPercent[i]
								= fabs(Drop_Info.m_dFlexibeDistPerDrop[i]-Drop_Info.m_dAverageDistPerDrop) / Drop_Info.m_dAverageDistPerDrop * 100;	
						if ( Drop_Info.m_dDistPerDropPercent[i] > pDoc->m_structAdjustCondition.m_dDistperdropEmergency )
						{
							strLog.Format("<DistPerDrop 기준값 알람>	H%d	Distperdrop %.2f	평균값대비 오차율	%.2f"
								, i+1, Drop_Info.m_dFlexibeDistPerDrop[i], Drop_Info.m_dDistPerDropPercent[i]);
							pView->SaveLog(0, strLog);	

							wTempIO=0x0001;
							wTempIO=wTempIO<<i;
							wIOResult1+=wTempIO; 
							Drop_Info.m_dFlexibeDistPerDrop[i]  = 0;
							g_bCalibrationStatus[i]=FALSE;
							//Drop_Info.bAdjustComplete[i] = FALSE;												
						}
					}

		//			if(m_dAdjustError > pDoc->m_structAdjustCondition.dTargetError[i])
		//			{
		//				m_bAdjustErrRange = TRUE;		
		//				break;
		//			}
				}
				if ( wIOResult1 != 0 )
				{
					pView->PostMessage(WM_ERROR,600,wIOResult1);
					m_bStepNotSelError = FALSE;
				}
			}
			else //150611 HSN General Mode 추가 
			{
				int  nHeadCount = 0; 
				WORD wTempIO , wIOResult1 = 0; 
				
				Drop_Info.m_dSumDistPerDrop = 0; 
				Drop_Info.m_dMaxDistPerDrop = 0.0; 
				Drop_Info.m_dMinDistPerDrop = 300000.0; 
				
				for ( i = 0; i < MAX_NOZZLE; i++ )
				{
					if ( pDoc->m_structPatternData[0].m_bIsOnHead[i] )
					{
						if ( ( Drop_Info.m_dMaxDistPerDrop < Drop_Info.m_dDistPerDrop[i] )
							|| ( Drop_Info.m_dMinDistPerDrop > Drop_Info.m_dDistPerDrop[i] ))
						{
							if ( Drop_Info.m_dMaxDistPerDrop < Drop_Info.m_dDistPerDrop[i] )
							{
								Drop_Info.m_dMaxDistPerDrop = Drop_Info.m_dDistPerDrop[i];												
							}
							
							if ( Drop_Info.m_dMinDistPerDrop > Drop_Info.m_dDistPerDrop[i] )
							{
								Drop_Info.m_dMinDistPerDrop = Drop_Info.m_dDistPerDrop[i];												
							}
						}		
						
						Drop_Info.m_dSumDistPerDrop += Drop_Info.m_dDistPerDrop[i];
						nHeadCount++; 
					}	
				}	
				
				Drop_Info.m_dSumDistPerDrop = Drop_Info.m_dSumDistPerDrop - ( Drop_Info.m_dMinDistPerDrop + Drop_Info.m_dMaxDistPerDrop );
				
				Drop_Info.m_dAverageDistPerDrop = Drop_Info.m_dSumDistPerDrop / (nHeadCount - 2);
				
				strLog.Format("<DistPerDrop 기준값> \t nHeadCount \t%d \t평균값 \t%.2f \t알람 기준 값 \t%.2f"
					,nHeadCount ,Drop_Info.m_dAverageDistPerDrop, pDoc->m_structAdjustCondition.m_dDistperdropEmergency);
				pView->SaveLog(0, strLog);
				
				for( i = 0; i < MAX_NOZZLE; i++)
				{
					if ( pDoc->m_structPatternData[0].m_bIsOnHead[i] )
					{
						Drop_Info.m_dDistPerDropPercent[i]
							= fabs(Drop_Info.m_dDistPerDrop[i]-Drop_Info.m_dAverageDistPerDrop) / Drop_Info.m_dAverageDistPerDrop * 100;
						
						if ( Drop_Info.m_dDistPerDropPercent[i] > pDoc->m_structAdjustCondition.m_dDistperdropEmergency )	
						{
							strLog.Format("<DistPerDrop 기준값 알람>	H%d	Distperdrop %.2f	평균값대비 오차율	%.2f"
								, i+1, Drop_Info.m_dDistPerDrop[i], Drop_Info.m_dDistPerDropPercent[i]);
							pView->SaveLog(0, strLog);
							
							
							wTempIO=0x0001;
							wTempIO=wTempIO<<i;
							wIOResult1+=wTempIO;
							
							Drop_Info.m_dDistPerDrop[i]  = 0;
							Drop_Info.bAdjustComplete[i] = FALSE;												
						}
					}
					
					if(m_dAdjustError > pDoc->m_structAdjustCondition.dTargetError[i])
					{
						m_bAdjustErrRange = TRUE;		
						break;
					}
				}
				
				if ( wIOResult1 != 0 )
				{
					pView->PostMessage(WM_ERROR,600,wIOResult1);
					m_bStepNotSelError = FALSE;
				}
			}
			
			if(ThreadStage.JobModeSelection == 1)
			{
				m_bAdjustErrRange = FALSE;
				g_bPumpOringFlag = FALSE;
			}


//////////////////////////////////////////////////////////////////////////////////////
// MSG 창 일단 삭제. 
// #if TEST
// 			//20101115 OFFLINE MODE 時 토출 완료 GLASS 인지 확인 BY SHLEE
// 			if(ThreadStage.JobStep == 40 && !ThreadStage.JobModeSelection)
// 			{
// 				if(pView->m_pDevice->Stage_glass_check())
// 				{
// 					CNormalMsg dlg;
// 					dlg.m_bTimer=FALSE;  //ehji 수정중...
// 
// 					if(pView->m_nLanguage == 0)
// 					{
// 						dlg.m_strTitle = _T("확인");
// 						dlg.m_strMsg1 = " GLASS 반출 전 토출 완료 상태인지 확인 하세요.";
// 						dlg.m_strMsg2 = " 토출 미완료 상태에서 확인 버튼 클릭 시 ALARM 이 발생합니다. ";
// 					}
// 
// 					else if(pView->m_nLanguage == 1)
// 					{
// 						dlg.m_strTitle = _T("Check");
// 						dlg.m_strMsg1 = " Before Unloading GLASS, Check state of Dispenting finish.";
// 						dlg.m_strMsg2 = " 토출 미완료 상태에서 확인 버튼 클릭 시 ALARM 이 발생합니다. ";
// 					}
// 
// 					else if(pView->m_nLanguage == 2)
// 					{
// 						dlg.m_strTitle = _T("確認");
// 						dlg.m_strMsg1 = " GLASS 搬出前請確認반출 전 토출 완료 상태인지 확인 하세요.";
// 						dlg.m_strMsg2 = " 토출 미완료 상태에서 확인 버튼 클릭 시 ALARM 이 발생합니다. ";
// 					}
// 
// 					if( dlg.DoModal() == IDOK )
// 					{
// 						strTemp3.Format("GLASS 반출 ( DROP STATUS : %d )" , ThreadStage.nDropStatus ); 
// 						pView->SaveLog(0, strTemp3);
// 					}
// 					else 	return; 
// 				}
// 			}
// #endif 
			BOOL bAdjustCompleteDone = TRUE;
			BOOL bMeasureCompleteDone = TRUE;
			BOOL bCalibrationCompleteDone = TRUE; 
			CString strTemp1,strTemp2,strTemp3,strTemp4,strTemp5,strTemp6;


			g_bPumpOringFlag = FALSE; // ehji수정중.

			strTemp1.Empty();strTemp2.Empty();strTemp3.Empty();
			strTemp4.Empty();strTemp5.Empty();strTemp6.Empty(); 

			for(int ii = 0; ii<MAX_NOZZLE; ii++)
			{
				if(pDoc->m_bIsHeadSelected[ii])
				{
					if(!Drop_Info.bAdjustComplete[ii])
					{
						bAdjustCompleteDone = FALSE;
						strTemp1.Format("H%d ",ii+1);
						strTemp2 += strTemp1;
					}
					if(!Drop_Info.bMeasureComplete[ii])
					{
						bMeasureCompleteDone = FALSE;
						strTemp3.Format("H%d ",ii+1);
						strTemp4 += strTemp1;
					}
					if(!g_bCalibrationStatus[ii])
					{
						bCalibrationCompleteDone = FALSE;
						strTemp5.Format("H%d ",ii+1);
						strTemp6 += strTemp5;
					}
				}
			}
			
			//작업 step이 선택되지 않은 상태에서 start가 눌리면 메모리 참조 에러가 뜬다...//
			if(ThreadStage.JobStep == 200) 
			{
				CNormalMsg dlg;
				// by ckh 2009.01.17
				// 시작 버튼 계속 누름으로 인해 알람창이 계속 호출됨으로 추가함.
				if(m_bStepNotSelError == FALSE)
				{
					dlg.m_bTimer=FALSE;

					if(pView->m_nLanguage == 0)
					{
						dlg.m_strTitle = _T("확인");
						dlg.m_strMsg1 = " '작업 스텝'이 선택되지 않았습니다. ";
						dlg.m_strMsg2 = " 작업 스텝을 선택 후 START 버튼을 누르세요...";
					}
					else if(pView->m_nLanguage == 1)
					{
						dlg.m_strTitle = _T("Check");
						dlg.m_strMsg1 = " ' Work Step is not Selected.. ";
						dlg.m_strMsg2 = " After Select Work Step, Push Start Button...."; 
					}
					else if(pView->m_nLanguage == 2)
					{
						dlg.m_strTitle = _T("確認");
						dlg.m_strMsg1 = " '作業步驟'未選擇. ";
						dlg.m_strMsg2 = " 選擇作業步驟后按START按紐...";
					}



					m_bStepNotSelError = TRUE;
					//pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispBuzzer,"0008"); //Warning소리
					dlg.DoModal();
					m_bStepNotSelError = FALSE;
				}
			}
			else if ( g_bPdDownReq ) //141026 PD DOWN : PD DOWN 일때 , AUTO RUN START 못하도록 한다. 
			{
 				CNormalMsg dlg;	

			 	if(m_bStepNotSelError == FALSE)
 				{	
					dlg.m_bTimer=FALSE;
 
 					if(pView->m_nLanguage == 0)
 					{
 						dlg.m_strTitle = _T("확인");
 						dlg.m_strMsg1 = " Process Down 상태 입니다. 장비를 START 할수 없습니다. ";
 						dlg.m_strMsg2 = " ECS 상태 확인 하세요.  ";
 					}
 					else if(pView->m_nLanguage == 1)
 					{
 						dlg.m_strTitle = _T("Check");
 						dlg.m_strMsg1 = " The Process Down state. Could not start the equipment.";
 						dlg.m_strMsg2 = " Check ECS Status ";
 					}
 					else if(pView->m_nLanguage == 2)
 					{
 						dlg.m_strTitle = _T("Check");
 						dlg.m_strMsg1 = " The Process Down state. Could not start the equipment.";
 						dlg.m_strMsg2 = " Check ECS Status ";
					}
 					m_bStepNotSelError = TRUE;

 					dlg.DoModal();

 					m_bStepNotSelError = FALSE;
				}
			}
#if _INTERLOCK
 			else if(m_bAdjustErrRange == TRUE)
 			{
 				CNormalMsg dlg;
 				if(m_bStepNotSelError == FALSE)
 				{
 					dlg.m_bTimer=FALSE;
 
 					if(pView->m_nLanguage == 0)
 					{
 						dlg.m_strTitle = _T("확인");
 						dlg.m_strMsg1 = " '조정 오차가 측정 오차보다 크게 설정되었습니다. ";
 						dlg.m_strMsg2 = " 조정 오차 재 설정 후 시작하세요...";
 					}
 					else if(pView->m_nLanguage == 1)
 					{
 						dlg.m_strTitle = _T("Check");
 						dlg.m_strMsg1 = " 'Adjust Error is Set more than Calibration Error. ";
 						dlg.m_strMsg2 = " Please restart. After it reset Adjust Error...";
 					}
 					else if(pView->m_nLanguage == 2)
 					{
 						dlg.m_strTitle = _T("確認");
 						dlg.m_strMsg1 = " '設置的調整誤差大於測量誤差. ";
 						dlg.m_strMsg2 = " 調整誤差重新設置再開始...";
 					}
 					m_bStepNotSelError = TRUE;
 					//pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispBuzzer,"0008"); //Warning소리
 					dlg.DoModal();
 					m_bStepNotSelError = FALSE;
 				}
 			}
 			else if(g_bPumpOringFlag == TRUE)
 			{
 				CNormalMsg dlg;
 				if(m_bStepNotSelError == FALSE)
 				{
 					dlg.m_bTimer=FALSE;
 
 					if(pView->m_nLanguage == 0)
 					{
 						dlg.m_strTitle = _T("확인");
 						dlg.m_strMsg1 = " 'P축 원점 동작 후 측정값을 확인 하지 않았습니다. ";
 						dlg.m_strMsg2 = " 측정 동작 실행 후 측정값 이상 없을 경우 다시 시작하세요...";		
 					}
 
 					else if(pView->m_nLanguage == 1)
 					{
 						dlg.m_strTitle = _T("Check");
 						dlg.m_strMsg1 = " 'Machine Don't Check Calibration Value After P Axis Origin Point Action. ";
 						dlg.m_strMsg2 = " if Machine state is good, Please restart After Calibration Action Run ...";		
 					} 
 					
 					else if(pView->m_nLanguage == 2)
 					{
 						dlg.m_strTitle = _T("確認");
 						dlg.m_strMsg1 = " 'P軸原點動作后沒有確認測量値. ";
 						dlg.m_strMsg2 = " 實行測量動作后測量?無異常下再開始...";		
 					}
 
 				
 					m_bStepNotSelError = TRUE;
 					//pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispBuzzer,"0008"); //Warning소리
 					dlg.DoModal();
 					m_bStepNotSelError = FALSE;
 				}
 			}

 			else if((pDoc->m_structDataEditor.m_nCalibrationMode != 2)
 				&&(!bAdjustCompleteDone)&&(ThreadStage.JobModeSelection != 1))
 			{
 				CNormalMsg dlg;
 				dlg.m_bTimer=FALSE;
 				dlg.m_strTitle = _T("확인");
 				dlg.m_strMsg1 = " '조정동작을 확인 하지 않았습니다.\n ";
 				dlg.m_strMsg1 += strTemp2;
 				dlg.m_strMsg2 = " 조정 동작 실행 후 다시 시작하세요...";		
 				bAdjustCompleteDone = TRUE;
 				dlg.DoModal();
 				bAdjustCompleteDone = FALSE;
 			}
 			else if(!bMeasureCompleteDone && (ThreadStage.JobModeSelection != 1))
 			{
 				CNormalMsg dlg;
 				dlg.m_bTimer=FALSE;
 				dlg.m_strTitle = _T("확인");
 				dlg.m_strMsg1 = " '측정동작을 확인 하지 않았습니다.\n ";
 				dlg.m_strMsg1 += strTemp4;
 				dlg.m_strMsg2 = " 측정 동작 실행 후 측정값 이상 없을 경우 다시 시작하세요...";		
 				bMeasureCompleteDone = TRUE;
 				dlg.DoModal();
 				bMeasureCompleteDone = FALSE;
 			}
 			else if(!bCalibrationCompleteDone && (ThreadStage.JobModeSelection != 1))
 			{
 				CNormalMsg dlg;
 				dlg.m_bTimer=FALSE;
 				dlg.m_strTitle = _T("확인");
 				dlg.m_strMsg1 = " 교정값이 잘못 되었습니다.\n ";
 				dlg.m_strMsg2 = " 교정 동작 실행 후 측정값 이상 없을 경우 다시 시작하세요...";		
 				dlg.DoModal(); 
 			}
/////////////////////////////////////////////////////////////////////////////////// ehji 150529
			else if (g_MeasureCountError)
			{		//g_bStatusError[HeadSelBK] = FALSE; 
				CNormalMsg dlg;

 				dlg.m_bTimer=FALSE;

 				if(pView->m_nLanguage == 0)
 				{
					str.Format("%d Head 교정값이 측정횟수에 전부 들어오지 않았습니다.",g_MeasureheadError+1);

 					dlg.m_strTitle = _T("확인");
 					dlg.m_strMsg1 = str;
 					dlg.m_strMsg2 = " 측정 동작 실행 후 측정값 이상 없을 경우 다시 시작하세요...";		
 				}

 				else if(pView->m_nLanguage == 1)
 				{
					str.Format("%d Head Measure Value is Error.",g_MeasureheadError+1);

 					dlg.m_strTitle = _T("Check");
 					dlg.m_strMsg1 = str;
 					dlg.m_strMsg2 = " if Machine state is good, Please restart After Calibration Action Run ...";		
 				} 
 				
 				else if(pView->m_nLanguage == 2)
 				{
					str.Format("%d Head Head Measure Value is Error.",g_MeasureheadError+1); 

 					dlg.m_strTitle = _T("確認");
 					dlg.m_strMsg1 = str;
 					dlg.m_strMsg2 = " 實行測量動作后測量?無異常下再開始...";		
 				}
 				
 				m_bStepNotSelError = TRUE;
 				//pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispBuzzer,"0008"); //Warning소리
 				dlg.DoModal();
 				m_bStepNotSelError = FALSE;

			//	bMeasureCompleteDone = FALSE;

			//	g_MeasureCountError = FALSE;
			}
////////////////////////////////////////////////////////////////////////////////////
#endif
			else
			{
				pView->SaveLog(0, "CAuto: START switch 눌림");
				pView->SaveLog(1, "CAuto: START switch 눌림");

				m_nOldTempSpanTime = 0;
				m_bTimeCheck = FALSE;

			

				// FTP File의 개수를 세어서 FILE_SAVE_COUNT개 이상이면 날짜를 비교해서 FILE_SAVE_COUNT개까지만 남겨 둔다.
	//      	pView->FtpFileSort(FILE_SAVE_COUNT);

	//2007.08.08 BY tskim
	//Target량과 Measuring량이 다를 경우 "시작이 되질 않게 한다..."
	/*			for(int i=0; i<MAX_NOZZLE;i++)
				{
					if(_bSpecOver[i])
					{
	//					CErrMsg dlg;
	//					dlg.m_nErrCode = 188;
	//					dlg.DoModal();
	//					return;
						ThreadStage.ExitFlag = TRUE;
						pView->SendMessage(WM_ERROR,188,NULL);
						ThreadStage.JobStep=200;
						return;
	//2007.08.21 BY tskim
	//시작 하면 에러가 뜨긴 하는데 에러 클리어가 안된다...
						
					}*/
	//			}

				////////////// 장비 시작전 Servo Alarm을 Check /////////
				int ibdID = 0, iaxis = 0, ialarmBD = 0, isvAlarm = 0, head = 0;
				LONG AxisStatus=0;
				BOOL bIOResult = false;
				DWORD wIOResult = 0, wIOResult1 = 0, wTempIO=0 ;
				
				for( ibdID = 1 ; ibdID <=4 ; ibdID++)
				{
					for( iaxis = 0 ; iaxis <16 ; iaxis++)
					{	// 사용하지 않는 축은 Skip 한다.

						if(ibdID==4)
						{
							if(iaxis > 3)
							{
								continue;
							}
						}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*						if(ibdID==2 || ibdID==4)//jykim 200509 HEAD SKIP (16번 헤드 P,G축 서보알람 체크 해제) P축: 2-3//G축: 4-3
						{	
							if(iaxis==3) 
							{
								continue;	
							}
						}
						
						if((ibdID==1 && iaxis==2) || (ibdID==3 && iaxis==5))//jykim 200509 HEAD SKIP (1번 헤드 P,G축 서보알람 체크 해제)P축: 1-2//G축: 3-5
						{	
							continue;	
						}*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

						FAS_GetAxisStatus(ibdID, iaxis, &AxisStatus);  	
						if((AxisStatus & SERVOALARM)!=0){ 
							ialarmBD = ibdID;  
							ibdID=9; 						 
							break;
						}
					}
				}
//				if(pDoc->m_structDataEditor.m_nLcSupplyType == 1) //중앙 공급을 사용하는 경우...//				
//				{
//					if(g_bLcSupplyUsed == TRUE)	
//					{
//						ThreadStage.strMsg1 = "Canister 무게 측정 중....잠시만 기다려 주세요"; //090702 통신중 메시지 추가
//
//						//by shin//초기 1번 획득한 data를 사용한다...//2009.09.02//
//						ThreadSupplyBal1.nBalanceStableCnt = 0;
//						ThreadSupplyBal1.bBalanceStableFirstFlag = FALSE;
//						ThreadSupplyBal2.nBalanceStableCnt = 0;
//						ThreadSupplyBal2.bBalanceStableFirstFlag = FALSE;
//
//						if(pDoc->m_nCanisterBalNo[0] != 0)
//						{
//							pView->SendData(pDoc->m_nCanisterBalNo[0],"Q"); 
//							Sleep(30);		
//						}
//						else ThreadSupplyBal1.bBalanceStableFirstFlag = TRUE;
//
//						if(pDoc->m_nCanisterBalNo[1] != 0)
//						{
//							pView->SendData(pDoc->m_nCanisterBalNo[1],"Q"); 
//							Sleep(30);		
//						}
//						else ThreadSupplyBal2.bBalanceStableFirstFlag = TRUE;
//
//						if((pDoc->m_nCanisterBalNo[0] != 0) || (pDoc->m_nCanisterBalNo[1] != 0))
//						{	
//							for(int j = 0; j < 50; j++)
//							{
//								if((ThreadSupplyBal1.bBalanceStableFirstFlag == TRUE)&&(ThreadSupplyBal2.bBalanceStableFirstFlag == TRUE))
//								{
//									ThreadStage.strMsg1 = "Canister 현재량 측정 정상 완료";  
//									Sleep(100);
//									SendMessage(WM_INFODISP,7,0);  //Canister 잔량 표기
//
//									break;
//								}
//								else
//								{
//									Sleep(100);
//								}
//							}
//							if(j > 49)
//							{
////								ThreadStage.ExitFlag = TRUE;
//
//								ThreadStage.strMsg1 = "Canister Balance 통신 Error"; 
////								AfxMessageBox("Canister Balance 통신 Error");
//							}
////							else
////							{
////								ThreadStage.strMsg1 = "Canister 현재량 측정 정상 완료";  
////								Sleep(100);
////								SendMessage(WM_INFODISP,7,0);  //Canister 잔량 표기
////							}
//						}				
//					}
//				}
			
				if(ibdID == 10){
					ThreadStage.ExitFlag = TRUE;
					isvAlarm = ialarmBD*0x10 + iaxis;
					pView->SendMessage(WM_ERROR,115,isvAlarm);
				
				}
				else 
				{
					ThreadStage.ExitFlag = FALSE;
					if(ThreadStage.ExitFlag == FALSE) //Canister 알람이 없을 경우...//
					{
						// by ckh 2009.03.09 작업 시작시 장비 상태 재 확인 함. //MC 설정후 자동운전 시작하면 모두 해제..
						
						// by ckh 2009.02.10
						//MC 설정후 자동운전 시작하면 모두 해제..
						pView->m_pDevice->MNET_BitSet(DISP_UNIT_SPC+STATUS_CHANGE_MODEL, FALSE); //모델교환
						pView->m_pDevice->MNET_BitSet(DISP_UNIT_SPC+STATUS_CLEARING, FALSE); //클리닝
						pView->m_pDevice->MNET_BitSet(DISP_UNIT_SPC+STATUS_CHANGE_MATERIAL, FALSE); //재료교체
						pView->m_nMachineStatus =-1;

						ThreadStage.JobFlag = STAGE_AUTO;
						// Auto로 시작할 때 모든 Interface는 초기화하고 시작한다.
		//				pView->m_pDevice->MNET_WriteWord(BaseAddr_DISP+0x06, "0001",1);
						ThreadStage.ExitFlag = false;
						pView->RunThread(THREAD_STAGE);	
						if(ThreadStage.JobModeSelection != 1)	
						{
							m_ctrlAutoLoad.EnableWindow(TRUE);
							m_ctrlAutoUnLoad.EnableWindow(TRUE);
							m_ctrlAutoDrop.EnableWindow(TRUE);
						}
						m_pTimeStepStart=m_pTimeCurrent = CTime::GetCurrentTime();
					}
				}
			}
		}

		IoinizerStatus=pView->m_pDevice->Ionizer_Status_Check();
		// control
		if(ThreadStage.bEmergencyStop==FALSE) pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispLamp,"0040");
		m_ctrlReturn.SetEnabled(true);
		m_ctrlAutoScrap.SetEnabled(true); 
		m_ctrlAdjustInAuto.EnableWindow(true);
		m_ctrlOffLineMode.EnableWindow(true);
	}
}

void CAuto::SubAutoMeasuredDataDisplay()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	
	UINT nsel1=0, nsel2=0, nsel3=0, nsel4=0;
	CString str1,str2,str3,str4;

	if(pView->m_bMesuredDataSaveCompleted == TRUE)		// Data 저장이 완료 되었을 경우, 이젠 이 값을 Display 하자.
	{
		if(pView->m_bMesuredDataDisplayEnable == TRUE)	
		{
			CString str=""; double error_percentage=0.0;
			for(int i =0 ; i <6 ; i ++) // 6개 Data 표시
			{
				for (int j =0 ; j <MAX_NOZZLE; j++) // 12개 Head 별로..
				{	
					if(g_bMultiTargetMeas)
						error_percentage = (pDoc->m_dMeasuredData[j][i] - pDoc->m_dMeasuredData_Target[j][i])*100/pDoc->m_dMeasuredData_Target[j][i];
					else
						error_percentage = (pDoc->m_dMeasuredData[j][i] - pDoc->m_structAdjustCondition.dTargetWeight[j])*100/pDoc->m_structAdjustCondition.dTargetWeight[j];
	
					//error_percentage = (pDoc->m_dMeasuredData[j][i] - poc->m_structAdjustCondition.dTargetWeight[j])*100/pDoc->m_structAdjustCondition.dTargetWeight[j];
					str.Format("%.4f",pDoc->m_dMeasuredData[j][i]);
					m_ctrlAutoMeaData.SetTextMatrix(i+2, (MAX_NOZZLE+1)-(j+1) ,(LPCSTR)str);

					if(error_percentage > 0)
					{
						error_percentage = error_percentage - 0.00000001;
					}
					else
					{
						error_percentage = error_percentage + 0.00000001;
					}

					//
					m_ctrlAutoMeaData.SetRow(i+2); m_ctrlAutoMeaData.SetCol((MAX_NOZZLE+1)-(j+1));
					if(pDoc->m_bIsHeadSelected[j]==TRUE)
					{
						//Auto에서는 조정 에러와 상관없이 타겟에러로 디스플레이 결정한다.//
						if(fabs(error_percentage) <= pDoc->m_structAdjustCondition.dTargetError[j])
							m_ctrlAutoMeaData.SetCellBackColor(GREEN);
						else
							m_ctrlAutoMeaData.SetCellBackColor(WHITE);
					}
					else	m_ctrlAutoMeaData.SetCellBackColor(WHITEGRAY);
				}
			}
			/////////////////////////////////////////////////////////////////////////////////////////////////////
			//lbg 080415
//			nsel1 = ThBal[BALID1].nHeadSelected;
//			nsel2 = ThBal[BALID2].nHeadSelected;
//			nsel3 = ThBal[BALID3].nHeadSelected;
//			nsel4 = ThBal[BALID4].nHeadSelected;
//
//			str1.Format("%d/%.3f",ThBal[BALID1].nLoopCount,pDoc->m_structAdjustCondition.dTargetWeight[nsel1]);
//			str2.Format("%d/%.3f",ThBal[BALID2].nLoopCount,pDoc->m_structAdjustCondition.dTargetWeight[nsel2]);
//			str3.Format("%d/%.3f",ThBal[BALID3].nLoopCount,pDoc->m_structAdjustCondition.dTargetWeight[nsel3]);
//			str4.Format("%d/%.3f",ThBal[BALID4].nLoopCount,pDoc->m_structAdjustCondition.dTargetWeight[nsel4]);
//
//			m_ctrlAutoMeaData.SetTextMatrix(1, nsel1+1 ,str1);
//			m_ctrlAutoMeaData.SetTextMatrix(1, nsel2+1 ,str2);
//			m_ctrlAutoMeaData.SetTextMatrix(1, nsel3+1 ,str3);
//			m_ctrlAutoMeaData.SetTextMatrix(1, nsel4+1 ,str4);
			/////////////////////////////////////////////////////////////////////////////////////////////////////
		}
		UpdateData(FALSE);
		pView->m_bMesuredDataDisplayEnable = FALSE;
	}
}

void CAuto::SubAutoMsgDispFunc()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CString str = "";
	CString strGlassCount="";
	CString strCanisterWeight="";
	CString strAbnormalMsg = "";
	int i;

	//
	if(m_nJobStep != m_nJobStepBK)
	{
		m_nJobStepBK=m_nJobStep;
		if(m_nJobStep >=0 && m_nJobStep <19) 
		{
		//	m_ctrlAutoDummyDrop.SetBackColor(WHITEGRAY);
			m_ctrlAutoDummyDrop.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoLoad.SetBackColor(GREEN);
			m_ctrlAutoDrop.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoUnLoad.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoMeasure.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoVision.SetBackColor(LIGHTPUPPLE);
//2011.04.27 by tskim
			for(i=0;i<MAX_BAL;i++)
			{
				ThBal[i].bAbnormalSpecOut = FALSE;
				ThBal[i].nBalanceMode = 0;
			}
		}
		else if(((m_nJobStep >= 19 && m_nJobStep <40)|| m_nJobStep == 160 || m_nJobStep == 170 || m_nJobStep == 180 || (m_nJobStep >= 150 && m_nJobStep <= 158 ) ) &&  m_nJobStep != 200)
		{
			m_ctrlAutoDummyDrop.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoLoad.SetBackColor(LIGHTPUPPLE);
			if(ThreadStage.JobModeSelection != 1)
			{	// 반송모드가 아닌 경우에만...
					if(ThreadStage.nDropStatus==0)	
					{
						if(pDoc->m_structDataEditor.m_bMarkAlignInsp && pDoc->m_structDataEditor.m_bUse_Vision)
						{
							m_ctrlAutoDrop.SetCaption("Mark Align  &  토출 요구");						
						}
						else if(pDoc->m_structDataEditor.m_bMarkAlignInsp)
						{
							m_ctrlAutoDrop.SetCaption("Mark Align  &  토출 요구");
						}
						else if(pDoc->m_structDataEditor.m_bUse_Vision)
						{
							m_ctrlAutoDrop.SetCaption("토출 요구");						
						}
					else
					{
						m_ctrlAutoDrop.SetCaption("토출 요구");
					}
				}
				else if(ThreadStage.nDropStatus==1)	
				{
					m_ctrlAutoDrop.SetCaption("토출 중");
				}
				else if(ThreadStage.nDropStatus==2)	
				{
					m_ctrlAutoDrop.SetCaption("토출 완료");
				}
			}
			m_ctrlAutoDrop.SetBackColor(GREEN);
			m_ctrlAutoUnLoad.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoMeasure.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoVision.SetBackColor(LIGHTPUPPLE);
//2011.04.27 by tskim
			for(i=0;i<MAX_BAL;i++)
			{
				ThBal[i].bAbnormalSpecOut = FALSE;
				ThBal[i].nBalanceMode = 0;
			}
		}
		else if ( m_nJobStep >= 1000 && m_nJobStep <= 1021 )
		{
			m_ctrlAutoDummyDrop.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoLoad.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoDrop.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoUnLoad.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoMeasure.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoVision.SetBackColor(GREEN);
		}
		else if(m_nJobStep >= 40 && m_nJobStep <80)
		{
			m_ctrlAutoDummyDrop.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoLoad.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoDrop.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoUnLoad.SetBackColor(GREEN);
			m_ctrlAutoMeasure.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoVision.SetBackColor(LIGHTPUPPLE);
//2011.04.27 by tskim
			for(i=0;i<MAX_BAL;i++)
			{
				ThBal[i].bAbnormalSpecOut = FALSE;
				ThBal[i].nBalanceMode = 0;
			}
		}
		else if(m_nJobStep >=80 && m_nJobStep <99) // DummyDrop
		{
			m_ctrlAutoDummyDrop.SetBackColor(GREEN);
			m_ctrlAutoLoad.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoDrop.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoUnLoad.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoMeasure.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoVision.SetBackColor(LIGHTPUPPLE);
//2011.04.27 by tskim
			for(i=0;i<MAX_BAL;i++)
			{
				ThBal[i].bAbnormalSpecOut = FALSE;
				ThBal[i].nBalanceMode = 0;
			}
		}
		//else if(m_nJobStep >= 100 && m_nJobStep <200) 
		else if(m_nJobStep >= 100 && m_nJobStep <150) 
		{
			m_ctrlAutoDummyDrop.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoLoad.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoDrop.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoUnLoad.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoMeasure.SetBackColor(GREEN);
			m_ctrlAutoVision.SetBackColor(LIGHTPUPPLE);
		}
		else
		{
			m_ctrlAutoDummyDrop.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoLoad.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoDrop.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoUnLoad.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoMeasure.SetBackColor(LIGHTPUPPLE);
			m_ctrlAutoVision.SetBackColor(LIGHTPUPPLE);
			for(i=0;i<MAX_BAL;i++)
			{
				ThBal[i].bAbnormalSpecOut = FALSE;
				ThBal[i].nBalanceMode = 0;
			}
		}
	}
	//
	m_ctrlMessage.SetCaption(ThreadStage.strMsg1);
	strGlassCount.Format("%d",g_nTotalGlassCount);
	m_ctrlGlassProcess.SetTextMatrix(0, 1, strGlassCount);
	str.Format("%d/%d",g_nGlassCountScanCycle, pDoc->m_structN_Vision.m_nScan_Cycle); //jykim 160530 scan_cycle
	m_ctrlGlassProcess.SetTextMatrix(2, 1, str); //jykim 160530 scan_cycle


	if(g_nSupplyHead < MAX_NOZZLE/2)
		m_ctrlMessage1.SetCaption(ThreadSupplyBal1.strMsg1);
	else 
		m_ctrlMessage1.SetCaption(ThreadSupplyBal2.strMsg1);

//2011.04.27 by tskim
	if(m_nJobStep >= 100 && m_nJobStep <150)
	{
		if(ThBal[BALID1].nBalanceMode == 1)
		{
			if(ThBal[BALID1].bAbnormalSpecOut || ThBal[BALID2].bAbnormalSpecOut || ThBal[BALID3].bAbnormalSpecOut ||ThBal[BALID4].bAbnormalSpecOut ||
				ThBal[BALID5].bAbnormalSpecOut ||ThBal[BALID6].bAbnormalSpecOut ||ThBal[BALID7].bAbnormalSpecOut ||ThBal[BALID8].bAbnormalSpecOut)
			{
				strAbnormalMsg ="<지진>측정 SpecOver Retry중.";
				m_ctrlMessage1.SetCaption(strAbnormalMsg);
				if(m_ctrlMessage1.GetBackColor() == DARKBLUE)
					m_ctrlMessage1.SetBackColor(RED);
				else if(m_ctrlMessage1.GetBackColor() == RED)
					m_ctrlMessage1.SetBackColor(DARKBLUE);
			}
		}
	}

	
	

	if(ThreadStage.JobModeSelection != 1)	// 반송 모드가 아닌 경우
	{
//2014.11.27 by tskim
		strGlassCount.Format("%d / %d,%d,%d",g_nGlassCount,pDoc->m_structAdjustCondition.m_n1stMeas,pDoc->m_structAdjustCondition.m_n2ndMeas,m_nMeaInterval); 
		m_ctrlGlassProcess.SetTextMatrix(1, 1, strGlassCount);
//		strGlassCount.Format("%d/%d",g_nGlassCount,m_nMeaInterval); 
//		m_ctrlGlassProcess.SetTextMatrix(1, 1, strGlassCount);

		if(ThreadStage.bMachineRunning == FALSE)	// 정지해 있는 경우에 대해서만 버튼 정보 표시.
		{
#if EQ
			if(pView->m_pDevice->Stage_glass_check())	// Glass 가 감지된 경우
			{
				m_ctrlAutoLoad.EnableWindow(FALSE);
				if(ThreadStage.nDropStatus==0)	// 토출 기억이 없는 경우
				{
					m_ctrlAutoDrop.EnableWindow(TRUE);
//					m_ctrlAutoUnLoad.EnableWindow(FALSE); //101119
				}
				else	// 토출 기억이 있는 경우
				{
					m_ctrlAutoDrop.EnableWindow(FALSE); 
					m_ctrlAutoUnLoad.EnableWindow(TRUE); 
				}
			}
			else // Glass가 감지되지 않은 경우
			{			
				m_ctrlAutoLoad.EnableWindow(TRUE);
//				m_ctrlAutoUnLoad.EnableWindow(FALSE);  //111019
				m_ctrlAutoDrop.EnableWindow(FALSE);
			}
#else
#endif
		}
	}
	else	// 반송모드
	{
		m_ctrlGlassProcess.SetTextMatrix(1, 1, "-------------");	
	}
}

void CAuto::OnSubMeaHeadDispReturn(WPARAM wParam, LPARAM lParam)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();	
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	int i=0;

	switch(lParam)
	{
	case BAL1:
		for(i=0;i<2;i++)
		{
			m_ctrlAutoMeaData.SetRow(0);	m_ctrlAutoMeaData.SetCol((MAX_NOZZLE+1)-(i+1));
			if(pDoc->m_bIsHeadSelected[i]==TRUE)	
				m_ctrlAutoMeaData.SetCellBackColor(GREEN);
			else
				m_ctrlAutoMeaData.SetCellBackColor(WHITEGRAY);
		}
		break;

	case BAL2:
		for(i=2;i<4;i++)
		{
			m_ctrlAutoMeaData.SetRow(0);	m_ctrlAutoMeaData.SetCol((MAX_NOZZLE+1)-(i+1));
			if(pDoc->m_bIsHeadSelected[i]==TRUE)	
				m_ctrlAutoMeaData.SetCellBackColor(GREEN);
			else
				m_ctrlAutoMeaData.SetCellBackColor(WHITEGRAY);
		}
		break;

	case BAL3:
		for(i=4;i<6;i++)
		{
			m_ctrlAutoMeaData.SetRow(0);	m_ctrlAutoMeaData.SetCol((MAX_NOZZLE+1)-(i+1));
			if(pDoc->m_bIsHeadSelected[i]==TRUE)	
				m_ctrlAutoMeaData.SetCellBackColor(GREEN);
			else
				m_ctrlAutoMeaData.SetCellBackColor(WHITEGRAY);
		}
		break;

	case BAL4:
		for(i=6;i<8;i++)
		{
			m_ctrlAutoMeaData.SetRow(0);	m_ctrlAutoMeaData.SetCol((MAX_NOZZLE+1)-(i+1));
			if(pDoc->m_bIsHeadSelected[i]==TRUE)	
				m_ctrlAutoMeaData.SetCellBackColor(GREEN);
			else
				m_ctrlAutoMeaData.SetCellBackColor(WHITEGRAY);
		}
		break;

	case BAL5:
		for(i=8;i<10;i++)
		{
			m_ctrlAutoMeaData.SetRow(0);	m_ctrlAutoMeaData.SetCol((MAX_NOZZLE+1)-(i+1));
			if(pDoc->m_bIsHeadSelected[i]==TRUE)	
				m_ctrlAutoMeaData.SetCellBackColor(GREEN);
			else
				m_ctrlAutoMeaData.SetCellBackColor(WHITEGRAY);
		}
		break;

	case BAL6:
		for(i=10;i<12;i++)
		{
			m_ctrlAutoMeaData.SetRow(0);	m_ctrlAutoMeaData.SetCol((MAX_NOZZLE+1)-(i+1));
			if(pDoc->m_bIsHeadSelected[i]==TRUE)	
				m_ctrlAutoMeaData.SetCellBackColor(GREEN);
			else
				m_ctrlAutoMeaData.SetCellBackColor(WHITEGRAY);
		}
		break;

	case BAL7:
		for(i=12;i<14;i++)
		{
			m_ctrlAutoMeaData.SetRow(0);	m_ctrlAutoMeaData.SetCol((MAX_NOZZLE+1)-(i+1));
			if(pDoc->m_bIsHeadSelected[i]==TRUE)	
				m_ctrlAutoMeaData.SetCellBackColor(GREEN);
			else
				m_ctrlAutoMeaData.SetCellBackColor(WHITEGRAY);
		}
		break;

	case BAL8:
		for(i=14;i<16;i++)
		{
			m_ctrlAutoMeaData.SetRow(0);	m_ctrlAutoMeaData.SetCol((MAX_NOZZLE+1)-(i+1));
			if(pDoc->m_bIsHeadSelected[i]==TRUE)	
				m_ctrlAutoMeaData.SetCellBackColor(GREEN);
			else
				m_ctrlAutoMeaData.SetCellBackColor(WHITEGRAY);
		}
		break;
	}
}

void CAuto::SubAutoMeaHeadDisp(int nBalNo)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();	
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	unsigned long CurCellBackColor=0;
	int i=0;
	switch(nBalNo)
	{
	case BAL1:
		if(ThBal[BALID1].bRunning==TRUE)
		{
			for(i=0;i<2;i++)
			{
				m_ctrlAutoMeaData.SetRow(0);	m_ctrlAutoMeaData.SetCol((MAX_NOZZLE+1)-(i+1));
	
				if(i!=ThBal[BALID1].nHeadSelected)
				{
					if(pDoc->m_bIsHeadSelected[i]==TRUE)
						m_ctrlAutoMeaData.SetCellBackColor(GREEN);
					else
						m_ctrlAutoMeaData.SetCellBackColor(WHITEGRAY);
				}
				else	// 측정/조정이 되고 있는 Head 번호...
				{
					if(pDoc->m_bIsHeadSelected[i]==TRUE)
					{
						CurCellBackColor=m_ctrlAutoMeaData.GetCellBackColor();
						if(CurCellBackColor==GREEN)
							m_ctrlAutoMeaData.SetCellBackColor(DARKGREEN);
						else
							m_ctrlAutoMeaData.SetCellBackColor(GREEN);

					}
				}
				
			}
		}
		break;

	case BAL2:
		if(ThBal[BALID2].bRunning==TRUE)
		{
			for(i=2;i<4;i++)
			{
				m_ctrlAutoMeaData.SetRow(0);	m_ctrlAutoMeaData.SetCol((MAX_NOZZLE+1)-(i+1));
				if(i!=ThBal[BALID2].nHeadSelected)
				{
					if(pDoc->m_bIsHeadSelected[i]==TRUE)
						m_ctrlAutoMeaData.SetCellBackColor(GREEN);
					else
						m_ctrlAutoMeaData.SetCellBackColor(WHITEGRAY);
				}
				else	// 측정/조정이 되고 있는 Head 번호...
				{
					if(pDoc->m_bIsHeadSelected[i]==TRUE)
					{
						CurCellBackColor=m_ctrlAutoMeaData.GetCellBackColor();
						if(CurCellBackColor==GREEN)
							m_ctrlAutoMeaData.SetCellBackColor(DARKGREEN);
						else
							m_ctrlAutoMeaData.SetCellBackColor(GREEN);

					}
				}
			}
		}
		break;

	case BAL3:
		if(ThBal[BALID3].bRunning==TRUE)
		{
			for(i=4;i<6;i++)
			{
				m_ctrlAutoMeaData.SetRow(0);	m_ctrlAutoMeaData.SetCol((MAX_NOZZLE+1)-(i+1));
				if(i!=ThBal[BALID3].nHeadSelected)
				{
					if(pDoc->m_bIsHeadSelected[i]==TRUE)
					{
						m_ctrlAutoMeaData.SetCellBackColor(GREEN);
					}
					else
						m_ctrlAutoMeaData.SetCellBackColor(WHITEGRAY);
				}
				else	// 측정/조정이 되고 있는 Head 번호...
				{
					if(pDoc->m_bIsHeadSelected[i]==TRUE)
					{
						CurCellBackColor=m_ctrlAutoMeaData.GetCellBackColor();
						if(CurCellBackColor==GREEN)
							m_ctrlAutoMeaData.SetCellBackColor(DARKGREEN);
						else
							m_ctrlAutoMeaData.SetCellBackColor(GREEN);
					}
				}
			}
		}
		break;

	case BAL4:
		if(ThBal[BALID4].bRunning==TRUE)
		{
			for(i=6;i<8;i++)
			{
				m_ctrlAutoMeaData.SetRow(0);	m_ctrlAutoMeaData.SetCol((MAX_NOZZLE+1)-(i+1));
				if(i!=ThBal[BALID4].nHeadSelected)
				{
					if(pDoc->m_bIsHeadSelected[i]==TRUE)
					{
						m_ctrlAutoMeaData.SetCellBackColor(GREEN);
					}
					else
						m_ctrlAutoMeaData.SetCellBackColor(WHITEGRAY);
				}
				else	// 측정/조정이 되고 있는 Head 번호...
				{
					if(pDoc->m_bIsHeadSelected[i]==TRUE)
					{
						CurCellBackColor=m_ctrlAutoMeaData.GetCellBackColor();
						if(CurCellBackColor==GREEN)
							m_ctrlAutoMeaData.SetCellBackColor(DARKGREEN);
						else
							m_ctrlAutoMeaData.SetCellBackColor(GREEN);
					}
				}
			}
		}
		break;

	case BAL5:
		if(ThBal[BALID5].bRunning==TRUE)
		{
			for(i=8;i<10;i++)
			{
				m_ctrlAutoMeaData.SetRow(0);	m_ctrlAutoMeaData.SetCol((MAX_NOZZLE+1)-(i+1));
				if(i!=ThBal[BALID5].nHeadSelected)
				{
					if(pDoc->m_bIsHeadSelected[i]==TRUE)
					{
						m_ctrlAutoMeaData.SetCellBackColor(GREEN);
					}
					else
						m_ctrlAutoMeaData.SetCellBackColor(WHITEGRAY);
				}
				else	// 측정/조정이 되고 있는 Head 번호...
				{
					if(pDoc->m_bIsHeadSelected[i]==TRUE)
					{
						CurCellBackColor=m_ctrlAutoMeaData.GetCellBackColor();
						if(CurCellBackColor==GREEN)
							m_ctrlAutoMeaData.SetCellBackColor(DARKGREEN);
						else
							m_ctrlAutoMeaData.SetCellBackColor(GREEN);
					}
				}
			}
		}
		break;

	case BAL6:
		if(ThBal[BALID6].bRunning==TRUE)
		{
			for(i=10;i<12;i++)
			{
				m_ctrlAutoMeaData.SetRow(0);	m_ctrlAutoMeaData.SetCol((MAX_NOZZLE+1)-(i+1));
				if(i!=ThBal[BALID6].nHeadSelected)
				{
					if(pDoc->m_bIsHeadSelected[i]==TRUE)
					{
						m_ctrlAutoMeaData.SetCellBackColor(GREEN);
					}
					else
						m_ctrlAutoMeaData.SetCellBackColor(WHITEGRAY);
				}
				else	// 측정/조정이 되고 있는 Head 번호...
				{
					if(pDoc->m_bIsHeadSelected[i]==TRUE)
					{
						CurCellBackColor=m_ctrlAutoMeaData.GetCellBackColor();
						if(CurCellBackColor==GREEN)
							m_ctrlAutoMeaData.SetCellBackColor(DARKGREEN);
						else
							m_ctrlAutoMeaData.SetCellBackColor(GREEN);
					}
				}
			}
		}
		break;
	case BAL7:
		if(ThBal[BALID7].bRunning==TRUE)
		{
			for(i=12;i<14;i++)
			{
				m_ctrlAutoMeaData.SetRow(0);	m_ctrlAutoMeaData.SetCol((MAX_NOZZLE+1)-(i+1));
				if(i!=ThBal[BALID7].nHeadSelected)
				{
					if(pDoc->m_bIsHeadSelected[i]==TRUE)
					{
						m_ctrlAutoMeaData.SetCellBackColor(GREEN);
					}
					else
						m_ctrlAutoMeaData.SetCellBackColor(WHITEGRAY);
				}
				else	// 측정/조정이 되고 있는 Head 번호...
				{
					if(pDoc->m_bIsHeadSelected[i]==TRUE)
					{
						CurCellBackColor=m_ctrlAutoMeaData.GetCellBackColor();
						if(CurCellBackColor==GREEN)
							m_ctrlAutoMeaData.SetCellBackColor(DARKGREEN);
						else
							m_ctrlAutoMeaData.SetCellBackColor(GREEN);
					}
				}
			}
		}
		break;
	case BAL8:
		if(ThBal[BALID8].bRunning==TRUE)
		{
			for(i=14;i<16;i++)
			{
				m_ctrlAutoMeaData.SetRow(0);	m_ctrlAutoMeaData.SetCol((MAX_NOZZLE+1)-(i+1));
				if(i!=ThBal[BALID8].nHeadSelected)
				{
					if(pDoc->m_bIsHeadSelected[i]==TRUE)
					{
						m_ctrlAutoMeaData.SetCellBackColor(GREEN);
					}
					else
						m_ctrlAutoMeaData.SetCellBackColor(WHITEGRAY);
				}
				else	// 측정/조정이 되고 있는 Head 번호...
				{
					if(pDoc->m_bIsHeadSelected[i]==TRUE)
					{
						CurCellBackColor=m_ctrlAutoMeaData.GetCellBackColor();
						if(CurCellBackColor==GREEN)
							m_ctrlAutoMeaData.SetCellBackColor(DARKGREEN);
						else
							m_ctrlAutoMeaData.SetCellBackColor(GREEN);
					}
				}
			}
		}
		break;
	}
}

void CAuto::SubTimeDisplay()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CString val="";
	unsigned long n_tempCurTime=0, n_tempStepStartTime=0, n_tempSpanTime=0; 
//	unsigned long m_nOldTempSpanTime = 0;
//	BOOL m_bTimeCheck = FALSE;

	if(ThreadStage.bMachineRunning == TRUE){
		if(m_nJobStep >=0 && m_nJobStep<5)
		{
			m_pTimeCurrent=m_pTimeStepStart;
			for(int i=0;i<7;i++) m_nAutoTime[i] = 0;
		}
		else
		{		
			m_pTimeCurrent = CTime::GetCurrentTime();
		}

		////////////////////////////////////
		if(m_pTimeCurrent!=m_pTimeStepStart)
		{
			if((m_pTimeCurrent.GetDay()==m_pTimeStepStart.GetDay()) && m_nJobStep !=200)
			{
//				if(bTimeCheck == TRUE)
//				{
//					n_tempCurTime = 0;
//					n_tempStepStartTime = 0;
//					n_tempSpanTime = 0;
//				}
//				else
//				{
					n_tempCurTime=(3600*m_pTimeCurrent.GetHour())+(60*m_pTimeCurrent.GetMinute())+m_pTimeCurrent.GetSecond();
					n_tempStepStartTime=(3600*m_pTimeStepStart.GetHour())+(60*m_pTimeStepStart.GetMinute())+m_pTimeStepStart.GetSecond();
					n_tempSpanTime=n_tempCurTime-n_tempStepStartTime;
//				}
				if(m_nJobStep >=5 && m_nJobStep<19) 
				{
					m_nAutoTime[1]=n_tempSpanTime;
					m_nOldTempSpanTime = 0;
					val.Format("%d",m_nAutoTime[1]);
					m_ctrlProgress01.SetWindowText(val);
				}
				else if((m_nJobStep >=19 && m_nJobStep<40 ) || (m_nJobStep >=150 && m_nJobStep<160) || m_nJobStep == 160 || m_nJobStep == 170 || m_nJobStep == 180
					)
				{
					m_nAutoTime[2]=n_tempSpanTime;
					m_nOldTempSpanTime = 0;
					CIM_Information.nWorkTime = m_nAutoTime[2];
//					if(bTimeCheck == TRUE) m_nAutoTime[2] = 0; 
					val.Format("%d",m_nAutoTime[2]);
					m_ctrlProgress02.SetWindowText(val);
//					bTimeCheck = FALSE;
					ThreadStage.nPassTimeCount=m_nAutoTime[2];
				}
				else if ( m_nJobStep >=1000 && m_nJobStep < 1022 )
				{
					m_nAutoTime[6]=n_tempSpanTime;
					m_nOldTempSpanTime = 0;
					val.Format("%d",m_nAutoTime[6]);
					m_ctrlProgress06.SetWindowText(val);
					ThreadStage.nPassTimeCount=m_nAutoTime[6];					
				}
				else if(m_nJobStep >=40 && m_nJobStep<80) 
				{
					if((bSupplyStart == TRUE)&&(m_bTimeCheck == TRUE))
					{
						m_nOldTempSpanTime = m_nAutoTime[3];
						m_bTimeCheck = FALSE;
					}

//					m_nAutoTime[3]=n_tempSpanTime;
					m_nAutoTime[3]=n_tempSpanTime+m_nOldTempSpanTime;
					val.Format("%d",m_nAutoTime[3]);
					m_ctrlProgress03.SetWindowText(val);
//2007.11.09 By tskim
//반출 대기 시간 1000초 경과 할 경우 알람 발생...
					if(m_nAutoTime[3] > 1000)
					{
						m_bUnloadingTimeOver = TRUE;
					}
					else
					{
						m_bUnloadingTimeOver = FALSE;
					}

					if(pDoc->m_structDataEditor.m_nLcSupplyType == 1)
					{
						//LC Supply
						if(bSupplyStart == TRUE)
						{
							if(m_nAutoTime[5] <= 0) m_bTimeCheck = TRUE;

							m_nAutoTime[5]=n_tempSpanTime;
							val.Format("%d",m_nAutoTime[5]);
							m_ctrlProgress05.SetWindowText(val);
						}
					}

				}
				else if(m_nJobStep >= 100 && m_nJobStep<150) 
				{
					m_nOldTempSpanTime = 0;
					m_nAutoTime[4]=n_tempSpanTime;
					val.Format("%d",m_nAutoTime[4]);
					m_ctrlProgress04.SetWindowText(val);
				}		
			}

			else if ((m_pTimeCurrent.GetDay() != m_pTimeStepStart.GetDay()) && m_nJobStep !=200)		//ehji 150301  날짜라 다를 경우 (24*3600) + (60*60) 값을 더함. 
			{
				//				if(bTimeCheck == TRUE)
//				{
//					n_tempCurTime = 0;
//					n_tempStepStartTime = 0;
//					n_tempSpanTime = 0;
//				}
//				else
//				{
					n_tempCurTime=((3600*m_pTimeCurrent.GetHour())+(60*m_pTimeCurrent.GetMinute())+m_pTimeCurrent.GetSecond()) + 86400 + 3600 ; 
					n_tempStepStartTime=(3600*m_pTimeStepStart.GetHour())+(60*m_pTimeStepStart.GetMinute())+m_pTimeStepStart.GetSecond();
					n_tempSpanTime=n_tempCurTime-n_tempStepStartTime;
//				}
				if(m_nJobStep >=5 && m_nJobStep<19) 
				{
					m_nAutoTime[1]=n_tempSpanTime;
					m_nOldTempSpanTime = 0;
					val.Format("%d",m_nAutoTime[1]);
					m_ctrlProgress01.SetWindowText(val);
				}
				else if((m_nJobStep >=19 && m_nJobStep<40 ) || (m_nJobStep >=150 && m_nJobStep<160) || m_nJobStep == 160 || m_nJobStep == 170 || m_nJobStep == 180
					|| (m_nJobStep >=1000 && m_nJobStep <1022 ))//20//40
				{
					m_nAutoTime[2]=n_tempSpanTime;
					m_nOldTempSpanTime = 0;
					CIM_Information.nWorkTime = m_nAutoTime[2];
//					if(bTimeCheck == TRUE) m_nAutoTime[2] = 0; 
					val.Format("%d",m_nAutoTime[2]);
					m_ctrlProgress02.SetWindowText(val);
//					bTimeCheck = FALSE;
					ThreadStage.nPassTimeCount=m_nAutoTime[2];
				}
				else if(m_nJobStep >=40 && m_nJobStep<80) 
				{
					if((bSupplyStart == TRUE)&&(m_bTimeCheck == TRUE))
					{
						m_nOldTempSpanTime = m_nAutoTime[3];
						m_bTimeCheck = FALSE;
					}

//					m_nAutoTime[3]=n_tempSpanTime;
					m_nAutoTime[3]=n_tempSpanTime+m_nOldTempSpanTime;
					val.Format("%d",m_nAutoTime[3]);
					m_ctrlProgress03.SetWindowText(val);
//2007.11.09 By tskim
//반출 대기 시간 1000초 경과 할 경우 알람 발생...
					if(m_nAutoTime[3] > 1000)
					{
						m_bUnloadingTimeOver = TRUE;
					}
					else
					{
						m_bUnloadingTimeOver = FALSE;
					}

					if(pDoc->m_structDataEditor.m_nLcSupplyType == 1)
					{
						//LC Supply
						if(bSupplyStart == TRUE)
						{
							if(m_nAutoTime[5] <= 0) m_bTimeCheck = TRUE;

							m_nAutoTime[5]=n_tempSpanTime;
							val.Format("%d",m_nAutoTime[5]);
							m_ctrlProgress05.SetWindowText(val);
						}
					}

				}
				else if(m_nJobStep >= 100 && m_nJobStep<150) 
				{
					m_nOldTempSpanTime = 0;
					m_nAutoTime[4]=n_tempSpanTime;
					val.Format("%d",m_nAutoTime[4]);
					m_ctrlProgress04.SetWindowText(val);
				}	
			}
		}
	}
}

void CAuto::SubTimerAutoCycleTimeDisplay()
{	
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	CString val="";
	m_nCycleTimeDisp = CTime::GetCurrentTime();
	m_nCycleTimeCur=m_nCycleTimeDisp.GetSecond();

	if(m_nCycleTimeCur!=m_nCycleTimePrev)
	{
		m_nCycleTimePrev=m_nCycleTimeCur;
		m_ulAutoCycletime++;
		val.Format("%lu",m_ulAutoCycletime);
		m_ctrlProgress00.SetWindowText(val);
	}


	///////////////////////////////////////////////////////////////////
}
void CAuto::OnClickCmdAutointerface() 
{
	CAutoIF dlg;
	dlg.DoModal();	
}


void CAuto::OnClickCmdMeasureddataDraw() 
{
	CAdjustDraw dlg;
	dlg.DoModal();	
}

void CAuto::OnAdjustInAuto() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	m_bBuzzer = TRUE; // by ckh 2009.03.15

	//by shin//2009//auto thread 동작중인지로 구분한다...//
	pDoc->m_bAutoManualFlag = FALSE; 

	if(PC_TYPE==TRUE) // Timer Off In Auto
	{
		KillTimer(TIMER_AUTO); // SubTimerAutoFunc()
		KillTimer(TIMER_AUTO_CLOCK); // SubTimerAutoFunc()

		// Dispenser 가 현재 자동 운전 중이 아님을 알려 준다.
		pView->m_pDevice->MNET_BitSet(DISP_BIT+0x05, FALSE);	

		//Manual Mode 전환..
		pView->m_pDevice->MNET_WriteBit(DISP_OP_MODE,"0002");//Manual Mode

//		pView->m_pDevice->MNET_BitSet(DISP_OP_MODE,FALSE); //AutoMode
//		pView->m_pDevice->MNET_BitSet(DISP_OP_MODE+0x01,TRUE);//Manual Mode

//		pView->m_pDevice->MNET_BitSet(BaseAddr_DISP+BitDispModeStatus+0x05, false);	
	}

	if(pView->m_nMachineStatus == 0)
	{
		pView->m_pMcStatus->KillTimer(0);
		pView->m_pMcStatus->CloseWindow();
	}

	CAdjust dlg;
	dlg.DoModal();
	
	if(PC_TYPE==TRUE) // Timer Of In Auto
	{
		// Loop Count BackUp in Temp Area// memcpy( dest, source , count ) // auto run에서 이용되는 count
		memcpy(m_nLoopCountBK,pDoc->m_structAdjustCondition.nLoopCount ,sizeof(int)*MAX_NOZZLE);
		SetTimer(TIMER_AUTO,200,NULL); // SubTimerAutoFunc()
		SetTimer(TIMER_AUTO_CLOCK,1000,NULL); // SubTimerAutoFunc()
		// Dispenser 가 현재 자동 운전 중임을 알려 준다.
		pView->m_pDevice->MNET_BitSet(DISP_BIT+0x05, true);
		//Auto Mode 전환..
		pView->m_pDevice->MNET_WriteBit(DISP_OP_MODE,"0001");//AutoMode


//		pView->m_pDevice->MNET_BitSet(DISP_OP_MODE,TRUE); //AutoMode
//		pView->m_pDevice->MNET_BitSet(DISP_OP_MODE+0x01,FALSE);//Manual Mode

		pView->m_pDevice->MNET_WriteBit(DISP_SEND, "0000");
		pView->m_pDevice->MNET_WriteBit(DISP_RECEIVE, "0000");

		//TAS DATA 영역 초기화//
		for(int i = 0; i <= TAS_DUMMY_DROP; i++)
		{
			pView->WriteTasData(TAS_EXCHANGE+i, 0, ALL_OFF); 
			Sleep(30);
		}
		//by shin//2009.08.25//MC 관련 TAS 추가...//
//		pView->WriteTasMCData(TAS_MC, 0, ALL_OFF); 


		pView->m_bMesuredDataDisplayEnable = TRUE;	  // Auto 화면상에서 측정 Data 한 번 뿌려 주기.
		pView->m_bMesuredDataSaveCompleted = TRUE;
		SubAutoMeasuredDataDisplay();
		Sub_Amount_Display(FALSE);
	}

	if(pView->m_nMachineStatus == 0)
	{
		pView->m_pMcStatus = new CMcStatus();
		pView->m_pMcStatus->Create(this);
		pView->m_pMcStatus->ShowWindow(SW_SHOW);
	}
}

void CAuto::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if( pView->m_pAuto!= NULL) delete(pView->m_pAuto); pView->m_pAuto= NULL;
	
	CDialog::PostNcDestroy();
}

void CAuto::OnClickCmdGlsCntClear() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	g_nGlassCountScanCycle =0;		//ehji 140918
	g_nGlassCount=0;	
	g_nTotalGlassCount=0;
//2014.11.27 by tskim 1stMeasure Skip
	if(pDoc->m_structAdjustCondition.m_n1stMeas <= 0)
		g_nInitMeasureGlassCount = pDoc->m_structAdjustCondition.m_n2ndMeas;
	else
		g_nInitMeasureGlassCount = pDoc->m_structAdjustCondition.m_n1stMeas;
}

//lbg 080701 Auto에서 측정주기 변경
void CAuto::OnClickMsflexgridGlassProcess() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	
	// 반송 지연 시간 설정
	int nRow=0, nCol=0;
	if(ThreadStage.ExitFlag == FALSE) return;

	nRow = m_ctrlGlassProcess.GetMouseRow();
	nCol = m_ctrlGlassProcess.GetMouseCol();


	if(nRow == 0 || nRow == 1 || nCol == 0)	return;
	if(ThreadStage.JobModeSelection != 1) 
	{
		if((nRow == 2) && (nCol == 1)) return;
	}

	//if(nCol == 0) return;
	//if(nRow == 1) return;
	Use_TKg(m_ctrlGlassProcess, 1000,1, 300, 300, TRUE, false);
//	if(nRow == 1) //작업매수 변경..
//		m_nMeaInterval=pDoc->m_structAdjustCondition.nCheckingIntervalSheet=atoi((char *)(LPCSTR)m_ctrlGlassProcess.GetTextMatrix(nRow, nCol));
	if(nRow == 2)
	{
		if(ThreadStage.JobModeSelection == 1) 
		{		// 반송모드
			ThreadStage.nPassTime = atoi((char *)(LPCSTR)m_ctrlGlassProcess.GetTextMatrix(2, 1));
		}
//		else
//		{
//			m_nForcedMeasInterval = atoi((char *)(LPCSTR)m_ctrlGlassProcess.GetTextMatrix(nRow, nCol));
//		}
	}
	//atoi((char *)(LPCSTR)m_ctrlGlassProcess.GetTextMatrix(nRow, nCol));
	//ThreadStage.nPassTime = atoi((char *)(LPCSTR)m_ctrlGlassProcess.GetTextMatrix(nRow, nCol));
	//if(nRow == 0) g_nTotalGlassCount = atoi((char *)(LPCSTR)m_ctrlGlassProcess.GetTextMatrix(nRow, nCol));
	//else m_nForcedMeasInterval = atoi((char *)(LPCSTR)m_ctrlGlassProcess.GetTextMatrix(nRow, nCol));

}

void CAuto::OnClickOnOffLinemode() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(ThreadStage.OffLineMode == FALSE)
	{
		ThreadStage.OffLineMode = TRUE;

		// by ckh 2009.03.18 // 오프 라인시 장비 다운으로 보고
		pView->SetOffLine(TRUE);
		
		// 모드 설정=>running (Onine status -> offline)	
		pView->m_pDevice->MNET_BitSet(DISP_BIT, false);

		m_ctrlOffLineMode.SetWindowText("Offline Mode 변경");
		m_ctrlOffLineMode.SetBackColor(RED);
		pView->SaveLog(0,"Offline Mode");

	}
	else
	{
		ThreadStage.OffLineMode = FALSE;

		// 모드 설정=>running (Onine status -> online)	
		pView->m_pDevice->MNET_BitSet(DISP_BIT, true);

		m_ctrlOffLineMode.SetWindowText("Online Mode");
		m_ctrlOffLineMode.SetBackColor(GREEN);

		pView->SaveLog(0,"Online Mode 변경");
		
		// by ckh 2009.03.18 // 오프 라인시 장비 다운 해제 보고
		pView->SetOffLine(FALSE);
	}
}

CString CAuto::PanelInformationConvert(int Panel_Info)
{
	CString strReturn="";

	switch(Panel_Info)
	{
	case 0:		strReturn=" ";	break;
	case 1:		strReturn="G";	break;
	case 2:		strReturn="N";	break;
	case 3:		strReturn="P";	break;
	case 4:		strReturn="M";	break;
	case 5:		strReturn="R";	break;
	case 6:		strReturn="H";	break;
	case 7:		strReturn="I";	break;	// 
	case 8:		strReturn="A";	break;
	case 9:		strReturn="Q";	break;
	case 10:	strReturn="E";	break;	//
	case 11:	strReturn="C";	break;	//
	case 12:	strReturn="B";	break;	// 
	case 13:	strReturn="F";	break;	// 
	case 14:	strReturn="D";	break;	//
	case 15:	strReturn="L";	break;	//
	case 16:	strReturn=" ";	break;	//
	case 17:	strReturn="W";	break;	//
	case 18:	strReturn="U";	break;	// 
	case 19:	strReturn="T";	break;	// 
	case 20:	strReturn="V";	break;	//
	case 21:	strReturn="K";	break;	//
	case 22:	strReturn="S";	break;	//
	case 23:	strReturn="X";	break;	
	case 24:	strReturn="Y";	break;
	case 25:	strReturn=" ";	break;
	case 26:	strReturn=" ";	break;	
	case 27:	strReturn=" ";	break;	
	case 28:	strReturn=" ";	break;	
	case 29:	strReturn=" ";	break;	
	case 30:	strReturn=" ";	break;	
	case 31:	strReturn=" ";	break;	
	default:    strReturn=" ";	break;

/*
	case 0:		strReturn=" ";	break;
	case 1:		strReturn="G";	break;
	case 2:		strReturn="N";	break;
	case 3:		strReturn="P";	break;
	case 4:		strReturn="M";	break;
	case 5:		strReturn="R";	break;
	case 6:		strReturn="H";	break;
	case 7:		strReturn="U";	break;	// 
	case 8:		strReturn="A";	break;
	case 9:		strReturn="Q";	break;
	case 10:	strReturn="W";	break;	//
	case 11:	strReturn="S";	break;	//
	case 12:	strReturn="NU";	break;	// Not used
	case 13:	strReturn="NU";	break;	// Not used
	case 14:	strReturn="NU";	break;	// Not used
	case 15:	strReturn="L";	break;	//
*/
	}
	return strReturn;
}

void CAuto::DispLCQntyLabel(int nH, int nValue)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	CString str="";
	if(pDoc->m_bIsHeadSelected[nH]==TRUE)
	{
		str.Format("%d",nValue);
	}
	else
	{
		str.Format("0");
	}
	switch(nH+1)
	{
	case 1:		m_ctrlLCQntyDisp1.SetCaption(str);	break;
	case 2:		m_ctrlLCQntyDisp2.SetCaption(str);	break;
	case 3:		m_ctrlLCQntyDisp3.SetCaption(str);	break;
	case 4:		m_ctrlLCQntyDisp4.SetCaption(str);	break;
	case 5:		m_ctrlLCQntyDisp5.SetCaption(str);	break;
	case 6:		m_ctrlLCQntyDisp6.SetCaption(str);	break;
	case 7:		m_ctrlLCQntyDisp7.SetCaption(str);	break;
	case 8:		m_ctrlLCQntyDisp8.SetCaption(str);	break;
	case 9:		m_ctrlLCQntyDisp9.SetCaption(str);	break;
	case 10:	m_ctrlLCQntyDisp10.SetCaption(str);	break;
	case 11:	m_ctrlLCQntyDisp11.SetCaption(str);	break;
	case 12:	m_ctrlLCQntyDisp12.SetCaption(str);	break;
	case 13:	m_ctrlLCQntyDisp13.SetCaption(str);	break;
	case 14:	m_ctrlLCQntyDisp14.SetCaption(str);	break;
	case 15:	m_ctrlLCQntyDisp15.SetCaption(str);	break;
	case 16:	m_ctrlLCQntyDisp16.SetCaption(str);	break;
	}		
}
void CAuto::CalcPanelCount()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	int i=0, j=0;
	CString strLog="";
	// 판넬 정보의 최대 값을 계산..
	CIM_Information.Panal_Max_Num = 0;
	ThreadStage.nExistMMG=0;
	for(i=0;i<pDoc->m_structDataEditor.m_nPatternNum;i++)
	{
		if((pDoc->m_structPatternData[i].m_bMMG==TRUE)&&(ThreadStage.nExistMMG==0))	ThreadStage.nExistMMG=i+1;
		for(j=0;j<MAX_NOZZLE;j++)
		{
			if(pDoc->m_structPatternData[i].m_nPanelNo[j] > CIM_Information.Panal_Max_Num)
				CIM_Information.Panal_Max_Num = pDoc->m_structPatternData[i].m_nPanelNo[j];
		}
	}

	// Sub Panel 번호의 시작을 체크한다.
	if(ThreadStage.nExistMMG!=0)
	{
		CIM_Information.SubPanalNo_Start=CIM_Information.Panal_Max_Num;
		for(i=0;i<pDoc->m_structDataEditor.m_nPatternNum;i++)
		{
			if(pDoc->m_structPatternData[i].m_bMMG==TRUE)
			{
				for(j=0;j<MAX_NOZZLE;j++)
				{
					if( pDoc->m_structPatternData[i].m_bIsOnHead[j] ) 
					{
						if(pDoc->m_structPatternData[i].m_nPanelNo[j] < CIM_Information.SubPanalNo_Start)
							CIM_Information.SubPanalNo_Start = pDoc->m_structPatternData[i].m_nPanelNo[j];
					}
				}
			}
		}
		// panel정보의 최대값과, add/del값을 log에 기록한다
		strLog.Format("Sub-Panel 번호 : %d 부터 %d까지. ",CIM_Information.SubPanalNo_Start, CIM_Information.Panal_Max_Num);
		pView->SaveLog(0, strLog);
	}
	else
	{
		CIM_Information.SubPanalNo_Start=0;
	}
}

void CAuto::OnClickMsflexgridAutoWorkdata() 
{
	// TODO: Add your control notification handler code here
	// Data Save 중인 경우에는 Return 한다.
	if((ThreadStage.JobStep>=7)&&(ThreadStage.JobStep<=17))		return;
	if((ThreadStage.JobStep>=67)&&(ThreadStage.JobStep<=76))	return;

	int nRow = m_ctrlAutoWorkData.GetMouseRow();
	int nCol = m_ctrlAutoWorkData.GetMouseCol();

	if(nRow!=1 || nCol!=0)	return;
	CGlassID dlg;
	dlg.DoModal();		
}

void CAuto::OnClickAutoErrorHistory() 
{
	// TODO: Add your control notification handler code here
/*	if(ThreadStage.bMachineRunning == TRUE)
	{
		return;
	}*/
	CErrListAuto dlg;
	dlg.DoModal();	
}


void CAuto::OnClickMsflexgridAutoMeadata() 
{
	return ;// by ckh 2008.12.24
	// TODO: Add your control notification handler code here
//	KillTimer(TIMER_AUTO_SPEC_TIME);
	CSpecCount dlg;
	dlg.DoModal();
//	m_strDuedate = OnCalDuedate();
//	SubTimerAutoSpecTime();
/*	if(g_bSpecCount) 
	{
		m_ctrlSpecmove.SetCaption("동 작");
		m_ctrlSpecmove.SetBackColor(LIGHTGREEN);
//		SetTimer(TIMER_AUTO_SPEC_TIME,10000,NULL);
	}
	else
	{
		m_ctrlSpecmove.SetCaption("정 지");
		m_ctrlSpecmove.SetBackColor(LIGHTGRAY);
	}
*/
}

/*CString CAuto::OnCalDuedate()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	CString strTotal = "";
	int temp_Y = 0, temp_M = 0, temp_D = 0, temp_H = 0, temp_B = 0;
	int plus_H = 0,plus_D = 0, plus_M = 0;
	int nMonth = 0;
	if(pDoc->m_nSpecTime > 0)
	{
		temp_Y = atoi(pDoc->m_strSpecTime.Mid(0,2));
		temp_M = atoi(pDoc->m_strSpecTime.Mid(2,2));
		temp_D = atoi(pDoc->m_strSpecTime.Mid(4,2));
		temp_H = atoi(pDoc->m_strSpecTime.Mid(6,2));
		temp_B = atoi(pDoc->m_strSpecTime.Mid(8,2));

		temp_H += pDoc->m_nSpecTime;
		plus_D = (int)(temp_H/24);
		if(plus_D > 0)
		{
			temp_H = temp_H%24;
			temp_D += plus_D;
			if(temp_M == 2)
			{
				if(temp_Y % 4 == 0) nMonth = 29;
				else nMonth = 28;
			}
			else if(temp_M == 1 || temp_M == 3 || temp_M == 5 || temp_M == 7 || temp_M == 8 || temp_M == 10 || temp_M == 12) nMonth = 31;
			else nMonth = 30;
			plus_M = (int)(temp_D/nMonth);
			if(plus_M > 0)
			{
				temp_D = temp_D%nMonth;
				temp_M++;
				if(temp_M > 12) 
				{temp_Y++; temp_M = 1;}
			}
		}
	}
	strTotal.Format("%02d%02d%02d%02d%02d",temp_Y,temp_M,temp_D,temp_H,temp_B);
	return strTotal;
}
*/
/*void CAuto::SubTimerAutoSpecTime()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CTime pTime = CTime::GetCurrentTime();
	BOOL bAct = FALSE;
	int temp[5] = {0,};

	for(int i = 0; i < 10; i+=2)
	{
		temp[i/2] = (int)atoi(m_strDuedate.Mid(i,2));
	}
	if(temp[0] < pTime.GetYear() - 2000) bAct = FALSE;
	else if((temp[1] < pTime.GetMonth()) && (temp[0] == pTime.GetYear() - 2000)) bAct = FALSE;
	else if(temp[2] < pTime.GetDay() && temp[1] == pTime.GetMonth()) bAct = FALSE;
	else if((temp[3] < pTime.GetHour()) && (temp[2] == pTime.GetDay()) && (temp[1] == pTime.GetMonth())) bAct = FALSE;
	else if((temp[4] <= pTime.GetMinute()) && (temp[3] == pTime.GetHour()) && (temp[2] == pTime.GetDay()) && (temp[1] == pTime.GetMonth())) bAct = FALSE;
	else bAct = TRUE;

	if(g_bSpecCount == TRUE)
	{
		m_ctrlSpecmove.SetBackColor(LIGHTGREEN);
		m_ctrlSpecmove.SetCaption("동   작");
	}
	if(g_bSpecCount == FALSE)
	{
		m_ctrlSpecmove.SetBackColor(LIGHTGRAY);
		m_ctrlSpecmove.SetCaption("정   지");
		KillTimer(TIMER_AUTO_SPEC_TIME);
	}
	if(bAct == FALSE)
	{
		pDoc->m_strSpecTime.Format("%02d%02d%02d%02d%02d",pTime.GetYear() - 2000,pTime.GetMonth(),pTime.GetDay(),pTime.GetHour(),pTime.GetMinute());
		m_strDuedate = OnCalDuedate();		
	}
}
*/

void CAuto::SubTimerSupplyFunc()
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	if(bSupplyStart == TRUE)
	{
		m_pTimeSupply = CTime::GetCurrentTime();
		m_nSupplyTimeCur=m_pTimeSupply.GetSecond();

		if(m_nSupplyTimeCur != m_nSupplyTimePrev)
		{
			m_nSupplyTimePrev = m_nSupplyTimeCur;

			m_nSupplyTimeCount++;
		}

//		if(m_nSupplyTimeCount >= pDoc->m_dTeachData[36][2])
		if(m_nSupplyTimeCount >= g_dSupplyTime)
		{
			m_nSupplyTimeCount = 0;
			bSupplyComplete = TRUE;
			bSupplyStart = FALSE;
//			KillTimer(TIMER_LC_SUPPLY);
		}
	}
}

void CAuto::OnClickGlsDataChange() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	g_bGlassDataChange = TRUE;

	CGlassDataChange dlg;
	if(dlg.DoModal()==IDOK)
	{
		//기존 받은 GlassData 적어준후...
		pView->SendMessage(WM_CIMINFO,3,0);//Write_GlassData()

		//CIM에 보고시..CEID는 0x4840에 "0007"을적어준다.
		pView->m_pDevice->MNET_WriteWord(SELF_INFO+0x20, "0007", 1); 

//		pView->m_pDevice->MNET_WriteWord(SELF_INFO,CIM_Information.Recipe_Number,1);
		pView->m_pDevice->MNET_WriteWord(SELF_INFO,CIM_Information.Recipe_No,1);
		
		pView->m_pDevice->MNET_WriteWord(SELF_INFO+0x02,CIM_Information.LotSlot_No,1);
//		pView->m_pDevice->MNET_WriteWord(SELF_INFO+0x02,CIM_Information.Lot_number,1);
//		pView->m_pDevice->MNET_WriteWord(SELF_INFO+0x02,CIM_Information.Slot_number,1);
		
		pView->m_pDevice->MNET_WriteWord(SELF_INFO+0x03,CIM_Information.Glass_ID,8);
//		pView->m_pDevice->MNET_WriteWord(SELF_INFO+0x03,CIM_Information.GlassID_ASC,8);

		pView->m_pDevice->MNET_WriteWord(SELF_INFO+0x0B,CIM_Information.Glass_Judge,1);
//		pView->m_pDevice->MNET_WriteWord(SELF_INFO+0x0B,CIM_Information.Glass_Judge_ASC,1);


		pView->m_pDevice->MNET_BitSet(DISP_BIT+0x24,TRUE);
		Sleep((int)(pDoc->m_dTeachData[37][0]*1000.0));
		pView->m_pDevice->MNET_BitSet(DISP_BIT+0x24,FALSE);

		PostMessage(WM_INFODISP,3,TRUE);	// CIM Data display  : Glass ID, Lot No, Slot No,..

		SetDlgItemText(IDC_LABEL_MESSAGE, CIM_Information.Glass_Type);
		CString str = "";
		str = CIM_Information.Glass_Type;
		
		AfxMessageBox(str);
	}
	else
	{
		Sleep(10);
	}

/*
	CString Recipe_Number,Lot_number,Glass_ID,Glass_Judge;

	//기존 받은 GlassData 적어준후...
	//pView->SendMessage(WM_CIMINFO,3,0);//Write_GlassData()
	
	//바뀐 Data를 덮어씌운다.
	CIM_Information.Glass_ID = pView->m_pDevice->MakeHexDatatoPLC(CIM_Information.Glass_ID,16);

	//Key in추가해야함.
	CIM_Information.Recipe_No="0009";
	CIM_Information.LotSlot_No= "0909";
	CIM_Information.Glass_ID="TOPENGDISPENSER";
	CIM_Information.Glass_Judge="FFFF";
	CIM_Information.Glass_ID = pView->m_pDevice->MakeHexDatatoPLC(CIM_Information.Glass_ID,16);

	pView->m_pDevice->MNET_WriteWord(SELF_INFO,CIM_Information.Recipe_No,1);
	////pView->m_pDevice->MNET_WriteWord(SELF_INFO,Glass_Type,1);
	pView->m_pDevice->MNET_WriteWord(SELF_INFO+0x02,CIM_Information.LotSlot_No,1);
	pView->m_pDevice->MNET_WriteWord(SELF_INFO+0x03,CIM_Information.Glass_ID,8);
	pView->m_pDevice->MNET_WriteWord(SELF_INFO+0x0B,CIM_Information.Glass_Judge,1);

	pView->m_pDevice->MNET_BitSet(DISP_BIT+0x24,TRUE);
	Sleep(1000);
	pView->m_pDevice->MNET_BitSet(DISP_BIT+0x24,FALSE);


*/
}

void CAuto::OnClickGrease2() 
{
	// TODO: Add your control notification handler code here
	g_bMotorMoveView = TRUE;
	CGrease dlg;
	dlg.DoModal();		
}

void CAuto::OnClickGlsDataRequest() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	short CIM_BIT_CNT=0x00;
	short CIM_REPORT_AREA=0x00E0;
	g_bGlassDataChange=FALSE;
	
	CString str;

	CGlassDataChange dlg;
	if(dlg.DoModal()==IDOK)
	{
	//	m_ctrlGlassDataReq.SetBackColor(LIGHTRED);
		pFrame->DoEvents();

	//	CIM_Information.Glass_ID = "22230";
		
	//	CIM_Information.Glass_ID = m_pDevice->MakeHexDatatoPLC(CIM_Information.Glass_ID,16);
	//	str = "0203";

		//둘중에 하나로만 선택하면 된다.
		//선택에 의해 ID(ACCII) or CODE(INTEGER)로 보고 할 수 있도록 하자...//by shin//
		if(dlg.m_nSelNo == 0) //GlsID로 요구시.."0000" 
		{
			pView->m_pDevice->MNET_WriteWord(GLS_SPC_DATA, CIM_Information.Glass_ID, 8);			
			pView->m_pDevice->MNET_WriteWord(GLS_SPC_DATA+0x09, "0000", 1);
		}
		else				  //GlsCode로 요구시.."0001"	
		{
			pView->m_pDevice->MNET_WriteWord(GLS_SPC_DATA+0x08, CIM_Information.LotSlot_No, 1);
			pView->m_pDevice->MNET_WriteWord(GLS_SPC_DATA+0x09, "0001", 1);
		}				
		pView->m_pDevice->MNET_BitSet(DISP_REPORT+0x03, true); //Glass Data Req
		Sleep(0);
		
		switch(atoi(pView->m_strMachineNo))
		{
			case 1 : 
				CIM_BIT_CNT = 0x00; break;
				//CIM_REPORT_BIT = 0x00E5; break;		
			case 2 :
				//CIM_REPORT_BIT = 0x00E7; break;		
				CIM_BIT_CNT = 0x03; break;
		}

		g_bTimeOverT3 = FALSE;
		pView->SetTimer(T3_TIMER ,(int)(pDoc->m_dTeachData[37][2]*1000.0), NULL);
			
		while(1)
		{
			//Ack Code ON?
			if(pView->m_pDevice->MNET_Read_A_Bit(CIM_ACK_BIT,0))
			{
				Sleep(200);
				//Data Report ON?
				//if(pView->m_pDevice->MNET_Read_A_Bit(CIM_REPORT_BIT,0))				
				if(pView->m_pDevice->MNET_Read_A_Bit(CIM_REPORT_AREA,CIM_BIT_CNT))
				{
					pView->m_pDevice->MNET_BitSet(DISP_REPORT+0x03,false); 				
					KillTimer(T3_TIMER);
					g_bTimeOverT3 = FALSE;
					//Data Read..
					pView->ReadGlassData(0); //# Read From CIM
					break;
				}			
			}
			//Nack Code ON?
			if(pView->m_pDevice->MNET_Read_A_Bit(CIM_ACK_BIT,1))
			{
				pView->m_pDevice->MNET_BitSet(DISP_REPORT+0x03,false); 
				KillTimer(T3_TIMER);
				g_bTimeOverT3 = FALSE;
				//Warning 처리..
				SendMessage(WM_ERROR, 206, NULL); 
				break;
			}
			if(g_bTimeOverT3)
			{
				pView->m_pDevice->MNET_BitSet(DISP_REPORT+0x03,false);
				KillTimer(T3_TIMER);
				g_bTimeOverT3 = FALSE;	
				
				//Warning 발생 보고..
				CIM_Information.bWarning_ON = TRUE;
				CIM_Information.nWarning_Code = 206|0x8000;
				CIM_Information.sWarning_Code.Format("%04x",CIM_Information.nWarning_Code);
				pView->m_pDevice->MNET_WriteWord(DISP_WORD+0x0E, CIM_Information.sWarning_Code, 1);
				pView->m_pDevice->MNET_BitSet(DISP_BIT+0x0E, TRUE);

				Sleep((int)(pDoc->m_dTeachData[37][0]*1000.0));
				
				pView->m_pDevice->MNET_BitSet(DISP_BIT+0x0E, FALSE);
				
				SendMessage(WM_ERROR, 207, NULL); 
				
				//Warning 해제 보고..
				CIM_Information.nWarning_Code = CIM_Information.nWarning_Code & 0x7fff;
				CIM_Information.sWarning_Code.Format("%04x", CIM_Information.nWarning_Code);	
				pView->m_pDevice->MNET_WriteWord(DISP_WORD+0x0E, CIM_Information.sWarning_Code, 1); 
				pView->m_pDevice->MNET_BitSet(DISP_BIT+0x0E, TRUE);
				
				Sleep((int)(pDoc->m_dTeachData[37][0]*1000.0));
				
				pView->m_pDevice->MNET_BitSet(DISP_BIT+0x0E, FALSE);
				CIM_Information.bWarning_ON = FALSE;
				break;
			}		
			pFrame->DoEvents();
			Sleep(100);
		}
		m_ctrlGlassDataReq.SetBackColor(LIGHTCYAN); 
	}
	else
	{
		return;	
	}
	
}

// by ckh 2009.03.09 부져 끄기
void CAuto::OnClickLcRemain() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	//lbg 080701 Buzzer Stop
	pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispBuzzer,"0000");
	//if(m_bBuzzer == TRUE) 
		pView->SaveLog(0,"Auto 창 하한 센서 미감지 부저 Off");
	m_bBuzzer = FALSE;
}

void CAuto::OnDblClickCmdAutoTitle(LPDISPATCH Cancel) 
{

	return;		//20160421 ysm 투입제한 버튼 막음

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	// TODO: Add your control notification handler code here
	if(ThreadStage.ExitFlag == FALSE) return;
	if(ThreadStage.JobModeSelection == 0)
	{
		if(g_bDropStartLimit == TRUE)
		{
			g_bDropStartLimit= false;

			if(pView->m_nLanguage == 0)
			m_ctrlAutoModeTitle.SetCaption("AUTO            (투입 제한 해제)");
			else if(pView->m_nLanguage == 1)
			m_ctrlAutoModeTitle.SetCaption("AUTO            (Insert Limit Clear)");
			else if(pView->m_nLanguage == 2)
			m_ctrlAutoModeTitle.SetCaption("AUTO            (投入限制解除)");

			m_ctrlAutoModeTitle.SetBackColor(BLUE);
		}
		else
		{
			g_bDropStartLimit= true;

			if(pView->m_nLanguage == 0)
			m_ctrlAutoModeTitle.SetCaption("AUTO            (투입 제한)");
			else if(pView->m_nLanguage == 1)
			m_ctrlAutoModeTitle.SetCaption("AUTO            (Insert Limit)");
			else if(pView->m_nLanguage == 2)
			m_ctrlAutoModeTitle.SetCaption("AUTO            (投入限制)");

			m_ctrlAutoModeTitle.SetBackColor(RED);
		}
	}					
}

void CAuto::OnClickAlignView() 
{
	// TODO: Add your control notification handler code here
	CAlignDraw dlg;
	dlg.DoModal();
}

void CAuto::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	// TODO: Add your message handler code here and/or call default
	if(g_bRemoteControl)
		pView->pTempOp->DoModal();
	else
		AfxMessageBox("Remote Mode Off!");
	CDialog::OnRButtonDblClk(nFlags, point);
}

void CAuto::OnClickCmdWarmmingUp() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(pView->m_bUseWarmUp)
	{
		pView->m_bUseWarmUp = FALSE ; 
		m_ctrlWarmUp.SetCaption("Warm Up X "); 
		m_ctrlWarmUp.SetBackColor(RED); 
	}
	else
	{
		pView->m_bUseWarmUp = TRUE ; 
		m_ctrlWarmUp.SetCaption("Warm Up O "); 
		m_ctrlWarmUp.SetBackColor(GREEN); 
	}
}

void CAuto::OnClickRecipeInfo() 
{
	// TODO: Add your control notification handler code here
#if N_MODIFY_EQ
	CPiezoStatus dlg;
	dlg.DoModal();
#endif	
}

void CAuto::SelectLanguage()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(pView->m_nLanguage == 0)		//ehji 140309
	{
		SetDlgItemText(IDC_STATIC,				_T("Step 동작 및 소요 시간"));
		SetDlgItemText(IDC_AUTO_LOAD,				_T("반 입"));
		SetDlgItemText(IDC_AUTO_DROP,				_T("토출 요구"));
		SetDlgItemText(IDC_AUTO_UNLOAD,				_T("반 출"));
		SetDlgItemText(IDC_AUTO_MEASURE,				_T("측정 및 보정"));
		SetDlgItemText(IDC_LABEL_BALANCE,				_T("BALANCE 상태"));
		SetDlgItemText(IDC_LC_REMAIN,				_T("Bottle 액정 잔량(g)"));
		SetDlgItemText(IDC_CAN_WEIGHT,				_T("Canister 잔량(kg) / 공급량(g)"));
		SetDlgItemText(IDC_CMD_GREASE_MSG,				_T("Column K축 구리스 주입!!!"));
		SetDlgItemText(IDC_GREASE2,				_T("모터 주행 거리 보기"));
		SetDlgItemText(IDC_GLS_DATA_CHANGE,				_T("GLS Data 변경"));
		SetDlgItemText(IDC_GLS_DATA_REQUEST,				_T("GLS Data 요구"));
		SetDlgItemText(IDC_CMD_GLS_CNT_CLEAR,				_T("반입 Glass 개수 초기화"));
		SetDlgItemText(IDC_CMD_AUTOINTERFACE,				_T("Robot Interface     모니터링"));
		SetDlgItemText(IDC_AUTO_SCRAP,				_T("Glass Scrap 처리"));
		SetDlgItemText(IDC_AUTO_ERROR_HISTORY,				_T("ERROR  이력"));
		SetDlgItemText(IDC_CMD_MEASUREDDATA_DRAW,				_T("측정결과 GRAPH"));
		SetDlgItemText(IDC_AUTO_STEP,				_T("스텝운전(정지)"));
		SetDlgItemText(IDC_AUTO_CONTINUOUS,				_T("연속운전"));
		SetDlgItemText(IDC_LABEL_ADDDEL_CHANGE,				_T("A/D 변환 ON"));
		SetDlgItemText(IDC_LABEL_ADDDEL_COMPARE,				_T("A/D 비교 ON"));
		SetDlgItemText(IDC_LC_QUAN_REPORT,				_T("상한보고ON"));
		SetDlgItemText(IDC_LC_QUAN_REPORT_01,				_T("하한보고ON"));
		SetDlgItemText(IDC_ON_OFF_LINEMODE,				_T("Offline Mode 변경"));
		SetDlgItemText(IDC_CMD_JOB_MOVE,				_T("작업위치이동"));
		SetDlgItemText(IDC_CMD_LOAD_SENSOR,				_T("반입센서모드ON"));

	}

	else if(pView->m_nLanguage == 1)	//ehji 140309
	{
		SetDlgItemText(IDC_STATIC,				_T("Step Action & Time Required"));
		SetDlgItemText(IDC_AUTO_LOAD,				_T("Loading"));
		SetDlgItemText(IDC_AUTO_DROP,				_T("Dispensing Demand"));
		SetDlgItemText(IDC_AUTO_UNLOAD,				_T("Unloading"));
		SetDlgItemText(IDC_AUTO_MEASURE,				_T("Measure & Revision"));
		SetDlgItemText(IDC_LABEL_BALANCE,				_T("BALANCE Condition"));
		SetDlgItemText(IDC_LC_REMAIN,				_T("Bottle LC Amount(g)"));
		SetDlgItemText(IDC_CAN_WEIGHT,				_T("Canister Amount(kg) / Supply Amount(g)"));
		SetDlgItemText(IDC_CMD_GREASE_MSG,				_T("Column K Axis Grease inJect!!!"));
		SetDlgItemText(IDC_GREASE2,				_T("Motor Distence Covered View"));
		SetDlgItemText(IDC_GLS_DATA_CHANGE,				_T("GLS Data Change"));
		SetDlgItemText(IDC_GLS_DATA_REQUEST,				_T("GLS Data Demand"));
		SetDlgItemText(IDC_CMD_GLS_CNT_CLEAR,				_T("Loading Glass Count Initial"));
		SetDlgItemText(IDC_CMD_AUTOINTERFACE,				_T("Robot Interface Mornitering"));
		SetDlgItemText(IDC_AUTO_SCRAP,				_T("Glass Scrap Conduct"));
		SetDlgItemText(IDC_AUTO_ERROR_HISTORY,				_T("ERROR Record"));
		SetDlgItemText(IDC_CMD_MEASUREDDATA_DRAW,				_T("Measure Result GRAPH"));
		SetDlgItemText(IDC_AUTO_STEP,				_T("Step (Stop)"));
		SetDlgItemText(IDC_AUTO_CONTINUOUS,				_T("Continue"));
		SetDlgItemText(IDC_LABEL_ADDDEL_CHANGE,				_T("A/D Change ON"));
		SetDlgItemText(IDC_LABEL_ADDDEL_COMPARE,				_T("A/D Conpare ON"));
		SetDlgItemText(IDC_LC_QUAN_REPORT,				_T("Higher Report ON"));
		SetDlgItemText(IDC_LC_QUAN_REPORT_01,				_T("Lowest Report ON"));
		SetDlgItemText(IDC_ON_OFF_LINEMODE,				_T("Offline Mode Change"));
		SetDlgItemText(IDC_CMD_JOB_MOVE,				_T("Job Position Move"));
		SetDlgItemText(IDC_CMD_LOAD_SENSOR,				_T("Loading Sensor Mode ON"));

	}

	else if(pView->m_nLanguage == 2)	//ehji 140309
	{
		SetDlgItemText(IDC_STATIC,				_T("Step 動作及需要時間"));
		SetDlgItemText(IDC_AUTO_LOAD,				_T("搬入"));
		SetDlgItemText(IDC_AUTO_DROP,				_T("吐出要求"));
		SetDlgItemText(IDC_AUTO_UNLOAD,				_T("搬出"));
		SetDlgItemText(IDC_AUTO_MEASURE,				_T("測定及補正"));
		SetDlgItemText(IDC_LABEL_BALANCE,				_T("BALANCE 狀態"));
		SetDlgItemText(IDC_LC_REMAIN,				_T("Bottle 液晶餘量(g)"));
		SetDlgItemText(IDC_CAN_WEIGHT,				_T("Canister 餘量(kg) / 供給量(g)"));
		SetDlgItemText(IDC_CMD_GREASE_MSG,				_T("Column K軸 注入潤滑油!!!"));
		SetDlgItemText(IDC_GREASE2,				_T("電機執行距離設定"));
		SetDlgItemText(IDC_GLS_DATA_CHANGE,				_T("GLS Data 變更"));
		SetDlgItemText(IDC_GLS_DATA_REQUEST,				_T("GLS Data 要求"));
		SetDlgItemText(IDC_CMD_GLS_CNT_CLEAR,				_T("搬入Glass數初期化"));
		SetDlgItemText(IDC_CMD_AUTOINTERFACE,				_T("Robot Interface     顯示"));
		SetDlgItemText(IDC_AUTO_SCRAP,				_T("Glass Scrap 處理"));
		SetDlgItemText(IDC_AUTO_ERROR_HISTORY,				_T("ERROR  歷史"));
		SetDlgItemText(IDC_CMD_MEASUREDDATA_DRAW,				_T("測定結果 GRAPH"));
		SetDlgItemText(IDC_AUTO_STEP,				_T("運轉步驟（停止）"));
		SetDlgItemText(IDC_AUTO_CONTINUOUS,				_T("連續運轉"));
		SetDlgItemText(IDC_LABEL_ADDDEL_CHANGE,				_T("A/D 變換 ON"));
		SetDlgItemText(IDC_LABEL_ADDDEL_COMPARE,				_T("A/D 比較 ON"));
		SetDlgItemText(IDC_LC_QUAN_REPORT,				_T("上限報告ON"));
		SetDlgItemText(IDC_LC_QUAN_REPORT_01,				_T("下限報告"));
		SetDlgItemText(IDC_ON_OFF_LINEMODE,				_T("Offline Mode 變更"));
		SetDlgItemText(IDC_CMD_JOB_MOVE,				_T("作業位置移動"));
		SetDlgItemText(IDC_CMD_LOAD_SENSOR,				_T("搬入Sensor Mode ON"));

	}

}
//2014.04.21 by tskim TPD
void CAuto::SubTimerAutoTPD()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	CString strTPD,strTemp;
	strTPD.Empty();strTemp.Empty();
	int i;
	
//	if(nTempCount_K[0] == 51) nTempCount_K[0] =0;
//	nTempCount_K[0]++;
	strTemp.Format("%04x",pDoc->m_structMotorload.m_nMotorLoadK[0]);
	strTPD += strTemp;
//	if(nTempCount_K[1] == 101) nTempCount_K[1] =50;
//	nTempCount_K[1]++;
	strTemp.Format("%04x",pDoc->m_structMotorload.m_nMotorLoadK[1]);
	strTPD += strTemp;
	for(i=0;i<MAX_NOZZLE;i++)
	{
//		if(nTempCount_S[i] == 0) nTempCount_S[i] =100+i;
//		nTempCount_S[i]--;
		strTemp.Format("%04x",pDoc->m_structMotorload.m_nMotorLoadS[i]);
		strTPD += strTemp;
	}

	pView->m_pDevice->MNET_WriteWord(TPD_DATA, strTPD, 18);
}

void CAuto::OnClickCmdLoadSensor() 
{

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	// TODO: Add your control notification handler code here
	if ( g_bLoadUnLoadSensorMode )
	{
		g_bLoadUnLoadSensorMode = FALSE; 

		m_ctrlLoadSensorMode.SetBackColor(RED); 
		if(pView->m_nLanguage == 0)
		m_ctrlLoadSensorMode.SetCaption("반입 센서 모드 ON"); 

		else if(pView->m_nLanguage == 1)
		m_ctrlLoadSensorMode.SetCaption("Loading Sensor Mode ON");
		
		else if(pView->m_nLanguage == 2)
		m_ctrlLoadSensorMode.SetCaption("搬入Sensor Mode ON");
	}
	else 
	{
		g_bLoadUnLoadSensorMode = TRUE;
		
		m_ctrlLoadSensorMode.SetBackColor(GREEN); 
		if(pView->m_nLanguage == 0)
		m_ctrlLoadSensorMode.SetCaption("반입 센서 모드 OFF"); 
	
		else if(pView->m_nLanguage == 1)
		m_ctrlLoadSensorMode.SetCaption("Loading Sensor Mode OFF"); 
	
		else if(pView->m_nLanguage == 2)
		m_ctrlLoadSensorMode.SetCaption("搬入Sensor Mode OFF"); 
	}	
}

void CAuto::OnClickCmdJobMove() 
{

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	// TODO: Add your control notification handler code here
	if ( g_bJobMovePos )
	{
	//	g_bJobMovePos = FALSE; 

		m_ctrlJobMovePos.SetBackColor(RED); 


		if(pView->m_nLanguage == 0)
		m_ctrlJobMovePos.SetCaption("작업 위치 이동 OFF"); 	
		else if(pView->m_nLanguage == 1)
		m_ctrlJobMovePos.SetCaption("Job Position Move OFF"); 		
		else if(pView->m_nLanguage == 2)
		m_ctrlJobMovePos.SetCaption("作業位置移動 OFF"); 

	}
	else 
	{
	//	g_bJobMovePos = TRUE;
		
		m_ctrlJobMovePos.SetBackColor(GREEN); 
		
		if(pView->m_nLanguage == 0)
		m_ctrlJobMovePos.SetCaption("작업 위치 이동 ON"); 
		else if(pView->m_nLanguage == 1)
		m_ctrlJobMovePos.SetCaption("Job Position Move ON"); 
		else if(pView->m_nLanguage == 2)
		m_ctrlJobMovePos.SetCaption("作業位置移動 ON"); 

	}	
}

void CAuto::OnClickCMDValcRun() //ehji 141013
{
	CString strLog;

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	// TODO: Add your control notification handler code here

	if (g_bValcRun)	//ehji 141013 
	{
		strLog.Empty();
		strLog.Format("VALC OFF");	//VALC 신호 Log 추가
		pView->SaveLog(0, strLog);

		g_bValcRun = FALSE;
		m_ctrlValcRun.SetBackColor(RED);
		m_ctrlValcRun.SetCaption("Valc RUN OFF"); 
	}
	else 
	{
		strLog.Empty();
		strLog.Format("VALC ON");	//VALC 신호 Log 추가
		pView->SaveLog(0, strLog);

		g_bValcRun = TRUE;
		m_ctrlValcRun.SetBackColor(GREEN);
		m_ctrlValcRun.SetCaption("Valc RUN ON"); 
	}
	
}

void CAuto::OnClickAutoVision() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(ThreadStage.ExitFlag == FALSE) return;
	ThreadStage.JobStep = 1000;

	CString strTemp;
	
	strTemp.Format("Auto Vision Click");
	pView->SaveLog(0, strTemp);	
}

void CAuto::OnClickCmdLogWrite() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if ( g_bLogWriteOff )
	{
		g_bLogWriteOff = FALSE; 

		m_ctrlLogWrite.SetBackColor(GREEN); 
		if(pView->m_nLanguage == 0)
		m_ctrlLogWrite.SetCaption("LOG ON"); 

		else if(pView->m_nLanguage == 1)
		m_ctrlLogWrite.SetCaption("LOG ON");
		
		else if(pView->m_nLanguage == 2)
		m_ctrlLogWrite.SetCaption("LOG ON");
	}
	else 
	{
		g_bLogWriteOff = TRUE;
		
		m_ctrlLogWrite.SetBackColor(RED); 
		if(pView->m_nLanguage == 0)
		m_ctrlLogWrite.SetCaption("LOG OFF"); 
	
		else if(pView->m_nLanguage == 1)
		m_ctrlLogWrite.SetCaption("LOG OFF"); 
	
		else if(pView->m_nLanguage == 2)
		m_ctrlLogWrite.SetCaption("LOG OFF"); 
	}
	
}

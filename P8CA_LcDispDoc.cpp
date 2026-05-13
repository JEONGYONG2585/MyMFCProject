// P8CA_LcDispDoc.cpp : implementation of the CP8CA_LcDispDoc class
//
#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "P8CA_LcDispDoc.h"
#include "P8CA_LcDispView.h"
#include "N_Vision.h"


#include <fstream.h>
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 외부변수를 사용하기 위해서 extern으로 선언..
extern CIM_DATA CIM_Information;
extern THREADSTAGE ThreadStage;
extern CIM_GLASS_INFORM_SAVE Glass_Inform[2][10];	// 0:TFT, 1:CF
extern DROPINFO	Drop_Info;
extern UINT g_nGlassAlignCount;

extern CN_Vision _NVision;

extern CString g_strLogPath;//2014.10.20 byb tskim LC 소모량

extern BOOL  g_bIsHeadSelected[MAX_NOZZLE];
extern double g_dTeachData[40][3];
extern double m_dMeasActionTimeOffSet[MAX_NOZZLE];
/////////////////////////////////////////////////////////////////////////////
// CP8CA_LcDispDoc

IMPLEMENT_DYNCREATE(CP8CA_LcDispDoc, CDocument)

BEGIN_MESSAGE_MAP(CP8CA_LcDispDoc, CDocument)
	//{{AFX_MSG_MAP(CP8CA_LcDispDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CP8CA_LcDispDoc construction/destruction

CP8CA_LcDispDoc::CP8CA_LcDispDoc()
{
	// TODO: add one-time construction code here

	int i=0, j=0;
//
	m_arDropData = new CArray<DROP_DATA, DROP_DATA>[MAX_PATTERN];
//2010.06.21 by tskim Repair
	m_arDropDataBk = new CArray<DROP_DATA, DROP_DATA>[MAX_PATTERN];

	m_arLineData = new CArray<LINE_DATA, LINE_DATA>;
	m_arLineDataBk = new CArray<LINE_DATA, LINE_DATA>;
//	m_arLineDataPat = new CArray<LINE_DATA, LINE_DATA>[MAX_PATTERN];
	m_arLineDataPat = new CArray<LINE_DATA, LINE_DATA>;

//2009.12.31 by tskim
	m_arDropData_R = new CArray<DROP_DATA, DROP_DATA>[MAX_PATTERN];
	m_arLineData_R = new CArray<LINE_DATA, LINE_DATA>;

//121210 
	m_arMMGLineData        = new CArray<LINE_DATA , LINE_DATA>; 
	m_arMainLineData       = new CArray<LINE_DATA , LINE_DATA>; 
	m_arMMGLineDataBk      = new CArray<LINE_DATA , LINE_DATA>;
	m_arMainLineDataBk     = new CArray<LINE_DATA , LINE_DATA>;
	m_arMMGLineDataTemp    = new CArray<LINE_DATA , LINE_DATA>;
	m_arMainLineDataTemp   = new CArray<LINE_DATA , LINE_DATA>;
	m_arMMGLineData_R      = new CArray<LINE_DATA , LINE_DATA>;
	m_arMainLineData_R     = new CArray<LINE_DATA , LINE_DATA>;
	m_arMainMMGYLineData   = new CArray<LINE_DATA , LINE_DATA>; 
	m_arMainMMGYLineData_R = new CArray<LINE_DATA , LINE_DATA>; 
//////////////
//121210 추가 사항
	m_arMMGLineDataPat      = new CArray<LINE_DATA , LINE_DATA>; 
	m_arMainLineDataPat     = new CArray<LINE_DATA , LINE_DATA>; 
	m_arMainMMGYLineDataPat = new CArray<LINE_DATA , LINE_DATA>;
//////////////////
	


// 현재 작업중인 doc가 engineer mode인지, operator mode인지...
	m_bIsEngineerMode = FALSE;
	m_bMaintMode=FALSE;	

	// sequence 설정 변수
	for(i = 0; i < 4; i++)
		for(j = 0; j < MAX_PATTERN; j++)
			m_nSeq[i][j] = 0;

	// Drop Data의 ADD와 DEL
	for(i = 0; i < 4; i++)
	{
		m_nAdd[i] = 0;
		m_nDel[i] = 0;
	}

	// Data Editor에 있는 변수들 초기화
	m_structDataEditor.m_dGlassSizeX = 2500.0;	
	m_structDataEditor.m_dGlassSizeY = 2250.0;	
	m_structDataEditor.m_dThickness = 0.70;	
	m_structDataEditor.m_nPatternNum = 3;	
	m_structDataEditor.m_dHomeX = 0.0;		
	m_structDataEditor.m_dHomeY = 0.0;		
	m_structDataEditor.m_strComment = _T("");		
	m_structDataEditor.m_nSequenceCount = 3;
	m_structDataEditor.m_nSetDispense = 999;
	m_structDataEditor.m_bPatternDispensing = TRUE;
	m_structDataEditor.m_bNgDropping = FALSE;
	m_structDataEditor.m_bRecipeNoCompare = FALSE;
	m_structDataEditor.m_bLCQuanReportPlus=FALSE;
	m_structDataEditor.m_bLCQuanReportMinus=FALSE;
	m_structDataEditor.m_bAddDelChange=FALSE;
	m_structDataEditor.m_bAddDelCompare=FALSE;
	m_structDataEditor.m_bIsTrfOnlyMode=FALSE;
	m_structDataEditor.m_nDropRepeatCnt=1;

	m_structDataEditor.m_bMarkAlignInsp = FALSE;
	m_structDataEditor.m_dMarkX[0] = 0.0;
	m_structDataEditor.m_dMarkX[1] = 0.0;
	m_structDataEditor.m_dMarkY[0] = 0.0;
	m_structDataEditor.m_dMarkY[1] = 0.0;
	m_structDataEditor.m_nMarkAlignInspInterval = 1;
	m_structDataEditor.m_nCamLightSet[0] = 255;
	m_structDataEditor.m_nCamLightSet[1] = 255;
	m_structDataEditor.m_nCamScore = 50;

	m_structDataEditor.m_bUse_Vision = FALSE;
	m_structDataEditor.m_bMove_Unload = FALSE;
	m_structDataEditor.m_nCalibrationMode = 0;
	m_structDataEditor.m_nNzlMode = 0;

	for(i = 0 ; i < MAX_NOZZLE ; i++)
	{
		m_structDataEditor.m_strLcType[i] = _T("");
		m_structDataEditor.m_strBarcode[i] = _T("AAAAAA");
		m_structDataEditor.m_nLineDropNumber[i] = -1;
		m_structDataEditor.m_dSuctionVolumn[i] = -1;
		m_structDataEditor.m_nBlockNumber[i] = -1;
		m_structDataEditor.m_nLineDropBlockNum[i] = -1;//2014.03.25 by tskim
	}
	m_structDataEditor.m_nLcSupplyType=0;
	m_structDataEditor.m_nDummyType=0;
	m_structDataEditor.m_bSpecialInfo = true;
	m_structDataEditor.m_bSafetyLock = true;
	m_structDataEditor.m_bGlassCheck = true;
	m_structDataEditor.m_bSelectDropMode = false;
	m_structDataEditor.m_nLinePatternNumber =1;

	m_structDataEditor.m_bWorkMove = FALSE;		//ehji 141125


//160903 DUAL INTENSITY //160929 JYKIM
	m_structDataEditor.m_bDualIntensity = FALSE; 
//160903 SAMPLING SCAN - DATA READ 부분은 삭제 하지 않는다. //160929 JYKIM
	m_structDataEditor.m_bSamplingScan  = FALSE; 

	for(i = 0 ; i < MAX_NOZZLE ; i++)			//ehji 150602
	{
		m_structDataEditor.m_bHeadable[i] = FALSE;
	}
	m_structDataEditor.m_bMMGLineDropMode = FALSE;

	m_structDataEditor.m_nGroupLineDrop = -1;
	m_structDataEditor.m_nMaxLineDropBlockNum = -1;

	// Adjusting Condition에 있는 변수들 초기화
	m_structAdjustCondition.dSpeed = 1.0;
	m_structAdjustCondition.nDelay = 30;
	m_structAdjustCondition.fDropStartPos = 10.0;
	m_structAdjustCondition.nDummyDropTime = 60;
	m_structAdjustCondition.nCheckingIntervalSheet = 5;
	m_structAdjustCondition.bAdjustInAuto = TRUE;
	m_structAdjustCondition.bDummyDropBeforeMeasInAuto=FALSE;
	m_structAdjustCondition.nAdjustTimes = 5;
	m_structAdjustCondition.nMeasureCycle = 1;

	for(i = 0;i < MAX_NOZZLE; i++)
	{
		m_structAdjustCondition.dTargetWeight[i] = 3.0;
		m_structAdjustCondition.dTargetError[i] = 0.3;
		m_structAdjustCondition.dTargetErrorAlarm[i] = 0.5;
		m_structAdjustCondition.dTargetErrorJudgePlus[i] = 1.0;
		m_structAdjustCondition.dTargetErrorJudgeMinus[i] = 1.0;
		m_structAdjustCondition.nDropCount[i] = 20;
		m_structAdjustCondition.nLoopCount[i] = 2;
		m_structAdjustCondition.nSpecIn[i]= 1;
		m_structAdjustCondition.dAbnormalTargetError[i] = 1.5;

		for(int j=0;j<10;j++)
		{
			m_structAdjustCondition.dMultiTarget[i][j] = 0.0;
			m_structAdjustCondition.dMultiTargetRate[i][j]= 0.0;			
		}
	}
	m_structAdjustCondition.nLiquidSupplyLimit = 70; //090702 액정 공급량 Limit 설정치 초기화 
	m_structAdjustCondition.nCanisterEmgWeight=3.5; //090702 Canister 경고량 설정치 초기화 
	m_structAdjustCondition.nCheckDegasSheet = 50;
	m_structAdjustCondition.nDegasCount = 10;
	m_structAdjustCondition.dAdjustPulse = BASE_PULSE;
	m_structAdjustCondition.dMarginRate = 0.0;

	m_structAdjustCondition.dCalibrationInterlock = 0.0; 

	// Pattern Data에 있는 변수들 초기화
	for(i = 0; i < MAX_PATTERN; i++)
	{
		m_structPatternData[i].m_dCenterPositionX = 0;
		m_structPatternData[i].m_dCenterPositionY = 0;
		m_structPatternData[i].m_dWidthX = 0;
		m_structPatternData[i].m_dWidthY = 0;
		m_structPatternData[i].m_nPointNumberX = 0;
		m_structPatternData[i].m_nPointNumberY = 0;
		m_structPatternData[i].m_nDroppingNum = 0;
		m_structPatternData[i].m_dAngleX = 0;
		m_structPatternData[i].m_dAngleY = 0;
		m_structPatternData[i].m_nDroppingStartPos = 0;
		m_structPatternData[i].m_nStartDroppingAng = 0;
		m_structPatternData[i].m_nDropDataCount = 0;
				
		m_structPatternData[i].m_nDispenseMode = 0;
		m_structPatternData[i].m_nTableSpeed = 0;
		m_structPatternData[i].m_nSAxisSpeed = 0;

		for(j = 0; j < MAX_NOZZLE; j++)
		{
			m_structPatternData[i].m_dDropWeight[j] = 0.00;
			m_structPatternData[i].m_bIsOnHead[j] = FALSE;
		}

		// Pattern Condition에 있는 일부 데이타
		m_structPatternData[i].m_nSequence = i+1;
		m_structPatternData[i].m_nSequence_R = i+1;

		for(j = 0; j < MAX_NOZZLE; j++)
		{
			m_structPatternData[i].m_dNozzleOffset[j] = 0;
			m_structPatternData[i].m_nPanelNo[j] = 0;
			m_nDropCount[i][j] = 0 ; 
			m_nTotalCount[i][j] = 0 ; 
			m_nFlexibleDropCount[j] = 0;
		}
		m_structPatternData[i].m_bMMG=FALSE;
		m_structPatternData[i].m_dBaseWeight=0.0;
		m_structPatternData[i].m_dLevelWeightDifference=0.0;	

		m_structPatternData[i].m_dDropOffset = 0.0;
		m_structPatternData[i].m_dDischargeSpeed = 0.0;
		m_structPatternData[i].m_dSuctionSpeed = 0;
		m_structPatternData[i].m_nSuctionMode = 0;
		m_structPatternData[i].m_dAccTime = 0;
		m_structPatternData[i].m_dDecTime = 0;
		m_structPatternData[i].m_dStartSpeed = 0;
		m_structPatternData[i].m_nSuctionLineNo = 0;
		m_structPatternData[i].LinePatNum = 0;

		m_structPatternData[i].m_nNShot = 0;
		m_structPatternData[i].m_dNHz = 0.0;
		m_structPatternData[i].m_dNShotActionTime = 0.0;
		m_structPatternData[i].m_dNShotPitch = 0.0;

		for( j = 0 ; j < MAX_NOZZLE ; j++)
		{
			m_structPatternData[i].m_nNAmplitude[j] = 0;
			m_structPatternData[i].m_nNNegOffset[j] = 0;
			m_structPatternData[i].m_dNDuty[j] = 0.0;
			m_structPatternData[i].m_nNFirstVoltage[j] = 0;
			m_structPatternData[i].m_nNRunLimit[j] = 0;
			m_structPatternData[i].m_nNFallingEdge[j] = 0;
			m_structPatternData[i].m_nNPAxisOffset[j] = 0;
			m_structPatternData[i].m_nNRunDelay[j] = 5;
			m_structPatternData[i].m_dTargetWeight_ByPattern[j] = -1;
		}
	}

	// drop data에 있는 변수들 초기화
	// drop data는 CArray이므로 초기화 필요 없음

	// recipe name 기본값으로 설정
	m_structOperatorModeRecipeData.strFrontRecipeName = _T("FrontName");
	m_structOperatorModeRecipeData.strBackRecipeName = _T("BackName");

	// 프로그램 설치시 등록되는 데이타의 경로
	m_strDataPath = _T("D:\\TOP\\P8CA_LC\\P8CA_LC_DATA");


	// nozzle 별 색상지정..
	for(i = 0; i < MAX_NOZZLE; i++)
		m_colorNozzle[i] = RGB(0, 0, 0);
	//0221
	// Col1
	m_colorNozzle[0] = RGB(255, 0, 0);
	m_colorNozzle[1] = RGB(0, 130, 0);
	m_colorNozzle[2] = RGB(0, 0, 255);
	m_colorNozzle[3] = RGB(132, 130,  0);
	m_colorNozzle[4] = RGB(255, 128, 63);
	m_colorNozzle[5] = RGB(128, 128, 255);	
	m_colorNozzle[6] = 0x00C080FF;
	m_colorNozzle[7] = 0x0000FF00;
	m_colorNozzle[8] = 0x00400080;
	m_colorNozzle[9] = 0x008000FF;
	m_colorNozzle[10] = 0x00004080;
	m_colorNozzle[11] = 0x00800080;	
	m_colorNozzle[12] = 0x00FF0080;
	m_colorNozzle[13] = 0x00808040;
	m_colorNozzle[14] = 0x00FFFF00;
	m_colorNozzle[15] = 0x00FF8000;

	// teach 변수 초기화
	for( i =0 ; i <40 ; i++) {
		for(j =0 ; j <3 ; j++) {
			m_dTeachData[i][j] = 0.0 ;
		}
	}
//
	// offset variable initaialization
	for( i =0 ; i <2 ; i++) { // row
		for(j =0 ; j <MAX_NOZZLE; j++) { // column
			m_dOffset[i][j] = 0.0 ;
//2015.04.29 by tskim 원재료 전산화Add LC잔량
			m_dOffset_Prev[i][j] = 0.0;
		}
	}

	// common variable initaialization
	for( i =0 ; i <2 ; i++) { // row
		for(j =0 ; j <MAX_BAL ; j++) { // column
			m_dCommon[i][j] = 0.0 ;
		}
	}

	// Canister variable initaialization
	for( i =0 ; i <2 ; i++) // row
	{ 
		for(j =0 ; j <4 ; j++) { // column
			m_dCanister[i][j] = 0.0 ;
		}
	}
	m_nCanisterBalNo[0] = SUPPLY_BAL1; //Supply Bal1
	m_nCanisterBalNo[1] = SUPPLY_BAL3; //Supply Bal2

	// dummy cup variable initaialization
	for( i =0 ; i <2 ; i++) { // row
		for(j =0 ; j <MAX_NOZZLE; j++) { // dummy cup weight
			m_dDummyCupWeight[i][j] = 0.0 ;
		}
	}

	for(i=0;i<MAX_PANEL;i++)	
	{
		CIM_Information.Worked_Report_DropPoint[i]=-1;
		CIM_Information.DropPoint[i]=-1;
		CIM_Information.Worked_Report_Add_Del[i]=100;

		CIM_Information.MeasuredResult[i]="5";
		m_nValidDrop_compare[i][0] = 0;
		m_nValidDrop_compare[i][1] = 0;
		CIM_Information.nRmsDropPoint[i] = -1;
		CIM_Information.nRms_Worked_Report_DropPoint[i] = -1;
	}
	 
	CIM_Information.nDotting_Grupe_Num = 0;// by ckh 2009.02.25 nDotting_Grupe_Num으로 변경함. 
	CIM_Information.GlassSendCount=0;

	// 이노나이져 항시 ON 모드를 False로 초기화.
	m_bIonizerAlwaysON=FALSE;
	//
	m_dSetLCDetectedCount=0;
	// Glass Identity변수를 초기화.
	for( i = 0 ; i < MAX_PANEL ; i++)
	{
		m_strGlassIden[i] = "";
		m_nConversonAddDel[i] = 0;
	}

	//  Level2Volume변수를 초기화.
	for( i = 0 ; i < 9 ; i++)
		m_dLevel2Volume[i] = 0.0;

	// MeasuredData변수 초기화
	for( i = 0; i < MAX_MEASURED_DATA ; i++)
	{
		for(j = 0 ; j < MAX_NOZZLE ; j++ )
		{
			m_dMeasuredData[j][i] = 0.0;
			m_dMeasuredData_First[j][i] = 0.0;
			m_dMeasuredData_Target[j][i]=0.0;
			m_dMeasuredData_Total[j][i]=0.0;
		}
	}
	// 현재작업중에 선택된 head저장..
	for(i=0; i<MAX_NOZZLE ;i++)
		m_bIsHeadSelected[i]=false; 
	for(i=0; i<2 ;i++)
	{
		for(j =0; j < MAX_NOZZLE; j++)
		{
			m_dDropCountErrorRange[i][j]=0;
			m_dDropCountErrorRange_per[i][j]=0;
			m_dDropCountErrorRange_temp[i][j]=0;
		}
	}

	// For calibration
	for(j=0 ; j<MAX_NOZZLE ;j++)
	{
		for(i=0; i<MAX_MEADATA_FORCALIB ;i++)
		{
			m_structCalibData[j].dMeasure[i][0] = 0.0;
			m_structCalibData[j].dMeasure[i][1] = 0.0;
//			m_structCalibData[j].dCoefficient[i] = 0.0; 
		}
	}
	for(j=0 ; j<MAX_NOZZLE ;j++)
	{
		for(i=0; i<6 ;i++)
		{
			m_structCalibData[j].dMeasure1[i][0] = 0.0;
			m_structCalibData[j].dMeasure2[i][1] = 0.0;
			m_structCalibData[j].dMeasure3[i][1] = 0.0;
			m_structCalibData[j].dCoefficient1[i] = 0.0; 
			m_structCalibData[j].dCoefficient2[i] = 0.0; 
			m_structCalibData[j].dCoefficient3[i] = 0.0; 
//			m_structCalibData[j].dCoefficient[i] = 0.0; 
		}
	}
	for(i = 0; i < MAX_NOZZLE; i++) 
	{
		m_nSpecCount[i] = 0;
		m_nRealSpecCount[i] = 0;
	}
	m_nSpecTime = 0;
	m_strSpecTime = "0000000000";
	m_bMovingSpecCount = FALSE;

	// Blob Parameter 를 초기화 
	Blob_Parameter.m_dObjectArea = 0;
	Blob_Parameter.m_dObjectCenterX = 0;
	Blob_Parameter.m_dObjectCenterY = 0;
	Blob_Parameter.m_dRef1CenterY = 0;
	Blob_Parameter.m_dRef2CenterY = 0;
	Blob_Parameter.m_dOffsetAdjustY = 0;	// 조정해야 할 Offset 값 
	Blob_Parameter.m_bObjectExist = false; 

	for(i = 0; i < MAX_NOZZLE; i++) 
	{
		m_dAccuracyPosX[i] = 0;
		m_dAccuracyPosY[i] = 0;
	}
	//Motor 주행 거리
	for(i = 0; i < 2; i++)
	{
		for(j = 0; j < 2; j++)
		{
			m_dMotorMoveDistance[i][j] = 0.0;
		}
	}
	//Canister 공급량
	for(i = 0; i < 4; i++)
	{
		m_dCanisterSupplyWeignt[i] = 0.0;
	}

	// teach 변수 초기화
	for( i =0 ; i <MAX_NOZZLE ; i++)
	{
		m_dDummyTeachData[i] = 0.0 ;
		m_dDummyTeachData2[i] = 0.0 ;
	}
	m_nInitSupplyTime = 10;
	m_nNormalSupplyTime = 15;  
	m_dManuCanAlarmWeight = 3.50;  

    m_bAutoManualFlag = FALSE; //090612 Auto및Manual 상태변수 초기화

	m_nCount = 0 ; //090711 ErrorInformation Data 개수 참조 변수 


	//by shin//2009.08.26//RMS C00값 보고를 위해서 위 유효드랍수를 백업한다...//
	for(i = 0; i < MAX_PATTERN; i++)
	{
		for(j = 0; j < MAX_NOZZLE; j++)
		{
			m_nRmsValidDropPoint[i][j] = 0;
		}
	}

	m_dRmsTargetWeight = 0.0;
    m_strRmsLcType = "-"; 
    m_nRmsPointNumberX = 0;
    m_nRmsPointNumberY = 0;  
    m_dRmsWidthX = 0.0; 
    m_dRmsWidthY = 0.0; 
	m_nRmsCenterDotting = 0; 

//2010.09.19 by tskim
	m_dRmsGlassSizeX = 2500.0;		// glass size X
	m_dRmsGlassSizeY = 2500.0;		// glass size Y
	m_dRmsHomeX = 0.0;			// home의 x
	m_dRmsHomeY = 0.0;			// home의 y
	m_dRmsThickness = 0.7;		// glass 두께
	m_nRmsPatternNum = 1;			// 패턴 갯수
	m_nRmsSpecialInfo = 1;
	m_nRmsSafetyLock = 1;
	m_nRmsGlassCheck = 1;	
	m_nRmsRecipeNumCompare = 1;
	m_nRmsPatternDispensing =1;
	m_nRmsNgDropping = 1;
	m_nRmsLcSupplyType =1;
	m_nRmsDummyType=1;
	m_nRmsCanisterEmgWeight=1; //090702 Canister경고량 
    m_nRmsLiquidSupplyLimit=1; //090702 액정공급Limit  
	m_nRmsAdjustInAuto=1;	// auto 구동중에 adjust
	m_nRmsDummyDropBeforeMeasInAuto=1;	// auto 구동 중에 측정전 Dummy Drop
	m_nRmsTableSpeed=100;
	m_nRmsSAxisSpeed=100;
	m_dRmsDropOffset=-1;
	m_dRmsDischargeSpeed=100;
	m_dRmsSuctionSpeed=100;
	m_dRmsAccTime=1;
	m_dRmsDecTime=1;
	m_dRmsStartSpeed=2000;
	for(i=0;i<MAX_NOZZLE;i++)
		m_dRmsNozzleOffset[i]=0;
	for(i=0;i<5;i++)
	{
		m_dRmsCenterPositionX[i] = 0.0;
		m_dRmsCenterPositionY[i] = 0.0;
	}
	m_nRmsSuctionLineNo = 1;
	m_nRmsSequence = 1;

//2015.03.15 ehji RMS ADD
	m_bRMS_LCQuanReportPlus=0;
	m_bRMS_LCQuanReportMinus=0;
	m_bRMS_AddDelChange=0;
	m_bRMS_AddDelCompare=0;
	m_bRMS_SelectDropMode=0;
	m_bRMS_InteractiveDrop=0;
	m_nRMS_GroupLineDrop=0;
	m_nRMS_CalibraionMode=1;
	m_nRMS_NzlMode=0;
//////////////////////////////////

	m_dRmsTargetWeight_R = 0.0;
    m_strRmsLcType_R = "-"; 
    m_nRmsPointNumberX_R = 0;
    m_nRmsPointNumberY_R = 0;  
    m_dRmsWidthX_R = 0.0; 
    m_dRmsWidthY_R = 0.0; 
	m_nRmsCenterDotting_R = 0; 

//2010.09.28 by tskim RMS Level3
	m_dRmsGlassSizeX_R = 2500.0;		// glass size X
	m_dRmsGlassSizeY_R = 2500.0;		// glass size Y
	m_dRmsHomeX_R = 0.0;			// home의 x
	m_dRmsHomeY_R = 0.0;			// home의 y
	m_dRmsThickness_R = 0.7;		// glass 두께
	m_nRmsPatternNum_R = 1;			// 패턴 갯수
	m_nRmsSpecialInfo_R = 1;
	m_nRmsSafetyLock_R = 1;
	m_nRmsGlassCheck_R = 1;	
	m_nRmsRecipeNumCompare_R = 1;
	m_nRmsPatternDispensing_R =1;
	m_nRmsNgDropping_R = 1;
	m_nRmsLcSupplyType_R =1;
	m_nRmsDummyType_R=1;
	m_nRmsCanisterEmgWeight_R=1; //090702 Canister경고량 
    m_nRmsLiquidSupplyLimit_R=1; //090702 액정공급Limit  
	m_nRmsAdjustInAuto_R=1;	// auto 구동중에 adjust
	m_nRmsDummyDropBeforeMeasInAuto_R=1;	// auto 구동 중에 측정전 Dummy Drop
	m_nRmsTableSpeed_R=100;
	m_nRmsSAxisSpeed_R=100;
	m_dRmsDropOffset_R=-1;
	m_dRmsDischargeSpeed_R=100;
	m_dRmsSuctionSpeed_R=100;
	m_dRmsAccTime_R=3;
	m_dRmsDecTime_R=3;
	m_dRmsStartSpeed_R=2000;
	for(i=0;i<MAX_NOZZLE;i++)
		m_dRmsNozzleOffset_R[i]=0;
	for(i=0;i<5;i++)
	{
		m_dRmsCenterPositionX_R[i] = 0.0;
		m_dRmsCenterPositionY_R[i] = 0.0;
	}
	m_nRmsSuctionLineNo_R = 1;
	m_nRmsSequence_R = 1;


	for(i = 0; i < MAX_PANEL; i++)
	{
		nRms_Center_Dotting[i] = -1;
	}
	m_nFlexiblePatNo = 0;

	m_strMachineNo = "1";
	m_strLineNo = "1";
	m_nSite = 1;

	for(i=0;i<100;i++)
	{
		m_dAlignXTrace[i] = 0.0;
		m_dAlignYTrace[i] = 0.0;
	}
	g_nGlassAlignCount = -1;
//2010.05.17 by tskim For Rms
	// Data Editor에 있는 변수들 초기화
	m_structDataEditor_Rms.m_dGlassSizeX = 2500.0;	
	m_structDataEditor_Rms.m_dGlassSizeY = 2250.0;	
	m_structDataEditor_Rms.m_dThickness = 0.70;	
	m_structDataEditor_Rms.m_nPatternNum = 3;	
	m_structDataEditor_Rms.m_dHomeX = 0.0;		
	m_structDataEditor_Rms.m_dHomeY = 0.0;		
	m_structDataEditor_Rms.m_strComment = _T("");		
	m_structDataEditor_Rms.m_nSequenceCount = 3;
	m_structDataEditor_Rms.m_nSetDispense = 999;
	m_structDataEditor_Rms.m_bPatternDispensing = TRUE;
	m_structDataEditor_Rms.m_bNgDropping = FALSE;
	m_structDataEditor_Rms.m_bRecipeNoCompare = FALSE;
	m_structDataEditor_Rms.m_bLCQuanReportPlus=FALSE;
	m_structDataEditor_Rms.m_bLCQuanReportMinus=FALSE;
	m_structDataEditor_Rms.m_bAddDelChange=FALSE;
	m_structDataEditor_Rms.m_bAddDelCompare=FALSE;
	m_structDataEditor_Rms.m_bIsTrfOnlyMode=FALSE;
	m_structDataEditor_Rms.m_nDropRepeatCnt=1;

	m_structDataEditor_Rms.m_bMarkAlignInsp = FALSE;
	m_structDataEditor_Rms.m_dMarkX[0] = 0.0;
	m_structDataEditor_Rms.m_dMarkX[1] = 0.0;
	m_structDataEditor_Rms.m_dMarkY[0] = 0.0;
	m_structDataEditor_Rms.m_dMarkY[1] = 0.0;
	m_structDataEditor_Rms.m_nMarkAlignInspInterval = 1;
	m_structDataEditor_Rms.m_nCamLightSet[0] = 255;
	m_structDataEditor_Rms.m_nCamLightSet[1] = 255;
	m_structDataEditor_Rms.m_nCamScore = 50;


	for(i = 0 ; i < MAX_NOZZLE ; i++)
	{
		m_structDataEditor_Rms.m_strLcType[i] = _T("");
		m_structDataEditor_Rms.m_strBarcode[i] = _T("AAAAAA");
	}
	m_structDataEditor_Rms.m_nLcSupplyType=0;
	m_structDataEditor_Rms.m_nDummyType=0;
	m_structDataEditor_Rms.m_bSpecialInfo = true;
	m_structDataEditor_Rms.m_bSafetyLock = true;
	m_structDataEditor_Rms.m_bGlassCheck = true;
	m_structDataEditor_Rms.m_bSelectDropMode = false;
	m_structDataEditor_Rms.m_nLinePatternNumber =1;

	// Adjusting Condition에 있는 변수들 초기화
	m_structAdjustCondition_Rms.dSpeed = 1.0;
	m_structAdjustCondition_Rms.nDelay = 30;
	m_structAdjustCondition_Rms.fDropStartPos = 10.0;
	m_structAdjustCondition_Rms.nDummyDropTime = 60;
	m_structAdjustCondition_Rms.nCheckingIntervalSheet = 5;
	m_structAdjustCondition_Rms.bAdjustInAuto = TRUE;
	m_structAdjustCondition_Rms.bDummyDropBeforeMeasInAuto=FALSE;
	m_structAdjustCondition_Rms.nAdjustTimes = 5;
	m_structAdjustCondition_Rms.nMeasureCycle = 1;

	for(i = 0;i < MAX_NOZZLE; i++)
	{
		m_structAdjustCondition_Rms.dTargetWeight[i] = 3.0;
		m_structAdjustCondition_Rms.dTargetError[i] = 0.3;
		m_structAdjustCondition_Rms.dTargetErrorAlarm[i] = 0.5;
		m_structAdjustCondition_Rms.dTargetErrorJudgePlus[i] = 1.0;
		m_structAdjustCondition_Rms.dTargetErrorJudgeMinus[i] = 1.0;
		m_structAdjustCondition_Rms.nDropCount[i] = 20;
		m_structAdjustCondition_Rms.nLoopCount[i] = 2;
		m_structAdjustCondition_Rms.nSpecIn[i]= 1;
		m_structAdjustCondition_Rms.dAbnormalTargetError[i] = 1.5;
	}
	m_structAdjustCondition_Rms.nLiquidSupplyLimit = 70; //090702 액정 공급량 Limit 설정치 초기화 
	m_structAdjustCondition_Rms.nCanisterEmgWeight=3.5; //090702 Canister 경고량 설정치 초기화 
	m_structAdjustCondition_Rms.nCheckDegasSheet = 50;
	m_structAdjustCondition_Rms.nDegasCount = 10;
/*
	// Pattern Data에 있는 변수들 초기화
	for(i = 0; i < MAX_PATTERN; i++)
	{
		m_structPatternData_Rms[i].m_dCenterPositionX = 0;
		m_structPatternData_Rms[i].m_dCenterPositionY = 0;
		m_structPatternData_Rms[i].m_dWidthX = 0;
		m_structPatternData_Rms[i].m_dWidthY = 0;
		m_structPatternData_Rms[i].m_nPointNumberX = 0;
		m_structPatternData_Rms[i].m_nPointNumberY = 0;
		m_structPatternData_Rms[i].m_nDroppingNum = 0;
		m_structPatternData_Rms[i].m_dAngleX = 0;
		m_structPatternData_Rms[i].m_dAngleY = 0;
		m_structPatternData_Rms[i].m_nDroppingStartPos = 0;
		m_structPatternData_Rms[i].m_nStartDroppingAng = 0;
		m_structPatternData_Rms[i].m_nDropDataCount = 0;
				
		m_structPatternData_Rms[i].m_nDispenseMode = 0;
		m_structPatternData_Rms[i].m_nTableSpeed = 0;
		m_structPatternData_Rms[i].m_nSAxisSpeed = 0;

		for(j = 0; j < MAX_NOZZLE; j++)
		{
			m_structPatternData_Rms[i].m_dDropWeight[j] = 0.00;
			m_structPatternData_Rms[i].m_bIsOnHead[j] = FALSE;
		}

		// Pattern Condition에 있는 일부 데이타
		m_structPatternData_Rms[i].m_nSequence = i+1;
		m_structPatternData_Rms[i].m_nSequence_R = i+1;
		for(j = 0; j < MAX_NOZZLE; j++)
		{
			m_structPatternData_Rms[i].m_dNozzleOffset[j] = 0;
			m_structPatternData_Rms[i].m_nPanelNo[j] = 0;
		}
		m_structPatternData_Rms[i].m_bMMG=FALSE;
		m_structPatternData_Rms[i].m_dBaseWeight=0.0;
		m_structPatternData_Rms[i].m_dLevelWeightDifference=0.0;	

		m_structPatternData_Rms[i].m_dDropOffset = 0.0;
		m_structPatternData_Rms[i].m_dDischargeSpeed = 0;
		m_structPatternData_Rms[i].m_dSuctionSpeed = 0;
		m_structPatternData_Rms[i].m_nSuctionMode = 0;
		m_structPatternData_Rms[i].m_dAccTime = 0;
		m_structPatternData_Rms[i].m_dDecTime = 0;
		m_structPatternData_Rms[i].m_dStartSpeed = 0;
		m_structPatternData_Rms[i].m_nSuctionLineNo = 0;
		m_structPatternData_Rms[i].LinePatNum = 0;
	}
*/
	// drop data에 있는 변수들 초기화
	// drop data는 CArray이므로 초기화 필요 없음

	// recipe name 기본값으로 설정
	m_structOperatorModeRecipeData_Rms.strFrontRecipeName = _T("FrontName");
	m_structOperatorModeRecipeData_Rms.strBackRecipeName = _T("BackName");
	
	m_structEc.m_dDmyPos_Col=0.0;
	for(i=0;i<MAX_NOZZLE;i++)
	{
		m_structEc.m_dMeasPos_S[i]=0.0;
		m_structEc.m_dDmyPos_S[i]=0.0;	
		m_structEc.m_dSupPos_S[i]=0.0;
	}
	m_structEc.m_dMeasPos_Col=0.0;
	for(i=0;i<3;i++)
	{			
		m_structEc.m_dMutePos_Col[i]=0.0;
		m_structEc.m_dSensingTime[i]=0.0;
		m_structEc.m_dTriggerTime[i]=0.0;	
	}
	m_structEc.m_dMarkPos_S1=0.0;				
	m_structEc.m_dMarkPos_S16=0.0;				
	m_structEc.m_dMarkPos_Col=0.0;			
	m_structEc.m_dCntPos_Col=0.0;				
	m_structEc.m_dOperPos_Col=0.0;				
	m_structEc.m_dLoadPos_F=0.0;				
	m_structEc.m_dLoadPos_R=0.0;				
	m_structEc.m_dSupPos_Col=0.0;				
	m_structEc.m_dSupAmount=0.0;				
	m_structEc.m_dSupTime=0.0;					
	m_structEc.m_dColSpd=0.0;
	for(i=0;i<2;i++)
		m_structEc.m_dInputLimit[i]=0.0;

//2010.09.28 by tskim Rms level3
	m_structEc_R.m_dDmyPos_Col=0.0;
	for(i=0;i<MAX_NOZZLE;i++)
	{
		m_structEc_R.m_dMeasPos_S[i]=0.0;
		m_structEc_R.m_dDmyPos_S[i]=0.0;	
		m_structEc_R.m_dSupPos_S[i]=0.0;
	}
	m_structEc_R.m_dMeasPos_Col=0.0;
	for(i=0;i<3;i++)
	{			
		m_structEc_R.m_dMutePos_Col[i]=0.0;
		m_structEc_R.m_dSensingTime[i]=0.0;
		m_structEc_R.m_dTriggerTime[i]=0.0;	
	}
	m_structEc_R.m_dMarkPos_S1=0.0;				
	m_structEc_R.m_dMarkPos_S16=0.0;				
	m_structEc_R.m_dMarkPos_Col=0.0;			
	m_structEc_R.m_dCntPos_Col=0.0;				
	m_structEc_R.m_dOperPos_Col=0.0;				
	m_structEc_R.m_dLoadPos_F=0.0;				
	m_structEc_R.m_dLoadPos_R=0.0;				
	m_structEc_R.m_dSupPos_Col=0.0;				
	m_structEc_R.m_dSupAmount=0.0;				
	m_structEc_R.m_dSupTime=0.0;					
	m_structEc_R.m_dColSpd=0.0;
	for(i=0;i<2;i++)
		m_structEc_R.m_dInputLimit[i]=0.0;

	m_nRecipecount = 0;
	m_bTeachDataSave = FALSE;
//2010.09.09 by tskim Meas Alarm
	m_bRecipeChanged = FALSE;
	m_bPumpChanged = FALSE;
	m_nRecipeEditCode = -1;

	m_strRmsRecipeNO = "";

	m_structMachineInfo.m_strMachineNo = "0";
	m_structMachineInfo.m_strLineNo = "0";
	m_structMachineInfo.m_strSite = "LGD_CA";

	m_structN_Vision.m_nScan_Cycle = 0;
	m_structN_Vision.m_nTotal_Permit_Over = 0;
	m_structN_Vision.m_nTotal_Permit_Miss = 0;
	m_structN_Vision.m_nCell_Permit_Over = 0;
	m_structN_Vision.m_nCell_Permit_Miss = 0;
	for(i = 0; i < MAX_VISION; i++)
		m_structN_Vision.m_nIntensity[i] = 0;

	m_structN_Vision.m_nIntensity[3] = 0;
	m_structN_Vision.m_nEdge_Intnsity = 0;
	for(i = 0; i < MAX_VISION; i++)
		m_structN_Vision.m_dDrop_Size_X[i] = 0.0;

	m_structN_Vision.m_dDrop_Size_X[3] = 0.0;
	for(i = 0; i < MAX_VISION; i++)
		m_structN_Vision.m_dDrop_Size_Y[i] = 0.0;

	m_structN_Vision.m_dDrop_Size_Y[3] = 0.0;
	m_structN_Vision.m_nNozzle_No = 0;
	m_structN_Vision.m_dNozzle_Pitch = 0.0;
	m_structN_Vision.m_nK_Vel = 0;
	m_structN_Vision.m_nLight_Con = 0;
	m_structN_Vision.m_dDist_Permit_X = 0.0;
	m_structN_Vision.m_dDist_Permit_Y = 0.0;
	m_structN_Vision.m_nGlass_Permit_Over = 0;
	m_structN_Vision.m_nSeal_Permit_Over = 0;
	m_structN_Vision.m_bReverse = 0;
	m_structN_Vision.m_bEdge_Reverse = 0;
	m_structN_Vision.m_dGlass_Size_X = 0.0;
	m_structN_Vision.m_dGlass_Size_Y = 0.0;
	m_structN_Vision.m_dCell_Rate = 0.0;
	m_structN_Vision.m_dEdge_Size = 0.0;
	for(i = 0; i < MAX_VISION; i++)
		m_structN_Vision.m_dDrop_Close[i] = 0.0;

	m_structN_Vision.m_dDrop_Close[3] = 0.0;
	m_structN_Vision.m_dScan_Start_Pos = 0.0;
	m_structN_Vision.m_dScan_Dist = 0.0;
	m_structN_Vision.m_nScan_Speed = 0;
	m_structN_Vision.m_nDisp_Rate = 0;
	m_structN_Vision.m_bDir_TTB = 0;
	m_structN_Vision.m_nFirst_Scan_Cycle = 0;												//ehji 150305


//160903 DUAL INTENSITY //160929 JYKIM
	for(i = 0; i < MAX_VISION; i++)
	{
		m_structN_Vision.m_nIntensityHigh[i] = 50;
		m_structN_Vision.m_dDrop_Size_High_X[i] = 1.0;
		m_structN_Vision.m_dDrop_Size_High_Y[i] = 1.0;
	}
//161219 PART SCAN
	m_structN_Vision.m_bPartScanMode  = FALSE;	//JYKIM 161219 PART SCAN 180K
	m_structN_Vision.m_nPartScanLineCount = 2;	//JYKIM 161219 PART SCAN 2줄마다 스캔 180K
//		동일 파라 ?? 

	for(i=0; i<MAX_NOZZLE; i++)
	{
		for(j = 0; j < MAX_PATTERN; j++)
		{
			m_diffTargetFlexBK[i][j]=0.0;
		}
	}

 	for(i=0; i<MAX_NOZZLE; i++)
	{
		for(j = 0; j < MAX_MEADATA_FORCALIB; j++)
		{
			m_structCalibData[i].dMeasure1[j][0]=m_structCalibData[i].dMeasure1[j][1]=0.0;
			m_structCalibData[i].dMeasure2[j][0]=m_structCalibData[i].dMeasure2[j][1]=0.0;
			m_structCalibData[i].dCoefficient1[j]=m_structCalibData[i].dCoefficient2[j]=0.0;
		}
		for(j = 0; j < MAX_MEADATA_GETCOUNT1; j++)
		{
			m_structCalibData[i].dMeasure[j][0]=m_structCalibData[i].dMeasure[j][1]=0.0;
		}
		for(j = 0; j < MAX_MEADATA_GETCOUNT2; j++)
		{
			m_structCalibData[i].dMeasureFlexible[j][0]=m_structCalibData[i].dMeasureFlexible[j][1]=0.0;
			m_structCalibData[i].dMeasureFlexible[j][2]=m_structCalibData[i].dMeasureFlexible[j][3]=0.0;
			m_structCalibData[i].dFGradOffset[j]=m_structCalibData[i].m_dFlexDropOffset[j]=0.0;
			m_structCalibData[i].dMeasureFlexibleFirst[j][0]=m_structCalibData[i].dMeasureFlexibleFirst[j][1]=0.0;
			m_structCalibData[i].dMeasureFlexibleFirst[j][2]=m_structCalibData[i].dMeasureFlexibleFirst[j][3]=0.0;
		}
		for(j = 0; j < 6; j++)
		{
			m_structCalibData[i].dCoefficient1[j] = 0.0;
			m_structCalibData[i].dMeasure3[j][0]=m_structCalibData[i].dMeasure3[j][0]=0.0;
		}
		for(j = 0; j < 100; j++)
		{
			m_structCalibData[i].m_dCompensationDist[j]=0.0;
			m_structCalibData[i].m_dDiffSuctionDist[j]=0.0;
			m_structCalibData[i].m_SectionNum[j][0]=m_structCalibData[i].m_SectionNum[j][1]=0.0;
			m_structCalibData[i].m_dRefSuctionDist[j]=0.0;
		}
		m_structCalibData[i].nCalibCountMin=0;
		m_structCalibData[i].nCalibCountBase=0;
		m_structCalibData[i].nCalibCountMax=0;
	}
	for(i=0; i<MAX_NOZZLE; i++)
	{
		for(j = 0; j < MAX_PATTERN; j++)
		{
			m_diffTargetFlexBK[i][j]=0.0;
		}
	}

	for(i=0; i<MAX_NOZZLE; i++)
	{
		m_dVision_Y_Offset[i] = 0.0;
		m_dVision_Y_Line_Offset[i] = 0.0; 
		m_dVision_X_Offset[i] = 0.0; 
	}

	m_nMMGDropcount = 0; 
	m_nMainDropCount = 0; 	
	m_nDropcount = 0;
	nTempMainCount = 0; 

	for(i=0; i<MAX_NOZZLE; i++)
	{
		m_strRFDataResidualData[i] = "";
		m_strRFDataBalanceNo[i] = "";
		m_strRFDataHeadName[i] = "";
		m_strRFDataLOTID[i] = "";
		m_strRFDataBarcodeID[i] = "";
		m_strRFDataLOTID_Full[i] = "";
	}

//2014.05.07 by tskim
	for( i = 0 ; i < MAX_PANEL ; i++)
	{
		for(j=0;j<2;j++)
			m_structVision.m_nPanelMissShot[i][j] = -1;
	}

	m_bDropDetectRealTimeMode = FALSE; 

	for ( i = 0; i < MAX_NOZZLE; i++ )
	{
		m_nMMGpatStatus[i] = 0; // 141015 BY SHLEE // 0: BLANC , 1 : MAIN , 2 : MMG 
	}

//2014.10,20 by tskim LC 소모량 
	for ( i = 0; i < 6; i++ )
	{
		for( j = 0; j < MAX_NOZZLE; j++ )
		{
			m_dLC_Consumption[i][j] = 0.0;//0:Dummy, 1: Supply, 2: Degae, 3,4,5:Spare
			m_strLC_Consumption[i][j] = "------";//0:Dummy, 1: Supply, 2: Degae, 3,4,5:Spare
		}
	}
//2014.11.27 by tskim RMS Add
/*****************************************************************************************************************/
	m_Rms_dMarginRate = 0.0;
	m_Rms_dCalibrationInterlock = 0.0; 
	m_Rms_bMMGLineDropMode = FALSE;
	m_Rms_nCheckDegasSheet = 0;
	m_Rms_structVision.nTotal_Over = 0;
	m_Rms_structVision.nTotal_Miss = 0;
	m_Rms_structVision.nCell_Over = 0;
	m_Rms_structVision.nCell_Miss = 0;
	m_Rms_structVision.dScanX = 0.0;
	m_Rms_structVision.dScanY = 0.0;
	m_Rms_structVision.nIntensity[0]=m_Rms_structVision.nIntensity[1]=m_Rms_structVision.nIntensity[2] = 0;
	m_Rms_structVision.dGlass_Intensity = 0;
	m_Rms_structVision.dDrop_Size[0]=m_Rms_structVision.dDrop_Size[1]=0;
	m_Rms_structVision.nNozzle_No = 0;
	m_Rms_structVision.dNozzle_Pitch = 0.0;
	m_Rms_structVision.dK_Speed = 0.0;
	m_Rms_structVision.bReverse = FALSE;
	m_Rms_structVision.bGlass_Reverse = FALSE;
	m_Rms_structVision.nGlass_Over = 0;
	m_Rms_structVision.nSeal_Over = 0;
	m_Rms_structVision.nLight = 0;
	m_Rms_structVision.dGlass_Size[0]=m_Rms_structVision.dGlass_Size[1]=0;
	m_Rms_structVision.dCell_Rate = 0.0;
	m_Rms_structVision.dGlass_Area = 0.0;
	m_Rms_structVision.dScan_Start = 0.0;
	m_Rms_structVision.dScan_End = 0.0;
	for ( i = 0; i < MAX_NOZZLE ; i++ )
	{
		m_Rms_structVision.dXPosOffset[i] = 0.0; 
		m_Rms_structVision.dYPosOffset[i] = 0.0; 
	}
	m_Rms_structVision.nScanCycle = 0; 
	m_Rms_structVision.nFirstScanCycle = 0; 
	m_Rms_structVision.nDummy_Distance = 0;
	m_Rms_structVision.dDummy_Min_Size[0]=m_Rms_structVision.dDummy_Min_Size[1]=0;
	m_Rms_structVision.dDummy_Max_Size[0]=m_Rms_structVision.dDummy_Max_Size[1]=0; 
	m_Rms_structVision.nDummy_Intensity[0]=m_Rms_structVision.nDummy_Intensity[1]=m_Rms_structVision.nDummy_Intensity[2]=0;
	m_Rms_bUse_Vision = FALSE; 
 	for(i=0;i<MAX_PATTERN_FOR_RMS;i++)
 	{
		 m_Rms_structPatternData[i].m_nNShot = 0; 
 		for ( j = 0; j < MAX_NOZZLE ; j++ )
 		{ 
 			m_Rms_structPatternData[i].m_nNRunLimit[j]=0;
 			m_Rms_structPatternData[i].m_nNAmplitude[j]=0;
 			m_Rms_structPatternData[i].m_dNDuty[j]=0.0;
 			m_Rms_structPatternData[i].m_nNFirstVoltage[j]=0;
 			m_Rms_structPatternData[i].m_nNNegOffset[j]=0;
 			m_Rms_structPatternData[i].m_nNFallingEdge[j]=0;
 		}
 	}
	for(i = 0; i < MAX_PATTERN_FOR_RMS; i++)//ETC
	{
		m_Rms_structPatternData[i].m_dCenterPositionX = 0;
		m_Rms_structPatternData[i].m_dCenterPositionY = 0;
		m_Rms_structPatternData[i].m_dWidthX = 0;
		m_Rms_structPatternData[i].m_dWidthY = 0;
		m_Rms_structPatternData[i].m_nPointNumberX = 0;
		m_Rms_structPatternData[i].m_nPointNumberY = 0;
		m_Rms_structPatternData[i].m_nDroppingNum = 0;
		m_Rms_structPatternData[i].m_dAngleX = 0;
		m_Rms_structPatternData[i].m_dAngleY = 0;
		m_Rms_structPatternData[i].m_nDroppingStartPos = 0;
		m_Rms_structPatternData[i].m_nStartDroppingAng = 0;
		m_Rms_structPatternData[i].m_nDropDataCount = 0;
				
		m_Rms_structPatternData[i].m_nDispenseMode = 0;
		m_Rms_structPatternData[i].m_nTableSpeed = 0;
		m_Rms_structPatternData[i].m_nSAxisSpeed = 0;

		for(j = 0; j < MAX_NOZZLE; j++)
		{
			m_Rms_structPatternData[i].m_dDropWeight[j] = 0.00;
			m_Rms_structPatternData[i].m_bIsOnHead[j] = FALSE;
		}

		// Pattern Condition에 있는 일부 데이타
		m_Rms_structPatternData[i].m_nSequence = 0;
		m_Rms_structPatternData[i].m_nSequence_R = 0;

		for(j = 0; j < MAX_NOZZLE; j++)
		{
			m_Rms_structPatternData[i].m_dNozzleOffset[j] = 0.0;
			m_Rms_structPatternData[i].m_nPanelNo[j] = 0;
			//m_nDropCount[i][j] = 0 ; 
			//m_nTotalCount[i][j] = 0 ; 
			//m_nFlexibleDropCount[j] = 0;
		}
		m_Rms_structPatternData[i].m_bMMG=FALSE;
		m_Rms_structPatternData[i].m_dBaseWeight=0;
		m_Rms_structPatternData[i].m_dLevelWeightDifference=0;	

		m_Rms_structPatternData[i].m_dDropOffset = 0;
		m_Rms_structPatternData[i].m_dDischargeSpeed = 0;
		m_Rms_structPatternData[i].m_dSuctionSpeed = 0;
		m_Rms_structPatternData[i].m_nSuctionMode = 0;
		m_Rms_structPatternData[i].m_dAccTime = 0;
		m_Rms_structPatternData[i].m_dDecTime = 0;
		m_Rms_structPatternData[i].m_dStartSpeed = 0;
		m_Rms_structPatternData[i].m_nSuctionLineNo = 0;
		m_Rms_structPatternData[i].LinePatNum = 0;

		m_Rms_structPatternData[i].m_nNShot = 0;
		m_Rms_structPatternData[i].m_dNHz = 0.0;
		m_Rms_structPatternData[i].m_dNShotActionTime = 0.0;
		m_Rms_structPatternData[i].m_dNShotPitch = 0.0;
	}

/*****************************************************************************************************************/

//2014.11.27 by tskim Measinterval
	m_structAdjustCondition.m_n1stMeas = 1;
	m_structAdjustCondition.m_n2ndMeas = 5;

	// 150527 ehji
	m_structAdjustCondition.m_dDistperdropEmergency = 1.5;
	
//2015.01.24 by tskim Section Calibration
 	for(i=0; i<MAX_NOZZLE; i++)
	{
		for(j = 0; j < MAX_MEADATA_GETCOUNT3; j++)
		{
			for(int k =0 ;k<4;k++)
			{
				m_structCalibData[i].dMeasureSection[j][k] = m_structCalibData[i].dMeasureFirstSection[j][k] = 0.0;
			}
		}
	}

//2015.02.10 by tskim Motorload
	for(i =0 ; i <4 ; i++)
		m_structMotorload.m_nMotorLoadK[i]=0;		
	for(i =0 ; i <MAX_NOZZLE ; i++)
		m_structMotorload.m_nMotorLoadS[i]=0;
	for(i =0 ; i <MAX_NOZZLE ; i++)
		m_structMotorload.m_nMotorLoadZ[i]=0;		
	m_structMotorload.m_nMotorLoadTH=0;

//2015.07.08 by tskim
	for ( i = 0; i < MAX_NOZZLE ; i++ )
	{
		m_structAdjustCondition.m_dPulseLimit_P[i]=m_structAdjustCondition.m_dPulseLimit_M[i]=0.0;
		m_lDropCount[i] = 0;
	}

//220603 BIG DROP DETECT ( (OLD) DEEP DROP ) 
	m_structN_Vision.m_nDetectBigDrop_Rate[0] = 0;
	m_structN_Vision.m_nDetectBigDrop_Rate[1] = 0;
	m_structN_Vision.m_nDetectBigDrop_Rate[2] = 0;
	m_structN_Vision.m_nDropSizeRate_X  = 0;
	m_structN_Vision.m_nDropSizeRate_Y  = 0;
//220603 BIG DROP DETECT ( (OLD) DEEP DROP ) 
}

CP8CA_LcDispDoc::~CP8CA_LcDispDoc()
{
	delete [] m_arDropData;
	delete [] m_arLineData;
	delete [] m_arLineDataBk;
	delete [] m_arLineDataPat;
	delete [] m_arDropData_R;
//2010.06.21 by tskim
	delete [] m_arDropDataBk;	

//121210
	delete [] m_arMMGLineData; 
	delete [] m_arMainLineData; 
	delete [] m_arMMGLineDataBk;
	delete [] m_arMainLineDataBk;
	delete [] m_arMMGLineDataTemp;
	delete [] m_arMainLineDataTemp;
	delete [] m_arMMGLineData_R;
	delete [] m_arMainLineData_R;

	delete [] m_arMainMMGYLineData; 
	delete [] m_arMainMMGYLineData_R; 
//////////121210 추가 사항 
	delete [] m_arMMGLineDataPat; 
	delete [] m_arMainLineDataPat; 
	delete [] m_arMainMMGYLineDataPat;
///////////
}

//extern BOOL gb3LineOpen; // by ckh 2009.02.28 

BOOL CP8CA_LcDispDoc::OnNewDocument()   // Return 값이 false 이면 응용 프로그램이 정상 실행될 수 없다.
{
	CString str;

	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	// by ckh 2009.02.28
	CString m_strMachineNo;
	m_strMachineNo.Format("%2d",AfxGetApp()->GetProfileInt("","MACHINE",3)); 
	
	// by ckh 2009.02.28  라인별 구분하기.
//	gb3LineOpen = FALSE;// 초기는 1,2 라인으로 설정

//	if(atoi(m_strMachineNo)%7 > 2) // 3라인을 빠른 번호로 설정(3라인 - 1,2 2라인 - 3,4, 1라인 - 5,6)
//	{
//		gb3LineOpen = FALSE;
//	}
//	else
//	{
//		gb3LineOpen = TRUE; 
//	}
	//
	// 프로그램 시작시 등록된 데이타 폴더에 있는 모든 recipe data를 찾아서 등록한다.
	BOOL bStart;
	bStart = FindRecipeData();  // 각종 File들을 Loading 한다.
	// 프로그램 시작 가능이면 recipe data중 마지막 작업용 Engineer mode파일을 읽어서
	// 변수에 보관한다.
	if(bStart == TRUE) 
	{
		m_bIsEngineerMode = FALSE; 
//		str.Format("%s",m_structOperatorModeRecipeData.strFrontRecipeName);
//		ReadRecipeFile(str);
		ReadRecipeFile(m_structOperatorModeRecipeData.strFrontRecipeName);
//		ReadCalSetFile(str);	
		ReadCalSetFile(m_structOperatorModeRecipeData.strFrontRecipeName); // Teaching Data를 읽어들인다.

	}
	else {
		return FALSE;   // 자동 종료
	}
	
	Read_AutoErrListInformData();  //090711 최초 구동시 Auto EList Data Setting 을 위해 추가 

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CP8CA_LcDispDoc serialization

void CP8CA_LcDispDoc::Serialize(CArchive& ar)
{
	int i , j;

	if(ar.IsStoring())
	{
		// 저장
		// m_nSelFile에 의해서 분류
		switch(m_nSelFile)
		{
		case 0: // teaching
			for(i=0; i<40; i++) {
				for(j=0; j<3 ;j++) {
					ar << m_dTeachData[i][j];
					g_dTeachData[i][j] = m_dTeachData[i][j]; 
				}
			}
			break;
		case 1:
			for(i=0; i<5; i++) {
				for(j=0; j<2 ;j++) {
					ar << m_dCounter[i][j];
				}
			}
			break;
		case 2: // offset
//2015.04.29 by tskim 원재료 전산화Add LC잔량 관련 		
// 			for(i=0; i<2; i++) { // row
// 				for(j=0; j<MAX_NOZZLE ;j++) { // column
// 					ar << m_dOffset[i][j];
// 				}
// 			}
			break;
		case 3: // common // cup의 현재 량 // 경고량..
			for(i=0; i<2; i++) { // row
				for(j=0; j<MAX_BAL ;j++) { // column
					ar << m_dCommon[i][j];
				}
			}
			break;

		case 4: 
			// Pump 번호
//			for(i=0; i<MAX_NOZZLE; i++) 
//			{	ar << m_strPumpNo[i];	}
			// 액정 Lot No.
			for(i=0; i<MAX_NOZZLE; i++) 
			{	ar << m_strLCLotNo[i];	}
			// Drop Count Sensor 사용/미사용
			for(i=0; i<MAX_NOZZLE; i++)
			{	ar << m_bDropCountSensorUSE[i];	}
			// Drop Count Range 값
			for(i=0; i<2; i++)
			{
				for(j=0; j< MAX_NOZZLE; j++)
				{
					ar << m_dDropCountErrorRange[i][j];
				}
			}
			// LC 잔량 감지 후 작업 가능 Glass 수
			ar << m_dSetLCDetectedCount;
			for(i=0; i<MAX_NOZZLE; i++)
			{	ar << m_bRemainSensorUSE[i];	}//잔량감지센서사용유무yamary...
			// Pump 번호
			for(i=0; i<MAX_NOZZLE; i++) 
			{	ar << m_strPumpNo[i];	}
			for(i=0; i<MAX_NOZZLE; i++)
			{	ar << m_bLowDetectSensorUSE[i];	}//하한감지센서사용유무 2010.06.07 by tskim
			for(i=0; i<2; i++)
			{	ar << m_dLCAmountDiff[i];	}//LC양 차이 비교  2010.06.07 by tskim
			for(i=0; i<2; i++) 
			{	ar << m_strLCLotNo_Canister[i];	}//Canister Lot No 추가..2010.06.07 by tskim

			//ar << m_bDropDetectRealTimeMode; // 맺힘 실시간 감지 Mode
			break;

		case 5:
			for(i=0;i<25;i++) // 알람
				ar << CIM_Information.Panel_NG_MarkSet[i];
			break;

		case 6:
			for(i=0; i<2; i++) 
			{
				for(j=0; j<4 ;j++)	
				{
					ar << m_dCanister[i][j];
				}				
			}
            ar << m_nCanisterBalNo[0];
            ar << m_nCanisterBalNo[1];
			break;

		case 7:
			for(i=0; i<2; i++) 
			{
				for(j=0; j<MAX_NOZZLE ;j++)	
				{
					ar << m_dDummyCupWeight[i][j];
				}
			}
			break;

		} //end of Switch
	}
	else
	{		
		// 불러오기
		//경호씨 프로그램 실행 후 풀기...//
		switch(m_nSelFile)
		{
		case 0: // teaching
			for(i=0; i<40; i++) {
				for(j=0; j<3 ;j++) {
					ar >> m_dTeachData[i][j];
				}
			}
			break;
		case 1:
			for(i=0; i<5; i++) {
				for(j=0; j<2 ;j++) {
					ar >> m_dCounter[i][j];
				}
			}
			break;
		case 2: // offset
//2015.04.29 by tskim 원재료 전산화Add LC잔량 관련 		
// 			for(i=0; i<2; i++) { // row
// 				for(j=0; j<MAX_NOZZLE ;j++) { // column
// 					ar >> m_dOffset[i][j];
// 				}
// 			}		
			break;
			
		case 3: // common
			for(i=0; i<2; i++) { // row
				for(j=0; j<MAX_BAL ;j++) { // column
					ar >> m_dCommon[i][j];
				}
			}
			break;
			
		case 4: 
			// Pump 번호
//			for(i=0; i<MAX_NOZZLE; i++) 
//			{	ar >> m_strPumpNo[i];}
			// 액정 Lot No.
			for(i=0; i<MAX_NOZZLE; i++) 
			{	ar >> m_strLCLotNo[i];}
			
			for(i=0; i<MAX_NOZZLE; i++)
			{	ar >> m_bDropCountSensorUSE[i];}

			for(i=0; i<2; i++)
			{	
				for(j=0; j< MAX_NOZZLE; j++)
				{
					ar >> m_dDropCountErrorRange[i][j];
				}
			}
		
			// LC 잔량 감지 후 작업 가능 Glass 수
			ar >> m_dSetLCDetectedCount;
			for(i=0; i<MAX_NOZZLE; i++)
			{	ar >> m_bRemainSensorUSE[i];} // 잔량감지센서사용유무yamary...
			// Pump 번호
			for(i=0; i<MAX_NOZZLE; i++) 
			{	ar >> m_strPumpNo[i];}
			// 하한 감지 센서 사용 유무 2010.06.07 by tskim
			for(i=0; i<MAX_NOZZLE; i++) 
			{	ar >> m_bLowDetectSensorUSE[i];}
			for(i=0; i<2; i++)
			{	ar >> m_dLCAmountDiff[i];	}//LC양 차이 비교  2010.06.07 by tskim
			for(i=0; i<2; i++) 
			{	ar >> m_strLCLotNo_Canister[i];}//Canister Lot No 추가..2010.06.07 by tskim

			//ar << m_bDropDetectRealTimeMode; // 맺힘 실시간 감지 Mode
			break;
			
		case 5:
			for(i=0; i<25; i++)
				ar >> CIM_Information.Panel_NG_MarkSet[i];
			break;

		case 6:
			for(i=0; i<2; i++) 
			{
				for(j=0; j<4 ;j++)
				{
					ar >> m_dCanister[i][j];
				}
			}
            ar >> m_nCanisterBalNo[0];
            ar >> m_nCanisterBalNo[1];
			break;

		case 7:
			for(i=0; i<2; i++) 
			{
				for(j=0; j<MAX_NOZZLE ;j++)
				{
					ar >> m_dDummyCupWeight[i][j];
				}
			}
			break;
			
		} // end of switch		
	} // end of else
	
} // end of Serialize

/////////////////////////////////////////////////////////////////////////////
// CP8CA_LcDispDoc diagnostics

#ifdef _DEBUG
void CP8CA_LcDispDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CP8CA_LcDispDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CP8CA_LcDispDoc commands

// 프로그램 처음 실행시 RecipeData를 찾아서 변수에 등록한다.
// recipedata가 하나도 없으면 FALSE 리턴
BOOL CP8CA_LcDispDoc::FindRecipeData()
{
	BOOL bContinue;
	CFileFind fileFind;
	CString strPath;
	CString strDirName;
	int temp = 0;
	int i = 0;
	// 변수 초기화
	for(i=0; i<MAX_RECIPE; i++)
		m_bRecipeExist[i]=FALSE;


	// 변수 초기화
	strPath.Format("%s\\*.*", m_strDataPath);
	bContinue = TRUE;

	if(!(bContinue == fileFind.FindFile(strPath)))
	{
		CString str;
		str.Format("%s 폴더를 찾을 수 없습니다!! 실행불가", strPath);
		AfxMessageBox(str);
		exit(-1);
	}

	// 지정된 디렉토리에서 파일을 차레로 읽어옴
	m_nRecipecount = 0;//2010.08.06 by tskim
	while(bContinue)
	{
		bContinue = fileFind.FindNextFile();
		// 디렉토리이면 recipe data 폴더로 우선은 인정
		// 그리고 그 폴더가 유효 폴더인지 검사하는 함수에서 유효폴더 여부를 검사함
		if(fileFind.IsDirectory())
		{
			strDirName = fileFind.GetFileName();
			// "."이나 ".."가 아니면 recipe 데이타에 추가
			if((strDirName != ".") && (strDirName != ".."))
			{
				temp = atoi(strDirName);
				m_bRecipeExist[temp-1] = TRUE;
//2010.05.17 by tskim Rms
				m_nRecipecount++;
				// 찾은 폴더에서 recipe 관련 데이타를 찾아서 읽는다.
				ReadRecipeFolder(strDirName);
			} // end of if((strDirName != ".") && (strDirName != ".."))
		} // end of if(fileFind.IsDirectory())
	} // end of while(bContinue)
	fileFind.Close();


	// recipe data가 하나도 없으면 프로그램 실행 불가
	// LastRecipe.dat파일을 읽어서 작업용파일로 등록
	if(m_arRecipeData.GetSize() == 0)
	{
		AfxMessageBox("Recipe Data가 없어서 프로그램 실행이 불가능합니다");
		return FALSE;
	}
	// 현찬 ... 
	ifstream fi;
	char ch[256];
	RECIPE_DATA rd;

	BOOL bIsFind;		// operator mode 유효성 여부
	bIsFind = FALSE;
	
	
	strPath.Format("%s\\LastRecipe.dat", m_strDataPath);	//마지막으로 열었던 Recipe가 어떤 것인지를 저장하고 있음.
	fi.open(strPath, ios::in);

	// 화일 열기
	if(!fi.is_open())
	{
		AfxMessageBox("LastRecipe.dat파일이 없습니다");
		return FALSE;
	}

	// recipe name 읽어오기
	fi >> m_bIsEngineerMode;
	fi >> ch;
	m_structOperatorModeRecipeData.strFrontRecipeName.Format("%s", ch);
//2010.02.10 by tskim
	fi >> ch;
	m_structMachineInfo.m_strMachineNo.Format("%s",ch);
	fi >> ch;
	m_structMachineInfo.m_strLineNo.Format("%s",ch);
	fi >> ch;//Site구분 
	m_structMachineInfo.m_strSite.Format("%s",ch);

	// 읽어온 데이타가 유효한지 검사해서, 기본적인 정보를 대입하고
	// 유효하지 않으면 첫번째 recipe folder를 현재 작업용으로 설정한다.
	for(i = 0; i < m_arRecipeData.GetSize(); i++)	// m_arRecipeData.GetSize() : Recipe 개수 (폴더가 6개 존재하면 6)
	{
	
		rd = m_arRecipeData.GetAt(i);

		// 같은게 있으면 
		if((bIsFind == FALSE) && rd.strFrontRecipeName == m_structOperatorModeRecipeData.strFrontRecipeName)
		{
			m_structOperatorModeRecipeData.strBackRecipeName = rd.strBackRecipeName;
			m_structOperatorModeRecipeData.dRecipeSizeX = rd.dRecipeSizeX;
			m_structOperatorModeRecipeData.dRecipeSizeY = rd.dRecipeSizeY;
			bIsFind = TRUE;
		}
	}

	// back up..무슨 말인지 모르겠다...현찬.
	CString strCopy;
	
	if(bIsFind == FALSE)
	{
		strCopy.Format("최근 Recipe인 %s폴더가 없습니다. BACKUP폴더 적용!", m_structOperatorModeRecipeData.strFrontRecipeName);
		AfxMessageBox(strCopy);
		// 폴더 생성
		strCopy.Format("mkdir %s\\%s", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
		system((char *)(LPCSTR)strCopy);
		
		// 생성한recipe folder에, 읽었던 마지막 recipe folder의 내용을 모두 복사
		strCopy.Format("copy %s\\%s\\*.* %s\\%s\\*.*", m_strDataPath, rd.strFrontRecipeName, m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
		system((char *)(LPCSTR)strCopy);
	}
	// 화일 닫기
	fi.close();	

	// Head config를 위한 변수를 읽어들인다.
	strPath.Format("%s\\HeadConfig.dat", m_strDataPath);
	fi.open(strPath, ios::in);

	if(fi == NULL)
	{
		AfxMessageBox("HeadConfig.dat파일이 없습니다");
		return FALSE;
	}

	for( i = 0; i < 6 ; i++)	
		fi >> ch; // display용..제목 

	for( i = 0; i < MAX_NOZZLE ; i++)
	{
		fi >> m_structHeadConfig[i].nA[0]; // bd_no  //P Axis
		fi >> m_structHeadConfig[i].nA[1]; // axis_no
		fi >> m_structHeadConfig[i].nD[0]; //G Axis
		fi >> m_structHeadConfig[i].nD[1];
		fi >> m_structHeadConfig[i].nS[0]; //S Axis
		fi >> m_structHeadConfig[i].nS[1];
	}
	fi.close();	

	// clb / LcType data part..
	/*
	if(ReadBarcodeData()==FALSE){
		AfxMessageBox("Barcode.dat파일이 없습니다"); 
		return FALSE;
	}
	*/
	return TRUE;
}

// recipe 폴더에서 기본적인 recipe 정보를 읽는다.
BOOL CP8CA_LcDispDoc::ReadRecipeFolder(CString strPath)
{
	ifstream fi;
	CString strPathName;
	
	strPathName.Format("%s\\%s\\DataEditorMain.dat", m_strDataPath, strPath);
	
	RECIPE_DATA rd;	
	
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);
	if(fi.is_open())
	{
		char back[256];
		double dx;
		double dy;

		fi >> back;
		fi >> dx;
		fi >> dy;

		rd.dRecipeSizeX = dx;
		rd.dRecipeSizeY = dy;
		rd.strBackRecipeName.Format("%s", back);
		rd.strFrontRecipeName = strPath;

		m_arRecipeData.Add(rd);
	}
	else
	{
		return FALSE;
	}
	fi.close();

	return TRUE;
}
// 해당되는 RecipeName의 폴더의 기반 이 되는 모든 파일을 읽어서 변수에 저장함
BOOL CP8CA_LcDispDoc::ReadRecipeFile(CString strRecipeName)
{
	ifstream fi;
	CString strPathName;
	char ch[256];
	int i=0 , j=0;

	// Data Editor 화일 읽기 //////////////////////////////////////////////////////
	// 화일명 설정
	strPathName.Format("%s\\%s\\DataEditorMain.dat", m_strDataPath, strRecipeName);
	
	// 화일 열기
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);

	// 화일 읽기
	if(fi.is_open())
	{
		fi >> ch;
			m_structOperatorModeRecipeData.strBackRecipeName.Format("%s", ch);

		fi >> m_structDataEditor.m_dGlassSizeX;
		fi >> m_structDataEditor.m_dGlassSizeY;
		fi >> m_structDataEditor.m_dThickness;
		fi >> m_structDataEditor.m_nPatternNum;
		fi >> m_structDataEditor.m_dHomeX;
		fi >> m_structDataEditor.m_dHomeY;
		fi >> ch;
		m_structDataEditor.m_strComment.Format("%s", ch);
		fi >> m_structDataEditor.m_nSequenceCount;
		fi >> m_structDataEditor.m_nSetDispense;
		
		fi >> m_structDataEditor.m_bRecipeNoCompare;
		fi >> m_structDataEditor.m_bPatternDispensing;
		fi >> m_structDataEditor.m_bNgDropping;
		fi >> m_structDataEditor.m_bLCQuanReportPlus;
		fi >> m_structDataEditor.m_bLCQuanReportMinus;

		for(int i=0;i<MAX_NOZZLE;i++)
		{
			fi >> ch;
			m_structDataEditor.m_strLcType[i].Format("%s", ch);
		}	
		fi >> m_structDataEditor.m_bIsTrfOnlyMode;
		fi >> m_structDataEditor.m_nDropRepeatCnt;
		for(i=0;i<MAX_NOZZLE;i++)
		{
			fi >> ch;
			m_structDataEditor.m_strBarcode[i].Format("%s", ch);
		}	
		fi >> m_structDataEditor.m_bAddDelChange;
		fi >> m_structDataEditor.m_bAddDelCompare;
		fi >> m_structDataEditor.m_nLcSupplyType;
		fi >> m_structDataEditor.m_nDummyType;
		fi >> m_structDataEditor.m_bSpecialInfo;
		fi >> m_structDataEditor.m_bSafetyLock;
		fi >> m_structDataEditor.m_bGlassCheck;
		fi >> m_structDataEditor.m_bSelectDropMode;
		fi >> m_structDataEditor.m_nLinePatternNumber;
		fi >> m_structDataEditor.m_bMarkAlignInsp;
		fi >> m_structDataEditor.m_dMarkX[0];
		fi >> m_structDataEditor.m_dMarkX[1];
		fi >> m_structDataEditor.m_dMarkY[0];
		fi >> m_structDataEditor.m_dMarkY[1];
		fi >> m_structDataEditor.m_nMarkAlignInspInterval;
		fi >> m_structDataEditor.m_nCamLightSet[0];
		fi >> m_structDataEditor.m_nCamLightSet[1];
		fi >> m_structDataEditor.m_nCamScore;
		fi >> m_structDataEditor.m_bInteractiveDrop;
		fi >> m_structDataEditor.m_nCalibrationMode;
		fi >> m_structDataEditor.m_nNzlMode;

		fi >> m_structDataEditor.m_bUse_Vision;
		fi >> m_structDataEditor.m_bMove_Unload;

		fi >> m_structDataEditor.m_bMMGLineDropMode;

		for(i=0;i<MAX_NOZZLE;i++)
			fi >> m_structDataEditor.m_nLineDropNumber[i];
		for(i=0;i<MAX_NOZZLE;i++)
			fi >> m_structDataEditor.m_nBlockNumber[i];
		for(i=0;i<MAX_NOZZLE;i++)
			fi >> m_structDataEditor.m_dSuctionVolumn[i];
		fi >> m_structDataEditor.m_nGroupLineDrop;//2014.03.24 by tskim
		for(i=0;i<MAX_NOZZLE;i++)
			fi >> m_structDataEditor.m_nLineDropBlockNum[i];//2014.03.25 by tskim

		fi >> m_structDataEditor.m_bWorkMove;		//ehji 141125
		fi >> m_structDataEditor.m_Drop_Limit;		//20160421

//160903 SAMPLING SCAN //160929 JYKIM
		fi >> m_structDataEditor.m_bSamplingScan; 
//160903 DUAL INTENSITY //160929 JYKIM
		fi >> m_structDataEditor.m_bDualIntensity; 
	}
	else
	{
		return FALSE;
	}

	m_structDataEditor.m_nFlexCalibrationMaxBlockNum = 1;
	for(i=0;i<MAX_NOZZLE;i++)
	{
		if(m_structDataEditor.m_nFlexCalibrationMaxBlockNum<=m_structDataEditor.m_nBlockNumber[i])
		{
			m_structDataEditor.m_nFlexCalibrationMaxBlockNum=m_structDataEditor.m_nBlockNumber[i];
		}
		if(m_structDataEditor.m_nMaxLineDropBlockNum<=m_structDataEditor.m_nLineDropBlockNum[i])
		{
			m_structDataEditor.m_nMaxLineDropBlockNum=m_structDataEditor.m_nLineDropBlockNum[i];
		}
	}

	// 왠지 꺼림직한거 마무리 점검
	if(m_structDataEditor.m_nPatternNum < 1) m_structDataEditor.m_nPatternNum = 1;

	
//2015.03.15 ehji RMS ADD
	m_bRMS_LCQuanReportPlus = m_structDataEditor.m_bLCQuanReportPlus;
	m_bRMS_LCQuanReportMinus = m_structDataEditor.m_bLCQuanReportMinus;
	m_bRMS_AddDelChange = m_structDataEditor.m_bAddDelChange;
	m_bRMS_AddDelCompare = m_structDataEditor.m_bAddDelCompare;
	m_bRMS_SelectDropMode = m_structDataEditor.m_bSelectDropMode;
	m_bRMS_InteractiveDrop = m_structDataEditor.m_bInteractiveDrop;
	m_nRMS_GroupLineDrop = m_structDataEditor.m_nGroupLineDrop;
	m_nRMS_CalibraionMode = m_structDataEditor.m_nCalibrationMode;
	m_nRMS_NzlMode = m_structDataEditor.m_nNzlMode;


	// 화일 닫기
	fi.close();
	/////////////////////////////////////////////////////////////////////////////////

	// AdjustingCondition 화일 읽기 //////////////////////////////////////////////////////
	// 화일명 설정
	strPathName.Format("%s\\%s\\AdjustingCondition.dat", m_strDataPath, strRecipeName);
	
	// 화일 열기
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);

	// 화일 읽기
	if(fi.is_open())
	{
		for(int i = 0; i < MAX_NOZZLE; i++)
			fi >> m_structAdjustCondition.dTargetWeight[i];
		for(i = 0; i < MAX_NOZZLE; i++)
			fi >> m_structAdjustCondition.dTargetError[i];
		for(i = 0; i < MAX_NOZZLE; i++)
			fi >> m_structAdjustCondition.dTargetErrorAlarm[i];
		for(i = 0; i < MAX_NOZZLE; i++)
			fi >> m_structAdjustCondition.dTargetErrorJudgePlus[i];
		for(i = 0; i < MAX_NOZZLE; i++)
			fi >> m_structAdjustCondition.dTargetErrorJudgeMinus[i];
		for(i = 0; i < MAX_NOZZLE; i++)
			fi >> m_structAdjustCondition.nDropCount[i];
		for(i = 0; i < MAX_NOZZLE; i++)
			fi >> m_structAdjustCondition.nLoopCount[i];
		//
		fi >> m_structAdjustCondition.nDummyDropTime;
		fi >> m_structAdjustCondition.nDummyDropCount;
		fi >> m_structAdjustCondition.nCheckingIntervalSheet;
		fi >> m_structAdjustCondition.bAdjustInAuto;
		fi >> m_structAdjustCondition.bDummyDropBeforeMeasInAuto;
		fi >> m_structAdjustCondition.nAdjustTimes;
		fi >> m_structAdjustCondition.dSpeed;
		fi >> m_structAdjustCondition.nDelay;
		fi >> m_structAdjustCondition.fDropStartPos;
		fi >> m_structAdjustCondition.nMeasureCycle;
		for(i = 0; i < MAX_NOZZLE; i++)
			fi >> m_structAdjustCondition.nSpecIn[i];

		fi >> m_structAdjustCondition.nCanisterEmgWeight; //090702 Canister 경고량 Value 읽어옴
		fi >> m_structAdjustCondition.nLiquidSupplyLimit; //090702 액정 공급량 Limit Value 읽어옴
		fi >> m_structAdjustCondition.nCheckDegasSheet;
		fi >> m_structAdjustCondition.nDegasCount;
//2011.04.27 by tskim
		for(i = 0; i < MAX_NOZZLE; i++)
			fi >> m_structAdjustCondition.dAbnormalTargetError[i];
		fi >> m_structAdjustCondition.dAdjustPulse;
//2013.08.22 by tskim 초기 AdjustPulse값은 일단 사용 Head의 Target값으로 자동 계산 하게 한다..
//아래 Pattern 정보 Read후 사용 헤드 정보를 바탕으로 계산한다...
		fi >> m_structAdjustCondition.dMarginRate;

		fi >> m_structAdjustCondition.dCalibrationInterlock; 

//2014.11.27 by tskim Measinterval
		fi >> m_structAdjustCondition.m_n1stMeas;
		fi >> m_structAdjustCondition.m_n2ndMeas;
		//150527 ehji
		fi >> m_structAdjustCondition.m_dDistperdropEmergency;
		for(i = 0; i < MAX_NOZZLE; i++) 
			fi >> m_structAdjustCondition.m_dPulseLimit_P[i];
		for(i = 0; i < MAX_NOZZLE; i++) 
			fi >> m_structAdjustCondition.m_dPulseLimit_M[i];
	}
	else
	{
		return FALSE;
	}
//	if(m_structAdjustCondition.m_nCheckingIntervalSheet != 0) Measure_Step_Enable = TRUE;
//	else Measure_Step_Enable = FALSE;

	// 화일 닫기
	fi.close();
	/////////////////////////////////////////////////////////////////////////////////
	// Level2Volume 화일읽기
	// 화일명 설정
	strPathName.Format("%s\\Level2Volume.dat", m_strDataPath);
	
	// 화일 열기
	fi.open((char *)(LPCSTR)strPathName, ios::in);

	// 화일 읽기
	if(fi.is_open())
	{
		for(int i = 0; i < 9; i++)
			fi >> m_dLevel2Volume[i];
	}
	else
	{
		return FALSE;
	}

	// 화일 닫기
	fi.close();

	///////////////////////////////////////////////////////////////////////////////
	// PatternData 화일 읽기 //////////////////////////////////////////////////////
	// PatternCondition창에 있는 내용도 같이 있음
	// 화일명 설정
	strPathName.Format("%s\\%s\\PatternData.dat", m_strDataPath, strRecipeName);
	
	// 화일 열기
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);

	// 화일 읽기
	if(!fi.is_open())  return FALSE;

	for(i = 0; i < m_structDataEditor.m_nPatternNum; i++)
	{
		fi >> ch;			// 패턴 번호 (debug용)
		fi >> m_structPatternData[i].m_dCenterPositionX;
//		if(gb3LineOpen !=TRUE)	// by ckh 2009.02.28;
//			m_structPatternData[i].m_dCenterPositionX*=(-1.0); // by ckh 2009.02.28
		fi >> m_structPatternData[i].m_dCenterPositionY;
//		if(gb3LineOpen !=TRUE)	// by ckh 2009.02.28;
//			m_structPatternData[i].m_dCenterPositionY*=(-1.0); // by ckh 2009.02.28;

		fi >> m_structPatternData[i].m_dWidthX;
		fi >> m_structPatternData[i].m_dWidthY;
		fi >> m_structPatternData[i].m_nPointNumberX;
		fi >> m_structPatternData[i].m_nPointNumberY;
		fi >> m_structPatternData[i].m_nDroppingNum;
		fi >> m_structPatternData[i].m_dAngleX;
		fi >> m_structPatternData[i].m_dAngleY;
		fi >> m_structPatternData[i].m_nDroppingStartPos;
		fi >> m_structPatternData[i].m_nStartDroppingAng;
		fi >> m_structPatternData[i].m_nDispenseMode;
		fi >> m_structPatternData[i].m_nTableSpeed;
		fi >> m_structPatternData[i].m_nSAxisSpeed;

		m_structPatternData[i].m_nDropDataCount = m_structPatternData[i].m_nPointNumberX * m_structPatternData[i].m_nPointNumberY;

		// Pattern Condition창에 있는 내용들 
		for(j = 0; j < MAX_NOZZLE; j++){
			fi >> m_structPatternData[i].m_dDropWeight[j];
		}

		for(j = 0; j < MAX_NOZZLE; j++){
			fi >> m_structPatternData[i].m_bIsOnHead[j];
		}
		
			
//		if(gb3LineOpen != TRUE)// by ckh 2009.02.28;
//  			fi >> m_structPatternData[m_structDataEditor.m_nPatternNum-1-i].m_nSequence; // by ckh 2009.02.28
// 		else// by ckh 2009.02.28;
			fi >> m_structPatternData[i].m_nSequence;
		//
		//
		for(j = 0; j < MAX_NOZZLE; j++){
			fi >> m_structPatternData[i].m_dNozzleOffset[j];
		}
		
//		if(gb3LineOpen != TRUE)// by ckh 2009.02.28;
//		{
//			for(j = 0; j < MAX_NOZZLE; j++)// by ckh 2009.02.28;
//				fi >> m_structPatternData[i].m_nPanelNo[MAX_NOZZLE-1-j];// by ckh 2009.02.28;
//		}
//		else// by ckh 2009.02.28;
//		{
			for(j = 0; j < MAX_NOZZLE; j++){
				fi >> m_structPatternData[i].m_nPanelNo[j];
			}
//		}
		fi >> m_structPatternData[i].m_bMMG;
		fi >> m_structPatternData[i].m_dBaseWeight;
		fi >> m_structPatternData[i].m_dLevelWeightDifference;		
		fi >> m_structPatternData[i].m_dDropOffset;		
		fi >> m_structPatternData[i].m_dDischargeSpeed;		
		fi >> m_structPatternData[i].m_dSuctionSpeed;		
		fi >> m_structPatternData[i].m_nSuctionMode;		
		fi >> m_structPatternData[i].m_dAccTime;		
		fi >> m_structPatternData[i].m_dDecTime;
		fi >> m_structPatternData[i].m_dStartSpeed;		
		fi >> m_structPatternData[i].m_nSuctionLineNo;		
		fi >> m_structPatternData[i].LinePatNum;
		fi >> m_structPatternData[i].m_nSequence_R;
	}

	// ehji 150310			RMS 보고문제 해결 55->49 inch Recipe Change 할때 Patten 초기화 구문 
	for (int k = 0; k < m_structDataEditor.m_nPatternNum; k++)
	{
		m_structPatternData[k].m_nNShot = 0;
		m_structPatternData[k].m_dNHz = 0;
		m_structPatternData[k].m_dNShotActionTime = 0;
		m_structPatternData[k].m_dNShotPitch = 0;
		
		for(j = 0; j < MAX_NOZZLE; j++)
			m_structPatternData[k].m_nNAmplitude[j] = 0;
		for(j = 0; j < MAX_NOZZLE; j++)
			m_structPatternData[k].m_nNNegOffset[j] = 0;
		for(j = 0; j < MAX_NOZZLE; j++)
			m_structPatternData[k].m_dNDuty[j]=0;
		for(j = 0; j < MAX_NOZZLE; j++)
			m_structPatternData[k].m_nNFirstVoltage[j]=0;
		for(j = 0; j < MAX_NOZZLE; j++)
			m_structPatternData[k].m_nNRunLimit[j]=0;
		for(j = 0; j < MAX_NOZZLE; j++)
			m_structPatternData[k].m_nNFallingEdge[j]=0;
		for(j = 0; j < MAX_NOZZLE; j++)
			m_structPatternData[k].m_nNPAxisOffset[j]=0;
		for(j = 0; j < MAX_NOZZLE; j++)
			m_structPatternData[k].m_nNRunDelay[j] = 8;
		for(j = 0; j < MAX_NOZZLE; j++)
			m_structPatternData[k].m_dTargetWeight_ByPattern[j] = -1;
	}


	for(k = 0; k < m_structDataEditor.m_nPatternNum; k++)
	{
		fi >> m_structPatternData[k].m_nNShot;
		fi >> m_structPatternData[k].m_dNHz;
		fi >> m_structPatternData[k].m_dNShotActionTime;
		fi >> m_structPatternData[k].m_dNShotPitch;
		
		for(j = 0; j < MAX_NOZZLE; j++)
			fi >> m_structPatternData[k].m_nNAmplitude[j];
		for(j = 0; j < MAX_NOZZLE; j++)
			fi >> m_structPatternData[k].m_nNNegOffset[j];
		for(j = 0; j < MAX_NOZZLE; j++)
			fi >> m_structPatternData[k].m_dNDuty[j];
		for(j = 0; j < MAX_NOZZLE; j++)
			fi >> m_structPatternData[k].m_nNFirstVoltage[j];
		for(j = 0; j < MAX_NOZZLE; j++)
			fi >> m_structPatternData[k].m_nNRunLimit[j];
		for(j = 0; j < MAX_NOZZLE; j++)
			fi >> m_structPatternData[k].m_nNFallingEdge[j];
		for(j = 0; j < MAX_NOZZLE; j++)
			fi >> m_structPatternData[k].m_nNPAxisOffset[j];
	}
	for(k = 0; k < m_structDataEditor.m_nPatternNum; k++)
	{
		for(j = 0; j < MAX_NOZZLE; j++)
			fi >> m_structPatternData[k].m_nNRunDelay[j];
		for(j = 0; j < MAX_NOZZLE; j++)
			fi >> m_structPatternData[k].m_dTargetWeight_ByPattern[j];		
	}

	fi.close();
	// 
	for(i=0;i<MAX_NOZZLE;i++)	m_bIsHeadSelected[i] = false;
	
	for(i = 0; i < m_structDataEditor.m_nPatternNum; i++){
		for(j = 0; j < MAX_NOZZLE; j++)
		{
			if(m_structPatternData[i].m_bIsOnHead[j])
				if(m_structPatternData[i].m_nSequence != 0)	m_bIsHeadSelected[j] = true;
		}
	}
	// 화일 닫기
	for(j = 0; j < MAX_NOZZLE; j++)
		g_bIsHeadSelected[j] = m_bIsHeadSelected[j];

//2013.08.22 by tskim 초기 AdjustPulse값은 일단 사용 Head의 Target값으로 자동 계산 하게 한다..
//Pattern 정보 Read후 사용 헤드 정보를 바탕으로 계산한다...
	for(k = 0; k < m_structDataEditor.m_nPatternNum; k++)
	{
		for(j = 0; j < MAX_NOZZLE; j++)
		{
			if(m_structPatternData[k].m_bIsOnHead[j])
			{
				m_structAdjustCondition.dAdjustPulse = m_structAdjustCondition.dTargetWeight[j] * BASE_PULSE;//1mg 800로 기준....
				break;
			}
		}
	}
	
	/////////////////////////////////////////////////////////////////////////////////

	// DropData 화일 읽기 //////////////////////////////////////////////////////
	// 이 함수는 없으면 기본값으로 셋팅한다. 
	for(i = 0; i < m_structDataEditor.m_nPatternNum; i++)
		ReadDropDataFile(i+1);  //////////////////////////////////////////////////////////////////////////////////

	// DropDataAddDel  화일 읽기 //////////////////////////////////////////////////////
	// 화일명 설정
	strPathName.Format("%s\\%s\\DropDataAddDel.dat", m_strDataPath, strRecipeName);
	
	// 화일 열기
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);

	// 화일 읽기
	if(fi.is_open())
	{
		for(int i = 0; i < 4; i++)
			fi >> m_nAdd[i];   
		for(i = 0; i < 4; i++)
			fi >> m_nDel[i];  
	}
	// 화일 닫기
	fi.close(); 

	/////////////////////////////////////////////////////////
	///// Sequence 화일 읽기
	/////////////////////////////////////////////////////////
	strPathName.Format("%s\\%s\\Sequence.dat", m_strDataPath, strRecipeName);
	
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);
	if(fi.is_open())
	{
		for(int i = 0; i < m_structDataEditor.m_nSequenceCount; i++)
		{
//			if(gb3LineOpen != TRUE) // by ckh 2009.02.28 작업자 위치 우선시 이부분 열기, 
//			{
//				for(int j = 0; j < m_structDataEditor.m_nPatternNum; j++)// by ckh 2009.02.28
//					fi >> m_nSeq[i][m_structDataEditor.m_nPatternNum-1-j];// by ckh 2009.02.28
//			}
//			else // by ckh 2009.02.28
//			{
				for(int j = 0; j < m_structDataEditor.m_nPatternNum; j++)  // JHC_C : RecipeNo.2인 경우 Pattern 수 2개 
				fi >> m_nSeq[i][j];    // JHC_C : Sequence 별로 갖는 Pattern 번호.  즉 어느 Pattern에 관한 Drop을 먼저 할 것인가 설정.
//			}
		}
	}
	else
	{
		return FALSE;
	}
	fi.close();

	/////////////////////////////////////////////////////////////////
	//측정값 보고에 따른 변수값 읽기..
	/////////////////////////////////////////////////////////////////
	strPathName.Format("%s\\MeasVariable.dat", m_strDataPath);
	fi.open(strPathName, ios::in);

	if(!fi.is_open())
	{
		AfxMessageBox("MeasVariable.dat파일이 없습니다");
		return FALSE;
	}

	
	fi>>CIM_Information.nDotting_Grupe_Num;// by ckh 2009.02.25 CIM_Information.MeasuringNum에서 nDotting_Grupe_Num으로 변경함. 
	fi>>CIM_Information.GlassSendCount;

	fi.close();

	/////////////////////////////////////////////////////////////////
	//// Glass Identity Read
	/////////////////////////////////////////////////////////////////
	strPathName.Format("%s\\%s\\GlassIden.dat", m_strDataPath, strRecipeName);
	fi.open(strPathName, ios::in);

	if(!fi.is_open())
	{
		AfxMessageBox("GlassIden.dat파일이 없습니다");
		return FALSE;
	}
	for(i=0; i<MAX_PANEL ;i++)
	{
		fi >> ch;
		m_strGlassIden[i].Format("%s",ch);
	}
	fi.close();

/*	/////////////////////////////////////////////////////////
	///// Calibration.dat Reading
	/////////////////////////////////////////////////////////
	strPathName.Format("%s\\VolCalibration.dat", m_strDataPath);
	fi.open((char *)(LPCSTR)strPathName, ios::in);

	if(fi.is_open())
	{
		// 측정data를 이용해서 계산된 coefficient
		for(i=0 ; i<MAX_NOZZLE ;i++)
		{
			for(j = 0; j < 6; j++){		fi >> m_structCalibData[i].dCoefficient1[j];	}
			for(j = 0; j < 6; j++){		fi >> m_structCalibData[i].dCoefficient2[j];	}
			for(j = 0; j < 6; j++){		fi >> m_structCalibData[i].dCoefficient3[j];	}
		}
		// vol
		for(i = 0; i < MAX_MEADATA_FORCALIB; i++)
			for(j=0 ; j<MAX_NOZZLE ;j++) fi >> m_structCalibData[j].dMeasure[i][0];
		// pos
		for(i = 0; i < MAX_MEADATA_FORCALIB; i++)
			for(j=0 ; j<MAX_NOZZLE ;j++) fi >> m_structCalibData[j].dMeasure[i][1];
	}
	else
	{
		return FALSE;
	}
	
	fi.close();
*/
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	// ValidDropPoint화일 읽기
	// 계산에 의한 DropPoint수는  실DropPoint수와 다른데, 실DropPoint수를 관리하기위한 파일을 작성한다.
	// 화일명 설정
	strPathName.Format("%s\\%s\\ValidDropPoint.dat", m_strDataPath, strRecipeName);
	
	// 화일 열기
	fi.open((char *)(LPCSTR)strPathName, ios::in);
//	for(i = 0; i < 88; i++) m_nValidDrop_compare[i][0] = 0;
	for(i = 0; i < MAX_PANEL; i++) m_nValidDrop_compare[i][0] = 0;
	// 화일 읽기
	if(fi.is_open())
	{
		for(int i = 0; i < m_structDataEditor.m_nPatternNum; i++)
		{
			fi >> ch;	// 패턴 번호
			for(int j = 0; j < MAX_NOZZLE; j++)
			{
				fi >> m_nValidDropPoint[i][j];
//////////////Auto 구동중 실Drop수와 비교하기 위해...Yamary...070629///////////////////
				m_nValidDrop_compare[m_structPatternData[i].m_nPanelNo[j]-1][0] += m_nValidDropPoint[i][j];
			}
		}
	}
	else
	{
		return FALSE;
	}

	// 화일 닫기
	fi.close();

	//by shin//2009.08.26//RMS C00값 보고를 위해서 위 유효드랍수를 백업한다...//
//	for(i = 0; i < m_structDataEditor.m_nPatternNum; i++)
//	{
//		for(j = 0; j < MAX_NOZZLE; j++)
//		{
//			m_nRmsValidDropPoint[i][j] = m_nValidDropPoint[i][j];
//		}
//	}

	/////////////////////////////////////////////////////////////////////////////////
	// RMS 보고 관련//by shin//2009.08.26//
	strPathName.Format("%s\\%s\\RMS_Report.dat", m_strDataPath, strRecipeName);
	fi.open(strPathName, ios::in);

	if(!fi.is_open())
	{
		AfxMessageBox("RMS_Report.dat파일이 없습니다");
		return FALSE;
	}
		
	fi >> m_dRmsTargetWeight;
	fi >> ch;
	m_strRmsLcType.Format("%s", ch);
	fi >> m_nRmsPointNumberX;
	fi >> m_nRmsPointNumberY;
	fi >> m_dRmsWidthX;
	fi >> m_dRmsWidthY;
	fi >> m_nRmsCenterDotting;
//2010.09.19 by tskim
	fi >> m_dRmsGlassSizeX;	
	fi >> m_dRmsGlassSizeY;	
	fi >> m_dRmsHomeX;			
	fi >> m_dRmsHomeY;			
	fi >> m_dRmsThickness;		
	fi >> m_nRmsPatternNum;		
	fi >> m_nRmsSpecialInfo;
	fi >> m_nRmsSafetyLock;
	fi >> m_nRmsGlassCheck;	
	fi >> m_nRmsRecipeNumCompare;
	fi >> m_nRmsPatternDispensing;
	fi >> m_nRmsNgDropping;
	fi >> m_nRmsLcSupplyType;
	fi >> m_nRmsDummyType;
	fi >> m_nRmsCanisterEmgWeight;  
    fi >> m_nRmsLiquidSupplyLimit;   
	fi >> m_nRmsAdjustInAuto;
	fi >> m_nRmsDummyDropBeforeMeasInAuto;	
	fi >> m_nRmsTableSpeed;
	fi >> m_nRmsSAxisSpeed;
	fi >> m_dRmsDropOffset;
	fi >> m_dRmsDischargeSpeed;
	fi >> m_dRmsSuctionSpeed;
	fi >> m_dRmsAccTime;
	fi >> m_dRmsDecTime;
	fi >> m_dRmsStartSpeed;
	for(i=0;i<MAX_NOZZLE;i++)
		fi >> m_dRmsNozzleOffset[i];
	for(i=0;i<5;i++)
	{
		fi >> m_dRmsCenterPositionX[i];
		fi >> m_dRmsCenterPositionY[i];
	}
	fi >> m_nRmsSuctionLineNo;
	fi >> m_nRmsSequence;

	fi.close();

////////////////////////////////////////////////////////ehji 150602
	strPathName.Format("%s\\%s\\Headable.dat", m_strDataPath, strRecipeName);
	fi.open(strPathName, ios::in);

	if(!fi.is_open())
	{
		AfxMessageBox("Headable.dat파일이 없습니다");
		return FALSE;
	}

	for(i=0;i<MAX_NOZZLE;i++)		//ehji 150602
	{
		fi >> m_structDataEditor.m_bHeadable[i];	
	}

	fi.close();
////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	///// MeasuredData.txt Reading
	/////////////////////////////////////////////////////////
	ReadMeasuredData();
	Read_SpecoutCount();
	//////////////////////////////////////////////////////////////
	if(ReadBarcodeData() == FALSE)
	{
		AfxMessageBox("Barcode.dat파일이 없습니다"); 
//		return FALSE;//2015.07.08 by tskim
	}
	ReadSPDData();
	ReadAccuracyMotorPos();
	ReadMotorMoveDistance();
	ReadSupplyWeight();
	ReadDummyData();
	ReadSupplyTimeData();
	if(ReadCanisterTypeData() == FALSE)
	{
		AfxMessageBox("CanisterType.dat파일이 없습니다"); 
//		return FALSE;//2015.07.08 by tskim
	}

 	for(i=0; i < 4 ; i++)
 			m_dCanister[1][i] = m_structAdjustCondition.nCanisterEmgWeight; //090702 Canister 경고량 Setting

	ReadLineDropData();

	ReadMarkAlignResult();


//2013.12.10 by tskim
 //	if(m_structDataEditor.m_bUse_Vision && _SCAN)
//	{
		Read_Vision_Param();
		Read_Vision_Y_Offset();
		Read_Vision_X_Offset();	//ehji 141025
//	}
	Read_Action_Offset();//2015.07.08 by tskim
//Calibration Mode by tskim
	ReadVolCalibData();

	ReadFlexVolCalibData();

	ReadMuitiTarget();

//2014.05.07 by tskim
	Read_Vision_PanelDropRange();

//2015.01.24 by tskim Section Calibration
	Read_SectionCalibrationData();
	Read_Section1stCalibrationData();
	Read_FlexibleData_First();
	
	return TRUE;
}

// 마지막으로 작업했던 recipe name들을 파일로 저장한다.
// 폴더명만 저장한다.
void CP8CA_LcDispDoc::SaveLastRecipe()
{
	CString strPath;
	FILE *fp;

	strPath.Format("%s\\LastRecipe.dat", m_strDataPath);

	fp = fopen(strPath, "wt");

	fprintf(fp, "%d\n", m_bIsEngineerMode);
	fprintf(fp, "%s\n", m_structOperatorModeRecipeData.strFrontRecipeName);
//2010.02.10 by tskim
	fprintf(fp, "%s\n", m_structMachineInfo.m_strMachineNo);
	fprintf(fp, "%s\n", m_structMachineInfo.m_strLineNo);
	fprintf(fp, "%s\n", m_structMachineInfo.m_strSite);//Site구분 

	fclose(fp);
}
// Panel 판정값 보고에 따른 측정번호, Glass 반출 후 보고 되지 않은 수 저장.
void CP8CA_LcDispDoc::SaveMeasVariable()
{
	CString strPath;
	FILE *fp;

	strPath.Format("%s\\MeasVariable.dat", m_strDataPath);

	fp = fopen(strPath, "wt");

	fprintf(fp, "%d\n", CIM_Information.nDotting_Grupe_Num);// by ckh 2009.02.25 nDotting_Grupe_Num으로 변경함. 
	fprintf(fp, "%d\n", CIM_Information.GlassSendCount);

	fclose(fp);
}

void CP8CA_LcDispDoc::SaveLevel2Volume()
{
	CString strPath;
	FILE *fp;

	strPath.Format("%s\\Level2Volume.dat", m_strDataPath);

	fp = fopen(strPath, "wt");

	for( int i = 0 ; i < 9 ; i ++)
		fprintf(fp, "%.2f\n", m_dLevel2Volume[i]);


	fclose(fp);
}

void CP8CA_LcDispDoc::SaveMeasuredData()
{
	FILE *fp;
	CString strPath ="";
	int i = 0 , j = 0;
//
	strPath.Format("%s\\%s\\Measured.txt", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
	fp = fopen(strPath, "wt");
	if(fp != NULL)
	{
		for(int i = 0; i < MAX_MEASURED_DATA ; i++)
		{
			for( int j = 0 ; j < MAX_NOZZLE ; j++ )
				fprintf(fp, "%.5f\t",m_dMeasuredData[j][i]);
			fprintf(fp, "\n");
		}
		// 화일 닫기
	//	fclose(fp);
	}
	// 화일 닫기
	fclose(fp);

	strPath.Format("%s\\%s\\Measured_First.txt", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
	fp = fopen(strPath, "wt");
	if(fp != NULL)
	{
		for(int i = 0; i < MAX_MEASURED_DATA ; i++)
		{
			for( int j = 0 ; j < MAX_NOZZLE ; j++ )
				fprintf(fp, "%.5f\t",m_dMeasuredData_First[j][i]);
			fprintf(fp, "\n");
		}
		// 화일 닫기
	//	fclose(fp);
	}
	// 화일 닫기
	fclose(fp);

	strPath.Format("%s\\%s\\Measured_Target.txt", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
	fp = fopen(strPath, "wt");
	if(fp != NULL)
	{
		for(int i = 0; i < MAX_MEASURED_DATA ; i++)
		{
			for( int j = 0 ; j < MAX_NOZZLE ; j++ )
				fprintf(fp, "%.3f\t",m_dMeasuredData_Target[j][i]);
			fprintf(fp, "\n");
		}
		// 화일 닫기
	//	fclose(fp);
	}
	// 화일 닫기
	fclose(fp);

	strPath.Format("%s\\%s\\Measured_Total.txt", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
	fp = fopen(strPath, "wt");
	if(fp != NULL)
	{
		for(int i = 0; i < MAX_MEASURED_DATA ; i++)
		{
			for( int j = 0 ; j < MAX_NOZZLE ; j++ )
				fprintf(fp, "%.3f\t",m_dMeasuredData_Total[j][i]);
			fprintf(fp, "\n");
		}
		// 화일 닫기
	//	fclose(fp);
	}
	// 화일 닫기
	fclose(fp);

//2015.07.08 by tskim 
	if(m_structDataEditor.m_nNzlMode == 1)
		Save_Action_Offset();
}
//여기까지...
// Calibration창 화일로 저장
void CP8CA_LcDispDoc::SaveVolCalibData()
{
	FILE *fp;
	CString strPath ="";
	int i=0 ,j=0, k=0 ;
//
	strPath.Format("%s\\VolCalibration.dat", m_strDataPath);	
	fp = fopen((char *)(LPCSTR)strPath, "wt");
	if(!fp) 
	{
		AfxMessageBox("VolCalibration.dat 파일 생성 실패");
		return;
	}

	for(i=0; i<MAX_NOZZLE; i++)
	{
		for(j = 0; j < MAX_MEADATA_FORCALIB; j++)
		{
			fprintf(fp, "%.3f\t", m_structCalibData[i].dCoefficient1[j]);
			fprintf(fp, "%.3f\t", m_structCalibData[i].dCoefficient2[j]);
		}
		fprintf(fp, "\n");
	}
	for(i=0; i<MAX_NOZZLE; i++)
	{
		for(k =0; k < MAX_MEADATA_GETCOUNT1; k++)
		{
			fprintf(fp, "%.3f\t", m_structCalibData[i].dMeasure[k][0]);
			fprintf(fp, "%.0f\t", m_structCalibData[i].dMeasure[k][1]);
		}
		fprintf(fp, "\n");
	}

	fclose(fp);
}

// actual pos
void CP8CA_LcDispDoc::SaveActualPos()
{
	FILE *fp;
	CString strPath ="";
	int ihead=0 ;
	double dPos=0.0;
//
	strPath.Format("%s\\McActualPos.dat", m_strDataPath);	
	fp = fopen((char *)(LPCSTR)strPath, "wt");
	if(!fp) 
	{
		AfxMessageBox("McActualPos.dat 파일 생성 실패");
		return;
	}
	
	// k1~k2
	FAS_GetActualPos(AXIS_K1/BOARD_AXES+1,AXIS_K1%BOARD_AXES,&dPos); fprintf(fp, "%.0f\t", dPos);
	FAS_GetActualPos(AXIS_K2/BOARD_AXES+1,AXIS_K2%BOARD_AXES,&dPos); fprintf(fp, "%.0f\t", dPos);
	fprintf(fp, "\n");

	// S
	for(ihead=0 ; ihead<MAX_NOZZLE ;ihead++) 
	{
		FAS_GetActualPos(m_structHeadConfig[ihead].nS[0],m_structHeadConfig[ihead].nS[1],&dPos);
		fprintf(fp, "%.0f\t", dPos);
	}

	fclose(fp);
}

BOOL CP8CA_LcDispDoc::ReadActualPos()
{
	ifstream fi;
	CString strPathName = "";
	int ihead=0 ,iIndex=0, iBdK = 0, iAxis = 0;
	double dCurPos=0.0, dPrevPos=0.0;

	ThreadStage.nHomeEnableCount=0;

	strPathName.Format("%s\\McActualPos.dat", m_strDataPath);
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);

	if(fi.is_open())
	{
		iIndex=0;

		// K1 ~ K2
		iBdK = 2;
		for(iAxis=0;iAxis<2;iAxis++)
		{
			FAS_GetActualPos(iBdK,iAxis,&dCurPos);
			fi >> dPrevPos;
			if(dCurPos>(dPrevPos+50) || dCurPos<(dPrevPos-50)) { iIndex++; break; }
		}

		// S축
		for(ihead=0 ; ihead<MAX_NOZZLE ;ihead++) 
		{
			FAS_GetActualPos(m_structHeadConfig[ihead].nS[0],m_structHeadConfig[ihead].nS[1],&dCurPos);
			fi >> dPrevPos;
			if(dCurPos>(dPrevPos+20) || dCurPos<(dPrevPos-20)) { iIndex++; break;}
		}

		if(iIndex != 0)	
			 ThreadStage.nHomeEnableCount=0;
		else ThreadStage.nHomeEnableCount=3;
	}
	fi.close();
	return TRUE;
}

// adjusting condition의 내용을 adjust부분에서 수정할 경우, 데이타를 저장한다..
void CP8CA_LcDispDoc::SaveAdjustingCondition()
{
	FILE *fp;
	CString strPath ="";
	int i = 0 , j = 0;

	// 화일명 설정
	strPath.Format("%s\\%s\\AdjustingCondition.dat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
 		
	// 화일 열기
	fp = fopen((char *)(LPCSTR)strPath, "wt");
	if(!fp) 
	{
		AfxMessageBox("AdjustingCondition.dat 파일 생성 실패");
		return;
	}

	// 데이타 저장
	for(i = 0; i < MAX_NOZZLE; i++)
		fprintf(fp, "%.3f\t", m_structAdjustCondition.dTargetWeight[i]);

	fprintf(fp, "\n");
	
	for(i = 0; i < MAX_NOZZLE; i++)
			fprintf(fp, "%.2f\t", m_structAdjustCondition.dTargetError[i]);

	fprintf(fp, "\n");

	for(i = 0; i < MAX_NOZZLE; i++)
			fprintf(fp, "%.2f\t", m_structAdjustCondition.dTargetErrorAlarm[i]);
	fprintf(fp, "\n");

	for(i = 0; i < MAX_NOZZLE; i++)
			fprintf(fp, "%.1f\t", m_structAdjustCondition.dTargetErrorJudgePlus[i]);

	for(i = 0; i < MAX_NOZZLE; i++)
			fprintf(fp, "%.1f\t", m_structAdjustCondition.dTargetErrorJudgeMinus[i]);

	fprintf(fp, "\n");
	
	for(i = 0; i < MAX_NOZZLE; i++)
		fprintf(fp, "%d\t", m_structAdjustCondition.nDropCount[i]);

	fprintf(fp, "\n");
	
	for(i = 0; i < MAX_NOZZLE; i++)
			fprintf(fp, "%d\t", m_structAdjustCondition.nLoopCount[i]);

	fprintf(fp, "\n");
	fprintf(fp, "%d\t%d\n",		m_structAdjustCondition.nDummyDropTime, m_structAdjustCondition.nDummyDropCount);
	fprintf(fp, "%d\n",			m_structAdjustCondition.nCheckingIntervalSheet); 
	fprintf(fp, "%d\n",			m_structAdjustCondition.bAdjustInAuto);
	fprintf(fp, "%d\n",			m_structAdjustCondition.bDummyDropBeforeMeasInAuto);
	fprintf(fp, "%d\n",			m_structAdjustCondition.nAdjustTimes);
	fprintf(fp, "%.1f\t%d\n",	m_structAdjustCondition.dSpeed, m_structAdjustCondition.nDelay);
	fprintf(fp, "%.1f\n",		m_structAdjustCondition.fDropStartPos);
	fprintf(fp, "%d\n",			m_structAdjustCondition.nMeasureCycle);
	for(i = 0; i < MAX_NOZZLE; i++)
		fprintf(fp, "%d\t",			m_structAdjustCondition.nSpecIn[i]);

	fprintf(fp, "\n");
	fprintf(fp, "%.2f\t%d\n",	m_structAdjustCondition.nCanisterEmgWeight,m_structAdjustCondition.nLiquidSupplyLimit); //090702 Data 추가에 따른 저장 형식 변경(Canister경고량, 액정공급량Limit) 
	fprintf(fp, "%d\n",			m_structAdjustCondition.nCheckDegasSheet); 
	fprintf(fp, "%d\n",			m_structAdjustCondition.nDegasCount); 
//2011.04.27 by tskim
	for(i = 0; i < MAX_NOZZLE; i++)
			fprintf(fp, "%.2f\t", m_structAdjustCondition.dAbnormalTargetError[i]);
	fprintf(fp, "\n");
	fprintf(fp,"%.0f\n",m_structAdjustCondition.dAdjustPulse);
	fprintf(fp, "%.2f\n",			m_structAdjustCondition.dMarginRate);

	fprintf(fp, "%.2f\n",			m_structAdjustCondition.dCalibrationInterlock);

//2014.11.27 by tskim
	fprintf(fp, "%d\n",			m_structAdjustCondition.m_n1stMeas); 
	fprintf(fp, "%d\n",			m_structAdjustCondition.m_n2ndMeas); 

	//150527	ehji
	fprintf(fp, "%.2f\n",			m_structAdjustCondition.m_dDistperdropEmergency); 	

	for(i = 0; i < MAX_NOZZLE; i++)
		fprintf(fp, "%.0f\n",			m_structAdjustCondition.m_dPulseLimit_P[i]);
	for(i = 0; i < MAX_NOZZLE; i++)
		fprintf(fp, "%.0f\n",			m_structAdjustCondition.m_dPulseLimit_M[i]);
	// 화일 닫기
	fclose(fp);
}

// 현재 작업중인 recipe의 해당 패턴의 drop data파일을 읽는다
// 데이타가없으면 기본 데이타 하나만 생성한다.
void CP8CA_LcDispDoc::ReadDropDataFile(long nP)
{
	// 일단 화일을 연다.
	ifstream fi;
	CString strPathName;
	DROP_DATA structDropData;

	nP = nP - 1;

	// 일단 배열 변수는 초기화 한다.
	m_arDropData[nP].RemoveAll();
//2010.06.21 by tskim Repair
	// 일단 배열 변수는 초기화 한다.
	m_arDropDataBk[nP].RemoveAll();

	// 화일명 설정
	strPathName.Format("%s\\%s\\SHT%03d.pat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName, nP+1);
	
	// 화일 열기
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);

	// 화일이 있으면 화일을 읽는다.
	if(fi.is_open())
	{
		fi >> m_structPatternData[nP].m_nDropDataCount;
		for(long d = 0; d < m_structPatternData[nP].m_nDropDataCount; d++)
		{
			fi >> structDropData.m_dPointX;
			fi >> structDropData.m_dPointY;
//			if(gb3LineOpen != TRUE)// by ckh 2009.02.28;
//				structDropData.m_dPointY*=(-1.0); // by ckh 2009.02.28
			fi >> structDropData.m_nN;
			for(int i = 0; i < MAX_NOZZLE; i++){
				fi >> structDropData.m_nZ[i];
			}
			for(i=0;i < MAX_NOZZLE; i++){
				fi >> structDropData.m_nAddDel[i];
			}
			m_arDropData[nP].Add(structDropData);
//2010.06.21 by tskim Repair
			m_arDropDataBk[nP].Add(structDropData);
		}
	}
	// 화일이 없으면 기본값으로 셋팅한다.
	else
	{
		structDropData.m_dPointX = 0.0;
		structDropData.m_dPointY = 0.0;
		structDropData.m_nN = 1;
		for(int i = 0; i < MAX_NOZZLE; i++){
			structDropData.m_nZ[i] = TRUE;
			structDropData.m_nAddDel[i] = 0;
		}

		m_arDropData[nP].Add(structDropData);
//2010.06.21 by tskim Repair
		m_arDropDataBk[nP].Add(structDropData);
	}
	// 화일 닫기
	fi.close();

	// 화일명 설정
	strPathName.Format("%s\\%s\\SHT_R%03d.pat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName, nP+1);
	
	// 화일 열기, 없어도 새로 만들지 않는다.
	fi.open((char *)(LPCSTR)strPathName, ios::nocreate);

	// 화일이 있으면 화일을 읽는다.
	if(fi.is_open())
	{
		fi >> m_structPatternData[nP].m_nDropDataCount;
		for(long d = 0; d < m_structPatternData[nP].m_nDropDataCount; d++)
		{
			fi >> structDropData.m_dPointX;
			fi >> structDropData.m_dPointY;
			fi >> structDropData.m_nN;
			for(int i = 0; i < MAX_NOZZLE; i++)
				fi >> structDropData.m_nZ[i];
			for( i = 0; i < MAX_NOZZLE; i++)
				fi >> structDropData.m_nAddDel[i];

			m_arDropData_R[nP].Add(structDropData);
		}
	}
	// 화일이 없으면 기본값으로 셋팅한다.
	else
	{
		fi >> m_structPatternData[nP].m_nDropDataCount;
		for(long d = 0; d < m_structPatternData[nP].m_nDropDataCount; d++)
		{
			structDropData.m_dPointX = 0.0;
			structDropData.m_dPointY = 0.0;
			structDropData.m_nN = 1;
			for(int i = 0; i < MAX_NOZZLE; i++)
			{
				structDropData.m_nZ[i] = TRUE;
				structDropData.m_nAddDel[i] = 0;
			}

			m_arDropData_R[nP].Add(structDropData);
		}
	}
	// 화일 닫기
	fi.close();
}

BOOL CP8CA_LcDispDoc::ReadCalSetFile(CString strRecipeName)
{
	CFile ReadFile;
	CString strPathName;

	// 프로그램 시작시에, Teaching.dat 화일을 열고서 Docu 변수에 저장
	// 화일명 설정
	strPathName.Format("%s\\Teaching.dat", m_strDataPath);
	if( ReadFile.Open((char *)(LPCSTR)strPathName, CFile::modeRead ) )
	{
		m_nSelFile = 0; 
		CArchive ar( &ReadFile, CArchive::load );
		Serialize(ar); 
		ar.Close();
		ReadFile.Close();
	}
/*
	strPathName.Format("%s\\%s\\counter.dat", m_strDataPath, strRecipeName);

	if( ReadFile.Open((char *)(LPCSTR)strPathName, CFile::modeRead ) )
	{
		m_nSelFile = 1; 
 		CArchive ar( &ReadFile, CArchive::load );
		Serialize(ar); 
		ar.Close();
		ReadFile.Close();
	}
*/
//2015.04.29 by tskim 원재료 전산화Add LC잔량보고
//	strPathName.Format("%s\\%s\\offset.dat", m_strDataPath, strRecipeName);
// 	strPathName.Format("%s\\offset.dat", m_strDataPath);
// 	if( ReadFile.Open((char *)(LPCSTR)strPathName, CFile::modeRead ) )
// 	{
// 		m_nSelFile = 2; 
//  		CArchive ar( &ReadFile, CArchive::load );
// 		Serialize(ar); 
// 		ar.Close();
// 		ReadFile.Close();
// 	}

	strPathName.Format("%s\\common.dat", m_strDataPath);
	if( ReadFile.Open((char *)(LPCSTR)strPathName, CFile::modeRead ) )
	{
		m_nSelFile = 3; 
 		CArchive ar( &ReadFile, CArchive::load );
		Serialize(ar); 
		ar.Close();
		ReadFile.Close();
	}

	strPathName.Format("%s\\pumpLCNo.dat", m_strDataPath);
	if( ReadFile.Open((char *)(LPCSTR)strPathName, CFile::modeRead ) )
	{
		m_nSelFile = 4; 
 		CArchive ar( &ReadFile, CArchive::load );
		Serialize(ar); 
		ar.Close();
		ReadFile.Close();
	}

	strPathName.Format("%s\\%s\\PanelNG_MarkSet.dat", m_strDataPath,strRecipeName);
	if( ReadFile.Open((char *)(LPCSTR)strPathName, CFile::modeRead ) )
	{
		m_nSelFile = 5; 
 		CArchive ar( &ReadFile, CArchive::load );
		Serialize(ar); 
		ar.Close();
		ReadFile.Close();
	}

	strPathName.Format("%s\\Canister.dat", m_strDataPath);
	if( ReadFile.Open((char *)(LPCSTR)strPathName, CFile::modeRead ) )
	{
		m_nSelFile = 6; 
 		CArchive ar( &ReadFile, CArchive::load );
		Serialize(ar); 
		ar.Close();
		ReadFile.Close();
	}

	strPathName.Format("%s\\DummyCupWeight.dat", m_strDataPath);
	if( ReadFile.Open((char *)(LPCSTR)strPathName, CFile::modeRead ) )
	{
		m_nSelFile = 7; 
 		CArchive ar( &ReadFile, CArchive::load );
		Serialize(ar); 
		ar.Close();
		ReadFile.Close();
	}

//2010.09.19 by tskim EC Data Read Teaching Data Read
//EC Data Load
 	for(int i=0;i<MAX_NOZZLE;i++)//
 	{
 		m_structEc.m_dDmyPos_S[i] = m_dTeachData[i/3][i%3];
 		m_structEc.m_dMeasPos_S[i] = m_dTeachData[6+(i/2)][i%2];
		if(i<MAX_NOZZLE/2)//8,7,6....
		{
			m_structEc.m_dSupPos_S[(MAX_NOZZLE/2-i)-1] = m_dTeachData[28+(i/3)][(i%3)];
		}
		else//9,10,11....
		{
			m_structEc.m_dSupPos_S[i] = m_dTeachData[31+((i -MAX_NOZZLE/2)/3)][((i-MAX_NOZZLE/2)%3)];	
		}
 	}
	m_structEc.m_dDmyPos_Col = m_dTeachData[5][2];
	m_structEc.m_dMeasPos_Col = m_dTeachData[13][2];
	m_structEc.m_dMutePos_Col[0] = m_dTeachData[5][1];//Dummy Cup 교환 
	m_structEc.m_dMutePos_Col[1] = m_dTeachData[5][2];//Dummy Drop
	m_structEc.m_dMutePos_Col[2] = m_dTeachData[18][2];//작업자...
	m_structEc.m_dMarkPos_S1 = m_dTeachData[15][0];
	m_structEc.m_dMarkPos_S16 = m_dTeachData[15][1];
	m_structEc.m_dMarkPos_Col = m_dTeachData[15][2];
	for(i=0;i<3;i++)
	{
		m_structEc.m_dSensingTime[i] = m_dTeachData[16][i];
		m_structEc.m_dTriggerTime[i] = m_dTeachData[37][i];
	}
	m_structEc.m_dCntPos_Col = m_dTeachData[17][2];
	m_structEc.m_dOperPos_Col = m_dTeachData[18][2];
	m_structEc.m_dLoadPos_F = m_dTeachData[19][1];
	m_structEc.m_dLoadPos_R = m_dTeachData[19][2];
	m_structEc.m_dSupPos_Col = m_dTeachData[34][2];
	m_structEc.m_dSupAmount = m_dTeachData[36][0];
	m_structEc.m_dSupTime = m_dTeachData[36][2];
	m_structEc.m_dInputLimit[0] = m_dTeachData[38][0]; 
	m_structEc.m_dInputLimit[1] = m_dTeachData[38][1];
	m_structEc.m_dColSpd = m_dTeachData[39][2];
//2015.044.29 by tskim 원재료 전산화 Add LC잔량 
	Read_LCRemain();
//2015.04.29 by tskim 원재료 전산화Add Dummy Drop 보고
	Read_DummyDropStatus();
	return TRUE;
}

// MeasuredData.txt열기..
BOOL CP8CA_LcDispDoc::ReadMeasuredData()
{
	ifstream fi;
	CString strPathName = "";
	int i = 0 , j = 0;
//
	strPathName.Format("%s\\%s\\Measured.txt", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
	
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);
//
	if(fi.is_open())
	{
		for(int i = 0; i < MAX_MEASURED_DATA ; i++)
		{
			for( int j = 0 ; j < MAX_NOZZLE ; j++ )
			fi >> m_dMeasuredData[j][i];
		}
	}
	// 화일이 없으면 기본값으로 셋팅한다.
	else
	{
		// 변수는 doc 생성자에서 초기화되었으므로 flag만 return한다..
		return FALSE;
	}
	// 화일 닫기
	fi.close();

	strPathName.Format("%s\\%s\\Measured_First.txt", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
	
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);
//
	if(fi.is_open())
	{
		for(int i = 0; i < MAX_MEASURED_DATA ; i++)
		{
			for( int j = 0 ; j < MAX_NOZZLE ; j++ )
			fi >> m_dMeasuredData_First[j][i];
		}
	}
	// 화일이 없으면 기본값으로 셋팅한다.
	else
	{
		// 변수는 doc 생성자에서 초기화되었으므로 flag만 return한다..
		return FALSE;
	}
	// 화일 닫기
	fi.close();

	strPathName.Format("%s\\%s\\Measured_Target.txt", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
	
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);
//
	if(fi.is_open())
	{
		for(int i = 0; i < MAX_MEASURED_DATA ; i++)
		{
			for( int j = 0 ; j < MAX_NOZZLE ; j++ )
			fi >> m_dMeasuredData_Target[j][i];
		}
	}
	// 화일이 없으면 기본값으로 셋팅한다.
	else
	{
		// 변수는 doc 생성자에서 초기화되었으므로 flag만 return한다..
		return FALSE;
	}
	// 화일 닫기
	fi.close();

	strPathName.Format("%s\\%s\\Measured_Total.txt", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
	
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);
//
	if(fi.is_open())
	{
		for(int i = 0; i < MAX_MEASURED_DATA ; i++)
		{
			for( int j = 0 ; j < MAX_NOZZLE ; j++ )
			fi >> m_dMeasuredData_Total[j][i];
		}
	}
	// 화일이 없으면 기본값으로 셋팅한다.
	else
	{
		// 변수는 doc 생성자에서 초기화되었으므로 flag만 return한다..
		return FALSE;
	}
	// 화일 닫기
	fi.close();
	
	return TRUE;
}

// MeasuredData.txt열기..


// Add/Del의 정보를 이용해서 -99~+99방울까지 차이를 둔다.
// FTP File로부터 입력된 CIM_Information.Add_Del_Number를 이용한다. 
void CP8CA_LcDispDoc::SetValidDropPoint(int nP)
{
	double dTempAdd = 1.0, dTemp =0.0;
	int nAddPlus = 0;
	DROP_DATA stDrop;// dropdata 구조체
	int nTempAddDel , n = 0; 
	int i=0,j=0;


	if(m_structDataEditor.m_nNzlMode == 1)
	{
		for( n =0; n < MAX_NOZZLE ; n++)
		{
			m_nValidDropPoint[nP][n] = 0 ;
			m_nValidDropPoint_Conv[nP][n] = 0;
		}
		//2007.06.06 by tskim
		//변환 된 add/del값 초기화
		for(i = 0; i < m_arDropData[nP].GetSize(); i++)   
		{
			stDrop = m_arDropData[nP].GetAt(i);			 
			for(j =0; j < MAX_NOZZLE; j++)
			{
				//2009.11.23 by tskim NG Cell은 유효 Drop에서 제외 한다..
				if(CIM_Information.bNGCell[m_structPatternData[nP].m_nPanelNo[j]-1])
				{
					m_nConversonAddDel[m_structPatternData[nP].m_nPanelNo[j]-1] = 0;
					if(m_structPatternData[nP].m_bIsOnHead[j] == FALSE)   
					{
						m_nValidDropPoint[nP][j] = 0;
						m_nValidDropPoint_Conv[nP][j] = 0;
						continue;
					}
					nTempAddDel = CIM_Information.Add_Del_Number[m_structPatternData[nP].m_nPanelNo[j]-1];
// 					if((m_structDataEditor.m_bAddDelChange == TRUE)&&(nTempAddDel != 0))///////////////////////////////////////
// 					{                                                                   //////////////////////////////////////
// 						//				dTempAdd = 4.0 / m_structAdjustCondition.dTargetWeight[j];      ///Add_Del 변환 관련 계산... 05.29
// 						dTempAdd = (4.0 * nTempAddDel) / m_structAdjustCondition.dTargetWeight[j];
// 						if(dTempAdd != 1.0 || dTempAdd != -1.0)
// 						{
// 							dTemp = dTempAdd - (int)dTempAdd;
// 							if(dTemp >= 0.5) nAddPlus = (int)dTempAdd + 1;
// 							//2007.06.07 By tskim
// 							//del값 변환 부분에 대한 추가
// 							else if(dTemp <= -0.5) nAddPlus = (int)dTempAdd -1;
// 							else nAddPlus = (int)dTempAdd;
// 							
// 							nTempAddDel = nAddPlus;
// 							//2007.06.06 By tskim
// 							//Add/Del 변환 값 저장..
// 							m_nConversonAddDel[m_structPatternData[nP].m_nPanelNo[j]-1] = nTempAddDel;
// 						}
// 					}
					m_nConversonAddDel[m_structPatternData[nP].m_nPanelNo[j]-1] = nTempAddDel;
					//2007.06.06 by tskim
					//add/del값이 변환 됐었다면 변환 된 add/del을 적용하여 유효 drop수 재 계산 
					// m_nN = FALSE이면
					if((stDrop.m_nN == 0)||(stDrop.m_nZ[j] == 0))   
					{
						m_nValidDropPoint[nP][j] = m_nValidDropPoint[nP][j] +0;
						m_nValidDropPoint_Conv[nP][j] = m_nValidDropPoint_Conv[nP][j] +0;
					}
					else //stDrop.m_nN == TRUE이면 m_nZ를 검사한다. (stDrop.m_nN 도 TRUE이고 stDrop.m_nZ[j]도 TRUE인 경우..)
					{
						if((stDrop.m_nAddDel[j] == 0) || (m_structPatternData[nP].m_bMMG==TRUE) )     // ADD/DEL 과 무관한 일반 Point 인 경우에..
						{
							m_nValidDropPoint[nP][j] = m_nValidDropPoint[nP][j] +1;
							m_nValidDropPoint_Conv[nP][j] = m_nValidDropPoint_Conv[nP][j] +1;
						}
						else                          // Add/Del 값이 있는 점(잠정적인 변화를 줄 수 있는 점)이면..
						{ //-					-
							//2007.06.06 BY tskim
							//기존 유효 drop수 display를 위해 추가...
							/**********************************************************************************************************************************/
							if(stDrop.m_nAddDel[j] == CIM_Information.Add_Del_Number[m_structPatternData[nP].m_nPanelNo[j]-1]){   // Panel 정보와 CIM 정보가 같은 경우. Add => Drop, Del => No Drop
								if(stDrop.m_nAddDel[j] < 0){
									m_nValidDropPoint_Conv[nP][j] = m_nValidDropPoint_Conv[nP][j] +0;
								}
								else{
									m_nValidDropPoint_Conv[nP][j] = m_nValidDropPoint_Conv[nP][j] +1;
								}
							}
							else{                                  // Panel 정보와 CIM 정보가 다른 경우. CIM 정보를 우선 반영한다.
								if(stDrop.m_nAddDel[j] <CIM_Information.Add_Del_Number[m_structPatternData[nP].m_nPanelNo[j]-1]){    // CIM 정보 값이 크면 => Drop 
									m_nValidDropPoint_Conv[nP][j] = m_nValidDropPoint_Conv[nP][j] +1;
								}
								else{								  // CIM 정보 값이 작으면 => No Drop
									m_nValidDropPoint_Conv[nP][j] = m_nValidDropPoint_Conv[nP][j] +0;
								}
							}
							/**********************************************************************************************************************************/
							if(stDrop.m_nAddDel[j] == nTempAddDel){   // Panel 정보와 CIM 정보가 같은 경우. Add => Drop, Del => No Drop
								if(stDrop.m_nAddDel[j] < 0){
									m_nValidDropPoint[nP][j] = m_nValidDropPoint[nP][j] +0;
								}
								else{
									m_nValidDropPoint[nP][j] = m_nValidDropPoint[nP][j] +1;
								}
							}
							else{                                  // Panel 정보와 CIM 정보가 다른 경우. CIM 정보를 우선 반영한다.
								if(stDrop.m_nAddDel[j] <nTempAddDel){    // CIM 정보 값이 크면 => Drop 
									m_nValidDropPoint[nP][j] = m_nValidDropPoint[nP][j] +1;
								}
								else{								  // CIM 정보 값이 작으면 => No Drop
									m_nValidDropPoint[nP][j] = m_nValidDropPoint[nP][j] +0;
								}
							}
							
						} // End of else //--
					}
				}
			} // End of for(int j =0; j < MAX_NOZZLE; j++)
		}
		for(j = 0; j < MAX_NOZZLE; j++)
		{
			m_nValidDropPoint[nP][j] = (m_nValidDropPoint[nP][j] * m_structPatternData[nP].m_nNShot)/2;
			m_nValidDropPoint_Conv[nP][j] = m_nValidDropPoint[nP][j];
			//			if(m_nValidDropPoint[nP][j] < 0) m_nValidDropPoint[nP][j] = 0;
		}	
	}
	else
	{
		for( n =0; n < MAX_NOZZLE ; n++)
		{
			m_nValidDropPoint[nP][n] = 0 ;
			m_nValidDropPoint_Conv[nP][n] = 0;
		}
		//2007.06.06 by tskim
		//변환 된 add/del값 초기화
		for(i = 0; i < m_arDropData[nP].GetSize(); i++)   
		{
			stDrop = m_arDropData[nP].GetAt(i);			 
			for(j =0; j < MAX_NOZZLE; j++)
			{
				//2009.11.23 by tskim NG Cell은 유효 Drop에서 제외 한다..
				if(CIM_Information.bNGCell[m_structPatternData[nP].m_nPanelNo[j]-1])
				{
					m_nConversonAddDel[m_structPatternData[nP].m_nPanelNo[j]-1] = 0;
					if(m_structPatternData[nP].m_bIsOnHead[j] == FALSE)   
					{
						m_nValidDropPoint[nP][j] = 0;
						m_nValidDropPoint_Conv[nP][j] = 0;
						continue;
					}
					nTempAddDel = CIM_Information.Add_Del_Number[m_structPatternData[nP].m_nPanelNo[j]-1];
					if((m_structDataEditor.m_bAddDelChange == TRUE)&&(nTempAddDel != 0))///////////////////////////////////////
					{                                                                   //////////////////////////////////////
						//				dTempAdd = 4.0 / m_structAdjustCondition.dTargetWeight[j];      ///Add_Del 변환 관련 계산... 05.29
						dTempAdd = (4.0 * nTempAddDel) / m_structAdjustCondition.dTargetWeight[j];
						if(dTempAdd != 1.0 || dTempAdd != -1.0)
						{
							dTemp = dTempAdd - (int)dTempAdd;
							if(dTemp >= 0.5) nAddPlus = (int)dTempAdd + 1;
							//2007.06.07 By tskim
							//del값 변환 부분에 대한 추가
							else if(dTemp <= -0.5) nAddPlus = (int)dTempAdd -1;
							else nAddPlus = (int)dTempAdd;
							
							nTempAddDel = nAddPlus;
							//2007.06.06 By tskim
							//Add/Del 변환 값 저장..
							m_nConversonAddDel[m_structPatternData[nP].m_nPanelNo[j]-1] = nTempAddDel;
						}
					}
					//2007.06.06 by tskim
					//add/del값이 변환 됐었다면 변환 된 add/del을 적용하여 유효 drop수 재 계산 
					// m_nN = FALSE이면
					if((stDrop.m_nN == 0)||(stDrop.m_nZ[j] == 0))   
					{
						m_nValidDropPoint[nP][j] = m_nValidDropPoint[nP][j] +0;
						m_nValidDropPoint_Conv[nP][j] = m_nValidDropPoint_Conv[nP][j] +0;
					}
					else //stDrop.m_nN == TRUE이면 m_nZ를 검사한다. (stDrop.m_nN 도 TRUE이고 stDrop.m_nZ[j]도 TRUE인 경우..)
					{
						if((stDrop.m_nAddDel[j] == 0) || (m_structPatternData[nP].m_bMMG==TRUE) )     // ADD/DEL 과 무관한 일반 Point 인 경우에..
						{
							m_nValidDropPoint[nP][j] = m_nValidDropPoint[nP][j] +1;
							m_nValidDropPoint_Conv[nP][j] = m_nValidDropPoint_Conv[nP][j] +1;
						}
						else                          // Add/Del 값이 있는 점(잠정적인 변화를 줄 수 있는 점)이면..
						{ //-					-
							//2007.06.06 BY tskim
							//기존 유효 drop수 display를 위해 추가...
							/**********************************************************************************************************************************/
							if(stDrop.m_nAddDel[j] == CIM_Information.Add_Del_Number[m_structPatternData[nP].m_nPanelNo[j]-1]){   // Panel 정보와 CIM 정보가 같은 경우. Add => Drop, Del => No Drop
								if(stDrop.m_nAddDel[j] < 0){
									m_nValidDropPoint_Conv[nP][j] = m_nValidDropPoint_Conv[nP][j] +0;
								}
								else{
									m_nValidDropPoint_Conv[nP][j] = m_nValidDropPoint_Conv[nP][j] +1;
								}
							}
							else{                                  // Panel 정보와 CIM 정보가 다른 경우. CIM 정보를 우선 반영한다.
								if(stDrop.m_nAddDel[j] <CIM_Information.Add_Del_Number[m_structPatternData[nP].m_nPanelNo[j]-1]){    // CIM 정보 값이 크면 => Drop 
									m_nValidDropPoint_Conv[nP][j] = m_nValidDropPoint_Conv[nP][j] +1;
								}
								else{								  // CIM 정보 값이 작으면 => No Drop
									m_nValidDropPoint_Conv[nP][j] = m_nValidDropPoint_Conv[nP][j] +0;
								}
							}
							/**********************************************************************************************************************************/
							if(stDrop.m_nAddDel[j] == nTempAddDel){   // Panel 정보와 CIM 정보가 같은 경우. Add => Drop, Del => No Drop
								if(stDrop.m_nAddDel[j] < 0){
									m_nValidDropPoint[nP][j] = m_nValidDropPoint[nP][j] +0;
								}
								else{
									m_nValidDropPoint[nP][j] = m_nValidDropPoint[nP][j] +1;
								}
							}
							else{                                  // Panel 정보와 CIM 정보가 다른 경우. CIM 정보를 우선 반영한다.
								if(stDrop.m_nAddDel[j] <nTempAddDel){    // CIM 정보 값이 크면 => Drop 
									m_nValidDropPoint[nP][j] = m_nValidDropPoint[nP][j] +1;
								}
								else{								  // CIM 정보 값이 작으면 => No Drop
									m_nValidDropPoint[nP][j] = m_nValidDropPoint[nP][j] +0;
								}
							}
							
						} // End of else //--
					}
				}
			} // End of for(int j =0; j < MAX_NOZZLE; j++)
		}
	}
/*	for(i = 0; i < 88; i++) m_nValidDrop_compare[i][1] = 0;
	for(i = 0; i < m_structDataEditor.m_nPatternNum; i++)
	{
		for(j = 0; j < MAX_NOZZLE; j++)
		{
			if(m_structPatternData[i].m_bIsOnHead[j] == FALSE) continue;
			m_nValidDrop_compare[m_structPatternData[i].m_nPanelNo[j]-1][1] += m_nValidDropPoint[i][j];
		}
	}*/
}
void CP8CA_LcDispDoc::GetDropCount(int nP)  // nP : 0부터 시작된다.
{
	long aCount[18]={0,};
	FASC_GetAllCount(5, aCount);

	for(int head=0; head<MAX_NOZZLE;head++)
	{
		m_nDropCount[nP][head]=(int)aCount[head];
	}

	//일단 죽인다...//
//	Match_Worked_Report_for_ValidDropPoint();
}

CString CP8CA_LcDispDoc::LevelTostr(int Level)
{
	CString str="",str1="";	

	if(Level>0)		str1="P";		
	else if(Level<0)	str1="M";		
	else str1="C";	

	if(abs(Level)<10)	str.Format("0%d", abs(Level));		
	else	str.Format("%2d", abs(Level));

	str=str1+str;
	return str;
}

int CP8CA_LcDispDoc::StrtoValue(CString str)
{
	int sign=0, value=0;
	if(str.GetAt(0)=='P')		sign=(+1);
	else if(str.GetAt(0)=='M')	sign=(-1);
	else if (str.GetAt(0)=='C')	sign=0;

	value = atoi(str.Mid(1,2));
	return (sign*value);
}


///////////////////////////////////////
// 실처리 보고용으로 CIM에 지정된 Address에 지정된 Panel의 Level을 보고하기 위한 함수.
void CP8CA_LcDispDoc::Match_Worked_Report_for_Level()
{

	int nBaseIndex=0, nOffsetAddress=0,ArrIndex=0;

// 미리 100으로 넣는 이유는 원래 정상적인 ADD/DEL Level은 -99 ~ 99까지 이므로 
// 만약 처음에 할당한 100의 값이 변하지 않았다면 공백으로 처리해야 할 Panel이라는 뜻이 될 수 있다.
//	for(int i=0;i<88;i++)	CIM_Information.Worked_Report_Add_Del[i]=100;	
	
	for( int i = 0; i < MAX_PANEL; i++ )	CIM_Information.Worked_Report_Add_Del[i]=100;		

	for( i = 0; i < MAX_PANEL; i++ )
	{
//공백으로 처리해야 할 Panel들 구분.
		nBaseIndex=100;	

// 사용하지 않는 Panel에 대한 정보가 Overwrite되지 않도록 조건을 건다.
		if( i < CIM_Information.Panal_Max_Num )		
		{
			if( m_strGlassIden[i] != "" )
			{
				// 8개 영역
				//if ( m_strGlassIden[i].GetAt(1) < 9  ) 
				char chtemp = m_strGlassIden[i].GetAt(1);
				if ( 48 < chtemp &&  chtemp < 58  ) //1~9//2014.03.13 by tskim
				{
					switch(m_strGlassIden[i].GetAt(0))
					{
						case 'A':	nBaseIndex=0;	break;  // 8
						case 'B':	nBaseIndex=1;	break;  // 8
						case 'C':	nBaseIndex=2;	break;  // 8
						case 'D':	nBaseIndex=3;	break;  // 8
						case 'E':	nBaseIndex=4;	break;  // 8
						case 'F':	nBaseIndex=5;	break;  // 8
						case 'G':	nBaseIndex=6;	break;  // 8
						case 'H':	nBaseIndex=7;	break;  // 8
						case 'J':	nBaseIndex=8;	break;  // 8
						case 'K':	nBaseIndex=9;	break;  // 8
					}
				}
				// 12개 또는 20개 영역 
				else 
				{
					switch(m_strGlassIden[i].GetAt(0))
					{
						case 'A':	nBaseIndex=10;	break;  // 12
						case 'B':	nBaseIndex=11;	break;  // 12
						case 'C':	nBaseIndex=12;	break;  // 12
						case 'D':	nBaseIndex=13;	break;  // 12
						case 'E':	nBaseIndex=14;	break;  // 12
						case 'F':	nBaseIndex=15;	break;  // 12
						case 'G':	nBaseIndex=16;	break;  // 12
						case 'H':	nBaseIndex=17;	break;  // 12
						case 'J':	nBaseIndex=18;	break;  // 12
						case 'K':	nBaseIndex=19;	break;  // 12
						case 'L':	nBaseIndex=20;	break;	// 20
						case 'M':	nBaseIndex=21;	break;	// 20
						case 'N':	nBaseIndex=22;	break;	// 20
						case 'P':	nBaseIndex=23;	break;  // 20
						case 'Q':	nBaseIndex=24;	break;	// 20
					}
				}
			}
		}

		if( nBaseIndex != 100 )		// 이걸 안해 주면 배열의 할당에 문제가 생김.
		{
			nOffsetAddress = atoi((char *)(LPCSTR)m_strGlassIden[i].Mid(1,1)) - 1;
			
			if ( nBaseIndex < 10 )
			{
				ArrIndex = nBaseIndex * 8  + nOffsetAddress;
			}
			else if ( nBaseIndex < 20)
			{
				// 80 : 이전 Index 총 개수 
				// nBaseIndex - 10 : 이전 Glass Iden 개수
				// 12 : ID 당 Cell 개수 
				ArrIndex = 80 + ( ( nBaseIndex - 10 ) * 12 ) + nOffsetAddress;
			}
			else
			{
				// 80 + 120 : 이전 Index 총 개수 
				// nBaseIndex - 20 : 이전 Glass Iden 개수 
				// 20 : ID 당 Cell 개수  
				ArrIndex = 80 + 120 + ( ( nBaseIndex - 20 ) * 20 ) + nOffsetAddress;
			}

			CIM_Information.Worked_Report_Add_Del[ArrIndex] = CIM_Information.Add_Del_Number[i];
		}
	}
}
#include "MainFrm.h"
void CP8CA_LcDispDoc::Match_Worked_Report_for_ValidDropPoint(int nPattern_num,BOOL bDropEnable[MAX_PATTERN][MAX_NOZZLE])
{
	// 우선 보고해야 하는 Drop 수를 하나의 배열로 순차적으로 정렬한다.
	int ValDropTot=0, nCountReport=0, nBaseIndex=0, nOffsetAddress=0,ArrIndex=0;
	int head=0, pat=0;
	int i,j;
	//2009.12.17 by tskim 
	int headOld=-1, patOld =-1;
	//	for(int i=0;i<88;i++)	
	
	//	if((m_structDataEditor.m_bSelectDropMode == TRUE) && (ThreadStage.JobModeSelection == 0)) //Flexible Line Drop && Auto Drop
	
	if(m_structDataEditor.m_nNzlMode != 1)
	{
		for( i=0;i<MAX_PANEL;i++)			
		{
			//	pat = nPattern_num;// by ckh 2009.03.09 추가함. Pattern 별로 Skip 패널인지를 확인 할 수 있으므로
			ValDropTot = 0;
			if(i<CIM_Information.Panal_Max_Num)
			{	
				for( pat = 0 ; pat < m_structDataEditor.m_nPatternNum ; pat++)// by ckh 2009.03.09 해당 패턴만 갱신하도록함.
				{
					for( head = 0 ; head < MAX_NOZZLE ; head++)
					{
						if( (m_structPatternData[pat].m_nSequence > 0) && (m_structPatternData[pat].m_nPanelNo[head] == i + 1) )
						{
							//2009.12.08 by tskim						
							if((m_structDataEditor.m_bSelectDropMode == TRUE) && (ThreadStage.JobModeSelection == 0)) //Flexible Line Drop && Auto Drop
							{
								if(m_structDataEditor.m_bNgDropping != TRUE)
								{
									// Drop Count Sensor 사용할 경우 미드롭 패널일 경우는 0, 그외는 실 드롭으로 보고함.
									if(m_bDropCountSensorUSE[head] ==TRUE && bDropEnable[pat][head] ==TRUE) // by ckh 2009.03.09 수정함. Drop Count Sensor 사용 시 && Drop 할 경우.
									{
										if((m_structDataEditor.m_bSelectDropMode == TRUE) && (ThreadStage.JobModeSelection == 0)) //Flexible Line Drop && Auto Drop
										{
											if(CIM_Information.bNGCell[m_structPatternData[pat].m_nPanelNo[head]-1])
											{
												//2009.12.17 by tskim 
												//if(pat != patOld && head != headOld)
												//{
												//m_nTotalCount[pat][head] = m_nValidDropPoint[pat][head]; //보고는 무조건 정상인것으로...임시...//
												//2010.05.17 by tskim
												//m_nTotalCount[pat][head] = m_nFlexibleDropCount[head];
												//2009.11.23 by tskim  실처리 보고 부분 수정...
												//								if(fabs(m_nFlexibleDropCount[head]/*m_nValidDropPoint[pat][head]*/ - m_nTotalCount[pat][head]) <= 3*m_structDataEditor.m_nPatternNum)
												//								{
												//									m_nTotalCount[LinePattern_Seq_count-1][head] = m_nFlexibleDropCount[head]; // m_nValidDropPoint[pat][head];
												//								}
												nCountReport=m_nTotalCount[pat][head];
												ValDropTot+=nCountReport;
												//}
												//else
												//{
												//	nCountReport=0;
												//	ValDropTot=nCountReport;
												//}
											}
											else
											{
												//m_nTotalCount[pat][head] = 9999; // 최대값으로 함. skip 패널
												nCountReport= 9999; //m_nTotalCount[pat][head];
												ValDropTot=nCountReport; 
											}
											headOld = head;
											patOld = pat;
										}
										else
										{
											if(fabs(m_nValidDropPoint[pat][head] - m_nTotalCount[pat][head]) <= 3)
											{
												m_nTotalCount[pat][head] = m_nValidDropPoint[pat][head];
											}
											nCountReport=m_nTotalCount[pat][head];
											ValDropTot+=nCountReport;
										}
									}
									else if(m_bDropCountSensorUSE[head] !=TRUE && bDropEnable[pat][head] ==TRUE) // Drop Sensor 사용 안할 경우문제가되면 nCountReport를 0으로 하고 추가하기.
									{
										//									if((m_structDataEditor.m_bSelectDropMode == TRUE) && (ThreadStage.JobModeSelection == 0)) //Flexible Line Drop && Auto Drop
										//									{
										//										m_nTotalCount[pat][head] = m_nValidDropPoint[pat][head]; //보고는 무조건 정상인것으로...임시...//
										//									}
										//									nCountReport = 0;
										for(int k=0;k<m_structDataEditor.m_nPatternNum;k++)
										{
											nCountReport += m_nValidDropPoint[k][head]; //m_nTotalCount[pat][head];	//사용 안할 경우 유효 Drop(m_nValidDropPoint[pat][head])에서 실 Drop올 변경함. 		
										}
										ValDropTot+=nCountReport;
									}
									else if(bDropEnable[pat][head] ==FALSE) // by ckh 2009.03.09 추가함. Drop Skip 인 패널일 경우 
									{
										//m_nTotalCount[pat][head] = 9999; // 최대값으로 함. skip 패널
										nCountReport= 9999; //m_nTotalCount[pat][head];
										ValDropTot=nCountReport; // skip 패널의 경우 맥스 값을 보내기 위해 각 조건 별로 추가함.
									}														
									else //if(m_bDropCountSensorUSE[head] !=TRUE) // Drop Sensor 사용 안할 경우문제가되면 nCountReport를 0으로 하고 추가하기.
									{
										//									if((m_structDataEditor.m_bSelectDropMode == TRUE) && (ThreadStage.JobModeSelection == 0)) //Flexible Line Drop && Auto Drop
										//									{
										//										m_nTotalCount[pat][head] = m_nValidDropPoint[pat][head]; //보고는 무조건 정상인것으로...임시...//
										//									}
										//									nCountReport = 0;
										nCountReport= m_nValidDropPoint[pat][head]; //m_nTotalCount[pat][head];	//사용 안할 경우 유효 Drop(m_nValidDropPoint[pat][head])에서 실 Drop올 변경함. 		
										ValDropTot+=nCountReport;
									}																		
									//	k++;
								}
								else
								{//2010.01.28 by tskim 
									m_nTotalCount[pat][head] = m_nFlexibleDropCount[head];//m_nValidDropPoint[pat][head]; //보고는 무조건 정상인것으로...임시...//
									nCountReport=m_nTotalCount[pat][head];	//사용 안할 경우 유효 Drop(m_nValidDropPoint[pat][head])에서 실 Drop올 변경함. 		
									ValDropTot+=nCountReport;
								}
							}
							else
							{
								if(m_structDataEditor.m_bNgDropping != TRUE)
								{
									// Drop Count Sensor 사용할 경우 미드롭 패널일 경우는 0, 그외는 실 드롭으로 보고함.
									if(m_bDropCountSensorUSE[head] ==TRUE && bDropEnable[pat][head] ==TRUE) // by ckh 2009.03.09 수정함. Drop Count Sensor 사용 시 && Drop 할 경우.
									{
										if(fabs(m_nValidDropPoint[pat][head] - m_nTotalCount[pat][head]) <= 3)
										{
											m_nTotalCount[pat][head] = m_nValidDropPoint[pat][head];
										}
										nCountReport=m_nTotalCount[pat][head];
										ValDropTot+=nCountReport;
									}
									else if(bDropEnable[pat][head] ==FALSE) // by ckh 2009.03.09 추가함. Drop Skip 인 패널일 경우 
									{
										//m_nTotalCount[pat][head] = 9999; // 최대값으로 함. skip 패널
										nCountReport=9999; //m_nTotalCount[pat][head];
										ValDropTot=nCountReport; // skip 패널의 경우 맥스 값을 보내기 위해 각 조건 별로 추가함.
									}														
									else //if(m_bDropCountSensorUSE[head] !=TRUE) // Drop Sensor 사용 안할 경우문제가되면 nCountReport를 0으로 하고 추가하기.
									{
										if((m_structDataEditor.m_bSelectDropMode == TRUE) && (ThreadStage.JobModeSelection == 0)) //Flexible Line Drop && Auto Drop
										{
											m_nTotalCount[pat][head] = m_nValidDropPoint[pat][head]; //보고는 무조건 정상인것으로...임시...//
										}	
										nCountReport=m_nTotalCount[pat][head];	//사용 안할 경우 유효 Drop(m_nValidDropPoint[pat][head])에서 실 Drop올 변경함. 		
										ValDropTot+=nCountReport;
									}																		
									//	k++;
								}
								else
								{
									m_nTotalCount[pat][head] = m_nValidDropPoint[pat][head]; //보고는 무조건 정상인것으로...임시...//
									nCountReport=m_nTotalCount[pat][head];	//사용 안할 경우 유효 Drop(m_nValidDropPoint[pat][head])에서 실 Drop올 변경함. 		
									ValDropTot+=nCountReport;
								}
							}
						}
					}				
				}
				CIM_Information.DropPoint[i]=ValDropTot;
			}
			else CIM_Information.DropPoint[i]=-1;
		}
	}
	else
	{
		for( i=0;i<MAX_CELL;i++)			
		{
		//	pat = nPattern_num;// by ckh 2009.03.09 추가함. Pattern 별로 Skip 패널인지를 확인 할 수 있으므로
			ValDropTot = 0;
			if(i<CIM_Information.Panal_Max_Num)
			{	
				for( pat = 0 ; pat < m_structDataEditor.m_nPatternNum ; pat++)// by ckh 2009.03.09 해당 패턴만 갱신하도록함.
				{
					for( head = 0 ; head < MAX_NOZZLE ; head++)
					{
						if( (m_structPatternData[pat].m_nSequence > 0) && (m_structPatternData[pat].m_nPanelNo[head] == i + 1) )
						{
	//2009.12.08 by tskim						
							if(/*(m_structDataEditor.m_bSelectDropMode == TRUE) && */(ThreadStage.JobModeSelection == 0)) //Flexible Line Drop && Auto Drop //150622 General Drop 추가
							{
								if(m_structDataEditor.m_bNgDropping != TRUE)
								{
									// Drop Count Sensor 사용할 경우 미드롭 패널일 경우는 0, 그외는 실 드롭으로 보고함.
									if(m_structDataEditor.m_bUse_Vision && bDropEnable[pat][head] ==TRUE) // by ckh 2009.03.09 수정함. Drop Count Sensor 사용 시 && Drop 할 경우.
									{
	//									if((m_structDataEditor.m_bSelectDropMode == TRUE) && (ThreadStage.JobModeSelection == 0)) //Flexible Line Drop && Auto Drop
	//									{
	//										if(CIM_Information.bNGCell[m_structPatternData[pat].m_nPanelNo[head]-1])
	//										{
	//											//2009.12.17 by tskim 
	//											//if(pat != patOld && head != headOld)
	//											//{
	//											//m_nTotalCount[pat][head] = m_nValidDropPoint[pat][head]; //보고는 무조건 정상인것으로...임시...//
	//											//2010.05.17 by tskim
	//											//m_nTotalCount[pat][head] = m_nFlexibleDropCount[head];
	//											//2009.11.23 by tskim  실처리 보고 부분 수정...
	//											//								if(fabs(m_nFlexibleDropCount[head]/*m_nValidDropPoint[pat][head]*/ - m_nTotalCount[pat][head]) <= 3*m_structDataEditor.m_nPatternNum)
	//											//								{
	//											//									m_nTotalCount[LinePattern_Seq_count-1][head] = m_nFlexibleDropCount[head]; // m_nValidDropPoint[pat][head];
	//											//								}
	//											nCountReport=m_nTotalCount[pat][head];
	//											ValDropTot+=nCountReport;
	//											//}
	//											//else
	//											//{
	//											//	nCountReport=0;
	//											//	ValDropTot=nCountReport;
	//											//}
	//										}
	//										else
	//										{
	//											//m_nTotalCount[pat][head] = 9999; // 최대값으로 함. skip 패널
	//											nCountReport= 9999; //m_nTotalCount[pat][head];
	//											ValDropTot=nCountReport; 
	//										}
	//										headOld = head;
	//										patOld = pat;
	//
	//									}
	//									else
	//									{
										//ValDropTot = _NVision.m_structCount_Result.m_nCell_Drop_Count[i]/N_MAX_NOZZLE;

										//MISS COUNT 만 표시되도록.... BY SHLEE
										ValDropTot = _NVision.m_structCount_Result.m_nMiss_Count[i];
										//if(fabs(m_nValidDropPoint[pat][head] - m_nTotalCount[pat][head]) <= 3)
										//{
										//	m_nTotalCount[pat][head] = m_nValidDropPoint[pat][head];
										//}
										//nCountReport=m_nTotalCount[pat][head];
										//ValDropTot+=nCountReport;
	//									}
									}
									else if(m_structDataEditor.m_bUse_Vision !=TRUE && bDropEnable[pat][head] ==TRUE) // Drop Sensor 사용 안할 경우문제가되면 nCountReport를 0으로 하고 추가하기.
									{
	//									if((m_structDataEditor.m_bSelectDropMode == TRUE) && (ThreadStage.JobModeSelection == 0)) //Flexible Line Drop && Auto Drop
	//									{
											m_nTotalCount[pat][head] = m_nValidDropPoint[pat][head]; //보고는 무조건 정상인것으로...임시...//
	//									}
										nCountReport = 0;
	//									for(int k=0;k<m_structDataEditor.m_nPatternNum;k++)
	//									{
	//										nCountReport += m_nValidDropPoint[k][head]; //m_nTotalCount[pat][head];	//사용 안할 경우 유효 Drop(m_nValidDropPoint[pat][head])에서 실 Drop올 변경함. 		
	//									}
	//									ValDropTot+=nCountReport;
										ValDropTot += m_nTotalCount[pat][head];
									}
									else if(bDropEnable[pat][head] ==FALSE) // by ckh 2009.03.09 추가함. Drop Skip 인 패널일 경우 
									{
										//m_nTotalCount[pat][head] = 9999; // 최대값으로 함. skip 패널
										nCountReport= 9999; //m_nTotalCount[pat][head];
										ValDropTot=nCountReport; // skip 패널의 경우 맥스 값을 보내기 위해 각 조건 별로 추가함.
									}														
									else //if(m_bDropCountSensorUSE[head] !=TRUE) // Drop Sensor 사용 안할 경우문제가되면 nCountReport를 0으로 하고 추가하기.
									{
	//									if((m_structDataEditor.m_bSelectDropMode == TRUE) && (ThreadStage.JobModeSelection == 0)) //Flexible Line Drop && Auto Drop
	//									{
	//										m_nTotalCount[pat][head] = m_nValidDropPoint[pat][head]; //보고는 무조건 정상인것으로...임시...//
	//									}
	//									nCountReport = 0;
										nCountReport= m_nValidDropPoint[pat][head]; //m_nTotalCount[pat][head];	//사용 안할 경우 유효 Drop(m_nValidDropPoint[pat][head])에서 실 Drop올 변경함. 		
										ValDropTot+=nCountReport;
									}																		
									//	k++;
								}
								else
								{//2010.01.28 by tskim 
									//m_nTotalCount[pat][head] = m_nFlexibleDropCount[head];//m_nValidDropPoint[pat][head]; //보고는 무조건 정상인것으로...임시...//
									//nCountReport=m_nTotalCount[pat][head];	//사용 안할 경우 유효 Drop(m_nValidDropPoint[pat][head])에서 실 Drop올 변경함. 		
									//ValDropTot+=nCountReport;

									//ValDropTot = _NVision.m_structCount_Result.m_nCell_Drop_Count[i]/N_MAX_NOZZLE;
									//MISS COUNT 만 표시되도록.... BY SHLEE
									ValDropTot = _NVision.m_structCount_Result.m_nMiss_Count[i];
								}
							}
							else
							{
								if(m_structDataEditor.m_bNgDropping != TRUE)
								{
									// Drop Count Sensor 사용할 경우 미드롭 패널일 경우는 0, 그외는 실 드롭으로 보고함.
									if(m_bDropCountSensorUSE[head] ==TRUE && bDropEnable[pat][head] ==TRUE) // by ckh 2009.03.09 수정함. Drop Count Sensor 사용 시 && Drop 할 경우.
									{
										if(fabs(m_nValidDropPoint[pat][head] - m_nTotalCount[pat][head]) <= 3)
										{
											m_nTotalCount[pat][head] = m_nValidDropPoint[pat][head];
										}
										nCountReport=m_nTotalCount[pat][head];
										ValDropTot+=nCountReport;
									}
									else if(bDropEnable[pat][head] ==FALSE) // by ckh 2009.03.09 추가함. Drop Skip 인 패널일 경우 
									{
										//m_nTotalCount[pat][head] = 9999; // 최대값으로 함. skip 패널
										nCountReport=9999; //m_nTotalCount[pat][head];
										ValDropTot=nCountReport; // skip 패널의 경우 맥스 값을 보내기 위해 각 조건 별로 추가함.
									}														
									else //if(m_bDropCountSensorUSE[head] !=TRUE) // Drop Sensor 사용 안할 경우문제가되면 nCountReport를 0으로 하고 추가하기.
									{
										if((m_structDataEditor.m_bSelectDropMode == TRUE) && (ThreadStage.JobModeSelection == 0)) //Flexible Line Drop && Auto Drop
										{
											m_nTotalCount[pat][head] = m_nValidDropPoint[pat][head]; //보고는 무조건 정상인것으로...임시...//
										}	
										nCountReport=m_nTotalCount[pat][head];	//사용 안할 경우 유효 Drop(m_nValidDropPoint[pat][head])에서 실 Drop올 변경함. 		
										ValDropTot+=nCountReport;
									}																		
									//	k++;
								}
								else
								{
									m_nTotalCount[pat][head] = m_nValidDropPoint[pat][head]; //보고는 무조건 정상인것으로...임시...//
									nCountReport=m_nTotalCount[pat][head];	//사용 안할 경우 유효 Drop(m_nValidDropPoint[pat][head])에서 실 Drop올 변경함. 		
									ValDropTot+=nCountReport;
								}
							}

						}
					}				
				}
				CIM_Information.DropPoint[i]=ValDropTot;
			}
			else CIM_Information.DropPoint[i]=-1;
		}
	}


//2011.03.01 by tskim
	for( head = 0 ; head < MAX_NOZZLE ; head++)
		HeadTemp[head] = 0;
	
	for( pat = 0 ; pat < m_structDataEditor.m_nPatternNum ; pat++)// by ckh 2009.03.09 해당 패턴만 갱신하도록함.
	{
		for( head = 0 ; head < MAX_NOZZLE ; head++)
		{
//			if(m_nTotalCount[pat][head] != 9999)
			HeadTemp[head] += m_nTotalCount[pat][head];
		}
	}

	if(m_structDataEditor.m_nNzlMode != 1)//spd경우만 처리...
	{
		for( i=0;i<MAX_PANEL;i++)			
		{
			ValDropTot = 0;
			CIM_Information.DropPoint[i] = -1;
			if(i<CIM_Information.Panal_Max_Num)
			{
				for( pat = 0 ; pat < m_structDataEditor.m_nPatternNum ; pat++)// by ckh 2009.03.09 해당 패턴만 갱신하도록함.
				{	
					for( head = 0 ; head < MAX_NOZZLE ; head++)
					{
						if( (m_structPatternData[pat].m_nSequence > 0) && (m_structPatternData[pat].m_nPanelNo[head] == i + 1) )
						{
							//2009.12.08 by tskim						
							if((m_structDataEditor.m_bSelectDropMode == TRUE) && (ThreadStage.JobModeSelection == 0)) //Flexible Line Drop && Auto Drop
							{
								if(m_structDataEditor.m_bNgDropping != TRUE)
								{
									// Drop Count Sensor 사용할 경우 미드롭 패널일 경우는 0, 그외는 실 드롭으로 보고함.
									if(!m_structDataEditor.m_bUse_Vision)
									{
										if(m_bDropCountSensorUSE[head] ==TRUE && bDropEnable[pat][head] ==TRUE) // by ckh 2009.03.09 수정함. Drop Count Sensor 사용 시 && Drop 할 경우.
										{
											if((m_structDataEditor.m_bSelectDropMode == TRUE) && (ThreadStage.JobModeSelection == 0)) //Flexible Line Drop && Auto Drop
											{
												if(CIM_Information.bNGCell[m_structPatternData[pat].m_nPanelNo[head]-1])
												{
													CIM_Information.DropPoint[i] += HeadTemp[head];
												}
												else
												{
													CIM_Information.DropPoint[i] = 9999;
												}
											}
										}
									}
									else//Vision 사용의 경우 처리...
									{

									}
								}
							}
						}
					}
				}
			}
		}
	}

/*	CString strCh,strOld;
	strCh.Empty();
	strOld.Empty();
	int SameIDcnt = 0;
	int DropCountTemp[96];
	for( i=0;i<96;i++)			
	{
		DropCountTemp[i] = CIM_Information.DropPoint[i];
		CIM_Information.DropPoint[i]=0;
		if(i<CIM_Information.Panal_Max_Num)
		{
			if(i==0) 
			{
				strCh = m_strGlassIden[i].Mid(0,1);
				strOld = strCh;
				SameIDcnt++;
				continue;
			}
			else
			{
				strCh = m_strGlassIden[i].Mid(0,1);
				if(strOld == strCh)
				{
					SameIDcnt++;
				}
			}
		}
	}

	for(i=0;i<CIM_Information.Panal_Max_Num/SameIDcnt;i++)
	{
		j=0;
		for(j=0;j<SameIDcnt;j++)
		{
//			CIM_Information.DropPoint[j+(i*SameIDcnt)]=0;
			if(DropCountTemp[j+(i*SameIDcnt)] == 9999)
			{
				CIM_Information.DropPoint[j+(i*SameIDcnt)] = 9999;
			}
			else
			{
				CIM_Information.DropPoint[j+(i*SameIDcnt)] = DropCountTemp[j+(i*SameIDcnt)];
				j=SameIDcnt;
				break;
			}
		}
	}
*/	
//2009.12.17 by tskim Line Drop Display 관련 
//2011.03.01 by tskim
// 	int IDLength;
// 	int DropCountTemp[96];
// 	if((m_structDataEditor.m_bSelectDropMode == TRUE) && (ThreadStage.JobModeSelection == 0))
// 	{
// 		for(i=0;i<CIM_Information.Panal_Max_Num;i++)
// 		{
// //2010.01.28 by tskim
// //			if(m_structDataEditor.m_bNgDropping == TRUE)
// //			{
// //
// //			}
// 			DropCountTemp[i] = 0;
// 			IDLength = m_strGlassIden[i].GetLength();
// 			
// 			if(CIM_Information.bNGCell[i] == TRUE)
// 			{
// 				if(atoi(m_strGlassIden[i].Mid(IDLength-1,1)) == 1)
// 				{
// 					DropCountTemp[i] = CIM_Information.DropPoint[i];
// 				}
// 				else
// 				{
// 					if(i>1)
// 					{
// 						if(CIM_Information.bNGCell[i-1] == FALSE && CIM_Information.bNGCell[i-2] == FALSE )
// 						{
// 							DropCountTemp[i] = CIM_Information.DropPoint[i];
// 						}
// 					}
// 				}
// 			}
// 			else
// 			{
// 				DropCountTemp[i] = 9999;
// 				if(atoi(m_strGlassIden[i].Mid(IDLength-1,1)) == 1)
// 				{
// 					i++;
// 					DropCountTemp[i] = CIM_Information.DropPoint[i];
// 				}
// 			}
// 			CIM_Information.DropPoint[i] = DropCountTemp[i];
// // 			CString strTemp;
// // 			strTemp.Format("%d",CIM_Information.DropPoint[i]);
// // 			AfxMessageBox(strTemp);
//  		}
// 
// 	}


	// 보고해야할 Data의 지정 Address를 맞추기 위해 재배열한다.
	// 미리 -1을 넣는 이유는 원래 정상적인 유효 Drop수(혹은 Drop Count수)는 0보다 큰 수이므로
	// 만약 처음에 할당한 -1의 값이 변하지 않았다면 공백으로 처리해야 할 Panel이라는 뜻이 될 수 있다.
//	for(i=0;i<88;i++)	CIM_Information.Worked_Report_DropPoint[i]=-1;
	for(i=0;i<MAX_PANEL;i++)	CIM_Information.Worked_Report_DropPoint[i]=-1;

//	for(i=0;i<88;i++)
	for(i=0;i<MAX_PANEL;i++)
	{
		nBaseIndex=100;	// 공백으로 처리해야 할 Panel들 구분.
		if(i<CIM_Information.Panal_Max_Num)	// 사용하지 않는 Panel에 대한 정보가 Overwrite되지 않도록 조건을 건다.
		{
			if(m_strGlassIden[i]!="")
			{
				// 8개 영역
				//if ( m_strGlassIden[i].GetAt(1) < 9  ) 
				char chtemp = m_strGlassIden[i].GetAt(1);
				if ( 48 < chtemp &&  chtemp < 58  ) //1~9
				{
					switch(m_strGlassIden[i].GetAt(0))	//150625 HSN case 'I' 추가
					{
						case 'A':	nBaseIndex=0;	break;  // 8
						case 'B':	nBaseIndex=1;	break;  // 8
						case 'C':	nBaseIndex=2;	break;  // 8
						case 'D':	nBaseIndex=3;	break;  // 8
						case 'E':	nBaseIndex=4;	break;  // 8
						case 'F':	nBaseIndex=5;	break;  // 8
						case 'G':	nBaseIndex=6;	break;  // 8
						case 'H':	nBaseIndex=7;	break;  // 8
						case 'I':	nBaseIndex=8;	break;  // 8
						case 'J':	nBaseIndex=9;	break;  // 8
					}
				}
				// 12개 또는 20개 영역 
				else 
				{
					switch(m_strGlassIden[i].GetAt(0))	//150625 HSN case 'I' 추가
					{
						case 'A':	nBaseIndex=10;	break;  // 12
						case 'B':	nBaseIndex=11;	break;  // 12
						case 'C':	nBaseIndex=12;	break;  // 12
						case 'D':	nBaseIndex=13;	break;  // 12
						case 'E':	nBaseIndex=14;	break;  // 12
						case 'F':	nBaseIndex=15;	break;  // 12
						case 'G':	nBaseIndex=16;	break;  // 12
						case 'H':	nBaseIndex=17;	break;  // 12
						case 'I':	nBaseIndex=18;	break;  // 12
						case 'J':	nBaseIndex=19;	break;  // 12
						case 'K':	nBaseIndex=20;	break;	// 20
						case 'L':	nBaseIndex=21;	break;	// 20
						case 'M':	nBaseIndex=22;	break;	// 20
						case 'N':	nBaseIndex=23;	break;  // 20
						case 'O':	nBaseIndex=24;	break;	// 20
					}
				}
			}
		}

		if(nBaseIndex != 100)		// 이걸 안해 주면 배열의 할당에 문제가 생김.
		{
			nOffsetAddress = atoi((char *)(LPCSTR)m_strGlassIden[i].Mid(1,1))-1;

			if ( nBaseIndex < 10 )
			{
				ArrIndex = nBaseIndex * 8  + nOffsetAddress;
			}
			else if ( nBaseIndex < 20)
			{
				// 80 : 이전 Index 총 개수 
				// nBaseIndex - 10 : 이전 Glass Iden 개수
				// 12 : ID 당 Cell 개수 
				ArrIndex = 80 + ( ( nBaseIndex - 10 ) * 12 ) + nOffsetAddress;
			}
			else
			{
				// 80 + 120 : 이전 Index 총 개수 
				// nBaseIndex - 20 : 이전 Glass Iden 개수 
				// 20 : ID 당 Cell 개수  
				ArrIndex = 80 + 120 + ( ( nBaseIndex - 20 ) * 20 ) + nOffsetAddress;
			}
			
			CIM_Information.Worked_Report_DropPoint[ArrIndex] = CIM_Information.DropPoint[i];			
		}
	}
}
///////////////////////////////////////

int CP8CA_LcDispDoc::StrtoValuePanelInform(CString strPanelInform, int PanelType)
{
	if(PanelType==TFT)
	{
		if(strPanelInform.GetAt(TFT)=='G')		return 1;
		else if(strPanelInform.GetAt(TFT)=='Q')	return 9;
		else if(strPanelInform.GetAt(TFT)=='N')	return 2;
		else if(strPanelInform.GetAt(TFT)=='W')	return 17; //10
		else if(strPanelInform.GetAt(TFT)=='Y')	return 24; //10
		else return 1;
	}
	else if(PanelType==CF)
	{
		if(strPanelInform.GetAt(CF)=='G')		return 1;
		else if(strPanelInform.GetAt(CF)=='Q')	return 9;
		else if(strPanelInform.GetAt(CF)=='N')	return 2;
		else if(strPanelInform.GetAt(CF)=='W')	return 17; //10
		else if(strPanelInform.GetAt(CF)=='X')	return 23; //10
		else return	1;
	}
	return	1;
}

void CP8CA_LcDispDoc::Save_SpecoutCount()
{
	FILE *fp;
	int i = 0,j=0;
	CString strPath ="";
	strPath.Format("%s\\Spec_Out_Count.dat", m_strDataPath);
	fp = fopen(strPath, "wt");
	if(fp != NULL)
	{
		fprintf(fp, "%d\n", m_bMovingSpecCount);
		fprintf(fp, "%d\n", m_nSpecTime);
		fprintf(fp, "%s\n", m_strSpecTime);
		for(i = 0; i < MAX_NOZZLE ; i++)
			fprintf(fp, "%d\t", m_nSpecCount[i]);
		fprintf(fp,"\n");
		for(i = 0; i < MAX_NOZZLE ; i++)
			fprintf(fp, "%d\t", m_nRealSpecCount[i]);
		fprintf(fp,"\n");
		fprintf(fp,"\n");
		for(i=0; i<2; i++)
		{
			for(j=0; j< MAX_NOZZLE; j++)
			{
				fprintf(fp, "%.0f\n", m_dDropCountErrorRange_per[i][j]);
			}
		}
		fprintf(fp,"\n");
		// 화일 닫기
	//	fclose(fp);
	}
	// 화일 닫기
	fclose(fp);

}

void CP8CA_LcDispDoc::Read_SpecoutCount()
{
	ifstream fi;
	CString strPathName;
	char ch[256];
	int i=0,j=0;
	
	strPathName.Format("%s\\Spec_Out_Count.dat", m_strDataPath);
	fi.open((char *)(LPCSTR)strPathName, ios::in);

	if(fi.is_open())
	{
		// 측정data를 이용해서 계산된 coefficient
		fi >> m_bMovingSpecCount;
		fi >> m_nSpecTime;
		fi >> ch;
		m_strSpecTime.Format("%s",ch);
		if(m_strSpecTime.GetLength() != 10) m_strSpecTime = "0000000000";
		for(i=0 ; i<MAX_NOZZLE ;i++)
			fi >> m_nSpecCount[i];
		for(i=0 ; i<MAX_NOZZLE ;i++)
			fi >> m_nRealSpecCount[i];
		for(i=0; i<2; i++)
		{
			for(j=0; j< MAX_NOZZLE; j++)
			{
				fi >> m_dDropCountErrorRange_per[i][j];
			}
		}
	}
	else
	{
		fi.close();
		return;
	}
	fi.close();
}

BOOL CP8CA_LcDispDoc::SaveBarcodeData()
{
	FILE *fp;
	CString strPathName;

	strPathName.Format("%s\\Barcode.dat", m_strDataPath);
	fp = fopen((char *)(LPCSTR)strPathName, "wt");

	if(fp==NULL)
	{
		fclose(fp);
		return FALSE;
	}
	else
	{
		for(int i=0;i<MAX_NOZZLE;i++)
			fprintf(fp, "%s\n",CIM_Information.sBarcodeData[i]);
		fclose(fp);
		return TRUE;
	}
}

BOOL CP8CA_LcDispDoc::ReadBarcodeData()
{
	ifstream fi;
	CString strPathName;
//2008.01.10 by tskim 
//BarCode Save Error관련 수정..256->512
	char ch[512];	

	strPathName.Format("%s\\Barcode.dat", m_strDataPath);
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);

	if(fi.is_open())
	{
		for(int i = 0 ; i < MAX_NOZZLE ; i++)
		{
			fi >> ch;
			CIM_Information.sBarcodeData[i].Format("%s", ch);
		}
	}
	else
	{
		fi.close();
		return FALSE;
	}

	fi.close();
	return TRUE;
}

BOOL CP8CA_LcDispDoc::FolderCopy(CString strFrom, CString strTo)
{
   TCHAR fromPath[MAX_PATH+2]; 

    memset(fromPath, 0, sizeof(fromPath));

    strcpy(fromPath, strFrom);

 

    TCHAR toPath[MAX_PATH+2];   

    memset(toPath, 0, sizeof(toPath));

    strcpy(toPath, strTo);

 

    SHFILEOPSTRUCT shfo = {0};

    ZeroMemory(&shfo, sizeof shfo);

    shfo.hwnd = AfxGetMainWnd()->m_hWnd;

    shfo.wFunc = FO_COPY;

    shfo.fFlags = NULL;// FOF_SILENT | FOF_NOERRORUI | FOF_NOCONFIRMATION;

    shfo.lpszProgressTitle="폴더 복사";

    shfo.fAnyOperationsAborted = false;

    shfo.pTo = toPath;

    shfo.pFrom=fromPath;

 

    int nResult = SHFileOperation(&shfo);   

 

    if( nResult == 0 )  return TRUE;

 

    return FALSE;
}


//SPD
//by shin//2008.07.24//
BOOL CP8CA_LcDispDoc::ReadSPDData()
{
	ifstream fi;
	CString strPathName;

	strPathName.Format("%s\\SPD.dat", m_strDataPath);
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);

	if(fi.is_open())
	{
		for(int i=0;i<MAX_NOZZLE;i++)
		{
			fi >> Drop_Info.m_dAdjustAbsPos[i];
			fi >> Drop_Info.m_dBasePos[i];
			fi >> Drop_Info.m_dAssemPos[i];
			fi >> Drop_Info.m_dFullBasePos[i];
			fi >> Drop_Info.m_dRemoveBubblePos[i];
			fi >> Drop_Info.m_dDistPerDrop[i];
			fi >> Drop_Info.m_nSuctionSpeed[i];
			fi >> Drop_Info.m_nDischargeSpeed[i];
			fi >> Drop_Info.m_dMMPerVolume[i];
			fi >> Drop_Info.m_nSuctionOffset[i];
			fi >> Drop_Info.m_nGateOffset[i];
			fi >> Drop_Info.m_dSucOffSpeed[i];
			fi >> Drop_Info.m_nRemoveBubbleTimes1[i];
			fi >> Drop_Info.m_nRemoveBubbleTimes2[i];
		}
		//by shin//2013.01.15//n-type add//
		for(i=0;i<MAX_NOZZLE;i++)
		{
			fi >> Drop_Info.m_dNRemoveBubblePos[i];
			fi >> Drop_Info.m_dNDischargeSpeed1[i];
			fi >> Drop_Info.m_dNDischargeSpeed2[i];
			fi >> Drop_Info.m_nNRemoveBubbleTimes[i];
			fi >> Drop_Info.m_nNOverTime[i];
		}
		fi >> Drop_Info.lSupplyAcctime;
		fi >> Drop_Info.lSupplyDectime;
		fi >> Drop_Info.lSupplyStartSpeed;
		fi >> Drop_Info.lDegasAcctime;
		fi >> Drop_Info.lDegasDectime;
		fi >> Drop_Info.lDegasStartSpeed;
		fi >> Drop_Info.m_nNAgingCount;
		fi >> Drop_Info.m_nNAgingCycle; //20210224 him

	}
	else
		return FALSE;

	fi.close();
	return TRUE;
}

BOOL CP8CA_LcDispDoc::SaveSPDData()
{
	FILE *fp;
	CString strPathName;

	strPathName.Format("%s\\SPD.dat", m_strDataPath);
	fp = fopen((char *)(LPCSTR)strPathName, "wt");

	if(fp==NULL) return FALSE;
	else
	{
		for(int i=0;i<MAX_NOZZLE;i++)
		{
			fprintf(fp, "%.5f\n",Drop_Info.m_dAdjustAbsPos[i]);
			fprintf(fp, "%.3f\n",Drop_Info.m_dBasePos[i]);
			fprintf(fp, "%.3f\n",Drop_Info.m_dAssemPos[i]);

//			if(m_structDataEditor.m_dSuctionVolumn[i]>=300)
//				Drop_Info.m_dFullBasePos[i] = Drop_Info.m_dBasePos[i] - (BASE_PULSE*m_structDataEditor.m_dSuctionVolumn[i]); //PUMP_LIMIT
//			else
//2014.11.01 by tskim Editor서 120%를 고려 했을 것이므로..여기서 생략 
				Drop_Info.m_dFullBasePos[i] = Drop_Info.m_dBasePos[i] - (BASE_PULSE*m_structDataEditor.m_dSuctionVolumn[i]); //PUMP_LIMIT

			//P축 오차 발생 고려 하여 5%더 준다..
			fprintf(fp, "%.3f\n",Drop_Info.m_dFullBasePos[i]);
			fprintf(fp, "%.5f\n",Drop_Info.m_dRemoveBubblePos[i]);
			fprintf(fp, "%.5f\n",Drop_Info.m_dDistPerDrop[i]);
			fprintf(fp, "%d\n",Drop_Info.m_nSuctionSpeed[i]);
			fprintf(fp, "%d\n",Drop_Info.m_nDischargeSpeed[i]);
			fprintf(fp, "%.5f\n",Drop_Info.m_dMMPerVolume[i]);
//			Drop_Info.m_nSuctionOffset[i] = 0;
			fprintf(fp, "%d\n",Drop_Info.m_nSuctionOffset[i]);
			fprintf(fp, "%d\n",Drop_Info.m_nGateOffset[i]);
			fprintf(fp, "%.1f\n",Drop_Info.m_dSucOffSpeed[i]);
			fprintf(fp, "%d\n",Drop_Info.m_nRemoveBubbleTimes1[i]);
			fprintf(fp, "%d\n",Drop_Info.m_nRemoveBubbleTimes2[i]);
		}
		//by shin//2013.01.15//n-type add//
		for(i=0;i<MAX_NOZZLE;i++)
		{
			fprintf(fp, "%.5f\n",Drop_Info.m_dNRemoveBubblePos[i]);
			fprintf(fp, "%.0f\n",Drop_Info.m_dNDischargeSpeed1[i]);
			fprintf(fp, "%.0f\n",Drop_Info.m_dNDischargeSpeed2[i]);
			fprintf(fp, "%d\n",Drop_Info.m_nNRemoveBubbleTimes[i]);
			fprintf(fp, "%d\n",Drop_Info.m_nNOverTime[i]);
		}
		fprintf(fp, "%d\n",Drop_Info.lSupplyAcctime);
		fprintf(fp, "%d\n",Drop_Info.lSupplyDectime);
		fprintf(fp, "%d\n",Drop_Info.lSupplyStartSpeed);
		fprintf(fp, "%d\n",Drop_Info.lDegasAcctime);
		fprintf(fp, "%d\n",Drop_Info.lDegasDectime);
		fprintf(fp, "%d\n",Drop_Info.lDegasStartSpeed);
		fprintf(fp, "%d\n",Drop_Info.m_nNAgingCount);
		fprintf(fp, "%d\n",Drop_Info.m_nNAgingCycle); //20210224 him
		fclose(fp);
		return TRUE;
	}
}

BOOL CP8CA_LcDispDoc::CanCloseFrame(CFrameWnd* pFrame) 
{
	// TODO: Add your specialized code here and/or call the base class
#if _VISION	
	grabber.Grab(MUX1);
	grabber.Grab(MUX2);
	grabber.Close();
#endif
	
	return CDocument::CanCloseFrame(pFrame);
}

BOOL CP8CA_LcDispDoc::SaveAccuracyMotorPos()
{
	FILE *fp;
	CString strPathName;

	strPathName.Format("%s\\Accuracy_Motor_Pos.dat", m_strDataPath);
	fp = fopen((char *)(LPCSTR)strPathName, "wt");

	if(fp==NULL) return FALSE;
	else
	{
		for(int i=0;i<MAX_NOZZLE;i++)
		{
			fprintf(fp, "%.3f\n",m_dAccuracyPosX[i]);
			fprintf(fp, "%.3f\n",m_dAccuracyPosY[i]);
		}
		fclose(fp);
		return TRUE;
	}
}

BOOL CP8CA_LcDispDoc::ReadAccuracyMotorPos()
{
	ifstream fi;
	CString strPathName;

	strPathName.Format("%s\\Accuracy_Motor_Pos.dat", m_strDataPath);
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);

	if(fi.is_open())
	{
		for(int i=0;i<MAX_NOZZLE;i++)
		{
			fi >> m_dAccuracyPosX[i];
			fi >> m_dAccuracyPosY[i];
		}
	}
	else
		return FALSE;

	fi.close();
	return TRUE;
}

BOOL CP8CA_LcDispDoc::SaveMotorMoveDistance()
{
	FILE *fp;
	CString strPathName;

	strPathName.Format("%s\\MotorDistance.dat", m_strDataPath);
	fp = fopen((char *)(LPCSTR)strPathName, "wt");

	if(fp==NULL) return FALSE;
	else
	{
		for(int i = 0; i < 2; i++)
		{
			for(int j = 0; j < 2; j++)
			{
				fprintf(fp, "%.5f\n", m_dMotorMoveDistance[i][j]);
			}
		}
		fclose(fp);
		return TRUE;
	}
}

BOOL CP8CA_LcDispDoc::ReadMotorMoveDistance()
{
	ifstream fi;
	CString strPathName;

	strPathName.Format("%s\\MotorDistance.dat", m_strDataPath);
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);

	if(fi.is_open())
	{
		for(int i = 0; i < 2; i++)
		{
			for(int j = 0; j < 2; j++)
			{
				fi >> m_dMotorMoveDistance[i][j];
			}
		}
	}
	else
		return FALSE;

	fi.close();
	return TRUE;
}

BOOL CP8CA_LcDispDoc::SaveSupplyWeight()
{
	FILE *fp;
	CString strPathName;

	strPathName.Format("%s\\CanisterSupplyWeight.dat", m_strDataPath);
	fp = fopen((char *)(LPCSTR)strPathName, "wt");

	if(fp==NULL) return FALSE;
	else
	{
		for(int i = 0; i < 4; i++)
		{
			fprintf(fp, "%.0f\n", m_dCanisterSupplyWeignt[i]);
		}
		fclose(fp);
		return TRUE;
	}
}

BOOL CP8CA_LcDispDoc::ReadSupplyWeight()
{
	ifstream fi;
	CString strPathName;

	strPathName.Format("%s\\CanisterSupplyWeight.dat", m_strDataPath);
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);

	if(fi.is_open())
	{
		for(int i = 0; i < 4; i++)
		{
			fi >> m_dCanisterSupplyWeignt[i];
		}
	}
	else
		return FALSE;

	fi.close();
	return TRUE;
}

BOOL CP8CA_LcDispDoc::ReadDummyData()
{
	ifstream fi;
	CString strPathName;
	int i = 0;

	strPathName.Format("%s\\DummyTeach.dat", m_strDataPath);
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);

	if(fi.is_open())
	{
		for(i=0;i<MAX_NOZZLE;i++)
		{
			fi >> m_dDummyTeachData[i];
		}
		for(i=0;i<MAX_NOZZLE;i++)
		{
			fi >> m_dDummyTeachData2[i];
		}
	}
	else
		return FALSE;

	fi.close();
	return TRUE;
}

BOOL CP8CA_LcDispDoc::SaveDummyData()
{
	FILE *fp;
	CString strPathName;
	int i = 0;

	strPathName.Format("%s\\DummyTeach.dat", m_strDataPath);
	fp = fopen((char *)(LPCSTR)strPathName, "wt");

	if(fp==NULL) return FALSE;
	else
	{
		for(i=0;i<MAX_NOZZLE;i++)
		{
			fprintf(fp, "%.3f\n",m_dDummyTeachData[i]);
		}
		for(i=0;i<MAX_NOZZLE;i++)
		{
			fprintf(fp, "%.3f\n",m_dDummyTeachData2[i]);
		}		fclose(fp);
		return TRUE;
	}
}

BOOL CP8CA_LcDispDoc::SaveCanisterData()
{
	FILE *fp;
	CString strPathName;

	strPathName.Format("%s\\CanisterType.dat", m_strDataPath);
	fp = fopen((char *)(LPCSTR)strPathName, "wt");

	if(fp==NULL)
	{
		fclose(fp);
		return FALSE;
	}
	else
	{
		for(int i=0;i<2;i++)
			fprintf(fp, "%s\n",CIM_Information.sCanisterTypeData[i]);
		fclose(fp);
		return TRUE;
	}
}

BOOL CP8CA_LcDispDoc::ReadCanisterTypeData()
{
	ifstream fi;
	CString strPathName;
	char ch[512];	

	strPathName.Format("%s\\CanisterType.dat", m_strDataPath);
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);

	if(fi.is_open())
	{
		for(int i = 0 ; i < 2 ; i++)
		{
			fi >> ch;
			CIM_Information.sCanisterTypeData[i].Format("%s", ch);

			//by shin//2009.05.28//쓰레기값 들어가는거...??//
			if(CIM_Information.sCanisterTypeData[i].GetLength() != 7)
			{
				CIM_Information.sCanisterTypeData[i] = "-";
			}
		}
	}
	else
	{
		fi.close();
		return FALSE;
	}

	fi.close();
	return TRUE;
}

BOOL CP8CA_LcDispDoc::SaveSupplyTimeData()
{
	FILE *fp;
	CString strPathName;
	int i = 0;

	strPathName.Format("%s\\SupplyTime.dat", m_strDataPath);
	fp = fopen((char *)(LPCSTR)strPathName, "wt");

	if(fp==NULL) return FALSE;
	else
	{
		fprintf(fp, "%d\n",m_nInitSupplyTime);
		fprintf(fp, "%d\n",m_nNormalSupplyTime);
		fprintf(fp, "%.2f\n",m_dManuCanAlarmWeight);

		fclose(fp);
		return TRUE;
	}
}

BOOL CP8CA_LcDispDoc::ReadSupplyTimeData()
{
	ifstream fi;
	CString strPathName;
	int i = 0;

	strPathName.Format("%s\\SupplyTime.dat", m_strDataPath);
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);

	if(fi.is_open())
	{
		fi >> m_nInitSupplyTime;
		fi >> m_nNormalSupplyTime;	
		fi >> m_dManuCanAlarmWeight;	
	}
	else
		return FALSE;

	fi.close();
	return TRUE;
}

void CP8CA_LcDispDoc::Read_AutoErrListInformData() //090711 최초 구동시 Auto EList Data Setting 을 위해 추가 
{
	CString strDir,strTime; 
	int nCode=0,nCode2=0,m_nTempFlag=0,m_nMonth,m_nDay,m_nHour,m_nMinite,m_nSecond;

	FILE *fp;
	strDir = "D:\\TOP\\P8CA_LC\\P8CA_LC_EXE\\";
	fp = fopen(strDir+"EList_New.txt","r");

	if( fp == NULL )
	{
		AfxMessageBox("EList_New.txt 가 존재하지 않으므로 표시할 수 없습니다");
		return;
	}

	while(fscanf(fp,"%d	%d	%d	%d	%d	%d	%d	%d\n",&nCode,&nCode2,&m_nTempFlag,&m_nMonth,&m_nDay,&m_nHour,&m_nMinite,&m_nSecond) != EOF)
	{
	
    	Error_Information[m_nCount].m_nCode[0] = nCode;
		Error_Information[m_nCount].m_nCode[1] = nCode2;
		Error_Information[m_nCount].m_nCode[2] = m_nTempFlag; 
		strTime.Format("%02d/%02d %02d:%02d:%02d",m_nMonth,m_nDay,m_nHour,m_nMinite,m_nSecond);
		Error_Information[m_nCount].strTime = strTime;

		m_nCount++;

		if(m_nCount > 49 )
		{
			m_nCount = 49; 
			break;
		}
	}
	fclose(fp);
	
}

void CP8CA_LcDispDoc::Match_Worked_Report_for_ValidDropPoint2(int nPattern_num, BOOL bDropEnable[MAX_PATTERN][MAX_NOZZLE])
{
// 우선 보고해야 하는 Drop 수를 하나의 배열로 순차적으로 정렬한다.
	int ValDropTot=0, nCountReport=0, nBaseIndex=0, nOffsetAddress=0,ArrIndex=0;
	int head=0, pat=0;
	int i;
	for( i=0;i<MAX_PANEL;i++)			
	{
		ValDropTot = 0;
		if(i<CIM_Information.Panal_Max_Num)
		{	
			for( pat = 0 ; pat < m_structDataEditor.m_nPatternNum ; pat++)
			{
				for( head = 0 ; head < MAX_NOZZLE ; head++)
				{
					if( (m_structPatternData[pat].m_nSequence > 0) && (m_structPatternData[pat].m_nPanelNo[head] == i + 1) )
					{
						if(bDropEnable[pat][head] == TRUE) 
						{
//							nCountReport=m_nValidDropPoint[pat][head];	 		
							nCountReport=m_nRmsValidDropPoint[pat][head];	 		
							ValDropTot+=nCountReport;
						}
						else
						{
							nCountReport=0;		
							ValDropTot+=nCountReport;
						}
					}
				}				
			}
			CIM_Information.nRmsDropPoint[i]=ValDropTot;
		}
		else CIM_Information.nRmsDropPoint[i]=-1;
	}

	// 보고해야할 Data의 지정 Address를 맞추기 위해 재배열한다.
	// 미리 -1을 넣는 이유는 원래 정상적인 유효 Drop수(혹은 Drop Count수)는 0보다 큰 수이므로
	// 만약 처음에 할당한 -1의 값이 변하지 않았다면 공백으로 처리해야 할 Panel이라는 뜻이 될 수 있다.
	for(i=0;i<MAX_PANEL;i++)	CIM_Information.nRms_Worked_Report_DropPoint[i]=-1;

	for(i=0;i<MAX_PANEL;i++)
	{
		nBaseIndex=100;	// 공백으로 처리해야 할 Panel들 구분.
		if(i<CIM_Information.Panal_Max_Num)	// 사용하지 않는 Panel에 대한 정보가 Overwrite되지 않도록 조건을 건다.
		{
			if(m_strGlassIden[i]!="")
			{
				// 8개 영역
				// ( m_strGlassIden[i].GetAt(1) < 9  ) 
				char chtemp = m_strGlassIden[i].GetAt(1);
				if ( 48 < chtemp &&  chtemp < 58  ) //1~9
				{
					switch(m_strGlassIden[i].GetAt(0))
					{
						case 'A':	nBaseIndex=0;	break;  // 8
						case 'B':	nBaseIndex=1;	break;  // 8
						case 'C':	nBaseIndex=2;	break;  // 8
						case 'D':	nBaseIndex=3;	break;  // 8
						case 'E':	nBaseIndex=4;	break;  // 8
						case 'F':	nBaseIndex=5;	break;  // 8
						case 'G':	nBaseIndex=6;	break;  // 8
						case 'H':	nBaseIndex=7;	break;  // 8
						case 'J':	nBaseIndex=8;	break;  // 8
						case 'K':	nBaseIndex=9;	break;  // 8
					}
				}
				// 12개 또는 20개 영역 
				else 
				{
					switch(m_strGlassIden[i].GetAt(0))
					{
						case 'A':	nBaseIndex=10;	break;  // 12
						case 'B':	nBaseIndex=11;	break;  // 12
						case 'C':	nBaseIndex=12;	break;  // 12
						case 'D':	nBaseIndex=13;	break;  // 12
						case 'E':	nBaseIndex=14;	break;  // 12
						case 'F':	nBaseIndex=15;	break;  // 12
						case 'G':	nBaseIndex=16;	break;  // 12
						case 'H':	nBaseIndex=17;	break;  // 12
						case 'J':	nBaseIndex=18;	break;  // 12
						case 'K':	nBaseIndex=19;	break;  // 12
						case 'L':	nBaseIndex=20;	break;	// 20
						case 'M':	nBaseIndex=21;	break;	// 20
						case 'N':	nBaseIndex=22;	break;	// 20
						case 'P':	nBaseIndex=23;	break;  // 20
						case 'Q':	nBaseIndex=24;	break;	// 20
					}
				}
			}
		}

		if(nBaseIndex!=100)		// 이걸 안해 주면 배열의 할당에 문제가 생김.
		{
			nOffsetAddress=atoi((char *)(LPCSTR)m_strGlassIden[i].Mid(1,1))-1;

			if ( nBaseIndex < 10 )
			{
				ArrIndex = nBaseIndex * 8  + nOffsetAddress;
			}
			else if ( nBaseIndex < 20)
			{
				// 80 : 이전 Index 총 개수 
				// nBaseIndex - 10 : 이전 Glass Iden 개수
				// 12 : ID 당 Cell 개수 
				ArrIndex = 80 + ( ( nBaseIndex - 10 ) * 12 ) + nOffsetAddress;
			}
			else
			{
				// 80 + 120 : 이전 Index 총 개수 
				// nBaseIndex - 20 : 이전 Glass Iden 개수 
				// 20 : ID 당 Cell 개수  
				ArrIndex = 80 + 120 + ( ( nBaseIndex - 20 ) * 20 ) + nOffsetAddress;
			}

			CIM_Information.nRms_Worked_Report_DropPoint[ArrIndex] = CIM_Information.nRmsDropPoint[i];			
		}
	}
}

BOOL CP8CA_LcDispDoc::ReadRecipRms(CString strRecipeName)
{
	ifstream fi;
	CString strPathName;
	char ch[256];
	int i=0 , j=0;

	/////////////////////////////////////////////////////////////////////////////////
	// RMS 보고 관련//by shin//2009.08.26//
	strPathName.Format("%s\\%s\\RMS_Report.dat", m_strDataPath, strRecipeName);
	fi.open(strPathName, ios::in);

	if(!fi.is_open())
	{
//		AfxMessageBox("RMS_Report.dat파일이 없습니다");
		return FALSE;
	}
		
	fi >> m_dRmsTargetWeight;
	fi >> ch;
	m_strRmsLcType.Format("%s", ch);
	fi >> m_nRmsPointNumberX;
	fi >> m_nRmsPointNumberY;
	fi >> m_dRmsWidthX;
	fi >> m_dRmsWidthY;
	fi >> m_nRmsCenterDotting;
//2010.09.19 by tskim
	fi >> m_dRmsGlassSizeX;	
	fi >> m_dRmsGlassSizeY;	
	fi >> m_dRmsHomeX;			
	fi >> m_dRmsHomeY;			
	fi >> m_dRmsThickness;		
	fi >> m_nRmsPatternNum;		
	fi >> m_nRmsSpecialInfo;
	fi >> m_nRmsSafetyLock;
	fi >> m_nRmsGlassCheck;	
	fi >> m_nRmsRecipeNumCompare;
	fi >> m_nRmsPatternDispensing;
	fi >> m_nRmsNgDropping;
	fi >> m_nRmsLcSupplyType;
	fi >> m_nRmsDummyType;
	fi >> m_nRmsCanisterEmgWeight;  
    fi >> m_nRmsLiquidSupplyLimit;   
	fi >> m_nRmsAdjustInAuto;
	fi >> m_nRmsDummyDropBeforeMeasInAuto;	
	fi >> m_nRmsTableSpeed;
	fi >> m_nRmsSAxisSpeed;
	fi >> m_dRmsDropOffset;
	fi >> m_dRmsDischargeSpeed;
	fi >> m_dRmsSuctionSpeed;
	fi >> m_dRmsAccTime;
	fi >> m_dRmsDecTime;
	fi >> m_dRmsStartSpeed;
	for(i=0;i<MAX_NOZZLE;i++)
		fi >> m_dRmsNozzleOffset[i];
	for(i=0;i<5;i++)
	{
		fi >> m_dRmsCenterPositionX[i];
		fi >> m_dRmsCenterPositionY[i];
	}
	fi >> m_nRmsSuctionLineNo;
	fi >> m_nRmsSequence;
//2014.11.27 by tskim RMS Add
/**********************************************************************************************************/
	fi >> m_Rms_dMarginRate;
	fi >> m_Rms_dCalibrationInterlock; 
	fi >> m_Rms_bMMGLineDropMode;
	fi >> m_Rms_nCheckDegasSheet;
	fi >> m_Rms_structVision.nTotal_Miss;
	fi >> m_Rms_structVision.nCell_Miss;
	fi >> m_Rms_structVision.nTotal_Over;
	fi >> m_Rms_structVision.nCell_Over;
	fi >> m_Rms_structVision.dCell_Rate;
	fi >> m_Rms_structVision.dScanX;
	fi >> m_Rms_structVision.dScanY;
	fi >> m_Rms_structVision.nLight;
	fi >> m_Rms_bUse_Vision; 
	fi >> m_Rms_structVision.dDrop_Size[0];	
	fi >> m_Rms_structVision.dDrop_Size[1];
	fi >> m_Rms_structVision.nScanCycle; 
	fi >> m_Rms_structVision.dK_Speed;
	fi >> m_Rms_structVision.bReverse;
	fi >> m_Rms_structVision.nGlass_Over;

 	for(i=0;i<MAX_PATTERN_FOR_RMS;i++)
 	{
		 fi >> m_Rms_structPatternData[i].m_nNShot; 
	}
 	for(i=0;i<MAX_PATTERN_FOR_RMS;i++)
 	{
 		for ( j = 0; j < MAX_NOZZLE ; j++ )
 			fi >> m_Rms_structPatternData[i].m_nNRunLimit[j];
	}
 	for(i=0;i<MAX_PATTERN_FOR_RMS;i++)
 	{
 		for ( j = 0; j < MAX_NOZZLE ; j++ )
 			fi >> m_Rms_structPatternData[i].m_nNAmplitude[j];
 	}
 	for(i=0;i<MAX_PATTERN_FOR_RMS;i++)
 	{
 		for ( j = 0; j < MAX_NOZZLE ; j++ )
 			fi >> m_Rms_structPatternData[i].m_dNDuty[j];
 	}
 	for(i=0;i<MAX_PATTERN_FOR_RMS;i++)
 	{
 		for ( j = 0; j < MAX_NOZZLE ; j++ )
 			fi >> m_Rms_structPatternData[i].m_nNFirstVoltage[j];
 	}
 	for(i=0;i<MAX_PATTERN_FOR_RMS;i++)
 	{
 		for ( j = 0; j < MAX_NOZZLE ; j++ )
 			fi >> m_Rms_structPatternData[i].m_nNNegOffset[j];
 	}
 	for(i=0;i<MAX_PATTERN_FOR_RMS;i++)
 	{
 		for ( j = 0; j < MAX_NOZZLE ; j++ )
 			fi >> m_Rms_structPatternData[i].m_nNFallingEdge[j];
 	}

//2015.03.15 ehji RMS ADD

	fi >> m_bRMS_LCQuanReportPlus;
	fi >> m_bRMS_LCQuanReportMinus;
	fi >> m_bRMS_AddDelChange;
	fi >> m_bRMS_AddDelCompare;
	fi >> m_bRMS_SelectDropMode;
	fi >> m_bRMS_InteractiveDrop;
	fi >> m_nRMS_GroupLineDrop;
	fi >> m_nRMS_CalibraionMode;
	fi >> m_nRMS_NzlMode;

/**********************************************************************************************************/

	fi.close();

//2010.09,.28 by tskim Rms level3
	/////////////////////////////////////////////////////////////////////////////////
	// RMS 보고 관련//by shin//2009.08.26//
	strPathName.Format("%s\\%s\\RMS_Report_Read.dat", m_strDataPath, strRecipeName);
	fi.open(strPathName, ios::in);

	if(!fi.is_open())
	{
		AfxMessageBox("RMS_Report.dat파일이 없습니다");
		return FALSE;
	}
		
//	MakeRmsData();

	fi >> m_dRmsTargetWeight_R;
	fi >> ch;
	m_strRmsLcType_R.Format("%s", ch);
	fi >> m_nRmsPointNumberX_R;
	fi >> m_nRmsPointNumberY_R;
	fi >> m_dRmsWidthX_R;
	fi >> m_dRmsWidthY_R;
	fi >> m_nRmsCenterDotting_R;
//2010.09.19 by tskim
	fi >> m_dRmsGlassSizeX_R;	
	fi >> m_dRmsGlassSizeY_R;	
	fi >> m_dRmsHomeX_R;			
	fi >> m_dRmsHomeY_R;			
	fi >> m_dRmsThickness_R;		
	fi >> m_nRmsPatternNum_R;		
	fi >> m_nRmsSpecialInfo_R;
	fi >> m_nRmsSafetyLock_R;
	fi >> m_nRmsGlassCheck_R;	
	fi >> m_nRmsRecipeNumCompare_R;
	fi >> m_nRmsPatternDispensing_R;
	fi >> m_nRmsNgDropping_R;
	fi >> m_nRmsLcSupplyType_R;
	fi >> m_nRmsDummyType_R;
	fi >> m_nRmsCanisterEmgWeight_R;  
    fi >> m_nRmsLiquidSupplyLimit_R;   
	fi >> m_nRmsAdjustInAuto_R;
	fi >> m_nRmsDummyDropBeforeMeasInAuto_R;	
	fi >> m_nRmsTableSpeed_R;
	fi >> m_nRmsSAxisSpeed_R;
	fi >> m_dRmsDropOffset_R;
	fi >> m_dRmsDischargeSpeed_R;
	fi >> m_dRmsSuctionSpeed_R;
	fi >> m_dRmsAccTime_R;
	fi >> m_dRmsDecTime_R;
	fi >> m_dRmsStartSpeed_R;
	for(i=0;i<MAX_NOZZLE;i++)
		fi >> m_dRmsNozzleOffset_R[i];
	for(i=0;i<5;i++)
	{
		fi >> m_dRmsCenterPositionX_R[i];
		fi >> m_dRmsCenterPositionY_R[i];
	}
	fi >> m_nRmsSuctionLineNo_R;
	fi >> m_nRmsSequence_R;

	// 화일 닫기
	fi.close();
	
	return TRUE;
}

void CP8CA_LcDispDoc::DropDataSet()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetMainWnd();
	int i,j,k; //,temp;
	DROP_DATA stDropData;
	int Index=0;
	LINE_DATA stLineData;
	CString str;

//2008.06.08 by tskim
//Line Data Define
	int LineConfig[150][3]; // start point no , drop no , direction//50->100
	double dPointX = 0.0 , dPointY =0.0;
	int direction=0, drop_count=0, line_count=0;
	int PatNo,StartNo,LineNo,DropTotal = 0;
	BOOL bTemp;
	double YDropMin = 0.0, YDropMax = 0.0, XDropMin = 0.0, XDropMax = -500.0; //-500이 뭘 의미 하는 거지?//
//	int StartPos;
//	int DrMin, DrMax;
	int count[500];
	int TempNo = 0;
	for(i=0;i<500;i++)
		count[i] = 0;
	LineConfig[0][0] = 0;
	LineNo = 0;
	bTemp = FALSE;
	int YMax=0, YMin=0, XMax=0, XMin=0;

	double TempY[500];
	double TempX[500];
//	double TempPat[500];
	double Temp = 0;
	int MaxLine;
	for(i=0;i<500;i++)
		TempX[i] = 1000.0;
	BOOL Sort = FALSE;

	m_nLineDropCount = 0;

	m_arLineData->RemoveAll();

	for(i=0; i<m_structDataEditor.m_nPatternNum; i++)
	{
		if(m_structPatternData[i].m_nSequence != 0)
		{
			Index += m_structPatternData[i].m_nDropDataCount;
		}
	}

	for(i=0; i<Index; i++)
		m_arLineData->Add(stLineData);

	Index = 0;
	for(i=0; i<m_structDataEditor.m_nPatternNum; i++)
	{
		if(m_structPatternData[i].m_nSequence != 0)
		{
			for(j=0; j<m_structPatternData[i].m_nDropDataCount; j++)
			{
				stDropData = m_arDropData[i].GetAt(j);
				stLineData.m_dPointX = stDropData.m_dPointX;
				str.Format("%.2f",stLineData.m_dPointX);
				stLineData.m_dPointX = atof(str);
				stLineData.m_dPointY = stDropData.m_dPointY;
				stLineData.m_nN = stDropData.m_nN;
				for(k=0; k<MAX_NOZZLE; k++)
				{
					stLineData.m_nZ[k] = stDropData.m_nZ[k];
					stLineData.m_nAddDel[k] = stDropData.m_nAddDel[k];
				}
				stLineData.PatNo = i+1;
				m_arLineData->SetAt(Index, stLineData);
				Index++;
			}
		}
	}
	m_nLineDropCount = Index;
	Index = 0;
	for(i=0; i<m_nLineDropCount; i++)//동일 라인 좌표 정렬 최대, 최소 값 정렬에 필요함...
	{
		stLineData = m_arLineData->GetAt(i);
		if(stLineData.m_dPointY <= YDropMin)
		{
			YDropMin = stLineData.m_dPointY;
			YMin = i;
		}
		else if(stLineData.m_dPointY >= YDropMax)
		{
			YDropMax = stLineData.m_dPointY;
			YMax = i;
		}
		else if(stLineData.m_dPointX <= XDropMin)
		{
			XDropMin = stLineData.m_dPointX;
			XMin = i;
		}
		else if(stLineData.m_dPointX >= XDropMax)
		{
			XDropMax = stLineData.m_dPointX;
			XMax = i;
		}
	}
	
	stLineData = m_arLineData->GetAt(0);
	dPointX = stLineData.m_dPointX;//Pattern Data X라인 갯수가 다른경우..
	dPointY = stLineData.m_dPointY;
	PatNo = stLineData.PatNo;
	drop_count = 0;
	LineNo = 0;

	while(1)
	{
		for(i=LineNo; i<m_nLineDropCount; i++)
		{
			stLineData = m_arLineData->GetAt(i);

			if(dPointX == stLineData.m_dPointX)
			{
				drop_count++;
				StartNo = i;
				stLineData.DropCount = drop_count;
				stLineData.LineCount = line_count;
				m_arLineData->SetAt(i,stLineData);
			}
			else if(dPointX != stLineData.m_dPointX && !bTemp)
			{
				TempX[line_count] = dPointX;
				LineNo = i;
				bTemp = TRUE;
			}
		}
		if(Sort) 
		{
			Sort = FALSE;
		}
		else
		{
			DropTotal +=drop_count;
			line_count++;
			drop_count = 0;
			stLineData = m_arLineData->GetAt(LineNo);
			dPointX = stLineData.m_dPointX;       
			dPointY = stLineData.m_dPointY;
			bTemp = FALSE;
		}

		if(DropTotal == m_nLineDropCount)
		{
			MaxLine = line_count;
			break;
		}
	}


	for(k=0; k<MaxLine; k++)
	{
		int IndexM = -1;
		for(i=0; i<m_nLineDropCount; i++)//동일 라인 좌표 정렬
		{
			stLineData = m_arLineData->GetAt(i);
			if(stLineData.LineCount == k)
			{
				IndexM++;
				TempY[IndexM] = stLineData.m_dPointY;
			}
		}
		
		for(i=0;i<IndexM;i++)
		{
			for(j=i+1;j<IndexM+1;j++)
			{
				if(TempY[i]>TempY[j])
				{
					Temp=TempY[i];
					TempY[i]=TempY[j];
					TempY[j]=Temp;
				}
			}
		}
	
		for(i=0; i<m_nLineDropCount; i++)//동일 라인 좌표 정렬
		{
			stLineData = m_arLineData->GetAt(i);
			if((stLineData.LineCount == k))
			{
				for(int j=0; j<IndexM+1; j++)
				{
					if(stLineData.m_dPointY == TempY[j])
					{
						stLineData.DropSeq = j+1;
						m_arLineData->SetAt(i,stLineData);
					}
				}
			}
		}
	
		if(k > 0)
			//누적 line count drop number 계산...
			TempNo = TempNo + count[k-1];
		
		for(i=0; i<m_nLineDropCount; i++)//동일 라인 좌표 정렬
		{
			stLineData = m_arLineData->GetAt(i);
			if(stLineData.LineCount == k)
			{
				if(m_structPatternData[0].m_nDroppingStartPos == 0)//0: Left Up 1: Left Dn 2: Right Up 3: Right Dn
				{
					if(k%2==0)
					{
						stLineData.DropDirection = 1;//Direction 0:- , 1:+
						for(int j=0; j<IndexM+1; j++)
						{
							if(stLineData.DropSeq == j+1)//DropSeq 역방향
							{
								stLineData.DropNum = (IndexM+1) - j +(TempNo);
								m_arLineData->SetAt(i,stLineData);
							}
						}
						count[k]  = IndexM+1;
					}
					else
					{
						stLineData.DropDirection = 0;
						for(int j=0; j<IndexM+1; j++)
						{
							if(stLineData.DropSeq == j+1)//DropSeq 정방향...
							{
								stLineData.DropNum = (j+1) +(TempNo);
								m_arLineData->SetAt(i,stLineData);
							}
						}
						count[k] = IndexM+1;
					}
				}
				else if(m_structPatternData[0].m_nDroppingStartPos == 1)//0: Left Up 1: Left Dn 2: Right Up 3: Right Dn
				{
					if(k%2==0)
					{
						stLineData.DropDirection = 0;//Direction 0:- , 1:+
						for(int j=0; j<IndexM+1; j++)
						{
							if(stLineData.DropSeq == j+1)//DropSeq 역방향
							{
								stLineData.DropNum = (j+1) +(TempNo);
								m_arLineData->SetAt(i,stLineData);
							}
						}
						count[k]  = IndexM+1;
					}
					else
					{
						stLineData.DropDirection = 1;
						for(int j=0; j<IndexM+1; j++)
						{
							if(stLineData.DropSeq == j+1)//DropSeq 정방향...
							{
								stLineData.DropNum = (IndexM+1) - j +(TempNo);
								m_arLineData->SetAt(i,stLineData);
							}
						}
						count[k] = IndexM+1;
					}
				}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
				else if(m_structPatternData[0].m_nDroppingStartPos == 2)//0: Left Up 1: Left Dn 2: Right Up 3: Right Dn
				{
					if(k%2==0)
					{
						stLineData.DropDirection = 1;//Direction 0:- , 1:+
						for(int j=0; j<IndexM+1; j++)
						{
							if(stLineData.DropSeq == j+1)//DropSeq 역방향
							{
								stLineData.DropNum = (IndexM+1) - j +(TempNo);
								//stLineData.DropNum = 
								m_arLineData->SetAt(i,stLineData);
							}
						}
						count[k]  = IndexM+1;
					}
					else
					{
						stLineData.DropDirection = 0;
						for(int j=0; j<IndexM+1; j++)
						{
							if(stLineData.DropSeq == j+1)//DropSeq 정방향...
							{
								stLineData.DropNum = (j+1) +(TempNo);
								//stLineData.DropNum = 
								m_arLineData->SetAt(i,stLineData);
							}
						}
						count[k] = IndexM+1;
					}
				}
				else if(m_structPatternData[0].m_nDroppingStartPos == 3)//0: Left Up 1: Left Dn 2: Right Up 3: Right Dn
				{
					if(k%2==0)
					{
						stLineData.DropDirection = 0;//Direction 0:- , 1:+
						for(int j=0; j<IndexM+1; j++)
						{
							if(stLineData.DropSeq == j+1)//DropSeq 역방향
							{
								stLineData.DropNum = (j+1) +(TempNo);
								//stLineData.DropNum = 
								m_arLineData->SetAt(i,stLineData);
							}
						}
						count[k]  = IndexM+1;
					}
					else
					{
						stLineData.DropDirection = 1;
						for(int j=0; j<IndexM+1; j++)
						{
							if(stLineData.DropSeq == j+1)//DropSeq 정방향...
							{
								stLineData.DropNum = (IndexM+1) - j +(TempNo);
								//stLineData.DropNum = 
								m_arLineData->SetAt(i,stLineData);
							}
						}
						count[k] = IndexM+1;
					}
				}
			}
		}
	}
}

void CP8CA_LcDispDoc::ReadLineDropData()
{
	// 일단 화일을 연다.
	ifstream fi;
	CString strPathName;
//	DROP_DATA structDropData;
	int i,k;
	char ch[512];
	LINE_DATA stLineData;
	LINE_DATA stLineDataBk;

////////////////////////////////////////////////////////////////121210 
	LINE_DATA stMMGLineData , stMainLineData , stMMGMainLineData;
//////////////////////////////////////////////////////////////////////


	m_arLineData->RemoveAll();
	//2010.07.27 by tskim
	m_arLineDataPat->RemoveAll();
	// 화일명 설정
	strPathName.Format("%s\\%s\\SHT_LINEDrop.pat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
	
	// 화일 열기, 없어도 새로 만들지 않는다.
	fi.open((char *)(LPCSTR)strPathName, ios::nocreate);

	// 화일이 있으면 화일을 읽는다.
	if(fi.is_open())
	{
		for(i=0; i<43; i++)
			fi >> ch;
		fi >> m_nLineDropCount;
		for(i = 0; i < m_nLineDropCount; i++)
		{
			fi >> stLineData.DropNum;
			fi >> stLineData.DropDirection;
			fi >> stLineData.DropSeq;
			fi >> stLineData.DropCount;
			fi >> stLineData.LineCount;
			fi >> stLineData.m_dPointX;
			fi >> stLineData.m_dPointY;
			fi >> stLineData.m_nN;
			for(k = 0; k < MAX_NOZZLE; k++)
				fi >> stLineData.m_nZ[k];
			for(k = 0; k < MAX_NOZZLE; k++)
				fi >> stLineData.m_nAddDel[k];
			fi >> stLineData.PatNo;
			fi >> stLineData.LinePat;
			fi >> stLineData.PatSeq;

			m_arLineData->Add(stLineData);
			//2010.07.27 by tskim
			m_arLineDataPat->Add(stLineData);
		}
		// 화일 닫기
		fi.close();
	}
	// 화일이 없으면 기본값으로 셋팅한다.
	else
	{
	}

/*############################################################################################################*/
	m_arLineDataBk->RemoveAll();
	// 화일명 설정
	strPathName.Format("%s\\%s\\SHT_LINEDropBk.pat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
	
	// 화일 열기, 없어도 새로 만들지 않는다.
	fi.open((char *)(LPCSTR)strPathName, ios::nocreate);

	// 화일이 있으면 화일을 읽는다.
	if(fi.is_open())
	{
		for(i=0; i<43; i++)
			fi >> ch;
		fi >> m_nLineDropCount;
		for(i = 0; i < m_nLineDropCount; i++)
		{
			fi >> stLineDataBk.DropNum;
			fi >> stLineDataBk.DropDirection;
			fi >> stLineDataBk.DropSeq;
			fi >> stLineDataBk.DropCount;
			fi >> stLineDataBk.LineCount;
			fi >> stLineDataBk.m_dPointX;
			fi >> stLineDataBk.m_dPointY;
			fi >> stLineDataBk.m_nN;
			for(k = 0; k < MAX_NOZZLE; k++)
				fi >> stLineDataBk.m_nZ[k];
			for(k = 0; k < MAX_NOZZLE; k++)
				fi >> stLineDataBk.m_nAddDel[k];
			fi >> stLineDataBk.PatNo;
			fi >> stLineData.PatSeq;

			m_arLineData->Add(stLineDataBk);
		}
		// 화일 닫기
		fi.close();
	}
	// 화일이 없으면 기본값으로 셋팅한다.
	else
	{
	}

	m_arLineData_R->RemoveAll();
	// 화일명 설정
	strPathName.Format("%s\\%s\\SHT_LINEDrop_R.pat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
	
	// 화일 열기, 없어도 새로 만들지 않는다.
	fi.open((char *)(LPCSTR)strPathName, ios::nocreate);

	// 화일이 있으면 화일을 읽는다.
	if(fi.is_open())
	{
		for(i=0; i<43; i++)
			fi >> ch;
		fi >> m_nLineDropCount;
		for(i = 0; i < m_nLineDropCount; i++)
		{
			fi >> stLineData.DropNum;
			fi >> stLineData.DropDirection;
			fi >> stLineData.DropSeq;
			fi >> stLineData.DropCount;
			fi >> stLineData.LineCount;
			fi >> stLineData.m_dPointX;
			fi >> stLineData.m_dPointY;
			fi >> stLineData.m_nN;
			for(k = 0; k < MAX_NOZZLE; k++)
				fi >> stLineData.m_nZ[k];
			for(k = 0; k < MAX_NOZZLE; k++)
				fi >> stLineData.m_nAddDel[k];
			fi >> stLineData.PatNo;
			fi >> stLineData.LinePat;
			fi >> stLineData.PatSeq;

			m_arLineData_R->Add(stLineData);
		}
		// 화일 닫기
		fi.close();
	}
	// 화일이 없으면 기본값으로 셋팅한다.
	else
	{
	}

/*############################################################################################################*/

/*###############################################################*///MMG MODE -> MMG PATTERN LINE DATA FILE SAVE

	if( m_structDataEditor.m_bMMGLineDropMode )
	{
		m_arMMGLineData->RemoveAll();

		strPathName.Format("%s\\%s\\SHT_LINE(MMG)_MMG.pat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
		
		fi.open((char *)(LPCSTR)strPathName, ios::nocreate);

		if(fi.is_open()){

			for(i=0; i<44; i++)			fi >> ch;

			fi >> m_nMMGLineDropCount;

			for(i = 0; i < m_nMMGLineDropCount; i++){

				fi >> stMMGLineData.DropNum;				fi >> stMMGLineData.DropDirection;
				fi >> stMMGLineData.DropSeq;				fi >> stMMGLineData.DropCount;
				fi >> stMMGLineData.LineCount;				fi >> stMMGLineData.m_dPointX;
				fi >> stMMGLineData.m_dPointY;				fi >> stMMGLineData.m_nN;

				for(k = 0; k < MAX_NOZZLE; k++)				fi >> stMMGLineData.m_nZ[k];
				for(k = 0; k < MAX_NOZZLE; k++)				fi >> stMMGLineData.m_nAddDel[k];

				fi >> stMMGLineData.PatNo;
				fi >> stMMGLineData.LinePat;				fi >> stMMGLineData.PatSeq;

				m_arMMGLineData->Add(stMMGLineData);        m_arMMGLineDataPat->Add(stMMGLineData);
			}
			fi.close();
		}
		else{
		}

/*############################################################################################################*/

/*#############################################################*///MMG MODE -> MMG PATTERN LINE R DATA FILE SAVE

		m_arMMGLineData_R->RemoveAll();

		strPathName.Format("%s\\%s\\SHT_LINE(MMG)R_MMG.pat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
		
		fi.open((char *)(LPCSTR)strPathName, ios::nocreate);

		if(fi.is_open()){

			for(i=0; i<44; i++)			fi >> ch;

			fi >> m_nMMGLineDropCount;

			for(i = 0; i < m_nMMGLineDropCount; i++){

				fi >> stMMGLineData.DropNum;				fi >> stMMGLineData.DropDirection;
				fi >> stMMGLineData.DropSeq;				fi >> stMMGLineData.DropCount;
				fi >> stMMGLineData.LineCount;				fi >> stMMGLineData.m_dPointX;
				fi >> stMMGLineData.m_dPointY;				fi >> stMMGLineData.m_nN;

				for(k = 0; k < MAX_NOZZLE; k++)				fi >> stMMGLineData.m_nZ[k];
				for(k = 0; k < MAX_NOZZLE; k++)				fi >> stMMGLineData.m_nAddDel[k];

				fi >> stMMGLineData.PatNo;
				fi >> stMMGLineData.LinePat;				fi >> stMMGLineData.PatSeq;

				m_arMMGLineData_R->Add(stMMGLineData);
			}
			fi.close();
		}
		else{
		}


/*############################################################################################################*/

/*##############################################################*///MMG MODE -> MAIN PATTERN LINE DATA FILE SAVE

		m_arMainLineData->RemoveAll();

		strPathName.Format("%s\\%s\\SHT_LINE(MMG)_Main.pat", m_strDataPath , m_structOperatorModeRecipeData.strFrontRecipeName);
		
		fi.open((char *)(LPCSTR)strPathName, ios::nocreate);

		if(fi.is_open()){

			for(i=0; i<44; i++)			fi >> ch;

			fi >> m_nMMGLineDropCount_Main;

			for(i = 0; i < m_nMMGLineDropCount_Main; i++){

				fi >> stMainLineData.DropNum;
				fi >> stMainLineData.DropDirection;
				fi >> stMainLineData.DropSeq;
				fi >> stMainLineData.DropCount;
				fi >> stMainLineData.LineCount;
				fi >> stMainLineData.m_dPointX;
				fi >> stMainLineData.m_dPointY;
				fi >> stMainLineData.m_nN;

				for(k = 0; k < MAX_NOZZLE; k++)				fi >> stMainLineData.m_nZ[k];
				for(k = 0; k < MAX_NOZZLE; k++)				fi >> stMainLineData.m_nAddDel[k];

				fi >> stMainLineData.PatNo;
				fi >> stMainLineData.LinePat;
				fi >> stMainLineData.PatSeq;

				m_arMainLineData->Add(stMainLineData);       m_arMainLineDataPat->Add(stMainLineData);
			}
			fi.close();
		}
		else{
		}

/*############################################################################################################*/

/*#############################################################*///MMG MODE -> Main PATTERN LINE R DATA FILE SAVE

		m_arMainLineData_R->RemoveAll();

		strPathName.Format("%s\\%s\\SHT_LINE(MMG)R_Main.pat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
		
		fi.open((char *)(LPCSTR)strPathName, ios::nocreate);

		if(fi.is_open()){

			for(i=0; i<44; i++)			fi >> ch;

			fi >> m_nMMGLineDropCount_Main;

			for(i = 0; i < m_nMMGLineDropCount_Main; i++){

				fi >> stMainLineData.DropNum;				fi >> stMainLineData.DropDirection;
				fi >> stMainLineData.DropSeq;				fi >> stMainLineData.DropCount;
				fi >> stMainLineData.LineCount;				fi >> stMainLineData.m_dPointX;
				fi >> stMainLineData.m_dPointY;				fi >> stMainLineData.m_nN;

				for(k = 0; k < MAX_NOZZLE; k++)				fi >> stMainLineData.m_nZ[k];
				for(k = 0; k < MAX_NOZZLE; k++)				fi >> stMainLineData.m_nAddDel[k];

				fi >> stMainLineData.PatNo;
				fi >> stMainLineData.LinePat;				fi >> stMainLineData.PatSeq;

//				fi >> stMainLineData.DropStatus;

				m_arMainLineData_R->Add(stMainLineData);
			}
			fi.close();
		}
		else{
		}

/*############################################################################################################*/

/*###############################################################*///MMG MODE -> ALL PATTERN LINE DATA FILE SAVE

		m_arMainMMGYLineData->RemoveAll();

		strPathName.Format("%s\\%s\\SHT_LINE(MMG)_MAIN_MMG.pat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
		
		fi.open((char *)(LPCSTR)strPathName, ios::nocreate);

		if(fi.is_open()){

			for(i=0; i<44; i++)		
				fi >> ch;

			fi >> m_nMMGAllLineDropCount;

			for(i = 0; i < m_nMMGAllLineDropCount; i++){

				fi >> stMMGMainLineData.DropNum;
				fi >> stMMGMainLineData.DropDirection;
				fi >> stMMGMainLineData.DropSeq;
				fi >> stMMGMainLineData.DropCount;
				fi >> stMMGMainLineData.LineCount;
				fi >> stMMGMainLineData.m_dPointX;
				fi >> stMMGMainLineData.m_dPointY;
				fi >> stMMGMainLineData.m_nN;

				for(k = 0; k < MAX_NOZZLE; k++)				fi >> stMMGMainLineData.m_nZ[k];
				for(k = 0; k < MAX_NOZZLE; k++)				fi >> stMMGMainLineData.m_nAddDel[k];

				fi >> stMMGMainLineData.PatNo;
				fi >> stMMGMainLineData.LinePat;
				fi >> stMMGMainLineData.PatSeq;

				m_arMainMMGYLineData->Add(stMMGMainLineData);  
				m_arMainMMGYLineDataPat->Add(stMMGMainLineData); 
			}
			fi.close();
		}
		else{
		}

/*############################################################################################################*/

/*##############################################################*///MMG MODE -> ALL PATTERN LINE R DATA FILE SAVE
		m_arMainMMGYLineData_R->RemoveAll();

		strPathName.Format("%s\\%s\\SHT_LINE(MMG)_MAIN_MMG_R.pat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
		
		fi.open((char *)(LPCSTR)strPathName, ios::nocreate);

		if(fi.is_open()){

			for(i=0; i<44; i++)			fi >> ch;

			fi >> m_nMMGAllLineDropCount;

			for(i = 0; i < m_nMMGAllLineDropCount; i++){

				fi >> stMMGMainLineData.DropNum;
				fi >> stMMGMainLineData.DropDirection;
				fi >> stMMGMainLineData.DropSeq;
				fi >> stMMGMainLineData.DropCount;
				fi >> stMMGMainLineData.LineCount;
				fi >> stMMGMainLineData.m_dPointX;
				fi >> stMMGMainLineData.m_dPointY;
				fi >> stMMGMainLineData.m_nN;

				for(k = 0; k < MAX_NOZZLE; k++)				fi >> stMMGMainLineData.m_nZ[k];
				for(k = 0; k < MAX_NOZZLE; k++)				fi >> stMMGMainLineData.m_nAddDel[k];

				fi >> stMMGMainLineData.PatNo;
				fi >> stMMGMainLineData.LinePat;
				fi >> stMMGMainLineData.PatSeq;

				m_arMainMMGYLineData_R->Add(stMMGMainLineData);
			}
			fi.close();
		}
		else{
		}
	}

}

BOOL CP8CA_LcDispDoc::ReadMarkAlignResult()
{
	ifstream fi;
	CString strPathName;
	int i;

	strPathName.Format("%s\\MarkAlignResult.dat", m_strDataPath);
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);

	if(fi.is_open())
	{
		fi >> ThreadStage.dAlignOffsetX;
		fi >> ThreadStage.dAlignOffsetY;
		fi >> g_nGlassAlignCount;
		for(i=0;i<100;i++)
		{
			fi >> m_dAlignXTrace[i];
			fi >> m_dAlignYTrace[i];
		}
	}
	else
		return FALSE;

	fi.close();
	return TRUE;
}

void CP8CA_LcDispDoc::SaveMarkAlignResult()
{
	FILE *fp;
	CString strPathName;
	int i;

	strPathName.Format("%s\\MarkAlignResult.dat", m_strDataPath);
	fp = fopen((char *)(LPCSTR)strPathName, "wt");

	if(fp==NULL) return;
	else
	{
		fprintf(fp, "%.2f\n", ThreadStage.dAlignOffsetX);
		fprintf(fp, "%.2f\n", ThreadStage.dAlignOffsetY);
		fprintf(fp, "%d\n", g_nGlassAlignCount);
		for(i=0;i<100;i++)
		{
			fprintf(fp, "%.2f\t", m_dAlignXTrace[i]);
			fprintf(fp, "%.2f\n", m_dAlignYTrace[i]);
		}
		fclose(fp);
		return;
	}
}

BOOL CP8CA_LcDispDoc::SaveRmsData(CString strRecipeName)
{
	FILE *fp;
	CFileException e;
	CString strFile;
	int i = 0 , j = 0 ;

	/////////////////////////////////////////////////////////////////////////////////////////
	// RMS Report 
	strFile.Format("%s\\%s\\RMS_Report_Read.dat", m_strDataPath, strRecipeName);	
	fp = fopen((char *)(LPCSTR)strFile, "wt");
	if(!fp) 
	{
		AfxMessageBox("RMS_Report.dat 파일 생성 실패");
		return FALSE;
	}


	fprintf(fp, "%.3f\n", m_dRmsTargetWeight_R);
	fprintf(fp, "%s\n",		m_strRmsLcType_R);	
	fprintf(fp, "%d\n",		m_nRmsPointNumberX_R);	
	fprintf(fp, "%d\n",		m_nRmsPointNumberY_R);
	fprintf(fp, "%.3f\n",	m_dRmsWidthX_R);
	fprintf(fp, "%.3f\n",	m_dRmsWidthY_R);
	fprintf(fp, "%d\n",		m_nRmsCenterDotting_R);
//2010.09.19 by tskim
	fprintf(fp, "%.0f\n",		m_dRmsGlassSizeX_R);
	fprintf(fp, "%.0f\n",		m_dRmsGlassSizeY_R);
	fprintf(fp, "%.3f\n",		m_dRmsHomeX_R);
	fprintf(fp, "%.3f\n",		m_dRmsHomeY_R);
	fprintf(fp, "%.3f\n",		m_dRmsThickness_R);		
	fprintf(fp, "%d\n",		m_nRmsPatternNum_R);
	fprintf(fp, "%d\n",		m_nRmsSpecialInfo_R);
	fprintf(fp, "%d\n",		m_nRmsSafetyLock_R);
	fprintf(fp, "%d\n",		m_nRmsGlassCheck_R);
	fprintf(fp, "%d\n",		m_nRmsRecipeNumCompare_R);
	fprintf(fp, "%d\n",		m_nRmsPatternDispensing_R);
	fprintf(fp, "%d\n",		m_nRmsNgDropping_R);
	fprintf(fp, "%d\n",		m_nRmsLcSupplyType_R);
	fprintf(fp, "%d\n",		m_nRmsDummyType_R);
	fprintf(fp, "%.0f\n",		m_nRmsCanisterEmgWeight_R);
	
	fprintf(fp, "%d\n",		m_nRmsLiquidSupplyLimit_R);
	fprintf(fp, "%d\n",		m_nRmsAdjustInAuto_R);
	fprintf(fp, "%d\n",		m_nRmsDummyDropBeforeMeasInAuto_R);
	fprintf(fp, "%d\n",		m_nRmsTableSpeed_R);
	fprintf(fp, "%d\n",		m_nRmsSAxisSpeed_R);

	fprintf(fp, "%d\n",		m_dRmsDropOffset_R);	//20160331
							



	fprintf(fp, "%d\n",		m_dRmsDischargeSpeed_R);
	fprintf(fp, "%d\n",		m_dRmsSuctionSpeed_R);
	fprintf(fp, "%d\n",		m_dRmsAccTime_R);
	fprintf(fp, "%d\n",		m_dRmsDecTime_R);
	fprintf(fp, "%d\n",		m_dRmsStartSpeed_R); 

	for(i=0;i<MAX_NOZZLE;i++)
		fprintf(fp, "%.3f\n",		m_dRmsNozzleOffset_R[i]);
	for(i=0;i<5;i++)
	{
		fprintf(fp, "%.3f\n",		m_dRmsCenterPositionX_R[i]);
		fprintf(fp, "%.3f\n",		m_dRmsCenterPositionY_R[i]);
	}
	fprintf(fp, "%d\n",		m_nRmsSuctionLineNo_R);
	fprintf(fp, "%d\n",		m_nRmsSequence_R);	


	fclose(fp);

	/////////////////////////////////////////////////////////////////////////////////////////
	// RMS Report 
	strFile.Format("%s\\%s\\RMS_Report.dat", m_strDataPath, strRecipeName);	
	fp = fopen((char *)(LPCSTR)strFile, "wt");
	if(!fp) 
	{
		AfxMessageBox("RMS_Report.dat 파일 생성 실패");
		return FALSE;
	}


	fprintf(fp, "%.3f\n", m_dRmsTargetWeight);
	fprintf(fp, "%s\n",		m_strRmsLcType);	
	fprintf(fp, "%d\n",		m_nRmsPointNumberX);	
	fprintf(fp, "%d\n",		m_nRmsPointNumberY);
	fprintf(fp, "%.3f\n",	m_dRmsWidthX);
	fprintf(fp, "%.3f\n",	m_dRmsWidthY);
	fprintf(fp, "%d\n",		m_nRmsCenterDotting);
//2010.09.19 by tskim
	fprintf(fp, "%.0f\n",		m_dRmsGlassSizeX);
	fprintf(fp, "%.0f\n",		m_dRmsGlassSizeY);
	fprintf(fp, "%.3f\n",		m_dRmsHomeX);
	fprintf(fp, "%.3f\n",		m_dRmsHomeY);
	fprintf(fp, "%.3f\n",		m_dRmsThickness);		
	fprintf(fp, "%d\n",		m_nRmsPatternNum);
	fprintf(fp, "%d\n",		m_nRmsSpecialInfo);
	fprintf(fp, "%d\n",		m_nRmsSafetyLock);
	fprintf(fp, "%d\n",		m_nRmsGlassCheck);
	fprintf(fp, "%d\n",		m_nRmsRecipeNumCompare);
	fprintf(fp, "%d\n",		m_nRmsPatternDispensing);
	fprintf(fp, "%d\n",		m_nRmsNgDropping);
	fprintf(fp, "%d\n",		m_nRmsLcSupplyType);
	fprintf(fp, "%d\n",		m_nRmsDummyType);
	fprintf(fp, "%.0f\n",		m_nRmsCanisterEmgWeight);
	
	fprintf(fp, "%d\n",		m_nRmsLiquidSupplyLimit);
	fprintf(fp, "%d\n",		m_nRmsAdjustInAuto);
	fprintf(fp, "%d\n",		m_nRmsDummyDropBeforeMeasInAuto);
	fprintf(fp, "%d\n",		m_nRmsTableSpeed);
	fprintf(fp, "%d\n",		m_nRmsSAxisSpeed);

	fprintf(fp, "%d\n",		m_dRmsDropOffset);	//20160331
							

	fprintf(fp, "%d\n",		m_dRmsDischargeSpeed);




	fprintf(fp, "%d\n",		m_dRmsSuctionSpeed);
	fprintf(fp, "%d\n",		m_dRmsAccTime);
	fprintf(fp, "%d\n",		m_dRmsDecTime);
	fprintf(fp, "%d\n",		m_dRmsStartSpeed); 

	for(i=0;i<MAX_NOZZLE;i++)
		fprintf(fp, "%.3f\n",		m_dRmsNozzleOffset[i]);
	for(i=0;i<5;i++)
	{
		fprintf(fp, "%.3f\n",		m_dRmsCenterPositionX[i]);
		fprintf(fp, "%.3f\n",		m_dRmsCenterPositionY[i]);
	}
	fprintf(fp, "%d\n",		m_nRmsSuctionLineNo);
	fprintf(fp, "%d\n",		m_nRmsSequence);	


	fclose(fp);

	return TRUE;
}

BOOL CP8CA_LcDispDoc::SaveRmsTeachData()
{
	FILE *fp;
	CFileException e;
	CString strFile;
	int i = 0 , j = 0 ;

	/////////////////////////////////////////////////////////////////////////////////////////
	// RMS EC Report 
	strFile.Format("%s\\Teaching_RMS.dat", m_strDataPath);	
	fp = fopen((char *)(LPCSTR)strFile, "wt");
	if(!fp) 
	{
		AfxMessageBox("RMS_Report_RMS.dat 파일 생성 실패");
		return FALSE;
	}

	fprintf(fp, "%.3f\n", m_structEc_R.m_dDmyPos_Col);
	for(i=0;i<MAX_NOZZLE;i++)
	{
		fprintf(fp, "%.3f\n", m_structEc_R.m_dMeasPos_S[i]);
		fprintf(fp, "%.3f\n", m_structEc_R.m_dDmyPos_S[i]);	
		fprintf(fp, "%.3f\n", m_structEc_R.m_dSupPos_S[i]);
	}
	fprintf(fp, "%.3f\n", m_structEc_R.m_dMeasPos_Col);
	for(i=0;i<3;i++)
	{			
		fprintf(fp, "%.3f\n", m_structEc_R.m_dMutePos_Col[i]);
		fprintf(fp, "%.3f\n", m_structEc_R.m_dSensingTime[i]);
		fprintf(fp, "%.3f\n", m_structEc_R.m_dTriggerTime[i]);	
	}
	fprintf(fp, "%.3f\n", m_structEc_R.m_dMarkPos_S1);				
	fprintf(fp, "%.3f\n", m_structEc_R.m_dMarkPos_S16);				
	fprintf(fp, "%.3f\n", m_structEc_R.m_dMarkPos_Col);			
	fprintf(fp, "%.3f\n", m_structEc_R.m_dCntPos_Col);				
	fprintf(fp, "%.3f\n", m_structEc_R.m_dOperPos_Col);				
	fprintf(fp, "%.3f\n", m_structEc_R.m_dLoadPos_F);				
	fprintf(fp, "%.3f\n", m_structEc_R.m_dLoadPos_R);				
	fprintf(fp, "%.3f\n", m_structEc_R.m_dSupPos_Col);				
	fprintf(fp, "%.3f\n", m_structEc_R.m_dSupAmount);				
	fprintf(fp, "%.3f\n", m_structEc_R.m_dSupTime);					
	fprintf(fp, "%.3f\n", m_structEc_R.m_dColSpd);
	for(i=0;i<2;i++)
		fprintf(fp, "%.3f\n", m_structEc_R.m_dInputLimit[i]);

	fclose(fp);

	return TRUE;
}

BOOL CP8CA_LcDispDoc::ReadTeachRmsData()
{
	ifstream fi;
	CString strPathName;
	char ch[256];
	int i=0 , j=0;

	/////////////////////////////////////////////////////////////////////////////////
	// RMS 보고 관련//by shin//2009.08.26//
	strPathName.Format("%s\\Teaching_RMS.dat", m_strDataPath);
	fi.open(strPathName, ios::in);

	if(!fi.is_open())
	{
		AfxMessageBox("RMS_Report.dat파일이 없습니다");
		return FALSE;
	}

	fi >> m_structEc_R.m_dDmyPos_Col;
	for(i=0;i<MAX_NOZZLE;i++)
	{
		fi >> m_structEc_R.m_dMeasPos_S[i];
		fi >> m_structEc_R.m_dDmyPos_S[i];	
		fi >> m_structEc_R.m_dSupPos_S[i];
	}
	fi >> m_structEc_R.m_dMeasPos_Col;
	for(i=0;i<3;i++)
	{			
		fi >> m_structEc_R.m_dMutePos_Col[i];
		fi >> m_structEc_R.m_dSensingTime[i];
		fi >> m_structEc_R.m_dTriggerTime[i];	
	}
	fi >> m_structEc_R.m_dMarkPos_S1;				
	fi >> m_structEc_R.m_dMarkPos_S16;				
	fi >> m_structEc_R.m_dMarkPos_Col;			
	fi >> m_structEc_R.m_dCntPos_Col;				
	fi >> m_structEc_R.m_dOperPos_Col;				
	fi >> m_structEc_R.m_dLoadPos_F;				
	fi >> m_structEc_R.m_dLoadPos_R;				
	fi >> m_structEc_R.m_dSupPos_Col;				
	fi >> m_structEc_R.m_dSupAmount;				
	fi >> m_structEc_R.m_dSupTime;					
	fi >> m_structEc_R.m_dColSpd;
	for(i=0;i<2;i++)
		fi >> m_structEc_R.m_dInputLimit[i];
	fi.close();

	return TRUE;
}

void CP8CA_LcDispDoc::SetValidDropPoint_temp(int nP)
{
	double dTempAdd = 1.0, dTemp =0.0;
	int nAddPlus = 0;
	DROP_DATA stDrop;// dropdata 구조체
	int nTempAddDel , n = 0; 
	int i=0,j=0;

	for( n =0; n < MAX_NOZZLE ; n++)
	{
		m_nValidDropPoint[nP][n] = 0 ;
		m_nValidDropPoint_Conv[nP][n] = 0;
	}
//2007.06.06 by tskim
//변환 된 add/del값 초기화
	for(i = 0; i < m_arDropData[nP].GetSize(); i++)   
	{
		stDrop = m_arDropData[nP].GetAt(i);			 
		for(j =0; j < MAX_NOZZLE; j++)
		{
//2009.11.23 by tskim NG Cell은 유효 Drop에서 제외 한다..
//			if(CIM_Information.bNGCell[m_structPatternData[nP].m_nPanelNo[j]-1])
//			{
				m_nConversonAddDel[m_structPatternData[nP].m_nPanelNo[j]-1] = 0;
				if(m_structPatternData[nP].m_bIsOnHead[j] == FALSE)   
				{
					m_nValidDropPoint[nP][j] = 0;
					m_nValidDropPoint_Conv[nP][j] = 0;
					continue;
				}
				nTempAddDel = CIM_Information.Add_Del_Number[m_structPatternData[nP].m_nPanelNo[j]-1];
				if((m_structDataEditor.m_bAddDelChange == TRUE)&&(nTempAddDel != 0))///////////////////////////////////////
				{                                                                   //////////////////////////////////////
	//				dTempAdd = 4.0 / m_structAdjustCondition.dTargetWeight[j];      ///Add_Del 변환 관련 계산... 05.29
					dTempAdd = (4.0 * nTempAddDel) / m_structAdjustCondition.dTargetWeight[j];
					if(dTempAdd != 1.0 || dTempAdd != -1.0)
					{
						dTemp = dTempAdd - (int)dTempAdd;
						if(dTemp >= 0.5) nAddPlus = (int)dTempAdd + 1;
	//2007.06.07 By tskim
	//del값 변환 부분에 대한 추가
						else if(dTemp <= -0.5) nAddPlus = (int)dTempAdd -1;
						else nAddPlus = (int)dTempAdd;
					
						nTempAddDel = nAddPlus;
	//2007.06.06 By tskim
	//Add/Del 변환 값 저장..
						m_nConversonAddDel[m_structPatternData[nP].m_nPanelNo[j]-1] = nTempAddDel;
					}
				}
	//2007.06.06 by tskim
	//add/del값이 변환 됐었다면 변환 된 add/del을 적용하여 유효 drop수 재 계산 
				// m_nN = FALSE이면
				if((stDrop.m_nN == 0)||(stDrop.m_nZ[j] == 0))   
				{
					m_nValidDropPoint[nP][j] = m_nValidDropPoint[nP][j] +0;
					m_nValidDropPoint_Conv[nP][j] = m_nValidDropPoint_Conv[nP][j] +0;
				}
				else //stDrop.m_nN == TRUE이면 m_nZ를 검사한다. (stDrop.m_nN 도 TRUE이고 stDrop.m_nZ[j]도 TRUE인 경우..)
				{
					if((stDrop.m_nAddDel[j] == 0) || (m_structPatternData[nP].m_bMMG==TRUE) )     // ADD/DEL 과 무관한 일반 Point 인 경우에..
					{
						m_nValidDropPoint[nP][j] = m_nValidDropPoint[nP][j] +1;
						m_nValidDropPoint_Conv[nP][j] = m_nValidDropPoint_Conv[nP][j] +1;
					}
					else                          // Add/Del 값이 있는 점(잠정적인 변화를 줄 수 있는 점)이면..
					{ //-					-
	//2007.06.06 BY tskim
	//기존 유효 drop수 display를 위해 추가...
	/**********************************************************************************************************************************/
						if(stDrop.m_nAddDel[j] == CIM_Information.Add_Del_Number[m_structPatternData[nP].m_nPanelNo[j]-1]){   // Panel 정보와 CIM 정보가 같은 경우. Add => Drop, Del => No Drop
							if(stDrop.m_nAddDel[j] < 0){
								m_nValidDropPoint_Conv[nP][j] = m_nValidDropPoint_Conv[nP][j] +0;
							}
							else{
								m_nValidDropPoint_Conv[nP][j] = m_nValidDropPoint_Conv[nP][j] +1;
							}
						}
						else{                                  // Panel 정보와 CIM 정보가 다른 경우. CIM 정보를 우선 반영한다.
							if(stDrop.m_nAddDel[j] <CIM_Information.Add_Del_Number[m_structPatternData[nP].m_nPanelNo[j]-1]){    // CIM 정보 값이 크면 => Drop 
								m_nValidDropPoint_Conv[nP][j] = m_nValidDropPoint_Conv[nP][j] +1;
							}
							else{								  // CIM 정보 값이 작으면 => No Drop
								m_nValidDropPoint_Conv[nP][j] = m_nValidDropPoint_Conv[nP][j] +0;
							}
						}
	/**********************************************************************************************************************************/
						if(stDrop.m_nAddDel[j] == nTempAddDel){   // Panel 정보와 CIM 정보가 같은 경우. Add => Drop, Del => No Drop
							if(stDrop.m_nAddDel[j] < 0){
								m_nValidDropPoint[nP][j] = m_nValidDropPoint[nP][j] +0;
							}
							else{
								m_nValidDropPoint[nP][j] = m_nValidDropPoint[nP][j] +1;
							}
						}
						else{                                  // Panel 정보와 CIM 정보가 다른 경우. CIM 정보를 우선 반영한다.
							if(stDrop.m_nAddDel[j] <nTempAddDel){    // CIM 정보 값이 크면 => Drop 
								m_nValidDropPoint[nP][j] = m_nValidDropPoint[nP][j] +1;
							}
							else{								  // CIM 정보 값이 작으면 => No Drop
								m_nValidDropPoint[nP][j] = m_nValidDropPoint[nP][j] +0;
							}
						}
						
					} // End of else //--
				}
//			}
		} // End of for(int j =0; j < MAX_NOZZLE; j++)
	}
}

void CP8CA_LcDispDoc::Read_Vision_Param()
{
	ifstream fi;
	CString strPathName;
	// Data Editor 화일 읽기 //////////////////////////////////////////////////////
	// 화일명 설정
	strPathName.Format("%s\\%s\\Wings_Vision_Param.dat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
	// 화일 열기
	fi.open((char *)(LPCSTR)strPathName, ios::nocreate/*in*/);
	
	// 화일 읽기
	if(!fi.is_open())
	{
		fi.close();
		return;
	}
	else
	{
		fi >> m_structN_Vision.m_nScan_Cycle;
		fi >> m_structN_Vision.m_nTotal_Permit_Over;
		fi >> m_structN_Vision.m_nTotal_Permit_Miss;
		fi >> m_structN_Vision.m_nCell_Permit_Over;
		fi >> m_structN_Vision.m_nCell_Permit_Miss;
		for(int i = 0; i < MAX_VISION; i++)
			fi >> m_structN_Vision.m_nIntensity[i];
		m_structN_Vision.m_nIntensity[3] = 0;
		fi >> m_structN_Vision.m_nEdge_Intnsity;
		for(i = 0; i < MAX_VISION; i++)
			fi >> m_structN_Vision.m_dDrop_Size_X[i];
		m_structN_Vision.m_dDrop_Size_X[3] = 0.0;
		for(i = 0; i < MAX_VISION; i++)
			fi >> m_structN_Vision.m_dDrop_Size_Y[i];
		m_structN_Vision.m_dDrop_Size_Y[3] = 0.0;
		fi >> m_structN_Vision.m_nNozzle_No;
		fi >> m_structN_Vision.m_dNozzle_Pitch;
		fi >> m_structN_Vision.m_nK_Vel;
		fi >> m_structN_Vision.m_nLight_Con;
		fi >> m_structN_Vision.m_dDist_Permit_X;
		fi >> m_structN_Vision.m_dDist_Permit_Y;
		fi >> m_structN_Vision.m_nGlass_Permit_Over;
		fi >> m_structN_Vision.m_nSeal_Permit_Over;
		fi >> m_structN_Vision.m_bReverse;
		fi >> m_structN_Vision.m_bEdge_Reverse;
		fi >> m_structN_Vision.m_dGlass_Size_X;
		fi >> m_structN_Vision.m_dGlass_Size_Y;
		fi >> m_structN_Vision.m_dCell_Rate;
		fi >> m_structN_Vision.m_dEdge_Size;
		for(i = 0; i < MAX_VISION; i++)
			fi >> m_structN_Vision.m_dDrop_Close[i];
		m_structN_Vision.m_dDrop_Close[3] = 0.0;
		fi >> m_structN_Vision.m_dScan_Start_Pos;
		fi >> m_structN_Vision.m_dScan_Dist;
		fi >> m_structN_Vision.m_nScan_Speed;
		fi >> m_structN_Vision.m_nDisp_Rate;
		fi >> m_structN_Vision.m_bDir_TTB;
		fi >> m_structN_Vision.m_nFirst_Scan_Cycle;												//ehji 150305

//		동일 파라 ?? 
		m_structN_Vision.m_nK_Vel = m_structN_Vision.m_nScan_Speed; 


//160903 DUAL INTENSITY //160929 JYKIM
		for(i = 0; i < MAX_VISION; i++)
		{
			fi >> m_structN_Vision.m_dDrop_Size_High_X[i];
		}
//160903 DUAL INTENSITY //160929 JYKIM
		for(i = 0; i < MAX_VISION; i++)
		{
			fi >> m_structN_Vision.m_dDrop_Size_High_Y[i];
		}
//160903 DUAL INTENSITY //160929 JYKIM
		for(i = 0; i < MAX_VISION; i++)
		{
			fi >> m_structN_Vision.m_nIntensityHigh[i];
		}
//161214 PART SCAN
		fi >> m_structN_Vision.m_bPartScanMode; //JYKIM 161219 PART SCAN 180K
		fi >> m_structN_Vision.m_nPartScanLineCount; //JYKIM 161219 PART SCAN 스캔 주기 180K

//220603 BIG DROP DETECT ( (OLD) DEEP DROP ) 
		fi >> m_structN_Vision.m_nDetectBigDrop_Rate[0];
		fi >> m_structN_Vision.m_nDropSizeRate_X;
		fi >> m_structN_Vision.m_nDropSizeRate_Y;
		fi >> m_structN_Vision.m_nDetectBigDrop_Rate[1];
		fi >> m_structN_Vision.m_nDetectBigDrop_Rate[2];
//220603 BIG DROP DETECT ( (OLD) DEEP DROP ) 

	}	
	fi.close();
	return;
}

void CP8CA_LcDispDoc::Save_Vision_Param()
{
	FILE *fp;
	CFileException e;
	CString strFile;
	int i = 0 , j = 0 ;
	
	// Data Editor창 화일로 저장 ////////////////////////////////////////////////////////
	// 화일명 설정
	strFile.Format("%s\\%s\\Vision_Param.dat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
	
	// 화일 열기
	fp = fopen((char *)(LPCSTR)strFile, "wt");
	if(!fp) 
	{
		AfxMessageBox("Vision_Param.dat file is nothing");
		return ;
	}
	
	if(m_structDataEditor.m_nNzlMode == 0)
	{
		m_structVision.dNozzle_Pitch = 0.0;
		m_structVision.nNozzle_No = 1;
	}
	else if(m_structDataEditor.m_nNzlMode == 1)
	{
		m_structVision.dNozzle_Pitch = N_NOZZLE_PITCH;
		m_structVision.nNozzle_No = N_MAX_NOZZLE;
	}
	//////////////////////////////////////////////////////////////////////////////////
	// 안쓰는 Param 은 '0' 으로...Yamary...

//140806
//	m_structVision.nGlass_Over = 0;

	m_structVision.nSeal_Over = 0;
	m_structVision.dGlass_Area = 0.0;
	m_structVision.dGlass_Intensity = 0.0;
	m_structVision.bGlass_Reverse = FALSE;
	//////////////////////////////////////////////////////////////////////////////////
	fprintf(fp, "%.3f\t%.3f\n",			m_structVision.dDrop_Size[0], m_structVision.dDrop_Size[1]);
	fprintf(fp, "%d\t%d\t%d\n",			m_structVision.nIntensity[0], m_structVision.nIntensity[1], m_structVision.nIntensity[2]); // Yamary...20140507
	fprintf(fp, "%.3f\n",				m_structVision.dK_Speed);
	fprintf(fp, "%.3f\n",				m_structVision.dNozzle_Pitch);
	fprintf(fp, "%.3f\n",				m_structVision.dScanX);
	fprintf(fp, "%.3f\n",				m_structVision.dScanY);
	fprintf(fp, "%d\n",					m_structVision.nCell_Miss);
	fprintf(fp, "%d\n",					m_structVision.nCell_Over);
	fprintf(fp, "%d\n",					m_structVision.nNozzle_No);
	fprintf(fp, "%d\n",					m_structVision.nTotal_Miss);
	fprintf(fp, "%d\n",					m_structVision.nTotal_Over);
	fprintf(fp, "%d\n",					m_structVision.bReverse);
	fprintf(fp, "%d\n",					m_structVision.nGlass_Over);
	fprintf(fp, "%d\n",					m_structVision.nSeal_Over);
	fprintf(fp, "%d\n",					m_structVision.nLight);
	fprintf(fp, "%.3f\n",				m_structVision.dCell_Rate);
	fprintf(fp, "%d\n",					m_structVision.bGlass_Reverse);
	fprintf(fp, "%.3f\n",				m_structVision.dGlass_Area);
	fprintf(fp, "%.3f\n",				m_structVision.dGlass_Intensity);
	fprintf(fp, "%.3f\t%.3f\n",			m_structVision.dScan_Start, m_structVision.dScan_End);

	fprintf(fp, "%d\n",					m_structVision.nScanCycle); 

	//140806
	fprintf(fp, "%d\n",					m_structVision.nDummy_Distance);
	fprintf(fp, "%d\n",					m_structVision.nDummy_Intensity[0]);
	fprintf(fp, "%d\n",					m_structVision.nDummy_Intensity[1]);
	fprintf(fp, "%d\n",					m_structVision.nDummy_Intensity[2]);
	fprintf(fp, "%.f\n",				m_structVision.dDummy_Min_Size[0]);
	fprintf(fp, "%.f\n",				m_structVision.dDummy_Min_Size[1]);
	fprintf(fp, "%.f\n",				m_structVision.dDummy_Max_Size[0]);
	fprintf(fp, "%.f\n",				m_structVision.dDummy_Max_Size[1]);


	fprintf(fp, "%d\n",					m_structVision.nFirstScanCycle);	//ehji 141115

	fprintf(fp, "%d\n",					m_structVision.nMain_Intensity[0]);	//ehji 141210
	fprintf(fp, "%d\n",					m_structVision.nMain_Intensity[1]);	//ehji 141210
	fprintf(fp, "%d\n",					m_structVision.nMain_Intensity[2]);	//ehji 141210

	// 	fprintf(fp, "%.3f\t%.3f\n",			m_dSeal_Offset[0], m_dSeal_Offset[1]);
	// 화일 닫기
	fclose(fp);
}

void CP8CA_LcDispDoc::ReadNozzleCleanerData()
{
	ifstream fi;
	CString strPathName;
	
	strPathName.Format("%s\\N_Type_Nozzle_Clean.dat", m_strDataPath);
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);
	
	
	if(fi.is_open())
	{
		for(int i = 0; i < 4; i++)
		{
			fi >> Drop_Info.m_nNCleanStep[i]; //step 구분...//step1~step4
			fi >> Drop_Info.m_nNCleanStepHz[i];
			fi >> Drop_Info.m_nNCleanStepAmp[i];
			fi >> Drop_Info.m_nNCleanStepNegOffset[i];
			fi >> Drop_Info.m_dNCleanStepDuty[i];
			fi >> Drop_Info.m_nNCleanStepFallingEdge[i];
			fi >> Drop_Info.m_dNCleanStepSpeed[i];
		}
	}
	else
		return ;
	
	fi.close();
	
	return ;
}

void CP8CA_LcDispDoc::SaveFlexibleData()
{
	FILE *fp;
	CString strPath ="";
	int i=0 ,j=0 ;
	int k;
//
	strPath.Format("%s\\FlexVolCalibration.dat", m_strDataPath);	
	fp = fopen((char *)(LPCSTR)strPath, "wt");
	if(!fp) 
	{
		AfxMessageBox("FlexVolCalibration.dat file make faile.");
		return;
	}

	// vol
	for(j = 0; j <MAX_NOZZLE; j++)
	{
		for(i = 0 ; i < MAX_MEADATA_GETCOUNT2 ;i++) 
		{
			fprintf(fp, "%.3f\t", m_structCalibData[j].dMeasureFlexible[i][0]);
			fprintf(fp, "%.0f\t", m_structCalibData[j].dMeasureFlexible[i][1]);
			fprintf(fp, "%.9f\t", m_structCalibData[j].dMeasureFlexible[i][2]);
			fprintf(fp, "%.9f\t", m_structCalibData[j].dMeasureFlexible[i][3]);
		}
		fprintf(fp, "\n");
	}
	
	for(i = 0; i <MAX_NOZZLE; i++)
	{
		for(j = 0 ; j < MAX_MEADATA_GETCOUNT2 ;j++) 
		{
			fprintf(fp, "%.9f\t", m_structCalibData[i].dFGradOffset[j]);
			fprintf(fp, "%.9f\t",m_structCalibData[i].m_dFlexDropOffset[j]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);	


//2009.03.12 by tskim flex line Calibration data reference
	strPath.Format("%s\\VolCalibration4_Flex_Ref.dat", m_strDataPath);	
	fp = fopen((char *)(LPCSTR)strPath, "wt");
	if(!fp) 
	{
		AfxMessageBox("VolCalibration4_Flex_Ref.dat file make faile.");
		return;
	}

	for(j = 0; j <MAX_NOZZLE; j++)
	{	
		for(i = 0 ; i < MAX_MEADATA_GETCOUNT1 ;i++) 
		{
			fprintf(fp, "%.3f\t", m_structCalibData[j].dMeasureFlexible[i][0]);
			fprintf(fp, "%.0f\t", m_structCalibData[j].dMeasureFlexible[i][1]);
			fprintf(fp, "%.9f\t", m_structCalibData[j].dMeasureFlexible[i][2]);
			fprintf(fp, "%.9f\n", m_structCalibData[j].dMeasureFlexible[i][3]);
		}
		fprintf(fp, "\n");
	}

//2009.03.14 by tskim
	for(k = 0; k < MAX_MEADATA_GETCOUNT1; k++)
	{
		for(j = 0 ; j < MAX_NOZZLE ;j++)
		{
			fprintf(fp, "%.9f\t", m_structCalibData[j].dFGradOffset[k]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}

void CP8CA_LcDispDoc::SaveMultiTarget()
{
	FILE *fp;
	CString strPath ="";
	int i = 0 , j = 0;

	// 화일명 설정
	strPath.Format("%s\\%s\\MultiTarget.dat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
 		
	// 화일 열기
	fp = fopen((char *)(LPCSTR)strPath, "wt");
	if(!fp) 
	{
		AfxMessageBox("MultiTarget.dat 파일 생성 실패");
		return;
	}

 	for(i = 0; i < MAX_NOZZLE; i++)
 	{
 		for(j=0;j<10;j++)
		{
			if(j==9)
				fprintf(fp, "%.3f\n",			m_structAdjustCondition.dMultiTarget[i][j]);
			else
				fprintf(fp, "%.3f\t",			m_structAdjustCondition.dMultiTarget[i][j]);
		}
 	}
	for(i = 0; i < MAX_NOZZLE; i++)
		fprintf(fp, "%d\n",			m_structAdjustCondition.nMultiTargetCnt[i]);
 	for(i = 0; i < MAX_NOZZLE; i++)
 	{
 		for(j=0;j<10;j++)
		{
			if(j==9)
				fprintf(fp, "%.3f\n",			m_structAdjustCondition.dMultiTargetRate[i][j]);
			else
				fprintf(fp, "%.3f\t",			m_structAdjustCondition.dMultiTargetRate[i][j]);
		}
 	}
	// 화일 닫기
	fclose(fp);
}

void CP8CA_LcDispDoc::ReadVolCalibData()
{
	ifstream fi;
	CString strPathName;
//	char ch[256];
	int i=0,j=0,k=0;
	
	strPathName.Format("%s\\VolCalibration.dat", m_strDataPath);
	fi.open((char *)(LPCSTR)strPathName, ios::in);

	if(fi.is_open())
	{
		for(i=0; i<MAX_NOZZLE; i++)
		{
			for(j = 0; j < MAX_MEADATA_FORCALIB; j++)
			{
				fi >> m_structCalibData[i].dCoefficient1[j];
				fi >> m_structCalibData[i].dCoefficient2[j];
			}
			//		fprintf(fp, "\n");
		}
		for(i=0; i<MAX_NOZZLE; i++)
		{
			for(k =0; k < MAX_MEADATA_GETCOUNT1; k++)
			{
				fi >>  m_structCalibData[i].dMeasure[k][0];
				fi >>  m_structCalibData[i].dMeasure[k][1];
			}
			//		fprintf(fp, "\n");
		}
	}
	fi.close();
}

void CP8CA_LcDispDoc::ReadFlexVolCalibData()
{
	ifstream fi;
	CString strPathName;
	char ch[256];
	int i=0,j=0,k=0;
	
	strPathName.Format("%s\\FlexVolCalibration.dat", m_strDataPath);
	fi.open((char *)(LPCSTR)strPathName, ios::in);

	if(fi.is_open())
	{
		for(j = 0; j <MAX_NOZZLE; j++)
		{
			for(i = 0 ; i < MAX_MEADATA_GETCOUNT2 ;i++) 
			{
				fi >> m_structCalibData[j].dMeasureFlexible[i][0];
				fi >>  m_structCalibData[j].dMeasureFlexible[i][1];
				fi >>  m_structCalibData[j].dMeasureFlexible[i][2];
				fi >>  m_structCalibData[j].dMeasureFlexible[i][3];
			}
//			fprintf(fp, "\n");
		}
		
		for(i = 0; i <MAX_NOZZLE; i++)
		{
			for(j = 0 ; j < MAX_MEADATA_GETCOUNT2 ;j++) 
			{
				fi >> m_structCalibData[i].dFGradOffset[j];
				fi >> m_structCalibData[i].m_dFlexDropOffset[j];
			}
//			fprintf(fp, "\n");
		}
	}
	fi.close();
}

BOOL CP8CA_LcDispDoc::Make_Cell_for_Vision()
{
	int nNozzleNo;
 //	int nY_Shot = m_nNShot; // N_Type 이면...
//	int nY_Shot = 0;	

//150717 MAX_PATTERN -> MAX_CELL 기준으로 변경 ( 65+32 Cell 2번 Cell Info Y Count 이상 ) 
//	int nY_Shot[MAX_PATTERN] = {0,};	//ehji 150417	MMG Pattern은 N_Shot수 다름. 
	int nY_Shot[MAX_CELL] = {0,};	
 
	int i, j, nTemp = 0;
 	double dYOffset = 0.0, dTemp;
	int nDrop[MAX_CELL] = {0,}, nLine[MAX_CELL] = {0,}, nPanel[MAX_CELL][2] = {0,}, nHead_Line[MAX_CELL][MAX_NOZZLE] = {0,};
 	BOOL bTemp = FALSE;
	DROP_DATA stDropData;	

 	if(m_structDataEditor.m_nNzlMode == 1) nNozzleNo = 8;
 	else								   nNozzleNo = 1;
 	
	for(i = 0; i < m_structDataEditor.m_nPatternNum; i++)
	{
// 		if(m_structDataEditor.m_bFlexibleDrop)
// 		{
// 			dTemp = -10000.0;
// 			for(int nhead = 0; nhead < MAX_NOZZLE; nhead++)
// 			{
// 				for(j = 0; j < m_arFlexDropData[i].GetSize(); j++)
// 				{
// 					stDropData = m_arFlexDropData[0].GetAt(j);
// 					if(stDropData.m_dPointX != dTemp && stDropData.m_nPatternNo == i && stDropData.m_nN == 1 && stDropData.m_nZ[nhead] == 1)
// 					{
// 						dTemp = stDropData.m_dPointX;
// 						nHead_Line[m_structPatternData[i].m_nPanelNo[nhead]-1][nhead]++;
// 					}
// 				}
// 			}
// 		}
// 		else
// 		{
			dTemp = -10000.0;
			for(int nhead = 0; nhead < MAX_NOZZLE; nhead++)
			{
				if(m_structPatternData[i].m_bIsOnHead[nhead] == FALSE) continue;
//2015.10.11 by tskim
// 				if(m_structPatternData[i].m_nDroppingStartPos == 1 || m_structPatternData[i].m_nDroppingStartPos == 0)//Left Up & Down Drop Start
// 				{
// 					for(j = m_arDropData[i].GetSize()-1; j > -1; j--)
// 					{
// 						stDropData = m_arDropData[i].GetAt(j);
// 						if(stDropData.m_dPointX != dTemp && stDropData.m_nN == 1 && stDropData.m_nZ[nhead] == 1)
// 						{
// 							dTemp = stDropData.m_dPointX;
// 							nHead_Line[m_structPatternData[i].m_nPanelNo[nhead]-1][nhead]++;
// 						}
// 					}
// 				}
// 				else if(m_structPatternData[i].m_nDroppingStartPos == 3 || m_structPatternData[i].m_nDroppingStartPos == 2)//Right Up & Down Drop Start
// 				{
					for(j = 0; j < m_arDropData[i].GetSize(); j++)
					{
						stDropData = m_arDropData[i].GetAt(j);
						if(stDropData.m_dPointX != dTemp && stDropData.m_nN == 1 && stDropData.m_nZ[nhead] == 1)
						{
							dTemp = stDropData.m_dPointX;
							nHead_Line[m_structPatternData[i].m_nPanelNo[nhead]-1][nhead]++;
						}
					}					
//				}
			}
//		}
	}
	for(i = 0; i < m_structDataEditor.m_nPatternNum; i++)
 	{
 		for(j = 0; j < MAX_NOZZLE; j++)
 		{
 			if(m_structPatternData[i].m_bIsOnHead[j] == FALSE) continue;
 			if(m_structPatternData[i].m_nPanelNo[j] > nTemp) nTemp = m_structPatternData[i].m_nPanelNo[j];
 		}
 	}
 	_NVision.m_nCellCount = nTemp;		
 	for(i = 0; i < m_structDataEditor.m_nPatternNum; i++)
 	{
// 		( m_structDataEditor.m_nNzlMode == 1 ) ? nY_Shot = m_structPatternData[i].m_nNShot : nY_Shot = m_structPatternData[i].m_nPointNumberY;
 		( m_structDataEditor.m_nNzlMode == 1 ) ? nY_Shot[i] = m_structPatternData[i].m_nNShot : nY_Shot[i] = m_structPatternData[i].m_nPointNumberY;		//ehji 151417 	MMG Pattern은 N_Shot수 다름. 
		for(j = 0; j < MAX_NOZZLE; j++)
 		{
 			if(m_structPatternData[i].m_bIsOnHead[j] == FALSE) continue;
// 			nDrop[m_structPatternData[i].m_nPanelNo[j] - 1] += nHead_Line[m_structPatternData[i].m_nPanelNo[j]-1][j] * nY_Shot * nNozzleNo;
			nDrop[m_structPatternData[i].m_nPanelNo[j] - 1] += nHead_Line[m_structPatternData[i].m_nPanelNo[j]-1][j] * nY_Shot[i] * nNozzleNo;	//ehji 151417 	MMG Pattern은 N_Shot수 다름. 
 			nLine[m_structPatternData[i].m_nPanelNo[j] - 1] += nHead_Line[m_structPatternData[i].m_nPanelNo[j]-1][j];
 		}
 	}
 	for(i = 0; i < _NVision.m_nCellCount; i++)
 	{
 		_NVision.m_structCell_Info_Lc.m_dStart_X[i] = m_structDataEditor.m_dGlassSizeX / 2;
 		_NVision.m_structCell_Info_Lc.m_dEnd_X[i] = (-1) * m_structDataEditor.m_dGlassSizeX / 2;
 	}
 	for(; i < MAX_CELL; i++)
 	{
 		_NVision.m_structCell_Info_Lc.m_dStart_X[i] = 0.0;
 		_NVision.m_structCell_Info_Lc.m_dEnd_X[i] = 0.0;
 	}
  	for(i = 0; i < m_structDataEditor.m_nPatternNum; i++)
  	{
  		for(nTemp = 0; nTemp < _NVision.m_nCellCount; nTemp++)
  		{
  			bTemp = FALSE;
  			for(j = 0; j < MAX_NOZZLE; j++)
  			{
  				if(m_structPatternData[i].m_bIsOnHead[j] == FALSE) continue;
  				if(m_structPatternData[i].m_nPanelNo[j] == nTemp+1)
  				{
  					if(bTemp == FALSE)
  					{
  						bTemp = TRUE;
  						nPanel[nTemp][0] = nPanel[nTemp][1] = j;
  					}
  					if(nPanel[nTemp][1] < j) nPanel[nTemp][1] = j;
  				}
  			}
  			if(bTemp)
  			{
//   				if(m_structPatternData[i].m_dCenterPositionX + m_structPatternData[i].m_dNozzleOffset[nPanel[nTemp][0]] - m_structPatternData[i].m_dWidthX / 2 < _NVision.m_structCell_Info_Lc.m_dStart_X[nTemp]) 
//  					_NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] = m_structPatternData[i].m_dCenterPositionX + m_structPatternData[i].m_dNozzleOffset[nPanel[nTemp][0]] - m_structPatternData[i].m_dWidthX / 2;
  					_NVision.m_structCell_Info_Lc.m_dStart_Y[nTemp] = m_structPatternData[i].m_dCenterPositionY - m_structPatternData[i].m_dWidthY / 2;
//   				if(m_structPatternData[i].m_dCenterPositionX + m_structPatternData[i].m_dNozzleOffset[nPanel[nTemp][1]] + m_structPatternData[i].m_dWidthX / 2 > _NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp]) 
//  					_NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] = m_structPatternData[i].m_dCenterPositionX + m_structPatternData[i].m_dNozzleOffset[nPanel[nTemp][1]] + m_structPatternData[i].m_dWidthX / 2;
  					_NVision.m_structCell_Info_Lc.m_dEnd_Y[nTemp] = m_structPatternData[i].m_dCenterPositionY + m_structPatternData[i].m_dWidthY / 2;
				
 					_NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] += m_dVision_X_Offset[nPanel[nTemp][1]]; 
 					_NVision.m_structCell_Info_Lc.m_dStart_Y[nTemp] += m_dVision_Y_Offset[nPanel[nTemp][1]]; 
					_NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] += m_dVision_X_Offset[nPanel[nTemp][1]]; 
 					_NVision.m_structCell_Info_Lc.m_dEnd_Y[nTemp] += m_dVision_Y_Offset[nPanel[nTemp][1]]; 
  			}
  		}
	}

	for(nTemp = 0; nTemp < _NVision.m_nCellCount; nTemp++)
	{
		for(i = 0; i < m_structDataEditor.m_nPatternNum; i++)
		{
			if(m_structDataEditor.m_bMMGLineDropMode)		//ehji 150507
			{
				for(int nhead = 0; nhead < MAX_NOZZLE; nhead++)
				{
					if(m_structPatternData[i].m_bIsOnHead[nhead] == FALSE) continue;
					if(m_structPatternData[i].m_nPanelNo[nhead] - 1 == nTemp)
					{
// 						//2015.10.11 by tskim
// 						if(m_structPatternData[i].m_nDroppingStartPos == 1 || m_structPatternData[i].m_nDroppingStartPos == 0)//Left Up & Down Drop Start
// 						{
// 							for(j = m_arDropData[i].GetSize()-1; j > -1 ; j--)
// 							{
// 								stDropData = m_arDropData[i].GetAt(j);
// 								if(stDropData.m_nN == 1 && stDropData.m_nZ[nhead] == 1)
// 								{
// 									if(         _NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] > (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead]) 
// 										&& fabs(_NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] - (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead])) > 0.0001)
// 										//150118 INTERLOCK 좌표 반전 
// 										//								_NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] = (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead]);
// 										//	_NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] = (-1)*((stDropData.m_dPointX) + m_structPatternData[i].m_dNozzleOffset[nhead] + m_structPatternData[i].m_dWidthX);
// 										if(nTemp < (_NVision.m_nCellCount-4))
// 										{
// 											_NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] = (-1)*((stDropData.m_dPointX) + m_structPatternData[i].m_dNozzleOffset[nhead] - m_structPatternData[nTemp].m_dWidthX);
// 										}
// 										else if(nTemp >= (_NVision.m_nCellCount-4))
// 										{
// 											_NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] = (-1)*((stDropData.m_dPointX) + m_structPatternData[i].m_dNozzleOffset[nhead] + 192);
// 										}
// 										
// 									if(         _NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] < (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead]) 
// 										&& fabs(_NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] - (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead])) > 0.0001) 
// 										//150118 INTERLOCK 좌표 반전 
// 										//								_NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] = (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead]);
// 										//		_NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] = (-1)*((stDropData.m_dPointX) + m_structPatternData[i].m_dNozzleOffset[nhead] + m_structPatternData[i].m_dWidthX);
// 										if(nTemp < (_NVision.m_nCellCount-4))
// 										{
// 											_NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] = (-1)*((stDropData.m_dPointX) + m_structPatternData[i].m_dNozzleOffset[nhead] + m_structPatternData[nTemp].m_dWidthX);
// 										}
// 										else if(nTemp >= (_NVision.m_nCellCount-4))
// 										{
// 											_NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] = ((stDropData.m_dPointX) - m_structPatternData[i].m_dNozzleOffset[nhead] + m_structPatternData[nTemp].m_dWidthX);
// 										}											
// 								}
// 							}
// 						}
// 						else if(m_structPatternData[i].m_nDroppingStartPos == 3 || m_structPatternData[i].m_nDroppingStartPos == 2)//Right Up & Down Drop Start
// 						{
							for(j = 0; j < m_arDropData[i].GetSize(); j++)
							{
								stDropData = m_arDropData[i].GetAt(j);
								if(stDropData.m_nN == 1 && stDropData.m_nZ[nhead] == 1)
								{
									if(         _NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] > (-1)*(stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead]) 
										&& fabs(_NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] - (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead])) > 0.0001)
										//150118 INTERLOCK 좌표 반전 
										//								_NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] = (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead]);
										//	_NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] = (-1)*((stDropData.m_dPointX) + m_structPatternData[i].m_dNozzleOffset[nhead] + m_structPatternData[i].m_dWidthX);
										if(nTemp < (_NVision.m_nCellCount-4))
										{
											_NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] = (-1)*((stDropData.m_dPointX) + m_structPatternData[i].m_dNozzleOffset[nhead]/* - m_structPatternData[nTemp].m_dWidthX*/);
										}
										else if(nTemp >= (_NVision.m_nCellCount-4))
										{
											_NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] = (-1)*((stDropData.m_dPointX) + m_structPatternData[i].m_dNozzleOffset[nhead]/* - m_structPatternData[nTemp].m_dWidthX*/);
										}
										
									if(         _NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] < (-1)*(stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead]) 
										&& fabs(_NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] - (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead])) > 0.0001) 
										//150118 INTERLOCK 좌표 반전 
										//								_NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] = (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead]);
										//		_NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] = (-1)*((stDropData.m_dPointX) + m_structPatternData[i].m_dNozzleOffset[nhead] + m_structPatternData[i].m_dWidthX);
										if(nTemp < (_NVision.m_nCellCount-4))
										{
											_NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] = (-1)*((stDropData.m_dPointX) + m_structPatternData[i].m_dNozzleOffset[nhead] /*+ m_structPatternData[nTemp].m_dWidthX*/);
										}
										else if(nTemp >= (_NVision.m_nCellCount-4))
										{
											_NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] = (-1)*((stDropData.m_dPointX) + m_structPatternData[i].m_dNozzleOffset[nhead] /*+ m_structPatternData[nTemp].m_dWidthX*/);
										}
											
								}
							}					
//						}	
					}
				}
			}
			else
			{
				for(int nhead = 0; nhead < MAX_NOZZLE; nhead++)
				{
					if(m_structPatternData[i].m_bIsOnHead[nhead] == FALSE) continue;
					if(m_structPatternData[i].m_nPanelNo[nhead] - 1 == nTemp)
					{	
// 						//2015.10.11 by tskim
// 						if(m_structPatternData[i].m_nDroppingStartPos == 1 || m_structPatternData[i].m_nDroppingStartPos == 0)//Left Up & Down Drop Start
// 						{
// 							for(j = m_arDropData[i].GetSize()-1; j > -1 ; j--)
// 							{
// 								stDropData = m_arDropData[i].GetAt(j);
// 								if(stDropData.m_nN == 1 && stDropData.m_nZ[nhead] == 1)
// 								{
// 	//150717 2번 CELL 영역 크기 이상 관련 
// 	//								if(         _NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] > (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead]) 
// 	//									&& fabs(_NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] - (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead])) > 0.0001)
// 									if(         _NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] > (-1)*(stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead]) 
// 										&& fabs(_NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] - (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead])) > 0.0001)
// 	//150118 INTERLOCK 좌표 반전 
// 	//									_NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] = (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead]);
// 										_NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] = (-1)*(stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead]);
// 	//150717 2번 CELL 영역 크기 이상 관련
// 	//								if(         _NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] < (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead]) 
// 	//									&& fabs(_NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] - (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead])) > 0.0001) 
// 									if(         _NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] < (-1)*(stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead]) 
// 										&& fabs(_NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] - (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead])) > 0.0001) 
// 	//150118 INTERLOCK 좌표 반전 
// 	//									_NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] = (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead]);
// 										_NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] = (-1)*(stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead]);
// 								}
// 							}
// 						}
// 						else if(m_structPatternData[i].m_nDroppingStartPos == 3 || m_structPatternData[i].m_nDroppingStartPos == 2)//Right Up & Down Drop Start
// 						{
							for(j = 0; j < m_arDropData[i].GetSize(); j++)
							{
								stDropData = m_arDropData[i].GetAt(j);
								if(stDropData.m_nN == 1 && stDropData.m_nZ[nhead] == 1)
								{
	//150717 2번 CELL 영역 크기 이상 관련 
	//								if(         _NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] > (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead]) 
	//									&& fabs(_NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] - (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead])) > 0.0001)
									if(         _NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] > (-1)*(stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead]) 
										&& fabs(_NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] - (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead])) > 0.0001)
	//150118 INTERLOCK 좌표 반전 
	//									_NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] = (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead]);
										_NVision.m_structCell_Info_Lc.m_dStart_X[nTemp] = (-1)*(stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead]);
	//150717 2번 CELL 영역 크기 이상 관련
	//								if(         _NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] < (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead]) 
	//									&& fabs(_NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] - (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead])) > 0.0001) 
									if(         _NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] < (-1)*(stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead]) 
										&& fabs(_NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] - (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead])) > 0.0001) 
	//150118 INTERLOCK 좌표 반전 
	//									_NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] = (stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead]);
										_NVision.m_structCell_Info_Lc.m_dEnd_X[nTemp] = (-1)*(stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead]);
								}
							}
//						}
					}
				}
			}
		}
	}

//150717 MAX_PATTERN -> MAX_CELL 기준으로 변경 ( 65+32 Cell 2번 Cell Info Y Count 이상 ) 
// 	for (int k = m_structDataEditor.m_nPatternNum; k < _NVision.m_nCellCount; k++ )
 	for (int k = 0; k < _NVision.m_nCellCount; k++ )
	{
 		for(i = 0; i < m_structDataEditor.m_nPatternNum; i++)
 		{
 			for(j = 0; j < MAX_NOZZLE; j++)
 			{
				if( m_structPatternData[i].m_nPanelNo[j] == ( k+1 ) )
				m_structDataEditor.m_nNzlMode == 1 ? nY_Shot[k] = m_structPatternData[i].m_nNShot : nY_Shot[k] = m_structPatternData[i].m_nPointNumberY;			
			}
		}
	}

///////////////////////////////////////////////////////////////

/////////////////////////////////////////////////
	if(m_structDataEditor.m_bSelectDropMode == FALSE)		//150426 ehji 
	{
		for(k = m_structDataEditor.m_nPatternNum; k < _NVision.m_nCellCount; k++)
		{
			if(nY_Shot[k-1] < nY_Shot[k])
			{
				nY_Shot[k-1] = nY_Shot[k];
			}						
			else if((nY_Shot[k-1] >= nY_Shot[k]))
			{
				nY_Shot[k] = nY_Shot[k];
			}
		}
	}
////////////////////////////////////////////////
			
	for(i = 0; i < _NVision.m_nCellCount; i++)
 	{
 		_NVision.m_structCell_Info_Lc.m_nDrop_Count[i] = nDrop[i];
 		_NVision.m_structCell_Info_Lc.m_nLine_Count[i] = nLine[i];
// 		_NVision.m_structCell_Info_Lc.m_nDrop_Count_Y[i] = nY_Shot;
		_NVision.m_structCell_Info_Lc.m_nDrop_Count_Y[i] = nY_Shot[i];		//ehji 150417 MMG Pattern은 샷수가 다름.
		for(j = 0; j < MAX_NOZZLE; j++)
		{
			_NVision.m_structCell_Info_Lc.m_nHead_Line[i][j] = nHead_Line[i][j];
		}
		_NVision.m_structCell_Info_Lc.m_strCell_ID[i] = m_strGlassIden[i];
//  	dYOffset = _NVision.m_structCell_Info_Lc.m_dStart_X[i];
// 		_NVision.m_structCell_Info_Lc.m_dStart_X[i] = _NVision.m_structCell_Info_Lc.m_dEnd_X[i] * (-1);
// 		_NVision.m_structCell_Info_Lc.m_dEnd_X[i] = dYOffset * (-1);
// 		dYOffset = _NVision.m_structCell_Info_Lc.m_dStart_Y[i];
  	_NVision.m_structCell_Info_Lc.m_dStart_Y[i] *= (-1);
  	_NVision.m_structCell_Info_Lc.m_dEnd_Y[i] *= (-1);
	}
 
 	int n = 0;
 	for(i = 0; i < MAX_CELL; i++)
 	{
 		for(j = 0; j < MAX_LINE; j++)
 		{
 			_NVision.m_structCell_Info_Lc.m_dX_Center_Pos[i][j] = 0.0;
 			_NVision.m_structCell_Info_Lc.m_dLine_Y[i][j] = 0.0;

			//161214 VISION PART SCAN //JYKIM 161219 PART SCAN 추가정보 180K
			_NVision.m_structCell_Info_Lc.m_nPointHeadInform [i][j] = 0.0;
			_NVision.m_structCell_Info_Lc.m_nPointYshotInform[i][j] = 0.0;
 		}
 	}

//	150717 65+32인치 이상 관련 좌표 계산 변경 
	double dXLinePos[MAX_CELL][MAX_LINE]={0,}; //20190319 jeongyong - int->double 변경 (CELL_INFO.CSV 정상화)

	for(nTemp = 0; nTemp < _NVision.m_nCellCount; nTemp++)
	{
		n=0;
		for ( i=0; i < m_structDataEditor.m_nPatternNum; i++ )
		{
			for(int nhead = 0; nhead < MAX_NOZZLE; nhead++)		
			{
				if(m_structPatternData[i].m_bIsOnHead[nhead] == FALSE) continue;

				if(m_structPatternData[i].m_nPanelNo[nhead] - 1 == nTemp)
				{
// 					//2015.10.11 by tskim
// 					if(m_structPatternData[i].m_nDroppingStartPos == 1 || m_structPatternData[i].m_nDroppingStartPos == 0)//Left Up & Down Drop Start
// 					{
// 						for(j = m_arDropData[i].GetSize()-1; j > -1 ; j--)
// 						{
// 							stDropData = m_arDropData[i].GetAt(j);
// 
// 							if(stDropData.m_dPointX != dTemp && stDropData.m_nN == 1 && stDropData.m_nZ[nhead] == 1)
// 							{
// 								dTemp = stDropData.m_dPointX;
// 								dXLinePos[nTemp][n] = ( stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead] );
// 								n++; 
// 							}
// 						}
// 					}
// 					else if(m_structPatternData[i].m_nDroppingStartPos == 3 || m_structPatternData[i].m_nDroppingStartPos == 2)//Right Up & Down Drop Start
// 					{
						for(j = 0; j < m_arDropData[i].GetSize(); j++)
						{
							stDropData = m_arDropData[i].GetAt(j);

							if(stDropData.m_dPointX != dTemp && stDropData.m_nN == 1 && stDropData.m_nZ[nhead] == 1)
							{
								dTemp = stDropData.m_dPointX;
								dXLinePos[nTemp][n] = ( stDropData.m_dPointX + m_structPatternData[i].m_dNozzleOffset[nhead] );
								n++; 
							}
						}
//					}
				}
			}
		}
	}

 	for(i = 0; i < _NVision.m_nCellCount; i++)
 	{
 		n = 0;
 		for(nTemp = 0; nTemp < m_structDataEditor.m_nPatternNum; nTemp++)
 		{
 			for(int head = 0; head < MAX_NOZZLE; head++)
 			{
 				for(j = 0; j < m_structPatternData[nTemp].m_nPointNumberX; j++)
 				{
 					if(m_structPatternData[nTemp].m_bIsOnHead[head] == FALSE) continue;
					if(n > nLine[i]-1) break;
 					if(m_structPatternData[nTemp].m_nPanelNo[head] - 1 == i)
 					{
//150717 65+32인치 이상 관련 좌표 계산 변경 
// 						_NVision.m_structCell_Info_Lc.m_dX_Center_Pos[i][n] = (m_structPatternData[nTemp].m_dCenterPositionX + m_structPatternData[nTemp].m_dNozzleOffset[head] - m_structPatternData[nTemp].m_dWidthX / 2)
// 							+ ((m_structPatternData[nTemp].m_dWidthX / (m_structPatternData[nTemp].m_nPointNumberX - 1))*(j));
						_NVision.m_structCell_Info_Lc.m_dX_Center_Pos[i][n] = dXLinePos[i][n] ; 

 						_NVision.m_structCell_Info_Lc.m_dLine_Y[i][n] = _NVision.m_structCell_Info_Lc.m_dStart_Y[i] + m_dVision_Y_Offset[head];

//150118 INTERLOCK 
//ehjiehji
						_NVision.m_structCell_Info_Lc.m_dX_Center_Pos[i][n] += m_dVision_X_Offset[head]; 
//						//X Shift 적용 해보자 
//						if(m_structAutoOffset.m_bOnce_Apply) _NVision.m_structCell_Info_Lc.m_dX_Center_Pos[i][n] += m_dVision_X_Offset[head]; 
//						else								 _NVision.m_structCell_Info_Lc.m_dX_Center_Pos[i][n] += m_structAutoOffset.m_dS_Vision_Offset[head]; 

 //2013.12.21 by tskim 좌표 반전
 						_NVision.m_structCell_Info_Lc.m_dX_Center_Pos[i][n] = _NVision.m_structCell_Info_Lc.m_dX_Center_Pos[i][n] * (-1.0);
// 						_NVision.m_structCell_Info_Lc.m_dLine_Y[i][n] = _NVision.m_structCell_Info_Lc.m_dLine_Y[i][n] * (-1.0);

						//161214 VISION PART SCAN //JYKIM 161219 PART SCAN 180K
						_NVision.m_structCell_Info_Lc.m_nPointHeadInform [i][n] = head; 
						_NVision.m_structCell_Info_Lc.m_nPointYshotInform[i][n] = nY_Shot[i]; 

 						n++;
 					}
 				}
 			}
 		}
 	}
 	for(i = 0; i < _NVision.m_nCellCount; i++)
 	{
 		_NVision.m_structCell_Info_Lc.m_dSealLine_Start_X[i] = /*m_structDataEditor.m_dSealLine_Start[i][0]*/0.0;
 		_NVision.m_structCell_Info_Lc.m_dSealLine_Start_Y[i] = /*m_structDataEditor.m_dSealLine_Start[i][1]*/0.0;
 		_NVision.m_structCell_Info_Lc.m_dSealLine_End_X[i] = /*m_structDataEditor.m_dSealLine_End[i][0]*/0.0;
 		_NVision.m_structCell_Info_Lc.m_dSealLine_End_Y[i] = /*m_structDataEditor.m_dSealLine_End[i][1]*/0.0;
	}
	return TRUE;
}

BOOL CP8CA_LcDispDoc::Read_Vision_Y_Offset()
{
	// 일단 화일을 연다.
	ifstream fi;
	CString strPathName;
//	BOOL bOk;
	// 화일명 설정
	strPathName.Format("%s\\%s\\Vision_Y_Offset.dat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
	
	// 화일 열기, 없어도 새로 만들지 않는다.
	fi.open((char *)(LPCSTR)strPathName, ios::nocreate);
	
	// 화일이 있으면 화일을 읽는다.
	if(fi.is_open())
	{
		for(int i = 0; i < MAX_NOZZLE; i++)
		{
			fi >> m_dVision_Y_Offset[i];
		}
		fi.close();
		return TRUE;
	}
	else 
	{
		fi.close();
		return FALSE;
	}
}

BOOL CP8CA_LcDispDoc::Save_Vision_Y_Offset()
{
	FILE *fp;
	CString strPathName;
	
	strPathName.Format("%s\\%s\\Vision_Y_Offset.dat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
	fp = fopen((char *)(LPCSTR)strPathName, "wt");
	
	if(fp==NULL)
	{
		fclose(fp);
		return FALSE;
	}
	else
	{
		for(int i = 0; i < MAX_NOZZLE; i++)
		{
			fprintf(fp, "%.3f\n",m_dVision_Y_Offset[i]);
		}
	}
	fclose(fp);
	return TRUE;
}

BOOL CP8CA_LcDispDoc::Make_Recipe_for_Vision()
{
	_NVision.m_structRecipe_Info_Lc.m_strRecipe_Name =		 m_structOperatorModeRecipeData.strBackRecipeName;
	_NVision.m_structRecipe_Info_Lc.m_strRecipe_No =		 m_structOperatorModeRecipeData.strFrontRecipeName;
	_NVision.m_structRecipe_Info_Lc.m_dGlass_Size_X =		 m_structDataEditor.m_dGlassSizeX;
	_NVision.m_structRecipe_Info_Lc.m_dGlass_Size_Y =		 m_structDataEditor.m_dGlassSizeY;
	_NVision.m_structRecipe_Info_Lc.m_nTotal_Permit_Over =	 m_structN_Vision.m_nTotal_Permit_Over;
	_NVision.m_structRecipe_Info_Lc.m_nTotal_Permit_Miss =	 m_structN_Vision.m_nTotal_Permit_Miss; 
	_NVision.m_structRecipe_Info_Lc.m_nCell_Permit_Over =	 m_structN_Vision.m_nCell_Permit_Over;
	_NVision.m_structRecipe_Info_Lc.m_nCell_Permit_Miss =	 m_structN_Vision.m_nCell_Permit_Miss;
	_NVision.m_structRecipe_Info_Lc.m_nSeal_Permit_Over =	 m_structN_Vision.m_nSeal_Permit_Over;
	_NVision.m_structRecipe_Info_Lc.m_nGlass_Permit_Over =	 m_structN_Vision.m_nGlass_Permit_Over;
	_NVision.m_structRecipe_Info_Lc.m_dDist_Permit_X =		 m_structN_Vision.m_dDist_Permit_X;
	_NVision.m_structRecipe_Info_Lc.m_dDist_Permit_Y =		 m_structN_Vision.m_dDist_Permit_Y; 
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	_NVision.m_structRecipe_Info_Lc.m_dNozzle_Pitch =		 m_structN_Vision.m_dNozzle_Pitch;
	_NVision.m_structRecipe_Info_Lc.m_nK_Vel =				 m_structN_Vision.m_nK_Vel;
	
	_NVision.m_structRecipe_Info_Lc.m_nNozzle_No =			 m_structN_Vision.m_nNozzle_No;
	_NVision.m_structRecipe_Info_Lc.m_nLight_Con =			 m_structN_Vision.m_nLight_Con;
	_NVision.m_structRecipe_Info_Lc.m_bReverse =			 m_structN_Vision.m_bReverse;
	_NVision.m_structRecipe_Info_Lc.m_dCell_Rate =			 m_structN_Vision.m_nDisp_Rate;
	_NVision.m_structRecipe_Info_Lc.m_dEdge_Size =			 0.0;
	_NVision.m_structRecipe_Info_Lc.m_bEdge_Reverse =		 0;
	_NVision.m_structRecipe_Info_Lc.m_nEdge_Intnsity =		 0;
	_NVision.m_structRecipe_Info_Lc.m_nCirculaty =			 0;
	_NVision.m_structRecipe_Info_Lc.m_nDetect_Rate =         0;
	for(int i = 0; i < MAX_VISION; i++)
	{
		_NVision.m_structRecipe_Info_Lc.m_nIntensity[i] =		 m_structN_Vision.m_nIntensity[i];
		_NVision.m_structRecipe_Info_Lc.m_dDrop_Size_X[i] =		 m_structN_Vision.m_dDrop_Size_X[i]; 
		_NVision.m_structRecipe_Info_Lc.m_dDrop_Size_Y[i] =		 m_structN_Vision.m_dDrop_Size_Y[i]; 
		_NVision.m_structRecipe_Info_Lc.m_dDrop_Close[i] =		 m_structN_Vision.m_dDrop_Close[i];
	}
	_NVision.m_structScan_Info.m_bDir_TTB = m_structN_Vision.m_bDir_TTB;

	if ( _NVision.m_structScan_Info.m_bDir_TTB )
		_NVision.m_structScan_Info.m_dStartPos = m_dTeachData[26][1]; 
	else
		_NVision.m_structScan_Info.m_dStartPos = m_dTeachData[26][0]; 

	_NVision.m_structScan_Info.m_dDistance = m_structN_Vision.m_dScan_Dist;

//160909 DUAL INTENSITY 개선 //160929 JYKIM
	_NVision.m_structRecipe_Info_Lc.m_bDualIntensity = m_structDataEditor.m_bDualIntensity;

//160903 DUAL INTENSITY //160929 JYKIM
	for( i = 0; i < MAX_VISION; i++)
	{
		_NVision.m_structThreshold_Info.m_nIntensityLow[i]     = m_structN_Vision.m_nIntensity[i];
		_NVision.m_structThreshold_Info.m_nIntensityHigh[i]    = m_structN_Vision.m_nIntensityHigh[i];
		_NVision.m_structThreshold_Info.m_dDrop_Size_X_Low[i]  = m_structN_Vision.m_dDrop_Size_X[i]; 
		_NVision.m_structThreshold_Info.m_dDrop_Size_Y_Low[i] = m_structN_Vision.m_dDrop_Size_Y[i];
		_NVision.m_structThreshold_Info.m_dDrop_Size_X_High[i] = m_structN_Vision.m_dDrop_Size_High_X[i]; 
		_NVision.m_structThreshold_Info.m_dDrop_Size_Y_High[i] = m_structN_Vision.m_dDrop_Size_High_Y[i]; 
	}
//	_NVision.m_structScan_Info.m_dDistance > 0 ? _NVision.m_structScan_Info.m_bDir_TTB = true : _NVision.m_structScan_Info.m_bDir_TTB = false;

//220603 BIG DROP DETECT ( (OLD) DEEP DROP ) 	
	_NVision.m_structRecipe_Info_Lc.m_nDetectBigDrop_Rate[0] =		m_structN_Vision.m_nDetectBigDrop_Rate[0];
	_NVision.m_structRecipe_Info_Lc.m_nDetectBigDrop_Rate[1] =		m_structN_Vision.m_nDetectBigDrop_Rate[1];
	_NVision.m_structRecipe_Info_Lc.m_nDetectBigDrop_Rate[2] =		m_structN_Vision.m_nDetectBigDrop_Rate[2];

	_NVision.m_structRecipe_Info_Lc.m_nDropSizeRate_X =			m_structN_Vision.m_nDropSizeRate_X;
	_NVision.m_structRecipe_Info_Lc.m_nDropSizeRate_Y =			m_structN_Vision.m_nDropSizeRate_X;
//220603 BIG DROP DETECT ( (OLD) DEEP DROP ) 

	return TRUE;
}

void CP8CA_LcDispDoc::ReadMuitiTarget()
{
//2011.05.12 by tskim
	// MultiTarget 화일 읽기 //////////////////////////////////////////////////////
	CString strPathName;
	ifstream fi;
	int i,j;
	char ch[256];

	strPathName.Format("%s\\%s\\MultiTarget.dat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);

	// 화일 읽기
	if(fi.is_open())
	{
 		for(i = 0; i < MAX_NOZZLE; i++)
 		{
			for(j=0;j<10;j++)
			{
				fi >> ch;
				if(DataFiltering(ch) == -999999)
					m_structAdjustCondition.dMultiTarget[i][j] = m_structAdjustCondition.dMultiTarget[i][j];
				else
					m_structAdjustCondition.dMultiTarget[i][j] = m_structAdjustCondition.dMultiTarget[i][j];
			}
		}
		for(i = 0; i < MAX_NOZZLE; i++)
			fi >> m_structAdjustCondition.nMultiTargetCnt[i];
 		for(i = 0; i < MAX_NOZZLE; i++)
 		{
			for(j=0;j<10;j++)
				fi >> m_structAdjustCondition.dMultiTargetRate[i][j];
		}
	}
	else
		return;

	fi.close();
}

void CP8CA_LcDispDoc::SaveNozzleCleanerData()
{
	FILE *fp;
	CString strPathName;
	int i;
	
	strPathName.Format("%s\\N_Type_Nozzle_Clean.dat", m_strDataPath);
	fp = fopen((char *)(LPCSTR)strPathName, "wt");
	if(!fp) 
	{
		AfxMessageBox("N_Type_Nozzle_Clean.dat 파일 생성 실패");
		return;
	}	
	
	for(i = 0; i < 4; i++)
	{
		fprintf(fp, "%d\t",			Drop_Info.m_nNCleanStep[i]); //step 구분...//step1~step4
		fprintf(fp, "%d\t",			Drop_Info.m_nNCleanStepHz[i]);
		fprintf(fp, "%d\t",			Drop_Info.m_nNCleanStepAmp[i]);
		fprintf(fp, "%d\t",			Drop_Info.m_nNCleanStepNegOffset[i]);
		fprintf(fp, "%.1f\t",		Drop_Info.m_dNCleanStepDuty[i]);
		fprintf(fp, "%d\t",			Drop_Info.m_nNCleanStepFallingEdge[i]);
		fprintf(fp, "%.1f\n",			Drop_Info.m_dNCleanStepSpeed[i]);
	}

	fclose(fp);
	
	return ;
}

double CP8CA_LcDispDoc::DataFiltering(char ch[])
{
	double RtnValue;

	CString strTemp;

	strTemp.Format("%s",ch);
	int strlength;
	strlength = strTemp.GetLength();
#define JUDGE_LENGTH		20
	if(strlength > JUDGE_LENGTH)
		RtnValue = -999999;
	else
		RtnValue = atof(strTemp);
	return 	RtnValue;
}

void CP8CA_LcDispDoc::Save_Vision_PanelDropRange()
{
	FILE *fp;
	CFileException e;
	CString strFile;
	int i = 0 , j = 0 ;
	
	// Data Editor창 화일로 저장 ////////////////////////////////////////////////////////
	// 화일명 설정
	strFile.Format("%s\\%s\\Vision_PanelDropRange.dat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
	
	// 화일 열기
	fp = fopen((char *)(LPCSTR)strFile, "wt");
	
	if(fp==NULL)
	{
		fclose(fp);
		return;
	}
	else
	{
		for( i = 0 ; i < MAX_PANEL ; i++)
		{
			fprintf(fp, "%d\t%d\n",m_structVision.m_nPanelMissShot[i][0],m_structVision.m_nPanelMissShot[i][1]);
		}
	}
	fclose(fp);
}

void CP8CA_LcDispDoc::Read_Vision_PanelDropRange()
{
	ifstream fi;
	CString strPathName;
	int i,j;
	// Data Editor 화일 읽기 //////////////////////////////////////////////////////
	// 화일명 설정
	strPathName.Format("%s\\%s\\Vision_PanelDropRange.dat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
	
	// 화일 열기
	fi.open((char *)(LPCSTR)strPathName, ios::nocreate/*in*/);
	
	// 화일 읽기
	if(!fi.is_open())
	{
		fi.close();
		return;
	}
	else
	{
		for( i = 0 ; i < MAX_PANEL ; i++)
		{
			for( j = 0 ; j < 2 ; j++)
				fi >> m_structVision.m_nPanelMissShot[i][j];
		}
	}
	
	fi.close();
	
	return;
}

BOOL CP8CA_LcDispDoc::SaveDropStatus()
{
	FILE *fp;
	CString strPathName;
	
	strPathName.Format("%s\\DropStatus.dat", m_strDataPath);
	fp = fopen((char *)(LPCSTR)strPathName, "wt");
	
	if(fp==NULL)
	{
		fclose(fp);
		return FALSE;
	}
	else
	{
		fprintf(fp, "%d\n",ThreadStage.nDropStatus);
	}
	fclose(fp);
	return TRUE;
}

BOOL CP8CA_LcDispDoc::ReadDropStatus()
{
	// 일단 화일을 연다.
	ifstream fi;
	CString strPathName;
//	BOOL bOk;
	// 화일명 설정
	strPathName.Format("%s\\DropStatus.dat", m_strDataPath);
	
	// 화일 열기, 없어도 새로 만들지 않는다.
	fi.open((char *)(LPCSTR)strPathName, ios::nocreate);
	
	// 화일이 있으면 화일을 읽는다.
	if(fi.is_open())
	{
		for(int i = 0; i < MAX_NOZZLE; i++)
		{
			fi >> ThreadStage.nDropStatus;
		}
		fi.close();
		return TRUE;
	}
	else 
	{
		fi.close();
		return FALSE;
	}
}

BOOL CP8CA_LcDispDoc::Read_Vision_X_Offset()
{
	// 일단 화일을 연다.
	ifstream fi;
	CString strPathName;
//	BOOL bOk;
	// 화일명 설정
	strPathName.Format("%s\\%s\\Vision_X_Offset.dat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
	
	// 화일 열기, 없어도 새로 만들지 않는다.
	fi.open((char *)(LPCSTR)strPathName, ios::nocreate);
	
	// 화일이 있으면 화일을 읽는다.
	if(fi.is_open())
	{
		for(int i = 0; i < MAX_NOZZLE; i++)
		{
			fi >> m_dVision_X_Offset[i];
		}
		fi.close();
		return TRUE;
	}
	else 
	{
		fi.close();
		return FALSE;
	}
}

BOOL CP8CA_LcDispDoc::Read_Vision_Y_Line_Offset()
{
	// 일단 화일을 연다.
	ifstream fi;
	CString strPathName;
//	BOOL bOk;
	// 화일명 설정
	strPathName.Format("%s\\%s\\Vision_Y_Line_Offset.dat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
	
	// 화일 열기, 없어도 새로 만들지 않는다.
	fi.open((char *)(LPCSTR)strPathName, ios::nocreate);
	
	// 화일이 있으면 화일을 읽는다.
	if(fi.is_open())
	{
		for(int i = 0; i < MAX_NOZZLE; i++)
		{
			fi >> m_dVision_Y_Line_Offset[i];
		}
		fi.close();
		return TRUE;
	}
	else 
	{
		fi.close();
		return FALSE;
	}
}

BOOL CP8CA_LcDispDoc::Save_Vision_Y_Line_Offset()
{
	FILE *fp;
	CString strPathName;
	
	strPathName.Format("%s\\%s\\Vision_Y_Line_Offset.dat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
	fp = fopen((char *)(LPCSTR)strPathName, "wt");
	
	if(fp==NULL)
	{
		fclose(fp);
		return FALSE;
	}
	else
	{
		for(int i = 0; i < MAX_NOZZLE; i++)
		{
			fprintf(fp, "%.3f\n",m_dVision_Y_Line_Offset[i]);
		}
	}
	fclose(fp);
	return TRUE;
}

BOOL CP8CA_LcDispDoc::Save_Vision_X_Offset()
{
	FILE *fp;
	CString strPathName;
	
	strPathName.Format("%s\\%s\\Vision_X_Offset.dat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
	fp = fopen((char *)(LPCSTR)strPathName, "wt");
	
	if(fp==NULL)
	{
		fclose(fp);
		return FALSE;
	}
	else
	{
		for(int i = 0; i < MAX_NOZZLE; i++)
		{
			fprintf(fp, "%.3f\n",m_dVision_X_Offset[i]);
		}
	}
	fclose(fp);
	return TRUE;
}

void CP8CA_LcDispDoc::Read_LC_Consumption()
{
	ifstream fi;
	CString strPathName;
	CTime t = CTime::GetCurrentTime();
	char ch[512];
	int i,j;

	// 화일명 설정
	strPathName.Format(g_strLogPath+"\\LC_Consumption\\\\LC_Consumption_%d-%02d-%02d.log", t.GetYear(), t.GetMonth(), t.GetDay());
	
	// 화일 열기, 없어도 새로 만들지 않는다.
	fi.open((char *)(LPCSTR)strPathName, ios::nocreate);
	
	// 화일이 있으면 화일을 읽는다.
	if(fi.is_open())
	{
		for(i = 0; i < 6; i++)
		{
			for(j = 0; j < MAX_NOZZLE; j++)
			{
				fi >> ch;
				m_strLC_Consumption[i][j].Format("%s",ch);
				fi >> m_dLC_Consumption[i][j];
			}
		}
		fi.close();
		return;
	}
	else 
	{
		for(i = 0; i < 6; i++)
		{
			for(j = 0; j < MAX_NOZZLE; j++)
			{
				m_dLC_Consumption[i][j] = 0.0;
			}
		}
		fi.close();
		return;
	}
}

void CP8CA_LcDispDoc::Save_LC_Consumption()
{
	CFileFind filefind;
	FILE *fp;
	CString strPathName;

	CString strFileName, strDirName, str;
	BOOL bIsDir = FALSE;
	BOOL bContinue = TRUE;
	int i,j;

	CTime t = CTime::GetCurrentTime();

	// 디렉토리 탐색
	filefind.FindFile(g_strLogPath+"\\*.*");
	bIsDir = FALSE;
	bContinue = TRUE;
	while(bContinue)
	{
		bContinue = filefind.FindNextFile();
		if(filefind.IsDirectory())
		{
			strDirName = filefind.GetFileName();
			if(strDirName == "LC_Consumption")
			{
				bIsDir = TRUE;
				break;
			}
		}
	}
	
	filefind.Close();
	
	if(!bIsDir)	_mkdir(g_strLogPath+"\\LC_Consumption");

	strFileName.Format(g_strLogPath+"\\LC_Consumption\\LC_Consumption_%d-%02d-%02d.log", t.GetYear(), t.GetMonth(), t.GetDay());
	// 화일 열기
	fp = fopen((char *)(LPCSTR)strFileName, "wt");
	if(fp==NULL)
	{
		fclose(fp);
		return;
	}
	else
	{
		for(i = 0; i < 6; i++)
		{
			for(j = 0; j < MAX_NOZZLE; j++)
			{
				if(i==0) 
					m_strLC_Consumption[i][j].Format("DUMMY_H%02d",j+1);
				else if(i==1) 
					m_strLC_Consumption[i][j].Format("SUPPLY_H%02d",j+1);
				else if(i==2) 
					m_strLC_Consumption[i][j].Format("DEGAS_H%02d",j+1);
				else
					m_strLC_Consumption[i][j].Format("------_H%02d",j+1);

				fprintf(fp, "%s\t%.3f\n",m_strLC_Consumption[i][j],m_dLC_Consumption[i][j]);
			}
		}
	}
	fclose(fp);
}

int CP8CA_LcDispDoc::Save_SectionCalibrationData()
{
	FILE *fp;
	CString strPath ="";
	int i=0 ,j=0, k=0;

	strPath.Format("%s\\SectionCalibration.dat", m_strDataPath);	
	fp = fopen((char *)(LPCSTR)strPath, "wt");
	if(!fp) 
	{
		AfxMessageBox("SectionCalibration.dat file make faile.");
		return -1;
	}

// 	for(i = 0 ; i < MAX_MEADATA_GETCOUNT3 ;i++) 
// 	{
// 		for(j = 0; j <MAX_NOZZLE; j++)
// 		{
// 			for(k = 0; k <4; k++)
// 				m_structCalibData[j].dMeasureSection[i][k] = i+j+(k*2);
// 		}
// 	}

	fprintf(fp, "dMeasureSection[0]\n");
	for(i = 0 ; i < MAX_MEADATA_GETCOUNT3 ;i++) 
	{
		for(j = 0; j <MAX_NOZZLE; j++)
		{
			if(j==MAX_NOZZLE-1)
				fprintf(fp, "%.3f\n", m_structCalibData[j].dMeasureSection[i][0]);
			else
				fprintf(fp, "%.3f\t", m_structCalibData[j].dMeasureSection[i][0]);				
		}
	}

	fprintf(fp, "dMeasureSection[1]\n");
	for(i = 0 ; i < MAX_MEADATA_GETCOUNT3 ;i++) 
	{
		for(j = 0; j <MAX_NOZZLE; j++)
		{
			if(j==MAX_NOZZLE-1)
				fprintf(fp, "%.3f\n", m_structCalibData[j].dMeasureSection[i][1]);
			else
				fprintf(fp, "%.3f\t", m_structCalibData[j].dMeasureSection[i][1]);				
		}
	}

	fprintf(fp, "dMeasureSection[2]\n");
	for(i = 0 ; i < MAX_MEADATA_GETCOUNT3 ;i++) 
	{
		for(j = 0; j <MAX_NOZZLE; j++)
		{
			if(j==MAX_NOZZLE-1)
				fprintf(fp, "%.6f\n", m_structCalibData[j].dMeasureSection[i][2]);
			else
				fprintf(fp, "%.6f\t", m_structCalibData[j].dMeasureSection[i][2]);				
		}
	}

	fprintf(fp, "dMeasureSection[3]\n");
	for(i = 0 ; i < MAX_MEADATA_GETCOUNT3 ;i++) 
	{
		for(j = 0; j <MAX_NOZZLE; j++)
		{
			if(j==MAX_NOZZLE-1)
				fprintf(fp, "%.6f\n", m_structCalibData[j].dMeasureSection[i][3]);
			else
				fprintf(fp, "%.6f\t", m_structCalibData[j].dMeasureSection[i][3]);				
		}
	}

	fclose(fp);
	return 0;
}

int CP8CA_LcDispDoc::Read_SectionCalibrationData()
{
	ifstream fi;
	CString strPathName;
	char ch[256];
	int i=0,j=0,k=0;
	
	strPathName.Format("%s\\SectionCalibration.dat", m_strDataPath);
	fi.open((char *)(LPCSTR)strPathName, ios::in);

	if(fi.is_open())
	{
		fi >> ch;
		for(i = 0 ; i < MAX_MEADATA_GETCOUNT3 ;i++) 
		{
			for(j = 0; j <MAX_NOZZLE; j++)
			{
				fi >> m_structCalibData[j].dMeasureSection[i][0];
			}
		}

		fi >> ch;
		for(i = 0 ; i < MAX_MEADATA_GETCOUNT3 ;i++) 
		{
			for(j = 0; j <MAX_NOZZLE; j++)
			{
				fi >> m_structCalibData[j].dMeasureSection[i][1];
			}
		}

		fi >> ch;
		for(i = 0 ; i < MAX_MEADATA_GETCOUNT3 ;i++) 
		{
			for(j = 0; j <MAX_NOZZLE; j++)
			{
				fi >> m_structCalibData[j].dMeasureSection[i][2];
			}
		}

		fi >> ch;
		for(i = 0 ; i < MAX_MEADATA_GETCOUNT3 ;i++) 
		{
			for(j = 0; j <MAX_NOZZLE; j++)
			{
				fi >> m_structCalibData[j].dMeasureSection[i][3];
			}
		}
	}
	fi.close();
	return 0;
}

int CP8CA_LcDispDoc::Save_Section1stCalibrationData()
{
	FILE *fp;
	CString strPath ="";
	int i=0 ,j=0, k=0;

	strPath.Format("%s\\Section1stCalibration.dat", m_strDataPath);	
	fp = fopen((char *)(LPCSTR)strPath, "wt");
	if(!fp) 
	{
		AfxMessageBox("SectionCalibration.dat file make faile.");
		return -1;
	}

	fprintf(fp, "dMeasureFirstSection[0]\n");
	for(i = 0 ; i < MAX_MEADATA_GETCOUNT3 ;i++) 
	{
		for(j = 0; j <MAX_NOZZLE; j++)
		{
			if(j==MAX_NOZZLE-1)
				fprintf(fp, "%.3f\n", m_structCalibData[j].dMeasureFirstSection[i][0]);
			else
				fprintf(fp, "%.3f\t", m_structCalibData[j].dMeasureFirstSection[i][0]);				
		}
	}

	fprintf(fp, "dMeasureFirstSection[1]\n");
	for(i = 0 ; i < MAX_MEADATA_GETCOUNT3 ;i++) 
	{
		for(j = 0; j <MAX_NOZZLE; j++)
		{
			if(j==MAX_NOZZLE-1)
				fprintf(fp, "%.3f\n", m_structCalibData[j].dMeasureFirstSection[i][1]);
			else
				fprintf(fp, "%.3f\t", m_structCalibData[j].dMeasureFirstSection[i][1]);				
		}
	}

	fprintf(fp, "dMeasureFirstSection[2]\n");
	for(i = 0 ; i < MAX_MEADATA_GETCOUNT3 ;i++) 
	{
		for(j = 0; j <MAX_NOZZLE; j++)
		{
			if(j==MAX_NOZZLE-1)
				fprintf(fp, "%.6f\n", m_structCalibData[j].dMeasureFirstSection[i][2]);
			else
				fprintf(fp, "%.6f\t", m_structCalibData[j].dMeasureFirstSection[i][2]);				
		}
	}

	fprintf(fp, "dMeasureFirstSection[3]\n");
	for(i = 0 ; i < MAX_MEADATA_GETCOUNT3 ;i++) 
	{
		for(j = 0; j <MAX_NOZZLE; j++)
		{
			if(j==MAX_NOZZLE-1)
				fprintf(fp, "%.6f\n", m_structCalibData[j].dMeasureFirstSection[i][3]);
			else
				fprintf(fp, "%.6f\t", m_structCalibData[j].dMeasureFirstSection[i][3]);				
		}
	}
	fclose(fp);
	return 0;
}

int CP8CA_LcDispDoc::Read_Section1stCalibrationData()
{
	ifstream fi;
	CString strPathName;
	char ch[256];
	int i=0,j=0,k=0;
	
	strPathName.Format("%s\\Section1stCalibration.dat", m_strDataPath);
	fi.open((char *)(LPCSTR)strPathName, ios::in);

	if(fi.is_open())
	{

		fi >> ch;
		for(i = 0 ; i < MAX_MEADATA_GETCOUNT3 ;i++) 
		{
			for(j = 0; j <MAX_NOZZLE; j++)
			{
				fi >> m_structCalibData[j].dMeasureFirstSection[i][0];
			}
		}

		fi >> ch;
		for(i = 0 ; i < MAX_MEADATA_GETCOUNT3 ;i++) 
		{
			for(j = 0; j <MAX_NOZZLE; j++)
			{
				fi >> m_structCalibData[j].dMeasureFirstSection[i][1];
			}
		}

		fi >> ch;
		for(i = 0 ; i < MAX_MEADATA_GETCOUNT3 ;i++) 
		{
			for(j = 0; j <MAX_NOZZLE; j++)
			{
				fi >> m_structCalibData[j].dMeasureFirstSection[i][2];
			}
		}

		fi >> ch;
		for(i = 0 ; i < MAX_MEADATA_GETCOUNT3 ;i++) 
		{
			for(j = 0; j <MAX_NOZZLE; j++)
			{
				fi >> m_structCalibData[j].dMeasureFirstSection[i][3];
			}
		}
	}
	fi.close();
	return 0;
}

int CP8CA_LcDispDoc::Save_FlexibleData_First()
{
	FILE *fp;
	CString strPath ="";
	int i=0 ,j=0 ;
	int k;
//
	strPath.Format("%s\\FlexVolCalibration_First.dat", m_strDataPath);	
	fp = fopen((char *)(LPCSTR)strPath, "wt");
	if(!fp) 
	{
		AfxMessageBox("FlexVolCalibration.dat file make faile.");
		return -1;
	}

	fprintf(fp, "dMeasureFlexibleFirst[0]\n");
	for(i = 0 ; i < MAX_MEADATA_GETCOUNT2 ;i++) 
	{
		for(j = 0; j <MAX_NOZZLE; j++)
		{
			if(j==MAX_NOZZLE-1)
				fprintf(fp, "%.3f\n", m_structCalibData[j].dMeasureFlexibleFirst[i][0]);
			else
				fprintf(fp, "%.3f\t", m_structCalibData[j].dMeasureFlexibleFirst[i][0]);				
		}
	}

	fprintf(fp, "dMeasureFlexibleFirst[1]\n");
	for(i = 0 ; i < MAX_MEADATA_GETCOUNT2 ;i++) 
	{
		for(j = 0; j <MAX_NOZZLE; j++)
		{
			if(j==MAX_NOZZLE-1)
				fprintf(fp, "%.3f\n", m_structCalibData[j].dMeasureFlexibleFirst[i][1]);
			else
				fprintf(fp, "%.3f\t", m_structCalibData[j].dMeasureFlexibleFirst[i][1]);				
		}
	}

	fprintf(fp, "dMeasureFlexibleFirst[2]\n");
	for(i = 0 ; i < MAX_MEADATA_GETCOUNT2 ;i++) 
	{
		for(j = 0; j <MAX_NOZZLE; j++)
		{
			if(j==MAX_NOZZLE-1)
				fprintf(fp, "%.6f\n", m_structCalibData[j].dMeasureFlexibleFirst[i][2]);
			else
				fprintf(fp, "%.6f\t", m_structCalibData[j].dMeasureFlexibleFirst[i][2]);				
		}
	}

	fprintf(fp, "dMeasureFlexibleFirst[3]\n");
	for(i = 0 ; i < MAX_MEADATA_GETCOUNT2 ;i++) 
	{
		for(j = 0; j <MAX_NOZZLE; j++)
		{
			if(j==MAX_NOZZLE-1)
				fprintf(fp, "%.6f\n", m_structCalibData[j].dMeasureFlexibleFirst[i][3]);
			else
				fprintf(fp, "%.6f\t", m_structCalibData[j].dMeasureFlexibleFirst[i][3]);				
		}
	}

	fclose(fp);	
	return 0;
}

int CP8CA_LcDispDoc::Read_FlexibleData_First()
{
	ifstream fi;
	CString strPathName;
	char ch[256];
	int i=0,j=0,k=0;
	
	strPathName.Format("%s\\FlexVolCalibration_First.dat", m_strDataPath);
	fi.open((char *)(LPCSTR)strPathName, ios::in);

	if(fi.is_open())
	{
		fi >> ch;
		for(i = 0 ; i < MAX_MEADATA_GETCOUNT2 ;i++) 
		{
			for(j = 0; j <MAX_NOZZLE; j++)
			{
				fi >> m_structCalibData[j].dMeasureFlexibleFirst[i][0];
			}
		}

		fi >> ch;
		for(i = 0 ; i < MAX_MEADATA_GETCOUNT2 ;i++) 
		{
			for(j = 0; j <MAX_NOZZLE; j++)
			{
				fi >> m_structCalibData[j].dMeasureFlexibleFirst[i][1];
			}
		}

		fi >> ch;
		for(i = 0 ; i < MAX_MEADATA_GETCOUNT2 ;i++) 
		{
			for(j = 0; j <MAX_NOZZLE; j++)
			{
				fi >> m_structCalibData[j].dMeasureFlexibleFirst[i][2];
			}
		}

		fi >> ch;
		for(i = 0 ; i < MAX_MEADATA_GETCOUNT2 ;i++) 
		{
			for(j = 0; j <MAX_NOZZLE; j++)
			{
				fi >> m_structCalibData[j].dMeasureFlexibleFirst[i][3];
			}
		}
	}
	fi.close();
	return 0;
}

void CP8CA_LcDispDoc::Read_MotorLoad()
{
	BOOL bContinue;
	ifstream fi;
    CString strDataPath;
	CString strfileName;
	int i;

	strDataPath = _T("E:\\TOP\\MotorLoad");
	strfileName.Format("%s\\MotorLoad_TPD.dat",strDataPath);
	fi.open(strfileName, ios::in);
	
	if(fi.is_open())
	{		
		for(i =0 ; i <4 ; i++)
			fi >> m_structMotorload.m_nMotorLoadK[i];
		for(i =0 ; i <4 ; i++)
			fi >> m_structMotorload.m_nMotorLoadS[i];
		for(i =0 ; i <4 ; i++)
			fi >> m_structMotorload.m_nMotorLoadZ[i];
		fi >> m_structMotorload.m_nMotorLoadTH;
	}
	else
	{
		AfxMessageBox(" MotorLoadConfig.dat 파일을 여는것을 실패하였습니다");
		return;
	}
	
	fi.close();
}

BOOL CP8CA_LcDispDoc::Read_Action_Offset()
{
	ifstream fi;
	CString strPathName;
	strPathName.Format("%s\\%s\\Action_Time_Offset.dat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
	fi.open(strPathName, ios::in);
	
	if(!fi.is_open())
	{
		//AfxMessageBox("GlassIden.dat파일이 없습니다");
		AfxMessageBox("Cannot find Action_Time_Offset.dat");
		return FALSE;
	}
	
	for(int i = 0; i < MAX_NOZZLE; i++)
	{
		fi >> m_dMeasActionTimeOffSet[i];
	}
	
	fi.close();
	return TRUE;
}

BOOL CP8CA_LcDispDoc::Save_Action_Offset()
{
	FILE *fp;
	CString strFile;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	strFile.Format("%s\\%s\\Action_Time_Offset.dat", m_strDataPath, m_structOperatorModeRecipeData.strFrontRecipeName);
	
	// 화일 열기
	fp = fopen((char *)(LPCSTR)strFile, "wt");
	if(!fp) 
	{
		AfxMessageBox("Fail to make Action_Time_Offset.dat");
		return FALSE;
	}
	
	for(int i = 0; i < MAX_NOZZLE; i++)
	{
		fprintf(fp, "%.3f\n",			m_dMeasActionTimeOffSet[i]);
	}
	
	fclose(fp);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	return TRUE;
}
BOOL CP8CA_LcDispDoc::Save_LCRemain()
{
	FILE *fp;
	CString strPathName;
	int i,j;


	strPathName.Format("%s\\LC_Remain.dat", m_strDataPath);
	fp = fopen((char *)(LPCSTR)strPathName, "wt");

	if(fp==NULL)
	{
		fclose(fp);
		return FALSE;
	}
	else
	{
		for(i=0; i<2; i++) 
		{ 
			for(j=0; j<MAX_NOZZLE ;j++)
				fprintf(fp, "%.0f\n", m_dOffset[i][j]);
		}
		for(i=0; i<2; i++) 
		{ 
			for(j=0; j<MAX_NOZZLE ;j++)
				fprintf(fp, "%.0f\n", m_dOffset_Prev[i][j]);
		}
		for(j=0;j<MAX_NOZZLE;j++)
		{
			m_strRFDataResidualData[j].Format("%.0f",m_dOffset[0][j]);
			fprintf(fp, "%s\n", m_strRFDataResidualData[j]);
		}
		fclose(fp);
		return TRUE;
	}
}

BOOL CP8CA_LcDispDoc::Read_LCRemain()
{
	ifstream fi;
	CString strPathName;
	char ch[512];	
	int i,j;

	strPathName.Format("%s\\LC_Remain.dat", m_strDataPath);
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);

	if(fi.is_open())
	{
		for(i=0; i<2; i++) 
		{ 
			for(j=0; j<MAX_NOZZLE ;j++)
				fi >> m_dOffset[i][j];
		}
		for(i=0; i<2; i++) //혹시 모르
		{ 
			for(j=0; j<MAX_NOZZLE ;j++)
				fi >> m_dOffset_Prev[i][j];
		}
		for(i = 0 ; i < MAX_NOZZLE ; i++)
		{
			fi >> ch;
			m_strRFDataResidualData[i].Format("%s", ch);
		}
	}
	else
	{
		fi.close();
		return FALSE;
	}

	fi.close();
	return TRUE;
}

BOOL CP8CA_LcDispDoc::Read_DummyDropStatus()
{
	ifstream fi;
	CString strPathName;
	char ch[512];	
	int i,j;

	strPathName.Format("%s\\DummyDropStatus.dat", m_strDataPath);
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);

	if(fi.is_open())
	{
		for(i = 0 ; i < MAX_NOZZLE ; i++)
			fi >> m_nRFDataDummyDrop[i];
	}
	else
	{
		fi.close();
		return FALSE;
	}

	fi.close();
	return TRUE;
}

BOOL CP8CA_LcDispDoc::Save_DummyDropStatus()
{
	FILE *fp;
	CString strPathName;
	int i,j;


	strPathName.Format("%s\\DummyDropStatus.dat", m_strDataPath);
	fp = fopen((char *)(LPCSTR)strPathName, "wt");

	if(fp==NULL)
	{
		fclose(fp);
		return FALSE;
	}
	else
	{
		for(j=0;j<MAX_NOZZLE;j++)
			fprintf(fp, "%d\n", m_nRFDataDummyDrop[j]);
		fclose(fp);
		return TRUE;
	}
}

//20160227 Consumption YSM
void CP8CA_LcDispDoc::Save_LC_Consumption(int dfiledir, double val)
{
	CFileFind filefind;
	FILE *fp;
	CFileException e;
	CString strTime;

	CTime t = CTime::GetCurrentTime();

	// Glass ID Directory 생성

	CString strFileName, strDirName, str;
	BOOL bIsDir = FALSE;
	BOOL bContinue = TRUE;

	//디렉토리 탐색
	filefind.FindFile(g_strLogPath+"\\*.*");
	bIsDir = FALSE;
	bContinue = TRUE;
	while(bContinue)
	{
		bContinue = filefind.FindNextFile();
		if(filefind.IsDirectory())
		{
			strDirName = filefind.GetFileName();
			if(strDirName == "LC Consumption")
			{
				bIsDir = TRUE;
				break;
			}
		}
	}

	filefind.Close();

	if(!bIsDir)	_mkdir(g_strLogPath+"\\LC Consumption");
	// LC Consumption Directory 생성 완료

	//디렉토리 탐색
	filefind.FindFile(g_strLogPath+"\\LC Consumption"+"\\*.*");
	bIsDir = FALSE;
	bContinue = TRUE;

	strTime.Format("%d-%02d-%02d", t.GetYear(), t.GetMonth(), t.GetDay());

	while(bContinue)
	{
		bContinue = filefind.FindNextFile();
		if(filefind.IsDirectory())
		{
			strDirName = filefind.GetFileName();
			if(strDirName == strTime)
			{
				bIsDir = TRUE;
				break;
			}
		}
	}

	filefind.Close();

	if(!bIsDir)	_mkdir(g_strLogPath+"\\LC Consumption\\"+strTime);
	// LC Consumption Directory 생성 완료

	switch(dfiledir)
	{
		case 1:
			strFileName.Format(g_strLogPath+"\\LC Consumption\\"+strTime+"\\1 PTN.dat");
			break;
		case 2:
			strFileName.Format(g_strLogPath+"\\LC Consumption\\"+strTime+"\\2 Calibration.dat");
			break;
		case 3:
			strFileName.Format(g_strLogPath+"\\LC Consumption\\"+strTime+"\\3 Measure.dat");
			break;
		case 4:
			strFileName.Format(g_strLogPath+"\\LC Consumption\\"+strTime+"\\4 Measure_AUTO.dat");
			break;
		case 5:
			strFileName.Format(g_strLogPath+"\\LC Consumption\\"+strTime+"\\5 LC Suupply.dat");
			break;
		case 6:
			strFileName.Format(g_strLogPath+"\\LC Consumption\\"+strTime+"\\6 LC Degas.dat");
			break;
		case 7:
			strFileName.Format(g_strLogPath+"\\LC Consumption\\"+strTime+"\\7 Dummy Drop.dat");
			break;
		default :
			break;
	}

	// 화일 열기
	fp = fopen((char *)(LPCSTR)strFileName, "at");
	if(!fp) 
	{
		return;
	}
	
	str.Format("%s\t%lf\n",CIM_Information.GlassID_ASC, val);

	fprintf(fp, str);
	fclose(fp);
}
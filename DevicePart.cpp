// DevicePart.cpp: implementation of the CDevicePart class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"

#include "DevicePart.h"
#include "mdfunc.h"
#include "driver.h"

#include <ctype.h>
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

LONG        DriverHandle = (LONG)NULL;      // driver handle
static      PT_AIConfig     ptAIConfig;     // structure for AIConfig table
static      PT_AIVoltageIn  ptAIVoltageIn;  // structure for AIVoltageIn table
/////////////////////////////////////////////////////////////
#define MNET_CHANNEL	51
#define MNET_NETWORK	1
#define MNET_STATION	255			// 자국으로 지정
#define MNET_MODE		-1

#define MNET_TYPE_R		22
#define MNET_TYPE_B		23
#define MNET_TYPE_W		24
#define MNET_WORD		16
#define MNET_BYTE		8
#define MNET_ADDLEN		4
#define MNET_MAXBLOCK	512
#define MNET_BLOCK		16
#define MNET_RETRAY		3

long	MNETPath;
short	MNETData[512];

#define TCP_RETRAY		5

/*variable*/
double DIST_S1 ;	
double DIST_S2 ;	
double DIST_S3 ;	
double DIST_S4 ;	
double DIST_S5 ;	
double DIST_S6 ;	
double DIST_S7 ;	
double DIST_S8 ;	
double DIST_S9 ;	
double DIST_S10 ;	
double DIST_S11 ;	
double DIST_S12 ;
double DIST_S13 ;	
double DIST_S14 ;	
double DIST_S15 ;
double DIST_S16 ;

extern CCriticalSection g_critTCP;
extern BOOL g_bGateCloseAlarm[MAX_NOZZLE];

extern BOOL g_DoorLockDispOff; //20181130 jeongyong - DoorInterlock정상화

// by ckh 
#include "Define_List.h"


extern BOOL bTest;
extern BOOL m_bSWCheck;
extern BOOL g_bRemoteControl;

//2013.08.03 by tskim
extern BOOL g_bMMGLineDropMode;

extern CString g_MachineNo;
extern CString g_LineNo;
extern CString g_strLogPath;

extern double m_dPAxis_SuctionOffset[MAX_NOZZLE];//2014.12.01 by tskim Dummy Big Drop 방지 Suction 동작 테스트 용
//2014.12.01 by tskim
extern WORD wNoOfAxisP1_PAxis_SuctionOffset; 
extern WORD wNoOfAxisP2_PAxis_SuctionOffset; 
extern int iAxisP1_PAxis_SuctionOffset[MAX_NOZZLE]; 
extern int iAxisP2_PAxis_SuctionOffset[MAX_NOZZLE]; 
extern double dVelocity1P1_PAxis_SuctionOffset[MAX_NOZZLE];
extern double dVelocity1P2_PAxis_SuctionOffset[MAX_NOZZLE];
extern double dPos1P1_PAxis_SuctionOffset[MAX_NOZZLE];
extern double dPos1P2_PAxis_SuctionOffset[MAX_NOZZLE];

extern BOOL  g_bIsHeadSelected[MAX_NOZZLE];
extern double g_dTeachData[40][3];
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDevicePart::CDevicePart()
{
//	m_ActTCP = new CActQJ71E71TCP4();
	m_ActTCP = NULL;
	m_nMux1 = m_nMux2 = 0;
	m_nJobStep=m_nDropLineNO=-1;
}

CDevicePart::~CDevicePart()
{
//	delete m_ActTCP;
	m_ActTCP = NULL;
}

////////////////////////////////////////////////////////////////////////////////////
//  Functions related with Advantech AD board
////////////////////////////////////////////////////////////////////////////////////
 

 

// JHC...나중에 Loadcell 값을 입력 받아 Convert to digit 하는 함수.. 
//사용 안함//
float CDevicePart::GetAnalogLoadCell(USHORT channel)
{
	int iBdID = 2;
	float fResult = 0.0;

	long wGetAdResult;
	for(int i=0; i<10; i++){
		FAS_GetAdResult(iBdID, (WORD)channel, &wGetAdResult);
		if(wGetAdResult&0x0800){
			wGetAdResult &= 0x07ff;
			wGetAdResult = wGetAdResult*(-1);
		}
		else{
			wGetAdResult &= 0x07ff;
		}
		fResult += (float)wGetAdResult;
	}
	return	fResult/10;

}

////////////////////////////////////////////////////////////////////////////////////
//  Functions related with Motion Board
////////////////////////////////////////////////////////////////////////////////////
BOOL CDevicePart::Motion_Board_Init()
{
	int ibdid = 0 , iaxis = 0;
	if(!FAS_Open()) 
	{
		AfxMessageBox("Motion boards are not initialized.");
		return FALSE;
	}
	else
	{
		for(ibdid = 1 ; ibdid <=3 ; ibdid++)
		{
			for(iaxis = 0 ; iaxis <16 ; iaxis++)
			{
				FAS_ServoEnable(ibdid,iaxis,true);
			}
		}

		for(iaxis = 0 ; iaxis <8 ; iaxis++)
		{
			FAS_ServoEnable(4,iaxis,true);
		}

	}
	return TRUE;
}

BOOL CDevicePart::Count_Board_Init()   // Board Address Setting : 5번으로 설정함.
{
	return TRUE; //170809 lkjang 카운트 보드알람 막음 
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	int ibdid = 5;
	if(!FASC_Open()) 
	{
		AfxMessageBox("Count board is not initialized.");
	}
	else
	{
		if (FASC_IsExist(ibdid))
		{
			FASC_ResetAllCount(ibdid);
			for( int pat = 0 ; pat < pDoc->m_structDataEditor.m_nPatternNum ; pat++)
			{
				for( int head = 0 ; head < MAX_NOZZLE ; head++)
				{
					pDoc->m_nDropCount[pat][head]=0;
				}
			}			
			return TRUE;
		}
	}
	return FALSE;
}

void CDevicePart::Motion_Board_Stop()
{
	int ibdid = 0 , iaxis = 0;
	//by shin//2008.07.23//
	//P Axis는 servo off 안함.//

	for(ibdid = 1 ; ibdid <=2 ; ibdid++)
	{
		for(iaxis = 0; iaxis < 2; iaxis++)
		{
			FAS_ServoEnable(ibdid,iaxis,false); //S0~K0 //K1~K2//
		}
	}

	for(iaxis = 4; iaxis < 16; iaxis++)
	{
		FAS_ServoEnable(2,iaxis,false); //S1~S12//
	}

	for(iaxis = 0; iaxis < 16; iaxis++)
	{
		FAS_ServoEnable(3,iaxis,false); //S13~S16 //G1~G12//
	}

	for(iaxis = 0; iaxis < 8; iaxis++)
	{
		FAS_ServoEnable(4,iaxis,false); //G13~G16//
	}
}

////////////////////////////////////////////////////////////////////////////////////
//  Position Move Functions
////////////////////////////////////////////////////////////////////////////////////
void CDevicePart::StagePositionMove(double Pos_X, double Pos_Y, double dSSpeed,double dYSpeed,WORD EndCheck)
{
	int MultiAxis[2] = {0,1};
	double MultiPosition[2] = {Pos_X*S_PULSE, Pos_Y*K_PULSE};
	double Feedrate[2] = {dSSpeed,dYSpeed};


	FAS_MoveMultiAxisAbsPos(1, 2, MultiAxis, MultiPosition, Feedrate, 0);
	FAS_WaitMultiMoveDone(1,0x03);
}

void CDevicePart::StageWarmUpMove(double Pos_S, double Pos_Y,double speed_s,double speed_y,WORD EndCheck)
{
	int MultiAxis[2] = {0,1};
	double MultiPosition[2] = {Pos_S*S_PULSE,Pos_Y*K_PULSE};
	double Feedrate[2] = {speed_s,speed_y};			// speed_x , speed_y
	FAS_MoveMultiAxisAbsPos(1, 2, MultiAxis, MultiPosition, Feedrate, EndCheck);
	FAS_WaitMultiMoveDone(1,0x03);
}

//사용 안함//
void CDevicePart::StageLinearPositionMove(double Pos_X, double Pos_Y, double StageSpeed, WORD EndCheck)
{
	int MultiAxis[2] = {0,1};
	double MultiPosition[2] = {Pos_X*S_PULSE,Pos_Y*Y_PULSE};
	FAS_MoveLinearAbsPos(1, 2, MultiAxis, MultiPosition, StageSpeed, EndCheck);
}

//사용 안함//
void CDevicePart::StageDrop(double Pos_X,double Pos_Y,double StageSpeed, double DropSpeed)
{	
	WORD StageAxis[2] = {0,1};
	double StagePosition[2] = {Pos_X*S_PULSE, Pos_Y*Y_PULSE};
	double dStageStartSpeed = 1000.0 ;
	
	WORD DropNoOfAxis = 0;
	int DropAxis[MAX_NOZZLE] = {0,};	     // Drop 해야 할 축 번호를 저장
	double DropIncPos[MAX_NOZZLE] = {0,};
	double DropVelocity[MAX_NOZZLE] = {0,};
	double DropStartPos = 2000.0 ;

	//lbg 080416 ROM통합으로 인자 2개가 추가되었다.
	//Housing Type에선 무시한다.
	BOOL bIsLow = FALSE;
	DWORD SyncAxis = 0x0000;
	
	for(int i = 0 ; i < MAX_NOZZLE ; i++)
	{
		if((Drop_Info.head_onoff[i] * Drop_Info.drop_onoff[i]) == TRUE){ 
			DropAxis[DropNoOfAxis]=i+2;
			DropIncPos[DropNoOfAxis]=D_PULSE;   
			DropVelocity[DropNoOfAxis]=DropSpeed;
			DropNoOfAxis++;
		}		
	}

	
	// For Debugging...
	/*
	TRACE("Drop Axis(1 Board):");

	for(i=0; i<DropNoOfAxis;i++)
	{
		TRACE("%d\t", DropAxis[i]);
	}
	TRACE("\n");	
	*/
	FAS_MoveDropSyncAbs(1, 2, StageAxis, StagePosition, StageSpeed, dStageStartSpeed,2,  
                       DropNoOfAxis, DropAxis, DropIncPos, DropVelocity, DropStartPos, bIsLow, SyncAxis);	


	//FAS_MoveDropSyncAbs(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageAbsPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
		//	WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos, BOOL bIsLow, DWORD SyncAxis);

//	FAS_WaitMultiMoveDone(1, 0x02); // 
}

//by shin//2008.07.23//
void CDevicePart::StageLineDrop(double Pos_X,double Pos_Y,double StageSpeed, double DischargeSpeed,int point_Info)
{
	WORD StageAxis[2] = {0,1};//S0/K0
	double StagePosition[2] = {Pos_X*S_PULSE, Pos_Y*K_PULSE};

	double dStageStartSpeed = 1000.0;
	double DropStartPos = 3000.0 ;//기본 10mm에서 line drop offset 만큼 뺀다...//

	
	WORD DropNoOfAxis = 0;
	WORD DropNoOfAxis1 = 0;

	int DropAxis[NOZZLE_P1] = {0,};	     // Drop 해야 할 축 번호를 저장
	int DropAxis1[NOZZLE_P2] = {0,};	     // Drop 해야 할 축 번호를 저장

	double DropIncPos[NOZZLE_P1] = {0,};
	double DropIncPos1[NOZZLE_P2] = {0,};

	double DropVelocity[NOZZLE_P1] = {0,};
	double DropVelocity1[NOZZLE_P2] = {0,};
	
	for(int i = 0 ; i < MAX_NOZZLE ; i++)
	{
		if((Drop_Info.head_onoff[i] * Drop_Info.drop_onoff[i]) == TRUE)
		{ 
			if(i < NOZZLE_P1)
			{
				DropAxis[DropNoOfAxis]=AXIS_P1+i;
				if(point_Info!=2)
					DropIncPos[DropNoOfAxis]=Drop_Info.m_dDistPerDrop[i];
				else
					DropIncPos[DropNoOfAxis] = 0;

				TRACE("H%d #%d Point, Position(%.0f, %.0f) DropIncPos: %.0f, Speed: %d\n", i, point_Info, StagePosition[0], StagePosition[1], DropIncPos[DropNoOfAxis], Drop_Info.m_nDischargeSpeed[i]);

				DropVelocity[DropNoOfAxis]=DischargeSpeed*1000;
				DropNoOfAxis++;
			}
			else
			{
				DropAxis1[DropNoOfAxis1]=(AXIS_P15+i-NOZZLE_P1)%BOARD_AXES;
				if(point_Info!=2)
					DropIncPos1[DropNoOfAxis1]=Drop_Info.m_dDistPerDrop[i];
				else
					DropIncPos1[DropNoOfAxis1] = 0;

				TRACE("H%d #%d Point, Position(%.0f, %.0f) DropIncPos: %.0f, Speed: %d\n", i, point_Info, StagePosition[0], StagePosition[1], DropIncPos[DropNoOfAxis], Drop_Info.m_nDischargeSpeed[i]);

				DropVelocity1[DropNoOfAxis1]=DischargeSpeed*1000;
				DropNoOfAxis1++;
			}
		}
	}
	//2번 보드의 multi를 먼저 한다...
	FAS_MoveSpdMultiAxisIncPos(2, DropNoOfAxis1, DropAxis1, DropIncPos1, DropVelocity1, 0);

	FAS_MoveLineDropAbsStep(1, 2, StageAxis, StagePosition, StageSpeed, dStageStartSpeed,point_Info,
		DropNoOfAxis, DropAxis, DropIncPos, DropVelocity, DropStartPos, 7);//by shin//2008/07/23//(line drop complete bit)	

	Sleep(1);
}

void CDevicePart::ColumnPositionMove(double Pos_Column_Near, double Pos_Column_Far ,double dSpeed, WORD EndCheck)
{
	if(ThreadStage.bEmergencyStop) return; //jykim 170626 Emergencystop일 경우 리턴
	/*
	DWORD dwSetIo = 0x00000003;	
	FAS_SetIo(3,true,dwSetIo);
	FAS_SetIo(4,true,dwSetIo);
	*/
	FAS_SetIoBit(2,true,SYNC_K0,TRUE);
	FAS_SetIoBit(2,true,SYNC_K1,TRUE);
	FAS_SetIoBit(2,true,SYNC_K2,TRUE);

	double pos_col_near = 0.0 , pos_col_far = 0.0;
	pos_col_near = Pos_Column_Near * K_PULSE;
	pos_col_far = Pos_Column_Far * K_PULSE;
	//
	FAS_MoveSingleAxisAbsPos(AXIS_K_MASTER/BOARD_AXES+1, AXIS_K_MASTER%BOARD_AXES, pos_col_near, dSpeed*1000, 0);

	//by shin//2008.07.23//
	if(EndCheck != 0) 
	{
		FAS_WaitMultiMoveDone(1,0x02); // k_master
		FAS_WaitMultiMoveDone(2,0x03); // k1,k2
	}

//	if(EndCheck != 0)
//	{
//		FAS_WaitSingleMoveDone(2, 0x0000);
//	}
}


//사용 안함//
void CDevicePart::HeadZPositionMove(double Pos_Z1, double Pos_Z2, double Pos_Z3,  double Pos_Z4,  double Pos_Z5,   double Pos_Z6,  
									double Pos_Z7, double Pos_Z8, double Pos_Z9, double Pos_Z10, double Pos_Z11, double Pos_Z12 , WORD EndCheck)
										// "Hello" : 배열로 전달 해 보자.
{
	int MultiAxis[MAX_NOZZLE] = {2,3,4,5,6,7,8,9,10,11,12,13};
	double MultiPosition[MAX_NOZZLE] = {Pos_Z1*Z_PULSE, Pos_Z2*Z_PULSE, Pos_Z3*Z_PULSE, Pos_Z4*Z_PULSE, Pos_Z5*Z_PULSE, Pos_Z6*Z_PULSE, 
								Pos_Z7*Z_PULSE, Pos_Z8*Z_PULSE, Pos_Z9*Z_PULSE, Pos_Z10*Z_PULSE, Pos_Z11*Z_PULSE, Pos_Z12*Z_PULSE};
	
	double Feedrate[MAX_NOZZLE] = {100000.0,100000.0,100000.0,100000.0,100000.0,100000.0,100000.0,100000.0,100000.0,100000.0,100000.0,100000.0};
	
	FAS_MoveMultiAxisAbsPos(4, 12, MultiAxis, MultiPosition, Feedrate, EndCheck);
}

//사용 안함//
void CDevicePart::HeadZMoveCol(double Pos_Z1, double Pos_Z2, double Pos_Z3, double Pos_Z4, double Pos_Z5, double Pos_Z6, int ColumnSel, WORD EndCheck)
{
	int MultiAxis[6] = {0,};
	if(ColumnSel==0){
		for(int i=0; i<MAX_NOZZLE/2;i++)	MultiAxis[i] = i+2;
	}
	else{
		for(int i=0; i<MAX_NOZZLE/2;i++)	MultiAxis[i] = i+8;
	}
	double MultiPosition[6] = {Pos_Z1*Z_PULSE, Pos_Z2*Z_PULSE, Pos_Z3*Z_PULSE, Pos_Z4*Z_PULSE, Pos_Z5*Z_PULSE, Pos_Z6*Z_PULSE};	
	double Feedrate[6] = {100000.0,100000.0,100000.0,100000.0,100000.0,100000.0};
	
	FAS_MoveMultiAxisAbsPos(4, 6, MultiAxis, MultiPosition, Feedrate, EndCheck);
}

//by shin//2008/07/023/head_s position move방법 변경//
void CDevicePart::HeadSPositionMove(double Pos_S1, double Pos_S2 ,double Pos_S3, double Pos_S4, double Pos_S5, double Pos_S6,
									double Pos_S7, double Pos_S8 ,double Pos_S9, double Pos_S10, double Pos_S11, double Pos_S12,
									double Pos_S13, double Pos_S14, double Pos_S15, double Pos_S16,BOOL abs)
{								
	double dSpeed = 150000.0, dPos=0.0; //dSpeed = 200000.0//2010.07.02 by tskim Tact
	double dCurPos[MAX_NOZZLE], dSetPos[MAX_NOZZLE], dMovePos[MAX_NOZZLE];
	WORD wAxis[MAX_NOZZLE];
	BOOL bDirection[MAX_NOZZLE]; //dhkim 헤드 방향을 정하는 변수.
	int nOrder[MAX_NOZZLE];
	int nCount;
	double s_pos;
//	BOOL bSelectHead[MAX_NOZZLE];

	for(int i=0; i<MAX_NOZZLE;i++)
	{
		dCurPos[i]=0.0;//S축이 현재 위치.
		dSetPos[i]=0.0;
		dMovePos[i]=0.0;
		wAxis[i]=0;

		bDirection[i] = 0;//헤드 방향을 정하는 변수..0이면 왼쪽으로 이동, 1이면 오른쪽으로 이동.
		nOrder[i] = 0;//Head움지이는 순서를 정하는 변수..
//		bSelectHead[i] = false;
	}

	if( abs == FALSE )
	{
		// Data of Editor
		Pos_S1 += DIST_S1;				
		Pos_S2 += DIST_S2;
		Pos_S3 += DIST_S3;
		Pos_S4 += DIST_S4;
		Pos_S5 += DIST_S5;
		Pos_S6 += DIST_S6;
		Pos_S7 += DIST_S7;				
		Pos_S8 += DIST_S8;
		Pos_S9 += DIST_S9;
		Pos_S10 += DIST_S10;
		Pos_S11 += DIST_S11;
		Pos_S12 += DIST_S12;
		Pos_S13 += DIST_S13;				
		Pos_S14 += DIST_S14;
		Pos_S15 += DIST_S15;
		Pos_S16 += DIST_S16;
	}

	for(i = 0; i < NOZZLE_S1; i++)
	{
		FAS_GetCommandPos(2,i+4,&s_pos);
		dCurPos[i] = s_pos/1000;  //dCurPos[i]를 mm단위로 계산하기 위해서 1000을 나눈다.
	}	
	for(i = 0; i < NOZZLE_S2; i++)
	{
		FAS_GetCommandPos(3,i,&s_pos);
		dCurPos[i+NOZZLE_S1] = s_pos/1000;  //dCurPos[i]를 mm단위로 계산하기 위해서 1000을 나눈다.
	}	

	//방향성 조사,현재 위치(dCurPos[i])보다 목표위치(Pos_S1)보다 크면 오른쪽으로 이동 bDirection[0]=1 dhkim
	if(dCurPos[0] < Pos_S1)		 bDirection[0] = 1;
	if(dCurPos[1] < Pos_S2)		 bDirection[1] = 1;
	if(dCurPos[2] < Pos_S3)		 bDirection[2] = 1;
	if(dCurPos[3] < Pos_S4)		 bDirection[3] = 1;
	if(dCurPos[4] < Pos_S5)		 bDirection[4] = 1;
	if(dCurPos[5] < Pos_S6)		 bDirection[5] = 1;
	if(dCurPos[6] < Pos_S7)		 bDirection[6] = 1;
	if(dCurPos[7] < Pos_S8)		 bDirection[7] = 1;
	if(dCurPos[8] < Pos_S9)		 bDirection[8] = 1;
	if(dCurPos[9] < Pos_S10)	 bDirection[9] = 1;
	if(dCurPos[10] < Pos_S11)	 bDirection[10] = 1;
	if(dCurPos[11] < Pos_S12)	 bDirection[11] = 1;
	if(dCurPos[12] < Pos_S13)	 bDirection[12] = 1;
	if(dCurPos[13] < Pos_S14)	 bDirection[13] = 1;
	if(dCurPos[14] < Pos_S15)	 bDirection[14] = 1;
	if(dCurPos[15] < Pos_S16)	 bDirection[15] = 1;

	//////////////////////////////////////////////////////////////////////////////////////////////////
	
	//오른쪽으로 움직일때는 Head번호가 큰것부터 왼쪽으로 움직일때는 Head번호가 작은것 부터 움직인다.
	//오른쪽 방향으로 우선 움직이고 왼쪽 방향으로 움직인다.
	nCount = 0;

#if LINE1
	//오른쪽 방향으로 움직이는 경우 Check
	for(i = 0; i < MAX_NOZZLE; i++)
	{
		if(bDirection[MAX_NOZZLE-1-i])
		{
			nOrder[nCount] = MAX_NOZZLE-i;
			nCount++;
//			bSelectHead[i] = true;
		}
	}
	//왼쪽 방향으로 움직일 경우 Check
	for(i=0; i<MAX_NOZZLE; i++)
	{
		if(!bDirection[i])
		{
			nOrder[nCount++] = i+1;
//			bSelectHead[i] = false;
		}
	}
#else
	//오른쪽 방향으로 움직이는 경우 Check
	for(i = 0; i < MAX_NOZZLE; i++)
	{
		if(bDirection[MAX_NOZZLE-1-i])
		{
			nOrder[nCount] = MAX_NOZZLE-i;
			nCount++;
//			bSelectHead[i] = true;
		}
	}
	//왼쪽 방향으로 움직일 경우 Check
	for(i=0; i<MAX_NOZZLE; i++)
	{
		if(!bDirection[i])
		{
			nOrder[nCount++] = i+1;
//			bSelectHead[i] = false;
		}
	}
#endif

	double MultiPosition[MAX_NOZZLE] = { Pos_S1*S_PULSE, Pos_S2*S_PULSE, Pos_S3*S_PULSE, Pos_S4*S_PULSE, Pos_S5*S_PULSE, Pos_S6*S_PULSE,
										 Pos_S7*S_PULSE, Pos_S8*S_PULSE, Pos_S9*S_PULSE, Pos_S10*S_PULSE, Pos_S11*S_PULSE, Pos_S12*S_PULSE,
									     Pos_S13*S_PULSE, Pos_S14*S_PULSE, Pos_S15*S_PULSE, Pos_S16*S_PULSE};
	
	double Feedrate = 200000.0;
//	double Feedrate = 200000.0; // by ckh 2009.02.10 ,200000으로 설정시 모터 위치 확인 함수에서 알람이 발생됨, 변경시 일정시간 동안 대기 시간이  위치 체크 함수에 필요함.

	m_strDeviceLog.Format("\t%d\tLn%d\t%.0f\t%.0f\t%.0f\t%.0f\t%.0f\t%.0f\t%.0f\t%.0f\t%.0f\t%.0f\t%.0f\t%.0f\t%.0f\t%.0f\t%.0f\t%.0f\t"
		,m_nJobStep,m_nDropLineNO,MultiPosition[15],MultiPosition[14],MultiPosition[13],MultiPosition[12],MultiPosition[11],MultiPosition[10],MultiPosition[9],MultiPosition[8]
		,MultiPosition[7],MultiPosition[6],MultiPosition[5],MultiPosition[4],MultiPosition[3],MultiPosition[2],MultiPosition[1],MultiPosition[0]);
	TRACE(m_strDeviceLog+"\n");
	SaveDeviceLog(m_strDeviceLog);

	//방향 구분 할것...//	
	for(i=0; i<MAX_NOZZLE; i++)
	{
		if(nOrder[i]/(NOZZLE_S1+1)+2 == 3) //bd 3
		{
			FAS_MoveSingleAxisAbsPos(nOrder[i]/(NOZZLE_S1+1)+2, nOrder[i]-NOZZLE_S1-1, MultiPosition[nOrder[i]-1], Feedrate, 0);
		}
		else
		{
			FAS_MoveSingleAxisAbsPos(nOrder[i]/(NOZZLE_S1+1)+2, nOrder[i]+3, MultiPosition[nOrder[i]-1], Feedrate, 0);
		}
			
/*    
		if(i < NOZZLE_S1)
		{
			FAS_MoveSingleAxisAbsPos(2, nOrder[i]+3, MultiPosition[nOrder[i]-1], Feedrate, 0);
		}
		else
		{
			FAS_MoveSingleAxisAbsPos(3, nOrder[i]-NOZZLE_S1, MultiPosition[nOrder[i]-1], Feedrate, 0);
		}*/
		Sleep(30);//30
	}
}

///////////////////////////////////////////////////////////////////////////////////////
/////////////////////// 정위치 확인 함수
///////////////////////////////////////////////////////////////////////////////////////
// column
BOOL CDevicePart::ColumnPosition_Check(double Pos_Column_Near, double Pos_Column_Far)
{
	double pos_col_near = 0.0 , pos_col_far = 0.0;
	double pos =0.0;
	//
	pos_col_near = Pos_Column_Near * K_PULSE;
	pos_col_far = Pos_Column_Far * K_PULSE;
	//
	FAS_GetActualPos(AXIS_K1/BOARD_AXES+1, AXIS_K1%BOARD_AXES, &pos);
	if((pos < (double)(pos_col_near-500)) || (pos > (double)(pos_col_near+500))) return FALSE;

	return TRUE;
}

BOOL CDevicePart::Position_Check(int iBdID, WORD wAxis, double Pos_Pulse, double InposPulse)
{
	double Curpos=0.0;
	FAS_GetActualPos(iBdID, wAxis, &Curpos);
	if((Curpos < (double)(Pos_Pulse*K_PULSE-InposPulse)) || (Curpos > (double)(Pos_Pulse*K_PULSE+InposPulse))) return FALSE;

	return TRUE;
}
 
// S축 이동시 제위치에 갔는지 확인 루틴
BOOL CDevicePart::HeadSPosition_Check(double Pos_S1, double Pos_S2 ,double Pos_S3, double Pos_S4, double Pos_S5, double Pos_S6, double Pos_S7, 
									  double Pos_S8 , double Pos_S9, double Pos_S10 , double Pos_S11, double Pos_S12 , 
									  double Pos_S13 , double Pos_S14, double Pos_S15 , double Pos_S16 , BOOL abs)
{
	int iaxis = 0 , ibd = 0;
	double s_pos = 0.0 ;
	double s_pos_com[MAX_NOZZLE] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
	// abs == FALSE 이면...가상 원점을 기준으로 한 위치로서 인자로 넘어온 위치를 가상 원점으로 환산해 준다.
	if( abs == FALSE )
	{
		s_pos_com[0] =  (Pos_S1 + DIST_S1)*S_PULSE;
		s_pos_com[1] =  (Pos_S2 + DIST_S2)*S_PULSE;
		s_pos_com[2] =  (Pos_S3 + DIST_S3)*S_PULSE;
		s_pos_com[3] =  (Pos_S4 + DIST_S4)*S_PULSE;
		s_pos_com[4] =  (Pos_S5 + DIST_S5)*S_PULSE;
		s_pos_com[5] =  (Pos_S6 + DIST_S6)*S_PULSE;
		s_pos_com[6] =  (Pos_S7 + DIST_S7)*S_PULSE;
		s_pos_com[7] =  (Pos_S8 + DIST_S8)*S_PULSE;
		s_pos_com[8] =  (Pos_S9 + DIST_S9)*S_PULSE;
		s_pos_com[9] =  (Pos_S10 + DIST_S10)*S_PULSE;
		s_pos_com[10] = (Pos_S11 + DIST_S11)*S_PULSE;
		s_pos_com[11] = (Pos_S12 + DIST_S12)*S_PULSE;
		s_pos_com[12] = (Pos_S13 + DIST_S13)*S_PULSE;
		s_pos_com[13] = (Pos_S14 + DIST_S14)*S_PULSE;
		s_pos_com[14] = (Pos_S15 + DIST_S15)*S_PULSE;
		s_pos_com[15] = (Pos_S16 + DIST_S16)*S_PULSE;
		
		//
	}
	else	// abs == TRUE 이면...원점 센서를 기준으로 한 위치로서 인자로 넘어온 위치를 그대로 사용한다.
	{
		s_pos_com[0] =  (Pos_S1)*S_PULSE;
		s_pos_com[1] =  (Pos_S2)*S_PULSE;
		s_pos_com[2] =  (Pos_S3)*S_PULSE;
		s_pos_com[3] =  (Pos_S4)*S_PULSE;
		s_pos_com[4] =  (Pos_S5)*S_PULSE;
		s_pos_com[5] =  (Pos_S6)*S_PULSE;
		s_pos_com[6] =  (Pos_S7)*S_PULSE;
		s_pos_com[7] =  (Pos_S8)*S_PULSE;
		s_pos_com[8] =  (Pos_S9)*S_PULSE;
		s_pos_com[9] =  (Pos_S10)*S_PULSE;
		s_pos_com[10] = (Pos_S11)*S_PULSE;
		s_pos_com[11] = (Pos_S12)*S_PULSE;
		s_pos_com[12] = (Pos_S13)*S_PULSE;
		s_pos_com[13] = (Pos_S14)*S_PULSE;
		s_pos_com[14] = (Pos_S15)*S_PULSE;
		s_pos_com[15] = (Pos_S16)*S_PULSE;
	}

    Sleep(200); 

	for(iaxis = 0 ; iaxis <MAX_NOZZLE ; iaxis++)
	{
		if(iaxis < NOZZLE_S1)
		{
			FAS_GetActualPos(2, iaxis+4, &s_pos);
			//S축 알람 발생...position reading시 sleep준다...
			Sleep(10);
		}
		else
		{
			FAS_GetActualPos(3, iaxis-NOZZLE_S1, &s_pos);	
			//S축 알람 발생...position reading시 sleep준다...
			Sleep(10);
		}
//		TRACE("[s_pos]:%.3f // [s_pos_com[head_num]]:%.3f\n",s_pos,s_pos_com[head_num]);
		//S축 알람 발생...error pulse 100 -> 200
		if((s_pos < (s_pos_com[iaxis]-300.0)) || (s_pos > (s_pos_com[iaxis]+300.0))) 
			return FALSE;
	}
	return TRUE;
}

//사용 안함//
void CDevicePart::HeadDrop(double Speed)
{
	int DropAxis[MAX_NOZZLE]={0,};
	double dDropPulse[MAX_NOZZLE]={D_PULSE,D_PULSE,D_PULSE,D_PULSE,D_PULSE,D_PULSE,D_PULSE,D_PULSE,D_PULSE,D_PULSE,D_PULSE,D_PULSE};
	double dDropSpeed[MAX_NOZZLE]={Speed,Speed,Speed,Speed,Speed,Speed,Speed,Speed,Speed,Speed,Speed,Speed};

	WORD DropNoOfAxis=0;
	for(int i=0; i<MAX_NOZZLE;i++){
		if((Drop_Info.head_onoff[i] & Drop_Info.drop_onoff[i]) == TRUE){
			DropAxis[DropNoOfAxis]=i+2;
			DropNoOfAxis++;
		}
	}
	//FAS_MoveMultiAxisIncPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, WORD wEndCheck)
	FAS_MoveMultiAxisIncPos(1, DropNoOfAxis, DropAxis, dDropPulse, dDropSpeed, 1);  // "Hello"
}

//사용 안함//
void CDevicePart::HeadDummyDrop(double Speed)
{

	int DropAxis[MAX_NOZZLE]={0,};
	double dDropPulse[MAX_NOZZLE]={D_PULSE,D_PULSE,D_PULSE,D_PULSE,D_PULSE,D_PULSE,D_PULSE,D_PULSE,D_PULSE,D_PULSE,D_PULSE,D_PULSE};
	double dDropSpeed[MAX_NOZZLE]={Speed,Speed,Speed,Speed,Speed,Speed,Speed,Speed,Speed,Speed,Speed,Speed};

	WORD DropNoOfAxis=0;
	for(int i=0; i<MAX_NOZZLE;i++){
		if(Drop_Info.manu_head_job[i]== TRUE){	// "Hello"
			DropAxis[DropNoOfAxis]=i+2;
			DropNoOfAxis++;
		}
	}
	FAS_MoveMultiAxisIncPos(1, DropNoOfAxis, DropAxis, dDropPulse, dDropSpeed, 1);  
}

//사용 안함//
void CDevicePart::HeadDummyDropReverse(double Speed)
{
	int DropAxis[MAX_NOZZLE]={0,};
	double dDropPulse[MAX_NOZZLE]={-6400,-6400,-6400,-6400,-6400,-6400,-6400,-6400,-6400,-6400,-6400,-6400};
	double dDropSpeed[MAX_NOZZLE]={Speed,Speed,Speed,Speed,Speed,Speed,Speed,Speed,Speed,Speed,Speed,Speed};

	WORD DropNoOfAxis=0;
	for(int i=0; i<MAX_NOZZLE;i++){
		if(Drop_Info.manu_head_job[i]== TRUE){		
			DropAxis[DropNoOfAxis]=i+2;
			DropNoOfAxis++;
		}
	}
	FAS_MoveMultiAxisIncPos(1, DropNoOfAxis, DropAxis, dDropPulse, dDropSpeed, 1); 
}

void CDevicePart::Emergency_Stop()
{
	int ibdid = 0 , iaxis = 0;

	FAS_MoveStop(1,1,0); //K0
	FAS_MoveStop(2,0,0); //K1
	FAS_MoveStop(2,1,0); //K2
	FAS_MoveStop(1,0,0); //SO

	for(iaxis = 2 ; iaxis < MAX_NOZZLE ; iaxis++) //P1~P14//
		FAS_MoveStop(1,iaxis,0);

	for(iaxis = 2 ; iaxis < MAX_NOZZLE ; iaxis++) //P15~P16 //S1~S12//
		FAS_MoveStop(2,iaxis,0);

	for(iaxis = 0 ; iaxis < MAX_NOZZLE ; iaxis++) //S13~S16 //G1~G12//
		FAS_MoveStop(3,iaxis,0);

	for(iaxis = 0 ; iaxis < MAX_NOZZLE/2 ; iaxis++) //G13~G16//
		FAS_MoveStop(4,iaxis,0);

    MNET_WriteBit(DISP_RECEIVE, "0000");  // JHC_C : Interface Signal all off..
	MNET_WriteBit(DISP_SEND, "0000");	

	//by shin//2013.05.06//n-type//나중에 함수화 하자...귀찮아..ㅋㅋ//
	#if N_MODIFY_EQ
		//RUN STOP//
		m_pPiezoDevice->OnStop_All();

		//Standby-ALL//
		m_pPiezoDevice->Do_Standby_All(TRUE);
	#endif
	
	//FAPI int WINAPI FAS_EmergencyStop(int iBdID, WORD wAxis, WORD wEndCheck);
}

void CDevicePart::StageMoveDoneCheck()
{
	FAS_WaitMultiMoveDone(1,0x03);		// S0 & S K0
}

void CDevicePart::HeadSMoveDoneCheck()
{
	FAS_WaitMultiMoveDone(2, 0xFFF0);  // S : 1111 1111 1111 0000
	FAS_WaitMultiMoveDone(3, 0x000F);  // S : 0000 0000 0000 1111
}

//사용 안함//
void CDevicePart::HeadZMoveDoneCheck()
{
	FAS_WaitMultiMoveDone(4, 0x3FFC);  // Z : 0011 1111 1111 1100
}

////////////////////////////////////////////////////////////////////////////////////
//  I/O Functions
////////////////////////////////////////////////////////////////////////////////////
BOOL CDevicePart::Ionizer_Sol_On()
{
	BOOL io_result=0;

	FAS_SetIoBit(1,true,IONIZER_BLOW_SOL,true);
	for(int i=0;i<10;i++)
	{
		//FAS_GetIo(1,true,&io_result);
		FAS_GetIoBit(1,false,IONIZER_N2_PRESS,&io_result);
		if(io_result==TRUE) { Sleep(100); i=20; break;}// return TRUE;
		Sleep(100);
	}
	if(i==10) return FALSE;
	else return TRUE;
}

BOOL CDevicePart::Ionizer_Sol_Off()
{
//	WORD io_result;

	FAS_SetIoBit(1,true,IONIZER_BLOW_SOL,false);
/*	for(int i=0;i<3000;i++)
	{
		FAS_GetIo(2,&io_result);
		if((io_result&0x0030) == 0x0020) return TRUE;
		Sleep(0);
	}
*/	return FALSE;
}


//사양 확인 할것!!!//
int CDevicePart::Ionizer_Status_Check()
{
	//////////////////////////////////////////////////////////////////

	DWORD io_result=0;
	FAS_GetIo(1, true, &io_result);
	if(!(io_result&IONIZER1_ALARM1))	// N.C
		return 1;
	else if(io_result&IONIZER1_ALARM2)	// N.O
		return 2;
	else if(io_result&IONIZER1_ALARM3)	// N.O
		return 3;
//2015.11.23 YSM LGDCA Ionizer 1EA
// 	else if(!(io_result&IONIZER2_ALARM1))	// N.C
// 		return 4;
// 	else if(io_result&IONIZER2_ALARM2)	// N.O
// 		return 5;
// 	else if(io_result&IONIZER2_ALARM3)	// N.O
// 		return 6;
	else	
		return PASS;	// PASS : 0
}

BOOL CDevicePart::SST_Check(int but_num)
{
	BOOL io_result1=false;
//	if(g_bRemoteControl == FALSE)
//	{
#if LINE1
		FAS_GetIoBit(4,true, but_num, &io_result1);
#else
		FAS_GetIoBit(2,true, but_num, &io_result1);
#endif
		if(io_result1)
		{
			m_bSWCheck = FALSE;
			return io_result1;
		}
//	}
//	else
//	{
		if(g_bRemoteControl == TRUE)
		{
			if(m_bSWCheck == TRUE)
			{
				io_result1 =true;
			}
			else
			{
				io_result1 =false;
			}
		}
//	}
	return io_result1;
}

//사용 안함//
BOOL CDevicePart::Sylinge_Check(int head_num)  // 현찬아..수정 요함..
{
	BOOL io_result;
	FAS_GetIoBit(1,false, (head_num+1), &io_result);  //IO는 일단 맞춤
	return (io_result);
}

BOOL CDevicePart::Bit_Check(int bd_num,int bit_num)
{
	BOOL io_result;
	FAS_GetIoBit(bd_num,true, bit_num, &io_result);
	return (io_result);
}

BOOL CDevicePart::OutBit_Check(int bd_num,int bit_num)
{
	DWORD io_result;
	FAS_GetIoOutputStatus(bd_num,true,&io_result);
	if((io_result>>bit_num)&0x0001 == 1) return TRUE;
	else return FALSE;
}

BOOL CDevicePart::Stage_glass_check()
{
/*	DWORD io_result;
	FAS_GetIo(1,false,&io_result);
	if( (io_result&GLASS_DETECT) == GLASS_DETECT )	
	{
		AfxMessageBox("on");
		return TRUE;
	}
	else	return FALSE;
*/

	BOOL io_result;
//#if TEST
	FAS_GetIoBit(1,false, GLASS_DETECT, &io_result);

#if TEST
	return (io_result);
#else
	return TRUE; 
#endif 
	
//#else
//	if(bTest)  return io_result=TRUE;
//	else return io_result =FALSE;
//#endif  
}

BOOL CDevicePart::Robot_arm_check()
{
	BOOL io_result;
	FAS_GetIoBit(1,false, ROBOT_ARM_DETECT, &io_result);

#if _INTERLOCK
	return (io_result);
#else
	return FALSE; //TEST
#endif 
}

BOOL CDevicePart::Emergency_check()// A접점->B접점으로 변경 되었음..
{
	BOOL io_result1 = false;
////////////////////////////////////////////////////////////////////////////jykim 161026 Emergency stop 1,3라인 구분
#if LINE1
//	FAS_GetIoBit(4,true, EMERGENT_STOP, &io_result1);//Ready 신호가 살면 OFF
#else
	FAS_GetIoBit(4,true, READY_SWITCH, &io_result1);	// ehji 150327 
#endif
//#if EQ
//#else 
//	io_result1 = TRUE;
//#endif
	return io_result1;
}

BOOL CDevicePart::Stage_Vacuum_On()
{
	return TRUE;
//	DWORD io_result=0;
	BOOL bIo_result = false;
	
	FAS_SetIoBit(2,true,STAGE_PURGE_SOL1,false);
	FAS_SetIoBit(2,true,STAGE_PURGE_SOL2,false);
	
//	FAS_SetIoBit(2,true,STAGE_VACUUM_SOL_SP,false);

	FAS_SetIoBit(2,true,STAGE_VACUUM_SOL,false);//2010.02.12 by tskim
	
/*	for(int i=0;i<10;i++)
	{
		FAS_GetIoBit(2,true,STAGE_VACUUM_PRESS,&bIo_result);
		if(bIo_result == TRUE) 
		{
			Sleep(100); 
			i=20; 
			break;
		}
		Sleep(100);
	}

	if(i==10) return FALSE;
	else return TRUE;
*/
}

BOOL CDevicePart::Stage_Vacuum_Off()
{
	return TRUE;
	BOOL bIo_result = false;

	FAS_SetIoBit(2,true,STAGE_VACUUM_SOL,true);//2010.02.12 by tskim
/*
	for(int i=0;i<10;i++)
	{
		FAS_GetIoBit(2,true,STAGE_VACUUM_PRESS,&bIo_result);
		if(bIo_result == FALSE) 
		{
			Sleep(100); 
			i=20; 
			break;
		}
		Sleep(100);
	}

	if(i==10) return FALSE;
	else return TRUE;
*/
}


BOOL CDevicePart::Stage_Vacuum_Release_OnOff(int nOnOff)
{
	return TRUE;
//	FAS_SetIoBit(6,STAGE_N2_BLOW_SOL,true);
//	Sleep(300);
//	FAS_SetIoBit(6,STAGE_N2_BLOW_SOL,false);
//
	BOOL bIo_result=0;
	BOOL bIo_result1=0;

	FAS_SetIoBit(2,true,STAGE_VACUUM_SOL,true); //2012.02.12 by tskim

	if(nOnOff==NORMAL)
	{
		FAS_SetIoBit(2,true,STAGE_PURGE_SOL1,true);
		FAS_SetIoBit(2,true,STAGE_PURGE_SOL2,true);

//		FAS_SetIoBit(2,true,STAGE_VACUUM_SOL_SP,true);
/*
		for(int i=0;i<10;i++)
		{
			FAS_GetIoBit(2,true,STAGE_PURGE_PRESS1, &bIo_result);
			FAS_GetIoBit(2,true,STAGE_PURGE_PRESS2, &bIo_result1);
			if((bIo_result == TRUE) || (bIo_result1 == TRUE))
			{
				Sleep(100); 
				i=20; 
				break;
			}// return TRUE;
			Sleep(100);
		}
*/
		FAS_SetIoBit(2,true,STAGE_PURGE_SOL1,false);
		FAS_SetIoBit(2,true,STAGE_PURGE_SOL2,false);


//		FAS_SetIoBit(2,true,STAGE_VACUUM_SOL_SP,false);

//		if(i==10) return FALSE;
//		else return TRUE;
	}
	else if(nOnOff==ON)
	{
		FAS_SetIoBit(2,true,STAGE_PURGE_SOL1,true);
		FAS_SetIoBit(2,true,STAGE_PURGE_SOL2,true);

//		FAS_SetIoBit(2,true,STAGE_VACUUM_SOL_SP,true);
		Sleep(500);
//		FAS_GetIoBit(2,true,STAGE_PURGE_PRESS1, &bIo_result);
//		FAS_GetIoBit(2,true,STAGE_PURGE_PRESS2, &bIo_result1);
		if((bIo_result == TRUE) || (bIo_result1 == TRUE))
		{
			return TRUE;
		}
		else
			return FALSE;
	}
	else
	{
		FAS_SetIoBit(2,true,STAGE_PURGE_SOL1,false);
		FAS_SetIoBit(2,true,STAGE_PURGE_SOL2,false);

//		FAS_SetIoBit(2,true,STAGE_VACUUM_SOL_SP,false);
		return TRUE;
	}	
}


void CDevicePart::Start_button_onoff(BOOL ON_OFF)
{
	FAS_SetIoBit(4,true,START_LAMP,ON_OFF);
}

void CDevicePart::Stop_button_onoff(BOOL ON_OFF)
{	
	FAS_SetIoBit(4,true,STOP_LAMP,ON_OFF);
}

BOOL CDevicePart::SyncSetIO(int ibdno, double SetSPoint, BOOL biton, BOOL bInitial)	//현찬아..추가 내용 검토할 것...나중에 문제가 생기면 SetIo를 word로 하지말고 bit로 각각 할 것.
{
	//FAPI int WINAPI FAS_SetIo(int iBdID, WORD wIoValue);
//	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
//	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();


	DWORD dwSetIo1 = 0, dwSetIo2 = 0;// temp_value=0;
	DWORD dwIoOutputStatus1=0, dwIoOutputStatus2=0;
	BOOL bPositionError = FALSE;

//2015.01.23 by tskim 미사용헤드 Skip 기능 
#if ENABLE_HEAD_ONLY
	int i,j;
	DWORD dwHeadSetIO1;
	DWORD dwHeadSetIO2;
	DWORD dwHeadSetIO1_Reverse;
	DWORD dwHeadSetIO2_Reverse;
	DWORD dwTemp1,dwTemp2;
	dwHeadSetIO1 = 0xfff10000;															
	dwHeadSetIO2 = 0x00000000;
	for(i=0;i<2;i++)//Stage외곽으로 빠질수 있는 Head는 1,2,15,16번 만 임..
	{
		dwTemp1 = 0x00040000;		
		dwTemp1 = dwTemp1 << i;
		if(g_bIsHeadSelected[i])
		{
			dwHeadSetIO1 = dwHeadSetIO1 | dwTemp1;
			if(i==0)
				dwHeadSetIO1 = 0xfffd0000;
		}
		
		dwTemp2 = 0x00000001;
		dwTemp2 = dwTemp2 << i;
		if(g_bIsHeadSelected[NOZZLE_P1+i])
		{
			dwHeadSetIO2 = dwHeadSetIO2 | dwTemp2;	
			if(i==1)
				dwHeadSetIO2 = 0x00000003;
		}
			
	}

	if(g_WarmupPTPmode == TRUE)		// ehji 150527 Warmup 동작일때 전부 동기를 묶음. 사용하지 않는 헤드 포함. 
	{
		dwHeadSetIO1 = 0xfffd0000;//0xfff10000;															
		dwHeadSetIO2 = 0x00000003;//0x00000000;
	}

	dwHeadSetIO1_Reverse = ~dwHeadSetIO1;
	dwHeadSetIO2_Reverse = ~dwHeadSetIO2;

	if(ibdno == SYNC_AXIS_S)
	{
		if(biton == TRUE)   //동기를 묶는다..
		{		
			FAS_GetIoOutputStatus(2, true, &dwSetIo1); //S0~S14
			FAS_GetIoOutputStatus(2, false, &dwSetIo2); //S15~S16
			dwSetIo1 = dwSetIo1|dwHeadSetIO1; //0xfffd0000; 
			dwSetIo2 = dwSetIo2|dwHeadSetIO2; //0x00000003;		
			FAS_SetCommandPos(AXIS_S_MASTER/BOARD_AXES+1,AXIS_S_MASTER%BOARD_AXES, SetSPoint);	//현찬아..동기를 묶는 시점에서 Master S0에 대한 위치를 SetSPoint으로 Set한다.
			FAS_SetIo(2,true,dwSetIo1);
			FAS_SetIo(2,false,dwSetIo2);
			Sleep(20);
			FAS_GetIoOutputStatus(2,true,&dwIoOutputStatus1);
			FAS_GetIoOutputStatus(2,false,&dwIoOutputStatus2);
//			if( (0xfffd0000 == (dwIoOutputStatus1&0xfffd0000)) && (0x00000003 == (dwIoOutputStatus2&0x00000003)) ) return TRUE;
			if( (dwHeadSetIO1 == (dwIoOutputStatus1&dwHeadSetIO1)) && (dwHeadSetIO2 == (dwIoOutputStatus2&dwHeadSetIO2)) ) return TRUE;				
		}
		else 
		{
			FAS_GetIoOutputStatus(2, true, &dwSetIo1);  
 			FAS_GetIoOutputStatus(2, false, &dwSetIo2);
// TEST
//			dwSetIo1 = dwSetIo1&dwHeadSetIO1_Reverse; //0x0002ffff;
//			dwSetIo2 = dwSetIo2&dwHeadSetIO2_Reverse; //0xfffffffc;
			dwSetIo1 = dwSetIo1&0x0002ffff;
			dwSetIo2 = dwSetIo2&0xfffffffc;

//			SyncSetPos(SetSPoint);		//동기를 푼다..
			if(SyncSetPos(SetSPoint, bInitial) == FALSE) bPositionError = TRUE;
			
			FAS_SetIo(2,true,dwSetIo1);
			FAS_SetIo(2,false,dwSetIo2);
			Sleep(20);
			FAS_GetIoOutputStatus(2,true,&dwIoOutputStatus1);
			FAS_GetIoOutputStatus(2,false,&dwIoOutputStatus2);

			if(bPositionError == TRUE) return FALSE;

			if( (0x0002ffff == (dwIoOutputStatus1|0x0002ffff)) && 
				(0xfffffffc == (dwIoOutputStatus2|0xfffffffc)) ) return TRUE;	
//			if( (dwHeadSetIO1_Reverse == (dwIoOutputStatus1|dwHeadSetIO1_Reverse)) && (dwHeadSetIO2_Reverse == (dwIoOutputStatus2|dwHeadSetIO2_Reverse)) ) return TRUE;
		}
	}
	else /*if(ibdno = SYNC_AXIS_K)*/ 
	{
		FAS_GetIoOutputStatus(2, true, &dwSetIo1);
	
		dwSetIo1 = dwSetIo1|0x0000000d; 

		FAS_SetIo(2,true,dwSetIo1);
		Sleep(300);

		FAS_GetIoOutputStatus(2,true,&dwIoOutputStatus1);
		if(0x0000000d == (dwIoOutputStatus1&0x0000000d))  return TRUE;
	}
	return FALSE;
#else	
	if(ibdno == SYNC_AXIS_S)
	{
		if(biton == TRUE)   //동기를 묶는다..
		{		
			FAS_GetIoOutputStatus(2, true, &dwSetIo1); //S0~S14
			FAS_GetIoOutputStatus(2, false, &dwSetIo2); //S15~S16
			dwSetIo1 = dwSetIo1|0xfffd0000; 
			dwSetIo2 = dwSetIo2|0x00000003;		
			FAS_SetCommandPos(AXIS_S_MASTER/BOARD_AXES+1,AXIS_S_MASTER%BOARD_AXES, SetSPoint);	//현찬아..동기를 묶는 시점에서 Master S0에 대한 위치를 SetSPoint으로 Set한다.
			FAS_SetIo(2,true,dwSetIo1);
			FAS_SetIo(2,false,dwSetIo2);
			Sleep(20);
			FAS_GetIoOutputStatus(2,true,&dwIoOutputStatus1);
			FAS_GetIoOutputStatus(2,false,&dwIoOutputStatus2);
			if( (0xfffd0000 == (dwIoOutputStatus1&0xfffd0000)) && (0x00000003 == (dwIoOutputStatus2&0x00000003)) ) return TRUE;		
		}
		else 
		{
			FAS_GetIoOutputStatus(2, true, &dwSetIo1);  
			FAS_GetIoOutputStatus(2, false, &dwSetIo2);
			dwSetIo1 = dwSetIo1&0x0002ffff;
			dwSetIo2 = dwSetIo2&0xfffffffc;
//			SyncSetPos(SetSPoint);		//동기를 푼다..
			if(SyncSetPos(SetSPoint, bInitial) == FALSE) bPositionError = TRUE;
			
			FAS_SetIo(2,true,dwSetIo1);
			FAS_SetIo(2,false,dwSetIo2);
			Sleep(20);
			FAS_GetIoOutputStatus(2,true,&dwIoOutputStatus1);
			FAS_GetIoOutputStatus(2,false,&dwIoOutputStatus2);

			if(bPositionError == TRUE) return FALSE;

			if( (0x0002ffff == (dwIoOutputStatus1|0x0002ffff)) && (0xfffffffc == (dwIoOutputStatus2|0xfffffffc)) ) return TRUE;	
		}
	}
	else /*if(ibdno = SYNC_AXIS_K)*/ 
	{
		FAS_GetIoOutputStatus(2, true, &dwSetIo1);
	
		dwSetIo1 = dwSetIo1|0x0000000d; 

		FAS_SetIo(2,true,dwSetIo1);
		Sleep(300);

		FAS_GetIoOutputStatus(2,true,&dwIoOutputStatus1);
		if(0x0000000d == (dwIoOutputStatus1&0x0000000d))  return TRUE;
	}
	return FALSE;
#endif
}

BOOL CDevicePart::SyncSetPos(double SetSPoint, BOOL bInitial)   // SetSPoint : 동기를 묶기 전의 S마스터 좌표
{
	int ibdID = 0 , iaxis = 0 , i=0;
	double dPos,dPos2=0.0 , dPosSMaster=0.0;

//2014.11.29 by tskim 
	CString strLog;
	
	FAS_GetCommandPos(AXIS_S_MASTER/BOARD_AXES+1,AXIS_S_MASTER%BOARD_AXES,&dPosSMaster);

	dPosSMaster -= SetSPoint;	//  S 마스터의 이동량..
	//TRACE("dPosMaster:%f\n",dPosSMaster);
	ibdID=0;
	for(i=0; i<MAX_NOZZLE; i++)
	{
		iaxis=i;

		if(iaxis < NOZZLE_S1)
		{
			ibdID = 2;
//			FAS_GetCommandPos(ibdID,iaxis+4,&dPos);			  // 동기를 묶었을 때 당시의 좌표값을 가지고 있다...
//			FAS_SetCommandPos(ibdID,iaxis+4,dPos+dPosSMaster);  // 동기를 묶었을 때 당시의 좌표값에 S 마스터의 이동량을 더해 준다...

			FAS_GetCommandPos(ibdID,iaxis+4,&dPos2);			  // 동기를 묶었을 때 당시의 좌표값을 가지고 있다...
			FAS_GetActualPos(ibdID,iaxis+4,&dPos);			  // 동기를 묶었을 때 당시의 좌표값을 가지고 있다...
			Sleep(10);	
			FAS_SetCommandPos(ibdID,iaxis+4,dPos);  // 동기를 묶었을 때 당시의 좌표값에 S 마스터의 이동량을 더해 준다...
		}
		else
		{
			ibdID = 3;
//			FAS_GetCommandPos(ibdID,iaxis-NOZZLE_S1,&dPos);			  // 동기를 묶었을 때 당시의 좌표값을 가지고 있다...
//			FAS_SetCommandPos(ibdID,iaxis-NOZZLE_S1,dPos+dPosSMaster);  // 동기를 묶었을 때 당시의 좌표값에 S 마스터의 이동량을 더해 준다...
	
			FAS_GetCommandPos(ibdID,iaxis-NOZZLE_S1,&dPos2);			  // 동기를 묶었을 때 당시의 좌표값을 가지고 있다...
			FAS_GetActualPos(ibdID,iaxis-NOZZLE_S1,&dPos);			  // 동기를 묶었을 때 당시의 좌표값을 가지고 있다...
			Sleep(10);	
			FAS_SetCommandPos(ibdID,iaxis-NOZZLE_S1,dPos);  // 동기를 묶었을 때 당시의 좌표값에 S 마스터의 이동량을 더해 준다...	
		}		
	}
	FAS_SetCommandPos(AXIS_S_MASTER/BOARD_AXES+1,AXIS_S_MASTER%BOARD_AXES,0.0);	
	return TRUE;
}

/*
BOOL CDevicePart::SyncSetPos(double SetSPoint)   // SetSPoint : 동기를 묶기 전의 S마스터 좌표
{
	int ibdID = 0 , iaxis = 0 , i=0;
	double dPos=0.0 , dPosSMaster=0.0;
	
	FAS_GetCommandPos(1,AXIS_S_MASTER,&dPosSMaster);

	dPosSMaster -= SetSPoint;	//  S 마스터의 이동량..
	//TRACE("dPosMaster:%f\n",dPosSMaster);
	ibdID=0;
	for(i=0; i<MAX_NOZZLE; i++)
	{
		iaxis=i;

		if(iaxis < NOZZLE_S1)
		{
			ibdID = 2;
			FAS_GetCommandPos(ibdID,iaxis+4,&dPos);			  // 동기를 묶었을 때 당시의 좌표값을 가지고 있다...

			FAS_SetCommandPos(ibdID,iaxis+4,dPos+dPosSMaster);  // 동기를 묶었을 때 당시의 좌표값에 S 마스터의 이동량을 더해 준다...
		}
		else
		{
			ibdID = 3;
			FAS_GetCommandPos(ibdID,iaxis-NOZZLE_S1,&dPos);			  // 동기를 묶었을 때 당시의 좌표값을 가지고 있다...

			FAS_SetCommandPos(ibdID,iaxis-NOZZLE_S1,dPos+dPosSMaster);  // 동기를 묶었을 때 당시의 좌표값에 S 마스터의 이동량을 더해 준다...
		}		
	}
	FAS_SetCommandPos(AXIS_S_MASTER/BOARD_AXES+1,AXIS_S_MASTER%BOARD_AXES,0.0);	
	return TRUE;
}
*/
////////////////////////////////////////////////////////////////////////////////////
//  s position offset data reading
////////////////////////////////////////////////////////////////////////////////////
BOOL CDevicePart::HeadSPositionOffsetRead()
{
	CString strPathName="";
	ifstream fi;

	strPathName.Format("%s","D:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\HeadSPositionOffset.dat");	
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);

	if(fi.is_open())
	{
		fi >> DIST_S1;
		fi >> DIST_S2;
		fi >> DIST_S3;
		fi >> DIST_S4;
		fi >> DIST_S5;
		fi >> DIST_S6;
		fi >> DIST_S7;
		fi >> DIST_S8;
		fi >> DIST_S9;
		fi >> DIST_S10;
		fi >> DIST_S11;
		fi >> DIST_S12;
		fi >> DIST_S13;
		fi >> DIST_S14;
		fi >> DIST_S15;
		fi >> DIST_S16;	
		fi.close();
#if LINE3	
		DIST_S9  *= -1; 
		DIST_S10 *= -1; 
		DIST_S11 *= -1; 
		DIST_S12 *= -1; 
		DIST_S13 *= -1; 
		DIST_S14 *= -1; 
		DIST_S15 *= -1; 
		DIST_S16 *= -1; 
#endif
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////
//  Functions related with MELSECNET
////////////////////////////////////////////////////////////////////////////////////
void CDevicePart::MELSECNET_open()
{
	short	ret;

	ret = mdOpen( MNET_CHANNEL, MNET_MODE, &MNETPath);	/*	open 1st MNET10 board	*/
	if(ret) AfxMessageBox("mdOpen Error");
}

void CDevicePart::MELSECNET_close()
{
	short ret;
	ret = mdClose(MNET_CHANNEL);
	if(ret) AfxMessageBox("mdClose Error");
}


//	PLC에 Write 해줄 Hex문자열로 변환해 준다.
CString CDevicePart::MakeHexDatatoPLC(CString lText, int lLength)
{
	int			dCount;
	int			dLen;

	char		*strData;
	CString		strReturn;
	CString		strTemp;

	// 문자열보다 큰 공간은 공백으로 채운다.
	dLen = lLength - lText.GetLength();

	for ( dCount = 0; dCount < dLen; dCount++ )
	{
		lText = lText + " ";
	}

	// 새로운 메모리 영역을 잡는다.
	strData = new char[lLength+1];
	strReturn.Empty();
	strcpy(strData, lText);

	// 2문자씩 상위 하위 Byte를 변경한다.
	for ( dCount = 0; dCount < lText.GetLength(); dCount += 2 )
	{
		strTemp.Format( "%x%x", strData[dCount+1], strData[dCount] );
		strReturn = strReturn + strTemp;
	}
	delete [] strData;
	return strReturn;
}

//	PLC에서 Read한 Hex 문자열을 원상태의 문자열로 바꾸어 준다.
//	"00"은 Space로 처리한다.
CString CDevicePart::MakeStringDatafromPLC(CString lText)
{
	int			dCount;

	CString		strReturn;
	CString		strTemp;
	
	strReturn.Empty();

	for ( dCount = 0; dCount < lText.GetLength(); dCount += 4 )
	{
		strTemp = lText.Mid(dCount+2, 2);
		if ( strTemp.Compare( "00" ) == 0 )
		{
			strReturn += " ";
		}
		else
		{
			// Hex 값을 10진수로 변환후 문자로 변경한다.
			strTemp = toascii(HexToByte(strTemp));
			strReturn += strTemp;
		}

		strTemp = lText.Mid(dCount, 2);
		if ( strTemp.Compare( "00" ) == 0 )
		{
			strReturn += " ";
		}
		else
		{
			// Hex 값을 10진수로 변환후 문자로 변경한다.
			strTemp = toascii(HexToByte(strTemp));
			strReturn += strTemp;
		}
	}		
	return strReturn;
}

int CDevicePart::HexConvert(CString lHexData)
{
	int		dCovert;

	if ( lHexData.Compare("0") > 0 && lHexData.Compare("A") < 0 )
	{
		dCovert = atoi(lHexData);
	}
	else
	{
		if		( (lHexData.Compare("A") == 0)||(lHexData.Compare("a") == 0) ) dCovert = 10;
		else if ( (lHexData.Compare("B") == 0)||(lHexData.Compare("b") == 0) ) dCovert = 11;
		else if ( (lHexData.Compare("C") == 0)||(lHexData.Compare("c") == 0) ) dCovert = 12;
		else if ( (lHexData.Compare("D") == 0)||(lHexData.Compare("d") == 0) ) dCovert = 13;
		else if ( (lHexData.Compare("E") == 0)||(lHexData.Compare("e") == 0) ) dCovert = 14;
		else if ( (lHexData.Compare("F") == 0)||(lHexData.Compare("f") == 0) ) dCovert = 15;
		else dCovert = 0;
	}

	return dCovert;
}

//	Hex 값을 10진수로 변환한다.
int CDevicePart::HexToByte(CString lData)
{
	int			dFirst, dSecond;

	dFirst = HexConvert(lData.Mid(0,1));
	dSecond = HexConvert(lData.Mid(1,1));

	return (dFirst * 16 + dSecond);
}

long CDevicePart::HexToVal(CString lData)
{
	int			dLen, dCount;
	int			dRepeat;
	int			dFirst, dSecond;
	long		dTotal;

	// 문자열 길이가 홀수 일경우 짝수로 작업한다.
	dLen = lData.GetLength();
	if ( (dLen % 2) == 1 )
	{
		dLen += 1;
		lData = "0" + lData;
	}

	dRepeat = 0;
	dTotal = 0;
	for ( dCount = dLen; dCount > 0; dCount -= 2 )
	{
		dFirst = HexConvert(lData.Mid(dCount-2,1));
		dSecond = HexConvert(lData.Mid(dCount-1,1));

		dTotal = (long)(dTotal + dFirst * pow(16, dRepeat+1) + dSecond * pow(16, dRepeat));
		dRepeat = 2;
	}
	return dTotal;	
}

//	Bit영역의 한 bit를 set 또는 reset한다.
int CDevicePart::MNET_BitSet(short Address, BOOL BitFlag)
{
	if(g_bCommunicationType == FALSE){
		short	dRet, dRepeat;

		for ( dRepeat = 0; dRepeat < MNET_RETRAY; dRepeat++ )
		{
			// SHORT WINAPI mdDevSet( LONG, SHORT, SHORT, SHORT );
			if ( BitFlag )
				dRet = mdDevSet( MNETPath, MNET_STATION, MNET_TYPE_B, Address );
			else
				dRet = mdDevRst( MNETPath, MNET_STATION, MNET_TYPE_B, Address );

			if ( dRet == 0 ) break;
		}
		return dRet;
	}
	else if(g_bActiveTCP){
		return TCP_BitSet(Address, BitFlag);
	}
	else{
		return 0;
	}
}

//	PLC에서 한 Bit를 읽음.
BOOL CDevicePart::MNET_Read_A_Bit(short Address, int IBitCount)
{
//#if TEST
//#else
//	if(bTest) return FALSE;
//	else return TRUE;
//	return TRUE;
//#endif
	if( g_bCommunicationType == FALSE ){
		short	dRet, dRepeat;
		short	dWordCount = 2;
		
		for ( dRepeat = 0; dRepeat < MNET_RETRAY; dRepeat++ )
		{
			// SHORT WINAPI mdReceive( LONG, SHORT, SHORT, SHORT, PSHORT, LPVOID );
			dRet = mdReceive( MNETPath, MNET_STATION, MNET_TYPE_B, Address, &dWordCount, MNETData );
			if ( dRet == 0 ) break;
		}

		if ( dRet == 0 )
		{
			if((MNETData[0]>>IBitCount)&0x0001) return true;
			else return false;
		}
		return false;
	}
	else if(g_bActiveTCP){
		return TCP_Read_A_Bit(Address, IBitCount);
	}
	else{
		return FALSE;
	}
}

//	PLC에서 Bit 영역을 word단위로 읽음.
int CDevicePart::MNET_ReadBit_As_Word(short Address)
{
	if(g_bCommunicationType == FALSE){
		short	dRet, dRepeat;

		CString	strReturn;
		CString	strTemp;
		
		short	dWordCount;
		USHORT	dCount, dMonData;
		
		dWordCount = 2;

		for ( dRepeat = 0; dRepeat < MNET_RETRAY; dRepeat++ )
		{
			// SHORT WINAPI mdReceive( LONG, SHORT, SHORT, SHORT, PSHORT, LPVOID );
			dRet = mdReceive( MNETPath, MNET_STATION, MNET_TYPE_B, Address, &dWordCount, MNETData );
			if ( dRet == 0 ) break;
		}

		strReturn.Empty();
		if ( dRet == 0 ) 
		{
			for ( dCount=0; dCount<(dWordCount/2); dCount++ )
			{
				dMonData = (WORD)MNETData[dCount];
				strTemp.Format( "%04X",dMonData );
				strReturn += strTemp;
			}
		}
	
		//	return strReturn;
		return (int)dMonData;
	}
	else if(g_bActiveTCP){
		return(TCP_ReadBit_As_Word(Address));
	}
	else{
		return 0;
	}
}

//	Bit영역을 word 단위로 입력
int CDevicePart::MNET_WriteBit(short Address, CString BitData)
{
	if(g_bCommunicationType == FALSE){
		short	dRet, dRepeat;
		short	dWordCount;

		CString	strReturn;
		CString	strTemp;

		int		dCount, dIndex;

		dIndex = 0;
		dWordCount = BitData.GetLength() / 2;
		for (dCount=0; dCount<BitData.GetLength(); dCount+=4 )
		{
			MNETData[dIndex++] = (USHORT)HexToVal( BitData.Mid(dCount, 4));
		}

		for ( dRepeat = 0; dRepeat < MNET_RETRAY; dRepeat++ )
		{
			// SHORT WINAPI mdSend( LONG, SHORT, SHORT, SHORT, PSHORT, LPVOID );
			dRet = mdSend( MNETPath, MNET_STATION, MNET_TYPE_B, Address, &dWordCount, MNETData );
			if ( dRet == 0 ) break;
		}
		return 0;
	}
	else if(g_bActiveTCP){
		return TCP_WriteBit(Address, BitData);
	}
	else{
		return 0;
	}
}

//	PLC에서 Word 영역을 읽어옴.
CString CDevicePart::MNET_ReadWord(short Address, short Length, BOOL IHexData)		
{
	CString	strReturn;
	CString	strTemp;
	strReturn.Empty();

	if(g_bCommunicationType == FALSE){
		short	dRet, dRepeat;
		short	dWordCount;
		int		dCount, dMonData;
		dWordCount = Length * 2;

		for ( dRepeat = 0; dRepeat < MNET_RETRAY; dRepeat++ )
		{
			// SHORT WINAPI mdReceive( LONG, SHORT, SHORT, SHORT, PSHORT, LPVOID );
			dRet = mdReceive( MNETPath, MNET_STATION, MNET_TYPE_W, Address, &dWordCount, MNETData );
			if ( dRet == 0 ) break;
		}

		
		if ( dRet == 0 ) // 항상 문자로 4자리로 넘어 온다..
		{
			for ( dCount=0; dCount<Length; dCount++ )
			{
				dMonData = ( MNETData[dCount] & 0xFF00 ) / 0x100;
				strTemp.Format( "%02X", dMonData );
				strReturn += strTemp;

				dMonData = ( MNETData[dCount] & 0xFF );
				strTemp.Format( "%02X", dMonData );
				strReturn += strTemp;
			}
		}

		// Hex 문자를 문자열로 변환한다.
		if ( IHexData == false ) 
		{
			strReturn = MakeStringDatafromPLC( strReturn );
		}	
		return strReturn;
	}
	else if(g_bActiveTCP){
		return TCP_ReadWord(Address, Length, IHexData);
	}
	else{
		return "0000";
	}
}

int CDevicePart::MNET_WriteWord(short Address, CString WordData, short Length)
{
	if(g_bCommunicationType == FALSE){
		short	dRet, dRepeat;

		CString	strReturn;
		CString	strTemp;

		int		dCount, dIndex;

		dIndex = 0;
		for (dCount=0; dCount<WordData.GetLength(); dCount+=4 )
			MNETData[dIndex++] = (short)HexToVal( WordData.Mid(dCount, 4) );

		Length *= 2;
		for ( dRepeat = 0; dRepeat < MNET_RETRAY; dRepeat++ )
		{
			// SHORT WINAPI mdSend( LONG, SHORT, SHORT, SHORT, PSHORT, LPVOID );
			dRet = mdSend( MNETPath, MNET_STATION, MNET_TYPE_W, Address, &Length, MNETData );
			if ( dRet == 0 ) break;
		}
		return 0;
	}
	else if(g_bActiveTCP){
		return TCP_WriteWord(Address, WordData, Length);
	}
	else{
		return 0;
	}
}

void CDevicePart::MNET_GUI_Start()
{
	// 모드 설정=>running (Onine status)	
	MNET_BitSet(DISP_BIT, true);
//	MNET_Mode_Set(1);
	MNET_WriteBit(DISP_RECEIVE, "0000");//
	MNET_WriteBit(DISP_SEND, "0000");//
}

void CDevicePart::MNET_GUI_Stop()
{
	// 모드 설정=>down.	
	MNET_BitSet(DISP_BIT, false);
	MNET_Mode_Set(2);
}

void CDevicePart::MNET_Mode_Set(int mode)
{
	// by ckh 2009.03.09 추가함. 일단 전체 OFf
// 	MNET_BitSet(DISP_BIT+0x0A,false);//down -> FALSE
// 	MNET_BitSet(DISP_BIT+0x08,false);//run -> FALSE
// 	MNET_BitSet(DISP_BIT+0x09,false);//idle -> FALSE

	switch(mode)
	{
		case 0 :	// Run
			MNET_BitSet(DISP_BIT+0x09,false);//idle -> FALSE
			MNET_BitSet(DISP_BIT+0x0A,false);//down -> FALSE
			MNET_BitSet(DISP_BIT+0x0E,false);//경보발생 -> false
			MNET_BitSet(DISP_BIT+0x0F,false);//이상발생 -> false
			MNET_BitSet(DISP_BIT+0x08,true); //run -> TRUE
//			SetMachineMonitoring(1);

			MNET_BitSet(DISP_UNIT_SPC + STATUS_PD_DOWN , FALSE);  ////141026 PD DOWN : PD DOWN	-> FALSE;

			// 생산 준비 완료시 모드 비트 OFF( 생산관련만)
			// by ckh 2009.02.10
			MNET_BitSet(DISP_UNIT_SPC+STATUS_CHANGE_MODEL, FALSE);
			MNET_BitSet(DISP_UNIT_SPC+STATUS_CLEARING, FALSE);
			MNET_BitSet(DISP_UNIT_SPC+STATUS_CHANGE_MATERIAL, FALSE);

			break;
		case 1 :	// Idle
			MNET_BitSet(DISP_BIT+0x08,false);//run -> FALSE
			MNET_BitSet(DISP_BIT+0x0A,false);//down -> FALSE
			MNET_BitSet(DISP_BIT+0x0E,false);//경보발생 -> FALSE
			MNET_BitSet(DISP_BIT+0x0F,false);//이상발생 -> FALSE
			MNET_BitSet(DISP_BIT+0x09,true);//idle -> TRUE

			MNET_BitSet(DISP_UNIT_SPC + STATUS_PD_DOWN , FALSE);  ////141026 PD DOWN : PD DOWN	-> FALSE;

			// 생산 준비 완료시 모드 비트 OFF( 생산관련만)
			// by ckh 2009.02.10
			MNET_BitSet(DISP_UNIT_SPC+STATUS_CHANGE_MODEL, FALSE);
			MNET_BitSet(DISP_UNIT_SPC+STATUS_CLEARING, FALSE);
			MNET_BitSet(DISP_UNIT_SPC+STATUS_CHANGE_MATERIAL, FALSE);
			break;

		//141026 PD DOWN : WARNNING / ALARM 발생 경우 , PD DOWN 상태를 추기화 하지 않는다. 
		//				   PD DOWN 상태에 알람이 발생 할수 있기 때문에. 

		case 2 :	// Warning
			MNET_BitSet(DISP_BIT+0x08,false);//run -> FALSE
			MNET_BitSet(DISP_BIT+0x09,true);//idle -> FALSE
			MNET_BitSet(DISP_BIT+0x0A,false);//down -> TRUE
//			MNET_BitSet(DISP_BIT+0x0E,true);//경보발생 -> TRUE // by ckh 막음 , 경보 보고 후 Off 해야 함
			MNET_BitSet(DISP_BIT+0x0F,false);//이상발생 -> false


			// 생산 준비 완료시 모드 비트 OFF( 생산관련만)
			// by ckh 2009.02.10
			MNET_BitSet(DISP_UNIT_SPC+STATUS_CHANGE_MODEL, FALSE);
			MNET_BitSet(DISP_UNIT_SPC+STATUS_CLEARING, FALSE);
			MNET_BitSet(DISP_UNIT_SPC+STATUS_CHANGE_MATERIAL, FALSE);
			break;

		case 3 :	// Alarm
			MNET_BitSet(DISP_BIT+0x08,false);//run -> FALSE
			MNET_BitSet(DISP_BIT+0x09,false);//idle -> FALSE
			MNET_BitSet(DISP_BIT+0x0A,true);//down -> TRUE
			MNET_BitSet(DISP_BIT+0x0E,false);//경보발생 -> 
//			MNET_BitSet(DISP_BIT+0x0F,true);//이상발생 -> TRUE// by ckh 막음 , 알람 보고 후 Off 해야 함
			// 생산 준비 완료시 모드 비트 OFF( 생산관련만)
			// by ckh 2009.02.10
			MNET_BitSet(DISP_UNIT_SPC+STATUS_CHANGE_MODEL, FALSE);
			MNET_BitSet(DISP_UNIT_SPC+STATUS_CLEARING, FALSE);
			MNET_BitSet(DISP_UNIT_SPC+STATUS_CHANGE_MATERIAL, FALSE);
			break;
		//////////////////////////////////////////////////////////////////
		// by ckh 2009.02.10
		case 4 : // 모델 교환
			MNET_BitSet(DISP_BIT+0x08,false);//run -> FALSE
			MNET_BitSet(DISP_BIT+0x09,false);//idle -> FALSE

			MNET_BitSet(DISP_UNIT_SPC+STATUS_CLEARING, FALSE);
			MNET_BitSet(DISP_UNIT_SPC+STATUS_CHANGE_MATERIAL, FALSE);
			MNET_BitSet(DISP_UNIT_SPC+STATUS_CHANGE_MODEL, true);
			break;
		case 5 : // Clearing
			MNET_BitSet(DISP_BIT+0x08,false);//run -> FALSE
			MNET_BitSet(DISP_BIT+0x09,false);//idle -> FALSE

			MNET_BitSet(DISP_UNIT_SPC+STATUS_CHANGE_MODEL, FALSE);
			MNET_BitSet(DISP_UNIT_SPC+STATUS_CHANGE_MATERIAL, FALSE);
			MNET_BitSet(DISP_UNIT_SPC+STATUS_CLEARING, true);
			break;
		case 6 : // 재료 교체
			MNET_BitSet(DISP_BIT+0x08,false);//run -> FALSE
			MNET_BitSet(DISP_BIT+0x09,false);//idle -> FALSE

			MNET_BitSet(DISP_UNIT_SPC+STATUS_CHANGE_MODEL, FALSE);
			MNET_BitSet(DISP_UNIT_SPC+STATUS_CLEARING, FALSE);
			MNET_BitSet(DISP_UNIT_SPC+STATUS_CHANGE_MATERIAL, true);
			break;
		//////////////////////////////////////////////////////////////////

		case 7 : //141026 PD DOWN : ECS -> EQP PD DOWN 시 상태 변경 위해 추가  
			MNET_BitSet(DISP_BIT+0x08,false);		 //RUN		-> FALSE
			MNET_BitSet(DISP_BIT+0x09,false);		 //IDLE		-> FALSE
			MNET_BitSet(DISP_BIT+0x0E,false);		 //WARNNING -> FALSE
			MNET_BitSet(DISP_BIT+0x0A,false);		 //DOWN		-> FALSE

			MNET_BitSet(DISP_UNIT_SPC + STATUS_PD_DOWN , TRUE);  ////141026 PD DOWN : PD DOWN	-> TRUE;

			// 생산 준비 완료시 모드 비트 OFF( 생산관련만)
			// by ckh 2009.02.10
			MNET_BitSet(DISP_UNIT_SPC+STATUS_CHANGE_MODEL	, FALSE);
			MNET_BitSet(DISP_UNIT_SPC+STATUS_CLEARING		, FALSE);
			MNET_BitSet(DISP_UNIT_SPC+STATUS_CHANGE_MATERIAL, FALSE);
			break;
	}
	if(MNET_Read_A_Bit(DISP_BIT+BitDispModeStatus, 0x0C) == TRUE)	// Glass 유
	{
		MNET_BitSet(DISP_BIT+0x0D,false);//상류투입가능 -> FALSE
		MNET_WriteWord(DISP_BIT+0x0C, "0001",1);				// Glass 有枚數
		MNET_WriteWord(DISP_BIT+0x0D, "0000",1);				// Glass 투입 가능 매수
		MNET_BitSet(BaseAddr_CIM_1+0x01, true); // CIM 추가 : Disp가 Glass 있으면 1, 아니면 0
		MNET_WriteWord(BaseAddr_CIM_1+0x01,CIM_Information.LotSlot_No,1);	// 있을때 Glass 정보 표기 : Lot/Slot
	}
	else
	{
		MNET_BitSet(DISP_BIT+0x0D,true);//상류투입가능 -> TRUE
		MNET_BitSet(BaseAddr_CIM_1+0x01, false); // CIM 추가 : Disp가 Glass 있으면 1, 아니면 0
		MNET_WriteWord(DISP_BIT+0x0C, "0000",1);				// Glass 有枚數
		MNET_WriteWord(DISP_BIT+0x0D, "0001",1);				// Glass 투입 가능 매수
		MNET_WriteWord(BaseAddr_CIM_1+0x01,CIM_Information.LotSlot_No,1);	// 있을때 Glass 정보 표기 : Lot/Slot
	}
}

void CDevicePart::SetMachineMonitoring(int nStatus)
{
	switch(nStatus)
	{
	case 1:	// Run
		MNET_BitSet(BaseAddr_DISP+BitDispStatus+0x00,true);     // Run
		MNET_BitSet(BaseAddr_DISP+BitDispStatus+0x01,false);	// Stop
		MNET_BitSet(BaseAddr_DISP+BitDispStatus+0x02,false);	// Alarm
		break;
	case 2: // Idle
		MNET_BitSet(BaseAddr_DISP+BitDispStatus+0x00,false);    // Run
		MNET_BitSet(BaseAddr_DISP+BitDispStatus+0x01,true);		// Stop
		MNET_BitSet(BaseAddr_DISP+BitDispStatus+0x02,false);	// Alarm
		break;
	case 3: // Alarm
		MNET_BitSet(BaseAddr_DISP+BitDispStatus+0x00,false);    // Run
		MNET_BitSet(BaseAddr_DISP+BitDispStatus+0x01,false);	// Stop
		MNET_BitSet(BaseAddr_DISP+BitDispStatus+0x02,true);		// Alarm
		break;
	}
}

/////////////////////////////////////////
//사용 안함//
void CDevicePart::CheckHeadSOrder(double *dSetPos, double *dCurPos, double *dMovePos, WORD *wAxis)  //"Hello"..반드시 추가 할 것...
{	
	int nOrder[MAX_NOZZLE];

	for(int nC=0; nC<MAX_NOZZLE ;nC+=6)
	{
	// column 1
	/////////////////////////////////////////////////////////////////////////////// 
	if(dSetPos[0+nC] < dCurPos[1+nC]) 
	{     
		if(dSetPos[1+nC] < dCurPos[2+nC])
		{
			// 1-A Start
			if(dSetPos[2+nC] < dCurPos[3+nC])
			{
				if(dSetPos[3+nC] < dCurPos[4+nC])
				{
					if(dSetPos[4+nC] < dCurPos[5+nC])
					{
						nOrder[0+nC] = 0+nC; nOrder[1+nC] = 1+nC; nOrder[2+nC] = 2+nC;
						nOrder[3+nC] = 3+nC; nOrder[4+nC] = 4+nC; nOrder[5+nC] = 5+nC;
					}
					else
					{
						nOrder[0+nC] = 0+nC; nOrder[1+nC] = 1+nC; nOrder[2+nC] = 2+nC;
						nOrder[3+nC] = 3+nC; nOrder[4+nC] = 5+nC; nOrder[5+nC] = 4+nC;
					}
				}
				else if(dSetPos[4+nC] < dCurPos[5+nC])
				{	
					nOrder[0+nC] = 0+nC; nOrder[1+nC] = 1+nC; nOrder[2+nC] = 2+nC;
					nOrder[3+nC] = 4+nC; nOrder[4+nC] = 3+nC; nOrder[5+nC] = 5+nC;
				}
				else
				{
					nOrder[0+nC] = 0+nC; nOrder[1+nC] = 1+nC; nOrder[2+nC] = 2+nC;
					nOrder[3+nC] = 5+nC; nOrder[4+nC] = 4+nC; nOrder[5+nC] = 3+nC;
				}
			}
			else if(dSetPos[3+nC] < dCurPos[4+nC])
			{
				if(dSetPos[4+nC] < dCurPos[5+nC])
				{
					nOrder[0+nC] = 0+nC; nOrder[1+nC] = 1+nC; nOrder[2+nC] = 3+nC;
					nOrder[3+nC] = 2+nC; nOrder[4+nC] = 4+nC; nOrder[5+nC] = 5+nC;
				}
				else
				{
					nOrder[0+nC] = 0+nC; nOrder[1+nC] = 1+nC; nOrder[2+nC] = 3+nC;
					nOrder[3+nC] = 2+nC; nOrder[4+nC] = 5+nC; nOrder[5+nC] = 4+nC;
				}
			}
			else if(dSetPos[4+nC] < dCurPos[5+nC])
			{
					nOrder[0+nC] = 0+nC; nOrder[1+nC] = 1+nC; nOrder[2+nC] = 4+nC;
					nOrder[3+nC] = 3+nC; nOrder[4+nC] = 2+nC; nOrder[5+nC] = 5+nC;
			}
			else
			{
					nOrder[0+nC] = 0+nC; nOrder[1+nC] = 1+nC; nOrder[2+nC] = 5+nC;
					nOrder[3+nC] = 4+nC; nOrder[4+nC] = 3+nC; nOrder[5+nC] = 2+nC;
			}////////////////////////////////////////////// 1-A End
		} 
		else
		{
			////////////////// 1-B Start
			if(dSetPos[2+nC] < dCurPos[3+nC])
			{
				if(dSetPos[3+nC] < dCurPos[4+nC])
				{
					if(dSetPos[4+nC] < dCurPos[5+nC])
					{
						nOrder[0+nC] = 0+nC; nOrder[1+nC] = 2+nC; nOrder[2+nC] = 1+nC;
						nOrder[3+nC] = 3+nC; nOrder[4+nC] = 4+nC; nOrder[5+nC] = 5+nC;
					}
					else
					{
						nOrder[0+nC] = 0+nC; nOrder[1+nC] = 2+nC; nOrder[2+nC] = 1+nC;
						nOrder[3+nC] = 3+nC; nOrder[4+nC] = 5+nC; nOrder[5+nC] = 4+nC;
					}
				}
				else if(dSetPos[4+nC] < dCurPos[5+nC])
				{	
					nOrder[0+nC] = 0+nC; nOrder[1+nC] = 2+nC; nOrder[2+nC] = 1+nC;
					nOrder[3+nC] = 4+nC; nOrder[4+nC] = 3+nC; nOrder[5+nC] = 5+nC;
				}
				else
				{
					nOrder[0+nC] = 0+nC; nOrder[1+nC] = 2+nC; nOrder[2+nC] = 1+nC;
					nOrder[3+nC] = 5+nC; nOrder[4+nC] = 4+nC; nOrder[5+nC] = 3+nC;
				}
			}
			else if(dSetPos[3+nC] < dCurPos[4+nC])
			{
				if(dSetPos[4+nC] < dCurPos[5+nC])
				{
					nOrder[0+nC] = 0+nC; nOrder[1+nC] = 3+nC; nOrder[2+nC] = 2+nC;
					nOrder[3+nC] = 1+nC; nOrder[4+nC] = 4+nC; nOrder[5+nC] = 5+nC;
				}
				else
				{
					nOrder[0+nC] = 0+nC; nOrder[1+nC] = 3+nC; nOrder[2+nC] = 2+nC;
					nOrder[3+nC] = 1+nC; nOrder[4+nC] = 5+nC; nOrder[5+nC] = 4+nC;
				}
			}
			else if(dSetPos[4+nC] < dCurPos[5+nC])
			{
					nOrder[0+nC] = 0+nC; nOrder[1+nC] = 4+nC; nOrder[2+nC] = 3+nC;
					nOrder[3+nC] = 2+nC; nOrder[4+nC] = 1+nC; nOrder[5+nC] = 5+nC;
			}
			else
			{
					nOrder[0+nC] = 0+nC; nOrder[1+nC] = 5+nC; nOrder[2+nC] = 4+nC;
					nOrder[3+nC] = 3+nC; nOrder[4+nC] = 2+nC; nOrder[5+nC] = 1+nC;
			}
			////////////////////////////////////////////// 1-B End
		}
	} // 1-End
	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	else if(dSetPos[1+nC] < dCurPos[2+nC]) // 2 Start
	{
		////////////////// 2-A Start
		if(dSetPos[2+nC] < dCurPos[3+nC])
		{
			if(dSetPos[3+nC] < dCurPos[4+nC])
			{
				if(dSetPos[4+nC] < dCurPos[5+nC])
				{
					nOrder[0+nC] = 1+nC; nOrder[1+nC] = 0+nC; nOrder[2+nC] = 2+nC;
					nOrder[3+nC] = 3+nC; nOrder[4+nC] = 4+nC; nOrder[5+nC] = 5+nC;
				}
				else
				{
					nOrder[0+nC] = 1+nC; nOrder[1+nC] = 0+nC; nOrder[2+nC] = 2+nC;
					nOrder[3+nC] = 3+nC; nOrder[4+nC] = 5+nC; nOrder[5+nC] = 4+nC;
				}
			}
			else if(dSetPos[4+nC] < dCurPos[5+nC])
			{	
				nOrder[0+nC] = 1+nC; nOrder[1+nC] = 0+nC; nOrder[2+nC] = 2+nC;
				nOrder[3+nC] = 4+nC; nOrder[4+nC] = 3+nC; nOrder[5+nC] = 5+nC;
			}
			else
			{
				nOrder[0+nC] = 1+nC; nOrder[1+nC] = 0+nC; nOrder[2+nC] = 2+nC;
				nOrder[3+nC] = 5+nC; nOrder[4+nC] = 4+nC; nOrder[5+nC] = 3+nC;
			}
		}
		else if(dSetPos[3+nC] < dCurPos[4+nC])
		{
			if(dSetPos[4+nC] < dCurPos[5+nC])
			{
				nOrder[0+nC] = 1+nC; nOrder[1+nC] = 0+nC; nOrder[2+nC] = 3+nC;
				nOrder[3+nC] = 2+nC; nOrder[4+nC] = 4+nC; nOrder[5+nC] = 5+nC;
			}
			else
			{
				nOrder[0+nC] = 1+nC; nOrder[1+nC] = 0+nC; nOrder[2+nC] = 3+nC;
				nOrder[3+nC] = 2+nC; nOrder[4+nC] = 5+nC; nOrder[5+nC] = 4+nC;
			}
		}
		else if(dSetPos[4+nC] < dCurPos[5+nC])
		{
				nOrder[0+nC] = 1+nC; nOrder[1+nC] = 0+nC; nOrder[2+nC] = 4+nC;
				nOrder[3+nC] = 3+nC; nOrder[4+nC] = 2+nC; nOrder[5+nC] = 5+nC;
		}
		else
		{
				nOrder[0+nC] = 1+nC; nOrder[1+nC] = 0+nC; nOrder[2+nC] = 5+nC;
				nOrder[3+nC] = 4+nC; nOrder[4+nC] = 3+nC; nOrder[5+nC] = 2+nC;
		}
		////////////////////////////////////////////// 2-A End
	}// 2 End
	// 3 Start
	else
	{
		////////////////// 3-A Start
		if(dSetPos[2+nC] < dCurPos[3+nC])
		{
			if(dSetPos[3+nC] < dCurPos[4+nC])
			{
				if(dSetPos[4+nC] < dCurPos[5+nC])
				{
					nOrder[0+nC] = 2+nC; nOrder[1+nC] = 1+nC; nOrder[2+nC] = 0+nC;
					nOrder[3+nC] = 3+nC; nOrder[4+nC] = 4+nC; nOrder[5+nC] = 5+nC;
				}
				else
				{
					nOrder[0+nC] = 2+nC; nOrder[1+nC] = 1+nC; nOrder[2+nC] = 0+nC;
					nOrder[3+nC] = 3+nC; nOrder[4+nC] = 5+nC; nOrder[5+nC] = 4+nC;
				}
			}
			else if(dSetPos[4+nC] < dCurPos[5+nC])
			{	
				nOrder[0+nC] = 2+nC; nOrder[1+nC] = 1+nC; nOrder[2+nC] = 0+nC;
				nOrder[3+nC] = 4+nC; nOrder[4+nC] = 3+nC; nOrder[5+nC] = 5+nC;
			}
			else
			{
				nOrder[0+nC] = 2+nC; nOrder[1+nC] = 1+nC; nOrder[2+nC] = 0+nC;
				nOrder[3+nC] = 5+nC; nOrder[4+nC] = 4+nC; nOrder[5+nC] = 3+nC;
			}
		}
		else if(dSetPos[3+nC] < dCurPos[4+nC])
		{
			if(dSetPos[4+nC] < dCurPos[5+nC])
			{
				nOrder[0+nC] = 3+nC; nOrder[1+nC] = 2+nC; nOrder[2+nC] = 1+nC;
				nOrder[3+nC] = 0+nC; nOrder[4+nC] = 4+nC; nOrder[5+nC] = 5+nC;
			}
			else
			{
				nOrder[0+nC] = 3+nC; nOrder[1+nC] = 2+nC; nOrder[2+nC] = 1+nC;
				nOrder[3+nC] = 0+nC; nOrder[4+nC] = 5+nC; nOrder[5+nC] = 4+nC;
			}
		}
		else if(dSetPos[4+nC] < dCurPos[5+nC])
		{
				nOrder[0+nC] = 4+nC; nOrder[1+nC] = 3+nC; nOrder[2+nC] = 2+nC;
				nOrder[3+nC] = 1+nC; nOrder[4+nC] = 0+nC; nOrder[5+nC] = 5+nC;
		}
		else
		{
				nOrder[0+nC] = 5+nC; nOrder[1+nC] = 4+nC; nOrder[2+nC] = 3+nC;
				nOrder[3+nC] = 2+nC; nOrder[4+nC] = 1+nC; nOrder[5+nC] = 0+nC;
		}
		////////////////////////////////////////////// 3-A End
	}		
	// 3 End
	} // End of if(nC=0;nC<2;nC+=6)

	for(int i=0 ; i<MAX_NOZZLE ; i++)
	{
		dMovePos[i] = dSetPos[nOrder[i]];
		wAxis[i] = nOrder[i] + AXIS_S1;
	}
}

int CDevicePart::CheckDropSensor(int head)
{
	// IO 확인..
	BOOL bDropSensing=0; 

//	FAS_GetIoBit(1,false, head+DROP_CHK1, &bDropSensing);//JYKIM 180126 맺힘감지SKIP

	return bDropSensing;
}

//사용 안함//
void CDevicePart::MoveAbs(WORD axis, double pos, double vel, WORD wait)
{
	FAS_MoveSingleAxisAbsPos(2, axis, (double)pos, (double)vel, wait);
	//FAS_MoveSingleAxisAbsPos(2, axis, (double)pos, (double)vel, wait);
}

void CDevicePart::OriginS_MoveIni()
{
#if LINE1
	double dSInc=0.0;

	for(int i=MAX_NOZZLE-1;i>=0;i--)
	{
		dSInc = i*10.0*S_PULSE;

	    if(i < NOZZLE_S1)
		{
			FAS_MoveSingleAxisIncPos(2,i+4,dSInc,70000.0,0);
			Sleep(200);
		}
		else
		{
			FAS_MoveSingleAxisIncPos(3,i-NOZZLE_S1,dSInc,70000.0,0);
			Sleep(200);
		}
	}
#else
	double dSInc=0.0;
	int TempHeadOffset=1; 
	long AxisStatus;
	BOOL bMoveFlag[2] = {TRUE,TRUE};
	//by ShLee(091110)// Head 1 ~ 6 -> RLS , Head 7 ~ 12 -> FLS 에서 2 Head 씩 간격 띄우기 

	do
	{
		TempHeadOffset = 8;

		for(int nHeadCount=(MAX_NOZZLE/2-1); nHeadCount>=0 ; nHeadCount--)
		{
			if(nHeadCount!=0)		dSInc = ((nHeadCount+1)*10.0)*S_PULSE;
			else					dSInc = 10.0*S_PULSE;

			if(bMoveFlag[0]) FAS_MoveSingleAxisIncPos(2,nHeadCount+4,dSInc,200000.0,0);
			if(bMoveFlag[1]) 
			{
				if(TempHeadOffset < NOZZLE_S1)
				{
					FAS_MoveSingleAxisIncPos(2,TempHeadOffset+4,-dSInc,200000.0,0);
					Sleep(500);
				}
				else
				{
					FAS_MoveSingleAxisIncPos(3,TempHeadOffset-NOZZLE_S1,-dSInc,200000.0,0);
					Sleep(500);
				}
				
			}

			TempHeadOffset++; 
			
			Sleep(200);
		}

		for(nHeadCount=0; nHeadCount<MAX_NOZZLE ; nHeadCount++)
		{
			if ( nHeadCount < NOZZLE_S1 )
			{
				FAS_GetAxisStatus(2, nHeadCount+4, &AxisStatus);
			}
			else
			{
				FAS_GetAxisStatus(3, nHeadCount-NOZZLE_S1, &AxisStatus);
			}

			if(nHeadCount < MAX_NOZZLE / 2)
			{
				(AxisStatus & FFLAG_HWPOSILMT) || (AxisStatus & FFLAG_HWNEGALMT) ? bMoveFlag[0] = TRUE : bMoveFlag[0] = FALSE;
			}
			else 
			{
				(AxisStatus & FFLAG_HWPOSILMT) || (AxisStatus & FFLAG_HWNEGALMT) ? bMoveFlag[1] = TRUE : bMoveFlag[1] = FALSE;
			}
		}
	} 
	while( (bMoveFlag[0] || bMoveFlag[1]) == TRUE );
#endif 
}

//사용 안함//
bool CDevicePart::SetDropCommand()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	for(int i = 0 ; i < MAX_NOZZLE ; i++)	
	{
		if(pDoc->m_bIsHeadSelected[i]){ 
			FAS_SetCommandPos(pDoc->m_structHeadConfig[i].nD[0],pDoc->m_structHeadConfig[i].nD[1],0);
		}
		Sleep(100);
	}		

	return TRUE;
}


void CDevicePart::SetActTCP(CActQJ71E71TCP3 *actTCP)
{
	m_ActTCP = actTCP;
}

BOOL CDevicePart::TCP_Read_A_Bit(short szDevice, int IBitCount)
{	
	CString strDevice = _T("");
	strDevice.Format("B%4x",szDevice+(short)IBitCount);
	long	lRet=1;
	short	lpsData=0;
	
	g_critTCP.Lock();
	for ( int dRepeat = 0; dRepeat < TCP_RETRAY; dRepeat++ )
	{
		lRet = m_ActTCP->GetDevice2((LPCTSTR)strDevice, &lpsData);
		
		if (lRet == 0)	{ g_critTCP.Unlock(); return lpsData;	}
		else	Sleep(1);
	}
	g_critTCP.Unlock();

	return FALSE;
}

int CDevicePart::TCP_ReadBit_As_Word(short szDevice)
{
	CString strDevice = _T("");
	strDevice.Format("B%4x",szDevice);
	long	lRet=1;
	short	TCPData[512]={0,};


	CString	strReturn=_T("");
	CString	strTemp=_T("");
	
	short	dWordCount;
	USHORT	dCount=0, dMonData=0;
	
	//dWordCount = (short)IBitCount;
	dWordCount = 2;
	
	g_critTCP.Lock();
	for ( int dRepeat = 0; dRepeat < TCP_RETRAY; dRepeat++ )
	{
		//lRet = m_ActTCP->ReadDeviceBlock2((LPCTSTR)strDevice,IBitCount ,TCPData);
		lRet = m_ActTCP->ReadDeviceBlock2((LPCTSTR)strDevice,dWordCount ,TCPData);
		if ( lRet == 0 ) break;
		else	Sleep(100);
	}

	strReturn.Empty();
	dMonData =0;
	if ( lRet == 0 ) 
	{
		for ( dCount=0; dCount<(dWordCount/2); dCount++ )
		{
			dMonData = (WORD)TCPData[dCount];
			strTemp.Format( "%04X",dMonData );
			strReturn += strTemp;
		}
	}
	//return strReturn;  
	g_critTCP.Unlock();

	return (int)dMonData;   
}

int CDevicePart::TCP_WriteBit(short szDevice, CString BitData)
{
	CString strDevice = _T("");
	strDevice.Format("B%4x",szDevice);

	long	lRet, dRepeat;
	short	dWordCount;
	short	TCPData[512]={0,};

	CString	strReturn= _T("");
	CString	strTemp= _T("");

	int		dCount, dIndex;
	lRet=1;
	dIndex = 0;
	dWordCount = (short)(BitData.GetLength() / 4);

	g_critTCP.Lock();
	for (dCount=0; dCount<BitData.GetLength(); dCount+=4 )
	{
		TCPData[dIndex++] = (USHORT)HexToVal( BitData.Mid(dCount, 4));
	}

	for ( dRepeat = 0; dRepeat < TCP_RETRAY; dRepeat++ )
	{
		lRet = m_ActTCP->WriteDeviceBlock2((LPCTSTR)strDevice, dWordCount, TCPData);
		if ( lRet == 0 ) { g_critTCP.Unlock();	return 1; }
		else Sleep(1);
	}
	g_critTCP.Unlock();

	return 0;
}

CString CDevicePart::TCP_ReadWord(short szDevice, short Length, BOOL IHexData)
{
	CString strDevice = _T("");
	strDevice.Format("W%4x",szDevice);

	long	lRet, dRepeat;
	short	TCPData[512]={0,};
	lRet = 1;

	CString	strReturn=_T("");
	CString	strTemp=_T("");

	int		dCount, dMonData;

	g_critTCP.Lock();
	for ( dRepeat = 0; dRepeat < TCP_RETRAY; dRepeat++ )
	{
		lRet = ReadDeviceBlock2((LPCTSTR)strDevice, Length, TCPData);
		if(lRet==0)	break;
		else	Sleep(1);		
	}

	strReturn.Empty();
	if ( lRet == 0 ) 
	{
		for ( dCount=0; dCount<Length; dCount++ )
		{
			dMonData = ( TCPData[dCount] & 0xFF00 ) / 0x100;
			strTemp.Format( "%02X", dMonData );
			strReturn += strTemp;

			dMonData = ( TCPData[dCount] & 0xFF );
			strTemp.Format( "%02X", dMonData );
			strReturn += strTemp;
		}
	}

	// Hex 문자를 문자열로 변환한다.
	if ( IHexData == FALSE ) 
	{
		strReturn = MakeStringDatafromPLC( strReturn );
	}
	g_critTCP.Unlock();

	return strReturn;
}

int CDevicePart::TCP_WriteWord(short szDevice, CString WordData, short Length)
{
	CString strDevice = _T("");
	strDevice.Format("W%4x",szDevice);

	long	lRet, dRepeat;
	short	TCPData[512]={0,};

	CString	strReturn=_T("");
	CString	strTemp=_T("");

	int		dCount, dIndex;

	dIndex = 0;
	lRet=1;

	g_critTCP.Lock();
	for (dCount=0; dCount< WordData.GetLength(); dCount+=4 )
		TCPData[dIndex++] = (short)HexToVal( WordData.Mid(dCount, 4) );

	for ( dRepeat = 0; dRepeat < TCP_RETRAY; dRepeat++ )
	{
		lRet = m_ActTCP->WriteDeviceBlock2((LPCTSTR)strDevice, Length, TCPData);
		if ( lRet == 0 ) { g_critTCP.Unlock();	return 1; }
		else	Sleep(1);
	}
	g_critTCP.Unlock();

	return 0;
}

int CDevicePart::TCP_BitSet(short szDevice, BOOL BitFlag)
{
	CString strDevice = _T("");
	strDevice.Format("B%4x",szDevice);
	long lRet = 1;
	short	lpsData=0;

	g_critTCP.Lock();
	for ( int dRepeat = 0; dRepeat < TCP_RETRAY; dRepeat++ )
	{		
		lRet = m_ActTCP->GetDevice2((LPCTSTR)strDevice, &lpsData); 
		if ( lRet == 0 )	break;
		else	Sleep(1);
	}
	lRet=1;
	for ( dRepeat = 0; dRepeat < TCP_RETRAY; dRepeat++ )
	{
		if ( BitFlag ){
			lpsData = lpsData | 0x0001;
			lRet = SetDevice2((LPCTSTR)strDevice,  lpsData);
		}
		else{
			lpsData = lpsData & 0xfffe;
			lRet = SetDevice2((LPCTSTR)strDevice, lpsData);
		}
		if ( lRet == 0 ) break;
		else	Sleep(1);
	}
	g_critTCP.Unlock();

	return lRet;
}

long CDevicePart::SetDevice(LPCTSTR szDevice, long dwData)
{
	long lRet;
	lRet = m_ActTCP->SetDevice(szDevice,dwData);
	return lRet;
}

long CDevicePart::SetDevice2(LPCTSTR szDevice, short sData)
{

	long lRet;
	lRet = m_ActTCP->SetDevice2(szDevice,sData);
	return lRet;
}

long CDevicePart::SetDevice2(short szDevice, short sData)
{
	CString strDevice = _T("");
	strDevice.Format("B%4x",szDevice);
	long lRet;
	lRet = m_ActTCP->SetDevice2((LPCTSTR)strDevice,sData);
	return lRet;
}

long CDevicePart::GetDevice(LPCTSTR szDevice, long *lpdwData)
{
	long lRet;
	lRet = m_ActTCP->GetDevice(szDevice, lpdwData);
	return lRet;
}

long CDevicePart::GetDevice2(LPCTSTR szDevice, short *lpsData)
{

	long lRet;
	lRet = m_ActTCP->GetDevice2(szDevice, lpsData);
	return lRet;
}

long CDevicePart::ReadDeviceBlock(LPCTSTR szDevice, long dwSize, long *lpdwData)
{
	long lRet;
	lRet = m_ActTCP->ReadDeviceBlock(szDevice,dwSize,lpdwData);
	return lRet;

}

long CDevicePart::ReadDeviceBlock2(LPCTSTR szDevice, long lSize, short *lpsData)
{
	long lRet;
	lRet = m_ActTCP->ReadDeviceBlock2(szDevice,lSize,lpsData);
	return lRet;
}

long CDevicePart::WriteDeviceBlock(LPCTSTR szDevice, long dwSize, long *lpdwData)
{
	long lRet;
	lRet = m_ActTCP->WriteDeviceBlock(szDevice,dwSize,lpdwData);
	return lRet;
}

long CDevicePart::WriteDeviceBlock2(LPCTSTR szDevice, long lSize, short *lpsData)
{

	long lRet;
	lRet = m_ActTCP->WriteDeviceBlock2(szDevice,lSize,lpsData);
	return lRet;
}

void CDevicePart::TCP_Close()
{
	long lRet;
	lRet = m_ActTCP->Close();
	lRet == 0 ? AfxMessageBox("TCP_IP is closed successfully") : AfxMessageBox("TCP_IP is't closed successfully");
}

BOOL CDevicePart::MotionEndCheck(WORD axis)//2009.12.31 by tskim  Axis-p MotionEnd Check!!
{
#if EQ
	BOOL bMotioning = TRUE;
	int BdID = axis/BOARD_AXES + 1;
	WORD Axis = (WORD)axis%BOARD_AXES;
	long AxisStatus;

	FAS_IsMotioning(BdID, Axis, &bMotioning);
	if (0 == bMotioning)
	{
		switch(axis)
		{
		case AXIS_S_MASTER: 
		case AXIS_G1: case AXIS_G2: case AXIS_G3: case AXIS_G4: case AXIS_G5: case AXIS_G6:	case AXIS_G7: case AXIS_G8: case AXIS_G9: case AXIS_G10: case AXIS_G11: case AXIS_G12: case AXIS_G13: case AXIS_G14: case AXIS_G15: case AXIS_G16:
			return TRUE;
		default:
			while(1)
			{		
				Sleep(1);
				FAS_GetAxisStatus(BdID, Axis, &AxisStatus);
//				return TRUE;
				if(AxisStatus & FFLAG_INPOSITION)//FFLAG_MOTIONCONST
				//if(!(AxisStatus & FFLAG_MOTIONING))//FFLAG_MOTIONING
				{
					return TRUE;
//					break;//12.31 by tskim
				}
			}
		}
	}
	else
	{
		return FALSE;
	}
#else
	return TRUE;
#endif
}

void CDevicePart::HeadSPositionMove1(double Pos_S1, double Pos_S2, double Pos_S3, double Pos_S4, double Pos_S5, double Pos_S6, double Pos_S7, double Pos_S8, BOOL abs)
{
	double dSpeed = 100000.0, dPos=0.0; //dSpeed = 200000.0
	double dCurPos[MAX_NOZZLE/2], dSetPos[MAX_NOZZLE/2], dMovePos[MAX_NOZZLE/2];
	WORD wAxis[MAX_NOZZLE/2];
	BOOL bDirection[MAX_NOZZLE/2]; //dhkim 헤드 방향을 정하는 변수.
	int nOrder[MAX_NOZZLE/2];
	int nCount;
	double s_pos;
//	BOOL bSelectHead[MAX_NOZZLE];

	for(int i=0; i<MAX_NOZZLE/2;i++)
	{
		dCurPos[i]=0.0;//S축이 현재 위치.
		dSetPos[i]=0.0;
		dMovePos[i]=0.0;
		wAxis[i]=0;

		bDirection[i] = 0;//헤드 방향을 정하는 변수..0이면 왼쪽으로 이동, 1이면 오른쪽으로 이동.
		nOrder[i] = 0;//Head움지이는 순서를 정하는 변수..
//		bSelectHead[i] = false;
	}

	if( abs == FALSE )
	{
		// Data of Editor
		Pos_S1 += DIST_S1;				
		Pos_S2 += DIST_S2;
		Pos_S3 += DIST_S3;
		Pos_S4 += DIST_S4;
		Pos_S5 += DIST_S5;
		Pos_S6 += DIST_S6;
		Pos_S7 += DIST_S7;				
		Pos_S8 += DIST_S8;
	}

	for(i = 0; i < MAX_NOZZLE/2; i++)
	{
		FAS_GetCommandPos(2,i+4,&s_pos);
		dCurPos[i] = s_pos/1000;  //dCurPos[i]를 mm단위로 계산하기 위해서 1000을 나눈다.
	}	

	//방향성 조사,현재 위치(dCurPos[i])보다 목표위치(Pos_S1)보다 크면 오른쪽으로 이동 bDirection[0]=1 dhkim
	if(dCurPos[0] < Pos_S1)		 bDirection[0] = 1;
	if(dCurPos[1] < Pos_S2)		 bDirection[1] = 1;
	if(dCurPos[2] < Pos_S3)		 bDirection[2] = 1;
	if(dCurPos[3] < Pos_S4)		 bDirection[3] = 1;
	if(dCurPos[4] < Pos_S5)		 bDirection[4] = 1;
	if(dCurPos[5] < Pos_S6)		 bDirection[5] = 1;
	if(dCurPos[6] < Pos_S7)		 bDirection[6] = 1;
	if(dCurPos[7] < Pos_S8)		 bDirection[7] = 1;
	//////////////////////////////////////////////////////////////////////////////////////////////////
	
	//오른쪽으로 움직일때는 Head번호가 큰것부터 왼쪽으로 움직일때는 Head번호가 작은것 부터 움직인다.
	//오른쪽 방향으로 우선 움직이고 왼쪽 방향으로 움직인다.
	nCount = 0;
	//오른쪽 방향으로 움직이는 경우 Check
	for(i = 0; i < MAX_NOZZLE/2; i++)
	{
		if(bDirection[(MAX_NOZZLE/2)-1-i])
		{
			nOrder[nCount] = MAX_NOZZLE/2-i;
			nCount++;
//			bSelectHead[i] = true;
		}
	}
	//왼쪽 방향으로 움직일 경우 Check
	for(i=0; i<MAX_NOZZLE/2; i++)
	{
		if(!bDirection[i])
		{
			nOrder[nCount++] = i+1;
//			bSelectHead[i] = false;
		}
	}

	double MultiPosition[MAX_NOZZLE/2] = { Pos_S1*S_PULSE, Pos_S2*S_PULSE, Pos_S3*S_PULSE, Pos_S4*S_PULSE, Pos_S5*S_PULSE, Pos_S6*S_PULSE,
										   Pos_S7*S_PULSE, Pos_S8*S_PULSE};
	
	double Feedrate = 100000.0;
//	double Feedrate = 200000.0;


	//방향 구분 할것...//	
	for(i=0; i<MAX_NOZZLE/2; i++)
	{
		FAS_MoveSingleAxisAbsPos(2, nOrder[i]+3, MultiPosition[nOrder[i]-1], Feedrate, 0);
		Sleep(30);//30
	}
}

void CDevicePart::HeadSPositionMove2(double Pos_S1, double Pos_S2, double Pos_S3, double Pos_S4, double Pos_S5, double Pos_S6, double Pos_S7, double Pos_S8, BOOL abs)
{
	double dSpeed = 100000.0, dPos=0.0; //dSpeed = 200000.0
	double dCurPos[MAX_NOZZLE/2], dSetPos[MAX_NOZZLE/2], dMovePos[MAX_NOZZLE/2];
	WORD wAxis[MAX_NOZZLE/2];
	BOOL bDirection[MAX_NOZZLE/2]; //dhkim 헤드 방향을 정하는 변수.
	int nOrder[MAX_NOZZLE/2];
	int nCount;
	double s_pos;
//	BOOL bSelectHead[MAX_NOZZLE];

	for(int i=0; i<MAX_NOZZLE/2;i++)
	{
		dCurPos[i]=0.0;//S축이 현재 위치.
		dSetPos[i]=0.0;
		dMovePos[i]=0.0;
		wAxis[i]=0;

		bDirection[i] = 0;//헤드 방향을 정하는 변수..0이면 왼쪽으로 이동, 1이면 오른쪽으로 이동.
		nOrder[i] = 0;//Head움지이는 순서를 정하는 변수..
//		bSelectHead[i] = false;
	}

	if( abs == FALSE )
	{
		// Data of Editor
		Pos_S1 += DIST_S9;				
		Pos_S2 += DIST_S10;
		Pos_S3 += DIST_S11;
		Pos_S4 += DIST_S12;
		Pos_S5 += DIST_S13;
		Pos_S6 += DIST_S14;
		Pos_S7 += DIST_S15;				
		Pos_S8 += DIST_S16;
	}

	for(i = 0; i < MAX_NOZZLE/2; i++)
	{
		if(i < 4)
		{
			FAS_GetCommandPos(2,i+12,&s_pos);
			dCurPos[i] = s_pos/1000;  //dCurPos[i]를 mm단위로 계산하기 위해서 1000을 나눈다.
		}
		else
		{
			FAS_GetCommandPos(3,i-4,&s_pos);
			dCurPos[i] = s_pos/1000;  //dCurPos[i]를 mm단위로 계산하기 위해서 1000을 나눈다.
		}
	}	

	//방향성 조사,현재 위치(dCurPos[i])보다 목표위치(Pos_S1)보다 크면 오른쪽으로 이동 bDirection[0]=1 dhkim
	if(dCurPos[0] < Pos_S1)		 bDirection[0] = 1;
	if(dCurPos[1] < Pos_S2)		 bDirection[1] = 1;
	if(dCurPos[2] < Pos_S3)		 bDirection[2] = 1;
	if(dCurPos[3] < Pos_S4)		 bDirection[3] = 1;
	if(dCurPos[4] < Pos_S5)		 bDirection[4] = 1;
	if(dCurPos[5] < Pos_S6)		 bDirection[5] = 1;
	if(dCurPos[6] < Pos_S7)		 bDirection[6] = 1;
	if(dCurPos[7] < Pos_S8)		 bDirection[7] = 1;
	//////////////////////////////////////////////////////////////////////////////////////////////////
	
	//오른쪽으로 움직일때는 Head번호가 큰것부터 왼쪽으로 움직일때는 Head번호가 작은것 부터 움직인다.
	//오른쪽 방향으로 우선 움직이고 왼쪽 방향으로 움직인다.
	nCount = 0;
	//오른쪽 방향으로 움직이는 경우 Check
	for(i = 0; i < MAX_NOZZLE/2; i++)
	{
		if(bDirection[(MAX_NOZZLE/2)-1-i])
		{
			nOrder[nCount] = MAX_NOZZLE/2-i;
			nCount++;
//			bSelectHead[i] = true;
		}
	}
	//왼쪽 방향으로 움직일 경우 Check
	for(i=0; i<MAX_NOZZLE/2; i++)
	{
		if(!bDirection[i])
		{
			nOrder[nCount++] = i+1;
//			bSelectHead[i] = false;
		}
	}

	double MultiPosition[MAX_NOZZLE/2] = { Pos_S1*S_PULSE, Pos_S2*S_PULSE, Pos_S3*S_PULSE, Pos_S4*S_PULSE, Pos_S5*S_PULSE, Pos_S6*S_PULSE,
										   Pos_S7*S_PULSE, Pos_S8*S_PULSE};
	
	double Feedrate = 100000.0;
//	double Feedrate = 200000.0;


	//방향 구분 할것...//	
	for(i=0; i<MAX_NOZZLE/2; i++)
	{
		if(nOrder[i]/5+2 == 3) //bd 3
		{
			FAS_MoveSingleAxisAbsPos(3, nOrder[i]-(MAX_NOZZLE/2)+3, MultiPosition[nOrder[i]-1], Feedrate, 0);
		}
		else
		{
			FAS_MoveSingleAxisAbsPos(2, nOrder[i]+11, MultiPosition[nOrder[i]-1], Feedrate, 0);
		}
		Sleep(30);
	}
}



BOOL CDevicePart::HeadSPosition_Check1(double Pos_S1, double Pos_S2, double Pos_S3, double Pos_S4, double Pos_S5, double Pos_S6, double Pos_S7, double Pos_S8, BOOL abs)
{
	int iaxis = 0 , ibd = 0;
	double s_pos = 0.0 ;
	double s_pos_com[MAX_NOZZLE/2] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
	// abs == FALSE 이면...가상 원점을 기준으로 한 위치로서 인자로 넘어온 위치를 가상 원점으로 환산해 준다.
	if( abs == FALSE )
	{
		s_pos_com[0] =  (Pos_S1 + DIST_S1)*S_PULSE;
		s_pos_com[1] =  (Pos_S2 + DIST_S2)*S_PULSE;
		s_pos_com[2] =  (Pos_S3 + DIST_S3)*S_PULSE;
		s_pos_com[3] =  (Pos_S4 + DIST_S4)*S_PULSE;
		s_pos_com[4] =  (Pos_S5 + DIST_S5)*S_PULSE;
		s_pos_com[5] =  (Pos_S6 + DIST_S6)*S_PULSE;
		s_pos_com[6] =  (Pos_S7 + DIST_S7)*S_PULSE;
		s_pos_com[7] =  (Pos_S8 + DIST_S8)*S_PULSE;
	}
	else	// abs == TRUE 이면...원점 센서를 기준으로 한 위치로서 인자로 넘어온 위치를 그대로 사용한다.
	{
		s_pos_com[0] =  (Pos_S1)*S_PULSE;
		s_pos_com[1] =  (Pos_S2)*S_PULSE;
		s_pos_com[2] =  (Pos_S3)*S_PULSE;
		s_pos_com[3] =  (Pos_S4)*S_PULSE;
		s_pos_com[4] =  (Pos_S5)*S_PULSE;
		s_pos_com[5] =  (Pos_S6)*S_PULSE;
		s_pos_com[6] =  (Pos_S7)*S_PULSE;
		s_pos_com[7] =  (Pos_S8)*S_PULSE;
	}

	for(iaxis = 0 ; iaxis <MAX_NOZZLE/2 ; iaxis++)
	{
		FAS_GetActualPos(2, iaxis+4, &s_pos);	

		if((s_pos < (s_pos_com[iaxis]-300.0)) || (s_pos > (s_pos_com[iaxis]+300.0))) return FALSE;
	}
	return TRUE;
}

BOOL CDevicePart::HeadSPosition_Check2(double Pos_S1, double Pos_S2, double Pos_S3, double Pos_S4, double Pos_S5, double Pos_S6, double Pos_S7, double Pos_S8, BOOL abs)
{
	int iaxis = 0 , ibd = 0;
	double s_pos = 0.0 ;
	double s_pos_com[MAX_NOZZLE/2] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
	// abs == FALSE 이면...가상 원점을 기준으로 한 위치로서 인자로 넘어온 위치를 가상 원점으로 환산해 준다.
	if( abs == FALSE )
	{
		s_pos_com[0] =  (Pos_S1 + DIST_S9)*S_PULSE;
		s_pos_com[1] =  (Pos_S2 + DIST_S10)*S_PULSE;
		s_pos_com[2] =  (Pos_S3 + DIST_S11)*S_PULSE;
		s_pos_com[3] =  (Pos_S4 + DIST_S12)*S_PULSE;
		s_pos_com[4] =  (Pos_S5 + DIST_S13)*S_PULSE;
		s_pos_com[5] =  (Pos_S6 + DIST_S14)*S_PULSE;
		s_pos_com[6] =  (Pos_S7 + DIST_S15)*S_PULSE;
		s_pos_com[7] =  (Pos_S8 + DIST_S16)*S_PULSE;
	}
	else	// abs == TRUE 이면...원점 센서를 기준으로 한 위치로서 인자로 넘어온 위치를 그대로 사용한다.
	{
		s_pos_com[0] =  (Pos_S1)*S_PULSE;
		s_pos_com[1] =  (Pos_S2)*S_PULSE;
		s_pos_com[2] =  (Pos_S3)*S_PULSE;
		s_pos_com[3] =  (Pos_S4)*S_PULSE;
		s_pos_com[4] =  (Pos_S5)*S_PULSE;
		s_pos_com[5] =  (Pos_S6)*S_PULSE;
		s_pos_com[6] =  (Pos_S7)*S_PULSE;
		s_pos_com[7] =  (Pos_S8)*S_PULSE;
	}

	for(iaxis = 0 ; iaxis <MAX_NOZZLE/2 ; iaxis++)
	{
		if(iaxis < 4)
		{
			FAS_GetActualPos(2, iaxis+12, &s_pos);	
		}
		else
		{
			FAS_GetActualPos(3, iaxis-4, &s_pos);	
		}
		if((s_pos < (s_pos_com[iaxis]-300.0)) || (s_pos > (s_pos_com[iaxis]+300.0))) return FALSE;
	}
	return TRUE;
}


void CDevicePart::StageLineFill(double DropSpeed, double SuctionSpeed, int *LineDropCount, int DropStartPos)
{
	WORD wNoOfAxisG1 = 0; //G1~G12
	WORD wNoOfAxisG2 = 0; //G13~G16
	WORD wNoOfAxisP1 = 0; //P1~P14
	WORD wNoOfAxisP2 = 0; //P15~P16

	int iAxisG1[MAX_NOZZLE] = {0,}; //G motor	     
	int iAxisG2[MAX_NOZZLE] = {0,}; //G motor	     
	int iAxisP1[MAX_NOZZLE] = {0,}; //P motor
	int iAxisP2[MAX_NOZZLE] = {0,}; //P motor

	double dPos1G1[MAX_NOZZLE] = {0,};
	double dPos1G2[MAX_NOZZLE] = {0,};
	double dPos2G1[MAX_NOZZLE] = {0,};
	double dPos2G2[MAX_NOZZLE] = {0,};
	double dPos1P1[MAX_NOZZLE] = {0,};
	double dPos1P2[MAX_NOZZLE] = {0,};
	double dPos2P1[MAX_NOZZLE] = {0,};
	double dPos2P2[MAX_NOZZLE] = {0,};

	double dVelocity1G1[MAX_NOZZLE] = {0,};
	double dVelocity1G2[MAX_NOZZLE] = {0,};
	double dVelocity1P1[MAX_NOZZLE] = {0,};
	double dVelocity1P2[MAX_NOZZLE] = {0,};
	double dVelocity2P1[MAX_NOZZLE] = {0,};
	double dVelocity2P2[MAX_NOZZLE] = {0,};

	int i = 0 , nNoAxisCount = 0 ,nNoAxisCount13 = 0;

	CString m_strLog; 
	BOOL bIoResult = FALSE;

	
	for(i = 0; i < MAX_NOZZLE; i++)
	{
		g_bGateCloseAlarm[i] = FALSE;
	}

	//임시...//
//	Drop_Info.m_nSuctionOffset[2] = -500.0;
//	Drop_Info.m_nSuctionOffset[3] = -500.0;

	//Suction 정보//
	for( i= 0; i < MAX_NOZZLE; i++)
	{					
		if((Drop_Info.head_onoff[i]) == TRUE)
		{
			if(LineDropCount[i] != 0)
			{
				if(i < NOZZLE_G1) 
				{
					iAxisG1[wNoOfAxisG1] = (AXIS_G1+i)%BOARD_AXES;
					dPos1G1[wNoOfAxisG1] = G_PULSE/2 + Drop_Info.m_nGateOffset[i];
					dPos2G1[wNoOfAxisG1] = -(G_PULSE/2 + Drop_Info.m_nGateOffset[i]);
					dVelocity1G1[wNoOfAxisG1] = DropSpeed;
					wNoOfAxisG1++;	
				}
				else
				{
					iAxisG2[wNoOfAxisG2] = (AXIS_G13+(i%NOZZLE_G1))%BOARD_AXES;
					dPos1G2[wNoOfAxisG2] = G_PULSE/2 + Drop_Info.m_nGateOffset[i];
					dPos2G2[wNoOfAxisG2] = -(G_PULSE/2 + Drop_Info.m_nGateOffset[i]);
					dVelocity1G2[wNoOfAxisG2] = DropSpeed;
					wNoOfAxisG2++;	
				}
				if(i < NOZZLE_P1) 
				{
					iAxisP1[wNoOfAxisP1] = (AXIS_P1+i)%BOARD_AXES;
					dPos1P1[wNoOfAxisP1] = Drop_Info.m_dFullBasePos[i];
					dPos2P1[wNoOfAxisP1] = -(Drop_Info.m_nSuctionOffset[i]);
					dVelocity1P1[wNoOfAxisP1] = SuctionSpeed*1000;
					dVelocity2P1[wNoOfAxisP1] = Drop_Info.m_dSucOffSpeed[i];
					wNoOfAxisP1++;	
				}
				else
				{
					iAxisP2[wNoOfAxisP2] = (AXIS_P15+(i%NOZZLE_P1))%BOARD_AXES;
					dPos1P2[wNoOfAxisP2] = Drop_Info.m_dFullBasePos[i];
					dPos2P2[wNoOfAxisP2] = -(Drop_Info.m_nSuctionOffset[i]);
					dVelocity1P2[wNoOfAxisP2] = SuctionSpeed*1000;
					dVelocity2P2[wNoOfAxisP2] = Drop_Info.m_dSucOffSpeed[i];
					wNoOfAxisP2++;	
				}
			}
		}
	}

	//suction offset//
	FAS_MoveMultiAxisIncPos(AXIS_P1/BOARD_AXES+1, wNoOfAxisP1, iAxisP1, dPos2P1, dVelocity2P1, TRUE);	
	FAS_MoveMultiAxisIncPos(AXIS_P15/BOARD_AXES+1, wNoOfAxisP2, iAxisP2, dPos2P2, dVelocity2P2, TRUE);	
//2010.07.07 by tskim 	
//	FAS_MoveMultiAxisIncPos(AXIS_P1/BOARD_AXES+1, wNoOfAxisP1, iAxisP1, dPos2P1, dVelocity2P1, FALSE);//2010.07.07 by tskim 	
//	FAS_MoveMultiAxisIncPos(AXIS_P15/BOARD_AXES+1, wNoOfAxisP2, iAxisP2, dPos2P2, dVelocity2P2, TRUE);//2010.07.07 by tskim 	
	

//#if PC_TYPE    //2009.11.23 by tskim
//	Sleep(INPOSITION_DELAY);//2010.07.07 by tskim 
//2010.07.07 by tskim 
// 	for(i = 0; i < MAX_NOZZLE; i++)
// 	{
// 		if(Drop_Info.head_onoff[i] == TRUE)
// 		{
// 			if(LineDropCount[i] != 0)
// 			{
// 				if(i < NOZZLE_P1)							
// 					while(!MotionEndCheck(AXIS_P1+i));
// 				else
// 					while(!MotionEndCheck(AXIS_P1+i+2));
// 			}
// 		}
// 	}

	//close//
 //	FAS_MoveMultiAxisIncPos(AXIS_G1/BOARD_AXES+1, wNoOfAxisG1, iAxisG1, dPos2G1, dVelocity1G1, TRUE);												
 //	FAS_MoveMultiAxisIncPos(AXIS_G13/BOARD_AXES+1, wNoOfAxisG2, iAxisG2, dPos2G2, dVelocity1G2, TRUE);	
 	FAS_MoveMultiAxisIncPos(AXIS_G1/BOARD_AXES+1, wNoOfAxisG1, iAxisG1, dPos2G1, dVelocity1G1, FALSE);												
 	FAS_MoveMultiAxisIncPos(AXIS_G13/BOARD_AXES+1, wNoOfAxisG2, iAxisG2, dPos2G2, dVelocity1G2, TRUE);	
	Sleep(10); //100

#if EQ
	//Gate Close Check//
	for( i = 0; i < MAX_NOZZLE; i++ )
	{
		if( Drop_Info.head_onoff[i] == TRUE )
		{
			if( LineDropCount[i] != 0 )
			{
				if( GateCloseAlarmCheck(i) != i )
				{
					FAS_GetIoBit(3, false, GATE1_CLOSE_POSITION+i, &bIoResult); 


					m_strLog.Empty(); 
 					m_strLog.Format("Suction GATE CLOSE CHECK ERROR ( BEFORE RETRY ) : HEAD %d → OPEN  STATUS : %d"
						, i+1, bIoResult);

 					SaveDeviceLog(m_strLog); 


					if ( i < 12 ) // 3 BOARD G AXIS 개수 
						FAS_MoveOriginSingleAxis( AXIS_G1  / BOARD_AXES  + 1, i + 4 , 1 );
					else 
						FAS_MoveOriginSingleAxis( AXIS_G13 / BOARD_AXES  + 1, i - 12, 1 );
					
					Sleep(100);

					if ( i < 12 ) // 3 BOARD G AXIS 개수 
					{
						FAS_MoveSingleAxisIncPos( AXIS_G1  / BOARD_AXES  + 1, i + 4 , dPos2G1[nNoAxisCount], dVelocity1G1[nNoAxisCount] , 1 ); 
						nNoAxisCount++; 
					}
					else 
					{
					    FAS_MoveSingleAxisIncPos( AXIS_G13 / BOARD_AXES  + 1, i - 12, dPos2G2[nNoAxisCount13], dVelocity1G2[nNoAxisCount13] , 1 );
						nNoAxisCount13++;
					}
					FAS_GetIoBit(3, false, GATE1_CLOSE_POSITION+i, &bIoResult); 

					m_strLog.Empty(); 
 					m_strLog.Format("Suction GATE CLOSE CHECK ERROR ( AFTER RETRY ) : HEAD %d → OPEN  STATUS : %d"
						, i+1, bIoResult );

 					SaveDeviceLog(m_strLog); 

					
					
					if( GateCloseAlarmCheck(i) != i )
					{
						g_bGateCloseAlarm[i] = TRUE;
						return;
					}
				}
			}
		}
	}
#endif

	//Full Base Position Move//
//	FAS_MoveMultiAxisAbsPos(AXIS_P1/BOARD_AXES+1, wNoOfAxisP1, iAxisP1, dPos1P1, dVelocity1P1, TRUE);	
//	FAS_MoveMultiAxisAbsPos(AXIS_P15/BOARD_AXES+1, wNoOfAxisP2, iAxisP2, dPos1P2, dVelocity1P2, TRUE);			
	FAS_MoveMultiAxisAbsPos(AXIS_P1/BOARD_AXES+1, wNoOfAxisP1, iAxisP1, dPos1P1, dVelocity1P1, FALSE);	
	FAS_MoveMultiAxisAbsPos(AXIS_P15/BOARD_AXES+1, wNoOfAxisP2, iAxisP2, dPos1P2, dVelocity1P2, TRUE);			

	Sleep(INPOSITION_DELAY);

	for(i = 0; i < MAX_NOZZLE; i++)
	{
		if(Drop_Info.head_onoff[i] == TRUE)
		{
			if(LineDropCount[i] != 0)
			{
				if(i < NOZZLE_P1)							
					while(!MotionEndCheck(AXIS_P1+i));
				else
					while(!MotionEndCheck(AXIS_P1+i+2));
			}
		}
	}
//#endif      //2009.11.23 by tskim

	//Open//
 //	FAS_MoveMultiAxisIncPos(AXIS_G1/BOARD_AXES+1, wNoOfAxisG1, iAxisG1, dPos1G1, dVelocity1G1, TRUE);												
 //	FAS_MoveMultiAxisIncPos(AXIS_G13/BOARD_AXES+1, wNoOfAxisG2, iAxisG2, dPos1G2, dVelocity1G2, TRUE);	
 	FAS_MoveMultiAxisIncPos( AXIS_G1 /BOARD_AXES+1, wNoOfAxisG1, iAxisG1, dPos1G1, dVelocity1G1, FALSE);												
 	FAS_MoveMultiAxisIncPos( AXIS_G13/BOARD_AXES+1, wNoOfAxisG2, iAxisG2, dPos1G2, dVelocity1G2, TRUE);	
	Sleep(10); //100

	nNoAxisCount = 0; 
	nNoAxisCount13 = 0;
#if EQ
	for( i = 0; i < MAX_NOZZLE; i++ )
	{
		if( Drop_Info.head_onoff[i] == TRUE )
		{
			if( LineDropCount[i] != 0 )
			{
				FAS_GetIoBit(3, true, GATE1_OPEN_POSITION+i, &bIoResult);

				if( !bIoResult )
				{
					FAS_GetIoBit(3, true, GATE1_OPEN_POSITION+i, &bIoResult); 

					m_strLog.Empty(); 
 					m_strLog.Format("Suction GATE OPEN CHECK ERROR ( BEFORE RETRY ) : HEAD %d → OPEN  STATUS : %d"
						, i+1, bIoResult );

 					SaveDeviceLog(m_strLog); 

					//OPEN == 원점 동작  
					if ( i < 12 ) // 3 BOARD G AXIS 개수 
						FAS_MoveOriginSingleAxis( AXIS_G1  / BOARD_AXES  + 1, i + 4 , 1 );
					else 
						FAS_MoveOriginSingleAxis( AXIS_G13 / BOARD_AXES  + 1, i - 12, 1 );
					
					Sleep(100);

					nNoAxisCount++; 

					FAS_GetIoBit(3, true, GATE1_OPEN_POSITION+i, &bIoResult);

					m_strLog.Empty(); 
 					m_strLog.Format("Suction GATE OPEN CHECK ERROR ( AFTER RETRY ) : HEAD %d → OPEN  STATUS : %d"
						, i+1, bIoResult );

 					SaveDeviceLog(m_strLog); 

//진행 시켜도 알람 발생 함. 					
//					if( !FAS_GetIoBit(3, false, GATE1_OPEN_POSITION+i, &bIoResult) )
//					{
//						g_bGateCloseAlarm[i] = TRUE;
//						return;
//					}	
				}
			}
		}
	}
#endif

}

void CDevicePart::HeadStepDrop(double DischargeSpeed, int DropStartPos, BOOL EndCheck)
{
	int i = 0;

	WORD wNoOfAxisP1 = 0; //P1~P14
	WORD wNoOfAxisP2 = 0; //P15~P16

	int iAxisP1[MAX_NOZZLE] = {0,}; //P motor
	int iAxisP2[MAX_NOZZLE] = {0,}; //P motor

	double dPos1P1[MAX_NOZZLE] = {0,}; // P_Axis: 1drop distance
	double dPos1P2[MAX_NOZZLE] = {0,};

	double dVelocity1P1[MAX_NOZZLE] = {0,}; // P_Axis: Discharge speed
	double dVelocity1P2[MAX_NOZZLE] = {0,};

	for(i = 0; i < MAX_NOZZLE; i++)
	{
		if(TRUE==(Drop_Info.head_onoff[i] * Drop_Info.drop_onoff[i]))
		{
			if(i < NOZZLE_P1) 
			{
				iAxisP1[wNoOfAxisP1] = (AXIS_P1+i)%BOARD_AXES;
				dPos1P1[wNoOfAxisP1] = Drop_Info.m_dDistPerDrop[i];
				dVelocity1P1[wNoOfAxisP1] = DischargeSpeed*1000;
				wNoOfAxisP1++;	
			}
			else
			{
				iAxisP2[wNoOfAxisP2] = (AXIS_P15+(i%NOZZLE_P1))%BOARD_AXES;
				dPos1P2[wNoOfAxisP2] = Drop_Info.m_dDistPerDrop[i];
				dVelocity1P2[wNoOfAxisP2] = DischargeSpeed*1000;
				wNoOfAxisP2++;	
			}
		}
	}
//	FAS_MoveMultiAxisIncPos(AXIS_P1/BOARD_AXES+1, wNoOfAxisP1, iAxisP1, dPos1P1, dVelocity1P1, TRUE);	
//	FAS_MoveMultiAxisIncPos(AXIS_P15/BOARD_AXES+1, wNoOfAxisP2, iAxisP2, dPos1P2, dVelocity1P2, TRUE);	
	FAS_MoveMultiAxisIncPos(AXIS_P1/BOARD_AXES+1, wNoOfAxisP1, iAxisP1, dPos1P1, dVelocity1P1, FALSE);	
	FAS_MoveMultiAxisIncPos(AXIS_P15/BOARD_AXES+1, wNoOfAxisP2, iAxisP2, dPos1P2, dVelocity1P2, TRUE);	
	
	Sleep(INPOSITION_DELAY);

	for(i = 0; i < MAX_NOZZLE; i++)
	{
		if(Drop_Info.head_onoff[i] == TRUE)
		{
			if(i < NOZZLE_P1)							
				while(!MotionEndCheck(AXIS_P1+i));
			else
				while(!MotionEndCheck(AXIS_P1+i+2));
		}
	}
}



void CDevicePart::CameraSelect(int CameraNum)
{
//2009.12.30 by tskim
// 	int iBdID = 3;
// 	switch(CameraNum)
// 	{
// 	 case HEAD1_CAM :// head #1
// 		FAS_SetIoBit(iBdID, FALSE, MUX1_CH_SEL1, FALSE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX1_CH_SEL2, FALSE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX1_CH_SEL3, FALSE);
// 		m_nMux1 = HEAD1_CAM;
// 		break;	
// 	case HEAD2_CAM : // head 2 
// 		FAS_SetIoBit(iBdID, FALSE, MUX1_CH_SEL1, TRUE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX1_CH_SEL2, FALSE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX1_CH_SEL3, FALSE);
// 		m_nMux1 = HEAD2_CAM;
// 		break;
// 	case HEAD3_CAM : // head 3 
// 		FAS_SetIoBit(iBdID, FALSE, MUX1_CH_SEL1, FALSE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX1_CH_SEL2, TRUE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX1_CH_SEL3, FALSE);
// 		m_nMux1 = HEAD3_CAM;
// 		break;
// 	case HEAD4_CAM : // head 4 
// 		FAS_SetIoBit(iBdID, FALSE, MUX1_CH_SEL1, TRUE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX1_CH_SEL2, TRUE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX1_CH_SEL3, FALSE);
// 		m_nMux1 = HEAD4_CAM;
// 		break;
// 	case HEAD5_CAM : // head 5 
// 		FAS_SetIoBit(iBdID, FALSE, MUX1_CH_SEL1, FALSE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX1_CH_SEL2, FALSE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX1_CH_SEL3, TRUE);
// 		m_nMux1 = HEAD5_CAM;
// 		break;
// 	case HEAD6_CAM : // head 6 
// 		FAS_SetIoBit(iBdID, FALSE, MUX1_CH_SEL1, TRUE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX1_CH_SEL2, FALSE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX1_CH_SEL3, TRUE);
// 		m_nMux1 = HEAD6_CAM;
// 		break;
// 	case HEAD7_CAM : // head 7
// 		FAS_SetIoBit(iBdID, FALSE, MUX1_CH_SEL1, FALSE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX1_CH_SEL2, TRUE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX1_CH_SEL3, TRUE);
// 		m_nMux1 = HEAD7_CAM;
// 		break;
// 	case HEAD8_CAM : // head 8 선폭 
// 		FAS_SetIoBit(iBdID, FALSE, MUX1_CH_SEL1, TRUE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX1_CH_SEL2, TRUE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX1_CH_SEL3, TRUE);
// 		m_nMux1 = HEAD8_CAM;
// 		break;
// ///////////////////////////////////////////Mux#2//////////////////////////////////////
// 	case HEAD9_CAM : // head 9 
// 		FAS_SetIoBit(iBdID, FALSE, MUX2_CH_SEL1, FALSE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX2_CH_SEL2, FALSE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX2_CH_SEL3, FALSE);
// 		m_nMux2 = HEAD9_CAM;
// 		break;	
// 	case HEAD10_CAM :   // head 10 
// 		FAS_SetIoBit(iBdID, FALSE, MUX2_CH_SEL1, TRUE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX2_CH_SEL2, FALSE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX2_CH_SEL3, FALSE);		
// 		m_nMux2 = HEAD10_CAM;
// 		break;
// 	case HEAD11_CAM : // head 11
// 		FAS_SetIoBit(iBdID, FALSE, MUX2_CH_SEL1, FALSE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX2_CH_SEL2, TRUE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX2_CH_SEL3, FALSE);		
// 		m_nMux2 = HEAD11_CAM;
// 		break;
// 	 case HEAD12_CAM : // head 12
// 		FAS_SetIoBit(iBdID, FALSE, MUX2_CH_SEL1, TRUE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX2_CH_SEL2, TRUE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX2_CH_SEL3, FALSE);		
// 		m_nMux2 = HEAD12_CAM;
// 		break;
// 	case HEAD13_CAM : // head 13
// 		FAS_SetIoBit(iBdID, FALSE, MUX2_CH_SEL1, FALSE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX2_CH_SEL2, FALSE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX2_CH_SEL3, TRUE);		
// 		m_nMux2 = HEAD13_CAM;
// 		break;
// 	case HEAD14_CAM : // head 14
// 		FAS_SetIoBit(iBdID, FALSE, MUX2_CH_SEL1, TRUE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX2_CH_SEL2, FALSE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX2_CH_SEL3, TRUE);		
// 		m_nMux2 = HEAD14_CAM;
// 		break;
// 	case HEAD15_CAM : // head 15
// 		FAS_SetIoBit(iBdID, FALSE, MUX2_CH_SEL1, FALSE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX2_CH_SEL2, TRUE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX2_CH_SEL3, TRUE);		
// 		m_nMux2 = HEAD15_CAM;
// 		break;
// 	case HEAD16_CAM : // head 16
// 		FAS_SetIoBit(iBdID, FALSE, MUX2_CH_SEL1, TRUE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX2_CH_SEL2, TRUE);
// 		FAS_SetIoBit(iBdID, FALSE, MUX2_CH_SEL3, TRUE);		
// 		m_nMux2 = HEAD16_CAM;
// 		break;
// 	}
// 	Sleep(100);
// 	m_nSelectCamera = CameraNum;
	return;
}

// by ckh
void CDevicePart::HeadSPositionOffsetRead(double *dData)
{
	CString strPathName="";
	ifstream fi;

	strPathName.Format("%s","D:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\HeadSPositionOffset.dat");	
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);

	if(fi.is_open())
	{
		for(int i =0;i< MAX_NOZZLE;i++)
		{
			fi >> *dData++;
		}
		fi.close();
	}
}
// by ckh 2008.10.01 
// 정위치 획득 관련 정보 가져오기.
// by ckh 2008.12.26 
// 모드별 데이터 변경이 필요, 
BOOL CDevicePart::GetPosChkTeachingData(PosChk_Data *PosChkData, int nMotionMode)
{
	CString strPathName="";
	ifstream fi;
	char ch[256];
	int nHeadNum, nResult;
	switch(nMotionMode)
	{
	case MODE_POSCHK:
		strPathName.Format("%s","D:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\Teaching_PosChk.dat");	
		break;
	case MODE_ALIGN:
		strPathName.Format("%s","D:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\Teaching_AlignMark.dat");	
		break;
	} 
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);

	if(fi.is_open())
	{
		fi >> ch;//fprintf(fp,"Head_Num\t");
		
		fi >> ch;//fprintf(fp,"Mark_PosX\t");
		fi >> ch;//fprintf(fp,"SpecMin_X\t");
		fi >> ch;//fprintf(fp,"SpecMax_X\t");

		fi >> ch;//fprintf(fp,"Mark_PosY\t");
		fi >> ch;//fprintf(fp,"SpecMin_Y\t");
		fi >> ch;//fprintf(fp,"SpecMax_Y\t");
		// 		
		fi >> ch;//fprintf(fp,"Mark_Size\t");
		fi >> ch;//fprintf(fp,"Mark_Size_Min\t");
		fi >> ch;//fprintf(fp,"Mark_Size_Max\t");

		fi >> ch;//fprintf(fp,"AxisPosX\t");
		fi >> ch;//fprintf(fp,"AxisPosY\t");
		fi >> ch;//fprintf(fp,"AxisPosX\t");
		fi >> ch;//fprintf(fp,"AxisPosY\t");
		
		// 20008.12.20 by ckh 
		// Lighte 관련
		fi >> ch;// s/w Light 사용 유무 ( 미사용시 하드웨어 셋팅값으로 사용함.)	
		fi >> ch;// s/w Light 사용시의 Light의 값
		///////////////////////////////////////////////////
		for(int i =0; i< MAX_NOZZLE;i++)
		{
			fi >> nHeadNum; // 헤더 번호

			// 헤더 번호에 데이터를 저장함. 
			// 저장된 순서가 다를 경우가 있으므로
			fi >> PosChkData[nHeadNum].dMarkX;
			fi >> PosChkData[nHeadNum].dSpecMin_X;
			fi >> PosChkData[nHeadNum].dSpecMax_X;

			fi >> PosChkData[nHeadNum].dMarkY;
			fi >> PosChkData[nHeadNum].dSpecMin_Y;
			fi >> PosChkData[nHeadNum].dSpecMax_Y;

			fi >> PosChkData[nHeadNum].dMarkSize;
			fi >> PosChkData[nHeadNum].dMarkSizeMin;
			fi >> PosChkData[nHeadNum].dMarkSizeMax;

			fi >> PosChkData[nHeadNum].dPosX;
			fi >> PosChkData[nHeadNum].dPosY;

			fi >> PosChkData[nHeadNum].dSpeed_S;
			fi >> PosChkData[nHeadNum].dSpeed_K;

			fi >> PosChkData[nHeadNum].bLightUse;
			fi >> PosChkData[nHeadNum].nLightVal;

			fi >> PosChkData[nHeadNum].dTagetPosX;
			fi >> PosChkData[nHeadNum].dTagetPosY;
		}
		fi.close();
		nResult =  TRUE;
	}
	else
		nResult = FALSE;

	return nResult;
}
// by ckh 2008.10.01
// 정위치 티칭 데이터 저장하기.
BOOL CDevicePart::SaveTeachingPosChk(PosChk_Data PosChkData,int nHeadNum)
{
	FILE *fp;
	CString strPath,strFile,strFileTemp;

	CString strTime,strTemp;
	strPath.Format("D:\\TOP\\P8CA_LC\\P8CA_LC_DATA");

	strFile = strPath + "\\Teaching_PosChkTemp.dat";

	fp = fopen((char *)(LPCSTR)strFile, "at"); 
	if(!fp)
	{
		return FALSE;
	}
	// 제목 열 추가 첫번째만.
	if(nHeadNum == 0)
	{
		fprintf(fp,"Head_Num\t");

		// 화면상에서의 위치 값임.
		fprintf(fp,"MarkX\t"); 
		fprintf(fp,"SpecMin_X\t");
		fprintf(fp,"SpecMax_X\t");

		fprintf(fp,"MarkY\t");
		fprintf(fp,"SpecMin_Y\t");
		fprintf(fp,"SpecMax_Y\t");
		
		fprintf(fp,"Mark_Size\t");
		fprintf(fp,"Mark_Size_Min\t");
		fprintf(fp,"Mark_Size_Max\t");

		fprintf(fp,"AxisPosX\t");
		fprintf(fp,"AxisPosY\t");

		/////////////////////////////////////
		// 2008.12.20 by ckh 
		fprintf(fp,"Speed_S\t");
		fprintf(fp,"Speed_K\t");
		fprintf(fp,"Light_Use\t");
		fprintf(fp,"Light_Val\t");
		/////////////////////////////////////
		fprintf(fp,"\n");
	}	
	
	// 헤더 번호 0번부터 시작
	fprintf(fp, "%d\t", nHeadNum);
	// 마크 위치
	fprintf(fp, "%.3f\t", PosChkData.dMarkX);
	fprintf(fp, "%.3f\t", PosChkData.dSpecMin_X);
	fprintf(fp, "%.3f\t", PosChkData.dSpecMax_X);

	fprintf(fp, "%.3f\t", PosChkData.dMarkY);
	PosChkData.dSpecMin_Y = PosChkData.dSpecMin_X;
	PosChkData.dSpecMax_Y = PosChkData.dSpecMax_X;
	fprintf(fp, "%.3f\t", PosChkData.dSpecMin_Y);
	fprintf(fp, "%.3f\t", PosChkData.dSpecMax_Y);

	// 마크 사이즈
	fprintf(fp, "%f\t", PosChkData.dMarkSize);
	fprintf(fp, "%f\t", PosChkData.dMarkSizeMin);// 마크 최소
	fprintf(fp, "%f\t", PosChkData.dMarkSizeMax);// 마크 최대

	fprintf(fp, "%.3f\t", PosChkData.dPosX);// 
	fprintf(fp, "%.3f\t", PosChkData.dPosY);// 
	
	// 20008.12.20 by ckh 
	fprintf(fp, "%.3f\t", PosChkData.dSpeed_S);// 
	fprintf(fp, "%.3f\t", PosChkData.dSpeed_K);// 
	fprintf(fp, "%d\t", PosChkData.bLightUse);// 
	fprintf(fp, "%d\t", PosChkData.nLightVal);// 

	fprintf(fp, "%.3f\t", PosChkData.dTagetPosX);// 
	fprintf(fp, "%.3f\t", PosChkData.dTagetPosY);//

	fprintf(fp, "\n");
	fclose(fp);

	return TRUE;
}

int CDevicePart::HexString2Dec(CString lHexData)
{
	int dConvert=0, length;
	
	length = lHexData.GetLength();
	for(int i=0;i<length;i++)
	{
		dConvert *= 16;
		dConvert += HexConvert(lHexData.Mid(i,1));
	}

	return dConvert;
}

int CDevicePart::GateCloseAlarmCheck(int headno)
{
	BOOL bIoResult = FALSE;
//#if EQ
	for(int i=0;i<200;i++)
	{
		FAS_GetIoBit(3, false, GATE1_CLOSE_POSITION+headno, &bIoResult);
		
		if(bIoResult == TRUE)
		{
			break;
		}
		Sleep(10);
//		i++;
	}
	if(i>=199) return 100+headno;
	else return headno;
//#else
//	return headno;
//#endif
	return 100+headno;
}

// by ckh 
BOOL CDevicePart::SetLightBright(int nCam, int nVal)
{
#if _VISION
	int  nLight_int_ext,nLight_Write,nLight_odd_even,nLight_H;//nCh,
	BOOL bLowIO;//bEven,
	CString strnVal;
	int nDelay = 10; // by ckh 2009.01.09 
//	if(nCam < MAX_NOZZLE/2) // Light 1
//	{
		nLight_int_ext	= LIGHT1_INT_EXT;
		nLight_Write	= LIGHT1_WRITE1 + nCam/2;
		nLight_odd_even	= LIGHT1_ODD_EVEN;
		nLight_H		= LIGHT1_1H;
		bLowIO			= false;
//	}
//	else
//	{
// 		nLight_int_ext	= LIGHT2_INT_EXT;
// 		nLight_Write	= LIGHT2_WRITE1+(nCam-MAX_NOZZLE/2)/2;
// 		nLight_odd_even	= LIGHT2_ODD_EVEN;
// 		nLight_H		= LIGHT2_1H;
// 		bLowIO			= FALSE;
//	}
	// 외부 셋팅 입력 선택
	FAS_SetIoBit(2, bLowIO, nLight_int_ext, TRUE);
	Sleep(nDelay);

	// 조명 값 변경하기.
	m_strResult = ""; // 조명 변환 값
	strnVal = GetDec2Bin(nVal);
	strnVal = strnVal+"00000000" ;// 문자 8개 이상을 유지 하기 위해.
	strnVal = strnVal.Left(8);
	BOOL bSetIO;
	for(int i =0; i< 8; i++)
	{
		bSetIO = atoi(strnVal.Mid(i,1));
		FAS_SetIoBit(2, false, nLight_H + i, bSetIO);
	}
	Sleep(nDelay);// by ckh 2009.02.10 , 값 변경시에는 Sleep이 들어가야함. 

	// Light Chanel Odd/Even 선택	
	if(nCam%2 == 1)
	{
		FAS_SetIoBit(2,bLowIO, nLight_odd_even, TRUE);
	}
	else
	{
		FAS_SetIoBit(2, bLowIO, nLight_odd_even, FALSE);
	}
	Sleep(nDelay);

	// Light Chanel 선택
	FAS_SetIoBit(2, bLowIO, nLight_Write, true);
	Sleep(nDelay);
	// Light Chanel 선택
	FAS_SetIoBit(2, bLowIO, nLight_Write, false);
	Sleep(nDelay);
#endif
	return TRUE;
}

CString CDevicePart::GetDec2Bin(int nNumber)
{
	 int bin;
	 CString str;

	 if (nNumber>0)
	 {
		  bin = nNumber % 2;
		  nNumber = nNumber/2;
		  str.Format("%d", bin);
		  m_strResult += str; // by ckh 2009.02.10 비트를 역으로 하기 위함. 

		  GetDec2Bin(nNumber);
	 }

 return m_strResult;
}

BOOL CDevicePart::SupplySolOff1()
{
	//CDA 이상 발생시 CDA SOL을 끊다...//
	FAS_SetIoBit(3, FALSE, SUPPLY_CDA_SOL1, FALSE);

	return TRUE;

}

BOOL CDevicePart::SupplySolOff2()
{
	//CDA 이상 발생시 CDA SOL을 끊다...//
	FAS_SetIoBit(3, FALSE, SUPPLY_CDA_SOL2, FALSE);

	return TRUE;
}

// by ckh 2009.02.04
// 2진수를 10진수로 
int CDevicePart:: GetBin2Dec( char* bin_str )
{  

   // bit_value: 문자열에서 어떤 자릿 수의 값

  // pos_value:자릿수 크기, 맨 아래는  1(2의 0승), 그 다음은 2(2의1승), 4(2의 2승), 8.... 
  int i, len = strlen( bin_str ), bit_value, pos_value = 1, sum = 0; 

  for( i = len-1; i >= 0;  i-- ){ 
    bit_value = (int)(bin_str[i] - '0');  // 문자열의 문자를 숫자로 바꾼다.

    // bit_value가 0 이나 1이 아니면 문자입력이 잘못된 것으로 간주하여 걸러낸다.. 
    if( !( bit_value == 0 || bit_value == 1 )) 
     return -1; 


    bit_value *= pos_value; 
    sum += bit_value; 

     

    // 자릿수의 크기는 루프를 한 번 돌 때마다 2를 곱하여 얻는다. 
    pos_value *= 2; 
  } 
//  itoa( sum, dec_str, 10 ); 
  return sum; 
} 

int CDevicePart::GetStageVac_Purge()
{
	return FALSE;
	int nResult = 0;
	BOOL bIOResult1 = FALSE,bIOResult2 = FALSE,bIOResult3 = FALSE;
//	FAS_GetIoBit(2,true,STAGE_VACUUM_PRESS,&bIOResult1);
//	FAS_GetIoBit(2,true,STAGE_PURGE_PRESS1,&bIOResult2);
//	FAS_GetIoBit(2,true,STAGE_PURGE_PRESS2,&bIOResult3);

	Sleep(10);
	if(bIOResult1 == TRUE) // Vacuum On 일 경우 
	{
		nResult = 1;
	}
	if(bIOResult2 == TRUE || bIOResult3 == TRUE)
	{
		nResult +=2;
	}

	return nResult;//2010.02.07 by tskim
}

void CDevicePart::StagePositionMove2(double Pos_X, double Pos_Y, double dSSpeed, double dYSpeed, WORD EndCheck)
{
	//by shin//2009.07.08//
	int Axis1 = 0, Axis2 = 1;
	double Position1 = Pos_X*S_PULSE;
	double Position2 = Pos_Y*K_PULSE;
	double Speed1 = dSSpeed;
	double Speed2 = dYSpeed;
	
	//by shin//2009.07.08//
	//bottle내 액정의 흔들림을 최소화하기 위해 column 이동 완료 후 s축 이동으로 변경...//

//	FAS_MoveSingleAxisAbsPos(1, Axis1, Position1, Speed1, 1); //S축
	//P축 SUCTION & S축 이동 OVERLAP 위한 EndCheck FALSE
	FAS_MoveSingleAxisAbsPos(1, Axis1, Position1, Speed1, 0); //S축

	Sleep(20);//Sleep(30);
//	FAS_MoveSingleAxisAbsPos(1, Axis2, Position2, Speed2, 1); //K축
}

CString CDevicePart::Make2sComplement(CString Data)
{
	CString strReturn;
	CString strTemp;
	int length;
	DWORD Temp;
	length = Data.GetLength();
	strTemp = Data.Mid(0,1);
// 	if(strTemp == "-")
// 	{
// 		Temp = atoi(Data.Mid(1,length))*1000;
// 		Temp = ~Temp;
// 		Temp +=0x1;
// 		strReturn.Format("%08x",Temp);
// 	}
// 	else
// 	{
// 		Temp = atoi(Data)*1000;
// 		strReturn.Format("%08x",Temp);
// 	}
	if(strTemp == "-")
	{
		Temp = atoi(Data.Mid(1,length));
		Temp = ~Temp;
		Temp +=0x1;
		strReturn.Format("%08x",Temp);
	}
	else
	{
		Temp = atoi(Data);
		strReturn.Format("%08x",Temp);
	}
 	return strReturn;
}

long CDevicePart::LongHexString2Dec(CString lHexData)
{
	long dConvert=0, length;
	
	length = lHexData.GetLength();
	for(int i=0;i<length;i++)
	{
		dConvert *= 16;
		dConvert += HexConvert(lHexData.Mid(i,1));
	}

	return dConvert;
}

double CDevicePart::GetPressValue(int Gauge)
{
	return 0.0;//2016.05.26
/*
//	long Resolution[6];
	long Resolution;
	int Press;
	int i;
	short ret1, ret2;
	ret1 = ret2 = 0;
// 	for(i=0;i<6;i++)
// 		Resolution[i] = 0;
	if(Gauge<3)//P3_P5 0,1,2
	{
		for(i=0;i<3;i++)
		{
			FAS_GetAdResult(1,Gauge+AXIS_P3, &Resolution);
			ret1 = Resolution;
			if(ret1 & 0x0800) ret1 |= 0xf000;	// 음수 계산
			ret2 = (short)(ret2+ret1);
		}
		ret2 /= 3;		
		(ret2 & 0x0800) ? (ret2|=0xf000) : 1;
	}
	else //P7_P8 3,4
	{
		for(i=0;i<3;i++)
		{
			FAS_GetAdResult(1,Gauge+AXIS_P3+1, &Resolution);
			ret1 = Resolution;
			if(ret1 & 0x0800) ret1 |= 0xf000;	// 음수 계산
			ret2 = (short)(ret2+ret1);
		}
		ret2 /= 3;		
		(ret2 & 0x0800) ? (ret2|=0xf000) : 1;
	}
//1. HEAD CDA P3 4
//2. ION CDA P4  5
//3. STAGE VAC P5  6
//4. L SUP P7  8 
//5. R SUP P8  9
//진공 게이지 0~-101.3kpa
//압력 게이지 0~1000kpa
//공통 출력 전압 1~5V
//fastech 12bit resolution 범위 -10~+10(1~4096)
//204.8/Volt
//CDA 250Kpa/Volt
//VAC 25.325kpa/Volt
//CDA
		// AD는 12bit AD Converter임. 2^12 = 4096 digit, ±10V(range 20V) -> 4096 digit
		// 10V는 2048 digit임.
		//5(V) = 1000(Kpa), 0.6(V) = -100(Kpa)
		//5(V) : 1024(Digit) = 1(V) : 204.8(Digit)
		//1000/(1024-204.8) = 1.22(1Digit당 kpa)
		//1000/( 5(V)-1(V) )= 250
//Vac
		// AD는 12bit AD Converter임. 2^12 = 4096 digit, ±10V(range 20V) -> 4096 digit
		// 10V는 2048 digit임.
		//5(V) = -101(Kpa), 1(V) = 0(Kpa)
		//5(V) : 1024(Digit) = 1(V) : 204.8(Digit)
		//-101/(1024-204.8) = -0.123(1Digit당 kpa)
		//-101/( 5(V)-1(V) )= -25.25

	if(Gauge == 2)//Vacuum Gauge
	{
// 		Resolution[Gauge] = Resolution[Gauge] - 150;
// 		Press = -((Resolution[Gauge]/204.8)*25.325);
//		(ret2<205) ? ret2 = 205 : ret2+=0;
		if(g_LineNo == "1")//m_strLineNo
		{
			if(g_MachineNo == "2")
			{
				switch(Gauge)
				{
				case 2:
					ret2 += 55;
					break;
				}
			}
			if(g_MachineNo == "1")
			{
				switch(Gauge)
				{
				case 2:
					ret2 += 0;
					break;
				}
			}
			(ret2<205) ? ret2 = 205 : ret2+=0;
		}
		if(g_LineNo == "2")//m_strLineNo
		{
			if(g_MachineNo == "2")
			{
				switch(Gauge)
				{
				case 2:
					ret2 += 0;
					break;
				}
			}
			if(g_MachineNo == "1")
			{
				switch(Gauge)
				{
				case 2:
					ret2 += 15;
					break;
				}
			}
			(ret2<205) ? ret2 = 205 : ret2+=0;
		}
		return (-0.123*ret2+25.25);
	}
	else//CDA Gauge
	{
// 		if(Gauge == 3 || Gauge == 4)
// 			Resolution[Gauge] = Resolution[Gauge] - 180;
// 		else
// 			Resolution[Gauge] = Resolution[Gauge] - 160; 
// 
// 		Press = (Resolution[Gauge]/204.8)*250.0;
//		(ret2<205) ? ret2 = 205 : ret2+=0;
		if(g_LineNo == "1")//m_strLineNo
		{
			if(g_MachineNo == "2")
			{
				switch(Gauge)
				{
				case 0:
					ret2 += 55;
					break;
				case 1:
					ret2 += 35;
					break;
				case 2:
					break;
				case 3:
					ret2 += 0;
					break;
				case 4:
					ret2 += 0;
					break;
				}
			}
			if(g_MachineNo == "1")
			{
				switch(Gauge)
				{
				case 0:
					ret2 += 0;
					break;
				case 1:
					ret2 += 0;
					break;
				case 2:
					break;
				case 3:
					ret2 += 0;
					break;
				case 4:
					ret2 += 0;
					break;
				}
			}
			(ret2<205) ? ret2 = 205 : ret2+=0;
		}
		if(g_LineNo == "2")//m_strLineNo
		{
			if(g_MachineNo == "2")
			{
				switch(Gauge)
				{
				case 0:
					ret2 += 10;
					break;
				case 1:
					ret2 += 0;
					break;
				case 2:
					break;
				case 3:
					ret2 += 0;
					break;
				case 4:
					ret2 += 0;
					break;
				}
			}
			if(g_MachineNo == "1")
			{
				switch(Gauge)
				{
				case 0:
					ret2 += 20;
					break;
				case 1:
					ret2 += 5;
					break;
				case 2:
					break;
				case 3:
					ret2 += 0;
					break;
				case 4:
					ret2 += 0;
					break;
				}
			}
			(ret2<205) ? ret2 = 205 : ret2+=0;
		}
		return (1.22*ret2-250);
	}
 //   return Press;
*/
/*
	short ret1, ret2;
	long wGetAdResult;
			
	ret1 = ret2 = 0;
	
	// 3회 평균
	for(int i=0 ; i<3 ; i++)
	{		
		FAS_GetAdResult(nAxis/_FAS_BOARD_AXES+1, (WORD)(nAxis%_FAS_BOARD_AXES), &wGetAdResult);	
		ret1 = wGetAdResult;
		if(ret1 & 0x0800) ret1 |= 0xf000;	// 음수 계산
		ret2 = (short)(ret2+ret1);
	}
	ret2 /= 3;
		
	(ret2 & 0x0800) ? (ret2|=0xf000) : 1;

	if(nAxis<HEAD_S4) //CDA쪽..
	{ 
		// AD는 12bit AD Converter임. 2^12 = 4096 digit, ±10V(range 20V) -> 4096 digit
		// 10V는 2048 digit임.
		//5(V) = 1000(Kpa), 0.6(V) = -100(Kpa)
		//5(V) : 1024(Digit) = 1(V) : 204.8(Digit)
		//1000/(1024-204.8) = 1.22(1Digit당 kpa)
		//1000/( 5(V)-1(V) )= 250
		
		//임의로 Offset
		ret2 += 40; 
		return (1.22*ret2-250);
		//return(ret2/1.024);
	}
	else //Vac쪽..
	{
		// AD는 12bit AD Converter임. 2^12 = 4096 digit, ±10V(range 20V) -> 4096 digit
		// 10V는 2048 digit임.
		//5(V) = -101(Kpa), 1(V) = 0(Kpa)
		//5(V) : 1024(Digit) = 1(V) : 204.8(Digit)
		//-101/(1024-204.8) = -0.123(1Digit당 kpa)
		//-101/( 5(V)-1(V) )= -25.25
									  //임의로 Offset
		(ret2<204.8) ? ret2 = 204.8 : ret2+=40;
		return (-0.123*ret2+25.25);
		//return(-ret2/10.14);
	}
*/
}

void CDevicePart::StageLineDropNtype(double Pos_X, double Pos_Y, double StageSpeed, double *DischargeSpeed, int point_Info)
{
	WORD StageAxis[2] = {0,1};//S0/K0
	double StagePosition[2] = {Pos_X*S_PULSE, Pos_Y*K_PULSE};

	double dStageStartSpeed = 1000.0 ;

//#ifdef LINE_NUMBER_7
//	double DropStartPos = 5000.0 ;//5000
//#endif
//
//#ifdef LINE_NUMBER_8
//	double DropStartPos = 2000.0 ;//5000
//#endif

	double DropStartPos = 10000.0 ;//기본 10mm에서 line drop offset 만큼 뺀다...//
	
	WORD DropNoOfAxis = 0;
	WORD DropNoOfAxis1 = 0;


	int DropAxis[MAX_NOZZLE] = {0,};	     // Drop 해야 할 축 번호를 저장
	int DropAxis1[MAX_NOZZLE] = {0,};	     // Drop 해야 할 축 번호를 저장

	double DropIncPos[MAX_NOZZLE] = {0,};
	double DropIncPos1[MAX_NOZZLE] = {0,};

	double DropVelocity[MAX_NOZZLE] = {0,};
	double DropVelocity1[MAX_NOZZLE] = {0,};

	int nStartDelay[MAX_NOZZLE] = {0,};
	int nStopDelay[MAX_NOZZLE] = {0,};
	int nInposValue[MAX_NOZZLE] = {0,};
	
	for(int i = 0 ; i < MAX_NOZZLE ; i++)
	{
		if((Drop_Info.head_onoff[i] * Drop_Info.drop_onoff[i]) == TRUE)
		{ 
			if(i < NOZZLE_P1)
			{
				DropAxis[DropNoOfAxis]=AXIS_P1+i;
				if(point_Info!=2)
					DropIncPos[DropNoOfAxis]=Drop_Info.m_dDistPerDropAutoNtype[i];
				else
					DropIncPos[DropNoOfAxis] = 0;
				
				TRACE("H%d #%d Point, Position(%.0f, %.0f) DropIncPos: %.0f, Speed: %d\n", i, point_Info, StagePosition[0], StagePosition[1], DropIncPos[DropNoOfAxis], Drop_Info.m_nDischargeSpeed[i]);
				
				DropVelocity[DropNoOfAxis]=DischargeSpeed[i]*1000;
				DropNoOfAxis++;
			}
			else
			{
				DropAxis1[DropNoOfAxis1]=(AXIS_P15+i-NOZZLE_P1)%BOARD_AXES;
				if(point_Info!=2)
					DropIncPos1[DropNoOfAxis1]=Drop_Info.m_dDistPerDropAutoNtype[i];
				else
					DropIncPos1[DropNoOfAxis1] = 0;
				
				TRACE("H%d #%d Point, Position(%.0f, %.0f) DropIncPos: %.0f, Speed: %d\n", i, point_Info, StagePosition[0], StagePosition[1], DropIncPos[DropNoOfAxis], Drop_Info.m_nDischargeSpeed[i]);
				
				DropVelocity1[DropNoOfAxis1]=DischargeSpeed[i]*1000;
				DropNoOfAxis1++;
			}

		}
	}
	//2번 보드의 multi를 먼저 한다...
	FAS_MoveSpdMultiAxisIncPos(2, DropNoOfAxis1, DropAxis1, DropIncPos1, DropVelocity1, 0);
//	FAS_MoveMultiTJDAxisIncPos(2, DropNoOfAxis1, DropAxis1, DropIncPos1, DropVelocity1, nStartDelay, nStopDelay, nInposValue, 0);

	FAS_MoveLineDropAbsStep(1, 2, StageAxis, StagePosition, StageSpeed, dStageStartSpeed,point_Info,
		DropNoOfAxis, DropAxis, DropIncPos, DropVelocity, DropStartPos, 7, nStartDelay, nStopDelay, nInposValue);//by shin//2008/07/23//(line drop complete bit)	

}

void CDevicePart::FlexibleStageLineDrop(double Pos_X, double Pos_Y, double StageSpeed, double DischargeSpeed, int point_Info, int DotPoint)
{
	WORD StageAxis[2] = {0,1};//S0/K0
	double StagePosition[2] = {Pos_X*S_PULSE, Pos_Y*K_PULSE};

	double dStageStartSpeed = 1000.0;
	double DropStartPos = 10000.0 ;//기본 10mm에서 line drop offset 만큼 뺀다...//

	
	WORD DropNoOfAxis = 0;
	WORD DropNoOfAxis1 = 0;

	int DropAxis[NOZZLE_P1] = {0,};	     // Drop 해야 할 축 번호를 저장
	int DropAxis1[NOZZLE_P2] = {0,};	     // Drop 해야 할 축 번호를 저장

	double DropIncPos[NOZZLE_P1] = {0,};
	double DropIncPos1[NOZZLE_P2] = {0,};

	double DropVelocity[NOZZLE_P1] = {0,};
	double DropVelocity1[NOZZLE_P2] = {0,};

	CString str[MAX_NOZZLE] = {"",};
	CString strDeviceLog;
	
	strDeviceLog.Format("%d\t\t",DotPoint);
	
	for(int i = 0 ; i < MAX_NOZZLE ; i++)
	{
		if((Drop_Info.head_onoff[i] * Drop_Info.drop_onoff[i]) == TRUE)
		{ 
//2011.05.12 by tskim 
			if(DotPoint > 0)
			{
				if(i < NOZZLE_P1)
				{
					DropAxis[DropNoOfAxis]=AXIS_P1+i;
					if(point_Info!=2)
	//					DropIncPos[DropNoOfAxis]=Drop_Info.m_dDistPerDrop[i];
						DropIncPos[DropNoOfAxis]=Drop_Info.m_dRealFlexibeDistPerDrop[i][DotPoint];
					else
						DropIncPos[DropNoOfAxis] = 0;

					TRACE("H%d #%d Point, Position(%.0f, %.0f) DropIncPos: %.0f, Speed: %d\n", i, point_Info, StagePosition[0], StagePosition[1], DropIncPos[DropNoOfAxis], Drop_Info.m_nDischargeSpeed[i]);

					DropVelocity[DropNoOfAxis]=DischargeSpeed*1000;
					DropNoOfAxis++;
				}
				else
				{
					DropAxis1[DropNoOfAxis1]=(AXIS_P15+i-NOZZLE_P1)%BOARD_AXES;
					if(point_Info!=2)
	//					DropIncPos1[DropNoOfAxis1]=Drop_Info.m_dDistPerDrop[i];
						DropIncPos1[DropNoOfAxis]=Drop_Info.m_dRealFlexibeDistPerDrop[i][DotPoint];
					else
						DropIncPos1[DropNoOfAxis1] = 0;

					TRACE("H%d #%d Point, Position(%.0f, %.0f) DropIncPos: %.0f, Speed: %d\n", i, point_Info, StagePosition[0], StagePosition[1], DropIncPos[DropNoOfAxis], Drop_Info.m_nDischargeSpeed[i]);

					DropVelocity1[DropNoOfAxis1]=DischargeSpeed*1000;
					DropNoOfAxis1++;
				}
				str[i].Format("%.0f\t",Drop_Info.m_dRealFlexibeDistPerDrop[i][DotPoint]);
			}
		}
		else
			str[i].Format("\t");

	}

	for(i = 0; i < MAX_NOZZLE; i++)
	{
		strDeviceLog += str[i];
	}
//	SaveDeviceLog(strDeviceLog);

	//2번 보드의 multi를 먼저 한다...
	FAS_MoveSpdMultiAxisIncPos(2, DropNoOfAxis1, DropAxis1, DropIncPos1, DropVelocity1, 0);

	FAS_MoveLineDropAbsStep(1, 2, StageAxis, StagePosition, StageSpeed, dStageStartSpeed,point_Info,
		DropNoOfAxis, DropAxis, DropIncPos, DropVelocity, DropStartPos, 7);//by shin//2008/07/23//(line drop complete bit)	

	Sleep(1);
}

void CDevicePart::FlexibleHeadStepDrop(double DischargeSpeed, int DropStartPos, BOOL EndCheck)
{
	int i = 0;

	WORD wNoOfAxisP1 = 0; //P1~P14
	WORD wNoOfAxisP2 = 0; //P15~P16

	int iAxisP1[MAX_NOZZLE] = {0,}; //P motor
	int iAxisP2[MAX_NOZZLE] = {0,}; //P motor

	double dPos1P1[MAX_NOZZLE] = {0,}; // P_Axis: 1drop distance
	double dPos1P2[MAX_NOZZLE] = {0,};

	double dVelocity1P1[MAX_NOZZLE] = {0,}; // P_Axis: Discharge speed
	double dVelocity1P2[MAX_NOZZLE] = {0,};


	CString str[MAX_NOZZLE] = {"",};
	CString strDeviceLog;
	strDeviceLog.Format("%d\t\t",0);
	for(i = 0; i < MAX_NOZZLE; i++)
	{
		if(TRUE==(Drop_Info.head_onoff[i] * Drop_Info.drop_onoff[i]))
		{
			if(i < NOZZLE_P1) 
			{
				iAxisP1[wNoOfAxisP1] = (AXIS_P1+i)%BOARD_AXES;

				dPos1P1[wNoOfAxisP1] = Drop_Info.m_dRealFlexibeDistPerDrop[i][0];
				dVelocity1P1[wNoOfAxisP1] = DischargeSpeed*1000;
				wNoOfAxisP1++;	
			}
			else
			{
				iAxisP2[wNoOfAxisP2] = (AXIS_P15+(i%NOZZLE_P1))%BOARD_AXES;
				dPos1P2[wNoOfAxisP2] = Drop_Info.m_dRealFlexibeDistPerDrop[i][0];
				dVelocity1P2[wNoOfAxisP2] = DischargeSpeed*1000;
				wNoOfAxisP2++;	
			}
			str[i].Format("%.0f\t",Drop_Info.m_dRealFlexibeDistPerDrop[i][0]);
		}
		else
			str[i].Format("\t");
	}

	for(i = 0; i < MAX_NOZZLE; i++)
	{
		strDeviceLog += str[i];
	}
//	SaveDeviceLog(strDeviceLog);
#if EQ	
	FAS_MoveMultiAxisIncPos(AXIS_P1/BOARD_AXES+1, wNoOfAxisP1, iAxisP1, dPos1P1, dVelocity1P1, FALSE);	
	FAS_MoveMultiAxisIncPos(AXIS_P15/BOARD_AXES+1, wNoOfAxisP2, iAxisP2, dPos1P2, dVelocity1P2, TRUE);	
	
	Sleep(INPOSITION_DELAY);

	for(i = 0; i < MAX_NOZZLE; i++)
	{
		if(Drop_Info.head_onoff[i] == TRUE)
		{
			if(i < NOZZLE_P1)							
				while(!MotionEndCheck(AXIS_P1+i));
			else
				while(!MotionEndCheck(AXIS_P1+i+2));
		}
	}
#endif
}

void CDevicePart::FlexibleNtypeStageLineDrop(int DotPoint,double Pos_X, double Pos_Y, double StageSpeed, double DischargeSpeed, int point_Info, int mode, int start_delay, int stop_delay, int inpos_value)
{
	WORD StageAxis[2] = {0,1};//S0/K0
	double StagePosition[2] = {Pos_X*S_PULSE, Pos_Y*K_PULSE};

	double dStageStartSpeed = 1000.0;
	double DropStartPos = 10000.0 ;//기본 10mm에서 line drop offset 만큼 뺀다...//

	
	WORD DropNoOfAxis = 0;
	WORD DropNoOfAxis1 = 0;

	int DropAxis[NOZZLE_P1] = {0,};	     // Drop 해야 할 축 번호를 저장
	int DropAxis1[NOZZLE_P2] = {0,};	     // Drop 해야 할 축 번호를 저장

	double DropIncPos[NOZZLE_P1] = {0,};
	double DropIncPos1[NOZZLE_P2] = {0,};

	double DropVelocity[NOZZLE_P1] = {0,};
	double DropVelocity1[NOZZLE_P2] = {0,};

	CString str[MAX_NOZZLE] = {" ",};
	CString strDeviceLog;
	CString strDeviceLog2;

	int nStartDelay[MAX_NOZZLE] = {0,};
	int nStopDelay[MAX_NOZZLE] = {0,};
	int nInposValue[MAX_NOZZLE] = {0,};
	double dNtypeDischarge[MAX_NOZZLE] = {0.0,};
	
	strDeviceLog.Format("%d\t\t",DotPoint);
	strDeviceLog.Empty();strDeviceLog2.Empty();
	
	for(int i = 0 ; i < MAX_NOZZLE ; i++)
	{
		if((Drop_Info.head_onoff[i] * Drop_Info.drop_onoff[i]) == TRUE)
		{ 
//2011.05.12 by tskim 
//			if(DotPoint > 0)
			{
				Drop_Info.m_dDistPerDropAutoNtype[i] = Drop_Info.m_dDistPerDropAutoFlexibleNtype[i][DotPoint];
				if(i < NOZZLE_P1)
				{
					DropAxis[DropNoOfAxis]=AXIS_P1+i;
					if(point_Info!=2)
	//					DropIncPos[DropNoOfAxis]=Drop_Info.m_dDistPerDrop[i];
	//					DropIncPos[DropNoOfAxis]=Drop_Info.m_dRealFlexibeDistPerDrop[i][DotPoint];
						DropIncPos[DropNoOfAxis]=Drop_Info.m_dDistPerDropAutoNtype[i];
					else
//2014.12.01 by tskim Dummy Big Drop 방지 Suction 동작 테스트 용
						DropIncPos[DropNoOfAxis] = m_dPAxis_SuctionOffset[i];
//						DropIncPos[DropNoOfAxis] = 0;


//					DropVelocity[DropNoOfAxis]=DischargeSpeed*1000;
					dNtypeDischarge[i] = Drop_Info.m_dNtypeDischargeLineDropSpeed[i][DotPoint];
					
					DropVelocity[DropNoOfAxis]=dNtypeDischarge[i]*1000;	
					
					//TRACE("H%d #%d Point, Position(%.0f, %.0f) DropIncPos: %.0f, Speed: %f\n", i, point_Info, StagePosition[0], StagePosition[1], Drop_Info.m_dDistPerDropAutoNtype[i], dNtypeDischarge[i]);


					DropNoOfAxis++;
				}
				else
				{
					DropAxis1[DropNoOfAxis1]=(AXIS_P15+i-NOZZLE_P1)%BOARD_AXES;
					if(point_Info!=2)
	//					DropIncPos1[DropNoOfAxis1]=Drop_Info.m_dDistPerDrop[i];
	//					DropIncPos1[DropNoOfAxis]=Drop_Info.m_dRealFlexibeDistPerDrop[i][DotPoint];
						DropIncPos1[DropNoOfAxis1]=Drop_Info.m_dDistPerDropAutoNtype[i];
					else
//2014.12.01 by tskim Dummy Big Drop 방지 Suction 동작 테스트 용
						DropIncPos[DropNoOfAxis1] = m_dPAxis_SuctionOffset[i];
//						DropIncPos1[DropNoOfAxis1] = 0;

					//TRACE("H%d #%d Point, Position(%.0f, %.0f) DropIncPos: %.0f, Speed: %d\n", i, point_Info, StagePosition[0], StagePosition[1], DropIncPos1[DropNoOfAxis], Drop_Info.m_nDischargeSpeed[i]);

//					DropVelocity1[DropNoOfAxis1]=DischargeSpeed*1000;
					dNtypeDischarge[i] = Drop_Info.m_dNtypeDischargeLineDropSpeed[i][DotPoint];
					
					DropVelocity1[DropNoOfAxis1]=dNtypeDischarge[i]*1000;
					DropNoOfAxis1++;
				}
				//str[i].Format("%.0f/%.1f\t",Drop_Info.m_dDistPerDropAutoFlexibleNtype[i][DotPoint],dNtypeDischarge[i]);
				//str[(MAX_NOZZLE-1)-i].Format("%.0f\t",Drop_Info.m_dDistPerDropAutoFlexibleNtype[(MAX_NOZZLE-1)-i][DotPoint]);
				str[(MAX_NOZZLE-1)-i].Format("%.0f(%d)_%.0f\t",Drop_Info.m_dDistPerDropAutoFlexibleNtype[(MAX_NOZZLE-1)-i][DotPoint],Drop_Info.m_nDistPerDropPanelNO[(MAX_NOZZLE-1)-i][DotPoint],m_dPAxis_SuctionOffset[(MAX_NOZZLE-1)-i]);
				strDeviceLog += str[(MAX_NOZZLE-1)-i];
			}
//			TRACE("<<>>H%d #%d Point, Position(%.0f, %.0f) DropIncPos: %.0f, Speed: %f\n", i, point_Info, StagePosition[0], StagePosition[1], Drop_Info.m_dDistPerDropAutoNtype[i], dNtypeDischarge[i]);
		}
		else
		{
//			str[i].Format("%.0f/%.1f\t",Drop_Info.m_dDistPerDropAutoFlexibleNtype[i][DotPoint],dNtypeDischarge[i]);
//			str[(MAX_NOZZLE-1)-i].Format("%.0f\t",Drop_Info.m_dDistPerDropAutoFlexibleNtype[(MAX_NOZZLE-1)-i][DotPoint]);
			str[(MAX_NOZZLE-1)-i].Format("%.0f(%d)_%.0f\t",Drop_Info.m_dDistPerDropAutoFlexibleNtype[(MAX_NOZZLE-1)-i][DotPoint],Drop_Info.m_nDistPerDropPanelNO[(MAX_NOZZLE-1)-i][DotPoint],m_dPAxis_SuctionOffset[(MAX_NOZZLE-1)-i]);
			strDeviceLog += str[(MAX_NOZZLE-1)-i];
		}
	}
	//if(point_Info!=2)if(point_Info!=2)
	{
		strDeviceLog2.Format("\t<<>>LineNO%d\tDotPt%d\tDropInfo#%d\tPos(\t%.0f\t%.0f\t)\t%s",m_nDropLineNO ,DotPoint, point_Info, StagePosition[0], StagePosition[1], strDeviceLog);
		//TRACE(strDeviceLog2+"\n");
	}	

//	for(i = 0; i < MAX_NOZZLE; i++)
//	{
//		strDeviceLog += str[i];
//	}

	SaveDeviceLog(strDeviceLog2);
//2014.12.01 by tskim Dummy Big Drop 방지 Suction 동작 테스트 용
// 	if(point_Info!=2)
// 	{
// 		FAS_MoveMultiAxisIncPos(AXIS_P1/BOARD_AXES+1, wNoOfAxisP1_PAxis_SuctionOffset, iAxisP1_PAxis_SuctionOffset, dPos1P1_PAxis_SuctionOffset, dVelocity1P1_PAxis_SuctionOffset, FALSE);	
// 		FAS_MoveMultiAxisIncPos(AXIS_P15/BOARD_AXES+1, wNoOfAxisP2_PAxis_SuctionOffset, iAxisP2_PAxis_SuctionOffset, dPos1P2_PAxis_SuctionOffset, dVelocity1P2_PAxis_SuctionOffset, TRUE);
// 	}

	//2번 보드의 multi를 먼저 한다...
	FAS_MoveSpdMultiAxisIncPos(2, DropNoOfAxis1, DropAxis1, DropIncPos1, DropVelocity1, 0);

//	FAS_MoveLineDropAbsStep(1, 2, StageAxis, StagePosition, StageSpeed, dStageStartSpeed,point_Info,
//		DropNoOfAxis, DropAxis, DropIncPos, DropVelocity, DropStartPos, 7);//by shin//2008/07/23//(line drop complete bit)	

//	FAS_MoveMultiTJDAxisIncPos(2, DropNoOfAxis1, DropAxis1, DropIncPos1, DropVelocity1, nStartDelay, nStopDelay, nInposValue, 0);

    FAS_MoveLineDropAbsStep(1, 2, StageAxis, StagePosition, StageSpeed, dStageStartSpeed, point_Info,
			DropNoOfAxis, DropAxis, DropIncPos, DropVelocity, DropStartPos, 7, nStartDelay, nStopDelay, nInposValue);

//	Sleep(1);

/*	WORD StageAxis[2] = {0,1};//S0/K0
	double StagePosition[2] = {Pos_X*S_PULSE, Pos_Y*K_PULSE};

	double dStageStartSpeed = 1000.0 ;
//#ifdef LINE_NUMBER_7
//	double DropStartPos = 5000.0 ;//5000
//#endif
//
//#ifdef LINE_NUMBER_8
//	double DropStartPos = 2000.0 ;//5000
//#endif
	

	mode = 0;

	WORD DropNoOfAxis = 0;
	WORD DropNoOfAxis1 = 0;

	int DropAxis[MAX_NOZZLE] = {0,};	     // Drop 해야 할 축 번호를 저장

	double DropIncPos[MAX_NOZZLE] = {0,};

	double DropVelocity[MAX_NOZZLE] = {0,};

	int nStartDelay[MAX_NOZZLE] = {0,};
	int nStopDelay[MAX_NOZZLE] = {0,};
	int nInposValue[MAX_NOZZLE] = {0,};
	double dNtypeDischarge[MAX_NOZZLE] = {0.0,};
	
	for(int i = 0 ; i < MAX_NOZZLE ; i++)
	{
		if((Drop_Info.head_onoff[i] * Drop_Info.drop_onoff[i]) == TRUE)
		{ 
			Drop_Info.m_dDistPerDropAuto[i] = Drop_Info.m_dDistPerDropAutoFlexibleNtype[i][Block[i]];

			DropAxis[DropNoOfAxis]=AXIS_P1+i;
			if(point_Info!=2)
//				DropIncPos[DropNoOfAxis]=Drop_Info.m_dDistPerDrop[i];
				DropIncPos[DropNoOfAxis]=Drop_Info.m_dDistPerDropAuto[i];
			else
				DropIncPos[DropNoOfAxis] = 0;

		//	TRACE("H%d #%d Point, Position(%.0f, %.0f) DropIncPos: %.0f, Speed: %d\n", i, point_Info, StagePosition[0], StagePosition[1], DropIncPos[DropNoOfAxis], Drop_Info.m_nDischargeSpeed[i]);

//			DropVelocity[DropNoOfAxis]=DischargeSpeed[i][Block[i]]*1000;

			dNtypeDischarge[i] = Drop_Info.m_dNtypeDischargeLineDropSpeed[i][Block[i]];

			DropVelocity[DropNoOfAxis]=dNtypeDischarge[i]*1000;
			DropNoOfAxis++;
		}
	}


    FAS_MoveLineDropAbsStep(1, 2, StageAxis, StagePosition, StageSpeed, dStageStartSpeed, point_Info,
			DropNoOfAxis, DropAxis, DropIncPos, DropVelocity, DropStartPos, 7, nStartDelay, nStopDelay, nInposValue);
*/
}

CString CDevicePart::BinToHex(CString strData)
{
	int nLength;
	int i,j;
	long Temp,TempHex;
	CString strReturn,strTemp[4],str[4];
	nLength = strData.GetLength();

	if(nLength<16)
	{
		for(i=0;i<16-nLength;i++)
		{
			strData ="0"+strData;
		}
	}
	for(i=0;i<4;i++)
	{
		strTemp[i] = strData.Mid(i*4,4);
		Temp = HexConvert(strTemp[i].GetAt(0))*8+HexConvert(strTemp[i].GetAt(1))*4+HexConvert(strTemp[i].GetAt(2))*2+HexConvert(strTemp[i].GetAt(3))*1;
		str[i].Format("%x",Temp);
		strReturn += str[i];
	}

	return strReturn;
}

void CDevicePart::SaveDeviceLog(CString strText)
{
	CFileFind filefind;
	FILE *fp;
	CFileException e;
	
	if(strText == "") return;
	
	CTime t = CTime::GetCurrentTime();
	
	// Glass ID Directory 생성
	
	CString strFileName, strDirName, str;
	BOOL bIsDir = FALSE;
	BOOL bContinue = TRUE;
	
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
			if(strDirName == "DeviceLog")
			{
				bIsDir = TRUE;
				break;
			}
		}
	}
	
	filefind.Close();
	
	if(!bIsDir)	_mkdir(g_strLogPath+"\\DeviceLog");
	// 날짜 Directory 생성 완료
	
	strFileName.Format(g_strLogPath+"\\DeviceLog\\Device_%d-%02d-%02d.log", t.GetYear(), t.GetMonth(), t.GetDay());
	// 화일 열기
	fp = fopen((char *)(LPCSTR)strFileName, "at");
	if(!fp) 
	{
		return;
	}
	str.Format("%02d시%02d분%02d초 - ", t.GetHour(), t.GetMinute(), t.GetSecond());
	str += strText;
	str += "\n";
	fprintf(fp, str);
	fclose(fp);
	
	//Log Back Up
	filefind.FindFile(g_strLogPath+"\\Set-Up\\*.*");
	bIsDir = FALSE;
	bContinue = TRUE;
	while(bContinue)
	{
		bContinue = filefind.FindNextFile();
		if(filefind.IsDirectory())
		{
			strDirName = filefind.GetFileName();
			if(strDirName == "DeviceLog")
			{
				bIsDir = TRUE;
				break;
			}
		}
	}
	
	filefind.Close();
	
	if(!bIsDir)	_mkdir(g_strLogPath+"\\Set-Up\\DeviceLog");
	// 날짜 Directory 생성 완료
	
	strFileName.Format(g_strLogPath+"\\Set-Up\\DeviceLog\\Device_%d-%02d-%02d.log", t.GetYear(), t.GetMonth(), t.GetDay());
	// 화일 열기
	fp = fopen((char *)(LPCSTR)strFileName, "at");
	if(!fp) 
	{
		return;
	}
	str.Format("%02d시%02d분%02d초 - ", t.GetHour(), t.GetMinute(), t.GetSecond());
	str += strText;
	str += "\n";
	fprintf(fp, str);
	fclose(fp);
/*	
	// 한달 이전 Log file은 지워버린다...
	// 디렉토리 탐색
	filefind.FindFile(g_strLogPath+"\\DeviceLog\\*.*");
	bIsDir = FALSE;
	bContinue = TRUE;
	while(bContinue)
	{
		bContinue = filefind.FindNextFile();
		if(!filefind.IsDirectory())
		{
			strFileName = filefind.GetFileName();
			str = strFileName;
			int nYear, nMonth, nDay;
			int nYear1, nMonth1, nDay1;
			nYear = t.GetYear();
			nMonth = t.GetMonth();
			nDay = t.GetDay();
			
			nYear1 = atoi(str.Mid(7, 4));
			nMonth1 = atoi(str.Mid(12, 2));
			nDay1 = atoi(str.Mid(15, 2));
			
			if(nYear1 < nYear) 
			{
				::DeleteFile(g_strLogPath+"\\DeviceLog\\"+strFileName);
			}
			if((nMonth1*30+nDay1) < (nMonth*30+nDay-30))
			{
				::DeleteFile(g_strLogPath+"\\DeviceLog\\"+strFileName);
			}
		}
	}
*///2014.11.29 by tskim 막음...용량이 크지 않아서;;;
}

BOOL CDevicePart::ColumnPosition_Compare()		//ehji 141022
{
	double pos = 0.0, pos1 = 0.0;
	
	FAS_GetActualPos(AXIS_K1/BOARD_AXES+1, AXIS_K1%BOARD_AXES, &pos);

	FAS_GetActualPos(AXIS_K2/BOARD_AXES+1, AXIS_K2%BOARD_AXES, &pos1);
	
	if( pos < 0) 
		 pos = -1*(pos);
	if( pos1 < 0) 
		pos1 = -1*(pos1);
	
	if( (pos - pos1) >= 100 || (pos - pos1) <= -100 )
		return TRUE;
	else
		return FALSE;
}

WORD CDevicePart::CheckNewDropSensor(int *nHeadInform) // 맺힘 감지 신규 컨셉 
{
	BOOL bSyncIOResult = FALSE, bDropSensing = FALSE, bUpSensing = FALSE , bDownSensing = FALSE , bErrorStatus = FALSE; 
	WORD wTempIO = 0, wIOResult = 0, wIOResult1 = 0;

CString str = "SYLINDER " ,strTemp = ""; 

#if LINE3
// INTERLOCK 추가 할것. 
	for ( int nHeadCount = 0; nHeadCount < (MAX_NOZZLE/2) ; nHeadCount++ )
	{
		FAS_GetIoBit(4,true, nHeadCount+SYLINDER1_UP_SENSOR, &bUpSensing);
		FAS_GetIoBit(4,true, nHeadCount+SYLINDER1_DW_SENSOR, &bDownSensing);

		if( bUpSensing || !bDownSensing )
		{
			strTemp.Format("%d, ", nHeadCount+1); 
			str += strTemp; 
			bErrorStatus = TRUE; 
		}
	}

	if ( bErrorStatus )
	{
		str += "실린더 상태를 확인하세요."; 
		
		AfxMessageBox(str); //GLOBAL 구조체 추가 하여 알람 처리 할것. 
		wIOResult = 0xffff; 
		return wIOResult; 
	}

//  DUMMY 위치이동 	
	bSyncIOResult = SyncSetIO(SYNC_AXIS_S, 0.0, false, false);	
	Sleep(100);

	if(bSyncIOResult != TRUE) 
	{
		AfxMessageBox("S축 동기I/O 해제를 실패하였습니다"); //GLOBAL 구조체 추가 하여 알람 처리 할것. 
		wIOResult = 0xffff; 
		return wIOResult; 
	}
	else
	{
		HeadSPositionMove( g_dTeachData[0][0],g_dTeachData[0][1],g_dTeachData[0][2],
						   g_dTeachData[1][0],g_dTeachData[1][1],g_dTeachData[1][2],
						   g_dTeachData[2][0],g_dTeachData[2][1],g_dTeachData[2][2],
						   g_dTeachData[3][0],g_dTeachData[3][1],g_dTeachData[3][2],
						   g_dTeachData[4][0],g_dTeachData[4][1],g_dTeachData[4][2],
						   g_dTeachData[5][0],1);

    	ColumnPositionMove(g_dTeachData[5][2] + 40.0, 0, g_dTeachData[39][2],1);
		Sleep(10);
     	ColumnPositionMove(g_dTeachData[5][2], 0, g_dTeachData[39][2],1);

		//구동 완료 되면...대기시간 동안 대기...//
		Sleep((int)(g_dTeachData[14][1]*1000));

		//Dummy 위치로 보낸다...//
     	ColumnPositionMove( g_dTeachData[5][2], 0, g_dTeachData[39][2], 0 );
	}

	FAS_SetIoBit( 4, true, SYLINDER_UPDW_SOL, TRUE ); 
	Sleep(100);


	for ( nHeadCount = 0; nHeadCount < MAX_NOZZLE; nHeadCount++ )
	{
		if ( nHeadInform[nHeadCount] )
		{
			FAS_GetIoBit( 1,false, nHeadCount+DROP_CHK1, &bDropSensing );

			if ( bDropSensing )
			{
				wTempIO = 0x0001;
				wTempIO = wTempIO<<nHeadCount;
				wIOResult += wTempIO;
			}

		}
	}

	FAS_SetIoBit( 4, true, SYLINDER_UPDW_SOL, FALSE ); 

	for ( nHeadCount = 0; nHeadCount < (MAX_NOZZLE/2) ; nHeadCount++ )
	{
		FAS_GetIoBit(4,true, nHeadCount+SYLINDER1_UP_SENSOR, &bUpSensing);
		FAS_GetIoBit(4,true, nHeadCount+SYLINDER1_DW_SENSOR, &bDownSensing);

		if( bUpSensing || !bDownSensing )
		{
			strTemp.Format("%d, ", nHeadCount+1); 
			str += strTemp; 
			bErrorStatus = TRUE; 
		}
	}

	if ( bErrorStatus )
	{
		str += "실린더 상태를 확인하세요."; 
		
		AfxMessageBox(str); //GLOBAL 구조체 추가 하여 알람 처리 할것. 
		wIOResult = 0xffff; 
		return wIOResult; 
	}
#endif

#if _INTERLOCK
	return wIOResult;
#else
	wIOResult1 = 0;	
#endif
}

 // Manual.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"
#include "P8CA_LcDispDoc.h"

#include "Manual.h"
//
#include "NormalMsg.h"
#include "ManuIo.h"
#include "ManuOper.h"
#include "ManuPatDrop.h"
#include "ManuWarmUp.h"
#include "ManuMeasWidth.h"
#include "MarkAlignInsp.h"

#include "ManuVisionWings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//

extern int g_nSupplyHead;
extern BOOL g_bManualSupply;

BOOL bDoorAlarm;//jykim 170626 bDoorAlarm 변수 추가

/////////////////////////////////////////////////////////////////////////////
// CManual dialog


CManual::CManual(CWnd* pParent /*=NULL*/)
	: CDialog(CManual::IDD, pParent)
{
	//{{AFX_DATA_INIT(CManual)
	//}}AFX_DATA_INIT
	m_nTimerCount = 0;
	m_nMoveCode = 0;

	m_bCDA1 = false;
	m_bCDA2 = false;

	m_bPinch1 = false;
	m_bPinch2 = false;

	m_bNozzleForword1 = false;
	m_bNozzleForword2 = false;

}

CManual::~CManual()
{
}

void CManual::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CManual)
	DDX_Control(pDX, IDC_MANU_VACUUM_ON, m_ctrlStageVacOn);
	DDX_Control(pDX, IDC_MANU_VACUUM_OFF, m_ctrlStageVacOff);
	DDX_Control(pDX, IDC_LABEL_RECIPE_NAME, m_ctrlRecipeName);
	DDX_Control(pDX, IDC_IONIZER_ON, m_ctrlIonizerON);
	DDX_Control(pDX, IDC_IONIZER_OFF, m_ctrlIonizerOFF);
	DDX_Control(pDX, IDC_CDA1, m_ctrlCDA1);
	DDX_Control(pDX, IDC_CDA2, m_ctrlCDA2);
	DDX_Control(pDX, IDC_PINCH1, m_ctrlPinch1);
	DDX_Control(pDX, IDC_PINCH2, m_ctrlPinch2);
	DDX_Control(pDX, IDC_SUPPLY_NOZZLE1, m_ctrlNozzleForword1);
	DDX_Control(pDX, IDC_SUPPLY_NOZZLE2, m_ctrlNozzleForword2);
	DDX_Control(pDX, IDC_SPY_HEAD, m_ctrlSpyHead);
	DDX_Control(pDX, IDC_MANU_PURGE_ON, m_ctrlStagePurge);
	DDX_Control(pDX, IDC_SEN_DOOR0, m_ctrlSenDoor0);
	DDX_Control(pDX, IDC_SEN_DOOR1, m_ctrlSenDoor1);
	DDX_Control(pDX, IDC_SEN_DOOR2, m_ctrlSenDoor2);
	DDX_Control(pDX, IDC_SEN_DOOR3, m_ctrlSenDoor3);
	DDX_Control(pDX, IDC_SEN_ROBOT, m_ctrlSenRobot);
	DDX_Control(pDX, IDC_SEN_GLASS, m_ctrlSenGlass);
	DDX_Control(pDX, IDC_SEN_EMO1, m_ctrlSenEMO1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CManual, CDialog)
	//{{AFX_MSG_MAP(CManual)
	ON_COMMAND_RANGE(IDC_MANU_PTP_CENT,IDC_MANU_PTP_HEAD_CHANGE,OnManuMove)//IDC_MANU_PTP_HEAD_WAIT	
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CManual message handlers

BOOL CManual::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	SelectLanguage();
//
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CString str="";

	bDoorAlarm = FALSE; //jykim 170626 도어알람변수 초기화
	ThreadStage.AdjustInitCode=0;
	//by shin//2009.06.16//

//	pDoc->m_bAutoManualFlag = TRUE; //090612 Manual 시 ErrMsg 처리 확인 변수 초기화 by shlee
//	pView->SendMessage(WM_ERROR,306,NULL);

	if(pDoc->m_bIsEngineerMode==FALSE)
	{
		GetDlgItem(IDC_CMD_IO)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
		
//		m_ctrlSpyHead.SetEnabled(false);    
		m_ctrlCDA1.SetEnabled(false);
		m_ctrlCDA2.SetEnabled(false);
		m_ctrlPinch1.SetEnabled(false); //by shin//2009.06.16//사용 head가 선택 되지 않으면 무조건 막는다...//
		m_ctrlPinch2.SetEnabled(false);
		m_ctrlNozzleForword1.SetEnabled(false);
		m_ctrlNozzleForword2.SetEnabled(false);
	}
	else    
	{
 		m_ctrlCDA1.SetEnabled(false);
 		m_ctrlCDA2.SetEnabled(false);
 		m_ctrlPinch1.SetEnabled(false);
 		m_ctrlPinch2.SetEnabled(false);
 		m_ctrlNozzleForword1.SetEnabled(false);
 		m_ctrlNozzleForword2.SetEnabled(false);
	}    //09.06.05 EngineerMode 수동 중앙 공급 활성화 by lee seung hyun // OperatorMode Pinch 기능 활성화 090612

//	GetDlgItem(IDC_MANU_PTP_HOME)->MoveWindow(0,0,0,0);
	GetDlgItem(IDC_MANU_PTP_CHANGE)->MoveWindow(0,0,0,0);
//	GetDlgItem(IDC_MANU_PTP_HEAD_CHANGE)->MoveWindow(0,0,0,0);
	GetDlgItem(IDC_MANU_PTP_HEAD_ORG)->MoveWindow(0,0,0,0);

	str.Format("%s|%s", pDoc->m_structOperatorModeRecipeData.strFrontRecipeName, pDoc->m_structOperatorModeRecipeData.strBackRecipeName);
	m_ctrlRecipeName.SetCaption(str);
	SetTimer(0,100,NULL);
//
	//Door 안전사고 예방을 위해 추가...Yamary...070628
	m_bDoorUsing[0] = FALSE;
	m_bDoorUsing[1] = FALSE;

	//임시...//
	g_nSupplyHead = 100;

	if(pView->m_nLanguage == 0)
	m_ctrlSpyHead.SetCaption("사용안함");
	else if(pView->m_nLanguage == 1)
	m_ctrlSpyHead.SetCaption("Don't USE");
	else if(pView->m_nLanguage == 2)
	m_ctrlSpyHead.SetCaption("不適用");

// 	m_ctrlCDA1.SetEnabled(false);
// 	m_ctrlCDA2.SetEnabled(false);
// 	m_ctrlPinch1.SetEnabled(false);
// 	m_ctrlPinch2.SetEnabled(false);
// 	m_ctrlNozzleForword1.SetEnabled(false);
// 	m_ctrlNozzleForword2.SetEnabled(false);
	
	// by ckh 2009.02.12 Stage에 Vacuum이 On 되어 있을 경우. 버튼 색깔 변경함. 
	BOOL	bIOResult1 = FALSE;	
/*	FAS_GetIoBit(2,true,STAGE_VACUUM_PRESS,&bIOResult1);
	Sleep(10);
	if(bIOResult1 == TRUE) // Vacuum On 일 경우 
	{ 
		m_ctrlStageVacOn.SetBackColor(LIGHTGREEN);
	}*/
	GetDlgItem(IDC_MANU_VACUUM_ON)->MoveWindow(0,0,0,0);
//	GetDlgItem(IDC_MANU_MEASURE_POS)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );

//2015.01.13 by tskim
	GetDlgItem(IDC_MANU_MARK_ALIGN)->EnableWindow(FALSE);
	GetDlgItem(IDC_MANU_MEASURE_POS)->EnableWindow(FALSE);	

//2010.02.18 by tskim ECS Test 
//	pDoc->m_bAutoManualFlag = TRUE;
//	pView->PostMessage(WM_ERROR,51,NULL);

	if(pDoc->m_structDataEditor.m_bUse_Vision)
		GetDlgItem(IDC_MANU_VISION)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_MANU_VISION)->EnableWindow(FALSE);		
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CManual::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CPen pen;
	pen.CreatePen(PS_SOLID,5,DARKBLUE);
	CPen *pOldPen = (CPen *)dc.SelectObject(&pen);

	dc.MoveTo(0,HEIGHT_YPOS);
	dc.LineTo(SCREEN_WIDTH,HEIGHT_YPOS);

//	dc.MoveTo(0,SCREEN_HEIGHT - HEIGHT_YPOS);
//	dc.LineTo(SCREEN_WIDTH,SCREEN_HEIGHT - HEIGHT_YPOS);

	dc.SelectObject(pOldPen);	
	// Do not call CDialog::OnPaint() for painting messages
}

void CManual::OnOK() 
{
	// TODO: Add extra validation here
	
//	CDialog::OnOK();
}

void CManual::OnCancel() 
{
	// TODO: Add extra cleanup here
	
//	CDialog::OnCancel();
}

BEGIN_EVENTSINK_MAP(CManual, CDialog)
    //{{AFX_EVENTSINK_MAP(CManual)
	ON_EVENT(CManual, IDC_MANU_VACUUM_ON, -600 /* Click */, OnClickManuVacuumOn, VTS_NONE)
	ON_EVENT(CManual, IDC_MANU_VACUUM_OFF, -600 /* Click */, OnClickManuVacuumOff, VTS_NONE)
	ON_EVENT(CManual, IDC_CMD_IO, -600 /* Click */, OnClickCmdIo, VTS_NONE)
	ON_EVENT(CManual, IDC_CMD_RETURN, -600 /* Click */, OnClickCmdReturn, VTS_NONE)
	ON_EVENT(CManual, IDC_MANU_MOVE, -600 /* Click */, OnClickManuMove, VTS_NONE)
	ON_EVENT(CManual, IDC_MANU_WARM_UP, -600 /* Click */, OnClickManuWarmUp, VTS_NONE)
	ON_EVENT(CManual, IDC_MANU_DROP, -600 /* Click */, OnClickManuDrop, VTS_NONE)
	ON_EVENT(CManual, IDC_MANU_DISCHARGE, -600 /* Click */, OnClickManuDischarge, VTS_NONE)
	ON_EVENT(CManual, IDC_IONIZER_ON, -600 /* Click */, OnClickIonizerOn, VTS_NONE)
	ON_EVENT(CManual, IDC_IONIZER_OFF, -600 /* Click */, OnClickIonizerOff, VTS_NONE)
	ON_EVENT(CManual, IDC_CDA1, -600 /* Click */, OnClickCda1, VTS_NONE)
	ON_EVENT(CManual, IDC_CDA2, -600 /* Click */, OnClickCda2, VTS_NONE)
	ON_EVENT(CManual, IDC_PINCH1, -600 /* Click */, OnClickPinch1, VTS_NONE)
	ON_EVENT(CManual, IDC_PINCH2, -600 /* Click */, OnClickPinch2, VTS_NONE)
	ON_EVENT(CManual, IDC_SUPPLY_NOZZLE1, -600 /* Click */, OnClickSupplyNozzle1, VTS_NONE)
	ON_EVENT(CManual, IDC_SUPPLY_NOZZLE2, -600 /* Click */, OnClickSupplyNozzle2, VTS_NONE)
	ON_EVENT(CManual, IDC_MANU_MEASURE_POS, -600 /* Click */, OnClickManuMeasurePos, VTS_NONE)
	ON_EVENT(CManual, IDC_SPY_HEAD, -600 /* Click */, OnClickSpyHead, VTS_NONE)
	ON_EVENT(CManual, IDC_LABEL_LC_SUPPLY_GROUP, -601 /* DblClick */, OnDblClickLabelLcSupplyGroup, VTS_DISPATCH)
	ON_EVENT(CManual, IDC_MANU_PURGE_ON, -600 /* Click */, OnClickManuPurgeOn, VTS_NONE)
	ON_EVENT(CManual, IDC_MANU_MARK_ALIGN, -600 /* Click */, OnClickManuMarkAlign, VTS_NONE)
	ON_EVENT(CManual, IDC_MANU_VISION, -600 /* Click */, OnClickManuVision, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CManual::OnManuMove(int nsel) 
{
	// TODO: Add your control notification handler code here
////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
	// BY CKH 2008.11.29 
	BOOL bIOResult1=FALSE;
	BOOL bIOResult2=FALSE;
	BOOL bIOResult3=FALSE;
	BOOL bIOResult4=FALSE;
	BOOL bSatetyInterlock = FALSE;

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();


	FAS_GetIoBit(1,false,DOOR_SENSOR1,&bIOResult1);	// DOOR Check
	FAS_GetIoBit(1,false,DOOR_SENSOR2,&bIOResult2);	// DOOR Check
	FAS_GetIoBit(1,false,DOOR_SENSOR3,&bIOResult3);	// DOOR Check
	FAS_GetIoBit(1,false,MAINT_DOOR,&bIOResult4);	// DOOR Check


	if((bIOResult1 == TRUE)||(bIOResult2 == TRUE)||(bIOResult3 == TRUE)||(bIOResult4 == TRUE))
	{
		ThreadStage.bEmergencyStop=TRUE; ThreadStage.ExitFlag = TRUE; //jykim 170626 매뉴얼 동작 시 문열리면 장비 멈춤
		pView->m_pDevice->Emergency_Stop();
///////////////////////////////////////////////////////////////20180830 jeongyong - Door No 추가
		int nDoorNo= 0;
		if(bIOResult1==TRUE) nDoorNo=1;
		else if(bIOResult2==TRUE) nDoorNo=2;			
		else if(bIOResult3==TRUE) nDoorNo=3;
		else if(bIOResult4==TRUE) nDoorNo=4;

		pView->SendMessage(WM_ERROR,101,nDoorNo);
		return;
	}

//2011.04.06 by tskim Robot arm InterLock
	if(pView->m_pDevice->Robot_arm_check()==TRUE)
	{	
		pView->m_pDevice->Emergency_Stop();	
		pView->SendMessage(WM_ERROR,104,NULL);
		return;
	}

//////////////////////////////////////////////////20181130 jeongyong - KEY 상태 체크
#if AUTOKEY_INTERLOCK
	int nAutoKey = 0;
	FAS_GetIoBit(2,TRUE,AUTO_MODE,&nAutoKey);
	if(nAutoKey != TRUE)
	{
		AfxMessageBox("Change Key (Manual -> Auto)!!"); 
		Sleep(1000);
		return;
	}
#endif

//#endif	
/////////////////////////////////////////////////////////////////
	switch( nsel )
	{
	case IDC_MANU_PTP_CENT:		// resource define : 1015
		ThreadStage.PtpMoveCode='a';

		if(pView->m_nLanguage ==0)
		m_NormalMsg.m_strMsg1 = " Column이 중심으로 움직입니다"; 
		else if(pView->m_nLanguage ==1)
		m_NormalMsg.m_strMsg1 = " Column will move to Stage Center"; 
		else if(pView->m_nLanguage == 2)
		m_NormalMsg.m_strMsg1 = " 移動Column中心位置"; 

		break;
	case IDC_MANU_PTP_CHANGE:	// 1018
		return;
//		ThreadStage.PtpMoveCode='c';
//		m_NormalMsg.m_strMsg1 = " Model 교환 위치로 움직입니다"; 
		break;
	case IDC_MANU_PTP_OPER:		// 1017
		ThreadStage.PtpMoveCode='d';
		if(pView->m_nLanguage ==0)
		m_NormalMsg.m_strMsg1 = " 작업자 위치로 움직입니다";
		else if(pView->m_nLanguage ==1)
		m_NormalMsg.m_strMsg1 = " Column will move to Operater Position"; 
		else if(pView->m_nLanguage == 2)
		m_NormalMsg.m_strMsg1 = " 移動作業者位置"; 

		break;
	case IDC_MANU_PTP_HOME:		// 1016
		ThreadStage.PtpMoveCode='e';
		if(pView->m_nLanguage ==0)
		m_NormalMsg.m_strMsg1 = " Dummy Cup 교환 위치로 이동합니다";
		else if(pView->m_nLanguage ==1)
		m_NormalMsg.m_strMsg1 = " Column will move to Dummy Cup change Position"; 
		else if(pView->m_nLanguage == 2)
		m_NormalMsg.m_strMsg1 = " 移動Dummy Cup交換位置"; 
		break;
	case IDC_MANU_PTP_LOAD:		// 1019
		ThreadStage.PtpMoveCode='f';
		if(pView->m_nLanguage ==0)
		m_NormalMsg.m_strMsg1 = " Column이 Front Loading 위치로 움직입니다"; 
		else if(pView->m_nLanguage ==1)
		m_NormalMsg.m_strMsg1 = " Column will move to Front Loading Position "; 
		else if(pView->m_nLanguage == 2)
		m_NormalMsg.m_strMsg1 = " 移動Column到 Front Loading位置"; 
		break;
	case IDC_MANU_PTP_DUMMY:	// 1020
		ThreadStage.PtpMoveCode='g';
		if(pView->m_nLanguage ==0)
		m_NormalMsg.m_strMsg1 = " Column이 더미 토출 위치로 움직입니다"; 
		else if(pView->m_nLanguage ==1)
		m_NormalMsg.m_strMsg1 = " Column will move to Dummy Dispensing Position"; 
		else if(pView->m_nLanguage == 2)
		m_NormalMsg.m_strMsg1 = " 移動 Column到Dummy吐出位置"; 
		break;
	case IDC_MANU_PTP_HEAD_ORG:	// 1021
		return;
//		ThreadStage.PtpMoveCode='i';
//		m_NormalMsg.m_strMsg1 = " Head s축이 Pump 장착 위치로 움직입니다"; 
		break;
	case IDC_MANU_PTP_HEAD_JOB:	// 1022
		ThreadStage.PtpMoveCode='j';
		if(pView->m_nLanguage ==0)
			m_NormalMsg.m_strMsg1 = " Head s축이 작업 위치로 움직입니다"; 
		else if(pView->m_nLanguage ==1)
			m_NormalMsg.m_strMsg1 = " Head S Axis will move to Operation position"; 
		else if(pView->m_nLanguage == 2)
			m_NormalMsg.m_strMsg1 = " 移動Head s軸到作業者位置"; 
		break;
	case IDC_MANU_PTP_HEAD_MEAS:	// 1023
		ThreadStage.PtpMoveCode='k';
		if(pView->m_nLanguage ==0)
		m_NormalMsg.m_strMsg1 = " Head s축이 측정 위치로 움직입니다"; 
		else if(pView->m_nLanguage ==1)
		m_NormalMsg.m_strMsg1 = " Head S Axis will move to Calibration position"; 
		else if(pView->m_nLanguage == 2)
		m_NormalMsg.m_strMsg1 = " 移動Head s周到測量位置"; 
		break;
	case IDC_MANU_PTP_HEAD_WAIT:	// 1026
		ThreadStage.PtpMoveCode='l';
		if(pView->m_nLanguage ==0)
		m_NormalMsg.m_strMsg1 = " Head s축이 더미 위치로 움직입니다"; 
		else if(pView->m_nLanguage ==1)
		m_NormalMsg.m_strMsg1 = " Head S Axis will move to Dummy position"; 
		else if(pView->m_nLanguage == 2)
		m_NormalMsg.m_strMsg1 = " Head s軸移動到Dummy位置"; 
		break;
	case IDC_MANU_PTP_BOTCHG:	//new : 1024
		ThreadStage.PtpMoveCode='x';
		if(pView->m_nLanguage ==0)
		m_NormalMsg.m_strMsg1 = "Column이 액정 공급 위치로 움직입니다 ";
		else if(pView->m_nLanguage ==1)
		m_NormalMsg.m_strMsg1 = "Column will move to LC supply Position ";
		else if(pView->m_nLanguage == 2)
		m_NormalMsg.m_strMsg1 = "移動Column到液晶供給位置 ";
		break;
	case IDC_MANU_PTP_COL_MEA:	// new : 1025
		ThreadStage.PtpMoveCode='y';
		if(pView->m_nLanguage ==0)
		m_NormalMsg.m_strMsg1 = " Column이 측정 위치로 움직입니다"; 
		else if(pView->m_nLanguage ==1)
		m_NormalMsg.m_strMsg1 = " Column will move to Calibration Position";
		else if(pView->m_nLanguage == 2)
		m_NormalMsg.m_strMsg1 = " 移動 Column到測量位置";
		break;
	case IDC_MANU_PTP_HEAD_SUPPLY: //중앙 공급 위치
		ThreadStage.PtpMoveCode='m';
		if(pView->m_nLanguage ==0)
		m_NormalMsg.m_strMsg1 = " Head s축이 액정 공급 위치로 움직입니다"; 
		else if(pView->m_nLanguage ==1)
		m_NormalMsg.m_strMsg1 = " Head S Axis will move to LC supply position";
		else if(pView->m_nLanguage == 2)
		m_NormalMsg.m_strMsg1 = " 移動Head s到液晶供給位置";
		break;
	case IDC_MANU_PTP_HEAD_CHANGE://2010.01.04 by tskim 반출 위치 R 이동으로 사용한다..
		ThreadStage.PtpMoveCode='n';
		if(pView->m_nLanguage ==0)
		m_NormalMsg.m_strMsg1 = " Column이 Rear Loading 위치로 움직입니다"; 
		else if(pView->m_nLanguage ==1)
		m_NormalMsg.m_strMsg1 = " Column will move to Rear Loading Position "; 
		else if(pView->m_nLanguage == 2)
		m_NormalMsg.m_strMsg1 = "  移動Column到Rear Loading位置"; 
		 break;
	case IDC_MANU_PTP_ALIGN://Mark#1,2 위치는 -1086,1086대칭으로 일단 본다..
		ThreadStage.PtpMoveCode='o';
		if(pView->m_nLanguage ==0)
		m_NormalMsg.m_strMsg1 = "Mark Align 정도 확인 위치로 움직입니다"; 
		else if(pView->m_nLanguage ==1)
		m_NormalMsg.m_strMsg1 = " Column will move to Mark Align Dgree Position ";
		else if(pView->m_nLanguage == 2)
		m_NormalMsg.m_strMsg1 = "移動Mark Align正確度位置";
		break;
	default:
		AfxMessageBox("**error**");
	}
	// by ckh 2009.02.12 구동 전 Stage의 Glass 흡착 상태를 확인 하도록 함.
	bIOResult1 = FALSE;	
/*	FAS_GetIoBit(2,true,STAGE_VACUUM_PRESS,&bIOResult1);
	Sleep(10);
	if(bIOResult1 == FALSE) // Vacuum On 일 경우 
	{
		if(pView->m_nLanguage ==0)
			m_NormalMsg.m_strMsg1+="\n\n Stage에 Glass가 흡착 상태입니다.!!! \n 주의 바랍니다. ";
		else if(pView->m_nLanguage ==1)
			m_NormalMsg.m_strMsg1+="\n\n  Glass was adsorbed on stage. Be Careful ";
		else if(pView->m_nLanguage == 2)
			m_NormalMsg.m_strMsg1+="\n\n Stage上 Glass被吸附狀態.!!! \n 請注意. ";
	}
*/	//
	m_NormalMsg.m_bTimer=TRUE;
	if(pView->m_nLanguage ==0)
	{
		m_NormalMsg.m_strTitle="PTP 구동";
		m_NormalMsg.m_strMsg2 = " 실행하려면 OK버튼을, 취소하려면 방향버튼을 누르시오.";
	}
	else if(pView->m_nLanguage ==1)
	{
		m_NormalMsg.m_strTitle="PTP Move";
		m_NormalMsg.m_strMsg2 = " if you Move 'OK' or Cancel please push 'Cancel' .";
	}
	else if(pView->m_nLanguage == 2)
	{
		m_NormalMsg.m_strTitle="PTP 驅動";
		m_NormalMsg.m_strMsg2 = " 如需執行點擊 'OK', 否則點擊CANCEL.";
	}
	m_NormalMsg.DoModal();   
}

void CManual::OnClickManuVacuumOn() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	m_ctrlStageVacOff.SetBackColor(WHITEGRAY);
	m_ctrlStageVacOn.SetBackColor(LIGHTGREEN);
	
	// by ckh 2009.02.12 퍼지 OFF 및 색깔 바꾸기
	m_ctrlStagePurge.SetBackColor(WHITEGRAY);
	pView->m_pDevice->Stage_Vacuum_Release_OnOff(OFF);

	if(PC_TYPE==TRUE) 	
	{
		if(pView->m_pDevice->Stage_Vacuum_On() == FALSE) 
		{ 
			m_ctrlStageVacOn.SetBackColor(WHITEGRAY);//by ckh 2009.02.16
			ThreadStage.ExitFlag = true;
			pView->PostMessage(WM_ERROR,52,NULL);
		}
		pView->m_pDevice->Stage_Vacuum_Release_OnOff(OFF);
	}
}

void CManual::OnClickManuVacuumOff() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	m_ctrlStageVacOff.SetBackColor(LIGHTGREEN);
	m_ctrlStageVacOn.SetBackColor(WHITEGRAY);
	m_ctrlStagePurge.SetBackColor(WHITEGRAY); // by ckh 2009.02.12

	if(PC_TYPE==TRUE) 
	{
		pView->m_pDevice->Stage_Vacuum_Release_OnOff(ON);
		Sleep(200);
		if(pView->m_pDevice->Stage_Vacuum_Off() == FALSE) 
		{ 
			pView->m_pDevice->Stage_Vacuum_Release_OnOff(OFF);
			ThreadStage.ExitFlag = true; 
			pView->PostMessage(WM_ERROR,50,NULL);
		}	
		pView->m_pDevice->Stage_Vacuum_Release_OnOff(OFF);
	}
}

// by ckh 2009.02.12 퍼지 추가
void CManual::OnClickManuPurgeOn() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	m_ctrlStageVacOff.SetBackColor(WHITEGRAY);
	m_ctrlStageVacOn.SetBackColor(WHITEGRAY);
	m_ctrlStagePurge.SetBackColor(LIGHTGREEN);
	if(PC_TYPE==TRUE) 
	{
		if(pView->m_pDevice->Stage_Vacuum_Release_OnOff(ON) == FALSE)
		{
			pView->m_pDevice->Stage_Vacuum_Release_OnOff(OFF);
			ThreadStage.ExitFlag = true; 
			pView->PostMessage(WM_ERROR,63,NULL);// 퍼지 압력 이상
			return;
		}
		Sleep(200);
		if(pView->m_pDevice->Stage_Vacuum_Off() == FALSE) 
		{ 
			pView->m_pDevice->Stage_Vacuum_Release_OnOff(OFF);
			ThreadStage.ExitFlag = true; 
			pView->PostMessage(WM_ERROR,50,NULL);// 퍼지 하였으나 진공이 풀리지 않았을 경우. 
			return;
		}
		
		//pView->m_pDevice->Stage_Vacuum_Release_OnOff(OFF);//by ckh 2009.02.16 막음
	}	
}

void CManual::OnClickCmdIo() 
{
	// TODO: Add your control notification handler code here
//////////////////////////////////////////////////20181130 jeongyong - KEY 상태 체크
#if AUTOKEY_INTERLOCK
	int nAutoKey = 0;
	FAS_GetIoBit(2,TRUE,AUTO_MODE,&nAutoKey);
	if(nAutoKey != TRUE)
	{
		AfxMessageBox("Change Key (Manual -> Auto)!!"); 
		Sleep(1000);
		return;
	}
#endif
	
	g_bManualSupply = FALSE;
	CManuIO dlg;
	dlg.DoModal();
}

void CManual::OnClickCmdReturn() 
{
	// by ckh 2009.02.12 Glass 흡착 여부 확인
//#if TRF
	if(GetStageVac() != TRUE)
		return;
//#endif
	KillTimer(0);
	g_bManualSupply = FALSE;
	EndDialog(IDOK);
}

void CManual::OnClickManuMove() 
{
	// by ckh 2009.02.12 Glass 흡착 여부 확인
	if(GetStageVac() != TRUE)
		return;
//////////////////////////////////////////////////20181130 jeongyong - KEY 상태 체크
#if AUTOKEY_INTERLOCK
	int nAutoKey = 0;
	FAS_GetIoBit(2,TRUE,AUTO_MODE,&nAutoKey);
	if(nAutoKey != TRUE)
	{
		AfxMessageBox("Change Key (Manual -> Auto)!!"); 
		Sleep(1000);
		return;
	}
#endif
	
	g_bManualSupply = FALSE;
	CManuOper dlg;
	dlg.DoModal();	
}

void CManual::OnClickManuWarmUp() 
{
	// by ckh 2009.02.12 Glass 흡착 여부 확인
	if(GetStageVac() != TRUE)
		return;
//////////////////////////////////////////////////20181130 jeongyong - KEY 상태 체크
#if AUTOKEY_INTERLOCK
	int nAutoKey = 0;
	FAS_GetIoBit(2,TRUE,AUTO_MODE,&nAutoKey);
	if(nAutoKey != TRUE)
	{
		AfxMessageBox("Change Key (Manual -> Auto)!!"); 
		Sleep(1000);
		return;
	}
#endif
	
	g_bManualSupply = FALSE;
	CManuWarmUp dlg;
	dlg.DoModal();	
}

void CManual::OnClickManuDrop() 
{
	// by ckh 2009.02.12 Glass 흡착 여부 확인
	if(GetStageVac() != TRUE)
		return;
//////////////////////////////////////////////////20181130 jeongyong - KEY 상태 체크
#if AUTOKEY_INTERLOCK
	int nAutoKey = 0;
	FAS_GetIoBit(2,TRUE,AUTO_MODE,&nAutoKey);
	if(nAutoKey != TRUE)
	{
		AfxMessageBox("Change Key (Manual -> Auto)!!"); 
		Sleep(1000);
		return;
	}
#endif

	g_bManualSupply = FALSE;
	CManuPatDrop dlg;
	dlg.DoModal();	
}

void CManual::OnClickManuDischarge() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	// by ckh 2009.02.12 Glass 흡착 여부 확인
	if(GetStageVac() != TRUE)
		return;
//////////////////////////////////////////////////20181130 jeongyong - KEY 상태 체크
#if AUTOKEY_INTERLOCK
	int nAutoKey = 0;
	FAS_GetIoBit(2,TRUE,AUTO_MODE,&nAutoKey);
	if(nAutoKey != TRUE)
	{
		AfxMessageBox("Change Key (Manual -> Auto)!!"); 
		Sleep(1000);
		return;
	}
#endif

#if EQ
	if (pDoc->m_structDataEditor.m_nNzlMode == 1)
	{
		for ( int nHeadCount = 0; nHeadCount < MAX_NOZZLE; nHeadCount++ )
		{
			if (pDoc->m_bIsHeadSelected[nHeadCount])
			{
				if (  pView->pN_Func->m_structNParam[nHeadCount].m_strModuleID == "" 
					||pView->pN_Func->m_structNParam[nHeadCount].m_strModuleID == "0000" )
				{
					if(pView->m_nLanguage == 0)
					AfxMessageBox("입력되지 않은 N Module ID 가 있습니다. 확인 바랍니다.!!"); 

					else if(pView->m_nLanguage == 1)
					AfxMessageBox("There are N Module ID to be not input. Please Check.!!"); 

					else if(pView->m_nLanguage == 2)
					AfxMessageBox("有無輸入 N Module ID.請確認.!!"); 
					
					return; 
				}
			}
		}
	}
#endif
	g_bManualSupply = FALSE;
	CAdjust dlg;
	dlg.DoModal();	
}

void CManual::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	BOOL io_Door1 = 0, io_Door2 = 0, io_Door3 = 0, io_Door0 = 0, io_Robot_Arm = 0,io_Glass =0;
	BOOL io_EMO1 = 0, io_EMO2 = 0, io_EMO3 = 0, io_EMO4 = 0, io_EMO5 = 0, io_EMO6 =0, io_EMO7 =0;


	DWORD dwIOResultF=0;
	BOOL bIOResult=FALSE, bMiniDoorSafetyOK=FALSE;

	BOOL bIOResult1=FALSE;
	BOOL bIOResult2=FALSE;
	BOOL bIOResult3=FALSE;
	BOOL bIOResult4=FALSE;


	BOOL bMutingON = FALSE;//jykim 170626 MutingON 변수 추가
	FAS_GetIoBit(2,TRUE,MUTING_ON,&bMutingON); //JYKIM 170626 뮤팅위치 상시 체크	
	FAS_GetIoBit(1,false,DOOR_SENSOR1,&bIOResult1);	
	FAS_GetIoBit(1,false,DOOR_SENSOR2,&bIOResult2);	
	FAS_GetIoBit(1,false,DOOR_SENSOR3,&bIOResult3);	
	FAS_GetIoBit(1,false,MAINT_DOOR,&bIOResult4);	
	FAS_GetIoBit(1,false,ROBOT_ARM_DETECT,&bIOResult);	// Robot Arm Check
//	if(ThreadStage.bMachineRunning == TRUE)
//	if((ThreadStage.bMachineRunning == TRUE||!bMutingON)&&!bDoorAlarm) //JYKIM 170626 뮤팅위치 아니면 또는 구동중이면 도어확인 후 알람(알람은 한 번만 뜨게 함)
/*	if((ThreadStage.bMachineRunning == TRUE)&&!bDoorAlarm) //20190718 JEONGYONG - 뮤팅 조건 삭제
	{
		FAS_GetIo(1,false,&dwIOResultF);
		if(((bIOResult1 == TRUE)||(bIOResult2 == TRUE)||(bIOResult3 == TRUE)||(bIOResult4 == TRUE))&&!bMutingON) //jykim 170626 Manual에선 뮤팅위치 아닐때만 도어 체크
		{
			bDoorAlarm=TRUE; //jykim 170626 도어알람변수 살림

			if(ThreadStage.bUseInterLock==TRUE)
			{
				ThreadStage.bEmergencyStop=TRUE; ThreadStage.ExitFlag = TRUE; ThreadStage.nHomeEnableCount = 0;  //현찬 : 문을 열었다고 해서 원점을 다시 잡게 하면 문제가 있을 수도 있을 거라 생각함.
				pView->m_pDevice->Emergency_Stop();
				if(bDoorAlarm==TRUE)
				{
///////////////////////////////////////////////////////////////20180830 jeongyong - Door No 추가
					int nDoorNo= 0;
					if(bIOResult1==TRUE) nDoorNo=1;
					else if(bIOResult2==TRUE) nDoorNo=2;			
					else if(bIOResult3==TRUE) nDoorNo=3;
					else if(bIOResult4==TRUE) nDoorNo=4;

					pView->SendMessage(WM_ERROR,101,nDoorNo);
					bDoorAlarm=FALSE; //jykim 170626 도어알람변수 OFF
				}
			}
		}
		else if(bIOResult==TRUE)
		{
			ThreadStage.bEmergencyStop=TRUE; ThreadStage.ExitFlag = TRUE; ThreadStage.nHomeEnableCount = 0;
			pView->m_pDevice->Emergency_Stop();
			pView->SendMessage(WM_ERROR,104,NULL);
		}
	}
*/
//////////////////////////////////////////////////////////////////////JYKIM 170327 EMO 모니터링추가
	FAS_GetIoBit(4,true,EMO1,&io_EMO1);
	FAS_GetIoBit(4,true,EMO2,&io_EMO2);
	FAS_GetIoBit(4,true,EMO3,&io_EMO3);
	FAS_GetIoBit(4,true,EMO4,&io_EMO4);
	FAS_GetIoBit(4,true,EMO5,&io_EMO5);
	FAS_GetIoBit(4,true,EMO6,&io_EMO6);
	FAS_GetIoBit(4,true,EMO7,&io_EMO7);


	if(io_EMO1==TRUE)//by ckh 2009.02.16
	{
		m_ctrlSenEMO1.SetBackColor(RED);
		m_ctrlSenEMO1.SetCaption("EMO1 Push");	
	}
	else
	{
		m_ctrlSenEMO1.SetBackColor(WHITE);	
		m_ctrlSenEMO1.SetCaption("EMO1");
	}

	FAS_GetIoBit(1,false,GLASS_DETECT,&io_Glass);
	if(bIOResult4==TRUE)//by ckh 2009.02.16
	{
		m_ctrlSenDoor0.SetBackColor(RED);
		m_ctrlSenDoor0.SetCaption("Maint Door Open");	
	}
	else
	{
		m_ctrlSenDoor0.SetBackColor(WHITE);	//by ckh 2009.02.16
		m_ctrlSenDoor0.SetCaption("Maint Door Close");
	}

	if(bIOResult1==TRUE)//by ckh 2009.02.16
	{
		m_ctrlSenDoor1.SetBackColor(RED);
		m_ctrlSenDoor1.SetCaption("Door 1 Open");	
	}
	else
	{
		m_ctrlSenDoor1.SetBackColor(WHITE);	
		m_ctrlSenDoor1.SetCaption("Door 1 Close");
	}

	if(bIOResult2==TRUE)//by ckh 2009.02.16
	{
		m_ctrlSenDoor2.SetBackColor(RED);
		m_ctrlSenDoor2.SetCaption("Door 2 Close");	
	}
	else
	{
		m_ctrlSenDoor2.SetBackColor(WHITE);	
		m_ctrlSenDoor2.SetCaption("Door 2 Close");
	}

	if(bIOResult3==TRUE)//by ckh 2009.02.16
	{
		m_ctrlSenDoor3.SetBackColor(RED);
		m_ctrlSenDoor3.SetCaption("Door 3 Open");	
	}
	else
	{
		m_ctrlSenDoor3.SetBackColor(WHITE);	
		m_ctrlSenDoor3.SetCaption("DOOR 3 Close");
	}

	if(io_Robot_Arm==TRUE)
	{
		m_ctrlSenRobot.SetBackColor(RED);
		m_ctrlSenRobot.SetCaption("Robot Arm Detect ");	
	}
	else
	{
		m_ctrlSenRobot.SetBackColor(WHITE);	//by ckh 2009.02.16
		m_ctrlSenRobot.SetCaption("Robot Arm");
	}

	if(io_Glass == TRUE) 
	{
		pView->m_pDevice->MNET_BitSet(DISP_GLS_EXIST, true);		//ehji 150414
		m_ctrlSenGlass.SetBackColor(LIGHTBLUE);
		m_ctrlSenGlass.SetCaption("Glass 有 ");	
	}
	else
	{
		pView->m_pDevice->MNET_BitSet(DISP_GLS_EXIST, false);		//ehji 150414
		m_ctrlSenGlass.SetBackColor(WHITE);	//by ckh 2009.02.16
		m_ctrlSenGlass.SetCaption("Glass 無");
	}
	UpdateData(FALSE);
	CDialog::OnTimer(nIDEvent);
}

void CManual::OnClickIonizerOn() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	int IoinizerStatus=0;

	m_ctrlIonizerON.SetBackColor(LIGHTGREEN);
	m_ctrlIonizerOFF.SetBackColor(WHITEGRAY);

	if(pView->m_pDevice->Ionizer_Sol_On() == TRUE)
	{
		Sleep(1000);
		IoinizerStatus=pView->m_pDevice->Ionizer_Status_Check();
		if(IoinizerStatus!=PASS)
		{
			pView->m_pDevice->Ionizer_Sol_Off();
			pView->PostMessage(WM_ERROR,53,IoinizerStatus);
			pView->m_pDevice->MNET_WriteWord(DISP_WORD+0x06,"0000",1); 
			ThreadStage.ExitFlag = true;
		}
 		else	
 		{
// 			//Ionizer Status ON
// 			//Ionizer가 1개이므로, (0번째 Not Use)->LC는 2개이므로.."0006"
 			pView->m_pDevice->MNET_WriteWord(DISP_WORD+0x06,"0006",1); 
 		}
	}
	else 
	{ 
		pView->m_pDevice->Ionizer_Sol_Off();
		pView->m_pDevice->MNET_WriteWord(DISP_WORD+0x06,"0000",1); 
		pView->PostMessage(WM_ERROR,51,NULL);
		ThreadStage.ExitFlag = true;
	}	
}

void CManual::OnClickIonizerOff() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

//	if(pDoc->m_bIonizerAlwaysON==FALSE)
//	{
		m_ctrlIonizerON.SetBackColor(WHITEGRAY);
		m_ctrlIonizerOFF.SetBackColor(LIGHTGREEN);
		pView->m_pDevice->Ionizer_Sol_Off();
		pView->m_pDevice->MNET_WriteWord(DISP_WORD+0x06,"0000",1); 		
//	}	
}

void CManual::OnClickCda1() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	//by shin//2009.06.16//
	BOOL bIoResult = false;
	int i = 0;
	
	if(g_nSupplyHead == 100) return;

	if(m_bCDA1 == false)
    {
//2011.03.23 by tskim CDA ON 전 Pinch Valve 열려 있는 경우 알람 발생 
		//Pinch Close Check
		for(i = 0; i < 50; i++) //0.5초 동안 검사한다...//
		{
			FAS_GetIoBit(3, FALSE, PINCH_CLOSE1,&bIoResult);
			if(bIoResult == TRUE) 
			{
				break;
			}
			Sleep(10);
		}
		if(i>=49)
		{
			//Pinch Valve Close
			pView->PostMessage(WM_ERROR,147,1);
			return;
		}

		FAS_SetIoBit(3, false, SUPPLY_CDA_SOL1,true);
		m_ctrlCDA1.SetCaption("CDA1 ON");
		m_bCDA1 = true;
//		Sleep(5000);

		for(i = 0; i < 1000; i++) //10초 동안 검사한다...//
		{
			FAS_GetIoBit(3, FALSE, SUPPLY_CDA1,&bIoResult);

			if(bIoResult == TRUE) 
			{
				break;
			}
			Sleep(10);
		}

		if(i>=999)
		{
			FAS_SetIoBit(3, false, SUPPLY_CDA_SOL1,false);
			m_ctrlCDA1.SetCaption("CDA1 OFF");
			m_bCDA1 = false;
			pView->PostMessage(WM_ERROR,148,1);
		}
	}
	else
	{
		FAS_SetIoBit(3, false, SUPPLY_CDA_SOL1,false);
		m_ctrlCDA1.SetCaption("CDA1 OFF");
		m_bCDA1 = false;
	}	
}

void CManual::OnClickCda2() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	//by shin//2009.06.16//

	BOOL bIoResult = false;
	int i = 0;

	if(g_nSupplyHead == 100) return;
	if(m_bCDA2 == false)
    {
//2011.03.23 by tskim CDA ON 전 Pinch Valve 열려 있는 경우 알람 발생 
		//Pinch Close Check
		for(i = 0; i < 50; i++) //0.5초 동안 검사한다...//
		{
			FAS_GetIoBit(3, FALSE, PINCH_CLOSE2,&bIoResult);
			if(bIoResult == TRUE) 
			{
				break;
			}
			Sleep(10);
		}
		if(i>=49)
		{
			//Pinch Valve Close
			pView->PostMessage(WM_ERROR,147,2);
			return;
		}	
		
		FAS_SetIoBit(3, false, SUPPLY_CDA_SOL2,true);
		m_ctrlCDA2.SetCaption("CDA2 ON");
		m_bCDA2 = true;

//		Sleep(3500);

		for(i = 0; i < 1000; i++) //10초 동안 검사한다...//
		{
			FAS_GetIoBit(3, FALSE, SUPPLY_CDA2,&bIoResult);

			if(bIoResult == TRUE) 
			{
				break;
			}
			Sleep(10);
		}
		if(i>=999)
		{
			FAS_SetIoBit(3, false, SUPPLY_CDA_SOL2,false);
			m_ctrlCDA2.SetCaption("CDA2 OFF");
			m_bCDA2 = false;
			pView->PostMessage(WM_ERROR,148,2);
		}
	}
	else
	{
		FAS_SetIoBit(3, false, SUPPLY_CDA_SOL2,false);
		m_ctrlCDA2.SetCaption("CDA2 OFF");
		m_bCDA2 = false;
	}
}

void CManual::OnClickPinch1() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	//by shin//2009.06.16//

	BOOL bIoResult = false;
	int i = 0;
	
	if(g_nSupplyHead == 100) return;
	if(m_bPinch1 == false)
    {
		FAS_SetIoBit(3, false, PINCH_VALVE_SOL1,true);
		m_ctrlPinch1.SetCaption("Pinch1 Open");
		m_bPinch1 = true;
//		Sleep(500);
		
		for(i = 0; i < 50; i++) //0.5초 동안 검사한다...//
		{
			FAS_GetIoBit(3, FALSE, PINCH_OPEN1,&bIoResult);
			if(bIoResult == TRUE) 
			{
				break;
			}
			Sleep(10);
		}

		if(i>=49)
		{
			FAS_SetIoBit(3, false, PINCH_VALVE_SOL1,false);
			m_ctrlPinch1.SetCaption("Pinch1 Close");
			m_bPinch1 = false;
			pView->PostMessage(WM_ERROR,147,1);
		}
	}
	else
	{
		FAS_SetIoBit(3, false, PINCH_VALVE_SOL1,false);
		m_ctrlPinch1.SetCaption("Pinch1 Close");
		m_bPinch1 = false;
	}
}

void CManual::OnClickPinch2() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	//by shin//2009.06.16//

	BOOL bIoResult = false;
	int i = 0;

	if(g_nSupplyHead == 100) return;
	if(m_bPinch2 == false)
    {
		FAS_SetIoBit(3, false, PINCH_VALVE_SOL2,true);
		m_ctrlPinch2.SetCaption("Pinch2 Open");
		m_bPinch2 = true;

//		Sleep(500);

		for(i = 0; i < 50; i++) //0.5초 동안 검사한다...//
		{
			FAS_GetIoBit(3, FALSE, PINCH_OPEN2,&bIoResult);
			if(bIoResult == TRUE) 
			{
				break;
			}
			Sleep(10);
		}

		if(i>=49)
		{
			FAS_SetIoBit(3, false, PINCH_VALVE_SOL2,false);
			m_ctrlPinch2.SetCaption("Pinch2 Close");
			m_bPinch2 = false;
			pView->PostMessage(WM_ERROR,147,2);
		}
	}
	else
	{
		FAS_SetIoBit(3, false, PINCH_VALVE_SOL2,false);
		m_ctrlPinch2.SetCaption("Pinch2 Close");
		m_bPinch2 = false;
	}			
}

void CManual::OnClickSupplyNozzle1() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	//by shin//2009.06.16//

	BOOL bIoResult = false;
	int i = 0;

	if(g_nSupplyHead == 100) return;
	if(m_bNozzleForword1 == false)
    {
		FAS_SetIoBit(3, false, NOZZLE_MOVE_SOL1,true);
		m_ctrlNozzleForword1.SetCaption("Nozzle1  전진");
		m_bNozzleForword1 = true;
//		Sleep(2000);

		for(i = 0; i < 200; i++) //2초 동안 검사한다...//
		{
			FAS_GetIoBit(3, FALSE, NOZZLE_FORWARD1,&bIoResult);
			if(bIoResult == TRUE) 
			{
				break;
			}
			Sleep(10);
		}

		if(i>=199)
		{
			FAS_SetIoBit(3, false, NOZZLE_MOVE_SOL1,false);
			m_ctrlNozzleForword1.SetCaption("Nozzle1  후진");
			m_bNozzleForword1 = false;
			pView->PostMessage(WM_ERROR,146,1);
		}
	}
	else
	{
		FAS_SetIoBit(3, false, NOZZLE_MOVE_SOL1,false);
		m_ctrlNozzleForword1.SetCaption("Nozzle1  후진");
		m_bNozzleForword1 = false;
	}				
}

void CManual::OnClickSupplyNozzle2() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	//by shin//2009.06.16//

	BOOL bIoResult = false;
	int i = 0;

	if(g_nSupplyHead == 100) return;

	if(m_bNozzleForword2 == false)
    {
		FAS_SetIoBit(3, false, NOZZLE_MOVE_SOL2,true);
		m_ctrlNozzleForword2.SetCaption("Nozzle2  전진");
		m_bNozzleForword2 = true;
//		Sleep(2000);

		for(i = 0; i < 200; i++) //2초 동안 검사한다...//
		{
			FAS_GetIoBit(3, FALSE, NOZZLE_FORWARD2,&bIoResult);
			if(bIoResult == TRUE) 
			{
				break;
			}
			Sleep(10);
		}

		if(i>=199)
		{
			FAS_SetIoBit(3, false, NOZZLE_MOVE_SOL2,false);
			m_ctrlNozzleForword2.SetCaption("Nozzle2  후진");
			m_bNozzleForword2 = false;
			pView->PostMessage(WM_ERROR,146,2);
		}
	}
	else
	{
		FAS_SetIoBit(3, false, NOZZLE_MOVE_SOL2,false);
		m_ctrlNozzleForword2.SetCaption("Nozzle2  후진");
		m_bNozzleForword2 = false;
	}					
}

void CManual::OnClickManuMeasurePos() 
{
//	return;
	// by ckh 2009.02.12 Glass 흡착 여부 확인
	if(GetStageVac() != TRUE)
		return;
	
	g_bManualSupply = FALSE;
	CManuMeasWidth dlg;
	dlg.DoModal();		
}


void CManual::OnClickSpyHead() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	//임시...//
	if(g_nSupplyHead == 100)
	{
		g_nSupplyHead = 0;
		m_ctrlSpyHead.SetCaption("HEAD 1");
	}
	else if(g_nSupplyHead == 0)
	{
		g_nSupplyHead = 1;
		m_ctrlSpyHead.SetCaption("HEAD 2");
	}
	else if(g_nSupplyHead == 1)
	{
		g_nSupplyHead = 2;
		m_ctrlSpyHead.SetCaption("HEAD 3");
	}
	else if(g_nSupplyHead == 2)
	{
		g_nSupplyHead = 3;
		m_ctrlSpyHead.SetCaption("HEAD 4");
	}
	else if(g_nSupplyHead == 3)
	{
		g_nSupplyHead = 4;
		m_ctrlSpyHead.SetCaption("HEAD 5");
	}
	else if(g_nSupplyHead == 4)
	{
		g_nSupplyHead = 5;
		m_ctrlSpyHead.SetCaption("HEAD 6");
	}
	else if(g_nSupplyHead == 5)
	{
		g_nSupplyHead = 6;
		m_ctrlSpyHead.SetCaption("HEAD 7");
	}
	else if(g_nSupplyHead == 6)
	{
		g_nSupplyHead = 7;
		m_ctrlSpyHead.SetCaption("HEAD 8");
	}
	else if(g_nSupplyHead == 7)
	{
		g_nSupplyHead = 8;
		m_ctrlSpyHead.SetCaption("HEAD 9");
	}
	else if(g_nSupplyHead == 8)
	{
		g_nSupplyHead = 9;
		m_ctrlSpyHead.SetCaption("HEAD 10");
	}
	else if(g_nSupplyHead == 9)
	{
		g_nSupplyHead = 10;
		m_ctrlSpyHead.SetCaption("HEAD 11");
	}
	else if(g_nSupplyHead == 10)
	{
		g_nSupplyHead = 11;
		m_ctrlSpyHead.SetCaption("HEAD 12");
	}
	else if(g_nSupplyHead == 11)
	{
		g_nSupplyHead = 12;
		m_ctrlSpyHead.SetCaption("HEAD 13");
	}
	else if(g_nSupplyHead == 12)
	{
		g_nSupplyHead = 13;
		m_ctrlSpyHead.SetCaption("HEAD 14");
	}
	else if(g_nSupplyHead == 13)
	{
		g_nSupplyHead = 14;
		m_ctrlSpyHead.SetCaption("HEAD 15");
	}
	else if(g_nSupplyHead == 14)
	{
		g_nSupplyHead = 15;
		m_ctrlSpyHead.SetCaption("HEAD 16");
	}
	else if(g_nSupplyHead == 15)
	{
		g_nSupplyHead = 100;
		if(pView->m_nLanguage == 0)
		m_ctrlSpyHead.SetCaption("사용안함");
		else if(pView->m_nLanguage == 1)
		m_ctrlSpyHead.SetCaption("Don't USE");
		else if(pView->m_nLanguage == 2)
		m_ctrlSpyHead.SetCaption("不適用");
	}
}


void CManual::OnDblClickLabelLcSupplyGroup(LPDISPATCH Cancel) 
{
	// TODO: Add your control notification handler code here
	//임시...
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	if(g_nSupplyHead != 100)
	{
		if(pDoc->m_bIsEngineerMode==FALSE) //by shin//2009.06.16//
		{
			m_ctrlPinch1.SetEnabled(true);
			m_ctrlPinch2.SetEnabled(true);
		}
		else
		{
			m_ctrlCDA1.SetEnabled(true);
			m_ctrlCDA2.SetEnabled(true);
			m_ctrlPinch1.SetEnabled(true);
			m_ctrlPinch2.SetEnabled(true);
			m_ctrlNozzleForword1.SetEnabled(true);
			m_ctrlNozzleForword2.SetEnabled(true);
		}
		g_bManualSupply = TRUE;
	}
}

// by ckh 2009.02.12 Stage의 Glass 흡착 상태 확인 
BOOL CManual::GetStageVac()
{
	return TRUE;
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
		// by ckh 2009.02.12 구동 전 Stage의 Glass 흡착 상태를 확인 하도록 함.
// 	BOOL bIOResult1 = FALSE,bIOResult2 = FALSE,bIOResult3 = FALSE;
// 	FAS_GetIoBit(2,true,STAGE_VACUUM_PRESS,&bIOResult1);
// 	FAS_GetIoBit(2,true,STAGE_PURGE_PRESS1,&bIOResult2);
// 	FAS_GetIoBit(2,true,STAGE_PURGE_PRESS2,&bIOResult3);
//#if TRF
	int nResult = pView->m_pDevice->GetStageVac_Purge();
	Sleep(10);
	if(nResult == 1)
	{
		if(pView->m_nLanguage == 0)
		{
			m_NormalMsg.m_strMsg1="Stage에 Glass가 흡착 상태입니다.!!! \n 흡착 Off 후 실행바랍니다.";	
		}
		else if(pView->m_nLanguage == 1)
		{
			m_NormalMsg.m_strMsg1="Glass is Adhesion state on Stage .!!! \n After adhesion Off, Please Start.";	
		}
		else if(pView->m_nLanguage == 2) // 중국어 
		{
			m_NormalMsg.m_strMsg1="Stage上 Glass被吸附狀態.!!! \n 吸附 Off 后執行..";	
		}
	}
	else if(nResult == 2)
	{
		if(pView->m_nLanguage == 0)
		{
			m_NormalMsg.m_strMsg1="Stage 퍼지 상태입니다.!!! \n 퍼지 Off 후 실행바랍니다.";	
		}
		else if(pView->m_nLanguage == 1)
		{
			m_NormalMsg.m_strMsg1="Machine is Stage purge State.!!! \n After purge Off, Please Start.";	
		}
		else if(pView->m_nLanguage == 2) // 중국어 
		{
			m_NormalMsg.m_strMsg1="Stage Fuzzy狀態.!!! \n Fuzzy Off后再執行.";	
		}
		
	}
	else if(nResult == 3)
	{
		if(pView->m_nLanguage == 0)
		{
			m_NormalMsg.m_strMsg1="Stage 퍼지 및 흡착 센서 이상 입니다.!!! \n  Stage 공압 센서 확인 후 실행 바랍니다.";	
		}
		else if(pView->m_nLanguage == 1)
		{
			m_NormalMsg.m_strMsg1="Stage purge And adhesion Sensor is abnormal..!!! \n  After Stage pneumatic(air) Sensor Check, Plase start";	
		}
		else if(pView->m_nLanguage == 2) // 중국어 
		{
			m_NormalMsg.m_strMsg1="Stage Fuzzy及吸附感應器異常.!!! \n  Stage空壓感應器確認後再執行.";	
		}
	}
	else
	{
		return TRUE;
	}
	//
	m_NormalMsg.m_bTimer=FALSE;

	if(pView->m_nLanguage == 0)
	{
		m_NormalMsg.m_strTitle = _T("확인");
		m_NormalMsg.m_strMsg2 = "";//" 실행하려면 OK버튼이나 방향버튼을 누르시오.";	
	}
	
	else if(pView->m_nLanguage == 1)
	{
		m_NormalMsg.m_strTitle = _T("Check");
		m_NormalMsg.m_strMsg2 = "";//" 실행하려면 OK버튼이나 방향버튼을 누르시오.";
	}

	else if(pView->m_nLanguage == 2)
	{
		m_NormalMsg.m_strTitle = _T("確認");
		m_NormalMsg.m_strMsg2 = "";//" 실행하려면 OK버튼이나 방향버튼을 누르시오.";	
	}

	m_NormalMsg.DoModal();   

//#else 
//	return TRUE;
//#endif
	return FALSE;
}

void CManual::OnClickManuMarkAlign() 
{
//	return;
	CMarkAlignInsp dlg;
	dlg.DoModal();
}

void CManual::OnClickManuVision() 
{
//////////////////////////////////////////////////20181130 jeongyong - KEY 상태 체크
#if AUTOKEY_INTERLOCK
	int nAutoKey = 0;
	FAS_GetIoBit(2,TRUE,AUTO_MODE,&nAutoKey);
	if(nAutoKey != TRUE)
	{
		AfxMessageBox("Change Key (Manual -> Auto)!!"); 
		Sleep(1000);
		return;
	}
#endif
	
	CManuVisionWings dlg; 
	dlg.DoModal();	
}

void CManual::SelectLanguage()	//ehji 140309
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();


	if(pView->m_nLanguage == 0) //ehji 140309 .
	{
		//Manu
		SetDlgItemText(IDC_MANU_MOVE,			_T("수동 운전"));
		SetDlgItemText(IDC_MANU_WARM_UP,			_T("Warm-Up"));
		SetDlgItemText(IDC_MANU_DROP,			_T("Manual Drop"));
		SetDlgItemText(IDC_MANU_DISCHARGE,			_T("측정 및 조정"));
//		SetDlgItemText(IDC_MANU_MARK_ALIGN,			_T("Mark Align 정도 확인"));
		SetDlgItemText(IDC_MANU_MEASURE_POS,			_T("Motor 위치 확인"));
		SetDlgItemText(IDC_MANU_VISION,			_T("Vision Set"));
		SetDlgItemText(IDC_CMD_IO,			_T("I/O 체크"));	


		//COLUMN PTP구동
		SetDlgItemText(IDC_LABEL_CAP_STAGEPTP_GROUP,			_T("COLUMN PTP구동"));	
		SetDlgItemText(IDC_MANU_PTP_OPER,			_T("작업자 위치"));	
		SetDlgItemText(IDC_MANU_PTP_CENT,			_T("중심 위치"));	
		SetDlgItemText(IDC_MANU_PTP_BOTCHG,			_T("액정 공급 위치"));	
		SetDlgItemText(IDC_MANU_PTP_HOME,			_T("Dummy Cup교환 위치"));	
		SetDlgItemText(IDC_MANU_PTP_DUMMY,			_T("dummy토출 위치"));	
		SetDlgItemText(IDC_MANU_PTP_LOAD,			_T("반출입 위치(F)"));	
		SetDlgItemText(IDC_MANU_PTP_HEAD_CHANGE,			_T("반출입 위치(R)"));
		SetDlgItemText(IDC_MANU_PTP_COL_MEA,			_T("Column 측정 위치"));
		SetDlgItemText(IDC_MANU_PTP_ALIGN,			_T("ALIGN 정도 확인 위치"));
		
		//	HEAD S PTP구동
		SetDlgItemText(IDC_LABEL_CAP_HEADPTP_GROUP,			_T("HEAD S PTP구동"));
		SetDlgItemText(IDC_MANU_PTP_HEAD_WAIT,			_T("Head dummy 위치"));
		SetDlgItemText(IDC_MANU_PTP_HEAD_MEAS,			_T("Head 측정 위치"));
		SetDlgItemText(IDC_MANU_PTP_HEAD_JOB,			_T("Head 작업 위치"));
		SetDlgItemText(IDC_MANU_PTP_HEAD_SUPPLY,			_T("Head 액정 공급 위치"));

		//STAGE VAC.
		SetDlgItemText(IDC_LABEL_CAP_HEADPTP_GROUP2,			_T("STAGE VAC."));
		SetDlgItemText(IDC_MANU_VACUUM_OFF,			_T("Stage 퍼지    OFF"));
		SetDlgItemText(IDC_MANU_PURGE_ON,			_T("Stage 퍼지     ON"));

		// IONIZER CHECK
		SetDlgItemText(IDC_LABEL_CAP_HEADPTP_GROUP3,			_T(" IONIZER CHECK"));
		SetDlgItemText(IDC_IONIZER_ON,			_T("이오나이져         ON"));
		SetDlgItemText(IDC_IONIZER_OFF,			_T("이오나이져       OFF"));

		//액정 공급 관련
		SetDlgItemText(IDC_LABEL_LC_SUPPLY_GROUP, _T("액정 공급 관련"));
		SetDlgItemText(IDC_LABEL2,			_T("공급 Head 선택"));
		SetDlgItemText(IDC_CDA2,			_T("CDA2 ON"));
		SetDlgItemText(IDC_PINCH2,			_T("PINCH2 OPEN"));
		SetDlgItemText(IDC_SUPPLY_NOZZLE2,			_T("NOZZLE2    전진"));
		SetDlgItemText(IDC_SPY_HEAD,			_T("HEAD 1"));
		SetDlgItemText(IDC_CDA1,			_T("CDA1 ON"));
		SetDlgItemText(IDC_PINCH1,			_T("PINCH1 OPEN"));
		SetDlgItemText(IDC_SUPPLY_NOZZLE1,			_T("NOZZLE1      전진"));

		//이전 화면
		SetDlgItemText(IDC_CMD_RETURN,			_T("이전 화면"));


	}
	
	else if(pView->m_nLanguage == 1)
	{
		//Manu
		SetDlgItemText(IDC_LABEL_LC_SUPPLY_GROUP, _T("LC Supply Relation"));
		SetDlgItemText(IDC_MANU_MOVE,			_T("Manual Operation"));
		SetDlgItemText(IDC_MANU_WARM_UP,			_T("Warm-Up"));
		SetDlgItemText(IDC_MANU_DROP,			_T("Manual Drop"));
		SetDlgItemText(IDC_MANU_DISCHARGE,			_T("Adjust & Calibration"));
//		SetDlgItemText(IDC_MANU_MARK_ALIGN,			_T("Mark Align Level Check"));
		SetDlgItemText(IDC_MANU_MEASURE_POS,			_T("Motor position Check"));
		SetDlgItemText(IDC_MANU_VISION,			_T("Vision Set"));
		SetDlgItemText(IDC_CMD_IO,			_T("I/O Check"));	

			//COLUMN PTP구동
		SetDlgItemText(IDC_LABEL_CAP_STAGEPTP_GROUP,			_T("COLUMN PTP Operation"));	
		SetDlgItemText(IDC_MANU_PTP_OPER,			_T("Operater POS."));	
		SetDlgItemText(IDC_MANU_PTP_CENT,			_T("Center POS"));	
		SetDlgItemText(IDC_MANU_PTP_BOTCHG,			_T("LC Supply POS"));	
		SetDlgItemText(IDC_MANU_PTP_HOME,			_T("Dummy Cup Change POS"));	
		SetDlgItemText(IDC_MANU_PTP_DUMMY,			_T("Dummy Dispensing POS"));	
		SetDlgItemText(IDC_MANU_PTP_LOAD,			_T("Load POS(F)"));	
		SetDlgItemText(IDC_MANU_PTP_HEAD_CHANGE,			_T("Load POS(R)"));
		SetDlgItemText(IDC_MANU_PTP_COL_MEA,			_T("Column measurement POS"));
		SetDlgItemText(IDC_MANU_PTP_ALIGN,			_T("ALIGN Level Check POS"));

			//	HEAD S PTP구동
		SetDlgItemText(IDC_LABEL_CAP_HEADPTP_GROUP,			_T("HEAD S PTP POS "));
		SetDlgItemText(IDC_MANU_PTP_HEAD_WAIT,			_T("Head dummy POS"));
		SetDlgItemText(IDC_MANU_PTP_HEAD_MEAS,			_T("Head Measurement POS"));
		SetDlgItemText(IDC_MANU_PTP_HEAD_JOB,			_T("Head work POS"));
		SetDlgItemText(IDC_MANU_PTP_HEAD_SUPPLY,			_T("Head Supply POS "));

		//STAGE VAC.
		SetDlgItemText(IDC_LABEL_CAP_HEADPTP_GROUP2,			_T("STAGE VAC."));
		SetDlgItemText(IDC_MANU_VACUUM_OFF,			_T("Stage fuzzy    OFF"));
		SetDlgItemText(IDC_MANU_PURGE_ON,			_T("Stage fuzzy    ON"));

		// IONIZER CHECK
		SetDlgItemText(IDC_LABEL_CAP_HEADPTP_GROUP3,			_T(" IONIZER CHECK"));
		SetDlgItemText(IDC_IONIZER_ON,			_T("IONIZER ON"));
		SetDlgItemText(IDC_IONIZER_OFF,			_T("IONIZER OFF"));

		//액정 공급 관련
		SetDlgItemText(IDC_LABEL2,			_T("Supply Head Select"));
		SetDlgItemText(IDC_CDA2,			_T("CDA2 ON"));
		SetDlgItemText(IDC_PINCH2,			_T("PINCH2 OPEN"));
		SetDlgItemText(IDC_SUPPLY_NOZZLE2,			_T("NOZZLE2    Forward"));
		SetDlgItemText(IDC_SPY_HEAD,			_T("HEAD 1"));
		SetDlgItemText(IDC_CDA1,			_T("CDA1 ON"));
		SetDlgItemText(IDC_PINCH1,			_T("PINCH1 OPEN"));
		SetDlgItemText(IDC_SUPPLY_NOZZLE1,			_T("NOZZLE1      Forward"));

		//이전 화면
		SetDlgItemText(IDC_CMD_RETURN,			_T("Return"));
	}
	
	else if(pView->m_nLanguage == 2) // ehji 중국어 작업 필요.
	{
		//manu
		SetDlgItemText(IDC_LABEL_LC_SUPPLY_GROUP, _T("液晶供給關聯"));
		SetDlgItemText(IDC_MANU_MOVE,			_T("手動運轉"));
		SetDlgItemText(IDC_MANU_WARM_UP,			_T("Warm-Up"));
		SetDlgItemText(IDC_MANU_DROP,			_T("Manual Drop"));
		SetDlgItemText(IDC_MANU_DISCHARGE,			_T("測定及調整"));
//		SetDlgItemText(IDC_MANU_MARK_ALIGN,			_T("Mark Align 精度確認"));
		SetDlgItemText(IDC_MANU_MEASURE_POS,			_T("Motor 位置確認"));
		SetDlgItemText(IDC_MANU_VISION,			_T("Vision Set"));
		SetDlgItemText(IDC_CMD_IO,			_T("I/O Check"));
		
		//COLUMN PTP구동
		SetDlgItemText(IDC_LABEL_CAP_STAGEPTP_GROUP,			_T("COLUMN PTP驅動"));	
		SetDlgItemText(IDC_MANU_PTP_OPER,			_T("作業者位置"));	
		SetDlgItemText(IDC_MANU_PTP_CENT,			_T("中心位置"));	
		SetDlgItemText(IDC_MANU_PTP_BOTCHG,			_T("液晶供給位置"));	
		SetDlgItemText(IDC_MANU_PTP_HOME,			_T("Dummy Cup交換位置"));	
		SetDlgItemText(IDC_MANU_PTP_DUMMY,			_T("dummy吐出位置"));	
		SetDlgItemText(IDC_MANU_PTP_LOAD,			_T("搬出入位置(F)"));	
		SetDlgItemText(IDC_MANU_PTP_HEAD_CHANGE,			_T("搬出入位置(R)"));
		SetDlgItemText(IDC_MANU_PTP_COL_MEA,			_T("Column 測定位置"));
		SetDlgItemText(IDC_MANU_PTP_ALIGN,			_T("ALIGN 精度確認位置"));

			//	HEAD S PTP구동
		SetDlgItemText(IDC_LABEL_CAP_HEADPTP_GROUP,			_T("HEAD S PTP驅動"));
		SetDlgItemText(IDC_MANU_PTP_HEAD_WAIT,			_T("Head dummy 位置"));
		SetDlgItemText(IDC_MANU_PTP_HEAD_MEAS,			_T("Head 測定位置"));
		SetDlgItemText(IDC_MANU_PTP_HEAD_JOB,			_T("Head 作業位置"));
		SetDlgItemText(IDC_MANU_PTP_HEAD_SUPPLY,			_T("Head 液晶供給位置"));

		//STAGE VAC.
		SetDlgItemText(IDC_LABEL_CAP_HEADPTP_GROUP2,			_T("STAGE VAC."));
		SetDlgItemText(IDC_MANU_VACUUM_OFF,			_T("Stage 苔    OFF"));
		SetDlgItemText(IDC_MANU_PURGE_ON,			_T("Stage 苔     ON"));

		// IONIZER CHECK
		SetDlgItemText(IDC_LABEL_CAP_HEADPTP_GROUP3,			_T(" IONIZER CHECK"));
		SetDlgItemText(IDC_IONIZER_ON,			_T("除靜電裝置         ON"));
		SetDlgItemText(IDC_IONIZER_OFF,			_T("除靜電裝置       OFF"));

		//액정 공급 관련
		SetDlgItemText(IDC_LABEL2,			_T("供給Head選擇"));
		SetDlgItemText(IDC_CDA2,			_T("CDA2 ON"));
		SetDlgItemText(IDC_PINCH2,			_T("PINCH2 OPEN"));
		SetDlgItemText(IDC_SUPPLY_NOZZLE2,			_T("NOZZLE2    前進"));
		SetDlgItemText(IDC_SPY_HEAD,			_T("HEAD 1"));
		SetDlgItemText(IDC_CDA1,			_T("CDA1 ON"));
		SetDlgItemText(IDC_PINCH1,			_T("PINCH1 OPEN"));
		SetDlgItemText(IDC_SUPPLY_NOZZLE1,			_T("NOZZLE1      前進"));

		//이전 화면
		SetDlgItemText(IDC_CMD_RETURN,			_T("返回"));
	}
}

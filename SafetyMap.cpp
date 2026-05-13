// SafetyMap.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "SafetyMap.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"
#include "P8CA_LcDispDoc.h"
#include "Color.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL g_bPvgOn; //jykim 171208 DoorInterlock PVG 추가
/////////////////////////////////////////////////////////////////////////////
// CSafetyMap dialog


CSafetyMap::CSafetyMap(CWnd* pParent /*=NULL*/)
	: CDialog(CSafetyMap::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSafetyMap)
	//}}AFX_DATA_INIT
}


void CSafetyMap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSafetyMap)
	DDX_Control(pDX, IDC_ROBOTARM, m_ctrlRobotArm);
	DDX_Control(pDX, IDC_MAINTDOOR2, m_ctrlMaintDoor2);
	DDX_Control(pDX, IDC_MAINTDOOR1, m_ctrlMaintDoor1);
	DDX_Control(pDX, IDC_GLASS, m_ctrlGlass);
//	DDX_Control(pDX, IDC_EMS6, m_ctrlEms6);
	DDX_Control(pDX, IDC_EMS5, m_ctrlEms5);
	DDX_Control(pDX, IDC_EMS4, m_ctrlEms4);
	DDX_Control(pDX, IDC_EMS3, m_ctrlEms3);
	DDX_Control(pDX, IDC_EMS2, m_ctrlEms2);
	DDX_Control(pDX, IDC_EMS1, m_ctrlEms1);
	DDX_Control(pDX, IDC_EMS0, m_ctrlEms0);
	DDX_Control(pDX, IDC_EMO, m_ctrlEmo);
	DDX_Control(pDX, IDC_DOOR2, m_ctrlDoor2);
	DDX_Control(pDX, IDC_DOOR3, m_ctrlDoor3);
	DDX_Control(pDX, IDC_DOOR1, m_ctrlDoor1);
	DDX_Control(pDX, IDC_AREA1, m_ctrlArea1);
	DDX_Control(pDX, IDC_AREA2, m_ctrlArea2);
	DDX_Control(pDX, IDC_AREA3, m_ctrlArea3);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSafetyMap, CDialog)
	//{{AFX_MSG_MAP(CSafetyMap)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSafetyMap message handlers

BOOL CSafetyMap::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ctrlRobotArm.SetBackColor(WHITE);
	m_ctrlGlass.SetBackColor(WHITE);	
	
	m_ctrlDoor1.SetBackColor(WHITE);
	m_ctrlDoor2.SetBackColor(WHITE);
	m_ctrlDoor3.SetBackColor(WHITE);
	m_ctrlMaintDoor2.SetBackColor(WHITE);
	m_ctrlMaintDoor1.SetBackColor(WHITE);
	
	m_ctrlEmo.SetBackColor(WHITE);
	m_ctrlEms0.SetBackColor(WHITE);
	m_ctrlEms1.SetBackColor(WHITE);
	m_ctrlEms2.SetBackColor(WHITE);
	m_ctrlEms3.SetBackColor(WHITE);
	m_ctrlEms4.SetBackColor(WHITE);
	m_ctrlEms5.SetBackColor(WHITE);
//	m_ctrlEms6.SetBackColor(WHITE);

	SetTimer(0,100,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSafetyMap::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE) // ESC Key가 아니면
		{
			pMsg->wParam = NULL;
			pMsg->lParam = NULL;
		}
	}			
	return CDialog::PreTranslateMessage(pMsg);
}

void CSafetyMap::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pView = (CP8CA_LcDispDoc*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	
	BOOL bIoRobotarm,bIoDoor1,bIoDoor2,bIoDoor3,bIoMaintDoorL,bIoGlass,bIoEmo,bIoMaintDoorR;
	WORD EmsIo,EmsIoTemp;
	BOOL bEmsIO[7] = {FALSE,};	
	BOOL io_EMO1 = 0, io_EMO2 = 0, io_EMO3 = 0, io_EMO4 = 0, io_EMO5 = 0, io_EMO6 =0, io_EMO7 =0;//jykim 170327 EMO I/O
	BOOL io_AREA1 = 0, io_AREA2 = 0, io_AREA3 = 0;

	int i;

	bIoRobotarm=bIoDoor1=bIoDoor2=bIoDoor3=bIoMaintDoorL=bIoMaintDoorR=bIoGlass=bIoEmo=FALSE;
	EmsIo=EmsIoTemp=0x0000;

	FAS_GetIoBit(1,false,ROBOT_ARM_DETECT,&bIoRobotarm);
	FAS_GetIoBit(1,false,DOOR_SENSOR1,&bIoDoor1);	
	FAS_GetIoBit(1,false,DOOR_SENSOR2,&bIoDoor2);	
	FAS_GetIoBit(1,false,DOOR_SENSOR3,&bIoDoor3);	

	FAS_GetIoBit(1,false,GLASS_DETECT,&bIoGlass);
	FAS_GetIoBit(4,TRUE,POWER_ON,&bIoEmo);	
	FAS_GetIoBit(1,false,MAINT_DOOR,&bIoMaintDoorL);	
	FAS_GetIoBit(1,false,MAINT_DOOR,&bIoMaintDoorR);
////////////////////////////////////////////////////////jykim 170327 EMO I/O
	FAS_GetIoBit(4,true,EMO1,&io_EMO1);
	FAS_GetIoBit(4,true,EMO2,&io_EMO2);
	FAS_GetIoBit(4,true,EMO3,&io_EMO3);
	FAS_GetIoBit(4,true,EMO4,&io_EMO4);
	FAS_GetIoBit(4,true,EMO5,&io_EMO5);
	FAS_GetIoBit(4,true,EMO6,&io_EMO6);
	FAS_GetIoBit(4,true,EMO7,&io_EMO7);

	if(pDoc->m_structMachineInfo.m_strLineNo == "5") //jykim 180222 버전통합 5라인만 AREA SENSOR 사용
	{
		FAS_GetIoBit(2,true,AREA1,&io_AREA1);
		FAS_GetIoBit(2,true,AREA2,&io_AREA2);
		FAS_GetIoBit(2,true,AREA3,&io_AREA3);
	}
////////////////////////////////////////////////////////jykim 170327 EMO I/O


	bIoRobotarm==TRUE ? m_ctrlRobotArm.SetBackColor(RED) : m_ctrlRobotArm.SetBackColor(WHITE);
	bIoDoor1==TRUE ? m_ctrlDoor1.SetBackColor(RED) : m_ctrlDoor1.SetBackColor(WHITE);
	bIoDoor2==TRUE ? m_ctrlDoor2.SetBackColor(RED) : m_ctrlDoor2.SetBackColor(WHITE);
	bIoDoor3==TRUE ? m_ctrlDoor3.SetBackColor(RED) : m_ctrlDoor3.SetBackColor(WHITE);
	bIoGlass==TRUE ? m_ctrlGlass.SetBackColor(GREEN) : m_ctrlGlass.SetBackColor(WHITE);
	
	if(bIoMaintDoorL==TRUE)
		m_ctrlMaintDoor1.SetBackColor(RED); 
	else
		m_ctrlMaintDoor1.SetBackColor(WHITE); 
	if(bIoMaintDoorR==TRUE)
		m_ctrlMaintDoor2.SetBackColor(RED);
	else
		m_ctrlMaintDoor2.SetBackColor(WHITE);

	io_EMO1==TRUE ? m_ctrlEmo.SetBackColor(RED):m_ctrlEmo.SetBackColor(WHITE);
	io_EMO2==TRUE ? m_ctrlEms0.SetBackColor(RED):m_ctrlEms0.SetBackColor(WHITE);
	io_EMO3==TRUE ? m_ctrlEms1.SetBackColor(RED):m_ctrlEms1.SetBackColor(WHITE);
	io_EMO4==TRUE ? m_ctrlEms2.SetBackColor(RED):m_ctrlEms2.SetBackColor(WHITE);
	io_EMO5==TRUE ? m_ctrlEms3.SetBackColor(RED):m_ctrlEms3.SetBackColor(WHITE);
	io_EMO6==TRUE ? m_ctrlEms4.SetBackColor(RED):m_ctrlEms4.SetBackColor(WHITE);
	io_EMO7==TRUE ? m_ctrlEms5.SetBackColor(RED):m_ctrlEms5.SetBackColor(WHITE);

	io_AREA1==TRUE ? m_ctrlArea1.SetBackColor(RED):m_ctrlArea1.SetBackColor(WHITE);
	io_AREA2==TRUE ? m_ctrlArea2.SetBackColor(RED):m_ctrlArea2.SetBackColor(WHITE);
	io_AREA3==TRUE ? m_ctrlArea3.SetBackColor(RED):m_ctrlArea3.SetBackColor(WHITE);

	CDialog::OnTimer(nIDEvent);
}

void CSafetyMap::OnOK() 
{
	// TODO: Add extra validation here
	g_bPvgOn = FALSE; //20181130 jeongyong - DoorInterlock PVG 추가
	KillTimer(0);
	CDialog::OnOK();
}

// NormalMsg.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"

#include "NormalMsg.h"
#include "TempOP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//extern THREADSTAGE ThreadStage;
//extern THREADORIGIN ThreadOrigin;
extern BOOL m_bSWCheck;
extern BOOL g_bRemoteControl;
/////////////////////////////////////////////////////////////////////////////
// CNormalMsg dialog


CNormalMsg::CNormalMsg(CWnd* pParent /*=NULL*/)
	: CDialog(CNormalMsg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNormalMsg)
	m_bCheckZServoOff = FALSE;
	//}}AFX_DATA_INIT
	m_strMsg1 = "";
	m_strMsg2 = "";
	m_strTitle = "";
	m_bCheckZServoOff=false;
	m_nTimerCount=0;
	m_bTimer= false;

	// origin searching concerned..
	m_bOrigin = false;
	m_nStep = 0;
	m_nEndMessage = -1;
	m_nSelHide = 0x00;
	m_bSWCheck = false;
}

CNormalMsg::~CNormalMsg()
{
}

void CNormalMsg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNormalMsg)
	DDX_Control(pDX, IDC_CMD_TITLE, m_ctrlTitle);
	DDX_Control(pDX, IDC_LABEL_MSG1, m_ctrlMsg1);
	DDX_Control(pDX, IDC_LABEL_MSG2, m_ctrlMsg2);
	DDX_Check(pDX, IDC_CHECK_ZSERVOOFF, m_bCheckZServoOff);
	//}}AFX_DATA_MAP
}

BEGIN_EVENTSINK_MAP(CNormalMsg, CDialog)
  //{{AFX_EVENTSINK_MAP(CNormalMsg)
	ON_EVENT(CNormalMsg, IDC_CMD_TITLE, -600 /* Click */, OnClickCmdTitle, VTS_NONE)	
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

BEGIN_MESSAGE_MAP(CNormalMsg, CDialog)
	//{{AFX_MSG_MAP(CNormalMsg)
	ON_WM_TIMER()
	ON_WM_RBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNormalMsg message handlers

BOOL CNormalMsg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here	
	CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView * pView = (CP8CA_LcDispView *) pFrame->GetActiveView();

 	this->SetWindowPos( NULL,0,0,1024,768, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER ); // NomalMessage 튕김현상으로 인한 
	                                                                                     // 강제 위치 설정
	m_ctrlTitle.SetCaption( m_strTitle );
	m_ctrlMsg1.SetCaption( m_strMsg1 );
	m_ctrlMsg2.SetCaption( m_strMsg2 );
	m_bSWCheck = false;
	if(m_bOrigin == TRUE) 
	{
		pView->RunThread(THREAD_ORIGINALL);
		while(1)
		{
			if(ThreadOrigin.bRunning == TRUE) 
			{
				SetTimer(1,100,NULL); 
				pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispBuzzer,"0000");
				pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispLamp,"0040");
				break;
			}
			pFrame->DoEvents();
		}
	}
	else
	{
		if(PC_TYPE == TRUE ) 
		{
			if(m_bTimer==TRUE)
			{ 
				SetTimer(0,100,NULL); 
				m_nSelHide = 0x11; 
				ThreadStage.ManualJobStep=0;
			}
		}
		else m_nSelHide = 0x00;
	}

	SubButtonHide();
	//
	GetDlgItem(IDC_CHECK_ZSERVOOFF)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNormalMsg::OnOK() 
{
	// TODO: Add extra validation here
	m_nEndMessage =0 ;
	if(m_bOrigin == TRUE) {ThreadOrigin.nEndMessage=0; return;}

	if(g_bRemoteControl)
	{
//		return;
//		CTempOP dlg;
//		dlg.DoModal();	
	}
	CDialog::OnOK();
}

void CNormalMsg::OnCancel() 
{
	// TODO: Add extra cleanup here
	m_nEndMessage =1 ;
	if(m_bOrigin == TRUE) {ThreadOrigin.nEndMessage=1; return;}
	
	CDialog::OnCancel();
}

//
void CNormalMsg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
//
	switch( nIDEvent )
	{
	case 0:
		SubTimerFunc0();   // 원점이 이미 잡혀 있는 경우에.. 
		break;

	case 1:
		SubTimerFunc1();   // 원점을 잡을 때..
		break;
	}
//
	CDialog::OnTimer(nIDEvent);
}

void CNormalMsg::SubTimerFunc0() 
{
	CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView * pView = (CP8CA_LcDispView *) pFrame->GetActiveView();
//	CP8CA_LcDispDoc * pDoc = (CP8CA_LcDispDoc *) pFrame->GetActiveDocument();

//	m_ctrlMsg2.SetCaption( ThreadStage.strMsg1 );
	//////////////////////////////////////////////
	DWORD io_val1 = 0x00000000;
	int ialarmBD = 0, isvAlarm = 0, ibdID = 0 ;
	WORD iaxis = 0;
	LONG AxisStatus=0;
	BOOL bMiniDoorSafetyOK=FALSE;

#if LINE1
	FAS_GetIo(4,true,&io_val1);     
#else
	FAS_GetIo(2,true,&io_val1);  //판넬 조작키 1		// 
#endif

	// Moni Door Sensor
//	if(ThreadStage.AdjustInitCode==1)	bMiniDoorSafetyOK=pView->MiniDoorSafetyOK(TRUE);	// 0:일반 Safety 적용, 1: Setting Mode Safety 적용
//	else	bMiniDoorSafetyOK=pView->MiniDoorSafetyOK(FALSE);

	if(ThreadStage.bMachineRunning == FALSE)
	{
		if(m_bSWCheck || pView->pTempOp->m_bOK)
		{
			io_val1 = 9999;
			io_val1 = OK_SWITCH;
			m_bSWCheck = FALSE;
		}
		
		m_nTimerCount++;
		if(m_nTimerCount == 5)
		{
#if LINE1
			FAS_SetIoBit(4,true,OK_LAMP, TRUE);
#else
			FAS_SetIoBit(4,true,OK_LAMP, TRUE);//2015.04.08 by tskim
#endif
		}
		else if(m_nTimerCount == 10)
		{
#if LINE1
			FAS_SetIoBit(4,true,OK_LAMP, FALSE);
#else
			FAS_SetIoBit(4,true,OK_LAMP, FALSE);//2015.04.08 by tskim
#endif
			m_nTimerCount = 0;
		}
		//
		if(((io_val1 & UDRL_SWITCH) != 0) || pView->pTempOp->m_bUp || pView->pTempOp->m_bDown || pView->pTempOp->m_bLeft || pView->pTempOp->m_bRight) // 방향key중에 한 개만 감지되면..
		{
#if LINE1
			FAS_SetIoBit(4,true,OK_LAMP, FALSE);
#else
			FAS_SetIoBit(4,true,OK_LAMP, FALSE);//2015.04.08 by tskim
#endif

			KillTimer(0);
			pView->pTempOp->m_bOK = FALSE;
			EndDialog(IDOK);
		}
		else if(((io_val1 & OK_SWITCH) != 0) || pView->pTempOp->m_bOK)  // OK 스위치가 들어오면..
		{	
			for( ibdID = 1 ; ibdID <5 ; ibdID++){			// 모터 Reset은 일단 Seal과 구분하지 않아도 됨.
				for( iaxis = 0 ; iaxis <16 ; iaxis++){
					//비어 있는 축은 Check Skip 한다.
					if(ibdID==4){	if(iaxis>3) continue;	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*					if(ibdID==2 || ibdID==4)//jykim 200509 HEAD SKIP (16번 헤드 P,G축 서보알람 체크 해제) P축: 2-3//G축: 4-3
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
			//
			if(ibdID == 10) {
				isvAlarm = ialarmBD*0x10 + iaxis;
				ThreadStage.ExitFlag = TRUE;
				EndDialog(IDOK);
				KillTimer(0);
				pView->SendMessage(WM_ERROR,115,isvAlarm);
				
				return;
			}

			for( ibdID = 1 ; ibdID <=4 ; ibdID++)
			{
				for( iaxis = 0 ; iaxis <16 ; iaxis++)
				{
					if(ibdID==4){	if(iaxis>3) continue;	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*					if(ibdID==2 || ibdID==4)//jykim 200509 HEAD SKIP (16번 헤드 P,G축 서보알람 체크 해제) P축: 2-3//G축: 4-3
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
					if(!(AxisStatus & SERVO_ON)){		// Servo On이 되어 있지 않은 축만 Servo On한다.
						FAS_ServoEnable(ibdID,iaxis,true);
						Sleep(100);
					}
				}
			}
			///////////////////////////////////////

			ThreadStage.JobFlag=STAGE_MANUAL; 
			ThreadStage.ManualCode='P';
			pView->RunThread(THREAD_STAGE);

			pView->pTempOp->m_bOK = FALSE;
		}
		///
		if(ThreadStage.ManualJobStep==100) // Exit..
		{
#if LINE1
			FAS_SetIoBit(4,true,OK_LAMP, FALSE);
#else
			FAS_SetIoBit(4,true,OK_LAMP, FALSE);//2015.04.08 by tskim
#endif
			EndDialog(IDOK);
			KillTimer(0);
			pView->pTempOp->m_bOK = FALSE;
		}
		pFrame->DoEvents();
	}
	else // ThreadStage.bMachineRunning
	{

	}
}

void CNormalMsg::SubTimerFunc1() 
{
	CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView * pView = (CP8CA_LcDispView *) pFrame->GetActiveView();
    CP8CA_LcDispDoc * pDoc = (CP8CA_LcDispDoc *) pFrame->GetActiveDocument();	
	DWORD dwIOResultF=0;
	BOOL bIOResult=FALSE, bMiniDoorSafetyOK=FALSE;
	BOOL bIOResult1=FALSE;
	BOOL bIOResult2=FALSE;
	BOOL bIOResult3=FALSE;
	BOOL bIOResult4=FALSE;


//	m_nTimerCount = 0 ;
	m_nTimerCount++;

	if(m_nTimerCount == 5) m_ctrlTitle.SetBackColor(GREEN);
	else if(m_nTimerCount == 10) 
	{
		m_nTimerCount = 0;
		m_ctrlTitle.SetBackColor(YELLOW);
	}
	////////////////////////////////////////////
	m_nSelHide = ThreadOrigin.nHide;

	m_ctrlTitle.SetCaption( "원점감지 동작" );
	m_ctrlMsg1.SetCaption( ThreadOrigin.strMsg1 );
	m_ctrlMsg2.SetCaption( ThreadOrigin.strMsg2 );
	SubButtonHide();
	//////////////////////////////////////////////
	if(pView->m_pDevice->SST_Check(STOP_SWITCH)) 
	{
		ThreadOrigin.ExitFlag = true; // ** exit **
	}
	/////////////////////////////////////////////
	if(ThreadOrigin.bRunning == FALSE)
	{
		KillTimer(1);
		EndDialog(IDOK);
	}
	else
	{
		FAS_GetIo(1,false,&dwIOResultF);
		FAS_GetIoBit(1,false,ROBOT_ARM_DETECT,&bIOResult);	// Robot Arm Check

		FAS_GetIoBit(1,false,DOOR_SENSOR1,&bIOResult1);	// Door Check
		FAS_GetIoBit(1,false,DOOR_SENSOR2,&bIOResult2);	// Door Check
		FAS_GetIoBit(1,false,DOOR_SENSOR3,&bIOResult3);	// Door Check
		FAS_GetIoBit(1,false,MAINT_DOOR,&bIOResult4);	// Door Check
	}
}
void CNormalMsg::SubButtonHide() 
{
	if(m_nSelHide == 0x00)
	{
		GetDlgItem(IDOK)->EnableWindow(true);
		GetDlgItem(IDCANCEL)->EnableWindow(true);
	}
	else if(m_nSelHide == 0x01)
	{
		GetDlgItem(IDCANCEL)->EnableWindow(false);
	}
	else if(m_nSelHide == 0x10)
	{
		GetDlgItem(IDOK)->EnableWindow(false);
	}
	else if(m_nSelHide == 0x11)
	{
		GetDlgItem(IDOK)->EnableWindow(false);
		GetDlgItem(IDCANCEL)->EnableWindow(false);
	}

}

void CNormalMsg::OnClickCmdTitle() 
{
	return;
	// TODO: Add your control notification handler code here
	CTempOP dlg;
	dlg.DoModal();	
}





void CNormalMsg::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	// TODO: Add your message handler code here and/or call default
	pView->pTempOp->DoModal();
	CDialog::OnRButtonDblClk(nFlags, point);
}

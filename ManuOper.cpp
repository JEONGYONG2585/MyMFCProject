 // ManuOper.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"
#include "P8CA_LcDispDoc.h"

#include "ManuOper.h"
#include "NormalMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern THREADSTAGE ThreadStage;
//extern THREADBALANCE ThreadBalance;
//extern THREADORIGIN ThreadOrigin;
extern BOOL g_bRemoteControl;
extern BOOL m_bSWCheck;
/////////////////////////////////////////////////////////////////////////////
// CManuOper dialog


CManuOper::CManuOper(CWnd* pParent /*=NULL*/)
	: CDialog(CManuOper::IDD, pParent)
{
	//{{AFX_DATA_INIT(CManuOper)
	m_nY = -1;
	m_nS = -1;
	m_nSpeed = -1;
	//}}AFX_DATA_INIT

	m_nAxis = 0;
	m_nBdID = 0;
	m_nTimerCount = 0;

	// selected axis for jog move..
	ThreadStage.JogAxis = 0;
	m_bSWCheck = false;
}

CManuOper::~CManuOper()
{
}

void CManuOper::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CManuOper)
	DDX_Radio(pDX, IDC_CMD_Y, m_nY);
	DDX_Radio(pDX, IDC_CMD_S1, m_nS);
	DDX_Control(pDX, IDC_CMD_JOG, m_ctrlJog);
	DDX_Control(pDX, IDC_CMD_ORG_ALL, m_ctrlOrgAll);
	DDX_Control(pDX, IDC_CMD_ORG_SELECTED, m_ctrlOrgSelected);
	DDX_Radio(pDX, IDC_SPEED_LOW, m_nSpeed);
	DDX_Control(pDX, IDC_CMD_RETURN, m_ctrlReturn);
	DDX_Control(pDX, IDC_LM_PLUS, m_ctrlLimitPlus);
	DDX_Control(pDX, IDC_LM_MINUS, m_ctrlLimitMinus);
	DDX_Control(pDX, IDC_HOME, m_ctrlHome);
	DDX_Control(pDX, IDC_MOTOR_ALARM, m_ctrlMotorAlarm);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CManuOper, CDialog)
	//{{AFX_MSG_MAP(CManuOper)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_CMD_S1, OnCmdS1)
	ON_BN_CLICKED(IDC_CMD_Y, OnCmdY)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SPEED_LOW, OnSpeedLow)
	ON_BN_CLICKED(IDC_CMD_S2, OnCmdS1)
	ON_BN_CLICKED(IDC_CMD_S3, OnCmdS1)
	ON_BN_CLICKED(IDC_CMD_S4, OnCmdS1)
	ON_BN_CLICKED(IDC_CMD_S5, OnCmdS1)
	ON_BN_CLICKED(IDC_CMD_S6, OnCmdS1)
	ON_BN_CLICKED(IDC_CMD_S7, OnCmdS1)
	ON_BN_CLICKED(IDC_CMD_S8, OnCmdS1)
	ON_BN_CLICKED(IDC_CMD_S9, OnCmdS1)
	ON_BN_CLICKED(IDC_CMD_S10, OnCmdS1)
	ON_BN_CLICKED(IDC_CMD_S11, OnCmdS1)
	ON_BN_CLICKED(IDC_CMD_S12, OnCmdS1)
	ON_BN_CLICKED(IDC_SPEED_MID, OnSpeedLow)
	ON_BN_CLICKED(IDC_SPEED_HIGH, OnSpeedLow)
	ON_BN_CLICKED(IDC_CMD_S13, OnCmdS1)
	ON_BN_CLICKED(IDC_CMD_S14, OnCmdS1)
	ON_BN_CLICKED(IDC_CMD_S15, OnCmdS1)
	ON_BN_CLICKED(IDC_CMD_S16, OnCmdS1)
	ON_WM_RBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CManuOper message handlers

void CManuOper::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CPen pen;
	pen.CreatePen(PS_SOLID,5,BLUE);
	CPen *pOldPen = (CPen *)dc.SelectObject(&pen);

	dc.MoveTo(0,HEIGHT_YPOS);
	dc.LineTo(SCREEN_WIDTH,HEIGHT_YPOS);

//	dc.MoveTo(0,SCREEN_HEIGHT - HEIGHT_YPOS);
//	dc.LineTo(SCREEN_WIDTH,SCREEN_HEIGHT - HEIGHT_YPOS);

	dc.SelectObject(pOldPen);	
	// Do not call CDialog::OnPaint() for painting messages
}

void CManuOper::OnCmdS1() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	UpdateData(TRUE);
	//
	m_nY = -1;
	UpdateData(FALSE);
	//
	ThreadStage.JogAxis = 'S';
	m_nBdID=pDoc->m_structHeadConfig[m_nS].nS[0];
	m_nAxis=pDoc->m_structHeadConfig[m_nS].nS[1];
	
}

void CManuOper::OnCmdY() 
{
	UpdateData(TRUE);

	m_nS = -1;
	UpdateData(FALSE);
	// jog
	if(m_nY == 0)
		ThreadStage.JogAxis = 'K';
	else
		ThreadStage.JogAxis = 0;

	// axis
	if(m_nY == 0)
	{
		m_nBdID = 1;
		m_nAxis = AXIS_K_MASTER;
	}
}


BEGIN_EVENTSINK_MAP(CManuOper, CDialog)
    //{{AFX_EVENTSINK_MAP(CManuOper)
	ON_EVENT(CManuOper, IDC_CMD_RETURN, -600 /* Click */, OnClickCmdReturn, VTS_NONE)
	ON_EVENT(CManuOper, IDC_CMD_JOG, -600 /* Click */, OnClickCmdJog, VTS_NONE)
	ON_EVENT(CManuOper, IDC_CMD_ORG_SELECTED, -600 /* Click */, OnClickCmdOrgSelected, VTS_NONE)
	ON_EVENT(CManuOper, IDC_CMD_ORG_ALL, -600 /* Click */, OnClickCmdOrgAll, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CManuOper::OnClickCmdReturn() 
{
	// TODO: Add your control notification handler code here
	//
	KillTimer(3);
	EndDialog(IDOK);	
}

BOOL CManuOper::OnInitDialog() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	SelectLanguage();

	m_nSpeed = 0;
	UpdateData(false);
	//
	OnSpeedLow();
	SetTimer(3,500,NULL);
	if(PC_TYPE == TRUE) 
	{
//		SetTimer(2,200,NULL);
		SubPositionDisplay(1);
	}
//
	pView->pTempOp->m_bJogControl = FALSE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CManuOper::OnOK() 
{
	// TODO: Add extra validation here
	
//	CDialog::OnOK();
}

void CManuOper::OnCancel() 
{
	// TODO: Add extra cleanup here
	
//	CDialog::OnCancel();
}

void CManuOper::OnClickCmdJog() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

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

	if(ThreadStage.JogAxis == 0) return; // 선택된 axis가 존재하지 않는다..
	//
	ThreadStage.HomeSearchMotor = m_nAxis;
	ThreadStage.HomeSearchBD = m_nBdID;
	ThreadStage.ManualCode = 'J';

	pView->pTempOp->m_bJogControl = TRUE;//조그 동작..
/*
	SubButtonDisable();
	m_ctrlJog.SetBackColor(LIGHTRED);
*/
	if(PC_TYPE == TRUE)
	{
		ThreadStage.JobFlag = STAGE_MANUAL; // manual
		ThreadStage.ExitFlag = FALSE;
		pView->RunThread(THREAD_STAGE);	// Stage Manual Thread Start
		//SetTimer(2,200,NULL);
		m_nTimerCount = 0;
		//
		while(1)
		{
			if(ThreadStage.bMachineRunning == TRUE) 
			{
				SubButtonDisable();
				m_ctrlJog.SetBackColor(LIGHTRED);
				SetTimer(0,100,NULL);
				break;
			}
			pFrame->DoEvents();
		}
	}
	else SetTimer(0,100,NULL);	
}

void CManuOper::OnClickCmdOrgSelected() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
//
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

	ThreadStage.HomeSearchMotor = m_nAxis;
	ThreadStage.HomeSearchBD = m_nBdID;
//
//	BOOL bSyncIoReady = 0;
	int icount=0;
//
	if(m_nY >= 0)
	{
		ThreadStage.ManualCode = 'G'; //'K';	
	}
	else if(m_nS >= 0)	ThreadStage.ManualCode = 'S';
	else return; // no selected
//
//	SubButtonDisable();
//	m_ctrlOrgSelected.EnableWindow(true);
//	m_ctrlOrgSelected.SetBackColor(LIGHTRED);

	//
	if(PC_TYPE == TRUE)
	{
		ThreadStage.JobFlag = STAGE_MANUAL; // manual
		ThreadStage.ExitFlag = false;
		ThreadStage.ManualJobStep = 0;
		pView->RunThread(THREAD_STAGE);	// Stage Manual Thread Start
		m_nTimerCount = 0;
		//
		while(1)
		{
			if(ThreadStage.bMachineRunning == TRUE) 
			{
				SubButtonDisable();
				SetTimer(1,100,NULL);
				break;
			}
			pFrame->DoEvents();
			Sleep(50);
			if(ThreadStage.ExitFlag==TRUE) icount++;
			if(icount>100) break; 
		}		
	}
	else
		SetTimer(1,100,NULL);
}

void CManuOper::OnClickCmdOrgAll() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
//
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
	
	if(PC_TYPE == TRUE)
	{
		CNormalMsg dlg;

		dlg.m_bOrigin = true;
		dlg.DoModal();

		SubPositionDisplay(1); // position display
	}
}

void CManuOper::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	DWORD dwIOResultF=0;
	BOOL bIOResult=FALSE, bMiniDoorSafetyOK=FALSE;

	m_nTimerCount++;
	SubPositionDisplay(0);
	///////////////////////////
	/// botton on-off
	switch(nIDEvent)
	{
	case 0 : // jog
		if(m_nTimerCount <= 5) 
			m_ctrlJog.SetBackColor(WHITEGRAY);
		else
			m_ctrlJog.SetBackColor(LIGHTRED);

		if(m_nTimerCount > 10)
		{
			m_nTimerCount = 0;
//			m_ctrlJog.SetBackColor(LIGHTRED);
		}

		if(ThreadStage.bMachineRunning == FALSE)
		{
			KillTimer(0);
			m_ctrlJog.SetBackColor(WHITEGRAY);
			SubButtonEnable();
		}
		else
		{
			FAS_GetIoBit(1,false,ROBOT_ARM_DETECT,&bIOResult);	// Robot Arm Check
			if(bIOResult==TRUE)
			{
				ThreadStage.bEmergencyStop=TRUE; ThreadStage.ExitFlag = TRUE; ThreadStage.nHomeEnableCount = 0;
				pView->m_pDevice->Emergency_Stop();
				pView->SendMessage(WM_ERROR,104,NULL);
			}
		}

		break;
	case 1 : // org selected
		if(m_nTimerCount == 5) m_ctrlOrgSelected.SetBackColor(LIGHTRED);
		if(m_nTimerCount == 10)
		{
			m_nTimerCount = 0;
			m_ctrlOrgSelected.SetBackColor(WHITEGRAY);
		}
		//////////////////////////////////////////////
		if(pView->m_pDevice->SST_Check(STOP_SWITCH)) 
		{

			for(int ibd = 1 ; ibd <=3 ; ibd++) 
				for(int iaxis = 0 ; iaxis < 16 ; iaxis++) 
					FAS_MoveStop(ibd,iaxis,0); // all axis stop..
				
			for(int iaxis = 0 ; iaxis < 8 ; iaxis++) 
				FAS_MoveStop(4,iaxis,0); // all axis stop..
			
			ThreadStage.ExitFlag = true; // ** exit **
		}
		/////////////////////////////////////////////
		if(ThreadStage.bMachineRunning == FALSE)
		{
			KillTimer(1);
			m_ctrlOrgSelected.SetBackColor(WHITEGRAY);
			SubButtonEnable();
		}
		else
		{
			FAS_GetIoBit(1,false,ROBOT_ARM_DETECT,&bIOResult);	// Robot Arm Check
			if(bIOResult==TRUE)
			{
				ThreadStage.bEmergencyStop=TRUE; ThreadStage.ExitFlag = TRUE; ThreadStage.nHomeEnableCount = 0;
				pView->m_pDevice->Emergency_Stop();
				pView->SendMessage(WM_ERROR,104,NULL);
			}
		}
		break;
	case 2 : // stop switch 감시 timer..
		if(pView->m_pDevice->SST_Check(STOP_SWITCH)) 
		{
			ThreadStage.ExitFlag = TRUE;
			FAS_MoveStop(AXIS_S_MASTER/BOARD_AXES+1,AXIS_S_MASTER%BOARD_AXES,0);
			FAS_MoveStop(AXIS_K_MASTER/BOARD_AXES+1,AXIS_K_MASTER%BOARD_AXES,0);
			//
			for(int ihead=0 ; ihead <MAX_NOZZLE ; ihead++)
			{
				FAS_MoveStop(pDoc->m_structHeadConfig[ihead].nA[0],pDoc->m_structHeadConfig[ihead].nA[1],0);
				FAS_MoveStop(pDoc->m_structHeadConfig[ihead].nD[0],pDoc->m_structHeadConfig[ihead].nD[1],0);

				FAS_MoveStop(pDoc->m_structHeadConfig[ihead].nS[0],pDoc->m_structHeadConfig[ihead].nS[1],0);
			}
		}
	case 3:
		SubTimerMotorStatusDisp();		//선택된 축의 모터 Status표시..
		break;
	} // End of switch(nIDEvent)
	
	CDialog::OnTimer(nIDEvent);
}

void CManuOper::SubButtonEnable()
{
	int i = 0;

	for( i = 0 ; i < MAX_NOZZLE ; i++ )
		GetDlgItem(IDC_CMD_S1 + i)->EnableWindow(TRUE);
	
	GetDlgItem(IDC_CMD_Y)->EnableWindow(TRUE);
	//
	m_ctrlJog.EnableWindow(true);
	m_ctrlOrgSelected.EnableWindow(true);
	m_ctrlOrgAll.EnableWindow(true);
	m_ctrlReturn.EnableWindow(true);
}

void CManuOper::SubButtonDisable()
{

	int i = 0;

	for( i = 0 ; i < MAX_NOZZLE ; i++ )
		GetDlgItem(IDC_CMD_S1 + i)->EnableWindow(FALSE);

	GetDlgItem(IDC_CMD_Y)->EnableWindow(FALSE);

	//
	m_ctrlJog.EnableWindow(false);
	m_ctrlOrgSelected.EnableWindow(false);
	m_ctrlOrgAll.EnableWindow(false);
	m_ctrlReturn.EnableWindow(false);
}

void CManuOper::SubPositionDisplay(int nsel)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
//	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
//
	double dPos = 0;
	CString str = "";
	int i = 0;

	// Position Display Routine
	if( nsel == 0 ) // jog || origin selected..
	{
		// S axis 선택
		if(m_nS >= 0)
		{
			for( i=0 ; i < MAX_NOZZLE ; i++)
			{
				FAS_GetActualPos(pDoc->m_structHeadConfig[i].nS[0],pDoc->m_structHeadConfig[i].nS[1],&dPos);
				str.Format("%.2f",dPos /= S_PULSE);
				GetDlgItem(IDC_LABEL_S1 + i)->SetWindowText(str);
			}
		}
		// K axis 선택
		else if(m_nY >= 0)
		{
			// K1
			FAS_GetActualPos(AXIS_K1/BOARD_AXES+1,AXIS_K1%BOARD_AXES,&dPos);
			str.Format("%.2f",dPos /= K_PULSE);
			GetDlgItem(IDC_LABEL_Y)->SetWindowText(str);
		}
	}
	else // nsel ==1 // all home position
	{
		// S
		for( i=0 ; i < MAX_NOZZLE ; i++)
		{
			FAS_GetActualPos(pDoc->m_structHeadConfig[i].nS[0],pDoc->m_structHeadConfig[i].nS[1],&dPos);
			str.Format("%.2f",dPos /= S_PULSE);
			GetDlgItem(IDC_LABEL_S1 + i)->SetWindowText(str);
		}
		// K1
		FAS_GetActualPos(AXIS_K1/BOARD_AXES+1,AXIS_K1%BOARD_AXES,&dPos);
		str.Format("%.2f",dPos /= K_PULSE);
		GetDlgItem(IDC_LABEL_Y)->SetWindowText(str);
		//
	} // END OF ELSE
//
}

void CManuOper::OnSpeedLow() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
//
	UpdateData(TRUE);
//
	int ihead=0, ibdid=0, iaxis=0;
	long lParaNo = 8;	// Speed Setting Parameter No.=> 8

	if(m_nSpeed == 0)
	{
		FAS_SetLocalPara(AXIS_K_MASTER/BOARD_AXES+1, AXIS_K_MASTER%BOARD_AXES, lParaNo, 10000); //K0
		for(iaxis = 0 ; iaxis < 2 ; iaxis++) // k1 ~ k2
		{
			FAS_SetLocalPara(AXIS_K1/BOARD_AXES+1, iaxis, lParaNo, 10000);
		}
		for(ihead = 0 ; ihead < MAX_NOZZLE ; ihead++)
		{
			FAS_SetLocalPara(pDoc->m_structHeadConfig[ihead].nS[0], pDoc->m_structHeadConfig[ihead].nS[1], lParaNo, 1000);
		}

	}
	else if(m_nSpeed == 1)
	{
		FAS_SetLocalPara(AXIS_K_MASTER/BOARD_AXES+1, AXIS_K_MASTER%BOARD_AXES, lParaNo, 50000); //K0
		for(iaxis = 0 ; iaxis < 2 ; iaxis++) // k1 ~ k2
		{
			FAS_SetLocalPara(AXIS_K1/BOARD_AXES+1, iaxis, lParaNo, 50000);			
		}
		for(ihead = 0 ; ihead < MAX_NOZZLE ; ihead++)
		{
			FAS_SetLocalPara(pDoc->m_structHeadConfig[ihead].nS[0], pDoc->m_structHeadConfig[ihead].nS[1], lParaNo, 50000);
		}
	}
	else
	{
		FAS_SetLocalPara(AXIS_K_MASTER/BOARD_AXES+1, AXIS_K_MASTER%BOARD_AXES, lParaNo, 100000); //K0
		for(iaxis = 0 ; iaxis < 2 ; iaxis++) // k1 ~ k2
		{
			FAS_SetLocalPara(AXIS_K1/BOARD_AXES+1, iaxis, lParaNo, 100000);			
		}
		for(ihead = 0 ; ihead < MAX_NOZZLE ; ihead++)
		{
			FAS_SetLocalPara(pDoc->m_structHeadConfig[ihead].nS[0], pDoc->m_structHeadConfig[ihead].nS[1], lParaNo, 100000);
		}
	}		
}



void CManuOper::SubTimerMotorStatusDisp()
{
	LONG AxisStatus=0;

	if(ThreadStage.JogAxis !=0 )
	{
		ThreadStage.HomeSearchMotor = m_nAxis;
		ThreadStage.HomeSearchBD = m_nBdID;
		FAS_GetAxisStatus(ThreadStage.HomeSearchBD,ThreadStage.HomeSearchMotor,&AxisStatus);
		
		if(AxisStatus&LM_PLUS_DETECT)	m_ctrlLimitPlus.SetBackColor(RED);
		else	m_ctrlLimitPlus.SetBackColor(LIGHTGREEN);

		if(AxisStatus&LM_MINUS_DETECT)	m_ctrlLimitMinus.SetBackColor(RED);
		else	m_ctrlLimitMinus.SetBackColor(LIGHTGREEN);

		if(AxisStatus&HOME_DETECT)	m_ctrlHome.SetBackColor(RED);
		else	m_ctrlHome.SetBackColor(LIGHTGREEN);

		if(AxisStatus&SERVOALARM)	m_ctrlMotorAlarm.SetBackColor(RED);
		else	m_ctrlMotorAlarm.SetBackColor(LIGHTGREEN);
	}
	else
	{
		m_ctrlLimitPlus.SetBackColor(DARKGRAY);
		m_ctrlLimitMinus.SetBackColor(DARKGRAY);
		m_ctrlHome.SetBackColor(DARKGRAY);
		m_ctrlMotorAlarm.SetBackColor(DARKGRAY);
	}
}

void CManuOper::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	// TODO: Add your message handler code here and/or call default
	if(ThreadStage.JogAxis == 0) return; // 선택된 axis가 존재하지 않는다..
	if(!pView->pTempOp->m_bJogControl) return;
	pView->pTempOp->DoModal();
//	pView->pTempOp->m_bJogControl = TRUE;	
	CDialog::OnRButtonDblClk(nFlags, point);
}

void CManuOper::SelectLanguage()		//ehji 140309
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(pView->m_nLanguage == 0)
	{
		SetDlgItemText(IDC_STATIC1,				_T("속도설정"));
		SetDlgItemText(IDC_SPEED_LOW,				_T("저속 ( 0.1 mm /s)"));
		SetDlgItemText(IDC_SPEED_MID,				_T("중속 ( 10 mm /s)"));
		SetDlgItemText(IDC_SPEED_HIGH,				_T("고속 ( 70 mm /s)"));
		SetDlgItemText(IDC_CMD_JOG,				_T("Jog"));
		SetDlgItemText(IDC_CMD_ORG_SELECTED,				_T("선택 축 원점"));
		SetDlgItemText(IDC_CMD_ORG_ALL,				_T("모든 축 원점"));
		SetDlgItemText(IDC_CMD_RETURN,				_T("이전 화면"));
	}

	else if(pView->m_nLanguage == 1)
	{
		SetDlgItemText(IDC_STATIC1,				_T("Speed Setting"));
		SetDlgItemText(IDC_SPEED_LOW,				_T("L/S ( 0.1 mm /s)"));
		SetDlgItemText(IDC_SPEED_MID,				_T("M/S ( 10 mm /s)"));
		SetDlgItemText(IDC_SPEED_HIGH,				_T("H/S ( 70 mm /s)"));
		SetDlgItemText(IDC_CMD_JOG,				_T("Jog"));
		SetDlgItemText(IDC_CMD_ORG_SELECTED,				_T("Selected\n Axis Origin"));
		SetDlgItemText(IDC_CMD_ORG_ALL,				_T("All Axis Origin"));
		SetDlgItemText(IDC_CMD_RETURN,				_T("Return"));
	}

	else if(pView->m_nLanguage == 2)
	{
		SetDlgItemText(IDC_STATIC1,				_T("速度設定"));
		SetDlgItemText(IDC_SPEED_LOW,				_T("低速 ( 0.1 mm /s)"));
		SetDlgItemText(IDC_SPEED_MID,				_T("中速 ( 10 mm /s)"));
		SetDlgItemText(IDC_SPEED_HIGH,				_T("高速 ( 70 mm /s)"));
		SetDlgItemText(IDC_CMD_JOG,				_T("Jog"));
		SetDlgItemText(IDC_CMD_ORG_SELECTED,				_T("選擇軸原點"));
		SetDlgItemText(IDC_CMD_ORG_ALL,				_T("所有軸原點"));
		SetDlgItemText(IDC_CMD_RETURN,				_T("返回"));
	}

}

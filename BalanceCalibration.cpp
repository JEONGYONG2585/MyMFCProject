// BalanceCalibration.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "BalanceCalibration.h"
#include "MainFrm.h"
#include "P8CA_LcDispDoc.h"
#include "P8CA_LcDispView.h"
#include "Color.h"
#include "NormalMsg.h"
#include "TempOP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL m_bSWCheck;
extern BOOL g_bRemoteControl;
/////////////////////////////////////////////////////////////////////////////
// CBalanceCalibration dialog


CBalanceCalibration::CBalanceCalibration(CWnd* pParent /*=NULL*/)
	: CDialog(CBalanceCalibration::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBalanceCalibration)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bSWCheck = false;
}


void CBalanceCalibration::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBalanceCalibration)
	DDX_Control(pDX, IDOK, m_ctrlOK);
	DDX_Control(pDX, IDC_GRID_CAL, m_GridCal);
	DDX_Control(pDX, IDC_CAL_START, m_ctrlStart);
	DDX_Control(pDX, IDC_MSG, m_ctrlMsg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBalanceCalibration, CDialog)
	//{{AFX_MSG_MAP(CBalanceCalibration)
	ON_WM_TIMER()
	ON_WM_RBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBalanceCalibration message handlers

void CBalanceCalibration::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	SubTimerCal();
	CDialog::OnTimer(nIDEvent);
}

BOOL CBalanceCalibration::OnInitDialog() 
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here

	int i,j;
	CString str;

	m_GridCal.SetRows(6);
	m_GridCal.SetCols(5);

	m_GridCal.SetTextMatrix(0,0,"발란스");
	m_GridCal.SetTextMatrix(3,0,"발란스");
	m_GridCal.SetTextMatrix(1,0,"현재 무게(mg)");
	m_GridCal.SetTextMatrix(4,0,"현재 무게(mg)");
	m_GridCal.SetTextMatrix(2,0,"발란스 상태");
	m_GridCal.SetTextMatrix(5,0,"발란스 상태");

// data를 Control설정 // common
// m_dCommon[0][Bal별] : 발란스 현재 무게
// m_dCommon[1][Bal별] : 측정 컵의 경고 무게
   for(i=0;i<6;i++)
   {
	   m_GridCal.SetRowHeight(i,510);
	   m_GridCal.SetRow(i);
	   m_GridCal.SetCol(0);
	   m_GridCal.SetCellBackColor(LIGHTGRAY);
	   for(j=0;j<5;j++)
	   {
		   m_GridCal.SetColWidth(j,2000);
		   m_GridCal.SetColAlignment(j,4);
	   }
   }
   for(j=1;j<5;j++)
   {
//Balance#8~5
	   str.Format("발란스%d",(MAX_BAL+1)-j);
	   m_GridCal.SetTextMatrix(0,j,str);
	   str.Format("%.3f",pDoc->m_dCommon[0][(MAX_BAL-1)-(j-1)]);
	   m_GridCal.SetTextMatrix(1,j,str);
	   if(ThBal[(MAX_BAL-1)-(j-1)].nBalanceMode == 0)
	   {
		   m_GridCal.SetRow(2);
		   m_GridCal.SetCol(j);
		   m_GridCal.SetCellBackColor(LIGHTGREEN);
		   str.Format("정상 모드");
		   m_GridCal.SetTextMatrix(2,j,str);
	   }
	   else if(ThBal[(MAX_BAL-1)-(j-1)].nBalanceMode == 1)
	   {
		   m_GridCal.SetRow(2);
		   m_GridCal.SetCol(j);
		   m_GridCal.SetCellBackColor(RED);
		   str.Format("이상 모드");
		   m_GridCal.SetTextMatrix(2,j,str);
	   }
//Balance#4~1
	   str.Format("발란스%d",(MAX_BAL/2+1)-j);
	   m_GridCal.SetTextMatrix(3,j,str);
	   str.Format("%.3f",pDoc->m_dCommon[0][(MAX_BAL/2-1)-(j-1)]);
	   m_GridCal.SetTextMatrix(4,j,str);
	   if(ThBal[(MAX_BAL/2-1)-(j-1)].nBalanceMode == 0)
	   {
		   m_GridCal.SetRow(5);
		   m_GridCal.SetCol(j);
		   m_GridCal.SetCellBackColor(LIGHTGREEN);
		   str.Format("정상 모드");
		   m_GridCal.SetTextMatrix(5,j,str);
	   }
	   else if(ThBal[(MAX_BAL/2-1)-(j-1)].nBalanceMode == 1)
	   {
		   m_GridCal.SetRow(5);
		   m_GridCal.SetCol(j);
		   m_GridCal.SetCellBackColor(RED);
		   str.Format("이상 모드");
		   m_GridCal.SetTextMatrix(5,j,str);
	   }
   }

	for(i=0;i<MAX_BAL;i++)
	{
		pView->m_bBalanceCalibDo = FALSE;
		pView->m_bCalBal_Complete[i] = FALSE;
		pView->m_bSelectCalBal[i] = FALSE;
		if(i>3)
		{
			m_GridCal.SetRow(0);
			m_GridCal.SetCol((i-4)+1);
			m_GridCal.SetCellBackColor(WHITE);
		}
		else
		{
			m_GridCal.SetRow(3);
			m_GridCal.SetCol(i+1);
			m_GridCal.SetCellBackColor(WHITE);
		}
	}
	m_ctrlStart.SetBackColor(WHITEGRAY);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBalanceCalibration::OnOK() 
{
	// TODO: Add extra validation here
	KillTimer(0);
	CDialog::OnOK();
}

BEGIN_EVENTSINK_MAP(CBalanceCalibration, CDialog)
    //{{AFX_EVENTSINK_MAP(CBalanceCalibration)
	ON_EVENT(CBalanceCalibration, IDC_GRID_CAL, -600 /* Click */, OnClickGridCal, VTS_NONE)
	ON_EVENT(CBalanceCalibration, IDC_CAL_START, -600 /* Click */, OnClickCalStart, VTS_NONE)
	ON_EVENT(CBalanceCalibration, IDC_MSG, -600 /* Click */, OnClickMsg, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CBalanceCalibration::OnClickGridCal() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();


	if(pView->m_bBalanceCalibDo) return;

	int Row,Col;

	Row = m_GridCal.GetMouseRow();
	Col = m_GridCal.GetMouseCol();

	if(Col == 0) return;

	if(Row == 0 || Row == 1 || Row == 2) 
		Row = 0;
	else if(Row == 3 || Row == 4 || Row ==5)
		Row = 3;

//Col = 1 Bal 8,4
	if(m_GridCal.GetCellBackColor() == YELLOW)
	{
		if(Row == 0)
		{
			pView->m_bSelectCalBal[((MAX_BAL)-Row)-Col] = FALSE;
			m_GridCal.SetRow(Row);
			m_GridCal.SetCol(Col);
			m_GridCal.SetCellBackColor(WHITE);
		}
		else
		{
			pView->m_bSelectCalBal[((MAX_BAL)-(Row+1))-Col] = FALSE;
			m_GridCal.SetRow(Row);
			m_GridCal.SetCol(Col);
			m_GridCal.SetCellBackColor(WHITE);
		}
	}
	else if(m_GridCal.GetCellBackColor() == WHITE)
	{
		if(Row == 0)
		{
			pView->m_bSelectCalBal[((MAX_BAL)-Row)-Col] = TRUE;
			m_GridCal.SetRow(Row);
			m_GridCal.SetCol(Col);
			m_GridCal.SetCellBackColor(YELLOW);
		}
		else
		{
			pView->m_bSelectCalBal[((MAX_BAL)-(Row+1))-Col] = TRUE;
			m_GridCal.SetRow(Row);
			m_GridCal.SetCol(Col);
			m_GridCal.SetCellBackColor(YELLOW);
		}
	}
}

void CBalanceCalibration::OnClickCalStart() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	int SelectCnt = -1;
	int i=0;
	CString str;


	for(i=0;i<MAX_BAL;i++)
	{
		pView->m_bCalBal_Complete[i]=FALSE;
		if(pView->m_bSelectCalBal[i])
		{
			SelectCnt++;
		}
	}
	if(SelectCnt == -1) 
	{
		AfxMessageBox("선택 된 Balance 없음!!");
		return;
	}

	BOOL bIOResult1=FALSE;
	BOOL bIOResult2=FALSE;
	BOOL bIOResult3=FALSE;
	BOOL bIOResult4=FALSE;
	BOOL bSatetyInterlock = FALSE;


	FAS_GetIoBit(1,false,DOOR_SENSOR1,&bIOResult1);	// DOOR Check
	FAS_GetIoBit(1,false,DOOR_SENSOR2,&bIOResult2);	// DOOR Check
	FAS_GetIoBit(1,false,DOOR_SENSOR3,&bIOResult3);	// DOOR Check
	FAS_GetIoBit(1,false,MAINT_DOOR,&bIOResult4);	// DOOR Check

#if !TRF
bIOResult1=bIOResult2=bIOResult3=bIOResult4=FALSE;
#endif

//#if TRF
	if((bIOResult1 == TRUE)||(bIOResult2 == TRUE)||(bIOResult3 == TRUE)||(bIOResult4 == TRUE))
	{
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
	BOOL abBalance = FALSE;
	for(i=0;i<MAX_BAL;i++)
	{
		if(ThBal[i].nBalanceMode == 1)
		{
			abBalance = TRUE;
		}
	}
	if(abBalance)
	{
		AfxMessageBox("발란스 비정상 모드 임!");
	}
//#endif
	m_ctrlStart.SetBackColor(LIGHTGREEN);
	str.Format("판넬에 시작 버튼을 누르면 교정을 시작 합니다.!");
	m_ctrlMsg.SetCaption(str);
	SetTimer(0,500,NULL);
//	CNormalMsg m_NormalMsg;
//	m_NormalMsg.m_strTitle="Balance Calibration 실행";
//	m_NormalMsg.m_strMsg2 = " 실행하려면 OK버튼을, 취소하려면 방향버튼을 누르시오.";
//	m_NormalMsg.DoModal(); 	
}

void CBalanceCalibration::SubTimerCal()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	WORD wTempIO = 0 , wIOResult=0, wIOResult1=0;
	BOOL bIOResult=false, bMiniDoorSafetyOK=FALSE;
	DWORD dwIOResultF=0;
	int i=0,j=0;
	CString str;
//
	m_nTimerCount++;

// 	if(g_bMessageDisplay == TRUE)
// 	{
// 		m_ctrlMessageDummyDrop.SetCaption( ThreadStage.strMsg1 );
// 	}

	/////////////////////////////////////////////
	if(ThreadStage.bMachineRunning == TRUE)
	{
// 		if(g_bMessageDisplay == FALSE)
// 		{
// 			m_nMeasureJob=1;
// 		}
//         else
// 		{
// 			m_nMeasureJob=2;
// 		}
		//
		FAS_GetIo(1,false,&dwIOResultF);
//2008.01.25 By tskim
		FAS_GetIoBit(1,false,ROBOT_ARM_DETECT,&bIOResult);	// Robot Arm Check

			if(ThBal[BALID1].nTimerCount > -1) ThBal[BALID1].nTimerCount++; // ThreadBal1
			if(ThBal[BALID2].nTimerCount > -1) ThBal[BALID2].nTimerCount++; // ThreadBal2
			if(ThBal[BALID3].nTimerCount > -1) ThBal[BALID3].nTimerCount++; // ThreadBal3
			if(ThBal[BALID4].nTimerCount > -1) ThBal[BALID4].nTimerCount++; // ThreadBal4
			if(ThBal[BALID5].nTimerCount > -1) ThBal[BALID5].nTimerCount++; // ThreadBal5
			if(ThBal[BALID6].nTimerCount > -1) ThBal[BALID6].nTimerCount++; // ThreadBal6
			if(ThBal[BALID7].nTimerCount > -1) ThBal[BALID7].nTimerCount++; // ThreadBal7
			if(ThBal[BALID8].nTimerCount > -1) ThBal[BALID8].nTimerCount++; // ThreadBal8

		if(pView->m_pDevice->SST_Check(STOP_SWITCH)) 
		{
			m_bSWCheck = false;//2010.01.19

			//axis stop..
			FAS_MoveStop(1,0,0);	//S0
			FAS_MoveStop(1,1,0);	//K0
			FAS_MoveStop(2,0,0);	//K1
			FAS_MoveStop(2,1,0);	//K2

			for(int iaxis = 4; iaxis < 16; iaxis++)
			{
				FAS_MoveStop(2,iaxis,0);	//S1~S12				
			}

			for(iaxis = 0; iaxis < 4; iaxis++)
			{
				FAS_MoveStop(3,iaxis,0);	//S13~S16				
			}

			ThreadStage.ExitFlag = true;				// ** exit **
			ThBal[BALID1].ExitFlag = true;		ThBal[BALID2].ExitFlag = true;
			ThBal[BALID3].ExitFlag = true;		ThBal[BALID4].ExitFlag = true;
			ThBal[BALID5].ExitFlag = true;		ThBal[BALID6].ExitFlag = true;
			ThBal[BALID7].ExitFlag = true;		ThBal[BALID8].ExitFlag = true;
// 			g_bMessageDisplay = TRUE;
			pView->m_bBalanceCalibDo = FALSE;
		}
		for(i=0;i<MAX_BAL;i++)
		{
			if(pView->m_bCalBal_Complete[i])
			{
				if(i>3)
				{
					str.Format("%.3f",pDoc->m_dCommon[0][i]);
					m_GridCal.SetTextMatrix(1,8-i,str);
				}
				else
				{
					str.Format("%.3f",pDoc->m_dCommon[0][i]);
					m_GridCal.SetTextMatrix(4,4-i,str);
				}
			}
		}
		// 현재 Run 상태에서 Start 버튼을 On 시켜 놓는다.
		pView->m_pDevice->Start_button_onoff(true);
		pView->m_pDevice->Stop_button_onoff(false);
		m_nTimerCount = 0;
		//
 		m_ctrlStart.EnableWindow(false);
 		m_ctrlOK.EnableWindow(false);
// 		m_ctrlMeasure.EnableWindow(false);
// 		m_ctrlDummyDrop.EnableWindow(false);
// 		m_ctrlDropCount.EnableWindow(false);	
// 		m_ctrlPumpInitial.EnableWindow(false);
// 		m_ctrlVolCalculate.EnableWindow(false);
// 		m_ctrlReturn.EnableWindow(false);
// 		m_ctrlBalModeSet.EnableWindow(false);
	}
	else // ThreadStage.bMachineRunning == FALSE
	{

	//Initial Move
// 		if(ThreadStage.ManualJobStep > 0) m_nMeasureJob=1;
		//
		if(pView->m_pDevice->SST_Check(START_SWITCH)) 
		{	
			m_bSWCheck = false;//2010.01.19
			DWORD wIOResultF = 0, wIOResultR = 0;
			BOOL bIOResult=FALSE;
			FAS_GetIo(1,false,&wIOResultF); FAS_GetIo(1,false,&wIOResultR);
//2008.01.28 By tskim
			FAS_GetIoBit(1,false,ROBOT_ARM_DETECT,&bIOResult);	// Robot Arm Check

			if(wIOResult1!=0) // Balance 안에 측정 컵이 있을 경우 NG
			{
				ThreadStage.ExitFlag = true;
				pView->SendMessage(WM_ERROR,301,wIOResult1);	
				return;	
			}
			else
			{
				pView->m_bBalanceCalibDo = TRUE;
				ThreadStage.AdjustInitCode = 'Q';
				ThreadStage.ManualCode = 'Q'; 
				ThreadStage.ManualJobStep = 0;
				ThreadStage.JobFlag = STAGE_MANUAL;
				pView->RunThread(THREAD_STAGE);
				for(int i=0;i<MAX_BAL;i++)
				ThBal[i].JobStep = 0;
				UpdateData(FALSE);
			}				
		}
		// Stop 상태에서 
		// Stop 버튼이 On되도록 한다.
		pView->m_pDevice->Stop_button_onoff(true);
		// Start 버튼은 깜박거리게 한다.
		if(m_nTimerCount==2) pView->m_pDevice->Start_button_onoff(true);
		else if(m_nTimerCount>=4) 
		{
			pView->m_pDevice->Start_button_onoff(false);
			m_nTimerCount = 0;
		}
 		m_ctrlStart.EnableWindow(true);
 		m_ctrlOK.EnableWindow(true);
		m_ctrlStart.SetBackColor(WHITEGRAY);
	}
}

void CBalanceCalibration::OnClickMsg() 
{
	// TODO: Add your control notification handler code here
//	if(!g_bRemoteControl) return;
//	CTempOP dlg;
//	dlg.DoModal();	
}

void CBalanceCalibration::OnRButtonDblClk(UINT nFlags, CPoint point) 
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

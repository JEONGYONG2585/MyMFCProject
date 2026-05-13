// ErrMsg.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"

#include "SafetyMap.h"

#include "ErrMsg.h"

#include "TempOP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//2007.07.17 By tskim
//Target값 측정값 다를 경우 알람...
extern BOOL _bSpecOver[MAX_NOZZLE];

extern BOOL m_bSWCheck;
extern BOOL g_bRemoteControl;
/////////////////////////////////////////////////////////////////////////////
// CErrMsg dialog


CErrMsg::CErrMsg(CWnd* pParent /*=NULL*/)
	: CDialog(CErrMsg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CErrMsg)
	//}}AFX_DATA_INIT
//
	m_strMsg1 = "";
	m_strMsg2 = "";
	m_nErrCode = 0;
	m_nErrCode2 = 0;
	m_nSelectAlarm = 0;
	m_nTimerCount = 0;
	m_bSWCheck = false;
}

CErrMsg::~CErrMsg()
{
}

void CErrMsg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CErrMsg)
	DDX_Control(pDX, IDC_LABEL_ERRMSG_TITLE, m_ctrlTitle);
	DDX_Control(pDX, IDC_LABEL_ERRMSG_MSG1, m_ctrlMsg1);
	DDX_Control(pDX, IDC_LABEL_ERRMSG_MSG2, m_ctrlMsg2);
	DDX_Control(pDX, IDC_CMD_DO, m_ctrlReturn);
	DDX_Control(pDX, IDC_CMD_SAFETY, m_ctrlSafety);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CErrMsg, CDialog)
	//{{AFX_MSG_MAP(CErrMsg)
	ON_WM_TIMER()
	ON_WM_RBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CErrMsg message handlers

BOOL CErrMsg::OnInitDialog()		// 경알람, 중알람. 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	//by shin//2013.05.18
	for(int j = 0; j < MAX_NOZZLE; j++)
	{
		//n-type trigger off//TJD 함수 실행전에 반드시...//
		//n-type trigger off//TJD 함수 실행전에 반드시...//
		if(j<NOZZLE_P1)
		{
			FAS_SetIoBit(1, false, N_STOP_TRIGGER_01+j, FALSE);
			FAS_SetIoBit(1, true, N_START_TRIGGER_01+j, FALSE);
		}
		else
		{
			FAS_SetIoBit(2, true, N_STOP_TRIGGER_15+(j-NOZZLE_P1), FALSE);
			FAS_SetIoBit(2, true, N_START_TRIGGER_15+(j-NOZZLE_P1), FALSE);
		}
	}
	
//
	CString str = "";
	CString strErr;
	m_ctrlReturn.EnableWindow(false);

	pView->pTempOp->m_bJogControl = FALSE;
	
	
	ThreadStage.nAlarmcode=m_nErrCode;

	strErr.Format("<Err1> ErrorCode %d Alarm유형 %d pDoc->m_bAutoManualFlag %d",m_nErrCode,m_nSelectAlarm,pDoc->m_bAutoManualFlag);
	pView->SaveLog(0,strErr);
	

	if(m_nSelectAlarm == 0 && (m_nErrCode!=19) && (m_nErrCode!=20) )		// Error로 보고
	{
		m_strMsg1.Format("Error code : %d ",m_nErrCode);
		err_Message();
		//
		m_ctrlTitle.SetCaption(" ERROR ");
		m_ctrlMsg1.SetCaption(m_strMsg1);
		m_ctrlMsg2.SetCaption(m_strMsg2);

		m_ctrlReturn.EnableWindow(FALSE);
		//by shin//예외 처리 없앤다...//나중에 요청이 있을시 조건에 따라 할 수 있도록...//
		//요청 들어옴...ㅋㅋ//
		//by lee seung hyun // 효일씨 요청으로 18번 액정 잔량 설정치 관련 BM 해제 요청으로  추가 하였습니다.
		//by shlee //091027//BM해제 추가(146)//
		if((m_nErrCode!=1) && (m_nErrCode!=2) && (m_nErrCode!=3) && (m_nErrCode!=4) && (m_nErrCode!=5) &&
		   (m_nErrCode!=6) && (m_nErrCode!=13) && (m_nErrCode!=14) && (m_nErrCode!=15) && (m_nErrCode!=16) &&
		   (m_nErrCode!=18) && (m_nErrCode!=19) && (m_nErrCode!=20) && (m_nErrCode!=24) && (m_nErrCode!=25) && 
		   (m_nErrCode!=26) && (m_nErrCode!=27) && (m_nErrCode!=28) && (m_nErrCode!=29) && (m_nErrCode!=30) &&
		   (m_nErrCode!=31) && (m_nErrCode!=32) && (m_nErrCode!=33) && (m_nErrCode!=34) && (m_nErrCode!=60) && 
		   (m_nErrCode!=61) && (m_nErrCode!=108) && (m_nErrCode!=109) && (m_nErrCode!=111) && (m_nErrCode!=112) &&
		   (m_nErrCode!=113) && (m_nErrCode!=123) && (m_nErrCode!=137) && (m_nErrCode!=138) && (m_nErrCode!=140) &&
		   (m_nErrCode!=143) && (m_nErrCode!=144) && (m_nErrCode!=146) && (m_nErrCode!=152) && (m_nErrCode!=153) && 
		   (m_nErrCode!=154) && (m_nErrCode!=188) && (m_nErrCode!=189) && (m_nErrCode!=159)) 
		{
			if( pDoc->m_bAutoManualFlag == TRUE ) //090612 Manual 동작 시 BM 보고 안함. //by shin//2009.06.16//
			{
				//# Error Code: 1Word중 0-E(bit)까지는 Code, F(bit)는 (1:Set, 0:Reset)이다.
				CIM_Information.bAlarm_ON = TRUE;
				CIM_Information.nAlarm_Code = m_nErrCode|0x8000;
				CIM_Information.sAlarm_Code.Format("%04x",CIM_Information.nAlarm_Code);
				pView->m_pDevice->MNET_WriteWord(DISP_WORD+0x0F, CIM_Information.sAlarm_Code, 1); //# Alarm Code
				
				str.Format("%04x",m_nErrCode);// by ckh 2009.02.25 'F' 비트 사용함으로 인함. 
				pView->m_pDevice->MNET_WriteWord(DISP_WORD+0x09, str, 1); //# Unit Down Alarm Code
				
				pView->m_pDevice->MNET_BitSet(DISP_BIT+0x0F, TRUE);
				Sleep((int)(pDoc->m_dTeachData[37][0]*1000.0));
				pView->m_pDevice->MNET_BitSet(DISP_BIT+0x0F, FALSE);

	//			str.Format("%4x",m_nErrCode);
	//			pView->m_pDevice->MNET_WriteWord(BaseAddr_DISP+0x0F,str,1);//m_nErrCode write
				strErr.Format("<Err2> Auto Run Alarm ErrorCode %d CIM_Information.nAlarm_Code %s",m_nErrCode,CIM_Information.sAlarm_Code);
				pView->SaveLog(0,strErr);
			}
		}
		//
		pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispBuzzer,"0008");
		pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispLamp,"0020");	  // RED

		SetTimer(0,100,NULL);
	}
	
	else if(m_nSelectAlarm == 1)	// Warning으로 보고
	{
		m_strMsg1.Format("Error code : %d ",m_nErrCode);
		err_Message();

		m_ctrlTitle.SetCaption(" WARNING ");
		m_ctrlMsg1.SetCaption(m_strMsg1);
		m_ctrlMsg2.SetCaption(m_strMsg2);

		m_ctrlReturn.EnableWindow(FALSE);

		//# Warning Code: 1Word중 0-E(bit)까지는 Code, F(bit)는 (1:Set, 0:Reset)이다.
		CIM_Information.bWarning_ON = TRUE;
		CIM_Information.nWarning_Code = m_nErrCode|0x8000;
		CIM_Information.sWarning_Code.Format("%04x",CIM_Information.nWarning_Code);
		pView->m_pDevice->MNET_WriteWord(DISP_WORD+0x0E, CIM_Information.sWarning_Code, 1);
		pView->m_pDevice->MNET_BitSet(DISP_BIT+0x0E, TRUE);
		Sleep((int)(pDoc->m_dTeachData[37][0]*1000.0));
		pView->m_pDevice->MNET_BitSet(DISP_BIT+0x0E, FALSE);

//		str.Format("%4x",m_nErrCode);
//		pView->m_pDevice->MNET_WriteWord(BaseAddr_DISP+0x0E,str,1);//m_nErrCode write
		strErr.Format("<Err3> Warning ErrorCode %d CIM_Information.sWarning_Code %s",m_nErrCode,CIM_Information.sWarning_Code);
		pView->SaveLog(0,strErr);


//		pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispBuzzer,"0004");
		//by shin//2009.04.03//알람 박스 추가...무조건 알람 소리...
		pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispBuzzer,"0008");
//		pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispLamp,"0044"); //160920 jykim Red Lamp On 원본
		pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispLamp,"0024"); //160920 jykim Red Lamp On 

		SetTimer(0,100,NULL);
	}
	else if ( (m_nErrCode==19) || (m_nErrCode==20) )
	{
		m_strMsg1.Format("Error code : %d ",m_nErrCode);
		err_Message();

		m_ctrlTitle.SetCaption(" WARNING ");
		m_ctrlMsg1.SetCaption(m_strMsg1);
		m_ctrlMsg2.SetCaption(m_strMsg2);

		m_ctrlReturn.EnableWindow(FALSE);

		//# Warning Code: 1Word중 0-E(bit)까지는 Code, F(bit)는 (1:Set, 0:Reset)이다.
		CIM_Information.bWarning_ON = TRUE;
		CIM_Information.nWarning_Code = m_nErrCode|0x8000;
		CIM_Information.sWarning_Code.Format("%04x",CIM_Information.nWarning_Code);
		pView->m_pDevice->MNET_WriteWord(DISP_WORD+0x0E, CIM_Information.sWarning_Code, 1);
		pView->m_pDevice->MNET_BitSet(DISP_BIT+0x0E, TRUE);
		Sleep((int)(pDoc->m_dTeachData[37][0]*1000.0));
		pView->m_pDevice->MNET_BitSet(DISP_BIT+0x0E, FALSE);

//		str.Format("%4x",m_nErrCode);
//		pView->m_pDevice->MNET_WriteWord(BaseAddr_DISP+0x0E,str,1);//m_nErrCode write
		strErr.Format("<Err3> Warning ErrorCode %d CIM_Information.sWarning_Code %s",m_nErrCode,CIM_Information.sWarning_Code);
		pView->SaveLog(0,strErr);


//		pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispBuzzer,"0004");
		//by shin//2009.04.03//알람 박스 추가...무조건 알람 소리...
		pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispBuzzer,"0008");
		pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispLamp,"0020");	  // RED

		SetTimer(0,100,NULL);
	}
	else
	{
			//
		AfxMessageBox("여기에 debug code를 넣어요");
	}
//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CErrMsg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
//
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	DWORD io_val1 = 0, io_val2 = 0;
	CString strErr;

	m_nTimerCount++;
	//
	switch(nIDEvent)
	{
		case 0 :
			if(m_nTimerCount == 5)
			{
				m_ctrlTitle.SetBackColor(YELLOW);
				m_ctrlTitle.SetForeColor(LIGHTRED);
				FAS_SetIoBit(4,true,OK_LAMP, TRUE);
			}
			else if(m_nTimerCount == 10)
			{
				m_ctrlTitle.SetBackColor(LIGHTRED);
				m_ctrlTitle.SetForeColor(YELLOW);
				FAS_SetIoBit(4,true, OK_LAMP, FALSE);
				m_nTimerCount = 0;
			}
#if LINE1
			FAS_GetIo(4,true,&io_val1);
#else
			FAS_GetIo(2,true,&io_val1);
#endif

			io_val1 = io_val1&OK_SWITCH;

			if(m_bSWCheck || pView->pTempOp->m_bOK)
			{
				io_val1 = 9999;
				io_val1 = OK_SWITCH;
				m_bSWCheck = FALSE;
			}

			if(io_val1 != 0)   // ok1 switch가 눌리면..
			{
				FAS_SetIoBit(4, true,OK_LAMP, FALSE);
				pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispBuzzer, "0000");

/////////////////////////////////////////////////////////20211223 JEONGYONG - Door Open 시 ok버튼 무시
//				if(ThreadStage.nAlarmcode == 101)
/*				{
					BOOL bIOResult1, bIOResult2, bIOResult3, bIOResult4;
					bIOResult1 = FALSE;
					bIOResult2 = FALSE;
					bIOResult3 = FALSE;
					bIOResult4 = FALSE;
					FAS_GetIoBit(1,false,DOOR_SENSOR1,&bIOResult1);	// Door Check
					FAS_GetIoBit(1,false,DOOR_SENSOR2,&bIOResult2);	// Door Check
					FAS_GetIoBit(1,false,DOOR_SENSOR3,&bIOResult3);	// Door Check
					FAS_GetIoBit(1,false,MAINT_DOOR,&bIOResult4);	// Door Check
					//bIOResult1 = TRUE;
					if((bIOResult1 == TRUE)||(bIOResult2 == TRUE)||(bIOResult3 == TRUE)||(bIOResult4 == TRUE))
					{
						break;//Door Open 시 Return
					}
				}*/
/////////////////////////////////////////////////////////

				pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispLamp, "0040");

				// Error를 Clear 한다.
				ThreadStage.nAlarmcode=0;	
				CString str=""; str.Format("%4x",0);

				//# Error Code: 1Word중 0-E(bit)까지는 Code, F(bit)는 (1:Set, 0:Reset)이다.				
				if(CIM_Information.bAlarm_ON) //Alarm
				{	
					CIM_Information.nAlarm_Code = CIM_Information.nAlarm_Code & 0x7fff;
					CIM_Information.sAlarm_Code.Format("%04x", CIM_Information.nAlarm_Code);	
					pView->m_pDevice->MNET_WriteWord(DISP_WORD+0x0F, CIM_Information.sAlarm_Code, 1); 
					
					pView->m_pDevice->MNET_BitSet(DISP_BIT+0x0F, TRUE);
					Sleep((int)(pDoc->m_dTeachData[37][0]*1000.0));
					pView->m_pDevice->MNET_BitSet(DISP_BIT+0x0F, FALSE);
					CIM_Information.bAlarm_ON = FALSE;
					strErr.Format("<Err4> Alarm 해제 ErrorCode %d CIM_Information.nAlarm_Code %s",m_nErrCode,CIM_Information.sAlarm_Code);
					pView->SaveLog(0,strErr);
				}
				if(CIM_Information.bWarning_ON) //Warning
				{
					CIM_Information.nWarning_Code = CIM_Information.nWarning_Code & 0x7fff;
					CIM_Information.sWarning_Code.Format("%04x", CIM_Information.nWarning_Code);	
					pView->m_pDevice->MNET_WriteWord(DISP_WORD+0x0E, CIM_Information.sWarning_Code, 1); 
					pView->m_pDevice->MNET_BitSet(DISP_BIT+0x0E, TRUE);
					Sleep((int)(pDoc->m_dTeachData[37][0]*1000.0));
					pView->m_pDevice->MNET_BitSet(DISP_BIT+0x0E, FALSE);
					CIM_Information.bWarning_ON = FALSE;
					strErr.Format("<Err4> Alarm 해제 ErrorCode %d CIM_Information.nAlarm_Code %s",m_nErrCode,CIM_Information.sAlarm_Code);
					pView->SaveLog(0,strErr);
				}
				//Sleep(100);// by ckh 2009.03.09 추가함.
				// by ckh 2009.02.10
				if(pView->m_nMachineStatus !=-1)
				{
					pView->m_pDevice->MNET_Mode_Set(pView->m_nMachineStatus +4);
				}
				else // by ckh 2009.02.04 else 추가함. 
				{
					if(pView->m_pDevice->Stage_glass_check()==FALSE )	
					{
						pView->m_pDevice->MNET_Mode_Set(1); //IDLE
					}
					else
					{
						pView->m_pDevice->MNET_Mode_Set(0); //RUN
					}
				}

				m_ctrlTitle.SetBackColor(YELLOW);
				m_ctrlTitle.SetForeColor(LIGHTRED);
				m_ctrlReturn.EnableWindow(true);
				KillTimer(0);
			}
			break;
		case 1 :
			break;
		case 2:
			break;
		default :
			break;
	}	
//
	CDialog::OnTimer(nIDEvent);
}

void CErrMsg::OnOK() 
{
	// TODO: Add extra validation here
	
//	CDialog::OnOK();
}

void CErrMsg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
//	CDialog::OnCancel();
}

BEGIN_EVENTSINK_MAP(CErrMsg, CDialog)
    //{{AFX_EVENTSINK_MAP(CErrMsg)
	ON_EVENT(CErrMsg, IDC_CMD_DO, -600 /* Click */, OnClickCmdDo, VTS_NONE)
	ON_EVENT(CErrMsg, IDC_LABEL_ERRMSG_TITLE, -600 /* Click */, OnClickLabelErrmsgTitle, VTS_NONE)
	ON_EVENT(CErrMsg, IDC_CMD_SAFETY, -600 /* Click */, OnClickCmdSafety, VTS_NONE)
	ON_EVENT(CErrMsg, IDC_CMD_BUZZER_OFF, -600 /* Click */, OnClickCmdBuzzerOff, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CErrMsg::OnClickCmdDo() 
{
	// by ckh 2009.02.16 알람 해제가 안되 UI 화면에 DOWN으로 보고 됨으로 한번 더 해봄.
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(pView->m_nMachineStatus !=-1)
	{
		pView->m_pDevice->MNET_Mode_Set(pView->m_nMachineStatus +4);
	}
	else // by ckh 2009.02.04 else 추가함. 
	{
		// by ckh 2009.03.09
		if(pView->m_pDevice->Stage_glass_check()==FALSE )	
		{
			pView->m_pDevice->MNET_Mode_Set(1); //IDLE
		}
		else
		{
			pView->m_pDevice->MNET_Mode_Set(0); //RUN
		}
	
	}
	
	//////////////////////////////////////////////////////////////////////////
	EndDialog(IDOK);
}

// error code에 따른 message display..
void CErrMsg::err_Message()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc=(CP8CA_LcDispDoc*)pFrame->GetActiveDocument();//2010.06.08 by tskim
	int i=0; WORD wTempValue=0;
	CString str="",strTemp="";
	CString strTemp1="",strTemp2="";
	int iBoardNum=0;
	int iAxisNum=0;
	int RefHead;
//

	if(pView->m_nLanguage == 0)
	{
		switch(m_nErrCode)
		{
			case 1 : m_strMsg2 = "PLCAddr.DAT 파일읽기를 실패하였습니다.";
				break;
			case 2 : m_strMsg2 = "HeadSPositionOffset.DAT 파일읽기를 실패하였습니다.";
				break;
			case 3 : m_strMsg2 = "Comm Port가 정상적으로 Open되지 않았습니다.";
				break;
			case 4 : m_strMsg2 = "Motion BD가 정상적으로 Open되지 않았습니다.";
				break;
			case 5 : m_strMsg2 = "Count BD가 정상적으로 Open되지 않았습니다.";
				break;
			case 6 : m_strMsg2 = "장비가 OffLine 상태입니다.";
				break;
			//
			case 10: 
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + " : 자동운전 중 토출량 측정이 설정치를 초과하여 판정값을 바꿉니다.";
				break;
			case 11 : m_strMsg2.Format("%d번 저울의 문이 열리지 않습니다.",m_nErrCode2); //m_nErrCode2: BAL 번호
				break;
			case 12 : m_strMsg2.Format("%d번 저울의 문이 닫히지 않습니다.",m_nErrCode2);
				break;
			case 13 : m_strMsg2.Format("%d번 저울에 측정cup이 없습니다.",m_nErrCode2);;
				break;
			case 14 : m_strMsg2.Format("%d번 측정cup에 LC양이 너무 많습니다.",m_nErrCode2);
				break;
			case 15 : 
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + " : 자동운전 중 토출량 조정을 실패하였습니다.";
				break;
			case 16 : 
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + " : 자동운전 중 토출량 측정이 설정치를 초과하였습니다.";
				break;
			case 17 : 
	// LC 잔량 감지 센서에 의한 Error 발생 ....
				str="",strTemp="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp.Format("H%d/",i+1);
						str+=strTemp;
					}
				}
				m_strMsg2 = str+" BOTTLE의 LC 용액 양이 부족합니다.(LC 용액 잔량 감지 센서 확인)";
				break;

	// LC 경고량 설정에 의한 Error 발생
			case 18 : 
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d,",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2+" BOTTLE의 LC 용액 양이 부족합니다.(설정치 확인)";
				break;
			case 19 : 
				str="",strTemp="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp.Format("%d, ",i+1);
						str+=strTemp;
					}
				}
				m_strMsg2 = str + "Head의 Drop Count 확인하세요.";				
				break;
			//
			case 20 : 
				str="",strTemp="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp.Format("%d, ",i+1);
						str+=strTemp;
					}
				}
				m_strMsg2 = str + "Drop 방울 감지 센서에 맺힘이 감지 되었습니다.";
				break;
			case 21 : m_strMsg2 = "Loading전 Stage위에 Glass 감지 또는 Glass Exist bit가 ON 되었습니다.";
				break;
			case 22 : m_strMsg2 = "Unloading전 Stage위에 Glass가 감지 되지 않았습니다.";
				break;
			case 23 : m_strMsg2 = "Dropping전 Stage위에 Glass가 감지 되지 않았거나 Glass Exist bit가 Off되었습니다.";
				break;
			case 24 : m_strMsg2 = "Dropping이 완료된 Glass에 다시 Drop 명령이 내려졌습니다.";
				break;
			case 25 : m_strMsg2 = "Dropping이 완료되지 않은  Glass를 Unloading합니다.";
				break;
			case 26 : 
				strTemp.Format("%d Panel",m_nErrCode2);
				m_strMsg2 = strTemp + "의 Add/Del 정보가 설정치를 초과 하였습니다. 정보 변경 후 다시 시작하세요. "; 
				break;
			case 27 : m_strMsg2 = "Glass의 Recipe와 장치에 설정된 Recipe 번호가 같지 않습니다.";
				break;
			case 28 : m_strMsg2 = "Glass의 정보가 없습니다.";
				break;
			case 29 : m_strMsg2 = "ADD/DEL 정보를 담은 File을 Download 할 수 없습니다.";
				break;
			case 30 : m_strMsg2 = "ADD/DEL 정보가 올바르지 않아 Default 값으로 처리하였습니다.";
				break;
			case 31 : 
				strTemp.Format("%d Head",m_nErrCode2);
				m_strMsg2 = strTemp + "Calibration 기준점을 잡는데 실패하였습니다.";
				break;
			case 32 : 
				str="",strTemp="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp.Format("%d, ",i+1);
						str+=strTemp;
					}
				}
				m_strMsg2 = str + "Total Drop Counting이 최소 Drop Counting 설정치 미만입니다.";			
				break;
			case 33 : 
				str="",strTemp="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp.Format("%d, ",i+1);
						str+=strTemp;
					}
				}
				m_strMsg2 = str + "Head의 Barcode값이 일치하지 않습니다. 확인하세요.";			
				break;

			case 34 :
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + " 계산된 Drop수와 설정한 Drop수가 일치하지 않습니다. 확인하세요.";
				break;
	//2013.08.24 by tskim N-controller alarm
			case 40:
				m_strMsg2.Format("H%d N_Controller Connection Alarm...!!.",m_nErrCode2);
				break;
			case 41:
				m_strMsg2.Format("H%d N_Parameter Writting Alarm....!!.",m_nErrCode2);
				break;
			case 42 :
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + "N-Type, Nozzle Status is 'NG', Please Check N-nozzle";
				break;

			// util 관련
			case 50 : m_strMsg2 = "CDA 압력부족으로 GLASS PURGE가 불량합니다.";
				break;
			case 51 : m_strMsg2 = "CDA 압력부족으로 IONIZER BLOW가 불량합니다.";
				break;
			case 52 : m_strMsg2 = "Vacuum 압력부족으로 GLASS흡착이 불량합니다.";
				break;
			case 53 :
				if(m_nErrCode2==1)		m_strMsg2 = "IONIZER1이 Stop 상태 입니다, 상태를 확인 해 주세요 .";
				else if(m_nErrCode2==2)	m_strMsg2 = "IONIZER1에서 생성되는 ION LEVEL이 불량합니다.";
				else if(m_nErrCode2==3)	m_strMsg2 = "IONIZER1 TIP 상태가 불량합니다.";
				else if(m_nErrCode2==4)	m_strMsg2 = "IONIZER2이 Stop 상태 입니다, 상태를 확인 해 주세요.";
				else if(m_nErrCode2==5)	m_strMsg2 = "IONIZER2에서 생성되는 ION LEVEL이 불량합니다.";
				else if(m_nErrCode2==6)	m_strMsg2 = "IONIZER2 TIP 상태가 불량합니다.";		
				break;
			case 59 : // by ckh 2009.02.13
				m_strMsg2 = "피스톤 공급 압력이 부족합니다. 공급 압력을 확인 하세요.";
				break;
			case 60 : // by ckh 2009.02.16
				m_strMsg2 = "Stage에 Glass가 흡착 상태입니다. 흡착 Off 후 실행바랍니다.";
				break;
			case 61 : // by ckh 2009.02.16
				m_strMsg2 = "Stage 퍼지 상태입니다. 퍼지 Off 후 실행바랍니다.";	
				break;
			case 62 : // by ckh 2009.02.16
				m_strMsg2 = "Stage 퍼지 및 흡착 센서 이상 입니다. Stage 공압 센서 확인 후 실행 바랍니다.";	
				break;
			case 63 : // by ckh 2009.02.16
				m_strMsg2 = "Stage 퍼지 공급 압력이 부족합니다. 공급 압력을 확인 하세요.";	
				break;
			// Alarm
			case 100 : m_strMsg2 = "비상정지";
				if(m_nErrCode2==1)		m_strMsg2 = "비상정지 : EMO 1 눌림.";
				else if(m_nErrCode2==2)	m_strMsg2 = "비상정지 : EMO 2 눌림.";
				else if(m_nErrCode2==3)	m_strMsg2 = "비상정지 : EMO 3 눌림.";
				else if(m_nErrCode2==4)	m_strMsg2 = "비상정지 : EMO 4 눌림.";
				else if(m_nErrCode2==5)	m_strMsg2 = "비상정지 : EMO 5 눌림.";
				else if(m_nErrCode2==6)	m_strMsg2 = "비상정지 : EMO 6 눌림.";		
				else if(m_nErrCode2==7)	m_strMsg2 = "비상정지 : EMO 7 눌림.";
				else if(m_nErrCode2==8)	m_strMsg2 = "비상정지 : AREA 1 감지됨.";
				else if(m_nErrCode2==9)	m_strMsg2 = "비상정지 : AREA 2 감지됨.";		
				else if(m_nErrCode2==10)	m_strMsg2 = "비상정지 : AREA 3 감지됨.";
				break;
			case 101 : m_strMsg2 = "장비 운행 도중 안전 커버가 열렸습니다.";
				if(m_nErrCode2==1)		m_strMsg2 = "장비 운행 도중 안전 커버가 열렸습니다. : DOOR 1";
				else if(m_nErrCode2==2)	m_strMsg2 = "장비 운행 도중 안전 커버가 열렸습니다. : DOOR 2";
				else if(m_nErrCode2==3)	m_strMsg2 = "장비 운행 도중 안전 커버가 열렸습니다. : DOOR 3";
				else if(m_nErrCode2==4)	m_strMsg2 = "장비 운행 도중 안전 커버가 열렸습니다. : MAINT DOOR";	
				pView->m_bSafetyOpened=TRUE;
				break;
			case 102 : m_strMsg2 = "장비 운행 도중 Area Sensor가 감지되었습니다.";
				break;
			case 103 : m_strMsg2 = "모든축의 원점 감지가 되지 않았습니다. 원점 감지를 다시 하세요.";
				break;
			case 104 : m_strMsg2 = "작업전이나 도중 Robot arm 센서가 감지 되었습니다. 확인하세요.";  // 안전상의 문제로 arm을 확인하고 작업함. 2003. 01. 10.
				break;
			case 105 : m_strMsg2 = "S축이 정위치로 가지 못했습니다.";
				break;
			case 106 : m_strMsg2 = "Y축이 정위치로 가지 못했습니다.";
				break;
			case 107 : m_strMsg2 = "K축이 정위치로 가지 못했습니다.";
				break;
			case 108 : // by ckh 2009.02.13 반입위치값이 잘못되었을 경우.
				m_strMsg2 = "K축의 반입 위치값이 잘못되었습니다. 티칭값을 수정하세요.";
				break;
			case 109 : // by ckh 2009.02.13 공급위치값이 잘못되었을 경우.
				m_strMsg2 = "K축의 공급 위치값이 잘못되었습니다. 티칭값을 수정하세요.";
				break;
			case 110 : m_strMsg2 = "토출 작업 도중 S축 또는 K축 Limit 감지가 되었습니다. 확인하여 원점 복귀 작업 후 다시 시작하세요.";
				break;
			case 111 : m_strMsg2 = "S축 동기I/O 해제를 실패하였습니다";
				break;
			case 112 : m_strMsg2 = "Count BD가 정상적으로 동작하지 않습니다.";
				break;
			case 113 : // by ckh 2009.02.13 공급위치값이 잘못되었을 경우.
				m_strMsg2 = "K축의 측정 위치값이 잘못되었습니다. 티칭값을 수정하세요.";
				break;
			case 114 : // by ckh 2009.02.13 반입위치값이 잘못되었을 경우.
				m_strMsg2 = "K축의 Mark Align 위치값이 잘못되었습니다. 티칭값을 수정하세요.";
				break;
			case 115 : 
				iBoardNum = m_nErrCode2/0x10;
				iAxisNum  = m_nErrCode2%0x10;
				m_strMsg2.Format("%d Board의 %d Axis이 Servo Alarm 입니다. 전원을 해지하거나 비상 정지 버튼을 누른 후 다시 시작하십시오.",iBoardNum, iAxisNum);
				break;
			case 116 : 
				m_strMsg2 = "K축의 반입위치 센서 ON이 되지 않았습니다. 위치 및 센서 상태 확인하세요.";
				break;
			case 117 :
				m_strMsg2 = "K1, K2 축의 엔코더 값 차이가 큽니다.";
				break;
			case 120 : 
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + " : MEASURING 동작 비정상 완료.";
				break;
			case 121 : 
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + " : ADJUSTING동작 비정상 완료.";
				break;
			case 122 : m_strMsg2 = "토출모터 위치 초기화 실패. PC를 재부팅해 주세요";
				break;
			case 123 : 
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + " : Specout counting 값이 설정치를 초과.";
				break;

			case 130: m_strMsg2.Format("H%d Pump P축 Base기준 위치가 아닙니다.", m_nErrCode2);//m_strMsg2.Format("H%d Pump P축 (-)기준 위치가 아닙니다.", m_nErrCode2);	
				break;
			case 131: m_strMsg2.Format("H%d Pump P축 Suction기준 위치가 아닙니다.", m_nErrCode2);//m_strMsg2.Format("H%d Pump P축 (+)기준 위치가 아닙니다.", m_nErrCode2);	// by shin, 2006.10.21 Volume축 정위치 에러 구문 추가
				break;
			case 135: m_strMsg2.Format("맺힘감지 Up/Down 실린더 상태가 이상합니다...");
				break;
			case 136: m_strMsg2.Format("H%d 피스톤 실린더 센서 상태가 이상합니다...", m_nErrCode2);	// // by shin, 2007.01.03 
				break;	
			case 137: m_strMsg2.Format("H%d Joint Step1을 완료하지 않았습니다...", m_nErrCode2);	// by shin, 2007.01.03 
				break;	
			case 138: m_strMsg2.Format("H%d Joint Step2 작업 중 장착 센서가 작동 되지 않았습니다...", m_nErrCode2);	// // by shin, 2007.01.03 
				break;	
			case 139: m_strMsg2.Format("H%d Pump P축의 Limit Sensor가 감지 되었습니다...", m_nErrCode2);	// // by shin, 2007.01.03 
				break;	
			case 140: m_strMsg2.Format("H%d Pump 용적량을 초과하는 LC양이 계산 되었습니다...", m_nErrCode2);
				break;
			case 141: m_strMsg2.Format("H%d Gate G축 Open 위치가 아닙니다.", m_nErrCode2);	// by shin, 2007.01.03 
				break;
			case 142: m_strMsg2.Format("H%d Gate G축 Close 위치가 아닙니다.", m_nErrCode2);	// by shin, 2007.01.03 
				break;
			case 143: m_strMsg2.Format("H%d Pump 용적 거리를 초과하는 LC양이 계산되었습니다...", m_nErrCode2);
				break;		
			case 144: m_strMsg2.Format("H%d Pump P축 장착 위치가 아닙니다.", m_nErrCode2);	// by shson, 2006.10.11 Volume축 정위치 에러 구문 추가
				break;
			case 145: m_strMsg2.Format("H%d Pump P축 정위치 Error 발생.", m_nErrCode2);	// by shson, 2006.10.11 Volume축 정위치 에러 구문 추가
				break;
			case 146: m_strMsg2.Format("%d번 공급 장치의 주입구의 상태가 이상합니다...",m_nErrCode2);
				break;
			case 147: m_strMsg2.Format("%d번 공급 장치의 Pinch 밸브의 상태가 이상합니다....",m_nErrCode2);
				break;
			case 148: m_strMsg2.Format("%d번 공급 장치의 CDA 상태가 이상합니다. ...",m_nErrCode2);
				break;
			case 149: m_strMsg2.Format("%d번 액정 공급 장치 상태를 체크하십시요...",m_nErrCode2);
				break;
			case 150: m_strMsg2.Format("Safety Interlock이 해제 되었습니다...");
				break;
	//2010.08.18 by tskim alarm 보고 
	//		case 151: m_strMsg2.Format("H%d LC 공급 상한 센서가 감지 되었습니다...", m_nErrCode2);	
	//			break;
			case 152: m_strMsg2.Format("%d번 Canister의 초기 무게 측정 실패...",m_nErrCode2);
				break;
			case 153: m_strMsg2.Format("%d번 Canister의 액정이 비었습니다..",m_nErrCode2);
				break;
			case 154: m_strMsg2.Format("%d번 Canister의 액정 경고량에 도달했습니다...",m_nErrCode2);
				break;
			case 155 : m_strMsg2 = "토출 작업 도중 Line의 X Position 계산값이 이상합니다. 확인하여 원점 복귀 작업 후 다시 시작하세요.";
				break;
			case 156 : m_strMsg2 = "토출 작업 도중 Line의 Y Position 계산값이 이상합니다. 확인하여 원점 복귀 작업 후 다시 시작하세요.";
				break;
			case 157 : m_strMsg2 = "토출 작업 도중 Line의 X Position 이상 발생! 확인하여 원점 복귀 작업 후 다시 시작하세요.";
				break;
			case 158 : m_strMsg2 = "토출 작업 도중 Line의 Y Position 이상 발생! 확인하여 원점 복귀 작업 후 다시 시작하세요.";
				break;
	//2011.01.22 by tskim
			case 159: m_strMsg2.Format("%d번 Canister의 액정 공급량 Limit 이하 알람!!...",m_nErrCode2);
				break;
						
	//Target값과 측정값이 다를 경우 알람 발생..
			case 188:  m_strMsg2 = "타겟량과 측정량이 맞지 않습니다. 조정 완료 후 진행하세요..";
				break;


	//2007.11.09 By tskim
	//반출 대기 시간 1000초 경과 할 경우 알람 발생...
			case 189:
				strTemp1 = "",strTemp2 = "";
				m_strMsg2 = "반출 대기 시간 1000초 초과 하였습니다.";
				break;
			case 190:
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2)
					{
						strTemp1.Format("H%d, PTN#%d (M:%.0f,T:%.0f,P:%.0f)\n",i+1,pView->m_nErrPatternNum+1,pDoc->m_structAdjustCondition.m_dPulseLimit_M[i],Drop_Info.m_dDistPerDrop_ForPulseCheck[i][pView->m_nErrPatternNum],pDoc->m_structAdjustCondition.m_dPulseLimit_P[i]);						
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + " : Target Pulse 값 설정 범위 이탈.";
				break;
			case 191:
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2)
					{
						strTemp1.Format("H%d,(M:%.0f,P:%.0f)\n",i+1,pDoc->m_structAdjustCondition.m_dPulseLimit_M[i],pDoc->m_structAdjustCondition.m_dPulseLimit_P[i]);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + " : Target Pulse 값설정 비정상...";
				break;	
	////////////////////////// 여기서부터는 Warning으로 보고되는 Error 항목입니다. /////////////////////
			case 200 : m_strMsg2 = "운행전 간섭외 신호가 끊어 졌습니다. PLC의 신호를 확인한 후 다시 시작하세요..";
				break;
			case 201 : m_strMsg2 = "Transfer배출 동작 중이므로 진행할 수 없습니다..";
				break;
			case 202 : m_strMsg2 = "Transfer수취 동작 중이므로 진행할 수 없습니다..";
				break;
			case 203 : m_strMsg2 = "Balance Mode가 정상 Mode로 전환이 되었습니다. Balance의 상태가 초기화 되었는지 확인하세요.";
				break;
			case 204 :// by ckh 2009.02.14 알람 1회만 하기위해 ,
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 +(" Dummy Cup 1에 LC양이 너무 많습니다.");	
				break;
			case 205: // by ckh 2009.02.14 알람 1회만 하기위해 
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 +(" Dummy Cup 2에 LC양이 너무 많습니다.");	

				break;
			case 206: m_strMsg2 = "INTERFACE중 NACK CODE로 응답이 왔습니다.";	
				break;
			case 207: m_strMsg2 = "INTERFACE중 응답시간이.. 초과하였습니다.";	
				break;
			case 208: m_strMsg2 = "ECS로부터의 ADD/DEL 정보 이상, Panel 정보를 확인하세요...";	
				break;
			case 209 : m_strMsg2 = "Column K축 모터 주행 거리 초과. 설정값 조정 및 구리스를 주입하여 주세요...";	
				break;
			case 210 : m_strMsg2 = "Head S축 모터 주행 거리 초과. 설정값 조정 및 구리스를 주입하여 주세요...";	
				break;
			case 211 : m_strMsg2 = "Pair Glass의 Seal 도포 완료 신호가 없습니다.확인 하세요.";	
				break;
			case 212 : m_strMsg2 = "[ECS]PD DOWN REQUEST OFF SIGNAL TIMEOVER. CHECK REQUEST BIT"; //141026 PD DOWN : ECS REQUEST BIT 가 OFF되지 않은면 해당 알람이 떠야됨. 
				break; 
			case 246: m_strMsg2.Format("%d번 공급 장치의 주입구의 상태가 이상합니다...",m_nErrCode2);
				break;
			case 247: m_strMsg2.Format("%d번 공급 장치의 Pinch 밸브의 상태가 이상합니다....",m_nErrCode2);
				break;
			case 248: m_strMsg2.Format("%d번 공급 장치의 CDA 상태가 이상합니다. ...",m_nErrCode2);
				break;
			case 249: m_strMsg2.Format("%d번 액정 공급 장치 상태를 체크하십시요...",m_nErrCode2);
				break;
			case 300: m_strMsg2 = "Panel 정보 취득 실패, Panel 정보를 확인하세요...";	
				break;
			case 301: m_strMsg2 = "발란스 내부에 측정 컵이 감지 되었습니다.!!";	
				break;
			case 302: m_strMsg2 = "Mark Align 정도 데이터 획득 실패!!";
				break;
			case 303: m_strMsg2 = "Mark#1 Search 실패!!";
				break;
			case 304: m_strMsg2 = "Mark#2 Search 실패!!";
				break;
			case 305: //2010.06.08 by tskim LC 유의차 (Canister 1~2)
				strTemp1="",strTemp2="";
				//for(i=0; i<MAX_NOZZLE/2 ; i++)
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
						RefHead = i;//2010.06.07 by tskim LC Diff차이 설정치 Display를 위해.
					}
				}
				strTemp1.Empty();
				if(RefHead<MAX_NOZZLE/2)
					strTemp1.Format("LC양 유의차가 Max Head %d (설정치%.0f g)를 초과 하였습니다.",pView->m_nHeadLcMax1+1,pDoc->m_dLCAmountDiff[RefHead/(MAX_NOZZLE/2)]);
				else
					strTemp1.Format("LC양 유의차가 Max Head %d (설정치%.0f g)를 초과 하였습니다.",pView->m_nHeadLcMax2+1,pDoc->m_dLCAmountDiff[RefHead/(MAX_NOZZLE/2)]);
				//m_strMsg2 = strTemp2 +("LC양이 유의차(설정치)를 초과 하였습니다.");
				m_strMsg2 = strTemp2 +strTemp1;
				break;
			case 306: //2010.07.05 by tskim 이전 중공 헤드에 재 중공 지령 방지
				m_strMsg2.Format("직전 중앙 공급 헤드와 중복 됩니다. Head%d",m_nErrCode2);	
				break;
			case 307 : 
	// LC 하한 감지 센서에 의한 Error 발생 ....
				str="",strTemp="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp.Format("H%d/",i+1);
						str+=strTemp;
					}
				}
				m_strMsg2 = str+" BOTTLE의 LC 용액 양이 부족합니다.(LC 용액 하한 감지 센서 확인)";
				break;
			case 308 : 
				str="",strTemp="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp.Format("%d, ",i+1);
						str+=strTemp;
					}
				}
				m_strMsg2 = str + "동일 판넬 사용 Head의 Lot No 불일치.. 확인하세요.";			
				break;
			case 309 : 
				str="",strTemp="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp.Format("%d, ",i+1);
						str+=strTemp;
					}
				}
				m_strMsg2 = str + "Canister Head간 Lot No 불일치.. 확인하세요.";			
				break;
			case 310: 
				str="",strTemp="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp.Format("%d, ",i+1);
						str+=strTemp;
					}
				}
				m_strMsg2 = str + "LINE Drop 작업 중 맺힘이 감지 되었습니다.";
				break; 

			case 311 : m_strMsg2 = "VALC Auto Mode가 아닙니다. 확인하세요.";		
				break;
			case 320 : m_strMsg2 = "FAN STOP. Check Please.";		
				break;
	//2011.04.27 by tskim
			case 400 : m_strMsg2.Format("%d번 지진 모드 Retry Count 초과!!.",m_nErrCode2);
				break;
			case 401: m_strMsg2.Format("Vision Communication Fail.");
				break;
			case 402: m_strMsg2.Format("Vision Writing Fail 'CSV File.' ");
				break;
			case 403: m_strMsg2.Format("Time Over of Reading Vision Count Result");
				break;
			case 404: m_strMsg2.Format("File Reading Fail");
				break;
			case 405: m_strMsg2.Format("There is no File");
				break;
			case 406: m_strMsg2.Format("Vision Responds NG");
				break;
			case 407: m_strMsg2.Format("Cell No is Dismatched");
				break;
			case 408: m_strMsg2.Format("Cell Sequence is wrong");
				break;
			case 409: m_strMsg2.Format("Vision Drop Count Result Error");
				break;
			case 410: m_strMsg2.Format("Vision Send to Dispenser 'ERROR'");
				break;
			case 411: m_strMsg2.Format("Response TimeOver!");
				break;
			case 412: m_strMsg2.Format("Sending Title & Receiving Title is different");
				break;
			case 413:  m_strMsg2.Format("Vision Communication Error!");
				break;
			//140806
			case 414:  m_strMsg2.Format("Dummy Pattern Drop Detected");
				break;
//220603 BIG DROP DETECT ( (OLD) DEEP DROP ) 
			case 415:  m_strMsg2.Format("Big Drop Detected ( Cell )");
				break;
//220603 BIG DROP DETECT ( (OLD) DEEP DROP ) 
			case 500 : m_strMsg2.Format("H%d N DRIVER OVERLOAD ALARM", m_nErrCode2);
				break; 
		
			case 600 :			//ehji 150527
					str="",strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&m_nErrCode2){
							strTemp1.Format("%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					m_strMsg2 = strTemp2 + "DistPerDrop 평균값 초과 !!";
					break;		



				//drop detect alarm
			//140926
			case 700 : m_strMsg2.Format("Head1 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
				break;
			case 701 : m_strMsg2.Format("Head2 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
				break;
			case 702 : m_strMsg2.Format("Head3 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
				break;
			case 703 : m_strMsg2.Format("Head4 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
				break;
			case 704 : m_strMsg2.Format("Head5 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
				break;
			case 705 : m_strMsg2.Format("Head6 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
				break;
			case 706 : m_strMsg2.Format("Head7 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
				break;
			case 707 : m_strMsg2.Format("Head8 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
				break;
			case 708 : m_strMsg2.Format("Head9 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
				break;
			case 709 : m_strMsg2.Format("Head10 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
				break;
			case 710 : m_strMsg2.Format("Head11 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
				break;
			case 711 : m_strMsg2.Format("Head12 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
				break;
			case 712 : m_strMsg2.Format("Head13 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
				break;
			case 713 : m_strMsg2.Format("Head14 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
				break;
			case 714 : m_strMsg2.Format("Head15 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
				break;
			case 715 : m_strMsg2.Format("Head16 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
				break;
	//2014.12.01 by tskim Amp, Duty 연산 관련 오류 Error
			case 800:case 801:case 802:case 803:case 804:case 805:case 806:case 807:
			case 808:case 809:case 810:case 811:case 812:case 813:case 814:case 815: 
				m_strMsg2.Format("Head%d Amplitude, Duty 값이 0 입니다..!",(m_nErrCode-800)+1);
				break;
	//2015.04.29 by tskim 원재료 전산화Add Dummy Drop 보고 
			case 820:
				m_strMsg2.Format("From ECS Dummy Drop 미실시 & Dispesner Dummy 미 실시 Alarm!");
				break;
	//2010.08.18 by tskim
			case 7001:case 7002:case 7003:case 7004:case 7005:case 7006:case 7007:case 7008:
			case 7009:case 7010:case 7011:case 7012:case 7013:case 7014:case 7015:case 7016:
				m_strMsg2.Format("H%d LC 공급 상한 센서가 감지 되었습니다...", m_nErrCode2);	
				break;
		}
	}
	else if(pView->m_nLanguage == 1)
	{
		switch(m_nErrCode)
		{
			case 1 : m_strMsg2 = "PLCAddr.DAT file not founded.";
				break;
			case 2 : m_strMsg2 = "HeadSPositionOffset.DAT file not founded.";
				break;
			case 3 : m_strMsg2 = "Comm Port is not open normaly.";
				break;
			case 4 : m_strMsg2 = "Motion BD is not open normaly.";
				break;
			case 5 : m_strMsg2 = "Count BD is not open normaly.";
				break;
			case 6 : m_strMsg2 = "Machine state Offline.";
				break;
	//
			case 10 :
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				str = strTemp2 + " : During Auto operation, discharge measurement value was exceeded. So set value is replaced.";
				break;

			case 11 : m_strMsg2.Format("Nomber %d of Scales Can't Open.",m_nErrCode2); //nCode2: BAL 번호
				break;
			case 12 : m_strMsg2.Format("Nomber %d of Scales Can't Close.",m_nErrCode2);
				break;
			case 13 : m_strMsg2.Format("%d measurement cup is not in Scales.",m_nErrCode2);;
				break;
			case 14 : m_strMsg2.Format("%d volume of measure cup LC Solution is too much",m_nErrCode2);
				break;
			case 15 :
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + " : During Auto operation, discharge measurement exceeds the set value.";
				break;
			case 16 : 
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + " : During Auto operation, discharge measurement exceeds the set value.";
				break;
			case 17 :
	// LC 잔량 감지 센서에 의한 Error 발생 ....
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d,",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2+" LC of Bottle is lack Please check LC Level detection Sensor ";
				break;
			case 18 : 
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d,",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2+" LC Amount of BOTTLE lack (To confirm setting value)";
				break;
			case 19 :
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + "Please check drop counter of head.";
				break;
			//
			case 20 : 
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				str = strTemp2 + "Before Drop, Dropping detected.";			
				break;
			case 21 : m_strMsg2 = "Loading ago, Glass detection on the stage or Glass Exist bit is ON.";
				break;
			case 22 : m_strMsg2 = "Before unloading, The glass be not checked on the stage .";
				break;
			case 23 : m_strMsg2 = "Before dropping, The glass be not checked on the stage.";
				break;
			case 24 : m_strMsg2 = "Glass of Drop Complete redropped. Please Check.";
				break;
			case 25 : m_strMsg2 = "Not Dropping Glass is into Unloading step. Please Check.";
				break;
			case 26 : 
				m_strMsg2.Format("%d Panel",m_nErrCode2);
				m_strMsg2 += "Add_Del of Panel is abnormal."; 
				break;
			case 27 : m_strMsg2 = "Glass Type in Work Data & Glass Type in Recipe are different. Please check up.";
				break;
			case 28 : m_strMsg2 = "Glass impormation don't exist.";
				break;
			case 29 : m_strMsg2 = "it can't download File having ADD_DEL impormation ";
				break;
			case 30 : m_strMsg2 = "ADD_DEL impormation was incorect. So it change Default Value.";
				break;
			case 31 : 
				m_strMsg2.Format("%d Head",m_nErrCode2);
				m_strMsg2 +="it Failed to Calibration standard point.";
				break;
			case 32 :
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + "Total Drop Counting is low to minimum Drop Counting set value.";				
				break;
			case 33 :
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + "Barcode of Head do not match, Please Check.";				
				break;
			case 34 :
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + " Calculated Drop count and Setting Drop count is different, Please Check.";
				break;

			case 40:
				m_strMsg2.Format("H%d N_Controller Connection Alarm...!!.",m_nErrCode2);
				break;
			case 41:
				m_strMsg2.Format("H%d N_Parameter Writting Alarm....!!.",m_nErrCode2);
				break;
			case 42 :
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + "N-Type, Nozzle Status is 'NG', Please Check N-nozzle";
				break;

			// util 관련
			case 50 : m_strMsg2 = "Due to CDA Lack of pressure, GLASS PURGE is bad.";
				break;
			case 51 : m_strMsg2 = "Due to CDA Lack of pressure, IONIZER BLOW is bad.";
				break;
			case 52 : m_strMsg2 = "Due to Vacuum Lack of pressure, GLASS absorption is bad.";
				break;
			case 53 :
				if(m_nErrCode2==1)		str = "IONIZER1 Status is Stop, Status Check Please.";
				else if(m_nErrCode2==2)	str = "IONIZER1's ION LEVEL is bad.";
				else if(m_nErrCode2==3)	str = "IONIZER1's TIP Condition is bad.";
				else if(m_nErrCode2==4)	str = "IONIZER2 Status is Stop, Status Check Please.";
				else if(m_nErrCode2==5)	str = "IONIZER2's ION LEVEL is bad.";
				else if(m_nErrCode2==6)	str = "IONIZER2's TIP Condition is bad.";		
			case 59 : // by ckh 2009.02.13
				m_strMsg2 = "Pistion pressure is low. Please check.";
				break;	
			case 60 : // by ckh 2009.02.16
				m_strMsg2 = "Glass state on stage is absorption, after absorption off, please operate.";
				break;
			case 61 : // by ckh 2009.02.16
				m_strMsg2 = "Stage fuzzy state. After fuzzy off, Run please.";	
				break;
			case 62 : // by ckh 2009.02.16
				m_strMsg2 = "Stage fuzzy or absorption sensor is abnormal. After checking Stage pneumatic sensor, please operate.";	
				break;
			// Alarm
			case 100 : m_strMsg2 = "Emergency stop";
				if(m_nErrCode2==1)		m_strMsg2 = "Emergency stop : EMO 1 is pressed.";
				else if(m_nErrCode2==2)	m_strMsg2 = "Emergency stop : EMO 2 is pressed.";
				else if(m_nErrCode2==3)	m_strMsg2 = "Emergency stop : EMO 3 is pressed.";
				else if(m_nErrCode2==4)	m_strMsg2 = "Emergency stop : EMO 4 is pressed.";
				else if(m_nErrCode2==5)	m_strMsg2 = "Emergency stop : EMO 5 is pressed.";
				else if(m_nErrCode2==6)	m_strMsg2 = "Emergency stop : EMO 6 is pressed.";		
				else if(m_nErrCode2==7)	m_strMsg2 = "Emergency stop : EMO 7 is pressed.";	
				else if(m_nErrCode2==8)	m_strMsg2 = "Emergency stop : AREA 1 is detected.";
				else if(m_nErrCode2==9)	m_strMsg2 = "Emergency stop : AREA 2 is detected.";		
				else if(m_nErrCode2==10)	m_strMsg2 = "Emergency stop : AREA 3 is detected.";
				break;
			case 101 : m_strMsg2 = "During Equipment operating, Safety cover is opened.";
				if(m_nErrCode2==1)		m_strMsg2 = "During Equipment operating, Safety cover is opened. : DOOR 1";
				else if(m_nErrCode2==2)	m_strMsg2 = "During Equipment operating, Safety cover is opened. : DOOR 2";
				else if(m_nErrCode2==3)	m_strMsg2 = "During Equipment operating, Safety cover is opened. : DOOR 3";
				else if(m_nErrCode2==4)	m_strMsg2 = "During Equipment operating, Safety cover is opened. : MAINT DOOR";	
				break;
			case 102 : m_strMsg2 = "During Equipment operating, Area sensor detect alarm.";
				break;
			case 103 : m_strMsg2 = "the origin detection of all axis was not. Please restart origin detection.";
				break;
			case 104 : m_strMsg2 = "During operation or to be operation, Robot arm sensor is detected. Please check.";  // 안전상의 문제로 arm을 확인하고 작업함. 2003. 01. 10.
				break;
			case 105 : m_strMsg2 = "S Axis don't go in Right position.";
				break;
			case 106 : m_strMsg2 = "Y Axis don't go in Right position.";
				break;
			case 107 : m_strMsg2 = "K Axis don't go in Right position.";
				break;
			case 108 : // by ckh 2009.02.13 반입위치값이 잘못되었을 경우.
				m_strMsg2 = "Import Position value of K Axis is wrong. Please modify teaching value.";
				break;
			case 109 : // by ckh 2009.02.13 공급위치값이 잘못되었을 경우.
				m_strMsg2 = "Supply Position value of K Axis is wrong. Please modify teaching value.";
				break;	
			case 110 : m_strMsg2 = "During dispensing work,  Limit of K or S is detected. Please Check and Please restart after origin return work.";
				break;
			case 111 : m_strMsg2 = "S axis sync I/O remove alarm.";
				break;
			case 112 : m_strMsg2 = "Count BD Don't operate normaly.";
				break;
			case 113 : // by ckh 2009.02.13 공급위치값이 잘못되었을 경우.
				m_strMsg2 = "Measure position value Of K Axis is wrong. Please modify teaching value.";
				break;
			case 115 : 
				iBoardNum = m_nErrCode2/0x10;
				iAxisNum  = m_nErrCode2%0x10;
				m_strMsg2.Format("%d Board %d Axis Servo Alarm. Please Power off or after push emergency button, Please restart.",iBoardNum, iAxisNum);
				break;
			case 117 :
				m_strMsg2 = "Encorder Value of K1, K2 Axis is different";
				break;
			case 120 : 
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + " : MEASURING action is abnormal.";
				break;
			case 121 : 
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + " : ADJUSTING action is abnormal.";
				break;
			case 122 : str = " Dispensing Motor Position initial is Fail. Please restart PC.";
				break;
			case 123 : 
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + " : Specout counting value exceeds set value.";
				break;
			case 130: m_strMsg2.Format("H%d Pump P Axis - Standard Position is not.", m_nErrCode2);
				break;
			case 131: m_strMsg2.Format("H%d Pump P Axis + Standard Position is not.", m_nErrCode2);	// by shin, 2006.10.21 Volume축 정위치 에러 구문 추가
				break;
			case 135: m_strMsg2.Format("There is a problem with the Up/Down cylinder sensor.....");
				break;
			case 136: m_strMsg2.Format("H%d There is a problem with the piston cylinder sensor..", m_nErrCode2);	// // by shin, 2007.01.03 
				break;	
			case 137: m_strMsg2.Format("H%d Joint Step1 was not completed..", m_nErrCode2);	// by shin, 2007.01.03 
				break;	
			case 138: m_strMsg2.Format("H%d During Joint Step2's Operation, mounting sensor do not operate...", m_nErrCode2);	// // by shin, 2007.01.03 
				break;	
			case 139: m_strMsg2.Format("H%d Head Pump P limit sensor detected...", m_nErrCode2);	// // by shin, 2007.01.03 
				break;	
			case 140: m_strMsg2.Format("H%d LC amount be calculated to exceed Pump volume amount..", m_nErrCode2);
				break;
			case 141: m_strMsg2.Format("H%d Gate G Axis Open Position isn't.", m_nErrCode2);	// by shin, 2007.01.03 
				break;
			case 142: m_strMsg2.Format("H%d Gate G Axis Close Position isn't.", m_nErrCode2);	// by shin, 2007.01.03 
				break;
			case 143: m_strMsg2.Format("H%d LC amount be calculated to exceed Pump volume distance...", m_nErrCode2);
				break;		
			case 144: m_strMsg2.Format("H%d Pump P Axis Mounting Position isn't.", m_nErrCode2);	// by shson, 2006.10.11 Volume축 정위치 에러 구문 추가
				break;
			case 145: m_strMsg2.Format("H%d Pump P Axis in position Error.", m_nErrCode2);	// by shson, 2006.10.11 Volume축 정위치 에러 구문 추가
				break;
			case 146: m_strMsg2.Format("Inlet of Supply device %d is abnormal...",m_nErrCode2);
				break;
			case 147: m_strMsg2.Format("pinch valves state of Supply device %d is abnormal....",m_nErrCode2);
				break;
			case 148: m_strMsg2.Format("CDA of Supply device %d is abnormal...",m_nErrCode2);
				break;
			case 149: m_strMsg2.Format("Please check LC supply divice %d...",m_nErrCode2);
				break;
			case 150: str.Format("Safety Interlock was released...");
				break;
	//2010.08.18 by tskim
	//		case 151: str.Format("H%d LC supply maximum sensor was detected...", nCode2);	
	//			break;
			case 152: m_strMsg2.Format("%d Beginning weight measurement of Canister is Fail ...",m_nErrCode2);
				break;
			case 153: m_strMsg2.Format("LC of Canister %d empty..",m_nErrCode2);
				break;
			case 154: m_strMsg2.Format("LC warning Amount of canister %d..",m_nErrCode2);
				break;
			case 155 : m_strMsg2 = "During discharge work, X Position Calculation is wrong. Please Check and Please restart after origin return work.";
				break;
			case 156 : m_strMsg2 = "During discharge work, Y Position Calculation is wrong. Please Check and Please restart after origin return work.";
				break;
			case 157 : m_strMsg2 = "During discharge work, X Position of Line is alarm!! Please Check and Please restart after origin return work.";
				break;
			case 158 : m_strMsg2 = "During discharge work, Y Position of Line is alarm!! Please Check and Please restart after origin return work.";
				break;
	//2011.01.26 by tskim
			case 159: m_strMsg2.Format("LC supply amount Of Canister %d Limit Alarm!!...",m_nErrCode2);
				break;
			
			case 188: m_strMsg2 = "Target amount and measurement different, after complete adjustment, progress ..";
				break;

	//반출 대기 시간 1000초 경과 할 경우 알람 발생...
			case 189:
				strTemp1 = "",strTemp2 = "";
				m_strMsg2 = "Export Wait Time exceed 1000sec..";
				break;


	////////////////////////// 여기서부터는 Warning으로 보고되는 Error 항목입니다. ///////////////////// 
			case 200 : m_strMsg2 = "Before operation interference signal is lost. After Check PLC signal, please restart..";
				break;
			case 201 : m_strMsg2 = "it can't operate, because Transfer relaase operate..";
				break;
			case 202 : m_strMsg2 = "it can't operate, because Transfer reception operate..";
				break;
			case 203 : m_strMsg2 = " Balance Mode changed to normal Mode. Please Check whether state of balance become initialized  or not.";
				break;
			case 204:// by ckh 2009.02.14 알람 1회만 하기위해 ,
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 +(" LC in Dummy Cup 1 is too much.");	
				break;
			case 205: 
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 +("LC in Dummy Cup 2 is too much.");	
				break;
			case 206 : m_strMsg2 = "NACK CODE is ON, when Interfacing.";	
				break;
			case 207 : m_strMsg2 = "During interface, response Time was exceeded.";	
				break;
			case 208 : m_strMsg2 = "ADD/DEL impormation is abnormal From ECS , Please Check Panel impormation....";	
				break;
			case 209 : m_strMsg2 = "Column K Axis motor driving distance is over, Please adjust set value or inject the grease...";	
				break;
			case 210 : m_strMsg2 = "Head S Axis motor driving distance is over, Please adjust set value or inject the grease....";	
				break;
			case 211 : m_strMsg2 = "Seal embrocation Complete signal of Pair Glass isn't. Please Check.";		
				break;
			case 212 : m_strMsg2 = "[ECS]PD DOWN REQUEST OFF SIGNAL TIMEOVER. CHECK REQUEST BIT"; //141026 PD DOWN : ECS REQUEST BIT 가 OFF되지 않은면 해당 알람이 떠야됨. 
				break;
			case 300: m_strMsg2 = "Panel Impormation Failed to acquisition. Please Check Panel impormation...";	
				break;
			case 301: m_strMsg2 = "Measurement cup in Balance insite was detected..!!";	
				break;
			case 302: m_strMsg2 = "To gain Mark Align dgree data is Fail!!";
				break;
			case 303: m_strMsg2 = "Mark#1 Search Fail!!";
				break;
			case 304: m_strMsg2 = "Mark#2 Search Fail!!";
				break;
			case 305: //2010.06.08 by tskim LC 유의차 (Canister 1~2)
				strTemp1="",strTemp2="";
				//for(i=0; i<MAX_NOZZLE/2 ; i++)
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
						RefHead = i;//2010.06.07 by tskim LC Diff차이 설정치 Display를 위해.
					}
				}
				strTemp1.Empty();
				if(RefHead<MAX_NOZZLE/2)
					strTemp1.Format("LC amount different exceeded Max Head %d(set value).",pView->m_nHeadLcMax1+1,pDoc->m_dLCAmountDiff[RefHead/(MAX_NOZZLE/2)]);
				else
					strTemp1.Format("LC amount different exceeded Max Head %d(set value).",pView->m_nHeadLcMax2+1,pDoc->m_dLCAmountDiff[RefHead/(MAX_NOZZLE/2)]);
				//m_strMsg2 = strTemp2 +("LC양이 유의차(설정치)를 초과 하였습니다.");
				m_strMsg2 = strTemp2 +strTemp1;
				break;
			case 306: //2010.07.05 by tskim 이전 중공 헤드에 재 중공 지령 방지
				str.Format("Center Supply Head was Duplicated. Head%d",m_nErrCode2);	
				break;
			case 307 : 
	// LC 하한 감지 센서에 의한 Error 발생 ....
				m_strMsg2="",strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d/",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2+"LC of bottle is low. (please check)";
				break;
			case 308 : 
				m_strMsg2="",strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + "Lot No Of Same Panel Using Head  is inconsistency. Please Check.";			
				break;
			case 309 : 
				m_strMsg2="",strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + "Lot No of Canister Head is inconsistency. Please Check.";			
				break;
			case 310: 
				m_strMsg2="",strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + "LINE Drop 작업 중 맺힘이 감지 되었습니다.";
				break; 

			case 311 : m_strMsg2 = "VALC Auto Mode is not. Check Please.";		
				break;
			case 320 : m_strMsg2 = "FAN STOP. Check Please.";		
				break;
	//2011.04.27 by tskim
			case 400 : m_strMsg2.Format("NO.%d earthquake mode Retry Count excess!!.",m_nErrCode2);
				break;
			case 401: m_strMsg2.Format("Vision Communication Fail.");
				break;
			case 402: m_strMsg2.Format("Vision Writing Fail 'CSV File.' ");
				break;
			case 403: m_strMsg2.Format("Time Over of Reading Vision Count Result");
				break;
			case 404: m_strMsg2.Format("File Reading Fail");
				break;
			case 405: m_strMsg2.Format("There is no File");
				break;
			case 406: m_strMsg2.Format("Vision Responds NG");
				break;
			case 407: m_strMsg2.Format("Cell No is Dismatched");
				break;
			case 408: m_strMsg2.Format("Cell Sequence is wrong");
				break;
			case 409: m_strMsg2.Format("Vision Drop Count Result Error");
				break;
			case 410: m_strMsg2.Format("Vision Send to Dispenser 'ERROR'");
				break;
			case 411: m_strMsg2.Format("Response TimeOver!");
				break;
			case 412: m_strMsg2.Format("Sending Title & Receiving Title is different");
				break;
			case 413: m_strMsg2.Format("Vision Communication Error!");
				break;
			case 414: m_strMsg2.Format("Dummy Pattern Drop Detected");  //ehji
				break;
//220603 BIG DROP DETECT ( (OLD) DEEP DROP ) 
			case 415:  m_strMsg2.Format("Big Drop Detected ( Cell )");
				break;
//220603 BIG DROP DETECT ( (OLD) DEEP DROP ) 
			case 500 : m_strMsg2.Format("H%d N DRIVER OVEROAD ALARM", m_nErrCode2);
				break;
			case 600 :			//ehji 150527
					m_strMsg2="",strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&m_nErrCode2){
							strTemp1.Format("%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					m_strMsg2 = strTemp2 + "DistPerDrop Average Over !!";
					break;

		//drop detect alarm
			//140926
			case 700 : m_strMsg2.Format("Head1 Before Drop, Dropping detected.");
				break;
			case 701 : m_strMsg2.Format("Head2 Before Drop, Dropping detected.");
				break;
			case 702 : m_strMsg2.Format("Head3 Before Drop, Dropping detected.");
				break;
			case 703 : m_strMsg2.Format("Head4 Before Drop, Dropping detected.");
				break;
			case 704 : m_strMsg2.Format("Head5 Before Drop, Dropping detected.");
				break;
			case 705 : m_strMsg2.Format("Head6 Before Drop, Dropping detected.");
				break;
			case 706 : m_strMsg2.Format("Head7 Before Drop, Dropping detected.");
				break;
			case 707 : m_strMsg2.Format("Head8 Before Drop, Dropping detected.");
				break;
			case 708 : m_strMsg2.Format("Head9 Before Drop, Dropping detected.");
				break;
			case 709 : m_strMsg2.Format("Head10 Before Drop, Dropping detected.");
				break;
			case 710 : m_strMsg2.Format("Head11 Before Drop, Dropping detected.");
				break;
			case 711 : m_strMsg2.Format("Head12 Before Drop, Dropping detected.");
				break;
			case 712 : m_strMsg2.Format("Head13 Before Drop, Dropping detected.");
				break;
			case 713 : m_strMsg2.Format("Head14 Before Drop, Dropping detected.");
				break;
			case 714 : m_strMsg2.Format("Head15 Before Drop, Dropping detected.");
				break;
			case 715 : m_strMsg2.Format("Head16 Before Drop, Dropping detected.");
				break;

//2015.04.29 by tskim 원재료 전산화Add Dummy Drop 보고 
		            case 820:
			              m_strMsg2.Format("From ECS Dummy Drop 미실시 & Dispesner Dummy 미 실시 Alarm!");
			            break;					//2014.12.01 by tskim Amp, Duty 연산 관련 오류 Error
			case 800:case 801:case 802:case 803:case 804:case 805:case 806:case 807:
			case 808:case 809:case 810:case 811:case 812:case 813:case 814:case 815: 
			m_strMsg2.Format("Head%d Amplitude, Duty Value is 0..!",(m_nErrCode2-800)+1);
				break;


			case 7001:case 7002:case 7003:case 7004:case 7005:case 7006:case 7007:case 7008:
			case 7009:case 7010:case 7011:case 7012:case 7013:case 7014:case 7015:case 7016:
				m_strMsg2.Format(" Head No.%d LC supply maximum Sensor is detected...", m_nErrCode2);
				break;
			default: m_strMsg2 = "Error Nomber is not exist.!!";
				break;
		}
	}
	else if(pView->m_nLanguage == 2)
	{
		switch(m_nErrCode)
		{
			case 1 : m_strMsg2 = "PLCAddr.DAT 讀取失敗.";
				break;
			case 2 : m_strMsg2 = "HeadSPositionOffset.dat 文件讀取失敗.";
				break;
			case 3 : m_strMsg2 = "Comm Port無法正常Open..";
				break;
			case 4 : m_strMsg2 = "Motion BD無法正常Open..";
				break;
			case 5 : m_strMsg2 = "Count BD無法正常Open.";
				break;
			case 6 : m_strMsg2 = "장비가 OffLine 상태입니다.";
				break;
	//
			case 10 :
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + " : 자동운전 중 토출량 측정이 설정치를 초과하여 판정값을 바꿉니다.";
				break;

			case 11 : m_strMsg2.Format("%d번 저울의 문이 열리지 않습니다.",m_nErrCode2); //nCode2: BAL 번호
				break;
			case 12 : m_strMsg2.Format("%d번 저울의 문이 닫히지 않습니다.",m_nErrCode2);
				break;
			case 13 : m_strMsg2.Format("%d測量儀上無側重cup.",m_nErrCode2);;
				break;
			case 14 : m_strMsg2.Format("%d測量cup里LC量太多.",m_nErrCode2);
				break;
			case 15 :
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + " : 自動運轉中吐出量調整失敗.";
				break;
			case 16 : 
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + " : 自動運轉中吐出量超過了設定?.";
				break;
			case 17 :
	// LC 잔량 감지 센서에 의한 Error 발생 ....
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d,",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2+" BOTTLE內 LC溶液量不足.(確認LC溶液量感應器)";
				break;
			case 18 : 
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d,",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2+" BOTTLE內 LC溶液量不足.(確認設定?)";
				break;
			case 19 :
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + "確認Head的 Drop Count .";
				break;
			//
			case 20 : 
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + "Drop 水珠感應器被感應.";			
				break;
			case 21 : m_strMsg2 = "Loading前Stage上Glass感應或Glass Exist bit ON .";
				break;
			case 22 : m_strMsg2 = "Unloading前Stage上 Glass無感應";
				break;
			case 23 : m_strMsg2 = "Dropping前 Stage上 Glass無感應.";
				break;
			case 24 : m_strMsg2 = "Dropping完成的Glass上再次指令 Drop.";
				break;
			case 25 : m_strMsg2 = "Dropping未完成的Glass Unloading.";
				break;
			case 26 : 
				m_strMsg2.Format("%d Panel",m_nErrCode2);
				m_strMsg2 += " Add/Del 情報異常.."; 
				break;
			case 27 : m_strMsg2 = "Glass的Recipe和設備設定的 Recipe編號不同.";
				break;
			case 28 : m_strMsg2 = "無Glass情報.";
				break;
			case 29 : m_strMsg2 = "ADD/DEL 정보를 담은 File을 Download 할 수 없습니다.";
				break;
			case 30 : m_strMsg2 = "ADD/DEL 정보가 올바르지 않아 Default 값으로 처리하였습니다.";
				break;
			case 31 : 
				m_strMsg2.Format("%d Head",m_nErrCode2);
				m_strMsg2 +="Calibration 기준점을 잡는데 실패하였습니다.";
				break;
			case 32 :
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + "Total Drop Counting이 최소 Drop Counting 설정치 미만입니다.";				
				break;
			case 33 :
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + "Head的 Barcode?不一致請確認.";				
				break;
			case 34 :
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + " 계산된 Drop수와 설정한 Drop수가 일치하지 않습니다. 확인하세요.";
				break;

			case 40:
				m_strMsg2.Format("H%d N_Controller Connection Alarm...!!.",m_nErrCode2);
				break;
			case 41:
				m_strMsg2.Format("H%d N_Parameter Writting Alarm....!!.",m_nErrCode2);
				break;
			case 42 :
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + "N-Type, Nozzle Status is 'NG', Please Check N-nozzle";
				break;

			// util 관련
			case 50 : m_strMsg2 = "CDA 壓力不足導致Glass Purge不良..";
				break;
			case 51 : m_strMsg2 = "CDA 壓力不足導致 IONIZER BLOW不良..";
				break;
			case 52 : m_strMsg2 = "Vacuum 壓力不足導致 GLASS吸附不良.";
				break;
			case 53 :
				if(m_nErrCode2==1)		m_strMsg2 = "IONIZER1이 Stop 상태 입니다, 상태를 확인 해 주세요 .";
				else if(m_nErrCode2==2)	m_strMsg2 = "IONIZER1에서 생성되는 ION LEVEL이 불량합니다.";
				else if(m_nErrCode2==3)	m_strMsg2 = "IONIZER1 TIP 상태가 불량합니다.";
				else if(m_nErrCode2==4)	m_strMsg2 = "IONIZER2이 Stop 상태 입니다, 상태를 확인 해 주세요.";
				else if(m_nErrCode2==5)	m_strMsg2 = "IONIZER2에서 생성되는 ION LEVEL이 불량합니다.";
				else if(m_nErrCode2==6)	m_strMsg2 = "IONIZER2 TIP 상태가 불량합니다.";		
			case 59 : // by ckh 2009.02.13
				m_strMsg2 = "피스톤 공급 압력이 부족합니다. 공급 압력을 확인 하세요.";
				break;	
			case 60 : // by ckh 2009.02.16
				m_strMsg2 = "Stage에 Glass가 흡착 상태입니다. 흡착 Off 후 실행바랍니다.";
				break;
			case 61 : // by ckh 2009.02.16
				m_strMsg2 = "Stage 퍼지 상태입니다. 퍼지 Off 후 실행바랍니다.";	
				break;
			case 62 : // by ckh 2009.02.16
				m_strMsg2 = "Stage 퍼지 및 흡착 센서 이상 입니다. Stage 공압 센서 확인 후 실행 바랍니다.";	
				break;
			// Alarm
			case 100 : m_strMsg2 = "非常停止";
				if(m_nErrCode2==1)		m_strMsg2 = "非常停止 : EMO 1 is pressed.";
				else if(m_nErrCode2==2)	m_strMsg2 = "非常停止 : EMO 2 is pressed.";
				else if(m_nErrCode2==3)	m_strMsg2 = "非常停止 : EMO 3 is pressed.";
				else if(m_nErrCode2==4)	m_strMsg2 = "非常停止 : EMO 4 is pressed.";
				else if(m_nErrCode2==5)	m_strMsg2 = "非常停止 : EMO 5 is pressed.";
				else if(m_nErrCode2==6)	m_strMsg2 = "非常停止 : EMO 6 is pressed.";		
				else if(m_nErrCode2==7)	m_strMsg2 = "非常停止 : EMO 7 is pressed.";	
				else if(m_nErrCode2==8)	m_strMsg2 = "非常停止 : AREA 1 is detected.";
				else if(m_nErrCode2==9)	m_strMsg2 = "非常停止 : AREA 2 is detected.";		
				else if(m_nErrCode2==10)	m_strMsg2 = "非常停止 : AREA 3 is detected.";
				break;
			case 101 : m_strMsg2 = "設備運行中安全門打開. ";
				if(m_nErrCode2==1)		m_strMsg2 = "設備運行中安全門打開. : DOOR 1";
				else if(m_nErrCode2==2)	m_strMsg2 = "設備運行中安全門打開. : DOOR 2";
				else if(m_nErrCode2==3)	m_strMsg2 = "設備運行中安全門打開. : DOOR 3";
				else if(m_nErrCode2==4)	m_strMsg2 = "設備運行中安全門打開. : MAINT DOOR";	
				break;
			case 102 : m_strMsg2 = "設備運行中Area Sensor被感應. ";
				break;
			case 103 : m_strMsg2 = "所有軸原點無感應請重新原點復位.";
				break;
			case 104 : m_strMsg2 = "作業前或作業中Robot arm 感應器被感應請確認";  // 안전상의 문제로 arm을 확인하고 작업함. 2003. 01. 10.
				break;
			case 105 : m_strMsg2 = " S軸沒回到正位置.";
				break;
			case 106 : m_strMsg2 = "Y축이 정위치로 가지 못했습니다.";
				break;
			case 107 : m_strMsg2 = "K軸沒回到正位置.";
				break;
			case 108 : // by ckh 2009.02.13 반입위치값이 잘못되었을 경우.
				m_strMsg2 = "K축의 반입 위치값이 잘못되었습니다. 티칭값을 수정하세요.";
				break;
			case 109 : // by ckh 2009.02.13 공급위치값이 잘못되었을 경우.
				m_strMsg2 = "K축의 공급 위치값이 잘못되었습니다. 티칭값을 수정하세요.";
				break;	
			case 110 : m_strMsg2 = "吐出作業中 K, S 的 Limit 被感應確認后原點復位再?動.";
				break;
			case 111 : m_strMsg2 = "S軸連接I/O 解除失敗";
				break;
			case 112 : m_strMsg2 = "Count BD非正常動作.";
				break;
			case 113 : // by ckh 2009.02.13 공급위치값이 잘못되었을 경우.
				m_strMsg2 = "K축의 측정 위치값이 잘못되었습니다. 티칭값을 수정하세요.";
				break;
			case 115 : 
				iBoardNum = m_nErrCode2/0x10;
				iAxisNum  = m_nErrCode2%0x10;
				m_strMsg2.Format("%d Board的 %d Axis Servo Alarm . 切斷電源或按非常停止后再開始..",iBoardNum, iAxisNum);
				break;
			case 117 :
				m_strMsg2 = "Encorder Value of K1, K2 Axis is different";
				break;
			case 120 : 
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + " : MEASURING 동작 비정상 완료.";
				break;
			case 121 : 
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + " : ADJUSTING동작 비정상 완료.";
				break;
			case 122 : m_strMsg2 = "토출모터 위치 초기화 실패. PC를 재부팅해 주세요";
				break;
			case 123 : 
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + " : Specout counting 값이 설정치를 초과.";
				break;
			case 130: m_strMsg2.Format("H%d Pump P軸 (-)不是基準位置.", m_nErrCode2);
				break;
			case 131: m_strMsg2.Format("H%d Pump P? (+)不是基準位置.", m_nErrCode2);	// by shin, 2006.10.21 Volume축 정위치 에러 구문 추가
				break;
			case 135: m_strMsg2.Format("There is a problem with the Up/Down cylinder sensor.....");
				break;
			case 136: m_strMsg2.Format("H%d Piston氣缸感應器狀態異常...", m_nErrCode2);	// // by shin, 2007.01.03 
				break;	
			case 137: m_strMsg2.Format("H%d Joint Step1을 완료하지 않았습니다...", m_nErrCode2);	// by shin, 2007.01.03 
				break;	
			case 138: m_strMsg2.Format("H%d Joint Step2 作業中安裝感應器無動作....", m_nErrCode2);	// // by shin, 2007.01.03 
				break;	
			case 139: m_strMsg2.Format("H%d Pump P軸的 Limit Sensor感應,..", m_nErrCode2);	// // by shin, 2007.01.03 
				break;	
			case 140: m_strMsg2.Format("H%d 計算出超過Pump溶儲量的LC量..", m_nErrCode2);
				break;
			case 141: m_strMsg2.Format("H%d Gate G軸不在Open位置.", m_nErrCode2);	// by shin, 2007.01.03 
				break;
			case 142: m_strMsg2.Format("H%d Gate G軸不在Close位置.", m_nErrCode2);	// by shin, 2007.01.03 
				break;
			case 143: m_strMsg2.Format("H%d 計算出超過Pump溶儲量的LC量....", m_nErrCode2);
				break;		
			case 144: m_strMsg2.Format("H%d Pump P軸不在安裝位置.", m_nErrCode2);	// by shson, 2006.10.11 Volume축 정위치 에러 구문 추가
				break;
			case 145: m_strMsg2.Format("H%d Pump P軸正位置Error發生.", m_nErrCode2);	// by shson, 2006.10.11 Volume축 정위치 에러 구문 추가
				break;
			case 146: m_strMsg2.Format("%d供給裝置的注入口狀態異常...",m_nErrCode2);
				break;
			case 147: m_strMsg2.Format("%d供給裝置的Pinch閥狀態異常....",m_nErrCode2);
				break;
			case 148: m_strMsg2.Format("%d供給裝置 CDA狀態異常...",m_nErrCode2);
				break;
			case 149: m_strMsg2.Format("%d液晶供給裝置狀態檢?...",m_nErrCode2);
				break;
			case 150: m_strMsg2.Format("Safety Interlock被解除..");
				break;
	//2010.08.18 by tskim
	//		case 151: str.Format("H%d LC 공급 상한 센서가 감지 되었습니다...", nCode2);	
	//			break;
			case 152: m_strMsg2.Format("%d Canister初期稱重測量失敗...",m_nErrCode2);
				break;
			case 153: m_strMsg2.Format("%d Canister內無液晶..",m_nErrCode2);
				break;
			case 154: m_strMsg2.Format("%d Canister內液晶警告量到達...",m_nErrCode2);
				break;
			case 155 : m_strMsg2 = "토출 작업 도중 Line의 X Position 계산값이 이상합니다. 확인하여 원점 복귀 작업 후 다시 시작하세요.";
				break;
			case 156 : m_strMsg2 = "토출 작업 도중 Line의 Y Position 계산값이 이상합니다. 확인하여 원점 복귀 작업 후 다시 시작하세요.";
				break;
			case 157 : m_strMsg2 = "토출 작업 도중 Line의 X Position 이상 발생! 확인하여 원점 복귀 작업 후 다시 시작하세요.";
				break;
			case 158 : m_strMsg2 = "토출 작업 도중 Line의 Y Position 이상 발생! 확인하여 원점 복귀 작업 후 다시 시작하세요.";
				break;
	//2011.01.26 by tskim
			case 159: m_strMsg2.Format("%d번 Canister의 액정 공급량 Limit 이하 알람!!...",m_nErrCode2);
				break;
			
			case 188: m_strMsg2 = "타겟량과 측정량이 맞지 않습니다. 조정 완료 후 진행하세요..";
				break;

	//반출 대기 시간 1000초 경과 할 경우 알람 발생...
			case 189:
				strTemp1 = "",strTemp2 = "";
				str = "반출 대기 시간 1000초 초과 하였습니다.";
				break;


	////////////////////////// 여기서부터는 Warning으로 보고되는 Error 항목입니다. ///////////////////// 
			case 200 : m_strMsg2 = "運行中干涉外信號切斷.確認PLC的信號后在開始.";
				break;
			case 201 : m_strMsg2 = "Transfer배출 동작 중이므로 진행할 수 없습니다..";
				break;
			case 202 : m_strMsg2 = "Transfer수취 동작 중이므로 진행할 수 없습니다..";
				break;
			case 203 : m_strMsg2 = "Balance Mode가 정상 Mode로 전환이 되었습니다. Balance의 상태가 초기화 되었는지 확인하세요.";
				break;
			case 204:// by ckh 2009.02.14 알람 1회만 하기위해 ,
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 +(" Dummy Cup 1內 LC量過多");	
				break;
			case 205: 
				strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 +(" Dummy Cup 2內 LC量過多.");	
				break;
			case 206 : m_strMsg2 = "INTERFACE중 NACK CODE로 응답이 왔습니다.";	
				break;
			case 207 : m_strMsg2 = "INTERFACE中應答時間超過.";	
				break;
			case 208 : m_strMsg2 = " 從ECS 下來的ADD/DEL情報異常, 請確認Pane情報 ...";	
				break;
			case 209 : m_strMsg2 = " Column K軸電機超過行?距離.調整設定?及注入潤滑油...";	
				break;
			case 210 : m_strMsg2 = " Head S軸電機超過行?距離.調整設定?及注入潤滑油...";	
				break;
			case 211 : m_strMsg2 = " 無Pair Glass的Seal 塗佈完了信號,請確認.";		
				break;
			case 212 : m_strMsg2 = "[ECS]PD DOWN REQUEST OFF SIGNAL TIMEOVER. CHECK REQUEST BIT"; //141026 PD DOWN : ECS REQUEST BIT 가 OFF되지 않은면 해당 알람이 떠야됨. 
				break;
			case 300: m_strMsg2 = " Panel 情報讀取失敗, 請確認Panel 情報...";	
				break;
			case 301: m_strMsg2 = "발란스 내부에 측정 컵이 감지 되었습니다.!!";	
				break;
			case 302: m_strMsg2 = "Mark Align 정도 데이터 획득 실패!!";
				break;
			case 303: m_strMsg2 = "Mark#1 Search 실패!!";
				break;
			case 304: m_strMsg2 = "Mark#2 Search 실패!!";
				break;
			case 305: //2010.06.08 by tskim LC 유의차 (Canister 1~2)
				strTemp1="",strTemp2="";
				//for(i=0; i<MAX_NOZZLE/2 ; i++)
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d, ",i+1);
						strTemp2+=strTemp1;
						RefHead = i;//2010.06.07 by tskim LC Diff차이 설정치 Display를 위해.
					}
				}
				strTemp1.Empty();
				if(RefHead<MAX_NOZZLE/2)
					strTemp1.Format("LC양 유의차가 Max Head %d (설정치%.0f g)를 초과 하였습니다.",pView->m_nHeadLcMax1+1,pDoc->m_dLCAmountDiff[RefHead/(MAX_NOZZLE/2)]);
				else
					strTemp1.Format("LC양 유의차가 Max Head %d (설정치%.0f g)를 초과 하였습니다.",pView->m_nHeadLcMax2+1,pDoc->m_dLCAmountDiff[RefHead/(MAX_NOZZLE/2)]);
				//m_strMsg2 = strTemp2 +("LC양이 유의차(설정치)를 초과 하였습니다.");
				m_strMsg2 = strTemp2 +strTemp1;
				break;
			case 306: //2010.07.05 by tskim 이전 중공 헤드에 재 중공 지령 방지
				str.Format("직전 중앙 공급 헤드와 중복 됩니다. Head%d",m_nErrCode2);	
				break;
			case 307 : 
	// LC 하한 감지 센서에 의한 Error 발생 ....
				m_strMsg2="",strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("H%d/",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2+" BOTTLE의 LC 용액 양이 부족합니다.(LC 용액 하한 감지 센서 확인)";
				break;
			case 308 : 
				m_strMsg2="",strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + "동일 판넬 사용 Head의 Lot No 불일치.. 확인하세요.";			
				break;
			case 309 : 
				m_strMsg2="",strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + "Canister Head간 Lot No 불일치.. 확인하세요.";			
				break;
			case 310: 
				m_strMsg2="",strTemp1="",strTemp2="";
				for(i=0; i<MAX_NOZZLE ; i++)
				{
					wTempValue = 0x0001;
					wTempValue = wTempValue << i;
					if(wTempValue&m_nErrCode2){
						strTemp1.Format("%d, ",i+1);
						strTemp2+=strTemp1;
					}
				}
				m_strMsg2 = strTemp2 + "LINE Drop 작업 중 맺힘이 감지 되었습니다.";
				break; 

			case 311 : m_strMsg2 = "VALC Auto Mode is not. Check Please.";		
				break;
			case 320 : m_strMsg2 = "FAN STOP. Check Please.";		
				break;
	//2011.04.27 by tskim
			case 400 : m_strMsg2.Format("%d번 지진 모드 Retry Count 초과!!.",m_nErrCode2);
				break;
			case 401: m_strMsg2.Format("Vision Communication Fail.");
				break;
			case 402: m_strMsg2.Format("Vision Writing Fail 'CSV File.' ");
				break;
			case 403: m_strMsg2.Format("Time Over of Reading Vision Count Result");
				break;
			case 404: m_strMsg2.Format("File Reading Fail");
				break;
			case 405: m_strMsg2.Format("There is no File");
				break;
			case 406: m_strMsg2.Format("Vision Responds NG");
				break;
			case 407: m_strMsg2.Format("Cell No is Dismatched");
				break;
			case 408: m_strMsg2.Format("Cell Sequence is wrong");
				break;
			case 409: m_strMsg2.Format("Vision Drop Count Result Error");
				break;
			case 410: m_strMsg2.Format("Vision Send to Dispenser 'ERROR'");
				break;
			case 411: m_strMsg2.Format("Response TimeOver!");
				break;
			case 412: m_strMsg2.Format("Sending Title & Receiving Title is different");
				break;
			case 413: m_strMsg2.Format("Vision Communication Error!");
				break;
			case 414: m_strMsg2.Format("Dummy Pattern Drop Detected");	//ehji
				break;
//220603 BIG DROP DETECT ( (OLD) DEEP DROP ) 
			case 415:  m_strMsg2.Format("Big Drop Detected ( Cell )");
				break;
//220603 BIG DROP DETECT ( (OLD) DEEP DROP ) 
			case 500 : m_strMsg2.Format("H%d N DRIVER OVEROAD ALARM", m_nErrCode2);
				break; 
			case 600 :			//ehji 150527
					m_strMsg2="",strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&m_nErrCode2){
							strTemp1.Format("%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					m_strMsg2 = strTemp2 + "DistPerDrop Average Over !!";
					break;

		//drop detect alarm
			//140926
			case 700 : m_strMsg2.Format("Head1 Drop 水珠感應器被感應.");
				break;
			case 701 : m_strMsg2.Format("Head2 Drop 水珠感應器被感應.");
				break;
			case 702 : m_strMsg2.Format("Head3 Drop 水珠感應器被感應.");
				break;
			case 703 : m_strMsg2.Format("Head4 Drop 水珠感應器被感應.");
				break;
			case 704 : m_strMsg2.Format("Head5 Drop 水珠感應器被感應.");
				break;
			case 705 : m_strMsg2.Format("Head6 Drop 水珠感應器被感應.");
				break;
			case 706 : m_strMsg2.Format("Head7 Drop 水珠感應器被感應.");
				break;
			case 707 : m_strMsg2.Format("Head8 Drop 水珠感應器被感應.");
				break;
			case 708 : m_strMsg2.Format("Head9 Drop 水珠感應器被感應.");
				break;
			case 709 : m_strMsg2.Format("Head10 Drop 水珠感應器被感應.");
				break;
			case 710 : m_strMsg2.Format("Head11 Drop 水珠感應器被感應.");
				break;
			case 711 : m_strMsg2.Format("Head12 Drop 水珠感應器被感應.");
				break;
			case 712 : m_strMsg2.Format("Head13 Drop 水珠感應器被感應.");
				break;
			case 713 : m_strMsg2.Format("Head14 Drop 水珠感應器被感應.");
				break;
			case 714 : m_strMsg2.Format("Head15 Drop 水珠感應器被感應.");
				break;
			case 715 : m_strMsg2.Format("Head16 Drop 水珠感應器被感應.");
				break;

//2015.04.29 by tskim 원재료 전산화Add Dummy Drop 보고 
		            case 820:
			               m_strMsg2.Format("From ECS Dummy Drop 미실시 & Dispesner Dummy 미 실시 Alarm!");
			            break;
				//2014.12.01 by tskim Amp, Duty 연산 관련 오류 Error
			case 800:case 801:case 802:case 803:case 804:case 805:case 806:case 807:
			case 808:case 809:case 810:case 811:case 812:case 813:case 814:case 815: 
			m_strMsg2.Format("Head%d Amplitude, Duty Value is 0..!",(m_nErrCode2-800)+1);
				break;

			case 7001:case 7002:case 7003:case 7004:case 7005:case 7006:case 7007:case 7008:
			case 7009:case 7010:case 7011:case 7012:case 7013:case 7014:case 7015:case 7016:
				m_strMsg2.Format("H%d LC 공급 상한 센서가 감지 되었습니다...", m_nErrCode2);
				break;
			default: m_strMsg2 = "존재하지 않는 오류 번호 입니다.!!";
				break;
		}
	}






























//
	if ( m_nErrCode == 10 || m_nErrCode == 16 ||
		 m_nErrCode == 20 || m_nErrCode == 310 || m_nErrCode == 409 )
	{
		for( i = 0; i < MAX_NOZZLE; i++ ) 
		{
			wTempValue = 0x0001;
			wTempValue = wTempValue << i;

			if( wTempValue & m_nErrCode2 )
			{
				pView->SaveDB_HistoryLog(atoi(pView->pN_Func->m_structNParam[i].m_strModuleID),i,DB_ALARM,m_nErrCode,m_strMsg2); 
			}
		}
	}


	pView->SaveLog(0,"<Msg>"+m_strMsg2);
}

void CErrMsg::OnClickLabelErrmsgTitle() 
{
//	return;
	// TODO: Add your control notification handler code here
	CTempOP dlg;
	dlg.DoModal();	
}

void CErrMsg::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	// TODO: Add your message handler code here and/or call default
	pView->pTempOp->DoModal();	
	CDialog::OnRButtonDblClk(nFlags, point);
}

void CErrMsg::OnClickCmdSafety() 
{
	// TODO: Add your control notification handler code here
	CSafetyMap dlg;
	dlg.DoModal();
}

void CErrMsg::OnClickCmdBuzzerOff() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispBuzzer, "0000");
}

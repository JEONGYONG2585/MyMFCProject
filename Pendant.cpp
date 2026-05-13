// Pendant.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "Pendant.h"
#include <Windows.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPendant dialog


CPendant::CPendant(CWnd* pParent /*=NULL*/)
	: CDialog(CPendant::IDD, pParent)
{
	m_pThread = NULL;
	m_hThread = NULL;
	m_bConnectionCheck = FALSE;
	g_bThreadRunFlag = FALSE;
	m_bSelectServoOn = FALSE;
	m_bFlagSelectServoOnOff = FALSE;
	m_nSelectAxis = -1;
	m_nSelectAxisBk = -1;
	m_dSelectSpeed = 0;
	for (int nIndex =0; nIndex < LC_MAX_AXIS; nIndex++)
	{
		m_structAxisData[nIndex].bUse = FALSE;
		m_structAxisData[nIndex].bMotionType = FALSE;
		m_structAxisData[nIndex].nAxisNo = FALSE;
		if(nIndex < 5) 
		{
			m_structAxisData[nIndex].wUse_Address = nIndex;
			m_structAxisData[nIndex].wSelectAxis_Address = nIndex+80;
		}
		else	
		{
			m_structAxisData[nIndex].wUse_Address = nIndex+11;
			m_structAxisData[nIndex].wSelectAxis_Address = nIndex+80+11;
		}

	}
	
	//{{AFX_DATA_INIT(CPendant)
	//}}AFX_DATA_INIT
}


void CPendant::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPendant)
	DDX_Control(pDX, IDC_LIST_MESSAGE, m_ctrlListMessage);
	DDX_Control(pDX, IDC_CONNECTION, m_ctrlConnection);
	DDX_Control(pDX, IDC_MSCOMM_PENDANT, m_ctrlCommPendant);
	DDX_Control(pDX, IDC_MESSAGE_BOX, m_ctrlMessageBox);
	DDX_Control(pDX, IDC_POWER, m_ctrlPowerCheck);
	DDX_Control(pDX, IDC_AXIS, m_ctrlSelectAxis);
	DDX_Control(pDX, IDC_SPEED, m_ctrlAxisSpeed);
	DDX_Control(pDX, IDC_SERVO_ON, m_ctrlAxisServoOn);
	DDX_Control(pDX, IDC_ACTUAL_PULSE, m_ctrlAxisActualPulse);
	DDX_Control(pDX, IDC_PENDANT_INIT, m_ctrlInitial);
	DDX_Control(pDX, IDC_POWER_ON, m_ctrlPowerOnOff);
	DDX_Control(pDX, IDC_CMD_DISCONNECT, m_ctrlDisconnect);
	DDX_Control(pDX, IDC_JOG_MODE, m_ctrlJogMode);
	DDX_Control(pDX, IDC_COMMUNICATION, m_ctrlCommunication);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPendant, CDialog)
	//{{AFX_MSG_MAP(CPendant)
	ON_MESSAGE(WM_MESSAGEBOX, AddStringMessageListBox)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(CPendant, CDialog)
    //{{AFX_EVENTSINK_MAP(CPendant)
	ON_EVENT(CPendant, IDC_POWER_ON, -601 /* DblClick */, OnDblClickPowerOn, VTS_DISPATCH)
	ON_EVENT(CPendant, IDC_PENDANT_INIT, -601 /* DblClick */, OnDblClickPendantInit, VTS_DISPATCH)
	ON_EVENT(CPendant, IDC_MSCOMM_PENDANT, 1 /* OnComm */, OnOnCommMscommPendant, VTS_NONE)
	ON_EVENT(CPendant, IDC_CMD_DISCONNECT, -601 /* DblClick */, OnDblClickCmdDisconnect, VTS_DISPATCH)
	ON_EVENT(CPendant, IDC_CMD_RETURN, -600 /* Click */, OnClickCmdReturn, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPendant message handlers
UINT ThreadPendant(LPVOID pParam)
{
	PDTTHREADPARAMS *pThreadParams = (PDTTHREADPARAMS *)pParam;
	CPendant *pPdt = pThreadParams->pPdt;
	delete pThreadParams;
	BOOL bPendantJogMode = FALSE;
	short nJogBit;
	pPdt->g_bThreadRunFlag = TRUE;
	CString str;
	while (!pPdt->m_bPendantThreadExitFlag)
	{
		pPdt->ReadSelectAxis();
		if (pPdt->m_nSelectAxis != pPdt->m_nSelectAxisBk) // 축이 바뀌면 이전축 Servo Off 한다.
		{
			pPdt->SetPendantSingleCoil(0x00A0, FALSE);
			pPdt->AllAxisServoOff();
			if(PENDANT_TEST == TRUE)	pPdt->m_bSelectServoOn = FALSE;
			pPdt->WriteSelectAxisStatus();
			pPdt->m_nSelectAxisBk = pPdt->m_nSelectAxis;
		}
		if (pPdt->m_nSelectAxis > -1)  // 선택된 축이 있으면 축정보를 읽어온다...
		{
			pPdt->ReadSelectAxisStatus();  // Pendant에서 선택축 서보온 상태와 선택축 속도 정보를 읽어 온다.
			pPdt->WriteSelectAxisStatus();  // Pendant에 선택축  정보를 보낸다.
			pPdt->m_bPendantJogMode = pPdt->ReadPendantJogModeBit();
			while (pPdt->m_bPendantJogMode == TRUE)
			{
				pPdt->SetHeartBitOnOff();
				nJogBit = pPdt->ReadPendantJogIO();
				if ((nJogBit == 1))
				{
					FAS_MoveJog(pPdt->m_structAxisData[pPdt->m_nSelectAxis].nAxisNo / pPdt->m_nMaxFasAxis + 1, pPdt->m_structAxisData[pPdt->m_nSelectAxis].nAxisNo%pPdt->m_nMaxFasAxis, 1);
				}
				else if ((nJogBit == 2))
				{
					FAS_MoveJog(pPdt->m_structAxisData[pPdt->m_nSelectAxis].nAxisNo / pPdt->m_nMaxFasAxis + 1, pPdt->m_structAxisData[pPdt->m_nSelectAxis].nAxisNo%pPdt->m_nMaxFasAxis, 0);
				}
				else
				{	
					FAS_MoveStop(pPdt->m_structAxisData[pPdt->m_nSelectAxis].nAxisNo / pPdt->m_nMaxFasAxis + 1, pPdt->m_structAxisData[pPdt->m_nSelectAxis].nAxisNo%pPdt->m_nMaxFasAxis, 0);
				}
				pPdt->m_bPendantJogMode = pPdt->ReadPendantJogModeBit();
				if (pPdt->m_bPendantThreadExitFlag == TRUE) break;
			}
		}
		pPdt->SetHeartBitOnOff();
		Sleep(0);
	}
	pPdt->g_bThreadRunFlag = FALSE;
	return 0;
}
BOOL CPendant::OnInitDialog() 
{
	CDialog::OnInitDialog();

	memset(m_szRcvBuf, 0x00, 200);
	m_bReciveDataOn = FALSE;
	ReadDataInfo();
	AllAxisServoOff();
	AllAxisJogSpeedSet();

	SetTimer(TIMER_UI, 500, NULL);
	SetTimer(TIMER_THREADCHECK, 50, NULL);
	
	
	// All Axis Servo off

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPendant::OnTimer(UINT nIDEvent) 
{
	DWORD ExitCode;
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
		case TIMER_UI: 
			SubTimerUIControll();
			break;
		case TIMER_THREADCHECK:
			if(m_pThread != NULL)
			{
				m_hThread = m_pThread->m_hThread;
				GetExitCodeThread(m_hThread,&ExitCode);
									
				if(ExitCode != STILL_ACTIVE) KillThreadPendant();
			}			
			break;
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CPendant::SubTimerUIControll() 
{
	DWORD dwOutPutIoStatus=0, dwIoBin = 0, wSelectAxisStatusValue=0;
	long AxisStatus=0, AxisStatus1=0;
	CString strSpeed, strActualPulse;
	double dPos1  = 0.0;

	if(ReadPendantConnectionIO() == TRUE) m_ctrlConnection.SetCaption("CONNECTION");
	else m_ctrlConnection.SetCaption("DISCONNECTION");

	int nBit = m_nPowerOnBit%(m_nMaxFasAxis*4);
	if (nBit<32)
	{
		FAS_GetIoOutputStatus(m_nPowerOnBit / (m_nMaxFasAxis*4) + 1, TRUE, &dwOutPutIoStatus);
		dwIoBin = 0x00000001<<nBit;
	}
	else
	{
		FAS_GetIoOutputStatus(m_nPowerOnBit / (m_nMaxFasAxis*4) + 1, FALSE, &dwOutPutIoStatus);
		dwIoBin = 0x00000001<<(nBit-32);
	}
	if (dwOutPutIoStatus & dwIoBin) m_ctrlPowerCheck.SetCaption("POWER_ON");
	else m_ctrlPowerCheck.SetCaption("POWER_OFF");
	

	if (m_nSelectAxis == STAGE_Y) m_ctrlSelectAxis.SetCaption("STAGE_Y");
	else if (m_nSelectAxis == STAGE_X) m_ctrlSelectAxis.SetCaption("STAGE_X");
	else if (m_nSelectAxis == STAGE_TH) m_ctrlSelectAxis.SetCaption("STAGE_TH");
	else if (m_nSelectAxis == GANTRY_L) m_ctrlSelectAxis.SetCaption("GANTRY_L");
	else if (m_nSelectAxis == GANTRY_R) m_ctrlSelectAxis.SetCaption("GANTRY_R");
	else if (m_nSelectAxis == HEAD_S1) m_ctrlSelectAxis.SetCaption("HEAD_S1");
	else if (m_nSelectAxis == HEAD_S2) m_ctrlSelectAxis.SetCaption("HEAD_S2");
	else if (m_nSelectAxis == HEAD_S3) m_ctrlSelectAxis.SetCaption("HEAD_S3");
	else if (m_nSelectAxis == HEAD_S4) m_ctrlSelectAxis.SetCaption("HEAD_S4");
	else if (m_nSelectAxis == HEAD_S5) m_ctrlSelectAxis.SetCaption("HEAD_S5");
	else if (m_nSelectAxis == HEAD_S6) m_ctrlSelectAxis.SetCaption("HEAD_S6");
	else if (m_nSelectAxis == HEAD_S7) m_ctrlSelectAxis.SetCaption("HEAD_S7");
	else if (m_nSelectAxis == HEAD_S8) m_ctrlSelectAxis.SetCaption("HEAD_S8");
	else if (m_nSelectAxis == HEAD_S9) m_ctrlSelectAxis.SetCaption("HEAD_S9");
	else if (m_nSelectAxis == HEAD_S10) m_ctrlSelectAxis.SetCaption("HEAD_S10");
	else if (m_nSelectAxis == HEAD_S11) m_ctrlSelectAxis.SetCaption("HEAD_S11");
	else if (m_nSelectAxis == HEAD_S12) m_ctrlSelectAxis.SetCaption("HEAD_S12");
	else if (m_nSelectAxis == HEAD_S13) m_ctrlSelectAxis.SetCaption("HEAD_S13");
	else if (m_nSelectAxis == HEAD_S14) m_ctrlSelectAxis.SetCaption("HEAD_S14");
	else if (m_nSelectAxis == HEAD_S15) m_ctrlSelectAxis.SetCaption("HEAD_S15");
	else if (m_nSelectAxis == HEAD_S16) m_ctrlSelectAxis.SetCaption("HEAD_S16");
	else if (m_nSelectAxis == HEAD_P1) m_ctrlSelectAxis.SetCaption("HEAD_P1");
	else if (m_nSelectAxis == HEAD_P2) m_ctrlSelectAxis.SetCaption("HEAD_P2");
	else if (m_nSelectAxis == HEAD_P3) m_ctrlSelectAxis.SetCaption("HEAD_P3");
	else if (m_nSelectAxis == HEAD_P4) m_ctrlSelectAxis.SetCaption("HEAD_P4");
	else if (m_nSelectAxis == HEAD_P5) m_ctrlSelectAxis.SetCaption("HEAD_P5");
	else if (m_nSelectAxis == HEAD_P6) m_ctrlSelectAxis.SetCaption("HEAD_P6");
	else if (m_nSelectAxis == HEAD_P7) m_ctrlSelectAxis.SetCaption("HEAD_P7");
	else if (m_nSelectAxis == HEAD_P8) m_ctrlSelectAxis.SetCaption("HEAD_P8");
	else if (m_nSelectAxis == HEAD_P9) m_ctrlSelectAxis.SetCaption("HEAD_P9");
	else if (m_nSelectAxis == HEAD_P10) m_ctrlSelectAxis.SetCaption("HEAD_P10");
	else if (m_nSelectAxis == HEAD_P11) m_ctrlSelectAxis.SetCaption("HEAD_P11");
	else if (m_nSelectAxis == HEAD_P12) m_ctrlSelectAxis.SetCaption("HEAD_P12");
	else if (m_nSelectAxis == HEAD_P13) m_ctrlSelectAxis.SetCaption("HEAD_P13");
	else if (m_nSelectAxis == HEAD_P14) m_ctrlSelectAxis.SetCaption("HEAD_P14");
	else if (m_nSelectAxis == HEAD_P15) m_ctrlSelectAxis.SetCaption("HEAD_P15");
	else if (m_nSelectAxis == HEAD_P16) m_ctrlSelectAxis.SetCaption("HEAD_P16");
	else if (m_nSelectAxis == HEAD_G1) m_ctrlSelectAxis.SetCaption("HEAD_G1");
	else if (m_nSelectAxis == HEAD_G2) m_ctrlSelectAxis.SetCaption("HEAD_G2");
	else if (m_nSelectAxis == HEAD_G3) m_ctrlSelectAxis.SetCaption("HEAD_G3");
	else if (m_nSelectAxis == HEAD_G4) m_ctrlSelectAxis.SetCaption("HEAD_G4");
	else if (m_nSelectAxis == HEAD_G5) m_ctrlSelectAxis.SetCaption("HEAD_G5");
	else if (m_nSelectAxis == HEAD_G6) m_ctrlSelectAxis.SetCaption("HEAD_G6");
	else if (m_nSelectAxis == HEAD_G7) m_ctrlSelectAxis.SetCaption("HEAD_G7");
	else if (m_nSelectAxis == HEAD_G8) m_ctrlSelectAxis.SetCaption("HEAD_G8");
	else if (m_nSelectAxis == HEAD_G9) m_ctrlSelectAxis.SetCaption("HEAD_G9");
	else if (m_nSelectAxis == HEAD_G10) m_ctrlSelectAxis.SetCaption("HEAD_G10");
	else if (m_nSelectAxis == HEAD_G11) m_ctrlSelectAxis.SetCaption("HEAD_G11");
	else if (m_nSelectAxis == HEAD_G12) m_ctrlSelectAxis.SetCaption("HEAD_G12");
	else if (m_nSelectAxis == HEAD_G13) m_ctrlSelectAxis.SetCaption("HEAD_G13");
	else if (m_nSelectAxis == HEAD_G14) m_ctrlSelectAxis.SetCaption("HEAD_G14");
	else if (m_nSelectAxis == HEAD_G15) m_ctrlSelectAxis.SetCaption("HEAD_G15");
	else if (m_nSelectAxis == HEAD_G16) m_ctrlSelectAxis.SetCaption("HEAD_G16");
	else	m_ctrlSelectAxis.SetCaption("NOT SELECT");
	
	strSpeed.Format("%.03f", m_dSelectSpeed);
	m_ctrlAxisSpeed.SetCaption(strSpeed);

	if (m_structAxisData[m_nSelectAxis].bMotionType == 0)
	{
		if (m_nSelectAxis == GANTRY_L)
		{
			FAS_GetAxisStatus(m_nGantryL1AxisNo / m_nMaxFasAxis + 1, m_nGantryL1AxisNo%m_nMaxFasAxis, &AxisStatus);  
			FAS_GetAxisStatus(m_nGantryL2AxisNo / m_nMaxFasAxis + 1, m_nGantryL2AxisNo%m_nMaxFasAxis, &AxisStatus1);  
		}
		else if (m_nSelectAxis == GANTRY_R)
		{
			FAS_GetAxisStatus(m_nGantryR1AxisNo / m_nMaxFasAxis + 1, m_nGantryR1AxisNo%m_nMaxFasAxis, &AxisStatus);  
			FAS_GetAxisStatus(m_nGantryR2AxisNo / m_nMaxFasAxis + 1, m_nGantryR2AxisNo%m_nMaxFasAxis, &AxisStatus1); 
		}
		else
		{
			FAS_GetAxisStatus(m_structAxisData[m_nSelectAxis].nAxisNo / m_nMaxFasAxis + 1, m_structAxisData[m_nSelectAxis].nAxisNo%m_nMaxFasAxis, &AxisStatus);  
			Sleep(100);
		}
		if((AxisStatus|AxisStatus1)&0x00100000) //SERVO_ON
			wSelectAxisStatusValue |= 0x0001;
	}  // 수정 필요
	
	if (wSelectAxisStatusValue == TRUE) 
	{
		m_ctrlAxisServoOn.SetCaption("SERVO ON");
		m_bSelectServoOn = TRUE;
	}
	else 
	{
		m_ctrlAxisServoOn.SetCaption("SERVO OFF");	
		m_bSelectServoOn = FALSE;
	}



	if (m_nSelectAxis > -1)
	{
		if (m_nSelectAxis == GANTRY_L)
		{
			FAS_GetActualPos(m_nGantryL1AxisNo / m_nMaxFasAxis + 1, m_nGantryL1AxisNo%m_nMaxFasAxis, &dPos1);	
		}
		else if (m_nSelectAxis == GANTRY_R)
		{
			FAS_GetActualPos(m_nGantryR1AxisNo / m_nMaxFasAxis + 1, m_nGantryR1AxisNo%m_nMaxFasAxis, &dPos1);	
		}
		else
		{
			FAS_GetActualPos(m_structAxisData[m_nSelectAxis].nAxisNo / m_nMaxFasAxis + 1, m_structAxisData[m_nSelectAxis].nAxisNo%m_nMaxFasAxis, &dPos1);	
		}
		strActualPulse.Format("%.0f",dPos1);
		m_ctrlAxisActualPulse.SetCaption(strActualPulse);
	}
	else m_ctrlAxisActualPulse.SetCaption("0");

	if (g_bThreadRunFlag == TRUE)
	{
		m_ctrlCommunication.SetCaption("RUN");
		if (m_bPendantJogMode == TRUE) m_ctrlJogMode.SetCaption("ON");
		else m_ctrlJogMode.SetCaption("OFF");
	}
	else
	{
		m_ctrlCommunication.SetCaption("STOP");
		m_ctrlJogMode.SetCaption("OFF");
	}
}
void CPendant::AddStringMessageListBox(WPARAM wParam, LPARAM lParam)
{
	CTime time = CTime::GetCurrentTime();
	CString strTime;
	strTime.Format("[%02d:%02d:%02d]   ", time.GetHour(), time.GetMinute(), time.GetSecond());
	m_ctrlListMessage.InsertString(0, strTime + m_strMessageText);
	UpdateData(TRUE);

}
void CPendant::AllAxisJogSpeedSet() 
{
	for (int nIndexBoard = 1; nIndexBoard<=m_nMaxFasBoard; nIndexBoard++)
	{
		for (int nIndexAxis = 0; nIndexAxis<m_nMaxFasAxis; nIndexAxis++)
		{
			FAS_SetLocalPara(nIndexBoard,nIndexAxis,8,1000);
		}
	}
	if (m_bEZRUse)
	{
		for (int nIndexPort = m_nEZRStartPort ; nIndexPort<=m_nEZREndPort; nIndexPort++)
		{
			for (int nIndexSlave = 0; nIndexSlave<m_nEZRUseSlaveNo; nIndexSlave++)
			{
				FAS_SetLocalPara(nIndexPort,nIndexSlave,8,1000); // 수정 필요 EZR 함수로
			}
		}
	}
}
void CPendant::AllAxisServoOff() 
{
	// TODO: Add your message handler code here and/or call default

	for (int nIndexBoard = 1; nIndexBoard<=m_nMaxFasBoard; nIndexBoard++)
	{
		for (int nIndexAxis = 0; nIndexAxis<m_nMaxFasAxis; nIndexAxis++)
		{
			FAS_ServoEnable(nIndexBoard, nIndexAxis, FALSE);
		}
	}
	if (m_bEZRUse)
	{
		for (int nIndexPort = m_nEZRStartPort ; nIndexPort<=m_nEZREndPort; nIndexPort++)
		{
			for (int nIndexSlave = 0; nIndexSlave<m_nEZRUseSlaveNo; nIndexSlave++)
			{
				FAS_ServoEnable(nIndexPort, nIndexSlave, FALSE); // 수정 필요 EZR 함수로
			}
		}
	}
}
BOOL CPendant::ReadPendantConnectionIO()
{
	if(PENDANT_TEST == TRUE)	return TRUE;

	BOOL Io_Result=FALSE;
	int nBit = m_nConnectionBit%(m_nMaxFasAxis*4);

	if (nBit<32)
	{
		FAS_GetIoBit(m_nConnectionBit / (m_nMaxFasAxis*4) + 1, TRUE, (WORD)nBit, &Io_Result);
	}
	else
	{
		FAS_GetIoBit(m_nConnectionBit / (m_nMaxFasAxis*4) + 1, FALSE, (WORD)(nBit - 32), &Io_Result);
	}
	return Io_Result;
}
BOOL CPendant::ReadPendantKeyLockIO()
{
    if(PENDANT_TEST == TRUE)	return TRUE;
	BOOL Io_Result=FALSE;
	int nBit = m_nKeyLockBit%(m_nMaxFasAxis*4);

	if (nBit<32)
	{
		FAS_GetIoBit(m_nKeyLockBit / (m_nMaxFasAxis*4) + 1, TRUE, (WORD)nBit, &Io_Result);
	}
	else
	{
		FAS_GetIoBit(m_nKeyLockBit / (m_nMaxFasAxis*4) + 1, FALSE, (WORD)(nBit - 32), &Io_Result);
	}
	return Io_Result;

}
void CPendant::ReadDataInfo() 
{
	int nIndexAxis = 0;
	m_bEZRUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "USE_BOARD", "EZR_USE");
	m_nMaxFasBoard = INI_ReadInt(PENDANT_INFO_FILE_PATH, "USE_BOARD", "FAS_MAX_BOARD");
	m_nMaxFasAxis = INI_ReadInt(PENDANT_INFO_FILE_PATH, "USE_BOARD", "FAS_MAX_AXIS");
	if (m_bEZRUse)
	{
		m_nEZRStartPort = INI_ReadInt(PENDANT_INFO_FILE_PATH, "USE_BOARD", "EZR_START_PORT");
		m_nEZREndPort = INI_ReadInt(PENDANT_INFO_FILE_PATH, "USE_BOARD", "EZR_END_PORT");
		m_nEZRUseSlaveNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "USE_BOARD", "EZR_USE_SLAVE");
	}

	m_nPendantGripBit = INI_ReadInt(PENDANT_INFO_FILE_PATH, "USE_INPUT_IO", "PENDANT_GRIP_HALF_PUSH");
	m_nJogPlusBit = INI_ReadInt(PENDANT_INFO_FILE_PATH, "USE_INPUT_IO", "PENDANT_JOG_PLUS");
	m_nJogMinusBit = INI_ReadInt(PENDANT_INFO_FILE_PATH, "USE_INPUT_IO", "PENDANT_JOG_MINUS");
	m_nConnectionBit = INI_ReadInt(PENDANT_INFO_FILE_PATH, "USE_INPUT_IO", "PENDANT_CONNECTION");
	m_nKeyLockBit = INI_ReadInt(PENDANT_INFO_FILE_PATH, "USE_INPUT_IO", "PENDANT_KEY_LOCK");

	m_nPowerOnBit = INI_ReadInt(PENDANT_INFO_FILE_PATH, "USE_OUTPUT_IO", "PENDANT_POWER_ON");

	m_structAxisData[STAGE_Y].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "STAGE_Y");
	m_structAxisData[STAGE_X].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "STAGE_X");
	m_structAxisData[STAGE_TH].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "STAGE_TH");
	m_structAxisData[GANTRY_L].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "GANTRY_L");
	m_structAxisData[GANTRY_R].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "GANTRY_R");

	m_structAxisData[HEAD_S1].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_S1");
	m_structAxisData[HEAD_S2].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_S2");
	m_structAxisData[HEAD_S3].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_S3");
	m_structAxisData[HEAD_S4].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_S4");
	m_structAxisData[HEAD_S5].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_S5");
	m_structAxisData[HEAD_S6].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_S6");
	m_structAxisData[HEAD_S7].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_S7");
	m_structAxisData[HEAD_S8].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_S8");
	m_structAxisData[HEAD_S9].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_S9");
	m_structAxisData[HEAD_S10].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_S10");
	m_structAxisData[HEAD_S11].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_S11");
	m_structAxisData[HEAD_S12].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_S12");
	m_structAxisData[HEAD_S13].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_S13");
	m_structAxisData[HEAD_S14].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_S14");
	m_structAxisData[HEAD_S15].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_S15");
	m_structAxisData[HEAD_S16].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_S16");
	
	m_structAxisData[HEAD_P1].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_P1");
	m_structAxisData[HEAD_P2].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_P2");
	m_structAxisData[HEAD_P3].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_P3");
	m_structAxisData[HEAD_P4].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_P4");
	m_structAxisData[HEAD_P5].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_P5");
	m_structAxisData[HEAD_P6].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_P6");
	m_structAxisData[HEAD_P7].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_P7");
	m_structAxisData[HEAD_P8].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_P8");
	m_structAxisData[HEAD_P9].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_P9");
	m_structAxisData[HEAD_P10].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_P10");
	m_structAxisData[HEAD_P11].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_P11");
	m_structAxisData[HEAD_P12].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_P12");
	m_structAxisData[HEAD_P13].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_P13");
	m_structAxisData[HEAD_P14].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_P14");
	m_structAxisData[HEAD_P15].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_P15");
	m_structAxisData[HEAD_P16].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_P16");
	
	m_structAxisData[HEAD_G1].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_G1");
	m_structAxisData[HEAD_G2].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_G2");
	m_structAxisData[HEAD_G3].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_G3");
	m_structAxisData[HEAD_G4].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_G4");
	m_structAxisData[HEAD_G5].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_G5");
	m_structAxisData[HEAD_G6].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_G6");
	m_structAxisData[HEAD_G7].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_G7");
	m_structAxisData[HEAD_G8].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_G8");
	m_structAxisData[HEAD_G9].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_G9");
	m_structAxisData[HEAD_G10].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_G10");
	m_structAxisData[HEAD_G11].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_G11");
	m_structAxisData[HEAD_G12].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_G12");
	m_structAxisData[HEAD_G13].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_G13");
	m_structAxisData[HEAD_G14].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_G14");
	m_structAxisData[HEAD_G15].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_G15");
	m_structAxisData[HEAD_G16].bUse = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_USE", "HEAD_G16");


	m_structAxisData[STAGE_Y].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "STAGE_Y");
	m_structAxisData[STAGE_X].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "STAGE_X");
	m_structAxisData[STAGE_TH].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "STAGE_TH");
	m_structAxisData[GANTRY_L].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "GANTRY_L");
	m_structAxisData[GANTRY_R].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "GANTRY_R");

	m_structAxisData[HEAD_S1].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_S1");
	m_structAxisData[HEAD_S2].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_S2");
	m_structAxisData[HEAD_S3].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_S3");
	m_structAxisData[HEAD_S4].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_S4");
	m_structAxisData[HEAD_S5].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_S5");
	m_structAxisData[HEAD_S6].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_S6");
	m_structAxisData[HEAD_S7].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_S7");
	m_structAxisData[HEAD_S8].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_S8");
	m_structAxisData[HEAD_S9].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_S9");
	m_structAxisData[HEAD_S10].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_S10");
	m_structAxisData[HEAD_S11].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_S11");
	m_structAxisData[HEAD_S12].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_S12");
	m_structAxisData[HEAD_S13].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_S13");
	m_structAxisData[HEAD_S14].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_S14");
	m_structAxisData[HEAD_S15].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_S15");
	m_structAxisData[HEAD_S16].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_S16");
	
	m_structAxisData[HEAD_P1].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_P1");
	m_structAxisData[HEAD_P2].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_P2");
	m_structAxisData[HEAD_P3].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_P3");
	m_structAxisData[HEAD_P4].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_P4");
	m_structAxisData[HEAD_P5].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_P5");
	m_structAxisData[HEAD_P6].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_P6");
	m_structAxisData[HEAD_P7].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_P7");
	m_structAxisData[HEAD_P8].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_P8");
	m_structAxisData[HEAD_P9].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_P9");
	m_structAxisData[HEAD_P10].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_P10");
	m_structAxisData[HEAD_P11].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_P11");
	m_structAxisData[HEAD_P12].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_P12");
	m_structAxisData[HEAD_P13].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_P13");
	m_structAxisData[HEAD_P14].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_P14");
	m_structAxisData[HEAD_P15].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_P15");
	m_structAxisData[HEAD_P16].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_P16");
	
	m_structAxisData[HEAD_G1].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_G1");
	m_structAxisData[HEAD_G2].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_G2");
	m_structAxisData[HEAD_G3].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_G3");
	m_structAxisData[HEAD_G4].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_G4");
	m_structAxisData[HEAD_G5].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_G5");
	m_structAxisData[HEAD_G6].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_G6");
	m_structAxisData[HEAD_G7].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_G7");
	m_structAxisData[HEAD_G8].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_G8");
	m_structAxisData[HEAD_G9].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_G9");
	m_structAxisData[HEAD_G10].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_G10");
	m_structAxisData[HEAD_G11].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_G11");
	m_structAxisData[HEAD_G12].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_G12");
	m_structAxisData[HEAD_G13].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_G13");
	m_structAxisData[HEAD_G14].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_G14");
	m_structAxisData[HEAD_G15].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_G15");
	m_structAxisData[HEAD_G16].bMotionType = INI_ReadBool(PENDANT_INFO_FILE_PATH, "AXIS_TYPE", "HEAD_G16");

	
	m_structAxisData[STAGE_Y].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "STAGE_Y");
	m_structAxisData[STAGE_X].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "STAGE_X");
	m_structAxisData[STAGE_TH].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "STAGE_TH");
	m_structAxisData[GANTRY_L].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "GANTRY_L");
	m_structAxisData[GANTRY_R].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "GANTRY_R");
	m_nGantryL1AxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "GANTRY_L1");  // Gantry 실축 Axis
	m_nGantryL2AxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "GANTRY_L2");  // Gantry 실축 Axis
	m_nGantryR1AxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "GANTRY_R1");  // Gantry 실축 Axis
	m_nGantryR2AxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "GANTRY_R2");  // Gantry 실축 Axis

	m_structAxisData[HEAD_S1].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_S1");
	m_structAxisData[HEAD_S2].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_S2");
	m_structAxisData[HEAD_S3].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_S3");
	m_structAxisData[HEAD_S4].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_S4");
	m_structAxisData[HEAD_S5].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_S5");
	m_structAxisData[HEAD_S6].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_S6");
	m_structAxisData[HEAD_S7].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_S7");
	m_structAxisData[HEAD_S8].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_S8");
	m_structAxisData[HEAD_S9].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_S9");
	m_structAxisData[HEAD_S10].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_S10");
	m_structAxisData[HEAD_S11].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_S11");
	m_structAxisData[HEAD_S12].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_S12");
	m_structAxisData[HEAD_S13].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_S13");
	m_structAxisData[HEAD_S14].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_S14");
	m_structAxisData[HEAD_S15].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_S15");
	m_structAxisData[HEAD_S16].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_S16");
	
	m_structAxisData[HEAD_P1].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_P1");
	m_structAxisData[HEAD_P2].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_P2");
	m_structAxisData[HEAD_P3].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_P3");
	m_structAxisData[HEAD_P4].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_P4");
	m_structAxisData[HEAD_P5].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_P5");
	m_structAxisData[HEAD_P6].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_P6");
	m_structAxisData[HEAD_P7].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_P7");
	m_structAxisData[HEAD_P8].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_P8");
	m_structAxisData[HEAD_P9].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_P9");
	m_structAxisData[HEAD_P10].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_P10");
	m_structAxisData[HEAD_P11].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_P11");
	m_structAxisData[HEAD_P12].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_P12");
	m_structAxisData[HEAD_P13].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_P13");
	m_structAxisData[HEAD_P14].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_P14");
	m_structAxisData[HEAD_P15].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_P15");
	m_structAxisData[HEAD_P16].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_P16");
	
	m_structAxisData[HEAD_G1].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_G1");
	m_structAxisData[HEAD_G2].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_G2");
	m_structAxisData[HEAD_G3].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_G3");
	m_structAxisData[HEAD_G4].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_G4");
	m_structAxisData[HEAD_G5].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_G5");
	m_structAxisData[HEAD_G6].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_G6");
	m_structAxisData[HEAD_G7].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_G7");
	m_structAxisData[HEAD_G8].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_G8");
	m_structAxisData[HEAD_G9].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_G9");
	m_structAxisData[HEAD_G10].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_G10");
	m_structAxisData[HEAD_G11].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_G11");
	m_structAxisData[HEAD_G12].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_G12");
	m_structAxisData[HEAD_G13].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_G13");
	m_structAxisData[HEAD_G14].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_G14");
	m_structAxisData[HEAD_G15].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_G15");
	m_structAxisData[HEAD_G16].nAxisNo = INI_ReadInt(PENDANT_INFO_FILE_PATH, "AXIS_NO", "HEAD_G16");
}


void CPendant::OnDblClickPowerOn(LPDISPATCH Cancel) 
{
	// TODO: Add your control notification handler code here
	if(ReadPendantConnectionIO() == FALSE) 
	{
		AfxMessageBox("Pendant Cable Not Connected");
		return;
	}
	m_strMessageText.Format("MACHINE OUT_PUT Pendant_Power_ON I/O ON.");
	SendMessage(WM_MESSAGEBOX, 0, 0);	
	PendantPowerOnOff(TRUE);

}
void CPendant::PendantPowerOnOff(BOOL bOn) 
{
	// TODO: Add your control notification handler code here
	int nPowerOnBit = INI_ReadInt(PENDANT_INFO_FILE_PATH, "USE_OUTPUT_IO", "PENDANT_POWER_ON");
	int nBit = nPowerOnBit%(m_nMaxFasAxis*4);
	if (nBit<32)
		FAS_SetIoBit(nPowerOnBit / (m_nMaxFasAxis*4) + 1, TRUE, (WORD)nBit, bOn);
	else
		FAS_SetIoBit(nPowerOnBit / (m_nMaxFasAxis*4) + 1, FALSE, (WORD)(nBit - 32), bOn);

}

void CPendant::OnDblClickPendantInit(LPDISPATCH Cancel) 
{
	int nTempTime = 0;
	BtnEnable(FALSE);
	if(m_pThread != NULL)
	{	
		m_bPendantThreadExitFlag = TRUE;
		KillThreadPendant();
	}
	while (g_bThreadRunFlag == TRUE)
	{
		Sleep(1);
		nTempTime++;
		DoEvents();
		if (nTempTime>30000) break;
	}
 	if(ReadPendantConnectionIO() == FALSE) 
 	{
 		AfxMessageBox("Pendant Cable Not Connected");	
 		BtnEnable(TRUE);
 		return;
 	}
 	if(ReadPendantKeyLockIO() == FALSE) 
 	{
 		AfxMessageBox("Pendant Key_UnLock Mode... Please Key_Lock Mode Change!!");
 		BtnEnable(TRUE);
 		return;
	}
	m_strMessageText.Format("Pendant Initial Start");
	SendMessage(WM_MESSAGEBOX, 0, 0);
	AllAxisServoOff();
	m_strMessageText.Format("ALL AXIS SERVO OFF");
	SendMessage(WM_MESSAGEBOX, 0, 0);
	if (!PendantPortOpen()) 
	{
		m_strMessageText.Format("Pendant Communication Port_Open Fail");
		SendMessage(WM_MESSAGEBOX, 0, 0);
		AfxMessageBox("Pendant Communication Fail");
		BtnEnable(TRUE);
		return;
	}
	m_strMessageText.Format("Pendant Communication Port_Open Success");
	SendMessage(WM_MESSAGEBOX, 0, 0);
	SendData_UseAxisInfo();		// 설비 사용 축정보를 팬던트에 전달.
	m_strMessageText.Format("Pendant Initial END");
	SendMessage(WM_MESSAGEBOX, 0, 0);
	BtnEnable(TRUE);
}
void CPendant::BtnEnable(BOOL bEnable) 
{
	if (bEnable == TRUE)
	{
		m_ctrlInitial.SetEnabled(TRUE);
		m_ctrlPowerOnOff.SetEnabled(TRUE);
		m_ctrlDisconnect.SetEnabled(TRUE);
	}
	else
	{
		m_ctrlInitial.SetEnabled(FALSE);
		m_ctrlPowerOnOff.SetEnabled(FALSE);
		m_ctrlDisconnect.SetEnabled(FALSE);
	}
}
void CPendant::SendData_UseAxisInfo() 
{

	m_strMessageText.Format("[EQ->PENDANT] : Use Axis Info Data Send.");
	SendMessage(WM_MESSAGEBOX, 0, 0);
	BOOL bPortOpen = m_ctrlCommPendant.GetPortOpen();
	if (!bPortOpen)
	{
		AfxMessageBox("Pendant Communication Fail");
		return;
	} 
	WORD wStageGantryAxisUseValue = 0x0000; //0000 0000 0001(xxxK2) 0000(K1,TH,X,Y)
	WORD wHeadSAxisUseValue = 0x0000; //1111 1111 1111 1111 (Head16 Head15.... Head1)
	WORD wHeadPAxisUseValue = 0x0000; //1111 1111 1111 1111 (Head16 Head15.... Head1)
	WORD wHeadGAxisUseValue = 0x0000; //1111 1111 1111 1111 (Head16 Head15.... Head1)
	if (m_structAxisData[STAGE_Y].bUse) wStageGantryAxisUseValue |=  0x0001;
	if (m_structAxisData[STAGE_X].bUse) wStageGantryAxisUseValue |=  0x0002;
	if (m_structAxisData[STAGE_TH].bUse) wStageGantryAxisUseValue |= 0x0004;
	if (m_structAxisData[GANTRY_L].bUse) wStageGantryAxisUseValue |= 0x0008;
	if (m_structAxisData[GANTRY_R].bUse) wStageGantryAxisUseValue |= 0x0010;
	SetPendantMultyCoil(m_structAxisData[STAGE_Y].wUse_Address, 5, wStageGantryAxisUseValue);  // Start Address, Address 점수, Byte Value
	Sleep(200);
	if (m_structAxisData[HEAD_S1].bUse) wHeadSAxisUseValue |= 0x0001;
	if (m_structAxisData[HEAD_S2].bUse) wHeadSAxisUseValue |= 0x0002;
	if (m_structAxisData[HEAD_S3].bUse) wHeadSAxisUseValue |= 0x0004;
	if (m_structAxisData[HEAD_S4].bUse) wHeadSAxisUseValue |= 0x0008;
	if (m_structAxisData[HEAD_S5].bUse) wHeadSAxisUseValue |= 0x0010;
	if (m_structAxisData[HEAD_S6].bUse) wHeadSAxisUseValue |= 0x0020;
	if (m_structAxisData[HEAD_S7].bUse) wHeadSAxisUseValue |= 0x0040;
	if (m_structAxisData[HEAD_S8].bUse) wHeadSAxisUseValue |= 0x0080;
	if (m_structAxisData[HEAD_S9].bUse) wHeadSAxisUseValue |= 0x0100;
	if (m_structAxisData[HEAD_S10].bUse) wHeadSAxisUseValue |= 0x0200;
	if (m_structAxisData[HEAD_S11].bUse) wHeadSAxisUseValue |= 0x0400;
	if (m_structAxisData[HEAD_S12].bUse) wHeadSAxisUseValue |= 0x0800;
	if (m_structAxisData[HEAD_S13].bUse) wHeadSAxisUseValue |= 0x1000;
	if (m_structAxisData[HEAD_S14].bUse) wHeadSAxisUseValue |= 0x2000;
	if (m_structAxisData[HEAD_S15].bUse) wHeadSAxisUseValue |= 0x4000;
	if (m_structAxisData[HEAD_S16].bUse) wHeadSAxisUseValue |= 0x8000;
	SetPendantMultyCoil(m_structAxisData[HEAD_S1].wUse_Address, 16, wHeadSAxisUseValue); // Start Address, Address 점수, Byte Value
	Sleep(200);	
	if (m_structAxisData[HEAD_P1].bUse) wHeadPAxisUseValue |= 0x0001;
	if (m_structAxisData[HEAD_P2].bUse) wHeadPAxisUseValue |= 0x0002;
	if (m_structAxisData[HEAD_P3].bUse) wHeadPAxisUseValue |= 0x0004;
	if (m_structAxisData[HEAD_P4].bUse) wHeadPAxisUseValue |= 0x0008;
	if (m_structAxisData[HEAD_P5].bUse) wHeadPAxisUseValue |= 0x0010;
	if (m_structAxisData[HEAD_P6].bUse) wHeadPAxisUseValue |= 0x0020;
	if (m_structAxisData[HEAD_P7].bUse) wHeadPAxisUseValue |= 0x0040;
	if (m_structAxisData[HEAD_P8].bUse) wHeadPAxisUseValue |= 0x0080;
	if (m_structAxisData[HEAD_P9].bUse) wHeadPAxisUseValue |= 0x0100;
	if (m_structAxisData[HEAD_P10].bUse) wHeadPAxisUseValue |= 0x0200;
	if (m_structAxisData[HEAD_P11].bUse) wHeadPAxisUseValue |= 0x0400;
	if (m_structAxisData[HEAD_P12].bUse) wHeadPAxisUseValue |= 0x0800;
	if (m_structAxisData[HEAD_P13].bUse) wHeadPAxisUseValue |= 0x1000;
	if (m_structAxisData[HEAD_P14].bUse) wHeadPAxisUseValue |= 0x2000;
	if (m_structAxisData[HEAD_P15].bUse) wHeadPAxisUseValue |= 0x4000;
	if (m_structAxisData[HEAD_P16].bUse) wHeadPAxisUseValue |= 0x8000;
	SetPendantMultyCoil(m_structAxisData[HEAD_P1].wUse_Address, 16, wHeadPAxisUseValue); // Start Address, Address 점수, Byte Value
	Sleep(200);	
	if (m_structAxisData[HEAD_G1].bUse) wHeadGAxisUseValue |= 0x0001;
	if (m_structAxisData[HEAD_G2].bUse) wHeadGAxisUseValue |= 0x0002;
	if (m_structAxisData[HEAD_G3].bUse) wHeadGAxisUseValue |= 0x0004;
	if (m_structAxisData[HEAD_G4].bUse) wHeadGAxisUseValue |= 0x0008;
	if (m_structAxisData[HEAD_G5].bUse) wHeadGAxisUseValue |= 0x0010;
	if (m_structAxisData[HEAD_G6].bUse) wHeadGAxisUseValue |= 0x0020;
	if (m_structAxisData[HEAD_G7].bUse) wHeadGAxisUseValue |= 0x0040;
	if (m_structAxisData[HEAD_G8].bUse) wHeadGAxisUseValue |= 0x0080;
	if (m_structAxisData[HEAD_G9].bUse) wHeadGAxisUseValue |= 0x0100;
	if (m_structAxisData[HEAD_G10].bUse) wHeadGAxisUseValue |= 0x0200;
	if (m_structAxisData[HEAD_G11].bUse) wHeadGAxisUseValue |= 0x0400;
	if (m_structAxisData[HEAD_G12].bUse) wHeadGAxisUseValue |= 0x0800;
	if (m_structAxisData[HEAD_G13].bUse) wHeadGAxisUseValue |= 0x1000;
	if (m_structAxisData[HEAD_G14].bUse) wHeadGAxisUseValue |= 0x2000;
	if (m_structAxisData[HEAD_G15].bUse) wHeadGAxisUseValue |= 0x4000;
	if (m_structAxisData[HEAD_G16].bUse) wHeadGAxisUseValue |= 0x8000;
	SetPendantMultyCoil(m_structAxisData[HEAD_G1].wUse_Address, 16, wHeadGAxisUseValue); // Start Address, Address 점수, Byte Value
	Sleep(200);	

	if(m_pThread == NULL) 
	{
		RunThreadPendant();
		m_strMessageText.Format("Pendant Thread Funtion Start");
		SendMessage(WM_MESSAGEBOX, 0, 0);
	}
	
}

BOOL CPendant::PendantPortOpen() 
{
	BOOL bResult = FALSE;
	CString	strInitInfo ="";
	CString	strPortName ="";
	short nNewValue = 0;
	TCHAR lpTargetPath[5000]; 
	DWORD wComResult;
	int nPort_num = INI_ReadInt(PENDANT_INFO_FILE_PATH, "USE_BOARD", "USE_SERIAL_PORT_NO");
	nNewValue = (short)(nPort_num) ;
	strPortName.Format("COM%d",nNewValue);
	wComResult = QueryDosDevice(strPortName,(LPSTR)lpTargetPath, 5000);
	if (wComResult==0) 
	{
		m_strMessageText.Format("HAVE NO COM%d PORT", nNewValue);
		SendMessage(WM_MESSAGEBOX, 0, 0);
		return FALSE;
	}
	strInitInfo.Format("%d,%c,%d,%d",115200,'N',8,1);
	BOOL bPortOpen = m_ctrlCommPendant.GetPortOpen();
	if (bPortOpen)
	{
		Sleep(500);
		m_ctrlCommPendant.SetPortOpen( FALSE );
	}

	m_ctrlCommPendant.SetCommPort( nNewValue );
	m_ctrlCommPendant.SetSettings(strInitInfo);
	m_ctrlCommPendant.SetInputMode( 1 );	// comminputMode: Binary ; 0:TEXT 1:Binary
	m_ctrlCommPendant.SetRThreshold( 1 ); // every time event 
	m_ctrlCommPendant.SetInputLen(0);     // receive entry data
	m_ctrlCommPendant.SetPortOpen( true );
	bPortOpen = m_ctrlCommPendant.GetPortOpen();
	bResult = bPortOpen;
	return bResult;
}

void CPendant::OnOnCommMscommPendant() 
{
	// TODO: Add your control notification handler code here
	short			evNum ;
	VARIANT			var;
	int				cnt;
	unsigned char HUGEP	*pData;
	memset(m_szRcvBuf, 0x00, 200);

	evNum = m_ctrlCommPendant.GetCommEvent();

	switch( evNum ) //switch 00 start //GetCommEvent();
	{
		case 2:	// comEvReceive	
			
			Sleep(10);
			cnt = m_ctrlCommPendant.GetInBufferCount();

			var = m_ctrlCommPendant.GetInput();

			SafeArrayAccessData( var.parray,(void HUGEP* FAR*)&pData );
			memcpy(m_szRcvBuf, pData, cnt);
			SafeArrayUnaccessData(var.parray);
			m_bReciveDataOn = TRUE;
			break;
	}
}
void CPendant::DoEvents()
{
	MSG	msg;

	while(::PeekMessage( &msg,NULL,0,0,PM_NOREMOVE ) )
	{
		if( !AfxGetThread()->PumpMessage() )
			return;		
	}
}


void CPendant::RunThreadPendant()
{
	if(m_pThread == NULL)
	{	
		PDTTHREADPARAMS *pThreadParams = new PDTTHREADPARAMS;
		pThreadParams->pPdt = this;
		Sleep(100);
	
		m_bPendantThreadExitFlag = FALSE;
		m_pThread = AfxBeginThread((AFX_THREADPROC)ThreadPendant, pThreadParams);

	}
	else// if(m_pThread[nID] != NULL)
	{
		AfxMessageBox(" Thread is notting");
	}
}


void CPendant::KillThreadPendant()
{
	if(m_pThread != NULL)
	{
		m_pThread = NULL;
	}
}
void CPendant::SetHeartBitOnOff()
{
	if (m_bHeartBitFlag == TRUE)
	{
		m_bHeartBitFlag = FALSE;
		SetPendantSingleCoil(0x00B0, FALSE);
	}
	else
	{
		m_bHeartBitFlag = TRUE;
		SetPendantSingleCoil(0x00B0, TRUE);
	}
}
void CPendant::WriteSelectAxisStatus()
{
	long AxisStatus=0, AxisStatus1=0;
	double dPos1 = 0.0;
	WORD wSelectAxisStatusValue = 0, wSelectAxisActualPulseHigh = 0, wSelectAxisActualPulseLow = 0;
	DWORD wSelectAxisActualPulse =0;

	if (m_nSelectAxis == -1)
	{
		SetPendantMultyCoil(0x00A1, 4, 0); // Start Address, Address 점수, Byte Value
		SetPendantSingleRegister(0x0190, 0);
		SetPendantSingleRegister(0x0191, 0);
		return;
	}
	if (m_structAxisData[m_nSelectAxis].bMotionType == 0)
	{
		if (m_nSelectAxis == GANTRY_L)
		{
			FAS_GetAxisStatus(m_nGantryL1AxisNo / m_nMaxFasAxis + 1, m_nGantryL1AxisNo%m_nMaxFasAxis, &AxisStatus);  
			FAS_GetAxisStatus(m_nGantryL2AxisNo / m_nMaxFasAxis + 1, m_nGantryL2AxisNo%m_nMaxFasAxis, &AxisStatus1);  
			FAS_GetActualPos(m_nGantryL1AxisNo / m_nMaxFasAxis + 1, m_nGantryL1AxisNo%m_nMaxFasAxis, &dPos1);
		}
		else if (m_nSelectAxis == GANTRY_R)
		{
			FAS_GetAxisStatus(m_nGantryR1AxisNo / m_nMaxFasAxis + 1, m_nGantryR1AxisNo%m_nMaxFasAxis, &AxisStatus);  
			FAS_GetAxisStatus(m_nGantryR2AxisNo / m_nMaxFasAxis + 1, m_nGantryR2AxisNo%m_nMaxFasAxis, &AxisStatus1);  
			FAS_GetActualPos(m_nGantryR1AxisNo / m_nMaxFasAxis + 1, m_nGantryR1AxisNo%m_nMaxFasAxis, &dPos1);
		}
		else
		{
			FAS_GetAxisStatus(m_structAxisData[m_nSelectAxis].nAxisNo / m_nMaxFasAxis + 1, m_structAxisData[m_nSelectAxis].nAxisNo%m_nMaxFasAxis, &AxisStatus);  
			Sleep(100);
			FAS_GetActualPos(m_structAxisData[m_nSelectAxis].nAxisNo / m_nMaxFasAxis + 1, m_structAxisData[m_nSelectAxis].nAxisNo%m_nMaxFasAxis, &dPos1);	
			Sleep(100);
		}
		if((AxisStatus|AxisStatus1)&0x00100000) //SERVO_ON
			wSelectAxisStatusValue |= 0x0001;
		if((AxisStatus|AxisStatus1)&0x00000002) //SERVOALARM
			wSelectAxisStatusValue |= 0x0002;
		if((AxisStatus|AxisStatus1)&0x00000008) //LM_PLUS_DETECT
			wSelectAxisStatusValue |= 0x0004;
		if((AxisStatus|AxisStatus1)&0x00000010) //LM_MINUS_DETECT
			wSelectAxisStatusValue |= 0x0008;
		
		wSelectAxisActualPulse = (long)dPos1;
		wSelectAxisActualPulseHigh = (long)(wSelectAxisActualPulse & 0xffff0000)>>16;
		wSelectAxisActualPulseLow = (long)wSelectAxisActualPulse & 0x0000ffff;
		if(PENDANT_TEST == TRUE)
		{
			if (m_bSelectServoOn == TRUE)
			{
				wSelectAxisStatusValue = 1;
			}
		}	
		SetPendantMultyCoil(0x00A1, 4, wSelectAxisStatusValue); // Start Address, Address 점수, Byte Value
		SetPendantSingleRegister(0x0190, wSelectAxisActualPulseLow);
		SetPendantSingleRegister(0x0191, wSelectAxisActualPulseHigh);

	}
	else   //EZR 일경우 수정 필요....
	{
		if (m_nSelectAxis == GANTRY_L)
		{
			FAS_GetAxisStatus(m_nGantryL1AxisNo / m_nMaxFasAxis + 1, m_nGantryL1AxisNo%m_nMaxFasAxis, &AxisStatus);  
			FAS_GetAxisStatus(m_nGantryL2AxisNo / m_nMaxFasAxis + 1, m_nGantryL2AxisNo%m_nMaxFasAxis, &AxisStatus1);  
			FAS_GetActualPos(m_nGantryL1AxisNo / m_nMaxFasAxis + 1, m_nGantryL1AxisNo%m_nMaxFasAxis, &dPos1);
		}
		else if (m_nSelectAxis == GANTRY_R)
		{
			FAS_GetAxisStatus(m_nGantryR1AxisNo / m_nMaxFasAxis + 1, m_nGantryR1AxisNo%m_nMaxFasAxis, &AxisStatus);  
			FAS_GetAxisStatus(m_nGantryR2AxisNo / m_nMaxFasAxis + 1, m_nGantryR2AxisNo%m_nMaxFasAxis, &AxisStatus1);  
			FAS_GetActualPos(m_nGantryR1AxisNo / m_nMaxFasAxis + 1, m_nGantryR1AxisNo%m_nMaxFasAxis, &dPos1);
		}
		else
		{
			FAS_GetAxisStatus(m_structAxisData[m_nSelectAxis].nAxisNo / m_nMaxFasAxis + 1, m_structAxisData[m_nSelectAxis].nAxisNo%m_nMaxFasAxis, &AxisStatus);  
			FAS_GetActualPos(m_structAxisData[m_nSelectAxis].nAxisNo / m_nMaxFasAxis + 1, m_structAxisData[m_nSelectAxis].nAxisNo%m_nMaxFasAxis, &dPos1);	
		}
		if((AxisStatus|AxisStatus1)&0x00100000) //SERVO_ON
			wSelectAxisStatusValue |= 0x0001;
		if((AxisStatus|AxisStatus1)&0x00000002) //SERVOALARM
			wSelectAxisStatusValue |= 0x0002;
		if((AxisStatus|AxisStatus1)&0x00000008) //LM_PLUS_DETECT
			wSelectAxisStatusValue |= 0x0004;
		if((AxisStatus|AxisStatus1)&0x00000010) //LM_MINUS_DETECT
			wSelectAxisStatusValue |= 0x0008;

		wSelectAxisActualPulse = (long)dPos1;
		wSelectAxisActualPulseHigh = (long)(wSelectAxisActualPulse & 0xffff0000)>>16;
		wSelectAxisActualPulseLow = (long)wSelectAxisActualPulse & 0x0000ffff;
		if(PENDANT_TEST == TRUE)
		{
			if (m_bSelectServoOn == TRUE)
			{
				wSelectAxisStatusValue = 1;
			}
		}
		SetPendantMultyCoil(0x00A1, 4, wSelectAxisStatusValue); // Start Address, Address 점수, Byte Value
		SetPendantSingleRegister(0x0190, wSelectAxisActualPulseLow);
		SetPendantSingleRegister(0x0191, wSelectAxisActualPulseHigh);

	}
	if (ReadPendantKeyLockIO()==TRUE)
	{
		SetPendantSingleCoil(0x00D0, TRUE);
	}
	else
	{
		SetPendantSingleCoil(0x00D0, FALSE);
	}
	
}
int CPendant::ReadPendantJogIO()
{		
	BOOL Io_Result=FALSE;
	int nGripBit = m_nPendantGripBit%(m_nMaxFasAxis*4);
	int nPlusBit = m_nJogPlusBit%(m_nMaxFasAxis*4);	
	int nMinusBit = m_nJogMinusBit%(m_nMaxFasAxis*4);

	if (ReadPendantKeyLockIO() == FALSE)
	{
		return 0;
	}
	
	if (nGripBit<32)  // Grip Switch가 반 눌렸을 경우만 동작 되도록... 안눌렸으면 그냥 리턴 시키자..
	{
		FAS_GetIoBit(m_nPendantGripBit / (m_nMaxFasAxis*4) + 1, TRUE, (WORD)nGripBit, &Io_Result);
		if (Io_Result == FALSE)
		{
			return 0;
		}
	}
	else
	{
		FAS_GetIoBit(m_nPendantGripBit / (m_nMaxFasAxis*4) + 1, FALSE, (WORD)(nGripBit - 32), &Io_Result);
		if (Io_Result == FALSE)
		{
			return 0;
		}
	}


	if (nPlusBit<32)
	{
		FAS_GetIoBit(m_nJogPlusBit / (m_nMaxFasAxis*4) + 1, TRUE, (WORD)nPlusBit, &Io_Result);
		if (Io_Result == TRUE)
		{
			return 1;
		}
	}
	else
	{
		FAS_GetIoBit(m_nJogPlusBit / (m_nMaxFasAxis*4) + 1, FALSE, (WORD)(nPlusBit - 32), &Io_Result);
		if (Io_Result == TRUE)
		{
			return 1;
		}
	}

	if (nMinusBit<32)
	{
		FAS_GetIoBit(m_nJogMinusBit / (m_nMaxFasAxis*4) + 1, TRUE, (WORD)nMinusBit, &Io_Result);
		if (Io_Result == TRUE)
		{
			return 2;
		}
	}
	else
	{
		FAS_GetIoBit(m_nJogMinusBit / (m_nMaxFasAxis*4) + 1, FALSE, (WORD)(nMinusBit - 32), &Io_Result);
		if (Io_Result == TRUE)
		{
			return 2;
		}
	}
	return 0;
}
BOOL CPendant::ReadPendantJogModeBit()
{
	if (GetPendantCoil(0x00C0, 0x0001) == 1) return TRUE;// Coil Address(Servo On/Off Select) 193번 Coile Read... 
	else return FALSE;//
}
void CPendant::ReadSelectAxisStatus()
{
 	if (GetPendantCoil(0x00A0, 0x0001) == 1) m_bFlagSelectServoOnOff = TRUE;// Coil Address(Servo On/Off Select) 161번 Coile Read... 
	else 
	{
		m_bFlagSelectServoOnOff = FALSE;
	}
	if (m_bFlagSelectServoOnOff == TRUE)
	{
		if (m_bSelectServoOn == FALSE)
		{		
			if (m_structAxisData[m_nSelectAxis].bMotionType == 0) //Motion 보드 사용일 경우...
			{
				if ((m_nSelectAxis == GANTRY_L) || (m_nSelectAxis == GANTRY_R))
				{
					if (m_nSelectAxis == GANTRY_L)
					{
						FAS_ServoEnable(m_nGantryL1AxisNo / m_nMaxFasAxis + 1, m_nGantryL1AxisNo%m_nMaxFasAxis, TRUE);
						FAS_ServoEnable(m_nGantryL2AxisNo / m_nMaxFasAxis + 1, m_nGantryL2AxisNo%m_nMaxFasAxis, TRUE);
						FAS_ServoEnable(m_structAxisData[m_nSelectAxis].nAxisNo / m_nMaxFasAxis + 1, m_structAxisData[m_nSelectAxis].nAxisNo%m_nMaxFasAxis, TRUE);
					}
					else if (m_nSelectAxis == GANTRY_R)
					{
						FAS_ServoEnable(m_nGantryR1AxisNo / m_nMaxFasAxis + 1, m_nGantryL1AxisNo%m_nMaxFasAxis, TRUE);
						FAS_ServoEnable(m_nGantryR2AxisNo / m_nMaxFasAxis + 1, m_nGantryL2AxisNo%m_nMaxFasAxis, TRUE);
						FAS_ServoEnable(m_structAxisData[m_nSelectAxis].nAxisNo / m_nMaxFasAxis + 1, m_structAxisData[m_nSelectAxis].nAxisNo%m_nMaxFasAxis, TRUE);
					}
				}
				else
				{
					FAS_ServoEnable(m_structAxisData[m_nSelectAxis].nAxisNo / m_nMaxFasAxis + 1, m_structAxisData[m_nSelectAxis].nAxisNo%m_nMaxFasAxis, TRUE);
				}
			}
			else  //EZR 축의 경우...  // 수정 필요..
			{
				if ((m_nSelectAxis == GANTRY_L) || (m_nSelectAxis == GANTRY_R))
				{
					if (m_nSelectAxis == GANTRY_L)
					{
						FAS_ServoEnable(m_nGantryL1AxisNo / m_nMaxFasAxis + 1, m_nGantryL1AxisNo%m_nMaxFasAxis, TRUE);
						FAS_ServoEnable(m_nGantryL2AxisNo / m_nMaxFasAxis + 1, m_nGantryL2AxisNo%m_nMaxFasAxis, TRUE);
						FAS_ServoEnable(m_structAxisData[m_nSelectAxis].nAxisNo / m_nMaxFasAxis + 1, m_structAxisData[m_nSelectAxis].nAxisNo%m_nMaxFasAxis, TRUE);
					}
					else if (m_nSelectAxis == GANTRY_R)
					{
						FAS_ServoEnable(m_nGantryR1AxisNo / m_nMaxFasAxis + 1, m_nGantryL1AxisNo%m_nMaxFasAxis, TRUE);
						FAS_ServoEnable(m_nGantryR2AxisNo / m_nMaxFasAxis + 1, m_nGantryL2AxisNo%m_nMaxFasAxis, TRUE);
						FAS_ServoEnable(m_structAxisData[m_nSelectAxis].nAxisNo / m_nMaxFasAxis + 1, m_structAxisData[m_nSelectAxis].nAxisNo%m_nMaxFasAxis, TRUE);
					}
				}
				else
				{
					 FAS_ServoEnable(m_structAxisData[m_nSelectAxis].nAxisNo / m_nMaxFasAxis + 1, m_structAxisData[m_nSelectAxis].nAxisNo%m_nMaxFasAxis,TRUE);
				}
			}	
			m_bSelectServoOn = TRUE;
		}
	}
	else
	{
		if (m_bSelectServoOn == TRUE)
		{
			if (m_structAxisData[m_nSelectAxis].bMotionType == 0) //Motion 보드 사용일 경우...
			{
				if ((m_nSelectAxis == GANTRY_L) || (m_nSelectAxis == GANTRY_R))
				{
					if (m_nSelectAxis == GANTRY_L)
					{
						FAS_ServoEnable(m_nGantryL1AxisNo / m_nMaxFasAxis + 1, m_nGantryL1AxisNo%m_nMaxFasAxis, FALSE);
						FAS_ServoEnable(m_nGantryL2AxisNo / m_nMaxFasAxis + 1, m_nGantryL2AxisNo%m_nMaxFasAxis, FALSE);
						FAS_ServoEnable(m_structAxisData[m_nSelectAxis].nAxisNo / m_nMaxFasAxis + 1, m_structAxisData[m_nSelectAxis].nAxisNo%m_nMaxFasAxis, FALSE);
					}
					else if (m_nSelectAxis == GANTRY_R)
					{
						FAS_ServoEnable(m_nGantryR1AxisNo / m_nMaxFasAxis + 1, m_nGantryL1AxisNo%m_nMaxFasAxis, FALSE);
						FAS_ServoEnable(m_nGantryR2AxisNo / m_nMaxFasAxis + 1, m_nGantryL2AxisNo%m_nMaxFasAxis, FALSE);
						FAS_ServoEnable(m_structAxisData[m_nSelectAxis].nAxisNo / m_nMaxFasAxis + 1, m_structAxisData[m_nSelectAxis].nAxisNo%m_nMaxFasAxis, FALSE);
					}
				}
				else
				{
					FAS_ServoEnable(m_structAxisData[m_nSelectAxis].nAxisNo / m_nMaxFasAxis + 1, m_structAxisData[m_nSelectAxis].nAxisNo%m_nMaxFasAxis, FALSE);
				}
			}
			else  //EZR 축의 경우... // 수정 필요..
			{
				if ((m_nSelectAxis == GANTRY_L) || (m_nSelectAxis == GANTRY_R))
				{
					if (m_nSelectAxis == GANTRY_L)
					{
						FAS_ServoEnable(m_nGantryL1AxisNo / m_nMaxFasAxis + 1, m_nGantryL1AxisNo%m_nMaxFasAxis, FALSE);
						FAS_ServoEnable(m_nGantryL2AxisNo / m_nMaxFasAxis + 1, m_nGantryL2AxisNo%m_nMaxFasAxis, FALSE);
						FAS_ServoEnable(m_structAxisData[m_nSelectAxis].nAxisNo / m_nMaxFasAxis + 1, m_structAxisData[m_nSelectAxis].nAxisNo%m_nMaxFasAxis, FALSE);
					}
					else if (m_nSelectAxis == GANTRY_R)
					{
						FAS_ServoEnable(m_nGantryR1AxisNo / m_nMaxFasAxis + 1, m_nGantryL1AxisNo%m_nMaxFasAxis, FALSE);
						FAS_ServoEnable(m_nGantryR2AxisNo / m_nMaxFasAxis + 1, m_nGantryL2AxisNo%m_nMaxFasAxis, FALSE);
						FAS_ServoEnable(m_structAxisData[m_nSelectAxis].nAxisNo / m_nMaxFasAxis + 1, m_structAxisData[m_nSelectAxis].nAxisNo%m_nMaxFasAxis, FALSE);
					}
				}
				else
				{
					FAS_ServoEnable(m_structAxisData[m_nSelectAxis].nAxisNo / m_nMaxFasAxis + 1, m_structAxisData[m_nSelectAxis].nAxisNo%m_nMaxFasAxis,FALSE); 
				}
			}
			m_bSelectServoOn = FALSE;
		}
	}
	
	m_dSelectSpeed = (double)GetPendantRegister(0x012C, 0x0001) / 1000;// Register Address(Speed Select) 300번 Read... 
	if (m_dSelectSpeed > 50) m_dSelectSpeed = 50;
	FAS_SetLocalPara(m_structAxisData[m_nSelectAxis].nAxisNo / m_nMaxFasAxis + 1, m_structAxisData[m_nSelectAxis].nAxisNo%m_nMaxFasAxis, 8, m_dSelectSpeed*1000);
}
void CPendant::ReadSelectAxis()
{
	int nSelectAxisData = 0;

	nSelectAxisData = GetPendantCoil(m_structAxisData[STAGE_Y].wSelectAxis_Address, 5);  // Coil Address 81번 부터 5개 Coile Read... Stage 와 Gantry Axis Select 영역...
	if (nSelectAxisData == -100) // 통신응답이 없는경우.. 
	{
		return;
	}
	if (nSelectAxisData != 0) 
	{
		if (nSelectAxisData == 0x01) m_nSelectAxis = STAGE_Y;  // 0000 0001 
		else if (nSelectAxisData == 0x02) m_nSelectAxis = STAGE_X; // 0000 0010 
		else if (nSelectAxisData == 0x04) m_nSelectAxis = STAGE_TH; // 0000 0100 
		else if (nSelectAxisData == 0x08) m_nSelectAxis = GANTRY_L; // 0000 1000 
		else if (nSelectAxisData == 0x10) m_nSelectAxis = GANTRY_R; // 0001 0000 
		else m_nSelectAxis = -1; // 다른값이 들어올때는 -1으로 처리...
		return;
	}

	nSelectAxisData = GetPendantCoil(m_structAxisData[HEAD_S1].wSelectAxis_Address, 16);  // Coil Address 97번 부터 16개 Coile Read... HEAD_S Axis Select 영역...
	if (nSelectAxisData != 0) 
	{
		if (nSelectAxisData == 0x0001) m_nSelectAxis = HEAD_S1;  // 0000 0001 
		else if (nSelectAxisData == 0x0002) m_nSelectAxis = HEAD_S2; // 0000 0010 
		else if (nSelectAxisData == 0x0004) m_nSelectAxis = HEAD_S3; // 0000 0100 
		else if (nSelectAxisData == 0x0008) m_nSelectAxis = HEAD_S4; // 0000 1000 
		else if (nSelectAxisData == 0x0010) m_nSelectAxis = HEAD_S5; // 0001 0000	
		else if (nSelectAxisData == 0x0020) m_nSelectAxis = HEAD_S6; // 0010 0010 
		else if (nSelectAxisData == 0x0040) m_nSelectAxis = HEAD_S7; // 0100 0000 
		else if (nSelectAxisData == 0x0080) m_nSelectAxis = HEAD_S8; // 1000 0000 
		else if (nSelectAxisData == 0x0100) m_nSelectAxis = HEAD_S9; // 0000 0001 0000 0000 
		else if (nSelectAxisData == 0x0200) m_nSelectAxis = HEAD_S10; // 0000 0010 0000 0000 
		else if (nSelectAxisData == 0x0400) m_nSelectAxis = HEAD_S11; // 0000 0100 0000 0000 
		else if (nSelectAxisData == 0x0800) m_nSelectAxis = HEAD_S12; // 0000 1000 0000 0000  
		else if (nSelectAxisData == 0x1000) m_nSelectAxis = HEAD_S13; // 0001 0000 0000 0000 
		else if (nSelectAxisData == 0x2000) m_nSelectAxis = HEAD_S14; // 0010 0000 0000 0000 
		else if (nSelectAxisData == 0x4000) m_nSelectAxis = HEAD_S15; // 0100 0000 0000 0000 
		else if (nSelectAxisData == 0x8000) m_nSelectAxis = HEAD_S16; // 1000 0000 0000 0000 
		else m_nSelectAxis = -1; // 다른값이 들어올때는 -1으로 처리...
		return;
	}	
	nSelectAxisData = GetPendantCoil(m_structAxisData[HEAD_P1].wSelectAxis_Address, 16);  // Coil Address 113번 부터 16개 Coile Read... HEAD_S Axis Select 영역...
	if (nSelectAxisData != 0) 
	{
		if (nSelectAxisData == 0x0001) m_nSelectAxis = HEAD_P1;  // 0000 0001 
		else if (nSelectAxisData == 0x0002) m_nSelectAxis = HEAD_P2; // 0000 0010 
		else if (nSelectAxisData == 0x0004) m_nSelectAxis = HEAD_P3; // 0000 0100 
		else if (nSelectAxisData == 0x0008) m_nSelectAxis = HEAD_P4; // 0000 1000 
		else if (nSelectAxisData == 0x0010) m_nSelectAxis = HEAD_P5; // 0001 0000 
		else if (nSelectAxisData == 0x0020) m_nSelectAxis = HEAD_P6; // 0010 0010 
		else if (nSelectAxisData == 0x0040) m_nSelectAxis = HEAD_P7; // 0100 0000 
		else if (nSelectAxisData == 0x0080) m_nSelectAxis = HEAD_P8; // 1000 0000 
		else if (nSelectAxisData == 0x0100) m_nSelectAxis = HEAD_P9; // 0000 0001 0000 0000 
		else if (nSelectAxisData == 0x0200) m_nSelectAxis = HEAD_P10; // 0000 0010 0000 0000 
		else if (nSelectAxisData == 0x0400) m_nSelectAxis = HEAD_P11; // 0000 0100 0000 0000 
		else if (nSelectAxisData == 0x0800) m_nSelectAxis = HEAD_P12; // 0000 1000 0000 0000  
		else if (nSelectAxisData == 0x1000) m_nSelectAxis = HEAD_P13; // 0001 0000 0000 0000 
		else if (nSelectAxisData == 0x2000) m_nSelectAxis = HEAD_P14; // 0010 0000 0000 0000 
		else if (nSelectAxisData == 0x4000) m_nSelectAxis = HEAD_P15; // 0100 0000 0000 0000 
		else if (nSelectAxisData == 0x8000) m_nSelectAxis = HEAD_P16; // 1000 0000 0000 0000 
		else m_nSelectAxis = -1; // 다른값이 들어올때는 -1으로 처리...
		return;
	}

	nSelectAxisData = GetPendantCoil(m_structAxisData[HEAD_G1].wSelectAxis_Address, 16);  // Coil Address 129번 부터 16개 Coile Read... HEAD_S Axis Select 영역...
	if (nSelectAxisData != 0) 
	{
		if (nSelectAxisData == 0x0001) m_nSelectAxis = HEAD_G1;  // 0000 0001 
		else if (nSelectAxisData == 0x0002) m_nSelectAxis = HEAD_G2; // 0000 0010 
		else if (nSelectAxisData == 0x0004) m_nSelectAxis = HEAD_G3; // 0000 0100 
		else if (nSelectAxisData == 0x0008) m_nSelectAxis = HEAD_G4; // 0000 1000 
		else if (nSelectAxisData == 0x0010) m_nSelectAxis = HEAD_G5; // 0001 0000 
		else if (nSelectAxisData == 0x0020) m_nSelectAxis = HEAD_G6; // 0010 0010 
		else if (nSelectAxisData == 0x0040) m_nSelectAxis = HEAD_G7; // 0100 0000 
		else if (nSelectAxisData == 0x0080) m_nSelectAxis = HEAD_G8; // 1000 0000 
		else if (nSelectAxisData == 0x0100) m_nSelectAxis = HEAD_G9; // 0000 0001 0000 0000 
		else if (nSelectAxisData == 0x0200) m_nSelectAxis = HEAD_G10; // 0000 0010 0000 0000 
		else if (nSelectAxisData == 0x0400) m_nSelectAxis = HEAD_G11; // 0000 0100 0000 0000 
		else if (nSelectAxisData == 0x0800) m_nSelectAxis = HEAD_G12; // 0000 1000 0000 0000  
		else if (nSelectAxisData == 0x1000) m_nSelectAxis = HEAD_G13; // 0001 0000 0000 0000 
		else if (nSelectAxisData == 0x2000) m_nSelectAxis = HEAD_G14; // 0010 0000 0000 0000 
		else if (nSelectAxisData == 0x4000) m_nSelectAxis = HEAD_G15; // 0100 0000 0000 0000 
		else if (nSelectAxisData == 0x8000) m_nSelectAxis = HEAD_G16; // 1000 0000 0000 0000 
		else m_nSelectAxis = -1; // 다른값이 들어올때는 -1으로 처리...
		return;
	}	

	m_nSelectAxis = -1; // 아무축도 선택되지 않은경우....
	
	
}

void CPendant::OnDblClickCmdDisconnect(LPDISPATCH Cancel) 
{
	// TODO: Add your control notification handler code here
	int nTempTime = 0;
	BtnEnable(FALSE);
	m_bPendantThreadExitFlag = TRUE;
	KillThreadPendant();
	while (g_bThreadRunFlag)
	{
		Sleep(1);
		nTempTime++;
		DoEvents();
		if (nTempTime>30000) break;
	}
	m_strMessageText.Format("Pendant Thread Funtion End");
	SendMessage(WM_MESSAGEBOX, 0, 0);
	
	if ( m_ctrlCommPendant.GetPortOpen()) m_ctrlCommPendant.SetPortOpen( FALSE );
	
	m_strMessageText.Format("Pendant Communication Port Close");
	SendMessage(WM_MESSAGEBOX, 0, 0);
	PendantPowerOnOff(FALSE);
	m_strMessageText.Format("Pendant Power Off");
	SendMessage(WM_MESSAGEBOX, 0, 0);
	BtnEnable(TRUE);
}
////////////////////////Modbus RTU 프로트콜 적용 함수부 Start//////////////////////////////
void CPendant::SetPendantSingleRegister(WORD wRegister, WORD wValue) 
{
	CString strUseAxisInfo;
	VARIANT	var;
	byte OutBuff[8];
	byte InBuff[8];
	byte wRegisterHigh = 0X00, wRegisterLow = 0X00, wValueHigh = 0X00, wValueLow = 0X00;
	wRegisterHigh = ((wRegister & 0xff00)>>8);
	wRegisterLow = wRegister & 0x00ff;

	wValueHigh = ((wValue & 0xff00)>>8);
	wValueLow = wValue & 0x00ff;

	OutBuff[0] = 0x00;  // Slave Address
	OutBuff[1] = 0x06;  // Function 
	OutBuff[2] = wRegisterHigh;  // Start Address High
	OutBuff[3] = wRegisterLow;  // Start Address Low
	OutBuff[4] = wValueHigh;  // Registers Value High
	OutBuff[5] = wValueLow;  // Registers Value Low

///////////////////////// CRC DATA 계산 ////////////////////////////
	WORD CRC = 0xFFFF; // -> 0xFFFF
	BYTE BuffData;
	WORD CRCLowBit, CRCHigh;
	for(int i = 0; i<= 5 ;i++)  // OutBuff DataLen
	{
		BuffData = OutBuff[i];
		CRC = CRC ^ BuffData; //Exclusive OR
		for(int j = 1;j<= 8;j++)
		{
			CRCLowBit = CRC & 1; 
			CRC = CRC / 2;
			if( CRCLowBit == 1 ) CRC = CRC ^ 0xA001;
			// 적출한 하위비트가 1이면 0xA001과 Exclusive OR
		}
	}
	CRCHigh = CRC / 256;
	OutBuff[6] = (BYTE)(CRC - CRCHigh * 256); // CRC_low 
	OutBuff[7] = (BYTE)CRCHigh; // CRC_high

///////////////////////////////////////////////////////////////////////
    SAFEARRAYBOUND aDim[1];
    aDim[0].cElements=8;
	aDim[0].lLbound=0;
	var.vt = VT_UI1 | VT_ARRAY;    
	var.parray = SafeArrayCreate(VT_R8, 1, aDim);
	strUseAxisInfo.Empty();
	byte byteTemp;
	CString strTemp;
	for (long nIndex = 0; nIndex<8; nIndex++)
	{
		byteTemp = OutBuff[nIndex];
		SafeArrayPutElement( var.parray, &nIndex ,&byteTemp );
	}

	m_ctrlCommPendant.SetOutput( var );

	DoEvents();

	memset(InBuff, 0x00, 8);
	
	for (int nIndexTime = 0; nIndexTime < 3000; nIndexTime++)
	{
		Sleep(1);
		DoEvents();
		if (m_bReciveDataOn == TRUE)
		{
			memcpy(InBuff, m_szRcvBuf, 8);  // 수신 Data 비교 필요... 귀찮다.. 나중에 필요하면 하자.
			memset(m_szRcvBuf, 0x00, 200);
			m_bReciveDataOn = FALSE;
			break;
		}
	}
	if (nIndexTime>=3000)
	{
		m_strMessageText.Format("[PENDANT->EQ] : SetPendantSingleRegister Funtion NO ANSWER");
		SendMessage(WM_MESSAGEBOX, 0, 0);
	}
}
void CPendant::SetPendantMultyCoil(WORD wStartCoilAddress, WORD wPointCoilNo, WORD wValue) 
{
	CString strUseAxisInfo;
	VARIANT	var;
	byte OutBuff[200];
	byte InBuff[200];
	byte wCoilAddressHigh = 0X00, wCoilAddressLow = 0X00, wPointCoilHigh = 0X00, wPointCoilLow = 0X00, wDataByteSize = 0x00;
	wCoilAddressHigh = ((wStartCoilAddress & 0xff00)>>8);
	wCoilAddressLow = wStartCoilAddress & 0x00ff;

	wPointCoilHigh = ((wPointCoilNo & 0xff00)>>8);
	wPointCoilLow = wPointCoilNo & 0x00ff;

	wDataByteSize = (wPointCoilNo / 8) + 1;
	
	OutBuff[0] = 0x00;  // Slave Address
	OutBuff[1] = 0x0F;  // Function. Force Multiple Coils
	OutBuff[2] = wCoilAddressHigh;  // Start Address High
	OutBuff[3] = wCoilAddressLow;  // Start Address Low
	OutBuff[4] = wPointCoilHigh;  // Controll Coil Point High
	OutBuff[5] = wPointCoilLow;  // Controll Coil Point Low
	OutBuff[6] = wDataByteSize;  // Data Byte Size
	for (int nIndexDataByteSize = 0; nIndexDataByteSize < wDataByteSize; nIndexDataByteSize++)
	{
		OutBuff[7+nIndexDataByteSize] = ((wValue & (0x00ff<<(8*nIndexDataByteSize))) >> (8*nIndexDataByteSize));
	}


///////////////////////// CRC DATA 계산 ////////////////////////////
	WORD CRC = 0xFFFF; // -> 0xFFFF
	BYTE BuffData;
	WORD CRCLowBit, CRCHigh;
	for(int i = 0; i <= (6+wDataByteSize) ;i++)  // OutBuff DataLen
	{
		BuffData = OutBuff[i];
		CRC = CRC ^ BuffData; //Exclusive OR
		for(int j = 1;j<= 8;j++)
		{
			CRCLowBit = CRC & 1; 
			CRC = CRC / 2;
			if( CRCLowBit == 1 ) CRC = CRC ^ 0xA001;
			// 적출한 하위비트가 1이면 0xA001과 Exclusive OR
		}
	}
	CRCHigh = CRC / 256;
	OutBuff[6+wDataByteSize+1] = (BYTE)(CRC - CRCHigh * 256); // CRC_low 
	OutBuff[6+wDataByteSize+2] = (BYTE)CRCHigh; // CRC_high

///////////////////////////////////////////////////////////////////////
    SAFEARRAYBOUND aDim[1];
    aDim[0].cElements=(6+wDataByteSize+2+1);
	aDim[0].lLbound=0;
	var.vt = VT_UI1 | VT_ARRAY;
	var.parray = SafeArrayCreate(VT_R8, 1, aDim);
	strUseAxisInfo.Empty();
	byte byteTemp;
	CString strTemp;
	for (long nIndex = 0; nIndex<(6+wDataByteSize+2+1); nIndex++)
	{
		byteTemp = OutBuff[nIndex];
		SafeArrayPutElement( var.parray, &nIndex ,&byteTemp );
	}

	m_ctrlCommPendant.SetOutput( var );

	DoEvents();

	memset(InBuff, 0x00, 200);
	
	for (int nIndexTime = 0; nIndexTime < 3000; nIndexTime++)
	{
		Sleep(1);
		DoEvents();
		if (m_bReciveDataOn == TRUE)
		{
			memcpy(InBuff, m_szRcvBuf, 8);  // 수신 Data 비교 필요... 귀찮다.. 나중에 필요하면 하자.
			memset(m_szRcvBuf, 0x00, 200);
			m_bReciveDataOn = FALSE;
			break;
		}
	}
	if (nIndexTime>=3000)
	{
		m_strMessageText.Format("[PENDANT->EQ] : SetPendantMultyCoil Funtion NO ANSWER");
		SendMessage(WM_MESSAGEBOX, 0, 0);
	}
}

void CPendant::SetPendantSingleCoil(DWORD wCoil, BOOL bOnOff) 
{
	CString strUseAxisInfo;
	VARIANT	var;
	byte OutBuff[8];
	byte InBuff[8];
	byte wRegisterHigh = 0X00, wRegisterLow = 0X00, wOnOffHigh = 0X00, wOnOffLow = 0X00;
	wRegisterHigh = ((wCoil & 0xff00)>>8);
	wRegisterLow = wCoil & 0x00ff;
	if (bOnOff)
	{
		wOnOffHigh = 0xFF;
		wOnOffLow = 0x00;
	}

	OutBuff[0] = 0x00;  // Slave Address
	OutBuff[1] = 0x05;  // Function 
	OutBuff[2] = wRegisterHigh;  // Start Address High
	OutBuff[3] = wRegisterLow;  // Start Address Low
	OutBuff[4] = wOnOffHigh;  // Registers Value High
	OutBuff[5] = wOnOffLow;  // Registers Value Low

///////////////////////// CRC DATA 계산 ////////////////////////////
	WORD CRC = 0xFFFF; // -> 0xFFFF
	BYTE BuffData;
	WORD CRCLowBit, CRCHigh;
	for(int i = 0; i<= 5 ;i++)  // OutBuff DataLen
	{
		BuffData = OutBuff[i];
		CRC = CRC ^ BuffData; //Exclusive OR
		for(int j = 1;j<= 8;j++)
		{
			CRCLowBit = CRC & 1; 
			CRC = CRC / 2;
			if( CRCLowBit == 1 ) CRC = CRC ^ 0xA001;
			// 적출한 하위비트가 1이면 0xA001과 Exclusive OR
		}
	}
	CRCHigh = CRC / 256;
	OutBuff[6] = (BYTE)(CRC - CRCHigh * 256); // CRC_low 
	OutBuff[7] = (BYTE)CRCHigh; // CRC_high

///////////////////////////////////////////////////////////////////////
    SAFEARRAYBOUND aDim[1];
    aDim[0].cElements=8;
	aDim[0].lLbound=0;
	var.vt = VT_UI1 | VT_ARRAY;    
	var.parray = SafeArrayCreate(VT_R8, 1, aDim);
	strUseAxisInfo.Empty();
	byte byteTemp;
	CString strTemp;
	for (long nIndex = 0; nIndex<8; nIndex++)
	{
		byteTemp = OutBuff[nIndex];
		SafeArrayPutElement( var.parray, &nIndex ,&byteTemp );
	}

	m_ctrlCommPendant.SetOutput( var );

	DoEvents();

	memset(InBuff, 0x00, 8);
	
	for (int nIndexTime = 0; nIndexTime < 3000; nIndexTime++)
	{
		Sleep(1);
		DoEvents();
		if (m_bReciveDataOn == TRUE)
		{
			memcpy(InBuff, m_szRcvBuf, 8);  // 수신 Data 비교 필요... 귀찮다.. 나중에 필요하면 하자.
			memset(m_szRcvBuf, 0x00, 200);
			m_bReciveDataOn = FALSE;
			break;
		}
	}
	if (nIndexTime>=3000)
	{
		m_strMessageText.Format("[PENDANT->EQ] : SetPendantSingleCoil Funtion NO ANSWER");
		SendMessage(WM_MESSAGEBOX, 0, 0);
	}


}
int CPendant::GetPendantCoil(WORD wRegisterStartAddress, WORD wAddressPointNo) 
{
	int nGetCoilData =0;
	VARIANT	var;
	byte OutBuff[8];
	byte InBuff[200];
	byte wStartRegisterHigh = 0X00, wStartRegisterLow = 0X00, wPointHigh = 0X00, wPointLow = 0X00;
	wStartRegisterHigh = ((wRegisterStartAddress & 0xff00)>>8);
	wStartRegisterLow = wRegisterStartAddress & 0x00ff;

	wPointHigh = ((wAddressPointNo & 0xff00)>>8);
	wPointLow = wAddressPointNo & 0x00ff;

	OutBuff[0] = 0x00;  // Slave Address
	OutBuff[1] = 0x01;  // Function 
	OutBuff[2] = wStartRegisterHigh;  // Start Address High
	OutBuff[3] = wStartRegisterLow;  // Start Address Low
	OutBuff[4] = wPointHigh;  // Registers Value High
	OutBuff[5] = wPointLow;  // Registers Value Low

///////////////////////// CRC DATA 계산 ////////////////////////////
	WORD CRC = 0xFFFF; // -> 0xFFFF
	BYTE BuffData;
	WORD CRCLowBit, CRCHigh;
	for(int i = 0; i<= 5 ;i++)  // OutBuff DataLen
	{
		BuffData = OutBuff[i];
		CRC = CRC ^ BuffData; //Exclusive OR
		for(int j = 1;j<= 8;j++)
		{
			CRCLowBit = CRC & 1; 
			CRC = CRC / 2;
			if( CRCLowBit == 1 ) CRC = CRC ^ 0xA001;
			// 적출한 하위비트가 1이면 0xA001과 Exclusive OR
		}
	}
	CRCHigh = CRC / 256;
	OutBuff[6] = (BYTE)(CRC - CRCHigh * 256); // CRC_low 
	OutBuff[7] = (BYTE)CRCHigh; // CRC_high

///////////////////////////////////////////////////////////////////////
    SAFEARRAYBOUND aDim[1];
    aDim[0].cElements=8;
	aDim[0].lLbound=0;
	var.vt = VT_UI1 | VT_ARRAY;
	var.parray = SafeArrayCreate(VT_R8, 1, aDim);
	byte byteTemp;
	CString strTemp;
	for (long nIndex = 0; nIndex<8; nIndex++)
	{
		byteTemp = OutBuff[nIndex];
		SafeArrayPutElement( var.parray, &nIndex ,&byteTemp );
	}

	m_ctrlCommPendant.SetOutput( var );

	DoEvents();

	memset(InBuff, 0x00, 200);
	
	for (int nIndexTime = 0; nIndexTime < 3000; nIndexTime++)
	{
		Sleep(1);
		DoEvents();
		if (m_bReciveDataOn == TRUE)
		{
			Sleep(100);
			memcpy(InBuff, m_szRcvBuf, 200);  
			memset(m_szRcvBuf, 0x00, 200);
			m_bReciveDataOn = FALSE;
			break;
		}
	}
	if (nIndexTime>=3000)
	{
		m_strMessageText.Format("[PENDANT->EQ] : GetPendantCoil Funtion NO ANSWER");
		SendMessage(WM_MESSAGEBOX, 0, 0);
		return -100;
	}
	nGetCoilData = 0;
	int nReciveDataByteCount = InBuff[2]; // 3번째 번지에 Data Byte 수를 받는다...
	for (int nIndexDataByte = 0; nIndexDataByte < nReciveDataByteCount; nIndexDataByte++)
	{
		nGetCoilData |= InBuff[3+nIndexDataByte] << (8*nIndexDataByte);
	}
	
	return nGetCoilData;
}

int CPendant::GetPendantRegister(WORD wRegisterStartAddress, WORD wAddressPointNo) 
{
	int nGetRegisterData =0;
	VARIANT	var;
	byte OutBuff[8];
	byte InBuff[200];
	byte wStartRegisterHigh = 0X00, wStartRegisterLow = 0X00, wPointHigh = 0X00, wPointLow = 0X00;
	wStartRegisterHigh = ((wRegisterStartAddress & 0xff00)>>8);
	wStartRegisterLow = wRegisterStartAddress & 0x00ff;

	wPointHigh = ((wAddressPointNo & 0xff00)>>8);
	wPointLow = wAddressPointNo & 0x00ff;

	OutBuff[0] = 0x00;  // Slave Address
	OutBuff[1] = 0x04;  // Function 
	OutBuff[2] = wStartRegisterHigh;  // Start Address High
	OutBuff[3] = wStartRegisterLow;  // Start Address Low
	OutBuff[4] = wPointHigh;  // Registers Value High
	OutBuff[5] = wPointLow;  // Registers Value Low

///////////////////////// CRC DATA 계산 ////////////////////////////
	WORD CRC = 0xFFFF; // -> 0xFFFF
	BYTE BuffData;
	WORD CRCLowBit, CRCHigh;
	for(int i = 0; i<= 5 ;i++)  // OutBuff DataLen
	{
		BuffData = OutBuff[i];
		CRC = CRC ^ BuffData; //Exclusive OR
		for(int j = 1;j<= 8;j++)
		{
			CRCLowBit = CRC & 1; 
			CRC = CRC / 2;
			if( CRCLowBit == 1 ) CRC = CRC ^ 0xA001;
			// 적출한 하위비트가 1이면 0xA001과 Exclusive OR
		}
	}
	CRCHigh = CRC / 256;
	OutBuff[6] = (BYTE)(CRC - CRCHigh * 256); // CRC_low 
	OutBuff[7] = (BYTE)CRCHigh; // CRC_high

///////////////////////////////////////////////////////////////////////
    SAFEARRAYBOUND aDim[1];
    aDim[0].cElements=8;
	aDim[0].lLbound=0;
	var.vt = VT_UI1 | VT_ARRAY;
	var.parray = SafeArrayCreate(VT_R8, 1, aDim);
	byte byteTemp;
	CString strTemp;
	for (long nIndex = 0; nIndex<8; nIndex++)
	{
		byteTemp = OutBuff[nIndex];
		SafeArrayPutElement( var.parray, &nIndex ,&byteTemp );
	}

	m_ctrlCommPendant.SetOutput( var );
	
	DoEvents();

	memset(InBuff, 0x00, 200);
	
	for (int nIndexTime = 0; nIndexTime < 3000; nIndexTime++)
	{
		Sleep(1);
		DoEvents();
		if (m_bReciveDataOn == TRUE)
		{
			Sleep(100);
			memcpy(InBuff, m_szRcvBuf, 200);  
			memset(m_szRcvBuf, 0x00, 200);
			m_bReciveDataOn = FALSE;
			break;
		}
	}
	if (nIndexTime>=3000)
	{
		m_strMessageText.Format("[PENDANT->EQ] : GetPendantRegister Funtion NO ANSWER");
		SendMessage(WM_MESSAGEBOX, 0, 0);
		return -100;
	}
	nGetRegisterData = 0;
	int nReciveDataByteCount = InBuff[2]; // 3번째 번지에 Data Byte 수를 받는다...
	for (int nIndexDataByte = 0; nIndexDataByte < nReciveDataByteCount; nIndexDataByte++)
	{
		nGetRegisterData |= InBuff[3+nIndexDataByte] << (8*(nReciveDataByteCount-nIndexDataByte-1));
	}
	
	return nGetRegisterData;
}
////////////////////////////////////Modbus 프로트콜 적용 함수부 End////////////////////////////////




////////////////////////INI  함수부 Start///////////////////////////////////
int CPendant::INI_ReadKeyCount(CString strFileName, CString strSection)//char* szSection)
{
	memset(m_szFileName, 0x00, 255);
	memcpy(m_szFileName, strFileName, strlen(strFileName));
	char szResultAll[10000];
	DWORD dwSize;
	int nKeyDataCount = 0;
	strcpy(szSection, strSection);
	dwSize = GetPrivateProfileSection(szSection, szResultAll, 10000, m_szFileName);
	for (int nIndex = 0; nIndex < dwSize; nIndex++)
	{
		if (szResultAll[nIndex] == '\0') nKeyDataCount++;
	}
	if (dwSize == 0)
	{
		CString strMessage;
		strMessage.Format(strFileName + " [SECTION : %s] No Data", szSection);
		AfxMessageBox(strMessage);
	}
	return nKeyDataCount;
}
CString CPendant::INI_ReadKeyName(CString strFileName, CString strSection, int nLine)
{
	memset(m_szFileName, 0x00, 255);
	memcpy(m_szFileName, strFileName, strlen(strFileName));
	char szResultAll[10000];
	DWORD dwSize;
	int nKeyDataCount = 0;
	BOOL bKeyName = TRUE;
	CString strKeyName, strTempKeyName;
	strcpy(szSection, strSection);
	dwSize = GetPrivateProfileSection(szSection, szResultAll, 10000, m_szFileName);
	for (int nIndex = 0; nIndex < dwSize; nIndex++)
	{
		if (szResultAll[nIndex] == '=') bKeyName = FALSE;
		if (bKeyName == TRUE) strTempKeyName += szResultAll[nIndex];
		if (szResultAll[nIndex] == '\0')
		{
			nKeyDataCount++;
			if (nKeyDataCount == nLine) strKeyName = strTempKeyName;
			else
			{
				strTempKeyName = "";
				bKeyName = TRUE;
			}
		}
	}
	return strKeyName;
}
int CPendant::INI_ReadInt(CString strFileName, CString strSection, CString strKey)
{
	memset(m_szFileName, 0x00, 255);
	memcpy(m_szFileName, strFileName, strlen(strFileName));
	char szResult[255];
	int nResult, nReadStringCount;
	strcpy(szSection, strSection);
	strcpy(szKey, strKey);
	nReadStringCount = GetPrivateProfileString(szSection, szKey, _T(""), szResult, 255, m_szFileName);
	if (nReadStringCount == 0)
	{
		CString strMessage;
		strMessage.Format(strFileName + " [SECTION : %s], [KEY : %s] No Data", szSection, szKey);
		AfxMessageBox(strMessage);
	}
	nResult = atoi(szResult);
	return nResult;
}
double CPendant::INI_ReadDouble(CString strFileName, CString strSection, CString strKey)
{
	memset(m_szFileName, 0x00, 255);
	memcpy(m_szFileName, strFileName, strlen(strFileName));
	char szResult[255];
	double dResult;
	int nReadStringCount;
	strcpy(szSection, strSection);
	strcpy(szKey, strKey);
	nReadStringCount = GetPrivateProfileString(szSection, szKey, _T(""), szResult, 255, m_szFileName);
	if (nReadStringCount == 0)
	{
		CString strMessage;
		strMessage.Format(strFileName + " [SECTION : %s], [KEY : %s] No Data", szSection, szKey);
		AfxMessageBox(strMessage);
	}
	dResult = atof(szResult);
	return dResult;
}
bool CPendant::INI_ReadBool(CString strFileName, CString strSection, CString strKey)
{
	memset(m_szFileName, 0x00, 255);
	memcpy(m_szFileName, strFileName, strlen(strFileName));
	bool bolDefaultValue = FALSE;
	char szResult[255];
	bool bolResult;
	int nReadStringCount;
	strcpy(szSection, strSection);
	strcpy(szKey, strKey);
	nReadStringCount = GetPrivateProfileString(szSection, szKey, _T(""), szResult, 255, m_szFileName);
	if (nReadStringCount == 0)
	{
		CString strMessage;
		strMessage.Format(strFileName + " [SECTION : %s], [KEY : %s] No Data", szSection, szKey);
		AfxMessageBox(strMessage);
	}
	bolResult = (strcmp(szResult, "True") == 0 ||
		strcmp(szResult, "true") == 0 || strcmp(szResult, "1") == 0) ? true : false;
	return bolResult;
}
char* CPendant::INI_ReadString(CString strFileName, CString strSection, CString strKey)
{
 	memset(m_szFileName, 0x00, 255);
	memcpy(m_szFileName, strFileName, strlen(strFileName));
	char* szResult = new char[255];
	memset(szResult, 0x00, 255);
	int nReadStringCount;
	strcpy(szSection, strSection);
	strcpy(szKey, strKey);
	nReadStringCount = GetPrivateProfileString(szSection, szKey, _T(""), szResult, 255, m_szFileName);
	if (nReadStringCount == 0)
	{
		CString strMessage;
		strMessage.Format(strFileName + " [SECTION : %s], [KEY : %s] No Data", szSection, szKey);
		AfxMessageBox(strMessage);
	}
	return szResult;
}

void CPendant::INI_WriteInt(CString strFileName, CString strSection, CString strKey, int iValue)
{
	memset(m_szFileName, 0x00, 255);
	memcpy(m_szFileName, strFileName, strlen(strFileName));
	char szValue[255];
	sprintf(szValue, "%d", iValue);
	strcpy(szSection, strSection);
	strcpy(szKey, strKey);
	WritePrivateProfileString(szSection, szKey, szValue, m_szFileName);
}
void CPendant::INI_WriteDouble(CString strFileName, CString strSection, CString strKey, double dValue)
{
	memset(m_szFileName, 0x00, 255);
	memcpy(m_szFileName, strFileName, strlen(strFileName));
	char szValue[255];
	sprintf(szValue, "%.3f", dValue);
	strcpy(szSection, strSection);
	strcpy(szKey, strKey);
	WritePrivateProfileString(szSection, szKey, szValue, m_szFileName);
}
void CPendant::INI_WriteDouble(CString strFileName, CString strSection, CString strKey, double dValue, int nPrecision)	//기존 WriteDouble함수에 자리수 인자 추가
{
	memset(m_szFileName, 0x00, 255);
	memcpy(m_szFileName, strFileName, strlen(strFileName));
	char szValue[255];
	char szFmt[16];
	if (nPrecision > 0)
		sprintf(szFmt, _T("%%.%df"), nPrecision);
	sprintf(szValue, szFmt, dValue);
	strcpy(szSection, strSection);
	strcpy(szKey, strKey);
	WritePrivateProfileString(szSection, szKey, szValue, m_szFileName);
}
void CPendant::INI_WriteBool(CString strFileName, CString strSection, CString strKey, bool bolValue)
{
	memset(m_szFileName, 0x00, 255);
	memcpy(m_szFileName, strFileName, strlen(strFileName));
	char szValue[255];
	sprintf(szValue, "%s", bolValue ? "True" : "False");
	strcpy(szSection, strSection);
	strcpy(szKey, strKey);
	WritePrivateProfileString(szSection, szKey, szValue, m_szFileName);
}
void CPendant::INI_WriteString(CString strFileName, CString strSection, CString strKey, char* szValue)
{
	memset(m_szFileName, 0x00, 255);
	memcpy(m_szFileName, strFileName, strlen(strFileName));
	strcpy(szSection, strSection);
	strcpy(szKey, strKey);
	WritePrivateProfileString(szSection, szKey, szValue, m_szFileName);
}
void CPendant::INI_DeleteSection(CString strFileName, CString strSection)
{
	memset(m_szFileName, 0x00, 255);
	memcpy(m_szFileName, strFileName, strlen(strFileName));
	strcpy(szSection, strSection);
	WritePrivateProfileString(szSection, NULL, "", m_szFileName);
}
////////////////////////////// INI 함수부 End///////////////////////////////

void CPendant::OnClickCmdReturn() 
{
	// TODO: Add your control notification handler code here
	int nTempTime = 0;

	BtnEnable(FALSE);
	m_bPendantThreadExitFlag = TRUE;
	KillThreadPendant();
	while (g_bThreadRunFlag)
	{
		Sleep(1);
		nTempTime++;
		DoEvents();
		if (nTempTime>30000) break;
	}
	m_strMessageText.Format("Pendant Thread Funtion End");
	SendMessage(WM_MESSAGEBOX, 0, 0);
	
	if ( m_ctrlCommPendant.GetPortOpen()) m_ctrlCommPendant.SetPortOpen( FALSE );
	
	m_strMessageText.Format("Pendant Communication Port Close");
	SendMessage(WM_MESSAGEBOX, 0, 0);
	PendantPowerOnOff(FALSE);
	m_strMessageText.Format("Pendant Power Off");
	SendMessage(WM_MESSAGEBOX, 0, 0);
	BtnEnable(TRUE);
	if (PENDANT_TEST == FALSE)
	{
		if(ReadPendantConnectionIO() == TRUE) 
		{
			AfxMessageBox("Pendant Cable Not Disconnected");	
			BtnEnable(TRUE);
			return;
		}
	}
	m_strMessageText.Format("Pendant Dialog Close");
	SendMessage(WM_MESSAGEBOX, 0, 0);
	KillTimer(TIMER_UI);
	KillTimer(TIMER_THREADCHECK);
	EndDialog(IDOK);
	
}

BOOL CPendant::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch (pMsg->message)
	{
		case WM_KEYDOWN:
		{
			switch (pMsg->wParam)
			{
				case VK_ESCAPE:
				case VK_RETURN:
				return TRUE;
			}
		}                   
		case WM_SYSKEYDOWN:
		{
			switch (pMsg->wParam)
			{
				case VK_F4:
				return TRUE;
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

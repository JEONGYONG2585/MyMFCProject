// AdjustInitial.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"
//
#include "AdjustInitial.h"
#include "NormalMsg.h"
#include "Polylist.h"

#include "TempOP.h"
#include "Common.h"
#include "SettingAssemble.h"

#include "Employee_Num.h"		//ehji 141023

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL bModeSelect;
double dRemovalDistance;
int nSuctionSpeed;
int nDischargeSpeed;
int nRemovalTimes1;
int nRemovalTimes2;
int nInitSupplyTime;
int nNormalSupplyTime;
double dManuCanWeight;
extern BOOL GateHomeFlag[MAX_NOZZLE];
extern BOOL bInitialStopCheck;
extern BOOL AssemblePosMoveFlag;
extern BOOL g_bInitilDisplay;
extern UINT  g_nInitMeasureGlassCount;
extern BOOL g_bManualSupply;

extern BOOL m_bSWCheck;
extern BOOL g_bRemoteControl;
extern CString m_strMotionLog[MAX_NOZZLE];

//by shin//2013.01.15//n-type add//
double dNRemovalDistance;
double dNDischargeSpeed1;
double dNDischargeSpeed2;
int nNRemovalTimes;
int nNOverTime;
int nNAgingTimes;//2015.08.29 by tskim
int nNAgingSupplyTime; //20210224 him n-aging supply time
int nNAgingCycle; //20210224 him n-aging cycle

//±âÆ÷Á¦°Å µ¿ÀÛ À¯¹« È®ÀÎ ÇÃ·¡±×·Î »ç¿ë 
extern BOOL N_Nozzle_Detect_Flag[MAX_NOZZLE];

extern BOOL g_bAceeptSettingStatus; 
/////////////////////////////////////////////////////////////////////////////
// CAdjustInitial dialog


CAdjustInitial::CAdjustInitial(CWnd* pParent /*=NULL*/)
	: CDialog(CAdjustInitial::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAdjustInitial)
	//}}AFX_DATA_INIT

	ThreadStage.strMsg1="";
	m_nTimerCount=0;
	ThreadStage.AdjustInitCode = 1000;
	m_bMoveFlag = FALSE;
	pSettingAssembleDlg = NULL;
	m_bSWCheck = false;
}

CAdjustInitial::~CAdjustInitial()
{
}

void CAdjustInitial::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdjustInitial)
	DDX_Control(pDX, IDC_LABEL_RECIPENAME_ADJUSTINITIAL, m_ctrlRecipeName);
	DDX_Control(pDX, IDC_CMD_RETURN, m_ctrlReturn);
	DDX_Control(pDX, IDC_LABEL_MESSAGE_ADJUSTINITIAL, m_ctrlMessageAdjustInitial);
	DDX_Control(pDX, IDC_CMD_INTERLOCKUSAGE, m_ctrlUseInterlock);
	DDX_Control(pDX, IDC_MSFLEXGRID_ADJUSTINITIAL_SET, m_ctrlAdjustInitialSet);
	DDX_Control(pDX, IDC_CMD_MODE_SECLECT, m_ctrlModeSel);
	DDX_Control(pDX, IDC_CMD_PISTON_DOWN, m_ctrlPistonClear);
	DDX_Control(pDX, IDC_CMD_PUMP_DISJOINT, m_ctrlDisjoint);
	DDX_Control(pDX, IDC_CMD_PUMP_JOINT1, m_ctrlJoint1);
	DDX_Control(pDX, IDC_CMD_PUMP_JOINT2, m_ctrlJoint2);
	DDX_Control(pDX, IDC_CMD_LC_SUPPLY, m_ctrlLcSupply);
	DDX_Control(pDX, IDC_CMD_BUBBLE_REMOVAL1, m_ctrlBubbleRemoval);
	DDX_Control(pDX, IDC_DEGAS_SUCTION_DISTANCE, m_ctrlDegasSucPos);
	DDX_Control(pDX, IDC_DEGAS_SUCTION_SPEED, m_ctrlSuctionSpeed);
	DDX_Control(pDX, IDC_DEGAS_TIMES2, m_ctrlDegasTimes2);
	DDX_Control(pDX, IDC_CMD_GATE_HOME, m_ctrlGateHome);
	DDX_Control(pDX, IDC_CMD_GATE_OPEN, m_ctrlGateOpen);
	DDX_Control(pDX, IDC_CMD_GATE_CLOSE, m_ctrlGateClose);
	DDX_Control(pDX, IDC_CMD_PUMP_HOME, m_ctrlPumpHome);
	DDX_Control(pDX, IDC_CMD_SETTING, m_ctrlPumpTeach);
	DDX_Control(pDX, IDC_CMD_SPD_SAVE, m_ctrlPumpDataSave);
	DDX_Control(pDX, IDC_DEGAS_TIMES1, m_ctrlDegasTimes1);
	DDX_Control(pDX, IDC_DEGAS_DISCHARGE_SPEED, m_ctrlDischargeSpeed);
	DDX_Control(pDX, IDC_CMD_LC_INIT_SUPPLY_JOB, m_ctrlInitSupplyJob);
	DDX_Control(pDX, IDC_CMD_LC_NORMAL_SUPPLY_JOB2, m_ctrlNormalSupplyJob);
	DDX_Control(pDX, IDC_INIT_SUPPLY_TIME, m_ctrlInitSupplyTime);
	DDX_Control(pDX, IDC_NORMAL_SUPPLY_TIME, m_ctrlNormalSupplyTime);
	DDX_Control(pDX, IDC_MANUAL_ALARM_WEIGHT, m_ctrlManuCanWeight);
	DDX_Control(pDX, IDC_DEGAS_N_DISHCARGE_SPEED1, m_ctrlNDischargeSpeed1);
	DDX_Control(pDX, IDC_DEGAS_N_DISHCARGE_SPEED2, m_ctrlNDischargeSpeed2);
	DDX_Control(pDX, IDC_DEGAS_N_SUCTION_DISTANCE, m_ctrlNDegasSucPos);
	DDX_Control(pDX, IDC_CMD_N_BUBBLE_REMOVAL, m_ctrlNDegas);
	DDX_Control(pDX, IDC_N_DEGAS_TIMES, m_ctrlNDegasTimes);
	DDX_Control(pDX, IDC_N_OVER_TIME, m_ctrlNOverTime);
	DDX_Control(pDX, IDC_CMD_N_AGING, m_ctrlNAging);
	DDX_Control(pDX, IDC_N_AGING_TIMES, m_ctrlNAgingTimes);
	DDX_Control(pDX, IDC_N_AGING_SUPPLY_TIME, m_ctrlNAgingSupplyTime);
	DDX_Control(pDX, IDC_N_AGING_CYCLE, m_ctrlNAgingCycle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdjustInitial, CDialog)
	//{{AFX_MSG_MAP(CAdjustInitial)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_RBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdjustInitial message handlers

BOOL CAdjustInitial::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
//
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();

	//

	SelectLanguage();

	CString str,str1 = "";
	int i = 0 , j = 0;

	dRemovalDistance = 0.0;
	nSuctionSpeed = 0;
	nDischargeSpeed = 0;
	nRemovalTimes1 = 0;
	nRemovalTimes2 = 0;	
	nInitSupplyTime = 0;
	nNormalSupplyTime = 0;
	dManuCanWeight = 0.0;

	//by shin//2013.01.15//n-type add//
	dNRemovalDistance = 3.0;
	dNDischargeSpeed1 = 50.0;
	dNDischargeSpeed2 = 450.0;
	nNRemovalTimes = 3;
	nNOverTime = 0;
	nNAgingTimes = 1;

	//20210224 him
	nNAgingSupplyTime = 1; 
	nNAgingCycle = 1;
	
	SubDisplayRecipeName();

	//Manual LC Supply or »ç¿ë ÇÒ Canister°¡ ¾øÀ» °æ¿ì...//
	if((pDoc->m_structDataEditor.m_nLcSupplyType != 1) || ((pDoc->m_nCanisterBalNo[0] == 0)&&(pDoc->m_nCanisterBalNo[1] == 0))) 
	{
		m_ctrlInitSupplyJob.EnableWindow(false);
		m_ctrlNormalSupplyJob.EnableWindow(false);
		m_ctrlInitSupplyTime.EnableWindow(false);
		m_ctrlNormalSupplyTime.EnableWindow(false);
		m_ctrlManuCanWeight.EnableWindow(false);
	}
	else
	{
		for(i = 0; i < MAX_NOZZLE; i++)
		{
			if(Drop_Info.manu_head_job[i] == TRUE)
			{
				if((pDoc->m_structDataEditor.m_strBarcode[i] != CIM_Information.sCanisterTypeData[0])
									|| (pDoc->m_structDataEditor.m_strBarcode[i] != CIM_Information.sCanisterTypeData[1]))
				{
					m_ctrlInitSupplyJob.EnableWindow(false);
					m_ctrlNormalSupplyJob.EnableWindow(false);
					m_ctrlInitSupplyTime.EnableWindow(false);
					m_ctrlNormalSupplyTime.EnableWindow(false);
					m_ctrlManuCanWeight.EnableWindow(false);

					break;
				}
			}
		}
	}

	GetDlgItem(IDC_CMD_PUMP_JOINT1)->MoveWindow(0,0,0,0);

/// AdjustInitialSet
	// caption t¼³Á¤ºÎºÐ..
	for(i = 0 ; i < MAX_NOZZLE/2 ; i++)
	{
		str.Format("H%d",i+1);
		m_ctrlAdjustInitialSet.SetTextMatrix(0, (MAX_NOZZLE/2)-(i) ,str);		
	}
	for(i = 0 ; i < MAX_NOZZLE/2 ; i++)
	{
		str.Format("H%d",i+1+MAX_NOZZLE/2);
		m_ctrlAdjustInitialSet.SetTextMatrix(5, (MAX_NOZZLE/2)-(i) ,str);
	}

	if(pView->m_nLanguage == 0)
	{
		m_ctrlAdjustInitialSet.SetTextMatrix(1, 0, "ÆßÇÁ ¸ðÅÍ À§Ä¡(mm)");
		m_ctrlAdjustInitialSet.SetTextMatrix(2, 0, "ÃæÁø À§Ä¡(mm)");
		m_ctrlAdjustInitialSet.SetTextMatrix(3, 0, "°ÔÀÌÆ® ¿É¼Â(Pulse)");
		m_ctrlAdjustInitialSet.SetTextMatrix(4, 0, "N-±âÆ÷Á¦°Å");

		m_ctrlAdjustInitialSet.SetTextMatrix(6, 0, "ÆßÇÁ ¸ðÅÍ À§Ä¡(mm)");
		m_ctrlAdjustInitialSet.SetTextMatrix(7, 0, "ÃæÁø À§Ä¡(mm)");
		m_ctrlAdjustInitialSet.SetTextMatrix(8, 0, "°ÔÀÌÆ® ¿É¼Â(Pulse)");
		m_ctrlAdjustInitialSet.SetTextMatrix(9, 0, "N-±âÆ÷Á¦°Å");
	}
	else if(pView->m_nLanguage == 1)
	{
		m_ctrlAdjustInitialSet.SetTextMatrix(1, 0, "Pump Motor Pos(mm)");
		m_ctrlAdjustInitialSet.SetTextMatrix(2, 0, "Charge Pos(mm)");
		m_ctrlAdjustInitialSet.SetTextMatrix(3, 0, "Gate Offset(Pulse)");
		m_ctrlAdjustInitialSet.SetTextMatrix(4, 0, "N-Bubble Removal");

		m_ctrlAdjustInitialSet.SetTextMatrix(6, 0, "Pump Motor Pos(mm)");
		m_ctrlAdjustInitialSet.SetTextMatrix(7, 0, "Charge Pos(mm)");
		m_ctrlAdjustInitialSet.SetTextMatrix(8, 0, "Gate Offset(Pulse)");
		m_ctrlAdjustInitialSet.SetTextMatrix(9, 0, "N-Bubble Removal");
	}
	else if(pView->m_nLanguage == 2)
	{
		m_ctrlAdjustInitialSet.SetTextMatrix(1, 0, "Pumpï³Ñ¦êÈöÇ(mm)");
		m_ctrlAdjustInitialSet.SetTextMatrix(2, 0, "õöòäêÈöÇ(mm)");
		m_ctrlAdjustInitialSet.SetTextMatrix(3, 0, "Gate Offset(Pulse)");
		m_ctrlAdjustInitialSet.SetTextMatrix(4, 0, "N-Ëäð¶Ñ¨øÜ");

		m_ctrlAdjustInitialSet.SetTextMatrix(6, 0, "Pumpï³Ñ¦êÈöÇ(mm)");
		m_ctrlAdjustInitialSet.SetTextMatrix(7, 0, "õöòäêÈöÇ(mm)");
		m_ctrlAdjustInitialSet.SetTextMatrix(8, 0, "Gate Offset(Pulse)");
		m_ctrlAdjustInitialSet.SetTextMatrix(9, 0, "N-Ëäð¶Ñ¨øÜ");
	}

	// column width ¹× row height ¼³Á¤
	m_ctrlAdjustInitialSet.SetColWidth(0, 2500);
	m_ctrlAdjustInitialSet.SetColWidth(5, 2500);

	m_ctrlAdjustInitialSet.SetRowHeight(0,500);
	m_ctrlAdjustInitialSet.SetRowHeight(5,500);

	for(i=1 ; i < 5 ; i++) m_ctrlAdjustInitialSet.SetRowHeight(i,350);
	for(i=6 ; i < 10 ; i++) m_ctrlAdjustInitialSet.SetRowHeight(i,350);

	for(i=1 ; i < 9 ; i++) 	m_ctrlAdjustInitialSet.SetColWidth(i,1550);

/*	for(i=0; i<MAX_NOZZLE/2 ;i++)
	{
		m_ctrlAdjustInitialSet.SetRow(0);

		m_ctrlAdjustInitialSet.SetCol(i+1);

//		if(Drop_Info.manu_head_job[i] == TRUE)
		if(pDoc->m_bIsHeadSelected[i]==TRUE) 
			m_ctrlAdjustInitialSet.SetCellBackColor(GREEN);
		else
			m_ctrlAdjustInitialSet.SetCellBackColor(WHITEGRAY);
	}

	for(i=0; i<MAX_NOZZLE/2 ;i++)
	{
		m_ctrlAdjustInitialSet.SetRow(4);

		m_ctrlAdjustInitialSet.SetCol(i+1);

//		if(Drop_Info.manu_head_job[i+MAX_NOZZLE/2] == TRUE)
		if(pDoc->m_bIsHeadSelected[i+MAX_NOZZLE/2]==TRUE) 
			m_ctrlAdjustInitialSet.SetCellBackColor(GREEN);
		else
			m_ctrlAdjustInitialSet.SetCellBackColor(WHITEGRAY);
	}
*/
	
	for(i =0 ; i <MAX_NOZZLE/2; i ++)
	{
		str.Format("%.5f", Drop_Info.m_dAdjustAbsPos[i]*VOLUME_RESOLUTION);
		str1.Format("%.5f", Drop_Info.m_dAdjustAbsPos[i+MAX_NOZZLE/2]*VOLUME_RESOLUTION);
		m_ctrlAdjustInitialSet.SetTextMatrix(2, MAX_NOZZLE/2-i ,(LPCSTR)str);
		m_ctrlAdjustInitialSet.SetTextMatrix(7, MAX_NOZZLE/2-i ,(LPCSTR)str1);
	}

	for(i = 0; i < MAX_NOZZLE/2; i++)
	{
		str.Format("%d", Drop_Info.m_nGateOffset[i]);
		str1.Format("%d", Drop_Info.m_nGateOffset[i+MAX_NOZZLE/2]);
		m_ctrlAdjustInitialSet.SetTextMatrix(3, MAX_NOZZLE/2-i ,(LPCSTR)str);
		m_ctrlAdjustInitialSet.SetTextMatrix(8, MAX_NOZZLE/2-i ,(LPCSTR)str1);
	}

	if(pDoc->m_structDataEditor.m_nNzlMode == 1) //n-type ±Ø¹Ì¼¼ ³ëÁñÀÎ °æ¿ì...//
	{
		for(i=0; i< MAX_NOZZLE ; i++)
		{
			if( i < MAX_NOZZLE/2 )
			{
				if(Drop_Info.m_bDegasNtypeStatus[i] == FALSE)
				{
					m_ctrlAdjustInitialSet.SetRow(4);
					m_ctrlAdjustInitialSet.SetCol(MAX_NOZZLE/2-i);
					m_ctrlAdjustInitialSet.SetCellBackColor(RED);
					m_ctrlAdjustInitialSet.SetTextMatrix(4, MAX_NOZZLE/2-i, "NG");
					
				}
				else
				{
					m_ctrlAdjustInitialSet.SetRow(4);
					m_ctrlAdjustInitialSet.SetCol(MAX_NOZZLE/2-i);
					m_ctrlAdjustInitialSet.SetCellBackColor(LIGHTGREEN);
					m_ctrlAdjustInitialSet.SetTextMatrix(4, MAX_NOZZLE/2-i, "OK");
				}
			}
			else
			{
				if(Drop_Info.m_bDegasNtypeStatus[i] == FALSE)
				{
					m_ctrlAdjustInitialSet.SetRow(9);
					m_ctrlAdjustInitialSet.SetCol(MAX_NOZZLE-i);
					m_ctrlAdjustInitialSet.SetCellBackColor(RED);
					m_ctrlAdjustInitialSet.SetTextMatrix(9, MAX_NOZZLE-i, "NG");
					
				}
				else
				{
					m_ctrlAdjustInitialSet.SetRow(9);
					m_ctrlAdjustInitialSet.SetCol(MAX_NOZZLE-i);
					m_ctrlAdjustInitialSet.SetCellBackColor(LIGHTGREEN);
					m_ctrlAdjustInitialSet.SetTextMatrix(9, MAX_NOZZLE-i, "OK");
				}
			}
		}
	}
	else
	{
		for(i=0; i< MAX_NOZZLE/2 ; i++)
		{
			m_ctrlAdjustInitialSet.SetRow(4);
			m_ctrlAdjustInitialSet.SetCol(MAX_NOZZLE/2-i);
			m_ctrlAdjustInitialSet.SetCellBackColor(LIGHTGRAY);
			m_ctrlAdjustInitialSet.SetTextMatrix(4, MAX_NOZZLE/2-i, "None");
			m_ctrlAdjustInitialSet.SetRow(9);
			m_ctrlAdjustInitialSet.SetCol(MAX_NOZZLE/2-i);
			m_ctrlAdjustInitialSet.SetCellBackColor(LIGHTGRAY);
			m_ctrlAdjustInitialSet.SetTextMatrix(9, MAX_NOZZLE/2-i, "None");
		}
	}

	//
	for(i=0; i<MAX_NOZZLE/2 ;i++)
	{
		m_ctrlAdjustInitialSet.SetRow(0);
		m_ctrlAdjustInitialSet.SetCol(MAX_NOZZLE/2-i);
		m_ctrlAdjustInitialSet.SetColWidth(i+1, 1550);

		if(g_bInitilDisplay == FALSE)
		{
			if(Drop_Info.manu_head_job[i] == TRUE)
			{
				m_ctrlAdjustInitialSet.SetCellBackColor(GREEN);
			}
			else
			{
				m_ctrlAdjustInitialSet.SetCellBackColor(WHITEGRAY);
			}
		}
		else
		{
			if(pDoc->m_bIsHeadSelected[i]==TRUE)
			{
				m_ctrlAdjustInitialSet.SetCellBackColor(GREEN);
			}
			else
			{
				m_ctrlAdjustInitialSet.SetCellBackColor(WHITEGRAY);
			}
		}
	}
	for(i=0; i<MAX_NOZZLE/2 ;i++)
	{
		m_ctrlAdjustInitialSet.SetRow(5);
		m_ctrlAdjustInitialSet.SetCol(MAX_NOZZLE/2-i);
		m_ctrlAdjustInitialSet.SetColWidth(i+1, 1550);
		//
		if(g_bInitilDisplay == FALSE)
		{
			if(Drop_Info.manu_head_job[i+MAX_NOZZLE/2] == TRUE)
			{
				m_ctrlAdjustInitialSet.SetCellBackColor(GREEN);
			}
			else
			{
				m_ctrlAdjustInitialSet.SetCellBackColor(WHITEGRAY);
			}
		}
		else
		{
			if(pDoc->m_bIsHeadSelected[i+MAX_NOZZLE/2]==TRUE)
			{
				m_ctrlAdjustInitialSet.SetCellBackColor(GREEN);
			}
			else
			{
				m_ctrlAdjustInitialSet.SetCellBackColor(WHITEGRAY);
			}
		}
	}

	for(i =0 ; i <MAX_NOZZLE/2; i ++)
	{
		str.Format("%.5f", Drop_Info.m_dAdjustAbsPos[i]*VOLUME_RESOLUTION);
		m_ctrlAdjustInitialSet.SetTextMatrix(2, MAX_NOZZLE/2-i ,(LPCSTR)str);
	}
	for(i =0 ; i <MAX_NOZZLE/2; i ++)
	{
		str.Format("%.5f", Drop_Info.m_dAdjustAbsPos[i+MAX_NOZZLE/2]*VOLUME_RESOLUTION);
		m_ctrlAdjustInitialSet.SetTextMatrix(7, MAX_NOZZLE/2-i ,(LPCSTR)str);
	}
	
	for(i = 0; i < MAX_NOZZLE/2; i++)
	{
		str.Format("%d", Drop_Info.m_nGateOffset[i]);
		m_ctrlAdjustInitialSet.SetTextMatrix(3, MAX_NOZZLE/2-i ,(LPCSTR)str);
	}
	for(i = 0; i < MAX_NOZZLE/2; i++)
	{
		str.Format("%d", Drop_Info.m_nGateOffset[i+MAX_NOZZLE/2]);
		m_ctrlAdjustInitialSet.SetTextMatrix(8, MAX_NOZZLE/2-i ,(LPCSTR)str);
	}

	if(bModeSelect == TRUE)
	{
		m_ctrlModeSel.SetBackColor(GREEN);

		if(pView->m_nLanguage == 0)
		m_ctrlModeSel.SetWindowText("Auto äÌíû");
		else if(pView->m_nLanguage == 1)
		m_ctrlModeSel.SetWindowText("Auto Joint");
		else if(pView->m_nLanguage == 2)
		m_ctrlModeSel.SetWindowText("Auto äÌíû");

	}
	else
	{
		m_ctrlModeSel.SetBackColor(RED);

		if(pView->m_nLanguage == 0)
		m_ctrlModeSel.SetWindowText("Manual äÌíû");
		else if(pView->m_nLanguage == 0)
		m_ctrlModeSel.SetWindowText("Manual Joint");
		else if(pView->m_nLanguage == 0)
		m_ctrlModeSel.SetWindowText("Manual äÌíû");

	}	
	SubColorControl();

	//
	if(ThreadStage.AdjustInitCode=='A'+1)	m_ctrlLcSupply.SetBackColor(GREEN);	// LC Supply step // B
	else if(ThreadStage.AdjustInitCode=='A'+2)	m_ctrlBubbleRemoval.SetBackColor(GREEN);	// removal bubble step2B //C
	else if(ThreadStage.AdjustInitCode=='A'+3)	m_ctrlDisjoint.SetBackColor(GREEN);	// Disjoint step //D
	else if(ThreadStage.AdjustInitCode=='A'+4)	m_ctrlJoint1.SetBackColor(GREEN);	// Joint step1 //E
	else if(ThreadStage.AdjustInitCode=='A'+5)	m_ctrlJoint2.SetBackColor(GREEN);	// Joint step2 //F
	else if(ThreadStage.AdjustInitCode=='A'+7)	m_ctrlInitSupplyJob.SetBackColor(GREEN);	// ÃÊ±â ¾×Á¤ °ø±Þ //H
	else if(ThreadStage.AdjustInitCode=='A'+8)	m_ctrlNormalSupplyJob.SetBackColor(GREEN);	// Normal ¾×Á¤ °ø±Þ //I
	else if(ThreadStage.AdjustInitCode=='N')	m_ctrlNDegas.SetBackColor(GREEN); // N-type ±âÆ÷ Á¦°Å //
	else if(ThreadStage.AdjustInitCode=='T')    m_ctrlNAging.SetBackColor(GREEN); // N_Type Aging // 2015.08.29 by tskim
	//

	ThreadStage.bUseInterLock=false;

	if(ThreadStage.bUseInterLock==TRUE) 
	{
		m_ctrlUseInterlock.SetBackColor(GREEN);

		if(pView->m_nLanguage == 0)
		m_ctrlUseInterlock.SetCaption("INTERLOCK »ç¿ë");
		else if(pView->m_nLanguage ==1)
		m_ctrlUseInterlock.SetCaption("INTERLOCK Use");
		else if(pView->m_nLanguage == 2)
		m_ctrlUseInterlock.SetCaption("INTERLOCK îêéÄ");
	}
	else 
	{
		 m_ctrlUseInterlock.SetBackColor(RED);

		if(pView->m_nLanguage == 0)
		m_ctrlUseInterlock.SetCaption("INTERLOCK »ç¿ë¾ÈÇÔ");
		else if(pView->m_nLanguage == 1)
		m_ctrlUseInterlock.SetCaption("INTERLOCK Not Use");
		else if(pView->m_nLanguage == 2)
		m_ctrlUseInterlock.SetCaption("INTERLOCK ÜôîêéÄ");
	}

	for(i=0;i<MAX_NOZZLE/2;i++)
	{
		FAS_GetAxisStatus(pDoc->m_structHeadConfig[i].nA[0], pDoc->m_structHeadConfig[i].nA[1], &m_lAdjustMotorStatus[i]);
		m_ctrlAdjustInitialSet.SetRow(1);
		m_ctrlAdjustInitialSet.SetCol(MAX_NOZZLE/2-i);
		if(m_lAdjustMotorStatus[i]&LIMITDETECT)
		{
			m_ctrlAdjustInitialSet.SetCellBackColor(RED);// SetBackColor(RED);
		}
		else 
		{
			m_ctrlAdjustInitialSet.SetCellBackColor(YELLOW);//.SetBackColor(YELLOW);	
		}
	}
	for(i=0;i<MAX_NOZZLE/2;i++)
	{
		FAS_GetAxisStatus(pDoc->m_structHeadConfig[i+MAX_NOZZLE/2].nA[0], pDoc->m_structHeadConfig[i+MAX_NOZZLE/2].nA[1], &m_lAdjustMotorStatus[i+MAX_NOZZLE/2]);
		m_ctrlAdjustInitialSet.SetRow(6);
		m_ctrlAdjustInitialSet.SetCol(MAX_NOZZLE/2-i);
		if(m_lAdjustMotorStatus[i+MAX_NOZZLE/2]&LIMITDETECT)
		{
			m_ctrlAdjustInitialSet.SetCellBackColor(RED);// SetBackColor(RED);
		}
		else 
		{
			m_ctrlAdjustInitialSet.SetCellBackColor(YELLOW);//.SetBackColor(YELLOW);	
		}
	}
	SPDDataDisplay();

	//by shin//2009.08.25//MC °ü·Ã TAS Ãß°¡...//
	//Adjust Initial
	pView->WriteTasMCData(TAS_MC, 6, BIT_ON); 
	Sleep(200);

	if(pDoc->m_bIsEngineerMode==FALSE)
	{
		GetDlgItem(IDC_CMD_SETTING)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	}
	//Pump ÆÄ¼Õ ¿ì·Á·Î ÀÌ ¹öÆ°Àº ¼û±ä´Ù...//
	GetDlgItem(IDC_CMD_PUMP_HOME)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );

//2011.03.23 by tskim
	pView->m_bAllHeadDisJointDone = FALSE;
	for(i=0;i<MAX_NOZZLE;i++)
		pView->m_bHeadDisJointDone[i] = FALSE;

SetTimer(TIMER_ADJUSTINIT,500,NULL);			// SubTimerAdjustInitialFunc()

	// ¿ëÀû¸ðÅÍ À§Ä¡Ç¥½Ã¿ë timer½ÃÀÛ..
	if(PC_TYPE == TRUE) 
	{
		SetTimer(TIMER_ADJUSTINIT,500,NULL);			// SubTimerAdjustInitialFunc()
		SetTimer(TIMER_ADJUSTINIT_VOLDISP,200,NULL);	// SubTimerAdjustInitialVolDispFunc()
		SetTimer(TIMER_ADJUSTINIT_LM_DISP,1000,NULL);	// SubTimerAdjustInitialLimitDisp()
	}

	if(pView->m_nMachineStatus == 0)
	{
		pView->m_pMcStatus = new CMcStatus();
		pView->m_pMcStatus->Create(this);
		pView->m_pMcStatus->ShowWindow(SW_SHOW);
	}

	pDoc->ReadNozzleCleanerData();

	if(pDoc->m_structDataEditor.m_nNzlMode != 1) //n-type ±Ø¹Ì¼¼ ³ëÁñÀÌ ¾Æ´Ñ °æ¿ì...//
	{
		m_ctrlNDegasSucPos.SetEnabled(FALSE);
		m_ctrlNDischargeSpeed1.SetEnabled(FALSE);
		m_ctrlNDischargeSpeed2.SetEnabled(FALSE);
		m_ctrlNDegasTimes.SetEnabled(FALSE);
		m_ctrlNOverTime.SetEnabled(FALSE);
		m_ctrlNDegas.SetEnabled(FALSE);
		m_ctrlNAgingTimes.SetEnabled(FALSE);
		m_ctrlNAging.SetEnabled(FALSE);
	}
	else
	{
		m_ctrlBubbleRemoval.SetEnabled(FALSE);
		m_ctrlDegasTimes2.EnableWindow(FALSE);
	}

//	
//	GetDlgItem(IDC_LABEL3)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
//	GetDlgItem(IDC_N_AGING_TIMES)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
//	GetDlgItem(IDC_CMD_N_AGING)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAdjustInitial::OnPaint() 
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

void CAdjustInitial::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
//
	switch(nIDEvent)
	{
	case TIMER_ADJUSTINIT:
		SubTimerAdjustInitialFunc();
		break;
	case TIMER_ADJUSTINIT_VOLDISP:
		SubTimerAdjustInitialVolDispFunc();
		break;
	case TIMER_ADJUSTINIT_LM_DISP:
		SubTimerAdjustInitialLimitDisp();
		break;
	default:
		AfxMessageBox("¿©±â¿¡ debug code¸¦ ³Ö¾î¿ä..");
	} // end of switch()
//
	
	CDialog::OnTimer(nIDEvent);
}

void CAdjustInitial::OnOK() 
{
	// TODO: Add extra validation here
	
//	CDialog::OnOK();
}

void CAdjustInitial::OnCancel() 
{
	// TODO: Add extra cleanup here
	
//	CDialog::OnCancel();
}

BEGIN_EVENTSINK_MAP(CAdjustInitial, CDialog)
    //{{AFX_EVENTSINK_MAP(CAdjustInitial)
	ON_EVENT(CAdjustInitial, IDC_CMD_RETURN, -600 /* Click */, OnClickCmdReturn, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_CMD_INTERLOCKUSAGE, -600 /* Click */, OnClickCmdInterlockusage, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_MSFLEXGRID_ADJUSTINITIAL_SET, -600 /* Click */, OnClickMsflexgridAdjustinitialSet, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_CMD_MODE_SECLECT, -600 /* Click */, OnClickCmdModeSeclect, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_CMD_PISTON_DOWN, -600 /* Click */, OnClickCmdPistonDown, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_CMD_PUMP_DISJOINT, -600 /* Click */, OnClickCmdPumpDisjoint, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_CMD_PUMP_JOINT1, -600 /* Click */, OnClickCmdPumpJoint1, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_CMD_PUMP_JOINT2, -600 /* Click */, OnClickCmdPumpJoint2, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_CMD_LC_SUPPLY, -600 /* Click */, OnClickCmdLcSupply, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_CMD_BUBBLE_REMOVAL1, -600 /* Click */, OnClickCmdBubbleRemoval1, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_DEGAS_SUCTION_DISTANCE, -600 /* Click */, OnClickDegasSuctionDistance, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_DEGAS_SUCTION_SPEED, -600 /* Click */, OnClickDegasSuctionSpeed, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_DEGAS_DISCHARGE_SPEED, -600 /* Click */, OnClickDegasDischargeSpeed, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_DEGAS_TIMES1, -600 /* Click */, OnClickDegasTimes1, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_DEGAS_TIMES2, -600 /* Click */, OnClickDegasTimes2, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_CMD_GATE_CLOSE, -600 /* Click */, OnClickCmdGateClose, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_CMD_GATE_HOME, -600 /* Click */, OnClickCmdGateHome, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_CMD_GATE_OPEN, -600 /* Click */, OnClickCmdGateOpen, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_CMD_PUMP_HOME, -600 /* Click */, OnClickCmdPumpHome, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_CMD_SETTING, -600 /* Click */, OnClickCmdSetting, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_CMD_SPD_SAVE, -600 /* Click */, OnClickCmdSpdSave, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_CMD_LC_INIT_SUPPLY_JOB, -600 /* Click */, OnClickCmdLcInitSupplyJob, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_CMD_LC_NORMAL_SUPPLY_JOB2, -600 /* Click */, OnClickCmdLcNormalSupplyJob2, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_INIT_SUPPLY_TIME, -600 /* Click */, OnClickInitSupplyTime, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_NORMAL_SUPPLY_TIME, -600 /* Click */, OnClickNormalSupplyTime, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_MANUAL_ALARM_WEIGHT, -600 /* Click */, OnClickManualAlarmWeight, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_LABEL_RECIPE, -600 /* Click */, OnClickLabelRecipe, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_CMD_N_BUBBLE_REMOVAL, -600 /* Click */, OnClickCmdNBubbleRemoval, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_DEGAS_N_SUCTION_DISTANCE, -600 /* Click */, OnClickDegasNSuctionDistance, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_DEGAS_N_DISHCARGE_SPEED1, -600 /* Click */, OnClickDegasNDishcargeSpeed1, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_DEGAS_N_DISHCARGE_SPEED2, -600 /* Click */, OnClickDegasNDishcargeSpeed2, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_N_DEGAS_TIMES, -600 /* Click */, OnClickNDegasTimes, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_N_OVER_TIME, -600 /* Click */, OnClickNOverTime, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_CMD_N_AGING, -600 /* Click */, OnClickCmdNAging, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_N_AGING_TIMES, -600 /* Click */, OnClickNAgingTimes, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_N_AGING_SUPPLY_TIME, -600 /* Click */, OnClickNAgingSupplyTime, VTS_NONE)
	ON_EVENT(CAdjustInitial, IDC_N_AGING_CYCLE, -600 /* Click */, OnClickNAgingCycle, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CAdjustInitial::OnClickMsflexgridAdjustinitialSet() 
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	int col = 0 , row = 0;
	double max = 0.0 , min = 0.0;
	CString str = "";
	BOOL bSingleRow = TRUE;
	BOOL bTmpJobHeadExist = FALSE;
	int i = 0;

	row = m_ctrlAdjustInitialSet.GetMouseRow();
	col = m_ctrlAdjustInitialSet.GetMouseCol();

	// by ckh 2009.02.12 , ¿£Áö´Ï¾î ¸ðµå°¡ ¾Æ´Ï¸é ¼±ÅÃ¾ÈµÇ°Ô ÇÏ±â.
	if(row%5 != 0 && pDoc->m_bIsEngineerMode==FALSE)
	{
		return;
	}

	if( col == 0 )	
	{
		if(row == 0)
		{			
			for(i=0;i<MAX_NOZZLE/2;i++)
			{
				if((pDoc->m_bIsHeadSelected[i] == TRUE)&&(Drop_Info.manu_head_job[i]==TRUE))
				{
					bTmpJobHeadExist=true;
					break;
				}
			}
			if(bTmpJobHeadExist==TRUE)
			{
				for(i=0;i<MAX_NOZZLE/2;i++)
				{
					m_ctrlAdjustInitialSet.SetRow(0);
					m_ctrlAdjustInitialSet.SetCol(MAX_NOZZLE/2-i);
					Drop_Info.manu_head_job[i] = false;
					m_ctrlAdjustInitialSet.SetCellBackColor(WHITEGRAY);
				}
			}
			else
			{
				for(i=0;i<MAX_NOZZLE/2;i++)
				{
					m_ctrlAdjustInitialSet.SetRow(0);
					m_ctrlAdjustInitialSet.SetCol(MAX_NOZZLE/2-i);
					if(pDoc->m_bIsHeadSelected[i] == TRUE)
					{
						Drop_Info.manu_head_job[i] = true;
						m_ctrlAdjustInitialSet.SetCellBackColor(GREEN);
					}
					else
					{
						Drop_Info.manu_head_job[i] = false;
						m_ctrlAdjustInitialSet.SetCellBackColor(WHITEGRAY);
					}
				}
			}
			return;
		}
		else if(row == 5)
		{
			for(i=0;i<MAX_NOZZLE/2;i++)
			{
				if((pDoc->m_bIsHeadSelected[i+MAX_NOZZLE/2] == TRUE)&&(Drop_Info.manu_head_job[i+MAX_NOZZLE/2]==TRUE))
				{
					bTmpJobHeadExist=true;
					break;
				}
			}
			if(bTmpJobHeadExist==TRUE)
			{
				for(i=0;i<MAX_NOZZLE/2;i++)
				{
					m_ctrlAdjustInitialSet.SetRow(5);
					m_ctrlAdjustInitialSet.SetCol(MAX_NOZZLE/2-i);
					Drop_Info.manu_head_job[i+MAX_NOZZLE/2] = false;
					m_ctrlAdjustInitialSet.SetCellBackColor(WHITEGRAY);
				}
			}
			else
			{
				for(i=0;i<MAX_NOZZLE/2;i++)
				{
					m_ctrlAdjustInitialSet.SetRow(5);
					m_ctrlAdjustInitialSet.SetCol(MAX_NOZZLE/2-i);
					if(pDoc->m_bIsHeadSelected[i+MAX_NOZZLE/2] == TRUE)
					{
						Drop_Info.manu_head_job[i+MAX_NOZZLE/2] = true;
						m_ctrlAdjustInitialSet.SetCellBackColor(GREEN);
					}
					else
					{
						Drop_Info.manu_head_job[i+MAX_NOZZLE/2] = false;
						m_ctrlAdjustInitialSet.SetCellBackColor(WHITEGRAY);
					}
				}
			}
			return;
		}
	}

//	if(col == 0) return;
	if((row == 1) || (row == 6)) return;

	if(row == 0)
	{
		if(Drop_Info.manu_head_job[MAX_NOZZLE/2-col] == TRUE) 
		{
			Drop_Info.manu_head_job[MAX_NOZZLE/2-col] = FALSE;
			m_ctrlAdjustInitialSet.SetRow(0);
			m_ctrlAdjustInitialSet.SetCol(col);
			m_ctrlAdjustInitialSet.SetCellBackColor(WHITEGRAY);
		}
		else
		{
			Drop_Info.manu_head_job[MAX_NOZZLE/2-col] = TRUE;
			m_ctrlAdjustInitialSet.SetRow(0);
			m_ctrlAdjustInitialSet.SetCol(col);
			m_ctrlAdjustInitialSet.SetCellBackColor(GREEN);
		}
	}
	else if(row == 5)
	{
		if(Drop_Info.manu_head_job[MAX_NOZZLE/2-col+(MAX_NOZZLE/2)] == TRUE) 
		{
			Drop_Info.manu_head_job[MAX_NOZZLE/2-col+(MAX_NOZZLE/2)] = FALSE;
			m_ctrlAdjustInitialSet.SetRow(5);
			m_ctrlAdjustInitialSet.SetCol(col);
			m_ctrlAdjustInitialSet.SetCellBackColor(WHITEGRAY);
		}
		else
		{
			Drop_Info.manu_head_job[MAX_NOZZLE/2-col+(MAX_NOZZLE/2)] = TRUE;
			m_ctrlAdjustInitialSet.SetRow(5);
			m_ctrlAdjustInitialSet.SetCol(col);
			m_ctrlAdjustInitialSet.SetCellBackColor(GREEN);
		}
	}

	if(m_ctrlAdjustInitialSet.GetMouseRow() == FALSE ) return; 

	if(m_ctrlAdjustInitialSet.GetMouseCol() == FALSE)	bSingleRow = FALSE;	

	if(m_ctrlAdjustInitialSet.GetRow() == 0 || m_ctrlAdjustInitialSet.GetRow() == 1)
	{
		return;
	}
	if(m_ctrlAdjustInitialSet.GetRow() == 5 || m_ctrlAdjustInitialSet.GetRow() == 6)
	{
		return;
	}
	if(m_ctrlAdjustInitialSet.GetRow() == 4 || m_ctrlAdjustInitialSet.GetRow() == 9)
	{
		return;
	}

	
	if((row == 2) || (row == 7))
	{ 
		max = 25.0 ; min = 0.0; 
	}
	else if((row == 3) || (row == 8))
	{
		max = 70; min = -20;
	}
	Use_TKg(m_ctrlAdjustInitialSet, max, min, 512, 384, TRUE, bSingleRow);

/*	ThreadStage.nHeadSelected = CheckIfOnlyOneHead();

	if(ThreadStage.nHeadSelected==0)
	{
		if(ThreadStage.AdjustInitCode=='C')
		{
			SubColorControl();
			ThreadStage.AdjustInitCode=0;
		}
	}*/
}

//////////////////////////////////////////////////////////////////////////////////
void CAdjustInitial::OnClickCmdReturn() 
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();

	//Áß¾Ó °ø±Þ ¾Ð·ÂÀ» Normal °ø±Þ »óÅÂ·Î º¯°æ...//0.2mpa//
	FAS_SetIoBit(3,false,SUPPLY_CHANGE_SOL,true);//2010.02.23 by tskim true->false
	Sleep(10);

	g_bManualSupply = FALSE;

	//by shin//2009.08.25//MC °ü·Ã TAS Ãß°¡...//
	//µ¿ÀÛ bit off
	pView->WriteTasMCData(TAS_MC, 1, BIT_OFF);
	Sleep(10);
	pView->WriteTasMCData(TAS_MC, 4, BIT_OFF);
	Sleep(10);
	pView->WriteTasMCData(TAS_MC, 7, BIT_OFF);
	Sleep(10);
	pView->WriteTasMCData(TAS_MC, 8, BIT_OFF);
	Sleep(10);

	//by shin//2009.08.25//MC °ü·Ã TAS Ãß°¡...//
	//Adjust Initial
	pView->WriteTasMCData(TAS_MC, 6, BIT_OFF);
	Sleep(10);

	if(PC_TYPE == TRUE)
	{
		KillTimer(TIMER_ADJUSTINIT);
		KillTimer(TIMER_ADJUSTINIT_VOLDISP);
		KillTimer(TIMER_ADJUSTINIT_LM_DISP);
		//
		pView->m_pDevice->Start_button_onoff(false);
		pView->m_pDevice->Stop_button_onoff(false);
	}
	ThreadStage.bUseInterLock=TRUE;
	if(!pSettingAssembleDlg) delete pSettingAssembleDlg;
	
	if(pView->m_nMachineStatus == 0)
	{
		pView->m_pMcStatus->KillTimer(0);
		pView->m_pMcStatus->CloseWindow();
	}

	EndDialog(IDOK);
	
}

void CAdjustInitial::SubDisplayRecipeName()
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	CString str = "";

	// Recipe Name ¼³Á¤
	str.Format("%s|%s", pDoc->m_structOperatorModeRecipeData.strFrontRecipeName, pDoc->m_structOperatorModeRecipeData.strBackRecipeName);

	m_ctrlRecipeName.SetCaption(str);
}

void CAdjustInitial::SubDisplayVolumnPosition()
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();

	CString str = "";
	double dPos = 0.0;
//	int ibdid = 0 , iaxis = 0 , head_num = 0;
	int iheadindex=0;
	long AxisStatus=0;
	int i;
	BOOL bIoResult3;
	
// adjust motor ÀÇ ÇöÀç À§Ä¡°ªÀ» motion board¿¡¼­ ÀÐ¾î¿Í¼­ Ç¥½ÃÇÑ´Ù. Limit¿¡ °¨Áö µÇ¾î ÀÖÀ¸¸é »¡°£»öÀ¸·Î Ç¥½ÃÇØ ÁØ´Ù..
	for(iheadindex=0; iheadindex< MAX_NOZZLE/2 ; iheadindex++)
	{
		FAS_GetCommandPos(pDoc->m_structHeadConfig[iheadindex].nA[0], pDoc->m_structHeadConfig[iheadindex].nA[1], &dPos);  
//#if EQ
		str.Format("%.3f",dPos*VOLUME_RESOLUTION);
//#else
//		dPos = iheadindex+1.0;
//		str.Format("%.3f",dPos);
//#endif
		m_ctrlAdjustInitialSet.SetTextMatrix(1,(MAX_NOZZLE/2+1)-(iheadindex+1),str);
	}
	for(iheadindex=0; iheadindex< MAX_NOZZLE/2 ; iheadindex++)
	{
		FAS_GetCommandPos(pDoc->m_structHeadConfig[iheadindex+MAX_NOZZLE/2].nA[0], pDoc->m_structHeadConfig[iheadindex+MAX_NOZZLE/2].nA[1], &dPos);  
//#if EQ
		str.Format("%.3f",dPos*VOLUME_RESOLUTION);
//#else
//		dPos = iheadindex+9.0;
//		str.Format("%.3f",dPos);
//#endif	
		m_ctrlAdjustInitialSet.SetTextMatrix(6,(MAX_NOZZLE/2+1)-(iheadindex+1),str);
	}
//2011.03.23 by tskim Joint½Ã Pump NO Reset ÇÔ..
	if(pView->m_bAllHeadDisJointDone)
	{
		for(i=0;i<MAX_NOZZLE;i++)
		{
			bIoResult3 = FALSE;
			if(pDoc->m_bIsHeadSelected[i] && pView->m_bHeadDisJointDone[i])
			{
				FAS_GetIoBit(1,true,LC_LIMIT1+i,&bIoResult3);
				if(!bIoResult3)
					pDoc->m_strPumpNo[i] = "0000";
			}
		}
	}
}

void CAdjustInitial::SubTimerAdjustInitialFunc()
{
	//
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	CString str="";
	BOOL bRun=false,bRun1=false,bRun2=false;
	BOOL bMiniDoorSafetyOK=FALSE;
	DWORD dwIOResultF=0;
	BOOL bIOResult=FALSE,  bDoorSafeOK=FALSE, bAreaSafeOK=FALSE;
	BOOL bIOResult1=FALSE;
	BOOL bIOResult2=FALSE;
	BOOL bIOResult3=FALSE;
	BOOL bIOResult4=FALSE;
	WORD wIOResult=0, wTempIO = 0;
	int i = 0;
	int nUsedHead1 = 0;
	int nUsedHead2 = 0;
//
	m_nTimerCount++;

	/////////////////////////////////////////////

	bRun1 = false;	bRun2 = false;
//	bRun1 = ThBal[BALID1].bRunning | ThBal[BALID2].bRunning | ThBal[BALID3].bRunning | ThBal[BALID4].bRunning | ThBal[BALID5].bRunning | ThBal[BALID6].bRunning;
	bRun2 = ThreadStage.bMachineRunning;
//	bRun = bRun1 | bRun2;
	bRun = bRun2;
//	if(bRun1)
//	{
//		str=ThBal[BALID1].strMsg1 + ThBal[BALID2].strMsg1 + ThBal[BALID3].strMsg1 + ThBal[BALID4].strMsg1 + ThBal[BALID5].strMsg1 + ThBal[BALID6].strMsg1;
//		m_ctrlMessageAdjustInitial.SetCaption(str);		
//	}
	if(bRun2)
	{
		m_ctrlMessageAdjustInitial.SetCaption( ThreadStage.strMsg1);
	}

	if(bRun == TRUE)
	{
		if(pView->m_pDevice->SST_Check(STOP_SWITCH)) 
		{
			Sleep(100);
			if(pView->m_pDevice->SST_Check(STOP_SWITCH)) 
			{
										m_bSWCheck = false;//2010.01.19
//				for(int ibd = 1 ; ibd <=4 ; ibd++) 
//					for(int iaxis = 0 ; iaxis <16 ; iaxis++) 
//						FAS_MoveStop(ibd,iaxis,0);		// all axis stop..
				//
				ThBal[BALID1].ExitFlag = true;		ThBal[BALID2].ExitFlag = true;
				ThBal[BALID3].ExitFlag = true;		ThBal[BALID4].ExitFlag = true;
				ThBal[BALID5].ExitFlag = true;		ThBal[BALID6].ExitFlag = true;
				ThBal[BALID7].ExitFlag = true;		ThBal[BALID8].ExitFlag = true;
				ThreadStage.ExitFlag = true; // ** exit **
				AssemblePosMoveFlag = FALSE;
				bInitialStopCheck = TRUE;

				// by ckh 2009.02.12 ½ºÅÜ ¹×  ¹öÆ° ÃÊ±âÈ­ Ãß°¡ÇÏ±â.
			}
		}
/////////////////
		FAS_GetIo(1,false,&dwIOResultF);
		FAS_GetIoBit(1,false,ROBOT_ARM_DETECT,&bIOResult);	// Robot Arm Check

		FAS_GetIoBit(1,false,DOOR_SENSOR1,&bIOResult1);	// Robot Arm Check
		FAS_GetIoBit(1,false,DOOR_SENSOR2,&bIOResult2);	// Robot Arm Check
		FAS_GetIoBit(1,false,DOOR_SENSOR3,&bIOResult3);	// Robot Arm Check
		FAS_GetIoBit(1,false,MAINT_DOOR,&bIOResult4);	// Robot Arm Check


			
		if(ThreadStage.bUseInterLock==TRUE)
		{
			if(bIOResult1 == TRUE) bDoorSafeOK=FALSE;  
			else if(bIOResult2 == TRUE) bDoorSafeOK=FALSE;  
			else if(bIOResult3 == TRUE) bDoorSafeOK=FALSE; 
			else if(bIOResult4 == TRUE) bDoorSafeOK=FALSE; 
			else bDoorSafeOK=TRUE;
		}
		else
		{
			if(bIOResult2 == TRUE) bDoorSafeOK=FALSE;  
			else if(bIOResult3 == TRUE) bDoorSafeOK=FALSE; 
			else bDoorSafeOK=TRUE;
		}
//#if TRF
		if(!pDoc->m_structDataEditor.m_bSafetyLock)//2010.02.12 by tskim safety unlock
		{
			bDoorSafeOK = TRUE;
		}
		if(bDoorSafeOK==FALSE) 
		{
			ThreadStage.bEmergencyStop=TRUE; ThreadStage.ExitFlag = TRUE; ThreadStage.nHomeEnableCount = 0;  //ÇöÂù : ¹®À» ¿­¾ú´Ù°í ÇØ¼­ ¿øÁ¡À» ´Ù½Ã Àâ°Ô ÇÏ¸é ¹®Á¦°¡ ÀÖÀ» ¼öµµ ÀÖÀ» °Å¶ó »ý°¢ÇÔ.
			ThBal[BALID1].ExitFlag = true;		ThBal[BALID2].ExitFlag = true;
			ThBal[BALID3].ExitFlag = true;		ThBal[BALID4].ExitFlag = true;
			ThBal[BALID5].ExitFlag = true;		ThBal[BALID6].ExitFlag = true;
			ThBal[BALID7].ExitFlag = true;		ThBal[BALID8].ExitFlag = true;

			AssemblePosMoveFlag = FALSE;
			bInitialStopCheck = TRUE;

			pView->m_pDevice->Emergency_Stop();
///////////////////////////////////////////////////////////////20180830 jeongyong - Door No Ãß°¡				
			int nDoorNo= 0;
			if(bIOResult1==TRUE) nDoorNo=1;
			else if(bIOResult2==TRUE) nDoorNo=2;			
			else if(bIOResult3==TRUE) nDoorNo=3;
			else if(bIOResult4==TRUE) nDoorNo=4;

			pView->SendMessage(WM_ERROR,101,nDoorNo);
			return;
		}
		else if(bIOResult==TRUE)
		{
			ThreadStage.bEmergencyStop=TRUE; ThreadStage.ExitFlag = TRUE; ThreadStage.nHomeEnableCount = 0;  //ÇöÂù : ¹®À» ¿­¾ú´Ù°í ÇØ¼­ ¿øÁ¡À» ´Ù½Ã Àâ°Ô ÇÏ¸é ¹®Á¦°¡ ÀÖÀ» ¼öµµ ÀÖÀ» °Å¶ó »ý°¢ÇÔ.
			ThBal[BALID1].ExitFlag = true;		ThBal[BALID2].ExitFlag = true;
			ThBal[BALID3].ExitFlag = true;		ThBal[BALID4].ExitFlag = true;
			ThBal[BALID5].ExitFlag = true;		ThBal[BALID6].ExitFlag = true;
			ThBal[BALID7].ExitFlag = true;		ThBal[BALID8].ExitFlag = true;

			AssemblePosMoveFlag = FALSE;
			bInitialStopCheck = TRUE;

			pView->m_pDevice->Emergency_Stop();
			pView->SendMessage(WM_ERROR,104,NULL);			
			return;
		}
//#endif
/////////////////

		// ÇöÀç Run »óÅÂ¿¡¼­ Start ¹öÆ°À» On ½ÃÄÑ ³õ´Â´Ù.
		pView->m_pDevice->Start_button_onoff(true);
		pView->m_pDevice->Stop_button_onoff(false);
		m_nTimerCount = 0;

		//
		if(m_nTimerCount == 2) pView->m_pDevice->Stop_button_onoff(true);
		else if(m_nTimerCount == 4) 
		{
			pView->m_pDevice->Stop_button_onoff(false);
			pView->m_pDevice->Start_button_onoff(false);
			m_nTimerCount = 0;
		}
		//
//		m_ctrlReturn.EnableWindow(false);
		OnButtonSet(false);

		if(ThreadSupplyBal1.nTimerCount > -1) ThreadSupplyBal1.nTimerCount++; // ThreadBal1
		if(ThreadSupplyBal2.nTimerCount > -1) ThreadSupplyBal2.nTimerCount++; // ThreadBal2
	}
	else
	{
		if(pView->m_pDevice->SST_Check(START_SWITCH)) 
		{

//////////////////////////////////////////////////20181130 jeongyong - KEY »óÅÂ Ã¼Å©
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
			
			m_bSWCheck = false;//2010.01.19
			DWORD wIOResultF = 0, wIOResultR = 0 ;
			BOOL bIOResult=FALSE;
			BOOL bIOResult1=FALSE;
			BOOL bIOResult2=FALSE;
			BOOL bIOResult3=FALSE;
			BOOL bIOResult4=FALSE;
			// by ckh 2009.01.17
			// ÇÑ¹ø ´õ ÃÊ±âÈ­ÇÔ.
			// Stop ¹öÆ° »ç¿ë½Ã¿¡´Â TRUE°¡ µÊÀ¸·Î Ãß°¡ÇÔ.
			bInitialStopCheck = FALSE; 
			FAS_GetIo(1,false,&wIOResultF); FAS_GetIo(1,false,&wIOResultR);
			
			FAS_GetIoBit(1,false,DOOR_SENSOR1,&bIOResult1);	// Robot Arm Check
			FAS_GetIoBit(1,false,DOOR_SENSOR2,&bIOResult2);	// Robot Arm Check
			FAS_GetIoBit(1,false,DOOR_SENSOR3,&bIOResult3);	// Robot Arm Check
			FAS_GetIoBit(1,false,MAINT_DOOR,&bIOResult4);	// Robot Arm Check
			FAS_GetIoBit(1,false,ROBOT_ARM_DETECT,&bIOResult);	// Robot Arm Check
			
//			bMiniDoorSafetyOK=pView->MiniDoorSafetyOK(FALSE);
			if(ThreadStage.bUseInterLock==TRUE)
			{
				if(bIOResult1 == TRUE) bDoorSafeOK=FALSE;  
				else if(bIOResult2 == TRUE) bDoorSafeOK=FALSE;  
				else if(bIOResult3 == TRUE) bDoorSafeOK=FALSE; 
				else if(bIOResult4 == TRUE) bDoorSafeOK=FALSE; 
				else bDoorSafeOK=TRUE;

//				if((wIOResultF|DOOR_SENSOR1234)==DOOR_SENSOR1234) bDoorSafeOK=FALSE;
//				else bDoorSafeOK=TRUE;
			}
			else
			{
				if(bIOResult2 == TRUE) bDoorSafeOK=FALSE;  
				else if(bIOResult3 == TRUE) bDoorSafeOK=FALSE; 
				else bDoorSafeOK=TRUE;

//				if((dwIOResultF&DOOR_SENSOR23)==DOOR_SENSOR23) bDoorSafeOK=FALSE;
//				else bDoorSafeOK=TRUE;
			}
			if(!pDoc->m_structDataEditor.m_bSafetyLock)//2010.02.12 by tskim safety unlockÀÎ °æ¿ì Ã³¸®...
			{
				bDoorSafeOK = TRUE;
			}
			if(bDoorSafeOK==FALSE) 
			{
				ThreadStage.ExitFlag = TRUE;
				ThBal[BALID1].ExitFlag = true;		ThBal[BALID2].ExitFlag = true;
				ThBal[BALID3].ExitFlag = true;		ThBal[BALID4].ExitFlag = true;
				ThBal[BALID5].ExitFlag = true;		ThBal[BALID6].ExitFlag = true;
				ThBal[BALID7].ExitFlag = true;		ThBal[BALID8].ExitFlag = true;
///////////////////////////////////////////////////////////////20180830 jeongyong - Door No Ãß°¡
				int nDoorNo= 0;
				if(bIOResult1==TRUE) nDoorNo=1;
				else if(bIOResult2==TRUE) nDoorNo=2;			
				else if(bIOResult3==TRUE) nDoorNo=3;
				else if(bIOResult4==TRUE) nDoorNo=4;

				pView->SendMessage(WM_ERROR,101,nDoorNo);									
			}
			else if(bIOResult==TRUE)
			{
				ThreadStage.ExitFlag = TRUE;
				ThBal[BALID1].ExitFlag = true;		ThBal[BALID2].ExitFlag = true;
				ThBal[BALID3].ExitFlag = true;		ThBal[BALID4].ExitFlag = true;
				ThBal[BALID5].ExitFlag = true;		ThBal[BALID6].ExitFlag = true;
				ThBal[BALID7].ExitFlag = true;		ThBal[BALID8].ExitFlag = true;

				pView->SendMessage(WM_ERROR,104,NULL);
			}			
			else
			{
				ThreadStage.ManualCode = 'U';

				if(ThreadStage.AdjustInitCode == 'H') //ÃÊ±â ¾×Á¤ Áß¾Ó °ø±Þ 
				{
					nUsedHead1 = MAX_NOZZLE;
					nUsedHead2 = 0;

					// Bottle À¯¹«¸¦ °Ë»çÇÏ¿© °ø±Þ ÁøÇà ÇÒÁö ºñ±³ÇÑ´Ù...//
					for(i = 0; i < MAX_NOZZLE/2; i++)
					{
						if((Drop_Info.manu_head_job[i] == TRUE)&&(pDoc->m_bIsHeadSelected[i] == TRUE))
						{
							nUsedHead1 = i;
							break;
						}
					}
					for(i = MAX_NOZZLE/2; i < MAX_NOZZLE; i++)
					{
						if((Drop_Info.manu_head_job[i] == TRUE)&&(pDoc->m_bIsHeadSelected[i] == TRUE))
						{
							nUsedHead2 = i;
							break;
						}
					}
					if((nUsedHead1 >= MAX_NOZZLE/2 ) || (nUsedHead1 < 0)) 
					{
						AfxMessageBox("°ø±Þ ÇÒ Head¸¦ ¼±ÅÃ ÇØ ÁÖ¼¼¿ä...!!");
						return;
					}
					if((nUsedHead2 >= MAX_NOZZLE ) || (nUsedHead2 < MAX_NOZZLE/2))
					{
						AfxMessageBox("°ø±Þ ÇÒ Head¸¦ ¼±ÅÃ ÇØ ÁÖ¼¼¿ä...!!");
						return;
					}

/*					wIOResult = 0x0000;	
					FAS_GetIoBit(1,true,LC_LIMIT1+nUsedHead1,&bIOResult);			
					if((!bIOResult) && (pDoc->m_bRemainSensorUSE[nUsedHead1]==TRUE))
					{
						wTempIO=0x0001;
						wTempIO=wTempIO<<nUsedHead1;
						wIOResult+=wTempIO;
					}
					if(wIOResult!=0)		// BottleÀÌ ¾øÀ» ½Ã ¾Ë¶÷ ¹ß»ý...//
					{		
						ThreadStage.ExitFlag = true;
						ThBal[BALID1].ExitFlag = true;		ThBal[BALID2].ExitFlag = true;
						ThBal[BALID3].ExitFlag = true;		ThBal[BALID4].ExitFlag = true;
						ThBal[BALID5].ExitFlag = true;		ThBal[BALID6].ExitFlag = true;

						pView->SendMessage(WM_ERROR,17,wIOResult);
						return;				
					}

					wIOResult = 0x0000;	
					FAS_GetIoBit(1,true,LC_LIMIT1+nUsedHead2,&bIOResult);			
					if((!bIOResult) && (pDoc->m_bRemainSensorUSE[nUsedHead2]==TRUE))
					{
						wTempIO=0x0001;
						wTempIO=wTempIO<<nUsedHead2;
						wIOResult+=wTempIO;
					}
					if(wIOResult!=0)		// BottleÀÌ ¾øÀ» ½Ã ¾Ë¶÷ ¹ß»ý...//
					{		
						ThreadStage.ExitFlag = true;
						ThBal[BALID1].ExitFlag = true;		ThBal[BALID2].ExitFlag = true;
						ThBal[BALID3].ExitFlag = true;		ThBal[BALID4].ExitFlag = true;
						ThBal[BALID5].ExitFlag = true;		ThBal[BALID6].ExitFlag = true;

						pView->SendMessage(WM_ERROR,17,wIOResult);
						return;				
					}
*/
				}
/*				else if(ThreadStage.AdjustInitCode == 'I') //Normal ¾×Á¤ Áß¾Ó °ø±Þ//
				{
					wIOResult = 0x0000;	
					for(i=0; i<MAX_NOZZLE ;i++)
					{
						if(Drop_Info.manu_head_job[i] == TRUE)
						{
							FAS_GetIoBit(1,true,LC_LIMIT1+i,&bIOResult);			
							if((!bIOResult) && (pDoc->m_bRemainSensorUSE[i]==TRUE))
							{
								wTempIO=0x0001;
								wTempIO=wTempIO<<i;
								wIOResult+=wTempIO;
							}
						}
					}

					if(wIOResult!=0)		// BottleÀÌ ¾øÀ» ½Ã ¾Ë¶÷ ¹ß»ý...//
					{		
						ThreadStage.ExitFlag = true;
						ThBal[BALID1].ExitFlag = true;		ThBal[BALID2].ExitFlag = true;
						ThBal[BALID3].ExitFlag = true;		ThBal[BALID4].ExitFlag = true;
						ThBal[BALID5].ExitFlag = true;		ThBal[BALID6].ExitFlag = true;

						pView->SendMessage(WM_ERROR,17,wIOResult);
						return;				
					}
				}
*/								
				if((ThreadStage.AdjustInitCode=='D')||(ThreadStage.AdjustInitCode=='E')||(ThreadStage.AdjustInitCode=='F'))
				{
					AssemblePosMoveFlag = TRUE;
				}
				ThreadStage.ManualJobStep = 0;

				//
				if((ThreadStage.AdjustInitCode<='F')&&(ThreadStage.AdjustInitCode>0))
				{
					m_ctrlReturn.EnableWindow(false);

					ThreadStage.JobFlag = STAGE_MANUAL;
					pView->RunThread(THREAD_STAGE);
					return;
				}
				else if((ThreadStage.AdjustInitCode == 'H')||(ThreadStage.AdjustInitCode == 'I')||(ThreadStage.AdjustInitCode == 'N') ||(ThreadStage.AdjustInitCode == 'T'))
				{
					m_ctrlReturn.EnableWindow(false);

					ThreadStage.JobFlag = STAGE_MANUAL;
					pView->RunThread(THREAD_STAGE);
					return;
				}
			 }
		}
		// Stop »óÅÂ¿¡¼­ 
		// Stop ¹öÆ°ÀÌ OnµÇµµ·Ï ÇÑ´Ù.
		pView->m_pDevice->Stop_button_onoff(true);
		// Start ¹öÆ°Àº ±ô¹Ú°Å¸®°Ô ÇÑ´Ù.
		if(m_nTimerCount==2) pView->m_pDevice->Start_button_onoff(true);
		else if(m_nTimerCount>=4) 
		{
			pView->m_pDevice->Start_button_onoff(false);
			m_nTimerCount = 0;
		}
		//
		if(m_bMoveFlag == FALSE)
		{
			OnButtonSet(true);
			// by ckh 2009.02.12 ½ºÅÜ ¹×  ¹öÆ° ÃÊ±âÈ­ Ãß°¡ÇÏ±â.
			
		}

		if(pDoc->m_structDataEditor.m_nNzlMode == 1)
		{
// 			for ( int nHeadCount = 0; nHeadCount < MAX_NOZZLE/2; nHeadCount++ )
// 			{
// 				if( Drop_Info.manu_head_job[nHeadCount] )
// 				{
// 					if( nHeadCount < MAX_NOZZLE/2 )
// 					{
// 						if( !Drop_Info.m_bDegasNtypeStatus[nHeadCount] )
// 						{
// 							m_ctrlAdjustInitialSet.SetRow(4);
// 							m_ctrlAdjustInitialSet.SetCol(MAX_NOZZLE/2-nHeadCount);
// 							m_ctrlAdjustInitialSet.SetCellBackColor(RED);
// 							m_ctrlAdjustInitialSet.SetTextMatrix(4, MAX_NOZZLE/2-nHeadCount, "NG");
// 						}
// 						else
// 						{
// 							m_ctrlAdjustInitialSet.SetRow(4);
// 							m_ctrlAdjustInitialSet.SetCol(MAX_NOZZLE/2-nHeadCount);
// 							m_ctrlAdjustInitialSet.SetCellBackColor(LIGHTGREEN);
// 							m_ctrlAdjustInitialSet.SetTextMatrix(4, MAX_NOZZLE/2-nHeadCount, "OK");							
// 						}
// 					}
// 					else
// 					{
// 						if( !Drop_Info.m_bDegasNtypeStatus[nHeadCount] )
// 						{
// 							m_ctrlAdjustInitialSet.SetRow(9);
// 							m_ctrlAdjustInitialSet.SetCol(MAX_NOZZLE-nHeadCount);
// 							m_ctrlAdjustInitialSet.SetCellBackColor(RED);
// 							m_ctrlAdjustInitialSet.SetTextMatrix(9, MAX_NOZZLE-nHeadCount, "NG");
// 						}
// 						else
// 						{
// 							m_ctrlAdjustInitialSet.SetRow(9);
// 							m_ctrlAdjustInitialSet.SetCol(MAX_NOZZLE-nHeadCount);
// 							m_ctrlAdjustInitialSet.SetCellBackColor(LIGHTGREEN);
// 							m_ctrlAdjustInitialSet.SetTextMatrix(9, MAX_NOZZLE-nHeadCount, "OK");							
// 						}
// 					}
// 				}
// 			}
		}

//		m_ctrlReturn.EnableWindow(true);

	}
	//////////////////////////////////////////////
}

void CAdjustInitial::SubTimerAdjustInitialVolDispFunc()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	//
	SubDisplayVolumnPosition();
}

void CAdjustInitial::SubColorControl()
{
	m_ctrlDisjoint.SetBackColor(LIGHTGRAY);
	m_ctrlJoint1.SetBackColor(LIGHTGRAY);
	m_ctrlJoint2.SetBackColor(LIGHTGRAY);
	m_ctrlLcSupply.SetBackColor(LIGHTGRAY);
	m_ctrlBubbleRemoval.SetBackColor(LIGHTGRAY);
	m_ctrlPumpTeach.SetBackColor(LIGHTGRAY);
	m_ctrlGateHome.SetBackColor(LIGHTGRAY);
	m_ctrlPumpHome.SetBackColor(LIGHTGRAY);
	m_ctrlGateOpen.SetBackColor(LIGHTGRAY);
	m_ctrlGateClose.SetBackColor(LIGHTGRAY);
	m_ctrlPistonClear.SetBackColor(LIGHTGRAY);
	m_ctrlInitSupplyJob.SetBackColor(LIGHTGRAY);
	m_ctrlNormalSupplyJob.SetBackColor(LIGHTGRAY);
	m_ctrlNDegas.SetBackColor(LIGHTGRAY);
	m_ctrlNAging.SetBackColor(LIGHTGRAY);
}

void CAdjustInitial::OnClickCmdInterlockusage() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

//  TEST
// 	double DropSpeed     = 2000; 
// 	double SuctionSpeed  = 150; 
// 
// 	int    LineDropCount = 255; 
// 	int    DropStartPos  = 0; 
// 
// 	pView->m_pDevice->StageLineFill(DropSpeed, SuctionSpeed, &LineDropCount, DropStartPos);


	if(ThreadStage.bUseInterLock==TRUE) 
	{
		ThreadStage.bUseInterLock=FALSE;
		m_ctrlUseInterlock.SetBackColor(RED); m_ctrlUseInterlock.SetCaption("INTERLOCK »ç¿ë ¾ÈÇÔ");
	}
	else
	{
		ThreadStage.bUseInterLock=TRUE;
		m_ctrlUseInterlock.SetBackColor(GREEN); m_ctrlUseInterlock.SetCaption("INTERLOCK »ç¿ë");
	}
}

void CAdjustInitial::SubTimerAdjustInitialLimitDisp()
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	int iheadindex=0;
	long AxisStatus=0;
	int i;

	for(iheadindex=0; iheadindex< MAX_NOZZLE/2 ; iheadindex++)
	{
		FAS_GetAxisStatus(pDoc->m_structHeadConfig[iheadindex].nA[0], pDoc->m_structHeadConfig[iheadindex].nA[1], &AxisStatus);  	
		if(AxisStatus!=m_lAdjustMotorStatus[iheadindex])
		{
			m_lAdjustMotorStatus[iheadindex]=AxisStatus;
//#if EQ
//#else
//			if(iheadindex == 0)
//				AxisStatus = LIMITDETECT;
//#endif
			m_ctrlAdjustInitialSet.SetRow(1);
			m_ctrlAdjustInitialSet.SetCol((MAX_NOZZLE/2+1)-(iheadindex+1));
			if(AxisStatus&LIMITDETECT)
				m_ctrlAdjustInitialSet.SetCellBackColor(RED);
			else 
				m_ctrlAdjustInitialSet.SetCellBackColor(WHITE);
		}
	}
	
	for(iheadindex=0; iheadindex< MAX_NOZZLE/2 ; iheadindex++)
	{
		FAS_GetAxisStatus(pDoc->m_structHeadConfig[iheadindex+MAX_NOZZLE/2].nA[0], pDoc->m_structHeadConfig[iheadindex+MAX_NOZZLE/2].nA[1], &AxisStatus);  	
		if(AxisStatus!=m_lAdjustMotorStatus[iheadindex+MAX_NOZZLE/2])
		{
			m_lAdjustMotorStatus[iheadindex+MAX_NOZZLE/2]=AxisStatus;
//#if EQ
//#else
//			if(iheadindex == 7)
//				AxisStatus = LIMITDETECT;
//#endif
			m_ctrlAdjustInitialSet.SetRow(6);
			m_ctrlAdjustInitialSet.SetCol((MAX_NOZZLE/2+1)-(iheadindex+1));
			if(AxisStatus&LIMITDETECT)
				m_ctrlAdjustInitialSet.SetCellBackColor(RED);
			else 
				m_ctrlAdjustInitialSet.SetCellBackColor(WHITE);
		}
	}

	if(pDoc->m_structDataEditor.m_nNzlMode == 1) //n-type ±Ø¹Ì¼¼ ³ëÁñÀÎ °æ¿ì...//
	{
		for(i=0; i< MAX_NOZZLE/2 ; i++)
			{
				if( !N_Nozzle_Detect_Flag[i] )
				{
					m_ctrlAdjustInitialSet.SetRow(4);
					m_ctrlAdjustInitialSet.SetCol(MAX_NOZZLE/2-i);
					m_ctrlAdjustInitialSet.SetCellBackColor(RED);
					m_ctrlAdjustInitialSet.SetTextMatrix(4, MAX_NOZZLE/2-i, "NG");
					
				}
				else
				{
					m_ctrlAdjustInitialSet.SetRow(4);
					m_ctrlAdjustInitialSet.SetCol(MAX_NOZZLE/2-i);
					m_ctrlAdjustInitialSet.SetCellBackColor(LIGHTGREEN);
					m_ctrlAdjustInitialSet.SetTextMatrix(4, MAX_NOZZLE/2-i, "OK");
				}
				
				if(!N_Nozzle_Detect_Flag[i+MAX_NOZZLE/2])
				{
					m_ctrlAdjustInitialSet.SetRow(9);
					m_ctrlAdjustInitialSet.SetCol(MAX_NOZZLE/2-i);
					m_ctrlAdjustInitialSet.SetCellBackColor(RED);
					m_ctrlAdjustInitialSet.SetTextMatrix(9, MAX_NOZZLE/2-i, "NG");
					
				}
				else
				{
					m_ctrlAdjustInitialSet.SetRow(9);
					m_ctrlAdjustInitialSet.SetCol(MAX_NOZZLE/2-i);
					m_ctrlAdjustInitialSet.SetCellBackColor(LIGHTGREEN);
					m_ctrlAdjustInitialSet.SetTextMatrix(9, MAX_NOZZLE/2-i, "OK");
				}
			}
	}
	else
	{
		for(i=0; i< MAX_NOZZLE/2 ; i++)
		{
			m_ctrlAdjustInitialSet.SetRow(4);
			m_ctrlAdjustInitialSet.SetCol(MAX_NOZZLE/2-i);
			m_ctrlAdjustInitialSet.SetCellBackColor(LIGHTGRAY);
			m_ctrlAdjustInitialSet.SetTextMatrix(4, MAX_NOZZLE/2-i, "None");
			m_ctrlAdjustInitialSet.SetRow(9);
			m_ctrlAdjustInitialSet.SetCol(MAX_NOZZLE/2-i);
			m_ctrlAdjustInitialSet.SetCellBackColor(LIGHTGRAY);
			m_ctrlAdjustInitialSet.SetTextMatrix(9, MAX_NOZZLE/2-i, "None");
		}
	}
}

int CAdjustInitial::CheckIfOnlyOneHead()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	int nCount=0;
	for(int i=0; i<MAX_NOZZLE;i++)
	{
		pDoc->m_bIsCompleted[i] = TRUE;
		if(Drop_Info.manu_head_job[i])
		{
			nCount++;		
			if(nCount>1)	return 0;
		}		
	}

	if(nCount==1)
	{
		for(i=0; i<MAX_NOZZLE;i++)
		{
			if(Drop_Info.manu_head_job[i])	
			{
				pDoc->m_bIsCompleted[i]=FALSE;
				return (i+1);
			}
		}
	}
	return 0;
}

void CAdjustInitial::OnClickCmdModeSeclect() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();



	// TODO: Add your control notification handler code here
	if(bModeSelect == TRUE)
	{
		bModeSelect = FALSE;
		m_ctrlModeSel.SetBackColor(RED);

		if(pView->m_nLanguage ==0)
		m_ctrlModeSel.SetWindowText("Manual ÀåÂø");
		else if(pView->m_nLanguage ==1)
		m_ctrlModeSel.SetWindowText("Manual Joint");
		else if(pView->m_nLanguage ==2)
		m_ctrlModeSel.SetWindowText("Manual äÌíû");
	}
	else
	{
		bModeSelect = TRUE;
		m_ctrlModeSel.SetBackColor(GREEN);

		if(pView->m_nLanguage ==0)
		m_ctrlModeSel.SetWindowText("Auto ÀåÂø");
		else if(pView->m_nLanguage ==1)
		m_ctrlModeSel.SetWindowText("Auto Joint");
		else if(pView->m_nLanguage ==2)
		m_ctrlModeSel.SetWindowText("Auto äÌíû");
	}			
}

void CAdjustInitial::OnClickCmdPistonDown() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	
	int i;
	BOOL bIoResult1=false , bIoResult2=false;

	CNormalMsg dlg;

	dlg.m_bTimer=FALSE;
	
	if(pView->m_nLanguage == 0)
	{
		dlg.m_strTitle = _T("È®ÀÎ");
		dlg.m_strMsg1 = " 'Alarm Head¸¦ ¼±ÅÃÇÏ¿© ÁÖ¼¼¿ä...' ÇÇ½ºÅæ ´Ù¿î µ¿ÀÛÀ» ÇÏ½Ã°Ú½À´Ï±î?";
		dlg.m_strMsg2 = " ½ÇÇàÇÏ·Á¸é 'OK', ±×·¸Áö ¾ÊÀ¸¸é 'CANCEL' ¹öÆ°À» ´©¸£¼¼¿ä...";
	}	

	else if(pView->m_nLanguage == 1)
	{
		dlg.m_strTitle = _T("Check");
		dlg.m_strMsg1 = " 'Select Alarm Head...' Will you Action to Down Piston?";
		dlg.m_strMsg2 = " if you Action 'OK', or Not Action Please Push Button 'CANCEL'...";
	}	
	
	else if(pView->m_nLanguage == 2)  // Áß±¹¾î
	{
		dlg.m_strTitle = _T("ü¬ìã");
		dlg.m_strMsg1 = " 'ôëàÔ÷ÉAlarm Head...' é©ñ®Pistonù»Ë½ÔÑíÂØ§?";
		dlg.m_strMsg2 = " åýâÍòûú¼ïÇÌª 'OK', ÜúöÎïÇÌªCANCEL...";
	}	

	if( dlg.DoModal() == IDOK )
	{
		OnButtonSet(false);
		m_ctrlPistonClear.SetBackColor(LIGHTRED);

		for(i=0;i<MAX_NOZZLE;i++)
		{
			if(pDoc->m_structHeadConfig[i].nA[0]==0) continue;

			if(Drop_Info.manu_head_job[i])
			{
				FAS_SetIoBit(3, TRUE, HEAD1_DW_SOL+i, FALSE);
				FAS_SetIoBit(2, FALSE, HEAD1_UP_SOL+i, FALSE);
			}
		}
		Sleep(1000);

		//Head Down Sol On//
		for(i=0;i<MAX_NOZZLE;i++)
		{
			if(pDoc->m_structHeadConfig[i].nA[0]==0) continue;

			if(Drop_Info.manu_head_job[i])
			{
				FAS_SetIoBit(3, TRUE, HEAD1_DW_SOL+i, TRUE);
				FAS_SetIoBit(2, FALSE, HEAD1_UP_SOL+i, FALSE);
			}
		}
		Sleep(1000);
		for(i = 0; i< MAX_NOZZLE;i++)
		{
			if(pDoc->m_structHeadConfig[i].nA[0]==0) continue;

			if(Drop_Info.manu_head_job[i])
			{
				//Head up/down sensor check//
				FAS_GetIoBit(2, FALSE, HEAD1_UP_SENSOR+i,&bIoResult1);
				FAS_GetIoBit(3, TRUE, HEAD1_DW_SENSOR+i,&bIoResult2);


				if(!((bIoResult1==FALSE)&&(bIoResult2==TRUE)))
				{
					pView->PostMessage(WM_ERROR,136,i+1);
					OnButtonSet(true);
					m_ctrlPistonClear.SetBackColor(LIGHTGRAY);
					return;
				}				
			}
		}
		OnButtonSet(true);
		m_ctrlPistonClear.SetBackColor(LIGHTGRAY);

//ÃÊ±âÈ­ ÇÏ¸é ¾ÈµÊ 
// 		for(i = 0; i<MAX_NOZZLE; i++)
// 		{
// 			//by shin//2013.01.21//n-type nozzle ¸ÎÈû °Ë»ç...//
// 			//ÀÛ¾÷ ÈÄ ÇØ´ç ÀÌ·Â clear...//´ÜÁö display ¿ëµµ·Î »ç¿ëÇÔÀ¸·Î...//
// 			N_Nozzle_Detect_Flag[i] = FALSE;
// 		}
	}
	else
	{
		return;
	}			
}

void CAdjustInitial::OnClickCmdPumpDisjoint() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	CNormalMsg dlg; 
	CString strTempLog = ""; 
	strTempLog = "[MANUAL] ÆßÇÁ ÇØÃ¼ µ¿ÀÛ ¹öÆ° Å¬¸¯"; //141113 LOG °­È­ 
	pView->SaveLog(0,strTempLog); 

	ThreadStage.AdjustInitCode='D';
	SubColorControl();
	m_ctrlDisjoint.SetBackColor(GREEN);		

	//by shin//2009.08.25//MC °ü·Ã TAS Ãß°¡...//
	//µ¿ÀÛ bit off
	pView->WriteTasMCData(TAS_MC, 4, BIT_OFF);
	Sleep(10);
	pView->WriteTasMCData(TAS_MC, 7, BIT_OFF);
	Sleep(10);
	pView->WriteTasMCData(TAS_MC, 8, BIT_OFF);
	Sleep(10);
	
	//by shin//2009.08.25//MC °ü·Ã TAS Ãß°¡...//
	//Pump Joint1/Disjoint
	pView->WriteTasMCData(TAS_MC, 1, BIT_ON);
	Sleep(200);		
}

void CAdjustInitial::OnClickCmdPumpJoint1() 
{
	// TODO: Add your control notification handler code here
	ThreadStage.AdjustInitCode='E';
	SubColorControl();
	m_ctrlJoint1.SetBackColor(GREEN);			
}

void CAdjustInitial::OnClickCmdPumpJoint2() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	
	int i;
	WORD wNoOfAxisG1 = 0; //G1~G12
	WORD wNoOfAxisG2 = 0; //G13~G16
	WORD wNoOfAxisP1 = 0; //P1~P14
	WORD wNoOfAxisP2 = 0; //P15~P16

	int iAxisG1[MAX_NOZZLE] = {0,}; //G motor	     
	int iAxisG2[MAX_NOZZLE] = {0,}; //G motor	     
	int iAxisP1[MAX_NOZZLE] = {0,}; //P motor
	int iAxisP2[MAX_NOZZLE] = {0,}; //P motor

	double dPos = 0.0;

	double dPos1G1[MAX_NOZZLE] = {0,};
	double dPos1G2[MAX_NOZZLE] = {0,};
	double dPos2G1[MAX_NOZZLE] = {0,};
	double dPos2G2[MAX_NOZZLE] = {0,};
	double dPos1P1[MAX_NOZZLE] = {0,};
	double dPos1P2[MAX_NOZZLE] = {0,};
	double dVelocity1G1[MAX_NOZZLE] = {0,};
	double dVelocity1G2[MAX_NOZZLE] = {0,};
	double dVelocity2G1[MAX_NOZZLE] = {0,};
	double dVelocity2G2[MAX_NOZZLE] = {0,};
	double dVelocity1P1[MAX_NOZZLE] = {0,};
	double dVelocity1P2[MAX_NOZZLE] = {0,};

	BOOL bIoResult1=false , bIoResult2=false, bIoResult3 = false;

	CNormalMsg dlg;
	if(bModeSelect == TRUE)
	{
		//Auto Assemble//
		dlg.m_bTimer=FALSE;

		if (pView->m_nLanguage == 0)
		{
			dlg.m_strTitle = _T("È®ÀÎ");
			dlg.m_strMsg1 = " 'ÀÚµ¿ Pump ÀåÂø' Pump ÀåÂøÀ» ÇÏ½Ã°Ú½À´Ï±î?";

			if(pDoc->m_structDataEditor.m_nNzlMode == 1)
				dlg.m_strMsg2 = " ½ÇÇàÇÏ·Á¸é 'OK', ±×·¸Áö ¾ÊÀ¸¸é 'CANCEL' ¹öÆ°À» ´©¸£¼¼¿ä...\nÀåÂø ÈÄ N CABLE ¿¬°á ÇÏ¼¼¿ä.";
			else
				dlg.m_strMsg2 = " ½ÇÇàÇÏ·Á¸é 'OK', ±×·¸Áö ¾ÊÀ¸¸é 'CANCEL' ¹öÆ°À» ´©¸£¼¼¿ä...";
		}

		else if (pView->m_nLanguage == 1)
		{
			dlg.m_strTitle = _T("Check");
			dlg.m_strMsg1 = " 'Auto Pump Joint' Will you Joint Pump?";
			if(pDoc->m_structDataEditor.m_nNzlMode == 1)
				dlg.m_strMsg2 = " if you Action 'OK', or Not Action Please Push Button 'CANCEL'...\n Connect N - Cable after ASSEMBLE ";
			else
				dlg.m_strMsg2 = " if you Action 'OK', or Not Action Please Push Button 'CANCEL'...";
		}
		
		else if (pView->m_nLanguage == 2)		//Áß±¹¾î
		{
			dlg.m_strTitle = _T("ü¬ìã");
			dlg.m_strMsg1 = " 'í»ÔÑäÌíû Pump' âÍé©äÌíûPumpØ§?";
			
			if(pDoc->m_structDataEditor.m_nNzlMode == 1)
				dlg.m_strMsg2 = " åýâÍòûú¼ïÇÌª 'OK', ÜúöÎïÇÌªCANCEL...\näÌíûý­Ö§ïÈ N CABLE.";
			else
				dlg.m_strMsg2 = " åýâÍòûú¼ïÇÌª 'OK', ÜúöÎïÇÌªCANCEL...";
		}

		if( dlg.DoModal() == IDOK )
		{
			ThreadStage.AdjustInitCode='F';
			SubColorControl();
			m_ctrlJoint2.SetBackColor(GREEN);

			//by shin//2009.08.25//MC °ü·Ã TAS Ãß°¡...//
			//µ¿ÀÛ bit off
			pView->WriteTasMCData(TAS_MC, 1, BIT_OFF);
			Sleep(10);
			pView->WriteTasMCData(TAS_MC, 7, BIT_OFF);
			Sleep(10);
			pView->WriteTasMCData(TAS_MC, 8, BIT_OFF);
			Sleep(10);

			//by shin//2009.08.25//MC °ü·Ã TAS Ãß°¡...//
			//Pump Joint2
			pView->WriteTasMCData(TAS_MC, 4, BIT_ON);
			Sleep(200);

			for(i=0;i<MAX_NOZZLE;i++)
			{
				pView->m_bAssemCheckOK[i] = FALSE;
				pView->m_bHeadJointDone[i] = FALSE;
			}
			pView->m_bAllHeadJointDone = FALSE;
		}
		else
		{
			return;
		}
	}
	else
	{
		//Manual Assemble//
		dlg.m_bTimer=FALSE;
		
		if (pView->m_nLanguage == 0)
		{
			dlg.m_strTitle = _T("È®ÀÎ");
			dlg.m_strMsg1 = " '¼öµ¿ Pump ÀåÂø' Pump ÀåÂøÀ» ÇÏ½Ã°Ú½À´Ï±î?";
			dlg.m_strMsg2 = " ½ÇÇàÇÏ·Á¸é 'OK', ±×·¸Áö ¾ÊÀ¸¸é 'CANCEL' ¹öÆ°À» ´©¸£¼¼¿ä...";
		}

		else if (pView->m_nLanguage == 1)
		{
			dlg.m_strTitle = _T("Check");
			dlg.m_strMsg1 = " 'Manual Pump Joint' Will you Joint Pump??";
			dlg.m_strMsg2 = " if you Action 'OK', or Not Action Please Push Button 'CANCEL'...";
		}

		else if (pView->m_nLanguage == 2)
		{
			dlg.m_strTitle = _T("ü¬ìã");
			dlg.m_strMsg1 = " 'â¢ÔÑäÌíû Pump' âÍé©äÌíûPumpØ§?";
			dlg.m_strMsg2 = " åýâÍòûú¼ïÇÌª 'OK', ÜúöÎïÇÌªCANCEL...";
		}

		if( dlg.DoModal() == IDOK )
		{
			SubColorControl();
			m_ctrlJoint2.SetBackColor(GREEN);
			OnButtonSet(false);

			//by shin//2009.08.25//MC °ü·Ã TAS Ãß°¡...//
			//µ¿ÀÛ bit off
			pView->WriteTasMCData(TAS_MC, 1, BIT_OFF);
			Sleep(10);
			pView->WriteTasMCData(TAS_MC, 7, BIT_OFF);
			Sleep(10);
			pView->WriteTasMCData(TAS_MC, 8, BIT_OFF);
			Sleep(10);		
			
			//by shin//2009.08.25//MC °ü·Ã TAS Ãß°¡...//
			//Pump Joint2
			pView->WriteTasMCData(TAS_MC, 4, BIT_ON);
			Sleep(200);

			//Gate Close Check//
			for(i = 0; i< MAX_NOZZLE;i++)
			{
				if(pDoc->m_structHeadConfig[i].nA[0]==0) continue;

				if(Drop_Info.manu_head_job[i])
				{
					//Gate Close Check//
					if(pView->GateCloseCheck(i) != i)
					{	
						pView->PostMessage(WM_ERROR,141,i+1);	
						OnButtonSet(true);
						return;	
					}
				}
			}

			wNoOfAxisG1 = 0;
			wNoOfAxisG2 = 0;
			wNoOfAxisP1 = 0;
			wNoOfAxisP2 = 0;

			//Gate Á¤º¸//
			for( i= 0; i < MAX_NOZZLE; i++)
			{					
				if(pDoc->m_structHeadConfig[i].nD[0]==0) continue;

				if(Drop_Info.manu_head_job[i])
				{
					if(i < NOZZLE_G1) 
					{
						iAxisG1[wNoOfAxisG1] = (AXIS_G1+i)%BOARD_AXES;
						dPos1G1[wNoOfAxisG1] = G_PULSE;
						dPos2G1[wNoOfAxisG1] = G_PULSE/2 + Drop_Info.m_nGateOffset[i];
						dVelocity1G1[wNoOfAxisG1] = 2000.0*G_PULSE/60;//2000RpmÀ¸·Î È¸Àü ½ÃÅ²´Ù.
						dVelocity2G1[wNoOfAxisG1] = pDoc->m_structAdjustCondition.dSpeed*G_PULSE/60;
						wNoOfAxisG1++;	
					}
					else 
					{
						iAxisG2[wNoOfAxisG2] = (AXIS_G13+(i%NOZZLE_G1))%BOARD_AXES;						
						dPos1G2[wNoOfAxisG2] = G_PULSE;
						dPos2G2[wNoOfAxisG2] = G_PULSE/2 + Drop_Info.m_nGateOffset[i];
						dVelocity1G2[wNoOfAxisG2] = 2000.0*G_PULSE/60;//2000RpmÀ¸·Î È¸Àü ½ÃÅ²´Ù.
						dVelocity2G2[wNoOfAxisG2] = pDoc->m_structAdjustCondition.dSpeed*G_PULSE/60;
						wNoOfAxisG2++;	
					}
				}
			}

			//Pump Á¤º¸//
			for( i= 0; i < MAX_NOZZLE; i++)
			{					
				if(pDoc->m_structHeadConfig[i].nA[0]==0) continue;

				if(Drop_Info.manu_head_job[i])
				{
					if(i < NOZZLE_P1) 
					{
						iAxisP1[wNoOfAxisP1] = (AXIS_P1+i)%BOARD_AXES;
						dPos1P1[wNoOfAxisP1] = Drop_Info.m_dBasePos[i];
						dVelocity1P1[wNoOfAxisP1] = 40000;
						wNoOfAxisP1++;	
					}
					else 
					{
						iAxisP2[wNoOfAxisP2] = (AXIS_P15+(i%NOZZLE_P1))%BOARD_AXES;
						dPos1P2[wNoOfAxisP2] = Drop_Info.m_dBasePos[i];
						dVelocity1P2[wNoOfAxisP2] = 40000;
						wNoOfAxisP2++;	
					}
				}
			}

			//Assemble Position Compare//
			for(i = 0; i < MAX_NOZZLE; i++)
			{
				if(Drop_Info.manu_head_job[i])
				{
					FAS_GetActualPos(pDoc->m_structHeadConfig[i].nA[0], pDoc->m_structHeadConfig[i].nA[1], &dPos);
					if(fabs(Drop_Info.m_dAssemPos[i]-dPos)>ERR_PULSE)
					{
						Sleep(P_DELAY);
						FAS_GetActualPos(pDoc->m_structHeadConfig[i].nA[0], pDoc->m_structHeadConfig[i].nA[1], &dPos);
						if(fabs(Drop_Info.m_dAssemPos[i]-dPos)>ERR_PULSE)
						{
							m_strMotionLog[i].Format("<M> Head %d P-Axis AssemPos %f Current Pos %f",i+1,Drop_Info.m_dAssemPos[i],dPos);
							pView->SaveLog(0,m_strMotionLog[i]);
							pView->PostMessage(WM_ERROR,144,i+1);							
							OnButtonSet(true);
							return;
						}
					}
				}
			}
						
			//Up/Down sol Off//
			for(i=0;i<MAX_NOZZLE;i++)
			{
				if(pDoc->m_structHeadConfig[i].nA[0]==0) continue;

				if(Drop_Info.manu_head_job[i])
				{
					FAS_SetIoBit(3, TRUE, HEAD1_DW_SOL+i, FALSE);
					FAS_SetIoBit(2, FALSE, HEAD1_UP_SOL+i, FALSE);
				}
			}
			Sleep(500);

			//Head Up Sol On//
			for(i=0;i<MAX_NOZZLE;i++)
			{
				if(pDoc->m_structHeadConfig[i].nA[0]==0) continue;

				if(Drop_Info.manu_head_job[i])
				{
					FAS_SetIoBit(3, TRUE, HEAD1_DW_SOL+i, FALSE);
					FAS_SetIoBit(2, FALSE, HEAD1_UP_SOL+i, TRUE);
				}
			}
			Sleep(1000);
			for(i = 0; i< MAX_NOZZLE;i++)
			{
				if(pDoc->m_structHeadConfig[i].nA[0]==0) continue;

				if(Drop_Info.manu_head_job[i])
				{
					//Head up/down sensor check//
					FAS_GetIoBit(2, FALSE, HEAD1_UP_SENSOR+i,&bIoResult1);
					FAS_GetIoBit(3, TRUE, HEAD1_DW_SENSOR+i,&bIoResult2);
					if(!((bIoResult1==TRUE)&&(bIoResult2==FALSE)))
					{
						pView->PostMessage(WM_ERROR,136,i+1);
						OnButtonSet(true);
						return;
					}				
				}
			}

			//Gate 360µµ È¸Àü 10È¸//
			for(int j = 0; j < 10; j++)
			{
 //				FAS_MoveMultiAxisIncPos(AXIS_G1/BOARD_AXES+1, wNoOfAxisG1, iAxisG1, dPos1G1, dVelocity1G1, TRUE);												
 //				FAS_MoveMultiAxisIncPos(AXIS_G13/BOARD_AXES+1, wNoOfAxisG2, iAxisG2, dPos1G2, dVelocity1G2, TRUE);	
 				FAS_MoveMultiAxisIncPos(AXIS_G1/BOARD_AXES+1, wNoOfAxisG1, iAxisG1, dPos1G1, dVelocity1G1, FALSE);												
 				FAS_MoveMultiAxisIncPos(AXIS_G13/BOARD_AXES+1, wNoOfAxisG2, iAxisG2, dPos1G2, dVelocity1G2, TRUE);	
				Sleep(100);
			}
			Sleep(200);

			//Gate Open//
//			FAS_MoveMultiAxisIncPos(AXIS_G1/BOARD_AXES+1, wNoOfAxisG1, iAxisG1, dPos2G1, dVelocity2G1, TRUE);
//			FAS_MoveMultiAxisIncPos(AXIS_G13/BOARD_AXES+1, wNoOfAxisG2, iAxisG2, dPos2G2, dVelocity2G2, TRUE);
			FAS_MoveMultiAxisIncPos(AXIS_G1/BOARD_AXES+1, wNoOfAxisG1, iAxisG1, dPos2G1, dVelocity2G1, FALSE);
			FAS_MoveMultiAxisIncPos(AXIS_G13/BOARD_AXES+1, wNoOfAxisG2, iAxisG2, dPos2G2, dVelocity2G2, TRUE);
			Sleep(100);

			for(i = 0; i< MAX_NOZZLE;i++)
			{
				if(pDoc->m_structHeadConfig[i].nA[0]==0) continue;

				if(Drop_Info.manu_head_job[i])
				{
					//Gate Open Check//
					if(pView->GateOpenCheck(i) != i)
					{	
						pView->PostMessage(WM_ERROR,142,i+1);	
						OnButtonSet(true);
						return;	
					}
				}
			}	
			
			//Base Position Move//
//			FAS_MoveMultiAxisAbsPos(AXIS_P1/BOARD_AXES+1, wNoOfAxisP1, iAxisP1, dPos1P1, dVelocity1P1, TRUE);	
//			FAS_MoveMultiAxisAbsPos(AXIS_P15/BOARD_AXES+1, wNoOfAxisP2, iAxisP2, dPos1P2, dVelocity1P2, TRUE);			
			FAS_MoveMultiAxisAbsPos(AXIS_P1/BOARD_AXES+1, wNoOfAxisP1, iAxisP1, dPos1P1, dVelocity1P1, FALSE);	
			FAS_MoveMultiAxisAbsPos(AXIS_P15/BOARD_AXES+1, wNoOfAxisP2, iAxisP2, dPos1P2, dVelocity1P2, TRUE);			

			Sleep(INPOSITION_DELAY);
				
			for(i = 0; i< MAX_NOZZLE;i++)
			{
				if(pDoc->m_structHeadConfig[i].nA[0]==0) continue;

				if(Drop_Info.manu_head_job[i])
				{
					if(i < NOZZLE_P1)							
						while(!pView->m_pDevice->MotionEndCheck(AXIS_P1+i));
					else
						while(!pView->m_pDevice->MotionEndCheck(AXIS_P1+i+2));
				}
			}

			//Base Position Compare//
			for(i = 0; i < MAX_NOZZLE; i++)
			{
				if(pDoc->m_structHeadConfig[i].nA[0]==0) continue;

				if(Drop_Info.manu_head_job[i])
				{
					FAS_GetActualPos(pDoc->m_structHeadConfig[i].nA[0], pDoc->m_structHeadConfig[i].nA[1], &dPos);
					if(fabs(Drop_Info.m_dBasePos[i] - dPos) > ERR_PULSE)
					{
						Sleep(P_DELAY);
						FAS_GetActualPos(pDoc->m_structHeadConfig[i].nA[0], pDoc->m_structHeadConfig[i].nA[1], &dPos);
						if(fabs(Drop_Info.m_dBasePos[i] - dPos) > ERR_PULSE)
						{
							m_strMotionLog[i].Format("<M> Head %d P-Axis BasePos %f Current Pos %f",i+1,Drop_Info.m_dBasePos[i],dPos);
							pView->SaveLog(0,m_strMotionLog[i]);
							pView->PostMessage(WM_ERROR,130,i+1);							
							OnButtonSet(true);
							return;	
						}
					}	
				}
			}

			//Gate 360µµ È¸Àü 10È¸//
			for(j = 0; j < 10; j++)
			{
 // 			FAS_MoveMultiAxisIncPos(AXIS_G1/BOARD_AXES+1, wNoOfAxisG1, iAxisG1, dPos1G1, dVelocity1G1, TRUE);												
 //				FAS_MoveMultiAxisIncPos(AXIS_G13/BOARD_AXES+1, wNoOfAxisG2, iAxisG2, dPos1G2, dVelocity1G2, TRUE);	
				FAS_MoveMultiAxisIncPos(AXIS_G1/BOARD_AXES+1, wNoOfAxisG1, iAxisG1, dPos1G1, dVelocity1G1, FALSE);												
 				FAS_MoveMultiAxisIncPos(AXIS_G13/BOARD_AXES+1, wNoOfAxisG2, iAxisG2, dPos1G2, dVelocity1G2, TRUE);	
				Sleep(100);
			}
			Sleep(200);


			for(i = 0; i< MAX_NOZZLE;i++)
			{
				if(pDoc->m_structHeadConfig[i].nA[0]==0) continue;

				if(Drop_Info.manu_head_job[i])
				{
					//Gate Open Check//
					if(pView->GateOpenCheck(i) != i)
					{	
						pView->PostMessage(WM_ERROR,142,i+1);	
						OnButtonSet(true);
						return;	
					}
//					OnButtonSet(true);
				}
			}

			//Pump ÀåÂø ¿Ï·á ÈÄ Gate MotorÀÇ Pulse¸¦ '0'·Î ÇÑ´Ù.//joint 2step¿¡¼­ÀÇ 360µµ È¸Àü¿¡ ÀÇÇÑ pulse ¼ö ½×ÀÌ´Â °Í ¹æÁö//
			//Gate Motor Set '0'//
			for(i = 0; i < MAX_NOZZLE; i++)
			{
				if(Drop_Info.manu_head_job[i])
				{
					FAS_SetCommandPos(pDoc->m_structHeadConfig[i].nD[0], pDoc->m_structHeadConfig[i].nD[1], 0);
				}
			}	
			g_nInitMeasureGlassCount = INIT_MEASURE_COUNT;
			
//2014.11.27 by tskim 1stMeasure Skip
			if(pDoc->m_structAdjustCondition.m_n1stMeas <= 0)
				g_nInitMeasureGlassCount = pDoc->m_structAdjustCondition.m_n2ndMeas;				
			else
				g_nInitMeasureGlassCount = pDoc->m_structAdjustCondition.m_n1stMeas;

			OnButtonSet(true);
		}
		else
		{
			return;
		}
	}			
}

void CAdjustInitial::OnClickCmdLcSupply() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	
	/////////////////////////////////////////////////////////////////
	// by ckh 2009.01.17
// 	nRemovalTimes1 = atoi(m_ctrlDegasTimes1.GetCaption());
// 
// 	for(int j = 0; j < MAX_NOZZLE; j++)
// 	{
// 		Drop_Info.m_nRemoveBubbleTimes1[j] = nRemovalTimes1;
// 	}
// 	pDoc->SaveSPDData();	
	/////////////////////////////////////////////////////////////////
	


	ThreadStage.AdjustInitCode='B';
	SubColorControl();
	m_ctrlLcSupply.SetBackColor(GREEN);	

	//by shin//2009.08.25//MC °ü·Ã TAS Ãß°¡...//
	//µ¿ÀÛ bit off
	pView->WriteTasMCData(TAS_MC, 1, BIT_OFF);
	Sleep(10);
	pView->WriteTasMCData(TAS_MC, 4, BIT_OFF);
	Sleep(10);
	pView->WriteTasMCData(TAS_MC, 8, BIT_OFF);
	Sleep(10);
	
	//by shin//2009.08.25//MC °ü·Ã TAS Ãß°¡...//
	//LC Supply
	pView->WriteTasMCData(TAS_MC, 7, BIT_ON);
	Sleep(200);
}

void CAdjustInitial::OnClickCmdBubbleRemoval1() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

//	return; //131217 ±âÆ÷ Á¦°Å µ¿ÀÛ ¸øÇÏµµ·Ï ...

	//////////////////////////////////////////////////////////////////
	// by ckh 2009.01.17
// 	nRemovalTimes2 = atoi(m_ctrlDegasTimes2.GetCaption());
// 
// 	for(int j = 0; j < MAX_NOZZLE; j++)
// 	{
// 		Drop_Info.m_nRemoveBubbleTimes2[j] = nRemovalTimes2;
// 	}
// 	pDoc->SaveSPDData();			
	//////////////////////////////////////////////////////////////////
	ThreadStage.AdjustInitCode='C';
	SubColorControl();
	m_ctrlBubbleRemoval.SetBackColor(GREEN);

	//by shin//2009.08.25//MC °ü·Ã TAS Ãß°¡...//
	//µ¿ÀÛ bit off
	pView->WriteTasMCData(TAS_MC, 1, BIT_OFF);
	Sleep(10);
	pView->WriteTasMCData(TAS_MC, 4, BIT_OFF);
	Sleep(10);
	pView->WriteTasMCData(TAS_MC, 7, BIT_OFF);
	Sleep(10);
	
	//by shin//2009.08.25//MC °ü·Ã TAS Ãß°¡...//
	//Degas
	pView->WriteTasMCData(TAS_MC, 8, BIT_ON);
	Sleep(200);
}

void CAdjustInitial::OnClickDegasSuctionDistance() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	
	Use_TK(m_ctrlDegasSucPos,5,0,0,0);// by ckh 2009.02.12 Max 10 ¼öÁ¤ //090702 ÃæÁø°Å¸® 2 -> 5 Max Value º¯°æ

	dRemovalDistance = atof(m_ctrlDegasSucPos.GetCaption());

	for(int j = 0; j < MAX_NOZZLE; j++)
	{
		Drop_Info.m_dRemoveBubblePos[j] = dRemovalDistance / VOLUME_RESOLUTION;
		Drop_Info.m_dRemoveBubblePos[j]= (double)pView->SubRoundAdjust(Drop_Info.m_dRemoveBubblePos[j]);
	}

	CString strLog;
	for(int i=0; i<MAX_NOZZLE; i++)		//jykim 170517 DegasSuctionDistance º¯°æ ½Ã BasePos °ª ·Î±× Ãß°¡
	{
		strLog.Format("OnClickDegasSuctionDistance - m_dBasePos[%d] = %.0f", i, Drop_Info.m_dBasePos[i]);
		pView->SaveLog(0,strLog);
	}								//jykim 170517 DegasSuctionDistance º¯°æ ½Ã BasePos °ª ·Î±× Ãß°¡

	pDoc->SaveSPDData();				
}

void CAdjustInitial::OnClickDegasSuctionSpeed() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	
	Use_TK(m_ctrlSuctionSpeed,350,0,0,0);// by ckh 2009.02.12 Max 200 ¼öÁ¤

	nSuctionSpeed = atoi(m_ctrlSuctionSpeed.GetCaption());

	for(int j = 0; j < MAX_NOZZLE; j++)
	{
		Drop_Info.m_nSuctionSpeed[j] = nSuctionSpeed * 1000;
	}

	CString strLog;
	for(int i=0; i<MAX_NOZZLE; i++)		//jykim 170517 DegasSuctionSpeed º¯°æ ½Ã BasePos °ª ·Î±× Ãß°¡
	{
		strLog.Format("OnClickDegasSuctionSpeed - m_dBasePos[%d] = %.0f", i, Drop_Info.m_dBasePos[i]);
		pView->SaveLog(0,strLog);
	}								//jykim 170517 DegasSuctionSpeed º¯°æ ½Ã BasePos °ª ·Î±× Ãß°¡
	pDoc->SaveSPDData();				
}

void CAdjustInitial::OnClickDegasDischargeSpeed() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	
	Use_TK(m_ctrlDischargeSpeed,450,0,0,0);// by ckh 2009.02.12 Max 200 ¼öÁ¤

	nDischargeSpeed = atoi(m_ctrlDischargeSpeed.GetCaption());

	for(int j = 0; j < MAX_NOZZLE; j++)
	{
		Drop_Info.m_nDischargeSpeed[j] = nDischargeSpeed * 1000;
	}
	CString strLog;
	for(int i=0; i<MAX_NOZZLE; i++)		//jykim 170517 DegasDischargeSpeed º¯°æ ½Ã BasePos °ª ·Î±× Ãß°¡
	{
		strLog.Format("OnClickDegasDischargeSpeed - m_dBasePos[%d] = %.0f", i, Drop_Info.m_dBasePos[i]);
		pView->SaveLog(0,strLog);
	}								//jykim 170517 DegasDischargeSpeed º¯°æ ½Ã BasePos °ª ·Î±× Ãß°¡
	pDoc->SaveSPDData();				
}

void CAdjustInitial::OnClickDegasTimes1() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	
	Use_TK(m_ctrlDegasTimes1,500,0,0,0);

	nRemovalTimes1 = atoi(m_ctrlDegasTimes1.GetCaption());

	for(int j = 0; j < MAX_NOZZLE; j++)
	{
		Drop_Info.m_nRemoveBubbleTimes1[j] = nRemovalTimes1;
	}
	CString strLog;
	for(int i=0; i<MAX_NOZZLE; i++)		//jykim 170517 DegasTimes1 º¯°æ ½Ã BasePos °ª ·Î±× Ãß°¡
	{
		strLog.Format("OnClickDegasTimes1 - m_dBasePos[%d] = %.0f", i, Drop_Info.m_dBasePos[i]);
		pView->SaveLog(0,strLog);
	}								//jykim 170517 DegasTimes1 º¯°æ ½Ã BasePos °ª ·Î±× Ãß°¡
	pDoc->SaveSPDData();	
}

void CAdjustInitial::OnClickDegasTimes2() 
{
	// TODO: Add your control notification handler code here	
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	
	Use_TK(m_ctrlDegasTimes2,5,0,0,0);

	nRemovalTimes2 = atoi(m_ctrlDegasTimes2.GetCaption());

	for(int j = 0; j < MAX_NOZZLE; j++)
	{
		Drop_Info.m_nRemoveBubbleTimes2[j] = nRemovalTimes2;
	}
	CString strLog;
	for(int i=0; i<MAX_NOZZLE; i++)		//jykim 170517 DegasTimes2 º¯°æ ½Ã BasePos °ª ·Î±× Ãß°¡
	{
		strLog.Format("OnClickDegasTimes2 - m_dBasePos[%d] = %.0f", i, Drop_Info.m_dBasePos[i]);
		pView->SaveLog(0,strLog);
	}								//jykim 170517 DegasTimes2 º¯°æ ½Ã BasePos °ª ·Î±× Ãß°¡
	pDoc->SaveSPDData();			
}

void CAdjustInitial::OnClickCmdGateClose() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	int i = 0;
	m_bMoveFlag = TRUE;
	OnButtonSet(false);
	m_ctrlGateClose.SetBackColor(LIGHTGREEN);
	pFrame->DoEvents();

	//by shin//2009.08.25//MC °ü·Ã TAS Ãß°¡...//
	//µ¿ÀÛ bit off
	pView->WriteTasMCData(TAS_MC, 1, BIT_OFF);
	Sleep(10);
	pView->WriteTasMCData(TAS_MC, 4, BIT_OFF);
	Sleep(10);
	pView->WriteTasMCData(TAS_MC, 7, BIT_OFF);
	Sleep(10);
	pView->WriteTasMCData(TAS_MC, 8, BIT_OFF);
	Sleep(10);

	//Gate Open Check//
	for(i=0;i<MAX_NOZZLE;i++)
	{
		if(Drop_Info.manu_head_job[i])
		{
			if(pView->GateOpenCheck(i) != i)
			{	
				pView->PostMessage(WM_ERROR,142,i+1);
				OnButtonSet(true);
				return;
			}
		}
	}

	WORD wNoOfAxisG1 = 0; //G1~G12
	WORD wNoOfAxisG2 = 0; //G13~G16

	int iAxisG1[MAX_NOZZLE] = {0,}; //G motor	     
	int iAxisG2[MAX_NOZZLE] = {0,}; //G motor	     

	double dPos1G1[MAX_NOZZLE] = {0,};
	double dPos1G2[MAX_NOZZLE] = {0,};

	double dVelocity1G1[MAX_NOZZLE] = {0,};
	double dVelocity1G2[MAX_NOZZLE] = {0,};

	for(i=0;i<MAX_NOZZLE;i++)
	{
		if(Drop_Info.manu_head_job[i])
		{
			if(i < NOZZLE_G1) 
			{
				iAxisG1[wNoOfAxisG1]=(AXIS_G1+i)%BOARD_AXES;
				dPos1G1[wNoOfAxisG1] = -(G_PULSE/2 + Drop_Info.m_nGateOffset[i]);
				dVelocity1G1[wNoOfAxisG1] = pDoc->m_structAdjustCondition.dSpeed*G_PULSE/60;
				wNoOfAxisG1++;
			}
			else
			{
				iAxisG2[wNoOfAxisG2]=(AXIS_G13+(i%NOZZLE_G1))%BOARD_AXES;
				dPos1G2[wNoOfAxisG2] = -(G_PULSE/2 + Drop_Info.m_nGateOffset[i]);
				dVelocity1G2[wNoOfAxisG2] = pDoc->m_structAdjustCondition.dSpeed*G_PULSE/60;
				wNoOfAxisG2++;
			}
		}
	}

	//Gate Close//
// 	FAS_MoveMultiAxisIncPos(AXIS_G1/BOARD_AXES+1, wNoOfAxisG1, iAxisG1, dPos1G1, dVelocity1G1, TRUE);												
// 	FAS_MoveMultiAxisIncPos(AXIS_G13/BOARD_AXES+1, wNoOfAxisG2, iAxisG2, dPos1G2, dVelocity1G2, TRUE);	
	FAS_MoveMultiAxisIncPos(AXIS_G1/BOARD_AXES+1, wNoOfAxisG1, iAxisG1, dPos1G1, dVelocity1G1, FALSE);												
 	FAS_MoveMultiAxisIncPos(AXIS_G13/BOARD_AXES+1, wNoOfAxisG2, iAxisG2, dPos1G2, dVelocity1G2, TRUE);	
	Sleep(100);
	
	//Gate Close Check//
	for(i=0;i<MAX_NOZZLE;i++)
	{
		if(Drop_Info.manu_head_job[i])
		{
			if(pView->GateCloseCheck(i) != i)
			{	
				pView->PostMessage(WM_ERROR,141,i+1);
				m_bMoveFlag = FALSE;
				OnButtonSet(true);
				return;
			}
		}
	}

	m_bMoveFlag = FALSE;
	OnButtonSet(true);

	m_ctrlGateClose.SetBackColor(LIGHTGRAY);

	pFrame->DoEvents();		
}

void CAdjustInitial::OnClickCmdGateHome() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	
	int k = 0;
	long AxisStatus;
	m_bMoveFlag = TRUE;
	OnButtonSet(false);
	m_ctrlGateHome.SetBackColor(LIGHTGREEN);

	//by shin//2009.08.25//MC °ü·Ã TAS Ãß°¡...//
	//µ¿ÀÛ bit off
	pView->WriteTasMCData(TAS_MC, 1, BIT_OFF);
	Sleep(10);
	pView->WriteTasMCData(TAS_MC, 4, BIT_OFF);
	Sleep(10);
	pView->WriteTasMCData(TAS_MC, 7, BIT_OFF);
	Sleep(10);
	pView->WriteTasMCData(TAS_MC, 8, BIT_OFF);
	Sleep(10);
	
	if(PC_TYPE == TRUE)
	{
		for(int iheadindex = 0 ; iheadindex < MAX_NOZZLE ;iheadindex++)
		{
			if(Drop_Info.manu_head_job[iheadindex])
			{
				pFrame->DoEvents();
				FAS_MoveOriginSingleAxis(pDoc->m_structHeadConfig[iheadindex].nD[0], pDoc->m_structHeadConfig[iheadindex].nD[1], 0);  	
			}
		}
		while(1)
		{
			k = 0;
			for(iheadindex = 0 ; iheadindex < MAX_NOZZLE ;iheadindex++)
			{
				if(Drop_Info.manu_head_job[iheadindex])
				{
					FAS_GetAxisStatus(pDoc->m_structHeadConfig[iheadindex].nD[0], pDoc->m_structHeadConfig[iheadindex].nD[1], &AxisStatus);  	
					if((AxisStatus & MOTIONING)== FALSE) k++;
				}
				else
					k++;
			}
			if(k >= MAX_NOZZLE) break;
			Sleep(100);
			if(pView->m_pDevice->SST_Check(STOP_SWITCH))
			{
				for(iheadindex = 0 ; iheadindex < MAX_NOZZLE ;iheadindex++)
					FAS_MoveStop(pDoc->m_structHeadConfig[iheadindex].nD[0], pDoc->m_structHeadConfig[iheadindex].nD[1],1);
				break;
			}
			pFrame->DoEvents();
		}

		//Gate Motor Set '0'//
		for(iheadindex = 0; iheadindex < MAX_NOZZLE; iheadindex++)
		{
			if(Drop_Info.manu_head_job[iheadindex])
			{
				FAS_MoveSingleAxisIncPos(pDoc->m_structHeadConfig[iheadindex].nD[0],pDoc->m_structHeadConfig[iheadindex].nD[1],
											GATE_ORIGIN_OFFSET,pDoc->m_structAdjustCondition.dSpeed*G_PULSE/60,1);
				Sleep(50);
				FAS_SetCommandPos(pDoc->m_structHeadConfig[iheadindex].nD[0], pDoc->m_structHeadConfig[iheadindex].nD[1], 0);
			}
		}
	}
	OnButtonSet(true);
	m_ctrlGateHome.SetBackColor(LIGHTGRAY);
	m_ctrlGateOpen.SetBackColor(LIGHTGREEN);// by ckh 2009.03.20
	m_bMoveFlag = FALSE;
	pFrame->DoEvents();		
}

void CAdjustInitial::OnClickCmdGateOpen() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	int i = 0;
	m_bMoveFlag = TRUE;
	OnButtonSet(false);
	m_ctrlGateOpen.SetBackColor(LIGHTGREEN);
	pFrame->DoEvents();

	//by shin//2009.08.25//MC °ü·Ã TAS Ãß°¡...//
	//µ¿ÀÛ bit off
	pView->WriteTasMCData(TAS_MC, 1, BIT_OFF);
	Sleep(10);
	pView->WriteTasMCData(TAS_MC, 4, BIT_OFF);
	Sleep(10);
	pView->WriteTasMCData(TAS_MC, 7, BIT_OFF);
	Sleep(10);
	pView->WriteTasMCData(TAS_MC, 8, BIT_OFF);
	Sleep(10);

	//Gate Close Check//
	for(i=0;i<MAX_NOZZLE;i++)
	{
		if(Drop_Info.manu_head_job[i])
		{
			if(pView->GateCloseCheck(i) != i)
			{	
				pView->PostMessage(WM_ERROR,142,i+1);
				OnButtonSet(true);
				return;
			}
		}
	}

	WORD wNoOfAxisG1 = 0; //G1~G12
	WORD wNoOfAxisG2 = 0; //G13~G16

	int iAxisG1[MAX_NOZZLE] = {0,}; //G motor	     
	int iAxisG2[MAX_NOZZLE] = {0,}; //G motor	     

	double dPos1G1[MAX_NOZZLE] = {0,};
	double dPos1G2[MAX_NOZZLE] = {0,};

	double dVelocity1G1[MAX_NOZZLE] = {0,};
	double dVelocity1G2[MAX_NOZZLE] = {0,};

	for(i=0;i<MAX_NOZZLE;i++)
	{
		if(Drop_Info.manu_head_job[i])
		{
			if(i < NOZZLE_G1) 
			{
				iAxisG1[wNoOfAxisG1]=(AXIS_G1+i)%BOARD_AXES;
				dPos1G1[wNoOfAxisG1] = (G_PULSE/2 + Drop_Info.m_nGateOffset[i]);
				dVelocity1G1[wNoOfAxisG1] = pDoc->m_structAdjustCondition.dSpeed*G_PULSE/60;
				wNoOfAxisG1++;
			}
			else
			{
				iAxisG2[wNoOfAxisG2]=(AXIS_G13+(i%NOZZLE_G1))%BOARD_AXES;
				dPos1G2[wNoOfAxisG2] = (G_PULSE/2 + Drop_Info.m_nGateOffset[i]);
				dVelocity1G2[wNoOfAxisG2] = pDoc->m_structAdjustCondition.dSpeed*G_PULSE/60;
				wNoOfAxisG2++;
			}
		}
	}

	//Gate Open//
//	FAS_MoveMultiAxisIncPos(AXIS_G1/BOARD_AXES+1, wNoOfAxisG1, iAxisG1, dPos1G1, dVelocity1G1, TRUE);												
//	FAS_MoveMultiAxisIncPos(AXIS_G13/BOARD_AXES+1, wNoOfAxisG2, iAxisG2, dPos1G2, dVelocity1G2, TRUE);	
	FAS_MoveMultiAxisIncPos(AXIS_G1/BOARD_AXES+1, wNoOfAxisG1, iAxisG1, dPos1G1, dVelocity1G1, FALSE);												
 	FAS_MoveMultiAxisIncPos(AXIS_G13/BOARD_AXES+1, wNoOfAxisG2, iAxisG2, dPos1G2, dVelocity1G2, TRUE);	
	Sleep(100);

	//Gate Open Check//
	for(i=0;i<MAX_NOZZLE;i++)
	{
		if(Drop_Info.manu_head_job[i])
		{
			if(pView->GateOpenCheck(i) != i)
			{	
				pView->PostMessage(WM_ERROR,142,i+1);
				m_bMoveFlag = FALSE;
				OnButtonSet(true);
				return;
			}
		}
	}

	m_bMoveFlag = FALSE;
	OnButtonSet(true);

	m_ctrlGateOpen.SetBackColor(LIGHTGRAY);

	pFrame->DoEvents();			
}

void CAdjustInitial::OnClickCmdPumpHome() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();


	return;
	
////////////////////////////////////////
//Pump ÆÄ¼ÕÀÇ ¿ì·Á·Î ÀÌ µ¿ÀÛÀº »ç¿ë ¾ÈÇÑ´Ù...//

//////////////////////////////////////////
	int k = 0;
	long AxisStatus;
	m_bMoveFlag = TRUE;
	OnButtonSet(false);
	m_ctrlPumpHome.SetBackColor(LIGHTGREEN);
	double dActuralPos = 0.0;

	pFrame->DoEvents();

	if(PC_TYPE == TRUE)
	{
		//Gate Open Check//
		for(int iheadindex = 0; iheadindex < MAX_NOZZLE; iheadindex++)
		{
			if(Drop_Info.manu_head_job[iheadindex])
			{			
				if(pView->GateOpenCheck(iheadindex) != iheadindex)
				{	
					GateHomeFlag[iheadindex] = FALSE;					
				}
			}
		}
		for(iheadindex = 0; iheadindex < MAX_NOZZLE; iheadindex++)
		{
			//Gate Home//
			if(GateHomeFlag[iheadindex] == FALSE)
			{
				pFrame->DoEvents();
				FAS_MoveOriginSingleAxis(pDoc->m_structHeadConfig[iheadindex].nD[0], pDoc->m_structHeadConfig[iheadindex].nD[1], 0);  	
			}
		}
		while(1)
		{
			k = 0;
			for(iheadindex = 0 ; iheadindex < MAX_NOZZLE ;iheadindex++)
			{
				if(GateHomeFlag[iheadindex] == FALSE)
				{
					FAS_GetAxisStatus(pDoc->m_structHeadConfig[iheadindex].nD[0], pDoc->m_structHeadConfig[iheadindex].nD[1], &AxisStatus);  	
					if((AxisStatus & MOTIONING)== FALSE) k++;
				}
				else
					k++;
			}
			if(k >= MAX_NOZZLE) break;
			Sleep(100);
			if(pView->m_pDevice->SST_Check(STOP_SWITCH))
			{
				for(iheadindex = 0 ; iheadindex < MAX_NOZZLE ;iheadindex++)
					FAS_MoveStop(pDoc->m_structHeadConfig[iheadindex].nD[0], pDoc->m_structHeadConfig[iheadindex].nD[1],1);
				break;
			}
			pFrame->DoEvents();
		}
		for(iheadindex = 0; iheadindex < MAX_NOZZLE; iheadindex++)
		{
			if(GateHomeFlag[iheadindex] == FALSE)
			{
				if(pDoc->m_structHeadConfig[iheadindex].nD[0]!=0)
				{
					FAS_MoveSingleAxisIncPos(pDoc->m_structHeadConfig[iheadindex].nD[0],pDoc->m_structHeadConfig[iheadindex].nD[1],
												GATE_ORIGIN_OFFSET,pDoc->m_structAdjustCondition.dSpeed*G_PULSE/60,1);
					Sleep(50);

					FAS_SetCommandPos(pDoc->m_structHeadConfig[iheadindex].nD[0], pDoc->m_structHeadConfig[iheadindex].nD[1], 0);
				}
				GateHomeFlag[iheadindex] = TRUE;
			}
		}
		Sleep(50);

		//Gate Open Check//
		for(iheadindex = 0; iheadindex < MAX_NOZZLE; iheadindex++)
		{
			if(Drop_Info.manu_head_job[iheadindex])
			{
				if(pView->GateOpenCheck(iheadindex) != iheadindex)
				{	
					pView->PostMessage(WM_ERROR,142,iheadindex+1);
					OnButtonSet(true);
					return;
				}
			}
		}	
		
		//P Motor Home//
		for(iheadindex = 0 ; iheadindex < MAX_NOZZLE ;iheadindex++)
		{
			if(Drop_Info.manu_head_job[iheadindex])
			{
				pFrame->DoEvents();
				FAS_MoveOriginSingleAxis(pDoc->m_structHeadConfig[iheadindex].nA[0], pDoc->m_structHeadConfig[iheadindex].nA[1], 0);  	
			}
		}
		while(1)
		{
			k = 0;
			for(iheadindex = 0 ; iheadindex < MAX_NOZZLE ;iheadindex++)
			{
				if(Drop_Info.manu_head_job[iheadindex])
				{
					FAS_GetAxisStatus(pDoc->m_structHeadConfig[iheadindex].nA[0], pDoc->m_structHeadConfig[iheadindex].nA[1], &AxisStatus);  	
					if((AxisStatus & MOTIONING)== FALSE) k++;
				}
				else
					k++;
			}
			if(k >= MAX_NOZZLE) break;
			Sleep(100);
			if(pView->m_pDevice->SST_Check(STOP_SWITCH))
			{
				for(iheadindex = 0 ; iheadindex < MAX_NOZZLE ;iheadindex++)
					FAS_MoveStop(pDoc->m_structHeadConfig[iheadindex].nA[0], pDoc->m_structHeadConfig[iheadindex].nA[1],1);
				break;
			}
			pFrame->DoEvents();
		}
		Sleep(50);
		for(iheadindex = 0; iheadindex < MAX_NOZZLE; iheadindex++)
		{
			if(Drop_Info.manu_head_job[iheadindex])
			{
				FAS_SetCommandPos(pDoc->m_structHeadConfig[iheadindex].nA[0], pDoc->m_structHeadConfig[iheadindex].nA[1], 0.0);
				FAS_SetActualPos(pDoc->m_structHeadConfig[iheadindex].nA[0], pDoc->m_structHeadConfig[iheadindex].nA[1], 0.0);
			}
		}

		//P_Axis Base Position Move//
		WORD wNoOfAxisP1 = 0; //P1~P14
		WORD wNoOfAxisP2 = 0; //P15~P16

		int iAxisP1[MAX_NOZZLE] = {0,}; //P motor
		int iAxisP2[MAX_NOZZLE] = {0,}; //P motor

		double dPos1P1[MAX_NOZZLE] = {0,};
		double dPos1P2[MAX_NOZZLE] = {0,};

		double dVelocity1P1[MAX_NOZZLE] = {0,};
		double dVelocity1P2[MAX_NOZZLE] = {0,};

		wNoOfAxisP1 = 0;
		wNoOfAxisP2 = 0;

		for(iheadindex=0;iheadindex<MAX_NOZZLE;iheadindex++)
		{
			if(Drop_Info.manu_head_job[iheadindex])
			{
				if(iheadindex < NOZZLE_P1) 
				{
					iAxisP1[wNoOfAxisP1]=(AXIS_P1+iheadindex)%BOARD_AXES;
					dPos1P1[wNoOfAxisP1]=Drop_Info.m_dBasePos[iheadindex];
					dVelocity1P1[wNoOfAxisP1]=40000.0;
					wNoOfAxisP1++;
				}
				else
				{
					iAxisP2[wNoOfAxisP2]=(AXIS_P15+(iheadindex%NOZZLE_P1))%BOARD_AXES;
					dPos1P2[wNoOfAxisP2]=Drop_Info.m_dBasePos[iheadindex];
					dVelocity1P2[wNoOfAxisP2]=40000.0;
					wNoOfAxisP2++;
				}
			}
		}

//		FAS_MoveMultiAxisAbsPos(AXIS_P1/BOARD_AXES+1, wNoOfAxisP1, iAxisP1, dPos1P1, dVelocity1P1, TRUE);
//		FAS_MoveMultiAxisAbsPos(AXIS_P15/BOARD_AXES+1, wNoOfAxisP2, iAxisP2, dPos1P2, dVelocity1P2, TRUE);
		FAS_MoveMultiAxisAbsPos(AXIS_P1/BOARD_AXES+1, wNoOfAxisP1, iAxisP1, dPos1P1, dVelocity1P1, FALSE);
		FAS_MoveMultiAxisAbsPos(AXIS_P15/BOARD_AXES+1, wNoOfAxisP2, iAxisP2, dPos1P2, dVelocity1P2, TRUE);

//		Sleep(100);
		Sleep(INPOSITION_DELAY);
		for(iheadindex=0; iheadindex<MAX_NOZZLE; iheadindex++)
		{
			if(Drop_Info.manu_head_job[iheadindex])
			{
				if(iheadindex < NOZZLE_P1)							
					while(!pView->m_pDevice->MotionEndCheck(AXIS_P1+iheadindex));
				else
					while(!pView->m_pDevice->MotionEndCheck(AXIS_P1+iheadindex+2));
			}
		}
		
		//base position compare//
		for(iheadindex = 0; iheadindex < MAX_NOZZLE; iheadindex++)
		{
			if(Drop_Info.manu_head_job[iheadindex])
			{
				FAS_GetActualPos(pDoc->m_structHeadConfig[iheadindex].nA[0], pDoc->m_structHeadConfig[iheadindex].nA[1], &dActuralPos);
				if(fabs(Drop_Info.m_dBasePos[iheadindex]-dActuralPos)>ERR_PULSE)
				{
					Sleep(P_DELAY);
					FAS_GetActualPos(pDoc->m_structHeadConfig[iheadindex].nA[0], pDoc->m_structHeadConfig[iheadindex].nA[1], &dActuralPos);
					if(fabs(Drop_Info.m_dBasePos[iheadindex]-dActuralPos)>ERR_PULSE)
					{
						m_strMotionLog[iheadindex].Format("<M> Head %d P-Axis BasePos %f Current Pos %f",iheadindex+1,Drop_Info.m_dBasePos[iheadindex],dActuralPos);
						pView->SaveLog(0,m_strMotionLog[iheadindex]);
						pView->PostMessage(WM_ERROR,130,iheadindex+1);	
						OnButtonSet(true);
						return;	
					}
				}
			}
		}

		//Gate Open Check//
		for(iheadindex = 0; iheadindex < MAX_NOZZLE; iheadindex++)
		{
			if(Drop_Info.manu_head_job[iheadindex])
			{
				if(pView->GateOpenCheck(iheadindex) != iheadindex)
				{	
					pView->PostMessage(WM_ERROR,142,iheadindex+1);
					m_bMoveFlag = FALSE;
					OnButtonSet(true);
					return;
				}
			}
		}		
	}
	m_ctrlPumpHome.SetBackColor(LIGHTGRAY);
	m_bMoveFlag = FALSE;
	pFrame->DoEvents();				
}

void CAdjustInitial::OnClickCmdSetting() 
{
	// TODO: Add your control notification handler code here

/*	if(!pSettingAssembleDlg) delete pSettingAssembleDlg;

	pSettingAssembleDlg = new CSettingAssemble;
	pSettingAssembleDlg->Create(IDD_SETTING, this);

	pSettingAssembleDlg->SetWindowPos(NULL,100,100,
						0,0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER );	*/
	CSettingAssemble dlg;
	dlg.DoModal();

}

void CAdjustInitial::OnClickCmdSpdSave() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	CNormalMsg dlg;
	dlg.m_bTimer=FALSE;

	if (pView->m_nLanguage == 0)
	{
		dlg.m_strTitle = _T("È®ÀÎ");
		dlg.m_strMsg1 = " Pump µ¥ÀÌÅÍ¸¦ ¼öÁ¤ÇÏ½Ã°Ú½À´Ï±î? ";
		dlg.m_strMsg2 = " ¼öÁ¤ÇÏ·Á¸é 'OK', ±×·¸Áö ¾ÊÀ¸¸é 'CANCEL' ¹öÆ°À» ´©¸£¼¼¿ä...";
	}

	else if (pView->m_nLanguage == 1)
	{
		dlg.m_strTitle = _T("Check");
		dlg.m_strMsg1 = " Will you revise Pump data? ";
		dlg.m_strMsg2 = " if you revise 'OK', or Not revise Please Push Button 'CANCEL'......";
	}
	else if(pView->m_nLanguage == 2)	//Áß±¹¾î
	{
		dlg.m_strTitle = _T("ü¬ìã");
		dlg.m_strMsg1 = "  é©áóËÇPumpâ¦ËàØ§? ";
		dlg.m_strMsg2 = " åýâÍáóËÇïÇÌª 'OK', ÜúöÎïÇÌªCANCEL...";
	}

	if( dlg.DoModal() == IDOK )
	{
		for(int i = 0; i < MAX_NOZZLE/2; i++)
		{
			if(Drop_Info.manu_head_job[i])
			{			
				Drop_Info.m_dAdjustAbsPos[i] = (atof((char *)(LPCSTR)m_ctrlAdjustInitialSet.GetTextMatrix(2, MAX_NOZZLE/2-i)))/VOLUME_RESOLUTION;
				Drop_Info.m_dDistPerDrop[i] = Drop_Info.m_dAdjustAbsPos[i]/pDoc->m_structAdjustCondition.nDropCount[i];
				Drop_Info.m_dDistPerDrop[i]= (double)pView->SubRoundAdjust(Drop_Info.m_dDistPerDrop[i]);
				Drop_Info.m_nGateOffset[i] = (atoi((char *)(LPCSTR)m_ctrlAdjustInitialSet.GetTextMatrix(3, MAX_NOZZLE/2-i)));

//				Drop_Info.m_bDegasNtypeStatus[i] = (atoi((char *)(LPCSTR)m_ctrlAdjustInitialSet.GetTextMatrix(4, MAX_NOZZLE/2-i)));
			}
		}
		for(i = 0; i < MAX_NOZZLE/2; i++)
		{
			if(Drop_Info.manu_head_job[i+MAX_NOZZLE/2])
			{			
				Drop_Info.m_dAdjustAbsPos[i+MAX_NOZZLE/2] = (atof((char *)(LPCSTR)m_ctrlAdjustInitialSet.GetTextMatrix(7, MAX_NOZZLE/2-i)))/VOLUME_RESOLUTION;
				Drop_Info.m_dDistPerDrop[i+MAX_NOZZLE/2] = Drop_Info.m_dAdjustAbsPos[i+MAX_NOZZLE/2]/pDoc->m_structAdjustCondition.nDropCount[i+MAX_NOZZLE/2];
				Drop_Info.m_dDistPerDrop[i+MAX_NOZZLE/2]= (double)pView->SubRoundAdjust(Drop_Info.m_dDistPerDrop[i+MAX_NOZZLE/2]);
				Drop_Info.m_nGateOffset[i+MAX_NOZZLE/2] = (atoi((char *)(LPCSTR)m_ctrlAdjustInitialSet.GetTextMatrix(8, MAX_NOZZLE/2-i)));

//				Drop_Info.m_bDegasNtypeStatus[i+MAX_NOZZLE/2] = (atoi((char *)(LPCSTR)m_ctrlAdjustInitialSet.GetTextMatrix(9, MAX_NOZZLE/2-i)));
			}
		}
		CString strLog;
		for(i=0; i<MAX_NOZZLE; i++)		//jykim 170517 CmdSpdSave Å¬¸¯ ½Ã BasePos °ª ·Î±× Ãß°¡
		{
			strLog.Format("OnClickCmdSpdSave - m_dBasePos[%d] = %.0f", i, Drop_Info.m_dBasePos[i]);
			pView->SaveLog(0,strLog);
		}								//jykim 170517 CmdSpdSave Å¬¸¯ ½Ã BasePos °ª ·Î±× Ãß°¡
		pDoc->SaveSPDData();//by shin//2006/09/11
	}
	else
	{
		return;
	}			
}

void CAdjustInitial::OnButtonSet(BOOL i)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(i == FALSE)
	{
		m_ctrlReturn.EnableWindow(false);
		m_ctrlDisjoint.EnableWindow(false);
		m_ctrlJoint1.EnableWindow(false);
		m_ctrlJoint2.EnableWindow(false);
		m_ctrlBubbleRemoval.EnableWindow(false);
		m_ctrlLcSupply.EnableWindow(false);
		m_ctrlPumpTeach.EnableWindow(false);
		m_ctrlPumpDataSave.EnableWindow(false);
		m_ctrlGateHome.EnableWindow(false);
		m_ctrlPumpHome.EnableWindow(false);
		m_ctrlGateClose.EnableWindow(false);
		m_ctrlGateOpen.EnableWindow(false);
		m_ctrlDegasTimes1.EnableWindow(false);
		m_ctrlDegasTimes2.EnableWindow(false);
		m_ctrlDegasSucPos.EnableWindow(false);
		m_ctrlModeSel.EnableWindow(false);
		m_ctrlPistonClear.EnableWindow(false);
		m_ctrlAdjustInitialSet.EnableWindow(false);

		//by shin//2013.01.15//n-type add//
		m_ctrlNDegas.EnableWindow(false);
		m_ctrlNDegasSucPos.EnableWindow(false);
		m_ctrlNDischargeSpeed1.EnableWindow(false);
		m_ctrlNDischargeSpeed2.EnableWindow(false);
		m_ctrlNOverTime.EnableWindow(false);
		m_ctrlNDegasTimes.EnableWindow(false);
		m_ctrlNAgingTimes.EnableWindow(false);
		m_ctrlNAging.EnableWindow(false);
	}
	else
	{
		m_ctrlReturn.EnableWindow(true);
		m_ctrlDisjoint.EnableWindow(true);
		m_ctrlJoint1.EnableWindow(true);
		m_ctrlJoint2.EnableWindow(true);
		m_ctrlBubbleRemoval.EnableWindow(true);
		m_ctrlLcSupply.EnableWindow(true);
		m_ctrlPumpTeach.EnableWindow(true);
		m_ctrlPumpDataSave.EnableWindow(true);
		m_ctrlGateHome.EnableWindow(true);
		m_ctrlPumpHome.EnableWindow(true);
		m_ctrlGateClose.EnableWindow(true);
		m_ctrlGateOpen.EnableWindow(true);
		m_ctrlDegasTimes1.EnableWindow(true);
		m_ctrlDegasTimes2.EnableWindow(true);
		m_ctrlDegasSucPos.EnableWindow(true);
		m_ctrlModeSel.EnableWindow(true);
		m_ctrlPistonClear.EnableWindow(true);
		m_ctrlAdjustInitialSet.EnableWindow(true);
		//by shin//2013.01.15//n-type add//
		if(pDoc->m_structDataEditor.m_nNzlMode == 1) //n-type ±Ø¹Ì¼¼ ³ëÁñÀÎ °æ¿ì...//
		{
			m_ctrlNDegas.EnableWindow(true);
			m_ctrlNDegasTimes.EnableWindow(true);
			m_ctrlBubbleRemoval.EnableWindow(false);
			m_ctrlDegasTimes2.EnableWindow(false);
			m_ctrlNAgingTimes.EnableWindow(true);
			m_ctrlNAging.EnableWindow(true);
		}
		else
		{
			m_ctrlNDegas.EnableWindow(false);
			m_ctrlNDegasTimes.EnableWindow(false);
			m_ctrlBubbleRemoval.EnableWindow(true);
			m_ctrlDegasTimes2.EnableWindow(true);
			m_ctrlNAgingTimes.EnableWindow(false);
			m_ctrlNAging.EnableWindow(false);
		}
		if(pDoc->m_bIsEngineerMode)
		{
			m_ctrlPistonClear.EnableWindow(true);
			m_ctrlDegasSucPos.EnableWindow(true);
			m_ctrlSuctionSpeed.EnableWindow(true);
			m_ctrlDischargeSpeed.EnableWindow(true);
			m_ctrlPumpTeach.EnableWindow(true);
			//by shin//2013.01.15//n-type add//
			if(pDoc->m_structDataEditor.m_nNzlMode == 1) //n-type ±Ø¹Ì¼¼ ³ëÁñÀÎ °æ¿ì...//
			{
				m_ctrlNDegasSucPos.EnableWindow(true);
				m_ctrlNDischargeSpeed1.EnableWindow(true);
				m_ctrlNDischargeSpeed2.EnableWindow(true);
				m_ctrlNOverTime.EnableWindow(true);
				m_ctrlNAgingTimes.EnableWindow(true);
				m_ctrlNAging.EnableWindow(true);
			}
		}
	}
}

void CAdjustInitial::SPDDataDisplay()
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	CString val;

	val.Format("%d",Drop_Info.m_nRemoveBubbleTimes1[0]);
	m_ctrlDegasTimes1.SetCaption(val);
	nRemovalTimes1 = Drop_Info.m_nRemoveBubbleTimes1[0];

	val.Format("%d",Drop_Info.m_nRemoveBubbleTimes2[0]);
	m_ctrlDegasTimes2.SetCaption(val);
	nRemovalTimes2 = Drop_Info.m_nRemoveBubbleTimes2[0];

	val.Format("%.3f",Drop_Info.m_dRemoveBubblePos[0]*VOLUME_RESOLUTION);
	m_ctrlDegasSucPos.SetCaption(val);
	dRemovalDistance = Drop_Info.m_dRemoveBubblePos[0]*VOLUME_RESOLUTION;

	val.Format("%d",Drop_Info.m_nDischargeSpeed[0]/1000);
	m_ctrlDischargeSpeed.SetCaption(val);
	nDischargeSpeed = Drop_Info.m_nDischargeSpeed[0]/1000;

	val.Format("%d",Drop_Info.m_nSuctionSpeed[0]/1000);
	m_ctrlSuctionSpeed.SetCaption(val);
	nSuctionSpeed = Drop_Info.m_nSuctionSpeed[0]/1000;

	val.Format("%d",pDoc->m_nInitSupplyTime);
	m_ctrlInitSupplyTime.SetCaption(val);
	nInitSupplyTime = pDoc->m_nInitSupplyTime;

	val.Format("%d",pDoc->m_nNormalSupplyTime);
	m_ctrlNormalSupplyTime.SetCaption(val);
	nNormalSupplyTime = pDoc->m_nNormalSupplyTime;

	val.Format("%.2f",pDoc->m_dManuCanAlarmWeight);
	m_ctrlManuCanWeight.SetCaption(val);
	dManuCanWeight = pDoc->m_dManuCanAlarmWeight;

	//by shin//2013.01.15//n-type add//
	val.Format("%.3f",Drop_Info.m_dNRemoveBubblePos[0]*VOLUME_RESOLUTION);
	m_ctrlNDegasSucPos.SetCaption(val);
	dNRemovalDistance = Drop_Info.m_dNRemoveBubblePos[0]*VOLUME_RESOLUTION;
	
	val.Format("%.0f",Drop_Info.m_dNDischargeSpeed1[0]/1000);
	m_ctrlNDischargeSpeed1.SetCaption(val);
	dNDischargeSpeed1 = Drop_Info.m_dNDischargeSpeed1[0]/1000;
	
	val.Format("%.0f",Drop_Info.m_dNDischargeSpeed2[0]/1000);
	m_ctrlNDischargeSpeed2.SetCaption(val);
	dNDischargeSpeed2 = Drop_Info.m_dNDischargeSpeed2[0]/1000;
	
	val.Format("%d",Drop_Info.m_nNRemoveBubbleTimes[0]);
	m_ctrlNDegasTimes.SetCaption(val);
	nNRemovalTimes = Drop_Info.m_nNRemoveBubbleTimes[0];
	
	val.Format("%d",Drop_Info.m_nNOverTime[0]);
	m_ctrlNOverTime.SetCaption(val);
	nNOverTime = Drop_Info.m_nNOverTime[0];

	val.Format("%d",Drop_Info.m_nNAgingCount);
	m_ctrlNAgingTimes.SetCaption(val);
	nNAgingTimes = Drop_Info.m_nNAgingCount;

	//20210224 him n-aging cycle Ãß°¡
	val.Format("%d", Drop_Info.m_nNAgingCycle); 
	m_ctrlNAgingCycle.SetCaption(val);
	nNAgingCycle = Drop_Info.m_nNAgingCycle;	
}

void CAdjustInitial::OnClickCmdLcInitSupplyJob() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	
	CNormalMsg dlg;

	dlg.m_bTimer=FALSE;

	if(pView->m_nLanguage == 0)
	{
		dlg.m_strTitle = _T("È®ÀÎ");
		dlg.m_strMsg1 = " 'ÃÊ±â ¾×Á¤ °ø±Þ µ¿ÀÛÀ» ½ÇÇàÇÕ´Ï´Ù. Bottle ¹× °ø±Þ ÀåÄ¡ Setting »óÅÂ¸¦ È®ÀÎ ÇÏ¼Ì½À´Ï±î?";
		dlg.m_strMsg2 = " ½ÇÇàÇÏ·Á¸é 'OK', ±×·¸Áö ¾ÊÀ¸¸é 'CANCEL' ¹öÆ°À» ´©¸£¼¼¿ä...";
	}	

	else if(pView->m_nLanguage == 1)  //ehji
	{
		dlg.m_strTitle = _T("Check");
		dlg.m_strMsg1 = " 'Macine Execute Initial LC Supply Action. Will you Check state of Bottle & supply equipment Bottle?";
		dlg.m_strMsg2 = " if you Action 'OK', or Not Action Please Push Button 'CANCEL'...";
	}	
	else if(pView->m_nLanguage == 2)
	{
		dlg.m_strTitle = _T("ü¬ìã");
		dlg.m_strMsg1 = " 'ãùú¼ôøÑ¢äûïÜÍêÐåÔÑíÂ. ü¬ìãBottleÐàÍêÐåíûöÇîÜSettingßÒ÷¾ÖõØ§?";
		dlg.m_strMsg2 = " åýâÍòûú¼ïÇÌª 'OK', ÜúöÎïÇÌªCANCEL...";
	}	

	if( dlg.DoModal() == IDOK )
	{
		ThreadStage.AdjustInitCode='H';
		SubColorControl();
		m_ctrlInitSupplyJob.SetBackColor(GREEN);	
		
		//Áß¾Ó °ø±Þ ¾Ð·ÂÀ» ÃÊ±â °ø±Þ »óÅÂ·Î º¯°æ...//0.05mpa//
		FAS_SetIoBit(3,false,SUPPLY_CHANGE_SOL,false);//2010.02.23 by tskim false->true
		Sleep(10);

		//by shin//2009.08.25//MC °ü·Ã TAS Ãß°¡...//
		//µ¿ÀÛ bit off
		pView->WriteTasMCData(TAS_MC, 1, BIT_OFF);
		Sleep(10);
		pView->WriteTasMCData(TAS_MC, 4, BIT_OFF);
		Sleep(10);
		pView->WriteTasMCData(TAS_MC, 7, BIT_OFF);
		Sleep(10);
		pView->WriteTasMCData(TAS_MC, 8, BIT_OFF);
		Sleep(10);
	}
	else
	{
		return;
	}		
}

void CAdjustInitial::OnClickCmdLcNormalSupplyJob2() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	
	CNormalMsg dlg;

	dlg.m_bTimer=FALSE;

	if(pView->m_nLanguage == 0)   
	{
		dlg.m_strTitle = _T("È®ÀÎ");
		dlg.m_strMsg1 = " '¾×Á¤ °ø±Þ µ¿ÀÛÀ» ½ÇÇàÇÕ´Ï´Ù. Bottle ¹× °ø±Þ ÀåÄ¡ Setting »óÅÂ¸¦ È®ÀÎ ÇÏ¼Ì½À´Ï±î?";
		dlg.m_strMsg2 = " ½ÇÇàÇÏ·Á¸é 'OK', ±×·¸Áö ¾ÊÀ¸¸é 'CANCEL' ¹öÆ°À» ´©¸£¼¼¿ä...";
	}

	else if(pView->m_nLanguage == 1)  
	{
		dlg.m_strTitle = _T("Check");
		dlg.m_strMsg1 = " 'Macine Execute LC Supply Action. Will you Check Setting of Bottle & supply equipment?";
		dlg.m_strMsg2 = " if you Action 'OK', or Not Action Please Push Button 'CANCEL'...";
	}
	
	else if(pView->m_nLanguage == 2)  
	{
		dlg.m_strTitle = _T("ü¬ìã");
		dlg.m_strMsg1 = " 'ãùú¼äûïÜÍêÐåÔÑíÂ. ü¬ìãBottleÐàÍêÐåíûöÇîÜSettingßÒ÷¾ÖõØ§?";
		dlg.m_strMsg2 = "åýâÍòûú¼ïÇÌª 'OK', ÜúöÎïÇÌªCANCEL...";
	}

	if( dlg.DoModal() == IDOK )
	{
		ThreadStage.AdjustInitCode='I';
		SubColorControl();
		m_ctrlNormalSupplyJob.SetBackColor(GREEN);
	
		//Áß¾Ó °ø±Þ ¾Ð·ÂÀ» Normal °ø±Þ »óÅÂ·Î º¯°æ...//0.2mpa//
		FAS_SetIoBit(3,false,SUPPLY_CHANGE_SOL,true);//2010.02.23 by tskim true->false
		Sleep(10);

		//by shin//2009.08.25//MC °ü·Ã TAS Ãß°¡...//
		//µ¿ÀÛ bit off
		pView->WriteTasMCData(TAS_MC, 1, BIT_OFF);
		Sleep(10);
		pView->WriteTasMCData(TAS_MC, 4, BIT_OFF);
		Sleep(10);
		pView->WriteTasMCData(TAS_MC, 7, BIT_OFF);
		Sleep(10);
		pView->WriteTasMCData(TAS_MC, 8, BIT_OFF);
		Sleep(10);
	}
	else
	{
		return;
	}				
}

void CAdjustInitial::OnClickInitSupplyTime() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	
	Use_TK(m_ctrlInitSupplyTime,1000,0,0,0);

	nInitSupplyTime = atoi(m_ctrlInitSupplyTime.GetCaption());
	pDoc->m_nInitSupplyTime = nInitSupplyTime;

	pDoc->SaveSupplyTimeData();		
}

void CAdjustInitial::OnClickNormalSupplyTime() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	
	Use_TK(m_ctrlNormalSupplyTime,100,0,0,0);

	nNormalSupplyTime = atoi(m_ctrlNormalSupplyTime.GetCaption());
	pDoc->m_nNormalSupplyTime = nNormalSupplyTime;

	pDoc->SaveSupplyTimeData();		
}

void CAdjustInitial::OnClickManualAlarmWeight() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	
	Use_TK(m_ctrlManuCanWeight,100,0,0,0);

	dManuCanWeight = atof(m_ctrlManuCanWeight.GetCaption());
	pDoc->m_dManuCanAlarmWeight = dManuCanWeight;

	pDoc->SaveSupplyTimeData();			
}

void CAdjustInitial::OnClickLabelRecipe() 
{
	if(!g_bRemoteControl) return;
	CTempOP dlg;
	dlg.DoModal();
// 	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
// 	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
// 	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
// 	// TODO: Add your control notification handler code here
// 	ThreadStage.AdjustInitCode='F';
// 	ThreadStage.ManualCode = 'U';
// 	AssemblePosMoveFlag = TRUE;
// 	ThreadStage.ManualJobStep = 70;
// 	ThreadStage.JobFlag = STAGE_MANUAL;
// 	pView->RunThread(THREAD_STAGE);
}

void CAdjustInitial::OnRButtonDblClk(UINT nFlags, CPoint point) 
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

void CAdjustInitial::OnClickCmdNBubbleRemoval() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	
	CString str;
	str = m_ctrlNDegasTimes.GetCaption();
//	pView->Save_Click_Log("PUMP INITIAL WINDOW : 'PUMP N_Degas Step' BUTTON, Count : "+str);
	BOOL bIoResult1=false , bIoResult2=false;
#if EQ	
	for(int i = 0; i< MAX_NOZZLE;i++)
	{
		if(pDoc->m_structHeadConfig[i].nA[0]==0) continue;
		
		if(Drop_Info.manu_head_job[i])
		{
			//Head up/down sensor check//
			FAS_GetIoBit(2, FALSE, HEAD1_UP_SENSOR+i,&bIoResult1);
			FAS_GetIoBit(3, TRUE, HEAD1_DW_SENSOR+i,&bIoResult2);
			
			if((!bIoResult1)||(bIoResult2))
			{
				pView->SendMessage(WM_ERROR,136,i+1);
				OnButtonSet(true);
				return;
			}				
		}
	}
#endif
	ThreadStage.AdjustInitCode='N'; //º¯°æ//
	SubColorControl();
	m_ctrlNDegas.SetBackColor(GREEN);	
}

void CAdjustInitial::OnClickDegasNSuctionDistance() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
/*
	Use_TK(m_ctrlDegasSucPos,10,0,0,0);

	dRemovalDistance = atof(m_ctrlDegasSucPos.GetCaption());

	for(int i=0;i<MAX_NOZZLE;i++)
	{
		Drop_Info.m_dRemoveBubblePos[i] = dRemovalDistance/VOLUME_RESOLUTION;
	}
*/
	
//	pView->Save_Click_Log("PUMP INITIAL WINDOW : 'N_Suction Distance' LABEL");
	Use_TK(m_ctrlNDegasSucPos,10,1,0,0);

  
	dNRemovalDistance = atof(m_ctrlNDegasSucPos.GetCaption());

	if(dNRemovalDistance > 10.0) dNRemovalDistance = 10.0;

	for(int j = 0; j < MAX_NOZZLE; j++)
	{
		Drop_Info.m_dNRemoveBubblePos[j] = dNRemovalDistance / VOLUME_RESOLUTION;
		Drop_Info.m_dNRemoveBubblePos[j]= (double)pView->SubRoundAdjust(Drop_Info.m_dNRemoveBubblePos[j]);
	}
	CString strLog;
	for(int i=0; i<MAX_NOZZLE; i++)		//jykim 170517 DegasNSuctionDistance º¯°æ ½Ã BasePos °ª ·Î±× Ãß°¡
	{
		strLog.Format("OnClickDegasNSuctionDistance - m_dBasePos[%d] = %.0f", i, Drop_Info.m_dBasePos[i]);
		pView->SaveLog(0,strLog);
	}								//jykim 170517 DegasNSuctionDistance º¯°æ ½Ã BasePos °ª ·Î±× Ãß°¡
	pDoc->SaveSPDData();	
}

void CAdjustInitial::OnClickDegasNDishcargeSpeed1() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
/*		
	Use_TK(m_ctrlDischargeSpeed,120,0,0,0);

	nDischargeSpeed = atoi(m_ctrlDischargeSpeed.GetCaption());
	for(int i=0;i<MAX_NOZZLE;i++)
	{
		Drop_Info.m_nDischargeSpeed[i] = nDischargeSpeed*1000;
	}
*/
	
//	pView->Save_Click_Log("PUMP INITIAL WINDOW : 'N_Discharge Speed1' LABEL");
	Use_TK(m_ctrlNDischargeSpeed1,100,50,0,0);

	dNDischargeSpeed1 = atof(m_ctrlNDischargeSpeed1.GetCaption());

	if(dNDischargeSpeed1 > 100) dNDischargeSpeed1 = 100;

	for(int j = 0; j < MAX_NOZZLE; j++)
	{
		Drop_Info.m_dNDischargeSpeed1[j] = dNDischargeSpeed1 * 1000;
	}
	CString strLog;
	for(int i=0; i<MAX_NOZZLE; i++)		//jykim 170517 DegasNDishcargeSpeed1 º¯°æ ½Ã BasePos °ª ·Î±× Ãß°¡
	{
		strLog.Format("OnClickDegasNDishcargeSpeed1 - m_dBasePos[%d] = %.0f", i, Drop_Info.m_dBasePos[i]);
		pView->SaveLog(0,strLog);
	}								//jykim 170517 DegasNDishcargeSpeed1 º¯°æ ½Ã BasePos °ª ·Î±× Ãß°¡
	pDoc->SaveSPDData();	
}

void CAdjustInitial::OnClickDegasNDishcargeSpeed2() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
/*		
	Use_TK(m_ctrlDischargeSpeed,120,0,0,0);

	nDischargeSpeed = atoi(m_ctrlDischargeSpeed.GetCaption());
	for(int i=0;i<MAX_NOZZLE;i++)
	{
		Drop_Info.m_nDischargeSpeed[i] = nDischargeSpeed*1000;
	}
*/
	
//	pView->Save_Click_Log("PUMP INITIAL WINDOW : 'N_Discharge Speed2' LABEL");
	Use_TK(m_ctrlNDischargeSpeed2,450,50,0,0);

	dNDischargeSpeed2 = atof(m_ctrlNDischargeSpeed2.GetCaption());

	if(dNDischargeSpeed2 > 450) dNDischargeSpeed2 = 450;

	for(int j = 0; j < MAX_NOZZLE; j++)
	{
		Drop_Info.m_dNDischargeSpeed2[j] = dNDischargeSpeed2 * 1000;
	}
	CString strLog;
	for(int i=0; i<MAX_NOZZLE; i++)		//jykim 170517 DegasNDishcargeSpeed2 º¯°æ ½Ã BasePos °ª ·Î±× Ãß°¡
	{
		strLog.Format("OnClickDegasNDishcargeSpeed2 - m_dBasePos[%d] = %.0f", i, Drop_Info.m_dBasePos[i]);
		pView->SaveLog(0,strLog);
	}								//jykim 170517 DegasNDishcargeSpeed2 º¯°æ ½Ã BasePos °ª ·Î±× Ãß°¡
	pDoc->SaveSPDData();	
}

void CAdjustInitial::OnClickNDegasTimes() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
/*	
	Use_TK(m_ctrlDegasTimes2,500,0,0,0);

	nRemovalTimes2 = atoi(m_ctrlDegasTimes2.GetCaption());

	for(int i=0;i<MAX_NOZZLE;i++)
	{
		Drop_Info.m_nRemoveBubbleTimes2[i] = nRemovalTimes2;
	}
*/	
	
	CString str;
	Use_TK(m_ctrlNDegasTimes,10,1,0,0);

	nNRemovalTimes = atoi(m_ctrlNDegasTimes.GetCaption());

	for(int j = 0; j < MAX_NOZZLE; j++)
	{
		Drop_Info.m_nNRemoveBubbleTimes[j] = nNRemovalTimes;
	}
	str = m_ctrlNDegasTimes.GetCaption();
//	pView->Save_Click_Log("PUMP INITIAL WINDOW : 'N_Degas Step Times' LABEL, Count :" + str);
	CString strLog;
	for(int i=0; i<MAX_NOZZLE; i++)		//jykim 170517 NDegasTime º¯°æ ½Ã BasePos °ª ·Î±× Ãß°¡
	{
		strLog.Format("OnClickNDegasTime - m_dBasePos[%d] = %.0f", i, Drop_Info.m_dBasePos[i]);
		pView->SaveLog(0,strLog);
	}								//jykim 170517 NDegasTime º¯°æ ½Ã BasePos °ª ·Î±× Ãß°¡
	pDoc->SaveSPDData();		
}

void CAdjustInitial::OnClickNOverTime() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
/*	
	Use_TK(m_ctrlDegasTimes2,500,0,0,0);

	nRemovalTimes2 = atoi(m_ctrlDegasTimes2.GetCaption());

	for(int i=0;i<MAX_NOZZLE;i++)
	{
		Drop_Info.m_nRemoveBubbleTimes2[i] = nRemovalTimes2;
	}
*/	
	
	CString str;
	Use_TK(m_ctrlNOverTime,10000,0,0,0);

	nNOverTime = atoi(m_ctrlNOverTime.GetCaption());

	for(int j = 0; j < MAX_NOZZLE; j++)
	{
		Drop_Info.m_nNOverTime[j] = nNOverTime;
	}
	str = m_ctrlNOverTime.GetCaption();
//	pView->Save_Click_Log("PUMP INITIAL WINDOW : 'N_Over Time' LABEL, Count :" + str);
	CString strLog;
	for(int i=0; i<MAX_NOZZLE; i++)		//jykim 170517 NOverTime º¯°æ ½Ã BasePos °ª ·Î±× Ãß°¡
	{
		strLog.Format("OnClickNOverTime - m_dBasePos[%d] = %.0f", i, Drop_Info.m_dBasePos[i]);
		pView->SaveLog(0,strLog);
	}								//jykim 170517 NOverTime º¯°æ ½Ã BasePos °ª ·Î±× Ãß°¡
	pDoc->SaveSPDData();	
}

void CAdjustInitial::SelectLanguage()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(pView->m_nLanguage == 0) //ehji 140309 .
	{
		//AdjustInital
		SetDlgItemText(IDC_COMMANDBUTTON6,			_T("ÆßÇÁ ÀåÂø ¹× ÇØÃ¼"));
		SetDlgItemText(IDC_CMD_MODE_SECLECT,			_T("Auto ÀåÂø"));
		SetDlgItemText(IDC_CMD_PISTON_DOWN,			_T("ÇÇ½ºÅæ Alarm Clear"));
		SetDlgItemText(IDC_CMD_PUMP_DISJOINT,			_T("ÆßÇÁ ÀåÂø1 ¹× ÇØÃ¼"));
		SetDlgItemText(IDC_CMD_PUMP_JOINT2,			_T("ÆßÇÁ ÀåÂø2"));
		SetDlgItemText(IDC_COMMANDBUTTON5,			_T("¾×Á¤ °ø±Þ ¹× ±âÆ÷ Á¦°Å"));
		SetDlgItemText(IDC_LABEL1,			_T("ÃæÁø °Å¸®(mm) :"));
		SetDlgItemText(IDC_LABEL7,			_T("ÃæÁø ¼Óµµ(Kpps) :"));
		SetDlgItemText(IDC_LABEL8,			_T("ÅäÃâ ¼Óµµ(Kpps) :"));
		SetDlgItemText(IDC_CMD_LC_SUPPLY,			_T("¾×Á¤ °ø±Þ"));
		SetDlgItemText(IDC_CMD_BUBBLE_REMOVAL1,			_T("±âÆ÷ Á¦°Å"));
		SetDlgItemText(IDC_LABEL2,			_T("¾×Á¤ °ø±Þ  È½¼ö :"));
		SetDlgItemText(IDC_LABEL13,			_T("±âÆ÷  Á¦°Å  È¸¼ö :"));
		SetDlgItemText(IDC_LABEL69,			_T("N_ÃæÁø °Å¸®(mm) :"));
		SetDlgItemText(IDC_LABEL88,			_T("N_ÅäÃâ¼Óµµ1 (mm/s) :"));
		SetDlgItemText(IDC_LABEL70,			_T("N_ÅäÃâ¼Óµµ2 (mm/s) :"));
		SetDlgItemText(IDC_CMD_N_BUBBLE_REMOVAL,			_T("N_±âÆ÷Á¦°Å"));
		SetDlgItemText(IDC_LABEL89,			_T("Á¦°Å È½¼ö"));
		SetDlgItemText(IDC_COMMANDBUTTON7,			_T("¸ðÅÍ °ü·Ã"));
		SetDlgItemText(IDC_CMD_GATE_HOME,			_T("Gate ¸ðÅÍ ¿øÁ¡"));
		SetDlgItemText(IDC_COMMANDBUTTON13,			_T("Áß¾Ó °ø±Þ"));
		SetDlgItemText(IDC_CMD_LC_INIT_SUPPLY_JOB,			_T("ÃÊ±â ¾×Á¤ °ø±Þ"));
		SetDlgItemText(IDC_CMD_LC_NORMAL_SUPPLY_JOB2,			_T("Bottle ¾×Á¤ °ø±Þ"));
		SetDlgItemText(IDC_LABEL50,			_T("Canister  °æ°í·®(Kg)"));
		SetDlgItemText(IDC_LABEL37,			_T("°ø±Þ ½Ã°£(sec)"));
		SetDlgItemText(IDC_LABEL38,			_T("°ø±Þ ½Ã°£(sec)"));
		SetDlgItemText(IDC_CMD_INTERLOCKUSAGE,			_T("INTERLOCK »ç¿ë"));
		SetDlgItemText(IDC_CMD_SETTING,			_T("ÆßÇÁ Teaching"));
		SetDlgItemText(IDC_CMD_SPD_SAVE,			_T("ÆßÇÁ µ¥ÀÌÅÍ Save"));
		SetDlgItemText(IDC_CMD_RETURN,			_T("ÀÌÀü È­¸é"));
	}

	else if(pView->m_nLanguage == 1) //ehji 140309 .
	{
		//Manu
		SetDlgItemText(IDC_COMMANDBUTTON6,			_T("Pump Joint or Disjoint"));
		SetDlgItemText(IDC_CMD_MODE_SECLECT,			_T("Auto Joint"));
		SetDlgItemText(IDC_CMD_PISTON_DOWN,			_T("Piston Alarm Clear"));
		SetDlgItemText(IDC_CMD_PUMP_DISJOINT,			_T("Pump Joint1 or Disjoint"));
		SetDlgItemText(IDC_CMD_PUMP_JOINT2,			_T("Pump Joint2"));
		SetDlgItemText(IDC_COMMANDBUTTON5,			_T("LC Supply and Bubble Removal"));
		SetDlgItemText(IDC_LABEL1,			_T("Charge Distence(mm) :"));
		SetDlgItemText(IDC_LABEL7,			_T("Charge Speed(Kpps) :"));
		SetDlgItemText(IDC_LABEL8,			_T("Dispence Speed(Kpps) :"));
		SetDlgItemText(IDC_CMD_LC_SUPPLY,			_T("LC Supply"));
		SetDlgItemText(IDC_CMD_BUBBLE_REMOVAL1,			_T("Bubble Removal"));
		SetDlgItemText(IDC_LABEL2,			_T("LC Supply Count :"));
		SetDlgItemText(IDC_LABEL13,			_T("Bubble Removal Count :"));
		SetDlgItemText(IDC_LABEL69,			_T("N_Charge Distence(mm) :"));
		SetDlgItemText(IDC_LABEL88,			_T("N_Dispence Speed1 (mm/s) :"));
		SetDlgItemText(IDC_LABEL70,			_T("N_Dispence Speed2 (mm/s) :"));
		SetDlgItemText(IDC_CMD_N_BUBBLE_REMOVAL,			_T("N_Bubble Removal"));
		SetDlgItemText(IDC_LABEL89,			_T("Removal Count"));
		SetDlgItemText(IDC_COMMANDBUTTON7,			_T("Motor Relation"));
		SetDlgItemText(IDC_CMD_GATE_HOME,			_T("Gate Motor Origin"));
		SetDlgItemText(IDC_COMMANDBUTTON13,			_T("Center Supply"));
		SetDlgItemText(IDC_CMD_LC_INIT_SUPPLY_JOB,			_T("Inital LC Supply"));
		SetDlgItemText(IDC_CMD_LC_NORMAL_SUPPLY_JOB2,			_T("Bottle LC Supply"));
		SetDlgItemText(IDC_LABEL50,			_T("Canister Warning amount(Kg)"));
		SetDlgItemText(IDC_LABEL37,			_T("Supply Time(sec)"));
		SetDlgItemText(IDC_LABEL38,			_T("Supply Time(sec)"));
		SetDlgItemText(IDC_CMD_INTERLOCKUSAGE,			_T("INTERLOCK USE"));
		SetDlgItemText(IDC_CMD_SETTING,			_T("Pump Teaching"));
		SetDlgItemText(IDC_CMD_SPD_SAVE,			_T("Pump Data Save"));
		SetDlgItemText(IDC_CMD_RETURN,			_T("Return"));
	}

	else if(pView->m_nLanguage == 2) //ehji 140309 .
	{
		//Manu
		SetDlgItemText(IDC_COMMANDBUTTON6,			_T("PumpäÌíûÐàú°ð¶"));
		SetDlgItemText(IDC_CMD_MODE_SECLECT,			_T("Auto äÌíû"));
		SetDlgItemText(IDC_CMD_PISTON_DOWN,			_T("Piston Alarm Clear"));
		SetDlgItemText(IDC_CMD_PUMP_DISJOINT,			_T("PumpäÌíû1Ðàú°ð¶"));
		SetDlgItemText(IDC_CMD_PUMP_JOINT2,			_T("PumpäÌíû2"));
		SetDlgItemText(IDC_COMMANDBUTTON5,			_T("äûïÜÍêÐåÐàËäð¶Ñ¨øÜ"));
		SetDlgItemText(IDC_LABEL1,			_T("õöòäËå×î(mm) :"));
		SetDlgItemText(IDC_LABEL7,			_T("õöòäáÜÓø(Kpps) :"));
		SetDlgItemText(IDC_LABEL8,			_T("÷ÎõóáÜÓø(Kpps) :"));
		SetDlgItemText(IDC_CMD_LC_SUPPLY,			_T("äûïÜÍêÐå"));
		SetDlgItemText(IDC_CMD_BUBBLE_REMOVAL1,			_T("Ëäð¶Ñ¨øÜ"));
		SetDlgItemText(IDC_LABEL2,			_T("äûïÜÍêÐåüÞâ¦ :"));
		SetDlgItemText(IDC_LABEL13,			_T("Ëäð¶Ñ¨øÜüÞâ¦ :"));
		SetDlgItemText(IDC_LABEL69,			_T("N_õöòäËå×î(mm) :"));
		SetDlgItemText(IDC_LABEL88,			_T("N_÷ÎõóáÜÓø1 (mm/s) :"));
		SetDlgItemText(IDC_LABEL70,			_T("N_÷ÎõóáÜÓø2 (mm/s) :"));
		SetDlgItemText(IDC_CMD_N_BUBBLE_REMOVAL,			_T("Ëäð¶Ñ¨øÜ"));
		SetDlgItemText(IDC_LABEL89,			_T("Ëäð¶üÞâ¦"));
		SetDlgItemText(IDC_COMMANDBUTTON7,			_T("ï³Ñ¦Î¼Ö¤"));
		SetDlgItemText(IDC_CMD_GATE_HOME,			_T("Gate ï³Ñ¦ê«ïÇ"));
		SetDlgItemText(IDC_COMMANDBUTTON13,			_T("ñéäçÍêÐå"));
		SetDlgItemText(IDC_CMD_LC_INIT_SUPPLY_JOB,			_T("ôøÑ¢äûïÜÍêÐå"));
		SetDlgItemText(IDC_CMD_LC_NORMAL_SUPPLY_JOB2,			_T("Bottle äûïÜÍêÐå"));
		SetDlgItemText(IDC_LABEL50,			_T("Canister  ÌíÍ±Õá(Kg)"));
		SetDlgItemText(IDC_LABEL37,			_T("ÍêÐåãÁÊà(sec)"));
		SetDlgItemText(IDC_LABEL38,			_T("ÍêÐåãÁÊà(sec)"));
		SetDlgItemText(IDC_CMD_INTERLOCKUSAGE,			_T("INTERLOCK ÞÅéÄ"));
		SetDlgItemText(IDC_CMD_SETTING,			_T("Pump Teaching"));
		SetDlgItemText(IDC_CMD_SPD_SAVE,			_T("Pump â¦Ëà Save"));
		SetDlgItemText(IDC_CMD_RETURN,			_T("Ú÷üÞ"));
	}
}

void CAdjustInitial::OnClickCmdNAging() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	
	CString str;
	str = m_ctrlNAgingTimes.GetCaption();
	BOOL bIoResult1=false , bIoResult2=false;
#if EQ	
	for(int i = 0; i< MAX_NOZZLE;i++)
	{
		if(pDoc->m_structHeadConfig[i].nA[0]==0) continue;
		
		if(Drop_Info.manu_head_job[i])
		{
			//Head up/down sensor check//
			FAS_GetIoBit(2, FALSE, HEAD1_UP_SENSOR+i,&bIoResult1);
			FAS_GetIoBit(3, TRUE, HEAD1_DW_SENSOR+i,&bIoResult2);
			
			if((!bIoResult1)||(bIoResult2))
			{
				pView->SendMessage(WM_ERROR,136,i+1);
				OnButtonSet(true);
				return;
			}				
		}
	}
#endif
	ThreadStage.AdjustInitCode='T'; 
	SubColorControl();
	m_ctrlNAging.SetBackColor(GREEN);	
}

void CAdjustInitial::OnClickNAgingTimes() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	
//	if(pDoc->m_structDataEditor.m_bN_Type == FALSE) return;
	
	int nTemp = 0;
	CString str;
	
	str.Format("%d",Drop_Info.m_nNAgingCount);
	m_ctrlNAgingTimes.SetCaption(str);
	nTemp = atoi(m_ctrlNAgingTimes.GetCaption());
	Use_TK(m_ctrlNAgingTimes,100,1,0,0);
	
	Drop_Info.m_nNAgingCount = atoi(m_ctrlNAgingTimes.GetCaption());

	CString strLog;
	for(int i=0; i<MAX_NOZZLE; i++)		//jykim 170517 NAgingTimes º¯°æ ½Ã BasePos °ª ·Î±× Ãß°¡
	{
		strLog.Format("OnClickNAgingTimes - m_dBasePos[%d] = %.0f", i, Drop_Info.m_dBasePos[i]);
		pView->SaveLog(0,strLog);
	}								//jykim 170517 NAgingTimes º¯°æ ½Ã BasePos °ª ·Î±× Ãß°¡	
	pDoc->SaveSPDData();	

	if(nTemp != Drop_Info.m_nNAgingCount)
	{
		str.Format("<Adjust Initial Window> TJD Warm up Count Change : %d -> %d", nTemp, Drop_Info.m_nNAgingCount);
		pView->SaveLog(0, str);
	}		
}


void CAdjustInitial::OnClickNAgingSupplyTime() 
{
	// TODO: Add your control notification handler code here
	//20210224 him
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	
	Use_TK(m_ctrlNAgingSupplyTime,500,1,0,0);

	nNAgingSupplyTime = atoi(m_ctrlNAgingSupplyTime.GetCaption());

	for(int j = 0; j < MAX_NOZZLE; j++)
	{
		Drop_Info.m_nRemoveBubbleTimes1[j] = nNAgingSupplyTime;
	}
	CString strLog;
	for(int i=0; i<MAX_NOZZLE; i++)		//jykim 170517 DegasTimes1 º¯°æ ½Ã BasePos °ª ·Î±× Ãß°¡
	{
		strLog.Format("OnClickNAgingSupplyTime - m_dBasePos[%d] = %.0f", i, Drop_Info.m_dBasePos[i]);
		pView->SaveLog(0,strLog);
	}								//jykim 170517 DegasTimes1 º¯°æ ½Ã BasePos °ª ·Î±× Ãß°¡
	pDoc->SaveSPDData();		
}

void CAdjustInitial::OnClickNAgingCycle() 
{
	// TODO: Add your control notification handler code here
	//20210224 him
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	int nTemp = 0;
	CString str;
	
	str.Format("%d",Drop_Info.m_nNAgingCycle);
	m_ctrlNAgingCycle.SetCaption(str);
	nTemp = atoi(m_ctrlNAgingCycle.GetCaption());
	Use_TK(m_ctrlNAgingCycle,100,1,0,0);
	
	Drop_Info.m_nNAgingCycle = atoi(m_ctrlNAgingCycle.GetCaption());

	pDoc->SaveSPDData();	

	if(nTemp != Drop_Info.m_nNAgingCycle)
	{
		str.Format("<Adjust Initial Window> TJD Warm up Cycle Change : %d -> %d", nTemp, Drop_Info.m_nNAgingCycle);
		pView->SaveLog(0, str);
	}				
}

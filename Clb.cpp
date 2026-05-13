// Clb.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"
#include "P8CA_LcDispDoc.h"
#include "Barcode.h"
#include "AdjustInitial.h"
#include "Grease.h"
#include "CounttingRange.h"
#include "HeadDropInformation.h"
#include "ModuleInfo.h"
#include "VisionDropCountSet.h"
#include "Employee_Num.h"	//ehji 141029


#include "Clb.h"
#include "NormalMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL bInitialShot;
extern BOOL g_bMotorMoveView;
extern BOOL g_bInitilDisplay;


/////////////////////////////////////////////////////////////////////////////
// CClb dialog


CClb::CClb(CWnd* pParent /*=NULL*/)
	: CDialog(CClb::IDD, pParent)
{
	//{{AFX_DATA_INIT(CClb)
	m_nCheckType = -1;
	//}}AFX_DATA_INIT
	for(int i = 0; i < MAX_NOZZLE; i++) 
//		HeadOffset[i] = 0.0;
	m_nClickCount = 0;

}

CClb::~CClb()
{
}

void CClb::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClb)
	DDX_Control(pDX, IDC_MSFLEXGRID_OFFSET, m_ctrlOffset);
	DDX_Control(pDX, IDC_MSFLEXGRID_COMMON, m_ctrlCommon);
	DDX_Control(pDX, IDC_LABEL_RECIPE_NAME, m_ctrlRecipeName);
	DDX_Control(pDX, IDC_MSFLEXGRID_LC_LOT_NUM, m_ctrlLCLotNo);
	DDX_Control(pDX, IDC_LABEL_PUMPNO, m_ctrlPumpNoLabel);
	DDX_Control(pDX, IDC_LC_COUNT_SET, m_ctrlLCDetectedCountSet);
	DDX_Radio(pDX, IDC_RADIO_Door, m_nCheckType);
	DDX_Control(pDX, IDC_MSFLEXGRID_OFFSET2, m_ctrlOffset2);
	DDX_Control(pDX, IDC_MSFLEXGRID_DROP_COUNT_ERROR_RANGE, m_crtlDropCntErrorRange);
	DDX_Control(pDX, IDC_MSFLEXGRID_PUMP_NUM, m_ctrlPumpNo);
	DDX_Control(pDX, IDC_MSFLEXGRID_LC_SUPPLY, m_ctrlLcSupplyBal);
	DDX_Control(pDX, IDC_LABEL_HEAD1, m_ctrlLcBal1);
	DDX_Control(pDX, IDC_LABEL_HEAD2, m_ctrlLcBal2);
	DDX_Control(pDX, IDC_LABEL1, m_ctrlLcLotName);
	DDX_Control(pDX, IDC_CMD_INITIAL_SHOT, m_ctrlHeadDropInf);
	DDX_Control(pDX, IDC_CMD_DROPCOUNT_REALTIME, m_ctrlRunDetectMode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CClb, CDialog)
	//{{AFX_MSG_MAP(CClb)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RADIO_Door, OnRADIODoor)
	ON_BN_CLICKED(IDC_RADIO_Remain, OnRADIORemain)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClb message handlers

BOOL CClb::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CString str = "", str1="";
	CString strTemp = "";
	int i=0,j=0,nR=0,nC=0,pat=0,k=0;	
	BOOL bIoResult=FALSE;
	GetDlgItem(IDC_LABEL_PUMPNO)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
//	m_ctrlInitialShot.MoveWindow(0,0,0,0);

	SelectLanguage(); //ehji 140606

//
	SubDisplayRecipeName();
	Doc2Val();
	m_nCheckType = 0;
	GetDlgItem(IDC_CHECKGROUP)->SetWindowText(" *  Drop Count Sensor »ç¿ë/¹Ì»ç¿ë ¼±ÅÃ  ");
	// Drop Count Sensor ¼³Á¤
	for(i=0; i<MAX_NOZZLE; i++)
	{
		m_pDropCountSensor[i] = (CButton*)GetDlgItem(IDC_CHK_DROP_H1+i);
		m_pDropCountSensor[i]->EnableWindow(FALSE);
		m_pDropCountSensor[i]->SetCheck(FALSE);
	}

	for(pat = 0 ; pat < pDoc->m_structDataEditor.m_nPatternNum ; pat++)
	{
		for(i=0; i<MAX_NOZZLE; i++)
		{		
			if(pDoc->m_structPatternData[pat].m_bIsOnHead[i]==TRUE)
			{
				m_pDropCountSensor[i]->EnableWindow(TRUE);
				if(m_bDropCount[i]==TRUE)	m_pDropCountSensor[i]->SetCheck(TRUE);
			}
		}
	}

	m_crtlDropCntErrorRange.SetRows(3);
	m_crtlDropCntErrorRange.SetCols(17);

	// Drop Count Error Range ¼³Á¤
	if(pView->m_nLanguage == 0)
	{
		m_crtlDropCntErrorRange.SetTextMatrix(1, 0 ,"»óÇÑ(+)");
		m_crtlDropCntErrorRange.SetTextMatrix(2, 0 ,"ÇÏÇÑ(-)");
	}
	else if(pView->m_nLanguage == 1)
	{
		m_crtlDropCntErrorRange.SetTextMatrix(1, 0 ,"Maximum(+)");
		m_crtlDropCntErrorRange.SetTextMatrix(2, 0 ,"Lowest(-)");
	}
	else if(pView->m_nLanguage == 2)
	{
		m_crtlDropCntErrorRange.SetTextMatrix(1, 0 ,"ß¾ùÚ(+)");
		m_crtlDropCntErrorRange.SetTextMatrix(2, 0 ,"ù»ùÚ(-)");
	}

	m_crtlDropCntErrorRange.SetColWidth(0,900);
	for(i=0;i<MAX_NOZZLE;i++)
		m_crtlDropCntErrorRange.SetColWidth(i+1,660);

	for(i=0; i<2 ;i++)
	{
		for(j =0; j < MAX_NOZZLE; j++)
		{
			pDoc->m_dDropCountErrorRange[i][j]=0;
			pDoc->m_dDropCountErrorRange_temp[i][j]=0;
		}
	}

	for(i = 0 ; i < pDoc->m_structDataEditor.m_nPatternNum ; i++)
	{
		if( pDoc->m_structPatternData[i].m_nSequence > 0 ) 
			pDoc->SetValidDropPoint_temp(i);// 0 :drop¼ö, SetValidDropPoint(int nP)  
	}

	for(j = 0; j < MAX_NOZZLE; j++)
	{
		for(k=0;k<2;k++)
		{
			pDoc->m_dDropCountErrorRange_temp[k][j] += pDoc->m_nValidDropPoint[0][j];
		}
	}

	CString strLog;
	for(i=0; i<2 ;i++)
	{
		for(j =0; j < MAX_NOZZLE; j++)
		{
			pDoc->m_dDropCountErrorRange[i][j] = (int)(pDoc->m_dDropCountErrorRange_temp[i][j] * (pDoc->m_dDropCountErrorRange_per[i][j]/100.0));
			strLog.Format("<Drop Count Range>\t%d\tÀ¯È¿%d\tÆÛ¼¾Æ®%.0f\t¹üÀ§%d"
					,i,pDoc->m_dDropCountErrorRange_temp[i][j],pDoc->m_dDropCountErrorRange_per[i][j],pDoc->m_dDropCountErrorRange[i][j]);
			pView->SaveLog(0, strLog);
		}
	}

	for(j=0;j<MAX_NOZZLE;j++)
	{
		if(pDoc->m_bIsHeadSelected[(MAX_NOZZLE-1)-j]==TRUE) 
		{
			m_crtlDropCntErrorRange.SetRow(0);
			m_crtlDropCntErrorRange.SetCol(j+1);
			m_crtlDropCntErrorRange.SetCellBackColor(GREEN);
		}
		m_crtlDropCntErrorRange.SetRowHeight(0,450);
		str.Format("H%d", MAX_NOZZLE-j);
		m_crtlDropCntErrorRange.SetTextMatrix(0,j+1,str);
		m_crtlDropCntErrorRange.SetRowHeight(1,450);
		str.Format("%4d",(int)pDoc->m_dDropCountErrorRange[0][MAX_NOZZLE-(j+1)]);	// 0: »óÇÑ, 1: ÇÏÇÑ
		m_crtlDropCntErrorRange.SetTextMatrix(1, j+1, str);
		m_crtlDropCntErrorRange.SetRowHeight(2,450);
		str.Format("%4d",(int)pDoc->m_dDropCountErrorRange[1][MAX_NOZZLE-(j+1)]);	// 0: »óÇÑ, 1: ÇÏÇÑ
		m_crtlDropCntErrorRange.SetTextMatrix(2, j+1, str);
	}

	// offset : Drop½Ã Head³ôÀÌ, bottle ³» LCÀÜ·® (g), °æ°í ÃÖ¼Ò LCÀÜ·® (g)
	for(i = 0 ; i < MAX_NOZZLE ; i++)
	{
		str.Format("  H%2d",(MAX_NOZZLE)-(i));
		m_ctrlOffset.SetTextMatrix(0, (i+1) ,str);
		m_ctrlOffset.SetColWidth(i+1, 800);
		m_ctrlOffset2.SetColWidth(i+1, 800);
		if(pDoc->m_bIsHeadSelected[(MAX_NOZZLE-1)-i]==TRUE) 
		{
			m_ctrlOffset.SetRow(0);
			m_ctrlOffset.SetCol(i+1);
			m_ctrlOffset.SetCellBackColor(GREEN);
		}
/*		if(pDoc->m_bIsEngineerMode==FALSE)
		{
			m_ctrlOffset.SetRow(1);
			m_ctrlOffset.SetCol(i+1);
			m_ctrlOffset.SetCellBackColor(WHITEGRAY);
		}
*/
	}

	if(pView->m_nLanguage == 0)
	{
		m_ctrlOffset.SetTextMatrix(1, 0, "Dummy Cup1 ¾ç(g) ");
		m_ctrlOffset.SetTextMatrix(2, 0, "Dummy Cup2 ¾ç(g) ");
		m_ctrlOffset2.SetTextMatrix(0, 0, "bottle ³» LCÀÜ·®(g)");
		m_ctrlOffset2.SetTextMatrix(1, 0, "°æ°í ÃÖ¼Ò LCÀÜ·®(g)");
		m_ctrlOffset2.SetTextMatrix(2, 0, "ÀÜ·® DetectÈÄ ÀÛ¾÷¼ö");
	}
	else if(pView->m_nLanguage == 1)
	{
		m_ctrlOffset.SetTextMatrix(1, 0, "Dummy Cup1 (g) ");
		m_ctrlOffset.SetTextMatrix(2, 0, "Dummy Cup2 (g) ");
		m_ctrlOffset2.SetTextMatrix(0, 0, "bottle in LC(g)");
		m_ctrlOffset2.SetTextMatrix(1, 0, "Alarm Min LC(g)");
		m_ctrlOffset2.SetTextMatrix(2, 0, "Amount Detect After Work");
	}
	else if(pView->m_nLanguage == 2)
	{
		m_ctrlOffset.SetTextMatrix(1, 0, "Dummy Cup1 Õá(g) ");
		m_ctrlOffset.SetTextMatrix(2, 0, "Dummy Cup2 Õá(g) ");
		m_ctrlOffset2.SetTextMatrix(0, 0, "bottleÒ®LCæ®Õá(g)");
		m_ctrlOffset2.SetTextMatrix(1, 0, "ÌíÍ±õÌá´LCæ®Õá(g)");
		m_ctrlOffset2.SetTextMatrix(2, 0, "æ®ÕáDetectý¨íÂåöâ¦");
	}

	m_ctrlOffset.SetColWidth(0, 2150);
	m_ctrlOffset2.SetColWidth(0, 2150);

	for(i=0 ; i < 3 ; i++) 
	{
		m_ctrlOffset.SetRowHeight(i,370); 
	}
	for(i=0 ; i < 3 ; i++) 
	{
		m_ctrlOffset2.SetRowHeight(i,400);
	}

	// Pump No. ¼³Á¤
	for(i = 0 ; i < (MAX_NOZZLE/2) ; i++)
	{
		str.Format("H%d / H%d",(i+1), (i+(MAX_NOZZLE/2)+1));
		m_ctrlPumpNo.SetTextMatrix(0, (MAX_NOZZLE/2-1)-i ,str);
	}
	for(i=0 ; i < (MAX_NOZZLE/2) ; i++)		
		m_ctrlPumpNo.SetColWidth(i,1625);
	for(i=0 ; i < 3 ; i++)		
		m_ctrlPumpNo.SetRowHeight(i,400);

	// LC Lot No. ¼³Á¤
	for(i = 1 ; i < (MAX_NOZZLE/2)+1 ; i++)
	{
		str.Format("H%d / H%d",i, (i+(MAX_NOZZLE/2)));
		m_ctrlLCLotNo.SetTextMatrix(0, (MAX_NOZZLE/2)-i+1 ,str);
	}
//	for(i = 1 ; i < 3 ; i++)
//	{
//		str.Format("ÇÑÁÙ ÀüÃ¼ ¼±ÅÃ");
//		m_ctrlLCLotNo.SetTextMatrix(i,0,str);
//	}
	str.Format("HEAD#8~HEAD1");
	m_ctrlLCLotNo.SetTextMatrix(1,0,str);
	str.Format("HEAD#16~HEAD9");
	m_ctrlLCLotNo.SetTextMatrix(2,0,str);

	for(i=0 ; i < (MAX_NOZZLE/2)+1 ; i++)
	{
		if(i==0) m_ctrlLCLotNo.SetColWidth(i,2150);
		else m_ctrlLCLotNo.SetColWidth(i,1600);
	}
	for(i=0 ; i < 3 ; i++)		
		m_ctrlLCLotNo.SetRowHeight(i,400);

	for(i=1 ; i < 3 ; i++)		
	{
		m_ctrlLCLotNo.SetCol(0);	
		m_ctrlLCLotNo.SetRow(i);	
		m_ctrlLCLotNo.SetCellBackColor(LIGHTGRAY);
	}

	//	common
	for(i = 0 ; i < 8 ; i++)
	{
		str.Format("B%d",(i+1));
		m_ctrlCommon.SetTextMatrix(0, (9)-(i+1) ,str);
	}
	for(i = 0 ; i < 9 ; i++)
		m_ctrlCommon.SetColAlignment(i,4);

	if(pView->m_nLanguage == 0)
	{
		m_ctrlCommon.SetTextMatrix(1, 0, "LCÇöÀç¹«°Ô(mg)");
		m_ctrlCommon.SetTextMatrix(2, 0, "LC°æ°í¹«°Ô(mg)");
	}
	else if(pView->m_nLanguage == 1)
	{
		m_ctrlCommon.SetTextMatrix(1, 0, "LC Existing (mg)");
		m_ctrlCommon.SetTextMatrix(2, 0, "LC Alarm (mg)");
	}
	else if(pView->m_nLanguage == 2)
	{
		m_ctrlCommon.SetTextMatrix(1, 0, "LCúÞî¤ñìÕá(mg)");
		m_ctrlCommon.SetTextMatrix(2, 0, "LCÌíÍ±ñìÕá(mg)");
	}

	m_ctrlCommon.SetColWidth(0, 800); 
	for(i=0 ; i < 3 ; i++) m_ctrlCommon.SetRowHeight(i,480);
	for(i=1 ; i < 9 ; i++) m_ctrlCommon.SetColWidth(i,800);

	//	Áß¾Ó °ø±Þ ¹ß¶õ½º
	for(i = 0 ; i < 4 ; i++)
	{
		if(pView->m_nLanguage == 0)
		str.Format("¹ß¶õ½º%d",i+1);

		else if(pView->m_nLanguage == 1)
		str.Format("Bal%d",i+1);

		else if(pView->m_nLanguage == 2)
		str.Format("Bal%d",i+1);
		
		m_ctrlLcSupplyBal.SetTextMatrix(0, (5)-(i+1) ,str);
	}

	if(pView->m_nLanguage == 0)
	{
		m_ctrlLcSupplyBal.SetTextMatrix(1, 0, "CANÇöÀç¹«°Ô(Kg)");
		m_ctrlLcSupplyBal.SetTextMatrix(2, 0, "CAN°æ°í¹«°Ô(Kg)");
	}
	else if(pView->m_nLanguage == 1)
	{
		m_ctrlLcSupplyBal.SetTextMatrix(1, 0, "CAN Existing(Kg)");
		m_ctrlLcSupplyBal.SetTextMatrix(2, 0, "CAN Alarm(Kg)");
	}
	else if(pView->m_nLanguage == 2)
	{
		m_ctrlLcSupplyBal.SetTextMatrix(1, 0, "CANúÞî¤ñìÕá(Kg)");
		m_ctrlLcSupplyBal.SetTextMatrix(2, 0, "CANÌíÍ±ñìÕá(Kg)");
	}

	m_ctrlLcSupplyBal.SetColWidth(0, 1000); 
	for(i=0 ; i < 3 ; i++) m_ctrlLcSupplyBal.SetRowHeight(i,500);
	for(i=1 ; i < 4+1 ; i++) m_ctrlLcSupplyBal.SetColWidth(i,900);

	///////////////////////////////////////////////////////////////////////////////////
	// data¸¦ Control¼³Á¤ // offset
	// offset[0][headº°] : Bottle ÀÜ·®
	// offset[1][headº°] : °æ°í µÇ¾îÁö´Â ÀÜ·® ¼³Á¤
//	HeadOffsetRead();
	for(i =0 ; i <m_ctrlOffset.GetRows() -1; i ++)
	{
		for (j =0 ; j <m_ctrlOffset.GetCols() -1; j++)
		{
			str.Format("%.1f", pDoc->m_dDummyCupWeight[i][j]); // Dummy Cup
			m_ctrlOffset.SetTextMatrix(i+1, (MAX_NOZZLE+1)-(j+1) ,(LPCSTR)str);
		}
	}
	for(i =0 ; i <m_ctrlOffset2.GetRows(); i ++)
	{
		for (j =0 ; j <m_ctrlOffset2.GetCols() -1; j++)
		{
			if(i<2) str.Format("%.1f", pDoc->m_dOffset[i][j]);
			else if(i==2) str.Format("%d", ThreadStage.nLCDetectedCount[j]);	// ÀÜ·® °¨Áö ÈÄ ÀÛ¾÷ÇÑ Glass ¼ö 
			m_ctrlOffset2.SetTextMatrix(i, (MAX_NOZZLE+1)-(j+1) ,(LPCSTR)str);
		}
	}
	// ÀÜ·® °¨Áö ¼¾¼­ Detect ¿©ºÎ¿¡ µû¸¥ »ö±ò Ç¥½Ã
	for(i =0 ; i<m_ctrlOffset2.GetCols() -1; i++)
	{
		m_ctrlOffset2.SetRow(2);
		m_ctrlOffset2.SetCol((MAX_NOZZLE+1)-(i+1));
		m_ctrlOffset2.SetCellBackColor(WHITE);		
		if(pDoc->m_bIsHeadSelected[i] == TRUE)
		{
			FAS_GetIoBit(1,true,LC_LIMIT1+i,&bIoResult);
			if(!bIoResult)	
				m_ctrlOffset2.SetCellBackColor(RED);				
		}
	}
	// ÀÜ·® °¨Áö ¼¾¼­¿¡ °¨ÁöµÈ ÈÄ ÀÛ¾÷ °¡´ÉÇÑ ¼ö Ç¥½Ã
	str.Format("%d",pDoc->m_dSetLCDetectedCount);
	m_ctrlLCDetectedCountSet.SetCaption(str);

	///////////////////////////////////////////////////////////////////////////////////
	// Pump No. ¼³Á¤
	for(i =0 ; i <m_ctrlPumpNo.GetCols(); i++)
	{
		m_ctrlPumpNo.SetColAlignment(i,7);	// ¿ìÃø Á¤·Ä
		m_ctrlPumpNo.SetTextMatrix(1, (m_ctrlPumpNo.GetCols()-1)-i ,(LPCSTR)pDoc->m_strPumpNo[i]);
		m_ctrlPumpNo.SetTextMatrix(2, (m_ctrlPumpNo.GetCols()-1)-i ,(LPCSTR)pDoc->m_strPumpNo[(i+MAX_NOZZLE/2)]);
	}
	RefreshData();

#if EQ
/*		for(i=1; i<=MAX_BAL; i++)
		{
			pView->SendData(i,ThBal[i-1].strBalCmd);
			Sleep(1000);
		}

		for(i=9; i<=12; i++)
		{

			pView->SendData(i,"Q");
			Sleep(1000);
		}*/
#endif

	// data¸¦ Control¼³Á¤ // common
	// m_dCommon[0][Balº°] : ¹ß¶õ½º ÇöÀç ¹«°Ô
	// m_dCommon[1][Balº°] : ÃøÁ¤ ÄÅÀÇ °æ°í ¹«°Ô
	for(i =0 ; i <m_ctrlCommon.GetRows() -1; i ++)
	{
		for (j =0 ; j <m_ctrlCommon.GetCols() -1; j++)
		{
			str.Format("%.1f", pDoc->m_dCommon[i][j]);
			m_ctrlCommon.SetTextMatrix(i+1, m_ctrlCommon.GetCols()-(j+1) ,(LPCSTR)str);
		}
	}

	// data¸¦ Control¼³Á¤ // Lc Supply Bal
	// m_dCanister[0][Balº°] : ¹ß¶õ½º ÇöÀç ¹«°Ô
	// m_dCanister[1][Balº°] : ¹ß¶õ½º °æ°í ¹«°Ô
	for(i =0 ; i <m_ctrlLcSupplyBal.GetRows() -1; i ++)
	{
		for (j =0 ; j <m_ctrlLcSupplyBal.GetCols() -1; j++)
		{
			str.Format("%.3f", pDoc->m_dCanister[i][j]);
			m_ctrlLcSupplyBal.SetTextMatrix(i+1, m_ctrlLcSupplyBal.GetCols()-(j+1) ,(LPCSTR)str);
		}
	}

	if(pDoc->m_nCanisterBalNo[0] == SUPPLY_BAL1)
	{
		pDoc->m_nCanisterBalNo[0] = SUPPLY_BAL1;
		m_ctrlLcBal1.SetCaption("Bal1");
		m_ctrlLcBal1.SetBackColor(GREEN);
	}
	else if(pDoc->m_nCanisterBalNo[0] == SUPPLY_BAL2)
	{
		pDoc->m_nCanisterBalNo[0] = SUPPLY_BAL2;
		m_ctrlLcBal1.SetCaption("Bal2");
		m_ctrlLcBal1.SetBackColor(YELLOW);
	}
	else
	{
		pDoc->m_nCanisterBalNo[0] = 0;

		if(pView->m_nLanguage == 0)
		m_ctrlLcBal1.SetCaption("»ç¿ë¾ÈÇÔ");
		else if(pView->m_nLanguage == 1)
		m_ctrlLcBal1.SetCaption("Not Use");
		else if(pView->m_nLanguage == 2)
		m_ctrlLcBal1.SetCaption("ÜôîêéÄ");

		m_ctrlLcBal1.SetBackColor(RED);
	}

	if(pDoc->m_nCanisterBalNo[1] == SUPPLY_BAL3)
	{
		pDoc->m_nCanisterBalNo[1] = SUPPLY_BAL3;
		m_ctrlLcBal2.SetCaption("Bal3");
		m_ctrlLcBal2.SetBackColor(GREEN);
	}
	else if(pDoc->m_nCanisterBalNo[1] == SUPPLY_BAL4)
	{
		pDoc->m_nCanisterBalNo[1] = SUPPLY_BAL4;
		m_ctrlLcBal2.SetCaption("Bal4");
		m_ctrlLcBal2.SetBackColor(YELLOW);
	}	
	else
	{
		pDoc->m_nCanisterBalNo[1] = 0;

		if(pView->m_nLanguage == 0)
		m_ctrlLcBal2.SetCaption("»ç¿ë¾ÈÇÔ");
		else if(pView->m_nLanguage == 1)
		m_ctrlLcBal2.SetCaption("Not Use");
		else if(pView->m_nLanguage == 2)
		m_ctrlLcBal2.SetCaption("ÜôîêéÄ");

		m_ctrlLcBal2.SetBackColor(RED);
	}

	if ( pDoc->m_bDropDetectRealTimeMode )
	{
		if(pView->m_nLanguage == 0)
		m_ctrlRunDetectMode.SetCaption("RUN & 1LINE MODE           ( ¸ÎÈû°¨Áö )"); 
		else if(pView->m_nLanguage == 1)
		m_ctrlRunDetectMode.SetCaption("RUN & 1LINE MODE           ( Detect )"); 
		else if(pView->m_nLanguage == 2)
		m_ctrlRunDetectMode.SetCaption("RUN & 1LINE MODE           ( Êïëë )"); 

		m_ctrlRunDetectMode.SetBackColor(GREEN); 
	}
	else
	{
		if(pView->m_nLanguage == 0)
		m_ctrlRunDetectMode.SetCaption("1LINE MODE		   ( ¸ÎÈû°¨Áö )"); 
		else if(pView->m_nLanguage == 1)
		m_ctrlRunDetectMode.SetCaption("1LINE MODE		   ( Detect )"); 
		else if(pView->m_nLanguage == 2)
		m_ctrlRunDetectMode.SetCaption("1LINE MODE		   ( Êïëë )"); 
		
		m_ctrlRunDetectMode.SetBackColor(RED); 
	}

	if(PC_TYPE==TRUE) SetTimer(TIMER_CLB_LCTYPE,500,NULL);

// 	if(bInitialShot == TRUE)
// 	{
// 		m_ctrlInitialShot.SetCaption("ÃÊ±â ÅäÃâ ON");
// 		m_ctrlInitialShot.SetBackColor(YELLOW);
// 	}
// 	else
// 	{
// 		m_ctrlInitialShot.SetCaption("ÃÊ±â ÅäÃâ OFF");
// 		m_ctrlInitialShot.SetBackColor(LIGHTBLUE);
// 	}

	//by shin//2009.03.30//
	//¿£Áö´Ï¾î ¿äÃ»¿¡ ÀÇÇØ ¿£Áö´Ï¾î ¸ðµå¿¡¼­¸¸ ¼±ÅÃ °¡´ÉÇÏµµ·Ï ¼öÁ¤...//
	if(pDoc->m_bIsEngineerMode==FALSE)
	{
		for(i=0; i<MAX_NOZZLE; i++)
		{
			m_pDropCountSensor[i] = (CButton*)GetDlgItem(IDC_CHK_DROP_H1+i);
			m_pDropCountSensor[i]->EnableWindow(FALSE);
		}

		m_crtlDropCntErrorRange.EnableWindow(FALSE); 
		m_ctrlLCDetectedCountSet.EnableWindow(FALSE); 
	}

	//by shin//2009.08.25//MC °ü·Ã TAS Ãß°¡...//
	//Data Clib
	pView->WriteTasMCData(TAS_MC, 5, BIT_ON); 
	Sleep(200);
//2010.06.07 by tskim
	GetDlgItem(IDC_RADIO_Door)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_RADIO_Remain)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CHECKGROUP)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CHK_DROP_H16)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CHK_DROP_H15)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CHK_DROP_H14)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CHK_DROP_H13)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CHK_DROP_H12)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CHK_DROP_H11)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CHK_DROP_H10)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CHK_DROP_H9)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CHK_DROP_H8)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CHK_DROP_H7)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CHK_DROP_H6)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CHK_DROP_H5)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CHK_DROP_H4)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CHK_DROP_H3)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CHK_DROP_H2)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CHK_DROP_H1)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CMD_DROP_COUNTING_RANGE)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
//	GetDlgItem(IDC_MSFLEXGRID_DROP_COUNT_ERROR_RANGE)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_COMMANDBUTTON4)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_LC_COUNT_SET)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_LABEL2)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CClb::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CPen pen;
	pen.CreatePen(PS_SOLID,5,DARKBLUE);
	CPen *pOldPen = (CPen *)dc.SelectObject(&pen);

	dc.MoveTo(0,HEIGHT_YPOS);
	dc.LineTo(SCREEN_WIDTH,HEIGHT_YPOS);
/*
	dc.MoveTo(0,SCREEN_HEIGHT - HEIGHT_YPOS);
	dc.LineTo(SCREEN_WIDTH,SCREEN_HEIGHT - HEIGHT_YPOS);
*/
	dc.SelectObject(pOldPen);

	// Do not call CFormView::OnPaint() for painting messages
}

void CClb::OnOK() 
{
	// TODO: Add extra validation here
	
//	CDialog::OnOK();
}

void CClb::OnCancel() 
{
	// TODO: Add extra cleanup here
	
//	CDialog::OnCancel();
}

BEGIN_EVENTSINK_MAP(CClb, CDialog)
    //{{AFX_EVENTSINK_MAP(CClb)
	ON_EVENT(CClb, IDC_MSFLEXGRID_OFFSET, -600 /* Click */, OnClickMsflexgridOffset, VTS_NONE)
	ON_EVENT(CClb, IDC_MSFLEXGRID_COMMON, -600 /* Click */, OnClickMsflexgridCommon, VTS_NONE)
	ON_EVENT(CClb, IDC_CMD_RETURN, -600 /* Click */, OnClickCmdReturn, VTS_NONE)
	ON_EVENT(CClb, IDC_CMD_REGIST, -600 /* Click */, OnClickCmdRegist, VTS_NONE)
	ON_EVENT(CClb, IDC_MSFLEXGRID_LC_LOT_NUM, -600 /* Click */, OnClickMsflexgridLcLotNum, VTS_NONE)
	ON_EVENT(CClb, IDC_LC_COUNT_SET, -600 /* Click */, OnClickLcCountSet, VTS_NONE)
	ON_EVENT(CClb, IDC_MSFLEXGRID_OFFSET2, -600 /* Click */, OnClickMsflexgridOffset2, VTS_NONE)
	ON_EVENT(CClb, IDC_INPUT_BARCODE, -600 /* Click */, OnClickInputBarcode, VTS_NONE)
	ON_EVENT(CClb, IDC_MSFLEXGRID_DROP_COUNT_ERROR_RANGE, -600 /* Click */, OnClickMsflexgridDropCountErrorRange, VTS_NONE)
	ON_EVENT(CClb, IDC_MSFLEXGRID_PUMP_NUM, -600 /* Click */, OnClickMsflexgridPumpNum, VTS_NONE)
	ON_EVENT(CClb, IDC_CMD_INITIAL_SHOT, -600 /* Click */, OnClickCmdInitialShot, VTS_NONE)
	ON_EVENT(CClb, IDC_CMD_PUMP_INITIAL, -600 /* Click */, OnClickCmdPumpInitial, VTS_NONE)
	ON_EVENT(CClb, IDC_MSFLEXGRID_LC_SUPPLY, -600 /* Click */, OnClickMsflexgridLcSupply, VTS_NONE)
	ON_EVENT(CClb, IDC_LABEL_HEAD1, -600 /* Click */, OnClickLabelHead1, VTS_NONE)
	ON_EVENT(CClb, IDC_LABEL_HEAD2, -600 /* Click */, OnClickLabelHead2, VTS_NONE)
	ON_EVENT(CClb, IDC_CMD_GREASE, -600 /* Click */, OnClickCmdGrease, VTS_NONE)
	ON_EVENT(CClb, IDC_CMD_DROP_COUNTING_RANGE, -600 /* Click */, OnClickCmdDropCountingRange, VTS_NONE)
	ON_EVENT(CClb, IDC_CMD_MODULE_INFO, -600 /* Click */, OnClickCmdModuleInfo, VTS_NONE)
	ON_EVENT(CClb, IDC_CMD_VISION_DROP_COUNT, -600 /* Click */, OnClickCmdVisionDropCount, VTS_NONE)
	ON_EVENT(CClb, IDC_CMD_DROPCOUNT_REALTIME, -600 /* Click */, OnClickCmdDropcountRealtime, VTS_NONE)
	ON_EVENT(CClb, IDC_CMD_MATERIAL_CHAN, -600 /* Click */, OnClickCmdMaterialChan, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CClb::OnClickMsflexgridOffset() 
{
	m_nClickCount = 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	BOOL bSingleRow = TRUE;
	double max = 0.0 , min = 0.0;
	int Row=m_ctrlOffset.GetMouseRow();
	int Col=m_ctrlOffset.GetMouseCol();

	if(Col>=1)
		if( (Row==0)&&(pDoc->m_bIsHeadSelected[(MAX_NOZZLE-1)-(Col-1)]==FALSE) ) return;
	if(Col == FALSE)	bSingleRow = FALSE;	
	
	max = 50 ; min = 0 ; 

	Use_TKg(m_ctrlOffset, max, min, 512, 384, TRUE, bSingleRow);



/*

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	BOOL bSingleRow = TRUE;
	double max = 0.0 , min = 0.0;
	int Row=m_ctrlOffset.GetMouseRow();
	int Col=m_ctrlOffset.GetMouseCol();

	m_nClickCount = 0;

	if(Col == FALSE) return;
	if(Col == FALSE)	bSingleRow = FALSE;	

	if(Col>=1)
	if( (Row==2)&&(pDoc->m_bIsHeadSelected[Col-1]==FALSE) ) return;

	if(Col == FALSE)	bSingleRow = FALSE;	

	max = 70 ; min = 0 ; 

	Use_TKg(m_ctrlOffset, max, min, 512, 384, TRUE, bSingleRow);*/

/*	for (int i =0 ; i <MAX_NOZZLE; i++)
		ThreadStage.nLCDetectedCount[i]=atoi((char *)(LPCSTR)m_ctrlOffset.GetTextMatrix(5,i+1));*/
}

void CClb::OnClickMsflexgridOffset2() 
{
	// TODO: Add your control notification handler code here
	m_nClickCount = 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();//JYKIM 161008 ¾×Á¤ÀÜ·® ÀÔ·Â½Ã PASSWORDÃ¢ Ãß°¡
	BOOL bSingleRow = TRUE;
	double max = 0.0 , min = 0.0;
	int Row=m_ctrlOffset2.GetMouseRow();
	int Col=m_ctrlOffset2.GetMouseCol();

//2010.06.05 by tskim 
	if(pDoc->m_bIsEngineerMode)//JYKIM 161008 ¾×Á¤ÀÜ·® ÀÔ·Â½Ã PASSWORDÃ¢ Ãß°¡
	{
		if(Row == 1 || Row == 2) return;

		if(Col>=1)
			if( (Row==2)&&(pDoc->m_bIsHeadSelected[(MAX_NOZZLE-1)-(Col-1)]==FALSE) ) return;
		if(Col == FALSE)	bSingleRow = FALSE;	
		if(Row == 0 ) { max = 1000 ; min = 0 ; }
		else if(Row == 1)	{ max = 500 ; min = 0 ;	}
		else if(Row == 2)	{ max = 10 ; min = 0 ;	}
		else { max = 3000 ; min = -3000 ; }
		
		Use_TKg(m_ctrlOffset2, max, min, 512, 384, TRUE, bSingleRow);

		for (int i =0 ; i <MAX_NOZZLE; i++)
			ThreadStage.nLCDetectedCount[i]=atoi((char *)(LPCSTR)m_ctrlOffset2.GetTextMatrix(2,(MAX_NOZZLE-1)-(i+1)));
	}
}
void CClb::OnClickMsflexgridCommon() 
{
	m_nClickCount = 0;
	BOOL bSingleRow = TRUE;
	if((m_ctrlCommon.GetMouseRow() == FALSE ) || (m_ctrlCommon.GetMouseRow() == 1 ) ) return;
	if(m_ctrlCommon.GetMouseCol() == FALSE)	bSingleRow = FALSE;	

	Use_TKg(m_ctrlCommon, 100000, 30000, 512, 384, TRUE, bSingleRow);//30gÀÌ»ó..	
}

void CClb::OnClickCmdReturn() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	CNormalMsg dlg;
	dlg.m_bTimer=FALSE;

	if(pView->m_nLanguage == 0)
	{
		dlg.m_strTitle = _T("È®ÀÎ");
		dlg.m_strMsg1 = " Pump No,¾×Á¤ Lot NoµîÀ» Á¤È®È÷ ÀÔ·Â ÇÏ¼Ì½À´Ï±î? ¶ÇÇÑ µ¥ÀÌÅÍ º¯°æ ÈÄ 'ÀúÀå' ¹öÆ°À» ´­·¶½À´Ï±î? ";
		dlg.m_strMsg2 = " ¸ÂÀ¸¸é 'OK', ±×·¸Áö ¾ÊÀ¸¸é 'CANCEL' ¹öÆ°À» ´©¸£¼¼¿ä...";
	}

	else if(pView->m_nLanguage == 1)
	{
		dlg.m_strTitle = _T("Check");
		dlg.m_strMsg1 = " did you input Pump NO, LC Lot NO exactly Or After data Change, did you push 'Save button'?.  ";
		dlg.m_strMsg2 = " if you Check 'OK' or Not Check, Please Push Button 'CANCEL'...";
	}
	else if(pView->m_nLanguage == 2)
	{
		dlg.m_strTitle = _T("ü¬ìã");
		dlg.m_strMsg1 = " Pump No,äûïÜLot NoÔõâÃìýïáü¬ÖõØ§£¿ûäâ¦ËàÜ¨ÌÚý¨ïÇÌª¡®ÜÁðí¡¯äÎÒïÖõØ§? ";
		dlg.m_strMsg2 = " ïáü¬ïÇÌª 'OK', ÜúöÎïÇÌªCANCEL...";
	}

	if( dlg.DoModal() == IDOK )
	{
		//by shin//2009.08.25//MC °ü·Ã TAS Ãß°¡...//
		//Data Clib
		pView->WriteTasMCData(TAS_MC, 5, BIT_OFF); 
		Sleep(10);
	
		m_nClickCount = 0;
		if(PC_TYPE==TRUE) KillTimer(TIMER_CLB_LCTYPE);
		EndDialog(IDOK);	
	}
}



void CClb::OnClickCmdRegist()	// Data ÀúÀå ¹öÆ°À» ´­·¶À» °æ¿ì
{
	m_nClickCount = 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

//	char *stopstring;
	CString Temp="";
	BOOL bIoResult=FALSE;
	
	int i = 0 , j = 0 ;
	// common
	for(i =0 ; i <m_ctrlCommon.GetRows() -1; i++)
	{
		for (j =0 ; j <m_ctrlCommon.GetCols() -1; j++)
		{
			pDoc->m_dCommon[i][j] = atof((char *)(LPCSTR)m_ctrlCommon.GetTextMatrix(i+1, m_ctrlCommon.GetCols()-1-j));
		}
	}

	// 
	CString strPathName;
	strPathName.Format("%s\\common.dat", pDoc->m_strDataPath);

	CFile SaveFile; 
	if( SaveFile.Open( (char *)(LPCSTR)strPathName, CFile::modeCreate | CFile::modeWrite ) )
	{
		pDoc->m_nSelFile = 3;
		CArchive ar( &SaveFile, CArchive::store );
		pDoc->Serialize(ar);
		ar.Close();
		SaveFile.Close();
	}
	
	// Pump No. & LC Lot No.
	//20160321	YSM	(PumpNo = ModuleID)
// 	for(i =0 ; i <m_ctrlPumpNo.GetCols(); i++)
// 	{
// 		pDoc->m_strPumpNo[i]=(LPCSTR)m_ctrlPumpNo.GetTextMatrix(1, (m_ctrlPumpNo.GetCols()-1)-i);
// 		pDoc->m_strPumpNo[i+(MAX_NOZZLE/2)]=(LPCSTR)m_ctrlPumpNo.GetTextMatrix(2, (m_ctrlPumpNo.GetCols()-1)-i);
// 	}
	
	for(i =0 ; i <m_ctrlLCLotNo.GetCols()-1; i++)
	{
		pDoc->m_strLCLotNo[i]=(LPCSTR)m_ctrlLCLotNo.GetTextMatrix(1, (m_ctrlLCLotNo.GetCols()-1)-i);
		pDoc->m_strLCLotNo[i+(MAX_NOZZLE/2)]=(LPCSTR)m_ctrlLCLotNo.GetTextMatrix(2, (m_ctrlLCLotNo.GetCols()-1)-i);

		
//		pDoc->m_strLCLotNo[i]=strtoul( (char *)(LPCSTR)m_ctrlLCLotNo.GetTextMatrix(1, i+1), &stopstring, 10 );
//		pDoc->m_strLCLotNo[i+(MAX_NOZZLE/2)]=strtoul( (char *)(LPCSTR)m_ctrlLCLotNo.GetTextMatrix(2, i+1), &stopstring, 10 );
	}
// 	if(m_nCheckType == 0)
// 	{
// 		for(i=0; i<MAX_NOZZLE; i++)
// 		{
// 			if(m_pDropCountSensor[i]->GetCheck()) 
// 				pDoc->m_bDropCountSensorUSE[i] = TRUE;
// 			else
// 				pDoc->m_bDropCountSensorUSE[i] = FALSE;
// //			pDoc->m_bDropCountSensorUSE[i] = m_bDropCount[i];
// 			pDoc->m_bRemainSensorUSE[i] = m_bRemain[i];
// 		}
// 	}
// 	else
// 	{
// 		for(i=0; i<MAX_NOZZLE; i++)
// 		{
// 			if(m_pDropCountSensor[i]->GetCheck()) 
// 				pDoc->m_bRemainSensorUSE[i] = TRUE;
// 			else
// 				pDoc->m_bRemainSensorUSE[i] = FALSE;
// 			pDoc->m_bDropCountSensorUSE[i] = m_bDropCount[i];
// //			pDoc->m_bRemainSensorUSE[i] = m_bRemain[i];
// 		}
// 	}
//2010.07.05 by tskim
// 	for(i=0;i<MAX_NOZZLE;i++)
// 	{
// 		pDoc->m_bDropCountSensorUSE[i] = m_bDropCount[i];
// 		pDoc->m_bRemainSensorUSE[i] = m_bRemain[i];
// 		pDoc->m_bLowDetectSensorUSE[i] = m_bLowDetect[i];
// 	}

//	for(i=0; i<2; i++)	// 0 : »óÇÑ, 1: ÇÏÇÑ
//		pDoc->m_dDropCountErrorRange[i]=atoi((char *)(LPCSTR)m_crtlDropCntErrorRange.GetTextMatrix(i, 1));

	pDoc->m_dSetLCDetectedCount=atoi(m_ctrlLCDetectedCountSet.GetCaption());	//(char*)(LPCSTR)

	strPathName.Format("%s\\pumpLCNo.dat", pDoc->m_strDataPath);

	if( SaveFile.Open( (char *)(LPCSTR)strPathName, CFile::modeCreate | CFile::modeWrite ) )
	{
		pDoc->m_nSelFile = 4;
		CArchive ar( &SaveFile, CArchive::store );
		pDoc->Serialize(ar);
		ar.Close();
		SaveFile.Close();
	}

	// offset
	for(i =0 ; i < 2; i++)
	{
		for (j =0 ; j <m_ctrlOffset2.GetCols() -1; j++)
		{
			pDoc->m_dOffset[i][j] = atof((char *)(LPCSTR)m_ctrlOffset2.GetTextMatrix(i,m_ctrlOffset2.GetCols()-1-j));
		}
	}
//2015.04.29 by tskim ¿øÀç·á Àü»êÈ­Add LCÀÜ·®º¸°í
//	strPathName.Format("%s\\offset.dat", pDoc->m_strDataPath);
//
//	if( SaveFile.Open( (char *)(LPCSTR)strPathName, CFile::modeCreate | CFile::modeWrite ) )
//	{
//		pDoc->m_nSelFile = 2;
//		CArchive ar( &SaveFile, CArchive::store );
//		pDoc->Serialize(ar);
//		ar.Close();
//		SaveFile.Close();
//	}
	pDoc->Save_LCRemain();

	// LC ÀÜ·® °¨Áö Sensor¸¦ È®ÀÎÇÏ°í Ã³¸®µÈ Count¸¦ SaveÇÑ´Ù.
	for(i=0;i<MAX_NOZZLE;i++)
	{
		if(pDoc->m_bIsHeadSelected[i] == TRUE)
		{	
			FAS_GetIoBit(1,true,LC_LIMIT1+i,&bIoResult);  
			if(bIoResult==TRUE)	// ¾×Á¤·®ÀÌ °¨Áö µÇ¸é ÃÊ±âÈ­ ÇÑ´Ù.
			{
				ThreadStage.bLCDectectedMem[i]=FALSE;
				ThreadStage.nLCDetectedCount[i]=0;
			}			
		}
		else
		{
			ThreadStage.bLCDectectedMem[i]=FALSE;
			ThreadStage.nLCDetectedCount[i]=0;
		}
	}
	for(i =0 ; i <m_ctrlOffset2.GetCols()-1; i++)
	{
		m_ctrlOffset2.SetRow(2);
		m_ctrlOffset2.SetCol(m_ctrlOffset2.GetCols()-1-i);
		m_ctrlOffset2.SetCellBackColor(WHITE);		
		if(pDoc->m_bIsHeadSelected[i] == TRUE)
		{
			FAS_GetIoBit(1,true,LC_LIMIT1+i,&bIoResult);
			if(!bIoResult)	
				m_ctrlOffset2.SetCellBackColor(RED);				
		}
	}

	// LC Supply Bal
	for(i =0 ; i <m_ctrlLcSupplyBal.GetRows() -1; i++)
	{
		for (j =0 ; j <m_ctrlLcSupplyBal.GetCols() -1; j++)
		{
			pDoc->m_dCanister[i][j] = atof((char *)(LPCSTR)m_ctrlLcSupplyBal.GetTextMatrix(i+1,m_ctrlLcSupplyBal.GetCols()-1-j));
		}
	}
	
	strPathName.Format("%s\\Canister.dat", pDoc->m_strDataPath);

	if( SaveFile.Open( (char *)(LPCSTR)strPathName, CFile::modeCreate | CFile::modeWrite ) )
	{
		pDoc->m_nSelFile = 6;
		CArchive ar( &SaveFile, CArchive::store );
		pDoc->Serialize(ar);
		ar.Close();
		SaveFile.Close();
	}

	// dummy cup weight
	for(i =0 ; i <m_ctrlOffset.GetRows() -1; i++)
	{
		for (j =0 ; j <m_ctrlOffset.GetCols() -1; j++)
		{
			pDoc->m_dDummyCupWeight[i][j] = atof((char *)(LPCSTR)m_ctrlOffset.GetTextMatrix(i+1,(m_ctrlOffset.GetCols()-1-j)));
		}
	}

	// 
	strPathName.Format("%s\\DummyCupWeight.dat", pDoc->m_strDataPath);

	if( SaveFile.Open( (char *)(LPCSTR)strPathName, CFile::modeCreate | CFile::modeWrite ) )
	{
		pDoc->m_nSelFile = 7;
		CArchive ar( &SaveFile, CArchive::store );
		pDoc->Serialize(ar);
		ar.Close();
		SaveFile.Close();
	}	

//	for(i = 0;i <m_ctrlOffset.GetCols()-1;i++)
//	{
//		HeadOffset[i] = atof((char *)(LPCSTR)m_ctrlOffset.GetTextMatrix(1, i+1));
//	}
//	HeadOffsetSave();
	pView->m_pDevice->HeadSPositionOffsetRead();
}

void CClb::SubDisplayRecipeName()
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	CString str = "";

	// Recipe Name ¼³Á¤
	str.Format("%s|%s", pDoc->m_structOperatorModeRecipeData.strFrontRecipeName, pDoc->m_structOperatorModeRecipeData.strBackRecipeName);

	m_ctrlRecipeName.SetCaption(str);
}

void CClb::SubClbDataSave()		// ÇÁ·Î±×·¥À» Á¾·áÇÒ ¶§ Calibration Data¸¦ ÀúÀåÇÏµµ·Ï ÇÑ´Ù..
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CString strPathName=""; CFile SaveFile;

	strPathName.Format("%s\\pumpLCNo.dat", pDoc->m_strDataPath);
	if( SaveFile.Open( (char *)(LPCSTR)strPathName, CFile::modeCreate | CFile::modeWrite ) )
	{
		pDoc->m_nSelFile = 4;
		CArchive ar( &SaveFile, CArchive::store );
		pDoc->Serialize(ar);
		ar.Close();
		SaveFile.Close();
	}
	// common
	strPathName.Format("%s\\common.dat", pDoc->m_strDataPath);
	if( SaveFile.Open( (char *)(LPCSTR)strPathName, CFile::modeCreate | CFile::modeWrite ) )
	{
		pDoc->m_nSelFile = 3;
		CArchive ar( &SaveFile, CArchive::store );
		pDoc->Serialize(ar);
		ar.Close();
		SaveFile.Close();
	}
	// offset
//2015.04.29 by tskim ¿øÀç·á Àü»êÈ­Add LCÀÜ·®º¸°í
//	strPathName.Format("%s\\offset.dat", pDoc->m_strDataPath);
//	if( SaveFile.Open( (char *)(LPCSTR)strPathName, CFile::modeCreate | CFile::modeWrite ) )
//	{
//		pDoc->m_nSelFile = 2;
//		CArchive ar( &SaveFile, CArchive::store );
//		pDoc->Serialize(ar);
//		ar.Close();
//		SaveFile.Close();
//	}
	pDoc->Save_LCRemain();
	//Canister
	strPathName.Format("%s\\Canister.dat", pDoc->m_strDataPath);
	if( SaveFile.Open( (char *)(LPCSTR)strPathName, CFile::modeCreate | CFile::modeWrite ) )
	{
		pDoc->m_nSelFile = 6;
		CArchive ar( &SaveFile, CArchive::store );
		pDoc->Serialize(ar);
		ar.Close();
		SaveFile.Close();
	}

	// LcType
//	pDoc->SaveBarcodeData();
}



void CClb::OnClickMsflexgridPumpNum() 
{
	return;		//20160321	YSM	(PumpNo = ModuleID)
	
	m_nClickCount = 0;
	if(m_ctrlPumpNo.GetMouseRow()==0) return;

	unsigned int max = 9999 , min = 0;
	CString strTemp="",strTemp1="";
	int i=0, strlen=0, nC=0, nR=0;
	
	nC = m_ctrlPumpNo.GetCol();
	nR = m_ctrlPumpNo.GetRow();
	m_ctrlPumpNoLabel.SetCaption("");	
	Use_TK(m_ctrlPumpNoLabel, max, min, 512, 384);

	strTemp1=m_ctrlPumpNoLabel.GetCaption();
	strlen=strTemp1.GetLength();

	if(strlen<4)
	{
		for(i=0;i<(4-strlen);i++)
		{
			strTemp1="0"+strTemp1;
		}
	}
//	strTemp1="P8"+strTemp1+" ";
//	strTemp1=strTemp1+" ";
	m_ctrlPumpNoLabel.SetCaption(strTemp1);	
	m_ctrlPumpNo.SetTextMatrix(nR,nC,m_ctrlPumpNoLabel.GetCaption());	

}

void CClb::OnClickMsflexgridLcLotNum() 
{
	// TODO: Add your control notification handler code here
/*	m_nClickCount = 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	BOOL bSingleRow = TRUE;
	unsigned long max = 4294967295 , min = 0;
	CString strTemp = "";
	int nC=0, nR=0, nCount = 0;

	nC = m_ctrlLCLotNo.GetCol();
	nR = m_ctrlLCLotNo.GetRow();

	if(m_ctrlLCLotNo.GetMouseRow()==0) return;


	if(nC == 0) 
	{
		strTemp=m_ctrlLCLotNo.GetTextMatrix(nR,nC+1);		
	}
	else strTemp=m_ctrlLCLotNo.GetTextMatrix(nR,nC);
	m_ctrlLcLotName.SetCaption(strTemp);

	Use_STR(m_ctrlLcLotName, CPoint(70,300));	

	nCount = m_ctrlLcLotName.GetCaption().GetLength();

	nCount = m_ctrlLcLotName.GetCaption().GetLength();

	if (!(nCount==10))	
	{
		m_ctrlLcLotName.SetCaption(strTemp);
		AfxMessageBox("ÀÔ·Â ¹®ÀÚÀÇ °³¼ö°¡ ¸ÂÁö ¾Ê½À´Ï´Ù. 10ÀÚ¸® ÀÔ·Â");
		return;
	}

	strTemp =  m_ctrlLcLotName.GetCaption();

	if((nC == 0) && ((nR == 1) || (nR == 2)))
	{
		for(int i = 0; i < MAX_NOZZLE/2; i++)
		{
			m_ctrlLCLotNo.SetTextMatrix(nR,i+1,m_ctrlLcLotName.GetCaption());
		}
	}
	else
	{
		m_ctrlLCLotNo.SetTextMatrix(nR,nC,m_ctrlLcLotName.GetCaption());
	}	

	for(int i=0;i<MAX_NOZZLE/2;i++)
	{
		if(pDoc->m_bIsHeadSelected[i] == FALSE)
		{
			strTemp = "0000000000";
			m_ctrlLCLotNo.SetTextMatrix(1,i+1,strTemp);
		}
	}
	
	for(i=0;i<MAX_NOZZLE/2;i++)
	{
		if(pDoc->m_bIsHeadSelected[i+MAX_NOZZLE/2] == FALSE)
		{
			strTemp = "0000000000";
			m_ctrlLCLotNo.SetTextMatrix(2,i+1,strTemp);
		}
	}
*/
}

void CClb::OnClickMsflexgridDropCountErrorRange() 
{
	// TODO: Add your control notification handler code here
	return;
/*	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	int max = 9999 , min = 0;
	if(m_crtlDropCntErrorRange.GetMouseCol()==0)	return;

	Use_TKg(m_crtlDropCntErrorRange, max, min, 512, 384, TRUE, TRUE);	
*/
}

void CClb::OnClickLcCountSet() 
{
	m_nClickCount = 0;
	Use_TK(m_ctrlLCDetectedCountSet, 10, 0 ,350 ,100);
}

void CClb::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	BOOL bIoResult=FALSE;

	switch(nIDEvent)
	{
	case TIMER_CLB_LCTYPE:

		break;
	}

	CDialog::OnTimer(nIDEvent);
}

void CClb::OnRADIODoor() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	int i,pat;
	m_nClickCount = 0;

	if(m_nCheckType == 1)
	{
		for(i = 0; i < MAX_NOZZLE; i++)
		{
			if(m_pDropCountSensor[i]->GetCheck() == TRUE) m_bRemain[i] = TRUE;
			else m_bRemain[i] = FALSE;
		}
	}
	m_nCheckType = 0;
	GetDlgItem(IDC_CHECKGROUP)->SetWindowText(" *  Drop Count Sensor »ç¿ë/¹Ì»ç¿ë ¼±ÅÃ  ");
	for(i=0; i<MAX_NOZZLE; i++)
	{
		m_pDropCountSensor[i]->EnableWindow(FALSE);
		m_pDropCountSensor[i]->SetCheck(FALSE);
	}

	for(pat = 0 ; pat < pDoc->m_structDataEditor.m_nPatternNum ; pat++)
	{
		for(i=0; i<MAX_NOZZLE; i++)
		{		
			if(pDoc->m_structPatternData[pat].m_bIsOnHead[i]==TRUE)
			{
				m_pDropCountSensor[i]->EnableWindow(TRUE);
				if(m_bDropCount[i]==TRUE)	m_pDropCountSensor[i]->SetCheck(TRUE);
			}
		}
	}

	//by shin//2009.03.30//
	//¿£Áö´Ï¾î ¿äÃ»¿¡ ÀÇÇØ ¿£Áö´Ï¾î ¸ðµå¿¡¼­¸¸ ¼±ÅÃ °¡´ÉÇÏµµ·Ï ¼öÁ¤...//
	if(pDoc->m_bIsEngineerMode==FALSE)
	{
		for(i=0; i<MAX_NOZZLE; i++)
		{
			m_pDropCountSensor[i] = (CButton*)GetDlgItem(IDC_CHK_DROP_H1+i);
			m_pDropCountSensor[i]->EnableWindow(FALSE);
		}
	}
}

void CClb::OnRADIORemain() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	int i;
	if(m_nCheckType == 0)
	{
		for(i = 0; i < MAX_NOZZLE; i++)
		{
			if(m_pDropCountSensor[i]->GetCheck() == TRUE) m_bDropCount[i] = TRUE;
			else m_bDropCount[i] = FALSE;
		}
	}
	m_nCheckType = 1;
	GetDlgItem(IDC_CHECKGROUP)->SetWindowText(" *  ÀÜ·® °¨Áö Sensor »ç¿ë/¹Ì»ç¿ë ¼±ÅÃ  ");
	for(i=0; i<MAX_NOZZLE; i++)
	{
		m_pDropCountSensor[i]->EnableWindow(TRUE);
		if(m_bRemain[i] == TRUE) m_pDropCountSensor[i]->SetCheck(TRUE);
		else m_pDropCountSensor[i]->SetCheck(FALSE);
	}
	//by shin//2009.03.30//
	//¿£Áö´Ï¾î ¿äÃ»¿¡ ÀÇÇØ ¿£Áö´Ï¾î ¸ðµå¿¡¼­¸¸ ¼±ÅÃ °¡´ÉÇÏµµ·Ï ¼öÁ¤...//
	if(pDoc->m_bIsEngineerMode==FALSE)
	{
		for(i=0; i<MAX_NOZZLE; i++)
		{
			m_pDropCountSensor[i] = (CButton*)GetDlgItem(IDC_CHK_DROP_H1+i);
			m_pDropCountSensor[i]->EnableWindow(FALSE);
		}
	}
}

void CClb::Doc2Val()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	int i;

	for(i = 0; i < MAX_NOZZLE; i++)
	{
		m_bDropCount[i] = pDoc->m_bDropCountSensorUSE[i];
		m_bRemain[i] = pDoc->m_bRemainSensorUSE[i];
	}
}

BOOL CClb::HeadOffsetRead()
{
	CString strPathName="";
	ifstream fi;

	strPathName.Format("%s","D:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\HeadSPositionOffset.dat");	
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);

	if(fi.is_open())
	{
		fi >> HeadOffset[0];
		fi >> HeadOffset[1];
		fi >> HeadOffset[2];
		fi >> HeadOffset[3];
		fi >> HeadOffset[4];
		fi >> HeadOffset[5];
		fi >> HeadOffset[6];
		fi >> HeadOffset[7];
		fi >> HeadOffset[8];
		fi >> HeadOffset[9];
		fi >> HeadOffset[10];
		fi >> HeadOffset[11];
		fi >> HeadOffset[12];
		fi >> HeadOffset[13];
		fi >> HeadOffset[14];
		fi >> HeadOffset[15];
		fi.close();
	}
	else
	{
		return FALSE;
	}
	return TRUE;

}

void CClb::HeadOffsetSave()
{
	FILE *fp;
	CString strPath ="";
	strPath.Format("%s", "D:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\HeadSPositionOffset.dat");
	fp = fopen(strPath, "wt");
	if(fp != NULL)
	{
		for(int i = 0; i < MAX_NOZZLE; i++)
			fprintf(fp, "%.2f\n", HeadOffset[i]);
	}
	// È­ÀÏ ´Ý±â
	fclose(fp);
}


void CClb::OnClickInputBarcode() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	m_nClickCount = 0;
	CBarcode dlg;
	dlg.DoModal();

	// LC Lot No. ¼³Á¤
	for(int i =1 ; i <m_ctrlLCLotNo.GetCols(); i++)
	{
		m_ctrlLCLotNo.SetColAlignment(i,7);	// ¿ìÃø Á¤·Ä
		m_ctrlLCLotNo.SetTextMatrix(1, (MAX_NOZZLE/2+1)-i ,(LPCSTR)pDoc->m_strLCLotNo[i-1]);
		m_ctrlLCLotNo.SetTextMatrix(2, (MAX_NOZZLE/2+1)-i ,(LPCSTR)pDoc->m_strLCLotNo[i+MAX_NOZZLE/2-1]);
	}	
//2015.04.29 by tskim ¿øÀç·á Àü»êÈ­Add LCÀÜ·®º¸°í
	CString str;
	for (int j =0 ; j <m_ctrlOffset2.GetCols() -1; j++)
	{
		pDoc->m_dOffset[0][j] = atof(pDoc->m_strRFDataResidualData[j]);
		pDoc->m_dOffset_Prev[0][j] = pDoc->m_dOffset[0][j];//ECS·Î ºÎÅÍ ÀÜ·® ¼ö½Å->m_dOffset_Prev¿¡ ¹é¾÷
		str.Format("%.0f", pDoc->m_dOffset[0][j]);
		m_ctrlOffset2.SetTextMatrix(0, (MAX_NOZZLE+1)-(j+1) ,(LPCSTR)str);
	}
}

void CClb::OnClickCmdInitialShot() //2010.06.07 by tskim head º°µµ ºÐ¸® ÇÑ´Ù..
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CString str;
	int i,j;

	if(pDoc->m_bIsEngineerMode == FALSE) return;
	else
	{
		CHeadDropInformation dlg;
		dlg.DoModal();
		
		for(i =0 ; i <m_ctrlOffset2.GetRows(); i ++)
		{
			for (j =0 ; j <m_ctrlOffset2.GetCols() -1; j++)
			{
				if(i<2) str.Format("%.1f", pDoc->m_dOffset[i][j]);
				else if(i==2) str.Format("%d", ThreadStage.nLCDetectedCount[j]);	// ÀÜ·® °¨Áö ÈÄ ÀÛ¾÷ÇÑ Glass ¼ö 
				m_ctrlOffset2.SetTextMatrix(i, (MAX_NOZZLE+1)-(j+1) ,(LPCSTR)str);
			}
		}
		// 		if(bInitialShot == TRUE)
		// 		{
		// 			m_ctrlInitialShot.SetCaption("ÃÊ±â ÅäÃâ OFF");
		// 			m_ctrlInitialShot.SetBackColor(LIGHTBLUE);
		// 			bInitialShot = FALSE;
		// 		}
		// 		else
		// 		{
		// 			m_ctrlInitialShot.SetCaption("ÃÊ±â ÅäÃâ ON");
		// 			m_ctrlInitialShot.SetBackColor(YELLOW);
		// 			bInitialShot = TRUE;
		// 		}
	}		
}

void CClb::OnClickCmdPumpInitial() 
{
	// TODO: Add your control notification handler code here

	if(PC_TYPE==TRUE) KillTimer(TIMER_CLB_LCTYPE);

	g_bInitilDisplay = TRUE;

	CAdjustInitial dlg;
	dlg.DoModal();

}

void CClb::OnClickMsflexgridLcSupply() 
{
	// TODO: Add your control notification handler code here
	m_nClickCount = 0;
	BOOL bSingleRow = TRUE;
	if(m_ctrlLcSupplyBal.GetMouseRow() == FALSE ) return;  // || (m_ctrlLcSupplyBal.GetMouseRow() == 1 ) ) return;
	if(m_ctrlLcSupplyBal.GetMouseCol() == FALSE)	bSingleRow = FALSE;	

	Use_TKg(m_ctrlLcSupplyBal, 20, 2, 512, 384, TRUE, bSingleRow);//2kgÀÌ»ó..	
}

void CClb::OnClickLabelHead1() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(pDoc->m_nCanisterBalNo[0] == SUPPLY_BAL1)
	{
		pDoc->m_nCanisterBalNo[0] = SUPPLY_BAL2;
		m_ctrlLcBal1.SetCaption("Bal2");
		m_ctrlLcBal1.SetBackColor(YELLOW);
	}
	else if(pDoc->m_nCanisterBalNo[0] == SUPPLY_BAL2)
	{
		pDoc->m_nCanisterBalNo[0] = 0;
		
		if(pView->m_nLanguage == 0)
		m_ctrlLcBal1.SetCaption("»ç¿ë¾ÈÇÔ");
		else if(pView->m_nLanguage == 1)
		m_ctrlLcBal1.SetCaption("Not Use");
		else if(pView->m_nLanguage == 2)
		m_ctrlLcBal1.SetCaption("ÜôîêéÄ");
		
		m_ctrlLcBal1.SetBackColor(RED);
	}
	else
	{
		pDoc->m_nCanisterBalNo[0] = SUPPLY_BAL1;
		m_ctrlLcBal1.SetCaption("Bal1");
		m_ctrlLcBal1.SetBackColor(GREEN);
	}
}

void CClb::OnClickLabelHead2() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	if(pDoc->m_nCanisterBalNo[1] == SUPPLY_BAL3)
	{
		pDoc->m_nCanisterBalNo[1] = SUPPLY_BAL4;
		m_ctrlLcBal2.SetCaption("Bal4");
		m_ctrlLcBal2.SetBackColor(YELLOW);
	}
	else if(pDoc->m_nCanisterBalNo[1] == SUPPLY_BAL4)
	{
		pDoc->m_nCanisterBalNo[1] = 0;
		m_ctrlLcBal2.SetCaption("Not Use");
		m_ctrlLcBal2.SetBackColor(RED);
	}	
	else
	{
		pDoc->m_nCanisterBalNo[1] = SUPPLY_BAL3;
		m_ctrlLcBal2.SetCaption("Bal3");
		m_ctrlLcBal2.SetBackColor(GREEN);
	}
}

void CClb::OnClickCmdGrease() 
{
	// TODO: Add your control notification handler code here
	m_nClickCount = 0;
	g_bMotorMoveView = FALSE;
	CGrease dlg;
	dlg.DoModal();	
}

void CClb::OnClickCmdDropCountingRange() 
{
	// TODO: Add your control notification handler code here
	CCounttingRange dlg;
	dlg.DoModal();
}

void CClb::RefreshData()
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	int i;
	CString strTemp;
	// LC Lot No. ¼³Á¤
	for(i =0 ; i <8; i++)
	{
		m_ctrlLCLotNo.SetColAlignment(i,7);	// ¿ìÃø Á¤·Ä
		
		if(pDoc->m_bIsHeadSelected[i] == FALSE)
		{
			strTemp = "0000000000";
			m_ctrlLCLotNo.SetTextMatrix(1,(MAX_NOZZLE/2)-(i),strTemp);
		}
		else
		{
			m_ctrlLCLotNo.SetTextMatrix(1, (MAX_NOZZLE/2)-(i) ,(LPCSTR)pDoc->m_strLCLotNo[i]);
		}
		if(pDoc->m_bIsHeadSelected[(i+MAX_NOZZLE/2)] == FALSE)
		{
			strTemp = "0000000000";
			m_ctrlLCLotNo.SetTextMatrix(2, (MAX_NOZZLE/2)-(i),strTemp);
		}
		else
		{
			m_ctrlLCLotNo.SetTextMatrix(2, (MAX_NOZZLE/2)-(i) ,(LPCSTR)pDoc->m_strLCLotNo[(i+MAX_NOZZLE/2)]);
		}			
	}

	if( ThreadStage.Recipe_Change == TRUE )
	{
			for(i =0 ; i <8; i++)
			{
			m_ctrlLCLotNo.SetColAlignment(i,7);	// ¿ìÃø Á¤·Ä

			m_ctrlLCLotNo.SetTextMatrix(1,(i+1),strTemp);
			m_ctrlLCLotNo.SetTextMatrix(2, (i+1),strTemp);
		
			}
	}
}

void CClb::OnClickCmdModuleInfo() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();

	pView->bModuleInfoStatus = TRUE; 

	CModuleInfo dlg;
	dlg.DoModal();
}

void CClb::OnClickCmdVisionDropCount() 
{
	// TODO: Add your control notification handler code here
	CVisionDropCountSet dlg;
	dlg.DoModal();
}

void CClb::OnClickCmdDropcountRealtime() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();

	if ( !pDoc->m_bDropDetectRealTimeMode )
	{
		pDoc->m_bDropDetectRealTimeMode = TRUE; 
	
		if(pView->m_nLanguage == 0)
		m_ctrlRunDetectMode.SetCaption("RUN & 1LINE MODE           ( ¸ÎÈû°¨Áö )"); 
		else if(pView->m_nLanguage == 1)
		m_ctrlRunDetectMode.SetCaption("RUN & 1LINE MODE           ( Detect )"); 
		else if(pView->m_nLanguage == 2)
		m_ctrlRunDetectMode.SetCaption("RUN & 1LINE MODE           ( Êïëë )"); 


		m_ctrlRunDetectMode.SetBackColor(GREEN); 
		pView->SaveLog(0,"RUN & 1LINE MODE           ( ¸ÎÈû°¨Áö )");
		pView->m_pDevice->SaveDeviceLog("RUN & 1LINE MODE           ( ¸ÎÈû°¨Áö )");
	}
	else
	{
		pDoc->m_bDropDetectRealTimeMode = FALSE; 
			
		if(pView->m_nLanguage == 0)
		m_ctrlRunDetectMode.SetCaption("1LINE MODE		   ( ¸ÎÈû°¨Áö )"); 
		else if(pView->m_nLanguage == 1)
		m_ctrlRunDetectMode.SetCaption("1LINE MODE		   ( Detect )"); 
		else if(pView->m_nLanguage == 2)
		m_ctrlRunDetectMode.SetCaption("1LINE MODE		   ( Êïëë )"); 
		
		
		m_ctrlRunDetectMode.SetBackColor(RED); 
		pView->SaveLog(0,"1LINE MODE           ( ¸ÎÈû°¨Áö )");
		pView->m_pDevice->SaveDeviceLog("1LINE MODE           ( ¸ÎÈû°¨Áö )");
	}	
}

void CClb::SelectLanguage()	//ehji 140606
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(pView->m_nLanguage == 0) 
	{
		SetDlgItemText(IDC_CMD_INITIAL_SHOT, _T(" Head Drop Á¤º¸"));		
		SetDlgItemText(IDC_CMD_GREASE, _T("¸ðÅÍ ÁÖÇà °Å¸® ¼³Á¤"));
		SetDlgItemText(IDC_LABEL_DUMMY_WEIGHT, _T("Dummy Cup ¾×Á¤·® ¼³Á¤"));
		SetDlgItemText(IDC_LABEL_RECIPE3, _T("* ¾×Á¤ Lot No."));
		SetDlgItemText(IDC_LABEL_RECIPE4, _T("* ÃøÁ¤ Cup   Setting"));
		SetDlgItemText(IDC_INPUT_BARCODE, _T("¾×Á¤ ¹ÙÄÚµå ÀÔ·Â"));
		SetDlgItemText(IDC_CMD_REGIST, _T("Àú Àå"));
		SetDlgItemText(IDC_CMD_RETURN, _T("ÀÌÀüÈ­¸é"));
		SetDlgItemText(IDC_CMD_PUMP_INITIAL, _T("Pump ÃÊ±â È­¸é"));
		SetDlgItemText(IDC_LABEL_RECIPE5, _T("LC ¾×Á¤·® ¼³Á¤"));

	}
	else if(pView->m_nLanguage == 1) 
	{
		SetDlgItemText(IDC_CMD_INITIAL_SHOT, _T(" Head Drop Information"));		
		SetDlgItemText(IDC_CMD_GREASE, _T("the Motor Distance Covered Set"));
		SetDlgItemText(IDC_LABEL_DUMMY_WEIGHT, _T("Dummy Cup LC Amount Set"));
		SetDlgItemText(IDC_LABEL_RECIPE3, _T("* LC Lot No."));
		SetDlgItemText(IDC_LABEL_RECIPE4, _T("* Measure Cup   Setting"));
		SetDlgItemText(IDC_INPUT_BARCODE, _T("LC Barcord Input"));
		SetDlgItemText(IDC_CMD_REGIST, _T("SAVE"));
		SetDlgItemText(IDC_CMD_RETURN, _T("Before"));
		SetDlgItemText(IDC_CMD_PUMP_INITIAL, _T("Pump Initial Scene"));
		SetDlgItemText(IDC_LABEL_RECIPE5, _T("LC Amount Set"));

	}
	else if(pView->m_nLanguage == 2) 
	{
		SetDlgItemText(IDC_CMD_INITIAL_SHOT, _T(" Head Drop ï×ÜÃ"));		
		SetDlgItemText(IDC_CMD_GREASE, _T("ï³Ñ¦òûú¼Ëå×îàâïÒ"));
		SetDlgItemText(IDC_LABEL_DUMMY_WEIGHT, _T("Dummy Cup äûïÜÕáàâïÒ"));
		SetDlgItemText(IDC_LABEL_RECIPE3, _T("* äûïÜ Lot No."));
		SetDlgItemText(IDC_LABEL_RECIPE4, _T("* ö´ïÒ Cup   Setting"));
		SetDlgItemText(IDC_INPUT_BARCODE, _T("âÃìýäûïÜðÉû¡Ø§"));
		SetDlgItemText(IDC_CMD_REGIST, _T("ÜÁðí"));
		SetDlgItemText(IDC_CMD_RETURN, _T("Ú÷üÞ"));
		SetDlgItemText(IDC_CMD_PUMP_INITIAL, _T("PumpôøÑ¢ûþØü"));
		SetDlgItemText(IDC_LABEL_RECIPE5, _T("LC äûïÜÕáàâïÒ"));

	}

}

void CClb::OnClickCmdMaterialChan() 
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	// TODO: Add your control notification handler code here

	CEmployee_Num dlg;
	dlg.DoModal();
}

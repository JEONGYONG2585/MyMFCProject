// ModuleInfo.cpp : implementation file
//

#include "stdafx.h"
#include "p8ca_lcdisp.h"
#include "ModuleInfo.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"
#include "color.h"
#include "ModuleDBRegistry.h"

#include "DevicePart.h"		//150227 ehji 
#include "Serial.h"			//150227 ehji
#include "ModuleBarTeaching.h"		//150227 ehji

#include "NormalMsg.h"

#include <fstream.h>
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModuleInfo dialog


CModuleInfo::CModuleInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CModuleInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModuleInfo)
		// NOTE: the ClassWizard will add member initialization here
	strModuleID = "";
	nAmplitude_DB = 0;
	nNegOffset_DB = 0;
	nFrequecy_DB = 0;
	nDuty_DB = 0.0;
	nFirst_Vol_DB = 0;
	nFallingEdge_DB = 0;
	//}}AFX_DATA_INIT

//141113 «ÏµÂ √ ±‚»≠ ªË¡¶ 
//	for(int i=0 ; i<MAX_NOZZLE ;i++)
//		Drop_Info.manu_head_job[i] = false;

	bModuleID = FALSE; 

	m_bReadStatus = FALSE; 
}


void CModuleInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModuleInfo)
	DDX_Control(pDX, IDC_GRID_MODULE_INFO, m_GridModule);
	DDX_Control(pDX, IDC_LABEL_MSG, m_strMsgDB);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModuleInfo, CDialog)
	//{{AFX_MSG_MAP(CModuleInfo)
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModuleInfo message handlers

BOOL CModuleInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView(); 

	// TODO: Add extra initialization here
	CString str;
	str.Format("%s|%s", pDoc->m_structOperatorModeRecipeData.strFrontRecipeName, pDoc->m_structOperatorModeRecipeData.strBackRecipeName);
	SetDlgItemText(IDC_LABEL_RECIPENAME, str);


	SelectLanguage();

	LoadModuleBarTeach();		//ehji 150227

	SetGrid_Initial();
	//Get_ModuleInfo(nHead);

	pView->pN_Func->Read_Module_DB(pDoc->m_structOperatorModeRecipeData.strFrontRecipeName);


	for ( int nHeadCount = 0; nHeadCount < MAX_NOZZLE; nHeadCount++ )
	{
		if(pDoc->m_bIsHeadSelected[nHeadCount])
			Get_ModuleInfo(atoi(pView->pN_Func->m_structNParam[nHeadCount].m_strModuleID),nHeadCount,TRUE);
	}


	GetDlgItem(IDOK)    ->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDCANCEL)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	

//	GetDlgItem(IDC_CMD_READ)   ->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );

	GetDlgItem(IDC_DB_MODIFY)  ->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );	
	GetDlgItem(IDC_CMD_BARCODE)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );

	if ( !pView->bModuleInfoStatus )
	{
		GetDlgItem(IDC_CMD_MODULE_DB_REGISTRY)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
		GetDlgItem(IDC_CMD_MODULE_DB_BEFORE)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//2015.11.10 YSM Barcode--> Timer Function Add
void CModuleInfo::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
//
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	int i = 0;

	switch(nIDEvent)
	{
	case 0:
		m_strMsgDB.SetCaption(ThreadStage.strMsg1);

		for(i = 0; i < 16; i++)
			NowModuleID[i].Format("%s", pView->SerialModuleValue[i]);

		for(int i = 0; i<MAX_NOZZLE; i++)
		{		
			if(Drop_Info.manu_head_job[i])
			{
				m_GridModule.SetTextMatrix(1,MAX_NOZZLE-i,NowModuleID[i]);
			}
		}

		if(ThreadStage.ExitFlag == TRUE)
		{
			GetDlgItem(IDC_CMD_RETURN)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMD_READ)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMD_APPLY)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMD_MODULE_DB_BEFORE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMD_MODULE_DB_MOVE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMD_MODULE_DB_TEACHING)->EnableWindow(TRUE);
			GetDlgItem(IDC_CMD_MODULE_DB_REGISTRY)->EnableWindow(TRUE);

			m_bReadStatus = FALSE;
			KillTimer(0);
		}
			

		break;
	}// end of switch(nIDEvent)

//
	CDialog::OnTimer(nIDEvent);
}//

void CModuleInfo::SetGrid_Initial()
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	int nR,nC;
	CString strText1[14] = {"Index","ID","Amp(V)","Offset","Current","Neg. Offset(V)","Offset","Current","Duty(%)","Offset","Current","First Volt(V)","Offset","Current"};
	CString strText2[17] = {"Index","H16","H15","H14","H13","H12","H11","H10","H9","H8","H7","H6","H5","H4","H3","H2","H1",};

	m_GridModule.SetRows(14);m_GridModule.SetCols(17);

	for(nR = 0 ; nR<14; nR++)
	{
		m_GridModule.SetRowHeight(nR,550);
		m_GridModule.SetTextMatrix(nR,0,strText1[nR]);
		
		for(nC = 0; nC<17; nC++)
		{
			if(nR == 2 || nR == 5 || nR == 8 || nR == 11 || nR == 14)
			{
				m_GridModule.SetRow(nR);m_GridModule.SetCol(nC);
				m_GridModule.SetCellBackColor(LIGHTGRAY);
			}
			else if(nR == 3 || nR == 6 || nR == 9 || nR == 12 || nR == 15)
			{
				m_GridModule.SetRow(nR);m_GridModule.SetCol(nC);
				m_GridModule.SetCellBackColor(WHITEGRAY);
			}
			else if(nR == 4 || nR == 7 || nR == 10 || nR == 13 || nR == 16)
			{
				m_GridModule.SetRow(nR);m_GridModule.SetCol(nC);
				m_GridModule.SetCellBackColor(WHITEGREEN);
			}		
			m_GridModule.SetColWidth(nC,880);
			m_GridModule.SetColAlignment(nC,4);
			m_GridModule.SetTextMatrix(0,nC,strText2[nC]);

		}
	}

	m_GridModule.SetColWidth(0,1130);
	m_GridModule.SetRowHeight(0,850);

	for(nC=0;nC<MAX_NOZZLE;nC++)
	{
		if(pDoc->m_bIsHeadSelected[nC])//2014.12.01 by tskim
		{
			m_GridModule.SetRow(0);m_GridModule.SetCol(MAX_NOZZLE-nC);
			m_GridModule.SetCellBackColor(GREEN);	
			Drop_Info.manu_head_job[nC] = true;
		}
		else
		{
			m_GridModule.SetRow(0);m_GridModule.SetCol(MAX_NOZZLE-nC);
			m_GridModule.SetCellBackColor(WHITE);
			Drop_Info.manu_head_job[nC] = false;	
		}
	}	
}

void CModuleInfo::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
// 	CPen pen;
// 	pen.CreatePen(PS_SOLID,5,DARKBLUE);
// 	CPen *pOldPen = (CPen *)dc.SelectObject(&pen);
// 
// 	dc.MoveTo(0,HEIGHT_YPOS);
// 	dc.LineTo(SCREEN_WIDTH,HEIGHT_YPOS);
// 
// 	dc.SelectObject(pOldPen);

	// Do not call CFormView::OnPaint() for painting messages
}

void CModuleInfo::Get_ModuleInfo(int nModuleID , int nHead , BOOL bInitial)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument(); 

	int nR,nC;
	CString str;

	if (bInitial)
	{
		str.Format("%s",pView->pN_Func->m_structNParam[nHead].m_strModuleID);
		m_GridModule.SetTextMatrix(1,(MAX_NOZZLE-nHead),str);

		str.Format("%d",pDoc->m_structPatternData[0].m_nNAmplitude[nHead]);
		m_GridModule.SetTextMatrix(2,(MAX_NOZZLE-nHead),str);

		str.Format("%d",pView->pN_Func->m_structNParam[nHead].m_nNAmplitude_Offset);
		m_GridModule.SetTextMatrix(3,(MAX_NOZZLE-nHead),str);

		str.Format("%d",pDoc->m_structPatternData[0].m_nNAmplitude[nHead] + pView->pN_Func->m_structNParam[nHead].m_nNAmplitude_Offset);
		m_GridModule.SetTextMatrix(4,(MAX_NOZZLE-nHead),str);

		str.Format("%d",pDoc->m_structPatternData[0].m_nNNegOffset[nHead]);
		m_GridModule.SetTextMatrix(5,(MAX_NOZZLE-nHead),str);

		str.Format("%d",pView->pN_Func->m_structNParam[nHead].m_nNNegOffset_Offset);
		m_GridModule.SetTextMatrix(6,(MAX_NOZZLE-nHead),str);

		str.Format("%d",pDoc->m_structPatternData[0].m_nNNegOffset[nHead] + pView->pN_Func->m_structNParam[nHead].m_nNNegOffset_Offset);
		m_GridModule.SetTextMatrix(7,(MAX_NOZZLE-nHead),str);

		str.Format("%.0f",pDoc->m_structPatternData[0].m_dNDuty[nHead]);
		m_GridModule.SetTextMatrix(8,(MAX_NOZZLE-nHead),str);

		str.Format("%.0f",pView->pN_Func->m_structNParam[nHead].m_dNDuty_Offset);
		m_GridModule.SetTextMatrix(9,(MAX_NOZZLE-nHead),str);

		str.Format("%.0f",pDoc->m_structPatternData[0].m_dNDuty[nHead] + pView->pN_Func->m_structNParam[nHead].m_dNDuty_Offset);
		m_GridModule.SetTextMatrix(10,(MAX_NOZZLE-nHead),str);

		str.Format("%d",pDoc->m_structPatternData[0].m_nNFirstVoltage[nHead]);
		m_GridModule.SetTextMatrix(11,(MAX_NOZZLE-nHead),str);

		str.Format("%d",pView->pN_Func->m_structNParam[nHead].m_nNFirstVoltage_Offset);
		m_GridModule.SetTextMatrix(12,(MAX_NOZZLE-nHead),str);

		str.Format("%d",pDoc->m_structPatternData[0].m_nNFirstVoltage[nHead]  + pView->pN_Func->m_structNParam[nHead].m_nNFirstVoltage_Offset);
		m_GridModule.SetTextMatrix(13,(MAX_NOZZLE-nHead),str);
	}
	else
	{
		str.Format("%s",pView->pN_Func->m_structModuleData_Get[nModuleID-1].strModuleID);
		m_GridModule.SetTextMatrix(1,(MAX_NOZZLE-nHead),str);

		str.Format("%d",pDoc->m_structPatternData[0].m_nNAmplitude[nHead]);
		m_GridModule.SetTextMatrix(2,(MAX_NOZZLE-nHead),str);

		str.Format("%d",pView->pN_Func->m_structModuleData_Get[nModuleID-1].nAmplitude_Offset);
		m_GridModule.SetTextMatrix(3,(MAX_NOZZLE-nHead),str);

		str.Format("%d",pDoc->m_structPatternData[0].m_nNAmplitude[nHead] + pView->pN_Func->m_structModuleData_Get[nModuleID-1].nAmplitude_Offset);
		m_GridModule.SetTextMatrix(4,(MAX_NOZZLE-nHead),str);

		str.Format("%d",pDoc->m_structPatternData[0].m_nNNegOffset[nHead]);
		m_GridModule.SetTextMatrix(5,(MAX_NOZZLE-nHead),str);

		str.Format("%d",pView->pN_Func->m_structModuleData_Get[nModuleID-1].nNegOffset_Offset);
		m_GridModule.SetTextMatrix(6,(MAX_NOZZLE-nHead),str);

		str.Format("%d",pDoc->m_structPatternData[0].m_nNNegOffset[nHead] + pView->pN_Func->m_structModuleData_Get[nModuleID-1].nNegOffset_Offset);
		m_GridModule.SetTextMatrix(7,(MAX_NOZZLE-nHead),str);

		str.Format("%.0f",pDoc->m_structPatternData[0].m_dNDuty[nHead]);
		m_GridModule.SetTextMatrix(8,(MAX_NOZZLE-nHead),str);

		str.Format("%.0f",pView->pN_Func->m_structModuleData_Get[nModuleID-1].nDuty_Offset);
		m_GridModule.SetTextMatrix(9,(MAX_NOZZLE-nHead),str);

		str.Format("%.0f",pDoc->m_structPatternData[0].m_dNDuty[nHead] + pView->pN_Func->m_structModuleData_Get[nModuleID-1].nDuty_Offset);
		m_GridModule.SetTextMatrix(10,(MAX_NOZZLE-nHead),str);

		str.Format("%d",pDoc->m_structPatternData[0].m_nNFirstVoltage[nHead]);
		m_GridModule.SetTextMatrix(11,(MAX_NOZZLE-nHead),str);

		str.Format("%d",pView->pN_Func->m_structModuleData_Get[nModuleID-1].nFirst_Vol_Offset);
		m_GridModule.SetTextMatrix(12,(MAX_NOZZLE-nHead),str);

		str.Format("%d",pDoc->m_structPatternData[0].m_nNFirstVoltage[nHead]  + pView->pN_Func->m_structModuleData_Get[nModuleID-1].nFirst_Vol_Offset);
		m_GridModule.SetTextMatrix(13,(MAX_NOZZLE-nHead),str);		
	}
	
}

BEGIN_EVENTSINK_MAP(CModuleInfo, CDialog)
    //{{AFX_EVENTSINK_MAP(CModuleInfo)
	ON_EVENT(CModuleInfo, IDC_GRID_MODULE_INFO, -600 /* Click */, OnClickGridModuleInfo, VTS_NONE)
	ON_EVENT(CModuleInfo, IDC_CMD_RETURN, -600 /* Click */, OnClickCmdReturn, VTS_NONE)
	ON_EVENT(CModuleInfo, IDC_CMD_MODULE_DB_REGISTRY, -600 /* Click */, OnClickCmdModuleDbRegistry, VTS_NONE)
	ON_EVENT(CModuleInfo, IDC_CMD_READ, -600 /* Click */, OnClickCmdRead, VTS_NONE)
	ON_EVENT(CModuleInfo, IDC_CMD_APPLY, -600 /* Click */, OnClickCmdApply, VTS_NONE)
	ON_EVENT(CModuleInfo, IDC_DB_MODIFY, -600 /* Click */, OnClickDbModify, VTS_NONE)
	ON_EVENT(CModuleInfo, IDC_CMD_MODULE_DB_BEFORE, -600 /* Click */, OnClickCmdModuleDbBefore, VTS_NONE)
	ON_EVENT(CModuleInfo, IDC_CMD_MODULE_DB_MOVE, -601 /* DblClick */, OnDblClickCmdModuleDbMove, VTS_DISPATCH)
	ON_EVENT(CModuleInfo, IDC_CMD_MODULE_DB_TEACHING, -600 /* Click */, OnClickCmdModuleDbTeaching, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CModuleInfo::OnClickGridModuleInfo() 
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	// TODO: Add your control notification handler code here
	int nR, nC;
	int nlength=0,i=0;
	CString strTemp , strMsg = "" , strTempLog = "" , strTempModuleID = "";
	int nMax=0,nMin=0;


	nR = m_GridModule.GetMouseRow();
	nC = m_GridModule.GetMouseCol();


	strModuleID = (char *)(LPCSTR)m_GridModule.GetTextMatrix(nR,nC);
	if ( (strModuleID == "" || strModuleID == "0000" ) && nR > 1 )
	{
		if(pView->m_nLanguage == 0)
		strMsg = "Module ID ∞° ¿‘∑¬ µ«¡ˆ æ æ“Ω¿¥œ¥Ÿ."; 
		else if(pView->m_nLanguage == 1)
		strMsg = "Module ID don't input."; 
		else if(pView->m_nLanguage == 1)
		strMsg = "Module ID ∞° ¿‘∑¬ µ«¡ˆ æ æ“Ω¿¥œ¥Ÿ."; 

		m_strMsgDB.SetCaption(strMsg); 

		pView->pN_Func->Save_N_Log(0,strMsg); 
		return; 
	}

	if(nC == 0) return;

	if ( !pView->bModuleInfoStatus && nR == 1 ) 
	{
		if(pView->m_nLanguage == 0)
		strMsg = "√¯¡§/¡∂¡§ »≠∏Èø°º≠¥¬ Module ID∞° ¿‘∑¬ µ«¡ˆ æ Ω¿¥œ¥Ÿ.\n¿Á∑· ¿‘∑¬√¢ø°º≠ ¡§Ωƒ µÓ∑œ πŸ∂¯¥œ¥Ÿ."; 
		else if(pView->m_nLanguage == 1)
		strMsg = "Can't input Module ID in the Adjust/Cali Screen.\n Register at the Material input Screen.";
		else if(pView->m_nLanguage == 2)
		strMsg = "ˆ¥’·/‡Ô⁄ ˚˛ÿ¸ﬂæ Module IDŸ“ÍÛ‚√Ï˝.\nÓß÷˘‚√Ï˝ÛÎœ¢˝Òÿ–Ô·„“‘Ù◊¡.";


		m_strMsgDB.SetCaption(strMsg); 

		pView->pN_Func->Save_N_Log(0,strMsg); 
		return; 
	}
	
	if(nR != 0 && nR != 1 && nR != 3 && nR != 6 && nR != 9 && nR != 12 && nR != 15) return;

	if(nR == 1)//ID
	{
		nMin = 0;nMax = 9999;  
	}
	else if(nR ==3 || nR ==6 || nR ==12)
	{
		nMin = -10;nMax = 10;	//ehji 140512  	nMin = -150;nMax = 150;
	}
	else
	{
		nMin = -10;nMax = 10;
	}

	if(nR != 0)
		Use_TKg(m_GridModule, nMax, nMin, 15, 250, TRUE, TRUE);

	if(nR == 0)//head select
	{
		m_GridModule.SetRow(0);
		m_GridModule.SetCol(nC);
		if(Drop_Info.manu_head_job[MAX_NOZZLE-nC] == FALSE)
		{
			Drop_Info.manu_head_job[MAX_NOZZLE-nC] = true;
			m_GridModule.SetCellBackColor(GREEN);
		}
		else
		{
			Drop_Info.manu_head_job[MAX_NOZZLE-nC] = false;
			m_GridModule.SetCellBackColor(WHITE);
		}	
	}
	else if(nR == 1)//ID
	{
		strModuleID = (char *)(LPCSTR)m_GridModule.GetTextMatrix(nR,nC);

		nlength = strModuleID.GetLength();
		if(nlength <4)
		{
			for(i=0;i<4-nlength;i++)
			{
				strTemp += "0";
			}
			strTemp = strTemp;
			strModuleID = strTemp + strModuleID;
		}
		else
			strModuleID = strModuleID;	

		for ( int nHeadCount = 0; nHeadCount < MAX_NOZZLE; nHeadCount++ )
		{
			if ( ( MAX_NOZZLE - nC ) != nHeadCount ) 
			{
				strTempModuleID = (char *)(LPCSTR)m_GridModule.GetTextMatrix(nR,MAX_NOZZLE-nHeadCount);

				if ( strModuleID == strTempModuleID)
				{
					if(pView->m_nLanguage == 0)
					strTempLog.Format("HEAD %d , %d ∞° ¡ﬂ∫π µÀ¥œ¥Ÿ.", nHeadCount+1 , ( MAX_NOZZLE - nC )+1);
					else if(pView->m_nLanguage == 1)
					strTempLog.Format("HEAD %d , %d is duplicated.", nHeadCount+1 , ( MAX_NOZZLE - nC )+1);
					else if(pView->m_nLanguage == 2)
					strTempLog.Format("HEAD %d , %d ÒÏ‹‹.", nHeadCount+1 , ( MAX_NOZZLE - nC )+1);

					pView->pN_Func->Save_N_Log(0,strTempLog); 

					m_strMsgDB.SetCaption(strTempLog); 

					strModuleID = ""; 
					m_GridModule.SetTextMatrix(nR,nC,strModuleID);	
					
					return; 
				}
			}
		}

		m_GridModule.SetTextMatrix(nR,nC,strModuleID);	



		m_bReadStatus = FALSE; // ID ¿‘∑¬ Ω√ READ ¿Á¿€æ˜ 

		strTempLog.Format("HEAD %d INPUT ID : %s", (MAX_NOZZLE-nC)+1 , strModuleID);
		pView->pN_Func->Save_N_Log(0,strTempLog); 
	}
	else if(nR ==3)//Amp, Neg.Offset, First Volt
	{
		nAmplitude_DB = atoi((char *)(LPCSTR)m_GridModule.GetTextMatrix(nR,nC));

		strTempLog.Format("HEAD %d INPUT AMP OFFSET : %d", (MAX_NOZZLE-nC)+1 , nAmplitude_DB);
		pView->pN_Func->Save_N_Log(0,strTempLog); 

		m_GridModule.SetCellBackColor(RED);
	}
	else if(nR ==6)//Amp, Neg.Offset, First Volt
	{
		nNegOffset_DB = atoi((char *)(LPCSTR)m_GridModule.GetTextMatrix(nR,nC));

		strTempLog.Format("HEAD %d INPUT NEG OFFSET : %d", (MAX_NOZZLE-nC)+1 , nNegOffset_DB);
		pView->pN_Func->Save_N_Log(0,strTempLog); 

		m_GridModule.SetCellBackColor(RED);
	}
	else if(nR ==9)//Amp, Neg.Offset, First Volt
	{
		nDuty_DB = atof((char *)(LPCSTR)m_GridModule.GetTextMatrix(nR,nC));

		strTempLog.Format("HEAD %d INPUT DUTY OFFSET : %d", (MAX_NOZZLE-nC)+1 , nDuty_DB);
		pView->pN_Func->Save_N_Log(0,strTempLog);

		m_GridModule.SetCellBackColor(RED);
	}
	else if(nR ==12)//Amp, Neg.Offset, First Volt
	{
		nFirst_Vol_DB = atoi((char *)(LPCSTR)m_GridModule.GetTextMatrix(nR,nC));

		strTempLog.Format("HEAD %d INPUT FIRST_VOL OFFSET : %d", (MAX_NOZZLE-nC)+1 , nFirst_Vol_DB);
		pView->pN_Func->Save_N_Log(0,strTempLog); 

		m_GridModule.SetCellBackColor(RED);
	}

}

void CModuleInfo::OnClickCmdReturn() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	CString strMsg = ""; 

	SaveModuleBarTeach();		//ehji 150227

	for ( int nHeadCount  = 0; nHeadCount < MAX_NOZZLE; nHeadCount++ )
	{
		if ( Drop_Info.manu_head_job[nHeadCount] )
		{
			m_GridModule.SetRow(1);m_GridModule.SetCol(MAX_NOZZLE-nHeadCount);
			if ( m_GridModule.GetCellBackColor() == YELLOW || (m_GridModule.GetTextMatrix(1,MAX_NOZZLE-nHeadCount)==""))
			{
				if(pView->m_nLanguage == 0)
				strMsg = 
					"Module ID ∞° ¡ˆ¡§µ«¡ˆ æ ¿∫ Head ∞° ¿÷Ω¿¥œ¥Ÿ.≥™∞°Ω√∞⁄Ω¿¥œ±Ó???\n\n∫Ø∞Êµ» Offset∞™¿∫ ¿˚øÎµ«¡ˆ æ Ω¿¥œ¥Ÿ.        ";
				else if(pView->m_nLanguage == 1)
				strMsg = 
					"There is Head that Module ID don't Appoint. Will you go back to Screen Before???\n\nChanged Offset don't apply .        ";
				else if(pView->m_nLanguage == 2)
				strMsg = 
					"Module ID ÍÛŸÌÚ¶Ô“Ó‹Head.‚ÕÈ©ıÛÀ€ÿß???\n\n‹®Ã⁄Ó‹Offset‹Ù¸ÂÎÎÈƒ ";

				if(AfxMessageBox(strMsg,MB_SYSTEMMODAL|MB_YESNO)== IDYES)
				{
					EndDialog(IDOK);
					return; 
				}
				else
				{
					if(pView->m_nLanguage == 0)
					strMsg = "Module ID ∞° ¡ˆ¡§µ«¡ˆ æ ¿∫ Head ∞° ¿÷Ω¿¥œ¥Ÿ. \nΩ≈±‘ µÓ∑œ «œ∞≈≥™ π¯»£ »Æ¿Œ «œººø‰. ";
					else if(pView->m_nLanguage == 0)
					strMsg = "There is Head that Module ID don't Appoint. \n  register new Number or Check Number . "; 
					else if(pView->m_nLanguage == 0)
					strMsg = "ÍÛÌÓ§Module ID ŸÌÚ¶Ô“Ó‹Head. \n„ÊÀÔ‘Ù◊¡˚‰Ì∫¸¨Ï„¯∫˚‹. "; 


					m_strMsgDB.SetCaption(strMsg); 

					pView->pN_Func->Save_N_Log(0,strMsg); 
					return;
				} 
			}
		}
	}

	pView->pN_Func->Read_Module_DB(pDoc->m_structOperatorModeRecipeData.strFrontRecipeName); 

	CaculateOffset(); 

	EndDialog(IDOK);	
}

void CModuleInfo::OnClickCmdModuleDbRegistry() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	
	CString strTempLog = ""; 

	strTempLog = "DATA READ CLICK!!";
	pView->pN_Func->Save_N_Log(0,strTempLog); 

	CModuleDBRegistry dlg;
	dlg.DoModal();
}

void CModuleInfo::OnClickCmdRead() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	// TODO: Add your control notification handler code here
	int i , nModuleID = -1;
	CString strMsg = "" , strTempMsg = "" , strTempLog = ""; 
	BOOL bModuleIDStatus = TRUE; 

	CNormalMsg dlg; 

	dlg.m_strTitle = _T("CHECK");

	if(pView->m_nLanguage == 0)
	{
		dlg.m_strMsg1 = " HEAD ∫∞ MODULE π¯»£∏¶ »Æ¿Œ«œººø‰.";
		dlg.m_strMsg2 = " ∞Ëº” ¡¯«‡ «œΩ√∑¡∏È OK ∏¶ ¥≠∑Ø¡÷ººø‰. ";
	}
	else if(pView->m_nLanguage == 1)
	{
		dlg.m_strMsg1 = " Check Number MODULE by Head."; 
		dlg.m_strMsg2 = " if you will Continue, Push Button 'OK'";
	}
	else if(pView->m_nLanguage == 2)
	{
		dlg.m_strMsg1 = "  ¸¨Ï„ ¿HEADÓ‹MODULE¯∫˚‹.";
		dlg.m_strMsg2 = " Õ©·Ÿ„˘˙ºÔ«Ã™OK. ";
	}

	if( dlg.DoModal() != IDOK )
	{
		dlg.DoModal();
		return;
	}

	strTempLog = "DATA READ CLICK!!";
	pView->pN_Func->Save_N_Log(0,strTempLog); 
	
	for(i=0;i<MAX_NOZZLE;i++)
	{
		if(Drop_Info.manu_head_job[i])
		{
			nModuleID = atoi ((char *)(LPCSTR)m_GridModule.GetTextMatrix(1,MAX_NOZZLE-i));

			NowModuleID[i] = m_GridModule.GetTextMatrix(1,MAX_NOZZLE-i);

			if( nModuleID != 0 )
			{
				if (   pView->pN_Func->m_structModuleData_Get[nModuleID-1].strModuleID != "0000"
					&& pView->pN_Func->m_structModuleData_Get[nModuleID-1].strModuleID != ""    )
				{
					
					Get_ModuleInfo(nModuleID,i,FALSE);

					m_GridModule.SetRow(1);m_GridModule.SetCol(MAX_NOZZLE-i);
					m_GridModule.SetCellBackColor(WHITE);

					m_bReadStatus = TRUE; 
				}
				else
				{
					strTempMsg.Format("H%d , " , i+1); 
					strMsg += strTempMsg; 
					m_bReadStatus = bModuleIDStatus = FALSE; 


					m_GridModule.SetRow(1);m_GridModule.SetCol(MAX_NOZZLE-i);
					m_GridModule.SetCellBackColor(YELLOW);

					m_GridModule.SetTextMatrix(2,(MAX_NOZZLE-i),"00");
					m_GridModule.SetTextMatrix(3,(MAX_NOZZLE-i),"00");
					m_GridModule.SetTextMatrix(4,(MAX_NOZZLE-i),"00");
					m_GridModule.SetTextMatrix(5,(MAX_NOZZLE-i),"00");
					m_GridModule.SetTextMatrix(6,(MAX_NOZZLE-i),"00");
					m_GridModule.SetTextMatrix(7,(MAX_NOZZLE-i),"00");
					m_GridModule.SetTextMatrix(8,(MAX_NOZZLE-i),"00");
					m_GridModule.SetTextMatrix(9,(MAX_NOZZLE-i),"00");
					m_GridModule.SetTextMatrix(10,(MAX_NOZZLE-i),"00");
					m_GridModule.SetTextMatrix(11,(MAX_NOZZLE-i),"00");
					m_GridModule.SetTextMatrix(12,(MAX_NOZZLE-i),"00");
					m_GridModule.SetTextMatrix(13,(MAX_NOZZLE-i),"00");
				}
			}
			else
			{
				strTempMsg.Format("H%d , " , i+1); 
				strMsg += strTempMsg; 
				m_bReadStatus = bModuleIDStatus = FALSE; 

				m_GridModule.SetRow(1);m_GridModule.SetCol(MAX_NOZZLE-i);
				m_GridModule.SetCellBackColor(YELLOW);
			}
		}
	}

	if(pView->m_nLanguage == 0)
	{
		if (!bModuleIDStatus) strMsg += "Module ID ∞° æ¯Ω¿¥œ¥Ÿ.\n¿‘∑¬ ∂«¥¬ Ω≈±‘ µÓ∑œ «œººø‰.";
		else				  strMsg = "Module ID ∫∞ Data Read ∞° øœ∑· µ«æ˙Ω¿¥œ¥Ÿ."; 
	}
	else if(pView->m_nLanguage == 1)
	{
		if (!bModuleIDStatus) strMsg += "Module ID don't exist .\n Input or regist new Number.";
		else				  strMsg = "Module ID & Data Read are Completed."; 
	}
	else if(pView->m_nLanguage == 2)
	{
		if (!bModuleIDStatus) strMsg += "ŸÌModule ID.\n‚√Ï˝˚‰Ì∫„ÊÀÔ‘Ù◊¡.";
		else				  strMsg = " ¿Module ID  Data ReadË«÷ı."; 
	}

	pView->pN_Func->Save_N_Log(0,strMsg); 
	m_strMsgDB.SetCaption(strMsg); 

}

void CModuleInfo::OnClickCmdApply() 
{
		// TODO: Add your control notification handler code her
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	
	
	int nModuleID = 0; 
	CString strMsg = "",strTempMsg = "";
	BOOL bModuleIDStatus = FALSE; 

	if ( !m_bReadStatus )
	{
		if(pView->m_nLanguage ==0)
		strMsg = " Module ID Read ∏¶ «œ¡ˆ æ æ“Ω¿¥œ¥Ÿ."; 
		else if(pView->m_nLanguage == 1)
		strMsg = " Did not Read Module ID."; 
		else if(pView->m_nLanguage == 2)
		strMsg = " Ÿ“ÍÛModule ID Read."; 

		m_strMsgDB.SetCaption(strMsg); 
		
		pView->pN_Func->Save_N_Log(0,strMsg); 
		return;
	}


	strMsg.Empty(); 
	for ( int i = 0; i < MAX_NOZZLE; i++ )
	{
		if(Drop_Info.manu_head_job[i])
		{
			nModuleID = atoi((char *)(LPCSTR)m_GridModule.GetTextMatrix(1,MAX_NOZZLE-i)); 

			if ( nModuleID == 0 || (pView->pN_Func->m_structModuleData_Get[nModuleID-1].strModuleID==""))
			{
				m_GridModule.SetRow(1);m_GridModule.SetCol(MAX_NOZZLE-i);
				m_GridModule.SetCellBackColor(YELLOW); 

				bModuleIDStatus = TRUE; 

				strTempMsg.Format("H %d, ",i+1); 
				strMsg += strTempMsg; 
			}
		}
	}

	if ( bModuleIDStatus )
	{
		if(pView->m_nLanguage == 0)
		strMsg += " Module ID ¡§∫∏∞° æ¯Ω¿¥œ¥Ÿ.";
		else if(pView->m_nLanguage == 0)
		strMsg += " Module ID don't exist.";
		else if(pView->m_nLanguage == 0)
		strMsg += " Module ID ¡§∫∏∞° æ¯Ω¿¥œ¥Ÿ.";

		m_strMsgDB.SetCaption(strMsg); 
		
		pView->pN_Func->Save_N_Log(0,strMsg); 
		return;
	}

	CNormalMsg dlg; 

	dlg.m_strTitle = _T("CHECK");

	if(pView->m_nLanguage == 0)
	{
		dlg.m_strMsg1 = " »≠∏Èø° «•Ω√µ» OFFSET ∞™¿Ã ¿˙¿Â µÀ¥œ¥Ÿ. ";
		dlg.m_strMsg2 = " ¿˙¿Â «œΩ√∞⁄Ω¿¥œ±Ó?? YES ->  OK , NO -> CANCEL " ;
	}
	else if(pView->m_nLanguage == 1)
	{
		dlg.m_strMsg1 = " OFFSET Value indicatied in Screen Will Save. ";
		dlg.m_strMsg2 = " Will you Save?? YES ->  OK , NO -> CANCEL " ;
	}
	else if(pView->m_nLanguage == 2)
	{
		dlg.m_strMsg1 = " ‹¡Ì˚˛ÿ¸¯˙„∆Ó‹OFFSET?. ";
		dlg.m_strMsg2 = " ‹¡Ì?? YES ->  OK , NO -> CANCEL " ;
	}

	if( dlg.DoModal() != IDOK )
	{
		dlg.DoModal();
		return;
	}

	for( i = 0; i < MAX_NOZZLE; i++ )
	{
		if(Drop_Info.manu_head_job[i])
		{
			nModuleID = atoi((char *)(LPCSTR)m_GridModule.GetTextMatrix(1,MAX_NOZZLE-i)); 
			
			if ( nModuleID != 0 )
			{
				pView->pN_Func->m_structModuleData_Set[nModuleID-1].strModuleID	
								= (char *)(LPCSTR)m_GridModule.GetTextMatrix(1 ,MAX_NOZZLE-i);
				pView->pN_Func->m_structModuleData_Set[nModuleID-1].nAmplitude_Offset
								= atoi((char *)(LPCSTR)m_GridModule.GetTextMatrix(3 ,MAX_NOZZLE-i));
				pView->pN_Func->m_structModuleData_Set[nModuleID-1].nNegOffset_Offset
								= atoi((char *)(LPCSTR)m_GridModule.GetTextMatrix(6 ,MAX_NOZZLE-i));
				pView->pN_Func->m_structModuleData_Set[nModuleID-1].nDuty_Offset
								= atoi((char *)(LPCSTR)m_GridModule.GetTextMatrix(9 ,MAX_NOZZLE-i));
				pView->pN_Func->m_structModuleData_Set[nModuleID-1].nFirst_Vol_Offset
								= atoi((char *)(LPCSTR)m_GridModule.GetTextMatrix(12,MAX_NOZZLE-i));

				pView->pN_Func->m_structNParam[i].m_strModuleID = 
					pView->pN_Func->m_structModuleData_Set[nModuleID-1].strModuleID; 
				pView->pN_Func->m_structNParam[i].m_nNAmplitude_Offset = 
					pView->pN_Func->m_structModuleData_Set[nModuleID-1].nAmplitude_Offset; 
				pView->pN_Func->m_structNParam[i].m_dNDuty_Offset = 
					pView->pN_Func->m_structModuleData_Set[nModuleID-1].nDuty_Offset; 
				pView->pN_Func->m_structNParam[i].m_nNNegOffset_Offset = 
					pView->pN_Func->m_structModuleData_Set[nModuleID-1].nNegOffset_Offset; 
				pView->pN_Func->m_structNParam[i].m_nNFirstVoltage_Offset = 
					pView->pN_Func->m_structModuleData_Set[nModuleID-1].nFirst_Vol_Offset;

				Get_ModuleInfo(atoi(pView->pN_Func->m_structNParam[i].m_strModuleID),i,TRUE); 
			}

		}
	}

	pView->pN_Func->Save_Module_DB(pDoc->m_structOperatorModeRecipeData.strFrontRecipeName);

	Sleep(300); 

	pView->pN_Func->Read_Module_DB(pDoc->m_structOperatorModeRecipeData.strFrontRecipeName);

	int nR , nC; 

	for(nR = 0 ; nR < 14; nR++)
	{	
		for(nC = 0; nC < 17; nC++)
		{
			if(nR == 2 || nR == 5 || nR == 8 || nR == 11 || nR == 14)
			{
				m_GridModule.SetRow(nR);m_GridModule.SetCol(nC);
				m_GridModule.SetCellBackColor(LIGHTGRAY);
			}
			else if(nR == 3 || nR == 6 || nR == 9 || nR == 12 || nR == 15)
			{
				m_GridModule.SetRow(nR);m_GridModule.SetCol(nC);
				m_GridModule.SetCellBackColor(WHITEGRAY);
			}
			else if(nR == 4 || nR == 7 || nR == 10 || nR == 13 || nR == 16)
			{
				m_GridModule.SetRow(nR);m_GridModule.SetCol(nC);
				m_GridModule.SetCellBackColor(WHITEGREEN);
			}
		}
	}

	CaculateOffset();

	for( i = 0; i < MAX_NOZZLE; i++ )
	{
		if(Drop_Info.manu_head_job[i])
		{
			nModuleID = atoi((char *)(LPCSTR)m_GridModule.GetTextMatrix(1,MAX_NOZZLE-i)); 
			
			if ( nModuleID != 0 )
			{
				if ( pView->bModuleInfoStatus )
					pView->SaveDB_HistoryLog(nModuleID,i,DB_MATERIAL_REGISTRY_CLB,NULL,"");
				else 
					pView->SaveDB_HistoryLog(nModuleID,i,DB_MATERIAL_REGISTRY_MANUAL_MEAS,NULL,"");
			}
		}

		pDoc->m_strPumpNo[i] = NowModuleID[i];		//20160321	YSM	(PumpNo = ModuleID)
	}
	
	if(pView->m_nLanguage ==0)
	strMsg = "Module No. π◊ Offset ¿˚øÎ¿Ã øœ∑· µ«æ˙Ω¿¥œ¥Ÿ."; 
	else if(pView->m_nLanguage ==1)
	strMsg = "Module No. & Offset Application are completed."; 
	else if(pView->m_nLanguage == 2)
	strMsg = "Module No. –‡ Offset ÎÎÈƒË«÷ı."; 

	pView->pN_Func->Save_N_Log(0,strMsg); 
	m_strMsgDB.SetCaption(strMsg); 

	Save_Module_Before(pDoc->m_structOperatorModeRecipeData.strFrontRecipeName);
}

void CModuleInfo::OnClickDbModify() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	
	int nModuleID = 0; 

	for(int i=0;i<MAX_NOZZLE;i++)
	{
		if(Drop_Info.manu_head_job[i])
		{
			nModuleID = atoi((char *)(LPCSTR)m_GridModule.GetTextMatrix(1,MAX_NOZZLE-i)); 

			pView->pN_Func->m_structModuleData_Set[nModuleID-1].strModuleID	
							= (char *)(LPCSTR)m_GridModule.GetTextMatrix(1 ,MAX_NOZZLE-i);
			pView->pN_Func->m_structModuleData_Set[nModuleID-1].nAmplitude_Offset
							= atoi((char *)(LPCSTR)m_GridModule.GetTextMatrix(3 ,MAX_NOZZLE-i));
			pView->pN_Func->m_structModuleData_Set[nModuleID-1].nNegOffset_Offset
							= atoi((char *)(LPCSTR)m_GridModule.GetTextMatrix(6 ,MAX_NOZZLE-i));
			pView->pN_Func->m_structModuleData_Set[nModuleID-1].nDuty_Offset
							= atoi((char *)(LPCSTR)m_GridModule.GetTextMatrix(9 ,MAX_NOZZLE-i));
			pView->pN_Func->m_structModuleData_Set[nModuleID-1].nFirst_Vol_Offset
							= atoi((char *)(LPCSTR)m_GridModule.GetTextMatrix(12,MAX_NOZZLE-i));

			pView->pN_Func->Save_Module_DB(pDoc->m_structOperatorModeRecipeData.strFrontRecipeName);
		}
	}	
}

void CModuleInfo::CaculateOffset()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	CString strTempLog = ""; 
	
	for (int nPatternCount = 0; nPatternCount < pDoc->m_structDataEditor.m_nPatternNum; nPatternCount++)
	{
		for( int i = 0; i < MAX_NOZZLE; i++ )
		{
			if(Drop_Info.manu_head_job[i])
			{
				pView->pN_Func->m_structNParam[i].m_nNAmplitude[nPatternCount] = pDoc->m_structPatternData[nPatternCount].m_nNAmplitude[i] 
																+ pView->pN_Func->m_structNParam[i].m_nNAmplitude_Offset; 
			
				pView->pN_Func->m_structNParam[i].m_dNDuty[nPatternCount] = pDoc->m_structPatternData[nPatternCount].m_dNDuty[i]
																+ pView->pN_Func->m_structNParam[i].m_dNDuty_Offset; 

				pView->pN_Func->m_structNParam[i].m_nNNegOffset[nPatternCount] = pDoc->m_structPatternData[nPatternCount].m_nNNegOffset[i]
																+ pView->pN_Func->m_structNParam[i].m_nNNegOffset_Offset; 

				pView->pN_Func->m_structNParam[i].m_nNFirstVoltage[nPatternCount] = pDoc->m_structPatternData[nPatternCount].m_nNFirstVoltage[i]
																+ pView->pN_Func->m_structNParam[i].m_nNFirstVoltage_Offset;

				strTempLog.Format("PATTERN #%d , HEAD #%d , MODULE ID %s : AMP ( %d / %d / %d ) , DUTY ( %f / %f / %f ) , NEG_OFFSET( %d / %d / %d ) , FIRST_VOL( %d / %d / %d )"
					, nPatternCount+1 , i+1 , pView->pN_Func->m_structNParam[i].m_strModuleID
					, pDoc->m_structPatternData[nPatternCount].m_nNAmplitude[i] , pView->pN_Func->m_structNParam[i].m_nNAmplitude_Offset , pView->pN_Func->m_structNParam[i].m_nNAmplitude[nPatternCount] 
					, pDoc->m_structPatternData[nPatternCount].m_dNDuty[i]      , pView->pN_Func->m_structNParam[i].m_dNDuty_Offset      , pView->pN_Func->m_structNParam[i].m_dNDuty[nPatternCount]
					, pDoc->m_structPatternData[nPatternCount].m_nNNegOffset[i] , pView->pN_Func->m_structNParam[i].m_nNNegOffset_Offset , pView->pN_Func->m_structNParam[i].m_nNNegOffset[nPatternCount]
					, pDoc->m_structPatternData[nPatternCount].m_nNFirstVoltage[i] , pView->pN_Func->m_structNParam[i].m_nNFirstVoltage_Offset , pView->pN_Func->m_structNParam[i].m_nNFirstVoltage[nPatternCount]);

				pView->pN_Func->Save_N_Log(0,strTempLog); 
				
//2014.12.01 by tskim Amp & Duty∞™ ¿ÃªÛ ø°∑Ø...0¿œ ∞ÊøÏ ∞«¥Ÿ..
				if(pView->pN_Func->m_structNParam[i].m_nNAmplitude[nPatternCount] == 0 || pView->pN_Func->m_structNParam[i].m_dNDuty[nPatternCount] == 0.0)
				{
					pView->PostMessage(WM_ERROR,800+i,NULL);							
					break;                                               
				}
			}
		}
	}
}

void CModuleInfo::SelectLanguage()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(pView->m_nLanguage == 0)
	{
		SetDlgItemText(IDC_CMD_MODULE_DB_REGISTRY,				_T("Ω≈±‘ µÓ∑œ"));
		SetDlgItemText(IDC_CMD_RETURN,				_T("¿Ã¿¸ »≠∏È"));
		SetDlgItemText(IDC_CMD_MODULE_DB_BEFORE,		_T("¿Ã¿¸∞™ ∫“∑Øø¿±‚"));
	}
	else if(pView->m_nLanguage == 1)
	{
		SetDlgItemText(IDC_CMD_MODULE_DB_REGISTRY,				_T("New Record"));
		SetDlgItemText(IDC_CMD_RETURN,				_T("Before"));
		SetDlgItemText(IDC_CMD_MODULE_DB_BEFORE,		_T("Before Value Read"));
	}
	else if(pView->m_nLanguage == 2)
	{
		SetDlgItemText(IDC_CMD_MODULE_DB_REGISTRY,				_T("„Ê‘Ù÷‚"));
		SetDlgItemText(IDC_CMD_RETURN,				_T("⁄˜¸ﬁ"));
		SetDlgItemText(IDC_CMD_MODULE_DB_BEFORE,		_T("Before Value Read"));
	}
}

void CModuleInfo::OnClickCmdModuleDbBefore() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	CString strMsg;

//	CString NowModuleID_Load[MAX_NOZZLE];
	
	Load_Module_Before(pDoc->m_structOperatorModeRecipeData.strFrontRecipeName);

	for(int i = 0; i<MAX_NOZZLE; i++)
	{		
		if(Drop_Info.manu_head_job[i])
		{
			m_GridModule.SetTextMatrix(1,MAX_NOZZLE-i,NowModuleID[i]);
		}
	}

	strMsg.Empty();
	strMsg ="∏µ‚ ¿Ã¿¸∞™ ∫“∑Øø¿±‚ ≈¨∏Ø";
	pView->pN_Func->Save_N_Log(0,strMsg); 
}

void CModuleInfo::Save_Module_Before(CString strRecipeNo)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	
	CString strPath;
	FILE *fp;
	CString strDataPath = _T("D:\\TOP\\P8CA_LC\\N_MODULE_DB");

	strPath.Format("%s\\%s.dat", strDataPath, strRecipeNo);

	fp = fopen(strPath, "wt");

	for(int i = 0; i<MAX_NOZZLE; i++)
	{
		if(Drop_Info.manu_head_job[i])
		{
			fprintf(fp, "%s\n", NowModuleID[i] );
		}
		else
		{
			NowModuleID[i] = "0";
			fprintf(fp, "%s\n", NowModuleID[i] );
		}
	}
	fclose(fp);
}

void CModuleInfo::Load_Module_Before(CString strRecipeNo)
{

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
//#if PC_TYPE
/*	CString strPath;
	FILE *fp;
	CString strDataPath = _T("D:\\TOP\\P8CA_LC\\N_MODULE_DB");

	strPath.Format("%s\\%s.dat", strDataPath, strRecipeNo);

	fp = fopen(strPath, "r");


	for(int i = 0; i<MAX_NOZZLE; i++)
	{
		if(Drop_Info.manu_head_job[i])
		{
			fscanf(fp, "%s\n", &NowModuleID[i]);	
		}
	}

fclose(fp);*/	
//#endif
//m_GridModule.SetTextMatrix(1,MAX_NOZZLE,NowModuleID[i])
//m_GridModule.SetRow(1);m_GridModule.SetCol(MAX_NOZZLE-i)

	ifstream fi;
	char ch[256] = {0,0,0,0,};	

//	ch[256] = {0,0,};


//		CString NowModuleID[MAX_NOZZLE];
	CString strDataPath ;

	strDataPath.Format("D:\\TOP\\P8CA_LC\\N_MODULE_DB\\%s.dat", strRecipeNo);


	// »≠¿œ ø≠±‚
	fi.open((char *)(LPCSTR)strDataPath, ios::nocreate/*in*/);
	
	// »≠¿œ ¿–±‚
	if(!fi.is_open())
	{
		fi.close();
		return;
	}

	else
	{
		for(int i = 0; i<MAX_NOZZLE; i++)
		{
			fi >> ch;
			NowModuleID[i].Format("%s", ch);
		}
	}

	fi.close();
}



void CModuleInfo::OnDblClickCmdModuleDbMove(LPDISPATCH Cancel) 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	CNormalMsg dlg; 

	dlg.m_strTitle = _T("CHECK");

	if(pView->m_nLanguage == 0)
	{
		dlg.m_strMsg1 = " ¡÷¿««œººø‰. BarCode Machine¿ª ¿ÃøÎ«œø© ¿⁄µø¿∏∑Œ MODULE π¯»£∏¶ ¿‘∑¬«’¥œ¥Ÿ.";
		dlg.m_strMsg2 = " ∞Ëº” ¡¯«‡ «œΩ√∑¡∏È OK ∏¶ ¥≠∑Ø¡÷ººø‰. ";
	}
	else if(pView->m_nLanguage == 1)
	{
		dlg.m_strMsg1 = " Becareful. Input MODULE NUMBER Useing BarCode Machine "; 
		dlg.m_strMsg2 = " if you will Continue, Push Button 'OK'";
	}
	else if(pView->m_nLanguage == 2)
	{
		dlg.m_strMsg1 = " Becareful. Input MODULE NUMBER Useing BarCode Machine "; 
		dlg.m_strMsg2 = " Õ©·Ÿ„˘˙ºÔ«Ã™OK. ";
	}

	if( dlg.DoModal() != IDOK )
	{
		return;
	}


	// ø¨∞·.

	//m_ctrlReturn.EnableWindow(false);
//	ThreadStage.ExitFlag = true;


	for(int i = 0; i<MAX_NOZZLE; i++)
	{		
		NowModuleID[i] = "";
		
		m_GridModule.SetTextMatrix(1,MAX_NOZZLE-i," ");

		pView->SerialModuleValue[i] = "0";
	}


	GetDlgItem(IDC_CMD_RETURN)->EnableWindow(FALSE);
	GetDlgItem(IDC_CMD_READ)->EnableWindow(FALSE);
	GetDlgItem(IDC_CMD_APPLY)->EnableWindow(FALSE);
	GetDlgItem(IDC_CMD_MODULE_DB_BEFORE)->EnableWindow(FALSE);
	GetDlgItem(IDC_CMD_MODULE_DB_MOVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_CMD_MODULE_DB_TEACHING)->EnableWindow(FALSE);
	GetDlgItem(IDC_CMD_MODULE_DB_REGISTRY)->EnableWindow(FALSE);

	//2015.11.10 YSM Barcode
	SetTimer(0,100,NULL);//

	ThreadStage.ManualJobStep = 0;
	ThreadStage.ManualCode='T';
	ThreadStage.JobFlag = STAGE_MANUAL;
	pView->RunThread(THREAD_STAGE);
	return;
}

void CModuleInfo::OnClickCmdModuleDbTeaching() 
{
	// TODO: Add your control notification handler code here
	CModuleBarTeaching dlg;
	dlg.DoModal();
}

void CModuleInfo::SaveModuleBarTeach()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	CString strPath;
	FILE *fp;
	CString strDataPath = _T("D:\\TOP\\P8CA_LC\\P8CA_LC_DATA");

	strPath.Format("%s\\SaveModuleBarcodeTeach.dat", strDataPath);

	fp = fopen(strPath, "wt");

	for(int i = 0; i < (MAX_NOZZLE+2); i++)		// col, offset ∞™ ¿˙¿Â«ÿæﬂ µ . 
	{
		fprintf(fp, "%.3f\n", pView->ModuleTeachHead[i]);
	}

	fclose(fp);
}

void CModuleInfo::LoadModuleBarTeach()
{
#if PC_TYPE

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	ifstream fi;
	CString strDataPath = _T("D:\\TOP\\P8CA_LC\\P8CA_LC_DATA");
	CString strPathName;
	// Data Editor »≠¿œ ¿–±‚ //////////////////////////////////////////////////////
	// »≠¿œ∏Ì º≥¡§
	strPathName.Format("%s\\SaveModuleBarcodeTeach.dat", strDataPath);
	
	// »≠¿œ ø≠±‚

	fi.open((char *)(LPCSTR)strPathName, ios::nocreate/*in*/);
	
	// »≠¿œ ¿–±‚
	if(!fi.is_open())
	{
		fi.close();
		return;
	}
	else
	{
		for (int i = 0; i < (MAX_NOZZLE+2); i++)
		{
			fi >> pView->ModuleTeachHead[i];
			if(i<MAX_NOZZLE)
				pDoc->m_strPumpNo[i] = NowModuleID[i];		//20160321	YSM	(PumpNo = ModuleID)
		}
		
	}

#endif
}

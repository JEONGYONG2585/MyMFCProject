// SettingAssemble.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "SettingAssemble.h"

#include "MainFrm.h"
#include "P8CA_LcDispDoc.h"
#include "P8CA_LcDispView.h"
#include "NormalMsg.h"
//#include "DevicePart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSettingAssemble dialog
extern DROPINFO	Drop_Info;
CSettingAssemble::CSettingAssemble(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingAssemble::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSettingAssemble)
	//}}AFX_DATA_INIT

	m_nLoopCount = 0;
}


void CSettingAssemble::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingAssemble)
	DDX_Control(pDX, IDC_MSFLEXGRID_SETTING, m_ctrlSettingGrid);
	DDX_Control(pDX, IDC_MSFLEXGRID_NOZZLE_CLEAN, m_ctrlNozzleCleanGrid);
	DDX_Control(pDX, IDC_GRID_CONDITION, m_GridCondition);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingAssemble, CDialog)
	//{{AFX_MSG_MAP(CSettingAssemble)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingAssemble message handlers

BEGIN_EVENTSINK_MAP(CSettingAssemble, CDialog)
    //{{AFX_EVENTSINK_MAP(CSettingAssemble)
	ON_EVENT(CSettingAssemble, IDC_CMD_RETURN, -600 /* Click */, OnClickCmdReturn, VTS_NONE)
	ON_EVENT(CSettingAssemble, IDC_MSFLEXGRID_SETTING, -600 /* Click */, OnClickMsflexgridSetting, VTS_NONE)
	ON_EVENT(CSettingAssemble, IDC_CMD_PUMP_POSITION_SAVE, -600 /* Click */, OnClickCmdPumpPositionSave, VTS_NONE)
	ON_EVENT(CSettingAssemble, IDC_GRID_CONDITION, -600 /* Click */, OnClickGridCondition, VTS_NONE)
	ON_EVENT(CSettingAssemble, IDC_MSFLEXGRID_NOZZLE_CLEAN, -600 /* Click */, OnClickMsflexgridNozzleClean, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void CSettingAssemble::OnClickCmdReturn() 
{
	// TODO: Add your control notification handler code here
	// spd data save//2006/09/11//
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	int i = 0;

/*	CNormalMsg dlg;
	dlg.m_strTitle = "CONFIRM";
	dlg.m_strMsg1 = "변경된 데이터를 저장 할까요?";
	dlg.m_strMsg2 = "실행을 원하시면  '확인'button, 그렇지 않으면 '취소' button 을 누르세요.";
	if(dlg.DoModal() == IDOK)
	{
		for(i =0 ; i <m_ctrlSettingGrid.GetRows() -1; i++)
		{
			Drop_Info.m_dAssemPos[i] = atof((char *)(LPCSTR)m_ctrlSettingGrid.GetTextMatrix(i+1, 1));
			Drop_Info.m_dBasePos[i] = atof((char *)(LPCSTR)m_ctrlSettingGrid.GetTextMatrix(i+1, 2));
			Drop_Info.m_dAdjustAbsPos[i] = atof((char *)(LPCSTR)m_ctrlSettingGrid.GetTextMatrix(i+1, 3));
			Drop_Info.m_dFullBasePos[i] = atof((char *)(LPCSTR)m_ctrlSettingGrid.GetTextMatrix(i+1, 4));
		}
		pDoc->SaveSPDData();
	}
*/	KillTimer(0);

	CDialog::EndDialog(IDOK);	
}

BOOL CSettingAssemble::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	UpdateData(FALSE);

	SetSettingMSFlexGrid();
	//by shin//2013.01.16//n-type nozzle clean step parameter add..//
	SetNozzleCleanGrid();
	//Supply & Degas Condition
	SetSupplyDegasCondition();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSettingAssemble::SetSettingMSFlexGrid()
{
	CString str;
	int i;

	m_ctrlSettingGrid.SetRows(MAX_NOZZLE+1);
	m_ctrlSettingGrid.SetCols(6);

	for(i=0; i<6; i++)
	{
		if(0==i)
			m_ctrlSettingGrid.SetColWidth(i, 500);
		else
			m_ctrlSettingGrid.SetColWidth(i, 1300);

		m_ctrlSettingGrid.SetColAlignment(i, 4);
	}
	m_ctrlSettingGrid.SetRowHeight(0, 650);
	for(i=1; i<MAX_NOZZLE+1; i++)
	{
		m_ctrlSettingGrid.SetRowHeight(i, 450);
	}

	m_ctrlSettingGrid.SetTextMatrix(0, 1, "Assemble Position");
	m_ctrlSettingGrid.SetTextMatrix(0, 2, "Base Position");
	m_ctrlSettingGrid.SetTextMatrix(0, 3, "Adjust Position");
	m_ctrlSettingGrid.SetTextMatrix(0, 4, "Full Base Position");
	m_ctrlSettingGrid.SetTextMatrix(0, 5, "Suction Offset");

	for(i=0; i<MAX_NOZZLE; i++)
	{
		str.Format("H%d", i+1);
		m_ctrlSettingGrid.SetTextMatrix(i+1, 0, str);

		str.Format("%.0f", Drop_Info.m_dAssemPos[i]);
		m_ctrlSettingGrid.SetTextMatrix(i+1, 1, str);
		
		str.Format("%.0f", Drop_Info.m_dBasePos[i]);
		m_ctrlSettingGrid.SetTextMatrix(i+1, 2, str);

		str.Format("%.0f", Drop_Info.m_dAdjustAbsPos[i]);
		m_ctrlSettingGrid.SetTextMatrix(i+1, 3, str);

		str.Format("%.0f", Drop_Info.m_dFullBasePos[i]);
		m_ctrlSettingGrid.SetTextMatrix(i+1, 4, str);

		str.Format("%d", Drop_Info.m_nSuctionOffset[i]);
		m_ctrlSettingGrid.SetTextMatrix(i+1, 5, str);
	}
}

void CSettingAssemble::OnClickMsflexgridSetting() 
{
	// TODO: Add your control notification handler code here
	int row, col;
	double m_dMax, m_dMin;
	
	row = m_ctrlSettingGrid.GetMouseRow();
	col = m_ctrlSettingGrid.GetMouseCol();

	if(0==row || 0==col) return;

	switch(col)
	{
	case 1:	// AssemPos
		m_dMin = 10000;
		m_dMax = 260000;
		break;
	case 2:	// BasePos
		m_dMin = 20000;
		m_dMax = 350000;
		break;
	case 3:	// AdjustPos
		m_dMin = 0;
		m_dMax = 260000;
		break;
	case 4:	// Full BasePos
		m_dMin = 10000;
		m_dMax = 260000;
		break;
	case 5:	// Suction Offset
		m_dMin = -1000;
		m_dMax = 1000;
		break;
	}

	Use_TKg(m_ctrlSettingGrid, m_dMax, m_dMin, 512, 384, TRUE, TRUE);
}

void CSettingAssemble::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case 0: //P_Axis limit check timer//by shin//2007.01.04//
		SetPumpLimitCheck();
		break;
	default:
		AfxMessageBox("여기에 debug code를 넣어요..");
	} // end of switch()	
	CDialog::OnTimer(nIDEvent);
}

void CSettingAssemble::SetPumpLimitCheck()
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	static LONG AxisStatusP = 0;

	for(int i = 0; i < MAX_NOZZLE; i++)
	{
		if(pDoc->m_structHeadConfig[i].nA[0]==0) continue;

		if(Drop_Info.manu_head_job[i])
		{
			FAS_GetAxisStatus(AXIS_P1/BOARD_AXES+1,(AXIS_P1+i)%BOARD_AXES, &AxisStatusP);
			if(((AxisStatusP & 0x00000018)!=0))
			{ 
				KillTimer(0);
				pView->m_pDevice->Emergency_Stop();	
				pView->PostMessage(WM_ERROR,139,i+1);
				break;
			}
		}
	}
}

void CSettingAssemble::OnClickCmdPumpPositionSave() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	int i = 0;

	CNormalMsg dlg;
	dlg.m_bTimer=FALSE;

	if(pView->m_nLanguage == 0)
	{
		dlg.m_strTitle = _T("확인");
		dlg.m_strMsg1 = " Pump 데이터를 수정하시겠습니까? ";
		dlg.m_strMsg2 = " 수정하려면 'OK', 그렇지 않으면 'CANCEL' 버튼을 누르세요...";
	}	
	else if(pView->m_nLanguage == 1)
	{
		dlg.m_strTitle = _T("확인");
		dlg.m_strMsg1 = " Will you revise Pump data? ";
		dlg.m_strMsg2 = " if you revise 'OK', or Not revise Please Push Button 'CANCEL'......";
	}
	else if(pView->m_nLanguage == 2)
	{
		dlg.m_strTitle = _T("확인");
		dlg.m_strMsg1 = "  要修改Pump數據碼? ";
		dlg.m_strMsg2 = " 如需修改點擊 'OK', 否則點擊CANCEL...";
	}


	
	if( dlg.DoModal() == IDOK )
	{
		for(i =0 ; i <m_ctrlSettingGrid.GetRows() -1; i++)
		{
			Drop_Info.m_dAssemPos[i] = atof((char *)(LPCSTR)m_ctrlSettingGrid.GetTextMatrix(i+1, 1));
			Drop_Info.m_dBasePos[i] = atof((char *)(LPCSTR)m_ctrlSettingGrid.GetTextMatrix(i+1, 2));
			Drop_Info.m_dAdjustAbsPos[i] = atof((char *)(LPCSTR)m_ctrlSettingGrid.GetTextMatrix(i+1, 3));
			Drop_Info.m_dFullBasePos[i] = atof((char *)(LPCSTR)m_ctrlSettingGrid.GetTextMatrix(i+1, 4));
			Drop_Info.m_nSuctionOffset[i] = atoi((char *)(LPCSTR)m_ctrlSettingGrid.GetTextMatrix(i+1, 5));
		}
		Drop_Info.lSupplyAcctime = atoi((char *)(LPCSTR)m_GridCondition.GetTextMatrix(1, 1));
		Drop_Info.lSupplyDectime = atoi((char *)(LPCSTR)m_GridCondition.GetTextMatrix(2, 1));
		Drop_Info.lSupplyStartSpeed = atoi((char *)(LPCSTR)m_GridCondition.GetTextMatrix(3, 1));
		Drop_Info.lDegasAcctime = atoi((char *)(LPCSTR)m_GridCondition.GetTextMatrix(5, 1));
		Drop_Info.lDegasDectime = atoi((char *)(LPCSTR)m_GridCondition.GetTextMatrix(6, 1));
		Drop_Info.lDegasStartSpeed = atoi((char *)(LPCSTR)m_GridCondition.GetTextMatrix(7, 1));
		for(i = 0; i < 4; i++)
		{
			Drop_Info.m_nNCleanStep[i] = i;
			Drop_Info.m_nNCleanStepHz[i] = atoi((char *)(LPCSTR)m_ctrlNozzleCleanGrid.GetTextMatrix(i+1, 1));
			Drop_Info.m_nNCleanStepAmp[i] = atoi((char *)(LPCSTR)m_ctrlNozzleCleanGrid.GetTextMatrix(i+1, 2));
			Drop_Info.m_nNCleanStepNegOffset[i] = atoi((char *)(LPCSTR)m_ctrlNozzleCleanGrid.GetTextMatrix(i+1, 3));
			Drop_Info.m_dNCleanStepDuty[i] = atof((char *)(LPCSTR)m_ctrlNozzleCleanGrid.GetTextMatrix(i+1, 4));
			Drop_Info.m_nNCleanStepFallingEdge[i] = atoi((char *)(LPCSTR)m_ctrlNozzleCleanGrid.GetTextMatrix(i+1, 5));
			Drop_Info.m_dNCleanStepSpeed[i] = atoi((char *)(LPCSTR)m_ctrlNozzleCleanGrid.GetTextMatrix(i+1, 6));
		}
		
		CString strLog;
		for(int i=0; i<MAX_NOZZLE; i++)		//jykim 170517 PumpPositionSave 클릭 시 BasePos 값 로그 추가
		{
			strLog.Format("OnClickCmdPumpPositionSave - m_dBasePos[%d] = %.0f", i, Drop_Info.m_dBasePos[i]);
			pView->SaveLog(0,strLog);
		}								//jykim 170517 PumpPositionSave 클릭 시 BasePos 값 로그 추가

		pDoc->SaveSPDData();	
		pDoc->SaveNozzleCleanerData();
	}
	else
	{
		return;
	}	
}

void CSettingAssemble::SetNozzleCleanGrid()
{
	CString str;
	int i;
	
	m_ctrlNozzleCleanGrid.SetRows(5);
	m_ctrlNozzleCleanGrid.SetCols(7);
	
	for(i=0; i<7; i++)
	{
		if(0==i)
			m_ctrlNozzleCleanGrid.SetColWidth(i, 700);
		else if(6==i)
			m_ctrlNozzleCleanGrid.SetColWidth(i, 1330);
		else
			m_ctrlNozzleCleanGrid.SetColWidth(i, 980);
		
		m_ctrlNozzleCleanGrid.SetColAlignment(i, 4);
	}
	
	for(i=0; i<5; i++)
	{
		if(i == 0) 
			m_ctrlNozzleCleanGrid.SetRowHeight(i, 800);
		else
			m_ctrlNozzleCleanGrid.SetRowHeight(i, 450);
	}
	
	//	m_ctrlNozzleCleanGrid.SetTextMatrix(0, 0, "Step");
	m_ctrlNozzleCleanGrid.SetTextMatrix(0, 1, "   Freq    (Hz)");
	m_ctrlNozzleCleanGrid.SetTextMatrix(0, 2, "Amplitude  (V)");
	m_ctrlNozzleCleanGrid.SetTextMatrix(0, 3, "Neg.Offset  (V)");
	m_ctrlNozzleCleanGrid.SetTextMatrix(0, 4, "    Duty     (%)");
	m_ctrlNozzleCleanGrid.SetTextMatrix(0, 5, "Falling Edge (V)");
	m_ctrlNozzleCleanGrid.SetTextMatrix(0, 6, "Action_Speed  (pps)");
	
	for(i=1; i<4; i++)
	{
		m_ctrlNozzleCleanGrid.SetRow(i);m_ctrlNozzleCleanGrid.SetCol(6);
		m_ctrlNozzleCleanGrid.SetCellBackColor(LIGHTGRAY);
	}
	
	for(i=0; i<4; i++) //step1~step4
	{
		str.Format("Step%d", i+1);
		m_ctrlNozzleCleanGrid.SetTextMatrix(i+1, 0, str);
		
		str.Format("%d", Drop_Info.m_nNCleanStepHz[i]);
		m_ctrlNozzleCleanGrid.SetTextMatrix(i+1, 1, str);
		
		str.Format("%d", Drop_Info.m_nNCleanStepAmp[i]);
		m_ctrlNozzleCleanGrid.SetTextMatrix(i+1, 2, str);
		
		str.Format("%d", Drop_Info.m_nNCleanStepNegOffset[i]);
		m_ctrlNozzleCleanGrid.SetTextMatrix(i+1, 3, str);
		
		str.Format("%.1f", Drop_Info.m_dNCleanStepDuty[i]);
		m_ctrlNozzleCleanGrid.SetTextMatrix(i+1, 4, str);
		
		str.Format("%d", Drop_Info.m_nNCleanStepFallingEdge[i]);
		m_ctrlNozzleCleanGrid.SetTextMatrix(i+1, 5, str);
		
		if(i == 3) //step4만 사용//
		{
			str.Format("%.3f", Drop_Info.m_dNCleanStepSpeed[i]);
			m_ctrlNozzleCleanGrid.SetTextMatrix(i+1, 6, str);
		}
		else
		{
			Drop_Info.m_dNCleanStepSpeed[i] =0;
			str.Format("%.3f", Drop_Info.m_dNCleanStepSpeed[i]);
			m_ctrlNozzleCleanGrid.SetTextMatrix(i+1, 6, str);
		}
	}
}

void CSettingAssemble::OnClickGridCondition() 
{
	// TODO: Add your control notification handler code here
	int row, col;
	double m_dMax, m_dMin;
	
	row = m_GridCondition.GetMouseRow();
	col = m_GridCondition.GetMouseCol();

	if(0==row || 0==col || row == 4) return;

	switch(row)
	{
	case 1: case 2: case 5: case 6:	// Acc, Dec
		m_dMin = 1;
		m_dMax = 100;
		break;
	case 3:	case 7: // start Speed
		m_dMin = 1;
		m_dMax = 300000;
		break;
	default:
		break;
	}

	Use_TKg(m_GridCondition, m_dMax, m_dMin, 512, 384, TRUE, TRUE);	
}

void CSettingAssemble::SetSupplyDegasCondition()
{
	m_GridCondition.SetRows(8);//Supply,Degas,Acc,Dec,Start Speed
	m_GridCondition.SetCols(2);

	int i,j;
	m_GridCondition.SetColWidth(0,2500);
	m_GridCondition.SetColWidth(1,2000);
	for(i=0;i<m_GridCondition.GetRows();i++)
	{
		m_GridCondition.SetRowHeight(i,450);
		if(i == 0 || i == 4)
		{
			for(j=0;j<m_GridCondition.GetCols();j++)
			{
				m_GridCondition.SetRow(i);
				m_GridCondition.SetCol(j);
				m_GridCondition.SetCellBackColor(LIGHTGRAY);
			}
		}
		m_GridCondition.SetRow(i);
		m_GridCondition.SetCol(0);
		m_GridCondition.SetCellBackColor(LIGHTGRAY);
	}

	CString strText[8] = {"Supply","Acc (msec)","Dec (msec)","Start Speed (pps)","Degas","Acc (msec)","Dec (msec)","Start Speed (pps)"};
	CString str;
	for(i=0;i<m_GridCondition.GetRows();i++)
		m_GridCondition.SetTextMatrix(i,0,strText[i]);

	str.Format("%d",Drop_Info.lSupplyAcctime);
	m_GridCondition.SetTextMatrix(1,1,str);
	str.Format("%d",Drop_Info.lSupplyDectime);
	m_GridCondition.SetTextMatrix(2,1,str);
	str.Format("%d",Drop_Info.lSupplyStartSpeed);
	m_GridCondition.SetTextMatrix(3,1,str);
	str.Format("%d",Drop_Info.lDegasAcctime);
	m_GridCondition.SetTextMatrix(5,1,str);
	str.Format("%d",Drop_Info.lDegasDectime);
	m_GridCondition.SetTextMatrix(6,1,str);
	str.Format("%d",Drop_Info.lDegasStartSpeed);
	m_GridCondition.SetTextMatrix(7,1,str);
}

void CSettingAssemble::OnClickMsflexgridNozzleClean() 
{
	// TODO: Add your control notification handler code here
	int row, col;
	double m_dMax, m_dMin;
	
	row = m_ctrlNozzleCleanGrid.GetMouseRow();
	col = m_ctrlNozzleCleanGrid.GetMouseCol();

	if(0==row || col == 0) return;
	if(col == 6 && row !=4 ) return;

	switch(col)
	{
	case 1: //Hz
		m_dMin = 0;
		m_dMax = 300;
		break;
	case 2:	//Amp
		m_dMin = 0;
		m_dMax = 150;
		break;
	case 3:	//Neg. Offset
		m_dMin = 0;
		m_dMax = 150;
		break;
	case 4:	//Duty
		m_dMin = 0.0;
		m_dMax = 100.0;
		break;
	case 5:	//Falling Edge
		m_dMin = 0.0;
		m_dMax = 100.0;
		break;
	case 6:	//P Speed
		m_dMin = 0.0;
		m_dMax = 500000.0;
		break;
	default:
		break;
	}

	Use_TKg(m_ctrlNozzleCleanGrid, m_dMax, m_dMin, 512, 384, TRUE, TRUE);	
}

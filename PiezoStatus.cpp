// PiezoStatus.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "PiezoStatus.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"
#include "ErrMsg.h"
#include "P8CA_LcDispDoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern PIEZO_PARAM m_structParam;
extern PIEZO_STATUS m_structStatus;
extern PIEZO_AMP m_structAmp;
/////////////////////////////////////////////////////////////////////////////
// CPiezoStatus dialog


CPiezoStatus::CPiezoStatus(CWnd* pParent /*=NULL*/)
	: CDialog(CPiezoStatus::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPiezoStatus)
	pFont = new CFont;
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPiezoStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPiezoStatus)
	DDX_Control(pDX, IDC_COMBO_PIEZO_ID, m_comboPiezoID);
	DDX_Control(pDX, IDC_GRID_PARA1, m_GridPara1);
	DDX_Control(pDX, IDC_GRID_PARA2, m_GridPara2);
	DDX_Control(pDX, IDC_LABEL_RECIPE_NAME, m_ctrlRecipeName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPiezoStatus, CDialog)
	//{{AFX_MSG_MAP(CPiezoStatus)
	ON_CBN_SELCHANGE(IDC_COMBO_PIEZO_ID, OnSelchangeComboPiezoId)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPiezoStatus message handlers

BOOL CPiezoStatus::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	
	// TODO: Add extra initialization here
	int i;
	CString str;

	str.Format("%s|%s", pDoc->m_structOperatorModeRecipeData.strFrontRecipeName, pDoc->m_structOperatorModeRecipeData.strBackRecipeName);
	m_ctrlRecipeName.SetCaption(str);

	pFont->CreateFont(32,12,0,0,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,
	OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY, 
	DEFAULT_PITCH,"Tahoma");

	m_comboPiezoID.SetFont(pFont, TRUE);
	m_comboPiezoID.SetItemHeight(-1, 38);
	for(i=0;i<MAX_NOZZLE;i++)
	{
		m_comboPiezoID.SetItemHeight(i,38);
		str.Format("HEAD%d",i+1);
		m_comboPiezoID.AddString((LPCSTR)str);
	}
	
	//m_nTabSelect = 0;
	m_nHead = 0;
	m_comboPiezoID.SetCurSel(m_nHead);
	Set_PiezoStatus(m_nHead);
	SetGridData1(m_nHead);
	SetGridData2(m_nHead);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
// typedef struct tagPiezoParam
// {
// 	int nSlave;
// 	int nAmplitude;
// 	int nOffset;
// 	int nFrequecy;
// 	double nDuty;
// 	int nK_Amplitude;
// 	int nK_Offset;
// 	int nRun_Limit;
// 	int nFirst_Vol;
// 	int nRisingEdge;
// 	int nFallingEdge;
// 	int nPTNumber[100];
// 	int nPTSetNumber;
// } PIEZO_PARAM;
// 
// typedef struct tagPiezoStatus
// {
// 	BOOL bRun; // TRUE : Run , FALSE : Stop
// 	BOOL bError; // TRUE : Error , FALSE : Ok
// 	BOOL bOverLoad; // TRUE : Overload , FALSE : Ok
// 	BOOL bStandby; // TRUE : Standby On , FALSE : Standby off
// 	int nPTGetNextNumber;
// } PIEZO_STATUS;
// 
// typedef struct tagAmplitude
// {
// 	int nAp; // Plus
// 	int nAm; // Minus
// 	int nApp; // Total
//} PIEZO_AMP;
void CPiezoStatus::SetGridData1(int nID)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	int i,j;
	int nC,nR;
	m_GridPara1.SetRows(23); m_GridPara1.SetCols(3);
	CString Index1[23] = {"","nSlave ","nAmplitude ","nOffset ","nFrequecy","nDuty","nK_Amplitude","nK_Offset","nRun_Limit","nFirst_Vol","nRisingEdge","nFallingEdge","nPTSetNumber"
	,"","bRun ","bError ","bOverLoad ","bStandby","nPTGetNextNumber","","nAp ","nAm ","nApp"};
	CString Index2[3] = {"Parameter","Data",""};
	CString Data1[23] = {"",};

	m_GridPara1.ShowWindow(0);
	
	m_GridPara1.SetTextMatrix(0,0,Index2[0]);
	m_GridPara1.SetTextMatrix(0,1,Index2[1]);
	m_GridPara1.SetTextMatrix(0,2,Index2[2]);


	Data1[0].Format("%s","");
	Data1[1].Format("%d",m_structParam.nSlave);
	Data1[2].Format("%d",m_structParam.nAmplitude);
	Data1[3].Format("%d",m_structParam.nOffset);
	Data1[4].Format("%d",m_structParam.nFrequecy);	
	Data1[5].Format("%f",m_structParam.nDuty);
	Data1[6].Format("%d",m_structParam.nK_Amplitude);	
	Data1[7].Format("%d",m_structParam.nK_Offset);
	Data1[8].Format("%d",m_structParam.nRun_Limit);	
	Data1[9].Format("%d",m_structParam.nFirst_Vol);
	Data1[10].Format("%d",m_structParam.nRisingEdge);	
	Data1[11].Format("%d",m_structParam.nFallingEdge);
	Data1[12].Format("%d",m_structParam.nPTSetNumber);
	Data1[13].Format("%s","");	
	Data1[14].Format("%d",m_structStatus.bRun);
	Data1[15].Format("%d",m_structStatus.bError);	
	Data1[16].Format("%d",m_structStatus.bOverLoad);
	Data1[17].Format("%d",m_structStatus.bStandby);	
	Data1[18].Format("%d",m_structStatus.nPTGetNextNumber);
	Data1[19].Format("%s","");
	Data1[20].Format("%d",m_structAmp.nAp);	
	Data1[21].Format("%d",m_structAmp.nAm);
	Data1[22].Format("%d",m_structAmp.nApp);	


	for(i=0;i<m_GridPara1.GetRows();i++)
	{
		for(j=0;j<m_GridPara1.GetCols();j++)
		{
			m_GridPara1.SetRowHeight(i,380);
			if(j==0)
				m_GridPara1.SetColWidth(j,2200);
			else if(j==1)
				m_GridPara1.SetColWidth(j,900);
			else if(j==2)
				m_GridPara1.SetColWidth(j,600);
			m_GridPara1.SetColAlignment(j,4);
			if(i == 0) continue;
			m_GridPara1.SetTextMatrix(i,0,Index1[i]);
			m_GridPara1.SetTextMatrix(i,1,Data1[i]);
			//13,19
			if(i == 13 || i == 19)
			{
				m_GridPara1.SetRow(i);m_GridPara1.SetCol(j);
				m_GridPara1.SetCellBackColor(LIGHTGRAY);
			}
		}
	}
	m_GridPara1.ShowWindow(1);
}

void CPiezoStatus::SetGridData2(int nID)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	int i,j;
	int nC,nR;
	CString str;
	CString Data2[100] = {"",};
	m_GridPara2.SetRows(20); m_GridPara2.SetCols(10);

	m_GridPara2.ShowWindow(0);

	for(i=0;i<100;i++)
	{
		Data2[i].Format("%d", m_structParam.nPTNumber[i]);
		//Data2[i].Format("%d", i*10);
	}

	for(i=0;i<m_GridPara2.GetRows();i++)
	{
		for(j=0;j<m_GridPara2.GetCols();j++)
		{
			m_GridPara2.SetRowHeight(i,400);
			if(j%2 == 0)
			{
				m_GridPara2.SetColWidth(j,750);
				m_GridPara2.SetRow(i);m_GridPara2.SetCol(j);
				m_GridPara2.SetCellBackColor(LIGHTGRAY);
				str.Format("%d",i+((j/2)*20));
				m_GridPara2.SetTextMatrix(i,j,str);
			}
			else 
			{
				m_GridPara2.SetColWidth(j,1100);
				m_GridPara2.SetTextMatrix(i,j,Data2[i+(((j-1)/2)*20)]);					
			}

			m_GridPara2.SetColAlignment(j,4);
		}
	}
	m_GridPara2.ShowWindow(1);
}

void CPiezoStatus::OnSelchangeComboPiezoId() 
{
	// TODO: Add your control notification handler code here
	m_nHead = m_comboPiezoID.GetCurSel();
	Set_PiezoStatus(m_nHead);
	SetGridData1(m_nHead);
	SetGridData2(m_nHead);
}

void CPiezoStatus::Set_PiezoStatus(int nHead)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	
// 	int nBasePort = N_PORT; //port define 필요...//
// 	
// 	//N-Con Serial Port Open의 Port번호는???
// 	//Baud rate set
// 	pView->m_pPiezoDevice->m_nPort = nBasePort;
// 	pView->m_pPiezoDevice->m_nBaudrate = 115200;
// 	pView->m_pPiezoDevice->m_nSlave = nHead;
// 	
// 	pView->m_pPiezoDevice->Set_Port(pView->m_pPiezoDevice->m_nPort, pView->m_pPiezoDevice->m_nBaudrate, pView->m_pPiezoDevice->m_nSlave);
// 	Sleep(100);
	
	pView->m_pPiezoDevice->m_nSlave = nHead;
#if N_MODIFY_EQ	
	//Connection
	if(!pView->m_pPiezoDevice->Connect())
	{
		CErrMsg dlg; 
		dlg.m_nSelectAlarm = 0; 
		dlg.m_nErrCode = 40;
		dlg.m_nErrCode2 = nHead+1;
		dlg.DoModal();
		AfxGetMainWnd()->PostMessage(WM_CLOSE); 
		return;
	}
	
	//Read//
	pView->m_pPiezoDevice->Read_Param(&m_structParam);
	pView->m_pPiezoDevice->Get_Status(&m_structStatus);
	pView->m_pPiezoDevice->Cal_Amplitude(&m_structAmp);
	Sleep(5);
#endif	
// 	//write//default//
// 	m_structParam.nAmplitude = 50;
// 	m_structParam.nDuty = 50.0;
// 	m_structParam.nFrequecy = 200;
// 	m_structParam.nOffset = 10;
// 	m_structParam.nFallingEdge = 10;
// 	m_structParam.nFirst_Vol = 50; //first voltage
// 	m_structParam.nRun_Limit = 0; //run limit // 측정 동작에서는 run limit가 동작 되지 않도록...//
// 	for(int j = 0; j < 100; j++)
// 	{
// 		m_structParam.nPTNumber[0] = 0; //pt table[100] //첫번째 배열이 0이면 동작 되지 않음..//
// 	}
// 	//사용 하지 않는 파라미터는 0으로 초기화 한다.//data 먹통 방지...//
// 	m_structParam.nK_Amplitude = 0;
// 	m_structParam.nK_Offset = 0;
// 	m_structParam.nRisingEdge = 0;
// 	m_structParam.nPTSetNumber = 0;
// 	
// 	if(pView->m_pPiezoDevice->Write_Param(m_structParam) == FALSE)
// 	{
// 		CErrMsg dlg; 
// 		dlg.m_nSelectAlarm = 0; 
// 		dlg.m_nErrCode = 41;
// 		dlg.m_nErrCode2 = nHead+1;
// 		dlg.DoModal();
// 		AfxGetMainWnd()->PostMessage(WM_CLOSE); 
// 		return;
// 	}
// 	
// 	//Standby-ALL//
// 	pView->m_pPiezoDevice->Do_Standby_All(TRUE);
// 	Sleep(300);
// 	pView->m_pPiezoDevice->Do_Standby_All(FALSE);
}

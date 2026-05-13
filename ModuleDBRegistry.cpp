// ModuleDBRegistry.cpp : implementation file
//

#include "stdafx.h"
#include "p8ca_lcdisp.h"
#include "ModuleDBRegistry.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModuleDBRegistry dialog


CModuleDBRegistry::CModuleDBRegistry(CWnd* pParent /*=NULL*/)
	: CDialog(CModuleDBRegistry::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModuleDBRegistry)
		// NOTE: the ClassWizard will add member initialization here
	strModuleID = "";
	nAmplitudeOffset_DB = 0;
	nNegOffsetOffset_DB = 0;
	nFrequecyOffset_DB = 0;
	nDutyOffset_DB = 0.0;
	nFirst_VolOffset_DB = 0;
	nFallingEdgeOffset_DB = 0;
	//}}AFX_DATA_INIT
}


void CModuleDBRegistry::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModuleDBRegistry)
	DDX_Control(pDX, IDC_GRID_DB_REGISTRY, m_GridDBRegistry);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModuleDBRegistry, CDialog)
	//{{AFX_MSG_MAP(CModuleDBRegistry)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModuleDBRegistry message handlers

BOOL CModuleDBRegistry::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	this->MoveWindow(0 ,1 , 1024 ,768);

	GetDlgItem(IDOK)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDCANCEL)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	
	GetDlgItem(IDC_CMD_REGISTRY)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );

	SetGrid_Initial();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CModuleDBRegistry::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
// 	CPen pen;
// 	pen.CreatePen(PS_SOLID,5,DARKBLUE);
// 	CPen *pOldPen = (CPen *)dc.SelectObject(&pen);
// 
// 	dc.MoveTo(0,HEIGHT_YPOS+310);
// 	dc.LineTo(SCREEN_WIDTH,HEIGHT_YPOS+310);
// 
// 	dc.SelectObject(pOldPen);	
	// Do not call CDialog::OnPaint() for painting messages
}

BEGIN_EVENTSINK_MAP(CModuleDBRegistry, CDialog)
    //{{AFX_EVENTSINK_MAP(CModuleDBRegistry)
	ON_EVENT(CModuleDBRegistry, IDC_GRID_DB_REGISTRY, -600 /* Click */, OnClickGridDbRegistry, VTS_NONE)
	ON_EVENT(CModuleDBRegistry, IDC_CMD_RETURN, -600 /* Click */, OnClickCmdReturn, VTS_NONE)
	ON_EVENT(CModuleDBRegistry, IDC_CMD_CREATION, -600 /* Click */, OnClickCmdCreation, VTS_NONE)
	ON_EVENT(CModuleDBRegistry, IDC_CMD_REGISTRY, -600 /* Click */, OnClickCmdRegistry, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CModuleDBRegistry::OnClickGridDbRegistry() 
{
	// TODO: Add your control notification handler code here
	int nR=0,nC=0;
	int nMax=0,nMin=0;
	int nlength=0,i=0;
	CString strTemp;

	nR = m_GridDBRegistry.GetMouseRow();
	nC = m_GridDBRegistry.GetMouseCol();

	if(nR == 0)
	{
		nMin = 0;
		nMax = 9999;
	}
	else if( nR == 1 ) //AMP
	{
		nMin = -15;
		nMax =  15;
	}
	else if( nR == 2 ) //NEG
	{
		nMin = -15;
		nMax =  15;
	}
	else if( nR == 3 ) //DUTY
	{
		nMin = -5;
		nMax =  5;
	}
	else			   //FIRST VOL
	{
		nMin = -15;
		nMax =  15;
	}

	Use_TKg(m_GridDBRegistry, nMax, nMin, 15, 250, TRUE, TRUE);

	if(nR == 0)
	{
		strModuleID = (char *)(LPCSTR)m_GridDBRegistry.GetTextMatrix(nR,nC);
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
		m_GridDBRegistry.SetTextMatrix(nR,nC,strModuleID);

		m_GridDBRegistry.SetTextMatrix(1,1,"00");
		m_GridDBRegistry.SetTextMatrix(2,1,"00");
		m_GridDBRegistry.SetTextMatrix(3,1,"00");
		m_GridDBRegistry.SetTextMatrix(4,1,"00");
	}
	if(nR == 1)
		nAmplitudeOffset_DB = atoi((char *)(LPCSTR)m_GridDBRegistry.GetTextMatrix(nR,nC));
	if(nR == 2)
		nNegOffsetOffset_DB = atoi((char *)(LPCSTR)m_GridDBRegistry.GetTextMatrix(nR,nC));
	if(nR == 3)
		nDutyOffset_DB      = atof((char *)(LPCSTR)m_GridDBRegistry.GetTextMatrix(nR,nC));
	if(nR == 4)
		nFirst_VolOffset_DB = atoi((char *)(LPCSTR)m_GridDBRegistry.GetTextMatrix(nR,nC));
}

void CModuleDBRegistry::OnClickCmdReturn() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument(); 

	pView->pN_Func->Read_Module_DB(pDoc->m_structOperatorModeRecipeData.strFrontRecipeName);

	EndDialog(IDOK);
}

void CModuleDBRegistry::SetGrid_Initial()
{
	int nR,nC;
	m_GridDBRegistry.SetRows(5);
	m_GridDBRegistry.SetCols(2);

	CString strText[5] = {"ID","Amp(V)Offset","Neg.Offset(V)Offset","Duty(%)Offset","FirstVolt(V)Offset"};

	for(nR=0;nR<m_GridDBRegistry.GetRows();nR++)
	{
		m_GridDBRegistry.SetRowHeight(nR,700);

		for(nC=0;nC<m_GridDBRegistry.GetCols();nC++)
		{
			m_GridDBRegistry.SetTextMatrix(nR,0,strText[nR]);
			m_GridDBRegistry.SetColWidth(nC,2100);
			m_GridDBRegistry.SetColAlignment(nC, 4);
		}
	}

}

void CModuleDBRegistry::OnClickCmdCreation() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument(); 

	CString strModuleID_Temp , strTempLog = "";

	strModuleID.Empty();
	nAmplitudeOffset_DB=nNegOffsetOffset_DB=nFrequecyOffset_DB=nFirst_VolOffset_DB=nFallingEdgeOffset_DB=0;
	nDutyOffset_DB = 0.0;

	strModuleID           = (char *)(LPCSTR)m_GridDBRegistry.GetTextMatrix(0,1);
	nAmplitudeOffset_DB   = atoi((char *)(LPCSTR)m_GridDBRegistry.GetTextMatrix(1,1));
	nNegOffsetOffset_DB   = atoi((char *)(LPCSTR)m_GridDBRegistry.GetTextMatrix(2,1));
	nDutyOffset_DB        = atof((char *)(LPCSTR)m_GridDBRegistry.GetTextMatrix(3,1));
	nFirst_VolOffset_DB   = atoi((char *)(LPCSTR)m_GridDBRegistry.GetTextMatrix(4,1));

	if(strModuleID == "")
	{
		AfxMessageBox("대상 Module ID 없음..");

		strTempLog = "ID 가 입력 되지 않았습니다.";
		pView->pN_Func->Save_N_Log(0,strTempLog);
		
		return;		
	}

	if(pView->pN_Func->Module_Duplication_Check(1,pDoc->m_structOperatorModeRecipeData.strFrontRecipeName,strModuleID) == 100)
	{
		AfxMessageBox("중복 DB 존재 함...");

		strTempLog = "중복 DB 가 존재 합니다.";
		pView->pN_Func->Save_N_Log(0,strTempLog);
		return;
	}

	int nModuleID = atoi(strModuleID); 


	pView->pN_Func->m_structModuleData_Set[nModuleID-1].strModuleID = strModuleID;
		
	pView->pN_Func->m_structModuleData_Set[nModuleID-1].nAmplitude_Offset = nAmplitudeOffset_DB;
	pView->pN_Func->m_structModuleData_Set[nModuleID-1].nNegOffset_Offset = nNegOffsetOffset_DB;
	pView->pN_Func->m_structModuleData_Set[nModuleID-1].nDuty_Offset      = nDutyOffset_DB;
	pView->pN_Func->m_structModuleData_Set[nModuleID-1].nFirst_Vol_Offset = nFirst_VolOffset_DB;

	strTempLog.Format("NEW REGISTRY -> Module ID %d : AMP OFFSET (%d) , NEG OFFSET (%d) , DUTY OFFSET (%f) , FIRST OFFSET (%d)"
		,nModuleID,pView->pN_Func->m_structModuleData_Set[nModuleID-1].nAmplitude_Offset ,pView->pN_Func->m_structModuleData_Set[nModuleID-1].nNegOffset_Offset
		,pView->pN_Func->m_structModuleData_Set[nModuleID-1].nDuty_Offset ,pView->pN_Func->m_structModuleData_Set[nModuleID-1].nFirst_Vol_Offset);
	pView->pN_Func->Save_N_Log(0,strTempLog);	
	
	pView->SaveDB_HistoryLog(nModuleID,NULL,DB_NEW_REGISTRY,NULL,""); 

	pView->pN_Func->Save_Module_DB(pDoc->m_structOperatorModeRecipeData.strFrontRecipeName);
}

void CModuleDBRegistry::OnClickCmdRegistry() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument(); 

	
}

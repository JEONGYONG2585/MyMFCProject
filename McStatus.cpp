// McStatus.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "McStatus.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"
#include "Color.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMcStatus dialog


CMcStatus::CMcStatus(CWnd* pParent /*=NULL*/)
	: CDialog(CMcStatus::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMcStatus)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMcStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMcStatus)
	DDX_Control(pDX, IDC_GRDI_MCSTATUS, m_GridMcStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMcStatus, CDialog)
	//{{AFX_MSG_MAP(CMcStatus)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMcStatus message handlers

BOOL CMcStatus::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();	
	// TODO: Add extra initialization here

//	this->MoveWindow(420,-1,600,70);
	SetWindowPos( &wndTopMost,420,0,600,90, SWP_SHOWWINDOW );
//////////////////////////////////////////////////////////////////////////////////////
//	int i;
//	for(i=0;i<11;i++)
//		pView->m_nMCNaviStatus[i] = 0;
//	pView->m_nMCNaviStatus[0]=0;
//	pView->m_nMCNaviStatus[1]=1;
//	pView->m_nMCNaviStatus[2]=1;
//	pView->m_nMCNaviStatus[3]=0;
//	pView->m_nMCNaviStatus[4]=1;
//	pView->m_nMCNaviStatus[5]=1;
//	pView->m_nMCNaviStatus[6]=1;
//	pView->m_nMCNaviStatus[7]=1;
//	pView->m_nMCNaviStatus[8]=1;
//	pView->m_nMCNaviStatus[9]=1;
//	pView->m_nMCNaviStatus[10]=1;
////////////////////////////////////////////////////////////////////////////////////////

	InitMcStatus();

	pView->McNaviStatusReport();

	SetTimer(0,1000,NULL);

//	SetTimer(0,500,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMcStatus::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();	
	pView->McNaviStatusReport();
	InitMcStatus();
	CDialog::OnTimer(nIDEvent);
}

void CMcStatus::InitMcStatus()
{
	CMainFrame *pFrame = (CMainFrame*) AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView*) pFrame->GetActiveView();

	int i,j;
	char *Title[11] = {"LC교체","LC공급","기포제거","조정","측정","Cnt확인","W/Up","반입","Dotting","상태확인","완료"};
	CString str;

	m_GridMcStatus.SetRows(2);
	m_GridMcStatus.SetCols(11);
	
	m_GridMcStatus.SetRowHeight(0,500);
	m_GridMcStatus.SetRowHeight(1,350);
	for(i=0;i<11;i++)
	{
		m_GridMcStatus.SetColAlignment(i,4);
		m_GridMcStatus.SetColWidth(i,750);
		str.Format("%s",Title[i]);
		m_GridMcStatus.SetTextMatrix(0,i,str);
	}

	for(i=0;i<11;i++)
	{
//		pView->m_nMCNaviStatus[i] = 2;
		if(pView->m_nMCNaviStatus[i] == 0)
		{
			str.Format("%s","미실시");
			m_GridMcStatus.SetTextMatrix(1,i,str);
			m_GridMcStatus.SetRow(1);
			m_GridMcStatus.SetCol(i);
			m_GridMcStatus.SetCellBackColor(LIGHTGRAY);
		}
		else if(pView->m_nMCNaviStatus[i] == 1)
		{
			str.Format("%s","진행중");
			m_GridMcStatus.SetTextMatrix(1,i,str);
			m_GridMcStatus.SetRow(1);
			m_GridMcStatus.SetCol(i);
			m_GridMcStatus.SetCellBackColor(YELLOW);
		}
		else if(pView->m_nMCNaviStatus[i] == 2)
		{
			str.Format("%s","완료");
			m_GridMcStatus.SetTextMatrix(1,i,str);
			m_GridMcStatus.SetRow(1);
			m_GridMcStatus.SetCol(i);
			m_GridMcStatus.SetCellBackColor(GREEN);
		}
	}
}

BOOL CMcStatus::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE) // ESC Key가 아니면
		{
			pMsg->wParam = NULL;
			pMsg->lParam = NULL;
		}
	}		
	return CDialog::PreTranslateMessage(pMsg);
}

void CMcStatus::OnOK() 
{
	// TODO: Add extra validation here
	CMainFrame *pFrame = (CMainFrame*) AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView*) pFrame->GetActiveView();

	pView->m_pMcStatus = NULL;
	KillTimer(0);

	CDialog::OnOK();
}

BOOL CMcStatus::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DestroyWindow();
}

BOOL CMcStatus::Create(CWnd* pParentWnd) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::Create(IDD, pParentWnd);
}

void CMcStatus::CloseWindow()
{
	DestroyWindow();
}

BEGIN_EVENTSINK_MAP(CMcStatus, CDialog)
    //{{AFX_EVENTSINK_MAP(CMcStatus)
	ON_EVENT(CMcStatus, IDC_GRDI_MCSTATUS, -601 /* DblClick */, OnDblClickGrdiMcstatus, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CMcStatus::OnDblClickGrdiMcstatus() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();

	int nRow,nCol;
	CString strStatus1,strStatus2;

	nRow = m_GridMcStatus.GetMouseRow();
	nCol = m_GridMcStatus.GetMouseCol();

	if(nRow == 0) return;

	strStatus1 = (LPCSTR)(m_GridMcStatus.GetTextMatrix(nRow,nCol));
	m_GridMcStatus.SetRow(nRow);
	m_GridMcStatus.SetCol(nCol);

	if(strStatus1 == "미실시")
	{
		strStatus2 = "진행중";
		m_GridMcStatus.SetCellBackColor(YELLOW);
		pView->m_nMCNaviStatus[nCol] = 1;
	}
	else if(strStatus1 == "진행중")
	{
		strStatus2 = "완료";
		m_GridMcStatus.SetCellBackColor(GREEN);
		pView->m_nMCNaviStatus[nCol] = 2;
	}
	else if(strStatus1 == "완료")
	{
		strStatus2 = "미실시";
		m_GridMcStatus.SetCellBackColor(LIGHTGRAY);
		pView->m_nMCNaviStatus[nCol] = 0;
	}
	m_GridMcStatus.SetTextMatrix(nRow,nCol,strStatus2);
	
}

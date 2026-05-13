// MCTimer.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "MCTimer.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMCTimer dialog
extern double g_dMCTimer[3]; // by ckh 2009.03.24 장비 상태 관련 타이머 시간. 



CMCTimer::CMCTimer(CWnd* pParent /*=NULL*/)
	: CDialog(CMCTimer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMCTimer)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMCTimer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMCTimer)
	DDX_Control(pDX, IDC_LAB_TIMER, m_ctrlLabTimer);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMCTimer, CDialog)
	//{{AFX_MSG_MAP(CMCTimer)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMCTimer message handlers



BOOL CMCTimer::Create(CWnd* pParentWnd)
{
	// TODO: Add your specialized code here and/or call the base class
		
	return CDialog::Create(IDD, pParentWnd);
}

void CMCTimer::PostNcDestroy() 
{
	CMainFrame* pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	
	pView->m_pMCTimer = NULL;

	delete this;	
	
	CDialog::PostNcDestroy();
}

void CMCTimer::CloseWindow()
{
	CMainFrame* pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CString strTemp,strTemp1;
	int nSettingTime  = g_dMCTimer[m_nStatus]*60;
	switch(m_nStatus)
	{
	case 0:
		strTemp1 = "MODEL CHANGE";
		break;
	case 1:
		strTemp1 = "CLEANING";
		break;
	case 2:
		strTemp1 = "MATERIAL CHANGE";
		break;
	}

	strTemp.Format("MC END TIMES[%s]  %02d : %02d : %02d [Setting Time(%02d : %02d : %02d)] ",strTemp1,((m_nCount/60)/60)%60,(m_nCount/60)%60, m_nCount%60,((nSettingTime/60)/60)%60,(nSettingTime/60)%60, nSettingTime%60);

	pView->SaveLog(0,strTemp);
	DestroyWindow();
}

BOOL CMCTimer::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CMainFrame* pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	SetWindowPos( &wndTopMost,0,0,450,76, SWP_SHOWWINDOW );
	
	m_nStatus = pView->m_nMachineStatus;
	m_nTimer = g_dMCTimer[m_nStatus]*60;
	m_nCount = 0;
	SetTimer(0,1000,NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
#include "Color.h"
///#include <math.h>
void CMCTimer::OnTimer(UINT nIDEvent) 
{
	CString strTemp;
	
	strTemp.Format(" %02d : %02d : %02d ",((abs(m_nTimer)/60)/60)%60,(abs(m_nTimer)/60)%60, abs(m_nTimer)%60);
	m_ctrlLabTimer.SetCaption(strTemp);
	m_nTimer--;
	m_nCount++;

	if(m_nTimer <0 )
	{
		if(m_nTimer%2 == 0)
			m_ctrlLabTimer.SetBackColor(RED);
		else
			m_ctrlLabTimer.SetBackColor(YELLOW);
		
		m_ctrlLabTimer.SetCaption("-" + strTemp);

	}

	CDialog::OnTimer(nIDEvent);
}

BOOL CMCTimer::PreTranslateMessage(MSG* pMsg) 
{
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

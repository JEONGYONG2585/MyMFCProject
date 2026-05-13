// Maint.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "Maint.h"

#include "MainFrm.h"
#include "P8CA_LcDispView.h"
#include "SafetyMap.h" //20181130 jeongyong - DoorInterlock PVG 추가

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL	m_bMaintModeExit;
extern CString m_strMaintModePw;
/////////////////////////////////////////////////////////////////////////////
// CMaint dialog

extern BOOL g_DoorLockDispOff; //jykim 171104 DoorInterlock정상화
extern BOOL g_bPvgOn; //jykim 171208 DoorInterlock PVG 추가

CMaint::CMaint(CWnd* pParent /*=NULL*/)
	: CDialog(CMaint::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMaint)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMaint::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMaint)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMaint, CDialog)
	//{{AFX_MSG_MAP(CMaint)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMaint message handlers

BEGIN_EVENTSINK_MAP(CMaint, CDialog)
    //{{AFX_EVENTSINK_MAP(CMaint)
	ON_EVENT(CMaint, IDC_CMD_PREV, -600 /* Click */, OnClickCmdPrev, VTS_NONE)
	ON_EVENT(CMaint, IDC_CMD_PVG, -600 /* Click */, OnClickCmdPvg, VTS_NONE)
	ON_EVENT(CMaint, IDC_CMD_BUZZER_OFF, -600 /* Click */, OnClickCmdBuzzerOff, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CMaint::OnClickCmdPrev() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	if(g_DoorLockDispOff == TRUE)
	{
		AfxMessageBox("Door Open 상태입니다!!", MB_OK);
	}
	else if(pView->m_pPassQa == NULL)
	{
		m_bMaintModeExit = TRUE;
		pDoc->m_bPassWordOK=FALSE;
		pView->m_pPassQa = new CPassQa;
		pView->m_pPassQa->Create(IDD_PASS_QA,this);
		pView->m_pPassQa->SetWindowPos( NULL,0,0,0,0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER ); 
	}
}

void CMaint::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	m_pTime = CTime::GetCurrentTime();
	m_strTime.Format("%04d/ %02d/ %02d",m_pTime.GetYear(),m_pTime.GetMonth(),m_pTime.GetDay());
	SetDlgItemText(IDC_LABEL_TIME,m_strTime);
	m_strTime1.Format("%02d : %02d",m_pTime.GetHour(),m_pTime.GetMinute());	
	SetDlgItemText(IDC_LABEL_TIME_1,m_strTime1);

	if(pDoc->m_bMaintMode==FALSE)
	{
		pView->m_bMaintModeClick=FALSE;
		KillTimer(0);
		EndDialog(IDOK);	
	}
	CDialog::OnTimer(nIDEvent);
}

BOOL CMaint::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();	
	// TODO: Add extra initialization here
	CString str,str1;

	SelectLanguage();

	if(g_DoorLockDispOff == TRUE)
	{

	}
	else
	{
		str.Format("설정 PassWord %s",m_strMaintModePw);
		AfxMessageBox(str);
		SetTimer(0,100,NULL);
	}
	// hide
	GetDlgItem(IDC_CMD_PVG)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CMD_PREV)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMaint::PreTranslateMessage(MSG* pMsg) 
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

void CMaint::SelectLanguage()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	if(pView->m_nLanguage == 0)
	{
		SetDlgItemText(1256, _T("Maintenance 중 입니다."));	
		SetDlgItemText(IDC_LABEL49, _T("주의 하세요..!!"));	
	}
	else if(pView->m_nLanguage == 1)
	{
		SetDlgItemText(1256, _T("Maintenance Mode "));	
		SetDlgItemText(IDC_LABEL49, _T("Be careful..!!"));	
	}
	else if(pView->m_nLanguage == 2)
	{
		SetDlgItemText(1256, _T("Maintenance中."));	
		SetDlgItemText(IDC_LABEL49, _T("請注意..!!"));	
	}

}

void CMaint::OnClickCmdPvg() 
{
	// TODO: Add your control notification handler code here
	g_bPvgOn = TRUE;
	CSafetyMap dlg;
	dlg.DoModal();		
}

void CMaint::OnClickCmdBuzzerOff() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
///////////////////////////////////////////////////////////////20211223 jeongyong - Buzzer Off 기능 추가.
	pView->m_pDevice->MNET_WriteBit(BaseAddr_DISPCTRL+BitDispBuzzer,"0000"); 
}

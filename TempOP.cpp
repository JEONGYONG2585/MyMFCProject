// TempOP.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "TempOP.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTempOP dialog

extern BOOL m_bSWCheck;
CTempOP::CTempOP(CWnd* pParent /*=NULL*/)
	: CDialog(CTempOP::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTempOP)
		// NOTE: the ClassWizard will add member initialization here
	m_bStart=m_bStop=m_bUp=m_bDown=m_bLeft=m_bRight=m_bOK=FALSE;
	//}}AFX_DATA_INIT
}


void CTempOP::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTempOP)
	DDX_Control(pDX, IDC_START, m_ctrlStart);
	DDX_Control(pDX, IDC_STOP, m_ctrlStop);
	DDX_Control(pDX, IDC_OK, m_ctrlOK);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTempOP, CDialog)
	//{{AFX_MSG_MAP(CTempOP)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTempOP message handlers

void CTempOP::OnOK() 
{
	// TODO: Add extra validation here
	return;
	CDialog::OnOK();
}

BOOL CTempOP::OnInitDialog() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CDialog::OnInitDialog();

	SelectLanguage();
	// TODO: Add extra initialization here
	m_bUp=m_bDown=m_bLeft=m_bRight=FALSE;
	m_bOK = FALSE;m_bSWCheck=FALSE;
	GetDlgItem(IDOK)->SetWindowPos(NULL,0,0,0,0,SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_EVENTSINK_MAP(CTempOP, CDialog)
    //{{AFX_EVENTSINK_MAP(CTempOP)
	ON_EVENT(CTempOP, IDC_START, -600 /* Click */, OnClickStart, VTS_NONE)
	ON_EVENT(CTempOP, IDC_OK, -600 /* Click */, OnClickOk, VTS_NONE)
	ON_EVENT(CTempOP, IDC_STOP, -600 /* Click */, OnClickStop, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CTempOP::OnClickStart() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
//////////////////////////////////////////////////20181130 jeongyong - KEY ªÛ≈¬ √º≈©
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
	
	m_bSWCheck = TRUE;
	CDialog::OnOK();
}

void CTempOP::OnClickOk() 
{
	// TODO: Add your control notification handler code here
//	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
//	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	// TODO: Add your control notification handler code here
//	pView->m_pDevice->SST_Check(OK_SWITCH);	
	m_bSWCheck = TRUE;
	CDialog::OnOK();
}

void CTempOP::OnClickStop() 
{
	// TODO: Add your control notification handler code here
//	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
//	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	// TODO: Add your control notification handler code here
//	pView->m_pDevice->SST_Check(STOP_SWITCH);
	m_bSWCheck = TRUE;
	CDialog::OnOK();
}

void CTempOP::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnTimer(nIDEvent);
}

void CTempOP::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}


BOOL CTempOP::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
//	m_bStart=m_bStop=m_bUp=m_bDown=m_bLeft=m_bRight=m_bOK=FALSE;

	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->message == VK_ESCAPE)
		{
			pMsg->wParam = NULL;
			pMsg->lParam = NULL;
		}
	}
	if(pMsg->message == WM_LBUTTONDOWN)
	{
		CWnd* pWnd = CWnd::FromHandle(pMsg->hwnd);
		int nTID = pWnd->GetDlgCtrlID();
		m_bStart=m_bStop=m_bUp=m_bDown=m_bLeft=m_bRight=m_bOK=FALSE;
 		if(nTID == IDC_LEFT)
		{
			m_bLeft = TRUE;
		}
		else if(nTID == IDC_RIGHT)
		{
			m_bRight = TRUE;
		}
		else if(nTID == IDC_UP)
		{
			m_bUp = TRUE;
		}
		else if(nTID == IDC_DOWN)
		{
			m_bDown = TRUE;
		}
		else if(nTID == IDC_OK)
		{
			m_bOK = TRUE;
		}
//		return 0;
	}
	else if(pMsg->message == WM_LBUTTONUP)
	{
//		m_bStart=m_bStop=m_bUp=m_bDown=m_bLeft=m_bRight=m_bOK=FALSE;
		m_bUp=m_bDown=m_bLeft=m_bRight=FALSE;
		m_bOK = FALSE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CTempOP::SelectLanguage()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(pView->m_nLanguage == 0)	//ehji 140309
	{
		SetDlgItemText(IDC_START,				_T("Ω√¿€"));
		SetDlgItemText(IDC_STOP,				_T("¡§¡ˆ"));
	}
	else if(pView->m_nLanguage == 1)	//ehji 140309
	{
		SetDlgItemText(IDC_START,				_T("Start"));
		SetDlgItemText(IDC_STOP,				_T("Stop"));
	}
	else if(pView->m_nLanguage == 2)	//ehji 140309
	{
		SetDlgItemText(IDC_START,				_T("À“„∑"));
		SetDlgItemText(IDC_STOP,				_T("ÔŒÚ≠"));
	}
}

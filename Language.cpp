// Language.cpp : implementation file
//  ehji 140309 언어 관리 

#include "stdafx.h"
#include "p8ca_lcdisp.h"
#include "Language.h"

#include "MainFrm.h"
#include "P8CA_LcDispView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLanguage dialog


CLanguage::CLanguage(CWnd* pParent /*=NULL*/)
	: CDialog(CLanguage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLanguage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CLanguage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLanguage)
	DDX_Control(pDX, IDC_CMD_CHINA, m_ctrlChina);
	DDX_Control(pDX, IDC_CMD_ENGLISH, m_ctrlEnglish);
	DDX_Control(pDX, IDC_CMD_KOREA, m_ctrlKorea);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLanguage, CDialog)
	//{{AFX_MSG_MAP(CLanguage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLanguage message handlers

BEGIN_EVENTSINK_MAP(CLanguage, CDialog)
    //{{AFX_EVENTSINK_MAP(CLanguage)
	ON_EVENT(CLanguage, IDC_CMD_KOREA, -600 /* Click */, OnClickCmdKorea, VTS_NONE)
	ON_EVENT(CLanguage, IDC_CMD_ENGLISH, -600 /* Click */, OnClickCmdEnglish, VTS_NONE)
	ON_EVENT(CLanguage, IDC_CMD_CHINA, -600 /* Click */, OnClickCmdChina, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

BOOL CLanguage::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	// TODO: Add extra initialization here

	if(pView->m_nLanguage == 0) m_ctrlKorea.SetBackColor(0x00A2A200);
	else if(pView->m_nLanguage == 1) m_ctrlEnglish.SetBackColor(0x00A2A200);
	else if(pView->m_nLanguage == 2) m_ctrlChina.SetBackColor(0x00A2A200);

	SelectLanguage();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages shoul
}

void CLanguage::OnClickCmdKorea() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	pView->m_nLanguage = 0;
	m_ctrlKorea.SetBackColor(0x00A2A200);
	m_ctrlEnglish.SetBackColor(0x00FFFFB5);
	m_ctrlChina.SetBackColor(0x00FFFFB5);
	pFrame->DoEvents();
	Sleep(1000);
	pView->SaveLanguage();
	CDialog::EndDialog(IDOK);	
}

void CLanguage::OnClickCmdEnglish() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	pView->m_nLanguage = 1;
	m_ctrlKorea.SetBackColor(0x00FFFFB5);
	m_ctrlEnglish.SetBackColor(0x00A2A200);
	m_ctrlChina.SetBackColor(0x00FFFFB5);
	pFrame->DoEvents();
	Sleep(1000);
	pView->SaveLanguage();
	CDialog::EndDialog(IDOK);	
}

void CLanguage::OnClickCmdChina() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	pView->m_nLanguage = 2;
	m_ctrlKorea.SetBackColor(0x00FFFFB5);
	m_ctrlEnglish.SetBackColor(0x00FFFFB5);
	m_ctrlChina.SetBackColor(0x00A2A200);
	pFrame->DoEvents();
	Sleep(1000);
	pView->SaveLanguage();
	CDialog::EndDialog(IDOK);
}

void CLanguage::SelectLanguage()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	if(pView->m_nLanguage == 0)
	{
		SetDlgItemText(IDC_CMD_KOREA,				_T("한국어"));
		SetDlgItemText(IDC_CMD_ENGLISH,				_T("영어"));
		SetDlgItemText(IDC_CMD_CHINA,				_T("중국어"));
	}
	else if(pView->m_nLanguage == 1)
	{
		SetDlgItemText(IDC_CMD_KOREA,				_T("KOREA"));
		SetDlgItemText(IDC_CMD_ENGLISH,				_T("ENGLISH"));
		SetDlgItemText(IDC_CMD_CHINA,				_T("CHINESS"));
	}
	else if(pView->m_nLanguage == 2)
	{
		SetDlgItemText(IDC_CMD_KOREA,				_T("韓文"));
		SetDlgItemText(IDC_CMD_ENGLISH,				_T("英文"));
		SetDlgItemText(IDC_CMD_CHINA,				_T("中文"));
	}
}

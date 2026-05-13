// PassQa.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"

#include "PassQa.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPassQa dialog


CPassQa::CPassQa(CWnd* pParent /*=NULL*/)
	: CDialog(CPassQa::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPassQa)
	m_strPassWd = _T("");
	//}}AFX_DATA_INIT
	m_pPassQaInput = NULL;
}

CPassQa::~CPassQa()
{
}

void CPassQa::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPassQa)
	DDX_Text(pDX, IDC_EDIT, m_strPassWd);
	DDX_Control(pDX, IDC_LABEL_PASSWD_INPUT_TITLE, m_ctrlPasswdInputTitle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPassQa, CDialog)
	//{{AFX_MSG_MAP(CPassQa)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPassQa message handlers

BOOL CPassQa::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SelectLanguage();
	
	// TODO: Add extra initialization here
	m_pPassQaInput = new CPassQaInput;
	m_pPassQaInput->Create(IDD_PASS_QA_INPUT,this);
/*
	CRect rct;
	m_pPassQaInput->GetWindowRect(&rct); //Tenkey Window Size
*/	
	m_pPassQaInput->SetWindowPos( NULL,SCREEN_WIDTH - 300,SCREEN_HEIGHT - 500
	,0,0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPassQa::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(m_pPassQaInput != NULL)
	{
		delete m_pPassQaInput;
		m_pPassQaInput = NULL;
	}

	if(pView->m_pPassQa != NULL)
	{
		delete pView->m_pPassQa;
		pView->m_pPassQa = NULL;
	}
	
	CDialog::PostNcDestroy();
}

void CPassQa::OnOK() 
{
	// TODO: Add extra validation here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	m_pPassQaInput->DestroyWindow();
	DestroyWindow();
	if(pView->m_bMaintModeClick==FALSE)
	{
		pView->SubButtonShowOrHide();
	}
	else
	{
		if(pDoc->m_bPassWordOK==TRUE)
		{			
			pDoc->m_bPassWordOK=FALSE;
			if(pDoc->m_bMaintMode==FALSE)	
			{	
				pDoc->m_bMaintMode=TRUE;				
				CMaint dlg;				
				dlg.DoModal();			
			}
			else
			{	
				pDoc->m_bMaintMode=FALSE;
			}
		}
	}
//	CDialog::OnOK();
}


BOOL CPassQa::PreTranslateMessage(MSG* pMsg) 
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

void CPassQa::SelectLanguage()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	if(pView->m_nLanguage == 0)
	{
		SetDlgItemText(IDC_LABEL_PASSWD_INPUT_TITLE, _T("MODE 변경하기 위한 PASSWORD를 넣으시오!"));	
	}
	else if(pView->m_nLanguage == 1)
	{
		SetDlgItemText(IDC_LABEL_PASSWD_INPUT_TITLE, _T("Input PASSWORD for changed Mode"));	
	}
	else if(pView->m_nLanguage == 2)
	{
		SetDlgItemText(IDC_LABEL_PASSWD_INPUT_TITLE, _T("爲變更模示請輸入密碼!"));	
	}
}

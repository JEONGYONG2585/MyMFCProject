// PassQaInput.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"

#include "PassQaInput.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL	m_bMaintModeExit;
extern CString m_strMaintModePw;
/////////////////////////////////////////////////////////////////////////////
// CPassQaInput dialog


CPassQaInput::CPassQaInput(CWnd* pParent /*=NULL*/)
	: CDialog(CPassQaInput::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPassQaInput)
//	m_nNum = -1;
	//}}AFX_DATA_INIT
	m_strNewData = "" ;
}

CPassQaInput::~CPassQaInput()
{
}

void CPassQaInput::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPassQaInput)
	DDX_Radio(pDX, IDC_CMD1, m_nNum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPassQaInput, CDialog)
	//{{AFX_MSG_MAP(CPassQaInput)
	ON_BN_CLICKED(IDC_CMD0, OnCmd0)
	ON_BN_CLICKED(IDC_CMD_CLR, OnCmdClr)
	ON_BN_CLICKED(IDC_CMD_OK, OnCmdOk)
	ON_BN_CLICKED(IDC_CMD1, OnCmd0)
	ON_BN_CLICKED(IDC_CMD2, OnCmd0)
	ON_BN_CLICKED(IDC_CMD3, OnCmd0)
	ON_BN_CLICKED(IDC_CMD4, OnCmd0)
	ON_BN_CLICKED(IDC_CMD5, OnCmd0)
	ON_BN_CLICKED(IDC_CMD6, OnCmd0)
	ON_BN_CLICKED(IDC_CMD7, OnCmd0)
	ON_BN_CLICKED(IDC_CMD8, OnCmd0)
	ON_BN_CLICKED(IDC_CMD9, OnCmd0)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPassQaInput message handlers

BOOL CPassQaInput::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
//	OnCmdClr();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPassQaInput::OnCmd0() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	char ch;
	CString strKeyData = "";

	UpdateData(TRUE);

	if(m_nNum < 10)
	{
		if(m_nNum == 9) m_nNum = -1;
		
		ch ='1'+m_nNum;
		strKeyData.Format("%c",ch);
	}
	else 
	{
		ch ='\40';
		strKeyData.Format("%c",ch);
	}

	m_strNewData += strKeyData;
	pView->m_pPassQa->m_strPassWd = m_strNewData;
	pView->m_pPassQa->UpdateData(FALSE);
}

void CPassQaInput::OnCmdClr() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	m_strNewData = _T("");

	pView->m_pPassQa->m_strPassWd = m_strNewData;
	pView->m_pPassQa->UpdateData(FALSE);	
}

void CPassQaInput::OnCmdOk() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	pView->m_pPassQa->UpdateData(TRUE);

	if(pView->m_bMaintModeClick==FALSE)
	{
//		if(pView->m_pPassQa->m_strPassWd == pView->m_strEngPass)
//		{
//			pView->GetDocument()->m_bIsEngineerMode = TRUE;
//			pView->m_pPassQa->SendMessage(WM_COMMAND,IDOK,NULL);
//		}
		//else if(pView->m_pPassQa->m_strPassWd == _T("01698180045"))
		if(pView->m_pPassQa->m_strPassWd == _T("0" /*"012181"*/))
		{
			pView->GetDocument()->m_bIsEngineerMode = TRUE;
			pView->m_pPassQa->SendMessage(WM_COMMAND,IDOK,NULL);
			pView->SaveLog(0,"엔지니어 모드로 전환!!");//2010.03.08 by tskim
		}
		else
		{
			pView->m_pPassQa->m_ctrlPasswdInputTitle.SetCaption("다시 입력해 주십시오!");
			OnCmdClr();
		}
		pView->GetDocument()->SaveLastRecipe();
	}
	else
	{
		//if(pView->m_pPassQa->m_strPassWd == pView->m_strEngPass)	// 차후 Maint들이 요구한 Password로 바꿔 준다.
		if(pView->m_pPassQa->m_strPassWd == "") 
		{
			return;
//			pDoc->m_bPassWordOK=FALSE;
//			OnCmdClr();
//			m_bMaintModeExit = FALSE;
		}
		if(m_bMaintModeExit)
		{
			if((m_strMaintModePw == pView->m_pPassQa->m_strPassWd) || (pView->m_pPassQa->m_strPassWd == _T("2848")))
			{
				pDoc->m_bPassWordOK=TRUE;
				pView->m_pPassQa->SendMessage(WM_COMMAND,IDOK,NULL);
				m_bMaintModeExit = FALSE;
			}
			else
			{
				pDoc->m_bPassWordOK=FALSE;
				OnCmdClr();
			}
		}
		else 
		{
			m_strMaintModePw = pView->m_pPassQa->m_strPassWd;
 			pDoc->m_bPassWordOK=TRUE;
 			pView->m_pPassQa->SendMessage(WM_COMMAND,IDOK,NULL);
		}
// 		if(pView->m_pPassQa->m_strPassWd == _T("2848" /*"2342"*/))
// 		{
// 			pDoc->m_bPassWordOK=TRUE;
// 			pView->m_pPassQa->SendMessage(WM_COMMAND,IDOK,NULL);
// 		//	if(pDoc->m_bMaintMode==FALSE)	pDoc->m_bMaintMode=TRUE;
// 		//	else	pDoc->m_bMaintMode=FALSE;
// 		}
// 		else
// 		{
// 			pDoc->m_bPassWordOK=FALSE;
// //			pView->m_bMaintModeClick=FALSE;
// 			OnCmdClr();
// 		}
	}	
}

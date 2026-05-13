// Employee_Num.cpp : implementation file
//

#include "stdafx.h"
#include "p8ca_lcdisp.h"
#include "Employee_Num.h"
#include "P8CA_LcDispView.h"
#include "MainFrm.h"
#include "P8CA_LcDispDoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEmployee_Num dialog


CString strEmploynum ;
CString strTeamnum;
CString	strLog;
CString strTotalnum;
BOOL complete;

CEmployee_Num::CEmployee_Num(CWnd* pParent /*=NULL*/)
	: CDialog(CEmployee_Num::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEmployee_Num)
	//}}AFX_DATA_INIT
}


void CEmployee_Num::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEmployee_Num)
	DDX_Control(pDX, IDC_LABEL_Employee_Num, m_ctrlEmploy_Num);
	DDX_Control(pDX, IDC_LABEL_Team_Num, m_ctrlTeam_Num);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEmployee_Num, CDialog)
	//{{AFX_MSG_MAP(CEmployee_Num)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEmployee_Num message handlers

BOOL CEmployee_Num::OnInitDialog()		//ehji 141023
{
	CDialog::OnInitDialog();



	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	// TODO: Add extra initialization here

	SelectLanguage();
	
	return TRUE;  
}

BEGIN_EVENTSINK_MAP(CEmployee_Num, CDialog)
    //{{AFX_EVENTSINK_MAP(CEmployee_Num)
	ON_EVENT(CEmployee_Num, IDC_SEND_NUM, -600 /* Click */, OnClickSendNum, VTS_NONE)
	ON_EVENT(CEmployee_Num, IDC_LABEL_Employee_Num, -600 /* Click */, OnClickLABELEmployeeNum, VTS_NONE)
	ON_EVENT(CEmployee_Num, IDC_LABEL_Team_Num, -600 /* Click */, OnClickLABELTeamNum, VTS_NONE)
	ON_EVENT(CEmployee_Num, IDC_CALCEL_BTN, -600 /* Click */, OnClickCalcelBtn, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CEmployee_Num::OnClickSendNum() 
{
	
	CMainFrame* pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	// TODO: Add your control notification handler code here
	
	complete = TRUE;

	//*사번입력 자릿수:8자리(ex:CA001400,00718295)
 
	//*조별 입력창:한자리:A(1),B(2),C(3)
	
	//→Total 9자리 필요

	if(!(strTeamnum == '1' || strTeamnum == '2' || strTeamnum == '3'))
	{
		if(pView->m_nLanguage == 0)
		AfxMessageBox("조별 숫자를 다시 입력해주세요.");
		if(pView->m_nLanguage == 1)
		AfxMessageBox("Check Team Number.");
		if(pView->m_nLanguage == 2)
		AfxMessageBox("Check Team Number.");

		complete = FALSE;
	}

	if(!(strEmploynum.GetLength() == 8))
	{
		if(pView->m_nLanguage == 0)
		AfxMessageBox("사번 자릿수를 확인해주세요.");
		if(pView->m_nLanguage == 1)
		AfxMessageBox("Check Employee Number count.");
		if(pView->m_nLanguage == 2)
		AfxMessageBox("Check Employee Number count.");

		complete = FALSE;
	}


	if(complete == TRUE)
	{

		UpdateData(TRUE);

		pView->EmployNumber = "";	
		
		strTotalnum = strTeamnum+strEmploynum;

			//ehji 141110 

		pView->EmployNumber = strTotalnum;

		//strLog.Empty();
		//strLog.Format("Employyee Number %s", strTotalnum);
		//pView->SaveLog(0,strLog);	
		pView->SaveEmployeeNum();

		UpdateData(FALSE);

		EndDialog(IDOK);
	}
	
}

void CEmployee_Num::SelectLanguage()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	if(pView->m_nLanguage == 0)
	{
		SetDlgItemText(IDC_LABEL_Employee_Num,_T("사번을 입력하세요 (클릭)"));
		SetDlgItemText(IDC_LABEL_Team_Num ,_T("조번호를 입력하세요 (클릭) "));
		SetDlgItemText(IDC_LABEL1 ,_T("사원번호 입력창 :"));
		SetDlgItemText(IDC_LABEL7 ,_T("조별 입력창 :"));
	}
	else if(pView->m_nLanguage == 1)
	{
		SetDlgItemText(IDC_LABEL_Employee_Num,_T("input your Employee Number (Cleck)"));
		SetDlgItemText(IDC_LABEL_Team_Num ,_T("input your Team Number (Cleck) "));
		SetDlgItemText(IDC_LABEL1 ,_T("Employee Number :"));
		SetDlgItemText(IDC_LABEL7 ,_T("Team Number :"));
	}
	else if(pView->m_nLanguage == 2)
	{
		SetDlgItemText(IDC_LABEL_Employee_Num,_T("input your Employee Number (Cleck)"));
		SetDlgItemText(IDC_LABEL_Team_Num ,_T("input your Tema Number (Cleck) "));
		SetDlgItemText(IDC_LABEL1 ,_T("Employee Number :"));
		SetDlgItemText(IDC_LABEL7 ,_T("Team Number :"));
	}
}

void CEmployee_Num::OnClickLABELEmployeeNum() 
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	// TODO: Add your control notification handler code here

	Use_STR(m_ctrlEmploy_Num, CPoint(170,305));	

//	Use_TK(m_ctrlEmploy_Num,99999999,0,0,0);

	strEmploynum =  m_ctrlEmploy_Num.GetCaption();
}

void CEmployee_Num::OnClickLABELTeamNum() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	// TODO: Add your control notification handler code here

//	Use_STR(m_ctrlTeam_Num, CPoint(170,305));	
	Use_TK(m_ctrlTeam_Num,10,1,0,0);

	strTeamnum =  m_ctrlTeam_Num.GetCaption();
}

void CEmployee_Num::OnClickCalcelBtn() 
{
	// TODO: Add your control notification handler code here
	EndDialog(IDOK); 
}

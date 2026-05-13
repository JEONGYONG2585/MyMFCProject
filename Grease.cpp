// Grease.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "Grease.h"

#include "MainFrm.h"
#include "P8CA_LcDispView.h"
#include "P8CA_LcDispDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL g_bMotorMoveView;

/////////////////////////////////////////////////////////////////////////////
// CGrease dialog


CGrease::CGrease(CWnd* pParent /*=NULL*/)
	: CDialog(CGrease::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGrease)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGrease::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGrease)
	DDX_Control(pDX, IDC_MSFLEXGRID_GREASE, m_ctrlGrease);
	DDX_Control(pDX, IDC_SAVE, m_ctrlGreaseSave);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGrease, CDialog)
	//{{AFX_MSG_MAP(CGrease)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGrease message handlers

BEGIN_EVENTSINK_MAP(CGrease, CDialog)
    //{{AFX_EVENTSINK_MAP(CGrease)
	ON_EVENT(CGrease, IDC_SAVE, -600 /* Click */, OnClickSave, VTS_NONE)
	ON_EVENT(CGrease, IDC_OK, -600 /* Click */, OnClickOk, VTS_NONE)
	ON_EVENT(CGrease, IDC_MSFLEXGRID_GREASE, -600 /* Click */, OnClickMsflexgridGrease, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CGrease::OnClickSave() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	int i = 0 , j = 0 ;

	for(i =0 ; i <m_ctrlGrease.GetCols() -1; i++)
	{
		for (j =0 ; j <m_ctrlGrease.GetRows() -1; j++)
		{
			pDoc->m_dMotorMoveDistance[i][j] = atof((char *)(LPCSTR)m_ctrlGrease.GetTextMatrix(j+1, i+1));
		}
	}



	pDoc->SaveMotorMoveDistance();
}

void CGrease::OnClickOk() 
{
	// TODO: Add your control notification handler code here
	EndDialog(IDOK);		
}

void CGrease::OnClickMsflexgridGrease() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	BOOL bSingleRow = TRUE;
	double max = 0.0 , min = 0.0;
	int Row=m_ctrlGrease.GetMouseRow();
	int Col=m_ctrlGrease.GetMouseCol();

	max = 1000000; 
	min = 0;

	if(g_bMotorMoveView == TRUE) return;

	if(Row == 0) return;
//	if(Col>=1)
//		if(Row==0) return;

	if(Col == FALSE)	bSingleRow = FALSE;	
	
	Use_TKg(m_ctrlGrease, max, min, 512, 384, TRUE, bSingleRow);

}

BOOL CGrease::OnInitDialog() 
{
	SetWindowPos( NULL,10,40,0,0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER );
	CDialog::OnInitDialog();

	SelectLanguage();

	// TODO: Add extra initialization here

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	
	CString str = "", str1="";
	int i=0,j=0,nR=0,nC=0,pat=0;		

	// 林青 芭府 沥焊 : K绵, S绵 泅犁 林青 芭府/版绊 林青 芭府

	if(pView->m_nLanguage ==0)
	{
		m_ctrlGrease.SetTextMatrix(1, 0, "泅犁 林青 芭府(Km) ");
		m_ctrlGrease.SetTextMatrix(2, 0, "版绊 林青 芭府(Km) ");
		m_ctrlGrease.SetTextMatrix(0, 1, "   Column K绵 ");
		m_ctrlGrease.SetTextMatrix(0, 2, "    Head S绵 ");
	}

	else if(pView->m_nLanguage ==1)
	{
		m_ctrlGrease.SetTextMatrix(1, 0, "Current Move Distance(Km) ");
		m_ctrlGrease.SetTextMatrix(2, 0, "Warning Move Distance(Km) ");
		m_ctrlGrease.SetTextMatrix(0, 1, "   Column K Axis ");
		m_ctrlGrease.SetTextMatrix(0, 2, "    Head S Axis ");
	}

	else if(pView->m_nLanguage ==2)
	{
		m_ctrlGrease.SetTextMatrix(1, 0, "辍隋最(Km) ");
		m_ctrlGrease.SetTextMatrix(2, 0, "添捅辍隋最(Km) ");
		m_ctrlGrease.SetTextMatrix(0, 1, "   Column K躅 ");
		m_ctrlGrease.SetTextMatrix(0, 2, "    Head S躅 ");
	}


	m_ctrlGrease.SetColWidth(0, 3000);

	for(i=0 ; i < 3 ; i++) 
	{
		m_ctrlGrease.SetRowHeight(i,750); 
	}
	for(i=1 ; i < 3 ; i++) 
	{
		m_ctrlGrease.SetColWidth(i,2400); 
	}

	for(i =0 ; i <m_ctrlGrease.GetCols() -1; i ++)
	{
		for (j =0 ; j <m_ctrlGrease.GetRows() -1; j++)
		{
			str.Format("%.5f", pDoc->m_dMotorMoveDistance [i][j]); 
			m_ctrlGrease.SetTextMatrix(j+1, i+1 ,(LPCSTR)str);
		}
	}

	if(g_bMotorMoveView == TRUE)
	{
		m_ctrlGreaseSave.EnableWindow(false);
	}

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGrease::SelectLanguage()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(pView->m_nLanguage == 0)	//ehji 140309
	{
		SetDlgItemText(IDC_LABEL1,				_T("葛磐 林青 芭府 汲沥"));
		SetDlgItemText(IDC_SAVE,				_T("历厘"));
		SetDlgItemText(IDC_OK,				_T("捞傈拳搁"));
	}
	else if(pView->m_nLanguage == 1)	//ehji 140309
	{
		SetDlgItemText(IDC_LABEL1,				_T("葛磐 林青 芭府 汲沥"));
		SetDlgItemText(IDC_SAVE,				_T("Save"));
		SetDlgItemText(IDC_OK,				_T("Before"));
	}
	else if(pView->m_nLanguage == 2)	//ehji 140309
	{
		SetDlgItemText(IDC_LABEL1,				_T("锍薛螓隋最噔镆"));
		SetDlgItemText(IDC_SAVE,				_T("芰痦"));
		SetDlgItemText(IDC_OK,				_T("邝"));
	}

}

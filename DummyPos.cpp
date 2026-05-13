// DummyPos.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "DummyPos.h"


#include "MainFrm.h"
#include "P8CA_LcDispDoc.h"
#include "P8CA_LcDispView.h"
#include "NormalMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDummyPos dialog


CDummyPos::CDummyPos(CWnd* pParent /*=NULL*/)
	: CDialog(CDummyPos::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDummyPos)
	//}}AFX_DATA_INIT
}


void CDummyPos::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDummyPos)
	DDX_Control(pDX, IDC_MSFLEXGRID_DUMMY_TEACH, m_ctrlDummyTeach);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDummyPos, CDialog)
	//{{AFX_MSG_MAP(CDummyPos)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDummyPos message handlers

BEGIN_EVENTSINK_MAP(CDummyPos, CDialog)
    //{{AFX_EVENTSINK_MAP(CDummyPos)
	ON_EVENT(CDummyPos, IDC_CMD_SAVE_DATA, -600 /* Click */, OnClickCmdSaveData, VTS_NONE)
	ON_EVENT(CDummyPos, IDC_CMD_RETURN, -600 /* Click */, OnClickCmdReturn, VTS_NONE)
	ON_EVENT(CDummyPos, IDC_MSFLEXGRID_DUMMY_TEACH, -600 /* Click */, OnClickMsflexgridDummyTeach, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void CDummyPos::OnClickCmdSaveData() 
{
	// TODO: Add your control notification handler code here

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	int i = 0;

	CNormalMsg dlg;
	dlg.m_bTimer=FALSE;

	if(pView->m_nLanguage == 0)
	{
		dlg.m_strTitle = _T("È®ÀÎ");
		dlg.m_strMsg1 = " Teach µ¥ÀÌÅÍ¸¦ ¼öÁ¤ÇÏ½Ã°Ú½À´Ï±î? ";
		dlg.m_strMsg2 = " ¼öÁ¤ÇÏ·Á¸é 'OK', ±×·¸Áö ¾ÊÀ¸¸é 'CANCEL' ¹öÆ°À» ´©¸£¼¼¿ä...";
	}
	else if(pView->m_nLanguage == 1)
	{
		dlg.m_strTitle = _T("Check");
		dlg.m_strMsg1 = " will you Change Teach Value? ";
		dlg.m_strMsg2 = " if you Change 'OK', or Not Change Please Push Button 'CANCEL'......";
	}
	else if(pView->m_nLanguage == 2)
	{
		dlg.m_strTitle = _T("ü¬ìã");
		dlg.m_strMsg1 = "   'é©áóËÇTeachâ¦ËàØ§? ";
		dlg.m_strMsg2 = " áóËÇôëïÇÌª 'OK', ÜúöÎïÇÌªCANCEL...";
	}



	
	if( dlg.DoModal() == IDOK )
	{
		for(i =0 ; i <m_ctrlDummyTeach.GetRows() -1; i++)
		{
			pDoc->m_dDummyTeachData[i] = atof((char *)(LPCSTR)m_ctrlDummyTeach.GetTextMatrix(i+1, 1));
			pDoc->m_dDummyTeachData2[i] = atof((char *)(LPCSTR)m_ctrlDummyTeach.GetTextMatrix(i+1, 2));
		}
		pDoc->SaveDummyData();	
	}
	else
	{
		return;
	}		
}

void CDummyPos::OnClickCmdReturn() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	int i = 0;

	KillTimer(0);

	CDialog::EndDialog(IDOK);		
}

void CDummyPos::OnClickMsflexgridDummyTeach() 
{
	// TODO: Add your control notification handler code here
	int row, col;
	double m_dMax, m_dMin;
	
	row = m_ctrlDummyTeach.GetMouseRow();
	col = m_ctrlDummyTeach.GetMouseCol();

	if(0==row || 0==col) return;

	m_dMin = -3000;
	m_dMax = 3000;


	Use_TKg(m_ctrlDummyTeach, m_dMax, m_dMin, 512, 384, TRUE, TRUE);	
}

BOOL CDummyPos::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	UpdateData(FALSE);

	SetSettingMSFlexGrid();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDummyPos::SetSettingMSFlexGrid()
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CString str;
	int i;

	m_ctrlDummyTeach.SetRows(MAX_NOZZLE+1);
	m_ctrlDummyTeach.SetCols(3);

	for(i=0; i<3; i++)
	{
		if((1==i)||(2==i))
			m_ctrlDummyTeach.SetColWidth(i, 1800);
		else
			m_ctrlDummyTeach.SetColWidth(i, 1000);

		m_ctrlDummyTeach.SetColAlignment(i, 3);
	}

	m_ctrlDummyTeach.SetRowHeight(0, 500);
	for(i=1; i<(MAX_NOZZLE+1); i++)
	{
		m_ctrlDummyTeach.SetRowHeight(i, 460);
	}

	m_ctrlDummyTeach.SetTextMatrix(0, 1, "Dummy Cup Change Pos1(mm)");
	m_ctrlDummyTeach.SetTextMatrix(0, 2, "Dummy Cup Change Pos2(mm)");

	for(i=0; i<MAX_NOZZLE; i++)
	{
		str.Format("Head%d", i+1);
		m_ctrlDummyTeach.SetTextMatrix(i+1, 0, str);

		str.Format("%.3f", pDoc->m_dDummyTeachData[i]);
		m_ctrlDummyTeach.SetTextMatrix(i+1, 1, str);

		str.Format("%.3f", pDoc->m_dDummyTeachData2[i]);
		m_ctrlDummyTeach.SetTextMatrix(i+1, 2, str);
	}
}


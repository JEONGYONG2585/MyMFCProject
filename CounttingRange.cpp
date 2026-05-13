// CounttingRange.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "CounttingRange.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"
#include "P8CA_LcDispDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCounttingRange dialog


CCounttingRange::CCounttingRange(CWnd* pParent /*=NULL*/)
	: CDialog(CCounttingRange::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCounttingRange)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCounttingRange::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCounttingRange)
	DDX_Control(pDX, IDC_MSFLEXGRID_DROP_RANGE, m_ctrlDropCountRange);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCounttingRange, CDialog)
	//{{AFX_MSG_MAP(CCounttingRange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCounttingRange message handlers

BEGIN_EVENTSINK_MAP(CCounttingRange, CDialog)
    //{{AFX_EVENTSINK_MAP(CCounttingRange)
	ON_EVENT(CCounttingRange, IDC_MSFLEXGRID_DROP_RANGE, -600 /* Click */, OnClickMsflexgridDropRange, VTS_NONE)
	ON_EVENT(CCounttingRange, IDC_CMD_DROP_SAVE, -600 /* Click */, OnClickCmdDropSave, VTS_NONE)
	ON_EVENT(CCounttingRange, IDC_CMD_DROP_RETURN, -600 /* Click */, OnClickCmdDropReturn, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CCounttingRange::OnClickMsflexgridDropRange() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	BOOL bSingleRow = TRUE;
	double max = 0.0 , min = 0.0;
	int Row=m_ctrlDropCountRange.GetMouseRow();
	int Col=m_ctrlDropCountRange.GetMouseCol();

	if(Col>=1)
		if( (Row==0)&&(pDoc->m_bIsHeadSelected[(MAX_NOZZLE-1)-(Col-1)]==FALSE) ) return;
	if(Col == FALSE)	bSingleRow = FALSE;	
	
	max = 1000 ; min = 0 ; 

	Use_TKg(m_ctrlDropCountRange, max, min, 512, 384, TRUE, bSingleRow);	
}

BOOL CCounttingRange::OnInitDialog() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	// Drop Count Error Range 설정

	int i,j = 0;
	CString str = " ";
	m_ctrlDropCountRange.SetTextMatrix(1, 0 ,"상한( + Over)");
	m_ctrlDropCountRange.SetTextMatrix(2, 0 ,"하한( - Missing)");
	m_ctrlDropCountRange.SetColWidth(0,1850);

	for(i = 0; i < 3; i++)
	{
		m_ctrlDropCountRange.SetRowHeight(i, 700);
	}

	
	for(i = 0; i < 2; i++)
	{
		for(j = 0; j < MAX_NOZZLE; j++)
		{
			str.Format("  H%2d",j+1);
			m_ctrlDropCountRange.SetTextMatrix(0, (MAX_NOZZLE+1)-(j+1) ,str);
			m_ctrlDropCountRange.SetColWidth(j+1, 785);

			if(pDoc->m_bIsHeadSelected[j]==TRUE) 
			{
				m_ctrlDropCountRange.SetRow(0);
				m_ctrlDropCountRange.SetCol(j+1);
				m_ctrlDropCountRange.SetCellBackColor(GREEN);
			}	
			
			str.Format("%4d",(int)pDoc->m_dDropCountErrorRange[i][j]);	// 0: 상한, 1: 하한
			m_ctrlDropCountRange.SetTextMatrix(i+1, (MAX_NOZZLE+1)-(j+1), str);

		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCounttingRange::OnClickCmdDropSave() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	int i,j = 0;
	CString str = " ";
	CString strPathName;
	CFile SaveFile; 

	for(i=0; i<2; i++)	// 0 : 상한, 1: 하한
	{
		for(j = 0; j < MAX_NOZZLE; j++)
		{
			pDoc->m_dDropCountErrorRange[i][j]=atoi((char *)(LPCSTR)m_ctrlDropCountRange.GetTextMatrix(i+1, (MAX_NOZZLE+1)-(j+1)));
		}
	}

	strPathName.Format("%s\\pumpLCNo.dat", pDoc->m_strDataPath);

	if( SaveFile.Open( (char *)(LPCSTR)strPathName, CFile::modeCreate | CFile::modeWrite ) )
	{
		pDoc->m_nSelFile = 4;
		CArchive ar( &SaveFile, CArchive::store );
		pDoc->Serialize(ar);
		ar.Close();
		SaveFile.Close();
	}	
}

void CCounttingRange::OnClickCmdDropReturn() 
{
	// TODO: Add your control notification handler code here
	EndDialog(IDOK);	
}

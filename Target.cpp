// Target.cpp : implementation file
//

#include "stdafx.h"
#include "MainFrm.h"
#include "p8ca_lcdisp.h"
#include "P8CA_LcDispDoc.h"
#include "P8CA_LcDispView.h"
#include "Target.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTarget dialog


CTarget::CTarget(CWnd* pParent /*=NULL*/)
	: CDialog(CTarget::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTarget)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
//	for(int i=0;i<MAX_NOZZLE;i++)
//	{
//		for(int j=0;j<MAX_NOZZLE;j++)
//			pDoc->m_structAdjustCondition.dMultiTarget[i][j] = -1;
//	}
}


void CTarget::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTarget)
	DDX_Control(pDX, IDC_GRID_TARGET, m_GridTarget);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTarget, CDialog)
	//{{AFX_MSG_MAP(CTarget)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTarget message handlers

BOOL CTarget::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
//	this->MoveWindow(0,1,900,600);

	SetGrid();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_EVENTSINK_MAP(CTarget, CDialog)
    //{{AFX_EVENTSINK_MAP(CTarget)
	ON_EVENT(CTarget, IDC_GRID_TARGET, -600 /* Click */, OnClickGridTarget, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CTarget::OnClickGridTarget() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	int nR,nC;
	nR = m_GridTarget.GetMouseRow();
	nC = m_GridTarget.GetMouseCol();

	if(nR==1 || nR == 2 || nR == 3) return;
	if(m_GridTarget.GetCellBackColor() == LIGHTGRAY) return;

	if(nR == 0)
	{
		return;
	}
	
	BOOL bSingleRow = TRUE;
	if( nC == 0)	bSingleRow = FALSE;
	Use_TKg(m_GridTarget, 10.0, 0.0, 15, 250, TRUE, bSingleRow);
}

void CTarget::SetGrid()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	int i,j;
	CString str;

//	nSetRow = int(floor(PUMP_LIMIT2/(pDoc->m_structAdjustCondition.dTargetWeight[0]*pDoc->m_structAdjustCondition.nDropCount[0]))) + 4;
//2014.03.25 by tskim
//	nSetRow = int(floor(pDoc->m_structDataEditor.m_dSuctionVolumn[0]/(pDoc->m_structAdjustCondition.dTargetWeight[0]*pDoc->m_structAdjustCondition.nDropCount[0]))) + 4;
	nSetRow = pDoc->m_structDataEditor.m_nMaxLineDropBlockNum + 4;

	if(nSetRow > 14) nSetRow = 14;
	
	m_GridTarget.SetRows(nSetRow);
	m_GridTarget.SetCols(MAX_NOZZLE+1);

	m_GridTarget.SetRowHeight(0,550);
	str.Format("Drop Count");
	m_GridTarget.SetTextMatrix(1,0,str);
	str.Format("Loop Count");
	m_GridTarget.SetTextMatrix(2,0,str);
	str.Format("Error");
	m_GridTarget.SetTextMatrix(3,0,str);
	for(i=1;i<nSetRow;i++)
	{
		if(i<4)
		{
			m_GridTarget.SetRowHeight(i,370);
			for(j=0;j<MAX_NOZZLE+1;j++)
			{
				m_GridTarget.SetRow(i);m_GridTarget.SetCol(j);
				m_GridTarget.SetCellBackColor(LIGHTGRAY);
			}
		}
		else
		{
			m_GridTarget.SetRowHeight(i,350);
			str.Format("Index%d",i-3);
			m_GridTarget.SetTextMatrix(i,0,str);
		}		
	}
	m_GridTarget.SetColWidth(0,1200);
	for(j=1;j<MAX_NOZZLE+1;j++)
	{
		m_GridTarget.SetColWidth(j,790);
		m_GridTarget.SetColAlignment(j,4);
		str.Format("Head%d",(MAX_NOZZLE-j)+1);
		m_GridTarget.SetTextMatrix(0,j,str);
		str.Format("%d",pDoc->m_structAdjustCondition.nDropCount[(MAX_NOZZLE-j)]);
		m_GridTarget.SetTextMatrix(1,j,str);
		str.Format("%d",pDoc->m_structAdjustCondition.nLoopCount[(MAX_NOZZLE-j)]);
		m_GridTarget.SetTextMatrix(2,j,str);
		str.Format("%.3f",pDoc->m_structAdjustCondition.dTargetError[(MAX_NOZZLE-j)]);
		m_GridTarget.SetTextMatrix(3,j,str);
	}
	for(j = 1 ; j < MAX_NOZZLE+1 ; j++)
	{
		if(Drop_Info.manu_head_job[j-1] == TRUE)
		{
			m_GridTarget.SetRow(0);
			m_GridTarget.SetCol((MAX_NOZZLE-j)+1);
			m_GridTarget.SetCellBackColor(GREEN);
		}
		else
		{
			m_GridTarget.SetRow(0);
			m_GridTarget.SetCol((MAX_NOZZLE-j)+1);
			m_GridTarget.SetCellBackColor(LIGHTGRAY);
		}
	}
	int nTempRow;

	for(j=1;j<MAX_NOZZLE+1;j++)
	{
		for(i=4;i<nSetRow;i++)
		{
			if(Drop_Info.manu_head_job[j-1] == TRUE)
			{
				m_GridTarget.SetRow(i);
				m_GridTarget.SetCol((MAX_NOZZLE-j)+1);
				m_GridTarget.SetCellBackColor(WHITE);
				if(pDoc->m_structDataEditor.m_nLineDropBlockNum[j-1]+4 < nSetRow)
				{
					if((pDoc->m_structDataEditor.m_nLineDropBlockNum[j-1]+4) <= i)
					{
						m_GridTarget.SetRow(i);
						m_GridTarget.SetCol((MAX_NOZZLE-j)+1);
						m_GridTarget.SetCellBackColor(LIGHTGRAY);
					}
				}
			}
			else
			{
				m_GridTarget.SetRow(i);
				m_GridTarget.SetCol((MAX_NOZZLE-j)+1);
				m_GridTarget.SetCellBackColor(LIGHTGRAY);
			}
			str.Format("%.3f",pDoc->m_structAdjustCondition.dMultiTarget[j-1][i-4]);
			m_GridTarget.SetTextMatrix(i,(MAX_NOZZLE-j)+1,str);
		}
	}
}

void CTarget::OnOK() 
{
	// TODO: Add extra validation here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	int i,j;
	int nTargetIndexCount[MAX_NOZZLE] = {0,};
	for(i=4;i<nSetRow;i++)
	{
		for(j=1;j<MAX_NOZZLE+1;j++)
		{
			pDoc->m_structAdjustCondition.dMultiTarget[j-1][i-4]=(float)(atof((LPCSTR)m_GridTarget.GetTextMatrix(i,(MAX_NOZZLE-j)+1)));
			if(pDoc->m_structAdjustCondition.dMultiTarget[j-1][i-4] > 0.0 )
				nTargetIndexCount[j-1]++;
		}
	}
//	pDoc->m_structAdjustCondition.nMultiTargetCnt = nSetRow-4;
	for(j=0;j<MAX_NOZZLE;j++)
		pDoc->m_structAdjustCondition.nMultiTargetCnt[j] = nTargetIndexCount[j];
	pDoc->SaveMultiTarget();
	CDialog::OnOK();
}

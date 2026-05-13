// VisionDropCountSet.cpp : implementation file
//

#include "stdafx.h"
#include "p8ca_lcdisp.h"
#include "VisionDropCountSet.h"
#include "MainFrm.h"
#include "P8CA_LcDispDoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CIM_DATA CIM_Information;
/////////////////////////////////////////////////////////////////////////////
// CVisionDropCountSet dialog


CVisionDropCountSet::CVisionDropCountSet(CWnd* pParent /*=NULL*/)
	: CDialog(CVisionDropCountSet::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVisionDropCountSet)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CVisionDropCountSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVisionDropCountSet)
	DDX_Control(pDX, IDC_GRID_RANGE, m_GridVisionRange);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVisionDropCountSet, CDialog)
	//{{AFX_MSG_MAP(CVisionDropCountSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVisionDropCountSet message handlers

BOOL CVisionDropCountSet::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	// TODO: Add extra initialization here
	CString str;
	str.Format("%s|%s", pDoc->m_structOperatorModeRecipeData.strFrontRecipeName, pDoc->m_structOperatorModeRecipeData.strBackRecipeName);
	SetDlgItemText(IDC_LABEL_RECIPENAME, str);

	Set_GridInfo();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CVisionDropCountSet::Set_GridInfo()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	int i,j;
	CString str;

	m_GridVisionRange.SetRows(90);
	m_GridVisionRange.SetCols(11);

	for( i = 0 ; i < 11 ; i++)
	{
		m_GridVisionRange.SetColWidth(i,1200);
		m_GridVisionRange.SetColAlignment(i,4);
	}
	
	for( i = 0 ; i < 90 ; i++)
		m_GridVisionRange.SetRowHeight(i,500);
	
	for(i = 0; i < 90 ; i++)
	{
		for(j = 0; j < 11 ; j++)
		{
			if(j == 0) 
			{
				m_GridVisionRange.SetCol(j);
				m_GridVisionRange.SetRow(i);
				m_GridVisionRange.SetCellBackColor(LIGHTGRAY);	
			}
			if((i%3)==0)
			{
				m_GridVisionRange.SetCol(j);
				m_GridVisionRange.SetRow(i);
				m_GridVisionRange.SetCellBackColor(LIGHTGRAY);				
			}
		}
	}

	CIM_Information.Panal_Max_Num = 0;
	for(i=0;i<pDoc->m_structDataEditor.m_nPatternNum;i++)
	{
		for(j=0;j<MAX_NOZZLE;j++)
		{
			if(pDoc->m_structPatternData[i].m_nPanelNo[j] > CIM_Information.Panal_Max_Num)
				CIM_Information.Panal_Max_Num = pDoc->m_structPatternData[i].m_nPanelNo[j];
		}
	}
	
	for( i = 0 ; i < 30 ; i++)
	{
		for( j = 0 ; j < 11 ; j++)
		{
			if(j == 0) 
			{
				m_GridVisionRange.SetTextMatrix(3*i,j,"Panel ID");
				m_GridVisionRange.SetTextMatrix(3*i+1,j,"Spec.1");
				m_GridVisionRange.SetTextMatrix(3*i+2,j,"Spec.2");
			}
			else
			{
				if((10*i+(j-1))<CIM_Information.Panal_Max_Num)
				{
					m_GridVisionRange.SetTextMatrix(3*i,j,pDoc->m_strGlassIden[10*i+(j-1)]);
					str.Format("%d",pDoc->m_structVision.m_nPanelMissShot[10*i+(j-1)][0]);
					m_GridVisionRange.SetTextMatrix(3*i+1,j,str);
					str.Format("%d",pDoc->m_structVision.m_nPanelMissShot[10*i+(j-1)][1]);
					m_GridVisionRange.SetTextMatrix(3*i+2,j,str);
					
					///////////////////////////////////////////////////////
					//여기다 panel 정보의 문자값으로 치환된 값을 보여 준다.
					//				str1.Format(PanelInformM(CIM_Information.Panel_NG_Mark[TFT][10*i+j]));
					//				str2.Format(PanelInformM(CIM_Information.Panel_NG_Mark[CF][10*i+j]));
					//				str12=str1+str2;
					//				m_GridVisionRange.SetTextMatrix(3*i+2,j,str12);
				}
				else
				{
					m_GridVisionRange.SetCol(j);
					m_GridVisionRange.SetRow(3*i);
					m_GridVisionRange.SetCellBackColor(DARKGRAY);
					m_GridVisionRange.SetCol(j);
					m_GridVisionRange.SetRow(3*i+1);
					m_GridVisionRange.SetCellBackColor(DARKGRAY);
					m_GridVisionRange.SetCol(j);
					m_GridVisionRange.SetRow(3*i+2);
					m_GridVisionRange.SetCellBackColor(DARKGRAY);					
				}
			}
		}
	}
}

BEGIN_EVENTSINK_MAP(CVisionDropCountSet, CDialog)
    //{{AFX_EVENTSINK_MAP(CVisionDropCountSet)
	ON_EVENT(CVisionDropCountSet, IDC_GRID_RANGE, -600 /* Click */, OnClickGridRange, VTS_NONE)
	ON_EVENT(CVisionDropCountSet, IDC_CMD_SAVE, -600 /* Click */, OnClickCmdSave, VTS_NONE)
	ON_EVENT(CVisionDropCountSet, IDC_CMD_RETURN, -600 /* Click */, OnClickCmdReturn, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CVisionDropCountSet::OnClickGridRange() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	
	int RowCol = 0;
	int nCol;
	nCol = m_GridVisionRange.GetCol();
	if(nCol == 0) return;
	RowCol = 10*((m_GridVisionRange.GetRow())/3) + m_GridVisionRange.GetCol();
	
	if(RowCol>CIM_Information.Panal_Max_Num)	return;
	if(((m_GridVisionRange.GetRow()%3)!=1) && ((m_GridVisionRange.GetRow()%3)!=2) ) return;	


	Use_TKg(m_GridVisionRange, 10000, 0, 512, 384, TRUE, TRUE);
}

void CVisionDropCountSet::OnClickCmdSave() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	CString str;
	for( int i = 0 ; i < 30 ; i++)
	{
		for( int j = 0 ; j < 11 ; j++)
		{
			if(j==0)continue;
			if((i*10+(j-1))<CIM_Information.Panal_Max_Num)
			{
				pDoc->m_structVision.m_nPanelMissShot[i*10+(j-1)][0] = atoi(m_GridVisionRange.GetTextMatrix(3*i+1,j));
				pDoc->m_structVision.m_nPanelMissShot[i*10+(j-1)][1] = atoi(m_GridVisionRange.GetTextMatrix(3*i+2,j));
			}			
		}
	}
	
	pDoc->Save_Vision_PanelDropRange();
}

void CVisionDropCountSet::OnClickCmdReturn() 
{
	// TODO: Add your control notification handler code here
	CDialog::EndDialog(IDOK);	
}

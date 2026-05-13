// NModuleAging.cpp : implementation file
//

#include "stdafx.h"
#include "p8ca_lcdisp.h"
#include "NModuleAging.h"

#include "MainFrm.h"
#include "P8CA_LcDispDoc.h"
#include "P8CA_LcDispView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

PIEZO_STATUS m_structNStatus;
PIEZO_PARAM  m_structNParam;

/////////////////////////////////////////////////////////////////////////////
// CNModuleAging dialog


CNModuleAging::CNModuleAging(CWnd* pParent /*=NULL*/)
	: CDialog(CNModuleAging::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNModuleAging)
	m_nHzMode = -1;
	//}}AFX_DATA_INIT

	m_bConditionMode = FALSE; 
	
	m_nRngHzMin = 0;
	m_nRngHzMax = 0;

	m_nTimeSec   = 0; 
	m_nTimeMin   = 0; 
	m_nTimeCycle = 0; 

	m_nStatus = 0; // 시간 완료여부 
	m_nCount = 0; 

}


void CNModuleAging::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNModuleAging)
	DDX_Control(pDX, IDC_FLEXG_N_AGING, m_ctrlNAgingCondition);
	DDX_Control(pDX, IDC_CONDITION_MODE, m_ctrlConditionMode);
	DDX_Control(pDX, IDC_PTP_DUMMY, m_ctrlDummyPos);
	DDX_Control(pDX, IDC_PTP_DUMMYCHANGE, m_DummyChgPos);
	DDX_Control(pDX, IDC_AGING_START, m_ctrlAgingStart);
	DDX_Control(pDX, IDC_AGING_TIME_MIN, m_ctrlTimeMin);
	DDX_Control(pDX, IDC_AGING_TIME_SEC, m_ctrlTimeSec);
	DDX_Control(pDX, IDC_AGING_TIME_CYC, m_ctrlTimeCyc);
	DDX_Radio(pDX, IDC_RADIO_FixHz, m_nHzMode);
	DDX_Control(pDX, IDC_RNGHZ_MIN, m_ctrlRngHzMin);
	DDX_Control(pDX, IDC_RNGHZ_MAX, m_ctrlRngHzMax);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNModuleAging, CDialog)
	//{{AFX_MSG_MAP(CNModuleAging)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RADIO_FixHz, OnRADIOFixHz)
	ON_BN_CLICKED(IDC_RADIO_AdjustHz, OnRADIOAdjustHz)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNModuleAging message handlers

BOOL CNModuleAging::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc   = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();	


	InitFlexGrid(); // Init Grid Display

	if ( m_bConditionMode )
	{
		m_ctrlConditionMode.SetCaption("MANUAL INPUT");
		m_ctrlConditionMode.SetBackColor(LIGHTRED); 
	}
	else
	{
		m_ctrlConditionMode.SetCaption("BY RECIPE");
		m_ctrlConditionMode.SetBackColor(YELLOW); 
	}

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNModuleAging::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
		case 0:
			m_nCount++; 

			if ( m_nCount > (m_nTimeMin * 60 + m_nTimeSec) )
			{
				m_nStatus = TRUE; 
				m_nCount  = 0; 
			}

		break; 
	}
	
	CDialog::OnTimer(nIDEvent);
}

BEGIN_EVENTSINK_MAP(CNModuleAging, CDialog)
    //{{AFX_EVENTSINK_MAP(CNModuleAging)
	ON_EVENT(CNModuleAging, IDC_CONDITION_MODE, -600 /* Click */, OnClickConditionMode, VTS_NONE)
	ON_EVENT(CNModuleAging, IDC_RETURN, -600 /* Click */, OnClickReturn, VTS_NONE)
	ON_EVENT(CNModuleAging, IDC_PTP_DUMMY, -600 /* Click */, OnClickPtpDummy, VTS_NONE)
	ON_EVENT(CNModuleAging, IDC_PTP_DUMMYCHANGE, -600 /* Click */, OnClickPtpDummychange, VTS_NONE)
	ON_EVENT(CNModuleAging, IDC_AGING_START, -600 /* Click */, OnClickAgingStart, VTS_NONE)
	ON_EVENT(CNModuleAging, IDC_AGING_TIME_MIN, -600 /* Click */, OnClickAgingTimeMin, VTS_NONE)
	ON_EVENT(CNModuleAging, IDC_AGING_TIME_SEC, -600 /* Click */, OnClickAgingTimeSec, VTS_NONE)
	ON_EVENT(CNModuleAging, IDC_AGING_TIME_CYC, -600 /* Click */, OnClickAgingTimeCyc, VTS_NONE)
	ON_EVENT(CNModuleAging, IDC_RNGHZ_MIN, -600 /* Click */, OnClickRnghzMin, VTS_NONE)
	ON_EVENT(CNModuleAging, IDC_RNGHZ_MAX, -600 /* Click */, OnClickRnghzMax, VTS_NONE)
	ON_EVENT(CNModuleAging, IDC_FLEXG_N_AGING, -600 /* Click */, OnClickFlexgNAging, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CNModuleAging::OnClickConditionMode() 
{
	// TODO: Add your control notification handler code here

	if ( !m_bConditionMode )
	{
		m_bConditionMode = TRUE; 
		m_ctrlConditionMode.SetCaption("MANUAL INPUT");
		m_ctrlConditionMode.SetBackColor(RED); 
	}
	else
	{
		m_bConditionMode = FALSE; 
		m_ctrlConditionMode.SetCaption("BY RECIPE");
		m_ctrlConditionMode.SetBackColor(YELLOW); 
	}	
}

void CNModuleAging::InitFlexGrid()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc   = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();	


	CString str, strTemp;  
	CString strRecipe[7]={"Amplitude","Duty","NegOffset","FirstVoltage","RunLimit","FallingEdge","P-Offset"}; 


	for ( int nHeadCount = 0; nHeadCount < MAX_NOZZLE; nHeadCount++ )
	{
		str.Format("H%d",nHeadCount+1);

		m_ctrlNAgingCondition.SetTextMatrix(nHeadCount+1, 0 ,str);
		m_ctrlNAgingCondition.SetRowHeight(nHeadCount,350);
	}



	for ( int nCount = 0; nCount < 6; nCount++ )
	{
		if ( nCount == 0 )
		{
			m_ctrlNAgingCondition.SetColAlignment(0,4); 
			m_ctrlNAgingCondition.SetColWidth(nCount,1000);
		}

		m_ctrlNAgingCondition.SetTextMatrix(0, nCount+1 ,strRecipe[nCount]);
		m_ctrlNAgingCondition.SetColWidth(nCount+1,1200);

		m_ctrlNAgingCondition.SetColAlignment(nCount+1,4); 
	}

	for ( nHeadCount = 0; nHeadCount < MAX_NOZZLE; nHeadCount++ )
	{
		strTemp.Format("%d",pDoc->m_structPatternData[0].m_nNAmplitude[nHeadCount]); 
		m_ctrlNAgingCondition.SetTextMatrix(nHeadCount+1, 1 ,strTemp);
		strTemp.Format("%.f",pDoc->m_structPatternData[0].m_dNDuty[nHeadCount]); 
		m_ctrlNAgingCondition.SetTextMatrix(nHeadCount+1, 2 ,strTemp);
		strTemp.Format("%d",pDoc->m_structPatternData[0].m_nNNegOffset[nHeadCount]); 
		m_ctrlNAgingCondition.SetTextMatrix(nHeadCount+1, 3 ,strTemp);
		strTemp.Format("%d",pDoc->m_structPatternData[0].m_nNFirstVoltage[nHeadCount]); 
		m_ctrlNAgingCondition.SetTextMatrix(nHeadCount+1, 4 ,strTemp);
		strTemp.Format("%d",pDoc->m_structPatternData[0].m_nNShot); 
		m_ctrlNAgingCondition.SetTextMatrix(nHeadCount+1, 5 ,strTemp);
		strTemp.Format("%d",pDoc->m_structPatternData[0].m_nNFallingEdge[nHeadCount]); 
		m_ctrlNAgingCondition.SetTextMatrix(nHeadCount+1, 6 ,strTemp);
	}

	for ( nHeadCount = 0; nHeadCount < MAX_NOZZLE; nHeadCount++ )
	{
		for ( int nColCount = 0; nColCount < 7; nColCount++ )
		{
			m_ctrlNAgingCondition.SetRow(nHeadCount+1);
			m_ctrlNAgingCondition.SetCol(nColCount); 

			if ( Drop_Info.manu_head_job[nHeadCount] == TRUE )
			{					
				if ( nColCount == 0 )					
					m_ctrlNAgingCondition.SetCellBackColor(LIGHTGREEN);
				else 
					m_ctrlNAgingCondition.SetCellBackColor(YELLOW);
			}
			else 
			{
				if ( nColCount == 0 )					
					m_ctrlNAgingCondition.SetCellBackColor(LIGHTGRAY);
				else 
					m_ctrlNAgingCondition.SetCellBackColor(WHITE);
			}
		}
	}
}

void CNModuleAging::OnClickReturn() 
{
	// TODO: Add your control notification handler code here
	EndDialog(IDOK);	
}

void CNModuleAging::OnClickPtpDummy() 
{
	// TODO: Add your control notification handler code here
	
}

void CNModuleAging::OnClickPtpDummychange() 
{
	// TODO: Add your control notification handler code here
	
}

void CNModuleAging::OnClickAgingStart() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc   = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();	
	
	if ( m_nHzMode == -1 ) 
	{
		AfxMessageBox("Hz Mode 설정 필요!!!"); 
		return; 
	}
	else if ( m_nHzMode == 1 && ( m_nRngHzMin == 0 || m_nRngHzMin == 0 ) )
	{
		AfxMessageBox("Hz Range 설정 필요!!!"); 
		return; 
	}
	else if (  m_nTimeSec == 0 && m_nTimeMin == 0 )
	{
		AfxMessageBox("Aging Time 설정 필요!!!"); 
		return; 
	}
	else if ( m_nTimeCycle == 0 )
	{
		AfxMessageBox("Aging Cycle 설정 필요!!!"); 
		return;
	}

	m_nCount  = 0; 
	m_nStatus = FALSE; 
	
	for ( int nHeadCount = 0; nHeadCount < MAX_NOZZLE; nHeadCount++ )
	{
		m_ctrlNAgingCondition.SetRow(nHeadCount+1);
		m_ctrlNAgingCondition.SetCol(0);
		
		if ( m_ctrlNAgingCondition.GetCellBackColor() == LIGHTGREEN )
		{
			WriteNparam(nHeadCount);
		}
	}

	SetTimer(0,1000,NULL);
}

void CNModuleAging::OnClickAgingTimeMin() 
{
	// TODO: Add your control notification handler code here
	Use_TK( m_ctrlTimeMin , 10, 0, 105, 175);

	m_nTimeMin = atoi(m_ctrlTimeMin.GetCaption()); 
}

void CNModuleAging::OnClickAgingTimeSec() 
{
	// TODO: Add your control notification handler code here
	Use_TK( m_ctrlTimeSec , 59, 0, 105, 175);

	m_nTimeSec = atoi(m_ctrlTimeSec.GetCaption()); 
}

void CNModuleAging::OnClickAgingTimeCyc() 
{
	// TODO: Add your control notification handler code here
	Use_TK( m_ctrlTimeCyc , 3, 1, 105, 175);

	m_nTimeCycle = atoi(m_ctrlTimeCyc.GetCaption());
}

void CNModuleAging::OnRADIOFixHz() 
{
	// TODO: Add your control notification handler code here
	m_nHzMode = 0; 

	m_ctrlRngHzMin.SetEnabled(FALSE); 
	m_ctrlRngHzMax.SetEnabled(FALSE); 
}

void CNModuleAging::OnRADIOAdjustHz() 
{
	// TODO: Add your control notification handler code here
	m_nHzMode = 1; 

	m_ctrlRngHzMin.SetEnabled(TRUE); 
	m_ctrlRngHzMax.SetEnabled(TRUE); 
}

void CNModuleAging::OnClickRnghzMin() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc   = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();	


	Use_TK( m_ctrlRngHzMin , pDoc->m_structPatternData[0].m_dNHz, 50, 105, 175);

	m_nRngHzMin = atoi(m_ctrlRngHzMin.GetCaption()); 
}

void CNModuleAging::OnClickRnghzMax() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc   = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();	

	Use_TK( m_ctrlRngHzMax , 250, pDoc->m_structPatternData[0].m_dNHz, 105, 175);	

	m_nRngHzMax = atoi(m_ctrlRngHzMax.GetCaption()); 
}

void CNModuleAging::OnClickFlexgNAging() 
{
	// TODO: Add your control notification handler code here
	int nCol = 0, nRow = 0 , nHeadCount = 0; 

	nRow = m_ctrlNAgingCondition.GetMouseRow();
 	nCol = m_ctrlNAgingCondition.GetMouseCol();

	if ( nRow == 0 && ( nCol > 0 )) return; // 제목줄 눌렀을때 메세지 박스 . 귀찮음. 

	if ( nCol == 0 ) // HEAD SELECT 
	{
		if ( !m_bConditionMode && !Drop_Info.manu_head_job[nRow-1] && nRow != 0 )
		{
			AfxMessageBox("BY RECIPE MODE 에서는 사용 HEAD 외 선택 불가.");
			return;
		}		

		m_ctrlNAgingCondition.ShowWindow(SW_HIDE);

		if ( nRow == 0 )
		{
			for ( nHeadCount = 0; nHeadCount < MAX_NOZZLE; nHeadCount++ )
			{
				if ( Drop_Info.manu_head_job[nHeadCount] == TRUE )
				{
					for ( int nColCount = 0; nColCount < 7; nColCount++ )
					{
						m_ctrlNAgingCondition.SetRow(nHeadCount+1);
						m_ctrlNAgingCondition.SetCol(nColCount); 
					
						if ( nColCount == 0 )					
							m_ctrlNAgingCondition.SetCellBackColor(LIGHTGREEN);
						else 
							m_ctrlNAgingCondition.SetCellBackColor(YELLOW);
					}
				}
				else 
				{
					for ( int nColCount = 0; nColCount < 7; nColCount++ )
					{
						m_ctrlNAgingCondition.SetRow(nHeadCount+1);
						m_ctrlNAgingCondition.SetCol(nColCount); 
						
						if ( nColCount == 0 )					
							m_ctrlNAgingCondition.SetCellBackColor(LIGHTGRAY);
						else 
							m_ctrlNAgingCondition.SetCellBackColor(WHITE);
					}
				}
			}
		}
		else 
		{
			if ( m_ctrlNAgingCondition.GetCellBackColor() == LIGHTGRAY )
			{
				for ( int nCount = 0; nCount < 7; nCount++ )
				{
					m_ctrlNAgingCondition.SetRow(nRow);
					m_ctrlNAgingCondition.SetCol(nCount);
				
					if ( nCount == 0 )					
						m_ctrlNAgingCondition.SetCellBackColor(LIGHTGREEN);
					else 
						m_ctrlNAgingCondition.SetCellBackColor(YELLOW);
				}
			}
			else 
			{
				for ( int nCount = 0; nCount < 7; nCount++ )
				{
					m_ctrlNAgingCondition.SetRow(nRow);
					m_ctrlNAgingCondition.SetCol(nCount);

					if ( nCount == 0 )					
						m_ctrlNAgingCondition.SetCellBackColor(LIGHTGRAY);
					else 
						m_ctrlNAgingCondition.SetCellBackColor(WHITE);
				}
			}
		}

		m_ctrlNAgingCondition.ShowWindow(SW_SHOW);
	}
	else			 // CONDITION CHANGE
	{
		if ( !m_bConditionMode )
		{
			AfxMessageBox("BY RECIPE MODE 에서는 수정할수 없음.");
			return;
		}
		else 
		{
			Use_TKg(m_ctrlNAgingCondition, 10.0, 0.0, 690, 180, nCol, nRow);
		}
	}

}

BOOL CNModuleAging::WriteNparam(int nHeadNum)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc   = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	pView->m_pPiezoDevice->m_nSlave = nHeadNum;
									
	if(pView->m_pPiezoDevice->Connect())
	{	
		if(pView->m_pPiezoDevice->Get_Status(&m_structNStatus) != -1)
		{
			if(m_structNStatus.bStandby == FALSE)
			{
				pView->m_pPiezoDevice->Do_Standby(FALSE);
				Sleep(30);
			}
		}
				
		//Read//
		pView->m_pPiezoDevice->Read_Param(&m_structNParam);
		Sleep(30);

		m_structNParam.nAmplitude   = pDoc->m_structPatternData[0].m_nNAmplitude[nHeadNum]; //amplitude
		m_structNParam.nDuty        = pDoc->m_structPatternData[0].m_dNDuty[nHeadNum]; //duty
		m_structNParam.nFrequecy    = (int)pDoc->m_structPatternData[0].m_dNHz; //hz
		m_structNParam.nOffset      = pDoc->m_structPatternData[0].m_nNNegOffset[nHeadNum]; //neg.offset
		m_structNParam.nFallingEdge = pDoc->m_structPatternData[0].m_nNFallingEdge[nHeadNum]; //falling_edge
		m_structNParam.nFirst_Vol   = pDoc->m_structPatternData[0].m_nNFirstVoltage[nHeadNum]; //first voltage


		m_structNParam.nRun_Limit   = pDoc->m_structPatternData[0].m_nNShot; 
										
		//FIRST DATA 0 
		m_structNParam.nPTNumber[0] = 0; 
		m_structNParam.nK_Amplitude = 0;
		m_structNParam.nK_Offset    = 0;
		m_structNParam.nRisingEdge  = 0;
		m_structNParam.nPTSetNumber = 0;
										
		if(pView->m_pPiezoDevice->Write_Param(m_structNParam) == FALSE)
		{
			//alarm add..
			pView->SendMessage(WM_ERROR,41,nHeadNum+1);							
			return FALSE; 
		}
		return TRUE; 
	}
	else
	{
		return FALSE; 
	}
}

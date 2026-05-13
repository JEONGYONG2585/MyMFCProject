// HeadDropInformation.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "HeadDropInformation.h"
#include "MainFrm.h"
#include "P8CA_LcDispDoc.h"
#include "P8CA_LcDispView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHeadDropInformation dialog


CHeadDropInformation::CHeadDropInformation(CWnd* pParent /*=NULL*/)
	: CDialog(CHeadDropInformation::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHeadDropInformation)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CHeadDropInformation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHeadDropInformation)
	DDX_Control(pDX, IDC_GRID_HEADSENSOR, m_GridHeadSensor);
	DDX_Control(pDX, IDC_GRID_DROPCNT_RANGE, m_GridDropCntRange);
	DDX_Control(pDX, IDC_GRID_LC_AMOUNT, m_GridLcAmount);
	DDX_Control(pDX, IDC_LC_COUNT_SET, m_ctrlLcCountSet);
	DDX_Control(pDX, IDC_GRID_AMOUNTDIFF, m_GridAmountDiff);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHeadDropInformation, CDialog)
	//{{AFX_MSG_MAP(CHeadDropInformation)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHeadDropInformation message handlers

BOOL CHeadDropInformation::OnInitDialog() 
{
	CDialog::OnInitDialog();
	InforDisplay();

	SelectLanguage();
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CHeadDropInformation::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CHeadDropInformation::InforDisplay()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	int i,j;
	CString str;
	BOOL bIoResult=FALSE;

	m_GridHeadSensor.SetRows(4);m_GridHeadSensor.SetCols(17);

	for(j=0;j<MAX_NOZZLE;j++)
	{
		str.Format("H%d",j+1);
		m_GridHeadSensor.SetTextMatrix(0,(MAX_NOZZLE+1)-(j+1),str);
		m_GridHeadSensor.SetColAlignment(j,4);
		m_GridHeadSensor.SetColWidth(j+1,700);
		if(pDoc->m_bIsHeadSelected[(MAX_NOZZLE-1)-j]==TRUE) 
		{
			m_GridHeadSensor.SetRow(0);
			m_GridHeadSensor.SetCol(j+1);
			m_GridHeadSensor.SetCellBackColor(GREEN);
		}
	}
	m_GridHeadSensor.SetColWidth(0,1950);
	m_GridHeadSensor.SetColAlignment(0,4);

	if(pView->m_nLanguage == 0)
	{
		m_GridHeadSensor.SetTextMatrix(1,0,"Drop Count");
		m_GridHeadSensor.SetTextMatrix(2,0,"ÀÜ·® °¨Áö");
		m_GridHeadSensor.SetTextMatrix(3,0,"ÇÏÇÑ °¨Áö");
	}
	else if(pView->m_nLanguage == 1)
	{
		m_GridHeadSensor.SetTextMatrix(1,0,"Drop Count");
		m_GridHeadSensor.SetTextMatrix(2,0,"Min Detect");
		m_GridHeadSensor.SetTextMatrix(3,0,"Lowest Detect");
	}
	else if(pView->m_nLanguage == 2)
	{
		m_GridHeadSensor.SetTextMatrix(1,0,"Drop Count");
		m_GridHeadSensor.SetTextMatrix(2,0,"æ®Õá Êïëë");
		m_GridHeadSensor.SetTextMatrix(3,0,"ù»ùÚ Êïëë");
	}


	m_GridHeadSensor.SetRowHeight(0,300);
	for(i=0;i<4;i++)
	{
		m_GridHeadSensor.SetRow(i);
		m_GridHeadSensor.SetCol(0);
		m_GridHeadSensor.SetCellBackColor(WHITEGRAY);
	}
	for(i=0;i<3;i++)
	{
		m_GridHeadSensor.SetRowHeight(i+1,500);
	}
	for(i = 0; i < MAX_NOZZLE; i++)
	{
		m_bDropCount[i] = pDoc->m_bDropCountSensorUSE[i];
		m_bRemain[i] = pDoc->m_bRemainSensorUSE[i];
		m_bLowDetect[i] = pDoc->m_bLowDetectSensorUSE[i];
	}
	for (j =0 ; j <m_GridHeadSensor.GetCols() -1; j++)
	{
		if(m_bDropCount[(MAX_NOZZLE-1)-j])
		{
			m_GridHeadSensor.SetRow(1);
			m_GridHeadSensor.SetCol(j+1);	
			m_GridHeadSensor.SetCellBackColor(ORANGE);
		}
		else
		{
			m_GridHeadSensor.SetRow(1);
			m_GridHeadSensor.SetCol(j+1);	
			m_GridHeadSensor.SetCellBackColor(WHITE);
		}
		if(m_bRemain[(MAX_NOZZLE-1)-j])
		{
			m_GridHeadSensor.SetRow(2);
			m_GridHeadSensor.SetCol(j+1);	
			m_GridHeadSensor.SetCellBackColor(ORANGE);
		}
		else
		{
			m_GridHeadSensor.SetRow(2);
			m_GridHeadSensor.SetCol(j+1);	
			m_GridHeadSensor.SetCellBackColor(WHITE);
		}
		if(m_bLowDetect[(MAX_NOZZLE-1)-j])
		{
			m_GridHeadSensor.SetRow(3);
			m_GridHeadSensor.SetCol(j+1);	
			m_GridHeadSensor.SetCellBackColor(ORANGE);
		}
		else
		{
			m_GridHeadSensor.SetRow(3);
			m_GridHeadSensor.SetCol(j+1);	
			m_GridHeadSensor.SetCellBackColor(WHITE);
		}
		//			m_GridHeadSensor.SetTextMatrix(i+1, (MAX_NOZZLE+1)-(j+1) ,(LPCSTR)str);
	}

//Drop Count 
	m_GridDropCntRange.SetRows(3);m_GridDropCntRange.SetCols(17);
	
	if(pView->m_nLanguage == 0)
	{
		m_GridDropCntRange.SetTextMatrix(1, 0 ,"»óÇÑ( + %)");
		m_GridDropCntRange.SetTextMatrix(2, 0 ,"ÇÏÇÑ( - %)");
	}
	else if(pView->m_nLanguage == 1)
	{
		m_GridDropCntRange.SetTextMatrix(1, 0 ,"Maximum( + %)");
		m_GridDropCntRange.SetTextMatrix(2, 0 ,"Lowest( - %)");
	}
	else if(pView->m_nLanguage == 2)
	{
		m_GridDropCntRange.SetTextMatrix(1, 0 ,"ß¾ùÚ( + %)");
		m_GridDropCntRange.SetTextMatrix(2, 0 ,"ù»ùÚ( - %)");
	}



	m_GridDropCntRange.SetColWidth(0,1950);
	
	
	m_GridDropCntRange.SetRowHeight(0, 300);
	m_GridDropCntRange.SetRowHeight(1, 500);
	m_GridDropCntRange.SetRowHeight(2, 500);
	
	for(i=0;i<3;i++)
	{
		m_GridDropCntRange.SetRow(i);
		m_GridDropCntRange.SetCol(0);
		m_GridDropCntRange.SetCellBackColor(WHITEGRAY);
	}

	for(i = 0; i < 2; i++)
	{
		for(j = 0; j < MAX_NOZZLE; j++)
		{
			str.Format("  H%2d",j+1);
			m_GridDropCntRange.SetTextMatrix(0, (MAX_NOZZLE+1)-(j+1) ,str);
			m_GridDropCntRange.SetColWidth(j+1, 700);
			m_GridDropCntRange.SetColAlignment(j+1,4);	
			if(pDoc->m_bIsHeadSelected[(MAX_NOZZLE-1)-j]==TRUE) 
			{
				m_GridDropCntRange.SetRow(0);
				m_GridDropCntRange.SetCol(j+1);
				m_GridDropCntRange.SetCellBackColor(GREEN);
			}	
			
			str.Format("%4d",(int)pDoc->m_dDropCountErrorRange_per[i][j]);	// 0: »óÇÑ, 1: ÇÏÇÑ
			m_GridDropCntRange.SetTextMatrix(i+1, (MAX_NOZZLE+1)-(j+1), str);
		}
	}
//LC·® °æ°í 
	m_GridLcAmount.SetRows(4);m_GridLcAmount.SetCols(17);

	for(j = 0 ; j < MAX_NOZZLE ; j++)
	{
		str.Format("  H%2d",j+1);
		m_GridLcAmount.SetTextMatrix(0, (MAX_NOZZLE+1)-(j+1) ,str);
		m_GridLcAmount.SetColWidth(j+1, 700);
		m_GridLcAmount.SetColAlignment(j+1,4);		
		if(pDoc->m_bIsHeadSelected[(MAX_NOZZLE-1)-j]==TRUE) 
		{
			m_GridLcAmount.SetRow(0);
			m_GridLcAmount.SetCol(j+1);
			m_GridLcAmount.SetCellBackColor(GREEN);
		}
	}

	if(pView->m_nLanguage ==0)
	{
		m_GridLcAmount.SetTextMatrix(1, 0, "Çö LCÀÜ·®(g)");
		m_GridLcAmount.SetTextMatrix(2, 0, "°æ°í LCÀÜ·®(g)");
		m_GridLcAmount.SetTextMatrix(3, 0, "ÀÜ·® DetectÈÄ ÀÛ¾÷¼ö");
	}

	else if(pView->m_nLanguage ==1)
	{
		m_GridLcAmount.SetTextMatrix(1, 0, "Çö LCÀÜ·®(g)");
		m_GridLcAmount.SetTextMatrix(2, 0, "°æ°í LCÀÜ·®(g)");
		m_GridLcAmount.SetTextMatrix(3, 0, "ÀÜ·® DetectÈÄ ÀÛ¾÷¼ö");
	}

	else if(pView->m_nLanguage ==2)
	{
		m_GridLcAmount.SetTextMatrix(1, 0, "úÞ LCæ®Õá(g)");
		m_GridLcAmount.SetTextMatrix(2, 0, "ÌíÍ± æ®Õá(g)");
		m_GridLcAmount.SetTextMatrix(3, 0, "æ®ÕáDetectý¨íÂåöâ¦");
	}


	m_GridLcAmount.SetColWidth(0, 1950);
	for(i=1 ; i < 4 ; i++) 
	{
		m_GridLcAmount.SetRowHeight(i,500);
	}
	for(i=0;i<4;i++)
	{
		m_GridLcAmount.SetRow(i);
		m_GridLcAmount.SetCol(0);
		m_GridLcAmount.SetCellBackColor(WHITEGRAY);
	}

	for(i =0 ; i <m_GridLcAmount.GetRows() -1; i ++)
	{
		for (j =0 ; j <m_GridLcAmount.GetCols() -1; j++)
		{
			if(i<2) str.Format("%.1f", pDoc->m_dOffset[i][j]);
			else if(i==2) str.Format("%d", ThreadStage.nLCDetectedCount[j]);	// ÀÜ·® °¨Áö ÈÄ ÀÛ¾÷ÇÑ Glass ¼ö 
			m_GridLcAmount.SetTextMatrix(i+1, (MAX_NOZZLE+1)-(j+1) ,(LPCSTR)str);
		}
	}
	// ÀÜ·® °¨Áö ¼¾¼­ Detect ¿©ºÎ¿¡ µû¸¥ »ö±ò Ç¥½Ã
	for(i =0 ; i<m_GridLcAmount.GetCols() -1; i++)
	{
		m_GridLcAmount.SetRow(3);
		m_GridLcAmount.SetCol((MAX_NOZZLE+1)-(i+1));
		m_GridLcAmount.SetCellBackColor(WHITE);		
		if(pDoc->m_bIsHeadSelected[i] == TRUE)
		{
			FAS_GetIoBit(1,true,LC_LIMIT1+i,&bIoResult);
			if(!bIoResult)	
				m_GridLcAmount.SetCellBackColor(RED);				
		}
	}
	// ÀÜ·® °¨Áö ¼¾¼­¿¡ °¨ÁöµÈ ÈÄ ÀÛ¾÷ °¡´ÉÇÑ ¼ö Ç¥½Ã
	str.Format("%d",pDoc->m_dSetLCDetectedCount);
	m_ctrlLcCountSet.SetCaption(str);

//LC¾ç Max~Min °ü¸®
	m_GridAmountDiff.SetRows(2); m_GridAmountDiff.SetCols(3);

	for(i=0;i<2;i++)
		m_dLCDiff[i] = pDoc->m_dLCAmountDiff[i];

	m_GridAmountDiff.SetColAlignment(0,4);
	m_GridAmountDiff.SetColWidth(0,2600);
	m_GridAmountDiff.SetColAlignment(1,4);
	m_GridAmountDiff.SetColWidth(1,2000);
	m_GridAmountDiff.SetColAlignment(2,4);
	m_GridAmountDiff.SetColWidth(2,2000);
	m_GridAmountDiff.SetRowHeight(0,400);
	m_GridAmountDiff.SetRowHeight(1,600);

	for(i=0;i<3;i++)
	{
		m_GridAmountDiff.SetRow(0);
		m_GridAmountDiff.SetCol(i);
		m_GridAmountDiff.SetCellBackColor(WHITEGRAY);
	}
	m_GridAmountDiff.SetRow(1);
	m_GridAmountDiff.SetCol(0);
	m_GridAmountDiff.SetCellBackColor(WHITEGRAY);

	if(pView->m_nLanguage ==0)
	m_GridAmountDiff.SetTextMatrix(1,0,"Head LC À¯ÀÇÂ÷(g)");
	else if(pView->m_nLanguage ==1)
	m_GridAmountDiff.SetTextMatrix(1,0,"Head LC Difference(g)");
	else if(pView->m_nLanguage ==2)
	m_GridAmountDiff.SetTextMatrix(1,0,"Head LC ø¶ó¬(g)");

	
	m_GridAmountDiff.SetTextMatrix(0,1,"Canister3~4");
	m_GridAmountDiff.SetTextMatrix(0,2,"Canister1~2");
	str.Format("%.0f",m_dLCDiff[0]);
	m_GridAmountDiff.SetTextMatrix(1,2,str);
	str.Format("%.0f",m_dLCDiff[1]);
	m_GridAmountDiff.SetTextMatrix(1,1,str);
	
	GetDlgItem(IDOK)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDCANCEL)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
}

BEGIN_EVENTSINK_MAP(CHeadDropInformation, CDialog)
    //{{AFX_EVENTSINK_MAP(CHeadDropInformation)
	ON_EVENT(CHeadDropInformation, IDC_GRID_HEADSENSOR, -600 /* Click */, OnClickGridHeadsensor, VTS_NONE)
	ON_EVENT(CHeadDropInformation, IDC_GRID_DROPCNT_RANGE, -600 /* Click */, OnClickGridDropcntRange, VTS_NONE)
	ON_EVENT(CHeadDropInformation, IDC_GRID_LC_AMOUNT, -600 /* Click */, OnClickGridLcAmount, VTS_NONE)
	ON_EVENT(CHeadDropInformation, IDC_SAVE, -600 /* Click */, OnClickSave, VTS_NONE)
	ON_EVENT(CHeadDropInformation, IDC_LC_COUNT_SET, -600 /* Click */, OnClickLcCountSet, VTS_NONE)
	ON_EVENT(CHeadDropInformation, IDC_GRID_AMOUNTDIFF, -600 /* Click */, OnClickGridAmountdiff, VTS_NONE)
	ON_EVENT(CHeadDropInformation, IDC_RETURN, -600 /* Click */, OnClickReturn, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CHeadDropInformation::OnClickGridHeadsensor() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	BOOL bSingleRow = TRUE;
	double max = 0.0 , min = 0.0;
	int Row=m_GridHeadSensor.GetMouseRow();
	int Col=m_GridHeadSensor.GetMouseCol();

	if(Col>=1)
		if((Row==0)&&(pDoc->m_bIsHeadSelected[(MAX_NOZZLE)-(Col)]==FALSE) ) return;
	if(Col == FALSE)	bSingleRow = FALSE;	

	if(m_GridHeadSensor.GetCellBackColor() == ORANGE)
	{
		if(Row == 1)
		{
			m_bDropCount[(MAX_NOZZLE)-Col] = FALSE;
		}
		else if(Row == 2)
		{
			m_bRemain[(MAX_NOZZLE)-Col] = FALSE;
		}
		else if(Row == 3)
		{
			m_bLowDetect[(MAX_NOZZLE)-Col] = FALSE;
		}
		m_GridHeadSensor.SetCol(Col);
		m_GridHeadSensor.SetRow(Row);
		m_GridHeadSensor.SetCellBackColor(WHITE);
	}
	else if(m_GridHeadSensor.GetCellBackColor() == WHITE)
	{
		if(Row == 1)
		{
			m_bDropCount[(MAX_NOZZLE)-Col] = TRUE;
		}
		else if(Row == 2)
		{
			m_bRemain[(MAX_NOZZLE)-Col] = TRUE;
		}
		else if(Row == 3)
		{
			m_bLowDetect[(MAX_NOZZLE)-Col] = TRUE;
		}
		m_GridHeadSensor.SetCol(Col);
		m_GridHeadSensor.SetRow(Row);
		m_GridHeadSensor.SetCellBackColor(ORANGE);
	}
}

void CHeadDropInformation::OnClickGridDropcntRange() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	BOOL bSingleRow = TRUE;
	double max = 0.0 , min = 0.0;
	int Row=m_GridDropCntRange.GetMouseRow();
	int Col=m_GridDropCntRange.GetMouseCol();

	if(Row == 0) return;
	if(Col>=1)
		if( /*(Row==0)&&*/(pDoc->m_bIsHeadSelected[(MAX_NOZZLE)-(Col)]==FALSE) ) return;
	if(Col == FALSE)	bSingleRow = FALSE;	
	
	max = 120.0 ; min = 0 ; 

	Use_TKg(m_GridDropCntRange, max, min, 512, 384, TRUE, bSingleRow);	
}

void CHeadDropInformation::OnClickGridLcAmount() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	BOOL bSingleRow = TRUE;
	double max = 0.0 , min = 0.0;
	int Row=m_GridLcAmount.GetMouseRow();
	int Col=m_GridLcAmount.GetMouseCol();


	if(Row == 0 || Row == 1) return;
//	if(Row == 3 && Col == 0) return;
	if(Col>=1)
		if(/* (Row==2)&&*/(pDoc->m_bIsHeadSelected[(MAX_NOZZLE)-(Col)]==FALSE) ) return;
	if(Col == FALSE)	bSingleRow = FALSE;	
//	if(Row == 0 ) { max = 1000 ; min = 0 ; }
	if(Row == 2)	{ max = 500 ; min = 30 ;	}
	else if(Row == 3)	{ max = 10 ; min = 0 ;	}
	else { max = 3000 ; min = -3000 ; }
	
	Use_TKg(m_GridLcAmount, max, min, 512, 384, TRUE, bSingleRow);

	for (int i =0 ; i <MAX_NOZZLE; i++)
		ThreadStage.nLCDetectedCount[i]=atoi((char *)(LPCSTR)m_GridLcAmount.GetTextMatrix(3,(MAX_NOZZLE+1)-(i+1)));
}

void CHeadDropInformation::OnClickSave() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	// TODO: Add your control notification handler code here
	CString strPathName;
	CFile SaveFile;
	int i,j;
//Head Sensor 
	for(i = 0; i < MAX_NOZZLE; i++)
	{
		pDoc->m_bDropCountSensorUSE[i] = m_bDropCount[i];
		pDoc->m_bRemainSensorUSE[i] = m_bRemain[i];
		pDoc->m_bLowDetectSensorUSE[i] = m_bLowDetect[i];
	}

	for(i=0;i<2;i++)
	{
		 m_dLCDiff[i]=atoi((char *)(LPCSTR)m_GridAmountDiff.GetTextMatrix(1, 2-i));
		 pDoc->m_dLCAmountDiff[i] = m_dLCDiff[i];
	}

//Drop Count Range
	for(i=0; i<2; i++)	// 0 : »óÇÑ, 1: ÇÏÇÑ
	{
		for(j = 0; j < MAX_NOZZLE; j++)
		{
			pDoc->m_dDropCountErrorRange_per[i][j]=atof((char *)(LPCSTR)m_GridDropCntRange.GetTextMatrix(i+1, (MAX_NOZZLE+1)-(j+1)));
		}
	}
	pDoc->m_dSetLCDetectedCount=atoi(m_ctrlLcCountSet.GetCaption());

	strPathName.Format("%s\\pumpLCNo.dat", pDoc->m_strDataPath);

	if( SaveFile.Open( (char *)(LPCSTR)strPathName, CFile::modeCreate | CFile::modeWrite ) )
	{
		pDoc->m_nSelFile = 4;
		CArchive ar( &SaveFile, CArchive::store );
		pDoc->Serialize(ar);
		ar.Close();
		SaveFile.Close();
	}	

//LC °æ°í·®
	for(i =0 ; i < 2; i++)
	{
		for (j =0 ; j <m_GridLcAmount.GetCols() -1; j++)
		{
			pDoc->m_dOffset[i][j] = atof((char *)(LPCSTR)m_GridLcAmount.GetTextMatrix(i+1,m_GridLcAmount.GetCols()-1-j));
		}
	}
//2015.04.29 by tskim ¿øÀç·á Àü»êÈ­Add LCÀÜ·®º¸°í	
//	strPathName.Format("%s\\offset.dat", pDoc->m_strDataPath);
//	
//	if( SaveFile.Open( (char *)(LPCSTR)strPathName, CFile::modeCreate | CFile::modeWrite ) )
//	{
//		pDoc->m_nSelFile = 2;
//		CArchive ar( &SaveFile, CArchive::store );
//		pDoc->Serialize(ar);
//		ar.Close();
//		SaveFile.Close();
//	}
	pDoc->Save_LCRemain();
	pDoc->Save_SpecoutCount();	
}

void CHeadDropInformation::OnClickLcCountSet() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlLcCountSet, 10, 0 ,350 ,100);	
}
//LC¾çDiff
void CHeadDropInformation::OnClickGridAmountdiff() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	double max = 0.0 , min = 0.0;
	int Row=m_GridAmountDiff.GetMouseRow();
	int Col=m_GridAmountDiff.GetMouseCol();

	if(Row == 0) return;
	if(Col == 0) return;
	
	max = 1000 ; min = 0 ; 

	Use_TKg(m_GridAmountDiff, max, min, 512, 384, TRUE, TRUE);	
}

void CHeadDropInformation::OnClickReturn() 
{
	// TODO: Add your control notification handler code here
	EndDialog(IDOK);
}

void CHeadDropInformation::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CPen pen;
	pen.CreatePen(PS_SOLID,768,BLACK);
	CPen *pOldpen = (CPen *)dc.SelectObject(&pen);

	dc.MoveTo(0,HEIGHT_YPOS+310);
	dc.LineTo(SCREEN_WIDTH,HEIGHT_YPOS+310);

	dc.SelectObject(pOldpen);
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CHeadDropInformation::SelectLanguage()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(pView->m_nLanguage == 0)
	{
			SetDlgItemText(IDC_LABEL4, _T("<Head Sensor ¼³Á¤>"));		
			SetDlgItemText(IDC_LABEL3, _T("<Drop Count ¼³Á¤>"));	
			SetDlgItemText(IDC_LABEL1, _T("<LC ¾ç ¼³Á¤>"));	
			SetDlgItemText(IDC_COMMANDBUTTON4, _T("  ÀÜ·® °¨Áö ÈÄ   Ã³¸® °¡´É Glass ¸Å¼ö ¼³Á¤"));	
			SetDlgItemText(IDC_SAVE, _T("ÀúÀå"));
			SetDlgItemText(IDC_RETURN, _T("ÀÌÀü È­¸é"));
			SetDlgItemText(IDC_LABEL2, _T("(Àå)"));	
	}

	else if(pView->m_nLanguage == 1)
	{
			SetDlgItemText(IDC_LABEL4, _T("<Head Sensor Set>"));		
			SetDlgItemText(IDC_LABEL3, _T("<Drop Count Set>"));	
			SetDlgItemText(IDC_LABEL1, _T("<LC Amount Set>"));	
			SetDlgItemText(IDC_COMMANDBUTTON4, _T(" After detect Min LC , Glass sheet Count Set To Operate"));	
			SetDlgItemText(IDC_SAVE, _T("Save"));
			SetDlgItemText(IDC_RETURN, _T("Before"));
			SetDlgItemText(IDC_LABEL2, _T("(Sheet)"));	
	}

	else if(pView->m_nLanguage == 2)
	{
			SetDlgItemText(IDC_LABEL4, _T("<Head Sensor àâïÒ>"));		
			SetDlgItemText(IDC_LABEL3, _T("<Drop Count àâïÒ>"));	
			SetDlgItemText(IDC_LABEL1, _T("<LC Õá àâïÒ>"));	
			SetDlgItemText(IDC_COMMANDBUTTON4, _T(" íÑÕáÊïëëý­   ô¥ìµÊ¦àâïôGlassâ¦Õá"));	
			SetDlgItemText(IDC_SAVE, _T("ÜÁðí"));
			SetDlgItemText(IDC_RETURN, _T("Ú÷üÞ"));
			SetDlgItemText(IDC_LABEL2, _T("(íå)"));	
	}


}

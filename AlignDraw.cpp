// AlignDraw.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "AlignDraw.h"
#include "MainFrm.h"
#include "P8CA_LcDispDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int m_nGraphSel;
/////////////////////////////////////////////////////////////////////////////
// CAlignDraw dialog


CAlignDraw::CAlignDraw(CWnd* pParent /*=NULL*/)
	: CDialog(CAlignDraw::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAlignDraw)
		// NOTE: the ClassWizard will add member initialization here
	m_nGraphSel = 1;
	//}}AFX_DATA_INIT
}


void CAlignDraw::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAlignDraw)
	DDX_Control(pDX, IDC_ALIGN_DATA, m_ctrlAlignData);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlignDraw, CDialog)
	//{{AFX_MSG_MAP(CAlignDraw)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlignDraw message handlers

BOOL CAlignDraw::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CMainFrame *pFrame = (CMainFrame *) AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *) pFrame->GetActiveDocument();
	
	// TODO: Add extra initialization here
	CRect rect;
	GetDlgItem(IDC_GRAPH)->GetWindowRect(rect);
	ScreenToClient(rect);          //모니터에 있는 lefttop를 프로그램의 lefttop로 옴기는것 


	mp_TrendView = new TrendWindow();
	mp_TrendView->CreateEx(WS_EX_CLIENTEDGE, NULL, NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, rect, this, 21001);
	

	int i,j;
	double Max_X,Max_Y,Min_X,Min_Y,Avr_X,Avr_Y;
	int Avrcnt_X = 0 ,Avrcnt_Y = 0;
	double Temp_x = 0, Temp_y = 0;
	Max_X = Max_Y = -100;
	Min_X = Min_Y = 100;
//For Test 
// #include <math.h>
// 	for(i=0;i<120;i++)
// 	{
// 		pDoc->m_dAlignXTrace[i] = 0.0;
// 		pDoc->m_dAlignYTrace[i] = 0.0;
// 		while(1)
// 		{
// 			pDoc->m_dAlignXTrace[i] = rand();
// 			if(i%2 == 0)
// 			 pDoc->m_dAlignXTrace[i] = pDoc->m_dAlignXTrace[i] * -1;
// 			if(pDoc->m_dAlignXTrace[i] > -2000 && pDoc->m_dAlignXTrace[i] < 2000 ) break;
// 		}
// 		while(1)
// 		{
// 			pDoc->m_dAlignYTrace[i] = rand();
// 			if(i%3 == 0)
// 				pDoc->m_dAlignYTrace[i] = pDoc->m_dAlignYTrace[i] * -1;
// 			if(pDoc->m_dAlignYTrace[i] > -1500 && pDoc->m_dAlignYTrace[i] < 1500 ) break;
// 		}
// 	}
// 	pDoc->SaveMarkAlignResult();
/////////////////////////////////////////////////////
	for(i=0;i<100;i++)
	{
		if(pDoc->m_dAlignXTrace[i] != 0)
		{
// 			if(pDoc->m_dAlignXTrace[i] > Max_X)
// 				Max_X = pDoc->m_dAlignXTrace[i];
// 			if(pDoc->m_dAlignXTrace[i] < Min_X)
// 				Min_X = pDoc->m_dAlignXTrace[i];
			Avrcnt_X++;
			Temp_x += pDoc->m_dAlignXTrace[i];
		}
		if(pDoc->m_dAlignYTrace[i] != 0)
		{
// 			if(pDoc->m_dAlignYTrace[i] > Max_Y)
// 				Max_Y = pDoc->m_dAlignYTrace[i];
// 			if(pDoc->m_dAlignYTrace[i] < Min_Y)
// 				Min_Y = pDoc->m_dAlignYTrace[i];
			Avrcnt_Y++;
			Temp_y += pDoc->m_dAlignYTrace[i];
		}
	}

	double SortX[100],SortY[100];
	double TempSortX[100],TempSortY[100];
	for(i=0;i<100;i++)
	{
		SortX[i] = pDoc->m_dAlignXTrace[i];
		SortY[i] = pDoc->m_dAlignYTrace[i];
		TempSortX[i]=TempSortY[i]=0.0;
	}

	for(i=0;i<100;i++)
	{
		for(j=i+1;j<100;j++)
		{
			if(SortX[i]>SortX[j])
			{
				TempSortX[i] = SortX[i];
				SortX[i] = SortX[j];
				SortX[j] = TempSortX[i];
				
			}
		}
	}

	for(i=0;i<100;i++)
	{
		for(j=i+1;j<100;j++)
		{
			if(SortY[i]>SortY[j])
			{
				TempSortY[i] = SortY[i];
				SortY[i] = SortY[j];
				SortY[j] = TempSortY[i];
				
			}
		}
	}

	for(i=0;i<100;i++)
	{
		if(i == 99)
		{
			Max_X = SortX[i];
			break;
		}
	}
	for(i=0;i<100;i++)
	{
		if(i == 99)
		{
			Max_Y = SortY[i];
			break;
		}
	}

	Min_X = SortX[0];
	Min_Y = SortY[0];
	Avr_X = (double)(Temp_x/Avrcnt_X);
	Avr_Y = (double)(Temp_y/Avrcnt_Y);

	CString str;
	str.Format("X 최대값 %.2f 최소값 %.2f 평균값 %.2f\nY 최대값 %.2f 최소값 %.2f 평균값 %.2f"
		,Max_X,Min_X,Avr_X,Max_Y,Min_Y,Avr_Y);
	m_ctrlAlignData.SetCaption(str);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAlignDraw::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	mp_TrendView->SendMessage(WM_CLOSE);
	delete mp_TrendView; mp_TrendView=NULL;		
}

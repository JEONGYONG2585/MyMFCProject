// TrendWindow.cpp : implementation file
//

#include "stdafx.h"
//#include "RealGraph.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"
#include "P8CA_LcDispDoc.h"
#include "P8CA_LcDispView.h"


#include "TrendWindow.h"
#include <math.h>
#include <fstream.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern double m_dAdjustError;
extern int m_nGraphSel;
//2011.05.12 by tskim
extern BOOL g_bMultiTargetMeas;
/////////////////////////////////////////////////////////////////////////////
// TrendWindow

TrendWindow::TrendWindow() : CWnd()
{
	for(int i=0;i<MAX_BUFFER;i++) { m_data[i] = 0.0; m_dataBK[i]=0.0;}

	mp_displayMemory = new CDC();
	mp_bitmap = new CBitmap();
	m_timer_flag = 0;
	m_dHRange=0.0, m_dLRange=0.0;
	m_nHead=0;
	m_nRealWidth=0; m_nRealHeight=0;
	m_nData = 0;
}

TrendWindow::~TrendWindow()
{
}

BEGIN_MESSAGE_MAP(TrendWindow, CWnd)
	//{{AFX_MSG_MAP(TrendWindow)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// TrendWindow message handlers

//  "수평/수직" 축을 그린다.
void TrendWindow::DrawAxis(CDC *parm_dc)
{
	CPen *p_old_pen = (CPen *)parm_dc->SelectStockObject(WHITE_PEN);
//	int m_nRealWidth = m_rect.Width() - 40, m_nRealHeight = m_rect.Height() - 40;

	// m_rect의 우측,하단의 좌표를 구한다.
//	TRACE("m_rect.Width:%d\n",m_rect.Width());
//	TRACE("m_rect.Height:%d\n",m_rect.Height());

	if(m_nGraphSel == 0)
	{
		parm_dc->MoveTo(40, m_nRealHeight);
		parm_dc->LineTo(m_nRealWidth + 30, m_nRealHeight);
		parm_dc->MoveTo(40, 16);
		parm_dc->LineTo(40, m_nRealHeight);
	}
	else if(m_nGraphSel == 1)
	{
		parm_dc->MoveTo(20, int(m_nRealHeight/8)*(4) /*m_nRealHeight/2*/);
		parm_dc->LineTo(m_nRealWidth-20,int(m_nRealHeight/8)*(4) /*m_nRealHeight/2*/);
		parm_dc->MoveTo(int((m_nRealWidth/12)*(6)) /*m_nRealWidth/2*/, 20);
		parm_dc->LineTo(int((m_nRealWidth/12)*(6)) /*m_nRealWidth/2*/, m_nRealHeight-20);

// 		for(int i = 0; i < 7; i++)
// 		{          
// 			parm_dc->MoveTo(20, int(m_nRealHeight/8)*(i+1));
// 			parm_dc->LineTo(m_nRealWidth-20, int(m_nRealHeight/8)*(i+1));
// 		}
		
		// 수직 그리드를 그린다.
// 		for(i = 0; i <11; i++)
// 		{ 
// 			parm_dc->MoveTo(int((m_nRealWidth/12)*(5)), 20);
// 			parm_dc->LineTo(int((m_nRealWidth/12)*(i+1)), m_nRealHeight-20);
// 		}
	}

	parm_dc->SelectObject(p_old_pen);
}

void TrendWindow::DrawRange(CDC *parm_dc)
{
	CPen pen, *p_old_pen;
	pen.CreatePen(PS_SOLID,1,RED);
	p_old_pen = (CPen *)parm_dc->SelectObject(&pen);
	int nHRange=0, nLRange=0;
//
	if(m_nGraphSel == 0)
	{
		nHRange=m_nRealHeight/2+10 - (int)((m_nRealHeight-20)*m_dHRange/17.0);
		nLRange=m_nRealHeight/2+10 - (int)((m_nRealHeight-20)*m_dLRange/17.0);
		
		parm_dc->MoveTo(40, nHRange);
		parm_dc->LineTo(m_nRealWidth+20, nHRange);
		parm_dc->MoveTo(40, nLRange);
		parm_dc->LineTo(m_nRealWidth+20, nLRange);
	}
	else if(m_nGraphSel == 1)
	{
//		nHRange=m_nRealHeight/2+10 - (int)((m_nRealHeight-20)*m_dHRange/17.0);
//		nLRange=m_nRealHeight/2+10 - (int)((m_nRealHeight-20)*m_dLRange/17.0);
//		
//		parm_dc->MoveTo(40, nHRange);
//		parm_dc->LineTo(m_nRealWidth+20, nHRange);
//		parm_dc->MoveTo(40, nLRange);
//		parm_dc->LineTo(m_nRealWidth+20, nLRange);
	}
	parm_dc->SelectObject(p_old_pen);
}

void TrendWindow::DrawGrid(CDC *parm_dc)
{
	CPen grid_pen(PS_DOT, 1, RGB(0, 128, 0));
	CPen *p_old_pen = parm_dc->SelectObject(&grid_pen);
	double ystep, xstep ;// = (m_rect.Height() - 60)/double(4);

	if(m_nGraphSel == 0)
	{
		ystep = (m_rect.Height() - 60)/double(4);
		// 수평 그리드를 그린다. 
		for(int i = 0; i < 5; i++)
		{          
			parm_dc->MoveTo(40, int(m_nRealHeight - (i+1)*ystep));
			parm_dc->LineTo(m_nRealWidth+20, int(m_nRealHeight - (i+1)*ystep));
		}
		
		// 수직 그리드를 그린다.
		for(i = 1; i <= MAX_BUFFER/5; i++)
		{ 
			parm_dc->MoveTo(40 + i*20 , 20);
			parm_dc->LineTo(40 + i*20, m_nRealHeight);
		}
	}
	else if(m_nGraphSel == 1)
	{
//		ystep = (m_rect.Height() - 60)/double(4);
//		xstep = m_rect.Width();
		// 수평 그리드를 그린다. 
		for(int i = 0; i < 7; i++)
		{     
			if(i !=3)
			{
				parm_dc->MoveTo(20, int(m_nRealHeight/8)*(i+1));
				parm_dc->LineTo(m_nRealWidth-20, int(m_nRealHeight/8)*(i+1));
			}
		}
		
		// 수직 그리드를 그린다.
		for(i = 0; i <11; i++)
		{ 
			if(i != 5)
			{
				parm_dc->MoveTo(int((m_nRealWidth/12)*(i+1)), 20);
				parm_dc->LineTo(int((m_nRealWidth/12)*(i+1)), m_nRealHeight-20);
			}
		}
	}

	parm_dc->SelectObject(p_old_pen);
	grid_pen.DeleteObject();
}

void TrendWindow::DrawGraph(CDC *parm_dc)
{
	CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc * pDoc = (CP8CA_LcDispDoc *) pFrame->GetActiveDocument();

	CPen graph_pen(PS_SOLID, 1, RGB(0, 255, 0));
	CPen *p_old_pen = parm_dc->SelectObject(&graph_pen);
	CRect rect;

// 	for(int i = 0; i <100; i++)
// 	{
// 		pDoc->m_dAlignXTrace[i] = pDoc->m_dAlignYTrace[i] = 0.0;
// 	}

	if(m_nGraphSel == 0)
	{
		for(int i = 0; i <MAX_BUFFER; i++)
		{
			if(i) parm_dc->LineTo(40 + (i+1)*4, m_nRealHeight/2+10 - int((m_nRealHeight-20)*m_dataBK[i]/10.0));
			else parm_dc->MoveTo(40 + (i+1)*4, m_nRealHeight/2+10 - int((m_nRealHeight-20)*m_dataBK[i]/10.0));
		}
	}
	else if(m_nGraphSel == 1)
	{
		for(int i = 0; i <MAX_BUFFER; i++)
		{
			parm_dc->MoveTo((m_nRealWidth/12)*6 - int((pDoc->m_dAlignXTrace[i]/500)*(m_nRealWidth/12))-3, (m_nRealHeight/8)*4 + int((pDoc->m_dAlignYTrace[i]/500)*(m_nRealHeight/8))-3);
			rect.bottom=(m_nRealHeight/8)*4 + int((pDoc->m_dAlignYTrace[i]/500)*(m_nRealHeight/8))+3;
			rect.top=(m_nRealHeight/8)*4 + int((pDoc->m_dAlignYTrace[i]/500)*(m_nRealHeight/8))-3;
			rect.right = (m_nRealWidth/12)*6 - int((pDoc->m_dAlignXTrace[i]/500)*((m_nRealWidth/12)))+3;
			rect.left = (m_nRealWidth/12)*6 - int((pDoc->m_dAlignXTrace[i]/500)*((m_nRealWidth/12)))-3;
			parm_dc->Ellipse(rect);
//			parm_dc->FloodFill(rect.top,rect.right,RED);
		}
	}
	parm_dc->SelectObject(p_old_pen);
	graph_pen.DeleteObject();
}

void TrendWindow::DrawValue(CDC *parm_dc)
{
	double ystep = (m_rect.Height() - 60)/double(4);
	CString strRange[5] = {"-0.8","-0.4","0%","0.4","0.8"};
	CString str=""; int i=0;

	if(m_nGraphSel == 0)
	{
		parm_dc->SetTextColor(YELLOW);
		parm_dc->SetTextAlign(TA_CENTER | TA_BOTTOM);
		
		
		str.Format("data = %.3f", m_data[0]); // first data
		parm_dc->TextOut(50, m_rect.Height(), str);
		
		for(i = 0; i < 5; i++)
		{          
			parm_dc->TextOut(20 , int(m_nRealHeight+10 - (i)*ystep), strRange[i]);
		}
		
		for(i = 1; i <= MAX_BUFFER/10; i++)
		{ 
			str.Format("%d",i*10);
			parm_dc->TextOut(40 + i*20*2, m_nRealHeight+20, str);
		}
	}
	else if(m_nGraphSel == 1)
	{
		parm_dc->SetTextColor(YELLOW);
		parm_dc->SetTextAlign(TA_CENTER | TA_BOTTOM);
		
//		str.Format("data = %.3f", m_data[0]); // first data
//		parm_dc->TextOut(50, m_rect.Height(), str);
		for(int i = 0; i < 7; i++)
		{          
			str.Format("%d",-2000+(500*(i+1)));
			parm_dc->TextOut(20 , int(m_nRealHeight/8)*(i+1), str);
		}
		
		// 수직 그리드를 그린다.
		for(i = 0; i <11; i++)
		{ 
			str.Format("%d",3000-(500*(i+1)));
			parm_dc->TextOut(int((m_nRealWidth/12)*(i+1)), m_nRealHeight-20, str);
		}		
	}
}

void TrendWindow::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// 화면의 부드러운 갱신을 위해 MemoryDC & bitmap 방식을 이용한다..
	p_old_bitmap = (CBitmap *) mp_displayMemory->SelectObject(mp_bitmap);
	CBrush *old_brush = (CBrush *) mp_displayMemory->SelectStockObject(BLACK_BRUSH);
	
	mp_displayMemory->Rectangle(m_rect);
	mp_displayMemory->SelectObject(old_brush);
	
	int old_mode = mp_displayMemory->SetBkMode(TRANSPARENT);
	
	DrawAxis(mp_displayMemory);
	DrawRange(mp_displayMemory);
	DrawGrid(mp_displayMemory);
	DrawGraph(mp_displayMemory);
	DrawValue(mp_displayMemory);
	
	mp_displayMemory->SetBkMode(old_mode);
	
	dc.BitBlt(0, 0, m_rect.Width(), m_rect.Height(), mp_displayMemory, 0, 0, SRCCOPY);
	mp_displayMemory->SelectObject(p_old_bitmap);
}

int TrendWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc * pDoc = (CP8CA_LcDispDoc *) pFrame->GetActiveDocument();
//

		if(CWnd::OnCreate(lpCreateStruct) == -1) return -1;
		CString str="";

		srand((unsigned)time(NULL));
		GetClientRect(m_rect);

		CClientDC dc(this);
		mp_displayMemory->CreateCompatibleDC(&dc); // Memory DC를 사용하기 위한 정보를 구성한다..
		mp_bitmap->CreateCompatibleBitmap(&dc, m_rect.Width(), m_rect.Height()); // Memory DC에 사용할 Bitmap을 생성한다..
		//
		if(m_nGraphSel == 0)
		{
			if((ThBal[BALID1].bAdjust == TRUE)||(ThBal[BALID2].bAdjust == TRUE)||(ThBal[BALID3].bAdjust == TRUE)
				||(ThBal[BALID4].bAdjust == TRUE)||(ThBal[BALID5].bAdjust == TRUE)||(ThBal[BALID6].bAdjust == TRUE)
				||(ThBal[BALID7].bAdjust == TRUE)||(ThBal[BALID8].bAdjust == TRUE))
			{
				m_dHRange=m_dAdjustError*10.0;
				m_dLRange=-m_dAdjustError*10.0;
			}
			else
			{
				m_dHRange=pDoc->m_structAdjustCondition.dTargetError[m_nHead]*10.0;
				m_dLRange=-pDoc->m_structAdjustCondition.dTargetError[m_nHead]*10.0;
			}
			
			m_nRealWidth = m_rect.Width() - 40; m_nRealHeight = m_rect.Height() - 40;
		}
		else if(m_nGraphSel == 1)
		{
			m_dHRange = (CAMH*10.0)/2.0;
			m_dLRange = -(CAMH*10.0)/2.0;
			m_nRealHeight = m_rect.Height();
			m_nRealWidth = m_rect.Width();
		}
	return 0;
}

void TrendWindow::OnDestroy() 
{
	StopGraph();

	// 객체를 복사하기 위해 사용했던 메모리 공간을 해제한다.. ( Local Buffer )
	mp_bitmap->DeleteObject();    // Bitmap 정보를 제거한다..
	delete mp_bitmap;             // Bitmap Object를 제거한다..
	mp_displayMemory->DeleteDC();  // Memory DC 정보를 제거한다..
	delete mp_displayMemory;       //$ Memory DC Object를 제거한다..

	CWnd::OnDestroy();
}

void TrendWindow::OnTimer(UINT nIDEvent) 
{
	CMainFrame * pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc * pDoc = (CP8CA_LcDispDoc *) pFrame->GetActiveDocument();

	if(nIDEvent == 1)
	{
		if(m_nGraphSel == 0)
		{
			// memcpy( dest, source , count )
			if(m_nData == 0)
			{
				memcpy(m_data, pDoc->m_dMeasuredData[m_nHead], sizeof(float)*MAX_BUFFER);
				memcpy(m_dTarget, pDoc->m_dMeasuredData_Target[m_nHead], sizeof(float)*MAX_BUFFER);
			}
			else if(m_nData == 1)
				memcpy(m_data, pDoc->m_dMeasuredData_First[m_nHead],sizeof(float)*MAX_BUFFER);

//			for(int i=0 ; i<MAX_BUFFER; i++)
//			{
//				m_dataBK[i]=float((m_data[i]-pDoc->m_structAdjustCondition.dTargetWeight[m_nHead])
//					/pDoc->m_structAdjustCondition.dTargetWeight[m_nHead]*1000.0);
//				if(m_dataBK[i] > 5.0) m_dataBK[i]=5.0;
//				else if(m_dataBK[i] <-5.0) m_dataBK[i]=-5.0;
//			}
			for(int i=0 ; i<MAX_BUFFER; i++)
			{
//				if(/*pDoc->m_structDataEditor.m_nCalibrationMode == 2 && */g_bMultiTargetMeas)
//				{
					m_dataBK[i]=float((m_data[i]-m_dTarget[i])
					/m_dTarget[i]*1000.0);
//				}
//				else
//				{
//					m_dataBK[i]=float((m_data[i]-pDoc->m_structAdjustCondition.dTargetWeight[m_nHead])
//					/pDoc->m_structAdjustCondition.dTargetWeight[m_nHead]*1000.0);
//				}
				if(m_dataBK[i] > 5.0) m_dataBK[i]=5.0;
				else if(m_dataBK[i] <-5.0) m_dataBK[i]=-5.0;
			}
			TRACE("m_data:%.3f\n",m_data[0]);
			
			Invalidate(FALSE);
		}
		else if(m_nGraphSel == 1)
		{
			// memcpy( dest, source , count )
// 			memcpy(m_data, pDoc->m_dMeasuredData[m_nHead], sizeof(float)*MAX_BUFFER);
// 			for(int i=0 ; i<MAX_BUFFER; i++)
// 			{
// 				m_dataBK[i]=float((m_data[i]-pDoc->m_structAdjustCondition.dTargetWeight[m_nHead])
// 					/pDoc->m_structAdjustCondition.dTargetWeight[m_nHead]*1000.0);
// 				if(m_dataBK[i] > 5.0) m_dataBK[i]=5.0;
// 				else if(m_dataBK[i] <-5.0) m_dataBK[i]=-5.0;
// 			}
// 			TRACE("m_data:%.3f\n",m_data[0]);
			
			Invalidate(FALSE);
		}
	} 
	else CWnd::OnTimer(nIDEvent);
}

void TrendWindow::StartGraph()
{
	if(m_nGraphSel == 0)
	{
		// m_timer_flag는 char type..
		if(!m_timer_flag)
		{
			SetTimer(1, 100, NULL);
			m_timer_flag = 1;
		}
	}
}

void TrendWindow::StopGraph()
{
	if(m_nGraphSel == 0)
	{
		if(m_timer_flag)
		{
			KillTimer(1);	
			m_timer_flag = 0;
		}
	}
}


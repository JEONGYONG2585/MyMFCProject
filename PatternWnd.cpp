// PatternWnd.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"

#include "PatternWnd.h"
#include "DrawPattern.h"

//#include "EditPatIndic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern BOOL g_bOnly_Center;
extern BOOL g_bOnly_Nozzle;
extern BOOL g_bMouse_Down;
extern BOOL g_b1Drop_Disp;
/////////////////////////////////////////////////////////////////////////////
// CPatternWnd

CPatternWnd::CPatternWnd()
{
	m_dDefaultZoom = 0;
	m_dZoom = 1.;
	m_nPatternNum = 1;
	m_pOrgPoint = CPoint(0, 0);
	m_colorBackground = RGB(0, 0, 0);
	m_colorForeground = RGB(255, 255, 0);
	m_bIsBig = TRUE;
	m_bIsDirectSetting = FALSE;
	m_bIsDispenseOrder = FALSE;
	m_dBigClientX = m_dBigClientY = 100;
	m_dAboutBigZoom = 2.4;
	m_dBigZoom = 1;
	m_dBigDefaultZoom = 1;
	m_nCurrentDraw = 0;
	
	for(int i = 0; i < MAX_PATTERN + 1; i++) //flexible... Yamary...
	{
		for(int j = 0; j < MAX_PATTERN + 1; j++)
			m_nSeq[i]/*[j]*/ = 0;
	}
	m_bCheck = FALSE;
	m_nCurSelPointNum = 0;
	m_bFirst = TRUE;
	m_nDispPanel = 0;
}

CPatternWnd::~CPatternWnd()
{
	if(m_bIsDispenseOrder) KillTimer(0);
}


BEGIN_MESSAGE_MAP(CPatternWnd, CWnd)
	//{{AFX_MSG_MAP(CPatternWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPatternWnd message handlers

void CPatternWnd::OnPaint() 
{

	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CDrawPattern drawPattern;

	// 이부분은 전체 그리기나 개별 그리기나 통틀어서 한번식만 실행 되면 되는 부분임 /////////
	ApplyDC(&dc);
	m_pDC = &dc;

	CP8CA_LcDispDoc *pDoc = GetDocument();

	SetDefaultZoom();	// 기본 줌을 설정한다. 한번만 실행 됨


	// 배경을 그린다.
	if(!m_bIsBig && !m_bIsDispenseOrder)
		DrawForeground(&dc);

	if(m_nCurrentDraw == 0)
		DrawBackground(&dc);
	/////////////////////////////////////////////////////////////////////////////////////////

	// 패턴을 그린다. //////////////////////////////////////////////////////////////////////
	// zoom 설정%
	if(m_bIsBig && !m_bIsDispenseOrder) drawPattern.m_dZoom = m_dZoom;
	else drawPattern.m_dZoom = m_dDefaultZoom;
	
	// orgpoint 설정
	if(m_bIsBig && !m_bIsDispenseOrder) drawPattern.m_pOrgPoint = m_pOrgPoint;
	else drawPattern.m_pOrgPoint = CPoint(0, 0);

	// drop point반지름 설정
	if(!m_bIsBig) 
	{
		drawPattern.m_pDrop->m_dRadius = 2;
		drawPattern.m_pDrop->m_nAddDelSize = 2;
	}

	// 큰창인지 작인 창인지 결정. 2003. 07. 25.
	drawPattern.m_bIsBig = m_bIsBig;

	m_bIsDisplayText2 = FALSE;
	// text 설정
	if(m_dZoom > m_dDefaultZoom * 6.) m_bIsDisplayText = TRUE;
	else m_bIsDisplayText = FALSE;
	if(m_dZoom > m_dDefaultZoom * 10.) m_bIsDisplayText2 = TRUE;
//	if(m_nPatternNum == -2) m_bIsDisplayText = FALSE; // 패턴 표시창에서도 숫자가 표시되게 삭제. 2003. 07. 25.
	drawPattern.m_bIsDisplayText = m_bIsDisplayText; // 위에서 아래로 이동 2003. 07. 25.
	drawPattern.m_bIsDisplayText2 = m_bIsDisplayText2;

	pDoc->m_nDropcount = 0;
	pDoc->m_nMMGDropcount = 0; 
	pDoc->m_nMainDropCount = 0; 

	for(int nCount = 0; nCount < pDoc->m_structDataEditor.m_nPatternNum; nCount++){
		if ( pDoc->m_structDataEditor.m_bMMGLineDropMode ){
			if ( !pDoc->m_structPatternData[nCount].m_bMMG && pDoc->nTempMainCount == 0){
				pDoc->nTempMainCount = 	nCount; 		
			}
		}
	}
	

		// 그릴 패턴들 설정
	if(m_nPatternNum > 0)			// 개별 패턴을 그릴때
	{
		drawPattern.SetPattern(&dc, m_nPatternNum);
		if(m_bIsBig) 
			drawPattern.m_BigRect = m_BigRect;
		drawPattern.Draw();
	}
	else if(m_nPatternNum == 0)     // 모든 패턴을 그릴때
	{
		if(m_nCurrentDraw == -1) m_nCurrentDraw = 0;
		else
		{
			for(int i = 0; i < pDoc->m_structDataEditor.m_nPatternNum; i++) // flexible... Yamary...
			{
				if(m_nSeq/*[i]*/[m_nCurrentDraw] == 0) continue;
				drawPattern.SetPattern(&dc, m_nSeq/*[i]*/[m_nCurrentDraw]);
				if(m_bIsBig)
					drawPattern.m_BigRect = m_BigRect;
				drawPattern.Draw();
			}
		}
	}
	else if(m_nPatternNum == -1)    // 아무것도 그리지 않을때
	{
		//
	}
	else if(m_nPatternNum == -2)	// 모든 패턴을 그리지만, 움직이진 않는다.
	{
		for(int i = 0; i < pDoc->m_structDataEditor.m_nPatternNum; i++)
		{
//			//for(int j = 0; j < pDoc->m_structDataEditor.m_nPatternNum; j++)
//			{
//				// m_nSeq[i] < 0 이면 아래 두 함수에서 메모리 참조 오류가 발생한다(8월12일)
//				if( m_nSeq[i]/*[j]*/ > 0)
//				{
//					drawPattern.SetPattern(&dc, m_nSeq[i]/*[j]*/); // flexible... Yamary...
//					if(m_bIsBig)
//						drawPattern.m_BigRect = m_BigRect;
//					drawPattern.Draw();
//				}
//			}

			if( m_nSeq[i] > 0)
			{
				if ( pDoc->m_structDataEditor.m_bSelectDropMode )
				{
					if(pDoc->m_structDataEditor.m_nNzlMode != 1)
					{
						if(i == 1 && m_nSeq[0] == 0)
						{
							if ( pDoc->m_structDataEditor.m_bMMGLineDropMode )
							{
								if ( pDoc->m_structPatternData[i].m_bMMG )
												pDoc->m_nMMGDropcount = 0; 
								else
												pDoc->m_nMainDropCount = 0;		
							}
							else
								pDoc->m_nDropcount = 0;	
						}
						else
						{
							if ( pDoc->m_structDataEditor.m_bMMGLineDropMode )
							{						
								if ( pDoc->m_structPatternData[i].m_bMMG ){
									pDoc->m_nMMGDropcount += pDoc->m_structPatternData[i-1].m_nDropDataCount;

									drawPattern.SetPattern(&dc, m_nSeq[i]);
									drawPattern.Draw();
								}
								else{
									if ( i != pDoc->nTempMainCount )
										pDoc->m_nMainDropCount += pDoc->m_structPatternData[i-1].m_nDropDataCount;

									drawPattern.SetPattern(&dc, m_nSeq[i]);
									drawPattern.Draw();
								}
							}
							else
							{
									pDoc->m_nDropcount += pDoc->m_structPatternData[i-1].m_nDropDataCount;

									drawPattern.SetPattern(&dc, m_nSeq[i]);
									drawPattern.Draw();	
							}
						}
					}
					else
					{
						drawPattern.SetPattern(&dc, m_nSeq[i]/*[j]*/); // flexible... Yamary...

						if(m_bIsBig)
							drawPattern.m_BigRect = m_BigRect;
						drawPattern.Draw();						
					}
				}
				else
				{
					drawPattern.SetPattern(&dc, m_nSeq[i]/*[j]*/); // flexible... Yamary...

					if(m_bIsBig)
						drawPattern.m_BigRect = m_BigRect;
					drawPattern.Draw();
				}	
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////////
	if(!m_bIsBig && !m_bIsDispenseOrder)
	{
//		DrawPanel(&dc);
//		DrawForeground(&dc);
	} // 2013.09.04 by tskim

	// Do not call CWnd::OnPaint() for painting messages

// 	CPaintDC dc(this); // device context for painting
// 	
// 	// TODO: Add your message handler code here
// 	CDrawPattern drawPattern;
// 
// 	// 이부분은 전체 그리기나 개별 그리기나 통틀어서 한번식만 실행 되면 되는 부분임 /////////
// 	ApplyDC(&dc);
// 	m_pDC = &dc;
// 
// 	CP8CA_LcDispDoc *pDoc = GetDocument();
// 
// 	SetDefaultZoom();	// 기본 줌을 설정한다. 한번만 실행 됨
// 
// 
// 	// 배경을 그린다.
// 	if(!m_bIsBig && !m_bIsDispenseOrder)
// 		DrawForeground(&dc);
// 
// 	if(m_nCurrentDraw == 0)
// 		DrawBackground(&dc);
// 	/////////////////////////////////////////////////////////////////////////////////////////
// 
// 	// 작업자가 보고자 하는 위치 결정
// 	if(m_nPosOper==1)	drawPattern.m_nPosOper=1;				//작업자 1 위치
// 	else if(m_nPosOper==(-1))	drawPattern.m_nPosOper=(-1);	//작업자 2 위치
// 
// 	// 패턴을 그린다. //////////////////////////////////////////////////////////////////////
// 	// zoom 설정%
// 	if(m_bIsBig && !m_bIsDispenseOrder) drawPattern.m_dZoom = m_dZoom;
// 	else drawPattern.m_dZoom = m_dDefaultZoom;
// 	
// 	// orgpoint 설정
// 	if(m_bIsBig && !m_bIsDispenseOrder) drawPattern.m_pOrgPoint = m_pOrgPoint;
// 	else drawPattern.m_pOrgPoint = CPoint(0, 0);
// 
// 	// drop point반지름 설정
// 	if(!m_bIsBig) 
// 	{
// 		drawPattern.m_pDrop->m_dRadius = 2;
// 		drawPattern.m_pDrop->m_nAddDelSize = 2;
// 	}
// 
// 	// 큰창인지 작인 창인지 결정. 2003. 07. 25.
// 	drawPattern.m_bIsBig = m_bIsBig;
// 
// 	// text 설정
// 	if(m_dZoom > m_dDefaultZoom * 6.) m_bIsDisplayText = TRUE;
// 	else if(m_dZoom > (m_dDefaultZoom * 1.5))
// 	{
// 		if(m_bTextDisp){	//Text표시를 사용자 선택에 의해 원하는 경우
// 			m_bIsDisplayText = TRUE;
// 		}
// 		else	m_bIsDisplayText = FALSE;
// 	}
// 	else	m_bIsDisplayText = FALSE;
// //	else m_bIsDisplayText = FALSE;
// 	m_bIsDisplayText2 = FALSE;
// 	if(m_dZoom > m_dDefaultZoom * 10.) m_bIsDisplayText2 = TRUE;
// //	if(m_nPatternNum == -2) m_bIsDisplayText = FALSE; // 패턴 표시창에서도 숫자가 표시되게 삭제. 2003. 07. 25.
// 	drawPattern.m_bIsDisplayText = m_bIsDisplayText; // 위에서 아래로 이동 2003. 07. 25.
// 	drawPattern.m_bIsDisplayText2 = m_bIsDisplayText2;
// 
// 	pDoc->m_nMMGDropcount = 0; 
// 
// 		// 그릴 패턴들 설정
// 	if(m_nPatternNum > 0)			// 개별 패턴을 그릴때
// 	{
// 		drawPattern.SetPattern(&dc, m_nPatternNum);
// 		if(m_bIsBig) 
// 			drawPattern.m_BigRect = m_BigRect;
// 		drawPattern.Draw();
// 	}
// 	else if(m_nPatternNum == 0)     // 모든 패턴을 그릴때
// 	{
// 		if(m_nCurrentDraw == -1) m_nCurrentDraw = 0;
// 		else
// 		{
// 			for(int i = 0; i < pDoc->m_structDataEditor.m_nPatternNum; i++) // flexible... Yamary...
// 			{
// 				if(m_nSeq/*[i]*/[m_nCurrentDraw] == 0) continue;
// 				drawPattern.SetPattern(&dc, m_nSeq/*[i]*/[m_nCurrentDraw]);
// 				if(m_bIsBig)
// 					drawPattern.m_BigRect = m_BigRect;
// 				drawPattern.Draw();
// 			}
// 		}
// 	}
// 	else if(m_nPatternNum == -1)    // 아무것도 그리지 않을때
// 	{
// 		//
// 	}
// 	else if(m_nPatternNum == -2)	// 모든 패턴을 그리지만, 움직이진 않는다.
// 	{
// 		for(int i = 0; i < pDoc->m_structDataEditor.m_nPatternNum; i++)
// 		{
// 			//for(int j = 0; j < pDoc->m_structDataEditor.m_nPatternNum; j++)
// 			{
// 				// m_nSeq[i] < 0 이면 아래 두 함수에서 메모리 참조 오류가 발생한다(8월12일)
// 				if( m_nSeq[i]/*[j]*/ > 0)
// 				{
// 					drawPattern.SetPattern(&dc, m_nSeq[i]/*[j]*/); // flexible... Yamary...
// 					if(m_bIsBig)
// 						drawPattern.m_BigRect = m_BigRect;
// 					drawPattern.Draw();
// 				}
// 			}
// 		}
// 	}
// 	////////////////////////////////////////////////////////////////////////////////////
// 	if(!m_bIsBig && !m_bIsDispenseOrder)
// 	{
// //		DrawPanel(&dc);
// //		DrawForeground(&dc);
// 	} // 2013.09.04 by tskim
// 
// 	// Do not call CWnd::OnPaint() for painting messages
}

CP8CA_LcDispDoc * CPatternWnd::GetDocument()
{
	CMainFrame *pFrame = (CMainFrame *)::AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	return pDoc;
}


// dc관련 설정
void CPatternWnd::ApplyDC(CDC *pDC)
{
	// 사각 박스에 대한 dc 설정
	// 기본 좌표계 설정(1mm 단위) ///
	
	pDC->SetMapMode(MM_ISOTROPIC);
	pDC->SetWindowExt(1, 1);
	pDC->SetViewportExt(1, -1);

	CRect rect;
	GetClientRect(&rect);
	pDC->DPtoLP(&rect);

	pDC->SetWindowOrg(-rect.right / 2, -rect.bottom / 2);
	pDC->SetViewportOrg(0, 0);

	pDC->SetBkMode(TRANSPARENT);
	OnGetRect(rect);
	///////////////////////////////////////
}


// 처음에 화면에 꽉 맞추기 위해 줌을 딱 한번 설정한다.
void CPatternWnd::SetDefaultZoom()
{
	// 한번 설정되고 나면 계속 skip된다
	if(m_dDefaultZoom == 0)
	{
		CP8CA_LcDispDoc *pDoc = GetDocument();

		// 처음엔 줌을 화면에 꽉 차게 맞춘다.
		CRect rect;
		GetClientRect(rect);

		// x, y방향 스케일을 모두 측정한다.
		// 측정해서 스케일이 작은쪽을 실제 스케일로 사용한다.
		double dXZoom = 0.0 , dYZoom = 0.0;
		dXZoom = rect.Width() / pDoc->m_structDataEditor.m_dGlassSizeX;
		dYZoom = rect.Height() / pDoc->m_structDataEditor.m_dGlassSizeY;

//		dXZoom  = dXZoom*0.9;//For Test 2013.09.12 by tskim
//		dYZoom  = dYZoom*0.9;

		// 기본 줌 설정
		if(dXZoom < dYZoom) m_dDefaultZoom = dXZoom;
		else m_dDefaultZoom = dYZoom;

		m_dZoom = m_dDefaultZoom;
	}
}

BOOL CPatternWnd::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	CRect rect;
	CBrush newBrush;
	CBrush *oldBrush;
	CPen newPen;
	CPen *oldPen;
	
	//by syk
//	if(m_bIsBig)
//		m_colorBackground = RGB(255, 255, 255);
//	else
//		m_colorBackground = RGB(255, 255, 255);

	if(m_bIsDispenseOrder)//by syk
		newBrush.CreateStockObject(NULL_BRUSH);//CreateSolidBrush(m_colorBackground);
	else
		newBrush.CreateSolidBrush(RGB(255, 255, 255));

	oldBrush = pDC->SelectObject(&newBrush);

	newPen.CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
	oldPen = pDC->SelectObject(&newPen);


	GetClientRect(rect);
	pDC->Rectangle(rect);

	
	// 브러쉬, 펜 지우기
	pDC->SelectObject(oldBrush);
	newBrush.DeleteObject();
	pDC->SelectObject(oldPen);
	newPen.DeleteObject();
		
	return CWnd::OnEraseBkgnd(pDC);
}

// 마우스를 클릭하면 drop data중 하나가 선택되었는지 확인한다.
void CPatternWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_bRepairEditMode==TRUE)
	{
		if(m_bIsBig && !m_bIsDispenseOrder) 
		{
			CDC *pDC;
			pDC = GetDC();
			
			// 좌표를 현재 dc에 맞게 적용
			ApplyDC(pDC);
			pDC->DPtoLP(&point);

			double dX, dY;
			dX = (double)point.x;
			dY = (double)point.y;

			// org 와 zoom 적용 ////////////////
			dX = dX - m_pOrgPoint.x;
			dY = dY - m_pOrgPoint.y;
			dX = dX / m_dZoom;
			dY = dY / m_dZoom;
			////////////////////////////////////

			// 실제로 적용된 좌표를 drawpattern을 통해서 선택되었는지 판단
			CDrawPattern drawPattern;
			drawPattern.m_dZoom = m_dZoom;
			drawPattern.m_pOrgPoint = m_pOrgPoint;
			drawPattern.SetPattern(pDC, m_nPatternNum);
			drawPattern.m_bIsDirectSetting = m_bIsDirectSetting;

			// direct setting모드로 작동하면 drop이 선택되면 더블 클릭 메시지를 부모에게 보낸다.
			drawPattern.CheckSelect(dX, dY);
			m_nCurSelDropNum = drawPattern.m_nCurSelDropNum;
		
			// 다시 그림
			Invalidate();
			// dc 해제
			ReleaseDC(pDC);
		
			if(m_bIsDirectSetting) GetParent()->SendMessage(WM_LBUTTONDBLCLK);
		}
		
		if(!m_bIsDirectSetting) CWnd::OnLButtonUp(nFlags, point);
	}
}

BOOL CPatternWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default

	
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}


// 배경을 그린다.
void CPatternWnd::DrawBackground(CDC *pDC)
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	CPen newPen;
	CPen *oldPen;

	CBrush newBrush;
	CBrush *oldBrush;


	// 외곽선 펜 생성
//	newPen.CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
	newPen.CreatePen(PS_SOLID, 2, DARKMAGENTA);//by syk  1->2
	oldPen = pDC->SelectObject(&newPen);

	// 브러쉬 생성
	if(m_bIsBig)// m_bIsDispenseOrder)
		newBrush.CreateSolidBrush(RGB(255, 255, 255));
	else 	
		newBrush.CreateStockObject(NULL_BRUSH);
		
	oldBrush = pDC->SelectObject(&newBrush);

	double dL, dR, dT, dB;
	
	dL = pDoc->m_structDataEditor.m_dGlassSizeX / -2.;
	dR = pDoc->m_structDataEditor.m_dGlassSizeX / 2.;
	dT = pDoc->m_structDataEditor.m_dGlassSizeY / 2.;
	dB = pDoc->m_structDataEditor.m_dGlassSizeY / -2.;

	// 좌표에 스케일과 org 적용
	dL = dL * m_dZoom;
	dR = dR * m_dZoom;
	dT = dT * m_dZoom;
	dB = dB * m_dZoom;

	if(m_bIsBig)
	{
		dL += m_pOrgPoint.x;
		dR += m_pOrgPoint.x;
		dT += m_pOrgPoint.y;
		dB += m_pOrgPoint.y;
	}

	// 원점에 스케일과 org적용
	double dX, dY;
	dX = dY = 0.0;

	dX = dX * m_dZoom;
	dY = dY * m_dZoom;

	if(m_bIsBig)
	{
		dX = dX + m_pOrgPoint.x;
		dY = dY + m_pOrgPoint.y;
	}

	// 외곽선 그리기
	if(pDoc->m_structDataEditor.m_nNzlMode == 0)
		pDC->Rectangle((long)dL-2, (long)dT-2, (long)dR+2, (long)dB+2);
	else
		pDC->Rectangle((long)(dL-2)*(-1), (long)(dT-2)*(-1), (long)(dR+2)*(-1), (long)(dB+2)*(-1));

	// 외곽선 펜 제거
	pDC->SelectObject(oldPen);
	newPen.DeleteObject();
//
	// 십자가 펜 생성
	if(m_bIsBig)
		newPen.CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
	else
		newPen.CreatePen(PS_DOT, 1, RGB(200, 200, 200));

	oldPen = pDC->SelectObject(&newPen);

	// 십자가 그리기
	if(pDoc->m_structDataEditor.m_nNzlMode == 0)
	{
		pDC->MoveTo((long)dX, (long)dY);
		pDC->LineTo((long)dL, (long)dY);
		pDC->MoveTo((long)dX, (long)dY);
		pDC->LineTo((long)dR, (long)dY);
		pDC->MoveTo((long)dX, (long)dY);
		pDC->LineTo((long)dX, (long)dB);
		pDC->MoveTo((long)dX, (long)dY);
		pDC->LineTo((long)dX, (long)dT);
	}
	else
	{
		pDC->MoveTo((long)dX*(-1), (long)dY*(-1));
		pDC->LineTo((long)dL*(-1), (long)dY*(-1));
		pDC->MoveTo((long)dX*(-1), (long)dY*(-1));
		pDC->LineTo((long)dR*(-1), (long)dY*(-1));
		pDC->MoveTo((long)dX*(-1), (long)dY*(-1));
		pDC->LineTo((long)dX*(-1), (long)dB*(-1));
		pDC->MoveTo((long)dX*(-1), (long)dY*(-1));
		pDC->LineTo((long)dX*(-1), (long)dT*(-1));
	}

	// 해제 
	pDC->SelectObject(oldPen);
	newPen.DeleteObject();
	pDC->SelectObject(oldBrush);
	newBrush.DeleteObject();
//
	// column 그리기..
	//
	double dC1 = 0.0 , dC2 = 0.0 ;
//	dL = pDoc->m_structDataEditor.m_dGlassSizeX / -2.;
//	dC1 = pDoc->m_structPatternData[0].m_dColumnOffset[0] * m_dZoom; // k1//2013.09.04 by tskim
//	dC2 = pDoc->m_structPatternData[0].m_dColumnOffset[1] * m_dZoom; // k2
//
	if(m_bIsBig)
	{
//		dC1 = dC1 + m_pOrgPoint.y;
//		dC2 = dC2 + m_pOrgPoint.y;
	}
	//
	if(m_bIsBig)
		newPen.CreatePen(PS_SOLID, 1, GREEN);
	else
		newPen.CreatePen(PS_DOT, 1, GREEN);

	oldPen = pDC->SelectObject(&newPen);

	// column 그리기
//	pDC->MoveTo((long)dL, (long)dC1);
//	pDC->LineTo((long)dR, (long)dC1);
//	pDC->MoveTo((long)dL, (long)dC2);
//	pDC->LineTo((long)dR, (long)dC2);

	// 해제 
	pDC->SelectObject(oldPen);
	newPen.DeleteObject();
	pDC->SelectObject(oldBrush);
	newBrush.DeleteObject();

}


// small pattern wnd에 나오는 큰창의 상을 그린다.
// 이 함수는 small pattern wnd일 경우만 호출 되는 함수이다.
void CPatternWnd::DrawForeground(CDC *pDC)
{
	CP8CA_LcDispDoc *pDoc = GetDocument();


	CBrush newBrush, *oldBrush;
	CPen newPen, *oldPen;

	// 객체 생성
 	newBrush.CreateSolidBrush(m_colorForeground);
//	newBrush.CreateStockObject(NULL_BRUSH);
	newPen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));

	// 객체 선택
	oldBrush = pDC->SelectObject(&newBrush);
	oldPen = pDC->SelectObject(&newPen);


	// 좌표 적용 ////
	// 실제 좌표
	double dL, dR, dT, dB;
	
	dL = m_dBigClientX / -2;
	dR = m_dBigClientX / 2;
	dT = m_dBigClientY / 2;
	dB = m_dBigClientY / -2;

	// zoom 적용
	dL = dL / m_dAboutBigZoom;
	dR = dR / m_dAboutBigZoom;
	dT = dT / m_dAboutBigZoom;
	dB = dB / m_dAboutBigZoom;

	dL = dL * (m_dBigDefaultZoom / m_dBigZoom);
	dR = dR * (m_dBigDefaultZoom / m_dBigZoom);
	dT = dT * (m_dBigDefaultZoom / m_dBigZoom);
	dB = dB * (m_dBigDefaultZoom / m_dBigZoom);

	// org 적용
	dL = dL - m_pOrgPoint.x;
	dR = dR - m_pOrgPoint.x;
	dT = dT - m_pOrgPoint.y;
	dB = dB - m_pOrgPoint.y;

	// 그리기
	pDC->Rectangle((long)dL*(1), (long)dT*(1), (long)dR*(1), (long)dB*(1));

	// 객체 제거
	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
	newPen.DeleteObject();
	newBrush.DeleteObject();
}

void CPatternWnd::DrawPanel(CDC *pDC)
{
/*	if(m_nDispPanel == 0) return;
	CP8CA_DataEditorDoc *pDoc = GetDocument();
	CBrush newBrush, *oldBrush;
	CPen newPen, *oldPen;
	CString str;

	newBrush.CreateSolidBrush(YELLOW);
	newPen.CreatePen(PS_SOLID, 1, YELLOW);
	
	// 객체 선택
	oldBrush = pDC->SelectObject(&newBrush);
	oldPen = pDC->SelectObject(&newPen);

	double dStart_X[MAX_PANEL] = {0.0,}, dStart_Y[MAX_PANEL] = {0.0,}, dEnd_X[MAX_PANEL] = {0.0,}, dEnd_Y[MAX_PANEL] = {0.0,};
	int i, j, nMax_Cell = 0, nHead[MAX_PANEL][2] = {0,}, nTemp;
	BOOL bTemp;
	double dX, dY;
	CRect rect;

	for(i = 0; i < pDoc->m_structDataEditor.m_nPatternNum; i++)
	{
		for(j = 0; j < MAX_NOZZLE; j++)
		{
			if(pDoc->m_structPatternData[i].m_nPanelNo[j] > nMax_Cell) nMax_Cell = pDoc->m_structPatternData[i].m_nPanelNo[j];
		}
	}

	for(i = 0; i < nMax_Cell; i++)
	{
		dStart_X[i] = pDoc->m_structDataEditor.m_dGlassSizeX / 2;
		dEnd_X[i] = (-1) * pDoc->m_structDataEditor.m_dGlassSizeX / 2;
	}
	for(; i < MAX_PANEL; i++)
	{
		dStart_X[i] = 0.0;
		dEnd_X[i] = 0.0;
	}
	for(i = 0; i < pDoc->m_structDataEditor.m_nPatternNum; i++)
	{
		for(nTemp = 0; nTemp < nMax_Cell; nTemp++)
		{
			bTemp = FALSE;
			for(j = 0; j < MAX_NOZZLE; j++)
			{
				if(pDoc->m_structPatternData[i].m_bIsOnHead[j] == FALSE) continue;
				if(pDoc->m_structPatternData[i].m_nPanelNo[j] == nTemp+1)
				{
					if(bTemp == FALSE)
					{
						bTemp = TRUE;
						nHead[nTemp][0] = nHead[nTemp][1] = j;
					}
					if(nHead[nTemp][1] < j) nHead[nTemp][1] = j;
				}
			}
			if(bTemp)
			{
				if(pDoc->m_structPatternData[i].m_dCenterPositionX + pDoc->m_structPatternData[i].m_dNozzleOffset[nHead[nTemp][0]] - pDoc->m_structPatternData[i].m_dWidthX / 2 < dStart_X[nTemp]) dStart_X[nTemp] = pDoc->m_structPatternData[i].m_dCenterPositionX + pDoc->m_structPatternData[i].m_dNozzleOffset[nHead[nTemp][0]] - pDoc->m_structPatternData[i].m_dWidthX / 2;
				dStart_Y[nTemp] = pDoc->m_structPatternData[i].m_dCenterPositionY + pDoc->m_structPatternData[i].m_dWidthY / 2;
				if(pDoc->m_structPatternData[i].m_dCenterPositionX + pDoc->m_structPatternData[i].m_dNozzleOffset[nHead[nTemp][1]] + pDoc->m_structPatternData[i].m_dWidthX / 2 > dEnd_X[nTemp]) dEnd_X[nTemp] = pDoc->m_structPatternData[i].m_dCenterPositionX + pDoc->m_structPatternData[i].m_dNozzleOffset[nHead[nTemp][1]] + pDoc->m_structPatternData[i].m_dWidthX / 2;
				dEnd_Y[nTemp] = pDoc->m_structPatternData[i].m_dCenterPositionY - pDoc->m_structPatternData[i].m_dWidthY / 2;
			}
		}
	}

	for(i = 0; i < nMax_Cell; i++)
	{
		dX = dStart_X[i] + (dEnd_X[i] - dStart_X[i]) / 2;
		dY = dStart_Y[i] - (dStart_Y[i] - dEnd_Y[i]) / 2;
		
		// zoom 적용
		dX = dX / m_dAboutBigZoom;
		dY = dY / m_dAboutBigZoom;
		
 		dX = dX * (m_dBigDefaultZoom);
 		dY = dY * (m_dBigDefaultZoom);
		
		// 그리기
		rect.left = (long)dX-8;
		rect.right = (long)dX+8;
		rect.bottom = (long)dY-5;
		rect.top = (long)dY+5;
//		pDC->Rectangle((long)dX-7, (long)dY-5, (long)dX+7, (long)dY+5);

		CPoint point;
		point.x = 5; point.y = 5;
		pDC->RoundRect(rect, point);
//		pDC->Ellipse(rect);
 		
		if(m_nDispPanel == 1) str.Format("%d", i+1);
		else str.Format("%s", pDoc->m_strGlassIden[i]);
		
		pDC->TextOut((str.GetLength() == 2 ? dX-6 : dX-4), dY+5, str);
	}
	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
	newPen.DeleteObject();
	newBrush.DeleteObject();*/ //2013.09.04 by tskim
}

// 타이머 에서 dispense order를 그림
void CPatternWnd::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CP8CA_LcDispDoc *pDoc = GetDocument();

	if(nIDEvent == 0)
	{
//		TRACE("m_nSeq[%d]:%d\n",m_nCurrentDraw,m_nSeq[m_nCurrentDraw]);
		//
		if(m_nCurrentDraw == -1)
		{
			// erase previous back-ground and redraw..
			m_nCurrentDraw ++; // -1++ -> 0
			Invalidate();
		}			
		else
		{
/*			//
			for(int i = 0; i < pDoc->m_structDataEditor.m_nPatternNum; i++)
			{
				if(m_nSeq[i][m_nCurrentDraw] != 0) break;//flexible... Yamary...
			}
			if(i == pDoc->m_structDataEditor.m_nPatternNum) //flexible... Yamary...
			{ 
				KillTimer(0);
				m_nCurrentDraw = -1;
	//			DrawBackground(&dc);
			}
			else
			{
				m_nCurrentDraw ++;
				Invalidate();
			}
*/
			if(m_nSeq[m_nCurrentDraw] == 0)
			{ 
				KillTimer(0);
				m_nCurrentDraw = -1;
				//m_bIsSimulation=FALSE;
	//			DrawBackground(&dc);
			}
			else
			{
				m_nCurrentDraw ++;
				Invalidate();
			}
		}
	}

//	if(nIDEvent == 0)
//	{
//		m_nCurrentDraw ++;
//		if(m_nCurrentDraw == pDoc->m_structDataEditor.m_nPatternNum) 
//		{
//			m_nCurrentDraw = -1;
//		}
//		Invalidate();
//	}
	
	CWnd::OnTimer(nIDEvent);
}

void CPatternWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	return;
	CP8CA_LcDispDoc *pDoc = GetDocument();

	if(pDoc->m_structDataEditor.m_nNzlMode == 0)
		return;
	int nPat = 0;
	BOOL bCheck = FALSE;
	if(!m_bIsDispenseOrder)
	if(pDoc->m_structDataEditor.m_nNzlMode == 1 /*&& g_bOnly_Center == FALSE */&& g_bOnly_Nozzle == FALSE && m_bIsBig && m_dZoom >= 1.0)
	{
		CDC *pDC;
		pDC = GetDC();
		
		// 좌표를 현재 dc에 맞게 적용
		ApplyDC(pDC);
		pDC->DPtoLP(&point);

		double dX, dY;
		dX = (double)point.x;
		dY = (double)point.y;

		dX = dX * (-1);
		dY = dY * (-1);

		// org 와 zoom 적용 ////////////////
		dX = dX - m_pOrgPoint.x;
		dY = dY - m_pOrgPoint.y;
		dX = dX / m_dZoom;
		dY = dY / m_dZoom;

		for(int i = 0; i < pDoc->m_structDataEditor.m_nPatternNum; i++)
		{
			if(pDoc->m_structPatternData[i].m_dCenterPositionY + pDoc->m_structPatternData[i].m_dWidthY/2 >= dY &&
				pDoc->m_structPatternData[i].m_dCenterPositionY - pDoc->m_structPatternData[i].m_dWidthY/2 <= dY)
			{
				nPat = i+1;
				break;
			}
		}
		////////////////////////////////////

		// 실제로 적용된 좌표를 drawpattern을 통해서 선택되었는지 판단
		CDrawPattern drawPattern;
		drawPattern.m_dZoom = m_dZoom;
		drawPattern.m_pOrgPoint = m_pOrgPoint;
		drawPattern.SetPattern(pDC, (m_nPatternNum >= 0 ? m_nPatternNum : nPat));

		if(pDoc->m_structDataEditor.m_nNzlMode == 1) bCheck = drawPattern.CheckSelect_NType(dX, dY);
		else drawPattern.CheckSelect_2Nozzle(dX, dY);
		if(bCheck && g_bMouse_Down)
		{
			g_b1Drop_Disp = TRUE;
			Invalidate();
		}
		else g_b1Drop_Disp = FALSE;
		if(m_bCheck && !bCheck) Invalidate();
		m_bCheck = bCheck;
		if(m_nCurSelDropNum != drawPattern.m_nCurSelDropNum || m_nCurSelHeadNum != drawPattern.m_nCurSelHeadNum)
		{
			m_nCurSelDropNum = drawPattern.m_nCurSelDropNum;
			m_nCurSelHeadNum = drawPattern.m_nCurSelHeadNum;
			Invalidate();
		}
		else if(pDoc->m_structDataEditor.m_nNzlMode ==1)
		{
			if(m_nCurSelPointNum != drawPattern.m_nCurSelPointNum)
			{
				m_nCurSelPointNum = drawPattern.m_nCurSelPointNum;
				Invalidate();
			}
		}
		// dc 해제
		ReleaseDC(pDC);
	
//			if(m_bIsDirectSetting) GetParent()->SendMessage(WM_LBUTTONDBLCLK);
	}
	CWnd::OnMouseMove(nFlags, point);
}

void CPatternWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(GetDocument()->m_structDataEditor.m_nNzlMode != 1) return;
	g_bMouse_Down = TRUE;
	OnMouseMove(nFlags, point);
 	CWnd::OnLButtonDown(nFlags, point);
}

void CPatternWnd::OnGetRect(CRect rect)
{
	m_BigRect = rect;
}

void CPatternWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if(m_bIsBig) return;
	if(m_nDispPanel	== 0) m_nDispPanel = 1;
	else if(m_nDispPanel == 1) m_nDispPanel = 2;
	else m_nDispPanel = 0;
// 	CEditPatIndic *m_pEditPatIndic = (CEditPatIndic *)GetParent();
// 	
// 	m_pEditPatIndic->m_pSmallPatternWnd->Invalidate();
	Invalidate();

	CWnd::OnRButtonUp(nFlags, point);
}

void CPatternWnd::PartConvDropPoint(int Max, int Min, int i)
{
	CP8CA_LcDispDoc *pDoc = GetDocument();
	DROP_DATA stDrop;
//2010.06.21 by tskim Repair 
	DROP_DATA stDropBK;
	int n=0, k=0;

//2010.06.21 by tskim Repair
	for(n = Min-1; n < Max; n++)
	{
		//stDropBK = pDoc->m_arDropDataBk[m_nPatternNum-1].GetAt(n);
		//pDoc->m_arDropDataBk[m_nPatternNum-1].SetAt(n, stDropBK);
		//pDoc->m_arDropDataBk[m_nPatternNum-1].Add(stDropBK);
	}

	if(pDoc->m_structDataEditor.m_nNzlMode != 1)
	{
		if(i == 0)					// Reset
		{	
			for(n = Min-1; n < Max; n++)
			{
				stDrop = pDoc->m_arDropData[m_nPatternNum-1].GetAt(n);
				for(k=0; k<MAX_NOZZLE; k++)
				{
					if(pDoc->m_structPatternData[m_nPatternNum-1].m_bIsOnHead[k])
					{
						stDrop.m_nZ[k] = FALSE;
					}
				}
				pDoc->m_arDropData[m_nPatternNum-1].SetAt(n, stDrop);
			}
		}
		else if(i == 1)				// Set
		{
			for(n = Min-1; n < Max; n++)
			{
				stDrop = pDoc->m_arDropData[m_nPatternNum-1].GetAt(n);
				stDropBK = pDoc->m_arDropDataBk[m_nPatternNum-1].GetAt(n);
				for(int k=0; k<MAX_NOZZLE; k++)
				{
					if(pDoc->m_structPatternData[m_nPatternNum-1].m_bIsOnHead[k])
					{
						if(stDropBK.m_nZ[k])
							stDrop.m_nZ[k] = TRUE;
						else
							stDrop.m_nZ[k] = FALSE;
					}
				}
				pDoc->m_arDropData[m_nPatternNum-1].SetAt(n, stDrop);
			}
		}
		else if(i == 2)				// 부분 반전 영역 설정시..
		{
			for(n = Min-1; n < Max; n++)
			{
				stDrop = pDoc->m_arDropData[m_nPatternNum-1].GetAt(n);
				for(k=0; k<MAX_NOZZLE; k++)
				{
					if(pDoc->m_structPatternData[m_nPatternNum-1].m_bIsOnHead[k])
					{
						if(stDrop.m_nZ[k]) stDrop.m_nZ[k] = FALSE;
						else	stDrop.m_nZ[k] = TRUE;
					}
				}
				pDoc->m_arDropData[m_nPatternNum-1].SetAt(n, stDrop);
			}
		}
		else if(i == 3)
		{
			// Pattern 의 시작점과 마지막 점으로 Drop Point 설정
			
			for(k=0; k<MAX_NOZZLE; k++)
			{
				if(pDoc->m_structPatternData[m_nPatternNum-1].m_bIsOnHead[k])
				{
					for(n = 0; n < pDoc->m_arDropData[m_nPatternNum-1].GetSize(); n++)
					{
						stDrop = pDoc->m_arDropData[m_nPatternNum-1].GetAt(n);
						if( (n < (Min-1)) || (n >= Max) ){
							stDrop.m_nZ[k] = FALSE;
						}
						pDoc->m_arDropData[m_nPatternNum-1].SetAt(n, stDrop);
					}
				}
			}
		}
	}
	else
	{
		if(i == 0)					// Reset
		{	
			for(n = Min-1; n < Max; n++)
			{
				stDrop = pDoc->m_arDropData[m_nPatternNum-1].GetAt(n);
				for(k=0; k<MAX_NOZZLE; k++)
				{
					if(pDoc->m_structPatternData[m_nPatternNum-1].m_bIsOnHead[k])
					{
						stDrop.m_nZ[k] = FALSE;
					}
				}
				pDoc->m_arDropData[m_nPatternNum-1].SetAt(n, stDrop);
			}
		}
		else if(i == 1)				// Set
		{
			for(n = Min-1; n < Max; n++)
			{
				stDrop = pDoc->m_arDropData[m_nPatternNum-1].GetAt(n);
				stDropBK = pDoc->m_arDropDataBk[m_nPatternNum-1].GetAt(n);
				for(int k=0; k<MAX_NOZZLE; k++)
				{
					if(pDoc->m_structPatternData[m_nPatternNum-1].m_bIsOnHead[k])
					{
						if(stDropBK.m_nZ[k])
							stDrop.m_nZ[k] = TRUE;
						else
							stDrop.m_nZ[k] = FALSE;
					}
				}
				pDoc->m_arDropData[m_nPatternNum-1].SetAt(n, stDrop);
			}
		}
		else if(i == 2)				// 부분 반전 영역 설정시..
		{
			for(n = Min-1; n < Max; n++)
			{
				stDrop = pDoc->m_arDropData[m_nPatternNum-1].GetAt(n);
				for(k=0; k<MAX_NOZZLE; k++)
				{
					if(pDoc->m_structPatternData[m_nPatternNum-1].m_bIsOnHead[k])
					{
						if(stDrop.m_nZ[k]) stDrop.m_nZ[k] = FALSE;
						else	stDrop.m_nZ[k] = TRUE;
					}
				}
				pDoc->m_arDropData[m_nPatternNum-1].SetAt(n, stDrop);
			}
		}
		else if(i == 3)
		{
			// Pattern 의 시작점과 마지막 점으로 Drop Point 설정
			
			for(k=0; k<MAX_NOZZLE; k++)
			{
				if(pDoc->m_structPatternData[m_nPatternNum-1].m_bIsOnHead[k])
				{
					for(n = 0; n < pDoc->m_arDropData[m_nPatternNum-1].GetSize(); n++)
					{
						stDrop = pDoc->m_arDropData[m_nPatternNum-1].GetAt(n);
						if( (n < (Min-1)) || (n >= Max) ){
							stDrop.m_nZ[k] = FALSE;
						}
						pDoc->m_arDropData[m_nPatternNum-1].SetAt(n, stDrop);
					}
				}
			}
		}		
	}
}

// DropPoint.cpp: implementation of the CDropPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"

#include "DropPoint.h"
#include "math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
extern BOOL nX_Display;
extern BOOL nY_Display;extern BOOL g_bOnly_Center;
extern BOOL g_bOnly_Nozzle;
extern BOOL g_bOnly_1_Point;
extern int g_nNTypeInfo[3];
extern BOOL g_b1Drop_Disp;
//#define DISTANCE_2NOZZLE 9.25  //9.25 + 9.25 = 18.5
#define DISTANCE_2NOZZLE 10.0  //10.0 + 10.0 = 20.0
#define DISTANCE_2NOZZLE_2 5.0  //5.0 + 5.0 = 10.0

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDropPoint::CDropPoint()
{
	// 기본값들
	m_bOn = TRUE;
	m_bIsDisplayText = TRUE;
	m_bIsDisplayText2 = FALSE;
	m_dPointX = m_dPointY = 0.0;
	m_dRadius = 4;
	m_nAddDelSize = 5;
	m_dZoom = 1;
	m_nNumber = 1;
	m_pOrgPoint = CPoint(0, 0);
	m_bIsAdd = FALSE;
	m_bIsDel = FALSE;
	m_nLine = 1; //Yamary...flexible
	m_nPattern = 0; //Yamary...flexible
	m_d2NozzleX[0] = m_d2NozzleX[1] = 0.0;
	m_b2NozzleDiplay = FALSE;
	m_nCount = 0;
	m_bOnly = FALSE;
	m_nNLine = 0;
}

CDropPoint::~CDropPoint()
{

}

// drop을 그리는 곳
void CDropPoint::Draw(CDC *pDC)
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CString str,str1,str2,str3;
	int temp = 0;
	CRect rect;
	pDoc->m_structDataEditor.m_nNzlMode == 1 ? m_dRadius = 2 : m_dRadius = 4;
	if(pDoc->m_structPatternData[m_nPattern].m_nDispenseMode == 2)
	{
		str.Format("%d/%d",m_nLine,m_nNumber);//Yamary...flexible
		temp = str.FindOneOf("/");
	}
	else str.Format("%d", m_nNumber);

		// 좌표 적용
	rect.left = (long)((m_dPointX) * m_dZoom) + m_pOrgPoint.x + (pDoc->m_structDataEditor.m_nNzlMode == 1 ? -2 : 0);
	rect.top = (long)((m_dPointY) * m_dZoom) + m_pOrgPoint.y + (pDoc->m_structDataEditor.m_nNzlMode == 1 ? -2 : 0);
	rect.right = (long)((m_dPointX) * m_dZoom) + m_pOrgPoint.x + (pDoc->m_structDataEditor.m_nNzlMode == 1 ? +2 : 0);
	rect.bottom = (long)((m_dPointY) * m_dZoom) + m_pOrgPoint.y + (pDoc->m_structDataEditor.m_nNzlMode == 1 ? +2 : 0);
	// 그린다.
	// 일반 형태
	if(!m_bIsAdd && !m_bIsDel) 
	{
		// 반지름적용 
		rect.left -= (long)m_dRadius;
		rect.top -= (long)m_dRadius;
		rect.right += (long)m_dRadius;
		rect.bottom += (long)m_dRadius;

		//add/del
		str3=" ";

		pDC->Ellipse(rect);
	} 
	
	// add 형태
	else if(m_bIsAdd && !m_bIsDel) 
	{
		pDC->MoveTo(rect.left, rect.top + m_nAddDelSize);
		pDC->LineTo(rect.left + m_nAddDelSize, rect.top - m_nAddDelSize);
		pDC->LineTo(rect.left - m_nAddDelSize, rect.top - m_nAddDelSize);
		pDC->LineTo(rect.left, rect.top + m_nAddDelSize);
		str3.Format("%d",m_nAddDelNum);
		
	}

	// del 형태
	else if(m_bIsDel && !m_bIsAdd) 
	{
		pDC->MoveTo(rect.left, rect.top - m_nAddDelSize);
		pDC->LineTo(rect.left + m_nAddDelSize, rect.top + m_nAddDelSize);
		pDC->LineTo(rect.left - m_nAddDelSize, rect.top + m_nAddDelSize);
		pDC->LineTo(rect.left, rect.top - m_nAddDelSize);
		str3.Format("%d",m_nAddDelNum);
	}

	pDC->SetTextAlign(TA_LEFT | TA_TOP);

	
	
	if(m_bIsDisplayText) 
	{
		if(!g_bOnly_1_Point || pDoc->m_structDataEditor.m_nNzlMode == 0)
		{
			pDC->TextOut(rect.right-10, rect.bottom-(long)m_dRadius*2, pDoc->m_structPatternData[m_nPattern].m_nDispenseMode != 2 ? str : str.Mid(temp+1, str.GetLength() - temp - 1));
			if(pDoc->m_structPatternData[m_nPattern].m_nDispenseMode == 2)
				pDC->TextOut(rect.left-10, rect.bottom+(long)m_dRadius, str.Mid(0,temp));
	//		pDC->TextOut(rect.right+2, rect.bottom-15-(long)m_dRadius*2,str3);
			pDC->TextOut(rect.right-15, rect.bottom+15-(long)m_dRadius*2,str3);
		}
		if(m_bIsDisplayText2) 
		{
			if(g_bOnly_1_Point)
			{
				if(m_bOnly) str1.Format("X:%.2f",m_dPointX+(pDoc->m_structDataEditor.m_dGlassSizeX/2));
				else str1.Empty();
			}
			else if(nX_Display)
				str1.Format("X:%.2f",m_dPointX+(pDoc->m_structDataEditor.m_dGlassSizeX/2));
			else if(nY_Display)
				str1.Format("Y:%.2f",m_dPointY+(pDoc->m_structDataEditor.m_dGlassSizeY/2));
			pDC->TextOut(rect.right-40, rect.bottom+20-(long)m_dRadius*2, str1);
		}
	}
}

// drop point가 선택되었는지 여부 
// 만약에 선택되면 On, Off를 flag 시킨다.
BOOL CDropPoint::CheckSelect(double dX, double dY)
{
	double dDistX;		// 중심점과 현재 커서와의 거리(x)
	double dDistY;		// 중심점과 현재 커서와의 거리(y)
	BOOL bResult;		// 선택 여부
	
	// 거리체크
	if(dX > m_dPointX) dDistX = dX - m_dPointX;
	else dDistX = m_dPointX - dX;

	if(dY > m_dPointY) dDistY = dY - m_dPointY;
	else dDistY = m_dPointY - dY;

	// 선택여부 체크
	if((dDistX <= (m_dRadius / m_dZoom)) && (dDistY <= (m_dRadius / m_dZoom))) bResult = TRUE;
	else bResult = FALSE;

	// 선택 되었을 경우의 처리
	// On, off를 flag하고
	if(bResult) m_bOn = m_bOn ^ TRUE;

	// 결과 반환
	return bResult;
}

// 좌표를 설정한다.
void CDropPoint::SetPoint(double dX, double dY)
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();


//	m_dPointX = dX;
//	m_dPointY = dY;
	m_dPointX = dX*(-1.0);
	m_dPointY = dY*(-1.0);


	if(pDoc->m_structDataEditor.m_nNzlMode == 2)
	{
		if(pDoc->m_structDataEditor.m_nNzlMode == 2)
		{
			m_d2NozzleX[0] = m_dPointX-DISTANCE_2NOZZLE;
			m_d2NozzleX[1] = m_dPointX+DISTANCE_2NOZZLE;
		}
		else
		{
//			m_d2NozzleX[0] = m_dPointX-DISTANCE_2NOZZLE_2;
//			m_d2NozzleX[1] = m_dPointX+DISTANCE_2NOZZLE_2;
		}
	}

//	if(fabs(dX) > pView->GetDocument()->m_structDataEditor.m_dGlassSizeX*(0.5) || fabs(dY) > pView->GetDocument()->m_structDataEditor.m_dGlassSizeY*(0.5) )
//	{
//		pView->m_ctrlMessage.SetCaption("Glass 영역을 넘어섰읍니다");
//		pView->m_nLimitOverCount++;
//	}
}

void CDropPoint::Draw_2Nozzle(CDC *pDC)
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CString str,str1,str2;
	int temp = 0;
	CRect rect1, rect2;
	m_b2NozzleDiplay ? m_dRadius = 6 : m_dRadius = 2;
		// 좌표 적용
	if(pDoc->m_structPatternData[m_nPattern].m_nDispenseMode == 2)
	{
		str.Format("%d/%d",m_nLine,m_nNumber);//Yamary...flexible
		temp = str.FindOneOf("/");
	}
	else str.Format("%d", m_nNumber);
	rect1.left = (long)((m_d2NozzleX[0]) * m_dZoom) * (1)+ m_pOrgPoint.x;
	rect1.top = (long)((m_dPointY) * m_dZoom) * (1) + m_pOrgPoint.y;
	rect1.right = (long)((m_d2NozzleX[0]) * m_dZoom) * (1) + m_pOrgPoint.x;
	rect1.bottom = (long)((m_dPointY) * m_dZoom) * (1) + m_pOrgPoint.y;


	rect2.left = (long)((m_d2NozzleX[1]) * m_dZoom) * (1) + m_pOrgPoint.x;
	rect2.top = (long)((m_dPointY) * m_dZoom) * (1) + m_pOrgPoint.y;
	rect2.right = (long)((m_d2NozzleX[1]) * m_dZoom) * (1) + m_pOrgPoint.x;
	rect2.bottom = (long)((m_dPointY) * m_dZoom) * (1) + m_pOrgPoint.y;

	// 그린다.
	// 일반 형태
	if(!m_bIsAdd && !m_bIsDel) 
	{

		rect1.left -= (long)m_dRadius;
		rect1.top -= (long)m_dRadius;
		rect1.right += (long)m_dRadius;
		rect1.bottom += (long)m_dRadius;

		rect2.left -= (long)m_dRadius;
		rect2.top -= (long)m_dRadius;
		rect2.right += (long)m_dRadius;
		rect2.bottom += (long)m_dRadius;
		pDC->Ellipse(rect1);
		pDC->Ellipse(rect2);
	} 
//	
//	// add 형태
//	else if(m_bIsAdd && !m_bIsDel) 
//	{
//		pDC->MoveTo(rect.left, rect.top + m_nAddDelSize);
//		pDC->LineTo(rect.left + m_nAddDelSize, rect.top - m_nAddDelSize);
//		pDC->LineTo(rect.left - m_nAddDelSize, rect.top - m_nAddDelSize);
//		pDC->LineTo(rect.left, rect.top + m_nAddDelSize);
//		str3.Format("%d",m_nAddDelNum);
//		
//	}
//
//	// del 형태
//	else if(m_bIsDel && !m_bIsAdd) 
//	{
//		pDC->MoveTo(rect.left, rect.top - m_nAddDelSize);
//		pDC->LineTo(rect.left + m_nAddDelSize, rect.top + m_nAddDelSize);
//		pDC->LineTo(rect.left - m_nAddDelSize, rect.top + m_nAddDelSize);
//		pDC->LineTo(rect.left, rect.top - m_nAddDelSize);
//		str3.Format("%d",m_nAddDelNum);
//	}
	pDC->SetTextAlign(TA_LEFT | TA_TOP);
	
	if(m_bIsDisplayText && !g_bOnly_Center && g_bOnly_Nozzle) 
	{
		pDC->TextOut(rect1.right-10, rect1.bottom-(long)m_dRadius*2, pDoc->m_structPatternData[m_nPattern].m_nDispenseMode != 2 ? str : str.Mid(temp+1, str.GetLength() - temp - 1));
//		if(pDoc->m_structPatternData[m_nPattern].m_nDispenseMode == 2)
//			pDC->TextOut(rect1.left-10, rect1.bottom+(long)m_dRadius, str.Mid(0,temp));
//		pDC->TextOut(rect.right+2, rect.bottom-15-(long)m_dRadius*2,str3);
//		pDC->TextOut(rect1.right-15, rect1.bottom+15-(long)m_dRadius*2,str3);
		if(m_bIsDisplayText2) 
		{
			if(nX_Display)
				str1.Format("X:%.2f",m_d2NozzleX[0]+(pDoc->m_structDataEditor.m_dGlassSizeX/2));
			else if(nY_Display)
				str1.Format("Y:%.2f",m_dPointY+(pDoc->m_structDataEditor.m_dGlassSizeY/2));
			pDC->TextOut(rect1.right-20, rect1.bottom+(m_nCount % 2 == 0 ? 20 : -10)-(long)m_dRadius*2, str1);
		}

		pDC->TextOut(rect2.right-10, rect2.bottom-(long)m_dRadius*2, pDoc->m_structPatternData[m_nPattern].m_nDispenseMode != 2 ? str : str.Mid(temp+1, str.GetLength() - temp - 1));
//		if(pDoc->m_structPatternData[m_nPattern].m_nDispenseMode == 2)
//			pDC->TextOut(rect2.left-10, rect2.bottom+(long)m_dRadius, str.Mid(0,temp));
//		pDC->TextOut(rect.right+2, rect.bottom-15-(long)m_dRadius*2,str3);
//		pDC->TextOut(rect2.right-15, rect2.bottom+15-(long)m_dRadius*2,str3);
		if(m_bIsDisplayText2) 
		{
			if(nX_Display)
				str1.Format("X:%.2f",m_d2NozzleX[1]+(pDoc->m_structDataEditor.m_dGlassSizeX/2));
			else if(nY_Display)
				str1.Format("Y:%.2f",m_dPointY+(pDoc->m_structDataEditor.m_dGlassSizeY/2));
			pDC->TextOut(rect2.right-20, rect2.bottom+(m_nCount % 2 == 0 ? 20 : -10)-(long)m_dRadius*2, str1);
		}
	}

}

void CDropPoint::Draw_N_Line(CDC *pDC, double dx, double dy)
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

//	dx = dx*(-1.0);
//	dy = dy*(-1.0);
	
	double dLength = (N_MAX_NOZZLE - 1) * N_NOZZLE_PITCH;

//	m_pOrgPoint.x = m_pOrgPoint.x * (-1);
//	m_pOrgPoint.y = m_pOrgPoint.y * (-1);
	
	pDC->MoveTo((int)((((dx - dLength/2) * m_dZoom)*(-1) + m_pOrgPoint.x))*(1), (int)(((dy * m_dZoom)*(-1) + m_pOrgPoint.y))*(1));
	pDC->LineTo((int)((((dx + dLength/2) * m_dZoom)*(-1) + m_pOrgPoint.x))*(1), (int)(((dy * m_dZoom)*(-1) + m_pOrgPoint.y))*(1));
}

void CDropPoint::Draw_N_Drop(CDC *pDC, double dx, double dy)
{
	if(g_b1Drop_Disp && m_bOnly == FALSE) return;
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

//	dx = dx*(-1.0);
//	dy = dy*(-1.0);

//	m_pOrgPoint.x = m_pOrgPoint.x * (-1);
//	m_pOrgPoint.y = m_pOrgPoint.y * (-1);
	
	CRect rect;
	CString str;
	double dLength = (N_MAX_NOZZLE - 1) * N_NOZZLE_PITCH;
	double dStart = dx - dLength / 2;
	for(int i = 0; i < N_MAX_NOZZLE; i++)
	{
		rect.left = (long)(((dStart + N_NOZZLE_PITCH*i) * m_dZoom)*(-1) + (m_pOrgPoint.x))*(1) + (m_bOnly ? -2 : 0);
		rect.top = (long)(((dy) * m_dZoom)*(-1) + (m_pOrgPoint.y))*(1) + (m_bOnly ? -2 : 0);
		rect.right = (long)(((dStart + N_NOZZLE_PITCH*i) * m_dZoom)*(-1) + (m_pOrgPoint.x))*(1) + (m_bOnly ? 2 : 0);
		rect.bottom = (long)(((dy) * m_dZoom)*(-1) + (m_pOrgPoint.y))*(1) + (m_bOnly ? 2 : 0);

//		rect.left = (long)(((dStart + N_NOZZLE_PITCH*i) * m_dZoom) + m_pOrgPoint.x)*(1) + (m_bOnly ? -2 : 0);
//		rect.top = (long)(((dy) * m_dZoom) + m_pOrgPoint.y)*(1) + (m_bOnly ? -2 : 0);
//		rect.right = (long)(((dStart + N_NOZZLE_PITCH*i) * m_dZoom) + m_pOrgPoint.x)*(1) + (m_bOnly ? 2 : 0);
//		rect.bottom = (long)(((dy) * m_dZoom) + m_pOrgPoint.y)*(1) + (m_bOnly ? 2 : 0);

		// 그린다.
		// 일반 형태
		if(!m_bIsAdd && !m_bIsDel) 
		{
			// 반지름적용
			rect.left -= 2;
			rect.top -= 2;
			rect.right += 2;
			rect.bottom += 2;
			
 			pDC->Ellipse(rect);
		} 
	}
//	g_nNTypeInfo
	if(g_b1Drop_Disp && m_bOnly)
	{
		int nY_Pos[5];
		int nX_Pos;
		for(i = 0; i < 5; i++)
		{
//			if(dy*m_dZoom+m_pOrgPoint.y > 0) nY_Pos[i] = dy*m_dZoom+m_pOrgPoint.y - 20 - 20 * i;
//			else nY_Pos[i] = dy*m_dZoom+m_pOrgPoint.y + 30 + 20 * (4-i);
			if(dy*m_dZoom+m_pOrgPoint.y > 0) nY_Pos[i] = dy*m_dZoom+m_pOrgPoint.y - 10 * i;
			else nY_Pos[i] = dy*m_dZoom+m_pOrgPoint.y + 10 * (4-i);
		}
		dx*m_dZoom+m_pOrgPoint.x > 0 ? nX_Pos = -300 : nX_Pos = 0;
		str.Format("Head : %d , X Line : %d , Y Line : %d", g_nNTypeInfo[1]+1, m_nNLine, g_nNTypeInfo[2]+1);
		pDC->TextOut(rect.left - 50 + nX_Pos, nY_Pos[0], str);
		str.Format("Center Axis ( X , Y ) : %.3f , %.3f", dx, dy);
		pDC->TextOut(rect.left - 50 + nX_Pos, nY_Pos[1], str);
		pDC->TextOut(rect.left - 10 + nX_Pos, nY_Pos[2], "Each X Position");
		str.Format("1 : %.3f , 2 : %.3f , 3 : %.3f , 4 : %.3f",
			dStart, dStart + N_NOZZLE_PITCH, dStart + N_NOZZLE_PITCH*2, dStart + N_NOZZLE_PITCH*3);
		pDC->TextOut(rect.left - 50 + nX_Pos, nY_Pos[3], str);
		str.Format("5 : %.3f , 6 : %.3f , 7 : %.3f , 8 : %.3f",	dStart + N_NOZZLE_PITCH*4, dStart + N_NOZZLE_PITCH*5, dStart + N_NOZZLE_PITCH*6, dStart + N_NOZZLE_PITCH*7);
		pDC->TextOut(rect.left - 50 + nX_Pos, nY_Pos[4], str);
	}
}

void CDropPoint::Draw_N_Center(CDC *pDC, int nCell, double dx)
{
	if(m_bOnly)
	{
		return;
	}
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

//	dx = dx*(-1.0);

//	m_pOrgPoint.x = m_pOrgPoint.x * (-1);
//	m_pOrgPoint.y = m_pOrgPoint.y * (-1);

	pDC->MoveTo((long)((dx * m_dZoom)*(-1) + m_pOrgPoint.x)*(1), (long)(((pDoc->m_structPatternData[m_nPattern].m_dCenterPositionY-pDoc->m_structPatternData[m_nPattern].m_dWidthY/2) * m_dZoom) * (-1) + m_pOrgPoint.y)*(1));
	pDC->LineTo((long)((dx * m_dZoom)*(-1) + m_pOrgPoint.x)*(1), (long)(((pDoc->m_structPatternData[m_nPattern].m_dCenterPositionY+pDoc->m_structPatternData[m_nPattern].m_dWidthY/2) * m_dZoom) * (-1) + m_pOrgPoint.y)*(1));
}
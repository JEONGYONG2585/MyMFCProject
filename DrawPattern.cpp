// DrawPattern.cpp: implementation of the CDrawPattern class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"

#include "DrawPattern.h"
#include "DropPoint.h"
#include "Math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
extern BOOL m_bIndicate;
extern BOOL g_bOnly_Center;
extern BOOL g_bOnly_Nozzle;
extern int g_n2NozzleInfo[2];
extern int g_nNTypeInfo[3];
extern BOOL g_bOnly_1_Point;
extern BOOL nX_Display;
extern BOOL g_bMouse_Down;
extern BOOL g_b1Drop_Disp;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrawPattern::CDrawPattern()
{
	m_dZoom = 1;
	m_pOrgPoint = CPoint(0, 0);
	m_pHomePoint = CPoint(0, 0);

	m_nCurSelDropNum = 0; m_nCurSelHeadNum=0;
	m_bIsDirectSetting = FALSE;
	
	m_colorOff = RGB(155, 155, 155);
	m_Pos = RGB(0,0,0);

	m_bIsDisplayText = TRUE;
	m_bIsDisplayText2 = FALSE;
	m_pOldPen = NULL;
	m_pOldBrush = NULL;
	m_pOldFont = NULL;
	pDC = NULL;

	m_nPatternNumber = 0;

	for(int i = 0; i < MAX_NOZZLE; i++)
		m_dNozzleOffset[i] = 0.0 ;

	m_pDrop = new CDropPoint;
	m_pLine = new CLinePoint;
	
	// 0 -> drop, 1 -> line
	m_nDispenseMode = 0;
	m_nCurSelPointNum = 0;
}

CDrawPattern::~CDrawPattern()
{
	// dc 관련 객체 복원
	if(pDC != NULL)
	{
		pDC->SelectObject(m_pOldPen);
		pDC->SelectObject(m_pOldBrush);
		pDC->SelectObject(m_pOldFont);

		for(int i = 0; i < MAX_NOZZLE; i++)
		{
			m_OnPen[i].DeleteObject();
			m_LinePen[i].DeleteObject();
			m_OnBrush[i].DeleteObject();
		}
		m_Center.DeleteObject();
		m_OffPen.DeleteObject();
		m_OffBrush.DeleteObject();
		m_newFont.DeleteObject();
		m_newLogFont.DeleteObject();
		m_PosPen.DeleteObject();
		m_WhiteBrush.DeleteObject();
	}

	delete m_pDrop;
	delete m_pLine;
}

// drop들을 그리는 곳
BOOL CDrawPattern::Draw()
{
	CP8CA_LcDispDoc *pDoc = GetDocument();
	DROP_DATA stDrop;				// dropdata 구조체
	LINE_DATA stLine; // Yamary...flexible
	CString str;	
	double dTemp, dNozzle_Length;
	double dNPitch;
	BOOL bPlus;
	int nLine = 0;
	int nTop, nBottom, nRight, nLeft;

	int z4; 

	if(m_bIsBig)
	{
 		nTop = m_BigRect.top - m_BigRect.Height() / 2 - 1;
		nBottom = m_BigRect.bottom - m_BigRect.Height() / 2 + 1;
		nLeft = m_BigRect.left - m_BigRect.Width() / 2 - 1;
		nRight = m_BigRect.right - m_BigRect.Width() / 2 + 1;
//		nTop = nTop * (-1);
//		nBottom = nBottom * (-1);
//		nLeft = nLeft * (-1);
//		nRight = nRight * (-1);
	}
	if(pDC == NULL) 
	{
		AfxMessageBox("Pattern condition is not setting. Can't viewing!!");// SetPattern(CDC *dc, long nPatNum)함수 호출 필요");
		return FALSE;
	}
	dNozzle_Length = ((N_MAX_NOZZLE - 1) * N_NOZZLE_PITCH) / 2;
	// 현재 그릴 패턴에 대한 설정
	m_nDispenseMode = pDoc->m_structPatternData[m_nPatternNumber-1].m_nDispenseMode;
	// line일 경우를 대비해서 dc를 넘긴다.
	m_pLine->SetDC(pDC);
	m_pLine->m_dZoom = m_dZoom;
	m_pLine->m_pOrgPoint = m_pOrgPoint;
	// 모든 노즐을 다 그린다. (on 인것만) /////////////////////////////////////////////////////////
	for(int i = 0; i < MAX_NOZZLE; i++)
	{
		// off인것은 지나친다
		if(!m_bOnNozzle[i]) continue;
		if(pDoc->m_structDataEditor.m_nNzlMode == 1)
		{
			stDrop = pDoc->m_arDropData[m_nPatternNumber-1].GetAt(0);
			dTemp = stDrop.m_dPointY;
			stDrop = pDoc->m_arDropData[m_nPatternNumber-1].GetAt(1);
			stDrop.m_dPointY - dTemp > 0 ? bPlus = TRUE : bPlus = FALSE;
		}
		
		pDC->SelectObject(&m_LinePen[i]);
		m_pLine->m_bIsStart = TRUE;
//
		// on인것에 대해서 각 데이타별로 그림
		nLine = 0;
		for(int n = 0; n < pDoc->m_structPatternData[m_nPatternNumber-1].m_nDropDataCount; n++)
		{
			// 
			stDrop = pDoc->m_arDropData[m_nPatternNumber-1].GetAt(n);
			// m_nN이 0이면 화면에 표시하지 않는다
			if(stDrop.m_nN ==0) continue;

			if(pDoc->m_structDataEditor.m_nNzlMode == 1/* && (m_bIsBig)*/)
			{
				if(m_bIsBig)
				{
					if((long)((stDrop.m_dPointX + m_dNozzleOffset[i] + dNozzle_Length) * m_dZoom) + m_pOrgPoint.x < nLeft || (long)((stDrop.m_dPointX + m_dNozzleOffset[i] - dNozzle_Length) * m_dZoom) + m_pOrgPoint.x > nRight)
					{
						if(n == 0 || dTemp != stDrop.m_dPointX)
						{
							nLine++;
							dTemp = stDrop.m_dPointX;
							bPlus = 1 - bPlus;
						}
						continue;
					}
				}
				m_pDrop->m_dZoom = m_dZoom;
				m_pDrop->m_pOrgPoint = m_pOrgPoint;
				m_pDrop->m_pOrgPoint.x = m_pDrop->m_pOrgPoint.x * (-1);
				m_pDrop->m_pOrgPoint.y = m_pDrop->m_pOrgPoint.y * (-1);

				m_pDrop->m_nPattern = m_nPatternNumber-1;
				int nCell = 0;
//				for(int nCell = 0; nCell < pDoc->m_structPatternData[m_nPatternNumber-1].m_nCellNO; nCell++)
//				{
//					if(stDrop.m_dPointX + m_dNozzleOffset[i] >= pDoc->m_structPatternData[m_nPatternNumber-1].m_nCellStartX[nCell] - pDoc->m_structDataEditor.m_dGlassSizeX/2 - 0.01 &&
//						stDrop.m_dPointX + m_dNozzleOffset[i] <= pDoc->m_structPatternData[m_nPatternNumber-1].m_nCellEndX[nCell] - pDoc->m_structDataEditor.m_dGlassSizeX/2 + 0.01) break;
//				}

				double TempCoord = stDrop.m_dPointX;
				if(i >= 0 && i <= MAX_NOZZLE-1) // i : head번호..
						m_pDrop->SetPoint(TempCoord + m_dNozzleOffset[i] + m_pHomePoint.x, stDrop.m_dPointY + m_pHomePoint.y);


				if(n == 0 || dTemp != stDrop.m_dPointX)
				{
					nLine++;
					dTemp = stDrop.m_dPointX;
					dNPitch = fabs(pDoc->m_structPatternData[m_nPatternNumber-1].m_dWidthY) / (pDoc->m_structPatternData[m_nPatternNumber-1].m_nNShot - 1);
					bPlus == FALSE ? dNPitch = dNPitch * (-1) : 1;

					if(stDrop.m_nZ[i]) 
					{
						pDC->SelectObject(&m_LinePen[i]);
						pDC->SelectObject(&m_OnBrush[i]);
					}
					else
					{
						pDC->SelectObject(&m_OffPen);
						pDC->SelectObject(&m_OffBrush);
					}

					for(int k = 0; k < pDoc->m_structPatternData[m_nPatternNumber-1].m_nNShot; k++)
					{
						if(g_nNTypeInfo[0] == n+1 && g_nNTypeInfo[1] == i && g_nNTypeInfo[2] == k)
						{
							m_pDrop->m_bOnly = g_bOnly_1_Point;
							m_pDrop->m_nNLine = nLine;
						}
						else
						{
							m_pDrop->m_bOnly = FALSE;
						}

						if(m_bIsBig)
						if((long)((stDrop.m_dPointY + k*dNPitch) * m_dZoom) + m_pOrgPoint.y > nTop|| (long)((stDrop.m_dPointY + k*dNPitch) * m_dZoom) + m_pOrgPoint.y < nBottom)
							continue;
						if(m_dZoom < 1.3)
							m_pDrop->Draw_N_Line(pDC, (stDrop.m_dPointX + m_dNozzleOffset[i]), stDrop.m_dPointY + k*dNPitch);
						else 
							m_pDrop->Draw_N_Drop(pDC, (stDrop.m_dPointX + m_dNozzleOffset[i]), stDrop.m_dPointY + k*dNPitch);
					}
					pDC->SelectObject(&m_Center);
					if(g_nNTypeInfo[1] == MAX_NOZZLE && m_bIsBig) m_pDrop->Draw_N_Center(pDC, nCell, dTemp + m_dNozzleOffset[i]);
					bPlus = 1 - bPlus;

				}
				continue;
			}

			// dispense mode가 0이면 drop
			// drop과 line mode가 화면상에 표현되는것은 동일하게한다.(03년9월24일)
			if(m_nDispenseMode == 0 || m_nDispenseMode == 1 || m_nDispenseMode == 2) 
			{
				// drop이 on인지 off인지에 따라 dc에 펜과 브러쉬를 다시 설정한다.
				if(pDoc->m_structDataEditor.m_nNzlMode == 1)
				{			
				}
				else if(pDoc->m_structDataEditor.m_nNzlMode == 2)
				{
					if(stDrop.m_nZ[i]) 
					{
						if(g_bOnly_Center) pDC->SelectObject(&m_OnPen[i]);
						else pDC->SelectObject(&m_PosPen);
						pDC->SelectObject(&m_WhiteBrush);
						if(g_n2NozzleInfo[0] == n+1 && g_n2NozzleInfo[1] == i)
						{
							m_pDrop->m_bOnly = g_bOnly_1_Point;
						}
						else
						{
							m_pDrop->m_bOnly = FALSE;
						}

						if(g_n2NozzleInfo[0] == n+1 && g_n2NozzleInfo[1] == i)
						{
							m_pDrop->m_b2NozzleDiplay = TRUE;
						}
						else
						{
							m_pDrop->m_b2NozzleDiplay = FALSE;
						}
						pDC->SelectObject(&m_OnPen[i]);
						pDC->SelectObject(&m_OnBrush[i]);
					}
					else continue;
				}
				else
				{
					if(stDrop.m_nZ[i]) 
					{
						pDC->SelectObject(&m_OnPen[i]);
						pDC->SelectObject(&m_OnBrush[i]);
					}
					else
					{
						pDC->SelectObject(&m_OffPen);
						pDC->SelectObject(&m_OffBrush);
					}
				}

				// drop 데이타를 그리기 위함 ///////////////////////////////////////
				// drop데이타의 종류를 매번 설정해줌
				// add인지 검사
				m_pDrop->m_bIsAdd = FALSE;
				m_pDrop->m_bIsDel = FALSE;

				//dhkim Add Dell기능을 사용하지 않으므로 잠시 지움..
				// Add point의 경우 회색으로 표시. 2003. 07. 25. 수정.
				if(stDrop.m_nAddDel[i] > 0 && stDrop.m_nAddDel[i] <= 20) 
				{
					m_pDrop->m_bIsAdd = TRUE;
					m_pDrop->m_nAddDelNum = stDrop.m_nAddDel[i];
					//pDC->SelectObject(&m_OffPen);
					//pDC->SelectObject(&m_OffBrush);
				}
				else if(stDrop.m_nAddDel[i] >= -20 && stDrop.m_nAddDel[i] < 0)
				{
					m_pDrop->m_nAddDelNum = stDrop.m_nAddDel[i];
					m_pDrop->m_bIsDel = TRUE;
				}

				m_pDrop->m_dZoom = m_dZoom;
				m_pDrop->m_pOrgPoint = m_pOrgPoint;

				if(pDoc->m_structDataEditor.m_nNzlMode == 2)
				{
					m_pDrop->m_pOrgPoint.x = m_pDrop->m_pOrgPoint.x * (-1);
					m_pDrop->m_pOrgPoint.y = m_pDrop->m_pOrgPoint.y * (-1);
				}

				//
//				m_pDrop->SetPoint(stDrop.m_dPointX + m_dNozzleOffset[i] + m_pHomePoint.x,/**/ stDrop.m_dPointY + m_pHomePoint.y/* + pDoc->m_structPatternData[m_nPatternNumber-1].m_dColumnOffset[0]*/);
				double TempCoord = stDrop.m_dPointX;
				if(pDoc->m_structDataEditor.m_nNzlMode == 0)
				{
					if(i >= 0 && i <= MAX_NOZZLE-1) // i : head번호..
						m_pDrop->SetPoint(TempCoord + m_dNozzleOffset[i] + m_pHomePoint.x, stDrop.m_dPointY + m_pHomePoint.y);
				}
				else /*(pDoc->m_structDataEditor.m_nNzlMode == 0)*/
				{
					if(i >= 0 && i <= MAX_NOZZLE-1) // i : head번호..
						m_pDrop->SetPoint(TempCoord + m_dNozzleOffset[i] + m_pHomePoint.x, stDrop.m_dPointY + m_pHomePoint.y);
				}

				m_pDrop->m_nPattern = m_nPatternNumber-1;

				
//131229 넘버링 수정 해야됨. 
///////////////////////Yamary...flexible//////////////////////////////////////////////////////////////////////////////////
// 				if(pDoc->m_structPatternData[m_nPatternNumber-1].m_nDispenseMode != 2) 
// 				{
// 					if ( !pDoc->m_structDataEditor.m_bSelectDropMode )
// 					{
// 						m_pDrop->m_nNumber = n+1;
// 					}
// 					else
// 					{
// 						if ( pDoc->m_structDataEditor.m_nNzlMode != 1 )
// 						{
// 							if ( pDoc->m_structDataEditor.m_bMMGLineDropMode ){
// 								if ( pDoc->m_structPatternData[m_nPatternNumber-1].m_bMMG )
// 									z4 = n + pDoc->m_nMMGDropcount;
// 								else
// 									z4 = n + pDoc->m_nMainDropCount;
// 							}
// 							else 
// 								z4 = n + pDoc->m_nDropcount;
// 
// 							if ( pDoc->m_structDataEditor.m_bMMGLineDropMode ){
// 								if( pDoc->m_structPatternData[m_nPatternNumber-1].m_bMMG ) 
// 									stLine = pDoc->m_arMMGLineDataBk->GetAt(z4);
// 								else{
// 									stLine = pDoc->m_arMainLineDataBk->GetAt(z4);
// 								}
// 							}
// 							else{
// 								stLine = pDoc->m_arLineDataBk->GetAt(z4);
// 							}
// 
// 							m_pDrop->m_nNumber = stLine.DropNum; 
// 						}
// 						else 
// 							m_pDrop->m_nNumber = n+1;
// 					}
// 				}
// 				else
// 				{
// 					for(int j = 0; j < pDoc->m_structDataEditor.m_nLinePatternNumber; j++)
// 					{
// 						for(int k = 0; k < pDoc->m_nLineDropCount; k++)
// 						{
//  							stLine = pDoc->m_arLineData[j].GetAt(k);
// 
// 							if(stLine.DropNum == n && stLine.PatNo == m_nPatternNumber-1)
// 							{
// 								m_pDrop->m_nNumber = k + 1;
// 								m_pDrop->m_nLine = j + 1;
// 								break;
// 							}
// 						}
// 						if(stLine.DropNum == n && stLine.PatNo == m_nPatternNumber-1) break;
// 					}
// 				}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				
				m_pDrop->m_bIsDisplayText = m_bIsDisplayText;
				m_pDrop->m_bIsDisplayText2 = m_bIsDisplayText2;
//				if(pDoc->m_structDataEditor.m_n2Nozzle || !g_bOnly_Nozzle) m_pDrop->Draw(pDC);
				if(pDoc->m_structDataEditor.m_nNzlMode == 0 && !g_bOnly_Nozzle)
					m_pDrop->Draw(pDC);
				else if(pDoc->m_structDataEditor.m_nNzlMode == 2 && !g_bOnly_Center)
					m_pDrop->Draw_2Nozzle(pDC);
				//////////////////////////////////////////////////////////////////
			} // End of for(int n = 0; n < pDoc->m_structPatternData[m_nPatternNumber-1].m_nDropDataCount; n++)
		}
/*		
		if(pDoc->m_structDataEditor.m_nNzlMode > 0 && pDoc->m_structDataEditor.m_nNzlMode != 1)
		{
			// off인것은 지나친다
			if(!m_bOnNozzle[i]) continue;
			// on인것에 대해서 각 데이타별로 그림
			for(int n = 0; n < pDoc->m_structPatternData[m_nPatternNumber-1].m_nDropDataCount; n++)
			{
				// 
				m_pDrop->m_nCount = n;
				stDrop = pDoc->m_arDropData[m_nPatternNumber-1].GetAt(n);

				// m_nN이 0이면 화면에 표시하지 않는다
				if(stDrop.m_nN ==0) continue;

				// dispense mode가 0이면 drop
				// drop과 line mode가 화면상에 표현되는것은 동일하게한다.(03년9월24일)
				if(m_nDispenseMode == 0 || m_nDispenseMode == 1 || m_nDispenseMode == 2) 
				{
					// drop이 on인지 off인지에 따라 dc에 펜과 브러쉬를 다시 설정한다.
					if(stDrop.m_nZ[i]) 
					{
						if(g_n2NozzleInfo[0] == n+1 && g_n2NozzleInfo[1] == i)
						{
							m_pDrop->m_b2NozzleDiplay = TRUE;
						}
						else
						{
							m_pDrop->m_b2NozzleDiplay = FALSE;
						}
						pDC->SelectObject(&m_OnPen[i]);
						pDC->SelectObject(&m_OnBrush[i]);
					}
					else continue;
					m_pDrop->SetPoint(stDrop.m_dPointX + m_dNozzleOffset[i] + m_pHomePoint.x, stDrop.m_dPointY + m_pHomePoint.y);
					m_pDrop->m_nPattern = m_nPatternNumber-1;
///////////////////////Yamary...flexible//////////////////////////////////////////////////////////////////////////////////
					if(pDoc->m_structPatternData[m_nPatternNumber-1].m_nDispenseMode != 2) m_pDrop->m_nNumber = n+1;
					else
					{

						for(int j = 0; j < pDoc->m_structDataEditor.m_nLinePatternNumber; j++)
						{
							for(int k = 0; k < pDoc->m_nLineDropCount; k++)
							{
 								stLine = pDoc->m_arLineData[j].GetAt(k);
								if(stLine.DropNum == n && stLine.PatNo == m_nPatternNumber-1)
								{
									m_pDrop->m_nNumber = k + 1;
									m_pDrop->m_nLine = j + 1;
									break;
								}
							}
							if(stLine.DropNum == n && stLine.PatNo == m_nPatternNumber-1) break;
						}
					}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					if(g_bOnly_Center == FALSE) m_pDrop->Draw_2Nozzle(pDC);
					//////////////////////////////////////////////////////////////////
				} // End of for(int n = 0; n < pDoc->m_structPatternData[m_nPatternNumber-1].m_nDropDataCount; n++)
			}
		}
*/
		/////////////////////////////////////////////
		// 각 판넬의 정보 표시. 2003. 07. 25.////////
		if((m_dZoom < 0.5) && (m_bIsBig) /*&& pDoc->m_structDataEditor.m_nNzlMode != 1*/)
		{
			CString sPanelNum="", temp="";
			CPoint pt; pt.x=0; pt.y=0;
			m_pOldLogFont = pDC->SelectObject(&m_newLogFont);
			/////////////////////////////

//			if(i >= 0 && i <= 5) // i : head번호..
//			{
				pt.x = (long)((pDoc->m_structPatternData[m_nPatternNumber-1].m_dCenterPositionX + (long)m_dNozzleOffset[i] + m_pHomePoint.x) * m_dZoom) + m_pOrgPoint.x;
				pt.y = (long)((pDoc->m_structPatternData[m_nPatternNumber-1].m_dCenterPositionY + m_pHomePoint.y/* + pDoc->m_structPatternData[m_nPatternNumber-1].m_dColumnOffset[0]*/) * m_dZoom) + m_pOrgPoint.y;
//			}
//			else /*if(i >= 6 && i <= 11)*/
//			{
//				pt.x = (long)((pDoc->m_structPatternData[m_nPatternNumber-1].m_dCenterPositionX + (long)m_dNozzleOffset[i] + m_pHomePoint.x) * m_dZoom) + m_pOrgPoint.x;
//				pt.y = (long)((pDoc->m_structPatternData[m_nPatternNumber-1].m_dCenterPositionY + m_pHomePoint.y + /* add*/pDoc->m_structPatternData[m_nPatternNumber-1].m_dColumnOffset[1]) * m_dZoom) + m_pOrgPoint.y;
//			}
				pt.x = pt.x * (-1);
				pt.y = pt.y * (-1);

			// GlassIden 과 panel정보의 설정을 matching한다(8월7일)
			// A,B,C...   1,2,3...
			sPanelNum = pDoc->m_strGlassIden[pDoc->m_structPatternData[m_nPatternNumber-1].m_nPanelNo[i] -1] ;

//			if( (i == 4) && (pDoc->m_structDataEditor.m_bVolCon[4] == TRUE) )
//				temp.Format(":%.2f ",pDoc->m_structPatternData[m_nPatternNumber-1].m_dDropWeight[4]);
//			else
//				temp.Format(" : %d",pDoc->m_nValidDropPoint[m_nPatternNumber-1][i]);

//			m_sPanelNum = m_sPanelNum + temp;
			if(!m_bIsDisplayText2) pDC->TextOut(pt.x, pt.y, sPanelNum);
			str.Format("H%d",i+1);
//			if(!m_bIsDisplayText2) pDC->TextOut(pt.x-10, pt.y+20, str);
		} // End of if((m_dZoom < 0.5) && (m_bIsBig))
		/////////////////////////////////////////////
	} // End of for(int i = 0; i < MAX_NOZZLE; i++)
//	if(pDoc->m_structDataEditor.m_b2Nozzle)
//	{
//		for(int i = 0; i < MAX_NOZZLE; i++)
//		{
//			// off인것은 지나친다
//			if(!m_bOnNozzle[i]) continue;
//			// on인것에 대해서 각 데이타별로 그림
//			for(int n = 0; n < pDoc->m_structPatternData[m_nPatternNumber-1].m_nDropDataCount; n++)
//			{
//				// 
//				m_pDrop->m_nCount = n;
//				stDrop = pDoc->m_arDropData[m_nPatternNumber-1].GetAt(n);
//
//				// m_nN이 0이면 화면에 표시하지 않는다
//				if(stDrop.m_nN ==0) continue;
//
//				// dispense mode가 0이면 drop
//				// drop과 line mode가 화면상에 표현되는것은 동일하게한다.(03년9월24일)
//				if(m_nDispenseMode == 0 || m_nDispenseMode == 1 || m_nDispenseMode == 2) 
//				{
//					// drop이 on인지 off인지에 따라 dc에 펜과 브러쉬를 다시 설정한다.
//					if(stDrop.m_nZ[i]) 
//					{
//						if(g_n2NozzleInfo[0] == n+1 && g_n2NozzleInfo[1] == i)
//						{
//							m_pDrop->m_b2NozzleDiplay = TRUE;
//						}
//						else
//						{
//							m_pDrop->m_b2NozzleDiplay = FALSE;
//						}
//						pDC->SelectObject(&m_OnPen[i]);
//						pDC->SelectObject(&m_OnBrush[i]);
//					}
//					else continue;
//					m_pDrop->SetPoint(stDrop.m_dPointX + m_dNozzleOffset[i] + m_pHomePoint.x,/**/ stDrop.m_dPointY + m_pHomePoint.y/* + pDoc->m_structPatternData[m_nPatternNumber-1].m_dColumnOffset[0]*/);
//					m_pDrop->m_nPattern = m_nPatternNumber-1;
/////////////////////////Yamary...flexible//////////////////////////////////////////////////////////////////////////////////
//					if(pDoc->m_structPatternData[m_nPatternNumber-1].m_nDispenseMode != 2) m_pDrop->m_nNumber = n+1;
//					else
//					{
//						for(int j = 0; j < pDoc->m_structDataEditor.nLinePattern; j++)
//						{
//							for(int k = 0; k < pDoc->m_nTotalPoint_Line[j]; k++)
//							{
// 								stLine = pDoc->m_arLineData[j].GetAt(k);
//								if(stLine.DropNum == n && stLine.m_nPatternNo == m_nPatternNumber-1)
//								{
//									m_pDrop->m_nNumber = k + 1;
//									m_pDrop->m_nLine = j + 1;
//									break;
//								}
//							}
//							if(stLine.DropNum == n && stLine.m_nPatternNo == m_nPatternNumber-1) break;
//						}
//					}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//					if(g_bOnly_Center == FALSE) m_pDrop->Draw_2Nozzle(pDC);
//					//////////////////////////////////////////////////////////////////
//				} // End of for(int n = 0; n < pDoc->m_structPatternData[m_nPatternNumber-1].m_nDropDataCount; n++)
//			}
//		} // End of for(int i = 0; i < MAX_NOZZLE; i++)
//	}
	////////////////////////////////////////////////////////////////////////////////////////////////

	return TRUE;

// 	CP8CA_LcDispDoc *pDoc = GetDocument();
// 	DROP_DATA stDrop;				// dropdata 구조체
// 	LINE_DATA stLine; // Yamary...flexible
// 	CString str;	
// 	double dTemp, dNozzle_Length;
// 	double dNPitch;
// 	BOOL bPlus;
// 	int nLine = 0;
// 	int nTop, nBottom, nRight, nLeft;
// 	if(m_bIsBig)
// 	{
//  		nTop = m_BigRect.top - m_BigRect.Height() / 2 - 1;
// 		nBottom = m_BigRect.bottom - m_BigRect.Height() / 2 + 1;
// 		nLeft = m_BigRect.left - m_BigRect.Width() / 2 - 1;
// 		nRight = m_BigRect.right - m_BigRect.Width() / 2 + 1;
// //		nTop = nTop * (-1);
// //		nBottom = nBottom * (-1);
// //		nLeft = nLeft * (-1);
// //		nRight = nRight * (-1);
// 	}
// 	if(pDC == NULL) 
// 	{
// 		AfxMessageBox("Pattern condition is not setting. Can't viewing!!");// SetPattern(CDC *dc, long nPatNum)함수 호출 필요");
// 		return FALSE;
// 	}
// 	dNozzle_Length = ((N_MAX_NOZZLE - 1) * N_NOZZLE_PITCH) / 2;
// 	// 현재 그릴 패턴에 대한 설정
// 	m_nDispenseMode = pDoc->m_structPatternData[m_nPatternNumber-1].m_nDispenseMode;
// 	// line일 경우를 대비해서 dc를 넘긴다.
// 	m_pLine->SetDC(pDC);
// 	m_pLine->m_dZoom = m_dZoom;
// 	m_pLine->m_pOrgPoint = m_pOrgPoint;
// 	// 모든 노즐을 다 그린다. (on 인것만) /////////////////////////////////////////////////////////
// 	for(int i = 0; i < MAX_NOZZLE; i++)
// 	{
// 		// off인것은 지나친다
// 		if(!m_bOnNozzle[i]) continue;
// 		if(pDoc->m_structDataEditor.m_nNzlMode == 1)
// 		{
// 			stDrop = pDoc->m_arDropData[m_nPatternNumber-1].GetAt(0);
// 			dTemp = stDrop.m_dPointY;
// 			stDrop = pDoc->m_arDropData[m_nPatternNumber-1].GetAt(1);
// 			stDrop.m_dPointY - dTemp > 0 ? bPlus = TRUE : bPlus = FALSE;
// 		}
// 		
// 		pDC->SelectObject(&m_LinePen[i]);
// 		m_pLine->m_bIsStart = TRUE;
// //
// 		// on인것에 대해서 각 데이타별로 그림
// 		nLine = 0;
// 		for(int n = 0; n < pDoc->m_structPatternData[m_nPatternNumber-1].m_nDropDataCount; n++)
// 		{
// 			// 
// 			stDrop = pDoc->m_arDropData[m_nPatternNumber-1].GetAt(n);
// 			// m_nN이 0이면 화면에 표시하지 않는다
// 			if(stDrop.m_nN ==0) continue;
// 			if(pDoc->m_structDataEditor.m_nNzlMode == 1/* && (m_bIsBig)*/)
// 			{
// 				if(m_bIsBig)
// 				{
// 					if((long)((stDrop.m_dPointX + m_dNozzleOffset[i] + dNozzle_Length) * m_dZoom) + m_pOrgPoint.x < nLeft || (long)((stDrop.m_dPointX + m_dNozzleOffset[i] - dNozzle_Length) * m_dZoom) + m_pOrgPoint.x > nRight)
// 					{
// 						if(n == 0 || dTemp != stDrop.m_dPointX)
// 						{
// 							nLine++;
// 							dTemp = stDrop.m_dPointX;
// 							bPlus = 1 - bPlus;
// 						}
// 						continue;
// 					}
// 				}
// 				m_pDrop->m_dZoom = m_dZoom;
// 				m_pDrop->m_pOrgPoint = m_pOrgPoint;
// 				m_pDrop->m_pOrgPoint.x = m_pDrop->m_pOrgPoint.x * (-1);
// 				m_pDrop->m_pOrgPoint.y = m_pDrop->m_pOrgPoint.y * (-1);
// 
// 				m_pDrop->m_nPattern = m_nPatternNumber-1;
// 				int nCell = 0;
// //				for(int nCell = 0; nCell < pDoc->m_structPatternData[m_nPatternNumber-1].m_nCellNO; nCell++)
// //				{
// //					if(stDrop.m_dPointX + m_dNozzleOffset[i] >= pDoc->m_structPatternData[m_nPatternNumber-1].m_nCellStartX[nCell] - pDoc->m_structDataEditor.m_dGlassSizeX/2 - 0.01 &&
// //						stDrop.m_dPointX + m_dNozzleOffset[i] <= pDoc->m_structPatternData[m_nPatternNumber-1].m_nCellEndX[nCell] - pDoc->m_structDataEditor.m_dGlassSizeX/2 + 0.01) break;
// //				}
// 
// 				double TempCoord = stDrop.m_dPointX;
// 				if(i >= 0 && i <= MAX_NOZZLE-1) // i : head번호..
// 						m_pDrop->SetPoint(TempCoord + m_dNozzleOffset[i] + m_pHomePoint.x, stDrop.m_dPointY + m_pHomePoint.y);
// 
// 
// 				if(n == 0 || dTemp != stDrop.m_dPointX)
// 				{
// 					nLine++;
// 					dTemp = stDrop.m_dPointX;
// 					dNPitch = fabs(pDoc->m_structPatternData[m_nPatternNumber-1].m_dWidthY) / (pDoc->m_structPatternData[m_nPatternNumber-1].m_nNShot - 1);
// 					bPlus == FALSE ? dNPitch = dNPitch * (-1) : 1;
// 					pDC->SelectObject(&m_LinePen[i]);
// 					pDC->SelectObject(&m_OnBrush[i]);
// 					for(int k = 0; k < pDoc->m_structPatternData[m_nPatternNumber-1].m_nNShot; k++)
// 					{
// 						if(g_nNTypeInfo[0] == n+1 && g_nNTypeInfo[1] == i && g_nNTypeInfo[2] == k)
// 						{
// 							m_pDrop->m_bOnly = g_bOnly_1_Point;
// 							m_pDrop->m_nNLine = nLine;
// 						}
// 						else
// 						{
// 							m_pDrop->m_bOnly = FALSE;
// 						}
// 
// 						if(m_bIsBig)
// 						if((long)((stDrop.m_dPointY + k*dNPitch) * m_dZoom) + m_pOrgPoint.y > nTop|| (long)((stDrop.m_dPointY + k*dNPitch) * m_dZoom) + m_pOrgPoint.y < nBottom)
// 							continue;
// 						if(m_dZoom < 1.3)
// 							m_pDrop->Draw_N_Line(pDC, (stDrop.m_dPointX + m_dNozzleOffset[i]), stDrop.m_dPointY + k*dNPitch);
// 						else 
// 							m_pDrop->Draw_N_Drop(pDC, (stDrop.m_dPointX + m_dNozzleOffset[i]), stDrop.m_dPointY + k*dNPitch);
// 					}
// 					pDC->SelectObject(&m_Center);
// 					if(g_nNTypeInfo[1] == MAX_NOZZLE && m_bIsBig) m_pDrop->Draw_N_Center(pDC, nCell, dTemp + m_dNozzleOffset[i]);
// 					bPlus = 1 - bPlus;
// 				}
// 				continue;
// 			}
// 
// 			// dispense mode가 0이면 drop
// 			// drop과 line mode가 화면상에 표현되는것은 동일하게한다.(03년9월24일)
// 			if(m_nDispenseMode == 0 || m_nDispenseMode == 1 || m_nDispenseMode == 2) 
// 			{
// 				// drop이 on인지 off인지에 따라 dc에 펜과 브러쉬를 다시 설정한다.
// 				if(pDoc->m_structDataEditor.m_nNzlMode == 1)
// 				{
// 					
// 				}
// 				else if(pDoc->m_structDataEditor.m_nNzlMode == 2)
// 				{
// 					if(stDrop.m_nZ[i]) 
// 					{
// 						if(g_bOnly_Center) pDC->SelectObject(&m_OnPen[i]);
// 						else pDC->SelectObject(&m_PosPen);
// 						pDC->SelectObject(&m_WhiteBrush);
// 						if(g_n2NozzleInfo[0] == n+1 && g_n2NozzleInfo[1] == i)
// 						{
// 							m_pDrop->m_bOnly = g_bOnly_1_Point;
// 						}
// 						else
// 						{
// 							m_pDrop->m_bOnly = FALSE;
// 						}
// 
// 						if(g_n2NozzleInfo[0] == n+1 && g_n2NozzleInfo[1] == i)
// 						{
// 							m_pDrop->m_b2NozzleDiplay = TRUE;
// 						}
// 						else
// 						{
// 							m_pDrop->m_b2NozzleDiplay = FALSE;
// 						}
// 						pDC->SelectObject(&m_OnPen[i]);
// 						pDC->SelectObject(&m_OnBrush[i]);
// 					}
// 					else continue;
// 				}
// 				else
// 				{
// 					if(stDrop.m_nZ[i]) 
// 					{
// 						pDC->SelectObject(&m_OnPen[i]);
// 						pDC->SelectObject(&m_OnBrush[i]);
// 					}
// 					else
// 					{
// 						pDC->SelectObject(&m_OffPen);
// 						pDC->SelectObject(&m_OffBrush);
// 					}
// 				}
// 
// 				// drop 데이타를 그리기 위함 ///////////////////////////////////////
// 				// drop데이타의 종류를 매번 설정해줌
// 				// add인지 검사
// 				m_pDrop->m_bIsAdd = FALSE;
// 				m_pDrop->m_bIsDel = FALSE;
// 
// 				//dhkim Add Dell기능을 사용하지 않으므로 잠시 지움..
// 				// Add point의 경우 회색으로 표시. 2003. 07. 25. 수정.
// 				if(stDrop.m_nAddDel[i] > 0 && stDrop.m_nAddDel[i] <= 20) 
// 				{
// 					m_pDrop->m_bIsAdd = TRUE;
// 					m_pDrop->m_nAddDelNum = stDrop.m_nAddDel[i];
// 					//pDC->SelectObject(&m_OffPen);
// 					//pDC->SelectObject(&m_OffBrush);
// 				}
// 				else if(stDrop.m_nAddDel[i] >= -20 && stDrop.m_nAddDel[i] < 0)
// 				{
// 					m_pDrop->m_nAddDelNum = stDrop.m_nAddDel[i];
// 					m_pDrop->m_bIsDel = TRUE;
// 				}
// 
// 				m_pDrop->m_dZoom = m_dZoom;
// 				m_pDrop->m_pOrgPoint = m_pOrgPoint;
// 
// 				if(pDoc->m_structDataEditor.m_nNzlMode == 2)
// 				{
// 					m_pDrop->m_pOrgPoint.x = m_pDrop->m_pOrgPoint.x * (-1);
// 					m_pDrop->m_pOrgPoint.y = m_pDrop->m_pOrgPoint.y * (-1);
// 				}
// 
// 				//
// //				m_pDrop->SetPoint(stDrop.m_dPointX + m_dNozzleOffset[i] + m_pHomePoint.x,/**/ stDrop.m_dPointY + m_pHomePoint.y/* + pDoc->m_structPatternData[m_nPatternNumber-1].m_dColumnOffset[0]*/);
// 				double TempCoord = stDrop.m_dPointX;
// 				if(pDoc->m_structDataEditor.m_nNzlMode == 0)
// 				{
// 					if(i >= 0 && i <= MAX_NOZZLE-1) // i : head번호..
// 						m_pDrop->SetPoint(TempCoord + m_dNozzleOffset[i] + m_pHomePoint.x, stDrop.m_dPointY + m_pHomePoint.y);
// 				}
// 				else /*(pDoc->m_structDataEditor.m_nNzlMode == 0)*/
// 				{
// 					if(i >= 0 && i <= MAX_NOZZLE-1) // i : head번호..
// 						m_pDrop->SetPoint(TempCoord + m_dNozzleOffset[i] + m_pHomePoint.x, stDrop.m_dPointY + m_pHomePoint.y);
// 				}
// 
// 				m_pDrop->m_nPattern = m_nPatternNumber-1;
// ///////////////////////Yamary...flexible//////////////////////////////////////////////////////////////////////////////////
// 				if(pDoc->m_structPatternData[m_nPatternNumber-1].m_nDispenseMode != 2) m_pDrop->m_nNumber = n+1;
// 				else
// 				{
// 					for(int j = 0; j < pDoc->m_structDataEditor.m_nLinePatternNumber; j++)
// 					{
// 						for(int k = 0; k < pDoc->m_nLineDropCount; k++)
// 						{
//  							stLine = pDoc->m_arLineData[j].GetAt(k);
// 							if(stLine.DropNum == n && stLine.PatNo == m_nPatternNumber-1)
// 							{
// 								m_pDrop->m_nNumber = k + 1;
// 								m_pDrop->m_nLine = j + 1;
// 								break;
// 							}
// 						}
// 						if(stLine.DropNum == n && stLine.PatNo == m_nPatternNumber-1) break;
// 					}
// 				}
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 				m_pDrop->m_bIsDisplayText = m_bIsDisplayText;
// 				m_pDrop->m_bIsDisplayText2 = m_bIsDisplayText2;
// //				if(pDoc->m_structDataEditor.m_n2Nozzle || !g_bOnly_Nozzle) m_pDrop->Draw(pDC);
// 				if(pDoc->m_structDataEditor.m_nNzlMode == 0 && !g_bOnly_Nozzle)
// 					m_pDrop->Draw(pDC);
// 				else if(pDoc->m_structDataEditor.m_nNzlMode == 2 && !g_bOnly_Center)
// 					m_pDrop->Draw_2Nozzle(pDC);
// 				//////////////////////////////////////////////////////////////////
// 			} // End of for(int n = 0; n < pDoc->m_structPatternData[m_nPatternNumber-1].m_nDropDataCount; n++)
// 		}
// 
// 		/////////////////////////////////////////////
// 		// 각 판넬의 정보 표시. 2003. 07. 25.////////
// 		if((m_dZoom < 0.5) && (m_bIsBig) /*&& pDoc->m_structDataEditor.m_nNzlMode != 1*/)
// 		{
// 			CString sPanelNum="", temp="";
// 			CPoint pt; pt.x=0; pt.y=0;
// 			m_pOldLogFont = pDC->SelectObject(&m_newLogFont);
// 			/////////////////////////////
// 
// //			if(i >= 0 && i <= 5) // i : head번호..
// //			{
// 				pt.x = (long)((pDoc->m_structPatternData[m_nPatternNumber-1].m_dCenterPositionX + (long)m_dNozzleOffset[i] + m_pHomePoint.x) * m_dZoom) + m_pOrgPoint.x;
// 				pt.y = (long)((pDoc->m_structPatternData[m_nPatternNumber-1].m_dCenterPositionY + m_pHomePoint.y/* + pDoc->m_structPatternData[m_nPatternNumber-1].m_dColumnOffset[0]*/) * m_dZoom) + m_pOrgPoint.y;
// //			}
// //			else /*if(i >= 6 && i <= 11)*/
// //			{
// //				pt.x = (long)((pDoc->m_structPatternData[m_nPatternNumber-1].m_dCenterPositionX + (long)m_dNozzleOffset[i] + m_pHomePoint.x) * m_dZoom) + m_pOrgPoint.x;
// //				pt.y = (long)((pDoc->m_structPatternData[m_nPatternNumber-1].m_dCenterPositionY + m_pHomePoint.y + /* add*/pDoc->m_structPatternData[m_nPatternNumber-1].m_dColumnOffset[1]) * m_dZoom) + m_pOrgPoint.y;
// //			}
// 			pt.x =  pt.x * (-1);pt.y =  pt.y * (-1);
// 			// GlassIden 과 panel정보의 설정을 matching한다(8월7일)
// 			// A,B,C...   1,2,3...
// 			sPanelNum = pDoc->m_strGlassIden[pDoc->m_structPatternData[m_nPatternNumber-1].m_nPanelNo[i] -1] ;
// 
// //			if( (i == 4) && (pDoc->m_structDataEditor.m_bVolCon[4] == TRUE) )
// //				temp.Format(":%.2f ",pDoc->m_structPatternData[m_nPatternNumber-1].m_dDropWeight[4]);
// //			else
// //				temp.Format(" : %d",pDoc->m_nValidDropPoint[m_nPatternNumber-1][i]);
// 
// //			m_sPanelNum = m_sPanelNum + temp;
// 			if(!m_bIsDisplayText2) pDC->TextOut(pt.x, pt.y, sPanelNum);
// 			str.Format("H%d",i+1);
// //			if(!m_bIsDisplayText2) pDC->TextOut(pt.x-10, pt.y+20, str);
// 		} // End of if((m_dZoom < 0.5) && (m_bIsBig))
// 		/////////////////////////////////////////////
// 	} // End of for(int i = 0; i < MAX_NOZZLE; i++)
// 	return TRUE;
}


// 패턴 설정시 패턴 넘버와 dc만 넘기면 여기서
// 알아서 doc의 정보를 읽어와서 패턴에 관련된 모든 설정을 한다.
// 딱 한번만 호출한다.
void CDrawPattern::SetPattern(CDC *dc, long nPatNum)
{
	CP8CA_LcDispDoc *pDoc = GetDocument();

	m_nPatternNumber = nPatNum;

	// doc에서 필요한 데이타 받아 오기 //
	for(int i = 0; i < MAX_NOZZLE; i++)
	{
		m_bOnNozzle[i] = pDoc->m_structPatternData[m_nPatternNumber-1].m_bIsOnHead[i];
		m_dNozzleOffset[i] = pDoc->m_structPatternData[m_nPatternNumber-1].m_dNozzleOffset[i];
		m_colorOn[i] = pDoc->m_colorNozzle[i];
	}
	m_colorWhite = RGB(255,255,255);
	m_pHomePoint = CPoint((long)pDoc->m_structDataEditor.m_dHomeX, (long)pDoc->m_structDataEditor.m_dHomeY);
	//////////////////////////////////////////////////////////////////////////////////////////////

	if(pDC == NULL)
	{
		pDC = dc;

		// 각 노즐별 펜과 브러쉬 생성
		for(i = 0; i < MAX_NOZZLE; i++)
		{
			m_OnPen[i].CreatePen(PS_SOLID, 3, m_colorOn[i]);
			m_LinePen[i].CreatePen(PS_SOLID, 1, m_colorOn[i]);
			m_OnBrush[i].CreateSolidBrush(m_colorOn[i]);
		}
		m_Center.CreatePen(PS_DOT, 1, RGB(0,0,0));

		// off시 펜과 브러쉬 생성
		m_OffPen.CreatePen(PS_SOLID, 3, m_colorOff);
		m_OffBrush.CreateSolidBrush(m_colorOff);
		m_PosPen.CreatePen(PS_SOLID, 3, m_Pos);//Yamary...
		m_WhiteBrush.CreateSolidBrush(m_colorWhite);
//	
		m_newFont.CreateFont(12,6,0,0,NULL,FALSE,FALSE,FALSE,ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY, 
		DEFAULT_PITCH,"돋음체");
//
		// 폰트 생성 및 속성 설정
//		CFont newFont, *pOldFont;
		LOGFONT logFont;
		logFont.lfHeight = 20;
		logFont.lfWidth = 8;
		logFont.lfEscapement = 0;
		logFont.lfOrientation = 0;
		logFont.lfWeight = FW_BOLD;//FW_NORMAL;
		logFont.lfItalic = FALSE;
		logFont.lfUnderline = FALSE;
		logFont.lfStrikeOut = FALSE;
		logFont.lfCharSet = DEFAULT_CHARSET;
		logFont.lfOutPrecision = OUT_CHARACTER_PRECIS;
		logFont.lfClipPrecision = CLIP_CHARACTER_PRECIS;
		logFont.lfQuality = DEFAULT_QUALITY;
		logFont.lfPitchAndFamily = DEFAULT_PITCH|FF_DONTCARE;
		strcpy(logFont.lfFaceName,"Arial");

		m_newLogFont.CreateFontIndirect(&logFont);
//
		// dc 설정하면서 원래 펜을 받아 놓는다.
		m_pOldPen = pDC->SelectObject(&m_OnPen[0]);
		m_pOldBrush = pDC->SelectObject(&m_OnBrush[0]);
		m_pOldFont = pDC->SelectObject(&m_newFont);
	}
}

CP8CA_LcDispDoc *CDrawPattern::GetDocument()
{
	CMainFrame *pFrame = (CMainFrame *)::AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	return pDoc;
}

// 모든 노즐중에서 선택된 drop point가 있는지 체크 한다.
// 있으면 당근 색을 바꾼다.
BOOL CDrawPattern::CheckSelect(double dX, double dY)
{
	CP8CA_LcDispDoc *pDoc = GetDocument();
	//
	DROP_DATA stDrop;				// dropdata 구조체
	BOOL bResult;

	int i = 0 , j = 0, k = 0;	
	bResult = FALSE;

	// 모든 노즐을 다 체크한다.(on 인것만) /////////////////////////////////////////////////////////
	for(i = 0; i < MAX_NOZZLE; i++)
	{
		// off인것은 지나친다
		if(!m_bOnNozzle[i]) continue;
		// 결과가 하나라도 있으면 다른 노즐은 다 지나간다.
		if(bResult) continue;

		// on인것에 대해서 각 데이타별로 체크
		for(int n = 0; n < pDoc->m_arDropData[m_nPatternNumber-1].GetSize(); n++)
		{
			stDrop = pDoc->m_arDropData[m_nPatternNumber-1].GetAt(n);

			// head1~head6 column1 , h7~h12 column2
			//if(i >= 0 && i <= 5) j = 0;
			//else j = 1;
			//
			if(i >= 0 && i <= 5) j = 0;
			else j = 1;
			m_pDrop->m_dPointX = stDrop.m_dPointX + m_dNozzleOffset[i] + m_pHomePoint.x;
			m_pDrop->m_dPointY = stDrop.m_dPointY + m_pHomePoint.y;

			m_pDrop->m_dPointX *= (-1.0);
			m_pDrop->m_dPointY *= (-1.0);
			//
			m_pDrop->m_dZoom = m_dZoom;
			

			if(m_pDrop->CheckSelect(dX, dY)) 
			{
				
				if(!m_bIsDirectSetting)
				{					
					for(k=0; k<MAX_NOZZLE; k++){
						if(m_bOnNozzle[k]){
							if(stDrop.m_nZ[k]) stDrop.m_nZ[k] = FALSE;
							else stDrop.m_nZ[k] = TRUE;										
						}
					}

					pDoc->m_arDropData[m_nPatternNumber-1].SetAt(n, stDrop);
				}
				else
				{
					m_nCurSelDropNum = n+1;
				}

				bResult = TRUE;
				break;
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////

	return bResult;
}

BOOL CDrawPattern::CheckSelect_2Nozzle(double dX, double dY)
{
	int i;
	CP8CA_LcDispDoc *pDoc = GetDocument();
	DROP_DATA stDrop;				// dropdata 구조체
	
	// 모든 노즐을 다 체크한다.(on 인것만) /////////////////////////////////////////////////////////
	for(i = 0; i < MAX_NOZZLE; i++)
	{
		// off인것은 지나친다
		if(!m_bOnNozzle[i]) continue;
		// 결과가 하나라도 있으면 다른 노즐은 다 지나간다.
		// on인것에 대해서 각 데이타별로 체크
		for(int n = 0; n < pDoc->m_arDropData[m_nPatternNumber-1].GetSize(); n++)
		{
			stDrop = pDoc->m_arDropData[m_nPatternNumber-1].GetAt(n);

			// head1~head6 column1 , h7~h12 column2
//			if(i >= 0 && i <= 5) j = 0;
//			else j = 1;
			//
			m_pDrop->m_dPointX = stDrop.m_dPointX + m_dNozzleOffset[i] + m_pHomePoint.x;
			m_pDrop->m_dPointY = stDrop.m_dPointY + m_pHomePoint.y;// + pDoc->m_structPatternData[m_nPatternNumber-1].m_dColumnOffset[j];
			//
			m_pDrop->m_dZoom = m_dZoom;
			
			if(m_pDrop->CheckSelect(dX, dY)) 
			{
				g_n2NozzleInfo[0] = m_nCurSelDropNum = n+1;
				g_n2NozzleInfo[1] = m_nCurSelHeadNum = i;
				if(pDoc->m_structDataEditor.m_nNzlMode != 0 && g_bOnly_Center && nX_Display) g_bOnly_1_Point = TRUE;
				else g_bOnly_1_Point = FALSE;
				return TRUE;
			}
			else g_bOnly_1_Point = FALSE;
		}
	}
	g_n2NozzleInfo[1] = MAX_NOZZLE;
	////////////////////////////////////////////////////////////////////////////////////////////////
	return FALSE;

}

BOOL CDrawPattern::CheckSelect_NType(double dX, double dY)
{
	int i;
	CP8CA_LcDispDoc *pDoc = GetDocument();
	DROP_DATA stDrop;				// dropdata 구조체
	DROP_DATA stDrop_Temp;
	double dStart;
	double dTemp = 0.0;
	BOOL bPlus = FALSE;
	double dNPitch;	
	// 모든 노즐을 다 체크한다.(on 인것만) /////////////////////////////////////////////////////////
	for(i = 0; i < MAX_NOZZLE; i++)
	{
		// off인것은 지나친다
		if(!m_bOnNozzle[i]) continue;
		// 결과가 하나라도 있으면 다른 노즐은 다 지나간다.
		// on인것에 대해서 각 데이타별로 체크
		stDrop = pDoc->m_arDropData[m_nPatternNumber-1].GetAt(0);
		dTemp = stDrop.m_dPointY;
		stDrop = pDoc->m_arDropData[m_nPatternNumber-1].GetAt(1);
		stDrop.m_dPointY - dTemp > 0 ? bPlus = TRUE : bPlus = FALSE;
		for(int n = 0; n < pDoc->m_arDropData[m_nPatternNumber-1].GetSize(); n++)
		{
			stDrop = pDoc->m_arDropData[m_nPatternNumber-1].GetAt(n);
			if(n == 0 || fabs(dTemp - stDrop.m_dPointX) > 0.01)
			{
				dTemp = stDrop.m_dPointX;
				dStart = stDrop.m_dPointX - ((N_MAX_NOZZLE - 1) * N_NOZZLE_PITCH / 2);
//				for(int nCell = 0; nCell < pDoc->m_structPatternData[m_nPatternNumber-1].m_nCellNO; nCell++)
//				{
//					if(stDrop.m_dPointX + m_dNozzleOffset[i] >= pDoc->m_structPatternData[m_nPatternNumber-1].m_nCellStartX[nCell] - pDoc->m_structDataEditor.m_dGlassSizeX/2 - 0.01 &&
//						stDrop.m_dPointX + m_dNozzleOffset[i] <= pDoc->m_structPatternData[m_nPatternNumber-1].m_nCellEndX[nCell] - pDoc->m_structDataEditor.m_dGlassSizeX/2 + 0.01) break;
//				}
//				dNPitch = fabs(pDoc->m_structPatternData[m_nPatternNumber-1].m_nCellStartY[nCell] - pDoc->m_structPatternData[m_nPatternNumber-1].m_nCellEndY[nCell]) / (pDoc->m_structPatternData[m_nPatternNumber-1].m_nNShot - 1);
				dNPitch = fabs(pDoc->m_structPatternData[m_nPatternNumber-1].m_dWidthY) / (pDoc->m_structPatternData[m_nPatternNumber-1].m_nNShot - 1);
			
				bPlus == FALSE ? dNPitch = dNPitch * (-1) : 1;
				for(int y = 0; y < pDoc->m_structPatternData[m_nPatternNumber-1].m_nNShot; y++)
				{
					for(int k = 0; k < N_MAX_NOZZLE; k++)
					{
						m_pDrop->m_dPointX = dStart + (k * N_NOZZLE_PITCH) + m_dNozzleOffset[i] + m_pHomePoint.x;
						m_pDrop->m_dPointY = stDrop.m_dPointY + y*dNPitch + m_pHomePoint.y;// + pDoc->m_structPatternData[m_nPatternNumber-1].m_dColumnOffset[j];
						//
						m_pDrop->m_dZoom = m_dZoom;
						if(m_pDrop->CheckSelect(dX, dY)) 
						{
							g_nNTypeInfo[0] = m_nCurSelDropNum = n+1;
							g_nNTypeInfo[1] = m_nCurSelHeadNum = i;
							g_nNTypeInfo[2] = m_nCurSelPointNum = y;
							if(pDoc->m_structDataEditor.m_nNzlMode != 0) g_bOnly_1_Point = TRUE;
							else g_bOnly_1_Point = FALSE;
							return TRUE;
						}
						else g_bOnly_1_Point = FALSE;
					}

				}
				bPlus = 1 - bPlus;
			}
		}
	}
	g_nNTypeInfo[1] = MAX_NOZZLE;
	////////////////////////////////////////////////////////////////////////////////////////////////
	return FALSE;
	
}


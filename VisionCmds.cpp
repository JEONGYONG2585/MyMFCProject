// VisionCmds.cpp: implementation of the CVisionCmds class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "VisionCmds.h"
#include "VisionDefine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MUX1	0
#define MUX2	1

#define LINEWIDTH	320
#define LINECOLOR	RGB(255,0,0)
#define GRIDCOLOR	RGB(255,255,0)
#define TEXTCOLOR	RGB(0,255,0)

#define GRID	8
#define VISION_WIDTH	640
#define VISION_HEIGHT	480


#define CELL_WIDTH_H	10//4.1667//3.125
#define CELL_WIDTH_V	10//4.8482//3.6437
//#define CELL_WIDTH_H	4.75//4.1667//3.125
//#define CELL_WIDTH_V	4.85//4.8482//3.6437

#include "MilGrabber.h"

//extern CString m_strKindOfMachine;
//extern CMilGrabber grabber;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVisionCmds::CVisionCmds()
{
	//{{AFX_DATA_INIT(CVisionCmds)
	m_nColor = 0;
	m_nEdgeDir = 1;
	m_nMinDiff = 30;
	m_nPixelOffset = 2;
	m_sResultPos = _T("");
	m_sResutlGap = _T("");
	m_bShow = FALSE;
	//}}AFX_DATA_INIT

	m_nPanX = 160;
	m_nPanY = 0;
	m_dblZoom = 1.0;
	m_pDisplay0 = NULL;
	m_pDisplay1 = NULL;
	nChannel = 0;
	BOTH = TRUE;
	m_dblWidth = 0;
	m_dblWidthShort = 0;
	m_nPatternIDs = 2;

	m_dbWidthAbsPosX = 0.0;
	m_dbWidthAbsPosY = 0.0;
}

CVisionCmds::~CVisionCmds()
{	
}

void CVisionCmds::SetChannel(int nCam)
{
	nChannel = nCam;
}

BOOL CVisionCmds::Live(int nCam)
{
	if(nCam!=2)
	{
		if(grabber.IsLive(nCam)) return TRUE;
		grabber.Live((long)nCam);
		return TRUE;
	}
	else
	{
		if(grabber.IsLive(MUX1)&&grabber.IsLive(MUX2)) return TRUE;
		grabber.Live(MUX1);
		grabber.Live(MUX2);
		return TRUE;
	}
	return FALSE;
}

void CVisionCmds::Grab(int nCam)
{
	if(nCam!=2)
	{
		if(!grabber.IsLive(nCam)) 
		{
			grabber.Live(nCam);
			Sleep(500);
		}
		grabber.Grab((long)nCam);
	}
	else
	{
		if(!grabber.IsLive(MUX1) || !grabber.IsLive(MUX2))
		{
			grabber.Live(MUX1);
			grabber.Live(MUX2);
			Sleep(500);
		}
		grabber.Grab(MUX1);
		grabber.Grab(MUX2);
	}
}

void CVisionCmds::SetDisplay(int nCam)
{
	switch (nCam)
	{
	case MUX1:
		m_pDisplay0->Open(0,0,0);
		grabber.SetDisplay(MUX1, m_pDisplay0->GetDisplayID());
		m_pDisplay0->ImagePan(m_nPanX, m_nPanY);
		Live(MUX1);
		nChannel = MUX1;
		break;
	case MUX2:
		m_pDisplay1->Open(0,0,0);
		grabber.SetDisplay(MUX2, m_pDisplay1->GetDisplayID());
		m_pDisplay1->ImagePan(m_nPanX, m_nPanY);
		Live(MUX2);
		nChannel = MUX2;
		break;
	}
}

void CVisionCmds::SetDisplay(int nCam, long pDisplay)
{
	switch (nCam)
	{
	case MUX1:		
		grabber.SetDisplay(MUX1, pDisplay);
		Live(MUX1);
		nChannel = MUX1;
		break;
	case MUX2:
		grabber.SetDisplay(MUX2, pDisplay);
		Live(MUX2);
		nChannel = MUX2;
		break;
	}
}

void CVisionCmds::CrossDisplay(CString str, int x, int y)
{
	int TextX = (VISION_WIDTH/2 - 3*str.GetLength());
	int TextY = 10;

	switch(nChannel)
	{
	case MUX1:
		ClearOverlay(MUX1);
		m_pDisplay0->DrawCrossLine(x, y, LINEWIDTH, LINECOLOR);
		m_pDisplay0->TextOut(str, TextX, TextY, TEXTCOLOR);
		if(!grabber.IsLive(MUX1))	m_pDisplay0->UpdateDisplay();
		break;
	case MUX2:
		ClearOverlay(MUX2);
		m_pDisplay1->DrawCrossLine(x, y, LINEWIDTH, LINECOLOR);
		m_pDisplay1->TextOut(str, TextX, TextY, TEXTCOLOR);
		if(!grabber.IsLive(MUX2))	m_pDisplay1->UpdateDisplay();
		break;
	}
}

void CVisionCmds::CrossDisplay(int nCam, CString str, int x, int y)
{
	int TextX = (VISION_WIDTH/2 - 3*str.GetLength());
	int TextY = 10;

	switch(nCam)
	{
	case MUX1:
		ClearOverlay(MUX1);
		m_pDisplay0->DrawCrossLine(x, y, LINEWIDTH, LINECOLOR);
		m_pDisplay0->TextOut(str, TextX, TextY, TEXTCOLOR);
		if(!grabber.IsLive(MUX1))	m_pDisplay0->UpdateDisplay();
		break;
	case MUX2:
		ClearOverlay(MUX2);
		m_pDisplay1->DrawCrossLine(x, y, LINEWIDTH, LINECOLOR);
		m_pDisplay1->TextOut(str, TextX, TextY, TEXTCOLOR);
		if(!grabber.IsLive(MUX2))	m_pDisplay1->UpdateDisplay();
		break;
	}
}

void CVisionCmds::CrossDisplay(CString str1, CString str2, int x1, int y1, int x2, int y2)
{
	int TextX1 = (VISION_WIDTH/2 - 3*str1.GetLength());
	int TextY1 = 10;

	int TextX2 = (VISION_WIDTH/2 - 3*str2.GetLength());
	int TextY2 = 10;
	
	ClearOverlay();

	m_pDisplay0->DrawCrossLine(x1, y1, LINEWIDTH, LINECOLOR);
	m_pDisplay0->TextOut(str1, TextX1, TextY1, TEXTCOLOR);
	if(!grabber.IsLive(MUX1))	m_pDisplay0->UpdateDisplay();

	m_pDisplay1->DrawCrossLine(x2, y2, LINEWIDTH, LINECOLOR);
	m_pDisplay1->TextOut(str2, TextX2, TextY2, TEXTCOLOR);
	if(!grabber.IsLive(MUX2))	m_pDisplay1->UpdateDisplay();
}


void CVisionCmds::CaliperLineWidth(int nCam, int nEdgeDir)
{
	// Seo H. Son: EdgeDir과 width를 return하는 방식은 미결	

	if(grabber.IsLive(nCam)) grabber.Grab(nCam);
	
	LPBYTE fm=(LPBYTE)grabber.GetImagePtr(nCam);
	int nResult;
	double rx[2], ry[2];
	nResult=m_pVision->FindWidth1((long)fm, m_pTracker->m_rect.left, m_pTracker->m_rect.top, m_pTracker->m_rect.right,m_pTracker->m_rect.bottom, 
								640, m_nColor, nEdgeDir, m_nMinDiff, m_nPixelOffset, 0, rx, ry, &m_dblWidth, &m_dblWidthShort);

	if(nResult==0)
	{		
		switch(nCam)
		{
		case MUX1:
			m_pDisplay0->DrawCrossLine((long)rx[0],(long)ry[0],10,RGB(0,0,255));
			m_pDisplay0->DrawCrossLine((long)rx[1],(long)ry[1],10,RGB(0,0,255));
			if(!grabber.IsLive(MUX1)) m_pDisplay0->UpdateDisplay();
			break;
		case MUX2:
			m_pDisplay1->DrawCrossLine((long)rx[0],(long)ry[0],10,RGB(0,0,255));
			m_pDisplay1->DrawCrossLine((long)rx[1],(long)ry[1],10,RGB(0,0,255));
			if(!grabber.IsLive(MUX2)) m_pDisplay1->UpdateDisplay();
			break;
		}
	}
	grabber.Live(nCam);
}

void CVisionCmds::CloseDisplay(int nCam)
{
	if(grabber.IsLive(nCam)) grabber.Grab((long)nCam);

	switch(nCam)
	{
	case MUX1:
		grabber.SetDisplay(MUX1, NULL);
		break;
	case MUX2:
		grabber.SetDisplay(MUX2, NULL);
		break;
	}
}

/*
void CVisionCmds::SetFocus(int nCam)
{
	//@TRACKER----------------------------------
	m_pTracker->m_nStyle=CRectTracker::solidLine;
	m_pTracker->m_nHandleSize=5;
	m_pTracker->m_rect.SetRect(100,100,200,200);
	//------------------------------------------
	switch (nCam)
	{
	case MUX1:
		m_pDisplay0->DrawRect(m_pTracker->m_rect.left, m_pTracker->m_rect.top,m_pTracker->m_rect.right,m_pTracker->m_rect.bottom, RGB(255,0,0));
		break;
	case MUX2:
		m_pDisplay1->DrawRect(m_pTracker->m_rect.left, m_pTracker->m_rect.top,m_pTracker->m_rect.right,m_pTracker->m_rect.bottom, RGB(255,0,0));
		break;
	}
}
*/
void CVisionCmds::ClearOverlay(int nCam)
{
	switch(nCam)
	{
	case MUX1:
		m_pDisplay0->ClearOvr();
		m_pDisplay0->Invalidate();
		break;
	case MUX2:
		m_pDisplay1->ClearOvr();
		m_pDisplay1->Invalidate();
		break;
	}

}

void CVisionCmds::ClearOverlay()
{
	ClearOverlay(MUX1);
	ClearOverlay(MUX2);
}
/*
void CVisionCmds::SetFocus(int nCam, int cx, int cy, int height, int width)
{
	
	int left = cx-(width)/2;
	int right = cx+(width)/2;
	int top = cy-(height)/2;
	int bottom = cy+(height)/2;
	
}
*/
BOOL CVisionCmds::CaliperLineWidth(int nCam, int nEdgeDir, int cx, int cy, int height, int width, double dAngle, int m_nDiff, int m_nPixelOff, BOOL bIsShow)
{
	//if(grabber.IsLive(nCam)) grabber.Grab(nCam);

	grabber.Grab(nCam);

//	if(0!=dAngle)	RotateImage(nCam, 320, 240, dAngle); // 070901 shson, Stage 회전되었으므로 RotateImage 할 필요 없음

	LPBYTE fm;

	
	Sleep(100);
	//grabber.Grab(nCam);

	fm=(LPBYTE)grabber.GetImagePtr(nCam);

	int left = cx-(width)/2;
	int right = cx+(width)/2;
	int top = cy-(height)/2;
	int bottom = cy+(height)/2;
	
	int nDir;
	double CELL_WIDTH;

/*	if(1==nEdgeDir || 2==nEdgeDir)
	{
		CELL_WIDTH = CELL_WIDTH_V;
		nDir = 0;
	}
	else if(3==nEdgeDir || 4==nEdgeDir)
	{
		CELL_WIDTH = CELL_WIDTH_H;
		nDir = 1;
	}
	else	nDir = 0;
*/ //P7

	if(3==nEdgeDir || 4==nEdgeDir)
	{
		CELL_WIDTH = CELL_WIDTH_V;
		nDir = 0;
	}
	else if(1==nEdgeDir || 2==nEdgeDir)
	{
		CELL_WIDTH = CELL_WIDTH_H;
		nDir = 1;
	}
	else	nDir = 0;
	
	int nResult;
	double rx[2], ry[2];	

	nResult=m_pVision->FindWidth1((long)fm, left, top, right,bottom, 640, m_nColor, nDir, m_nDiff, m_nPixelOff, 0, rx, ry, &m_dblWidth, &m_dblWidthShort);

	if(nResult==0)
	{		
		m_dblWidth*=CELL_WIDTH;
		m_dblWidthShort*=CELL_WIDTH;
		//m_dblWidth = m_dblWidthShort = 0;
		
		fabs(rx[0] - rx[1]) > 1000.0 ? rx[0] = 0, rx[1] = 0 : 1; // 선폭의 측정번위를 벗어나면 강제로 초기화 시킴.
		fabs(ry[0] - ry[1]) > 1000.0 ? ry[0] = 0, ry[1] = 0 : 1; // 선폭의 측정번위를 벗어나면 강제로 초기화 시킴.

		rx[0] == 0 && rx[1] == 0 ? m_dblWidth = 0, m_dblWidthShort=0: 1;
		ry[0] == 0 && ry[1] == 0 ? m_dblWidth = 0, m_dblWidthShort=0: 1;

		m_dbWidthAbsPosX = ((rx[0] + rx[1])/2)*CELL_WIDTH_H; //[um]
		m_dbWidthAbsPosY = ((ry[0] + ry[1])/2)*CELL_WIDTH_V; //[um]

		if(bIsShow)
		{
			//grabber.Grab(nCam);

			switch(nCam)
			{
			case MUX1:
				m_pDisplay0->DrawCrossLine((long)rx[0],(long)ry[0],10,RGB(0,0,255));
				m_pDisplay0->DrawCrossLine((long)rx[1],(long)ry[1],10,RGB(0,0,255));
				m_pDisplay0->DrawCrossLine(320,240,LINEWIDTH,LINECOLOR);
				m_pDisplay0->DrawRect(left,top,right,bottom,RGB(255,0,0));
				m_pDisplay0->UpdateDisplay();
				//if(!grabber.IsLive(MUX1)) m_pDisplay0->UpdateDisplay();
				break;
			case MUX2:
				m_pDisplay1->DrawCrossLine((long)rx[0],(long)ry[0],10,RGB(0,0,255));
				m_pDisplay1->DrawCrossLine((long)rx[1],(long)ry[1],10,RGB(0,0,255));
				m_pDisplay1->DrawCrossLine(320,240,LINEWIDTH,LINECOLOR);
				m_pDisplay1->DrawRect(left,top,right,bottom,RGB(255,0,0));
				m_pDisplay1->UpdateDisplay();
				//if(!grabber.IsLive(MUX2)) m_pDisplay1->UpdateDisplay();
				break;
			}
		}
		return TRUE;
	}
	else
	{
		if(bIsShow)
		{
			switch(nCam)
			{
			case MUX1:
				m_pDisplay0->ClearOvr();
				break;
			case MUX2:
				m_pDisplay1->ClearOvr();
				break;
			}
		}
		return FALSE;
	}
	//grabber.Live(nCam);
}

void CVisionCmds::SaveImage(int nCam)
{
	//CFileDialog dlg(FALSE, "bmp", "*.bmp");
	CFileDialog dlg( FALSE, _T("BMP"),_T("*.bmp"),
                OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
                _T("BMP Files (*.BMP)|*.bmp|") );
	if(dlg.DoModal() !=IDOK) return;
	grabber.SaveBMP(nCam, dlg.GetPathName());	
}

void CVisionCmds::ImagePan(int nCam)
{
	switch(nCam)
	{
	case MUX1:
		m_pDisplay0->ImagePan(m_nPanX, m_nPanY);
		break;
	case MUX2:
		m_pDisplay1->ImagePan(m_nPanX, m_nPanY);
		break;
	}
}

void CVisionCmds::ImagePan(int nCam, int nPanX, int nPanY)
{
	switch(nCam)
	{
	case MUX1:
		m_pDisplay0->ImagePan(nPanX, nPanY);
		break;
	case MUX2:
		m_pDisplay1->ImagePan(nPanX, nPanY);
		break;
	}
}

void CVisionCmds::CreatePMDlg(int nCam)
{
	CreatePM(nCam);

	switch(nCam)
	{
	case MUX1:
		m_pMark->PMDialog(0, (long)m_pDisplay0->m_hWnd, VISION_WIDTH, VISION_HEIGHT, 0, 0, m_dblZoom);
		break;
	case MUX2:
		m_pMark->PMDialog(1, (long)m_pDisplay1->m_hWnd, VISION_WIDTH, VISION_HEIGHT, 0, 0, m_dblZoom);
		break;
	}
}

void CVisionCmds::CreatePM(int nCam)
{
	m_pMark->PMCreate((long)m_nPatternIDs);
//	grabber.Grab(nCam);
	LPBYTE fm = (LPBYTE)grabber.GetImagePtr(nCam);
	m_pMark->PMSetSearchFrame((long)fm);
	m_pMark->PMSetRedraw(true);
}

void CVisionCmds::CreatePM()
{
	// Seo H. Son: file로부터 pattern 설정
}

void CVisionCmds::DrawRect(int nCam, long left, long top, long right, long bottom, unsigned long color)
{
	switch(nCam)
	{
	case MUX1:
		m_pDisplay0->ClearOvr();
		m_pDisplay0->DrawRect(left,top,right,bottom,color);
		if(!Live(MUX1)) m_pDisplay0->UpdateDisplay();
		break;
	case MUX2:
		m_pDisplay1->ClearOvr();
		m_pDisplay1->DrawRect(left,top,right,bottom,color);
		if(!Live(MUX2)) m_pDisplay1->UpdateDisplay();
		break;
	}
}

void CVisionCmds::RegisterAlignMark(int nCam, int left, int top, int right, int bottom)
{
	int m_nViewMode;
	switch(nCam)
	{
	case MUX1: case MUX2:
		m_nViewMode = nCam;
		break;
	case 2: // Scribe Mark
		m_nViewMode = MUX1;
		break;
	case 3: // Nozzle 
		m_nViewMode = MUX1;
		break;
	case 8: case 9:
		m_nViewMode = nCam-8;
		break;
	}
	if(grabber.IsLive(m_nViewMode)) grabber.Grab(m_nViewMode);

	LPBYTE fm = (LPBYTE)grabber.GetImagePtr(m_nViewMode);

	m_pMark->PMSetRedraw(true);	

	m_pMark->PMSetPatternId(nCam);
	m_pMark->PMRegister((long)fm,left,top,right,bottom,VISION_WIDTH);
	m_pDisplay0->DrawRect(left,top,right,bottom,RGB(255,0,0));
}

void CVisionCmds::RegisterAlignMark(CString str)
{
	grabber.SetDisplay(MUX1, m_pDisplay0->GetDisplayID());
	grabber.SetDisplay(MUX2, m_pDisplay1->GetDisplayID());
	
/*	if(m_strKindOfMachine=="Seal")
	{
		grabber.LoadBMP(MUX1, "D:\\TOP\\CT_2COL\\CT_2COL_SEALDATA\\"+str+"Align1.bmp");
		grabber.LoadBMP(MUX2, "D:\\TOP\\CT_2COL\\CT_2COL_SEALDATA\\"+str+"Align2.bmp");
	}
	else
	{
		grabber.LoadBMP(MUX1, "D:\\TOP\\CT_2COL\\CT_2COL_AGDATA\\"+str+"Align1.bmp");
		grabber.LoadBMP(MUX2, "D:\\TOP\\CT_2COL\\CT_2COL_AGDATA\\"+str+"Align2.bmp");
	}*/
}

BOOL CVisionCmds::FindAlignMark(int nID, int nScore, BOOL bIsShow)
{
/*	//m_pMark->PMClearWindow();
	//m_pMark->PMSetPatternId((long)nID);
	//m_pMark->PMSearch();

	//ClearOverlay(nID);

	int m_nViewMode;

	switch(nID)
	{
	case MUX1: case MUX2:
		m_nViewMode = nID;
		break;
	case 2: // Scribe mark
		m_nViewMode = MUX1;
		break;
	case 3: // Nozzle
		m_nViewMode = MUX1;
		break;
	case 8: case 9:
		m_nViewMode = nID-8;
		break;
	}

	m_pMark->PMSetPatternId((long)nID);
	LPBYTE fm = (LPBYTE)grabber.GetImagePtr(m_nViewMode);
	m_pMark->PMSetSearchFrame((long)fm);
	m_pMark->PMSetRedraw(true);
	m_pMark->PMSetSearchArea(0,0,639,479,VISION_WIDTH);

	m_dblMarkX = m_dblMarkY = m_dblAngle = 0;
	m_lScore = 0;	
	
	//if(m_pMark->PMSearchId((long)nID))
	if(m_pMark->PMSearch())
	{
		//m_pMark->PMClearWindow();
		//if(bIsShow)
		//{
		//	ClearOverlay(m_nViewMode);
		//	m_pMark->PMSetDrawCross(TRUE);
		//	m_pMark->PMSetDrawRectangle(TRUE);		
		//	switch(m_nViewMode)
		//	{
		//	case MUX1:
		//		m_pDisplay0->UpdateDisplay();
		//		break;
		//	case MUX2:
		//		m_pDisplay1->UpdateDisplay();
		//		break;
		//	case 2:
		//		m_pDisplay0->UpdateDisplay();
		//		break;
		//	}
		//}
		m_pMark->PMGetReturn(0, &m_dblMarkX, &m_dblMarkY, &m_lScore, &m_dblAngle);
		if(bIsShow)
		{
			ClearOverlay(m_nViewMode);
			m_pMark->PMSetDrawCross(TRUE);
			m_pMark->PMSetDrawRectangle(TRUE);		
			switch(m_nViewMode)
			{
			case MUX1:
				m_pDisplay0->DrawCrossLine((int)m_dblMarkX, (int)m_dblMarkY, 50, LINECOLOR);	// 0709091 shson, Mark 중심에 십자 표시 
				m_pDisplay0->UpdateDisplay();
				break;
			case MUX2:
				m_pDisplay1->DrawCrossLine((int)m_dblMarkX, (int)m_dblMarkY, 50, LINECOLOR);	// 070901 shson, Mark 중심에 십자 표시
				m_pDisplay1->UpdateDisplay();
				break;
			case 2:
				m_pDisplay0->UpdateDisplay();
				break;
			}
		}
		if(m_lScore<nScore)
		{
			m_dblMarkX = m_dblMarkY = m_dblAngle = 0.0;
			m_lScore = 0;
			return FALSE;
		}
		else
		{
			m_dblMarkX = (m_dblMarkX - VISION_WIDTH/2) + 0.5;
			m_dblMarkY = (VISION_HEIGHT/2 - m_dblMarkY) + 0.5;
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}
*/
	//m_pMark->PMClearWindow();
	//m_pMark->PMSetPatternId((long)nID);
	//m_pMark->PMSearch();

	//ClearOverlay(nID);

	int m_nViewMode;

	BOOL bMarkFind = FALSE;
//2009.05.20 by tskim
	m_bFindOK= FALSE;

	switch(nID)
	{
	case MUX1: case MUX2:
		m_nViewMode = nID;
		break;
	}
//#if _VISION
	//m_pMark->PMSetPatternId((long)nID);
	if(MUX1==nID || MUX2==nID)
	{
		(MUX1==nID) ? m_pMark->PMSetPatternId((long)m_nPatternID1) : 1;
		(MUX2==nID) ? m_pMark->PMSetPatternId((long)m_nPatternID2) : 1;
	}
	else
	{
		m_pMark->PMSetPatternId((long)nID);
	}

	LPBYTE fm = (LPBYTE)grabber.GetImagePtr(m_nViewMode);
	m_pMark->PMSetSearchFrame((long)fm);
	m_pMark->PMSetRedraw(true);
	m_pMark->PMSetSearchArea(0,0,639,479,VISION_WIDTH);

	m_dblMarkX = m_dblMarkY = m_dblAngle = 0;
	m_lScore = 0;	

	for(int i=0; i<4; i++)
	{
		//if(m_pMark->PMSearchId((long)nID))
		if(m_pMark->PMSearch())
		{
			//m_pMark->PMClearWindow();
			if(bIsShow)
			{
				ClearOverlay(m_nViewMode);
				m_pMark->PMSetDrawCross(TRUE);
				m_pMark->PMSetDrawRectangle(TRUE);		
				switch(m_nViewMode)
				{
				case MUX1:
					m_pDisplay0->UpdateDisplay();
					break;
				case MUX2:
					m_pDisplay1->UpdateDisplay();
					break;
				case 2:
					m_pDisplay0->UpdateDisplay();
					break;
				}
			}
			m_pMark->PMGetReturn(0, &m_dblMarkX, &m_dblMarkY, &m_lScore, &m_dblAngle);
			m_lScoreOld = m_lScore;
//2009.08.05 by tskim 
//			if(m_lScore == 0) return FALSE;
			if(m_lScore<nScore)
			{
				m_dblMarkX = m_dblMarkY = m_dblAngle = 0.0;
				m_lScore = 0;
				int nPatternIDTemp = 0;
				m_nPatternID1_Old = m_nPatternID1;
				m_nPatternID2_Old = m_nPatternID2;
				if(MUX1 == nID)
				{
					switch(m_nPatternID1)
					{
					case MUX1: nPatternIDTemp = 2; break;
					case 2: nPatternIDTemp = 4; break;
					case 4: nPatternIDTemp = 6; break;
					case 6: nPatternIDTemp = 0; break;
					}			
					m_nPatternID1 = nPatternIDTemp;
					RetryCount1++;
					if(RetryCount1 > 4)
					{
						//return FALSE;
						m_bRetryOK = FALSE;
					}
					else
					{
						m_bRetryOK = TRUE;
					}
				}
				else if(MUX2 == nID)
				{
					switch(m_nPatternID2)
					{
					case MUX2: nPatternIDTemp = 3; break;
					case 3: nPatternIDTemp = 5; break;
					case 5: nPatternIDTemp = 7; break;
					case 7: nPatternIDTemp = 1; break;
					}			
					m_nPatternID2 = nPatternIDTemp;
					RetryCount2++;
					if(RetryCount2 > 4)
					{
						//return FALSE;
						m_bRetryOK = FALSE;
					}
					else
					{
						m_bRetryOK = TRUE;
					}
				}
				//return FALSE;
//2009.05.20 by tskim
				return TRUE;
			}
			else
			{
				m_dblMarkX = (VISION_WIDTH/2 - m_dblMarkX)*CELL_WIDTH_V; //+ 0.5;
				m_dblMarkY = (m_dblMarkY - VISION_HEIGHT/2)*CELL_WIDTH_H; //+ 0.5;
				m_bRetryOK = FALSE;
				m_bFindOK = TRUE;
				return TRUE;
			}
		}
		else
		{
			return FALSE;
		}
	}
//#endif
	return bMarkFind;
}

BOOL CVisionCmds::FindNozzleMark(int nID, int nScore, BOOL bIsShow)
{
	m_pMark->PMSetPatternId((long)_NOZZLE_ALIGN_MARK);
	LPBYTE fm = (LPBYTE)grabber.GetImagePtr(nID);
	m_pMark->PMSetSearchFrame((long)fm);
	m_pMark->PMSetRedraw(true);
	m_pMark->PMSetSearchArea(0,0,639,479,VISION_WIDTH);

	m_dblMarkX = m_dblMarkY = m_dblAngle = 0;
	m_lScore = 0;	
	
	if(m_pMark->PMSearch())
	{
		if(bIsShow)
		{
			ClearOverlay(nID);
			m_pMark->PMSetDrawCross(TRUE);
			m_pMark->PMSetDrawRectangle(TRUE);		
			switch(nID)
			{
			case MUX1:
				m_pDisplay0->UpdateDisplay();
				break;
			case MUX2:
				m_pDisplay1->UpdateDisplay();
				break;
			}
		}
		m_pMark->PMGetReturn(0, &m_dblMarkX, &m_dblMarkY, &m_lScore, &m_dblAngle);
		if(m_lScore<nScore)
		{
			m_dblMarkX = m_dblMarkY = m_dblAngle = 0.0;
			m_lScore = 0;
			return FALSE;
		}
		else
		{
			m_dblMarkX = (m_dblMarkX - VISION_WIDTH/2) + 0.5;
			m_dblMarkY = (VISION_HEIGHT/2 - m_dblMarkY) + 0.5;
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}

}

void CVisionCmds::RotateImage(int nCam, double dCx, double dCy, double dAngle)
{
	/*
		2004.12.01 By Seo H. Son
		영상을 회전시키는 함수: dCx, dCy을 중심으로 dAngle만큼 회전
		시계방향이 +방향
	*/
	LPBYTE fm = (LPBYTE)grabber.GetImagePtr(nCam);
	LPBYTE fmD = new BYTE[640*480];

	//double pi = 4.0 * atan(1);
	//double dAngle = dRadian*180/pi;
	
	m_pVision->ImageRotate((long)fm, (long)fmD, 0, 0, 639, 479, 640, dCx, dCy, dAngle);

	grabber.PutImage(nCam, (long)fmD);

	delete[] fmD;
}

BOOL CVisionCmds::IsLive(int nCam)
{
	if(2!=nCam)
	{
		return grabber.IsLive(nCam);
	}
	else
	{
		return grabber.IsLive(MUX1)&grabber.IsLive(MUX2);
	}
}

void CVisionCmds::GridDisplay(int nCam)
{

	int x1, y1, interval, lColor;
	
	for(int i=1;i<8;i++)
	{
		x1 = (50*i)/CELL_WIDTH_H;
		y1 = (50*i)/CELL_WIDTH_V;

		if(i==3 || i==6)
		{
			interval = GRID*3;
			lColor = LINECOLOR;
		}
		else
		{			
			interval = GRID;
			lColor = GRIDCOLOR;
		}

		if(MUX1==nCam)
		{
			m_pDisplay0->DrawLine(320-x1, 240-interval, 320-x1, 240+interval, lColor);
			m_pDisplay0->DrawLine(320+x1, 240-interval, 320+x1, 240+interval, lColor);
			m_pDisplay0->DrawLine(320-interval, 240-y1, 320+interval, 240-y1, lColor);
			m_pDisplay0->DrawLine(320-interval, 240+y1, 320+interval, 240+y1, lColor);
		}
		else if(MUX2==nCam)
		{
			m_pDisplay1->DrawLine(320-x1, 240-interval, 320-x1, 240+interval, lColor);
			m_pDisplay1->DrawLine(320+x1, 240-interval, 320+x1, 240+interval, lColor);
			m_pDisplay1->DrawLine(320-interval, 240-y1, 320+interval, 240-y1, lColor);
			m_pDisplay1->DrawLine(320-interval, 240+y1, 320+interval, 240+y1, lColor);
		}
		else
		{
			m_pDisplay0->DrawLine(320-x1, 240-interval, 320-x1, 240+interval, lColor);
			m_pDisplay0->DrawLine(320+x1, 240-interval, 320+x1, 240+interval, lColor);
			m_pDisplay0->DrawLine(320-interval, 240-y1, 320+interval, 240-y1, lColor);
			m_pDisplay0->DrawLine(320-interval, 240+y1, 320+interval, 240+y1, lColor);

			m_pDisplay1->DrawLine(320-x1, 240-interval, 320-x1, 240+interval, lColor);
			m_pDisplay1->DrawLine(320+x1, 240-interval, 320+x1, 240+interval, lColor);
			m_pDisplay1->DrawLine(320-interval, 240-y1, 320+interval, 240-y1, lColor);
			m_pDisplay1->DrawLine(320-interval, 240+y1, 320+interval, 240+y1, lColor);			
		}
	}
	switch(nCam)
	{
	case MUX1:
		if(!grabber.IsLive(MUX1))	m_pDisplay0->UpdateDisplay();
		break;
	case MUX2:
		if(!grabber.IsLive(MUX2))	m_pDisplay1->UpdateDisplay();
		break;
	default:
		if(!grabber.IsLive(MUX1))	m_pDisplay0->UpdateDisplay();
		if(!grabber.IsLive(MUX2))	m_pDisplay1->UpdateDisplay();
		break;
	}
}

void CVisionCmds::GridDisplay(int nCam, int nX, int nY)
{

	int x1, y1, interval, lColor;
	
	for(int i=1;i<8;i++)
	{
		x1 = (50*i)/CELL_WIDTH_H;
		y1 = (50*i)/CELL_WIDTH_V;

		if(i==3 || i==6)
		{
			interval = GRID*3;
			lColor = LINECOLOR;
		}
		else
		{			
			interval = GRID;
			lColor = GRIDCOLOR;
		}

		if(MUX1==nCam)
		{
			m_pDisplay0->DrawLine(nX-x1, nY-interval, nX-x1, nY+interval, lColor);
			m_pDisplay0->DrawLine(nX+x1, nY-interval, nX+x1, nY+interval, lColor);
			m_pDisplay0->DrawLine(nX-interval, nY-y1, nX+interval, nY-y1, lColor);
			m_pDisplay0->DrawLine(nX-interval, nY+y1, nX+interval, nY+y1, lColor);
		}
		else if(MUX2==nCam)
		{
			m_pDisplay1->DrawLine(nX-x1, nY-interval, nX-x1, nY+interval, lColor);
			m_pDisplay1->DrawLine(nX+x1, nY-interval, nX+x1, nY+interval, lColor);
			m_pDisplay1->DrawLine(nX-interval, nY-y1, nX+interval, nY-y1, lColor);
			m_pDisplay1->DrawLine(nX-interval, nY+y1, nX+interval, nY+y1, lColor);
		}
		else
		{
			m_pDisplay0->DrawLine(nX-x1, nY-interval, nX-x1, nY+interval, lColor);
			m_pDisplay0->DrawLine(nX+x1, nY-interval, nX+x1, nY+interval, lColor);
			m_pDisplay0->DrawLine(nX-interval, nY-y1, nX+interval, nY-y1, lColor);
			m_pDisplay0->DrawLine(nX-interval, nY+y1, nX+interval, nY+y1, lColor);

			m_pDisplay1->DrawLine(nX-x1, nY-interval, nX-x1, nY+interval, lColor);
			m_pDisplay1->DrawLine(nX+x1, nY-interval, nX+x1, nY+interval, lColor);
			m_pDisplay1->DrawLine(nX-interval, nY-y1, nX+interval, nY-y1, lColor);
			m_pDisplay1->DrawLine(nX-interval, nY+y1, nX+interval, nY+y1, lColor);			
		}
	}
	switch(nCam)
	{
	case MUX1:
		if(!grabber.IsLive(MUX1))	m_pDisplay0->UpdateDisplay();
		break;
	case MUX2:
		if(!grabber.IsLive(MUX2))	m_pDisplay1->UpdateDisplay();
		break;
	default:
		if(!grabber.IsLive(MUX1))	m_pDisplay0->UpdateDisplay();
		if(!grabber.IsLive(MUX2))	m_pDisplay1->UpdateDisplay();
		break;
	}
}

void CVisionCmds::CircleDisplay(int nCam, int Radius)
{
	switch(nCam)
	{
	case MUX1:
		//m_pDisplay0->DeleteROI("Nozzle#1");
		//m_pDisplay0->AddROI("", VISION_WIDTH/2 - Radius/CELL_WIDTH_H, VISION_HEIGHT/2 - Radius/CELL_WIDTH_V, VISION_WIDTH/2 + Radius/CELL_WIDTH_H, VISION_HEIGHT/2 + Radius/CELL_WIDTH_V, LINECOLOR);
		m_pDisplay0->DrawCircle(VISION_WIDTH/2, VISION_HEIGHT/2, Radius/CELL_WIDTH_H, LINECOLOR);
		break;
	case MUX2:
		//m_pDisplay1->AddROI("", VISION_WIDTH/2 - Radius/CELL_WIDTH_H, VISION_HEIGHT/2 - Radius/CELL_WIDTH_V, VISION_WIDTH/2 + Radius/CELL_WIDTH_H, VISION_HEIGHT/2 + Radius/CELL_WIDTH_V, LINECOLOR);
		m_pDisplay1->DrawCircle(VISION_WIDTH/2, VISION_HEIGHT/2, Radius/CELL_WIDTH_H, LINECOLOR);
		break;
	}

	switch(nCam)
	{
	case MUX1:
		if(!grabber.IsLive(MUX1))	m_pDisplay0->UpdateDisplay();
		break;
	case MUX2:
		if(!grabber.IsLive(MUX2))	m_pDisplay1->UpdateDisplay();
		break;
	}
}

void CVisionCmds::CircleDisplay(int nCam, int nX, int nY, int Radius)
{
	switch(nCam)
	{
	case MUX1:
		m_pDisplay0->DrawCircle(nX, nY, Radius/CELL_WIDTH_H, LINECOLOR);
		break;
	case MUX2:
		m_pDisplay1->DrawCircle(nX, nY, Radius/CELL_WIDTH_H, LINECOLOR);
		break;
	}

	switch(nCam)
	{
	case MUX1:
		if(!grabber.IsLive(MUX1))	m_pDisplay0->UpdateDisplay();
		break;
	case MUX2:
		if(!grabber.IsLive(MUX2))	m_pDisplay1->UpdateDisplay();
		break;
	}
}

void CVisionCmds::ImageZoom(int nCam, int nZoom)
{
	switch(nCam)
	{
	case MUX1:
		m_pDisplay0->ImageZoom(nZoom, nZoom);
		break;
	case MUX2:
		m_pDisplay1->ImageZoom(nZoom, nZoom);
		break;
	}
}

void CVisionCmds::CrossDisplay(int nCam, int x, int y, long Size, unsigned long color)
{
	switch(nCam)
	{
	case MUX1:
		ClearOverlay(MUX1);
		m_pDisplay0->DrawCrossLine(x, y, Size, color);
		if(!grabber.IsLive(MUX1))	m_pDisplay0->UpdateDisplay();
		break;
	case MUX2:
		ClearOverlay(MUX2);
		m_pDisplay1->DrawCrossLine(x, y, Size, color);
		if(!grabber.IsLive(MUX2))	m_pDisplay1->UpdateDisplay();
		break;
	}
}

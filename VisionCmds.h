//{{AFX_INCLUDES()
#include "ligeledisplay.h"
//}}AFX_INCLUDES

// VisionCmds.h: interface for the CVisionCmds class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VISIONCMDS_H__31250984_23ED_48F0_A915_317ABD534B04__INCLUDED_)
#define AFX_VISIONCMDS_H__31250984_23ED_48F0_A915_317ABD534B04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "kovision.h"
#include "pmocx.h"
#include "math.h"

class CVisionCmds  
{
public:
	BOOL Live(int nCam);
	CVisionCmds();
	virtual ~CVisionCmds();

public:	
	BOOL m_bAlignComplete;
	BOOL m_bFindOK;
	int m_nPatternID1_Old;
	int m_nPatternID2_Old;
	BOOL m_bRetryOK;
	int RetryCount2;
	int RetryCount1;
	long m_lScoreOld;
	int m_nPatternID2;
	int m_nPatternID1;
	BOOL m_bMarkFind1;
	BOOL m_bMarkFind2;
	BOOL m_bMarkAlingOK;
	void CrossDisplay(int nCam, int x, int y,long Size,unsigned long color);
	void ImageZoom(int nCam, int nZoom);
	void CircleDisplay(int nCam, int nX, int nY, int Radius);
	void CircleDisplay(int nCam, int Radius);
	void GridDisplay(int nCam);
	void GridDisplay(int nCam, int nX, int nY);
	BOOL IsLive(int nCam);
	void RotateImage(int nCam, double dCx, double dCy, double dAngle);
	void CrossDisplay(CString str1, CString str2, int x1, int y1, int x2, int y2);
	void ClearOverlay(int nCam);
	double m_dblAngle;
	long m_lScore;
	double m_dblMarkY;
	double m_dblMarkX;
	BOOL FindAlignMark(int nID, int nScore, BOOL bIsShow);
	BOOL FindNozzleMark(int nID, int nScore, BOOL bIsShow);
	void RegisterAlignMark(CString str);
	void RegisterAlignMark(int nCam, int left, int top, int right, int bottom);
	int m_nPatternIDs;
	void DrawRect(int nCam, long left, long top, long right, long bottom, unsigned long color);
	double m_dblZoom;
	void CreatePM();
	void CreatePM(int nCam);
	void CreatePMDlg(int nCam);
	void Grab(int nCam);
	void ImagePan(int nCam);
	void ImagePan(int nCam, int nPanX, int nPanY);
	void SaveImage(int nCam);
	BOOL CaliperLineWidth(int nCam, int nEdgeDir, int cx, int cy, int height, int width, double dAngle, int m_nDiff, int m_nPixelOff, BOOL bIsShow);
//	void SetFocus(int nCam, int cx, int cy, int height, int width);
	void ClearOverlay();
	double m_dblWidthShort;
	double m_dblWidth;
	double m_dbWidthAbsPosX;
	double m_dbWidthAbsPosY;
	//void SetFocus(int nCam);
	CKOVISION		*m_pVision;
	CPMOCX			*m_pMark;
	CRectTracker    m_cTracker; //@TRACKER
	CRectTracker	*m_pTracker;
	int m_nPanY;
	int m_nPanX;
	void CloseDisplay(int nCam);
	BOOL BOTH;
	void CaliperLineWidth(int nCam, int nEdgeDir);
	void SetDisplay(int nCam, long pDisplay);
	void SetChannel(int nCam);
	void CrossDisplay(CString str, int x, int y);
	void CrossDisplay(int nCam, CString str, int x, int y);
	void SetDisplay(int nCam);

	//{{AFX_DATA(CVisionCmds)	
	int		m_nColor;
	int		m_nEdgeDir;
	int		m_nMinDiff;
	int		m_nPixelOffset;
	CString	m_sResultPos;
	CString	m_sResutlGap;
	BOOL	m_bShow;
	CLigEleDisplay	*m_pDisplay0;
	CLigEleDisplay	*m_pDisplay1;
	//}}AFX_DATA

protected:
	int nChannel;

};

#endif // !defined(AFX_VISIONCMDS_H__31250984_23ED_48F0_A915_317ABD534B04__INCLUDED_)

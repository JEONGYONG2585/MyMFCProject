// DrawPattern.h: interface for the CDrawPattern class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWPATTERN_H__CFD93808_1759_42CD_A4B0_0A9894AF7018__INCLUDED_)
#define AFX_DRAWPATTERN_H__CFD93808_1759_42CD_A4B0_0A9894AF7018__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "P8CA_LcDispDoc.h"
#include "DropPoint.h"	// Added by ClassView
#include "LinePoint.h"	// Added by ClassView


// 이 클래스는 해당 패턴의 모든 노즐을 다 그린다.
class CDrawPattern
{
public:
	int m_nPosOper;
	CRect m_BigRect;
	CFont m_newFont , *m_pOldFont;
	CFont m_newLogFont , *m_pOldLogFont;// To display panel id..
//
	BOOL m_bIsBig;
	BOOL m_bIsDirectSetting;
	BOOL m_bIsDisplayText;
	BOOL m_bIsDisplayText2;

	int m_nDispenseMode;
	int m_nCurSelDropNum;
	int m_nCurSelHeadNum;
	int m_nCurSelPointNum;

	double m_dDefaultZoom;	// 맨처음 1배줌 일때의 줌 (기본 줌)
	double m_dZoom;			// 현재 사용중인 줌

	CPoint m_pHomePoint;
	CPoint m_pOrgPoint;
//
	CDropPoint *m_pDrop;
	CLinePoint *m_pLine;

	CP8CA_LcDispDoc *GetDocument();
//
	BOOL Draw();
	BOOL CheckSelect(double dX, double dY);
	BOOL CheckSelect_2Nozzle(double dX, double dY);
	BOOL CheckSelect_NType(double dX, double dY);
	void SetPattern(CDC *dc, long nPatNum);
	
	
	// 패턴 번호
	long m_nPatternNumber;

	// nozzle , column offset 가상의 축으로부터..
	double m_dNozzleOffset[MAX_NOZZLE];

	BOOL m_bOnNozzle[MAX_NOZZLE];
	
	// On, off일때의 색상 설정 관련
	CPen m_OnPen[MAX_NOZZLE];
	CPen m_LinePen[MAX_NOZZLE];
	CPen m_OffPen;
	CPen m_PosPen;
	CPen *m_pOldPen;
	CPen m_Center; // N_Type Yamary...

	CBrush m_OnBrush[MAX_NOZZLE];
	CBrush m_WhiteBrush;
	CBrush m_OffBrush;
	CBrush *m_pOldBrush;

	COLORREF m_colorOn[MAX_NOZZLE];
	COLORREF m_colorWhite;
	COLORREF m_colorOff;
	COLORREF m_Pos;//Yamary...

	CDC *pDC;

	CDrawPattern();
	virtual ~CDrawPattern();

};

#endif // !defined(AFX_DRAWPATTERN_H__CFD93808_1759_42CD_A4B0_0A9894AF7018__INCLUDED_)

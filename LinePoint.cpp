// LinePoint.cpp: implementation of the CLinePoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "LinePoint.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLinePoint::CLinePoint()
{
	m_bIsStart = FALSE;
	m_pDC = NULL;
	m_dZoom = 1;
	m_pOrgPoint = CPoint(0, 0);

}

CLinePoint::~CLinePoint()
{

}

void CLinePoint::SetDC(CDC *pDC)
{
	m_pDC = pDC;
}

void CLinePoint::Draw(double dx, double dy)
{
	// ÁÂÇ¥ Àû¿ë
	dx = (dx * m_dZoom) + m_pOrgPoint.x;
	dy = (dy * m_dZoom) + m_pOrgPoint.y;

	if(m_bIsStart)
	{
		m_pDC->MoveTo(CPoint((long)dx, (long)dy));
		m_bIsStart = FALSE;
	}
	else
	{
		m_pDC->LineTo((long)dx, (long)dy);
	}
}

// LinePoint.h: interface for the CLinePoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINEPOINT_H__5D9699B8_0CBE_41BF_81AD_D39FE940731E__INCLUDED_)
#define AFX_LINEPOINT_H__5D9699B8_0CBE_41BF_81AD_D39FE940731E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLinePoint  
{
public:
	CPoint m_pOrgPoint;
	double m_dZoom;
	void Draw(double dx, double dy);
	CDC *m_pDC;
	void SetDC(CDC *pDC);
	BOOL m_bIsStart;
	CLinePoint();
	virtual ~CLinePoint();

};

#endif // !defined(AFX_LINEPOINT_H__5D9699B8_0CBE_41BF_81AD_D39FE940731E__INCLUDED_)

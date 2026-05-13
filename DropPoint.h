// DropPoint.h: interface for the CDropPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DROPPOINT_H__415EE098_D1AA_46E6_B33C_9F1B98D044A9__INCLUDED_)
#define AFX_DROPPOINT_H__415EE098_D1AA_46E6_B33C_9F1B98D044A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDropPoint  
{
public:
	CRect m_BigRect;
	int m_nNLine;
	void Draw_N_Center(CDC *pDC, int nCell, double dx);
	void Draw_N_Drop(CDC *pDC, double dx, double dy);
	void Draw_N_Line(CDC *pDC, double dx, double dy);
	BOOL m_bOnly;
	int m_nAddDelSize;
	BOOL m_bIsDel;
	BOOL m_bIsAdd;
	CPoint m_pOrgPoint;
	void SetPoint(double dX, double dY);
	BOOL CheckSelect(double dX, double dY);
	void Draw(CDC *pDC);
	void Draw_2Nozzle(CDC *pDC);
	BOOL m_bIsDisplayText;				// drop 번호 텍스트 출력 여부
	BOOL m_bIsDisplayText2;				// drop X,Y좌표 텍스트 출력 여부
	BOOL m_bOn;							// On, Off
	double m_dZoom;						// 스케일
	double m_dRadius;					// 그려질 drop의 Radius
	double m_dPointY;					// y좌표
	double m_dPointX;					// x좌표
	double m_d2NozzleX[2];
	long m_nNumber;						// DropNumber
	int m_nAddDelNum;                  //Add/Del num
	long m_nLine; // Yamary...flexible
	int m_nPattern;
	BOOL m_b2NozzleDiplay;
	UINT m_nCount;
	
	CDropPoint();
	virtual ~CDropPoint();

};

#endif // !defined(AFX_DROPPOINT_H__415EE098_D1AA_46E6_B33C_9F1B98D044A9__INCLUDED_)

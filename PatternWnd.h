#if !defined(AFX_PATTERNWND_H__2116151A_A8BA_48F6_9BE6_6A4F587C04E0__INCLUDED_)
#define AFX_PATTERNWND_H__2116151A_A8BA_48F6_9BE6_6A4F587C04E0__INCLUDED_

#include "P8CA_LcDispDoc.h"	// Added by ClassView
#include "DrawPattern.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PatternWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPatternWnd window

class CPatternWnd : public CWnd
{
// Construction
public:
	CPatternWnd();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatternWnd)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	int m_nPosOper;
	BOOL m_bTextDisp;
	BOOL m_bRepairEditMode;
	void PartConvDropPoint(int Max, int Min, int i);
	int m_nDispPanel;
	BOOL m_bFirst;
	CRect m_BigRect;
	void OnGetRect(CRect rect);
	BOOL m_bCheck;
//
	int m_nCurSelPointNum;
	int m_nCurrentDraw;
	int m_nCurSelDropNum;
	int m_nCurSelHeadNum;
	int m_nPatternNum;
	int m_nSeq[MAX_PATTERN +1];//[MAX_PATTERN +1]; // flexible... Yamary...
	//
	BOOL m_bIsDispenseOrder;	// dispense order용 pattern wnd인지 구별 
	//
	BOOL m_bIsStart;
	BOOL m_bIsBig;
	BOOL m_bIsDirectSetting;
	BOOL m_bIsDisplayText;
	BOOL m_bIsDisplayText2;
//
	double m_dBigDefaultZoom;	// big pattern wnd의 default zoom(small wnd에서만 사용)
	double m_dBigZoom;			// big pattern wnd의 현재 zoom(small wnd에서만 사용)
	double m_dAboutBigZoom;		// big pattern wnd에 대한 small wnd의 zoom(small wnd에서만 사용)
	double m_dBigClientY;		// big pattern wnd의 실제 크기(small pattern wnd에서 사용할 것임)
	double m_dBigClientX;		// big pattern wnd의 실제 크기(small pattern wnd에서 사용할 것임)

	double m_dDefaultZoom;
	double m_dZoom;
	//
	COLORREF m_colorForeground;
	COLORREF m_colorBackground;

	CDC *m_pDC;

	CDrawPattern *m_pDrawPattern;

	void DrawForeground(CDC *pDC);
	void DrawBackground(CDC *pDC);
	void DrawPanel(CDC *pDC);

	void SetDefaultZoom();
	void ApplyDC(CDC *pDC);

	CPoint m_pOrgPoint;

	virtual ~CPatternWnd();

	// Generated message map functions
protected:
	CP8CA_LcDispDoc * GetDocument();
	//{{AFX_MSG(CPatternWnd)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PATTERNWND_H__2116151A_A8BA_48F6_9BE6_6A4F587C04E0__INCLUDED_)

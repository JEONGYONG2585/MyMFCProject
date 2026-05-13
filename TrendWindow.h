#if !defined(AFX_TRENDWINDOW_H__5710D18F_9CC7_11D1_9154_D49ADC1B04BE__INCLUDED_)
#define AFX_TRENDWINDOW_H__5710D18F_9CC7_11D1_9154_D49ADC1B04BE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TrendWindow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TrendWindow window

#define MAX_BUFFER  100

class TrendWindow : public CWnd
{
private :
	float m_data[MAX_BUFFER];
	float m_dataBK[MAX_BUFFER];
	CRect m_rect;
	CDC *mp_displayMemory;
	CBitmap *mp_bitmap, *p_old_bitmap;
	char m_timer_flag;
	double m_dHRange, m_dLRange;
	int m_nRealWidth, m_nRealHeight;

// Construction
public:
	float m_dTarget[MAX_BUFFER];
	int m_nData;
	TrendWindow();
	virtual ~TrendWindow();

	void StartGraph();
	void StopGraph();
	void DrawAxis(CDC *parm_dc);
	void DrawGrid(CDC *parm_dc);
	void DrawGraph(CDC *parm_dc);
	void DrawValue(CDC *parm_dc);
	void DrawRange(CDC *parm_dc);
	int m_nHead;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TrendWindow)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(TrendWindow)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRENDWINDOW_H__5710D18F_9CC7_11D1_9154_D49ADC1B04BE__INCLUDED_)

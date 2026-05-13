// EditGrid.h : header file
//
//
// Written by Kelly Marie Ward
// of Microsoft Developer Support.
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
///////////////////////////////////////////////////////////////////////////

#if !defined(AFX_EDITGRID_H__1F986762_FFEB_11D1_8540_00C04FAD7DBE__INCLUDED_)
#define AFX_EDITGRID_H__1F986762_FFEB_11D1_8540_00C04FAD7DBE__INCLUDED_
#include "msflexgrid.h"
#include "EditWnd.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CEditGrid window

class CEditGrid : public CMSFlexGrid
{
// Construction
public:
	CEditGrid();

// Attributes
public:

      CEditWnd m_edit;
      long m_lBorderWidth; 
      long m_lBorderHeight;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditGrid)
	//}}AFX_VIRTUAL

// Implementation
public:
	int m_nLogY;
	int m_nLogX;
	virtual ~CEditGrid();

	// Generated message map functions
protected:

	  virtual void PreSubclassWindow();
	  
      afx_msg void OnKeyPressGrid(short FAR* KeyAscii);
      afx_msg void OnDblClickGrid();
      afx_msg void OnUpdateGrid();
      afx_msg void OnBClickGrid();
 	  DECLARE_EVENTSINK_MAP()

	//{{AFX_MSG(CEditGrid)
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITGRID_H__1F986762_FFEB_11D1_8540_00C04FAD7DBE__INCLUDED_)

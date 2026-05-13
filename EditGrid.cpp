// EditGrid.cpp : implementation file
//
//
// Written by Kelly Marie Ward
// of Microsoft Developer Support.
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EditGrid.h"
#include "ctype.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditGrid

CEditGrid::CEditGrid()
{

}

CEditGrid::~CEditGrid()
{
}


BEGIN_MESSAGE_MAP(CEditGrid, CMSFlexGrid)
	//{{AFX_MSG_MAP(CEditGrid)
	ON_WM_GETDLGCODE()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CEditGrid, CMSFlexGrid)
    //{{AFX_EVENTSINK_MAP(CEditGrid)
	//}}AFX_EVENTSINK_MAP
	ON_EVENT_REFLECT(CEditGrid, -603 /* KeyPress */, OnKeyPressGrid, VTS_PI2)
	ON_EVENT_REFLECT(CEditGrid, -600 /* BClick */, OnBClickGrid, VTS_NONE)
	ON_EVENT_REFLECT(CEditGrid, -601 /* DblClick */, OnDblClickGrid, VTS_NONE)
	ON_EVENT_REFLECT(CEditGrid, 72 /* LeaveCell */, OnUpdateGrid, VTS_NONE)
END_EVENTSINK_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEditGrid message handlers

void CEditGrid::PreSubclassWindow() 
{
	//Calculate border size
	SetRow(0);
	SetCol(0);
	m_lBorderWidth = GetCellLeft();
	m_lBorderHeight = GetCellTop();

	// To convert grid rect from twips to DC units
	// We need pixels per inch
	CDC* pDC = GetDC();
	m_nLogX = pDC->GetDeviceCaps(LOGPIXELSX);
	m_nLogY = pDC->GetDeviceCaps(LOGPIXELSY);
	ReleaseDC(pDC);
	
	//Create invisible edit control
	m_edit.Create(WS_CHILD|ES_MULTILINE|ES_WANTRETURN, 
		CRect(0,0,0,0), this, GetDlgCtrlID());	
}

void CEditGrid::OnDblClickGrid() 
{
	//Check to see if edit is visible
	BOOL bVisible = ::GetWindowLong(m_edit.GetSafeHwnd(), GWL_STYLE) & WS_VISIBLE;
	if(bVisible)
		m_edit.ShowWindow(SW_HIDE);
	
	short i = 13;
	OnKeyPressGrid(&i); //Simulate a return
}
void CEditGrid::OnBClickGrid() 
{
	//Check to see if edit is visible
	BOOL bVisible = ::GetWindowLong(m_edit.GetSafeHwnd(), GWL_STYLE) & WS_VISIBLE;
	if(bVisible)
		m_edit.ShowWindow(SW_HIDE);
}

void CEditGrid::OnKeyPressGrid(short FAR* KeyAscii) 
{	
	
	ASSERT (KeyAscii != NULL);

	if( 9 == *KeyAscii ) {
		int nCols = GetCols();
		int nRows = GetRows();
		if(nRows > GetRow() + 1)
			SetRow(GetRow() + 1);
		else {
			if(nRows > GetRow() + 1)
				SetRow(GetRow() + 1);
			SetRow(1);
		}
		return;
	}		
	
	m_edit.SetWindowText(GetText());
	m_edit.m_strVal = GetText();

	if (*KeyAscii == 13)
	{
		m_edit.SetSel(0,-1);
	}
	else
	{
		char buf[] = " ";
		buf[0] = (char)*KeyAscii;
		m_edit.SetSel(-1,-1);
		m_edit.ReplaceSel(buf);
	}
		
	//Adjust for border heigth and
	//Convert from twips to screen units
	m_edit.MoveWindow(((GetCellLeft() - m_lBorderWidth) * m_nLogX)/1440, 
		((GetCellTop() - m_lBorderHeight) * m_nLogY)/1440,
		(GetCellWidth()* m_nLogX)/1440, 
		(GetCellHeight()* m_nLogY)/1440, FALSE);

	m_edit.ShowWindow(SW_SHOW);
	m_edit.SetFocus();	
}

void CEditGrid::OnUpdateGrid() 
{
	if("" != m_edit.m_strVal ) {
		CString cStr, cOld;

		cOld = GetText();
		cStr = m_edit.m_strVal;
		// 입력값의 유효성 체크 
		cStr.TrimLeft();
		cStr.TrimRight(); 
		SetText(cStr);
		m_edit.m_strVal="";
		int nCols = GetCols();
		int nRows = GetRows();
		if(nRows > GetRow() + 1)
			SetRow(GetRow() + 1);
		else {
			if(nRows > GetRow() + 1)
				SetRow(GetRow() + 1);
			else
				SetRow(1);
		    SetRow(1);
		}
	}

}

UINT CEditGrid::OnGetDlgCode() 
{	
	return DLGC_WANTALLKEYS;
}

void CEditGrid::OnSetFocus(CWnd* pOldWnd) 
{
	OnUpdateGrid();	

	CMSFlexGrid::OnSetFocus(pOldWnd);
	
}


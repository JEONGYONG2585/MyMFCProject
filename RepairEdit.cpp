// RepairEdit.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"
#include "RepairEdit.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRepairEdit dialog


CRepairEdit::CRepairEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CRepairEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRepairEdit)
	//}}AFX_DATA_INIT
	
	m_pPatternWnd = NULL;
	m_nCurrentPatternNo = 0;
	m_arDropData_Repair = new CArray<DROP_DATA, DROP_DATA>[MAX_PATTERN];
}

CRepairEdit::~CRepairEdit()
{
	if(m_pPatternWnd != NULL) 
	{
		delete m_pPatternWnd;
		m_pPatternWnd = NULL;
	}
	delete [] m_arDropData_Repair;
}

void CRepairEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRepairEdit)
	DDX_Control(pDX, IDC_SCROLLBAR_VER, m_ctlScrollVer);
	DDX_Control(pDX, IDC_SCROLLBAR_HOR, m_ctlScrollHor);
	DDX_Control(pDX, IDC_STATIC_SCREEN, m_staticScreen);
	DDX_Control(pDX, IDC_PAT_NO, m_ctrlCurPatNo);
	DDX_Control(pDX, IDC_POINT_RANGE, m_ctrlDropPointRange);
	DDX_Control(pDX, IDC_CMD_DROP_NUM_DISP, m_ctrlTextDisp);
	DDX_Control(pDX, IDC_GRID_POINT, m_GridPoint);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRepairEdit, CDialog)
	//{{AFX_MSG_MAP(CRepairEdit)
	ON_BN_CLICKED(IDC_RADIO_ZOOM1, OnRadioZoom1)
	ON_BN_CLICKED(IDC_RADIO_ZOOM2, OnRadioZoom2)
	ON_BN_CLICKED(IDC_RADIO_ZOOM4, OnRadioZoom4)
	ON_BN_CLICKED(IDC_RADIO_ZOOM7, OnRadioZoom7)
	ON_BN_CLICKED(IDC_RADIO_ZOOM10, OnRadioZoom10)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_HEAD1_CHK, OnHead1Chk)
	ON_BN_CLICKED(IDC_HEAD2_CHK, OnHead1Chk)
	ON_BN_CLICKED(IDC_HEAD3_CHK, OnHead1Chk)
	ON_BN_CLICKED(IDC_HEAD4_CHK, OnHead1Chk)
	ON_BN_CLICKED(IDC_HEAD5_CHK, OnHead1Chk)
	ON_BN_CLICKED(IDC_HEAD6_CHK, OnHead1Chk)
	ON_BN_CLICKED(IDC_HEAD7_CHK, OnHead1Chk)
	ON_BN_CLICKED(IDC_HEAD8_CHK, OnHead1Chk)
	ON_BN_CLICKED(IDC_HEAD9_CHK, OnHead1Chk)
	ON_BN_CLICKED(IDC_HEAD10_CHK, OnHead1Chk)
	ON_BN_CLICKED(IDC_HEAD11_CHK, OnHead1Chk)
	ON_BN_CLICKED(IDC_HEAD12_CHK, OnHead1Chk)
	ON_BN_CLICKED(IDC_HEAD13_CHK, OnHead1Chk)
	ON_BN_CLICKED(IDC_HEAD14_CHK, OnHead1Chk)
	ON_BN_CLICKED(IDC_HEAD15_CHK, OnHead1Chk)
	ON_BN_CLICKED(IDC_HEAD16_CHK, OnHead1Chk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRepairEdit message handlers

BOOL CRepairEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	
	m_structPatternData_Repair[m_nCurrentPatternNo-1] = pDoc->m_structPatternData[m_nCurrentPatternNo-1];

	CString str;
	str.Format("%d", m_structPatternData_Repair[m_nCurrentPatternNo-1].m_nDropDataCount);

	// Flex Grid 초기화(시작점/마지막점)
	for( int i = 0 ; i < 2 ; i++)
		m_ctrlDropPointRange.SetColWidth(i,1600);

	for( i = 0 ; i < 2 ; i++)
		m_ctrlDropPointRange.SetRowHeight(i,480);
	
	m_ctrlDropPointRange.SetTextMatrix(0,0,"시작점");
	m_ctrlDropPointRange.SetTextMatrix(1,0,"마지막점");

	if(pDoc->m_structDataEditor.m_nNzlMode == 0)
	{
		m_ctrlDropPointRange.SetTextMatrix(0,1,"1");
		m_ctrlDropPointRange.SetTextMatrix(1,1,str);
		m_nMinPoint = atoi((char *)(LPCSTR)m_ctrlDropPointRange.GetTextMatrix(0, 1));
		m_nMaxPoint = atoi((char *)(LPCSTR)m_ctrlDropPointRange.GetTextMatrix(1, 1));
	}
	else
	{
		m_nMinPoint = 0;
		m_nMaxPoint = 0;
	}

	// 사용하는 Head 만 On/Off 될 수 있도록.. 초기화
	
	for(i=0; i<MAX_NOZZLE; i++)
	{
		m_pHead[i] = (CButton*)GetDlgItem(IDC_HEAD1_CHK+i);
		m_pHead[i]->EnableWindow(FALSE);
		m_pHead[i]->SetCheck(FALSE);
	}

	for(i=0; i<MAX_NOZZLE; i++)
	{		
		if(m_structPatternData_Repair[m_nCurrentPatternNo-1].m_bIsOnHead[i]){
			m_pHead[i]->EnableWindow(TRUE);
		}
		else{
			m_pHead[i]->EnableWindow(FALSE);
		}
		if(m_structPatternData_Repair[m_nCurrentPatternNo-1].m_bIsOnHead[i]){
			m_pHead[i]->SetCheck(TRUE);
		}
		else{
			m_pHead[i]->SetCheck(FALSE);
		}
	}
	// 현재 Pattern Number 표시
//	CString str=""; 
	str.Format("%2d",m_nCurrentPatternNo);
	m_ctrlCurPatNo.SetCaption(str);
	
	// Screen Window 초기화
	CRect rectBig;
	m_staticScreen.GetWindowRect(rectBig);
	GetParent()->ScreenToClient(rectBig);
	
	if(m_pPatternWnd != NULL) 
	{
		delete m_pPatternWnd;
		m_pPatternWnd = NULL;
	}
	m_pPatternWnd = new CPatternWnd();

	m_pPatternWnd->CreateEx(WS_EX_WINDOWEDGE, NULL, NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, rectBig, this, ID_PATTERN_NO_WND);
	m_pPatternWnd->m_bIsDisplayText = FALSE;
	m_pPatternWnd->m_bTextDisp = FALSE;
	
	m_pPatternWnd->m_nPosOper=1;

	SubDrawPattern();

	/////
	OnRadioZoom1();
	SetScrollBar();

	if(pDoc->m_structDataEditor.m_nNzlMode == 1)
	{
		//GetDlgItem(IDC_POINT_RANGE)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
		SetGrid(m_nCurrentPatternNo-1);
	}
	else
	{
		GetDlgItem(IDC_GRID_POINT)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	}

//2010.06.21 by tskim Repair 
//	DROP_DATA stDropBK;
//	for(i = 0; i < pDoc->m_structPatternData[m_nCurrentPatternNo-1].m_nDropDataCount; i++)
//	{
//		stDropBK = pDoc->m_arDropData[m_nCurrentPatternNo-1].GetAt(i);
//		pDoc->m_arDropDataBk[m_nCurrentPatternNo-1].Add(stDropBK);
//		pDoc->m_arDropDataBk[m_nCurrentPatternNo-1].SetAt(i, stDropBK);
//	}


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRepairEdit::SubDrawPattern()
{
	m_pPatternWnd->m_nPatternNum = m_nCurrentPatternNo;
	m_pPatternWnd->m_bIsBig = TRUE;
	m_pPatternWnd->m_bIsDirectSetting = FALSE;
	m_pPatternWnd->m_bRepairEditMode = TRUE;
	m_pPatternWnd->SetDefaultZoom();
	m_pPatternWnd->Invalidate();
}

void CRepairEdit::OnRadioZoom1() 
{
	// TODO: Add your control notification handler code here
	m_pPatternWnd->m_dZoom = m_pPatternWnd->m_dDefaultZoom * 1;
	
	
	m_pPatternWnd->m_pOrgPoint.x = 0;
	m_pPatternWnd->m_pOrgPoint.y = 0;
	
	m_ctlScrollHor.SetScrollRange(0, 0, TRUE);
	m_ctlScrollVer.SetScrollRange(0, 0, TRUE);
	m_ctlScrollVer.SetScrollPos(0);
	m_ctlScrollHor.SetScrollPos(0);

	m_nVPos = 0;
	m_nHPos = 0;
	m_ctrlTextDisp.EnableWindow(FALSE);
	Invalidate();
	//////////////////////////////////////////
	m_pPatternWnd->Invalidate();
	
/*
	// 2005.01.25 By shson
	// Zoom 배율 변경시 원점 복귀
	m_pBigPatternWnd->m_pOrgPoint.x = m_pBigPatternWnd->m_pOrgPoint.y = 0;
	m_pSmallPatternWnd->m_pOrgPoint.x = m_pSmallPatternWnd->m_pOrgPoint.y = 0;
*/	
	SetScrollBar();
}

void CRepairEdit::OnRadioZoom2() 
{
	// TODO: Add your control notification handler code here
	m_pPatternWnd->m_dZoom = m_pPatternWnd->m_dDefaultZoom * 2;
	
	// 2005.01.25 By shson
	// Zoom 배율 변경시 원점 복귀
	m_ctrlTextDisp.EnableWindow(TRUE);
	Invalidate();
	m_pPatternWnd->Invalidate();
	
	SetScrollBar();	
}

void CRepairEdit::OnRadioZoom4() 
{
	// TODO: Add your control notification handler code here
	m_pPatternWnd->m_dZoom = m_pPatternWnd->m_dDefaultZoom * 4;
	m_ctrlTextDisp.EnableWindow(TRUE);
	Invalidate();
	m_pPatternWnd->Invalidate();
	
	SetScrollBar();
}



void CRepairEdit::OnRadioZoom7() 
{
	// TODO: Add your control notification handler code here
	m_pPatternWnd->m_dZoom = m_pPatternWnd->m_dDefaultZoom * 7;
	m_ctrlTextDisp.EnableWindow(TRUE);
	Invalidate();
	m_pPatternWnd->Invalidate();
		
	SetScrollBar();
}



void CRepairEdit::OnRadioZoom10() 
{
	// TODO: Add your control notification handler code here
	m_pPatternWnd->m_dZoom = m_pPatternWnd->m_dDefaultZoom * 10;
	m_pPatternWnd->Invalidate();
	m_ctrlTextDisp.EnableWindow(TRUE);
	Invalidate();
	
	SetScrollBar();
}

void CRepairEdit::SetScrollBar()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	double dXOffset, dYOffset;

	dXOffset = (m_pPatternWnd->m_dZoom * pDoc->m_structDataEditor.m_dGlassSizeX) -
			   (m_pPatternWnd->m_dDefaultZoom * pDoc->m_structDataEditor.m_dGlassSizeX);

	dYOffset = (m_pPatternWnd->m_dZoom * pDoc->m_structDataEditor.m_dGlassSizeY) -
		       (m_pPatternWnd->m_dDefaultZoom * pDoc->m_structDataEditor.m_dGlassSizeY);

	m_nMaxHor = (int)(dXOffset * m_pPatternWnd->m_dZoom);
	m_nMaxVer = (int)(dYOffset * m_pPatternWnd->m_dZoom);


	m_ctlScrollHor.SetScrollRange(-m_nMaxHor/2, m_nMaxHor/2, TRUE);
	m_ctlScrollVer.SetScrollRange(-m_nMaxVer/2, m_nMaxVer/2, TRUE);
	m_ctlScrollVer.SetScrollPos(m_nVPos);
	m_ctlScrollHor.SetScrollPos(m_nHPos);
}

void CRepairEdit::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if(nSBCode == SB_LINELEFT)
    {
		m_nHPos-=10;
    }
    else if(nSBCode == SB_LINERIGHT)
    {
		m_nHPos+=10;
    }
    else if(nSBCode == SB_PAGELEFT)
    {
		m_nHPos-=50;
    }
    else if(nSBCode == SB_PAGERIGHT)
    {
		m_nHPos+=50;
    }
    else if(nSBCode == SB_THUMBTRACK)
    {
		m_nHPos = nPos;
    }
    else if(nSBCode == SB_ENDSCROLL && nSBCode != SB_THUMBTRACK)
    {

		
    }
	

	if(m_nHPos < -m_nMaxHor/2) m_nHPos = -m_nMaxHor/2;
	if(m_nHPos > m_nMaxHor/2) m_nHPos = m_nMaxHor/2;

	m_pPatternWnd->m_pOrgPoint.x = (long)(-m_nHPos / m_pPatternWnd->m_dZoom);
	m_pPatternWnd->Invalidate();
	m_ctlScrollHor.SetScrollPos(m_nHPos);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CRepairEdit::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
		if(nSBCode == SB_LINELEFT)
    {
		m_nVPos-=10;
	}
    else if(nSBCode == SB_LINERIGHT)
    {
		m_nVPos+=10;
    }
    else if(nSBCode == SB_PAGELEFT)
    {
		m_nVPos-=50;
    }
    else if(nSBCode == SB_PAGERIGHT)
    {
		m_nVPos+=50;
    }
    else if(nSBCode == SB_THUMBTRACK)
    {
		m_nVPos = nPos;
    }
    else if(nSBCode == SB_ENDSCROLL && nSBCode != SB_THUMBTRACK)
    {
    }

	if(m_nVPos < -m_nMaxVer/2) 
	{
		m_nVPos = -m_nMaxVer/2;
	}
	if(m_nVPos > m_nMaxVer/2) 
	{
		m_nVPos = m_nMaxVer/2;
	}

	
	m_pPatternWnd->m_pOrgPoint.y = (long)(m_nVPos / m_pPatternWnd->m_dZoom);
	m_pPatternWnd->Invalidate();

	m_ctlScrollVer.SetScrollPos(m_nVPos);

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CRepairEdit::OnHead1Chk() 
{
	// TODO: Add your control notification handler code here

	CMainFrame *pFrame = (CMainFrame *)::AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	
	for(int i=0; i<MAX_NOZZLE; i++)
	{
		if(m_pHead[i]->GetCheck()) 
			pDoc->m_structPatternData[m_nCurrentPatternNo-1].m_bIsOnHead[i] = TRUE;
		else
			pDoc->m_structPatternData[m_nCurrentPatternNo-1].m_bIsOnHead[i] = FALSE;
	}
	SubDrawPattern();
	//Invalidate();
}

BEGIN_EVENTSINK_MAP(CRepairEdit, CDialog)
    //{{AFX_EVENTSINK_MAP(CRepairEdit)
	ON_EVENT(CRepairEdit, IDC_CMD_RETURN, -600 /* Click */, OnClickCmdReturn, VTS_NONE)
	ON_EVENT(CRepairEdit, IDC_ALL_RESET, -600 /* Click */, OnClickAllReset, VTS_NONE)
	ON_EVENT(CRepairEdit, IDC_ALL_SET, -600 /* Click */, OnClickAllSet, VTS_NONE)
	ON_EVENT(CRepairEdit, IDC_POINT_RANGE, -600 /* Click */, OnClickPointRange, VTS_NONE)
	ON_EVENT(CRepairEdit, IDC_CMD_DROP_NUM_DISP, -600 /* Click */, OnClickCmdDropNumDisp, VTS_NONE)
	ON_EVENT(CRepairEdit, IDC_DROP_RANGE, -600 /* Click */, OnClickDropRange, VTS_NONE)
	ON_EVENT(CRepairEdit, IDC_PART_CONV, -600 /* Click */, OnClickPartConv, VTS_NONE)
	ON_EVENT(CRepairEdit, IDC_GRID_POINT, -600 /* Click */, OnClickGridPoint, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CRepairEdit::OnClickCmdReturn() 
{
	// TODO: Add your control notification handler code here
	m_pPatternWnd->m_bRepairEditMode = FALSE;
	EndDialog(IDOK);
}

void CRepairEdit::OnClickAllReset() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)::AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	if(pDoc->m_structDataEditor.m_nNzlMode != 1)
	{
		m_nMinPoint = atoi((char *)(LPCSTR)m_ctrlDropPointRange.GetTextMatrix(0, 1));
		m_nMaxPoint = atoi((char *)(LPCSTR)m_ctrlDropPointRange.GetTextMatrix(1, 1));
	}

	if(m_nMaxPoint > m_nMinPoint){
		m_pPatternWnd->PartConvDropPoint(m_nMaxPoint,m_nMinPoint,0);  //부분반전
		SubDrawPattern();
	}
	else{
		AfxMessageBox("시작점 위치와 마지막점 위치를 확인하여 주세요.!");
	}
}

void CRepairEdit::OnClickAllSet() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)::AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	if(pDoc->m_structDataEditor.m_nNzlMode != 1)
	{
		m_nMinPoint = atoi((char *)(LPCSTR)m_ctrlDropPointRange.GetTextMatrix(0, 1));
		m_nMaxPoint = atoi((char *)(LPCSTR)m_ctrlDropPointRange.GetTextMatrix(1, 1));
	}

	if(m_nMaxPoint > m_nMinPoint){
		m_pPatternWnd->PartConvDropPoint(m_nMaxPoint,m_nMinPoint,1);  //All Reset
		SubDrawPattern();
	}
	else{
		AfxMessageBox("시작점 위치와 마지막점 위치를 확인하여 주세요.!");
	}
}

void CRepairEdit::OnClickPartConv() 
{
	return;
	// TODO: Add your control notification handler code here
	m_nMinPoint = atoi((char *)(LPCSTR)m_ctrlDropPointRange.GetTextMatrix(0, 1));
	m_nMaxPoint = atoi((char *)(LPCSTR)m_ctrlDropPointRange.GetTextMatrix(1, 1));

	if(m_nMaxPoint > m_nMinPoint){
		m_pPatternWnd->PartConvDropPoint(m_nMaxPoint,m_nMinPoint,2);  //부분반전
		SubDrawPattern();
	}
	else{
		AfxMessageBox("시작점 위치와 마지막점 위치를 확인하여 주세요.!");
	}
}

void CRepairEdit::OnClickDropRange() 
{
	return;
	// TODO: Add your control notification handler code here
	m_nMinPoint = atoi((char *)(LPCSTR)m_ctrlDropPointRange.GetTextMatrix(0, 1));
	m_nMaxPoint = atoi((char *)(LPCSTR)m_ctrlDropPointRange.GetTextMatrix(1, 1));

	if(m_nMaxPoint > m_nMinPoint){
		m_pPatternWnd->PartConvDropPoint(m_nMaxPoint,m_nMinPoint,3);	// Drop 영역
		SubDrawPattern();
	}
	else{
		AfxMessageBox("시작점 위치와 마지막점 위치를 확인하여 주세요.!");
	}	
}

void CRepairEdit::OnClickPointRange() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)::AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	
	int nRow = 0, nCol = 0;
	int max = 1, min = 1;
	
	//
	nRow = m_ctrlDropPointRange.GetMouseRow();
	nCol = m_ctrlDropPointRange.GetMouseCol();
	
	max = pDoc->m_arDropData[m_nCurrentPatternNo-1].GetSize();

	if(nCol == 1){
		Use_TKg(m_ctrlDropPointRange, max, min, 720, 80, TRUE, TRUE);
		
		m_nMinPoint = atoi((char *)(LPCSTR)m_ctrlDropPointRange.GetTextMatrix(0, 1));
		m_nMaxPoint = atoi((char *)(LPCSTR)m_ctrlDropPointRange.GetTextMatrix(1, 1));
	}
}

void CRepairEdit::OnClickCmdDropNumDisp() 
{
	// TODO: Add your control notification handler code here
	if(m_pPatternWnd->m_bTextDisp)	m_pPatternWnd->m_bTextDisp = FALSE;
	else	m_pPatternWnd->m_bTextDisp = TRUE;
	SubDrawPattern();
}




void CRepairEdit::SetGrid(int nPatternNo)
{
	CMainFrame *pFrame = (CMainFrame *)::AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	CString str = "";
	int i = 0 , j = 0, k=0;

	DROP_DATA structDropData;
	int nOffset;
	
	// 배열은 일단 지우고 다시 추가
	m_arDropData_Repair[i].RemoveAll();
	
	for(j = 0; j < pDoc->m_arDropData[nPatternNo].GetSize(); j++)
		m_arDropData_Repair[nPatternNo].Add(pDoc->m_arDropData[nPatternNo].GetAt(j));

	// grid 설정
	// 원래 있던거 다 지우고 나서 
	m_GridPoint.SetRows((m_structPatternData_Repair[nPatternNo].m_nDropDataCount/m_structPatternData_Repair[nPatternNo].m_nPointNumberX)*2);
	m_GridPoint.SetCols((m_structPatternData_Repair[nPatternNo].m_nDropDataCount/m_structPatternData_Repair[nPatternNo].m_nPointNumberY));

	for(i = 0; i < m_GridPoint.GetRows() ; i++)
	{
		if(i%2 == 0)
		{
			m_GridPoint.SetRowHeight(i,200);
		}
		else
		{
			m_GridPoint.SetRowHeight(i,400);
//			str.Format("%d", i);
//			m_GridPoint.SetTextMatrix(i, 0, str);			
		}

		for(j = 0; j < m_GridPoint.GetCols() ; j++)
		{
//			if(j==0)
//			{
//				m_GridPoint.SetColWidth(j,500);
//			}
//			else
//			{
				m_GridPoint.SetColWidth(j,650);	
//				str.Format("%d", j);
//				m_GridPoint.SetTextMatrix(0, j, str);	
//			}
			m_GridPoint.SetColAlignment(j,4);
		}
	}

	// 현재 데이타 갯수와 배열의 갯수를 매치시킴
//	MatchDropDataCount(nP);
	
	// 원래 있던 doc의 데이타의 갯수와 비교해서, 기존 잡혀있던 갯수가 현재 계산될
	// 갯수보다 작으면 0으로 채워서 늘려준다.
	nOffset = m_structPatternData_Repair[nPatternNo].m_nDropDataCount - m_arDropData_Repair[nPatternNo].GetSize();
	for(i = 0; i < nOffset; i++)
	{
		m_structDropData_Repair.m_dPointX = 0.0;
		m_structDropData_Repair.m_dPointY = 0.0;
		m_structDropData_Repair.m_nN = 1;
		//
		for(int k = 0; k < MAX_NOZZLE; k++)
		{
			m_structDropData_Repair.m_nZ[(MAX_NOZZLE-1)-k] = TRUE;
			m_structDropData_Repair.m_nAddDel[(MAX_NOZZLE-1)-k] = 0;
		}
		// 추가
		m_arDropData_Repair[nPatternNo].Add(m_structDropData_Repair);
	}
	
	// grid에 데이타 설정하기 ////////////////////////////////////////////
	// 포인터 갯수만큼 설정 
	int nRow,nCol;

	for(i = 0; i < m_structPatternData_Repair[nPatternNo].m_nDropDataCount; i++)
	{
		// CArray의 데이타의 한 부분을 구조체에 넣는다.
		m_structDropData_Repair = m_arDropData_Repair[nPatternNo].GetAt(i);

		if(m_structPatternData_Repair[nPatternNo].m_nDroppingStartPos == 0)//0: Left Up 1: Left Dn 2: Right Up 3: Right Dn
		{
			
		}
		else if(m_structPatternData_Repair[nPatternNo].m_nDroppingStartPos == 1)
		{
			
		}
		else if(m_structPatternData_Repair[nPatternNo].m_nDroppingStartPos == 2)
		{
			
		}
		else if(m_structPatternData_Repair[nPatternNo].m_nDroppingStartPos == 3)
		{
			
		}
		nRow = i%m_structPatternData_Repair[nPatternNo].m_nPointNumberY;
		nCol = i/m_structPatternData_Repair[nPatternNo].m_nPointNumberY;
		
		str.Format("%.2f\n/%.2f", m_structDropData_Repair.m_dPointX,m_structDropData_Repair.m_dPointY);
		m_GridPoint.SetTextMatrix(nRow, (m_GridPoint.GetCols() - nCol)-1, str);
		for(j=0;j<MAX_NOZZLE;j++)
		{
			m_GridPoint.SetRow(nRow);
			m_GridPoint.SetCol((m_GridPoint.GetCols() - nCol)-1);
			if(m_structDropData_Repair.m_nZ[j]/* && m_structPatternData_Repair[nPatternNo].m_bIsOnHead[j]*/)
				m_GridPoint.SetCellBackColor(LIGHTGREEN);
			else
				m_GridPoint.SetCellBackColor(WHITE);				
		}
	}
}

void CRepairEdit::OnClickGridPoint() 
{
	return;
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)::AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	int nRow,nCol;
	int i;
	int nIndex1,nIndex2;
	nRow = m_GridPoint.GetMouseRow();
	nCol = m_GridPoint.GetMouseCol();

	if(nCol == 0 || nRow != 0) return;

//	nIndex1 = (m_structPatternData_Repair[m_nCurrentPatternNo-1].m_nPointNumberX*m_structPatternData_Repair[m_nCurrentPatternNo-1].m_nPointNumberY);
	nIndex1 = (m_structPatternData_Repair[m_nCurrentPatternNo-1].m_nPointNumberX-nCol)*m_structPatternData_Repair[m_nCurrentPatternNo-1].m_nPointNumberY + (nRow%m_structPatternData_Repair[m_nCurrentPatternNo-1].m_nPointNumberY);
//	nC = i/m_structPatternData_Repair[m_nCurrentPatternNo-1].m_nPointNumberY+1;//1,2 =12
//1,1 = 11
//2.1 = 12	
	if(m_GridPoint.GetCellBackColor()==LIGHTGREEN)
	{
		m_GridPoint.SetCellBackColor(WHITE);
		for(i=0;i<MAX_NOZZLE;i++)
		{
			if(m_structPatternData_Repair[m_nCurrentPatternNo-1].m_bIsOnHead[i])
			{
				m_structDropData_Repair = m_arDropData_Repair[m_nCurrentPatternNo-1].GetAt(nIndex1);
				m_structDropData_Repair.m_nZ[i] = FALSE;
				//m_nMinPoint = nIndex1+1;
				m_structDropData_Repair = m_arDropData_Repair[m_nCurrentPatternNo-1].GetAt(nIndex1+1);
				m_structDropData_Repair.m_nZ[i] = FALSE;
				//m_nMaxPoint = nIndex1+2;
			}
		}
	}
	else
	{
		m_GridPoint.SetCellBackColor(LIGHTGREEN);
	}

// 	for(i = 0; i < m_GridPoint.GetCols() ; i++)
// 	{
// 		if(m_GridPoint.GetCellBackColor()==LIGHTGREEN)	
// 		else
// 	}
}

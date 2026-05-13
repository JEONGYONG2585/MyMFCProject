// SpecCount.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"
#include "P8CA_LcDispDoc.h"
#include "SpecCount.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpecCount dialog


CSpecCount::CSpecCount(CWnd* pParent /*=NULL*/)
	: CDialog(CSpecCount::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpecCount)
	//}}AFX_DATA_INIT
	m_strTime_spec = "";
	m_nTime_Spec = 0;
	for(int i = 0; i < MAX_NOZZLE; i++)
	{
		m_nCount_Spec[i] = 0;
		m_nRealCount_Spec[i] = 0;
		m_bSel[i] = FALSE;
	}
	m_strDueDate = "0000000000";
	m_bMoveSpec = FALSE;
}


void CSpecCount::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpecCount)
	DDX_Control(pDX, IDC_SET_COUNT1, m_ctrlSetCount1);
	DDX_Control(pDX, IDC_SET_COUNT10, m_ctrlSetCount10);
	DDX_Control(pDX, IDC_SET_COUNT11, m_ctrlSetCount11);
	DDX_Control(pDX, IDC_SET_COUNT12, m_ctrlSetCount12);
	DDX_Control(pDX, IDC_SET_COUNT2, m_ctrlSetCount2);
	DDX_Control(pDX, IDC_SET_COUNT3, m_ctrlSetCount3);
	DDX_Control(pDX, IDC_SET_COUNT4, m_ctrlSetCount4);
	DDX_Control(pDX, IDC_SET_COUNT5, m_ctrlSetCount5);
	DDX_Control(pDX, IDC_SET_COUNT6, m_ctrlSetCount6);
	DDX_Control(pDX, IDC_SET_COUNT7, m_ctrlSetCount7);
	DDX_Control(pDX, IDC_SET_COUNT8, m_ctrlSetCount8);
	DDX_Control(pDX, IDC_SET_COUNT9, m_ctrlSetCount9);
	DDX_Control(pDX, IDC_SET_TIME, m_ctrlSetTime);
	DDX_Control(pDX, IDC_REMAIND_TIME, m_ctrlRemainTime);
	DDX_Control(pDX, IDC_DISP_TIME, m_ctrlDispDate);
	DDX_Control(pDX, IDC_SELECT_HEAD, m_ctrlSelect_Head);
	DDX_Control(pDX, IDC_RESET, m_ctrlReset);
	DDX_Control(pDX, IDC_COUNT_DISP1, m_ctrlCountDisp1);
	DDX_Control(pDX, IDC_COUNT_DISP10, m_ctrlCountDisp10);
	DDX_Control(pDX, IDC_COUNT_DISP11, m_ctrlCountDisp11);
	DDX_Control(pDX, IDC_COUNT_DISP12, m_ctrlCountDisp12);
	DDX_Control(pDX, IDC_COUNT_DISP2, m_ctrlCountDisp2);
	DDX_Control(pDX, IDC_COUNT_DISP3, m_ctrlCountDisp3);
	DDX_Control(pDX, IDC_COUNT_DISP4, m_ctrlCountDisp4);
	DDX_Control(pDX, IDC_COUNT_DISP5, m_ctrlCountDisp5);
	DDX_Control(pDX, IDC_COUNT_DISP6, m_ctrlCountDisp6);
	DDX_Control(pDX, IDC_COUNT_DISP7, m_ctrlCountDisp7);
	DDX_Control(pDX, IDC_COUNT_DISP8, m_ctrlCountDisp8);
	DDX_Control(pDX, IDC_COUNT_DISP9, m_ctrlCountDisp9);
	DDX_Control(pDX, IDC_SEL_HEAD1, m_ctrlHead1);
	DDX_Control(pDX, IDC_SEL_HEAD10, m_ctrlHead10);
	DDX_Control(pDX, IDC_SEL_HEAD11, m_ctrlHead11);
	DDX_Control(pDX, IDC_SEL_HEAD12, m_ctrlHead12);
	DDX_Control(pDX, IDC_SEL_HEAD2, m_ctrlHead2);
	DDX_Control(pDX, IDC_SEL_HEAD3, m_ctrlHead3);
	DDX_Control(pDX, IDC_SEL_HEAD4, m_ctrlHead4);
	DDX_Control(pDX, IDC_SEL_HEAD5, m_ctrlHead5);
	DDX_Control(pDX, IDC_SEL_HEAD6, m_ctrlHead6);
	DDX_Control(pDX, IDC_SEL_HEAD7, m_ctrlHead7);
	DDX_Control(pDX, IDC_SEL_HEAD8, m_ctrlHead8);
	DDX_Control(pDX, IDC_SEL_HEAD9, m_ctrlHead9);
	DDX_Control(pDX, IDC_MOVING, m_ctrlSpecmove1);
	DDX_Control(pDX, IDC_SEL_HEAD13, m_ctrlHead13);
	DDX_Control(pDX, IDC_SEL_HEAD14, m_ctrlHead14);
	DDX_Control(pDX, IDC_SEL_HEAD15, m_ctrlHead15);
	DDX_Control(pDX, IDC_SEL_HEAD16, m_ctrlHead16);
	DDX_Control(pDX, IDC_SET_COUNT13, m_ctrlSetCount13);
	DDX_Control(pDX, IDC_SET_COUNT14, m_ctrlSetCount14);
	DDX_Control(pDX, IDC_SET_COUNT15, m_ctrlSetCount15);
	DDX_Control(pDX, IDC_SET_COUNT16, m_ctrlSetCount16);
	DDX_Control(pDX, IDC_COUNT_DISP13, m_ctrlCountDisp13);
	DDX_Control(pDX, IDC_COUNT_DISP14, m_ctrlCountDisp14);
	DDX_Control(pDX, IDC_COUNT_DISP15, m_ctrlCountDisp15);
	DDX_Control(pDX, IDC_COUNT_DISP16, m_ctrlCountDisp16);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpecCount, CDialog)
	//{{AFX_MSG_MAP(CSpecCount)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpecCount message handlers

BOOL CSpecCount::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	CString str;
//	this->SetWindowPos( NULL,SCREEN_WIDTH-815,SCREEN_HEIGHT-690
//	,0,0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER );
//	this->SetWindowPos( NULL,SCREEN_WIDTH,SCREEN_HEIGHT
//	,0,0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER );
	Doc2Value();	
	
	m_strDueDate = OnReturnTime();
	str.Format("%d",m_nTime_Spec);
	m_ctrlSetTime.SetCaption(str);

	if(m_strTime_spec.GetLength() < 10) m_strTime_spec = "0000000000";
	if(m_strDueDate.GetLength() < 10) m_strDueDate = "0000000000";
	str.Format("%s년 %s월 %s일 %s시 %s분  ~ %s년 %s월 %s일 %s시 %s분", m_strTime_spec.Mid(0,2),m_strTime_spec.Mid(2,2),m_strTime_spec.Mid(4,2),m_strTime_spec.Mid(6,2),m_strTime_spec.Mid(8,2)
		,m_strDueDate.Mid(0,2),m_strDueDate.Mid(2,2),m_strDueDate.Mid(4,2),m_strDueDate.Mid(6,2),m_strTime_spec.Mid(8,2));
	m_ctrlDispDate.SetCaption(str);
	for(int i = 0; i < MAX_NOZZLE; i++)
	{
		str.Format("%d", m_nCount_Spec[i]);
		GetDlgItem(IDC_SET_COUNT1+i)->SetWindowText(str);
		str.Format("%d", m_nRealCount_Spec[i]);
		GetDlgItem(IDC_COUNT_DISP1+i)->SetWindowText(str);
		if(pDoc->m_bIsHeadSelected[i] == FALSE)
		{
			switch(i)
			{
			case 0: m_ctrlCountDisp1.SetBackColor(LIGHTGRAY); m_ctrlHead1.SetForeColor(LIGHTGRAY);
				break;
			case 1: m_ctrlCountDisp2.SetBackColor(LIGHTGRAY); m_ctrlHead2.SetForeColor(LIGHTGRAY); 
				break;
			case 2: m_ctrlCountDisp3.SetBackColor(LIGHTGRAY); m_ctrlHead3.SetForeColor(LIGHTGRAY);
				break;
			case 3: m_ctrlCountDisp4.SetBackColor(LIGHTGRAY); m_ctrlHead4.SetForeColor(LIGHTGRAY);
				break;
			case 4: m_ctrlCountDisp5.SetBackColor(LIGHTGRAY); m_ctrlHead5.SetForeColor(LIGHTGRAY);
				break;
			case 5: m_ctrlCountDisp6.SetBackColor(LIGHTGRAY); m_ctrlHead6.SetForeColor(LIGHTGRAY);
				break;
			case 6: m_ctrlCountDisp7.SetBackColor(LIGHTGRAY); m_ctrlHead7.SetForeColor(LIGHTGRAY);
				break;
			case 7: m_ctrlCountDisp8.SetBackColor(LIGHTGRAY); m_ctrlHead8.SetForeColor(LIGHTGRAY);
				break;
			case 8: m_ctrlCountDisp9.SetBackColor(LIGHTGRAY); m_ctrlHead9.SetForeColor(LIGHTGRAY);
				break;
			case 9: m_ctrlCountDisp10.SetBackColor(LIGHTGRAY); m_ctrlHead10.SetForeColor(LIGHTGRAY);
				break;
			case 10: m_ctrlCountDisp11.SetBackColor(LIGHTGRAY); m_ctrlHead11.SetForeColor(LIGHTGRAY);
				break;
			case 11: m_ctrlCountDisp12.SetBackColor(LIGHTGRAY); m_ctrlHead12.SetForeColor(LIGHTGRAY);
				break;
			case 12: m_ctrlCountDisp13.SetBackColor(LIGHTGRAY); m_ctrlHead13.SetForeColor(LIGHTGRAY);
				break;
			case 13: m_ctrlCountDisp14.SetBackColor(LIGHTGRAY); m_ctrlHead14.SetForeColor(LIGHTGRAY);
				break;
			case 14: m_ctrlCountDisp15.SetBackColor(LIGHTGRAY); m_ctrlHead15.SetForeColor(LIGHTGRAY);
				break;
			case 15: m_ctrlCountDisp16.SetBackColor(LIGHTGRAY); m_ctrlHead16.SetForeColor(LIGHTGRAY);
				break;
			default: break;
			}
		}
	}
	if(ThreadStage.bMachineRunning)
	{
		m_ctrlSetCount1.SetEnabled(FALSE);
		m_ctrlSetCount2.SetEnabled(FALSE);
		m_ctrlSetCount3.SetEnabled(FALSE);
		m_ctrlSetCount4.SetEnabled(FALSE);
		m_ctrlSetCount5.SetEnabled(FALSE);
		m_ctrlSetCount6.SetEnabled(FALSE);
		m_ctrlSetCount7.SetEnabled(FALSE);
		m_ctrlSetCount8.SetEnabled(FALSE);
		m_ctrlSetCount9.SetEnabled(FALSE);
		m_ctrlSetCount10.SetEnabled(FALSE);
		m_ctrlSetCount11.SetEnabled(FALSE);
		m_ctrlSetCount12.SetEnabled(FALSE);
		m_ctrlSetCount13.SetEnabled(FALSE);
		m_ctrlSetCount14.SetEnabled(FALSE);
		m_ctrlSetCount15.SetEnabled(FALSE);
		m_ctrlSetCount16.SetEnabled(FALSE);
		m_ctrlSetTime.SetEnabled(FALSE);
		m_ctrlSelect_Head.SetEnabled(FALSE);
		m_ctrlReset.SetEnabled(FALSE);
		m_ctrlSpecmove1.SetEnabled(FALSE);
	}
	for(i = 0; i < MAX_NOZZLE; i++)
	{
		str.Format("%d",m_nRealCount_Spec[i]);
		GetDlgItem(IDC_COUNT_DISP1+i)->SetWindowText(str);
		if((m_nRealCount_Spec[i] >= m_nCount_Spec[i]) && m_nCount_Spec[i] != 0)
		{
			switch(i) 
			{
			case 0: m_ctrlCountDisp1.SetBackColor(LIGHTRED);
				break;
			case 1: m_ctrlCountDisp2.SetBackColor(LIGHTRED);
				break;
			case 2: m_ctrlCountDisp3.SetBackColor(LIGHTRED); 
				break;
			case 3: m_ctrlCountDisp4.SetBackColor(LIGHTRED);
				break;
			case 4: m_ctrlCountDisp5.SetBackColor(LIGHTRED); 
				break;
			case 5: m_ctrlCountDisp6.SetBackColor(LIGHTRED);
				break;
			case 6: m_ctrlCountDisp7.SetBackColor(LIGHTRED); 
				break;
			case 7: m_ctrlCountDisp8.SetBackColor(LIGHTRED);
				break;
			case 8: m_ctrlCountDisp9.SetBackColor(LIGHTRED); 
				break;
			case 9: m_ctrlCountDisp10.SetBackColor(LIGHTRED);
				break;
			case 10: m_ctrlCountDisp11.SetBackColor(LIGHTRED); 
				break;
			case 11: m_ctrlCountDisp12.SetBackColor(LIGHTRED);
				break;
			case 12: m_ctrlCountDisp13.SetBackColor(LIGHTRED); 
				break;
			case 13: m_ctrlCountDisp14.SetBackColor(LIGHTRED);
				break;
			case 14: m_ctrlCountDisp15.SetBackColor(LIGHTRED); 
				break;
			case 15: m_ctrlCountDisp16.SetBackColor(LIGHTRED);
				break;		
			default: break;
			}
		}
	}
	SubDispTime();
	SubTimerFunc();
	if(m_bMoveSpec) 
	{
		m_ctrlSpecmove1.SetBackColor(LIGHTGREEN);
		m_ctrlSpecmove1.SetCaption("동작");
		SetTimer(0,1000, NULL);	
	}
	else
	{
		m_ctrlSpecmove1.SetBackColor(RED);
		m_ctrlSpecmove1.SetCaption("정지");
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSpecCount::Valu2Doc()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	
	int i = 0;
	CString strPath ="";
	for(i = 0; i < MAX_NOZZLE; i++) 
	{
		pDoc->m_nSpecCount[i] = m_nCount_Spec[i];
		pDoc->m_nRealSpecCount[i] = m_nRealCount_Spec[i];
	}
//
	pDoc->m_strSpecTime = m_strTime_spec;
	pDoc->m_nSpecTime = m_nTime_Spec;
	pDoc->m_bMovingSpecCount = g_bSpecCount = m_bMoveSpec;
	pDoc->Save_SpecoutCount();
}

void CSpecCount::Doc2Value()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	m_nTime_Spec = pDoc->m_nSpecTime;
	m_strTime_spec = pDoc->m_strSpecTime;
	m_bMoveSpec = g_bSpecCount;
	for(int i = 0; i < MAX_NOZZLE; i++)
	{
		m_nCount_Spec[i] = pDoc->m_nSpecCount[i];
		m_nRealCount_Spec[i] = pDoc->m_nRealSpecCount[i];
	}
}

int CSpecCount::OnTimeCal()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CTime pTime = CTime::GetCurrentTime();
	int temp_Y = 0, temp_M = 0, temp_D = 0, temp_H = 0, temp_B = 0;
	int temp_Y1 = 0, temp_M1 = 0, temp_D1 = 0, temp_H1 = 0, nMonth = 0, temp_B1 = 0;
	int nTotal_Hour = 0, nTotal_Day = 0, nTotal_Min = 0;
	if(m_strDueDate.GetLength() < 10) m_strDueDate = "0000000000";
	temp_Y = atoi(m_strDueDate.Mid(0,2));
	temp_M = atoi(m_strDueDate.Mid(2,2));
	temp_D = atoi(m_strDueDate.Mid(4,2));
	temp_H = atoi(m_strDueDate.Mid(6,2));
	temp_B = atoi(m_strDueDate.Mid(8,2));
	
	temp_Y1 = pTime.GetYear() - 2000;
	temp_M1 = pTime.GetMonth();
	temp_D1 = pTime.GetDay();
	temp_H1 = pTime.GetHour();
	temp_B1 = pTime.GetMinute();

	if(temp_D1 > temp_D)
	{
		if(temp_M1 > temp_M)
		{
			if(temp_Y1 < temp_Y)
				nTotal_Day += (int)fabs(31 - (temp_D1 - temp_D)); 
			else return 0;
		}
		else if(temp_M1 < temp_M)
		{
			if(temp_M1 == 2) 
			{
				if(temp_Y1 % 4 == 0) nMonth = 29;
				else nMonth = 28;
			}
			else if(temp_M1 == 1 || temp_M1 == 3 || temp_M1 == 5 || temp_M1 == 7 || temp_M1 == 8 || temp_M1 == 10 || temp_M1 == 12) nMonth = 31;
			else nMonth = 30;

			nTotal_Day += (int)(fabs(nMonth -(temp_D1 - temp_D)));
		}
		else return 0;
	}
	else 
	{
		if((temp_Y1 < temp_Y)||(temp_M1 < temp_M)) return 0;
		else nTotal_Day = temp_D - temp_D1;
	}

	if((temp_H - temp_H1) < 0) nTotal_Hour = nTotal_Day * 24 - (temp_H1 - temp_H);
	else nTotal_Hour = nTotal_Day * 24 + (temp_H - temp_H1);
	if((temp_B - temp_B1) < 0) 
	{
		nTotal_Min = 60 - (temp_B1 - temp_B);
		nTotal_Hour--;
	}
	else nTotal_Min = temp_B - temp_B1;
	nTotal_Min = nTotal_Hour * 60 + nTotal_Min;
	return nTotal_Min;
}

CString CSpecCount::OnReturnTime()
{
	CString strTotal = "";
	int temp_Y = 0, temp_M = 0, temp_D = 0, temp_H = 0, temp_B = 0;
	int plus_H = 0,plus_D = 0, plus_M = 0;
	int nMonth = 0;
	if(m_strTime_spec.GetLength() < 10) m_strTime_spec = "0000000000";
	if(m_nTime_Spec > 0)
	{
		temp_Y = atoi(m_strTime_spec.Mid(0,2));
		temp_M = atoi(m_strTime_spec.Mid(2,2));
		temp_D = atoi(m_strTime_spec.Mid(4,2));
		temp_H = atoi(m_strTime_spec.Mid(6,2));
		temp_B = atoi(m_strTime_spec.Mid(8,2));

		temp_H += m_nTime_Spec;
		plus_D = (int)(temp_H/24);
		if(plus_D > 0)
		{
			temp_H = temp_H%24;
			temp_D += plus_D;
			if(temp_M == 2)
			{
				if(temp_Y % 4 == 0) nMonth = 29;
				else nMonth = 28;
			}
			else if(temp_M == 1 || temp_M == 3 || temp_M == 5 || temp_M == 7 || temp_M == 8 || temp_M == 10 || temp_M == 12) nMonth = 31;
			else nMonth = 30;
			if(temp_D > nMonth) plus_M = (int)(temp_D/nMonth);
			if(plus_M > 0)
			{
				temp_D = temp_D%nMonth;
				temp_M++;
				if(temp_M > 12) 
				{temp_Y++; temp_M = 1;}
			}
		}
	}
	strTotal.Format("%02d%02d%02d%02d%02d",temp_Y,temp_M,temp_D,temp_H,temp_B);
	return strTotal;
}

void CSpecCount::SubDispTime()
{
	CString str;
	int i = 0, Temp_H = 0;
	int Temp_B = OnTimeCal();
	Temp_H = (int)(Temp_B / 60);
	Temp_B = int(Temp_B%60);
	if(Temp_H <= 0) Temp_H = 0;
	if(Temp_B <= 0) Temp_B = 0;
	if(Temp_H == 0) m_ctrlRemainTime.SetForeColor(RED);
	else m_ctrlRemainTime.SetForeColor(LIGHTBLUE);
	str.Format("%d시간 %d분", Temp_H, Temp_B);
	m_ctrlRemainTime.SetCaption(str);
}

BEGIN_EVENTSINK_MAP(CSpecCount, CDialog)
    //{{AFX_EVENTSINK_MAP(CSpecCount)
	ON_EVENT(CSpecCount, IDC_SET_COUNT1, -600 /* Click */, OnClickSetCount1, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SET_COUNT2, -600 /* Click */, OnClickSetCount2, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SET_COUNT3, -600 /* Click */, OnClickSetCount3, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SET_COUNT4, -600 /* Click */, OnClickSetCount4, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SET_COUNT5, -600 /* Click */, OnClickSetCount5, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SET_COUNT6, -600 /* Click */, OnClickSetCount6, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SET_COUNT7, -600 /* Click */, OnClickSetCount7, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SET_COUNT8, -600 /* Click */, OnClickSetCount8, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SET_COUNT9, -600 /* Click */, OnClickSetCount9, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SET_COUNT10, -600 /* Click */, OnClickSetCount10, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SET_COUNT11, -600 /* Click */, OnClickSetCount11, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SET_COUNT12, -600 /* Click */, OnClickSetCount12, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SET_TIME, -600 /* Click */, OnClickSetTime, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SELECT_HEAD, -600 /* Click */, OnClickSelectHead, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_RESET, -600 /* Click */, OnClickReset, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_CANCEL, -600 /* Click */, OnClickCancel, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_OK, -600 /* Click */, OnClickOk, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SEL_HEAD1, -600 /* Click */, OnClickSelHead1, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SEL_HEAD2, -600 /* Click */, OnClickSelHead2, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SEL_HEAD3, -600 /* Click */, OnClickSelHead3, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SEL_HEAD4, -600 /* Click */, OnClickSelHead4, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SEL_HEAD5, -600 /* Click */, OnClickSelHead5, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SEL_HEAD6, -600 /* Click */, OnClickSelHead6, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SEL_HEAD7, -600 /* Click */, OnClickSelHead7, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SEL_HEAD8, -600 /* Click */, OnClickSelHead8, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SEL_HEAD9, -600 /* Click */, OnClickSelHead9, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SEL_HEAD10, -600 /* Click */, OnClickSelHead10, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SEL_HEAD11, -600 /* Click */, OnClickSelHead11, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SEL_HEAD12, -600 /* Click */, OnClickSelHead12, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_MOVING, -600 /* Click */, OnClickMoving, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SEL_HEAD13, -600 /* Click */, OnClickSelHead13, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SEL_HEAD14, -600 /* Click */, OnClickSelHead14, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SEL_HEAD15, -600 /* Click */, OnClickSelHead15, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SEL_HEAD16, -600 /* Click */, OnClickSelHead16, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SET_COUNT13, -600 /* Click */, OnClickSetCount13, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SET_COUNT14, -600 /* Click */, OnClickSetCount14, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SET_COUNT15, -600 /* Click */, OnClickSetCount15, VTS_NONE)
	ON_EVENT(CSpecCount, IDC_SET_COUNT16, -600 /* Click */, OnClickSetCount16, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CSpecCount::OnClickSetCount1() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlSetCount1, 1000, 0, 400, 300);
	m_nCount_Spec[0] = atoi(m_ctrlSetCount1.GetCaption());	
}

void CSpecCount::OnClickSetCount2() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlSetCount2, 1000, 0, 400, 300);
	m_nCount_Spec[1] = atoi(m_ctrlSetCount2.GetCaption());	
}

void CSpecCount::OnClickSetCount3() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlSetCount3, 1000, 0, 400, 300);
	m_nCount_Spec[2] = atoi(m_ctrlSetCount3.GetCaption());	
}

void CSpecCount::OnClickSetCount4() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlSetCount4, 1000, 0, 400, 300);
	m_nCount_Spec[3] = atoi(m_ctrlSetCount4.GetCaption());	
}

void CSpecCount::OnClickSetCount5() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlSetCount5, 1000, 0, 400, 300);
	m_nCount_Spec[4] = atoi(m_ctrlSetCount5.GetCaption());	
}

void CSpecCount::OnClickSetCount6() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlSetCount6, 1000, 0, 400, 300);
	m_nCount_Spec[5] = atoi(m_ctrlSetCount6.GetCaption());	
}

void CSpecCount::OnClickSetCount7() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlSetCount7, 1000, 0, 400, 300);
	m_nCount_Spec[6] = atoi(m_ctrlSetCount7.GetCaption());	
}

void CSpecCount::OnClickSetCount8() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlSetCount8, 1000, 0, 400, 300);
	m_nCount_Spec[7] = atoi(m_ctrlSetCount8.GetCaption());	
}

void CSpecCount::OnClickSetCount9() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlSetCount9, 1000, 0, 400, 300);
	m_nCount_Spec[8] = atoi(m_ctrlSetCount9.GetCaption());	
}

void CSpecCount::OnClickSetCount10() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlSetCount10, 1000, 0, 400, 300);
	m_nCount_Spec[9] = atoi(m_ctrlSetCount10.GetCaption());	
}

void CSpecCount::OnClickSetCount11() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlSetCount11, 1000, 0, 400, 300);
	m_nCount_Spec[10] = atoi(m_ctrlSetCount11.GetCaption());	
}

void CSpecCount::OnClickSetCount12() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlSetCount12, 1000, 0, 400, 300);
	m_nCount_Spec[11] = atoi(m_ctrlSetCount12.GetCaption());	
}

void CSpecCount::OnClickSetTime() 
{
	// TODO: Add your control notification handler code here
	KillTimer(0);
	double dMax = 0.0;
	CTime pTime = CTime::GetCurrentTime();
	if(pTime.GetMonth() == 2) 
	{
		if(pTime.GetYear() % 4 == 0) dMax = 29.0;
		else dMax = 28.0;
	}
	else if(pTime.GetMonth() == 1 || pTime.GetMonth() == 3 || pTime.GetMonth() == 5 || pTime.GetMonth() == 7 || pTime.GetMonth() == 8 || pTime.GetMonth() == 10 || pTime.GetMonth() == 12) dMax = 31.0;
	else dMax = 30.0;
	dMax = (dMax-1)*24;
	Use_TK(m_ctrlSetTime, dMax, 1, 400, 300);
	m_nTime_Spec = atoi(m_ctrlSetTime.GetCaption());
	OnDispDuedate();
	SubTimerFunc();
	SetTimer(0,10000, NULL);
}

void CSpecCount::OnClickSelectHead() 
{
	// TODO: Add your control notification handler code here
	CString str;
	Use_TK(m_ctrlSetCount1, 1000, 0, 400, 300);
	m_nCount_Spec[0] = atoi(m_ctrlSetCount1.GetCaption());
	for(int i = 0; i < MAX_NOZZLE - 1; i++)
	{
		str.Format("%d", m_nCount_Spec[0]);
		GetDlgItem(IDC_SET_COUNT2+i)->SetWindowText(str);
		m_nCount_Spec[i+1] = m_nCount_Spec[0];
	}
}

void CSpecCount::OnClickReset() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	for(int i = 0; i < MAX_NOZZLE; i++)
	{
		if(m_bSel[i])
		{
			GetDlgItem(IDC_COUNT_DISP1 + i)->SetWindowText("0");
			m_nRealCount_Spec[i] = 0;
			if(pDoc->m_bIsHeadSelected[i] == FALSE) continue;
			switch(i) 
			{
			case 0: m_ctrlCountDisp1.SetBackColor(YELLOW);
				break;
			case 1: m_ctrlCountDisp2.SetBackColor(YELLOW);
				break;
			case 2: m_ctrlCountDisp3.SetBackColor(YELLOW); 
				break;
			case 3: m_ctrlCountDisp4.SetBackColor(YELLOW);
				break;
			case 4: m_ctrlCountDisp5.SetBackColor(YELLOW); 
				break;
			case 5: m_ctrlCountDisp6.SetBackColor(YELLOW);
				break;
			case 6: m_ctrlCountDisp7.SetBackColor(YELLOW); 
				break;
			case 7: m_ctrlCountDisp8.SetBackColor(YELLOW);
				break;
			case 8: m_ctrlCountDisp9.SetBackColor(YELLOW); 
				break;
			case 9: m_ctrlCountDisp10.SetBackColor(YELLOW);
				break;
			case 10: m_ctrlCountDisp11.SetBackColor(YELLOW); 
				break;
			case 11: m_ctrlCountDisp12.SetBackColor(YELLOW);
				break;
			case 12: m_ctrlCountDisp13.SetBackColor(YELLOW); 
				break;
			case 13: m_ctrlCountDisp14.SetBackColor(YELLOW);
				break;
			case 14: m_ctrlCountDisp15.SetBackColor(YELLOW); 
				break;
			case 15: m_ctrlCountDisp16.SetBackColor(YELLOW);
				break;
			default: break;
			}
		}
	}
}

void CSpecCount::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CString str;
	if(nIDEvent == 0)
	{
		SubDispTime();
		SubTimerFunc();
		for(int i = 0; i < MAX_NOZZLE; i++)
		{
			if(ThreadStage.bMachineRunning == FALSE) 
				str.Format("%d",m_nRealCount_Spec[i]);
			else 
				str.Format("%d",pDoc->m_nRealSpecCount[i]);
			GetDlgItem(IDC_COUNT_DISP1+i)->SetWindowText(str);
			if(ThreadStage.bMachineRunning == FALSE) continue;
			m_nRealCount_Spec[i] = pDoc->m_nRealSpecCount[i];
			if((pDoc->m_nRealSpecCount[i] >= m_nCount_Spec[i]) && m_nCount_Spec[i] != 0)
			{
				switch(i) 
				{
				case 0: m_ctrlCountDisp1.SetBackColor(LIGHTRED);
					break;
				case 1: m_ctrlCountDisp2.SetBackColor(LIGHTRED);
					break;
				case 2: m_ctrlCountDisp3.SetBackColor(LIGHTRED); 
					break;
				case 3: m_ctrlCountDisp4.SetBackColor(LIGHTRED);
					break;
				case 4: m_ctrlCountDisp5.SetBackColor(LIGHTRED); 
					break;
				case 5: m_ctrlCountDisp6.SetBackColor(LIGHTRED);
					break;
				case 6: m_ctrlCountDisp7.SetBackColor(LIGHTRED); 
					break;
				case 7: m_ctrlCountDisp8.SetBackColor(LIGHTRED);
					break;
				case 8: m_ctrlCountDisp9.SetBackColor(LIGHTRED); 
					break;
				case 9: m_ctrlCountDisp10.SetBackColor(LIGHTRED);
					break;
				case 10: m_ctrlCountDisp11.SetBackColor(LIGHTRED); 
					break;
				case 11: m_ctrlCountDisp12.SetBackColor(LIGHTRED);
					break;
				case 12: m_ctrlCountDisp13.SetBackColor(LIGHTRED); 
					break;
				case 13: m_ctrlCountDisp14.SetBackColor(LIGHTRED);
					break;
				case 14: m_ctrlCountDisp15.SetBackColor(LIGHTRED); 
					break;
				case 15: m_ctrlCountDisp16.SetBackColor(LIGHTRED);
					break;
				default: break;
				}
			}
		}
	}
	CDialog::OnTimer(nIDEvent);
}


void CSpecCount::OnClickCancel() 
{
	CDialog::OnCancel();
}

void CSpecCount::OnClickOk() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	
	if(ThreadStage.bMachineRunning == FALSE)
		Valu2Doc();	
	pView->m_strDuedate = m_strDueDate;
	CDialog::OnOK();
}

HBRUSH CSpecCount::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
		
	if(nCtlColor == CTLCOLOR_DLG)
	{
		return CreateSolidBrush(WHITEGREEN);
	}
	// TODO: Change any attributes of the DC here
	// TODO: Return a different brush if the default is not desired
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CSpecCount::SubTimerFunc()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CTime pTime = CTime::GetCurrentTime();
	int temp[5] = {0,};
	BOOL bAct = FALSE;
	if(m_strDueDate.GetLength() < 10) m_strDueDate = "0000000000";
	for(int i = 0; i < 10; i+=2)
	{
		temp[i/2] = (int)atoi(m_strDueDate.Mid(i,2));
	}
	if(temp[0] < pTime.GetYear() - 2000) bAct = FALSE;
	else if((temp[1] < pTime.GetMonth()) && (temp[0] == pTime.GetYear() - 2000)) bAct = FALSE;
	else if(temp[2] < pTime.GetDay() && temp[1] == pTime.GetMonth()) bAct = FALSE;
	else if((temp[3] < pTime.GetHour()) && (temp[2] == pTime.GetDay()) && (temp[1] == pTime.GetMonth())) bAct = FALSE;
	else if((temp[4] <= pTime.GetMinute()) && (temp[3] == pTime.GetHour()) && (temp[2] == pTime.GetDay()) && (temp[1] == pTime.GetMonth())) bAct = FALSE;
	else bAct = TRUE;

	if(bAct == FALSE)
	{
		pDoc->m_strSpecTime.Format("%02d%02d%02d%02d%02d",pTime.GetYear() - 2000,pTime.GetMonth(),pTime.GetDay(),pTime.GetHour(),pTime.GetMinute());
		m_strDueDate = OnReturnTime();	
		OnDispDuedate();
//////////////////////////////////////////////////////////////////////////////////////////////////
		for(int i = 0; i < MAX_NOZZLE; i++)
		{
			GetDlgItem(IDC_COUNT_DISP1 + i)->SetWindowText("0");
			pDoc->m_nRealSpecCount[i] = m_nRealCount_Spec[i] = 0;
			if(pDoc->m_bIsHeadSelected[i] == FALSE) continue;
			switch(i) 
			{
			case 0: m_ctrlCountDisp1.SetBackColor(YELLOW);
				break;
			case 1: m_ctrlCountDisp2.SetBackColor(YELLOW);
				break;
			case 2: m_ctrlCountDisp3.SetBackColor(YELLOW); 
				break;
			case 3: m_ctrlCountDisp4.SetBackColor(YELLOW);
				break;
			case 4: m_ctrlCountDisp5.SetBackColor(YELLOW); 
				break;
			case 5: m_ctrlCountDisp6.SetBackColor(YELLOW);
				break;
			case 6: m_ctrlCountDisp7.SetBackColor(YELLOW); 
				break;
			case 7: m_ctrlCountDisp8.SetBackColor(YELLOW);
				break;
			case 8: m_ctrlCountDisp9.SetBackColor(YELLOW); 
				break;
			case 9: m_ctrlCountDisp10.SetBackColor(YELLOW);
				break;
			case 10: m_ctrlCountDisp11.SetBackColor(YELLOW); 
				break;
			case 11: m_ctrlCountDisp12.SetBackColor(YELLOW);
				break;
			case 12: m_ctrlCountDisp13.SetBackColor(YELLOW); 
				break;
			case 13: m_ctrlCountDisp14.SetBackColor(YELLOW);
				break;
			case 14: m_ctrlCountDisp15.SetBackColor(YELLOW); 
				break;
			case 15: m_ctrlCountDisp16.SetBackColor(YELLOW);
				break;
			default: break;
			}
			
		}

	}
}

void CSpecCount::OnClickSelHead1() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	if(ThreadStage.bMachineRunning) return;
	if(m_bSel[0] ==  FALSE) 
	{
		m_bSel[0] = TRUE;
		m_ctrlHead1.SetBackColor(0x00800080);
	}
	else 
	{
		m_bSel[0] = FALSE;
		m_ctrlHead1.SetBackColor(0x00008080);
	}

}

void CSpecCount::OnClickSelHead2() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	if(ThreadStage.bMachineRunning) return;
	if(m_bSel[1] ==  FALSE) 
	{
		m_bSel[1] = TRUE;
		m_ctrlHead2.SetBackColor(0x00800080);
	}
	else 
	{
		m_bSel[1] = FALSE;
		m_ctrlHead2.SetBackColor(0x00008080);
	}
	
}

void CSpecCount::OnClickSelHead3() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	if(ThreadStage.bMachineRunning) return;
	if(m_bSel[2] ==  FALSE) 
	{
		m_bSel[2] = TRUE;
		m_ctrlHead3.SetBackColor(0x00800080);
	}
	else 
	{
		m_bSel[2] = FALSE;
		m_ctrlHead3.SetBackColor(0x00008080);
	}
	
}

void CSpecCount::OnClickSelHead4() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	if(ThreadStage.bMachineRunning) return;
	if(m_bSel[3] ==  FALSE) 
	{
		m_bSel[3] = TRUE;
		m_ctrlHead4.SetBackColor(0x00800080);
	}
	else 
	{
		m_bSel[3] = FALSE;
		m_ctrlHead4.SetBackColor(0x00008080);
	}
	
}

void CSpecCount::OnClickSelHead5() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	if(ThreadStage.bMachineRunning) return;
	if(m_bSel[4] ==  FALSE) 
	{
		m_bSel[4] = TRUE;
		m_ctrlHead5.SetBackColor(0x00800080);
	}
	else 
	{
		m_bSel[4] = FALSE;
		m_ctrlHead5.SetBackColor(0x00008080);
	}
	
}

void CSpecCount::OnClickSelHead6() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	if(ThreadStage.bMachineRunning) return;
	if(m_bSel[5] ==  FALSE) 
	{
		m_bSel[5] = TRUE;
		m_ctrlHead6.SetBackColor(0x00800080);
	}
	else 
	{
		m_bSel[5] = FALSE;
		m_ctrlHead6.SetBackColor(0x00008080);
	}
	
}

void CSpecCount::OnClickSelHead7() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	if(ThreadStage.bMachineRunning) return;
	if(m_bSel[6] ==  FALSE) 
	{
		m_bSel[6] = TRUE;
		m_ctrlHead7.SetBackColor(0x00800080);
	}
	else 
	{
		m_bSel[6] = FALSE;
		m_ctrlHead7.SetBackColor(0x00008080);
	}
	
}

void CSpecCount::OnClickSelHead8() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	if(ThreadStage.bMachineRunning) return;
	if(m_bSel[7] ==  FALSE) 
	{
		m_bSel[7] = TRUE;
		m_ctrlHead8.SetBackColor(0x00800080);
	}
	else 
	{
		m_bSel[7] = FALSE;
		m_ctrlHead8.SetBackColor(0x00008080);
	}
	
}

void CSpecCount::OnClickSelHead9() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	if(ThreadStage.bMachineRunning) return;
	if(m_bSel[8] ==  FALSE) 
	{
		m_bSel[8] = TRUE;
		m_ctrlHead9.SetBackColor(0x00800080);
	}
	else 
	{
		m_bSel[8] = FALSE;
		m_ctrlHead9.SetBackColor(0x00008080);
	}
	
}

void CSpecCount::OnClickSelHead10() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	if(ThreadStage.bMachineRunning) return;
	if(m_bSel[9] ==  FALSE) 
	{
		m_bSel[9] = TRUE;
		m_ctrlHead10.SetBackColor(0x00800080);
	}
	else 
	{
		m_bSel[9] = FALSE;
		m_ctrlHead10.SetBackColor(0x00008080);
	}
	
}

void CSpecCount::OnClickSelHead11() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	if(ThreadStage.bMachineRunning) return;
	if(m_bSel[10] ==  FALSE) 
	{
		m_bSel[10] = TRUE;
		m_ctrlHead11.SetBackColor(0x00800080);
	}
	else 
	{
		m_bSel[10] = FALSE;
		m_ctrlHead11.SetBackColor(0x00008080);
	}
	
}

void CSpecCount::OnClickSelHead12() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	if(ThreadStage.bMachineRunning) return;
	if(m_bSel[11] ==  FALSE) 
	{
		m_bSel[11] = TRUE;
		m_ctrlHead12.SetBackColor(0x00800080);
	}
	else 
	{
		m_bSel[11] = FALSE;
		m_ctrlHead12.SetBackColor(0x00008080);
	}
	
}
//0x00008080

void CSpecCount::OnClickMoving() 
{
	// TODO: Add your control notification handler code here
	if(m_bMoveSpec == TRUE)
	{
		m_bMoveSpec = FALSE;
		m_ctrlSpecmove1.SetBackColor(RED);
		m_ctrlSpecmove1.SetCaption("정지");
		KillTimer(0);
	}
	else
	{
		m_bMoveSpec = TRUE;
		m_ctrlSpecmove1.SetBackColor(LIGHTGREEN);
		m_ctrlSpecmove1.SetCaption("동작");
		SetTimer(0,1000,NULL);
	}
}

void CSpecCount::OnDispDuedate()
{
	CString str;
	CTime pTime = CTime::GetCurrentTime();
	m_strTime_spec.Format("%02d%02d%02d%02d%02d",pTime.GetYear()-2000,pTime.GetMonth(),pTime.GetDay(),pTime.GetHour(),pTime.GetMinute());
	m_nTime_Spec = atoi(m_ctrlSetTime.GetCaption());
	m_strDueDate = OnReturnTime();
	if(m_strTime_spec.GetLength() < 10) m_strTime_spec = "0000000000";
	if(m_strDueDate.GetLength() < 10) m_strDueDate = "0000000000";
	str.Format("%s년 %s월 %s일 %s시 %s분  ~ %s년 %s월 %s일 %s시 %s분", m_strTime_spec.Mid(0,2),m_strTime_spec.Mid(2,2),m_strTime_spec.Mid(4,2),m_strTime_spec.Mid(6,2),m_strTime_spec.Mid(8,2)
		,m_strDueDate.Mid(0,2),m_strDueDate.Mid(2,2),m_strDueDate.Mid(4,2),m_strDueDate.Mid(6,2),m_strTime_spec.Mid(8,2));
	m_ctrlDispDate.SetCaption(str);
	SubDispTime();
}

void CSpecCount::OnClickSelHead13() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	if(ThreadStage.bMachineRunning) return;
	if(m_bSel[12] ==  FALSE) 
	{
		m_bSel[12] = TRUE;
		m_ctrlHead13.SetBackColor(0x00800080);
	}
	else 
	{
		m_bSel[12] = FALSE;
		m_ctrlHead13.SetBackColor(0x00008080);
	}	
}

void CSpecCount::OnClickSelHead14() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	if(ThreadStage.bMachineRunning) return;
	if(m_bSel[13] ==  FALSE) 
	{
		m_bSel[13] = TRUE;
		m_ctrlHead14.SetBackColor(0x00800080);
	}
	else 
	{
		m_bSel[13] = FALSE;
		m_ctrlHead14.SetBackColor(0x00008080);
	}		
}

void CSpecCount::OnClickSelHead15() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	if(ThreadStage.bMachineRunning) return;
	if(m_bSel[14] ==  FALSE) 
	{
		m_bSel[14] = TRUE;
		m_ctrlHead15.SetBackColor(0x00800080);
	}
	else 
	{
		m_bSel[14] = FALSE;
		m_ctrlHead15.SetBackColor(0x00008080);
	}		
}

void CSpecCount::OnClickSelHead16() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	if(ThreadStage.bMachineRunning) return;
	if(m_bSel[15] ==  FALSE) 
	{
		m_bSel[15] = TRUE;
		m_ctrlHead16.SetBackColor(0x00800080);
	}
	else 
	{
		m_bSel[15] = FALSE;
		m_ctrlHead16.SetBackColor(0x00008080);
	}		
}

void CSpecCount::OnClickSetCount13() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlSetCount13, 1000, 0, 400, 300);
	m_nCount_Spec[12] = atoi(m_ctrlSetCount13.GetCaption());		
}

void CSpecCount::OnClickSetCount14() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlSetCount14, 1000, 0, 400, 300);
	m_nCount_Spec[13] = atoi(m_ctrlSetCount14.GetCaption());			
}

void CSpecCount::OnClickSetCount15() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlSetCount15, 1000, 0, 400, 300);
	m_nCount_Spec[14] = atoi(m_ctrlSetCount15.GetCaption());				
}

void CSpecCount::OnClickSetCount16() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlSetCount16, 1000, 0, 400, 300);
	m_nCount_Spec[15] = atoi(m_ctrlSetCount16.GetCaption());				
}

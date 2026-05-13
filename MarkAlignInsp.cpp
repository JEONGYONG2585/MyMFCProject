// MarkAlignInsp.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "MarkAlignInsp.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"
#include "P8CA_LcDispDoc.h"
#include "NormalMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

double m_nSpd[6] = {0.1,5,10,30,50,100};
/////////////////////////////////////////////////////////////////////////////
// CMarkAlignInsp dialog


CMarkAlignInsp::CMarkAlignInsp(CWnd* pParent /*=NULL*/)
	: CDialog(CMarkAlignInsp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMarkAlignInsp)
	m_nCamSel = -1;
	//}}AFX_DATA_INIT
	m_nInfoDisplay = 0;
	m_fontComboSpd = new CFont;
	m_nCamChannel = 0;
	m_nCamSel = -1;

	m_dCurrPos_K = 0.0;
	for(int i=0;i<MAX_NOZZLE;i++)
		m_dCurrPos_S[i] =0.0;
}


void CMarkAlignInsp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMarkAlignInsp)
	DDX_Control(pDX, IDC_COMBO_SPD, m_comboSpd);
	DDX_Control(pDX, IDC_LIGELEDISPLAYCTRL1, m_ctrlAlign1);
	DDX_Control(pDX, IDC_RESET, m_ctrlReset);
	DDX_Control(pDX, IDC_LIGELEDISPLAYCTRL2, m_ctrlAlign2);
	DDX_Control(pDX, IDC_GRID_MARKINFO, m_GridMarkInfo);
	DDX_Control(pDX, IDC_LIGHT_H1, m_ctrlLightH1);
	DDX_Control(pDX, IDC_LIGHT_H16, m_ctrlLightH16);
	DDX_Control(pDX, IDC_REGIST_MARK, m_ctrlRegistMark);
	DDX_Control(pDX, IDC_MOVE_JOG, m_ctrlMoveJog);
	DDX_Control(pDX, IDC_MARK_SEL, m_ctrlMarkSel);
	DDX_Control(pDX, IDC_MARK_FIND, m_ctrlMarkFind);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMarkAlignInsp, CDialog)
	//{{AFX_MSG_MAP(CMarkAlignInsp)
	ON_CBN_SELCHANGE(IDC_COMBO_SPD, OnSelchangeComboSpd)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMarkAlignInsp message handlers

BOOL CMarkAlignInsp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	SelectLanguage();
//	this->MoveWindow(0, 1, 1024, 768);
/*#if _VISION
	pView->pVision->m_pDisplay0 = &m_ctrlAlign1;
	pView->pVision->SetDisplay(MUX1);
//	pView->pVision->Live(MUX1);
//	pView->pVision->Grab(MUX1);
 	pView->pVision->CrossDisplay("", 320, 240);
	m_ctrlAlign1.ImageZoom(-2,-2);
	pView->pVision->m_pDisplay1 = &m_ctrlAlign2;
 	pView->pVision->SetDisplay(MUX2);
//	pView->pVision->Live(MUX2);
//	pView->pVision->Grab(MUX2);
 	pView->pVision->CrossDisplay("", 320, 240);
	m_ctrlAlign2.ImageZoom(-2,-2);
#endif*/
	GetDlgItem(IDC_LIGELEDISPLAYCTRL1)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_LIGELEDISPLAYCTRL2)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
//	GetDlgItem(IDC_GRID_MARKINFO)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
#if _VISION
	CRect rect;
	GetClientRect( rect );
	int nSizeX = 640;
	rect.left = 10 + nSizeX/2;
	rect.top = 100;
	rect.right = rect.left + nSizeX/2;
	rect.bottom = rect.top + 480;

	if(!m_ctrlDisplay0.Create("View00",WS_CHILD|WS_VISIBLE,rect,this,DISPLAYVIEWCTRL1))
	{
		AfxMessageBox("Failed to create a display view control");
		PostMessage(WM_CLOSE);
		return FALSE;
	}	
	ASSERT(m_ctrlDisplay0);
	
	pView->pVision->m_pDisplay0 = &m_ctrlDisplay0;
	pView->pVision->SetDisplay(MUX1);
	pView->pVision->CrossDisplay("Head # 1",320,240);
	pView->pVision->Live(MUX1);
	

	rect.left = 10;
	rect.top = 100;
	rect.right = rect.left + nSizeX/2;
	rect.bottom = rect.top + 480;

	if(!m_ctrlDisplay1.Create("View01",WS_CHILD|WS_VISIBLE,rect,this,DISPLAYVIEWCTRL2))
	{
		AfxMessageBox("Failed to create a display view control");
		PostMessage(WM_CLOSE);
		return FALSE;
	}
	ASSERT(m_ctrlDisplay1);
	pView->pVision->m_pDisplay1 = &m_ctrlDisplay1;
	pView->pVision->SetDisplay(MUX2);
	pView->pVision->CrossDisplay("Head #16", 320,240);
	pView->pVision->Live(MUX2);

	pView->pVision->m_nPatternID1 = MUX1;
	pView->pVision->m_nPatternID2 = MUX2;
	pView->pVision->m_bAlignComplete = FALSE;
#endif


	int i;
	CString str;
	
	for(i=0;i<2;i++)
	{
		pView->m_dblMarkX[i]=0.0;
		pView->m_dblMarkY[i]=0.0;
		pView->m_dblAngle[i]=0.0;
		pView->m_lScore[i]=0;
	}

	// combo 폰트와 높이 설정 //////////////////////////////////////////////
	m_fontComboSpd->CreateFont(24,10,0,0,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,
	OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY, 
	DEFAULT_PITCH,"Tahoma");

	m_comboSpd.SetFont(m_fontComboSpd, TRUE);
	m_comboSpd.SetItemHeight(-1, 38);//Spd 1,1,5,10,50,100
	for( i=0;i<6;i++)
	{
		m_comboSpd.SetItemHeight(i,38);
		str.Format("%.1f",m_nSpd[i]);
		m_comboSpd.AddString((LPCSTR)str);
	}
	m_comboSpd.SetCurSel(0);

	str.Format("%s|%s", pDoc->m_structOperatorModeRecipeData.strFrontRecipeName, pDoc->m_structOperatorModeRecipeData.strBackRecipeName);
	SetDlgItemText(IDC_LABEL_RECIPENAME, str);

	str.Format("%d",pDoc->m_structDataEditor.m_nCamLightSet[1]);
	m_ctrlLightH16.SetCaption(str);
	str.Format("%d",pDoc->m_structDataEditor.m_nCamLightSet[0]);
	m_ctrlLightH1.SetCaption(str);

	m_nCamSel = 2;
	m_nCamChannel = 2;
	UpdateData(FALSE);
	m_ctrlRegistMark.SetCaption("MARK   등록");
	m_ctrlRegistMark.SetEnabled(FALSE);
	m_ctrlMoveJog.SetEnabled(FALSE);
	m_ctrlMarkFind.SetEnabled(FALSE);
	m_ctrlMarkSel.SetCaption("ALL");

	m_nInfoDisplay = 3;
	SetGrid(m_nInfoDisplay);

	pView->m_pDevice->SetLightBright(MUX1,pDoc->m_structDataEditor.m_nCamLightSet[MUX1]);
	pView->m_pDevice->SetLightBright(MUX2,pDoc->m_structDataEditor.m_nCamLightSet[MUX2]);

	GetDlgItem(IDCANCEL)->MoveWindow(0,0,0,0);
	GetDlgItem(IDOK)->MoveWindow(0,0,0,0);



	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_EVENTSINK_MAP(CMarkAlignInsp, CDialog)
    //{{AFX_EVENTSINK_MAP(CMarkAlignInsp)
	ON_EVENT(CMarkAlignInsp, IDC_RETURN, -600 /* Click */, OnClickReturn, VTS_NONE)
	ON_EVENT(CMarkAlignInsp, IDC_REGIST_MARK, -600 /* Click */, OnClickRegistMark, VTS_NONE)
	ON_EVENT(CMarkAlignInsp, IDC_MOVE_MARK_POS, -600 /* Click */, OnClickMoveMarkPos, VTS_NONE)
	ON_EVENT(CMarkAlignInsp, IDC_MARK_ALIGN, -600 /* Click */, OnClickMarkAlign, VTS_NONE)
	ON_EVENT(CMarkAlignInsp, IDC_MARK_FIND, -600 /* Click */, OnClickMarkFind, VTS_NONE)
	ON_EVENT(CMarkAlignInsp, IDC_MARK_SAVE, -600 /* Click */, OnClickMarkSave, VTS_NONE)
	ON_EVENT(CMarkAlignInsp, IDC_MOVE_JOG, -600 /* Click */, OnClickMoveJog, VTS_NONE)
	ON_EVENT(CMarkAlignInsp, IDC_LIGHT_H16, -600 /* Click */, OnClickLightH16, VTS_NONE)
	ON_EVENT(CMarkAlignInsp, IDC_LIGHT_H1, -600 /* Click */, OnClickLightH1, VTS_NONE)
	ON_EVENT(CMarkAlignInsp, IDC_MARK_SEL, -600 /* Click */, OnClickMarkSel, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CMarkAlignInsp::OnClickReturn() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	
	delete m_fontComboSpd;
#if _VISION
	pView->pVision->CloseDisplay(MUX1);
	m_ctrlAlign1.Close();

	pView->pVision->CloseDisplay(MUX2);
	m_ctrlAlign2.Close();

//	pView->pVision->CloseDisplay(MUX1);
	m_ctrlDisplay0.Close();

//	pView->pVision->CloseDisplay(MUX2);
	m_ctrlDisplay1.Close();
#endif
	CDialog::EndDialog(IDOK);
}

void CMarkAlignInsp::SetGrid(int ViewType)
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

// 			0	1	2	3	4
// 0	Align Result			
// 1			Dx	Dy	Score
// 2	Mark#1			
// 3	Mark#2			
// 4	Teaching Result			
// 5		X	Y	Score Brightness
// 6	Mark#1 Pos	
// 7	Mark#2 Pos		
// 8 	Current Pos			
// 9		X	Y	K
// 10	Head#1			
// 11	Head#16			

	m_GridMarkInfo.SetRows(18);
	m_GridMarkInfo.SetCols(3);

	m_GridMarkInfo.ShowWindow(FALSE);
	int i,j;
	for(i=0;i<m_GridMarkInfo.GetRows();i++)
	{
		m_GridMarkInfo.SetRowHeight(i,380);
		for(j=0;j<m_GridMarkInfo.GetCols();j++)
		{
			m_GridMarkInfo.SetColAlignment(j,4);
			if(j==0)
				m_GridMarkInfo.SetColWidth(j,1900);
			else
				m_GridMarkInfo.SetColWidth(j,1550);
			m_GridMarkInfo.SetRow(0);
			m_GridMarkInfo.SetCol(j);
			m_GridMarkInfo.SetCellBackColor(LIGHTGRAY);
			m_GridMarkInfo.SetRow(1);
			m_GridMarkInfo.SetCol(j);
			m_GridMarkInfo.SetCellBackColor(LIGHTGRAY);
			m_GridMarkInfo.SetRow(4);
			m_GridMarkInfo.SetCol(j);
			m_GridMarkInfo.SetCellBackColor(LIGHTGRAY);
			m_GridMarkInfo.SetRow(7);
			m_GridMarkInfo.SetCol(j);
			m_GridMarkInfo.SetCellBackColor(LIGHTGRAY);
			m_GridMarkInfo.SetRow(8);
			m_GridMarkInfo.SetCol(j);
			m_GridMarkInfo.SetCellBackColor(LIGHTGRAY);
			m_GridMarkInfo.SetRow(11);
			m_GridMarkInfo.SetCol(j);
			m_GridMarkInfo.SetCellBackColor(LIGHTGRAY);
			m_GridMarkInfo.SetRow(14);
			m_GridMarkInfo.SetCol(j);
			m_GridMarkInfo.SetCellBackColor(LIGHTGRAY);
			m_GridMarkInfo.SetRow(15);
			m_GridMarkInfo.SetCol(j);
			m_GridMarkInfo.SetCellBackColor(LIGHTGRAY);
		}
	}
	m_GridMarkInfo.SetTextMatrix(0,0,"Align Result");
	m_GridMarkInfo.SetTextMatrix(1,1,"Dx");
	m_GridMarkInfo.SetTextMatrix(1,2,"Dy");
	m_GridMarkInfo.SetTextMatrix(2,0,"Head#1");
	m_GridMarkInfo.SetTextMatrix(3,0,"Head#16");
	m_GridMarkInfo.SetTextMatrix(4,1,"Score");
	m_GridMarkInfo.SetTextMatrix(5,0,"Head#1");
	m_GridMarkInfo.SetTextMatrix(6,0,"Head#16");


	m_strAlignData.Empty();
	m_GridMarkInfo.SetTextMatrix(2,1,"--");
	m_GridMarkInfo.SetTextMatrix(2,2,"--");
	m_GridMarkInfo.SetTextMatrix(5,1,"--");
	m_GridMarkInfo.SetTextMatrix(3,1,"--");
	m_GridMarkInfo.SetTextMatrix(3,2,"--");
	m_GridMarkInfo.SetTextMatrix(6,1,"--");

	m_GridMarkInfo.SetTextMatrix(7,0,"Mark 위치");
	m_GridMarkInfo.SetTextMatrix(8,1,"X");
	m_GridMarkInfo.SetTextMatrix(8,2,"Y");
	m_GridMarkInfo.SetTextMatrix(9,0,"Head#1");
	m_GridMarkInfo.SetTextMatrix(10,0,"Head#16");
	m_GridMarkInfo.SetTextMatrix(11,1,"Score");
	m_GridMarkInfo.SetTextMatrix(11,2,"조명밝기");
	m_GridMarkInfo.SetTextMatrix(12,0,"Head#1");
	m_GridMarkInfo.SetTextMatrix(13,0,"Head#16");

	m_strTeachingData.Empty();
	m_strTeachingData.Format("%.2f",pDoc->m_structDataEditor.m_dMarkX[0]);
	m_GridMarkInfo.SetTextMatrix(9,1,m_strTeachingData);
	m_strTeachingData.Format("%.2f",pDoc->m_structDataEditor.m_dMarkY[0]);
	m_GridMarkInfo.SetTextMatrix(9,2,m_strTeachingData);

	m_strTeachingData.Format("%d",pDoc->m_structDataEditor.m_nCamScore);
	m_GridMarkInfo.SetTextMatrix(12,1,m_strTeachingData);
	m_strTeachingData.Format("%d",pDoc->m_structDataEditor.m_nCamLightSet[0]);
	m_GridMarkInfo.SetTextMatrix(12,2,m_strTeachingData);

	m_strTeachingData.Format("%.2f",pDoc->m_structDataEditor.m_dMarkX[1]);
	m_GridMarkInfo.SetTextMatrix(10,1,m_strTeachingData);
	m_strTeachingData.Format("%.2f",pDoc->m_structDataEditor.m_dMarkY[1]);
	m_GridMarkInfo.SetTextMatrix(10,2,m_strTeachingData);

	m_strTeachingData.Format("%d",pDoc->m_structDataEditor.m_nCamScore);
	m_GridMarkInfo.SetTextMatrix(13,1,m_strTeachingData);
	m_strTeachingData.Format("%d",pDoc->m_structDataEditor.m_nCamLightSet[1]);
	m_GridMarkInfo.SetTextMatrix(13,2,m_strTeachingData);

	m_GridMarkInfo.SetTextMatrix(14,0,"Current Pos");
	m_GridMarkInfo.SetTextMatrix(15,1,"S(X)");
	m_GridMarkInfo.SetTextMatrix(15,2,"K(Col)");
//	m_GridMarkInfo.SetTextMatrix(9,3,"-");
	m_GridMarkInfo.SetTextMatrix(16,0,"Head#1");
	m_GridMarkInfo.SetTextMatrix(17,0,"Head#16");
	
	m_strCurrentData.Empty();
	m_GridMarkInfo.SetTextMatrix(16,1,"--");
	m_GridMarkInfo.SetTextMatrix(16,2,"--");
//	m_GridMarkInfo.SetTextMatrix(10,3,"--");
	m_GridMarkInfo.SetTextMatrix(17,1,"--");
	m_GridMarkInfo.SetTextMatrix(17,2,"--");
//	m_GridMarkInfo.SetTextMatrix(11,3,"--");

	switch(m_nInfoDisplay)
	{
	case 0://Mark Align 값 
		m_strAlignData.Format("%f",pView->m_dblMarkX[0]);
		m_GridMarkInfo.SetTextMatrix(2,1,m_strAlignData);
		m_strAlignData.Format("%f",pView->m_dblMarkY[0]);
		m_GridMarkInfo.SetTextMatrix(2,2,m_strAlignData);
		m_strAlignData.Format("%d",pView->m_lScore[0]);
		m_GridMarkInfo.SetTextMatrix(5,1,m_strAlignData);

		m_strAlignData.Format("%f",pView->m_dblMarkX[1]);
		m_GridMarkInfo.SetTextMatrix(3,1,m_strAlignData);
		m_strAlignData.Format("%f",pView->m_dblMarkY[1]);
		m_GridMarkInfo.SetTextMatrix(3,2,m_strAlignData);
		m_strAlignData.Format("%d",pView->m_lScore[1]);
		m_GridMarkInfo.SetTextMatrix(6,1,m_strAlignData);
		break;
	case 1://Current Data(Teaching Data)
		m_strCurrentData.Format("%f",m_dCurrPos_S[0]);
		m_GridMarkInfo.SetTextMatrix(16,1,m_strCurrentData);
		m_strCurrentData.Format("%f",m_dCurrPos_K);
		m_GridMarkInfo.SetTextMatrix(16,2,m_strCurrentData);
		m_strCurrentData.Format("%f",m_dCurrPos_S[15]);
		m_GridMarkInfo.SetTextMatrix(17,1,m_strCurrentData);
		m_strCurrentData.Format("%f",m_dCurrPos_K);
		m_GridMarkInfo.SetTextMatrix(17,2,m_strCurrentData);
		break;
	case 3:
		break;
	}
	m_GridMarkInfo.ShowWindow(TRUE);
}

void CMarkAlignInsp::OnSelchangeComboSpd() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	UpdateData(TRUE);
	int ComboCount = m_comboSpd.GetCurSel();
	m_nJogSpd = m_nSpd[ComboCount];

	int ihead=0, ibdid=0, iaxis=0;
	long lParaNo = 8;	// Speed Setting Parameter No.=> 8

	FAS_SetLocalPara(AXIS_K_MASTER/BOARD_AXES+1, AXIS_K_MASTER%BOARD_AXES, lParaNo, m_nJogSpd*K_PULSE); //K0
	for(iaxis = 0 ; iaxis < 2 ; iaxis++) // k1 ~ k2
	{
		FAS_SetLocalPara(AXIS_K1/BOARD_AXES+1, iaxis, lParaNo, m_nJogSpd*K_PULSE);
	}
	for(ihead = 0 ; ihead < MAX_NOZZLE ; ihead++)
	{
		FAS_SetLocalPara(pDoc->m_structHeadConfig[ihead].nS[0], pDoc->m_structHeadConfig[ihead].nS[1], lParaNo, m_nJogSpd*S_PULSE);
	}
}

void CMarkAlignInsp::OnClickRegistMark() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	
	LPBYTE fm;
	fm=(LPBYTE)grabber.GetImagePtr(m_nCamChannel);
	pView->pVision->m_pMark->PMSetSearchFrame((long)fm);
	if(m_nCamChannel == MUX1)
	{
		pView->pVision->m_pMark->PMDialog(MUX1, (long)m_ctrlDisplay0.m_hWnd, 640,480,0,0,1.0);
	}
	else if(m_nCamChannel == MUX2)
	{
		pView->pVision->m_pMark->PMDialog(MUX2, (long)m_ctrlDisplay1.m_hWnd, 640,480,0,0,1.0);
	}
}

void CMarkAlignInsp::OnClickMoveMarkPos() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	// TODO: Add your control notification handler code here
	CNormalMsg m_NormalMsg;
	ThreadStage.PtpMoveCode='o';
	m_NormalMsg.m_strMsg1 = "Mark Align 정도 확인 위치로 움직입니다"; 
	m_NormalMsg.m_bTimer=TRUE;
	m_NormalMsg.m_strTitle="PTP 구동";
	m_NormalMsg.m_strMsg2 = " 실행하려면 OK버튼을, 취소하려면 방향버튼을 누르시오.";
	m_NormalMsg.DoModal();   
}

void CMarkAlignInsp::OnClickMarkAlign() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	
	pView->KillThread(THREAD_STAGE);
	pView->pVision->m_bAlignComplete = FALSE;
	ThreadStage.ManualCode = 'K'; 
	ThreadStage.ManualJobStep = 150;
	ThreadStage.JobFlag = STAGE_MANUAL;
	pView->RunThread(THREAD_STAGE);
	Sleep(100);
	SetTimer(1,100,NULL);
}

void CMarkAlignInsp::OnClickMarkFind() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	if(m_nCamSel == 2) return;

//	int i;
// 	for(i=0;i<2;i++)
// 	{
// 		pView->m_dblMarkX[i]=0.0;
// 		pView->m_dblMarkY[i]=0.0;
// 		pView->m_dblAngle[i]=0.0;
// 		pView->m_lScore[i]=0;
// 	}

//	for(i=0;i<2;i++)
//	{
		pView->pVision->Grab(m_nCamSel);
		Sleep(300);
		if(pView->pVision->FindAlignMark(m_nCamSel, pDoc->m_structDataEditor.m_nCamScore, TRUE))
		{		
			pView->m_dblMarkX[m_nCamSel] = pView->pVision->m_dblMarkX;
			pView->m_dblMarkY[m_nCamSel] = pView->pVision->m_dblMarkY;
			pView->m_lScore[m_nCamSel] = pView->pVision->m_lScore;
			pView->m_dblAngle[m_nCamSel] = pView->pVision->m_dblAngle;
		}
		CString str;
		str.Format("Mark#%d dx:%f, dy:%f",m_nCamSel+1, pView->m_dblMarkX[m_nCamSel], pView->m_dblMarkY[m_nCamSel]);
		SetDlgItemText(IDC_LABEL_MSG, str);
		
		str.Format("CNT");
		pView->pVision->SetChannel(m_nCamSel);
		pView->pVision->CrossDisplay(str, 320,240);
//	}
	m_nInfoDisplay = 0;
	SetGrid(m_nInfoDisplay);
}

void CMarkAlignInsp::OnClickMarkSave() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	
	ButtonControl(FALSE);
	CString str;
	str.Format("D:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\"+pDoc->m_structOperatorModeRecipeData.strFrontRecipeName+"\\AlignMark.dat");
	pView->pVision->m_pMark->PMSave(str);
	ButtonControl(TRUE);	
}

void CMarkAlignInsp::OnClickMoveJog() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

//	if(ThreadStage.JogAxis == 0) return; // 선택된 axis가 존재하지 않는다..
	//
	if(m_nCamChannel == 0)
	{
		ThreadStage.HomeSearchMotor = 4;
		ThreadStage.HomeSearchBD = 2;
	}
	else if(m_nCamChannel == 1)
	{
		ThreadStage.HomeSearchMotor = 3;
		ThreadStage.HomeSearchBD = 3;
	}

	ThreadStage.ManualCode = 'J';
	ThreadStage.JogAxis = 'H';

	if(PC_TYPE == TRUE)
	{
		ThreadStage.JobFlag = STAGE_MANUAL; // manual
		ThreadStage.ExitFlag = FALSE;
		pView->RunThread(THREAD_STAGE);	// Stage Manual Thread Start
		m_nTimerCount = 0;
		//
		while(1)
		{
			if(ThreadStage.bMachineRunning == TRUE) 
			{
				ButtonControl(FALSE);
				m_ctrlMoveJog.SetBackColor(LIGHTRED);
				SetTimer(0,100,NULL);
				break;
			}
			pFrame->DoEvents();
		}
	}
	else SetTimer(0,100,NULL);		
}

void CMarkAlignInsp::OnClickLightH16() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlLightH16, 255, 0, 400, 200); // by ckh 2009.01.31
	int TempLightH16 = atoi(m_ctrlLightH16.GetCaption());
	pView->m_pDevice->SetLightBright(MUX2,TempLightH16);
}

void CMarkAlignInsp::OnClickLightH1() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlLightH1, 255, 0, 400, 200); // by ckh 2009.01.31
	int TempLightH1 = atoi(m_ctrlLightH1.GetCaption());	
	pView->m_pDevice->SetLightBright(MUX1,TempLightH1);
}

void CMarkAlignInsp::ButtonControl(BOOL bCtrl)
{
	GetDlgItem(IDC_COMBO_SPD)->EnableWindow(bCtrl);
	GetDlgItem(IDC_RESET)->EnableWindow(bCtrl);
//	GetDlgItem(IDC_REGIST_MARK)->EnableWindow(bCtrl);
	GetDlgItem(IDC_MARK_ALIGN)->EnableWindow(bCtrl);
	GetDlgItem(IDC_RETURN)->EnableWindow(bCtrl);
	GetDlgItem(IDC_MARK_SAVE)->EnableWindow(bCtrl);
	GetDlgItem(IDC_MOVE_MARK_POS)->EnableWindow(bCtrl);
	GetDlgItem(IDC_MARK_SEL)->EnableWindow(bCtrl);
//	GetDlgItem(IDC_MOVE_JOG)->EnableWindow(bCtrl);
//	GetDlgItem(IDC_MARK_FIND)->EnableWindow(bCtrl);
}

void CMarkAlignInsp::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();

	double dPos = 0;
	int i;
	m_nTimerCount++;

	///////////////////////////
	/// botton on-off
	switch(nIDEvent)
	{
	case 0 : // jog
		if(pView->pVision->m_bAlignComplete)
			KillTimer(0);
		for(i=0 ; i < MAX_NOZZLE ; i++)
		{
			FAS_GetActualPos(pDoc->m_structHeadConfig[i].nS[0],pDoc->m_structHeadConfig[i].nS[1],&dPos);
			//str.Format("%.2f",dPos /= S_PULSE);
			//GetDlgItem(IDC_LABEL_S1 + i)->SetWindowText(str);
			m_dCurrPos_S[i] = dPos / S_PULSE;
		}
		// K1
		FAS_GetActualPos(AXIS_K1/BOARD_AXES+1,AXIS_K1%BOARD_AXES,&dPos);
		//str.Format("%.2f",dPos /= K_PULSE);
		//GetDlgItem(IDC_LABEL_Y)->SetWindowText(str);
		m_dCurrPos_K = dPos / K_PULSE;

		m_nInfoDisplay = 1;
		SetGrid(m_nInfoDisplay);
		if(m_nTimerCount <= 5) 
			m_ctrlMoveJog.SetBackColor(WHITEGRAY);
		else
			m_ctrlMoveJog.SetBackColor(LIGHTRED);

		if(m_nTimerCount > 10)
		{
			m_nTimerCount = 0;
		}

		if(ThreadStage.bMachineRunning == FALSE)
		{
			KillTimer(0);
			m_ctrlMoveJog.SetBackColor(WHITEGRAY);
			ButtonControl(TRUE);
		}
		else
		{

		}
		pFrame->DoEvents();
		break;
	case 1 : //Mark Align
		if(pView->pVision->m_bAlignComplete)
		{
			KillTimer(1);
			ButtonControl(TRUE);
		}
		else
		{
			ButtonControl(FALSE);
		}
		if(ThreadStage.bEmergencyStop || ThreadStage.ExitFlag)
		{
			KillTimer(1);
			ButtonControl(TRUE);
		}
		m_nInfoDisplay = 0;
		SetGrid(m_nInfoDisplay);
		SetDlgItemText(IDC_LABEL_MSG,ThreadStage.strMsg1);

		break;
	case 2 : 
		break;
	case 3:
		break;
	} // End of switch(nIDEvent)	
	CDialog::OnTimer(nIDEvent);
}

void CMarkAlignInsp::OnClickMarkSel() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	// TODO: Add your control notification handler code here
	if(m_nCamChannel == 0)
	{
		m_nCamSel = 3;
	}

	m_nCamSel--;
	
	if(m_nCamSel == 2)
	{
		m_ctrlRegistMark.SetEnabled(FALSE);
		m_ctrlMoveJog.SetEnabled(FALSE);
		m_ctrlMarkFind.SetEnabled(FALSE);
	}
	else
	{
		m_ctrlRegistMark.SetEnabled(TRUE);
		m_ctrlMoveJog.SetEnabled(TRUE);
		m_ctrlMarkFind.SetEnabled(TRUE);
	}
	m_ctrlDisplay0.MoveWindow(0,0,0,0);
	m_ctrlDisplay1.MoveWindow(0,0,0,0);
	if(m_nCamSel == 0)//Mark1
	{
		m_ctrlMarkSel.SetCaption("MARK#1");
		m_ctrlDisplay0.MoveWindow(10,100,640,480);
		m_ctrlDisplay1.MoveWindow(0,0,0,0);
		pView->pVision->SetDisplay(MUX1);
		pView->pVision->CrossDisplay("Head #1", 320,240);
		pView->pVision->Live(MUX1);
		m_nCamChannel = 0;
//		m_nCamSel = 0;
	}
	else if(m_nCamSel == 1)//Mark2
	{
		m_ctrlMarkSel.SetCaption("MARK#2");
		m_ctrlDisplay0.MoveWindow(0,0,0,0);
		m_ctrlDisplay1.MoveWindow(10,100,640,480);
		pView->pVision->SetDisplay(MUX2);
		pView->pVision->CrossDisplay("Head #16", 320,240);
		pView->pVision->Live(MUX2);
		m_nCamChannel = 1;
//		m_nCamSel = 1;
	}
	else if(m_nCamSel == 2)//ALL
	{
		m_ctrlMarkSel.SetCaption("ALL");
		m_ctrlDisplay0.MoveWindow(330,100,320,480);
		pView->pVision->SetDisplay(MUX1);
		pView->pVision->CrossDisplay("Head #1", 320,240);
		pView->pVision->Live(MUX1);
		m_ctrlDisplay1.MoveWindow(10,100,320,480);
		pView->pVision->SetDisplay(MUX2);
		pView->pVision->CrossDisplay("Head #16", 320,240);
		pView->pVision->Live(MUX2);
		m_nCamChannel = 2;
//		m_nCamSel = 2;
	}
}

void CMarkAlignInsp::SelectLanguage()
{

	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(pView->m_nLanguage == 0)	//ehji 140309
	{
		SetDlgItemText(IDC_MARK_SAVE,				_T("MARK 저장"));
		SetDlgItemText(IDC_MOVE_MARK_POS,				_T("Mark 위치 이동"));
		SetDlgItemText(IDC_MARK_FIND,				_T("Mark 찾기"));
		SetDlgItemText(IDC_MARK_ALIGN,				_T("MARK ALIGN 정도 확인"));
		SetDlgItemText(IDC_REGIST_MARK,				_T("MARK   등록(#1)"));
	}

	else if(pView->m_nLanguage == 1)	//ehji 140309
	{
		SetDlgItemText(IDC_MARK_SAVE,				_T("MARK Save"));
		SetDlgItemText(IDC_MOVE_MARK_POS,				_T("Mark Position Move"));
		SetDlgItemText(IDC_MARK_FIND,				_T("Mark Find"));
		SetDlgItemText(IDC_MARK_ALIGN,				_T("MARK ALIGN Level Check"));
		SetDlgItemText(IDC_REGIST_MARK,				_T("MARK Regist(#1)"));
	}

	else if(pView->m_nLanguage == 2)	//ehji 140309
	{
		SetDlgItemText(IDC_MARK_SAVE,				_T("MARK 保存"));
		SetDlgItemText(IDC_MOVE_MARK_POS,				_T("Mark 移動位置"));
		SetDlgItemText(IDC_MARK_FIND,				_T("Mark 찾기"));
		SetDlgItemText(IDC_MARK_ALIGN,				_T("MARK ALIGN 精度確認"));
		SetDlgItemText(IDC_REGIST_MARK,				_T("MARK   登陸(#1)"));
	}

}

// ManuMeasWidth.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "ManuMeasWidth.h"

#include "MainFrm.h"
#include "P8CA_LcDispView.h"
#include "NormalMsg.h"

#include "Color.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


////////////////////////////////////////////////////////////////////////////////////////////
// by ckh 
#include "Define_List.h"
extern PosChk_Data PosChk_Datas[MAX_NOZZLE];				// PosChk를 위한 각 축별 데이터
extern PosChk_ThreadData PosChk_Thread_Datas;
extern THREADSTAGE ThreadStage;

//2013.08.03 by tskim
extern BOOL g_bMMGLineDropMode;
/////////////////////////////////////////////////////////////////////////////
// CManuMeasWidth dialog


CManuMeasWidth::CManuMeasWidth(CWnd* pParent /*=NULL*/)
	: CDialog(CManuMeasWidth::IDD, pParent)
{
	//{{AFX_DATA_INIT(CManuMeasWidth)
	m_bFirstChk = FALSE;
	//}}AFX_DATA_INIT

//	m_fontCombo = new CFont;
	m_nViewMode = 2;	// 2 -> 두 화면 모두 보기
						// 0 -> Mux 1 보기
						// 1 -> Mux 2 보기

	for(int i = 0; i < MAX_NOZZLE; i++)
	{
		m_dXPointCam[i] = 0.0;
		m_dYPointCam[i] = 0.0;
		m_dDistanceCamX[i] = 0.0;
		m_dDistanceCamY[i] = 0.0;
	}
	m_nHeadSelectNo = -1;
}

CManuMeasWidth::~CManuMeasWidth()
{
	m_fontCombo.DeleteObject();
}

void CManuMeasWidth::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CManuMeasWidth)
	DDX_Control(pDX, IDC_CHK_FIRSTCHK, m_ctrlFirstChk);
	DDX_Control(pDX, IDC_SPINLIGHTLEVEL, m_ctrlSpenLightLevel);
	DDX_Control(pDX, IDC_CMD_HEAD1, m_ctrlHead1);
	DDX_Control(pDX, IDC_CMD_HEAD10, m_ctrlHead10);
	DDX_Control(pDX, IDC_CMD_HEAD11, m_ctrlHead11);
	DDX_Control(pDX, IDC_CMD_HEAD12, m_ctrlHead12);
	DDX_Control(pDX, IDC_CMD_HEAD13, m_ctrlHead13);
	DDX_Control(pDX, IDC_CMD_HEAD14, m_ctrlHead14);
	DDX_Control(pDX, IDC_CMD_HEAD15, m_ctrlHead15);
	DDX_Control(pDX, IDC_CMD_HEAD16, m_ctrlHead16);
	DDX_Control(pDX, IDC_CMD_HEAD2, m_ctrlHead2);
	DDX_Control(pDX, IDC_CMD_HEAD3, m_ctrlHead3);
	DDX_Control(pDX, IDC_CMD_HEAD4, m_ctrlHead4);
	DDX_Control(pDX, IDC_CMD_HEAD5, m_ctrlHead5);
	DDX_Control(pDX, IDC_CMD_HEAD6, m_ctrlHead6);
	DDX_Control(pDX, IDC_CMD_HEAD7, m_ctrlHead7);
	DDX_Control(pDX, IDC_CMD_HEAD8, m_ctrlHead8);
	DDX_Control(pDX, IDC_CMD_HEAD9, m_ctrlHead9);
	DDX_Control(pDX, IDC_CMD_MANUAL_JOG, m_ctrlJog);
	DDX_Control(pDX, IDC_CMD_MOVE_POS1, m_ctrlMovePos1);
	DDX_Control(pDX, IDC_CMD_MOVE_POS2, m_ctrlMovePos2);
	DDX_Control(pDX, IDC_LABEL_CENTER_X, m_ctrlCenterPosX);
	DDX_Control(pDX, IDC_LABEL_CENTER_Y, m_ctrlCenterPosY);
	DDX_Control(pDX, IDC_LABEL_DIST_X, m_ctrlDistX);
	DDX_Control(pDX, IDC_LABEL_DIST_Y, m_ctrlDistY);
	DDX_Check(pDX, IDC_CHK_FIRSTCHK, m_bFirstChk);
	DDX_Control(pDX, IDC_CMD_POSCHK, m_ctrlPosChk);
	DDX_Control(pDX, IDC_CMD_SAVE, m_ctrlCmdSave);
	DDX_Control(pDX, IDC_CMD_MARK_SEARCH, m_ctrlMarkSearch);
	DDX_Control(pDX, IDC_CMD_LC_SEARCH, m_ctrlLCSearch);
	DDX_Control(pDX, IDC_LAB_NAME, m_ctrlLabName);
	DDX_Control(pDX, IDC_LAB_LIGHTLEVEL, m_ctrlLabLightBright);
	DDX_Control(pDX, IDC_CMD_ALIGN_SEARCH, m_ctrlAlignSearch);
	DDX_Control(pDX, IDC_CMD_TEACHINGDATA, m_ctrlTeachData);
	DDX_Control(pDX, IDC_CMD_TEACHING_ALIGNMARK, m_ctrlTeachingAling);
	DDX_Control(pDX, IDC_CMD_PMLOAD, m_ctrlPMLoad);
	DDX_Control(pDX, IDC_CMD_PMSAVE, m_ctrlPMSave);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CManuMeasWidth, CDialog)
	//{{AFX_MSG_MAP(CManuMeasWidth)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHK_FIRSTCHK, OnChkFirstchk)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPINLIGHTLEVEL, OnDeltaposSpinlightlevel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CManuMeasWidth message handlers

BEGIN_EVENTSINK_MAP(CManuMeasWidth, CDialog)
    //{{AFX_EVENTSINK_MAP(CManuMeasWidth)
	ON_EVENT(CManuMeasWidth, IDC_CMD_PREV, -600 /* Click */, OnClickCmdPrev, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_MEAS, -600 /* Click */, OnClickCmdMeas, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_MANUAL_JOG, -600 /* Click */, OnClickCmdManualJog, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_HEAD1, -600 /* Click */, OnClickCmdHead1, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_HEAD2, -600 /* Click */, OnClickCmdHead2, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_HEAD3, -600 /* Click */, OnClickCmdHead3, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_HEAD4, -600 /* Click */, OnClickCmdHead4, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_HEAD5, -600 /* Click */, OnClickCmdHead5, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_HEAD6, -600 /* Click */, OnClickCmdHead6, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_HEAD7, -600 /* Click */, OnClickCmdHead7, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_HEAD8, -600 /* Click */, OnClickCmdHead8, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_HEAD9, -600 /* Click */, OnClickCmdHead9, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_HEAD10, -600 /* Click */, OnClickCmdHead10, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_HEAD11, -600 /* Click */, OnClickCmdHead11, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_HEAD12, -600 /* Click */, OnClickCmdHead12, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_HEAD13, -600 /* Click */, OnClickCmdHead13, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_HEAD14, -600 /* Click */, OnClickCmdHead14, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_HEAD15, -600 /* Click */, OnClickCmdHead15, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_HEAD16, -600 /* Click */, OnClickCmdHead16, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_SAVE, -600 /* Click */, OnClickCmdSave, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_BLOB, -600 /* Click */, OnClickCmdBlob, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_MOVE_POS2, -600 /* Click */, OnClickCmdMovePos2, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_MOVE_POS1, -600 /* Click */, OnClickCmdMovePos1, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_POSCHK, -600 /* Click */, OnClickCmdPoschk, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_MARK_SEARCH, -600 /* Click */, OnClickCmdMarkSearch, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_LC_SEARCH, -600 /* Click */, OnClickCmdLcSearch, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_LAB_LIGHTLEVEL, -600 /* Click */, OnClickLabLightlevel, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_TEACHINGDATA, -600 /* Click */, OnClickCmdTeachingdata, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_POSCHK_RESULT, -600 /* Click */, OnClickCmdPoschkResult, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_ALIGN_SEARCH, -600 /* Click */, OnClickCmdAlignSearch, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_TEACHING_ALIGNMARK, -600 /* Click */, OnClickCmdTeachingAlignmark, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_PMLOAD, -600 /* Click */, OnClickCmdPmload, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_PMSAVE, -600 /* Click */, OnClickCmdPmsave, VTS_NONE)
	ON_EVENT(CManuMeasWidth, IDC_CMD_MARK_ALIGN, -600 /* Click */, OnClickCmdMarkAlign, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

BOOL CManuMeasWidth::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	this->MoveWindow(0, 1, 1024, 768);
	// TODO: Add extra initialization here
	
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	CString str;
	SetDlgItemText(IDC_LABEL_RECIPE, str);

	SelectLanguage();

	// combo 폰트와 높이 설정 //////////////////////////////////////////////
	m_fontCombo.CreateFont(24,10,0,0,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,
	OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY, 
	DEFAULT_PITCH,"Tahoma");

//2013.08.03 by tskim
	 g_bMMGLineDropMode = pDoc->m_structDataEditor.m_bMMGLineDropMode;

	/////////////////////////////////////////////////////////
	// by ckh 2009.01.12
	int i =0;
	BOOL bResult =	pView->m_pDevice->GetPosChkTeachingData(PosChk_Datas);// 정위치 티칭데이터 가져오기.(Spec 사양, 마크 사이즈 , 마크 위치 등)
	if(bResult != TRUE)
	{
		for(i=0;i< MAX_NOZZLE;i++)
			PosChk_Datas[i].nLightVal = 100;
	}
	m_bButExect = TRUE;
	for(i =0; i<MAX_NOZZLE; i++)
	{
//		if(i==0 || i == 15)
//			PosChk_Datas[i].bSel = TRUE;
//		else
			PosChk_Datas[i].bSel = FALSE;
		
		if(PosChk_Datas[i].bSel)
			SetBut_HeadColor(i,GREEN);
		// by ckh 2009.01.12
		if(PosChk_Datas[i].bLightUse == TRUE)
		{
			if(i==0)
			{
				pView->m_pDevice->SetLightBright(MUX1,pDoc->m_structDataEditor.m_nCamLightSet[MUX1]);
				PosChk_Datas[i].nLightVal = pDoc->m_structDataEditor.m_nCamLightSet[MUX1];
				m_nLightVal[i] = PosChk_Datas[i].nLightVal;// by ckh 2009.01.31
				Sleep(50);// by ckh 2009.02.10				
			}
			else if(i ==15)
			{
				pView->m_pDevice->SetLightBright(MUX2,pDoc->m_structDataEditor.m_nCamLightSet[MUX2]);
				PosChk_Datas[i].nLightVal = pDoc->m_structDataEditor.m_nCamLightSet[MUX2];
				m_nLightVal[i] = PosChk_Datas[i].nLightVal;// by ckh 2009.01.31
				Sleep(50);// by ckh 2009.02.10	
			}
		}
	}
	/////////////////////////////////////////////////////////


#if _VISION
	CRect rect;
	GetClientRect( rect );
	int nSizeX = 640;
	rect.left = 10;
	rect.top = 200;
	rect.right = rect.left + nSizeX;
	rect.bottom = rect.top + 480;

	pView->m_pDevice->CameraSelect(MUX1);

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
	pView->m_pDevice->CameraSelect(MUX1);
	pView->pVision->Live(MUX1);
	

// 	rect.left = 340;
// 	rect.top = 200;
// 	rect.right = rect.left + 320;
// 	rect.bottom = rect.top + 480;
//2010.03.03 by tskim
// 	rect.left = 0;
// 	rect.top = 0;
// 	rect.right = rect.left + 0;
// 	rect.bottom = rect.top + 0;

	rect.left = 10;
	rect.top = 200;
	rect.right = rect.left + nSizeX;
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
	pView->m_pDevice->CameraSelect(MUX2);
	pView->pVision->Live(MUX2);

	m_ctrlDisplay0.SetWindowPos(NULL, 0,0,640,480,SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	m_ctrlDisplay1.SetWindowPos(NULL, 0,0,0,0,SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );

	////////////////////////////////////////////////////////////////////////////////////////////////
	// by ckh 2009.01.12
	// 화면 처음에 보여주기 위치 추가함. 삭제해도 될것같은데.
// 	PosChk_Datas[HEAD8_CAM-1].bSel = FALSE;
// 	pView->pVision->CrossDisplay("Head # 8",320,240);
// 	if( SetSelCam(HEAD8_CAM-1) == TRUE)
// 	{
// 		m_nHeadSelectNo = HEAD8_CAM-1;
// 		if(PosChk_Datas[HEAD8_CAM-1].bLightUse == TRUE)// by ckh 2009.01.29
// 			pView->m_pDevice->SetLightBright(m_nHeadSelectNo,PosChk_Datas[m_nHeadSelectNo].nLightVal);
// 		str.Format("%d",PosChk_Datas[m_nHeadSelectNo].nLightVal);
// 		m_ctrlLabLightBright.SetCaption(str);
// 	}
	////////////////////////////////////////////////////////////////////////////////////////////////
	pView->pVision->m_pMark->PMLoad("D:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\TFTAlign.dat");
#endif
	
	m_nSelect1 = 0;
	m_nSelect2 = 0;

	pDoc->ReadAccuracyMotorPos();

	// 결과 표시 창
	SetMSFlexGrid();

	m_bProcess = FALSE;
	UpdateData(FALSE);
	m_bLamp = TRUE;


	// 찾을 마크 색깔 초기화 
	PosChk_Thread_Datas.lSearchColor = WHITE; // 찾야할 마크 찾기// 흰색 찾기
	/////////////////////////////////////////////////////////
	
	m_ctrlSpenLightLevel.SetRange(255,1);


	// 엔지니어 모드에서만 활성화됨.
	if(pDoc->m_bIsEngineerMode != TRUE)
	{
		m_ctrlTeachData.EnableWindow(FALSE);
		m_ctrlFirstChk.EnableWindow(FALSE);
		m_ctrlTeachingAling.EnableWindow(FALSE);
		// by ckh 2009.01.31
		m_ctrlPMLoad.EnableWindow(FALSE);
		m_ctrlPMSave.EnableWindow(FALSE);
		m_ctrlCmdSave.EnableWindow(FALSE);
	}

	// by ckh 2009.2.10
	ThreadStage.JobFlag = -1 ;//

	
	GetDlgItem(IDC_CMD_HEAD2)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CMD_HEAD3)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CMD_HEAD4)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CMD_HEAD5)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CMD_HEAD6)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CMD_HEAD7)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CMD_HEAD8)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CMD_HEAD9)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CMD_HEAD10)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CMD_HEAD11)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CMD_HEAD12)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CMD_HEAD13)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CMD_HEAD14)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CMD_HEAD15)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );

	GetDlgItem(IDC_CMD_POSCHK_RESULT2)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_CMD_ALIGN_SEARCH)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );

// 	int HeadCnt = 0;
// 	for(i =0; i<MAX_NOZZLE; i++)
// 	{
// 		if(PosChk_Datas[i].bSel)
// 			HeadCnt++;
// 	}
// 	if(HeadCnt>=2)
// 	{
		GetDlgItem(IDC_CMD_LC_SEARCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_CMD_MARK_SEARCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_CMD_TEACHING_ALIGNMARK)->EnableWindow(FALSE);
// 	}
		
	ThreadStage.nMoveHeadNum = -1;
	for(i=0;i<MAX_NOZZLE;i++)
		PosChk_Datas[i].nSelectPos = -1;
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CManuMeasWidth::SetMSFlexGrid()
{
/*	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	
	m_gridResult.ShowWindow(SW_HIDE);
	int m_nCols = MAX_NOZZLE/2+1;
//	int m_nRows = WIDTH_INDEX+1+1; // AVR 추가
	int i, j, k;
	CString str;

	m_gridResult.SetRows(m_nRows);
	m_gridResult.SetCols(m_nCols);

	m_gridResult.SetColWidth(0, 580);

	for(i=1 ; i<m_nCols ; i++)
	{
		m_gridResult.SetColWidth(i, 700);
		m_nColumn == 0 ? str.Format("L%d", i) : str.Format("R%d", i);
		m_gridResult.SetTextMatrix(0, i, str);
	}	
	m_gridResult.SetRowHeight(0, 500);

	for(i=1 ; i<m_nRows ; i++)
	{
		m_gridResult.SetRowHeight(i, 400);
		i == WIDTH_INDEX+1 ? str = "AVR" : str.Format("P%d", i);
		m_gridResult.SetTextMatrix(i, 0, str);
	}






	// Sequence의 순서대로 작업을 진행해야 할 것임.
	for(k=0 ; k<WIDTH_INDEX ; k++)
	{
		for(i=0 ; i<WIDTH_INDEX ; i++)
		{
			if((pDoc->m_structWidth[i].m_nSeq == k+1)&&(pDoc->m_structWidth[i].m_nAng != 0))
			{
				for(j=0 ; j<HALF_HEAD ; j++)
				{
					if(pDoc->m_structWidth[i].m_bHead[j+m_nColumn])
					{
						m_gridResult.SetRow(i+1);
						m_gridResult.SetCol(j+1);
						m_bCheckMeas[i][j+m_nColumn] ?  m_gridResult.SetCellBackColor(_CLICKCOLOR) : m_gridResult.SetCellBackColor(WHITE);
						str.Format("%d", (int)pDoc->m_structWidth[i].m_nResult[j+m_nColumn]);
						m_gridResult.SetTextMatrix(i+1, j+1, str);						
					}
					else
					{
						m_gridResult.SetRow(i+1);
						m_gridResult.SetCol(j+1);
						m_gridResult.SetCellBackColor(WHITE);
						m_gridResult.SetTextMatrix(i+1, j+1, "");						
					}
				}
			}
		}
	}


	// Average 계산
    int nTotValue = 0;
	int nDataNum = 0;

	for( i=0; i<HALF_HEAD; i++ )
	{
		pDoc->m_nWidthAverage[i+m_nColumn] = 0;
		nDataNum = 0;
		nTotValue = 0;

		for( int j=0; j<WIDTH_INDEX; j++)
		{

			if( pDoc->m_structWidth[j].m_nResult[i+m_nColumn] != 0 )
			{
				nDataNum++;
				nTotValue += pDoc->m_structWidth[j].m_nResult[i+m_nColumn];
			}
			
		}
		nDataNum != 0 ? pDoc->m_nWidthAverage[i+m_nColumn] = nTotValue / nDataNum : 1;


		str.Format("%d", pDoc->m_nWidthAverage[i+m_nColumn]);
		m_gridResult.SetTextMatrix(WIDTH_INDEX+1, i+1, str);	
	}







	m_gridResult.ShowWindow(SW_SHOW);*/
}

void CManuMeasWidth::OnClickCmdPrev() 
{
	// TODO: Add your control notification handler code here

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

//	return;

//	if(m_bProcess) return;

//	delete m_fontCombo;

//	if(m_bMeasPosition)
//	{
//		CMsgDlg dlg;
//		dlg.MsgStatus = MSG_CMD_MOVE_WORKINGPOSITION;
//
//		if(IDOK != dlg.DoModal())
//		{
//			m_bProcess = FALSE;
//			return;
//		}
//	}
	if(m_bProcess) return;
	
#if _VISION
	pView->pVision->CloseDisplay(MUX1);
	m_ctrlDisplay0.Close();

	pView->pVision->CloseDisplay(MUX2);
	m_ctrlDisplay1.Close();
#endif

	// by ckh 
	// 화면 종료시 선택 헤더 False함.(추후에 Recipe 별로 선택 헤더만 True로 하기.)
// 	for(int i =0 ; i<MAX_NOZZLE ;i++)
// 	{
// 		PosChk_Datas[i].bSel = pDoc->m_bIsHeadSelected[i];
// 	}


	CDialog::EndDialog(IDOK);
}
/*
void CManuMeasWidth::OnClickCmdView() 
{
	// TODO: Add your control notification handler code here
	
	return;

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	
	if(m_bProcess) return;
	if(m_nViewMode == 2)
	{
		m_ctrlView.SetCaption("Column #1");
		m_nViewMode = 0;
		m_ctrlDisplay0.MoveWindow(10,210,640,480);

		pView->m_pDevice->CameraSelect(m_nSelect1 + HEAD1_CAM);
		pView->pVision->Grab(MUX1);

		CString str;
		str.Format(" Head %d ", m_nSelect1+1);
	}
	else if(m_nViewMode == 0)
	{
		m_ctrlView.SetCaption("Column #2");
		m_nViewMode = 1;
		m_ctrlDisplay0.MoveWindow(0,0,0,0);

		pView->m_pDevice->CameraSelect(m_nSelect2+HEAD1_CAM);
		pView->pVision->Grab(MUX2);

		CString str;
		str.Format(" Head %d ", m_nSelect2+1);
	}
	else if(m_nViewMode == 1)
	{
		m_ctrlView.SetCaption("Column #1, #2");
		m_nViewMode = 2;
		m_ctrlDisplay0.MoveWindow(10,210,320,480);
		pView->pVision->ImagePan(MUX1);
	}
}
*/
void CManuMeasWidth::OnClickCmdMeas() 
{
	return;
	// TODO: Add your control notification handler code here

/*	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	if(m_bProcess) return;
	// 각종 interlock 확인 할 것...
	
	if(!CheckInterlock()) return;

	ButtonControl(FALSE);

	m_bProcess = TRUE;

	bool   CheckColInterval;
	double dbCom_Y_K1_K3 = (STAGE_Y_BASE - (_STAGE_Y_DIMENSION/2) + pDoc->m_structDispensingCondition.m_dColOffset); // [mm]
	double dbCom_Y_K1 = 0.0; //-pDoc->m_dblCamOffsetY[m_nBaseHead];// + pDoc->m_dblCamOffsetY[m_nBaseHead+HALF_HEAD];
	double dbCom_Y_K2 = 0.0; //-pDoc->m_dblCamOffsetY[m_nBaseHead+HALF_HEAD];
	double k1 = dbCom_Y_K1_K3 + pDoc->m_dblKDistance[0];  // [mm]
	double k3 = dbCom_Y_K1_K3 + pDoc->m_dblKDistance[1] + pDoc->m_structDispensingCondition.m_dColOneFromColTwo; //[mm]
		   
//Cleaning Glass내 도포
   	k1 += pDoc->m_structPatternData[ThreadStage.PatternNo].m_dbDrawingPosOffset;
	k3 += pDoc->m_structPatternData[ThreadStage.PatternNo].m_dbDrawingPosOffset;

	
	if( pDoc->m_structDispensingCondition.m_dColOneFromColTwo  < 400 )
	{
		CheckColInterval = TRUE;
	}
	else
	{
		dbCom_Y_K1 = -pDoc->m_dblCamOffsetY[m_nBaseHead];
		dbCom_Y_K2 = -pDoc->m_dblCamOffsetY[m_nBaseHead+HALF_HEAD];
		CheckColInterval = FALSE;
	}
	
	k1 += dbCom_Y_K1;   
	k3 += dbCom_Y_K2;
		   
	pView->m_pDevice->MoveAbs(STAGE_THETA, _dbFinalThetaPos, pDoc->m_dblThetaMoveSpeed, FALSE);

	int i,j,k, m;
	double poss, posy;
	int Model, Nozzle, point, w, h, cx, cy, filter, score;
	CString str;

	BOOL m_bClickMeas = FALSE;

	for(i=0 ; i<WIDTH_INDEX ; i++)
	{
		for(j=0 ; j<MAX_HEAD ; j++)
		{
			m_gridResult.SetTextMatrix(i+1, j%HALF_HEAD+1, "");
			// 기존 측정 Data 
			//pDoc->m_structWidth[i].m_nResult[j] = 0;

			m_bClickMeas |= m_bCheckMeas[i][j];
		}
	}

	if(!m_bClickMeas)	// Click된 point가 없다면 원래대로
	{
		for(i=0;i<WIDTH_INDEX;i++)
		{
			for(j=0;j<MAX_HEAD;j++)
			{
				pDoc->m_structWidth[i].m_nResult[j] = 0;	// Click된 Point가 없으므로 전체 초기화
				m_bCheckMeas[i][j] = TRUE;
			}
		}
	}

	for( i=0; i<MAX_HEAD; i++) // Analog 조명 전환
	{
		pView->m_pDevice->SetRightDigitalValue(i, _nLightValue[i], FALSE);
	}

	int sHeadNo, eHeadNo;
	for(k=0;k<WIDTH_INDEX;k++)
	{	
		for(i=0;i<WIDTH_INDEX;i++)
		{
			if((k+1==pDoc->m_structWidth[i].m_nSeq)&&(0!=pDoc->m_structWidth[i].m_nAng))
			{
				int nCheckHead = 0;
				for(j=0;j<MAX_HEAD;j++)
				{
					if(pDoc->m_structWidth[i].m_bHead[j] && m_bCheckMeas[i][j]) 
					{
						nCheckHead++;
					}
				}
				nCheckHead != 0 ? MoveCameraPosition2(i) : 0;

 				pView->m_pDevice->MoveAbs(K1, k1*1000.0, pDoc->m_dblKMoveSpeed, FALSE);	
				Sleep(500);
				pView->m_pDevice->MoveAbs(K3, k3*1000.0 + m_dblColumnOffsetY, pDoc->m_dblKMoveSpeed, FALSE);

				for(int m_nHeadNo=0;m_nHeadNo<MAX_HEAD;m_nHeadNo++)
				{
					while(!pView->m_pDevice->MotionEndCheck(m_nHeadNo+HEAD_S1)) pFrame->DoEvents();
				}
				
				pView->m_pDevice->SetSync(HEAD_S_ALL, TRUE);

				while(!pView->m_pDevice->MotionEndCheck(K1) || !pView->m_pDevice->MotionEndCheck(K3) || !pView->m_pDevice->MotionEndCheck(STAGE_THETA))	pFrame->DoEvents();
					

 				for(j=0;j<MAX_HEAD;j++)
				{
					if(pDoc->m_structWidth[i].m_bHead[j] && m_bCheckMeas[i][j]) 
					{

						// camera select
							pView->m_pDevice->CameraSelect(j+3);
						
						// column에 대한 vision board channel 선택
						if( (j<HALF_HEAD) ) 
						{
							m_nViewMode = MUX1;
							m_ctrlDisplay0.MoveWindow(10,200,640,480);	
							m_ctrlDisplay1.MoveWindow(0,0,0,0);	
						}
						else
						{
							m_nViewMode = MUX2;
							m_ctrlDisplay0.MoveWindow(0,0,0,0);	
							m_ctrlDisplay1.MoveWindow(10,200,640,480);	
						}
						
						pView->pVision->ClearOverlay(m_nViewMode);
						//pView->pVision->Live(m_nViewMode);
						//Sleep(300);




						posy = dbCom_Y_K1_K3-pDoc->m_dbRotateFinalStartPointY/1000.0; //auo  m_dbRotateFinalStartPointY에 측정포인터 값이 pDoc->m_structWidth[i].m_dYPosition이 포함되어져 있음.
						posy += dbCom_Y_K1;

						double dbCamDiff = 0.0;
						
							j < HALF_HEAD ? dbCamDiff = (pDoc->m_dblCamOffsetY[j] - pDoc->m_dblCamOffsetY[m_nBaseHead]) : dbCamDiff = (pDoc->m_dblCamOffsetY[j] - pDoc->m_dblCamOffsetY[m_nBaseHead+HALF_HEAD]);

						posy += ( dbCamDiff ); // pDoc->m_dbRotateFinalStartPointY 에 base head의 camera offset이 적용되어 있으므로 현재 측정Head의 값의 차로 이동

						if(CheckColInterval)
						{
							if( j > HALF_HEAD) posy += (-pDoc->m_dblCamOffsetY[m_nBaseHead]+pDoc->m_dblCamOffsetY[m_nBaseHead+HALF_HEAD]);
						}
//Cleaning Glass내 도포
						posy += pDoc->m_structPatternData[ThreadStage.PatternNo].m_dbDrawingPosOffset;
						posy *= 1000.0;
						posy += ThreadStage.GlassCenterY;		
						
						pView->m_pDevice->MoveAbs(STAGE_Y, posy, pDoc->m_dblStageMoveSpeed, FALSE);	
						
						while(!pView->m_pDevice->MotionEndCheck(STAGE_Y))	pFrame->DoEvents();
						//Sleep(_INPOS_DELAY);							
						
						if(!pView->m_pMotion->MoveSInPosition(dblHeadSetPos)||!pView->m_pMotion->MoveKInPosition(k1*1000.0, k3*1000.0)||!pView->m_pMotion->MoveYInPosition(posy))
						{
								m_bProcess = FALSE;
								ButtonControl(TRUE);
								return;
						}
						Sleep(200);
						
						for(int l=0;l<HALF_HEAD;l++)
						{
							j < HALF_HEAD ? str.Format("L%d", l+1):str.Format("R%d", l+1);
							m_gridResult.SetTextMatrix(0, l+1, str);
						}



						int	VISION_W = 640;
						int VISION_H = 480;
						double dbMAaxWidth = 0.0;
						//double dbMinY = 50.0;
						for( int m=0; m<MAX_CP; m++) // X 위치
						{
							_bAgMode == TRUE ? Model = 3 : Model = pDoc->m_structWidth[i].m_nAng;
							//Model = pDoc->m_structWidth[i].m_nAng;
							if( Model == 1 || Model == 2)
							{
								w = VISION_W*1/2;
								h = 10;
								cx = VISION_W/2;
								m==0 ? cy = VISION_H/2-h*4 : 0;
								m==1 ? cy = VISION_H/2-h*2 : 0;
								m==2 ? cy = VISION_H/2 : 0;
								m==3 ? cy = VISION_H/2+h*2 : 0;
								m==4 ? cy = VISION_H/2+h*4: 0;	
							}
							else if( Model == 3 || Model == 4)
							{
								w = 10;
								h = VISION_H*1/2;
								m==0 ? cx = VISION_W/2-w*4 : 0;
								m==1 ? cx = VISION_W/2-w*2 : 0;
								m==2 ? cx = VISION_W/2 : 0;
								m==3 ? cx = VISION_W/2+w*2 : 0;
								m==4 ? cx = VISION_W/2+w*4: 0;	
								cy = VISION_H/2;

							}

							filter = 50;
							score = 4;
							// column에 대한 vision board channel 선택
							
							if(pView->pVision->CaliperLineWidth(m_nViewMode,Model,cx,cy,h,w, pDoc->_alignAxis.AngleDegree, filter, score, TRUE))
							{
								pView->pVision->m_dblWidthShort = pView->pVision->m_dblWidthShort;
								str.Format("%.f", (float)pView->pVision->m_dblWidthShort);
								pDoc->m_structWidth[i].m_nResult[j] = (int)pView->pVision->m_dblWidthShort;

								//dbWidthAbsPosX[k] = pView->pVision->m_dbWidthAbsPosX;
								(dbMAaxWidth < pDoc->m_structWidth[i].m_nResult[j]) && (pDoc->m_structWidth[i].m_nResult[j] <800) ? dbMAaxWidth  = pDoc->m_structWidth[i].m_nResult[j] : 1;
								pDoc->m_structWidth[i].m_nResult[j] = dbMAaxWidth;
								pFrame->DoEvents();


							}
							else
								str = "";	
						}
		

						m_gridResult.SetTextMatrix(i+1, j%HALF_HEAD+1, str);

						m_nColumn =  (j/HALF_HEAD) * HALF_HEAD;
						SetMSFlexGrid();
						pFrame->DoEvents();
												
					}
				}
			}
		}
	}
		

	bLineWidth = TRUE;
	pView->SaveWidthData();
	m_bProcess = FALSE;
	ButtonControl(TRUE);
*/
}

void CManuMeasWidth::MoveCameraPosition(int nPointNo, int nColumnNo)
{
	//if(m_bMeasPosition) return;

/*	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	pView->m_pMotion->MoveSToCameraPosition(nPointNo, nColumnNo, FALSE);
	m_bMeasPosition = TRUE;*/
}
void CManuMeasWidth::MoveCameraPosition2(int nPointNo)
{
	//if(m_bMeasPosition) return;

/*	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	pView->m_pMotion->MoveSToCameraPosition(nPointNo, FALSE);
	m_bMeasPosition = TRUE;*/
}
BOOL CManuMeasWidth::CheckInterlock()
{
/*	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	
	DWORD INTERLOCK = INTERLOCK_STAGE_HOME|INTERLOCK_THETA_HOME|INTERLOCK_K1_HOME|INTERLOCK_K3_HOME|INTERLOCK_S_HOME|INTERLOCK_Z_HOME|INTERLOCK_DOOR|INTERLOCK_AREA|INTERLOCK_ROBOT;

	if(!pView->CheckInterlock(INTERLOCK)) return FALSE;
	else*/
		return TRUE;
}

//DEL void CManuMeasWidth::ButtonControl(BOOL bCtrl)
//DEL {
//DEL 	GetDlgItem(IDC_CMD_PREV)->EnableWindow(bCtrl);
//DEL 	GetDlgItem(IDC_CMD_MEAS)->EnableWindow(bCtrl);
//DEL 	GetDlgItem(IDC_CMD_ACQUIRE1)->EnableWindow(bCtrl);
//DEL }

BOOL CManuMeasWidth::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE) // ESC Key가 아니면
		{
			pMsg->wParam = NULL;
			pMsg->lParam = NULL;
		}
	}

	///////////////////////////////////////////////////////////////
	// by ckh
// 	CPoint point;
// 	RECT v_rect1,v_rect2;
// 	m_ctrlDisplay0.GetWindowRect(v_rect1);
// 	
// 	switch(pMsg->message)
// 	{
// 	case WM_LBUTTONDOWN:
// 		GetCursorPos(&point);
// 		if((point.x - v_rect1.left < 0) || (point.y - v_rect1.top<0)) 	return CDialog::PreTranslateMessage(pMsg);
// 		if((point.x - v_rect1.left > 320) || (point.y - v_rect1.top> 480)) 	return CDialog::PreTranslateMessage(pMsg);
// 
// 		
// 
// 		break;
// 	}
	//////////////////////////////////////////////////////////

	return CDialog::PreTranslateMessage(pMsg);
}



void CManuMeasWidth::OnClickCmdManualJog() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();	
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	if(m_bProcess) return;

	m_bProcess = TRUE;
	DWORD IO_val1,NEW_IO_val1;
	LONG AxisStatus=0, AxisStatus1=0;

//	if(bManualMove) return;
//	if(2 == m_nViewMode) return;

//	ButtonControl(FALSE);

//	bManualMove = TRUE;
	long old_color = m_ctrlJog.GetBackColor();
	m_ctrlJog.SetBackColor(LIGHTGREEN);

	SetDlgItemText(IDC_LABEL_MSG, "종료를 원하면 STOP or OK");
	
	FAS_SetIoBit(4,true,OK_LAMP,true);

	pFrame->DoEvents();

//	WORD new_key;

	pView->pVision->Live(m_nViewMode); 

	long lParaNo = 8;	// Speed Setting Parameter No.=> 8

	FAS_SetLocalPara(AXIS_K_MASTER/BOARD_AXES+1, AXIS_K_MASTER%BOARD_AXES, lParaNo, 100); //K0

	for(int iaxis = 0 ; iaxis < 2 ; iaxis++) // k1 ~ k2
	{
		FAS_SetLocalPara(AXIS_K1/BOARD_AXES+1, iaxis, lParaNo, 100);
	}

	for(int ihead = 0 ; ihead < MAX_NOZZLE ; ihead++)
	{
		FAS_SetLocalPara(pDoc->m_structHeadConfig[ihead].nS[0], pDoc->m_structHeadConfig[ihead].nS[1], lParaNo, 100);
	}		

	double dPosX ,dPosY = 0.0;
	FAS_GetActualPos(pDoc->m_structHeadConfig[m_nHeadSelectNo].nS[0],pDoc->m_structHeadConfig[m_nHeadSelectNo].nS[1],&dPosX);
	//FAS_GetActualPos(AXIS_K1/BOARD_AXES+1,AXIS_K1%BOARD_AXES,&dPosY);
	FAS_GetCommandPos(AXIS_K_MASTER/BOARD_AXES+1,AXIS_K_MASTER%BOARD_AXES,&dPosY);

	m_dXPointCam[m_nHeadSelectNo] = dPosX/S_PULSE;
	m_dYPointCam[m_nHeadSelectNo] = dPosY/K_PULSE;

	CString str;

	while(1)
	{
		FAS_GetIo(4,true,&NEW_IO_val1);
		Sleep(100);// by ckh 
		NEW_IO_val1 = NEW_IO_val1&0x000003e0;	// OP : 상,하,좌,우, OK // 0000 1110 0011 0000 0000 0000 0000 0000					
		
		if(NEW_IO_val1 != IO_val1)		// 상, 하, 좌, 우, OK 중 하나가 눌려 지면....
		{
			IO_val1 = NEW_IO_val1;		// FAS_MoveJog() : 이 함수는 Motor를 한 방향으로 무조건 돌리는 함수이다. 정지 Command가 있기 전까지 움직인다.
										// 한번 움직이고 나서 Switch를 떼면 정지시키는 함수를 수행하기 때문에 누를 때만 움직이는 것처럼 보인다.						

			FAS_GetAxisStatus(AXIS_K_MASTER/BOARD_AXES+1,AXIS_K_MASTER%BOARD_AXES,&AxisStatus);
			FAS_GetAxisStatus(pDoc->m_structHeadConfig[m_nHeadSelectNo].nS[0],pDoc->m_structHeadConfig[m_nHeadSelectNo].nS[1],&AxisStatus1);

			if((IO_val1 & OK_SWITCH) != 0)
			{	
				FAS_MoveStop(AXIS_K_MASTER/BOARD_AXES+1,AXIS_K_MASTER%BOARD_AXES,0);
				FAS_MoveStop(pDoc->m_structHeadConfig[m_nHeadSelectNo].nS[0],pDoc->m_structHeadConfig[m_nHeadSelectNo].nS[1],0);
			
				break;	
			}
			else if(((IO_val1 & UP_SWITCH)!= 0) && !(AxisStatus&LM_PLUS_DETECT))		
			{
				FAS_MoveJog(AXIS_K_MASTER/BOARD_AXES+1,AXIS_K_MASTER%BOARD_AXES,PLUS);
				FAS_MoveStop(pDoc->m_structHeadConfig[m_nHeadSelectNo].nS[0],pDoc->m_structHeadConfig[m_nHeadSelectNo].nS[1],0);
			}
			else if(((IO_val1 & DOWN_SWITCH)!=0) && !(AxisStatus&LM_MINUS_DETECT))	
			{
				FAS_MoveJog(AXIS_K_MASTER/BOARD_AXES+1,AXIS_K_MASTER%BOARD_AXES,MINUS);	
				FAS_MoveStop(pDoc->m_structHeadConfig[m_nHeadSelectNo].nS[0],pDoc->m_structHeadConfig[m_nHeadSelectNo].nS[1],0);
			}
			else if(((IO_val1 & LEFT_SWITCH)!= 0) && !(AxisStatus1&LM_MINUS_DETECT))	
			{
				FAS_MoveJog(pDoc->m_structHeadConfig[m_nHeadSelectNo].nS[0],pDoc->m_structHeadConfig[m_nHeadSelectNo].nS[1],MINUS);
				FAS_MoveStop(AXIS_K_MASTER/BOARD_AXES+1,AXIS_K_MASTER%BOARD_AXES,0);
			}
			else if(((IO_val1 & RIGHT_SWITCH)!= 0) && !(AxisStatus1&LM_PLUS_DETECT))	
			{
				FAS_MoveJog(pDoc->m_structHeadConfig[m_nHeadSelectNo].nS[0],pDoc->m_structHeadConfig[m_nHeadSelectNo].nS[1],PLUS);
				FAS_MoveStop(AXIS_K_MASTER/BOARD_AXES+1,AXIS_K_MASTER%BOARD_AXES,0);
			}
			// BY CKH
			else
			{
				FAS_MoveStop(AXIS_K_MASTER/BOARD_AXES+1,AXIS_K_MASTER%BOARD_AXES,0);
				FAS_MoveStop(pDoc->m_structHeadConfig[m_nHeadSelectNo].nS[0],pDoc->m_structHeadConfig[m_nHeadSelectNo].nS[1],0);
			}		
		}


		FAS_GetActualPos(pDoc->m_structHeadConfig[m_nHeadSelectNo].nS[0],pDoc->m_structHeadConfig[m_nHeadSelectNo].nS[1],&dPosX);
	//	FAS_GetActualPos(AXIS_K1/BOARD_AXES+1,AXIS_K1%BOARD_AXES,&dPosY);
		FAS_GetCommandPos(AXIS_K_MASTER/BOARD_AXES+1,AXIS_K_MASTER%BOARD_AXES,&dPosY);

		m_dXPointCam[m_nHeadSelectNo] = dPosX/S_PULSE;
		m_dYPointCam[m_nHeadSelectNo] = dPosY/K_PULSE;	
		
		str.Format("%.3f", m_dXPointCam[m_nHeadSelectNo]);
		SetDlgItemText(IDC_LABEL_POS_X, str);
		str.Format("%.3f", m_dYPointCam[m_nHeadSelectNo]);
		SetDlgItemText(IDC_LABEL_POS_Y, str);

		pFrame->DoEvents();
		// by ckh
		if(pView->m_pDevice->SST_Check(STOP_SWITCH) == TRUE )
		{
			m_bProcess= false;
			break;
		}
	}
	
	m_bProcess= false; // 한번 더 
	FAS_SetIoBit(4,true,OK_LAMP,false);

	m_ctrlJog.SetBackColor(old_color);

	SetDlgItemText(IDC_LABEL_MSG, "");

//	ButtonControl(TRUE);
}

void CManuMeasWidth::OnClickCmdHead1() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	if(m_bProcess) return;
	UpdateData(TRUE);
	
	int nCam = HEAD1_CAM-1;
	if( SetSelCam(nCam) == TRUE)
	{
		m_nHeadSelectNo = nCam;
		ThreadStage.nMoveHeadNum = m_nHeadSelectNo;
// 		int HeadCnt = 0;
// 		for(int i =0; i<MAX_NOZZLE; i++)
// 		{
// 			if(PosChk_Datas[i].bSel)
// 				HeadCnt++;
// 		}
// 		if(HeadCnt>=2)
// 		{
// 			GetDlgItem(IDC_CMD_LC_SEARCH)->EnableWindow(FALSE);
// 			GetDlgItem(IDC_CMD_MARK_SEARCH)->EnableWindow(FALSE);
// 			GetDlgItem(IDC_CMD_TEACHING_ALIGNMARK)->EnableWindow(FALSE);			
// 		}
	}
	else
	{
//		ThreadStage.nMoveHeadNum = -1;
// 		int HeadCnt = 0;
// 		for(int i =0; i<MAX_NOZZLE; i++)
// 		{
// 			if(PosChk_Datas[i].bSel)
// 				HeadCnt++;
// 		}
// 		if(HeadCnt==1)
// 		{
// 			GetDlgItem(IDC_CMD_LC_SEARCH)->EnableWindow(TRUE);
// 			GetDlgItem(IDC_CMD_MARK_SEARCH)->EnableWindow(TRUE);
// 			GetDlgItem(IDC_CMD_TEACHING_ALIGNMARK)->EnableWindow(TRUE);
// 		}
// 		else
// 		{
// 			GetDlgItem(IDC_CMD_LC_SEARCH)->EnableWindow(FALSE);
// 			GetDlgItem(IDC_CMD_MARK_SEARCH)->EnableWindow(FALSE);
// 			GetDlgItem(IDC_CMD_TEACHING_ALIGNMARK)->EnableWindow(FALSE);	
// 		}
	}
	int HeadCnt = 0;
	for(int i =0; i<MAX_NOZZLE; i++)
	{
		if(PosChk_Datas[i].bSel)
		{
			HeadCnt++;
			m_nHeadSelectNo = i;
		}
	}
	if(HeadCnt==1)
	{
		GetDlgItem(IDC_CMD_LC_SEARCH)->EnableWindow(TRUE);
		GetDlgItem(IDC_CMD_MARK_SEARCH)->EnableWindow(TRUE);
		GetDlgItem(IDC_CMD_TEACHING_ALIGNMARK)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_CMD_LC_SEARCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_CMD_MARK_SEARCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_CMD_TEACHING_ALIGNMARK)->EnableWindow(FALSE);	
	}
}

void CManuMeasWidth::OnClickCmdHead2() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	if(m_bProcess) return;
	UpdateData(TRUE);

	int nCam = HEAD2_CAM-1;
	if( SetSelCam(nCam) == TRUE)
		m_nHeadSelectNo = nCam;}

void CManuMeasWidth::OnClickCmdHead3() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(m_bProcess) return;
	UpdateData(TRUE);

	int nCam = HEAD3_CAM-1;
	if( SetSelCam(nCam) == TRUE)
		m_nHeadSelectNo = nCam;}

void CManuMeasWidth::OnClickCmdHead4() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(m_bProcess) return;
	UpdateData(TRUE);

	int nCam = HEAD4_CAM-1;
	if( SetSelCam(nCam) == TRUE)
		m_nHeadSelectNo = nCam;}

void CManuMeasWidth::OnClickCmdHead5() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(m_bProcess) return;
	UpdateData(TRUE);

	int nCam = HEAD5_CAM-1;
	if( SetSelCam(nCam) == TRUE)
		m_nHeadSelectNo = nCam;
}


void CManuMeasWidth::OnClickCmdHead6() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(m_bProcess) return;
	UpdateData(TRUE);
	
	int nCam = HEAD6_CAM-1;
	if( SetSelCam(nCam) == TRUE)
		m_nHeadSelectNo = nCam;}

void CManuMeasWidth::OnClickCmdHead7() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	if(m_bProcess) return;
	UpdateData(TRUE);

	int nCam = HEAD7_CAM-1;
	if( SetSelCam(nCam) == TRUE)
		m_nHeadSelectNo = nCam;}

void CManuMeasWidth::OnClickCmdHead8() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	if(m_bProcess) return;
	UpdateData(TRUE);

	int nCam = HEAD8_CAM-1;
	if( SetSelCam(nCam) == TRUE)
		m_nHeadSelectNo = nCam;
}

void CManuMeasWidth::OnClickCmdHead9() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	if(m_bProcess) return;
	UpdateData(TRUE);

	int nCam = HEAD9_CAM-1;
	if( SetSelCam(nCam) == TRUE)
		m_nHeadSelectNo = nCam;
}

void CManuMeasWidth::OnClickCmdHead10() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	if(m_bProcess) return;
	UpdateData(TRUE);

	int nCam = HEAD10_CAM-1;
	if( SetSelCam(nCam) == TRUE)
		m_nHeadSelectNo = nCam;

}

void CManuMeasWidth::OnClickCmdHead11() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	if(m_bProcess) return;
	UpdateData(TRUE);

	int nCam = HEAD11_CAM-1;
	if( SetSelCam(nCam) == TRUE)
		m_nHeadSelectNo = nCam;

}

void CManuMeasWidth::OnClickCmdHead12() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	if(m_bProcess) return;
	UpdateData(TRUE);

	int nCam = HEAD12_CAM-1;
	if( SetSelCam(nCam) == TRUE)
		m_nHeadSelectNo = nCam;
}

void CManuMeasWidth::OnClickCmdHead13() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	if(m_bProcess) return;
	UpdateData(TRUE);

	int nCam = HEAD13_CAM-1;
	if( SetSelCam(nCam) == TRUE)
		m_nHeadSelectNo = nCam;}

void CManuMeasWidth::OnClickCmdHead14() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	if(m_bProcess) return;
	UpdateData(TRUE);

	int nCam = HEAD14_CAM-1;
	if( SetSelCam(nCam) == TRUE)
		m_nHeadSelectNo = nCam;}

void CManuMeasWidth::OnClickCmdHead15() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	if(m_bProcess) return;
	UpdateData(TRUE);

	int nCam = HEAD15_CAM-1;
	if( SetSelCam(nCam) == TRUE)
		m_nHeadSelectNo = nCam;}

void CManuMeasWidth::OnClickCmdHead16() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	if(m_bProcess) return;
	UpdateData(TRUE);

	int nCam = HEAD16_CAM-1;
	if( SetSelCam(nCam) == TRUE)
	{
		m_nHeadSelectNo = nCam;
		ThreadStage.nMoveHeadNum = m_nHeadSelectNo;
// 		int HeadCnt = 0;
// 		for(int i =0; i<MAX_NOZZLE; i++)
// 		{
// 			if(PosChk_Datas[i].bSel)
// 				HeadCnt++;
// 		}
// 		if(HeadCnt>=2)
// 		{
// 			GetDlgItem(IDC_CMD_LC_SEARCH)->EnableWindow(FALSE);
// 			GetDlgItem(IDC_CMD_MARK_SEARCH)->EnableWindow(FALSE);
// 			GetDlgItem(IDC_CMD_TEACHING_ALIGNMARK)->EnableWindow(FALSE);
// 		}
	}
	else
	{
//		ThreadStage.nMoveHeadNum = -1;
// 		int HeadCnt = 0;
// 		for(int i =0; i<MAX_NOZZLE; i++)
// 		{
// 			if(PosChk_Datas[i].bSel)
// 				HeadCnt++;
// 		}
// 		if(HeadCnt==1)
// 		{
// 			GetDlgItem(IDC_CMD_LC_SEARCH)->EnableWindow(TRUE);
// 			GetDlgItem(IDC_CMD_MARK_SEARCH)->EnableWindow(TRUE);
// 			GetDlgItem(IDC_CMD_TEACHING_ALIGNMARK)->EnableWindow(TRUE);
// 		}
// 		else
// 		{
// 			GetDlgItem(IDC_CMD_LC_SEARCH)->EnableWindow(FALSE);
// 			GetDlgItem(IDC_CMD_MARK_SEARCH)->EnableWindow(FALSE);
// 			GetDlgItem(IDC_CMD_TEACHING_ALIGNMARK)->EnableWindow(FALSE);
// 		}
	}
	int HeadCnt = 0;
	for(int i =0; i<MAX_NOZZLE; i++)
	{
		if(PosChk_Datas[i].bSel)
		{
			HeadCnt++;
			m_nHeadSelectNo = i;
		}
	}
	if(HeadCnt==1)
	{
		GetDlgItem(IDC_CMD_LC_SEARCH)->EnableWindow(TRUE);
		GetDlgItem(IDC_CMD_MARK_SEARCH)->EnableWindow(TRUE);
		GetDlgItem(IDC_CMD_TEACHING_ALIGNMARK)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_CMD_LC_SEARCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_CMD_MARK_SEARCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_CMD_TEACHING_ALIGNMARK)->EnableWindow(FALSE);	
	}
}

void CManuMeasWidth::OnClickCmdSave() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	// by ckh 
	CString strFile,strPath,strFileTemp;
	strPath.Format("D:\\TOP\\P8CA_LC\\P8CA_LC_DATA");

	CNormalMsg dlg;
	dlg.m_bTimer=FALSE;
	dlg.m_strTitle = _T("CONFIRM");
	dlg.m_strMsg1 = " 변경된 위치를 티칭 위치로 저장 하시겠습니까? 저장시 모터 정위치 기준값이 변경됩니다. ";
	dlg.m_strMsg2 = " 실행하려면 '실행'버튼 그렇지 않으면, '취소' 버튼을 누르세요!";
	
	double dDistHead[MAX_NOZZLE];
	if( dlg.DoModal() == IDOK )
	{
		pDoc->m_dAccuracyPosX[m_nHeadSelectNo] = m_dXPointCam[m_nHeadSelectNo];
		pDoc->m_dAccuracyPosY[m_nHeadSelectNo] = m_dYPointCam[m_nHeadSelectNo];

		pDoc->SaveAccuracyMotorPos();

		pView->m_pDevice->HeadSPositionOffsetRead(dDistHead);
		////////////////////////////////////////////////////////////
		// by ckh 
		PosChk_Datas[m_nHeadSelectNo].dPosX = m_dXPointCam[m_nHeadSelectNo] - dDistHead[m_nHeadSelectNo];
		PosChk_Datas[m_nHeadSelectNo].dPosY = m_dYPointCam[m_nHeadSelectNo] - pDoc->m_dTeachData[17][2];
		for(int i=0; i<MAX_NOZZLE;i++) // 스레드 내에서 헤더별 티칭 데이터동작을 하므로 하나씩 데이터를 저장하게 함.
		{
			pView->m_pDevice->SaveTeachingPosChk(PosChk_Datas[i],i);
		}
		strFile = strPath + "\\Teaching_PosChk.dat";
		strFileTemp = strPath + "\\Teaching_PosChkTemp.dat";
		CopyFile(strFileTemp,strFile,FALSE);// 기존 파일에 덮어쓰기.

		// 임시저장파일 제거하기
		::DeleteFile(strFileTemp);

		/////////////////////////////////////////////////////////////
	}
	else
	{
		return;
	}				
}

void CManuMeasWidth::OnClickCmdBlob() 
{
	return;
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView* m_pLcView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *m_pLcDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	CString str,str1,str2,str3 = "";
	int nSearchCount = 0;
	double dBlobObjectArea = 0;		//임시 Area
	double dCx = 0;					//임시 X 센터 
	double dCy = 0;					// 임시 Y 센터 
	long lCenterRange = 10;	// Object의 중심점이 Spec In되는 화면 중심점으로 부터의 범위 
	int m_nOptObjectNumber = 0;
	long left, top, right, bottom;	// 화면에 그리기 
		
	double dNewDistance = 0;		// Object들의 중심점간의 거리를 구하기 위한 변수 
	double dOldDistance = 1000;		// Object들의 중심점간의 거리를 구하기 위한 변수 
	double m_dDistXfromCenter = 0;
	double m_dDistYfromCenter = 0;
	
	m_pLcDoc->Blob_Parameter.m_bObjectExist = false;


/*
	pView->pVision->m_pDisplay0 = &m_ctrlDisplay0;
	pView->pVision->SetDisplay(MUX1);
	pView->pVision->CrossDisplay("Head # 1",240,320);
	pView->m_pDevice->CameraSelect(HEAD1_CAM);
	pView->pVision->Live(MUX1);
*/

	m_pLcView->m_pDevice->CameraSelect(m_nHeadSelectNo+1);
	m_pLcView->pVision->ClearOverlay(m_nViewMode);

	//---------------------------------------//
	m_pLcView->pVision->m_pMark->PMSetPatternId((long)m_nViewMode);

	LPBYTE fm = (LPBYTE)grabber.GetImagePtr(m_nViewMode);

	m_pLcView->pVision->m_pMark->PMSetSearchFrame((long)fm);

	//---------------------------------------//
	m_pLcView->pVision->m_pMark->BLOBSetTouchAllow(1,1,1,1); // 화면 경계 인정 
	m_pLcView->pVision->m_pMark->BLOBSetAutoThreshold(true); // 이진화를 위한 임계치 자동 설정 -> BLOBSetAutoThreshold(BOOL flag);
	
//	m_Mark.BLOBSetObjectColor(true);		// true 는 백색의 물체를 검색 //TEST후 결정할 것!!!
	m_pLcView->pVision->m_pMark->BLOBSetObjectColor(true);
	
	nSearchCount=m_pLcView->pVision->m_pMark->BLOBProcessArea(0,0,640,480); // 영역내에서 인식된 물체의 갯수 
	
	for(int n=0; n<nSearchCount; n++)
	{
		dBlobObjectArea=m_pLcView->pVision->m_pMark->BLOBGetArea(n);	// N 번째 Object의 Area 를 획득 
		m_pLcView->pVision->m_pMark->BLOBGetCenter(n,&dCx,&dCy);	// Object의 중심값 획득 

		if(dBlobObjectArea > 5000)	// Area 가 일정 크기 이상이면 //Target Area를 설절할 수 있도록 하자...//
		{
			// 중심점이 설정 범위 내에 있는지 확인 
			if((BLOB_MIN_POSX < dCx)&&(dCx < BLOB_MAX_POSX))
			{
				if((BLOB_MIN_POSY < dCy)&&(dCy < BLOB_MAX_POSY))
				{
					// 중심값으로 부터 거리 계산 (Object가 2개 이상일 경우 대비)
					dNewDistance = sqrt(pow((BLOB_CENTER_X-dCx),2) + pow((BLOB_CENTER_Y-dCy),2)); 
					
					if(dNewDistance < dOldDistance)	// 거리가 가장 짧은값 찾기 
					{
						dOldDistance = dNewDistance; // New값을 Old로 갱신 

						m_dDistanceCamX[m_nHeadSelectNo] = BLOB_CENTER_X-dCx; //(-)이면 +오차 ,//(+)이면 -오차
						m_dDistanceCamY[m_nHeadSelectNo] = BLOB_CENTER_Y-dCy; //(-)이면 +오차 ,//(+)이면 -오차
								
						
						// 최적의 값을 갖는 Object에 대한 정보를 저장 
						m_pLcDoc->Blob_Parameter.m_dObjectArea = dBlobObjectArea;
						m_pLcDoc->Blob_Parameter.m_dObjectCenterX = dCx;
						m_pLcDoc->Blob_Parameter.m_dObjectCenterY = dCy;
						m_pLcDoc->Blob_Parameter.m_bObjectExist = true;
						m_nOptObjectNumber = n;
					}		
				}
			}
		}

	}

	// Object중심과 화면중심과의 거리가 Spec In 인지 판별 
	lCenterRange = long(4/PIXEL_SIZE);
	m_pLcView->pVision->DrawRect(m_nViewMode,BLOB_CENTER_X-lCenterRange, BLOB_CENTER_Y-lCenterRange, BLOB_CENTER_X+lCenterRange, BLOB_CENTER_Y+lCenterRange, RGB(255,0,0));	// 사각형 그리기 

	if(m_pLcDoc->Blob_Parameter.m_bObjectExist==TRUE)	// Object가 존재하면...
	{
		if(dOldDistance < lCenterRange)	
		{
			m_pLcView->pVision->m_pMark->BLOBGetRectangle(m_nOptObjectNumber, &left, &top, &right, &bottom);
			
			m_dDistXfromCenter = (right-left)*0.01; // 원의 지름 X 축 
			m_dDistYfromCenter = (bottom-top)*0.01; // 원의 지름 Y 축 

			m_pLcView->pVision->DrawRect(m_nViewMode,left, top, right, bottom, RGB(0,0,255));	// 사각형 그리기 
			
			m_pLcView->pVision->CrossDisplay(int(m_pLcDoc->Blob_Parameter.m_dObjectCenterX),int(m_pLcDoc->Blob_Parameter.m_dObjectCenterY),5,RGB(255,0,0));	// 중심점 표시 
		}		
		//중심값 표기...//
		m_pLcDoc->Blob_Parameter.m_dObjectCenterX = -(BLOB_CENTER_X-m_pLcDoc->Blob_Parameter.m_dObjectCenterX)*PIXEL_SIZE; // pixel 값을 mm 값으로 변환 
		m_pLcDoc->Blob_Parameter.m_dObjectCenterY = (BLOB_CENTER_Y-m_pLcDoc->Blob_Parameter.m_dObjectCenterY)*PIXEL_SIZE; // pixel 값을 mm 값으로 변환 

		//중심값으로 부터의 거리(오차)를 표기한다...//
		m_dDistanceCamX[m_nHeadSelectNo] = -m_dDistanceCamX[m_nHeadSelectNo]*PIXEL_SIZE;
		m_dDistanceCamY[m_nHeadSelectNo] = m_dDistanceCamY[m_nHeadSelectNo]*PIXEL_SIZE;

		str.Format("%.3f", m_pLcDoc->Blob_Parameter.m_dObjectCenterX);
		SetDlgItemText(IDC_LABEL_CENTER_X, str);
		str1.Format("%.3f", m_pLcDoc->Blob_Parameter.m_dObjectCenterY);
		SetDlgItemText(IDC_LABEL_CENTER_Y, str1);
		str2.Format("%.3f", m_dDistanceCamX[m_nHeadSelectNo]);
		SetDlgItemText(IDC_LABEL_DIST_X, str2);
		str3.Format("%.3f", m_dDistanceCamY[m_nHeadSelectNo]);
		SetDlgItemText(IDC_LABEL_DIST_Y, str3);
	}
	else
	{
		AfxMessageBox(" Position Error ");
	}
/*	if(m_nViewMode == 0)
	{
		m_pLcView->pVision->CrossDisplay("Head # 1",320,240);
	}
	else
	{
		m_pLcView->pVision->CrossDisplay("Head #9", 320,240);
	}*/
}

void CManuMeasWidth::OnClickCmdMovePos1() 
{
	return;	
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	
	double dCurPos[MAX_NOZZLE], dSetPos[MAX_NOZZLE];//, dMovePos[MAX_NOZZLE];
//	WORD wAxis[MAX_NOZZLE];
	double dPos = 0.0;
	BOOL bSyncIOResult = false;
	double dTempPos = 1165.83;
	BOOL bPosResult = false;
	long AxisStatus=0;

	pDoc->m_dAccuracyPosY[0] = 911.56;
	pDoc->m_dAccuracyPosX[0] = 1052.97;

	pView->m_pDevice->ColumnPositionMove(pDoc->m_dAccuracyPosY[0],0,pDoc->m_dTeachData[39][2],1);
	Sleep(100);

	FAS_GetAxisStatus(AXIS_K_MASTER/BOARD_AXES+1, AXIS_K_MASTER%BOARD_AXES, &AxisStatus);  	
		
//	if((AxisStatus & MOTIONING)== FALSE) 
//	{
//		while(!pView->m_pDevice->MotionEndCheck(AXIS_K_MASTER));
//	}

	bPosResult = pView->m_pDevice->ColumnPosition_Check(pDoc->m_dAccuracyPosY[0],0);
	if(bPosResult== FALSE)
	{
		pView->PostMessage(WM_ERROR,107,NULL);
		return;
	}						
						
	for(int i=0 ; i<MAX_NOZZLE/2 ; i++)
	{
		dCurPos[i] = FAS_GetActualPos(pDoc->m_structHeadConfig[i].nS[0],pDoc->m_structHeadConfig[i].nS[1],&dPos);
		dCurPos[i+MAX_NOZZLE/2] = FAS_GetActualPos(pDoc->m_structHeadConfig[i+MAX_NOZZLE/2].nS[0],
												   pDoc->m_structHeadConfig[i+MAX_NOZZLE/2].nS[1],&dPos);

		dSetPos[i] = pDoc->m_dAccuracyPosX[0] + 4*(i);
		dSetPos[i+MAX_NOZZLE/2] = dTempPos + 4*(i);
	}

	// S축 동기 해제
	bSyncIOResult = pView->m_pDevice->SyncSetIO(SYNC_AXIS_S, 0.0,  false, false);
	if(bSyncIOResult== FALSE)
	{
		ThreadStage.ManualJobStep=100; // **exit**
		pView->PostMessage(WM_ERROR,111,NULL);
	}	

	pView->m_pDevice->HeadSPositionMove(dSetPos[0], dSetPos[1] ,dSetPos[2], dSetPos[3],  dSetPos[4] ,dSetPos[5], 
											dSetPos[6], dSetPos[7] ,dSetPos[8], dSetPos[9], dSetPos[10] ,dSetPos[11],
											dSetPos[12],dSetPos[13],dSetPos[14],dSetPos[15],1);

	for(i = 0; i < MAX_NOZZLE; i++)
	{
		if(i < NOZZLE_S1)							
			while(!pView->m_pDevice->MotionEndCheck(AXIS_S1+i));
		else
			while(!pView->m_pDevice->MotionEndCheck(AXIS_S1+i));
	}
	pFrame->DoEvents();
}

void CManuMeasWidth::OnClickCmdMovePos2() 
{
	// TODO: Add your control notification handler code here
/*	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	
	double dCurPos[MAX_NOZZLE], dSetPos[MAX_NOZZLE], dMovePos[MAX_NOZZLE];
	WORD wAxis[MAX_NOZZLE];
	double dPos = 0.0;
	BOOL bSyncIOResult = false;
	double dTempPos = 1000.0;
	BOOL bPosResult = false;
	long AxisStatus=0;

	pView->m_pDevice->ColumnPositionMove(pDoc->m_dAccuracyPosY[8],0,pDoc->m_dTeachData[39][2],1);
	Sleep(100);

	FAS_GetAxisStatus(AXIS_K_MASTER/BOARD_AXES+1, AXIS_K_MASTER%BOARD_AXES, &AxisStatus);  	
		
	if((AxisStatus & MOTIONING)== FALSE) 
	{
		while(!pView->m_pDevice->MotionEndCheck(AXIS_K1));
	}
	
	bPosResult = pView->m_pDevice->ColumnPosition_Check(pDoc->m_dAccuracyPosY[8],0);
	if(bPosResult== FALSE)
	{
		pView->PostMessage(WM_ERROR,107,NULL);
		return;
	}	
	
	for(int i=0 ; i<MAX_NOZZLE/2 ; i++)
	{
		dCurPos[i] = FAS_GetActualPos(pDoc->m_structHeadConfig[i].nS[0],pDoc->m_structHeadConfig[i].nS[1],&dPos);
		dCurPos[i+MAX_NOZZLE/2] = FAS_GetActualPos(pDoc->m_structHeadConfig[i+MAX_NOZZLE/2].nS[0],
												   pDoc->m_structHeadConfig[i+MAX_NOZZLE/2].nS[1],&dPos);

		dSetPos[i] = dTempPos + 4*(i);
		dSetPos[i+MAX_NOZZLE/2] = pDoc->m_dAccuracyPosX[8] + 4*(i);
	}

	// S축 동기 해제
	bSyncIOResult = pView->m_pDevice->SyncSetIO(SYNC_AXIS_S, 0.0,  false);
	if(bSyncIOResult== FALSE)
	{
		ThreadStage.ManualJobStep=100; // **exit**
		pView->PostMessage(WM_ERROR,111,NULL);
	}	

	pView->m_pDevice->HeadSPositionMove(dSetPos[0], dSetPos[1] ,dSetPos[2], dSetPos[3],  dSetPos[4] ,dSetPos[5], 
											dSetPos[6], dSetPos[7] ,dSetPos[8], dSetPos[9], dSetPos[10] ,dSetPos[11],
											dSetPos[12],dSetPos[13],dSetPos[14],dSetPos[15],1);

	for(i = 0; i < MAX_NOZZLE; i++)
	{
		if(i < NOZZLE_S1)							
			while(!pView->m_pDevice->MotionEndCheck(AXIS_S1+i));
		else
			while(!pView->m_pDevice->MotionEndCheck(AXIS_S1+i));
	}
	pFrame->DoEvents();	*/
}

////////////////////////////////////////////////////////////////////////////////////////////
// by ckh 
#include "Define_List.h"
extern PosChk_ThreadData PosChk_Thread_Datas;	// PosChk를 위한 Flow 선택 데이터 
extern PosChk_Data PosChk_Datas[MAX_NOZZLE];				// PosChk를 위한 각 축별 데이터

void CManuMeasWidth::OnClickCmdPoschk() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	
	// 인터록 체크를 여기서 할까~~!!!!????

	// 헤더 16개 축 선택하여 테스트 진행, 임의 선택 사항은 추후 진행.( 체크 버튼으로 하자..)
	if (m_bProcess == TRUE)
	{
		return;
	}
	m_bProcess = TRUE; // 작업 시작 플렝그
	m_bButExect = TRUE;
	// PosChk 스레드 스텝 변경하기.
	m_ctrlPosChk.SetBackColor(RED);
	m_ctrlPosChk.SetCaption("정위치 실행 중");
  	UpdateData(true);
	PosChk_Thread_Datas.bFirstChk = m_bFirstChk; // 초기 값 저장 플레그 확인. 

	// by ckh 2009.01.28
	SetCamViewr(0); // 처음에는 1번 카메라. 
	// by ckh 2009.01.29
	for(int i =0; i<MAX_NOZZLE; i++)
	{
		if(PosChk_Datas[i].bLightUse == TRUE)
		{	
			if(i==0)
				pView->m_pDevice->SetLightBright(MUX1,PosChk_Datas[i].nLightVal);
			else if(i == 15)
				pView->m_pDevice->SetLightBright(MUX2,PosChk_Datas[i].nLightVal);
		}
	}

	// by ckh 2008.12.26
	// 모드 추가.
	// 얼라인 모드도 여기에 추가하기.( 얼라인 찾기 서치 Flag추가하기.)
	PosChk_Thread_Datas.nMode = MODE_POSCHK;// 정위치 
	// 시작 버튼 감지를 위함.
	SetTimer(START_POSCHK,500,NULL);
	
	// by ckh 2009.01.31 이건 무슨 코멘트인가, 확인 후 추가하기.
	//SetDlgItemText(IDC_LABEL_MSG, "OK 버튼을 눌려주세요.");

}
////////////////////////////////////////////////////////////////////////////////////////////////

void CManuMeasWidth::OnTimer(UINT nIDEvent) 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	switch(nIDEvent)	
	{
	case START_POSCHK:
		if(m_bLamp == TRUE)
		{
			pView->m_pDevice->Start_button_onoff(true);
			m_bLamp = FALSE;
		}
		else
		{
			pView->m_pDevice->Start_button_onoff(false);
			m_bLamp = TRUE;
		}
		if(pView->m_pDevice->SST_Check(STOP_SWITCH) == TRUE  || m_bButExect == FALSE)
		{
			m_bProcess = FALSE; // 작업 중 플레그
			pView->m_pDevice->Start_button_onoff(false);
			pView->m_pDevice->Stop_button_onoff(false);
			// by ckh 2009.01.09 모드 구분 추가
			if(PosChk_Thread_Datas.nMode == MODE_POSCHK)
				m_ctrlPosChk.SetBackColor(GREEN);
			else if(PosChk_Thread_Datas.nMode == MODE_ALIGN)
				m_ctrlAlignSearch.SetBackColor(GREEN);


			KillTimer(BUT_TIMER);
			KillTimer(START_POSCHK);
			pView->KillThread(THREAD_POS_CHK);

			//////////////////////
			m_bFirstChk = FALSE;
			UpdateData(FALSE);
			//////////////////////

			m_bProcess = false;
			PosChk_Thread_Datas.bExitFlag = TRUE;
	
			SetDlgItemText(IDC_LABEL_MSG, "종료를 원하면 STOP");// by ckh 2009.02.10

			break;
		}
		/* Start 버튼 선택 */
		if(pView->m_pDevice->SST_Check(START_SWITCH) == TRUE)
		{	
			
			PosChk_Datas[0].nCam = 0; // by ckh 2009.02.10 , 시작은 1번 카메라부터

			pView->m_pDevice->Start_button_onoff(true);
			// PosChk 스레드 스텝 변경하기.
			UpdateData(true);
			PosChk_Thread_Datas.bExitFlag = FALSE;
			PosChk_Thread_Datas.bFirstChk = m_bFirstChk; // 초기 값 저장 플레그 확인. 
			PosChk_Thread_Datas.nNextStep = POS_CHK_SEL_AXIS;// 선택 모터가 있는지 확인부터 시작함.
			pView->RunThread(THREAD_POS_CHK);

			KillTimer(START_POSCHK);
			SetTimer(BUT_TIMER,500,NULL);

			m_bProcess = true;
		}

		break;

	case BUT_TIMER:
		if(PosChk_Thread_Datas.nMode == MODE_POSCHK)
		{
			if(m_ctrlPosChk.GetBackColor() == GREEN)
				m_ctrlPosChk.SetBackColor(RED);
			else
				m_ctrlPosChk.SetBackColor(GREEN);
		}
		else if(PosChk_Thread_Datas.nMode == MODE_ALIGN)
		{
			if(m_ctrlAlignSearch.GetBackColor() == GREEN)
				m_ctrlAlignSearch.SetBackColor(RED);
			else
				m_ctrlAlignSearch.SetBackColor(GREEN);
		}
		// by ckh 2009.01.28
		if(PosChk_Datas[0].nCam >= MAX_NOZZLE/2)
			SetCamViewr(PosChk_Datas[0].nCam);
		
		// by ck 2009.01.31
		SetDlgItemText(IDC_LABEL_MSG, PosChk_Thread_Datas.strMsg);

		pView->m_pDevice->Stop_button_onoff(true);
		if(PosChk_Thread_Datas.nCntStep == MOTION_WAIT)
		{
			pView->m_pDevice->Start_button_onoff(false);
			pView->m_pDevice->Stop_button_onoff(false);
//			m_ctrlPosChk.SetEnabled(TRUE);
			if(PosChk_Thread_Datas.nMode == MODE_POSCHK)
			{
				m_ctrlPosChk.SetBackColor(GREEN);
				m_ctrlPosChk.SetCaption(" 정위치 실행");
			}
			else if(PosChk_Thread_Datas.nMode == MODE_ALIGN)
			{
				m_ctrlAlignSearch.SetBackColor(GREEN);
				m_ctrlAlignSearch.SetCaption("Align Mark Search");
			}

			UpdateData(TRUE);
			KillTimer(BUT_TIMER);
			pView->KillThread(THREAD_POS_CHK);
			
			//////////////////////
			m_bFirstChk = FALSE;
			UpdateData(FALSE);
			//////////////////////
			PosChk_Thread_Datas.bExitFlag = TRUE;
			m_bProcess = false;
		}

		break;
	}
	CDialog::OnTimer(nIDEvent);
}


void CManuMeasWidth::OnChkFirstchk() 
{
	return;
	if(m_bProcess) 
	{
		UpdateData(FALSE);
		return;
	}
	// by ckh 2009.02.10
	if(m_bFirstChk == TRUE)
	{	
		m_bFirstChk = FALSE;
		UpdateData(FALSE);
		return;
	}

// 기준 마크 값 저장하기.			
	CNormalMsg dlgNorMsg;
	dlgNorMsg.m_strTitle = "조심하세요.";
	dlgNorMsg.m_strMsg1 = "모터 정위치 티칭 값을 다시 저장합니다. 저장시 모터 정위치 기준 값이 변경 됩니다. 실행하겠습니까?";
	dlgNorMsg.m_strMsg2 = "실행하려면 '실행', 아니면 '취소'를 누르십시오.";
	if(dlgNorMsg.DoModal() == IDOK)
		UpdateData(TRUE);
	else // by ckh 2009.02.10
	{
		//////////////////////
		m_bFirstChk = FALSE;
		UpdateData(FALSE);
		//////////////////////
	}

}

void CManuMeasWidth::OnClickCmdMarkSearch() 
{
 
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	CNormalMsg m_NormalMsg;
	ThreadStage.PtpMoveCode='p';
	PosChk_Datas[ThreadStage.nMoveHeadNum].nSelectPos = 1;

	if(pView->m_nLanguage == 0)
	{
		m_NormalMsg.m_strMsg1 = "정위치 측정 설정 위치로 움직입니다."; 
		m_NormalMsg.m_bTimer=TRUE;
		m_NormalMsg.m_strTitle="PTP 구동";
		m_NormalMsg.m_strMsg2 = " 실행하려면 OK버튼을, 취소하려면 방향버튼을 누르시오.";
	}
	else if(pView->m_nLanguage == 1)
	{
		m_NormalMsg.m_strMsg1 = "Marchine will operate to accuracy Setting position."; 
		m_NormalMsg.m_bTimer=TRUE;
		m_NormalMsg.m_strTitle="PTP Action";
		m_NormalMsg.m_strMsg2 = " if you will Action 'OK', or Cancel please Push direction button.";
	} 
	else if(pView->m_nLanguage == 2)
	{
		m_NormalMsg.m_strMsg1 = "移動正位置測量設置位置."; 
		m_NormalMsg.m_bTimer=TRUE;
		m_NormalMsg.m_strTitle="PTP 驅動";
		m_NormalMsg.m_strMsg2 = " 如需執行點擊 'OK', 否則點擊CANCEL.";
	}

	m_NormalMsg.DoModal();
// 	if(m_bProcess) return;
// 	UpdateData(TRUE);
// 
// 	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
// 	CP8CA_LcDispView* m_pLcView = (CP8CA_LcDispView*)pFrame->GetActiveView();
// 	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
// 
// 	int i;
// 	int N;
// 	double dArea;
// 	double dCx, dCy;
// 	CString  str, strTotal1="",strTotal2="";
// 	long left, top, right, bottom;
// 	LPBYTE fm;
// 	///////////////////////////////////////////////////////////////
// 	CRect rect;
// 	GetClientRect( rect );
// 	rect.left = 340;
// 	rect.top = 200;
// 	rect.right = rect.left + 320;
// 	rect.bottom = rect.top + 480;
// 	///////////////////////////////////////////////////////////////
// // 	m_ctrlDisplay0.SetWindowPos(NULL, 0,0,320,480,SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
// // 	m_ctrlDisplay1.MoveWindow(rect);
// 
// 	PosChk_Thread_Datas.lSearchColor = WHITE; // 찾야할 마크 찾기// 흰색 찾기
// 
// 	m_ctrlLCSearch.SetBackColor(YELLOW);
// 	m_ctrlMarkSearch.SetBackColor(GREEN);
// 	//-----------------------------------------
// 	m_pLcView->pVision->ClearOverlay(MUX1);
// 	m_pLcView->pVision->ClearOverlay(MUX2);
// 
// 	m_pLcView->pVision->m_pMark->PMSetPatternId(0);
// 	fm=(LPBYTE)grabber.GetImagePtr(0);
// 	m_pLcView->pVision->m_pMark->PMSetSearchFrame((long)fm);
// 	//-----------------------------------------
// 	m_pLcView->pVision->m_pMark->BLOBSetObjectColor(PosChk_Thread_Datas.lSearchColor); 
// 
// 	N=m_pLcView->pVision->m_pMark->BLOBProcessArea(0,0,639,470);
// 
// 	strTotal1.Format("Ch1\nN   Area    Cx      Cy\n");
// 	for(i=0;i<N;i++)
// 	{
// 		dArea=m_pLcView->pVision->m_pMark->BLOBGetArea(i);
// 		m_pLcView->pVision->m_pMark->BLOBGetCenter(i, &dCx, &dCy);
// 		str.Format("%d  %6.lf   %6.2lf   %6.2lf\n",i, dArea, dCx,dCy);
// 		strTotal1+=str;
// 
// 		if(dArea > 1000) // 최소 사이즈임 더 작게 해도 됨
// 		{
// 			m_pLcView->pVision->m_pMark->BLOBGetRectangle(i, &left, &top, &right, &bottom);
// 			m_ctrlDisplay0.DrawRect(left, top, right, bottom, RGB(0,0,255));
// 		}// 
// 	}
// 
// 	//-----------------------------------------
// 	m_pLcView->pVision->m_pMark->PMSetPatternId(1);
// 	fm=(LPBYTE)grabber.GetImagePtr(1);
// 	m_pLcView->pVision->m_pMark->PMSetSearchFrame((long)fm);
// 	//-----------------------------------------
// 	m_pLcView->pVision->m_pMark->BLOBSetObjectColor(PosChk_Thread_Datas.lSearchColor); // 흰색 찾기
// 
// 	N=m_pLcView->pVision->m_pMark->BLOBProcessArea(0,0,639,470);
// 
// 	str.Format("Ch2\nN   Area    Cx      Cy\n");
// 	strTotal2+=str;
// 	for(i=0;i<N;i++)
// 	{
// 		dArea=m_pLcView->pVision->m_pMark->BLOBGetArea(i);
// 		m_pLcView->pVision->m_pMark->BLOBGetCenter(i, &dCx, &dCy);
// 		str.Format("%d  %6.lf   %6.2lf   %6.2lf\n",i, dArea, dCx,dCy);
// 		strTotal2+=str;
// 
// 		if(dArea > 1000)
// 		{
// 			m_pLcView->pVision->m_pMark->BLOBGetRectangle(i, &left, &top, &right, &bottom);
// 			m_ctrlDisplay1.DrawRect(left, top, right, bottom, RGB(0,0,255));
// 		}// 
// 	}
// 	
// 	// by ckh 2009.01.29
// 	if(m_nHeadSelectNo < MAX_NOZZLE/2)
// 		strTotal2 = "";
// 	else
// 		strTotal1 = "";
// 	MessageBox(strTotal1+strTotal2);
// 
// 	grabber.Live(0);
// 	grabber.Live(1);	
// 
// 	// by ckh 2008.10.05
// 	// 마지막에 선택된 Head의 데이터
// 	SetDisplayMarkData(m_nHeadSelectNo);
}

// by ckh 2008.10.01
void CManuMeasWidth::SetBut_HeadColor(int nHead,long lColor)
{
	switch(nHead)
	{
	case	0	:
		m_ctrlHead1.SetBackColor(lColor);
		break;
	case	1	:
		m_ctrlHead2.SetBackColor(lColor);
		break;
	case	2	:
		m_ctrlHead3.SetBackColor(lColor);
		break;
	case	3	:
		m_ctrlHead4.SetBackColor(lColor);
		break;
	case	4	:
		m_ctrlHead5.SetBackColor(lColor);
		break;
	case	5	:
		m_ctrlHead6.SetBackColor(lColor);
		break;
	case	6	:
		m_ctrlHead7.SetBackColor(lColor);
		break;
	case	7	:
		m_ctrlHead8.SetBackColor(lColor);
		break;
	case	8	:
		m_ctrlHead9.SetBackColor(lColor);
		break;
	case	9	:
		m_ctrlHead10.SetBackColor(lColor);
		break;
	case	10	:
		m_ctrlHead11.SetBackColor(lColor);
		break;
	case	11	:
		m_ctrlHead12.SetBackColor(lColor);
		break;
	case	12	:
		m_ctrlHead13.SetBackColor(lColor);
		break;
	case	13	:
		m_ctrlHead14.SetBackColor(lColor);
		break;
	case	14	:
		m_ctrlHead15.SetBackColor(lColor);
		break;
	case	15	:
		m_ctrlHead16.SetBackColor(lColor);
		break;
	}

}
// by ckh 
// 카메라 선택 및 모터 구동 선택.
BOOL CManuMeasWidth::SetSelCam(int nHeadCam)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CString str;
//	int i;

	if(PosChk_Datas[nHeadCam].bSel == FALSE)
	{
		SetBut_HeadColor(nHeadCam, GREEN);
		PosChk_Datas[nHeadCam].bSel = TRUE;
		m_ctrlJog.SetEnabled(TRUE);
	}
	else
	{
		SetBut_HeadColor(nHeadCam, LIGHTGRAY);
		PosChk_Datas[nHeadCam].bSel = FALSE;
		m_ctrlJog.SetEnabled(FALSE);
		return FALSE;
	}

	// by ckh 2009.01.28 수정 , 함수로 변경함.
#if _VISION
	if(nHeadCam == 0)
		SetCamViewr(nHeadCam);
	else if(nHeadCam == 15)
		SetCamViewr(nHeadCam);
#endif
// 	///////////////////////////////////////////////////////////////
// 	CRect rect;
// 	GetClientRect( rect );
// 	rect.left = 10;
// 	rect.top = 200;
// 	rect.right = rect.left + 640;
// 	rect.bottom = rect.top + 480;
// 	///////////////////////////////////////////////////////////////
// 
// 	if(nHeadCam < MAX_NOZZLE/2) // 카메라 1번부터 시작함으로.
// 	{
// 		m_nViewMode = MUX1;
// 
// 		for(i=0;i<4;i++) // 조명 채널 선택 Off
// 			FAS_SetIoBit(1,TRUE,LIGHT1_WRITE1+i,FALSE);
// 
// 		m_ctrlDisplay0.SetWindowPos(NULL, 0,0,640,480,SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
// 		m_ctrlDisplay1.SetWindowPos(NULL, 0,0,0,0,SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
// 
// 	}
// 	else
// 	{
// 		m_nViewMode = MUX2;
// 
// 		for(i=0;i<4;i++) // 조명 채널 선택 Off
// 			FAS_SetIoBit(1,FALSE,LIGHT2_WRITE1+i,FALSE);
// 
// 		m_ctrlDisplay1.MoveWindow(rect);
// 		m_ctrlDisplay0.SetWindowPos(NULL, 0,0,0,0,SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
// 
//	}
//////////////////////////////////////////
// by ckh 2009.01.31
	// 선택 헤더 이름 표시
	str.Format("Head %d",nHeadCam+1);

//	pView->m_pDevice->CameraSelect(nHeadCam+1);
#if _VISION
	pView->pVision->Grab(m_nViewMode);
	// 선택된 헤더의 영상에서 마크 위치 결과 값 표시함. 
	SetDisplayMarkData(nHeadCam);
	pView->pVision->CrossDisplay(m_nViewMode,str,320,240);
#endif
//////////////////////////////////////////

	// 조그, 위치 저장  버튼 이름 변경
	m_ctrlCmdSave.SetCaption(str + " 현재 위치 저장 ");
	m_ctrlJog.SetCaption(str +" Jog");

	m_ctrlLabName.SetCaption(str + " Light Level");

	// by ckh 2009.01.31
//	if(PosChk_Datas[m_nHeadSelectNo].bLightUse == TRUE)// by ckh 2009.01.29
	{
		str.Format("%d",m_nLightVal[nHeadCam]);
		m_ctrlLabLightBright.SetCaption(str);
	}


	return TRUE;
}

// by ckh 2008.10.05
void CManuMeasWidth::SetDisplayMarkData(int nHeadNum)
{
	if(m_bProcess) return;
	UpdateData(TRUE);

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* m_pLcView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	int i;
	int N;
	double dArea, dCx, dCy;
	CString  str, strTotal="";
	long left, top, right, bottom;
	LPBYTE fm;
	int nMux;
	BOOL bFind = FALSE;
	//--------------------------------------------------------------------
	if(nHeadNum < MAX_NOZZLE/2)
		nMux = MUX1;
	else
		nMux = MUX2;

	m_pLcView->pVision->m_pMark->PMSetPatternId(nMux);
	fm=(LPBYTE)grabber.GetImagePtr(nMux);
	m_pLcView->pVision->m_pMark->PMSetSearchFrame((long)fm);
	//-------------------------------------------------------------------
	m_pLcView->pVision->m_pMark->BLOBSetObjectColor(PosChk_Thread_Datas.lSearchColor);

	N=m_pLcView->pVision->m_pMark->BLOBProcessArea(0,0,639,470);

	for(i=0;i<N;i++)
	{
		dArea=m_pLcView->pVision->m_pMark->BLOBGetArea(i);
		if(dArea >1000)
		{
			m_pLcView->pVision->m_pMark->BLOBGetCenter(i, &dCx, &dCy);

			if(( PosChk_Datas[nHeadNum].dMarkSizeMin < dArea) && (dArea < PosChk_Datas[nHeadNum].dMarkSizeMax))
			{
			/* //마크의 화면상에서의 좌표
				str.Format("%.3f",dCx);
				m_ctrlCenterPosX.SetCaption(str);
				str.Format("%.3f",dCy);
				m_ctrlCenterPosY.SetCaption(str);
			*/
				str.Format("%.3f",PosChk_Datas[nHeadNum].dPosX+(dCx - 320)*PIXEL_SIZE);
				m_ctrlCenterPosX.SetCaption(str);
				str.Format("%.3f",PosChk_Datas[nHeadNum].dPosY-(dCy - 240)*PIXEL_SIZE);
				m_ctrlCenterPosY.SetCaption(str);
				str.Format("%.3f",(dCx - 320)*PIXEL_SIZE);
				m_ctrlDistX.SetCaption(str);
				str.Format("%.3f",-(dCy - 240)*PIXEL_SIZE);
				m_ctrlDistY.SetCaption(str);
				bFind = TRUE;

				m_pLcView->pVision->m_pMark->BLOBGetRectangle(i, &left, &top, &right, &bottom);
				if(nMux == MUX1)
				{
					m_ctrlDisplay0.DrawRect(left, top, right, bottom, RGB(0,0,255));
					m_ctrlDisplay0.DrawCrossLine(320, 240, 320, RED);
				}
				else
				{
					m_ctrlDisplay1.DrawRect(left, top, right, bottom, RGB(0,0,255));
					m_ctrlDisplay1.DrawCrossLine(320, 240, 320, RED);
				}
			}
		}
	}
/////////////////////////////////////////////////////////////////////////
// 마크 못찾았을 경우

	if(bFind != TRUE)
	{
			str.Format("-----");
			m_ctrlCenterPosX.SetCaption(str);
			m_ctrlCenterPosY.SetCaption(str);
			m_ctrlDistX.SetCaption(str);
			m_ctrlDistY.SetCaption(str);
	}
	// by ckh 2009.01.31
	m_ctrlDisplay0.DrawCrossLine(320, 240, 320, RED);
	m_ctrlDisplay1.DrawCrossLine(320, 240, 320, RED);

	//화면에 그리기 
	for(i=0;i<N;i++)
	{
// 		m_pLcView->pVision->m_pMark->BLOBGetRectangle(i, &left, &top, &right, &bottom);
// 		if(nMux == MUX1)
// 		{
// 			m_ctrlDisplay0.DrawRect(left, top, right, bottom, RGB(0,0,255));
// 			m_ctrlDisplay0.DrawCrossLine(320, 240, 320, RED);
// 		}
// 		else
// 		{
// 			m_ctrlDisplay1.DrawRect(left, top, right, bottom, RGB(0,0,255));
// 			m_ctrlDisplay1.DrawCrossLine(320, 240, 320, RED);
// 		}
	}

	grabber.Live(0);
	grabber.Live(1);
}


void CManuMeasWidth::OnClickCmdLcSearch() 
{

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	CNormalMsg m_NormalMsg;
	ThreadStage.PtpMoveCode='p';
	PosChk_Datas[ThreadStage.nMoveHeadNum].nSelectPos = 0;

	if(pView->m_nLanguage == 0)
	{
		m_NormalMsg.m_strMsg1 = "정위치 측정 기준 위치로 움직입니다."; 
		m_NormalMsg.m_bTimer=TRUE;
		m_NormalMsg.m_strTitle="PTP 구동";
		m_NormalMsg.m_strMsg2 = " 실행하려면 OK버튼을, 취소하려면 방향버튼을 누르시오.";
	}
	else if(pView->m_nLanguage == 1)
	{
		m_NormalMsg.m_strMsg1 = "Marchine will operate to accuracy Calibration position."; 
		m_NormalMsg.m_bTimer=TRUE;
		m_NormalMsg.m_strTitle="PTP Action";
		m_NormalMsg.m_strMsg2 = " if you will Action 'OK', or Cancel please Push direction button.";
	}
	else if(pView->m_nLanguage == 2)
	{
		m_NormalMsg.m_strMsg1 = "정위치 측정 기준 위치로 움직입니다."; 
		m_NormalMsg.m_bTimer=TRUE;
		m_NormalMsg.m_strTitle="PTP 驅動";
		m_NormalMsg.m_strMsg2 = "실행하려면 OK버튼을, 취소하려면 방향버튼을 누르시오.";
	}

	
	m_NormalMsg.DoModal();	
// 		if(m_bProcess) return;
// 	UpdateData(TRUE);
// 
// 	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
// 	CP8CA_LcDispView* m_pLcView = (CP8CA_LcDispView*)pFrame->GetActiveView();
// 	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
// 
// 	int i;
// 	int N;
// 	double dArea;
// 	double dCx, dCy;
// 	CString  str, strTotal="";
// 	long left, top, right, bottom;
// 	LPBYTE fm;
// 	
// 	m_ctrlLCSearch.SetBackColor(GREEN);
// 	m_ctrlMarkSearch.SetBackColor(YELLOW);
// 	//-----------------------------------------
// 	m_pLcView->pVision->ClearOverlay(MUX1);
// 	m_pLcView->pVision->ClearOverlay(MUX2);
// 
// 	m_pLcView->pVision->m_pMark->PMSetPatternId(0);
// 	fm=(LPBYTE)grabber.GetImagePtr(0);
// 	m_pLcView->pVision->m_pMark->PMSetSearchFrame((long)fm);
// 	//-----------------------------------------
// 	m_pLcView->pVision->m_pMark->BLOBSetObjectColor(BLACK); // 검은색 찾기
// 
// 	N=m_pLcView->pVision->m_pMark->BLOBProcessArea(0,0,639,470);
// 
// 	strTotal.Format("Ch1\nN   Area    Cx      Cy\n");
// 	for(i=0;i<N;i++)
// 	{
// 		dArea=m_pLcView->pVision->m_pMark->BLOBGetArea(i);
// 		m_pLcView->pVision->m_pMark->BLOBGetCenter(i, &dCx, &dCy);
// 		str.Format("%d  %6.lf   %6.2lf   %6.2lf\n",i, dArea, dCx,dCy);
// 		strTotal+=str;
// 	}
// 
// 	//화면에 그리기 
// 	for(i=0;i<N;i++)
// 	{
// 		m_pLcView->pVision->m_pMark->BLOBGetRectangle(i, &left, &top, &right, &bottom);
// 		m_ctrlDisplay0.DrawRect(left, top, right, bottom, RGB(0,0,255));
// 	}
// 
// 	//-----------------------------------------
// 	m_pLcView->pVision->m_pMark->PMSetPatternId(1);
// 	fm=(LPBYTE)grabber.GetImagePtr(1);
// 	m_pLcView->pVision->m_pMark->PMSetSearchFrame((long)fm);
// 	//-----------------------------------------
// 	m_pLcView->pVision->m_pMark->BLOBSetObjectColor(BLACK); // 검은색 찾기
// 
// 	N=m_pLcView->pVision->m_pMark->BLOBProcessArea(0,0,639,470);
// 
// 	str.Format("Ch2\nN   Area    Cx      Cy\n");
// 	strTotal+=str;
// 	for(i=0;i<N;i++)
// 	{
// 		dArea=m_pLcView->pVision->m_pMark->BLOBGetArea(i);
// 		m_pLcView->pVision->m_pMark->BLOBGetCenter(i, &dCx, &dCy);
// 		str.Format("%d  %6.lf   %6.2lf   %6.2lf\n",i, dArea, dCx,dCy);
// 		strTotal+=str;
// 	}
// 
// 	//화면에 그리기 
// 	for(i=0;i<N;i++)
// 	{
// 		m_pLcView->pVision->m_pMark->BLOBGetRectangle(i, &left, &top, &right, &bottom);
// 		m_ctrlDisplay1.DrawRect(left, top, right, bottom, RGB(0,0,255));
// 	}
// 
// 	MessageBox(strTotal);
// 
// 	grabber.Live(0);
// 	grabber.Live(1);	
// 
// 	// by ckh 2008.10.05
// 	// 마지막에 선택된 Head의 데이터
// 	SetDisplayMarkData(m_nHeadSelectNo);
}

void CManuMeasWidth::OnDeltaposSpinlightlevel(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(m_bProcess != TRUE)
	{
		int nSpinDelta = pNMUpDown->iDelta;
		CString strBright;
		int nBright = atoi( m_ctrlLabLightBright.GetCaption());

		if(nBright>255)
		{
			nBright = 254;
		}
		if( nBright <0)
		{
			nBright = 1;
		}
		m_ctrlSpenLightLevel.SetPos(nBright);

		if(nSpinDelta == -1)
			++nBright;
		else if(nSpinDelta == +1)
			--nBright;
		m_ctrlSpenLightLevel.SetPos(nBright);

		strBright.Format("%d",nBright);
		m_ctrlLabLightBright.SetCaption(strBright);

		pView->m_pDevice->SetLightBright(m_nHeadSelectNo,atoi(m_ctrlLabLightBright.GetCaption()));
	}
	*pResult = 0;
}
#include "LcLib.h"
void CManuMeasWidth::OnClickLabLightlevel() 
{
	// by ckh 209.01.09
	if(m_bProcess == TRUE)
		return;

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
 
	int row = 0 , col = 0;
	
	Use_TK(m_ctrlLabLightBright, 255, 0, 400, 200); // by ckh 2009.01.31
	
	pView->m_pDevice->SetLightBright(m_nHeadSelectNo,atoi(m_ctrlLabLightBright.GetCaption()));

	// by ckh 2009.01.31
	m_nLightVal[m_nHeadSelectNo] = atoi(m_ctrlLabLightBright.GetCaption());
		
}

#include "TeachingPosChk.h"
void CManuMeasWidth::OnClickCmdTeachingdata() 
{
	// by ckh 209.01.09
	if(m_bProcess == TRUE)
		return;

	CTeachingPosChk dlg;
	dlg.DoModal();
}

#include "ManuMeasPosResult.h"
void CManuMeasWidth::OnClickCmdPoschkResult() 
{
	if(m_bProcess == TRUE)
		return;

	CManuMeasPosResult dlg;
	dlg.DoModal();
	
}

void CManuMeasWidth::OnClickCmdAlignSearch() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	
	// 인터록 체크를 여기서 할까~~!!!!????

	// 헤더 16개 축 선택하여 테스트 진행, 임의 선택 사항은 추후 진행.( 체크 버튼으로 하자..)
	if (m_bProcess == TRUE)
	{
		return;
	}
	m_bProcess = TRUE; // 작업 시작 플렝그
	m_bButExect = TRUE;
	// PosChk 스레드 스텝 변경하기.
	m_ctrlAlignSearch.SetBackColor(RED);
	m_ctrlAlignSearch.SetCaption("Alig Mark Search 실행 중");
  	UpdateData(true);

	// by ckh 2008.12.26
	// 모드 추가.
	// 얼라인 모드도 여기에 추가하기.( 얼라인 찾기 서치 Flag추가하기.)
	PosChk_Thread_Datas.nMode = MODE_ALIGN;// 얼라인 마크 찾기 
	// 시작 버튼 감지를 위함.
	SetTimer(START_POSCHK,500,NULL);	
}

void CManuMeasWidth::OnClickCmdTeachingAlignmark() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	// by ckh 2009.02.10
	int nSel=0;
	LPBYTE fm=(LPBYTE)grabber.GetImagePtr(nSel);
	
	int m_nPanX=0,m_nPanY=0,m_dZoom=1;
	if(m_nHeadSelectNo==0)
	{
		//nSel = 0;
		nSel = 2;
		fm=(LPBYTE)grabber.GetImagePtr(MUX1);
		pView->pVision->m_pMark->PMSetSearchFrame((long)fm);
		pView->pVision->m_pMark->PMDialog(nSel, (long)m_ctrlDisplay0.m_hWnd, 640,480,m_nPanX,m_nPanY,m_dZoom);	

	}
	else if(m_nHeadSelectNo==15)
	{
		//nSel = 1;
		nSel = 3;
		fm=(LPBYTE)grabber.GetImagePtr(MUX2);
		pView->pVision->m_pMark->PMSetSearchFrame((long)fm);
		pView->pVision->m_pMark->PMDialog(nSel, (long)m_ctrlDisplay1.m_hWnd, 640,480,m_nPanX,m_nPanY,m_dZoom);	

	}



}

void CManuMeasWidth::OnClickCmdPmload() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	pView->pVision->m_pMark->PMLoad("D:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\TFTAlign.dat");
	
}

void CManuMeasWidth::OnClickCmdPmsave() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	pView->pVision->m_pMark->PMSave("D:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\TFTAlign.dat");	
}

void CManuMeasWidth::OnClickCmdMarkAlign() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	double dRx=0, dRy=0, dAngle=0;
	long nScore;
	CString str,strTemp;
	int nSearch=0;
	LPBYTE fm=(LPBYTE)grabber.GetImagePtr(0);

	pView->pVision->ClearOverlay(MUX1);
	pView->pVision->ClearOverlay(MUX2);
	if(m_nHeadSelectNo == 0)
		fm=(LPBYTE)grabber.GetImagePtr(MUX1);
	else if(m_nHeadSelectNo == 15)	
		fm=(LPBYTE)grabber.GetImagePtr(MUX2);

	pView->pVision->m_pMark->PMSetSearchFrame((long)fm);

	//패턴 번호 찾기
	if(m_nHeadSelectNo == 0)
		pView->pVision->m_pMark->PMSetPatternId(MUX1+2);
	else if(m_nHeadSelectNo == 15)	
		pView->pVision->m_pMark->PMSetPatternId(MUX2+2);

	nSearch = pView->pVision->m_pMark->PMSearch();
	if(nSearch<0)
	{
		// 0번 패턴을 못찾았을 경우 1번 패턴을 재 서치 함. 
		//패턴 번호 찾기
		if(m_nHeadSelectNo == 0)
			pView->pVision->m_pMark->PMSetPatternId(MUX1+2);
		else if(m_nHeadSelectNo == 15)	
			pView->pVision->m_pMark->PMSetPatternId(MUX2+2);
		nSearch = pView->pVision->m_pMark->PMSearch();
	}

	if(nSearch > 0)
	{
		pView->pVision->m_pMark->PMGetReturn(0, &dRx, &dRy, &nScore, &dAngle);
	}
	
	// 선택 헤더 이름 표시
	str.Format("Head %d",m_nHeadSelectNo+1);
	pView->pVision->CrossDisplay(m_nViewMode,str,320,240);

	str.Format("(%7.3lf, %7.3lf)  Score: %d", (320 - dRx)*10, (dRy-240)*10, nScore);
	// 얼라인 마크 센터 표시
	if(m_nHeadSelectNo< MAX_NOZZLE/2)
	{
		m_ctrlDisplay0.DrawCrossLine((long)dRx,(long)dRy, 30, RGB(255,0,0));
		m_ctrlDisplay0.TextOut(str, (long)dRx, (long)dRy, GREEN);
	}
	else
	{
		m_ctrlDisplay1.DrawCrossLine((long)dRx, (long)dRy, 30, RGB(255,0,0));
		m_ctrlDisplay1.TextOut(str, (long)dRx, (long)dRy, GREEN);
	}

	if(!grabber.IsLive(0)) m_ctrlDisplay0.UpdateDisplay();
	if(!grabber.IsLive(0)) m_ctrlDisplay1.UpdateDisplay();

	
}
// by ckh 2009.01.28 추가함.
void CManuMeasWidth::SetCamViewr(int nHeadCam)
{
//	int i;
	///////////////////////////////////////////////////////////////
	CRect rect;
	GetClientRect( rect );
	rect.left = 10;
	rect.top = 200;
	rect.right = rect.left + 640;
	rect.bottom = rect.top + 480;
	///////////////////////////////////////////////////////////////

	if(nHeadCam == 0) // 카메라 1번부터 시작함으로.
	{
		m_nViewMode = MUX1;

// 		for(i=0;i<4;i++) // 조명 채널 선택 Off
// 			FAS_SetIoBit(1,TRUE,LIGHT1_WRITE1+i,FALSE);

		m_ctrlDisplay0.SetWindowPos(NULL, 0,0,640,480,SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
		m_ctrlDisplay1.SetWindowPos(NULL, 0,0,0,0,SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );

	}
	else if(nHeadCam == 15)
	{
		m_nViewMode = MUX2;

// 		for(i=0;i<4;i++) // 조명 채널 선택 Off
// 			FAS_SetIoBit(1,TRUE,LIGHT1_WRITE1+i,FALSE);

		m_ctrlDisplay0.SetWindowPos(NULL, 0,0,0,0,SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
		m_ctrlDisplay1.SetWindowPos(NULL, 0,0,640,480,SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	}

}

void CManuMeasWidth::SelectLanguage()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(pView->m_nLanguage == 0)	//ehji 140309
	{
		SetDlgItemText(IDC_LABEL_TITLE3,				_T("모터 위치 확인"));
		SetDlgItemText(IDC_LAB_NAME,				_T("Head 1 조명밝기 설정"));
		SetDlgItemText(IDC_LABEL3,				_T("위치 결과"));
		SetDlgItemText(IDC_LABEL7,				_T("중심점 X"));
		SetDlgItemText(IDC_LABEL14,				_T("중심점 Y :"));
		SetDlgItemText(IDC_LABEL27,				_T("오차거리 X :"));
		SetDlgItemText(IDC_LABEL29,				_T("오차거리 Y :"));
		SetDlgItemText(IDC_CMD_SAVE,				_T("현재 위치 저장"));
		SetDlgItemText(IDC_CMD_POSCHK_RESULT,				_T("정위치 실행 결과"));
		SetDlgItemText(IDC_CMD_POSCHK_RESULT2,				_T("Align Mark 실행 결과"));
		SetDlgItemText(IDC_CMD_POSCHK,				_T("정위치 실행"));
		SetDlgItemText(IDC_CMD_ALIGN_SEARCH,				_T("Align Mark Search(미적용)"));
		SetDlgItemText(IDC_CMD_LC_SEARCH,				_T("기준 위치 이동"));
		SetDlgItemText(IDC_CMD_MARK_SEARCH,				_T("설정 위치 이동"));
		SetDlgItemText(IDC_LABEL10,				_T("Engineer 모드"));
		SetDlgItemText(IDC_CHK_FIRSTCHK,				_T("기준 위치설정"));
		SetDlgItemText(IDC_CMD_TEACHING_ALIGNMARK,				_T("Mark 등록"));
		SetDlgItemText(IDC_CMD_MOVE_POS1,				_T("확인 위치1"));
		SetDlgItemText(IDC_CMD_MOVE_POS2,				_T("확인 위치2"));
		SetDlgItemText(IDC_CMD_BLOB,				_T("중심값 확인"));
		SetDlgItemText(IDC_CMD_MEAS,				_T("실행"));
	}

	else if(pView->m_nLanguage == 1)	//ehji 140309
	{
		SetDlgItemText(IDC_LABEL_TITLE3,				_T("Motor Position Check"));
		SetDlgItemText(IDC_LAB_NAME,				_T("Head 1 Brightness Setting"));
		SetDlgItemText(IDC_LABEL3,				_T("Position Result"));
		SetDlgItemText(IDC_LABEL7,				_T("Center Point X"));
		SetDlgItemText(IDC_LABEL14,				_T("Center Point Y :"));
		SetDlgItemText(IDC_LABEL27,				_T("Error Distence X :"));
		SetDlgItemText(IDC_LABEL29,				_T("Error Distence Y :"));
		SetDlgItemText(IDC_CMD_SAVE,				_T("Current Position Save"));
		SetDlgItemText(IDC_CMD_POSCHK_RESULT,				_T("Ture Position Execution Result"));
		SetDlgItemText(IDC_CMD_POSCHK_RESULT2,				_T("Align Mark Execution Result"));
		SetDlgItemText(IDC_CMD_POSCHK,				_T("Ture Position Execution"));
		SetDlgItemText(IDC_CMD_ALIGN_SEARCH,				_T("Align Mark Search(Not apply)"));
		SetDlgItemText(IDC_CMD_LC_SEARCH,				_T("Standard Position Move"));
		SetDlgItemText(IDC_CMD_MARK_SEARCH,				_T("Setting Position Move"));
		SetDlgItemText(IDC_LABEL10,				_T("Engineer Mode"));
		SetDlgItemText(IDC_CHK_FIRSTCHK,				_T("Standard Position Setting"));
		SetDlgItemText(IDC_CMD_TEACHING_ALIGNMARK,				_T("Mark Resident"));
		SetDlgItemText(IDC_CMD_MOVE_POS1,				_T("Confirm Position1"));
		SetDlgItemText(IDC_CMD_MOVE_POS2,				_T("Confirm Position2"));
		SetDlgItemText(IDC_CMD_BLOB,				_T("Center Value Check"));
		SetDlgItemText(IDC_CMD_MEAS,				_T("Execution"));
	}

	else if(pView->m_nLanguage == 2)	//ehji 140309
	{
		SetDlgItemText(IDC_LABEL_TITLE3,				_T("Motor 位置確認"));
		SetDlgItemText(IDC_LAB_NAME,				_T("Head 1 照明亮度 設定"));
		SetDlgItemText(IDC_LABEL3,				_T("位置結果"));
		SetDlgItemText(IDC_LABEL7,				_T("中心點 X"));
		SetDlgItemText(IDC_LABEL14,				_T("中心點 Y :"));
		SetDlgItemText(IDC_LABEL27,				_T("誤差距離 X :"));
		SetDlgItemText(IDC_LABEL29,				_T("誤差距離 Y :"));
		SetDlgItemText(IDC_CMD_SAVE,				_T("現在位置保存"));
		SetDlgItemText(IDC_CMD_POSCHK_RESULT,				_T("正文職實行結果"));
		SetDlgItemText(IDC_CMD_POSCHK_RESULT2,				_T("Align Mark 實行結果"));
		SetDlgItemText(IDC_CMD_POSCHK,				_T("正文職實行"));
		SetDlgItemText(IDC_CMD_ALIGN_SEARCH,				_T("Align Mark Search(未适用)"));
		SetDlgItemText(IDC_CMD_LC_SEARCH,				_T("基本位置移動"));
		SetDlgItemText(IDC_CMD_MARK_SEARCH,				_T("設定位置移動"));
		SetDlgItemText(IDC_LABEL10,				_T("Engineer 模式"));
		SetDlgItemText(IDC_CHK_FIRSTCHK,				_T("基本位置設定"));
		SetDlgItemText(IDC_CMD_TEACHING_ALIGNMARK,				_T("Mark 登陸"));
		SetDlgItemText(IDC_CMD_MOVE_POS1,				_T("確認 位置1"));
		SetDlgItemText(IDC_CMD_MOVE_POS2,				_T("確認 位置2"));
		SetDlgItemText(IDC_CMD_BLOB,				_T("中心値 確認"));
		SetDlgItemText(IDC_CMD_MEAS,				_T("實行"));
	}
}

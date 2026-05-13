// ManuPatDrop.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"
//
#include "ManuPatDrop.h"
#include "AddDelInput.h"

#include "RepairEdit.h"

#include "PanelNGSet.h"
#include "PanelInformInput.h"
#include "NormalMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// extern variable 선언..
extern CIM_DATA CIM_Information;
extern THREADSTAGE ThreadStage;
extern long global_Sequence_Count;
extern long g_nDrop_Repeat_Count;
extern BOOL g_bPumpOringFlag;
//
#define ID_PATTERN_WND		21001

extern BOOL m_bSWCheck;
extern BOOL g_bRemoteControl;

//2013.08.03 by tskim
extern BOOL g_bMMGLineDropMode;
/////////////////////////////////////////////////////////////////////////////
// CManuPatDrop dialog


CManuPatDrop::CManuPatDrop(CWnd* pParent /*=NULL*/)
	: CDialog(CManuPatDrop::IDD, pParent)
{
	//{{AFX_DATA_INIT(CManuPatDrop)
	//}}AFX_DATA_INIT
//	Selected_sequence_count = 0;
	m_bAllPattern = TRUE;
	m_pPatternWnd = NULL;
	m_nCurrentPatternNo = 0;
	m_bStart = TRUE;
	m_nTimerCount=0;
	m_nSelectedSequence=0;
	ThreadStage.bDropCountDisp=FALSE;

	m_structDropData = new CArray<DROP_DATA, DROP_DATA>[MAX_PATTERN];
	m_bSWCheck = false;
}

CManuPatDrop::~CManuPatDrop()
{
	if(m_pPatternWnd != NULL) 
	{
		delete m_pPatternWnd;
		m_pPatternWnd = NULL;
	}
}

void CManuPatDrop::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CManuPatDrop)
	DDX_Control(pDX, IDC_COMBO_PATTERN_NO, m_ctrlComboPattern);
	DDX_Control(pDX, IDC_STATIC_SCREEN, m_staticScreen);
	DDX_Control(pDX, IDC_MSFLEXGRID_CIM_ADEL, m_ctrlCimAdel);
	DDX_Control(pDX, IDC_LABEL_ADEL, m_ctrlSetAdel);
	DDX_Control(pDX, IDC_LABEL_PAN_INFORM, m_ctrlSetPanelInform);
	DDX_Control(pDX, IDC_LABEL_RECIPE_NAME_IN_PATDROP, m_ctrlRecipeName);
	DDX_Control(pDX, IDC_CMD_SEE_ALL_PATTERN, m_ctrlSeeAllPattern);
	DDX_Control(pDX, IDC_CMD_HEAD1, m_ctrlHead1);
	DDX_Control(pDX, IDC_LABEL_MESSAGE_MANUPATDROP, m_ctrlMessageManuPatDrop);
	DDX_Control(pDX, IDC_CMD_RETURN, m_ctrlReturn);
	DDX_Control(pDX, IDC_CMD_HEAD2, m_ctrlHead2);
	DDX_Control(pDX, IDC_CMD_HEAD3, m_ctrlHead3);
	DDX_Control(pDX, IDC_CMD_HEAD10, m_ctrlHead10);
	DDX_Control(pDX, IDC_CMD_HEAD11, m_ctrlHead11);
	DDX_Control(pDX, IDC_CMD_HEAD12, m_ctrlHead12);
	DDX_Control(pDX, IDC_CMD_HEAD4, m_ctrlHead4);
	DDX_Control(pDX, IDC_CMD_HEAD5, m_ctrlHead5);
	DDX_Control(pDX, IDC_CMD_HEAD6, m_ctrlHead6);
	DDX_Control(pDX, IDC_CMD_HEAD7, m_ctrlHead7);
	DDX_Control(pDX, IDC_CMD_HEAD8, m_ctrlHead8);
	DDX_Control(pDX, IDC_CMD_HEAD9, m_ctrlHead9);
	DDX_Control(pDX, IDC_REPAIR_EDIT, m_ctrlRepairEdit);
	DDX_Control(pDX, IDC_GLS_STATUS_CLEAR, m_ctrlDropClear);
	DDX_Control(pDX, IDC_GLS_STATUS_SET, m_ctrlDropCompleted);
	DDX_Control(pDX, IDC_MSFLEXGRID_PANEL_INFORM_SET, m_ctrlPanelMatrixSet);
	DDX_Control(pDX, IDC_LABEL_NOZZLE1, m_ctrlHeadlabel1);
	DDX_Control(pDX, IDC_LABEL_NOZZLE10, m_ctrlHeadlabel10);
	DDX_Control(pDX, IDC_LABEL_NOZZLE11, m_ctrlHeadlabel11);
	DDX_Control(pDX, IDC_LABEL_NOZZLE12, m_ctrlHeadlabel12);
	DDX_Control(pDX, IDC_LABEL_NOZZLE2, m_ctrlHeadlabel2);
	DDX_Control(pDX, IDC_LABEL_NOZZLE3, m_ctrlHeadlabel3);
	DDX_Control(pDX, IDC_LABEL_NOZZLE4, m_ctrlHeadlabel4);
	DDX_Control(pDX, IDC_LABEL_NOZZLE5, m_ctrlHeadlabel5);
	DDX_Control(pDX, IDC_LABEL_NOZZLE6, m_ctrlHeadlabel6);
	DDX_Control(pDX, IDC_LABEL_NOZZLE7, m_ctrlHeadlabel7);
	DDX_Control(pDX, IDC_LABEL_NOZZLE8, m_ctrlHeadlabel8);
	DDX_Control(pDX, IDC_LABEL_NOZZLE9, m_ctrlHeadlabel9);
	DDX_Control(pDX, IDC_CMD_HEAD13, m_ctrlHead13);
	DDX_Control(pDX, IDC_CMD_HEAD14, m_ctrlHead14);
	DDX_Control(pDX, IDC_CMD_HEAD15, m_ctrlHead15);
	DDX_Control(pDX, IDC_CMD_HEAD16, m_ctrlHead16);
	DDX_Control(pDX, IDC_LABEL_NOZZLE13, m_ctrlHeadlabel13);
	DDX_Control(pDX, IDC_LABEL_NOZZLE14, m_ctrlHeadlabel14);
	DDX_Control(pDX, IDC_LABEL_NOZZLE15, m_ctrlHeadlabel15);
	DDX_Control(pDX, IDC_LABEL_NOZZLE16, m_ctrlHeadlabel16);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CManuPatDrop, CDialog)
	//{{AFX_MSG_MAP(CManuPatDrop)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO_PATTERN_NO, OnSelchangeComboPatternNo)
	ON_BN_CLICKED(IDC_BTN_CHANGE, OnBtnChange)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_ALL_CHG, OnPanelInformAllChg)
	ON_WM_RBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CManuPatDrop message handlers


BOOL CManuPatDrop::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	SelectLanguage();

	for(int k=0; k<MAX_PATTERN; k++){
		m_structDropData[k].RemoveAll();
	}

	PatDataTmpSave();	// Manual Pattern Drop 최초 한 번 화면에 들어 올 때 Data를 저장한다.

	int i = 0 , j = 0;
	CString str = "", str1="", str2="", str12="";

	m_bStepNotSelError = FALSE;

/*
	m_fontCombo = new CFont;
	m_fontCombo->CreateFont(35,25,0,0,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"Lucida Sans Unicode");
	m_ctrlComboPattern.SetFont(m_fontCombo, TRUE);
*/	
	m_ctrlComboPattern.SetItemHeight(-1, 38);
	for( i=0;i<pDoc->m_structDataEditor.m_nPatternNum;i++)
	{
		m_ctrlComboPattern.SetItemHeight(i,38);
		str.Format("%02d",i+1);
		m_ctrlComboPattern.AddString((LPCSTR)str);
	}
	m_ctrlComboPattern.SetCurSel(0);

	str.Format("%s|%s", pDoc->m_structOperatorModeRecipeData.strFrontRecipeName, pDoc->m_structOperatorModeRecipeData.strBackRecipeName);
	m_ctrlRecipeName.SetCaption(str);
//

// CIM의 Add/Del의 값을 설정한다..
	CIM_Information.Panal_Max_Num = 0;
	for(i=0;i<pDoc->m_structDataEditor.m_nPatternNum;i++)
	{
		for(j=0;j<MAX_NOZZLE;j++)
		{
			if(pDoc->m_structPatternData[i].m_nPanelNo[j] > CIM_Information.Panal_Max_Num)
				CIM_Information.Panal_Max_Num = pDoc->m_structPatternData[i].m_nPanelNo[j];
		}
	}

	for( i = 0 ; i < 10 ; i++)
		m_ctrlCimAdel.SetColWidth(i,535);

	for( i = 0 ; i < 120 ; i++)
		m_ctrlCimAdel.SetRowHeight(i,300);

	for(i = 0; i < 120 ; i++)
	{
		for(j = 0; j < 10 ; j++)
		{
			if((i%4)==0)
			{
				m_ctrlCimAdel.SetCol(j);
				m_ctrlCimAdel.SetRow(i);
				m_ctrlCimAdel.SetCellBackColor(LIGHTGRAY);				
			}
		}
	}

	int pat,head;
	for( i = 0 ; i < 30 ; i++)
	{
		for( j = 0 ; j < 10 ; j++)
		{
			if((10*i+j)<CIM_Information.Panal_Max_Num)
			{
				m_ctrlCimAdel.SetTextMatrix(4*i,j,pDoc->m_strGlassIden[10*i+j]);
				str=pDoc->LevelTostr(CIM_Information.Add_Del_Number[10*i+j]);
				m_ctrlCimAdel.SetTextMatrix(4*i+1,j,str);

				///////////////////////////////////////////////////////
				//여기다 panel 정보의 문자값으로 치환된 값을 보여 준다.
				str1.Format(PanelInformM(CIM_Information.Panel_NG_Mark[TFT][10*i+j]));
				str2.Format(PanelInformM(CIM_Information.Panel_NG_Mark[CF][10*i+j]));
				str12=str1+str2;
				m_ctrlCimAdel.SetTextMatrix(4*i+2,j,str12);
//2015.07.14 by tskim
				for( pat = 0 ; pat < pDoc->m_structDataEditor.m_nPatternNum ; pat++)
				{
					for( head = 0 ; head < MAX_NOZZLE ; head++)
					{
						if((pDoc->m_structPatternData[pat].m_nSequence > 0) && (pDoc->m_structPatternData[pat].m_nPanelNo[head] == 10*i + j +1))
						{
							m_ctrlCimAdel.SetRow(4*i+3); m_ctrlCimAdel.SetCol(j);
							m_ctrlCimAdel.SetCellFontSize(6);
#if PTN_TARGET
							str.Format("%.3f",pDoc->m_structPatternData[pat].m_dTargetWeight_ByPattern[head]);
#else
							str.Format("%.3f",pDoc->m_structAdjustCondition.dTargetWeight[head]);
#endif
							m_ctrlCimAdel.SetTextMatrix(4*i+3,j,str);
						}
					}
				}
			}
		}
	}


	m_ctrlPanelMatrixSet.SetRows(6);m_ctrlPanelMatrixSet.SetCols(6);
	CString str_CF[5] = {"G","Q","N","W","X"};
	CString str_TFT[5] = {"G","Q","N","W","Y"};
	for(i = 0 ; i < 6 ; i++)
	{
		m_ctrlPanelMatrixSet.SetColWidth(i,950);
		m_ctrlPanelMatrixSet.SetRowHeight(i,400);
		m_ctrlPanelMatrixSet.SetColAlignment(i,4);
	}
	m_ctrlPanelMatrixSet.SetTextMatrix(0,0,"CF＼TFT");
	for(i = 1 ; i < 6 ; i++)
	{
		m_ctrlPanelMatrixSet.SetTextMatrix(i,0,str_CF[i-1]);
		m_ctrlPanelMatrixSet.SetTextMatrix(0,i,str_TFT[i-1]);
	}

	for(i = 0 ; i < 25 ; i++)
	{
		m_ctrlPanelMatrixSet.SetRow((i/5)+1);
		m_ctrlPanelMatrixSet.SetCol((i%5)+1);
		m_ctrlPanelMatrixSet.SetCellAlignment(4);
		if(pDoc->m_structDataEditor.m_bNgDropping == TRUE)
		{
			m_ctrlPanelMatrixSet.SetCellBackColor(WHITEGREEN);
			str="All ON";
		}
		else if(CIM_Information.Panel_NG_MarkSet[i]==0)
		{
			m_ctrlPanelMatrixSet.SetCellBackColor(WHITE);
			str="OFF";
		}
		else
		{
			m_ctrlPanelMatrixSet.SetCellBackColor(GREEN);
			str="ON";
		}
		m_ctrlPanelMatrixSet.SetTextMatrix((i/5)+1,(i%5)+1,str);
	}
	// 여기서 모든 Pattern의 그림을 그린다..
	if(m_bAllPattern == TRUE)
	{
		m_ctrlComboPattern.EnableWindow(FALSE);
		m_ctrlSeeAllPattern.SetBackColor(LIGHTGREEN);

		m_ctrlHead1.SetEnabled(FALSE);
		m_ctrlHead2.SetEnabled(FALSE);
		m_ctrlHead3.SetEnabled(FALSE);
		m_ctrlHead4.SetEnabled(FALSE);
		m_ctrlHead5.SetEnabled(FALSE);
		m_ctrlHead6.SetEnabled(FALSE);
		m_ctrlHead7.SetEnabled(FALSE);
		m_ctrlHead8.SetEnabled(FALSE);
		m_ctrlHead9.SetEnabled(FALSE);
		m_ctrlHead10.SetEnabled(FALSE);
		m_ctrlHead11.SetEnabled(FALSE);
		m_ctrlHead12.SetEnabled(FALSE);
		m_ctrlHead13.SetEnabled(FALSE);
		m_ctrlHead14.SetEnabled(FALSE);
		m_ctrlHead15.SetEnabled(FALSE);
		m_ctrlHead16.SetEnabled(FALSE);

		m_ctrlRepairEdit.EnableWindow(FALSE);

	}
	else {
		SubSetControl(m_ctrlComboPattern.GetCurSel());
	}
	
	SubDrawPattern();

	m_ctrlHeadlabel1.SetCaption("16");
	m_ctrlHeadlabel1.SetBackColor(H16);
	m_ctrlHeadlabel2.SetCaption("15");
	m_ctrlHeadlabel2.SetBackColor(H15);
	m_ctrlHeadlabel3.SetCaption("14");
	m_ctrlHeadlabel3.SetBackColor(H14);
	m_ctrlHeadlabel4.SetCaption("13");
	m_ctrlHeadlabel4.SetBackColor(H13);
	m_ctrlHeadlabel5.SetCaption("12");
	m_ctrlHeadlabel5.SetBackColor(H12);
	m_ctrlHeadlabel6.SetCaption("11");
	m_ctrlHeadlabel6.SetBackColor(H11);
	m_ctrlHeadlabel7.SetCaption("10");
	m_ctrlHeadlabel7.SetBackColor(H10);
	m_ctrlHeadlabel8.SetCaption("9");
	m_ctrlHeadlabel8.SetBackColor(H9);
	m_ctrlHeadlabel9.SetCaption("8");
	m_ctrlHeadlabel9.SetBackColor(H8);
	m_ctrlHeadlabel10.SetCaption("7");
	m_ctrlHeadlabel10.SetBackColor(H7);
	m_ctrlHeadlabel11.SetCaption("6");
	m_ctrlHeadlabel11.SetBackColor(H6);
	m_ctrlHeadlabel12.SetCaption("5");
	m_ctrlHeadlabel12.SetBackColor(H5);
	m_ctrlHeadlabel13.SetCaption("4");
	m_ctrlHeadlabel13.SetBackColor(H4);
	m_ctrlHeadlabel14.SetCaption("3");
	m_ctrlHeadlabel14.SetBackColor(H3);
	m_ctrlHeadlabel15.SetCaption("2");
	m_ctrlHeadlabel15.SetBackColor(H2);
	m_ctrlHeadlabel16.SetCaption("1");
	m_ctrlHeadlabel16.SetBackColor(H1);
	//	
	if(PC_TYPE==TRUE)
	{
		SetTimer(0,500,NULL);
		SetTimer(1, 1000, NULL);   //Drop Count Display

		if(pView->m_nLanguage == 0)
		ThreadStage.strMsg1="시작하려면 Start버튼을 누르세요!";
		else if(pView->m_nLanguage == 1)
		ThreadStage.strMsg1="Push START Button, If you want to start!";
		else if(pView->m_nLanguage == 2)
		ThreadStage.strMsg1="시작하려면 Start버튼을 누르세요!";
	}

//2013.08.03 by tskim
	 g_bMMGLineDropMode = pDoc->m_structDataEditor.m_bMMGLineDropMode;
//

//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CManuPatDrop::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case 0:	
		SubTimerManuPatDropFunc();
		break;
	case 1:
		SubTimerManuDropCountDisp();
		break;
	}
	CDialog::OnTimer(nIDEvent);
}

void CManuPatDrop::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CPen pen;
	pen.CreatePen(PS_SOLID,5,DARKBLUE);
	CPen *pOldPen = (CPen *)dc.SelectObject(&pen);

	dc.MoveTo(0,HEIGHT_YPOS);
	dc.LineTo(SCREEN_WIDTH,HEIGHT_YPOS);

//	dc.MoveTo(0,SCREEN_HEIGHT - HEIGHT_YPOS);
//	dc.LineTo(SCREEN_WIDTH,SCREEN_HEIGHT - HEIGHT_YPOS);

	dc.SelectObject(pOldPen);	
//	
	// Do not call CDialog::OnPaint() for painting messages
}

void CManuPatDrop::OnSelchangeComboPatternNo() 
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
/*	CString str;

	for(int PattenNum=0;PattenNum<pDoc->m_structDataEditor.m_nPatternNum;PattenNum++)
		if(pDoc->m_structPatternData[PattenNum].m_nSequence == (m_ctrlComboPattern.GetCurSel()+1)) break;

	if(PattenNum == pDoc->m_structDataEditor.m_nPatternNum)
	{
		Selected_sequence_count = 0;
		str = "--";
		m_ctrlTableSpeed.SetCaption(str);
		str = "無";
		m_ctrlPattenNo.SetCaption(str);
	}
	else
	{
		Selected_sequence_count = pDoc->m_structPatternData[PattenNum].m_nSequence;
		str.Format("%d",pDoc->m_structPatternData[PattenNum].m_nTableSpeed);
		m_ctrlTableSpeed.SetCaption(str);
		str.Format("%d",(PattenNum+1));
		m_ctrlPattenNo.SetCaption(str);
	}
*/
	m_nCurrentPatternNo = m_ctrlComboPattern.GetCurSel()+1;

	// 현재선택된 pattern의 sequence를 찾아서 변수에 대입한다..
	m_nSelectedSequence = pDoc->m_structPatternData[m_ctrlComboPattern.GetCurSel()].m_nSequence;

	// control을 설정하고, pattern을 도시한다..
	SubSetControl(m_ctrlComboPattern.GetCurSel());
	SubDrawPattern();	
}

void CManuPatDrop::OnOK() 
{
	// TODO: Add extra validation here
	
//	CDialog::OnOK();
}

void CManuPatDrop::OnCancel() 
{
	// TODO: Add extra cleanup here
	
//	CDialog::OnCancel();
}

BEGIN_EVENTSINK_MAP(CManuPatDrop, CDialog)
    //{{AFX_EVENTSINK_MAP(CManuPatDrop)
	ON_EVENT(CManuPatDrop, IDC_CMD_SEE_ALL_PATTERN, -600 /* Click */, OnClickCmdSeeAllPattern, VTS_NONE)
	ON_EVENT(CManuPatDrop, IDC_LABEL_ADEL, -600 /* Click */, OnClickLabelAdel, VTS_NONE)
	ON_EVENT(CManuPatDrop, IDC_MSFLEXGRID_CIM_ADEL, -600 /* Click */, OnClickMsflexgridCimAdel, VTS_NONE)
	ON_EVENT(CManuPatDrop, IDC_CMD_RETURN, -600 /* Click */, OnClickCmdReturn, VTS_NONE)
	ON_EVENT(CManuPatDrop, IDC_DROP_COUNT_RESET, -600 /* Click */, OnClickDropCountReset, VTS_NONE)
	ON_EVENT(CManuPatDrop, IDC_REPAIR_EDIT, -600 /* Click */, OnClickRepairEdit, VTS_NONE)
	ON_EVENT(CManuPatDrop, IDC_GLS_STATUS_CLEAR, -600 /* Click */, OnClickGlsStatusClear, VTS_NONE)
	ON_EVENT(CManuPatDrop, IDC_GLS_STATUS_SET, -600 /* Click */, OnClickGlsStatusSet, VTS_NONE)
	ON_EVENT(CManuPatDrop, IDC_MSFLEXGRID_PANEL_INFORM_SET, -600 /* Click */, OnClickMsflexgridPanelInformSet, VTS_NONE)
	ON_EVENT(CManuPatDrop, IDC_LABEL_PAN_INFORM, -600 /* Click */, OnClickLabelPanInform, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CManuPatDrop::OnClickCmdSeeAllPattern() 
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	if(m_bAllPattern)
	{
		m_ctrlComboPattern.EnableWindow(TRUE);
		m_ctrlSeeAllPattern.SetBackColor(WHITEGRAY);
		m_bAllPattern = FALSE;
		m_ctrlHead1.SetEnabled(TRUE);
		m_ctrlHead2.SetEnabled(TRUE);
		m_ctrlHead3.SetEnabled(TRUE);
		m_ctrlHead4.SetEnabled(TRUE);
		m_ctrlHead5.SetEnabled(TRUE);
		m_ctrlHead6.SetEnabled(TRUE);
		m_ctrlHead7.SetEnabled(TRUE);
		m_ctrlHead8.SetEnabled(TRUE);
		m_ctrlHead9.SetEnabled(TRUE);
		m_ctrlHead10.SetEnabled(TRUE);
		m_ctrlHead11.SetEnabled(TRUE);
		m_ctrlHead12.SetEnabled(TRUE);
		m_ctrlHead13.SetEnabled(TRUE);
		m_ctrlHead14.SetEnabled(TRUE);
		m_ctrlHead15.SetEnabled(TRUE);
		m_ctrlHead16.SetEnabled(TRUE);

		m_ctrlRepairEdit.SetEnabled(TRUE);
		SubSetControl(m_ctrlComboPattern.GetCurSel());
	}
	else
	{
		m_ctrlComboPattern.EnableWindow(FALSE);
		m_ctrlSeeAllPattern.SetBackColor(LIGHTGREEN);
		m_bAllPattern = TRUE;
		m_ctrlHead1.SetEnabled(FALSE);
		m_ctrlHead2.SetEnabled(FALSE);
		m_ctrlHead3.SetEnabled(FALSE);
		m_ctrlHead4.SetEnabled(FALSE);
		m_ctrlHead5.SetEnabled(FALSE);
		m_ctrlHead6.SetEnabled(FALSE);
		m_ctrlHead7.SetEnabled(FALSE);
		m_ctrlHead8.SetEnabled(FALSE);
		m_ctrlHead9.SetEnabled(FALSE);
		m_ctrlHead10.SetEnabled(FALSE);
		m_ctrlHead11.SetEnabled(FALSE);
		m_ctrlHead12.SetEnabled(FALSE);
		m_ctrlHead13.SetEnabled(FALSE);
		m_ctrlHead14.SetEnabled(FALSE);
		m_ctrlHead15.SetEnabled(FALSE);
		m_ctrlHead16.SetEnabled(FALSE);

		m_ctrlRepairEdit.SetEnabled(FALSE);
	}

	m_nCurrentPatternNo = m_ctrlComboPattern.GetCurSel()+1;
	m_nSelectedSequence = pDoc->m_structPatternData[m_ctrlComboPattern.GetCurSel()].m_nSequence;

	SubDrawPattern();	

}

void CManuPatDrop::OnClickLabelAdel() 
{
	CString str = "C00";
//	str = m_ctrlSetAdel.GetCaption();
	CAddDelInput dlg(str, CPoint(0, 0));
//	dlg.m_nHide = TRUE;

	if(dlg.DoModal() == IDOK)
	{
		str = dlg.m_strNewData;
		m_ctrlSetAdel.SetCaption(str);
	}
	
}

void CManuPatDrop::OnBtnChange() 
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	CString str = "";
	CString temp_AddDel;
	int value=0;
	temp_AddDel = m_ctrlSetAdel.GetCaption();
//	strcpy(temp_AddDel,m_ctrlSetAdel.GetCaption());
	for( int i = 0 ; i < 30 ; i++)
	{
		for( int j = 0 ; j < 10 ; j++)
		{
			// 변수갱신
			if((i*10+j)<CIM_Information.Panal_Max_Num)
			{
				str.Format("%s",temp_AddDel);    
				m_ctrlCimAdel.SetTextMatrix(4*i+1,j,str);

				value = pDoc->StrtoValue(temp_AddDel);
				CIM_Information.Add_Del_Number[10*i+j]=value;

			}			
		}
	}

	pDoc->Match_Worked_Report_for_Level();
		
	// 이부분에 drop수 갱신 부분을 넣는당.
	for( i = 0 ; i < pDoc->m_structDataEditor.m_nPatternNum ; i++ )
		pDoc->SetValidDropPoint(i);

	SubDrawPattern();	
}

void CManuPatDrop::OnClickMsflexgridCimAdel() 
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	int RowCol = 0;
	//RowCol = 8*((m_ctrlCimAdel.GetRow()-1)/3) + m_ctrlCimAdel.GetCol()+1;
	RowCol = 10*((m_ctrlCimAdel.GetRow())/4) + m_ctrlCimAdel.GetCol()+1;

	if(RowCol>CIM_Information.Panal_Max_Num)	return;
	if(((m_ctrlCimAdel.GetRow()%4)!=1) && ((m_ctrlCimAdel.GetRow()%4)!=2) ) return;


	if((m_ctrlCimAdel.GetRow()%4)==1)
	{
	
		CString str = "";
	//	str = m_ctrlCimAdel.GetTextMatrix(m_ctrlCimAdel.GetRow(), m_ctrlCimAdel.GetCol());
		str = "C00";
		CAddDelInput dlg(str, CPoint(0, 0));

		if(dlg.DoModal() == IDOK)
		{
			str = dlg.m_strNewData;
			m_ctrlCimAdel.SetTextMatrix(m_ctrlCimAdel.GetRow(), m_ctrlCimAdel.GetCol() ,str);
		}

		CIM_Information.Add_Del_Number[10*(int)(m_ctrlCimAdel.GetRow()/4) + m_ctrlCimAdel.GetCol()] = pDoc->StrtoValue(str);

		pDoc->Match_Worked_Report_for_Level();	
		
		// 이부분에 drop수 갱신 부분을 넣는당.
		for( int i = 0 ; i < pDoc->m_structDataEditor.m_nPatternNum ; i++ )
			pDoc->SetValidDropPoint(i);

		//
		SubDrawPattern();	
	}
	else if((m_ctrlCimAdel.GetRow()%4)==2)
	{
		if(pDoc->m_structDataEditor.m_bNgDropping == TRUE)	return;
		CString str="", str1="",str2="",str12="";
		char strTFT, strCF;
		int nRowIndex=0, nColIndex=0, nIndex=0;

		nRowIndex=m_ctrlCimAdel.GetRow()/4;	
		nColIndex=m_ctrlCimAdel.GetCol();
		nIndex=10*nRowIndex+nColIndex;

		str = m_ctrlCimAdel.GetTextMatrix(m_ctrlCimAdel.GetRow(), m_ctrlCimAdel.GetCol());
		strTFT=str.GetAt(0);	strCF=str.GetAt(1);
	
		CPanelInformInput dlg(nIndex, strTFT, strCF);
		if(dlg.DoModal()== IDOK)
		{
			for(int i = 0 ; i < 30 ; i++)
			{
				for(int j = 0 ; j < 10 ; j++)
				{
					if((10*i+j)<CIM_Information.Panal_Max_Num)
					{
						str1.Format(PanelInformM(CIM_Information.Panel_NG_Mark[TFT][10*i+j]));
						str2.Format(PanelInformM(CIM_Information.Panel_NG_Mark[CF][10*i+j]));
						str12=str1+str2;
						m_ctrlCimAdel.SetTextMatrix(4*i+2,j,str12);
					}
				}
			}
		}
	}
}

void CManuPatDrop::OnClickCmdReturn() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	if(PC_TYPE==TRUE)
	{
		KillTimer(0);
		KillTimer(1);
		pView->m_pDevice->Stop_button_onoff(false);
		pView->m_pDevice->Start_button_onoff(false);
	}

	ReturnRecipeData();   // 원래의 Pattern Data를 복원 한다.
	SavePanelMarkSet();	//Panel NG Mark에 대한 정보를 저장한다.

	EndDialog(IDOK);
	
}
//////////////////////////////////
void CManuPatDrop::SavePanelMarkSet() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	CString strPathName;

	strPathName.Format("%s\\%s\\PanelNG_MarkSet.dat", pDoc->m_strDataPath, pDoc->m_structOperatorModeRecipeData.strFrontRecipeName);

	CFile SaveFile; 
	if( SaveFile.Open((char *)(LPCSTR)strPathName, CFile::modeCreate | CFile::modeWrite ) )
	{
		pDoc->m_nSelFile = 5;
		CArchive ar( &SaveFile, CArchive::store );
		pDoc->Serialize(ar);
		ar.Close();
		SaveFile.Close();
	}				
}
//////////////////////////////////
void CManuPatDrop::SubDrawPattern()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();


	// 큰 스크린의 위치와 크기를 알아내서 생성
	if(m_bStart == TRUE)
	{
		CRect rectBig;
		m_staticScreen.GetWindowRect(rectBig);
		GetParent()->ScreenToClient(rectBig);

		if(m_pPatternWnd != NULL) 
		{
			delete m_pPatternWnd;
			m_pPatternWnd = NULL;
		}
		m_pPatternWnd = new CPatternWnd();
		m_pPatternWnd->CreateEx(WS_EX_WINDOWEDGE, NULL, NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, rectBig, this, ID_PATTERN_WND);
		m_pPatternWnd->m_bIsDisplayText = FALSE;
		m_pPatternWnd->m_bRepairEditMode = FALSE;
		m_pPatternWnd->m_bTextDisp = FALSE;
		m_bStart = FALSE;
		m_pPatternWnd->m_nPosOper=1;

	}

	
	if(m_bAllPattern == FALSE) 
	{
		for(int i = 0 ; i < MAX_PATTERN; i++)
		{
			if(pDoc->m_nSeq[0][i] == m_nCurrentPatternNo )
			{
				m_pPatternWnd->m_nPatternNum = m_nCurrentPatternNo;
				break;
			}
			else m_pPatternWnd->m_nPatternNum = -1;
		}		
	}
	else 
	{
		for(int i = 0; i < MAX_PATTERN; i++)
		{
			m_pPatternWnd->m_nSeq[i] = pDoc->m_nSeq[0][i];
		}
		m_pPatternWnd->m_nPatternNum = -2;		// -2는 모든 패턴을 다 그리겠다는 뜻.
	}

	m_pPatternWnd->m_bIsBig = TRUE;
	m_pPatternWnd->m_bIsDirectSetting = FALSE;
	m_pPatternWnd->SetDefaultZoom();
	
	// 선택된 Pattern에 대해서 다시 그린다..
	if(m_bStart == FALSE) m_pPatternWnd->Invalidate();

}

void CManuPatDrop::SubSetControl(int nPatternNum) 
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	UINT i = 0;
	CString str = "";

	// head on, off // text 설정 부분..
	for(i = 0 ; i < MAX_NOZZLE ; i++)
	{
		str.Format("Head%d OFF",i+1);
		GetDlgItem(IDC_CMD_HEAD1 + i)->SetWindowText(str);
	}

	for(i = 0 ; i < MAX_NOZZLE ; i++)
	{
		if(pDoc->m_structPatternData[nPatternNum].m_bIsOnHead[i] == TRUE) 
		{
			str.Format("Head%d ON",i+1);
			GetDlgItem(IDC_CMD_HEAD1 + i)->SetWindowText(str);
		}
	}

	// head on, off // color 설정 부분..


	if(pDoc->m_structPatternData[nPatternNum].m_bIsOnHead[0]) 
		m_ctrlHead1.SetBackColor(GREEN);
	else 
		m_ctrlHead1.SetBackColor(WHITEGRAY);

	if(pDoc->m_structPatternData[nPatternNum].m_bIsOnHead[1]) 
		m_ctrlHead2.SetBackColor(GREEN);
	else 
		m_ctrlHead2.SetBackColor(WHITEGRAY);

	if(pDoc->m_structPatternData[nPatternNum].m_bIsOnHead[2]) 
		m_ctrlHead3.SetBackColor(GREEN);
	else 
		m_ctrlHead3.SetBackColor(WHITEGRAY);

	if(pDoc->m_structPatternData[nPatternNum].m_bIsOnHead[3]) 
		m_ctrlHead4.SetBackColor(GREEN);
	else 
		m_ctrlHead4.SetBackColor(WHITEGRAY);

	if(pDoc->m_structPatternData[nPatternNum].m_bIsOnHead[4]) 
		m_ctrlHead5.SetBackColor(GREEN);
	else 
		m_ctrlHead5.SetBackColor(WHITEGRAY);

	if(pDoc->m_structPatternData[nPatternNum].m_bIsOnHead[5]) 
		m_ctrlHead6.SetBackColor(GREEN);
	else 
		m_ctrlHead6.SetBackColor(WHITEGRAY);

	if(pDoc->m_structPatternData[nPatternNum].m_bIsOnHead[6]) 
		m_ctrlHead7.SetBackColor(GREEN);
	else 
		m_ctrlHead7.SetBackColor(WHITEGRAY);

	if(pDoc->m_structPatternData[nPatternNum].m_bIsOnHead[7]) 
		m_ctrlHead8.SetBackColor(GREEN);
	else 
		m_ctrlHead8.SetBackColor(WHITEGRAY);

	if(pDoc->m_structPatternData[nPatternNum].m_bIsOnHead[8]) 
		m_ctrlHead9.SetBackColor(GREEN);
	else 
		m_ctrlHead9.SetBackColor(WHITEGRAY);

	if(pDoc->m_structPatternData[nPatternNum].m_bIsOnHead[9]) 
		m_ctrlHead10.SetBackColor(GREEN);
	else 
		m_ctrlHead10.SetBackColor(WHITEGRAY);

	if(pDoc->m_structPatternData[nPatternNum].m_bIsOnHead[10]) 
		m_ctrlHead11.SetBackColor(GREEN);
	else 
		m_ctrlHead11.SetBackColor(WHITEGRAY);

	if(pDoc->m_structPatternData[nPatternNum].m_bIsOnHead[11]) 
		m_ctrlHead12.SetBackColor(GREEN);
	else 
		m_ctrlHead12.SetBackColor(WHITEGRAY);
	if(pDoc->m_structPatternData[nPatternNum].m_bIsOnHead[12]) 
		m_ctrlHead13.SetBackColor(GREEN);
	else 
		m_ctrlHead13.SetBackColor(WHITEGRAY);

	if(pDoc->m_structPatternData[nPatternNum].m_bIsOnHead[13]) 
		m_ctrlHead14.SetBackColor(GREEN);
	else 
		m_ctrlHead14.SetBackColor(WHITEGRAY);

	if(pDoc->m_structPatternData[nPatternNum].m_bIsOnHead[14]) 
		m_ctrlHead15.SetBackColor(GREEN);
	else 
		m_ctrlHead15.SetBackColor(WHITEGRAY);

	if(pDoc->m_structPatternData[nPatternNum].m_bIsOnHead[15]) 
		m_ctrlHead16.SetBackColor(GREEN);
	else 
		m_ctrlHead16.SetBackColor(WHITEGRAY);

	UpdateData(FALSE);

	// Pattern Drawing을 갱신한다..
	m_pPatternWnd->Invalidate();
}

void CManuPatDrop::SubTimerManuPatDropFunc()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	DWORD dwIOResultF=0;
	BOOL bIOResult=FALSE, bMiniDoorSafetyOK=FALSE;
//
	m_nTimerCount++;
	m_ctrlMessageManuPatDrop.SetCaption( ThreadStage.strMsg1 );

//  lbg잠시
#if TEST
	if(pView->m_pDevice->Stage_glass_check()/*&&(pView->m_pDevice->MNET_Read_A_Bit(DISP_BIT,12) == TRUE)*/)
	{
		if(ThreadStage.nDropStatus==0)		// Drop을 하지 않은 Glass가 올라와 있을 때...
		{
			m_ctrlDropClear.EnableWindow(false);
			m_ctrlDropCompleted.EnableWindow(true);
		}
		else								// Drop을 완료 하였거나 하다가 완료되지 못한 Glass가 올라와 있을 때...
		{
			m_ctrlDropClear.EnableWindow(true);
			m_ctrlDropCompleted.EnableWindow(false);
		}
	}
	else	
	{
		m_ctrlDropClear.EnableWindow(false);
		m_ctrlDropCompleted.EnableWindow(false);
	}
#else
	if(ThreadStage.nDropStatus==0)		// Drop을 하지 않은 Glass가 올라와 있을 때...
	{
		m_ctrlDropClear.EnableWindow(true);
		m_ctrlDropCompleted.EnableWindow(true);
	}
	else								// Drop을 완료 하였거나 하다가 완료되지 못한 Glass가 올라와 있을 때...
	{
		m_ctrlDropClear.EnableWindow(true);
		m_ctrlDropCompleted.EnableWindow(true);
	}
#endif
/////////////////////////////////////////////
	if(ThreadStage.bMachineRunning == TRUE)
	{
		FAS_GetIo(1,false,&dwIOResultF);
		FAS_GetIoBit(1,false,ROBOT_ARM_DETECT,&bIOResult);	// Robot Arm Check

		if(bIOResult==TRUE)
		{
			ThreadStage.bEmergencyStop=TRUE; ThreadStage.ExitFlag = TRUE; ThreadStage.nHomeEnableCount = 0;
			pView->m_pDevice->Emergency_Stop();
			pView->SendMessage(WM_ERROR,104,NULL);
			pView->m_pDevice->SyncSetIO(SYNC_AXIS_S,0.0, false, false); // off
			FAS_ResetDropQueue(1);
			FAS_ResetSpdQueue(2); //2번 보드에 들어 있는 P축 Clear
		}

		if(pView->m_pDevice->SST_Check(STOP_SWITCH)) 
		{

			//axis stop..
			FAS_MoveStop(1,0,0);	//S0
			FAS_MoveStop(1,1,0);	//K0
			FAS_MoveStop(2,0,0);	//K1
			FAS_MoveStop(2,1,0);	//K2

			for(int iaxis = 4; iaxis < 16; iaxis++)
			{
				FAS_MoveStop(2,iaxis,0);	//S1~S12				
			}

			for(iaxis = 0; iaxis < 4; iaxis++)
			{
				FAS_MoveStop(3,iaxis,0);	//S13~S16				
			}
			//
			ThreadStage.ExitFlag = true; // ** exit **
		}
		// 현재 Run 상태에서 Start 버튼을 On 시켜 놓는다.
		pView->m_pDevice->Start_button_onoff(true);
		pView->m_pDevice->Stop_button_onoff(false);
		m_nTimerCount = 0;
		//
		m_ctrlReturn.EnableWindow(false);
	}
	else // ThreadStage.bMachineRunning == FALSE
	{
		//
//2008.01.10 By tskim
//Manual Drop시 Down현상 관련 Log Add...
		CString strLog = " ";
		if(pView->m_pDevice->SST_Check(START_SWITCH)) 
		{
						m_bSWCheck = false;//2010.01.19
			if(m_bAllPattern == TRUE) { 
				global_Sequence_Count=0; 
				g_nDrop_Repeat_Count=0;
				ThreadStage.JobModeSelection = 3;
//2008.01.10 by tskim
//Log Add....
				strLog.Format("<Manual Drop> All Pattern Drop Start");
				pView->SaveLog(0, strLog);
			}
			else 
			{
				global_Sequence_Count=(m_nSelectedSequence-1);
				ThreadStage.JobModeSelection = 2;
//2008.01.10 By tskim
//Log Add..
				strLog.Format("<Manual Drop> Part Pattern Drop Start");
				pView->SaveLog(0, strLog);
			}
// #if TEST//나중에 지울것...2010.02.09 by tskim
// 	m_bStepNotSelError = TRUE;
// 	g_bPumpOringFlag = FALSE;
// #else
// 			g_bPumpOringFlag = FALSE;
// #endif
//2013.09.05 by tskim
			g_bPumpOringFlag = false;
			if(g_bPumpOringFlag == TRUE)
			{
				CNormalMsg dlg;
				if(m_bStepNotSelError == FALSE)
				{
					dlg.m_bTimer=FALSE;

					if(pView->m_nLanguage == 0)
					{
						dlg.m_strTitle = _T("확인");
						dlg.m_strMsg1 = " 'P축 원점 동작 후 측정값을 확인 하지 않았습니다. ";
						dlg.m_strMsg2 = " 측정 동작 실행 후 측정값 이상 없을 경우 다시 시작하세요...";		
					}

					else if(pView->m_nLanguage == 1)
					{
						dlg.m_strTitle = _T("Check");
						dlg.m_strMsg1 = " 'Machine Don't Check Calibration Value After P Axis Origin Point Action. ";
						dlg.m_strMsg2 = " if Machine state is good, Please restart After Calibration Action Run ...";		
					} 
					
					else if(pView->m_nLanguage == 2)
					{
						dlg.m_strTitle = _T("確認");
						dlg.m_strMsg1 = " 'P축 원점 동작 후 측정값을 확인 하지 않았습니다. ";
						dlg.m_strMsg2 = " 實行測量動作后測量?無異常下再開始....";		
					}

					m_bStepNotSelError = TRUE;
					dlg.DoModal();
					m_bStepNotSelError = FALSE;
				}
			}
			else
			{
				ThreadStage.JobStep=19;
				ThreadStage.JobFlag = STAGE_AUTO;
				pView->RunThread(THREAD_STAGE);
			}
		}
		// Stop 상태에서 
		// Stop 버튼이 On되도록 한다.
		pView->m_pDevice->Stop_button_onoff(true);
		// Start 버튼은 깜박거리게 한다.
		if(m_nTimerCount==2) pView->m_pDevice->Start_button_onoff(true);
		else if(m_nTimerCount>=4) 
		{
			pView->m_pDevice->Start_button_onoff(false);
			m_nTimerCount = 0;
		}
		m_ctrlReturn.EnableWindow(true);
	}
	//////////////////////////////////////////////
}


void CManuPatDrop::SubTimerManuDropCountDisp()
{
	if(ThreadStage.bDropCountDisp==TRUE)
	{
		SubDrawPattern();
		ThreadStage.bDropCountDisp=FALSE;
	}
}

void CManuPatDrop::OnClickDropCountReset() 
{
	// TODO: Add your control notification handler code here
	if(FASC_ResetAllCount(5)!=FMM_OK){
		AfxMessageBox("Couneter Board 초기화가 되지 않음.");
	}
}

void CManuPatDrop::OnClickRepairEdit() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	if(PC_TYPE==TRUE)
	{
		KillTimer(0);
		KillTimer(1);
		pView->m_pDevice->Stop_button_onoff(false);
		pView->m_pDevice->Start_button_onoff(false);
	}
////////////////////////////////////////////////////////////////
	CRepairEdit dlg;
	dlg.m_nCurrentPatternNo = m_nCurrentPatternNo;

	dlg.m_structPatternData_Repair[m_nCurrentPatternNo-1] = m_structPatternData[m_nCurrentPatternNo-1]; 
	dlg.DoModal();
////////////////////////////////////////////////////////////////	
/*
	for(int k=0; k<MAX_PATTERN; k++){
		pDoc->m_arDropData[k].RemoveAll();
	}
*/
	if(PC_TYPE==TRUE)
	{
		SetTimer(0,500,NULL);
		SetTimer(1, 1000, NULL);   //Drop Count Display

		if(pView->m_nLanguage == 0)
		ThreadStage.strMsg1="시작하려면 Start버튼을 누르세요!";
		else if(pView->m_nLanguage == 1)
		ThreadStage.strMsg1="Push START Button, If you want to start!";
		else if(pView->m_nLanguage == 2)
		ThreadStage.strMsg1="시작하려면 Start버튼을 누르세요!";
	}
	m_pPatternWnd->m_bTextDisp = FALSE;
	m_pPatternWnd->m_nPosOper=1;
	SubSetControl(m_nCurrentPatternNo-1);
	SubDrawPattern();
}

void CManuPatDrop::PatDataTmpSave()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	
	DROP_DATA stDropData;

	for(int PatNum = 0; PatNum < pDoc->m_structDataEditor.m_nPatternNum; PatNum++)
	{
		m_structPatternData[PatNum] = pDoc->m_structPatternData[PatNum]; 
		for(int DropNum=0; DropNum < pDoc->m_structPatternData[PatNum].m_nDropDataCount; DropNum++){
			stDropData = pDoc->m_arDropData[PatNum].GetAt(DropNum);
			//m_structDropData[PatNum].SetAt(DropNum, stDropData);
			m_structDropData[PatNum].Add(stDropData);
			
		}
	}
}

void CManuPatDrop::ReturnRecipeData()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	DROP_DATA stDropData;

	for(int i = 0; i < pDoc->m_structDataEditor.m_nPatternNum; i++){
		pDoc->m_structPatternData[i] = m_structPatternData[i];
		for(int j=0; j <pDoc->m_structPatternData[i].m_nDropDataCount; j++){
			stDropData = m_structDropData[i].GetAt(j);
			pDoc->m_arDropData[i].SetAt(j,stDropData);
		}
	}
	delete [] m_structDropData;
}

void CManuPatDrop::OnClickGlsStatusClear() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument(); 

	CString strLog="";
	ThreadStage.nDropStatus=0;//2013.09.05 by tskim
#if EQ
	if(pView->m_pDevice->Stage_glass_check()&&(pView->m_pDevice->MNET_Read_A_Bit(DISP_BIT,12) == TRUE))
	{
		strLog.Format("임의로 Glass Drop 초기 상태로 함.");
		pView->SaveLog(1, strLog);
		ThreadStage.nDropStatus=0;
		
		//DROP STATUS FILE SAVE
		pDoc->SaveDropStatus(); 
	}
#else
	strLog.Format("임의로 Glass Drop 초기 상태로 함.");
	pView->SaveLog(1, strLog);
	ThreadStage.nDropStatus=0;

	//DROP STATUS FILE SAVE
	pDoc->SaveDropStatus(); 
#endif
}

void CManuPatDrop::OnClickGlsStatusSet() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument(); 

	CString strLog="";
	ThreadStage.nDropStatus=2;//2013.09.05 by tskim

	//DROP STATUS FILE SAVE
	pDoc->SaveDropStatus(); 
#if EQ
	if(pView->m_pDevice->Stage_glass_check()&&(pView->m_pDevice->MNET_Read_A_Bit(DISP_BIT,12) == TRUE))
	{
		strLog.Format("임의로 Glass Drop 완료 상태로 함.");
		pView->SaveLog(1, strLog);
		ThreadStage.nDropStatus=2;
		
		//DROP STATUS FILE SAVE
		pDoc->SaveDropStatus(); 
	}
#else
	strLog.Format("임의로 Glass Drop 완료 상태로 함.");
	pView->SaveLog(1, strLog);
	ThreadStage.nDropStatus=2;	

	//DROP STATUS FILE SAVE
	pDoc->SaveDropStatus(); 
#endif
}

void CManuPatDrop::OnClickMsflexgridPanelInformSet() 
{
	// TODO: Add your control notification handler code here
///////////////////////////////////////

	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	if((pDoc->m_structDataEditor.m_bNgDropping == TRUE)||(pDoc->m_bIsEngineerMode == FALSE))	return;



	CString strTFT="",strCF = "",str="";
	int Row=0, Col=0;

	
	Row = m_ctrlPanelMatrixSet.GetRow();
	Col = m_ctrlPanelMatrixSet.GetCol();

	if(Row == 0 || Col == 0) return;

	switch(Row)
	{
	case 1:	strCF.Format(_T("G")); break;
	case 2:	strCF.Format(_T("Q")); break;
	case 3:	strCF.Format(_T("N")); break;
	case 4: strCF.Format(_T("W")); break;
	case 5: strCF.Format(_T("X")); break;
	}

	switch(Col)
	{
	case 1: strTFT.Format(_T("G")); break;
	case 2: strTFT.Format(_T("Q")); break;
	case 3: strTFT.Format(_T("N")); break;
	case 4:	strTFT.Format(_T("W")); break;
	case 5:	strTFT.Format(_T("Y")); break;
	}
	
	CPanelNGSet dlg(strTFT, strCF, CIM_Information.Panel_NG_MarkSet[(Row-1)*5+(Col-1)]);

	if(dlg.DoModal() == IDOK)	// 갱신 후 Data를 다시 뿌려 준다.
	{
		for(int i = 0 ; i < 25 ; i++)
		{
			m_ctrlPanelMatrixSet.SetRow((i/5)+1);
			m_ctrlPanelMatrixSet.SetCol((i%5)+1);
			m_ctrlPanelMatrixSet.SetCellAlignment(4);
			if(CIM_Information.Panel_NG_MarkSet[i]==0)
			{
				m_ctrlPanelMatrixSet.SetCellBackColor(WHITE);
				str="OFF";
			}
			else
			{
				m_ctrlPanelMatrixSet.SetCellBackColor(GREEN);
				str="ON";
			}
			m_ctrlPanelMatrixSet.SetTextMatrix((i/5)+1,(i%5)+1,str);
		}		
	}
}

CString CManuPatDrop::PanelInformM(int Panel_Info)
{
	CString strReturn="";

	switch(Panel_Info)
	{
	case 0:		strReturn=" ";	break;
	case 1:		strReturn="G";	break;
	case 2:		strReturn="N";	break;
	case 3:		strReturn="P";	break;
	case 4:		strReturn="M";	break;
	case 5:		strReturn="R";	break;
	case 6:		strReturn="H";	break;
	case 7:		strReturn="I";	break;	// 
	case 8:		strReturn="A";	break;
	case 9:		strReturn="Q";	break;
	case 10:	strReturn="E";	break;	//
	case 11:	strReturn="C";	break;	//
	case 12:	strReturn="B";	break;	// 
	case 13:	strReturn="F";	break;	// 
	case 14:	strReturn="D";	break;	//
	case 15:	strReturn="L";	break;	//
	case 16:	strReturn=" ";	break;	//
	case 17:	strReturn="W";	break;	//
	case 18:	strReturn="U";	break;	// 
	case 19:	strReturn="T";	break;	// 
	case 20:	strReturn="V";	break;	//
	case 21:	strReturn="K";	break;	//
	case 22:	strReturn="S";	break;	//
	case 23:	strReturn="X";	break;	
	case 24:	strReturn="Y";	break;
	case 25:	strReturn=" ";	break;
	case 26:	strReturn=" ";	break;	
	case 27:	strReturn=" ";	break;	
	case 28:	strReturn=" ";	break;	
	case 29:	strReturn=" ";	break;	
	case 30:	strReturn=" ";	break;	
	case 31:	strReturn=" ";	break;	
	default:    strReturn=" ";	break;
	}
	return strReturn;
}

void CManuPatDrop::OnClickLabelPanInform() 
{
	// TODO: Add your control notification handler code here
	char strTFT, strCF;
	int nIndex=0;

	CIM_Information.Panel_Inform_AllManual="GG";
	strTFT=CIM_Information.Panel_Inform_AllManual.GetAt(0);	
	strCF=CIM_Information.Panel_Inform_AllManual.GetAt(1);
	nIndex=(-1);
	CPanelInformInput dlg(nIndex, strTFT, strCF);

	if(dlg.DoModal()== IDOK)
	{
		m_ctrlSetPanelInform.SetCaption(CIM_Information.Panel_Inform_AllManual);
	}

}

void CManuPatDrop::OnPanelInformAllChg() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	CString str="", strTFT="", strCF = "";
	CString temp_PanelInform;
	int value=0;
	temp_PanelInform = m_ctrlSetPanelInform.GetCaption();
	if(pDoc->m_structDataEditor.m_bNgDropping == TRUE)	return;
//	strcpy(temp_AddDel,m_ctrlSetAdel.GetCaption());
	for( int i = 0 ; i < 30 ; i++)
	{
		for( int j = 0 ; j < 10 ; j++)
		{
			// 변수갱신
			if((i*10+j)<CIM_Information.Panal_Max_Num)
			{
				str.Format("%s",temp_PanelInform);    
				m_ctrlCimAdel.SetTextMatrix(4*i+2,j,str);

				value = pDoc->StrtoValuePanelInform(temp_PanelInform, TFT);
				CIM_Information.Panel_NG_Mark[TFT][10*i+j]=value;

				value = pDoc->StrtoValuePanelInform(temp_PanelInform, CF);
				CIM_Information.Panel_NG_Mark[CF][10*i+j]=value;
			}			
		}
	}	
}




void CManuPatDrop::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	// TODO: Add your message handler code here and/or call default
	if(g_bRemoteControl)
		pView->pTempOp->DoModal();
	else
		AfxMessageBox("Remote Mode Off!");
	CDialog::OnRButtonDblClk(nFlags, point);
}

void CManuPatDrop::SelectLanguage()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(pView->m_nLanguage == 0)	//ehji 140309
	{
		SetDlgItemText(IDC_GLS_STATUS_CLEAR,				_T("Glass Drop  초기화 처리"));
		SetDlgItemText(IDC_GLS_STATUS_SET,				_T("Glass Drop 완료 처리"));
		SetDlgItemText(IDC_CMD_SEE_ALL_PATTERN,				_T("모든 PATTERN 보기"));
		SetDlgItemText(IDC_BTN_CHANGE,				_T("모두변경"));
		SetDlgItemText(IDC_ALL_CHG,				_T("모두변경"));
	}
	
	else if(pView->m_nLanguage == 1)	//ehji 140309
	{
		SetDlgItemText(IDC_GLS_STATUS_CLEAR,				_T("Glass Drop  Inital Manage"));
		SetDlgItemText(IDC_GLS_STATUS_SET,				_T("Glass Drop Finish Manage"));
		SetDlgItemText(IDC_CMD_SEE_ALL_PATTERN,				_T("ALL PATTERN VIEW"));
		SetDlgItemText(IDC_BTN_CHANGE,				_T("ALL CHANGE"));
		SetDlgItemText(IDC_ALL_CHG,				_T("ALL CHANGE"));
	}

	else if(pView->m_nLanguage == 2)	//ehji 140309
	{
		SetDlgItemText(IDC_GLS_STATUS_CLEAR,				_T("Glass Drop  初期化處理"));
		SetDlgItemText(IDC_GLS_STATUS_SET,				_T("Glass Drop 完了處理"));
		SetDlgItemText(IDC_CMD_SEE_ALL_PATTERN,				_T("看所有pattern"));
		SetDlgItemText(IDC_BTN_CHANGE,				_T("所有變更"));
		SetDlgItemText(IDC_ALL_CHG,				_T("所有變更"));
	}
}

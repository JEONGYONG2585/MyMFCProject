DC_BRUSH// Barcode.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"
#include "P8CA_LcDispDoc.h"

#include "Barcode.h"
#include "Clb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern short ECS_MIF_Report;
extern short ECS_MIF_DATA;
extern short Disp_MIF_Request;
extern short Disp_MIF_Report_confirm;
extern short Disp_MIF_Residual_Report;
extern short Disp_MIF_DataACK;
extern short Disp_MIF_Residual_HeadData;
extern short Disp_MIF_Residual_Data;
//2015.04.29 by tskim 원재료 전산화Add 
extern short ECS_MIF_Data_Remain;//LC 잔량 
extern short ECS_MIF_Data_HeadName;//Head Name ECS->Disp
extern short ECS_MIF_Data_DummyDrop;//DummyDrop  From ECS -> To Disp
extern short Disp_MIF_DummyDrop_Result;//DummyDrop여부 From Disp -> To ECS
extern short Disp_MIF_DummuDrop_Report;//DummyDrop여부 Report
/////////////////////////////////////////////////////////////////////////////
// CBarcode dialog


CBarcode::CBarcode(CWnd* pParent /*=NULL*/)
	: CDialog(CBarcode::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBarcode)
	m_bBarEnable = TRUE;
	m_strBarcode = _T("");
	m_bBarAllSelect = TRUE;
	nBarcodeStatus = 0;
	m_nBarcode = 0;
	//}}AFX_DATA_INIT
}


void CBarcode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBarcode)
	DDX_Control(pDX, IDC_EDIT1, m_ctrlEditType);
	DDX_Control(pDX, IDC_MSFLEXGRID_BARCODE, m_ctrlLcType);
	DDX_Check(pDX, IDC_BARCODE, m_bBarEnable);
	DDX_Text(pDX, IDC_EDIT1, m_strBarcode);
	DDX_Control(pDX, IDC_LABEL3, m_ctrlLcTypeName);
	DDX_Control(pDX, IDC_EXPLAIN, m_ctrlExplain);
	DDX_Check(pDX, IDC_BARCODE2, m_bBarAllSelect);
	DDX_Control(pDX, IDC_MSFLEXGRID_CANISTER_TYPE, m_ctrlCanisterLcType);
	DDX_Control(pDX, IDC_LABEL27, m_ctrlCanisterTypeName);
	DDX_Radio(pDX, IDC_RADIO_HEAD, m_nBarcode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBarcode, CDialog)
	//{{AFX_MSG_MAP(CBarcode)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RADIO_HEAD, OnRadioHead)
	ON_BN_CLICKED(IDC_RADIO_CANISTER, OnRadioHead)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBarcode message handlers

BOOL CBarcode::OnInitDialog() 
{
//	SetWindowPos( NULL,10,40,0,0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER );
	this->MoveWindow(0,1,1024,768,NULL);
	CDialog::OnInitDialog();
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();

	CString str = "";
	CString strTemp ="";
	CString strRecipeChangeBarcode = "0000000000--";
	int i = 0, j = 0, nR = 0, nC = 0;	


	GetDlgItem(IDC_BARCODE_ID)->SetWindowText("LGDCA#"+pDoc->m_structMachineInfo.m_strLineNo+"-"+pDoc->m_structMachineInfo.m_strMachineNo);


	SelectLanguage();

	m_ctrlLcType.SetRows(9);
	m_ctrlLcType.SetCols(4);

	for(i=0;i<=MAX_NOZZLE/2;i++)
	{
		m_ctrlLcType.SetRowHeight(i,800);
	}
	for(i=0;i<4;i++)
	{
		if(i%2 == 0)
		 m_ctrlLcType.SetColWidth(i,2250);
		else
		 m_ctrlLcType.SetColWidth(i,5000);
		m_ctrlLcType.SetColAlignment(i,4);
	}

	str.Format("HEAD NO.");
	m_ctrlLcType.SetTextMatrix(0, 0 ,str);
	str.Format("HEAD NO.");
	m_ctrlLcType.SetTextMatrix(0, 2 ,str);
	
//2015.04.29 by tskim 원재료 전산화Add LC잔량보고
	for(i = 0 ; i < (MAX_NOZZLE/2) ; i++)
	{
		//str.Format("H%d\nRemain: %.0fg\nDummyDrop: %d",i+1,pDoc->m_dOffset[0][i],pDoc->m_nRFDataDummyDrop[i]);
		str.Format("HEAD%d",i+1);	//201605`
		m_ctrlLcType.SetTextMatrix(i+1,0 ,str);
		//str.Format("H%d\nRemain: %.0fg\nDummyDrop: %d",(MAX_NOZZLE/2)+(i+1),pDoc->m_dOffset[0][(MAX_NOZZLE/2)+i],pDoc->m_nRFDataDummyDrop[(MAX_NOZZLE/2)+i]);
		str.Format("HEAD%d",i+9);
		m_ctrlLcType.SetTextMatrix(i+1,2 ,str);
	}

 	for(i = 0; i < (MAX_NOZZLE/2); i++)
 	{
		if(pView->m_nLanguage ==0)
 		str.Format(" 전체선택");
		else if(pView->m_nLanguage ==1)
 		str.Format(" All Select");
		else if(pView->m_nLanguage ==2)
 		str.Format(" 選擇全部");


 		m_ctrlLcType.SetTextMatrix(0, 1 ,str);
 		m_ctrlLcType.SetTextMatrix(0, 3 ,str);
 	}

	for(i = 0 ; i <MAX_NOZZLE/2; i++)
	{
		m_ctrlLcType.SetTextMatrix(i+1, 1 ,pDoc->m_strLCLotNo[i]+"-"+(LPCSTR)CIM_Information.sBarcodeData[i]);
		m_ctrlLcType.SetTextMatrix(i+1, 3 ,pDoc->m_strLCLotNo[i+(MAX_NOZZLE/2)]+"-"+(LPCSTR)CIM_Information.sBarcodeData[i+(MAX_NOZZLE/2)]);
	}

	for(i = 0 ; i <MAX_NOZZLE; i++)
	{
		pDoc->m_strRFDataLOTID[i] = pDoc->m_strLCLotNo[i];
		pDoc->m_strRFDataBarcodeID[i] = CIM_Information.sBarcodeData[i];
	}

	for(i=0 ; i<MAX_NOZZLE/2 ; i++)	
	{
		if(pDoc->m_bIsHeadSelected[i] == FALSE)
		{
			m_ctrlLcType.SetCol(0);
			m_ctrlLcType.SetRow(i+1);
			m_ctrlLcType.SetCellBackColor(LIGHTGRAY);
			m_ctrlLcType.SetCellForeColor(WHITE);

			//2016053
			m_ctrlLcType.SetCol(1);
			m_ctrlLcType.SetRow(i+1);
			m_ctrlLcType.SetCellBackColor(LIGHTGRAY);
			m_ctrlLcType.SetCellForeColor(WHITE);
		}
		if(pDoc->m_bIsHeadSelected[i+MAX_NOZZLE/2] == FALSE)
		{
			m_ctrlLcType.SetCol(2);
			m_ctrlLcType.SetRow(i+1);
			m_ctrlLcType.SetCellBackColor(LIGHTGRAY);
			m_ctrlLcType.SetCellForeColor(WHITE);

			//20160503
			m_ctrlLcType.SetCol(3);
			m_ctrlLcType.SetRow(i+1);
			m_ctrlLcType.SetCellBackColor(LIGHTGRAY);
			m_ctrlLcType.SetCellForeColor(WHITE);
		}
	}
	
	for(i = 0 ; i < 2 ; i++)
	{
		str.Format("     Canister%d",i+1);
		m_ctrlCanisterLcType.SetTextMatrix(0, i ,str);
	}

	for(i=0 ; i < 2; i++)
	{
		m_ctrlCanisterLcType.SetColWidth(i,4970);
	}
	for(i=0 ; i < 2 ; i++)		
		m_ctrlCanisterLcType.SetRowHeight(i,500);

	for(i = 0 ; i <m_ctrlCanisterLcType.GetCols(); i++)
	{
		m_ctrlCanisterLcType.SetColAlignment(i,3);	// 우측 정렬		

		m_ctrlCanisterLcType.SetTextMatrix(1, i ,pDoc->m_strLCLotNo_Canister[i]+"-"+(LPCSTR)CIM_Information.sCanisterTypeData[i]);
	}
	if(ThreadStage.Recipe_Change == TRUE) //20120903김관훈 
	{
		ThreadStage.Recipe_Change = FALSE;
		for(i=0; i<MAX_NOZZLE/2; i++)
		{
			m_ctrlLcType.SetTextMatrix(i+1, 1 ,strRecipeChangeBarcode);
			m_ctrlLcType.SetTextMatrix(i+1, 3 ,strRecipeChangeBarcode);
		}
		for(i = 0 ; i <m_ctrlCanisterLcType.GetCols(); i++)
		{
			m_ctrlCanisterLcType.SetTextMatrix(1, i ,strRecipeChangeBarcode);
		}
	}

	
	// TODO: Add extra initialization here
	for(i = 0; i < MAX_NOZZLE; i++) m_bSeleted_Head[i] = FALSE;
	m_bBarEnable =TRUE;
	m_bBarAllSelect = TRUE;
//2010.07.05 by tskim
	m_bSelected_Canister[0]=m_bSelected_Canister[1]=FALSE;

	UpdateData(false);
	SetTimer(0,500,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_EVENTSINK_MAP(CBarcode, CDialog)
    //{{AFX_EVENTSINK_MAP(CBarcode)
	ON_EVENT(CBarcode, IDC_OK, -600 /* Click */, OnClickOk, VTS_NONE)
	ON_EVENT(CBarcode, IDC_BARCODE_INPUT, -600 /* Click */, OnClickBarcodeInput, VTS_NONE)
	ON_EVENT(CBarcode, IDC_BARCODE_STOP, -600 /* Click */, OnClickBarcodeStop, VTS_NONE)
	ON_EVENT(CBarcode, IDC_MSFLEXGRID_BARCODE, -600 /* Click */, OnClickMsflexgridBarcode, VTS_NONE)
	ON_EVENT(CBarcode, IDC_SELECT_AUTO, -600 /* Click */, OnClickSelectAuto, VTS_NONE)
	ON_EVENT(CBarcode, IDC_MSFLEXGRID_BARCODE, -601 /* DblClick */, OnDblClickMsflexgridBarcode, VTS_NONE)
	ON_EVENT(CBarcode, IDC_EXAMINE, -600 /* Click */, OnClickExamine, VTS_NONE)
	ON_EVENT(CBarcode, IDC_MSFLEXGRID_CANISTER_TYPE, -600 /* Click */, OnClickMsflexgridCanisterType, VTS_NONE)
	ON_EVENT(CBarcode, IDC_LABEL_CANISTER_NAME, -600 /* Click */, OnClickLabelCanisterName, VTS_NONE)
	ON_EVENT(CBarcode, IDC_RF_INFORMATION, -600 /* Click */, OnClickRfInformation, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CBarcode::OnClickOk() 
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CString strTemp,str = "";

	for(int i = 0; i < MAX_NOZZLE; i++)
	{
		//2008.01.10 By tskim
		//BarCode Save시 쓰레기값 저장 관련 처리...
		if(pDoc->m_bIsHeadSelected[i] == TRUE)
		{
			if(i < MAX_NOZZLE/2) //1~8
			{
				strTemp = (char *)(LPCSTR)m_ctrlLcType.GetTextMatrix(i+1, 1);
			}
			else if((i >= MAX_NOZZLE/2)&&(i < MAX_NOZZLE))//8~16
			{
				strTemp = (char *)(LPCSTR)m_ctrlLcType.GetTextMatrix((i-MAX_NOZZLE/2)+1, 3);
			}
// 			else if((i >= (MAX_NOZZLE/4)*2)&&(i < (MAX_NOZZLE/4)*3))//12,11,10,9(1,2,3,4)
// 			{
// 				strTemp = (char *)(LPCSTR)m_ctrlLcType.GetTextMatrix(3, ((MAX_NOZZLE/4)*3)-i);
// 			}
// 			else //16,15,14,13(1,2,3,4)
// 			{
// 				strTemp = (char *)(LPCSTR)m_ctrlLcType.GetTextMatrix(4, ((MAX_NOZZLE/4)*4)-i);	
// 			}
			
			if(strTemp.GetLength() > 11)// by ckh 2009.02.25 글자 길이 검사 추가함.
			{
				pDoc->m_strLCLotNo[i] = strTemp.Mid(0,10);
				CIM_Information.sBarcodeData[i] = strTemp.Mid(11,7);	
			}
		}
		else
		{
			CIM_Information.sBarcodeData[i] = "-";
			str = "0000000000";
			pDoc->m_strLCLotNo[i] = (LPCSTR)str;
//			if(i < MAX_NOZZLE/2) CIM_Information.sBarcodeData[i] = "-";
//			else CIM_Information.sBarcodeData[i] = "-";			
		}
	}

	pDoc->SaveBarcodeData();
//2010.07.05 by tskim Canister Lot No 추가.
	for(i=0;i<2;i++)
	{
		strTemp.Empty();
		strTemp = (char *)(LPCSTR)m_ctrlCanisterLcType.GetTextMatrix(1,i);
		pDoc->m_strLCLotNo_Canister[i] = strTemp.Mid(0,10);
	}

	//by shin//2009.05.28//쓰레기값 들어가는거...??//
	for(i =0 ; i < 2; i++)
	{
		strTemp.Empty();
		strTemp=(char *)(LPCSTR)m_ctrlCanisterLcType.GetTextMatrix(1, i);
		if(strTemp.GetLength() == 18)
		{
			CIM_Information.sCanisterTypeData[i] = strTemp.Mid(11,7);
		}
		else
		{
			CIM_Information.sCanisterTypeData[i] = "-";
		}

	}

	pDoc->SaveCanisterData();

	KillTimer(0);

	EndDialog(IDOK);	
	// TODO: Add your control notification handler code here
	
}

HBRUSH CBarcode::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
// 	if(nCtlColor == CTLCOLOR_DLG)
// 	{
// 		return CreateSolidBrush(CYAN);
// 	}
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CBarcode::OnClickBarcodeInput() 
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

//	double dColor;
	int nCount=0;
	CString str, strTemp;
	int i, j, k,temp = 0;;

	UpdateData(true);
	if (nBarcodeStatus!=0)	//바코드 입력이 아니거나 바코드 입력 중이면..
	{
		m_ctrlEditType.SetFocus();
		return;
	}
	m_strMessage.Empty();
	nBarcodeStatus = 1;	//바코드 입력 실행 중..

		//해드 수 만큼 반복하며 선택된 칸을 찾는다..
	GetDlgItem(IDC_BARCODE_INPUT)->EnableWindow(FALSE);
	GetDlgItem(IDC_OK)->EnableWindow(FALSE);
	GetDlgItem(IDC_SELECT_AUTO)->EnableWindow(FALSE);
	GetDlgItem(IDC_EXAMINE)->EnableWindow(FALSE);

	if(m_nBarcode == 0)
	{
		for(i=0 ; i<MAX_NOZZLE/2 ; i++)	
		{
	//		for (j=1 ; j<5 ; j++)
	//		{
				m_ctrlLcType.SetCol(1);
				m_ctrlLcType.SetRow(i+1);
				if(m_ctrlLcType.GetCellBackColor() == LIGHTGRAY) continue;
				if(m_bSeleted_Head[i]) m_ctrlLcType.SetCellBackColor(LIGHTCYAN);
				else m_ctrlLcType.SetCellBackColor(WHITE);
				m_ctrlLcType.SetCol(3);
				m_ctrlLcType.SetRow(i+1);
				if(m_ctrlLcType.GetCellBackColor() == LIGHTGRAY) continue;
				if(m_bSeleted_Head[i+MAX_NOZZLE/2]) m_ctrlLcType.SetCellBackColor(LIGHTCYAN);
				else m_ctrlLcType.SetCellBackColor(WHITE);

	//		}
		}

		for(i=0 ; i<MAX_NOZZLE ; i++)	
		{
			if(m_bSeleted_Head[i] == TRUE)
			{
				temp++;
	//			if(m_bBarEnable == TRUE) {m_strMessage.Format("#%d번 액정병의 Barcode를 찍으십시요!", i+1); nCount = 20;}
	//			else {m_strMessage.Format("#%d번 액정병의 Barcode 값을 직접 입력하십시요!", i+1); nCount = 6;}
				if(m_bBarEnable == TRUE) {m_strMessage.Format("#%d번 액정병의 Barcode를 찍으십시요!", i+1); nCount = 29;} //LC Type과 Lot No를 같이 읽는다.
				else {m_strMessage.Format("#%d번 액정병의 Barcode 값을 직접 입력하십시요!", i+1); nCount = 18;}
				m_strBarcode = "";
				UpdateData(FALSE);
				if(i < MAX_NOZZLE/2)
				{
					m_ctrlLcType.SetRow(i+1);
					m_ctrlLcType.SetCol(1);
				}
				else if((i >= (MAX_NOZZLE/2))&&(i < MAX_NOZZLE))
				{
					m_ctrlLcType.SetRow((i-MAX_NOZZLE/2)+1);
					m_ctrlLcType.SetCol(3);
				}
	// 			else if((i >= (MAX_NOZZLE/4*2))&&(i < (MAX_NOZZLE/4)*3))
	// 			{
	// 				m_ctrlLcType.SetRow(3);
	// 				m_ctrlLcType.SetCol((MAX_NOZZLE/4)*3-i);
	// 			}
	// 			else
	// 			{
	// 				m_ctrlLcType.SetRow(4);
	// 				m_ctrlLcType.SetCol((MAX_NOZZLE/4)*4-i);
	// 			}
				m_ctrlLcType.SetCellBackColor(YELLOW);
				m_ctrlEditType.SetFocus(); 

				while (1) 
				{
					pFrame->DoEvents();
					if(m_bBarEnable == FALSE)
					{
						if(i < MAX_NOZZLE/2) strTemp=m_ctrlLcType.GetTextMatrix(i+1,1);
						else if((i >= (MAX_NOZZLE/2))&&(i < MAX_NOZZLE)) strTemp=m_ctrlLcType.GetTextMatrix((i-MAX_NOZZLE/2)+1,3);
	//					else if((i >= (MAX_NOZZLE/4)*2)&&(i < (MAX_NOZZLE/4)*3)) strTemp=m_ctrlLcType.GetTextMatrix(3,(MAX_NOZZLE/4)*3-i);
	//					else strTemp=m_ctrlLcType.GetTextMatrix(4,(MAX_NOZZLE/4)*4-i);

						m_ctrlLcTypeName.SetCaption(strTemp);

						Use_STR(m_ctrlLcTypeName, CPoint(170,305));	
						m_strBarcode = m_ctrlLcTypeName.GetCaption();
	/*					nCount = m_ctrlLcTypeName.GetCaption().GetLength();
						if (!(nCount==6))	//직접 입력 하면 12개 이므로..
						{
							m_ctrlLcTypeName.SetCaption(strTemp);
							AfxMessageBox("입력 문자의 개수가 맞지 않습니다.");
							return;
						}

						m_ctrlLcType.SetTextMatrix(nR,nC,m_ctrlLcTypeName.GetCaption());	*/
					}
					else UpdateData(TRUE);
					if (m_strBarcode.GetLength() != 0)
					{
						for (k=0 ; k<100 ; k++)	//시간 내 입력이 제대로 안되었을 경우..
						{
							if(m_bBarEnable) UpdateData(TRUE);
							pFrame->DoEvents();
							if (m_strBarcode.GetLength() == nCount)
							{
								Sleep(100);	//잠시 보여준다.. 
								break;
							}
							else if(m_bBarEnable == FALSE) k = 99;
							Sleep(50);
						}
					}
					if ((m_strBarcode.GetLength() == nCount) || nBarcodeStatus==99)	//nBarcodeStop=99= 정지..
					{
						break;
					}
					if (k == 100)
					{
						k = 0;
						m_strBarcode = "";
						UpdateData(false);
						str.Format("입력 Data가 잘못 되었습니다. 다시 입력 해주세요\n");
						if(m_bBarEnable) AfxMessageBox(str);
						else 
						{
							str += "다시 입력하시겠습니까?";
							if(AfxMessageBox(str,MB_SYSTEMMODAL|MB_YESNO)==IDNO) break;
						}
					}
				}
				if (nBarcodeStatus==99)
					break;

				str.Empty();
	/*			for (n=2 ; n<(nCount-2) ; n++)	//유효 12자리를 구한다..2005.12.28..hoho
					if (m_strBarcode.Mid(n,1) != '-')
						str +=m_strBarcode.Mid(n,1);
	*/
	//			if(m_strBarcode.GetLength() == 20)
				if(m_strBarcode.GetLength() == 29)
				{
					k = m_strBarcode.FindOneOf("-");
					if(k != 2)
					{
						AfxMessageBox("입력된 문자의 3번째 자리 문자가 "-"이 아닙니다.");
						continue;
					}
					for(j = 0; j < k+1; j++) m_strBarcode.Delete(0);
					for(j = 0; j < 18; j++) str += m_strBarcode.Mid(j,1);
	//				for(j = 3; j < 21; j++) str += m_strBarcode.Mid(j,1);

	//				for(j = 0; j < 6; j++) str += m_strBarcode.Mid(j,1);
				}
				else if(m_strBarcode.GetLength() == 18) str = m_strBarcode;
				else continue;
	//			m_ctrlLcTypeName.SetCaption(str);

			   if(m_bBarAllSelect == TRUE)
			   {
				   for(int HeadNo = 0; HeadNo < MAX_NOZZLE; HeadNo++)
				   {
					   if(m_bSeleted_Head[HeadNo] == TRUE)
					   {
							if(HeadNo < MAX_NOZZLE/2)
							{
								m_ctrlLcType.SetTextMatrix(HeadNo+1, 1, str);
								m_ctrlLcType.SetRow(HeadNo+1);
								m_ctrlLcType.SetCol(1);
							}
							else if((HeadNo >= (MAX_NOZZLE/2))&&(HeadNo < MAX_NOZZLE))
							{
								m_ctrlLcType.SetTextMatrix((HeadNo-MAX_NOZZLE/2)+1, 3, str);
								m_ctrlLcType.SetRow((HeadNo-MAX_NOZZLE/2)+1);
								m_ctrlLcType.SetCol(3);
							}
	// 						else if((HeadNo >= (MAX_NOZZLE/4)*2)&&(HeadNo < (MAX_NOZZLE/4)*3))
	// 						{
	// 							m_ctrlLcType.SetTextMatrix(3, (MAX_NOZZLE/4)*3-HeadNo, str);
	// 							m_ctrlLcType.SetRow(3);
	// 							m_ctrlLcType.SetCol((MAX_NOZZLE/4)*3-HeadNo);
	// 						}
	// 						else
	// 						{
	// 							m_ctrlLcType.SetTextMatrix(4, (MAX_NOZZLE/4)*4-HeadNo, str);
	// 							m_ctrlLcType.SetRow(4);
	// 							m_ctrlLcType.SetCol((MAX_NOZZLE/4)*4-HeadNo);
	// 						}

							m_ctrlLcType.SetCellBackColor(WHITE);
				//			m_ctrlLcType.SetTextMatrix(, i, str);	
							m_strBarcode = "";
							UpdateData(false);
					   }
				   }
				   if(m_bBarAllSelect == TRUE) break;
			   }
			   else
			   {
					if(i < MAX_NOZZLE/2)
					{
						m_ctrlLcType.SetTextMatrix(i+1, 1, str);
						m_ctrlLcType.SetRow(i+1);
						m_ctrlLcType.SetCol(1);
					}
					else if((i >= (MAX_NOZZLE/2))&&(i < MAX_NOZZLE))
					{
						m_ctrlLcType.SetTextMatrix((i-MAX_NOZZLE/2)+1, 3, str);
						m_ctrlLcType.SetRow((i-MAX_NOZZLE/2)+1);
						m_ctrlLcType.SetCol(3);
					}
	// 				else if((i >= (MAX_NOZZLE/4)*2)&&(i < (MAX_NOZZLE/4)*3))
	// 				{
	// 					m_ctrlLcType.SetTextMatrix(3, (MAX_NOZZLE/4)*3-i, str);
	// 					m_ctrlLcType.SetRow(3);
	// 					m_ctrlLcType.SetCol((MAX_NOZZLE/4)*3-i);
	// 				}
	// 				else
	// 				{
	// 					m_ctrlLcType.SetTextMatrix(4, (MAX_NOZZLE/4)*4-i, str);
	// 					m_ctrlLcType.SetRow(4);
	// 					m_ctrlLcType.SetCol((MAX_NOZZLE/4)*4-i);
	// 				}

					m_ctrlLcType.SetCellBackColor(WHITE);
		//			m_ctrlLcType.SetTextMatrix(, i, str);	
					m_strBarcode = "";
					UpdateData(false);
			   }
			}
	//		if(m_bBarAllSelect == TRUE) break;
		}
		if (nBarcodeStatus==99)
		{
			for(i=0 ; i<MAX_NOZZLE/2 ; i++)	
			{
				for (j=1 ; j<4 ; j++)
				{
					m_ctrlLcType.SetCol(j);
					m_ctrlLcType.SetRow(i+1);
					if(m_ctrlLcType.GetCellBackColor() == LIGHTGRAY) continue;
					m_ctrlLcType.SetCellBackColor(WHITE);
				}
			}
		}

		if(temp == 0) m_strMessage = "Head를 선택하여 주십시요.";
		else if(OnCompare_BarValue() == TRUE)
		{
			str.Empty();
			for(i = 0; i < MAX_NOZZLE; i++)
			{
				if(m_bSeleted_Head[i])
				{
					strTemp.Format(" %d번",i+1);
					str += strTemp;
				}
			}
			m_strMessage = str + "Head의 Barcode 입력을 완료하였습니다.";
		}
		for(i = 0; i < MAX_NOZZLE; i++) m_bSeleted_Head[i] = FALSE;
	}
/******************************************************************************************************************************************/
/******************************************************************************************************************************************/
	else if(m_nBarcode == 1)
	{
		m_bBarAllSelect = FALSE;
		for(i=0 ; i<2 ; i++)	
		{
			m_ctrlCanisterLcType.SetCol(1-i);
			m_ctrlCanisterLcType.SetRow(1);
			if(m_ctrlCanisterLcType.GetCellBackColor() == LIGHTGRAY) continue;
			if(m_bSelected_Canister[i]) m_ctrlCanisterLcType.SetCellBackColor(LIGHTCYAN);
			else m_ctrlCanisterLcType.SetCellBackColor(WHITE);
		}

		for(i=0 ; i<2 ; i++)	
		{
			if(m_bSelected_Canister[i] == TRUE)
			{
				temp++;
				if(m_bBarEnable == TRUE) {m_strMessage.Format("#%d번 Canister의 Barcode를 찍으십시요!", i+1); nCount = 29;} //LC Type과 Lot No를 같이 읽는다.
				else {m_strMessage.Format("#%d번 Canister의 Barcode 값을 직접 입력하십시요!", i+1); nCount = 18;}
				m_strBarcode = "";
				UpdateData(FALSE);
//				if(i < MAX_NOZZLE/2)
//				{
					m_ctrlCanisterLcType.SetRow(1);
					m_ctrlCanisterLcType.SetCol(i);
//				}
// 				else if((i >= (MAX_NOZZLE/2))&&(i < MAX_NOZZLE))
// 				{
// 					m_ctrlLcType.SetRow((i-MAX_NOZZLE/2)+1);
// 					m_ctrlLcType.SetCol(3);
// 				}
	// 			else if((i >= (MAX_NOZZLE/4*2))&&(i < (MAX_NOZZLE/4)*3))
	// 			{
	// 				m_ctrlLcType.SetRow(3);
	// 				m_ctrlLcType.SetCol((MAX_NOZZLE/4)*3-i);
	// 			}
	// 			else
	// 			{
	// 				m_ctrlLcType.SetRow(4);
	// 				m_ctrlLcType.SetCol((MAX_NOZZLE/4)*4-i);
	// 			}
				m_ctrlCanisterLcType.SetCellBackColor(YELLOW);
				m_ctrlEditType.SetFocus(); 

				while (1) 
				{
					pFrame->DoEvents();
					if(m_bBarEnable == FALSE)
					{
						strTemp=m_ctrlCanisterLcType.GetTextMatrix(1,i);
	//					if(i < MAX_NOZZLE/2) strTemp=m_ctrlLcType.GetTextMatrix(1,i);
	//					else if((i >= (MAX_NOZZLE/2))&&(i < MAX_NOZZLE)) strTemp=m_ctrlLcType.GetTextMatrix((i-MAX_NOZZLE/2)+1,3);
	//					else if((i >= (MAX_NOZZLE/4)*2)&&(i < (MAX_NOZZLE/4)*3)) strTemp=m_ctrlLcType.GetTextMatrix(3,(MAX_NOZZLE/4)*3-i);
	//					else strTemp=m_ctrlLcType.GetTextMatrix(4,(MAX_NOZZLE/4)*4-i);

						m_ctrlLcTypeName.SetCaption(strTemp);

						Use_STR(m_ctrlLcTypeName, CPoint(170,305));	
						m_strBarcode = m_ctrlLcTypeName.GetCaption();
	/*					nCount = m_ctrlLcTypeName.GetCaption().GetLength();
						if (!(nCount==6))	//직접 입력 하면 12개 이므로..
						{
							m_ctrlLcTypeName.SetCaption(strTemp);
							AfxMessageBox("입력 문자의 개수가 맞지 않습니다.");
							return;
						}

						m_ctrlLcType.SetTextMatrix(nR,nC,m_ctrlLcTypeName.GetCaption());	*/
					}
					else UpdateData(TRUE);
					if (m_strBarcode.GetLength() != 0)
					{
						for (k=0 ; k<100 ; k++)	//시간 내 입력이 제대로 안되었을 경우..
						{
							if(m_bBarEnable) UpdateData(TRUE);
							pFrame->DoEvents();
							if (m_strBarcode.GetLength() == nCount)
							{
								Sleep(100);	//잠시 보여준다.. 
								break;
							}
							else if(m_bBarEnable == FALSE) k = 99;
							Sleep(50);
						}
					}
					if ((m_strBarcode.GetLength() == nCount) || nBarcodeStatus==99)	//nBarcodeStop=99= 정지..
					{
						break;
					}
					if (k == 100)
					{
						k = 0;
						m_strBarcode = "";
						UpdateData(false);
						str.Format("입력 Data가 잘못 되었습니다. 다시 입력 해주세요\n");
						if(m_bBarEnable) AfxMessageBox(str);
						else 
						{
							str += "다시 입력하시겠습니까?";
							if(AfxMessageBox(str,MB_SYSTEMMODAL|MB_YESNO)==IDNO) break;
						}
					}
				}
				if (nBarcodeStatus==99)
					break;

				str.Empty();
	/*			for (n=2 ; n<(nCount-2) ; n++)	//유효 12자리를 구한다..2005.12.28..hoho
					if (m_strBarcode.Mid(n,1) != '-')
						str +=m_strBarcode.Mid(n,1);
	*/
	//			if(m_strBarcode.GetLength() == 20)
				if(m_strBarcode.GetLength() == 29)
				{
					k = m_strBarcode.FindOneOf("-");
					if(k != 2)
					{
						AfxMessageBox("입력된 문자의 3번째 자리 문자가 "-"이 아닙니다.");
						continue;
					}
					for(j = 0; j < k+1; j++) m_strBarcode.Delete(0);
					for(j = 0; j < 18; j++) str += m_strBarcode.Mid(j,1);
	//				for(j = 3; j < 21; j++) str += m_strBarcode.Mid(j,1);

	//				for(j = 0; j < 6; j++) str += m_strBarcode.Mid(j,1);
				}
				else if(m_strBarcode.GetLength() == 18) str = m_strBarcode;
				else continue;
	//			m_ctrlLcTypeName.SetCaption(str);

			   if(m_bBarAllSelect == TRUE)
			   {
// 				   for(int HeadNo = 0; HeadNo < MAX_NOZZLE; HeadNo++)
// 				   {
// 					   if(m_bSeleted_Head[HeadNo] == TRUE)
// 					   {
// 							if(HeadNo < MAX_NOZZLE/2)
// 							{
// 								m_ctrlLcType.SetTextMatrix(HeadNo+1, 1, str);
// 								m_ctrlLcType.SetRow(HeadNo+1);
// 								m_ctrlLcType.SetCol(1);
// 							}
// 							else if((HeadNo >= (MAX_NOZZLE/2))&&(HeadNo < MAX_NOZZLE))
// 							{
// 								m_ctrlLcType.SetTextMatrix((HeadNo-MAX_NOZZLE/2)+1, 3, str);
// 								m_ctrlLcType.SetRow((HeadNo-MAX_NOZZLE/2)+1);
// 								m_ctrlLcType.SetCol(3);
// 							}
// 							m_ctrlLcType.SetCellBackColor(WHITE);
// 				//			m_ctrlLcType.SetTextMatrix(, i, str);	
// 							m_strBarcode = "";
// 							UpdateData(false);
// 					   }
// 				   }
// 				   if(m_bBarAllSelect == TRUE) break;
			   }
			   else
			   {
//					if(i < MAX_NOZZLE/2)
//					{
//						str = str.Mid(11,7);
						m_ctrlCanisterLcType.SetTextMatrix(1, i, str);
						m_ctrlCanisterLcType.SetRow(1);
						m_ctrlCanisterLcType.SetCol(i);
//					}
// 					else if((i >= (MAX_NOZZLE/2))&&(i < MAX_NOZZLE))
// 					{
// 						m_ctrlLcType.SetTextMatrix((i-MAX_NOZZLE/2)+1, 3, str);
// 						m_ctrlCanisterLcType.SetRow((i-MAX_NOZZLE/2)+1);
// 						m_ctrlCanisterLcType.SetCol(3);
// 					}
					m_ctrlCanisterLcType.SetCellBackColor(WHITE);
		//			m_ctrlLcType.SetTextMatrix(, i, str);	
					m_strBarcode = "";
					UpdateData(false);
			   }
			}
	//		if(m_bBarAllSelect == TRUE) break;
		}
		if (nBarcodeStatus==99)
		{
			for(i=0 ; i<2 ; i++)	
			{
				m_ctrlCanisterLcType.SetCol(i);
				m_ctrlCanisterLcType.SetRow(1);
				if(m_ctrlCanisterLcType.GetCellBackColor() == LIGHTGRAY) continue;
				m_ctrlCanisterLcType.SetCellBackColor(WHITE);
			}
		}

		if(temp == 0) m_strMessage = "Canister를 선택하여 주십시요.";
		else if(OnCompare_BarValue() == TRUE)
		{
			str.Empty();
			for(i = 0; i < 2; i++)
			{
				if(m_bSelected_Canister[i])
				{
					strTemp.Format(" %d번",i+1);
					str += strTemp;
				}
			}
			m_strMessage = str + "Canister의 Barcode 입력을 완료하였습니다.";
		}
		for(i = 0; i < 2; i++) m_bSelected_Canister[i] = FALSE;
	}
	GetDlgItem(IDC_BARCODE_INPUT)->EnableWindow(TRUE);
	GetDlgItem(IDC_OK)->EnableWindow(TRUE);
	GetDlgItem(IDC_EXAMINE)->EnableWindow(TRUE);
	GetDlgItem(IDC_SELECT_AUTO)->EnableWindow(TRUE);
	nBarcodeStatus = 0; //nBarcodeStatus=0=대기 상태..
	m_ctrlLcType.SetFocus();
}

void CBarcode::OnClickBarcodeStop() 
{
	if (nBarcodeStatus==1)		//바코드 입력 중이면..
		nBarcodeStatus = 99;	//정지 시킨다.
	else
		nBarcodeStatus = 0;		
}

void CBarcode::OnClickMsflexgridBarcode() 
{
	CString str,str1;
	BOOL btemp = FALSE;
//2010.07.05 by tskim 
	if(m_nBarcode != 0) return;


	if (nBarcodeStatus != 0)	//바코드 입력 중이면 return;
	{
		m_ctrlEditType.SetFocus();
		return;
	}

	if(m_ctrlLcType.GetMouseRow() == 0 && (m_ctrlLcType.GetMouseCol() == 0 || m_ctrlLcType.GetMouseCol() == 2))
	{
		return;
	}
	if(m_ctrlLcType.GetMouseCol() == 0 || m_ctrlLcType.GetMouseCol() == 2) return;

	if(m_ctrlLcType.GetCellBackColor() == LIGHTGRAY) return;

	

	UpdateData(true);

	int nC=0, nR=0, nCount=0;
	CString strTemp = "";

	nC = m_ctrlLcType.GetCol();
	nR = m_ctrlLcType.GetRow();
// 
// 	if(m_ctrlLcType.GetCellBackColor() == LIGHTGRAY)
// 	{
// 		strTemp=m_ctrlLcType.GetTextMatrix(nR,nC);
// 		m_ctrlLcTypeName.SetCaption(strTemp);
// 
// 		Use_STR(m_ctrlLcTypeName, CPoint(70,300));
// 		if(m_ctrlLcTypeName.GetCaption() =="") 
// 		{
// 			AfxMessageBox("입력 문자의 길이가 '0'이면 안됩니다.!!");
// 			m_ctrlLcTypeName.SetCaption("-");
// 		}
// 
// 		m_ctrlLcType.SetTextMatrix(nR,nC,m_ctrlLcTypeName.GetCaption());	
// 		return;
// 	}
	if (m_ctrlLcType.GetCellBackColor() == LIGHTCYAN)
		m_ctrlLcType.SetCellBackColor(WHITE);
	else
			m_ctrlLcType.SetCellBackColor(LIGHTCYAN);
//	}
	if(nR == 0)
	{
		if(nC == 1)
		{
			for(int i=0;i<MAX_NOZZLE/2;i++)
			{
				if(m_ctrlLcType.GetCellBackColor() == LIGHTCYAN) m_bSeleted_Head[i] = TRUE;
				else m_bSeleted_Head[i]= FALSE;
			}

		}
		else if(nC == 3)
		{
			for(int i=MAX_NOZZLE/2-1;i<MAX_NOZZLE;i++)
			{
				if(m_ctrlLcType.GetCellBackColor() == LIGHTCYAN) m_bSeleted_Head[i] = TRUE;
				else m_bSeleted_Head[i]= FALSE;
			}
		}
	}
	else
	{
		if(nC == 1)
		{
			if(m_ctrlLcType.GetCellBackColor() == LIGHTCYAN) m_bSeleted_Head[nR-1] = TRUE;
			else m_bSeleted_Head[nR-1]= FALSE;
		}
		else if(nC == 3)
		{
			if(m_ctrlLcType.GetCellBackColor() == LIGHTCYAN) m_bSeleted_Head[nR-1+MAX_NOZZLE/2] = TRUE;
			else m_bSeleted_Head[nR-1+MAX_NOZZLE/2]= FALSE;
		}
	}

	str1.Empty();
	for(int i = 0 ; i < MAX_NOZZLE; i++)
	{
		if(m_bSeleted_Head[i])
		{
			btemp = TRUE;
			str.Format("#%d번 ",i+1);
			str1 += str;
		}
	}
    if(btemp) m_strMessage = str1 + " Head가 선택 되었습니다.";
	else m_strMessage = "어떤 Head도 선택 되지 않았습니다.";
}

void CBarcode::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case 0:
		m_ctrlExplain.SetCaption(m_strMessage);
		break;
	case 1:
		break;
	}
	CDialog::OnTimer(nIDEvent);
}

BOOL CBarcode::OnCompare_BarValue()
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

//	char ch;
	CString str, str1 = "", str2;
	BOOL bFault[MAX_NOZZLE] = {false,};
	BOOL btemp = FALSE;
	int i;
	CString strcmpLcType;
	for(i=0;i<MAX_NOZZLE;i++)
	{
		if(pDoc->m_bIsHeadSelected[i])
		{
			strcmpLcType = pDoc->m_structDataEditor.m_strBarcode[i];
			break;
		}
	}

//2010.07.05 by tskim
	if(m_nBarcode == 0)
	{
		for(i = 0; i < MAX_NOZZLE; i++)
		{
			//if(pDoc->m_bIsHeadSelected[i] == FALSE) continue;
			if(m_bSeleted_Head[i] == FALSE) continue;			
			if(i < MAX_NOZZLE/2)
			{
				str = m_ctrlLcType.GetTextMatrix(i+1,1);
				m_ctrlLcType.SetRow(i+1);
				m_ctrlLcType.SetCol(1);
			}
			else if((i >= (MAX_NOZZLE/2))&&(i < (MAX_NOZZLE)))
			{
				str = m_ctrlLcType.GetTextMatrix((i-MAX_NOZZLE/2)+1,3);
				m_ctrlLcType.SetRow((i-MAX_NOZZLE/2)+1);
				m_ctrlLcType.SetCol(3);
			}
//			CIM_Information.sBarcodeData[i] = str.Mid(11,7);	 

			if(pDoc->m_structDataEditor.m_strBarcode[i].GetLength() != 7)  // 비교구문
			{
				bFault[i] = true;
				m_ctrlLcType.SetCellBackColor(RED);
				Sleep(500);
			}
			else if(pDoc->m_structDataEditor.m_strBarcode[i] != pDoc->m_strRFDataBarcodeID[i])
			{
				btemp = TRUE;
				str2.Format(" %d번",i+1);
				str1 += str2;
				m_ctrlLcType.SetCellBackColor(RED);
				Sleep(100);
			}
			else 
			{
				m_ctrlLcType.SetCellBackColor(WHITE);
			}
		}

		if(btemp)
		{
			m_strMessage = str1 + " 입력된 값이 잘 못 되었습니다. 확인하세요!! \n";
		}
		else m_strMessage = "";
		str1.Empty();
		for(i = 0; i < MAX_NOZZLE; i++)
		{
			if(bFault[i] == TRUE)
			{
				btemp = TRUE;
				str2.Format(" %d번",i+1);
				str1 += str2;
			}
		}
		if(str1.GetLength() != 0) str1 += " Editor에 입력된 값이 7자리가 아닙니다. 확인하세요!!";
		m_strMessage += str1;
	}
/**********************************************************************************************************/
/**********************************************************************************************************/
	else if(m_nBarcode == 1)
	{
		for(i = 0; i < 2; i++)
		{
			str = m_ctrlCanisterLcType.GetTextMatrix(1,1-i);
			m_ctrlCanisterLcType.SetRow(1);
			m_ctrlCanisterLcType.SetCol(1-i);

			CIM_Information.sCanisterTypeData[i] = str.Mid(11,7);	 
			//CIM_Information.sCanisterTypeData[i] = str;

			if(pDoc->m_structDataEditor.m_strBarcode[i].GetLength() != 7)  // 비교구문
			{
				bFault[i] = true;
				m_ctrlCanisterLcType.SetCellBackColor(RED);
				Sleep(500);
			}
			else if(strcmpLcType != CIM_Information.sCanisterTypeData[i])
			{
				btemp = TRUE;
				str2.Format(" %d번",i+1);
				str1 += str2;
				m_ctrlCanisterLcType.SetCellBackColor(RED);
				Sleep(100);
			}
			else 
			{
				m_ctrlCanisterLcType.SetCellBackColor(WHITE);
			}
		}

		if(btemp)
		{
			m_strMessage = str1 + " 입력된 값이 잘 못 되었습니다. 확인하세요!! \n";
		}
		else m_strMessage = "";
		str1.Empty();
		for(i = 0; i < 2; i++)
		{
			if(bFault[i] == TRUE)
			{
				btemp = TRUE;
				str2.Format(" %d번",i+1);
				str1 += str2;
			}
		}
		if(str1.GetLength() != 0) str1 += " Editor에 입력된 값이 7자리가 아닙니다. 확인하세요!!";
		m_strMessage += str1;
	}
	
	return !btemp;
}

void CBarcode::OnClickSelectAuto() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	if(m_nBarcode != 0) return;

	BOOL btemp = FALSE;
	CString str1, str;
	for(int i = 0 ; i < MAX_NOZZLE; i++)
	{
		m_bSeleted_Head[i] = pDoc->m_bIsHeadSelected[i];

		if(i < MAX_NOZZLE/2) m_ctrlLcType.SetCol(1);
		else m_ctrlLcType.SetCol(3);

// 		if(i < MAX_NOZZLE/4) m_ctrlLcType.SetRow(1);
// 		else if((i >= (MAX_NOZZLE/4))&&(i < (MAX_NOZZLE/4)*2)) m_ctrlLcType.SetRow(2);
// 		else if((i >= (MAX_NOZZLE/4)*2)&&(i < (MAX_NOZZLE/4)*3)) m_ctrlLcType.SetRow(3);
// 		else m_ctrlLcType.SetRow(4);

		m_ctrlLcType.SetRow((i%(MAX_NOZZLE/2))+1);
		if(pDoc->m_bIsHeadSelected[i] == TRUE) m_ctrlLcType.SetCellBackColor(LIGHTCYAN);
		else m_ctrlLcType.SetCellBackColor(LIGHTGRAY);
	}

	str1.Empty();
	for(i = 0 ; i < MAX_NOZZLE; i++)
	{
		if(m_bSeleted_Head[i])
		{
			btemp = TRUE;
			str.Format("#%d번 ",i+1);
			str1 += str;
		}
	}
    if(btemp) m_strMessage = str1 + " Head가 선택 되었습니다.";
	else m_strMessage = "어떤 Head도 선택 되지 않았습니다.";
}

void CBarcode::OnDblClickMsflexgridBarcode() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	int nC=0, nR=0, nCount=0;
	CString strTemp = "";

	nC = m_ctrlLcType.GetCol();
	nR = m_ctrlLcType.GetRow();

	if(m_ctrlLcType.GetCellBackColor() == LIGHTGRAY) return;
	if(m_ctrlLcType.GetMouseRow()==0) return;

	UpdateData(TRUE);//by ckh 2009.02.11
	if (!m_bBarEnable)
	{
		for(int i=0 ; i<MAX_NOZZLE/4 ; i++)	
		{
			for (int j=1 ; j<5 ; j++)
			{
				if(j == 1) m_bSeleted_Head[i] = false;
				else if(j == 2) m_bSeleted_Head[i+MAX_NOZZLE/4] = false;
				else if(j == 3) m_bSeleted_Head[i+(MAX_NOZZLE/4)*2] = false;
				else m_bSeleted_Head[i+(MAX_NOZZLE/4)*3] = false;

				m_ctrlLcType.SetCol(i+1);
				m_ctrlLcType.SetRow(j);
				if(m_ctrlLcType.GetCellBackColor() == LIGHTGRAY) continue;
				m_ctrlLcType.SetCellBackColor(WHITE);
			}
		}
		if(nC == 0) 
		{
			for(i = 0; i < MAX_NOZZLE/4; i++)
			{
				m_ctrlLcType.SetCol(i+1);
				m_ctrlLcType.SetRow(nR);
				if(m_ctrlLcType.GetCellBackColor() == LIGHTGRAY) continue; // by ckh 2009.02.11
				m_ctrlLcType.SetCellBackColor(YELLOW);
			}	
			if(nR ==1) m_strMessage.Format("#%d,#%d,#%d,#%d번 액정병의 Barcode 값을 직접 입력하십시요!", 1,2,3,4);
			else if(nR ==2) m_strMessage.Format("#%d,#%d,#%d,#%d번 액정병의 Barcode 값을 직접 입력하십시요!", 5,6,7,8);
			else if(nR ==3) m_strMessage.Format("#%d,#%d,#%d,#%d번 액정병의 Barcode 값을 직접 입력하십시요!", 9,10,11,12);
			else if(nR ==4) m_strMessage.Format("#%d,#%d,#%d,#%d번 액정병의 Barcode 값을 직접 입력하십시요!", 13,14,15,16);
		}
		else 
		{
			m_ctrlLcType.SetCol(nC);
			m_ctrlLcType.SetRow(nR);
			m_ctrlLcType.SetCellBackColor(YELLOW);
			m_strMessage.Format("#%d번 액정병의 Barcode 값을 직접 입력하십시요!", 4*nR+nC-4);
		}

		if(nC == 0) 
		{			
			// by ckh 2009.02.11 추가함, 선택되지않은 셀의 데이터가 올라옴으로 수정
			for(i = 0; i < MAX_NOZZLE/4; i++)
			{
				m_ctrlLcType.SetCol(i+1);
				m_ctrlLcType.SetRow(nR);
				if(m_ctrlLcType.GetCellBackColor() == LIGHTGRAY) continue;
				break;
			}	
			strTemp=m_ctrlLcType.GetTextMatrix(nR,nC+1+i);

		}
		else strTemp=m_ctrlLcType.GetTextMatrix(nR,nC);

		m_ctrlLcTypeName.SetCaption(strTemp);

		Use_STR(m_ctrlLcTypeName, CPoint(70,300));	

		nCount = m_ctrlLcTypeName.GetCaption().GetLength();
		if (!(nCount==18))	
		{
			m_ctrlLcTypeName.SetCaption(strTemp);
			AfxMessageBox("입력 문자의 개수가 맞지 않습니다.");
			return;
		}
		strTemp =  m_ctrlLcTypeName.GetCaption();
		strTemp = strTemp.Mid(11,7);	

		if(nC == 0)
		{
//			m_ctrlLcType.SetTextMatrix(nR,nC+1,m_ctrlLcTypeName.GetCaption());

			for(i = 0; i < MAX_NOZZLE/4; i++)
			{
//				m_ctrlLcType.SetTextMatrix(nR,i+1,m_ctrlLcTypeName.GetCaption());

				if(pDoc->m_bIsHeadSelected[4*nR+i-4] == TRUE)
				{
					m_ctrlLcType.SetTextMatrix(nR,i+1,m_ctrlLcTypeName.GetCaption());
					if(pDoc->m_structDataEditor.m_strBarcode[4*nR+i+1-4-1].GetLength() != 7)
					{
						m_strMessage.Format("%d번 Editor에 입력된 값이 7자리가 아닙니다. 확인하세요!!",4*nR+i+1-4);
						m_ctrlLcType.SetCol(i+1);
						m_ctrlLcType.SetRow(nR);
						m_ctrlLcType.SetCellBackColor(RED);
					}
					else if(strTemp != pDoc->m_structDataEditor.m_strBarcode[4*nR+i+1-4-1])
					{
						m_strMessage.Format("%d번 입력된 값이 잘못 되었습니다. 확인하세요!!", 4*nR+i+1-4);
						m_ctrlLcType.SetCol(i+1);
						m_ctrlLcType.SetRow(nR);
						m_ctrlLcType.SetCellBackColor(RED);
					}
					else
					{
						m_strMessage.Format("%d번 Barcode값이 정상적으로 입력되었습니다.!!", 4*nR+i+1-4);
						m_ctrlLcType.SetCol(i+1);
						m_ctrlLcType.SetRow(nR);
						m_ctrlLcType.SetCellBackColor(WHITE);
					}
				}
			}
		}
		else
		{
			m_ctrlLcType.SetTextMatrix(nR,nC,m_ctrlLcTypeName.GetCaption());
			if(pDoc->m_structDataEditor.m_strBarcode[4*nR+nC-4-1].GetLength() != 7)
			{
				m_strMessage.Format("%d번 Editor에 입력된 값이 7자리가 아닙니다. 확인하세요!!",4*nR+nC-4);
				m_ctrlLcType.SetCellBackColor(RED);
			}
			else if(strTemp != pDoc->m_structDataEditor.m_strBarcode[4*nR+nC-4-1])
			{
				m_strMessage.Format("%d번 입력된 값이 잘못 되었습니다. 확인하세요!!", 4*nR+nC-4);
				m_ctrlLcType.SetCellBackColor(RED);
			}
			else
			{
				m_strMessage.Format("%d번 Barcode값이 정상적으로 입력되었습니다.!!", 4*nR+nC-4);
				m_ctrlLcType.SetCellBackColor(WHITE);
			}
		}
	}
}

void CBarcode::OnClickExamine() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	for(int i = 0; i < MAX_NOZZLE; i++) m_bSeleted_Head[i] = pDoc->m_bIsHeadSelected[i];
	Sleep(50);
	if(OnCompare_BarValue() == TRUE) 
	m_strMessage = "Barcode값이 정상적으로 입력되었습니다.";
	for(i = 0; i < MAX_NOZZLE; i++) m_bSeleted_Head[i] = false;
}

void CBarcode::OnClickMsflexgridCanisterType() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	BOOL bSingleRow = TRUE;
	CString strTemp = "";
	int nC=0, nR=0, nCount = 0;
	int i = 0;

	CString str,str1;
	BOOL btemp = FALSE;

	if(m_nBarcode !=1 ) return;

	if (nBarcodeStatus != 0)	//바코드 입력 중이면 return;
	{
		m_ctrlEditType.SetFocus();
		return;
	}

	if(m_ctrlCanisterLcType.GetMouseRow() == 0 && (m_ctrlCanisterLcType.GetMouseCol() == 0 ||m_ctrlCanisterLcType.GetMouseCol() == 1))
	{
		return;
	}
	if(m_ctrlCanisterLcType.GetMouseRow() == 0) return;

	if(m_ctrlCanisterLcType.GetCellBackColor() == LIGHTGRAY) return;

	

	UpdateData(true);

	nC = m_ctrlCanisterLcType.GetCol();
	nR = m_ctrlCanisterLcType.GetRow();

	if (m_ctrlCanisterLcType.GetCellBackColor() == LIGHTCYAN)
		m_ctrlCanisterLcType.SetCellBackColor(WHITE);
	else
			m_ctrlCanisterLcType.SetCellBackColor(LIGHTCYAN);
	
	if(nR == 0)
	{
	}
	else if(nR == 1)
	{
		if(m_ctrlCanisterLcType.GetCellBackColor() == LIGHTCYAN) m_bSelected_Canister[nC] = TRUE;
		else m_bSelected_Canister[nC]= FALSE;
	}

	str1.Empty();
	for(i = 0 ; i < 2; i++)
	{
		if(m_bSelected_Canister[i])
		{
			btemp = TRUE;
			str.Format("#%d번 ",i+1);
			str1 += str;
		}
	}
    if(btemp) m_strMessage = str1 + " Canister가 선택 되었습니다.";
	else m_strMessage = "어떤 Canister도 선택 되지 않았습니다.";

//2010.07.05 by tskim
// 	nC = m_ctrlCanisterLcType.GetCol();
// 	nR = m_ctrlCanisterLcType.GetRow();
// 	
// 	
// 	strTemp=m_ctrlCanisterLcType.GetTextMatrix(nR,nC);
// 	m_ctrlCanisterTypeName.SetCaption(strTemp);
// 	
// 	Use_STR(m_ctrlCanisterTypeName, CPoint(70,300));	
// 	
// 	
// 	nCount = m_ctrlCanisterTypeName.GetCaption().GetLength();
// 	
// 	if (!(nCount==7))	
// 	{
// 		m_ctrlCanisterTypeName.SetCaption(strTemp);
// 		AfxMessageBox("입력 문자의 개수가 맞지 않습니다. 7자리 입력");
// 		return;
// 	}
// 	
// 	strTemp =  m_ctrlCanisterTypeName.GetCaption();
// 	
// 	m_ctrlCanisterLcType.SetTextMatrix(nR,nC,strTemp);
// 	
// 	// Canister LC Type
// 	for(i =0 ; i <m_ctrlCanisterLcType.GetCols(); i++)
// 	{
// 		CIM_Information.sCanisterTypeData[i]=(LPCSTR)m_ctrlCanisterLcType.GetTextMatrix(1, 1-i);
// 	}
}


void CBarcode::OnClickLabelCanisterName() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	BOOL bSingleRow = TRUE;
	CString strTemp = "";
	int nC=0, nR=0, nCount = 0;
	int i = 0;

	nC = m_ctrlCanisterLcType.GetCol();
	nR = m_ctrlCanisterLcType.GetRow();

	strTemp=m_ctrlCanisterLcType.GetTextMatrix(nR,nC);
	m_ctrlCanisterTypeName.SetCaption(strTemp);

	Use_STR(m_ctrlCanisterTypeName, CPoint(70,300));	


	nCount = m_ctrlCanisterTypeName.GetCaption().GetLength();

	if (!(nCount==7))	
	{
		m_ctrlCanisterTypeName.SetCaption(strTemp);
		AfxMessageBox("입력 문자의 개수가 맞지 않습니다. 7자리 입력");
		return;
	}

	strTemp =  m_ctrlCanisterTypeName.GetCaption();

	for(i = 0; i < 2; i++)
	{
		m_ctrlCanisterLcType.SetTextMatrix(1,i,strTemp);
	}	

	// Canister LC Type
	for(i =0 ; i <m_ctrlCanisterLcType.GetCols(); i++)
	{
		CIM_Information.sCanisterTypeData[i]=(LPCSTR)m_ctrlCanisterLcType.GetTextMatrix(1, i);
	}	
}

void CBarcode::OnRadioHead() 
{
	// TODO: Add your control notification handler code here
	int i;

	for(i=0;i<MAX_NOZZLE;i++)
	{
		if(m_bSeleted_Head[i])
		{
			UpdateData(FALSE);
			return;
		}
	}
	for(i=0;i<2;i++)
	{
		if(m_bSelected_Canister[i])
		{
			UpdateData(FALSE);
			return;
		}
	}
	UpdateData(TRUE);
}

BOOL CBarcode::PreTranslateMessage(MSG* pMsg) 
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
	return CDialog::PreTranslateMessage(pMsg);
}

void CBarcode::OnClickRfInformation() 
{
	// TODO: Add your control notification handler code here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	int i;
	int Timeover;
	CString strLotNoTemp[MAX_NOZZLE],strHeadName[MAX_NOZZLE],strCanisterNo[MAX_NOZZLE],strRemain[MAX_NOZZLE];
	CString m_strAck,Temp1,Temp2,strLog;
	CString strTempData1[MAX_NOZZLE];
	CString strTempData2[MAX_NOZZLE];
//2015.04.29 by tskim 원재료 전산화Add
	CString strTmp1;
	long li;
	int ntemp[MAX_NOZZLE] = {0,};
	int nLength;

	BOOL bHeadSelect = FALSE;
	for(i=0;i<MAX_NOZZLE;i++)
	{
		if(m_bSeleted_Head[i])
		{
			bHeadSelect = TRUE;
			break;
		}
	}

	if(!bHeadSelect)
	{
		AfxMessageBox("선택 된 Head 없음");
		return;
	}

	pView->m_pDevice->MNET_BitSet(Disp_MIF_Report_confirm,FALSE);
	pView->m_pDevice->MNET_BitSet(Disp_MIF_Request,FALSE);
	
	strLog.Format("<MIF> RF 재료 정보 IF 시작!");
	pView->SaveLog(0,strLog);
	pView->m_pDevice->MNET_BitSet(Disp_MIF_Request,TRUE);
	strLog.Format("<MIF> Disp_MIF_Request ON!");
	pView->SaveLog(0,strLog);
	Sleep(1000);
	pView->m_pDevice->MNET_BitSet(Disp_MIF_Request,FALSE);
	strLog.Format("<MIF> Disp_MIF_Request OFF!");
	pView->SaveLog(0,strLog);

	for(i=0;i<MAX_NOZZLE;i++)
	{
		if(m_bSeleted_Head[i])
		{
			switch(i)
			{
				case  0:case  1:case  2:case  3:case  4:
					strLotNoTemp[i].Empty();strHeadName[i].Empty();strCanisterNo[i].Empty();strRemain[i].Empty();
					strTempData1[i].Empty(); strTempData2[i].Empty();

					strTempData1[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+(i*0x30),16,FALSE);	//ECS_MIF_DATA = 0x0700
					strLog.Format("<MIF> TEMP1 H\t%d\tAdd\t%04x\tstrTempData1\t%s",i+1,ECS_MIF_DATA+(i*0x30),strTempData1[i]);
					pView->SaveLog(0,strLog);
					strTempData2[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+0x10+(i*0x30),16,FALSE);
					strLog.Format("<MIF> TEMP2 H\t%d\tAdd\t%04x\tstrTempData2\t%s",i+1,ECS_MIF_DATA+0x10+(i*0x30),strTempData2[i]);
					pView->SaveLog(0,strLog);
			
					strLotNoTemp[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+(i*0x30),16,FALSE);

//2015.04.29 by tskim 원재료 전산화Add 테스트용 LC잔량보고
#if !EQ
//CL-1110240001-ML-1103-1F-0002 : RF정보 Format	
li = atoi(pDoc->m_strRFDataLOTID[i]);
li += i;
strTmp1.Format("CL-%d-ML-1103-1F-0002",li);
Temp1 = pView->m_pDevice->MakeHexDatatoPLC(strTmp1,32);
nLength = strTmp1.GetLength();
strLotNoTemp[i] = pView->m_pDevice->MakeStringDatafromPLC(Temp1);
//int nlength = strLotNoTemp[i].GetLength();
#endif
//2015.10.13 by tskim LC Material LOT ID 실처리 보고..
					pDoc->m_strRFDataLOTID_Full[i] = strLotNoTemp[i].Mid(0,30);
					pDoc->m_strRFDataLOTID[i] = strLotNoTemp[i].Mid(3,10);
					strLog.Format("<MIF>  H\t%d\tpDoc->m_strRFDataLOTID\t%s",i+1,pDoc->m_strRFDataLOTID[i]);
					pView->SaveLog(0,strLog);

//strLotNoTemp[i] = pView->m_pDevice->MNET_ReadWord(ESC_MIF_DATA+0x13+(i*0x20),13,FALSE);
					pDoc->m_strRFDataBarcodeID[i] = strLotNoTemp[i].Mid(14,7);
					strLog.Format("<MIF>  H\t%d\tpDoc->m_strRFDataBarcodeID\t%s",i+1,pDoc->m_strRFDataBarcodeID[i]);
					pView->SaveLog(0,strLog);

					strHeadName[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+ECS_MIF_Data_HeadName+(i*0x30),1,TRUE);//ECS_MIF_DATA = 0x0700 //ECS_MIF_Data_HeadName = 0x10
#if !EQ
strHeadName[i].Format("%d",i+1);
#endif
					pDoc->m_strRFDataHeadName[i] = strHeadName[i];
					strLog.Format("<MIF>  H\t%d\tpDoc->m_strRFDataHeadName\t%s",i+1,pDoc->m_strRFDataHeadName[i]);
					pView->SaveLog(0,strLog);
//2015.04.29 by tskim 원재료 전산화Add Canister잔량영역은 없다..
// 					strCanisterNo[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+0x11+(i*0x30),1,TRUE);
// 					pDoc->m_strRFDataBalanceNo[i] = strCanisterNo[i];
// 					strLog.Format("<MIF>  H\t%d\tpDoc->m_strRFDataBalanceNo\t%s",i+1,pDoc->m_strRFDataBalanceNo[i]);
// 					pView->SaveLog(0,strLog);
//ehji. 수정중. 
//2015.04.29 by tskim 원재료 전산화Add LC잔량보고
//					strRemain[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+0x12+(i*0x30),1,TRUE);
					strRemain[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+ECS_MIF_Data_Remain+(i*0x30),1,TRUE);//ECS_MIF_DATA = 0x0700 //ECS_MIF_Data_Remain = 0x11;


					//2015.04.29 by tskim 원재료 전산화Add 테스트용 LC잔량보고
#if !EQ
strRemain[i].Format("%04x",(WORD)(pDoc->m_dOffset[0][i]+i));
#endif
					//pDoc->m_strRFDataResidualData[i] = strRemain[i];	//ehji 150505
					//nTemp = m_pDevice->HexString2Dec(strTemp); ReadWord를 읽고 변환. 
					pDoc->m_strRFDataResidualData[i].Format("%d",pView->m_pDevice->HexConvert(strRemain[i].GetAt(0))*4096+pView->m_pDevice->HexConvert(strRemain[i].GetAt(1))*256+pView->m_pDevice->HexConvert(strRemain[i].GetAt(2))*16 + pView->m_pDevice->HexConvert(strRemain[i].GetAt(3)));
					strLog.Format("<MIF>  H\t%d\tpDoc->m_strRFDataResidualData\t%s(%s)",i+1,pDoc->m_strRFDataResidualData[i],strRemain[i]);
					pView->SaveLog(0,strLog);
//2015.04.29 by tskim 원재료 전산화Add Dummy Drop 보고 m_nRFDataDummyDrop 0: 실시 , 1: 미실시.
					pDoc->m_nRFDataDummyDrop[i] = atoi(pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+ECS_MIF_Data_DummyDrop+(i*0x30),1,TRUE));//ECS_MIF_DATA = 0x0700 //ECS_MIF_Data_DummyDrop = 0x2C
					strLog.Format("<MIF>  H\t%d\tpDoc->m_nRFDataDummyDrop\t%04d",i+1,pDoc->m_nRFDataDummyDrop[i]);
					pView->SaveLog(0,strLog);

					break; 
				case  5:case  6:case  7:case  8:case  9:
					strLotNoTemp[i].Empty();strHeadName[i].Empty();strCanisterNo[i].Empty();strRemain[i].Empty();
					strTempData1[i].Empty(); strTempData2[i].Empty();

					strTempData1[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+(i*0x30+0x10),16,FALSE); 
					strLog.Format("<MIF> TEMP1 H\t%d\tAdd\t%04x\tstrTempData1\t%s",i+1,ECS_MIF_DATA+(i*0x30+0x10),strTempData1[i]);
					pView->SaveLog(0,strLog);
					strTempData2[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+0x10+(i*0x30+0x10),16,FALSE);
					strLog.Format("<MIF> TEMP2 H\t%d\tAdd\t%04x\tstrTempData2\t%s",i+1,ECS_MIF_DATA+0x10+(i*0x30+0x10),strTempData2[i]);
					pView->SaveLog(0,strLog);
			
					strLotNoTemp[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+(i*0x30+0x10),16,FALSE);

//2015.04.29 by tskim 원재료 전산화Add 테스트용 LC잔량보고
#if !EQ
//CL-1110240001-ML-1103-1F-0002 : RF정보 Format	
li = atoi(pDoc->m_strRFDataLOTID[i]);
li += i;
strTmp1.Format("CL-%d-ML-1103-1F-0002",li);
Temp1 = pView->m_pDevice->MakeHexDatatoPLC(strTmp1,32);
strLotNoTemp[i] = pView->m_pDevice->MakeStringDatafromPLC(Temp1);
//int nlength = strLotNoTemp[i].GetLength();
#endif
//2015.10.13 by tskim LC Material LOT ID 실처리 보고..
					pDoc->m_strRFDataLOTID_Full[i] = strLotNoTemp[i].Mid(0,30);
					pDoc->m_strRFDataLOTID[i] = strLotNoTemp[i].Mid(3,10);
					strLog.Format("<MIF>  H\t%d\tpDoc->m_strRFDataLOTID\t%s",i+1,pDoc->m_strRFDataLOTID[i]);
					pView->SaveLog(0,strLog);

//strLotNoTemp[i] = pView->m_pDevice->MNET_ReadWord(ESC_MIF_DATA+0x13+(i*0x20),13,FALSE);
					pDoc->m_strRFDataBarcodeID[i] = strLotNoTemp[i].Mid(14,7);
					strLog.Format("<MIF>  H\t%d\tpDoc->m_strRFDataBarcodeID\t%s",i+1,pDoc->m_strRFDataBarcodeID[i]);
					pView->SaveLog(0,strLog);

					strHeadName[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+ECS_MIF_Data_HeadName+(i*0x30+0x10),1,TRUE);
#if !EQ
strHeadName[i].Format("%d",i+1);
#endif
					pDoc->m_strRFDataHeadName[i] = strHeadName[i];
					strLog.Format("<MIF>  H\t%d\tpDoc->m_strRFDataHeadName\t%s",i+1,pDoc->m_strRFDataHeadName[i]);
					pView->SaveLog(0,strLog);
//2015.04.29 by tskim 원재료 전산화Add Canister잔량영역은 없다..
// 					strCanisterNo[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+0x11+(i*0x30+0x10),1,TRUE);
// 					pDoc->m_strRFDataBalanceNo[i] = strCanisterNo[i];
// 					strLog.Format("<MIF>  H\t%d\tpDoc->m_strRFDataBalanceNo\t%s",i+1,pDoc->m_strRFDataBalanceNo[i]);
// 					pView->SaveLog(0,strLog);

//2015.04.29 by tskim 원재료 전산화Add LC잔량보고
//					strRemain[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+0x12+(i*0x30+0x10),1,TRUE);
					strRemain[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+ECS_MIF_Data_Remain+(i*0x30+0x10),1,TRUE); //700+11+
//2015.04.29 by tskim 원재료 전산화Add 테스트용 LC잔량보고
#if !EQ
strRemain[i].Format("%04x",(WORD)(pDoc->m_dOffset[0][i]+i));
#endif
					pDoc->m_strRFDataResidualData[i].Format("%d",pView->m_pDevice->HexConvert(strRemain[i].GetAt(0))*4096+pView->m_pDevice->HexConvert(strRemain[i].GetAt(1))*256+pView->m_pDevice->HexConvert(strRemain[i].GetAt(2))*16 + pView->m_pDevice->HexConvert(strRemain[i].GetAt(3)));
					strLog.Format("<MIF>  H\t%d\tpDoc->m_strRFDataResidualData\t%s(%s)",i+1,pDoc->m_strRFDataResidualData[i],strRemain[i]);
					pView->SaveLog(0,strLog);
//2015.04.29 by tskim 원재료 전산화Add Dummy Drop 보고 m_nRFDataDummyDrop 0: 실시 , 1: 미실시.
					pDoc->m_nRFDataDummyDrop[i] = atoi(pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+ECS_MIF_Data_DummyDrop+(i*0x30+0x10),1,TRUE));
					strLog.Format("<MIF>  H\t%d\tpDoc->m_nRFDataDummyDrop\t%04d",i+1,pDoc->m_nRFDataDummyDrop[i]);
					pView->SaveLog(0,strLog);
					break; 
				case 10:case 11:case 12:case 13:case 14:
					strLotNoTemp[i].Empty();strHeadName[i].Empty();strCanisterNo[i].Empty();strRemain[i].Empty();
					strTempData1[i].Empty(); strTempData2[i].Empty();

					strTempData1[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+(i*0x30+0x20),16,FALSE);
					strLog.Format("<MIF> TEMP1 H\t%d\tAdd\t%04x\tstrTempData1\t%s",i+1,ECS_MIF_DATA+(i*0x30+0x20),strTempData1[i]);
					pView->SaveLog(0,strLog);
					strTempData2[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+0x10+(i*0x30+0x20),16,FALSE);
					strLog.Format("<MIF> TEMP2 H\t%d\tAdd\t%04x\tstrTempData2\t%s",i+1,ECS_MIF_DATA+0x10+(i*0x30+0x20),strTempData2[i]);
					pView->SaveLog(0,strLog);
			
					strLotNoTemp[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+(i*0x30+0x20),16,FALSE);

//2015.04.29 by tskim 원재료 전산화Add 테스트용 LC잔량보고
#if !EQ
//CL-1110240001-ML-1103-1F-0002 : RF정보 Format	
li = atoi(pDoc->m_strRFDataLOTID[i]);
li += i;
strTmp1.Format("CL-%d-ML-1103-1F-0002",li);
Temp1 = pView->m_pDevice->MakeHexDatatoPLC(strTmp1,32);
strLotNoTemp[i] = pView->m_pDevice->MakeStringDatafromPLC(Temp1);
//int nlength = strLotNoTemp[i].GetLength();
#endif
//2015.10.13 by tskim LC Material LOT ID 실처리 보고..
					pDoc->m_strRFDataLOTID_Full[i] = strLotNoTemp[i].Mid(0,30);
					pDoc->m_strRFDataLOTID[i] = strLotNoTemp[i].Mid(3,10);
					strLog.Format("<MIF>  H\t%d\tpDoc->m_strRFDataLOTID\t%s",i+1,pDoc->m_strRFDataLOTID[i]);
					pView->SaveLog(0,strLog);

//strLotNoTemp[i] = pView->m_pDevice->MNET_ReadWord(ESC_MIF_DATA+0x13+(i*0x20),13,FALSE);
					pDoc->m_strRFDataBarcodeID[i] = strLotNoTemp[i].Mid(14,7);
					strLog.Format("<MIF>  H\t%d\tpDoc->m_strRFDataBarcodeID\t%s",i+1,pDoc->m_strRFDataBarcodeID[i]);
					pView->SaveLog(0,strLog);

					strHeadName[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+ECS_MIF_Data_HeadName+(i*0x30+0x20),1,TRUE);	//0700+10
#if !EQ
strHeadName[i].Format("%d",i+1);
#endif
					pDoc->m_strRFDataHeadName[i] = strHeadName[i];
					strLog.Format("<MIF>  H\t%d\tpDoc->m_strRFDataHeadName\t%s",i+1,pDoc->m_strRFDataHeadName[i]);
					pView->SaveLog(0,strLog);
//2015.04.29 by tskim 원재료 전산화Add Canister잔량영역은 없다..
// 					strCanisterNo[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+0x11+(i*0x30+0x20),1,TRUE);
// 					pDoc->m_strRFDataBalanceNo[i] = strCanisterNo[i];
// 					strLog.Format("<MIF>  H\t%d\tpDoc->m_strRFDataBalanceNo\t%s",i+1,pDoc->m_strRFDataBalanceNo[i]);
// 					pView->SaveLog(0,strLog);

//2015.04.29 by tskim 원재료 전산화Add LC잔량보고
//					strRemain[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+0x12+(i*0x30+0x20),1,TRUE);
					strRemain[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+ECS_MIF_Data_Remain+(i*0x30+0x20),1,TRUE);
//2015.04.29 by tskim 원재료 전산화Add 테스트용 LC잔량보고
#if !EQ
strRemain[i].Format("%04x",(WORD)(pDoc->m_dOffset[0][i]+i));
#endif
					pDoc->m_strRFDataResidualData[i].Format("%d",pView->m_pDevice->HexConvert(strRemain[i].GetAt(0))*4096+pView->m_pDevice->HexConvert(strRemain[i].GetAt(1))*256+pView->m_pDevice->HexConvert(strRemain[i].GetAt(2))*16 + pView->m_pDevice->HexConvert(strRemain[i].GetAt(3)));
					strLog.Format("<MIF>  H\t%d\tpDoc->m_strRFDataResidualData\t%s(%s)",i+1,pDoc->m_strRFDataResidualData[i],strRemain[i]);
					pView->SaveLog(0,strLog);
//2015.04.29 by tskim 원재료 전산화Add Dummy Drop 보고 m_nRFDataDummyDrop 0: 실시 , 1: 미실시.
					pDoc->m_nRFDataDummyDrop[i] = atoi(pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+ECS_MIF_Data_DummyDrop+(i*0x30+0x20),1,TRUE));
					strLog.Format("<MIF>  H\t%d\tpDoc->m_nRFDataDummyDrop\t%04d",i+1,pDoc->m_nRFDataDummyDrop[i]);
					pView->SaveLog(0,strLog);
					break; 
				case 15:
					strLotNoTemp[i].Empty();strHeadName[i].Empty();strCanisterNo[i].Empty();strRemain[i].Empty();
					strTempData1[i].Empty(); strTempData2[i].Empty();

					strTempData1[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+(i*0x30+0x30),16,FALSE);
					strLog.Format("<MIF> TEMP1 H\t%d\tAdd\t%04x\tstrTempData1\t%s",i+1,ECS_MIF_DATA+(i*0x30+0x30),strTempData1[i]);
					pView->SaveLog(0,strLog);
					strTempData2[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+0x10+(i*0x30+0x30),16,FALSE);
					strLog.Format("<MIF> TEMP2 H\t%d\tAdd\t%04x\tstrTempData2\t%s",i+1,ECS_MIF_DATA+0x10+(i*0x30+0x30),strTempData2[i]);
					pView->SaveLog(0,strLog);
			
					strLotNoTemp[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+(i*0x30+0x30),16,FALSE);

//2015.04.29 by tskim 원재료 전산화Add 테스트용 LC잔량보고
#if !EQ
//CL-1110240001-ML-1103-1F-0002 : RF정보 Format	
li = atoi(pDoc->m_strRFDataLOTID[i]);
li += i;
strTmp1.Format("CL-%d-ML-1103-1F-0002",li);
Temp1 = pView->m_pDevice->MakeHexDatatoPLC(strTmp1,32);
strLotNoTemp[i] = pView->m_pDevice->MakeStringDatafromPLC(Temp1);
//int nlength = strLotNoTemp[i].GetLength();
#endif
//2015.10.13 by tskim LC Material LOT ID 실처리 보고..
					pDoc->m_strRFDataLOTID_Full[i] = strLotNoTemp[i].Mid(0,30);
					pDoc->m_strRFDataLOTID[i] = strLotNoTemp[i].Mid(3,10);
					strLog.Format("<MIF>  H\t%d\tpDoc->m_strRFDataLOTID\t%s",i+1,pDoc->m_strRFDataLOTID[i]);
					pView->SaveLog(0,strLog);

//strLotNoTemp[i] = pView->m_pDevice->MNET_ReadWord(ESC_MIF_DATA+0x13+(i*0x20),13,FALSE);
					pDoc->m_strRFDataBarcodeID[i] = strLotNoTemp[i].Mid(14,7);
					strLog.Format("<MIF>  H\t%d\tpDoc->m_strRFDataBarcodeID\t%s",i+1,pDoc->m_strRFDataBarcodeID[i]);
					pView->SaveLog(0,strLog);

					strHeadName[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+ECS_MIF_Data_HeadName+(i*0x30+0x30),1,TRUE);
#if !EQ
strHeadName[i].Format("%d",i+1);
#endif
					pDoc->m_strRFDataHeadName[i] = strHeadName[i];
					strLog.Format("<MIF>  H\t%d\tpDoc->m_strRFDataHeadName\t%s",i+1,pDoc->m_strRFDataHeadName[i]);
					pView->SaveLog(0,strLog);
//2015.04.29 by tskim 원재료 전산화Add Canister잔량영역은 없다..
// 					strCanisterNo[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+0x11+(i*0x30+0x30),1,TRUE);
// 					pDoc->m_strRFDataBalanceNo[i] = strCanisterNo[i];
// 					strLog.Format("<MIF>  H\t%d\tpDoc->m_strRFDataBalanceNo\t%s",i+1,pDoc->m_strRFDataBalanceNo[i]);
// 					pView->SaveLog(0,strLog);

//2015.04.29 by tskim 원재료 전산화Add LC잔량보고
//					strRemain[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+0x12+(i*0x30+0x30),1,TRUE);
					strRemain[i] = pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+ECS_MIF_Data_Remain+(i*0x30+0x30),1,TRUE);
//2015.04.29 by tskim 원재료 전산화Add 테스트용 LC잔량보고
#if !EQ
strRemain[i].Format("%04x",(WORD)(pDoc->m_dOffset[0][i]+i));
#endif
					pDoc->m_strRFDataResidualData[i].Format("%d",pView->m_pDevice->HexConvert(strRemain[i].GetAt(0))*4096+pView->m_pDevice->HexConvert(strRemain[i].GetAt(1))*256+pView->m_pDevice->HexConvert(strRemain[i].GetAt(2))*16 + pView->m_pDevice->HexConvert(strRemain[i].GetAt(3)));
					strLog.Format("<MIF>  H\t%d\tpDoc->m_strRFDataResidualData\t%s(%s)",i+1,pDoc->m_strRFDataResidualData[i],strRemain[i]);
					pView->SaveLog(0,strLog);
//2015.04.29 by tskim 원재료 전산화Add Dummy Drop 보고  m_nRFDataDummyDrop 0: 실시 , 1: 미실시.
					pDoc->m_nRFDataDummyDrop[i] = atoi(pView->m_pDevice->MNET_ReadWord(ECS_MIF_DATA+ECS_MIF_Data_DummyDrop+(i*0x30+0x30),1,TRUE));
					strLog.Format("<MIF>  H\t%d\tpDoc->m_nRFDataDummyDrop\t%04d",i+1,pDoc->m_nRFDataDummyDrop[i]);
					pView->SaveLog(0,strLog);
					break; 
			}
		}
	}
//RFID Data 가공, 비교 구문 추가 
//	1006180002-ML-1103
	m_strAck.Empty();
	Temp1.Empty();
	for(i=MAX_NOZZLE-1;i>-1;i--)
	{
		if(pDoc->m_structDataEditor.m_strBarcode[i] == pDoc->m_strRFDataBarcodeID[i])
		{
//OK일때 처리 
			Temp1 = "0";
		}
		else
		{
			Temp1 = "1";
		}
		Temp2 +=Temp1;
		strLog.Format("<MIF>\tH\t%d\t설정정보\t%s\tMIF정보\t%s\tOK(0),NG(1)\t%s",i+1,pDoc->m_structDataEditor.m_strBarcode[i],pDoc->m_strRFDataBarcodeID[i],Temp1);
		pView->SaveLog(0,strLog);
	}

	m_strAck = pView->m_pDevice->BinToHex(Temp2);
	strLog.Format("<MIF> 판정 정보 Temp2 %s m_strAck %s",Temp2,m_strAck);
	pView->SaveLog(0,strLog);

	Timeover = 0;
	while(1)
	{
		if(pView->m_pDevice->MNET_Read_A_Bit(ECS_MIF_Report,0))
		{
			Sleep(100);
			if(pView->m_pDevice->MNET_Read_A_Bit(ECS_MIF_Report,0))
			{
				strLog.Format("<MIF> ECS_MIF_Report ON!");
				pView->SaveLog(0,strLog);

				pView->m_pDevice->MNET_WriteWord(Disp_MIF_DataACK,m_strAck,1);
				Sleep(100);
				strLog.Format("<MIF> Disp_MIF_DataACK Write!!");
				pView->SaveLog(0,strLog);

				pView->m_pDevice->MNET_BitSet(Disp_MIF_Report_confirm,TRUE);
				strLog.Format("<MIF> Disp_MIF_Report_confirm ON!!");
				pView->SaveLog(0,strLog);

				//				if(!pView->m_pDevice->MNET_Read_A_Bit(ECS_MIF_Report,0))
				//				{
				Sleep(1000);
				//					if(!pView->m_pDevice->MNET_Read_A_Bit(ECS_MIF_Report,0))
				//					{
				strLog.Format("<MIF> ECS_MIF_Report OFF!");
				pView->SaveLog(0,strLog);
				pView->m_pDevice->MNET_BitSet(Disp_MIF_Report_confirm,FALSE);
				strLog.Format("<MIF> Disp_MIF_Report_confirm OFF!!");
				pView->SaveLog(0,strLog);
				break;
				//					}
				//				}
			}
		}
		Timeover++;
		if(Timeover>30)
		{
			//Light Alarm추가
			pView->m_pDevice->MNET_BitSet(Disp_MIF_Report_confirm,FALSE);
			AfxMessageBox("TimeOver!!");
			break;
		}
		Sleep(100);
	}
	strLog.Format("<MIF> RF 재료 정보 IF 종료!");
	pView->SaveLog(0,strLog);

//2015.04.29 by tskim 원재료 전산화Add LC잔량보고
	CString str;
	for(i = 0 ; i < (MAX_NOZZLE/2) ; i++)
	{
		//str.Format("H%d\nRemain: %sg\nDummyDrop: %d",i+1,pDoc->m_strRFDataResidualData[i],pDoc->m_nRFDataDummyDrop[i]);//  pDoc->m_dOffset[0][i]);
		str.Format("HEAD%d",i+1);	//20160503
		m_ctrlLcType.SetTextMatrix(i+1,0 ,str);
		//str.Format("H%d\nRemain: %sg\nDummyDrop: %d",(MAX_NOZZLE/2)+(i+1),pDoc->m_strRFDataResidualData[(MAX_NOZZLE/2)+i],pDoc->m_nRFDataDummyDrop[(MAX_NOZZLE/2)+i]); //pDoc->m_dOffset[0][(MAX_NOZZLE/2)+i]);
		str.Format("HEAD%d",i+9);	//20160503
		m_ctrlLcType.SetTextMatrix(i+1,2 ,str);
	}

	for(i = 0 ; i <MAX_NOZZLE/2; i++)
	{
		m_ctrlLcType.SetTextMatrix(i+1, 1 ,pDoc->m_strRFDataLOTID[i]+"-"+(LPCSTR)pDoc->m_strRFDataBarcodeID[i]);
		m_ctrlLcType.SetTextMatrix(i+1, 3 ,pDoc->m_strRFDataLOTID[i+(MAX_NOZZLE/2)]+"-"+(LPCSTR)pDoc->m_strRFDataBarcodeID[i+(MAX_NOZZLE/2)]);
	}
	OnCompare_BarValue();

//2015.04.29 by tskim 원재료 전산화Add Dummy Drop보고 
	pDoc->Save_DummyDropStatus();

//2015.04.29 by tskim 원재료 전산화Add Pair Interlock
	pView->SendMessage(WM_CIM_REPORT_WORK,10,NULL);	

	pView->SendMessage(WM_CIM_REPORT_WORK,9,NULL);//150622 HSN 탈착 시 잔량 보고 추가 


//2011.07.11 by tskin MC navi
	pView->m_nMCNaviStatus[LC_CHANGE] = 2;
	pView->McNaviStatusReport();
}

void CBarcode::SelectLanguage()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(pView->m_nLanguage == 0)	//ehji 140309
	{
		SetDlgItemText(IDC_BARCODE,				_T("바코드 사용"));
		SetDlgItemText(IDC_BARCODE2,				_T("선택된 HEAD 동일 값 적용"));
		SetDlgItemText(IDC_BARCODE_INPUT,				_T("입력 시작"));
		SetDlgItemText(IDC_BARCODE_STOP,				_T("입력 중지"));
		SetDlgItemText(IDC_SELECT_AUTO,				_T("자동 선택"));
		SetDlgItemText(IDC_EXAMINE,				_T("검 사"));
		SetDlgItemText(IDC_RF_INFORMATION,				_T("RFID 정보 READ"));
		SetDlgItemText(IDC_STATIC,				_T("LC 정보 입력 선택"));
	}
	
	else if(pView->m_nLanguage == 1)	//ehji 140309
	{
		SetDlgItemText(IDC_BARCODE,				_T("Barcode Use"));
		SetDlgItemText(IDC_BARCODE2,				_T("Selected HEAD Value Apply"));
		SetDlgItemText(IDC_BARCODE_INPUT,				_T("Input Start"));
		SetDlgItemText(IDC_BARCODE_STOP,				_T("Input Stop"));
		SetDlgItemText(IDC_SELECT_AUTO,				_T("Auto Select"));
		SetDlgItemText(IDC_EXAMINE,				_T("Check"));
		SetDlgItemText(IDC_RF_INFORMATION,				_T("RFID Info READ"));
		SetDlgItemText(IDC_STATIC,				_T("LC Info Input Select"));
	}
	
	else if(pView->m_nLanguage == 2)	//ehji 140309
	{
		SetDlgItemText(IDC_BARCODE,				_T("使用調碼"));
		SetDlgItemText(IDC_BARCODE2,				_T("适用被選HEAD同一値"));
		SetDlgItemText(IDC_BARCODE_INPUT,				_T("開始輸入"));
		SetDlgItemText(IDC_BARCODE_STOP,				_T("停止輸入"));
		SetDlgItemText(IDC_SELECT_AUTO,				_T("自動選擇"));
		SetDlgItemText(IDC_EXAMINE,				_T("檢査"));
		SetDlgItemText(IDC_RF_INFORMATION,				_T("RFID 情報 READ"));
		SetDlgItemText(IDC_STATIC,				_T("選擇要輸入LC 情報 "));
	}

}

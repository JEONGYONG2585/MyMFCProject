// TeachingPosChk.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"
#include "P8CA_LcDispDoc.h"
#include "P8CA_LcDispView.h"
#include "TeachingPosChk.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Define_List.h"
extern PosChk_Data PosChk_Datas[MAX_NOZZLE];				// PosChk를 위한 각 축별 데이터

/////////////////////////////////////////////////////////////////////////////
// CTeachingPosChk dialog


CTeachingPosChk::CTeachingPosChk(CWnd* pParent /*=NULL*/)
	: CDialog(CTeachingPosChk::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTeachingPosChk)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTeachingPosChk::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTeachingPosChk)
	DDX_Control(pDX, IDC_GRID_TEACHING_POSCHK, m_ctrlGridTeaching);
	DDX_Control(pDX, IDC_LAB_TITLE, m_ctrlLabTitle);
	DDX_Control(pDX, IDC_CMD_TEACHING, m_ctrlTeaching);
	DDX_Control(pDX, IDC_CMD_DATASAVE, m_ctrlDataSave);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTeachingPosChk, CDialog)
	//{{AFX_MSG_MAP(CTeachingPosChk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTeachingPosChk message handlers

BOOL CTeachingPosChk::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_bPosChk = TRUE;
	SetGridData(); // 데이터 디스플레이
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTeachingPosChk::SetGridData()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	BOOL bResult;
	
	int i,j,nRow,nCol = 13;
	CString strCol[14]= {"Head",	"SpecMin",	"SpecMax", "Target", "Target",
		"MarkScore", "MarkScore", "AxisPos",	"AxisPos","Speed","Speed","Light","Light"};
	CString strCol1[14]= {"",	"X,Y(mm)",	"X,Y(mm)", "X(mm)", "Y(mm)",
		"Min", "Max", "X(mm)",	"Y(mm)","S(mm/s)","K(mm/s)","Use","Val"};

	CString strTemp;
	// Read Data
	// by ckh 2009.01.10 
	if(m_bPosChk == TRUE)
	{
		bResult = pView->m_pDevice->GetPosChkTeachingData(PosChk_Datas,MODE_POSCHK);// 정위치 티칭데이터 가져오기.(Spec 사양, 마크 사이즈 , 마크 위치 등)
	}
	else
	{
		bResult = pView->m_pDevice->GetPosChkTeachingData(PosChk_Datas,MODE_ALIGN);// 정위치 티칭데이터 가져오기.(Spec 사양, 마크 사이즈 , 마크 위치 등)
	}

	nRow = MAX_NOZZLE + 2;
	m_ctrlGridTeaching.SetRows(nRow);
	m_ctrlGridTeaching.SetCols(nCol);

	for(i=0;i<nRow;i++)
	{		
		// 컬럼 이름 표시
		for(j=0;j<nCol;j++)
		{
			if(i==0) // 첫번째 열 - 컬럼 제목 
			{
				m_ctrlGridTeaching.SetTextMatrix(i,j,strCol[j]);
				m_ctrlGridTeaching.SetColWidth(j,1110);
				m_ctrlGridTeaching.SetColWidth(0,1000);
				m_ctrlGridTeaching.SetColAlignment(j,4);
			}
			else if(i ==1)
			{
				m_ctrlGridTeaching.SetTextMatrix(i,j,strCol1[j]);
				m_ctrlGridTeaching.SetColWidth(j,1110);
				m_ctrlGridTeaching.SetColWidth(0,1000);
			}
			else
			{
				if(j==0)
				{
					strTemp.Format("Head_%d",i-1);
					m_ctrlGridTeaching.SetTextMatrix(i,j,strTemp);
					m_ctrlGridTeaching.SetRowHeight(i,450);
				}
				else
				{

				}
			}
			if(i>2 && i<nRow-1)
			{
				m_ctrlGridTeaching.SetRow(i);
				m_ctrlGridTeaching.SetCol(j);
				m_ctrlGridTeaching.SetCellBackColor(DARKGRAY);
			}
		}

		if(i < MAX_NOZZLE)
		{
			
			j=1;
			strTemp.Format("%.3f",PosChk_Datas[i].dSpecMin_X * PIXEL_SIZE );
			m_ctrlGridTeaching.SetTextMatrix(i+2,j,strTemp);
			++j;

			strTemp.Format("%.3f",PosChk_Datas[i].dSpecMax_X * PIXEL_SIZE);
			m_ctrlGridTeaching.SetTextMatrix(i+2,j,strTemp);
			++j;

			//PosChk_Datas[i].dMarkY;
//			strTemp.Format("%.3f",PosChk_Datas[i].dSpecMin_Y * PIXEL_SIZE);
			strTemp.Format("%.3f",PosChk_Datas[i].dTagetPosX);
			m_ctrlGridTeaching.SetTextMatrix(i+2,j,strTemp);
			++j;

//			strTemp.Format("%.3f",PosChk_Datas[i].dSpecMax_Y * PIXEL_SIZE );
			strTemp.Format("%.3f",PosChk_Datas[i].dTagetPosY);
			m_ctrlGridTeaching.SetTextMatrix(i+2,j,strTemp);
			++j;

			//PosChk_Datas[i].dMarkSize;
			strTemp.Format("%.f",PosChk_Datas[i].dMarkSizeMin); // by ckh 2009.01.10
			m_ctrlGridTeaching.SetTextMatrix(i+2,j,strTemp);
			++j;

			strTemp.Format("%.f",PosChk_Datas[i].dMarkSizeMax);
			m_ctrlGridTeaching.SetTextMatrix(i+2,j,strTemp);
			++j;

			strTemp.Format("%.3f",PosChk_Datas[i].dPosX);
			m_ctrlGridTeaching.SetTextMatrix(i+2,j,strTemp);
			++j;

			strTemp.Format("%.3f",PosChk_Datas[i].dPosY);
			m_ctrlGridTeaching.SetTextMatrix(i+2,j,strTemp);
			++j;

			strTemp.Format("%.3f",PosChk_Datas[i].dSpeed_S);
			m_ctrlGridTeaching.SetTextMatrix(i+2,j,strTemp);
			++j;

			strTemp.Format("%.3f",PosChk_Datas[i].dSpeed_K);
			m_ctrlGridTeaching.SetTextMatrix(i+2,j,strTemp);
			++j;

			strTemp.Format("%d",PosChk_Datas[i].bLightUse);
			m_ctrlGridTeaching.SetTextMatrix(i+2,j,strTemp);
			++j;

			strTemp.Format("%d",PosChk_Datas[i].nLightVal);
			m_ctrlGridTeaching.SetTextMatrix(i+2,j,strTemp);
			++j;
		}
	}
}

BEGIN_EVENTSINK_MAP(CTeachingPosChk, CDialog)
    //{{AFX_EVENTSINK_MAP(CTeachingPosChk)
	ON_EVENT(CTeachingPosChk, IDC_CMD_DATASAVE, -600 /* Click */, OnClickCmdDatasave, VTS_NONE)
	ON_EVENT(CTeachingPosChk, IDC_GRID_TEACHING_POSCHK, -600 /* Click */, OnClickGridTeachingPoschk, VTS_NONE)
	ON_EVENT(CTeachingPosChk, IDC_CMD_TEACHING, -600 /* Click */, OnClickCmdTeaching, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

// by ckh 2008.12.02
#include "NormalMsg.h"
void CTeachingPosChk::OnClickCmdDatasave() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView *)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	int i,j;
	
	CString strFile,strPath,strFileTemp;
	strPath.Format("D:\\TOP\\P8CA_LC\\P8CA_LC_DATA");
	// by ckh 2009.01.12
	CString strMsg;
	if(m_bPosChk == TRUE)
	{
		strFile = strPath + "\\Teaching_PosChk.dat";
		strMsg = " 변경된 위치를 '모터 정위치' 티칭 위치로 저장 하시겠습니까? ";
	}
	else
	{
		strFile = strPath + "\\Teaching_AlignMark.dat";
		strMsg = " 변경된 위치를 '얼라인 마크' 티칭 위치로 저장 하시겠습니까? ";
	}

	CNormalMsg dlg;
	dlg.m_bTimer=FALSE;

 	if(pView->m_nLanguage == 0)
	{
		dlg.m_strTitle = _T("확인");
		dlg.m_strMsg1 = strMsg;// by ckh 2009.01.12 
		dlg.m_strMsg2 = " 실행하려면 '실행'버튼 그렇지 않으면, '취소' 버튼을 누르세요!";
	}
	else if(pView->m_nLanguage == 1)
	{
		dlg.m_strTitle = _T("확인");
		dlg.m_strMsg1 = strMsg;// by ckh 2009.01.12 
		dlg.m_strMsg2 = " if you Action 'OK', or Not Action Please Push Button 'CANCEL'!";
	}
	else if(pView->m_nLanguage == 2)
	{
		dlg.m_strTitle = _T("확인");
		dlg.m_strMsg1 = strMsg;// by ckh 2009.01.12 
		dlg.m_strMsg2 = " 실행하려면 '실행'버튼 그렇지 않으면, '취소' 버튼을 누르세요!";
	}


	
	if( dlg.DoModal() == IDOK )
	{
		////////////////////////////////////////////////////////////
		// by ckh 
		for( i=0; i<MAX_NOZZLE;i++)// 데이터 가져오기
		{
			if(i == 0 || i == 15)
			{
				j=1;
				PosChk_Datas[i].dSpecMin_X = atof((char *)(LPCSTR)m_ctrlGridTeaching.GetTextMatrix(i+2,j))/PIXEL_SIZE;
				++j;
				PosChk_Datas[i].dSpecMax_X = atof((char *)(LPCSTR)m_ctrlGridTeaching.GetTextMatrix(i+2,j))/PIXEL_SIZE;
				++j;
//				PosChk_Datas[i].dSpecMin_Y = atof((char *)(LPCSTR)m_ctrlGridTeaching.GetTextMatrix(i+2,j))/PIXEL_SIZE;
				PosChk_Datas[i].dTagetPosX = atof((char *)(LPCSTR)m_ctrlGridTeaching.GetTextMatrix(i+2,j));
				++j;
//				PosChk_Datas[i].dSpecMax_Y = atof((char *)(LPCSTR)m_ctrlGridTeaching.GetTextMatrix(i+2,j))/PIXEL_SIZE;
				PosChk_Datas[i].dTagetPosY = atof((char *)(LPCSTR)m_ctrlGridTeaching.GetTextMatrix(i+2,j));
				++j;
				PosChk_Datas[i].dMarkSizeMin = atof((char *)(LPCSTR)m_ctrlGridTeaching.GetTextMatrix(i+2,j));
				++j;
				PosChk_Datas[i].dMarkSizeMax = atof((char *)(LPCSTR)m_ctrlGridTeaching.GetTextMatrix(i+2,j));
				++j;
				PosChk_Datas[i].dPosX = atof((char *)(LPCSTR)m_ctrlGridTeaching.GetTextMatrix(i+2,j));
				++j;
				PosChk_Datas[i].dPosY = atof((char *)(LPCSTR)m_ctrlGridTeaching.GetTextMatrix(i+2,j));
				++j;
				PosChk_Datas[i].dSpeed_S = atof((char *)(LPCSTR)m_ctrlGridTeaching.GetTextMatrix(i+2,j));
				++j;
				PosChk_Datas[i].dSpeed_K = atof((char *)(LPCSTR)m_ctrlGridTeaching.GetTextMatrix(i+2,j));

				++j;
				PosChk_Datas[i].bLightUse = atoi((char *)(LPCSTR)m_ctrlGridTeaching.GetTextMatrix(i+2,j));
				++j;
				PosChk_Datas[i].nLightVal = atoi((char *)(LPCSTR)m_ctrlGridTeaching.GetTextMatrix(i+2,j));
			}
			else
			{
				j=1;
				PosChk_Datas[i].dSpecMin_X = 0;
				++j;
				PosChk_Datas[i].dSpecMax_X = 0;
				++j;
				//PosChk_Datas[i].dSpecMin_Y = 0;
				PosChk_Datas[i].dTagetPosX = 0;
				++j;
				//PosChk_Datas[i].dSpecMax_Y = 0;
				PosChk_Datas[i].dTagetPosY = 0;
				++j;
				PosChk_Datas[i].dMarkSizeMin = 0;
				++j;
				PosChk_Datas[i].dMarkSizeMax = 0;
				++j;
				PosChk_Datas[i].dPosX = 0;
				++j;
				PosChk_Datas[i].dPosY = 0;
				++j;
				PosChk_Datas[i].dSpeed_S = 0;
				++j;
				PosChk_Datas[i].dSpeed_K = 0;

				++j;
				PosChk_Datas[i].bLightUse = 0;
				++j;
				PosChk_Datas[i].nLightVal = 0;
			}
		}

		for(i=0; i<MAX_NOZZLE;i++) // 스레드 내에서 헤더별 티칭 데이터동작을 하므로 하나씩 데이터를 저장하게 함.
		{
			pView->m_pDevice->SaveTeachingPosChk(PosChk_Datas[i],i);
		}

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

void CTeachingPosChk::OnClickGridTeachingPoschk() 
{
	int row, col;
//	double  m_dMin;//m_dMax,
//	int i;
	int n = 1;
	BOOL bSingle = TRUE;
	row = m_ctrlGridTeaching.GetMouseRow();
	col = m_ctrlGridTeaching.GetMouseCol();

	if( 0==col || 0 == row || 1 == row) return;
	if(row>2 && row<MAX_NOZZLE+1) return;

	//by ckh 2009.01.10;
	if(m_bPosChk != TRUE)
	{
		if(0 == row || m_ctrlGridTeaching.GetCellBackColor() == LIGHTGRAY)
			return;
	}

	int nMaxX[13] = { 10,10,1500,3000,100,100,1500,3000,300,300,1,256}; 
				//SpecMin_X(mm),SpecMax_X(mm), SpecMin_Y(mm),SpecMax_Y(mm),Mark_Size_Min,Mark_Size_Max,AxisPosX(mm),AxisPosY(mm)
	int nMinX[13] = {-10,	-10,	-1500,	-3000,	1, 1, -1500,-3000,0,0,0,0 };

	if(row == 0)
	{
//		bSingle = FALSE;
	}

	Use_TKg(m_ctrlGridTeaching,nMaxX[col-1],nMinX[col-1],0,0,bSingle,TRUE);
	
}

// by ckh 2009.01.10
void CTeachingPosChk::OnClickCmdTeaching() 
{
	int i,j;
	int nData = 14;
	if(m_bPosChk == FALSE) 	
	{
		m_bPosChk = TRUE;
		m_ctrlTeaching.SetCaption("Teaching Data        Align Mark >>");
		m_ctrlLabTitle.SetCaption("Position Check Teaching - 모터 정위치 Teaching Data");
		m_ctrlDataSave.SetCaption("Teaching Data            정위치 데이터 Save");
		
		for( i =1 ;i <nData;i++)
		{
			if(i<(nData)-1)
			{
				m_ctrlGridTeaching.SetCol(i);
				for(j=2;j<MAX_NOZZLE ; j++)
				{
					m_ctrlGridTeaching.SetRow(j);
					m_ctrlGridTeaching.SetCellBackColor(WHITE);
				}
			}
		}

	}
	else
	{
		m_bPosChk = FALSE;
		m_ctrlTeaching.SetCaption("<< Teaching Data        정위치 데이터");
		m_ctrlLabTitle.SetCaption("Position Check Teaching - Align Mark Teaching Data");
		m_ctrlDataSave.SetCaption("Teaching Data Align Mark Data Save");

		for( i =1 ;i <nData;i++)
		{
			if(i<(nData)-1)
			{
				m_ctrlGridTeaching.SetCol(i);
				for(j=2;j<MAX_NOZZLE ; j++)
				{
					m_ctrlGridTeaching.SetRow(j);
					m_ctrlGridTeaching.SetCellBackColor(LIGHTGRAY);
				}
			}
		}
	}
	SetGridData();

}

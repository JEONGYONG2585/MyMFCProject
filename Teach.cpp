// Teach.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"
#include "P8CA_LcDispDoc.h"
#include "DummyPos.h"

#include "Teach.h"
#include <fstream.h>
#include "P8CA_LcDispView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTeach dialog


CTeach::CTeach(CWnd* pParent /*=NULL*/)
	: CDialog(CTeach::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTeach)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pCap = new CString[80];

	for(int i = 0 ; i <40 ; i++)
		for(int j = 0 ; j < 3 ; j++)
		{
			m_structTeachingConfig[i][j].dMax = 10.0;
			m_structTeachingConfig[i][j].dMin = 0.0;
			m_structTeachingConfig[i][j].bCellOn = TRUE;
		}
	pDummyTeachDlg = NULL;
}

CTeach::~CTeach()
{
}

void CTeach::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTeach)
	DDX_Control(pDX, IDC_MSFLEXGRID_TEACH0, m_ctrlTeach0);
	DDX_Control(pDX, IDC_MSFLEXGRID_TEACH1, m_ctrlTeach1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTeach, CDialog)
	//{{AFX_MSG_MAP(CTeach)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTeach message handlers

BEGIN_EVENTSINK_MAP(CTeach, CDialog)
    //{{AFX_EVENTSINK_MAP(CTeach)
	ON_EVENT(CTeach, IDC_CMD_RETURN, -600 /* Click */, OnClickCmdReturn, VTS_NONE)
	ON_EVENT(CTeach, IDC_MSFLEXGRID_TEACH0, -600 /* Click */, OnClickMsflexgridTeach0, VTS_NONE)
	ON_EVENT(CTeach, IDC_MSFLEXGRID_TEACH1, -600 /* Click */, OnClickMsflexgridTeach1, VTS_NONE)
	ON_EVENT(CTeach, IDC_CMD_TEACHDATA_SAVE, -600 /* Click */, OnClickCmdTeachdataSave, VTS_NONE)
	ON_EVENT(CTeach, IDC_CMD_DUMMY_TEACH, -600 /* Click */, OnClickCmdDummyTeach, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CTeach::OnClickCmdReturn() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();	//JYKIM 170106 제어기 TEST 용 버튼(임시)

	pView->m_pPiezoDevice->Do_Standby_All(TRUE);			//JYKIM 170106 제어기 TEST 용 버튼(임시)
	pDoc->m_bTeachDataSave = FALSE;
	if(!pDummyTeachDlg) delete pDummyTeachDlg;
	EndDialog(IDOK);	
}

void CTeach::OnOK() 
{
	// TODO: Add extra validation here
	
//	CDialog::OnOK();
}

void CTeach::OnCancel() 
{
	// TODO: Add extra cleanup here
	
//	CDialog::OnCancel();
}

void CTeach::SubOpenStrFile()
{

	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	

	// TeachingStr.dat 화일을 열고서 임시저장
	ifstream fi;
	char ch[256];
	int i = 0, j = 0;


	if(pView->m_nLanguage == 0)
	fi.open("D:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\TeachingStr_kor.dat", ios::in);

	else if(pView->m_nLanguage == 1)
	fi.open("D:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\TeachingStr_eng.dat", ios::in);
	
	else if(pView->m_nLanguage == 2)
	fi.open("D:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\TeachingStr_chi.dat", ios::in);


	if(fi.is_open())
	{
		for(i =0 ; i <80 ; i++)
		{
			fi >> ch;
			m_pCap[i].Format("%s", ch);
		}
	}
	else
	{
		AfxMessageBox(" TeachingStr.dat 파일을 여는것을 실패하였습니다");
		return;
	}

	fi.close();

	// TeachingConfig.dat // 개별 cell의 max,min,on-off설정..
	fi.open("D:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\TeachingConfig.dat", ios::in);

	if(fi.is_open())
	{
		for(i =0 ; i <40 ; i++) // 40열
			for(j=0; j < 3 ; j++) // 3개행..
			{
				fi >> m_structTeachingConfig[i][j].dMax;
				fi >> m_structTeachingConfig[i][j].dMin;
				fi >> m_structTeachingConfig[i][j].bCellOn;
			}
	}
	else
	{
		AfxMessageBox(" TeachingConfig.dat 파일을 여는것을 실패하였습니다");
		return;
	}
	fi.close();
//
}

BOOL CTeach::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	// grid의 caption 설정하기 //
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	CString str = "", strFormat[3]={"(0)","(1)","(2)"};
	int i = 0 , j = 0;

	SubOpenStrFile();

	// Align 설정
	m_ctrlTeach0.SetColAlignment(0,1);
	m_ctrlTeach1.SetColAlignment(0,1);

	// caption 설정
	for(i =0 ; i < 20 ; i++) 
	{
		// col 0
		m_ctrlTeach0.SetTextMatrix(i+1,0,m_pCap[i]);
		m_ctrlTeach1.SetTextMatrix(i+1,0,m_pCap[i+20]);
		// col 1
		m_ctrlTeach0.SetTextMatrix(i+1,1,m_pCap[i+40]); //40 ~ 59
		m_ctrlTeach1.SetTextMatrix(i+1,1,m_pCap[i+60]); //60 ~ 79
	}

	for(i = 0; i < 3 ; i++)
	{
		m_ctrlTeach0.SetTextMatrix(0,i+2,strFormat[i]);
		m_ctrlTeach1.SetTextMatrix(0,i+2,strFormat[i]);
	}

	// cell config 설정 
	for(i =0 ; i <20 ; i++)		// 40열
		for(j=0; j < 3 ; j++)	// 3개행..
		{
			if(m_structTeachingConfig[i][j].bCellOn == FALSE)
			{
				m_ctrlTeach0.SetRow(i+1); // fixed 1
				m_ctrlTeach0.SetCol(j+2); // fixed 2
				m_ctrlTeach0.SetCellBackColor(DARKGRAY);
			}
			if(m_structTeachingConfig[i+20][j].bCellOn == FALSE)
			{
				m_ctrlTeach1.SetRow(i+1);
				m_ctrlTeach1.SetCol(j+2);
				m_ctrlTeach1.SetCellBackColor(DARKGRAY);
			}
		}

	// width 설정 // col0
	m_ctrlTeach0.SetColWidth(0, 3350);
	m_ctrlTeach1.SetColWidth(0, 3350);
	//	col1
	m_ctrlTeach0.SetColWidth(1, 810);
	m_ctrlTeach1.SetColWidth(1, 810);

	for(i=0 ; i < 21 ; i++) 
	{
		m_ctrlTeach0.SetRowHeight(i,380);
		m_ctrlTeach1.SetRowHeight(i,380);
	}

	////////////////////////////////////////////////////////////////////
//	DocToValue();		
	// teach0
	for(i =0 ; i <20; i ++)
	{
		for (j =0 ; j <3; j++)
		{
			str.Format("%.2f", pDoc->m_dTeachData[i][j]);
			m_ctrlTeach0.SetTextMatrix(i+1, j+2 ,(LPCSTR)str);
		}
	}	

	// teach1
	for(i = 0 ; i <20 ; i ++) // row
	{
		for (j =0 ; j <3; j++) // col
		{
			str.Format("%.2f", pDoc->m_dTeachData[i+20][j]);
			m_ctrlTeach1.SetTextMatrix(i+1, j+2 ,(LPCSTR)str);
		}
	}
	GetDlgItem(IDC_CMD_DUMMY_TEACH)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTeach::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	delete [] m_pCap;		
}



void CTeach::OnClickMsflexgridTeach0() 
{
	int row = 0 , col = 0;
	row = m_ctrlTeach0.GetMouseRow();
	col = m_ctrlTeach0.GetMouseCol();
//
	if(row == 0 ) return;
	if(col == 0 || col == 1) return;
	//
	if( m_structTeachingConfig[row-1][col-2].bCellOn == FALSE ) return;

	Use_TKg(m_ctrlTeach0, m_structTeachingConfig[row-1][col-2].dMax, m_structTeachingConfig[row-1][col-2].dMin, 512, 384, TRUE, TRUE);	// single == TRUE
}

void CTeach::OnClickMsflexgridTeach1() 
{
	int row = 0 , col = 0;
	row = m_ctrlTeach1.GetMouseRow();
	col = m_ctrlTeach1.GetMouseCol();
//
	if(row == 0 ) return;
	if(col == 0 || col == 1) return;
	//
	if( m_structTeachingConfig[row-1+20][col-2].bCellOn == FALSE ) return; // data +20
//
	Use_TKg(m_ctrlTeach1, m_structTeachingConfig[row-1+20][col-2].dMax, m_structTeachingConfig[row-1+20][col-2].dMin, 512, 384, TRUE, TRUE);	// single == TRUE
}

void CTeach::OnClickCmdTeachdataSave() 
{
	UpdateData(true);

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	// TeachPage1 ,2 에서 변경된 내용을 Docu에 Refresh
	// Column +2 , Row+1 Shifting
	for(int i =0 ; i <20 ;i ++) {
		for (int j =0 ; j <3 ; j++) {
			pDoc->m_dTeachData[i+0][j] = atof((char *)(LPCSTR)m_ctrlTeach0.GetTextMatrix(i+1, j+2));
			pDoc->m_dTeachData[i+20][j] = atof((char *)(LPCSTR)m_ctrlTeach1.GetTextMatrix(i+1, j+2));
		}
	}

//	CString str;			//ehji 140512 Intencity Log 남기기. 
//	str.Empty();
//	str.Format("Intencity : %d",pDoc->m_dTeachData[27][0]);
//	pView->SaveLog(0,str);


	// 파일명 설정 필요
	CString strPathName;
	strPathName.Format("%s\\Teaching.dat", pDoc->m_strDataPath);

	CFile SaveFile; 
	if( SaveFile.Open((char *)(LPCSTR)strPathName, CFile::modeCreate | CFile::modeWrite ) )
	{
		pDoc->m_nSelFile = 0;
		CArchive ar( &SaveFile, CArchive::store );
		pDoc->Serialize(ar);
		ar.Close();
		SaveFile.Close();
	}
	
	

//2008.01.11 by tskim
//Teaching file view

//	DataBK();


	FILE *fp;
	CString strPath;
	
	strPath.Format("D:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\Teaching(ReadOnly).dat");
	fp = fopen((char *)(LPCSTR)strPath, "wt");
	if(!fp) 
	{
		AfxMessageBox("Teaching(ReadOnly).dat 생성 실패");
	}
	
	for(i=0; i<40; i++)
	{
		fprintf(fp, "%s\t", m_pCap[i]);
		for(int j=0; j<3; j++)
		{
			fprintf(fp, "%.3f\t", pDoc->m_dTeachData[i][j]);
		}
		fprintf(fp, "\n");
	}
 	fclose(fp);

//2010.09.19 by tskim EC Data Read Teaching Data Read
//EC Data Load
 	for(i=0;i<MAX_NOZZLE;i++)//
 	{
 		pDoc->m_structEc.m_dDmyPos_S[i] = pDoc->m_dTeachData[i/3][i%3];
 		pDoc->m_structEc.m_dMeasPos_S[i] = pDoc->m_dTeachData[6+(i/2)][i%2];
		if(i<MAX_NOZZLE/2)//8,7,6....
		{
			pDoc->m_structEc.m_dSupPos_S[(MAX_NOZZLE/2-i)-1] = pDoc->m_dTeachData[28+(i/3)][(i%3)];
		}
		else//9,10,11....
		{
			pDoc->m_structEc.m_dSupPos_S[i] = pDoc->m_dTeachData[31+((i -MAX_NOZZLE/2)/3)][((i-MAX_NOZZLE/2)%3)];	
		}
 	}
	pDoc->m_structEc.m_dDmyPos_Col = pDoc->m_dTeachData[5][2];
	pDoc->m_structEc.m_dMeasPos_Col = pDoc->m_dTeachData[13][2];
	pDoc->m_structEc.m_dMutePos_Col[0] = pDoc->m_dTeachData[5][1];//Dummy Cup 교환 
	pDoc->m_structEc.m_dMutePos_Col[1] = pDoc->m_dTeachData[5][2];//Dummy Drop
	pDoc->m_structEc.m_dMutePos_Col[2] = pDoc->m_dTeachData[18][2];//작업자...
	pDoc->m_structEc.m_dMarkPos_S1 = pDoc->m_dTeachData[15][0];
	pDoc->m_structEc.m_dMarkPos_S16 = pDoc->m_dTeachData[15][1];
	pDoc->m_structEc.m_dMarkPos_Col = pDoc->m_dTeachData[15][2];
	for(i=0;i<3;i++)
	{
		pDoc->m_structEc.m_dSensingTime[i] = pDoc->m_dTeachData[16][i];
		pDoc->m_structEc.m_dTriggerTime[i] = pDoc->m_dTeachData[37][i];
	}
	pDoc->m_structEc.m_dCntPos_Col = pDoc->m_dTeachData[17][2];
	pDoc->m_structEc.m_dOperPos_Col = pDoc->m_dTeachData[18][2];
	pDoc->m_structEc.m_dLoadPos_F = pDoc->m_dTeachData[19][1];
	pDoc->m_structEc.m_dLoadPos_R = pDoc->m_dTeachData[19][2];
	pDoc->m_structEc.m_dSupPos_Col = pDoc->m_dTeachData[34][2];
	pDoc->m_structEc.m_dSupAmount = pDoc->m_dTeachData[36][0];
	pDoc->m_structEc.m_dSupTime = pDoc->m_dTeachData[36][2];
	pDoc->m_structEc.m_dInputLimit[0] = pDoc->m_dTeachData[38][0]; 
	pDoc->m_structEc.m_dInputLimit[1] = pDoc->m_dTeachData[38][1];
	pDoc->m_structEc.m_dColSpd = pDoc->m_dTeachData[39][2];

	pDoc->m_bTeachDataSave = TRUE;
	pView->SendMessage(WM_CIM_REPORT_WORK,5,NULL);



}



void CTeach::DataBK()
{
	CFileFind filefind;
	//FILE *fp;
	
	CTime t = CTime::GetCurrentTime();

	CString strDir, strDirName,strFileBk,strFileOrg;
	BOOL bIsDir = FALSE;
	BOOL bContinue = TRUE;

	strDir = "D:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\BK";

	filefind.FindFile("D:\\*.*");

	while(bContinue)
	{
		bContinue = filefind.FindNextFile();
		if(filefind.IsDirectory())
		{
			strDirName = filefind.GetFileName();
			if(strDir == strDirName)
			{
				bIsDir = TRUE;
				break;
			}
		}
	}

	filefind.Close();

	if(!bIsDir) _mkdir(strDir);

	strFileOrg = "D:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\Teaching(OnlyView).dat";
	strFileBk.Format("D:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\BK\\Teaching(OnlyView)_%04d%02d%02d_%02d%02d%02d.dat",t.GetYear(),t.GetMonth(),t.GetDay(),t.GetHour(),t.GetMinute(),t.GetSecond());

	CopyFile(strFileOrg,strFileBk,FALSE);

}

void CTeach::OnClickCmdDummyTeach() 
{
	// TODO: Add your control notification handler code here
	if(!pDummyTeachDlg) delete pDummyTeachDlg;

	pDummyTeachDlg = new CDummyPos;
	pDummyTeachDlg->Create(IDD_TEACH2, this);

	pDummyTeachDlg->SetWindowPos(NULL,100,100,
						0,0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER );		
}

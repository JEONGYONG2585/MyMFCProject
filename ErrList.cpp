// ErrList.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"
#include "ErrList.h"


//
#include <fstream.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CErrList dialog


CErrList::CErrList(CWnd* pParent /*=NULL*/)
	: CDialog(CErrList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CErrList)
	m_bCheckLog = FALSE;
	m_bPreviousErrList = FALSE;
	//}}AFX_DATA_INIT
}

CErrList::~CErrList()
{
}

void CErrList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CErrList)
	DDX_Control(pDX, IDC_BTN_CLEAR, m_ctrlClear);
	DDX_Control(pDX, IDC_BTN_LOG_OPEN, m_ctrlLog);
	DDX_Control(pDX, IDC_ERROR_LIST, m_ctrlErrList);
	DDX_Control(pDX, IDC_LABEL_DATE, m_ctrlDate);
	DDX_Check(pDX, IDC_CHECK_LOG, m_bCheckLog);
	DDX_Check(pDX, IDC_PREVIOUS_ERRLIST_CHECK, m_bPreviousErrList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CErrList, CDialog)
	//{{AFX_MSG_MAP(CErrList)
	ON_BN_CLICKED(IDC_BTN_LOG_OPEN, OnBtnLogOpen)
	ON_BN_CLICKED(IDC_CHECK_LOG, OnCheckLog)
	ON_BN_CLICKED(IDC_BTN_CLEAR, OnBtnClear)
	ON_BN_CLICKED(IDC_PREVIOUS_ERRLIST_CHECK, OnPreviousErrlistCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CErrList message handlers

BOOL CErrList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = ( CP8CA_LcDispDoc* )pFrame->GetActiveDocument(); 

	SelectLanguage();	
// 
	MakeListView(0); 	
	SetListData(0);
//	



	if(pView->GetDocument()->m_bIsEngineerMode==FALSE)
	{
		m_ctrlLog  .EnableWindow(FALSE); 
		m_ctrlClear.EnableWindow(FALSE);
	}
	else
	{
		m_ctrlLog  .EnableWindow(TRUE);
		m_ctrlClear.EnableWindow(TRUE);
	}

	GetDlgItem(IDC_CHECK_LOG)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );
	GetDlgItem(IDC_BTN_CLEAR)->SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER ); //20180830 jeongyong - LOG 삭제기능 제거.


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CErrList::MakeListView(int nList)
{
	CRect rect;
	GetClientRect(&rect);

	// 컬럼에 대한 정보를 저장하는 구조체
	LV_COLUMN lvcolumn;
	// 설정할 항목들을 명시함
	lvcolumn.mask = LVCF_FMT|LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH;
	// 컬럼 헤더를 중앙에 출력하도록 함
	lvcolumn.fmt = LVCFMT_CENTER;

	m_ctrlErrList.DeleteColumn(4);
	m_ctrlErrList.DeleteColumn(3);
	m_ctrlErrList.DeleteColumn(2);
	m_ctrlErrList.DeleteColumn(1);
	m_ctrlErrList.DeleteColumn(0);
	

	if(m_bPreviousErrList == TRUE && nList == 0) // error report
	{
		char *listcolumn[4]={"No.","Code","Time","Contents"};
		int width[4]={45,70,150,400};
		for (int i=0;i<4;i++)
		{
			lvcolumn.fmt = LVCFMT_LEFT;		
			lvcolumn.cx = width[i];	
			lvcolumn.iSubItem = i;
			lvcolumn.pszText = listcolumn[i];
			m_ctrlErrList.InsertColumn(i, &lvcolumn);
		}

		// 목록으로 보기
		m_ctrlErrList.ModifyStyle(LVS_TYPEMASK,LVS_REPORT);
	}
	//090711 이전 로그 확인 위해 추가 	////////////////////090711 Errlist Manual/Auto 구분 적용
	else if(m_bPreviousErrList == FALSE && nList == 0) // error report
	{
		char *listcolumn[5]={"No.","Code","Status","Time","Contents"};
		int width[5]={45,70,65,150,400};
		for (int i=0;i<5;i++)
		{
			lvcolumn.fmt = LVCFMT_LEFT;		
			lvcolumn.cx = width[i];	
			lvcolumn.iSubItem = i;
			lvcolumn.pszText = listcolumn[i];
			m_ctrlErrList.InsertColumn(i, &lvcolumn);
		}
		// 목록으로 보기
		m_ctrlErrList.ModifyStyle(LVS_TYPEMASK,LVS_REPORT);
	}
	else // Log
	{
		char *listcolumn[2]={"Time", "Contents"};
		int width[2]={150, 400};
		for (int i=0;i<2;i++)
		{
			lvcolumn.fmt = LVCFMT_LEFT;		
			lvcolumn.cx = width[i];	
			lvcolumn.iSubItem = i;
			lvcolumn.pszText = listcolumn[i];
			m_ctrlErrList.InsertColumn(i, &lvcolumn);
		}
		// 목록으로 보기
		m_ctrlErrList.ModifyStyle(LVS_TYPEMASK,LVS_REPORT);
	}
}

void CErrList::SetListData(int nList)
{
	LV_ITEM lvitem;
	lvitem.iItem = 0;
	lvitem.mask = LVIF_TEXT;
	int Idx=0,nCode=0,nCode2=0,m_nMonth,m_nDay,m_nHour,m_nMinite,m_nSecond;
	CString str, strDir;

	//리스트뷰에 있는 모든 항목을 지운다
	m_ctrlErrList.DeleteAllItems();

	if(nList == 0) // Error Item Display
	{
		FILE *fp;
		
		strDir = "D:\\TOP\\P8CA_LC\\P8CA_LC_EXE\\";
		
		if(m_bPreviousErrList==TRUE)
			fp = fopen(strDir+"EList.txt","r");
		else
			fp = fopen(strDir+"EList_New.txt","r");	////////////////////090711 Errlist Manual/Auto 구분 적용

//		fp = fopen("EList.txt","r");

		if( fp == NULL && m_bPreviousErrList==TRUE )
		{			
			AfxMessageBox("EList.txt 가 존재하지 않으므로 표시할 수 없습니다");
			return;		
		}
		////////////////////090711 Errlist Manual/Auto 구분 적용
		else if(fp == NULL && m_bPreviousErrList==FALSE) 
		{			
			AfxMessageBox("EList_New.txt 가 존재하지 않으므로 표시할 수 없습니다");		
			return;		
		}
		////////////////////////////////////////////////////////////
		if(m_bPreviousErrList == TRUE)
		{
			while(fscanf(fp,"%d	%d	%d	%d	%d	%d	%d	\n",/**/&nCode,&nCode2,&m_nMonth,&m_nDay,&m_nHour,&m_nMinite,&m_nSecond) != EOF)
			{
				lvitem.iItem = 0;
				str.Format("%d",1);
				m_ctrlErrList.InsertItem(lvitem.iItem, str);
				str.Format("%d",nCode);
				m_ctrlErrList.SetItemText(lvitem.iItem, 1, str);
				str.Format("%02d/%02d %02d:%02d:%02d",m_nMonth,m_nDay,m_nHour,m_nMinite,m_nSecond);
				m_ctrlErrList.SetItemText(lvitem.iItem, 2, str);
				//
				str = InsertContents(nCode,nCode2);
				m_ctrlErrList.SetItemText(lvitem.iItem, 3, str);

				if(m_ctrlErrList.GetItemCount() > 100)
					m_ctrlErrList.DeleteItem(100);
			}
			fclose(fp);

			for(int i=0 ; i<m_ctrlErrList.GetItemCount() ; i++)
			{
				str.Format("%d", i+1);
				m_ctrlErrList.SetItemText(i, 0, str);
			}
		}
		////////////////////090711 Errlist Manual/Auto 구분 적용
		else 
		{
			
			while(fscanf(fp,"%d	%d	%d  %d	%d	%d	%d	%d	\n",/**/&nCode,&nCode2,&m_nTempFlag,&m_nMonth,&m_nDay,&m_nHour,&m_nMinite,&m_nSecond) != EOF)
			{
				lvitem.iItem = 0;
				str.Format("%d",1);
				m_ctrlErrList.InsertItem(lvitem.iItem, str);
				str.Format("%d",nCode);
				m_ctrlErrList.SetItemText(lvitem.iItem, 1, str);
				str.Format("%d",m_nTempFlag); 
				m_ctrlErrList.SetItemText(lvitem.iItem, 2, str);
				str.Format("%02d/%02d %02d:%02d:%02d",m_nMonth,m_nDay,m_nHour,m_nMinite,m_nSecond);
				m_ctrlErrList.SetItemText(lvitem.iItem, 3, str);
				//
				str = InsertContents(nCode,nCode2);
				m_ctrlErrList.SetItemText(lvitem.iItem, 4, str);

				if(m_ctrlErrList.GetItemCount() > 100)
					m_ctrlErrList.DeleteItem(100);
			}
			fclose(fp);

			for(int i=0 ; i<m_ctrlErrList.GetItemCount() ; i++)
			{
				str.Format("%d", i+1);
				m_ctrlErrList.SetItemText(i, 0, str);
			}
		}
	}
	else // Log File Display  :: 현찬아...FTP 이용시 본 내용을 참조하여라...
	{
		CString strPathName, strTime, strMessage;
		ifstream fp;
		strPathName = "D:\\LOG\\"+m_strFileName;
		fp.open((char *)(LPCSTR)strPathName, ios::in);

		// 화일 읽기
		if(!fp.is_open())
		{
			AfxMessageBox("File Open Fail");
			return;
		}

		char ch[512];
		while(!fp.eof())
		{
			m_ctrlDate.SetCaption(m_strFileName);
			fp.getline(ch, 512, '\n');
			str.Format("%s", ch);
			if(str.GetLength() != 0)
			{
				lvitem.iItem = 0;

				strTime = str.Mid(0, 12);
				m_ctrlErrList.InsertItem(lvitem.iItem, strTime);

				strMessage = str.Mid(15, str.GetLength());
				m_ctrlErrList.SetItemText(lvitem.iItem, 1, strMessage);
//				Idx++;
			}
		}
		fp.close();
	}
}

CString CErrList::InsertContents(int nCode, int nCode2)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	CString str;
	CString strTemp1="",	strTemp2="";
	int i=0;
	int iBoardNum=0,	iAxisNum=0;
	WORD wTempValue=0;
	int RefHead = -1;


	if(pView->m_nLanguage == 0)
	{//ehji 140309 한국어.. 간결하게 할 수 있는 좋은 방법 모색중...



		switch(nCode)
		{
				case 1 : str = "PLCAddr.DAT 파일읽기를 실패하였습니다.";
					break;
				case 2 : str = "HeadSPositionOffset.DAT 파일읽기를 실패하였습니다.";
					break;
				case 3 : str = "Comm Port가 정상적으로 Open되지 않았습니다.";
					break;
				case 4 : str = "Motion BD가 정상적으로 Open되지 않았습니다.";
					break;
				case 5 : str = "Count BD가 정상적으로 Open되지 않았습니다.";
					break;
				case 6 : str = "장비가 OffLine 상태입니다.";
					break;
		//
				case 10 :
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + " : 자동운전 중 토출량 측정이 설정치를 초과하여 판정값을 바꿉니다.";
					break;

				case 11 : str.Format("%d번 저울의 문이 열리지 않습니다.",nCode2); //nCode2: BAL 번호
					break;
				case 12 : str.Format("%d번 저울의 문이 닫히지 않습니다.",nCode2);
					break;
				case 13 : str.Format("%d번 저울에 측정cup이 없습니다.",nCode2);;
					break;
				case 14 : str.Format("%d번 측정cup에 LC양이 너무 많습니다.",nCode2);
					break;
				case 15 :
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + " : 자동운전 중 토출량 조정을 실패하였습니다.";
					break;
				case 16 : 
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + " : 자동운전 중 토출량 측정이 설정치를 초과하였습니다.";
					break;
				case 17 :
		// LC 잔량 감지 센서에 의한 Error 발생 ....
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d,",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2+" BOTTLE의 LC 용액 양이 부족합니다.(LC 용액 잔량 감지 센서 확인)";
					break;
				case 18 : 
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d,",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2+" BOTTLE의 LC 용액 양이 부족합니다.(설정치 확인)";
					break;
				case 19 :
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + "Head의 Drop Count 확인하세요.";
					break;
				//
				case 20 : 
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + "Drop 방울 감지 센서에 맺힘이 감지 되었습니다.";			
					break;
				case 21 : str = "Loading전 Stage위에 Glass가 감지 또는 Glass Exist bit 가 ON 되었습니다. 확인하시오.";
					break;
				case 22 : str = "Unloading전 Stage위에 Glass가 감지 되지 않았습니다. 확인하시오.";
					break;
				case 23 : str = "Dropping전 Stage위에 Glass가 감지 되지 않았거나 Glass Exist bit가 Off되었습니다. 확인하시오.";
					break;
				case 24 : str = "Dropping이 완료된 Glass에 다시 Drop 명령이 내려졌습니다. 확인하시오.";
					break;
				case 25 : str = "Dropping이 완료되지 않은  Glass를 Unloading합니다. 확인하시오.";
					break;
				case 26 : 
					str.Format("%d Panel",nCode2);
					str += "의 Add/Del 정보가 설정치를 초과 하였습니다. 정보 변경 후 다시 시작하세요."; 
					break;
				case 27 : str = "Glass의 Recipe와 장치에 설정된 Recipe 번호가 같지 않습니다.";
					break;
				case 28 : str = "Glass의 정보가 없습니다.";
					break;
				case 29 : str = "ADD/DEL 정보를 담은 File을 Download 할 수 없습니다.";
					break;
				case 30 : str = "ADD/DEL 정보가 올바르지 않아 Default 값으로 처리하였습니다.";
					break;
				case 31 : 
					str.Format("%d Head",nCode2);
					str +="Calibration 기준점을 잡는데 실패하였습니다.";
					break;
				case 32 :
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + "Total Drop Counting이 최소 Drop Counting 설정치 미만입니다.";				
					break;
				case 33 :
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + "Head의 Barcode값이 일치하지 않습니다. 확인하세요.";				
					break;
				case 34 :
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + " 계산된 Drop수와 설정한 Drop수가 일치하지 않습니다. 확인하세요.";
					break;

				case 40:
					str.Format("H%d N_Controller Connection Alarm...!!.",nCode2);
					break;
				case 41:
					str.Format("H%d N_Parameter Writting Alarm....!!.",nCode2);
					break;
				case 42 :
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + "N-Type, Nozzle Status is 'NG', Please Check N-nozzle";
					break;

				// util 관련
				case 50 : str = "CDA 압력부족으로 GLASS PURGE가 불량합니다.";
					break;
				case 51 : str = "CDA 압력부족으로 IONIZER BLOW가 불량합니다.";
					break;
				case 52 : str = "Vacuum 압력부족으로 GLASS흡착이 불량합니다.";
					break;
				case 53 :
					if(nCode2==1)		str = "IONIZER1이 Stop 상태 입니다, 상태를 확인 해 주세요 .";
					else if(nCode2==2)	str = "IONIZER1에서 생성되는 ION LEVEL이 불량합니다.";
					else if(nCode2==3)	str = "IONIZER1 TIP 상태가 불량합니다.";
					else if(nCode2==4)	str = "IONIZER2이 Stop 상태 입니다, 상태를 확인 해 주세요.";
					else if(nCode2==5)	str = "IONIZER2에서 생성되는 ION LEVEL이 불량합니다.";
					else if(nCode2==6)	str = "IONIZER2 TIP 상태가 불량합니다.";		
				case 59 : // by ckh 2009.02.13
					str = "피스톤 공급 압력이 부족합니다. 공급 압력을 확인 하세요.";
					break;	
				case 60 : // by ckh 2009.02.16
					str = "Stage에 Glass가 흡착 상태입니다. 흡착 Off 후 실행바랍니다.";
					break;
				case 61 : // by ckh 2009.02.16
					str = "Stage 퍼지 상태입니다. 퍼지 Off 후 실행바랍니다.";	
					break;
				case 62 : // by ckh 2009.02.16
					str = "Stage 퍼지 및 흡착 센서 이상 입니다. Stage 공압 센서 확인 후 실행 바랍니다.";	
					break;
				// Alarm
				case 100 : str = "비상정지";
					if(nCode2==1)		str = "비상정지 : EMO 1 눌림.";
					else if(nCode2==2)	str = "비상정지 : EMO 2 눌림.";
					else if(nCode2==3)	str = "비상정지 : EMO 3 눌림.";
					else if(nCode2==4)	str = "비상정지 : EMO 4 눌림.";
					else if(nCode2==5)	str = "비상정지 : EMO 5 눌림.";
					else if(nCode2==6)	str = "비상정지 : EMO 6 눌림.";		
					else if(nCode2==7)	str = "비상정지 : EMO 7 눌림.";		
					else if(nCode2==8)	str = "비상정지 : AREA 1 감지됨.";
					else if(nCode2==9)	str = "비상정지 : AREA 2 감지됨.";		
					else if(nCode2==10)	str = "비상정지 : AREA 3 감지됨.";	
					break;
				case 101 : str = "장비 운행 도중 안전 커버가 열렸습니다. ";
					if(nCode2==1)		str = "장비 운행 도중 안전 커버가 열렸습니다. : DOOR 1";
					else if(nCode2==2)	str = "장비 운행 도중 안전 커버가 열렸습니다. : DOOR 2";
					else if(nCode2==3)	str = "장비 운행 도중 안전 커버가 열렸습니다. : DOOR 3";
					else if(nCode2==4)	str = "장비 운행 도중 안전 커버가 열렸습니다. : MAINT DOOR";
					break;
				case 102 : str = "장비 운행 도중 Area Sensor가 감지되었습니다. ";
					break;
				case 103 : str = "모든축의 원점 감지가 되지 않았습니다. 원점 감지를 다시 하세요.";
					break;
				case 104 : str = "작업전이나 도중 Robot arm 센서가 감지 되었습니다. 확인하세요.";  // 안전상의 문제로 arm을 확인하고 작업함. 2003. 01. 10.
					break;
				case 105 : str = "S축이 정위치로 가지 못했습니다.";
					break;
				case 106 : str = "Y축이 정위치로 가지 못했습니다.";
					break;
				case 107 : str = "K축이 정위치로 가지 못했습니다.";
					break;
				case 108 : // by ckh 2009.02.13 반입위치값이 잘못되었을 경우.
					str = "K축의 반입 위치값이 잘못되었습니다. 티칭값을 수정하세요.";
					break;
				case 109 : // by ckh 2009.02.13 공급위치값이 잘못되었을 경우.
					str = "K축의 공급 위치값이 잘못되었습니다. 티칭값을 수정하세요.";
					break;	
				case 110 : str = "토출 작업 도중 S축 또는 K축 Limit 감지가 되었습니다. 확인하여 원점 복귀 작업 후 다시 시작하세요.";
					break;
				case 111 : str = "S축 동기I/O 해제를 실패하였습니다";
					break;
				case 112 : str = "Count Board가 정상적으로 동작하지 않습니다.";
					break;
				case 113 : // by ckh 2009.02.13 공급위치값이 잘못되었을 경우.
					str = "K축의 측정 위치값이 잘못되었습니다. 티칭값을 수정하세요.";
					break;
				case 115 : 
					iBoardNum = nCode2/0x10;
					iAxisNum  = nCode2%0x10;
					str.Format("%d Board의 %d Axis이 Servo Alarm 입니다. 전원을 해지하거나 비상 정지 버튼을 누른 후 다시 시작하십시오.",iBoardNum, iAxisNum);
					break;
				case 116 : 
					str = "K축의 반입위치 센서 ON이 되지 않았습니다. 위치 및 센서 상태 확인하세요.";
					break;
				case 117 :
					str = "K1, K2 축의 엔코더 값 차이가 큽니다";
					break;
				case 120 : 
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + " : MEASURING 동작 비정상 완료.";
					break;
				case 121 : 
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + " : ADJUSTING동작 비정상 완료.";
					break;
				case 122 : str = "토출모터 위치 초기화 실패. PC를 재부팅해 주세요";
					break;
				case 123 : 
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + " : Specout counting 값이 설정치를 초과.";
					break;
				case 130: str.Format("H%d Pump P축 (-)기준 위치가 아닙니다.", nCode2);
					break;
				case 131: str.Format("H%d Pump P축 (+)기준 위치가 아닙니다.", nCode2);	// by shin, 2006.10.21 Volume축 정위치 에러 구문 추가
					break;
				case 135: str.Format("맺힘감지 Up/Down 실린더 상태가 이상합니다...");
					break;
				case 136: str.Format("H%d 피스톤 실린더 센서 상태가 이상합니다...", nCode2);	// // by shin, 2007.01.03 
					break;	
				case 137: str.Format("H%d Joint Step1을 완료하지 않았습니다...", nCode2);	// by shin, 2007.01.03 
					break;	
				case 138: str.Format("H%d Joint Step2 작업 중 장착 센서가 작동 되지 않았습니다...", nCode2);	// // by shin, 2007.01.03 
					break;	
				case 139: str.Format("H%d Pump P축의 Limit Sensor가 감지 되었습니다...", nCode2);	// // by shin, 2007.01.03 
					break;	
				case 140: str.Format("H%d Pump 용적량을 초과하는 LC양이 계산 되었습니ㅏ...", nCode2);
					break;
				case 141: str.Format("H%d Gate G축 Open 위치가 아닙니다.", nCode2);	// by shin, 2007.01.03 
					break;
				case 142: str.Format("H%d Gate G축 Close 위치가 아닙니다.", nCode2);	// by shin, 2007.01.03 
					break;
				case 143: str.Format("H%d Pump 용적 거리를 초과하는 LC양이 계산되었습니다...", nCode2);
					break;		
				case 144: str.Format("H%d Pump P축 장착 위치가 아닙니다.", nCode2);	// by shson, 2006.10.11 Volume축 정위치 에러 구문 추가
					break;
				case 145: str.Format("H%d Pump P축 정위치 Error 발생.", nCode2);	// by shson, 2006.10.11 Volume축 정위치 에러 구문 추가
					break;
				case 146: str.Format("%d번 공급 장치의 주입구의 상태가 이상합니다...",nCode2);
					break;
				case 147: str.Format("%d번 공급 장치의 Pinch 밸브의 상태가 이상합니다....",nCode2);
					break;
				case 148: str.Format("%d번 공급 장치의 CDA 상태가 이상합니다. ...",nCode2);
					break;
				case 149: str.Format("%d번 액정 공급 장치 상태를 체크하십시요...",nCode2);
					break;
				case 150: str.Format("Safety Interlock이 해제 되었습니다...");
					break;
		//2010.08.18 by tskim
		//		case 151: str.Format("H%d LC 공급 상한 센서가 감지 되었습니다...", nCode2);	
		//			break;
				case 152: str.Format("%d번 Canister의 초기 무게 측정 실패...",nCode2);
					break;
				case 153: str.Format("%d번 Canister의 액정이 비었습니다..",nCode2);
					break;
				case 154: str.Format("%d번 Canister의 액정 경고량에 도달했습니다...",nCode2);
					break;
				case 155 : str = "토출 작업 도중 Line의 X Position 계산값이 이상합니다. 확인하여 원점 복귀 작업 후 다시 시작하세요.";
					break;
				case 156 : str = "토출 작업 도중 Line의 Y Position 계산값이 이상합니다. 확인하여 원점 복귀 작업 후 다시 시작하세요.";
					break;
				case 157 : str = "토출 작업 도중 Line의 X Position 이상 발생! 확인하여 원점 복귀 작업 후 다시 시작하세요.";
					break;
				case 158 : str = "토출 작업 도중 Line의 Y Position 이상 발생! 확인하여 원점 복귀 작업 후 다시 시작하세요.";
					break;
		//2011.01.26 by tskim
				case 159: str.Format("%d번 Canister의 액정 공급량 Limit 이하 알람!!...",nCode2);
					break;
				
				case 188: str = "타겟량과 측정량이 맞지 않습니다. 조정 완료 후 진행하세요..";
					break;

		//반출 대기 시간 1000초 경과 할 경우 알람 발생...
				case 189:
					strTemp1 = "",strTemp2 = "";
					str = "반출 대기 시간 1000초 초과 하였습니다.";
					break;


		////////////////////////// 여기서부터는 Warning으로 보고되는 Error 항목입니다. ///////////////////// 
				case 200 : str = "운행전 간섭외 신호가 끊어 졌읍니다. PLC의 신호를 확인한 후 다시 시작하세요..";
					break;
				case 201 : str = "Transfer배출 동작 중이므로 진행할 수 없습니다..";
					break;
				case 202 : str = "Transfer수취 동작 중이므로 진행할 수 없습니다..";
					break;
				case 203 : str = "Balance Mode가 정상 Mode로 전환이 되었습니다. Balance의 상태가 초기화 되었는지 확인하세요.";
					break;
				case 204:// by ckh 2009.02.14 알람 1회만 하기위해 ,
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 +(" Dummy Cup 1에 LC양이 너무 많습니다.");	
					break;
				case 205: 
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 +(" Dummy Cup 2에 LC양이 너무 많습니다.");	
					break;
				case 206 : str = "INTERFACE중 NACK CODE로 응답이 왔습니다.";	
					break;
				case 207 : str = "INTERFACE중 응답시간이 초과하였습니다.";	
					break;
				case 208 : str = "ECS로부터의 ADD/DEL 정보 이상, Panel 정보를 확인하세요...";	
					break;
				case 209 : str = "Column K축 모터 주행 거리 초과. 설정값 조정 및 구리스를 주입하여 주세요...";	
					break;
				case 210 : str = "Head S축 모터 주행 거리 초과. 설정값 조정 및 구리스를 주입하여 주세요...";	
					break;
				case 211 : str = "Pair Glass의 Seal 도포 완료 신호가 없습니다.확인 하세요.";		
					break;
				case 212 : str = "[ECS]PD DOWN REQUEST OFF SIGNAL TIMEOVER. CHECK REQUEST BIT"; //141026 PD DOWN : ECS REQUEST BIT 가 OFF되지 않은면 해당 알람이 떠야됨. 
					break; 
				case 300: str = "Panel 정보 취득 실패, Panel 정보를 확인하세요...";	
					break;
				case 301: str = "발란스 내부에 측정 컵이 감지 되었습니다.!!";	
					break;
				case 302: str = "Mark Align 정도 데이터 획득 실패!!";
					break;
				case 303: str = "Mark#1 Search 실패!!";
					break;
				case 304: str = "Mark#2 Search 실패!!";
					break;
				case 305: //2010.06.08 by tskim LC 유의차 (Canister 1~2)
					strTemp1="",strTemp2="";
					//for(i=0; i<MAX_NOZZLE/2 ; i++)
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
							RefHead = i;//2010.06.07 by tskim LC Diff차이 설정치 Display를 위해.
						}
					}
					strTemp1.Empty();
					if(RefHead<MAX_NOZZLE/2)
						strTemp1.Format("LC양 유의차가 Max Head %d (설정치%.0f g)를 초과 하였습니다.",pView->m_nHeadLcMax1+1,pDoc->m_dLCAmountDiff[RefHead/(MAX_NOZZLE/2)]);
					else
						strTemp1.Format("LC양 유의차가 Max Head %d (설정치%.0f g)를 초과 하였습니다.",pView->m_nHeadLcMax2+1,pDoc->m_dLCAmountDiff[RefHead/(MAX_NOZZLE/2)]);
					//m_strMsg2 = strTemp2 +("LC양이 유의차(설정치)를 초과 하였습니다.");
					str = strTemp2 +strTemp1;
					break;
				case 306: //2010.07.05 by tskim 이전 중공 헤드에 재 중공 지령 방지
					str.Format("직전 중앙 공급 헤드와 중복 됩니다. Head%d",nCode2);	
					break;
				case 307 : 
		// LC 하한 감지 센서에 의한 Error 발생 ....
					str="",strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d/",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2+" BOTTLE의 LC 용액 양이 부족합니다.(LC 용액 하한 감지 센서 확인)";
					break;
				case 308 : 
					str="",strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + "동일 판넬 사용 Head의 Lot No 불일치.. 확인하세요.";			
					break;
				case 309 : 
					str="",strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + "Canister Head간 Lot No 불일치.. 확인하세요.";			
					break;
				case 310: 
					str="",strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + "LINE Drop 작업 중 맺힘이 감지 되었습니다.";
					break; 

				case 311 : str = "VALC Auto Mode가 아닙니다. 확인하세요.";		
					break;
				case 320 : str = "FAN이 정지 상태입니다. 확인하세요.";		
					break;
		//2011.04.27 by tskim
				case 400 : str.Format("%d번 지진 모드 Retry Count 초과!!.",nCode2);
					break;
				case 401: str.Format("Vision Communication Fail.");
					break;
				case 402: str.Format("Vision Writing Fail 'CSV File.' ");
					break;
				case 403: str.Format("Time Over of Reading Vision Count Result");
					break;
				case 404: str.Format("File Reading Fail");
					break;
				case 405: str.Format("There is no File");
					break;
				case 406: str.Format("Vision Responds NG");
					break;
				case 407: str.Format("Cell No is Dismatched");
					break;
				case 408: str.Format("Cell Sequence is wrong");
					break;
				case 409: str.Format("Vision Drop Count Result Error");
					break;
				case 410: str.Format("Vision Send to Dispenser 'ERROR'");
					break;
				case 411: str.Format("Response TimeOver!");
					break;
				case 412: str.Format("Sending Title & Receiving Title is different");
					break;
				case 413: str.Format("Vision Communication Error!");
					break;
				//140806
				case 414: str.Format("Dummy Pattern Drop Detected");
					break;
//220603 BIG DROP DETECT ( (OLD) DEEP DROP ) 
				case 415: str.Format("Big Drop Detected ( Cell )");
					break;
//220603 BIG DROP DETECT ( (OLD) DEEP DROP )
				case 500 : str.Format("H%d N DRIVER OVEROAD ALARM", nCode2);
					break; 

				case 600 :			//ehji 150527
						str="",strTemp1="",strTemp2="";
						for(i=0; i<MAX_NOZZLE ; i++)
						{
							wTempValue = 0x0001;
							wTempValue = wTempValue << i;
							if(wTempValue&nCode2){
								strTemp1.Format("%d, ",i+1);
								strTemp2+=strTemp1;
							}
						}
						str = strTemp2 + "DistPerDrop 평균값 초과 !!";
						break;
	//140926
				case 700 : str.Format("Head1 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
					break;
				case 701 : str.Format("Head2 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
					break;
				case 702 : str.Format("Head3 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
					break;
				case 703 : str.Format("Head4 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
					break;
				case 704 : str.Format("Head5 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
					break;
				case 705 : str.Format("Head6 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
					break;
				case 706 : str.Format("Head7 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
					break;
				case 707 : str.Format("Head8 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
					break;
				case 708 : str.Format("Head9 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
					break;
				case 709 : str.Format("Head10 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
					break;
				case 710 : str.Format("Head11 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
					break;
				case 711 : str.Format("Head12 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
					break;
				case 712 : str.Format("Head13 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
					break;
				case 713 : str.Format("Head14 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
					break;
				case 714 : str.Format("Head15 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
					break;
				case 715 : str.Format("Head16 Drop 방울 감지 센서에 맺힘이 감지 되었습니다.");
					break;
//2015.04.29 by tskim 원재료 전산화Add Dummy Drop 보고 
		                case 820:
			                   str.Format("From ECS Dummy Drop 미실시 & Dispesner Dummy 미 실시 Alarm!");
			                break;
				//2014.12.01 by tskim Amp, Duty 연산 관련 오류 Error
				case 800:case 801:case 802:case 803:case 804:case 805:case 806:case 807:
				case 808:case 809:case 810:case 811:case 812:case 813:case 814:case 815: 
					str.Format("Head%d Amplitude, Duty 값이 0 입니다..!",(nCode2-800)+1);
					break;


				case 7001:case 7002:case 7003:case 7004:case 7005:case 7006:case 7007:case 7008:
				case 7009:case 7010:case 7011:case 7012:case 7013:case 7014:case 7015:case 7016:
					str.Format("H%d LC 공급 상한 센서가 감지 되었습니다...", nCode2);
					break;
				default: str = "존재하지 않는 오류 번호 입니다.!!";
					break;
		}		 
	}	//ehji 140309 한국어끝

	else if(pView->m_nLanguage == 1)
	{//ehji 140309 영어.. 간결하게 할 수 있는 좋은 방법 모색중...

		switch(nCode)
		{
				case 1 : str = "PLCAddr.DAT file not founded.";
					break;
				case 2 : str = "HeadSPositionOffset.DAT file not founded.";
					break;
				case 3 : str = "Comm Port is not open normaly.";
					break;
				case 4 : str = "Motion BD is not open normaly.";
					break;
				case 5 : str = "Count BD is not open normaly.";
					break;
				case 6 : str = "Machine state Offline.";
					break;
		//
				case 10 :
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + " : During Auto operation, discharge measurement value was exceeded. So set value is replaced.";
					break;

				case 11 : str.Format("Nomber %d of Scales Can't Open.",nCode2); //nCode2: BAL 번호
					break;
				case 12 : str.Format("Nomber %d of Scales Can't Close.",nCode2);
					break;
				case 13 : str.Format("%d measurement cup is not in Scales.",nCode2);;
					break;
				case 14 : str.Format("%d volume of measure cup LC Solution is too much",nCode2);
					break;
				case 15 :
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + " : During Auto operation, discharge measurement exceeds the set value.";
					break;
				case 16 : 
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + " : During Auto operation, discharge measurement exceeds the set value.";
					break;
				case 17 :
		// LC 잔량 감지 센서에 의한 Error 발생 ....
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d,",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2+" LC of Bottle is lack Please check LC Level detection Sensor ";
					break;
				case 18 : 
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d,",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2+" LC Amount of BOTTLE lack (To confirm setting value)";
					break;
				case 19 :
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + "Please check drop counter of head.";
					break;
				//
				case 20 : 
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + "Before Drop, Dropping detected.";			
					break;
				case 21 : str = "Loading ago, Glass detection on the stage or Glass Exist bit is ON.";
					break;
				case 22 : str = "Before unloading, The glass be not checked on the stage .";
					break;
				case 23 : str = "Before dropping, The glass be not checked on the stage.";
					break;
				case 24 : str = "Glass of Drop Complete redropped. Please Check.";
					break;
				case 25 : str = "Not Dropping Glass is into Unloading step. Please Check.";
					break;
				case 26 : 
					str.Format("%d Panel",nCode2);
					str += "Add_Del of Panel is abnormal."; 
					break;
				case 27 : str = "Glass Type in Work Data & Glass Type in Recipe are different. Please check up.";
					break;
				case 28 : str = "Glass impormation don't exist.";
					break;
				case 29 : str = "it can't download File having ADD_DEL impormation ";
					break;
				case 30 : str = "ADD_DEL impormation was incorect. So it change Default Value.";
					break;
				case 31 : 
					str.Format("%d Head",nCode2);
					str +="it Failed to Calibration standard point.";
					break;
				case 32 :
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + "Total Drop Counting is low to minimum Drop Counting set value.";				
					break;
				case 33 :
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + "Barcode of Head do not match, Please Check.";				
					break;
				case 34 :
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + " Calculated Drop count and Setting Drop count is different, Please Check.";
					break;

				case 40:
					str.Format("H%d N_Controller Connection Alarm...!!.",nCode2);
					break;
				case 41:
					str.Format("H%d N_Parameter Writting Alarm....!!.",nCode2);
					break;
				case 42 :
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + "N-Type, Nozzle Status is 'NG', Please Check N-nozzle";
					break;

				// util 관련
				case 50 : str = "Due to CDA Lack of pressure, GLASS PURGE is bad.";
					break;
				case 51 : str = "Due to CDA Lack of pressure, IONIZER BLOW is bad.";
					break;
				case 52 : str = "Due to Vacuum Lack of pressure, GLASS absorption is bad.";
					break;
				case 53 :
					if(nCode2==1)		str = "IONIZER1 Status is Stop, Status Check Please.";
					else if(nCode2==2)	str = "IONIZER1's ION LEVEL is bad.";
					else if(nCode2==3)	str = "IONIZER1's TIP Condition is bad.";
					else if(nCode2==4)	str = "IONIZER2 Status is Stop, Status Check Please.";
					else if(nCode2==5)	str = "IONIZER2's ION LEVEL is bad.";
					else if(nCode2==6)	str = "IONIZER2's TIP Condition is bad.";		
				case 59 : // by ckh 2009.02.13
					str = "Pistion pressure is low. Please check.";
					break;	
				case 60 : // by ckh 2009.02.16
					str = "Glass state on stage is absorption, after absorption off, please operate.";
					break;
				case 61 : // by ckh 2009.02.16
					str = "Stage fuzzy state. After fuzzy off, Run please.";	
					break;
				case 62 : // by ckh 2009.02.16
					str = "Stage fuzzy or absorption sensor is abnormal. After checking Stage pneumatic sensor, please operate.";	
					break;
				// Alarm
				case 100 : str = "Emergency stop";
					if(nCode2==1)		str = "Emergency stop : EMO 1 is pressed.";
					else if(nCode2==2)	str = "Emergency stop : EMO 2 is pressed.";
					else if(nCode2==3)	str = "Emergency stop : EMO 3 is pressed.";
					else if(nCode2==4)	str = "Emergency stop : EMO 4 is pressed.";
					else if(nCode2==5)	str = "Emergency stop : EMO 5 is pressed.";
					else if(nCode2==6)	str = "Emergency stop : EMO 6 is pressed.";		
					else if(nCode2==7)	str = "Emergency stop : EMO 7 is pressed.";	
					else if(nCode2==8)	str = "Emergency stop : AREA 1 is detected.";
					else if(nCode2==9)	str = "Emergency stop : AREA 2 is detected.";		
					else if(nCode2==10)	str = "Emergency stop : AREA 3 is detected.";	
					break;
				case 101 : str = "During Equipment operating, Safety cover is opened.";
					if(nCode2==1)		str = "During Equipment operating, Safety cover is opened. : DOOR 1";
					else if(nCode2==2)	str = "During Equipment operating, Safety cover is opened. : DOOR 2";
					else if(nCode2==3)	str = "During Equipment operating, Safety cover is opened. : DOOR 3";
					else if(nCode2==4)	str = "During Equipment operating, Safety cover is opened. : MAINT DOOR";
					break;
				case 102 : str = "During Equipment operating, Area sensor detect alarm.";
					break;
				case 103 : str = "the origin detection of all axis was not. Please restart origin detection.";
					break;
				case 104 : str = "During operation or to be operation, Robot arm sensor is detected. Please check.";  // 안전상의 문제로 arm을 확인하고 작업함. 2003. 01. 10.
					break;
				case 105 : str = "S Axis don't go in Right position.";
					break;
				case 106 : str = "Y Axis don't go in Right position.";
					break;
				case 107 : str = "K Axis don't go in Right position.";
					break;
				case 108 : // by ckh 2009.02.13 반입위치값이 잘못되었을 경우.
					str = "Import Position value of K Axis is wrong. Please modify teaching value.";
					break;
				case 109 : // by ckh 2009.02.13 공급위치값이 잘못되었을 경우.
					str = "Supply Position value of K Axis is wrong. Please modify teaching value.";
					break;	
				case 110 : str = "During dispensing work,  Limit of K or S is detected. Please Check and Please restart after origin return work.";
					break;
				case 111 : str = "S axis sync I/O remove alarm.";
					break;
				case 112 : str = "Count BD Don't operate normaly.";
					break;
				case 113 : // by ckh 2009.02.13 공급위치값이 잘못되었을 경우.
					str = "Measure position value Of K Axis is wrong. Please modify teaching value.";
					break;
				case 115 : 
					iBoardNum = nCode2/0x10;
					iAxisNum  = nCode2%0x10;
					str.Format("%d Board %d Axis Servo Alarm. Please Power off or after push emergency button, Please restart.",iBoardNum, iAxisNum);
					break;
				case 117 :
					str = "Encorder Value of K1, K2 Axis is different";
					break;
				case 120 : 
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + " : MEASURING action is abnormal.";
					break;
				case 121 : 
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + " : ADJUSTING action is abnormal.";
					break;
				case 122 : str = " Dispensing Motor Position initial is Fail. Please restart PC.";
					break;
				case 123 : 
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + " : Specout counting value exceeds set value.";
					break;
				case 130: str.Format("H%d Pump P Axis - Standard Position is not.", nCode2);
					break;
				case 131: str.Format("H%d Pump P Axis + Standard Position is not.", nCode2);	// by shin, 2006.10.21 Volume축 정위치 에러 구문 추가
					break;
				case 135: str.Format("There is a problem with the Up/Down cylinder sensor.....");
					break;
				case 136: str.Format("H%d There is a problem with the piston cylinder sensor..", nCode2);	// // by shin, 2007.01.03 
					break;	
				case 137: str.Format("H%d Joint Step1 was not completed..", nCode2);	// by shin, 2007.01.03 
					break;	
				case 138: str.Format("H%d During Joint Step2's Operation, mounting sensor do not operate...", nCode2);	// // by shin, 2007.01.03 
					break;	
				case 139: str.Format("H%d Head Pump P limit sensor detected...", nCode2);	// // by shin, 2007.01.03 
					break;	
				case 140: str.Format("H%d LC amount be calculated to exceed Pump volume amount..", nCode2);
					break;
				case 141: str.Format("H%d Gate G Axis Open Position isn't.", nCode2);	// by shin, 2007.01.03 
					break;
				case 142: str.Format("H%d Gate G Axis Close Position isn't.", nCode2);	// by shin, 2007.01.03 
					break;
				case 143: str.Format("H%d LC amount be calculated to exceed Pump volume distance...", nCode2);
					break;		
				case 144: str.Format("H%d Pump P Axis Mounting Position isn't.", nCode2);	// by shson, 2006.10.11 Volume축 정위치 에러 구문 추가
					break;
				case 145: str.Format("H%d Pump P Axis in position Error.", nCode2);	// by shson, 2006.10.11 Volume축 정위치 에러 구문 추가
					break;
				case 146: str.Format("Inlet of Supply device %d is abnormal...",nCode2);
					break;
				case 147: str.Format("pinch valves state of Supply device %d is abnormal....",nCode2);
					break;
				case 148: str.Format("CDA of Supply device %d is abnormal...",nCode2);
					break;
				case 149: str.Format("Please check LC supply divice %d...",nCode2);
					break;
				case 150: str.Format("Safety Interlock was released...");
					break;
		//2010.08.18 by tskim
		//		case 151: str.Format("H%d LC supply maximum sensor was detected...", nCode2);	
		//			break;
				case 152: str.Format("%d Beginning weight measurement of Canister is Fail ...",nCode2);
					break;
				case 153: str.Format("LC of Canister %d empty..",nCode2);
					break;
				case 154: str.Format("LC warning Amount of canister %d..",nCode2);
					break;
				case 155 : str = "During discharge work, X Position Calculation is wrong. Please Check and Please restart after origin return work.";
					break;
				case 156 : str = "During discharge work, Y Position Calculation is wrong. Please Check and Please restart after origin return work.";
					break;
				case 157 : str = "During discharge work, X Position of Line is alarm!! Please Check and Please restart after origin return work.";
					break;
				case 158 : str = "During discharge work, Y Position of Line is alarm!! Please Check and Please restart after origin return work.";
					break;
		//2011.01.26 by tskim
				case 159: str.Format("LC supply amount Of Canister %d Limit Alarm!!...",nCode2);
					break;
				
				case 188: str = "Target amount and measurement different, after complete adjustment, progress ..";
					break;

		//반출 대기 시간 1000초 경과 할 경우 알람 발생...
				case 189:
					strTemp1 = "",strTemp2 = "";
					str = "Export Wait Time exceed 1000sec..";
					break;


		////////////////////////// 여기서부터는 Warning으로 보고되는 Error 항목입니다. ///////////////////// 
				case 200 : str = "Before operation interference signal is lost. After Check PLC signal, please restart..";
					break;
				case 201 : str = "it can't operate, because Transfer relaase operate..";
					break;
				case 202 : str = "it can't operate, because Transfer reception operate..";
					break;
				case 203 : str = " Balance Mode changed to normal Mode. Please Check whether state of balance become initialized  or not.";
					break;
				case 204:// by ckh 2009.02.14 알람 1회만 하기위해 ,
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 +(" LC in Dummy Cup 1 is too much.");	
					break;
				case 205: 
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 +("LC in Dummy Cup 2 is too much.");	
					break;
				case 206 : str = "NACK CODE is ON, when Interfacing.";	
					break;
				case 207 : str = "During interface, response Time was exceeded.";	
					break;
				case 208 : str = "ADD/DEL impormation is abnormal From ECS , Please Check Panel impormation....";	
					break;
				case 209 : str = "Column K Axis motor driving distance is over, Please adjust set value or inject the grease...";	
					break;
				case 210 : str = "Head S Axis motor driving distance is over, Please adjust set value or inject the grease....";	
					break;
				case 211 : str = "Seal embrocation Complete signal of Pair Glass isn't. Please Check.";		
					break;
				case 212 : str = "[ECS]PD DOWN REQUEST OFF SIGNAL TIMEOVER. CHECK REQUEST BIT"; //141026 PD DOWN : ECS REQUEST BIT 가 OFF되지 않은면 해당 알람이 떠야됨. 
					break;
				case 300: str = "Panel Impormation Failed to acquisition. Please Check Panel impormation...";	
					break;
				case 301: str = "Measurement cup in Balance insite was detected..!!";	
					break;
				case 302: str = "To gain Mark Align dgree data is Fail!!";
					break;
				case 303: str = "Mark#1 Search Fail!!";
					break;
				case 304: str = "Mark#2 Search Fail!!";
					break;
				case 305: //2010.06.08 by tskim LC 유의차 (Canister 1~2)
					strTemp1="",strTemp2="";
					//for(i=0; i<MAX_NOZZLE/2 ; i++)
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
							RefHead = i;//2010.06.07 by tskim LC Diff차이 설정치 Display를 위해.
						}
					}
					strTemp1.Empty();
					if(RefHead<MAX_NOZZLE/2)
						strTemp1.Format("LC amount different exceeded Max Head %d(set value).",pView->m_nHeadLcMax1+1,pDoc->m_dLCAmountDiff[RefHead/(MAX_NOZZLE/2)]);
					else
						strTemp1.Format("LC amount different exceeded Max Head %d(set value).",pView->m_nHeadLcMax2+1,pDoc->m_dLCAmountDiff[RefHead/(MAX_NOZZLE/2)]);
					//m_strMsg2 = strTemp2 +("LC양이 유의차(설정치)를 초과 하였습니다.");
					str = strTemp2 +strTemp1;
					break;
				case 306: //2010.07.05 by tskim 이전 중공 헤드에 재 중공 지령 방지
					str.Format("Center Supply Head was Duplicated. Head%d",nCode2);	
					break;
				case 307 : 
		// LC 하한 감지 센서에 의한 Error 발생 ....
					str="",strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d/",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2+"LC of bottle is low. (please check)";
					break;
				case 308 : 
					str="",strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + "Lot No Of Same Panel Using Head  is inconsistency. Please Check.";			
					break;
				case 309 : 
					str="",strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + "Lot No of Canister Head is inconsistency. Please Check.";			
					break;
				case 310: 
					str="",strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + "LINE Drop 작업 중 맺힘이 감지 되었습니다.";
					break; 

				case 311 : str = "VALC Auto Mode is not. Check Please.";		
					break;
				case 320 : str = "FAN STOP. Check Please.";		
					break;
		//2011.04.27 by tskim
				case 400 : str.Format("NO.%d earthquake mode Retry Count excess!!.",nCode2);
					break;
				case 401: str.Format("Vision Communication Fail.");
					break;
				case 402: str.Format("Vision Writing Fail 'CSV File.' ");
					break;
				case 403: str.Format("Time Over of Reading Vision Count Result");
					break;
				case 404: str.Format("File Reading Fail");
					break;
				case 405: str.Format("There is no File");
					break;
				case 406: str.Format("Vision Responds NG");
					break;
				case 407: str.Format("Cell No is Dismatched");
					break;
				case 408: str.Format("Cell Sequence is wrong");
					break;
				case 409: str.Format("Vision Drop Count Result Error");
					break;
				case 410: str.Format("Vision Send to Dispenser 'ERROR'");
					break;
				case 411: str.Format("Response TimeOver!");
					break;
				case 412: str.Format("Sending Title & Receiving Title is different");
					break;
				case 413: str.Format("Vision Communication Error!");
					break;
				case 414: str.Format("Dummy Pattern Drop Detected");  //ehji
					break;
//220603 BIG DROP DETECT ( (OLD) DEEP DROP ) 
				case 415: str.Format("Big Drop Detected ( Cell )");
					break;
//220603 BIG DROP DETECT ( (OLD) DEEP DROP )
				case 500 : str.Format("H%d N DRIVER OVEROAD ALARM", nCode2);
					break;
				case 600 :			//ehji 150527
						str="",strTemp1="",strTemp2="";
						for(i=0; i<MAX_NOZZLE ; i++)
						{
							wTempValue = 0x0001;
							wTempValue = wTempValue << i;
							if(wTempValue&nCode2){
								strTemp1.Format("%d, ",i+1);
								strTemp2+=strTemp1;
							}
						}
						str = strTemp2 + "DistPerDrop Average Over !!";
						break;

			//drop detect alarm
				//140926
				case 700 : str.Format("Head1 Before Drop, Dropping detected.");
					break;
				case 701 : str.Format("Head2 Before Drop, Dropping detected.");
					break;
				case 702 : str.Format("Head3 Before Drop, Dropping detected.");
					break;
				case 703 : str.Format("Head4 Before Drop, Dropping detected.");
					break;
				case 704 : str.Format("Head5 Before Drop, Dropping detected.");
					break;
				case 705 : str.Format("Head6 Before Drop, Dropping detected.");
					break;
				case 706 : str.Format("Head7 Before Drop, Dropping detected.");
					break;
				case 707 : str.Format("Head8 Before Drop, Dropping detected.");
					break;
				case 708 : str.Format("Head9 Before Drop, Dropping detected.");
					break;
				case 709 : str.Format("Head10 Before Drop, Dropping detected.");
					break;
				case 710 : str.Format("Head11 Before Drop, Dropping detected.");
					break;
				case 711 : str.Format("Head12 Before Drop, Dropping detected.");
					break;
				case 712 : str.Format("Head13 Before Drop, Dropping detected.");
					break;
				case 713 : str.Format("Head14 Before Drop, Dropping detected.");
					break;
				case 714 : str.Format("Head15 Before Drop, Dropping detected.");
					break;
				case 715 : str.Format("Head16 Before Drop, Dropping detected.");
					break;

//2015.04.29 by tskim 원재료 전산화Add Dummy Drop 보고 
		                case 820:
			                  str.Format("From ECS Dummy Drop 미실시 & Dispesner Dummy 미 실시 Alarm!");
			                break;					//2014.12.01 by tskim Amp, Duty 연산 관련 오류 Error
				case 800:case 801:case 802:case 803:case 804:case 805:case 806:case 807:
				case 808:case 809:case 810:case 811:case 812:case 813:case 814:case 815: 
				str.Format("Head%d Amplitude, Duty Value is 0..!",(nCode2-800)+1);
					break;


				case 7001:case 7002:case 7003:case 7004:case 7005:case 7006:case 7007:case 7008:
				case 7009:case 7010:case 7011:case 7012:case 7013:case 7014:case 7015:case 7016:
					str.Format(" Head No.%d LC supply maximum Sensor is detected...", nCode2);
					break;
				default: str = "Error Nomber is not exist.!!";
					break;
		}	
	}//ehji 140309 영어 끝 


	
	else if(pView->m_nLanguage == 2)
	{//ehji 140309 중국어.. 간결하게 할 수 있는 좋은 방법 모색중...

		switch(nCode)
		{
				case 1 : str = "PLCAddr.DAT 讀取失敗.";
					break;
				case 2 : str = "HeadSPositionOffset.dat 文件讀取失敗.";
					break;
				case 3 : str = "Comm Port無法正常Open..";
					break;
				case 4 : str = "Motion BD無法正常Open..";
					break;
				case 5 : str = "Count BD無法正常Open.";
					break;
				case 6 : str = "장비가 OffLine 상태입니다.";
					break;
		//
				case 10 :
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + " : 자동운전 중 토출량 측정이 설정치를 초과하여 판정값을 바꿉니다.";
					break;

				case 11 : str.Format("%d번 저울의 문이 열리지 않습니다.",nCode2); //nCode2: BAL 번호
					break;
				case 12 : str.Format("%d번 저울의 문이 닫히지 않습니다.",nCode2);
					break;
				case 13 : str.Format("%d測量儀上無側重cup.",nCode2);;
					break;
				case 14 : str.Format("%d測量cup里LC量太多.",nCode2);
					break;
				case 15 :
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + " : 自動運轉中吐出量調整失敗.";
					break;
				case 16 : 
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + " : 自動運轉中吐出量超過了設定?.";
					break;
				case 17 :
		// LC 잔량 감지 센서에 의한 Error 발생 ....
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d,",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2+" BOTTLE內 LC溶液量不足.(確認LC溶液量感應器)";
					break;
				case 18 : 
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d,",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2+" BOTTLE內 LC溶液量不足.(確認設定?)";
					break;
				case 19 :
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + "確認Head的 Drop Count .";
					break;
				//
				case 20 : 
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + "Drop 水珠感應器被感應.";			
					break;
				case 21 : str = "Loading前Stage上Glass感應或Glass Exist bit ON .";
					break;
				case 22 : str = "Unloading前Stage上 Glass無感應";
					break;
				case 23 : str = "Dropping前 Stage上 Glass無感應.";
					break;
				case 24 : str = "Dropping完成的Glass上再次指令 Drop.";
					break;
				case 25 : str = "Dropping未完成的Glass Unloading.";
					break;
				case 26 : 
					str.Format("%d Panel",nCode2);
					str += " Add/Del 情報異常.."; 
					break;
				case 27 : str = "Glass的Recipe和設備設定的 Recipe編號不同.";
					break;
				case 28 : str = "無Glass情報.";
					break;
				case 29 : str = "ADD/DEL 정보를 담은 File을 Download 할 수 없습니다.";
					break;
				case 30 : str = "ADD/DEL 정보가 올바르지 않아 Default 값으로 처리하였습니다.";
					break;
				case 31 : 
					str.Format("%d Head",nCode2);
					str +="Calibration 기준점을 잡는데 실패하였습니다.";
					break;
				case 32 :
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + "Total Drop Counting이 최소 Drop Counting 설정치 미만입니다.";				
					break;
				case 33 :
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + "Head的 Barcode?不一致請確認.";				
					break;
				case 34 :
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + " 계산된 Drop수와 설정한 Drop수가 일치하지 않습니다. 확인하세요.";
					break;

				case 40:
					str.Format("H%d N_Controller Connection Alarm...!!.",nCode2);
					break;
				case 41:
					str.Format("H%d N_Parameter Writting Alarm....!!.",nCode2);
					break;
				case 42 :
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + "N-Type, Nozzle Status is 'NG', Please Check N-nozzle";
					break;

				// util 관련
				case 50 : str = "CDA 壓力不足導致Glass Purge不良..";
					break;
				case 51 : str = "CDA 壓力不足導致 IONIZER BLOW不良..";
					break;
				case 52 : str = "Vacuum 壓力不足導致 GLASS吸附不良.";
					break;
				case 53 :
					if(nCode2==1)		str = "IONIZER1이 Stop 상태 입니다, 상태를 확인 해 주세요 .";
					else if(nCode2==2)	str = "IONIZER1에서 생성되는 ION LEVEL이 불량합니다.";
					else if(nCode2==3)	str = "IONIZER1 TIP 상태가 불량합니다.";
					else if(nCode2==4)	str = "IONIZER2이 Stop 상태 입니다, 상태를 확인 해 주세요.";
					else if(nCode2==5)	str = "IONIZER2에서 생성되는 ION LEVEL이 불량합니다.";
					else if(nCode2==6)	str = "IONIZER2 TIP 상태가 불량합니다.";		
				case 59 : // by ckh 2009.02.13
					str = "피스톤 공급 압력이 부족합니다. 공급 압력을 확인 하세요.";
					break;	
				case 60 : // by ckh 2009.02.16
					str = "Stage에 Glass가 흡착 상태입니다. 흡착 Off 후 실행바랍니다.";
					break;
				case 61 : // by ckh 2009.02.16
					str = "Stage 퍼지 상태입니다. 퍼지 Off 후 실행바랍니다.";	
					break;
				case 62 : // by ckh 2009.02.16
					str = "Stage 퍼지 및 흡착 센서 이상 입니다. Stage 공압 센서 확인 후 실행 바랍니다.";	
					break;
				// Alarm
				case 100 : str = "非常停止";
					if(nCode2==1)		str = "非常停止 : EMO 1 is pressed.";
					else if(nCode2==2)	str = "非常停止 : EMO 2 is pressed.";
					else if(nCode2==3)	str = "非常停止 : EMO 3 is pressed.";
					else if(nCode2==4)	str = "非常停止 : EMO 4 is pressed.";
					else if(nCode2==5)	str = "非常停止 : EMO 5 is pressed.";
					else if(nCode2==6)	str = "非常停止 : EMO 6 is pressed.";		
					else if(nCode2==7)	str = "非常停止 : EMO 7 is pressed.";	
					else if(nCode2==8)	str = "非常停止 : AREA 1 is detected.";
					else if(nCode2==9)	str = "非常停止 : AREA 2 is detected.";		
					else if(nCode2==10)	str = "非常停止 : AREA 3 is detected.";
					break;
				case 101 : str = "設備運行中安全門打開. ";
					if(nCode2==1)		str = "設備運行中安全門打開. : DOOR 1";
					else if(nCode2==2)	str = "設備運行中安全門打開. : DOOR 2";
					else if(nCode2==3)	str = "設備運行中安全門打開. : DOOR 3";
					else if(nCode2==4)	str = "設備運行中安全門打開. : MAINT DOOR";
					break;
				case 102 : str = "設備運行中Area Sensor被感應. ";
					break;
				case 103 : str = "所有軸原點無感應請重新原點復位.";
					break;
				case 104 : str = "作業前或作業中Robot arm 感應器被感應請確認";  // 안전상의 문제로 arm을 확인하고 작업함. 2003. 01. 10.
					break;
				case 105 : str = " S軸沒回到正位置.";
					break;
				case 106 : str = "Y축이 정위치로 가지 못했습니다.";
					break;
				case 107 : str = "K軸沒回到正位置.";
					break;
				case 108 : // by ckh 2009.02.13 반입위치값이 잘못되었을 경우.
					str = "K축의 반입 위치값이 잘못되었습니다. 티칭값을 수정하세요.";
					break;
				case 109 : // by ckh 2009.02.13 공급위치값이 잘못되었을 경우.
					str = "K축의 공급 위치값이 잘못되었습니다. 티칭값을 수정하세요.";
					break;	
				case 110 : str = "吐出作業中 K, S 的 Limit 被感應確認后原點復位再?動.";
					break;
				case 111 : str = "S軸連接I/O 解除失敗";
					break;
				case 112 : str = "Count BD非正常動作.";
					break;
				case 113 : // by ckh 2009.02.13 공급위치값이 잘못되었을 경우.
					str = "K축의 측정 위치값이 잘못되었습니다. 티칭값을 수정하세요.";
					break;
				case 115 : 
					iBoardNum = nCode2/0x10;
					iAxisNum  = nCode2%0x10;
					str.Format("%d Board的 %d Axis Servo Alarm . 切斷電源或按非常停止后再開始..",iBoardNum, iAxisNum);
					break;
				case 117 :
					str = "Encorder Value of K1, K2 Axis is different";
					break;
				case 120 : 
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + " : MEASURING 동작 비정상 완료.";
					break;
				case 121 : 
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + " : ADJUSTING동작 비정상 완료.";
					break;
				case 122 : str = "토출모터 위치 초기화 실패. PC를 재부팅해 주세요";
					break;
				case 123 : 
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + " : Specout counting 값이 설정치를 초과.";
					break;
				case 130: str.Format("H%d Pump P軸 (-)不是基準位置.", nCode2);
					break;
				case 131: str.Format("H%d Pump P? (+)不是基準位置.", nCode2);	// by shin, 2006.10.21 Volume축 정위치 에러 구문 추가
					break;
				case 135: str.Format("There is a problem with the Up/Down cylinder sensor.....");
					break;
				case 136: str.Format("H%d Piston氣缸感應器狀態異常...", nCode2);	// // by shin, 2007.01.03 
					break;	
				case 137: str.Format("H%d Joint Step1을 완료하지 않았습니다...", nCode2);	// by shin, 2007.01.03 
					break;	
				case 138: str.Format("H%d Joint Step2 作業中安裝感應器無動作....", nCode2);	// // by shin, 2007.01.03 
					break;	
				case 139: str.Format("H%d Pump P軸的 Limit Sensor感應,..", nCode2);	// // by shin, 2007.01.03 
					break;	
				case 140: str.Format("H%d 計算出超過Pump溶儲量的LC量..", nCode2);
					break;
				case 141: str.Format("H%d Gate G軸不在Open位置.", nCode2);	// by shin, 2007.01.03 
					break;
				case 142: str.Format("H%d Gate G軸不在Close位置.", nCode2);	// by shin, 2007.01.03 
					break;
				case 143: str.Format("H%d 計算出超過Pump溶儲量的LC量....", nCode2);
					break;		
				case 144: str.Format("H%d Pump P軸不在安裝位置.", nCode2);	// by shson, 2006.10.11 Volume축 정위치 에러 구문 추가
					break;
				case 145: str.Format("H%d Pump P軸正位置Error發生.", nCode2);	// by shson, 2006.10.11 Volume축 정위치 에러 구문 추가
					break;
				case 146: str.Format("%d供給裝置的注入口狀態異常...",nCode2);
					break;
				case 147: str.Format("%d供給裝置的Pinch閥狀態異常....",nCode2);
					break;
				case 148: str.Format("%d供給裝置 CDA狀態異常...",nCode2);
					break;
				case 149: str.Format("%d液晶供給裝置狀態檢?...",nCode2);
					break;
				case 150: str.Format("Safety Interlock被解除..");
					break;
		//2010.08.18 by tskim
		//		case 151: str.Format("H%d LC 공급 상한 센서가 감지 되었습니다...", nCode2);	
		//			break;
				case 152: str.Format("%d Canister初期稱重測量失敗...",nCode2);
					break;
				case 153: str.Format("%d Canister內無液晶..",nCode2);
					break;
				case 154: str.Format("%d Canister內液晶警告量到達...",nCode2);
					break;
				case 155 : str = "토출 작업 도중 Line의 X Position 계산값이 이상합니다. 확인하여 원점 복귀 작업 후 다시 시작하세요.";
					break;
				case 156 : str = "토출 작업 도중 Line의 Y Position 계산값이 이상합니다. 확인하여 원점 복귀 작업 후 다시 시작하세요.";
					break;
				case 157 : str = "토출 작업 도중 Line의 X Position 이상 발생! 확인하여 원점 복귀 작업 후 다시 시작하세요.";
					break;
				case 158 : str = "토출 작업 도중 Line의 Y Position 이상 발생! 확인하여 원점 복귀 작업 후 다시 시작하세요.";
					break;
		//2011.01.26 by tskim
				case 159: str.Format("%d번 Canister의 액정 공급량 Limit 이하 알람!!...",nCode2);
					break;
				
				case 188: str = "타겟량과 측정량이 맞지 않습니다. 조정 완료 후 진행하세요..";
					break;

		//반출 대기 시간 1000초 경과 할 경우 알람 발생...
				case 189:
					strTemp1 = "",strTemp2 = "";
					str = "반출 대기 시간 1000초 초과 하였습니다.";
					break;


		////////////////////////// 여기서부터는 Warning으로 보고되는 Error 항목입니다. ///////////////////// 
				case 200 : str = "運行中干涉外信號切斷.確認PLC的信號后在開始.";
					break;
				case 201 : str = "Transfer배출 동작 중이므로 진행할 수 없습니다..";
					break;
				case 202 : str = "Transfer수취 동작 중이므로 진행할 수 없습니다..";
					break;
				case 203 : str = "Balance Mode가 정상 Mode로 전환이 되었습니다. Balance의 상태가 초기화 되었는지 확인하세요.";
					break;
				case 204:// by ckh 2009.02.14 알람 1회만 하기위해 ,
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 +(" Dummy Cup 1內 LC量過多");	
					break;
				case 205: 
					strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 +(" Dummy Cup 2內 LC量過多.");	
					break;
				case 206 : str = "INTERFACE중 NACK CODE로 응답이 왔습니다.";	
					break;
				case 207 : str = "INTERFACE中應答時間超過.";	
					break;
				case 208 : str = " 從ECS 下來的ADD/DEL情報異常, 請確認Pane情報 ...";	
					break;
				case 209 : str = " Column K軸電機超過行?距離.調整設定?及注入潤滑油...";	
					break;
				case 210 : str = " Head S軸電機超過行?距離.調整設定?及注入潤滑油...";	
					break;
				case 211 : str = " 無Pair Glass的Seal 塗佈完了信號,請確認.";		
					break;
				case 212 : str = "[ECS]PD DOWN REQUEST OFF SIGNAL TIMEOVER. CHECK REQUEST BIT"; //141026 PD DOWN : ECS REQUEST BIT 가 OFF되지 않은면 해당 알람이 떠야됨. 
					break;
				case 300: str = " Panel 情報讀取失敗, 請確認Panel 情報...";	
					break;
				case 301: str = "발란스 내부에 측정 컵이 감지 되었습니다.!!";	
					break;
				case 302: str = "Mark Align 정도 데이터 획득 실패!!";
					break;
				case 303: str = "Mark#1 Search 실패!!";
					break;
				case 304: str = "Mark#2 Search 실패!!";
					break;
				case 305: //2010.06.08 by tskim LC 유의차 (Canister 1~2)
					strTemp1="",strTemp2="";
					//for(i=0; i<MAX_NOZZLE/2 ; i++)
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d, ",i+1);
							strTemp2+=strTemp1;
							RefHead = i;//2010.06.07 by tskim LC Diff차이 설정치 Display를 위해.
						}
					}
					strTemp1.Empty();
					if(RefHead<MAX_NOZZLE/2)
						strTemp1.Format("LC양 유의차가 Max Head %d (설정치%.0f g)를 초과 하였습니다.",pView->m_nHeadLcMax1+1,pDoc->m_dLCAmountDiff[RefHead/(MAX_NOZZLE/2)]);
					else
						strTemp1.Format("LC양 유의차가 Max Head %d (설정치%.0f g)를 초과 하였습니다.",pView->m_nHeadLcMax2+1,pDoc->m_dLCAmountDiff[RefHead/(MAX_NOZZLE/2)]);
					//m_strMsg2 = strTemp2 +("LC양이 유의차(설정치)를 초과 하였습니다.");
					str = strTemp2 +strTemp1;
					break;
				case 306: //2010.07.05 by tskim 이전 중공 헤드에 재 중공 지령 방지
					str.Format("직전 중앙 공급 헤드와 중복 됩니다. Head%d",nCode2);	
					break;
				case 307 : 
		// LC 하한 감지 센서에 의한 Error 발생 ....
					str="",strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("H%d/",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2+" BOTTLE의 LC 용액 양이 부족합니다.(LC 용액 하한 감지 센서 확인)";
					break;
				case 308 : 
					str="",strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + "동일 판넬 사용 Head의 Lot No 불일치.. 확인하세요.";			
					break;
				case 309 : 
					str="",strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + "Canister Head간 Lot No 불일치.. 확인하세요.";			
					break;
				case 310: 
					str="",strTemp1="",strTemp2="";
					for(i=0; i<MAX_NOZZLE ; i++)
					{
						wTempValue = 0x0001;
						wTempValue = wTempValue << i;
						if(wTempValue&nCode2){
							strTemp1.Format("%d, ",i+1);
							strTemp2+=strTemp1;
						}
					}
					str = strTemp2 + "LINE Drop 작업 중 맺힘이 감지 되었습니다.";
					break; 

				case 311 : str = "VALC Auto Mode is not. Check Please.";		
					break;
				case 320 : str = "FAN STOP. Check Please.";		
					break;
		//2011.04.27 by tskim
				case 400 : str.Format("%d번 지진 모드 Retry Count 초과!!.",nCode2);
					break;
				case 401: str.Format("Vision Communication Fail.");
					break;
				case 402: str.Format("Vision Writing Fail 'CSV File.' ");
					break;
				case 403: str.Format("Time Over of Reading Vision Count Result");
					break;
				case 404: str.Format("File Reading Fail");
					break;
				case 405: str.Format("There is no File");
					break;
				case 406: str.Format("Vision Responds NG");
					break;
				case 407: str.Format("Cell No is Dismatched");
					break;
				case 408: str.Format("Cell Sequence is wrong");
					break;
				case 409: str.Format("Vision Drop Count Result Error");
					break;
				case 410: str.Format("Vision Send to Dispenser 'ERROR'");
					break;
				case 411: str.Format("Response TimeOver!");
					break;
				case 412: str.Format("Sending Title & Receiving Title is different");
					break;
				case 413: str.Format("Vision Communication Error!");
					break;
				case 414: str.Format("Dummy Pattern Drop Detected");	//ehji
					break;
//220603 BIG DROP DETECT ( (OLD) DEEP DROP ) 
				case 415: str.Format("Big Drop Detected ( Cell )");
					break;
//220603 BIG DROP DETECT ( (OLD) DEEP DROP )
				case 500 : str.Format("H%d N DRIVER OVEROAD ALARM", nCode2);
					break; 
				case 600 :			//ehji 150527
						str="",strTemp1="",strTemp2="";
						for(i=0; i<MAX_NOZZLE ; i++)
						{
							wTempValue = 0x0001;
							wTempValue = wTempValue << i;
							if(wTempValue&nCode2){
								strTemp1.Format("%d, ",i+1);
								strTemp2+=strTemp1;
							}
						}
						str = strTemp2 + "DistPerDrop Average Over !!";
						break;

			//drop detect alarm
				//140926
				case 700 : str.Format("Head1 Drop 水珠感應器被感應.");
					break;
				case 701 : str.Format("Head2 Drop 水珠感應器被感應.");
					break;
				case 702 : str.Format("Head3 Drop 水珠感應器被感應.");
					break;
				case 703 : str.Format("Head4 Drop 水珠感應器被感應.");
					break;
				case 704 : str.Format("Head5 Drop 水珠感應器被感應.");
					break;
				case 705 : str.Format("Head6 Drop 水珠感應器被感應.");
					break;
				case 706 : str.Format("Head7 Drop 水珠感應器被感應.");
					break;
				case 707 : str.Format("Head8 Drop 水珠感應器被感應.");
					break;
				case 708 : str.Format("Head9 Drop 水珠感應器被感應.");
					break;
				case 709 : str.Format("Head10 Drop 水珠感應器被感應.");
					break;
				case 710 : str.Format("Head11 Drop 水珠感應器被感應.");
					break;
				case 711 : str.Format("Head12 Drop 水珠感應器被感應.");
					break;
				case 712 : str.Format("Head13 Drop 水珠感應器被感應.");
					break;
				case 713 : str.Format("Head14 Drop 水珠感應器被感應.");
					break;
				case 714 : str.Format("Head15 Drop 水珠感應器被感應.");
					break;
				case 715 : str.Format("Head16 Drop 水珠感應器被感應.");
					break;

//2015.04.29 by tskim 원재료 전산화Add Dummy Drop 보고 
		                case 820:
			                   str.Format("From ECS Dummy Drop 미실시 & Dispesner Dummy 미 실시 Alarm!");
			                break;
					//2014.12.01 by tskim Amp, Duty 연산 관련 오류 Error
				case 800:case 801:case 802:case 803:case 804:case 805:case 806:case 807:
				case 808:case 809:case 810:case 811:case 812:case 813:case 814:case 815: 
				str.Format("Head%d Amplitude, Duty Value is 0..!",(nCode2-800)+1);
					break;

				case 7001:case 7002:case 7003:case 7004:case 7005:case 7006:case 7007:case 7008:
				case 7009:case 7010:case 7011:case 7012:case 7013:case 7014:case 7015:case 7016:
					str.Format("H%d LC 공급 상한 센서가 감지 되었습니다...", nCode2);
					break;
				default: str = "존재하지 않는 오류 번호 입니다.!!";
					break;
		}		 
	}	//ehji 140309 중국어끝
	return str;
}

void CErrList::OnOK() 
{
	// TODO: Add extra validation here
	
//	CDialog::OnOK();
}

void CErrList::OnCancel() 
{
	// TODO: Add extra cleanup here
	
//	CDialog::OnCancel();
}

void CErrList::OnBtnLogOpen() 
{
	CFileDialog dlg( TRUE, _T("LOG"),_T("D:\\LOG\\*.log"),OFN_HIDEREADONLY,_T("LOG File (*.log)|*.log|") );
	if( dlg.DoModal() != IDOK )
		return;
	m_strFileName = dlg.GetFileTitle()+".log";
	MakeListView(1);
	SetListData(1);	
}

void CErrList::OnCheckLog() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	UpdateData(TRUE);

	//by shin//2009.08.20//debug error로 인해...//

/*	if(m_bCheckLog)
	{
		MakeListView(1);	// Log
		m_ctrlErrList.DeleteAllItems();
		m_ctrlLog.EnableWindow(TRUE);
		m_ctrlClear.EnableWindow(FALSE);
		m_ctrlDate.SetCaption("");

	}*/
//	else
//	{
		MakeListView(0);	// Error
		SetListData(0);
		m_ctrlLog.EnableWindow(FALSE);
		if(pView->GetDocument()->m_bIsEngineerMode==TRUE)
		{
			m_ctrlClear.EnableWindow(TRUE);
		}
		else	m_ctrlClear.EnableWindow(FALSE);
		m_ctrlDate.SetCaption("");
//	}	
}
void CErrList::OnBtnClear() 
{
	CString strDir;
	strDir = "D:\\TOP\\P8CA_LC\\P8CA_LC_EXE\\";
	FILE *fp;
	fp = fopen(strDir+"EList.txt","w");
	if(!fp) return;
	fclose(fp);
//	Sleep(500);
	SetListData(0);	
}

BEGIN_EVENTSINK_MAP(CErrList, CDialog)
    //{{AFX_EVENTSINK_MAP(CErrList)
	ON_EVENT(CErrList, IDC_BTN_RETURN, -600 /* Click */, OnClickBtnReturn, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CErrList::OnClickBtnReturn() 
{
	EndDialog(IDOK);		
}



void CErrList::OnPreviousErrlistCheck() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE); 
	
	if(m_bPreviousErrList)
		m_bPreviousErrList=TRUE; 
	else 
		m_bPreviousErrList=FALSE; 

	MakeListView(0); 	
	SetListData(0);
}

void CErrList::SelectLanguage()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(pView->m_nLanguage == 0)	//ehji 140309
	{
		SetDlgItemText(IDC_PREVIOUS_ERRLIST_CHECK,				_T("이전 ErrList 보기"));
		SetDlgItemText(IDC_CHECK_LOG,				_T("LOG File 보기"));
		SetDlgItemText(IDC_BTN_CLEAR,				_T("이력 삭제 "));
	}
	else if(pView->m_nLanguage == 1)	//ehji 140309
	{
		SetDlgItemText(IDC_PREVIOUS_ERRLIST_CHECK,				_T("Before ErrList View"));
		SetDlgItemText(IDC_CHECK_LOG,				_T("LOG File View"));
		SetDlgItemText(IDC_BTN_CLEAR,				_T("Record Delete "));
	}
	else if(pView->m_nLanguage == 2)	//ehji 140309
	{
		SetDlgItemText(IDC_PREVIOUS_ERRLIST_CHECK,				_T("査看之前ErrList"));
		SetDlgItemText(IDC_CHECK_LOG,				_T("査看LOG File "));
		SetDlgItemText(IDC_BTN_CLEAR,				_T("刪除歷例 "));
	}
}

// AutoIF.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
//
#include "MainFrm.h"
#include "P8CA_LcDispView.h"
#include "P8CA_LcDispDoc.h"
#include "AutoIF.h"

//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//
extern short RobotSend;
extern short RobotReceive;
extern short RobotStatus;
/////////////////////////////////////////////////////////////////////////////
// CAutoIF dialog


CAutoIF::CAutoIF(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoIF::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAutoIF)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nIFStatus=0;
	m_nIFStatusTemp=0;
	//
	for( int i = 0 ; i < 6 ; i++ ) 
		for( int j = 0 ; j < 10 ; j++ )	
		{ m_bIFStatus[i][j]=FALSE; m_bIFStatusBK[i][j]=FALSE;}
}

CAutoIF::~CAutoIF()
{
}

void CAutoIF::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoIF)
	DDX_Control(pDX, IDC_MSFLEXGRID_IF, m_ctrlIF);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAutoIF, CDialog)
	//{{AFX_MSG_MAP(CAutoIF)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoIF message handlers

void CAutoIF::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CAutoIF::OnCancel() 
{
	// TODO: Add extra cleanup here
	//lbg for test
	//if(PC_TYPE==TRUE)
		KillTimer(TIMER_AUTO_IF);
	//
	CDialog::OnCancel();
}

BOOL CAutoIF::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int i=0,j=0;
	for(i=0 ; i < 6 ; i++)m_ctrlIF.SetColWidth(i, 1700);
	for(i=0 ; i < 11 ; i++)m_ctrlIF.SetRowHeight(i,300);

	SubOpenIFStrFile();
	SubDisplayIFStr();
//lbg for test
//	if(PC_TYPE==TRUE)
		SetTimer(TIMER_AUTO_IF,500,NULL);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAutoIF::SubOpenIFStrFile() 
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();


	// CimStr.dat 화일을 열고서 저장
	ifstream fi;
	char ch[256];
	int i = 0, j = 0;


	if(pView->m_nLanguage ==0)
	fi.open("D:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\IFStr_kor.dat", ios::in);
	else if(pView->m_nLanguage ==1)
	fi.open("D:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\IFStr_eng.dat", ios::in);
	else if(pView->m_nLanguage ==2)
	fi.open("D:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\IFStr_chi.dat", ios::in);


	if(fi.is_open())
	{
		for(i = 0 ; i <6 ; i++) //  type
			for(j = 0 ; j < 11 ; j++) // bit
			{
				fi >> ch ;
				m_structIFCaptionSet.strIF[i][j].Format("%s",ch);
			}
	}
	else
	{
		AfxMessageBox("IFStr.dat 파일을 여는것을 실패하였습니다");
		return;
	}

	fi.close();

//	
}

void CAutoIF::SubDisplayIFStr() 
{
	// IoStr.data에서 읽어들인 str설정..
	int i = 0 , j = 0;

	for( i = 0 ; i < 11 ; i++ ) //	row
	{
		for(  j = 0 ; j < 6 ; j++ )	//	col
		{
			// m_ctrlIF 에 str 설정..
			m_ctrlIF.SetTextMatrix(i,j,m_structIFCaptionSet.strIF[j][i]);
		}
	}	
}


void CAutoIF::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	//
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	int i=0,j=0;
	//
	// memcpy( dest, source , count )
	memcpy(m_bIFStatusBK,m_bIFStatus ,sizeof(BOOL)*60);

	//# DISP수취...
	m_nIFStatus = pView->m_pDevice->MNET_ReadBit_As_Word(DISP_RECEIVE);
	for(i = 0 ; i < 7 ; i++)
	{
		m_nIFStatusTemp = m_nIFStatus >> i;
		if( (m_nIFStatusTemp & 0x0001) != 0 ) // LSB bit ? 1	
			m_bIFStatus[0][i] = true;	
		else	
			m_bIFStatus[0][i] = false;
	}
	//# 이건 뭐지?
	//m_bIFStatus[0][8] = pView->m_pDevice->MNET_Read_A_Bit(BaseAddr_DISP+0x90,7);

	//# Robot배출...
	m_nIFStatus = pView->m_pDevice->MNET_ReadBit_As_Word(RobotSend);
	for(i = 0 ; i < 7 ; i++)
	{
		m_nIFStatusTemp = m_nIFStatus >> i;
		if( (m_nIFStatusTemp & 0x0001) != 0 ) // LSB bit ? 1	
			m_bIFStatus[1][i] = true;	
		else	
			m_bIFStatus[1][i] = false;
	}
	//# Disp 배출
	m_nIFStatus = pView->m_pDevice->MNET_ReadBit_As_Word(DISP_SEND);
	for(i = 0 ; i < 7 ; i++)
	{
		m_nIFStatusTemp = m_nIFStatus >> i;
		if( (m_nIFStatusTemp & 0x0001) != 0 ) // LSB bit ? 1	
			m_bIFStatus[2][i] = true;	
		else	
			m_bIFStatus[2][i] = false;
	}

	//# Robot 수취
	m_nIFStatus = pView->m_pDevice->MNET_ReadBit_As_Word(RobotReceive);
	for(i = 0 ; i < 7 ; i++)
	{
		m_nIFStatusTemp = m_nIFStatus >> i;
		if( (m_nIFStatusTemp & 0x0001) != 0 ) // LSB bit ? 1	
			m_bIFStatus[3][i] = true;	
		else	
			m_bIFStatus[3][i] = false;
	}
	//# Disp Status
	m_nIFStatus = pView->m_pDevice->MNET_ReadBit_As_Word(DISP_BIT);
	for(i = 0 ; i < 16 ; i++)
	{
		if(i==0) j=0;
		else if(i==5) j=1;
		else if(i>7) j=(i-6); // 2~9
		else continue;
		//
		m_nIFStatusTemp = m_nIFStatus >> i;
		if( (m_nIFStatusTemp & 0x0001) != 0 ) // LSB bit ? 1	
			m_bIFStatus[4][j] = true;	
		else	
			m_bIFStatus[4][j] = false;
	}
	//# TF Status
	m_nIFStatus = pView->m_pDevice->MNET_ReadBit_As_Word(RobotStatus);
	for(i = 0 ; i < 16 ; i++)
	{
		if(i==0) j=0;
		else if(i==5) j=1;
		else if(i>7) j=(i-6); // 2~9
		else continue;
		//
		m_nIFStatusTemp = m_nIFStatus >> i;
		if( (m_nIFStatusTemp & 0x0001) != 0 ) // LSB bit ? 1	
			m_bIFStatus[5][j] = true;	
		else	
			m_bIFStatus[5][j] = false;
	}
	
	// m_bIFStatus[i][j]의 값을 이용해서 control에 color 설정한다..
	for( i = 0 ; i < 6 ; i++ ) //	col
		for(  j = 0 ; j < 10 ; j++ )	//	row
		{
			if(m_bIFStatus[i][j]!=m_bIFStatusBK[i][j])
			{
				m_ctrlIF.SetCol(i);	// col
				m_ctrlIF.SetRow(j+1);// row
				if(m_bIFStatus[i][j] == TRUE)
					m_ctrlIF.SetCellBackColor(LIGHTBLUE);
				else
					m_ctrlIF.SetCellBackColor(WHITE);
			}
		}	
	CDialog::OnTimer(nIDEvent);
}



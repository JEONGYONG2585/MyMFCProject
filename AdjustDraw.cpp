// AdjustDraw.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "AdjustDraw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int m_nGraphSel;
/////////////////////////////////////////////////////////////////////////////
// CAdjustDraw dialog


CAdjustDraw::CAdjustDraw(CWnd* pParent /*=NULL*/)
	: CDialog(CAdjustDraw::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAdjustDraw)
	m_nHeadSelected = -1;
	m_nGraphSel = 0;
	m_nDataSel = 0;
	//}}AFX_DATA_INIT
}

CAdjustDraw::~CAdjustDraw()
{
}

void CAdjustDraw::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdjustDraw)
	DDX_Radio(pDX, IDC_RADIO_GRAPH_H1, m_nHeadSelected);
	DDX_Radio(pDX, IDC_RADIO_ALL, m_nDataSel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdjustDraw, CDialog)
	//{{AFX_MSG_MAP(CAdjustDraw)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_RADIO_GRAPH_H1, OnRadioGraphH1)
	ON_BN_CLICKED(IDC_RADIO_ALL, OnRadioAll)
	ON_BN_CLICKED(IDC_RADIO_GRAPH_H2, OnRadioGraphH1)
	ON_BN_CLICKED(IDC_RADIO_GRAPH_H3, OnRadioGraphH1)
	ON_BN_CLICKED(IDC_RADIO_GRAPH_H4, OnRadioGraphH1)
	ON_BN_CLICKED(IDC_RADIO_GRAPH_H5, OnRadioGraphH1)
	ON_BN_CLICKED(IDC_RADIO_GRAPH_H6, OnRadioGraphH1)
	ON_BN_CLICKED(IDC_RADIO_GRAPH_H7, OnRadioGraphH1)
	ON_BN_CLICKED(IDC_RADIO_GRAPH_H8, OnRadioGraphH1)
	ON_BN_CLICKED(IDC_RADIO_GRAPH_H9, OnRadioGraphH1)
	ON_BN_CLICKED(IDC_RADIO_GRAPH_H10, OnRadioGraphH1)
	ON_BN_CLICKED(IDC_RADIO_GRAPH_H11, OnRadioGraphH1)
	ON_BN_CLICKED(IDC_RADIO_GRAPH_H12, OnRadioGraphH1)
	ON_BN_CLICKED(IDC_RADIO_GRAPH_H13, OnRadioGraphH1)
	ON_BN_CLICKED(IDC_RADIO_GRAPH_H14, OnRadioGraphH1)
	ON_BN_CLICKED(IDC_RADIO_GRAPH_H15, OnRadioGraphH1)
	ON_BN_CLICKED(IDC_RADIO_GRAPH_H16, OnRadioGraphH1)
	ON_BN_CLICKED(IDC_RADIO_FIRST, OnRadioAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdjustDraw message handlers

BOOL CAdjustDraw::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
//	SetIcon(m_hIcon, TRUE);			// Set big icon
//	SetIcon(m_hIcon, FALSE);		// Set small icon

	CRect r;
	GetDlgItem(IDC_DUMMY)->GetWindowRect(r);
	ScreenToClient(r);          //모니터에 있는 lefttop를 프로그램의 lefttop로 옴기는것 

	// 이곳에서, IDC_DUMMY를 현재 활성 window로 만든다..( CreateEx 이부분..)
	mp_TrendView = new TrendWindow();
	mp_TrendView->CreateEx(WS_EX_CLIENTEDGE, NULL, NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, r, this, 21001);

	mp_TrendView->StartGraph();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAdjustDraw::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
//	KillTimer(0);
	mp_TrendView->SendMessage(WM_CLOSE);
	delete mp_TrendView; mp_TrendView=NULL;	
}

/*
void CAdjustDraw::OnSelchangeComboHeadsel() 
{
	mp_TrendView->m_nHead = m_ctrlHeadSelected.GetCurSel();	
}
*/
void CAdjustDraw::OnOK() 
{
	// TODO: Add extra validation here
	
//	CDialog::OnOK();
}

void CAdjustDraw::OnRadioGraphH1() 
{
	UpdateData(TRUE);
	mp_TrendView->m_nHead = m_nHeadSelected;
}

void CAdjustDraw::OnRadioAll() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	mp_TrendView->m_nData = m_nDataSel;
}

// GlassID.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "GlassID.h"

#include "MainFrm.h"
#include "P8CA_LcDispView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGlassID dialog


CGlassID::CGlassID(CWnd* pParent /*=NULL*/)
	: CDialog(CGlassID::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGlassID)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CGlassID::~CGlassID()
{
}

void CGlassID::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGlassID)
	DDX_Control(pDX, IDC_MSFLEXGRID_GLASS_SELF, m_ctrlGridGlsInformSelf);
	DDX_Control(pDX, IDC_MSFLEXGRID_GLASS_PAIR, m_ctrlGridGlsInformPair);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGlassID, CDialog)
	//{{AFX_MSG_MAP(CGlassID)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGlassID message handlers

BOOL CGlassID::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
/*
	CString str="";
	
	m_ctrlGlassIDList.SetColWidth(0, 500);
	for(i=1 ; i <= 7 ; i++)	m_ctrlGlassIDList.SetColWidth(i, 1500);	
	for(i=0 ; i < 10 ; i++) m_ctrlGlassIDList.SetRowHeight(i,350);
	
	for(i=0; i<10;i++)
	{
		str.Format("%d",i+1);
		m_ctrlGlassIDList.SetTextMatrix(i, 0, str);
		m_ctrlGlassIDList.SetTextMatrix(i, 1, pDoc->m_strGlassIDSave[TFT_GLASS][i]);
	}
	m_ctrlTFTGlassDisp.SetBackColor(GREEN);
	m_ctrlCFGlassDisp.SetBackColor(WHITEGRAY);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
				  */
	CString str="";
	CString strTitle[4]={"Glass ID", "Lot No.", "Slot No.", "Panel Information"};
	m_ctrlGridGlsInformSelf.SetColWidth(0, 500);
	m_ctrlGridGlsInformPair.SetColWidth(0, 500);
	m_ctrlGridGlsInformSelf.SetRowHeight(0,350);
	m_ctrlGridGlsInformPair.SetRowHeight(0,350);

	for(int i=0 ; i < 10 ; i++) 
	{
		m_ctrlGridGlsInformSelf.SetRowHeight(i+1,320);
		m_ctrlGridGlsInformPair.SetRowHeight(i+1,320);

		str.Format("%d",i+1);
		m_ctrlGridGlsInformSelf.SetTextMatrix(i+1, 0, str);
		m_ctrlGridGlsInformPair.SetTextMatrix(i+1, 0, str);
	}
	m_ctrlGridGlsInformSelf.SetColWidth(1, 2200);	m_ctrlGridGlsInformSelf.SetColWidth(2, 1000);	
	m_ctrlGridGlsInformSelf.SetColWidth(3, 1000);	m_ctrlGridGlsInformSelf.SetColWidth(4, 4500);

	m_ctrlGridGlsInformPair.SetColWidth(1, 2200);	m_ctrlGridGlsInformPair.SetColWidth(2, 1000);	
	m_ctrlGridGlsInformPair.SetColWidth(3, 1000);	m_ctrlGridGlsInformPair.SetColWidth(4, 4500);
	
	for(int j=0 ; j < 4 ; j++)
	{
		m_ctrlGridGlsInformSelf.SetTextMatrix(0, j+1, strTitle[j]);
		m_ctrlGridGlsInformPair.SetTextMatrix(0, j+1, strTitle[j]);
	}


	m_ctrlGridGlsInformSelf.SetColAlignment(1,7);
	m_ctrlGridGlsInformPair.SetColAlignment(1,7);
// by ckh 2009.02.16  막음. 
// 	for(i=0;i<4;i++)	
// 	{	// 우측 정렬
// 		m_ctrlGridGlsInformSelf.SetColAlignment(i+1,7);
// 		m_ctrlGridGlsInformPair.SetColAlignment(i+1,7);
// 	}
	for(i=0;i<10;i++)
	{
		m_ctrlGridGlsInformSelf.SetTextMatrix(i+1, 1, pView->m_pDevice->MakeStringDatafromPLC(Glass_Inform[TFT_GLASS][i].Glass_ID)); //by ckh 2009.02.16
		m_ctrlGridGlsInformSelf.SetTextMatrix(i+1, 2, Glass_Inform[TFT_GLASS][i].Lot_number);
		m_ctrlGridGlsInformSelf.SetTextMatrix(i+1, 3, Glass_Inform[TFT_GLASS][i].Slot_number);
		m_ctrlGridGlsInformSelf.SetTextMatrix(i+1, 4, Glass_Inform[TFT_GLASS][i].Panel_Information);
	
		if(pView->m_pDevice->MakeStringDatafromPLC(Glass_Inform[CF_GLASS][i].Glass_ID)== "") continue;
		m_ctrlGridGlsInformPair.SetTextMatrix(i+1, 1, pView->m_pDevice->MakeStringDatafromPLC(Glass_Inform[CF_GLASS][i].Glass_ID));//by ckh 2009.02.16
		m_ctrlGridGlsInformPair.SetTextMatrix(i+1, 2, Glass_Inform[CF_GLASS][i].Lot_number);
		m_ctrlGridGlsInformPair.SetTextMatrix(i+1, 3, Glass_Inform[CF_GLASS][i].Slot_number);
		m_ctrlGridGlsInformPair.SetTextMatrix(i+1, 4, Glass_Inform[CF_GLASS][i].Panel_Information);
	}


	return TRUE;  // return TRUE unless you set the focus to a control
}

BEGIN_EVENTSINK_MAP(CGlassID, CDialog)
    //{{AFX_EVENTSINK_MAP(CGlassID)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

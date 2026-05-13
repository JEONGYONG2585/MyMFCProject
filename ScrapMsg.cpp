// ScrapMsg.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"

#include "ScrapMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL m_bNGScrap;//2010.04.05 by tskim NG Scrap
/////////////////////////////////////////////////////////////////////////////
// CScrapMsg dialog


CScrapMsg::CScrapMsg(CWnd* pParent /*=NULL*/)
	: CDialog(CScrapMsg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScrapMsg)
	m_nScrapSelType = -1;
	m_nRecipeNum = 0;
	m_strGlassID = _T("");
	m_nLotNum = 0;
	m_nSlotNum = 0;
	//}}AFX_DATA_INIT
}

CScrapMsg::~CScrapMsg()
{
}

void CScrapMsg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScrapMsg)
	DDX_Radio(pDX, IDC_SCRAP_SEL1, m_nScrapSelType);
	DDX_Text(pDX, IDC_EDIT_RECIPE_NUM, m_nRecipeNum);
	DDV_MinMaxUInt(pDX, m_nRecipeNum, 0, 199);
	DDX_Text(pDX, IDC_EDIT_GLASS_ID, m_strGlassID);
	DDV_MaxChars(pDX, m_strGlassID, 14);
	DDX_Text(pDX, IDC_EDIT_LOT_NUM, m_nLotNum);
	DDV_MinMaxUInt(pDX, m_nLotNum, 0, 999);
	DDX_Text(pDX, IDC_EDIT_SLOT_NUM, m_nSlotNum);
	DDV_MinMaxUInt(pDX, m_nSlotNum, 0, 99);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CScrapMsg, CDialog)
	//{{AFX_MSG_MAP(CScrapMsg)
	ON_BN_CLICKED(IDC_SCRAP_SEL1, OnScrapSel)
	ON_BN_CLICKED(IDC_BUTTON_GLASS_DATA_EDIT, OnButtonGlassDataEdit)
	ON_BN_CLICKED(IDC_SCRAP_SEL2, OnScrapSel)
	ON_BN_CLICKED(IDC_SCRAP_SEL3, OnScrapSel)
	ON_BN_CLICKED(IDC_SCRAP_SEL4, OnScrapSel)
	ON_BN_CLICKED(IDC_SCRAP_SEL5, OnScrapSel)
	ON_BN_CLICKED(IDC_NG_SCRAP, OnNgScrap)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScrapMsg message handlers
BOOL CScrapMsg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();	
	m_nRecipeNum=0, m_nLotNum=0, m_nSlotNum=0;

	SelectLanguage();

//	if(pDoc->m_bIsEngineerMode==FALSE)
//		GetDlgItem(IDC_BUTTON_GLASS_DATA_EDIT)->EnableWindow(FALSE);

	GetDlgItem(IDC_STATIC_SCRAP)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_RECIPE_NUM)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_GLASS_ID)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_RECIPE_NUM)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_GLASS_ID)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_LOT_NUM)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_SLOT_NO)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_LOT_NUM)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_SLOT_NUM)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_GLASS_DATA_EDIT)->ShowWindow(SW_HIDE);

/*	
	// 초기화..
	for(int i=0;i<2;i++)
	{
		m_nRecipeNum+=(pView->m_pDevice->HexConvert(CIM_Information.Recipe_Number.GetAt(i+2))*(int)pow(16,1-i));
		m_nLotNum+=(pView->m_pDevice->HexConvert(CIM_Information.Lot_number.GetAt(i))*(int)pow(16,1-i));
		m_nSlotNum+=(pView->m_pDevice->HexConvert(CIM_Information.Lot_number.GetAt(i+2))*(int)pow(16,1-i));
	}
	m_strGlassID=CIM_Information.Glass_ID;
*/
	UpdateData(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CScrapMsg::OnScrapSel() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	// TODO: Add your control notification handler code here
	BOOL bLcStatus = false;

	//나중에 장비내 Glass 투입 정보 확인 후 Scrap 할 수 있게 아래 내용 수정...//by shin
//	bLcStatus = pView->m_pDevice->MNET_Read_A_Bit(DISP_BIT, 12); 

//	UpdateData(TRUE); if((m_nScrapSelType >= 0) && (bLcStatus==TRUE)) GetDlgItem(IDOK)->EnableWindow();
	UpdateData(TRUE); if(m_nScrapSelType >= 0) GetDlgItem(IDOK)->EnableWindow();
}

void CScrapMsg::OnButtonGlassDataEdit() 
{
/*	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	UpdateData(true);
	CString str="";
	int j=0;

	str.Format("%2x",m_nRecipeNum);
	for(int i=0; i<2 ; i++) CIM_Information.Recipe_Number.SetAt(i+2,str.GetAt(i)); 
	
//	str="   "; // SP×3
//	CIM_Information.Glass_ID = m_strGlassID+str;
	
	str="";
	j=m_strGlassID.GetLength();
	if(j>14)
	{
		AfxMessageBox("14개 이하 문자열만 입력이 가능합니다.");
		return;
	}
	else if(j<14)
	{
		for(int i=0;i<(14-j);i++)
		{
			str+=" ";
		}
	}
	CIM_Information.Glass_ID = m_strGlassID+str;
	
	str.Format("%2x",m_nLotNum);
	for(i=0; i<2 ; i++) CIM_Information.Lot_number.SetAt(i,str.GetAt(i)); 

	str.Format("%2x",m_nSlotNum);
	for(i=0; i<2 ; i++) CIM_Information.Lot_number.SetAt(i+2,str.GetAt(i)); 
	
	//pView->m_pAuto->PostMessage(WM_INFODISP,1,m_nRecipeNum);
	//pView->m_pAuto->PostMessage(WM_INFODISP,1,m_nLotNum);
	pView->m_pAuto->PostMessage(WM_INFODISP,3,TRUE);
	if(PC_TYPE==TRUE && pView->m_pDevice->Stage_glass_check()==TRUE ){
		pView->m_pDevice->MNET_BitSet(BaseAddr_DISP+BitDispModeStatus+0x0c, true); 	// Glass 有
		pView->m_pDevice->MNET_BitSet(BaseAddr_DISP+BitDispModeStatus+0x0d, false); 	// 상류투입 가능 OFF
		
		pView->m_pDevice->MNET_BitSet(BaseAddr_CIM_1+BitDispModeStatus+0x01, true); // CIM 추가 : Disp가 Glass 있으면 1, 아니면 0
		pView->m_pDevice->MNET_WriteWord(BaseAddr_DISP+0x0C, "0001",1);				// Glass 有枚數
		pView->m_pDevice->MNET_WriteWord(BaseAddr_DISP+0x0D, "0000",1);				// Glass 투입 가능 매수
		pView->m_pDevice->MNET_WriteWord(BaseAddr_CIM_1+BitDispModeStatus+0x01,CIM_Information.Lot_number,1);	// 있을때 Glass 정보 표기 : Lot/Slot

		ThreadStage.nDropStatus=0;
	}
*/
}


void CScrapMsg::OnNgScrap() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE); 
	m_bNGScrap = TRUE;
	GetDlgItem(IDOK)->EnableWindow();
}

void CScrapMsg::SelectLanguage()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(pView->m_nLanguage ==0)
	{
		SetDlgItemText(IDC_SCRAP_SEL1,				_T("Unit내 Glass파손"));
		SetDlgItemText(IDC_SCRAP_SEL3,				_T("Operator Glass파손"));
		SetDlgItemText(IDC_SCRAP_SEL4,				_T("Engineer Glass파손"));
		SetDlgItemText(IDC_SCRAP_SEL5,				_T("기타"));
		SetDlgItemText(IDC_NG_SCRAP,				_T("NG 폐기 Scrap"));
	}

	else if(pView->m_nLanguage ==1)
	{
		SetDlgItemText(IDC_SCRAP_SEL1,				_T(" In Unit Glass damaged"));
		SetDlgItemText(IDC_SCRAP_SEL3,				_T("Operator Glass damaged"));
		SetDlgItemText(IDC_SCRAP_SEL4,				_T("Engineer Glass damaged"));
		SetDlgItemText(IDC_SCRAP_SEL5,				_T("Etc"));
		SetDlgItemText(IDC_NG_SCRAP,				_T("NG Discared Scrap"));
	}

	else if(pView->m_nLanguage ==2)
	{
		SetDlgItemText(IDC_SCRAP_SEL1,				_T("Unit內 Glass破損"));
		SetDlgItemText(IDC_SCRAP_SEL3,				_T("Operator Glass 破損"));
		SetDlgItemText(IDC_SCRAP_SEL4,				_T("Engineer Glass 破損"));
		SetDlgItemText(IDC_SCRAP_SEL5,				_T("其他"));
		SetDlgItemText(IDC_NG_SCRAP,				_T("NG 廢棄 Scrap"));
	}

}

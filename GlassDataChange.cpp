// GlassDataChange.cpp : implementation file
//
//lbg 추가
#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"
#include "GlassDataChange.h"
#include "Color.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CIM_DATA CIM_Information;
extern BOOL	g_bGlassDataChange;


/////////////////////////////////////////////////////////////////////////////
// CGlassDataChange dialog


CGlassDataChange::CGlassDataChange(CWnd* pParent /*=NULL*/)
	: CDialog(CGlassDataChange::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGlassDataChange)
		// NOTE: the ClassWizard will add member initialization here
	m_nSelNo = 0;
	//}}AFX_DATA_INIT
}


void CGlassDataChange::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGlassDataChange)
	DDX_Control(pDX, IDC_RECIEPE, m_labelHostRecipeNo);
	DDX_Control(pDX, IDC_LOT, m_labelLotNo);
	DDX_Control(pDX, IDC_SLOT, m_labelSlotNo);
	DDX_Control(pDX, IDC_CODE, m_labelCode);
	DDX_Control(pDX, IDC_ID, m_labelGlsID);
	DDX_Control(pDX, IDC_SEL1, m_ctrlSel1);
	DDX_Control(pDX, IDC_SEL2, m_ctrlSel2);
	DDX_Control(pDX, IDC_GLS_CODE, m_labelGlsCode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGlassDataChange, CDialog)
	//{{AFX_MSG_MAP(CGlassDataChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGlassDataChange message handlers

BEGIN_EVENTSINK_MAP(CGlassDataChange, CDialog)
    //{{AFX_EVENTSINK_MAP(CGlassDataChange)
	ON_EVENT(CGlassDataChange, IDC_RECIEPE, -600 /* Click */, OnClickReciepe, VTS_NONE)
	ON_EVENT(CGlassDataChange, IDC_LOT, -600 /* Click */, OnClickLot, VTS_NONE)
	ON_EVENT(CGlassDataChange, IDC_SLOT, -600 /* Click */, OnClickSlot, VTS_NONE)
	ON_EVENT(CGlassDataChange, IDC_CODE, -600 /* Click */, OnClickCode, VTS_NONE)
	ON_EVENT(CGlassDataChange, IDC_ID, -600 /* Click */, OnClickId, VTS_NONE)
	ON_EVENT(CGlassDataChange, IDC_OK, -600 /* Click */, OnClickOk, VTS_NONE)
	ON_EVENT(CGlassDataChange, IDC_CANCEL, -600 /* Click */, OnClickCancel, VTS_NONE)
	ON_EVENT(CGlassDataChange, IDC_SEL1, -600 /* Click */, OnClickSel1, VTS_NONE)
	ON_EVENT(CGlassDataChange, IDC_SEL2, -600 /* Click */, OnClickSel2, VTS_NONE)
	ON_EVENT(CGlassDataChange, IDC_GLS_CODE, -600 /* Click */, OnClickGlsCode, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


BOOL CGlassDataChange::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString str;

	GetDlgItem(IDC_LABEL49)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_LABEL54)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_LOT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SLOT)->ShowWindow(SW_HIDE);

	SelectLanguage();
	
	//str.Format("%)
	m_labelHostRecipeNo.SetCaption(CIM_Information.Recipe_Number);
//	m_labelLotNo.SetCaption(CIM_Information.Lot_number);
//	m_labelSlotNo.SetCaption(CIM_Information.Slot_number);
	m_labelGlsCode.SetCaption(CIM_Information.LotSlot_Dec);
	m_labelCode.SetCaption(CIM_Information.Glass_Judge_ASC);
	m_labelGlsID.SetCaption(CIM_Information.GlassID_ASC);

	if(g_bGlassDataChange)
	{		
		GetDlgItem(IDC_SEL1)->EnableWindow(FALSE);
		GetDlgItem(IDC_SEL2)->EnableWindow(FALSE);		
		
	}
	else
	{		
		GetDlgItem(IDC_RECIEPE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CODE)->EnableWindow(FALSE);	
		if(m_nSelNo==0)
		{
//			GetDlgItem(IDC_LOT)->EnableWindow(FALSE);
//			GetDlgItem(IDC_SLOT)->EnableWindow(FALSE);
			GetDlgItem(IDC_GLS_CODE)->EnableWindow(FALSE);
			m_ctrlSel1.SetBackColor(LIGHTGREEN);
		}
		else GetDlgItem(IDC_ID)->EnableWindow(FALSE);		
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGlassDataChange::OnClickReciepe() 
{
	Use_TK(m_labelHostRecipeNo, 999, 0, 0,0);	
	
}

void CGlassDataChange::OnClickLot() 
{
	Use_TK(m_labelLotNo, 999, 0, 0,0);	
}

void CGlassDataChange::OnClickSlot() 
{
	Use_TK(m_labelSlotNo, 999, 0, 0,0);	
}

void CGlassDataChange::OnClickGlsCode() 
{
	Use_TK(m_labelGlsCode, 99999, 0, 0,0);	
	
}

void CGlassDataChange::OnClickCode() 
{
	Use_STR(m_labelCode,CPoint(100,100));
}

void CGlassDataChange::OnClickId() 
{
	Use_STR(m_labelGlsID,CPoint(100,100));
}

void CGlassDataChange::OnClickOk() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CString temp_str,str;
	int n;


	if(g_bGlassDataChange)
	{	
		//나중에 기능 추가...//
/*		pView->SaveHistoryLog(0,"//////////////////////////////////////////////////");
		pView->SaveHistoryLog(0,"<< GLS DATA 변경 보고 >>");
		str.Format("변경전 Recipe(DEC):%s", CIM_Information.Recipe_No_Dec);
		pView->SaveHistoryLog(0,str);
		str.Format("변경전 Glass Code(DEC):%s (HEX):%s", CIM_Information.LotSlot_Dec, CIM_Information.LotSlot_Hex);
		pView->SaveHistoryLog(0,str);
		str.Format("변경전 Glass Judge(str):%s", CIM_Information.Glass_Judge_ASC);
		pView->SaveHistoryLog(0,str);
		str.Format("변경전 Glass ID(str):%s", CIM_Information.GlassID_ASC);
		pView->SaveHistoryLog(0,str);
*/

		temp_str.Format("%04x",atoi(m_labelHostRecipeNo.GetCaption()));
		CIM_Information.Recipe_No = temp_str;
		n = pView->m_pDevice ->HexConvert(temp_str.GetAt(1))*256+pView->m_pDevice ->HexConvert(temp_str.GetAt(2))*16 + pView->m_pDevice->HexConvert(temp_str.GetAt(3));
		str.Format("%03d", n);
		CIM_Information.Recipe_Number = str;

//		temp_str.Format("%02x",atoi(m_labelLotNo.GetCaption()));
//		CIM_Information.LotSlot_No = temp_str;
//		str.Format("%d", pView->m_pDevice->HexConvert(temp_str.GetAt(0))*16 + pView->m_pDevice->HexConvert(temp_str.GetAt(1)));
//		CIM_Information.Lot_number = str;

//		temp_str.Format("%02x",atoi(m_labelSlotNo.GetCaption()));
//		CIM_Information.LotSlot_No += temp_str;	
//		str.Format("%d", pView->m_pDevice->HexConvert(temp_str.GetAt(0))*16 + pView->m_pDevice->HexConvert(temp_str.GetAt(1)));
//		CIM_Information.Slot_number = str;

		temp_str.Format("%04x",atoi(m_labelGlsCode.GetCaption()));
		CIM_Information.LotSlot_No = temp_str;
		temp_str.Format("%d",atoi(m_labelGlsCode.GetCaption()));		
		CIM_Information.LotSlot_Dec = temp_str;	
		
		//Lot용
		str.Format("%d", pView->m_pDevice->HexConvert(CIM_Information.LotSlot_No.GetAt(0))*16 + pView->m_pDevice->HexConvert(CIM_Information.LotSlot_No.GetAt(1)));
		CIM_Information.Lot_number = str;
		//Slot용		
		str.Format("%d", pView->m_pDevice->HexConvert(CIM_Information.LotSlot_No.GetAt(2))*16 + pView->m_pDevice->HexConvert(CIM_Information.LotSlot_No.GetAt(3)));
		CIM_Information.Slot_number = str;

		

		CIM_Information.Glass_Judge_ASC = m_labelCode.GetCaption();
		CIM_Information.Glass_Judge = pView->m_pDevice->MakeHexDatatoPLC(CIM_Information.Glass_Judge_ASC,2);

		CIM_Information.GlassID_ASC = m_labelGlsID.GetCaption();
		CIM_Information.Glass_ID = pView->m_pDevice->MakeHexDatatoPLC(CIM_Information.GlassID_ASC,16);	

		//나중에 기능 추가...//
/*		str.Format("변경후 Recipe(DEC):%s", CIM_Information.Recipe_No_Dec);
		pView->SaveHistoryLog(0,str);
		str.Format("변경후 Glass Code(DEC):%s (HEX):%s", CIM_Information.LotSlot_Dec, CIM_Information.LotSlot_Hex);
		pView->SaveHistoryLog(0,str);
		str.Format("변경후 Glass Judge(str):%s", CIM_Information.Glass_Judge_ASC);
		pView->SaveHistoryLog(0,str);
		str.Format("변경후 Glass ID(str):%s", CIM_Information.GlassID_ASC);
		pView->SaveHistoryLog(0,str);
		pView->SaveHistoryLog(0,"//////////////////////////////////////////////////");*/
	}
	else
	{
		if(m_nSelNo==0)
		{
			CIM_Information.GlassID_ASC = m_labelGlsID.GetCaption();
			CIM_Information.Glass_ID = pView->m_pDevice->MakeHexDatatoPLC(CIM_Information.GlassID_ASC,16);

			//나중에 기능 추가...//
/*			pView->SaveHistoryLog(0,"//////////////////////////////////////////////////");
			pView->SaveHistoryLog(0,"<< 수동으로 GLS DATA 요구 >>");			
			str.Format("요구 Glass ID(str):%s\n\n", CIM_Information.GlassID_ASC);
			pView->SaveHistoryLog(0,str);*/
		}
		else
		{
/*			temp_str.Format("%02x",atoi(m_labelLotNo.GetCaption()));
			CIM_Information.LotSlot_No = temp_str;
			str.Format("%d", pView->m_pDevice->HexConvert(temp_str.GetAt(0))*16 + pView->m_pDevice->HexConvert(temp_str.GetAt(1)));
			CIM_Information.Lot_number = str;

			temp_str.Format("%02x",atoi(m_labelSlotNo.GetCaption()));
			CIM_Information.LotSlot_No += temp_str;	
			str.Format("%d", pView->m_pDevice->HexConvert(temp_str.GetAt(0))*16 + pView->m_pDevice->HexConvert(temp_str.GetAt(1)));
			CIM_Information.Slot_number = str;*/

			temp_str.Format("%04x",atoi(m_labelGlsCode.GetCaption()));
			CIM_Information.LotSlot_No = temp_str;
			temp_str.Format("%d",atoi(m_labelGlsCode.GetCaption()));
			CIM_Information.LotSlot_Dec = temp_str;

			//나중에 기능 추가...//
/*			pView->SaveHistoryLog(0,"//////////////////////////////////////////////////");
			pView->SaveHistoryLog(0,"<< 수동으로 GLS DATA 요구 >>");			
			str.Format("요구 Glass Code(DEC):%s (HEX):%s\n\n", CIM_Information.LotSlot_Dec,CIM_Information.LotSlot_Hex);
			pView->SaveHistoryLog(0,str);	*/		
			
		}
	}	
	EndDialog(IDOK);		
}

void CGlassDataChange::OnClickCancel() 
{	
	EndDialog(IDCANCEL);
}

void CGlassDataChange::OnClickSel1() 
{
	// TODO: Add your control notification handler code here
	if(m_ctrlSel1.GetBackColor()==LIGHTGREEN)
	{
//		GetDlgItem(IDC_LOT)->EnableWindow(TRUE);
//		GetDlgItem(IDC_SLOT)->EnableWindow(TRUE);	
		GetDlgItem(IDC_GLS_CODE)->EnableWindow(TRUE);	
		GetDlgItem(IDC_ID)->EnableWindow(FALSE);

		m_ctrlSel1.SetBackColor(0x8000000F);
		m_ctrlSel2.SetBackColor(LIGHTGREEN);
		m_nSelNo=1;
	}
	else
	{
//		GetDlgItem(IDC_LOT)->EnableWindow(FALSE);
//		GetDlgItem(IDC_SLOT)->EnableWindow(FALSE);	
		GetDlgItem(IDC_GLS_CODE)->EnableWindow(FALSE);	
		GetDlgItem(IDC_ID)->EnableWindow(TRUE);
		
		m_ctrlSel1.SetBackColor(LIGHTGREEN);
		m_ctrlSel2.SetBackColor(0x8000000F);
		m_nSelNo=0;
	}		
}

void CGlassDataChange::OnClickSel2() 
{
	// TODO: Add your control notification handler code here
	if(m_ctrlSel2.GetBackColor()==LIGHTGREEN)
	{
//		GetDlgItem(IDC_LOT)->EnableWindow(FALSE);
//		GetDlgItem(IDC_SLOT)->EnableWindow(FALSE);	
		GetDlgItem(IDC_GLS_CODE)->EnableWindow(FALSE);	
		GetDlgItem(IDC_ID)->EnableWindow(TRUE);

		m_ctrlSel1.SetBackColor(LIGHTGREEN);
		m_ctrlSel2.SetBackColor(0x8000000F);
		m_nSelNo=0;		
	}
	else
	{
//		GetDlgItem(IDC_LOT)->EnableWindow(TRUE);
//		GetDlgItem(IDC_SLOT)->EnableWindow(TRUE);	
		GetDlgItem(IDC_GLS_CODE)->EnableWindow(TRUE);	
		GetDlgItem(IDC_ID)->EnableWindow(FALSE);
		
		m_ctrlSel1.SetBackColor(0x8000000F);
		m_ctrlSel2.SetBackColor(LIGHTGREEN);
		m_nSelNo=1;
		
	}	
}



void CGlassDataChange::SelectLanguage()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(pView->m_nLanguage == 0)	//ehji 140309
	{
		SetDlgItemText(IDC_SEL1,				_T("Glass ID로 요구"));
		SetDlgItemText(IDC_SEL2,				_T("         [ Lot / Slot ]로 요구"));
		SetDlgItemText(IDC_LABEL3,				_T("< Data를 입력하세요 >"));
		SetDlgItemText(IDC_STATIC,				_T("GLASS DATA 요구시 선택"));
	}

	else if(pView->m_nLanguage == 1)	//ehji 140309
	{
		SetDlgItemText(IDC_SEL1,				_T("Glass ID Ask"));
		SetDlgItemText(IDC_SEL2,				_T("         [ Lot / Slot ] Ask"));
		SetDlgItemText(IDC_LABEL3,				_T("< Input Data >"));
		SetDlgItemText(IDC_STATIC,				_T("GLASS DATA Ask and Select"));

	}

	else if(pView->m_nLanguage == 2)	//ehji 140309
	{
		SetDlgItemText(IDC_SEL1,				_T("要求Glass ID"));
		SetDlgItemText(IDC_SEL2,				_T("        要求[ Lot / Slot ]"));
		SetDlgItemText(IDC_LABEL3,				_T("< 輸入Data >"));
		SetDlgItemText(IDC_STATIC,				_T("要求GLASS DATA時 選擇"));

	}

}

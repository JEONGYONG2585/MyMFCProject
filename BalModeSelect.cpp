// BalModeSelect.cpp : implementation file
//


#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "BalModeSelect.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBalModeSelect dialog


CBalModeSelect::CBalModeSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CBalModeSelect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBalModeSelect)
	m_nBalMode = -1;
	m_nBalRifLevel = -1;
	m_nBal1_Type = -1;
	m_nBal2_Type = -1;
	m_nBal3_Type = -1;
	m_nBal4_Type = -1;
	m_nBal5_Type = -1;
	m_nBal6_Type = -1;
	m_nBal7_Type = -1;
	m_nBal8_Type = -1;
	//}}AFX_DATA_INIT
}

CBalModeSelect::~CBalModeSelect()
{
}

void CBalModeSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBalModeSelect)
	DDX_Control(pDX, IDC_CHECK_BAL8, m_ctrlUseBal8);
	DDX_Control(pDX, IDC_CHECK_BAL7, m_ctrlUseBal7);
	DDX_Control(pDX, IDC_CHECK_BAL6, m_ctrlUseBal6);
	DDX_Control(pDX, IDC_CHECK_BAL5, m_ctrlUseBal5);
	DDX_Control(pDX, IDC_CHECK_BAL4, m_ctrlUseBal4);
	DDX_Control(pDX, IDC_CHECK_BAL3, m_ctrlUseBal3);
	DDX_Control(pDX, IDC_CHECK_BAL2, m_ctrlUseBal2);
	DDX_Control(pDX, IDC_CHECK_BAL1, m_ctrlUseBal1);
	DDX_Control(pDX, IDC_LEVEL1, m_ctrlBalRifLevel1);
	DDX_Control(pDX, IDC_LEVEL2, m_ctrlBalRifLevel2);
	DDX_Control(pDX, IDC_LEVEL3, m_ctrlBalRifLevel3);
	DDX_Radio(pDX, IDC_RADIO_BAL_NORMAL, m_nBalMode);
	DDX_Radio(pDX, IDC_LEVEL1, m_nBalRifLevel);
	DDX_Radio(pDX, IDC_RADIO_BAL11, m_nBal1_Type);
	DDX_Radio(pDX, IDC_RADIO_BAL21, m_nBal2_Type);
	DDX_Radio(pDX, IDC_RADIO_BAL31, m_nBal3_Type);
	DDX_Radio(pDX, IDC_RADIO_BAL41, m_nBal4_Type);
	DDX_Radio(pDX, IDC_RADIO_BAL51, m_nBal5_Type);
	DDX_Radio(pDX, IDC_RADIO_BAL61, m_nBal6_Type);
	DDX_Radio(pDX, IDC_RADIO_BAL71, m_nBal7_Type);
	DDX_Radio(pDX, IDC_RADIO_BAL81, m_nBal8_Type);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBalModeSelect, CDialog)
	//{{AFX_MSG_MAP(CBalModeSelect)
	ON_BN_CLICKED(IDC_RADIO_BAL_NORMAL, OnRadioBalNormal)
	ON_BN_CLICKED(IDC_RADIO_ABNORMAL, OnRadioAbnormal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBalModeSelect message handlers

BOOL CBalModeSelect::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SelectLanguage();

	// TODO: Add extra initialization here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	CString str;
	
	m_nPrevBalMode = m_nBalMode = ThBal[BALID1].nBalanceMode;
	m_nBalRifLevel = ThBal[BALID1].nBalanceRifLevel;
	if(m_nBalMode == 0)
	{
		m_ctrlBalRifLevel1.EnableWindow(false);
		m_ctrlBalRifLevel2.EnableWindow(false);
		m_ctrlBalRifLevel3.EnableWindow(false);
	}
	else
	{
		m_ctrlBalRifLevel1.EnableWindow(true);
		m_ctrlBalRifLevel2.EnableWindow(true);
		m_ctrlBalRifLevel3.EnableWindow(true);
	}

	GetDlgItem(IDC_CHECK_BAL1)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK_BAL2)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK_BAL3)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK_BAL4)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK_BAL5)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK_BAL6)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK_BAL7)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK_BAL8)->ShowWindow(FALSE);
	
// 	if(ThBal[BALID1].nBalanceUse==1)	
// 			m_ctrlUseBal1.SetCheck(TRUE);
// 	else	m_ctrlUseBal1.SetCheck(FALSE);
// 	if(ThBal[BALID2].nBalanceUse==1)	
// 			m_ctrlUseBal2.SetCheck(TRUE);
// 	else	m_ctrlUseBal2.SetCheck(FALSE);
// 	if(ThBal[BALID3].nBalanceUse==1)	
// 			m_ctrlUseBal3.SetCheck(TRUE);
// 	else	m_ctrlUseBal3.SetCheck(FALSE);
// 	if(ThBal[BALID4].nBalanceUse==1)
// 			m_ctrlUseBal4.SetCheck(TRUE);
// 	else	m_ctrlUseBal4.SetCheck(FALSE);
// 	if(ThBal[BALID5].nBalanceUse==1)
// 			m_ctrlUseBal5.SetCheck(TRUE);
// 	else	m_ctrlUseBal5.SetCheck(FALSE);
// 	if(ThBal[BALID6].nBalanceUse==1)
// 			m_ctrlUseBal6.SetCheck(TRUE);
// 	else	m_ctrlUseBal6.SetCheck(FALSE);
// 	if(ThBal[BALID7].nBalanceUse==1)
// 			m_ctrlUseBal7.SetCheck(TRUE);
// 	else	m_ctrlUseBal7.SetCheck(FALSE);
// 	if(ThBal[BALID8].nBalanceUse==1)
// 			m_ctrlUseBal8.SetCheck(TRUE);
// 	else	m_ctrlUseBal8.SetCheck(FALSE);

	m_nBal1_Type = ThBal[BALID1].nBalanceType;
	m_nBal2_Type = ThBal[BALID2].nBalanceType;
	m_nBal3_Type = ThBal[BALID3].nBalanceType;
	m_nBal4_Type = ThBal[BALID4].nBalanceType;
	m_nBal5_Type = ThBal[BALID5].nBalanceType;
	m_nBal6_Type = ThBal[BALID6].nBalanceType;
	m_nBal7_Type = ThBal[BALID7].nBalanceType;
	m_nBal8_Type = ThBal[BALID8].nBalanceType;


	UpdateData(FALSE);	// È­¸é¿¡ »Ñ·ÁÁÖ´Â °Í.
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_EVENTSINK_MAP(CBalModeSelect, CDialog)
    //{{AFX_EVENTSINK_MAP(CBalModeSelect)
	ON_EVENT(CBalModeSelect, IDC_EXE, -600 /* Click */, OnClickExe, VTS_NONE)
	ON_EVENT(CBalModeSelect, IDC_CANCEL, -600 /* Click */, OnClickCancel, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CBalModeSelect::OnClickExe() 
{
	// TODO: Add your control notification handler code here
	//µ¥ÀÌÅÍ¸¦ ÀúÀåÇÑÈÄ À©µµ¿ì¸¦ Á¾·áÇÑ´Ù.
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	UpdateData(true);

	ThBal[BALID1].nBalanceMode = ThBal[BALID2].nBalanceMode = ThBal[BALID3].nBalanceMode = ThBal[BALID4].nBalanceMode = ThBal[BALID5].nBalanceMode = ThBal[BALID6].nBalanceMode = ThBal[BALID7].nBalanceMode = ThBal[BALID8].nBalanceMode = m_nBalMode;
	ThBal[BALID1].nBalanceRifLevel = ThBal[BALID2].nBalanceRifLevel = ThBal[BALID3].nBalanceRifLevel = ThBal[BALID4].nBalanceRifLevel = ThBal[BALID5].nBalanceRifLevel = ThBal[BALID6].nBalanceRifLevel = ThBal[BALID7].nBalanceRifLevel = ThBal[BALID8].nBalanceRifLevel = m_nBalRifLevel;
	if(m_ctrlUseBal1.GetCheck())	
			ThBal[BALID1].nBalanceUse = 1;
	else	ThBal[BALID1].nBalanceUse = 0;
	if(m_ctrlUseBal2.GetCheck())	
			ThBal[BALID2].nBalanceUse = 1;
	else	ThBal[BALID2].nBalanceUse = 0;
	if(m_ctrlUseBal3.GetCheck())	
			ThBal[BALID3].nBalanceUse = 1;
	else	ThBal[BALID3].nBalanceUse = 0;
	if(m_ctrlUseBal4.GetCheck())	
			ThBal[BALID4].nBalanceUse = 1;
	else	ThBal[BALID4].nBalanceUse = 0;
	if(m_ctrlUseBal5.GetCheck())	
			ThBal[BALID5].nBalanceUse = 1;
	else	ThBal[BALID5].nBalanceUse = 0;
	if(m_ctrlUseBal6.GetCheck())	
			ThBal[BALID6].nBalanceUse = 1;
	else	ThBal[BALID6].nBalanceUse = 0;
	if(m_ctrlUseBal7.GetCheck())	
			ThBal[BALID7].nBalanceUse = 1;
	else	ThBal[BALID7].nBalanceUse = 0;
	if(m_ctrlUseBal8.GetCheck())	
			ThBal[BALID8].nBalanceUse = 1;
	else	ThBal[BALID8].nBalanceUse = 0;
	
	ThBal[BALID1].nBalanceType = m_nBal1_Type;
	ThBal[BALID2].nBalanceType = m_nBal2_Type;
	ThBal[BALID3].nBalanceType = m_nBal3_Type;
	ThBal[BALID4].nBalanceType = m_nBal4_Type;
	ThBal[BALID5].nBalanceType = m_nBal5_Type;
	ThBal[BALID6].nBalanceType = m_nBal6_Type;
	ThBal[BALID7].nBalanceType = m_nBal7_Type;
	ThBal[BALID8].nBalanceType = m_nBal8_Type;



	CString strPath;
	FILE *fp;

	strPath.Format("%s\\BalanceMode.dat", pDoc->m_strDataPath);

	fp = fopen(strPath, "wt");

	fprintf(fp, "%d\n", ThBal[BALID1].nBalanceMode);
	fprintf(fp, "%d\n", ThBal[BALID1].nBalanceRifLevel);
	for(int i=0;i<MAX_BAL;i++)
		fprintf(fp, "%d\n", ThBal[i].nBalanceUse);
	for(i=0;i<MAX_BAL;i++)
		fprintf(fp, "%d\n", ThBal[i].nBalanceType);
	
	fclose(fp);

	Sleep(1000);

	pView->BalanceModeSet();

//2010.03.03 by tskim
#if EQ
	if(ThBal[BALID1].nBalanceMode == 0)
	{
		for(i=1; i<=MAX_BAL; i++)
		{
			if(ThBal[i-1].nBalanceType == 1)
			{
				pView->SendData(i,"s3_");	// Á¤»ó ¸ðµå·Î ÀüÈ¯ÇÑ´Ù.
				Sleep(500);
			}
		}
	}
#endif

	EndDialog(IDOK);
}

void CBalModeSelect::OnClickCancel() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();	
}

void CBalModeSelect::OnRadioBalNormal() 
{
	// TODO: Add your control notification handler code here
	m_nBalMode = 0;
	m_ctrlBalRifLevel1.EnableWindow(false);
	m_ctrlBalRifLevel2.EnableWindow(false);
	m_ctrlBalRifLevel3.EnableWindow(false);

}

void CBalModeSelect::OnRadioAbnormal() 
{
	// TODO: Add your control notification handler code here
	m_nBalMode = 1;
	m_ctrlBalRifLevel1.EnableWindow(true);
	m_ctrlBalRifLevel2.EnableWindow(true);
	m_ctrlBalRifLevel3.EnableWindow(true);
}


void CBalModeSelect::SelectLanguage()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(pView->m_nLanguage == 0) //ehji 140309 .
	{
		//AdjustInital
		SetDlgItemText(IDC_RADIO_BAL_NORMAL,			_T("Á¤»ó ¸ðµå ( BALANCE ¾ÈÁ¤È­ »óÅÂ ) "));
		SetDlgItemText(IDC_RADIO_ABNORMAL,			_T(" ÀÌ»ó ¸ðµå ( BALANCE ºñ¾ÈÁ¤È­ »óÅÂ / ÁöÁø ¹ß»ý "));
		SetDlgItemText(IDC_EXE,			_T("Àû  ¿ë"));
		SetDlgItemText(IDC_CANCEL,			_T("Ãë  ¼Ò"));
		SetDlgItemText(IDC_LABEL_TITLE2,			_T("BALANCE ¼±ÅÃ"));
	}
	else if(pView->m_nLanguage == 1) //ehji 140309 .
	{
		//AdjustInital
		SetDlgItemText(IDC_RADIO_BAL_NORMAL,			_T("Normal mode ( BALANCE stability mode ) "));
		SetDlgItemText(IDC_RADIO_ABNORMAL,			_T("Adnormal mode ( BALANCE not stability mode / Earthquake"));
		SetDlgItemText(IDC_EXE,			_T("Apply"));
		SetDlgItemText(IDC_CANCEL,			_T("Cancel"));
		SetDlgItemText(IDC_LABEL_TITLE2,			_T("BALANCE Select"));
	}
	else if(pView->m_nLanguage == 2) //ehji 140309 .
	{
		//AdjustInital
		SetDlgItemText(IDC_RADIO_BAL_NORMAL,			_T("ïáßÈÙ¼ãÒ ( BALANCE è±ïÒßÒ÷¾ ) "));
		SetDlgItemText(IDC_RADIO_ABNORMAL,			_T(" ì¶ßÈÙ¼ãÒ ( BALANCE Þªè±ïÒßÒ÷¾ /Û¡ßæò¢òè "));
		SetDlgItemText(IDC_EXE,			_T("ëëéÄ"));
		SetDlgItemText(IDC_CANCEL,			_T("ö¢á¼"));
		SetDlgItemText(IDC_LABEL_TITLE2,			_T("BALANCE àÔ÷É"));
	}
}

// McStatusReport.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "McStatusReport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMcStatusReport dialog


CMcStatusReport::CMcStatusReport(CWnd* pParent /*=NULL*/)
	: CDialog(CMcStatusReport::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMcStatusReport)
	m_nStatusCode = -1;
	//}}AFX_DATA_INIT
}


void CMcStatusReport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMcStatusReport)
	DDX_Radio(pDX, IDC_MC_SEL1, m_nStatusCode);
	DDX_Control(pDX, IDC_LABMODEL, m_ctrlLabModel);
	DDX_Control(pDX, IDC_LABCLEAN, m_ctrlLabClean);
	DDX_Control(pDX, IDC_LABMATERIAL, m_ctrlLabMaterial);
	DDX_Control(pDX, IDC_CMD_SAVE, m_ctrlCmdSave);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMcStatusReport, CDialog)
	//{{AFX_MSG_MAP(CMcStatusReport)
	ON_BN_CLICKED(IDC_MC_SEL1, OnMcSel1)
	ON_BN_CLICKED(IDABORT, OnAbort)
	ON_BN_CLICKED(IDC_MC_SEL2, OnMcSel1)
	ON_BN_CLICKED(IDC_MC_SEL3, OnMcSel1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMcStatusReport message handlers

void CMcStatusReport::OnMcSel1() 
{
	// TODO: Add your control notification handler code here
	
}


void CMcStatusReport::OnAbort() 
{
	CDialog::EndDialog(IDABORT);	
	
}
#include "MainFrm.h"
#include "P8CA_LcDispView.h"
#include "P8CA_LcDispDoc.h"
#include <fstream.h>
extern double g_dMCTimer[3]; // by ckh 2009.03.24 Àåºñ »óÅÂ °ü·Ã Å¸ÀÌ¸Ó ½Ã°£. 

BOOL CMcStatusReport::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	CP8CA_LcDispView *pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CString str="";

	SelectLanguage();

	ReadData();
	str.Format("%.0f",g_dMCTimer[0]);
	m_ctrlLabModel.SetCaption(str);
	str.Format("%.0f",g_dMCTimer[1]);
	m_ctrlLabClean.SetCaption(str);
	str.Format("%.0f",g_dMCTimer[2]);
	m_ctrlLabMaterial.SetCaption(str);

	//¿£Áö´Ï¾î ¿äÃ»¿¡ ÀÇÇØ ÀÏ´Ü "cleaning, Àç·á ±³Ã¼"´Â ±â´É Áß´Ü...//by shin//2009.03.30//
	//¿£Áö´Ï¾î ¿äÃ»¿¡ ÀÇÇØ "cleaning, Àç·á ±³Ã¼"´Â ±â´É º¹¿ø...//by shin//2009.05.12//

//	GetDlgItem(IDC_MC_SEL2)->MoveWindow(0,0,0,0);
//	GetDlgItem(IDC_MC_SEL3)->MoveWindow(0,0,0,0);
//	GetDlgItem(IDC_MC_SEL2 )->EnableWindow(FALSE);
//	GetDlgItem(IDC_MC_SEL3 )->EnableWindow(FALSE);

	
	if(pDoc->m_bIsEngineerMode==FALSE)
	{
		m_ctrlLabModel.EnableWindow(FALSE);
		m_ctrlLabClean.EnableWindow(FALSE);
		m_ctrlLabMaterial.EnableWindow(FALSE);
		m_ctrlCmdSave.EnableWindow(FALSE);
	}
	
	if(pView->m_nMachineStatus !=-1)
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		GetDlgItem(IDC_MC_SEL1 )->EnableWindow(FALSE);
		GetDlgItem(IDC_MC_SEL2 )->EnableWindow(FALSE);
		GetDlgItem(IDC_MC_SEL3 )->EnableWindow(FALSE);

		m_ctrlLabModel.EnableWindow(FALSE);
		m_ctrlLabClean.EnableWindow(FALSE);
		m_ctrlLabMaterial.EnableWindow(FALSE);
		m_ctrlCmdSave.EnableWindow(FALSE);

	}

	//by shin//2009.08.31//MC °ü·Ã TAS Ãß°¡...//
	//MC È­¸é ÀüÈ¯...//
	pView->WriteTasMCData(TAS_MC, 0, BIT_ON); 
	Sleep(200);
	pView->WriteTasMCData(TAS_MC, 0, BIT_OFF); 

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_EVENTSINK_MAP(CMcStatusReport, CDialog)
    //{{AFX_EVENTSINK_MAP(CMcStatusReport)
	ON_EVENT(CMcStatusReport, IDC_LABMODEL, -600 /* Click */, OnClickLabmodel, VTS_NONE)
	ON_EVENT(CMcStatusReport, IDC_LABMATERIAL, -600 /* Click */, OnClickLabmaterial, VTS_NONE)
	ON_EVENT(CMcStatusReport, IDC_LABCLEAN, -600 /* Click */, OnClickLabclean, VTS_NONE)
	ON_EVENT(CMcStatusReport, IDC_CMD_SAVE, -600 /* Click */, OnClickCmdSave, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CMcStatusReport::OnClickLabmodel() 
{
	Use_TK(m_ctrlLabModel, 999, 0, 410,270);	
	
}

void CMcStatusReport::OnClickLabmaterial() 
{
	Use_TK(m_ctrlLabMaterial, 999, 0, 410,270);	
	
}

void CMcStatusReport::OnClickLabclean() 
{
	Use_TK(m_ctrlLabClean, 999, 0, 410,270);	
	
}


void CMcStatusReport::OnClickCmdSave() 
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();
	FILE *fp;
	CString strPathName;

	strPathName.Format("%s\\MCTimer.dat", pDoc->m_strDataPath);
	fp = fopen((char *)(LPCSTR)strPathName, "wt");

	if(fp==NULL) return ;
	else
	{
		fprintf(fp, "Model_Change\t%.1f\n",atof(m_ctrlLabModel.GetCaption()));
		fprintf(fp, "Cleaning\t%.1f\n",atof(m_ctrlLabClean.GetCaption()));
		fprintf(fp, "Material_Change\t%.1f\n",atof(m_ctrlLabMaterial.GetCaption()));
		fclose(fp);

		g_dMCTimer[0] = atof(m_ctrlLabModel.GetCaption());
		g_dMCTimer[1] = atof(m_ctrlLabClean.GetCaption());
		g_dMCTimer[2] = atof(m_ctrlLabMaterial.GetCaption());
	}
}
BOOL CMcStatusReport::ReadData()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	ifstream fi;
	CString strPathName;
	char ch[256];
	strPathName.Format("%s\\MCTimer.dat", pDoc->m_strDataPath);
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);

	if(fi.is_open())
	{
		for(int i=0 ;i<MAX_MCSTATUS;i++)
		{
			fi >> ch;
			fi >> g_dMCTimer[i];
		}
	}
	else
	{
		g_dMCTimer[0] = 20;
		g_dMCTimer[1] = 999;
		g_dMCTimer[2] = 20;
		return FALSE;
	}

	fi.close();
	return TRUE;
}

void CMcStatusReport::SelectLanguage()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(pView->m_nLanguage == 0)	//ehji 140309
	{
		SetDlgItemText(IDC_MC_SEL1,				_T("Model ±³È¯"));
		SetDlgItemText(IDC_MC_SEL3,				_T("Àç·á ±³Ã¼"));
		SetDlgItemText(IDC_LABEL37,				_T("Model ±³È¯ "));
		SetDlgItemText(IDC_LABEL59,				_T("Àç·á ±³Ã¼ "));
		SetDlgItemText(IDOK,				_T("¼³Á¤ "));
		SetDlgItemText(IDCANCEL,				_T("ÇØÁ¦"));
		SetDlgItemText(IDABORT,				_T("ÀÌÀü  È­¸é"));
		SetDlgItemText(IDC_LABEL1,				_T("  MC »óÅÂ ¼³Á¤"));
		SetDlgItemText(IDC_LABEL62,				_T("ºÐ"));
		SetDlgItemText(IDC_LABEL63,				_T("ºÐ"));
		SetDlgItemText(IDC_LABEL64,				_T("ºÐ"));

	}
	else if(pView->m_nLanguage == 1)	//ehji 140309
	{
		SetDlgItemText(IDC_MC_SEL1,				_T("Model Change"));
		SetDlgItemText(IDC_MC_SEL3,				_T("Material Change"));
		SetDlgItemText(IDC_LABEL37,				_T("Model Change "));
		SetDlgItemText(IDC_LABEL59,				_T("Material Change "));
		SetDlgItemText(IDOK,				_T("Set "));
		SetDlgItemText(IDCANCEL,				_T("Cancel"));
		SetDlgItemText(IDABORT,				_T("Before"));
		SetDlgItemText(IDC_LABEL1,				_T("  MC Stage Set"));
		SetDlgItemText(IDC_LABEL62,				_T("Minute"));
		SetDlgItemText(IDC_LABEL63,				_T("Minute"));
		SetDlgItemText(IDC_LABEL64,				_T("Minute"));
	}
	else if(pView->m_nLanguage == 2)	//ehji 140309
	{
		SetDlgItemText(IDC_MC_SEL1,				_T("Model Îßüµ"));
		SetDlgItemText(IDC_MC_SEL3,				_T("î§ÖùÌÚüµ"));
		SetDlgItemText(IDC_LABEL37,				_T("Model Îßüµ "));
		SetDlgItemText(IDC_LABEL59,				_T("î§ÖùÌÚüµ "));
		SetDlgItemText(IDOK,				_T("àâïÒ "));
		SetDlgItemText(IDCANCEL,				_T("ú°ð¶"));
		SetDlgItemText(IDABORT,				_T("Ú÷üÞ"));
		SetDlgItemText(IDC_LABEL1,				_T("  MC àâïÒ "));
		SetDlgItemText(IDC_LABEL62,				_T("ÝÂ"));
		SetDlgItemText(IDC_LABEL63,				_T("ÝÂ"));
		SetDlgItemText(IDC_LABEL64,				_T("ÝÂ"));
	}
}

// PanelInformInput.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "PanelInformInput.h"

#include "P8CA_LcDispView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPanelInformInput dialog


CPanelInformInput::CPanelInformInput(CWnd* pParent /*=NULL*/)
	: CDialog(CPanelInformInput::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPanelInformInput)
	m_nPanelValueTFT = -1;
	m_nPanelValueCF = -1;
	//}}AFX_DATA_INIT
}

CPanelInformInput::CPanelInformInput(int nIndex, char strTFT, char strCF, CWnd* pParent /*=NULL*/)
	: CDialog(CPanelInformInput::IDD, NULL)
{
	m_nIndex = nIndex;
	m_strTFT = strTFT;
	m_strCF = strCF;
}


void CPanelInformInput::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPanelInformInput)
	DDX_Radio(pDX, IDC_TFT_G, m_nPanelValueTFT);
	DDX_Radio(pDX, IDC_CF_G, m_nPanelValueCF);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPanelInformInput, CDialog)
	//{{AFX_MSG_MAP(CPanelInformInput)
	ON_BN_CLICKED(IDC_CF_G, OnCFClick)
	ON_BN_CLICKED(IDC_TFT_G, OnTFTClick)
	ON_BN_CLICKED(IDC_CF_Q, OnCFClick)
	ON_BN_CLICKED(IDC_CF_N, OnCFClick)
	ON_BN_CLICKED(IDC_CF_W, OnCFClick)
	ON_BN_CLICKED(IDC_TFT_N, OnTFTClick)
	ON_BN_CLICKED(IDC_TFT_Q, OnTFTClick)
	ON_BN_CLICKED(IDC_TFT_W, OnTFTClick)
	ON_BN_CLICKED(IDC_TFT_Y, OnTFTClick)
	ON_BN_CLICKED(IDC_CF_X, OnCFClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPanelInformInput message handlers

BOOL CPanelInformInput::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(m_strTFT=="G")		m_nPanelValueTFT = 0;
	else if(m_strTFT=="Q")	m_nPanelValueTFT = 1;
	else if(m_strTFT=="N")	m_nPanelValueTFT = 2;
	else if(m_strTFT=="W")	m_nPanelValueTFT = 3;
//2010.10.21 by tskim Judge code
	else if(m_strTFT=="Y")	m_nPanelValueTFT = 4;

	if(m_strCF=="G")		m_nPanelValueCF = 0;
	else if(m_strCF=="Q")	m_nPanelValueCF = 1;
	else if(m_strCF=="N")	m_nPanelValueCF = 2;
	else if(m_strCF=="W")	m_nPanelValueCF = 3;
//2010.10.21 by tskim Judge code
	else if(m_strCF=="X")	m_nPanelValueCF = 4;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// void CPanelInformInput::OnTFTClick() 
// {
// 	// TODO: Add your control notification handler code here
// 	UpdateData(TRUE);	
// }

void CPanelInformInput::OnCFClick() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);		
}

void CPanelInformInput::OnOK() 
{
	// TODO: Add extra validation here
	if(m_nIndex!=(-1))
	{
		switch(m_nPanelValueTFT)
		{
		case 0:	// G
			CIM_Information.Panel_NG_Mark[TFT][m_nIndex]=1;
			break;
		case 1: // Q
			CIM_Information.Panel_NG_Mark[TFT][m_nIndex]=9;
			break;
		case 2: // N
			CIM_Information.Panel_NG_Mark[TFT][m_nIndex]=2;
			break;
		case 3: // W
			CIM_Information.Panel_NG_Mark[TFT][m_nIndex]=17; //10
			break;
//2010.10.21 by tskim Judge code
		case 4: // Y
			CIM_Information.Panel_NG_Mark[TFT][m_nIndex]=24; //10
			break;
		}	

		switch(m_nPanelValueCF)
		{
		case 0: // G
			CIM_Information.Panel_NG_Mark[CF][m_nIndex]=1;
			break;
		case 1: // Q
			CIM_Information.Panel_NG_Mark[CF][m_nIndex]=9;
			break;
		case 2: // N
			CIM_Information.Panel_NG_Mark[CF][m_nIndex]=2;
			break;
		case 3: // W
			CIM_Information.Panel_NG_Mark[CF][m_nIndex]=17; //10
			break;
//2010.10.21 by tskim Judge code
		case 4: // X
			CIM_Information.Panel_NG_Mark[CF][m_nIndex]=23; //10
			break;
		}	
	}
	else
	{
		CString strTFTManual="", strCFManual="";
		switch(m_nPanelValueTFT)
		{
		case 0:	// G
			strTFTManual.Format(_T("G"));
			break;
		case 1: // Q
			strTFTManual.Format(_T("Q"));
			break;
		case 2: // N
			strTFTManual.Format(_T("N"));
			break;
		case 3: // W
			strTFTManual.Format(_T("W"));
			break;
//2010.10.21 by tskim Judge code
		case 4: // Y
			strTFTManual.Format(_T("Y"));
			break;
		}	

		switch(m_nPanelValueCF)
		{
		case 0: // G
			strCFManual.Format(_T("G"));
			break;
		case 1: // Q
			strCFManual.Format(_T("Q"));
			break;
		case 2: // N
			strCFManual.Format(_T("N"));
			break;
		case 3: // W
			strCFManual.Format(_T("W"));
			break;
//2010.10.21 by tskim Judge code
		case 4: // W
			strCFManual.Format(_T("X"));
			break;
		}	
		CIM_Information.Panel_Inform_AllManual=strTFTManual + strCFManual;	
	}
	CDialog::OnOK();
}

void CPanelInformInput::OnTFTClick() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);		
}

// PanelNGSet.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "PanelNGSet.h"

#include "stdafx.h"
#include "P8CA_LcDisp.h"

#include "P8CA_LcDispView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPanelNGSet dialog
CPanelNGSet::CPanelNGSet(CWnd* pParent /*=NULL*/)
	: CDialog(CPanelNGSet::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPanelNGSet)
	m_nDropJudge = -1;
	//}}AFX_DATA_INIT
	m_strTFTInform="";
	m_strCFInform ="";
	m_nValue = 0;
}

CPanelNGSet::CPanelNGSet(CString strTFT, CString strCF, CWnd* pParent /*=NULL*/)
	: CDialog(CPanelNGSet::IDD, NULL)
{
	m_strTFTInform = strTFT;
	m_strCFInform = strCF;
}

CPanelNGSet::CPanelNGSet(CString strTFT, CString strCF, int Value, CWnd* pParent/* =NULL*/)
	: CDialog(CPanelNGSet::IDD, NULL)
{
	m_strTFTInform = strTFT;
	m_strCFInform = strCF;
	m_nValue = Value;
}


void CPanelNGSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPanelNGSet)
	DDX_Control(pDX, IDC_LABEL_TFT_DISP, m_ctrlTFTInform);
	DDX_Control(pDX, IDC_LABEL_CF_DISP, m_ctrlCFInform);
	DDX_Radio(pDX, IDC_RADIO_ON, m_nDropJudge);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPanelNGSet, CDialog)
	//{{AFX_MSG_MAP(CPanelNGSet)
	ON_BN_CLICKED(IDC_RADIO_ON, OnRadioOn)
	ON_BN_CLICKED(IDC_RADIO_OFF, OnRadioOff)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPanelNGSet message handlers

BOOL CPanelNGSet::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_ctrlTFTInform.SetWindowText(m_strTFTInform);
	m_ctrlCFInform.SetWindowText(m_strCFInform);
	m_nDropJudge = 1 - m_nValue;
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelNGSet::OnOK() 
{
	// TODO: Add extra validation here
	int Row=0, Col=0;

	if(m_strTFTInform=="G")	Col = 1;
	else if(m_strTFTInform=="Q")	Col = 2;
	else if(m_strTFTInform=="N")	Col = 3;
	else if(m_strTFTInform=="W")	Col = 4;
	else if(m_strTFTInform=="Y")	Col = 5;

	if(m_strCFInform=="G")	Row = 1;
	else if(m_strCFInform=="Q")	Row = 2;
	else if(m_strCFInform=="N")	Row = 3;
	else if(m_strCFInform=="W")	Row = 4;
	else if(m_strCFInform=="X")	Row = 5;

	if(m_nDropJudge==0)	CIM_Information.Panel_NG_MarkSet[(Row-1)*5+(Col-1)]=1;
	else				CIM_Information.Panel_NG_MarkSet[(Row-1)*5+(Col-1)]=0;

	CDialog::OnOK();
}

void CPanelNGSet::OnRadioOn() 
{
	// TODO: Add your control notification handler code here
	m_nDropJudge=0;
}

void CPanelNGSet::OnRadioOff() 
{
	// TODO: Add your control notification handler code here
	m_nDropJudge=1;
}

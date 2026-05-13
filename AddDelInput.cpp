// AddDelInput.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"

#include "P8CA_LcDisp.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"
#include "P8CA_LcDispDoc.h"

#include "AddDelInput.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddDelInput dialog


CAddDelInput::CAddDelInput(CWnd* pParent /*=NULL*/)
	: CDialog(CAddDelInput::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddDelInput)
	//}}AFX_DATA_INIT
	
	m_fontCombo = new CFont;
	m_strOldData = _T("");
	m_strNewData = _T("");
}

CAddDelInput::CAddDelInput(CString strRecentString, CPoint pOrgPoint)
	: CDialog(CAddDelInput::IDD, NULL)
{
	m_fontCombo = new CFont;
	m_strOldData = strRecentString;
	m_pOrgPoint = pOrgPoint;
}

CAddDelInput::~CAddDelInput()
{
}

void CAddDelInput::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddDelInput)
	DDX_Control(pDX, IDC_SPIN1, m_ctrlSpinCH);
	DDX_Control(pDX, IDC_COMBO1, m_ctrlComCH);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddDelInput, CDialog)
	//{{AFX_MSG_MAP(CAddDelInput)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddDelInput message handlers
void CAddDelInput::AddText(CString str)
{
	UpdateData(FALSE);
}

void CAddDelInput::OnOK() 
{
	CString str1,str2="",strNewLevel="";
	if(m_ctrlComCH.GetCurSel()<99)
	{
		str1='P';
		str2.Format("%02d",(99-m_ctrlComCH.GetCurSel()));
	}
	else if(m_ctrlComCH.GetCurSel()==99)
	{
		str1='C';
		str2="00";
	}
	else if(m_ctrlComCH.GetCurSel()>99)
	{
		str1='M';
		str2.Format("%02d",(m_ctrlComCH.GetCurSel()-99));
	}

	strNewLevel=str1+str2;
	m_strNewData.Format(strNewLevel);
	UpdateData(FALSE);
//	
	CDialog::OnOK();
}

void CAddDelInput::OnCancel() 
{
	// TODO: Add extra cleanup here
	CDialog::OnCancel();
}

BOOL CAddDelInput::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc*)pFrame->GetActiveDocument();

	CString str=""; int index=0;

	SetWindowPos( NULL,m_pOrgPoint.x+30, m_pOrgPoint.y+50,
				0,0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER );

// TODO: Add extra initialization here

	m_dlg_brush.CreateSolidBrush(RGB(90,100,100));
//	m_strEditAdelLevel = m_strOldData;
	UpdateData(FALSE);

/////////////////////

	m_fontCombo->CreateFont(30,15,0,0,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,
	OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY, 
	DEFAULT_PITCH,"Tahoma");

	m_ctrlComCH.SetFont(m_fontCombo, TRUE);
/*
	m_ctrlComCH.SetItemHeight(-1, 35);

	m_ctrlComCH.SetFont(m_fontCombo, TRUE);
	m_ctrlComCH.SetItemHeight(-1, 35);
*/
	m_ctrlComCH.ResetContent();
	for(int i = 99; i >=-99 ; i--)
	{
		str=pDoc->LevelTostr(i);
		m_ctrlComCH.AddString((LPCSTR)str);
	}
	m_ctrlComCH.SetCurSel(99);

	CRect rect;
	m_ctrlSpinCH.GetWindowRect(rect);
	GetParent()->ScreenToClient(rect);
	rect.right = rect.left + 80;
	rect.bottom = rect.top + 180;
	m_ctrlSpinCH.MoveWindow(rect);
	//m_ctrlSpinCH.SetBuddy(&m_ctrlComCH);

	m_ctrlSpinCH.SetRange(0, 198);
	UpdateData(FALSE);
/////////////////////	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CAddDelInput::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
//	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	HBRUSH hbr=NULL;

	if(pWnd->m_hWnd == this->m_hWnd) hbr = HBRUSH(m_dlg_brush);
	else if(nCtlColor == CTLCOLOR_STATIC) {
		pDC->SetTextColor(RED);
		pDC->SetBkMode(TRANSPARENT);
//		pDC->SetBkColor(RGB(0, 0, 255));
		hbr = HBRUSH(m_dlg_brush);
	}
	else hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

BEGIN_EVENTSINK_MAP(CAddDelInput, CDialog)
    //{{AFX_EVENTSINK_MAP(CAddDelInput)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void CAddDelInput::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	delete m_fontCombo;
}

void CAddDelInput::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int nSpinDelta = pNMUpDown->iDelta;

	int Temp=m_ctrlComCH.GetCurSel();
	
	if(nSpinDelta == -1) //위로 진행할 경우 -1 발생한다
	{	
		m_ctrlComCH.SetCurSel(Temp+1);		
	}
	else if(nSpinDelta == 1)// +1이 발생한다
	{
		m_ctrlComCH.SetCurSel(Temp-1);	
	}		
	*pResult = 0;	
}



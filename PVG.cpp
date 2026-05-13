// PVG.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "PVG.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPVG dialog


CPVG::CPVG(CWnd* pParent /*=NULL*/)
	: CDialog(CPVG::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPVG)
	m_nTabSelect = -1;
	pFont = new CFont;
	m_nHead = 0;
	m_nTabSelectOld = -1;
	m_bPAxis_P=m_bPAxis_M=m_bGAxis_O=m_bGAxis_C=m_bSAxis_P=m_bSAxis_M=m_bKAxis_P=m_bKAxis_M=FALSE;
	//}}AFX_DATA_INIT
}

BEGIN_EVENTSINK_MAP(CPVG, CDialog)
    //{{AFX_EVENTSINK_MAP(CPVG)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CPVG::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPVG)
	DDX_Control(pDX, IDC_COMBO_HEAD, m_comboHead);
	DDX_Control(pDX, IDC_BITMAP_G_C, m_static_G_C);
	DDX_Control(pDX, IDC_BITMAP_G_O, m_static_G_O);
	DDX_Control(pDX, IDC_BITMAP_P_M, m_static_M);
	DDX_Control(pDX, IDC_BITMAP_P_P, m_static_P);
	DDX_Control(pDX, IDC_STATIC_BITMAP2, m_static_bitmap2);
	DDX_Control(pDX, IDC_STATIC_BITMAP, m_static_bitmap);
	DDX_Control(pDX, IDC_TAB_PVG, m_ctrlTabPVG);
	DDX_Control(pDX, IDC_LABEL_STATUS, m_ctrlLabel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPVG, CDialog)
	//{{AFX_MSG_MAP(CPVG)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_PVG, OnSelchangeTabPvg)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO_HEAD, OnSelchangeComboHead)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPVG message handlers

void CPVG::OnSelchangeTabPvg(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CWnd *pWnd = GetDlgItem(IDC_STATIC_BITMAP);
	pWnd->ShowWindow(SW_HIDE);
	m_nTabSelectOld = m_nTabSelect;
 	m_nTabSelect = m_ctrlTabPVG.GetCurSel();
	SelectView(m_nTabSelect);	
	pWnd->ShowWindow(SW_SHOW);	
	*pResult = 0;
}

BOOL CPVG::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString str;

	pFont->CreateFont(32,12,0,0,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,
	OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY, 
	DEFAULT_PITCH,"Tahoma");

	char *Index[] = {"Head1 ","Head2 ","COL ","sp","sp"};
	TC_ITEM item;
	item.mask = TCIF_TEXT;
	int i;
	for(i=0;i<3;i++)
	{
		item.pszText = Index[i];
		m_ctrlTabPVG.InsertItem(i,&item);
	}
	m_ctrlTabPVG.SetFont(pFont,TRUE);


	m_comboHead.SetFont(pFont, TRUE);
	m_comboHead.SetItemHeight(-1, 38);
	for(i=0;i<MAX_NOZZLE;i++)
	{
		m_comboHead.SetItemHeight(i,38);
		str.Format("HEAD%d",i+1);
		m_comboHead.AddString((LPCSTR)str);
	}
	
	m_nTabSelect = 0;
	m_nHead = 0;
	m_comboHead.SetCurSel(m_nHead);
	SelectView(m_nTabSelect);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPVG::SelectView(int nSel)
{
	m_static_M.ShowWindow(SW_HIDE);
	m_static_P.ShowWindow(SW_HIDE);
	m_static_G_C.ShowWindow(SW_HIDE);
	m_static_G_O.ShowWindow(SW_HIDE);
	m_ctrlBitmap.DeleteObject();
	m_ctrlBitmap2.DeleteObject();
	KillTimer(m_nTabSelectOld);
	switch(nSel)
	{
	case 0:
		Head1Status();
		m_comboHead.EnableWindow(TRUE);
		break;
	case 1:
		Head2Status();
		m_comboHead.EnableWindow(TRUE);
		break;
	case 2:
		ColStatus();
		m_comboHead.EnableWindow(FALSE);
		break;
	case 3:case 4:
		break;
	default :
		break;
	}
	LabelControl(nSel);
	SetTimer(nSel,500,NULL);
}

void CPVG::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	int iAxisNo =0; // by ckh 2009.03.09 추가 
	LONG AxisStatus = 0;// by ckh 2009.03.09 추가 
	m_bPAxis_P=m_bPAxis_M=m_bGAxis_O=m_bGAxis_C=m_bSAxis_P=m_bSAxis_M=m_bKAxis_P=m_bKAxis_M=FALSE;
	iAxisNo = m_nHead;
	m_strTitle.Empty();
	m_ctrlLabel.SetCaption(m_strTitle);
	switch(nIDEvent)
	{
	case 0://Head P& G
		if(iAxisNo<14)//P축
		{
			FAS_GetAxisStatus(1, iAxisNo+2, &AxisStatus);
			if(AxisStatus & LM_PLUS_DETECT)
			{
				m_bPAxis_P = TRUE;
			}
			if(AxisStatus & LM_MINUS_DETECT)
			{
				m_bPAxis_M = TRUE;
			}
		}
		else 	
		{
			FAS_GetAxisStatus(2, iAxisNo-12, &AxisStatus);
			if(AxisStatus & LM_PLUS_DETECT)
			{
				m_bPAxis_P = TRUE;
			}
			if(AxisStatus & LM_MINUS_DETECT)
			{
				m_bPAxis_M = TRUE;
			}
		}
		FAS_GetIoBit(3, false, GATE1_CLOSE_POSITION+iAxisNo, &m_bGAxis_C);
		FAS_GetIoBit(3, true, GATE1_OPEN_POSITION+iAxisNo, &m_bGAxis_O);
		Head1Status();
		break;
	case 1://Head S
		if(iAxisNo<12)//P축
		{
			FAS_GetAxisStatus(2, iAxisNo+4, &AxisStatus);
			if(AxisStatus & LM_PLUS_DETECT)
			{
				m_bSAxis_P = TRUE;
			}
			if(AxisStatus & LM_MINUS_DETECT)
			{
				m_bSAxis_M = TRUE;
			}
		}
		else 	
		{
			FAS_GetAxisStatus(3, iAxisNo-12, &AxisStatus);
			if(AxisStatus & LM_PLUS_DETECT)
			{
				m_bSAxis_P = TRUE;
			}
			if(AxisStatus & LM_MINUS_DETECT)
			{
				m_bSAxis_M = TRUE;
			}
		}
		Head2Status();
		break;
	case 2://Column
		FAS_GetAxisStatus(2, 0, &AxisStatus);
		if(AxisStatus & LM_PLUS_DETECT)
		{
			m_bKAxis_P = TRUE;
		}
		if(AxisStatus & LM_MINUS_DETECT)
		{
			m_bKAxis_M = TRUE;
		}
		ColStatus();
		break;
	default :
		break;
	}
	CDialog::OnTimer(nIDEvent);
}

void CPVG::Head1Status()//P_G
{
	CString strTemp1, strTemp2,strHead;
	strTemp1.Empty();
	strTemp2.Empty();
	strHead.Empty();
	m_strTitle.Empty();
	m_ctrlLabel.SetCaption(m_strTitle);
	m_ctrlBitmap.DeleteObject();
	m_ctrlBitmap2.DeleteObject();
	m_ctrlBitmap.LoadBitmap(IDB_BITMAP_HEAD);
	m_static_bitmap.SetBitmap(m_ctrlBitmap);
	m_static_bitmap2.SetWindowPos( NULL,0,0,0,0, SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOZORDER );

	m_ctrlBitmap_P.DeleteObject();
	m_ctrlBitmap_M.DeleteObject();
	m_ctrlBitmap_GC.DeleteObject();
	m_ctrlBitmap_GO.DeleteObject();
	strHead.Format("Head%02d ",m_nHead+1);
	if(!m_bPAxis_P)
		m_ctrlBitmap_P.LoadBitmap(IDB_BITMAP_CK_OFF);
	else
	{
		m_ctrlBitmap_P.LoadBitmap(IDB_BITMAP_CK_ON);
		strTemp1.Format("P축 : (+) 감지  ");
	}
	if(!m_bPAxis_M)
		m_ctrlBitmap_M.LoadBitmap(IDB_BITMAP_CK_OFF);
	else 
	{
		m_ctrlBitmap_M.LoadBitmap(IDB_BITMAP_CK_ON);
		strTemp1.Format("P축 : (-) 감지  ");
	}
	if(!m_bGAxis_C)
		m_ctrlBitmap_GC.LoadBitmap(IDB_BITMAP_CK_OFF);
	else
	{
		m_ctrlBitmap_GC.LoadBitmap(IDB_BITMAP_CK_ON);
		strTemp2.Format("G축 :  Close 감지");
	}
	if(!m_bGAxis_O)
		m_ctrlBitmap_GO.LoadBitmap(IDB_BITMAP_CK_OFF);
	else
	{
		m_ctrlBitmap_GO.LoadBitmap(IDB_BITMAP_CK_ON);
		strTemp2.Format("G축 : Open 감지");
	}
	m_strTitle = strHead + strTemp1 + strTemp2;
	m_ctrlLabel.SetCaption(m_strTitle);

	m_static_P.SetBitmap(m_ctrlBitmap_P);
	m_static_M.SetBitmap(m_ctrlBitmap_M);
	m_static_G_C.SetBitmap(m_ctrlBitmap_GC);
	m_static_G_O.SetBitmap(m_ctrlBitmap_GO);

//	m_static_P_P.ShowWindow(SW_SHOW);
//	m_static_P_M.ShowWindow(SW_SHOW);
//	m_static_G_C.ShowWindow(SW_SHOW);
//	m_static_G_O.ShowWindow(SW_SHOW);
	m_static_P.SetWindowPos( NULL,180,350,16,18, SWP_SHOWWINDOW | SWP_NOZORDER );
	m_static_M.SetWindowPos( NULL,180,300,16,18, SWP_SHOWWINDOW | SWP_NOZORDER );
	m_static_G_C.SetWindowPos( NULL,160,500,16,18, SWP_SHOWWINDOW | SWP_NOZORDER );
	m_static_G_O.SetWindowPos( NULL,220,460,16,18, SWP_SHOWWINDOW | SWP_NOZORDER );
}

void CPVG::Head2Status()//Head_S
{
	CString strTemp1, strTemp2,strHead;
	strTemp1.Empty();
	strTemp2.Empty();
	strHead.Empty();
	m_strTitle.Empty();
	m_ctrlLabel.SetCaption(m_strTitle);
	m_ctrlBitmap.DeleteObject();
	m_ctrlBitmap2.DeleteObject();
	m_ctrlBitmap.LoadBitmap(IDB_BITMAP_S_P);
	m_static_bitmap.SetBitmap(m_ctrlBitmap);
	m_ctrlBitmap2.LoadBitmap(IDB_BITMAP_S_M);
	m_static_bitmap2.SetBitmap(m_ctrlBitmap2);

	m_ctrlBitmap_P.DeleteObject();
	m_ctrlBitmap_M.DeleteObject();
	m_ctrlBitmap_GC.DeleteObject();
	m_ctrlBitmap_GO.DeleteObject();

	strHead.Format("Head%02d ",m_nHead+1);
	if(!m_bSAxis_P)
		m_ctrlBitmap_P.LoadBitmap(IDB_BITMAP_CK_OFF);
	else
	{
		m_ctrlBitmap_P.LoadBitmap(IDB_BITMAP_CK_ON);
		strTemp1.Format("S축 : (+) 감지  ");
	}
	if(!m_bSAxis_M)
		m_ctrlBitmap_M.LoadBitmap(IDB_BITMAP_CK_OFF);
	else 
	{
		m_ctrlBitmap_M.LoadBitmap(IDB_BITMAP_CK_ON);
		strTemp1.Format("S축(-) 감지  ");
	}
	m_strTitle = strHead + strTemp1 + strTemp2;
	m_ctrlLabel.SetCaption(m_strTitle);
	
	m_static_P.SetBitmap(m_ctrlBitmap_P);
	m_static_M.SetBitmap(m_ctrlBitmap_M);

	m_static_P.SetWindowPos( NULL,160,520,16,18, SWP_SHOWWINDOW | SWP_NOZORDER );
	m_static_M.SetWindowPos( NULL,650,500,16,18, SWP_SHOWWINDOW | SWP_NOZORDER );
}

void CPVG::ColStatus()
{
	CString strTemp1, strTemp2;
	strTemp1.Empty();
	strTemp2.Empty();
	m_strTitle.Empty();
	m_ctrlLabel.SetCaption(m_strTitle);
	m_ctrlBitmap.DeleteObject();
	m_ctrlBitmap2.DeleteObject();
	m_ctrlBitmap.LoadBitmap(IDB_BITMAP_K_P);
	m_static_bitmap.SetBitmap(m_ctrlBitmap);
	m_ctrlBitmap2.LoadBitmap(IDB_BITMAP_K_M);
	m_static_bitmap2.SetBitmap(m_ctrlBitmap2);

	m_ctrlBitmap_P.DeleteObject();
	m_ctrlBitmap_M.DeleteObject();
	m_ctrlBitmap_GC.DeleteObject();
	m_ctrlBitmap_GO.DeleteObject();

	if(!m_bKAxis_P)
		m_ctrlBitmap_P.LoadBitmap(IDB_BITMAP_CK_OFF);
	else
	{
		m_ctrlBitmap_P.LoadBitmap(IDB_BITMAP_CK_ON);
		strTemp1.Format("K축 : (+) 감지  ");
	}
	if(!m_bKAxis_M)
		m_ctrlBitmap_M.LoadBitmap(IDB_BITMAP_CK_OFF);
	else 
	{
		m_ctrlBitmap_M.LoadBitmap(IDB_BITMAP_CK_ON);
		strTemp1.Format("K축 : (-) 감지  ");
	}

	m_strTitle = strTemp1 + strTemp2;
	m_ctrlLabel.SetCaption(m_strTitle);

	m_static_P.SetBitmap(m_ctrlBitmap_P);
	m_static_M.SetBitmap(m_ctrlBitmap_M);

	m_static_P.SetWindowPos( NULL,760,380,16,18, SWP_SHOWWINDOW | SWP_NOZORDER );
	m_static_M.SetWindowPos( NULL,190,520,16,18, SWP_SHOWWINDOW | SWP_NOZORDER );
}

void CPVG::LabelControl(int nSel)
{
//	CWnd *pWnd1 = GetDlgItem(IDC_HEAD_P_P);
	switch(nSel)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	default:
		break;
	}
}
void CPVG::OnSelchangeComboHead() 
{
	// TODO: Add your control notification handler code here
	m_nHead = m_comboHead.GetCurSel();
}

void CPVG::OnOK() 
{
	// TODO: Add extra validation here
	delete pFont;
	CDialog::OnOK();
}

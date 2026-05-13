// ModuleBarTeaching.cpp : implementation file
//

#include "stdafx.h"
#include "p8ca_lcdisp.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"
#include "P8CA_LcDispView.h"
#include "ModuleBarTeaching.h"
#include "Lclib.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModuleBarTeaching dialog


CModuleBarTeaching::CModuleBarTeaching(CWnd* pParent /*=NULL*/)
	: CDialog(CModuleBarTeaching::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModuleBarTeaching)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CModuleBarTeaching::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModuleBarTeaching)
	DDX_Control(pDX, IDC_LABEL_MODULE_HEAD1, m_ctrlmodulehead1teach);
	DDX_Control(pDX, IDC_LABEL_MODULE_HEAD2, m_ctrlmodulehead2teach);
	DDX_Control(pDX, IDC_LABEL_MODULE_HEAD10, m_ctrlmodulehead10teach);
	DDX_Control(pDX, IDC_LABEL_MODULE_HEAD11, m_ctrlmodulehead11teach);
	DDX_Control(pDX, IDC_LABEL_MODULE_HEAD12, m_ctrlmodulehead12teach);
	DDX_Control(pDX, IDC_LABEL_MODULE_HEAD13, m_ctrlmodulehead13teach);
	DDX_Control(pDX, IDC_LABEL_MODULE_HEAD14, m_ctrlmodulehead14teach);
	DDX_Control(pDX, IDC_LABEL_MODULE_HEAD15, m_ctrlmodulehead15teach);
	DDX_Control(pDX, IDC_LABEL_MODULE_HEAD16, m_ctrlmodulehead16teach);
	DDX_Control(pDX, IDC_LABEL_MODULE_COL, m_ctrlmodulecolteach);
	DDX_Control(pDX, IDC_LABEL_MODULE_HEAD3, m_ctrlmodulehead3teach);
	DDX_Control(pDX, IDC_LABEL_MODULE_HEAD4, m_ctrlmodulehead4teach);
	DDX_Control(pDX, IDC_LABEL_MODULE_HEAD5, m_ctrlmodulehead5teach);
	DDX_Control(pDX, IDC_LABEL_MODULE_HEAD6, m_ctrlmodulehead6teach);
	DDX_Control(pDX, IDC_LABEL_MODULE_HEAD7, m_ctrlmodulehead7teach);
	DDX_Control(pDX, IDC_LABEL_MODULE_HEAD8, m_ctrlmodulehead8teach);
	DDX_Control(pDX, IDC_LABEL_MODULE_HEAD9, m_ctrlmodulehead9teach);
	DDX_Control(pDX, IDC_LABEL_MODULE_OFFSET, m_ctrlmoduleoffsetteach);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModuleBarTeaching, CDialog)
	//{{AFX_MSG_MAP(CModuleBarTeaching)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModuleBarTeaching message handlers

BEGIN_EVENTSINK_MAP(CModuleBarTeaching, CDialog)
    //{{AFX_EVENTSINK_MAP(CModuleBarTeaching)
	ON_EVENT(CModuleBarTeaching, IDC_LABEL_MODULE_HEAD1, -600 /* Click */, OnClickLabelModuleHead1, VTS_NONE)
	ON_EVENT(CModuleBarTeaching, IDC_LABEL_MODULE_HEAD10, -600 /* Click */, OnClickLabelModuleHead10, VTS_NONE)
	ON_EVENT(CModuleBarTeaching, IDC_LABEL_MODULE_HEAD11, -600 /* Click */, OnClickLabelModuleHead11, VTS_NONE)
	ON_EVENT(CModuleBarTeaching, IDC_LABEL_MODULE_HEAD12, -600 /* Click */, OnClickLabelModuleHead12, VTS_NONE)
	ON_EVENT(CModuleBarTeaching, IDC_LABEL_MODULE_HEAD13, -600 /* Click */, OnClickLabelModuleHead13, VTS_NONE)
	ON_EVENT(CModuleBarTeaching, IDC_LABEL_MODULE_HEAD14, -600 /* Click */, OnClickLabelModuleHead14, VTS_NONE)
	ON_EVENT(CModuleBarTeaching, IDC_LABEL_MODULE_HEAD15, -600 /* Click */, OnClickLabelModuleHead15, VTS_NONE)
	ON_EVENT(CModuleBarTeaching, IDC_LABEL_MODULE_HEAD16, -600 /* Click */, OnClickLabelModuleHead16, VTS_NONE)
	ON_EVENT(CModuleBarTeaching, IDC_LABEL_MODULE_HEAD2, -600 /* Click */, OnClickLabelModuleHead2, VTS_NONE)
	ON_EVENT(CModuleBarTeaching, IDC_LABEL_MODULE_HEAD3, -600 /* Click */, OnClickLabelModuleHead3, VTS_NONE)
	ON_EVENT(CModuleBarTeaching, IDC_LABEL_MODULE_HEAD4, -600 /* Click */, OnClickLabelModuleHead4, VTS_NONE)
	ON_EVENT(CModuleBarTeaching, IDC_LABEL_MODULE_HEAD5, -600 /* Click */, OnClickLabelModuleHead5, VTS_NONE)
	ON_EVENT(CModuleBarTeaching, IDC_LABEL_MODULE_HEAD6, -600 /* Click */, OnClickLabelModuleHead6, VTS_NONE)
	ON_EVENT(CModuleBarTeaching, IDC_LABEL_MODULE_HEAD7, -600 /* Click */, OnClickLabelModuleHead7, VTS_NONE)
	ON_EVENT(CModuleBarTeaching, IDC_LABEL_MODULE_HEAD8, -600 /* Click */, OnClickLabelModuleHead8, VTS_NONE)
	ON_EVENT(CModuleBarTeaching, IDC_LABEL_MODULE_HEAD9, -600 /* Click */, OnClickLabelModuleHead9, VTS_NONE)
	ON_EVENT(CModuleBarTeaching, IDC_LABEL_MODULE_OFFSET, -600 /* Click */, OnClickLabelModuleOffset, VTS_NONE)
	ON_EVENT(CModuleBarTeaching, IDC_SAVE_Module_Teach, -600 /* Click */, OnClickSAVEModuleTeach, VTS_NONE)
	ON_EVENT(CModuleBarTeaching, IDC_CALCEL_BTN, -600 /* Click */, OnClickCalcelBtn, VTS_NONE)
	ON_EVENT(CModuleBarTeaching, IDC_LABEL_MODULE_COL, -600 /* Click */, OnClickLabelModuleCol, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CModuleBarTeaching::OnClickLabelModuleHead1() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlmodulehead1teach, 3000, -3000,600 ,100);
}

void CModuleBarTeaching::OnClickLabelModuleHead10() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlmodulehead10teach, 3000, -3000,600 ,100);
}

void CModuleBarTeaching::OnClickLabelModuleHead11() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlmodulehead11teach, 3000, -3000,600 ,100);
}

void CModuleBarTeaching::OnClickLabelModuleHead12() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlmodulehead12teach, 3000, -3000,600 ,100);
}

void CModuleBarTeaching::OnClickLabelModuleHead13() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlmodulehead13teach, 3000, -3000,600 ,100);
}

void CModuleBarTeaching::OnClickLabelModuleHead14() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlmodulehead14teach, 3000, -3000,600 ,100);
}

void CModuleBarTeaching::OnClickLabelModuleHead15() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlmodulehead15teach, 3000, -3000,600 ,100);
}

void CModuleBarTeaching::OnClickLabelModuleHead16() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlmodulehead16teach, 3000, -3000,600 ,100);
}

void CModuleBarTeaching::OnClickLabelModuleHead2() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlmodulehead2teach, 3000, -3000,600 ,100);
}

void CModuleBarTeaching::OnClickLabelModuleHead3() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlmodulehead3teach, 3000, -3000,600 ,100);
}

void CModuleBarTeaching::OnClickLabelModuleHead4() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlmodulehead4teach, 3000, -3000,600 ,100);
}

void CModuleBarTeaching::OnClickLabelModuleHead5() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlmodulehead5teach, 3000, -3000,600 ,100);
}

void CModuleBarTeaching::OnClickLabelModuleHead6() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlmodulehead6teach, 3000, -3000,600 ,100);
}

void CModuleBarTeaching::OnClickLabelModuleHead7() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlmodulehead7teach, 3000, -3000,600 ,100);
}

void CModuleBarTeaching::OnClickLabelModuleHead8() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlmodulehead8teach, 3000, -3000,600 ,100);
}

void CModuleBarTeaching::OnClickLabelModuleHead9() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlmodulehead9teach, 3000, -3000,600 ,100);
}

void CModuleBarTeaching::OnClickLabelModuleOffset() 
{
	// TODO: Add your control notification handler code here
	Use_TK(m_ctrlmoduleoffsetteach, 3000, -3000,600 ,100);
}

void CModuleBarTeaching::OnClickSAVEModuleTeach() 
{
	// TODO: Add your control notification handler code here

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	
	pView->ModuleTeachHead[0] = atof(m_ctrlmodulehead1teach.GetCaption());
	pView->ModuleTeachHead[1] = atof(m_ctrlmodulehead2teach.GetCaption());
	pView->ModuleTeachHead[2] = atof(m_ctrlmodulehead3teach.GetCaption());
	pView->ModuleTeachHead[3] = atof(m_ctrlmodulehead4teach.GetCaption());
	pView->ModuleTeachHead[4] = atof(m_ctrlmodulehead5teach.GetCaption());
	pView->ModuleTeachHead[5] = atof(m_ctrlmodulehead6teach.GetCaption());
	pView->ModuleTeachHead[6] = atof(m_ctrlmodulehead7teach.GetCaption());
	pView->ModuleTeachHead[7] = atof(m_ctrlmodulehead8teach.GetCaption());
	pView->ModuleTeachHead[8] = atof(m_ctrlmodulehead9teach.GetCaption());
	pView->ModuleTeachHead[9] = atof(m_ctrlmodulehead10teach.GetCaption());
	pView->ModuleTeachHead[10] = atof(m_ctrlmodulehead11teach.GetCaption());
	pView->ModuleTeachHead[11] = atof(m_ctrlmodulehead12teach.GetCaption());
	pView->ModuleTeachHead[12] = atof(m_ctrlmodulehead13teach.GetCaption());
	pView->ModuleTeachHead[13] = atof(m_ctrlmodulehead14teach.GetCaption());
	pView->ModuleTeachHead[14] = atof(m_ctrlmodulehead15teach.GetCaption());
	pView->ModuleTeachHead[15] = atof(m_ctrlmodulehead16teach.GetCaption());
	pView->ModuleTeachHead[16] = atof(m_ctrlmodulecolteach.GetCaption());
	pView->ModuleTeachHead[17] = atof(m_ctrlmoduleoffsetteach.GetCaption());
}

void CModuleBarTeaching::OnClickCalcelBtn() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}

BOOL CModuleBarTeaching::OnInitDialog()
{
	CDialog::OnInitDialog();

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	CString str;

	str.Format("%.3f", pView->ModuleTeachHead[0]);		//ehji 150227
	m_ctrlmodulehead1teach.SetCaption(str);
	str.Format("%.3f", pView->ModuleTeachHead[1]);		//ehji 150227
	m_ctrlmodulehead2teach.SetCaption(str);
	str.Format("%.3f", pView->ModuleTeachHead[2]);		//ehji 150227
	m_ctrlmodulehead3teach.SetCaption(str);
	str.Format("%.3f", pView->ModuleTeachHead[3]);		//ehji 150227
	m_ctrlmodulehead4teach.SetCaption(str);
	str.Format("%.3f", pView->ModuleTeachHead[4]);		//ehji 150227
	m_ctrlmodulehead5teach.SetCaption(str);
	str.Format("%.3f", pView->ModuleTeachHead[5]);		//ehji 150227
	m_ctrlmodulehead6teach.SetCaption(str);
	str.Format("%.3f", pView->ModuleTeachHead[6]);		//ehji 150227
	m_ctrlmodulehead7teach.SetCaption(str);
	str.Format("%.3f", pView->ModuleTeachHead[7]);		//ehji 150227
	m_ctrlmodulehead8teach.SetCaption(str);
	str.Format("%.3f", pView->ModuleTeachHead[8]);		//ehji 150227
	m_ctrlmodulehead9teach.SetCaption(str);
	str.Format("%.3f", pView->ModuleTeachHead[9]);		//ehji 150227
	m_ctrlmodulehead10teach.SetCaption(str);
	str.Format("%.3f", pView->ModuleTeachHead[10]);		//ehji 150227
	m_ctrlmodulehead11teach.SetCaption(str);
	str.Format("%.3f", pView->ModuleTeachHead[11]);		//ehji 150227
	m_ctrlmodulehead12teach.SetCaption(str);
	str.Format("%.3f", pView->ModuleTeachHead[12]);		//ehji 150227
	m_ctrlmodulehead13teach.SetCaption(str);
	str.Format("%.3f", pView->ModuleTeachHead[13]);		//ehji 150227
	m_ctrlmodulehead14teach.SetCaption(str);
	str.Format("%.3f", pView->ModuleTeachHead[14]);		//ehji 150227
	m_ctrlmodulehead15teach.SetCaption(str);
	str.Format("%.3f", pView->ModuleTeachHead[15]);		//ehji 150227
	m_ctrlmodulehead16teach.SetCaption(str);
	str.Format("%.3f", pView->ModuleTeachHead[16]);		//ehji 150227
	m_ctrlmodulecolteach.SetCaption(str);
	str.Format("%.3f", pView->ModuleTeachHead[17]);		//ehji 150227
	m_ctrlmoduleoffsetteach.SetCaption(str);


	return 0;
}

void CModuleBarTeaching::OnClickLabelModuleCol() 
{
	// TODO: Add your control notification handler code here
		Use_TK(m_ctrlmodulecolteach, 3000, -3000,600 ,100);
}

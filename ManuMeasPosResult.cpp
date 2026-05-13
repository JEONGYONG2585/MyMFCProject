// ManuMeasPosResult.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "ManuMeasPosResult.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CManuMeasPosResult dialog


CManuMeasPosResult::CManuMeasPosResult(CWnd* pParent /*=NULL*/)
	: CDialog(CManuMeasPosResult::IDD, pParent)
{
	//{{AFX_DATA_INIT(CManuMeasPosResult)
	m_strPosResult = _T("");
	//}}AFX_DATA_INIT
}


void CManuMeasPosResult::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CManuMeasPosResult)
	DDX_Text(pDX, IDC_EDIT_RESULT, m_strPosResult);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CManuMeasPosResult, CDialog)
	//{{AFX_MSG_MAP(CManuMeasPosResult)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CManuMeasPosResult message handlers

BOOL CManuMeasPosResult::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char* cTmpData = NULL;
	CString CQMainLang=_T("");
	DWORD dwFileHardDataLengthMain;
	CString strFile;
	CTime t = CTime::GetCurrentTime();

//	strFile.Format("D:\\Postion_Check\\%d-%02d-%02d.dat", t.GetYear(), t.GetMonth(), t.GetDay());
	//CQMainLang.Format("%s","D:\\TOP\\P8_LC\\LC_DATA\\PosChkData.dat");	

	// by ckh 2009.01.09 \
	// 결과 파일 열기 창
	CFileDialog dlg( TRUE, _T("DAT"),_T("D:\\Postion_Check\\*.dat"),OFN_HIDEREADONLY,_T("LOG File (*.log)|*.log|") );
	if( dlg.DoModal() != IDOK )
		return TRUE;
	strFile = dlg.GetFileTitle()+".dat";

	CQMainLang = strFile;
	//데이타 파일 읽기====================================================================
	CFile fileHardData(CQMainLang, CFile::modeReadWrite | CFile::shareDenyNone | CFile::typeBinary);
	fileHardData.SeekToBegin();
	
	dwFileHardDataLengthMain = fileHardData.GetLength();

	cTmpData = new char[dwFileHardDataLengthMain];
	fileHardData.Read(cTmpData,dwFileHardDataLengthMain);//데이타 파일 읽기
	fileHardData.Close();

	m_strPosResult = cTmpData;
	m_strPosResult = m_strPosResult.Left(dwFileHardDataLengthMain);
	UpdateData(false);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

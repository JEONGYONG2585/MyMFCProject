// FileMng.cpp : implementation file
//

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "MainFrm.h"
#include "P8CA_LcDispDoc.h"
#include "P8CA_LcDispView.h"

#include "FileMng.h"
//

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
#define TERMINAL	0
#define ORIGIN		1

extern RMS_Data structRmsData_Disp;
extern BOOL m_bRecipechangeRoport;
/////////////////////////////////////////////////////////////////////////////
// CFileMng dialog


CFileMng::CFileMng(CWnd* pParent /*=NULL*/)
	: CDialog(CFileMng::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileMng)
	//}}AFX_DATA_INIT
	m_nSelMenu = 0;
//	for(int i = 0; i < 100; i++)	m_nRecipeTemp[i]=0;
	for(int i = 0; i < MAX_RECIPE; i++)	m_nRecipeTemp[i]=0;
}

CFileMng::~CFileMng()
{
}

void CFileMng::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileMng)
	DDX_Control(pDX, IDC_COMBO_TERMINAL, m_ctrlRecipeList);
	DDX_Control(pDX, IDC_COPY_TERMINAL, m_ctrlTerminal);
	DDX_Control(pDX, IDC_COPY_ORIGIN, m_ctrlOrigin);
	DDX_Control(pDX, IDC_BTN_DO, m_ctrlDo);
	DDX_Control(pDX, IDC_BTN_NAME_CHANGE, m_ctrlNameChange);
	DDX_Control(pDX, IDC_BTN_COPY, m_ctrlCopy);
	DDX_Control(pDX, IDC_LABEL_RECIPE_NAME, m_ctrlRecipeName);
	DDX_Control(pDX, IDC_LABEL_RECIPENAME_BACK, m_ctrlRecipeNameBack);
	DDX_Control(pDX, IDC_BTN_NEWMAKE, m_ctrlNewMake);
	DDX_Control(pDX, IDC_LABEL_RECIPENO, m_ctrlRecipeNO);
	DDX_Control(pDX, IDC_CHANGE, m_ctrlChange);
	DDX_Control(pDX, IDC_DELETE, m_ctrlDelete);
	DDX_Control(pDX, IDC_RENAME, m_ctrlRename);
	DDX_Control(pDX, IDC_LABEL_RECIPENO2, m_ctrlRecipeNO2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileMng, CDialog)
	//{{AFX_MSG_MAP(CFileMng)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_COMBO_TERMINAL, OnSelchangeComboTerminal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileMng message handlers

BOOL CFileMng::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
//
	CDialog::OnInitDialog();

/*	// list 폰트 설정
	m_fontList->CreateFont(30,10,0,0,NULL,FALSE,FALSE,FALSE,ANSI_CHARSET,
	OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY, 
	DEFAULT_PITCH,"Microsoft Sans Serif");

	m_fontListRadio->CreateFont(15,10,0,0,NULL,FALSE,FALSE,FALSE,ANSI_CHARSET,
	OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY, 
	DEFAULT_PITCH,"Microsoft Sans Serif");

	m_listFileOrigin.SetFont(m_fontList, TRUE);
	m_ctrlTerminal.SetFont(m_fontList, TRUE);

	for(int i =0; i <6; i++){
		GetDlgItem(IDC_RADIO_HDFD+i)->SetFont(m_fontListRadio, TRUE);
	}
	for(i=0; i<2 ; i++){
		GetDlgItem(IDC_STATIC_TERMINAL+i)->SetFont(m_fontListRadio, TRUE);
	}

*/

	SelectLanguage();  //ehji

	// 프로그램 설치시 등록되는 데이타의 경로
	m_strDataPath = _T("D:\\TOP\\P8CA_LC\\P8CA_LC_DATA");
	FindRecipeData(m_strDataPath);

	// recipe name 설정
	SubDisplayRecipeName();
	// list control 컬럼 설정
	SubMakeListView();	 // List Control
	// list control에 recipe data 설정
	SubSetListData(ORIGIN);
	SubSetListData(TERMINAL);

	UpdateData(FALSE);

//	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFileMng::OnOK() 
{
//	
//	CDialog::OnOK();
}

void CFileMng::OnCancel() 
{
	// TODO: Add extra cleanup here
	
//	CDialog::OnCancel();
}



BEGIN_EVENTSINK_MAP(CFileMng, CDialog)
    //{{AFX_EVENTSINK_MAP(CFileMng)
	ON_EVENT(CFileMng, IDC_BTN_COPY, -600 /* Click */, OnClickBtnCopy, VTS_NONE)
	ON_EVENT(CFileMng, IDC_BTN_DO, -600 /* Click */, OnClickBtnDo, VTS_NONE)
	ON_EVENT(CFileMng, IDC_BTN_NAME_CHANGE, -600 /* Click */, OnClickBtnNameChange, VTS_NONE)
	ON_EVENT(CFileMng, IDC_CMD_RETURN, -600 /* Click */, OnClickCmdReturn, VTS_NONE)
	ON_EVENT(CFileMng, IDC_BTN_NEWMAKE, -600 /* Click */, OnClickBtnNewmake, VTS_NONE)
	ON_EVENT(CFileMng, IDC_LABEL_RECIPENO, -600 /* Click */, OnClickLabelRecipeno, VTS_NONE)
	ON_EVENT(CFileMng, IDC_CHANGE, -600 /* Click */, OnClickChange, VTS_NONE)
	ON_EVENT(CFileMng, IDC_DELETE, -600 /* Click */, OnClickDelete, VTS_NONE)
	ON_EVENT(CFileMng, IDC_RENAME, -600 /* Click */, OnClickRename, VTS_NONE)
	ON_EVENT(CFileMng, IDC_LABEL_RECIPENO2, -600 /* Click */, OnClickLabelRecipeno2, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()
//
void CFileMng::OnClickBtnNewmake() 
{
	m_nSelMenu = 0;	
	m_ctrlNewMake.SetBackColor(GREEN);
	m_ctrlNameChange.SetBackColor(WHITEGRAY);
	m_ctrlCopy.SetBackColor(WHITEGRAY);
	m_ctrlRecipeList.ShowWindow(TRUE);
//
	RECIPE_DATA rd;
//	BOOL bExistChk[100];
	BOOL bExistChk[MAX_RECIPE];
	CString str="";
	int i=0 , c=0;
//
	m_ctrlRecipeList.ResetContent();
//
//	for(i = 0; i < 100; i++)	bExistChk[i]=FALSE;
	for(i = 0; i < MAX_RECIPE; i++)	bExistChk[i]=FALSE;

	for(i = 0; i < m_arRecipeData.GetSize(); i++)
	{
		rd = m_arRecipeData.GetAt(i);
		bExistChk[atoi(rd.strFrontRecipeName)-1]=TRUE;
	}

	m_ctrlRecipeList.SetItemHeight(-1, 35);
//	for(i = 0, c=0; i < 100; i++)
	for(i = 0, c=0; i < MAX_RECIPE; i++)
	{
		if(bExistChk[i]==FALSE)
		{
			m_nRecipeTemp[c]=(i+1); c++;
			//
			str.Format("%003d", i+1);
			m_ctrlRecipeList.SetItemHeight(i, 35);
			m_ctrlRecipeList.AddString((LPCSTR)str);
		}
	}
//
	GetDlgItem(IDC_LABEL_ORIGIN)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_LABEL_TERMINAL)->ShowWindow(SW_SHOW);
	//
	GetDlgItem(IDC_COPY_ORIGIN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_COPY_TERMINAL)->ShowWindow(SW_HIDE);
	//
//	if(m_nFromTo == 2) { m_nFromTo = 0 ; UpdateData(FALSE); }
	//
//	OnRadioHdfd();
	
}

void CFileMng::OnClickBtnNameChange() 
{
	// name change
	m_nSelMenu = 1;	

	m_ctrlNewMake.SetBackColor(WHITEGRAY);
	m_ctrlNameChange.SetBackColor(GREEN);
	m_ctrlCopy.SetBackColor(WHITEGRAY);
	m_ctrlRecipeList.ShowWindow(FALSE);

	GetDlgItem(IDC_LABEL_ORIGIN)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_LABEL_TERMINAL)->ShowWindow(SW_HIDE);
	//
	GetDlgItem(IDC_COPY_ORIGIN)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_COPY_TERMINAL)->ShowWindow(SW_HIDE);
	//
	UpdateData(FALSE);
	//
/////////
	m_strDataPath = _T("d:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\");
	//설정된 디렉토리로부터 recipe를 읽어서 등록한다
	FindRecipeData(m_strDataPath);
	SubSetListData(ORIGIN);
/////////
}

void CFileMng::OnClickBtnCopy() 
{
	// copy
	m_nSelMenu = 2;

	m_ctrlNewMake.SetBackColor(WHITEGRAY);
	m_ctrlCopy.SetBackColor(GREEN);
	m_ctrlNameChange.SetBackColor(WHITEGRAY);
	m_ctrlRecipeList.ShowWindow(FALSE);
//
	GetDlgItem(IDC_LABEL_ORIGIN)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_LABEL_TERMINAL)->ShowWindow(SW_SHOW);
	//
	GetDlgItem(IDC_COPY_ORIGIN)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_COPY_TERMINAL)->ShowWindow(SW_SHOW);
	//
///////////
	m_strDataPath = _T("d:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\");
	FindRecipeData(m_strDataPath);
	SubSetListData(ORIGIN);
	SubSetListData(TERMINAL);
///////////
//
}

void CFileMng::OnClickBtnDo() 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView *)pFrame->GetActiveView();

	m_ctrlDo.SetBackColor(RED);
	switch(m_nSelMenu)
	{
	case 0:
		SubDo0();
		m_bRecipechangeRoport =TRUE;
		//2010.09.29  by tskim
		pView->RmsFileBuilt(pDoc->m_strRmsRecipeNO);

		//140421 RMS TEST // RECIPE CHANGE 보고 요청함. 
		pView->AllRecipeReport(1);	
		
//		if(pDoc->m_nSite == 1)
		{
			pDoc->m_nRecipeEditCode = 1;
			pView->SendMessage(WM_CIM_REPORT_WORK,8,NULL);
		}
		break;
	case 1: // name change
		SubDo1();
		break;
	case 2: // copy
		SubDo2();
		break;
//	case 3:
//		SubDo3();
//		break;
	case 4://Change
		m_bRecipechangeRoport =TRUE;
//		if(pDoc->m_nSite == 1)
		{
			pDoc->m_nRecipeEditCode = 2;
			pView->SendMessage(WM_CIM_REPORT_WORK,8,NULL);
		}
		break;
	case 5://Delete
		m_bRecipechangeRoport =TRUE;
		pView->RmsFileDelete(structRmsData_Disp.FromRecipe);
//		if(pDoc->m_nSite == 1)
		{
			pDoc->m_nRecipeEditCode = 3;
			pView->SendMessage(WM_CIM_REPORT_WORK,8,NULL);
		}
		break;
	case 6://Rename
		m_bRecipechangeRoport =TRUE;
		pView->RmsFileRename(structRmsData_Disp.FromRecipe,structRmsData_Disp.ToRecipe);
//		if(pDoc->m_nSite == 1)
		{
			pDoc->m_nRecipeEditCode = 4;
			pView->SendMessage(WM_CIM_REPORT_WORK,8,NULL);
		}
		break;
	default:
		break;
	}	
	m_ctrlDo.SetBackColor(WHITEGRAY);
	m_bRecipechangeRoport =FALSE;
}

void CFileMng::SubDo0()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispDoc* pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	CString strFront,strNewDir;
	CString strFilePath, strFilePathTo;
	int i=0;
//
	strFront = "001"; // default directory
//
	i= m_ctrlRecipeList.GetCurSel();
	strNewDir.Format("%003d",m_nRecipeTemp[i]);
//	if(m_nRecipeTemp[i] <1 || m_nRecipeTemp[i] >100) return;
	if(m_nRecipeTemp[i] <1 || m_nRecipeTemp[i] >MAX_RECIPE) return;

	strFilePathTo = _T("d:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\");
	strFilePathTo += strNewDir;
	strFilePathTo += char(0);

	// 메인 디렉 토리가 없으면 폴드를 만든다.
	_mkdir(strFilePathTo);
	strFilePathTo += "\\";
	
 	strFilePath = _T("d:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\");
 	strFilePath += strFront;
 	if(strFilePath.Right(1) != "\\") strFilePath += "\\";
 	strFilePath += "DataEditorMain.dat";
 	strFilePath += char(0);

	// 파일복사를 한다
	SHFILEOPSTRUCT fo;
	fo.hwnd = NULL;
	fo.wFunc = FO_COPY;
	fo.pFrom = (LPCTSTR)strFilePath;
	fo.pTo = (LPCTSTR)strFilePathTo;
	fo.fFlags = FOF_NOCONFIRMMKDIR|FOF_SILENT ;
	fo.fAnyOperationsAborted = TRUE;
	fo.lpszProgressTitle = "파일 복사중";

	::SHFileOperation(&fo);

	strFilePath = _T("d:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\");
	strFilePath += strFront;
	if(strFilePath.Right(1) != "\\") strFilePath += "\\";
	strFilePath += "RMS_Report.dat";
	strFilePath += char(0);


	// 파일복사를 한다
//	SHFILEOPSTRUCT fo;
	fo.hwnd = NULL;
	fo.wFunc = FO_COPY;
	fo.pFrom = (LPCTSTR)strFilePath;
	fo.pTo = (LPCTSTR)strFilePathTo;
	fo.fFlags = FOF_NOCONFIRMMKDIR|FOF_SILENT ;
	fo.fAnyOperationsAborted = TRUE;
	fo.lpszProgressTitle = "파일 복사중";

	::SHFileOperation(&fo);

	// 파일복사 완료 후, Recipe를 다시 부른다
	m_arRecipeData.RemoveAll();
	FindRecipeData("d:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\");


// 	//140421 RMS TEST
// 	pView->AllRecipeReport(1);
//	SetListData(ORIGIN);
}

// change recipe name
void CFileMng::SubDo1()
{
	CString str;
	CString strFront, strBack;
	POSITION pos;
	int i = 0 , j = 0;
//
	// 현재 선택된 Recipe data 알아내기
	pos = m_ctrlOrigin.GetFirstSelectedItemPosition();
 	if(pos != NULL)
	{
		i = m_ctrlOrigin.GetNextSelectedItem(pos);
		str = m_ctrlOrigin.GetItemText(i, 0);

		// 선택된 recipe name에서 front와 back로 분리해서 
		j = str.FindOneOf("|");

		strFront = str.Left(j);
		strBack = (const char *)str+j+1;
	}
	else
	{
//		AfxMessageBox("선택된 recipe가 없습니다");
		return;
	}


//	CStringInput dlg(strBack, CPoint(350, 200));
//	if(dlg.DoModal() == IDOK)
//	{
//		strBack = dlg.m_strNewData;
//	}
//	else
//		return;

	m_ctrlRecipeNameBack.SetCaption(strBack);
	Use_STR(m_ctrlRecipeNameBack,CPoint(100,100));
//
	strBack = m_ctrlRecipeNameBack.GetCaption();

	// Radio 버튼과 연결된 멤버볌수(m_nFromTo)를 다시 부른다
	// 0 : C  1: A 
	UpdateData(TRUE);

	// 리스트 컨트롤에서 선택된 Recipe 데이타를 읽어온다
	//ReadKiban(m_nFromTo , strFront);
	ReadKiban(0 , strFront);

	//RECIPE DATA에 설정
	m_rd.strBackRecipeName = strBack;

	// 변경된 내용을 P8EDataEditorMain.dat에 저장한다
	//SaveKiban(m_nFromTo,strFront);
	SaveKiban(0,strFront);

	// 기존에 존재하는 m_arRecipeData를 clear한다.
	m_arRecipeData.RemoveAll();

	// m_arRecipeData를 다시 읽는다
	FindRecipeData(m_strDataPath);

	// list control에 recipe data 설정한다
	SubSetListData(ORIGIN);

	// 
	SubDisplayRecipeName();
}

void CFileMng::SubDo2()
{
	BOOL bContinue;
	CFileFind file;

	CString strFilePathOriginal,strFilePathTerminal,m_strFilePathOriginalTemp;
	CString str, strTemp, strTemp2;
	CString strFront, strBack;
	CString strRecipeOriginal, strRecipeTemp, strRecipeTerminal, strFileNameBuf;
	POSITION pos;
	int i , j;

	UpdateData(TRUE);

	// Original 의 Path를 찾아낸다.
	pos = m_ctrlOrigin.GetFirstSelectedItemPosition();
 	if(pos != NULL)
	{
		i = m_ctrlOrigin.GetNextSelectedItem(pos);
		str = m_ctrlOrigin.GetItemText(i, 0);

		j = str.FindOneOf("|");

		strFront = str.Left(j);
		strBack = (const char *)str+j+1;
	}
	strRecipeOriginal = strFront;	

	// Terminal 의 Path를 찾아낸다.
	pos = m_ctrlTerminal.GetFirstSelectedItemPosition();
 	if(pos != NULL)
	{
		i = m_ctrlTerminal.GetNextSelectedItem(pos);
		str = m_ctrlTerminal.GetItemText(i, 0);

		j = str.FindOneOf("|");

		strFront = str.Left(j);
		strBack = (const char *)str+j+1;
	}
	if(strFront =="") strFront = strRecipeOriginal;
	strRecipeTerminal = strFront;	
	
	if(strRecipeOriginal == "") return;
	//Original 과 Terminal이 같을경우에는 리턴한다
	if(strRecipeOriginal == strRecipeTerminal) return;

	strFilePathOriginal = _T("d:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\");
	strFilePathOriginal += strRecipeOriginal;
	if(strFilePathOriginal.Right(1) != "\\") strFilePathOriginal += "\\";
	strTemp = strFilePathOriginal;

	strFilePathTerminal = _T("d:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\");
	strFilePathTerminal += strRecipeTerminal; 
	strFilePathTerminal += char(0);
	//모든 파일을 검색한다..
	strFilePathOriginal += "*.*";

	// 설정한 파일을 찾는다
	if(!(bContinue = file.FindFile(strFilePathOriginal)))
		return;

	while(bContinue)
	{
		bContinue = file.FindNextFile();
		str = file.GetFileName();
		if((str != ".") && (str != ".."))
		{
			strFileNameBuf = strFileNameBuf + strTemp + str + char(0);
		}// End of if((str != ".") && (str != ".."))
	}// End of while(bContinue)
	strFileNameBuf += char(0);

	
	// 파일복사를 한다
	SHFILEOPSTRUCT fo;
	fo.hwnd = NULL;
	fo.wFunc = FO_COPY;
	fo.pFrom = (LPCTSTR)strFileNameBuf;
	fo.pTo = (LPCTSTR)strFilePathTerminal;
	fo.fFlags = FOF_NOCONFIRMMKDIR;
	fo.fAnyOperationsAborted = TRUE;
	fo.lpszProgressTitle = "파일 복사중";

	::SHFileOperation(&fo);
// 파일복사 완료 후, Recipe를 다시 부른다
	FindRecipeData("d:\\TOP\\P8CA_LC\\P8CA_LC_DATA\\");
	SubSetListData(ORIGIN);
	SubSetListData(TERMINAL);

}

void CFileMng::OnClickCmdReturn() 
{
	EndDialog(IDOK);
}

// List control 관련//
void CFileMng::SubMakeListView()
{
	//리스트뷰 헤더 칼럼을 만든다
	CRect rect;
	m_ctrlOrigin.GetClientRect(rect);

	LV_COLUMN lvColumn;
	char *listcolumn="Origin ";
	int width  = rect.right;

	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvColumn.fmt = LVCFMT_LEFT;		//정렬 방식
	lvColumn.cx = width;	        // 컬럼의 폭
	lvColumn.iSubItem = 0;          // 컬럼 인덱스 
	lvColumn.pszText = listcolumn;
	m_ctrlOrigin.InsertColumn(0, &lvColumn);
	m_ctrlOrigin.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	//TERMINAL 리스트뷰 헤더 칼럼을 만든다
	CRect rectTerminal;
	m_ctrlTerminal.GetClientRect(rectTerminal);

	LV_COLUMN lvColumnTerminal;
	char *listcolumnTerminal="Terminal ";
	int widthTerminal  = rect.right;

	lvColumnTerminal.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvColumnTerminal.fmt = LVCFMT_LEFT;		
	lvColumnTerminal.cx = width;	
	lvColumnTerminal.iSubItem = 0;
	lvColumnTerminal.pszText = listcolumnTerminal;
	m_ctrlTerminal.InsertColumn(0, &lvColumnTerminal);
	m_ctrlTerminal.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_bRecipechangeRoport =FALSE;

}

// recipe data를 list control에 설정
// 현재 크기 버튼이 뭘로 눌러져있냐에 따라 설정하는 것이 다를 수 있음
int CFileMng::SubSetListData(UINT nOrigin)
{
	CMainFrame *pFrame = (CMainFrame *)::AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	CString str;
	CString strCurrentRecipe;
	int nCurrentSel;

	// Hd의 내용인지 Fd의 내용인지에 따라서 표현
	if(nOrigin == TRUE)	m_ctrlOrigin.DeleteAllItems();
	else m_ctrlTerminal.DeleteAllItems();

	nCurrentSel = -1;

	CString strRecipeName;
	RECIPE_DATA rd;

	int c;
	c = 0;
	
	for(int i = 0; i < m_arRecipeData.GetSize(); i++)
	{
		rd = m_arRecipeData.GetAt(i);

		strRecipeName.Format("%s|%s", rd.strFrontRecipeName, rd.strBackRecipeName);

	
		// recipe 의 크기와 현재 보여주는 recipe의 설정값을 비교해서 
		// 설정값 보다 큰 사이즈이면 보여준다.
		// 그렇지 않으면 continue
		// 보여줄 recipe type 종류의 크기설정
		// 0 -> all recipe
		// 1 -> 1200 x 1000
		// 2 -> 792 x 1000
		// 3 -> 594 x 1000
/*
		switch(m_nViewRecipeSize)
		{
		case 0:
			c++;
			break;
		case 1:
			if(rd.dRecipeSizeX >= 1200 && rd.dRecipeSizeY >= 1000) 
			{
				c++;
				break;
			}
			else continue;
			break;
		case 2:
			if(rd.dRecipeSizeX >= 792 && rd.dRecipeSizeY >= 1000)
			{
				if(rd.dRecipeSizeX < 1200) 
				{
					c++;
					break;
				}
				else continue;
			}
			else 
			{
				continue;
			}
		case 3:
			if(rd.dRecipeSizeX >= 594 && rd.dRecipeSizeY >= 1000) 
			{
				if(rd.dRecipeSizeX < 792) 
				{
					c++;
					break;
				}
				else continue;
			}
			else 
			{
				continue;
			}
			break;
		default: break;
		}
*/

		// 현재 설정 되어 있는 recipe의 인덱스를 찾아낸다 ////////////////
		strCurrentRecipe.Format("%s|%s", pDoc->m_structOperatorModeRecipeData.strFrontRecipeName, pDoc->m_structOperatorModeRecipeData.strBackRecipeName);

		if(strCurrentRecipe == strRecipeName) nCurrentSel = 1;
		///////////////////////////////////////////////////////////////////
	

		if(!SubAddItem(nOrigin , strRecipeName)) AfxMessageBox("Recipe Data 설정 실패!");
	} // end of for(int i = 0; i < pDoc->m_arRecipeData.GetSize(); i++)
	
	// 어떤 종류이던 recipe가 하나도 없으면 No Recipe라고 리스트 박스에 출력
//	if(c == 0) 	SubAddItem(nOrigin , "--->");


	// 찾아낸 인덱스를 선택시켜 놓는다
	if(nOrigin) {
		if(nCurrentSel != -1)
			m_ctrlOrigin.SetItemState( nCurrentSel-1, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
	}
	else {
		if(nCurrentSel != -1)
			m_ctrlTerminal.SetItemState( nCurrentSel-1, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
	}

	return 1;
}

BOOL CFileMng::SubAddItem(UINT nOrigin , CString strRecipeName)
{
	if(nOrigin)
	{
		// 리스트 컨트롤에 항목을 추가하기 위해 LV_ITEM 구조체 설정 
		LV_ITEM lvi;
		// Recipe Name 
		lvi.mask = LVIF_TEXT;
		lvi.iItem = m_ctrlOrigin.GetItemCount();
		lvi.iSubItem = 0;
		lvi.pszText = (char *)(LPCSTR)strRecipeName;
		m_ctrlOrigin.InsertItem(&lvi);
	}
	else
	{
		// 리스트 컨트롤에 항목을 추가하기 위해 LV_ITEM 구조체 설정 
		LV_ITEM lvi;
		// Recipe Name 
		lvi.mask = LVIF_TEXT;
		lvi.iItem = m_ctrlTerminal.GetItemCount();
		lvi.iSubItem = 0;
		lvi.pszText = (char *)(LPCSTR)strRecipeName;
		m_ctrlTerminal.InsertItem(&lvi);
	}

	return TRUE;
}

BOOL CFileMng::FindRecipeData(CString m_strDataPath)
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	BOOL bContinue = TRUE;
	CFileFind fileFind;
	CString strPath;
	CString strDirName;
	int temp = 0;
	int i;

	// Recipe 데이타를 모두 삭제
	m_arRecipeData.RemoveAll();

		// 변수 초기화
	for(i=0; i<MAX_RECIPE; i++)
		pDoc->m_bRecipeExist[i]=FALSE;
	pDoc->m_nRecipecount = 0;//2010.08.06 by tskim
	// 변수 초기화
	strPath.Format("%s\\*.*", m_strDataPath);
//	bContinue = TRUE;

	if(!(bContinue == fileFind.FindFile(strPath)))
	{
		CString str;
		str.Format("%s 폴더를 찾을 수 없습니다!! 실행불가", strPath);
		AfxMessageBox(str);
		exit(-1);
	}

	// 
	while(bContinue)  
	{			//지정된 경로에 파일이 있으면..
		bContinue = fileFind.FindNextFile();
		//
		if(fileFind.IsDirectory())
		{
			strDirName = fileFind.GetFileName();
			if((strDirName != ".") && (strDirName != ".."))
			{
				ReadRecipeFolder(strDirName); // 
				temp = atoi(strDirName);
				pDoc->m_bRecipeExist[temp-1] = TRUE;
//2010.05.17 by tskim Rms
				pDoc->m_nRecipecount++;
			} 
		} 
	}
	
	fileFind.Close();


	// recipe data가 하나도 없으면 프로그램 실행 불가
	if(m_arRecipeData.GetSize() == 0)
	{
		AfxMessageBox("등록된 Recipe Data가 없습니다");
		return FALSE;
	}

	return TRUE;
}

BOOL CFileMng::ReadRecipeFolder(CString strPath)
{
	ifstream fi;
	CString strPathName;

	
	strPathName.Format("%s\\%s\\DataEditorMain.dat", m_strDataPath, strPath);
	
	RECIPE_DATA rd;	
	
	fi.open((char *)(LPCSTR)strPathName, ios::in);
	if(fi.is_open())
	{
		char back[256];
		double dx;   // Glass 크기 X
		double dy;   // Glass 크기 Y

		fi >> back;
		fi >> dx;
		fi >> dy;

		rd.dRecipeSizeX = dx;
		rd.dRecipeSizeY = dy;
		rd.strBackRecipeName.Format("%s", back);
		rd.strFrontRecipeName = strPath;

		m_arRecipeData.Add(rd);
	}
	else
	{
		return FALSE;
	}
	fi.close();

	return TRUE;
}

// P8EDataEditorMain.dat 불러오기..
BOOL CFileMng::ReadKiban(int nPath,CString strRecipeName)
{
	ifstream fi;
	CString strPathName , strPath;
	char ch[256];	

	// 0:C 1:A
	if(!nPath) strPath = _T("D:\\TOP\\P8CA_LC\\P8CA_LC_DATA");
		else strPath = _T("A:\\DATA");

	// Data Editor 화일 읽기
	strPathName.Format("%s\\%s\\DataEditorMain.dat", strPath, strRecipeName);
	
	fi.open((char *)(LPCSTR)strPathName, ios::in|ios::nocreate);

	if(fi.is_open())
	{
		fi >> ch;
		m_rd.strBackRecipeName.Format("%s", ch);

		fi >> m_structDataEditor.m_dGlassSizeX;
		fi >> m_structDataEditor.m_dGlassSizeY;
		fi >> m_structDataEditor.m_dThickness;
		fi >> m_structDataEditor.m_nPatternNum;
		fi >> m_structDataEditor.m_dHomeX;
		fi >> m_structDataEditor.m_dHomeY;
		fi >> ch;
		m_structDataEditor.m_strComment.Format("%s", ch);
		fi >> m_structDataEditor.m_nSequenceCount;	
		fi >> m_structDataEditor.m_nSetDispense;

		fi >> m_structDataEditor.m_bRecipeNoCompare;
		fi >> m_structDataEditor.m_bPatternDispensing;
		fi >> m_structDataEditor.m_bNgDropping;
		fi >> m_structDataEditor.m_bLCQuanReportPlus;
		fi >> m_structDataEditor.m_bLCQuanReportMinus;
		

		for(int i=0;i<MAX_NOZZLE;i++)
		{
			fi >> ch;
			m_structDataEditor.m_strLcType[i].Format("%s", ch);
		}
		fi >> m_structDataEditor.m_bIsTrfOnlyMode;
		fi >> m_structDataEditor.m_nDropRepeatCnt;
		for(i=0;i<MAX_NOZZLE;i++)
		{
			fi >> ch;
			m_structDataEditor.m_strBarcode[i].Format("%s", ch);
		}	
		fi >> m_structDataEditor.m_bAddDelChange;//Add_Del 변환 기능을 쓸 것인지에 대한 변수...05.29...
		fi >> m_structDataEditor.m_nLcSupplyType;
		fi >> m_structDataEditor.m_nDummyType;
		fi >> m_structDataEditor.m_bSpecialInfo;
		fi >> m_structDataEditor.m_bSafetyLock;
		fi >> m_structDataEditor.m_bGlassCheck;
		fi >> m_structDataEditor.m_bWorkMove;		//ehji 141125
	}
	else
	{
		AfxMessageBox("DataEditorMain.data를 읽는데 실패하였습니다");
		return FALSE;
	}
/*
	// 왠지 꺼림직한거 마무리 점검
	if(m_structDataEditor.m_nPatternNum < 1) m_structDataEditor.m_nPatternNum = 1;
	if(m_structDataEditor.m_dGlassSizeX < 0) m_structDataEditor.m_dGlassSizeX = 1;
	if(m_structDataEditor.m_dGlassSizeY < 0) m_structDataEditor.m_dGlassSizeY = 1;
	if(m_structDataEditor.m_dThickness < 0) m_structDataEditor.m_dThickness = 1;
	if(m_structDataEditor.m_dHomeX < 0) m_structDataEditor.m_dHomeX = 1;
	if(m_structDataEditor.m_dHomeY < 0) m_structDataEditor.m_dHomeY = 1;
	if(m_structDataEditor.m_nSequenceCount < 0) m_structDataEditor.m_nSequenceCount = 1;
	if(m_structDataEditor.m_nSetDispense < 0) m_structDataEditor.m_nSetDispense = 1;
	if(m_structDataEditor.m_bPatternDispensing < 0) m_structDataEditor.m_bPatternDispensing = 1;
	if(m_structDataEditor.m_bNoDropping < 0) m_structDataEditor.m_bNoDropping = 1;
*/	
	// 화일 닫기
	fi.close();
	/////////////////////////////////////////////////////////////////////////////////

	return TRUE;
}

// P8EDataEditorMain.dat 저장하기
BOOL CFileMng::SaveKiban(int nPath , CString strRecipeName)
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();

	FILE *fp;
	CFileException e;
	CString strFile , strPath ;

	// 0:C 1:A
	if(!nPath) strPath = _T("D:\\TOP\\P8CA_LC\\P8CA_LC_DATA");
		else strPath = _T("A:\\DATA");

	// Data Editor창 화일로 저장
	// 화일명 설정
	strFile.Format("%s\\%s\\DataEditorMain.dat", strPath, strRecipeName);
 		
	// 화일 열기
	fp = fopen((char *)(LPCSTR)strFile, "wt");
	if(!fp) 
	{
		AfxMessageBox("DataEditorMain.dat 파일 생성 실패");
		return FALSE;
	}
		
	// 데이타 저장
	fprintf(fp, "%s\n",			m_rd.strBackRecipeName);
	fprintf(fp, "%.3f\t%.3f\n",	m_structDataEditor.m_dGlassSizeX, m_structDataEditor.m_dGlassSizeY);
	fprintf(fp, "%.3f\n",		m_structDataEditor.m_dThickness);
	fprintf(fp, "%d\n",			m_structDataEditor.m_nPatternNum);
	fprintf(fp, "%.3f\t%.3f\n",	m_structDataEditor.m_dHomeX, m_structDataEditor.m_dHomeY);
	fprintf(fp, "%s\n",			m_structDataEditor.m_strComment);
	fprintf(fp, "%d\n",			m_structDataEditor.m_nSequenceCount);
	fprintf(fp, "%d\n",			m_structDataEditor.m_nSetDispense);

	fprintf(fp, "%d\t",			m_structDataEditor.m_bRecipeNoCompare);
	fprintf(fp, "%d\t%d\t",		m_structDataEditor.m_bPatternDispensing, m_structDataEditor.m_bNgDropping);
	fprintf(fp, "%d\t%d\n",		m_structDataEditor.m_bLCQuanReportPlus,m_structDataEditor.m_bLCQuanReportMinus);

	for(int i=0;i<MAX_NOZZLE;i++)
		fprintf(fp, "%s\n",		m_structDataEditor.m_strLcType[i]);

	fprintf(fp, "\n%d\t%d\n",	m_structDataEditor.m_bIsTrfOnlyMode,m_structDataEditor.m_nDropRepeatCnt);

	for(i=0;i<MAX_NOZZLE;i++)
		fprintf(fp, "%d\n",		m_structDataEditor.m_strBarcode[i]);
	fprintf(fp, "%d\n",			m_structDataEditor.m_bAddDelChange);
	fprintf(fp, "%d\n",			m_structDataEditor.m_bAddDelCompare);
	fprintf(fp, "%d\n",			m_structDataEditor.m_nLcSupplyType);
	fprintf(fp, "%d\n",			m_structDataEditor.m_nDummyType);
	fprintf(fp, "%d\n",			m_structDataEditor.m_bSpecialInfo);
	fprintf(fp, "%d\n",			m_structDataEditor.m_bSafetyLock);
	fprintf(fp, "%d\n",			m_structDataEditor.m_bGlassCheck);
	fprintf(fp, "%d\n",			m_structDataEditor.m_bWorkMove);		//ehji 141125

	// 화일 닫기
	fclose(fp);
	/////////////////////////////////////////////////////////////////////////////////////////

	return TRUE;
}

void CFileMng::SubDisplayRecipeName()
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	//
	CString str = "";
	str.Format("%s|%s", pDoc->m_structOperatorModeRecipeData.strFrontRecipeName, pDoc->m_structOperatorModeRecipeData.strBackRecipeName);
	m_ctrlRecipeName.SetCaption(str);
}

void CFileMng::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CPen pen;
	pen.CreatePen(PS_SOLID,5,BLUE);
	CPen *pOldPen = (CPen *)dc.SelectObject(&pen);

	dc.MoveTo(0,HEIGHT_YPOS);
	dc.LineTo(SCREEN_WIDTH,HEIGHT_YPOS);
/*
	dc.MoveTo(0,SCREEN_HEIGHT - HEIGHT_YPOS);
	dc.LineTo(SCREEN_WIDTH,SCREEN_HEIGHT - HEIGHT_YPOS);
*/
	dc.SelectObject(pOldPen);
//	
	// Do not call CDialog::OnPaint() for painting messages
}

void CFileMng::OnSelchangeComboTerminal() 
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	DWORD i=0; CString str="";
	i= m_ctrlRecipeList.GetCurSel();
	str.Format("%003d번 레서피 선택..",m_nRecipeTemp[i]);
	AfxMessageBox(str);

	//140421 RMS TEST BY SHLEE 왜 죽었지?? 살리자
	pDoc->m_strRmsRecipeNO.Format("%03d",m_nRecipeTemp[i]);

	structRmsData_Disp.FromRecipe.Format("%03d",m_nRecipeTemp[i]);
//	m_ctrlRecipeList.ResetContent();
}

void CFileMng::OnClickLabelRecipeno() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CString str = " ",strTemp;
	int nTemp,nTemp1,i;

	Use_TK(m_ctrlRecipeNO,999, 1, 29, 244);
	nTemp1 = atoi((LPCSTR)(m_ctrlRecipeNO.GetCaption()));
	if(!pDoc->m_bRecipeExist[nTemp1-1])
	{
		AfxMessageBox("해당 Recipe 없음!!");
	}

	strTemp = (LPCSTR)(m_ctrlRecipeNO.GetCaption());
	nTemp =strTemp.GetLength();
	nTemp1= 3-nTemp;
	if(3-nTemp != 0)
	{
		for(i=0;i<nTemp1;i++)
		{
			strTemp = "0"+strTemp;
		}
	}
//	structRmsData_Disp.FromRecipe = (LPCSTR)(m_ctrlRecipeNO.GetCaption());
	structRmsData_Disp.FromRecipe = strTemp;
}

void CFileMng::OnClickChange() 
{
	// TODO: Add your control notification handler code here
	m_nSelMenu = 4;

	GetDlgItem(IDC_LABEL_ORIGIN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_LABEL_TERMINAL)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_COPY_ORIGIN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_COPY_TERMINAL)->ShowWindow(SW_HIDE);

	m_ctrlChange.SetBackColor(GREEN);
	m_ctrlDelete.SetBackColor(WHITEGRAY);
	m_ctrlRename.SetBackColor(WHITEGRAY);
}

void CFileMng::OnClickDelete() 
{
	// TODO: Add your control notification handler code here
	m_nSelMenu = 5;	

	GetDlgItem(IDC_LABEL_ORIGIN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_LABEL_TERMINAL)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_COPY_ORIGIN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_COPY_TERMINAL)->ShowWindow(SW_HIDE);

	m_ctrlChange.SetBackColor(WHITEGRAY);
	m_ctrlDelete.SetBackColor(GREEN);
	m_ctrlRename.SetBackColor(WHITEGRAY);
}

void CFileMng::OnClickRename() 
{
	// TODO: Add your control notification handler code here
	m_nSelMenu = 6;	

	GetDlgItem(IDC_LABEL_ORIGIN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_LABEL_TERMINAL)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_COPY_ORIGIN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_COPY_TERMINAL)->ShowWindow(SW_HIDE);

	m_ctrlChange.SetBackColor(WHITEGRAY);
	m_ctrlDelete.SetBackColor(WHITEGRAY);
	m_ctrlRename.SetBackColor(GREEN);
}

void CFileMng::OnClickLabelRecipeno2() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CP8CA_LcDispDoc *pDoc = (CP8CA_LcDispDoc *)pFrame->GetActiveDocument();
	CString str = " ",strTemp;
	int nTemp,nTemp1,i;

	Use_TK(m_ctrlRecipeNO2,999, 1, 29, 244);
//	structRmsData_Disp.ToRecipe = (LPCSTR)(m_ctrlRecipeNO2.GetCaption());
	strTemp = (LPCSTR)(m_ctrlRecipeNO2.GetCaption());
	nTemp =strTemp.GetLength();
	nTemp1= 3-nTemp;
	if(3-nTemp != 0)
	{
		for(i=0;i<nTemp1;i++)
		{
			strTemp = "0"+strTemp;
		}
	}
	structRmsData_Disp.ToRecipe = strTemp;
}

void CFileMng::SelectLanguage()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CP8CA_LcDispView* pView = (CP8CA_LcDispView*)pFrame->GetActiveView();

	if(pView->m_nLanguage == 0)	//ehji 
	{
		SetDlgItemText(IDC_CMD_TITLE ,				_T("FILE 관리"));
		SetDlgItemText(IDC_BTN_NEWMAKE,				_T("새로 만들기"));
		SetDlgItemText(IDC_BTN_NAME_CHANGE,				_T("이름 바꾸기"));
		SetDlgItemText(IDC_BTN_COPY,				_T("복사하기"));
		SetDlgItemText(IDC_BTN_DO,				_T("실행"));
	}
	else if(pView->m_nLanguage == 1)	//ehji 
	{
		SetDlgItemText(IDC_CMD_TITLE ,				_T("FILE Control"));
		SetDlgItemText(IDC_BTN_NEWMAKE,				_T("New Make"));
		SetDlgItemText(IDC_BTN_NAME_CHANGE,				_T("Name Change"));
		SetDlgItemText(IDC_BTN_COPY,				_T("Copy"));
		SetDlgItemText(IDC_BTN_DO,				_T("Execution"));
	}
	else if(pView->m_nLanguage == 2)	//ehji 
	{
		SetDlgItemText(IDC_CMD_TITLE ,				_T("FILE管理"));
		SetDlgItemText(IDC_BTN_NEWMAKE,				_T("新建"));
		SetDlgItemText(IDC_BTN_NAME_CHANGE,				_T("更換名"));
		SetDlgItemText(IDC_BTN_COPY,				_T("複製"));
		SetDlgItemText(IDC_BTN_DO,				_T("實行"));
	}
}

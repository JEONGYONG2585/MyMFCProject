// Vision_Result.cpp : implementation file
//

#include "stdafx.h"
#include "p8ca_lcdisp.h"
#include "Vision_Result.h"
#include "Color.h"
#include "N_Vision.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CN_Vision _NVision;
/////////////////////////////////////////////////////////////////////////////
// CVision_Result dialog


CVision_Result::CVision_Result(CWnd* pParent /*=NULL*/)
	: CDialog(CVision_Result::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVision_Result)
	//}}AFX_DATA_INIT
	for(int i = 0; i < MAX_CELL; i++)
	{
		m_nMisStart[i] = 0;
		m_nOverStart[i] = 0;
		m_nMisCount[i] = 0;
		m_nOverCount[i] = 0;
	}
}


void CVision_Result::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVision_Result)
	DDX_Control(pDX, IDC_MSFLEXGRID_RESULT, m_gridResult);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVision_Result, CDialog)
	//{{AFX_MSG_MAP(CVision_Result)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVision_Result message handlers

BEGIN_EVENTSINK_MAP(CVision_Result, CDialog)
    //{{AFX_EVENTSINK_MAP(CVision_Result)
	ON_EVENT(CVision_Result, IDC_CMD_RETURN, -600 /* Click */, OnClickCmdReturn, VTS_NONE)
	ON_EVENT(CVision_Result, IDC_CMD_CHECK, -600 /* Click */, OnClickCmdCheck, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CVision_Result::OnClickCmdReturn() 
{
	CDialog::OnOK();	
}

BOOL CVision_Result::OnInitDialog() 
{
	CDialog::OnInitDialog();
//	this->MoveWindow(0 ,1 , 1024 ,768);		
	int nRow = 0;
	CString str;
	BOOL bTitle[1000000] = {FALSE,};

//	_NVision.m_nCellCount = 4;

// 	str.Format("X : %.3f\nY : %.3f", _NVision.m_structCount_Result.m_dGlass_Left_Up_X, _NVision.m_structCount_Result.m_dGlass_Left_Up_Y);
// 	SetDlgItemText(IDC_LABEL_GLASS_LEFT_UP, str);
// 	str.Format("X : %.3f\nY : %.3f", _NVision.m_structCount_Result.m_dGlass_Right_Up_X, _NVision.m_structCount_Result.m_dGlass_Right_Up_Y);
// 	SetDlgItemText(IDC_LABEL_GLASS_RIGHT_UP, str);
// 	str.Format("X : %.3f\nY : %.3f", _NVision.m_structCount_Result.m_dGlass_Left_Down_X, _NVision.m_structCount_Result.m_dGlass_Left_Down_Y);
// 	SetDlgItemText(IDC_LABEL_GLASS_LEFT_DOWN, str);
// 	str.Format("X : %.3f\nY : %.3f", _NVision.m_structCount_Result.m_dGlass_Right_Down_X, _NVision.m_structCount_Result.m_dGlass_Right_Down_Y);
// 	SetDlgItemText(IDC_LABEL_GLASS_RIGHT_DOWN,str);

	_NVision.Read_CSV_File(_CSV_COUNT_RESULT);
	
	SetDlgItemText(IDC_LABEL_GLASS_ID, _NVision.m_structCount_Result.m_strGlass_ID);
	_NVision.m_structCount_Result.m_bGlass_Judge ? str = "OK" : str = "NG";
	SetDlgItemText(IDC_LABEL_GLASS_JUDGE, str);
	str.Format("%d", _NVision.m_structCount_Result.m_nGlass_Over_Count);
	SetDlgItemText(IDC_LABEL_GLASS_OVER, str);
	str.Format("%d / %d", _NVision.m_structCount_Result.m_nTotal_Drop_Number, _NVision.m_structCount_Result.m_nTotal_Drop_Count);
	SetDlgItemText(IDC_LABEL_CELL_COUNT, str);
	if(_NVision.m_structCount_Result.m_nTotal_Judge == _RESULT_OK) str = "OK";
	else if(_NVision.m_structCount_Result.m_nTotal_Judge == _RESULT_NG) str = "NG";
	else str = "ER";
	SetDlgItemText(IDC_LABEL_CELL_JUDGE, str);
	_NVision.m_structCount_Result.m_bResult ? str = "OK" : str = "NG";
	SetDlgItemText(IDC_LABEL_STATUS, str);

#if EQ
#else
_NVision.m_structCount_Result.m_bResult = true;
#endif	

	if(_NVision.m_structCount_Result.m_bResult)
	{
		for(int i = 0; i < _NVision.m_nCellCount; i++)
		{
			bTitle[nRow] = TRUE; nRow+=2; bTitle[nRow] = TRUE; nRow++;
			if(_NVision.m_structCount_Result.m_nOver_Count[i] > 0 && _NVision.m_structCount_Result.m_nTotal_Judge != _RESULT_ER)
			{
				bTitle[++nRow] = TRUE;
				for(int j = 0; j < _NVision.m_structCount_Result.m_nOver_Count[i]; j++) nRow++;
			}
			bTitle[++nRow] = TRUE; nRow++;
			if(_NVision.m_structCount_Result.m_nMiss_Count[i] > 0 && _NVision.m_structCount_Result.m_nTotal_Judge != _RESULT_ER)
			{
				bTitle[++nRow] = TRUE;
				for(int j = 0; j < _NVision.m_structCount_Result.m_nMiss_Count[i]; j++) nRow++;
			}
			nRow++;
		}
		OnGrid(nRow, bTitle);
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CVision_Result::OnGrid(int nRow, BOOL *bTitle)
{
	int nTemp = 0, i, j;
	CString str;
//	m_gridResult.ShowWindow(SW_HIDE);
	m_gridResult.SetRows(nRow);
	m_gridResult.SetCols(4);
	for(i = 0; i < nRow; i++)
	{
		m_gridResult.SetRowHeight(i, 300);
		if(bTitle[i])
		{
			m_gridResult.SetRow(i);
			m_gridResult.SetCol(1);
			m_gridResult.SetCellBackColor(DARKGRAY);
			m_gridResult.SetCellForeColor(WHITE);
			m_gridResult.SetCol(2);
			m_gridResult.SetCellBackColor(DARKGRAY);
			m_gridResult.SetCellForeColor(WHITE);
			m_gridResult.SetCol(3);
			m_gridResult.SetCellBackColor(DARKGRAY);
			m_gridResult.SetCellForeColor(WHITE);
		}
	}
	for(i = 0; i < 4; i++)
	{
		i == 0 ? m_gridResult.SetColWidth(i, 2000) : m_gridResult.SetColWidth(i, 4000);
		m_gridResult.SetColAlignment(i,4);
	}

	for(i = 0; i < _NVision.m_nCellCount; i++)
	{
		m_gridResult.SetRow(nTemp);
		m_gridResult.SetCol(0);
		m_gridResult.SetCellBackColor(DARKGRAY);
		m_gridResult.SetCellForeColor(WHITE);

		m_gridResult.SetTextMatrix(nTemp, 0, "CELL NO");
		m_gridResult.SetTextMatrix(nTemp, 1, "CELL DROP NO");
		m_gridResult.SetTextMatrix(nTemp, 2, "CELL DROP COUNT");
		m_gridResult.SetTextMatrix(nTemp, 3, "CELL JUDGE");
		nTemp++;
		str.Format("%d", i+1);
		m_gridResult.SetTextMatrix(nTemp, 0, str);
		str.Format("%d", _NVision.m_structCount_Result.m_nCell_Drop_Number[i]);
		m_gridResult.SetTextMatrix(nTemp, 1, str);
		str.Format("%d", _NVision.m_structCount_Result.m_nCell_Drop_Count[i]);
		m_gridResult.SetTextMatrix(nTemp, 2, str);
		_NVision.m_structCount_Result.m_bCell_Judge[i] ? str = "OK" : str = "NG";
		m_gridResult.SetTextMatrix(nTemp, 3, str);
		nTemp++;
		str.Format("%d", i+1);
		m_gridResult.SetTextMatrix(nTemp, 0, str);
		m_gridResult.SetTextMatrix(nTemp, 1, "OVER COUNT");
// 		m_gridResult.SetTextMatrix(nTemp, 2, "S/L OVER COUNT");
// 		m_gridResult.SetTextMatrix(nTemp, 3, "S/L JUDGE");
		nTemp++;
		str.Format("%d", i+1);
		m_gridResult.SetTextMatrix(nTemp, 0, str);
		str.Format("%d", _NVision.m_structCount_Result.m_nOver_Count[i]);
		m_gridResult.SetTextMatrix(nTemp, 1, str);
		m_nOverCount[i] = _NVision.m_structCount_Result.m_nOver_Count[i];
// 		str.Format("%d", _NVision.m_structCount_Result.m_nSeal_Over_Count[i]);
// 		m_gridResult.SetTextMatrix(nTemp, 2, str);
// 		_NVision.m_structCount_Result.m_bSeal_Judge[i] ? str = "OK" : str = "NG";
// 		m_gridResult.SetTextMatrix(nTemp, 3, str);
		nTemp++;
		if(_NVision.m_structCount_Result.m_nOver_Count[i] > 0 && _NVision.m_structCount_Result.m_nTotal_Judge != _RESULT_ER)
		{
			str.Format("%d", i+1);
			m_gridResult.SetTextMatrix(nTemp, 0, str);
			m_gridResult.SetTextMatrix(nTemp, 1, "OVER DROP NO");
			m_gridResult.SetTextMatrix(nTemp, 2, "X COODINATE");
			m_gridResult.SetTextMatrix(nTemp, 3, "Y COODINATE");
			nTemp++;
			m_nOverStart[i] = nTemp;
			for(j = 0; j < _NVision.m_structCount_Result.m_nOver_Count[i]; j++)
			{
				str.Format("%d", i+1);
				m_gridResult.SetTextMatrix(nTemp, 0, str);
				str.Format("%d", j+1);
				m_gridResult.SetTextMatrix(nTemp, 1, str);
				str.Format("%.3f", _NVision.m_structCount_Result.m_dOver_Pos_X[i][j]);
				m_gridResult.SetTextMatrix(nTemp, 2, str);
				str.Format("%.3f", _NVision.m_structCount_Result.m_dOver_Pos_Y[i][j]);
				m_gridResult.SetTextMatrix(nTemp, 3, str);
				nTemp++;
			}
		}
		str.Format("%d", i+1);
		m_gridResult.SetTextMatrix(nTemp, 0, str);
		m_gridResult.SetTextMatrix(nTemp, 1, "MISS COUNT");
		nTemp++;
		str.Format("%d", i+1);
		m_gridResult.SetTextMatrix(nTemp, 0, str);
		str.Format("%d", _NVision.m_structCount_Result.m_nMiss_Count[i]);
		m_gridResult.SetTextMatrix(nTemp, 1, str);
		m_nMisCount[i] = _NVision.m_structCount_Result.m_nMiss_Count[i];
		nTemp++;
		if(_NVision.m_structCount_Result.m_nMiss_Count[i] > 0 && _NVision.m_structCount_Result.m_nTotal_Judge != _RESULT_ER)
		{
			str.Format("%d", i+1);
			m_gridResult.SetTextMatrix(nTemp, 0, str);
			m_gridResult.SetTextMatrix(nTemp, 1, "MISS DROP NO");
			m_gridResult.SetTextMatrix(nTemp, 2, "X COODINATE");
			m_gridResult.SetTextMatrix(nTemp, 3, "Y COODINATE");
			nTemp++;
			m_nMisStart[i] = nTemp;
			for(j = 0; j < _NVision.m_structCount_Result.m_nMiss_Count[i]; j++)
			{
				str.Format("%d", i+1);
				m_gridResult.SetTextMatrix(nTemp, 0, str);
				str.Format("%d", j+1);
				m_gridResult.SetTextMatrix(nTemp, 1, str);
				str.Format("%.3f", _NVision.m_structCount_Result.m_dMiss_Pos_X[i][j]);
				m_gridResult.SetTextMatrix(nTemp, 2, str);
				str.Format("%.3f", _NVision.m_structCount_Result.m_dMiss_Pos_Y[i][j]);
				m_gridResult.SetTextMatrix(nTemp, 3, str);
				nTemp++;
			}
		}
	}
	for(i = 0; i < nRow; i++)
	{
		if(bTitle[i]) continue;
		m_gridResult.SetRow(i);
		m_gridResult.SetCol(0);
		m_gridResult.SetCellFontSize(20);
		str = m_gridResult.GetTextMatrix(i, 3);
		m_gridResult.SetRow(i);
		m_gridResult.SetCol(3);
		if(str.Compare("OK") == 0) m_gridResult.SetCellBackColor(LIGHTGREEN);
		else if(str.Compare("NG") == 0) m_gridResult.SetCellBackColor(RED);
	}
// 	m_gridResult.SetTextMatrix(1,3,"LLL");
// 	m_gridResult.SetTextMatrix(2,3,"LLL");
// 	m_gridResult.SetTextMatrix(3,3,"LLL");
// 
	m_gridResult.SetMergeCells(3);
	m_gridResult.SetMergeCol(0, TRUE);
	m_gridResult.ShowWindow(SW_SHOW);

}

void CVision_Result::OnClickCmdCheck() 
{
	int i,j,k;
	double dTempX = 0.0, dTempY = 0.0;
	double dX,dY;
	for(i = 0; i < _NVision.m_nCellCount; i++)
	{
		if(m_nOverCount[i] > 0)
		{
			for(j = 0; j < m_nOverCount[i]; j++)
			{
				dTempX = atof(m_gridResult.GetTextMatrix(m_nOverStart[i]+j,2));
				dTempY = atof(m_gridResult.GetTextMatrix(m_nOverStart[i]+j,3));
				for(k = j+1; k < m_nOverCount[i]; k++)
				{
					dX = atof(m_gridResult.GetTextMatrix(m_nMisStart[i]+k,2));
					dY = atof(m_gridResult.GetTextMatrix(m_nMisStart[i]+k,3));
					if( fabs(dTempX - dX) < 0.001 &&
						fabs(dTempY - dY) < 0.001 )
					{
						m_gridResult.SetRow(m_nOverStart[i]+k);
						m_gridResult.SetCol(1);
						m_gridResult.SetCellBackColor(BLACK);
						m_gridResult.SetCellForeColor(YELLOW);
					}
				}
			}
		}
		if(m_nMisCount[i] > 0)
		{
			for(j = 0; j < m_nMisCount[i]; j++)
			{
				dTempX = atof(m_gridResult.GetTextMatrix(m_nMisStart[i]+j,2));
				dTempY = atof(m_gridResult.GetTextMatrix(m_nMisStart[i]+j,3));
				for(k = j+1; k < m_nMisCount[i]; k++)
				{
					dX = atof(m_gridResult.GetTextMatrix(m_nMisStart[i]+k,2));
					dY = atof(m_gridResult.GetTextMatrix(m_nMisStart[i]+k,3));
					if(fabs(dTempX - dX) < 0.001 &&
						fabs(dTempY - dY) < 0.001)
					{
						m_gridResult.SetRow(m_nMisStart[i]+k);
						m_gridResult.SetCol(1);
						m_gridResult.SetCellBackColor(BLACK);
						m_gridResult.SetCellForeColor(YELLOW);
					}
				}
			}
		}
	}
}

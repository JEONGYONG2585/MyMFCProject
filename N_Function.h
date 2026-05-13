// N_Function.h: interface for the CN_Function class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N_FUNCTION_H__0AECBF92_06DD_4132_A8E9_4EFDAC42586D__INCLUDED_)
#define AFX_N_FUNCTION_H__0AECBF92_06DD_4132_A8E9_4EFDAC42586D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PiezoDevice.h"
#include <fstream.h>
#include "MMTimer.h"

//RECIPE 기준 OFFSET 만 관리 한다. 
typedef struct tagN_ModuelData
{
	CString strModuleID;

	int nAmplitude_Offset;
	int nNegOffset_Offset;
	int nFrequecy_Offset;
	double nDuty_Offset;
	int nFirst_Vol_Offset;

//	CString strModuleInfo_Merge;
//	CString strRecipeNo;
//	CString strLineNo;
//	CString strMachineNo;
//	CString strHeadNo;
//	CString strLcMaterialID;
//	CString strTarget;

//	int nRisingEdge_Offset;
//	int nFallingEdge_Offset;
//	int nLcMaterialID_Index;
//	int nAmplitude_DB;
//	int nNegOffset_DB;
//	int nFrequecy_DB;
//	double nDuty_DB;
//	int nFirst_Vol_DB;
//	int nRisingEdge_DB;
//	int nFallingEdge_DB;
//	int nRegistryCnt;

} N_MODULE_DATA;

typedef struct tagNparam
{
	CString m_strModuleID;
	int m_nNAmplitude[256];
	int m_nNNegOffset[256];
	double m_dNDuty[256];
	int m_nNFirstVoltage[256];

	int m_nNAmplitude_Offset;
	int m_nNNegOffset_Offset;
	double m_dNDuty_Offset;
	int m_nNFirstVoltage_Offset;
} N_PARAMETER;

typedef struct tagDB_Index
{
	int nLineNo;
	int nMachineNo;
	int nHeadNo;
	int nRecipeNo;
	int nLcMaterialID;
	CString strLcMaterialID;
	int nTarget;
	int nDBIndex[MAX_NOZZLE];
	int nDBSearch_Start;
	int nDBSearch_End;
} DB_INDEX;


//ID	LineNo	MachineNo	HeadNo	MaterialID	Target
//ID	  2         4        16      	10 	     20 	25,600 
//총 25600....
//#define MAX_LINE_NO			2
//#define MAX_MACHINE_NO		2
//#define MAX_MATERIAL_NO		10
//#define MAX_TARGET_NO		20
//#define MAX_DB_NO			MAX_LINE_NO*MAX_MACHINE_NO*MAX_HEAD_NO*MAX_MATERIAL_NO*MAX_TARGET_NO
//#define MAX_HEAD_NO			MAX_NOZZLE

#define MAX_MODULE			1000

class CN_Function  
{
public:
	int Save_Module_Value_Before();
	CString m_strNLog;
	HANDLE m_hTimeEvent;
	CMMTimer m_mmTimer;
	int m_nSetIndex;
	CString m_strModuleInformation;
	CPiezoDevice *pPiezoDevice;

	DB_INDEX m_structDBIndex;

	N_PARAMETER m_structNParam[MAX_NOZZLE]; 

  //N_MODULE_DATA m_structModuleData_Set[MAX_DB_NO];
	N_MODULE_DATA m_structOffsetData[MAX_MODULE];
	N_MODULE_DATA m_structModuleData_Set[MAX_MODULE];
	N_MODULE_DATA m_structModuleData_Get[MAX_MODULE];
	
	int Save_Module_DB(CString strModuleID);
	int Read_Module_DB(CString strModuleID);
//	int Set_Module_DB(CString strModuleID, int nHeadNo);
//	int Get_Module_DB(CString strModuleID, int nHeadNo);
	int Get_NParameter(int nSlaveNo, PIEZO_PARAM Param);
	int Set_NParameter(int nSlaveNo, PIEZO_PARAM Param);
	int Module_Duplication_Check(int nKind, CString strRecipeName , CString strModuleID);
	int Registry_Initial_DB(N_MODULE_DATA m_structModuleData_Init);

	void Save_N_Log(int nKind, CString strLog);
	void Copy_NParameter(int nSlaveNo, PIEZO_PARAM ParamOrg, PIEZO_PARAM ParamDest);
	BOOL N_Initialize();

	int m_nRtn_Err_Code;
	PIEZO_PARAM m_structPiezo_NFunc;
	CN_Function();
	virtual ~CN_Function();
};

#endif // !defined(AFX_N_FUNCTION_H__0AECBF92_06DD_4132_A8E9_4EFDAC42586D__INCLUDED_)

// PiezoDevice.h: interface for the CPiezoDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PIEZODEVICE_H__71A181A2_EE74_4FAC_B72C_BFF1A142A63C__INCLUDED_)
#define AFX_PIEZODEVICE_H__71A181A2_EE74_4FAC_B72C_BFF1A142A63C__INCLUDED_

#include "PIEZODRIVER\PiezoDrive.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct tagPiezoParam
{
	int nSlave;
	int nAmplitude;
	int nOffset;
	int nFrequecy;
	double nDuty;
	int nK_Amplitude;
	int nK_Offset;
	int nRun_Limit;
	int nFirst_Vol;
	int nRisingEdge;
	int nFallingEdge;
	int nPTNumber[100];
	int nPTSetNumber;
	int nTimeOut;
	int nStartDelay;//2015.07.08 by tskim RunDelay
} PIEZO_PARAM;

typedef struct tagPiezoStatus
{
	BOOL bRun; // TRUE : Run , FALSE : Stop
	BOOL bError; // TRUE : Error , FALSE : Ok
	BOOL bOverLoad; // TRUE : Overload , FALSE : Ok
	BOOL bStandby; // TRUE : Standby On , FALSE : Standby off
	int nPTGetNextNumber;
} PIEZO_STATUS;

typedef struct tagAmplitude
{
	int nAp; // Plus
	int nAm; // Minus
	int nApp; // Total
} PIEZO_AMP;

class CPiezoDevice  
{
public:
	void Set_Slave(int nSlave);
	BOOL GetFreq_Count(long *lValue);
	void Get_Port(int *nPort, int *nBaud, int *nSlave);
	void Set_Port(int nPort, int nBaud, int nSlave);
	int m_nPort;
	int m_nBaudrate;
	int m_nSlave;
	BOOL Save_Param();
	void Cal_Amplitude(PIEZO_AMP *Amp);
	long Get_Status(PIEZO_STATUS *Status);
	void OnStop();
	void OnStop_All();
	void OnRun();
	void OnRun_All();
	BOOL Read_Param(PIEZO_PARAM *Param);
	BOOL Write_Param(PIEZO_PARAM Param);
	void Do_Standby(BOOL On);
	void Do_Standby_All(BOOL On);
	void Set_RtItem(int nRTItem); //ptw 20130121
	BOOL Get_RtItem(long *lValue); //ptw 20130121
	BOOL Connect();
	BOOL Disconnect();
	CPiezoDrive cPiezo;
	CPiezoDevice();
	virtual ~CPiezoDevice();

};

#endif // !defined(AFX_PIEZODEVICE_H__71A181A2_EE74_4FAC_B72C_BFF1A142A63C__INCLUDED_)

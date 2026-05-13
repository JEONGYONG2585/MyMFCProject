// PiezoDevice.cpp: implementation of the CPiezoDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "P8CA_LcDisp.h"
#include "PiezoDevice.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define PARA_AMPLITUDE		0x00
#define PARA_OFFSET			0x01
#define PARA_FREQUENCY		0x02
#define PARA_DUTY			0x03
#define PARA_KAMPLITUDE		0x04
#define PARA_KOFFSET		0x05
#define PARA_RUNLIMIT		0x06
#define PARA_STOPAMPLITUDE	0x07
#define PARA_RISINGEDGE		0x08
#define PARA_FALLINGEDGE	0x09
#define PARA_PTNUMBER00		0x0A
#define PARA_TIMEOUT		0x6E
#define PARA_STARTDELAY     0x6F//2015.07.08 by tskim

#define FFLAG_PARAERROR	0x00000001
#define FFLAG_OVERLOAD	0x00000200
#define FFLAG_RUNSTOP	0x00100000
#define FFLAG_STANDBY	0x10000000
#define FFLAG_ALARMRST	0x00200000

#define BROADCASTID		0xFF

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPiezoDevice::CPiezoDevice()
{
//	m_nPort = 8;
	m_nPort = 19;
	m_nBaudrate = 115200;
	m_nSlave = 1;
}

CPiezoDevice::~CPiezoDevice()
{

}

BOOL CPiezoDevice::Connect()
{	
#if EQ
	cPiezo.DeviceConnect(m_nPort, m_nBaudrate);
	return cPiezo.IsConnected();
#else
	return TRUE;
#endif	
}

BOOL CPiezoDevice::Disconnect()
{	
	return cPiezo.DeviceDisconnect();
}

void CPiezoDevice::Do_Standby(BOOL On)
{
	cPiezo.StandBy(m_nSlave, On);
}

void CPiezoDevice::Do_Standby_All(BOOL On)
{
	cPiezo.StandBy(BROADCASTID, On);
}


BOOL CPiezoDevice::Read_Param(PIEZO_PARAM *Param)
{
	BOOL bRcv = FALSE;
	long lValue;
	int i_pt=0;
	Param->nSlave = (int)m_nSlave;
	bRcv = cPiezo.GetParameter(m_nSlave, PARA_AMPLITUDE, &lValue);
	if(bRcv) return FALSE;
	else Param->nAmplitude = (int)lValue;
	bRcv = cPiezo.GetParameter(m_nSlave, PARA_OFFSET, &lValue);
	if(bRcv) return FALSE;
	else Param->nOffset = (int)lValue;
	bRcv = cPiezo.GetParameter(m_nSlave, PARA_FREQUENCY, &lValue);
	if(bRcv) return FALSE;
	else Param->nFrequecy = (int)lValue;
	bRcv = cPiezo.GetParameter(m_nSlave, PARA_DUTY, &lValue);
	if(bRcv) return FALSE;
	else Param->nDuty = (int)lValue*0.1;
// 	bRcv = cPiezo.GetParameter(m_nSlave, PARA_KAMPLITUDE, &lValue);
// 	if(bRcv) return FALSE;
// 	else Param->nK_Amplitude = (int)lValue;
// 	bRcv = cPiezo.GetParameter(m_nSlave, PARA_KOFFSET, &lValue);
// 	if(bRcv) return FALSE;
// 	else Param->nK_Offset = (int)lValue;
 	bRcv = cPiezo.GetParameter(m_nSlave, PARA_RUNLIMIT, &lValue);
 	if(bRcv) return FALSE;
 	else Param->nRun_Limit = (int)lValue;
	bRcv = cPiezo.GetParameter(m_nSlave, PARA_STOPAMPLITUDE, &lValue);
	if(bRcv) return FALSE;
	else Param->nFirst_Vol = (int)lValue;
	bRcv = cPiezo.GetParameter(m_nSlave, PARA_RISINGEDGE, &lValue);
	if(bRcv) return FALSE;
	else Param->nRisingEdge = (int)(lValue*0.1);
	bRcv = cPiezo.GetParameter(m_nSlave, PARA_FALLINGEDGE, &lValue);
	if(bRcv) return FALSE;
	else Param->nFallingEdge = (int)(lValue*0.1);
	bRcv = cPiezo.GetParameter(m_nSlave, PARA_FALLINGEDGE, &lValue);
	if(bRcv) return FALSE;
	else Param->nFallingEdge = (int)(lValue*0.1);
//2015.07.08 by tskim 제어기 내부엔 usec로 동작 하므로 
	bRcv = cPiezo.GetParameter(m_nSlave, PARA_STARTDELAY, &lValue);
	if(bRcv) return FALSE;
	else Param->nStartDelay = (int)(lValue/1000);

	for(i_pt=0; i_pt<100; i_pt++)
	{
		bRcv = cPiezo.GetParameter(m_nSlave, PARA_PTNUMBER00 + i_pt, &lValue);
		if(bRcv) return FALSE;
		else Param->nPTNumber[i_pt] = (int)lValue;
	}
//  잠시 막음	
//	bRcv = cPiezo.GetParameter(m_nSlave, PARA_TIMEOUT, &lValue);
//	if(bRcv) return FALSE;

	return TRUE;
}

BOOL CPiezoDevice::Write_Param(PIEZO_PARAM Param)
{
#if EQ
	long lValue;
	int i_pt=0;
	lValue = Param.nAmplitude;
	if(cPiezo.SetParameter(m_nSlave, PARA_AMPLITUDE, lValue)) return FALSE;
	lValue = Param.nOffset;
	if(cPiezo.SetParameter(m_nSlave, PARA_OFFSET, lValue)) return FALSE;
	lValue = Param.nFrequecy;
	if(cPiezo.SetParameter(m_nSlave, PARA_FREQUENCY, lValue)) return FALSE;
	lValue = (long)(Param.nDuty*10);
	if(cPiezo.SetParameter(m_nSlave, PARA_DUTY, lValue)) return FALSE;
	lValue = Param.nFirst_Vol;
	if(cPiezo.SetParameter(m_nSlave, PARA_STOPAMPLITUDE, lValue)) return FALSE;
//	lValue = Param.nK_Amplitude;
//	if(cPiezo.SetParameter(m_nSlave, PARA_KAMPLITUDE, lValue)) return FALSE;
//	lValue = Param.nK_Offset;
//	if(cPiezo.SetParameter(m_nSlave, PARA_KOFFSET, lValue)) return FALSE;
	lValue = Param.nRun_Limit;
	if(cPiezo.SetParameter(m_nSlave, PARA_RUNLIMIT, lValue)) return FALSE;
	lValue = Param.nRisingEdge*10;
	if(cPiezo.SetParameter(m_nSlave, PARA_RISINGEDGE, lValue)) return FALSE;
	lValue = Param.nFallingEdge*10;
	if(cPiezo.SetParameter(m_nSlave, PARA_FALLINGEDGE, lValue)) return FALSE;
//2015.07.08 by tskim 제어기 내부엔 usec로 동작 하므로 
	lValue = Param.nStartDelay*1000;
	if(cPiezo.SetParameter(m_nSlave, PARA_STARTDELAY, lValue)) return FALSE;

	for(i_pt=0; i_pt<100; i_pt++)
	{
		lValue = Param.nPTNumber[i_pt];
		if(cPiezo.SetParameter(m_nSlave, PARA_PTNUMBER00 + i_pt, lValue)) return FALSE;
	}
//  잠시 막음 
//	lValue = Param.nTimeOut;
//	if(cPiezo.SetParameter(m_nSlave, PARA_TIMEOUT, lValue)) return FALSE;
#endif
	return TRUE;
}

void CPiezoDevice::OnRun()
{
	cPiezo.Cmd_Start(m_nSlave, TRUE);
}

void CPiezoDevice::OnRun_All()
{
	cPiezo.Cmd_Start(BROADCASTID, TRUE);
}

void CPiezoDevice::OnStop()
{
	cPiezo.Cmd_Start(m_nSlave, FALSE);
}

void CPiezoDevice::OnStop_All()
{
	cPiezo.Cmd_Start(BROADCASTID, FALSE);
}

long CPiezoDevice::Get_Status(PIEZO_STATUS *Status)
{
	long lStatus;
	if(cPiezo.GetDeviceStatus(m_nSlave, &lStatus)) return -1;

	lStatus & FFLAG_RUNSTOP ? Status->bRun = TRUE : Status->bRun = FALSE;
	lStatus & FFLAG_PARAERROR ? Status->bError = TRUE : Status->bError = FALSE;
	lStatus & FFLAG_OVERLOAD ? Status->bOverLoad = TRUE : Status->bOverLoad = FALSE;
	lStatus & FFLAG_STANDBY ? Status->bStandby = FALSE : Status->bStandby = TRUE;

	return lStatus;
}

void CPiezoDevice::Cal_Amplitude(PIEZO_AMP *Amp)
{
	PIEZO_PARAM param;
	Read_Param(&param);
	Amp->nAp = param.nAmplitude - param.nOffset;
	Amp->nAm = (-1)*(param.nAmplitude + param.nOffset);
	Amp->nApp = 2 * param.nAmplitude;
}

BOOL CPiezoDevice::Save_Param()
{
	if(cPiezo.SaveAllParameter(m_nSlave)) return FALSE;
	else return TRUE;
}

void CPiezoDevice::Set_Port(int nPort, int nBaud, int nSlave)
{
	m_nPort = nPort;
	m_nBaudrate = nBaud;
	m_nSlave = nSlave;
}

void CPiezoDevice::Get_Port(int *nPort, int *nBaud, int *nSlave)
{
	*nPort = m_nPort;
	*nBaud = m_nBaudrate;
	*nSlave = m_nSlave;
}

BOOL CPiezoDevice::GetFreq_Count(long *lValue)
{
	if(cPiezo.GetActualPos(m_nSlave, lValue)) return FALSE;
	else return TRUE;
}

void CPiezoDevice::Set_Slave(int nSlave)
{
	m_nSlave = nSlave;
}
void CPiezoDevice::Set_RtItem(int nRTItem)
{
	cPiezo.PosTableRunItem(m_nSlave, nRTItem);
}
BOOL CPiezoDevice::Get_RtItem(long *lValue)
{
	if(cPiezo.GetPTStatus(m_nSlave, lValue)) return FALSE;
	else return TRUE;  //ptw 20130121
}



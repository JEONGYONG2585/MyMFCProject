#include "StdAfx.h"
#include "PiezoDrive.h"

#include "Rs232CB.h"

#define CMD_START	1
#define CMD_STOP	0

#define CMDTYPE_START	0x2A

#define CMDTYPE_STANDBY	0x58

#define CMDTYPE_SAVEALLPARAMETAER	0x10
#define CMDTYPE_WRITEPARAMETAER		0x12
#define CMDTYPE_READPARAMETAER		0x13
#define CMDTYPE_GETSTATUS			0x40
#define CMDTYPE_GETPTSTATUS			0x44
#define CMDTYPE_GETACTPOS			0x53
#define CMDTYPE_POSTABLERUNITEM		0x64

#define IDALL			0xFF


#define FFLAG_PARAERROR	0x00000001
#define FFLAG_RUNSTOP	0x00100000
#define FFLAG_STANDBY	0x10000000
#define FFLAG_ALARMRST	0x00200000


CRs232CB cDevicDrive;


#ifndef FMM_RETURN_CODE
#define FMM_RETURN_CODE

//------------------------------------------------------------------
//                 Return Code Defines.
//------------------------------------------------------------------
namespace PIEZO
{
typedef enum _FMM_ERROR
{
	FMM_OK = 0,
	
	FMM_NOT_OPEN,
	FMM_INVALID_PORT_NUM,
	FMM_INVALID_SLAVE_NUM,
	//FMM_INVALID_PARAMETER_NUM,

	FMC_DISCONNECTED = 5,
	FMC_TIMEOUT_ERROR,
	FMC_CRCFAILED_ERROR,
	FMC_RECVPACKET_ERROR,	// PACKET SIZE ERROR, QUEUE FULL ERROR

	FMM_POSTABLE_ERROR,

	FMP_FRAMETYPEERROR = 0x80,
	FMP_DATAERROR,
	FMP_PACKETERROR,
	FMP_RUNFAIL = 0x85,
	FMP_RESETFAIL,
	FMP_SERVOONFAIL1,
	FMP_SERVOONFAIL2,
	FMP_SERVOONFAIL3,

	FMP_PACKETCRCERROR = 0xAA,

	FMM_UNKNOWN_ERROR = 0xFF,

} FMM_ERROR;
}

#endif	//FMM_RETURN_CODE




CPiezoDrive::CPiezoDrive(void)
{
}

CPiezoDrive::~CPiezoDrive(void)
{
}


union {
   //double dData;
   int  lValue;
   char byValue[4];
}UNION_4DATA;


// 파라미터 쓰기
int CPiezoDrive::SetParameter(BYTE iSlaveNo, BYTE iParamNo, long lParamValue)
{

	BYTE byValue[5];

	
	int i = 0;

	byValue[i++] = iParamNo;
	UNION_4DATA.lValue = lParamValue;
	byValue[i++] = UNION_4DATA.byValue[0];
	byValue[i++] = UNION_4DATA.byValue[1];
	byValue[i++] = UNION_4DATA.byValue[2];
	byValue[i++] = UNION_4DATA.byValue[3];

	//ASSERT(i == sizeof(byValue));

	return cDevicDrive.DoSendCommand(iSlaveNo, CMDTYPE_WRITEPARAMETAER, byValue, i, 0, 0, COMM_WAITTIME);

}

// 파라미터 읽어오기
int CPiezoDrive::GetParameter(BYTE iSlaveNo, BYTE iParamNo, long* lParamValue)
{
	long lValue = 0;
	int nRtn;

	//ASSERT(lParamValue != 0);

	nRtn = cDevicDrive.DoSendCommand(iSlaveNo, CMDTYPE_READPARAMETAER, &iParamNo, 1, &lValue, 4, COMM_WAITTIME);
	
	if (nRtn == PIEZO::FMM_OK)
	{
		if (lParamValue != 0)
			*lParamValue = lValue;
	}
	
	return nRtn;
}

// Ram영역의 데이터를 Flash에 데이터 저장
int CPiezoDrive::SaveAllParameter(BYTE iSlaveNo)
{
	return cDevicDrive.DoSendCommand(iSlaveNo, CMDTYPE_SAVEALLPARAMETAER, 0,0,0,0,COMM_WAITTIME*5,0);
}

// START/STOP
int CPiezoDrive::Cmd_Start(BYTE iSlaveNo, BYTE cmd)
{
	BYTE byValue[5];

	
	int i = 0;

	byValue[i++] = cmd;

	//ASSERT(i == sizeof(byValue));

	return cDevicDrive.DoSendCommand(iSlaveNo, CMDTYPE_START, byValue, i, 0, 0, COMM_WAITTIME);
}

// 상태정보 읽어오기
int CPiezoDrive::GetDeviceStatus(BYTE iSlaveNo, long* lParamValue)
{
	long lValue = 0;
	int nRtn;

	//ASSERT(lParamValue != 0);

	nRtn = cDevicDrive.DoSendCommand(iSlaveNo, CMDTYPE_GETSTATUS, 0, 0, &lValue, 4, COMM_WAITTIME);
	
	if (nRtn == PIEZO::FMM_OK)
	{
		if (lParamValue != 0)
			*lParamValue = lValue;
	}
	
	return nRtn;
}

// 위치정보 읽어오기
int CPiezoDrive::GetActualPos(BYTE iSlaveNo, long* lPosition)
{
	long lValue = 0;
	int nRtn;

	//ASSERT(lParamValue != 0);

	nRtn = cDevicDrive.DoSendCommand(iSlaveNo, CMDTYPE_GETACTPOS, 0, 0, &lValue, 4, COMM_WAITTIME);
	
	if (nRtn == PIEZO::FMM_OK)
	{
		if (lPosition != 0)
			*lPosition = lValue;
	}
	
	return nRtn;
}

// PT정보 읽어오기
int CPiezoDrive::GetPTStatus(BYTE iSlaveNo, long* lPTStatusValue)
{
	long lValue = 0;
	int nRtn;

	//ASSERT(lParamValue != 0);

	nRtn = cDevicDrive.DoSendCommand(iSlaveNo, CMDTYPE_GETPTSTATUS, 0, 0, &lValue, 4, COMM_WAITTIME);
	
	if (nRtn == FMM_OK)
	{
		if (lPTStatusValue != 0)
			*lPTStatusValue = lValue;
	}
	
	return nRtn;
}

// PT Run
int CPiezoDrive::PosTableRunItem(BYTE iSlaveNo, int nPTItem)
{

	BYTE byValue[5];
	
	int i = 0;

	UNION_4DATA.lValue = nPTItem;
	byValue[i++] = UNION_4DATA.byValue[0];
	byValue[i++] = UNION_4DATA.byValue[1];
	byValue[i++] = UNION_4DATA.byValue[2];
	byValue[i++] = UNION_4DATA.byValue[3];

	//ASSERT(i == sizeof(byValue));

	return cDevicDrive.DoSendCommand(iSlaveNo, CMDTYPE_POSTABLERUNITEM, byValue, 2, 0, 0, COMM_WAITTIME);

}

// CONNECT
int CPiezoDrive::DeviceConnect(BYTE nPortNo, DWORD dwBaudrate)
{

	return cDevicDrive.OpenPort(nPortNo, dwBaudrate, 8, 0, 0);
}

// DISCONNECT
int CPiezoDrive::DeviceDisconnect()
{

	cDevicDrive.ClosePort();
	
	return cDevicDrive.IsConnected();
}

// Connected check
BOOL CPiezoDrive::IsConnected()
{
	return cDevicDrive.IsConnected();
}

// StandBy
int CPiezoDrive::StandBy(BYTE iSlaveNo, BYTE cmd)
{
	BYTE byValue[5];

	
	int i = 0;

	byValue[i++] = cmd;

	//ASSERT(i == sizeof(byValue));

	return cDevicDrive.DoSendCommand(iSlaveNo, CMDTYPE_STANDBY, byValue, i, 0, 0, COMM_WAITTIME);
}

// 동작중인지를 확인
int CPiezoDrive::IsRunning(BYTE iSlaveNo)
{
	long lStatus=0;

	GetDeviceStatus(iSlaveNo, &lStatus);
	
	if(lStatus&FFLAG_RUNSTOP)			
	{
		return TRUE;
	}

	return 0;
}

#pragma once

class CPiezoDrive
{
public:
	CPiezoDrive(void);
	~CPiezoDrive(void);

	// 파라미터 쓰기
	int SetParameter(BYTE iSlaveNo, BYTE iParamNo, long lParamValue);
	// 파라미터 읽어오기
	int GetParameter(BYTE iSlaveNo, BYTE iParamNo, long* lParamValue);
	// Ram영역의 데이터를 Flash에 데이터 저장
	int SaveAllParameter(BYTE iSlaveNo);
	// START/STOP
	int Cmd_Start(BYTE iSlaveNo, BYTE cmd);
	// 상태정보 읽어오기
	int GetDeviceStatus(BYTE iSlaveNo, long* lParamValue);
	// 위치정보 읽어오기
	int GetActualPos(BYTE iSlaveNo, long* lParamValue);
	// PT정보 읽어오기
	int CPiezoDrive::GetPTStatus(BYTE iSlaveNo, long* lPTStatusValue);
	// PT Run
	int CPiezoDrive::PosTableRunItem(BYTE iSlaveNo, int nPTItem);
	// CONNECT
	int DeviceConnect(BYTE nPortNo, DWORD dwBaudrate);
	// DISCONNECT
	int DeviceDisconnect();
	// Connected check
	BOOL IsConnected();
	// StandBy
	int StandBy(BYTE iSlaveNo, BYTE cmd);
	// 동작중인지를 확인
	int IsRunning(BYTE iSlaveNo);
};

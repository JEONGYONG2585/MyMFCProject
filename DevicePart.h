// DevicePart.h: interface for the CDevicePart class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEVICEPART_H__DF1232AB_E725_41CB_A396_10B84C20657A__INCLUDED_)
#define AFX_DEVICEPART_H__DF1232AB_E725_41CB_A396_10B84C20657A__INCLUDED_

#include "actqj71e71tcp3.h"	// Added by ClassView

// by ckh 
#include "Define_List.h"
#include "PiezoDevice.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDevicePart  
{
public:
	int GetBin2Dec(char* bin_str );
	BOOL Motion_Board_Init();
	//by shin//2008.07.23//
	void Motion_Board_Stop();
////////////////////////////////////////////////////////////////////////////////////
//  Position Move Functions
////////////////////////////////////////////////////////////////////////////////////
	void Emergency_Stop();
	void HeadZMoveDoneCheck();
	void HeadSMoveDoneCheck();
	void StageMoveDoneCheck();
	void HeadDrop(double Speed);
	void HeadDummyDrop(double Speed);
	void HeadDummyDropReverse(double Speed);
	//
	void ColumnPositionMove(double Pos1 ,double Pos2, double dSpeed,WORD EndCheck);
	BOOL ColumnPosition_Check(double Pos_Column_Near, double Pos_Column_Far);
	BOOL Position_Check(int iBdID, WORD wAxis, double Pos_Pulse, double InposPulse);
	void HeadSPositionMove(double s1, double s2, double s3, double s4,double s5 , double s6 , double s7 , double s8 , double s9 , double s10 , double s11 , double s12 , double s13 , double s14 , double s15 , double s16 ,BOOL abs);
	BOOL HeadSPosition_Check(double s1, double s2, double s3, double s4,double s5 , double s6 , double s7 , double s8 , double s9 , double s10 , double s11 , double s12 , double s13, double s14, double s15, double s16, BOOL abs);
	void CheckHeadSOrder(double *dSetPos, double *dCurPos, double *dMovePos, WORD *wAxis);
	int CheckDropSensor(int head);
	//
	void HeadZPositionMove(double z1, double z2, double z3, double z4, double z5, double z6 , double z7 , double z8 , double z9 , double z10 , double z11 , double z12 , WORD EndCheck);
	void StagePositionMove(double Pos_X, double Pos_Y, double dSSpeed,double dYSpeed,WORD EndCheck);
	void StageWarmUpMove(double Pos_S, double Pos_Y,double speed_s, double speed_y ,WORD EndCheck);
	void StageLinearPositionMove(double Pos_X, double Pos_Y, double StageSpeed, WORD EndCheck);
	void StageDrop(double Pos_X,double Pos_Y,double StageSpeed,double DropSpeed);
	void StageLineDrop(double Pos_X,double Pos_Y,double StageSpeed, double DischargeSpeed,int point_Info);

	void MoveAbs(WORD axis, double pos, double vel, WORD wait);
	void OriginS_MoveIni();

////////////////////////////////////////////////////////////////////////////////////
//  I/O Functions
////////////////////////////////////////////////////////////////////////////////////
//
	BOOL SyncSetIO(int, double, BOOL, BOOL); // sync io Á¦¾î..	
	BOOL SyncSetPos(double, BOOL);//
//
	BOOL Ionizer_Sol_On();
	BOOL Ionizer_Sol_Off();
	int Ionizer_Status_Check();
	//
	BOOL SST_Check(int but_num);
	BOOL Sylinge_Check(int head_num);
	BOOL Bit_Check(int bd_num,int bit_num);
	BOOL OutBit_Check(int bd_num,int bit_num);
	BOOL Stage_glass_check();
	BOOL Robot_arm_check();
	BOOL Emergency_check();
	BOOL InterLock_Cover_check();
	//
	BOOL Stage_Vacuum_On();
	BOOL Stage_Vacuum_Off();
	BOOL Stage_Vacuum_Release_OnOff(int nOnOff);
	void Start_button_onoff(BOOL ON_OFF);
	void Stop_button_onoff(BOOL ON_OFF);

////////////////////////////////////////////////////////////////////////////////////
//  A/D Functions
////////////////////////////////////////////////////////////////////////////////////
	float GetAnalogLoadCell(USHORT gwChannel);

////////////////////////////////////////////////////////////////////////////////////
//  MELSECNET Communication Functions
////////////////////////////////////////////////////////////////////////////////////
	void MELSECNET_close();
	void MELSECNET_open();
	CDevicePart();
	virtual ~CDevicePart();

public:
	WORD CheckNewDropSensor(int *nHeadInform );
	BOOL ColumnPosition_Compare();
	int m_nJobStep;
	CString m_strDeviceLog;
	int m_nDropLineNO;
	void SaveDeviceLog(CString strText);
	CString BinToHex(CString strData);
	void FlexibleNtypeStageLineDrop(int DotPoint, double Pos_X, double Pos_Y, double StageSpeed, double DischargeSpeed, int point_Info, int mode, int start_delay, int stop_delay, int inpos_value);
	void FlexibleHeadStepDrop(double DischargeSpeed, int DropStartPos, BOOL EndCheck);
	void FlexibleStageLineDrop(double Pos_X, double Pos_Y, double StageSpeed, double DischargeSpeed, int point_Info, int DotPoint);
	void StageLineDropNtype(double Pos_X, double Pos_Y, double StageSpeed, double *DischargeSpeed, int point_Info);
	CPiezoDevice *m_pPiezoDevice;
	double GetPressValue(int Gauge);
	long LongHexString2Dec(CString lHexData);
	CString Make2sComplement(CString Data);
	void StagePositionMove2(double Pos_X, double Pos_Y, double dSSpeed,double dYSpeed,WORD EndCheck);
	int GetStageVac_Purge();
	BOOL SupplySolOff2();
	BOOL SupplySolOff1();
	CString m_strResult;
	CString GetDec2Bin(int nNumber);
	BOOL SetLightBright(int nCam, int nVal);
	int GateCloseAlarmCheck(int headno);
	int HexString2Dec(CString lHexData);
	BOOL SaveTeachingPosChk(PosChk_Data PosChkData,int nHeadNum);
	BOOL GetPosChkTeachingData(PosChk_Data *PosChkData,int nMotionMode = MODE_POSCHK);
	void HeadSPositionOffsetRead(double *dData);
	void CameraSelect(int CameraNum);
	void HeadStepDrop(double DischargeSpeed, int DropStartPos, BOOL EndCheck);
	void StageLineFill(double DropSpeed, double SuctionSpeed, int *LineDropCount, int DropStartPos);
	BOOL HeadSPosition_Check2(double Pos_S1, double Pos_S2, double Pos_S3, double Pos_S4, double Pos_S5, double Pos_S6, double Pos_S7, double Pos_S8, BOOL abs);
	BOOL HeadSPosition_Check1(double Pos_S1, double Pos_S2 ,double Pos_S3, double Pos_S4, double Pos_S5, double Pos_S6, double Pos_S7, double Pos_S8,BOOL abs);
	void HeadSPositionMove2(double Pos_S1, double Pos_S2, double Pos_S3, double Pos_S4, double Pos_S5, double Pos_S6, double Pos_S7, double Pos_S8, BOOL abs);
	void HeadSPositionMove1(double Pos_S1, double Pos_S2 ,double Pos_S3, double Pos_S4, double Pos_S5, double Pos_S6,double Pos_S7,double Pos_S8,BOOL abs);
	BOOL MotionEndCheck(WORD axis);
	void HeadZMoveCol(double Pos_Z1, double Pos_Z2, double Pos_Z3, double Pos_Z4, double Pos_Z5, double Pos_Z6, int ColumnSel, WORD EndCheck);
	// TCP Communication Function
	void TCP_Close();
	int TCP_BitSet(short szDevice, BOOL BitFlag);
	int TCP_WriteWord(short szDevice, CString WordData, short Length);	
	CString TCP_ReadWord(short szDevice, short Length, BOOL IHexData);
	int TCP_WriteBit(short szDevice, CString BitData);
	int TCP_ReadBit_As_Word(short szDevice);
	BOOL TCP_Read_A_Bit(short szDevice, int IBitCount);
	CActQJ71E71TCP3 *m_ActTCP;
	void SetActTCP(CActQJ71E71TCP3 *actTCP);
//
	long SetDevice(LPCTSTR szDevice, long dwData);
	long SetDevice2(short szDevice, short sData);
	long SetDevice2(LPCTSTR szDevice, short sData);
	long GetDevice(LPCTSTR szDevice, long* lpdwData);
	long GetDevice2(LPCTSTR szDevice, short* lpsData);	
	long ReadDeviceBlock(LPCTSTR szDevice, long dwSize, long* lpdwData);	
	long ReadDeviceBlock2(LPCTSTR szDevice, long lSize,  short* lpsData);
	long WriteDeviceBlock(LPCTSTR szDevice, long dwSize, long* lpdwData);
	long WriteDeviceBlock2(LPCTSTR szDevice, long lSize, short* lpsData);
	///////////////////////////////////////////
	BOOL Count_Board_Init();
	bool SetDropCommand();
	void SetMachineMonitoring(int nStatus);
	void MNET_Mode_Set(int mode);
	void MNET_GUI_Stop();
	void MNET_GUI_Start();
	int MNET_BitSet(short Address, BOOL BitFlag);
	int MNET_WriteWord(short Address, CString WordData, short Length );
	CString MNET_ReadWord(short Address, short Length, BOOL IHexData);
	int MNET_WriteBit(short Address, CString BitData);
	BOOL MNET_Read_A_Bit(short Address, int IBitCount);
	int MNET_ReadBit_As_Word(short Address);
	CString MakeHexDatatoPLC(CString lText, int lLength);
	CString MakeStringDatafromPLC(CString lText);
	int HexConvert(CString lHexData);

	/* s position offset data reading form target file*/
	BOOL HeadSPositionOffsetRead();

	int m_nSelectCamera;
	int m_nMux1;
	int m_nMux2;

private:
	int HexToByte(CString lData);
	long HexToVal(CString lData);
};

#endif // !defined(AFX_DEVICEPART_H__DF1232AB_E725_41CB_A396_10B84C20657A__INCLUDED_)

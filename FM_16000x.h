#ifndef __FM_16000X_H__
#define	__FM_16000X_H__

//------------------------------------------------------------------
//                        Structures.
//------------------------------------------------------------------

#pragma pack(1)

typedef enum _FMM_ERROR
{
	FMM_OK = 0,
	
	FMM_OPEN_FAIL,				// 1
	FMM_CLOSE_FAIL,				// 2
	FMM_NOT_OPEN,				// 3
	FMM_INVALID_BD_NUM,			// 4
	FMM_INVALID_AXIS_NUM,		// 5
	FMM_INVALID_PARAMETER_NUM,	// 6
	FMM_PARAMETER_RANGE_ERROR,	// 7

	FMM_ZERO_RETURN_ERROR,		// 8

	FMM_TIMEOUT_ERROR,			// 9

	FMM_UNKNOWN_ERROR			// 10
} FMM_ERROR;

#pragma pack()

#define DIR_INC		1
#define DIR_DEC		0

#define ENDCHECK_ENABLE		1
#define ENDCHECK_DISABLE	0

//------------------------------------------------------------------
//                 Flag Defines.
//------------------------------------------------------------------
#define FFLAG_ERRORALL			0X00000001
#define FFLAG_ERRSERVOALARM		0X00000002
#define FFLAG_INPTIMEOUT		0X00000004
#define FFLAG_HWPOSILMT			0X00000008
#define FFLAG_HWNEGALMT			0X00000010
#define FFLAG_SWPOGILMT			0X00000020
#define FFLAG_SWNEGALMT			0X00000040
#define FFLAG_ORGTIMEOUT		0X00000080
#define FFLAG_POSTRACKING		0X00000100
#define FFLAG_POSCNTOVER		0X00000200
#define FFLAG_EMGSTOP			0X00000400
#define FFLAG_ERRSETDATA		0X00000800
#define FFLAG_ERRQFULL			0X00001000
#define FFLAG_ERRIOQFULL		0X00002000
#define FFLAG_ENCFEEDBACK		0X00004000
#define FFLAG_ERRHANDWHEEL		0X00008000
#define FFLAG_START				0X00010000
#define FFLAG_SLOWSTOP			0X00020000
#define FFLAG_ZERORETURNING		0X00040000
#define FFLAG_INPOSITION		0X00080000
#define FFLAG_SERVOON			0X00100000
#define FFLAG_ALARMRESET		0X00200000
#define FFLAG_COUNTERCLR		0X00400000
#define FFLAG_HOMESENSOR		0X00800000
#define FFLAG_ZPULSE			0X01000000
#define FFLAG_ZERORETOK			0X02000000
#define FFLAG_MOTIONDIR			0X04000000
#define FFLAG_MOTIONING			0X08000000
#define FFLAG_MOTIONPAUSE		0X10000000
#define FFLAG_MOTIONACCEL		0X20000000
#define FFLAG_MOTIONDECEL		0X40000000
#define FFLAG_MOTIONCONST		0X80000000

//------------------------------------------------------------------
//                        Initialized Functions.
//------------------------------------------------------------------
BOOL FAS_Open();
BOOL FAS_Close();
int  FAS_InitMotion();
BOOL FAS_IsExist(int iBdID);
int  FAS_IsAlive(int iBdID, BOOL *bStatus);

//------------------------------------------------------------------------------
//             Parameter Functions
//------------------------------------------------------------------------------
int FAS_SaveParameter(int iBdID);
int FAS_SaveLocalPara(int iBdID, WORD wAxis, long lLParaNo);
int FAS_SaveGlobalPara(int iBdID, long lGParaNo); 
int FAS_SetLocalPara(int iBdID, WORD wAxis, long lLParaNo, long lLParaValue);
int FAS_SetGlobalPara(int iBdID, long lGParaNo, long lGParaValue);
int FAS_GetLocalPara(int iBdID, WORD wAxis, long lLParaNo, long *lGetLocalPara);
int FAS_GetGlobalPara(int iBdID, long lGParaNo, long *lGetGlobalPara);
int FAS_GetRomGlobalPara(int iBdID, long lGParaNo, long *lGetRomGlobalPara);
int FAS_GetRomLocalPara(int iBdID, WORD wAxis, long lLParaNo, long *lGetRomLocalPara);

//------------------------------------------------------------------------------
//             Servo Driver Control Functions
//------------------------------------------------------------------------------		
int FAS_ServoEnable(int iBdID, WORD wAxis, BOOL bOnOff);
int FAS_ServoAlarmReset(int iBdID, WORD wAxis);
int FAS_ServoErrorCounterClear(int iBdID, WORD wAxis);

//------------------------------------------------------------------------------
//            IO Functions
//------------------------------------------------------------------------------
int FAS_SetIo(int iBdID, BOOL bIsLow, DWORD dwIoValue);
int FAS_SetIoBit(int iBdID, BOOL bIsLow, WORD wBitNo, BOOL bOnOrOff);
int FAS_GetIo(int iBdID, BOOL bIsLow, DWORD *dwGetIo);
int FAS_GetIoBit(int iBdID, BOOL bIsLow, WORD wBitNo,  BOOL *bGetIoBit);
int FAS_GetIoOutputStatus(int iBdID, BOOL bIsLow, DWORD *dwIoOutputStatus);

//------------------------------------------------------------------------------
//            Read Position
//------------------------------------------------------------------------------
int FAS_SetCommandPos(int iBdID, WORD wAxis, double dSetCmdPos);
int FAS_SetActualPos(int iBdID, WORD wAxis, double dSetActualPos);
int FAS_GetCommandPos(int iBdID, WORD wAxis, double *dGetCmdPos);
int FAS_GetActualPos(int iBdID, WORD wAxis,  double *dGetActualPos);
int FAS_GetPosError(int iBdID, WORD wAxis, double *dGetPosError);
int FAS_GetTrackingError(int iBdID, WORD wAxis, double *dGetTrackingError);
int FAS_GetActualVel(int iBdID, WORD wAxis, double *dGetActualVel);

//------------------------------------------------------------------
//                About Queue Functions.
//------------------------------------------------------------------
int FAS_GetFreePosCmdQueue(int iBdID, WORD *wFreeCmdQueue);
int FAS_GetFreePosIoQueue(int iBdID,  WORD *wFreeIoQueue);

int FAS_GetFreeArcQueue(int iBdID, WORD *wFreeArcQueue);
int FAS_GetFreeArcMotionQueue(int iBdID, WORD *wFreeArcMotionQueue);
int FAS_GetFreeLinearQueue(int iBdID, WORD *wFreeLinearQueue);
int FAS_GetFreeContLinearQueue(int iBdID, WORD *wFreeContLinearQueue);
int FAS_GetFreeCircleQueue(int iBdID, WORD *wFreeCircleQueue);
int FAS_GetFreeAxisQueue(int iBdID, WORD wAxis, WORD *wFreeAxisQueue);
int FAS_GetFreeDropQueue(int iBdID, WORD *wFreeDropQueue);

int FAS_ResetArcQueue(int iBdID);
int FAS_ResetArcMotionQueue(int iBdID);
int FAS_ResetLinearQueue(int iBdID);
int FAS_ResetContLinearQueue(int iBdID);
int FAS_ResetCircleQueue(int iBdID);
int FAS_ResetAxisQueue(int iBdID, WORD wAxis);
int FAS_ResetDropQueue(int iBdID);

//------------------------------------------------------------------------------
//			     About D/A Functions.
//------------------------------------------------------------------------------
int FAS_SetDacOutput(int iBdID, WORD wChannel, int iMVolt);
int FAS_SetDaOffset(int iBdID, WORD wChannel, int iMVolt);
int FAS_GetDacOutput(int iBdID, WORD wChannel, int iMVolt, WORD *wGetDaOutput);

//------------------------------------------------------------------
//                About AD Functions.
//------------------------------------------------------------------
int FAS_GetAdResult(int iBdID, WORD wChannel, long *lGetAdResult);

//------------------------------------------------------------------
//                About Laser Senser Functions.	 (2005/03/28)
//------------------------------------------------------------------
int FAS_GetLaserSensor(int iBdID, WORD wChannel, long *lGetLaser);

//------------------------------------------------------------------
//            Axis Status
//------------------------------------------------------------------
int FAS_GetAxisStatus(int iBdID, WORD wAxis, LONG *dwAxisStatus);

int FAS_IsMotioning(int iBdID, WORD wAxis, BOOL *bMotioning);

//------------------------------------------------------------------
//                Clear Functions.
//------------------------------------------------------------------
int FAS_ClearErrorCounter(int iBdID, WORD wAxis);
int FAS_ClearErrorStatus(int iBdID, WORD wAxis);

//------------------------------------------------------------------
//                Motion Functions.
//------------------------------------------------------------------
int FAS_MoveJog(int iBdID, WORD wAxis, int iJogDir);
int FAS_MoveStop(int iBdID, WORD wAxis, WORD wEndCheck);
int FAS_EmergencyStop(int iBdID, WORD wAxis, WORD wEndCheck);
int FAS_MoveOriginSingleAxis(int iBdID, WORD wAxis, WORD wEndCheck);
int FAS_MoveSingleAxisAbsPos(int iBdID, WORD wAxis, double dAbsPos, double dVelocity, WORD wEndCheck);
int FAS_MoveSingleAxisIncPos(int iBdID, WORD wAxis, double dIncPos, double dVelocity, WORD wEndCheck);
int FAS_MoveMultiAxisAbsPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdAbsPos, double *pdVelocity, WORD wEndCheck);
int FAS_MoveMultiAxisIncPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, WORD wEndCheck);
int FAS_MoveLinearIncPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdIncPos, double dFeedrate, WORD wEndCheck);
int FAS_MoveLinearAbsPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdAbsPos, double  dFeedrate, WORD wEndCheck);
int FAS_MoveContLinearAbsPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdAbsPos,double dFeedrate, WORD wStartOrEnd);
int FAS_MoveContLinearIncPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdIncPos, double dFeedrate, WORD wStartOrEnd);
int FAS_MoveCircleAbsPos(int iBdID, WORD wNoOfAxis, int *piCirAxis, double *pdCirEndPosAbs, double *pdCirCenterAbs, double dFeedrate, int iDirection, WORD wEndCheck);
int FAS_MoveCircleIncPos(int iBdID, WORD wNoOfAxis, int *piCirAxis, double *pdCirEndPosInc, double *pdCirCenterInc, double dFeedrate, int iDirection, WORD wEndCheck);
int FAS_MoveContCircleAbsPos(int iBdID, WORD wNoOfAxis, int *piCirAxis, double *pdCirEndPosAbs, double *pdCirCenterAbs, double dFeedrate, int iDirection, WORD wStartOrEnd);
int FAS_MoveContCircleIncPos(int iBdID, WORD wNoOfAxis, int *piCirAxis, double *pdCirEndPosInc, double *pdCirCenterInc, double dFeedrate, int iDirection, WORD wStartOrEnd);

int FAS_VelocityOverride(int iBdID, WORD wAxis, double dVelocity);

int FAS_PositionIncOverride(int iBdID, WORD wAxis, long lOverridePos);
int FAS_PositionAbsOverride(int iBdID, WORD wAxis, long lOverridePos);

int FAS_MoveArchSingleIncPos(int iBdID, WORD wAxis, double dIncPos, double dVelocity, WORD wEndCheck);
int FAS_MoveArchSingleAbsPos(int iBdID, WORD wAxis, double dAbsPos, double dVelocity, WORD wEndCheck);
int FAS_MoveArchMultiAbsPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdAbsPos, double  dFeedrate, WORD wEndCheck);
int FAS_MoveArchMultiIncPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdIncPos, double  dFeedrate, WORD wEndCheck);
int FAS_MoveArchSmoothToBondingIncPos (int iBdID, WORD wAxis, double dIncPos, double dVelocity, double dSmoothDistance, WORD wSmoothRatio, WORD wBondingForce, WORD wEndCheck);
int FAS_MoveArchSmoothToBondingAbsPos (int iBdID, WORD wAxis, double dAbsPos, double dVelocity, double dSmoothDistance, WORD wSmoothRatio, WORD wBondingForce, WORD wEndCheck);
int FAS_MoveOrgZpulse(int iBdID, WORD wAxis);
			
int FAS_MoveToLimit(int iBdID, WORD wAxis, double dSpeed, int iLimitDir, WORD wEndcheck);
int FAS_EscapeFromLimit(int iBdID, WORD wAxis, double dSpeed, int iLimitDir, WORD wEndcheck);
int FAS_MoveToZPulse(int iBdID, WORD wAxis, double dSpeed, int iZPulseDir, WORD wEndcheck);
int FAS_MoveOnePulse(int iBdID, WORD wAxis, int iOnePulseDir);
int FAS_MoveToOrgPoint(int iBdID, WORD wAxis, double dSpeed, WORD wEndcheck);
int FAS_MoveVelocity(int iBdID, WORD wAxis, double dSpeed, int iVelDir);
int FAS_MovePause(int iBdID, WORD wAxis);
int FAS_MoveRestart(int iBdID, WORD wAxis);
int FAS_WaitUntilIoSet(int iBdID, WORD wIoBitNo, BOOL bOnOff);
int FAS_Dwell(int iBdID, long lMsec);
int FAS_SoftResetMotion(int iBdID);
int FAS_WaitSingleMoveDone(int iBdID, WORD wAxis);
int FAS_WaitMultiMoveDone(int iBdID, int iAxes);
int FAS_MoveAbsSynchronous(int iBdID, int *piAxis, long lPosition, long lVelocity, WORD wEndCheck);
int FAS_MoveIncSynchronous(int iBdID, int *piAxis, long lPosition, long lVelocity, WORD wEndCheck);

//------------------------------------------------------------------
//                Special Motion Functions.
//------------------------------------------------------------------

int FAS_MoveArcAbsPos(int iBdID, WORD NoOfAxis, WORD *Axis, double *AbsPos, double  D_Feedrate, double D_CirFeedrate, double CornerDistance, WORD D_StartEndPoint);
int FAS_MoveArcIncPos(int iBdID, WORD NoOfAxis, WORD *Axis, double *IncPos, double  D_Feedrate, double D_CirFeedrate, double CornerDistance, WORD D_StartEndPoint);

// Disp Off + Z Up 
int FAS_MoveArcAbsPosWithSealOff(int iBdID, WORD NoOfAxis, WORD *Axis, double *AbsPos, 
			double D_Feedrate, double D_CirFeedrate, double CornerDistance, WORD D_StartEndPoint, 
			double dUpDistancePos, WORD wOffNoOfAxis, int *piOffAxis, double *pdOffIncPos, double *pdOffVelocity,
			double dMusashiOffDistancePos, DWORD dwOffValue, BOOL bIsLow);

// Disp Off + 1st Z Up + 2nd Z Up 
int FAS_MoveArcAbsPosWithSealOff(int iBdID, WORD NoOfAxis, WORD *Axis, double *AbsPos, 
			double D_Feedrate, double D_CirFeedrate, double CornerDistance, WORD D_StartEndPoint, 
			double dUpDistancePos, WORD wOffNoOfAxis, int *piOffAxis, double *pdOffIncPos, double *pdOffVelocity,
			double dMusashiOffDistancePos, DWORD dwOffValue, BOOL bIsLow,  double dUpDistancePos2, double *pdOffIncPos2);

// Disp Off + 1st Z Up + 2nd Z Up + Corner Pressure Control Flag
int FAS_MoveArcAbsPosWithSealOff(int iBdID, WORD NoOfAxis, WORD *Axis, double *AbsPos, 
			double D_Feedrate, double D_CirFeedrate, double CornerDistance, WORD D_StartEndPoint, 
			double dUpDistancePos, WORD wOffNoOfAxis, int *piOffAxis, double *pdOffIncPos, double *pdOffVelocity,		
			double dMusashiOffDistancePos, DWORD dwOffValue, BOOL bIsLow,  double dUpDistancePos2, double *pdOffIncPos2, BOOL bPressureControlOn);

// Disp Off + 1st Z Up + 2nd Z Up + Corner Pressure Control Flag + Corner Pressure Type
int FAS_MoveArcAbsPosWithSealOff(int iBdID, WORD NoOfAxis, WORD *Axis, double *AbsPos, 
			double D_Feedrate, double D_CirFeedrate, double CornerDistance, WORD D_StartEndPoint, 
			double dUpDistancePos, WORD wOffNoOfAxis, int *piOffAxis, double *pdOffIncPos, double *pdOffVelocity,		
			double dMusashiOffDistancePos, DWORD dwOffValue, BOOL bIsLow,  double dUpDistancePos2, double *pdOffIncPos2, BOOL bPressureControlOn, int nPressureControlType);

// Disp Off + 1st Z Up + 2nd Z Up + Corner Pressure Control Flag + Corner Pressure Type
// + Line Gap Offset(Z축 정보는 wOffNoOfAxis, piOffAxis변수 사용) + Corner Gap Control Off Speed
int FAS_MoveArcAbsPosWithSealOff(int iBdID, WORD NoOfAxis, WORD *Axis, double *AbsPos, 
			double D_Feedrate, double D_CirFeedrate, double CornerDistance, WORD D_StartEndPoint, 
			double dUpDistancePos, WORD wOffNoOfAxis, int *piOffAxis, double *pdOffIncPos, double *pdOffVelocity,		
			double dMusashiOffDistancePos, DWORD dwOffValue, BOOL bIsLow,  double dUpDistancePos2, double *pdOffIncPos2, BOOL bPressureControlOn, int nPressureControlType,
			int *piLineGapOffset, double dCornerGapOnFlag);

// Disp Off + 1st Z Up + 2nd Z Up + Corner Pressure Control Flag + Corner Pressure Type + Multi Disp Off
int FAS_MoveArcAbsPosWithSealOff(int iBdID, WORD NoOfAxis, WORD *Axis, double *AbsPos, 
			double D_Feedrate, double D_CirFeedrate, double CornerDistance, WORD D_StartEndPoint, 
			double dUpDistancePos, WORD wOffNoOfAxis, int *piOffAxis, double *pdOffIncPos, double *pdOffVelocity,
			double *dMusashiOffDistancePos, DWORD dwOffValue, BOOL bIsLow,  double dUpDistancePos2, double *pdOffIncPos2, BOOL bPressureControlOn, int nPressureControlType);

// Disp Off + 1st Z Up + 2nd Z Up + Corner Pressure Control Flag + EZI_R Z Up
int FAS_MoveArcAbsPosWithSealOff(int iBdID, WORD NoOfAxis, WORD *Axis, double *AbsPos, 
			double D_Feedrate, double D_CirFeedrate, double CornerDistance, WORD D_StartEndPoint, 
			double dUpDistancePos, WORD wOffNoOfAxis, int *piOffAxis, double *pdOffIncPos, double *pdOffVelocity,
			double dMusashiOffDistancePos, DWORD dwOffValue, BOOL bIsLow,  double dUpDistancePos2, double *pdOffIncPos2, BOOL bPressureControlOn, BOOL bGapIoLow, DWORD dwGapIoValue);

// Disp Off + 1st Z Up + 2nd Z Up + Corner Pressure Control Flag + EZI_R Z Up(HighLow, IoValue) + Multi Disp Off
int FAS_MoveArcAbsPosWithSealOff(int iBdID, WORD NoOfAxis, WORD *Axis, double *AbsPos, 
			double D_Feedrate, double D_CirFeedrate, double CornerDistance, WORD D_StartEndPoint, 
			double dUpDistancePos, WORD wOffNoOfAxis, int *piOffAxis, double *pdOffIncPos, double *pdOffVelocity,
			double *dMusashiOffDistancePos, DWORD dwOffValue, BOOL bIsLow,  double dUpDistancePos2, double *pdOffIncPos2, BOOL bPressureControlOn, BOOL bGapIoLow, DWORD dwGapIoValue);

int FAS_MoveArcSingleAbsPosWithSealOff(int iBdID, WORD wAxis, double AbsPos, double Velocity,
  									   double dUpDistancePos, WORD wOffNoOfAxis, int *piOffAxis, double *pdOffIncPos, double *pdOffVelocity,
								       double dMusashiOffDistancePos, DWORD dwOffValue, BOOL bIsLow);

int FAS_MoveArcSingleIncPosWithSealOff(int iBdID, WORD wAxis, double IncPos, double Velocity,
  									   double dUpDistancePos, WORD wOffNoOfAxis, int *piOffAxis, double *pdOffIncPos, double *pdOffVelocity,
								       double dMusashiOffDistancePos, DWORD dwOffValue, BOOL bIsLow);

//int FAS_MoveLineDropAbs(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageAbsPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
//                        WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos);

int FAS_MoveLineDropAbs(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageAbsPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
			WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos, BOOL bIsLow/*BOOL bHighOrLow*/, DWORD SyncAxis);

//int FAS_MoveLineDropAbs(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageAbsPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
//                        WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos, int *startDelay, int *stopDelay);


int FAS_MoveLineDropInc(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageIncPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
                        WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos);
//int FAS_MoveDropSyncAbs(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageAbsPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
//                        WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos);

int FAS_MoveDropSyncAbs(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageAbsPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
			WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos, BOOL bIsLow/*BOOL bHighOrLow*/, DWORD SyncAxis);



//int FAS_MoveLineDropInc(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageIncPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
//                        WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos);



int FAS_MoveDropSyncInc(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageIncPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
                        WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos);

////////////////////////////////////////////////////////////////////// 2003.12.27
// Seal
int FAS_GapControlEnable(int iBdID, WORD wAxis, BOOL bOnOff);
int FAS_GapControlEnableMulti(int iBdID, WORD wMultAxis);
int FAS_GapControlDisableMulti(int iBdID, WORD wMultiAxis);
int FAS_GetGapError(int iBdID, WORD wAxis,	BOOL *bGapError);
int FAS_ClearGapError(int iBdID, WORD wAxis);
int FAS_GetGapErrorCount(int iBdID, WORD wAxis,	int *iGapErrorCount);


//////////////////////////////////////////////////////////////////////
// LC
int FAS_MoveDropSyncAbsP6(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageAbsPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
                        WORD wSyncAxis, WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos);
int FAS_MoveLineDropAbsP6(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageAbsPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
                          WORD wSyncAxis, WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos);

// Add 2004/1003
int FAS_SetSyncSlaveAxes(int iBdID, int iAxes);
int FAS_SetSyncMasterAxis(int iBdID, int iAxes);

// Add 2004/1015
int FAS_CornerCompEnable(int iBdID, WORD wAxis, BOOL bOnOff);
int FAS_CornerCompEnableMulti(int iBdID, WORD wMultAxis);
int FAS_CornerCompDisableMulti(int iBdID, WORD wMultiAxis);

// by song 2004/10/27
int FAS_ResetSyncMasterAxes(int iBdID);

//------------------------------------------------------------------
//                About Monitoring Functions.   (2006/12/20)
//------------------------------------------------------------------
// SEAL Gap Comp
int FAS_GetMonitoring(int iBdID, WORD wAxis, double *dMonIndex, double *dMonCommadPos, double *dMonCurPos, 
                      double *dMonGapSensor, double *dMonCompPos);
int FAS_GetMonitoring(int iBdID, WORD wAxis, double *dMonIndex, double *dMonCommadPos, double *dMonCurPos, 
                      double *dMonGapSensor, double *dMonCompPos, double *dMonLCGo);
//ssh 100123
int FAS_GetMonitoring(int iBdID, WORD wAxis, double *dMonIndex, double *dMonCommadPos, double *dMonCurPos, 
                      double *dMonGapSensor, double *dMonCompPos, double *dMonLCGo, double *dMonPosX, double *dMonPosY, double *dMonSpeed);
int FAS_GetMultiMonitoring(int iBdID, WORD wNoOfAxis, int *piAxis, long *lMonIndex, long *lMonCommadPos, long *lMonCurPos, 
                      long *lMonGapSensor, long *lMonCompPos);
//ssh 100123
int FAS_GetMultiMonitoring(int iBdID, WORD wNoOfAxis, int *piAxis, double *dMonIndex, double *dMonCommadPos, double *dMonCurPos, 
                      double *dMonGapSensor, double *dMonCompPos, double *dMonLCGo, double *dMonPosX, double *dMonPosY, double *dMonSpeed);


/* **************************************************************** */
// LC REAL DROP COUNT FUNCTION
//int vEnableDropCount(int iBdID, WORD wAxis, WORD wDropCounterDelay);
int FAS_EnableDropCount(int iBdID, WORD wAxis, WORD wDropCounterDelay);
int vGetMonitoringDropCount(int iBdID, long *dMonIndexDrop, long *dMonCommandPosDropX, 
                            long *dMonCommandPosDropY, long *dMonIOLowDrop, long *dMonIOHihgDrop);
int FAS_GetMonitoringDropCount(int iBdID, long *dMonIndexDrop, long *dMonCommandPosDropX, 
                            long *dMonCommandPosDropY, long *dMonIOLowDrop, long *dMonIOHihgDrop);
//SPD STAGE LINE DROP
int FAS_MoveLineDropAbsStep(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageAbsPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
			                WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos, WORD wBitNo);

//SPD STAGE LINE DROP//Hybrid
int FAS_MoveLineDropAbsStepHybrid(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageAbsPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
			                WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos, WORD wBitNo, BOOL bHybridOn);

//SPD STAGE LINE DROP (EZI+R)
int FAS_MoveLineDropAbsStep(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageAbsPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
			WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos, WORD wBitNo, DWORD dwDropIoValue, BOOL bDropIoHighLow);

//TJD STAGE LINE DROP
int FAS_MoveLineDropAbsStep(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageAbsPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
			                WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos, WORD wBitNo, int *startDelay, int *stopDelay, int *inposValue);


// SPD Multi-Drop(QUEUE)
int FAS_MoveSpdMultiAxisAbsPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdAbsPos, double *pdVelocity, WORD wEndCheck);
int FAS_MoveSpdMultiAxisIncPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, WORD wEndCheck);
int FAS_ResetSpdQueue(int iBdID);
int FAS_GetFreeSpdQueue(int iBdID,  WORD *wFreeAxisQueue);


// Reference IO Monitoring
int FAS_EnableReferenceIOMonitoring(int iBdID, WORD wAxis);
int FAS_GetReferenceIODistance(int iBdID, WORD wAxis, long *lCheckPos);

// TJD Single Axis MoveAbs, MoveInc
int FAS_MoveSingleTJDAxisAbsPos(int iBdID, WORD wAxis, double dAbsPos, double dVelocity, int startDelay, int stopDelay, int inposValue, WORD wEndCheck=TRUE);
int FAS_MoveSingleTJDAxisIncPos(int iBdID, WORD wAxis, double dAbsPos, double dVelocity, int startDelay, int stopDelay, int inposValue, WORD wEndCheck=TRUE);

// TJD Multi Axis MoveAbs, MoveInc
int FAS_MoveMultiTJDAxisAbsPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdAbsPos, double *pdVelocity, int *startDelay, int *stopDelay, int *inposValue, WORD wEndCheck);
int FAS_MoveMultiTJDAxisIncPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, int *startDelay, int *stopDelay, int *inposValue, WORD wEndCheck);

#endif	//__FM_16000X_H__



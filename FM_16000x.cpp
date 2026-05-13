#include "stdafx.h"
#include "Acc.h"
#include "FM_16000x.h"

extern int g_nDropType;//0:SPD, 1: TJD_N
//------------------------------------------------------------------
//              Static Define Const Number
//------------------------------------------------------------------
const static short	AXIS_GLOBAL = 16;
const static short	NUM_GLOBAL_PARAM = 44; 
const static short	NUM_LOCAL_PARAM = 75;   //lbg 100715 73->75 //lbg 101117 75->76 AAA추가 

const static short	RANGE_4BIT_HEX = 0xF0;
const static short	RANGE_8BIT_HEX = 0xF1;
const static short	RANGE_32BIT_HEX = 0xF2;

const static DWORD  FLAG_CHECK_MOTIONING = (FFLAG_MOTIONING | FFLAG_MOTIONACCEL | FFLAG_MOTIONDECEL | FFLAG_MOTIONCONST);

//------------------------------------------------------------------
//              Inner Variables
//------------------------------------------------------------------
CAcc	gAcc;
WORD	awNoAxis[MAX_CARD_NUMS];		// 각 보드마다 축 수.

HANDLE	glb_aSync[MAX_CARD_NUMS];

//------------------------------------------------------------------
//                        Inner Functions Definition.
//------------------------------------------------------------------

#define CREATE_SYNCHRONIZATION(x)	char chBuff[12]; sprintf(chBuff, "FAS16SEMA%d", x); glb_aSync[x] = CreateSemaphore(NULL, 1, 1, chBuff)
#define DELETE_SYNCHRONIZATION(x)	CloseHandle(glb_aSync[x])
#define	ENTER_SYNCHRONIZATION(x)	WaitForSingleObject(glb_aSync[x], INFINITE)
#define	LEAVE_SYNCHRONIZATION(x)	ReleaseSemaphore(glb_aSync[x], 1, NULL)

inline void	SetAxisInfo(int iBdID, WORD wAxis);			//그저 내부적으로 설정하는 것인데... 리턴값이 필요할까?
inline void	SetAxisInfoMulti(int iBdID, WORD wAxes);
inline void	SetAxisInfoDrop(int iBdID, WORD wAxes);

int CheckParameterRange(BOOL bIsGlobal, long lParaNo, long lParaValue);

int WaitDpramCommand(int iBdID, WORD wAxis, BYTE byDpramCmd, DWORD dwmSec);
inline void	SetDpramCommand(int iBdID, WORD wAxis, int byCmd, int byOffset);

inline long	FAS_ReadRam(int iBdID, WORD wAxis, DWORD dwOffset);
inline void	FAS_ReadBlockRamB(int iBdID, int iAdd, BYTE *pbyData, long dwLen);
inline void	FAS_ReadBlockRam(int iBdID, int iAdd, BYTE *pbyData, long dwLen);

inline void	FAS_WriteRam(int iBdID, WORD wAxis, DWORD dwOffset, long dwData);
inline void	FAS_WriteBlockRamB(int iBdID, int iAdd, BYTE *pbyData, DWORD dwLen);

void __GetAxisNumberPara(int iBdID, long *lAxisNo);

//------------------------------------------------------------------
//                        Inner Functions.
//------------------------------------------------------------------

void SetAxisInfo(int iBdID, WORD wAxis)
{
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukServoInfo, (0x01 << wAxis));
}

void SetAxisInfoMulti(int iBdID, WORD wAxes)
{
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukServoInfo, (long)(0x0FFFF & wAxes));
}

void SetAxisInfoDrop(int iBdID, WORD wAxes)
{
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropAxisInfo, (long)(0x0FFFF & wAxes));
}

int CheckParameterRange(BOOL bIsGlobal, long lParaNo, long lParaValue)
{
	if (bIsGlobal)
	{
		if ((lParaNo < NUM_GLOBAL_PARAM) && (lParaNo >= 0))
		{
			switch (lParaNo)
			{
			case 0:	 
				if ((lParaValue > 0) && (lParaValue <= MAX_AXIS_NUMS)) return FMM_OK; 
				break;
			case 4:
				if ((lParaValue >= 0) && (lParaValue <= 2)) return FMM_OK; 
				break;
			case 14:
			case 17:
				if ((lParaValue >= 0) && (lParaValue <= 3)) return FMM_OK; 
				break;
			case 8:  
				if ((lParaValue >= 0) && (lParaValue <= 4)) return FMM_OK; 
				break;
			case 6:  
				if ((lParaValue >= 0) && (lParaValue <= 6)) return FMM_OK; 
				break;
			case 15: 
				if ((lParaValue >= 1) && (lParaValue <= 100)) return FMM_OK; 
				break;
			case 11: 
				if ((lParaValue >= 1) && (lParaValue <= 500)) return FMM_OK; 
				break;
			case 10: 
				if ((lParaValue >= 1) && (lParaValue <= 9999)) return FMM_OK; 
				break;
			case 5:  
				if ((lParaValue >= 1000) && (lParaValue <= 99999)) return FMM_OK; 
				break;
			case 3:
			case 9:
				if ((lParaValue >= 1) && (lParaValue <= 4000000)) return FMM_OK; 
				break;
			case 1:
			case 2:
			case 7:
			case 12:
			case 13:
			case 16:
			case 28:
			case 29:
				if ((lParaValue >= 0) && (lParaValue <= 1)) return FMM_OK; 
				break;
			case 19:
			case 20:
			case 21:
			case 22:
			case 25:
			case 26:
				if ((lParaValue >= 0) && (lParaValue <= 10000)) return FMM_OK; 
				break;
			case 23:
			case 24:
				if ((lParaValue >= 0) && (lParaValue <= 500000)) return FMM_OK; 
				break;
			case 30:
				if ((lParaValue >= 1) && (lParaValue <= 65535)) return FMM_OK; 
				break;
			case 18:
			case 27:
				if ((lParaValue >= -10000) && (lParaValue <= 10000)) return FMM_OK; 
				break;
			default :
				return FMM_OK; 
			}

//			return FMM_PARAMETER_RANGE_ERROR;
		}
		else
			return FMM_INVALID_PARAMETER_NUM;
	}
	else
	{
		if ((lParaNo < NUM_LOCAL_PARAM) && (lParaNo >= 0))
		{
			switch (lParaNo)
			{
			case 26:
			case 31:
				if ((lParaValue >= 0) && (lParaValue <= 2)) return FMM_OK; break;
			case 1:
			case 35:
			case 43:
				if ((lParaValue >= 0) && (lParaValue <= 3)) return FMM_OK; break;
			case 39:
				if ((lParaValue >= 0) && (lParaValue <= 6)) return FMM_OK; break;
			case 13:
				if ((lParaValue >= 0) && (lParaValue <= 8)) return FMM_OK; break;
			case 38:
				if ((lParaValue >= 1) && (lParaValue <= 9)) return FMM_OK; break;
			case 40:
				if ((lParaValue >= 0) && (lParaValue <= 100)) return FMM_OK; break;
			case 7:
				if ((lParaValue >= 1) && (lParaValue <= 500)) return FMM_OK; break;
			case 41:
				if ((lParaValue >= 0) && (lParaValue <= 4095)) return FMM_OK; break;
			case 27:
			case 36:
				if ((lParaValue >= 0) && (lParaValue <= 9999)) return FMM_OK; break;
			case 28:
				if ((lParaValue >= 0) && (lParaValue <= 134217727)) return FMM_OK; break;
			case 32:
			case 33:
				if ((lParaValue >= 1) && (lParaValue <= 134217727)) return FMM_OK; break;
			case 0:
			case 2:
			case 14:
			case 17:
			case 18:
			case 19:
			case 20:
			case 21:
			case 22:
			case 23:
			case 24:
			case 34:
			case 42:
			case 52:
			case 54:
				if ((lParaValue >= 0) && (lParaValue <= 1)) return FMM_OK; break;
			case 5:
			case 6:
			case 9:
			case 12:
			case 25:
			case 37:
				if ((lParaValue >= 1) && (lParaValue <= 9999)) return FMM_OK; break;
			case 3:
			case 4:
			case 8:
				if ((lParaValue >= 1) && (lParaValue <= 4000000)) return FMM_OK; break;
			case 10:
			case 11:
				if ((lParaValue >= 1) && (lParaValue <= 1000000)) return FMM_OK; break;
			case 15:
			case 16:
			case 29:
			case 30:
				if ((lParaValue >= -134217727) && (lParaValue <= 134217727)) return FMM_OK; break;
			case 44:
				if ((lParaValue >= 0) && (lParaValue <= 5)) return FMM_OK; break;
			case 45:
			case 50:
			case 51:
				if ((lParaValue >= -100000) && (lParaValue <= 100000)) return FMM_OK; break;
			case 46:
			case 47:
			case 49:
				if ((lParaValue >= 0) && (lParaValue <= 100000)) return FMM_OK; break;
			case 48:
				if ((lParaValue >= 0) && (lParaValue <= 5000000)) return FMM_OK; break;
			case 53:
				if ((lParaValue >= 0) && (lParaValue <= 63)) return FMM_OK; break;
			case 55:
				if ((lParaValue >= -10000) && (lParaValue <= 10000)) return FMM_OK; break;
			case 56:
				if ((lParaValue >= 1) && (lParaValue <= 10000)) return FMM_OK; break;
			case 57:
				if ((lParaValue >= 0) && (lParaValue <= 1)) return FMM_OK; break;
			case 58:
				if ((lParaValue >= 1) && (lParaValue <= 100000)) return FMM_OK; break;
			default :
				return FMM_OK;
			}
//			return FMM_PARAMETER_RANGE_ERROR;
		}
		else if (lParaNo >= RANGE_4BIT_HEX)
		{
			switch (lParaNo)
			{
			case RANGE_4BIT_HEX:
				if ((lParaValue >= 0) && (lParaValue <= 15)) return FMM_OK;
			case RANGE_8BIT_HEX:
				if ((lParaValue >= 0x00) && (lParaValue <= 0xFF)) return FMM_OK;
			case RANGE_32BIT_HEX:
				if ((lParaValue >= 0) && (lParaValue <= 31)) return FMM_OK;
			}
		}
		else
			return FMM_INVALID_PARAMETER_NUM;
	}

	return FMM_OK;
}

void SetDpramCommand(int iBdID, WORD wAxis, int byCmd, int byOffset)
{
    gAcc.SetCmd(iBdID, wAxis, (BYTE)byCmd, (BYTE)byOffset);
}

int  WaitDpramCommand(int iBdID, WORD wAxis, BYTE byDpramCmd, DWORD dwmSec)
{
	switch (gAcc.WaitCmd(iBdID, wAxis, byDpramCmd, dwmSec))
	{
	case WAIT_OBJECT_0:	return FMM_OK;				break;
	case WAIT_TIMEOUT:	return FMM_TIMEOUT_ERROR;	break;
	}

	return FMM_UNKNOWN_ERROR;
}

void __GetAxisNumberPara(int iBdID, long *lAxisNo)
{
	long lValue = 0;

	if (!gAcc.IsOpen()) return;
	if (!gAcc.IsExist(iBdID)) return;

	ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd1 In\n", iBdID+1);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukGParaNumber, 0);

	SetDpramCommand(iBdID, AXIS_GLOBAL, ukGetGPara, ukGParaValue);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukGetGPara, 0);

	lValue = FAS_ReadRam(iBdID, AXIS_GLOBAL, ukGParaValue);

	LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd1 Out\n", iBdID+1);

	*lAxisNo = lValue; 
}

//------------------------------------------------------------------
//                        Initialized Functions.
//------------------------------------------------------------------
BOOL FAS_Open()
{
	int		i;
	long	lValue;
	BOOL	bRtn;

	if ( gAcc.Open() == FALSE) return FALSE;
    else 
	{
		//각 보드의 축 수를 알아낸다.
		for (i=0; i<MAX_CARD_NUMS; i++)
		{
			CREATE_SYNCHRONIZATION(i);
			FAS_IsAlive(i, &bRtn);
			
			if (bRtn)
			{
				//FAS_GetGlobalPara(i, 0, &lValue);
				__GetAxisNumberPara(i, &lValue);
				awNoAxis[i] = (WORD)lValue;
			}
			else
			{
				awNoAxis[i] = 0;
			}
		}
		return TRUE;
	}
}

BOOL FAS_Close()
{
	BOOL	bStatus = TRUE;
	int		i;
	WORD	j;

	for (i=0; i<MAX_CARD_NUMS; i++)
	{
		if (gAcc.IsExist(i))
		{
			FAS_IsAlive(i, &bStatus);
			
			if (bStatus)
			{
				FAS_ResetArcQueue(i);
				FAS_ResetArcMotionQueue(i);
				FAS_ResetLinearQueue(i);
				FAS_ResetCircleQueue(i);

				for (j=0; j<awNoAxis[i]; j++)
				{
					FAS_ResetAxisQueue(i, j);
					FAS_MoveStop(i, j, 0);
				}
			}
		}

		DELETE_SYNCHRONIZATION(i);
	}

	if ( gAcc.Close() == FALSE) return FALSE;
    else return TRUE;
}

int FAS_InitMotion()
{
	int		i;
	WORD	j;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;

	for (i=0; i<MAX_CARD_NUMS; i++)
	{
		if (gAcc.IsExist(i))
		{
			FAS_ResetArcQueue(i);
			FAS_ResetArcMotionQueue(i);
			FAS_ResetLinearQueue(i);
			FAS_ResetCircleQueue(i);

			for (j=0; j<awNoAxis[i]; j++)
			{
				FAS_ResetAxisQueue(i, j);
				FAS_MoveStop(i, j, 0);
			}
		}
	}
	
	return FMM_OK;
}

BOOL FAS_IsExist(int iBdID)
{
	return gAcc.IsExist(iBdID);
}

int FAS_IsAlive(int iBdID, BOOL *bStatus)
{
	if ((!gAcc.IsOpen()) || (!gAcc.IsExist(iBdID)))
	{
		*bStatus = FALSE;
		return FMM_INVALID_BD_NUM;
	}

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd2 In\n", iBdID+1);

 	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukGParaNumber, 1);	    
    SetDpramCommand(iBdID, AXIS_GLOBAL, ukGetGPara, 0);
    
	if (WaitDpramCommand(iBdID, AXIS_GLOBAL, ukGetGPara, 100) == FMM_OK)
	{
		LEAVE_SYNCHRONIZATION(iBdID);
		
		*bStatus = TRUE;
		return FMM_OK;
	}
	else
	{
		gAcc.ShakeDSP(iBdID);		// 다시 깨움.

		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukGParaNumber, 1);
		SetDpramCommand(iBdID, AXIS_GLOBAL, ukGetGPara, 0);
		
		if (WaitDpramCommand(iBdID, AXIS_GLOBAL, ukGetGPara, 100) == FMM_OK)
		{
			LEAVE_SYNCHRONIZATION(iBdID);
			//TRACE("Bd%d Cmd2 Out\n", iBdID+1);
			
			*bStatus = TRUE;
			return FMM_OK;
		}
		else
		{
			LEAVE_SYNCHRONIZATION(iBdID);
			//TRACE("Bd%d Cmd2 Out\n", iBdID+1);
			
			*bStatus = FALSE;
			return FMM_TIMEOUT_ERROR;
		}
	} 
}

//------------------------------------------------------------------
//                        Read/Write Functions.
//------------------------------------------------------------------

long FAS_ReadRam(int iBdID, WORD wAxis, DWORD dwOffset)
{
	return gAcc.ReadRam(iBdID, wAxis, dwOffset);
}

void FAS_ReadBlockRamB(int iBdID, int iAdd, BYTE *pbyData, long dwLen)
{
	gAcc.ReadBlockRamB( iBdID, iAdd, pbyData, dwLen);
}

void FAS_ReadBlockRam(int iBdID, int iAdd, BYTE *pbyData, long dwLen)
{
	gAcc.ReadBlockRam( iBdID, iAdd, pbyData, dwLen);
}

void FAS_WriteRam(int iBdID, WORD wAxis, DWORD dwOffset, long dwData)
{
	gAcc.WriteRam(iBdID, wAxis, dwOffset, dwData);
}

void FAS_WriteBlockRamB(int iBdID, int iAdd, BYTE *pbyData, DWORD dwLen)
{
	gAcc.WriteBlockRamB(iBdID, iAdd, pbyData, dwLen);
}


//------------------------------------------------------------------------------
//             Parameter  Functions
//------------------------------------------------------------------------------
int FAS_SaveParameter(int iBdID)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
    if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
	
	ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd3 In\n", iBdID+1);
    SetDpramCommand(iBdID, AXIS_GLOBAL, ukSavePara, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSavePara, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd3 Out\n", iBdID+1);

    return FMM_OK;
}

int FAS_SaveLocalPara(int iBdID, WORD wAxis, long lLParaNo)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

	if ((lLParaNo < 0) || (lLParaNo >= NUM_LOCAL_PARAM))
		return FMM_INVALID_PARAMETER_NUM;
    
	ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd4 In\n", iBdID+1);
	FAS_WriteRam(iBdID, wAxis, ukLParaNumber, lLParaNo);		
	SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukSaveLocalPara, 0);
    WaitDpramCommand(iBdID, wAxis, ukSaveLocalPara, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd4 Out\n", iBdID+1);
    
	return FMM_OK;
}

int FAS_SaveGlobalPara(int iBdID, long lGParaNo)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((lGParaNo < 0) || (lGParaNo >= NUM_GLOBAL_PARAM))
		return FMM_INVALID_PARAMETER_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd5 In\n", iBdID+1);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukGParaNumber, lGParaNo);

    SetDpramCommand(iBdID, AXIS_GLOBAL, ukSaveGlobalPara, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSaveGlobalPara, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd5 Out\n", iBdID+1);

    return FMM_OK;
}

int FAS_SetLocalPara(int iBdID, WORD wAxis, long lLParaNo, long lLParaValue)
{
	int nRtn;

    if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

	if ((lLParaNo < 0) || (lLParaNo >= NUM_LOCAL_PARAM))
		return FMM_INVALID_PARAMETER_NUM;
	
	nRtn = CheckParameterRange(FALSE, lLParaNo, lLParaValue);
    if (nRtn != FMM_OK) return nRtn;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd6 In\n", iBdID+1);

	FAS_WriteRam(iBdID, wAxis, ukLParaNumber, lLParaNo);		
	FAS_WriteRam(iBdID, wAxis, ukLParaValue,  lLParaValue);

	SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukSetLPara, 0);
    WaitDpramCommand(iBdID, wAxis, ukSetLPara, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd6 Out\n", iBdID+1);
    
	return FMM_OK;
}

int FAS_SetGlobalPara(int iBdID, long lGParaNo, long lGParaValue)
{
	int nRtn;

    if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((lGParaNo < 0) || (lGParaNo >= NUM_GLOBAL_PARAM))
		return FMM_INVALID_PARAMETER_NUM;

	nRtn = CheckParameterRange(TRUE, lGParaNo, lGParaValue);
    if (nRtn != FMM_OK) return nRtn;
    
    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd7 In\n", iBdID+1);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukGParaNumber, lGParaNo);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukGParaValue, lGParaValue);

    SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGPara, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGPara, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd7 Out\n", iBdID+1);

    return FMM_OK;
}

int FAS_GetLocalPara(int iBdID, WORD wAxis, long lLParaNo, long *lGetLocalPara)
{
    long lTmpLP = 0;
	
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

	if ((lLParaNo < 0) || (lLParaNo >= NUM_LOCAL_PARAM))
		return FMM_INVALID_PARAMETER_NUM;
    
    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd8 In\n", iBdID+1);

 	FAS_WriteRam(iBdID, wAxis, ukLParaNumber,lLParaNo);	    
    SetAxisInfo(iBdID, wAxis);	    
    SetDpramCommand(iBdID, wAxis, ukGetLPara, ukLParaValue);
	WaitDpramCommand(iBdID, wAxis, ukGetLPara, 0);

	lTmpLP = FAS_ReadRam(iBdID, wAxis, ukLParaValue);

	LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd8 Out\n", iBdID+1);
		    
	*lGetLocalPara = lTmpLP;
	
	return FMM_OK;
}

int FAS_GetGlobalPara(int iBdID, long lGParaNo, long *lGetGlobalPara)
{
    long lTmpGP = 0;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
    
	if ((lGParaNo < 0) || (lGParaNo >= NUM_GLOBAL_PARAM))
		return FMM_INVALID_PARAMETER_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd9 In\n", iBdID+1);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukGParaNumber, lGParaNo);

    SetDpramCommand(iBdID, AXIS_GLOBAL, ukGetGPara, ukGParaValue);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukGetGPara, 0);

	lTmpGP = FAS_ReadRam(iBdID, AXIS_GLOBAL, ukGParaValue);

    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd9 Out\n", iBdID+1);

	*lGetGlobalPara = lTmpGP; 
	
	return FMM_OK;
}

int FAS_GetRomGlobalPara(int iBdID, long lGParaNo, long *lGetRomGlobalPara)
{
    long lTmpGP;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((lGParaNo < 0) || (lGParaNo >= NUM_GLOBAL_PARAM))
		return FMM_INVALID_PARAMETER_NUM;
    
    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd10 In\n", iBdID+1);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukGParaNumber, lGParaNo);

    SetDpramCommand(iBdID, AXIS_GLOBAL, ukGetRomGPara, ukGParaValue);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukGetRomGPara, 0);

	lTmpGP = FAS_ReadRam(iBdID, AXIS_GLOBAL, ukGParaValue);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd10 Out\n", iBdID+1);

	*lGetRomGlobalPara  = lTmpGP; 
	
	return FMM_OK;
}

int FAS_GetRomLocalPara(int iBdID, WORD wAxis, long lLParaNo, long *lGetRomLocalPara)
{
    long lTmpLP ;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
    
	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

	if ((lLParaNo < 0) || (lLParaNo >= NUM_LOCAL_PARAM))
		return FMM_INVALID_PARAMETER_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd11 In\n", iBdID+1);
 	FAS_WriteRam(iBdID, wAxis, ukLParaNumber,lLParaNo);	    
    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukGetRomLPara, ukLParaValue);
    WaitDpramCommand(iBdID, wAxis, ukGetRomLPara, 0);
    
	lTmpLP = FAS_ReadRam(iBdID, wAxis, ukLParaValue);
	LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd11 Out\n", iBdID+1);
		    
	*lGetRomLocalPara = lTmpLP;

	return FMM_OK;
}


//------------------------------------------------------------------------------
//             Servo Driver Control Functions
//------------------------------------------------------------------------------
int FAS_ServoEnable(int iBdID, WORD wAxis, BOOL bOnOff)
{
    if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd12 In\n", iBdID+1);
    FAS_WriteRam(iBdID, wAxis, ukTrueOrFalse, bOnOff);
 	SetAxisInfo(iBdID, wAxis);

	SetDpramCommand(iBdID, wAxis, ukServoEnable, 0);
    WaitDpramCommand(iBdID, wAxis, ukServoEnable, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd12 Out\n", iBdID+1);

    return FMM_OK;
}

int FAS_ServoAlarmReset(int iBdID, WORD wAxis)
{
    if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
    
	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd13 In\n", iBdID+1);
    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukClearServoError, 0);
    WaitDpramCommand(iBdID, wAxis, ukClearServoError, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd13 Out\n", iBdID+1);

    return FMM_OK;
}

int FAS_ServoErrorCounterClear(int iBdID, WORD wAxis)
{
    if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
	
	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;
    
    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd14 In\n", iBdID+1);
    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukClearServoCounter, 0);
    WaitDpramCommand(iBdID, wAxis, ukClearServoCounter, 0);
    LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}

//------------------------------------------------------------------------------
//             IO Functions
//------------------------------------------------------------------------------
int FAS_SetIo(int iBdID, BOOL bIsLow, DWORD dwIoValue)
{
    if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
	
    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd15 In\n", iBdID+1);
	if (bIsLow)
	{
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoValue, dwIoValue);
    
		SetDpramCommand(iBdID, AXIS_GLOBAL, ukIoWriteLow, 0);
		WaitDpramCommand(iBdID, AXIS_GLOBAL, ukIoWriteLow, 0);
	}
	else
	{
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoValue, dwIoValue);
    
		SetDpramCommand(iBdID, AXIS_GLOBAL, ukIoWriteHigh, 0);
		WaitDpramCommand(iBdID, AXIS_GLOBAL, ukIoWriteHigh, 0);
	}
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd15 Out\n", iBdID+1);

    return FMM_OK;
}

int FAS_SetIoBit(int iBdID, BOOL bIsLow, WORD wBitNo, BOOL bOnOrOff)
{
	int nRtn;

    if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
	
	nRtn = CheckParameterRange(FALSE, RANGE_32BIT_HEX, wBitNo);
	if (nRtn != FMM_OK) return nRtn;
    
    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd16 In\n", iBdID+1);
	if (bIsLow)
	{
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoBitNo, wBitNo);
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoValue, bOnOrOff);

		SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetIoBitLow, 0);
		WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetIoBitLow, 0);
	}
	else
	{
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoBitNo, wBitNo);
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoValue, bOnOrOff);

		SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetIoBitHigh, 0);
		WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetIoBitHigh, 0);
	}
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd16 Out\n", iBdID+1);

    return FMM_OK;
}

int FAS_GetIo(int iBdID, BOOL bIsLow, DWORD *dwGetIo)
{
    DWORD dwTmpIo;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
	
    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd17 In\n", iBdID+1);
	if (bIsLow)
	{
		SetDpramCommand(iBdID, AXIS_GLOBAL, ukIoReadLow, ukIoValue);
		WaitDpramCommand(iBdID, AXIS_GLOBAL, ukIoReadLow, 0);

		dwTmpIo = FAS_ReadRam(iBdID, AXIS_GLOBAL, ukIoValue);
	}
	else
	{
		SetDpramCommand(iBdID, AXIS_GLOBAL, ukIoReadHigh, ukIoValue);
		WaitDpramCommand(iBdID, AXIS_GLOBAL, ukIoReadHigh, 0);

		dwTmpIo = FAS_ReadRam(iBdID, AXIS_GLOBAL, ukIoValue);
	}
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd17 Out\n", iBdID+1);

	*dwGetIo = dwTmpIo;

	return FMM_OK;
}

int FAS_GetIoBit(int iBdID, BOOL bIsLow, WORD wBitNo,  BOOL *bGetIoBit)
{
    long ltmpBit;
	int nRtn;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
	
	nRtn = CheckParameterRange(FALSE, RANGE_32BIT_HEX, wBitNo);
	if (nRtn != FMM_OK) return nRtn;

	ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd18 In\n", iBdID+1);
	if (bIsLow)
	{
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoBitNo, wBitNo);
		SetDpramCommand(iBdID, AXIS_GLOBAL, ukGetIoBitLow, ukIoValue);

		WaitDpramCommand(iBdID, AXIS_GLOBAL, ukGetIoBitLow, 0);

		ltmpBit = FAS_ReadRam(iBdID, AXIS_GLOBAL, ukIoValue);
	}
	else
	{
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoBitNo, wBitNo);
		SetDpramCommand(iBdID, AXIS_GLOBAL, ukGetIoBitHigh, ukIoValue);

		WaitDpramCommand(iBdID, AXIS_GLOBAL, ukGetIoBitHigh, 0);

		ltmpBit = FAS_ReadRam(iBdID, AXIS_GLOBAL, ukIoValue);
	}
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd18 Out\n", iBdID+1);

    *bGetIoBit = (ltmpBit & 0x0FFFF) ? TRUE : FALSE;

	return FMM_OK;
}


int FAS_GetIoOutputStatus(int iBdID, BOOL bIsLow, DWORD *dwIoOutputStatus)
{
    DWORD dwIoTmp;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd19 In\n", iBdID+1);
	if (bIsLow)
	{
		SetDpramCommand(iBdID, AXIS_GLOBAL, ukGetOutputStatusLow, ukIoValue);
		WaitDpramCommand(iBdID, AXIS_GLOBAL, ukGetOutputStatusLow, 0);

		dwIoTmp = FAS_ReadRam(iBdID, AXIS_GLOBAL, ukIoValue);
	}
	else
	{
		SetDpramCommand(iBdID, AXIS_GLOBAL, ukGetOutputStatusHigh, ukIoValue);
		WaitDpramCommand(iBdID, AXIS_GLOBAL, ukGetOutputStatusHigh, 0);

		dwIoTmp = FAS_ReadRam(iBdID, AXIS_GLOBAL, ukIoValue);
	}
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd19 Out\n", iBdID+1);

	*dwIoOutputStatus = dwIoTmp;
    
	return FMM_OK;
}

//------------------------------------------------------------------------------
//            Read Position
//------------------------------------------------------------------------------
int FAS_SetCommandPos(int iBdID, WORD wAxis, double dSetCmdPos)
{
	int nRtn;

    if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
	
	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

	nRtn = CheckParameterRange(FALSE, 15, (long)dSetCmdPos);
	if (nRtn != FMM_OK) return nRtn;
    
    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd20 In\n", iBdID+1);
    FAS_WriteRam(iBdID, wAxis, ukGoalPosition, (long)dSetCmdPos);
   
	SetAxisInfo(iBdID, wAxis);
    SetDpramCommand(iBdID, wAxis, ukSetCommandPos, 0);
    WaitDpramCommand(iBdID, wAxis, ukSetCommandPos, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd20 Out\n", iBdID+1);

    return FMM_OK;
}


int FAS_SetActualPos(int iBdID, WORD wAxis, double dSetActualPos)
{
	int nRtn;
    if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
	
	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

	nRtn = CheckParameterRange(FALSE, 15, (long)dSetActualPos);
	if (nRtn != FMM_OK) return nRtn;

	ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd21 In\n", iBdID+1);
    FAS_WriteRam(iBdID, wAxis, ukGoalPosition, (long)dSetActualPos);
    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukSetCurrentPos, 0);
    WaitDpramCommand(iBdID, wAxis, ukSetCurrentPos, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd21 Out\n", iBdID+1);

    return FMM_OK;
} 

int FAS_GetCommandPos(int iBdID, WORD wAxis, double *dGetCmdPos)
{
    long lTmp;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
	
	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;
    
    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd22 In\n", iBdID+1);
    SetAxisInfo(iBdID, wAxis);
    SetDpramCommand(iBdID, wAxis, ukGetCommandPos, ukGoalPosition);
    WaitDpramCommand(iBdID, wAxis, ukGetCommandPos, 0);

	lTmp = FAS_ReadRam(iBdID, wAxis, ukGoalPosition);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd22 Out\n", iBdID+1);

    *dGetCmdPos = (double)lTmp;

    return FMM_OK;
}

int FAS_GetActualPos(int iBdID, WORD wAxis,  double *dGetActualPos)
{
    long lTmp;
	
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
	
	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd23 In\n", iBdID+1);
    SetAxisInfo(iBdID, wAxis);
    SetDpramCommand(iBdID, wAxis, ukReadCurrentPos, ukCurrentPos);
    WaitDpramCommand(iBdID, wAxis, ukReadCurrentPos, 0);

	lTmp = FAS_ReadRam(iBdID, wAxis, ukCurrentPos);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd23 Out\n", iBdID+1);

    *dGetActualPos = (double)lTmp;

    return FMM_OK;
}

int FAS_GetPosError(int iBdID, WORD wAxis, double *dGetPosError)
{
    long lTmp;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd24 In\n", iBdID+1);
    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukReadCurrentError, ukTrackingError);
    WaitDpramCommand(iBdID, wAxis, ukReadCurrentError, 0);

	lTmp = FAS_ReadRam(iBdID, wAxis, ukTrackingError);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd24 Out\n", iBdID+1);

    *dGetPosError = (double)lTmp;

    return FMM_OK;
}
 
int FAS_GetTrackingError(int iBdID, WORD wAxis, double *dGetTrackingError)
{
    long lTmp;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
	
	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd25 In\n", iBdID+1);
    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukGetTrackingError, ukTrackingError);
    WaitDpramCommand(iBdID, wAxis, ukGetTrackingError, 0);

	lTmp = FAS_ReadRam(iBdID, wAxis, ukTrackingError);
    LEAVE_SYNCHRONIZATION(iBdID);

    *dGetTrackingError = (double)lTmp;

    return FMM_OK;
}

int FAS_GetActualVel(int iBdID, WORD wAxis, double *dGetActualVel)
{
    long lTmp;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
	
	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd26 In\n", iBdID+1);
    SetAxisInfo(iBdID, wAxis);
    SetDpramCommand(iBdID, wAxis, ukReadCurrentVel, ukCurrentVel);
    WaitDpramCommand(iBdID, wAxis, ukReadCurrentVel, 0);

	lTmp = FAS_ReadRam(iBdID, wAxis, ukCurrentVel);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd26 Out\n", iBdID+1);

    *dGetActualVel = (double)lTmp;
    
    return FMM_OK;
}

//------------------------------------------------------------------------------
//			  Motin Queue 
//------------------------------------------------------------------------------
int FAS_GetFreePosCmdQueue(int iBdID, WORD *wFreeCmdQueue)
{
	long lTmpQ;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
    
    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd27 In\n", iBdID+1);
    SetDpramCommand(iBdID, AXIS_GLOBAL, ukGetNoFreeQ, ukFreePosCmdQ);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukGetNoFreeQ, 0);
   
	lTmpQ = FAS_ReadRam(iBdID, AXIS_GLOBAL, ukFreePosCmdQ);
    LEAVE_SYNCHRONIZATION(iBdID);

	*wFreeCmdQueue = (WORD)(lTmpQ & 0x0FFFF);
    return FMM_OK;
}

int FAS_GetFreePosIoQueue(int iBdID, WORD *wFreeIoQueue)
{
    long lTmpQ;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
    
    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd28 In\n", iBdID+1);
    SetDpramCommand(iBdID, AXIS_GLOBAL, ukGetPosIoCmdQ, ukFreePosCmdQ);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukGetPosIoCmdQ, 0);

	lTmpQ = FAS_ReadRam(iBdID, AXIS_GLOBAL, ukFreePosCmdQ);
    LEAVE_SYNCHRONIZATION(iBdID);

	*wFreeIoQueue = (WORD)(lTmpQ & 0x0FFFF) ;

    return FMM_OK;
}

//------------------------------------------------------------------------------
//			  D/A  
//------------------------------------------------------------------------------
int FAS_SetDacOutput(int iBdID, WORD wChannel, int iMVolt)
{
//		Insert Limit Check
//		0x3fff -> 10 [Volt]
//		0x2000 ->  0 [Volt]
//		0x0000 ->-10 [Volt]

    int iDaValue;
    int iDaData;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
    
    iDaValue = (iMVolt >= 10000) ? 10000 : iMVolt;
    iDaValue = (iMVolt <= -10000) ? -10000 : iMVolt;

    if (iMVolt >= 0) iDaData = 0x2000 + (long)((FLOAT)iDaValue * 0.001 * 0x1fff/10.0);
    else iDaData = 0x2000 - (long)((FLOAT)iDaValue * 0.001 * 0x1fff / 10.0);

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd29 In\n", iBdID+1);
    FAS_WriteRam(iBdID, wChannel, ukDaOutput, iDaData);

    SetAxisInfo(iBdID, wChannel);

    SetDpramCommand(iBdID, wChannel, ukSetDaIndex, 0);
    WaitDpramCommand(iBdID, wChannel, ukSetDaIndex, 0);
    LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}

int FAS_SetDaOffset(int iBdID, WORD wChannel, int iMVolt)
{
    int iDaOffsetValue;
    int iDaOffsetData;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
    
    iDaOffsetValue = (iMVolt >= 10000) ? 10000 : iMVolt;
    iDaOffsetValue = (iMVolt < -10000) ? -10000 : iMVolt;

    if (iMVolt >= 0) iDaOffsetData = 0x2000 + (int)((double)iDaOffsetValue * 0.001 * 0x1fff / 10.0);
    else iDaOffsetData = 0x2000 - (int)((double)iDaOffsetValue * 0.001 * 0x1fff / 10.0);

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd30 In\n", iBdID+1);
    FAS_WriteRam(iBdID, wChannel, ukDaOutput, iDaOffsetData);

    SetAxisInfo(iBdID, wChannel);

    SetDpramCommand(iBdID, wChannel, ukSetDaOffset, 0);
    WaitDpramCommand(iBdID, wChannel, ukSetDaOffset, 0);
    LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}

int FAS_GetDacOutput(int iBdID, WORD wChannel, int iMVolt, WORD *wGetDaOutput)
{
	long lTmpDa ;
    
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
    
	ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd31 In\n", iBdID+1);
    FAS_WriteRam(iBdID, wChannel, ukDaOutput, iMVolt);
    SetAxisInfo(iBdID, wChannel);

    SetDpramCommand(iBdID, wChannel, ukGetDaIndex, ukDaOutput);
    WaitDpramCommand(iBdID, wChannel, ukGetDaIndex, 0);
    
	lTmpDa = FAS_ReadRam(iBdID, wChannel, ukDaOutput);
    LEAVE_SYNCHRONIZATION(iBdID);

	*wGetDaOutput = (WORD)(lTmpDa & 0x0FFFF) ; 

    return FMM_OK;
}
 
//------------------------------------------------------------------
//            Axis Status
//------------------------------------------------------------------
int FAS_GetAxisStatus(int iBdID, WORD wAxis, long *lAxisStatus)
{
    long dwTmp;
	int nRtn;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd32 In\n", iBdID+1);
    SetAxisInfo(iBdID, wAxis);
    
	SetDpramCommand(iBdID, wAxis, ukGetServoStatus, ukServoStatus);
    nRtn = WaitDpramCommand(iBdID, wAxis, ukGetServoStatus, 0);

	dwTmp = FAS_ReadRam(iBdID, wAxis, ukServoStatus);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd32 Out\n", iBdID+1);
	
	*lAxisStatus = dwTmp;

	return nRtn;
}

////////////////////////////////////////////////////////////////////
//               2004 07 16일 추가됨
int FAS_IsMotioning(int iBdID, WORD wAxis, BOOL *bMotioning)
{
	long AxisStatus;
	int  Count = 0;
	int	 i;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;
	
	for (i=0; i<3; i++)
	{
		FAS_GetAxisStatus(iBdID, wAxis, &AxisStatus);  	
		if ((FLAG_CHECK_MOTIONING & (DWORD)AxisStatus) == 0x00) 
			Count++;
	}
	*bMotioning = (Count < 3) ? TRUE : FALSE;
	
    return FMM_OK;
}

//------------------------------------------------------------------
//                Clear Functions.
//------------------------------------------------------------------

int FAS_ClearErrorCounter(int iBdID, WORD wAxis)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd33 In\n", iBdID+1);
    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukClearServoCounter, 0);
    WaitDpramCommand(iBdID, wAxis, ukClearServoCounter, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd33 Out\n", iBdID+1);

    return FMM_OK;
}

int FAS_ClearErrorStatus(int iBdID, WORD wAxis)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd34 In\n", iBdID+1);
    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukClearErrorStatus, 0);
    WaitDpramCommand(iBdID, wAxis, ukClearErrorStatus, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd34 Out\n", iBdID+1);

    return FMM_OK;
}


////////////////////////////////////////////////////////////////////////////////
///      세부적인 함수 부분.   (Motion 관련)                                ////
////////////////////////////////////////////////////////////////////////////////

int FAS_MoveJog(int iBdID, WORD wAxis, int iJogDir)
{
	DWORD dwDir = (iJogDir != 1) ? -1 : 1;

    if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

	if ((iJogDir != DIR_INC) && (iJogDir != DIR_DEC))
		return FMM_PARAMETER_RANGE_ERROR;
	
	ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd35 In\n", iBdID+1);
	FAS_WriteRam(iBdID, wAxis, ukJogDir, dwDir);
    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukJog, 0);
    WaitDpramCommand(iBdID, wAxis, ukJog, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd35 Out\n", iBdID+1);

    return FMM_OK;
}


int FAS_MoveStop(int iBdID, WORD wAxis, WORD wEndCheck)
{
	long AxisStatus ;
	int  MotionCount = 0;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
    
	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd36 In\n", iBdID+1);
	FAS_WriteRam(iBdID, wAxis, ukEndCheck, wEndCheck);
    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukStopMotionAxis, 0);
    WaitDpramCommand(iBdID, wAxis, ukStopMotionAxis, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd36 Out\n", iBdID+1);
	
	if (wEndCheck != 0)
	{
		do
		{
			FAS_GetAxisStatus(iBdID, wAxis, &AxisStatus);  	
			if (!(AxisStatus & FFLAG_MOTIONING))
				MotionCount ++ ;
		} while(MotionCount < 10);
	}
	return FMM_OK;
}

int FAS_EmergencyStop(int iBdID, WORD wAxis, WORD wEndCheck)
{
	long AxisStatus ;
	int  MotionCount = 0;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
    
	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd37 In\n", iBdID+1);
	FAS_WriteRam(iBdID, wAxis, ukEndCheck, wEndCheck);
    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukEmergencyStop, 0);
    WaitDpramCommand(iBdID, wAxis, ukEmergencyStop, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd37 Out\n", iBdID+1);
	
	if (wEndCheck != 0)
	{
		do
		{
			FAS_GetAxisStatus(iBdID, wAxis, &AxisStatus);  	
			if (!(AxisStatus & FFLAG_MOTIONING))
				MotionCount ++ ;
		} while(MotionCount < 10);
	}

    return FMM_OK;
}

int FAS_MoveOriginSingleAxis(int iBdID, WORD wAxis, WORD wEndCheck)
{
	long AxisStatus ;
	int  MotionCount = 0;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
    
	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd38 In\n", iBdID+1);
    FAS_WriteRam(iBdID, wAxis, ukEndCheck, wEndCheck);
    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukDoOriginSingleAxis, 0);
    WaitDpramCommand(iBdID, wAxis, ukDoOriginSingleAxis, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd38 Out\n", iBdID+1);
    
	if (wEndCheck !=0)
    {   
		do
        {
		   FAS_GetAxisStatus(iBdID, wAxis, &AxisStatus);  	
		   if (!(AxisStatus & FFLAG_MOTIONING))
			   MotionCount ++ ;
        } while(MotionCount < 10);
		
        FAS_GetAxisStatus(iBdID, wAxis, &AxisStatus);  				
		if ( (AxisStatus & FFLAG_ZERORETOK) )  
			return FMM_OK;
		else
			return FMM_ZERO_RETURN_ERROR;
    } 
	else
		return FMM_OK;
}

int FAS_MoveSingleAxisAbsPos(int iBdID, WORD wAxis, double dAbsPos, double dVelocity, WORD wEndCheck)
{
   	long AxisStatus ;
    int  nRtn, MotionCount = 0 ;
	long lValue;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
    
	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

	nRtn = CheckParameterRange(FALSE, 15, (long)dAbsPos);
	if (nRtn != FMM_OK) return nRtn;
	nRtn = CheckParameterRange(FALSE, 3, (long)dVelocity);
	if (nRtn != FMM_OK) return nRtn;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd39 In\n", iBdID+1);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck, wEndCheck);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode, ukExactStopMode);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukG00);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc, ukAbs);
    
	FAS_WriteRam(iBdID, wAxis, ukG00Speed, (long)dVelocity); 
	FAS_WriteRam(iBdID, wAxis, ukGoalPosition, (long)dAbsPos); 
	FAS_WriteRam(iBdID, wAxis, ukTJDAxisOn, 0);
	
    SetAxisInfo(iBdID, wAxis);
	
    SetDpramCommand(iBdID, wAxis, ukSetGoalPos, 0);
	lValue = FAS_ReadRam(iBdID, wAxis, ukG00Speed);
    WaitDpramCommand(iBdID, wAxis, ukSetGoalPos, 0); 
	lValue = FAS_ReadRam(iBdID, wAxis, ukG00Speed);
	LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd39 Out\n", iBdID+1);

	if (wEndCheck != 0)
    {
		do
        {
		   FAS_GetAxisStatus(iBdID, wAxis, &AxisStatus);  	
		   if (!(AxisStatus & FFLAG_MOTIONING))
			   MotionCount ++ ;
        } while(MotionCount < 10);
    }
	return FMM_OK;
}


int FAS_MoveSingleAxisIncPos(int iBdID, WORD wAxis, double dIncPos, double dVelocity, WORD wEndCheck)
{
    long AxisStatus ;
    int  nRtn, MotionCount = 0 ;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

	nRtn = CheckParameterRange(FALSE, 15, (long)dIncPos);
	if (nRtn != FMM_OK) return nRtn;
	nRtn = CheckParameterRange(FALSE, 3, (long)dVelocity);
	if (nRtn != FMM_OK) return nRtn;

	ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd40 In\n", iBdID+1);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck, wEndCheck);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode, ukExactStopMode);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukG00);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc, ukInc);
    FAS_WriteRam(iBdID, wAxis, ukG00Speed, (long)dVelocity);
    FAS_WriteRam(iBdID, wAxis, ukGoalPosition, (long)dIncPos);
	FAS_WriteRam(iBdID, wAxis, ukTJDAxisOn, 0);

    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukSetGoalPos, 0);
    WaitDpramCommand(iBdID, wAxis, ukSetGoalPos, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd40 Out\n", iBdID+1);
    
	if (wEndCheck != 0)
    {
		do
        {
		   FAS_GetAxisStatus(iBdID, wAxis, &AxisStatus);  	
		   if (!(AxisStatus & FFLAG_MOTIONING))
			   MotionCount ++ ;
        } while(MotionCount < 10);
    }
    return FMM_OK;
}

int FAS_MoveMultiAxisAbsPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdAbsPos, double *pdVelocity, WORD wEndCheck)
{
    int i;
    int iTmpAxis = 0;
	long AxisStatus ;
    int  nRtn, MotionCount = 0 ;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	for (i=0; i<wNoOfAxis; i++)
	{
		if ((awNoAxis[iBdID] <= piAxis[i]) || (piAxis[i] < 0))
			return FMM_INVALID_AXIS_NUM;
		nRtn = CheckParameterRange(FALSE, 15, (long)pdAbsPos[i]);
		if (nRtn != FMM_OK) return nRtn;
		nRtn = CheckParameterRange(FALSE, 3, (long)pdVelocity[i]);
		if (nRtn != FMM_OK) return nRtn;
	}
    
    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd41 In\n", iBdID+1);
    for (i=0; i<wNoOfAxis; i++)
    {
        FAS_WriteRam(iBdID, piAxis[i], ukG00Speed, (long)pdVelocity[i]);
        FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition,  (long)pdAbsPos[i]);
		FAS_WriteRam(iBdID, piAxis[i], ukTJDAxisOn, 0);

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
    }

    SetAxisInfoMulti(iBdID, iTmpAxis);

    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck, wEndCheck);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode, ukExactStopMode);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukG00);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc, ukAbs);
	
    SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd41 Out\n", iBdID+1);
    
    if (wEndCheck != 0)
    {
       for(i=0; i<wNoOfAxis; i++)
       {  
		   do
		   {   
			  FAS_GetAxisStatus(iBdID, piAxis[i], &AxisStatus);  	
			  if (!(AxisStatus & FFLAG_MOTIONING))
				 MotionCount++;
           } while(MotionCount < 10);
		   
		   MotionCount = 0;
       } 
    }
    return FMM_OK;
}

int FAS_MoveMultiAxisIncPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, WORD wEndCheck)
{
    int i;
    int iTmpAxis = 0;
    long AxisStatus ;
    int  nRtn, MotionCount = 0 ;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	for (i=0; i<wNoOfAxis; i++)
	{
		if ((awNoAxis[iBdID] <= piAxis[i]) || (piAxis[i] < 0))
			return FMM_INVALID_AXIS_NUM;
		nRtn = CheckParameterRange(FALSE, 15, (long)pdIncPos[i]);
		if (nRtn != FMM_OK) return nRtn;
		nRtn = CheckParameterRange(FALSE, 3, (long)pdVelocity[i]);
		if (nRtn != FMM_OK) return nRtn;
	}
    
    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd42 In\n", iBdID+1);

    for(i=0; i<wNoOfAxis; i++)
    {
        FAS_WriteRam(iBdID, piAxis[i], ukG00Speed, (long)pdVelocity[i]);
        FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition, (long)pdIncPos[i]);
		FAS_WriteRam(iBdID, piAxis[i], ukTJDAxisOn, 0);

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
    }

    SetAxisInfoMulti(iBdID, iTmpAxis);

    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck, wEndCheck);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode, ukExactStopMode);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukG00);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc, ukInc);

    SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd42 Out\n", iBdID+1);
   
	if (wEndCheck != 0)
    {
       for(i=0; i<wNoOfAxis; i++)
       {  
		   do
		   {   
			  FAS_GetAxisStatus(iBdID, piAxis[i], &AxisStatus);  	
			  if (!(AxisStatus & FFLAG_MOTIONING))
				 MotionCount ++ ;
           } while(MotionCount < 10);
		   
		   MotionCount = 0 ;
       } 
    }
    return FMM_OK;
}

//////////////////////////////////////////////////////////////////////

int FAS_MoveLinearIncPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdIncPos, double dFeedrate, WORD wEndCheck)
{
    int i;
    int iTmpAxis = 0;
    
	long AxisStatus ;
    int  nRtn, MotionCount = 0;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	for (i=0; i<wNoOfAxis; i++)
	{
		if ((awNoAxis[iBdID] <= piAxis[i]) || (piAxis[i] < 0))
			return FMM_INVALID_AXIS_NUM;
		nRtn = CheckParameterRange(FALSE, 15, (long)pdIncPos[i]);
		if (nRtn != FMM_OK) return nRtn;
	}
	nRtn = CheckParameterRange(TRUE, 9, (long)dFeedrate);
	if (nRtn != FMM_OK) return nRtn;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd43 In\n", iBdID+1);

    for(i=0; i<wNoOfAxis; i++)
    {
        FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition, (long)pdIncPos[i]);

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
    }

    SetAxisInfoMulti(iBdID, iTmpAxis);

    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck, wEndCheck);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode, ukExactStopMode);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate, (long)dFeedrate);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukG01);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc, ukInc);
    
    SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
    
	if (wEndCheck != 0)
    {
       for(i=0; i<wNoOfAxis; i++)
       {  
		   do
		   {   
			  FAS_GetAxisStatus(iBdID, piAxis[i], &AxisStatus);  	
			  if (!(AxisStatus & FFLAG_MOTIONING))
				 MotionCount ++ ;
           } while(MotionCount < 10);
		   
		   MotionCount = 0 ;
       } 
    }
    return FMM_OK;
}

int FAS_MoveLinearAbsPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdAbsPos, double  dFeedrate, WORD wEndCheck)
{
    int i;
    int iTmpAxis = 0;
   
	long AxisStatus ;
    int  nRtn, MotionCount = 0 ;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	for (i=0; i<wNoOfAxis; i++)
	{
		if ((awNoAxis[iBdID] <= piAxis[i]) || (piAxis[i] < 0))
			return FMM_INVALID_AXIS_NUM;
		nRtn = CheckParameterRange(FALSE, 15, (long)pdAbsPos[i]);
		if (nRtn != FMM_OK) return nRtn;
	}

	nRtn = CheckParameterRange(TRUE, 9, (long)dFeedrate);
	if (nRtn != FMM_OK) return nRtn;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd44 In\n", iBdID+1);

    for (i=0; i<wNoOfAxis; i++)
    {
        FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition, (long)pdAbsPos[i]);

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
    }

    SetAxisInfoMulti(iBdID, iTmpAxis);

    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukG01);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck, wEndCheck);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode, ukExactStopMode);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate, (long)dFeedrate);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc, ukAbs);

    SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    LEAVE_SYNCHRONIZATION(iBdID);

	if (wEndCheck != 0)
    {
       for(i=0; i<wNoOfAxis; i++)
       {  
		   do
		   {   
			  FAS_GetAxisStatus(iBdID, piAxis[i], &AxisStatus);  	
			  if (!(AxisStatus & FFLAG_MOTIONING))
				 MotionCount ++ ;
           } while(MotionCount < 10);
		   
		   MotionCount = 0 ;
       } 
    }
    return FMM_OK;
}

int FAS_MoveContLinearAbsPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdAbsPos,double dFeedrate, WORD wStartOrEnd)
{
    int i;
    int iTmpAxis = 0;
    int  nRtn;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	for (i=0; i<wNoOfAxis; i++)
	{
		if ((awNoAxis[iBdID] <= piAxis[i]) || (piAxis[i] < 0))
			return FMM_INVALID_AXIS_NUM;

		nRtn = CheckParameterRange(FALSE, 15, (long)pdAbsPos[i]);
		if (nRtn != FMM_OK) return nRtn;
	}
	nRtn = CheckParameterRange(TRUE, 9, (long)dFeedrate);
	if (nRtn != FMM_OK) return nRtn;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd45 In\n", iBdID+1);

    for (i=0; i<wNoOfAxis; i++)
    {
        FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition, (long)pdAbsPos[i]);

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
    }

    SetAxisInfoMulti(iBdID, iTmpAxis);

    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,     ukContInterMode);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,      (long)dFeedrate);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck,      wStartOrEnd);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukG01Cont);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,      ukAbs);

    SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	
    return FMM_OK;
}

int FAS_MoveContLinearIncPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdIncPos, double dFeedrate, WORD wStartOrEnd)
{
    int i;
    int iTmpAxis = 0;
    int nRtn;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	for (i=0; i<wNoOfAxis; i++)
	{
		if ((awNoAxis[iBdID] <= piAxis[i]) || (piAxis[i] < 0))
			return FMM_INVALID_AXIS_NUM;
		nRtn = CheckParameterRange(FALSE, 15, (long)pdIncPos[i]);
		if (nRtn != FMM_OK) return nRtn;
	}

	nRtn = CheckParameterRange(TRUE, 9, (long)dFeedrate);
	if (nRtn != FMM_OK) return nRtn;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd46 In\n", iBdID+1);
    for (i=0; i<wNoOfAxis; i++)
    {
        FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition, (long)pdIncPos[i]);

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
    }

    SetAxisInfoMulti(iBdID, iTmpAxis);

    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,     ukContInterMode);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,      (long)dFeedrate);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck,      wStartOrEnd);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukG01Cont);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,      ukInc);

    SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}

// Question : ukExactStop모드에 wEndCheck값을 넣는다???
int FAS_MoveArchMultiAbsPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdAbsPos, double  dFeedrate, WORD wEndCheck)
{
    int i, nRtn;
    int iTmpAxis = 0;
    long lAxisStatus;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	for (i=0; i<wNoOfAxis; i++)
	{
		if ((awNoAxis[iBdID] <= piAxis[i]) || (piAxis[i] < 0))
			return FMM_INVALID_AXIS_NUM;
		nRtn = CheckParameterRange(FALSE, 15, (long)pdAbsPos[i]);
		if (nRtn != FMM_OK) return nRtn;
	}

	nRtn = CheckParameterRange(TRUE, 9, (long)dFeedrate);
	if (nRtn != FMM_OK) return nRtn;

    for (i=0; i<wNoOfAxis; i++)
    {
        FAS_GetAxisStatus(iBdID, piAxis[i], &lAxisStatus);

        if (lAxisStatus & 0x20) return 2;			// ErrorSwPlusLimit
        else if (lAxisStatus & 0x40) return 3;		// ErrorSwMinusLimit
        else if (lAxisStatus & 0x80) return 4;		// ErrorOrgTimeOut
    }

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd47 In\n", iBdID+1);

    for (i=0; i<wNoOfAxis; i++)
    {
        FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition, (long)pdAbsPos[i]);

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
    }

    SetAxisInfoMulti(iBdID, iTmpAxis);

    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode, ukContInterMode);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukOptimalSingle, 0);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate, (long)dFeedrate);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck, wEndCheck);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukG01Cont);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc, ukAbs);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukExactStop, wEndCheck);

    SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}

// Question : ukExactStopmode에 wEndCheck값을 넣는다??
int FAS_MoveArchMultiIncPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdIncPos, double  dFeedrate, WORD wEndCheck)
{
    int i, nRtn;
    int iTmpAxis = 0;
    long lAxisStatus;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	for (i=0; i<wNoOfAxis; i++)
	{
		if ((awNoAxis[iBdID] <= piAxis[i]) || (piAxis[i] < 0))
			return FMM_INVALID_AXIS_NUM;
		nRtn = CheckParameterRange(FALSE, 15, (long)pdIncPos[i]);
		if (nRtn != FMM_OK) return nRtn;
	}

	nRtn = CheckParameterRange(TRUE, 9, (long)dFeedrate);
	if (nRtn != FMM_OK) return nRtn;

    for (i=0; i<wNoOfAxis; i++)
    {
        FAS_GetAxisStatus(iBdID, piAxis[i], &lAxisStatus);

        if (lAxisStatus & 0x20) return 2;			// ErrorSwPlusLimit
        else if (lAxisStatus & 0x40) return 3;		// ErrorSwMinusLimit
        else if (lAxisStatus & 0x80) return 4;		// ErrorOrgTimeOut
    }

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd48 In\n", iBdID+1);

    for (i=0; i<wNoOfAxis; i++)
    {
        FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition, (long)pdIncPos[i]);

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
    }

    SetAxisInfoMulti(iBdID, iTmpAxis);

    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode, ukOptimalMode);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukOptimalSingle, 0);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate, (long)dFeedrate);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck, wEndCheck);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukG01Optimal);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc, ukInc);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukExactStop, wEndCheck);

    SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}


int FAS_MoveCircleAbsPos(int iBdID, WORD wNoOfAxis, int *piCirAxis, double *pdCirEndPosAbs, double *pdCirCenterAbs, double dFeedrate, int iDirection, WORD wEndCheck)
{
    int i, nRtn;
    int iTmpAxis = 0;
   
	long AxisStatus ;
    int  MotionCount = 0 ;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
	
	for (i=0; i<wNoOfAxis; i++)
	{
		if ((awNoAxis[iBdID] <= piCirAxis[i]) || (piCirAxis[i] < 0)) 
			return FMM_INVALID_AXIS_NUM;
		nRtn = CheckParameterRange(FALSE, 15, (long)pdCirEndPosAbs[i]);
		if (nRtn != FMM_OK) return nRtn;
		nRtn = CheckParameterRange(FALSE, 15, (long)pdCirCenterAbs[i]);
		if (nRtn != FMM_OK) return nRtn;
	}

	nRtn = CheckParameterRange(TRUE, 9, (long)dFeedrate);
	if (nRtn != FMM_OK) return nRtn;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd49 In\n", iBdID+1);

    for (i=0; i<wNoOfAxis; i++)
    {
        FAS_WriteRam(iBdID, piCirAxis[i], ukCirEndPos, (long)pdCirEndPosAbs[i]);
        FAS_WriteRam(iBdID, piCirAxis[i], ukCirCenter, (long)pdCirCenterAbs[i]);

		if (piCirAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piCirAxis[i]);
    }

    SetAxisInfoMulti(iBdID, iTmpAxis);

    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck,      wEndCheck);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,     ukExactStopMode);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,      (long)dFeedrate);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCirDirection,  iDirection);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukG02);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,      ukAbs);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukExactStop,     wEndCheck);

    SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
    
	if (wEndCheck != 0)
    {
       for(i=0; i<wNoOfAxis; i++)
       {  
		   do
		   {   
			  FAS_GetAxisStatus(iBdID, piCirAxis[i], &AxisStatus);  	
			  if (!(AxisStatus & FFLAG_MOTIONING))
				 MotionCount ++ ;
           } while(MotionCount < 10);
		   
		   MotionCount = 0 ;
       } 
    }
    return FMM_OK;
}

int FAS_MoveCircleIncPos(int iBdID, WORD wNoOfAxis, int *piCirAxis, double *pdCirEndPosInc, double *pdCirCenterInc, double dFeedrate, int iDirection, WORD wEndCheck)
{
    int i, nRtn;
    int iTmpAxis = 0;
    
	long AxisStatus ;
    int  MotionCount = 0 ;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	for (i=0; i<wNoOfAxis; i++)
	{
		if ((awNoAxis[iBdID] <= piCirAxis[i]) || (piCirAxis[i] < 0)) 
			return FMM_INVALID_AXIS_NUM;
		nRtn = CheckParameterRange(FALSE, 15, (long)pdCirEndPosInc[i]);
		if (nRtn != FMM_OK) return nRtn;
		nRtn = CheckParameterRange(FALSE, 15, (long)pdCirCenterInc[i]);
		if (nRtn != FMM_OK) return nRtn;
	}

	nRtn = CheckParameterRange(TRUE, 9, (long)dFeedrate);
	if (nRtn != FMM_OK) return nRtn;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd50 In\n", iBdID+1);

    for (i=0; i<wNoOfAxis; i++)
    {
        FAS_WriteRam(iBdID, piCirAxis[i], ukCirEndPos, (long)pdCirEndPosInc[i]);
        FAS_WriteRam(iBdID, piCirAxis[i], ukCirCenter, (long)pdCirCenterInc[i]);

		if (piCirAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piCirAxis[i]);
    }

    SetAxisInfoMulti(iBdID, iTmpAxis);

    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck,      wEndCheck);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,     ukExactStopMode);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,      (long)dFeedrate);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCirDirection,  iDirection);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukG02);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,      ukInc);

    SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd50 Out\n", iBdID+1);
	
	if (wEndCheck != 0)
    {
       for(i=0; i<wNoOfAxis; i++)
       {  
		   do
		   {   
			  FAS_GetAxisStatus(iBdID, piCirAxis[i], &AxisStatus);  	
			  if (!(AxisStatus & FFLAG_MOTIONING))
				 MotionCount ++ ;
           } while(MotionCount < 10);
		   
		   MotionCount = 0 ;
       } 
    }
    return FMM_OK;
}

int FAS_MoveContCircleAbsPos(int iBdID, WORD wNoOfAxis, int *piCirAxis, double *pdCirEndPosAbs, double *pdCirCenterAbs, double dFeedrate, int iDirection, WORD wStartOrEnd)
{
    int i, nRtn;
    int iTmpAxis = 0;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	for (i=0; i<wNoOfAxis; i++)
	{
		if ((awNoAxis[iBdID] <= piCirAxis[i]) || (piCirAxis[i] < 0)) 
			return FMM_INVALID_AXIS_NUM;
		nRtn = CheckParameterRange(FALSE, 15, (long)pdCirEndPosAbs[i]);
		if (nRtn != FMM_OK) return nRtn;
		nRtn = CheckParameterRange(FALSE, 15, (long)pdCirCenterAbs[i]);
		if (nRtn != FMM_OK) return nRtn;
	}

	nRtn = CheckParameterRange(TRUE, 9, (long)dFeedrate);
	if (nRtn != FMM_OK) return nRtn;

    ENTER_SYNCHRONIZATION(iBdID);

    for (i=0; i<wNoOfAxis; i++)
    {
        FAS_WriteRam(iBdID, piCirAxis[i], ukCirEndPos,    (long)pdCirEndPosAbs[i]);
        FAS_WriteRam(iBdID, piCirAxis[i], ukCirCenter, (long)pdCirCenterAbs[i]);

		if (piCirAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piCirAxis[i]);
    }

    SetAxisInfoMulti(iBdID, iTmpAxis);

    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,     ukContInterMode);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,      (long)dFeedrate);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCirDirection,  iDirection);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck,      wStartOrEnd);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukG02Cont);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,      ukAbs);

    SetDpramCommand(iBdID, piCirAxis[i], ukSetGoalPos, 0);
    WaitDpramCommand(iBdID, piCirAxis[i], ukSetGoalPos, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
    
    return FMM_OK;
}

int FAS_MoveContCircleIncPos(int iBdID, WORD wNoOfAxis, int *piCirAxis, double *pdCirEndPosInc, double *pdCirCenterInc, double dFeedrate, int iDirection, WORD wStartOrEnd)
{
    int i, nRtn;
    int iTmpAxis = 0;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	for (i=0; i<wNoOfAxis; i++)
	{
		if ((awNoAxis[iBdID] <= piCirAxis[i]) || (piCirAxis[i] < 0)) 
			return FMM_INVALID_AXIS_NUM;
		nRtn = CheckParameterRange(FALSE, 15, (long)pdCirEndPosInc[i]);
		if (nRtn != FMM_OK) return nRtn;
		nRtn = CheckParameterRange(FALSE, 15, (long)pdCirCenterInc[i]);
		if (nRtn != FMM_OK) return nRtn;
	}
	nRtn = CheckParameterRange(TRUE, 9, (long)dFeedrate);
	if (nRtn != FMM_OK) return nRtn;

    ENTER_SYNCHRONIZATION(iBdID);

    for (i=0; i<wNoOfAxis; i++)
    {
        FAS_WriteRam(iBdID, piCirAxis[i], ukCirEndPos, (long)pdCirEndPosInc[i]);
        FAS_WriteRam(iBdID, piCirAxis[i], ukCirCenter, (long)pdCirCenterInc[i]);

		if (piCirAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piCirAxis[i]);
    }

    SetAxisInfoMulti(iBdID, iTmpAxis);

    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,      ukContInterMode);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,       (long)dFeedrate);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCirDirection,   iDirection);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck,       wStartOrEnd);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation,  ukG02Cont);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,       ukInc);

    SetDpramCommand(iBdID, piCirAxis[i], ukSetGoalPos, 0);
    WaitDpramCommand(iBdID, piCirAxis[i], ukSetGoalPos, 0);

	LEAVE_SYNCHRONIZATION(iBdID);
    
    return FMM_OK;
}

int FAS_MoveOrgZpulse(int iBdID, WORD wAxis)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
	
	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukMoveOrgZpulse, 0);
    WaitDpramCommand(iBdID, wAxis, ukMoveOrgZpulse, 0);
    LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}

int FAS_MoveToLimit(int iBdID, WORD wAxis, double dSpeed, int iLimitDir, WORD wEndcheck)
{
    long lDir, AxisStatus;
	int nRtn, MotionCount = 0;

	lDir = (iLimitDir == 1) ? 1 : 0;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

	nRtn = CheckParameterRange(FALSE, 8, (long)dSpeed);
	if (nRtn != FMM_OK) return nRtn;

	ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd51 In\n", iBdID+1);
    FAS_WriteRam(iBdID, wAxis, ukG00Speed, (long)dSpeed);
    FAS_WriteRam(iBdID, wAxis, ukJogDir, lDir);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck, wEndcheck);

    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukMoveToLimit, 0);
    WaitDpramCommand(iBdID, wAxis, ukMoveToLimit, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd51 Out\n", iBdID+1);
	
	if (wEndcheck != 0)
    {
		do
		{
			FAS_GetAxisStatus(iBdID, wAxis, &AxisStatus);  	
			if (!(AxisStatus & FFLAG_MOTIONING)) MotionCount ++ ;
		} while(MotionCount < 10);
    }

    return FMM_OK;
}


int FAS_EscapeFromLimit(int iBdID, WORD wAxis, double dSpeed, int iLimitDir, WORD wEndcheck)
{
    long lDir;
	int nRtn;

	lDir = (iLimitDir == 1) ? 1 : 0;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

	nRtn = CheckParameterRange(FALSE, 8, (long)dSpeed);
	if (nRtn != FMM_OK) return nRtn;

    ENTER_SYNCHRONIZATION(iBdID);
    FAS_WriteRam(iBdID, wAxis, ukG00Speed, (long)dSpeed);
    FAS_WriteRam(iBdID, wAxis, ukJogDir, lDir);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck, wEndcheck);

    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukEscapeFromLimit, 0);
    WaitDpramCommand(iBdID, wAxis, ukEscapeFromLimit, 0);
    LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}

int FAS_MoveToZPulse(int iBdID, WORD wAxis, double dSpeed, int iZPulseDir, WORD wEndcheck)
{
    long lDir = (iZPulseDir == 1) ? 1 : 0;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
    FAS_WriteRam(iBdID, wAxis, ukG00Speed, (long)dSpeed);
    FAS_WriteRam(iBdID, wAxis, ukJogDir, lDir);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck, wEndcheck);

    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukMoveToZPulse, 0);
    WaitDpramCommand(iBdID, wAxis, ukMoveToZPulse, 0);
    LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}

int FAS_MoveOnePulse(int iBdID, WORD wAxis, int iOnePulseDir)
{
    long lDir =  (iOnePulseDir == 1) ? 1 : 0;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
    FAS_WriteRam(iBdID, wAxis, ukJogDir, lDir);
    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukMoveToOnePulse, 0);
    WaitDpramCommand(iBdID, wAxis, ukMoveToOnePulse, 0);
    LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}

int FAS_MoveToOrgPoint(int iBdID, WORD wAxis, double dSpeed, WORD wEndcheck)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
    FAS_WriteRam(iBdID, wAxis, ukG00Speed, (long)dSpeed);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck, wEndcheck);

    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukMoveOrgPoint, 0);
    WaitDpramCommand(iBdID, wAxis, ukMoveOrgPoint, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
    
    return FMM_OK;
}

int FAS_MoveVelocity(int iBdID, WORD wAxis, double dSpeed, int iVelDir)
{
    long lDir = (iVelDir == 1) ? 1 : 0;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd52 In\n", iBdID+1);
    FAS_WriteRam(iBdID, wAxis, ukG00Speed, (long)dSpeed);
    FAS_WriteRam(iBdID, wAxis, ukJogDir, lDir);

    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukVelocityMove, 0);
    WaitDpramCommand(iBdID, wAxis, ukVelocityMove, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd52 Out\n", iBdID+1);

    return FMM_OK;
}

int FAS_MovePause(int iBdID, WORD wAxis)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukMotionPause, 0);
    WaitDpramCommand(iBdID, wAxis, ukMotionPause, 0);
    LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}

int FAS_MoveRestart(int iBdID, WORD wAxis)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukMotionRestart, 0);
    WaitDpramCommand(iBdID, wAxis, ukMotionRestart, 0);
    LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}

int FAS_WaitUntilIoSet(int iBdID, WORD wIoBitNo, BOOL bOnOff)
{
    int i;
    UINT iIoValue;

	iIoValue = 0x0FFFF & (0x01 << wIoBitNo);

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	i = CheckParameterRange(FALSE, RANGE_4BIT_HEX, wIoBitNo);
	if (i != FMM_OK) return i;

    ENTER_SYNCHRONIZATION(iBdID);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, iIoValue, iIoValue);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukStatus, bOnOff);
    SetDpramCommand(iBdID, AXIS_GLOBAL, ukWaitUntilIoSet, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukWaitUntilIoSet, 0);
    LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}

int FAS_Dwell(int iBdID, long lMsec)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDwellTime, lMsec);

    SetDpramCommand(iBdID, AXIS_GLOBAL, ukDewell, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukDewell, 0);
    LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}

int FAS_SoftResetMotion(int iBdID)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
    SetDpramCommand(iBdID, AXIS_GLOBAL, ukSoftReset, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSoftReset, 0);
    LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}

int FAS_WaitSingleMoveDone(int iBdID, WORD wAxis)
{
	long AxisStatus;
	int MotionCount =0;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

	ENTER_SYNCHRONIZATION(iBdID);
	SetAxisInfo(iBdID, wAxis);

	SetDpramCommand(iBdID, wAxis, ukWaitSingleMoveDone, 0);
	WaitDpramCommand(iBdID, wAxis, ukWaitSingleMoveDone, 0);

	LEAVE_SYNCHRONIZATION(iBdID);

	do
	{
		FAS_GetAxisStatus(iBdID, wAxis, &AxisStatus);  	
		if (!(AxisStatus & FFLAG_MOTIONING)) 
			MotionCount ++;
		else
			MotionCount = 0;
	}while(MotionCount < 3);

	return FMM_OK;
}

int FAS_WaitMultiMoveDone(int iBdID, int iAxis)
{
	long AxisStatus;
	int i, MotionCount =0;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	//iAxis에 대한 범위검사를 해야 하는데....

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd53 In\n", iBdID+1);
    SetAxisInfoMulti(iBdID, iAxis);

    SetDpramCommand(iBdID, iAxis, ukWaitMultiMoveDone, 0);
    WaitDpramCommand(iBdID, iAxis, ukWaitMultiMoveDone, 0);

	LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd53 Out\n", iBdID+1);
	
	for(i=0; i<MAX_AXIS_NUMS; i++)
    {
		if (iAxis & (0x01 << i))
        {
			do
			{
				FAS_GetAxisStatus(iBdID, i, &AxisStatus);  	
				if (!(AxisStatus & FFLAG_MOTIONING)) MotionCount++;
			} while(MotionCount < 10);
		   
			MotionCount = 0;
		}
	}

    return FMM_OK;
}

int FAS_VelocityOverride(int iBdID, WORD wAxis, double dVelocity)
{
	int nRtn;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

	nRtn = CheckParameterRange(FALSE, 3, (long)dVelocity);
	if (nRtn != FMM_OK) return nRtn;

    ENTER_SYNCHRONIZATION(iBdID);
    FAS_WriteRam(iBdID, wAxis, ukG00Speed, (long)dVelocity);

    SetAxisInfo(iBdID, wAxis);
    SetDpramCommand(iBdID, wAxis,  ukVelocityOverride, 0);
    WaitDpramCommand(iBdID, wAxis, ukVelocityOverride, 0);

    LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}

int FAS_PositionIncOverride(int iBdID, WORD wAxis, long lOverridePos)
{
	int nRtn;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

	nRtn = CheckParameterRange(FALSE, 15, lOverridePos);
	if (nRtn != FMM_OK) return nRtn;

    ENTER_SYNCHRONIZATION(iBdID);
    FAS_WriteRam(iBdID, wAxis, ukGoalPosition,  lOverridePos);
    FAS_WriteRam(iBdID, wAxis, ukAbsOrInc, ukInc);
    
    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukPositionOverride, 0);
    WaitDpramCommand(iBdID, wAxis, ukPositionOverride, 0);
    LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}

int FAS_PositionAbsOverride(int iBdID, WORD wAxis, long lOverridePos)
{
	int nRtn;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;
	
	nRtn = CheckParameterRange(FALSE, 15, lOverridePos);
	if (nRtn != FMM_OK) return nRtn;

    ENTER_SYNCHRONIZATION(iBdID);
    FAS_WriteRam(iBdID, wAxis, ukGoalPosition, lOverridePos);
    FAS_WriteRam(iBdID, wAxis, ukAbsOrInc, ukAbs);

    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukPositionOverride, 0);
    WaitDpramCommand(iBdID, wAxis, ukPositionOverride, 0);
    LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}

int FAS_MoveAbsSynchronous(int iBdID, int *piAxis, long lPosition, long lVelocity, WORD wEndCheck)
{
    int i;
    int iTmpAxis = 0;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	//piAxis에 대한 범위검사를 해야 하지만....

    ENTER_SYNCHRONIZATION(iBdID);
    for(i=0; i<2; i++)
    {
        FAS_WriteRam(iBdID, piAxis[i], ukG00Speed, lVelocity);
        FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition, lPosition);

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
    }

    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck, wEndCheck);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc, ukAbs);

    SetDpramCommand(iBdID, AXIS_GLOBAL, ukMoveSync, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukMoveSync, 0);
    LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}

int FAS_MoveIncSynchronous(int iBdID, int *piAxis, long lPosition, long lVelocity, WORD wEndCheck)
{
    int i;
    int iTmpAxis = 0;
	
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
    for(i=0; i<2; i++)
    {
        FAS_WriteRam(iBdID, piAxis[i], ukG00Speed, lVelocity);
        FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition, lPosition);

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
    }

    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck, wEndCheck);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc, ukInc);

    SetAxisInfoMulti(iBdID, iTmpAxis);

    SetDpramCommand(iBdID, AXIS_GLOBAL, ukMoveSync, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukMoveSync, 0);
    LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}

//------------------------------------------------------------------
//                About Queue Functions.
//------------------------------------------------------------------
int FAS_ResetArcQueue(int iBdID)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd54 In\n", iBdID+1);
	SetDpramCommand(iBdID, AXIS_GLOBAL, ukResetArcQ, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukResetArcQ, 0);
	LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd54 Out\n", iBdID+1);

    return FMM_OK;
}


int FAS_ResetArcMotionQueue(int iBdID)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd55 In\n", iBdID+1);
    
	SetDpramCommand(iBdID, AXIS_GLOBAL, ukResetMotionArcQ, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukResetMotionArcQ, 0);
    
	LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd55 Out\n", iBdID+1);

    return FMM_OK;
}


int FAS_ResetLinearQueue(int iBdID)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
    
	SetDpramCommand(iBdID, AXIS_GLOBAL, ukResetG01Q, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukResetG01Q, 0);
    
	LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}

int FAS_ResetContLinearQueue(int iBdID)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
    
	SetDpramCommand(iBdID, AXIS_GLOBAL, ukResetContG01Q, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukResetContG01Q, 0);
    
	LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}

int FAS_ResetCircleQueue(int iBdID)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
    
	SetDpramCommand(iBdID, AXIS_GLOBAL, ukResetG02Q, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukResetG02Q, 0);
    
	LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}


int FAS_ResetAxisQueue(int iBdID, WORD wAxises)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd56 In\n", iBdID+1);
    
    SetAxisInfo(iBdID, wAxises);
	SetDpramCommand(iBdID, AXIS_GLOBAL, ukResetG00Q, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukResetG00Q, 0);
    
	LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd56 Out\n", iBdID+1);

    return FMM_OK;
}

int FAS_ResetDropQueue(int iBdID)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
    
	SetDpramCommand(iBdID, AXIS_GLOBAL, ukResetDropQ, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukResetDropQ, 0);
    
	LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}


int FAS_GetFreeDropQueue(int iBdID, WORD *wFreeDropQueue)
{
	long lTmpQ;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
    SetDpramCommand(iBdID, AXIS_GLOBAL, ukGetNoFreeQDrop, ukFreePosCmdQ);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukGetNoFreeQDrop, 0);
   
	lTmpQ = FAS_ReadRam(iBdID, AXIS_GLOBAL, ukFreePosCmdQ);
    LEAVE_SYNCHRONIZATION(iBdID);

	*wFreeDropQueue = (WORD)(lTmpQ & 0x0FFFF);
    return FMM_OK;
}



////////////////////////////////////////////////////////////////////// 9/17

int FAS_GetFreeArcQueue(int iBdID, WORD *wFreeArcQueue)
{
	long lTmpQ;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd57 In\n", iBdID+1);
    SetDpramCommand(iBdID, AXIS_GLOBAL, ukGetNoFreeQArch, ukFreePosCmdQ);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukGetNoFreeQArch, 0);
   
	lTmpQ = FAS_ReadRam(iBdID, AXIS_GLOBAL, ukFreePosCmdQ);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd57 Out\n", iBdID+1);

	*wFreeArcQueue = (WORD)(lTmpQ & 0x0FFFF);

    return FMM_OK;
}



int FAS_GetFreeArcMotionQueue(int iBdID, WORD *wFreeArcMotionQueue)
{
	long lTmpQ;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd58 In\n", iBdID+1);
    SetDpramCommand(iBdID, AXIS_GLOBAL, ukGetNoFreeMotionArcQ, ukFreePosCmdQ);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukGetNoFreeMotionArcQ, 0);
   
	lTmpQ = FAS_ReadRam(iBdID, AXIS_GLOBAL, ukFreePosCmdQ);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd58 Out\n", iBdID+1);

	*wFreeArcMotionQueue = (WORD)(lTmpQ & 0x0FFFF);

    return FMM_OK;
}

int FAS_GetFreeLinearQueue(int iBdID, WORD *wFreeLinearQueue)
{
	long lTmpQ;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
    SetDpramCommand(iBdID, AXIS_GLOBAL, ukGetNoFreeQG01, ukFreePosCmdQ);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukGetNoFreeQG01, 0);
   
	lTmpQ = FAS_ReadRam(iBdID, AXIS_GLOBAL, ukFreePosCmdQ);
    LEAVE_SYNCHRONIZATION(iBdID);

	*wFreeLinearQueue = (WORD)(lTmpQ & 0x0FFFF);

    return FMM_OK;
}

int FAS_GetFreeContLinearQueue(int iBdID, WORD *wFreeContLinearQueue)
{
	long lTmpQ;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
    SetDpramCommand(iBdID, AXIS_GLOBAL, ukGetNoFreeQContG01, ukFreePosCmdQ);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukGetNoFreeQContG01, 0);
   
	lTmpQ = FAS_ReadRam(iBdID, AXIS_GLOBAL, ukFreePosCmdQ);
    LEAVE_SYNCHRONIZATION(iBdID);

	*wFreeContLinearQueue = (WORD)(lTmpQ & 0x0FFFF);

    return FMM_OK;
}

int FAS_GetFreeCircleQueue(int iBdID, WORD *wFreeCircleQueue)
{
	long lTmpQ;
	
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
    SetDpramCommand(iBdID, AXIS_GLOBAL, ukGetNoFreeQG02, ukFreePosCmdQ);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukGetNoFreeQG02, 0);
   
	lTmpQ = FAS_ReadRam(iBdID, AXIS_GLOBAL, ukFreePosCmdQ);
    LEAVE_SYNCHRONIZATION(iBdID);

	*wFreeCircleQueue = (WORD)(lTmpQ & 0x0FFFF);

    return FMM_OK;
}


int FAS_GetFreeAxisQueue(int iBdID, WORD wAxis, WORD *wFreeAxisQueue)
{
	long lTmpQ;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	SetAxisInfo(iBdID, wAxis);

	SetDpramCommand(iBdID, AXIS_GLOBAL, ukGetNoFreeQG00, ukFreePosCmdQ);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukGetNoFreeQG00, 0);
   
	lTmpQ = FAS_ReadRam(iBdID, AXIS_GLOBAL, ukFreePosCmdQ);
    LEAVE_SYNCHRONIZATION(iBdID);

	*wFreeAxisQueue = (WORD)(lTmpQ & 0x0FFFF);

    return FMM_OK;
}

//------------------------------------------------------------------
//                About AD Functions.
//------------------------------------------------------------------
int FAS_GetAdResult(int iBdID, WORD wChannel, long *lGetAdResult)
{
	long wTmpAd ;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	SetAxisInfo(iBdID, wChannel);

    SetDpramCommand(iBdID, wChannel, ukGetAdResult, ukDaOutput);
    WaitDpramCommand(iBdID, wChannel, ukGetAdResult, 0);

	wTmpAd = FAS_ReadRam(iBdID, wChannel, ukDaOutput);
	LEAVE_SYNCHRONIZATION(iBdID);
   
	*lGetAdResult = wTmpAd;

    return FMM_OK;
}


//////////////////////////////////////////////////////////////////////
// New Library
//////////////////////////////////////////////////////////////////////
int FAS_MoveArcAbsPos(int iBdID, WORD NoOfAxis, WORD *Axis, double *AbsPos, 
					  double D_Feedrate, double D_CirFeedrate, double CornerDistance, WORD D_StartEndPoint)
{
	int i;
	int TempAxis = 0;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);
	for(i=0; i<NoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, Axis[i], ukGoalPosition, (long)AbsPos[i]);

		if (Axis[i] < MAX_AXIS_NUMS) TempAxis |= (0x01 << Axis[i]);
	}
	SetAxisInfoMulti(iBdID, TempAxis);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,     ukLclMode);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,      (long)D_Feedrate);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCirFeedrate,   (long)D_CirFeedrate);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCornerError,   (long)CornerDistance);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukStartEndPoint, (long)D_StartEndPoint);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukArch);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,      ukAbs);

	SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0); 

	LEAVE_SYNCHRONIZATION(iBdID);	

	return FMM_OK;
}

// 1. Disp Off + Z Up 
int FAS_MoveArcAbsPosWithSealOff(int iBdID, WORD NoOfAxis, WORD *Axis, double *AbsPos, 
			double D_Feedrate, double D_CirFeedrate, double CornerDistance, WORD D_StartEndPoint, 
			double dUpDistancePos, WORD wOffNoOfAxis, int *piOffAxis, double *pdOffIncPos, double *pdOffVelocity,
			double dMusashiOffDistancePos, DWORD dwOffValue, BOOL bIsLow)
{		
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd60 In\n", iBdID+1);

	int i;
	int TempAxis = 0;
	int iTmpAxis = 0;
	
	for(i=0; i<NoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, Axis[i], ukGoalPosition, (long)AbsPos[i]);

		if (Axis[i] < MAX_AXIS_NUMS) TempAxis |= (0x01 << Axis[i]);
	}
	SetAxisInfoMulti(iBdID, TempAxis);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,     ukLclMode);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,      (long)D_Feedrate);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCirFeedrate,   (long)D_CirFeedrate);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCornerError,   (long)CornerDistance);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukStartEndPoint, (long)D_StartEndPoint);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukArch);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,      ukAbs);
	//----------------------------------------------------------------------------
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos,   (long)dUpDistancePos);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos2,  (long)dUpDistancePos);	// 2nd Z Up Start Dist를 1st Z Up Start Dist와 동일하게 설정
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukPressureControlOn,  (long)FALSE/*bPressureControlOn*/);	// Corner 감압 유무 Flag를 강제로 FALSE
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukPressureControlType,  0);	// Default Corner 감압 Type

	for (i=0; i<wOffNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, piOffAxis[i], ukG00Speed,       (long)pdOffVelocity[i]);
		FAS_WriteRam(iBdID, piOffAxis[i], ukGoalPosition,   (long)0/*pdOffIncPos[i]*/);

		FAS_WriteRam(iBdID, piOffAxis[i], ukGoalPosition2,  (long)pdOffIncPos[i]);

		FAS_WriteRam(iBdID, piOffAxis[i], ukLineTargetGapPulse,  (long)0);	// Line Gap Offset		

		if (piOffAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piOffAxis[i]);
	}
	SetAxisInfoDrop(iBdID, iTmpAxis);	
	// --------------------------------------------------------------------------
	
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoValue,       dwOffValue); 
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTemp,		  bIsLow);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukMusashiOffPos, (long)dMusashiOffDistancePos); 	

	if(2==D_StartEndPoint)
	{
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTopConOff, 1);		
	}
	else
	{
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTopConOff, 0);
	}

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukGapControlOn, 0);

	// --------------------------------------------------------------------------

	SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0); 

	LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd60 Out\n", iBdID+1);

	return FMM_OK;
}

// 2. Disp Off + 1st Z Up + 2nd Z Up 
int FAS_MoveArcAbsPosWithSealOff(int iBdID, WORD NoOfAxis, WORD *Axis, double *AbsPos, 
			double D_Feedrate, double D_CirFeedrate, double CornerDistance, WORD D_StartEndPoint, 
			double dUpDistancePos, WORD wOffNoOfAxis, int *piOffAxis, double *pdOffIncPos, double *pdOffVelocity,
			double dMusashiOffDistancePos, DWORD dwOffValue, BOOL bIsLow,  double dUpDistancePos2, double *pdOffIncPos2)
{		
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd61 In\n", iBdID+1);

	int i;
	int TempAxis = 0;
	int iTmpAxis = 0;	
	
	for(i=0; i<NoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, Axis[i], ukGoalPosition, (long)AbsPos[i]);

		if (Axis[i] < MAX_AXIS_NUMS) TempAxis |= (0x01 << Axis[i]);
	}
	SetAxisInfoMulti(iBdID, TempAxis);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,     ukLclMode);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,      (long)D_Feedrate);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCirFeedrate,   (long)D_CirFeedrate);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCornerError,   (long)CornerDistance);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukStartEndPoint, (long)D_StartEndPoint);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukArch);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,      ukAbs);
	//----------------------------------------------------------------------------
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos,   (long)dUpDistancePos);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos2,  (long)dUpDistancePos2);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukPressureControlOn,  (long)FALSE/*bPressureControlOn*/);	// Corner 감압 유무 Flag를 강제로 FALSE
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukPressureControlType,  0);	// Default Corner 감압 Type
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCornerGapOnFlag,  1);		// Corner Gap On Flag, 사용하지 않을 경우 1을 넣으면 됨
																						// 0을 넣으면 Corner Gap Off 됨(Global 39에 설정된 속도 이하에서)

	for (i=0; i<wOffNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, piOffAxis[i], ukG00Speed,       (long)pdOffVelocity[i]);
		FAS_WriteRam(iBdID, piOffAxis[i], ukGoalPosition,   (long)pdOffIncPos[i]);

		FAS_WriteRam(iBdID, piOffAxis[i], ukGoalPosition2,  (long)pdOffIncPos2[i]);

		FAS_WriteRam(iBdID, piOffAxis[i], ukLineTargetGapPulse,  (long)0);	// Line Gap Offset

		if (piOffAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piOffAxis[i]);
	}
	SetAxisInfoDrop(iBdID, iTmpAxis);	
	// --------------------------------------------------------------------------
	
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoValue,       dwOffValue); 
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTemp,		  bIsLow);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukMusashiOffPos, (long)dMusashiOffDistancePos); 	

	if(2==D_StartEndPoint)
	{
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTopConOff, 1);		
	}
	else
	{
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTopConOff, 0);
	}

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukGapControlOn, 0);
	// --------------------------------------------------------------------------

	SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0); 

	LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd61 Out\n", iBdID+1);

	return FMM_OK;
}

// 3. Disp Off + 1st Z Up + 2nd Z Up + Corner Pressure Control Flag
int FAS_MoveArcAbsPosWithSealOff(int iBdID, WORD NoOfAxis, WORD *Axis, double *AbsPos, 
			double D_Feedrate, double D_CirFeedrate, double CornerDistance, WORD D_StartEndPoint, 
			double dUpDistancePos, WORD wOffNoOfAxis, int *piOffAxis, double *pdOffIncPos, double *pdOffVelocity,
			double dMusashiOffDistancePos, DWORD dwOffValue, BOOL bIsLow,  double dUpDistancePos2, double *pdOffIncPos2, BOOL bPressureControlOn)
{	
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd62 In\n", iBdID+1);

	int i;
	int TempAxis = 0;
	int iTmpAxis = 0;	
	
	for(i=0; i<NoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, Axis[i], ukGoalPosition, (long)AbsPos[i]);

		if (Axis[i] < MAX_AXIS_NUMS) TempAxis |= (0x01 << Axis[i]);
	}
	SetAxisInfoMulti(iBdID, TempAxis);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,     ukLclMode);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,      (long)D_Feedrate);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCirFeedrate,   (long)D_CirFeedrate);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCornerError,   (long)CornerDistance);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukStartEndPoint, (long)D_StartEndPoint);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukArch);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,      ukAbs);
	//----------------------------------------------------------------------------
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos,   (long)dUpDistancePos);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos2,  (long)dUpDistancePos2);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukPressureControlOn,  (long)bPressureControlOn);	// Corner 감압 유무 Flag
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukPressureControlType,  0);	// Default Corner 감압 Type
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCornerGapOnFlag,  1);		// Corner Gap On Flag, 사용하지 않을 경우 1을 넣으면 됨
																						// 0을 넣으면 Corner Gap Off 됨(Global 39에 설정된 속도 이하에서)

	for (i=0; i<wOffNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, piOffAxis[i], ukG00Speed,       (long)pdOffVelocity[i]);
		FAS_WriteRam(iBdID, piOffAxis[i], ukGoalPosition,   (long)pdOffIncPos[i]);

		FAS_WriteRam(iBdID, piOffAxis[i], ukGoalPosition2,  (long)pdOffIncPos2[i]);

		FAS_WriteRam(iBdID, piOffAxis[i], ukLineTargetGapPulse,  (long)0);	// Line Gap Offset

		if (piOffAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piOffAxis[i]);
	}
	SetAxisInfoDrop(iBdID, iTmpAxis);	
	// --------------------------------------------------------------------------
	
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoValue,       dwOffValue); 
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTemp,		  bIsLow);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukMusashiOffPos, (long)dMusashiOffDistancePos); 

	if(2==D_StartEndPoint)
	{
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTopConOff, 1);		
	}
	else
	{
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTopConOff, 0);		
	}

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukGapControlOn, 0);
	// --------------------------------------------------------------------------

	SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0); 

	LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd62 Out\n", iBdID+1);

	return FMM_OK;
}

// 4. Disp Off + 1st Z Up + 2nd Z Up + Corner Pressure Control Flag + Corner Pressure Type
int FAS_MoveArcAbsPosWithSealOff(int iBdID, WORD NoOfAxis, WORD *Axis, double *AbsPos, 
			double D_Feedrate, double D_CirFeedrate, double CornerDistance, WORD D_StartEndPoint, 
			double dUpDistancePos, WORD wOffNoOfAxis, int *piOffAxis, double *pdOffIncPos, double *pdOffVelocity,
			double dMusashiOffDistancePos, DWORD dwOffValue, BOOL bIsLow,  double dUpDistancePos2, double *pdOffIncPos2, BOOL bPressureControlOn, int nPressureControlType)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd63 In\n", iBdID+1);

	int i;
	int TempAxis = 0;
	int iTmpAxis = 0;	
	
	for(i=0; i<NoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, Axis[i], ukGoalPosition, (long)AbsPos[i]);

		if (Axis[i] < MAX_AXIS_NUMS) TempAxis |= (0x01 << Axis[i]);
	}
	SetAxisInfoMulti(iBdID, TempAxis);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,     ukLclMode);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,      (long)D_Feedrate);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCirFeedrate,   (long)D_CirFeedrate);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCornerError,   (long)CornerDistance);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukStartEndPoint, (long)D_StartEndPoint);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukArch);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,      ukAbs);
	//----------------------------------------------------------------------------
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos,   (long)dUpDistancePos);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos2,  (long)dUpDistancePos2);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukPressureControlOn,  (long)bPressureControlOn);		// Corner 감압 유무 Flag
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukPressureControlType,  (long)nPressureControlType);	// Corner 감압 Type	
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCornerGapOnFlag,  1);		// Corner Gap On Flag, 사용하지 않을 경우 1을 넣으면 됨
																						// 0을 넣으면 Corner Gap Off 됨(Global 39에 설정된 속도 이하에서)
	for (i=0; i<wOffNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, piOffAxis[i], ukG00Speed,       (long)pdOffVelocity[i]);
		FAS_WriteRam(iBdID, piOffAxis[i], ukGoalPosition,   (long)pdOffIncPos[i]);

		FAS_WriteRam(iBdID, piOffAxis[i], ukGoalPosition2,  (long)pdOffIncPos2[i]);

		FAS_WriteRam(iBdID, piOffAxis[i], ukLineTargetGapPulse,  (long)0);	// Line Gap Offset		

		if (piOffAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piOffAxis[i]);
	}
	SetAxisInfoDrop(iBdID, iTmpAxis);	
	// --------------------------------------------------------------------------
	
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoValue,       dwOffValue); 
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTemp,		  bIsLow);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukMusashiOffPos, (long)dMusashiOffDistancePos); 	

	if(2==D_StartEndPoint)
	{
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTopConOff, 1);		
	}
	else
	{
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTopConOff, 0);		
	}

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukGapControlOn, 0);
	// --------------------------------------------------------------------------

	SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0); 

	LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd63 Out\n", iBdID+1);

	return FMM_OK;
}

//ssh 100710
// 5. Disp Off + 1st Z Up + 2nd Z Up + Corner Pressure Control Flag + Corner Pressure Type
// + Line Gap Offset(Z축 정보는 wOffNoOfAxis, piOffAxis변수 사용) + Corner Gap Control Off Speed
int FAS_MoveArcAbsPosWithSealOff(int iBdID, WORD NoOfAxis, WORD *Axis, double *AbsPos, 
			double D_Feedrate, double D_CirFeedrate, double CornerDistance, WORD D_StartEndPoint, 
			double dUpDistancePos, WORD wOffNoOfAxis, int *piOffAxis, double *pdOffIncPos, double *pdOffVelocity,
			double dMusashiOffDistancePos, DWORD dwOffValue, BOOL bIsLow,  double dUpDistancePos2, double *pdOffIncPos2, BOOL bPressureControlOn, int nPressureControlType,
			int *piLineGapOffset, double dCornerGapOnFlag)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd64 In\n", iBdID+1);

	int i;
	int TempAxis = 0;
	int iTmpAxis = 0;	
	
	for(i=0; i<NoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, Axis[i], ukGoalPosition, (long)AbsPos[i]);

		if (Axis[i] < MAX_AXIS_NUMS) TempAxis |= (0x01 << Axis[i]);
	}
	SetAxisInfoMulti(iBdID, TempAxis);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,     ukLclMode);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,      (long)D_Feedrate);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCirFeedrate,   (long)D_CirFeedrate);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCornerError,   (long)CornerDistance);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukStartEndPoint, (long)D_StartEndPoint);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukArch);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,      ukAbs);
	//----------------------------------------------------------------------------
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos,   (long)dUpDistancePos);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos2,  (long)dUpDistancePos2);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukPressureControlOn,  (long)bPressureControlOn);		// Corner 감압 유무 Flag
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukPressureControlType,  (long)nPressureControlType);	// Corner 감압 Type	
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCornerGapOnFlag,  (long)dCornerGapOnFlag);		// Corner Gap On Flag, 사용하지 않을 경우 1을 넣으면 됨
																						// 0을 넣으면 Corner Gap Off 됨(Global 39에 설정된 속도 이하에서)
									
	for (i=0; i<wOffNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, piOffAxis[i], ukG00Speed,       (long)pdOffVelocity[i]);
		FAS_WriteRam(iBdID, piOffAxis[i], ukGoalPosition,   (long)pdOffIncPos[i]);

		FAS_WriteRam(iBdID, piOffAxis[i], ukGoalPosition2,  (long)pdOffIncPos2[i]);

		FAS_WriteRam(iBdID, piOffAxis[i], ukLineTargetGapPulse,  (long)piLineGapOffset[i]);	// Line Gap Offset

		if (piOffAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piOffAxis[i]);
	}
	SetAxisInfoDrop(iBdID, iTmpAxis);	
	// --------------------------------------------------------------------------
	
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoValue,       dwOffValue); 
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTemp,		  bIsLow);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukMusashiOffPos, (long)dMusashiOffDistancePos); 	

	if(2==D_StartEndPoint)
	{
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTopConOff, 1);		
	}
	else
	{
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTopConOff, 0);		
	}

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukGapControlOn, 0);
	// --------------------------------------------------------------------------

	SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0); 

	LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd64 Out\n", iBdID+1);

	return FMM_OK;
}

// 6. Disp Off + 1st Z Up + 2nd Z Up + Corner Pressure Control Flag + Corner Pressure Type + Multi Disp Off
int FAS_MoveArcAbsPosWithSealOff(int iBdID, WORD NoOfAxis, WORD *Axis, double *AbsPos, 
			double D_Feedrate, double D_CirFeedrate, double CornerDistance, WORD D_StartEndPoint, 
			double dUpDistancePos, WORD wOffNoOfAxis, int *piOffAxis, double *pdOffIncPos, double *pdOffVelocity,
			double *dMusashiOffDistancePos, DWORD dwOffValue, BOOL bIsLow,  double dUpDistancePos2, double *pdOffIncPos2, BOOL bPressureControlOn)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd63 In\n", iBdID+1);

	int i;
	int TempAxis = 0;
	int iTmpAxis = 0;	
	
	for(i=0; i<NoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, Axis[i], ukGoalPosition, (long)AbsPos[i]);

		if (Axis[i] < MAX_AXIS_NUMS) TempAxis |= (0x01 << Axis[i]);
	}
	SetAxisInfoMulti(iBdID, TempAxis);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,     ukLclMode);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,      (long)D_Feedrate);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCirFeedrate,   (long)D_CirFeedrate);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCornerError,   (long)CornerDistance);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukStartEndPoint, (long)D_StartEndPoint);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukArch);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,      ukAbs);
	//----------------------------------------------------------------------------
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos,   (long)dUpDistancePos);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos2,  (long)dUpDistancePos2);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukPressureControlOn,  (long)bPressureControlOn);		// Corner 감압 유무 Flag
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukPressureControlType,  0);	// Default Corner 감압 Type
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCornerGapOnFlag,  1);		// Corner Gap On Flag, 사용하지 않을 경우 1을 넣으면 됨
																						// 0을 넣으면 Corner Gap Off 됨(Global 39에 설정된 속도 이하에서)
	for (i=0; i<wOffNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, piOffAxis[i], ukG00Speed,       (long)pdOffVelocity[i]);
		FAS_WriteRam(iBdID, piOffAxis[i], ukGoalPosition,   (long)pdOffIncPos[i]);

		FAS_WriteRam(iBdID, piOffAxis[i], ukGoalPosition2,  (long)pdOffIncPos2[i]);

		FAS_WriteRam(iBdID, piOffAxis[i], ukLineTargetGapPulse,  (long)0);	// Line Gap Offset

		FAS_WriteRam(iBdID, piOffAxis[i], ukMusashiOffMultiPos, (long)dMusashiOffDistancePos[i]); 

		if (piOffAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piOffAxis[i]);
	}
	SetAxisInfoDrop(iBdID, iTmpAxis);	
	// --------------------------------------------------------------------------
	
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoValue,       dwOffValue); 
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTemp,		  bIsLow);	

	// Multi Off 사용 시 강제로 0 할당, Disp IO 정보는 Local Para No.75에서
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukMusashiOffPos, 0); 
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTopConOff, 0);

	if(2==D_StartEndPoint)
	{
		//FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTopConOff, 1);
		for(i=0; i<wOffNoOfAxis; i++)
			FAS_WriteRam(iBdID, piOffAxis[i], ukTopConMultiOff, 1);
	}
	else
	{
		//FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTopConOff, 0);
		for(i=0; i<wOffNoOfAxis; i++)
			FAS_WriteRam(iBdID, piOffAxis[i], ukTopConMultiOff, 0);
	}

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukGapControlOn, 0);
	// --------------------------------------------------------------------------

	SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0); 

	LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd63 Out\n", iBdID+1);

	return FMM_OK;
}

// 7. Disp Off + 1st Z Up + 2nd Z Up + Corner Pressure Control Flag + EZI_R Z Up(HighLow, IoValue)
int FAS_MoveArcAbsPosWithSealOff(int iBdID, WORD NoOfAxis, WORD *Axis, double *AbsPos, 
			double D_Feedrate, double D_CirFeedrate, double CornerDistance, WORD D_StartEndPoint, 
			double dUpDistancePos, WORD wOffNoOfAxis, int *piOffAxis, double *pdOffIncPos, double *pdOffVelocity,
			double dMusashiOffDistancePos, DWORD dwOffValue, BOOL bIsLow,  double dUpDistancePos2, double *pdOffIncPos2, BOOL bPressureControlOn, BOOL bGapIoLow, DWORD dwGapIoValue)
{	
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd65 In\n", iBdID+1);

	int i;
	int TempAxis = 0;
	int iTmpAxis = 0;	
	
	for(i=0; i<NoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, Axis[i], ukGoalPosition, (long)AbsPos[i]);

		if (Axis[i] < MAX_AXIS_NUMS) TempAxis |= (0x01 << Axis[i]);
	}
	SetAxisInfoMulti(iBdID, TempAxis);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,     ukLclMode);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,      (long)D_Feedrate);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCirFeedrate,   (long)D_CirFeedrate);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCornerError,   (long)CornerDistance);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukStartEndPoint, (long)D_StartEndPoint);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukArch);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,      ukAbs);
	//----------------------------------------------------------------------------
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos,   (long)dUpDistancePos);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos2,  (long)dUpDistancePos2);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukPressureControlOn,  (long)bPressureControlOn);	// Corner 감압 유무 Flag
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukPressureControlType,  0);	// Default Corner 감압 Type
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCornerGapOnFlag,  1);		// Corner Gap On Flag, 사용하지 않을 경우 1을 넣으면 됨
																						// 0을 넣으면 Corner Gap Off 됨(Global 39에 설정된 속도 이하에서)

	//for (i=0; i<wOffNoOfAxis; i++)
	//{
	//	FAS_WriteRam(iBdID, piOffAxis[i], ukG00Speed,       (long)pdOffVelocity[i]);
	//	FAS_WriteRam(iBdID, piOffAxis[i], ukGoalPosition,   (long)pdOffIncPos[i]);
	//
	//	FAS_WriteRam(iBdID, piOffAxis[i], ukGoalPosition2,  (long)pdOffIncPos2[i]);
	//
	//	FAS_WriteRam(iBdID, piOffAxis[i], ukLineTargetGapPulse,  (long)0);	// Line Gap Offset	
	//
	//	if (piOffAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piOffAxis[i]);
	//}
	SetAxisInfoDrop(iBdID, iTmpAxis);	
	// --------------------------------------------------------------------------
	
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoValue,       dwOffValue); 
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTemp,		  bIsLow);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukMusashiOffPos, (long)dMusashiOffDistancePos); 	

	if(2==D_StartEndPoint)
	{
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTopConOff, 1);
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukGapControlHighLow, bGapIoLow);
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukGapControlData, dwGapIoValue);		
	}
	else
	{
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTopConOff, 0);
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukGapControlHighLow, bGapIoLow);
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukGapControlData, dwGapIoValue);		
	}

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukGapControlOn, 0);
	// --------------------------------------------------------------------------

	SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0); 

	LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd65 Out\n", iBdID+1);

	return FMM_OK;
}

// 8. Disp Off + 1st Z Up + 2nd Z Up + Corner Pressure Control Flag + EZI_R Z Up(HighLow, IoValue) + Multi Disp Off
int FAS_MoveArcAbsPosWithSealOff(int iBdID, WORD NoOfAxis, WORD *Axis, double *AbsPos, 
			double D_Feedrate, double D_CirFeedrate, double CornerDistance, WORD D_StartEndPoint, 
			double dUpDistancePos, WORD wOffNoOfAxis, int *piOffAxis, double *pdOffIncPos, double *pdOffVelocity,
			double *dMusashiOffDistancePos, DWORD dwOffValue, BOOL bIsLow,  double dUpDistancePos2, double *pdOffIncPos2, BOOL bPressureControlOn, BOOL bGapIoLow, DWORD dwGapIoValue)
{	
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);
//	(g_bSemaphore[iBdID]==FALSE) ? g_bSemaphore[iBdID] = TRUE : TRACE("BdID%d Semaphore NG!\n", iBdID);
	//TRACE("Bd%d Cmd65 In\n", iBdID);

	int i;
	int TempAxis = 0;
	int iTmpAxis = 0;	
	
	for(i=0; i<NoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, Axis[i], ukGoalPosition, (long)AbsPos[i]);

		if (Axis[i] < MAX_AXIS_NUMS) TempAxis |= (0x01 << Axis[i]);
	}
	SetAxisInfoMulti(iBdID, TempAxis);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,     ukLclMode);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,      (long)D_Feedrate);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCirFeedrate,   (long)D_CirFeedrate);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCornerError,   (long)CornerDistance);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukStartEndPoint, (long)D_StartEndPoint);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukArch);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,      ukAbs);
	//----------------------------------------------------------------------------
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos,   (long)dUpDistancePos);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos2,  (long)dUpDistancePos2);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukPressureControlOn,  (long)bPressureControlOn);	// Corner 감압 유무 Flag
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukPressureControlType,  0);	// Default Corner 감압 Type
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCornerGapOnFlag,  1);		// Corner Gap On Flag, 사용하지 않을 경우 1을 넣으면 됨
																						// 0을 넣으면 Corner Gap Off 됨(Global 39에 설정된 속도 이하에서)

	//for (i=0; i<wOffNoOfAxis; i++)
	//{
	//	FAS_WriteRam(iBdID, piOffAxis[i], ukG00Speed,       (long)pdOffVelocity[i]);
	//	FAS_WriteRam(iBdID, piOffAxis[i], ukGoalPosition,   (long)pdOffIncPos[i]);
	//
	//	FAS_WriteRam(iBdID, piOffAxis[i], ukGoalPosition2,  (long)pdOffIncPos2[i]);
	//
	//	FAS_WriteRam(iBdID, piOffAxis[i], ukLineTargetGapPulse,  (long)0);	// Line Gap Offset
	//
	//	if (piOffAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piOffAxis[i]);
	//}
	SetAxisInfoDrop(iBdID, iTmpAxis);	
	// --------------------------------------------------------------------------
	
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoValue,       dwOffValue); 
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTemp,		  bIsLow);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukMusashiOffPos, 0); 

	if(2==D_StartEndPoint)
	{		
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTopConOff, 1);

		for(i=0; i<wOffNoOfAxis; i++)
		{
			FAS_WriteRam(iBdID, piOffAxis[i], ukTopConMultiOff, 1);
			FAS_WriteRam(iBdID, piOffAxis[i], ukMusashiOffMultiPos, (long)dMusashiOffDistancePos[i]); 
		}

		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukGapControlHighLow, bGapIoLow);
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukGapControlData, dwGapIoValue);
	}
	else
	{
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTopConOff, 0);

		for(i=0; i<wOffNoOfAxis; i++)
		{
			FAS_WriteRam(iBdID, piOffAxis[i], ukTopConMultiOff, 0);
			FAS_WriteRam(iBdID, piOffAxis[i], ukMusashiOffMultiPos, (long)dMusashiOffDistancePos[i]); 
		}

		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukGapControlHighLow, bGapIoLow);
		FAS_WriteRam(iBdID, AXIS_GLOBAL, ukGapControlData, dwGapIoValue);
	}

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukGapControlOn, 0);
	// --------------------------------------------------------------------------

	SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0); 

//	g_bSemaphore[iBdID] = FALSE;
	LEAVE_SYNCHRONIZATION(iBdID);	
	//TRACE("Bd%d Cmd65 Out\n", iBdID);

	return FMM_OK;
}

/* ************************************************************************* */
int FAS_MoveArcSingleAbsPosWithSealOff(int iBdID, WORD wAxis, double AbsPos, double Velocity,
			double dUpDistancePos, WORD wOffNoOfAxis, int *piOffAxis, double *pdOffIncPos, double *pdOffVelocity,
			double dMusashiOffDistancePos, DWORD dwOffValue, BOOL bIsLow)
{
	int i;
	int iTmpAxis = 0;
	long AxisStatus;
	int  MotionCount = 0 ;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);

	FAS_WriteRam(iBdID, wAxis, ukG00Speed, (long)Velocity); 
	FAS_WriteRam(iBdID, wAxis, ukGoalPosition,  (long)AbsPos); 

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukArchSingle);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc, ukAbs);
	SetAxisInfo(iBdID, wAxis);

	//----------------------------------------------------------------------------
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos,  (long)dUpDistancePos);

	for (i=0; i<wOffNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, piOffAxis[i], ukG00Speed,  (long)pdOffVelocity[i]);
		FAS_WriteRam(iBdID, piOffAxis[i], ukGoalPosition,   (long)pdOffIncPos[i]);

		if (piOffAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piOffAxis[i]);
	}
	SetAxisInfoDrop(iBdID, iTmpAxis);	
	// --------------------------------------------------------------------------
	
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoValue,       dwOffValue); 
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTemp,		  bIsLow);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukMusashiOffPos, (long)dMusashiOffDistancePos); 
	// --------------------------------------------------------------------------

	SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0); 

	LEAVE_SYNCHRONIZATION(iBdID);

	do
	{
		FAS_GetAxisStatus(iBdID, wAxis, &AxisStatus);  	
		if (!(AxisStatus & FFLAG_MOTIONING)) MotionCount ++ ;
	} while(MotionCount < 3) ;    // Motion Stop 3번 Check

	return FMM_OK;
}

int FAS_MoveArcSingleIncPosWithSealOff(int iBdID, WORD wAxis, double IncPos, double Velocity,
			double dUpDistancePos, WORD wOffNoOfAxis, int *piOffAxis, double *pdOffIncPos, double *pdOffVelocity,
			double dMusashiOffDistancePos, DWORD dwOffValue, BOOL bIsLow)
{
	int i;
	int iTmpAxis = 0;
	long AxisStatus;
	int  MotionCount = 0 ;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);

	FAS_WriteRam(iBdID, wAxis, ukG00Speed, (long)Velocity); 
	FAS_WriteRam(iBdID, wAxis, ukGoalPosition,  (long)IncPos); 

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukArchSingle);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc, ukInc);
	SetAxisInfo(iBdID, wAxis);

	//----------------------------------------------------------------------------
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos, (long)dUpDistancePos);

	for (i=0; i<wOffNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, piOffAxis[i], ukG00Speed,  (long)pdOffVelocity[i]);
		FAS_WriteRam(iBdID, piOffAxis[i], ukGoalPosition,   (long)pdOffIncPos[i]);

		if (piOffAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piOffAxis[i]);
	}
	SetAxisInfoDrop(iBdID, iTmpAxis);	
	// --------------------------------------------------------------------------
	
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoValue,       dwOffValue); 
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukTemp,		  bIsLow);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukMusashiOffPos, (long)dMusashiOffDistancePos); 
	
	// --------------------------------------------------------------------------

	SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0); 

	LEAVE_SYNCHRONIZATION(iBdID);

	do
	{
		FAS_GetAxisStatus(iBdID, wAxis, &AxisStatus);  	
		if (!(AxisStatus & FFLAG_MOTIONING)) MotionCount ++ ;
	} while(MotionCount < 3) ;    // Motion Stop 3번 Check

	return FMM_OK;
}


// Question : D_StartEndPoint가 사용되지 않고 있다.
/* ***************************************************************************** */
int FAS_MoveArcIncPos(int iBdID, WORD NoOfAxis, WORD *Axis, double *IncPos,
			double D_Feedrate,  double D_CirFeedrate, double CornerDistance, WORD D_StartEndPoint)
{
	int i;
	int TempAxis = 0;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);

	for(i=0; i<NoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, Axis[i], ukGoalPosition, (long)IncPos[i]);

		if (Axis[i] < MAX_AXIS_NUMS) TempAxis |= (0x01 << Axis[i]);
	}
	SetAxisInfoMulti(iBdID, TempAxis);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,      ukLclMode);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,       (long)D_Feedrate);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCirFeedrate,    (long)D_CirFeedrate);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukCornerError,    (long)CornerDistance);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation,  ukArch);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,       ukInc);

	SetAxisInfoMulti(iBdID, TempAxis);

	SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0); 

	LEAVE_SYNCHRONIZATION(iBdID);

	return FMM_OK;
}


//////////////////////////////////////////////////////////////////////
int FAS_MoveLineDropAbs(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageAbsPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
			WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos, BOOL bIsLow, DWORD SyncAxis)
						
{
	long lAxisStatus;
    int  MotionCount = 0 ;
    int  iTmpAxis = 0;
    int  iMainTmpAxis = 0 ;
	int  i;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);

	for (i=0; i<wStageNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, wStageAxis[i], ukGoalPosition, (long)pdStageAbsPos[i]);

		if (wStageAxis[i] < MAX_AXIS_NUMS) iMainTmpAxis |= (0x01 << wStageAxis[i]);
	}

	SetAxisInfoMulti(iBdID, iMainTmpAxis);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,        (long)dStageFeedrate);  
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartSpeed,  (long)dStageStartSpeed);  
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,       ukExactStopMode);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation,   ukLineDrop);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,        ukAbs);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos,    (long)dDropStartPos);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukStartEndPoint,   wStartOrEnd);

////////////////////////////////////////////////////////////////////////////////////
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoBitNo, bIsLow);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoValue, SyncAxis);          // added end
////////////////////////////////////////////////////////////////////////////////////    
                                                


	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStop,        0);   //0 --> Line Drop

	for (i=0; i<wNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, piAxis[i], ukG00Speed, (long)pdVelocity[i]);
		FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition,  (long)pdIncPos[i]);

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
	}

	SetAxisInfoDrop(iBdID, iTmpAxis);		

	SetDpramCommand (iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);

	LEAVE_SYNCHRONIZATION(iBdID);

	if (wStartOrEnd == 2)
	{
		for(i=0; i<wStageNoOfAxis; i++)
		{  
			do
			{   
				FAS_GetAxisStatus(iBdID, wStageAxis[i], &lAxisStatus);  	
				if (!(lAxisStatus & FFLAG_MOTIONING)) MotionCount ++ ;
			} while(MotionCount < 3) ;   // Motion Stop 3번 Check

			MotionCount = 0 ;
		} 
	}

	return FMM_OK;
}

/*
int FAS_MoveLineDropAbs(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageAbsPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
			WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos)
						
{
	long AxisStatus ;
    int  MotionCount = 0 ;
    int  iTmpAxis = 0;
    int  iMainTmpAxis = 0 ;
	int  i;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);

	for (i=0; i<wStageNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, wStageAxis[i], ukGoalPosition, (long)pdStageAbsPos[i]);

		if (wStageAxis[i] < MAX_AXIS_NUMS) iMainTmpAxis |= (0x01 << wStageAxis[i]);
	}

	SetAxisInfoMulti(iBdID, iMainTmpAxis);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,        (long)dStageFeedrate);  
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartSpeed,  (long)dStageStartSpeed);  
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,       ukExactStopMode);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation,   ukLineDrop);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,        ukAbs);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos,    (long)dDropStartPos);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukStartEndPoint,   wStartOrEnd);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStop,        0);   //0 --> Line Drop

	for (i=0; i<wNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, piAxis[i], ukG00Speed, (long)pdVelocity[i]);
		FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition,  (long)pdIncPos[i]);

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
	}

	SetAxisInfoDrop(iBdID, iTmpAxis);		

	SetDpramCommand (iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);

	LEAVE_SYNCHRONIZATION(iBdID);

	if (wStartOrEnd == 2)
	{
		for(i=0; i<wStageNoOfAxis; i++)
		{  
			do
			{   
				FAS_GetAxisStatus(iBdID, wStageAxis[i], &AxisStatus);  	
				if (!(AxisStatus & FFLAG_MOTIONING)) MotionCount ++ ;
			} while(MotionCount < 3) ;   // Motion Stop 3번 Check

			MotionCount = 0 ;
		} 
	}

	return FMM_OK;
}
*/
//////////////////////////////////////////////////////////////////////
int FAS_MoveLineDropInc(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageIncPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
			WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos)
						
{
	long AxisStatus ;
	int  MotionCount = 0 ;
	int  iTmpAxis = 0;
	int  iMainTmpAxis = 0 ;
	int  i;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);

	for (i=0; i<wStageNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, wStageAxis[i], ukGoalPosition, (long)pdStageIncPos[i]);

		if (wStageAxis[i] < MAX_AXIS_NUMS) iMainTmpAxis |= (0x01 << wStageAxis[i]);
	}

	SetAxisInfoMulti(iBdID, iMainTmpAxis);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,        (long)dStageFeedrate);  
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartSpeed,  (long)dStageStartSpeed); 
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,       ukExactStopMode);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation,   ukLineDrop);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,        ukInc);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos,    (long)dDropStartPos);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukStartEndPoint,   wStartOrEnd);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStop,        0);

	for (i=0; i<wNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, piAxis[i], ukG00Speed, (long)pdVelocity[i]);
		FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition,  (long)pdIncPos[i]);

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
	}
	SetAxisInfoDrop(iBdID, iTmpAxis);		

	SetDpramCommand (iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);

	LEAVE_SYNCHRONIZATION(iBdID);

	if (wStartOrEnd == 2)
	{
		for(i=0; i<wStageNoOfAxis; i++)
		{  
			do
			{   
				FAS_GetAxisStatus(iBdID, wStageAxis[i], &AxisStatus);  	
				if (!(AxisStatus & FFLAG_MOTIONING)) MotionCount ++ ;
			} while(MotionCount < 3) ;   // Motion Stop 3번 Check

			MotionCount = 0 ;
		} 
	}

	return FMM_OK;
}

//////////////////////////////////////////////////////////////////////
int FAS_MoveDropSyncAbs(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageAbsPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
			WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos, BOOL bIsLow, DWORD SyncAxis)
						
{
	long lAxisStatus;
	int  MotionCount = 0 ;
	int  iTmpAxis = 0;
	int  iMainTmpAxis = 0 ;
	int  i;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);

	for (i=0; i<wStageNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, wStageAxis[i], ukGoalPosition, (long)pdStageAbsPos[i]);

		if (wStageAxis[i] < MAX_AXIS_NUMS) iMainTmpAxis |= (0x01 << wStageAxis[i]);
	}

	SetAxisInfoMulti(iBdID, iMainTmpAxis);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,        (long)dStageFeedrate);       
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartSpeed,  (long)dStageStartSpeed); 
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,       ukExactStopMode);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation,   ukLineDrop);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,        ukAbs);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos,    (long)dDropStartPos);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukStartEndPoint,   wStartOrEnd);

////////////////////////////////////////////////////////////////////////////////////
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoBitNo, bIsLow);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoValue, SyncAxis);          // added end
////////////////////////////////////////////////////////////////////////////////////    

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStop,        1);

	for (i=0; i<wNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, piAxis[i], ukG00Speed, (long)pdVelocity[i]);
		FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition,  (long)pdIncPos[i]);

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
	}
	SetAxisInfoDrop(iBdID, iTmpAxis);		

	SetDpramCommand (iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);

	LEAVE_SYNCHRONIZATION(iBdID);

	if (wStartOrEnd == 2)
	{
		for(i=0; i<wStageNoOfAxis; i++)
		{  
			do
			{   
				FAS_GetAxisStatus(iBdID, wStageAxis[i], &lAxisStatus);  	
				if (!(lAxisStatus & FFLAG_MOTIONING)) MotionCount ++ ;
			} while(MotionCount < 3) ;   // Motion Stop 3번 Check

			MotionCount = 0 ;
		} 
	}

	return FMM_OK;
}

/*
int FAS_MoveDropSyncAbs(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageAbsPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
			WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos)
						
{
	long AxisStatus ;
	int  MotionCount = 0 ;
	int  iTmpAxis = 0;
	int  iMainTmpAxis = 0 ;
	int  i;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);

	for (i=0; i<wStageNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, wStageAxis[i], ukGoalPosition, (long)pdStageAbsPos[i]);

		if (wStageAxis[i] < MAX_AXIS_NUMS) iMainTmpAxis |= (0x01 << wStageAxis[i]);
	}

	SetAxisInfoMulti(iBdID, iMainTmpAxis);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,        (long)dStageFeedrate);       
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartSpeed,  (long)dStageStartSpeed); 
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,       ukExactStopMode);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation,   ukLineDrop);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,        ukAbs);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos,    (long)dDropStartPos);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukStartEndPoint,   wStartOrEnd);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStop,        1);

	for (i=0; i<wNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, piAxis[i], ukG00Speed, (long)pdVelocity[i]);
		FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition,  (long)pdIncPos[i]);

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
	}
	SetAxisInfoDrop(iBdID, iTmpAxis);		

	SetDpramCommand (iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);

	LEAVE_SYNCHRONIZATION(iBdID);

	if (wStartOrEnd == 2)
	{
		for(i=0; i<wStageNoOfAxis; i++)
		{  
			do
			{   
				FAS_GetAxisStatus(iBdID, wStageAxis[i], &AxisStatus);  	
				if (!(AxisStatus & FFLAG_MOTIONING)) MotionCount ++ ;
			} while(MotionCount < 3) ;   // Motion Stop 3번 Check

			MotionCount = 0 ;
		} 
	}

	return FMM_OK;
}
*/
//////////////////////////////////////////////////////////////////////
int FAS_MoveDropSyncInc(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageIncPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
			WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos)
						
{
	long AxisStatus ;
	int  MotionCount = 0 ;
	int  iTmpAxis = 0;
	int  iMainTmpAxis = 0 ;
	int  i;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);

	for (i=0; i<wStageNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, wStageAxis[i], ukGoalPosition, (long)pdStageIncPos[i]);

		if (wStageAxis[i] < MAX_AXIS_NUMS) iMainTmpAxis |= (0x01 << wStageAxis[i]);
	}

	SetAxisInfoMulti(iBdID, iMainTmpAxis);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,        (long)dStageFeedrate);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartSpeed,  (long)dStageStartSpeed); 
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,       ukExactStopMode);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation,   ukLineDrop);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,        ukInc);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos,    (long)dDropStartPos);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukStartEndPoint,   wStartOrEnd);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStop,        1);

	for (i=0; i<wNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, piAxis[i], ukG00Speed, (long)pdVelocity[i]);
		FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition,  (long)pdIncPos[i]);

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
	}
	SetAxisInfoDrop(iBdID, iTmpAxis);		

	SetDpramCommand (iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);

	LEAVE_SYNCHRONIZATION(iBdID);

	if (wStartOrEnd == 2)
	{
		for(i=0; i<wStageNoOfAxis; i++)
		{  
			do
			{   
				FAS_GetAxisStatus(iBdID, wStageAxis[i], &AxisStatus);  	
				if (!(AxisStatus & FFLAG_MOTIONING)) MotionCount ++ ;
			} while(MotionCount < 3) ;   // Motion Stop 3번 Check

			MotionCount = 0 ;
		} 
	}

	return FMM_OK;
}

//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////  2004.1.1
int FAS_GapControlEnable(int iBdID, WORD wAxis, BOOL bOnOff)
{
    if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

	ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd66 In\n", iBdID+1);
	FAS_WriteRam(iBdID, wAxis, ukTrueOrFalse, bOnOff);
	SetAxisInfo(iBdID,  wAxis);

	SetDpramCommand(iBdID,  wAxis, ukGapControlEnable, 0);
	WaitDpramCommand(iBdID, wAxis, ukGapControlEnable, 0);
	LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd66 Out\n", iBdID+1);

	return FMM_OK;
}

//////////////////////////////////////////////////////////////////////
int FAS_GapControlEnableMulti(int iBdID, WORD wMultiAxis)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
	
    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd67 In\n", iBdID+1);
    SetAxisInfoMulti(iBdID, wMultiAxis);

    SetDpramCommand(iBdID,  AXIS_GLOBAL, ukGapControlEnableMulti, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukGapControlEnableMulti, 0);
	LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd67 Out\n", iBdID+1);
    
    return FMM_OK;
}

//////////////////////////////////////////////////////////////////////
int FAS_GapControlDisableMulti(int iBdID, WORD wMultiAxis)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd68 In\n", iBdID+1);
    SetAxisInfoMulti(iBdID, wMultiAxis);

    SetDpramCommand(iBdID,  AXIS_GLOBAL, ukGapControlDisableMulti, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukGapControlDisableMulti, 0);
	LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd68 Out\n", iBdID+1);
    
    return FMM_OK;
}

//////////////////////////////////////////////////////////////   2004.1.1  as below
int FAS_MoveDropSyncAbsP6(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageAbsPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
                          WORD wSyncAxis, WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos)
						
{
	long AxisStatus ;
	int  MotionCount = 0 ;
	int  iTmpAxis = 0;
	int  iMainTmpAxis = 0 ;
	int  i;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);
	for (i=0; i<wStageNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, wStageAxis[i], ukGoalPosition, (long)pdStageAbsPos[i]);

		if (wStageAxis[i] < MAX_AXIS_NUMS) iMainTmpAxis |= (0x01 << wStageAxis[i]);
	}

	SetAxisInfoMulti(iBdID, iMainTmpAxis);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,        (long)dStageFeedrate);       
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartSpeed,  (long)dStageStartSpeed); 
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,       ukExactStopMode);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation,   ukLineDrop);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,        ukAbs);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos,    (long)dDropStartPos);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukStartEndPoint,   wStartOrEnd);
    
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoValue,         wSyncAxis);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStop,        1);

	for (i=0; i<wNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, piAxis[i], ukG00Speed, (long)pdVelocity[i]);
		FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition,  (long)pdIncPos[i]);

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
	}
	SetAxisInfoDrop(iBdID, iTmpAxis);		

	SetDpramCommand (iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);

	LEAVE_SYNCHRONIZATION(iBdID);

	if (wStartOrEnd == 2)
	{
		for(i=0; i<wStageNoOfAxis; i++)
		{  
			do
			{   
				FAS_GetAxisStatus(iBdID, wStageAxis[i], &AxisStatus);  	
				if (!(AxisStatus & FFLAG_MOTIONING)) MotionCount ++ ;
			} while(MotionCount < 3) ;   // Motion Stop 3번 Check

			MotionCount = 0 ;
		} 
	}

	return FMM_OK;
}

int FAS_MoveLineDropAbsP6(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageAbsPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
                          WORD wSyncAxis, WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos)
						
{
	long AxisStatus ;
    int  MotionCount = 0 ;
    int  iTmpAxis = 0;
    int  iMainTmpAxis = 0 ;
	int  i;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);

	for (i=0; i<wStageNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, wStageAxis[i], ukGoalPosition, (long)pdStageAbsPos[i]);

		if (wStageAxis[i] < MAX_AXIS_NUMS) iMainTmpAxis |= (0x01 << wStageAxis[i]);
	}

	SetAxisInfoMulti(iBdID, iMainTmpAxis);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,        (long)dStageFeedrate);  
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartSpeed,  (long)dStageStartSpeed);  
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,       ukExactStopMode);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation,   ukLineDrop);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,        ukAbs);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos,    (long)dDropStartPos);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukStartEndPoint,   wStartOrEnd);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoValue,         wSyncAxis);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStop,        0);   //0 --> Line Drop

	for (i=0; i<wNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, piAxis[i], ukG00Speed, (long)pdVelocity[i]);
		FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition,  (long)pdIncPos[i]);

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
	}

	SetAxisInfoDrop(iBdID, iTmpAxis);		

	SetDpramCommand (iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);

	LEAVE_SYNCHRONIZATION(iBdID);

	if (wStartOrEnd == 2)
	{
		for(i=0; i<wStageNoOfAxis; i++)
		{  
			do
			{   
				FAS_GetAxisStatus(iBdID, wStageAxis[i], &AxisStatus);  	
				if (!(AxisStatus & FFLAG_MOTIONING)) MotionCount ++ ;
			} while(MotionCount < 3) ;   // Motion Stop 3번 Check

			MotionCount = 0 ;
		} 
	}

	return FMM_OK;
}

//////////////////////////////////////////////////////////////////////
int FAS_GetGapError(int iBdID, WORD wAxis,	BOOL *bGapError)
{
    long lTmp;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
	
	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
    
	SetAxisInfo(iBdID, wAxis);
    SetDpramCommand(iBdID,  wAxis, ukGetGapError, ukGeneral);
    WaitDpramCommand(iBdID, wAxis, ukGetGapError, 0);

	lTmp = FAS_ReadRam(iBdID, wAxis, ukGeneral);

	LEAVE_SYNCHRONIZATION(iBdID);

    *bGapError = (BOOL)lTmp;

    return FMM_OK;
}

//////////////////////////////////////////////////////////////////////
int FAS_ClearGapError(int iBdID, WORD wAxis)
{
    if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
    SetAxisInfo(iBdID, wAxis);

	SetDpramCommand(iBdID,  wAxis, ukClearGapError, 0);
    WaitDpramCommand(iBdID, wAxis, ukClearGapError, 0);
    LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}

//////////////////////////////////////////////////////////////////////
int FAS_GetGapErrorCount(int iBdID, WORD wAxis,	int *iGapErrorCount)
{
    long lTmp;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
	
	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID,  wAxis, ukGetGapErrorCount, ukGeneral);
    WaitDpramCommand(iBdID, wAxis, ukGetGapErrorCount, 0);

	lTmp = FAS_ReadRam(iBdID, wAxis, ukGeneral);
	LEAVE_SYNCHRONIZATION(iBdID);

    *iGapErrorCount = (int)lTmp;

    return FMM_OK;
}

// Add 2004/1003
int FAS_SetSyncSlaveAxes(int iBdID, int iAxes)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukSyncAxis, iAxes);

    SetDpramCommand(iBdID,  iAxes, ukSetSyncSlaveAxes, 0);
    WaitDpramCommand(iBdID, iAxes, ukSetSyncSlaveAxes, 0);
	LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}

int FAS_SetSyncMasterAxis(int iBdID, int iAxes)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukSyncAxis, iAxes);

	SetDpramCommand(iBdID,  iAxes, ukSetSyncMasterAxis, 0);
	WaitDpramCommand(iBdID, iAxes, ukSetSyncMasterAxis, 0);
	LEAVE_SYNCHRONIZATION(iBdID);

	return FMM_OK;
}

// Add 2004/1015
int FAS_CornerCompEnable(int iBdID, WORD wAxis, BOOL bOnOff)
{
    if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd69 In\n", iBdID+1);
	SetAxisInfo(iBdID, wAxis);
	FAS_WriteRam(iBdID, wAxis, ukTrueOrFalse, bOnOff);

	SetDpramCommand(iBdID, wAxis, ukCornerCompEnable, 0);
    WaitDpramCommand(iBdID, wAxis, ukCornerCompEnable, 0);
	LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd69 Out\n", iBdID+1);

    return FMM_OK;
}

int FAS_CornerCompEnableMulti(int iBdID, WORD wMultiAxis)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd70 In\n", iBdID+1);
    SetAxisInfoMulti(iBdID, wMultiAxis);

    SetDpramCommand(iBdID,  AXIS_GLOBAL, ukCornerCompEnableMulti, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukCornerCompEnableMulti, 0);
	LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd70 Out\n", iBdID+1);
    
    return FMM_OK;
}

//////////////////////////////////////////////////////////////////////
int FAS_CornerCompDisableMulti(int iBdID, WORD wMultiAxis)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd71 In\n", iBdID+1);
    SetAxisInfoMulti(iBdID, wMultiAxis);

    SetDpramCommand(iBdID,  AXIS_GLOBAL, ukCornerCompDisableMulti, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukCornerCompDisableMulti, 0);
	LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd71 Out\n", iBdID+1);
    
    return FMM_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////
int FAS_ResetSyncMasterAxes(int iBdID)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	SetDpramCommand(iBdID, AXIS_GLOBAL, ukResetSyncMasterAxis, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukResetSyncMasterAxis, 0);
	LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}


//------------------------------------------------------------------
//                About Laser Senser Functions.   (2005/03/28)
//------------------------------------------------------------------
int FAS_GetLaserSensor(int iBdID, WORD wChannel, long *lGetLaser)
{
	long wLaserData;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd72 In\n", iBdID+1);
	SetAxisInfo(iBdID, wChannel);

    SetDpramCommand(iBdID, wChannel, ukGetLaserSensor, ukLaserSensor);
    WaitDpramCommand(iBdID, wChannel, ukGetLaserSensor, 0);

	wLaserData = FAS_ReadRam(iBdID, wChannel, ukLaserSensor);
	LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd72 Out\n", iBdID+1);
   
	*lGetLaser = wLaserData;

    return FMM_OK;
}


//------------------------------------------------------------------
//                About Monitoring Functions.   (2006/12/20)
//------------------------------------------------------------------

int FAS_GetMonitoring(int iBdID, WORD wAxis, double *dMonIndex, double *dMonCommadPos, double *dMonCurPos, 
                      double *dMonGapSensor, double *dMonCompPos)
{
    long lTmp1, lTmp2,lTmp3,lTmp4,lTmp5;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
	
	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;
    
    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd73 In\n", iBdID+1);

    SetAxisInfo(iBdID, wAxis);
    SetDpramCommand(iBdID,  wAxis, ukGetMonitoring, ukMonIndex);
    WaitDpramCommand(iBdID, wAxis, ukGetMonitoring, 0);   

 	  lTmp1 = FAS_ReadRam(iBdID, wAxis, ukMonIndex);
 	  lTmp2 = FAS_ReadRam(iBdID, wAxis, ukMonCommandPos);
 	  lTmp3 = FAS_ReadRam(iBdID, wAxis, ukMonCurPos);
 	  lTmp4 = FAS_ReadRam(iBdID, wAxis, ukMonGapSensor);
 	  lTmp5 = FAS_ReadRam(iBdID, wAxis, ukMonCompPos);
  
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd73 Out\n", iBdID+1);
	
    *dMonIndex       = (double)lTmp1;
    *dMonCommadPos   = (double)lTmp2;
    *dMonCurPos      = (double)lTmp3;
    *dMonGapSensor   = (double)lTmp4;
    *dMonCompPos     = (double)lTmp5;

    return FMM_OK;
}

int FAS_GetMonitoring(int iBdID, WORD wAxis, double *dMonIndex, double *dMonCommadPos, double *dMonCurPos, 
                      double *dMonGapSensor, double *dMonCompPos, double *dMonLCGo)
{
    long lTmp1, lTmp2,lTmp3,lTmp4,lTmp5,lTmp6;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
	
	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;
    
    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd74 In\n", iBdID+1);

    SetAxisInfo(iBdID, wAxis);
    SetDpramCommand(iBdID,  wAxis, ukGetMonitoring, ukMonIndex);
    WaitDpramCommand(iBdID, wAxis, ukGetMonitoring, 0);   

 	  lTmp1 = FAS_ReadRam(iBdID, wAxis, ukMonIndex);
 	  lTmp2 = FAS_ReadRam(iBdID, wAxis, ukMonCommandPos);
 	  lTmp3 = FAS_ReadRam(iBdID, wAxis, ukMonCurPos);
 	  lTmp4 = FAS_ReadRam(iBdID, wAxis, ukMonGapSensor);
 	  lTmp5 = FAS_ReadRam(iBdID, wAxis, ukMonCompPos);
	  lTmp6 = FAS_ReadRam(iBdID, wAxis, ukMonLCGo);
  
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd47 Out\n", iBdID+1);
	
    *dMonIndex       = (double)lTmp1;
    *dMonCommadPos   = (double)lTmp2;
    *dMonCurPos      = (double)lTmp3;
    *dMonGapSensor   = (double)lTmp4;
    *dMonCompPos     = (double)lTmp5;
	*dMonLCGo		 = (double)lTmp6;

    return FMM_OK;
}

//ssh 100123
int FAS_GetMonitoring(int iBdID, WORD wAxis, double *dMonIndex, double *dMonCommadPos, double *dMonCurPos, 
                      double *dMonGapSensor, double *dMonCompPos, double *dMonLCGo, double *dMonPosX, double *dMonPosY, double *dMonSpeed)
{
    long lTmp1, lTmp2,lTmp3,lTmp4,lTmp5,lTmp6,lTmp7,lTmp8,lTmp9;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
	
	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;
    
    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd75 In\n", iBdID+1);

    SetAxisInfo(iBdID, wAxis);
    SetDpramCommand(iBdID,  wAxis, ukGetMonitoring, ukMonIndex);
    WaitDpramCommand(iBdID, wAxis, ukGetMonitoring, 0);   

 	  lTmp1 = FAS_ReadRam(iBdID, wAxis, ukMonIndex);
 	  lTmp2 = FAS_ReadRam(iBdID, wAxis, ukMonCommandPos);
 	  lTmp3 = FAS_ReadRam(iBdID, wAxis, ukMonCurPos);
 	  lTmp4 = FAS_ReadRam(iBdID, wAxis, ukMonGapSensor);
 	  lTmp5 = FAS_ReadRam(iBdID, wAxis, ukMonCompPos);
	  lTmp6 = FAS_ReadRam(iBdID, wAxis, ukMonLCGo);
	  lTmp7 = FAS_ReadRam(iBdID, wAxis, ukMonCommandPosX);
	  lTmp8 = FAS_ReadRam(iBdID, wAxis, ukMonCommandPosY);
	  lTmp9 = FAS_ReadRam(iBdID, wAxis, ukMonSpeedXY);
  
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd75 Out\n", iBdID+1);
	
    *dMonIndex       = (double)lTmp1;
    *dMonCommadPos   = (double)lTmp2;
    *dMonCurPos      = (double)lTmp3;
    *dMonGapSensor   = (double)lTmp4;
    *dMonCompPos     = (double)lTmp5;
	*dMonLCGo		 = (double)lTmp6;
	*dMonPosX		 = (double)lTmp7;
	*dMonPosY		 = (double)lTmp8;
	*dMonSpeed		 = (double)lTmp9;

    return FMM_OK;
}

int FAS_GetMultiMonitoring(int iBdID, WORD wNoOfAxis, int *piAxis, long *lMonIndex, long *lMonCommadPos, long *lMonCurPos, 
                      long *lMonGapSensor, long *lMonCompPos)
{
    long lTmp1[AXIS_GLOBAL], lTmp2[AXIS_GLOBAL],lTmp3[AXIS_GLOBAL],lTmp4[AXIS_GLOBAL],lTmp5[AXIS_GLOBAL];
	int  iTmpAxis = 0;
	int i;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
	
	for(i=0; i<wNoOfAxis; i++)
	{
		if ((piAxis[i] >= awNoAxis[iBdID]) || (piAxis[i] < 0)) 
			return FMM_INVALID_AXIS_NUM;
	}
    
    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd76 In\n", iBdID+1);

	for(i=0; i<wNoOfAxis; i++)
	{
		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
	}

	SetAxisInfoMulti(iBdID, iTmpAxis);

    SetDpramCommand(iBdID,  AXIS_GLOBAL, ukGetMonitoring, ukMonIndex);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukGetMonitoring, 0);   

	for(i=0; i<wNoOfAxis; i++)
	{
 	  lTmp1[i] = FAS_ReadRam(iBdID, piAxis[i], ukMonIndex);
 	  lTmp2[i] = FAS_ReadRam(iBdID, piAxis[i], ukMonCommandPos);
 	  lTmp3[i] = FAS_ReadRam(iBdID, piAxis[i], ukMonCurPos);
 	  lTmp4[i] = FAS_ReadRam(iBdID, piAxis[i], ukMonGapSensor);
 	  lTmp5[i] = FAS_ReadRam(iBdID, piAxis[i], ukMonCompPos);

	  //TRACE("Axis %d, Index %d, CMD %d, POS %d, Gap %d, Comp %d\n", piAxis[i], lTmp1[i], lTmp2[i], lTmp3[i], lTmp4[i], lTmp5[i]);
	}
  
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd67 Out\n", iBdID+1);
	
    lMonIndex       = lTmp1;
    lMonCommadPos   = lTmp2;
    lMonCurPos      = lTmp3;
    lMonGapSensor   = lTmp4;
    lMonCompPos     = lTmp5;

    return FMM_OK;
}

//ssh 100123
int FAS_GetMultiMonitoring(int iBdID, WORD wNoOfAxis, int *piAxis, double *dMonIndex, double *dMonCommadPos, double *dMonCurPos, 
                      double *dMonGapSensor, double *dMonCompPos, double *dMonLCGo, double *dMonPosX, double *dMonPosY, double *dMonSpeed)
{
    long lTmp1,lTmp2,lTmp3,lTmp4,lTmp5,lTmp6,lTmp7,lTmp8,lTmp9;
	int  iTmpAxis = 0;
	int i;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
	
	for(i=0; i<wNoOfAxis; i++)
	{
		if ((piAxis[i] >= awNoAxis[iBdID]) || (piAxis[i] < 0)) 
			return FMM_INVALID_AXIS_NUM;
	}
    
    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd77 In\n", iBdID+1);

	for(i=0; i<wNoOfAxis; i++)
	{
		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
	}

	SetAxisInfoMulti(iBdID, iTmpAxis);

    SetDpramCommand(iBdID,  AXIS_GLOBAL, ukGetMonitoring, ukMonIndex);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukGetMonitoring, 0);   

	for(i=0; i<wNoOfAxis; i++)
	{		
		lTmp1 = FAS_ReadRam(iBdID, piAxis[i], ukMonIndex);
		lTmp2 = FAS_ReadRam(iBdID, piAxis[i], ukMonCommandPos);
 		lTmp3 = FAS_ReadRam(iBdID, piAxis[i], ukMonCurPos);
 		lTmp4 = FAS_ReadRam(iBdID, piAxis[i], ukMonGapSensor);
 		lTmp5 = FAS_ReadRam(iBdID, piAxis[i], ukMonCompPos);
		lTmp6 = FAS_ReadRam(iBdID, piAxis[i], ukMonLCGo);
		lTmp7 = FAS_ReadRam(iBdID, piAxis[i], ukMonCommandPosX);
		lTmp8 = FAS_ReadRam(iBdID, piAxis[i], ukMonCommandPosY);
		lTmp9 = FAS_ReadRam(iBdID, piAxis[i], ukMonSpeedXY);

		//TRACE("Axis %d, Index %d, CMD %d, POS %d, Gap %d, Comp %d, LCGo %d, PosX %d, PosY %d, Speed %d\n", piAxis[i], lTmp1, lTmp2, lTmp3, lTmp4, lTmp5, lTmp6, lTmp7, lTmp8, lTmp9);
		  
		dMonIndex[i]		= (double)lTmp1;
		dMonCommadPos[i]	= (double)lTmp2;
		dMonCurPos[i]       = (double)lTmp3;
		dMonGapSensor[i]    = (double)lTmp4;
		dMonCompPos[i]      = (double)lTmp5;
		dMonLCGo[i]			= (double)lTmp6;
		dMonPosX[i]			= (double)lTmp7;
		dMonPosY[i]			= (double)lTmp8;
		dMonSpeed[i]	    = (double)lTmp9;
	}
  
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd77 Out\n", iBdID+1);
	

    return FMM_OK;
}

//------------------------------------------------------------------
//                DropCounter Monitoring.   (2007/10/10)
//------------------------------------------------------------------
/* **************************************************************** */
int FAS_EnableDropCount(int iBdID, WORD wAxis, WORD wDropCounterDelay)
{
//	int nRtn;

  if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
	
  ENTER_SYNCHRONIZATION(iBdID);
  //SetAxisInfo(iBdID, wAxis);

  SetAxisInfoMulti(iBdID, wAxis);
  FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropCountDelay, wDropCounterDelay);
  
  SetDpramCommand(iBdID, AXIS_GLOBAL,  ukEnableDropCount, 0);
  WaitDpramCommand(iBdID, AXIS_GLOBAL, ukEnableDropCount, 0);
  LEAVE_SYNCHRONIZATION(iBdID);

  return FMM_OK;
	
}

/* **************************************************************** */
/* **************************************************************** */
int FAS_GetMonitoringDropCount(int iBdID, long *dMonIndexDrop, long *dMonCommandPosDropX, 
                            long *dMonCommandPosDropY, long *dMonIOLowDrop, long *dMonIOHihgDrop)
{
  long lTmp1, lTmp2,lTmp3,lTmp4,lTmp5;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
	
//	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
//		return FMM_INVALID_AXIS_NUM;
    
    ENTER_SYNCHRONIZATION(iBdID);

    SetDpramCommand( iBdID, AXIS_GLOBAL, ukGetMonitoringDropCount, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukGetMonitoringDropCount, 0);
    
 
 	  lTmp1 = FAS_ReadRam(iBdID, AXIS_GLOBAL, ukMonIndexDrop);
 	  lTmp2 = FAS_ReadRam(iBdID, AXIS_GLOBAL, ukMonCommandPosDropX);
 	  lTmp3 = FAS_ReadRam(iBdID, AXIS_GLOBAL, ukMonCommandPosDropY);
 	  lTmp4 = FAS_ReadRam(iBdID, AXIS_GLOBAL, ukMonIOLowDrop);
 	  lTmp5 = FAS_ReadRam(iBdID, AXIS_GLOBAL, ukMonIOHihgDrop);
  
    LEAVE_SYNCHRONIZATION(iBdID);
	
    *dMonIndexDrop          = lTmp1;
    *dMonCommandPosDropX    = lTmp2;
    *dMonCommandPosDropY    = lTmp3;
    *dMonIOLowDrop          = lTmp4;
    *dMonIOHihgDrop         = lTmp5;

    
    return FMM_OK;
}


//SPD STAGE LINE DROP//Hybrid
int FAS_MoveLineDropAbsStepHybrid(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageAbsPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
			                WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos, WORD wBitNo, BOOL bHybridOn)
{
	long AxisStatus ;
    int  MotionCount = 0 ;
    int  iTmpAxis = 0;
    int  iMainTmpAxis = 0 ;
	int  i;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);

	for (i=0; i<wStageNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, wStageAxis[i], ukGoalPosition, (long)pdStageAbsPos[i]);

		if (wStageAxis[i] < MAX_AXIS_NUMS) iMainTmpAxis |= (0x01 << wStageAxis[i]);
	}

	SetAxisInfoMulti(iBdID, iMainTmpAxis);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,        (long)dStageFeedrate);  
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartSpeed,  (long)dStageStartSpeed);  
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,       ukExactStopMode);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation,   ukLineDrop);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,        ukAbs);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos,    (long)dDropStartPos);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukStartEndPoint,   wStartOrEnd);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoBitNo,         (long)wBitNo);                     // added

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStop,        0);   //0 --> Line Drop
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropCompleteOn, bHybridOn);

	for (i=0; i<wNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, piAxis[i], ukG00Speed, (long)pdVelocity[i]);
		FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition,  (long)pdIncPos[i]);		

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
	}

	SetAxisInfoDrop(iBdID, iTmpAxis);		

	SetDpramCommand (iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);

	LEAVE_SYNCHRONIZATION(iBdID);

	if (wStartOrEnd == 2)
	{
		for(i=0; i<wStageNoOfAxis; i++)
		{  
			do
			{   
				FAS_GetAxisStatus(iBdID, wStageAxis[i], &AxisStatus);  	
				if (!(AxisStatus & FFLAG_MOTIONING)) MotionCount ++ ;
			} while(MotionCount < 3) ;   // Motion Stop 2번 Check

			MotionCount = 0 ;
		} 
	}

	return FMM_OK;
}



//SPD STAGE LINE DROP
int FAS_MoveLineDropAbsStep(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageAbsPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
			WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos, WORD wBitNo)
						
{
	long AxisStatus ;
    int  MotionCount = 0 ;
    int  iTmpAxis = 0;
    int  iMainTmpAxis = 0 ;
	int  i;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);

	for (i=0; i<wStageNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, wStageAxis[i], ukGoalPosition, (long)pdStageAbsPos[i]);

		if (wStageAxis[i] < MAX_AXIS_NUMS) iMainTmpAxis |= (0x01 << wStageAxis[i]);
	}

	SetAxisInfoMulti(iBdID, iMainTmpAxis);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,        (long)dStageFeedrate);  
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartSpeed,  (long)dStageStartSpeed);  
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,       ukExactStopMode);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation,   ukLineDrop);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,        ukAbs);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos,    (long)dDropStartPos);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukStartEndPoint,   wStartOrEnd);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoBitNo,         (long)wBitNo);                     // added

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStop,        0);   //0 --> Line Drop

	for (i=0; i<wNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, piAxis[i], ukG00Speed, (long)pdVelocity[i]);
		FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition,  (long)pdIncPos[i]);

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
	}

	SetAxisInfoDrop(iBdID, iTmpAxis);		

	SetDpramCommand (iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);

	LEAVE_SYNCHRONIZATION(iBdID);

	if (wStartOrEnd == 2)
	{
		for(i=0; i<wStageNoOfAxis; i++)
		{  
			do
			{   
				FAS_GetAxisStatus(iBdID, wStageAxis[i], &AxisStatus);  	
				if (!(AxisStatus & FFLAG_MOTIONING)) MotionCount ++ ;
			} while(MotionCount < 3) ;   // Motion Stop 2번 Check

			MotionCount = 0 ;
		} 
	}

	return FMM_OK;
}

//SPD STAGE LINE DROP(EZI+R)
int FAS_MoveLineDropAbsStep(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageAbsPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
			WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos, WORD wBitNo, DWORD dwDropIoValue, BOOL bDropIoHighLow)
						
{
	long AxisStatus ;
    int  MotionCount = 0 ;
    int  iTmpAxis = 0;
    int  iMainTmpAxis = 0 ;
	int  i;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);

	for (i=0; i<wStageNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, wStageAxis[i], ukGoalPosition, (long)pdStageAbsPos[i]);

		if (wStageAxis[i] < MAX_AXIS_NUMS) iMainTmpAxis |= (0x01 << wStageAxis[i]);
	}

	SetAxisInfoMulti(iBdID, iMainTmpAxis);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,        (long)dStageFeedrate);  
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartSpeed,  (long)dStageStartSpeed);  
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,       ukExactStopMode);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation,   ukLineDrop);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,        ukAbs);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos,    (long)dDropStartPos);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukStartEndPoint,   wStartOrEnd);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoBitNo,         (long)wBitNo);                     // added

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStop,        0);   //0 --> Line Drop

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropIo, dwDropIoValue);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropIoHighOrLow, bDropIoHighLow);

	for (i=0; i<wNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, piAxis[i], ukG00Speed, (long)pdVelocity[i]);
		FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition,  (long)pdIncPos[i]);

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
	}

	SetAxisInfoDrop(iBdID, iTmpAxis);		

	SetDpramCommand (iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);

	LEAVE_SYNCHRONIZATION(iBdID);

	if (wStartOrEnd == 2)
	{
		for(i=0; i<wStageNoOfAxis; i++)
		{  
			do
			{   
				FAS_GetAxisStatus(iBdID, wStageAxis[i], &AxisStatus);  	
				if (!(AxisStatus & FFLAG_MOTIONING)) MotionCount ++ ;
			} while(MotionCount < 3) ;   // Motion Stop 2번 Check

			MotionCount = 0 ;
		} 
	}

	return FMM_OK;
}

//TJD STAGE LINE DROP
int FAS_MoveLineDropAbsStep(int iBdID, WORD wStageNoOfAxis, WORD *wStageAxis, double *pdStageAbsPos, double dStageFeedrate, double dStageStartSpeed, WORD wStartOrEnd,
			WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, double dDropStartPos, WORD wBitNo, int *startDelay, int *stopDelay, int *inposValue)
						
{
	long AxisStatus ;
    int  MotionCount = 0 ;
    int  iTmpAxis = 0;
    int  iMainTmpAxis = 0 ;
	int  i;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	ENTER_SYNCHRONIZATION(iBdID);

	for (i=0; i<wStageNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, wStageAxis[i], ukGoalPosition, (long)pdStageAbsPos[i]);

		if (wStageAxis[i] < MAX_AXIS_NUMS) iMainTmpAxis |= (0x01 << wStageAxis[i]);
	}

	SetAxisInfoMulti(iBdID, iMainTmpAxis);

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukFeedRate,        (long)dStageFeedrate);  
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartSpeed,  (long)dStageStartSpeed);  
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode,       ukExactStopMode);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation,   ukLineDrop);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc,        ukAbs);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStartPos,    (long)dDropStartPos);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukStartEndPoint,   wStartOrEnd);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukIoBitNo,         (long)wBitNo);                     // added

	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukDropStop,        0);   //0 --> Line Drop

	for (i=0; i<wNoOfAxis; i++)
	{
		FAS_WriteRam(iBdID, piAxis[i], ukG00Speed, (long)pdVelocity[i]);
		FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition,  (long)pdIncPos[i]);

		FAS_WriteRam(iBdID, piAxis[i], ukTJDStartDelay, (long)startDelay[i]);
		FAS_WriteRam(iBdID, piAxis[i], ukTJDStopDelay, (long)stopDelay[i]);
		FAS_WriteRam(iBdID, piAxis[i], ukTJDInposValue, (long)inposValue[i]);

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
	}

	SetAxisInfoDrop(iBdID, iTmpAxis);		

	SetDpramCommand (iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
	WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);

	LEAVE_SYNCHRONIZATION(iBdID);

	if (wStartOrEnd == 2)
	{
		for(i=0; i<wStageNoOfAxis; i++)
		{  
			do
			{   
				FAS_GetAxisStatus(iBdID, wStageAxis[i], &AxisStatus);  	
				if (!(AxisStatus & FFLAG_MOTIONING)) MotionCount ++ ;
			} while(MotionCount < 3) ;   // Motion Stop 2번 Check

			MotionCount = 0 ;
		} 
	}

	return FMM_OK;
}

//Pump축
int FAS_MoveSpdMultiAxisAbsPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdAbsPos, double *pdVelocity, WORD wEndCheck)
{
    int i;
    int iTmpAxis = 0;
	long AxisStatus ;
    int  nRtn, MotionCount = 0 ;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	for (i=0; i<wNoOfAxis; i++)
	{
		if ((awNoAxis[iBdID] <= piAxis[i]) || (piAxis[i] < 0))
			return FMM_INVALID_AXIS_NUM;
		nRtn = CheckParameterRange(FALSE, 15, (long)pdAbsPos[i]);
		if (nRtn != FMM_OK) return nRtn;
		nRtn = CheckParameterRange(FALSE, 3, (long)pdVelocity[i]);
		if (nRtn != FMM_OK) return nRtn;
	}
    
    ENTER_SYNCHRONIZATION(iBdID);

    for (i=0; i<wNoOfAxis; i++)
    {
        FAS_WriteRam(iBdID, piAxis[i], ukG00Speed, (long)pdVelocity[i]);
        FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition,  (long)pdAbsPos[i]);

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
    }

    SetAxisInfoMulti(iBdID, iTmpAxis);

    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck, wEndCheck);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode, ukExactStopMode);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukMG00);           //ukMG00
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc, ukAbs);

    SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
   
	LEAVE_SYNCHRONIZATION(iBdID);
    
    if (wEndCheck != 0)
    {
       for(i=0; i<wNoOfAxis; i++)
       {  
		   do
		   {   
			  FAS_GetAxisStatus(iBdID, piAxis[i], &AxisStatus);  	
			  if (!(AxisStatus & FFLAG_MOTIONING))
				 MotionCount++;
           } while(MotionCount < 10);
		   
		   MotionCount = 0;
       } 
    }
    return FMM_OK;
}

/* --------------------------------------------------------------------------------------------- */
//Pump축
int FAS_MoveSpdMultiAxisIncPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, WORD wEndCheck)
{
    int i;
    int iTmpAxis = 0;
    long AxisStatus ;
    int  nRtn, MotionCount = 0 ;

#if EQ
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	for (i=0; i<wNoOfAxis; i++)
	{
		if ((awNoAxis[iBdID] <= piAxis[i]) || (piAxis[i] < 0))
			return FMM_INVALID_AXIS_NUM;
		nRtn = CheckParameterRange(FALSE, 15, (long)pdIncPos[i]);
		if (nRtn != FMM_OK) return nRtn;
		nRtn = CheckParameterRange(FALSE, 3, (long)pdVelocity[i]);
		if (nRtn != FMM_OK) return nRtn;
	}
#endif   
    ENTER_SYNCHRONIZATION(iBdID);

    for(i=0; i<wNoOfAxis; i++)
    {
        FAS_WriteRam(iBdID, piAxis[i], ukG00Speed, (long)pdVelocity[i]);
        FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition, (long)pdIncPos[i]);

      //by shin//2014.01.28//
		if(g_nDropType == 1)
		{
			FAS_WriteRam(iBdID, piAxis[i], ukTJDAxisOn, 1);
			FAS_WriteRam(iBdID, piAxis[i], ukTJDStartDelay, 0);
			FAS_WriteRam(iBdID, piAxis[i], ukTJDStopDelay, 0);
			FAS_WriteRam(iBdID, piAxis[i], ukTJDInposValue, 0);
		}
		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
    }

    SetAxisInfoMulti(iBdID, iTmpAxis);

    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck, wEndCheck);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode, ukExactStopMode);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukMG00);    // ukMG00
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc, ukInc);

    SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
   
	if (wEndCheck != 0)
    {
       for(i=0; i<wNoOfAxis; i++)
       {  
		   do
		   {   
			  FAS_GetAxisStatus(iBdID, piAxis[i], &AxisStatus);  	
			  if (!(AxisStatus & FFLAG_MOTIONING))
				 MotionCount ++ ;
           } while(MotionCount < 10);
		   
		   MotionCount = 0 ;
       } 
    }
    return FMM_OK;
}

/* --------------------------------------------------------------------------------------------- */
//Pump축
int FAS_ResetSpdQueue(int iBdID)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
    
    
	SetDpramCommand(iBdID, AXIS_GLOBAL,  ukResetMG00Q, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukResetMG00Q, 0);
    
	LEAVE_SYNCHRONIZATION(iBdID);

    return FMM_OK;
}

/* --------------------------------------------------------------------------------------------- */
//Pump축
int FAS_GetFreeSpdQueue(int iBdID,  WORD *wFreeAxisQueue)
{
	long lTmpQ;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	

	SetDpramCommand(iBdID, AXIS_GLOBAL,  ukResetMG00Q, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukResetMG00Q, 0);
	   
	lTmpQ = FAS_ReadRam(iBdID, AXIS_GLOBAL, ukFreePosCmdQ);
    LEAVE_SYNCHRONIZATION(iBdID);

	*wFreeAxisQueue = (WORD)(lTmpQ & 0x0FFFF);

    return FMM_OK;
}

// Reference IO Monitoring
int FAS_EnableReferenceIOMonitoring(int iBdID, WORD wAxis)
{
	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	
	SetAxisInfo(iBdID, wAxis);

	SetDpramCommand(iBdID,  wAxis, ukIODelayCheckEnable, 0);
	WaitDpramCommand(iBdID, wAxis, ukIODelayCheckEnable, 0);
	   	
    LEAVE_SYNCHRONIZATION(iBdID);

	return FMM_OK;
}

int FAS_GetReferenceIODistance(int iBdID, WORD wAxis, long *lCheckPos)
{
	long lCheckPosData;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

    ENTER_SYNCHRONIZATION(iBdID);
	
	SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukGetIODelayDistance, ukIODelayDistance);
    WaitDpramCommand(iBdID, wAxis, ukGetIODelayDistance, 0);

	lCheckPosData = FAS_ReadRam(iBdID, wAxis, ukIODelayDistance);
	LEAVE_SYNCHRONIZATION(iBdID);
   
	*lCheckPos = lCheckPosData;

	return FMM_OK;
}

int FAS_MoveSingleTJDAxisAbsPos(int iBdID, WORD wAxis, double dAbsPos, double dVelocity, int startDelay, int stopDelay, int inposValue, WORD wEndCheck)
{
   	long AxisStatus ;
    int  nRtn, MotionCount = 0 ;
	long lValue;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;
    
	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

	nRtn = CheckParameterRange(FALSE, 15, (long)dAbsPos);
	if (nRtn != FMM_OK) return nRtn;
	nRtn = CheckParameterRange(FALSE, 3, (long)dVelocity);
	if (nRtn != FMM_OK) return nRtn;

    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd39 In\n", iBdID+1);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck, wEndCheck);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode, ukExactStopMode);
	FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukG00);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc, ukAbs);
    
	FAS_WriteRam(iBdID, wAxis, ukG00Speed, (long)dVelocity); 
	FAS_WriteRam(iBdID, wAxis, ukGoalPosition, (long)dAbsPos); 
	FAS_WriteRam(iBdID, wAxis, ukTJDAxisOn, 1);
	FAS_WriteRam(iBdID, wAxis, ukTJDStartDelay, (long)startDelay);
	FAS_WriteRam(iBdID, wAxis, ukTJDStopDelay, (long)stopDelay);
	FAS_WriteRam(iBdID, wAxis, ukTJDInposValue, (long)inposValue);
	
    SetAxisInfo(iBdID, wAxis);
	
    SetDpramCommand(iBdID, wAxis, ukSetGoalPos, 0);
	lValue = FAS_ReadRam(iBdID, wAxis, ukG00Speed);
    WaitDpramCommand(iBdID, wAxis, ukSetGoalPos, 0); 
	lValue = FAS_ReadRam(iBdID, wAxis, ukG00Speed);
	LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd39 Out\n", iBdID+1);

	if (wEndCheck != 0)
    {
		do
        {
		   FAS_GetAxisStatus(iBdID, wAxis, &AxisStatus);  	
		   if (!(AxisStatus & FFLAG_MOTIONING))
			   MotionCount ++ ;
        } while(MotionCount < 10);
    }
	return FMM_OK;
}

int FAS_MoveSingleTJDAxisIncPos(int iBdID, WORD wAxis, double dIncPos, double dVelocity, int startDelay, int stopDelay, int inposValue, WORD wEndCheck)
{
    long AxisStatus ;
    int  nRtn, MotionCount = 0 ;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	if ((wAxis >= awNoAxis[iBdID]) || (wAxis < 0)) 
		return FMM_INVALID_AXIS_NUM;

	nRtn = CheckParameterRange(FALSE, 15, (long)dIncPos);
	if (nRtn != FMM_OK) return nRtn;
	nRtn = CheckParameterRange(FALSE, 3, (long)dVelocity);
	if (nRtn != FMM_OK) return nRtn;

	ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd40 In\n", iBdID+1);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck, wEndCheck);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode, ukExactStopMode);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukG00);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc, ukInc);
    FAS_WriteRam(iBdID, wAxis, ukG00Speed, (long)dVelocity);
    FAS_WriteRam(iBdID, wAxis, ukGoalPosition, (long)dIncPos);
	FAS_WriteRam(iBdID, wAxis, ukTJDAxisOn, 1);
	FAS_WriteRam(iBdID, wAxis, ukTJDStartDelay, (long)startDelay);
	FAS_WriteRam(iBdID, wAxis, ukTJDStopDelay, (long)stopDelay);
	FAS_WriteRam(iBdID, wAxis, ukTJDInposValue, (long)inposValue);

    SetAxisInfo(iBdID, wAxis);

    SetDpramCommand(iBdID, wAxis, ukSetGoalPos, 0);
    WaitDpramCommand(iBdID, wAxis, ukSetGoalPos, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd40 Out\n", iBdID+1);
    
	if (wEndCheck != 0)
    {
		do
        {
		   FAS_GetAxisStatus(iBdID, wAxis, &AxisStatus);  	
		   if (!(AxisStatus & FFLAG_MOTIONING))
			   MotionCount ++ ;
        } while(MotionCount < 10);
    }
    return FMM_OK;
}

int FAS_MoveMultiTJDAxisAbsPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdAbsPos, double *pdVelocity, int *startDelay, int *stopDelay, int *inposValue, WORD wEndCheck)
{
   int i;
    int iTmpAxis = 0;
	long AxisStatus ;
    int  nRtn, MotionCount = 0 ;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	for (i=0; i<wNoOfAxis; i++)
	{
		if ((awNoAxis[iBdID] <= piAxis[i]) || (piAxis[i] < 0))
			return FMM_INVALID_AXIS_NUM;
		nRtn = CheckParameterRange(FALSE, 15, (long)pdAbsPos[i]);
		if (nRtn != FMM_OK) return nRtn;
		nRtn = CheckParameterRange(FALSE, 3, (long)pdVelocity[i]);
		if (nRtn != FMM_OK) return nRtn;
	}
    
    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd41 In\n", iBdID+1);
    for (i=0; i<wNoOfAxis; i++)
    {
        FAS_WriteRam(iBdID, piAxis[i], ukG00Speed, (long)pdVelocity[i]);
        FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition,  (long)pdAbsPos[i]);
		
		FAS_WriteRam(iBdID, piAxis[i], ukTJDAxisOn, 1);
		FAS_WriteRam(iBdID, piAxis[i], ukTJDStartDelay, (long)startDelay[i]);
		FAS_WriteRam(iBdID, piAxis[i], ukTJDStopDelay, (long)stopDelay[i]);
		FAS_WriteRam(iBdID, piAxis[i], ukTJDInposValue, (long)inposValue[i]);

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
    }

    SetAxisInfoMulti(iBdID, iTmpAxis);

    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck, wEndCheck);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode, ukExactStopMode);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukG00);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc, ukAbs);
	
    SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd41 Out\n", iBdID+1);
    
    if (wEndCheck != 0)
    {
       for(i=0; i<wNoOfAxis; i++)
       {  
		   do
		   {   
			  FAS_GetAxisStatus(iBdID, piAxis[i], &AxisStatus);  	
			  if (!(AxisStatus & FFLAG_MOTIONING))
				 MotionCount++;
           } while(MotionCount < 10);
		   
		   MotionCount = 0;
       } 
    }
    return FMM_OK;
}

int FAS_MoveMultiTJDAxisIncPos(int iBdID, WORD wNoOfAxis, int *piAxis, double *pdIncPos, double *pdVelocity, int *startDelay, int *stopDelay, int *inposValue, WORD wEndCheck)
{
  int i;
    int iTmpAxis = 0;
    long AxisStatus ;
    int  nRtn, MotionCount = 0 ;

	if (!gAcc.IsOpen()) return FMM_NOT_OPEN;
	if (!gAcc.IsExist(iBdID)) return FMM_INVALID_BD_NUM;

	for (i=0; i<wNoOfAxis; i++)
	{
		if ((awNoAxis[iBdID] <= piAxis[i]) || (piAxis[i] < 0))
			return FMM_INVALID_AXIS_NUM;
		nRtn = CheckParameterRange(FALSE, 15, (long)pdIncPos[i]);
		if (nRtn != FMM_OK) return nRtn;
		nRtn = CheckParameterRange(FALSE, 3, (long)pdVelocity[i]);
		if (nRtn != FMM_OK) return nRtn;
	}
    
    ENTER_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd42 In\n", iBdID+1);

    for(i=0; i<wNoOfAxis; i++)
    {
        FAS_WriteRam(iBdID, piAxis[i], ukG00Speed, (long)pdVelocity[i]);
        FAS_WriteRam(iBdID, piAxis[i], ukGoalPosition, (long)pdIncPos[i]);
		
		FAS_WriteRam(iBdID, piAxis[i], ukTJDAxisOn, 1);
		FAS_WriteRam(iBdID, piAxis[i], ukTJDStartDelay, (long)startDelay[i]);
		FAS_WriteRam(iBdID, piAxis[i], ukTJDStopDelay, (long)stopDelay[i]);
		FAS_WriteRam(iBdID, piAxis[i], ukTJDInposValue, (long)inposValue[i]);

		if (piAxis[i] < MAX_AXIS_NUMS) iTmpAxis |= (0x01 << piAxis[i]);
    }

    SetAxisInfoMulti(iBdID, iTmpAxis);

    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukEndCheck, wEndCheck);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterMode, ukExactStopMode);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukInterpolation, ukG00);
    FAS_WriteRam(iBdID, AXIS_GLOBAL, ukAbsOrInc, ukInc);

    SetDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    WaitDpramCommand(iBdID, AXIS_GLOBAL, ukSetGoalPos, 0);
    LEAVE_SYNCHRONIZATION(iBdID);
	//TRACE("Bd%d Cmd42 Out\n", iBdID+1);
   
	if (wEndCheck != 0)
    {
       for(i=0; i<wNoOfAxis; i++)
       {  
		   do
		   {   
			  FAS_GetAxisStatus(iBdID, piAxis[i], &AxisStatus);  	
			  if (!(AxisStatus & FFLAG_MOTIONING))
				 MotionCount ++ ;
           } while(MotionCount < 10);
		   
		   MotionCount = 0 ;
       } 
    }
    return FMM_OK;
}

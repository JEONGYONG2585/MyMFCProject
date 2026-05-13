#ifndef __FM_COUNTERX_H__
#define	__FM_COUNTERX_H__

#ifdef FM_COUNTER_EXPORTS
	#define FM_COUNTER_API __declspec(dllexport)
#else
	#define FM_COUNTER_API __declspec(dllimport)
	#pragma comment( lib, "FM_COUNTER.lib" )
#endif

#define     FCAPI   extern "C" FM_COUNTER_API

//------------------------------------------------------------------
//                        Structures.
//------------------------------------------------------------------

typedef enum _FMMC_ERROR
{
	FMMC_OK = 0,
	
	FMMC_OPEN_FAIL,				// 1
	FMMC_CLOSE_FAIL,				// 2
	FMMC_NOT_OPEN,				// 3
	FMMC_INVALID_BD_NUM,			// 4
	FMMC_INVALID_AXIS_NUM,		// 5
	FMMC_INVALID_PARAMETER_NUM,	// 6
	FMMC_PARAMETER_RANGE_ERROR,	// 7

	FMMC_ZERO_RETURN_ERROR,		// 8

	FMMC_TIMEOUT_ERROR,			// 9

	FMMC_UNKNOWN_ERROR,			// 10

	FMMC_INVALID_CHANNEL_NUM
} FMMC_ERROR;

//------------------------------------------------------------------
//                        Initialized Functions.
//------------------------------------------------------------------
FCAPI BOOL WINAPI FASC_Open();
FCAPI BOOL WINAPI FASC_Close();
FCAPI BOOL WINAPI FASC_IsExist(int iBdID);
FCAPI int  WINAPI FASC_IsAlive(int iBdID, BOOL *bStatus);

////////////////////////////////////////////////////////////////////////////////////////////
//  Count Board 추가 부분
//------------------------------------------------------------------
//                Count Functions.
//------------------------------------------------------------------
FCAPI int WINAPI FASC_ResetAllCount(int iBdID);
FCAPI int WINAPI FASC_ResetMultiCount(int iBdID, WORD wNoOfChannel, WORD* pwChannel);
FCAPI int WINAPI FASC_GetAllCount(int iBdID, long aCount[18]);
FCAPI int WINAPI FASC_GetMultiCount(int iBdID, WORD wNoOfChannel, WORD* pwChannel, long aCount[18]);

#endif	//__FM_8000X_H__


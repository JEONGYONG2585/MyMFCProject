////////////////////////////////////////////////////////////////////////////
//
//      (C) 2002  FAS Technology Co., Ltd.
//		FILENAME:		pci_io.h
//		DESCRIPTION:	NT DRIVER'S PCI_IO 
//
/////////////////////////////////////////////////////////////////////////////

#ifndef PCI_IO_H_INCLUDED
#define PCI_IO_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//typedef int             BOOL;
//typedef unsigned char   BYTE;
//typedef unsigned short  WORD;
//typedef unsigned long   DWORD;

#define  MAX_CARD_NUMS				8
#define  MAX_BASE_ADDR_NUMS			6
#define	 MAX_AXIS_NUMS				16

#define  MAX_EVENT_NUMS				10

#define  REGPATH_PROGRAMUSE			("SOFTWARE\\FASTECH\\FAS16000MO\\USE")
#define  REGPATH_PROGRAM			("SOFTWARE\\FASTECH\\FAS16000MO")
#define  REGPATH_USE				("USE")

#define	 MAX_IO_BUF_SIZE			1024 	// maximum 1024 words 
#define  MIN_FM_16000_MESSAGE		0x80    
#define  MAX_FM_16000_MESSAGE		0xff    

#pragma pack(1)

typedef struct tagINFO_PCI
{
   ULONG	irq;
   ULONG	CardNum;
   ULONG	cs[MAX_BASE_ADDR_NUMS];
   ULONG	cs_len[MAX_BASE_ADDR_NUMS];
   ULONG	io[MAX_BASE_ADDR_NUMS];
   ULONG	io_len[MAX_BASE_ADDR_NUMS];

} INFO_PCI,*PINFO_PCI;

typedef struct _IOCTL_DATA 
{
	ULONG	Addr;
	LONG	Data;
	LONG	Data2;
} IOCTL_DATA, *PIOCTL_DATA;

typedef struct _IOCTL_BLOCK_B 
{
	   ULONG    Addr; 
	   ULONG	Length;				
	   BYTE 	Buf[MAX_IO_BUF_SIZE];			
	   ULONG    Return;  // read or write length 
} IOCTL_BLOCK_B, *PIOCTL_BLOCK_B;

typedef struct _IOCTL_BLOCK_W 
{
	   ULONG	Addr; 
	   ULONG	Length;				
	   USHORT 	Buf[MAX_IO_BUF_SIZE];			
	   ULONG    ulSum ;  
	   ULONG    ulSum2;  
//	   DOUBLE   dAve  ;  // AD Value
//	   DOUBLE   dAve2 ;  // AD Value
	   ULONG    Return;  // read or write length 
} IOCTL_BLOCK_W, *PIOCTL_BLOCK_W;

typedef struct _IOCTL_BLOCK 
{
	   ULONG    Addr; 
	   ULONG	Length;				
	   ULONG  	Buf[MAX_IO_BUF_SIZE];			
	   ULONG    Return;  // read or write length 
} IOCTL_BLOCK, *PIOCTL_BLOCK;

#pragma pack()

/////////////////////////
#define INT_END_FLAG				0xffff4444
#define INT_PENDING					0x80000000


#define PCI_NT_DEVICE_NAME			L"\\Device\\fm_16000"
#define PCI_WIN32_DEVICE_NAME		L"\\DosDevices\\fm_16000"


#define DEVICE_USER_TYPE			0xd000

#define	PCI_WDM_DEVICE_NAME0		"\\\\.\\fm_160000"
#define	PCI_WDM_DEVICE_NAME1		"\\\\.\\fm_160001"

//////////////////////////////////////////////////////////////////////
//    PCI DEVICE REGISTER MAP 
//////////////////////////////////////////////////////////////////////

enum {
	  PCI_BASE_REG  = 0,   // local reg register access
	  PCI_BASE_CS0,        // int ack
	  PCI_BASE_CS1,        // dipsw in
	  PCI_BASE_CS2,        // 7 segement  
	  PCI_BASE_CS3         // sram access	
};

#define  PCI_BASE_IO			0    // i/o  access	

#define  FM_16000_ACK_ADDRESS		(0x01FFE << 2) 	// cs3
#define  FM_16000_INT_ADDRESS		(0x01FFF << 2) 	// cs3
#define  FM_16000_DPRAM_BASE		(0x0)			// cs3
#define  FM_16000_SRAM_BASE			(0x0)			// cs3

#define  FM_16000_FLAG_ADDRESS		(0x0087F << 2)	// cs3		1FF8 -> 081D
#define  FM_16000_AXIS_ADDRESS		(0x0087E << 2) 	// cs3

enum {
	PCI_GET_INFO = 0x800,

	PCI_WRITE_B,
	PCI_WRITE_W,
	PCI_WRITE,

	PCI_READ_B,
	PCI_READ_W,
	PCI_READ,

    PCI_WRITE_PORT_B,
    PCI_WRITE_PORT_W,
    PCI_WRITE_PORT,

    PCI_READ_PORT_B,
    PCI_READ_PORT_W,
    PCI_READ_PORT,

	PCI_WRITE_BLOCK_B,
	PCI_WRITE_BLOCK_B_DW,
	PCI_WRITE_BLOCK_W,
	PCI_WRITE_BLOCK,

	PCI_READ_BLOCK_B,
	PCI_READ_BLOCK_B_DW,
	PCI_READ_BLOCK_W,
	PCI_READ_BLOCK,

	PCI_READ_INT,
	PCI_READ_INT_END,
	
      
	PCI_GET_INT,
	PCI_SET_INT,
	PCI_CLR_INT,
	PCI_AD_READ,
	PCI_AD_READ_BLOCK,
	
	PCI_DEREF_EVENT_REPLY,
	PCI_DEREF_EVENT_MESSAGE,
	PCI_REF_EVENT_REPLY,
	PCI_REF_EVENT_MESSAGE,
	PCI_GET_REPLY,
	PCI_GET_MSG,

	PCI_READ_BLOCK_DP_B,
	PCI_WRITE_BLOCK_DP_B,
	PCI_READ_BLOCK_DP,
	PCI_WRITE_BLOCK_DP,

	PCI_WRITE_DP_W,
	PCI_WRITE_DP,
	PCI_READ_DP_W,
	PCI_READ_DP

};

#define PCI_IOCTL_GET_INFO        CTL_CODE( DEVICE_USER_TYPE, PCI_GET_INFO, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define PCI_IOCTL_WRITE_B         CTL_CODE( DEVICE_USER_TYPE, PCI_WRITE_B, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_WRITE_W         CTL_CODE( DEVICE_USER_TYPE, PCI_WRITE_W, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_WRITE           CTL_CODE( DEVICE_USER_TYPE, PCI_WRITE, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define PCI_IOCTL_READ_B          CTL_CODE( DEVICE_USER_TYPE, PCI_READ_B, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_READ_W          CTL_CODE( DEVICE_USER_TYPE, PCI_READ_W, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_READ            CTL_CODE( DEVICE_USER_TYPE, PCI_READ, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define PCI_IOCTL_WRITE_PORT_B    CTL_CODE( DEVICE_USER_TYPE, PCI_WRITE_PORT_B, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_WRITE_PORT_W    CTL_CODE( DEVICE_USER_TYPE, PCI_WRITE_PORT_W, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_WRITE_PORT      CTL_CODE( DEVICE_USER_TYPE, PCI_WRITE_PORT, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define PCI_IOCTL_READ_PORT_B     CTL_CODE( DEVICE_USER_TYPE, PCI_READ_PORT_B, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_READ_PORT_W     CTL_CODE( DEVICE_USER_TYPE, PCI_READ_PORT_W, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_READ_PORT       CTL_CODE( DEVICE_USER_TYPE, PCI_READ_PORT, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define PCI_IOCTL_READ_BLOCK_B    	CTL_CODE( DEVICE_USER_TYPE,PCI_READ_BLOCK_B, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_READ_BLOCK_B_DW  	CTL_CODE( DEVICE_USER_TYPE,PCI_READ_BLOCK_B_DW, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_READ_BLOCK_W    	CTL_CODE( DEVICE_USER_TYPE,PCI_READ_BLOCK_W, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_READ_BLOCK    	CTL_CODE( DEVICE_USER_TYPE,PCI_READ_BLOCK,	  METHOD_BUFFERED, FILE_ANY_ACCESS)

#define PCI_IOCTL_WRITE_BLOCK_B		CTL_CODE( DEVICE_USER_TYPE,PCI_WRITE_BLOCK_B, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_WRITE_BLOCK_B_DW	CTL_CODE( DEVICE_USER_TYPE,PCI_WRITE_BLOCK_B_DW, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_WRITE_BLOCK_W		CTL_CODE( DEVICE_USER_TYPE,PCI_WRITE_BLOCK_W, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_WRITE_BLOCK		CTL_CODE( DEVICE_USER_TYPE,PCI_WRITE_BLOCK,   METHOD_BUFFERED, FILE_ANY_ACCESS)

#define PCI_IOCTL_READ_INT	        CTL_CODE( DEVICE_USER_TYPE,PCI_READ_INT,	 METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_READ_INT_END 		CTL_CODE( DEVICE_USER_TYPE,PCI_READ_INT_END, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define PCI_IOCTL_GET_INT           CTL_CODE( DEVICE_USER_TYPE, PCI_GET_INT, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_SET_INT           CTL_CODE( DEVICE_USER_TYPE, PCI_SET_INT, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_CLR_INT           CTL_CODE( DEVICE_USER_TYPE, PCI_CLR_INT, METHOD_BUFFERED, FILE_ANY_ACCESS)


#define PCI_IOCTL_AD_READ 			CTL_CODE( DEVICE_USER_TYPE, PCI_AD_READ,   METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_AD_READ_BLOCK		CTL_CODE( DEVICE_USER_TYPE, PCI_AD_READ_BLOCK,   METHOD_BUFFERED, FILE_ANY_ACCESS)

#define PCI_IOCTL_DEREF_EVENT_REPLY		CTL_CODE( DEVICE_USER_TYPE, PCI_DEREF_EVENT_REPLY,   METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_DEREF_EVENT_MESSAGE	CTL_CODE( DEVICE_USER_TYPE, PCI_DEREF_EVENT_MESSAGE,   METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_REF_EVENT_REPLY		CTL_CODE( DEVICE_USER_TYPE, PCI_REF_EVENT_REPLY,   METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_REF_EVENT_MESSAGE		CTL_CODE( DEVICE_USER_TYPE, PCI_REF_EVENT_MESSAGE,   METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_GET_REPLY				CTL_CODE( DEVICE_USER_TYPE, PCI_GET_REPLY,   METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_GET_MSG				CTL_CODE( DEVICE_USER_TYPE, PCI_GET_MSG,   METHOD_BUFFERED, FILE_ANY_ACCESS)

#define PCI_IOCTL_READ_BLOCK_DP_B	CTL_CODE( DEVICE_USER_TYPE, PCI_READ_BLOCK_DP_B,   METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_WRITE_BLOCK_DP_B	CTL_CODE( DEVICE_USER_TYPE, PCI_WRITE_BLOCK_DP_B,   METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_READ_BLOCK_DP		CTL_CODE( DEVICE_USER_TYPE, PCI_READ_BLOCK_DP,   METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_WRITE_BLOCK_DP	CTL_CODE( DEVICE_USER_TYPE, PCI_WRITE_BLOCK_DP,   METHOD_BUFFERED, FILE_ANY_ACCESS)

#define PCI_IOCTL_WRITE_DP_W		CTL_CODE( DEVICE_USER_TYPE, PCI_WRITE_DP_W,   METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_WRITE_DP			CTL_CODE( DEVICE_USER_TYPE, PCI_WRITE_DP,   METHOD_BUFFERED, FILE_ANY_ACCESS)

#define PCI_IOCTL_READ_DP_W		CTL_CODE( DEVICE_USER_TYPE, PCI_READ_DP_W,   METHOD_BUFFERED, FILE_ANY_ACCESS)
#define PCI_IOCTL_READ_DP		CTL_CODE( DEVICE_USER_TYPE, PCI_READ_DP,   METHOD_BUFFERED, FILE_ANY_ACCESS)


/****************************************************************************/
/*  DPRAM Command from Main                                                 */
/****************************************************************************/
/*------------  Axis Command  ----------- */
#define ukNoAction				0x00		/* No Action                         */
#define ukSetGoalPos			0x01		/* Get Distance from Main            */
#define ukGetServoStatus		0x02		/* Transfer Servo Status to Main     */
#define ukServoEnable			0x03		/* Servo On                          */
#define ukClearServoError		0x04		/* Clear Servo Error                 */
#define ukStopMotionAxis		0x05		/* Stop Motion Axis                  */
#define ukStopMotionAll			0x06		/* Stop Motion All                   */
#define ukEmergencyStop			0x07		/* Emergency Stop                    */
#define ukSetLPara				0x08		/* Get (Servo)Parameter from Main    */
#define ukGetLPara				0x09		/* Transfer (Servo)Parameter to Main */
#define ukSetGPara				0x0a		/* Get Global Parameter from Main    */
#define ukGetGPara				0x0b		/* Transfer Global Parameter to Main */
#define ukSavePara				0x0c		/* Save Parameter to EEPROM          */
#define ukDoOriginSingleAxis	0x0d		/* Do Zero Returning                 */
#define ukDoOriginMultiAxis		0x0e		/* Do Origin Simultaneously          */
#define ukJog					0x0f		/* Jog Command                       */
#define ukSetAdIndex			0x10		/* Set Ad Index                      */
#define ukGetAdIndex			0x11		/* Transfer Ad Index                 */
#define ukSetDaIndex			0x12		/* Out D/A                           */
#define ukGetDaIndex			0x13		/* Transfer Ad Index                 */

/* ------------ Global Command ---------- */
#define ukSetCurrentPos			0x14		/* Update Current & Goal Pos. as new value  */
#define ukVelocityMove			0x15      /* Velocity Move Continuously        */
#define ukMoveStop				0x16		/* Move Stop                         */
#define ukMotionPause			0x17		/* Motion Pause                      */
#define ukMotionRestart			0x18      /* Motion Restart                    */
#define ukResetPosCmdQ			0x19		/* Delete Position Command Queue     */
#define ukGetNoFreeQ			0x1a		/* Transfer Pos Command Q            */
#define ukIoWriteLow			0x1b		/* Set I/O (Low)                     */
#define ukIoReadLow				0x1c		/* Get I/0 (Low)                     */
#define ukPositionIo			0x1d		/* I/O Set According to position     */
#define ukWaitUntilIoSet		0x1e	    /* Wait Until I/O Set                */
#define ukDewell				0x1f	    /* Dwell                             */
#define ukReadCurrentPos		0x20		/* Read Current Pos                  */
#define ukReadCurrentVel		0x21		/* Read Current Vel                  */
#define ukReadCurrentAcc		0x22		/* Read Current Acc                  */
#define ukReadCurrentError		0x23		/* Read Curren Pos Error             */
#define ukStartContPoint		0x24
#define ukEndContPoint			0x25
#define ukStartContMotion		0x26
#define ukEndContMotion			0x27
#define ukSoftReset				0x28
#define ukClearServoCounter		0x29
#define ukMoveToLimit			0x2a
#define ukEscapeFromLimit		0x2b
#define ukMoveToZPulse			0x2c
#define ukMoveToOnePulse		0x2d
#define ukMoveOrgPoint			0x2e
#define ukWaitAxisMoveDone		0x2f
#define ukWaitAllMoveDone		0x30
#define ukClearStop				0x31
#define ukClearErrorStatus		0x32
#define ukWaitSingleMoveDone	0x33
#define ukWaitMultiMoveDone		0x34
#define ukSetDaOffset			0x35
#define ukServoAlarmReset		0x36
#define ukVelocityOverride		0x37
#define ukPositionOverride		0x38
#define ukMoveSync				0x39
#define ukSetIoBitLow			0x3a
#define ukGetIoBitLow			0x3b
#define ukGetOutputStatusLow	0x3c
#define ukSetCommandPos			0x3d 
#define ukGetCommandPos			0x3e
#define ukGetTrackingError		0x3f
#define ukResetPosIoCmdQ		0x40
#define ukGetPosIoCmdQ			0x41
#define ukMoveOrgZpulse			0x42
#define ukEraseFlash			0x43
#define ukWriteFlash			0x44
#define ukCheckSum				0x45
#define ukSaveLocalPara			0x46
#define ukSaveGlobalPara		0x47
#define ukGetRomLPara			0x48
#define ukGetRomGPara			0x49

#define ukResetArcQ				0x4a
#define ukGetNoFreeQArch		0x4b
#define ukResetG01Q				0x4c
#define ukGetNoFreeQG01			0x4d
#define ukResetG02Q				0x4e
#define ukGetNoFreeQG02			0x4f
#define ukResetG00Q				0x50
#define ukGetNoFreeQG00			0x51
#define ukResetMotionArcQ		0X52
#define ukGetNoFreeMotionArcQ	0x53
#define ukGetAdResult			0x54
#define ukSetIoTrigger			0x55
#define ukSetMultiIoTrigger		0x56
#define ukNoOfCommand			0x57

//////////////////////////////////////////////////////////////////////  2003.12.27
#define ukGapControlEnable			0x58
#define ukGetGapError				0x59
#define ukClearGapError				0x5a
#define ukGapControlEnableMulti		0x5b 
#define ukGapControlDisableMulti	0x5c 
//////////////////////////////////////////////////////////////////////  2004.02.01
#define ukResetDropQ				0x5d
#define ukGetNoFreeQDrop			0x5e

#define ukGetGapErrorCount			0x5f

#define ukResetContG01Q				0x60
#define ukGetNoFreeQContG01			0x61

#define ukIoWriteHigh				0x62		/* Set I/O (High)                     */
#define ukIoReadHigh				0x63		/* Get I/0 (High)                     */
#define ukSetIoBitHigh				0x64
#define ukGetIoBitHigh				0x65
#define ukGetOutputStatusHigh		0x66

// Add 2004/1003
#define ukSetSyncSlaveAxes			0x67
#define ukSetSyncMasterAxis			0x68

//////////////////////////////////////////////////////////////////////  2004.10.11
#define ukReadFlash					0x69

// Add 2004/1015
#define ukCornerCompEnable			0x6a
#define ukCornerCompEnableMulti		0x6b
#define ukCornerCompDisableMulti	0x6c
// Add 2004/10/27 (SONG)
#define ukResetSyncMasterAxis       0x6d

// Add 2005/03/28
#define ukGetLaserSensor			0x6e

#define   ukGetMonitoring           0x6f        // 2006. 12. 20

// 2007.  10.10
#define   ukGetMonitoringDropCount  0x70        // 2007.  10.10
#define   ukEnableDropCount			0x71 

// 2008.07.04	SPD Multi-Drop(QUEUE)
#define  ukResetMG00Q               0x72
#define  ukGetNoFreeQMG00           0x73

#define	  ukIODelayCheckEnable		0x74		//20120512 Gantry축 Reference Mark 확인용
#define	  ukGetIODelayDistance		0x75		//20120512 Gantry축 Reference Mark 인식 거리차(pulse) return

#define   ukGetDropData             0x76		//LC 기존 74였음

#define	  ukTJDControlEnable		0x77		//TJD: Start & Stop IO 동작 + SPD P축

#define ukCheckMotionEnd			0xf0

/*****************************************************************************/
/*   DPRAM  Offset Address (Byte Unit)                                       */
/*****************************************************************************/
/* ---------   Axis   ------------------ */  

#define   ukG00Speed            0x00        /* G00 Speed                               */
#define   ukGoalPosition		0x01    	/* Abs. Goal Position                      */
#define   ukCirStartPos     	0x02		/* Abs. Cir Start Position                 */
#define   ukCirEndPos           0x03        /* Abs. Cir End Position                   */
#define   ukCirCenter			0x04		/* Abs. Cir Center Position                */
#define   ukFeedPosition        0x05        /* Abs. Feedback Position                  */
#define   ukErrPosition			0x06		/* Postion Error                           */
#define   ukJogDir              0x07
#define   ukLParaNumber         0x08        /* Parameter Number                        */
#define   ukLParaValue          0x09        /* Parameter Value                         */
#define   ukServoStatus	       	0x0a		/* Servo Status                            */
#define   ukGeneral             0x0b
#define   ukAxisVelOverRatio    0x0c              
#define   ukTrueOrFalse         0x0d
#define   ukDaOutput            0x0e
#define   ukCurrentPos          0x0f
#define   ukCurrentVel          0x10
#define   ukCurrentAccel        0x11
#define   ukTrackingError       0x12
#define	  ukGraphAdResult		0x13
#define   ukGraphPosCom         0x14
#define   ukGraphPosFeed        0x15
#define   ukGraphVelFeed        0x16
#define   ukGraphPosError       0x17
#define   ukGraphStatus         0x18

#define   ukLaserSensor			0x19		/* Laser Senser (2005/03/28)				*/

#define   ukGoalPosition2       0x1a        /* double up                                */


#define   ukMonIndex            0x1b    // 2006.12.20
#define   ukMonCommandPos       0x1c
#define   ukMonCurPos           0x1d
#define   ukMonGapSensor        0x1e
#define   ukMonCompPos          0x1f           /*  up to 0x7f  */     
#define   ukMonCommandPosX      0x20
#define   ukMonCommandPosY      0x21
#define   ukMonSpeedXY          0x22	
#define   ukStartGapChangeValue 0x23    /*  up to 0x7f */
#define	  ukMonLCGo				0x24
#define   ukSearchSpeed         0x25  // Dpram Local
#define   ukUpPos               0x26  // Dpram Local
#define	  ukMonRawGapSensor     0x27
//#define	  ukTargetGap			0x28
#define	  ukLineTargetGapPulse		0x28	// Incremental //lbg 100715
#define   ukTopConMultiOff      0x29    
#define   ukMusashiOffMultiPos  0x2a
#define   ukIODelayDistance     0x2b
#define   ukDropData            0x2c	// 기존 24에서 2c 변경(SEAL/LC 통합)
#define	  ukTJDStartDelay		0x2d
#define   ukTJDStopDelay		0x2e
#define   ukTJDInposValue		0x2f
#define	  ukTJDAxisOn			0x30	// TJD Drop Axis


/* --------------------   Global   --------------------- */      
/*                                                       */
#define   ukInterpolation       0x00           /* 0:G00, 1:G01, 2:G02 3:G03               */
#define   ukTriggerStartPos		0x01           /* I/O TriggerStartPos                     */
#define   ukFeedRate            0x02           /* Feedrate for G01,G02                    */
#define   ukDwellTime           0x03           /* Dwell Time                              */
#define   ukExactStop           0x04           /* 0:Exact Stop, 1: Path Through           */
#define   ukEndCheck            0x05           /* Path Through Value                      */
#define   ukCompleteCircle      0x06
#define   ukCirDirection        0x07           /* Circle Direction 0:CW, 1:CCW            */
#define   ukFreePosCmdQ			0x08			/* No. of free space in Pcmd_Q             */
#define   ukGParaNumber         0x09           /* Parameter Value                         */
#define   ukGParaValue          0x0a           /* Parameter Number                        */
#define   ukOutput              0x0b           /* I/O Output Value                        */
#define   ukInput               0x0c           /* I/O Input Value                         */ 
//#define   ukSmoothDistance      0x0d           /* G01 Position Smooth Value               */
#define   ukTriggerPitch		0x0e           /* G01 Velocity Smooth Value               */
//#define   ukContinuousInter     0x0f           /* 0:No Cont.    1:Cont. Interpolation     */
//#define   ukPTPOptimal          0x10           /* 0:No Optimal, 1:Optimal                 */
#define   ukAbsOrInc            0x11           /* 0:Inc, 1:Abs                            */
#define   ukOptimalSingle       0x12
#define   ukInterMode           0x13 
#define   ukCornerError         0x14
#define   ukStartEndPoint       0x15     
#define   ukIoValue             0x16   
#define   ukStatus              0x17
#define   ukTemp                0x18           // TEMP
#define   ukIoBitNo             0x19
#define   ukFeedbackOrCommand   0x1a
#define   ukMotionEndInfoFlag	0x1b 
#define   ukNoOfTrigger			0x1c
//#define   ukInterruptFlag       0x1d		// Move to End
#define   ukWriteFlashStart     0x1e
#define   ukWriteLength         0x1f
#define   ukCheckSumData        0x20
#define   ukDropStartPos        0x21
#define   ukDropAxisInfo        0x22
#define   ukDropOrNot           0x23   // 35
#define   ukDropStop            0x24   // 36
#define   ukDropStartSpeed      0x25   // 37
#define   ukCirFeedrate         0x26
#define   ukMusashiOffPos       0x27   // 39
#define   ukGraphMonitorFlag    0x28

// Add 2007.10.10
// LC
#define   ukMonIndexDrop        0x2b    
#define   ukMonCommandPosDropX  0x2c
#define   ukMonCommandPosDropY  0x2d
#define   ukMonIOLowDrop        0x2e
#define   ukMonIOHihgDrop       0x2f      
#define   ukDropCountDelay      0x30     
//


// Add 2004/1003
// SEAL
#define   ukSyncAxis			0x29
#define   ukDropStartPos2       0x2a        // double up   
#define	  ukPressureControlOn	0x2b
#define	  ukPressureControlType	0x2c
#define   ukCornerGapOnFlag		0x2d	//ssh 100710, 코너 가감속 구간 Gap Off 속도 //lbg 100715
#define   ukTopConOff           0x2e                 //1 인 라인에서 동작

#define   ukTopConOnPos			0x2f			// Disp On
#define	  ukTopCOnOn			0x30			// Disp On/Line, Io Value는 기존 Musashi Off 사용
#define   ukGapControlHighLow   0x31			// Gap Control Io High or Low
#define   ukGapControlAbsPos    0x32			// Gap Control Abs Position
#define   ukGapControlData      0x33		    // Gap Control Io Value
#define   ukGapControlOn		0x34			// Gap Cotrol Flag, 1인 라인에서 Gap 변경
//
// LC
#define   ukDropIo				0x35			// Drop Io, EZI+R Drop 구동용
#define   ukDropIoHighOrLow		0x36			// Drop Io High Or Low
#define   ukDropCompleteOn		0x37

#define   ukServoInfo			0x7E		/* Servo Infomation                        */
#define   ukInterruptFlag       0x7F		// Move to End

#define   ukIntReq				0x01FFE		/* INT. Request to main                    */              
#define   ukIntAck				0x01FFF		/* INT. Acknowledge                        */

/*****************************************************************************/
/*							Motion Related                                   */
/*****************************************************************************/

#define ukAbs				   1
#define ukInc				   0
#define ukExactStopMode		   0x00
#define ukOptimalMode		   0x01
#define ukLclMode			   0x02
#define ukContInterMode		   0x03
#define ukG00				   0x00
#define ukG01				   0x01
#define ukG02				   0x02
#define ukG03				   0x03
#define ukG01Optimal		   0x04
#define ukArch      		   0x05
#define ukG01Cont			   0x06
#define ukG02Cont			   0x07
#define ukLineDrop             0x08
#define ukArchSingle           0x09
#define ukG00Io                0x0a
#define ukG01Io                0x0b
#define ukIoTrigger			   0x0c
#define ukMG00				   0x0d

#define   ukMotionEndG00       0x70
#define   ukMotionEndG01       0x71
#define   ukMotionEndG02       0x72
#define   ukMotionEndG01Arch   0x73
#define   ukMotionEndG01Cont   0x74
#define   ukMotionEndG02Cont   0x75


/*****************************************************************************/
/*							Monitoring Related                               */
/*****************************************************************************/

#define	MAX_MONOTORING_ITEM		6
#define	MAX_MONOTORING_COUNT	100

#define	MA_INDEX				0
#define	MA_CMDPOS				1
#define	MA_ACTPOS				2
#define	MA_ACTVEL				3
#define	MA_POSERR				4
#define	MA_STATUS				5

#ifdef __cplusplus
}
#endif

#endif


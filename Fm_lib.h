////////////////////////////////////////////////////////////////////////////
//
//      (C) 2002  FAS Technology Co., Ltd.
//		FILENAME:		Fm_lib.h
//		DESCRIPTION:	FM_8000
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __FM_LIB_H__
#define __FM_LIB_H__

#include "pci_io.h"

//#define DBG_THREAD_MSG		1

#define	WM_INT_REQ			WM_USER + 0x3333

#ifndef WM_FM_MSG
	#define	WM_FM_MSG			WM_USER + 0x2000
#endif	//WM_FM_MSG
#ifndef WM_FM_REPLY
	#define	WM_FM_REPLY			WM_USER + 0x2001
#endif	//WM_FM_REPLY

#pragma pack(1)

typedef struct tagFAS_DEVICE
{
	//Card
	BOOL		bIsExist;
	ULONG		CardNum;
	ULONG		CardID;
	HANDLE		hSys;	
	INFO_PCI	InfoPci;

	//Event 
	HANDLE		evReply;
	HANDLE		evMsg;
} FAS_DEVICE, *PFAS_DEVICE;

typedef struct tagFAS_MOTION
{
	int			iMaxNum;
	HWND		hMsgWnd;
	BOOL		bIsMsgQueued;
	HANDLE		hMutex;

	int			nSlotPos;

	//Card Device
	FAS_DEVICE	sDev[MAX_CARD_NUMS];
} FAS_MOTION, *PFAS_MOTION;

#pragma pack()

extern  FAS_MOTION sFas;

//extern  int  gCard;

//extern IOCTL_DATA      IoctlData;
//extern PIOCTL_DATA     pIoctlData;

//extern IOCTL_BLOCK_B    IoctlBlockB;
//extern PIOCTL_BLOCK_B   pIoctlBlockB;


//#define RAM_SIZE			   0X400
//#define LOCAL_SIZE             21
//#define EEPROM_SIZE            25

const static short RAM_SIZE = 0x400;
const static short LOCAL_SIZE = 21;
const static short EEPROM_SIZE = 25;

//extern DWORD   LocalBuf[MAX_CARD_NUMS][LOCAL_SIZE];
//extern DWORD   EepromBuf[MAX_CARD_NUMS][EEPROM_SIZE];
//extern DWORD   EepromDefault[EEPROM_SIZE];

//=============================
//           LIBRARY
//=============================
BOOL    PciStart(int slotno); 
BOOL    PciEnd();

void    PciSetWnd(HWND hWnd);

void    PciIntSet(int iBdNo);
void    PciGetInt(int iBdNo,DWORD *count1,DWORD *count2);
void    PciIntClr(int iBdNo);

void    PciWrite(int iBdNo,DWORD address, LONG data);		// DWORD에서 LONG으로 변경
//void    PciWriteW(int iBdNo,DWORD address, WORD data);
//void    PciWriteB(int iBdNo,DWORD address, BYTE data);
LONG	PciRead(int iBdNo,DWORD address);					// DWORD에서 LONG으로 변경
//WORD    PciReadW(int iBdNo,DWORD address);
//BYTE    PciReadB(int iBdNo,DWORD address);
DWORD   PciReadReg(int iBdNo,DWORD offset);
void    PciWriteReg(int iBdNo,DWORD offset,DWORD data);
void    PciWritePort(int iBdNo,DWORD address, DWORD data);
//void    PciWritePortW(int iBdNo,DWORD address, WORD data);
//void    PciWritePortB(int iBdNo,DWORD address, BYTE data);
DWORD   PciReadPort(int iBdNo,DWORD address);
//WORD    PciReadPortW(int iBdNo,DWORD address);
//BYTE    PciReadPortB(int iBdNo,DWORD address);
void    PciDeviceCtrl(int iBdNo,DWORD cmd);
DWORD   PciEepRead(int iBdNod);
void    PciEepWrite(int iBdNo,DWORD data);
int     PciIntRead(int iBdNo,PIOCTL_DATA pIoData, LPOVERLAPPED ovl);
int     PciIntReadEnd(int iBdNo);

BOOL    PciLinkEvent(int iBdNo, HANDLE *phReply, HANDLE *phMsg);
BOOL    PciUnlinkEvent(int iBdNo);
BOOL    PciSetEvent(int iBdNo, int slotno);
BOOL    PciClearEvent(int iBdNo);

DWORD   PciGetReplyType(int iBdNo);
DWORD   PciGetMsgType(int iBdNo);
BYTE    PciGetCardID(int iBdNo);
//추가..
BYTE    PciGetCardIDFast(int iBdNo);
void    PciSetMsgQueued(BOOL bQueued);
BOOL    PciIsExist(int iBdNo);
void    PciReadBlock(int iBdNo, DWORD address, BYTE* pData,  ULONG ulLength );
void    PciReadBlockDP(int iBdNo, DWORD address, BYTE* pData,  ULONG ulLength );
void    PciWriteBlockDP(int iBdNo, DWORD address, BYTE* pData,  ULONG ulLength );

void	PciWriteDP(int iBdNo,DWORD address, LONG data);
void	PciWriteDPW(int iBdNo,DWORD address, WORD data);
LONG	PciReadDP(int iBdNo,DWORD address);
WORD	PciReadDPW(int iBdNo,DWORD address);

// 몇몇 추가 함수.. (삽입 2002/08/02)
int     PciGetAxisInfo(int iBdNo);

//=============================
//           EEPROM
//=============================
#define EE_ADDR_MASK	0x003f		// for combining addresses with instructions
#define EE_CMD_LEN		9			// bits in instructions
#define EE_READ			0x0180		// 01 1000 0000 read instruction
#define EE_WRITE		0x0140		// 01 0100 0000 write instruction
#define EE_WREN			0x0130		// 01 0011 0000 write enable instruction
#define EE_WDS			0x0100		// 01 0000 0000 write disable instruction
#define EE_PREN			0x0130		// 01 0011 0000 protect enable instruction
#define EE_PRCLEAR		0x0177		// 01 1111 1111 clear protect register instr

DWORD	EepromReadDword(int card,UINT reg);
WORD	EepromReadWord(int card,UINT EpromAddr);
void	EepromWriteDword(int card,UINT reg, DWORD val);
BOOL	EepromWriteWord(int card,UINT EpromAddr, WORD val);
BOOL	EepromSendCmd(int card,UINT cmd);
BOOL	EepromClock(int card);

#endif	//__FM_LIB_H__

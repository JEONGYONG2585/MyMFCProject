////////////////////////////////////////////////////////////////////////////
//
//      (C) 2002  FAS Technology Co., Ltd.
//		FILENAME:		Fm_lib.cpp
//		DESCRIPTION:	FM_8000
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <winioctl.h>
#include <io.h>
#include <process.h>
#include "fm_lib.h"
#include "error.h"

DWORD GetAxisAddress(DWORD dwAxis);

DWORD   LocalBuf[MAX_CARD_NUMS][LOCAL_SIZE];
DWORD   EepromBuf[MAX_CARD_NUMS][EEPROM_SIZE];

DWORD   EepromDefault[EEPROM_SIZE] = 
	{ 
	    0x520110b5,		// 0x0   PCIIDR; Device ID, Vendor ID
        0x08010001,		// 0x4   PCICCR; Class Code
        0x1004fa80,		// 0x8   Subsystem ID, Subsystem Vendor ID
        0x00000100,		// 0xc   Maximum Latency, Minimum Grant, Int Pin, Int Routing
        0xfffe0000,		// 0x10  LAS0RR; Local Address Space 0 Range
        0xfffe0000,		// 0x14  LAS1RR; Local Address Space 1 Range
        0xfffe0000,		// 0x18  LAS2RR; Local Address Space 2 Range
        0xfffe0000,		// 0x1c  LAS3RR; Local Address Space 3 Range
        0x00000000,		// 0x20  EROMRR; Expansion ROM Range
        0x04000001,		// 0x24  LAS0BA; Local Address Space 0 Base Address (Re-Map)
        0x03000001,		// 0x28  LAS1BA; Local Address Space 1 Base Address (Re-Map)
        0x02000001,		// 0x2c  LAS2BA; Local Address Space 2 Base Address (Re-Map)
        0x01000001,		// 0x30  LAS3BA; Local Address Space 3 Base Address (Re-Map)
        0x00000000,		// 0x34  EROMBA; Expansion ROM Base Address (Re-Map)
        0x40000000,		// 0x38  LAS0BRD; Local Address Space 0 Bus Region Descriptors
        0x40000000,		// 0x3c  LAS1BRD; Local Address Space 1 Bus Region Descriptors
        0x40000000,		// 0x40  LAS2BRD; Local Address Space 2 Bus Region Descriptors
        0x5431B8C0,		// 0x44  LAS3BRD; Local Address Space 3 Bus Region Descriptors
        0x00000000,		// 0x48  EROMBRD; Expansion ROM Bus Region Descriptors
        0x04010001,		// 0x4c  CS0BASE; Chip Select 0 Base
        0x03010001,		// 0x50  CS1BASE; Chip Select 1 Base
        0x02010001,		// 0x54  CS2BASE; Chip Select 2 Base
        0x01010001,		// 0x58  CS3BASE; Chip Select 3 Base
        0x00000000,		// 0x5c  INTCSR; Interrupt Control/Status
        0x08785b00		// 0x60  CNTRL; User I/O, EEPROM, Init Control                    
};


IOCTL_BLOCK_B    IoctlBlockB;
PIOCTL_BLOCK_B   pIoctlBlockB;

FAS_MOTION sFas;

//////////////////////////////////////////////////////////////////////
//              LIBRARY
//////////////////////////////////////////////////////////////////////
#define IRQ_THREAD_STOP    0
#define IRQ_THREAD_RUN     1
#define IRQ_THREAD_END     2

static const BYTE DrvName[MAX_CARD_NUMS][20] = {
	"\\\\.\\fm_160000",
	"\\\\.\\fm_160001",
	"\\\\.\\fm_160002",
	"\\\\.\\fm_160003",
	"\\\\.\\fm_160004",
	"\\\\.\\fm_160005",
	"\\\\.\\fm_160006",
	"\\\\.\\fm_160007" 
};

BOOL PciStart(int slotno) 
{
	int		i, iBdNo;
	DWORD	cb;

	sFas.hMsgWnd = NULL;
	sFas.bIsMsgQueued = TRUE;
	
	sFas.hMutex = CreateMutex(NULL,FALSE,TEXT("FM_16000_IO_MUTEX"));

	for(iBdNo=0;iBdNo<MAX_CARD_NUMS;iBdNo++) 
	{
		sFas.sDev[iBdNo].bIsExist =FALSE;
		sFas.sDev[iBdNo].evMsg = NULL;
		sFas.sDev[iBdNo].evReply = NULL;
        
		sFas.sDev[iBdNo].hSys = CreateFile(
									(const char *)DrvName[iBdNo],
									GENERIC_READ | GENERIC_WRITE,
									FILE_SHARE_READ | FILE_SHARE_WRITE,
									NULL,
									OPEN_EXISTING,
									FILE_ATTRIBUTE_NORMAL,
									NULL
								);

		if (sFas.sDev[iBdNo].hSys != INVALID_HANDLE_VALUE) 
		{
			// info init
			sFas.sDev[iBdNo].InfoPci.irq = 0;
			for( i=0; i < MAX_BASE_ADDR_NUMS; i++) 
			{
			   sFas.sDev[iBdNo].InfoPci.cs[i]     =0;
			   sFas.sDev[iBdNo].InfoPci.cs_len[i] =0;
			   sFas.sDev[iBdNo].InfoPci.io_len[i] =0;
			   sFas.sDev[iBdNo].InfoPci.io[i]     =0;
			}
            //PCI_INFO를 읽는 부분.
			DeviceIoControl(
				(HANDLE)sFas.sDev[iBdNo].hSys,
				(DWORD)PCI_IOCTL_GET_INFO,
				(LPVOID)&sFas.sDev[iBdNo].InfoPci,
				(DWORD)sizeof(INFO_PCI),
				(LPVOID)&sFas.sDev[iBdNo].InfoPci,
				(DWORD)sizeof(INFO_PCI),
				(LPDWORD)&cb,
				(LPOVERLAPPED)0
			);

			if( sFas.sDev[iBdNo].InfoPci.cs[0]==0 ) 
			{
				CloseHandle(sFas.sDev[iBdNo].hSys);
				sFas.sDev[iBdNo].hSys= NULL;

				ShowErr(iBdNo, IDE_ERR_GET_CARD_INFO, "PciStart=> Get Card Inf Error !");
				for( i=0; i < iBdNo ; i++) 
				{
					//Close Event
					PciClearEvent(i);

					//Clear Event
					PciIntClr(i);

					CloseHandle(sFas.sDev[i].hSys);
					sFas.sDev[i].hSys = NULL;
				}
				CloseHandle(sFas.sDev[i].hSys);
				sFas.sDev[i].hSys = NULL;
				return FALSE;
			} 
			else 
			{
				sFas.sDev[iBdNo].CardNum = sFas.sDev[iBdNo].InfoPci.CardNum;
				sFas.sDev[iBdNo].CardID = PciGetCardID(iBdNo);
				sFas.sDev[iBdNo].bIsExist = TRUE;

				//-------------------------------------------------------------
				//Create & Link Event
                if( PciSetEvent(iBdNo, slotno) == FALSE )
				{
					ShowErr(iBdNo, IDE_ERR_EVENT_LINK, "PciStart=> Set Event Error !");
					for( i=0; i < iBdNo ; i++) 
					{
						//Close Event
						PciClearEvent(i);

						//Clear Event
						PciIntClr(i);

						CloseHandle(sFas.sDev[i].hSys);
						sFas.sDev[i].hSys = NULL;
					}
					CloseHandle(sFas.sDev[i].hSys);
					sFas.sDev[i].hSys = NULL;

					return FALSE;
				}

				//-------------------------------------------------------------
				//Interrupt Start
				PciIntSet(iBdNo);

				for(i=0;i<LOCAL_SIZE;i++) 
				{
					LocalBuf[iBdNo][i] = PciReadReg(iBdNo,i*4);
				}

				for(i=0;i<EEPROM_SIZE;i++) 
				{
					EepromBuf[iBdNo][i] = EepromReadDword(iBdNo,i*4);
				}
			}
		} 
		else 
		{
			sFas.sDev[iBdNo].hSys = NULL;
		}
	}	//For iBdNo
	
	if ((sFas.sDev[0].bIsExist) || (sFas.sDev[1].bIsExist)
		|| (sFas.sDev[2].bIsExist) || (sFas.sDev[3].bIsExist)
		|| (sFas.sDev[4].bIsExist) || (sFas.sDev[5].bIsExist)
		|| (sFas.sDev[6].bIsExist) || (sFas.sDev[7].bIsExist))
	{
		return TRUE;
	}
	else
	{
		ShowErr(iBdNo, IDE_ERR_NONE_CARD, "PciStart=>Fail! load device driver !");
		return FALSE;
	}
}

BOOL PciEnd()
{
	int iBdNo;

	for(iBdNo=0; iBdNo<MAX_CARD_NUMS;iBdNo++) 
	{
		//Close Event
		PciClearEvent(iBdNo);

		//Interrupt Clear
		PciIntClr(iBdNo);

		CloseHandle(sFas.sDev[iBdNo].hSys);
		sFas.sDev[iBdNo].hSys = NULL;
	}

	::CloseHandle(sFas.hMutex);
	sFas.hMutex = NULL;

	TRACE("Close Handle, Events\n");

	return TRUE;
}

void PciSetWnd(HWND hWnd)
{
	sFas.hMsgWnd = hWnd;
}


void PciSetMsgQueued(BOOL bQueued)
{
	sFas.bIsMsgQueued = bQueued;
}


BOOL PciIsExist(int iBdNo)
{
	return sFas.sDev[iBdNo].bIsExist;
}


BOOL PciLinkEvent(int iBdNo, HANDLE *phReply, HANDLE *phMsg) 
{
	DWORD  cb;
	IOCTL_DATA      IoctlData;

	::WaitForSingleObject(sFas.hMutex,INFINITE);

	IoctlData.Data = (ULONG)*phReply;
	IoctlData.Data2 = sFas.nSlotPos;

	if(sFas.sDev[iBdNo].hSys!=NULL) 
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_REF_EVENT_REPLY,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);

		// IoctlData.Data2 는 Reply Handle의 Slot 위치이다.
		if (IoctlData.Data2 > MAX_EVENT_NUMS)
			return FALSE;

		//sFas.sDev[iBdNo].nReplyPos = IoctlData.Data2;
		sFas.nSlotPos = IoctlData.Data2;
	}

	IoctlData.Data = (ULONG)*phMsg;
	IoctlData.Data2 = 0;

	if(sFas.sDev[iBdNo].hSys!=NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_REF_EVENT_MESSAGE,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}

	::ReleaseMutex(sFas.hMutex);
	return TRUE;
}

BOOL PciUnlinkEvent(int iBdNo) 
{
	IOCTL_DATA IoctlData;
	DWORD cb;

	::WaitForSingleObject(sFas.hMutex,INFINITE);

	//IoctlData.Data = sFas.sDev[iBdNo].nReplyPos;
	IoctlData.Data = sFas.nSlotPos;
	IoctlData.Data2 = 0;   

	if (sFas.sDev[iBdNo].hSys != NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_DEREF_EVENT_REPLY,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}

	IoctlData.Data = iBdNo;
	IoctlData.Data2 = 0;

	if (sFas.sDev[iBdNo].hSys != NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_DEREF_EVENT_MESSAGE,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}

	::ReleaseMutex(sFas.hMutex);

	return TRUE;
}
/*
BOOL PciUnlinkAllEvent()
{
	DWORD cb;
	IOCTL_DATA IoctlData;
	int i, j;

	::WaitForSingleObject(sFas.hMutex,INFINITE);

	for (i=0; i<MAX_CARD_NUMS; i++)
	{
		if ((sFas.sDev[i].bIsExist == FALSE) || (sFas.sDev[i].hSys == NULL))
			continue;

		for (j=0; j<MAX_EVENT_NUMS; j++)
		{
			IoctlData.Data = j;
			IoctlData.Data2 = 0;   

			DeviceIoControl(
				(HANDLE)sFas.sDev[i].hSys,
				(DWORD)PCI_IOCTL_DEREF_EVENT_REPLY,
				(LPVOID)&IoctlData,
				(DWORD)sizeof(IOCTL_DATA),
				(LPVOID)&IoctlData,
				(DWORD)sizeof(IOCTL_DATA),
				(LPDWORD)&cb,
				(LPOVERLAPPED)0
			);
		}

		IoctlData.Data = i;
		IoctlData.Data2 = 0;   

		DeviceIoControl(
			(HANDLE)sFas.sDev[i].hSys,
			(DWORD)PCI_IOCTL_DEREF_EVENT_MESSAGE,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);

		PciIntClr(i);
	}

	::ReleaseMutex(sFas.hMutex);

	return TRUE;
}
*/
BOOL PciSetEvent(int iBdNo, int slotno)
{
	if (sFas.sDev[iBdNo].bIsExist == FALSE)
		return FALSE;

	sFas.sDev[iBdNo].evReply = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	sFas.sDev[iBdNo].evMsg = ::CreateEvent(NULL, FALSE, FALSE, NULL);	

	if ((sFas.sDev[iBdNo].evReply == NULL) || (sFas.sDev[iBdNo].evMsg == NULL))
	{
		::CloseHandle(sFas.sDev[iBdNo].evReply);
		sFas.sDev[iBdNo].evReply = NULL;

		::CloseHandle(sFas.sDev[iBdNo].evMsg);
		sFas.sDev[iBdNo].evMsg = NULL;

		return FALSE;
	}

	sFas.nSlotPos = slotno;

	if (PciLinkEvent(iBdNo, &sFas.sDev[iBdNo].evReply, &sFas.sDev[iBdNo].evMsg) == FALSE)
	{
		::CloseHandle(sFas.sDev[iBdNo].evReply);
		sFas.sDev[iBdNo].evReply= NULL;

		::CloseHandle(sFas.sDev[iBdNo].evMsg);
		sFas.sDev[iBdNo].evMsg= NULL;

		return FALSE;
	}

	return TRUE;
}

BOOL PciClearEvent(int iBdNo)
{
	if(sFas.sDev[iBdNo].bIsExist == FALSE)
		return FALSE;

	PciUnlinkEvent(iBdNo);

	::CloseHandle(sFas.sDev[iBdNo].evReply);
	::CloseHandle(sFas.sDev[iBdNo].evMsg);

	sFas.sDev[iBdNo].evReply = NULL;
	sFas.sDev[iBdNo].evMsg = NULL;

	return TRUE;
}

DWORD PciGetReplyType(int iBdNo)
{
	DWORD cb;
	IOCTL_DATA IoctlData = {0};

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	if (sFas.sDev[iBdNo].hSys != NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_GET_REPLY,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}
	::ReleaseMutex(sFas.hMutex);

	return IoctlData.Data;
}

DWORD PciGetMsgType(int iBdNo)
{
	DWORD cb;
	IOCTL_DATA IoctlData = {0};

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	if (sFas.sDev[iBdNo].hSys != NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_GET_MSG,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}
	::ReleaseMutex(sFas.hMutex);

	return IoctlData.Data;
}

void PciIntSet(int iBdNo)
{
	DWORD cb;
	IOCTL_DATA IoctlData;

	if (sFas.sDev[iBdNo].bIsExist == FALSE)
		return;

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	if (sFas.sDev[iBdNo].hSys != NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_SET_INT,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}
	::ReleaseMutex(sFas.hMutex);
}

void PciIntClr(int iBdNo)
{
	DWORD cb;
	IOCTL_DATA IoctlData;

	if (sFas.sDev[iBdNo].bIsExist == FALSE)
		return;

	::WaitForSingleObject(sFas.hMutex,INFINITE);

	if (sFas.sDev[iBdNo].hSys != NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_CLR_INT,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}
	::ReleaseMutex(sFas.hMutex);
}

void PciGetInt(int iBdNo,DWORD *count1,DWORD *count2)
{
	DWORD  cb;
	IOCTL_DATA      IoctlData = {0};

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	if(sFas.sDev[iBdNo].hSys!=NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_GET_INT,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}
	::ReleaseMutex(sFas.hMutex);
	*count1 = IoctlData.Data;
	*count2 = IoctlData.Data2;   
}

BYTE PciGetCardID(int iBdNo)
{
	WORD data;
	BYTE byRet ;

	data = (WORD)PciReadReg(iBdNo, 0x50);

	if ((data & 0x04) == 0) 
		byRet = 0x01; 
	else 
		byRet = 0x00; 

	if ((data & 0x20) == 0) 
		byRet |= 0x02; 

	if ((data & 0x100) == 0) 
		byRet |= 0x04; 

	return byRet;
}

BYTE PciGetCardIDFast(int iBdNo)
{
    if (sFas.sDev[iBdNo].bIsExist)
        return (BYTE)(sFas.sDev[iBdNo].CardID);
    else return 10;
}

void PciWrite(int iBdNo,DWORD address, LONG data)
{
	DWORD  cb;
	IOCTL_DATA      IoctlData;
	IoctlData.Addr = address; 
	IoctlData.Data = data;

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	if(sFas.sDev[iBdNo].hSys!=NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_WRITE,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}
	::ReleaseMutex(sFas.hMutex);
}

void PciWriteW(int iBdNo,DWORD address, WORD data)
{
	DWORD  cb;
	IOCTL_DATA      IoctlData;
	IoctlData.Addr = address; 
	IoctlData.Data  = (LONG)data;

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	if(sFas.sDev[iBdNo].hSys!=NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_WRITE_W,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}
	::ReleaseMutex(sFas.hMutex);
}

void PciWriteB(int iBdNo,DWORD address, BYTE data)
{
	DWORD  cb;
	IOCTL_DATA      IoctlData;
	IoctlData.Addr  = address; 
	IoctlData.Data  = (LONG)data;

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	if(sFas.sDev[iBdNo].hSys!=NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_WRITE_B,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}
	::ReleaseMutex(sFas.hMutex);
}

LONG PciRead(int iBdNo,DWORD address)
{
	DWORD  cb;
	IOCTL_DATA      IoctlData = {0};
	IoctlData.Addr = address; 

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	if(sFas.sDev[iBdNo].hSys!=NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_READ,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}
	::ReleaseMutex(sFas.hMutex);
	return (LONG)IoctlData.Data;
}


WORD PciReadW(int iBdNo,DWORD address)
{
	DWORD  cb;
	IOCTL_DATA      IoctlData = {0};
	IoctlData.Addr = address; 

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	if(sFas.sDev[iBdNo].hSys!=NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_READ_W,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}
	::ReleaseMutex(sFas.hMutex);
	return (WORD)IoctlData.Data;
}


BYTE PciReadB(int iBdNo,DWORD address)
{
	DWORD  cb;
	IOCTL_DATA      IoctlData = {0};
	IoctlData.Addr  = address; 

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	if(sFas.sDev[iBdNo].hSys!=NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_READ_B,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}
	::ReleaseMutex(sFas.hMutex);
	return (BYTE)IoctlData.Data;
}


void PciWritePort(int iBdNo,DWORD address, DWORD data)
{
	DWORD  cb;
	IOCTL_DATA      IoctlData;
	IoctlData.Addr = address; 
	IoctlData.Data    = (DWORD)data;

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	if(sFas.sDev[iBdNo].hSys!=NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_WRITE_PORT,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}
	::ReleaseMutex(sFas.hMutex);
}

void PciWritePortW(int iBdNo,DWORD address, WORD data)
{
	DWORD  cb;
	IOCTL_DATA      IoctlData;
	IoctlData.Addr = address; 
	IoctlData.Data    = (DWORD)data;

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	if(sFas.sDev[iBdNo].hSys!=NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_WRITE_PORT_W,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}
	::ReleaseMutex(sFas.hMutex);
}


void PciWritePortB(int iBdNo,DWORD address, BYTE data)
{
	DWORD  cb;
	IOCTL_DATA      IoctlData;
	IoctlData.Addr = address; 
	IoctlData.Data = (DWORD)data;

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	if(sFas.sDev[iBdNo].hSys!=NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_WRITE_PORT_B,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}
	::ReleaseMutex(sFas.hMutex);
}


DWORD PciReadPort(int iBdNo,DWORD address)
{
	DWORD  cb;
	IOCTL_DATA      IoctlData = {0};
	IoctlData.Addr = address; 

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	if(sFas.sDev[iBdNo].hSys!=NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_READ_PORT,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}
	::ReleaseMutex(sFas.hMutex);
	return (DWORD)IoctlData.Data;
}


WORD PciReadPortW(int iBdNo,DWORD address)
{
	DWORD  cb;
	IOCTL_DATA      IoctlData = {0};
	IoctlData.Addr = address; 

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	if(sFas.sDev[iBdNo].hSys!=NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_READ_PORT_W,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}
	::ReleaseMutex(sFas.hMutex);
	return (WORD)IoctlData.Data;
}

BYTE PciReadPortB(int iBdNo,DWORD address)
{
	DWORD  cb;
	IOCTL_DATA      IoctlData = {0};
	IoctlData.Addr = address; 

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	if(sFas.sDev[iBdNo].hSys!=NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_READ_PORT_B,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}
	::ReleaseMutex(sFas.hMutex);
	return (BYTE)IoctlData.Data;
}

void PciReadBlock(int iBdNo, DWORD address, BYTE* pData,  ULONG ulLength)
{
	IOCTL_BLOCK Block = {0};
	DWORD  cb;

	Block.Addr	= address + sFas.sDev[iBdNo].InfoPci.cs[PCI_BASE_CS3];
	Block.Length= ulLength;

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	if(sFas.sDev[iBdNo].hSys!=NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_READ_BLOCK,
			(LPVOID)&Block,
			(DWORD)sizeof(IOCTL_BLOCK),
			(LPVOID)&Block,
			(DWORD)sizeof(IOCTL_BLOCK),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}
	::ReleaseMutex(sFas.hMutex);

	::CopyMemory(pData, Block.Buf, Block.Return * 4 );
}

void PciReadBlockDP(int iBdNo, DWORD address, BYTE* pData, ULONG ulLength)
{
	IOCTL_BLOCK_B Block = {0};
	DWORD  cb;

	Block.Addr	= address + sFas.sDev[iBdNo].InfoPci.cs[PCI_BASE_CS3];
	Block.Length= ulLength;

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	if(sFas.sDev[iBdNo].hSys!=NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_READ_BLOCK_DP_B,
			(LPVOID)&Block,
			(DWORD)sizeof(IOCTL_BLOCK_B),
			(LPVOID)&Block,
			(DWORD)sizeof(IOCTL_BLOCK_B),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}
	::ReleaseMutex(sFas.hMutex);

	::CopyMemory(pData, Block.Buf, Block.Return );
}


void PciWriteBlockDP(int iBdNo, DWORD address, BYTE* pData,  ULONG ulLength )
{
	IOCTL_BLOCK_B Block;
	DWORD  cb;

	Block.Addr	= address + sFas.sDev[iBdNo].InfoPci.cs[PCI_BASE_CS3];
	Block.Length= ulLength;

	::CopyMemory( Block.Buf, pData, ulLength );

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	if(sFas.sDev[iBdNo].hSys!=NULL) 
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys, 
			(DWORD)PCI_IOCTL_WRITE_BLOCK_DP_B,
			(LPVOID)&Block,
			(DWORD)sizeof(IOCTL_BLOCK_B),
			(LPVOID)&Block,
			(DWORD)sizeof(IOCTL_BLOCK_B),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}
	::ReleaseMutex(sFas.hMutex);
}


void PciWriteDP(int iBdNo,DWORD address, LONG data)
{
	DWORD  cb;
	IOCTL_DATA      IoctlData;
	IoctlData.Addr = address; 
	IoctlData.Data = data;

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	if(sFas.sDev[iBdNo].hSys!=NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_WRITE_DP,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}
	::ReleaseMutex(sFas.hMutex);
}

void PciWriteDPW(int iBdNo,DWORD address, WORD data)
{
	DWORD  cb;
	IOCTL_DATA      IoctlData;
	IoctlData.Addr = address; 
	IoctlData.Data  = (DWORD)data;

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	if(sFas.sDev[iBdNo].hSys!=NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_WRITE_DP_W,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}
	::ReleaseMutex(sFas.hMutex);
}


LONG PciReadDP(int iBdNo, DWORD address)
{
	DWORD  cb;
	IOCTL_DATA      IoctlData = {0};
	IoctlData.Addr = address; 

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	if(sFas.sDev[iBdNo].hSys!=NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_READ_DP,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}
	::ReleaseMutex(sFas.hMutex);
	return IoctlData.Data;
}


WORD PciReadDPW(int iBdNo,DWORD address)
{
	DWORD  cb;
	IOCTL_DATA      IoctlData = {0};
	IoctlData.Addr = address; 

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	if(sFas.sDev[iBdNo].hSys!=NULL)
	{
		DeviceIoControl(
			(HANDLE)sFas.sDev[iBdNo].hSys,
			(DWORD)PCI_IOCTL_READ_DP_W,
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPVOID)&IoctlData,
			(DWORD)sizeof(IOCTL_DATA),
			(LPDWORD)&cb,
			(LPOVERLAPPED)0
		);
	}
	::ReleaseMutex(sFas.hMutex);
	return (WORD)IoctlData.Data;
}


//////////////////////////////////////////////////////////////////////

DWORD PciReadReg(int iBdNo,DWORD offset)
{
	return PciRead(iBdNo,sFas.sDev[iBdNo].InfoPci.cs[PCI_BASE_REG]+offset);
}

void PciWriteReg(int iBdNo,DWORD offset,DWORD data)
{
	PciWrite(iBdNo,sFas.sDev[iBdNo].InfoPci.cs[PCI_BASE_REG]+offset,data);
}


DWORD PciEepRead(int iBdNo)
{
	return PciRead(iBdNo,sFas.sDev[iBdNo].InfoPci.cs[PCI_BASE_REG]+0x50);
}

void PciEepWrite(int iBdNo,DWORD data)
{
    PciWrite(iBdNo,sFas.sDev[iBdNo].InfoPci.cs[PCI_BASE_REG]+0x50,data);
}

int PciIntRead(int iBdNo,PIOCTL_DATA pIoData, LPOVERLAPPED ovl)
{
	DWORD cb;
	if(!sFas.sDev[iBdNo].hSys) return 1;

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	DeviceIoControl(
		(HANDLE)sFas.sDev[iBdNo].hSys,
		(DWORD)PCI_IOCTL_READ_INT,
		(LPVOID)pIoData,
		(DWORD)sizeof(IOCTL_DATA),
		(LPVOID)pIoData,
		(DWORD)sizeof(IOCTL_DATA), 
		(LPDWORD)&cb,
		(LPOVERLAPPED)ovl
	);
	::ReleaseMutex(sFas.hMutex);
	return 0;	
}

int PciIntReadEnd(int iBdNo)
{
	DWORD		cb;
	IOCTL_DATA	IoData;
	if(!sFas.sDev[iBdNo].hSys) return 1;

	::WaitForSingleObject(sFas.hMutex,INFINITE);
	DeviceIoControl(
		(HANDLE)sFas.sDev[iBdNo].hSys,
		(DWORD)PCI_IOCTL_READ_INT_END,
		(LPVOID)&IoData,
		(DWORD)sizeof(IOCTL_DATA),
		(LPVOID)&IoData,
		(DWORD)sizeof(IOCTL_DATA),
		(LPDWORD)&cb,
		(LPOVERLAPPED)0
	);
	::ReleaseMutex(sFas.hMutex);
	return 0;
}


//////////////////////////////////////////////////////////////////////
//
//              EEPROM
//
//////////////////////////////////////////////////////////////////////

DWORD EepromReadDword(int iBdNo,UINT reg)
{
	DWORD val;
	reg >>= 1;
	val = (DWORD)EepromReadWord(iBdNo,reg);
	val <<= 16;
	val |= EepromReadWord(iBdNo,reg+1);
	return val;
}

void EepromWriteDword(int iBdNo,UINT reg, DWORD val)
{	
	reg >>= 1;
	EepromWriteWord(iBdNo, reg, (WORD)(val >> 16));	
	EepromWriteWord(iBdNo, reg+1, (WORD)val);			
}


WORD EepromReadWord(int iBdNo,UINT EpromAddr)
{	
	WORD acc = 0x0;
	UINT i;
	EepromSendCmd(iBdNo,EE_READ | (EpromAddr & EE_ADDR_MASK));
	for (i = 0; i < 16; i++)
	{	
		acc <<= 1;
		EepromClock(iBdNo);
		if (PciEepRead(iBdNo) & 0x08000000)
			acc |= 0x0001;
	}
	return acc;
}


BOOL EepromWriteWord(int iBdNo, UINT EpromAddr, WORD val)
{	
	UINT i;
	DWORD regval;
	EepromSendCmd(iBdNo,EE_WREN);	
	EepromSendCmd(iBdNo,EE_WRITE | (EpromAddr & EE_ADDR_MASK));
	regval = PciEepRead(iBdNo) & ~0x04000000; // Pre-clear data bit

	for (i = 16; i ; i--) {	
		PciEepWrite(iBdNo,regval|((val & 0x8000)? 0x04000000: 0x0));
		EepromClock(iBdNo);
		val <<= 1;
	}
	// Activate chip's status mechanism: deselect and reselect
	PciEepWrite(iBdNo,regval & ~0x02000000);	// Deselect
	PciEepWrite(iBdNo,regval);					// Reselect
	// Poll chip status until write-cycle completes (or times out)
	// (Timeout counter is uncalibrated and somewhat arbitrary!)
	for (i = 2000; !(PciEepRead(iBdNo) & 0x08000000); i--) {
		if (i) continue;		
		return FALSE; // Timeout!
	}
	EepromSendCmd(iBdNo,EE_WDS);		// Write disable
	return TRUE;
}

BOOL EepromSendCmd(int iBdNo,UINT cmd)
{	
	UINT i;
	DWORD regval;
	
	regval  =  PciEepRead(iBdNo);
	regval |=  0x02000000;	// Chip select TRUE
	regval &= ~0x05000000;	// Clear instruction and clock bits
	// Toggle EEPROM's chip select (to get it out of Shift Register Mode)
	PciEepWrite(iBdNo,regval & ~0x02000000); // Chip select FALSE
	PciEepWrite(iBdNo,regval);

	for (i = 0; i < EE_CMD_LEN; i++)
	{	// Send instruction - one bit at a time
		regval &= ~0x04000000;	// Pre-clear the instruction bit
		// Set up one instruction bit
		regval |= (cmd & (0x01 << (EE_CMD_LEN-1)))? 0x04000000: 0x00000000;
		PciEepWrite(iBdNo,regval);
		EepromClock(iBdNo);
		cmd <<= 1;	// Align next instruction bit
	}
	return TRUE;
}

BOOL EepromClock(int iBdNo)
{	
	DWORD regval;
	regval = PciEepRead(iBdNo);
	PciEepWrite(iBdNo,regval & ~0x01000000);	// Clock low
	PciEepWrite(iBdNo,regval | 0x01000000);	    // Clock high
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//추가된 함수 정의 (삽입 2002/08/02)

int PciGetAxisInfo(int iBdNo)
// iBdNo 번째로 설치된 보드의 BdID를 반납.
{
	long lTmp;

	lTmp = PciReadDP(iBdNo, FM_16000_AXIS_ADDRESS + sFas.sDev[iBdNo].InfoPci.cs[PCI_BASE_CS3]);
	lTmp &= 0x0FFFF;

	switch(lTmp){
	case 0x0001 : return 0; break;
	case 0x0002 : return 1; break;
	case 0x0004 : return 2; break;
	case 0x0008 : return 3; break;
	case 0x0010 : return 4; break;
	case 0x0020 : return 5; break;
	case 0x0040 : return 6; break;
	case 0x0080 : return 7; break;
	case 0x0100 : return 8; break;
	case 0x0200 : return 9; break;
	case 0x0400 : return 10; break;
	case 0x0800 : return 11; break;
	case 0x1000 : return 12; break;
	case 0x2000 : return 13; break;
	case 0x4000 : return 14; break;
	case 0x8000 : return 15; break;
	default :   
		ShowErr(iBdNo, IDE_ERR_GET_CARD_ID, "PciGetAxisInfo=> Could not Get BdID!");
		return -1;
	}
}

DWORD GetAxisAddress(DWORD dwAxis)
{
	return (dwAxis < MAX_AXIS_NUMS) ? (0x0200 * dwAxis) : 0x2000;
}
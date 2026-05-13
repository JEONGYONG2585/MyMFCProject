// Rs232CB.cpp: implementation of the CRs232CB class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "Rs232CB.h"

#include "COMM_Define.h"
#include "ReturnCodes_Define.h"

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Defines. ----------------------------------------------------
static const char	ASCII_STX		= 0x02;
static const char	ASCII_ETX		= 0x03;
static const char	ASCII_LF		= 0x0a;
static const char	ASCII_CR		= 0x0d;
static const char	ASCII_SPACE		= 0x20;
static const char	ASCII_XON		= 0x11;
static const char	ASCII_XOFF		= 0x13;

static const BYTE	ASCII_NODE		= (0xAA);
static const BYTE	ASCII_NODESTART	= (0xCC);
static const BYTE	ASCII_NODEEND	= (0xEE);

static const int	LEVL_NONE		= 0;
static const int	LEVL_HEADER		= 1;
static const int	LEVL_DATA		= 2;
static const int	LEVL_NODEMARK	= 3;
static const int	LEVL_FINISHED	= 4;

static const int	PACKET_OFFSET_SLAVE		= 0;
static const int	PACKET_OFFSET_SYNC		= 1;
static const int	PACKET_OFFSET_CMD		= 2;
static const int	PACKET_OFFSET_RTNCODE	= 3;
static const int	PACKET_OFFSET_DATA		= 4;

static const unsigned int	PACKET_SIZE_SEND_PREFIXED	= /*Slave No.*/1 + /*SyncNo*/1 + /*Command*/1 + /*Checksum*/2;
static const unsigned int	PACKET_SIZE_RECV_PREFIXED	= /*Slave No.*/1 + /*SyncNo*/1 + /*Command*/1 + /*ReturnCode*/1 + /*Checksum*/2;

static const int	FRAME_SIZE_PREFIXED	= /*Header*/2 + /*Tail*/2;

static const int	PACKETV6_OFFSET_SLAVE		= 0;
static const int	PACKETV6_OFFSET_CMD			= 1;
static const int	PACKETV6_OFFSET_RTNCODE		= 2;
static const int	PACKETV6_OFFSET_DATA		= 3;

static const unsigned int	PACKETV6_SIZE_SEND_PREFIXED	= /*Slave No.*/1 + /*Command*/1 + /*Checksum*/2;
static const unsigned int	PACKETV6_SIZE_RECV_PREFIXED	= /*Slave No.*/1 + /*Command*/1 + /*ReturnCode*/1 + /*Checksum*/2;

static const int	FRAMEV6_SIZE_PREFIXED	= /*Header*/2 + /*Tail*/2;


static const enum _RECV_RETURN
{
	RETURN_OK = 0,
	RETURN_R_RECVFILEERROR,		// File Error
	RETURN_R_NORESPONSE,		// didn't receive packet which expected.
	RETURN_R_DATAOVERFLOW,		// Data Overflow
	RETURN_R_CORRUPTRECVDATA,	// CRC Failed.
	RETURN_A_INCORRECTTARGET,	// Not Mine
	RETURN_A_RECVNOTENOUGH,		// Received less than frame size
	RETURN_A_FMP_PACKETERROR,	// received FMP_PACKET_ERROR
	RETURN_A_FMP_PACKETCRCERROR,	// received FMP_PACKETCRCERROR
	RETURN_A_RECVNODATA			// received less than expected.
};

static const WORD TABLE_CRCVALUE[] =
{
	0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
	0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
	0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
	0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
	0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
	0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
	0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
	0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
	0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
	0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
	0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
	0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
	0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
	0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
	0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
	0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
	0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
	0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
	0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
	0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
	0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
	0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
	0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
	0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
	0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
	0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
	0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
	0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
	0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
	0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
	0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
	0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040
};

static const unsigned char DEFAULT_RETRY_COUNT		= 3;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRs232CB::CRs232CB()
{
	int i;

	m_bConnected = FALSE;
	//m_hSync = CreateSemaphore(NULL, 1, 1, _T("COMM_EZI_SERVO_PLUSR"));
	m_hSync = CreateMutex(NULL, FALSE, NULL);

	for (i=0; i<MAX_ALLOW_SLAVE_NUM; i++)
		m_nProtocolSyncByte[i] = 0xF0;	// 초기화 없이도 동작해야 함.
}

CRs232CB::~CRs232CB()
{
	if (m_bConnected)
		ClosePort();

	CloseHandle(m_hSync);
}

BOOL CRs232CB::OpenPort(BYTE nPortNo, DWORD dwBaud, BYTE nByteSize, BYTE nParity, BYTE nStopBit)
{
	COMMTIMEOUTS timeouts;
	DCB dcb;
	TCHAR chBuff[16];
	int i;

	if (m_bConnected)
		ClosePort();
	
	m_bConnected = FALSE;

	// 포트 열기
	m_nPortNo = nPortNo;
	m_dwBaudrate = dwBaud;

	_stprintf(chBuff, _T("//./COM%d"), nPortNo);	// = sPortName;
	m_hComm = CreateFile(chBuff, 
		GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	
	// 포트 상태 설정.
	
	// EV_RXCHAR event 설정
	SetCommMask(m_hComm, EV_RXCHAR | EV_RXFLAG | EV_EVENT1 | EV_EVENT2);
	
	// InQueue, OutQueue 크기 설정.
	SetupComm(m_hComm, 4096, 4096);	
	
	// 포트 비우기.
	PurgeComm(m_hComm, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
	
	// timeout 설정.
	timeouts.ReadIntervalTimeout = 1;
	timeouts.ReadTotalTimeoutMultiplier = 1;
	timeouts.ReadTotalTimeoutConstant = 1;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
	SetCommTimeouts(m_hComm, &timeouts);
	
	// dcb 설정
	dcb.DCBlength = sizeof(DCB);
	//GetCommState(m_hComm, &dcb);	// 예전 값을 읽음.
	dcb.BaudRate = dwBaud;
	dcb.fBinary = TRUE;
	dcb.fParity = FALSE;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDtrControl = DTR_CONTROL_ENABLE; 
	dcb.fDsrSensitivity = FALSE;
	dcb.fTXContinueOnXoff = FALSE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
	dcb.fErrorChar = FALSE;
	dcb.fNull = FALSE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;	// RTS_CONTROL_TOGGLE for Rs485 Communication.
	dcb.fAbortOnError = FALSE;
	dcb.XonLim = 1;
	dcb.XoffLim = 256;
	dcb.ByteSize = nByteSize;
	dcb.Parity = nParity;
	dcb.StopBits = nStopBit;
	dcb.XonChar = ASCII_XON;
	dcb.XoffChar = ASCII_XOFF;
	dcb.ErrorChar = 0;
	dcb.EofChar = 0;
	dcb.EvtChar = 0;

	SetCommState(m_hComm, &dcb);

	for (i=0; i<MAX_ALLOW_SLAVE_NUM; i++)
		m_nProtocolSyncByte[i] = 0xF0;

	m_bConnected = TRUE;

	return TRUE;
}

void CRs232CB::ClosePort()
{
	WaitForSingleObject(m_hSync, INFINITE);
	ClosePortWithoutSync();
	ReleaseMutex(m_hSync);
}

void CRs232CB::ClosePortWithoutSync()
{
	if (m_bConnected)
	{
		m_bConnected = FALSE;
		CloseHandle(m_hComm);
		m_hComm = NULL;
	}
}

BOOL CRs232CB::ReadCommOneByte(BYTE* by, BOOL* bError)
{
	DWORD dwRead;

	*by = 0x00;

	if (!ReadFile(m_hComm, by, 1, &dwRead, NULL))
	{
		DWORD dwValue = GetLastError();

		switch (dwValue)
		{
		case ERROR_ACCESS_DENIED:
			m_bConnected = FALSE;
			m_hComm = NULL;
			break;
		case ERROR_INVALID_HANDLE:
			m_bConnected = FALSE;
			m_hComm = NULL;
			break;
		case ERROR_GEN_FAILURE:
			m_bConnected = FALSE;
			m_hComm = NULL;
			break;
		default:
			break;
		}

		*bError = TRUE;

		// dcb.fAbortOnError = FALSE; 이므로 필요없음.
		//COMSTAT	comstat;
		//DWORD dwErrorFlags;
		//ClearCommError(m_hComm, &dwErrorFlags, &comstat);

		return FALSE;
	}

	*bError = FALSE;
	
	return (dwRead > 0);
}

BOOL CRs232CB::WriteComm(BYTE *pBuff, DWORD nToWrite)
{
	DWORD dwValue;

	if (WriteFile(m_hComm, pBuff, nToWrite, &dwValue, NULL))
	{
		FlushFileBuffers(m_hComm);
		return TRUE;
	}
	else
	{
		dwValue = GetLastError();
		switch (dwValue)
		{
		case ERROR_ACCESS_DENIED:
			m_bConnected = FALSE;
			m_hComm = NULL;
			break;
		case ERROR_INVALID_HANDLE:
			m_bConnected = FALSE;
			m_hComm = NULL;
			break;
		case ERROR_GEN_FAILURE:
			m_bConnected = FALSE;
			m_hComm = NULL;
			break;
		default:
			break;
		}

		return FALSE;
	}
}

BOOL CRs232CB::SendPacket(PACKET_INFO_V8* lpPacket)
{
	BYTE* ptr;
	WORD wCRCValue;
	DWORD i, nFrameLen;
	
	// DATA FRAME ---------------------------------------------------
	ptr = m_buffSend;
	// Address
	*(ptr++) = lpPacket->iSlaveNo;

	// Sync No.
	*(ptr++) = lpPacket->nSyncNo;
	
	// Frame Type
	*(ptr++) = lpPacket->byCmd;
	
	// Data
	if ((lpPacket->lpIN != NULL) && (lpPacket->dwINlen > 0))
	{
		for (i=0; i<lpPacket->dwINlen; i++)
			*(ptr++) = *(((BYTE*)(lpPacket->lpIN)) + i);
	}
	
	// CRC
	wCRCValue = CalcCRC(m_buffSend, (DWORD)(ptr - m_buffSend));
	*(ptr++) = LOBYTE(wCRCValue);
	*(ptr++) = HIBYTE(wCRCValue);

	nFrameLen = PACKET_SIZE_SEND_PREFIXED + lpPacket->dwINlen;	// Slave, Cmd, Checksum(2), data, SyncNo
	
	// PACKET FRAME -------------------------------------------------
	memset(m_buffSendModified, 0x00, MAX_DOUBLE_BUFFER_SIZE);
	ptr = m_buffSendModified;
	
	m_nLenBuffSendModified = FRAME_SIZE_PREFIXED + nFrameLen;	// Header(2), Tail(2)

	// Header
	*(ptr++) = ASCII_NODE;
	*(ptr++) = ASCII_NODESTART;
	
	for (i=0; i<nFrameLen; i++)
	{
		*(ptr++) = m_buffSend[i];
		if (m_buffSend[i] == ASCII_NODE)
		{
			*(ptr++) = ASCII_NODE;
			m_nLenBuffSendModified++;
		}
	}
	
	// Tail
	*(ptr++) = ASCII_NODE;
	*(ptr++) = ASCII_NODEEND;

	if (IsConnected() == FALSE)
		return FALSE;

	return WriteComm(m_buffSendModified, m_nLenBuffSendModified);
}


int CRs232CB::RecvPacket(PACKET_INFO_V8* lpPacket)
{
	DWORD dwStartTime;
	int nLevl = LEVL_NONE;
	BYTE by;
	BOOL bRecvError;

	m_nRecvCount = 0;

	// Receive Packet
	dwStartTime = timeGetTime();
	do
	{
		if (ReadCommOneByte(&by, &bRecvError) == FALSE)
		{
			if (bRecvError)
			{
				return RETURN_R_RECVFILEERROR;
			}
			else if ((timeGetTime() - dwStartTime) > lpPacket->dwWaitTime)
			{
				// Timeout.
				if (m_nRecvCount > 0)
				{
					if (lpPacket->bIgnoreTimeOut == FALSE)
					{
					}
				}
				else
				{
					//if (lpPacket->bIgnoreTimeOut == FALSE)
				}

				return RETURN_R_NORESPONSE;
			}
			else
			{
				continue;
			}
		}

		if (m_nRecvCount >= MAX_BUFFER_SIZE)
		{
			return RETURN_R_DATAOVERFLOW;
		}

		switch (nLevl)
		{
		case LEVL_NONE:
			if (by == ASCII_NODE)
				nLevl = LEVL_HEADER;
			break;
		case LEVL_HEADER:
			if (by == ASCII_NODESTART)
			{
				if (m_nRecvCount > 0)
				{
				}

				nLevl = LEVL_DATA;
				m_nRecvCount = 0;
			}
			else
			{
				nLevl = LEVL_NONE;
				m_nRecvCount = 0;
			}
			break;
		case LEVL_DATA:
			if (by == ASCII_NODE)
				nLevl = LEVL_NODEMARK;
			else
			{
				m_buffRecvData[m_nRecvCount] = by;
				m_nRecvCount++;
			}
			break;
		case LEVL_NODEMARK:
			if (by == ASCII_NODE)
			{
				nLevl = LEVL_DATA;
				
				m_buffRecvData[m_nRecvCount] = by;
				m_nRecvCount++;
			}
			else if (by == ASCII_NODESTART)
			{
				// 초기화
				nLevl = LEVL_DATA;
				m_nRecvCount = 0;
			}
			else if (by == ASCII_NODEEND)
			{
				if (CalcCRC(m_buffRecvData, m_nRecvCount) != 0x00)
				{
					return RETURN_R_CORRUPTRECVDATA;
				}
				else
					nLevl = LEVL_FINISHED;
			}
			break;
		}

		// 시간 체크를 뒤로 옮김.
		dwStartTime = timeGetTime();
	}
	while (nLevl != LEVL_FINISHED);

	return RETURN_OK;
}

int CRs232CB::AnalizePacket(PACKET_INFO_V8* lpPacket, BOOL bPrintLog)
{
	if (m_nRecvCount < PACKET_SIZE_RECV_PREFIXED)
	{
		//ASSERT(FALSE);

		//if (bPrintLog)
		return RETURN_A_RECVNOTENOUGH;
	}

	if ((m_buffRecvData[PACKET_OFFSET_SLAVE] != lpPacket->iSlaveNo) || (m_buffRecvData[PACKET_OFFSET_CMD] != lpPacket->byCmd) || (m_buffRecvData[PACKET_OFFSET_SYNC] != lpPacket->nSyncNo))
	{
		if (bPrintLog)
		{
			//if (m_buffRecvData[PACKET_OFFSET_SLAVE] != lpPacket->iSlaveNo)
			//	TraceMsg(_T("CRs232CB::RecvPacket(P%d, S%d, N0x%02X, C0x%02X) Different Slave Number. (Recv_Slave %d)\r\n"), m_nPortNo, lpPacket->iSlaveNo, lpPacket->nSyncNo, lpPacket->byCmd, m_buffRecvData[PACKET_OFFSET_SLAVE]);

			//if (m_buffRecvData[PACKET_OFFSET_CMD] != lpPacket->byCmd)
			//	TraceMsg(_T("CRs232CB::RecvPacket(P%d, S%d, N0x%02X, C0x%02X) Different Command. (Recv_Cmd 0x%02X)\r\n"), m_nPortNo, lpPacket->iSlaveNo, lpPacket->nSyncNo,lpPacket->byCmd, m_buffRecvData[PACKET_OFFSET_CMD]);

			//if (m_buffRecvData[PACKET_OFFSET_SYNC] != lpPacket->nSyncNo)
			//	TraceMsg(_T("CRs232CB::RecvPacket(P%d, S%d, N0x%02X, C0x%02X) Different Sync No. (Recv_Sync 0x%02X)\r\n"), m_nPortNo, lpPacket->iSlaveNo, lpPacket->nSyncNo, lpPacket->byCmd, m_buffRecvData[PACKET_OFFSET_SYNC]);
		}

		return RETURN_A_INCORRECTTARGET;
	}

	// Copy Return code.
	lpPacket->nRtnCode = m_buffRecvData[PACKET_OFFSET_RTNCODE];
	if (m_nRecvCount == PACKET_SIZE_RECV_PREFIXED)
	{
		switch (lpPacket->nRtnCode)
		{
			if (bPrintLog)
			{
			}
			return RETURN_A_FMP_PACKETERROR;
			break;
		case PIEZO_RETURN::FMP_PACKETCRCERROR:
			return RETURN_A_FMP_PACKETCRCERROR;
			break;
		}
	}
	else if ((lpPacket->lpOUT != NULL) && (lpPacket->dwOUTlen > 0))
	{
		// Copy Data.
		if ((m_nRecvCount - PACKET_SIZE_RECV_PREFIXED) != lpPacket->dwOUTlen)
		{
			if (bPrintLog)
			{
			}
			return RETURN_A_RECVNODATA;
		}

		memcpy(lpPacket->lpOUT, &(m_buffRecvData[PACKET_OFFSET_DATA]), min((m_nRecvCount - PACKET_SIZE_RECV_PREFIXED), lpPacket->dwOUTlen));
	}
	
	return RETURN_OK;
}


WORD CRs232CB::CalcCRC(BYTE* lpBuff, DWORD dwLen)
{
	WORD wCRCWord = 0xFFFF;
	
	while (dwLen--)
	{
		//nTemp = wCRCWord ^ *(lpBuff++);
		//wCRCWord >>= 8;
		//wCRCWord ^= TABLE_CRCVALUE[nTemp];
		wCRCWord = (wCRCWord >> 8) ^ TABLE_CRCVALUE[((wCRCWord ^ *lpBuff++) & 0xFF)];
	}

	return wCRCWord;
}

BYTE CRs232CB::GetNextSyncNo(BYTE iSlaveNo)
{
	if (iSlaveNo >= MAX_SLAVE_NUMS)
	{
		//ASSERT(FALSE);
		return 0;
	}

	m_nProtocolSyncByte[iSlaveNo]++;
	return m_nProtocolSyncByte[iSlaveNo];
}

BYTE CRs232CB::GetNextSyncNoAll()
{
	BYTE iSyncNo;
	int i;

	iSyncNo = m_nProtocolSyncByte[0] + 1;

	for (i=0; i<MAX_SLAVE_NUMS; i++)
	{
		if (iSyncNo == m_nProtocolSyncByte[i])
			iSyncNo++;
	}

	for (i=0; i<MAX_SLAVE_NUMS; i++)
		m_nProtocolSyncByte[i] = iSyncNo;

	return iSyncNo;
}

int CRs232CB::FuncCommand(PACKET_INFO_V8* lpPacket)
{
	int nRetry = lpPacket->nRetryCount;
	int nLastErr = RETURN_OK;
	int nReturn;

	// Check Connection ------------------------------------------------------------
	if (IsConnected() == FALSE)
		return PIEZO_RETURN::FMC_DISCONNECTED;

	do
	{
		// Send ------------------------------------------------------------
		if (SendPacket(lpPacket) == FALSE)
			return PIEZO_RETURN::FMC_DISCONNECTED;

		while (TRUE)
		{
			// Recv ------------------------------------------------------------
			memset(lpPacket->lpOUT, 0x00, lpPacket->dwOUTlen);
			nReturn = RecvPacket(lpPacket);

			switch (nReturn)
			{
			case RETURN_R_RECVFILEERROR:		// 에러 리턴
				return PIEZO_RETURN::FMC_DISCONNECTED;
				break;
			case RETURN_R_DATAOVERFLOW:			// Buffer Overflow
				return PIEZO_RETURN::FMC_RECVPACKET_ERROR;
				break;
			//case RETURN_OK:		break;
			//case RETURN_R_NORESPONSE:			break;	// Timeout1 : 재시도
			//case RETURN_R_INCORRECTRESPONSE:	break;	// Timeout2 : 재시도
			//case RETURN_R_CORRUPTRECVDATA:	break;	// 재시도
			}

			// Analyze ------------------------------------------------------------
			if (nReturn == RETURN_OK)
			{
				nReturn = AnalizePacket(lpPacket);

				switch (nReturn)
				{
				case RETURN_A_INCORRECTTARGET:
					// 내꺼가 아님.	다시 Recv
					continue;
					break;
				//case RETURN_OK:		break;			// 정상
				//case RETURN_A_RECVNOTENOUGH:		break;		// 크기 불충분 (Frame 구성도 안됨) : 재시도
				//case RETURN_A_FMP_PACKETERROR:	break;		// FMP_PACKETERROR 받음. : 재시도
				//case RETURN_A_FMP_PACKETCRCERROR:	break;		// FMP_PACKETCRCERROR 받음 : 재시도
				//case RETURN_A_RECVNODATA:			break;		// 원하는 크기의 Packet이 아님. : 재시도
				}
			}

			break;
		}

		// 같은 에러이면 nRetry 횟수 감소, 다른 에러이면 Default값으로 복귀.
		nRetry = ((nLastErr != RETURN_OK) && (nReturn != nLastErr)) ? (lpPacket->nRetryCount) : (nRetry - 1);
		nLastErr = nReturn;
	}
	while ((nReturn != RETURN_OK) && (nRetry > 0));	// nReturn이 RETURN_OK이거나 Retry회수가 0 이하가 되면 빠져나감.

	switch (nReturn)
	{
	case RETURN_R_RECVFILEERROR:			// File error
		nReturn = PIEZO_RETURN::FMC_DISCONNECTED;
		break;
	case RETURN_R_NORESPONSE:
	case RETURN_A_INCORRECTTARGET:
		nReturn = PIEZO_RETURN::FMC_TIMEOUT_ERROR;
		break;
	case RETURN_R_CORRUPTRECVDATA:			// received Packet CRC was wrong.
		nReturn = PIEZO_RETURN::FMC_CRCFAILED_ERROR;
		break;
	case RETURN_R_DATAOVERFLOW:
	case RETURN_A_RECVNOTENOUGH:
	case RETURN_A_RECVNODATA:
		nReturn = PIEZO_RETURN::FMC_RECVPACKET_ERROR;
		break;
	//case RETURN_OK:
	//case RETURN_A_FMP_PACKETCRCERROR:			// Packet CRC which Drive received was wrong.
	//case RETURN_A_FMP_PACKETERROR:
	default:
		nReturn = lpPacket->nRtnCode;
		break;
	}

	return nReturn;
}


int CRs232CB::FuncCommandNoResp(PACKET_INFO_V8* lpPacket)
{
	if (IsConnected() == FALSE)
		return PIEZO_RETURN::FMC_DISCONNECTED;

	if (SendPacket(lpPacket) == FALSE)
		return PIEZO_RETURN::FMC_DISCONNECTED;

	return FMM_OK;
}


int CRs232CB::DoSendCommand(BYTE iSlaveNo, BYTE byCmd, LPVOID lpIN, DWORD dwINlen, LPVOID lpOUT, DWORD dwOUTlen, DWORD dwWaitTime, BOOL bIgnoreTimeOut)
{
	PACKET_INFO_V8 packetinfo = {0};
	int nReturn;

	if (IsConnected() == FALSE)
		return PIEZO_RETURN::FMC_DISCONNECTED;

	packetinfo.iSlaveNo = iSlaveNo;
	packetinfo.byCmd = byCmd;

	packetinfo.lpIN = lpIN;
	packetinfo.dwINlen = dwINlen;

	packetinfo.lpOUT = lpOUT;
	packetinfo.dwOUTlen = dwOUTlen;

	packetinfo.dwWaitTime = dwWaitTime;
	packetinfo.bIgnoreTimeOut = bIgnoreTimeOut;

	packetinfo.nRetryCount = (bIgnoreTimeOut) ? DEFAULT_RETRY_COUNT - 1 : DEFAULT_RETRY_COUNT;

	WaitForSingleObject(m_hSync, INFINITE);

	packetinfo.nSyncNo = GetNextSyncNo(iSlaveNo);

	nReturn = FuncCommand(&packetinfo);

	if (nReturn == PIEZO_RETURN::FMC_DISCONNECTED)
	{
		ClosePortWithoutSync();
	}

	//ReleaseSemaphore(m_hSync, 1, NULL);
	ReleaseMutex(m_hSync);

	return nReturn;
}

int CRs232CB::DoSendCommandNoResp(BYTE iSlaveNo, BYTE byCmd, LPVOID lpIN, DWORD dwINlen)
{
	PACKET_INFO_V8 packetinfo = {0};
	int nReturn;

	if (IsConnected() == FALSE)
		return PIEZO_RETURN::FMC_DISCONNECTED;

	packetinfo.iSlaveNo = iSlaveNo;
	packetinfo.byCmd = byCmd;

	packetinfo.lpIN = lpIN;
	packetinfo.dwINlen = dwINlen;

	WaitForSingleObject(m_hSync, INFINITE);

	if (iSlaveNo == 0x99)
	{
		packetinfo.nSyncNo = GetNextSyncNoAll();
	}
	else //if (iSlaveNo < MAX_SLAVE_NUMS)
	{
		packetinfo.nSyncNo = GetNextSyncNo(iSlaveNo);
	}
	//else
	//{
	//	ASSERT(FALSE);
	//	return FMM_INVALID_SLAVE_NUM;
	//}

	nReturn = FuncCommandNoResp(&packetinfo);

	if (nReturn == PIEZO_RETURN::FMC_DISCONNECTED)
	{
		ClosePortWithoutSync();
	}

	// insert small delay after sending packet.
	Sleep(1);

	//ReleaseSemaphore(m_hSync, 1, NULL);
	ReleaseMutex(m_hSync);

	return nReturn;
}

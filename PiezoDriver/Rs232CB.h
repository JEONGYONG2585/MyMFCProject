// Rs232CB.h: interface for the CRs232CB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RS232CB_H__D5E5A255_AAB6_4039_91D5_FB766028544D__INCLUDED_)
#define AFX_RS232CB_H__D5E5A255_AAB6_4039_91D5_FB766028544D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <Windows.h>
#include <tchar.h>

#include "COMM_Define.h"

static const int MAX_DOUBLE_BUFFER_SIZE	= MAX_BUFFER_SIZE * 2;
typedef struct _PACKET_INFO_V8
{
	BYTE	iSlaveNo;
	BYTE	nSyncNo;
	BYTE	byCmd;

	LPVOID	lpIN;
	DWORD	dwINlen;

	BYTE	nRtnCode;

	LPVOID	lpOUT;
	DWORD	dwOUTlen;

	DWORD	dwWaitTime;
	int		nRetryCount;

	BOOL	bIgnoreTimeOut;
} PACKET_INFO_V8;

static const int MAX_ALLOW_SLAVE_NUM	= 99;

class CRs232CB  
{
public:
	CRs232CB();
	virtual ~CRs232CB();

private:
	//--------- 환경 변수 -----------------------------------------//
	HANDLE	m_hComm;				// 통신 포트 파일 핸들
	BYTE	m_nPortNo;				// 포트 번호 
	DWORD	m_dwBaudrate;
	BOOL	m_bConnected;			// 포트가 열렸는지 유무를 나타냄.

	BYTE	m_buffRecvData[MAX_BUFFER_SIZE];	///< RecvPacket용 저장소
	UINT	m_nRecvCount;

	BYTE	m_buffSend[MAX_BUFFER_SIZE];		///< SendPacket용 저장소
	BYTE	m_buffSendModified[MAX_DOUBLE_BUFFER_SIZE];	///< SendPacket용 저장소 (0xAA 처리한 것)
	DWORD	m_nLenBuffSendModified;

	BYTE	m_nProtocolSyncByte[MAX_ALLOW_SLAVE_NUM];	// Protocol의 Sync를 위한 변수.

	HANDLE	m_hSync;

protected:
	//--------- 내부 사용 함수 ------------------------------------//
	void	ClosePortWithoutSync();

	BOOL	ReadCommOneByte(BYTE* by, BOOL* bError);

	BOOL	SendPacket(PACKET_INFO_V8* lpPacketInfo);
	int		RecvPacket(PACKET_INFO_V8* lpPacketInfo);
	int		AnalizePacket(PACKET_INFO_V8* lpPacketInfo, BOOL bPrintLog = TRUE);
	WORD	CalcCRC(BYTE* lpBuff, DWORD dwLen);

	BYTE	GetNextSyncNo(BYTE iSlaveNo);
	BYTE	GetNextSyncNoAll();

	int		FuncCommand(PACKET_INFO_V8* lpPacketInfo);
	int		FuncCommandNoResp(PACKET_INFO_V8* lpPacketInfo);

public:
	//--------- 외부 사용 함수 ------------------------------------//
	BOOL	OpenPort(BYTE nPortNo,
				DWORD dwBaud,
				BYTE nByteSize,		// 4 ~ 8
				BYTE nParity,		// 0 (None), 1 (Odd), 2 (Even), 3 (Mark), 4 (space)
				BYTE nStopBit		// 0 (1),  1 (1.5), 2 (2)
			);
	void	ClosePort();

	BOOL	WriteComm(BYTE *pBuff, DWORD nToWrite);

	BOOL	IsConnected() const	{return m_bConnected;}
	BYTE	GetPortNo() const	{return m_nPortNo;}
	DWORD	GetBaudrate() const	{return m_dwBaudrate;}

	// Protocol Version 8
	int		DoSendCommand(BYTE iSlaveNo, BYTE byCmd, LPVOID lpIN, DWORD dwINlen, LPVOID lpOUT, DWORD dwOUTlen, DWORD dwWaitTime = COMM_WAITTIME, BOOL bIgnoreTimeOut = FALSE);
	int		DoSendCommandNoResp(BYTE iSlaveNo, BYTE byCmd, LPVOID lpIN, DWORD dwINlen);

};

#define CRs232C		CRs232CB

#endif // !defined(AFX_RS232CB_H__D5E5A255_AAB6_4039_91D5_FB766028544D__INCLUDED_)

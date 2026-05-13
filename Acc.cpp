// Acc.cpp: implementation of the CAcc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Acc.h"

#include "fm_lib.h"
#include "error.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//  iBdNo : 보드가 꼽힌 순서. 스레드의 순서. 디바이스에서의 순서.
//  iBdID : 보드의 딥스위치의 번호.

CAcc::CAcc()
{
	m_bOpen = FALSE;

    m_abyBdNos[0] = 10;
    m_abyBdNos[1] = 10;
    m_abyBdNos[2] = 10;
    m_abyBdNos[3] = 10;
    m_abyBdNos[4] = 10;
    m_abyBdNos[5] = 10;
    m_abyBdNos[6] = 10;
    m_abyBdNos[7] = 10;

	m_sProcName.Empty();
}

CAcc::~CAcc()
{
	if (m_bOpen)
		Close();
}

BOOL CAcc::Open()
{
    int iBdID, iBdNo, nSlotno;

	char	lpFilePath[_MAX_PATH];
	char	fname[_MAX_FNAME];

	GetModuleFileName(NULL, lpFilePath, _MAX_PATH);
	_splitpath(lpFilePath, NULL, NULL, fname, NULL);

	if (strlen(fname) < 1)
		return FALSE;

	nSlotno = GetSlotNumber(fname);

	if (PciStart(nSlotno) == FALSE)
		return FALSE;

	m_bOpen = TRUE;
	AddSlotNumber(sFas.nSlotPos);

    for(iBdNo=0; iBdNo<MAX_CARD_NUMS; iBdNo++)
		m_abyBdNos[iBdNo] = 10;

    for(iBdNo=0; iBdNo<MAX_CARD_NUMS; iBdNo++)
    {
        iBdID = (int)PciGetCardIDFast(iBdNo);
        if (iBdID < MAX_CARD_NUMS) m_abyBdNos[iBdID] = (BYTE)iBdNo;
    }

	return TRUE;
}

BOOL CAcc::Close()
{
	PciEnd();

	RemoveSlotNumber();
	m_bOpen = FALSE;

	return TRUE;
}

void CAcc::SetWnd(HWND hWnd)
{
	PciSetWnd(hWnd);
}

void CAcc::SetInt(int iBdID)
{
    if (m_abyBdNos[iBdID] > MAX_CARD_NUMS)       //해당 보드가 없습니다.(Error)
	{
		ShowErr(
			iBdID,
			IDE_ERR_NONE_CARD_ID,
			"setInt=> There is NO Board what has this BdID."
		);
	}
	else
	{
		PciIntSet(m_abyBdNos[iBdID]);
	}
}

void CAcc::ClearInt(int iBdID)
{
    if (m_abyBdNos[iBdID] > MAX_CARD_NUMS) 
	{
		ShowErr(
			iBdID,
			IDE_ERR_NONE_CARD_ID,
			"ClearInt=> There is NO Board what has this BdID."
		);
	}
	else
	{
		PciIntClr(m_abyBdNos[iBdID]);
	}
}

BOOL CAcc::IsOpen()
{
	return m_bOpen; 
}

BOOL CAcc::IsExist(int iBdID)
{
    if (m_abyBdNos[iBdID] > MAX_CARD_NUMS) 
    {
        /*
		ShowErr(
			iBdID,
			IDE_ERR_NONE_CARD_ID,
			"IsExist=> There is NO Board what has this BdID."
		);
		*/
        return FALSE;
    }
	else 
	{
		return PciIsExist(m_abyBdNos[iBdID]);
	}
}

BYTE CAcc::GetCardID(int iBdNo)
{
    if (m_abyBdNos[iBdNo] > MAX_CARD_NUMS) 
    {
        ShowErr(
			iBdNo,
			IDE_ERR_NONE_CARD_ID,
			"GetCardID=> There is NO Board what has this BdID."
		);
        return (BYTE)-1;
    }
	else
	{
		return PciGetCardID(m_abyBdNos[iBdNo]);
	}
}

void CAcc::SetMsgQueued(BOOL bQueued)
{
	PciSetMsgQueued(bQueued);
}

BOOL CAcc::SetCmd(int iBdID, WORD wAxis, BYTE byCmd, BYTE byOffset)
{
	ResetEvent(sFas.sDev[m_abyBdNos[iBdID]].evReply);

    if (m_abyBdNos[iBdID] > MAX_CARD_NUMS) 
    {
        ShowErr(
			iBdID,
			IDE_ERR_NONE_CARD_ID,
			"SetCmd=> There is NO Board what has this BdID."
		);
        return FALSE;
    }

	/*
	PciWriteB(
	*/
	PciWrite(
		m_abyBdNos[iBdID],
		sFas.sDev[m_abyBdNos[iBdID]].InfoPci.cs[PCI_BASE_CS3] + FM_16000_INT_ADDRESS,
		byCmd
	);

	return TRUE;
}

int CAcc::WaitCmd(int iBdID, WORD wAxis, BYTE byCmd, DWORD dwmSec)
{
	PFAS_DEVICE	pDev;
	BYTE byTemp[8];
    DWORD dwRtn;

	pDev = &(sFas.sDev[m_abyBdNos[iBdID]]);
    
//	TRACE("%d Axis WaitCmd Start\n", wAxis);

    if (dwmSec <= 0 )
        dwRtn = WaitForSingleObject(pDev->evReply, INFINITE);
    else
        dwRtn = WaitForSingleObject(pDev->evReply, dwmSec);

    PciReadBlockDP(pDev->CardNum, 0, byTemp, 4);
	
//	TRACE("%d Axis WaitCmd End\n", wAxis);

	return dwRtn;
}

BYTE CAcc::GetReply(int iBdID)
{
    if (m_abyBdNos[iBdID] > MAX_CARD_NUMS) 
    {
        ShowErr(
			iBdID,
			IDE_ERR_NONE_CARD_ID,
			"GetReply=> There is NO Board what has this BdID."
		);
        return (BYTE)-1;
    }
	else
	{
	    return (BYTE)PciGetReplyType(m_abyBdNos[iBdID]);
	}
}

BYTE CAcc::GetMsg(int iBdID)
{
    if (m_abyBdNos[iBdID] > MAX_CARD_NUMS) 
    {
        ShowErr(
			iBdID,
			IDE_ERR_NONE_CARD_ID,
			"GetMsg=> There is NO Board what has this BdID."
		);
        return (BYTE)-1;
    }
	else
	{
	    return (BYTE)PciGetMsgType(m_abyBdNos[iBdID]);
	}
}

void CAcc::ClearLastErr()
{
	ClearErr();
}

//////////////////////////////////////////////////////////////////////

void CAcc::WriteRam(int iBdID, WORD wAxis, DWORD offset, LONG data)
{
    if (m_abyBdNos[iBdID] > MAX_CARD_NUMS) 
    {
        ShowErr(
			iBdID,
			IDE_ERR_NONE_CARD_ID,
			"WriteRam=> There is NO Board what has this BdID."
		);
        return;
    }

    if(offset < 0x200)
    {
        PciWriteDP(
			m_abyBdNos[iBdID],
			(offset << 2) + GetAxisBaseAddress(m_abyBdNos[iBdID], wAxis),
			data
		);
    }
    else
    {
        //Offset 주소가 허용범위를 넘어갔다는 에러를 발생시켜야 함.
		ShowErr(
			iBdID,
			IDE_ERR_OVER_OFFSET,
			"WriteRam()=> offset must lower than 0x100."
		);
    }

}

LONG CAcc::ReadRam(int iBdID, WORD wAxis, DWORD offset)
{
    if (m_abyBdNos[iBdID] > MAX_CARD_NUMS) 
    {
        ShowErr(
			iBdID,
			IDE_ERR_NONE_CARD_ID,
			"ReadRam=> There is NO Board what has this BdID."
		);
        return 0l;
    }

    if(offset < 0x200)
    {
        return PciReadDP(m_abyBdNos[iBdID], (offset << 2) + GetAxisBaseAddress(m_abyBdNos[iBdID], wAxis));
    }
    else
    {
        ShowErr(
			iBdID,
			IDE_ERR_OVER_OFFSET,
			"ReadRam()=> offset must lower than 0x100."
		);
        return 0l;
    }
}

DWORD CAcc::GetAxisBaseAddress(int iBdNo, DWORD dwAxis)     //추가 예정
{
    if (iBdNo > MAX_CARD_NUMS) 
    {
        ShowErr(
			iBdNo,
			IDE_ERR_NONE_CARD_ID,
			"GetAxisBaseAddress=> There is NO Board what has this BdID."
		);
        return 0;
    }

	return sFas.sDev[iBdNo].InfoPci.cs[PCI_BASE_CS3] + ((dwAxis < MAX_AXIS_NUMS) ? 0x0200 * dwAxis : 0x2000);
}

DWORD CAcc::GetAxisSimpleAddress(DWORD dwAxis)
{
	return (dwAxis < MAX_AXIS_NUMS) ? (0x0200 * dwAxis) : 0x2000;
}

int CAcc::GetAxisInfo(int iBdID)
{
    if (m_abyBdNos[iBdID] > MAX_CARD_NUMS) 
    {
        ShowErr(
			iBdID,
			IDE_ERR_NONE_CARD_ID,
			"GetAxisInfo=> There is NO Board what has this BdID."
		);
        return -1;
    }

    return PciGetAxisInfo(m_abyBdNos[iBdID]);
}

//////////////////////////////////////////////////////////////////////

void CAcc::ReadBlockRamB(int iBdID, int iAdd, BYTE *pbyData, DWORD dwLen)
{
	PciReadBlockDP(m_abyBdNos[iBdID], (iAdd << 2), pbyData,  dwLen);
}

void CAcc::ReadBlockRam(int iBdID, int iAdd, BYTE *pbyData, DWORD dwLen)
{
	PciReadBlock(m_abyBdNos[iBdID], (iAdd << 2), pbyData,  dwLen);
}

void CAcc::WriteBlockRamB(int iBdID, int iAdd, BYTE *pbyData, DWORD dwLen)
{
	PciWriteBlockDP(m_abyBdNos[iBdID], (iAdd << 2), pbyData,  dwLen);
}

int CAcc::GetBdNo(int iBdID)
{
    int iBdNo;
    for(iBdNo=0; iBdNo<MAX_CARD_NUMS; iBdNo++)
    {
        if (PciIsExist(iBdNo))
            if (PciGetCardIDFast(iBdNo) == (BYTE)iBdID) return iBdNo;
    }

    return -1;
}

void CAcc::ShakeDSP(int iBdID)
{
	BYTE tmpBuff[4];

//	TRACE("Shake DSP\n");
	PciReadBlockDP(GetBdNo(iBdID), 0, tmpBuff, 4);
}

int CAcc::GetSlotNumber(LPCTSTR lpName)
{
	HKEY hRegKey;
	DWORD dwType, nSlotNo, dwRecvLen;
	LONG nRtn;

	m_sProcName = lpName;
	nSlotNo = 0;

	// Key를 여기서 만들 필요는 없다.
	nRtn = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGPATH_PROGRAMUSE, 0, KEY_ALL_ACCESS, &hRegKey);
	if (nRtn != ERROR_SUCCESS)
		return nSlotNo;

	nRtn = RegQueryValueEx(hRegKey, m_sProcName, NULL, &dwType, (LPBYTE)&nSlotNo, &dwRecvLen);
	if (nRtn != ERROR_SUCCESS)	// 있으면 그대로 사용하지만, 없으면 새로 하나 만들어야 한다.
		nSlotNo = 0;

	RegCloseKey(hRegKey);

	return nSlotNo;
}

void CAcc::AddSlotNumber(int nSlotNo)
{
	HKEY hRegKey;
	DWORD dwDisp;
	LONG nRtn;

	nRtn = RegCreateKeyEx(HKEY_LOCAL_MACHINE, REGPATH_PROGRAM, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hRegKey, &dwDisp);
	if (nRtn != ERROR_SUCCESS)
		return;

	// USE 키는 리부팅 시 없어져야 한다. 그래서 VOLATILE형이다.
	nRtn = RegCreateKeyEx(hRegKey, REGPATH_USE, 0, NULL, REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &hRegKey, &dwDisp);
	if (nRtn != ERROR_SUCCESS)
	{
		RegCloseKey(hRegKey);
		return;
	}

	RegSetValueEx(hRegKey, m_sProcName, 0, REG_DWORD, (LPBYTE)(&nSlotNo), sizeof(DWORD));
	RegCloseKey(hRegKey);
}

void CAcc::RemoveSlotNumber()
{
	HKEY hRegKey;
	LONG nRtn;

	nRtn = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGPATH_PROGRAMUSE, 0, KEY_ALL_ACCESS, &hRegKey);
	if (nRtn == ERROR_SUCCESS)
	{
		RegDeleteValue(hRegKey, m_sProcName);
		RegCloseKey(hRegKey);
	}

	m_sProcName.Empty();
}
// Acc.h: interface for the CAcc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACC_H__C4906C87_AAC5_4A27_AF1A_8487707C307E__INCLUDED_)
#define AFX_ACC_H__C4906C87_AAC5_4A27_AF1A_8487707C307E__INCLUDED_

#include "PCI_IO.H"	// Added by ClassView

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAcc  
{
public:
	CAcc();
	virtual	~CAcc();

	BOOL	Open();
	BOOL	Close();

	BOOL	IsOpen();
	BOOL	IsExist(int iBdID);
	BYTE	GetCardID(int iBdNo);

	void	ClearInt(int iBdID);
	void	SetInt(int iBdID);
	int		GetBdNo(int iBdID);

    void	GetLastErr(int* iBdID, int* iCode);
	void	ClearLastErr();

	BOOL	SetCmd(int iBdID, WORD wAxis, BYTE byCmd, BYTE byOffset);
	int	WaitCmd(int iBdID, WORD wAxis, BYTE byCmd, DWORD dwmSec);

	//		Read Part..
	LONG	ReadRam(int iBdID, WORD wAxis, DWORD offset);
	void	ReadBlockRamB(int iBdID, int iAdd, BYTE *pbyData, DWORD dwLen);
	void	ReadBlockRam(int iBdID, int iAdd, BYTE *pbyData, DWORD dwLen);
	
	//		Write Part..
	void	WriteRam(int iBdID, WORD wAxis, DWORD offset, LONG data);
    void	WriteBlockRamB(int iBdID, int iAdd, BYTE *pbyData, DWORD dwLen);

	//		Axis Infomation...
	DWORD	GetAxisBaseAddress(int iBdNo, DWORD dwAxis);
    DWORD	GetAxisSimpleAddress(DWORD dwAxis);
    int		GetAxisInfo(int iBdID);
    

//////////////////////////////////////////////////////////////////////
/////						Hidden function	
	BYTE	GetMsg(int iBdID);
	BYTE	GetReply(int iBdID);
	void	SetWnd( HWND hWnd);	
	void	SetMsgQueued(BOOL bQueued);

	void	ShakeDSP(int iBdID);

	int		GetSlotNumber(LPCTSTR lpName);
	void	AddSlotNumber(int nSlotNo);
	void	RemoveSlotNumber();
	
protected:
	BOOL	m_bOpen;
	CString	m_sProcName;

    BYTE	m_abyBdNos[MAX_CARD_NUMS];     //m_abyBdIDs[딥스위치 번호] = 카드의 순서.(0~)
};

#endif // !defined(AFX_ACC_H__C4906C87_AAC5_4A27_AF1A_8487707C307E__INCLUDED_)

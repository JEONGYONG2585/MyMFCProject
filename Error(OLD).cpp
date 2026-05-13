////////////////////////////////////////////////////////////////////////////
//
//      (C) 2002  Fas Technology Co., Ltd. 
//		FILENAME:		Error.h
//		DESCRIPTION:	FM_lib
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <winioctl.h>
#include <io.h>
#include <process.h>

#include "error.h"

int iLastErrID =0;
int iLastErrCode =0;


//iBdNo == 100 All card,
//iBdNo range 0 to 7,
void	ShowErr(int iBdNo, int iErr, char *str)
{
	SetErr( iBdNo, iErr);

#ifdef _DEBUG
//	char chMsg[255];
//	sprintf(chMsg, "Card(%d) = Code(%d) : %s", iBdNo, iErr, str); 
//	AfxMessageBox(chMsg);
#endif	//_DEBUG

}

void SetErr( int iBdNo, int iCode)
{
	iLastErrID = iBdNo;
	iLastErrCode = iCode;
}

/*
void ClearErr(int iBdNo)
{
	iLastErrID = 0;
	iLastErrCode = 0;
}
*/

void ClearErr()
{
	iLastErrID = 0;
	iLastErrCode = 0;
}

void GetErr( int* piBdNo, int* piCode)
{
	*piBdNo = iLastErrID ;
	*piCode = iLastErrCode;
}

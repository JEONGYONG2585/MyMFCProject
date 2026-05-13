////////////////////////////////////////////////////////////////////////////
//
//      (C) 2002  Fas Technology Co., Ltd. 
//		FILENAME:		Error.cpp
//		DESCRIPTION:	FM_lib 
//
/////////////////////////////////////////////////////////////////////////////

#ifndef		__ERROR_H__
#define		__ERROR_H__

#include "err_def.h"

void ShowErr(int iCard, int iErr, char *str);
void SetErr( int iCard, int iCode);
//void ClearErr(int iCard);
void ClearErr();
void GetErr( int* piCard, int* piCode);

#endif //__ERROR_H__
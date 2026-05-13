// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__7ADE471A_F971_43AE_8616_EDAE87992793__INCLUDED_)
#define AFX_STDAFX_H__7ADE471A_F971_43AE_8616_EDAE87992793__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxmt.h>
#include <afxctl.h>
//
#include <direct.h>
#include <fstream.h>
#include <math.h>
//
// FTP ฐüทร
#include <afxsock.h>
#include <afxinet.h>
//
#include "Color.h"
#include "Common.h"
#include "iodef.h"
#include "Lclib.h"
//
#include "ThreadData.h"
#include "FM_16000x.h"
#include "FM_COUNTERx.h"

#include "MilGrabber.h"

extern CMilGrabber grabber;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7ADE471A_F971_43AE_8616_EDAE87992793__INCLUDED_)

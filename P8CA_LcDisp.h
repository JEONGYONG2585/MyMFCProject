// P8CA_LcDisp.h : main header file for the P8CA_LcDisp application
//

#if !defined(AFX_P8CA_LcDisp_H__E19CA876_E87C_40A0_94FC_D1519F527F15__INCLUDED_)
#define AFX_P8CA_LcDisp_H__E19CA876_E87C_40A0_94FC_D1519F527F15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CP8CA_LcDispApp:
// See P8CA_LcDisp.cpp for the implementation of this class
//

class CP8CA_LcDispApp : public CWinApp
{
public:
	CP8CA_LcDispApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CP8CA_LcDispApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CP8CA_LcDispApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_P8CA_LcDisp_H__E19CA876_E87C_40A0_94FC_D1519F527F15__INCLUDED_)

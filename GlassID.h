//{{AFX_INCLUDES()
#include "msflexgrid.h"
#include "commandbutton.h"
//}}AFX_INCLUDES
#if !defined(AFX_GLASSID_H__9B41069E_6E35_450E_9717_C0F5950FC339__INCLUDED_)
#define AFX_GLASSID_H__9B41069E_6E35_450E_9717_C0F5950FC339__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GlassID.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGlassID dialog

class CGlassID : public CDialog
{
// Construction
public:
	CGlassID(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGlassID();

// Dialog Data
	//{{AFX_DATA(CGlassID)
	enum { IDD = IDD_GLASS_ID };
	CMSFlexGrid	m_ctrlGridGlsInformSelf;
	CMSFlexGrid	m_ctrlGridGlsInformPair;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGlassID)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGlassID)
	virtual BOOL OnInitDialog();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLASSID_H__9B41069E_6E35_450E_9717_C0F5950FC339__INCLUDED_)

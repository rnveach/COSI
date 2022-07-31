// COSI.h : main header file for the COSI application
//

#if !defined(AFX_COSI_H__D28E68E8_BE84_46E7_BF47_CADB67C09F1E__INCLUDED_)
#define AFX_COSI_H__D28E68E8_BE84_46E7_BF47_CADB67C09F1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCOSIApp:
// See COSI.cpp for the implementation of this class
//

class CCOSIApp : public CWinApp
{
public:
	CCOSIApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCOSIApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCOSIApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COSI_H__D28E68E8_BE84_46E7_BF47_CADB67C09F1E__INCLUDED_)

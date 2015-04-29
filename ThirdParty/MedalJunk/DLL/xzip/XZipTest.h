// XZipTest.h : main header file for the XZIPTEST application
//

#ifndef XZIPTEST_H
#define XZIPTEST_H

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CXZipTestApp:
// See XZipTest.cpp for the implementation of this class
//

class CXZipTestApp : public CWinApp
{
public:
	CXZipTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXZipTestApp)
public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CXZipTestApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //XZIPTEST_H

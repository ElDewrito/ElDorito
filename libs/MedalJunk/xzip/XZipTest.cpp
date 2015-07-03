// XZipTest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "XZipTest.h"
#include "XZipTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXZipTestApp

BEGIN_MESSAGE_MAP(CXZipTestApp, CWinApp)
	//{{AFX_MSG_MAP(CXZipTestApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXZipTestApp construction

CXZipTestApp::CXZipTestApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CXZipTestApp object

CXZipTestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CXZipTestApp initialization

BOOL CXZipTestApp::InitInstance()
{
#if _MFC_VER < 0x700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	CXZipTestDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}

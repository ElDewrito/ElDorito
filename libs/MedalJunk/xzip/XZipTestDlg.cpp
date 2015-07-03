// XZipTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "XZipTest.h"
#include "XZipTestDlg.h"
#include "about.h"
#include <io.h>
#include "XZip.h"
#include "XUnzip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(disable : 4996)	// disable bogus deprecation warning

#define TEXT_LINE    _T("This is line %5d\r\n")
#define TEXT_LINE1   _T("This is line     1\r\n")
#define IsValidFileHandle(x)	((x) && ((x) != INVALID_HANDLE_VALUE))

///////////////////////////////////////////////////////////////////////////////
// CXZipTestDlg dialog

BEGIN_MESSAGE_MAP(CXZipTestDlg, CDialog)
	//{{AFX_MSG_MAP(CXZipTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_TEST, OnTest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// ctor
CXZipTestDlg::CXZipTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXZipTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CXZipTestDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

///////////////////////////////////////////////////////////////////////////////
// DoDataExchange
void CXZipTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXZipTestDlg)
	DDX_Control(pDX, IDC_LIST, m_List);
	//}}AFX_DATA_MAP
}

///////////////////////////////////////////////////////////////////////////////
// CXZipTestDlg message handlers

///////////////////////////////////////////////////////////////////////////////
// OnInitDialog
BOOL CXZipTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_List.SetContextMenuId(IDR_XLISTBOX);
	m_List.AddLine(CXListBox::Blue, CXListBox::White, _T("XZipTest v1.3"));
	m_List.AddLine(CXListBox::Blue, CXListBox::White, _T(""));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

///////////////////////////////////////////////////////////////////////////////
// OnSysCommand
void CXZipTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
// If you add a minimize button to your dialog, you will need the code below
// to draw the icon.  For MFC applications using the document/view model,
// this is automatically done for you by the framework.
void CXZipTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnQueryDragIcon
// The system calls this to obtain the cursor to display while the user drags
// the minimized window.
HCURSOR CXZipTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

///////////////////////////////////////////////////////////////////////////////
// OnTest
void CXZipTestDlg::OnTest() 
{
#ifdef _UNICODE
	ZIPENTRYW ze;
#else
	ZIPENTRY ze; 
#endif

	memset(&ze, 0, sizeof(ze));
	TCHAR * pszArchive1 = _T("_TestZipTmp1.zip");
	TCHAR * pszArchive2 = _T("_TestZipTmp2.zip");

	// text files
	TCHAR * pszName1    = _T("_TestZipTmp1.txt");
	TCHAR * pszName2    = _T("_TestZipTmp2.txt");
	TCHAR * pszName3    = _T("_TestZipTmp3.txt");

	// binary files
	TCHAR * pszName4    = _T("_TestZipTmp4.bin");
	TCHAR * pszName5    = _T("_TestZipTmp5.bin");
	TCHAR * pszName6    = _T("_TestZipTmp6.bin");
	TCHAR * pszName7    = _T("_TestZipTmp7.bin");
	TCHAR * pszName8    = _T("_TestZipTmp8.bin");
	TCHAR * pszName9    = _T("_TestZipTmp9.bin");

	// delete zip files
	::DeleteFile(pszArchive1);
	::DeleteFile(pszArchive2);

	// create .txt files
	CreateTextFile(pszName1, 10000);
	CreateTextFile(pszName2, 100);
	CreateTextFile(pszName3, 10);

	// create .bin files
	CreateBinaryFile(pszName4, 16384);
	CreateBinaryFile(pszName5, 3*16384);
	CreateBinaryFile(pszName6, 8*16384);
	CreateBinaryFile(pszName7, 123);
	CreateBinaryFile(pszName8, 17000);
	CreateBinaryFile(pszName9, 8*16384+1);

	///////////////////////////////////////////////////////////////////////////
	// single-file zip

	m_List.AddLine(CXListBox::Blue, CXListBox::White, _T(""));

	m_List.Printf(CXListBox::Navy, CXListBox::White, 0, 
		_T("    === Testing single-file zip ==="));

	BOOL bRet = Zip(pszArchive1, pszName1);

	if (bRet)
	{
		m_List.Printf(CXListBox::Green, CXListBox::White, 0, 
			_T("    Zip archive created OK"));

		m_List.Printf(CXListBox::Black, CXListBox::White, 0, 
			_T("    === Testing unzip ==="));

		HZIP hz = OpenZip(pszArchive1, 0, ZIP_FILENAME);

		if (hz)
		{
			ZRESULT zr = GetZipItem(hz, -1, &ze); 

			if (zr == ZR_OK)
			{
				int numitems = ze.index;

				if (numitems == 1)
				{
					m_List.Printf(CXListBox::Green, CXListBox::White, 0, 
						_T("    Zip archive contains 1 file ==> OK"));

					VerifyZip(hz, pszName1);

					CloseZip(hz);
				}
				else
				{
					m_List.Printf(CXListBox::Red, CXListBox::White, 0, 
						_T("    Zip contents bad"));
				}
			}
			else
			{
				m_List.Printf(CXListBox::Red, CXListBox::White, 0, 
					_T("    GetZipItem failed"));
			}
		}
		else
		{
			m_List.Printf(CXListBox::Red, CXListBox::White, 0, 
				_T("   Failed to open zip file '%s'"), pszArchive1);
		}
	}
	else
	{
		m_List.Printf(CXListBox::Red, CXListBox::White, 0, 
			_T("    Failed to create zip"));
	}


	///////////////////////////////////////////////////////////////////////////
	// multi-file zip

	m_List.AddLine(CXListBox::Blue, CXListBox::White, _T(""));

	m_List.Printf(CXListBox::Navy, CXListBox::White, 0, 
		_T("    === Testing multi-file zip ==="));

	HZIP hz = CreateZip(pszArchive2, 0, ZIP_FILENAME);

	if (hz)
	{
		m_List.Printf(CXListBox::Green, CXListBox::White, 0, 
			_T("    Zip archive created OK"));

		if ((ZipAdd(hz, pszName1,  pszName1, 0, ZIP_FILENAME) == ZR_OK) &&
			(ZipAdd(hz, pszName2,  pszName2, 0, ZIP_FILENAME) == ZR_OK) &&
			(ZipAdd(hz, pszName3,  pszName3, 0, ZIP_FILENAME) == ZR_OK) &&
			(ZipAdd(hz, pszName4,  pszName4, 0, ZIP_FILENAME) == ZR_OK) &&
			(ZipAdd(hz, pszName5,  pszName5, 0, ZIP_FILENAME) == ZR_OK) &&
			(ZipAdd(hz, pszName6,  pszName6, 0, ZIP_FILENAME) == ZR_OK) &&
			(ZipAdd(hz, pszName7,  pszName7, 0, ZIP_FILENAME) == ZR_OK) &&
			(ZipAdd(hz, pszName8,  pszName8, 0, ZIP_FILENAME) == ZR_OK) &&
			(ZipAdd(hz, pszName9,  pszName9, 0, ZIP_FILENAME) == ZR_OK))
		{
			m_List.Printf(CXListBox::Green, CXListBox::White, 0, 
				_T("    Files added to zip archive OK"));

			CloseZip(hz);

			hz = OpenZip(pszArchive2, 0, ZIP_FILENAME);

			if (hz)
			{
				m_List.Printf(CXListBox::Green, CXListBox::White, 0, 
					_T("    Zip archive opened OK"));

				GetZipItem(hz, -1, &ze); 
				int numitems = ze.index;

				if (numitems == 9)
				{
					m_List.Printf(CXListBox::Green, CXListBox::White, 0, 
						_T("    Zip archive contains correct number of entries"));


					for (int i = 0; i < numitems; i++)
					{ 
						GetZipItem(hz, i, &ze);
						m_List.Printf(CXListBox::Black, CXListBox::White, 0, 
							_T("        %d:  %s"), i, ze.name);
					}

					VerifyZip(hz, pszName1);
					VerifyZip(hz, pszName2);
					VerifyZip(hz, pszName3);
					VerifyZip(hz, pszName4);
					VerifyZip(hz, pszName5);
					VerifyZip(hz, pszName6);
					VerifyZip(hz, pszName7);
					VerifyZip(hz, pszName8);
					VerifyZip(hz, pszName9);
				}
				else
				{
					m_List.Printf(CXListBox::Red, CXListBox::White, 0, 
						_T("    Number of entries in zip archive is incorrect"));
				}
			}
			else
			{
				m_List.Printf(CXListBox::Red, CXListBox::White, 0, 
					_T("    Failed to open zip archive"));
			}
		}
		else
		{
			m_List.Printf(CXListBox::Red, CXListBox::White, 0, 
				_T("    Failed to add file to zip archive"));
		}
	}
	else
	{
		m_List.Printf(CXListBox::Red, CXListBox::White, 0, 
			_T("    Failed to create zip archive"));
	}

	CloseZip(hz);

	// the files are not deleted - you can inspect them after running test

	//::DeleteFile(pszArchive1);
	//::DeleteFile(pszArchive2);
	//::DeleteFile(pszName1);
	//::DeleteFile(pszName2);
	//::DeleteFile(pszName3);

	m_List.AddLine(CXListBox::Blue, CXListBox::White, _T(""));
}

///////////////////////////////////////////////////////////////////////////////
// Zip
BOOL CXZipTestDlg::Zip(LPCTSTR lpszZipArchive, LPCTSTR lpszSrcFile)
{
	BOOL bResult = TRUE;

	_ASSERTE(lpszZipArchive);
	_ASSERTE(lpszZipArchive[0] != _T('\0'));

	if (!lpszZipArchive || lpszZipArchive[0] == _T('\0'))
		return FALSE;

	_ASSERTE(lpszSrcFile);
	_ASSERTE(lpszSrcFile[0] != _T('\0'));

	if (!lpszSrcFile || lpszSrcFile[0] == _T('\0'))
		return FALSE;

	// does zip source file exist?
	if (_taccess(lpszSrcFile, 04) != 0)
	{
		TRACE(_T("WARNING: zip source file '%s' cannot be found, operation aborted\n"),
			lpszSrcFile);
		return FALSE;
	}

	// use only the file name for zip file entry
	TCHAR * cp = (TCHAR *)_tcsrchr(lpszSrcFile, _T('\\'));
	if (cp == NULL)
		cp = (TCHAR *) lpszSrcFile;
	else
		cp++;

	HZIP hz = CreateZip((void *)lpszZipArchive, 0, ZIP_FILENAME);

	if (hz)
	{
		ZRESULT zr = ZipAdd(hz, cp, (void *)lpszSrcFile, 0, ZIP_FILENAME);

		CloseZip(hz);

		// did add work?
		if (zr == ZR_OK)
		{
			TRACE(_T("added '%s' to zip file '%s'\n"),
				lpszSrcFile, lpszZipArchive);

			bResult = TRUE;
		}
		else
		{
			TRACE(_T("WARNING: failed to add zip source file '%s'\n"),
				lpszSrcFile);
			bResult = FALSE;
		}
	}
	else
	{
		TRACE(_T("ERROR: failed to create zip file '%s'\n"), 
			lpszZipArchive);
		bResult = FALSE;
	}

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////
// VerifyZip
void CXZipTestDlg::VerifyZip(HZIP hz, LPCTSTR lpszFile)
{
#ifdef _UNICODE
	ZIPENTRYW ze;
#else
	ZIPENTRY ze; 
#endif

	memset(&ze, 0, sizeof(ze));
	int index = -1;
	ZRESULT zr = 0;

	zr = FindZipItem(hz, lpszFile, TRUE, &index, &ze);
	TRACE(_T("index=%d\n"), index);

	m_List.Printf(CXListBox::Black, CXListBox::White, 0, 
		_T("    === Checking contents of zip entry %s ==="), lpszFile);

	if (zr == ZR_OK)
		m_List.Printf(CXListBox::Green, CXListBox::White, 0, 
			_T("    FindZipItem returned OK"));
	else
		m_List.Printf(CXListBox::Red, CXListBox::White, 0, 
			_T("    FindZipItem failed"));

	if (_tcscmp(lpszFile, ze.name) == 0)
		m_List.Printf(CXListBox::Green, CXListBox::White, 0, 
			_T("    FindZipItem found name ==> OK"));
	else
		m_List.Printf(CXListBox::Red, CXListBox::White, 0, 
			_T("    FindZipItem failed to find name"));

	TCHAR targetname[MAX_PATH];
	_tcscpy(targetname, _T("_unzip"));
	_tcscat(targetname, lpszFile);

	// delete target file if it exists
	::DeleteFile(targetname);

	zr = UnzipItem(hz, index, targetname, 0, ZIP_FILENAME);

	if (zr == ZR_OK)
		m_List.Printf(CXListBox::Green, CXListBox::White, 0, 
			_T("    UnzipItem returned OK"));
	else
		m_List.Printf(CXListBox::Red, CXListBox::White, 0, 
			_T("    UnzipItem failed"));

	if (_taccess(targetname, 04) == 0)
		m_List.Printf(CXListBox::Green, CXListBox::White, 0, 
			_T("    Target file created OK"));
	else
		m_List.Printf(CXListBox::Red, CXListBox::White, 0, 
			_T("    UnzipItem failed to create target file"));

	BOOL bResult = FALSE;
	BOOL bRet = Compare(lpszFile, targetname, &bResult);

	if (bRet && bResult)
		m_List.Printf(CXListBox::Green, CXListBox::White, 0, 
			_T("    Target file matches original file ==> OK"));
	else
		m_List.Printf(CXListBox::Red, CXListBox::White, 0, 
			_T("    Target file does not match original file"));

	// the target file is not deleted - you can inspect it after running test

	//::DeleteFile(targetname);
}

#define COMPARE_BUF_SIZE (64*1024)

///////////////////////////////////////////////////////////////////////////////
// Compare
BOOL CXZipTestDlg::Compare(LPCTSTR lpszFile1, LPCTSTR lpszFile2, BOOL *pbResult)
{
	_ASSERTE(pbResult);
	if (!pbResult)
		return FALSE;

	*pbResult = FALSE;

	BOOL bCompare = FALSE;		// TRUE = files identical
								// FALSE = files not identical
	BOOL bOp = TRUE;			// TRUE = no API failures, compare completed
								// FALSE = file or memory API failed

	_ASSERTE(lpszFile1);
	_ASSERTE(lpszFile1[0] != _T('\0'));

	if (!lpszFile1 || lpszFile1[0] == _T('\0'))
		return FALSE;

	_ASSERTE(lpszFile2);
	_ASSERTE(lpszFile2[0] != _T('\0'));

	if (!lpszFile2 || lpszFile2[0] == _T('\0'))
		return FALSE;

	HANDLE hFile1 = NULL, hFile2 = NULL;

	TRACE(_T("opening '%s'"), lpszFile1);

	// open file 1
	hFile1 = ::CreateFile(lpszFile1,
						  GENERIC_READ,
						  FILE_SHARE_READ | FILE_SHARE_WRITE,
						  NULL,
						  OPEN_EXISTING,
						  FILE_ATTRIBUTE_NORMAL,
						  NULL);

	if (hFile1 == INVALID_HANDLE_VALUE)
	{
		TRACE(_T("ERROR: %s failed\n"), _T("CreateFile"));
		return FALSE;
	}

	TRACE(_T("opening '%s'"), lpszFile2);

	// open file 2
	hFile2 = ::CreateFile(lpszFile2,
						  GENERIC_READ,
						  FILE_SHARE_READ | FILE_SHARE_WRITE,
						  NULL,
						  OPEN_EXISTING,
						  FILE_ATTRIBUTE_NORMAL,
						  NULL);

	if (hFile2 == INVALID_HANDLE_VALUE)
	{
		TRACE(_T("ERROR: %s failed\n"), _T("CreateFile"));
		::CloseHandle(hFile1);
		return FALSE;
	}

	DWORD dwFileSize1 = ::GetFileSize(hFile1, NULL);
	DWORD dwFileSize2 = ::GetFileSize(hFile2, NULL);

	if ((dwFileSize1 != INVALID_FILE_SIZE) && (dwFileSize2 != INVALID_FILE_SIZE))
	{
		// continue if file sizes match
		if (dwFileSize1 == dwFileSize2)
		{
			BYTE * pBuf1 = new BYTE [COMPARE_BUF_SIZE];
			_ASSERTE(pBuf1);

			BYTE * pBuf2 = new BYTE [COMPARE_BUF_SIZE];
			_ASSERTE(pBuf2);

			if (pBuf1 && pBuf2)
			{
				while (dwFileSize1)
				{
					DWORD dwBytesRead1 = 0;
					BOOL bRet1 = ::ReadFile(hFile1, 
											(LPVOID) pBuf1, 
											COMPARE_BUF_SIZE, 
											&dwBytesRead1, 
											NULL);

					if (!bRet1)
					{
						TRACE(_T("ERROR: %s failed\n"), _T("ReadFile"));
						bOp = FALSE;
						break;
					}

					DWORD dwBytesRead2 = 0;
					BOOL bRet2 = ::ReadFile(hFile2, 
											(LPVOID) pBuf2, 
											COMPARE_BUF_SIZE, 
											&dwBytesRead2, 
											NULL);

					if (!bRet2)
					{
						TRACE(_T("ERROR: %s failed\n"), _T("ReadFile"));
						bOp = FALSE;
						break;
					}

					if (dwBytesRead1 != dwBytesRead2)
					{
						TRACE(_T("Compare failed ==> file Read sizes different\n"));
						break;
					}

					if (dwBytesRead1 == 0)
					{
						// Read ok, but nothing read
						TRACE(_T("Read %s ==> EOF reached.\n"), lpszFile1);
						bCompare = TRUE;
						break;
					}

					if (dwBytesRead2 == 0)
					{
						// Read ok, but nothing read
						TRACE(_T("Read %s ==> EOF reached.\n"), lpszFile2);
						bCompare = TRUE;
						break;
					}

					// do contents match?
					int nCmp = memcmp(pBuf1, pBuf2, dwBytesRead1);
					if (nCmp != 0)
					{
						TRACE(_T("Compare failed ==> file contents different\n"));
						break;
					}

					dwFileSize1 -= dwBytesRead1;

				} // while (dwFileSize1)

				if (dwFileSize1 == 0)					
					bCompare = TRUE;

				if (pBuf1)
					delete [] pBuf1;
				if (pBuf2)
					delete [] pBuf2;
			}
			else
			{
				// memory allocation failed
				TRACE(_T("ERROR: memory allocation failure\n"));
				bOp = FALSE;
			}
		}
		else
		{
			TRACE(_T("Compare failed ==> file sizes different\n"));
		}
	}
	else
	{
		// GetFileSize failed
		TRACE(_T("ERROR: %s failed\n"), _T("GetFileSize"));
		bOp = FALSE;
	}

	if (IsValidFileHandle(hFile1))
		::CloseHandle(hFile1);
	if (IsValidFileHandle(hFile2))
		::CloseHandle(hFile2);

	*pbResult = bCompare;

	return bOp;
}

///////////////////////////////////////////////////////////////////////////////
// CreateTextFile
void CXZipTestDlg::CreateTextFile(LPCTSTR lpszFile, int nLines)
{
	m_List.Printf(CXListBox::Black, CXListBox::White, 0, 
		_T("    Creating text file '%s' with %d lines"), lpszFile, nLines);

	// open existing or create new
	HANDLE hFile = NULL;
	hFile = ::CreateFile(lpszFile,
						 GENERIC_READ | GENERIC_WRITE,
						 FILE_SHARE_READ | FILE_SHARE_WRITE,
						 NULL,
						 CREATE_ALWAYS,
						 FILE_ATTRIBUTE_NORMAL,
						 NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return;

	TCHAR s[100];
	BOOL bRet = FALSE;

	for (int i = 0; i < nLines; i++)
	{
		_stprintf(s, TEXT_LINE, i+1);

		DWORD dwBytesWritten = 0;

		int n = _tcslen(s)*sizeof(TCHAR);

		bRet = ::WriteFile(hFile,
						   s,
						   n,
						   &dwBytesWritten,
						   NULL);

		if (!bRet)
		{
			TRACE(_T("ERROR: %s failed\n"), _T("WriteFile"));
			m_List.Printf(CXListBox::Red, CXListBox::White, 0, 
				_T("    ERROR: file write failed"));
			return;
		}
	}

	DWORD dwEOF = ::GetFileSize(hFile, NULL);

	DWORD dwSize = nLines * _tcslen(TEXT_LINE1) * sizeof(TCHAR);

	if (dwEOF == dwSize)
		m_List.Printf(CXListBox::Green, CXListBox::White, 0, 
			_T("    File size matches size written ==> file write OK"));
	else
		m_List.Printf(CXListBox::Red, CXListBox::White, 0, 
			_T("    File size does not match size written ==> file write failed"));

	::CloseHandle(hFile);
}

///////////////////////////////////////////////////////////////////////////////
// CreateBinaryFile
void CXZipTestDlg::CreateBinaryFile(LPCTSTR lpszFile, int nBytes)
{
	m_List.Printf(CXListBox::Black, CXListBox::White, 0, 
		_T("    Creating binary file '%s' with %d bytes"), lpszFile, nBytes);

	// open existing or create new
	HANDLE hFile = NULL;
	hFile = ::CreateFile(lpszFile,
						 GENERIC_READ | GENERIC_WRITE,
						 FILE_SHARE_READ | FILE_SHARE_WRITE,
						 NULL,
						 CREATE_ALWAYS,
						 FILE_ATTRIBUTE_NORMAL,
						 NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return;

	BYTE *buf = new BYTE [nBytes];

	for (int i = 0; i < nBytes; )
	{
		for (BYTE j = 0; j < 256; j++)
		{
			buf[i++] = j;
			if (i >= nBytes)
				break;
		}
	}

	BOOL bRet = FALSE;

	DWORD dwBytesWritten = 0;

	bRet = ::WriteFile(hFile,
					   buf,
					   nBytes,
					   &dwBytesWritten,
					   NULL);

	if (!bRet)
	{
		TRACE(_T("ERROR: %s failed\n"), _T("WriteFile"));
		m_List.Printf(CXListBox::Red, CXListBox::White, 0, 
			_T("    ERROR: file write failed"));
		return;
	}

	DWORD dwEOF = ::GetFileSize(hFile, NULL);

	if (dwEOF == (DWORD) nBytes)
		m_List.Printf(CXListBox::Green, CXListBox::White, 0, 
			_T("    File size matches size written ==> file write OK"));
	else
		m_List.Printf(CXListBox::Red, CXListBox::White, 0, 
			_T("    File size does not match size written ==> file write failed"));

	::CloseHandle(hFile);

	if (buf)
		delete [] buf;
}

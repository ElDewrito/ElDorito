// XZipTestDlg.h : header file
//

#ifndef XZIPTESTDLG_H
#define XZIPTESTDLG_H

#include "XListBox.h"
#include "XZip.h"

/////////////////////////////////////////////////////////////////////////////
// CXZipTestDlg dialog

class CXZipTestDlg : public CDialog
{
// Construction
public:
	CXZipTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CXZipTestDlg)
	enum { IDD = IDD_XZIPTEST_DIALOG };
	CXListBox	m_List;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXZipTestDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	BOOL Compare(LPCTSTR lpszFile1, LPCTSTR lpszFile2, BOOL *pbResult);
	void CreateBinaryFile(LPCTSTR lpszFile, int nBytes);
	void CreateTextFile(LPCTSTR lpszFile, int nLines);
	void VerifyZip(HZIP hz, LPCTSTR lpszFile);
	BOOL Zip(LPCTSTR lpszZipArchive, LPCTSTR lpszSrcFile);

	// Generated message map functions
	//{{AFX_MSG(CXZipTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //XZIPTESTDLG_H

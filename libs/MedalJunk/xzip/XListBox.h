// XListBox.h
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// This software is released into the public domain.
// You are free to use it in any way you like.
//
// This software is provided "as is" with no expressed
// or implied warranty.  I accept no liability for any
// damage or loss of business that this software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef XLISTBOX_H
#define XLISTBOX_H

///////////////////////////////////////////////////////////////////////////////
//  CXListBox class

class CXListBox : public CListBox
{
// Constructors
public:
	CXListBox();
	~CXListBox();

// Attributes
public:
	int				m_cxExtent;
	int				m_nTabPosition;
	BOOL			m_bColor;
	COLORREF		m_ColorWindow;
	COLORREF		m_ColorHighlight;
	COLORREF		m_ColorWindowText;
	COLORREF		m_ColorHighlightText;

	// NOTE - following list must be kept in sync with ColorPickerCB.cpp

	enum Color { Black,  White, Maroon,  Green,
				 Olive,  Navy,  Purple,  Teal,
				 Silver, Gray,  Red,     Lime,
				 Yellow, Blue,  Fuschia, Aqua };

	void EnableColor(BOOL bEnable);

// Operations
public:
	void AddLine(Color tc, Color bc, LPCTSTR lpszLine);
	void AddString(LPCTSTR lpszItem);
	int FindString(int nStartAfter, LPCTSTR lpszItem) const;
	int FindStringExact(int nStartAfter, LPCTSTR lpszItem) const;
	int GetText(int nIndex, LPTSTR lpszBuffer) const;
	void GetText(int nIndex, CString& rString) const;
	int GetTextLen(int nIndex) const;
	int GetTextWithColor(int nIndex, LPTSTR lpszBuffer) const;
	void GetTextWithColor(int nIndex, CString& rString) const;
	int GetVisibleLines();
	int InsertString(int nIndex, LPCTSTR lpszItem);
	void _cdecl Printf(Color tc, Color bc, UINT nID, LPCTSTR lpszFmt, ...);
	virtual void ResetContent();
	int SelectString(int nStartAfter, LPCTSTR lpszItem);
	void SetContextMenuId(UINT nId) { m_nContextMenuId = nId; }
	virtual void SetFont(CFont *pFont, BOOL bRedraw = TRUE);
	void SetTabPosition(int nSpacesPerTab);

// Implementation
protected:
	int SearchString(int nStartAfter, LPCTSTR lpszItem, BOOL bExact) const;

	#define MAXTABSTOPS 100
	int m_nTabStopPositions[MAXTABSTOPS];
	int m_nSpaceWidth;
	int m_nAveCharWidth;
	UINT m_nContextMenuId;

	virtual int  CompareItem (LPCOMPAREITEMSTRUCT lpCIS);
	virtual void DrawItem (LPDRAWITEMSTRUCT lpDIS);
	virtual void MeasureItem (LPMEASUREITEMSTRUCT lpMIS);

	// Generated message map functions
	//{{AFX_MSG(CXListBox)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnEditSelectAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
///////////////////////////////////////////////////////////////////////////////

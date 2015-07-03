// XHyperLink.h  Version 1.0
//
// XHyperLink static control. Will open the default browser with the 
// given URL when the user clicks on the link.
//
// Copyright Chris Maunder, 1997-1999 (cmaunder@mail.com)
// Feel free to use and distribute. May not be sold for profit. 
//
// 2/29/00 -- P. Shaffer standard font mod.
//
///////////////////////////////////////////////////////////////////////////////
//
// Modified by:  Hans Dietrich
//               hdietrich@gmail.com
//
///////////////////////////////////////////////////////////////////////////////


#ifndef XHYPERLINK_H
#define XHYPERLINK_H

extern UINT WM_XHYPERLINK_CLICKED;

/////////////////////////////////////////////////////////////////////////////
// CXHyperLink window

class CXHyperLink : public CStatic
{
// Construction/destruction
public:
    CXHyperLink();
    virtual ~CXHyperLink();

public:
    enum UnderLineOptions { ulHover = -1, ulNone = 0, ulAlways = 1};

// Attributes
public:
    void SetURL(CString strURL);
	CString GetURL() const
	{ 
		return m_strURL; 
	}

	void EnableURL(BOOL bFlag) { m_bIsURLEnabled = bFlag; }
	BOOL IsURLEnabled() const { return m_bIsURLEnabled; }

    void SetColours(COLORREF crLinkColour, 
					COLORREF crVisitedColour, 
                    COLORREF crHoverColour = -1);

	COLORREF GetLinkColour() const
	{ 
		return m_crLinkColour; 
	}

	COLORREF GetVisitedColour() const
	{
		return m_crVisitedColour; 
	}

	COLORREF GetHoverColour() const
	{
		return m_crHoverColour;
	}

	void SetBackgroundColour(COLORREF crBackground);
	COLORREF GetBackgroundColour() const
	{
		return m_crBackground;
	}

    void SetVisited(BOOL bVisited = TRUE);
	BOOL GetVisited() const
	{ 
		return m_bVisited; 
	}

    void SetLinkCursor(HCURSOR hCursor);
	HCURSOR CXHyperLink::GetLinkCursor() const
	{
		return m_hLinkCursor;
	}

    void SetUnderline(int nUnderline = ulHover);
	int GetUnderline() const
	{ 
		return m_nUnderline; 
	}

    void SetAutoSize(BOOL bAutoSize = TRUE);
	BOOL GetAutoSize() const
	{ 
		return m_bAdjustToFit; 
	}

	void SetNotifyParent(BOOL bFlag) { m_bNotifyParent = bFlag; }
	BOOL GetNotifyParent() const { return m_bNotifyParent; }

	void EnableTooltip(BOOL bFlag) 
	{ 
		m_bToolTip = bFlag; 
		m_ToolTip.Activate(m_bToolTip);
	}
	BOOL IsTooltipEmabled() const 
	{ 
		return m_bToolTip; 
	}

	void SetAlwaysOpenNew(BOOL bFlag) 
	{ 
		m_bAlwaysOpenNew = bFlag; 
	}
	BOOL GetAlwaysOpenNew() const 
	{ 
		return m_bAlwaysOpenNew; 
	}

	void SetWindowText(LPCTSTR lpszString);


// Operations
public:
	static HINSTANCE GotoURL(LPCTSTR url, int showcmd,  BOOL bAlwaysOpenNew = FALSE);


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CXHyperLink)
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();
protected:
    virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
protected:
	static LONG GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata);
	void NotifyParent();
	void PositionWindow();
	void ReportError(int nError);
	void SetDefaultCursor();

// Protected attributes
protected:
    COLORREF		m_crLinkColour;		// Hyperlink colours
	COLORREF		m_crVisitedColour;
    COLORREF		m_crHoverColour;	// Hover colour
	COLORREF		m_crBackground;		// background color
	CBrush			m_Brush;			// background brush
    BOOL			m_bOverControl;		// cursor over control?
    BOOL			m_bVisited;			// Has it been visited?
    int				m_nUnderline;		// underline hyperlink?
    BOOL			m_bAdjustToFit;		// Adjust window size to fit text?
    CString			m_strURL;			// hyperlink URL
    CFont			m_UnderlineFont;	// Font for underline display
    CFont			m_StdFont;			// Standard font
    HCURSOR			m_hLinkCursor;		// Cursor for hyperlink
    CToolTipCtrl	m_ToolTip;			// The tooltip
    UINT			m_nTimerID;
	BOOL			m_bIsURLEnabled;	// TRUE = navigate to url
	BOOL			m_bNotifyParent;	// TRUE = notify parent
	BOOL			m_bToolTip;			// TRUE = display tooltip
	BOOL			m_bAlwaysOpenNew;	// TRUE = always open new browser window

    // Generated message map functions
protected:
    //{{AFX_MSG(CXHyperLink)
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnClicked();
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif //XHYPERLINK_H

// FAVORG Version 1.4
// Copyright (c) 2000 Ziff Davis Media, Inc.
// All rights reserved.
// First Published in PC Magazine, US Edition, November 7, 2000.
// Programmer: Patrick Philippot

/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__26438FCE_6785_11D4_908E_00A0249EABF4__INCLUDED_)
#define AFX_MAINFRM_H__26438FCE_6785_11D4_908E_00A0249EABF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ActivateFrame(int nCmdShow = -1);
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;

public:
	CStatusBar*	GetStatusBar() {return &m_wndStatusBar;};

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#define MIN_FRAME_SIZEX 400
#define MIN_FRAME_SIZEY 300

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__26438FCE_6785_11D4_908E_00A0249EABF4__INCLUDED_)

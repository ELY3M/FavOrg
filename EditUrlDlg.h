// FAVORG Version 1.4
// Copyright (c) 2000 Ziff Davis Media, Inc.
// All rights reserved.
// First Published in PC Magazine, US Edition, November 7, 2000.
// Programmer: Patrick Philippot

#if !defined(AFX_EDITURLDLG_H__7C055AF6_6CF4_11D4_909F_00A0249EABF4__INCLUDED_)
#define AFX_EDITURLDLG_H__7C055AF6_6CF4_11D4_909F_00A0249EABF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditUrlDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditUrlDlg dialog

class CEditUrlDlg : public CDialog
{
// Construction
public:
	CEditUrlDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditUrlDlg)
	enum { IDD = IDD_EDITURL };
	CString	m_sRefLabel;
	CString	m_sNewUrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditUrlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditUrlDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITURLDLG_H__7C055AF6_6CF4_11D4_909F_00A0249EABF4__INCLUDED_)

// FAVORG Version 1.4
// Copyright (c) 2000 Ziff Davis Media, Inc.
// All rights reserved.
// First Published in PC Magazine, US Edition, November 7, 2000.
// Programmer: Patrick Philippot

#if !defined(AFX_LEGENDDLG_H__02255BD6_5FB1_11D4_9079_00A0249EABF4__INCLUDED_)
#define AFX_LEGENDDLG_H__02255BD6_5FB1_11D4_9079_00A0249EABF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LegendDlg.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CLegendDlg dialog

class CLegendDlg : public CDialog
{
// Construction
public:
	CLegendDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLegendDlg)
	enum { IDD = IDD_LEGEND };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLegendDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLegendDlg)
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEGENDDLG_H__02255BD6_5FB1_11D4_9079_00A0249EABF4__INCLUDED_)

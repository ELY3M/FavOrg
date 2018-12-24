// FAVORG Version 1.4
// Copyright (c) 2000 Ziff Davis Media, Inc.
// All rights reserved.
// First Published in PC Magazine, US Edition, November 7, 2000.
// Programmer: Patrick Philippot

#if !defined(AFX_FAVORG_H__26438FCA_6785_11D4_908E_00A0249EABF4__INCLUDED_)
#define AFX_FAVORG_H__26438FCA_6785_11D4_908E_00A0249EABF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#define MAX_USERNAMELEN 50

/////////////////////////////////////////////////////////////////////////////
// CFavOrgApp:
// See FavOrg.cpp for the implementation of this class
//

class CFavOrgApp : public CWinApp
{
public:
	CFavOrgApp();

	CStringEx m_sIniFile;
	CStringEx m_sUsername;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFavOrgApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CFavOrgApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFavorgHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FAVORG_H__26438FCA_6785_11D4_908E_00A0249EABF4__INCLUDED_)

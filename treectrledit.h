// FAVORG Version 1.4
// Copyright (c) 2000 Ziff Davis Media, Inc.
// All rights reserved.
// First Published in PC Magazine, US Edition, November 7, 2000.
// Programmer: Patrick Philippot

#if !defined(AFX_TREECTRLEDIT_H__C4E8B186_4A66_11D4_904B_00A0249EABF4__INCLUDED_)
#define AFX_TREECTRLEDIT_H__C4E8B186_4A66_11D4_904B_00A0249EABF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeCtrlEdit.h : header file
//

#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEdit window

class CTreeCtrlEdit : public CEdit
{
// Construction
public:
	CTreeCtrlEdit();

// Attributes
public:

private:
	HTREEITEM	m_hCurItem;
	CTreeCtrl*	m_pTreeCtrl;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeCtrlEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTreeCtrlEdit();
	void	SetCurItem(HTREEITEM hItem);
	void	SetTreeCtrl(CTreeCtrl* pTreeCtrl);

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeCtrlEdit)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREECTRLEDIT_H__C4E8B186_4A66_11D4_904B_00A0249EABF4__INCLUDED_)

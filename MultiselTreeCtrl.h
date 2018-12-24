// FAVORG Version 1.4
// Copyright (c) 2000 Ziff Davis Media, Inc.
// All rights reserved.
// First Published in PC Magazine, US Edition, November 7, 2000.
// Programmer: Patrick Philippot

#if !defined(AFX_MULTISELTREECTRL_H__F12E41C9_40F9_11D4_9042_00A0249EABF4__INCLUDED_)
#define AFX_MULTISELTREECTRL_H__F12E41C9_40F9_11D4_9042_00A0249EABF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MultiselTreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMultiselTreeCtrl window

class CMultiselTreeCtrl : public CTreeCtrl
{
	// Construction
	public:
		CMultiselTreeCtrl();

	// Attributes
	public:

		protected:
	HTREEITEM					m_hItemFirstSel;
	CList<HTREEITEM, HTREEITEM> m_SelItemList;

	// Operations
	public:

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CMultiselTreeCtrl)
	public:
	//}}AFX_VIRTUAL

		// Implementation
public:
	virtual ~CMultiselTreeCtrl();

	virtual void	ClearSelection(BOOL bCleanTree);
	BOOL			SelectItems(HTREEITEM hItemFrom, HTREEITEM hItemTo);
	BOOL			IsSibling(HTREEITEM hFromItem, HTREEITEM hToItem);
	void			UpdateSelectedItems();

	// Generated message map functions
	protected:
	//{{AFX_MSG(CMultiselTreeCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MULTISELTREECTRL_H__F12E41C9_40F9_11D4_9042_00A0249EABF4__INCLUDED_)

// FAVORG Version 1.4
// Copyright (c) 2000 Ziff Davis Media, Inc.
// All rights reserved.
// First Published in PC Magazine, US Edition, November 7, 2000.
// Programmer: Patrick Philippot

#if !defined(AFX_BYSTATEDLG_H__6613D5A6_61F9_11D4_907D_00A0249EABF4__INCLUDED_)
#define AFX_BYSTATEDLG_H__6613D5A6_61F9_11D4_907D_00A0249EABF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ByStateDlg.h : header file
//

#define BYSTATE_ACCESSDENIED			1
#define BYSTATE_PAGENONE				2
#define BYSTATE_SITENONE				4
#define BYSTATE_TIMEOUT					8

/////////////////////////////////////////////////////////////////////////////
// CByStateDlg dialog

class CByStateDlg : public CDialog
{
// Construction
public:
	CByStateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CByStateDlg)
	enum { IDD = IDD_BYSTATE };
	BOOL	m_bPageNotFound;
	BOOL	m_bAccessDenied;
	BOOL	m_bSiteNone;
	BOOL	m_bTimeOut;
	//}}AFX_DATA

	CFavOrgTreeCtl*	m_pFavTree;
	int				m_nCount;
	int				m_nCountAccessDenied;
	int				m_nCountTimeout;
	int				m_nCountSiteNone;
	int				m_nCountPageNotFound;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CByStateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CByStateDlg)
	afx_msg void OnCheckAccessDenied();
	afx_msg void OnCheckPageNone();
	afx_msg void OnCheckSiteNone();
	afx_msg void OnCheckTimeout();
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void CheckItemState(HTREEITEM hItem, BOOL bAccessDenied, BOOL bTimeout, BOOL bPageNotFound, BOOL bSiteNone);
	void CountItemsByState(HTREEITEM hItem, BOOL bAccessDenied, BOOL bTimeout, BOOL bPageNotFound, BOOL bSiteNone);
	void CByStateDlg::CountByState();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BYSTATEDLG_H__6613D5A6_61F9_11D4_907D_00A0249EABF4__INCLUDED_)

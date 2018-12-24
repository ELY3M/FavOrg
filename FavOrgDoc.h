// FAVORG Version 1.4
// Copyright (c) 2000 Ziff Davis Media, Inc.
// All rights reserved.
// First Published in PC Magazine, US Edition, November 7, 2000.
// Programmer: Patrick Philippot

/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FAVORGDOC_H__26438FD0_6785_11D4_908E_00A0249EABF4__INCLUDED_)
#define AFX_FAVORGDOC_H__26438FD0_6785_11D4_908E_00A0249EABF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CFavOrgDoc : public CDocument
{
protected: // create from serialization only
	CFavOrgDoc();
	DECLARE_DYNCREATE(CFavOrgDoc)

// Attributes
public:

protected:
	BOOL			m_bLocked;
	CFavOrgTreeCtl*	m_pctlFavTree;

// Operations
public:
	CStringEx		m_sStorePath;
	int				m_nTimeout;
	CRect			m_rectLegend;
	BOOL			m_bShowLegend;
	int				m_nNoConnectTest;
	int				m_nAllowUpdate;
	int				m_nFindIcons;
	int				m_nOverwrite;
	BOOL			m_bCommandsDisabled;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFavOrgDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void OnCloseDocument();
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	//}}AFX_VIRTUAL

	void SetTreeCtrl(CFavOrgTreeCtl* pctlFavTree) {m_pctlFavTree = pctlFavTree;};
	void FinalInit();

// Implementation
public:
	virtual ~CFavOrgDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CFavOrgDoc)
	afx_msg void OnUpdateAppExit(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#define FAVORG_DEFAULT_TIMEOUT 20

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FAVORGDOC_H__26438FD0_6785_11D4_908E_00A0249EABF4__INCLUDED_)

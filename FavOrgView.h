// FAVORG Version 1.4
// Copyright (c) 2000 Ziff Davis Media, Inc.
// All rights reserved.
// First Published in PC Magazine, US Edition, November 7, 2000.
// Programmer: Patrick Philippot

/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FAVORGVIEW_H__26438FD2_6785_11D4_908E_00A0249EABF4__INCLUDED_)
#define AFX_FAVORGVIEW_H__26438FD2_6785_11D4_908E_00A0249EABF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CFavOrgView : public CFormView
{
protected: // create from serialization only
	CFavOrgView();
	DECLARE_DYNCREATE(CFavOrgView)

public:
	//{{AFX_DATA(CFavOrgView)
	enum { IDD = IDD_FAVORG_FORM };
	CProgressCtrl	m_ctlProgress;
	//}}AFX_DATA

	CLegendDlg*		m_pdlgLegend;

private:
	CFavOrgTreeCtl	m_ctlFavTree;
	BOOL			m_bInitialUpdate;

	int				m_nRightOffset1;
	int				m_nRightOffset2;
	int				m_nBottomOffset;
	int				m_nTreeOffsetH;
	int				m_nTreeOffsetV;
	int				m_nProgressOffsetH;
	int				m_nProgressOffsetVTop;
	int				m_nProgressOffsetVBottom;
	BOOL			m_bSizeInit;
	BOOL			m_bLocked;
	CStatusBar*		m_pStatusBar;

// Attributes
public:
	CFavOrgDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFavOrgView)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFavOrgView();
	
	BOOL		IsLocked() {return m_bLocked;};
	CStatusBar*	GetStatusBar(){return m_pStatusBar;};

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void	ShowLegend(BOOL bShow);

// Generated message map functions
protected:
	//{{AFX_MSG(CFavOrgView)
	afx_msg void OnReplaceUrlAllowed();
	afx_msg void OnAssignThis();
	afx_msg void OnCancelAction();
	afx_msg void OnCustomIcon();
	afx_msg void OnDeleteByState();
	afx_msg void OnGenerateLog();
	afx_msg void OnGo();
	afx_msg void OnEditUrl();
	afx_msg void OnOptions();
	afx_msg void OnPickIcon();
	afx_msg void OnRefresh();
	afx_msg void OnRemove();
	afx_msg void OnResetDeadLinks();
	afx_msg void OnShowLegend();
	afx_msg void OnUpdateAssignThis(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCustomIcon(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDeleteByState(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGenerateLog(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNewUrl(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePickIcon(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRemove(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowLegend(CCmdUI* pCmdUI);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFindFavicons();
	afx_msg void OnDispInfo();
	afx_msg void OnUpdateDispInfo(CCmdUI* pCmdUI);
	afx_msg void OnExpandAll();
	afx_msg void OnExpandCurrent();
	afx_msg void OnUpdateExpandCurrent(CCmdUI* pCmdUI);
	afx_msg void OnCollapseAll();
	afx_msg void OnUpdateCollapseAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExpandAll(CCmdUI* pCmdUI);
	afx_msg void OnSelectNone();
	afx_msg void OnUpdateSelectNone(CCmdUI* pCmdUI);
	afx_msg void OnCollapseCurrent();
	afx_msg void OnUpdateCollapseCurrent(CCmdUI* pCmdUI);
	afx_msg void OnViewReport();
	afx_msg void OnUpdateViewReport(CCmdUI* pCmdUI);
	afx_msg void OnNoOverwrite();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void	OnStatus(UINT nID);
	void	OnUpdateStatus(CCmdUI* pCmdUI);
	void	OnUpdateCancelAction(CCmdUI* pCmdUI);
	void	OnUpdateAllowUpdate(CCmdUI* pCmdUI);
	void	OnUpdateFindFavIcons(CCmdUI* pCmdUI);
	void	OnUpdateNoOverwrite(CCmdUI* pCmdUI);
	void	OnUpdateGo(CCmdUI* pCmdUI);
	void	OnUpdateRefresh(CCmdUI* pCmdUI);
	void	OnUpdateResetDeadLinks(CCmdUI* pCmdUI);
	
	void	DisableCommands(BOOL bDisable);
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM , LPARAM);

	void	DisplayFavNum();
	void	FakeInitialUpdate();

	LRESULT OnUnlock(WPARAM wParam, LPARAM lParam);
	LRESULT OnDispMsg(WPARAM wParam, LPARAM lParam);
	LRESULT OnDispIcon(WPARAM wParam, LPARAM lParam);
	LRESULT OnRefreshCount(WPARAM wParam, LPARAM lParam);
};

#ifndef _DEBUG  // debug version in FavOrgView.cpp
inline CFavOrgDoc* CFavOrgView::GetDocument()
   { return (CFavOrgDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FAVORGVIEW_H__26438FD2_6785_11D4_908E_00A0249EABF4__INCLUDED_)

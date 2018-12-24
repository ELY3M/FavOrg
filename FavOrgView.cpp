// FAVORG Version 1.4
// Copyright (c) 2000 Ziff Davis Media, Inc.
// All rights reserved.
// First Published in PC Magazine, US Edition, November 7, 2000.
// Programmer: Patrick Philippot

#include "stdafx.h"

#include <shlobj.h>
#include <afxpriv.h>
#include <wininet.h>

#include "FavOrg.h"
#include "OptionsDlg.h"
#include "MultiselTreeCtrl.h"
#include "FavOrgTreeCtl.h"
#include "SelIconDlg.h"

#include "FavOrgDoc.h"
#include "FavOrgView.h"

#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char const g_szFilter[] = "Icon files (*.ico)|*.ico|Executables (*.exe; *.dll)|*.exe; *.dll|All Files (*.*)|*.*||";
char const g_szLogFilter[] = "Log files (*.log)|*.log|Text files (*.txt)|*.txt|All Files (*.*)|*.*||";
char const g_szCustIconTitle[] = "Please select an icon file";
char const g_szLogTitle[] = "Choose a name for the log file";
char const g_szViewLogTitle[] = "Please select a log file";

/////////////////////////////////////////////////////////////////////////////
// CFavOrgView

IMPLEMENT_DYNCREATE(CFavOrgView, CFormView)

BEGIN_MESSAGE_MAP(CFavOrgView, CFormView)
	//{{AFX_MSG_MAP(CFavOrgView)
	ON_BN_CLICKED(IDC_ALLOWUPDATE, OnReplaceUrlAllowed)
	ON_COMMAND(IDC_ASSIGNTHIS, OnAssignThis)
	ON_BN_CLICKED(IDC_CANCELACTION, OnCancelAction)
	ON_COMMAND(IDC_CUSTOMICON, OnCustomIcon)
	ON_COMMAND(IDC_DELETEBYSTATE, OnDeleteByState)
	ON_COMMAND(IDC_GENERATELOG, OnGenerateLog)
	ON_BN_CLICKED(IDC_GO, OnGo)
	ON_COMMAND(IDC_EDITURL, OnEditUrl)
	ON_COMMAND(IDC_OPTIONS, OnOptions)
	ON_COMMAND(IDC_PICKICON, OnPickIcon)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_COMMAND(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_RESETTOROOT, OnResetDeadLinks)
	ON_COMMAND(IDC_SHOWLEGEND, OnShowLegend)
	ON_UPDATE_COMMAND_UI(IDC_ASSIGNTHIS, OnUpdateAssignThis)
	ON_UPDATE_COMMAND_UI(IDC_CUSTOMICON, OnUpdateCustomIcon)
	ON_UPDATE_COMMAND_UI(IDC_DELETEBYSTATE, OnUpdateDeleteByState)
	ON_UPDATE_COMMAND_UI(IDC_GENERATELOG, OnUpdateGenerateLog)
	ON_UPDATE_COMMAND_UI(IDC_EDITURL, OnUpdateNewUrl)
	ON_UPDATE_COMMAND_UI(IDC_PICKICON, OnUpdatePickIcon)
	ON_UPDATE_COMMAND_UI(IDC_REMOVE, OnUpdateRemove)
	ON_UPDATE_COMMAND_UI(IDC_SHOWLEGEND, OnUpdateShowLegend)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_FINDFAVICONS, OnFindFavicons)
	ON_COMMAND(IDC_DISPINFO, OnDispInfo)
	ON_UPDATE_COMMAND_UI(IDC_DISPINFO, OnUpdateDispInfo)
	ON_COMMAND(IDC_EXPANDALL, OnExpandAll)
	ON_COMMAND(IDC_EXPANDCURRENT, OnExpandCurrent)
	ON_UPDATE_COMMAND_UI(IDC_EXPANDCURRENT, OnUpdateExpandCurrent)
	ON_COMMAND(IDC_COLLAPSEALL, OnCollapseAll)
	ON_UPDATE_COMMAND_UI(IDC_COLLAPSEALL, OnUpdateCollapseAll)
	ON_UPDATE_COMMAND_UI(IDC_EXPANDALL, OnUpdateExpandAll)
	ON_COMMAND(IDC_SELECTNONE, OnSelectNone)
	ON_UPDATE_COMMAND_UI(IDC_SELECTNONE, OnUpdateSelectNone)
	ON_COMMAND(IDC_COLLAPSECURRENT, OnCollapseCurrent)
	ON_UPDATE_COMMAND_UI(IDC_COLLAPSECURRENT, OnUpdateCollapseCurrent)
	ON_COMMAND(IDC_VIEWREPORT, OnViewReport)
	ON_UPDATE_COMMAND_UI(IDC_VIEWREPORT, OnUpdateViewReport)
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(IDC_CANCELACTION, OnUpdateCancelAction)
	ON_UPDATE_COMMAND_UI(IDC_ALLOWUPDATE, OnUpdateAllowUpdate)
	ON_UPDATE_COMMAND_UI(IDC_FINDFAVICONS, OnUpdateFindFavIcons)
	ON_UPDATE_COMMAND_UI(IDC_NOOVERWRITE, OnUpdateNoOverwrite)
	ON_UPDATE_COMMAND_UI(IDC_GO, OnUpdateGo)
	ON_UPDATE_COMMAND_UI(IDC_REFRESH, OnUpdateRefresh)
	ON_UPDATE_COMMAND_UI(IDC_RESETTOROOT, OnUpdateResetDeadLinks)
	ON_MESSAGE(WM_CAN_UNLOCK, OnUnlock)
	ON_MESSAGE(WM_DISP_MSG, OnDispMsg)
	ON_MESSAGE(WM_DISP_ICON, OnDispIcon)
	ON_MESSAGE(WM_REFRESH_COUNT, OnRefreshCount)
	ON_COMMAND(IDC_ASSIGNCUST, OnCustomIcon)
	ON_UPDATE_COMMAND_UI(IDC_ASSIGNCUST, OnUpdateCustomIcon)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_NOOVERWRITE, OnNoOverwrite)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(IDC_MNU_QUESTION, IDC_MNU_RESET, OnStatus)
	ON_UPDATE_COMMAND_UI_RANGE(IDC_MNU_QUESTION, IDC_MNU_RESET, OnUpdateStatus)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFavOrgView construction/destruction

CFavOrgView::CFavOrgView()
	: CFormView(CFavOrgView::IDD)
{
	//{{AFX_DATA_INIT(CFavOrgView)
	//}}AFX_DATA_INIT
	// TODO: add construction code here

	m_pdlgLegend = NULL;
	m_bInitialUpdate = FALSE;
	m_bLocked = FALSE;
	m_bSizeInit = FALSE;
}

CFavOrgView::~CFavOrgView()
{
}

void CFavOrgView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFavOrgView)
	DDX_Control(pDX, IDC_PROGRESS, m_ctlProgress);
	//}}AFX_DATA_MAP
}

void CFavOrgView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if (!m_bInitialUpdate)
	{
		m_bInitialUpdate = TRUE;
		// A trick to start painting before initiliazation is done
		// Thanks to Mike Blaszczak
		FakeInitialUpdate();
	}

	// Do not call CFormView::OnPaint() for painting messages
}

void CFavOrgView::FakeInitialUpdate()
{
	ShowWindow(SW_SHOW);
	UpdateWindow();

	DisableCommands(TRUE);
	// Load the Favorites tree
	m_ctlFavTree.LoadFavorites(((CFavOrgApp*) AfxGetApp())->m_sIniFile);
	DisableCommands(FALSE);

	DisplayFavNum();
}

void CFavOrgView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	m_pStatusBar = ((CMainFrame*) AfxGetMainWnd())->GetStatusBar();
	m_pStatusBar->UpdateWindow();

	GetParentFrame()->RecalcLayout();
	ResizeParentToFit(FALSE);

	// Don't want to see the scrollbars
	SetScrollSizes(MM_TEXT, CSize(0, 0));

	// Subclass our standard Tree control
	m_ctlFavTree.SubclassDlgItem(IDC_FAVTREE, this);
	GetDocument()->SetTreeCtrl(&m_ctlFavTree);
	GetDocument()->FinalInit();

	// Give our CFavOrgTreeCtl the address of interface elements
	m_ctlFavTree.SetProgressBar(&m_ctlProgress);

	// Store initial data for resizing
	CRect	rectView;
	CRect	rectGo;
	CRect	rectFind;
	CRect	rectRefresh;
	CRect	rectTree;
	CRect	rectProgress;

	CWnd*	pwndGo = GetDlgItem(IDC_GO);
	CWnd*	pwndFind = GetDlgItem(IDC_FINDFAVICONS);
	CWnd*	pwndRefresh = GetDlgItem(IDC_REFRESH);

	GetClientRect(rectView);
	ClientToScreen(&rectView);

	pwndGo->GetWindowRect(rectGo);
	pwndFind->GetWindowRect(rectFind);
	pwndRefresh->GetWindowRect(rectRefresh);

	m_ctlFavTree.GetWindowRect(rectTree);
	m_ctlProgress.GetWindowRect(rectProgress);

	m_nRightOffset1 = rectView.right - rectGo.left;
	m_nRightOffset2 = rectView.right - rectFind.left;
	m_nBottomOffset = rectView.bottom - rectRefresh.top;
	m_nTreeOffsetH = rectView.right - rectTree.right;
	m_nTreeOffsetV = rectView.bottom - rectTree.bottom;
	m_nProgressOffsetH = rectView.right - rectProgress.right;
	m_nProgressOffsetVTop = rectView.bottom - rectProgress.top;
	m_nProgressOffsetVBottom = rectView.bottom - rectProgress.bottom;
	m_bSizeInit = TRUE;
}

void CFavOrgView::OnSize(UINT nType, int cx, int cy)
{
	if (m_bSizeInit)
	{
		// Store initial data for resizing
		CRect	rectView;
		CRect	rectGo;
		CRect	rectCancel;
		CRect	rectFind;
		CRect   rectNoOvrw;
		CRect	rectOption;
		CRect	rectAllow;
		CRect	rectRefresh;
		CRect	rectReset;
		CRect	rectTree;
		CRect	rectProgress;

		CWnd*	pwndGo = GetDlgItem(IDC_GO);
		CWnd*	pwndCancel = GetDlgItem(IDC_CANCELACTION);
		CWnd*	pwndOption = GetDlgItem(IDC_OPTIONGROUP);
		CWnd*	pwndFind = GetDlgItem(IDC_FINDFAVICONS);
		CWnd*	pwndNoOvrw = GetDlgItem(IDC_NOOVERWRITE);
		CWnd*	pwndAllow = GetDlgItem(IDC_ALLOWUPDATE);
		CWnd*	pwndRefresh = GetDlgItem(IDC_REFRESH);
		CWnd*	pwndReset = GetDlgItem(IDC_RESETTOROOT);

		GetClientRect(rectView);

		pwndGo->GetWindowRect(rectGo);
		ScreenToClient(&rectGo);
		pwndCancel->GetWindowRect(rectCancel);
		ScreenToClient(&rectCancel);
		pwndOption->GetWindowRect(rectOption);
		ScreenToClient(&rectOption);
		pwndFind->GetWindowRect(rectFind);
		ScreenToClient(&rectFind);
		pwndNoOvrw->GetWindowRect(rectNoOvrw);
		ScreenToClient(&rectNoOvrw);
		pwndAllow->GetWindowRect(rectAllow);
		ScreenToClient(&rectAllow);

		pwndRefresh->GetWindowRect(rectRefresh);
		ScreenToClient(&rectRefresh);
		pwndReset->GetWindowRect(rectReset);
		ScreenToClient(&rectReset);

		m_ctlFavTree.GetWindowRect(rectTree);
		ScreenToClient(&rectTree);
		m_ctlProgress.GetWindowRect(rectProgress);
		ScreenToClient(&rectProgress);

		pwndGo->MoveWindow(rectView.right - m_nRightOffset1,
						   rectGo.top,
						   rectGo.Width(),
						   rectGo.Height());
		pwndCancel->MoveWindow(rectView.right - m_nRightOffset1,
						   rectCancel.top,
						   rectCancel.Width(),
						   rectCancel.Height());
		pwndOption->MoveWindow(rectView.right - m_nRightOffset1,
						   rectOption.top,
						   rectOption.Width(),
						   rectOption.Height());
		pwndFind->MoveWindow(rectView.right - m_nRightOffset2,
						   rectFind.top,
						   rectFind.Width(),
						   rectFind.Height());
		pwndNoOvrw->MoveWindow(rectView.right - m_nRightOffset2,
						   rectNoOvrw.top,
						   rectNoOvrw.Width(),
						   rectNoOvrw.Height());
		pwndAllow->MoveWindow(rectView.right - m_nRightOffset2,
						   rectAllow.top,
						   rectAllow.Width(),
						   rectAllow.Height());

		pwndRefresh->MoveWindow(rectRefresh.left,
						   rectView.bottom - m_nBottomOffset,
						   rectRefresh.Width(),
						   rectRefresh.Height());
		pwndReset->MoveWindow(rectReset.left,
						   rectView.bottom - m_nBottomOffset,
						   rectReset.Width(),
						   rectReset.Height());

		rectTree.right = rectView.right - m_nTreeOffsetH;
		rectTree.bottom = rectView.bottom - m_nTreeOffsetV;
		m_ctlFavTree.MoveWindow(&rectTree);

		rectProgress.right = rectView.right - m_nProgressOffsetH;
		rectProgress.top = rectView.bottom - m_nProgressOffsetVTop;
		rectProgress.bottom = rectView.bottom - m_nProgressOffsetVBottom;
		m_ctlProgress.MoveWindow(&rectProgress);
	}

	CFormView::OnSize(nType, cx, cy);
}

LRESULT CFavOrgView::OnUnlock(WPARAM wParam, LPARAM lParam)
{
	DisableCommands(FALSE);

	return(0);
}

LRESULT CFavOrgView::OnDispMsg(WPARAM wParam, LPARAM lParam)
{
	CString* psMsg = (CString*) lParam;

	m_pStatusBar->SetPaneText(2, lParam == NULL ? "" : *psMsg);
	delete psMsg;

	return(0);
}

LRESULT CFavOrgView::OnDispIcon(WPARAM wParam, LPARAM lParam)
{
	m_pStatusBar->GetStatusBarCtrl().SetIcon(1, (HICON) lParam);
	return(0);
}

LRESULT CFavOrgView::OnRefreshCount(WPARAM wParam, LPARAM lParam)
{
	DisplayFavNum();
	return(0);
}

/////////////////////////////////////////////////////////////////////////////
// CFavOrgView diagnostics

#ifdef _DEBUG
void CFavOrgView::AssertValid() const
{
	CFormView::AssertValid();
}

void CFavOrgView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CFavOrgDoc* CFavOrgView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFavOrgDoc)));
	return (CFavOrgDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFavOrgView helpers

void CFavOrgView::DisplayFavNum()
{
	// Display number of favorites
	CString sFavNum, sNumFmt;

	sNumFmt.LoadString(IDS_NUMFMT);
	sFavNum.Format(sNumFmt, m_ctlFavTree.GetItemCount());
	SetDlgItemText(IDC_FAVNUMBER, sFavNum);
}


/////////////////////////////////////////////////////////////////////////////
// CFavOrgView message handlers


void CFavOrgView::OnReplaceUrlAllowed()
{
	BOOL bAllowed = (BOOL) ((CButton*) GetDlgItem(IDC_ALLOWUPDATE))->GetCheck();
	m_ctlFavTree.SetRedirFlag(bAllowed);
	GetDocument()->m_nAllowUpdate = bAllowed ? 1 : 0;
}

void CFavOrgView::OnFindFavicons()
{
	BOOL bFindIcons = (BOOL) ((CButton*) GetDlgItem(IDC_FINDFAVICONS))->GetCheck();
	GetDocument()->m_nFindIcons = bFindIcons ? 1 : 0;
}

void CFavOrgView::OnNoOverwrite()
{
	BOOL bOverwrite = (BOOL) ((CButton*) GetDlgItem(IDC_NOOVERWRITE))->GetCheck();
	GetDocument()->m_nOverwrite = bOverwrite ? 1 : 0;
}

void CFavOrgView::OnAssignThis()
{
	// Assign picked icon to selected items
	m_ctlFavTree.AssignThisImage(NULL);
}

void CFavOrgView::OnCancelAction()
{
	// Cancel current action
	m_ctlFavTree.CancelAction();
}

void CFavOrgView::OnCustomIcon()
{
	m_ctlFavTree.ExecLocalCommand(IDC_ASSIGNCUST, FALSE);
}

void CFavOrgView::OnDispInfo()
{
	m_ctlFavTree.ExecLocalCommand(IDC_DISPINFO, FALSE);
}

void CFavOrgView::OnDeleteByState()
{
	DisableCommands(TRUE);
	m_ctlFavTree.DeleteByState();
	DisableCommands(FALSE);
}

void CFavOrgView::OnGenerateLog()
{
	// Prepare report generation
	CFileDialog dlg(FALSE,
					"*.log",
					NULL,
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST,
					g_szLogFilter,
					this);

	dlg.m_ofn.lpstrTitle = g_szLogTitle;

	CStringEx sInitialDir = ((CFavOrgApp*) AfxGetApp())->m_sIniFile;

	sInitialDir.StripToPath();
	dlg.m_ofn.lpstrInitialDir = sInitialDir;

	if (dlg.DoModal() == IDOK)
		m_ctlFavTree.GenerateLog(dlg.GetPathName());
}

void CFavOrgView::OnViewReport()
{
	CFileDialog dlg(TRUE,
					"*.log",
					NULL,
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST,
					g_szLogFilter,
					this);

	dlg.m_ofn.lpstrTitle = g_szViewLogTitle;
	CStringEx sInitialDir = ((CFavOrgApp*) AfxGetApp())->m_sIniFile;

	sInitialDir.StripToPath();
	dlg.m_ofn.lpstrInitialDir = sInitialDir;

	if (dlg.DoModal() == IDOK)
	{
		CString sCommand = "notepad.exe ";

		sCommand += dlg.GetPathName();
		WinExec(sCommand, SW_SHOWNORMAL);
	}
}

void CFavOrgView::OnGo()
{
	DisableCommands(TRUE);
	// Find dead links and icons
	m_ctlFavTree.CheckAndFindIcons(!(BOOL) ((CButton*) GetDlgItem(IDC_FINDFAVICONS))->GetCheck(),
								   (BOOL) ((CButton*) GetDlgItem(IDC_NOOVERWRITE))->GetCheck());
	DisableCommands(FALSE);
}

void CFavOrgView::OnEditUrl()
{
	m_ctlFavTree.UpdateUrl(NULL);
}

void CFavOrgView::OnOptions()
{
	COptionsDlg dlg;

	dlg.m_sStorePath = GetDocument()->m_sStorePath;
	dlg.m_nTimeout = GetDocument()->m_nTimeout;
	dlg.m_bNoConnectiontest = GetDocument()->m_nNoConnectTest == 1;
	dlg.m_bCanSetFolder = !GetDocument()->m_bCommandsDisabled;

	if (dlg.DoModal() == IDOK)
	{
		GetDocument()->m_sStorePath = dlg.m_sStorePath;
		m_ctlFavTree.SetIconStore(dlg.m_sStorePath);
		GetDocument()->m_nTimeout = dlg.m_nTimeout;
		m_ctlFavTree.SetTimeout(dlg.m_nTimeout);
		GetDocument()->m_nNoConnectTest = dlg.m_bNoConnectiontest ? 1 : 0;
		m_ctlFavTree.SetConnectCheck(dlg.m_bNoConnectiontest);
	}
}

void CFavOrgView::OnPickIcon()
{
	// Pick icon of selected link
	m_ctlFavTree.PickCurrentIcon(NULL);
}

void CFavOrgView::OnStatus(UINT nID)
{
	m_ctlFavTree.ExecLocalCommand(nID, FALSE);
}

void CFavOrgView::OnRefresh()
{
	DisableCommands(TRUE);
	// Refresh tree
	m_ctlFavTree.LoadFavorites(((CFavOrgApp*) AfxGetApp())->m_sIniFile);
	DisableCommands(FALSE);
	DisplayFavNum();
}

void CFavOrgView::OnRemove()
{
	// Send selected items to Recycle Bin
	DisableCommands(TRUE);
	m_ctlFavTree.RemoveSelected();
	DisableCommands(FALSE);
	DisplayFavNum();
}

void CFavOrgView::OnResetDeadLinks()
{
	DisableCommands(TRUE);
	// Reset dead links to root
	m_ctlFavTree.ResetDeadLinks();
	DisableCommands(FALSE);
}

void CFavOrgView::ShowLegend(BOOL bShow)
{
	// Display legend dialog box as a modeless dialog
	if (GetDocument()->m_bShowLegend)
	{
		// Display legend
		if (!m_pdlgLegend)	// Dialog not created
		{
			m_pdlgLegend = new CLegendDlg;
			m_pdlgLegend->Create(IDD_LEGEND, this);

			CRect rectLegend;

			if (GetDocument()->m_rectLegend.right != 0)
				m_pdlgLegend->MoveWindow(&(GetDocument()->m_rectLegend));
			else
			{
				m_pdlgLegend->GetWindowRect(&rectLegend);
				m_pdlgLegend->MoveWindow(0,
										 0,
										 rectLegend.right - rectLegend.left,
										 rectLegend.bottom - rectLegend.top);
			}
			m_pdlgLegend->ShowWindow(SW_SHOWNORMAL);
			m_pdlgLegend->UpdateWindow();
		}
		else	// Just show it
			m_pdlgLegend->ShowWindow(SW_SHOWNORMAL);
	}
	else if (m_pdlgLegend)
	{
		m_pdlgLegend->ShowWindow(SW_HIDE);
	}
}

void CFavOrgView::OnShowLegend()
{
	GetDocument()->m_bShowLegend = !GetDocument()->m_bShowLegend;
	ShowLegend(GetDocument()->m_bShowLegend);
}

void CFavOrgView::OnExpandAll()
{
	m_ctlFavTree.ExpandAll();
}

void CFavOrgView::OnExpandCurrent()
{
	m_ctlFavTree.ExpandCurrent();
}

void CFavOrgView::OnCollapseAll()
{
	m_ctlFavTree.CollapseAll();
}

void CFavOrgView::OnCollapseCurrent()
{
	m_ctlFavTree.CollapseCurrent();
}

void CFavOrgView::OnSelectNone()
{
	m_ctlFavTree.ClearSelection(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CFavOrgView message handlers for UPDATE_COMMAND_UI messages

void CFavOrgView::DisableCommands(BOOL bDisable)
{
	m_bLocked = bDisable;
	GetDocument()->m_bCommandsDisabled = bDisable;
	UpdateDialogControls(this, FALSE);
}

void CFavOrgView::OnUpdateAssignThis(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!GetDocument()->m_bCommandsDisabled && m_ctlFavTree.IsPickedIcon());
}

void CFavOrgView::OnUpdateCustomIcon(CCmdUI* pCmdUI)
{
	BOOL bDummy, bDummy2;
	pCmdUI->Enable(!GetDocument()->m_bCommandsDisabled && m_ctlFavTree.SelectionExist(bDummy, bDummy2));
}

void CFavOrgView::OnUpdateDeleteByState(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!GetDocument()->m_bCommandsDisabled);
}

void CFavOrgView::OnUpdateGenerateLog(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!GetDocument()->m_bCommandsDisabled);
}

void CFavOrgView::OnUpdateNewUrl(CCmdUI* pCmdUI)
{
	BOOL bOneShortcut, bOneFolder, bOK;

	bOK = m_ctlFavTree.SelectionExist(bOneShortcut, bOneFolder);

	pCmdUI->Enable(!GetDocument()->m_bCommandsDisabled && bOK && bOneShortcut);
}

void CFavOrgView::OnUpdatePickIcon(CCmdUI* pCmdUI)
{
	BOOL bOneShortcut, bOneFolder, bOK;

	bOK = m_ctlFavTree.SelectionExist(bOneShortcut, bOneFolder);

	pCmdUI->Enable(!GetDocument()->m_bCommandsDisabled && bOK && bOneShortcut);
}

void CFavOrgView::OnUpdateRemove(CCmdUI* pCmdUI)
{
	BOOL bDummy, bDummy2;
	pCmdUI->Enable(!GetDocument()->m_bCommandsDisabled && m_ctlFavTree.SelectionExist(bDummy, bDummy2));
}

void CFavOrgView::OnUpdateShowLegend(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(GetDocument()->m_bShowLegend ? 1 : 0);
}

// Necessary to update controls in the form view
LRESULT CFavOrgView::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM)
{
	UpdateDialogControls(this, FALSE);
	return 0L;
}

void CFavOrgView::OnUpdateCancelAction(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument()->m_bCommandsDisabled);
}

void CFavOrgView::OnUpdateAllowUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!GetDocument()->m_bCommandsDisabled);
}

void CFavOrgView::OnUpdateFindFavIcons(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!GetDocument()->m_bCommandsDisabled);
}

void CFavOrgView::OnUpdateNoOverwrite(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!GetDocument()->m_bCommandsDisabled);
}

void CFavOrgView::OnUpdateGo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!GetDocument()->m_bCommandsDisabled);
}

void CFavOrgView::OnUpdateRefresh(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!GetDocument()->m_bCommandsDisabled);
}

void CFavOrgView::OnUpdateResetDeadLinks(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!GetDocument()->m_bCommandsDisabled);
}

void CFavOrgView::OnUpdateDispInfo(CCmdUI* pCmdUI)
{
	BOOL bOneShortcut, bOneFolder, bOK;

	bOK = m_ctlFavTree.SelectionExist(bOneShortcut, bOneFolder);

	if (bOK)
	{
		DWORD dwData = m_ctlFavTree.GetItemData(m_ctlFavTree.GetSelectedItem());

		bOK = bOK /* && (dwData != 0) */ && (dwData != (DWORD) ITEM_IS_FOLDER);
	}

	pCmdUI->Enable(!GetDocument()->m_bCommandsDisabled && bOK && bOneShortcut);
}

void CFavOrgView::OnUpdateCollapseAll(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!GetDocument()->m_bCommandsDisabled);
}

void CFavOrgView::OnUpdateExpandAll(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!GetDocument()->m_bCommandsDisabled);
}

void CFavOrgView::OnUpdateViewReport(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!GetDocument()->m_bCommandsDisabled);
}

void CFavOrgView::OnUpdateCollapseCurrent(CCmdUI* pCmdUI)
{
	BOOL bOneShortcut, bOneFolder, bOK;

	bOK = m_ctlFavTree.SelectionExist(bOneShortcut, bOneFolder);

	if (bOK)
	{
		DWORD dwData = m_ctlFavTree.GetItemData(m_ctlFavTree.GetSelectedItem());

		bOK = bOK && (dwData != 0) && (dwData == (DWORD) ITEM_IS_FOLDER);
	}

	pCmdUI->Enable(!GetDocument()->m_bCommandsDisabled && bOK && bOneFolder);
}

void CFavOrgView::OnUpdateExpandCurrent(CCmdUI* pCmdUI)
{
	BOOL bOneShorcut, bOneFolder, bOK;

	bOK = m_ctlFavTree.SelectionExist(bOneShorcut, bOneFolder);

	if (bOK)
	{
		DWORD dwData = m_ctlFavTree.GetItemData(m_ctlFavTree.GetSelectedItem());

		bOK = bOK && (dwData != 0) && (dwData == (DWORD) ITEM_IS_FOLDER);
	}

	pCmdUI->Enable(!GetDocument()->m_bCommandsDisabled && bOK && bOneFolder);
}

void CFavOrgView::OnUpdateSelectNone(CCmdUI* pCmdUI)
{
	BOOL bDummy, bDummy2;
	pCmdUI->Enable(!GetDocument()->m_bCommandsDisabled && m_ctlFavTree.SelectionExist(bDummy, bDummy2));
}

void CFavOrgView::OnUpdateStatus(CCmdUI* pCmdUI)
{
	int nCommonStatus = m_ctlFavTree.GetCommonStatus(NULL);

	pCmdUI->SetCheck(0);
	if (nCommonStatus != -1)
	{
		int nMenuToCheck = m_ctlFavTree.GetMenuToCheckFromState(nCommonStatus);

		if ((nMenuToCheck != -1) && (pCmdUI->m_nID == (UINT) nMenuToCheck))
			pCmdUI->SetCheck();
	}

	pCmdUI->Enable(!GetDocument()->m_bCommandsDisabled);
}

void CFavOrgView::OnDestroy()
{
	if (m_pdlgLegend)
	{
		m_pdlgLegend->DestroyWindow();
		delete m_pdlgLegend;
	}

	CFormView::OnDestroy();
}

BOOL CFavOrgView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// The user closed the legend window
	if (wParam == CMD_LEGEND_CLOSING)
	{
		GetDocument()->m_bShowLegend = FALSE;
		m_pdlgLegend->GetWindowRect(&(GetDocument()->m_rectLegend));
		m_pdlgLegend->DestroyWindow();
		delete m_pdlgLegend;
		m_pdlgLegend = NULL;

		return TRUE;
	}

	return CFormView::OnCommand(wParam, lParam);
}


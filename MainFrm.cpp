// FAVORG Version 1.4
// Copyright (c) 2000 Ziff Davis Media, Inc.
// All rights reserved.
// First Published in PC Magazine, US Edition, November 7, 2000.
// Programmer: Patrick Philippot

#include "stdafx.h"

#include "FavOrg.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char const g_szPosSection[] = "wndpos";
char const g_szShowEntry[] = "show";
char const g_szFlagsEntry[] = "flags";
char const g_szLeftEntry[] = "left";
char const g_szRightEntry[] = "right";
char const g_szTopEntry[] = "top";
char const g_szBottomEntry[] = "bottom";
char const g_szIconLabel[] = "Picked icon:";

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.CreateEx(this, SBT_TOOLTIPS, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	CDC*	pDC = m_wndStatusBar.GetDC();
	CFont*	pFont = m_wndStatusBar.GetFont();
	CFont*	pOldFont = pDC->SelectObject(pFont);
	SIZE	sizeLabel;

	GetTextExtentPoint32(pDC->m_hDC, g_szIconLabel, strlen(g_szIconLabel), &sizeLabel);
	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);

	m_wndStatusBar.SetPaneInfo(0, 1, SBPS_NOBORDERS, __max(60, sizeLabel.cx + 5));
	m_wndStatusBar.SetPaneText(0, g_szIconLabel);
	m_wndStatusBar.SetPaneInfo(1, 2, SBPS_NOBORDERS, 17);
	m_wndStatusBar.SetPaneInfo(2, 0, SBPS_STRETCH, 0);
	m_wndStatusBar.GetStatusBarCtrl().SetMinHeight(22);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style = cs.style & ~FWS_PREFIXTITLE & ~FWS_ADDTOTITLE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (wParam == CMD_LEGEND_CLOSING)
	{
		POSITION pos = GetActiveDocument()->GetFirstViewPosition();
		if (pos)
		{
			CView* pView = GetActiveDocument()->GetNextView(pos);
			if (pView)
				pView->SendMessage(WM_COMMAND, wParam, lParam);
		}
		
		return TRUE;
	}
	else
		return CFrameWnd::OnCommand(wParam, lParam);
}

void CMainFrame::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	CFrameWnd::OnWindowPosChanging(lpwndpos);

	if (lpwndpos->cx < MIN_FRAME_SIZEX)
		lpwndpos->cx = MIN_FRAME_SIZEX;

	if (lpwndpos->cy < MIN_FRAME_SIZEY)
		lpwndpos->cy = MIN_FRAME_SIZEY;
}

void CMainFrame::OnClose() 
{
	WINDOWPLACEMENT WndStatus;
	
	WndStatus.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(&WndStatus);

	CString sIniFile = ((CFavOrgApp*) AfxGetApp())->m_sIniFile;
	CString sOut;
	
	sOut.Format("%d", WndStatus.flags);
	WritePrivateProfileString(g_szPosSection, g_szFlagsEntry, sOut, sIniFile);
	sOut.Format("%d", WndStatus.showCmd);
	WritePrivateProfileString(g_szPosSection, g_szShowEntry, sOut, sIniFile);
	sOut.Format("%d", WndStatus.rcNormalPosition.left);
	WritePrivateProfileString(g_szPosSection, g_szLeftEntry, sOut, sIniFile);
	sOut.Format("%d", WndStatus.rcNormalPosition.top);
	WritePrivateProfileString(g_szPosSection, g_szTopEntry, sOut, sIniFile);
	sOut.Format("%d", WndStatus.rcNormalPosition.right);
	WritePrivateProfileString(g_szPosSection, g_szRightEntry, sOut, sIniFile);
	sOut.Format("%d", WndStatus.rcNormalPosition.bottom);
	WritePrivateProfileString(g_szPosSection, g_szBottomEntry, sOut, sIniFile);
	
	CFrameWnd::OnClose();
}

void CMainFrame::ActivateFrame(int nCmdShow) 
{
	int nFlags;
 	int	nShow;
 	CRect rect;
 
 	CString sIniFile = ((CFavOrgApp*) AfxGetApp())->m_sIniFile;
 	
 	if (((nShow = GetPrivateProfileInt(g_szPosSection, g_szShowEntry, -1, sIniFile)) != -1)
 		&& 
 		((rect.top = GetPrivateProfileInt(g_szPosSection, g_szTopEntry, -1, sIniFile)) != -1)
 		&&
 		((rect.left = GetPrivateProfileInt(g_szPosSection, g_szLeftEntry, -1, sIniFile)) != -1)
 		&&
 		((rect.bottom = GetPrivateProfileInt(g_szPosSection, g_szBottomEntry, -1, sIniFile)) != -1)
 		&&
 		((rect.right = GetPrivateProfileInt(g_szPosSection, g_szRightEntry, -1, sIniFile)) != -1)) 
 	
 	{
 		WINDOWPLACEMENT WndStatus;
 
 		nFlags = GetPrivateProfileInt(g_szPosSection, g_szFlagsEntry, 0, sIniFile);
 		WndStatus.flags = nFlags;
 		WndStatus.rcNormalPosition = rect;
 		WndStatus.showCmd = nShow;
 		WndStatus.ptMinPosition = CPoint(0,0);
 		WndStatus.ptMaxPosition = CPoint(-GetSystemMetrics(SM_CXBORDER), -GetSystemMetrics(SM_CYBORDER));
 
 		SetWindowPlacement(&WndStatus);
		CFrameWnd::ActivateFrame(nShow);
  	}
	else
		CFrameWnd::ActivateFrame(nCmdShow);
}

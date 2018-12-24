// FAVORG Version 1.4
// Copyright (c) 2000 Ziff Davis Media, Inc.
// All rights reserved.
// First Published in PC Magazine, US Edition, November 7, 2000.
// Programmer: Patrick Philippot

#include "stdafx.h"

#include "FavOrg.h"
#include "multiseltreectrl.h"
#include "favorgtreectl.h"
#include "FavOrgDoc.h"
#include "favorgview.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char const g_szIniSection[] = "options";
char const g_szIniStoreEntry[] = "iconstore";
char const g_szIniTimeoutEntry[] = "timeout";
char const g_szIniAllowUpdateEntry[] = "allowupdate";
char const g_szIniFindIconsEntry[] = "findicons";
char const g_szIniNoOverwriteEntry[] = "nooverwrite";
char const g_szDelOptionEntry[] = "deloption";
char const g_szDefaultTimeout[] = "20";
char const g_szSortTreeEntry[] = "sortree";
char const g_szNoCheckEntry[] = "noconnectcheck";
char const g_szShowLegendEntry[] = "showlegend";
char const g_szLegendTopEntry[] = "legendtop";
char const g_szLegendBottomEntry[] = "legendbottom";
char const g_szLegendLeftEntry[] = "legendleft";
char const g_szLegendRightEntry[] = "legendright";

/////////////////////////////////////////////////////////////////////////////
// CFavOrgDoc

IMPLEMENT_DYNCREATE(CFavOrgDoc, CDocument)

BEGIN_MESSAGE_MAP(CFavOrgDoc, CDocument)
	//{{AFX_MSG_MAP(CFavOrgDoc)
	ON_UPDATE_COMMAND_UI(ID_APP_EXIT, OnUpdateAppExit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFavOrgDoc construction/destruction

CFavOrgDoc::CFavOrgDoc()
{
	m_bLocked = FALSE;
	m_bCommandsDisabled = FALSE;
	m_pctlFavTree = NULL;
}

CFavOrgDoc::~CFavOrgDoc()
{
}

BOOL CFavOrgDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// Initiliaze icon store path and default picked icon
	CString sPickedShortcut;

	CString sIniFile = ((CFavOrgApp*) AfxGetApp())->m_sIniFile;
	
	GetPrivateProfileString(g_szIniSection, g_szIniStoreEntry, _T(""), m_sStorePath.GetBuffer(MAX_PATH), MAX_PATH, sIniFile);
	m_sStorePath.ReleaseBuffer();

	if (m_sStorePath.IsEmpty())
	{
		// Create default store folder
		m_sStorePath = sIniFile;
		m_sStorePath.Delete(m_sStorePath.ReverseFind('\\') + 1);

		CString sUsername = ((CFavOrgApp*) AfxGetApp())->m_sUsername;
		if (!sUsername.IsEmpty())
		{
			m_sStorePath += sUsername;
			m_sStorePath += _T("_Data");
		}
		else
			m_sStorePath += _T("Data");

		CreateDirectory(m_sStorePath, NULL);
	}

	// Get user options
	m_nTimeout = GetPrivateProfileInt(g_szIniSection, g_szIniTimeoutEntry, FAVORG_DEFAULT_TIMEOUT, sIniFile);
	if ((m_nTimeout <= 15) && (m_nTimeout > 60))
		m_nTimeout = FAVORG_DEFAULT_TIMEOUT;

	m_nNoConnectTest = GetPrivateProfileInt(g_szIniSection, g_szNoCheckEntry, 1, sIniFile);

	m_rectLegend.top = GetPrivateProfileInt(g_szIniSection, g_szLegendTopEntry, 0, sIniFile);
	m_rectLegend.bottom = GetPrivateProfileInt(g_szIniSection, g_szLegendBottomEntry, 0, sIniFile);
	m_rectLegend.left = GetPrivateProfileInt(g_szIniSection, g_szLegendLeftEntry, 0, sIniFile);
	m_rectLegend.right = GetPrivateProfileInt(g_szIniSection, g_szLegendRightEntry, 0, sIniFile);

	m_nAllowUpdate = GetPrivateProfileInt(g_szIniSection, g_szIniAllowUpdateEntry, 0, sIniFile);
	m_nFindIcons = GetPrivateProfileInt(g_szIniSection, g_szIniFindIconsEntry, 1, sIniFile);
	m_nOverwrite = GetPrivateProfileInt(g_szIniSection, g_szIniNoOverwriteEntry, 1, sIniFile);

	POSITION pos = GetFirstViewPosition();
    CFavOrgView* pView = (CFavOrgView*) GetNextView(pos);
	
	((CButton*) pView->GetDlgItem(IDC_ALLOWUPDATE))->SetCheck(m_nAllowUpdate);
	((CButton*) pView->GetDlgItem(IDC_FINDFAVICONS))->SetCheck(m_nFindIcons);
	((CButton*) pView->GetDlgItem(IDC_NOOVERWRITE))->SetCheck(m_nOverwrite);

	int nShowLegend = GetPrivateProfileInt(g_szIniSection, g_szShowLegendEntry, 0, sIniFile);
	m_bShowLegend = nShowLegend == 1; 
	pView->ShowLegend(m_bShowLegend);

	return TRUE;
}

void CFavOrgDoc::FinalInit()
{
	if (m_pctlFavTree)
	{
		m_pctlFavTree->SetIconStore(m_sStorePath);
		m_pctlFavTree->SetTimeout(m_nTimeout);
		m_pctlFavTree->SetConnectCheck(m_nNoConnectTest == 1);
		m_pctlFavTree->SetRedirFlag(m_nAllowUpdate == 1);
	}
}

void CFavOrgDoc::OnCloseDocument() 
{
	if (m_bLocked)
		return;

	// Notify tree control
	m_pctlFavTree->CancelAction();

	CString sIniFile = ((CFavOrgApp*) AfxGetApp())->m_sIniFile;

	// Store user options
	WritePrivateProfileString(g_szIniSection, g_szIniStoreEntry, m_sStorePath, sIniFile);
	
	CStringEx sTimeout;
	sTimeout.Format("%d", m_nTimeout);
	WritePrivateProfileString(g_szIniSection, g_szIniTimeoutEntry, sTimeout, sIniFile);

	CString sAllowUpdate;
	sAllowUpdate.Format("%d", m_nAllowUpdate);
	WritePrivateProfileString(g_szIniSection, g_szIniAllowUpdateEntry, sAllowUpdate, sIniFile);

	CString sFindIcons;
	sFindIcons.Format("%d", m_nFindIcons);
	WritePrivateProfileString(g_szIniSection, g_szIniFindIconsEntry, sFindIcons, sIniFile);

	CString sNoOverwrite;
	sNoOverwrite.Format("%d", m_nOverwrite);
	WritePrivateProfileString(g_szIniSection, g_szIniNoOverwriteEntry, sNoOverwrite, sIniFile);

	CString sConnectTest;
	sConnectTest.Format("%d", m_nNoConnectTest);
	WritePrivateProfileString(g_szIniSection, g_szNoCheckEntry, sConnectTest, sIniFile);

	CString sShowLegend;
	sShowLegend.Format("%d", m_bShowLegend ? 1 : 0);
	WritePrivateProfileString(g_szIniSection, g_szShowLegendEntry, sShowLegend, sIniFile);

	CString sPos;

	POSITION pos = GetFirstViewPosition();
    CFavOrgView* pView = (CFavOrgView*) GetNextView(pos);
	if (pView && pView->m_pdlgLegend)
		pView->m_pdlgLegend->GetWindowRect(m_rectLegend);

	sPos.Format("%d", m_rectLegend.top);
	WritePrivateProfileString(g_szIniSection, g_szLegendTopEntry, sPos, sIniFile);
	sPos.Format("%d", m_rectLegend.bottom);
	WritePrivateProfileString(g_szIniSection, g_szLegendBottomEntry, sPos, sIniFile);
	sPos.Format("%d", m_rectLegend.left);
	WritePrivateProfileString(g_szIniSection, g_szLegendLeftEntry, sPos, sIniFile);
	sPos.Format("%d", m_rectLegend.right);
	WritePrivateProfileString(g_szIniSection, g_szLegendRightEntry, sPos, sIniFile);

	// Store status of user links
	m_pctlFavTree->StoreStatus();

	CDocument::OnCloseDocument();
}

/////////////////////////////////////////////////////////////////////////////
// CFavOrgDoc diagnostics

#ifdef _DEBUG
void CFavOrgDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFavOrgDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFavOrgDoc commands


BOOL CFavOrgDoc::CanCloseFrame(CFrameWnd* pFrame) 
{
    CFavOrgView* pView = (CFavOrgView*) pFrame->GetActiveView();
	if (pView)
		return !pView->IsLocked();
	else
		return CDocument::CanCloseFrame(pFrame);
}

void CFavOrgDoc::OnUpdateAppExit(CCmdUI* pCmdUI) 
{
	// We can't exit while FavOrg processes shortcuts
	POSITION pos = GetFirstViewPosition();
    CFavOrgView* pView = (CFavOrgView*) GetNextView(pos);

	if (pView)
		pCmdUI->Enable(!pView->IsLocked());
	else
		pCmdUI->Enable(TRUE);
}

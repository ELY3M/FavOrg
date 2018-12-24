// FAVORG Version 1.4
// Copyright (c) 2000 Ziff Davis Media, Inc.
// All rights reserved.
// First Published in PC Magazine, US Edition, November 7, 2000.
// Programmer: Patrick Philippot

#include "stdafx.h"

#include "FavOrg.h"
#include "FavOrgHlp.h"
#include "multiseltreectrl.h"
#include <wininet.h>
#include "favorgtreectl.h"
#include "legenddlg.h"

#include "MainFrm.h"
#include "FavOrgDoc.h"
#include "FavOrgView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFavOrgApp

BEGIN_MESSAGE_MAP(CFavOrgApp, CWinApp)
	//{{AFX_MSG_MAP(CFavOrgApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(IDC_FAVORGHELP, OnFavorgHelp)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFavOrgApp construction

CFavOrgApp::CFavOrgApp()
{
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFavOrgApp object

CFavOrgApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFavOrgApp initialization

BOOL CFavOrgApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	GetModuleFileName(AfxGetInstanceHandle(), m_sIniFile.GetBuffer(MAX_PATH), MAX_PATH);
	m_sIniFile.ReleaseBuffer();

	DWORD	dwLen = MAX_USERNAMELEN;

	if ((!::GetUserName(m_sUsername.GetBuffer(MAX_USERNAMELEN), &dwLen))
		||
		(dwLen == 0)
		||
		(dwLen == 1))
	{
		m_sUsername.ReleaseBuffer();
		m_sUsername.Empty();
	}
	else
	{
		m_sUsername.ReleaseBuffer();
		if (m_sUsername.FindOneOf(_T("\\/:\"*?<>|")) != -1)
			m_sUsername.Empty();
	}

	m_sIniFile.Delete(m_sIniFile.ReverseFind('.'));
	m_sIniFile += _T("_");
	m_sIniFile += m_sUsername;
	m_sIniFile += ".ini";

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CFavOrgDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CFavOrgView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CFavOrgApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CFavOrgApp message handlers


void CFavOrgApp::OnFavorgHelp() 
{
	WinHelp(HIDR_MAINFRAME);	
}

// FAVORG Version 1.4
// Copyright (c) 2000 Ziff Davis Media, Inc.
// All rights reserved.
// First Published in PC Magazine, US Edition, November 7, 2000.
// Programmer: Patrick Philippot

#include "stdafx.h"
#include "FavOrg.h"
#include "FavOrgHlp.h"
#include "OptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char const g_szBrowseTitle[] = "Select a FavIcon folder\n(";

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog


COptionsDlg::COptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsDlg)
	m_nTimeout = 0;
	m_bNoConnectiontest = FALSE;
	//}}AFX_DATA_INIT
}


void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDlg)
	DDX_Text(pDX, IDC_EDTIMEOUT, m_nTimeout);
	DDV_MinMaxUInt(pDX, m_nTimeout, 15, 60);
	DDX_Check(pDX, IDC_NOCONNECTIONTEST, m_bNoConnectiontest);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	//{{AFX_MSG_MAP(COptionsDlg)
	ON_BN_CLICKED(IDC_SELFOLDER, OnSelFolder)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg message handlers

void COptionsDlg::OnSelFolder() 
{
	// Select folder where icons are stored
	BROWSEINFO bi;
	
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = m_hWnd;
	bi.ulFlags = BIF_RETURNONLYFSDIRS;

	char szTitle[MAX_PATH + 50];
	strcpy(szTitle, g_szBrowseTitle);
	strcat(strcat(szTitle, m_sStorePath), ")");

	bi.lpszTitle = szTitle;

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (pidl)
	{
		SHGetPathFromIDList(pidl, m_sStorePath.GetBuffer(MAX_PATH));
		m_sStorePath.ReleaseBuffer();
	}
}

BOOL COptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetDlgItem(IDC_SELFOLDER)->EnableWindow(m_bCanSetFolder);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsDlg::OnHelp() 
{
	AfxGetApp()->WinHelp(HIDD_OPTIONS);
}

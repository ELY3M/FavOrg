// FAVORG Version 1.4
// Copyright (c) 2000 Ziff Davis Media, Inc.
// All rights reserved.
// First Published in PC Magazine, US Edition, November 7, 2000.
// Programmer: Patrick Philippot

#include "stdafx.h"
#include "favorg.h"
#include "favorghlp.h"
#include "multiseltreectrl.h"
#include "favorgtreectl.h"
#include "ByStateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CByStateDlg dialog


CByStateDlg::CByStateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CByStateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CByStateDlg)
	m_bPageNotFound = FALSE;
	m_bAccessDenied = FALSE;
	m_bSiteNone = FALSE;
	m_bTimeOut = FALSE;
	//}}AFX_DATA_INIT
}


void CByStateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CByStateDlg)
	DDX_Check(pDX, IDC_CHECK_PAGENONE, m_bPageNotFound);
	DDX_Check(pDX, IDC_CHECK_ACCESSDENIED, m_bAccessDenied);
	DDX_Check(pDX, IDC_CHECK_SITENONE, m_bSiteNone);
	DDX_Check(pDX, IDC_CHECK_TIMEOUT, m_bTimeOut);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CByStateDlg, CDialog)
	//{{AFX_MSG_MAP(CByStateDlg)
	ON_BN_CLICKED(IDC_CHECK_ACCESSDENIED, OnCheckAccessDenied)
	ON_BN_CLICKED(IDC_CHECK_PAGENONE, OnCheckPageNone)
	ON_BN_CLICKED(IDC_CHECK_SITENONE, OnCheckSiteNone)
	ON_BN_CLICKED(IDC_CHECK_TIMEOUT, OnCheckTimeout)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CByStateDlg implementation

BOOL CByStateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetDlgItemText(IDC_COUNT, "0");

	CString sCount;

	// Count how many shortcuts have a given status
	m_nCount = 0;
	CountItemsByState(m_pFavTree->GetRootItem(), TRUE, FALSE, FALSE, FALSE);
	m_nCountAccessDenied = m_nCount;
	sCount.Format("%d items", m_nCount);
	SetDlgItemText(IDC_ACCESSDENIED_COUNT, sCount);

	m_nCount = 0;
	CountItemsByState(m_pFavTree->GetRootItem(), FALSE, TRUE, FALSE, FALSE);
	m_nCountTimeout = m_nCount;
	sCount.Format("%d items", m_nCount);
	SetDlgItemText(IDC_TIMEOUT_COUNT, sCount);

	m_nCount = 0;
	CountItemsByState(m_pFavTree->GetRootItem(), FALSE, FALSE, TRUE, FALSE);
	m_nCountPageNotFound = m_nCount;
	sCount.Format("%d items", m_nCount);
	SetDlgItemText(IDC_PAGENOTFOUND_COUNT, sCount);

	m_nCount = 0;
	CountItemsByState(m_pFavTree->GetRootItem(), FALSE, FALSE, FALSE, TRUE);
	m_nCountSiteNone = m_nCount;
	sCount.Format("%d items", m_nCount);
	SetDlgItemText(IDC_SITENOTFOUND_COUNT, sCount);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CByStateDlg::CheckItemState(HTREEITEM hItem, BOOL bAccessDenied, BOOL bTimeout, BOOL bPageNotFound, BOOL bSiteNone)
{
	int nActualState = m_pFavTree->GetItemState(hItem, TVIS_STATEIMAGEMASK) & ~0xFF;

	// If item has one of the user defined state, delete it
	if (((nActualState == INDEXTOSTATEIMAGEMASK(STATE_INDEX_ACCESSDENIED)) && bAccessDenied)
		 ||
		 ((nActualState == INDEXTOSTATEIMAGEMASK(STATE_INDEX_PAGENONE)) && bPageNotFound)
		 ||
		 ((nActualState == INDEXTOSTATEIMAGEMASK(STATE_INDEX_SITENONE))&& bSiteNone)
		 ||
		 ((nActualState == INDEXTOSTATEIMAGEMASK(STATE_INDEX_TIMEOUT)) && bTimeout))
	{
		m_nCount++;
	}
}

void CByStateDlg::CountItemsByState(HTREEITEM hItem, BOOL bAccessDenied, BOOL bTimeout, BOOL bPageNotFound, BOOL bSiteNone)
{
	if (m_pFavTree->GetItemData(hItem) == ITEM_IS_FOLDER)
	{
		// It's a folder
		// Process all items in this folder
		HTREEITEM hChildItem = m_pFavTree->GetChildItem(hItem);

		while (hChildItem != NULL)
		{
			if (m_pFavTree->GetItemData(hChildItem) == ITEM_IS_FOLDER)
				CountItemsByState(hChildItem, bAccessDenied, bTimeout, bPageNotFound, bSiteNone);
			else
				CheckItemState(hChildItem, bAccessDenied, bTimeout, bPageNotFound, bSiteNone);
			hChildItem = m_pFavTree->GetNextItem(hChildItem, TVGN_NEXT);
		}
	}
	else
		CheckItemState(hItem, bAccessDenied, bTimeout, bPageNotFound, bSiteNone);
}

void CByStateDlg::CountByState()
{
	m_nCount = 0;

	if (m_bAccessDenied)
		m_nCount += m_nCountAccessDenied;
	if (m_bTimeOut)
		m_nCount += m_nCountTimeout;
	if (m_bPageNotFound)
		m_nCount += m_nCountPageNotFound;
	if (m_bSiteNone)
		m_nCount += m_nCountSiteNone;

	CString sCount;
	sCount.Format("%d", m_nCount);
	SetDlgItemText(IDC_COUNT, sCount);
}

void CByStateDlg::OnCheckAccessDenied() 
{
	m_bAccessDenied = ((CButton*) GetDlgItem(IDC_CHECK_ACCESSDENIED))->GetCheck() == 1;
	CountByState();
}

void CByStateDlg::OnCheckPageNone() 
{
	m_bPageNotFound = ((CButton*) GetDlgItem(IDC_CHECK_PAGENONE))->GetCheck() == 1;
	CountByState();
}

void CByStateDlg::OnCheckSiteNone() 
{
	m_bSiteNone = ((CButton*) GetDlgItem(IDC_CHECK_SITENONE))->GetCheck() == 1;
	CountByState();
}

void CByStateDlg::OnCheckTimeout() 
{
	m_bTimeOut = ((CButton*) GetDlgItem(IDC_CHECK_TIMEOUT))->GetCheck() == 1;
	CountByState();
}


void CByStateDlg::OnHelp() 
{
	AfxGetApp()->WinHelp(HIDD_DELETEBYSTATUS);
}

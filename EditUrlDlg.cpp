// FAVORG Version 1.4
// Copyright (c) 2000 Ziff Davis Media, Inc.
// All rights reserved.
// First Published in PC Magazine, US Edition, November 7, 2000.
// Programmer: Patrick Philippot

#include "stdafx.h"
#include "FavOrg.h"
#include "EditUrlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditUrlDlg dialog


CEditUrlDlg::CEditUrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditUrlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditUrlDlg)
	m_sRefLabel = _T("");
	m_sNewUrl = _T("");
	//}}AFX_DATA_INIT
}


void CEditUrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditUrlDlg)
	DDX_Text(pDX, IDC_REFNAME, m_sRefLabel);
	DDX_Text(pDX, IDC_URLED, m_sNewUrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditUrlDlg, CDialog)
	//{{AFX_MSG_MAP(CEditUrlDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditUrlDlg message handlers

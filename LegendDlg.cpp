// FAVORG Version 1.4
// Copyright (c) 2000 Ziff Davis Media, Inc.
// All rights reserved.
// First Published in PC Magazine, US Edition, November 7, 2000.
// Programmer: Patrick Philippot

#include "stdafx.h"
#include "favorg.h"
#include "LegendDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLegendDlg dialog


CLegendDlg::CLegendDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLegendDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLegendDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CLegendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLegendDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLegendDlg, CDialog)
	//{{AFX_MSG_MAP(CLegendDlg)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLegendDlg message handlers

void CLegendDlg::OnClose() 
{
	// Notify view
	GetParent()->SendMessage(WM_COMMAND, CMD_LEGEND_CLOSING, 0);
}

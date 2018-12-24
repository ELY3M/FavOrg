// FAVORG Version 1.4
// Copyright (c) 2000 Ziff Davis Media, Inc.
// All rights reserved.
// First Published in PC Magazine, US Edition, November 7, 2000.
// Programmer: Patrick Philippot

#include "stdafx.h"
#include "favorg.h"
#include "TreeCtrlEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEdit

CTreeCtrlEdit::CTreeCtrlEdit()
{
	m_hCurItem = NULL;
	m_pTreeCtrl = NULL;
}

CTreeCtrlEdit::~CTreeCtrlEdit()
{
}


BEGIN_MESSAGE_MAP(CTreeCtrlEdit, CEdit)
	//{{AFX_MSG_MAP(CTreeCtrlEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEdit message handlers

BOOL CTreeCtrlEdit::PreTranslateMessage(MSG* pMsg) 
{
	// Prevent early termination of program when
	// the user hits the ENTER or the ESCAPE key
	if((m_hCurItem != NULL) && (m_pTreeCtrl) && (pMsg->message == WM_KEYDOWN))
	{
		CString m_Test;

		switch(pMsg->wParam)
		{
			case VK_RETURN:
				GetWindowText(m_Test);
				m_pTreeCtrl->SetItemText(m_hCurItem, m_Test);
				break;

			case VK_ESCAPE:
				break;
		}

		::TranslateMessage(pMsg);
		::DispatchMessage(pMsg);

		return TRUE;
	}
	
	return CEdit::PreTranslateMessage(pMsg);
}

void CTreeCtrlEdit::SetCurItem(HTREEITEM hItem)
{
	m_hCurItem = hItem;
}

void CTreeCtrlEdit::SetTreeCtrl(CTreeCtrl* pTreeCtrl)
{
	m_pTreeCtrl = pTreeCtrl;
}


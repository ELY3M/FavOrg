// FAVORG Version 1.4
// Copyright (c) 2000 Ziff Davis Media, Inc.
// All rights reserved.
// First Published in PC Magazine, US Edition, November 7, 2000.
// Programmer: Patrick Philippot

#include "stdafx.h"
#include "MultiselTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMultiselTreeCtrl
//
// This code is a major rework of classes found at www.codeguru.com
// It partially implements multiple selection in the standard
// Windows tree control.

CMultiselTreeCtrl::CMultiselTreeCtrl()
{
	m_hItemFirstSel = NULL;
}

CMultiselTreeCtrl::~CMultiselTreeCtrl()
{
}

BEGIN_MESSAGE_MAP(CMultiselTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CMultiselTreeCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultiselTreeCtrl message handlers

void CMultiselTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	UINT flags;
	HTREEITEM m_hSelectedItem = HitTest(point, &flags);

	if ((flags & TVHT_ONITEM) == 0)
		m_hSelectedItem = NULL;

	if (m_hSelectedItem == NULL)	
	{
		//UpdateSelectedItems();
		ClearSelection(TRUE);
		CTreeCtrl::OnLButtonDown(nFlags, point);
		return;
	}

	BOOL bAlreadySelected = (GetItemState(m_hSelectedItem, TVIS_SELECTED) & TVIS_SELECTED) != 0;

	if ((!bAlreadySelected) && (flags & TVHT_ONITEM))
	{
		if (nFlags & MK_CONTROL)
		{
			//CTRL Key is set so select multiple items.
			if (!m_SelItemList.IsEmpty()) //there are few items already..
			{
				//Verify current item is a sibling of existing item in list..
				if (!IsSibling((HTREEITEM)m_SelItemList.GetTail(), m_hSelectedItem))
					return;
			}

			m_SelItemList.AddTail(m_hSelectedItem);
			// Toggle selection state
			UINT uNewSelState =  GetItemState(m_hSelectedItem, 
											  TVIS_SELECTED) & TVIS_SELECTED ? 0 : TVIS_SELECTED;

			// Get old selected (focus) item and state
			HTREEITEM hItemOld = GetSelectedItem();
			UINT uOldSelState  = hItemOld ? GetItemState(hItemOld, TVIS_SELECTED) : 0;

			// Select new item
			if (GetSelectedItem() == m_hSelectedItem)
				SelectItem(NULL);
			
			CTreeCtrl::OnLButtonDown(nFlags, point);

			// Set proper selection (highlight) state for new item
			SetItemState(m_hSelectedItem, uNewSelState, TVIS_SELECTED);

			// Restore state of old selected item
			if (hItemOld && hItemOld != m_hSelectedItem)
				SetItemState(hItemOld, uOldSelState, TVIS_SELECTED);

			m_hItemFirstSel = NULL;

			return;
		}
		else if (nFlags & MK_SHIFT)
		{
			//SHIFT Key is set so select multiple items.
			UINT flag;
			HTREEITEM hItem = HitTest(point, &flag);

			// Initialize the reference item if this is the first shift selection
			if (!m_hItemFirstSel)
				m_hItemFirstSel = GetSelectedItem();

			// Select new item
			if (GetSelectedItem() == hItem)
				SelectItem(NULL);			// to prevent edit

			CTreeCtrl::OnLButtonDown(nFlags, point);

			if (m_hItemFirstSel)
			{
				SelectItems(m_hItemFirstSel, hItem);
				return;
			}
		}
		else if (nFlags & MK_SHIFT & MK_CONTROL)
		{
		}
		else
		{
			ClearSelection(FALSE);
			m_hItemFirstSel = NULL;
			m_SelItemList.AddTail(m_hSelectedItem);

			CTreeCtrl::OnLButtonDown(nFlags, point);
		}
	}
	else if (bAlreadySelected && (nFlags & MK_CONTROL))
	{
		m_SelItemList.RemoveAt(m_SelItemList.Find(m_hSelectedItem));
		SetItemState(m_hSelectedItem, 0, TVIS_SELECTED);
	}
	else if (bAlreadySelected)
	{
		if (m_SelItemList.GetCount() > 1)
		{
			ClearSelection(FALSE);
			SelectItem(NULL);
			m_SelItemList.AddTail(m_hSelectedItem);
			SelectItem(m_hSelectedItem);
		}

		CTreeCtrl::OnLButtonDown(nFlags, point);
		UpdateSelectedItems();
	}
	else
	{
		CTreeCtrl::OnLButtonDown(nFlags, point);
		UpdateSelectedItems();
	}
}

void CMultiselTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_F2)
	{
		HTREEITEM hItem = GetSelectedItem();
		if (hItem != NULL)
			EditLabel(hItem);
		
		return;
	}

	if ((nChar == VK_UP || nChar == VK_DOWN) && (GetKeyState(VK_SHIFT) & 0x8000))
	{
		// Initialize the reference item if this is the first shift selection
		if (!m_hItemFirstSel)
		{
			ClearSelection(FALSE);
			m_hItemFirstSel = GetSelectedItem();
			m_SelItemList.AddTail(m_hItemFirstSel);
		}

		// Find which item is currently selected
		HTREEITEM	hItemPrevSel = GetSelectedItem();
		HTREEITEM	hItemNext;
		BOOL		bIsDir = GetItemData(m_hItemFirstSel);

		if (nChar==VK_UP)
			hItemNext = GetPrevVisibleItem(hItemPrevSel);
		else
			hItemNext = GetNextVisibleItem(hItemPrevSel);

		if (hItemNext && (bIsDir == (BOOL) GetItemData(hItemNext)))
		{
			// Determine if we need to reselect previously selected item
			BOOL bReselect = !(GetItemState(hItemNext, TVIS_SELECTED) & TVIS_SELECTED);

			// Select the next item - this will also deselect the previous item
			m_SelItemList.AddTail(hItemNext);
			SelectItem(hItemNext);

			// Reselect the previously selected item
			if (bReselect)
				SetItemState(hItemPrevSel, TVIS_SELECTED, TVIS_SELECTED);
		}
		return;
	}
	else if (nChar == VK_UP || nChar == VK_DOWN)
	{
		m_hItemFirstSel = NULL;
		ClearSelection(FALSE);

		CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);

		HTREEITEM hItem = GetSelectedItem();

		if (hItem)
			m_SelItemList.AddTail(hItem);

		return;
	}
	else if (nChar >= VK_SPACE)
	{
		m_hItemFirstSel = NULL;
		ClearSelection(FALSE);
	}	

	CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMultiselTreeCtrl::ClearSelection(BOOL bCleanTree)
{
	BOOL bMustUpdate = FALSE;

	if (!m_SelItemList.IsEmpty())
	{
		HTREEITEM hItem;

		POSITION pos = m_SelItemList.GetHeadPosition();

		while(pos != NULL)
		{
			hItem = m_SelItemList.GetNext(pos);
			SetItemState(hItem, 0, TVIS_SELECTED);
		}

		bMustUpdate =  m_SelItemList.GetCount() > 1;
		m_SelItemList.RemoveAll();
	}

	m_hItemFirstSel = NULL;
	
	if (bCleanTree)
		SelectItem(NULL);

	if (bMustUpdate)
		Invalidate();
}

BOOL CMultiselTreeCtrl::IsSibling(HTREEITEM hItemFrom, HTREEITEM hItemTo)
{
	return (GetParentItem(hItemFrom) == GetParentItem(hItemTo));
}

BOOL CMultiselTreeCtrl::SelectItems(HTREEITEM hItemFrom, HTREEITEM hItemTo)
{
	//If the given items aren't sibling do not multi-select
	if (!IsSibling(hItemFrom, hItemTo))
		return FALSE;

	ClearSelection(TRUE);

	HTREEITEM	hParentItem = GetParentItem(hItemFrom);
	HTREEITEM	hChildItem = GetChildItem(hParentItem);
	BOOL		bStarted = FALSE;
	BOOL		bLastDone = FALSE;

	while ((hChildItem != NULL) && (!bStarted || (bStarted && !bLastDone)))
	{
		bLastDone = bStarted && ((hChildItem == hItemFrom) || (hChildItem == hItemTo));
		bStarted = bStarted || (hChildItem == hItemFrom) || (hChildItem == hItemTo);

		if (bStarted)
		{
			SetItemState(hChildItem, TVIS_SELECTED, TVIS_SELECTED);
			m_SelItemList.AddTail(hChildItem);
		}

		if (!bLastDone)
			hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
		else
			// Useless but otherwise the dialog box will not get
			// the selchanged notification
			SelectItem(hChildItem);
	}

	return TRUE;
}

void CMultiselTreeCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	CTreeCtrl::OnKillFocus(pNewWnd);
	
	UpdateSelectedItems();
}

void CMultiselTreeCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	CTreeCtrl::OnSetFocus(pOldWnd);
	
	UpdateSelectedItems();
}

void CMultiselTreeCtrl::UpdateSelectedItems()
{ 
	if (m_SelItemList.IsEmpty())
	{
		SelectItem(NULL);
		return;
	}

	HTREEITEM hCurItem = NULL;

	POSITION	pos = m_SelItemList.GetHeadPosition();
	CRect		Rect;

	while (pos != NULL)
	{
		hCurItem = m_SelItemList.GetNext(pos);
		GetItemRect(hCurItem, Rect, TRUE);
		InvalidateRect(Rect); 
	}

	UpdateWindow();
}

void CMultiselTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	UpdateSelectedItems();	
}

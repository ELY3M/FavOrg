// FAVORG Version 1.4
// Copyright (c) 2000 Ziff Davis Media, Inc.
// All rights reserved.
// First Published in PC Magazine, US Edition, November 7, 2000.
// Programmer: Patrick Philippot

#if !defined(AFX_FavOrgTREECTL_H__4C2CD454_403B_11D4_9041_00A0249EABF4__INCLUDED_)
#define AFX_FavOrgTREECTL_H__4C2CD454_403B_11D4_9041_00A0249EABF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FavOrgTreeCtl.h : header file
//

#include <comdef.h>
#include <intshcut.h>
#include "shelldef.h"
#include "TreeCtrlEdit.h"
#include <ras.h>

// Icon and Bitmap related structures
// Used for the security check made by Favorg

typedef struct tagICONDIRENTRY
{
    BYTE  bWidth;
    BYTE  bHeight;
    BYTE  bColorCount;
    BYTE  bReserved;
    WORD  wPlanes;
    WORD  wBitCount;
    DWORD dwBytesInRes;
    DWORD dwImageOffset;
} ICONDIRENTRY;

typedef struct tagICONHEADER
{
    WORD          idReserved;
    WORD          idType;
    WORD          idCount;
    ICONDIRENTRY  idEntries[1];
} ICONHEADER;

#define BM 0x4D42
#define INTERNET_DIAL_FORCE_PROMPT		0x2000

typedef DWORD (APIENTRY* RASENUMENTRIES) (LPCSTR, LPCSTR, LPRASENTRYNAMEA, LPDWORD, LPDWORD);
typedef DWORD (APIENTRY* RASENUMCONNECTIONS) (LPRASCONNA, LPDWORD, LPDWORD);

/////////////////////////////////////////////////////////////////////////////
// CFavOrgTreeCtl window

class CFavOrgTreeCtl : public CMultiselTreeCtrl
{
// Construction
public:
	CFavOrgTreeCtl();

// Attributes
public:

private:
	CString				m_sPickedShortcut;
	CImageList			m_imgStd;
	CImageList			m_imgState;
	DWORD				m_dwConnection;
	BOOL				m_bDialed;
	int					m_nNumItems;
	int					m_nLoadedItems;
	BOOL				m_bProcessing;
	CTreeCtrlEdit		m_ctlEdit;
	CStringEx			m_sOldLabel;
	CStringEx			m_sOldFilename;
	HTREEITEM			m_hLastCtxtItem;
	CMap<CString, LPCSTR, int, int&>
						m_StatusMap;
	CMapStringToString	m_InfoMap;
	CStringEx			m_sStatusFile;
	CStringEx			m_sInfoFile;
	int					m_nFolderIconSel;
	int					m_nFolderIconNotSel;
	int					m_nStdIconNotSel;
	int					m_nJobsToRun;
	int					m_nActiveThreads;
	HMODULE				m_hRasDll;
	RASENUMENTRIES		m_pRasEnumEntries;
	RASENUMCONNECTIONS	m_pRasEnumConnections;
	CRITICAL_SECTION	m_csUrlLink;
	CRITICAL_SECTION	m_csDisplayMsg;

	static BOOL					m_bCancel;
	static HTREEITEM			m_hRootItem;
	static CString				m_sFavPath;
	static BOOL					m_bCanRedirect;
	static int					m_nTimeout;
	static BOOL					m_bLetUserScroll;
	//static CInternetSession*	m_pInet;
	static CProgressCtrl*		m_pProgressCtl;
	static CStringEx			m_sIconStore;
	static CString				m_sCurrentMsg;
	static BOOL					m_bConnected;
	static BOOL					m_bDoNotCheck;
	static HANDLE				m_hScanSem;

public:
	static CFavOrgTreeCtl*		mp_Me;

// Operations
public:

	typedef struct tagCheckFlags
	{
		BOOL bDeadLinksOnly;
		BOOL bNoOverwrite;
	}
	CHECKFLAGS, *PCHECKFLAGS;

	typedef struct tagSCANTHREADPARAMS
	{
		HTREEITEM			hItem;
		BOOL				bDeadLinksOnly;
		BOOL				bNoOverwrite;
	}
	SCANTHREADPARAMS, *PSCANTHREADPARAMS;

	typedef struct tagHTTPTIMEOUTPARAMS
	{
		CHttpFile*	pFile;
		CString		sServer;
	}
	HTTPTIMEOUTPARAMS, *PHTTPTIMEOUTPARAMS;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFavOrgTreeCtl)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFavOrgTreeCtl();

	int				GetItemCount() {return m_nNumItems;};
	void			CancelAction();

	BOOL			FvInternetConnect(BOOL* bDialed);

	void			SetRedirFlag(BOOL bOn){m_bCanRedirect = bOn;};
	int				SetTimeout(int nTimeout);
	CProgressCtrl*	SetProgressBar(CProgressCtrl* pProgressBar);
	CStringEx		SetIconStore(CStringEx sIconStore);

	void			LoadFavorites(CString sIniFilename);
	void			CheckAndFindIcons(BOOL bDeadLinksOnly, BOOL bNoOverwrite);
	void			ProcessItems(BOOL bDeadLinksOnly, BOOL bNoOverwrite);
	void			AssignCustomIcon(CString sIconFilename, int nIconIndex, HTREEITEM hItem);
	void			AssignThisImage(HTREEITEM hItem);
	void			PickCurrentIcon(HTREEITEM hItem);
	void			RemoveSelected();
	void			DeleteByState();
	void			UpdateUrl(HTREEITEM hItem);
	void			ResetDeadLinks();

	BOOL			GetConnectState();
	void			SetConnectCheck(BOOL bNoConnectCheck);

	BOOL			SelectionExist(BOOL& bOneShortcut, BOOL& bOneFolder);

	void			GenerateLog(CString sLogName);
	void			StoreStatus();
	void			StoreItemStatus(HTREEITEM hItem, BOOL bIsShortcut);
	void			StoreItemsStatus(HTREEITEM hItem);

	BOOL			IsPickedIcon() {return !m_sPickedShortcut.IsEmpty();};

	BOOL			ExecLocalCommand(UINT nID, BOOL bFromContextMenu);

	void			ExpandAll();
	void			ExpandCurrent();
	void			CollapseAll();
	void			CollapseCurrent();

	int				GetCommonStatus(HTREEITEM hItem);
	void			GetFoldersCommonStatus(HTREEITEM hItem, BOOL& bOK, int& nFirstStatus);
	void			GetItemCommonStatus(HTREEITEM hItem, BOOL& bOK, int& nFirstStatus);
	int				GetMenuToCheckFromState(int nState);

	virtual void	ClearSelection(BOOL bCleanTree);

protected:
	void		Initialize();

	int			AddStateIcon(int nId);
	int			CountShortcuts(CStringEx sRootPath);
	void		AddItemsFromPath(HTREEITEM hItem, CString sRootPath, CString sIniFilename);
	HTREEITEM	AddItem(CStringEx sPath, HTREEITEM hParent, CString sIniFilename, bool bIsDir = FALSE, int nIcon = -1, int nIcon2 = -1);
	int			CountSelected();
	int			CountFolder(CStringEx sPath);

	void		ProcessItem(HTREEITEM hItem, BOOL bDeadLinksOnly, BOOL bNoOverwrite);
	void		SetItemIcon(HTREEITEM hItem, CString sIconName, HICON hIcon, int nIndex, IShellLinkPtr psl);
	void		ProcessItemIcon(HTREEITEM hItem, HICON hIcon, CString sIconFilename, int nIndex);
	void		UpdateItemIcon(HTREEITEM hItem, HICON hIcon, CString sIconFilename, int nIndex);
	void		ProcessDeadLink(HTREEITEM hItem);
	void		ProcessDeadLinks(HTREEITEM hItem);
	void		ModifyStates(HTREEITEM hItem, int nStateIndex);
	void		RemoveItem(HTREEITEM hItem);
	void		DestroyItem(HTREEITEM hItem, BOOL bIsDir);
	void		ProcessItemsByState(HTREEITEM hItem, BOOL bAccessDenied, BOOL bTimeout, BOOL bPageNotFound, BOOL bSiteNone);
	void		ProcessItemByState(HTREEITEM hItem, BOOL bAccessDenied, BOOL bTimeout, BOOL bPageNotFound, BOOL bSiteNone);
	void		ClearMsg();
	void		PeekAndYield();
	void		ClearItemdata(HTREEITEM hItem);
	void		GenerateLogEntries(CFile* pLogfile, HTREEITEM hItem);
	void		GenerateLogEntry(CFile* pLogfile, HTREEITEM hItem);
	void		OutputString(CString sText, CFile* pLogfile);
	void		CollapseOrExpandAll(HTREEITEM hItem, BOOL bCollapse);

	static CString	MakeFullItemName(HTREEITEM hItem, BOOL bIsShortcut);
	static UINT		UpdateItemThread(LPVOID lParam);
	static UINT		HttpTimeOutThread(LPVOID lParam);
	static void		UpdateItem(HTREEITEM hItem, BOOL bDeadLinksOnly, BOOL bNoOverwrite);
	static BOOL		CheckURL(CInternetSession* pInet, CHttpConnection** ppHttpConnect, DWORD dwServiceType, CStringEx sUrl, CStringEx sServer, CStringEx sObject, int nPort, HTREEITEM hItem);
	static BOOL		GetOrSetURLFromShortcut(CString sFilename, CString& sURL, BOOL bGet);
	static void		DisplayMsg(int nTitleId, CString sContent);
	static BOOL		IsIconOrBmp(BYTE* pBuffer, DWORD dwLen);

	// Generated message map functions
protected:
	//{{AFX_MSG(CFavOrgTreeCtl)
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#define STATE_INDEX_QUESTION			1
#define STATE_INDEX_CHECKED				2
#define STATE_INDEX_PAGENONE			3
#define STATE_INDEX_MOVED				4
#define STATE_INDEX_SITENONE			5
#define STATE_INDEX_TIMEOUT				6
#define STATE_INDEX_ACCESSDENIED		7
#define STATE_INDEX_MOVED_UPDATED		8
#define STATE_INDEX_UNCHECKED			9
#define STATE_INDEX_RESET				10
#define STATE_INDEX_INPROCESS			11

#define ITEM_IS_FOLDER					-1

#define MAX_URL_LEN						2048
#define MAX_SCAN_THREADS				10
#define MAX_SCAN_THREAD_WAIT			INFINITE
#define MAX_SCAN_FINAL_WAIT				10000

#define WM_CAN_UNLOCK					WM_USER + 1
#define	WM_DISP_MSG						WM_USER + 2
#define WM_DISP_ICON					WM_USER + 3
#define WM_REFRESH_COUNT				WM_USER + 4

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FavOrgTREECTL_H__4C2CD454_403B_11D4_9041_00A0249EABF4__INCLUDED_)

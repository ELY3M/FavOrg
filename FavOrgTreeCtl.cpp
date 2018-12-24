// FAVORG Version 1.4
// Copyright (c) 2000 Ziff Davis Media, Inc.
// All rights reserved.
// First Published in PC Magazine, US Edition, November 7, 2000.
// Programmer: Patrick Philippot

#include "stdafx.h"
#include "MultiselTreeCtrl.h"
#include "FavOrg.h"
#include "FavOrgTreeCtl.h"
#include "FavOrgDoc.h"
#include "FavOrgView.h"
#include "ByStateDlg.h"
#include "editurldlg.h"
#include "SelIconDlg.h"
#include <ras.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char const g_szLocation[] = "Location: ";
char const g_szUrlExt[] = ".url";
char const g_szStateSection[] = "status";

BOOL				CFavOrgTreeCtl::m_bCancel = FALSE;
HTREEITEM			CFavOrgTreeCtl::m_hRootItem = NULL;
CString				CFavOrgTreeCtl::m_sFavPath = _T("");
BOOL				CFavOrgTreeCtl::m_bCanRedirect = FALSE;
int					CFavOrgTreeCtl::m_nTimeout = 0;
BOOL				CFavOrgTreeCtl::m_bLetUserScroll = FALSE;
//CInternetSession*	CFavOrgTreeCtl::m_pInet = NULL;
CProgressCtrl*		CFavOrgTreeCtl::m_pProgressCtl = NULL;
CStringEx			CFavOrgTreeCtl::m_sIconStore = _T("");
CString				CFavOrgTreeCtl::m_sCurrentMsg = _T("");
CFavOrgTreeCtl*		CFavOrgTreeCtl::mp_Me = NULL;
BOOL				CFavOrgTreeCtl::m_bConnected = FALSE;
BOOL				CFavOrgTreeCtl::m_bDoNotCheck = TRUE;
HANDLE				CFavOrgTreeCtl::m_hScanSem = NULL;
/////////////////////////////////////////////////////////////////////////////
// Worker thread checking URLs

UINT DoProcessItems(LPVOID pParam)
{
	// This thread handles the Find Dead links and FavIcons function
	// We must initialize COM for this thread
	CoInitialize(NULL);

	// We use the global pointer to our CFavOrgTreeCtl to do the job
	CFavOrgTreeCtl::mp_Me->ProcessItems(((CFavOrgTreeCtl::PCHECKFLAGS) pParam)->bDeadLinksOnly, ((CFavOrgTreeCtl::PCHECKFLAGS) pParam)->bNoOverwrite);

	CoUninitialize();
	return(0);
}

static int CALLBACK CustomCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	// lParamSort contains a pointer to the tree control.
	// The lParam of an item is just its handle.
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*) lParamSort;

	BOOL bIsFolder1 = lParam1 == ITEM_IS_FOLDER;
	BOOL bIsFolder2 = lParam2 == ITEM_IS_FOLDER;

	// The tree is sorted in such a way that folders are
	// displayed first
	if ((bIsFolder1 & bIsFolder2) || (!bIsFolder1 && !bIsFolder2))
		return 0;
	else if (bIsFolder1 && !bIsFolder2)
		return -1;
	else
		return 1;
}


/////////////////////////////////////////////////////////////////////////////
// CFavOrgTreeCtl

CFavOrgTreeCtl::CFavOrgTreeCtl()
{
	mp_Me = this;

	// Initiliaze object data
	m_dwConnection = 0;
	m_bConnected = FALSE;
	m_bDoNotCheck = TRUE;
	m_bCancel = FALSE;
	m_nNumItems = 0;
	m_bProcessing = FALSE;
	m_bCanRedirect = FALSE;
	m_bDialed = FALSE;
	m_nTimeout = 20;
	m_hRootItem = NULL;
	m_bLetUserScroll = FALSE;

	m_pProgressCtl = NULL;
	m_hLastCtxtItem = NULL;

	m_hRasDll = NULL;
	m_pRasEnumEntries = NULL;
	m_pRasEnumConnections = NULL;

	//m_pInet = new CInternetSession;

	InitializeCriticalSection(&m_csUrlLink);
	InitializeCriticalSection(&m_csDisplayMsg);

	CoInitialize(NULL);
}

CFavOrgTreeCtl::~CFavOrgTreeCtl()
{
	CoUninitialize();

	// Close WININET session
	//m_pInet->Close();
	//delete m_pInet;

	FreeLibrary(m_hRasDll);

	DeleteCriticalSection(&m_csUrlLink);
	DeleteCriticalSection(&m_csDisplayMsg);
}


BEGIN_MESSAGE_MAP(CFavOrgTreeCtl, CMultiselTreeCtrl)
	//{{AFX_MSG_MAP(CFavOrgTreeCtl)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginLabelEdit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndLabelEdit)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFavOrgTreeCtl implementation

void CFavOrgTreeCtl::Initialize()
{
	// Create Image List with state icons
	m_imgState.Create(16, 16, ILC_COLOR8 | ILC_MASK, 12, 1);
	m_imgStd.Create(16, 16, ILC_COLOR8 | ILC_MASK, 50, 100);

	// Dummy icon because the Tree control uses one-based indexes for states
	HICON hIcon;

	hIcon = LoadIcon(NULL, IDI_APPLICATION);
	m_imgState.Add(hIcon);
	DestroyIcon(hIcon);

	AddStateIcon(IDI_QUESTIONMARK);
	AddStateIcon(IDI_CHECKMARK);
	AddStateIcon(IDI_PAGENONE);
	AddStateIcon(IDI_PAGEMOVED);
	AddStateIcon(IDI_SITENONE);
	AddStateIcon(IDI_TIMEOUT);
	AddStateIcon(IDI_ACCESSDENIED);
	AddStateIcon(IDI_PAGEMOVED_UPDATED);
	AddStateIcon(IDI_UNCHECKED);
	AddStateIcon(IDI_RESET);
	AddStateIcon(IDI_INPROCESS);
	// Image indexes should be 1 to 11 respectively.

	// Set both image lists for control
	SetImageList(&m_imgStd, LVSIL_NORMAL);
	SetImageList(&m_imgState, LVSIL_STATE);

	// Compute status and info filenames
	GetModuleFileName(AfxGetInstanceHandle(), m_sStatusFile.GetBuffer(MAX_PATH), MAX_PATH);
	m_sStatusFile.ReleaseBuffer();
	m_sStatusFile.Delete(m_sStatusFile.ReverseFind('.'));
	m_sStatusFile += _T("_");
	m_sStatusFile += ((CFavOrgApp*) AfxGetApp())->m_sUsername;
	m_sInfoFile = m_sStatusFile;
	m_sStatusFile += _T(".sta");
	m_sInfoFile += _T(".ifo");

	// Initialize status map
	CFile logfile;
	if(logfile.Open(m_sStatusFile, CFile::modeRead))
	{
		try
		{
			CArchive ar(&logfile, CArchive::load);
			m_StatusMap.Serialize(ar);
			ar.Close();
			logfile.Close();
		}

		catch(CArchiveException* pEx)
		{
			pEx->Delete();
			logfile.Close();
			return;
		}
	}

	// Initialize information map
	if(logfile.Open(m_sInfoFile, CFile::modeRead))
	{
		try
		{
			CArchive ar(&logfile, CArchive::load);
			m_InfoMap.Serialize(ar);
			ar.Close();
			logfile.Close();
		}

		catch(CArchiveException* pEx)
		{
			pEx->Delete();
			logfile.Close();
			return;
		}
	}
}

int CFavOrgTreeCtl::AddStateIcon(int nId)
{
	// Helper adding state icons to image list
	HICON	hIcon;
	int		nIndex;

	hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
							  MAKEINTRESOURCE(nId),
							  IMAGE_ICON,
							  16, 16, NULL);
	nIndex = m_imgState.Add(hIcon);
	DestroyIcon(hIcon);

	return nIndex;
}

void CFavOrgTreeCtl::PreSubclassWindow()
{
	Initialize();

	CTreeCtrl::PreSubclassWindow();
}

// The following functions are just helper functions

CProgressCtrl* CFavOrgTreeCtl::SetProgressBar(CProgressCtrl* pProgressBar)
{
	CProgressCtrl* pTemp = m_pProgressCtl;
	m_pProgressCtl = pProgressBar;

	return pTemp;
}

CStringEx CFavOrgTreeCtl::SetIconStore(CStringEx sIconStore)
{
	CStringEx sTemp = m_sIconStore;
	m_sIconStore = sIconStore;
	m_sIconStore.AddSlash();

	return sTemp;
}

BOOL CFavOrgTreeCtl::GetConnectState()
{
	if (m_bDoNotCheck)
	// We consider we are connected
		return TRUE;

	if (m_hRasDll == NULL)
	{
		m_hRasDll = LoadLibrary("rasapi32.dll");
		if (m_hRasDll == NULL)
		{
			AfxMessageBox(IDS_RASALERTPROMPT, MB_OK | MB_ICONEXCLAMATION, 0);
			return FALSE;
		}

		m_pRasEnumEntries = (RASENUMENTRIES) GetProcAddress(m_hRasDll, "RasEnumEntriesA");
		m_pRasEnumConnections = (RASENUMCONNECTIONS) GetProcAddress(m_hRasDll, "RasEnumConnectionsA");
	}

	if ((m_pRasEnumEntries == NULL) || (m_pRasEnumConnections == FALSE))
	{
		AfxMessageBox(IDS_RASALERTPROMPT2, MB_OK | MB_ICONEXCLAMATION, 0);
		return FALSE;
	}

	LPRASCONN		lpRasConn;
	DWORD			nRet;
	DWORD			cb;
	DWORD			cConnections;

	lpRasConn = (LPRASCONN) malloc(sizeof(RASCONN));
	lpRasConn->dwSize = sizeof(RASCONN);
 	nRet = m_pRasEnumConnections(lpRasConn, &cb, &cConnections);
	free(lpRasConn);

	// Check if we are connected, try to connect if not,
	// and return state of connection
	if (cConnections == 0)
		FvInternetConnect(&m_bDialed);
	else
		m_bConnected = TRUE;

	return m_bConnected;
}

void CFavOrgTreeCtl::SetConnectCheck(BOOL bNoConnectCheck)
{
	m_bDoNotCheck = bNoConnectCheck;
}

BOOL CFavOrgTreeCtl::FvInternetConnect(BOOL* bDialed)
{
	// Try to establish a connection
	DWORD		dwFlags = 0;
	CWaitCursor waitCursor;

	*bDialed = FALSE;
	m_bConnected = FALSE;

	DWORD			cb;
	LPRASENTRYNAME	lpRasEntryName;
	DWORD			cEntries;

	cb = 0;
	lpRasEntryName = (LPRASENTRYNAME) malloc(sizeof(RASENTRYNAME));
	lpRasEntryName->dwSize = sizeof(RASENTRYNAME);
	m_pRasEnumEntries(NULL, NULL, lpRasEntryName, &cb, &cEntries);
	free (lpRasEntryName);

	// Try to dial or autodial
	if (cEntries > 0)
	{
		m_bConnected = InternetDial(m_hWnd,
									"",
									INTERNET_DIAL_FORCE_PROMPT,
									&m_dwConnection, 0) == ERROR_SUCCESS;

		*bDialed = m_bConnected;
	}

	return m_bConnected;
}

void CFavOrgTreeCtl::OnDestroy()
{
	ClearItemdata(m_hRootItem);

	CTreeCtrl::OnDestroy();

	// Disconnecting is allowed only when FavOrg itself established the
	// connection by using autodial.
	if (m_bDialed || (m_dwConnection != 0))
	{
		if (AfxMessageBox(IDS_CLOSECONNECTION, MB_YESNO | MB_ICONEXCLAMATION, 0) != IDYES)
			return;

		if (m_dwConnection != 0)
			InternetHangUp(m_dwConnection, 0);
		else if (m_bDialed)
			InternetAutodialHangup(0);
	}
}

int CFavOrgTreeCtl::CountShortcuts(CStringEx sRootPath)
{
	// Count total number of shorcuts
    CFileFind	finder;
	CStringEx	sFileSpec, sPath;
	int			nCount = 0;

	sFileSpec = sRootPath;
	sFileSpec.AddSlash();
	sFileSpec += CString("*.*");

	sPath = sRootPath;
	sPath.AddSlash();

    BOOL bOK = finder.FindFile(sFileSpec);
    while (bOK)
	{
        bOK = finder.FindNextFile();

		CStringEx sFilename = finder.GetFileName();

        if (finder.IsDirectory() && !finder.IsDots())
		{
			CString sDir = sPath + sFilename;
			nCount += CountShortcuts(sDir);
		}
		else if (!finder.IsDirectory() && (sFilename.GetExtension().CompareNoCase(g_szUrlExt) == 0))
		// We are looking for .URL files only
            nCount++;

		PeekAndYield();
	}

	return nCount;
}

void CFavOrgTreeCtl::LoadFavorites(CString sIniFilename)
{
	BOOL static bWarned = FALSE;

	LPITEMIDLIST pItemList;

	m_bProcessing = TRUE;
	m_nFolderIconSel = -1;
	m_nFolderIconNotSel = -1;
	m_nStdIconNotSel = -1;

	// Get location of Favorites folder
	SHGetSpecialFolderLocation (m_hWnd, CSIDL_FAVORITES, &pItemList);
	SHGetPathFromIDList(pItemList, m_sFavPath.GetBuffer(MAX_PATH));
	m_sFavPath.ReleaseBuffer();

	if (!bWarned
		 &&
		((GetDriveType(m_sFavPath.Left(3)) == DRIVE_REMOTE)
		  ||
		(m_sFavPath.Left(2).CompareNoCase("\\\\") == 0)))
	{
		AfxMessageBox(IDS_NETWORKALERT, MB_OK | MB_ICONEXCLAMATION, 0);
		bWarned = TRUE;
	}

	int nNumItems = CountShortcuts(m_sFavPath);
	m_nNumItems = 0;

	// Initialize progress bar
	if (m_pProgressCtl)
	{
		m_pProgressCtl->ShowWindow(SW_NORMAL);
		m_pProgressCtl->SetRange32(0, nNumItems);
		m_pProgressCtl->SetStep(1);
		m_pProgressCtl->SetPos(0);
	}

	// Clear Tree control
	ClearSelection(TRUE);
	if (m_hRootItem)
		ClearItemdata(m_hRootItem);
	DeleteAllItems();
	ImageList_RemoveAll(m_imgStd.m_hImageList);
	m_imgStd.DeleteImageList();
	m_imgStd.Create(16, 16, ILC_COLOR8 | ILC_MASK, 50, 100);

	// Create root item
	CString sRootItem;

	sRootItem.Format(IDS_FAVORITES, m_sFavPath);
	// Recursively constructs tree
	m_bLetUserScroll = FALSE;
	m_hRootItem = InsertItem(sRootItem);
	SetItemData(m_hRootItem, ITEM_IS_FOLDER);

	SetRedraw(FALSE);

	AddItemsFromPath(m_hRootItem, m_sFavPath, sIniFilename);

	m_nLoadedItems = m_nNumItems;
	m_nNumItems = nNumItems;

	TVSORTCB tvs;
	// Sort the tree control's items using a
	// custom callback procedure.
	tvs.hParent = m_hRootItem;
	tvs.lpfnCompare = CustomCompareProc;
	tvs.lParam = (LPARAM) this;
	SortChildren(m_hRootItem);
	SortChildrenCB(&tvs);

	SetRedraw(TRUE);
	EnsureVisible(m_hRootItem);
	Expand(m_hRootItem, TVE_EXPAND);

	SelectItem(NULL);

	if (m_pProgressCtl)
	{
		m_pProgressCtl->SetPos(0);
		m_pProgressCtl->ShowWindow(SW_HIDE);
	}
	ClearMsg();

	SetCursor(LoadCursor(NULL, IDC_ARROW));
	m_bProcessing = FALSE;
	m_bCancel = FALSE;
}

void CFavOrgTreeCtl::AddItemsFromPath(HTREEITEM hParent, CString sRootPath, CString sIniFilename)
{
	if (hParent != NULL)
	{
        CFileFind	finder;
		CStringEx	sFileSpec, sPath;

		sFileSpec = sRootPath;
		sFileSpec.AddSlash();
		sFileSpec += CString("*.*");

		sPath = sRootPath;
		sPath.AddSlash();

		DisplayMsg(IDS_MSG_FROMFOLDER, sRootPath);

        BOOL bOK = finder.FindFile(sFileSpec);
        while ((!m_bCancel) && bOK)
		{
            bOK = finder.FindNextFile();

			CStringEx sFilename = finder.GetFileName();

			// We exclude the Channels folder
			if (finder.IsDirectory()
				&&
				(sFilename.CompareNoCase("Channels") != 0)
				&&
				!finder.IsDots())
			{
				CString sDir = sPath + sFilename;
				HTREEITEM hItem = AddItem(sDir, hParent, sIniFilename, TRUE);
				AddItemsFromPath(hItem, sDir, sIniFilename);

				TVSORTCB tvs;
				// Sort the tree control's items using my
				// callback procedure.
				tvs.hParent = hItem;
				tvs.lpfnCompare = CustomCompareProc;
				tvs.lParam = (LPARAM) this;

				//SetRedraw(FALSE);
				SortChildren(hItem);
				SortChildrenCB(&tvs);

				CString sItemName = MakeFullItemName(hItem, FALSE);
				int nState;

				sItemName.MakeUpper();
				if (!m_StatusMap.Lookup(sItemName, nState))
					nState = TVIS_EXPANDED;
				SetItemState(hItem, nState, TVIS_EXPANDED);
				if (GetItemState(hItem, TVIS_EXPANDED) == TVIS_EXPANDED)
					Expand(hItem, TVE_EXPAND);
				else
					Expand(hItem, TVE_COLLAPSE);
			}
			else if (!finder.IsDirectory()
				     &&
					 (sFilename.GetExtension().CompareNoCase(g_szUrlExt) == 0))
				AddItem(sPath + sFilename, hParent, sIniFilename);
		}
	}
}

HTREEITEM CFavOrgTreeCtl::AddItem(CStringEx sPath, HTREEITEM hParent, CString sIniFilename, bool bIsDir, int nIcon, int nIcon2)
{
	int	nIconNotSel, nIconSel;

	PeekAndYield();

	if (bIsDir)
	// It's a folder - Retrieve icon
	{
		if (m_nFolderIconNotSel == -1) // Otherwise, we already have the icons
		{
			HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_FOLDER));
			m_nFolderIconNotSel = m_imgStd.Add(hIcon);
			DestroyIcon(hIcon);

			hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_FOLDERSEL));
			m_nFolderIconSel = m_imgStd.Add(hIcon);
			DestroyIcon(hIcon);

			SetItemImage(m_hRootItem, m_nFolderIconNotSel, m_nFolderIconSel);
		}

		nIconNotSel = nIcon != -1 ? nIcon : m_nFolderIconNotSel;
		nIconSel = nIcon2 != -1 ? nIcon2 : m_nFolderIconSel;
	}
	else
	// It's a shortcut - use COM to retrieve icon
	{
		IUniformResourceLocatorPtr pUrlLink(CLSID_InternetShortcut, NULL, CLSCTX_INPROC_SERVER);
		if (pUrlLink != NULL)
		{
			IShellLinkPtr psl(pUrlLink);
			IPersistFilePtr ppf(pUrlLink);

			if (ppf != NULL)
			{
				WCHAR wsz[MAX_PATH];

	            MultiByteToWideChar(CP_ACP, 0, sPath, -1, wsz, MAX_PATH);
		        // Load the shortcut.
			    if (SUCCEEDED(ppf->Load(wsz, STGM_READ)))
				{
					int		nIndex;
					CString	sIconFile;
					HICON	hIconLarge, hIconSmall;

					// Get icon path
					psl->GetIconLocation(sIconFile.GetBuffer(MAX_PATH), MAX_PATH, &nIndex);
					sIconFile.ReleaseBuffer();

					// Get icon / "standard" icon?
					BOOL bStdIcon = (sIconFile.Right(7).CompareNoCase("url.dll") == 0) && (nIndex == 0);

					if ((m_nStdIconNotSel == -1) || !bStdIcon)
					{
						ExtractIconEx(sIconFile, nIndex, &hIconLarge, &hIconSmall, 1);

						if (hIconSmall == NULL)
						{
							hIconSmall = hIconLarge;
							hIconLarge = NULL;
						}

						if (hIconSmall != NULL)
						{
							if (bStdIcon)
							{
								m_nStdIconNotSel = m_imgStd.Add(hIconSmall);
								nIconNotSel = nIcon != -1 ? nIcon : m_nStdIconNotSel;
							}
							else
								nIconNotSel = nIcon != -1 ? nIcon : m_imgStd.Add(hIconSmall);

							nIconSel = nIcon2 != -1 ? nIcon2 : nIconNotSel;
							DestroyIcon(hIconSmall);
						}
						else
						{
							nIconSel = m_nStdIconNotSel;
							nIconNotSel = m_nStdIconNotSel;
						}

						if (hIconLarge != NULL)
							DestroyIcon(hIconLarge);
					}
					else	// We already have the standard icon
					{
						nIconNotSel = m_nStdIconNotSel;
						nIconSel = m_nStdIconNotSel;
					}
				}
			}
		}
	}

	HTREEITEM hItem;
	CString sDrive, sDir, sName, sExt;

	CStringEx::SplitPath(sPath, sDrive, sDir, sName, sExt, bIsDir);

	if (bIsDir)
	{
		// Add folder to tree
		hItem = InsertItem(TVIF_TEXT | TVIF_STATE | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_IMAGE | TVIF_HANDLE,
						   sName,
						   nIconNotSel,
						   nIconSel,
						   0,
						   0,
						   ITEM_IS_FOLDER,
						   hParent,
						   TVI_LAST);
		// Mark item as folder
		SetItemData(hItem, ITEM_IS_FOLDER);
	}
	else
	{
		CString sItemName = MakeFullItemName(hParent, FALSE);
		int nState;

		sItemName += "\\";
		sItemName += sName + sExt;

		// Get item status if stored in status map
		sItemName.MakeUpper();
		if (!m_StatusMap.Lookup(sItemName, nState))
			nState = INDEXTOSTATEIMAGEMASK(STATE_INDEX_UNCHECKED);

		// Add item to tree
		hItem = InsertItem(TVIF_TEXT | TVIF_STATE | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_IMAGE | TVIF_HANDLE,
						   sName,
						   nIconNotSel,
						   nIconSel,
						   nState,
						   TVIS_STATEIMAGEMASK,
						   0,
						   hParent,
						   TVI_LAST);

		if (hItem)
		{
			CString* psData = new CString(); // leak
			if (m_InfoMap.Lookup(sItemName, *psData))
				SetItemData(hItem, (DWORD) psData);
			else
				delete psData;

			if (m_pProgressCtl && !m_bCancel)
				m_pProgressCtl->OffsetPos(1);
			m_nNumItems++;
		}
	}

	return 	hItem;
}

CString CFavOrgTreeCtl::MakeFullItemName(HTREEITEM hItem, BOOL bIsShortcut)
{
	// Compute the full item path from the name of its
	// parents in the tree
	CString		sFullItemName;
	HTREEITEM	hCurItem;

	hCurItem = hItem;

	while (hCurItem && (hCurItem != CFavOrgTreeCtl::mp_Me->m_hRootItem))
	{
		sFullItemName = CString("\\") + CFavOrgTreeCtl::mp_Me->GetItemText(hCurItem) + sFullItemName;
		hCurItem = CFavOrgTreeCtl::mp_Me->GetParentItem(hCurItem);
	}

	// Now, we are at the root
	if (sFullItemName.Compare(_T("\\")) != 0)
	{
		sFullItemName = m_sFavPath + sFullItemName;
		if (bIsShortcut)
			sFullItemName += g_szUrlExt;

		return sFullItemName;
	}
	else
		return CString(_T(""));
}

UINT CFavOrgTreeCtl::UpdateItemThread(LPVOID lParam)
{
	CFavOrgTreeCtl::mp_Me->m_nActiveThreads++;

	PSCANTHREADPARAMS lpParms = (PSCANTHREADPARAMS) lParam;

	HTREEITEM			hItem = lpParms->hItem;
	BOOL				bDeadLinksOnly = lpParms->bDeadLinksOnly ;
	BOOL				bNoOverwrite = lpParms->bNoOverwrite;

	delete lpParms;

	CInternetSession* pInet = new CInternetSession;
	int nTimeout = m_nTimeout * 1000;

	pInet->SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, nTimeout);
	pInet->SetOption(INTERNET_OPTION_SEND_TIMEOUT, nTimeout);
	pInet->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, nTimeout);

	// Get shortcut filename
	CString	sFilename = MakeFullItemName(hItem, TRUE);

	if (((!m_bConnected) && (!m_bDoNotCheck)) || sFilename.IsEmpty() || m_bCancel)
	{
		CFavOrgTreeCtl::mp_Me->m_nJobsToRun--;
		CFavOrgTreeCtl::mp_Me->m_nActiveThreads--;
		ReleaseSemaphore(CFavOrgTreeCtl::m_hScanSem, 1, NULL);
		pInet->Close();
		delete pInet;

		return 0;
	}

	CoInitialize(NULL);

	// Inform user
	DisplayMsg(IDS_MSG_FAVNAME, CFavOrgTreeCtl::mp_Me->GetItemText(hItem));

	// Get URL
	CString sURL;

	if (!GetOrSetURLFromShortcut(sFilename, sURL, TRUE))
	{
		CoUninitialize();
		CFavOrgTreeCtl::mp_Me->m_nJobsToRun--;
		CFavOrgTreeCtl::mp_Me->m_nActiveThreads--;
		ReleaseSemaphore(CFavOrgTreeCtl::m_hScanSem, 1, NULL);
		pInet->Close();
		delete pInet;

		return 0;
	}

	// Check URL
	DWORD				dwServiceType;
	CString				sServer;
	CString				sObject;
	INTERNET_PORT		nPort;
	CHttpConnection*	pHttpConnect = NULL;
	URL_COMPONENTS		UrlComponents;
	int					nUrlLen;

	// This code detects whether the URL contains extra information
	// e.g. an anchor specification. If this information exists, it
	// is removed before checking the URL because SendRequest may
	// fail in that case.
	nUrlLen = sURL.GetLength();

	if (sURL.Left(11).CompareNoCase(_T("javascript:")) == 0)
		goto exitandprogress;

	memset(&UrlComponents, 0, sizeof(URL_COMPONENTS));
	UrlComponents.dwStructSize = sizeof(URL_COMPONENTS);
    // The following lines set which components will be displayed.
    UrlComponents.dwExtraInfoLength = 1;

	if (!InternetCrackUrl(sURL.GetBuffer(nUrlLen + 1), nUrlLen, 0, &UrlComponents))
		goto exitandprogress;

	if ((UrlComponents.dwExtraInfoLength != 0) && (*(UrlComponents.lpszExtraInfo) == '#'))
		// Eliminate extra info for checking
		*(UrlComponents.lpszExtraInfo) = '\0';

	sURL.ReleaseBuffer();

	if (!AfxParseURL(sURL, dwServiceType, sServer, sObject, nPort))
		goto exitandprogress;

	// Check URL and try to find favicon if any
	//DisplayMsg(IDS_MSG_CHECKURL, sURL + " (" + CFavOrgTreeCtl::mp_Me->GetItemText(hItem) + ")");
	CFavOrgTreeCtl::mp_Me->SetItemState(hItem, INDEXTOSTATEIMAGEMASK(STATE_INDEX_INPROCESS), TVIS_STATEIMAGEMASK);
	
	if (CheckURL(pInet, &pHttpConnect, dwServiceType, sURL, sServer, sObject, nPort, hItem)
		&& ((dwServiceType == AFX_INET_SERVICE_HTTP) || (dwServiceType == AFX_INET_SERVICE_HTTPS))
		&& !bDeadLinksOnly
		&& (pHttpConnect != NULL))
	{
		if (bNoOverwrite)
		{
			EnterCriticalSection(&CFavOrgTreeCtl::mp_Me->m_csUrlLink);
			// If item already has an icon, do nothing
			IUniformResourceLocatorPtr pUrlLink(CLSID_InternetShortcut, NULL, CLSCTX_INPROC_SERVER);
			if (pUrlLink != NULL)
			{
				IShellLinkPtr psl(pUrlLink);
				IPersistFilePtr ppf(pUrlLink);

				if (ppf != NULL)
				{
					WCHAR wsz[MAX_PATH];
					int nIndex;

					MultiByteToWideChar(CP_ACP, 0, sFilename, -1, wsz, MAX_PATH);
					// Load the shortcut.
					if (SUCCEEDED(ppf->Load(wsz, STGM_READ)))
					{
						CString	sIconFile;

						// Get icon path
						psl->GetIconLocation(sIconFile.GetBuffer(MAX_PATH), MAX_PATH, &nIndex);
						sIconFile.ReleaseBuffer();

						// Get icon / "standard" icon?
						BOOL bStdIcon = (sIconFile.Right(7).CompareNoCase("url.dll") == 0) && (nIndex == 0);
						if (!bStdIcon)
						// We already have an icon assigned
						{
							LeaveCriticalSection(&CFavOrgTreeCtl::mp_Me->m_csUrlLink);
							goto exitandprogress;
						}
					}
				}
			}

			LeaveCriticalSection(&CFavOrgTreeCtl::mp_Me->m_csUrlLink);
		}

		// The URL has been successfully checked
		// Retrieve file from web site and store it to icon folder
		CHttpFile* pIconFile = NULL;

		try
		{
			pIconFile = pHttpConnect->OpenRequest(CHttpConnection::HTTP_VERB_GET,
												  "/favicon.ico",
												  NULL,
												  1,
												  NULL,
												  NULL,
												  INTERNET_FLAG_DONT_CACHE
												  | INTERNET_FLAG_EXISTING_CONNECT
												  | (dwServiceType == AFX_INET_SERVICE_HTTPS ? INTERNET_FLAG_SECURE : 0));

			if (pIconFile)
			{
				pIconFile->SetReadBufferSize(2048);
				pIconFile->SendRequest();
			}
			else
				goto exitandprogress;
		}

		catch(CInternetException* pEx)
		{
			pEx->Delete();
			if (pIconFile)
			{
				pIconFile->Close();
				delete pIconFile;
			}
			goto exitandprogress;
		}

		if (!pIconFile)
			goto exitandprogress;

		CString	sLocalIconName = m_sIconStore + sServer + ".ico";

		// Check whether file is more recent than the one we have on disk
		if (bNoOverwrite)
		{
			CFile		fIcon;
			SYSTEMTIME	date;
			FILETIME	currentfiletime;
			FILETIME	actualfiletime;
			BOOL		bOpened;

			if ((bOpened = fIcon.Open(sLocalIconName, CFile::modeRead))
				&&
				pIconFile->QueryInfo(HTTP_QUERY_FLAG_SYSTEMTIME | HTTP_QUERY_LAST_MODIFIED, &date, NULL))
			{
				GetFileTime((HANDLE) fIcon.m_hFile, NULL, NULL, &currentfiletime);
				fIcon.Close();
				SystemTimeToFileTime(&date, &actualfiletime);
				if (CompareFileTime(&actualfiletime, &currentfiletime) <= 0)
				{
					// We don't need to update the icon
					pIconFile->Close();
					delete pIconFile;
					goto exitandprogress;
				}
			}
		}

		// Get length of icon file
		CString sLen;

		pIconFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, sLen);

		DWORD dwLen = atoi(sLen);
		BYTE* pBuffer;

		if ((dwLen <= 0) || (dwLen > 65536)) // Take care of odd values returned from server
			pBuffer = NULL;
		else
			pBuffer = new BYTE[dwLen];

		if (pBuffer == NULL)
		{
			pIconFile->Close();
			delete pIconFile;
			goto exitandprogress;
		}

		try
		{
			// Get icon data
			pIconFile->Read(pBuffer, dwLen);
			pIconFile->Close();
			delete pIconFile;
			pIconFile = NULL;

			// Reject file if not true icon or bitmap
			BOOL bFileOK = IsIconOrBmp(pBuffer, dwLen);

			if (bFileOK)
			{
				CFile fIcon(sLocalIconName, CFile::modeCreate | CFile::modeWrite);
				fIcon.Write(pBuffer, dwLen);
				fIcon.Close();
			}

			delete pBuffer;
			pBuffer = NULL;

			if (!bFileOK)
				goto exitandprogress;
		}

		catch (CInternetException* pEx)
		{
			pEx->Delete();

			if (pBuffer)
				delete pBuffer;
			if (pIconFile)
			{
				pIconFile->Close();
				delete pIconFile;
				pIconFile = NULL;
			}
			goto exitandprogress;
		}

		catch (CFileException* pEx)
		{
			pEx->Delete();

			if (pBuffer)
				delete pBuffer;
			if (pIconFile)
			{
				pIconFile->Close();
				delete pIconFile;
			}
			goto exitandprogress;
		}

		// Verify that we have an icon
		HICON hIconLarge = NULL;
		HICON hIconSmall = NULL;

		ExtractIconEx(sLocalIconName, 0, &hIconLarge, &hIconSmall, 1);

		if (hIconSmall == NULL)
		{
			hIconSmall = hIconLarge;
			hIconLarge = NULL;
		}

		if (hIconSmall != NULL)
		{
			//DisplayMsg(IDS_MSG_ICONFOUND, CString(""));

			// Modify shortcut accordingly
			EnterCriticalSection(&CFavOrgTreeCtl::mp_Me->m_csUrlLink);
			IUniformResourceLocatorPtr pNewUrlLink(CLSID_InternetShortcut, NULL, CLSCTX_INPROC_SERVER);
			IUniformResourceLocatorPtr pUrlLink(CLSID_InternetShortcut, NULL, CLSCTX_INPROC_SERVER);
			if ((pUrlLink != NULL) && (pNewUrlLink != NULL))
			{
				IPersistFilePtr pNewpf(pNewUrlLink);
				IPersistFilePtr ppf(pUrlLink);

				if ((ppf != NULL) && (pNewpf != NULL))
				{
					WCHAR wsz[MAX_PATH];

					MultiByteToWideChar(CP_ACP, 0, sFilename, -1, wsz, MAX_PATH);
					// Load the shortcut.
					if (SUCCEEDED(ppf->Load(wsz, STGM_READ)))
					{
						IShellLinkPtr psl(pNewUrlLink);

						if (psl != NULL)
						{
							LPSTR lpszCurrentURL;

							pUrlLink->GetURL(&lpszCurrentURL);
							pNewUrlLink->SetURL(lpszCurrentURL, IURL_SETURL_FL_GUESS_PROTOCOL );
							CFavOrgTreeCtl::mp_Me->SetItemIcon(hItem, sLocalIconName, hIconSmall, 0, psl);
							DeleteFile(sFilename);
							pNewpf->Save(wsz, TRUE);
						}
					}
				}
			}

			LeaveCriticalSection(&CFavOrgTreeCtl::mp_Me->m_csUrlLink);

			DestroyIcon(hIconSmall);
		}
		else
			// Clean up file
			DeleteFile(sLocalIconName);

		if (hIconLarge != NULL)
			DestroyIcon(hIconLarge);
	}

exitandprogress:

	if (pHttpConnect)
	{
		pHttpConnect->Close();
		delete pHttpConnect;
	}

	if (AfxIsValidAddress(pInet, sizeof(CInternetSession*)))
	{
		pInet->Close();
		delete pInet;
	}

	CoUninitialize();
	CFavOrgTreeCtl::mp_Me->m_nJobsToRun--;
	CFavOrgTreeCtl::mp_Me->m_nActiveThreads--;
	ReleaseSemaphore(CFavOrgTreeCtl::m_hScanSem, 1, NULL);

	if (m_pProgressCtl && !m_bCancel)
		m_pProgressCtl->OffsetPos(1);

	return 0;
}

void CFavOrgTreeCtl::UpdateItem(HTREEITEM hItem, BOOL bDeadLinksOnly, BOOL bNoOverwrite)
{
	if (m_bCancel)
		return;

	PSCANTHREADPARAMS lpParms = new SCANTHREADPARAMS;

	lpParms->bDeadLinksOnly = bDeadLinksOnly;
	lpParms->bNoOverwrite = bNoOverwrite;
	lpParms->hItem = hItem;

	CWinThread* pScanThread = AfxBeginThread(UpdateItemThread, (LPVOID) lpParms, THREAD_PRIORITY_NORMAL, 0, 0);
	pScanThread->m_bAutoDelete = FALSE;
}

void CFavOrgTreeCtl::ProcessItem(HTREEITEM hItem, BOOL bDeadLinksOnly, BOOL bNoOverwrite)
{
	if (m_bCancel)
		return;

	if (GetItemData(hItem) == ITEM_IS_FOLDER)
	{
		CString			sFolderName = MakeFullItemName(hItem, FALSE);
		WIN32_FIND_DATA FindData;
 
		memset(&FindData, 0, sizeof FindData);
		
		HANDLE hFind = FindFirstFile(sFolderName, &FindData);

		FindClose(hFind);
		
		if (hFind != INVALID_HANDLE_VALUE && (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			// It's a folder (we have a selection of folders)
			// Process all items in this folder
			HTREEITEM hChildItem = GetChildItem(hItem);

			while (!m_bCancel && (hChildItem != NULL))
			{
				if (GetItemData(hChildItem) == ITEM_IS_FOLDER)
					ProcessItem(hChildItem, bDeadLinksOnly, bNoOverwrite);
				else
				{
					WaitForSingleObject(CFavOrgTreeCtl::m_hScanSem, MAX_SCAN_THREAD_WAIT);
					if (!m_bCancel)
						UpdateItem(hChildItem, bDeadLinksOnly, bNoOverwrite);
				}
				hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
			}
		}
	}
	else
	{
		WaitForSingleObject(CFavOrgTreeCtl::m_hScanSem, MAX_SCAN_THREAD_WAIT);
		if (!m_bCancel)
			UpdateItem(hItem, bDeadLinksOnly, bNoOverwrite);
	}
}

void CFavOrgTreeCtl::ProcessItems(BOOL bDeadLinksOnly, BOOL bNoOverwrite)
{
	// Connect if necessary
	if (!GetConnectState())
		return;

	if (m_bCancel)
		return;

	HTREEITEM	hCurItem;
	int			nJobsToRun;

	m_bCancel = FALSE;
	m_bProcessing = TRUE;
	m_bLetUserScroll = FALSE;
	m_nActiveThreads = 0;

	if (m_pProgressCtl)
	{
		m_pProgressCtl->ShowWindow(SW_NORMAL);
		m_pProgressCtl->SetStep(1);
		m_pProgressCtl->SetPos(0);
	}

	if (!m_SelItemList.IsEmpty())
	{
		nJobsToRun = CountSelected();
		m_nJobsToRun = nJobsToRun;
		// We have a selection
		if (m_pProgressCtl)
			m_pProgressCtl->SetRange32(0, nJobsToRun);

		POSITION pos = m_SelItemList.GetHeadPosition();

		// Enumerate selection
		while (!m_bCancel && (pos != NULL))
		{
			hCurItem = m_SelItemList.GetNext(pos);
			ProcessItem(hCurItem, bDeadLinksOnly, bNoOverwrite);
		}
	}
	else
	{
		// Enumerate tree
		if (m_pProgressCtl)
			m_pProgressCtl->SetRange32(0, m_nLoadedItems);

		m_nJobsToRun = m_nLoadedItems;
		hCurItem = GetChildItem(m_hRootItem);
		while (!m_bCancel && (hCurItem != NULL))
		{
			ProcessItem(hCurItem, bDeadLinksOnly, bNoOverwrite);
			hCurItem = GetNextItem(hCurItem, TVGN_NEXT);
		}
	}

	if (m_bCancel)
		m_nJobsToRun = 0;
}

void CFavOrgTreeCtl::CheckAndFindIcons(BOOL bDeadLinksOnly, BOOL bNoOverwrite)
{
#ifdef _DEBUG
	m_hScanSem = CreateSemaphore(NULL, MAX_SCAN_THREADS, MAX_SCAN_THREADS, _T("Favorg_Sem_Test"));
#else
	m_hScanSem = CreateSemaphore(NULL, MAX_SCAN_THREADS, MAX_SCAN_THREADS, NULL);
#endif

	//Start worker thread
	CHECKFLAGS chkFlags;

	chkFlags.bDeadLinksOnly = bDeadLinksOnly;
	chkFlags.bNoOverwrite = bNoOverwrite;
	m_nJobsToRun = 1;	// Just to start processing

	ClearMsg();

	CWinThread* pWorkThread = AfxBeginThread(DoProcessItems, (LPVOID) &chkFlags, THREAD_PRIORITY_NORMAL, 0, 0);
	pWorkThread->m_bAutoDelete = FALSE;

	BOOL	bQuitLoop = FALSE;

	////////////////////
	m_bProcessing = TRUE;
	m_bCancel = FALSE;

	ModifyStates(NULL, STATE_INDEX_UNCHECKED);
	///////////////////////////////////////////

	while (!bQuitLoop || (m_nJobsToRun > 0) || (m_nActiveThreads > 0))
	{
		// Wait for thread to terminate or
		// for a message in input queue
		if (MsgWaitForMultipleObjects(1,
									  &(pWorkThread->m_hThread),
									  FALSE,
									  INFINITE,
									  QS_ALLINPUT) == WAIT_OBJECT_0)
			bQuitLoop = TRUE;
		else
			PeekAndYield();

		if ((m_nJobsToRun == 0) && (m_nActiveThreads == 0))
			bQuitLoop = TRUE;
	}

	delete pWorkThread;

	// Notify parent so that the user interface can be re-enabled
	GetParent()->PostMessage(WM_CAN_UNLOCK, 0, 0);

	if (m_pProgressCtl)
	{
		m_pProgressCtl->SetPos(0);
		m_pProgressCtl->ShowWindow(SW_HIDE);
	}
	ClearMsg();

	SetCursor(LoadCursor(NULL, IDC_ARROW));
	m_bProcessing = FALSE;
	m_bCancel = FALSE;

	Invalidate();
	GetParent()->UpdateWindow();

	CloseHandle(m_hScanSem);
	m_hScanSem = NULL;
	ClearMsg();
}

void CFavOrgTreeCtl::ProcessDeadLink(HTREEITEM hItem)
{
	int nState = GetItemState(hItem, TVIS_STATEIMAGEMASK) & ~0xFF;

	if (nState == INDEXTOSTATEIMAGEMASK(STATE_INDEX_PAGENONE))
	{
		// Reset shortcut to root URL
		CString sName = MakeFullItemName(hItem, TRUE);
		CString sURL;

		if (GetOrSetURLFromShortcut(sName, sURL, TRUE))
		{
			DWORD			dwServiceType;
			CString			sServer;
			CString			sObject;
			INTERNET_PORT	nPort;

			// Parse URL
			if (!AfxParseURL(sURL, dwServiceType, sServer, sObject, nPort))
				goto exitandprogress;

			CStringEx sNewURL;

			// Compute favicon filename
			if (dwServiceType = AFX_INET_SERVICE_HTTP)
				sNewURL = "http://";
			else if (dwServiceType = AFX_INET_SERVICE_HTTPS)
				sNewURL = "https://";
			else
				goto exitandprogress;

			sNewURL += sServer;

			if (nPort != 80)
			{
				CString sPort;

				sPort.Format("%d", nPort);
				sPort = CString(":") + sPort;
				sNewURL += sPort;
			}

			DisplayMsg(IDS_MSG_UPDATINGDEADLINK, sNewURL);

			// Set new URL
			GetOrSetURLFromShortcut(sName, sNewURL, FALSE);
			if (!m_bLetUserScroll)
				EnsureVisible(hItem);
			// Check URL again
			ProcessItem(hItem, TRUE, TRUE);
			// Change state accordingly
			SetItemState(hItem, INDEXTOSTATEIMAGEMASK(STATE_INDEX_RESET), TVIS_STATEIMAGEMASK);
		}
	}

	PeekAndYield();

exitandprogress:
	if (m_pProgressCtl && !m_bCancel)
		m_pProgressCtl->OffsetPos(1);
}

void CFavOrgTreeCtl::ProcessDeadLinks(HTREEITEM hItem)
{
	if (GetItemData(hItem) == ITEM_IS_FOLDER)
	{
		// It's a folder
		// Process all items in this folder
		HTREEITEM hChildItem = GetChildItem(hItem);

		while (!m_bCancel && (hChildItem != NULL))
		{
			if (GetItemData(hChildItem) == ITEM_IS_FOLDER)
				ProcessDeadLinks(hChildItem);
			else
				ProcessDeadLink(hChildItem);
			hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
		}

		if (m_bCancel)
			GetParent()->PostMessage(WM_CAN_UNLOCK, 0, 0);
	}
	else
		ProcessDeadLink(hItem);
}

void CFavOrgTreeCtl::ResetDeadLinks()
{
	m_bProcessing = TRUE;
	m_bLetUserScroll = FALSE;

	if (m_pProgressCtl)
	{
		m_pProgressCtl->ShowWindow(SW_NORMAL);
		m_pProgressCtl->SetRange32(0, m_nLoadedItems);
		m_pProgressCtl->SetStep(1);
		m_pProgressCtl->SetPos(0);
	}

	m_bCancel = FALSE;

	ClearSelection(TRUE);
	// Find all items marked as dead links
	ProcessDeadLinks(m_hRootItem);

	m_bCancel = FALSE;
	if (m_pProgressCtl)
	{
		m_pProgressCtl->SetPos(0);
		m_pProgressCtl->ShowWindow(SW_HIDE);
	}
	ClearMsg();

	SetCursor(LoadCursor(NULL, IDC_ARROW));
	m_bProcessing = FALSE;
}

void CFavOrgTreeCtl::CancelAction()
{
	m_bCancel = TRUE;
	m_nJobsToRun = 0;
	m_nActiveThreads = 0;
}

void CFavOrgTreeCtl::ClearSelection(BOOL bCleanTree)
{
	if (!m_bProcessing)
		CMultiselTreeCtrl::ClearSelection(bCleanTree);
}

UINT CFavOrgTreeCtl::HttpTimeOutThread(LPVOID lParam)
{
	BOOL	bOK = TRUE;
	UINT	nStateIndex = 0;

	PHTTPTIMEOUTPARAMS lpParams = (PHTTPTIMEOUTPARAMS) lParam;

	CString		sServer = lpParams->sServer;
	CHttpFile*	pFile = lpParams->pFile;

	try
	{
		pFile->SendRequest();
	}

	catch (CInternetException* pEx)
	{
		// Handle main internet exceptions
		DWORD dwError = pEx->m_dwError;
		pEx->Delete();

		switch(dwError)
		{
			case ERROR_INTERNET_INCORRECT_USER_NAME:
			case ERROR_INTERNET_INCORRECT_PASSWORD:
			case ERROR_INTERNET_LOGIN_FAILURE:
			case ERROR_INTERNET_CLIENT_AUTH_CERT_NEEDED:
			case ERROR_INTERNET_CLIENT_AUTH_NOT_SETUP:
				DisplayMsg(IDS_MSG_ACCESSDENIED, CString(""));
				nStateIndex = STATE_INDEX_ACCESSDENIED;
				bOK = FALSE;
				break;

			case ERROR_INTERNET_TIMEOUT:
				DisplayMsg(IDS_MSG_TIMEOUT, CString(""));
				nStateIndex = STATE_INDEX_TIMEOUT;
				bOK = FALSE;
				break;

			case ERROR_INTERNET_EXTENDED_ERROR:
			case ERROR_INTERNET_INVALID_URL:
			case ERROR_INTERNET_NAME_NOT_RESOLVED:
			case ERROR_INTERNET_ITEM_NOT_FOUND:
			case ERROR_INTERNET_CANNOT_CONNECT:
			case ERROR_INTERNET_SERVER_UNREACHABLE:
			case ERROR_INTERNET_PROXY_SERVER_UNREACHABLE:
			case ERROR_INTERNET_DISCONNECTED:
				DisplayMsg(IDS_MSG_NOSITE, sServer);
				nStateIndex = STATE_INDEX_SITENONE;
				bOK = FALSE;
				break;

			default:
				DisplayMsg(IDS_MSG_UNKNOWN, sServer);
				nStateIndex = STATE_INDEX_QUESTION;
				bOK = FALSE;
				break;
		}
	}

	return bOK? 0 : nStateIndex;
}

BOOL CFavOrgTreeCtl::CheckURL(CInternetSession* pInet, CHttpConnection** ppHttpConnect, DWORD dwServiceType, CStringEx sUrl, CStringEx sServer, CStringEx sObject, int nPort, HTREEITEM hItem)
{
	CHttpConnection*	pHttpConnect = NULL;
	CFtpConnection*		pFtpConnect = NULL;
	DWORD				dwCurData;
	CHttpFile*			pFile = NULL;
	DWORD				dwStatusCode;
	int					nStateIndex;
	BOOL				bOK = TRUE;
	CString*			psItemInfo;

	int nTimeout = m_nTimeout * 1000;

	if (dwServiceType == AFX_INET_SERVICE_FILE)
	// URL is something like file:///c:/xxxxx/xxxx.xxx
	{
		HANDLE	hFile;

		sObject = sObject.URLDecode();

		if ((hFile = CreateFile(sObject, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL)) == INVALID_HANDLE_VALUE)
		{
			DisplayMsg(IDS_MSG_NOTFOUND, sObject);

			nStateIndex = STATE_INDEX_PAGENONE;
			bOK = FALSE;
		}
		else
		{
			CloseHandle(hFile);
			//DisplayMsg(IDS_MSG_OK, sUrl + " (" + CFavOrgTreeCtl::mp_Me->GetItemText(hItem) + ")");
			nStateIndex = STATE_INDEX_CHECKED;
		}

		goto AssignImage;
	}

	try
	{
		if ((dwServiceType == AFX_INET_SERVICE_HTTP) || (dwServiceType == AFX_INET_SERVICE_HTTPS))
		{
			// It's an HTTP URL - try to connect
			pHttpConnect = pInet->GetHttpConnection(sServer, (INTERNET_PORT) (nPort != 80 ? nPort : INTERNET_INVALID_PORT_NUMBER));
			if (pHttpConnect)
			{
				// OK - Now try to GET object
				// We should actually use the HEAD request to reduce traffic but :
				// 1. Many servers DO NOT support the HEAD request
				// 2. Using GET doesn't cause too much data to be sent over the network
				//    since we are only requesting the header.
				pFile = pHttpConnect->OpenRequest(CHttpConnection::HTTP_VERB_GET,
												  sObject,
												  NULL,
												  1,
												  NULL,
												  NULL,
												  INTERNET_FLAG_DONT_CACHE
												  | INTERNET_FLAG_NO_AUTO_REDIRECT
												  | INTERNET_FLAG_KEEP_CONNECTION
												  | (dwServiceType == AFX_INET_SERVICE_HTTPS ? INTERNET_FLAG_SECURE : 0));

				// We do not allow auto-redirection in order to be notified of redirected pages
				if (pFile)
				{
					pFile->SetReadBufferSize(512);
resend:
					PHTTPTIMEOUTPARAMS lpParams = new HTTPTIMEOUTPARAMS;

					lpParams->pFile = pFile;
					lpParams->sServer = sServer;

					// With certain versions of WININET DLLs blocking calls may never return.
					// So we make the call to SendRequest in a separate thread.
					CWinThread* pHttpTimeOutThread = AfxBeginThread(HttpTimeOutThread, (LPVOID) lpParams, THREAD_PRIORITY_NORMAL, 0, 0);
					pHttpTimeOutThread->m_bAutoDelete = FALSE;

					// We wait no longer than the user specified timeout.
					if (WaitForSingleObject(pHttpTimeOutThread->m_hThread, m_nTimeout * 1000) == WAIT_TIMEOUT)
					{
						// We explicitly close the connection. This should cause the thread
						// to terminate.
						pInet->Close();

						WaitForSingleObject(pHttpTimeOutThread->m_hThread, INFINITE);
						nStateIndex = STATE_INDEX_TIMEOUT;
						bOK = FALSE;
						delete pHttpTimeOutThread;
						goto AssignImage;
					}

					DWORD dwExitCode;

					GetExitCodeThread(pHttpTimeOutThread->m_hThread, &dwExitCode);
					delete pHttpTimeOutThread;

					// 0 = no exception
					if (dwExitCode != 0)
					{
						nStateIndex = dwExitCode;
						goto AssignImage;
					}

					pFile->QueryInfoStatusCode(dwStatusCode);
				}
			}
			else
				return FALSE;
		}
		else if (dwServiceType == AFX_INET_SERVICE_FTP)
		{
			// It's an FTP URL - try to connect
			try
			{
				if (pFtpConnect = pInet->GetFtpConnection(sServer))
				{
					// OK - We don't try to download the file
					//DisplayMsg(IDS_MSG_OK, sUrl + " (" + CFavOrgTreeCtl::mp_Me->GetItemText(hItem) + ")");
					nStateIndex = STATE_INDEX_CHECKED;
					pFtpConnect->Close();
					delete pFtpConnect;
					pFtpConnect = NULL;
					goto AssignImage;
				}
				else
				{
					// No connection - bad link
					DisplayMsg(IDS_MSG_NOSITE, sServer);
					nStateIndex = STATE_INDEX_SITENONE;
					bOK = FALSE;
					goto AssignImage;
				}

			}

			catch(CInternetException* pEx)
			{
				DWORD dwError = pEx->m_dwError;
				pEx->Delete();
				if (pFtpConnect)
				{
					pFtpConnect->Close();
					delete pFtpConnect;
					pFtpConnect = NULL;
				}
				DisplayMsg(IDS_MSG_NOSITE, sServer);
				nStateIndex = STATE_INDEX_SITENONE;
				bOK = FALSE;
				goto AssignImage;
			}
		}
		else
		{
			DisplayMsg(IDS_MSG_UNKNOWN, sServer);
			nStateIndex = STATE_INDEX_QUESTION;
			bOK = FALSE;
			goto AssignImage;
		}

	}

	catch(CInternetException* pEx)
	{
		// Handle main internet exceptions
		DWORD dwError = pEx->m_dwError;
		pEx->Delete();

		switch(dwError)
		{
			case ERROR_INTERNET_INCORRECT_USER_NAME:
			case ERROR_INTERNET_INCORRECT_PASSWORD:
			case ERROR_INTERNET_LOGIN_FAILURE:
			case ERROR_INTERNET_CLIENT_AUTH_CERT_NEEDED:
			case ERROR_INTERNET_CLIENT_AUTH_NOT_SETUP:
				DisplayMsg(IDS_MSG_ACCESSDENIED, CString(""));
				nStateIndex = STATE_INDEX_ACCESSDENIED;
				bOK = FALSE;
				break;

			case ERROR_INTERNET_TIMEOUT:
				DisplayMsg(IDS_MSG_TIMEOUT, CString(""));
				nStateIndex = STATE_INDEX_TIMEOUT;
				bOK = FALSE;
				break;

			case ERROR_INTERNET_EXTENDED_ERROR:
			case ERROR_INTERNET_INVALID_URL:
			case ERROR_INTERNET_NAME_NOT_RESOLVED:
			case ERROR_INTERNET_ITEM_NOT_FOUND:
			case ERROR_INTERNET_CANNOT_CONNECT:
			case ERROR_INTERNET_SERVER_UNREACHABLE:
			case ERROR_INTERNET_PROXY_SERVER_UNREACHABLE:
			case ERROR_INTERNET_DISCONNECTED:
				DisplayMsg(IDS_MSG_NOSITE, sServer);
				nStateIndex = STATE_INDEX_SITENONE;
				bOK = FALSE;
				break;

			default:
				DisplayMsg(IDS_MSG_UNKNOWN, sServer);
				nStateIndex = STATE_INDEX_QUESTION;
				bOK = FALSE;
				break;
		}

		goto AssignImage;
	}

	psItemInfo = new CString;

	if (!psItemInfo)
	{
		DisplayMsg(IDS_MSG_UNKNOWN, sServer);
		nStateIndex = STATE_INDEX_QUESTION;
		bOK = FALSE;
		goto AssignImage;
	}

	// Get response header and status code
	pFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, *psItemInfo);

	// Replace item info
	dwCurData = CFavOrgTreeCtl::mp_Me->GetItemData(hItem);
	if ((dwCurData != ITEM_IS_FOLDER) && (dwCurData != 0))
		delete (CString*) dwCurData;
	CFavOrgTreeCtl::mp_Me->SetItemData(hItem, (DWORD) psItemInfo);

	switch(dwStatusCode)
	{
		case HTTP_STATUS_OK:
			//DisplayMsg(IDS_MSG_OK, sUrl + " (" + CFavOrgTreeCtl::mp_Me->GetItemText(hItem) + ")");
			nStateIndex = STATE_INDEX_CHECKED;
			break;

		case HTTP_STATUS_MOVED:
		case HTTP_STATUS_REDIRECT:
			if (psItemInfo && m_bCanRedirect && (dwStatusCode != HTTP_STATUS_REDIRECT))
			// If allowed by the user, try to reset the shortcut to the new location.
			// We do not update the shortcut in case of a temporary redirection.
			{
				CStringEx sNewLocation = *psItemInfo;

				int nPos = sNewLocation.Find(g_szLocation);
				if (nPos == -1)
					goto pagemoved;
				sNewLocation = sNewLocation.Mid(nPos + strlen(g_szLocation));
				nPos = sNewLocation.Find(('\r'));
				if (nPos > 0)
				{
					sNewLocation = sNewLocation.Left(nPos);
					sNewLocation.Trim();

					if (sNewLocation.Left(4).CompareNoCase("http") != 0)
					{
						// We have to reconstruct the URL
						CString sURL;

						sURL = "http://" + sServer + sNewLocation;
						if (nPort != 80)
						{
							CString sPort;

							sPort.Format("%d", nPort);
							sPort = CString(":") + sPort;
							sURL += sPort;
						}

						sNewLocation = sURL;
					}

					// Set shortcut to new location
					if (GetOrSetURLFromShortcut(MakeFullItemName(hItem, TRUE), sNewLocation, FALSE))
					{
						//DisplayMsg(IDS_MSG_REDIRWITHUPDATE, CString(""));
						nStateIndex = STATE_INDEX_MOVED_UPDATED;
					}
					else
						goto pagemoved;
				}
				else
					goto pagemoved;
			}
			else
			{
pagemoved:
				//DisplayMsg(IDS_MSG_REDIR, CString(""));
				nStateIndex = STATE_INDEX_MOVED;
			}
			break;

		case HTTP_STATUS_NOT_FOUND:
		case HTTP_STATUS_GONE:
			DisplayMsg(IDS_MSG_NOTFOUND, sObject);
			nStateIndex = STATE_INDEX_PAGENONE;
			break;

		case HTTP_STATUS_PROXY_AUTH_REQ:
			{
				DWORD dwPrompt = pFile->ErrorDlg();

				if (dwPrompt == ERROR_INTERNET_FORCE_RETRY)
				{
					goto resend;
					break;
				}
				else if (dwPrompt == ERROR_SUCCESS)
				{
					nStateIndex = STATE_INDEX_CHECKED;
					break;
				}
				// else fall through
			}

		case HTTP_STATUS_DENIED:
		case HTTP_STATUS_FORBIDDEN:
			DisplayMsg(IDS_MSG_ACCESSDENIED, CString(""));
			nStateIndex = STATE_INDEX_ACCESSDENIED;
			break;

		case HTTP_STATUS_GATEWAY_TIMEOUT:
		case HTTP_STATUS_REQUEST_TIMEOUT:
			DisplayMsg(IDS_MSG_TIMEOUT, CString(""));
			nStateIndex = STATE_INDEX_TIMEOUT;
			break;

		case HTTP_STATUS_BAD_METHOD:
		case HTTP_STATUS_NOT_SUPPORTED:
		default:
			DisplayMsg(IDS_MSG_UNKNOWN, sServer);
			nStateIndex = STATE_INDEX_QUESTION;
			break;
	}

AssignImage:

	if (pFile)
	{
		pFile->Close();
		delete pFile;
	}

	// Pass back http connection to caller so that it can
	// retrieve the favicon from the same connection
	if (bOK)
		*ppHttpConnect = pHttpConnect;
	else if (pHttpConnect != NULL)
	{
		pHttpConnect->Close();
		delete pHttpConnect;
		*ppHttpConnect = NULL;
	}

	if (pFtpConnect)
	{
		pFtpConnect->Close();
		delete pFtpConnect;
	}

	// Display new status icon
	CFavOrgTreeCtl::mp_Me->SetItemState(hItem, INDEXTOSTATEIMAGEMASK(nStateIndex), TVIS_STATEIMAGEMASK);

	return bOK;
}

void CFavOrgTreeCtl::SetItemIcon(HTREEITEM hItem, CString sIconName, HICON hIcon, int nIndex, IShellLinkPtr psl)
{
	// Set icon location in shortcut
	if ((psl != NULL) && SUCCEEDED(psl->SetIconLocation(sIconName, nIndex)))
	{
		int nImage, nSelectedImage;

		GetItemImage(hItem, nImage, nSelectedImage);
		if (nImage != m_nStdIconNotSel)
		// Item already has a custom icon
		{
			m_imgStd.Replace(nImage, hIcon);
			if (nImage != nSelectedImage)
				m_imgStd.Replace(nSelectedImage, hIcon);
		}
		else
		// It's a new icon - do not replace the common standard icon
		{
			int nNewIndex = m_imgStd.Add(hIcon);
			SetItemImage(hItem, nNewIndex, nNewIndex);
		}
	}
}

BOOL CFavOrgTreeCtl::GetOrSetURLFromShortcut(CString sFilename, CString& sURL, BOOL bGet)
{
	EnterCriticalSection(&(CFavOrgTreeCtl::mp_Me->m_csUrlLink));

	// Get URL from shortcut
	HRESULT		hr;
	IMalloc*	pMalloc;

	if (bGet)
	{
		hr = SHGetMalloc(&pMalloc);
		if (!SUCCEEDED(hr))
		{
			LeaveCriticalSection(&(CFavOrgTreeCtl::mp_Me->m_csUrlLink));
			return FALSE;
		}
	}

	IUniformResourceLocatorPtr pUrlLink(CLSID_InternetShortcut, NULL, CLSCTX_INPROC_SERVER);
	if (pUrlLink != NULL)
	{
		// Get a pointer to the IPersistFile interface.
		IPersistFilePtr ppf(pUrlLink);

		if (ppf != NULL)
		{
			WCHAR wsz[MAX_PATH];
			MultiByteToWideChar(CP_ACP, 0, sFilename, -1, wsz, MAX_PATH);
			// Load the shortcut.
			if (SUCCEEDED(ppf->Load(wsz, STGM_READ)))
			{
				if (bGet)
				{
					LPTSTR  lpszURL = NULL;
					// Get URL
					hr = pUrlLink->GetURL(&lpszURL);
					if (SUCCEEDED(hr) && lpszURL)
					{
						sURL = lpszURL;
						pMalloc->Free(lpszURL);
					}
					else
					{
						pMalloc->Release();
						LeaveCriticalSection(&(CFavOrgTreeCtl::mp_Me->m_csUrlLink));
						return FALSE;
					}
				}
				else
				{
					// Set URL
					BOOL bOK = SUCCEEDED(pUrlLink->SetURL(sURL, 0));
					bOK = bOK && SUCCEEDED(ppf->Save(NULL, FALSE));

					LeaveCriticalSection(&(CFavOrgTreeCtl::mp_Me->m_csUrlLink));
					return bOK;
				}
			}
		}
	}
	else if (bGet)
	{
		pMalloc->Release();
		LeaveCriticalSection(&(CFavOrgTreeCtl::mp_Me->m_csUrlLink));
		return FALSE;
	}

	if (bGet)
		pMalloc->Release();

	LeaveCriticalSection(&(CFavOrgTreeCtl::mp_Me->m_csUrlLink));
	return TRUE;
}

void CFavOrgTreeCtl::OnLButtonDown(UINT nFlags, CPoint point)
{
	UINT flags;

	// Helper to fix tree control bug
	if (m_SelItemList.IsEmpty())
		SelectItem(NULL);

	HitTest(point, &flags);

	if (TVHT_ONITEM & flags)
	{
		if (!m_bProcessing)
			CMultiselTreeCtrl::OnLButtonDown(nFlags, point);
	}
	else
		CMultiselTreeCtrl::OnLButtonDown(nFlags, point);

	// Helper to fix tree control bug
	if (m_SelItemList.IsEmpty())
		SelectItem(NULL);
	// Helper to fix problem with CMultiselTreeCtrl
	else if (m_SelItemList.GetCount() == 1)
	{
		long lResult;

		OnSelChanged(NULL, &lResult);
	}
}

void CFavOrgTreeCtl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!m_bProcessing)
	{
		UINT flags;
		HTREEITEM hItem = HitTest(point, &flags);

		if (hItem == NULL)
		{
			ClearSelection(TRUE);
			return;
		}

		if ((flags & TVHT_ONITEM) && (GetItemData(hItem) != ITEM_IS_FOLDER))
		{
			CString sURL;

			// Get URL and execute
			GetOrSetURLFromShortcut(MakeFullItemName(hItem , TRUE), sURL, TRUE);
			ShellExecute(m_hWnd, "open", sURL, NULL, NULL, SW_SHOW);
		}

		CMultiselTreeCtrl::OnLButtonDblClk(nFlags, point);
	}

	// Helper to fix tree control bug
	if (m_SelItemList.IsEmpty())
		SelectItem(NULL);
}

void CFavOrgTreeCtl::GetItemCommonStatus(HTREEITEM hItem, BOOL& bOK, int& nFirstStatus)
{
	int nStatus;

	if (GetItemData(hItem) == ITEM_IS_FOLDER)
	{
		HTREEITEM hChildItem = GetChildItem(hItem);

		while (bOK && (hChildItem != NULL))
		{
			if (GetItemData(hChildItem) == ITEM_IS_FOLDER)
				GetItemCommonStatus(hChildItem, bOK, nFirstStatus);
			else
			{
				nStatus = GetItemState(hChildItem, TVIS_STATEIMAGEMASK) & ~0xFF;
				if (nFirstStatus == -1)
					nFirstStatus = nStatus;
				else
					bOK = bOK && (nFirstStatus == nStatus);
			}

			hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
		}
	}
	else
	{
		nStatus = GetItemState(hItem, TVIS_STATEIMAGEMASK) & ~0xFF;
		if (nFirstStatus == -1)
			nFirstStatus = nStatus;
		else
			bOK = bOK && (nFirstStatus == nStatus);
	}
}

void CFavOrgTreeCtl::GetFoldersCommonStatus(HTREEITEM hItem, BOOL& bOK, int& nFirstStatus)
{
	HTREEITEM	hCurItem;

	if (hItem == NULL)
	{
		// We want the selection if any
		POSITION pos = m_SelItemList.GetHeadPosition();

		while (bOK && (pos != NULL))
		{
			hCurItem = m_SelItemList.GetNext(pos);
			GetItemCommonStatus(hCurItem, bOK, nFirstStatus);
		}
	}
	else
		GetItemCommonStatus(hItem, bOK, nFirstStatus);
}

int CFavOrgTreeCtl::GetCommonStatus(HTREEITEM hItem)
{
	int		nFirstStatus = -1;
	BOOL	bOK = TRUE;

	GetFoldersCommonStatus(hItem, bOK, nFirstStatus);

	return bOK ? nFirstStatus : -1;
}

int CFavOrgTreeCtl::GetMenuToCheckFromState(int nState)
{
	int nMenuToCheck = -1;

	switch(nState)
	{
		case INDEXTOSTATEIMAGEMASK(STATE_INDEX_QUESTION):
			nMenuToCheck = IDC_MNU_QUESTION;
			break;
		case INDEXTOSTATEIMAGEMASK(STATE_INDEX_CHECKED):
			nMenuToCheck = IDC_MNU_CHECKED;
			break;
		case INDEXTOSTATEIMAGEMASK(STATE_INDEX_PAGENONE):
			nMenuToCheck = IDC_MNU_PAGENONE;
			break;
		case INDEXTOSTATEIMAGEMASK(STATE_INDEX_MOVED):
			nMenuToCheck = IDC_MNU_MOVED;
			break;
		case INDEXTOSTATEIMAGEMASK(STATE_INDEX_TIMEOUT):
			nMenuToCheck = IDC_MNU_TIMEOUT;
			break;
		case INDEXTOSTATEIMAGEMASK(STATE_INDEX_SITENONE):
			nMenuToCheck = IDC_MNU_SITENONE;
			break;
		case INDEXTOSTATEIMAGEMASK(STATE_INDEX_ACCESSDENIED):
			nMenuToCheck = IDC_MNU_ACCESSDENIED;
			break;
		case INDEXTOSTATEIMAGEMASK(STATE_INDEX_MOVED_UPDATED):
			nMenuToCheck = IDC_MNU_MOVED_UPDATED;
			break;
		case INDEXTOSTATEIMAGEMASK(STATE_INDEX_UNCHECKED):
			nMenuToCheck = IDC_MNU_UNCHECKED;
			break;
	}

	return nMenuToCheck;
}

void CFavOrgTreeCtl::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (!m_bProcessing)
	{
		UINT flags;
		HTREEITEM hItem = HitTest(point, &flags);
		CMenu	mnuMain;
		CMenu*	pmnuCtxt;

		ClientToScreen(&point);

		if (hItem != NULL)
		{
			BOOL bIsFolder = GetItemData(hItem) == ITEM_IS_FOLDER;

			// The user right-clicked a shortcut
			mnuMain.LoadMenu(IDR_MAINFRAME);
			pmnuCtxt = mnuMain.GetSubMenu(1);

			int		nMenuToCheck = -1;
			int		nState;
			BOOL	bInSelection;

			bInSelection = ((GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED) != 0)
							 &&
							(m_SelItemList.GetCount() > 1);

			if (!bInSelection && !bIsFolder)
				nState = GetItemState(hItem, TVIS_STATEIMAGEMASK) & ~0xFF;
			else if(!bInSelection)
				nState = GetCommonStatus(hItem);
			else
				nState = GetCommonStatus(NULL);

			if (nState != -1)
				// Get the menu item corresponding to the item(s) state
				nMenuToCheck = GetMenuToCheckFromState(nState);
			if (nMenuToCheck != -1)
					pmnuCtxt->CheckMenuItem(nMenuToCheck, MF_CHECKED | MF_BYCOMMAND);

			m_hLastCtxtItem = hItem;
			if (bInSelection)
				// Click in a selection - change the whole selection
				m_hLastCtxtItem = NULL;
			else
				SetItemState(hItem, TVIS_BOLD, TVIS_BOLD );

			if (m_hLastCtxtItem)
			{
				pmnuCtxt->EnableMenuItem(IDC_DISPINFO,
										 MF_BYCOMMAND | (bIsFolder /*|| (GetItemData(hItem) == 0)*/ ? MF_DISABLED | MF_GRAYED : MF_ENABLED));
				pmnuCtxt->EnableMenuItem(IDC_EDITURL,
										 MF_BYCOMMAND | (bIsFolder ? MF_DISABLED | MF_GRAYED : MF_ENABLED));
				pmnuCtxt->EnableMenuItem(IDC_PICKICON,
										 MF_BYCOMMAND | (bIsFolder ? MF_DISABLED | MF_GRAYED : MF_ENABLED));
			}
			else
			{
				pmnuCtxt->EnableMenuItem(IDC_DISPINFO, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
				pmnuCtxt->EnableMenuItem(IDC_EDITURL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
				pmnuCtxt->EnableMenuItem(IDC_PICKICON, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			}

			pmnuCtxt->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this, NULL);
			// Now wait for command messages

			SetItemState(hItem, 0, TVIS_BOLD );
		}
	}

	// Helper to fix tree control bug
	if (m_SelItemList.IsEmpty())
		SelectItem(NULL);
}

void CFavOrgTreeCtl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Allow scrolling
	if ((!m_bProcessing)
		|| (((nChar == VK_NEXT) || (nChar == VK_PRIOR))
		     && (!(GetKeyState(VK_SHIFT) & 0x8000))
			 && (!(GetKeyState(VK_CONTROL) & 0x8000))
			 && (!(GetKeyState(VK_MENU) & 0x8000))))
	{
		CMultiselTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
		m_bLetUserScroll = TRUE;
	}
}

int CFavOrgTreeCtl::SetTimeout(int nTimeout)
{
	int nTemp = m_nTimeout;
	m_nTimeout = nTimeout;

	// Set timeout
	/*
	if (m_pInet != NULL)
	{
		int nValue = nTimeout * 1000;

		m_pInet->SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, nValue);
		m_pInet->SetOption(INTERNET_OPTION_SEND_TIMEOUT, nValue);
		m_pInet->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, nValue);
	}
	*/

	return nTemp;
}

BOOL CFavOrgTreeCtl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_bProcessing)
	{
		SetCursor(LoadCursor(NULL, m_bProcessing ? IDC_APPSTARTING : IDC_WAIT));
		return(TRUE);
	}

	return CMultiselTreeCtrl::OnSetCursor(pWnd, nHitTest, message);
}

int CFavOrgTreeCtl::CountFolder(CStringEx sPath)
{
	// Count shortcuts in folder
    CFileFind	finder;
	CStringEx	sFileSpec;
	int			nCount = 0;

	sFileSpec = sPath;
	sFileSpec.AddSlash();
	sFileSpec += "*.*";

    BOOL bOK = finder.FindFile(sFileSpec);
    while (bOK)
	{
        bOK = finder.FindNextFile();

		CStringEx sFilename = finder.GetFileName();

        if (finder.IsDirectory() && !finder.IsDots())
		{
			CStringEx sNewPath = sPath;
			sNewPath.AddSlash();
			nCount += CountFolder(sNewPath + sFilename);
		}
		else if (!finder.IsDirectory()
			     &&
				 (sFilename.GetExtension().CompareNoCase(g_szUrlExt) == 0))
            nCount++;

		PeekAndYield();
	}

	return nCount;
}

BOOL CFavOrgTreeCtl::SelectionExist(BOOL& bOneShortcut, BOOL& bOneFolder)
{
	bOneShortcut = (m_SelItemList.GetCount() == 1)
					&&
					(GetItemData(m_SelItemList.GetHead()) != ITEM_IS_FOLDER);

	bOneFolder = (m_SelItemList.GetCount() == 1)
					&&
					(GetItemData(m_SelItemList.GetHead()) == ITEM_IS_FOLDER);

	return !m_SelItemList.IsEmpty();
}

int CFavOrgTreeCtl::CountSelected()
{
	// Count how many items are selected
	HTREEITEM hCurItem;

	if (m_SelItemList.IsEmpty())
		return 0;

	hCurItem = m_SelItemList.GetHead();

	if (GetItemData(hCurItem) == ITEM_IS_FOLDER)
	{
		// It's a selection of folders
		int nCount = 0;

		POSITION pos = m_SelItemList.GetHeadPosition();

		while (!m_bCancel && (pos != NULL))
		{
			hCurItem = m_SelItemList.GetNext(pos);
			nCount += CountFolder(MakeFullItemName(hCurItem, FALSE));
		}

		return nCount;
	}
	else
		return m_SelItemList.GetCount();
}

void CFavOrgTreeCtl::DisplayMsg(int nTitleId, CString sContent)
{
	EnterCriticalSection(&(CFavOrgTreeCtl::mp_Me->m_csDisplayMsg));

	CString sTitle;
	CString sMessage;

	sTitle.LoadString(nTitleId);
	sMessage.Format(sTitle, sContent);
	/*
	CFavOrgTreeCtl::mp_Me->GetParent()->SendMessage(WM_DISP_MSG, 0, (LPARAM) sMessage.GetBuffer(sMessage.GetLength() + 1));
	sMessage.ReleaseBuffer(-1);
	*/
	m_sCurrentMsg = sMessage;

	LeaveCriticalSection(&(CFavOrgTreeCtl::mp_Me->m_csDisplayMsg));
}

void CFavOrgTreeCtl::ClearMsg()
{
	GetParent()->PostMessage(WM_DISP_MSG, 0, NULL);
}

void CFavOrgTreeCtl::UpdateItemIcon(HTREEITEM hItem, HICON hIcon, CString sIconFilename, int nIndex)
{
	// Change the icon of a given shortcut
	CString sFilename = MakeFullItemName(hItem, TRUE);

	IUniformResourceLocatorPtr pNewUrlLink(CLSID_InternetShortcut, NULL, CLSCTX_INPROC_SERVER);
	IUniformResourceLocatorPtr pUrlLink(CLSID_InternetShortcut, NULL, CLSCTX_INPROC_SERVER);
	if ((pUrlLink != NULL) && (pNewUrlLink != NULL))
	{
		IPersistFilePtr pNewpf(pNewUrlLink);
		IPersistFilePtr ppf(pUrlLink);

		if ((ppf != NULL) && (pNewpf != NULL))
		{
			WCHAR wsz[MAX_PATH];

			// Use UNICODE string anyway
			MultiByteToWideChar(CP_ACP, 0, sFilename, -1, wsz, MAX_PATH);
			// Load the shortcut.
			if (SUCCEEDED(ppf->Load(wsz, STGM_READ)))
			{
				IShellLinkPtr psl(pNewUrlLink);

				if (psl != NULL)
				{
					LPSTR lpszCurrentURL;

					pUrlLink->GetURL(&lpszCurrentURL);
					pNewUrlLink->SetURL(lpszCurrentURL, IURL_SETURL_FL_GUESS_PROTOCOL);
					SetItemIcon(hItem, sIconFilename, hIcon, nIndex, psl);
					DeleteFile(sFilename);
					pNewpf->Save(wsz, TRUE);

					// A trick to make sure that the newly assigned
					// image is correctly refreshed. Bug in tree control?
					int nImage, nSelectedImage;
					GetItemImage(hItem, nImage, nSelectedImage);
					SetItemImage(hItem, nImage, nSelectedImage);

					if (!m_bLetUserScroll)
						EnsureVisible(hItem);
				}
			}
		}
	}

	if (m_pProgressCtl && !m_bCancel)
		m_pProgressCtl->OffsetPos(1);
}

void CFavOrgTreeCtl::ProcessItemIcon(HTREEITEM hItem, HICON hIcon, CString sIconFilename, int nIndex)
{
	if (GetItemData(hItem) == ITEM_IS_FOLDER)
	{
		// It's a folder (we have a selection of folders)
		// Process all items in this folder
		HTREEITEM hChildItem = GetChildItem(hItem);

		while (!m_bCancel && (hChildItem != NULL))
		{
			if (GetItemData(hChildItem) == ITEM_IS_FOLDER)
				ProcessItemIcon(hChildItem, hIcon, sIconFilename, nIndex);
			else
				UpdateItemIcon(hChildItem, hIcon, sIconFilename, nIndex);
			hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
		}

		if (m_bCancel)
			GetParent()->PostMessage(WM_CAN_UNLOCK, 0, 0);

		if (!m_bLetUserScroll)
			EnsureVisible(hItem);
	}
	else
		UpdateItemIcon(hItem, hIcon, sIconFilename, nIndex);
}

void CFavOrgTreeCtl::AssignCustomIcon(CString sIconFilename, int nIconIndex, HTREEITEM hItem)
{
	if ((m_SelItemList.IsEmpty()) && (hItem == NULL))
		return;

	HICON hIconLarge, hIconSmall;

	m_bLetUserScroll = FALSE;

	// Is there any icon in that file?
	ExtractIconEx(sIconFilename, nIconIndex, &hIconLarge, &hIconSmall, 1);

	if (hIconSmall == NULL)
	{
		hIconSmall = hIconLarge;
		hIconLarge = NULL;
	}

	if (hIconSmall != NULL)
	{
		// Yes - assign it
		HTREEITEM	hCurItem;

		if (hItem != NULL)
		{
			// Just process this one)
			ProcessItemIcon(hItem, hIconSmall, sIconFilename, nIconIndex);
			return;
		}

		m_bCancel = FALSE;
		m_bProcessing = TRUE;

		if (m_pProgressCtl)
		{
			m_pProgressCtl->ShowWindow(SW_NORMAL);
			m_pProgressCtl->SetStep(1);
			m_pProgressCtl->SetPos(0);
		}

		POSITION pos;

		if (AfxMessageBox(IDS_ASSIGNSELALERT, MB_YESNO | MB_ICONEXCLAMATION, 0) == IDNO)
			goto stopprocess;

		if (m_pProgressCtl)
			m_pProgressCtl->SetRange32(0, m_nLoadedItems);

		// Enumerate selected items
		pos = m_SelItemList.GetHeadPosition();

		while (!m_bCancel && (pos != NULL))
		{
			hCurItem = m_SelItemList.GetNext(pos);
			ProcessItemIcon(hCurItem, hIconSmall, sIconFilename, nIconIndex);
		}

stopprocess:

		if (m_bCancel)
			GetParent()->PostMessage(WM_CAN_UNLOCK, 0, 0);

		m_bCancel = FALSE;
		if (m_pProgressCtl)
		{
			m_pProgressCtl->SetPos(0);
			m_pProgressCtl->ShowWindow(SW_HIDE);
		}

		DestroyIcon(hIconSmall);
		Invalidate();
		UpdateWindow();

		SetCursor(LoadCursor(NULL, IDC_ARROW));
		m_bProcessing = FALSE;
	}
}

void CFavOrgTreeCtl::AssignThisImage(HTREEITEM hItem)
{
	// Assign the picked up icon to the selected shortcuts
	IUniformResourceLocatorPtr pUrlLink(CLSID_InternetShortcut, NULL, CLSCTX_INPROC_SERVER);
	if (pUrlLink != NULL)
	{
		IShellLinkPtr psl(pUrlLink);
		IPersistFilePtr ppf(pUrlLink);

		if (ppf != NULL)
		{
			WCHAR wsz[MAX_PATH];

	        MultiByteToWideChar(CP_ACP, 0, m_sPickedShortcut, -1, wsz, MAX_PATH);
		    // Load the shortcut.
			if (SUCCEEDED(ppf->Load(wsz, STGM_READ)))
			{
				CString	sIconFile;
				int		nIndex;

				psl->GetIconLocation(sIconFile.GetBuffer(MAX_PATH), MAX_PATH, &nIndex);
				sIconFile.ReleaseBuffer();
				AssignCustomIcon(sIconFile, nIndex, hItem);
			}
		}
	}
}

void CFavOrgTreeCtl::PickCurrentIcon(HTREEITEM hItem)
{
	// Get the icon of the currently selected shorcut
	if ((m_SelItemList.GetCount() == 1) || (hItem != NULL))
	{
		HTREEITEM hSourceItem;

		if (hItem)
			hSourceItem = hItem;
		else
			hSourceItem = GetSelectedItem();

		if (GetItemData(hSourceItem) != ITEM_IS_FOLDER)
		{
			CString sFilename = MakeFullItemName(hSourceItem, TRUE);
			CString sUrl;
			int nSelected, nNotSelected;

			GetItemImage(hSourceItem, nNotSelected, nSelected);
			GetParent()->SendMessage(WM_DISP_ICON, 0, (LPARAM) m_imgStd.ExtractIcon(nNotSelected));
			m_sPickedShortcut = sFilename;
		}
	}
}

void CFavOrgTreeCtl::DestroyItem(HTREEITEM hItem, BOOL bIsDir)
{
	// No permanent delete - always send to Recycle Bin
	// (unless the Favorites folder is a network share)
	CString sName = MakeFullItemName(hItem, !bIsDir);

	char szBuffer[MAX_PATH + 1];

	strcpy(szBuffer, sName.GetBuffer(MAX_PATH));
	szBuffer[strlen(szBuffer) + 1] = '\0';

	SHFILEOPSTRUCT shfo;

	ZeroMemory(&shfo, sizeof(SHFILEOPSTRUCT));
	shfo.hwnd = m_hWnd;
	shfo.wFunc = FO_DELETE;
	shfo.fFlags = FOF_NOCONFIRMATION | FOF_ALLOWUNDO;
	shfo.pFrom = szBuffer;

	if (SHFileOperation(&shfo) == 0)
	{
		// Remove item from tree
		DeleteItem(hItem);
		if (!bIsDir)
		{
			m_nLoadedItems--;
			m_nNumItems--;
		}
	}
}

void CFavOrgTreeCtl::RemoveItem(HTREEITEM hItem)
{
	if (GetItemData(hItem) == ITEM_IS_FOLDER)
	{
		// It's a folder (we have a selection of folders)
		// Process all items in this folder
		HTREEITEM hChildItem = GetChildItem(hItem);
		HTREEITEM hNextItem;

		while (!m_bCancel && (hChildItem != NULL))
		{
			hNextItem = GetNextItem(hChildItem, TVGN_NEXT);
			if (GetItemData(hChildItem) == ITEM_IS_FOLDER)
				RemoveItem(hChildItem);
			else
				DestroyItem(hChildItem, FALSE);
			hChildItem = hNextItem;
		}

		DestroyItem(hItem, TRUE);

		if (m_bCancel)
			GetParent()->PostMessage(WM_CAN_UNLOCK, 0, 0);
	}
	else
		DestroyItem(hItem, FALSE);
}

void CFavOrgTreeCtl::RemoveSelected()
{
	if (m_SelItemList.IsEmpty())
		return;

	HTREEITEM	hCurItem;

	m_bCancel = FALSE;
	m_bProcessing = TRUE;

	if (m_pProgressCtl)
	{
		m_pProgressCtl->ShowWindow(SW_NORMAL);
		m_pProgressCtl->SetStep(1);
		m_pProgressCtl->SetPos(0);
	}

	if (m_pProgressCtl)
		m_pProgressCtl->SetRange32(0, CountSelected());

	// Enumerate selected items
	POSITION pos = m_SelItemList.GetHeadPosition();

	while (!m_bCancel && (pos != NULL))
	{
		hCurItem = m_SelItemList.GetNext(pos);
		if (hCurItem == m_hRootItem)
		{
			if (AfxMessageBox(IDS_DELETEALLALERT, MB_YESNO | MB_ICONEXCLAMATION, 0) != IDYES)
				goto stopdeleteprocess;
		}
		RemoveItem(hCurItem);
	}

stopdeleteprocess:

	ClearSelection(TRUE);

	if (m_bCancel)
		GetParent()->PostMessage(WM_CAN_UNLOCK, 0, 0);

	m_bCancel = FALSE;
	if (m_pProgressCtl)
	{
		m_pProgressCtl->SetPos(0);
		m_pProgressCtl->ShowWindow(SW_HIDE);
	}

	Invalidate();

	SetCursor(LoadCursor(NULL, IDC_ARROW));
	m_bProcessing = FALSE;
}

void CFavOrgTreeCtl::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	// Handle edition of item (shorcut) name
	// (F2 or two clicks)
	TVITEM	TvItem = ((TV_DISPINFO*)pNMHDR)->item;

	// Subclass edit control with our CTreeCtrlEdit
	m_ctlEdit.SubclassWindow(GetEditControl()->m_hWnd);
	m_ctlEdit.SetCurItem(TvItem.hItem);
	m_ctlEdit.SetTreeCtrl(this);

	if ((TvItem.mask & TVIF_TEXT) != 0)
	{
		BOOL bIsDir = GetItemData(TvItem.hItem) == ITEM_IS_FOLDER;

		m_sOldLabel = TvItem.pszText;
		m_sOldFilename = MakeFullItemName(TvItem.hItem, !bIsDir);
	}

	*pResult = 0;
}

void CFavOrgTreeCtl::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	// Handle edition of item (shorcut) name
	// (F2 or two clicks)
	TVITEM	TvItem = ((TV_DISPINFO*)pNMHDR)->item;

	if ((TvItem.mask & TVIF_TEXT) != 0)
	{
		if ((TvItem.pszText != NULL) && (m_sOldLabel.CompareNoCase(TvItem.pszText) != 0))
		{
			BOOL bIsDir = GetItemData(TvItem.hItem) == ITEM_IS_FOLDER;

			CStringEx sNewName;

			sNewName = m_sOldFilename;
			sNewName.ReplaceNameComponent(CString(TvItem.pszText), EX_STRING_REPNAME);
			if (!MoveFile(m_sOldFilename, sNewName))
				// Restore old name if failed
				SetItemText(TvItem.hItem, m_sOldLabel);
		}
	}

	m_ctlEdit.UnsubclassWindow();
	m_ctlEdit.SetCurItem(NULL);
	m_ctlEdit.SetTreeCtrl(NULL);

	*pResult = 0;
}

void CFavOrgTreeCtl::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	// Activate edition of the URL when a single item is selected
	HTREEITEM hItem = GetSelectedItem();

	if ((m_SelItemList.GetCount() == 1)
		||
		((m_SelItemList.GetCount() == 0) && (hItem != NULL)))
	{
		if ((hItem != NULL) && (GetItemData(hItem) != ITEM_IS_FOLDER))
		{
			CString sFilename = MakeFullItemName(hItem, TRUE);
			CString sUrl;

			GetOrSetURLFromShortcut(sFilename, sUrl, TRUE);

			CString* psMessage = new CString;

			*psMessage = sUrl;
			GetParent()->PostMessage(WM_DISP_MSG, 0, (LPARAM) psMessage);
		}
		else
			goto folderselected;
	}
	else
	{
folderselected:
		GetParent()->PostMessage(WM_DISP_MSG, 0, NULL);
	}

	*pResult = 0;
}

void CFavOrgTreeCtl::UpdateUrl(HTREEITEM hItem)
{
	// Update URL with new name
	if ((m_SelItemList.GetCount() == 1) || (hItem != NULL))
	{
		HTREEITEM hSourceItem;

		if (hItem)
			hSourceItem = hItem;
		else
			hSourceItem = GetSelectedItem();

		if (GetItemData(hSourceItem) != ITEM_IS_FOLDER)
		{
			CString sFilename = MakeFullItemName(hSourceItem, TRUE);
			CEditUrlDlg dlg;

			GetOrSetURLFromShortcut(sFilename, dlg.m_sRefLabel, TRUE);

			dlg.m_sNewUrl = dlg.m_sRefLabel;
			if (dlg.DoModal() == IDOK)
				GetOrSetURLFromShortcut(sFilename, dlg.m_sNewUrl, FALSE);

			CString* psMsg = new CString;

			*psMsg = dlg.m_sNewUrl;
			GetParent()->PostMessage(WM_DISP_MSG, 0, (LPARAM) psMsg);
		}
	}
}

void CFavOrgTreeCtl::PeekAndYield()
{
	// A helper yielding control to the main window
	MSG	 Msg;

	EnterCriticalSection(&m_csDisplayMsg);
	if (!m_sCurrentMsg.IsEmpty())
	{
		CString* psMessage = new CString;

		*psMessage = m_sCurrentMsg;
		GetParent()->PostMessage(WM_DISP_MSG, 0, (LPARAM) psMessage);
	}
	LeaveCriticalSection(&m_csDisplayMsg);

	while(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
	{
		if (!::IsDialogMessage(AfxGetMainWnd()->m_hWnd, &Msg))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}
}

void CFavOrgTreeCtl::ModifyStates(HTREEITEM hItem, int nStateIndex)
{
	// Modify state of selection or single item when receiving
	// the corresponding command from the popup context menu
	if (hItem != NULL)
	{
		if (GetItemData(hItem) == ITEM_IS_FOLDER)
		{
			// It's a folder
			// Process all items in this folder
			HTREEITEM hChildItem = GetChildItem(hItem);

			while (hChildItem != NULL)
			{
				if (GetItemData(hChildItem) == ITEM_IS_FOLDER)
					ModifyStates(hChildItem, nStateIndex);
				else
					SetItemState(hChildItem, INDEXTOSTATEIMAGEMASK(nStateIndex), TVIS_STATEIMAGEMASK);
				hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
			}
		}
		else
			SetItemState(hItem, INDEXTOSTATEIMAGEMASK(nStateIndex), TVIS_STATEIMAGEMASK);
	}
	else if (!m_SelItemList.IsEmpty())
	{
		// Modify state of selection
		POSITION pos = m_SelItemList.GetHeadPosition();
		HTREEITEM hCurItem;

		while (pos != NULL)
		{
			hCurItem = m_SelItemList.GetNext(pos);
			ModifyStates(hCurItem, nStateIndex);
		}
	}
	else
		ModifyStates(m_hRootItem, nStateIndex);
}

BOOL CFavOrgTreeCtl::ExecLocalCommand(UINT nID, BOOL bFromContextMenu)
{
	switch(nID)
	{
		case IDC_MNU_QUESTION:
		case IDC_MNU_CHECKED:
		case IDC_MNU_PAGENONE:
		case IDC_MNU_MOVED:
		case IDC_MNU_SITENONE:
		case IDC_MNU_TIMEOUT:
		case IDC_MNU_ACCESSDENIED:
		case IDC_MNU_MOVED_UPDATED:
		case IDC_MNU_UNCHECKED:
		case IDC_MNU_RESET:
		{
			// Item state has been changed
			int nStateIndex = nID - 500;

			if (!bFromContextMenu || (m_hLastCtxtItem == NULL) || (GetItemData(m_hLastCtxtItem) == ITEM_IS_FOLDER))
			{
				m_bProcessing = TRUE;
				m_bCancel = FALSE;

				ModifyStates(m_hLastCtxtItem, nStateIndex);

				m_bCancel = FALSE;
				m_bProcessing = FALSE;
			}
			else
				SetItemState(m_hLastCtxtItem, INDEXTOSTATEIMAGEMASK(nStateIndex), TVIS_STATEIMAGEMASK);

			return TRUE;
		}

		case IDC_DISPINFO:
		{
			// Display item info
			
			CString sIconDesc;

			// Get icon location
			IUniformResourceLocatorPtr pUrlLink(CLSID_InternetShortcut, NULL, CLSCTX_INPROC_SERVER);
			if (pUrlLink != NULL)
			{
				IShellLinkPtr psl(pUrlLink);
				IPersistFilePtr ppf(pUrlLink);

				if (ppf != NULL)
				{
					WCHAR wsz[MAX_PATH];
					CString sItemName = MakeFullItemName(bFromContextMenu ? m_hLastCtxtItem : GetSelectedItem(), TRUE);

					MultiByteToWideChar(CP_ACP, 0, sItemName, -1, wsz, MAX_PATH);
					// Load the shortcut.
					if (SUCCEEDED(ppf->Load(wsz, STGM_READ)))
					{
						CString	sIconFile;
						int		nIndex;

						psl->GetIconLocation(sIconFile.GetBuffer(MAX_PATH), MAX_PATH, &nIndex);
						sIconFile.ReleaseBuffer();
						sIconDesc.Format("Icon Location : %s - Index : %d\r\n\r\n", sIconFile, nIndex);
					}
				}
			}

			CString* psItemData;

			if (!bFromContextMenu)
				psItemData = (CString*) GetItemData(GetSelectedItem());
			else
				psItemData = (CString*) GetItemData(m_hLastCtxtItem);

			if (psItemData)
				AfxMessageBox(sIconDesc + *psItemData, MB_OK | MB_ICONINFORMATION, 0);
			else
				AfxMessageBox(sIconDesc, MB_OK | MB_ICONINFORMATION, 0);

			return TRUE;
		}

		case IDC_REMOVE:
			if (AfxMessageBox(IDS_DELETESELALERT, MB_YESNO | MB_ICONEXCLAMATION, 0) != IDYES)
				return TRUE;

			// Delete item
			if (m_hLastCtxtItem == NULL)
				// Destroy selection
				RemoveSelected();
			else
				DestroyItem(m_hLastCtxtItem, GetItemData(m_hLastCtxtItem) == ITEM_IS_FOLDER);

			GetParent()->SendMessage(WM_REFRESH_COUNT, 0, 0);

			return TRUE;

		case IDC_PICKICON:
			if (bFromContextMenu)
				PickCurrentIcon(m_hLastCtxtItem);
			else
				PickCurrentIcon(NULL);

			return TRUE;

		case IDC_ASSIGNTHIS:
			if (bFromContextMenu)
				AssignThisImage(m_hLastCtxtItem);
			else
				AssignThisImage(GetSelectedItem());

			return TRUE;

		case IDC_ASSIGNCUST:
			{
				DWORD		nIconIndex = 0;
				CStringEx	sIconFilename;

				GetSystemDirectory(sIconFilename.GetBuffer(MAX_PATH), MAX_PATH);
				sIconFilename.ReleaseBuffer(-1);
				sIconFilename.AddSlash();
				sIconFilename += "url.dll";

				if (!SelectIcon(AfxGetMainWnd()->m_hWnd, sIconFilename.GetBuffer(MAX_PATH), MAX_PATH, &nIconIndex))
				{
					sIconFilename.ReleaseBuffer(-1);
					return TRUE;
				}

				sIconFilename.ReleaseBuffer(-1);
				AssignCustomIcon(sIconFilename, nIconIndex, m_hLastCtxtItem);

				return TRUE;
			}

		case IDC_EDITURL:
			if (bFromContextMenu)
				UpdateUrl(m_hLastCtxtItem);
			else
				UpdateUrl(GetSelectedItem());

			return TRUE;

		default:
			return FALSE;
	}
}

BOOL CFavOrgTreeCtl::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// Handle commands triggered by the context menu
	if ((nCode == 0) && ExecLocalCommand(nID, TRUE))
	{
		m_hLastCtxtItem = NULL;
		return TRUE;
	}
	else
		return CMultiselTreeCtrl::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CFavOrgTreeCtl::ClearItemdata(HTREEITEM hItem)
{
	// Reset item data for the whole tree
	if (GetItemData(hItem) == ITEM_IS_FOLDER)
	{
		// It's a folder
		// Process all items in this folder
		HTREEITEM hChildItem = GetChildItem(hItem);

		while (hChildItem != NULL)
		{
			if (GetItemData(hChildItem) == ITEM_IS_FOLDER)
				ClearItemdata(hChildItem);
			else
			{
				DWORD dwData = GetItemData(hChildItem);
				if ((dwData != ITEM_IS_FOLDER) && ((dwData != 0)))
					delete (CString*) dwData;
			}
			hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
		}
	}
	else
	{
		DWORD dwData = GetItemData(hItem);
		if ((dwData != ITEM_IS_FOLDER) && (dwData != 0))
			delete (CString*) dwData;
	}
}


void CFavOrgTreeCtl::GenerateLogEntry(CFile* pLogfile, HTREEITEM hItem)
{
	CString sName = MakeFullItemName(hItem, TRUE);
	CString sURL;

	// Get URL
	if (GetOrSetURLFromShortcut(sName, sURL, TRUE))
	{
		CString sEntry = "\r\n\t" + sName + "\r\n\t" + sURL + "\r\n";
		OutputString(sEntry, pLogfile);
	}

	// Get status
	int nState = GetItemState(hItem, TVIS_STATEIMAGEMASK) & ~0xFF;
	CString sState;
	int nStringId;

	// Create status string
	switch(nState)
	{
		case INDEXTOSTATEIMAGEMASK(STATE_INDEX_QUESTION):
			nStringId = IDS_STATUS_UNKNOWN;
			break;
		case INDEXTOSTATEIMAGEMASK(STATE_INDEX_CHECKED):
			nStringId = IDS_STATUS_CHECKED;
			break;
		case INDEXTOSTATEIMAGEMASK(STATE_INDEX_PAGENONE):
			nStringId = IDS_STATUS_PAGENOTFOUND;
			break;
		case INDEXTOSTATEIMAGEMASK(STATE_INDEX_MOVED):
			nStringId = IDS_STATUS_PAGEMOVED;
			break;
		case INDEXTOSTATEIMAGEMASK(STATE_INDEX_TIMEOUT):
			nStringId = IDS_STATUS_TIMEOUT;
			break;
		case INDEXTOSTATEIMAGEMASK(STATE_INDEX_SITENONE):
			nStringId = IDS_STATUS_SITENOTFOUND;
			break;
		case INDEXTOSTATEIMAGEMASK(STATE_INDEX_ACCESSDENIED):
			nStringId = IDS_STATUS_ACCESSDENIED;
			break;
		case INDEXTOSTATEIMAGEMASK(STATE_INDEX_MOVED_UPDATED):
			nStringId = IDS_STATUS_MOVEDUPDATED;
			break;
		case INDEXTOSTATEIMAGEMASK(STATE_INDEX_UNCHECKED):
			nStringId = IDS_STATUS_UNCHECKED;
			break;
		case INDEXTOSTATEIMAGEMASK(STATE_INDEX_RESET):
			nStringId = IDS_STATUS_RESET;
			break;

	}

	sState.LoadString(nStringId);
	sState = "\tStatus : " + sState + "\r\n";
	OutputString(sState, pLogfile);

	// Get icon location
	IUniformResourceLocatorPtr pUrlLink(CLSID_InternetShortcut, NULL, CLSCTX_INPROC_SERVER);
	if (pUrlLink != NULL)
	{
		IShellLinkPtr psl(pUrlLink);
		IPersistFilePtr ppf(pUrlLink);

		if (ppf != NULL)
		{
			WCHAR wsz[MAX_PATH];
			CString sItemName = MakeFullItemName(hItem, TRUE);

	        MultiByteToWideChar(CP_ACP, 0, sItemName, -1, wsz, MAX_PATH);
		    // Load the shortcut.
			if (SUCCEEDED(ppf->Load(wsz, STGM_READ)))
			{
				CString	sIconFile;
				int		nIndex;

				psl->GetIconLocation(sIconFile.GetBuffer(MAX_PATH), MAX_PATH, &nIndex);
				sIconFile.ReleaseBuffer();

				CString sIconDesc;

				sIconDesc.Format("\tIcon Location : %s - Index : %d\r\n", sIconFile, nIndex);
				OutputString(sIconDesc, pLogfile);
			}
		}
	}

	// Get raw header retrieved from the response header
	DWORD dwData;
	if ((dwData = GetItemData(hItem)) != 0)
	{
		OutputString(CString("\t---------------\r\n"), pLogfile);

		CStringEx sMsg = *((CString*) dwData);
		sMsg = "\t" + sMsg;

		for (int nIndex = sMsg.GetLength() - 1; nIndex > 0; nIndex --)
		{
			if (sMsg.GetAt(nIndex) == (char) 0x0A)
				sMsg.InsertChar((char) 0x09, nIndex + 1);
		}

		OutputString(sMsg, pLogfile);
	}

	PeekAndYield();

	if (m_pProgressCtl && !m_bCancel)
		m_pProgressCtl->OffsetPos(1);
}

void CFavOrgTreeCtl::GenerateLogEntries(CFile* pLogfile, HTREEITEM hItem)
{
	if (GetItemData(hItem) == ITEM_IS_FOLDER)
	{
		// It's a folder
		CString sFolderTitle = "\r\nFolder : " + MakeFullItemName(hItem, FALSE) + "\r\n";
		OutputString(sFolderTitle, pLogfile);

		// Process all items in this folder
		HTREEITEM hChildItem = GetChildItem(hItem);

		while (!m_bCancel && (hChildItem != NULL))
		{
			if (GetItemData(hChildItem) == ITEM_IS_FOLDER)
				GenerateLogEntries(pLogfile, hChildItem);
			else
				GenerateLogEntry(pLogfile, hChildItem);
			hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
		}

		if (m_bCancel)
			GetParent()->PostMessage(WM_CAN_UNLOCK, 0, 0);
	}
	else
		GenerateLogEntry(pLogfile, hItem);
}

void CFavOrgTreeCtl::GenerateLog(CString sLogName)
{
	m_bProcessing = TRUE;
	m_bCancel = FALSE;

	try
	{
		CFile logfile(sLogName, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive);

		CString sHeader;
		CString sDate;
		CString sTime;

		// Create time stamp
		GetDateFormat(LOCALE_USER_DEFAULT, 0, NULL, NULL, sDate.GetBuffer(50), 50);
		sDate.ReleaseBuffer();
		GetTimeFormat(LOCALE_USER_DEFAULT, 0, NULL, NULL, sTime.GetBuffer(50), 50);
		sTime.ReleaseBuffer();

		sHeader.Format("FavOrg report - %s - %s\r\n\r\n", sDate, sTime);
		OutputString(sHeader, &logfile);

		HTREEITEM hCurItem;

		if (!m_SelItemList.IsEmpty())
		{
			// Enumerate selection
			if (m_pProgressCtl)
				m_pProgressCtl->SetRange32(0, CountSelected());

			// We have a selection
			POSITION pos = m_SelItemList.GetHeadPosition();

			while (!m_bCancel && (pos != NULL))
			{
				hCurItem = m_SelItemList.GetNext(pos);
				GenerateLogEntries(&logfile, hCurItem);
			}
		}
		else
		{
			// Enumerate tree
			if (m_pProgressCtl)
				m_pProgressCtl->SetRange32(0, m_nLoadedItems);

			hCurItem = GetChildItem(m_hRootItem);
			while (!m_bCancel && (hCurItem != NULL))
			{
				GenerateLogEntries(&logfile, hCurItem);
				hCurItem = GetNextItem(hCurItem, TVGN_NEXT);
			}
		}

		logfile.Close();
	}

	catch(CFileException* pEx)
	{
		pEx->Delete();

		DisplayMsg(IDS_MSG_LOGERROR, sLogName);
		PeekAndYield();

		m_bCancel = FALSE;
		if (m_pProgressCtl)
		{
			m_pProgressCtl->SetPos(0);
			m_pProgressCtl->ShowWindow(SW_HIDE);
		}

		SetCursor(LoadCursor(NULL, IDC_ARROW));
		m_bProcessing = FALSE;

		return;
	}

	m_bCancel = FALSE;
	if (m_pProgressCtl)
	{
		m_pProgressCtl->SetPos(0);
		m_pProgressCtl->ShowWindow(SW_HIDE);
	}

	SetCursor(LoadCursor(NULL, IDC_ARROW));
	m_bProcessing = FALSE;
}

void CFavOrgTreeCtl::OutputString(CString sText, CFile* pLogfile)
{
	// We need a special function managing the output of strings
	// in the report file. Using the CStdioFile creates problem
	// under certain circumstances because the HTTP response header
	// already contains CRLF sequences.
	int nLen = sText.GetLength();
	pLogfile->Write(sText.GetBuffer(nLen + 1), nLen);
	sText.ReleaseBuffer();
}


void CFavOrgTreeCtl::StoreItemStatus(HTREEITEM hItem, BOOL bIsShortcut)
{
	CString sName = MakeFullItemName(hItem, bIsShortcut);
	int nState;

	// Store item state to map for further serialization
	if (bIsShortcut)
		nState = GetItemState(hItem, TVIS_STATEIMAGEMASK);
	else
		nState = GetItemState(hItem, TVIS_EXPANDED);

	sName.MakeUpper();
	m_StatusMap.SetAt(sName, nState);

	DWORD dwData = GetItemData(hItem);
	if ((dwData != 0) && (dwData != ITEM_IS_FOLDER))
		m_InfoMap.SetAt(sName, *((CString*) dwData));
}

void CFavOrgTreeCtl::StoreItemsStatus(HTREEITEM hItem)
{
	if (GetItemData(hItem) == ITEM_IS_FOLDER)
	{
		// It's a folder
		// Process all items in this folder
		StoreItemStatus(hItem, FALSE);

		HTREEITEM hChildItem = GetChildItem(hItem);

		while (hChildItem != NULL)
		{
			if (GetItemData(hChildItem) == ITEM_IS_FOLDER)
				StoreItemsStatus(hChildItem);
			else
				StoreItemStatus(hChildItem, TRUE);
			hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
		}
	}
	else
		StoreItemStatus(hItem, TRUE);
}

void CFavOrgTreeCtl::StoreStatus()
{
	ClearSelection(TRUE);

	// Clear maps
	m_StatusMap.RemoveAll();
	m_InfoMap.RemoveAll();

	// Store individual items to map
	StoreItemsStatus(m_hRootItem);

	// Open file for map serialization
	CFile logfile;
	if(!logfile.Open(m_sStatusFile, CFile::modeCreate | CFile::modeWrite))
		return;

	try
	{
		// Serialize
		CArchive ar(&logfile, CArchive::store);
		m_StatusMap.Serialize(ar);
		ar.Close();
		logfile.Close();

		if(!logfile.Open(m_sInfoFile, CFile::modeCreate | CFile::modeWrite))
			return;

		CArchive arInfo(&logfile, CArchive::store);
		m_InfoMap.Serialize(arInfo);
		arInfo.Close();
		logfile.Close();
	}

	catch(CArchiveException* pEx)
	{
		pEx->Delete();
		logfile.Close();
		return;
	}
}

void CFavOrgTreeCtl::ProcessItemByState(HTREEITEM hItem, BOOL bAccessDenied, BOOL bTimeout, BOOL bPageNotFound, BOOL bSiteNone)
{
	int nActualState = GetItemState(hItem, TVIS_STATEIMAGEMASK) & ~0xFF;

	// If item has one of the user defined state, delete it
	if (((nActualState == INDEXTOSTATEIMAGEMASK(STATE_INDEX_ACCESSDENIED)) && bAccessDenied)
		 ||
		 ((nActualState == INDEXTOSTATEIMAGEMASK(STATE_INDEX_PAGENONE)) && bPageNotFound)
		 ||
		 ((nActualState == INDEXTOSTATEIMAGEMASK(STATE_INDEX_SITENONE)) && bSiteNone)
		 ||
		 ((nActualState == INDEXTOSTATEIMAGEMASK(STATE_INDEX_TIMEOUT)) && bTimeout))
	{
		DestroyItem(hItem, FALSE);
	}

	PeekAndYield();

	if (m_pProgressCtl && !m_bCancel)
		m_pProgressCtl->OffsetPos(1);
}

void CFavOrgTreeCtl::ProcessItemsByState(HTREEITEM hItem, BOOL bAccessDenied, BOOL bTimeout, BOOL bPageNotFound, BOOL bSiteNone)
{
	if (GetItemData(hItem) == ITEM_IS_FOLDER)
	{
		// It's a folder
		// Process all items in this folder
		HTREEITEM hChildItem = GetChildItem(hItem);
		HTREEITEM hNextItem;

		while (!m_bCancel && (hChildItem != NULL))
		{
			// Take care of deleted items while enumerating
			hNextItem = GetNextItem(hChildItem, TVGN_NEXT);
			if (GetItemData(hChildItem) == ITEM_IS_FOLDER)
				ProcessItemsByState(hChildItem, bAccessDenied, bTimeout, bPageNotFound, bSiteNone);
			else
				ProcessItemByState(hChildItem, bAccessDenied, bTimeout, bPageNotFound, bSiteNone);
			hChildItem = hNextItem;
		}

		if (m_bCancel)
			GetParent()->PostMessage(WM_CAN_UNLOCK, 0, 0);
	}
	else
		ProcessItemByState(hItem, bAccessDenied, bTimeout, bPageNotFound, bSiteNone);
}

void CFavOrgTreeCtl::DeleteByState()
{
	CByStateDlg dlg;

	dlg.m_pFavTree = this;

	// The user must select the relevant states
	if ((dlg.DoModal() == IDCANCEL)
		||
		(!dlg.m_bAccessDenied && !dlg.m_bTimeOut && !dlg.m_bPageNotFound && !dlg.m_bSiteNone))
		return;

	m_bProcessing = TRUE;

	if (m_pProgressCtl)
	{
		m_pProgressCtl->ShowWindow(SW_NORMAL);
		m_pProgressCtl->SetRange32(0, m_nLoadedItems);
		m_pProgressCtl->SetStep(1);
		m_pProgressCtl->SetPos(0);
	}

	m_bCancel = FALSE;

	ClearSelection(TRUE);
	// Find all items having the specified state
	ProcessItemsByState(m_hRootItem, dlg.m_bAccessDenied, dlg.m_bTimeOut, dlg.m_bPageNotFound, dlg.m_bSiteNone);

	m_bCancel = FALSE;
	if (m_pProgressCtl)
	{
		m_pProgressCtl->SetPos(0);
		m_pProgressCtl->ShowWindow(SW_HIDE);
	}
	ClearMsg();

	SetCursor(LoadCursor(NULL, IDC_ARROW));
	m_bProcessing = FALSE;
}

BOOL CFavOrgTreeCtl::IsIconOrBmp(BYTE* pBuffer, DWORD dwLen)
{
	// Quick and dirty check to see if we actually got
	// an icon or a bitmap
	ICONHEADER*			pIconHeader = (ICONHEADER*) pBuffer;
	ICONDIRENTRY*		pIconEntry = (ICONDIRENTRY*) (pBuffer + sizeof(WORD) * 3);
	BITMAPFILEHEADER*	pBmpHeader = (BITMAPFILEHEADER*) pBuffer;

	if ((pIconHeader->idType == 1)
		&&
		(pIconHeader->idReserved == 0)
		&&
		(dwLen >= sizeof(ICONHEADER) + sizeof(ICONDIRENTRY)) )
	{
		if (pIconEntry->dwImageOffset >= dwLen)
			goto checkifbmp;

		return TRUE;
	}

	// Not an icon
checkifbmp:

	BITMAPFILEHEADER* pBmpFileHeader = (BITMAPFILEHEADER*) pBuffer;
	BITMAPINFOHEADER* pBmpInfoHeader = (BITMAPINFOHEADER*) (pBuffer + sizeof(BITMAPFILEHEADER));

	if ((dwLen < sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER))
		||
		(pBmpFileHeader->bfType != BM)
		||
		(pBmpFileHeader->bfSize != dwLen))
		return FALSE;

	return TRUE;
}

void CFavOrgTreeCtl::CollapseOrExpandAll(HTREEITEM hItem, BOOL bCollapse)
{
	if (GetItemData(hItem) == ITEM_IS_FOLDER)
	{
		// It's a folder
		// Process all items in this folder
		Expand(hItem, bCollapse ? TVE_COLLAPSE : TVE_EXPAND);

		HTREEITEM hChildItem = GetChildItem(hItem);

		while (hChildItem != NULL)
		{
			if (GetItemData(hChildItem) == ITEM_IS_FOLDER)
				CollapseOrExpandAll(hChildItem, bCollapse);
			hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
		}
	}
}

void CFavOrgTreeCtl::ExpandAll()
{
	CollapseOrExpandAll(m_hRootItem, FALSE);
	EnsureVisible(m_hRootItem);
}

void CFavOrgTreeCtl::CollapseAll()
{
	CollapseOrExpandAll(m_hRootItem, TRUE);
	Expand(m_hRootItem, TVE_EXPAND);
}

void CFavOrgTreeCtl::ExpandCurrent()
{
	Expand(GetSelectedItem(), TVE_EXPAND);
}

void CFavOrgTreeCtl::CollapseCurrent()
{
	Expand(GetSelectedItem(), TVE_COLLAPSE);
}

void CFavOrgTreeCtl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	m_bLetUserScroll = TRUE;

	CMultiselTreeCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CFavOrgTreeCtl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	m_bLetUserScroll = TRUE;

	CMultiselTreeCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CFavOrgTreeCtl::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	// Do nothing
}

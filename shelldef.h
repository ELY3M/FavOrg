#ifndef _INC_SHELLDEF_
#define _INC_SHELLDEF_

#ifndef _ALREADY_HAVE_SHELL_HEADERS_
#define NO_INTSHCUT_GUIDS
#define CLSID_InternetShortcut
#include <shlobj.h>   // NOTE: Make sure INETSDK shlobj.h/intshcut.h are first in path
#undef CLSID_InternetShortcut
#undef NO_INTSHCUT_GUIDS
#include <intshcut.h>
#endif // !_ALREADY_HAVE_SHELL_HEADERS_


// CoClasses:
//struct __declspec(uuid("00021400-0000-0000-C000-000000000046")) ShellDesktop;
//struct __declspec(uuid("00021400-0000-0000-C000-000000000046")) ShellLink;
struct __declspec(uuid("B091E540-83E3-11CF-A713-0020AFD79762")) FileTypes;
#ifndef NO_INTSHCUT_GUIDS
//struct __declspec(uuid("FBF23B40-E3F0-101B-8488-00AA003E56F8")) InternetShortcut;
#endif // NO_INTSHCUT_GUIDS
#ifndef NO_SHDOCVW_GUIDS
struct __declspec(uuid("EAB22AC3-30C1-11CF-A7EB-0000C05BAE0B")) ShellExplorer;
struct __declspec(uuid("EAB22AC6-30C1-11CF-A7EB-0000C05BAE0B")) SHItemOC;
//struct __declspec(uuid("0002DF01-0000-0000-C000-000000000046")) InternetExplorer;
struct __declspec(uuid("0002DF09-0000-0000-C000-000000000046")) StdHyperLink;
#endif // NO_SHDOCVW_GUIDS

// Interfaces:
struct __declspec(uuid("000214E2-0000-0000-C000-000000000046")) IShellBrowser;
struct __declspec(uuid("000214E3-0000-0000-C000-000000000046")) IShellView;
struct __declspec(uuid("000214E4-0000-0000-C000-000000000046")) IContextMenu;
struct __declspec(uuid("000214E5-0000-0000-C000-000000000046")) IShellIcon;
struct __declspec(uuid("000214E6-0000-0000-C000-000000000046")) IShellFolder;
struct __declspec(uuid("000214E8-0000-0000-C000-000000000046")) IShellExtInit;
struct __declspec(uuid("000214E9-0000-0000-C000-000000000046")) IShellPropSheetExt;
struct __declspec(uuid("000214EA-0000-0000-C000-000000000046")) IPersistFolder;
struct __declspec(uuid("000214F1-0000-0000-C000-000000000046")) ICommDlgBrowser;
struct __declspec(uuid("000214F2-0000-0000-C000-000000000046")) IEnumIDList;
struct __declspec(uuid("000214F3-0000-0000-C000-000000000046")) IFileViewerSite;
struct __declspec(uuid("000214F4-0000-0000-C000-000000000046")) IContextMenu2;
struct __declspec(uuid("000214F6-0000-0000-C000-000000000046")) IPropSheetPage;
struct __declspec(uuid("88E39E80-3578-11CF-AE69-08002B2E1262")) IShellView2;
#ifdef UNICODE
struct __declspec(uuid("000214F7-0000-0000-C000-000000000046")) INewShortcutHook;
struct __declspec(uuid("000214F8-0000-0000-C000-000000000046")) IFileViewer;
struct __declspec(uuid("000214F9-0000-0000-C000-000000000046")) IShellLink;
struct __declspec(uuid("000214FA-0000-0000-C000-000000000046")) IExtractIcon;
struct __declspec(uuid("000214FB-0000-0000-C000-000000000046")) IShellExecuteHook;
struct __declspec(uuid("000214FC-0000-0000-C000-000000000046")) IShellCopyHook;
#ifndef NO_INTSHCUT_GUIDS
struct __declspec(uuid("CABB0DA0-DA57-11CF-9974-0020AFD79762")) IUniformResourceLocator;
#endif // NO_INTSHCUT_GUIDS
#else // !UNICODE
struct __declspec(uuid("000214E1-0000-0000-C000-000000000046")) INewShortcutHook;
struct __declspec(uuid("000214F0-0000-0000-C000-000000000046")) IFileViewer;
struct __declspec(uuid("000214EE-0000-0000-C000-000000000046")) IShellLink;
struct __declspec(uuid("000214EB-0000-0000-C000-000000000046")) IExtractIcon;
struct __declspec(uuid("000214F5-0000-0000-C000-000000000046")) IShellExecuteHook;
struct __declspec(uuid("000214EF-0000-0000-C000-000000000046")) IShellCopyHook;
#ifndef NO_INTSHCUT_GUIDS
struct __declspec(uuid("FBF23B80-E3F0-101B-8488-00AA003E56F8")) IUniformResourceLocator;
#endif // NO_INTSHCUT_GUIDS
#endif // !UNICODE
_COM_SMARTPTR_TYPEDEF(IShellFolder, __uuidof(IShellFolder));
_COM_SMARTPTR_TYPEDEF(IEnumIDList, __uuidof(IEnumIDList));
_COM_SMARTPTR_TYPEDEF(IShellBrowser, __uuidof(IShellBrowser));
_COM_SMARTPTR_TYPEDEF(IShellView, __uuidof(IShellView));
_COM_SMARTPTR_TYPEDEF(IContextMenu, __uuidof(IContextMenu));
_COM_SMARTPTR_TYPEDEF(IShellIcon, __uuidof(IShellIcon));
_COM_SMARTPTR_TYPEDEF(IShellFolder, __uuidof(IShellFolder));
_COM_SMARTPTR_TYPEDEF(IShellExtInit, __uuidof(IShellExtInit));
_COM_SMARTPTR_TYPEDEF(IShellPropSheetExt, __uuidof(IShellPropSheetExt));
_COM_SMARTPTR_TYPEDEF(IPersistFolder, __uuidof(IPersistFolder));
_COM_SMARTPTR_TYPEDEF(ICommDlgBrowser, __uuidof(ICommDlgBrowser));
_COM_SMARTPTR_TYPEDEF(IEnumIDList, __uuidof(IEnumIDList));
_COM_SMARTPTR_TYPEDEF(IFileViewerSite, __uuidof(IFileViewerSite));
_COM_SMARTPTR_TYPEDEF(IContextMenu2, __uuidof(IContextMenu2));
_COM_SMARTPTR_TYPEDEF(IPropSheetPage, __uuidof(IPropSheetPage));
_COM_SMARTPTR_TYPEDEF(IShellView2, __uuidof(IShellView2));
// From UNICODE-conditional versions
_COM_SMARTPTR_TYPEDEF(INewShortcutHook, __uuidof(INewShortcutHook));
_COM_SMARTPTR_TYPEDEF(IFileViewer, __uuidof(IFileViewer));
_COM_SMARTPTR_TYPEDEF(IShellLink, __uuidof(IShellLink));
_COM_SMARTPTR_TYPEDEF(IExtractIcon, __uuidof(IExtractIcon));
_COM_SMARTPTR_TYPEDEF(IShellExecuteHook, __uuidof(IShellExecuteHook));
_COM_SMARTPTR_TYPEDEF(IShellCopyHook, __uuidof(IShellCopyHook));
#ifndef NO_INTSHCUT_GUIDS
_COM_SMARTPTR_TYPEDEF(IUniformResourceLocator, __uuidof(IUniformResourceLocator));
#endif // NO_INTSHCUT_GUIDS

#ifndef NO_SHDOCVW_GUIDS
struct __declspec(uuid("EAB22AC1-30C1-11CF-A7EB-0000C05BAE0B")) IShellExplorer;
struct __declspec(uuid("EAB22AC2-30C1-11CF-A7EB-0000C05BAE0B")) DShellExplorerEvents;
struct __declspec(uuid("EAB22AC4-30C1-11CF-A7EB-0000C05BAE0B")) ISHItemOC;
struct __declspec(uuid("EAB22AC5-30C1-11CF-A7EB-0000C05BAE0B")) DSHItemOCEvents;
struct __declspec(uuid("0002DF07-0000-0000-C000-000000000046")) DHyperLink;
struct __declspec(uuid("0002DF05-0000-0000-C000-000000000046")) DIExplorer;
struct __declspec(uuid("0002DF06-0000-0000-C000-000000000046")) DExplorerEvents;
_COM_SMARTPTR_TYPEDEF(IShellExplorer, __uuidof(IShellExplorer));
_COM_SMARTPTR_TYPEDEF(DShellExplorerEvents, __uuidof(DShellExplorerEvents));
_COM_SMARTPTR_TYPEDEF(ISHItemOC, __uuidof(ISHItemOC));
_COM_SMARTPTR_TYPEDEF(DSHItemOCEvents, __uuidof(DSHItemOCEvents));
_COM_SMARTPTR_TYPEDEF(DHyperLink, __uuidof(DHyperLink));
_COM_SMARTPTR_TYPEDEF(DIExplorer, __uuidof(DIExplorer));
_COM_SMARTPTR_TYPEDEF(DExplorerEvents, __uuidof(DExplorerEvents));
#endif // NO_SHDOCVW_GUIDS

#endif //_INC_SHELLDEF_

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__26438FCC_6785_11D4_908E_00A0249EABF4__INCLUDED_)
#define AFX_STDAFX_H__26438FCC_6785_11D4_908E_00A0249EABF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxinet.h>		// MFC support for WinInet
#include <afxtempl.h>		// Template-based classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "LegendDlg.h"
#include "stringext.h"

#define INTERNET_CONNECTION_OFFLINE 0x20
#define INTERNET_CONNECTION_CONFIGURED 0x40

#define CMD_LEGEND_CLOSING		2000

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__26438FCC_6785_11D4_908E_00A0249EABF4__INCLUDED_)

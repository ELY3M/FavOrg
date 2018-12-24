// FAVORG Version 1.4
// Copyright (c) 2000 Ziff Davis Media, Inc.
// All rights reserved.
// First Published in PC Magazine, US Edition, November 7, 2000.
// Programmer: Patrick Philippot

#include "stdafx.h"
#include "selicondlg.h"

typedef WINSHELLAPI BOOL (WINAPI *PFNSELECTICONDLG)(HWND hWndParent, LPTSTR pszFilename, LPDWORD pdwBufferSize, LPDWORD pdwIndex);

// This function uses an undocumented call to open the
// icon selection dialog. Since this function is used by 
// many programs, this is not a dangerous decision.
BOOL SelectIcon(HWND hParent, LPSTR lpszFilename, DWORD dwBufferSize, DWORD *pdwIndex)
{
	BOOL			bResult = FALSE;
	OSVERSIONINFO	vi;

	// Get OS version
	vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&vi);

	HMODULE hDll = ::LoadLibrary("shell32.dll");

	if (hDll)	// Successfully loaded
	{
		PFNSELECTICONDLG SelectIconDlg = (PFNSELECTICONDLG) GetProcAddress(hDll, (LPCSTR) SELECTICONDLG_INDEX);

		if (SelectIconDlg != NULL)
		{
			if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT) // We're running NT
			{
				// Must convert to UNICODE
				LPWSTR pszWideName = new WCHAR[dwBufferSize];
				::MultiByteToWideChar(CP_ACP, 0, lpszFilename, -1, pszWideName, dwBufferSize);
				bResult = SelectIconDlg(hParent, (LPTSTR)pszWideName, &dwBufferSize, pdwIndex);
				WideCharToMultiByte(CP_ACP, 0, pszWideName, -1, lpszFilename, dwBufferSize, NULL, NULL);
				delete[] pszWideName;
			}
			else
				bResult = SelectIconDlg(hParent, (LPTSTR)lpszFilename, &dwBufferSize, pdwIndex);
		}

		FreeLibrary(hDll);
	}

	return bResult;
}



// ==========================================================
// alterIWnet project
// 
// Component: aCI
// Sub-component: aci2dll
// Purpose: Detection for the 'sph4ck' external Python hack
//          (version 1.8.1)
//
// Initial author: NTAuthority
// Started: 2012-01-19
// ==========================================================

#include "StdInc.h"
#include <Shlwapi.h>

// characteristics: program with command line containing
// 'launcher.py' registering a window with wsEx like
// WS_EX_TOPMOST | WS_EX_COMPOSITED | WS_EX_TRANSPARENT | WS_EX_LAYERED
// and ws WS_POPUP

#define SPH4CK_WINDOW_STYLE (WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS)
#define SPH4CK_WINDOW_STYLE_EX (WS_EX_TOPMOST | WS_EX_COMPOSITED | WS_EX_TRANSPARENT | WS_EX_LAYERED)

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	DWORD winStyleEx = GetWindowLong(hwnd, GWL_EXSTYLE);

	if (winStyleEx == SPH4CK_WINDOW_STYLE_EX)
	{
		DWORD winStyle = GetWindowLong(hwnd, GWL_STYLE);

		if (winStyle == SPH4CK_WINDOW_STYLE)
		{
			DWORD processId;
			
			if (SUCCEEDED(GetWindowThreadProcessId(hwnd, &processId)))
			{
				WCHAR* commandLine = GetCommandLinePid(processId);

				if (commandLine)
				{
					if (StrStrIW(commandLine, L"launcher.py"))
					{
						CI_SendStatus(CI_ID_T5_PYTHON_SPH4X_ESP);
						//MessageBoxA(NULL, "WHY", "WHYWHY", MB_OK);
					}
				}

				free(commandLine);
			}

		}
	}

	return TRUE;
}

void T5_Detection_sph4x_Check()
{
	EnumWindows(EnumWindowsProc, NULL);
}
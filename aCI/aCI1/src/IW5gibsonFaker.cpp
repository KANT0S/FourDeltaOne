// ==========================================================
// alterIWnet project
// 
// Component: aCI
// Sub-component: aci2dll
// Purpose: Detection for the 'gibsonNameFaker' name changing
//          tool.
//
// Initial author: NTAuthority
// Started: 2012-01-19
// ==========================================================

#include "StdInc.h"
#include <Shlwapi.h>

// characteristics: both a window named 'Cheat Engine 6.1'
// (hidden) and a window whose name contains the word 'gibson'
// exist. both belong to the same process.

static bool foundGibson = false;
static bool foundCE = false;
static DWORD gibsonPid;
static DWORD cePid;

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	char string[1024];
	GetWindowText(hwnd, string, sizeof(string));

	if (!stricmp(string, "MW3 gibsonNicknameFaker 1.4.382"))
	{
		GetWindowThreadProcessId(hwnd, &gibsonPid);

		foundGibson = true;
	}

	// second name
	if (StrStrI(string, "Cheat Engine 6.1"))
	{
		GetWindowThreadProcessId(hwnd, &cePid);

		if (!(GetWindowLong(hwnd, GWL_STYLE) & WS_VISIBLE))
		{
			foundCE = true;
		}
	}

	return TRUE;
}

void IW5_Detection_gibsonNameFaker_Check()
{
	EnumWindows(EnumWindowsProc, NULL);

	if (foundCE)
	{
		if (foundGibson)
		{
			if (gibsonPid == cePid)
			{
				CI_SendStatus(CI_ID_IW5_GIBSONFAKER);
			}
		}
	}
}
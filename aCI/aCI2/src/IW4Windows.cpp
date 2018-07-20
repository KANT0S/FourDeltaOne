// ==========================================================
// alterIWnet project
// 
// Component: aCI
// Sub-component: aci2dll
// Purpose: Detection for the 'gibsonNameFaker' name changing
//          tool.
//
// Initial author: conglomeration
// Started: 2013-01-25
// ==========================================================

#include "StdInc.h"
#include <Shlwapi.h>

/*char *IW4str[] = {
	"Cheat Engine 6.3",
	0
};

static bool foundCE = false;
static DWORD cePid;

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	char string[1024], tmp[40];
	GetWindowText(hwnd, string, sizeof(string));

	if (!stricmp(string, STRdecrypt(IW4str[0], tmp)))
	{
		GetWindowThreadProcessId(hwnd, &cePid);
		foundCE = true;
	}

	return TRUE;
}

void IW4_Detection_WindowCheck()
{
	EnumWindows(EnumWindowsProc, NULL);

	if (foundCE)
	{

	}
}*/
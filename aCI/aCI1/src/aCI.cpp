// ==========================================================
// alterIWnet project
// 
// Component: aCI
// Sub-component: aci2dll
// Purpose: main entry point for the aCI DLL
//
// Initial author: NTAuthority
// Started: 2012-01-18
// ==========================================================

#include "StdInc.h"
#include <libnp.h>

void T5_Init();
void IW5_Init();
void IW4_Init();

BOOL WINAPI DllMain(
					__in  HINSTANCE hinstDLL,
					__in  DWORD fdwReason,
					__in  LPVOID lpvReserved
					)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		// try detecting which game we're running in
		if (*(DWORD*)0x401279 == 0x9C80818B) // T5 7.0.164
		{
			T5_Init();
		}

		if (*(DWORD*)0x401B40 == 0x5638EC83) // IW5 1.4.382
		{
			IW5_Init();
		}

		if (*(DWORD*)0x401090 == 0x06FD9BE8)
		{
			IW4_Init();
		}
	}

	return true;
}

void CI_SendStatus(int status)
{
	char str[64];
	sprintf(str, "troll %d", status);
	NP_SendRandomString(str);
}
// ==========================================================
// alterIWnet project
// 
// Component: xnp
// Sub-component: libnp
// Purpose: external API functions
//
// Initial author: NTAuthority
// Started: 2011-06-28
// ==========================================================

#include "StdInc.h"

LIBNP_API bool LIBNP_CALL NP_Init()
{
	if (!RPC_Init())
	{
		return false;
	}

	Authenticate_Init();
	Messaging_Init();
	Friends_Init();

	return true;
}

LIBNP_API bool LIBNP_CALL NP_Shutdown()
{
	//Friends_Shutdown();
	RPC_Shutdown();
	return true;
}

LIBNP_API bool LIBNP_CALL NP_RunFrame()
{
	Async_RunCallbacks();
	return true;
}

BOOL WINAPI DllMain(
					__in  HINSTANCE hinstDLL,
					__in  DWORD fdwReason,
					__in  LPVOID lpvReserved
					)
{
	if (fdwReason != DLL_PROCESS_ATTACH)
	{
		return TRUE;
	}

	if (!IsBadReadPtr((void*)0x411350, 4))
	{
		if (*(WORD*)0x411350 != 0xEC83)
		{
			return TRUE;
		}

		DWORD oldProtect;
		VirtualProtect((LPVOID)0x464AE4, 1, PAGE_EXECUTE_READWRITE, &oldProtect);

		// patch from MW2 stuff, put here for old time's sake (and to prevent people from shipping the game without libnp.dll)
		// un-neuter Com_ParseCommandLine to allow non-connect_lobby
		*(BYTE*)0x464AE4 = 0xEB;

		// up
		VirtualProtect((LPVOID)0x5B9962, 2, PAGE_EXECUTE_READWRITE, &oldProtect);
		VirtualProtect((LPVOID)0x5BA652, 2, PAGE_EXECUTE_READWRITE, &oldProtect);

		// third basic .ff hash check
		*(WORD*)0x5B9962 = 0x9090;
		*(WORD*)0x5BA652 = 0x9090;
	}

	return TRUE;
}
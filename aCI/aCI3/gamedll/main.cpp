// ==========================================================
// aCI3 project
// 
// Component: gamedll
// Purpose: aCI3 entry points
//
// Initial author: NTAuthority
// Started: 2013-01-05
// ==========================================================

#include "aci3.h"
#include "init.h"

#pragma data_seg(".CRT$XCA")
PVOIDFUNC __xc_a[] = { NULL };

#pragma data_seg(".CRT$XCZ")
PVOIDFUNC __xc_z[] = { NULL };

#pragma data_seg()
#pragma comment(linker, "/merge:.CRT=.data")

void ciRunConstructors()
{
	PVOIDFUNC* cur = __xc_a;

	for (; cur < __xc_z; cur++)
	{
		if (*cur)
		{
			(*cur)();
		}
	}
}

void CI_SetDescriptor(ci_descriptor_t* descriptor);
ci_descriptor_t* legDesc;

extern "C" bool ciInit(int gameID, ci_descriptor_t* descriptor)
{
	CI_SetDescriptor(descriptor);

	legDesc = descriptor;

	//return true;

	return ci::main::initialize(gameID, descriptor);
}

#ifdef CIDEV
void DBGprintf(const char* format, ...)
{
	if (!format)
	{
		AllocConsole();

		freopen("CONIN$", "r", stdin); 
		freopen("CONOUT$", "w", stdout); 
		freopen("CONOUT$", "w", stderr); 

		printf("Welcome to aCI3!\n");
	}
	else
	{
		va_list ap;
		va_start(ap, format);
		vprintf(format, ap);
		va_end(ap);
	}
}
#endif

extern "C" BOOL WINAPI _DllMainCRTStartup(__in  HANDLE hinstDLL, __in  DWORD fdwReason, __in  LPVOID lpvReserved);
extern "C" BOOL WINAPI LegacyDllMain(__in  HANDLE hinstDLL, __in  DWORD fdwReason, __in  LPVOID lpvReserved);

extern "C" BOOL WINAPI DllMain(HANDLE dllHandle, DWORD reason, LPVOID reserved)
{
	// nope
}

extern "C" BOOL WINAPI ciMain(HANDLE dllHandle, DWORD reason, LPVOID reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		ci::main::obfuscateAway();

		ci::SystemAPI::initializeSystemApi();

		_DllMainCRTStartup(dllHandle, reason, reserved);
		LegacyDllMain(dllHandle, reason, reserved);

#ifdef CIDEV
		DBGprintf(NULL);
#endif

		//ciRunConstructors();	
	}

	return TRUE;
}

extern "C" int _purecall()
{
	*(DWORD*)0x73127312 = 0;

	return 0;
}
// ==========================================================
// alterOps project
// 
// Component: client
// Sub-component: clientdll
// Purpose: DLL entry point.
//
// Initial author: NTAuthority
// Started: 2010-11-21
// ==========================================================

#include "stdinc.h"
#include "steam_patch.h"
#include "dw/dw.h"

void InitAntiCode(void);

static BYTE originalCode[5];
static PBYTE originalEP = 0;

void Main_UnprotectModule(HMODULE hModule)
{
	PIMAGE_DOS_HEADER header = (PIMAGE_DOS_HEADER)hModule;
	PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((DWORD)hModule + header->e_lfanew);

	// unprotect the entire PE image
	SIZE_T size = ntHeader->OptionalHeader.SizeOfImage;
	DWORD oldProtect;
	VirtualProtect((LPVOID)hModule, size, PAGE_EXECUTE_READWRITE, &oldProtect);
}

void Sys_RunInit();
void DS_RunInit();
void Auth_VerifyIdentity();
void DisableOldVersions();

void Main_DoInit()
{
	// unprotect our entire PE image
	HMODULE hModule;
	if (SUCCEEDED(GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)Main_DoInit, &hModule)))
	{
		Main_UnprotectModule(hModule);
	}

	DisableOldVersions();
	dw_init();

	if (*(DWORD*)0x401B40 == 0x5638EC83)
	{
		g_isDedicated = false;
		Sys_RunInit();
	}
	else
	{
		g_isDedicated = true;
		DS_RunInit();
	}

	// return to the original EP
	memcpy(originalEP, &originalCode, sizeof(originalCode));
	__asm jmp originalEP
}

void Main_SetSafeInit()
{
	// find the entry point for the executable process, set page access, and replace the EP
	HMODULE hModule = GetModuleHandle(NULL); // passing NULL should be safe even with the loader lock being held (according to ReactOS ldr.c)

	if (hModule)
	{
		PIMAGE_DOS_HEADER header = (PIMAGE_DOS_HEADER)hModule;
		PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((DWORD)hModule + header->e_lfanew);

		Main_UnprotectModule(hModule);

		// back up original code
		PBYTE ep = (PBYTE)((DWORD)hModule + ntHeader->OptionalHeader.AddressOfEntryPoint);
		memcpy(originalCode, ep, sizeof(originalCode));

		// patch to call our EP
		int newEP = (int)Main_DoInit - ((int)ep + 5);
		ep[0] = 0xE9; // for some reason this doesn't work properly when run under the debugger
		memcpy(&ep[1], &newEP, 4);

		originalEP = ep;
	}
}

//extern "C" void __declspec(dllimport) DependencyFunctionCCAPI();

bool isClient = false;

BOOL __stdcall DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		Main_UnprotectModule(GetModuleHandle(NULL));

		if (*(DWORD*)0x401B40 == 0x5638EC83)
		{
			isClient = true;
		}

		InitAntiCode();

		Main_SetSafeInit();
	}

	return true;
}
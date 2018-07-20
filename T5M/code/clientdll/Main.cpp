// ==========================================================
// T5M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Manages the initialization of T5.
//
// Initial author: NTAuthority
// Started: 2013-04-06
// ==========================================================

#include "StdInc.h"

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

void Main_DoInit()
{
	// unprotect our entire PE image
	HMODULE hModule;
	if (SUCCEEDED(GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)Main_DoInit, &hModule)))
	{
		Main_UnprotectModule(hModule);
	}

	int gameVersion = 0;

	if (*(DWORD*)0x401279 == 0x0F003E83)
	{
		gameVersion = MP_VERSION(189);
	}
	else if (*(DWORD*)0x401279 == 0xCCCCCCC3)
	{
		gameVersion = SP_VERSION(152);
	}

	if (!IS_VALID_GAME(gameVersion))
	{
		ExitProcess(0x1);
	}

	Sys_RunInit(gameVersion);

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

void InitAntiCode();

bool __stdcall DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		//Main_UnprotectModule(GetModuleHandle(NULL));
		//InitAntiCode();
		Main_SetSafeInit();
	}

	return true;
}
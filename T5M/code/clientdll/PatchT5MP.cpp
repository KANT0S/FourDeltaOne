// ==========================================================
// T5M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: T5 7.0.189 (MP) initialization.
//
// Initial author: NTAuthority
// Started: 2013-04-15
// ==========================================================

#include "StdInc.h"
#include "DW/DW.h"
#include "T5TargetFuncs.h"

typedef void*	   (WINAPI * BINKMEMALLOC) (unsigned int bytes);
typedef void       (WINAPI * BINKMEMFREE)  (void* ptr);
typedef void (WINAPI * BinkSetMemory_t)(BINKMEMALLOC a,BINKMEMFREE f);

void* WINAPI _balloc(unsigned int size)
{
	return malloc(size);
}

void WINAPI _bfree(void* ptr)
{
	free(ptr);
}

DWORD WINAPI testcin(LPVOID)
{
	while (true)
	{
		Sleep(50);

		if (GetAsyncKeyState(VK_F14))
		{
			//BinkSetMemory_t BinkSetMemory = (BinkSetMemory_t)GetProcAddress(GetModuleHandle("binkw32.dll"), "_BinkSetMemory@8");
			//BinkSetMemory(_balloc, _bfree);

			call(0x7016E0, _balloc, PATCH_JUMP);
			call(0x701710, _bfree, PATCH_JUMP);

			//*(BYTE*)0x70184D = 0x90;
			//memset((void*)0x701853, 0x90, 6);

			//*(DWORD*)0x70185A &= ~0x1000000; // BINKIOSIZE
			//*(DWORD*)0x70185A = 0x400;

			*(BYTE*)0x536FAB0 = 1;
			strcpy((char*)0x4BD5DD8, "fdologo");
			*(float*)0x4BD6590 = 1;
			*(DWORD*)0x4BD5ED8 = 0x42;
			*(BYTE*)0xE67B3C = 2;
		}
	}

	return 0;
};

void PatchT5_Validate();

void PatchT5_MP189()
{
	// disable SetWindowsHookExA caller
	*(BYTE*)0x582600 = 0xC3;

	// sv_pure to 0
	*(BYTE*)0x585104 = 0x00;

	// some steam connect patch
	*(BYTE*)0x42AB70 = 0xC3;

	// disable fileops reset (quick fix for cacvalidate issues)
	*(BYTE*)0x67D140 = 0xC3;

	// make name minimum length 2
	*(BYTE*)0x57B88C = 1;

	// always request country code, even if this is a dedicated server
	*(BYTE*)0x4B4B5B = 0xEB;

	// country code also for non-5 type
	*(WORD*)0x649B07 = 0x9090;
	memset((void*)0x879BE0, 0x90, 6);

	// post-148 check for 'can play online', which makes no sense on dedis
	memset((void*)0x875445, 0x90, 5);
	*(BYTE*)0x87544F = 0xEB;

	// argument passed to Com_Init which should be sys_cmdline but is some empty string
	*(DWORD*)0x403683 = 0x39CD138;

	PatchT5_Validate();

	// enable Steam auth
	//memset((void*)0x5B1DAC, 0x90, 6); (old addr)

	// --------------------------------------------------------------------------------
	// cg_drawVersion
	/**(BYTE*)0x5CAB36 = 1; // default
	*(BYTE*)0x5CAB34 = 0x50; // flags

	// y 450, x 10
	*(DWORD*)0x5CAB6A = (DWORD)&drawVersionX;
	*(DWORD*)0x5CAB9F = (DWORD)&drawVersionY;

	*(DWORD*)0xA12438 = (DWORD)customTimeBeginPeriod;*/

	// set port to 28960
	*(DWORD*)0x51EB55 = 28960;

	// disable UPnP
	//*(WORD*)0x973C6B = 0x9090;

	// remove the windows hook setting function
	*(BYTE*)0x582600 = 0xC3;

	// con stuff
	*(WORD*)0x803BF5 = 0x9090; // dvar set 1

	*(BYTE*)0x67E180 = 0xC3;   // dvar reset flag
	*(WORD*)0x4101DB = 0x9090; // dvar exec
	*(WORD*)0x869B8E = 0x9090; // cmd  exec
	*(WORD*)0x6B58FE = 0x9090; // dvar enum
	*(WORD*)0x4EB5B4 = 0x9090; // cmd  enum

	//bdLogHook.initialize("aaaaa", 5, (PBYTE)bdLogHookLoc);
	//bdLogHook.installHook(BDLogHookStub, true, true);

	//rawInterceptorHook.initialize("a", 5, (PBYTE)rawInterceptorHookLoc);
	//rawInterceptorHook.installHook(RawInterceptorHookStub, true, false);

	// force playlists to not be used (playlist_enabled non-ranked license)
	//*(BYTE*)0x88C3BA = 0xEB;
	//*(BYTE*)0x87C4D5 = 0xEB; // map_rotate

	// classic fs_game-for-script patch
	*(WORD*)0x8E37B7 = 0x9090;
	*(WORD*)0x8E37BF = 0x9090;

	// also enable voting if ranked server licensetype
	memset((void*)0x50E5C8, 0x90, 6);
	*(BYTE*)0x58A186 = 0xEB; // do not set g_allowVote to 0 if ranked

	// disable democlient
	//*(BYTE*)0x4BBE30 = 0xC3;

	// disable IWD validation
	*(BYTE*)0x8B3052 = 0xEB;

	// allow non-iw_ IWDs in main/
	*(BYTE*)0x8B2F5C = 0xEB;

	// hardcoding DW dedi key
	nop(0x4D3770, 5); // don't read a key
	*(BYTE*)0x4D377A = 0xEB; // don't error if no key read

	*(DWORD*)0x4D379A = (DWORD)DEDI_KEY;

	// matchmaking_mininfo default to 0 (old behavior)
	*(BYTE*)0x5B09EB = 0;

	// ShellExecuteA
	*(DWORD*)0xA0C318 = (DWORD)ShellExecuteA_hook;

	CreateThread(0, 0, testcin, 0, 0, 0);
}

void BDLogHookFunc(int a1, char* type, char* channel, char* file, char* func, int line, char* message, ...)
{
	static char buffer[32768];
	static char mbuffer[32768];
	va_list args;
	va_start(args, message);
	_vsnprintf(mbuffer, sizeof(mbuffer), message, args);
	va_end(args);

	_snprintf(buffer, sizeof(buffer), "bdLog: %s(%s%s): %s\n", "", type, func, mbuffer);

	OutputDebugString(buffer);
}

void dw_patch_mp_189(dw_entry* entryPoints)
{
	call(0x9887C0, BDLogHookFunc, PATCH_JUMP);

	*(DWORD*)0xA0C460 = (DWORD)entryPoints->recv;
	*(DWORD*)0xA0C46C = (DWORD)entryPoints->connect;
	*(DWORD*)0xA0C474 = (DWORD)entryPoints->send;
	*(DWORD*)0xA0C47C = (DWORD)entryPoints->recvfrom;
	*(DWORD*)0xA0C480 = (DWORD)entryPoints->closesocket;
	*(DWORD*)0xA0C490 = (DWORD)entryPoints->select;
	*(DWORD*)0xA0C4A4 = (DWORD)entryPoints->gethostbyname;
	*(DWORD*)0xA0C4A8 = (DWORD)entryPoints->sendto;
}
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

typedef void (__cdecl * Field_Clear_t)(void* field);
Field_Clear_t Field_Clear = (Field_Clear_t)0x50DB60;

void Con_ToggleConsole()
{
        // possibly cls.keyCatchers?
        DWORD* menuFlags = (DWORD*)0x2910160;
        *menuFlags ^= 1;

        // g_consoleField
        Field_Clear((void*)0xC72280);

        // show console output?
        *(BYTE*)0xC6924C = 0;
}

CallHook toggleConsoleHook1;
DWORD toggleConsoleHook1Loc = 0x587DC8;

CallHook toggleConsoleHook2;
DWORD toggleConsoleHook2Loc = 0x587633;

StompHook sustainAmmoHook;
DWORD sustainAmmoHookLoc = 0x6979B0;
DWORD sustainAmmoHookRet = 0x6979B7;

dvar_t** player_sustainAmmo = (dvar_t**)0xBCD250;

void __declspec(naked) SustainAmmoHookFunc()
{
    if ((*player_sustainAmmo)->current.boolean)
    {
        __asm retn
    }

    __asm
    {
        push    ebx
        push    esi
        push    edi
        mov     edi, [esp + 14h]

        jmp sustainAmmoHookRet
    }
}

void PatchT5_SP152()
{
	Com_Error = (Com_Error_t)0x651D90;

	// disable SetWindowsHookExA caller

	// disable bandwidth testing for ZM hosting
	*(WORD*)0x693842 = 0x9090; // (?!)

	//*(BYTE*)0x6DBCF0 = 0xC3;

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

	// console toggling
	toggleConsoleHook1.initialize(toggleConsoleHook1Loc, Con_ToggleConsole);
	toggleConsoleHook1.installHook();

	toggleConsoleHook2.initialize(toggleConsoleHook2Loc, Con_ToggleConsole);
	toggleConsoleHook2.installHook();

	// player_sustainAmmo re-adding
	sustainAmmoHook.initialize(sustainAmmoHookLoc, SustainAmmoHookFunc);
	sustainAmmoHook.installHook();

	// set port to 28960
	*(DWORD*)0x52E1D5 = 28960;

	// disable UPnP
	//*(WORD*)0x973C6B = 0x9090;

	// remove the windows hook setting function
	//*(BYTE*)0x8683D0 = 0xC3;

	// FOV protection flags
	*(DWORD*)0x4A3921 = 1;

	// con stuff
	*(WORD*)0x79E6E5 = 0x9090; // dvar set 1

	*(BYTE*)0x5119B0 = 0xC3;   // dvar reset flag
	*(WORD*)0x4A87EB = 0x9090; // dvar exec
	*(WORD*)0x829CBE = 0x9090; // cmd  exec
	*(WORD*)0x5F5D4E = 0x9090; // dvar enum
	*(WORD*)0x610A94 = 0x9090; // cmd  enum

	//bdLogHook.initialize("aaaaa", 5, (PBYTE)bdLogHookLoc);
	//bdLogHook.installHook(BDLogHookStub, true, true);

	//rawInterceptorHook.initialize("a", 5, (PBYTE)rawInterceptorHookLoc);
	//rawInterceptorHook.installHook(RawInterceptorHookStub, true, false);

	// force playlists to not be used (playlist_enabled non-ranked license)
	//*(BYTE*)0x88C3BA = 0xEB;
	//*(BYTE*)0x87C4D5 = 0xEB; // map_rotate

	// classic fs_game-for-script patch
	/**(WORD*)0x8E37B7 = 0x9090;
	*(WORD*)0x8E37BF = 0x9090;

	// also enable voting if ranked server licensetype
	memset((void*)0x50E5C8, 0x90, 6);
	*(BYTE*)0x58A186 = 0xEB; // do not set g_allowVote to 0 if ranked*/

	// disable democlient
	//*(BYTE*)0x4BBE30 = 0xC3;

	// enable console (normally this check is !monkeytoy || blah; now it's only 'blah'
	*(BYTE*)0x58761C = 0xEB;

	/*// disable IWD validation
	*(BYTE*)0x8B3052 = 0xEB;

	// allow non-iw_ IWDs in main/
	*(BYTE*)0x8B2F5C = 0xEB;

	// matchmaking_mininfo default to 0 (old behavior)
	*(BYTE*)0x5B09EB = 0;*/

	// small fix for infinite waiting for other players (?! -- NTA)
	//*(BYTE*)0x5026BC = 0x03;

	*(DWORD*)0x9A3308 = (DWORD)ShellExecuteA_hook;
}

void BDLogHookFunc(int a1, char* type, char* channel, char* file, char* func, int line, char* message, ...);

void dw_patch_sp_152(dw_entry* entryPoints)
{
	call(0x92EB80, BDLogHookFunc, PATCH_JUMP);

	*(DWORD*)0x9A3440 = (DWORD)entryPoints->recv;
	*(DWORD*)0x9A3424 = (DWORD)entryPoints->connect;
	*(DWORD*)0x9A3438 = (DWORD)entryPoints->send;
	*(DWORD*)0x9A3450 = (DWORD)entryPoints->recvfrom;
	*(DWORD*)0x9A3464 = (DWORD)entryPoints->closesocket;
	*(DWORD*)0x9A3454 = (DWORD)entryPoints->select;
	*(DWORD*)0x9A3470 = (DWORD)entryPoints->gethostbyname;
	*(DWORD*)0x9A3460 = (DWORD)entryPoints->sendto;
}
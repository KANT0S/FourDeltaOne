// ==========================================================
// alterOps project
// 
// Component: t5cli
// Sub-component: clientdll
// Purpose: Patches.
//
// Initial author: NTAuthority
// Started: 2011-08-21 (finally...)
// ==========================================================

#include "stdinc.h"
#include <ws2tcpip.h>
#include <MMSystem.h>

hostent* WINAPI custom_gethostbyname(const char* name);

#include "buildnumber.h"

void SetPT5Version()
{
        const char* versionTag = "secretSchemes SP (" BUILDNUMBER_STR ")";
        void* version = *(void**)0x243FD34;
        DWORD setString = 0x44A2A0;

        __asm
        {
                push versionTag
                push version
                call setString
                add esp, 8
        }
}

static dvar_t* r_noborder;


// Cmd commands
typedef int (__cdecl* Cmd_CommandExists_t)(const char* name);
Cmd_CommandExists_t CmdCommandExists = (Cmd_CommandExists_t)0x479DD0;

typedef dvar_t* (__cdecl* Dvar_RegisterBool_t)(const char* name, bool default, int flags, const char* description);
static Dvar_RegisterBool_t Dvar_RegisterBool = (Dvar_RegisterBool_t)0x45BB20;

// this point is to hook in Sys_Init
static MMRESULT __stdcall customTimeBeginPeriod(UINT uPeriod)
{
        //Cmd_AddCommand("ddlDump", pT5_DDLDump_f, &ddlDumpC);
        r_noborder = Dvar_RegisterBool("r_noborder", false, DVAR_ARCHIVE, "Do not use a border in windowed mode");
        SetPT5Version();
        return timeBeginPeriod(uPeriod);
}

static float drawVersionX = 10.0f;
static float drawVersionY = 450.0f;

static StompHook windowedWindowStyleHook;
static DWORD windowedWindowStyleHookLoc = 0x6B7A73;

static void __declspec(naked) WindowedWindowStyleHookStub()
{
        if (r_noborder->current.boolean)
        {
                __asm mov ebp, WS_VISIBLE | WS_POPUP
        }
        else
        {
                __asm mov ebp, WS_VISIBLE | WS_SYSMENU | WS_CAPTION;
        }

        __asm retn
}

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

void PatchSP()
{
        // cg_fov unprotect
        *(DWORD*)0x4A3921 = DVAR_ARCHIVE;

        // small fix for infinite waiting for other players
        *(BYTE*)0x5026BC = 0x0;
        
        // function changes
        Com_Error = (Com_Error_t)0x651D90;
		Com_Printf = (Com_Printf_t)0x43BF30;

        // disable bandwidth testing / better match finding
        *(WORD*)0x693842 = 0x9090;
		
		// enable console (normally this check is !monkeytoy || blah; now it's only 'blah'
        *(BYTE*)0x58761C = 0xEB;

        // console toggling
        toggleConsoleHook1.initialize("", (PBYTE)toggleConsoleHook1Loc);
        toggleConsoleHook1.installHook((void(*)())Con_ToggleConsole, false);

        toggleConsoleHook2.initialize("", (PBYTE)toggleConsoleHook2Loc);
        toggleConsoleHook2.installHook((void(*)())Con_ToggleConsole, false);

        // player_sustainAmmo re-adding
        sustainAmmoHook.initialize("", 7, (PBYTE)sustainAmmoHookLoc);
        sustainAmmoHook.installHook(SustainAmmoHookFunc, true, false);
        
        // cg_drawVersion
        *(BYTE*)0x4A3D46 = 1; // default
        *(BYTE*)0x4A3D44 = 0x50; // flags
		
        // y 450, x 10
        *(DWORD*)0x4A3D7A = (DWORD)&drawVersionX;
        *(DWORD*)0x4A3DAF = (DWORD)&drawVersionY;

        *(DWORD*)0x9A33FC = (DWORD)customTimeBeginPeriod;

        // set port to 28960
        *(DWORD*)0x52E1D5 = 28960;

        // disable UPnP
        //*(WORD*)0x973C6B = 0x9090;

        // remove the windows hook setting function
        //*(BYTE*)0x8683D0 = 0xC3;

        // con stuff
        *(WORD*)0x79E6E5 = 0x9090; // dvar set 1

        *(BYTE*)0x5119B0 = 0xC3;   // dvar reset flag
        *(WORD*)0x4A87EB = 0x9090; // dvar exec
        *(WORD*)0x829CBE = 0x9090; // cmd  exec
        *(WORD*)0x5F5D4E = 0x9090; // dvar enum
        *(WORD*)0x610A94 = 0x9090; // cmd  enum

        windowedWindowStyleHook.initialize("aaaaa", 5, (PBYTE)windowedWindowStyleHookLoc);
        windowedWindowStyleHook.installHook(WindowedWindowStyleHookStub, false, false);

        // gethostbyname
        *(DWORD*)0x9A3470 = (DWORD)custom_gethostbyname;

        // small fix for infinite waiting for other players
        *(BYTE*)0x5026BC = 0x03;
}
// ==========================================================
// secretSchemes
// 
// Component: steam_api
// Sub-component: clientdll
// Purpose: T5 patches: r_noborder for render window
//          without borders (and proper height for
//          text scaling)
//
// Initial author: Terminator / NTAuthority
// Started: 2012-05-08
// ==========================================================

#include "StdInc.h"

typedef dvar_t* (__cdecl* Dvar_RegisterBool_t)(const char* name, bool default, int flags, const char* description);
static Dvar_RegisterBool_t Dvar_RegisterBool = (Dvar_RegisterBool_t)0x41AEA0;

static StompHook windowedWindowStyleHook;
static DWORD windowedWindowStyleHookLoc = 0x6DD893;

dvar_t* r_noborder;

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


void PatchT5_NoBorder()
{
        r_noborder = Dvar_RegisterBool("r_noborder", false, DVAR_ARCHIVE, "Do not use a border in windowed mode");

        windowedWindowStyleHook.initialize("aaaaa", 5, (PBYTE)windowedWindowStyleHookLoc);
        windowedWindowStyleHook.installHook(WindowedWindowStyleHookStub, false, false);
}
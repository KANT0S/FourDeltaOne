// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: IW4M 'new client' functionality; hooks.
//
// Initial author: NTAuthority
// Started: 2013-01-22
// ==========================================================

#include "StdInc.h"

#ifdef WE_DO_WANT_NUI
#include "NUI.h"

CallHook initNUIHook; // somewhere in R_BeginRegistration or so
DWORD initNUIHookLoc = 0x506C82;

void __declspec(naked) InitNUIHookStub()
{
	__asm
	{
		call initNUIHook.pOriginal
		call Inverse_Init
		
		jmp NUI_Init
	}
}

StompHook cinematicUpdateFrameHook;
DWORD cinematicUpdateFrameHookLoc = 0x51CB42;

CallHook scrUpdateFrameHook;
DWORD scrUpdateFrameHookLoc = 0x5ACB99;

void __declspec(naked) DrawNUIHookStub()
{
	__asm
	{
		call NUI_Draw
		jmp scrUpdateFrameHook.pOriginal
	}
}

StompHook uiInitHook;
DWORD uiInitHookLoc = 0x4A5A14;

void UI_InitHookFunc()
{
	// don't change the gametype list anymore
	memset((void*)0x4A588A, 0x90, 10);
}

typedef menuDef_t* (__cdecl * Menus_FindByName_t)(void* uiInfoArray, const char* name);
Menus_FindByName_t Menus_FindByName = (Menus_FindByName_t)0x487240;

StompHook menusOpenHook;
DWORD menusOpenHookLoc = 0x4B72F0;

bool Menus_OpenHookFunc(void* uiInfo, menuDef_t* menu)
{
	NUI_OpenPage(menu->window.name);

	if (!strcmp(menu->window.name, "main"))
	{
		NUI_OpenPage("iw4_frontend");
	}

	if (strcmp(menu->window.name, "connect") && strcmp(menu->window.name, "pc_join_unranked") && strcmp(menu->window.name, "team_marinesopfor"))
	{
		return false;
	}

	if (!strcmp(menu->window.name, "team_marinesopfor"))
	{
		if (strcmp(GetStringConvar("mapname"), "ui_viewer_mp"))
		{
			return false;
		}
	}

	//return (strcmp(menu->window.name, "main")) ? true : false;
	return true;
}

void __declspec(naked) Menus_OpenHookStub()
{
	__asm
	{
		mov eax, [esp + 4h]
		mov ecx, [esp + 8h]

		push ecx
		push eax

		call Menus_OpenHookFunc
		add esp, 8

		test al, al
		jz doReturn

		retn

doReturn:
		push ecx
		mov [esp], 0

		push 4B72F8h
		retn
	}
}

static bool mainOpened;

StompHook menusOpenByNameHook;
DWORD menusOpenByNameHookLoc = 0x4CCE60;

bool Menus_OpenByNameHookFunc(void* uiInfo, const char* name)
{
	if (Menus_FindByName((void*)0x62E2858, name))
	{
		if (strcmp(name, "main") && strcmp(name, "connect") && strcmp(name, "pc_join_unranked") && strcmp(name, "team_marinesopfor"))
		{
			return false;
		}

		if (!strcmp(name, "team_marinesopfor"))
		{
			if (strcmp(GetStringConvar("mapname"), "ui_viewer_mp"))
			{
				return false;
			}
		}
	}

	if (!strcmp(name, "main"))
	{
		NUI_OpenPage("iw4_frontend");

		mainOpened = true;
	}
	else
	{
		NUI_OpenPage(name);
	}

	//return (strcmp(name, "main")) ? true : false;
	return true;
}

void __declspec(naked) Menus_OpenByNameHookStub()
{
	__asm
	{
		mov eax, [esp + 4h]
		mov ecx, [esp + 8h]

		push ecx
		push eax

		call Menus_OpenByNameHookFunc
		add esp, 8

		test al, al
		jz doReturn

		retn

doReturn:
		push esi
		mov esi, [esp + 8]

		push 4CCE65h
		retn
	}
}

StompHook menusCloseAllHook;
DWORD menusCloseAllHookLoc = 0x4BA5B0;

void Menus_CloseAllHookFunc(void* uiInfo)
{
	mainOpened = false;

	NUI_CloseAllPages();
}

void __declspec(naked) Menus_CloseAllHookStub()
{
	__asm
	{
		call Menus_CloseAllHookFunc

		push esi
		mov esi, 62E2858h
		
		push 4BA5B5h
		retn
	}
}

StompHook uiKeyEventHook;
DWORD uiKeyEventHookLoc = 0x4970F0;

void __declspec(naked) UI_KeyEventHookStub()
{
	__asm
	{
		mov eax, [esp + 8h]
		mov ecx, [esp + 0Ch]

		push ecx
		push eax

		call NUI_KeyEvent

		add esp, 8h

		push 62E2858h

		push 4970F5h
		retn
	}
}

CallHook mainSetActiveMenuHook;
DWORD mainSetActiveMenuHookLoc = 0x47E01F;

void __declspec(naked) MainSetActiveMenuHookStub()
{
	if (strcmp(GetStringConvar("mapname"), "ui_viewer_mp") && !mainOpened)
	{
		__asm jmp mainSetActiveMenuHook.pOriginal
	}

	__asm retn
}

// additional FS threads
bool _workaroundThreadSync = false;

int FS_GetHandleOffset(int thread)
{
	switch (thread)
	{
	case 0:
		if (_workaroundThreadSync)
		{
			return 21;
		}

		return 1;
	case 1:
		return 49;
	case 2:
		return 61;
	case 3:
		return 62;
	case 4:
		return 63;
	case 5: // custom
		return 31;
	default:
		return 62;
	}
}

int FS_GetHandleCount(int thread)
{
	switch (thread)
	{
	case 0:
		return 30;
	case 1:
		return 12;
	case 5:
		return 18;
	default:
		return 1;
	}
}

void __declspec(naked) FS_GetHandleHookStub()
{
	__asm
	{
		push ebx
		push ebp
		mov ebp, [esp + 8 + 4]

		push ebp
		call FS_GetHandleOffset
		mov ebx, eax

		call FS_GetHandleCount
		pop ebp

		push 439E2Eh
		retn
	}
}

void PatchMW2_NUI()
{
	initNUIHook.initialize(initNUIHookLoc, InitNUIHookStub);
	initNUIHook.installHook();

	cinematicUpdateFrameHook.initialize(cinematicUpdateFrameHookLoc, NUI_UpdateFrame);
	cinematicUpdateFrameHook.installHook();

	scrUpdateFrameHook.initialize(scrUpdateFrameHookLoc, DrawNUIHookStub);
	scrUpdateFrameHook.installHook();

	uiInitHook.initialize(uiInitHookLoc, UI_InitHookFunc);
	uiInitHook.installHook();

	menusOpenHook.initialize(menusOpenHookLoc, Menus_OpenHookStub);
	menusOpenHook.installHook();

	menusCloseAllHook.initialize(menusCloseAllHookLoc, Menus_CloseAllHookStub);
	menusCloseAllHook.installHook();

	menusOpenByNameHook.initialize(menusOpenByNameHookLoc, Menus_OpenByNameHookStub);
	menusOpenByNameHook.installHook();

	uiKeyEventHook.initialize(uiKeyEventHookLoc, UI_KeyEventHookStub);
	uiKeyEventHook.installHook();

	mainSetActiveMenuHook.initialize(mainSetActiveMenuHookLoc, MainSetActiveMenuHookStub);
	mainSetActiveMenuHook.installHook();

	call(0x439DF0, FS_GetHandleHookStub, PATCH_JUMP);

	// gametype invalidation somewhere odd
	*(BYTE*)0x4FD8C0 = 0xC3;

	// nop loadgameinfo
	memset((void*)0x45ED76, 0x90, 10);

	// don't unset catcher 16 when no menu is focused
	*(BYTE*)0x48E5EA = 0xEB;

	// same, but on mouse click
	*(BYTE*)0x4971CD = 0xEB;

	// breakpoint exitprocess
	if (IsDebuggerPresent())
	{
		DWORD oldProtect;
		DWORD exitProcessLoc = (DWORD)GetProcAddress(GetModuleHandle("kernel32.dll"), "TerminateProcess");
		VirtualProtect((LPVOID)exitProcessLoc, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
		*(DWORD*)exitProcessLoc = 0xcccccccc;
	}

	// D3DUSAGE_WRITEONLY preserve in Image_Create2DTexture_PC
	//*(DWORD*)0x51F180 |= 8;
}
#else
CallHook initNUIHook; // somewhere in R_BeginRegistration or so
DWORD initNUIHookLoc = 0x506C82;

void __declspec(naked) InitNUIHookStub()
{
	__asm
	{
		call initNUIHook.pOriginal
		
		jmp Inverse_Init
	}
}

void PatchMW2_NUI()
{
	initNUIHook.initialize(initNUIHookLoc, InitNUIHookStub);
	initNUIHook.installHook();
}

void NUI_Frame()
{

}
#endif
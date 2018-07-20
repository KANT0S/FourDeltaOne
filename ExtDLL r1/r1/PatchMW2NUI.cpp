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

StompHook menusOpenHook;
DWORD menusOpenHookLoc = 0x4B72F0;

bool Menus_OpenHookFunc(void* uiInfo, menuDef_t* menu)
{
	NUI_OpenPage(menu->window.name);

	return (strcmp(menu->window.name, "main")) ? true : false;
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

StompHook menusOpenByNameHook;
DWORD menusOpenByNameHookLoc = 0x4CCE60;

bool Menus_OpenByNameHookFunc(void* uiInfo, const char* name)
{
	NUI_OpenPage(name);

	return (strcmp(name, "main")) ? true : false;
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
	NUI_CloseAllPages();
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

	menusCloseAllHook.initialize(menusCloseAllHookLoc, Menus_CloseAllHookFunc);
	menusCloseAllHook.installHook();

	menusOpenByNameHook.initialize(menusOpenByNameHookLoc, Menus_OpenByNameHookStub);
	menusOpenByNameHook.installHook();

	uiKeyEventHook.initialize(uiKeyEventHookLoc, UI_KeyEventHookStub);
	uiKeyEventHook.installHook();

	// gametype invalidation somewhere odd
	*(BYTE*)0x4FD8C0 = 0xC3;

	// nop loadgameinfo
	memset((void*)0x45ED76, 0x90, 10);

	// don't unset catcher 16 when no menu is focused
	*(BYTE*)0x48E5EA = 0xEB;

	// same, but on mouse click
	*(BYTE*)0x4971CD = 0xEB;
}
#else
void PatchMW2_NUI()
{
	
}

void NUI_Frame()
{

}
#endif
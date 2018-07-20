#include "StdInc.h"

DWORD gSayPreHookLoc = 0x47FFB9;
CallHook gSayPreHook;

DWORD gSayPostHook1Loc = 0x480050;
CallHook gSayPostHook1;

DWORD gSayPostHook2Loc = 0x48008B;
CallHook gSayPostHook2;

bool gsShouldSend = true;
char* gspText;
char* gspName;
void* gspEnt;
int gspIsTeam;

void __declspec(naked) GSayPreHookFunc()
{
	__asm mov eax, [esp + 0FCh + 10h]
	__asm mov gspText, eax

	__asm mov eax, [esp + 4h] // as name is arg to this function, that should work too
	__asm mov gspName, eax

	__asm mov eax, [esp + 0FCh + 4h]
	__asm mov gspEnt, eax

	__asm mov eax, [esp + 0FCh + 0Ch] // team (0 = global, 1 = teambased)
	__asm mov gspIsTeam, eax

	gsShouldSend = true;

	if(gspText[0] == 21)
	{
		gspText += 1;
	}

	if (gspText[0] == '/')
	{
		gsShouldSend = false;
	}

	if (!Scriptability_OnSay((gentity_t*)gspEnt - g_entities, gspName, &gspText, gspIsTeam))
	{
		gsShouldSend = false;
	}

	// move gspText back with new values from Mono
	__asm mov eax, gspText
	__asm mov [esp + 0FCh + 10h], eax

	// safety check - material pointer stuff and such
	for (const char* c = gspText; *c != '\0'; c++)
	{
		if (*c >= '\0' && *c < ' ')
		{
			gsShouldSend = false;
			break;
		}
	}

	if (gsShouldSend)
	{
		Com_Printf(15, "%s: %s\n", gspName, gspText);
	}

	__asm jmp gSayPreHook.pOriginal
}

// these two need to pushad/popad as otherwise some registers the function uses as param are screwed up
void __declspec(naked) GSayPostHook1Func()
{
	__asm pushad

	if (!gsShouldSend)
	{
		__asm popad
		__asm retn
	}

	__asm popad

	__asm jmp gSayPostHook1.pOriginal
}

void __declspec(naked) GSayPostHook2Func()
{
	__asm pushad

	if (!gsShouldSend)
	{
		__asm popad
		__asm retn
	}

	__asm popad

	__asm jmp gSayPostHook2.pOriginal
}

void PatchIW5DS_DediSay()
{
	gSayPreHook.initialize("", (PBYTE)gSayPreHookLoc);
	gSayPreHook.installHook(GSayPreHookFunc, false);

	gSayPostHook1.initialize("", (PBYTE)gSayPostHook1Loc);
	gSayPostHook1.installHook(GSayPostHook1Func, false);

	gSayPostHook2.initialize("", (PBYTE)gSayPostHook2Loc);
	gSayPostHook2.installHook(GSayPostHook2Func, false);
}
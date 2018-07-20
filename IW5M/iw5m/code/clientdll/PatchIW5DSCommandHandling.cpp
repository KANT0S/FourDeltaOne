// ==========================================================
// IW5M project
// 
// Component: client
// Sub-component: clientdll
// Purpose: Handles command hooking for Scriptablity
//
// Initial author: NTAuthority 
// Started: 2012-10-22
// ==========================================================

#include "StdInc.h"

DWORD Com_ExecuteSingleCommandHookLoc = 0x4BFBC5;
DWORD Com_ExecuteSingleCommandHookLocCont = 0x4BFBCC;
DWORD Com_ExecuteSingleCommandHookLocCanc = 0x4BFC0B;
StompHook Com_ExecuteSingleCommandHook;

bool Com_ExecuteSingleCommandTest()
{
	return Scriptability_ServerCommand(Cmd_Argv(0));
}

void __declspec(naked) Com_ExecuteSingleCommandHookFunc()
{
	if (Com_ExecuteSingleCommandTest())
	{
		__asm
		{
			push edi
			jmp Com_ExecuteSingleCommandHookLocCanc
		}
	}
	__asm
	{
		cmp dword ptr ds:[1B2BB58h], 0
		jmp Com_ExecuteSingleCommandHookLocCont
	}
}

StompHook ClientCommandHook;
DWORD ClientCommandHookLoc = 0x48023A;
int* clientNum;

void ClientCommandHookTest()
{
	if(!Scriptability_ClientCommand(Cmd_Argv_sv(0), *clientNum))
	{
		const char* nocommand = va("%c \"GAME_UNKNOWNCLIENTCOMMAND\x15%s", 101, Cmd_Argv_sv(0));
		SV_GameSendServerCommand(*clientNum, 0, nocommand);
	}
}

void __declspec(naked) ClientCommandHookFunc()
{
	__asm
	{
		jnz redirect
		push esi
		mov eax,47FBD0h
		call [eax]
        add esp, 4
		jmp finish
redirect:
		mov clientNum, esi
		call ClientCommandHookTest
finish:
		pop edi
        pop esi
        add esp, 400h
        retn
	}
}
void PatchIW5DS_CommandHandling()
{
	Com_ExecuteSingleCommandHook.initialize("", 5, (PBYTE)Com_ExecuteSingleCommandHookLoc);
	Com_ExecuteSingleCommandHook.installHook(Com_ExecuteSingleCommandHookFunc, true, false);

	ClientCommandHook.initialize("", 5, (PBYTE)ClientCommandHookLoc);
	ClientCommandHook.installHook(ClientCommandHookFunc, true, false);
}
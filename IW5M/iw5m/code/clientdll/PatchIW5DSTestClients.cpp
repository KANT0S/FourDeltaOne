// ==========================================================
// IW5M project
// 
// Component: client
// Sub-component: clientdll
// Purpose: SV_AddTestClient implementation for IW5
//
// Initial author: NTAuthority
// Started: 2012-09-06
// ==========================================================

#include "StdInc.h"

// 'static' constant functions
typedef int (__cdecl * func_int_t)();
static func_int_t _getProtocol = (func_int_t)0x51B440;
static func_int_t _getStatMajor = (func_int_t)0x4C1E50;
static func_int_t _getStatMinor = (func_int_t)0x4C1E70;
static func_int_t _getDataChecksum = (func_int_t)0x4199F0;

// SV_Cmd functions
typedef void (__cdecl * SV_Cmd_TokenizeString_t)(const char* string);
typedef void (__cdecl * SV_Cmd_EndTokenizedString_t)();

SV_Cmd_TokenizeString_t SV_Cmd_TokenizeString = (SV_Cmd_TokenizeString_t)0x4BF6E0;
SV_Cmd_EndTokenizedString_t SV_Cmd_EndTokenizedString = (SV_Cmd_EndTokenizedString_t)0x4BF710;

// connection handling functions
typedef void (__cdecl * SV_DirectConnect_t)(netadr_t adr);
SV_DirectConnect_t SV_DirectConnect = (SV_DirectConnect_t)0x4E8200;

typedef void (__cdecl * SV_ClientEnterWorld_t)(void* client, void* usercmd);
SV_ClientEnterWorld_t SV_ClientEnterWorld = (SV_ClientEnterWorld_t)0x4E6E10;

void PatchIW5DS_TestClients();

int SV_AddTestClient()
{
	// make sure stuff is patched
	PatchIW5DS_TestClients();

	// initialize some local relevant variables
	int maxClients = *(int*)0x49EB68C;
	char* svs_clients = (char*)0x49EB690;
	size_t clientSize = 493192;

	// check if there are any free slots
	int i = 0;

	for (char* client = svs_clients; i < maxClients; client += clientSize, i++)
	{
		if (!*(int*)client)
		{
			break;
		}
	}

	if (i == maxClients)
	{
		return 0;
	}

	// prepare a connection string
	char connectString[1024];
	static int botport = 0;

	_snprintf(connectString, sizeof(connectString), "connect bot%d \"\\cg_predictItems\\1\\cl_anonymous\\0\\color\\4\\head\\default\\model\\multi\\snaps\\20\\rate\\5000\\name\\bot%d\\protocol\\%d\\checksum\\%d\\statver\\%d %u\\qport\\%d\"",
			  botport,
			  botport,
			  _getProtocol(),
			  _getDataChecksum(),
			  _getStatMajor(),
			  _getStatMinor(),
			  botport + 1);

	// handle client connection
	netadr_t botadr;
	memset(&botadr, 0, sizeof(botadr));
	botadr.port = botport;

	SV_Cmd_TokenizeString(connectString);
	SV_DirectConnect(botadr);
	SV_Cmd_EndTokenizedString();

	botport++;

	// get the bot's client_t* and clientnum; also return if the bot didn't connect
	char* botclient = 0;
	i = 0;

	for (botclient = svs_clients; i < maxClients; botclient += clientSize, i++)
	{
		if (*(int*)botclient)
		{
			netadr_t clientadr = *(netadr_t*)(botclient + 40);

			if (clientadr.type == botadr.type && clientadr.port == botadr.port)
			{
				// found him. or her. or it. or whatever.
				break;
			}
		}
	}

	if (i == maxClients)
	{
		return 0;
	}

	// set the 'bot' flag (actually steamid lower dword, but as we can't really expand this structomite)
	*(int*)(botclient + 283320) = -1;

	// set stat flags to 'already set' (may need to be done in a hook, not sure though)
	*(short*)(botclient + 283314) = 0x3FF;
	memset(botclient + 269489, 0, 12284);
	memset(botclient + 281777, 0, 0x600);

	// SV_SendClientGamestate, a usercall
	__asm
	{
		mov eax, 4E6CF0h
		push esi
		mov esi, botclient
		call eax
		pop esi
	}

	// SV_ClientEnterWorld
	char usercmd[44];
	memset(usercmd, 0, sizeof(usercmd));

	SV_ClientEnterWorld(botclient, usercmd);

	// and return the clientnum (which is also an entref, even, yay)
	return i;
}

extern "C" __declspec(dllexport) int GI_AddTestClient()
{
	return SV_AddTestClient();
}

void SendOOBHookFunc(char* string)
{
	Trace("OOB", (char*)va("%s\n", string));
}

void __declspec(naked) SendOOBHook()
{
	__asm
	{
		mov eax, [esp + 20h]
		push eax
		call SendOOBHookFunc
		add esp, 4h

		sub esp, 8h
		push ebx
		push esi
		push edi

		push 4D2356h
		retn
	}
}

bool SV_CheckTimeoutsFunc(char* clientPart)
{
	char* client = (clientPart - 136292);
	return (*(DWORD*)(clientPart - 136252) != 2 && (*(DWORD*)client == 1 || *(int*)(client + 283320) != -1));
}

void __declspec(naked) SV_CheckTimeoutsHook()
{
	__asm
	{
		push esi
		call SV_CheckTimeoutsFunc
		add esp, 4h
		test al, al
		
		push 4EC37Ah
		retn
	}
}

bool Steam_CheckSVAuthFunc(char* clientPart)
{
	char* client = (clientPart - 283317);

	return (*(int*)client >= 5 && *(int*)(client + 283320) != -1);
}

void __declspec(naked) Steam_CheckSVAuthHook()
{
	__asm
	{
		push esi
		call Steam_CheckSVAuthFunc
		add esp, 4h
		test al, al
		jnz continueSteamCheck

		push 5830EDh
		retn

continueSteamCheck:
		push 58305Dh
		retn
	}
}

void __declspec(naked) ResetReliableCmdHook()
{
	__asm
	{
		cmp [esi + 452B8h], 0FFFFFFFFh
		jne doReturn

		mov eax, [esi + 20E70h]
		mov [esi + 20E74h], eax

doReturn:
		// return to Com_Milliseconds
		push 5279D0h
		retn
	}
}

void PatchIW5DS_TestClients()
{
	static bool initialized = false;

	if (initialized)
	{
		return;
	}

	initialized = true;

	//call(0x4D2350, SendOOBHook, PATCH_JUMP);
	call(0x4EC373, SV_CheckTimeoutsHook, PATCH_JUMP);
	call(0x583050, Steam_CheckSVAuthHook, PATCH_JUMP);
	call(0x4EF8CE, ResetReliableCmdHook, PATCH_CALL);
}
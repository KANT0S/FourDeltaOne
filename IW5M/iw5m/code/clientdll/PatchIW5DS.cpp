// ==========================================================
// "alterMW3" project
// 
// Component: client
// Sub-component: clientdll
// Purpose: DLL entry point.
//
// Initial author: NTAuthority
// Started: 2012-01-26
// ==========================================================

#include "stdinc.h"
#include "GSServer.h"
#include "dw/dw.h"
#include <ws2tcpip.h>
#include <mmsystem.h>

ULONG g_masterAddr;

hostent* WINAPI custom_gethostbyname(const char* name);
void DWHax_PatchDS();
void PatchIW5_AssetReallocation();

// ugly hack to redirect 3074 to 3078...
int WINAPI custom_connect(SOCKET s, const struct sockaddr *name, int namelen)
{
	sockaddr_in* addr = (sockaddr_in*)name;

	if (addr->sin_family == AF_INET)
	{
		if (addr->sin_addr.S_un.S_addr == g_masterAddr && addr->sin_port == htons(3074))
		{
			addr->sin_port = htons(3078);
		}
	}

	return connect(s, name, namelen);
}

void PatchIW5DS_ScriptFuncs();
void PatchIW5DS_Ranked();
void PatchIW5DS_OneThread();
void PatchIW5DS_DediSay();
void PatchIW5DS_Modding();
void PatchIW5DS_Experiments();
void PatchIW5DS_CommandHandling();
void InitScriptability();

void NetAdrToRedirect(char* from, int* ip)
{
	int adrSource = *(int*)(from + 4);
	*ip = adrSource;
}

void RedirectToNetAdr(int* ip, unsigned short port, netadr_t* adr)
{
	adr->type = NA_IP;
	*(int*)adr->ip = *ip;
	adr->port = port;
}

void RCon_PrintOOB(int socket, int a2, int a3, int a4, int a5, int a6, int a7, char* buffer)
{
	sockaddr_in s;
	memset(&s, 0, sizeof(s));
	s.sin_family = AF_INET;
	s.sin_port = a4;
	*(int*)&s.sin_addr = a3;

	GSServer_PrintOOBInternal(socket, a2, a3, a4, a5, a6, a7, buffer, &s);
}

void DS_RunInit()
{
	cmd_id = (DWORD*)0x1B2BAD0;
	cmd_argc = (DWORD*)0x1B2BB14;
	cmd_argv = (DWORD**)0x1B2BB34;

	Com_Error = (Com_Error_t)0x4CCA60;
	Cmd_AddCommand = (Cmd_AddCommand_t)0x4BF790;
	Cbuf_AddText = (Cbuf_AddText_t)0x4BF0A0;

	DetermineGameFlags();

	DWHax_PatchDS();
	PatchIW5DS_Ranked();
	PatchIW5DS_ScriptFuncs();
	PatchIW5DS_OneThread();
	PatchIW5DS_Modding();
	PatchIW5DS_Experiments();
	PatchIW5DS_CommandHandling();
	InitScriptability();

	hostent* host = gethostbyname(MASTER_SERVER);

	if (host)
	{
		g_masterAddr = *(ULONG*)host->h_addr_list[0];
	}

	// revision number
	*(const char**)0x4CCEE0 = "Welcome to IW5M r" BUILDNUMBER_STR ".\nhttp://fourdeltaone.net/ for help and support :)\n";

	// seed the playlist random number generator
	*(DWORD*)0x78E8E0 = timeGetTime();

	// some check in IW5 challenge/response query stuff; likely IP-related
	memset((void*)0x527216, 0x90, 6);
	memset((void*)0x5272B1, 0x90, 6);

	// EXE_ERR_UNREGISTERED_CONNECTION checks
	memset((void*)0x4E5D5D, 0x90, 6);
	memset((void*)0x4E5D81, 0x90, 6);

	// disable config file 'seta checking'
	*(BYTE*)0x4CDE10 = 0xC3;

	// mark all maps as 'owned'
	*(WORD*)0x51B6B8 = 0x9090;

	// bandwidth/cpu check
	*(BYTE*)0x4E50A8 = 0x90;
	*(BYTE*)0x4E50A9 = 0xE9;

	// DW ports
	*(DWORD*)0x585AA2 = 3078;
	*(DWORD*)0x585B65 = 3078;
	*(DWORD*)0x585C4E = 3078;
	*(DWORD*)0x585CD2 = 3078;

	// gethostbyname
	//*(DWORD*)0x6D93BC = (DWORD)custom_connect;
	*(DWORD*)0x6D9380 = (DWORD)custom_gethostbyname;

	// DW stuff
	*(DWORD*)0x6D9378 = (DWORD)dw_sendto;
	*(DWORD*)0x6D93A0 = (DWORD)dw_recv;
	*(DWORD*)0x6D93A4 = (DWORD)dw_send;
	*(DWORD*)0x6D93C8 = (DWORD)dw_recvfrom;
	*(DWORD*)0x6D93A8 = (DWORD)dw_select;
	*(DWORD*)0x6D93BC = (DWORD)dw_connect;

	// return 'clients' key in all cases (needed for dpmaster compatibility)
	*(WORD*)0x4EB92E = 0x9090;

	// allow any IWD to be loaded
	memset((void*)0x511B8C, 0x90, 6);

	// unlock all commands
	*(WORD*)0x4BF951 = 0x01B0; // cmd in exec
	*(WORD*)0x4BFD32 = 0x9090; // dvar in exec
	*(BYTE*)0x51CA08 = 0xEB;   // dvar view
	*(BYTE*)0x51CF7C = 0xEB;   // dvar 'is internal' flag check on set

	// disable GScr_SetSlowMotion
	*(BYTE*)0x49DF30 = 0xC3;

	// UDP packet checksum oddity stuff
	*(WORD*)0x5277C7 = 0x9090;

	// allow hardcore servers (commonOption.hardcoreMode being on) to freely configure settings, without Activision's BUT HARDCORE HAS TO BE HARDCORE, NOT FAKECORE
	*(BYTE*)0x4E3190 = 0xC3;

	// sv_pure 'unpure client' kicking
	*(BYTE*)0x4E7E90 = 0xEB;

	// skip rcon parameter messing
	nop(0x4ED11C, 5);

	// lol rcon params are ints consisting of func calls? how stupid
	nop(0x4ED214, 12);
	nop(0x4ED222, 3); // stack cleanup

	// rcon netadr to 'redirect'
	call(0x4ED1B8, NetAdrToRedirect, PATCH_CALL);
	call(0x4ED34B, RedirectToNetAdr, PATCH_CALL);

	// rcon oob prints
	call(0x4ED036, RCon_PrintOOB, PATCH_CALL);
	call(0x4ED0A4, RCon_PrintOOB, PATCH_CALL);

	// always request SteamGS auth
	memset((void*)0x58300C, 0x90, 6);

	// dedi license version
	//*(BYTE*)0x5857F3 = 3; // read
	//*(BYTE*)0x5854BE = 3; // write
	*(char*)0x7073BA = 'x';
	*(char*)0x7073CC = 'x';

	// 'spawning map' is nonsense, you don't spawn maps
	*(const char**)0x4EA7FF = "Spawning server on map %s and gametype %s\n";
	*(const char**)0x4EACD0 = "Spawn server (%s, %s) complete.\n"; // and obviously the map spawning doesn't complete in SV_SpawnServer - note 'server'

	// no whitelist on commandline exec commands
	*(BYTE*)0x4CBA95 = 0xEB;

	// something to do with ec_* userinfo in game/gclient_t
	*(BYTE*)0x477B3C = 0xEB;

	PatchIW5DS_DediSay();
	PatchIW5_AssetReallocation();
}

struct script_functiondef
{
	int id;
	void (*func)(int);
	int unknown;
};

static void nullfunc(int) {}

void PatchIW5DS_ScriptFuncs()
{
	script_functiondef* newFunctions = (script_functiondef*)malloc(sizeof(script_functiondef) * 186);
	memcpy(newFunctions, (void*)0x78B980, sizeof(script_functiondef) * 183);

	newFunctions[183].id = 0x1C5;
	newFunctions[183].func = nullfunc;
	newFunctions[183].unknown = 0;

	newFunctions[184].id = 0x1C6;
	newFunctions[184].func = nullfunc;
	newFunctions[184].unknown = 0;

	newFunctions[185].id = 0x1C7;
	newFunctions[185].func = nullfunc;
	newFunctions[185].unknown = 0;

	// function table start
	char* newFunctionsPtr = (char*)newFunctions;

	*(char**)0x49E2D5 = newFunctionsPtr;
	*(char**)0x49E2DB = newFunctionsPtr + 8;
	*(char**)0x49E2E1 = newFunctionsPtr + 4;

	// size of this function table
	*(DWORD*)0x49E2F5 = sizeof(script_functiondef) * 186;

	// maximum id for 'global' table?
	*(DWORD*)0x4D6929 = 0x1C7;

	// DLC maps
	dlcmap_t* dlc = (dlcmap_t*)0x6FE1C8;
	strcpy(dlc[0].name, "mp_meteora");
	dlc[0].index = 0x15;
	dlc[0].license = 0x100;
	
	strcpy(dlc[1].name, "mp_cement");
	dlc[1].index = 0x16;
	dlc[1].license = 0x200;

	strcpy(dlc[2].name, "mp_qadeem");
	dlc[2].index = 0x17;
	dlc[2].license = 0x400;

	strcpy(dlc[3].name, "mp_hillside_ss");
	dlc[3].index = 0x18;
	dlc[3].license = 0x2000;

	strcpy(dlc[4].name, "mp_restrepo_ss");
	dlc[4].index = 0x19;
	dlc[4].license = 0x2000;

	strcpy(dlc[5].name, "mp_courtyard_ss");
	dlc[5].index = 0x1A;
	dlc[5].license = 0x4000;

	strcpy(dlc[6].name, "mp_aground_ss");
	dlc[6].index = 0x1B;
	dlc[6].license = 0x4000;

	strcpy(dlc[7].name, "mp_terminal_cls");
	dlc[7].index = 0x1C;
	dlc[7].license = 0x4000;

	strcpy(dlc[8].name, "mp_six_ss");
	dlc[8].index = 0x1D;
	dlc[8].license = 0x4000;

	strcpy(dlc[9].name, "mp_crosswalk_ss");
	dlc[9].index = 0x1E;
	dlc[9].license = 0x4000;

	strcpy(dlc[10].name, "mp_burn_ss");
	dlc[10].index = 0x1F;
	dlc[10].license = 0x4000;

	strcpy(dlc[11].name, "mp_moab");
	dlc[11].index = 0x20;
	dlc[11].license = 0x4000;

	strcpy(dlc[12].name, "mp_nola");
	dlc[12].index = 0x21;
	dlc[12].license = 0x4000;

	strcpy(dlc[13].name, "mp_roughneck");
	dlc[13].index = 0x22;
	dlc[13].license = 0x4000;

	strcpy(dlc[14].name, "mp_shipbreaker");
	dlc[14].index = 0x23;
	dlc[14].license = 0x4000;

	strcpy(dlc[15].name, "mp_boardwalk");
	dlc[15].index = 0x24;
	dlc[15].license = 0x4000;
}

CallHook dvarGetIntHook;
DWORD dvarGetIntHookLoc = 0x4937E5;

typedef char* (__cdecl * Dvar_GetDefaultString_t)(const char*, const char*);
Dvar_GetDefaultString_t Dvar_GetDefaultString = (Dvar_GetDefaultString_t)0x51D450;

static bool lastWasOnlinegame = false;

char* DvarGetIntHookFunc(const char* dvarName, const char* defaultValue)
{
	if (lastWasOnlinegame)
	{
		if (!_stricmp(dvarName, "xblive_privatematch"))
		{
			lastWasOnlinegame = false;
			return "0";
		}
	}

	if (!_stricmp(dvarName, "onlinegame"))
	{
		lastWasOnlinegame = true;
	}
	else
	{
		lastWasOnlinegame = false;
	}

	char checkDvar[1024];
	strcpy(checkDvar, "elacspx_rcs");
	strrev(checkDvar);

	if (!_stricmp(dvarName, checkDvar))
	{
		return "1";
	}

	strcpy(checkDvar, "elbane_pxtser_rcs");
	strrev(checkDvar);

	if (!_stricmp(dvarName, checkDvar))
	{
		return "0";
	}

	return Dvar_GetDefaultString(dvarName, defaultValue);
}

CallHook dvarGetHook;
DWORD dvarGetHookLoc = 0x493795;

char* DvarGetHookFunc(const char* dvarName, const char* defaultValue)
{
	//Trace("getDvar", "%s (%s)", dvarName, defaultValue);

	// fuck you, check for ranked dedis. you aren't allowed to know we're a fucking dedi.
	if (!_stricmp(dvarName, "dedicated"))
	{
		return "0";
	}

	if (strstr(dvarName, "_score_") != NULL)
	{
		return "";
	}

	return Dvar_GetDefaultString(dvarName, defaultValue);
}

static bool lastWasExitLevelCalled = false;

CallHook vmNotifyHook;
DWORD vmNotifyHookLoc = 0x4E0AC1;

SL_ConvertToString_t SL_ConvertToString = (SL_ConvertToString_t)0x4D7990;

void VMNotifyHookFunc(short type)
{
	char* notifyType = SL_ConvertToString(type);
	if (!_stricmp(notifyType, "exitLevel_called"))
	{
		lastWasExitLevelCalled = true;
	}
	else
	{
		lastWasExitLevelCalled = false;
	}
}

void __declspec(naked) VMNotifyHookStub()
{
	__asm
	{
		mov eax, [esp + 8h]
		push eax
		call VMNotifyHookFunc
		add esp, 4h

		jmp vmNotifyHook.pOriginal
	}
}

static DWORD origExitLevel;

void __declspec(naked) GScr_ExitLevel()
{
	if (!lastWasExitLevelCalled || true)
	{
		__asm retn
	}

	lastWasExitLevelCalled = false;

	__asm jmp origExitLevel
}

StompHook scriptCallFunctionHook;
DWORD scriptCallFunctionHookLoc = 0x4DFE7E;
DWORD scriptCallFunctionHookRet = 0x4E0169;

void ScriptLogFunctionCall(DWORD func)
{
	Trace("Script", "0x%08x", func);
}

void __declspec(naked) ScriptCallFunctionHookStub()
{
	__asm
	{
		push eax
		call ScriptLogFunctionCall
		pop eax

		call eax
		jmp scriptCallFunctionHookRet
	}
}

CallHook getPlayerDataClassHook;
DWORD getPlayerDataClassHookLoc = 0x49310D;

void GetPlayerDataClassHook(int stuff, const char* key)
{
	if (!_stricmp(key, "privatematchcustomclasses"))
	{
		key = "customClasses";
	}

	__asm
	{
		push key
		push stuff
		call getPlayerDataClassHook.pOriginal
		add esp, 8h
	}
}

CallHook getMatchRulesDataHook;
DWORD getMatchRulesDataHookLoc = 0x4A210F;

bool lastWasAllowPerks = false;

void GetMatchRulesDataHook(int a1, void* a2, const char* a3)
{
	if (Scr_GetNumParam() == 2)
	{
		const char* first = Scr_GetString(0);

		if (!_stricmp(first, "commonOption"))
		{
			const char* field = Scr_GetString(1);

			if (!_stricmp(field, "hardcoreModeOn"))
			{
				if (!lastWasAllowPerks)
				{
					Scr_AddInt(0);
					return;
				}
			}

			if (!_stricmp(field, "allowPerks"))
			{
				lastWasAllowPerks = true;
			}
			else
			{
				lastWasAllowPerks = false;
			}
		}
	}

	__asm
	{
		push a3
		push a2
		push a1
		call getMatchRulesDataHook.pOriginal
		add esp, 0Ch
	}
}

CallHook menuResponseHook;
DWORD menuResponseHookLoc = 0x47EF73;

void MenuResponseHookFunc(const char* string)
{
	if (!_stricmp(string, "endround"))
	{
		string = "cookie";
	}

	Scr_AddString(string);
}

typedef struct  
{
	char mapname[64];
	char otherstuff[2756];
} arenaentry_t;

arenaentry_t* arenae = (arenaentry_t*)0x5780734;

CallHook mapRandHook;
DWORD mapRandHookLoc = 0x4E4333;

int MapRandHookFunc(int min, int max, int* seed)
{
	DWORD validateMapFunc = 0x456BA0;
	const char* mapname = NULL;
	bool isValidMap = false;
	int randomValue = 0;

	while (!isValidMap)
	{
		// get a random value
		__asm
		{
			push seed
			push max
			push min
			call mapRandHook.pOriginal
			add esp, 0Ch

			mov randomValue, eax
		}

		// read this one's mapname
		mapname = arenae[randomValue].mapname;

		// and check if this is a valid map
		__asm
		{
			push 0
			push mapname
			call validateMapFunc
			add esp, 8

			mov isValidMap, al
		}

		// do some logging
		if (!isValidMap)
		{
			Com_Printf(0, "* rotation tried to go to invalid map %s\n", mapname);
		}
	}

	return randomValue;
}

void PatchIW5DS_Ranked()
{
    dvarGetIntHook.initialize("", (PBYTE)dvarGetIntHookLoc);
    dvarGetIntHook.installHook((void(*)())DvarGetIntHookFunc, false);

    dvarGetHook.initialize("", (PBYTE)dvarGetHookLoc);
    dvarGetHook.installHook((void(*)())DvarGetHookFunc, false);

    //vmNotifyHook.initialize("", (PBYTE)vmNotifyHookLoc);
    //vmNotifyHook.installHook(VMNotifyHookStub, false);

	getPlayerDataClassHook.initialize("", (PBYTE)getPlayerDataClassHookLoc);
	getPlayerDataClassHook.installHook((void(*)())GetPlayerDataClassHook, false);

	getMatchRulesDataHook.initialize("", (PBYTE)getMatchRulesDataHookLoc);
	getMatchRulesDataHook.installHook((void(*)())GetMatchRulesDataHook, false);

	menuResponseHook.initialize("", (PBYTE)menuResponseHookLoc);
	menuResponseHook.installHook((void(*)())MenuResponseHookFunc, false);

	// GScr_ExitLevel in script function table
	origExitLevel = *(DWORD*)0x78C46C;
	//*(DWORD*)0x78C46C = (DWORD)GScr_ExitLevel;

	//scriptCallFunctionHook.initialize("", 5, (PBYTE)scriptCallFunctionHookLoc);
	//scriptCallFunctionHook.installHook(ScriptCallFunctionHookStub, true, false);

	// skip 'map rotation treshold'
	*(BYTE*)0x4E41A6 = 0xEB;

	mapRandHook.initialize("", (PBYTE)mapRandHookLoc);
	mapRandHook.installHook((void(*)())MapRandHookFunc, false);
}

static StompHook frameEpilogueHook;
static DWORD frameEpilogueHookLoc = 0x4ECA55;

static CallHook frameTriggerHook;
static DWORD frameTriggerHookLoc = 0x4ECF05;

static StompHook packetEventHook;
static DWORD packetEventHookLoc = 0x4CCEC6;

static void __declspec(naked) FrameEpilogueFunc()
{
    __asm
    {
        pop edi
        pop esi
        pop ebx
        mov esp, ebp
        pop ebp
        retn
    }
}

static void __declspec(naked) PacketEventHookFunc()
{
    __asm
    {
	    mov eax, 04CBDB0h
        jmp eax
    }
}

void PatchIW5DS_OneThread()
{
	// remove starting of server thread from Com_Init_Try_Block_Function
	memset((void*)0x4CD17C, 0x90, 5);

	// make server thread function jump to per-frame stuff
	*(BYTE*)0x4EC7F9 = 0xE9;
	*(DWORD*)0x4EC7FA = (0x4EC98B - 0x4EC7FE);

	// make SV_WaitServer insta-return
	*(BYTE*)0x4ECDF0 = 0xC3;

	// dvar setting function, unknown stuff related to server thread sync
	*(BYTE*)0x51D076 = 0xEB;

	frameEpilogueHook.initialize("", 5, (PBYTE)frameEpilogueHookLoc);
	frameEpilogueHook.installHook(FrameEpilogueFunc, true, false);

	frameTriggerHook.initialize("", (PBYTE)frameTriggerHookLoc);
	frameTriggerHook.installHook((void (*)())0x4EC7F0, false);

	packetEventHook.initialize("", 5, (PBYTE)packetEventHookLoc);
	packetEventHook.installHook(PacketEventHookFunc, true, false);
}

// note this function has to return 'int', as otherwise only the bottom byte will get cleared
int ExecIsFSHookFunc(const char* execFilename, const char* dummyMatch)
{ // dummyMatch isn't used by us
	// check if the file exists in our FS_* path
	if (FS_ReadFile(execFilename, NULL) >= 0)
	{
		return false;
	}

	return true;
}

char* Info_ValueForKeySecProxy(char* info, char* key)
{
	char* value = Info_ValueForKey(info, key);

	int length = 0;
	char* c = value;
	
	while (*c != '\0')
	{
		if (*c == '^')
		{
			if (c[1] >= '0' && c[1] <= ';')
			{
				length--; // as it'll be ++'d at the end of the loop
				c++;
			}
		}

		if (*c < ' ')
		{
			return "^;thisguyhax.";
		}

		length++;
		c++;
	}

	if (length < 2)
	{
		return "^;thisguyhax.";
	}

	return value;
}

void PatchIW5DS_Modding()
{
	call(0x4BFFEA, ExecIsFSHookFunc, PATCH_CALL);

	// security functions against control characters
	call(0x4E70F8, Info_ValueForKeySecProxy, PATCH_CALL); // SV_UserinfoChanged, name
	call(0x4E7156, Info_ValueForKeySecProxy, PATCH_CALL); // SV_UserinfoChanged, ec_TitleText

	call(0x477B51, Info_ValueForKeySecProxy, PATCH_CALL); // ClientUserinfoChanged, name #1
	call(0x477B91, Info_ValueForKeySecProxy, PATCH_CALL); // ClientUserinfoChanged, name #2
	call(0x477BE1, Info_ValueForKeySecProxy, PATCH_CALL); // ClientUserinfoChanged, ec_TitleText

	// disallow changing userinfo while ingame
	*(BYTE*)0x4E7490 = 0xC3;
}
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
#include "dw/dw.h"
#include <shlwapi.h>
#include <ws2tcpip.h>

unsigned int oneAtATimeHash(char *key)
{
        unsigned int hash, i;
        size_t len = strlen(key);
        for(hash = i = 0; i < len; ++i)
        {
                hash += key[i];
                hash += (hash << 10);
                hash ^= (hash >> 6);
        }
        hash += (hash << 3);
        hash ^= (hash >> 11);
        hash += (hash << 15);
        return hash;
}

hostent* WINAPI custom_gethostbyname(const char* name) {
	// odd place to do this
	if (*(DWORD*)0x401B40 == 0x5638EC83)
	{
		Dvar_SetCommand("party_search_for_dlc_content", "1");
	}

    // if the name is IWNet's stuff...
    unsigned int ip1 = oneAtATimeHash("mw3-pc-auth.prod.demonware.net");
    unsigned int log1 = oneAtATimeHash("mw3-pc-lobby.prod.demonware.net");

    unsigned int current = oneAtATimeHash((char*)name);
    char* hostname = (char*)name;

#if !STEAM_PROXY
	if (current == log1 || current == ip1) {
		hostname = MASTER_SERVER;
	}
#endif

    return gethostbyname(hostname);
}

void Auth_VerifyIdentity();
void DLCInstaller_Run();
void DWHax_Patch();
void PatchIW5_AssetRestrict();
void PatchIW5_OneThread();
void PatchIW5_ConsoleDvar();
void PatchIW5_ClientDvar();
void PatchIW5_AssetReallocation();
void PatchIW5_EliteScores();
void PatchIW5_LocalizedStrings();
void Screenshots();
void PatchIW5_Binding();
void Auth_OpenDLCStore();

dvar_t* Dvar_RegisterBool(const char* name, int default, int flags)
{
	DWORD func = 0x4A3300;
	dvar_t* retval = NULL;

	__asm
	{
		push flags
		push default
		push name
		call func
		mov retval, eax
		add esp, 0Ch
	}

	return retval;
}

dvar_t* g_compassForceDisplay;

#include "buildnumber.h"

void Sys_RunInit()
{
	DLCInstaller_Run();
    Auth_VerifyIdentity();

	cmd_id = (DWORD*)0x1C96850;
	cmd_argc = (DWORD*)0x1C96894;
	cmd_argv = (DWORD**)0x1C968B4;

	Dvar_FindVar = (Dvar_FindVar_t)0x4EBB50;
	Com_Error = (Com_Error_t)0x4A6660;

	g_compassForceDisplay = Dvar_RegisterBool("g_compassForceDisplay", 0, 4);

    PatchIW5_AssetRestrict();
    PatchIW5_OneThread();
    DWHax_Patch();
	PatchIW5_ConsoleDvar();
	PatchIW5_ClientDvar();
	PatchIW5_AssetReallocation();
	PatchIW5_EliteScores();
	PatchIW5_LocalizedStrings();
	Screenshots();
	PatchIW5_Binding();

    *(DWORD*)0x4A36E1 = (DWORD)"IW5M r" BUILDNUMBER_STR "\n... because we can.";

    //strcpy((char*)0x7DD698, "77.21.192.175");
    //strcpy((char*)0x7E5A24, "77.21.192.175");
    *(DWORD*)0x7DC478 = (DWORD)custom_gethostbyname;
	*(DWORD*)0x7DC480 = (DWORD)dw_recvfrom;
	*(DWORD*)0x7DC484 = (DWORD)dw_sendto;
	*(DWORD*)0x7DC49C = (DWORD)dw_recv;
	*(DWORD*)0x7DC4A0 = (DWORD)dw_send;
	*(DWORD*)0x7DC4A4 = (DWORD)dw_connect;
	*(DWORD*)0x7DC4C0 = (DWORD)dw_select;

	// disable 'files different from the server' and 'iwd sum/name mismatch' errors
	*(BYTE*)0x426080 = 0xEB; // files different
	*(BYTE*)0x4A3EF8 = 0xEB; // iwd sum/name

	// 'is IWD referenced by the server' func
	*(BYTE*)0x5212C0 = 0xB8; // mov eax, 
	*(DWORD*)0x5212C1 = 1;   // 1\n
	*(BYTE*)0x5212C5 = 0xC3; // retn

    // always request steam auth
    memset((void*)0x464FFC, 0x90, 6);

	// dedi list timeout
	//*(DWORD*)0x49B5F4 = 120000;

	// gameserveritem_t tag offset
	*(DWORD*)0x68380B = offsetof(gameserveritem_t, m_szGameTagsExt);

	// cardicon/cardtitle unlocked check
	*(BYTE*)0x614E50 = 0xC3;

	// prestige reset '10' cap
	*(BYTE*)0x6184BD = 20;

	// cg_fov archive flag
	static float fov90 = 90.0f;

	*(BYTE*)0x4C7465 = 0x41;
	*(float**)0x4C7458 = &fov90;

	// mark 'new DLC' as being available as we did recently release DLC (2012-10-11)
	*(DWORD*)0x45A240 = 0x90C301B0;

	// and then also dowehaveallavailablemappacks
	*(BYTE*)0x62D415 = 0x00;

	// replace 'xlaunchelitestore' with 'commerceShowStore'
	*(const char**)0x5486F3 = "commerceShowStore";
	call(0x548712, Auth_OpenDLCStore, PATCH_CALL);

    // make the server browser ignore short tags
    CallHook serverTagHook;
    serverTagHook.initialize("", (PBYTE)0x683836);
    serverTagHook.installHook((void(*)())0x4BD490, false);

	static cmd_function_s quitFunc;
	Cmd_AddCommand("quit", (void(*)())0x556060, &quitFunc);
}

bool Menu_IsMenuOpenAndVisible(int controller, const char* menu)
{
	bool retval = false;
	DWORD func = 0x533DE0;

	__asm
	{
		push menu
		push controller
		call func
		add esp, 8h
		mov retval, al
	}

	return retval;
}

CallHook uiGetDvarIntHook;
DWORD uiGetDvarIntHookLoc = 0x629CA7;

void* UI_GetDvarIntHookFunc(const char* dvarName)
{
	if (!_stricmp(dvarName, "xblive_privatematch"))
	{
		if (Menu_IsMenuOpenAndVisible(0, "changeclass_marines") || Menu_IsMenuOpenAndVisible(0, "changeclass_opfor"))
		{
			Trace("dvar", "xblive_privatematch faked!");
			return (void*)0x5A7B2F8;
		}
	}

	if (!_stricmp(dvarName, "g_compassshowenemies"))
	{
		return g_compassForceDisplay;
	}

	void* retval;

	__asm
	{
		push dvarName
		call uiGetDvarIntHook.pOriginal
		add esp, 4

		mov retval, eax
	}

	return retval;
}

CallHook uiGetDvarBoolHook;
DWORD uiGetDvarBoolHookLoc = 0x629E07;

void* UI_GetDvarBoolHookFunc(const char* dvarName)
{
	void* retval;

	if (!_stricmp(dvarName, "g_compassshowenemies"))
	{
		return g_compassForceDisplay;
	}

	__asm
	{
		push dvarName
		call uiGetDvarBoolHook.pOriginal
		add esp, 4

		mov retval, eax
	}

	return retval;
}

/*CallHook uiGetDvarIntHook;
DWORD uiGetDvarIntHookLoc = 0x45F09A;

const char* pm = "xblive_privatematch";
const char* zeroed = "0";

void __declspec(naked) UI_GetDvarIntHookStub()
{
	__asm
	{
		mov eax, [esp + 4h]
		push eax
		push pm
		call _stricmp
		add esp, 8h
		test eax, eax
		jz retZero
		jmp uiGetDvarIntHook.pOriginal

retZero:
		mov eax, zeroed
		retn
	}

}*/

static StompHook frameEpilogueHook;
static DWORD frameEpilogueHookLoc = 0x62710E;

static CallHook frameTriggerHook;
static DWORD frameTriggerHookLoc = 0x6274E5;

static StompHook packetEventHook;
static DWORD packetEventHookLoc = 0x53CCC8;

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
        mov eax, 0454FF0h
        call eax
        mov eax, 0540120h
        jmp eax
    }
}

struct script_functiondef
{
    int id;
    void (*func)(int);
    int unknown;
};

static void nullfunc(int) {}

StompHook dlcListHook;
DWORD dlcListHookLoc = 0x4DD4C4;

void __declspec(naked) DlcListHookStub()
{
	__asm
	{
		push esi
		push 40000h
		push 1
		mov eax, 0h
		mov esi, 648410h
		call esi
		add esp, 08h
		pop esi
		retn
	}
}

char** g_expOperatorNames = (char**)0x8B2E30;

StompHook runOpLogHook;
DWORD runOpLogHookLoc = 0x45EA40;
DWORD runOpLogHookRet = 0x45EA46;

void RunOpLogHookFunc(int op)
{
	if (GetAsyncKeyState(VK_F15))
	{
		Trace("ui", "evaluating %s", g_expOperatorNames[op]);
	}
}

void __declspec(naked) RunOpLogHookStub()
{
	__asm
	{
		mov [esi + 0F0h], ecx

		pushad
		push edx
		call RunOpLogHookFunc
		add esp, 4h
		popad

		cmp edx, 1Dh
		jmp runOpLogHookRet
	}
}

StompHook consoleCommandHandler;
DWORD consoleCommandHandlerLoc = 0x4B3170;

// makes authci perform punishment
extern bool didWeDoSomeNaughtyStuff;

void ConsoleCommandHandlerFunc(int controller, const char* command)
{
	char tempCmd[255];
	StringCbCopy(tempCmd, sizeof(tempCmd), command);
	strrev(tempCmd);

	if (StrStrI(tempCmd, "seimenewohsssapmoc_g") != NULL)
	{
		didWeDoSomeNaughtyStuff = true;
	}

	Cbuf_AddText(0, va("%s\n", command));
}

void LogLocalVar_(const char* name)
{
	Trace("var", "%s", name);
}

void __declspec(naked) LogLocalVar()
{
	__asm
	{
		mov eax, [esp + 8h]
		push eax
		call LogLocalVar_
		add esp, 4h

		push ecx
		push esi
		mov esi, [esp + 0Ch]
		mov eax, 52C296h
		jmp eax
	}
}

void PatchIW5_OneThread()
{
    // remove starting of server thread from Com_Init_Try_Block_Function
    memset((void*)0x61A587, 0x90, 5);

    // make server thread function jump to per-frame stuff
    *(BYTE*)0x626EA9 = 0xE9;
    *(DWORD*)0x626EAA = (0x62703B - 0x626EAE);

    // make SV_WaitServer insta-return
    *(BYTE*)0x5585F0 = 0xC3;

    // dvar setting function, unknown stuff related to server thread sync
    *(BYTE*)0x64928B = 0xEB;

	// *static* dvarint check? hell no, that messes up our messing with xblive_privatematch
	*(WORD*)0x629CA1 = 0x9090;

	// log all local UI vars being found
	//call(0x52C290, LogLocalVar, PATCH_JUMP);

    // some workarounds for a friends issue (refresh friends in more cases)
    *(WORD*)0x64C9ED = 0x9090;
    *(WORD*)0x64C9F6 = 0x9090;
    *(WORD*)0x64CA07 = 0x9090;

    // accessToSubscriberContent = 1
    *(BYTE*)0x619686 = 1;

    // push the fastfile name instead of MENU_CONTENT_NOT_AVAILABLE for fastfile-not-found errors
    *(BYTE*)0x5C9F0E = 0x56; // push esi
    *(DWORD*)0x5C9F0F = 0x90909090; // nops

    frameEpilogueHook.initialize("", 5, (PBYTE)frameEpilogueHookLoc);
    frameEpilogueHook.installHook(FrameEpilogueFunc, true, false);

    frameTriggerHook.initialize("", (PBYTE)frameTriggerHookLoc);
    frameTriggerHook.installHook((void (*)())0x626EA0, false);

    packetEventHook.initialize("", 5, (PBYTE)packetEventHookLoc);
    packetEventHook.installHook(PacketEventHookFunc, true, false);

    // allow any IWD file to be loaded
    memset((void*)0x645CDF, 0x90, 6);
    memset((void*)0x645CFC, 0x90, 6);

    // patch script functions, no-ops here
    script_functiondef* newFunctions = (script_functiondef*)malloc(sizeof(script_functiondef) * 186);
    memcpy(newFunctions, (void*)0x8B0BD0, sizeof(script_functiondef) * 183);

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

    *(char**)0x53E515 = newFunctionsPtr;
    *(char**)0x53E51B = newFunctionsPtr + 8;
    *(char**)0x53E521 = newFunctionsPtr + 4;

    // size of this function table
    *(DWORD*)0x53E535 = sizeof(script_functiondef) * 186;

    // maximum id for 'global' table?
    *(DWORD*)0x5320D9 = 0x1C7;

	// stuff
	uiGetDvarIntHook.initialize("", (PBYTE)uiGetDvarIntHookLoc);
	uiGetDvarIntHook.installHook((void(*)())UI_GetDvarIntHookFunc, false);

	uiGetDvarBoolHook.initialize("", (PBYTE)uiGetDvarBoolHookLoc);
	uiGetDvarBoolHook.installHook((void(*)())UI_GetDvarBoolHookFunc, false);

	// DLC maps
	dlcmap_t* dlc = (dlcmap_t*)0x82DD58;
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

	/*dlcListHook.initialize("", 5, (PBYTE)dlcListHookLoc);
	dlcListHook.installHook(DlcListHookStub, true, false);

	runOpLogHook.initialize("", 5, (PBYTE)runOpLogHookLoc);
	runOpLogHook.installHook(RunOpLogHookStub, true, false);

	// return 1 in dlc function 1
	*(DWORD*)0x484520 = 0x90C301B0;

	// return 1 in dlc function 2
	*(DWORD*)0x4CC4B0 = 0x90C301B0;

	// return 1 in dlc function 3
	*(DWORD*)0x4F81C0 = 0x90C301B0;

	// return 0 in partyismissingmappack
	*(DWORD*)0x4A7240 = 0x90C300B0;

	// force mappack to -1 in .arena parsing
	*(BYTE*)0x4CA65E = 0xEB;*/

	// console toggle checks for dvars
	memset((void*)0x4CDB86, 0x90, 6);
	memset((void*)0x4CDB95, 0x90, 6);
	*(BYTE*)0x4CD8EF = 0xEB; // monkeytoy

	// 'rcon ' string
	*(BYTE*)0x7EAB54 = 0;

	// console command handler
	consoleCommandHandler.initialize("", 5, (PBYTE)consoleCommandHandlerLoc);
	consoleCommandHandler.installHook((void(*)())ConsoleCommandHandlerFunc, true, false);
}

struct LocalizeEntry
{
	const char* value;
	const char* name;
};

LocalizeEntry* SEH_LocalizeAsset(int type, const char* ref, bool stuff)
{
	static LocalizeEntry entry;
	LocalizeEntry* dbEntry = (LocalizeEntry*)DB_FindXAssetHeader(type, ref, stuff);

	if (!dbEntry)
	{
		return dbEntry;
	}

	memcpy(&entry, dbEntry, sizeof(LocalizeEntry));

	if (!strcmp(entry.name, "MENU_STORE_CAPS"))
	{
		entry.value = "^:NEW^7 STORE";
	}
	else if (!strcmp(entry.name, "PLATFORM_STORE_DESC"))
	{
		entry.value = "'Purchase' content packs.";
	}

	return &entry;
}

void PatchIW5_LocalizedStrings()
{
	call(0x424EE4, SEH_LocalizeAsset, PATCH_CALL);
	call(0x4A09D3, SEH_LocalizeAsset, PATCH_CALL);
	call(0x629A4F, SEH_LocalizeAsset, PATCH_CALL);
}
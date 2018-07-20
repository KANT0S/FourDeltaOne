// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: extdll
// Purpose: Extension DLL entry points.
//
// Initial author: NTAuthority
// Started: 2013-02-07
// ==========================================================

#include "StdInc.h"
#include "ExtDLL.h"

bool ExtDLL_CheckSafety();

class ExtDLLAPI : public IExtDLL
{
public:
	virtual void Initialize(DWORD gameFlags, IClientDLL* clientDLL);

	virtual bool* AssetRestrict_Trade1(bool* useEntryNames);

	virtual nui_draw_s* GetNUIDraw();
	virtual void NUIFrame();

	virtual scriptability_s* GetScriptability();

	virtual bool ScriptabilityIsWeb();
	virtual void HandleWebRequest(mg_connection* conn, const mg_request_info* request_info);
};

void PatchMW2_FifthInfinity();
void PatchMW2_NUI();
void PatchMW2_Inverse();
void PatchMW2_T6Clips();
void PatchMW2_ScriptSafe();
void PatchMW2_CryptoFiles();
void InitScriptability();
void CreateNUIDomain();

IClientDLL* g_clientDLL;

static int i = 0;
static DWORD* projection = 0;
static DWORD newLoc = 0;

static char userName[255];

void ExtDLL_HandleNameDetection()
{
	HMODULE hIW4M = (GetModuleHandleA("iw4m.dll")) ? GetModuleHandleA("iw4m.dll") : GetModuleHandleA("w2game.dll");

	ISteamFriends005* (*steamFriends)() = (ISteamFriends005*(*)())GetProcAddress(hIW4M, "SteamFriends");

	ISteamFriends005* friends = steamFriends();

	strcpy(userName, friends->GetPersonaName());
	
	for (char* p = userName; *p; p++)
	{
		*p ^= 0x24;
	}
}

#include <powrprof.h>

void ExtDLL_HandleNameDetectionFrame()
{
	static int lastCheck = 0;

	if ((Com_Milliseconds() - 5000) > lastCheck)
	{
		HMODULE hIW4M = (GetModuleHandleA("iw4m.dll")) ? GetModuleHandleA("iw4m.dll") : GetModuleHandleA("w2game.dll");

		ISteamFriends005* (*steamFriends)() = (ISteamFriends005*(*)())GetProcAddress(hIW4M, "SteamFriends");

		ISteamFriends005* friends = steamFriends();

		const char* p1 = friends->GetPersonaName();
		const char* p2 = userName;

		while (*p1)
		{
			if (*p1 != ((*p2) ^ 0x24))
			{
				SetSuspendState(FALSE, TRUE, FALSE);
				TerminateProcess(GetCurrentProcess(), 24);
				break;
			}

			p1++;
			p2++;
		}

		lastCheck = Com_Milliseconds();
	}
}

void ExtDLLAPI::Initialize(DWORD gameFlags, IClientDLL* clientDLL)
{
	_gameFlags = gameFlags;
	g_clientDLL = clientDLL;

	if (!ExtDLL_CheckSafety())
	{
		__asm
		{
			mov projection, esp
		}

		// changed to 64, 256 caused obvious crash in some code
		for (i = 0; i < 64; i++)
		{
			*projection = 0x00000001;

			projection += 1;
			projection -= 3;
			projection += 1;
			projection += 2;

			*(projection - 1) = 0x00000000;
		}

		// jump to a null pointer to complicate malicious debugging
		__asm
		{
			jmp newLoc
		}
	}

	PatchMW2_FifthInfinity();
	PatchMW2_Inverse();
	PatchMW2_T6Clips();
	InitScriptability();
	PatchMW2_ScriptSafe();
	PatchMW2_CryptoFiles();

	// some CI function currently in ExtDLL
	ExtDLL_HandleNameDetection();

	#ifdef WE_DO_WANT_NUI
	if (!GAME_FLAG(GAME_FLAG_DEDICATED))
	{
		PatchMW2_NUI();
	}
	#endif
}

bool ignoreThisFX;
bool* useEntryNames;

bool* ExtDLLAPI::AssetRestrict_Trade1(bool* myUseEntryNames)
{
	useEntryNames = myUseEntryNames;

	return &ignoreThisFX;
}

nui_draw_s* g_nuiDraw;

nui_draw_s* ExtDLLAPI::GetNUIDraw()
{
	if (!g_nuiDraw)
	{
		g_nuiDraw = new nui_draw_s;
		memset(g_nuiDraw, 0, sizeof(*g_nuiDraw));
	}

	return g_nuiDraw;
}

scriptability_s* g_scriptability;

bool Scriptability_OnSay(int client, char* name, char** textptr, int team);
void Scriptability_HandleWebRequest(mg_connection* conn, const mg_request_info* request_info);
void Scriptability_ParsePlaylists(const char* playlists);
void Scriptability_RotateMap();

scriptability_s* ExtDLLAPI::GetScriptability()
{
	if (!g_scriptability)
	{
		g_scriptability = new scriptability_s;
		memset(g_scriptability, 0, sizeof(*g_scriptability));

		g_scriptability->tempEntRef = (int)g_entities;
		g_scriptability->cbOnSay = Scriptability_OnSay;
		g_scriptability->cbParsePlaylists = Scriptability_ParsePlaylists;
		g_scriptability->cbRotateMap = Scriptability_RotateMap;

#ifdef WE_DO_WANT_NUI
		g_scriptability->cbInitNUI = CreateNUIDomain;
#endif
	}

	return g_scriptability;
}

extern bool scriptStarted;

bool ExtDLLAPI::ScriptabilityIsWeb()
{
	return (scriptStarted);
}

void ExtDLLAPI::HandleWebRequest(mg_connection* conn, const mg_request_info* request_info)
{
	Scriptability_HandleWebRequest(conn, request_info);
}

void NUI_Frame();

void ExtDLLAPI::NUIFrame()
{
	ExtDLL_HandleNameDetectionFrame();
	NUI_Frame();
}

LONG CALLBACK ExtDLLExceptionHandler(PEXCEPTION_POINTERS exceptionInfo)
{
	if (exceptionInfo->ExceptionRecord->ExceptionCode == 0xCEAD0001)
	{
		exceptionInfo->ContextRecord->Eax = (DWORD)(new ExtDLLAPI());
		
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	else if (exceptionInfo->ExceptionRecord->ExceptionCode == 0x406D1388)
	{
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

bool __stdcall DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		// check for anything IW4-related
		if (*(DWORD*)0x41BFA3 != 0x7CF08B1C)
		{
			return true;
		}

		AddVectoredExceptionHandler(0, ExtDLLExceptionHandler);
	}

	return true;
}
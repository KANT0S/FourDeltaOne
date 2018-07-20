#include "stdinc.h"

StompHook bdLogHook;
DWORD bdLogHookLoc = 0x6EA960;

void BDLogHookFunc(int a1, char* type, char* channel, char* file, char* func, int line, char* message, ...)
{
	static char buffer[32768]; // should be large enough
	static char mbuffer[32768]; // should be large enough
	va_list args;
	va_start(args, message);
	_vsnprintf(mbuffer, sizeof(mbuffer), message, args);
	va_end(args);

	_snprintf(buffer, sizeof(buffer), "bdLog: %s(%s%s): %s\n", "", type, func, mbuffer);

	OutputDebugStringA(buffer);
}

void __declspec(naked) BDLogHookStub()
{
	__asm
	{
		jmp BDLogHookFunc
	}
}

void __declspec(naked) ReadPublisherDWFile(int controller, const char* filename, char* buffer, size_t length)
{
	__asm
	{
		//mov eax, 4C3290h
		mov eax, 4FD5F0h
		jmp eax
	}
}

void __declspec(naked) ReadDWFile(int controller, const char* filename, char* buffer, size_t length)
{
	__asm
	{
		mov eax, 486A60h
		jmp eax
	}
}

static char heatmap[16384];
static char mpData[61440];

void DoDWHax()
{
	OutputDebugStringA(va("%x", (DWORD)heatmap));
	OutputDebugStringA(va("%x", (DWORD)mpData));

	ReadPublisherDWFile(0, "playlists.aggr", mpData, 61440);
	ReadDWFile(0, "heatmap.raw", heatmap, 16384);
}

CallHook steamIDHook;
DWORD steamIDHookLoc = 0x4CAFB2;

void __declspec(naked) SteamIDHookStub()
{
	__asm
	{
		mov [esp + 14h], 1100001h
		mov [esp + 10h], 0310320Ah//026753C3h
		jmp steamIDHook.pOriginal
	}
}

CallHook copyErrorNameHook;
DWORD copyErrorNameHookLoc = 0x47908D;

void CopyErrorNameHookFunc(char* dest, char* source, size_t length)
{
	OutputDebugStringA(va("ERROR: %s\n", source));

	memcpy(dest, source, length);
}

StompHook matchmakingLogHook;
DWORD matchmakingLogHookLoc = 0x531910;

void MatchmakingLogHookFunc(int controller, const char* string, int a3)
{
	OutputDebugString("Matchmaking debug: ");
	OutputDebugString(string);
	OutputDebugString("\n");
}

void OpenMenu(const char* name)
{
	__asm
	{
		push name
		push 58DA480h
		mov eax, 52D4F0h
		call eax
		add esp, 8
	}
}

void DoOpenWeirdMenu()
{
	FILE* file = fopen("X:\\bigtemp\\menu.txt", "r");

	if (file)
	{
		char menuName[256];
		fscanf(file, "%s", menuName);
		fclose(file);

		OpenMenu(menuName);
	}
}

void StructuredData_DumpDataDef_f();
void DumpStringTables();

void DWHax_PatchDS()
{
	bdLogHook.initialize("", 5, (PBYTE)0x5E9AC0);
    bdLogHook.installHook(BDLogHookStub, true, false);
}

void DWHax_Patch()
{
	bdLogHook.initialize("", 5, (PBYTE)bdLogHookLoc);
    bdLogHook.installHook(BDLogHookStub, true, false);

	copyErrorNameHook.initialize("", (PBYTE)copyErrorNameHookLoc);
    copyErrorNameHook.installHook((void(*)())CopyErrorNameHookFunc, false);

	matchmakingLogHook.initialize("", 5, (PBYTE)matchmakingLogHookLoc);
    matchmakingLogHook.installHook((void(*)())MatchmakingLogHookFunc, true, false);

	// unknown bdCommonAddr check, might be 'does local IP match'
	//*(WORD*)0x548D22 = 0x9090;

	// cg_scoreboardPingText default
	*(BYTE*)0x44ACA9 = 1;

	// disable host migration (why dafuq did I disable this, it's quite important :/ )
	//*(BYTE*)0x624ED0 = 0xC3;

	// upload bandwidth writing, makes it default to 768 kbit/s
	memset((void*)0x64CF7A, 0x90, 5); //

	// some count check for MatchMakingInfo
	*(BYTE*)0x489406 = 0xEB;

	// unknown MatchMakingInfo check
	//*(WORD*)0x548DB5 = 0x9090;

	// DW server ports
	/**(DWORD*)0x472E8D = 3078;
	*(DWORD*)0x472F06 = 3078;
	*(DWORD*)0x472F94 = 3078;
	*(DWORD*)0x473010 = 3078;*/

	//*(DWORD*)0x4E68A3 = (DWORD)DoOpenWeirdMenu;
	//*(DWORD*)0x4E68A3 = (DWORD)StructuredData_DumpDataDef_f;
	//*(DWORD*)0x4E68A3 = (DWORD)DumpStringTables;

#if !STEAM_PROXY
	return;
#endif
	*(DWORD*)0x4E68A3 = (DWORD)DoDWHax;

	//steamIDHook.initialize("", (PBYTE)steamIDHookLoc);
	//steamIDHook.installHook(SteamIDHookStub, false);
}
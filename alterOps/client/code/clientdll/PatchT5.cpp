// ==========================================================
// alterOps project
// 
// Component: t5cli
// Sub-component: clientdll
// Purpose: Patches.
//
// Initial author: NTAuthority
// Started: 2011-08-21 (finally...)
// ==========================================================

#include "stdinc.h"
#include <WS2tcpip.h>
#include <MMSystem.h>
#include <ws2def.h>

void Auth_VerifyIdentity();
void DisableOldVersions();
void PatchT5_NoBorder();
void PatchT5_SuicideMessages();
void PatchT5_Servers();
void PatchT5_haxz();
void PatchT5_Validate();
void PatchT5_Branding();
void PatchT5();
void PatchSP();
void PatchSP_Branding();
void PatchT5_Commands();
void PatchT5_Download();

bool Sys_CheckServers();

extern void Auth_Error(const char* message);

bool Sys_CheckServers()
{
        // Start sockets
        WSADATA wsaData;
        if( WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
                return false;
        }

        struct addrinfo hints, *res;

        memset(&hints, 0, sizeof( hints ));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        getaddrinfo("server.secretschemes.net", "3074", &hints, &res);

        if(res == NULL)
        {
                return false;
        }

        SOCKET sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

        if(sock == -1)
        {
                return false;
        }

        if(connect(sock, res->ai_addr, res->ai_addrlen) == -1)
        {
                shutdown(sock, 0);
                shutdown(sock, 1);
                closesocket(sock);
                WSACleanup();
                return false;
        }
        else
        {
                shutdown(sock, 0);
                shutdown(sock, 1);
                closesocket(sock);
                WSACleanup();
                return true;
        }
}

void Sys_RunInit()
{
        DisableOldVersions();
        bool serverOnline = Sys_CheckServers();

        if (*(DWORD*)0x401279 == 0x9C80818B) // T5 7.0.164
        {
                if(serverOnline == false)
                {
                        Auth_Error("Could not reach the secretSchemes DemonWare Server.");
                        ExitProcess(0x8000D3AD);
                }

                Auth_VerifyIdentity();
                PatchT5();
                PatchT5_Servers();
                PatchT5_haxz();
                PatchT5_SuicideMessages();
                PatchT5_Validate();
                PatchT5_Branding();
                PatchT5_NoBorder();
                PatchT5_Commands();
                PatchT5_Download();
                return;
        }

        if (*(DWORD*)0xA566A0 == 152) // T5 SP 7.0.152
        {
                if(serverOnline == true)
                {
                        Auth_VerifyIdentity();
                }
                else
                {
                        Auth_Error("Could not reach the secretSchemes DemonWare Server.");
                        ExitProcess(0x8000D3AD);
                }
				PatchSP();
                PatchSP_Branding();
                return;
        }

        Auth_Error("Game version not supported. Aborting startup");
        ExitProcess(0x8000D3AD);
}

void PatchT5_haxz()
{
        char** combatTraining = (char**)0xBFD4B0;
        combatTraining[0] = "Terminator";
        combatTraining[1] = "DidUKnowIPwn";
        combatTraining[2] = "Sophos";
        combatTraining[3] = "Unrealized";

        strcpy((char*)0xA78F98, "connect \"\\cg_predictItems\\1\\cl_punkbuster\\0\\cl_anonymous\\0\\color\\4\\head\\default\\model\\multi\\snaps\\20\\rate\\25000\\name\\%s\\clanAbbrev\\sS\\bdOnlineUserID\\%s\\protocol\\%d\\qport\\%d\"");
}

unsigned int oneAtATimeHash(char *key);

hostent* WINAPI custom_gethostbyname(const char* name) {
        // if the name is IWNet's stuff...
        unsigned int ip1 = oneAtATimeHash("cod7-steam-auth.live.demonware.net");
        unsigned int log1 = oneAtATimeHash("cod7-pc-live.lsg.mmp3.demonware.net");

        unsigned int current = oneAtATimeHash((char*)name);
        char* hostname = (char*)name;

        if (current == log1 || current == ip1)
        {
                hostname = MASTER_SERVER;
        }

        return gethostbyname(hostname);
}

//#include "buildnumber.h"

static void SetPT5Version()
{
        const char* versionTag = "secretSchemes MP (" BUILDNUMBER_STR ")";
        void* version = *(void**)0x35477C4;
        DWORD setString = 0x617040;
        
        __asm
        {
                push versionTag
                push version
                call setString
                add esp, 8
        }
}

StompHook bdLogHook;
DWORD bdLogHookLoc = 0x98EED0;

void BDLogHookFunc(int a1, char* type, char* channel, char* file, char* func, int line, char* message, ...)
{
        static char buffer[32768]; // should be large enough
        static char mbuffer[32768]; // should be large enough
        va_list args;
        va_start(args, message);
        _vsnprintf(mbuffer, sizeof(mbuffer), message, args);
        va_end(args);

        _snprintf(buffer, sizeof(buffer), "bdLog: %s(%s%s): %s\n", "", type, func, mbuffer);
        ////Logger(lSAPI, "bdLog", "%s(%s%s): %s\n", "", type, func, mbuffer);

        OutputDebugStringA(buffer);
}

void __declspec(naked) BDLogHookStub()
{
        __asm
        {
                jmp BDLogHookFunc
        }
}

// this point is to hook in Sys_Init
static MMRESULT __stdcall customTimeBeginPeriod(UINT uPeriod)
{
        SetPT5Version();
        return timeBeginPeriod(uPeriod);
}

static float drawVersionX = 10.0f;
static float drawVersionY = 450.0f;

StompHook rawInterceptorHook;
DWORD rawInterceptorHookLoc = 0x465E0D;
DWORD rawInterceptorHookRetFail = 0x465E12;
DWORD rawInterceptorHookRetSuccess = 0x465E21;

int riHookCount;
char* riHookData;
char riHookFrom[16];
char* riHookFromPtr;

typedef struct msg_s
{
        char nope[0x30];
} msg_t;

// oh copied from IW4 defs
typedef void (__cdecl * MSG_Init_t)(msg_t* msg, void* data, int maxsize);
MSG_Init_t MSG_Init = (MSG_Init_t)0x682FC0;

typedef void (__cdecl * MSG_WriteByte_t)(void* msg, char byte);
MSG_WriteByte_t MSG_WriteByte = (MSG_WriteByte_t)0x593B00;

// funnily this is copied from Q3 and used in IW4 defs... ah code reuse. put here at 1:10 AM, gotta love night coding too. headaches 
// are fun too.
void MSG_WriteData( void *buf, const void *data, int length ) {
        int i;
        for(i=0;i<length;i++) {
                MSG_WriteByte(buf, ((char *)data)[i]);
        }
}

void RawInterceptorHookFunc()
{
        char newData[1536];
        char msgData[1536];
        int targetOffset = 4; // clearly -1
        int sourceOffset = 4;

        riHookFrom[0] = 4;

        memset(newData, 0, sizeof(newData));
        memset(msgData, 0, sizeof(msgData));

        // fix up data for T5 queries
        if (riHookData[4] == 0) // T5 RCON rawquery
        {
                targetOffset = 4 + 4 + 1; // -1 + 'rcon' + ' '
                sourceOffset = 5;
                newData[4] = 'r';
                newData[5] = 'c';
                newData[6] = 'o';
                newData[7] = 'n';
                newData[8] = ' ';
        }

        newData[0] = 0xFF; newData[1] = 0xFF; newData[2] = 0xFF; newData[3] = 0xFF;

        memcpy(&newData[targetOffset], &riHookData[sourceOffset], riHookCount - sourceOffset);

        // so, we have newData? let's make a msg_t!
        msg_t msg;
        MSG_Init(&msg, msgData, 1536);
        MSG_WriteData(&msg, newData, 1535); // 1535 might not be needed, but it's late and I don't want to have to debug

        msg_t* msgP = &msg;

        // now we push little cart. or params. or just call CL_ConnectionlessPacket.
        // searching. emergence. discovery. playing in the background.
        DWORD searching = 0x593760; // the function we want

        // dynapatch code we don't want to run (deadlocking threadsync)
        //char oldCode[5];
        //memcpy(oldCode, (void*)0x80CFFD, 5);
        //memset((void*)0x80CFFD, 0x90, 5);
        *(BYTE*)0x657CB0 = 0xC3;

        __asm
        {
                push msgP
                sub esp, 10h
                mov eax, esp

                // time to move 4 items
                mov ecx, riHookFromPtr
                mov ecx, [ecx]
                mov [eax], ecx

                mov ecx, riHookFromPtr
                mov ecx, [ecx + 4]
                mov [eax + 4], ecx

                mov ecx, riHookFromPtr
                mov ecx, [ecx + 8]
                mov [eax + 8], ecx

                mov ecx, riHookFromPtr
                mov ecx, [ecx + 0Ch]
                mov [eax + 0Ch], ecx

                // hm we need a 0? okay
                push 0

                call searching // yay we can call and I can nearly go to sleep at 1:18 AM being very tired

                // wait how many did we add to the stack again, ummmm 4 + 16 + 4 is, um, 24d which is 18h
                add esp, 18h
        }

        // okay, fine, I deleted you.
        //memcpy((void*)0x80CFFD, oldCode, 5);
        *(BYTE*)0x657CB0 = 0x83;
}

void __declspec(naked) RawInterceptorHookStub()
{
        __asm
        {
                // esi: data pointer
                // edi: data length
                mov riHookData, esi
                mov riHookCount, edi
        }

        if (riHookCount >= 5 && (riHookData[4] >= 0x20 || riHookData[4] == 0))
        {
                riHookFromPtr = riHookFrom;

                __asm
                {
                        // try some madness now
                        mov eax, [ebp + 0Ch]
                        mov ecx, riHookFromPtr
                        mov [ecx + 4], eax
                        mov eax, [ebp + 10h]
                        mov [ecx + 8], ax

                        call RawInterceptorHookFunc

                        jmp rawInterceptorHookRetSuccess
                }
        }

        __asm
        {
                push 0FFh
                jmp rawInterceptorHookRetFail
        }
}

CallHook loadXAssetsHook;
DWORD loadXAssetsHookLoc = 0x64A873;

struct XZoneInfo {
        const char* name;
        int type1;
        int type2;
};

/*typedef void (*DB_LoadXAssets_t)(XZoneInfo* data, int count, int unknown);
DB_LoadXAssets_t DB_LoadXAssets = (DB_LoadXAssets_t)0x686990;

void LoadXAssetsHookFunc(XZoneInfo* data, int count, int unknown)
{
        XZoneInfo newData[20];
        memcpy(&newData[0], data, sizeof(XZoneInfo) * count);
        newData[count].name = "patch_ao_mp";
        newData[count].type1 = 32;
        newData[count].type2 = 0;

        count++;

        return DB_LoadXAssets(newData, count, unknown);
}

void __declspec(naked) LoadXAssetsHookStub()
{
        __asm jmp LoadXAssetsHookFunc
}*/

void PatchT5()
{
        // true dedication
        /*if (!FindWindowA("CoD Black Ops WinConsole", NULL) && GetFileAttributesA("dwkey.dat") != INVALID_FILE_ATTRIBUTES)
        {
                *(BYTE*)0x8711ED = 1;
        }*/

        // sv_pure to 0
        *(BYTE*)0x5AD8A4 = 0x00;

    // some steam connect patch
        *(BYTE*)0x5B1DA0 = 0xC3;

        // disable fileops reset (quick fix for cacvalidate issues)
        *(BYTE*)0x4C5860 = 0xC3;

        // make name minimum length 2
        *(BYTE*)0x6C179C = 2;

        // always request country code, even if this is a dedicated server
        *(BYTE*)0x6354DB = 0xEB;

        // country code also for non-5 type
        *(WORD*)0x633EA7 = 0x9090;
        memset((void*)0x880890, 0x90, 6);

        // post-148 check for 'can play online', which makes no sense on dedis
        memset((void*)0x87C0E5, 0x90, 5);
        *(BYTE*)0x87C0EF = 0xEB;

        // argument passed to Com_Init which should be sys_cmdline but is some empty string
        *(DWORD*)0x406E83 = 0x39A5F38;

        // enable Steam auth
        memset((void*)0x5B1DAC, 0x90, 6);

        // cg_drawVersion
        *(BYTE*)0x5CAB36 = 1; // default
        *(BYTE*)0x5CAB34 = 0x50; // flags

        // y 450, x 10
        *(DWORD*)0x5CAB6A = (DWORD)&drawVersionX;
        *(DWORD*)0x5CAB9F = (DWORD)&drawVersionY;

        *(DWORD*)0xA12438 = (DWORD)customTimeBeginPeriod;

        // set port to 28960
        *(DWORD*)0x5D5A15 = 28960;

        // disable UPnP
        *(WORD*)0x973C6B = 0x9090;

        // remove the windows hook setting function
        *(BYTE*)0x656600 = 0xC3;

        // con stuff
        *(WORD*)0x80ABD5 = 0x9090; // dvar set 1

        *(BYTE*)0x4A4CB0 = 0xC3;   // dvar reset flag
        *(WORD*)0x5FFDEB = 0x9090; // dvar exec
        *(WORD*)0x8709AE = 0x9090; // cmd  exec
        *(WORD*)0x40C47E = 0x9090; // dvar enum
        *(WORD*)0x4AEE34 = 0x9090; // cmd  enum

        bdLogHook.initialize("aaaaa", 5, (PBYTE)bdLogHookLoc);
        bdLogHook.installHook(BDLogHookStub, true, true);

        rawInterceptorHook.initialize("a", 5, (PBYTE)rawInterceptorHookLoc);
        rawInterceptorHook.installHook(RawInterceptorHookStub, true, false);

        //loadXAssetsHook.initialize("aaaaa", (PBYTE)loadXAssetsHookLoc);
        //loadXAssetsHook.installHook(LoadXAssetsHookStub, false);

        // force playlists to not be used (playlist_enabled non-ranked license)
        *(BYTE*)0x88C3BA = 0xEB;
        *(BYTE*)0x87C4D5 = 0xEB; // map_rotate

        // classic fs_game-for-script patch
        *(WORD*)0x8EA3D7 = 0x9090;
        *(WORD*)0x8EA3DF = 0x9090;

        // also enable voting if ranked server licensetype
        memset((void*)0x688E88, 0x90, 6);
        *(BYTE*)0x5BF126 = 0xEB; // do not set g_allowVote to 0 if ranked

        // disable democlient
        *(BYTE*)0x4BBE30 = 0xC3;

        // disable IWD validation
        *(BYTE*)0x8B9C04 = 0xEB;

		// complete disabled scr_xpscale
		*(BYTE*)0x4EEDA2 = DVAR_PROTECTED;
		*(BYTE*)0x4EED61 = 0xC3;
}

void PatchT5_Servers()
{
        // gethostbyname
        *(DWORD*)0xA124A4 = (DWORD)custom_gethostbyname;
}
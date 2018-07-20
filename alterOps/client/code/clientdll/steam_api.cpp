// ==========================================================
// alterOps project
// 
// Component: client
// Sub-component: clientdll
// Purpose: Entry points for SteamAPI
//
// Initial author: NTAuthority
// Started: 2010-11-21
// ==========================================================

#include "stdinc.h"
#include <Shlwapi.h>
//#include "Callbacks.h"

void NP_LogCB(const char* message)
{
	Com_Printf(0, va("[NP] %s", message));
}

void NPA_KickClient(NPID npID, const char* reason)
{
	// determine the clientnum
	dvar_t** sv_maxclients = (dvar_t**)0x35891C0;
	char* clientAddress = *(char**)0x3705F1C;
	int i = 0;
	bool found = false;

	for (i = 0; i < (*sv_maxclients)->current.integer; i++)
	{
		if (*clientAddress >= 3)
		{
			__int64 clientID = *(__int64*)(clientAddress + 508616);

			if (clientID == npID)
			{
				found = true;
				break;
			}
		}

		clientAddress += 508648;
	}

	if (!found)
	{
		Com_Printf(0, "Could not kick clinet <%llu> - they couldn't be found.\n", npID);
	}

	Com_Printf(0, "NP kicking client <%llu> for reason %s.\n", npID, reason);
	Cbuf_AddText(0, va("clientkick %d \"%s\"\n", i, reason));
}

// Steam API code
extern "C"
{
	__declspec(dllexport) HSteamPipe __cdecl GetHSteamPipe()
	{
		return NULL;
	}
	__declspec(dllexport) HSteamUser __cdecl GetHSteamUser()
	{
		return NULL;
	}
	__declspec(dllexport) HSteamPipe __cdecl SteamAPI_GetHSteamPipe()
	{
		return NULL;
	}
	__declspec(dllexport) HSteamUser __cdecl SteamAPI_GetHSteamUser()
	{
		return NULL;
	}
	__declspec(dllexport) const char *__cdecl SteamAPI_GetSteamInstallPath()
	{
		return NULL;
	}


	__declspec(dllexport) bool __cdecl SteamAPI_Init()
	{
	    NP_SetLogCallback(NP_LogCB);
		NP_Init();

#ifdef KEY_DISABLED
		if (!GAME_FLAG(GAME_FLAG_DEDICATED))
		{
#endif
		if (!NP_Connect(MASTER_SERVER, 3025))
		{
			// TODO: offer offline mode in this case/with an offline gameflag
			Com_Error(1, "Could not connect to NP server at " MASTER_SERVER);
			return false;
		}
#ifdef KEY_DISABLED
		}
#endif

		NPAuthenticateResult* result;

		if (!GAME_FLAG(GAME_FLAG_DEDICATED))
		{
			//NPAsync<NPAuthenticateResult>* async = NP_AuthenticateWithToken(Auth_GetSessionID());
			NPAsync<NPAuthenticateResult>* async = NP_AuthenticateWithLicenseKey("123456789012345678901234");
			result = async->Wait();
		}
		else
		{
			//NPAsync<NPAuthenticateResult>* async = NP_AuthenticateWithLicenseKey("123456789012345678901234");
#ifndef KEY_DISABLED
			//const char* licenseKey = GetLicenseKey();

			Dvar_RegisterString("dw_licensefile", "dwkey.dat", 16, "");

			char licenseKey[26];
			if (!DW_ReadLicenseKey(licenseKey, sizeof(licenseKey)))
			{
				return false;
			}

			licenseKey[24] = '\0';

			NPAsync<NPAuthenticateResult>* async = NP_AuthenticateWithLicenseKey(licenseKey);
			result = async->Wait();
#endif
		}
#ifndef KEY_DISABLED
		if (result->result != AuthenticateResultOK)
		{
			switch (result->result)
			{
			case AuthenticateResultBadDetails:
				Com_Error(1, "Could not authenticate to NP server at " MASTER_SERVER " -- bad details.");
				break;
			case AuthenticateResultAlreadyLoggedIn:
				Com_Error(1, "Could not authenticate to NP server at " MASTER_SERVER " -- already logged in.");
				break;
			case AuthenticateResultServiceUnavailable:
				Com_Error(1, "Could not authenticate to NP server at " MASTER_SERVER " -- service unavailable.");
				break;
			case AuthenticateResultBanned:
				Com_Error(1, "Could not authenticate to NP server at " MASTER_SERVER " -- banned.");
				break;
			case AuthenticateResultUnknown:
				Com_Error(1, "Could not authenticate to NP server at " MASTER_SERVER " -- unknown error.");
				break;
			}
		}
#endif
		NP_RegisterKickCallback(NPA_KickClient);

        return true;
	}

	__declspec(dllexport) bool __cdecl SteamAPI_InitSafe()
	{
		return true;
	}

	__declspec(dllexport) char __cdecl SteamAPI_RestartApp()
	{
		return 1;
	}

	__declspec(dllexport) char __cdecl SteamAPI_RestartAppIfNecessary()
	{
		return 0;
	}

	__declspec(dllexport) void __cdecl SteamAPI_RegisterCallResult( CCallbackBase* pResult, SteamAPICall_t APICall )
	{
		Trace("S_API", "RegisterCallResult for call ID %d", APICall);
		//Callbacks::RegisterResult(pResult, APICall);
		CSteamBase::RegisterCallResult(APICall, pResult);
	}

	__declspec(dllexport) void __cdecl SteamAPI_RegisterCallback( CCallbackBase *pCallback, int iCallback )
	{
		CSteamBase::RegisterCallback(pCallback, iCallback);
		//Callbacks::Register(pCallback, iCallback);
	}
	__declspec(dllexport) void __cdecl SteamAPI_RunCallbacks()
	{
		CSteamBase::RunCallbacks();
		NP_RunFrame();
		//Callbacks::Run();
	}
	__declspec(dllexport) void __cdecl SteamAPI_SetMiniDumpComment( const char *pchMsg )
	{
	}

	__declspec(dllexport) bool __cdecl SteamAPI_SetTryCatchCallbacks( bool bUnknown )
	{
		return bUnknown;
	}
	__declspec(dllexport) void __cdecl SteamAPI_Shutdown()
	{
	}
	__declspec(dllexport) void __cdecl SteamAPI_UnregisterCallResult( CCallbackBase* pResult, SteamAPICall_t APICall )
	{
	}
	__declspec(dllexport) void __cdecl SteamAPI_UnregisterCallback( CCallbackBase *pCallback, int iCallback )
	{
	}

	__declspec(dllexport) void __cdecl SteamAPI_WriteMiniDump( uint32 uStructuredExceptionCode, void* pvExceptionInfo, uint32 uBuildID )
	{
	}

	__declspec(dllexport) ISteamApps* __cdecl SteamApps()
	{
		Trace("S_API", "SteamApps");
		return (ISteamApps*)CSteamBase::GetInterface(INTERFACE_STEAMAPPS003);
		//return (ISteamApps*)g_pSteamAppsEmu;
	}
	__declspec(dllexport) ISteamClient* __cdecl SteamClient()
	{
		Trace("S_API", "SteamClient");
		return NULL;
		//return (ISteamClient*)g_pSteamClientEmu;
	}
	__declspec(dllexport) ISteamContentServer* __cdecl SteamContentServer()
	{
		return NULL;
	}

	__declspec(dllexport) ISteamUtils* __cdecl SteamContentServerUtils()
	{
		return NULL;
	}

	__declspec(dllexport) bool __cdecl SteamContentServer_Init( unsigned int uLocalIP, unsigned short usPort )
	{
		return NULL;
	}

	__declspec(dllexport) void __cdecl SteamContentServer_RunCallbacks()
	{
	}

	__declspec(dllexport) void __cdecl SteamContentServer_Shutdown()
	{
	}

	__declspec(dllexport) ISteamFriends* __cdecl SteamFriends()
	{
		//Trace("S_API", "SteamFriends");
		return (ISteamFriends*)CSteamBase::GetInterface(INTERFACE_STEAMFRIENDS007);
		//return (ISteamFriends*)g_pSteamFriendsEmu;
	}

	__declspec(dllexport) ISteamGameServer* __cdecl SteamGameServer()
	{
		Trace("S_API", "SteamGameServer");
		return (ISteamGameServer*)CSteamBase::GetInterface(INTERFACE_STEAMGAMESERVER010);
		//return (ISteamGameServer*)g_pSteamGameServerEmu;
	}

	__declspec(dllexport) ISteamUtils* __cdecl SteamGameServerUtils()
	{
		return NULL;
	}

	__declspec(dllexport) bool __cdecl SteamGameServer_BSecure()
	{
		return true;
	}

	__declspec(dllexport) HSteamPipe __cdecl SteamGameServer_GetHSteamPipe()
	{
		return NULL;
	}

	__declspec(dllexport) HSteamUser __cdecl SteamGameServer_GetHSteamUser()
	{
		return NULL;
	}

	__declspec(dllexport) int32 __cdecl SteamGameServer_GetIPCCallCount()
	{
		return NULL;
	}

	__declspec(dllexport) uint64 __cdecl SteamGameServer_GetSteamID()
	{
		return NULL;
	}

	__declspec(dllexport) bool __cdecl SteamGameServer_Init( uint32 unIP, uint16 usPort, uint16 usGamePort, EServerMode eServerMode, int nGameAppId, const char *pchGameDir, const char *pchVersionString )
	{
		return true;
	}

	__declspec(dllexport) bool __cdecl SteamGameServer_InitSafe( uint32 unIP, uint16 usPort, uint16 usGamePort, EServerMode eServerMode, int nGameAppId, const char *pchGameDir, const char *pchVersionString, unsigned long dongs )
	{
		return true;
	}

	__declspec(dllexport) void __cdecl SteamGameServer_RunCallbacks()
	{
	}

	__declspec(dllexport) void __cdecl SteamGameServer_Shutdown()
	{
	}

	__declspec(dllexport) ISteamMasterServerUpdater* __cdecl SteamMasterServerUpdater()
	{
		Trace("S_API", "SteamMasterServerUpdater");
		return (ISteamMasterServerUpdater*)CSteamBase::GetInterface(INTERFACE_STEAMMASTERSERVERUPDATER001);
		//return (ISteamMasterServerUpdater*)g_pSteamMasterServerUpdaterEmu;
	}

	__declspec(dllexport) ISteamMatchmaking* __cdecl SteamMatchmaking()
	{
		Trace("S_API", "SteamMatchmaking");
		return (ISteamMatchmaking*)CSteamBase::GetInterface(INTERFACE_STEAMMATCHMAKING007);
		//return (ISteamMatchmaking*)g_pSteamMatchMakingEmu;
	}

	__declspec(dllexport) ISteamMatchmakingServers* __cdecl SteamMatchmakingServers()
	{
		return NULL;
	}

	__declspec(dllexport) ISteamNetworking* __cdecl SteamNetworking()
	{
		//Trace("S_API", "SteamNetworking");
		return (ISteamNetworking*)CSteamBase::GetInterface(INTERFACE_STEAMNETWORKING004);
	}

	__declspec(dllexport) void* __cdecl SteamRemoteStorage()
	{
		//return g_pSteamRemoteStorageEmu;
		return CSteamBase::GetInterface(INTERFACE_STEAMREMOTESTORAGE002);
	}

	__declspec(dllexport) ISteamUser* __cdecl SteamUser()
	{
		//return (ISteamUser*)g_pSteamUserEmu;
		//Trace("S_API", "SteamUser");
		return (ISteamUser*)CSteamBase::GetInterface(INTERFACE_STEAMUSER014);
	}

	__declspec(dllexport) ISteamUserStats* __cdecl SteamUserStats()
	{
		//return (ISteamUserStats*)g_pSteamUStatsEmu;
		Trace("S_API", "SteamUserStats");
		return (ISteamUserStats*)CSteamBase::GetInterface(INTERFACE_STEAMUSERSTATS007);
	}

	__declspec(dllexport) ISteamUtils* __cdecl SteamUtils()
	{
		//return (ISteamUtils*)g_pSteamUtilsEmu;
		Trace("S_API", "SteamUtils");
		return (ISteamUtils*)CSteamBase::GetInterface(INTERFACE_STEAMUTILS005);
	}

	__declspec(dllexport) HSteamUser __cdecl Steam_GetHSteamUserCurrent()
	{
		return NULL;
	}

	__declspec(dllexport) void __cdecl Steam_RegisterInterfaceFuncs( void *hModule )
	{

	}

	__declspec(dllexport) void __cdecl Steam_RunCallbacks( HSteamPipe hSteamPipe, bool bGameServerCallbacks )
	{

	}

	__declspec(dllexport) void *g_pSteamClientGameServer = NULL;
}

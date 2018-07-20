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

void NPA_KickClient(NPID npID, const char* reason)
{
	// determine the clientnum
	/*dvar_t** sv_maxclients = (dvar_t**)0x35891C0;
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
		return;
	}*/

	//Cbuf_AddText(0, va("clientkick %d \"%s\"\n", i, reason));
}

char* Auth_GetSessionID();

void InitializeDediConfig();
void Com_SaveDediConfig();

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
		NP_Init();

		if (!NP_Connect(NP_SERVER, 3036))
		{
			Com_Error(1, "Could not connect to NP server at %s", NP_SERVER);
			return false;
		}

		//NPAsync<NPAuthenticateResult>* async = NP_AuthenticateWithLicenseKey("");
		//async->Wait();

		NPAuthenticateResult* result;

		if (StrStrI(GetCommandLine(), "dedicated") == NULL || StrStrI(GetCommandLine(), "sp.exe"))
		{
			NPAsync<NPAuthenticateResult>* async = NP_AuthenticateWithToken(Auth_GetSessionID());
			result = async->Wait(5000);
		}
		else
		{
			//NPAsync<NPAuthenticateResult>* async = NP_AuthenticateWithLicenseKey("123456789012345678901234");
#ifndef KEY_DISABLED
			/*const char* licenseKey = GetLicenseKey();

			if (!licenseKey)
			{
				Com_Error(1, "No license key set. Pass a license key in the command line of the application (like iw4.exe #123456789012345678901234).");
			}*/

			dvar_t* licenseKey = Dvar_RegisterString("np_licenseKey", "", DVAR_FLAG_DEDISAVED, "NP dedicated server license key");
			dvar_t* licenseID = Dvar_RegisterString("np_licenseID", "", DVAR_FLAG_DEDISAVED, "NP dedicated server ID");

			// as the registration functions only allow up to 16 bits of flags
			licenseID->flags |= DVAR_FLAG_DEDISAVED;
			licenseKey->flags |= DVAR_FLAG_DEDISAVED;

			InitializeDediConfig();

			bool justRegistered = false;

			if (!licenseKey->current.string[0])
			{
				NPAsync<NPRegisterServerResult>* regAsync = NP_RegisterServer("iw4m ftw! hey guys, it's me, the cake hero! cake is lovely and tasty! eat many cakes in your life! bye!");
				NPRegisterServerResult* regResult = regAsync->Wait();

				if (regResult->result != AuthenticateResultOK)
				{
					switch (result->result)
					{
					case AuthenticateResultBadDetails:
						Com_Error(1, "Could not register to NP server at " NP_SERVER " -- bad details.");
						break;
					case AuthenticateResultServiceUnavailable:
						Com_Error(1, "Could not register to NP server at " NP_SERVER " -- service unavailable.");
						break;
					case AuthenticateResultBanned:
						Com_Error(1, "Could not register to NP server at " NP_SERVER " -- banned.");
						break;
					case AuthenticateResultUnknown:
						Com_Error(1, "Could not register to NP server at " NP_SERVER " -- unknown error.");
						break;
					}
				}

				Dvar_SetCommand("np_licenseKey", regResult->licenseKey);
				Dvar_SetCommand("np_licenseID", va("%i", regResult->serverID));

				justRegistered = true;
			}

			NPAsync<NPAuthenticateResult>* async = NP_AuthenticateWithLicenseKey(licenseKey->current.string);
			result = async->Wait();

			if (justRegistered)
			{
				Com_SaveDediConfig();
			}

			//dvar_t* net_port = Dvar_FindVar("net_port");
			//NP_SendRandomString(va("port %i", net_port->current.integer));
#endif
		}
#ifndef KEY_DISABLED

		if (!result)
		{
			Com_Error(1, "Could not authenticate to NP server at " NP_SERVER " -- operation timed out. Please try again later.");
		}

		if (result->result != AuthenticateResultOK)
		{
			switch (result->result)
			{
			case AuthenticateResultBadDetails:
				if (!StrStrI(GetCommandLine(), "sp.exe"))
				{
					Dvar_SetCommand("np_licenseKey", "");

					if (StrStrI(GetCommandLine(), "dedicated") != NULL) // hah, copypaste!
					{
						Com_SaveDediConfig();
					}
				}

				Com_Error(1, "Could not authenticate to NP server at " NP_SERVER " -- bad details.");
				break;
			case AuthenticateResultAlreadyLoggedIn:
				Com_Error(1, "Could not authenticate to NP server at " NP_SERVER " -- already logged in.");
				break;
			case AuthenticateResultServiceUnavailable:
				Com_Error(1, "Could not authenticate to NP server at " NP_SERVER " -- service unavailable.");
				break;
			case AuthenticateResultBanned:
				Com_Error(1, "Could not authenticate to NP server at " NP_SERVER " -- banned.");
				break;
			case AuthenticateResultUnknown:
				Com_Error(1, "Could not authenticate to NP server at " NP_SERVER " -- unknown error.");
				break;
			}
		}
#endif

		NP_SetRichPresence("currentGame", (!strstr(GetCommandLine(), "sp.exe") ? "t5mp" : "t5sp"));
		NP_StoreRichPresence();
		//NP_RegisterKickCallback(NPA_KickClient);

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

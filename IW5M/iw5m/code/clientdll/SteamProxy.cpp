// ==========================================================
// alterIWnet project
// 
// Component: aiw_client
// Sub-component: steam_api
// Purpose: Steam proxying functionality
//
// Initial author: NTAuthority
// Started: 2011-11-05
// ==========================================================

#include "StdInc.h"
#include <string>
#include "Interface_OSW.h"

static CSteamAPILoader loader;

struct
{
	CreateInterfaceFn clientFactory;
	HSteamPipe pipe;
	HSteamUser user;

	ISteamClient011* steamClient;

	bool initialized;	

	EExternalAuthState eaState;
	int eaCall;
} g_steamProxy;

bool SteamProxy_InitInternal()
{
	g_steamProxy.clientFactory = loader.Load();

	if (g_steamProxy.clientFactory == NULL)
	{
		return false;
	}

	g_steamProxy.steamClient = (ISteamClient011*)g_steamProxy.clientFactory(STEAMCLIENT_INTERFACE_VERSION_011, NULL);

	if (g_steamProxy.steamClient == NULL)
	{
		return false;
	}

	g_steamProxy.pipe = g_steamProxy.steamClient->CreateSteamPipe();

	if (g_steamProxy.pipe == NULL)
	{
		return false;
	}

	g_steamProxy.user = g_steamProxy.steamClient->ConnectToGlobalUser(g_steamProxy.pipe);

	if (g_steamProxy.user == NULL)
	{
		return false;
	}

	ISteamUtils005* steamUtils = (ISteamUtils005*)g_steamProxy.steamClient->GetISteamUtils(g_steamProxy.pipe, STEAMUTILS_INTERFACE_VERSION_005);
	int appID = steamUtils->GetAppID();

	if (appID == 0)
	{
		return false;
	}

	ISteamUser016* steamUser = (ISteamUser016*)g_steamProxy.steamClient->GetISteamUser(g_steamProxy.user, g_steamProxy.pipe, STEAMUSER_INTERFACE_VERSION_016);
	CSteamID steamID = steamUser->GetSteamID();

	NP_SetExternalSteamID(steamID.ConvertToUint64());

	return true;
}

ISteamMatchmakingServers002* SteamProxy_GetSteamMatchmakingServers()
{
	ISteamMatchmakingServers002* mms = (ISteamMatchmakingServers002*)g_steamProxy.steamClient->GetISteamMatchmakingServers(g_steamProxy.user, g_steamProxy.pipe, STEAMMATCHMAKINGSERVERS_INTERFACE_VERSION_002);

	return mms;
}

SteamAPICall_t requestEncryptedAppTicket_call;
SteamAPICall_t ourRequestEncryptedAppTicket_call;

void SteamProxy_RunFrame()
{
	if (!g_steamProxy.initialized)
	{
		return;
	}

	ISteamUtils005* steamUtils = (ISteamUtils005*)g_steamProxy.steamClient->GetISteamUtils(g_steamProxy.pipe, STEAMUTILS_INTERFACE_VERSION_005);
	steamUtils->RunFrame();

	if (requestEncryptedAppTicket_call)
	{
		bool failed = false;
		if (steamUtils->IsAPICallCompleted(requestEncryptedAppTicket_call, &failed))
		{
			static EncryptedAppTicketResponse_t response;
			if (steamUtils->GetAPICallResult(requestEncryptedAppTicket_call, &response, sizeof(response), EncryptedAppTicketResponse_t::k_iCallback, &failed))
			{
				CSteamBase::ReturnCall(&response, sizeof(EncryptedAppTicketResponse_t), 0, ourRequestEncryptedAppTicket_call);
				requestEncryptedAppTicket_call = 0;
			}
		}
	}
}

SteamAPICall_t SteamProxy_RequestEncryptedAppTicket(void *pUserData, int cbUserData)
{
	OutputDebugString("SECRETSAUCE FOLLOWS\n");
	OutputDebugString((char*)pUserData);
	ISteamUser016* steamUser = (ISteamUser016*)g_steamProxy.steamClient->GetISteamUser(g_steamProxy.user, g_steamProxy.pipe, STEAMUSER_INTERFACE_VERSION_016);

	ourRequestEncryptedAppTicket_call = CSteamBase::RegisterCall();
	requestEncryptedAppTicket_call = steamUser->RequestEncryptedAppTicket(pUserData, cbUserData);

	return ourRequestEncryptedAppTicket_call;
}

bool SteamProxy_GetEncryptedAppTicket(void *pTicket, int cbMaxTicket, uint32 *pcbTicket)
{
	ISteamUser016* steamUser = (ISteamUser016*)g_steamProxy.steamClient->GetISteamUser(g_steamProxy.user, g_steamProxy.pipe, STEAMUSER_INTERFACE_VERSION_016);
	return steamUser->GetEncryptedAppTicket(pTicket, cbMaxTicket, pcbTicket);
}

void SteamProxy_LoadGameOverlayRenderer()
{
	if (GetModuleHandle("gameoverlayrenderer.dll") != NULL)
	{
		return;
	}

	std::string str(loader.GetSteamDir());
	std::string gameOverlayRendererPath = str + "\\gameoverlayrenderer.dll";
	LoadLibrary(gameOverlayRendererPath.c_str());
}

void SteamProxy_Init()
{
	SetEnvironmentVariable("SteamAppId", "42690");

	FILE* file = fopen("steam_appid.txt", "w");
	if (file)
	{
		fprintf(file, "42690");
		fclose(file);
	}

	g_steamProxy.initialized = SteamProxy_InitInternal();

	if (g_steamProxy.initialized)
	{
		SteamProxy_LoadGameOverlayRenderer();
	}
}

void NPA_StateSet(EExternalAuthState result)
{
	g_steamProxy.eaState = result;
	g_steamProxy.eaCall++;
}

void SteamProxy_DoThatTwinklyStuff()
{
	if (!g_isDedicated)
	{
		if (GetFileAttributes("dw") != INVALID_FILE_ATTRIBUTES)
		{
			Trace("SteamProxy", "Skipping Steam authentication.");
			return;
		}

		Com_Printf(0, "Performing Steam authentication...\n");
		Trace("SteamProxy", "Performing Steam authentication...");

		// wait until some non-unverified result is returned
		int startTime = GetTickCount();

		while (g_steamProxy.eaState == ExternalAuthStateUnverified)
		{
			Sleep(1);
			NP_RunFrame();

			if ((g_steamProxy.eaCall == 2 && !g_steamProxy.initialized) || ((GetTickCount() - startTime) > 10000)) // if on the second call, and we're not running Steam
			{
				Com_Error(1, "Steam must be running for initial authentication to succeed. Please start Steam and restart the game to continue.");
			}
		}

		// do stuff based on verification result
		if (g_steamProxy.eaState == ExternalAuthStatePirate)
		{
			Com_Error(1, "Your current Steam account does not own Call of Duty: Modern Warfare 3. Please purchase the game, try another Steam account or make a folder called 'dw'.");
		}
		else if (g_steamProxy.eaState == ExternalAuthStateError)
		{
			Com_Error(1, "An error occurred during Steam verification. Please try again later. If this issue persists, ask on http://fourdeltaone.net/.");
		}
		else if (g_steamProxy.eaState == ExternalAuthStatePrivate)
		{
			Com_Error(1, "Your Steam Community profile is inaccessible. Please go to http://steamcommunity.com/, edit your profile and select 'Public' to continue.");
		}

		// guess the twinkles were right, you really do own the game!
		Com_Printf(0, "Steam authentication passed.\n");
		Trace("SteamProxy", "Steam authentication passed.");
	}	
}
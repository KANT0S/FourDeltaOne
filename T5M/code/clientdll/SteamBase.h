// ==========================================================
// alterIWnet project
// 
// Component: aiw_client
// Sub-component: steam_api
// Purpose: Basic Steam interface functions for usage in main
//
// Initial author: NTAuthority
// Started: 2010-09-10
// ==========================================================

#pragma once

// all interface IDs supported
typedef enum SteamInterface_t {
	INTERFACE_STEAMCLIENT008,
	INTERFACE_STEAMUSER014,
	INTERFACE_STEAMREMOTESTORAGE002,
	INTERFACE_STEAMUTILS005,
	INTERFACE_STEAMNETWORKING004,
	INTERFACE_STEAMFRIENDS007,
	INTERFACE_STEAMAPPS003,
	INTERFACE_STEAMMATCHMAKING007,
	INTERFACE_STEAMGAMESERVER010,
	INTERFACE_STEAMMASTERSERVERUPDATER001,
	INTERFACE_STEAMUSERSTATS007
};

// result type
typedef struct SteamAPIResult_s
{
	void* data;
	int size;
	int type;
	SteamAPICall_t call;
} SteamAPIResult_t;

// basic class
class CSteamBase {
private:
	static std::map<SteamInterface_t, void*> _instances;
	static std::map<SteamAPICall_t, bool> _calls;
	static std::map<SteamAPICall_t, CCallbackBase*> _resultHandlers;
	static std::vector<SteamAPIResult_t> _results;
	static std::vector<CCallbackBase*> _callbacks;

	static int _callID;
private:
	static void* CreateInterface(SteamInterface_t interfaceID);
public:
	// get interface instance from identifier
	static void* GetInterface(SteamInterface_t interfaceID);

	// run callbacks
	static void RunCallbacks();

	// register a global callback
	static void RegisterCallback(CCallbackBase* handler, int callback);

	// register a call result
	static void RegisterCallResult(SteamAPICall_t call, CCallbackBase* result);

	// register a callback
	static SteamAPICall_t RegisterCall();

	// return a callback
	static void ReturnCall(void* data, int size, int type, SteamAPICall_t call);
};
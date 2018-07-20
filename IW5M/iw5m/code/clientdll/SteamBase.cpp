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

#include "StdInc.h"
#include "SteamRemoteStorage002.h"
#include "SteamUser016.h"
#include "SteamUtils005.h"
#include "SteamUserStats010.h"
#include "SteamNetworking005.h"
#include "SteamFriends009.h"
#include "SteamApps004.h"
#include "SteamMatchmaking008.h"
#include "SteamGameServer010.h"
#include "SteamMasterServerUpdater001.h"
#include "SteamMatchmakingServers002.h"

// define so the template can be compiled
std::map<SteamInterface_t, void*> CSteamBase::_instances;
std::map<SteamAPICall_t, bool> CSteamBase::_calls;
std::map<SteamAPICall_t, CCallbackBase*> CSteamBase::_resultHandlers;
std::vector<SteamAPIResult_t> CSteamBase::_results;
std::vector<CCallbackBase*> CSteamBase::_callbacks;
int CSteamBase::_callID;

void* CSteamBase::CreateInterface(SteamInterface_t interfaceID)
{
	// even though this might be done nicer, I'm doing it using a switch statement now
	switch (interfaceID)
	{
		case INTERFACE_STEAMREMOTESTORAGE002:
 			return new CSteamRemoteStorage002;
 		case INTERFACE_STEAMUTILS005:
 			return new CSteamUtils005;
 		case INTERFACE_STEAMNETWORKING005:
 			return new CSteamNetworking005;
		case INTERFACE_STEAMAPPS004:
			return new CSteamApps004;
 		case INTERFACE_STEAMFRIENDS009:
 			return new CSteamFriends009;
 		case INTERFACE_STEAMMATCHMAKING008:
 			return new CSteamMatchmaking008;
 		case INTERFACE_STEAMGAMESERVER010:
 			return new CSteamGameServer010;
 		case INTERFACE_STEAMMASTERSERVERUPDATER001:
 			return new CSteamMasterServerUpdater001;
		case INTERFACE_STEAMUSER016:
			return new CSteamUser016;
		case INTERFACE_STEAMUSERSTATS010:
			return new CSteamUserStats010;
		case INTERFACE_STEAMMATCHMAKINGSERVERS002:
			return new CSteamMatchmakingServers002;
		/*case INTERFACE_STEAMCLIENT008:
			return new CSteamClient008;
			break;*/
	}

	return NULL;
}

void* CSteamBase::GetInterface(SteamInterface_t interfaceID)
{
	// note the WTF in std::map - I still prefer the BCL
	if (_instances.find(interfaceID) == _instances.end())
	{
		// not found yet, so create it
		_instances[interfaceID] = CreateInterface(interfaceID);
	}

	return _instances[interfaceID];
}

void CSteamBase::RegisterCallback(CCallbackBase *handler, int callback)
{
	handler->SetICallback(callback);
	_callbacks.push_back(handler);
}

void CSteamBase::RegisterCallResult(SteamAPICall_t call, CCallbackBase *result)
{
	_resultHandlers[call] = result;
}

SteamAPICall_t CSteamBase::RegisterCall()
{
	_callID++;

	_calls[_callID] = false;
	
	return _callID;
}

void CSteamBase::ReturnCall(void *data, int size, int type, SteamAPICall_t call)
{
	SteamAPIResult_t result;

	_calls[call] = true;

	result.call = call;
	result.data = data;
	result.size = size;
	result.type = type;

	_results.push_back(result);
}

void CSteamBase::RunCallbacks()
{
	std::vector<SteamAPIResult_t>::iterator iter;

	for (iter = _results.begin(); iter < _results.end(); iter++)
	{
		SteamAPIResult_t result = *iter;

		if (_resultHandlers.find(result.call) != _resultHandlers.end())
		{
			_resultHandlers[result.call]->Run(result.data, false, result.call);
		}

		std::vector<CCallbackBase*>::iterator cbiter;

		for (cbiter = _callbacks.begin(); cbiter < _callbacks.end(); cbiter++)
		{
			CCallbackBase* cb = *cbiter;

			if (cb && cb->GetICallback() == result.type && result.type != 0)
			{
				cb->Run(result.data, false, 0);
			}
		}

		free(result.data);
	}

	_results.clear();
}
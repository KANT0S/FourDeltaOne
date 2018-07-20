// ==========================================================
// alterIWnet project
// 
// Component: xnp
// Sub-component: libnp
// Purpose: profile methods
//
// Initial author: NTAuthority
// Started: 2012-06-12
// ==========================================================

#include "StdInc.h"

struct np_get_profile_data_state_s
{
	NPAsyncImpl<NPGetProfileDataResult>* asyncResult;
	uint32_t numIDs;
	NPProfileData* buffer;
};

struct RPCProfileData
{
	int32_t experience;
	int32_t prestige;
};

static void GetProfileDataCB(NPAsync<INPRPCMessage>* async)
{
	np_get_profile_data_state_s* state = (np_get_profile_data_state_s*)async->GetUserData();
	RPCFriendsGetProfileDataResultMessage* rpcResult = (RPCFriendsGetProfileDataResultMessage*)async->GetResult();

	// make a result object
	NPGetProfileDataResult* result = new NPGetProfileDataResult();
	result->numResults = rpcResult->GetBuffer()->results_size();
	result->results = state->buffer;

	// add the results to the buffer
	for (uint32_t i = 0; i < result->numResults; i++)
	{
		ProfileDataResult result = rpcResult->GetBuffer()->results(i);
		RPCProfileData rpcData;
		memcpy(&rpcData, result.profile().c_str(), sizeof(rpcData));

		NPProfileData profileData;
		profileData.npID = result.npid();
		profileData.experience = rpcData.experience;
		profileData.prestige = rpcData.prestige;

		memcpy(&state->buffer[i], &profileData, sizeof(profileData));
	}

	// and handle the async result
	state->asyncResult->SetResult(result);
	delete state;
}

LIBNP_API NPAsync<NPGetProfileDataResult>* LIBNP_CALL NP_GetProfileData(uint32_t numIDs, const NPID* npIDs, NPProfileData* outData)
{
	RPCFriendsGetProfileDataMessage* request = new RPCFriendsGetProfileDataMessage();

	for (uint32_t i = 0; i < numIDs; i++)
	{
		request->GetBuffer()->add_npids(npIDs[i]);
	}

	NPAsync<INPRPCMessage>* async = RPC_SendMessageAsync(request);
	NPAsyncImpl<NPGetProfileDataResult>* result = new NPAsyncImpl<NPGetProfileDataResult>();

	np_get_profile_data_state_s* state = new np_get_profile_data_state_s();
	state->numIDs = numIDs;
	state->buffer = outData;
	state->asyncResult = result;

	async->SetCallback(GetProfileDataCB, state);

	request->Free();

	return result;
}
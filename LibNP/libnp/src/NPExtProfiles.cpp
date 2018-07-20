// ==========================================================
// alterIWnet project
// 
// Component: xnp
// Sub-component: libnp
// Purpose: profile methods
//
// Initial author: NTAuthority
// Started: 2013-08-20
// ==========================================================

#include "StdInc.h"

struct np_get_profile_data_state_s
{
	NPAsyncImpl<NPGetExtProfileDataResult>* asyncResult;
	uint32_t numIDs;
	NPExtProfileData* buffer;
};

static void GetProfileDataCB(NPAsync<INPRPCMessage>* async)
{
	np_get_profile_data_state_s* state = (np_get_profile_data_state_s*)async->GetUserData();
	RPCFriendsGetProfileDataResultMessage* rpcResult = (RPCFriendsGetProfileDataResultMessage*)async->GetResult();

	// make a result object
	NPGetExtProfileDataResult* result = new NPGetExtProfileDataResult();
	result->numResults = rpcResult->GetBuffer()->results_size();
	result->results = state->buffer;

	// add the results to the buffer
	for (uint32_t i = 0; i < result->numResults; i++)
	{
		ProfileDataResult result = rpcResult->GetBuffer()->results(i);

		NPExtProfileData profileData;
		profileData.npID = result.npid();
		profileData.bufLength = min(result.profile().size(), sizeof(profileData.buffer));

		memcpy(&state->buffer[i], &profileData, sizeof(NPID) + sizeof(size_t));
		memcpy(state->buffer[i].buffer, result.profile().c_str(), profileData.bufLength);
	}

	// and handle the async result
	state->asyncResult->SetResult(result);
	delete state;
}

LIBNP_API NPAsync<NPGetExtProfileDataResult>* LIBNP_CALL NP_GetExtProfileData(uint32_t numIDs, const char* profileType, const NPID* npIDs, NPExtProfileData* outData)
{
	RPCFriendsGetProfileDataMessage* request = new RPCFriendsGetProfileDataMessage();

	for (uint32_t i = 0; i < numIDs; i++)
	{
		request->GetBuffer()->add_npids(npIDs[i]);
	}

	request->GetBuffer()->set_profiletype(profileType);

	NPAsync<INPRPCMessage>* async = RPC_SendMessageAsync(request);
	NPAsyncImpl<NPGetExtProfileDataResult>* result = new NPAsyncImpl<NPGetExtProfileDataResult>();

	np_get_profile_data_state_s* state = new np_get_profile_data_state_s();
	state->numIDs = numIDs;
	state->buffer = outData;
	state->asyncResult = result;

	async->SetCallback(GetProfileDataCB, state);

	request->Free();

	return result;
}
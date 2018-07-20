// ==========================================================
// alterIWnet project
// 
// Component: xnp
// Sub-component: libnp
// Purpose: server list methods
//
// Initial author: NTAuthority
// Started: 2012-05-10
// ==========================================================

#include "StdInc.h"

// --- SESSION CREATION -------------------------------------
static void CreateSessionCB(NPAsync<INPRPCMessage>* async)
{
	NPAsyncImpl<NPCreateSessionResult>* asyncResult = (NPAsyncImpl<NPCreateSessionResult>*)async->GetUserData();
	RPCServersCreateSessionResultMessage* result = (RPCServersCreateSessionResultMessage*)async->GetResult();

	NPCreateSessionResult* sessionResult = new NPCreateSessionResult();
	sessionResult->result = (EServersResult)result->GetBuffer()->result();

	if (sessionResult->result == ServersResultOK)
	{
		sessionResult->sid = result->GetBuffer()->sessionid();
	}

	asyncResult->SetResult(sessionResult);
}

LIBNP_API NPAsync<NPCreateSessionResult>* LIBNP_CALL NP_CreateSession(NPSessionInfo* data)
{
	RPCServersCreateSessionMessage* request = new RPCServersCreateSessionMessage();
	SessionInfo* sessionInfo = request->GetBuffer()->mutable_info();
	sessionInfo->set_hostname(data->hostname);
	sessionInfo->set_mapname(data->mapname);
	sessionInfo->set_players(data->players);
	sessionInfo->set_maxplayers(data->maxplayers);
	sessionInfo->set_version(data->version);
	
	NPAsync<INPRPCMessage>* async = RPC_SendMessageAsync(request);
	NPAsyncImpl<NPCreateSessionResult>* result = new NPAsyncImpl<NPCreateSessionResult>();
	async->SetCallback(CreateSessionCB, result);

	request->Free();

	return result;
}

// --- SESSION UPDATING -------------------------------------
static void UpdateSessionCB(NPAsync<INPRPCMessage>* async)
{
	NPAsyncImpl<EServersResult>* asyncResult = (NPAsyncImpl<EServersResult>*)async->GetUserData();
	RPCServersUpdateSessionResultMessage* result = (RPCServersUpdateSessionResultMessage*)async->GetResult();

	EServersResult* sessionResult = new EServersResult((EServersResult)result->GetBuffer()->result());
	
	asyncResult->SetResult(sessionResult);
}

LIBNP_API NPAsync<EServersResult>* LIBNP_CALL NP_UpdateSession(NPSID sid, const char* hostname, const char* mapname, int16_t curPlayers, int16_t maxPlayers)
{
	RPCServersUpdateSessionMessage* request = new RPCServersUpdateSessionMessage();
	request->GetBuffer()->set_sessionid(sid);
	request->GetBuffer()->set_hostname(hostname);
	request->GetBuffer()->set_mapname(mapname);
	request->GetBuffer()->set_players(curPlayers);
	request->GetBuffer()->set_maxplayers(maxPlayers);

	NPAsync<INPRPCMessage>* async = RPC_SendMessageAsync(request);
	NPAsyncImpl<EServersResult>* result = new NPAsyncImpl<EServersResult>();
	async->SetCallback(UpdateSessionCB, result);

	request->Free();

	return result;
}

LIBNP_API NPAsync<EServersResult>* LIBNP_CALL NP_DeleteSession(NPSID sid)
{
	RPCServersDeleteSessionMessage* request = new RPCServersDeleteSessionMessage();
	request->GetBuffer()->set_sessionid(sid);

	NPAsync<INPRPCMessage>* async = RPC_SendMessageAsync(request);
	NPAsyncImpl<EServersResult>* result = new NPAsyncImpl<EServersResult>();
	async->SetCallback(UpdateSessionCB, result);

	request->Free();

	return result;
}

// --- SESSION RETRIEVING -----------------------------------
static NPSessionInfo sessions[512];
static int numSessions;

static void GetSessionsCB(NPAsync<INPRPCMessage>* async)
{
	NPAsyncImpl<bool>* asyncResult = (NPAsyncImpl<bool>*)async->GetUserData();
	RPCServersGetSessionsResultMessage* result = (RPCServersGetSessionsResultMessage*)async->GetResult();

	bool* sessionResult = new bool;
	*sessionResult = true;

	memset(sessions, 0, sizeof(sessions));
	numSessions = 0;

	ServersGetSessionsResultMessage* buffer = result->GetBuffer();
	int size = buffer->servers_size();

	for (int i = 0; i < size; i++)
	{
		if (i >= 8192)
		{
			break;
		}

		SessionInfo info = buffer->servers(i);
		sessions[i].address = info.address();
		sessions[i].port = info.port();
		sessions[i].maxplayers = info.maxplayers();
		sessions[i].players = info.players();
		sessions[i].version = info.version();
		sessions[i].npid = info.npid();

		strncpy(sessions[i].mapname, info.mapname().c_str(), sizeof(sessions[i].mapname) - 1);
		strncpy(sessions[i].hostname, info.hostname().c_str(), sizeof(sessions[i].mapname) - 1);

		numSessions++;
	}

	asyncResult->SetResult(sessionResult);
}

LIBNP_API NPAsync<bool>* LIBNP_CALL NP_RefreshSessions(const char* tags)
{
	RPCServersGetSessionsMessage* request = new RPCServersGetSessionsMessage();
	request->GetBuffer()->set_tags(tags);

	NPAsync<INPRPCMessage>* async = RPC_SendMessageAsync(request);
	NPAsyncImpl<bool>* result = new NPAsyncImpl<bool>();
	async->SetCallback(GetSessionsCB, result);

	request->Free();

	return result;
}

LIBNP_API int32_t LIBNP_CALL NP_GetNumSessions()
{
	return numSessions;
}

LIBNP_API void LIBNP_CALL NP_GetSessionData(int32_t index, NPSessionInfo* out)
{
	if (index >= 8192 || index < 0)
	{
		return;
	}

	memcpy(out, &sessions[index], sizeof(NPSessionInfo));
}
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

// --- DICTIONARY HANDLING ----------------------------------
static void DictionaryToProtobuf(NPSessionInfo* data, SessionInfo* proto)
{
	for (NPKeyValuePair& pair : data->data)
	{
		SessionData* ddata = proto->add_data();
		ddata->set_key(pair.key);
		ddata->set_value(pair.value);
	}
}

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
	NPID npID;
	NP_GetNPID(&npID);

	RPCServersCreateSessionMessage* request = new RPCServersCreateSessionMessage();
	SessionInfo* sessionInfo = request->GetBuffer()->mutable_info();
	sessionInfo->set_players(data->players);
	sessionInfo->set_maxplayers(data->maxplayers);
	sessionInfo->set_address(data->address);
	sessionInfo->set_npid(npID);
	sessionInfo->set_port(data->port);

	// move the dictionary to protobuf
	DictionaryToProtobuf(data, sessionInfo);
	
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

LIBNP_API NPAsync<EServersResult>* LIBNP_CALL NP_UpdateSession(NPSID sid, NPSessionInfo* data)
{
	RPCServersUpdateSessionMessage* request = new RPCServersUpdateSessionMessage();
	request->GetBuffer()->set_sessionid(sid);

	NPID npID;
	NP_GetNPID(&npID);
	
	SessionInfo* sessionInfo = request->GetBuffer()->mutable_info();
	sessionInfo->set_players(data->players);
	sessionInfo->set_maxplayers(data->maxplayers);
	sessionInfo->set_address(data->address);
	sessionInfo->set_npid(npID);
	sessionInfo->set_port(data->port);

	// move the dictionary to protobuf
	DictionaryToProtobuf(data, sessionInfo);
	
	NPAsync<INPRPCMessage>* async = RPC_SendMessageAsync(request);
	NPAsyncImpl<EServersResult>* result = new NPAsyncImpl<EServersResult>();
	async->SetCallback(UpdateSessionCB, result);

	request->Free();

	return result;
}

static void DeleteSessionCB(NPAsync<INPRPCMessage>* async)
{
	NPAsyncImpl<EServersResult>* asyncResult = (NPAsyncImpl<EServersResult>*)async->GetUserData();
	RPCServersDeleteSessionResultMessage* result = (RPCServersDeleteSessionResultMessage*)async->GetResult();

	EServersResult* sessionResult = new EServersResult((EServersResult)result->GetBuffer()->result());

	asyncResult->SetResult(sessionResult);
}

LIBNP_API NPAsync<EServersResult>* LIBNP_CALL NP_DeleteSession(NPSID sid)
{
	RPCServersDeleteSessionMessage* request = new RPCServersDeleteSessionMessage();
	request->GetBuffer()->set_sessionid(sid);

	NPAsync<INPRPCMessage>* async = RPC_SendMessageAsync(request);
	NPAsyncImpl<EServersResult>* result = new NPAsyncImpl<EServersResult>();
	async->SetCallback(DeleteSessionCB, result);

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
		sessions[i].sid = info.sid();
		sessions[i].address = info.address();
		sessions[i].port = info.port();
		sessions[i].maxplayers = info.maxplayers();
		sessions[i].players = info.players();
		//sessions[i].version = info.version();
		sessions[i].npid = info.npid();

		sessions[i].data.~NPDictionary();
		sessions[i].data.Init();

		for (int j = 0; j < info.data_size(); j++)
		{
			auto sKVP = info.data(j);

			sessions[i].data.Set(sKVP.key().c_str(), sKVP.value().c_str());
		}

		//strncpy(sessions[i].mapname, info.mapname().c_str(), sizeof(sessions[i].mapname) - 1);
		//strncpy(sessions[i].hostname, info.hostname().c_str(), sizeof(sessions[i].mapname) - 1);

		numSessions++;
	}

	asyncResult->SetResult(sessionResult);
}

LIBNP_API NPAsync<bool>* LIBNP_CALL NP_RefreshSessions(NPDictionary& infos)
{
	RPCServersGetSessionsMessage* request = new RPCServersGetSessionsMessage();
	//request->GetBuffer()->set_tags(tags);

	for (NPKeyValuePair& pair : infos)
	{
		SessionData* ddata = request->GetBuffer()->add_infos();
		ddata->set_key(pair.key);
		ddata->set_value(pair.value);
	}

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

	out->data.~NPDictionary();
	//memcpy(out, &sessions[index], sizeof(NPSessionInfo));
	*out = sessions[index];
	out->data.AddRef();
}
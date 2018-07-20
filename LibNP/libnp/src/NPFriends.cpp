// ==========================================================
// alterIWnet project
// 
// Component: xnp
// Sub-component: libnp
// Purpose: friends methods
//
// Initial author: NTAuthority
// Started: 2011-11-10
// ==========================================================

#include "StdInc.h"
#include <tomcrypt.h>
#include <strophe.h>

#include <google/dense_hash_map>

#define MAX_FRIENDS 1024

static struct  
{
	// connected?
	bool connected;

	// roster
	google::dense_hash_map<NPID, FriendState> registeredFriends;
	NPID roster[MAX_FRIENDS];
	uint32_t rosterIndex;

	// local rich presence state
	std::map<std::string, std::string> richPresenceProps;
	char richPresenceBody[256];
} g_friends;

void Friends_ReceivedRoster(INPRPCMessage* message)
{
	RPCFriendsRosterMessage* result = (RPCFriendsRosterMessage*)message;

	for (int i = 0; i < result->GetBuffer()->friends_size(); i++)
	{
		FriendDetails details = result->GetBuffer()->friends(i);

		std::string name = details.name();

		FriendState ritem;
		strncpy(ritem.name, name.c_str(), sizeof(ritem.name));
		ritem.presence = PresenceOffline;
		ritem.npID = details.npid();

		g_friends.registeredFriends[ritem.npID] = ritem;

		g_friends.roster[g_friends.rosterIndex] = ritem.npID;
		g_friends.rosterIndex++;
	}

	g_friends.connected = true;
}

// sorts the roster
static int Friends_SortRosterCompare(const void* left, const void* right)
{
	NPID leftID = *(NPID*)left;
	NPID rightID = *(NPID*)right;

	int presenceLeft = g_friends.registeredFriends[leftID].presence;
	int presenceRight = g_friends.registeredFriends[rightID].presence;

	if (presenceLeft == presenceRight)
	{
		return _stricmp(g_friends.registeredFriends[leftID].name, g_friends.registeredFriends[rightID].name);
	}

	return (presenceRight - presenceLeft);
}

static void Friends_SortRoster()
{
	qsort(g_friends.roster, g_friends.rosterIndex, sizeof(NPID), Friends_SortRosterCompare);
}

void Friends_ReceivedPresence(INPRPCMessage* message)
{
	RPCFriendsPresenceMessage* result = (RPCFriendsPresenceMessage*)message;
	NPID npID = result->GetBuffer()->friend_();

	FriendState ritem = g_friends.registeredFriends[npID];

	Presence newPresence = PresenceOnline;

	if (result->GetBuffer()->presencestate() == 0)
	{
		newPresence = PresenceOffline;
	}

	ritem.presence = newPresence;
	ritem.richPresence.clear();

	for (int i = 0; i < result->GetBuffer()->presence_size(); i++)
	{
		FriendsPresence rpres = result->GetBuffer()->presence(i);

		ritem.richPresence[rpres.key()] = rpres.value();
	}

	strcpy_s(ritem.richPresenceString, sizeof(ritem.richPresenceString), ritem.richPresence["__body"].c_str());

	g_friends.registeredFriends[npID] = ritem;

	Friends_SortRoster();
}

void Friends_Init()
{
	// initialize the hash map
	g_friends.registeredFriends.set_empty_key(0);

	// initial state
	g_friends.connected = false;
	g_friends.rosterIndex = 0;

	// register dispatchers
	RPC_RegisterDispatch(RPCFriendsRosterMessage::Type, &Friends_ReceivedRoster);
	RPC_RegisterDispatch(RPCFriendsPresenceMessage::Type, &Friends_ReceivedPresence);
}

void Friends_Shutdown()
{

}

void Friends_Connect()
{

}

// ----------------
// friends API bits
// ----------------

LIBNP_API bool LIBNP_CALL NP_FriendsConnected()
{
	return g_friends.connected;
}

LIBNP_API uint32_t LIBNP_CALL NP_GetNumFriends()
{
	if (!g_friends.connected)
	{
		return 0;
	}

	return g_friends.rosterIndex;
}

LIBNP_API NPID LIBNP_CALL NP_GetFriend(int32_t index)
{
	if (!g_friends.connected)
	{
		return 0;
	}

	return g_friends.roster[index];
}

LIBNP_API const char* LIBNP_CALL NP_GetFriendName(NPID npID)
{
	if (!g_friends.connected)
	{
		return "Marvelous. Just marvelous. Right when I tried to enter my house, someone throws a rock right through my #$^%^% window. It could have hit me, for all I'd know. Anyway, what were you asking?";
	}

	if (g_friends.registeredFriends[npID].npID != npID)
	{
		return NULL;
	}

	return g_friends.registeredFriends[npID].name;
}

LIBNP_API EPresenceState LIBNP_CALL NP_GetFriendPresence(NPID npID)
{
	if (!g_friends.connected)
	{
		// we can just say anything here, nobody will care, except for the party thing
		return PresenceStateExtendedAway;
	}

	if (g_friends.registeredFriends[npID].npID != npID)
	{
		return PresenceStateUnknown;
	}

	switch (g_friends.registeredFriends[npID].presence)
	{
		case PresenceOnline:
			return PresenceStateOnline;
		case PresenceAway:
			return PresenceStateAway;
		case PresenceXA:
			return PresenceStateExtendedAway;
		case PresenceOffline:
			return PresenceStateOffline;
	}

	return PresenceStateOffline;
}

LIBNP_API void LIBNP_CALL NP_SetExternalSteamID(uint64_t steamID)
{
	RPCFriendsSetSteamIDMessage* request = new RPCFriendsSetSteamIDMessage();
	request->GetBuffer()->set_steamid(steamID);

	RPC_SendMessage(request);

	request->Free();
}

LIBNP_API void LIBNP_CALL NP_SetRichPresence(const char* key, const char* value)
{
	if (value)
	{
		g_friends.richPresenceProps[key] = value;
	}
	else
	{
		g_friends.richPresenceProps.erase(key);
	}
}

LIBNP_API void LIBNP_CALL NP_SetRichPresenceBody(const char* body)
{
	if (body)
	{
		strncpy(g_friends.richPresenceBody, body, 255);
	}
	else
	{
		g_friends.richPresenceBody[0] = '\0';
	}
}

LIBNP_API const char* LIBNP_CALL NP_GetFriendRichPresence(NPID npID, const char* key)
{
	if (!g_friends.connected)
	{
		// we can just say anything here, nobody will care
		return "Cake.";
	}

	if (g_friends.registeredFriends[npID].npID != npID)
	{
		return NULL;
	}

	if (g_friends.registeredFriends[npID].presence == PresenceOffline)
	{
		return NULL;
	}

	if (g_friends.registeredFriends[npID].richPresence.find(key) == g_friends.registeredFriends[npID].richPresence.end())
	{
		return NULL;
	}

	return g_friends.registeredFriends[npID].richPresence[key].c_str();
}

LIBNP_API const char* LIBNP_CALL NP_GetFriendRichPresenceBody(NPID npID)
{
	if (!g_friends.connected)
	{
		// we can just say anything here, nobody will care
		return "Cake.";
	}

	if (g_friends.registeredFriends[npID].npID != npID)
	{
		return NULL;
	}

	if (g_friends.registeredFriends[npID].presence == PresenceOffline)
	{
		return NULL;
	}

	return g_friends.registeredFriends[npID].richPresenceString;
}

LIBNP_API void LIBNP_CALL NP_StoreRichPresence()
{
	RPCFriendsSetPresenceMessage* request = new RPCFriendsSetPresenceMessage();

	for (std::map<std::string, std::string>::iterator iter = g_friends.richPresenceProps.begin(); iter != g_friends.richPresenceProps.end(); iter++)
	{
		FriendsPresence* item = request->GetBuffer()->add_presence();

		item->set_key(iter->first);
		item->set_value(iter->second);
	}

	FriendsPresence* bodyItem = request->GetBuffer()->add_presence();

	bodyItem->set_key("__body");
	bodyItem->set_value(g_friends.richPresenceBody);

	RPC_SendMessage(request);

	request->Free();
}

// buffer details
struct np_get_user_avatar_state_s
{
	NPAsyncImpl<NPGetUserAvatarResult>* asyncResult;
	uint8_t* buffer;
	size_t bufferLength;
};

static void GetUserAvatarCB(NPAsync<INPRPCMessage>* async)
{
	np_get_user_avatar_state_s* state = (np_get_user_avatar_state_s*)async->GetUserData();
	RPCFriendsGetUserAvatarResultMessage* rpcResult = (RPCFriendsGetUserAvatarResultMessage*)async->GetResult();

	// print log
	Log_Print("GetUserAvatarCB for client %d\n", rpcResult->GetBuffer()->guid());

	// create and fill out result object
	NPGetUserAvatarResult* result = new NPGetUserAvatarResult();
	result->result = (EGetFileResult)rpcResult->GetBuffer()->result();
	result->buffer = state->buffer;
	result->fileSize = 0;
	result->guid = rpcResult->GetBuffer()->guid();

	// set the buffer to 0
	memset(state->buffer, 0, state->bufferLength);

	// copy file data to the buffer
	if (result->result == GetFileResultOK)
	{
		result->fileSize = rpcResult->GetBuffer()->filedata().length();

		size_t toCopy = (result->fileSize > state->bufferLength) ? state->bufferLength : result->fileSize;
		memcpy(result->buffer, rpcResult->GetBuffer()->filedata().c_str(), toCopy);
		result->fileSize = toCopy;
	}

	// and handle the async result
	state->asyncResult->SetResult(result);
	delete state;
}

LIBNP_API NPAsync<NPGetUserAvatarResult>* LIBNP_CALL NP_GetUserAvatar(int id, uint8_t* buffer, size_t bufferLength)
{
	RPCFriendsGetUserAvatarMessage* request = new RPCFriendsGetUserAvatarMessage();
	request->GetBuffer()->set_guid(id);

	NPAsync<INPRPCMessage>* async = RPC_SendMessageAsync(request);
	NPAsyncImpl<NPGetUserAvatarResult>* result = new NPAsyncImpl<NPGetUserAvatarResult>();

	np_get_user_avatar_state_s* state = new np_get_user_avatar_state_s();
	state->asyncResult = result;
	state->buffer = buffer;
	state->bufferLength = bufferLength;

	async->SetCallback(GetUserAvatarCB, state);

	request->Free();

	return result;
}
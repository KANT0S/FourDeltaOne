#include "StdInc.h"
#include <mmsystem.h>
#include "GSClient.h"
#include "SteamMatchmakingServers002.h"

// Request a new list of servers of a particular type.  These calls each correspond to one of the EMatchMakingType values.
// Each call allocates a new asynchronous request object.
// Request object must be released by calling ReleaseRequest( hServerListRequest )
static ISteamMatchmakingServerListResponse002* response;

ISteamMatchmakingServers002* SteamProxy_GetSteamMatchmakingServers();
gameserveritem_t* GSClient_ServerItem(int i);
int GSClient_NumServers();
void GSClient_QueryMaster();

struct ProxyResponse : ISteamMatchmakingServerListResponse
{
	ISteamMatchmakingServerListResponse* _real;

	ProxyResponse(ISteamMatchmakingServerListResponse* real)
	{
		_real = real;
	}

	void ServerResponded( HServerListRequest hRequest, int iServer )
	{
		Trace("ProxyResponse", "ServerResponded %i", iServer);

		_real->ServerResponded(hRequest, iServer);
	}

	void ServerFailedToRespond( HServerListRequest hRequest, int iServer )
	{
		Trace("ProxyResponse", "ServerFailedToRespond %i", iServer);

		_real->ServerFailedToRespond(hRequest, iServer);
	}

	void RefreshComplete( HServerListRequest hRequest, EMatchMakingServerResponse response )
	{
		Trace("ProxyResponse", "RefreshComplete");

		_real->RefreshComplete(hRequest, response);
	}
};

HServerListRequest CSteamMatchmakingServers002::RequestInternetServerList( AppId_t iApp, MatchMakingKeyValuePair_t **ppchFilters, uint32 nFilters, ISteamMatchmakingServerListResponse *pRequestServersResponse )
{
	Trace("SteamMatchmakingServers", "RequestInternetServerList");

	/*static ProxyResponse response(pRequestServersResponse);

	return SteamProxy_GetSteamMatchmakingServers()->RequestInternetServerList(iApp, ppchFilters, nFilters, &response);*/

	response = pRequestServersResponse;

	GSClient_QueryMaster();

	return NULL;
}
HServerListRequest CSteamMatchmakingServers002::RequestLANServerList( AppId_t iApp, ISteamMatchmakingServerListResponse *pRequestServersResponse )
{
	Trace("SteamMatchmakingServers", "RequestLANServerList");
	return NULL;
}
HServerListRequest CSteamMatchmakingServers002::RequestFriendsServerList( AppId_t iApp, MatchMakingKeyValuePair_t **ppchFilters, uint32 nFilters, ISteamMatchmakingServerListResponse *pRequestServersResponse )
{
	Trace("SteamMatchmakingServers", "RequestFriendsServerList");
	return NULL;
}
HServerListRequest CSteamMatchmakingServers002::RequestFavoritesServerList( AppId_t iApp, MatchMakingKeyValuePair_t **ppchFilters, uint32 nFilters, ISteamMatchmakingServerListResponse *pRequestServersResponse )
{
	Trace("SteamMatchmakingServers", "RequestFavoritesServerList");
	return NULL;
}
HServerListRequest CSteamMatchmakingServers002::RequestHistoryServerList( AppId_t iApp, MatchMakingKeyValuePair_t **ppchFilters, uint32 nFilters, ISteamMatchmakingServerListResponse *pRequestServersResponse )
{
	Trace("SteamMatchmakingServers", "RequestHistoryServerList");
	return NULL;
}
HServerListRequest CSteamMatchmakingServers002::RequestSpectatorServerList( AppId_t iApp, MatchMakingKeyValuePair_t **ppchFilters, uint32 nFilters, ISteamMatchmakingServerListResponse *pRequestServersResponse )
{
	Trace("SteamMatchmakingServers", "RequestSpectatorServerList");
	return NULL;
}

// Releases the asynchronous request object and cancels any pending query on it if there's a pending query in progress.
// RefreshComplete callback is not posted when request is released.
void CSteamMatchmakingServers002::ReleaseRequest( HServerListRequest hServerListRequest )
{
	Trace("SteamMatchmakingServers", "ReleaseRequest");
}

/* the filters that are available in the ppchFilters params are:

	"map"		- map the server is running, as set in the dedicated server api
	"dedicated" - reports bDedicated from the API
	"secure"	- VAC-enabled
	"full"		- not full
	"empty"		- not empty
	"noplayers" - is empty
	"proxy"		- a relay server

*/

// Get details on a given server in the list, you can get the valid range of index
// values by calling GetServerCount().  You will also receive index values in 
// ISteamMatchmakingServerListResponse::ServerResponded() callbacks
gameserveritem_t *CSteamMatchmakingServers002::GetServerDetails( HServerListRequest hRequest, int iServer )
{
	Trace("SteamMatchmakingServers", "GetServerDetails");

	//return SteamProxy_GetSteamMatchmakingServers()->GetServerDetails(hRequest, iServer);

	return GSClient_ServerItem(iServer);

	static gameserveritem_t item;

	// TODO: find a way to pass these ids as NP will need them
	item.m_steamID = CSteamID(2, 1, k_EUniversePublic, k_EAccountTypeGameServer);
	item.m_bDoNotRefresh = false;
	item.m_bHadSuccessfulResponse = true;
	item.m_bPassword = false;
	item.m_bSecure = false;
	item.m_nAppID = 42690;
	item.m_nBotPlayers = 0;
	item.m_NetAdr.Init(0xc0a8b27b, 27017, 27015);
	item.m_nMaxPlayers = 18;
	item.m_nPing = 23;
	item.m_nPlayers = 0;
	item.m_nServerVersion = 2;
	strcpy(item.m_szGameDescription, "war");
	strcpy(item.m_szGameDir, "modernwarfare3");
	strcpy(item.m_szMap, "mp_dome");
	strcpy(item.m_szGameTags, "gn\\IW5\\gt\\gun\\m\\mp_hardhat\\");
	strcpy(item.m_szGameTagsExt, "gamename\\IW5\\g_gametype\\gun\\mapname\\mp_hardhat\\");
	item.SetName("emptiness within");
	item.m_ulTimeLastPlayed = 0;
	return &item;
}

// Cancel an request which is operation on the given list type.  You should call this to cancel
// any in-progress requests before destructing a callback object that may have been passed 
// to one of the above list request calls.  Not doing so may result in a crash when a callback
// occurs on the destructed object.
// Canceling a query does not release the allocated request handle.
// The request handle must be released using ReleaseRequest( hRequest )
void CSteamMatchmakingServers002::CancelQuery( HServerListRequest hRequest )
{
	Trace("SteamMatchmakingServers", "CancelQuery");
}

// Ping every server in your list again but don't update the list of servers
// Query callback installed when the server list was requested will be used
// again to post notifications and RefreshComplete, so the callback must remain
// valid until another RefreshComplete is called on it or the request
// is released with ReleaseRequest( hRequest )
void CSteamMatchmakingServers002::RefreshQuery( HServerListRequest hRequest )
{
	Trace("SteamMatchmakingServers", "RefreshQuery");
}

// Returns true if the list is currently refreshing its server list
bool CSteamMatchmakingServers002::IsRefreshing( HServerListRequest hRequest )
{
	Trace("SteamMatchmakingServers", "IsRefreshing");
	return false;
}

// How many servers in the given list, GetServerDetails above takes 0... GetServerCount() - 1
int CSteamMatchmakingServers002::GetServerCount( HServerListRequest hRequest )
{
	Trace("SteamMatchmakingServers", "GetServerCount");
	return GSClient_NumServers();
}

// Refresh a single server inside of a query (rather than all the servers )
void CSteamMatchmakingServers002::RefreshServer( HServerListRequest hRequest, int iServer )
{
	Trace("SteamMatchmakingServers", "RefreshServer");
}


//-----------------------------------------------------------------------------
// Queries to individual servers directly via IP/Port
//-----------------------------------------------------------------------------

// Request updated ping time and other details from a single server
HServerQuery CSteamMatchmakingServers002::PingServer( uint32 unIP, uint16 usPort, ISteamMatchmakingPingResponse *pRequestServersResponse )
{
	Trace("SteamMatchmakingServers", "PingServer");
	return NULL;
}

// Request the list of players currently playing on a server
HServerQuery CSteamMatchmakingServers002::PlayerDetails( uint32 unIP, uint16 usPort, ISteamMatchmakingPlayersResponse *pRequestServersResponse )
{
	Trace("SteamMatchmakingServers", "PlayerDetails");
	return NULL;
}

// Request the list of rules that the server is running (See ISteamMasterServerUpdater->SetKeyValue() to set the rules server side)
HServerQuery CSteamMatchmakingServers002::ServerRules( uint32 unIP, uint16 usPort, ISteamMatchmakingRulesResponse *pRequestServersResponse )
{
	Trace("SteamMatchmakingServers", "ServerRules");
	return NULL;
}

// Cancel an outstanding Ping/Players/Rules query from above.  You should call this to cancel
// any in-progress requests before destructing a callback object that may have been passed 
// to one of the above calls to avoid crashing when callbacks occur.
void CSteamMatchmakingServers002::CancelServerQuery( HServerQuery hServerQuery )
{
	Trace("SteamMatchmakingServers", "CancelServerQuery");
}

struct gameserveritemext_t : gameserveritem_t
{
	DWORD queryTime;
	bool responded;
	bool queried;
};

static struct  
{
	SOCKET socket;
	sockaddr_in from;
	gameserveritemext_t servers[8192];
	int numServers;
	DWORD lastQueryStep;
} g_cls;

bool GSClient_Init()
{
	WSADATA wsaData;
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (err)
	{
		return false;
	}

	g_cls.socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (g_cls.socket == INVALID_SOCKET)
	{
		Trace("GSClient", "socket() failed - %d", WSAGetLastError());
		return false;
	}

	sockaddr_in bindAddr;
	memset(&bindAddr, 0, sizeof(bindAddr));
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = 0;
	bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(g_cls.socket, (sockaddr*)&bindAddr, sizeof(bindAddr)) == SOCKET_ERROR)
	{
		Trace("GSClient", "bind() failed - %d", WSAGetLastError());
		return false;
	}

	ULONG nonBlocking = 1;
	ioctlsocket(g_cls.socket, FIONBIO, &nonBlocking);

	return true;
}

gameserveritem_t* GSClient_ServerItem(int i)
{
	return &g_cls.servers[i];
}

int GSClient_NumServers()
{
	return g_cls.numServers;
}

void GSClient_QueryServer(int i)
{
	gameserveritemext_t* server = &g_cls.servers[i];

	server->queried = true;
	server->responded = false;
	server->queryTime = timeGetTime();

	sockaddr_in serverIP;
	serverIP.sin_family = AF_INET;
	serverIP.sin_addr.s_addr = htonl(server->m_NetAdr.GetIP());
	serverIP.sin_port = htons(server->m_NetAdr.GetQueryPort());

	char message[128];
	_snprintf(message, sizeof(message), "\xFF\xFF\xFF\xFFgetinfo xxx");

	sendto(g_cls.socket, message, strlen(message), 0, (sockaddr*)&serverIP, sizeof(serverIP));
}

void GSClient_QueryStep()
{
	if ((GetTickCount() - g_cls.lastQueryStep) < 50)
	{
		return;
	}

	g_cls.lastQueryStep = GetTickCount();

	int count = 0;

	for (int i = 0; i < g_cls.numServers && count < 20; i++)
	{
		if (!g_cls.servers[i].queried)
		{
			GSClient_QueryServer(i);
			count++;
		}
	}
}

void GSClient_HandleInfoResponse(const char* bufferx, int len)
{
	Trace("GSClient", "received infoResponse");

	for (int i = 0; i < g_cls.numServers; i++)
	{
		gameserveritemext_t* server = &g_cls.servers[i];

		if ((server->m_NetAdr.GetIP() == ntohl(g_cls.from.sin_addr.s_addr)) && (server->m_NetAdr.GetQueryPort() == ntohs(g_cls.from.sin_port)))
		{
			bufferx++;

			char buffer[8192];
			strcpy(buffer, bufferx);

			Trace("GSClient", "received *matching* infoResponse - %d %s", i, Info_ValueForKey(buffer, "hostname"));

			// filter odd characters out of the result
			int len = strlen(buffer);

			for (int i = 0; i < len; i++)
			{
				char thisChar = buffer[i];

				if (thisChar < ' ' || thisChar > '~')
				{
					buffer[i] = ' ';
				}
			}

			server->m_nPing = timeGetTime() - server->queryTime;

			// possible sneaky vuln here should anyone copy this stuff
			strcpy(server->m_szGameTagsExt, buffer);

			server->m_steamID = CSteamID(i, 1, k_EUniversePublic, k_EAccountTypeGameServer);
			server->m_bDoNotRefresh = false;
			server->m_bHadSuccessfulResponse = true;
			server->m_bPassword = atoi(Info_ValueForKey(buffer, "pswrd")) == 1;
			server->m_bSecure = false;
			server->m_nAppID = 42690;
			server->m_nBotPlayers = 0;
			server->m_nMaxPlayers = atoi(Info_ValueForKey(buffer, "sv_maxclients"));
			server->m_nPlayers = atoi(Info_ValueForKey(buffer, "clients"));
			server->m_nServerVersion = 2;
			strcpy(server->m_szGameDescription, Info_ValueForKey(buffer, "g_gametype"));
			strcpy(server->m_szGameDir, "modernwarfare3");
			strcpy(server->m_szMap, Info_ValueForKey(buffer, "mapname"));
			server->SetName(Info_ValueForKey(buffer, "hostname"));
			server->m_ulTimeLastPlayed = 0;

			Info_RemoveKey(server->m_szGameTagsExt, "gametype");
			Info_SetValueForKey(server->m_szGameTagsExt, "g_gametype", Info_ValueForKey(buffer, "gametype"));

			response->ServerResponded(NULL, i);

			break;
		}
	}
}

typedef struct  
{
	unsigned char ip[4];
	unsigned short qport;
	unsigned short port;
} serverAddress_t;

void GSClient_HandleServersResponse(const char* buffer, int len)
{
	int numservers = 0;
	const char* buffptr    = buffer;
	const char* buffend    = buffer + len;
	serverAddress_t addresses[256];
	while (buffptr+1 < buffend) {
		// advance to initial token
		do {
			if (*buffptr++ == '\\')
				break;		
		}
		while (buffptr < buffend);

		if ( buffptr >= buffend - 8 ) {
			break;
		}

		// parse out ip
		addresses[numservers].ip[0] = *buffptr++;
		addresses[numservers].ip[1] = *buffptr++;
		addresses[numservers].ip[2] = *buffptr++;
		addresses[numservers].ip[3] = *buffptr++;

		// parse out port
		addresses[numservers].qport = (*(buffptr++))<<8;
		addresses[numservers].qport += (*(buffptr++)) & 0xFF;
		addresses[numservers].qport = addresses[numservers].qport;

		addresses[numservers].port = (*(buffptr++))<<8;
		addresses[numservers].port += (*(buffptr++)) & 0xFF;
		addresses[numservers].port = addresses[numservers].port;

		// syntax check
		if (*buffptr != '\\') {
			break;
		}

		numservers++;
		if (numservers >= 256) {
			break;
		}

		// parse out EOT
		if (buffptr[1] == 'E' && buffptr[2] == 'O' && buffptr[3] == 'T') {
			break;
		}
	}

	int count = g_cls.numServers;
	int max = 8192;

	for (int i = 0; i < numservers && count < max; i++) {
		// build net address
		unsigned int ip = (addresses[i].ip[0] << 24) | (addresses[i].ip[1] << 16) | (addresses[i].ip[2] << 8) | (addresses[i].ip[3]);
		g_cls.servers[count].m_NetAdr.Init(ip, addresses[i].qport, addresses[i].port);
		g_cls.servers[count].queried = false;

		count++;
	}

	GSClient_QueryStep();

	g_cls.numServers = count;
}

#define CMD_GSR "getserversResponse"
#define CMD_INFO "infoResponse"

void GSClient_HandleOOB(const char* buffer, size_t len)
{
	if (!_strnicmp(buffer, CMD_GSR, strlen(CMD_GSR)))
	{
		GSClient_HandleServersResponse(&buffer[strlen(CMD_GSR)], len - strlen(CMD_GSR));
	}

	if (!_strnicmp(buffer, CMD_INFO, strlen(CMD_INFO)))
	{
		GSClient_HandleInfoResponse(&buffer[strlen(CMD_INFO)], len - strlen(CMD_INFO));
	}
}

void GSClient_PollSocket()
{
	char buf[2048];
	memset(buf, 0, sizeof(buf));

	sockaddr_in from;
	memset(&from, 0, sizeof(from));

	int fromlen = sizeof(from);

	while (true)
	{
		int len = recvfrom(g_cls.socket, buf, 2048, 0, (sockaddr*)&from, &fromlen);

		if (len == SOCKET_ERROR)
		{
			int error = WSAGetLastError();

			if (error != WSAEWOULDBLOCK)
			{
				Trace("GSClient", "recv() failed - %d", error);
			}

			return;
		}

		g_cls.from = from;

		if (*(int*)buf == -1)
		{
			GSClient_HandleOOB(&buf[4], len - 4);
		}
	}
}

void GSClient_RunFrame()
{
	/*if (response)
	{
		response->ServerResponded(NULL, 0);
		response->RefreshComplete(NULL, eServerResponded);
		response = NULL;
	}*/

	GSClient_QueryStep();
	GSClient_PollSocket();
}

void GSClient_QueryMaster()
{
	static bool lookedUp;
	static sockaddr_in masterIP;

	g_cls.numServers = 0;

	if (!lookedUp)
	{
		hostent* host = gethostbyname(MASTER_SERVER);

		if (!host)
		{
			Trace("GSClient", "gethostbyname() failed - %d", WSAGetLastError());
			return;
		}

		masterIP.sin_family = AF_INET;
		masterIP.sin_addr.s_addr = *(ULONG*)host->h_addr_list[0];
		masterIP.sin_port = htons(27950);

		lookedUp = true;
	}

	char message[128];
	_snprintf(message, sizeof(message), "\xFF\xFF\xFF\xFFgetservers IW5 19816 full empty");

	sendto(g_cls.socket, message, strlen(message), 0, (sockaddr*)&masterIP, sizeof(masterIP));
}
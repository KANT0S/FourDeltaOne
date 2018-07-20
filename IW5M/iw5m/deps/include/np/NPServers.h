// ==========================================================
// alterIWnet project
// 
// Component: xnp
// Sub-component: libnp
// Purpose: definitions for the NP server list service
//
// Initial author: NTAuthority
// Started: 2012-05-10
// ==========================================================

enum EServersResult
{
	ServersResultOK = 0,
	ServersResultNotAllowed = 1,
	ServersResultNotFound = 2
};

class NPSessionInfo
{
public:
	uint32_t address;
	uint16_t port;
	NPID npid;
	char hostname[512];
	char mapname[64];
	int16_t players;
	int16_t maxplayers;
	int32_t version;
};

typedef uint64_t NPSID;

class NPCreateSessionResult
{
public:
	EServersResult result;
	NPSID sid;
};

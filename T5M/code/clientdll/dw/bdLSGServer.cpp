// ==========================================================
// T5M project
// 
// Component: client
// Sub-component: loopdw
// Purpose: LSG service.
//
// Initial author: NTAuthority
// Started: 2013-04-15
// ==========================================================

#include "StdInc.h"
#include "DW.h"
#include "bdLSGServer.h"

void bdLobbyService::callService(bdServiceServer* server, std::string& message)
{
	bdBitBuffer data(message);
	bool unknownBool;
	unsigned int gameID;
	unsigned int randomNumber;
	char ticket[128];

	data.setUseDataTypes(false);
	data.readBoolean(&unknownBool);
	data.setUseDataTypes(true);

	data.readUInt32(&gameID);
	data.readUInt32(&randomNumber);

	data.readBytes(128, (BYTE*)ticket);

	Trace("dwauth", "setting global key");

	dw_set_global_key((BYTE*)ticket); // ticket starts with the global key
}

bdLSGServer::bdLSGServer()
{
	registerService(new bdLobbyService());
	registerService(new bdTitleUtilities());
	registerService(new bdStorage());
	registerService(new bdMatchMaking());
	registerService(new bdMessaging());
	registerService(new bdProfiles());
	registerService(new bdService50());
	registerService(new bdService58());
	registerService(new bdContentSearch());
}
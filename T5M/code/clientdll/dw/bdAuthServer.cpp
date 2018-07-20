// ==========================================================
// T5M project
// 
// Component: client
// Sub-component: loopdw
// Purpose: Authentication service.
//
// Initial author: NTAuthority
// Started: 2013-04-15
// ==========================================================

#include "StdInc.h"
#include "DW.h"
#include "bdAuthServer.h"

bdAuthServer::bdAuthServer()
{
	registerService(new bdSteamAuthService());
	registerService(new bdDediAuthService());
}
// ==========================================================
// alterIWnet project
// 
// Component: aiw_client
// Sub-component: steam_api
// Purpose: ISteamMasterServerUpdater001 implementation
//
// Initial author: NTAuthority
// Started: 2010-09-28
// ==========================================================

#include "StdInc.h"
#include "SteamMasterServerUpdater001.h"
#include "GSServer.h"

void CSteamMasterServerUpdater001::SetActive( bool bActive )
{
	Trace("SteamMasterServerUpdater", "SetActive %d", bActive);
	GSServer_SetHeartbeatActive(bActive);
}
void CSteamMasterServerUpdater001::SetHeartbeatInterval( int iHeartbeatInterval )
{
	Trace("SteamMasterServerUpdater", "SetHeartbeatInterval %i", iHeartbeatInterval);
}
bool CSteamMasterServerUpdater001::HandleIncomingPacket( const void *pData, int cbData, uint32 srcIP, uint16 srcPort )
{
	Trace("SteamMasterServerUpdater", "HandleIncomingPacket");
	return true;
}
int CSteamMasterServerUpdater001::GetNextOutgoingPacket( void *pOut, int cbMaxOut, uint32 *pNetAdr, uint16 *pPort )
{
	Trace("SteamMasterServerUpdater", "GetNextOutgoingPacket");
	return 0;
}
void CSteamMasterServerUpdater001::SetBasicServerData(unsigned short nProtocolVersion, bool bDedicatedServer,	const char *pRegionName, const char *pProductName, unsigned short nMaxReportedClients, bool bPasswordProtected,	const char *pGameDescription )
{
	Trace("SteamMasterServerUpdater", "SetBasicServerData %i %i %s %s %i %i %s", nProtocolVersion, bDedicatedServer, pRegionName, pProductName, nMaxReportedClients, bPasswordProtected, pGameDescription);
	GSServer_SetHeartbeatActive(true);
}

void CSteamMasterServerUpdater001::ClearAllKeyValues()
{
	Trace("SteamMasterServerUpdater", "ClearAllKeyValues");
}
void CSteamMasterServerUpdater001::SetKeyValue( const char *pKey, const char *pValue )
{
	Trace("SteamMasterServerUpdater", "SetKeyValue %s %s", pKey, pValue);
}

void CSteamMasterServerUpdater001::NotifyShutdown()
{
	Trace("SteamMasterServerUpdater", "NotifyShutdown");
}
bool CSteamMasterServerUpdater001::WasRestartRequested()
{
	Trace("SteamMasterServerUpdater", "WasRestartRequested");
	return false; 
}
void CSteamMasterServerUpdater001::ForceHeartbeat()
{
	Trace("SteamMasterServerUpdater", "ForceHeartbeat");
}
bool CSteamMasterServerUpdater001::AddMasterServer( const char *pServerAddress )
{
	Trace("SteamMasterServerUpdater", "AddMasterServer %s", pServerAddress);
	return true;
}
bool CSteamMasterServerUpdater001::RemoveMasterServer( const char *pServerAddress )
{
	Trace("SteamMasterServerUpdater", "RemoveMasterServer %s", pServerAddress);
	return true;
}
int CSteamMasterServerUpdater001::GetNumMasterServers() { return 1; }
int CSteamMasterServerUpdater001::GetMasterServerAddress( int iServer, char *pOut, int outBufferSize )
{
	Trace("SteamMasterServerUpdater", "GetMasterServerAddress %d", iServer);
	return 0;
}
// ==========================================================
// alterIWnet project
// 
// Component: aiw_client
// Sub-component: steam_api
// Purpose: ISteamNetworking004 implementation
//
// Initial author: NTAuthority
// Started: 2010-09-19
// ==========================================================

#include "StdInc.h"
#include "SteamNetworking004.h"

bool CSteamNetworking004::SendP2PPacket( CSteamID steamIDRemote, const void *pubData, uint32 cubData, EP2PSend eP2PSendType, int unk1 ) {
	Trace("SteamNetworking", "SendP2PPacket");
	return false;
}

bool CSteamNetworking004::IsP2PPacketAvailable( uint32 *pcubMsgSize, int unk1 ) {
	//g_Logging.AddToLogFileA("ISteamNetworking004.log", "IsP2PPacketAvailable");
	return false;
}

bool CSteamNetworking004::ReadP2PPacket( void *pubDest, uint32 cubDest, uint32 *pcubMsgSize, CSteamID *psteamIDRemote, int unk1 ) {
	Trace("SteamNetworking", "ReadP2PPacket");
	return false;
}

bool CSteamNetworking004::AcceptP2PSessionWithUser( CSteamID steamIDRemote ) {
	return false;
}

bool CSteamNetworking004::CloseP2PSessionWithUser( CSteamID steamIDRemote ) {
	return false;
}

bool CSteamNetworking004::GetP2PSessionState( CSteamID steamIDRemote, P2PSessionState_t *pConnectionState ) {
	return false;
}

SNetListenSocket_t CSteamNetworking004::CreateListenSocket( int nVirtualP2PPort, uint32 nIP, uint16 nPort, bool bAllowUseOfPacketRelay ) {
	return NULL;
}

SNetSocket_t CSteamNetworking004::CreateP2PConnectionSocket( CSteamID steamIDTarget, int nVirtualPort, int nTimeoutSec, bool bAllowUseOfPacketRelay ) {
	return NULL;
}

SNetSocket_t CSteamNetworking004::CreateConnectionSocket( uint32 nIP, uint16 nPort, int nTimeoutSec ) {
	return NULL;
}

bool CSteamNetworking004::DestroySocket( SNetSocket_t hSocket, bool bNotifyRemoteEnd ) {
	return false;
}

bool CSteamNetworking004::DestroyListenSocket( SNetListenSocket_t hSocket, bool bNotifyRemoteEnd ) {
	return false;
}

bool CSteamNetworking004::SendDataOnSocket( SNetSocket_t hSocket, void *pubData, uint32 cubData, bool bReliable ) {
	return false;
}

bool CSteamNetworking004::IsDataAvailableOnSocket( SNetSocket_t hSocket, uint32 *pcubMsgSize ) {
	return false;
}

bool CSteamNetworking004::RetrieveDataFromSocket( SNetSocket_t hSocket, void *pubDest, uint32 cubDest, uint32 *pcubMsgSize ) {
	return false;
}

bool CSteamNetworking004::IsDataAvailable( SNetListenSocket_t hListenSocket, uint32 *pcubMsgSize, SNetSocket_t *phSocket ) {
	return false;
}

bool CSteamNetworking004::RetrieveData( SNetListenSocket_t hListenSocket, void *pubDest, uint32 cubDest, uint32 *pcubMsgSize, SNetSocket_t *phSocket ) {
	return false;
}

bool CSteamNetworking004::GetSocketInfo( SNetSocket_t hSocket, CSteamID *pSteamIDRemote, int *peSocketStatus, uint32 *punIPRemote, uint16 *punPortRemote ) {
	return false;
}

bool CSteamNetworking004::GetListenSocketInfo( SNetListenSocket_t hListenSocket, uint32 *pnIP, uint16 *pnPort ) {
	return false;
}

ESNetSocketConnectionType CSteamNetworking004::GetSocketConnectionType( SNetSocket_t hSocket ) {
	return k_ESNetSocketConnectionTypeNotConnected;
}

int CSteamNetworking004::GetMaxPacketSize( SNetSocket_t hSocket ) {
	return 0;
}
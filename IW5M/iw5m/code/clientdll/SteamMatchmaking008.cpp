// ==========================================================
// alterIWnet project
// 
// Component: aiw_client
// Sub-component: steam_api
// Purpose: ISteamMatchmaking008 implementation
//
// Initial author: NTAuthority
// Started: 2010-09-27
// ==========================================================

#include "StdInc.h"
#include "SteamMatchmaking008.h"

unsigned int GetPlayerSteamID();

// ^{[a-z0-9_]*} {.*}\({.*}\) \{

int CSteamMatchmaking008::GetFavoriteGameCount() {
	return 0;
}

bool CSteamMatchmaking008::GetFavoriteGame( int iGame, AppId_t *pnAppID, uint32 *pnIP, uint16 *pnConnPort, uint16 *pnQueryPort, uint32 *punFlags, RTime32 *pRTime32LastPlayedOnServer ) {
	return false;
}

int CSteamMatchmaking008::AddFavoriteGame( AppId_t nAppID, uint32 nIP, uint16 nConnPort, uint16 nQueryPort, uint32 unFlags, RTime32 rTime32LastPlayedOnServer ) {
	return 0;
}

bool CSteamMatchmaking008::RemoveFavoriteGame( AppId_t nAppID, uint32 nIP, uint16 nConnPort, uint16 nQueryPort, uint32 unFlags ) {
	return false;
}

SteamAPICall_t CSteamMatchmaking008::RequestLobbyList() {
	SteamAPICall_t result = CSteamBase::RegisterCall();
	LobbyMatchList_t* retvals = (LobbyMatchList_t*)malloc(sizeof(LobbyMatchList_t));
	retvals->m_nLobbiesMatching = 0;

	CSteamBase::ReturnCall(retvals, sizeof(LobbyMatchList_t), LobbyMatchList_t::k_iCallback, result);

	return result;
}

void CSteamMatchmaking008::AddRequestLobbyListStringFilter( const char *pchKeyToMatch, const char *pchValueToMatch, ELobbyComparison eComparisonType ) {
	
}

void CSteamMatchmaking008::AddRequestLobbyListNumericalFilter( const char *pchKeyToMatch, int nValueToMatch, ELobbyComparison nComparisonType ) {
	
}

void CSteamMatchmaking008::AddRequestLobbyListNearValueFilter( const char *pchKeyToMatch, int nValueToBeCloseTo ) {
	
}

void CSteamMatchmaking008::AddRequestLobbyListFilterSlotsAvailable( int ) {
	
}

CSteamID CSteamMatchmaking008::GetLobbyByIndex( int iLobby ) {
	return CSteamID( Auth_GetUserID(), 0x40000, k_EUniversePublic, k_EAccountTypeChat );
}

SteamAPICall_t CSteamMatchmaking008::CreateLobby( ELobbyType eLobbyType, int ) {
	SteamAPICall_t result = CSteamBase::RegisterCall();
	LobbyCreated_t* retvals = (LobbyCreated_t*)malloc(sizeof(LobbyCreated_t));
	CSteamID id = CSteamID( Auth_GetUserID(), 0x40000, k_EUniversePublic, k_EAccountTypeChat );
	//CSteamID id = CSteamID( (uint64)0x1100001034bd36e );

	retvals->m_eResult = k_EResultOK;
	retvals->m_ulSteamIDLobby = id.ConvertToUint64();

	//

	CSteamBase::ReturnCall(retvals, sizeof(LobbyCreated_t), LobbyCreated_t::k_iCallback, result);

	JoinLobby(id);

	return result;
}

static CSteamID joinedLobbyID;

SteamAPICall_t CSteamMatchmaking008::JoinLobby( CSteamID steamIDLobby ) {
	SteamAPICall_t result = CSteamBase::RegisterCall();
	LobbyEnter_t* retvals = (LobbyEnter_t*)malloc(sizeof(LobbyEnter_t));
	retvals->m_bLocked = false;
	retvals->m_EChatRoomEnterResponse = k_EChatRoomEnterResponseSuccess;
	retvals->m_rgfChatPermissions = (EChatPermission)0xFFFFFFFF;
	retvals->m_ulSteamIDLobby = steamIDLobby.ConvertToUint64();

	joinedLobbyID = steamIDLobby;

	CSteamBase::ReturnCall(retvals, sizeof(LobbyEnter_t), LobbyEnter_t::k_iCallback, result);

	return result;
}

void CSteamMatchmaking008::LeaveLobby( CSteamID steamIDLobby ) {
	
}

bool CSteamMatchmaking008::InviteUserToLobby( CSteamID steamIDLobby, CSteamID steamIDInvitee ) {
	
	return true;
}

int CSteamMatchmaking008::GetNumLobbyMembers( CSteamID steamIDLobby ) {
	//
	return 1;
}

CSteamID CSteamMatchmaking008::GetLobbyMemberByIndex( CSteamID steamIDLobby, int iMember ) {
	NPID npID;
	NP_GetNPID(&npID);

	return CSteamID(npID);
}

const char *CSteamMatchmaking008::GetLobbyData( CSteamID steamIDLobby, const char *pchKey ) {
	//NPID friendID = steamIDLobby.ConvertToUint64();
	NPID friendID = joinedLobbyID.ConvertToUint64();
	friendID &= 0xFF000000FFFFFFFF;
	friendID |= 0x0010000100000000;

	const char* presence = NP_GetFriendRichPresence(friendID, pchKey);

	return (presence) ? presence : "";
}

bool CSteamMatchmaking008::SetLobbyData( CSteamID steamIDLobby, const char *pchKey, const char *pchValue ) {
	if (steamIDLobby.GetAccountID() == Auth_GetUserID())
	{
		NP_SetRichPresence(pchKey, pchValue);
	}
	return true;
}

int CSteamMatchmaking008::GetLobbyDataCount( CSteamID steamIDLobby ) {
	
	return 0;
}

bool CSteamMatchmaking008::GetLobbyDataByIndex( CSteamID steamIDLobby, int iData, char *pchKey, int cubKey, char *pchValue, int cubValue ) {
	
	return false;
}

bool CSteamMatchmaking008::DeleteLobbyData( CSteamID steamIDLobby, const char *pchKey ) {
	
	return 0;
}

const char * CSteamMatchmaking008::GetLobbyMemberData( CSteamID steamIDLobby, CSteamID steamIDUser, const char *pchKey ) {
	
	return "";
}

void CSteamMatchmaking008::SetLobbyMemberData( CSteamID steamIDLobby, const char *pchKey, const char *pchValue ) {
	
}

bool CSteamMatchmaking008::SendLobbyChatMsg( CSteamID steamIDLobby, const void *pvMsgBody, int cubMsgBody ) {
	
	return true;
}

int CSteamMatchmaking008::GetLobbyChatEntry( CSteamID steamIDLobby, int iChatID, CSteamID *pSteamIDUser, void *pvData, int cubData, EChatEntryType *peChatEntryType ) {
	
	return 0;
}

bool CSteamMatchmaking008::RequestLobbyData( CSteamID steamIDLobby ) {
	
	return false;
}

void CSteamMatchmaking008::SetLobbyGameServer( CSteamID steamIDLobby, uint32 unGameServerIP, uint16 unGameServerPort, CSteamID steamIDGameServer ) {
	
}

bool CSteamMatchmaking008::GetLobbyGameServer( CSteamID steamIDLobby, uint32 *punGameServerIP, uint16 *punGameServerPort, CSteamID *psteamIDGameServer ) {
	
	return false;
}

bool CSteamMatchmaking008::SetLobbyMemberLimit( CSteamID steamIDLobby, int cMaxMembers ) {
	
	return true;
}

int CSteamMatchmaking008::GetLobbyMemberLimit( CSteamID steamIDLobby ) {
	
	return 0;
}

bool CSteamMatchmaking008::SetLobbyType( CSteamID steamIDLobby, ELobbyType eLobbyType ) {
	
	return true;
}

bool CSteamMatchmaking008::SetLobbyJoinable( CSteamID steamIDLobby, bool bJoinable ) {
	
	return true;
}

CSteamID CSteamMatchmaking008::GetLobbyOwner( CSteamID steamIDLobby ) {
	NPID npID;
	NP_GetNPID(&npID);

	return CSteamID(npID);
}

bool CSteamMatchmaking008::SetLobbyOwner( CSteamID steamIDLobby, CSteamID steamIDNewOwner ) {
	return true;
}

void CSteamMatchmaking008::AddRequestLobbyListDistanceFilter(ELobbyDistanceFilter f)
{

}

void CSteamMatchmaking008::AddRequestLobbyListResultCountFilter( int cMaxResults )
{
	
}
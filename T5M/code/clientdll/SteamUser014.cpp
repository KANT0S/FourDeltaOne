// ==========================================================
// alterIWnet project
// 
// Component: aiw_client
// Sub-component: steam_api
// Purpose: ISteamUser014 implementation
//
// Initial author: NTAuthority
// Started: 2010-09-10
// ==========================================================

#include "StdInc.h"
#include "SteamUser014.h"

HSteamUser CSteamUser014::GetHSteamUser()
{
	return NULL;
}

bool CSteamUser014::BLoggedOn()
{
	return true;
}

CSteamID CSteamUser014::GetSteamID()
{
	NPID npID;
	NP_GetNPID(&npID);
	
	return CSteamID(npID);
}

int CSteamUser014::InitiateGameConnection( void *pAuthBlob, int cbMaxAuthBlob, CSteamID steamIDGameServer, uint32 unIPServer, uint16 usPortServer, bool bSecure )
{
	if (!NP_GetUserTicket(pAuthBlob, cbMaxAuthBlob, steamIDGameServer.ConvertToUint64()))
	{
		return 0;
	}

	return sizeof(NPAuthenticateTicket);
}

void CSteamUser014::TerminateGameConnection( uint32 unIPServer, uint16 usPortServer )
{
}

void CSteamUser014::TrackAppUsageEvent( CGameID gameID, EAppUsageEvent eAppUsageEvent, const char *pchExtraInfo )
{
}

bool CSteamUser014::GetUserDataFolder( char *pchBuffer, int cubBuffer )
{
	return true;
}

void CSteamUser014::StartVoiceRecording( )
{
}

void CSteamUser014::StopVoiceRecording( )
{
}

EVoiceResult CSteamUser014::GetAvailableVoice(uint32 *pcbCompressed, uint32 *pcbUncompressed)
{
	return k_EVoiceResultOK;
}

EVoiceResult CSteamUser014::GetVoice( bool bWantCompressed, void *pDestBuffer, uint32 cbDestBufferSize, uint32 *nBytesWritten, bool bWantUncompressed, void *pUncompressedDestBuffer, uint32 cbUncompressedDestBufferSize, uint32 *nUncompressBytesWritten )
{
	return k_EVoiceResultOK;
}

EVoiceResult CSteamUser014::DecompressVoice( void *pCompressed, uint32 cbCompressed, void *pDestBuffer, uint32 cbDestBufferSize, uint32 *nBytesWritten )
{
	return k_EVoiceResultOK;
}

HAuthTicket CSteamUser014::GetAuthSessionTicket( void *pTicket, int cbMaxTicket, uint32 *pcbTicket )
{
	return 0;
}

EBeginAuthSessionResult CSteamUser014::BeginAuthSession( const void *pAuthTicket, int cbAuthTicket, CSteamID steamID )
{
	return k_EBeginAuthSessionResultOK;
}

void CSteamUser014::EndAuthSession( CSteamID steamID )
{

}

void CSteamUser014::CancelAuthTicket( HAuthTicket hAuthTicket )
{

}

EUserHasLicenseForAppResult CSteamUser014::UserHasLicenseForApp( CSteamID steamID, AppId_t appID )
{
	return k_EUserHasLicenseResultHasLicense;
}

bool CSteamUser014::IsBehindNAT()
{
	return false;
}

bool CSteamUser014::AdvertiseGame(CSteamID steamIDGameServer, uint32 unIPServer, uint16 usPortServer)
{
	return true;
}

static unsigned char ticketData[128];

SteamAPICall_t CSteamUser014::RequestEncryptedAppTicket(const void *pUserData, int cbUserData)
{
	// TODO: implement this this is important
	Trace("SteamUser014", "RequestEncryptedAppTicket");

	unsigned __int64 userID = GetSteamID().ConvertToUint64();

	memset(ticketData, 0, sizeof(ticketData));
	memcpy(&ticketData[32], pUserData, min(cbUserData, sizeof(ticketData)));
	memcpy(&ticketData[32 + cbUserData], &userID, sizeof(userID));

	SteamAPICall_t callID = CSteamBase::RegisterCall();

	RequestAppTicketResponse_t* response = (RequestAppTicketResponse_t*)malloc(sizeof(RequestAppTicketResponse_t));
	response->m_EResult = k_EResultOK;

	CSteamBase::ReturnCall(response, sizeof(RequestAppTicketResponse_t), 0, callID);

	return callID;
}

bool CSteamUser014::GetEncryptedAppTicket(void *pTicket, int cbMaxTicket, uint32 *pcbTicket)
{
	Trace("SteamUser014", "GetEncryptedAppTicket");
	memcpy(pTicket, ticketData, min(cbMaxTicket, sizeof(ticketData)));
	*pcbTicket = sizeof(ticketData);
	return true;
}
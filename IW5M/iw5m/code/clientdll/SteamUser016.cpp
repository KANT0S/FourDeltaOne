// ==========================================================
// alterIWnet project
// 
// Component: aiw_client
// Sub-component: steam_api
// Purpose: ISteamUser016 implementation
//
// Initial author: NTAuthority
// Started: 2010-09-10
// ==========================================================

#include "StdInc.h"
#include "SteamUser016.h"

HSteamUser CSteamUser016::GetHSteamUser()
{
	return NULL;
}

bool CSteamUser016::BLoggedOn()
{
	return true;
}

CSteamID CSteamUser016::GetSteamID()
{
	NPID npID;
	NP_GetNPID(&npID);
	
	return CSteamID(npID);
}

int CSteamUser016::InitiateGameConnection( void *pAuthBlob, int cbMaxAuthBlob, CSteamID steamIDGameServer, uint32 unIPServer, uint16 usPortServer, bool bSecure )
{
	if (!NP_GetUserTicket(pAuthBlob, cbMaxAuthBlob, steamIDGameServer.ConvertToUint64()))
	{
		return 0;
	}

	return sizeof(NPAuthenticateTicket);
}

void CSteamUser016::TerminateGameConnection( uint32 unIPServer, uint16 usPortServer )
{
}

void CSteamUser016::TrackAppUsageEvent( CGameID gameID, EAppUsageEvent eAppUsageEvent, const char *pchExtraInfo )
{
}

bool CSteamUser016::GetUserDataFolder( char *pchBuffer, int cubBuffer )
{
	return true;
}

void CSteamUser016::StartVoiceRecording( )
{
}

void CSteamUser016::StopVoiceRecording( )
{
}

EVoiceResult CSteamUser016::GetAvailableVoice(uint32 *pcbCompressed, uint32 *pcbUncompressed, uint32 nUncompressedVoiceDesiredSampleRate)
{
	return k_EVoiceResultOK;
}

EVoiceResult CSteamUser016::GetVoice( bool bWantCompressed, void *pDestBuffer, uint32 cbDestBufferSize, uint32 *nBytesWritten, bool bWantUncompressed, void *pUncompressedDestBuffer, uint32 cbUncompressedDestBufferSize, uint32 *nUncompressBytesWritten, uint32 nUncompressedVoiceDesiredSampleRate )
{
	return k_EVoiceResultOK;
}

EVoiceResult CSteamUser016::DecompressVoice( const void *pCompressed, uint32 cbCompressed, void *pDestBuffer, uint32 cbDestBufferSize, uint32 *nBytesWritten, uint32 nDesiredSampleRate )
{
	return k_EVoiceResultOK;
}

uint32 CSteamUser016::GetVoiceOptimalSampleRate()
{
	return 44100;
}

HAuthTicket CSteamUser016::GetAuthSessionTicket( void *pTicket, int cbMaxTicket, uint32 *pcbTicket )
{
	return 0;
}

EBeginAuthSessionResult CSteamUser016::BeginAuthSession( const void *pAuthTicket, int cbAuthTicket, CSteamID steamID )
{
	return k_EBeginAuthSessionResultOK;
}

void CSteamUser016::EndAuthSession( CSteamID steamID )
{

}

void CSteamUser016::CancelAuthTicket( HAuthTicket hAuthTicket )
{

}

EUserHasLicenseForAppResult CSteamUser016::UserHasLicenseForApp( CSteamID steamID, AppId_t appID )
{
	return k_EUserHasLicenseResultHasLicense;
}

bool CSteamUser016::BIsBehindNAT()
{
	return false;
}

void CSteamUser016::AdvertiseGame(CSteamID steamIDGameServer, uint32 unIPServer, uint16 usPortServer)
{

}

static unsigned char ticketData[128];

SteamAPICall_t SteamProxy_RequestEncryptedAppTicket(void *pUserData, int cbUserData);
bool SteamProxy_GetEncryptedAppTicket(void *pTicket, int cbMaxTicket, uint32 *pcbTicket);

SteamAPICall_t CSteamUser016::RequestEncryptedAppTicket(void *pUserData, int cbUserData)
{
	// TODO: implement this this is important
	Trace("SteamUser016", "RequestEncryptedAppTicket");
#if STEAM_PROXY
	return SteamProxy_RequestEncryptedAppTicket(pUserData, cbUserData);
#endif

	unsigned __int64 userID = GetSteamID().ConvertToUint64();

	memset(ticketData, 0, sizeof(ticketData));
	strcpy((char*)ticketData, Auth_GetSessionID());
	memcpy(&ticketData[32], pUserData, min(cbUserData, sizeof(ticketData)));
	memcpy(&ticketData[32 + cbUserData], &userID, sizeof(userID));

	SteamAPICall_t callID = CSteamBase::RegisterCall();

	EncryptedAppTicketResponse_t* response = (EncryptedAppTicketResponse_t*)malloc(sizeof(EncryptedAppTicketResponse_t));
	response->m_eResult = k_EResultOK;

	CSteamBase::ReturnCall(response, sizeof(EncryptedAppTicketResponse_t), 0, callID);

	return callID;
}

bool CSteamUser016::GetEncryptedAppTicket(void *pTicket, int cbMaxTicket, uint32 *pcbTicket)
{
	Trace("SteamUser016", "GetEncryptedAppTicket");
#if STEAM_PROXY
	return SteamProxy_GetEncryptedAppTicket(pTicket, cbMaxTicket, pcbTicket);
#endif

	memcpy(pTicket, ticketData, min(cbMaxTicket, sizeof(ticketData)));
	*pcbTicket = sizeof(ticketData);
	return true;
}
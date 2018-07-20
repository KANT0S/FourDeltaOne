// ==========================================================
// Project T5
// 
// Component: t5code
// Sub-component: clientdll
// Purpose: ISteamFriends007 implementation
//
// Initial author: NTAuthority
// Started: 2011-03-31
// ==========================================================

#include "stdinc.h"
#include "SteamFriends007.h"

char* Auth_GetUsername();

const char *CSteamFriends007::GetPersonaName()
{
	return Auth_GetUsername();
}

void CSteamFriends007::SetPersonaName( const char *pchPersonaName )
{
}

EPersonaState CSteamFriends007::GetPersonaState()
{


	return k_EPersonaStateOnline;
}

int CSteamFriends007::GetFriendCount( EFriendFlags iFriendFlags )
{
	return NP_GetNumFriends();
}

CSteamID CSteamFriends007::GetFriendByIndex( int iFriend, int iFriendFlags )
{
	return CSteamID(NP_GetFriend(iFriend));
}

EFriendRelationship CSteamFriends007::GetFriendRelationship( CSteamID steamIDFriend )
{
	return k_EFriendRelationshipNone;

	if (NP_GetFriendName(steamIDFriend.ConvertToUint64()) != NULL)
	{
		return k_EFriendRelationshipFriend;
	}

	return k_EFriendRelationshipNone;
}

EPersonaState CSteamFriends007::GetFriendPersonaState( CSteamID steamIDFriend )
{
	switch (NP_GetFriendPresence(steamIDFriend.ConvertToUint64()))
	{
		case PresenceStateOnline:
			return k_EPersonaStateOnline;
		case PresenceStateAway:
			return k_EPersonaStateAway;
		case PresenceStateExtendedAway:
			return k_EPersonaStateSnooze;
	}

	return k_EPersonaStateOffline;
}

const char *CSteamFriends007::GetFriendPersonaName( CSteamID steamIDFriend )
{
	//return steamIDFriend.Render();

	return NP_GetFriendName(steamIDFriend.ConvertToUint64());
}

int CSteamFriends007::GetSmallFriendAvatar( CSteamID steamIDFriend )
{
	return 0;
}

int CSteamFriends007::GetMediumFriendAvatar( CSteamID steamIDFriend )
{
	return 0;
}

int CSteamFriends007::GetLargeFriendAvatar( CSteamID steamIDFriend )
{
	return 0;
}

bool CSteamFriends007::GetFriendGamePlayed( CSteamID steamIDFriend, FriendGameInfo_t *pFriendGameInfo )
{
	const char* friendGame = NP_GetFriendRichPresence(steamIDFriend.ConvertToUint64(), "currentGame");

	if (friendGame)
	{
		if (!strcmp(friendGame, "t5mp"))
		{
			pFriendGameInfo->m_gameID = CGameID(42710);
		}
		else
		{
			pFriendGameInfo->m_gameID = CGameID(42700);
		}

		return true;
	}

	//pFriendGameInfo->m_gameID = CGameID(42710);
	return false;
}

const char *CSteamFriends007::GetFriendPersonaNameHistory( CSteamID steamIDFriend, int iPersonaName )
{


	return "";
}

bool CSteamFriends007::HasFriend( CSteamID steamIDFriend, EFriendFlags iFriendFlags )
{


	return false;
}

int CSteamFriends007::GetClanCount()
{


	return 0;
}

CSteamID CSteamFriends007::GetClanByIndex( int iClan )
{


	return CSteamID();
}

const char *CSteamFriends007::GetClanName( CSteamID steamIDClan )
{


	return "c14n";
}

const char *CSteamFriends007::GetClanTag( CSteamID steamIDClan )
{
	return "3arc";
}

int CSteamFriends007::GetFriendCountFromSource( CSteamID steamIDSource )
{


	return 0;
}

CSteamID CSteamFriends007::GetFriendFromSourceByIndex( CSteamID steamIDSource, int iFriend )
{


	return CSteamID();
}

bool CSteamFriends007::IsUserInSource( CSteamID steamIDUser, CSteamID steamIDSource )
{


	return false;
}

void CSteamFriends007::SetInGameVoiceSpeaking( CSteamID steamIDUser, bool bSpeaking )
{

}

void CSteamFriends007::ActivateGameOverlay( const char *pchDialog )
{

}

void CSteamFriends007::ActivateGameOverlayToUser( const char *pchDialog, CSteamID steamID )
{

}

void CSteamFriends007::ActivateGameOverlayToWebPage( const char *pchURL )
{

}

void CSteamFriends007::ActivateGameOverlayToStore( AppId_t nAppID )
{

}

void CSteamFriends007::SetPlayedWith( CSteamID steamIDUserPlayedWith )
{

}

void CSteamFriends007::ActivateGameOverlayInviteDialog( CSteamID steamIDLobby ) {}
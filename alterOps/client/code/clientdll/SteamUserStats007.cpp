// ==========================================================
// alterOps project
// 
// Component: client
// Sub-component: clientdll
// Purpose: ISteamUserStats007 implementation
//
// Initial author: NTAuthority
// Started: 2010-11-21
// ==========================================================

#include "StdInc.h"
#include "SteamUserStats007.h"

// Ask the server to send down this user's data and achievements for this game
bool CSteamUserStats007::RequestCurrentStats()
{
	//Logger(lSAPI, "SteamUserStats007", "Requesting our current stats.");
	return true;
}

// Data accessors
bool CSteamUserStats007::GetStat( const char *pchName, int32 *pData )
{
	return false;
}

bool CSteamUserStats007::GetStat( const char *pchName, float *pData )
{
	return false;
}

// Set / update data
bool CSteamUserStats007::SetStat( const char *pchName, int32 nData )
{
	//Logger(lSAPI, "SteamUserStats007", "Setting out stats.");
	return false;
}

bool CSteamUserStats007::SetStat( const char *pchName, float fData )
{
	return false;
}

bool CSteamUserStats007::UpdateAvgRateStat( const char *pchName, float flCountThisSession, double dSessionLength )
{
	return false;
}

// Achievement flag accessors
bool CSteamUserStats007::GetAchievement( const char *pchName, bool *pbAchieved )
{
	*pbAchieved = false;
	return false;
}

bool CSteamUserStats007::SetAchievement( const char *pchName )
{
	return false;
}

bool CSteamUserStats007::ClearAchievement( const char *pchName )
{
	return false;
}

// most likely a bool return here
bool CSteamUserStats007::GetAchievementAndUnlockTime( const char *pchName, bool *pbAchieved, RTime32 *prtTime )
{
	return false;
}

// Store the current data on the server, will get a callback when set
// And one callback for every new achievement
bool CSteamUserStats007::StoreStats()
{
	return true;
}

// Gets the icon of the achievement, which is a handle to be used in IClientUtils::GetImageRGBA(), or 0 if none set
int CSteamUserStats007::GetAchievementIcon( const char *pchName )
{
	return 0;
}
// Get general attributes (display name / text, etc) for an Achievement
const char *CSteamUserStats007::GetAchievementDisplayAttribute( const char *pchName, const char *pchKey )
{
	return "";
}

// Achievement progress - triggers an AchievementProgress callback, that is all.
// Calling this w/ N out of N progress will NOT set the achievement, the game must still do that.
bool CSteamUserStats007::IndicateAchievementProgress( const char *pchName, uint32 nCurProgress, uint32 nMaxProgress )
{
	return false;
}

// Friends stats & achievements

// downloads stats for the user
// returns a UserStatsReceived_t received when completed
// if the other user has no stats, UserStatsReceived_t.m_eResult will be set to k_EResultFail
// these stats won't be auto-updated; you'll need to call RequestUserStats() again to refresh any data
SteamAPICall_t CSteamUserStats007::RequestUserStats( CSteamID steamIDUser )
{
	return 0;
}


// requests stat information for a user, usable after a successful call to RequestUserStats()
bool CSteamUserStats007::GetUserStat( CSteamID steamIDUser, const char *pchName, int32 *pData )
{
	return false;
}

bool CSteamUserStats007::GetUserStat( CSteamID steamIDUser, const char *pchName, float *pData )
{
	return false;
}

bool CSteamUserStats007::GetUserAchievement( CSteamID steamIDUser, const char *pchName, bool *pbAchieved )
{
	return false;
}

bool CSteamUserStats007::GetUserAchievementAndUnlockTime( CSteamID steamIDUser, const char *pchName, bool *pbAchieved, RTime32 *prtTime )
{
	return false;
}

// Reset stats 
bool CSteamUserStats007::ResetAllStats( bool bAchievementsToo )
{
	return false;
}


// Leaderboard functions

// asks the Steam back-end for a leaderboard by name, and will create it if it's not yet
// This call is asynchronous, with the result returned in LeaderboardFindResult_t
SteamAPICall_t CSteamUserStats007::FindOrCreateLeaderboard( const char *pchLeaderboardName, ELeaderboardSortMethod eLeaderboardSortMethod, ELeaderboardDisplayType eLeaderboardDisplayType )
{
	return 0;
}

// as above, but won't create the leaderboard if it's not found
// This call is asynchronous, with the result returned in LeaderboardFindResult_t
SteamAPICall_t CSteamUserStats007::FindLeaderboard( const char *pchLeaderboardName )
{
	return 0;
}


// returns the name of a leaderboard
const char *CSteamUserStats007::GetLeaderboardName( SteamLeaderboard_t hSteamLeaderboard )
{
	return "";
}

// returns the total number of entries in a leaderboard, as of the last request
int CSteamUserStats007::GetLeaderboardEntryCount( SteamLeaderboard_t hSteamLeaderboard )
{
	return 0;
}

// returns the sort method of the leaderboard
ELeaderboardSortMethod CSteamUserStats007::GetLeaderboardSortMethod( SteamLeaderboard_t hSteamLeaderboard )
{
	return k_ELeaderboardSortMethodNone;
}

// returns the display type of the leaderboard
ELeaderboardDisplayType CSteamUserStats007::GetLeaderboardDisplayType( SteamLeaderboard_t hSteamLeaderboard )
{
	return k_ELeaderboardDisplayTypeNone;
}

// Asks the Steam back-end for a set of rows in the leaderboard.
// This call is asynchronous, with the result returned in LeaderboardScoresDownloaded_t
// LeaderboardScoresDownloaded_t will contain a handle to pull the results from GetDownloadedLeaderboardEntries() (below)
// You can ask for more entries than exist, and it will return as many as do exist.
// k_ELeaderboardDataRequestGlobal requests rows in the leaderboard from the full table, with nRangeStart & nRangeEnd in the range [1, TotalEntries]
// k_ELeaderboardDataRequestGlobalAroundUser requests rows around the current user, nRangeStart being negate
//   e.g. DownloadLeaderboardEntries( hLeaderboard, k_ELeaderboardDataRequestGlobalAroundUser, -3, 3 ) will return 7 rows, 3 before the user, 3 after
// k_ELeaderboardDataRequestFriends requests all the rows for friends of the current user 
SteamAPICall_t CSteamUserStats007::DownloadLeaderboardEntries( SteamLeaderboard_t hSteamLeaderboard, ELeaderboardDataRequest eLeaderboardDataRequest, int nRangeStart, int nRangeEnd )
{
	return 0;
}

// Returns data about a single leaderboard entry
// use a for loop from 0 to LeaderboardScoresDownloaded_t::m_cEntryCount to get all the downloaded entries
// e.g.
//		void OnLeaderboardScoresDownloaded( LeaderboardScoresDownloaded_t *pLeaderboardScoresDownloaded )
//		{
//			for ( int index; index < pLeaderboardScoresDownloaded->m_cEntryCount; index++ )
//			{
//				LeaderboardEntry_t leaderboardEntry;
//				int32 details[3];		// we know this is how many we've stored previously
//				GetDownloadedLeaderboardEntry( pLeaderboardScoresDownloaded->m_hSteamLeaderboardEntries, index, &leaderboardEntry, details, 3 );
//				assert( leaderboardEntry.m_cDetails == 3 );
//				...
//			}
// once you've accessed all the entries, the data will be free'd, and the SteamLeaderboardEntries_t handle will become invalid
bool CSteamUserStats007::GetDownloadedLeaderboardEntry( SteamLeaderboardEntries_t hSteamLeaderboardEntries, int index, LeaderboardEntry_t *pLeaderboardEntry, int32 *pDetails, int cDetailsMax )
{
	return 0;
}

// Uploads a user score to the Steam back-end.
// This call is asynchronous, with the result returned in LeaderboardScoreUploaded_t
// If the score passed in is no better than the existing score this user has in the leaderboard, then the leaderboard will not be updated.
// Details are extra game-defined information regarding how the user got that score
// pScoreDetails points to an array of int32's, cScoreDetailsCount is the number of int32's in the list
SteamAPICall_t CSteamUserStats007::UploadLeaderboardScore( SteamLeaderboard_t hSteamLeaderboard,ELeaderboardUploadScoreMethod eLeaderboardUploadScoreMethod, int32 nScore, int32 *pScoreDetails, int cScoreDetailsCount )
{
	return 0;
}

// Retrieves the number of players currently playing your game (online + offline)
// This call is asynchronous, with the result returned in NumberOfCurrentPlayers_t
SteamAPICall_t CSteamUserStats007::GetNumberOfCurrentPlayers()
{
	Trace("SteamUserStats", "GetNumberOfCurrentPlayers");
	return 0;
}

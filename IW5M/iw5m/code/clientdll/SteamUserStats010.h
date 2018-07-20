// ==========================================================
// alterOps project
// 
// Component: client
// Sub-component: clientdll
// Purpose: ISteamUserStats007 definition
//
// Initial author: Open Steamworks project
// Started: 2010-11-21
// ==========================================================

class CSteamUserStats010 : public ISteamUserStats010
{
public:
	// Ask the server to send down this user's data and achievements for this game
	bool RequestCurrentStats();

	// Data accessors
	bool GetStat( const char *pchName, int32 *pData );
	bool GetStat( const char *pchName, float *pData );

	// Set / update data
	bool SetStat( const char *pchName, int32 nData );
	bool SetStat( const char *pchName, float fData );
	bool UpdateAvgRateStat( const char *pchName, float flCountThisSession, double dSessionLength );

	// Achievement flag accessors
	bool GetAchievement( const char *pchName, bool *pbAchieved );
	bool SetAchievement( const char *pchName );
	bool ClearAchievement( const char *pchName );

	// most likely a bool return here
	bool GetAchievementAndUnlockTime( const char *pchName, bool *pbAchieved, RTime32 *prtTime );

	// Store the current data on the server, will get a callback when set
	// And one callback for every new achievement
	bool StoreStats();

	// Gets the icon of the achievement, which is a handle to be used in IClientUtils::GetImageRGBA(), or 0 if none set
	int GetAchievementIcon( const char *pchName );
	// Get general attributes (display name / text, etc) for an Achievement
	const char *GetAchievementDisplayAttribute( const char *pchName, const char *pchKey );

	// Achievement progress - triggers an AchievementProgress callback, that is all.
	// Calling this w/ N out of N progress will NOT set the achievement, the game must still do that.
	bool IndicateAchievementProgress( const char *pchName, uint32 nCurProgress, uint32 nMaxProgress );

	// Friends stats & achievements

	// downloads stats for the user
	// returns a UserStatsReceived_t received when completed
	// if the other user has no stats, UserStatsReceived_t.m_eResult will be set to k_EResultFail
	// these stats won't be auto-updated; you'll need to call RequestUserStats() again to refresh any data
	SteamAPICall_t RequestUserStats( CSteamID steamIDUser );


	// requests stat information for a user, usable after a successful call to RequestUserStats()
	bool GetUserStat( CSteamID steamIDUser, const char *pchName, int32 *pData );
	bool GetUserStat( CSteamID steamIDUser, const char *pchName, float *pData );
	bool GetUserAchievement( CSteamID steamIDUser, const char *pchName, bool *pbAchieved );
	bool GetUserAchievementAndUnlockTime( CSteamID steamIDUser, const char *pchName, bool *pbAchieved, RTime32 *prtTime );

	// Reset stats 
	bool ResetAllStats( bool bAchievementsToo );


	// Leaderboard functions

	// asks the Steam back-end for a leaderboard by name, and will create it if it's not yet
	// This call is asynchronous, with the result returned in LeaderboardFindResult_t
	SteamAPICall_t FindOrCreateLeaderboard( const char *pchLeaderboardName, ELeaderboardSortMethod eLeaderboardSortMethod, ELeaderboardDisplayType eLeaderboardDisplayType );

	// as above, but won't create the leaderboard if it's not found
	// This call is asynchronous, with the result returned in LeaderboardFindResult_t
	SteamAPICall_t FindLeaderboard( const char *pchLeaderboardName );


	// returns the name of a leaderboard
	const char *GetLeaderboardName( SteamLeaderboard_t hSteamLeaderboard );

	// returns the total number of entries in a leaderboard, as of the last request
	int GetLeaderboardEntryCount( SteamLeaderboard_t hSteamLeaderboard );

	// returns the sort method of the leaderboard
	ELeaderboardSortMethod GetLeaderboardSortMethod( SteamLeaderboard_t hSteamLeaderboard );

	// returns the display type of the leaderboard
	ELeaderboardDisplayType GetLeaderboardDisplayType( SteamLeaderboard_t hSteamLeaderboard );

	// Asks the Steam back-end for a set of rows in the leaderboard.
	// This call is asynchronous, with the result returned in LeaderboardScoresDownloaded_t
	// LeaderboardScoresDownloaded_t will contain a handle to pull the results from GetDownloadedLeaderboardEntries() (below)
	// You can ask for more entries than exist, and it will return as many as do exist.
	// k_ELeaderboardDataRequestGlobal requests rows in the leaderboard from the full table, with nRangeStart & nRangeEnd in the range [1, TotalEntries]
	// k_ELeaderboardDataRequestGlobalAroundUser requests rows around the current user, nRangeStart being negate
	//   e.g. DownloadLeaderboardEntries( hLeaderboard, k_ELeaderboardDataRequestGlobalAroundUser, -3, 3 ) will return 7 rows, 3 before the user, 3 after
	// k_ELeaderboardDataRequestFriends requests all the rows for friends of the current user 
	SteamAPICall_t DownloadLeaderboardEntries( SteamLeaderboard_t hSteamLeaderboard, ELeaderboardDataRequest eLeaderboardDataRequest, int nRangeStart, int nRangeEnd );

	// as above, but downloads leaderboard entries for an arbitrary set of users - ELeaderboardDataRequest is k_ELeaderboardDataRequestUsers
	// if a user doesn't have a leaderboard entry, they won't be included in the result
	// a max of 100 users can be downloaded at a time, with only one outstanding call at a time
	SteamAPICall_t DownloadLeaderboardEntriesForUsers( SteamLeaderboard_t hSteamLeaderboard, CSteamID *prgUsers, int cUsers );

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
	bool GetDownloadedLeaderboardEntry( SteamLeaderboardEntries_t hSteamLeaderboardEntries, int index, LeaderboardEntry_t *pLeaderboardEntry, int32 *pDetails, int cDetailsMax );

	// Uploads a user score to the Steam back-end.
	// This call is asynchronous, with the result returned in LeaderboardScoreUploaded_t
	// If the score passed in is no better than the existing score this user has in the leaderboard, then the leaderboard will not be updated.
	// Details are extra game-defined information regarding how the user got that score
	// pScoreDetails points to an array of int32's, cScoreDetailsCount is the number of int32's in the list
	SteamAPICall_t UploadLeaderboardScore( SteamLeaderboard_t hSteamLeaderboard, ELeaderboardUploadScoreMethod eLeaderboardUploadScoreMethod, int32 nScore, const int32 *pScoreDetails, int cScoreDetailsCount );

	// Attaches a piece of user generated content the user's entry on a leaderboard.
	// hContent is a handle to a piece of user generated content that was shared using ISteamUserRemoteStorage::FileShare().
	// This call is asynchronous, with the result returned in LeaderboardUGCSet_t.
	SteamAPICall_t AttachLeaderboardUGC( SteamLeaderboard_t hSteamLeaderboard, UGCHandle_t hUGC );

	// Retrieves the number of players currently playing your game (online + offline)
	// This call is asynchronous, with the result returned in NumberOfCurrentPlayers_t
	SteamAPICall_t GetNumberOfCurrentPlayers();

	// Requests that Steam fetch data on the percentage of players who have received each achievement
	// for the game globally.
	// This call is asynchronous, with the result returned in GlobalAchievementPercentagesReady_t.
	SteamAPICall_t RequestGlobalAchievementPercentages() ;

	// Get the info on the most achieved achievement for the game, returns an iterator index you can use to fetch
	// the next most achieved afterwards.  Will return -1 if there is no data on achievement 
	// percentages (ie, you haven't called RequestGlobalAchievementPercentages and waited on the callback).
	int GetMostAchievedAchievementInfo( char *pchName, uint32 unNameBufLen, float *pflPercent, bool *pbAchieved ) ;

	// Get the info on the next most achieved achievement for the game. Call this after GetMostAchievedAchievementInfo or another
	// GetNextMostAchievedAchievementInfo call passing the iterator from the previous call. Returns -1 after the last
	// achievement has been iterated.
	int GetNextMostAchievedAchievementInfo( int iIteratorPrevious, char *pchName, uint32 unNameBufLen, float *pflPercent, bool *pbAchieved ) ;

	// Returns the percentage of users who have achieved the specified achievement.
	bool GetAchievementAchievedPercent( const char *pchName, float *pflPercent ) ;

	// Requests global stats data, which is available for stats marked as "aggregated".
	// This call is asynchronous, with the results returned in GlobalStatsReceived_t.
	// nHistoryDays specifies how many days of day-by-day history to retrieve in addition
	// to the overall totals. The limit is 60.
	SteamAPICall_t RequestGlobalStats( int nHistoryDays ) ;

	// Gets the lifetime totals for an aggregated stat
	bool GetGlobalStat( const char *pchStatName, int64 *pData ) ;
	bool GetGlobalStat( const char *pchStatName, double *pData ) ;

	// Gets history for an aggregated stat. pData will be filled with daily values, starting with today.
	// So when called, pData[0] will be today, pData[1] will be yesterday, and pData[2] will be two days ago, 
	// etc. cubData is the size in bytes of the pubData buffer. Returns the number of 
	// elements actually set.
	int32 GetGlobalStatHistory( const char *pchStatName, int64 *pData, uint32 cubData ) ;
	int32 GetGlobalStatHistory( const char *pchStatName, double *pData, uint32 cubData ) ;
};
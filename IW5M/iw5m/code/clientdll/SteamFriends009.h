// ==========================================================
// Project T5
// 
// Component: t5code
// Sub-component: clientdll
// Purpose: ISteamFriends007 definition
//
// Initial author: Open Steamworks project
// Started: 2011-03-31
// ==========================================================

class CSteamFriends009 : public ISteamFriends009
{
public:
	// returns the local players name - guaranteed to not be NULL.
	// this is the same name as on the users community profile page
	// this is stored in UTF-8 format
	// like all the other interface functions that return a char *, it's important that this pointer is not saved
	// off; it will eventually be free'd or re-allocated
	const char *GetPersonaName();

	// sets the player name, stores it on the server and publishes the changes to all friends who are online
	void SetPersonaName( const char *pchPersonaName );

	// gets the status of the current user
	EPersonaState GetPersonaState();

	// friend iteration
	// takes a set of k_EFriendFlags, and returns the number of users the client knows about who meet that criteria
	// then GetFriendByIndex() can then be used to return the id's of each of those users
	int GetFriendCount( int eFriendFlags );

	// returns the steamID of a user
	// iFriend is a index of range [0, GetFriendCount())
	// iFriendsFlags must be the same value as used in GetFriendCount()
	// the returned CSteamID can then be used by all the functions below to access details about the user
	CSteamID GetFriendByIndex( int iFriend, int iFriendFlags );

	// returns a relationship to a user
	EFriendRelationship GetFriendRelationship( CSteamID steamIDFriend );

	// returns the current status of the specified user
	// this will only be known by the local user if steamIDFriend is in their friends list; on the same game server; in a chat room or lobby; or in a small group with the local user
	EPersonaState GetFriendPersonaState( CSteamID steamIDFriend );

	// returns the name another user - guaranteed to not be NULL.
	// same rules as GetFriendPersonaState() apply as to whether or not the user knowns the name of the other user
	// note that on first joining a lobby, chat room or game server the local user will not known the name of the other users automatically; that information will arrive asyncronously
	const char *GetFriendPersonaName( CSteamID steamIDFriend );

	// returns true if the friend is actually in a game, and fills in pFriendGameInfo with an extra details 
	bool GetFriendGamePlayed( CSteamID steamIDFriend, FriendGameInfo_t *pFriendGameInfo );
	// accesses old friends names - returns an empty string when their are no more items in the history
	const char *GetFriendPersonaNameHistory( CSteamID steamIDFriend, int iPersonaName );

	// returns true if the specified user meets any of the criteria specified in iFriendFlags
	// iFriendFlags can be the union (binary or, |) of one or more k_EFriendFlags values
	bool HasFriend( CSteamID steamIDFriend, int eFriendFlags );

	// clan (group) iteration and access functions
	int GetClanCount();
	CSteamID GetClanByIndex( int iClan );
	const char *GetClanName( CSteamID steamIDClan );
	const char *GetClanTag( CSteamID steamIDClan );

	// iterators for getting users in a chat room, lobby, game server or clan
	// note that large clans that cannot be iterated by the local user
	// steamIDSource can be the steamID of a group, game server, lobby or chat room
	int GetFriendCountFromSource( CSteamID steamIDSource );
	CSteamID GetFriendFromSourceByIndex( CSteamID steamIDSource, int iFriend );

	// returns true if the local user can see that steamIDUser is a member or in steamIDSource
	bool IsUserInSource( CSteamID steamIDUser, CSteamID steamIDSource );

	// User is in a game pressing the talk button (will suppress the microphone for all voice comms from the Steam friends UI)
	void SetInGameVoiceSpeaking( CSteamID steamIDUser, bool bSpeaking );

	// activates the game overlay, with an optional dialog to open 
	// valid options are "Friends", "Community", "Players", "Settings", "LobbyInvite", "OfficialGameGroup"
	void ActivateGameOverlay( const char *pchDialog );

	// activates game overlay to a specific place
	// valid options are
	//		"steamid" - opens the overlay web browser to the specified user or groups profile
	//		"chat" - opens a chat window to the specified user, or joins the group chat 
	void ActivateGameOverlayToUser( const char *pchDialog, CSteamID steamID );

	// activates game overlay web browser directly to the specified URL
	// full address with protocol type is required, e.g. http://www.steamgames.com/
	void ActivateGameOverlayToWebPage( const char *pchURL );

	// activates game overlay to store page for app
	void ActivateGameOverlayToStore( AppId_t nAppID );

	// Mark a target user as 'played with'. This is a client-side only feature that requires that the calling user is 
	// in game 
	void SetPlayedWith( CSteamID steamIDUserPlayedWith );

	// activates game overlay to open the invite dialog. Invitations will be sent for the provided lobby.
	// You can also use ActivateGameOverlay( "LobbyInvite" ) to allow the user to create invitations for their current public lobby.
	void ActivateGameOverlayInviteDialog( CSteamID steamIDLobby );

	// gets the avatar of the current user, which is a handle to be used in IClientUtils::GetImageRGBA(), or 0 if none set
	int GetSmallFriendAvatar( CSteamID steamIDFriend );
	int GetMediumFriendAvatar( CSteamID steamIDFriend );
	int GetLargeFriendAvatar( CSteamID steamIDFriend );

	// requests information about a user - persona name & avatar
	// if bRequireNameOnly is set, then the avatar of a user isn't downloaded 
	// - it's a lot slower to download avatars and churns the local cache, so if you don't need avatars, don't request them
	// if returns true, it means that data is being requested, and a PersonaStateChanged_t callback will be posted when it's retrieved
	// if returns false, it means that we already have all the details about that user, and functions can be called immediately
	bool RequestUserInformation( CSteamID steamIDUser, bool bRequireNameOnly );

	// requests information about a clan officer list
	// when complete, data is returned in ClanOfficerListResponse_t call result
	// this makes available the calls below
	// you can only ask about clans that a user is a member of
	// note that this won't download avatars automatically; if you get an officer,
	// and no avatar image is available, call RequestUserInformation( steamID, false ) to download the avatar
	SteamAPICall_t RequestClanOfficerList( CSteamID steamIDClan );

	// iteration of clan officers - can only be done when a RequestClanOfficerList() call has completed

	// returns the steamID of the clan owner
	CSteamID GetClanOwner(CSteamID steamIDClan); /*CSteamID GetClanOwner( CSteamID steamIDClan );*/
		// returns the number of officers in a clan (including the owner)
		int GetClanOfficerCount( CSteamID steamIDClan );
	// returns the steamID of a clan officer, by index, of range [0,GetClanOfficerCount)
	CSteamID GetClanOfficerByIndex(CSteamID steamIDClan, int iOfficer); /*CSteamID GetClanOfficerByIndex( CSteamID steamIDClan, int iOfficer );*/
		// if current user is chat restricted, he can't send or receive any text/voice chat messages.
		// the user can't see custom avatars. But the user can be online and send/recv game invites.
		// a chat restricted user can't add friends or join any groups.
		EUserRestriction GetUserRestrictions();

	// Rich Presence data is automatically shared between friends who are in the same game
	// Each user has a set of Key/Value pairs
	// Up to 20 different keys can be set
	// There are two magic keys:
	//		"status"  - a UTF-8 string that will show up in the 'view game info' dialog in the Steam friends list
	//		"connect" - a UTF-8 string that contains the command-line for how a friend can connect to a game
	// GetFriendRichPresence() returns an empty string "" if no value is set
	// SetRichPresence() to a NULL or an empty string deletes the key
	// You can iterate the current set of keys for a friend with GetFriendRichPresenceKeyCount()
	// and GetFriendRichPresenceKeyByIndex() (typically only used for debugging)
	bool SetRichPresence( const char *pchKey, const char *pchValue );
	void ClearRichPresence();
	const char *GetFriendRichPresence( CSteamID steamIDFriend, const char *pchKey );
	int GetFriendRichPresenceKeyCount( CSteamID steamIDFriend );
	const char *GetFriendRichPresenceKeyByIndex( CSteamID steamIDFriend, int iKey );

	// rich invite support
	// if the target accepts the invite, the pchConnectString gets added to the command-line for launching the game
	// if the game is already running, a GameRichPresenceJoinRequested_t callback is posted containing the connect string
	// invites can only be sent to friends
	bool InviteUserToGame( CSteamID steamIDFriend, const char *pchConnectString );

	// recently-played-with friends iteration
	// this iterates the entire list of users recently played with, across games
	// GetFriendCoplayTime() returns as a unix time
	int GetCoplayFriendCount();
	CSteamID GetCoplayFriend(int iCoplayFriend); /*CSteamID GetCoplayFriend( int iCoplayFriend );*/
		int GetFriendCoplayTime( CSteamID steamIDFriend );
	AppId_t GetFriendCoplayGame( CSteamID steamIDFriend );
};

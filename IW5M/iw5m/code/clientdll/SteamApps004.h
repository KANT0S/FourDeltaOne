// ==========================================================
// Project T5
// 
// Component: t5code
// Sub-component: clientdll
// Purpose: ISteamApps004 definition
//
// Initial author: Open Steamworks project
// Started: 2011-03-31
// ==========================================================

class CSteamApps004 : public ISteamApps004
{
public:
	bool BIsSubscribed();
	bool BIsLowViolence();
	bool BIsCybercafe();
	bool BIsVACBanned();
	const char *GetCurrentGameLanguage();
	const char *GetAvailableGameLanguages();

	// only use this member if you need to check ownership of another game related to yours, a demo for example
	bool BIsSubscribedApp( AppId_t appID );

	// Takes AppID of DLC and checks if the user owns the DLC & if the DLC is installed
	bool BIsDlcInstalled( AppId_t appID );

	// returns the Unix time of the purchase of the app
	uint32 GetEarliestPurchaseUnixTime( AppId_t nAppID );

	// Checks if the user is subscribed to the current app through a free weekend
	// This function will return false for users who have a retail or other type of license
	// Before using, please ask your Valve technical contact how to package and secure your free weekened
	bool BIsSubscribedFromFreeWeekend();
	// Returns the number of DLC pieces for the running app
	int GetDLCCount();

	// Returns metadata for DLC by index, of range [0, GetDLCCount()]
	bool BGetDLCDataByIndex( int iDLC, AppId_t *pAppID, bool *pbAvailable, char *pchName, int cchNameBufferSize );

	// Install/Uninstall control for optional DLC
	void InstallDLC( AppId_t nAppID );
	void UninstallDLC( AppId_t nAppID );
};

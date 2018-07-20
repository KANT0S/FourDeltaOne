// ==========================================================
// Project T5
// 
// Component: t5code
// Sub-component: clientdll
// Purpose: ISteamApps003 definition
//
// Initial author: Open Steamworks project
// Started: 2011-03-31
// ==========================================================

class CSteamApps003 : public ISteamApps003
{
public:
	bool IsSubscribed();
	bool IsLowViolence();
	bool IsCybercafe();
	bool IsVACBanned();
	const char *GetCurrentGameLanguage();
	const char *GetAvailableGameLanguages();

	// only use this member if you need to check ownership of another game related to yours, a demo for example
	bool IsSubscribedApp( AppId_t appID );

	// Takes AppID of DLC and checks if the user owns the DLC & if the DLC is installed
	bool IsDlcInstalled( AppId_t appID );
};

// ==========================================================
// Project T5
// 
// Component: t5code
// Sub-component: clientdll
// Purpose: ISteamApps004 implementation
//
// Initial author: NTAuthority
// Started: 2011-03-31
// ==========================================================

#include "stdinc.h"
#include "SteamApps004.h"

bool CSteamApps004::BIsSubscribed()
{
	return true;
}

bool CSteamApps004::BIsLowViolence()
{
	return false;
}

bool CSteamApps004::BIsCybercafe()
{
	return false;
}

bool CSteamApps004::BIsVACBanned()
{
	return false;
}

const char *CSteamApps004::GetCurrentGameLanguage()
{
	return "english";
}

const char *CSteamApps004::GetAvailableGameLanguages()
{
	return "english";
}

bool CSteamApps004::BIsSubscribedApp( AppId_t appID )
{
	return true;
}

bool CSteamApps004::BIsDlcInstalled( AppId_t appID )
{
	return true;
}

uint32 CSteamApps004::GetEarliestPurchaseUnixTime( AppId_t nAppID )
{
	return 0;
}

bool CSteamApps004::BIsSubscribedFromFreeWeekend()
{
	return false;
}

int CSteamApps004::GetDLCCount()
{
	return 0;
}

bool CSteamApps004::BGetDLCDataByIndex( int iDLC, AppId_t *pAppID, bool *pbAvailable, char *pchName, int cchNameBufferSize )
{
	return false;
}

void CSteamApps004::InstallDLC( AppId_t nAppID )
{

}

void CSteamApps004::UninstallDLC( AppId_t nAppID )
{

}
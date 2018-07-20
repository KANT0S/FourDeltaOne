// ==========================================================
// interMW3 project
// 
// Component: code
// Sub-component: clientdll
// Purpose: ISteamRemoteStorage004 implementation
//
// Initial author: Terminator
// Started: 2012-06-1
// ==========================================================

#include "StdInc.h"
#include "SteamRemoteStorage002.h"

// file operations
bool CSteamRemoteStorage002::FileWrite( const char *pchFile, const void *pvData, int32 cubData )
{
	return true;
}

int32 CSteamRemoteStorage002::GetFileSize( const char *pchFile )
{
	//int32 size = (int32)StatSize(pchFile);
	return 12308;
}

int32 CSteamRemoteStorage002::FileRead( const char *pchFile, void *pvData, int32 cubDataToRead )
{
	return cubDataToRead;
}

bool CSteamRemoteStorage002::FileExists( const char *pchFile )
{
	return false;
}

// iteration
int32 CSteamRemoteStorage002::GetFileCount()
{
	return 0;
}

const char *CSteamRemoteStorage002::GetFileNameAndSize( int iFile, int32 *pnFileSizeInBytes )
{
	return "";
}

// quota management
bool CSteamRemoteStorage002::GetQuota(  int32 *pnTotalBytes, int32 *puAvailableBytes  )
{
	return true;
}
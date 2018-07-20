// ==========================================================
// interMW3 project
// 
// Component: code
// Sub-component: steam_api
// Purpose: ISteamRemoteStorage004 definition
//
// Initial author: Open Steamworks project
// Started: 2012-05-01
// ==========================================================

class CSteamRemoteStorage002 : public ISteamRemoteStorage002
{
public:
		// file operations
		bool	FileWrite( const char *pchFile, const void *pvData, int32 cubData );
		int32	GetFileSize( const char *pchFile );
		int32	FileRead( const char *pchFile, void *pvData, int32 cubDataToRead );
		bool	FileExists( const char *pchFile );

		// iteration
		int32 GetFileCount();
		const char *GetFileNameAndSize( int iFile, int32 *pnFileSizeInBytes );

		// quota management
		bool GetQuota( int32 *pnTotalBytes, int32 *puAvailableBytes );
};
// ==========================================================
// alterIWnet project
// 
// Component: aiw_client
// Sub-component: steam_api
// Purpose: Various generic utility functions.
//
// Initial author: NTAuthority
// Started: 2010-09-10
// ==========================================================

void Trace(char* source, char* message, ...);
const char *va( const char *fmt, ... );

// flag settings
#define GAME_FLAG_DEDICATED		(1 << 0)
#define GAME_FLAG_CONSOLE		(1 << 1)
#define GAME_FLAG_DUMPDATA		(1 << 2)
#define GAME_FLAG_ENTRIES		(1 << 3)
#define GAME_FLAG_GSCFILESYSTEM	(1 << 4)

//#ifndef PRE_RELEASE_DEMO
#define GAME_FLAG(x)			((_gameFlags & x) == x)
//#else
//#define GAME_FLAG(x)			false
//#endif

extern unsigned int _gameFlags;
void DetermineGameFlags();
const char* GetLicenseKey();
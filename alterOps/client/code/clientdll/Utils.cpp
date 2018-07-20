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

#include "StdInc.h"
#include <ShellAPI.h>
#include <sys/stat.h>
#include <direct.h>
#include <io.h>

// unmanaged as this uses vararg natively
#pragma unmanaged
void Trace(char* source, char* message, ...)
{
	va_list args;
	char buffer[1024];
	char buffer2[1024];

	va_start(args, message);
	_vsnprintf(buffer, sizeof(buffer), message, args);
	va_end(args);

	_snprintf(buffer2, sizeof(buffer2), "[%s] %s\n", source, buffer);

	OutputDebugStringA(buffer2);
}

// a funny thing is how this va() function could possibly come from leaked IW code.
#define VA_BUFFER_COUNT		4
#define VA_BUFFER_SIZE		4096

static char g_vaBuffer[VA_BUFFER_COUNT][VA_BUFFER_SIZE];
static int g_vaNextBufferIndex = 0;

const char *va( const char *fmt, ... )
{
	va_list ap;
	char *dest = &g_vaBuffer[g_vaNextBufferIndex][0];
	g_vaNextBufferIndex = (g_vaNextBufferIndex + 1) % VA_BUFFER_COUNT;
	va_start(ap, fmt);
	vsprintf( dest, fmt, ap );
	va_end(ap);
	return dest;
}

// determine which patchset to use
unsigned int _gameFlags;

typedef struct  
{
	const wchar_t* argument;
	unsigned int flag;
} flagDef_t;

flagDef_t flags[] =
{
	{ L"dedicated", GAME_FLAG_DEDICATED },
	{ L"console", GAME_FLAG_CONSOLE },
	{ L"dump", GAME_FLAG_DUMPDATA },
	{ L"entries", GAME_FLAG_ENTRIES },
	{ L"gscfilesystem", GAME_FLAG_GSCFILESYSTEM },
	{ 0, 0 }
};

bool hasLicenseKey = false;
char licenseKey[48];

const char* GetLicenseKey()
{
	return (hasLicenseKey) ? &licenseKey[1] : NULL;
}

void DetermineGameFlags()
{
	int numArgs;
	LPCWSTR commandLine = GetCommandLineW();
	LPWSTR* argv = CommandLineToArgvW(commandLine, &numArgs);

	for (int i = 0; i < numArgs; i++)
	{
		if (argv[i][0] == L'#' || argv[i][0] == L'@')
		{
			WideCharToMultiByte(CP_ACP, 0, argv[i], -1, licenseKey, sizeof(licenseKey), "?", NULL);

			hasLicenseKey = true;
		}

		if (argv[i][0] != L'-') continue;

		for (wchar_t* c = argv[i]; *c != L'\0'; c++)
		{
			if (*c != L'-')
			{
				for (flagDef_t* flag = flags; flag->argument; flag++)
				{
					if (!wcscmp(c, flag->argument))
					{
						_gameFlags |= flag->flag;
						break;
					}
				}
				break;
			}
		}
	}
}
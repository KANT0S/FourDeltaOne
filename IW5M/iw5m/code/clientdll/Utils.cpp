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
#include <shellapi.h>

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
	{ 0, 0 }
};

bool hasLicenseKey = false;
char licenseKey[48];

const char* GetLicenseKey()
{
	return (hasLicenseKey) ? &licenseKey[1] : "";
	//return (hasLicenseKey) ? &licenseKey[1] : "!!!!!!!!!!!!!!!!!!!!!!!!";
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

#define	MAX_INFO_STRING		1024
#define	MAX_INFO_KEY		  1024
#define	MAX_INFO_VALUE		1024

/*
===================
Info_RemoveKey
===================
*/
void Info_RemoveKey( char *s, const char *key ) {
	char	*start;
	char	pkey[MAX_INFO_KEY];
	char	value[MAX_INFO_VALUE];
	char	*o;

	if (strchr (key, '\\')) {
		return;
	}

	while (1)
	{
		start = s;
		if (*s == '\\')
			s++;
		o = pkey;
		while (*s != '\\')
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value;
		while (*s != '\\' && *s)
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;

		if (!strcmp (key, pkey) )
		{
			strcpy (start, s);	// remove this part
			return;
		}

		if (!*s)
			return;
	}

}

/*
==================
Info_SetValueForKey

Changes or adds a key/value pair
==================
*/
void Info_SetValueForKey( char *s, const char *key, const char *value ) {
	char    newi[MAX_INFO_STRING];
	const char* blacklist = "\\;\"";

	if ( strlen( s ) >= MAX_INFO_STRING ) {
		//Com_Printf( 0, "Info_SetValueForKey: oversize infostring");
		return;
	}

	for(; *blacklist; ++blacklist)
	{
		if (strchr (key, *blacklist) || strchr (value, *blacklist))
		{
			//Com_Printf (0, "Can't use keys or values with a '%c': %s = %s\n", *blacklist, key, value);
			return;
		}
	}

	Info_RemoveKey (s, key);
	if (!value || !strlen(value))
		return;

	_snprintf (newi, sizeof(newi), "\\%s\\%s", key, value);

	if (strlen(newi) + strlen(s) >= MAX_INFO_STRING)
	{
		//Com_Printf (0, "Info string length exceeded\n");
		return;
	}

	strcat (newi, s);
	strcpy (s, newi);
}

#define	BIG_INFO_STRING		8192  // used for system info key only
#define	BIG_INFO_KEY		  8192
#define	BIG_INFO_VALUE		8192

/*
===============
Info_ValueForKey

Searches the string for the given
key and returns the associated value, or an empty string.
FIXME: overflow check?
===============
*/
char *Info_ValueForKey( const char *s, const char *key )
{
	char	pkey[BIG_INFO_KEY];
	static	char value[2][BIG_INFO_VALUE];	// use two buffers so compares
	// work without stomping on each other
	static	int	valueindex = 0;
	char	*o;

	if ( !s || !key ) {
		return "";
	}

	if ( strlen( s ) >= BIG_INFO_STRING ) {
		//Com_Error( ERR_DROP, "Info_ValueForKey: oversize infostring" );
		return "";
	}

	valueindex ^= 1;
	if (*s == '\\')
		s++;
	while (1)
	{
		o = pkey;
		while (*s != '\\')
		{
			if (!*s)
				return "";
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value[valueindex];

		while (*s != '\\' && *s)
		{
			*o++ = *s++;
		}
		*o = 0;

		if (!_stricmp (key, pkey) )
			return value[valueindex];

		if (!*s)
			break;
		s++;
	}

	return "";
}
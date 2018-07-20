// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Project 'Inverse' - model loader
//
// Initial author: NTAuthority
// Started: 2013-02-19
// ==========================================================
/*
#include "StdInc.h"
#include "Inverse.h"
#include "tr_public.h"

refexport_t* re;

void RI_Printf(int printLevel, const char* fmt, ...)
{
	char message[16384];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf_s(message, sizeof(message), fmt, ap);
	va_end(ap);

	// TODO: support printLevels
	Com_Printf(0, "%s", message);
}

cvar_t* RI_Cvar_Get(const char* name, const char* value, int flags)
{
	char* newName = new char[strlen(name) + 1];
	newName[0] = 't';
	strcpy(&newName[1], name);

	// guess the variable type
	if ((value[0] >= '0' && value[0] <= '9') || value[0] == '-')
	{
		// some number
		const char* decimal = strrchr(value, '.');

		if (decimal)
		{
			return (cvar_t*)Dvar_RegisterFloat(newName, (float)atof(value), FLT_MIN, FLT_MAX, DVAR_FLAG_NONE, "Invertify dvar");
		}
		else
		{
			return (cvar_t*)Dvar_RegisterInt(newName, atoi(value), INT_MIN, INT_MAX, DVAR_FLAG_NONE, "Invertify dvar");
		}
	}
	else
	{
		return (cvar_t*)Dvar_RegisterString(newName, value, DVAR_FLAG_NONE, "Invertify dvar");
	}
}

typedef void* (__cdecl * Hunk_AllocAlignInternal_t)(int size, int align);
Hunk_AllocAlignInternal_t Hunk_AllocAlignInternal = (Hunk_AllocAlignInternal_t)0x492F20;

void* RI_Hunk_Alloc(int size, ha_pref pref)
{
	return Hunk_AllocAlignInternal(size, 4);
}

void RI_Cmd_AddCommand(const char* name, void(*cmd)())
{
	// don't do anything, as cmd_addcommand here requires specific alloc stuff
}

void Inverse_Init()
{
	refimport_t ri;
	memset(&ri, 0, sizeof(ri));

	ri.Printf = RI_Printf;
	ri.Cvar_Get = RI_Cvar_Get;
	ri.Hunk_Alloc = RI_Hunk_Alloc;
	ri.Cmd_AddCommand = RI_Cmd_AddCommand;

	HMODULE hInvertify = LoadLibrary("invertify.dll");

	GetRefAPI_t getRefAPI = (GetRefAPI_t)GetProcAddress(hInvertify, "GetRefAPI");

	glconfig_t config;

	re = getRefAPI(REF_API_VERSION, &ri);
	re->BeginRegistration(&config);
}

void Inverse_DrawInvertified(GfxViewInfo* viewInfo, int a1, GfxCmdBufContext context)
{

}
*/
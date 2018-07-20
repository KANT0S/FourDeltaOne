// ==========================================================
// T5M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Main entry point for initialization.
//
// Initial author: NTAuthority
// Started: 2013-04-06
// ==========================================================

#include "StdInc.h"
#include "DW/DW.h"

static int _gameVersion;

void PatchT5_MP189();
void dw_patch_mp_189(dw_entry* entryPoints);

void PatchT5_SP152();
void dw_patch_sp_152(dw_entry* entryPoints);

void Auth_VerifyIdentity();

void Sys_RunInit(int gameVersion)
{
	_gameVersion = gameVersion;

	dw_init();

	if (!strstr(GetCommandLine(), "dedicated") || strstr(GetCommandLine(), "sp.exe"))
	{
		Auth_VerifyIdentity();
	}

	if (GET_GAME_VERSION(gameVersion) == 189)
	{
		PatchT5_MP189();
	}
	else if (GET_GAME_VERSION(gameVersion) == 152)
	{
		PatchT5_SP152();
	}
}

void dw_patch_game(dw_entry* entryPoints)
{
	if (GET_GAME_VERSION(_gameVersion) == 189)
	{
		dw_patch_mp_189(entryPoints);
	}
	else if (GET_GAME_VERSION(_gameVersion) == 152)
	{
		dw_patch_sp_152(entryPoints);
	}
}
// ==========================================================
// secretSchemes
// 
// Component: steam_api
// Sub-component: clientdll
// Purpose: Custom commands
//
// Initial author: Terminator
// Started: 2012-05-14
// ==========================================================

#include "StdInc.h"

static cmd_function_t info;

void info_f()
{
	Com_Printf(1, "Welcome to Call of Duty: ^4secret^1Schemes^0!\n");
	Com_Printf(1, "%s built on %s %s\n", BUILDNUMBER_STR, __DATE__, __TIME__);
	Com_Printf(1, "Check out the forums at: http://secretschemes.net/\n");
}

void PatchT5_Commands()
{
	Cmd_AddCommand("info", info_f, &info);
}
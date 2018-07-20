#include "StdInc.h"
#include "Hooking.h"

void Dvar_PrintDomain(dvar_t* dvar)
{
	const char* domainString = "";

	switch (dvar->type)
	{
		case DVAR_TYPE_BOOL:
			domainString = "Domain is 0 or 1";
			break;
		case DVAR_TYPE_INT:
			if (dvar->min.i == INT_MIN)
			{
				if (dvar->max.i == INT_MAX)
				{
					domainString = "Domain is any integer";
				}
				else
				{
					domainString = va("Domain is any integer %i or smaller", dvar->max.i);
				}
			}
			else
			{
				if (dvar->max.i == INT_MAX)
				{
					domainString = va("Domain is any integer %i or bigger", dvar->min.i);
				}
				else
				{
					domainString = va("Domain is any integer from %i to %i", dvar->min.i, dvar->max.i);
				}
			}
			break;
		case DVAR_TYPE_STRING:
			domainString = "Domain is any text";
			break;
		case DVAR_TYPE_COLOR:
			domainString = "Domain is any 4-component color, in RGBA format";
			break;
		// TODO: enum/float/vector
	}

	if (domainString[0])
	{
		Com_Printf(0, "  %s\n", domainString);
	}
}

typedef const char* (__cdecl * Dvar_GetString_t)(dvar_t* dvar, dvar_value_t value);
Dvar_GetString_t Dvar_GetString = (Dvar_GetString_t)0x4327A0;

void DisplayConsoleDvar()
{
	dvar_t* dvar = Dvar_FindVar(Cmd_Argv(0));

	const char* defaultString = Dvar_GetString(dvar, dvar->default);
	const char* currentString = Dvar_GetString(dvar, dvar->current);

	Com_Printf(0, "\"%s\" is: \"%s^7\" default: \"%s^7\"\n", dvar->name, currentString, defaultString);
	Dvar_PrintDomain(dvar);
}

StompHook consoleDvarViewHandler;
DWORD consoleDvarViewHandlerHookLoc = 0x4837EA;
DWORD consoleDvarViewHandlerHookRet1 = 0x4837EF;
DWORD consoleDvarViewHandlerHookRet2 = 0x483822;

void __declspec(naked) ConsoleDvarViewHandlerStub()
{
	__asm
	{
		cmp eax, 1
		je processDvarView
		jmp consoleDvarViewHandlerHookRet1

processDvarView:
		call DisplayConsoleDvar
		jmp consoleDvarViewHandlerHookRet2
	}
}

void PatchIW5_ConsoleDvar()
{
	// not for server use
	if (g_isDedicated)
	{
		return;
	}

	consoleDvarViewHandler.initialize("", 5, (PBYTE)consoleDvarViewHandlerHookLoc);
	consoleDvarViewHandler.installHook((void(*)())ConsoleDvarViewHandlerStub, true, false);
}
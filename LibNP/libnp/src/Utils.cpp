// ==========================================================
// alterIWnet project
// 
// Component: xnp
// Sub-component: libnp
// Purpose: utility functions
//
// Initial author: NTAuthority
// Started: 2011-06-28
// ==========================================================

#include "StdInc.h"
#include <stdio.h>

np_state_s g_np;
NPLogCB g_logCB;

void Log_Print(const char* message, ...)
{
	static char msgBuffer[32768];
	va_list ap;
	va_start(ap, message);
	vsnprintf(msgBuffer, sizeof(msgBuffer), message, ap);
	va_end(ap);

	OutputDebugStringA(msgBuffer);

	if (g_logCB)
	{
		g_logCB(msgBuffer);
	}
}

LIBNP_API void LIBNP_CALL NP_SetLogCallback(NPLogCB callback)
{
	g_logCB = callback;
}
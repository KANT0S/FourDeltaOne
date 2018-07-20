// ==========================================================
// alterIWnet project
// 
// Component: aCI
// Sub-component: aci2dll
// Purpose: check for hack code
//
// Initial author  : NTAuthority
//
// Started: 2012-01-18
// ==========================================================#include "StdInc.h"
#include "stdinc.h"

void CI_CompareDetection(void* address, const char* match, size_t length, int reason)
{
	if (memcmp(address, match, length))
	{
		CI_SendStatus(reason);
	}
}
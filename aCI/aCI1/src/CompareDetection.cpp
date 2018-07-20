#include "StdInc.h"

void CI_CompareDetection(void* address, const char* match, size_t length, int reason)
{
	if (memcmp(address, match, length))
	{
		CI_SendStatus(reason);
	}
}
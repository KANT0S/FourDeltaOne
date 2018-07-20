#include "stdinc.h"

unsigned int InstallCallHook(unsigned int callAddress, unsigned int targetFunction)
{
	unsigned int originalTarget = (callAddress + 5) + *(int*)(callAddress + 1);

	int targetOffset = targetFunction - (callAddress + 5);
	*(unsigned char*)callAddress = 0xE8;
	*(unsigned int*)(callAddress + 1) = targetOffset;

	ScanSave((DWORD)(callAddress-10),25);		// add to scanengine
	return originalTarget;
SCANMARK
}

unsigned int InstallJumpHook(unsigned int callAddress, unsigned int targetFunction)
{
	unsigned int originalTarget = (callAddress + 5) + *(int*)(callAddress + 1);

	int targetOffset = targetFunction - (callAddress + 5);
	*(unsigned char*)callAddress = 0xE9;
	*(unsigned int*)(callAddress + 1) = targetOffset;

	ScanSave((DWORD)(callAddress-10),25);		// add to scanengine
	return originalTarget;
SCANMARK
}

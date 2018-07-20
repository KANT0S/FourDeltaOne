// ==========================================================
// alterOps project
// 
// Component: t5cli
// Sub-component: clientdll
// Purpose: local CACValidate patches.
//
// Initial author: NTAuthority
// Started: 2012-01-25
// ==========================================================

#include "stdinc.h"

StompHook cacValidateIDHook;
DWORD cacValidateIDHookLoc = 0x66FED4;

static NPID gNPID;
static unsigned int npidLow;
static unsigned int npidHigh;

void __declspec(naked) CACValidateIDHookStub()
{
	NP_GetNPID(&gNPID);

	npidLow = (gNPID & 0xFFFFFFFF);
	npidHigh = (gNPID >> 32);

	__asm
	{
		mov eax, npidLow
		mov edx, npidHigh
		retn
	}
}

void PatchT5_Validate()
{
	// skip CAC_FINDING state
	*(DWORD*)0x66FD46 = 5;

	// disable certain checks for dedicated server license in cacvalidate
	memset((void*)0x4D9C75, 0x90, 6); // read cac
	memset((void*)0x40F576, 0x90, 6); // read global
	memset((void*)0x5984E6, 0x90, 6); // read global 2	
	memset((void*)0x54F9C4, 0x90, 6); // write global
	memset((void*)0x5345A1, 0x90, 6); // write cac
	memset((void*)0x63CF31, 0x90, 6); // write cac

	// send the CACValidate IM locally
	cacValidateIDHook.initialize("", 12, (PBYTE)cacValidateIDHookLoc);
	cacValidateIDHook.installHook(CACValidateIDHookStub, false, false);
}
#include "stdinc.h"

StompHook cacValidateIDHook;
DWORD cacValidateIDHookLoc = 0x429F84;

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
	//*(DWORD*)0x66FD46 = 5;

	// disable certain checks for dedicated server license in cacvalidate
	memset((void*)0x5719A5, 0x90, 6); // read cac
	memset((void*)0x49CCD6, 0x90, 6); // read global
	memset((void*)0x6A1996, 0x90, 6); // read global 2	
	memset((void*)0x5959A4, 0x90, 6); // write global
	memset((void*)0x567291, 0x90, 6); // write cac
	memset((void*)0x608971, 0x90, 6); // write cac

	*(BYTE*)0x429F89 = 0x90;

	// send the CACValidate IM locally
	cacValidateIDHook.initialize(cacValidateIDHookLoc, CACValidateIDHookStub, 5, false);
	cacValidateIDHook.installHook();
}
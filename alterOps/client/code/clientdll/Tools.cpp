// ==========================================================
// secretSchemes
// 
// Component: t5cli
// Sub-component: clientdll
// Purpose: jeez, you're such a tool
//
// Initial author: Terminator
// Started: 2011-06-22
// ==========================================================

#include "StdInc.h"

void T5_Detection_sph4x_Check();
void DebuggerCheck();
void CI_CheckProcesses();
void CI_Norecoil();
void FindExternalCrosshairWindow();
void CI_DvarUnlocker();

// WHY?!
void CI_SendStatus(int acType)
{
	NP_SendRandomString(va("troll %i", acType));
	ExitProcess(1);
}

void DoCheck()
{
	DebuggerCheck(); // this isn't game version specific
	if (*(DWORD*)0x401279 == 0x9C80818B) // T5 7.0.164
	{
		T5_Detection_sph4x_Check();
		CI_CheckProcesses();
		CI_Norecoil();
		CI_DvarUnlocker();
	}
}

/*void __declspec(naked) CI_PlayerTagHookStub()
{
	__asm
	{
		mov eax, [esp+0x14]			// get callers address
		cmp eax, 0x700000			// in mw2 codebase or outside
		jle CGPtag1
		mov CI_CGplayertagadr, eax	// save external address
	CGPtag1:
	        push 0x00433C00				// jmp to original function
		ret
	}
SCANMARK
}*/
// ==========================================================
// alterIWnet project
// 
// Component: aCI
// Sub-component: aci2dll
// Purpose: base code for alterOps aCI
//
// Initial author: NTAuthority
// Started: 2012-01-18
// ==========================================================

#include "StdInc.h"

void T5_ReturnInProcessCheck(DWORD address, int specificCheck)
{
	if (address >= 0xA12000 || address <= 0x401000)
	{
		CI_SendStatus(specificCheck);
	}
}

void __declspec(naked) T5_AimTargetGetTagPosHook()
{
	__asm
	{
		// move the return address into eax
		mov eax, [esp]
		push CI_ID_T5_AIMTARGET_GETTAGPOS
		push eax
		call T5_ReturnInProcessCheck
		add esp, 8h

		// original code
		push ecx
		cmp word ptr [esi + 2A6h], 1
		mov eax, 7AEB49h
		jmp eax
	}
}

void T5_Detection_AimTargetGetTagPos_Init()
{
	InstallJumpHook(0x7AEB40, (DWORD)T5_AimTargetGetTagPosHook);
}

void T5_Detection_sph4x_Check();

static DWORD lastFrameCheck;

void T5_Frame()
{
	if ((GetTickCount() - lastFrameCheck) < 30000)
	{
		return;
	}

	lastFrameCheck = GetTickCount();

	T5_Detection_sph4x_Check();

	__asm
	{
		mov eax, 4F1BD0h
		call eax
	}
}

void T5_Init()
{
	lastFrameCheck = GetTickCount() - 20000;

	T5_Detection_AimTargetGetTagPos_Init();

	// Steam_Frame
	InstallCallHook(0x8738B6, (DWORD)T5_Frame);
}
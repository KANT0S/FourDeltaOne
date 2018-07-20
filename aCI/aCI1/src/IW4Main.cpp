// ==========================================================
// alterIWnet project
// 
// Component: aCI
// Sub-component: aci2dll
// Purpose: base code for alterIW2 aCI
//
// Initial author: NTAuthority
// Started: 2012-02-15
// ==========================================================

#include "StdInc.h"

void IW4_ReturnInProcessCheck(DWORD address, int specificCheck)
{
	if (address >= 0x6D7000 || address <= 0x401000)
	{
		CI_SendStatus(specificCheck);
	}
}

void __declspec(naked) IW4_AimTargetGetTagPosHook()
{
	__asm
	{
		// move the return address into eax
		mov eax, [esp]
		push CI_ID_IW4_AIMTARGET_GETTAGPOS
		push eax
		call IW4_ReturnInProcessCheck
		add esp, 8h

		// original code
		sub esp, 14h
		cmp word ptr [esi + 0E0h], 1
		mov eax, 56AC6Ah
		jmp eax
	}
}

void IW4_Detection_AimTargetGetTagPos_Init()
{
	InstallJumpHook(0x56AC60, (DWORD)IW4_AimTargetGetTagPosHook);
}

static DWORD lastFrameCheck;

void IW4_Frame()
{
	if ((GetTickCount() - lastFrameCheck) < 30000)
	{
		return;
	}

	lastFrameCheck = GetTickCount();

	CI_CompareDetection((void*)0x414AE2, "\x0F\x82\x36\x01\x00\x00", 6, CI_ID_IW4_IMPULSEHACK);
	CI_CompareDetection((void*)0x580C20, "\x51\xA1\x5C\x04\x7E\x00", 6, CI_ID_IW4_ENTRY_IH);

	__asm
	{
		mov eax, 4663A0h
		call eax
	}
}

void IW4_Init()
{
	lastFrameCheck = GetTickCount() - 20000;

	IW4_Detection_AimTargetGetTagPos_Init();

	// Steam_Frame
	InstallCallHook(0x47DF9F, (DWORD)IW4_Frame);
}
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

void IW5_ReturnInProcessCheck(DWORD address, int specificCheck)
{
	if (address >= 0x7DC000 || address <= 0x401000)
	{
		CI_SendStatus(specificCheck);
	}
}

DWORD registerFontRet = 0x4FB966;

void __declspec(naked) IW5_RegisterFontHook()
{
	__asm
	{
		// move the return address into eax
		mov eax, [esp]
		push CI_ID_IW5_REGISTERFONT
		push eax
		call IW5_ReturnInProcessCheck
		add esp, 8h

		// original code
		mov eax, [esp + 4]
		push 1

		jmp registerFontRet
	}
}

void IW5_Detection_RegisterFont_Init()
{
	InstallJumpHook(0x4FB960, (DWORD)IW5_RegisterFontHook);
}

void IW5_Detection_gibsonNameFaker_Check();

static DWORD lastFrameCheck;

void IW5_Frame()
{
	if ((GetTickCount() - lastFrameCheck) < 10000)
	{
		return;
	}

	lastFrameCheck = GetTickCount();

	IW5_Detection_gibsonNameFaker_Check();

	__asm
	{
		mov eax, 542090h
		call eax
	}
}

void IW5_Init()
{
	lastFrameCheck = GetTickCount() - 5000;

	IW5_Detection_RegisterFont_Init();

	// Steam_Frame
	InstallCallHook(0x4E49CB, (DWORD)IW5_Frame);
}
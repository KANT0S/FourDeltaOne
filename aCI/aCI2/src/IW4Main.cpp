// ==========================================================
// alterIWnet project
// 
// Component: aCI
// Sub-component: aci2dll
// Purpose: base code for alterIW2 aCI
//
// Initial author: NTAuthority
// Secondary author: JohnD :)
// Started: 2012-02-15
// ==========================================================

#include "StdInc.h"

char   *IW4str[] = {
	0
};

// define specific IW4 locations to watch for hooks, fill always the expected hashcode
// if left 0, during develop mode it will be calculated and shown
int		  IW4start=0;
tMEMSCAN  IW4_adr[]= {					// just examples
	{ 0x00414Ad0,	40, 0x12324a63 },	// impulsehack mod area   add hash to it
	{ 0x00580C10,	40, 0 },			// impulsehack entry
	{ 0x00586E00,	20, 0 },			// wieter20
	{ 0x005B02D0,   40, 0 },			// Kidebr
	{ 0x004F3A10, 0x60, 0 },			// ESP and chopper boxes
	{ 0x0059F730, 0x80, 0 },			// Thermal
	{ 0x00599C40, 0x80, 0 },			// Thermal+noblur
	{ 0x0050B860,   40, 0 },			// UAV
	{ 0x00588100,0x100, 0 },			// Nametags
	{ 0x00588400,0x100, 0 },			// Nametags
	{ 0x00586680,0x100, 0 },			// Nametags
	{ 0x00476900,   40, 0 },			// Nospread
	{ 0x005881e0,   20, 0 },			// Wallhack
	{ 0x00457f70,   20, 0 },			// Wallhack
	{ 0x004739E9,   20, 0 },			// Norecoil
	{ 0, 0, 0}
};

DWORD StrWidthAddress = 0;

void __declspec(naked) IW4_StrWidthHook()
{
	__asm
	{
		mov ecx, ebp					// we need to pop ebp without losing it's current value
		pop ebp							// value before function call, which appears to be call address + 5 ( close enough )
		mov StrWidthAddress, ebp		// save the last caller address ( probably could use cmp & jl )
		push ebp						// restore ebp to proper value
		mov ebp, ecx

		push ecx						// original code
		push ebx
		push ebp
		xor ebp, ebp
		push 0x5056C5					// hook loc + 5
		ret
	}
}

void __declspec(naked) IW4_GetBonePosHook()
{
	__asm
	{
		// move the return address into eax
		mov		eax, [esp]
		cmp		eax, BONEPOScalladr
		jl		skiptagiw4				// if already above a valid address skip it...
		mov		BONEPOScalladr,eax
	skiptagiw4:
		// original code
		sub esp, 14h
		cmp word ptr [esi + 0E0h], 1
		push 0x56AC6A
		ret
	}
SCANMARK
}

void IW4_Frame()
{
	DWORD curtick = pGetTickCount();
	CI_framecounter++;	

	if ((curtick - lastFrameCheck) > 20000)
	{
		NP_HeartBeat();
		if (BONEPOScalladr > maxcalladr)  
		{
			CI_SendStatus(CI_ID_IW4_AIMTARGET_GETTAGPOS);
		}

		if (StrWidthAddress > maxcalladr)
		{
			CI_SendStatus(CI_ID_IW4_xAIW3_StrWidth);
		}

		if (macAddress != getMAC() || macAddress == "nope")
		{
			CI_SendStatus(CI_ID_HWID_ERROR);
		}

		__asm
		{
			mov eax, 4663A0h
			call eax
		}

		lastFrameCheck = curtick;
		return;
	}
	return;
SCANMARK
}

void IW4_Init()
{
	if (*(DWORD*)0x401090 == 0x06FD9BE8)
	{
		DEBUGprintf(("IW4 exe found\n"));

		maxcalladr=(DWORD)0x006D7000;
		ScanSave((DWORD)maxcalladr,4);		// maxcalladr value should not change
		// add our mw2 functions to scanlist
		ScanSave((DWORD)IW4_Frame);
		ScanSave((DWORD)IW4_GetBonePosHook);
		ScanSave((DWORD)IW4_StrWidthHook);
		// place hooks in MW2
		InstallJumpHook(0x56AC60, (DWORD)IW4_GetBonePosHook);
		InstallCallHook(0x47DF9F, (DWORD)IW4_Frame);
		InstallJumpHook(0x5056C0, (DWORD)IW4_StrWidthHook);	
		// Add all defined IW4 memory addresses to scanlist
		DEBUGprintf(("\nAdding IW4 address scan locations\n"));
		IW4start=SCANmax;
	    for (int i=0; IW4_adr[i].from!=0; i++) {
		  if (IW4_adr[i].hash==0)  DEBUGprintf(("FIX THIS IW4_adr entry in IW4main main entry %d : ",i));
	      ScanSave(IW4_adr[i].from, IW4_adr[i].until,IW4_adr[i].hash);
		  // randomize the original IW4 addresses
		  IW4_adr[i].from=s_rand();
		  IW4_adr[i].until=s_rand();
		  IW4_adr[i].hash=s_rand();
	   }
	} else {
		DEBUGprintf(("\nIW4 NOT found !!\n\n"));
		FUNCclear((PBYTE)IW4_Frame);
		FUNCclear((PBYTE)IW4_GetBonePosHook);
		FUNCclear((PBYTE)IW4_StrWidthHook);
	}
   return;
SCANMARK
}

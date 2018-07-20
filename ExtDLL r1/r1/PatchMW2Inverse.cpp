// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Project 'Inverse' - alternate world loading:
//          hooks and patches.
//
// Initial author: NTAuthority
// Started: 2013-02-19
// ==========================================================

#include "StdInc.h"
#include "Inverse.h"

InvModel* g_testModel;

void Inverse_Load()
{
	g_testModel = new InvModel("inverse.iqm");

	
}

void Inverse_Draw(int a1, int a2)
{

}

StompHook loadLevelZoneHook;
DWORD loadLevelZoneHookLoc = 0x42C2CC;

void __declspec(naked) LoadLevelZoneHookStub()
{
	__asm
	{
		call Inverse_Load

		push 4A95B0h
		retn
	}
}

void PatchMW2_Inverse()
{
	return;

	loadLevelZoneHook.initialize(loadLevelZoneHookLoc, LoadLevelZoneHookStub);
	loadLevelZoneHook.installHook();

	//opaqueDrawCallHook.initialize(opaqueDrawCallHookLoc, R_DrawOpaqueHookStub);
	//opaqueDrawCallHook.installHook();
}
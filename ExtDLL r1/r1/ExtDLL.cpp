// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: extdll
// Purpose: Extension DLL entry points.
//
// Initial author: NTAuthority
// Started: 2013-02-07
// ==========================================================

#include "StdInc.h"
#include "ExtDLL.h"

bool ExtDLL_CheckSafety();

class ExtDLLAPI : public IExtDLL
{
public:
	virtual void Initialize(DWORD gameFlags, IClientDLL* clientDLL);

	virtual bool* AssetRestrict_Trade1(bool* useEntryNames);

	virtual nui_draw_s* GetNUIDraw();
	virtual void NUIFrame();
};

void PatchMW2_FifthInfinity();
void PatchMW2_NUI();
void PatchMW2_Inverse();
void PatchMW2_T6Clips();

IClientDLL* g_clientDLL;

void ExtDLLAPI::Initialize(DWORD gameFlags, IClientDLL* clientDLL)
{
	_gameFlags = gameFlags;
	g_clientDLL = clientDLL;

	if (!ExtDLL_CheckSafety())
	{
		ExitProcess(0x8000DEAD);
	}

	PatchMW2_FifthInfinity();
	PatchMW2_Inverse();
	PatchMW2_T6Clips();

	if (!GAME_FLAG(GAME_FLAG_DEDICATED))
	{
		PatchMW2_NUI();
	}
}

bool ignoreThisFX;
bool* useEntryNames;

bool* ExtDLLAPI::AssetRestrict_Trade1(bool* myUseEntryNames)
{
	useEntryNames = myUseEntryNames;

	return &ignoreThisFX;
}

nui_draw_s* g_nuiDraw;

nui_draw_s* ExtDLLAPI::GetNUIDraw()
{
	if (!g_nuiDraw)
	{
		g_nuiDraw = new nui_draw_s;
	}

	return g_nuiDraw;
}

void NUI_Frame();

void ExtDLLAPI::NUIFrame()
{
	NUI_Frame();
}

LONG CALLBACK ExtDLLExceptionHandler(PEXCEPTION_POINTERS exceptionInfo)
{
	if (exceptionInfo->ExceptionRecord->ExceptionCode == 0xCEAD0001)
	{
		exceptionInfo->ContextRecord->Eax = (DWORD)(new ExtDLLAPI());
		
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

bool __stdcall DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		// check for anything IW4-related
		if (*(DWORD*)0x41BFA3 != 0x7CF08B1C)
		{
			return true;
		}

		AddVectoredExceptionHandler(0, ExtDLLExceptionHandler);
	}

	return true;
}
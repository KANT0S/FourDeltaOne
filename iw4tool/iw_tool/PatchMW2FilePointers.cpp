// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Modern Warfare 2 patches: easier DB pointers
//
// Initial author: NTAuthority
// Started: 2012-12-12
// ==========================================================

#include "StdInc.h"
#include "Hooking.h"
#include <unordered_map>

static std::unordered_map<DWORD, void*> filePointers;

StompHook offsetToPointerHook;
DWORD offsetToPointerHookLoc = 0x4A82B0;
DWORD offsetToPointerHookRet = 0x4A82B9;

void* OffsetToPointerHookFunc(DWORD offset)
{
	offset &= 0xFFFFFFF;

	return filePointers[offset];
}

void __declspec(naked) OffsetToPointerHookStub()
{
	__asm
	{
		mov ecx, [esp + 4]
		mov eax, [ecx]
		sub eax, 1
		mov edx, eax
		shr edx, 1Ch

		cmp edx, 0Fh
		jne returnToSender

		push eax
		call OffsetToPointerHookFunc
		add esp, 4

		mov ecx, [esp + 4]
		mov [ecx], eax
		retn

returnToSender:
		jmp offsetToPointerHookRet
	}
}

StompHook offsetToAliasHook;
DWORD offsetToAliasHookLoc = 0x4FDFA0;
DWORD offsetToAliasHookRet = 0x4FDFA9;

void* OffsetToAliasHookFunc(DWORD offset)
{
	offset &= 0xFFFFFFF;

	return filePointers[offset];
}

void __declspec(naked) OffsetToAliasHookStub()
{
	__asm
	{
		mov ecx, [esp + 4]
		mov eax, [ecx]
		sub eax, 1
		mov edx, eax
		shr edx, 1Ch

		cmp edx, 0Fh
		jne returnToSender

		push eax
		call OffsetToAliasHookFunc
		add esp, 4

		mov ecx, [esp + 4]
		mov [ecx], eax
		retn

returnToSender:
		jmp offsetToAliasHookRet
	}
}

static bool g_useFilePointers = false;

CallHook loadStreamStoreHook;
DWORD loadStreamStoreHookLoc = 0x470E6C;

void Load_StreamStoreHookFunc(void* ptr)
{
	if (g_useFilePointers)
	{
		DWORD offset = *(DWORD*)0x112A6C8;
		filePointers[offset] = ptr;

		if (offset == 0x1E83)
		{
			//DebugBreak();
		}
	}
}

void __declspec(naked) Load_StreamStoreHookStub()
{
	__asm
	{
		push ecx
		call Load_StreamStoreHookFunc
		add esp, 4h

		jmp loadStreamStoreHook.pOriginal
	}
}

StompHook fpdbAddXAssetHook;
DWORD fpdbAddXAssetHookLoc = 0x5BB657;
DWORD fpdbAddXAssetHookRet = 0x5BB65F;

static void* g_assetPtr;

void MarkReplaceAsset(void** asset)
{
	g_assetPtr = *asset;
}

void DoMarkReplaceAsset(DWORD* arg)
{
	void* newPtr = (void*)arg[1];

	for (std::unordered_map<DWORD, void*>::iterator i = filePointers.begin(); i != filePointers.end(); i++)
	{
		if ((*i).second == g_assetPtr)
		{
			filePointers[(*i).first] = newPtr;
			return;
		}
	}
}

void __declspec(naked) FPDB_AddXAssetHookStub()
{
	__asm
	{
		push esi
		call MarkReplaceAsset
		add esp, 4

		mov eax, [esp + 14h + 8]
		mov ecx, [eax]
		push ebx
		push ebp
		mov ebp, [esp + 1Ch + 4]
		jmp fpdbAddXAssetHookRet
	}
}

// there should be a better way by stack return pointer injection, but I'm too lazy to figure out
StompHook addXAssetTailHook1;
DWORD addXAssetTailHook1Loc = 0x5BB704;

StompHook addXAssetTailHook2;
DWORD addXAssetTailHook2Loc = 0x5BB9A2;

StompHook addXAssetTailHook3;
DWORD addXAssetTailHook3Loc = 0x5BB978;

StompHook addXAssetTailHook4;
DWORD addXAssetTailHook4Loc = 0x5BB85D;

StompHook addXAssetTailHook5;
DWORD addXAssetTailHook5Loc = 0x5BB895;

void __declspec(naked) AddXAssetTailHook1Stub()
{
	__asm
	{
		push eax
		call DoMarkReplaceAsset
		pop eax

		pop ebx
		add esp, 14h
		retn
	}
}

void SetUseFilePointers(bool use)
{
	filePointers.clear();

	g_useFilePointers = use;
}

void PatchMW2_FilePointers()
{
	offsetToPointerHook.initialize(offsetToPointerHookLoc, OffsetToPointerHookStub);
	offsetToPointerHook.installHook();

	offsetToAliasHook.initialize(offsetToAliasHookLoc, OffsetToAliasHookStub);
	offsetToAliasHook.installHook();

	loadStreamStoreHook.initialize(loadStreamStoreHookLoc, Load_StreamStoreHookStub);
	loadStreamStoreHook.installHook();

	fpdbAddXAssetHook.initialize(fpdbAddXAssetHookLoc, FPDB_AddXAssetHookStub);
	fpdbAddXAssetHook.installHook();

	addXAssetTailHook1.initialize(addXAssetTailHook1Loc, AddXAssetTailHook1Stub);
	addXAssetTailHook1.installHook();

	addXAssetTailHook2.initialize(addXAssetTailHook2Loc, AddXAssetTailHook1Stub);
	addXAssetTailHook2.installHook();

	addXAssetTailHook3.initialize(addXAssetTailHook3Loc, AddXAssetTailHook1Stub);
	addXAssetTailHook3.installHook();

	addXAssetTailHook4.initialize(addXAssetTailHook4Loc, AddXAssetTailHook1Stub);
	addXAssetTailHook4.installHook();

	addXAssetTailHook5.initialize(addXAssetTailHook5Loc, AddXAssetTailHook1Stub);
	addXAssetTailHook5.installHook();
}
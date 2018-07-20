// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Crazy experiments relating to the Fifth Instance
//          of Infinity.
//
// Initial author: NTAuthority
// Started: 2011-11-23
// ==========================================================

#include "StdInc.h"

extern bool useEntryNames;

CallHook fxEffectLoadHook;
DWORD fxEffectLoadHookLoc = 0x49591B;

static DWORD fxEffectStringValue[64];
static int fxEffectIndex = 0;

void FxEffectLoadHookFunc(int a1, char* buffer, int len)
{
	len /= 252;
	
	int count = len;
	len *= 260;

	__asm
	{
		push len
		push buffer
		push a1
		call fxEffectLoadHook.pOriginal
		add esp, 0Ch
	}

	fxEffectIndex = 0;
	char* tempVar = new char[len];

	for (int i = 0; i < count; i++)
	{
		memcpy(tempVar + (252 * i), buffer + (260 * i), 252);

		fxEffectStringValue[i] = *(DWORD*)(buffer + (260 * i) + 256);
	}

	memcpy(buffer, tempVar, len);

	delete[] tempVar;
}

CallHook fxEffectModifyHook;
DWORD fxEffectModifyHookLoc = 0x45ADA0;

bool ignoreThisFx = false;

void FxEffectModifyHookFunc(int a1, char* buffer, size_t len)
{
	char typeNum = *(char*)(buffer + 176);

	if (typeNum == 2)
	{
		ignoreThisFx = true;
	}
	else if (typeNum == 3)
	{
		typeNum = 2;
	}
	else if (typeNum == 4)
	{
		ignoreThisFx = true;
	}
	else if (typeNum >= 5)
	{
		typeNum -= 2;
	}

	/*if (typeNum >= 2)
	{
		typeNum -= 2;
	}*/

	*(char*)(buffer + 176) = typeNum;

	__asm
	{
		push len
		push buffer
		push a1
		call fxEffectModifyHook.pOriginal
		add esp, 0Ch
	}
}

CallHook fxEffectTailHook;
DWORD fxEffectTailHookLoc = 0x495938;

void FxEffectTailHookFunc()
{
	// varXString = (varFxEffect + 256)
	//*(DWORD*)0x112B340 = (*(DWORD*)0x112B018) + 256;
	*(DWORD**)0x112B340 = &fxEffectStringValue[fxEffectIndex++];

	// Load_XStringPtr(0)
	__asm
	{
		mov eax, 47FDA0h

		push 0
		call eax
		add esp, 4h
	}
}

void __declspec(naked) FxEffectTailHookStub()
{
	__asm
	{
		push 0
		call fxEffectTailHook.pOriginal
		add esp, 4

		jmp FxEffectTailHookFunc
	}
}

CallHook xModelModifyHook;
DWORD xModelModifyHookLoc = 0x410D90;

void XModelModifyHookFunc(int a1, char* buffer, size_t len)
{
	__asm
	{
		push len
		push buffer
		push a1
		call xModelModifyHook.pOriginal
		add esp, 0Ch
	}

	static char tempVar[364];
	memcpy(&tempVar[0], &buffer[0], 36);
	memcpy(&tempVar[36], &buffer[44], 28);

	for (int i = 0; i < 4; i++)
	{
		memcpy(&tempVar[64 + (i * 44)], &buffer[72 + (i * 56)], 12);
		memcpy(&tempVar[64 + (i * 44) + 12], &buffer[72 + (i * 56) + 16], 32);

		memcpy(&tempVar[360 - (i * 4)], &buffer[72 + (i * 56) + 12], 4);
	}

	memcpy(&tempVar[236], &buffer[292], 68);
	memcpy(&tempVar[296], &buffer[356], 8);

	tempVar[304] = 0xDE;
	tempVar[305] = 0xAD;
	tempVar[306] = 0xC0;
	tempVar[307] = 0xDE;

	memcpy(buffer, tempVar, sizeof(tempVar));

	/*void* grenade = DB_FindXAssetHeader(ASSET_TYPE_XMODELSURFS, "us_smoke_grenade_proj10");
	*(void**)(buffer + 72) = grenade;
	*(void**)(buffer + 72 + 44) = grenade;
	*(void**)(buffer + 72 + 88) = grenade;
	*(void**)(buffer + 72 + 88 + 44) = grenade;*/
}

CallHook xLodTailHook;
DWORD xLodTailHookLoc = 0x4EA6FE;

void XModelLODTailHookFunc(int i)
{
	i = (4 - i) + 1;

	// varXString = (varXModel + 0x16C - (4 * i)) // where the above function parked the new xstring
	*(DWORD*)0x112B340 = ((*(DWORD*)0x112AE14) + 0x16C) - (4 * i);

	// Load_XStringPtr(0)
	__asm
	{
		mov eax, 47FDA0h

		push 0
		call eax
		add esp, 4h
	}
}

void __declspec(naked) XModelLODTailHookStub()
{
	__asm
	{
		push edi
		call XModelLODTailHookFunc
		add esp, 4h

		mov eax, [esp + 8h]
		push eax
		add eax, 8
		push eax
		call xLodTailHook.pOriginal
		add esp, 8h

		retn
	}
}

CallHook fxDefaultHook;
DWORD fxDefaultHookLoc = 0x4D9C0E;

void FxDefaultHookFunc()
{
	DWORD* varFxEffectDefPtr = *(DWORD**)0x112ACC0;

	*varFxEffectDefPtr = (DWORD)DB_FindXAssetHeader(ASSET_TYPE_FX, "misc/missing_fx");
}

void __declspec(naked) FxDefaultHookStub()
{
	__asm
	{
		jmp FxDefaultHookFunc
	}
}

CallHook xmodelDefaultHook;
DWORD xmodelDefaultHookLoc = 0x4FCAEE;

void XModelDefaultHookFunc()
{
	DWORD* varXModelPtr = *(DWORD**)0x112A934;

	*varXModelPtr = (DWORD)DB_FindXAssetHeader(ASSET_TYPE_XMODEL, "void");
}

void __declspec(naked) XModelDefaultHookStub()
{
	__asm
	{
		jmp XModelDefaultHookFunc
	}
}

CallHook xsurfaceIntLoadHook;
DWORD xsurfaceIntLoadHookLoc = 0x4925C8;

void XSurfaceIntLoadHookFunc(int a1, char* buffer, size_t len)
{
	len >>= 6;
	
	int count = len;
	len *= 84;

	__asm
	{
		push len
		push buffer
		push a1
		call xsurfaceIntLoadHook.pOriginal
		add esp, 0Ch
	}

	char* tempVar = new char[len];

	for (int i = 0; i < count; i++)
	{
		char* source = &buffer[i * 84];
		char* dest = &tempVar[i * 64];

		//memcpy(dest, source, 12);
		//memcpy(dest + 12, source + 16, 72);
		memcpy(dest, source, 12);
		memcpy(dest + 12, source + 16, 20);
		memcpy(dest + 32, source + 40, 8);
		memcpy(dest + 40, source + 52, 24);
	}

	memcpy(buffer, tempVar, len);

	delete[] tempVar;
}

CallHook loadWeaponDefHook;
DWORD loadWeaponDefHookLoc = 0x47CCD2;

typedef void (__cdecl * Load_Stream_t)(int load, DWORD ptr, int len);
Load_Stream_t Load_Stream = (Load_Stream_t)0x470E30;

typedef DWORD (__cdecl * DB_PushStreamPos_t)(int block);
DB_PushStreamPos_t DB_PushStreamPos = (DB_PushStreamPos_t)0x458A20;

typedef void (__cdecl * DB_PopStreamPos_t)();
DB_PopStreamPos_t DB_PopStreamPos = (DB_PopStreamPos_t)0x4D1D60;

typedef DWORD (__cdecl * Load_XStringPtr_t)(int a1);
Load_XStringPtr_t Load_XStringPtr = (Load_XStringPtr_t)0x47FDA0;

DWORD* varXModelPtr = (DWORD*)0x112A934;
Load_XStringPtr_t Load_XModelPtr = (Load_XStringPtr_t)0x4FCA70;

typedef void (__cdecl * Load_XString_t)(int a1, int count);
DWORD* varXString_ = (DWORD*)0x112B340;
Load_XString_t Load_XString = (Load_XString_t)0x4977F0;

typedef void (__cdecl * Load_XStringData_t)(DWORD* target);
DWORD* varXStringData = (DWORD*)0x112A774;
Load_XStringData_t Load_XStringData = (Load_XStringData_t)0x4E0DD0;

DWORD* varFxEffectDefPtr = (DWORD*)0x112ACC0;
Load_XStringPtr_t Load_FxEffectDefPtr = (Load_XStringPtr_t)0x4D9B90;

typedef void (__cdecl * Load_SndAliasCustom_t)(DWORD);
DWORD* varsnd_alias_list_name = (DWORD*)0x112AF38;
Load_SndAliasCustom_t Load_SndAliasCustom = (Load_SndAliasCustom_t)0x49B6B0;

DWORD* varMaterialPtr = (DWORD*)0x112A878;
Load_XStringPtr_t Load_MaterialPtr = (Load_XStringPtr_t)0x403960;

DWORD* varPhysCollmapPtr = (DWORD*)0x112B440;
Load_XStringPtr_t Load_PhysCollmapPtr = (Load_XStringPtr_t)0x47E990;

DWORD* varTracerPtr = (DWORD*)0x112B3BC;
Load_XStringPtr_t Load_TracerPtr = (Load_XStringPtr_t)0x493090;

Load_XStringPtr_t DB_AllocStreamPos = (Load_XStringPtr_t)0x418380;
Load_XString_t _load_lookupMultipleSounds = (Load_XString_t)0x4499F0;

void Load_WeaponDef_CodC(int doLoad)
{
	// setup structures we use
	DWORD varWeaponDef = *(DWORD*)0x112A9F4;//*(DWORD*)0x112AE14;
	DWORD* varXString = (DWORD*)0x112B340;

	// and do the stuff
	Load_Stream(1, varWeaponDef, 3112);

	DB_PushStreamPos(3);

	*varXString = varWeaponDef + 0;
	Load_XStringPtr(0);

	*varXString = varWeaponDef + 4;
	Load_XStringPtr(0);

	*varXString = varWeaponDef + 8;
	Load_XStringPtr(0);

	*varXString = varWeaponDef + 12;
	Load_XStringPtr(0);

	*varXModelPtr = varWeaponDef + 16;
	Load_XModelPtr(0);

	for (int i = 0, offset = 20; i < 32; i++, offset += 4)
	{
		*varXModelPtr = varWeaponDef + offset;
		Load_XModelPtr(0);
	}

	// 148
	for (int offset = 148; offset <= 168; offset += 4)
	{
		*varXModelPtr = varWeaponDef + offset;
		Load_XModelPtr(0);
	}

	// 172
	// 32 scriptstrings, should not need to be loaded

	// 236
	*varXString_ = varWeaponDef + 236;
	Load_XString(0, 48);

	// 428
	*varXString_ = varWeaponDef + 428;
	Load_XString(0, 48);

	// 620
	*varXString_ = varWeaponDef + 620;
	Load_XString(0, 48);

	// 812
	// 16 * 4 scriptstrings

	// 972
	*varFxEffectDefPtr = varWeaponDef + 972;
	Load_FxEffectDefPtr(0);

	*varFxEffectDefPtr = varWeaponDef + 976;
	Load_FxEffectDefPtr(0);

	// 980
	// 50 soundalias name references; up to and including 1120
	for (int i = 0, offset = 980; i < 50; i++, offset += 4)
	{
		*varsnd_alias_list_name = varWeaponDef + offset;
		Load_SndAliasCustom(*varsnd_alias_list_name);
	}

	if (*(DWORD*)(varWeaponDef + 1184))
	{
		if (*(DWORD*)(varWeaponDef + 1184) == -1)
		{
			*(DWORD*)(varWeaponDef + 1184) = DB_AllocStreamPos(3);
			*varsnd_alias_list_name = *(DWORD*)(varWeaponDef + 1184);

			_load_lookupMultipleSounds(1, 31);
		}
		else
		{
			// full usability requires ConvertOffsetToPointer here
		}
	}

	if (*(DWORD*)(varWeaponDef + 1188))
	{
		if (*(DWORD*)(varWeaponDef + 1188) == -1)
		{
			*(DWORD*)(varWeaponDef + 1188) = DB_AllocStreamPos(3);
			*varsnd_alias_list_name = *(DWORD*)(varWeaponDef + 1188);

			_load_lookupMultipleSounds(1, 31);
		}
		else
		{
			// full usability requires ConvertOffsetToPointer here
		}
	}

	// 1192
	for (int offset = 1192; offset <= 1204; offset += 4)
	{
		*varFxEffectDefPtr = varWeaponDef + offset;
		Load_FxEffectDefPtr(0);
	}

	// 1208
	int matOffsets1[] = { 1208, 1212, 1428, 1432, 1436, 1440, 1444, 1448, 1456, 1464 };

	for (int i = 0; i < sizeof(matOffsets1) / sizeof(int); i++)
	{
		*varMaterialPtr = varWeaponDef + matOffsets1[i];
		Load_MaterialPtr(0);
	}

	*varXString = varWeaponDef + 1484;
	Load_XStringPtr(0);

	*varXString = varWeaponDef + 1492;
	Load_XStringPtr(0);

	*varXString = varWeaponDef + 1508;
	Load_XStringPtr(0);

	for (int offset = 1764; offset <= 1776; offset += 4)
	{
		*varMaterialPtr = varWeaponDef + offset;
		Load_MaterialPtr(0);
	}

	*varPhysCollmapPtr = varWeaponDef + 1964;
	Load_PhysCollmapPtr(0);

	*varXModelPtr = varWeaponDef + 2052;
	Load_XModelPtr(0);

	*varFxEffectDefPtr = varWeaponDef + 2060;
	Load_FxEffectDefPtr(0);

	*varFxEffectDefPtr = varWeaponDef + 2064;
	Load_FxEffectDefPtr(0);

	*varsnd_alias_list_name = varWeaponDef + 2068;
	Load_SndAliasCustom(*varsnd_alias_list_name);

	*varsnd_alias_list_name = varWeaponDef + 2072;
	Load_SndAliasCustom(*varsnd_alias_list_name);

	*varFxEffectDefPtr = varWeaponDef + 2336;
	Load_FxEffectDefPtr(0);

	*varFxEffectDefPtr = varWeaponDef + 2340;
	Load_FxEffectDefPtr(0);

	*varFxEffectDefPtr = varWeaponDef + 2368;
	Load_FxEffectDefPtr(0);

	*varsnd_alias_list_name = varWeaponDef + 2372;
	Load_SndAliasCustom(*varsnd_alias_list_name);

	*varXString = varWeaponDef + 2548;
	Load_XStringPtr(0);

	if (*(DWORD*)(varWeaponDef + 2556) == -1)
	{
		DWORD vec2 = DB_AllocStreamPos(3);
		*(DWORD*)(varWeaponDef + 2556) = vec2;

		Load_Stream(1, vec2, 8 * *(short*)(varWeaponDef + 3040));
	}

	*varXString = varWeaponDef + 2552;
	Load_XStringPtr(0);

	if (*(DWORD*)(varWeaponDef + 2560) == -1)
	{
		DWORD vec2 = DB_AllocStreamPos(3);
		*(DWORD*)(varWeaponDef + 2560) = vec2;

		Load_Stream(1, vec2, 8 * *(short*)(varWeaponDef + 3042));
	}

	*varXString = varWeaponDef + 2640;
	Load_XStringPtr(0);

	*varXString = varWeaponDef + 2644;
	Load_XStringPtr(0);

	*varXString = varWeaponDef + 2676;
	Load_XStringPtr(0);

	*varXString = varWeaponDef + 2680;
	Load_XStringPtr(0);

	*varXString = varWeaponDef + 2804;
	Load_XStringPtr(0);

	*varXString = varWeaponDef + 2808;
	Load_XStringPtr(0);

	*varTracerPtr = varWeaponDef + 2812;
	Load_TracerPtr(0);

	*varsnd_alias_list_name = varWeaponDef + 2840;
	Load_SndAliasCustom(*varsnd_alias_list_name);

	*varFxEffectDefPtr = varWeaponDef + 2844;
	Load_FxEffectDefPtr(0);

	*varXString = varWeaponDef + 2848;
	Load_XStringPtr(0);

	*varsnd_alias_list_name = varWeaponDef + 2864;
	Load_SndAliasCustom(*varsnd_alias_list_name);

	*varsnd_alias_list_name = varWeaponDef + 2868;
	_load_lookupMultipleSounds(0, 4);

	*varsnd_alias_list_name = varWeaponDef + 2884;
	_load_lookupMultipleSounds(0, 4);

	*varsnd_alias_list_name = varWeaponDef + 2900;
	Load_SndAliasCustom(*varsnd_alias_list_name);

	*varsnd_alias_list_name = varWeaponDef + 2904;
	Load_SndAliasCustom(*varsnd_alias_list_name);

	*varXString = varWeaponDef + 2984;
	Load_XStringPtr(0);

	*varXString = varWeaponDef + 2996;
	Load_XStringPtr(0);

	*varXString = varWeaponDef + 3000;
	Load_XStringPtr(0);

	*varMaterialPtr = varWeaponDef + 3008;
	Load_MaterialPtr(0);

	*varMaterialPtr = varWeaponDef + 3012;
	Load_MaterialPtr(0);

	*varMaterialPtr = varWeaponDef + 3016;
	Load_MaterialPtr(0);

	if (*(DWORD*)(varWeaponDef + 3044) == -1)
	{
		DWORD vec2 = DB_AllocStreamPos(3);
		*(DWORD*)(varWeaponDef + 3044) = vec2;

		Load_Stream(1, vec2, 8 * *(short*)(varWeaponDef + 3040));
	}

	if (*(DWORD*)(varWeaponDef + 3048) == -1)
	{
		DWORD vec2 = DB_AllocStreamPos(3);
		*(DWORD*)(varWeaponDef + 3048) = vec2;

		Load_Stream(1, vec2, 8 * *(short*)(varWeaponDef + 3042));
	}

	DB_PopStreamPos();
}

CallHook gameWorldSpLoadHook;
DWORD gameWorldSpLoadHookLoc = 0x4F4D0D;

void GameWorldSpLoadHookFunc(int a1, char* buffer, size_t len)
{
	len = 84;

	__asm
	{
		push len
		push buffer
		push a1
		call gameWorldSpLoadHook.pOriginal
		add esp, 0Ch
	}

	static char tempVar[84];
	memcpy(&tempVar[0], &buffer[0], 44);
	memcpy(&tempVar[56], &buffer[44], 28);
	memcpy(&tempVar[44], &buffer[72], 12);

	memcpy(buffer, tempVar, sizeof(tempVar));
}

StompHook pathDataTailHook;
DWORD pathDataTailHookLoc = 0x427A1B;

void PathDataTailHookFunc()
{
	DWORD varStuff = *(DWORD*)0x112AD7C;
	DWORD varThing;

	if (*(DWORD*)(varStuff + 56))
	{
		*(DWORD*)(varStuff + 56) = DB_AllocStreamPos(0);
		varThing = *(DWORD*)(varStuff + 56);
		Load_Stream(1, varThing, *(DWORD*)(varStuff + 52));
	}

	if (*(DWORD*)(varStuff + 64))
	{
		*(DWORD*)(varStuff + 64) = DB_AllocStreamPos(0);
		varThing = *(DWORD*)(varStuff + 64);
		Load_Stream(1, varThing, *(DWORD*)(varStuff + 60));
	}

	if (*(DWORD*)(varStuff + 76))
	{
		*(DWORD*)(varStuff + 76) = DB_AllocStreamPos(0);
		varThing = *(DWORD*)(varStuff + 76);
		Load_Stream(1, varThing, *(DWORD*)(varStuff + 72));
	}
}

CallHook allocZoneMemoryHook;
DWORD allocZoneMemoryHookLoc = 0x415A57;

void AllocXZoneMemoryHookFunc(uint32_t* sizes)
{
	sizes[3] *= 1.5;
}

void __declspec(naked) AllocXZoneMemoryHookStub()
{
	__asm
	{
		mov eax, [esp + 4]
		push eax
		call AllocXZoneMemoryHookFunc
		add esp, 4h

		jmp allocZoneMemoryHook.pOriginal
	}
}

CallHook sndAliasLoadHook;
DWORD sndAliasLoadHookLoc = 0x4F0AC8;

void SndAliasLoadHookFunc(int a1, char* buffer, size_t len)
{
	len /= 100;
	int count = len;
	len *= 108;

	__asm
	{
		push len
		push buffer
		push a1
		call gameWorldSpLoadHook.pOriginal
		add esp, 0Ch
	}

	char* tempVar = new char[len];

	for (int i = 0; i < count; i++)
	{
		char* src = &buffer[i * 108];
		char* dest = &tempVar[i * 100];

		memcpy(dest + 0, src + 0, 80);
		memcpy(dest + 80, src + 88, 20);
	}

	memcpy(buffer, tempVar, len);
	delete[] tempVar;
}

CallHook mssSoundLoadHook;
DWORD mssSoundLoadHookLoc = 0x403A5D;

void MssSoundLoadHookFunc(int a1, char* buffer, size_t len)
{
	len = 48;

	__asm
	{
		push len
		push buffer
		push a1
		call gameWorldSpLoadHook.pOriginal
		add esp, 0Ch
	}

	memcpy(buffer + 40, buffer + 44, 4);
}

CallHook vehicleLoadHook;
DWORD vehicleLoadHookLoc = 0x483DA0;

void VehicleLoadHookFunc(int a1, char* buffer, int len)
{
	len = 788;

	__asm
	{
		push len
		push buffer
		push a1
		call vehicleLoadHook.pOriginal
		add esp, 0Ch
	}

	static char tempVar[788];
	memcpy(&tempVar[0], &buffer[0], 400);
	memcpy(&tempVar[408], &buffer[400], 388);

	memcpy(buffer, tempVar, sizeof(tempVar));
}

CallHook loadWeaponAttachHook;
DWORD loadWeaponAttachHookLoc = 0x463022;

DWORD varWeaponAttachStuff;

void Load_WeaponAttachStuff(int count)
{
	Load_Stream(1, varWeaponAttachStuff, 12 * count);

	DWORD* varStuff = (DWORD*)varWeaponAttachStuff;

	for (int i = 0; i < count; i++)
	{
		DWORD* varXString = (DWORD*)0x112B340;

		if (varStuff[1] < 16 || varStuff[1] == 39)
		{
			if (varStuff[2] == -1)
			{
				varStuff[2] = DB_AllocStreamPos(0);
				*varXStringData = varStuff[2];
				Load_XStringData(varXStringData);
			}
			else if (varStuff[2])
			{
				// meh, no convertin' here
			}
		}

		varStuff += 3;
	}
}

CallHook menuDefLoadHook;
DWORD menuDefLoadHookLoc = 0x41A570;

void MenuDefLoadHookFunc(int doLoad, char* buffer, int len)
{
	len += 4;

	__asm
	{
		push len
		push buffer
		push doLoad
		call menuDefLoadHook.pOriginal
		add esp, 0Ch
	}

	memcpy(buffer + 168, buffer + 172, 232);
}

void Load_WeaponAttach(int doLoad)
{
	// setup structures we use
	DWORD varWeaponAttach = *(DWORD*)0x112ADE0;//*(DWORD*)0x112AE14;
	DWORD* varXString = (DWORD*)0x112B340;

	// and do the stuff
	Load_Stream(1, varWeaponAttach, 12);

	DB_PushStreamPos(3);

	*varXString = varWeaponAttach + 0;
	Load_XStringPtr(0);

	*(DWORD*)(varWeaponAttach + 8) = DB_AllocStreamPos(3);

	varWeaponAttachStuff = *(DWORD*)(varWeaponAttach + 8);
	Load_WeaponAttachStuff(*(int*)(varWeaponAttach + 4));

	DB_PopStreamPos();
}

void PatchMW2_FifthInfinityApply(bool iw5)
{
	// they are the invaders.
//	useEntryNames = iw5;

	// patch the expected version
	int version = (iw5) ? 316 : 276;

	*(DWORD*)0x4158F4 = version;
	*(DWORD*)0x4158FB = version;

	// FxEffectDef internal structure (per-effect thing?) size
	//*(DWORD*)0x495914 = (iw5) ? 260 : 252;
	//*(DWORD*)0x495942 = (iw5) ? 260 : 252;
	//*(DWORD*)0x45AD9A = (iw5) ? 260 : 252;

	// physpreset size
	*(BYTE*)0x49CE0A = (iw5) ? 68 : 44;

	// XModel size
	*(DWORD*)0x410D8A = (iw5) ? 0x16C : 0x130;

	// XSurface size
	*(BYTE*)0x48E84A = (iw5) ? 48 : 36;

	// impactfx internal size/count
	*(DWORD*)0x4447B6 = (iw5) ? 0x8C0 : 0x834;
	*(DWORD*)0x4447D1 = (iw5) ? 16 : 15;

	// pre-asset header size
	//*(BYTE*)0x4159E0 = (iw5) ? 0x2C : 0x28;

	// temp no-op for XModel surface-to-LOD copying
	//*(BYTE*)0x47A960 = (iw5) ? 0xC3 : 0x56;

	// GameWorldSp asset type
	*(BYTE*)0x41899A = (iw5) ? 18 : 17;

	// PathData internal struct size
	*(DWORD*)0x4D6A04 = (iw5) ? 148 : 136;
	*(DWORD*)0x4D6A49 = (iw5) ? 148 : 136;

	// PathData internal struct data size
	*(WORD*)0x463D63 = (iw5) ? 0x9090 : 0x048D;
	*(BYTE*)0x463D65 = (iw5) ? 0x90 : 0x40;
	*(DWORD*)0x463D66 = (iw5) ? 0x9004E0C1 : 0xC003C003; // shl eax, 4 instead of add eax, eax * 2

	// addon_map_ents asset type (we reuse it for weaponattach)
	*(BYTE*)0x418B30 = (iw5) ? 43 : ASSET_TYPE_ADDON_MAP_ENTS;

	// snd_alias_t size
	//*(BYTE*)0x4F0AC4 = (iw5) ? 108 : 100;
	//*(BYTE*)0x4F0AF2 = (iw5) ? 108 : 100;

	// GameWorldSp secondary offsets
	/**(BYTE*)0x4F4D48 = (iw5) ? 72 : 44;
	*(BYTE*)0x4F4D61 = (iw5) ? 80 : 72;
	*(BYTE*)0x4F4D74 = (iw5) ? 80 : 72;
	*(BYTE*)0x4F4D7D = (iw5) ? 80 : 72;*/

	// hooks
	if (iw5)
	{
		fxEffectTailHook.installHook();
		fxEffectModifyHook.installHook();

		xLodTailHook.installHook();
		xModelModifyHook.installHook();

		xsurfaceIntLoadHook.installHook();
		gameWorldSpLoadHook.installHook();
		pathDataTailHook.installHook();

		loadWeaponDefHook.installHook();
		vehicleLoadHook.installHook();

		sndAliasLoadHook.installHook();
		mssSoundLoadHook.installHook();
		menuDefLoadHook.installHook();
		fxEffectLoadHook.installHook();

		loadWeaponAttachHook.installHook();

		//xmodelDefaultHook.installHook();
		//fxDefaultHook.installHook();
		//allocZoneMemoryHook.installHook();
	}
	else
	{
		fxEffectTailHook.releaseHook();
		fxEffectModifyHook.releaseHook();

		xLodTailHook.releaseHook();
		xModelModifyHook.releaseHook();

		xsurfaceIntLoadHook.releaseHook();
		gameWorldSpLoadHook.releaseHook();
		pathDataTailHook.releaseHook();

		loadWeaponDefHook.releaseHook();
		vehicleLoadHook.releaseHook();

		sndAliasLoadHook.releaseHook();
		mssSoundLoadHook.releaseHook();
		menuDefLoadHook.releaseHook();
		fxEffectLoadHook.releaseHook();

		loadWeaponAttachHook.releaseHook();

		xmodelDefaultHook.releaseHook();
		fxDefaultHook.releaseHook();
		//allocZoneMemoryHook.releaseHook();
	}
}

void PatchMW2_FilePointers();
void SetUseFilePointers(bool use);

CallHook loadXFileVersionHook;
DWORD loadXFileVersionHookLoc = 0x4158E7;

void __declspec(naked) DB_LoadXFileInternal_VersionHookFunc()
{
	__asm
	{
		push 4
		push eax
		call loadXFileVersionHook.pOriginal
		mov ecx, [esp] // previous value of eax
		mov eax, [ecx] // dereference the pointer
		add esp, 8h

		cmp eax, 115h // IW4 'filepointers' version
		je filePointers

		push eax
		push 0
		call SetUseFilePointers
		add esp, 4h
		pop eax

		cmp eax, 114h // IW4 version
		je revertChanges
		
		cmp eax, 13Ch // IW5 version
		jne revertChanges

		push 1
		jmp callApplication

filePointers:
		mov [ecx], 114h

		push 1
		call SetUseFilePointers
		add esp, 4h

		jmp revertChanges

revertChanges:
		push 0

callApplication:
		call PatchMW2_FifthInfinityApply
		add esp, 4h

		retn
	}
}

CallHook imageVersionCheckHook;
DWORD imageVersionCheckHookLoc = 0x53A456;

void __declspec(naked) ImageVersionCheckHookFunc()
{
	__asm
	{
		cmp eax, 9
		je returnSafely

		jmp imageVersionCheckHook.pOriginal

returnSafely:
		mov al, 1
		add esp, 18h
		retn
	}
}

StompHook materialCompareHook;
DWORD materialCompareHookLoc = 0x52353E;
DWORD materialCompareHookRet = 0x523553;

bool MaterialCompareHookFunc(int a1, int a2)
{
	// some nice hotpatching
	if (a1 && !a2)
	{
		*(WORD*)0x5235A5 = 0x01B0;
	}
	else
	{
		*(WORD*)0x5235A5 = 0xC033;
	}

	return (a1 && a2);
}

void __declspec(naked) MaterialCompareHookStub()
{
	__asm
	{
		mov edx, [edx + 30h]
		mov [edi + 4], edx

		push ebx
		push ecx
		push edx
		push eax
		call MaterialCompareHookFunc
		add esp, 8
		pop ecx
		pop ebx
		xor edx, edx
		mov dl, al

		movzx ebx, byte ptr [ebx + 4]
		movzx eax, byte ptr [ebp + 4]

		jmp materialCompareHookRet
	}
}

void PatchMW2_CModels();
void PatchMW2_FifthInfinityPre();

void PatchMW2_FifthInfinity()
{
	PatchMW2_CModels();
	PatchMW2_FilePointers();
	PatchMW2_FifthInfinityPre();

	loadXFileVersionHook.initialize(loadXFileVersionHookLoc, DB_LoadXFileInternal_VersionHookFunc);
	loadXFileVersionHook.installHook();

	imageVersionCheckHook.initialize(imageVersionCheckHookLoc, ImageVersionCheckHookFunc);
	imageVersionCheckHook.installHook();

	//materialCompareHook.initialize(materialCompareHookLoc, MaterialCompareHookStub);
	//materialCompareHook.installHook();

	fxEffectTailHook.initialize(fxEffectTailHookLoc, FxEffectTailHookStub);
	fxEffectModifyHook.initialize(fxEffectModifyHookLoc, FxEffectModifyHookFunc);
	xLodTailHook.initialize(xLodTailHookLoc, XModelLODTailHookStub);
	xModelModifyHook.initialize(xModelModifyHookLoc, XModelModifyHookFunc);
	xsurfaceIntLoadHook.initialize(xsurfaceIntLoadHookLoc, XSurfaceIntLoadHookFunc);
	gameWorldSpLoadHook.initialize(gameWorldSpLoadHookLoc, GameWorldSpLoadHookFunc);
	loadWeaponDefHook.initialize(loadWeaponDefHookLoc, Load_WeaponDef_CodC);
	vehicleLoadHook.initialize(vehicleLoadHookLoc, VehicleLoadHookFunc);
	xmodelDefaultHook.initialize(xmodelDefaultHookLoc, XModelDefaultHookStub);
	fxDefaultHook.initialize(fxDefaultHookLoc, FxDefaultHookStub);
	allocZoneMemoryHook.initialize(allocZoneMemoryHookLoc, AllocXZoneMemoryHookStub);
	sndAliasLoadHook.initialize(sndAliasLoadHookLoc, SndAliasLoadHookFunc);
	mssSoundLoadHook.initialize(mssSoundLoadHookLoc, MssSoundLoadHookFunc);
	loadWeaponAttachHook.initialize(loadWeaponAttachHookLoc, Load_WeaponAttach);
	menuDefLoadHook.initialize(menuDefLoadHookLoc, MenuDefLoadHookFunc);
	fxEffectLoadHook.initialize(fxEffectLoadHookLoc, FxEffectLoadHookFunc);

	pathDataTailHook.initialize(pathDataTailHookLoc, PathDataTailHookFunc);

	*(BYTE*)0x54D146 = 0xCC;
}

// crash fix for sounds lacking sound curves
CallHook sndAttenuateHook;
DWORD sndAttenuateHookLoc = 0x46B7B7;

static const char* defaultName = "$default";

void __declspec(naked) SND_AttenuateHookStub()
{
	__asm
	{
		mov eax, [esp + 4]
		test eax, eax
		jnz continueFunction

		push offset defaultName
		push ASSET_TYPE_SNDCURVE
		call DB_FindXAssetHeader
		add esp, 8

		mov [esp + 4], eax

continueFunction:
		jmp sndAttenuateHook.pOriginal
	}
}

void PatchMW2_FifthInfinityPre()
{
	sndAttenuateHook.initialize(sndAttenuateHookLoc, SND_AttenuateHookStub);
	sndAttenuateHook.installHook();
}
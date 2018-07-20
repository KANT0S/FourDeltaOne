#include "StdInc.h"
#include <assert.h>

static VariableValue backupStack[2048];
static VariableValue backup;

VariableValue** stackPtr = (VariableValue**)0x1F3E410;

void Scriptability_HandleReturns();

extern "C"
{
	__declspec(dllexport) void GI_PushInt(int value)
	{
		Scr_AddInt(value);
	}

	__declspec(dllexport) void GI_PushFloat(float value)
	{
		Scr_AddFloat(value);
	}

	__declspec(dllexport) void GI_PushEntRef(int value)
	{
		Scr_AddEntityNum(value & 0xFFFF, value >> 16);
	}

	__declspec(dllexport) void GI_PushVector(float x, float y, float z)
	{
		float value[3];
		value[0] = x;
		value[1] = y;
		value[2] = z;

		Scr_AddVector(value);
	}

	__declspec(dllexport) void GI_Call(int func, int entref, int numParams)
	{
		typedef void (__cdecl * scriptCall_t)(int entref);
		typedef int (__cdecl * __setjmp3_t)(void* env, int a2, int a3);
		typedef void (__cdecl* RemoveRefToValue_t)(int a1, int a2);

		__setjmp3_t __setjmp3 = (__setjmp3_t)0x62DD98;
		RemoveRefToValue_t RemoveRefToValue = (RemoveRefToValue_t)0x4D8E40;

		DWORD oldNumParam = *scr_numParam;
		*scr_numParam = numParams;
		
		scriptCall_t* instanceFunctions = (scriptCall_t*)0x1BD8BF8;
		scriptCall_t* globalFunctions = (scriptCall_t*)0x1BF84E4;
		scriptCall_t thisCall;
		
		if (func > 0x1C7)
		{
			// TODO: throw exception if not passing an entref	
			thisCall = instanceFunctions[func];
		}
		else
		{
			thisCall = globalFunctions[func];
		}

		*(DWORD*)0x1F3BB7C += 1;

		if (__setjmp3((void*)(0x1F3DB98 + (16 * *(DWORD*)0x1F3BB7C)), 0, 0))
		{
			// TODO: raise an exception
			DebugBreak();

			MessageBoxA(NULL, "Script error!", "Oshibka", MB_OK);

			*(DWORD*)0x1F3E410 -= (8 * numParams);
			*(DWORD*)0x1F3E414 = 0;
			*(DWORD*)0x1F3BB7C -= 1;

			*scr_numParam = oldNumParam;
			return;
		}

		*(DWORD*)0x1F3E414 = 0;

		thisCall(entref);

		__asm
		{
			mov eax, 4DC6C0h
			call eax
		}

		//*(DWORD*)0x1F3E410 -= (8 * numParams);
		*(DWORD*)0x1F3BB7C -= 1;

		*scr_numParam = oldNumParam;

		// handle return values
		Scriptability_HandleReturns();

		*(DWORD*)0x1F3E414 = 0;
	}

	__declspec(dllexport) void GI_SetField(int entref, int fieldID)
	{
		DWORD oldNumParam = *scr_numParam;
		*scr_numParam = 1;

		*(DWORD*)0x1F3BB7C += 1;
		*(DWORD*)0x1F3E414 = 0;

		DWORD _setField = 0x4A7350;
		int entType = (entref >> 16);
		int entNum = (entref & 0xFFFF);

		__asm
		{
			push fieldID
			push entNum
			push entType
			call _setField
			add esp, 0Ch

			mov eax, 4DC6C0h // clean up params
			call eax
		}

		*(DWORD*)0x1F3BB7C -= 1;
		*scr_numParam = oldNumParam;
		*(DWORD*)0x1F3E414 = 0;
	}

	__declspec(dllexport) void GI_GetField(int entref, int fieldID)
	{
		*(DWORD*)0x1F3BB7C += 1;
		*(DWORD*)0x1F3E414 = 0;

		DWORD _getField = 0x4A7440;
		int entType = (entref >> 16);
		int entNum = (entref & 0xFFFF);

		__asm
		{
			push fieldID
			push entNum
			push entType
			call _getField
			add esp, 0Ch
		}

		*(DWORD*)0x1F3BB7C -= 1;

		Scriptability_HandleReturns();

		*(DWORD*)0x1F3E414 = 0;
	}

	__declspec(dllexport) void GI_TempFunc()
	{
		short** arrayTable = (short**)0x6EAC78;
		int arrayEl = 6;
		int i = 0;

		for (short** entry = arrayTable; *entry != NULL; entry += arrayEl)
		{
			OutputDebugString(va("AddFieldMapping(\"%s\", %i);\n", SL_ConvertToString(**entry), i));
			i++;
		}

		arrayTable = (short**)0x6E5CB0;
		arrayEl = 6;
		i = 24576;

		for (short** entry = arrayTable; *entry != NULL; entry += arrayEl)
		{
			OutputDebugString(va("AddFieldMapping(\"%s\", %i);\n", SL_ConvertToString(**entry), i));
			i++;
		}

		arrayTable = (short**)0x705C70;
		arrayEl = 6;
		i = 32768;

		for (short** entry = arrayTable; *entry != NULL; entry += arrayEl)
		{
			OutputDebugString(va("AddFieldMapping(\"%s\", %i);\n", SL_ConvertToString(**entry), i));
			i++;
		}

		arrayTable = (short**)0x6E7050;
		arrayEl = 8;
		i = 0;

		for (short** entry = arrayTable; *entry != NULL; entry += arrayEl)
		{
			OutputDebugString(va("AddFieldMapping(\"%s\", %i);\n", SL_ConvertToString(**entry), i));
			i++;
		}

		arrayTable = (short**)0x790BE0;
		arrayEl = 4;
		i = 0;

		for (short** entry = arrayTable; *entry != NULL; entry += arrayEl)
		{
			OutputDebugString(va("AddFieldMapping(\"%s\", %i);\n", SL_ConvertToString(**entry), i));
			i++;
		}
	}

	__declspec(dllexport) int GI_GetObjectType(int object)
	{
		int retval = 0;
		DWORD GetObjectType = 0x4D9350;

		__asm
		{
			push object
			call GetObjectType
			add esp, 4h

			mov retval, eax
		}

		return retval;
	}

	__declspec(dllexport) void GI_SetDropItemEnabled(bool enabled)
	{
		/*if (enabled)
		{
			*(WORD*)0x47D53B = 0x0B75;
		}
		else
		{
			*(WORD*)0x47D53B = 0x9090;
		}*/
	}
};
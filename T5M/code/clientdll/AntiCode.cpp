#include "StdInc.h"

#include <stdio.h>
#include <stdlib.h>

#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "hwbrk.h"

#define MAX_SIGNATURES 256
/*typedef struct signature_s
{
	unsigned char* signature;
	char* mask;
	size_t size;
	unsigned char* replace;
	int replaceOffset;
	size_t replaceSize;
	void (*replaceCB)(void* address);
	int logOffset;
} signature_t;*/

FILE* progress;

size_t GetImageLength()
{
	IMAGE_DOS_HEADER* mzHeader = (IMAGE_DOS_HEADER*)0x400000;
	IMAGE_NT_HEADERS* peHeader = (IMAGE_NT_HEADERS*)((DWORD)mzHeader + mzHeader->e_lfanew);

	if(peHeader->Signature != IMAGE_NT_SIGNATURE) {
		return 0;
	}

	return (size_t)peHeader->OptionalHeader.SizeOfImage;
}

void PatchCEGFunctionPointer(void* address)
{
	DWORD callPoint = *(DWORD*)((char*)address + 1);
	DWORD function = ((DWORD)address) + callPoint + 5;
	DWORD target = 0;

	printf("\n");

	__asm
	{
		call function
		mov target, eax
	}
}

DWORD globalAddress;
DWORD lolReturn = 0;

LONG WINAPI Objection(LPEXCEPTION_POINTERS data)
{
	if ((DWORD)data->ExceptionRecord->ExceptionAddress == globalAddress)
	{
		DWORD* stack = (DWORD*)data->ContextRecord->Esp;
		data->ContextRecord->Eip = *stack;
		data->ContextRecord->Esp += 4;

		lolReturn = data->ContextRecord->Eax;
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

void PatchCEGFunctionPointer2(void* address)
{
	// first, find if this is a fake function
	bool fake = true;
	DWORD trg = (DWORD)address;

	unsigned char *pBasePtr = (unsigned char *)0x400000;
	unsigned char *pEndPtr = pBasePtr + 0x800000;//GetImageLength() - 0xF000;

	while (pBasePtr < pEndPtr)
	{
		if (*pBasePtr == 0xE8)
		{
			int weWant = trg - (DWORD)(pBasePtr + 5);

			if (*(int*)(pBasePtr + 1) == weWant)
			{
				fake = false;
				break;
			}
		}

		pBasePtr++;
	}

	if (fake) return;

	//DWORD callPoint = *(DWORD*)((char*)address + 1);
	//DWORD function = ((DWORD)address) + callPoint + 5;
	DWORD target = 0;
	//memset((char*)address + 5, 0xC3, 2);

	printf("b\n");

	//LPTOP_LEVEL_EXCEPTION_FILTER oldFilter = SetUnhandledExceptionFilter(Objection);
	HANDLE breakpoint = SetHardwareBreakpoint(GetCurrentThread(), HWBRK_TYPE_CODE, HWBRK_SIZE_1, (char*)address + 5);

	__try
	{
		globalAddress = (DWORD)((char*)address + 5);

		__asm
		{
			call address
		}
	}
	__except(Objection(GetExceptionInformation()))
	{
		ExitProcess(0xDECEA5ED);
	}

	RemoveHardwareBreakpoint(breakpoint);
	//SetUnhandledExceptionFilter(Objection);

	char wut[1024];
	sprintf(wut, "AddFPFixup(0x%x, 0x%x);\n", (DWORD)address, lolReturn);
	OutputDebugStringA(wut);
	//MessageBoxA(NULL, wut, "", 0);
	
}

void ProcessSignature(int id, signature_t* signature)
{
	unsigned char *pBasePtr = (unsigned char *)0x400000;
	unsigned char *pEndPtr = pBasePtr + 0x800000;//GetImageLength() - 0xF000;
	size_t i;
	int occurred = 0;

	while(pBasePtr < pEndPtr) {
		for(i = 0;i < signature->size;i++) {
			if((signature->mask[i] != '?') && (signature->signature[i] != pBasePtr[i]))
				break;
		}

		if(i == signature->size)
		{
			if (signature->replaceCB)
			{
				signature->replaceCB(pBasePtr + signature->replaceOffset);
			}

			if (signature->replace)
			{
				// patch output
				memcpy(pBasePtr + signature->replaceOffset, signature->replace, signature->replaceSize);
			}
		}

		pBasePtr++;
	}
}

void dumpMe()
{
	signature_t signatures[12];
	memset(signatures, 0, sizeof(signatures));

	signatures[0].signature = (unsigned char*)"\xCC\xE8\x00\x00\x00\x00\xFF\xE0\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC";
	signatures[0].mask = "xx????xxxxxxxxxxx";
	signatures[0].size = 17;
	signatures[0].logOffset = 1;
	signatures[0].replaceCB = PatchCEGFunctionPointer2;
	signatures[0].replaceOffset = 1;
	signatures[0].replaceSize = 7;

	signatures[1].signature = (unsigned char*)"\xC3\xE8\x00\x00\x00\x00\xFF\xE0\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC";
	signatures[1].mask = "xx????xxxxxxxxxxx";
	signatures[1].size = 17;
	signatures[1].logOffset = 1;
	signatures[1].replaceCB = PatchCEGFunctionPointer2;
	signatures[1].replaceOffset = 1;
	signatures[1].replaceSize = 7;

	signatures[2].signature = (unsigned char*)"\xCC\xCC\xCC\xCC\xCC\xCC\xE8\x00\x00\x00\x00\xFF\xE0";
	signatures[2].mask = "xxxxxxx????xx";
	signatures[2].size = 13;
	signatures[2].logOffset = 6;
	signatures[2].replaceCB = PatchCEGFunctionPointer2;
	signatures[2].replaceOffset = 6;
	signatures[2].replaceSize = 7;

	signatures[3].signature = (unsigned char*)"\xE8\x00\x00\x00\x00\xFF\xE0\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC";
	signatures[3].mask = "x????xxxxxxxxxxx";
	signatures[3].size = 16;
	signatures[3].logOffset = 0;
	signatures[3].replaceCB = PatchCEGFunctionPointer2;
	signatures[3].replaceOffset = 0;
	signatures[3].replaceSize = 7;
	
	for (int i = 0; i < 12; i++)
	{
		signature_t* signature = &signatures[i];

		if (signature->signature)
		{
			ProcessSignature(i, signature);
		}
	}

	MessageBoxA(NULL, "olol", "trolol", 0);
	ExitProcess(0xDEDEDEDE);
}

void addDumpCB(void* address)
{
	DWORD address2 = (DWORD)address;
	*(int*)(address2 + 1) = ((DWORD)dumpMe) - (address2 + 5);
}

void InitAntiCode()
{
	signature_t dumpSig;
	memset(&dumpSig, 0, sizeof(dumpSig));
	dumpSig.signature = (unsigned char*)"\xE8\x00\x00\x00\x00\x85\xC0\xA3\x00\x00\x00\x00\x74\x1F\xE8\x00\x00\x00\x00";
	dumpSig.mask = "x????xxx????xxx????";
	dumpSig.size = 19;
	dumpSig.replaceCB = addDumpCB;
	dumpSig.replaceOffset = 14;
	ProcessSignature(9, &dumpSig);
	//*(int*)0x6AFFEF = ((DWORD)dumpMe) - (0x6AFFEE + 5);
}
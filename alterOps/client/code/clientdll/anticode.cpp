#include <stdio.h>
#include <stdlib.h>

#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "hwbrk.h"

#define MAX_SIGNATURES 256
typedef struct signature_s
{
	unsigned char* signature;
	char* mask;
	size_t size;
	unsigned char* replace;
	int replaceOffset;
	size_t replaceSize;
	void (*replaceCB)(void* address);
	int logOffset;
} signature_t;

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
	sprintf_s(wut, "AddFPFixup(0x%x, 0x%x);\n", (DWORD)address, lolReturn);
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
#if nope
	static signature_t signatures[MAX_SIGNATURES];
	memset(signatures, 0, sizeof(signatures));

	signatures[22].signature = (unsigned char*)"\xCC\xE8\x00\x00\x00\x00\xFF\xE0\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC";
	signatures[22].mask = "xx????xxxxxxxxxxx";
	signatures[22].size = 17;
	signatures[22].logOffset = 1;
	//signatures[0].replace = (unsigned char*)"\xCC\xCC\xCC\xCC\xCC\xCC\xCC";
	//signatures[22].replaceCB = PatchCEGFunctionPointer;
	signatures[22].replaceOffset = 1;
	signatures[22].replaceSize = 7;

	signatures[1].signature = (unsigned char*)"\x6A\x00\xE8\x00\x00\x00\x00\x8A\x44\x24\x1E"; // note: causes 1 false positive
	signatures[1].mask = "??x????xxxx";
	signatures[1].size = 11;
	signatures[1].logOffset = 7;
	signatures[1].replace = (unsigned char*)"\xB0\x01\x90\x90";
	signatures[1].replaceOffset = 7;
	signatures[1].replaceSize = 4;

	signatures[2].signature = (unsigned char*)"\x24\x03\x6A\x00\x50\xE8\x00\x00\x00\x00\x83";
	signatures[2].mask = "xxx?xx????x";
	signatures[2].size = 11;
	signatures[2].logOffset = 5;
	signatures[2].replace = (unsigned char*)"\xB0\x01\x90\x90\x90";
	signatures[2].replaceOffset = 5;
	signatures[2].replaceSize = 5;

	signatures[3].signature = (unsigned char*)"\x8A\x44\x24\x12\x5F\x5E\x5B";
	signatures[3].mask = "xxxxxxx";
	signatures[3].size = 7;
	signatures[3].logOffset = 0;
	signatures[3].replace = (unsigned char*)"\xB0\x01\x90\x90";
	signatures[3].replaceOffset = 0;
	signatures[3].replaceSize = 4;

	signatures[4].signature = (unsigned char*)"\xC3\xE8\x00\x00\x00\x00\xFF\xE0\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC";
	signatures[4].mask = "xx????xxxxxxxxxxx";
	//signatures[4].replace = (unsigned char*)"\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC";
	signatures[4].size = 17;
	signatures[4].logOffset = 1;
	//signatures[4].replace = (unsigned char*)"\xCC\xCC\xCC\xCC\xCC\xCC\xCC";
	signatures[4].replaceOffset = 1;
	signatures[4].replaceSize = 7;

	signatures[5].signature = (unsigned char*)"\x80\x7C\x24\x04\x00\x75\x56\x56";
	signatures[5].mask = "xxxxxxxx";
	signatures[5].replace = (unsigned char*)"";
	signatures[5].size = 8;
	signatures[5].logOffset = 0;
	signatures[5].replace = (unsigned char*)"\xB0\x01\x90\x90\x90\xEB";
	signatures[5].replaceOffset = 0;
	signatures[5].replaceSize = 6;

	signatures[6].signature = (unsigned char*)"\xE8\x00\x00\x00\x00\x8A\x44\x24\x1F\x83";
	signatures[6].mask = "x????xxxxx";
	signatures[6].replace = (unsigned char*)"";
	signatures[6].size = 10;
	signatures[6].logOffset = 5;
	signatures[6].replace = (unsigned char*)"\xB0\x01\x90\x90";
	signatures[6].replaceOffset = 5;
	signatures[6].replaceSize = 4;

	signatures[7].signature = (unsigned char*)"\xE8\x00\x00\x00\x00\x8A\x44\x24\x1B\x83";
	signatures[7].mask = "x????xxxxx";
	signatures[7].replace = (unsigned char*)"";
	signatures[7].size = 10;
	signatures[7].logOffset = 5;
	signatures[7].replace = (unsigned char*)"\xB0\x01\x90\x90";
	signatures[7].replaceOffset = 5;
	signatures[7].replaceSize = 4;

	signatures[8].signature = (unsigned char*)"\x5E\x5D\x32\xC0\x5B\x81\xC4\x74\x02";
	signatures[8].mask = "xxxxxxxxx";
	signatures[8].replace = (unsigned char*)"";
	signatures[8].size = 9;
	signatures[8].logOffset = 2;
	signatures[8].replace = (unsigned char*)"\xB0\x01";
	signatures[8].replaceOffset = 2;
	signatures[8].replaceSize = 2;

	signatures[9].signature = (unsigned char*)"\xE8\x00\x00\x00\x00\x84\xC0\x75\x00\xB0\x01";
	signatures[9].mask = "x????xxx?xx";
	signatures[9].size = 11;
	signatures[9].logOffset = 7;
	signatures[9].replace = (unsigned char*)"\x90\x90";
	signatures[9].replaceOffset = 7;
	signatures[9].replaceSize = 2;

	signatures[10].signature = (unsigned char*)"\x83\xC4\x08\x8A\x44\x24\x0F\x5F\x5E\x00\x00\x00\x00\x00\xCC";
	signatures[10].mask = "xxxxxx?xx?????x";
	signatures[10].replace = (unsigned char*)"";
	signatures[10].size = 15;
	signatures[10].logOffset = 3;
	signatures[10].replace = (unsigned char*)"\xB0\x01\x90\x90";
	signatures[10].replaceOffset = 3;
	signatures[10].replaceSize = 4;

	signatures[11].signature = (unsigned char*)"\x83\xC4\x08\x8A\x44\x24\x0F\x5F\x5E\x00\x00\x00\x00\x00\x00\x00\x00\x00\xCC";
	signatures[11].mask = "xxxxxx?xx?????????x";
	signatures[11].replace = (unsigned char*)"";
	signatures[11].size = 19;
	signatures[11].logOffset = 3;
	signatures[11].replace = (unsigned char*)"\xB0\x01\x90\x90";
	signatures[11].replaceOffset = 3;
	signatures[11].replaceSize = 4;

	signatures[12].signature = (unsigned char*)"\x6A\x00\x6A\x00\xE8\x00\x00\x00\x00\x8A\x44\x24\x00\x5F\x5E";
	signatures[12].mask = "x?x?x????xxx?xx";
	signatures[12].replace = (unsigned char*)"";
	signatures[12].size = 15;
	signatures[12].logOffset = 9;
	signatures[12].replace = (unsigned char*)"\xB0\x01\x90\x90";
	signatures[12].replaceOffset = 9;
	signatures[12].replaceSize = 4;

	signatures[13].signature = (unsigned char*)"\x74\x00\x6A\x00\x6A\x00\xE8\x00\x00\x00\x00\x83\xC4\x08\x5F\x5E\x8A\xC3\x5B";
	signatures[13].mask = "x?x?x?x????xxxxxxxx";
	signatures[13].replace = (unsigned char*)"";
	signatures[13].size = 19;
	signatures[13].logOffset = 16;
	signatures[13].replace = (unsigned char*)"\xB0\x01";
	signatures[13].replaceOffset = 16;
	signatures[13].replaceSize = 2;

	signatures[14].signature = (unsigned char*)"\x6A\x00\x6A\x00\xE8\x00\x00\x00\x00\x8A\x44\x24\x00\x83\xC4\x0C";
	signatures[14].mask = "x?x?x????xxx?xxx";
	signatures[14].replace = (unsigned char*)"";
	signatures[14].size = 16;
	signatures[14].logOffset = 9;
	signatures[14].replace = (unsigned char*)"\xB0\x01\x90\x90";
	signatures[14].replaceOffset = 9;
	signatures[14].replaceSize = 4;

	signatures[15].signature = (unsigned char*)"\x53\x53\xE8\x00\x00\x00\x00\x8A\x44\x24\x00\x83\xC4\x08";
	signatures[15].mask = "xxx????xxx?xxx";
	signatures[15].replace = (unsigned char*)"";
	signatures[15].size = 14;
	signatures[15].logOffset = 7;
	signatures[15].replace = (unsigned char*)"\xB0\x01\x90\x90";
	signatures[15].replaceOffset = 7;
	signatures[15].replaceSize = 4;

	signatures[16].signature = (unsigned char*)"\x6A\x00\x6A\x00\xE8\x00\x00\x00\x00\x83\xC4\x08\x8A\x44\x24\x00\x5F\x5E";
	signatures[16].mask = "x?x?x????xxxxxx?xx";
	signatures[16].replace = (unsigned char*)"";
	signatures[16].size = 18;
	signatures[16].logOffset = 12;
	signatures[16].replace = (unsigned char*)"\xB0\x01\x90\x90";
	signatures[16].replaceOffset = 12;
	signatures[16].replaceSize = 4;

	signatures[17].signature = (unsigned char*)"\xCC\xCC\xCC\xCC\xCC\xCC\xE8\x00\x00\x00\x00\xFF\xE0";
	signatures[17].mask = "xxxxxxx????xx";
	//signatures[17].replace = (unsigned char*)"";
	signatures[17].size = 13;
	signatures[17].logOffset = 6;
	//signatures[17].replace = (unsigned char*)"\xCC\xCC\xCC\xCC\xCC\xCC\xCC";
	signatures[17].replaceOffset = 6;
	signatures[17].replaceSize = 7;

	signatures[18].signature = (unsigned char*)"\x6A\x00\x6A\x01\x8D\x4C\x24\x0C";
	signatures[18].mask = "xxxxxxxx";
	signatures[18].replace = (unsigned char*)"";
	signatures[18].size = 8;
	signatures[18].logOffset = 1;
	signatures[18].replace = (unsigned char*)"\x01";
	signatures[18].replaceOffset = 1;
	signatures[18].replaceSize = 1;

	signatures[19].signature = (unsigned char*)"\x6A\xFF\x68\x00\x01\x00\x00\x6A\x02";
	signatures[19].mask = "xxxxxxxxx";
	signatures[19].replace = (unsigned char*)"";
	signatures[19].size = 9;
	signatures[19].logOffset = 0;
	signatures[19].replace = (unsigned char*)"\xEB\x1C";
	signatures[19].replaceOffset = 0;
	signatures[19].replaceSize = 2;

	signatures[20].signature = (unsigned char*)"\x83\xFF\xFF\x74\x7A\x8B";
	signatures[20].mask = "xxxxxx";
	signatures[20].replace = (unsigned char*)"";
	signatures[20].size = 6;
	signatures[20].logOffset = 3;
	signatures[20].replace = (unsigned char*)"\xEB";
	signatures[20].replaceOffset = 3;
	signatures[20].replaceSize = 1;

	signatures[21].signature = (unsigned char*)"\xE8\x00\x00\x00\x00\xFF\xE0\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC";
	signatures[21].mask = "x????xxxxxxxxxxx";
	//signatures[21].replace = (unsigned char*)"";
	signatures[21].size = 16;
	signatures[21].logOffset = 0;
	//signatures[21].replace = (unsigned char*)"\xCC\xCC\xCC\xCC\xCC\xCC\xCC";
	signatures[21].replaceOffset = 0;
	signatures[21].replaceSize = 7;
#endif

	/*for (int i = 0; i < MAX_SIGNATURES; i++)
	{
		signature_t* signature = &signatures[i];

		if (signature->signature)
		{
			ProcessSignature(i, signature);
		}
	}

	PatchCEGFunctionPointer((void*)0x600600);
	PatchCEGFunctionPointer((void*)0x600600);
	PatchCEGFunctionPointer2((void*)0x600600);
	PatchCEGFunctionPointer2((void*)0x600600);*/

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
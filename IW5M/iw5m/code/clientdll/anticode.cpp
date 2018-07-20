#include "stdinc.h"
#include <stdio.h>
#include <stdlib.h>

#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define MAX_SIGNATURES 256
typedef struct signature_s
{
	unsigned char* signature;
	char* mask;
	size_t size;
	unsigned char* replace;
	int replaceOffset;
	size_t replaceSize;
	void (*replaceCB)(char* address, void* userData);
	void* userData;
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
			DWORD addy = ((DWORD)pBasePtr) + signature->logOffset;
			//printf(" | grep -v %x", addy);

			occurred++;
			OutputDebugString(va("0x%x / %d\n", addy, id));

			// patch output
			if (signature->replaceCB)
			{
				signature->replaceCB((char*)(pBasePtr + signature->replaceOffset), signature->userData);
			}

			if (signature->replace)
			{
				memcpy(pBasePtr + signature->replaceOffset, signature->replace, signature->replaceSize);
			}
		}

		pBasePtr++;
	}

	OutputDebugString(va("signature %d occurred %d times\n", id, occurred));
}

void PatchCEGCRC1(char* addr, void*)
{
	*(DWORD*)(addr) = 0x90909090;
	*(BYTE*)(addr + 4) = 0x90;
	*(DWORD*)(addr + 8) = 0xF883C033;
	*(WORD*)(addr + 12) = 0x9000;
}

void PatchCEGCRC2(char* addr, void*)
{
	*(DWORD*)(addr + 1) = 0x90909090;
	*(BYTE*)(addr + 5) = 0x90;
	*(DWORD*)(addr + 9) = 0xF883C033;
	*(WORD*)(addr + 13) = 0x9000;
}

void AntiCode_Assign(char* addr, void* variable)
{
	*(DWORD*)variable = (DWORD)addr;
}

void InitAntiCode()
{
	static signature_t signatures[24];
	memset(signatures, 0, sizeof(signatures));

	// type1 crc
	signatures[0].signature = (unsigned char*)"\xE8????\x83\xC4\x1C\x3B\x05????\x0F\x94\xC3\x84\xDB\x75";
	signatures[0].mask = "x????xxxxx????xxxxxx";
	signatures[0].size = 20;
	signatures[0].logOffset = 0;
	signatures[0].replaceCB = PatchCEGCRC1;
	signatures[0].replaceOffset = 0;

	// type2 crc
	signatures[1].signature = (unsigned char*)"\x57\xE8????\x83\xC4\x1C\x3B\x86????\x0F\x94\xC1\x83\xC6\x04";
	signatures[1].mask = "xx????xxxxx????xxxxxx";
	signatures[1].size = 20;
	signatures[1].logOffset = 0;
	signatures[1].replaceCB = PatchCEGCRC2;
	signatures[1].replaceOffset = 0;
	signatures[1].replaceSize = 7;

	// hkcr guid check
	signatures[2].signature = (unsigned char*)"\x81\xEC??\x00\x00\xE8????\x84\xC0\x75\x09\xB0\x01\x81\xC4??\x00\x00\xC3";
	signatures[2].mask = "xx??xxx????xxxxxxxx??xxx";
	signatures[2].size = 24;
	signatures[2].logOffset = 0;
	signatures[2].replace = (unsigned char*)"\x90\x90";
	signatures[2].replaceOffset = 13;
	signatures[2].replaceSize = 2;

	// steam ipc check
	signatures[3].signature = (unsigned char*)"\x83\xEC\x48\x53\x55\x56\x57\x6a\x01\x8d\x44\x24\x34\x50\x33\xed\xff\x15";
	signatures[3].mask = "xxxxxxxxxxxxxxxxxx";
	signatures[3].size = 18;
	signatures[3].logOffset = 0;
	signatures[3].replace = (unsigned char*)"\x33\xc0\x40\xc3";
	signatures[3].replaceOffset = 0;
	signatures[3].replaceSize = 4;

	// boring stuff
	// MSG_Init
	signatures[4].signature = (unsigned char*)"\x33\xC9\x89\x08\x89\x48\x14\x89\x48\x1C\x89\x48\x20\x89\x48\x24";
	signatures[4].mask = "xxxxxxxxxxxxxxxx";
	signatures[4].size = 16;
	signatures[4].logOffset = 0;
	signatures[4].replaceCB = AntiCode_Assign;
	signatures[4].replaceOffset = -22;
	signatures[4].userData = &MSG_Init;

	// DB_FindXAssetHeader
	signatures[5].signature = (unsigned char*)"\x51\x53\x8B\x1D????\x55\x56\x57\x8B\x7C\x24\x18\xC7";
	signatures[5].mask = "xxxx????xxxxxxxx";
	signatures[5].size = 16;
	signatures[5].logOffset = 0;
	signatures[5].replaceCB = AntiCode_Assign;
	signatures[5].replaceOffset = 0;
	signatures[5].userData = &DB_FindXAssetHeader;

	signatures[6].signature = (unsigned char*)"\x6A\x1F\xE8\x00\x00\x00\x00\x8B\x44\x24\x0C\x8A\x08\x83\xC4\x04\x80\xF9\x70";
	signatures[6].mask = "xxx????xxxxxxxxxxxx";
	signatures[6].size = strlen(signatures[6].mask);
	signatures[6].logOffset = 0;
	signatures[6].replaceCB = AntiCode_Assign;
	signatures[6].replaceOffset = 0;
	signatures[6].userData = &Cbuf_AddText;
	
	for (int i = 0; i < 7; i++)
	{
		signature_t* signature = &signatures[i];

		if (signature->signature)
		{
			ProcessSignature(i, signature);
		}
	}
}
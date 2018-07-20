#pragma once

#define CALL_NEAR32 0xE8U
#define JMP_NEAR32 0xE9U
#define NOP 0x90U

struct CallHook {
        BYTE bOriginalCode[5];
        PBYTE pPlace;
        PVOID pOriginal;

        void initialize(const char* pOriginalCode, PBYTE place);
        int installHook(void (*hookToInstall)(), bool unprotect);
        int releaseHook(bool unprotect);
};

struct PointerHook {
        PVOID* pPlace;
        PVOID pOriginal;

        void initialize(PVOID* place);
        int installHook(void (*hookToInstall)(), bool unprotect);
        int releaseHook(bool unprotect);
};

struct StompHook {
        BYTE bOriginalCode[15];
        BYTE bCountBytes;
        PBYTE pPlace;

        void initialize(const char* pOriginalCode, BYTE countBytes, PBYTE place);
        int installHook(void (*hookToInstall)(), bool useJump, bool unprotect);
        int releaseHook(bool unprotect);
};
 
enum eCallPatcher
{
	PATCH_CALL,
	PATCH_JUMP,
	PATCH_NOTHING
};

void _patch(void* pAddress, DWORD data, DWORD iSize);
void _nop(void* pAddress, DWORD size);
void _call(void* pAddress, DWORD data, eCallPatcher bPatchType);

#define patch(a, v, s) _patch((void*)(a), (DWORD)(v), (s))
#define nop(a, v) _nop((void*)(a), (v))
#define call(a, v, bAddCall) _call((void*)(a), (DWORD)(v), (bAddCall))
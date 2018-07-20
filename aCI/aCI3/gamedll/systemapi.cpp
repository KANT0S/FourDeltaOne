// ==========================================================
// aCI3 project
// 
// Component: gamedll
// Purpose: Native system API functions.
//
// Initial author: NTAuthority
// Started: 2013-01-05
// ==========================================================

#include "aci3.h"
#include <intrin.h>

#include "detection.h"

namespace ci
{
	extern Detection* g_macTokenDetectionPtr;

	static SystemAPI g_systemAPI;

	void SystemAPI::initializeSystemApi()
	{
		g_systemAPI.initialize();
	}

#define DECLARE_CONST_UNICODE_STRING(_var, _string) \
	const WCHAR _var ## _buffer[] = _string; \
	const UNICODE_STRING _var = { sizeof(_string) - sizeof(WCHAR), sizeof(_string), (PWCH) _var ## _buffer } 

	void SystemAPI::initialize()
	{
		this->initializeSystemCalls();

		/*g_macTokenDetectionPtr->initialize();

		DECLARE_CONST_UNICODE_STRING(str, L"\\Registry\\Machine\\Cassis");

		OBJECT_ATTRIBUTES attributes;
		InitializeObjectAttributes(&attributes, (PUNICODE_STRING)&str, 0, NULL, NULL);
		
		HANDLE hnd;

		int res = NtOpenKey(&hnd, KEY_READ, &attributes);

		__asm int 3*/
	}

	void SystemAPI::initializeSystemCalls()
	{
		DWORD wow64Func = __readfsdword(0xC0);

		if (wow64Func)
		{
			this->systemCallFunc = SystemAPI::WOW64SystemCall;
		}
		else
		{
			int cpuid[4];
			//__cpuid(cpuid, 1);

			// if the game runs on early 686s, this may need a family check for GenuineIntel
			if (cpuid[3] & 0x800 && false)
			{
				this->systemCallFunc = SystemAPI::sysEnterSystemCall;
			}
			else
			{
				this->systemCallFunc = SystemAPI::int2ESystemCall;
			}
		}
	}

	NTSTATUS __declspec(naked) SystemAPI::doSystemCall()
	{
		__asm
		{
			cmp dword ptr fs:[0C0h], 0
			jz do32

			jmp g_systemAPI.systemCallFunc

do32:
			call g_systemAPI.systemCallFunc
			retn
		}
	}

	static void __declspec(naked) doReturn()
	{
		__asm
		{
			add esp, 4h
			retn
		}
	}

	// system call functions
	NTSTATUS __declspec(naked) SystemAPI::int2ESystemCall()
	{
		__asm
		{
			xor ecx, ecx
			lea edx, [esp + 8]

			int 2Eh

			call doReturn
		}
	}

	NTSTATUS __declspec(naked) SystemAPI::sysEnterSystemCall()
	{
		__asm
		{
			xor ecx, ecx
			lea edx, [esp]

			__emit 0x0F
			__emit 0x34

			call doReturn
		}
	}

	NTSTATUS __declspec(naked) SystemAPI::WOW64SystemCall()
	{
		__asm
		{
			xor ecx, ecx
			lea edx, [esp + 4]

			call dword ptr fs:0C0h

			call doReturn
		}
	}
}
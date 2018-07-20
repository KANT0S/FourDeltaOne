#include "aci3.h"
#include "moduleutils.h"

//STRCRYPT_START
#pragma code_seg(".ci")
#pragma comment(linker, "/section:.ci,rwe")

STRCRYPT_ALIGN const char* g_ntdll = "lib2.dll";

#define DECLARE_SYSTEM_CALL(name, arguments) \
	STRCRYPT_ALIGN const char* name##_name = #name; \
	\
	NTSYSCALLAPI NTSTATUS __declspec(naked) NTAPI name arguments { \
		__asm mov eax, 0FFFFFFFFh \
		__asm jmp ci::SystemAPI::doSystemCall \
	} \
	\
	ci::SystemCall name##_sysCall(name##_name, name)

#define DECLARE_SYSTEM_CALL2(name, obname, arguments) \
	STRCRYPT_ALIGN const char* name##_name = obname; \
	\
	NTSYSCALLAPI NTSTATUS __declspec(naked) NTAPI name arguments { \
		__asm mov eax, 0FFFFFFFFh \
		__asm jmp ci::SystemAPI::doSystemCall \
	} \
	\
	ci::SystemCall name##_sysCall(name##_name, name, true)

DECLARE_SYSTEM_CALL2(NtCreateFile, "\x81\xBA\x8E\xBE\xAE\xAB\xBD\xAD\x81\xAF\xA9\xA1", (PHANDLE FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock, PLARGE_INTEGER AllocationSize, ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions, PVOID EaBuffer, ULONG EaLength));
DECLARE_SYSTEM_CALL2(NtOpenKey, "\x81\xBA\x82\xBC\xAE\xA4\x82\xAD\xBE", (PHANDLE KeyHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes));
DECLARE_SYSTEM_CALL2(NtDeviceIoControlFile, "\x81\xBA\x89\xA9\xBD\xA3\xAA\xAD\x8E\xA9\x86\xAB\xAD\xB6\xB3\xAF\xD3\xF8\xD4\xD0\xDE", (HANDLE DeviceHandle, HANDLE Event OPTIONAL, PIO_APC_ROUTINE UserApcRoutine OPTIONAL, PVOID UserApcContext OPTIONAL, PIO_STATUS_BLOCK IoStatusBlock, ULONG IoControlCode, PVOID InputBuffer, ULONG InputBufferSize, PVOID OutputBuffer, ULONG OutputBufferSize));
DECLARE_SYSTEM_CALL2(NtWaitForSingleObject, "\x81\xBA\x9A\xAD\xA2\xBE\x8F\xA7\xB5\x95\xAC\xAA\xA4\xAE\xA4\x8F\xDD\xD4\xD8\xDF\xCF", (HANDLE Object, BOOLEAN Alertable, PLARGE_INTEGER Time));
DECLARE_SYSTEM_CALL2(NtProtectVirtualMemory, "\x81\xBA\x9D\xBE\xA4\xBE\xAC\xAB\xB3\x90\xAC\xB6\xB7\xB7\xA0\xAC\xF2\xDB\xD0\xD3\xC9\xC3", (IN HANDLE ProcessHandle, IN PVOID *BaseAddress, IN SIZE_T *NumberOfBytesToProtect, IN ULONG NewAccessProtection, OUT PULONG OldAccessProtection));
DECLARE_SYSTEM_CALL2(NtFreeVirtualMemory, "\x81\xBA\x8B\xBE\xAE\xAF\x9F\xA1\xB5\xB2\xB0\xA5\xAF\x8F\xA4\xAD\xD0\xCC\xC4", (IN HANDLE ProcessHandle, IN PVOID *BaseAddress, IN PSIZE_T RegionSize, IN ULONG FreeType));
DECLARE_SYSTEM_CALL2(NtAllocateVirtualMemory, "\x81\xBA\x8C\xA0\xA7\xA5\xAA\xA9\xB3\xA3\x93\xAD\xB1\xB6\xB4\xA1\xD3\xF3\xD8\xD1\xD4\xC8\xC0", (IN HANDLE ProcessHandle, IN OUT PVOID *BaseAddress, IN ULONG_PTR ZeroBits, IN OUT PSIZE_T RegionSize, IN ULONG AllocationType, IN ULONG Protect));
DECLARE_SYSTEM_CALL2(NtTerminateThread, "\x81\xBA\x99\xA9\xB9\xA7\xA0\xA6\xA6\xB2\xA0\x90\xAB\xB0\xA4\xA1\xDB", (IN HANDLE ThreadHandle, IN NTSTATUS ExitStatus));
DECLARE_SYSTEM_CALL2(NtCreateThread, "\x81\xBA\x8E\xBE\xAE\xAB\xBD\xAD\x93\xAE\xB7\xA1\xA2\xA6", (OUT PHANDLE ThreadHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL, IN HANDLE ProcessHandle, OUT PCLIENT_ID ClientId, IN PCONTEXT ThreadContext, IN PINITIAL_TEB UserStack, IN BOOLEAN CreateSuspended));
DECLARE_SYSTEM_CALL2(NtClose, "\x81\xBA\x8E\xA0\xA4\xB9\xAC", (IN HANDLE Handle));
DECLARE_SYSTEM_CALL2(NtQueryValueKey, "\x81\xBA\x9C\xB9\xAE\xB8\xB0\x9E\xA6\xAA\xB0\xA1\x88\xA7\xB8", (IN HANDLE KeyHandle, IN PUNICODE_STRING ValueName, IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass, OUT PVOID KeyValueInformation, IN ULONG Length, OUT PULONG ResultLength));
DECLARE_SYSTEM_CALL2(NtSetInformationThread, "\x81\xBA\x9E\xA9\xBF\x83\xA7\xAE\xA8\xB4\xA8\xA5\xB7\xAB\xAE\xAE\xEB\xD6\xCF\xD9\xDA\xDE", (IN HANDLE ThreadHandle, IN THREADINFOCLASS ThreadInformationClass, IN PVOID ThreadInformation, IN ULONG ThreadInformationLength));

#pragma code_seg()
//STRCRYPT_END

namespace ci
{
	SystemCall::SystemCall(const char* name, void* function, bool obfuscated)
	{
		VMProtectBeginMutation(__FUNCTION__);

		// ntdll.dll
		resolveFunctionFromDll(ModuleUtils::getModule(StringObfuscation::deobfuscateString("\xA1\xBA\xA9\xA0\xA7\xE4\xAD\xA4\xAB")), (obfuscated) ? StringObfuscation::deobfuscateString(name) : name, function);		

		VMProtectEnd();
	}

	void SystemCall::resolveFunctionFromDll(Module* module, const char* name, void* function)
	{
		VMProtectBeginVirtualization(__FUNCTION__);

		LPBYTE sourceFunction = (LPBYTE)module->getProcedure(name);
		LPBYTE functionData = (LPBYTE)function;

		if (!sourceFunction)
		{
			return;
		}

		// check if both functions have the right instruction at the start
		if (*sourceFunction != 0xB8) // mov eax, imm32
		{
			// probably some antivirus hooking the function, we'll make a jump to the modified function then
			DWORD functionDataStart = (DWORD)(functionData + 5);

			*functionData = 0xE9;
			*(int*)(functionData + 1) = (DWORD)sourceFunction - functionDataStart;

			//MessageBoxA(NULL, name, "ERROR!", MB_OK);
			//ExitProcess(0);
			return;
		}

		if (*functionData != 0xB8)
		{
			return;
		}

		// copy the syscall number to the target instruction
		*(DWORD*)(functionData + 1) = *(DWORD*)(sourceFunction + 1);

		VMProtectEnd();
	}
}
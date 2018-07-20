// ==========================================================
// aCI3 project
// 
// Component: gamedll
// Purpose: Wrappers around NT system calls.
//
// Initial author: NTAuthority
// Started: 2013-02-03
// ==========================================================

#include "aci3.h"
#include "sysutils.h"
#include <ketypes.h>

namespace ci
{
NTSTATUS FileUtils::createFile(const wchar_t* filename, ACCESS_MASK access, ULONG createDisposition, HANDLE* handle)
{
	size_t length;
	StringCbLength(filename, 2048, &length);

	UNICODE_STRING string;
	string.Buffer = const_cast<wchar_t*>(filename);
	string.Length = length;
	string.MaximumLength = string.Length + sizeof(wchar_t);

	OBJECT_ATTRIBUTES attributes;
	InitializeObjectAttributes(&attributes, &string, OBJ_CASE_INSENSITIVE, NULL, NULL);

	IO_STATUS_BLOCK ioStatusBlock;

	NTSTATUS result = NtCreateFile(handle, SYNCHRONIZE | access, &attributes, &ioStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE, createDisposition, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);

	return result;
}

NTSTATUS FileUtils::openDevice(const wchar_t* devicename, HANDLE* handle)
{
	return createFile(devicename, GENERIC_EXECUTE, FILE_OPEN_IF, handle);
}

NTSTATUS FileUtils::performIOCTL(HANDLE handle, ULONG controlCode, VOID* inputBuffer, ULONG inLength, VOID* outputBuffer, ULONG* outLength)
{
	IO_STATUS_BLOCK ioStatusBlock;

	NTSTATUS result = NtDeviceIoControlFile(handle, NULL, NULL, NULL, &ioStatusBlock, controlCode, inputBuffer, inLength, outputBuffer, *outLength);

	if (NT_SUCCESS(result))
	{
		NtWaitForSingleObject(handle, FALSE, NULL);
		*outLength = ioStatusBlock.Information;
	}

	return result;
}

NTSTATUS RegistryUtils::openKey(const wchar_t* key, ACCESS_MASK access, HANDLE* handle)
{
	size_t length;
	StringCbLength(key, 2048, &length);

	UNICODE_STRING string;
	string.Buffer = const_cast<wchar_t*>(key);
	string.Length = length;
	string.MaximumLength = string.Length + sizeof(wchar_t);

	OBJECT_ATTRIBUTES attributes;
	InitializeObjectAttributes(&attributes, &string, OBJ_CASE_INSENSITIVE, NULL, NULL);

	IO_STATUS_BLOCK ioStatusBlock;

	NTSTATUS result = NtOpenKey(handle, access, &attributes);

	return result;
}

NTSTATUS RegistryUtils::queryKey(HANDLE handle, const wchar_t* valueName, KEY_VALUE_PARTIAL_INFORMATION** resultB, size_t* resultLength)
{
	size_t length;
	StringCbLength(valueName, 2048, &length);

	UNICODE_STRING string;
	string.Buffer = const_cast<wchar_t*>(valueName);
	string.Length = length;
	string.MaximumLength = string.Length + sizeof(wchar_t);
	
	NTSTATUS result = NtQueryValueKey(handle, &string, KeyValuePartialInformation, NULL, 0, (ULONG*)resultLength);

	*resultB = (KEY_VALUE_PARTIAL_INFORMATION*)halloc(*resultLength);

	result = NtQueryValueKey(handle, &string, KeyValuePartialInformation, *resultB, *resultLength, (ULONG*)resultLength);

	return result;
}

#define ROUND_DOWN(n, align) (((ULONG)n) & ~((align) - 1l))

#define ROUND_UP(n, align) ROUND_DOWN(((ULONG)n) + (align) - 1, (align))


ThreadUtils g_threadUtils;

ThreadUtils::ThreadUtils()
{
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);

	allocationGranulatity = systemInfo.dwAllocationGranularity;
	pageSize = systemInfo.dwPageSize;
}

NTSTATUS ThreadUtils::createThread(ThreadFunc function, LPVOID parameter, PHANDLE handle)
{
	INITIAL_TEB initialTeb;

	NTSTATUS status = baseCreateStack(0, 0, &initialTeb);

	if (NT_SUCCESS(status))
	{
		CONTEXT context;
		HANDLE thandle;
		CLIENT_ID clientID;

		baseCreateInitialContext(&context, parameter, function, initialTeb.StackBase, 0);

		status = NtCreateThread(&thandle, THREAD_ALL_ACCESS, NULL, NtCurrentProcess(), &clientID, &context, &initialTeb, FALSE);

		if (handle)
		{
			*handle = thandle;
		}
	}
	
	return status;
}

NTSTATUS ThreadUtils::baseCreateInitialContext(PCONTEXT Context, PVOID Parameter, PVOID StartAddress, PVOID StackAddress, ULONG ContextType)
{
	ULONG ContextFlags;

	/* Setup the Initial Win32 Thread Context */
	Context->Eax = (ULONG)StartAddress;
	Context->Ebx = (ULONG)Parameter;
	Context->Esp = (ULONG)StackAddress;
	/* The other registers are undefined */

	/* Setup the Segments */
	Context->SegFs = KGDT_R3_TEB;
	Context->SegEs = KGDT_R3_DATA;
	Context->SegDs = KGDT_R3_DATA;
	Context->SegCs = KGDT_R3_CODE;
	Context->SegSs = KGDT_R3_DATA;
	Context->SegGs = 0;

	/* Set the Context Flags */
	ContextFlags = Context->ContextFlags;
	Context->ContextFlags = CONTEXT_FULL;

	/* Give it some room for the Parameter */
	Context->Esp -= sizeof(PVOID);

	/* Set the EFLAGS */
	Context->EFlags = 0x3000; /* IOPL 3 */

	/* For Threads */
	Context->Eip = (ULONG)ThreadUtils::threadInitStub;

	return STATUS_SUCCESS;
}

void __stdcall ThreadUtils::threadInitFunc(LPVOID startAddress, PVOID parameter)
{
	ThreadFunc startFunction = (ThreadFunc)startAddress;

	NtTerminateThread(NtCurrentThread(), startFunction(parameter));
}

void __declspec(naked) ThreadUtils::threadInitStub()
{
	__asm
	{
		push ebx
		push eax
		call ThreadUtils::threadInitFunc
	}
}

NTSTATUS ThreadUtils::baseCreateStack(SIZE_T StackReserve, SIZE_T StackCommit, PINITIAL_TEB InitialTeb)
{
	NTSTATUS Status;
	PIMAGE_NT_HEADERS Headers;
	ULONG_PTR Stack;
	BOOLEAN UseGuard;
	ULONG PageSize, Dummy, AllocationGranularity;
	SIZE_T StackReserveHeader, StackCommitHeader, GuardPageSize, GuaranteedStackCommit;

	/* Read page size */
	PageSize = g_threadUtils.pageSize;
	AllocationGranularity = g_threadUtils.allocationGranulatity;

	/* Get the Image Headers */
	PIMAGE_DOS_HEADER DosHeader;
	DosHeader = (PIMAGE_DOS_HEADER)(NtCurrentPeb()->ImageBaseAddress);

	Headers = (PIMAGE_NT_HEADERS)(((char*)(NtCurrentPeb()->ImageBaseAddress)) + DosHeader->e_lfanew);
	if (!Headers) return STATUS_INVALID_IMAGE_FORMAT;

	StackCommitHeader = Headers->OptionalHeader.SizeOfStackCommit;
	StackReserveHeader = Headers->OptionalHeader.SizeOfStackReserve;

	if (!StackReserve) StackReserve = StackReserveHeader;

	if (!StackCommit)
	{
		StackCommit = StackCommitHeader;
	}
	else if (StackCommit >= StackReserve)
	{
		StackReserve = ROUND_UP(StackCommit, 1024 * 1024);
	}

	StackCommit = ROUND_UP(StackCommit, PageSize);
	StackReserve = ROUND_UP(StackReserve, AllocationGranularity);

	GuaranteedStackCommit = NtCurrentTeb()->GuaranteedStackBytes;
	if ((GuaranteedStackCommit) && (StackCommit < GuaranteedStackCommit))
	{
		StackCommit = GuaranteedStackCommit;
	}

	if (StackCommit >= StackReserve)
	{
		StackReserve = ROUND_UP(StackCommit, 1024 * 1024);
	}

	StackCommit = ROUND_UP(StackCommit, PageSize);
	StackReserve = ROUND_UP(StackReserve, AllocationGranularity);

	/* ROS Hack until we support guard page stack expansion */
	StackCommit = StackReserve;

	/* Reserve memory for the stack */
	Stack = 0;
	Status = NtAllocateVirtualMemory(NtCurrentProcess(),
		(PVOID*)&Stack,
		0,
		&StackReserve,
		MEM_RESERVE,
		PAGE_READWRITE);
	if (!NT_SUCCESS(Status))
	{
		return Status;
	}

	/* Now set up some basic Initial TEB Parameters */
	InitialTeb->AllocatedStackBase = (PVOID)Stack;
	InitialTeb->StackBase = (PVOID)(Stack + StackReserve);
	InitialTeb->PreviousStackBase = NULL;
	InitialTeb->PreviousStackLimit = NULL;

	/* Update the Stack Position */
	Stack += StackReserve - StackCommit;

	/* Check if we will need a guard page */
	if (StackReserve > StackCommit)
	{
		Stack -= PageSize;
		StackCommit += PageSize;
		UseGuard = TRUE;
	}
	else
	{
		UseGuard = FALSE;
	}

	/* Allocate memory for the stack */
	Status = NtAllocateVirtualMemory(NtCurrentProcess(),
		(PVOID*)&Stack,
		0,
		&StackCommit,
		MEM_COMMIT,
		PAGE_READWRITE);
	if (!NT_SUCCESS(Status))
	{
		GuardPageSize = 0;
		NtFreeVirtualMemory(NtCurrentProcess(), (PVOID*)&Stack, &GuardPageSize, MEM_RELEASE);
		return Status;
	}

	/* Now set the current Stack Limit */
	InitialTeb->StackLimit = (PVOID)Stack;

	/* Create a guard page */
	if (UseGuard)
	{
		/* Set the guard page */
		GuardPageSize = PAGE_SIZE;
		Status = NtProtectVirtualMemory(NtCurrentProcess(),
			(PVOID*)&Stack,
			&GuardPageSize,
			PAGE_GUARD | PAGE_READWRITE,
			&Dummy);
		if (!NT_SUCCESS(Status))
		{
			return Status;
		}

		/* Update the Stack Limit keeping in mind the Guard Page */
		InitialTeb->StackLimit = (PVOID)((ULONG_PTR)InitialTeb->StackLimit +
			GuardPageSize);
	}

	/* We are done! */
	return STATUS_SUCCESS;

}
};
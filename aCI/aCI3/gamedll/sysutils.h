// ==========================================================
// aCI3 project
// 
// Component: gamedll
// Purpose: Wrappers around NT system calls.
//
// Initial author: NTAuthority
// Started: 2013-02-03
// ==========================================================

#pragma once

namespace ci
{

class SystemUtils
{
public:
	// returns whether or not we're running in WOW64
	static inline bool isWOW64()
	{
		return (__readfsdword(192) != 0) ? true : false;
	}
};

class FileUtils
{
public:
	// calls NtCreateFile.
	// returns the handle in the last argument.
	static NTSTATUS createFile(const wchar_t* filename, ACCESS_MASK access, ULONG createDisposition, HANDLE* handle);

	// calls createFile with flags suitable for performing IOCTLs
	static NTSTATUS openDevice(const wchar_t* devicename, HANDLE* handle);

	// performs an IOCTL action on a handle
	static NTSTATUS performIOCTL(HANDLE handle, ULONG controlCode, VOID* inputBuffer, ULONG inLength, VOID* outputBuffer, ULONG* outLength);
};

class RegistryUtils
{
public:
	// calls NtOpenKey
	static NTSTATUS openKey(const wchar_t* key, ACCESS_MASK access, HANDLE* handle);

	// calls NtQueryKey
	static NTSTATUS queryKey(HANDLE handle, const wchar_t* valueName, KEY_VALUE_PARTIAL_INFORMATION** result, size_t* resultLength);
};

typedef DWORD (__cdecl * ThreadFunc)(LPVOID parameter);

class ThreadUtils
{
public:
	DWORD pageSize;
	DWORD allocationGranulatity;

public:
	ThreadUtils();

	static NTSTATUS createThread(ThreadFunc function, LPVOID parameter, PHANDLE handle);

private:
	// creates the stack/TEB for a thread
	static NTSTATUS baseCreateStack(SIZE_T StackReserve, SIZE_T StackCommit, PINITIAL_TEB InitialTeb);

	// creates the initial context for a thread
	static NTSTATUS baseCreateInitialContext(PCONTEXT Context, PVOID Parameter, PVOID StartAddress, PVOID StackAddress, ULONG ContextType);

	static void threadInitStub();

	static void __stdcall threadInitFunc(LPVOID startAddress, PVOID parameter);
};

};
#include "stdinc.h"

typedef NTSTATUS (NTAPI *_NtQueryInformationProcess)(
	HANDLE ProcessHandle,
	DWORD ProcessInformationClass,
	PVOID ProcessInformation,
	DWORD ProcessInformationLength,
	PDWORD ReturnLength
	);

typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaximumLength;
	unsigned short *Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _PROCESS_BASIC_INFORMATION
{
	LONG ExitStatus;
	PVOID PebBaseAddress;
	ULONG_PTR AffinityMask;
	LONG BasePriority;
	ULONG_PTR UniqueProcessId;
	ULONG_PTR ParentProcessId;
} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;

PVOID GetPebAddress(HANDLE ProcessHandle)
{
	char tmp[80];
	HMODULE hMod=GetModuleHandleA(STRdecrypt(GENstr[1],tmp));						 // ntdll.dll
	_NtQueryInformationProcess NtQueryInformationProcess =
		(_NtQueryInformationProcess)GetProcAddress(hMod,STRdecrypt(GENstr[2],tmp));  // NtQueryInformationProcess
	PROCESS_BASIC_INFORMATION pbi;

	NtQueryInformationProcess(ProcessHandle, 0, &pbi, sizeof(pbi), NULL);

	return pbi.PebBaseAddress;
}

char *GetCommandLinePid(int pid)
{
	HANDLE processHandle;
	PVOID pebAddress;
	PVOID rtlUserProcParamsAddress;
	UNICODE_STRING commandLine;
	char *cmdline;

	if ((processHandle = OpenProcess(
		PROCESS_QUERY_INFORMATION | /* required for NtQueryInformationProcess */
		PROCESS_VM_READ, /* required for ReadProcessMemory */
		FALSE, pid)) == 0)
	{
		return NULL;
	}

	pebAddress = GetPebAddress(processHandle);

	/* get the address of ProcessParameters */
	if (!ReadProcessMemory(processHandle, (PCHAR)pebAddress + 0x10,
		&rtlUserProcParamsAddress, sizeof(PVOID), NULL))
	{
		//printf("Could not read the address of ProcessParameters!\n");
		return NULL;
	}

	/* read the CommandLine UNICODE_STRING structure */
	if (!ReadProcessMemory(processHandle, (PCHAR)rtlUserProcParamsAddress + 0x40,
		&commandLine, sizeof(commandLine), NULL))
	{
		//printf("Could not read CommandLine!\n");
		return NULL;
	}

	/* allocate memory to hold the command line */
	cmdline = (char *)malloc(commandLine.Length + 2);
	memset(cmdline, 0, commandLine.Length + 2);

	/* read the command line */
	if (!ReadProcessMemory(processHandle, commandLine.Buffer,
		cmdline, commandLine.Length, NULL))
	{
		//printf("Could not read the command line string!\n");
		return NULL;
	}

	/* print it */
	/* the length specifier is in characters, but commandLine.Length is in bytes */
	/* a WCHAR is 2 bytes */
	//printf("%.*S\n", commandLine.Length / 2, commandLineContents);
	CloseHandle(processHandle);
	s_ustrcpy(cmdline,cmdline,0);			// make it an ascii cmdline
	
	return cmdline;
}
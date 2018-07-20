#include <windows.h>
#include <DbgHelp.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <libnp.h>

typedef struct
{
	void (__cdecl * GetFunctionPointers)(void*** pointers, int* num);
	void (__cdecl * SendStatus)(const char* status);
	void (__cdecl * GetSessionToken)(char* token, uint32_t length);
	void (__cdecl * ImportPacket)(char* buffer, int bufferLen, char* outPacket, int outPacketLen);
} ci_descriptor_t;

typedef void (*_PVFI)(int, const ci_descriptor_t*);

extern "C" __declspec(dllimport) const ci_descriptor_t* a;

LONG WINAPI CustomUnhandledExceptionFilter(LPEXCEPTION_POINTERS ExceptionInfo);

void main()
{
	if (!IsDebuggerPresent())
	{
		SetUnhandledExceptionFilter(&CustomUnhandledExceptionFilter);
	}

	NP_Init();
	//NP_Connect("iw4.prod.fourdeltaone.net", 3025);
	NP_Connect("192.168.178.83", 3025);

	NPAsync<NPAuthenticateResult>* async = NP_AuthenticateWithLicenseKey("");
	async->Wait();

	HMODULE module = LoadLibrary(L"gamedll.dll");
	_PVFI func = (_PVFI)GetProcAddress(module, "a");
	func(0, a);

	while (true)
	{
		Sleep(1);

		NP_RunFrame();
	}
}

LONG WINAPI CustomUnhandledExceptionFilter(LPEXCEPTION_POINTERS ExceptionInfo)
{
	// step 1: write minidump
	char error[1024];
	char filename[MAX_PATH];
	__time64_t time;
	tm* ltime;

	_time64(&time);
	ltime = _localtime64(&time);
	strftime(filename, sizeof(filename) - 1, "aci3-%Y%m%d%H%M%S.dmp", ltime);
	_snprintf(error, sizeof(error) - 1, "A minidump has been written to %s.", filename);

	HANDLE hFile = CreateFileA(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION ex;
		memset(&ex, 0, sizeof(ex));
		ex.ThreadId = GetCurrentThreadId();
		ex.ExceptionPointers = ExceptionInfo;
		ex.ClientPointers = FALSE;

		if (FAILED(MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ex, NULL, NULL)))
		{
			_snprintf(error, sizeof(error) - 1, "An error (0x%x) occurred during writing %s.", GetLastError(), filename);
		}

		CloseHandle(hFile);
	}
	else
	{
		_snprintf(error, sizeof(error) - 1, "An error (0x%x) occurred during creating %s.", GetLastError(), filename);
	}

	// step 2: exit the application
	//Com_Error(0, "Fatal error (0x%08x) at 0x%08x.\n%s", ExceptionInfo->ExceptionRecord->ExceptionCode, ExceptionInfo->ExceptionRecord->ExceptionAddress, error);

	return 0;
}
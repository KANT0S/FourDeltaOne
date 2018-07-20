#include <windows.h>

#define EXE_NAME "BlackOpsMP.exe"
#define DLL_NAME "alterOps.dll"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// find the actual BO executable we should load
	char exePath[MAX_PATH];
	GetModuleFileName(NULL, exePath, MAX_PATH);

	char* dirPointer = strrchr(exePath, '\\');
	dirPointer[1] = '\0';

	strcat(exePath, EXE_NAME);

	// check file existence
	if (GetFileAttributes(exePath) == INVALID_FILE_ATTRIBUTES)
	{
		MessageBox(NULL, "Could not find " EXE_NAME " in the directory this executable is located. Make sure it actually exists, and try again.", "alterOps", MB_ICONERROR | MB_OK);
		return 1;
	}

	// get DLL path
	char dllPath[MAX_PATH];
	GetModuleFileName(NULL, dllPath, MAX_PATH);

	dirPointer = strrchr(dllPath, '\\');
	dirPointer[1] = '\0';

	strcat(dllPath, DLL_NAME);

	// check file existence
	if (GetFileAttributes(dllPath) == INVALID_FILE_ATTRIBUTES)
	{
		MessageBox(NULL, "Could not find " DLL_NAME " in the directory this executable is located. Make sure it actually exists, and try again.", "alterOps", MB_ICONERROR | MB_OK);
		return 1;
	}

	// hm, start the process (in a suspended state, obviously)
	STARTUPINFO startupInfo;
	memset(&startupInfo, 0, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);

	PROCESS_INFORMATION processInfo;
	memset(&processInfo, 0, sizeof(processInfo));

	if (FAILED(CreateProcess(exePath, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &startupInfo, &processInfo)))
	{
		MessageBox(NULL, "Could not start the process.", "alterOps", MB_ICONERROR | MB_OK);
		return 2;
	}

	// allocate room for DLL name in the process
	void* memory = VirtualAllocEx(processInfo.hProcess, NULL, sizeof(dllPath), MEM_COMMIT, PAGE_READWRITE);
	
	if (!memory)
	{
		MessageBox(NULL, "Could not allocate memory in the process.", "alterOps", MB_ICONERROR | MB_OK);
		return 3;
	}

	DWORD written;
	WriteProcessMemory(processInfo.hProcess, memory, dllPath, sizeof(dllPath), &written);

	// create a remote thread for LoadLibrary
	HMODULE kernel32 = GetModuleHandle("kernel32");

	HANDLE thread = CreateRemoteThread(processInfo.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(kernel32, "LoadLibraryA"), memory, 0, NULL);

	if (!thread)
	{
		MessageBox(NULL, "Could not start remote thread.", "alterOps", MB_ICONERROR | MB_OK);
		return 3;
	}

	// wait for the thread to finish executing
	WaitForSingleObject(thread, INFINITE);

	// free remote memory
	VirtualFreeEx(processInfo.hProcess, memory, sizeof(dllPath), MEM_RELEASE);

	// clean up the handle
	CloseHandle(thread);

	// resume base process thread
	ResumeThread(processInfo.hThread);

	// and finally, clean everything else up
	CloseHandle(processInfo.hThread);
	CloseHandle(processInfo.hProcess);

	return 0;
}
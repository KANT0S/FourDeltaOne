// ==========================================================
// T5M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: T5 'shared' functions
//
// Initial author: NTAuthority
// Started: 2013-08-21
// ==========================================================

#include "StdInc.h"
#include "T5TargetFuncs.h"

#include <shellapi.h>

HINSTANCE WINAPI ShellExecuteA_hook(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd)
{
	if (strncmp(lpFile, "steam://", 8))
	{
		return ShellExecuteA(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
	}

	wchar_t currentDirectory[MAX_PATH];
	GetCurrentDirectoryW(sizeof(currentDirectory), currentDirectory);

	const wchar_t* parameters = L"-mp";

	if (!strcmp(lpFile, "steam://run/42700"))
	{
		parameters = L"-sp";
	}

	wchar_t moduleFileName[MAX_PATH];

	GetModuleFileNameW(GetModuleHandle(NULL), moduleFileName, sizeof(moduleFileName));

	return ShellExecuteW(hwnd, L"open", moduleFileName, parameters, currentDirectory, nShowCmd);
}
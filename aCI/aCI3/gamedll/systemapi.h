// ==========================================================
// aCI3 project
// 
// Component: gamedll
// Purpose: Native system API functions.
//
// Initial author: NTAuthority
// Started: 2013-01-05
// ==========================================================

#pragma once
#include "moduleutils.h"

typedef NTSTATUS (__cdecl * PNTFUNC)();

namespace ci
{
	class SystemAPI
	{
	private:
		PNTFUNC systemCallFunc;

	private:
		static NTSTATUS WOW64SystemCall();
		static NTSTATUS sysEnterSystemCall();
		static NTSTATUS int2ESystemCall();

	private:
		void initialize();
		void initializeSystemCalls();
	public:
		static NTSTATUS doSystemCall();

		static void initializeSystemApi();
	};

	class SystemCall
	{
	private:
		void resolveFunctionFromDll(Module* module, const char* name, void* function);
	public:
		SystemCall(const char* name, void* function, bool obfuscated = false);
	};
}
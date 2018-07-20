// ==========================================================
// aCI3 project
// 
// Component: gamedll
// Purpose: DLL handling functions.
//
// Initial author: NTAuthority
// Started: 2013-01-05
// ==========================================================

#pragma once

namespace ci
{
	class Module;

	class ModuleUtils
	{
	public:
		static Module* getModule(const char* moduleName);
	};

	// no fields allowed here apart from the header; the this address will be a module base
	class Module
	{
	private:
		IMAGE_DOS_HEADER header;

		LPVOID getRVA(int offset);
	public:
		LPVOID getProcedure(const char* procName);
	};
}
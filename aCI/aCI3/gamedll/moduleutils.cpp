// ==========================================================
// aCI3 project
// 
// Component: gamedll
// Purpose: DLL handling functions.
//
// Initial author: NTAuthority
// Started: 2013-01-05
// ==========================================================

#include "aci3.h"
#include "moduleutils.h"

namespace ci
{
	Module* ModuleUtils::getModule(const char* moduleName)
	{
		// convert module name to unicode
		wchar_t moduleUnicode[256];
		size_t length;
		StringCbLengthA(moduleName, sizeof(moduleUnicode) / 2, &length);

		for (int i = 0; i < length; i++)
		{
			moduleUnicode[i] = (wchar_t)moduleName[i];
			moduleUnicode[i + 1] = L'\0';
		}

		// get the PEB
		PPEB peb = (PPEB)__readfsdword(0x30);

		for (LDR_DATA_TABLE_ENTRY* entry = (LDR_DATA_TABLE_ENTRY*)peb->Ldr->InLoadOrderModuleList.Flink; entry->InLoadOrderLinks.Flink != (LIST_ENTRY*)&peb->Ldr->InLoadOrderModuleList; entry = (LDR_DATA_TABLE_ENTRY*)entry->InLoadOrderLinks.Flink)
		{
			PUNICODE_STRING entryName = &entry->BaseDllName;

			//if (CompareStringOrdinal(entryName->Buffer, entryName->Length / 2, moduleUnicode, length, TRUE) == CSTR_EQUAL)
			if (StringUtils::equals(entryName->Buffer, moduleUnicode, entryName->Length / 2))
			{
				return (Module*)entry->DllBase;
			}
		}

		return nullptr;
	}

	LPVOID Module::getRVA(int offset)
	{
		return ((char*)&this->header + offset);
	}

	PVOID Module::getProcedure(const char* procName)
	{
		PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)(getRVA(this->header.e_lfanew));

		if (ntHeader->Signature != IMAGE_NT_SIGNATURE)
		{
			return NULL;
		}

		PIMAGE_DATA_DIRECTORY exportDir = &ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
		PIMAGE_EXPORT_DIRECTORY exports = (PIMAGE_EXPORT_DIRECTORY)getRVA(exportDir->VirtualAddress);

		if (exports->NumberOfFunctions == 0 || exports->NumberOfNames == 0)
		{
			return NULL;
		}

		DWORD* namePtr = (DWORD*)getRVA(exports->AddressOfNames);
		WORD* ordinalRef = (WORD*)getRVA(exports->AddressOfNameOrdinals);

		int ordinal = -1;

		for (int i = 0; i < exports->NumberOfNames; i++, ordinalRef++, namePtr++)
		{
			if (StringUtils::equals(procName, (const char*)getRVA(*namePtr), true))
			{
				ordinal = *ordinalRef;
				break;
			}
		}

		if (ordinal == -1)
		{
			return NULL;
		}

		return getRVA(*(DWORD*)(getRVA(exports->AddressOfFunctions + (ordinal * sizeof(DWORD)))));
	}
}
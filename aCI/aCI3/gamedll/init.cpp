// ==========================================================
// aCI3 project
// 
// Component: gamedll
// Purpose: CI initialization code
//
// Initial author: NTAuthority
// Started: 2013-01-05
// ==========================================================

#include "aci3.h"
#include "init.h"
#include "detections.h"

static DWORD obfuscateWChar = (DWORD)((const wchar_t*(*)(const wchar_t*))&ci::StringObfuscation::deobfuscateString);
static DWORD obfuscateChar = (DWORD)((const char*(*)(const char*))&ci::StringObfuscation::deobfuscateString);

namespace ci
{
	int main::_gameID;
	ci_descriptor_t* main::_descriptor;

	bool main::initialize(int gameID, ci_descriptor_t* descriptor)
	{
		if (gameID > GAME_ID_MAX)
		{
			return false;
		}

		_gameID = gameID;
		_descriptor = descriptor;

		// then we go and initialize!
		SystemAPI::initializeSystemApi();

		// this needs syscalls
		antiDebug();

		DetectionManager::initialize();

		return true;
	}

	void main::obfuscateAway()
	{
		VMProtectBeginVirtualization(__FUNCTION__);

		// first we obfuscate away string obfuscation
		DWORD address = obfuscateWChar;
		
		if (_gameID > 0xFFFF0000) // always false, actually
		{
			address ^= 0xFA335806;
		}

		call(address, main::stringObfuscatoryWChar, PATCH_JUMP);

		address = obfuscateChar;

		if (getGameID() > 0xFFFFF000)
		{
			address ^= 0xFA830576;
		}

		call(address, main::stringObfuscatoryChar, PATCH_JUMP);

		VMProtectEnd();
	}

	void main::antiDebug()
	{
		return;

		/*VMProtectBeginVirtualization(__FUNCTION__);

		// ntdll.dll
		Module* module = ModuleUtils::getModule(StringObfuscation::deobfuscateString("\xA1\xBA\xA9\xA0\xA7\xE4\xAD\xA4\xAB"));

		// DbgUiRemoteBreakin
		LPVOID dbgBreakin = module->getProcedure(StringObfuscation::deobfuscateString("\x8B\xAC\xAA\x99\xA2\x98\xAC\xA5\xA8\xB2\xA0\x86\xB1\xA7\xA0\xAB\xD6\xD0"));

		if (dbgBreakin)
		{
			DWORD oldProtect;
			DWORD protectSize = 4;
			LPVOID* ptr = &dbgBreakin;

			NtProtectVirtualMemory(NtCurrentProcess(), ptr, &protectSize, PAGE_EXECUTE_READWRITE, &oldProtect);

			__try
			{
				//patch(dbgBreakin, 0xF8000CC2, 4);
			}
			__except(-1)
			{

			}

			NtProtectVirtualMemory(NtCurrentProcess(), ptr, &protectSize, oldProtect, &oldProtect);
		}

		// hiding the main thread
		NtSetInformationThread(NtCurrentThread(), ThreadHideFromDebugger, 0, 0);

		VMProtectEnd();*/
	}

	char* main::stringObfuscatoryChar(char* string)
	{
		VMProtectBeginMutation(__FUNCTION__);

		static int stringCall;
		static char stringValues[2][2048];

		stringCall++;
		stringCall = (stringCall % 2);

		int num = 0;

		for (const char* i = string; *i != '\0'; i++)
		{
			stringValues[stringCall][num] = (~(*i) & 0xFF) ^ (0x30 + num);

			num++;
		}

		stringValues[stringCall][num] = '\0';

		return stringValues[stringCall];

		VMProtectEnd();
	}

	wchar_t* main::stringObfuscatoryWChar(wchar_t* string)
	{
		VMProtectBeginMutation(__FUNCTION__);

		static int stringCall;
		static wchar_t stringValues[2][2048];

		stringCall++;
		stringCall = (stringCall % 2);

		int num = 0;

		for (const wchar_t* i = string; *i != '\0'; i++)
		{
			stringValues[stringCall][num] = (~(*i) & 0xFF) ^ (0x30 + num);

			num++;
		}

		stringValues[stringCall][num] = '\0';

		return stringValues[stringCall];

		VMProtectEnd();
	}

	int main::getGameID()
	{
		return _gameID;
	}

	ci_descriptor_t* main::getDescriptor()
	{
		return _descriptor;
	}
}
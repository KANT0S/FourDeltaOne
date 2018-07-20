// ==========================================================
// aCI3 project
// 
// Component: gamedll
// Purpose: CI initialization code
//
// Initial author: NTAuthority
// Started: 2013-01-05
// ==========================================================

#pragma once

struct ci_descriptor_t
{
	void (__cdecl * GetFunctionPointers)(void*** pointers, int* num);
	void (__cdecl * SendStatus)(const char* status);
	void (__cdecl * GetSessionToken)(char* token, uint32_t length);
	void (__cdecl * ImportPacket)(char* buffer, int bufferLen, char* outPacket, int outPacketLen);
};

#define GAME_ID_TESTAPP 0
#define GAME_ID_IW4 1
#define GAME_ID_IW5 2

#define GAME_ID_MAX 2

namespace ci
{
	class main
	{
	private:
		static int _gameID;
		static ci_descriptor_t* _descriptor;

		main() {}
		
		static wchar_t* stringObfuscatoryWChar(wchar_t* str);
		static char* stringObfuscatoryChar(char* str);
	public:
		static void obfuscateAway();

		static void antiDebug();

		static bool initialize(int gameID, ci_descriptor_t* descriptor);

		static int getGameID();
		static ci_descriptor_t* getDescriptor();
	};
}
// name change protection
#include "StdInc.h"
#include <process.h>
#include <time.h>

int ci_punishment;
bool didWeDoSomeNaughtyStuff;

static HANDLE g_thread;
static HANDLE g_threadDeux;

void AuthCI_ThreadDeux(void* args);

void AuthCI_Thread(void* args)
{
	Sleep(2000);

	while (true)
	{
		Sleep(50);

		// check #1: Auth.cpp username validation
		const char* username = Auth_GetUsername();
		const char* obUsername = Auth_GetObfuscatedUsername();
		int length = strlen(username);
		bool matches = true;

		char username3[256];

		for (int i = 0; i < length; i++)
		{
			username3[i] = obUsername[i] ^ 0xD3;
			username3[i + 1] = '\0';
		}

		if (_stricmp(username, username3))
		{
			char str[200];
			str[0] = '\x68';
			str[1] = '\x6e';
			str[2] = '\x73';
			str[3] = '\x70';
			str[4] = '\x70';
			str[5] = '\x3c';
			str[6] = '\x28';
			str[7] = '\x2c';
			str[8] = '\x2c';
			str[9] = '\x2c';
			str[10] = '\x2d';
			str[11] = '\0';

			length = strlen(str);

			char str2[200];
			for (int i = 0; i < length; i++)
			{
				str2[i] = str[i] ^ 0x1C;
				str2[i + 1] = '\0';
			}

			NP_SendRandomString(str2);
			return;
		}

		// check #3: quick fix for people patching SendRandomString and other shit
		BYTE* func = (BYTE*)NP_SendRandomString;

		if (func[0] == 0xE8 || func[0] == 0xE9 || *(BYTE*)0x644240 == 0xE9 || *(BYTE*)0x430430 == 0xE9 || *(BYTE*)0x588530 == 0xE9 || didWeDoSomeNaughtyStuff)
		{
			switch (ci_punishment)
			{
				case 0:
					*(BYTE*)0x5CB5268 = 0x1; // MP_BUILDEXPIRED thing
				case 1:
					/**(DWORD*)0x8AF1E4 = 0x80C4FC; // +attack bind reference = +prone
					*(DWORD*)0x8AF1E8 = 0x7F7498; // -attack bind reference = -prone

					*(DWORD*)0x8AF1EC = 0x80C4FC; // +melee bind reference = +prone
					*(DWORD*)0x8AF1F0 = 0x7F7498; // -melee bind reference = -prone

					*(DWORD*)0x8AF264 = 0x80C4FC; // +melee_zoom bind reference = +prone
					*(DWORD*)0x8AF268 = 0x7F7498; // -melee_zoom bind reference = -prone*/

					DWORD* _cmdHandlers = (DWORD*)0x535FDC;
					_cmdHandlers[0] = _cmdHandlers[62]; // +attack -> +prone
					_cmdHandlers[1] = _cmdHandlers[63]; // -attack -> -prone

					_cmdHandlers[2] = _cmdHandlers[62]; // +melee -> +prone
					_cmdHandlers[3] = _cmdHandlers[63]; // -melee -> -prone

					_cmdHandlers[37] = _cmdHandlers[63]; // -forward -> +prone

					// patch CL_CreatePacket called func adding buttons value
					// attack
					*(DWORD*)0x5B0064 = 0x1DC;
					*(DWORD*)0x5B006C = 0x1DD;
					*(DWORD*)0x5B0078 = 0x1DD;

					// melee
					*(DWORD*)0x5B00F2 = 0x1DC;
					*(DWORD*)0x5B00FA = 0x1DD;
					*(DWORD*)0x5B0106 = 0x1DD;
					break;
			}
		}

		if (WaitForSingleObject(g_threadDeux, 0) == WAIT_OBJECT_0)
		{
			g_threadDeux = (HANDLE)_beginthread(AuthCI_ThreadDeux, 0, NULL);
		}
	}
}

void AuthCI_ThreadDeux(void* args)
{
	while (true)
	{
		Sleep(25);

		if (WaitForSingleObject(g_thread, 0) == WAIT_OBJECT_0)
		{
			g_thread = (HANDLE)_beginthread(AuthCI_Thread, 0, NULL);

			// patch CL_CreatePacket called func adding buttons value
			// attack
			*(DWORD*)0x5B0064 = 0x1DC;
			*(DWORD*)0x5B006C = 0x1DD;
			*(DWORD*)0x5B0078 = 0x1DD;

			// melee
			*(DWORD*)0x5B00F2 = 0x1DC;
			*(DWORD*)0x5B00FA = 0x1DD;
			*(DWORD*)0x5B0106 = 0x1DD;
		}


		// check #2: IWSteamClient check
		const char* username = Auth_GetUsername();
		int length;

		char** iwSteamClient = (char**)0x5CCB138;

		if (*iwSteamClient)
		{
			char* name = ((*iwSteamClient) + 322);

			if (_strnicmp(name, username, 128))
			{
				char str[200];
				str[0] = '\x5b';
				str[1] = '\x5d';
				str[2] = '\x40';
				str[3] = '\x43';
				str[4] = '\x43';
				str[5] = '\x0f';
				str[6] = '\x1b';
				str[7] = '\x1f';
				str[8] = '\x1f';
				str[9] = '\x1f';
				str[10] = '\x1d';
				str[11] = '\0';

				length = strlen(str);

				char str2[200];
				for (int i = 0; i < length; i++)
				{
					str2[i] = str[i] ^ 0x2F;
					str2[i + 1] = '\0';
				}

				NP_SendRandomString(str2);
				return;
			}
		}
	}
}

void AuthCI_Init()
{
	srand(time(NULL));
	if ((rand() % 100) < 20)
	{
		ci_punishment = 0;
	}
	else
	{
		ci_punishment = 1;
	}

	g_thread = (HANDLE)_beginthread(AuthCI_Thread, 0, NULL);
	g_threadDeux = (HANDLE)_beginthread(AuthCI_ThreadDeux, 0, NULL);
}
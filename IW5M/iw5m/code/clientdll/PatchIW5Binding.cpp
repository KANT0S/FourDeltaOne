// ==========================================================
// IW5M project
// 
// Component: client
// Sub-component: clientdll
// Purpose: To unbind the bind.
//
// Initial author: NTAuthority 
// Started: 2013-01-24
// ==========================================================

#include "StdInc.h"

char** commandWhiteList = (char**)0x8AF1E0;
int* keybindings = (int*)0xB3A398;
char* commands[512] = {};

int ConvertCommandToIndexCustom(char* command)
{
	for (int i = 0; i < 512; i++)
	{
		if (commands[i] == NULL)
		{
			commands[i] = (char*)malloc(strlen(command) + 1);
			strcpy(commands[i], command);
			return 91 + i;
		}
		else if (strcmp(command, commands[i]) == 0)
		{
			return 91 + i;
		}
	}
}
int ConvertCommandToIndexHookFunc(char *command)
{
	int index = 0;
	while (strcmp(command, commandWhiteList[index]))
	{
		index++;
		if (index >= 91)
		{
			return ConvertCommandToIndexCustom(command);
		}
	}
	return index;
}

DWORD executeCommandForIndexRetn = 0x535546;
int index;
void ExecuteCommandForIndexHookFunc()
{
	if (index >= 91)
	{
		index -= 91;
		if (commands[index] != NULL)
		{
			Cbuf_AddText(0, va("%s\n", commands[index]));
		}
	}
}

void __declspec(naked) ExecuteCommandForIndexHookStub()
{
	__asm mov eax, [esp+8]
	__asm mov index, eax
	ExecuteCommandForIndexHookFunc();

	__asm push ebp
	__asm mov  ebp, [esp+0Ch]
	__asm push esi
	__asm jmp executeCommandForIndexRetn
}

void Key_WriteBindings(int a1, int file)
{
	for (int i = 0; i < 256; i++)
	{
		char* key = Key_KeynumToString(i, 0);
		int value = keybindings[3 * i];
		if (value != 0 && value < 91)
		{
			FS_Printf(file, "bind %s \"%s\"\n", key, commandWhiteList[value]);
		}
		else if (value >= 91)
		{
			value -= 91;
			if (commands[value] != NULL)
			{
				FS_Printf(file, "bind %s \"%s\"\n", key, commands[value]);
			}
		}
	}
}

void PatchIW5_Binding()
{
	call(0x47D300, ConvertCommandToIndexHookFunc, PATCH_JUMP); // string command in, index out - allocate space when they aren't in the whitelist
	call(0x535540, ExecuteCommandForIndexHookStub, PATCH_JUMP); // index in - execute any custom bound commands
	call(0x4F9A10, Key_WriteBindings, PATCH_JUMP); // write all custom binds to the config
}
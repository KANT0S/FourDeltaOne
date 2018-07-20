// ==========================================================
// alterIWnet project
// 
// Component: aiw_client
// Sub-component: steam_api
// Purpose: Modern Warfare 2 patches: asset restrictions and
//          pre-load modifications.
//
// Initial author: NTAuthority
// Started: 2011-05-20
// ==========================================================

#include "StdInc.h"
#include <direct.h>
//#include "Hooking.h"

// TODO: provide generic hooking for these calls
bool AssetRestrict_RestrictFromMaps(int type, const char* name, const char* zone);
void AssetRestrict_PreLoadFromMaps(int type, void* entry, const char* zone);
void AssetRestrict_PreLoadFromExperimental(int type, void* entry, const char* zone);

StompHook dbAddXAssetHook;
DWORD dbAddXAssetHookLoc = 0x5C8C60;
DWORD dbAddXAssetHookRet = 0x5C8C67;

//typedef const char* (__cdecl * DB_GetXAssetNameHandler_t)(void* asset);
//DB_GetXAssetNameHandler_t* DB_GetXAssetNameHandlers = (DB_GetXAssetNameHandler_t*)0x799328;

struct localizedEntry_s
{
	const char* value;
	const char* name;
};

struct stuff_s
{
	const char* unknown;
	const char* name;
};

void AddStringTableEntry(const char* name);

void DoBeforeLoadAsset(int type, void** entry)
{
	/*if (type == 5)
	{
		Material* material = (Material*)(*entry);

		if (material->maps)
		{
			if (material->maps[0].image)
			{
				if (material->maps[0].image->name)
				{
					//OutputDebugStringA(va("'%s' => '%s',\n", material->name, material->maps[0].image->name));
					FILE* materialFile = fopen("raw/materials.txt", "a");
					fprintf(materialFile, "'%s' => '%s',\n", material->name, material->maps[0].image->name);
					fclose(materialFile);
				}
			}
		}

		if (material->animationX > 1 || material->animationY > 1)
		{
			//OutputDebugStringA(va("'%s' => array(%d, %d),", material->name, material->animationX, material->animationY));
		}
	}*/

	if (type == 40)
	{
		//AddStringTableEntry(*(char**)(*entry));
	}
	
	if (type == 4)
	{
		//OutputDebugString(va("model: %s\n", *(char**)(*entry)));
	}

	if (type == 42)
	{
		OutputDebugString(*(char**)(*entry));
		OutputDebugString(" <- .def\n");
	}
	return;
	if (entry)
	{
		if (type == 26)
		{
			_mkdir("raw");

			void* entryPtr = *entry;
			stuff_s* stuff = (stuff_s*)entryPtr;

			FILE* file = fopen("raw/menus.log", "a");
			fprintf(file, "%s\n", stuff->name);
			fclose(file);
		}
	}
	return;

	if (entry)
	{
		if (type == 27)
		{
			_mkdir("raw");
			_mkdir("raw/english/");
			_mkdir("raw/english/localizedstrings/");

			char nameStuff[512];
			char valueStuff[1024];
			localizedEntry_s* ent = (localizedEntry_s*)*entry;
			const char* module = "unknown";

			strcpy_s(nameStuff, sizeof(nameStuff), ent->name);

			char* entryName = strchr(nameStuff, '_');
			entryName[0] = '\0';
			entryName++;

			for (char* pt = nameStuff; *pt; pt++)
			{
				*pt = tolower(*pt);
			}

			char* p2 = &valueStuff[0];
			for (const char* pt = ent->value; *pt; pt++)
			{
				char c = *pt;
				if (c == '\n')
				{
					*p2 = '\\';
					p2++;
					*p2 = 'n';
				}
				else if (c == '"')
				{
					*p2 = '\\';
					p2++;
					*p2 = '"';
				}
				else
				{
					*p2 = c;
				}

				p2++;
			}

			*p2 = '\0';

			FILE* file = fopen(va("raw/english/localizedstrings/%s.str", nameStuff), "a");

			if (file)
			{
				fprintf(file, "REFERENCE           %s\n", entryName);
				fprintf(file, "LANG_ENGLISH        \"%s\"\n\n", valueStuff);
				fclose(file);
			}
		}
	}
}

void __declspec(naked) DB_AddXAssetHookStub()
{
	__asm
	{
		mov eax, [esp + 4]
		mov ecx, [esp + 8]

		push ecx
		push eax
		//call CanWeLoadAsset
		add esp, 08h

		test al, al
		//jz doNotLoad

		mov eax, [esp + 4]
		mov ecx, [esp + 8]
		push ecx
		push eax
		call DoBeforeLoadAsset
		add esp, 08h

		mov eax, [esp + 8]
		sub esp, 10h
		jmp dbAddXAssetHookRet

doNotLoad:
		mov eax, [esp + 8]
		retn
	}
}

void PatchIW5_AssetRestrict()
{
	 dbAddXAssetHook.initialize("a", 7, (PBYTE)dbAddXAssetHookLoc);
     dbAddXAssetHook.installHook(DB_AddXAssetHookStub, true, false);
}
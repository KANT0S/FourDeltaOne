// ==========================================================
// IW5M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: elite clantag display on the scoreboard
//
// Initial author: NTAuthority
// Started: 2012-10-11
// ==========================================================

#include "StdInc.h"
#include "Hooking.h"

typedef struct clientState_s 
{
	int clientNum;
	char pad[120];
	int use_elite_clan_tag;
	int use_elite_clan_title;
	char elite_clan_title_text[24];
	int elite_clan_title_bg;
	char elite_clan_tag_text[8];
} clientState_t;

typedef struct
{
	char blah[209696];
	clientState_t clientState[18];
	char blaah[164];
} cgsnapshot_t;

cgsnapshot_t* cg_snapshots = (cgsnapshot_t*)0x90246C;

const char* AddClanTagToName(int* client, const char* oldName)
{
	static char tagName[32];
	int clientNum = *client;

	// find the clientState_t
	for (int i = 0; i < 18; i++)
	{
		clientState_t* state = &cg_snapshots[0].clientState[i];

		if (state->clientNum == clientNum)
		{
			if (state->use_elite_clan_tag)
			{
				sprintf_s(tagName, "^3[%s]^7%s", state->elite_clan_tag_text, oldName);

				return tagName;
			}
		}
	}

	return oldName;
}

void __declspec(naked) DrawListString_name()
{
	__asm
	{
		push edi // intended name to draw
		push ebp // pointer to clientnum
		call AddClanTagToName
		add esp, 8h

		mov edi, eax // returned pointer

		push 5996A0h // DrawListString itself
		retn
	}
}

void PatchIW5_EliteScores()
{
	call(0x599F9E, DrawListString_name, PATCH_CALL);
	call(0x599FD8, DrawListString_name, PATCH_CALL);
}
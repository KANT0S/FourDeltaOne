// ==========================================================
// IW5M project
// 
// Component: client
// Sub-component: clientdll
// Purpose: The more, well, "experimental" side of things.
//
// Initial author: NTAuthority
// Started: 2012-10-07 (wow, that's late)
// ==========================================================

#include "StdInc.h"

struct pathlink_t
{
	int unknown;
	short nodeId;
	char stuff;
	char moreStuff;
	int evenMoreStuff;
};

struct pathnode_t
{
	int type;
	char stuff[16];
	float origin[3];
	char stuff2[24];
	short linkCount;
	short stuff3;
	pathlink_t* links;
	char stuff4[72];
};

struct PathData
{
	const char* name;
	int nodeCount;
	pathnode_t* nodes;
};

void DoPath_f()
{
	PathData* data = (PathData*)0x18CE628;

	const char* filename = va("raw/%s.nodes", data->name);
	FILE* file = fopen(filename, "w");

	if (!file)
	{
		Com_Printf(0, "Couldn't write %s. Maybe you need to create the directory?\n", filename);
		return;
	}
	
	for (int i = 0; i < data->nodeCount; i++)
	{
		pathnode_t* node = &data->nodes[i];

		if (node->type == 1) // node_pathnode, not any of the thousands of cover types
		{
			fprintf(file, "node %i: %g %g %g\nlinks: %i\n", i, node->origin[0], node->origin[1], node->origin[2], node->linkCount);

			for (int j = 0; j < node->linkCount; j++)
			{
				fprintf(file, "link: %i\n", node->links[j].nodeId);
			}

			fprintf(file, "\n");
		}
	}

	fclose(file);
}

struct MapEnts
{
	const char* wat;
	const char* name;
	const char* ents;
};

void DoEnts_f()
{
	MapEnts* data = (MapEnts*)0x15F4C38;

	char name[256];
	strcpy(name, data->name);
	strrchr(name, '.')[0] = '\0';

	const char* filename = va("raw/%s.mapents", name);
	FILE* file = fopen(filename, "w");

	if (!file)
	{
		Com_Printf(0, "Couldn't write %s. Maybe you need to create the directory?\n", filename);
		return;
	}

	fwrite(data->ents, 1, strlen(data->ents), file);
	fclose(file);
}

struct XZoneInfo {
	const char* name;
	int type1;
	int type2;
};

typedef void (__cdecl * DB_LoadXAssets_t)(XZoneInfo* info, int count, int flags);
DB_LoadXAssets_t DB_LoadXAssets = (DB_LoadXAssets_t)0x458110;

void LoadLevelZoneHookFunc(XZoneInfo* zone, int count, int flags)
{
	XZoneInfo newZone[4];
	memset(newZone, 0, sizeof(newZone));
	memcpy(newZone, zone, sizeof(XZoneInfo) * 2);

	int newCount = count;

	if (_strnicmp(newZone[1].name, "so_", 3) == NULL)
	{
		const char* basename = strchr(&newZone[1].name[3], '_');

		if (!basename)
		{
			Com_Error(0, "Can't find base map for '%s'.", newZone[1].name);
		}

		basename++;

		static char patchBasename[256];
		_snprintf(patchBasename, sizeof(patchBasename), "patch_%s", basename);

		newZone[2].name = patchBasename;
		newZone[2].type1 = newZone[0].type1;
		newZone[2].type2 = newZone[0].type2;

		newZone[3].name = basename;
		newZone[3].type1 = newZone[1].type1;
		newZone[3].type2 = newZone[1].type2;

		newCount += 2;
	}

	DB_LoadXAssets(newZone, newCount, flags);
}

void PatchIW5DS_Experiments()
{
	static cmd_function_s pathCmd;
	static cmd_function_s entsCmd;
	Cmd_AddCommand("path", DoPath_f, &pathCmd);
	Cmd_AddCommand("ents", DoEnts_f, &entsCmd);

	// allow loading unsigned fastfiles
	*(WORD*)0x443C76 = 0x9090;

	// Com_LoadLevelZone: so_ dependencies and friends
	call(0x458E78, LoadLevelZoneHookFunc, PATCH_CALL);

	static_assert(sizeof(pathnode_t) == 136, "pathnode_t must be 136 bytes");
}
#include "StdInc.h"
#include "Save.h"

void Save_ScriptStringList(char*** list, int num)
{
	if (num)
	{
		Save_Allocate(sizeof(char**) * num, *list, (void**)list);

		for (int i = 0; i < num; i++)
		{
			if ((*list)[i])
			{
				Save_XString(&(*list)[i]);
			}
		}

		*list = (char**)-1;
	}
}

void Save_VertexDecl(char* var);
void Save_VertexShader(char* var);
void Save_PixelShader(char* var);

void Save_XAssetEntries(XAssetEntry** assets, int num)
{
	Save_Allocate(sizeof(XAssetEntry) * num, *assets, (void**)assets);

	XAssetEntry* list = *assets;

	for (int i = 0; i < num; i++)
	{
		switch (list[i].type)
		{
			case ASSET_TYPE_TECHSET:
				Save_MaterialTechniqueSet(&list[i].asset);
				break;
			case ASSET_TYPE_RAWFILE:
				Save_RawFile((RawFile**)&list[i].asset);
				break;
			case ASSET_TYPE_MATERIAL:
				Save_Material((Material**)&list[i].asset);
				break;
			case ASSET_TYPE_XMODEL:
				Save_XModel((XModel**)&list[i].asset);
				break;
			case ASSET_TYPE_XANIM:
				Save_XAnim((XAnimParts**)&list[i].asset);
				break;
			case ASSET_TYPE_PHYSPRESET:
				Save_PhysPreset((PhysPreset**)&list[i].asset);
				break;
			case ASSET_TYPE_VERTEXDECL:
				Save_VertexDecl((char*)&list[i].asset);
				break;
			case ASSET_TYPE_VERTEXSHADER:
				Save_VertexShader((char*)&list[i].asset);
				break;
			case ASSET_TYPE_PIXELSHADER:
				Save_PixelShader((char*)&list[i].asset);
				break;
			default:
				Sys_Error("Asset type %s not handled!", g_assetTypes[list[i].type]);
				break;
		}

		list[i].asset = (void*)-1;
	}
}

void Save_XAssetList(XAssetList* list)
{
	XAssetList* outList;
	Save_Allocate(sizeof(XAssetList), list, (void**)&outList);

	Save_ScriptStringList(&outList->scriptStrings, outList->numScriptStrings);

	Save_XAssetEntries(&outList->assets, outList->numAssets);
	outList->assets = (XAssetEntry*)-1;
}

static bool stringsMarked = false;

short Save_ScriptString(const char* string)
{
	// first try finding the string in the map
	for (int i = 0; i < g_save.scriptStringMap.size(); i++)
	{
		if (g_save.scriptStringMap[i] == string)
		{
			return (i + 1);
		}
	}

	short index = g_save.scriptStringMap.size();
	g_save.scriptStringMap.push_back(string);

	return (index + 1);
}

void SL_MarkAssets(XAssetEntry* entries, int numEntries)
{
	for (int i = 0; i < numEntries; i++)
	{
		switch (entries[i].type)
		{
			case ASSET_TYPE_XMODEL:
				SL_MarkXModel((XModel*)entries[i].asset);
				break;
			case ASSET_TYPE_XANIM:
				SL_MarkXAnim((XAnimParts*)entries[i].asset);
				break;
		}
	}

	stringsMarked = true;
}

void XAsset_Rename(assetType_t type, void* asset, const char* newName)
{
	void** assetPtr = (void**)asset;

	switch (type)
	{
		case ASSET_TYPE_TECHSET:
		{
			MaterialTechniqueSet* techset = (MaterialTechniqueSet*)*assetPtr;
			MaterialTechniqueSet* duplicate = new MaterialTechniqueSet;

			memcpy(duplicate, techset, sizeof(MaterialTechniqueSet));

			duplicate->name = newName;

			*assetPtr = duplicate;

			break;
		}
	}
}
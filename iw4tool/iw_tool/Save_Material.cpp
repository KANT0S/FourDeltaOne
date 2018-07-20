#include "StdInc.h"
#include "Save.h"

void Save_GfxImage(GfxImage** var)
{
	char* image = (char*)*var;

	if (!Save_GetPointer(image, (DWORD*)var))
	{
		GfxImage* varImage;
		Save_PushStream(0);
		Save_Allocate(sizeof(GfxImage), image, &varImage);
		Save_PopStream();

		Save_XString(&varImage->name);
		Save_PushStream(0);

		if (varImage->texture)
		{
			char* varTexture;
			Save_Allocate(16, varImage->texture, &varTexture);
			Save_Allocate(*(DWORD*)(varTexture + 12), varImage->texture + 16, &varTexture);

			varImage->texture = (char*)-1;
		}

		Save_PopStream();

		*(DWORD*)var = -2;
	}
}

static const char* materialName;

void Save_MaterialTextureDef(MaterialTextureDef* def)
{
	if (def->unknown2 == 11)
	{
		Sys_Error("Material texture type 11 (water) not supported (%s)!", materialName);
	}

	Save_GfxImage(&def->image);
}

void Save_MaterialTextureDefs(MaterialTextureDef** var, int num)
{
	if (num)
	{
		char* defs = (char*)*var;

		if (!Save_GetPointer(defs, (DWORD*)var))
		{
			MaterialTextureDef* varDefs;
			Save_Allocate(12 * num, defs, &varDefs);

			for (int i = 0; i < num; i++)
			{
				Save_MaterialTextureDef(&varDefs[i]);
			}

			*(DWORD*)var = -1;
		}
	}
}

void Save_Material(Material** var)
{
	char* material = (char*)*var;

	if (!Save_GetPointer(material, (DWORD*)var))
	{
		Material* varMaterial;
		Save_PushStream(0);
		Save_Allocate(sizeof(Material), material, &varMaterial);
		Save_PopStream();

		materialName = varMaterial->name;

		Save_XString(&varMaterial->name);
		Save_MaterialTechniqueSet(&varMaterial->techniqueSet);

		Save_MaterialTextureDefs(&varMaterial->maps, varMaterial->numMaps);

		char* varUnknown;

		if (varMaterial->unknown8)
		{
			if (!Save_GetPointer(varMaterial->unknown8, &varMaterial->unknown8))
			{
				Save_Align(15);
				Save_Allocate(varMaterial->unknown5 * 32, varMaterial->unknown8, &varUnknown);

				varMaterial->unknown8 = (char*)-1;
			}
		}

		if (varMaterial->stateMap)
		{
			if (!Save_GetPointer(varMaterial->stateMap, &varMaterial->stateMap))
			{
				Save_Allocate(varMaterial->unknownCount2 * 8, varMaterial->stateMap, &varUnknown);

				varMaterial->stateMap = (char*)-1;
			}
		}

		*(DWORD*)var = -2;
	}
}
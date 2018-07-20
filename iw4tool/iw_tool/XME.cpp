#include "StdInc.h"

void Material_Copy(Material* source, Material* destination)
{
	memcpy(destination, source, sizeof(Material));

	destination->maps = (MaterialTextureDef*)malloc(sizeof(MaterialTextureDef) * destination->numMaps);
	memcpy(destination->maps, source->maps, sizeof(MaterialTextureDef) * destination->numMaps);
}

struct iwi_header_t
{
	int fourCC;
	char pad[4];
	short type;
	short width;
	short height;
	short depth;
	int fileLength;
	int mipSizes[3];
};

GfxImage* Image_Clone(GfxImage* image, char* newName)
{
	GfxImage* newImage = new GfxImage;
	memcpy(newImage, image, sizeof(GfxImage));

	newImage->texture = new char[16];
	memcpy(newImage->texture, image->texture, 16);

	newImage->texture[12] = 0;
	newImage->texture[13] = 0;
	newImage->texture[14] = 0;
	newImage->texture[15] = 0;

	newName = strdup(newName);
	newImage->name = newName;

	int fh;
	iwi_header_t iwiHeader;

	if (FS_FOpenFileRead((char*)va("images/%s.iwi", newName), &fh, 0))
	{
		FS_Read(&iwiHeader, sizeof(iwiHeader), fh);
		FS_FCloseFile(fh);

		newImage->width = iwiHeader.width;
		newImage->height = iwiHeader.height;
		newImage->depth = iwiHeader.depth;
		newImage->dataLength1 = iwiHeader.fileLength - 32;
		newImage->dataLength2 = iwiHeader.fileLength - 32;
	}

	return newImage;
}

typedef int (__cdecl * _Reader_t)(int, int); // defaulting to int for now
typedef bool (__cdecl * Image_LoadFromFileWithReader_t)(GfxImage* image, _Reader_t reader);
Image_LoadFromFileWithReader_t Image_LoadFromFileWithReader = (Image_LoadFromFileWithReader_t)0x53ABF0;

typedef void (__cdecl * Image_Release_t)(GfxImage* image);
Image_Release_t Image_Release = (Image_Release_t)0x51F010;

// a3 is 'disable data needed for smoothing' or so, maybe 'do not auto-generate mipmaps'?
GfxImage* Image_Alloc(const char* name, char a2, char a3, char a4)
{
	GfxImage* memory = (GfxImage*)malloc(sizeof(GfxImage) + strlen(name) + 1);
	memset(memory, 0, sizeof(GfxImage));

	memory->a2 = a2;
	memory->a3 = a3;
	memory->a4 = a4;
	memory->name = ((char*)memory) + sizeof(GfxImage);
	strcpy(memory->name, name);

	return memory;
}

GfxImage* Image_Load(const char* name)
{
	GfxImage* image = Image_Alloc(name, 3, 0, 0);
	Image_LoadFromFileWithReader(image, (_Reader_t)/*0x42ECA0*/0x46CBF0);
	return image;
}

typedef void (__cdecl * Com_EndParseSession_t)();
Com_EndParseSession_t Com_EndParseSession = (Com_EndParseSession_t)0x4B80B0;

typedef void (__cdecl * Com_BeginParseSession_t)(const char* why);
Com_BeginParseSession_t Com_BeginParseSession = (Com_BeginParseSession_t)0x4AAB80;

unsigned int R_HashString(const char* string)
{
	unsigned int hash = 0;

	while (*string)
	{
		hash = (*string | 0x20) ^ (33 * hash);
		string++;
	}

	return hash;
}

std::unordered_map<std::string, Material*> _customMaterials;

Material* Material_Load(const char* materialName, const char* techsetName)
{
	if (_customMaterials.find(materialName) != _customMaterials.end())
	{
		return _customMaterials[materialName];
	}

	const char* materialFilename = (strnicmp(materialName, "mc/", 3) == NULL) ? &materialName[3] : materialName;

	char* buffer;
	int size = FS_ReadFile(va("materials/%s.txt", materialFilename), &buffer);

	char* startBuffer = buffer;

	Com_BeginParseSession("material");
	
	const char* command = Com_ParseExt(&buffer);

	if (_stricmp(command, "basemat"))
	{
		Sys_Error("%s: first command is not baseMat", materialName);
	}

	const char* value = Com_ParseExt(&buffer);

	Material* material = new Material;
	Material* sourceMat = (Material*)DB_FindXAssetHeader(ASSET_TYPE_MATERIAL, value);
	
	Material_Copy(sourceMat, material);

	material->name = const_cast<char*>(materialName);
	material->techniqueSet = DB_FindXAssetHeader(ASSET_TYPE_TECHSET, techsetName);

	GfxImage* colorImage = NULL;

	while (*command)
	{
		command = Com_ParseExt(&buffer);

		if (!*command)
		{
			break;
		}

		if (!_stricmp(command, "map"))
		{
			const char* type = Com_ParseExt(&buffer);

			if (!*type)
			{
				Sys_Error("%s: end of file in 'map'");
			}

			unsigned int mapType = R_HashString(type);

			const char* index = Com_ParseExt(&buffer);

			if (!*index)
			{
				Sys_Error("%s: end of file in 'map'");
			}

			int indexNum = atoi(index);

			int targetIndex = -1;
			int foundIndex = -1;

			for (int i = 0; i < material->numMaps; i++)
			{
				if (material->maps[i].typeHash == R_HashString("colorMap"))
				{
					colorImage = material->maps[i].image;
				}

				if (material->maps[i].typeHash == mapType)
				{
					foundIndex++;

					if (foundIndex == indexNum)
					{
						targetIndex = i;
						break;
					}
				}
			}

			if (targetIndex == -1)
			{
				Sys_Error("%s: no %s.%i in %s", materialName, type, indexNum, sourceMat->name);
			}

			const char* imageName = Com_ParseExt(&buffer);

			if (!*imageName)
			{
				Sys_Error("%s: end of file in 'map'");
			}

			GfxImage* image = Image_Clone(colorImage, (char*)imageName);
			material->maps[targetIndex].image = image;
		}
		else
		{
			Sys_Error("%s: unknown command %s", materialName, command);
		}
	}

	Com_EndParseSession();
	FS_FreeFile(startBuffer);

	//ak74u->maps[0].image = Image_Clone(ak74u->maps[0].image, "~weapon_ak74u_spec-rgb&weapon~8d98e74a"); //Image_Load("~weapon_mp40_mp_s-r-25g-25b-2~28f2e4de");
	//ak74u->maps[2].image = Image_Clone(ak74u->maps[2].image, "weapon_ak74u_col");

	_customMaterials[materialName] = material;

	return material;
}

typedef short (__cdecl * SL_GetString_t)(const char* string);
SL_GetString_t SL_GetString = (SL_GetString_t)0x48F890;

char* freadstr(FILE* file)
{
	char tempBuf[1024];
	char ch = 0;
	int i = 0;

	do 
	{
		fread(&ch, 1, 1, file);

		tempBuf[i++] = ch;

		if (i >= sizeof(tempBuf))
		{
			throw std::exception("this is wrong");
		}
	} while (ch);

	char* retval = new char[i];
	strcpy(retval, tempBuf);

	return retval;
}

int freadint(FILE* file)
{
	int value;

	fread(&value, sizeof(int), 1, file);

	return value;
}

void XME_LoadCTEntry(XSurfaceCTEntry* entry, FILE* file)
{
	fread(entry->pad, 1, sizeof(entry->pad), file);

	entry->numNode = freadint(file);
	entry->numLeaf = freadint(file);

	entry->node = 0;

	if (entry->numNode > 0)
	{
		entry->node = new char[16 * entry->numNode];
		fread(entry->node, 16, entry->numNode, file);
	}

	entry->leaf = 0;

	if (entry->numLeaf > 0)
	{
		entry->leaf = new short[entry->numLeaf];
		fread(entry->leaf, 2, entry->numLeaf, file);
	}
}

void XME_LoadCT(XSurfaceCT* ct, FILE* file)
{
	ct->pad = freadint(file);
	ct->pad2 = freadint(file);

	ct->entry = new XSurfaceCTEntry;
	XME_LoadCTEntry(ct->entry, file);
}

void XME_LoadSurface(XSurface* surface, FILE* file)
{
	surface->numVertices = freadint(file);
	surface->numPrimitives = freadint(file);
	surface->blendNum1 = freadint(file);
	surface->blendNum2 = freadint(file);
	surface->blendNum3 = freadint(file);
	surface->blendNum4 = freadint(file);

	int blendSize = surface->blendNum1 + (3 * surface->blendNum2) + (5 * surface->blendNum3) + (7 * surface->blendNum4);

	surface->blendInfo = NULL;

	if (blendSize)
	{
		surface->blendInfo = new short[blendSize];
		fread(surface->blendInfo, 2, blendSize, file);
	}

	surface->vertexBuffer = new char[32 * surface->numVertices];
	fread(surface->vertexBuffer, 32, surface->numVertices, file);

	surface->ct = NULL;

	if (freadint(file))
	{
		surface->numCT = freadint(file);
		surface->ct = new XSurfaceCT[surface->numCT];

		for (int i = 0; i < surface->numCT; i++)
		{
			XME_LoadCT(&surface->ct[i], file);
		}
	}
	else
	{
		surface->numCT = 0;
	}

	surface->indexBuffer = new char[6 * surface->numPrimitives];
	fread(surface->indexBuffer, 6, surface->numPrimitives, file);
}

#include "s10e5.h"

struct GfxPackedVertex
{
	float x;
	float y;
	float z;
	float unk; // either 1.0f or -1.0f
	DWORD color;
	union
	{
		struct  
		{
			unsigned short texX;
			unsigned short texY;
		};

		DWORD texCoord;
	};

	DWORD normal;
	DWORD unk2;
};

// *best* function
DWORD __declspec(naked) Vec2PackTexCoords(float* coords)
{
	__asm
	{
		push    ebp
		mov     ebp, esp
		push    ebx
		sub     esp, 10h
		mov     eax, [ebp+8]
		mov     ebx, [eax+4]
		mov     eax, [eax]
		mov     [ebp-8], eax
		mov     eax, [ebp-8]
		mov     edx, eax
		sar     edx, 10h
		and     edx, 0C000h
		lea     eax, [eax+eax-80000000h]
		sar     eax, 0Eh
		cmp     eax, 3FFEh
		jle     loc_1CEA97

		mov     eax, 3FFFh

loc_1CEA5E:
		mov     ecx, edx
		or      ecx, eax
		mov     [ebp-8], ebx
		mov     eax, [ebp-8]
		mov     edx, eax
		sar     edx, 10h
		and     edx, 0C000h
		lea     eax, [eax+eax-80000000h]
		sar     eax, 0Eh
		cmp     eax, 3FFEh
		jle     loc_1CEAA2

		mov     eax, 3FFFh

loc_1CEA89:
		or      edx, eax
		shl     ecx, 10h
		lea     eax, [edx+ecx]
		add     esp, 10h
		pop     ebx
		pop     ebp
		retn


loc_1CEA97:
		cmp     eax, 0FFFFC000h
		jg      loc_1CEAB9

		xor     eax, eax
		jmp     loc_1CEA5E

loc_1CEAB9:
		and     eax, 3FFFh
		jmp     loc_1CEA5E

loc_1CEAA2:
		cmp     eax, 0FFFFC000h
		jg      loc_1CEAC0

		xor     eax, eax
		or      edx, eax
		shl     ecx, 10h
		lea     eax, [edx+ecx]
		add     esp, 10h
		pop     ebx
		pop     ebp
		retn

loc_1CEAC0:
		and     eax, 3FFFh
		jmp     loc_1CEA89

	}
}

void XME_DumpOBJ(XSurface* surface)
{
	GfxPackedVertex* vertices = (GfxPackedVertex*)surface->vertexBuffer;

	for (int i = 0; i < surface->numVertices; i++)
	{
		s10e5 x, y;
		x.setBits(vertices[i].texX);
		y.setBits(vertices[i].texY);
		
		float v[2];
		v[0] = (float)x;
		v[1] = (float)y;

		vertices[i].texCoord = Vec2PackTexCoords(v);
	}
}

void XME_AddTag(XModel* model)
{
	int newBone = (model->numBones - 1);

	for (int i = 0; i < model->numBones; i++)
	{
		if (!strcmp(SL_ConvertToString(model->boneNames[i]), "tag_sights_off"))
		{
			/*float* boneFloats = (float*)(model->boneUnknown3 + (12 * (i - model->numSubBones)));
			boneFloats[0] = 2.0f;
			boneFloats[1] = 2.0f;
			boneFloats[2] = 5.0f;*/

			// i is our base bone to copy data from
			model->boneNames[newBone] = SL_GetString("tag_clip");
			model->boneUnknown1[(newBone - model->numSubBones)] = model->boneUnknown1[i - model->numSubBones];
			
			memcpy(&model->boneUnknown2[(newBone - model->numSubBones) * 8], &model->boneUnknown2[(i - model->numSubBones) * 8], 8); // angles!
			memcpy(&model->boneUnknown3[(newBone - model->numSubBones) * 12], &model->boneUnknown3[(i - model->numSubBones) * 12], 12); // origin!

			model->boneUnknown4[newBone] = model->boneUnknown4[i];
			
			memcpy(&model->boneUnknown5[newBone * 32], &model->boneUnknown5[i * 32], 32);
			memcpy(&model->unknowns[newBone * 28], &model->unknowns[i * 28], 28);

			//model->boneUnknown1[(newBone - model->numSubBones)] = 0;

			float* boneFloats = (float*)(model->boneUnknown3 + (12 * (newBone - model->numSubBones)));
			boneFloats[0] = (3.90f + 1.1f) - 9.15323f;
			boneFloats[1] = 0.00f - 0.0f;
			boneFloats[2] = 3.3f - 4.96271f;

			/*short* boneShorts = (short*)(model->boneUnknown2 + (8 * (newBone - model->numSubBones)));

			float boneAngles[3];
			float angle90 = boneShorts[3];

			boneAngles[0] = (boneShorts[0] / angle90) * 90.0f;
			boneAngles[1] = (boneShorts[1] / angle90) * 90.0f;
			boneAngles[2] = (boneShorts[2] / angle90) * 90.0f;

			// new angles!
			boneAngles[0] = (angle90 / 90.0f) * -10.0f;*/
		}
	}

	
}

XModel* XME_Load(const char* name)
{
	XModel* baseModel = (XModel*)DB_FindXAssetHeader(ASSET_TYPE_XMODEL, "viewmodel_mp5k");
	//XModel* baseModel = (XModel*)DB_FindXAssetHeader(ASSET_TYPE_XMODEL, "mp_body_op_sniper_ghillie_desert");
	XModel* model = new XModel;

	memcpy(model, baseModel, sizeof(XModel));

	model->lods[0].surfaces = new XModelSurfs;
	memcpy(model->lods[0].surfaces, baseModel->lods[0].surfaces, sizeof(XModelSurfs));

	XModelSurfs* newSurfs = model->lods[0].surfaces;
	XSurface* baseSurface = &newSurfs->surfaces[0];

	static int xmeNum = 0;
	xmeNum++;

	newSurfs->name = new char[24];
	sprintf(newSurfs->name, "hello_world%d", xmeNum);
	//newSurfs->name = "hello_world01";

	FILE* file = fopen(va("X:\\aiw\\%s.xme", name), "rb");
	
	model->name = freadstr(file);
	model->numBones = freadint(file);
	model->numSubBones = freadint(file);
	model->numMaterials = freadint(file);
	newSurfs->numSurfaces = model->numMaterials;
	model->lods[0].numSurfacesInLod = model->numMaterials;

	int readBoneCount = 0;

	if (!strcmp(name, "t6_wpn_smg_peacekeeper_view"))
	{
		model->numBones++; // for our fake tag_clip
		
		readBoneCount = 1;
	}

	// bone names
	model->boneNames = new short[model->numBones];

	for (int i = 0; i < (model->numBones - readBoneCount); i++)
	{
		model->boneNames[i] = SL_GetString(freadstr(file));
	}

	// bone unknown 1
	model->boneUnknown1 = new char[model->numBones - model->numSubBones];

	fread(model->boneUnknown1, 1, model->numBones - model->numSubBones - readBoneCount, file);

	// bone unknown 2
	model->boneUnknown2 = new char[8 * (model->numBones - model->numSubBones)];

	fread(model->boneUnknown2, 8, model->numBones - model->numSubBones - readBoneCount, file);

	// bone unknown 3
	model->boneUnknown3 = new char[12 * (model->numBones - model->numSubBones)];

	fread(model->boneUnknown3, 12, model->numBones - model->numSubBones - readBoneCount, file);

	// bone unknown 4
	model->boneUnknown4 = new char[model->numBones];

	fread(model->boneUnknown4, 1, model->numBones - readBoneCount, file);

	// bone unknown 5
	model->boneUnknown5 = new char[model->numBones * 32];

	fread(model->boneUnknown5, 32, model->numBones - readBoneCount, file);

	// surfaces
	newSurfs->surfaces = new XSurface[newSurfs->numSurfaces];

	for (int i = 0; i < model->numMaterials; i++)
	{
		memcpy(&newSurfs->surfaces[i], baseSurface, sizeof(XSurface));
		XME_LoadSurface(&newSurfs->surfaces[i], file);

		if (strstr(name, "peace"))
		{
			XME_DumpOBJ(&newSurfs->surfaces[i]);
		}
	}

	if (!strcmp(name, "t6_wpn_smg_peacekeeper_view"))
	{
		XME_AddTag(model);
	}

	// materials
	model->materials = new Material*[model->numMaterials];

	for (int i = 0; i < model->numMaterials; i++)
	{
		const char* materialName = freadstr(file);
		const char* techsetName = freadstr(file);

		// remove mc/ from material name
		const char* materialFilename = (strnicmp(materialName, "mc/", 3) == NULL) ? &materialName[3] : materialName;

		if (FS_ReadFile(va("materials/%s.txt", materialFilename), NULL) >= 0)
		{
			model->materials[i] = Material_Load(materialName, techsetName);
			continue;
		}

		model->materials[i] = (Material*)DB_FindXAssetHeader(ASSET_TYPE_MATERIAL, materialName);

		/*if (!strcmp(materialName, "mc/mtl_weapon_mp_mp40"))
		{
			static Material* mp40;

			if (!mp40)
			{
				Material* sourceMat = (Material*)DB_FindXAssetHeader(ASSET_TYPE_MATERIAL, "mc/mtl_weapon_ak47");
				mp40 = new Material;

				Material_Copy(sourceMat, mp40);

				mp40->name = "mc/mtl_weapon_mp_mp40";
				mp40->techniqueSet = DB_FindXAssetHeader(ASSET_TYPE_TECHSET, techsetName);

				mp40->maps[0].image = Image_Clone(mp40->maps[0].image, "~weapon_mp40_mp_s-r-25g-25b-2~28f2e4de"); //Image_Load("~weapon_mp40_mp_s-r-25g-25b-2~28f2e4de");
				mp40->maps[2].image = Image_Clone(mp40->maps[2].image, "weapon_mp40_mp_c");

				//mp40->maps[0].image->texture = nullptr;
				//mp40->maps[2].image->texture = nullptr;
			}

			model->materials[i] = mp40;
		}
		else if (!strcmp(materialName, "mc/mtl_weapon_ak74u") || !strcmp(materialName, "mc/mtl_weapon_ak74u_misc_nocamo"))
		{
			static Material* ak74u;
			
			if (!ak74u)
			{
				Material* sourceMat = (Material*)DB_FindXAssetHeader(ASSET_TYPE_MATERIAL, "mc/mtl_weapon_ak47");
				ak74u = new Material;

				Material_Copy(sourceMat, ak74u);

				ak74u->name = "mc/mtl_weapon_ak74u";
				ak74u->techniqueSet = DB_FindXAssetHeader(ASSET_TYPE_TECHSET, techsetName);

				ak74u->maps[0].image = Image_Clone(ak74u->maps[0].image, "~weapon_ak74u_spec-rgb&weapon~8d98e74a"); //Image_Load("~weapon_mp40_mp_s-r-25g-25b-2~28f2e4de");
				ak74u->maps[2].image = Image_Clone(ak74u->maps[2].image, "weapon_ak74u_col");

				//mp40->maps[0].image->texture = nullptr;
				//mp40->maps[2].image->texture = nullptr;
			}

			model->materials[i] = ak74u;
		}
		else 
		{
			model->materials[i] = (Material*)DB_FindXAssetHeader(ASSET_TYPE_MATERIAL, "$default3d");
		}*/
	}

	// collsurfs
	if (freadint(file))
	{
		// collsurfs are still todo
		DebugBreak();
	}

	// bone info
	if (freadint(file))
	{
		model->unknowns = new char[28 * model->numBones];

		fread(model->unknowns, 28, model->numBones - readBoneCount, file);
	}
	else
	{
		DebugBreak();
	}

	fclose(file);

	return model;
}
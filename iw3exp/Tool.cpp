#include "StdInc.h"
#include "Hooking.h"

const char* SL_ConvertToString(int idx)
{
	char* retPtr = *(char**)0x14E8A04;
	retPtr += (idx * 0xC);
	retPtr += 4;

	return retPtr;
}

void fwritestr(FILE* file, const char* str)
{
	while (*str)
	{
		fwrite(str, 1, 1, file);

		str++;
	}

	fwrite(str, 1, 1, file);
}

void fwriteint(FILE* file, int value)
{
	fwrite(&value, 4, 1, file);
}

typedef void* (__cdecl * DB_FindXAssetHeader_t)(int type, const char* name);
DB_FindXAssetHeader_t DB_FindXAssetHeader = (DB_FindXAssetHeader_t)0x489570;

struct XModelCollSurf
{
	char* tri; // element size 48
	int numTri;
	char pad[36];

	void export(FILE* file)
	{
		fwriteint(file, numTri);
		fwrite(tri, 48, numTri, file);

		fwrite(pad, sizeof(pad), 1, file);
	}
};

struct XSurfaceCTEntry
{
	char pad[24];
	int numNode;
	char* node; // el size 16
	int numLeaf;
	short* leaf;

	void export(FILE* file)
	{
		fwrite(pad, 1, sizeof(pad), file);

		fwriteint(file, numNode);
		fwriteint(file, numLeaf);

		fwrite(node, 16, numNode, file);
		fwrite(leaf, 2, numLeaf, file);
	}
};

struct XSurfaceCT
{
	int pad;
	int pad2;
	XSurfaceCTEntry* entry;

	void export(FILE* file)
	{
		fwriteint(file, pad);
		fwriteint(file, pad2);

		entry->export(file);
	}
};

struct XSurface
{
	short pad;
	short numVertices;
	short numPrimitives;
	char streamHandle; // something to do with buffers
	char pad2;
	char pad3[4];
	short* indices;
	short blendNum1;
	short blendNum2;
	short blendNum3;
	short blendNum4;
	char* blendInfo;
	char* vertices; // 32b
	int numCT;
	XSurfaceCT* ct;
	char pad4[16];

	void export(FILE* file)
	{
		fwriteint(file, numVertices);
		fwriteint(file, numPrimitives);
		fwriteint(file, blendNum1);
		fwriteint(file, blendNum2);
		fwriteint(file, blendNum3);
		fwriteint(file, blendNum4);

		fwrite(blendInfo, 2, blendNum1 + (3 * blendNum2) + (5 * blendNum3) + (7 * blendNum4), file);

		fwrite(vertices, 32, numVertices, file);

		fwriteint(file, (ct) ? 1 : 0);

		if (ct)
		{
			fwriteint(file, numCT);

			for (int i = 0; i < numCT; i++)
			{
				ct[i].export(file);
			}
		}

		fwrite(indices, 6, numPrimitives, file);
	}
};

struct XModel
{
	const char* name;
	char numBones;
	char numSubBones;
	char numSurfaces; // also materials
	char pad;
	short* boneNames;
	char* boneUnknown1;
	short* boneUnknown2; // bone angles
	float* boneUnknown3; // bone origins
	char* boneUnknown4;
	char* boneUnknown5;
	XSurface* surfaces;
	char** materials;
	char pad2[112];
	XModelCollSurf* collSurfs;
	int numCollSurfs;
	int pad3;
	char* unknowns; // floats; first 6 + the last match IW4

	void export(FILE* file)
	{
		fwritestr(file, name);
		fwriteint(file, numBones);
		fwriteint(file, numSubBones);
		fwriteint(file, numSurfaces);

		for (int i = 0; i < numBones; i++)
		{
			fwritestr(file, SL_ConvertToString(boneNames[i]));
		}

		fwrite(boneUnknown1, 1, (numBones - numSubBones), file);

		fwrite(boneUnknown2, 8, (numBones - numSubBones), file);

		fwrite(boneUnknown3, 12, (numBones - numSubBones), file);

		fwrite(boneUnknown4, 1, numBones, file);

		fwrite(boneUnknown5, 32, numBones, file);

		for (int i = 0; i < numSurfaces; i++)
		{
			surfaces[i].export(file);
		}

		for (int i = 0; i < numSurfaces; i++)
		{
			const char** materialName = (const char**)materials[i];
			const char*** techsetName = (const char***)(materials[i] + 64);

			fwritestr(file, *materialName);
			fwritestr(file, **techsetName);
		}

		fwriteint(file, (collSurfs) ? 1 : 0);

		if (collSurfs)
		{
			fwriteint(file, numCollSurfs);

			for (int i = 0; i < numCollSurfs; i++)
			{
				collSurfs[i].export(file);
			}
		}

		fwriteint(file, (unknowns) ? 1 : 0);

		if (unknowns)
		{
			for (int i = 0; i < numBones; i++)
			{
				float thisUnknowns[7];
				float* unknown = (float*)&unknowns[40 * i];

				thisUnknowns[0] = unknown[0];
				thisUnknowns[1] = unknown[1];
				thisUnknowns[2] = unknown[2];
				thisUnknowns[3] = unknown[3];
				thisUnknowns[4] = unknown[4];
				thisUnknowns[5] = unknown[5];
				thisUnknowns[6] = unknown[9];

				fwrite(thisUnknowns, sizeof(float), 7, file);
			}
		}


	}
};

struct XAnimTL
{
	short tagName; // script string
	short pad1;
	int pad2;
};

struct XAnimDeltaTransData
{
	char pad[24];
	char* unk;
}; // more data follows struct

struct XAnimDeltaTrans
{
	short deltaBase;
	short deltaBase2;
	union
	{
		float rawData[3];
		XAnimDeltaTransData data;
	};
};

struct XAnimDeltaQuatData
{
	char* data;
}; // more data follows end of struct

struct XAnimDeltaQuat
{
	short deltaBase;
	short deltaBase2;
	union
	{
		int rawData;
		XAnimDeltaQuatData data;
	};
};

struct XAnimDeltaUnkData
{
	char* data;
}; // more data follows end of struct, as usual

struct XAnimDeltaUnk
{
	short deltaBase;
	short deltaBase2;
	union
	{
		__int64 rawData;
		XAnimDeltaUnkData data;
	};
};

struct XAnimDeltaPart
{
	XAnimDeltaTrans* trans;
	XAnimDeltaQuat* quat;
	XAnimDeltaUnk* unk;
};

struct XAnimParts
{
	char* name; // +0
	short numUnk1; // +4
	short numUnk2; // +6
	short numUnk3; // +8
	short numUnk4; // +10
	short numUnk5; // +12
	short numIndices; // +14, it being > 255 changes a lot of stuff to short rather than byte
	char pad[11]; // +16
	char numTags1; // +26
	char numTags2; // +27
	char pad2[3]; // + 29
	int numUnk6; // +32
	char pad3[12]; // +36
	short* tagList; // +48, script strings
	char* unk1; // +52
	char* unk2; // +56
	char* unk3; // +60
	char* unk6; // +64
	char* unk4; // +68
	char* unk5; // +72
	char* indices; // +76
	XAnimTL* tagList2; // +80
	XAnimDeltaPart* deltaParts; // +84

	void export(FILE* file)
	{
		// untested!
		char animThis[88];
		memcpy(animThis, this, 88);
		memcpy(((char*)animThis) + 17, ((char*)animThis) + 18, 13);

		fwrite(animThis, sizeof(XAnimParts), 1, file);

		fwritestr(file, name);

		for (int i = 0; i < numTags1; i++)
		{
			fwritestr(file, SL_ConvertToString(tagList[i]));
		}

		fwriteint(file, (tagList2) ? 1 : 0);

		if (tagList2)
		{
			for (int i = 0; i < numTags2; i++) // NOTE: actually +28, not numTags2!
			{
				fwrite(&tagList2[i], sizeof(XAnimTL), 1, file);

				fwritestr(file, SL_ConvertToString(tagList2[i].tagName));

				//printf("%s: %i - %s\n", name, i, SL_ConvertToString(tagList2[i].tagName));
			}
		}

		if (unk1)
		{
			fwrite(unk1, 1, numUnk1, file);
		}

		if (unk2)
		{
			fwrite(unk2, 2, numUnk2, file);
		}

		if (unk3)
		{
			fwrite(unk3, 4, numUnk3, file);
		}

		if (unk6)
		{
			fwrite(unk6, 2, numUnk6, file);
		}

		if (unk4)
		{
			fwrite(unk4, 1, numUnk4, file);
		}

		if (unk5)
		{
			fwrite(unk5, 4, numUnk5, file);
		}

		if (indices)
		{
			if (numIndices < 256)
			{
				fwrite(indices, 1, numIndices, file); // actually another pad
			}
			else
			{
				fwrite(indices, 2, numIndices, file);
			}
		}

		if (deltaParts)
		{
			DebugBreak(); // not implemented yet
		}
	}
}; // size 88

void XME_Export(const char* name)
{
	XModel* varModel = (XModel*)DB_FindXAssetHeader(3, name);

	char filename[255];
	sprintf(filename, "E:\\aiw\\%s.xme", varModel->name);

	FILE* file = fopen(filename, "wb");
	varModel->export(file);
	fclose(file);
}

void XAE_Export(const char* name)
{
	XAnimParts* varAnim = (XAnimParts*)DB_FindXAssetHeader(2, name);

	char filename[255];
	sprintf(filename, "E:\\aiw\\%s.xae", varAnim->name);

	FILE* file = fopen(filename, "wb");
	varAnim->export(file);
	fclose(file);
}

void RunStuff()
{
	//XModel* varModel = (XModel*)DB_FindXAssetHeader(3, "viewmodel_mp_mp40");
	/*XME_Export("viewmodel_mp_mp40");
	XAE_Export("viewmodel_mp40_idle");
	XAE_Export("viewmodel_mp40_idle_empty");
	XAE_Export("viewmodel_mp40_fire");
	XAE_Export("viewmodel_mp40_lastshot");
	XAE_Export("viewmodel_mp40_reload");
	XAE_Export("viewmodel_mp40_reload_empty");
	XAE_Export("viewmodel_mp40_pullout");
	XAE_Export("viewmodel_mp40_putaway");
	XAE_Export("viewmodel_mp40_pullout_fast");
	XAE_Export("viewmodel_mp40_putaway_fast");
	XAE_Export("viewmodel_mp40_pullout_empty");
	XAE_Export("viewmodel_mp40_putaway_empty");
	XAE_Export("viewmodel_mp40_ADS_up_PC");
	XAE_Export("viewmodel_mp40_ADS_down_PC");

	XME_Export("viewmodel_ak74u_mp");
	XME_Export("weapon_ak74u");
	XAE_Export("viewmodel_ak74u_idle");
	XAE_Export("viewmodel_ak74u_fire");
	XAE_Export("viewmodel_ak74u_reload");
	XAE_Export("viewmodel_ak74u_reload_empty");
	XAE_Export("viewmodel_ak74u_pullout");
	XAE_Export("viewmodel_ak74u_putaway");
	XAE_Export("viewmodel_ak74u_pullout_first");
	XAE_Export("viewmodel_ak74u_pullout_quick");
	XAE_Export("viewmodel_ak74u_putaway_quick");
	XAE_Export("viewmodel_ak74u_sprint_in");
	XAE_Export("viewmodel_ak74u_sprint_loop");
	XAE_Export("viewmodel_ak74u_sprint_out");
	XAE_Export("viewmodel_ak74u_fire_ads");
	XAE_Export("viewmodel_ak74u_ads_up");
	XAE_Export("viewmodel_ak74u_ads_down");
	XAE_Export("viewmodel_ak74u_idle");*/

	/*XAE_Export("viewmodel_m40a3_sprint_loop");

	XME_Export("viewmodel_bo_galil");
	XME_Export("worldmodel_bo_galil");

	XAE_Export("viewmodel_galil_acog_ads_down");
	XAE_Export("viewmodel_galil_acog_ads_up");
	XAE_Export("viewmodel_galil_ads_down");
	XAE_Export("viewmodel_galil_ads_fire");
	XAE_Export("viewmodel_galil_ads_up");
	XAE_Export("viewmodel_galil_dualclip_reload_empty_quick");
	XAE_Export("viewmodel_galil_dualclip_reload_quick");
	XAE_Export("viewmodel_galil_elbit_ads_down");
	XAE_Export("viewmodel_galil_elbit_ads_up");
	XAE_Export("viewmodel_galil_fire");
	XAE_Export("viewmodel_galil_first_raise");
	XAE_Export("viewmodel_galil_idle");
	XAE_Export("viewmodel_galil_idle_loop");
	XAE_Export("viewmodel_galil_ir_ads_down");
	XAE_Export("viewmodel_galil_ir_ads_up");
	XAE_Export("viewmodel_galil_pullout");
	XAE_Export("viewmodel_galil_pullout_quick");
	XAE_Export("viewmodel_galil_putaway");
	XAE_Export("viewmodel_galil_putaway_quick");
	XAE_Export("viewmodel_galil_sprint_in");
	XAE_Export("viewmodel_galil_sprint_loop");
	XAE_Export("viewmodel_galil_sprint_out");*/

	/*XME_Export("viewmodel_mp_ppsh");
	XME_Export("weapon_mp_ppsh_smg");

	XME_Export("viewmodel_usa_kbar_knife");
	XME_Export("weapon_usa_kbar_knife_mp");

	XAE_Export("viewmodel_ppsh_idle");
	XAE_Export("viewmodel_ppsh_idle_empty");
	XAE_Export("viewmodel_ppsh_fire");
	XAE_Export("viewmodel_ppsh_banana_clip_reload");
	XAE_Export("viewmodel_ppsh_banana_clip_reload_empty");
	XAE_Export("viewmodel_ppsh_pullout");
	XAE_Export("viewmodel_ppsh_putaway");
	XAE_Export("viewmodel_ppsh_switch");
	XAE_Export("viewmodel_ppsh_pullout_fast");
	XAE_Export("viewmodel_ppsh_putaway_fast");
	XAE_Export("viewmodel_ppsh_pullout_empty");
	XAE_Export("viewmodel_ppsh_putaway_empty");
	XAE_Export("viewmodel_ppsh_ads_up");
	XAE_Export("viewmodel_ppsh_ads_down");
	XAE_Export("viewmodel_ppsh_lastshot");
	XAE_Export("viewmodel_ppsh_aperture_ads_up");
	XAE_Export("viewmodel_ppsh_aperture_ads_down");
	XAE_Export("viewmodel_ppsh_reload_notempty");
	XAE_Export("viewmodel_ppsh_reload");

	XAE_Export("viewmodel_mp5_sprint_in");
	XAE_Export("viewmodel_mp5_sprint_out");
	XAE_Export("viewmodel_mp5_sprint_loop");*/

	XME_Export("playermodel_dnf_duke");
	XME_Export("viewhands_dnf_duke");

	ExitProcess(0);
}

CallHook runStuffHook;
DWORD runStuffHookLoc = 0x4FF9B1;

void Sys_RunInit()
{
	runStuffHook.initialize(runStuffHookLoc, RunStuff);
	runStuffHook.installHook();

	// strcpy of commandline
	memset((void*)0x57760E, 0x90, 5);

	strcpy((char*)0xCC157E0, "+set dedicated 1 +set fs_game mods/iwexp3");
}
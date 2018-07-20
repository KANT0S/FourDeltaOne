#include "StdInc.h"
#include <conio.h>
#include "optionparser.h"
#include "strtk.hpp"

XModel* XME_Load(const char* name);
XAnimParts* XAE_Load(const char* name);
void SetUseFilePointers(bool use);
void XAsset_Rename(assetType_t type, void* asset, const char* name);

bool useEntryNames = false;

struct XZoneInfo {
	const char* name;
	int type1;
	int type2;
};

typedef void (__cdecl * DB_LoadXAssets_t)(XZoneInfo* data, int count, int unknown);
DB_LoadXAssets_t DB_LoadXAssets = (DB_LoadXAssets_t)0x4E5930;

typedef void* (__cdecl * DB_FindXAssetHeader_t)(int type, const char* filename);
DB_FindXAssetHeader_t DB_FindXAssetHeader = (DB_FindXAssetHeader_t)0x407930;

void Tool_Verify(const char* zone)
{
	*(DWORD*)0x4158F4 = 0x115;
	useEntryNames = true;

	SetUseFilePointers(true);

	XZoneInfo info;
	info.name = zone;
	info.type1 = 3;
	info.type2 = 0;

	DB_LoadXAssets(&info, 1, 1);
}

Material* Material_Load(const char* materialName, const char* techsetName);

#include "zlib.h"

void Tool_Build(const char* zone, option::Option* option)
{
	// load origin fastfiles
	int count = (option) ? option->count() : 0;

	XZoneInfo* info = new XZoneInfo[3 + count];
	info[0].name = "code_post_gfx_mp";
	info[0].type1 = 3;
	info[0].type2 = 0;

	info[1].name = "localized_code_post_gfx_mp";
	info[1].type1 = 3;
	info[1].type2 = 0;

	info[2].name = "common_mp";
	info[2].type1 = 3;
	info[2].type2 = 0;

	int i = 3;

	if (option)
	{
		option = option->first();

		for (; option; option = option->next())
		{
			info[i].name = option->arg;
			info[i].type1 = 3;
			info[i].type2 = 0;

			i++;
		}
	}

	DB_LoadXAssets(info, i, 0);

	// get a list from zone_source
	char csvName[512];
	sprintf_s(csvName, sizeof(csvName), "zone_source/%s.csv", zone);

	if (GetFileAttributes(csvName) == INVALID_FILE_ATTRIBUTES)
	{
		printf("no such file: %s\n", csvName);
		return;
	}

	strtk::token_grid::options options;
	options.set_row_delimiters("\r\n");
	options.set_column_delimiters(",");
	options.set_column_split_option(strtk::split_options::default_mode);
	options.set_row_split_option(strtk::split_options::default_mode);

	strtk::token_grid grid(csvName, options);
	int rows = grid.row_count();

	XAssetEntry* entries = new XAssetEntry[rows];
	int numEntries = 0;

	for (int i = 0; i < rows; i++)
	{
		std::string type = grid.row(i).get<std::string>(0);
		std::string name = grid.row(i).get<std::string>(1);

		// get the asset type name
		int typeId = ASSET_TYPE_MAX - 1;
		for (; typeId >= 0; typeId--)
		{
			if (!_stricmp(g_assetTypes[typeId], type.c_str()))
			{
				break;
			}
		}

		if (typeId < 0)
		{
			printf("ignoring asset type %s\n", type.c_str());
			continue;
		}

		if (typeId == ASSET_TYPE_XMODEL)
		{
			const char* xmeFilename = va("X:\\aiw\\%s.xme", name.c_str());

			if (GetFileAttributes(xmeFilename) != INVALID_FILE_ATTRIBUTES)
			{
				entries[numEntries].type = (assetType_t)typeId;
				entries[numEntries].asset = XME_Load(name.c_str());
				numEntries++;
				continue;
			}
		}
		else if (typeId == ASSET_TYPE_XANIM) // yay code repetition
		{
			const char* xmeFilename = va("X:\\aiw\\%s.xae", name.c_str());

			if (GetFileAttributes(xmeFilename) != INVALID_FILE_ATTRIBUTES)
			{
				entries[numEntries].type = (assetType_t)typeId;
				entries[numEntries].asset = XAE_Load(name.c_str());
				numEntries++;
				continue;
			}
		}
		else if (typeId == ASSET_TYPE_MATERIAL)
		{
			const char* matFilename = va("materials/%s.txt", name.c_str());

			if (FS_ReadFile(matFilename, NULL) > 0)
			{
				entries[numEntries].type = (assetType_t)typeId;
				entries[numEntries].asset = Material_Load(strdup(name.c_str()), "2d");
				numEntries++;

				continue;
			}
		}
		else if (typeId == ASSET_TYPE_RAWFILE)
		{
			const char* rawFilename = name.c_str();
			char* buffer;
			int length;

			if ((length = FS_ReadFile(rawFilename, &buffer)) >= 0)
			{
				char* newBuf = new char[length + 1];
				memcpy(newBuf, buffer, length);
				newBuf[length] = 0;
				FS_FreeFile(buffer);

				char* destBuf = new char[length + 8192];
				uLongf destLen = length + 8192;
				int err = compress2((Bytef*)destBuf, &destLen, (Bytef*)newBuf, length + 1, Z_BEST_COMPRESSION);

				if (err < 0)
				{
					printf("ERROR!!!! %d\n", err);
					ExitProcess(1);
				}

				delete[] newBuf;

				RawFile* rawFile = new RawFile;
				rawFile->compressedSize = destLen;
				rawFile->size = length + 1;
				rawFile->data = destBuf;
				rawFile->name = strdup(name.c_str());

				entries[numEntries].type = ASSET_TYPE_RAWFILE;
				entries[numEntries].asset = rawFile;

				numEntries++;

				continue;
			}
		}

		const char* newName = NULL;

		if (name == "mc_ambient_t0c0_z")
		{
			newName = strdup(name.c_str());
			name = "mc_ambient_t0c0";
		}
		else if (name.find("_nocast") != std::string::npos)
		{
			newName = strdup(name.c_str());
			name = name.replace(name.find("_nocast"), 7, "");
		}
		else if (name == "effect_falloff_screen")
		{
			newName = strdup(name.c_str());
			name = "effect_falloff_add";
		}
		else if (name == "effect_zfeather_falloff_screen")
		{
			newName = strdup(name.c_str());
			name = "effect_zfeather_falloff_add";
		}

		entries[numEntries].type = (assetType_t)typeId;
		entries[numEntries].asset = DB_FindXAssetHeader(typeId, name.c_str());

		if (newName)
		{
			XAsset_Rename(entries[numEntries].type, &entries[numEntries].asset, newName);
		}

		numEntries++;
	}

	Save_LinkFile(zone, entries, numEntries);

	delete[] entries;
}

enum optionIndex
{
	UNKNOWN,
	HELP,
	SOURCE_ZONE,
	VERIFY
};

static const option::Descriptor usage[] =
{
	{ UNKNOWN, 0, "", "", option::Arg::None, "USAGE: iw_tool [options] zone_name\n\n"
										     "Options:" },

	{ HELP, 0, "?", "help", option::Arg::None, "  --help, -?  \tShow usage information." },
	{ SOURCE_ZONE, 0, "s", "source", Arg::Optional, "  --source=oilrig, -soilrig  \tSet an additional source zone to use.\n"
													"\tDefault source zones are (code_post_gfx_mp, common_mp)" },
	{ VERIFY, 0, "v", "verify", option::Arg::None, "  --verify, -v  \tVerifies the loading of zone_name." },
	{ UNKNOWN, 0, "", "", option::Arg::None, "\nExamples:\n"
											 "  iw_tool -sroadkill weap_skorpion\n"
											 "  iw_tool --verify weap_skorpion" }
};

PCHAR* CommandLineToArgvA(PCHAR CmdLine, int* _argc);

void Tool_Handle()
{
	int argc;
	char** argv = CommandLineToArgvA(GetCommandLine(), &argc);

	argc-=(argc>0); argv+=(argc>0);

	option::Stats stats(usage, argc, argv);
	option::Option* options = new option::Option[stats.options_max];
	option::Option* buffer = new option::Option[stats.buffer_max];

	option::Parser parse(usage, argc, argv, options, buffer);
	
	if (parse.error())
	{
		return;
	}

	if (options[HELP] || argc == 0 || parse.nonOptionsCount() != 1)
	{
		option::printUsage(fwrite, stdout, usage);
		return;
	}

	if (options[VERIFY])
	{
		Tool_Verify(parse.nonOption(0));
	}
	else
	{
		Tool_Build(parse.nonOption(0), options[SOURCE_ZONE]);
	}

	delete[] options;
	delete[] buffer;

}

// function definitions
typedef void (__cdecl * Win_InitLocalization_t)(int);
typedef void (__cdecl * SL_Init_t)();
typedef void (__cdecl * Swap_Init_t)();
typedef void (__cdecl * Com_InitHunkMemory_t)();
typedef void (__cdecl * Sys_InitializeCriticalSections_t)();
typedef void (__cdecl * DB_InitThread_t)();
typedef void (__cdecl * Com_InitDvars_t)();
typedef void (__cdecl * PMem_Init_t)();
typedef void (__cdecl * R_RegisterDvars_t)();
typedef void (__cdecl * FS_Init_t)(); // wrong name, but lazy
typedef void (__cdecl * LargeLocalInit_t)(); // guessed name

#ifdef IWTOOL_IW4
SL_Init_t SL_Init = (SL_Init_t)0x4D2280;
Swap_Init_t Swap_Init = (Swap_Init_t)0x47F390;
Com_InitHunkMemory_t Com_InitHunkMemory = (Com_InitHunkMemory_t)0x420830;
Sys_InitializeCriticalSections_t Sys_InitializeCriticalSections = (Sys_InitializeCriticalSections_t)0x42F0A0;
Sys_InitializeCriticalSections_t Sys_InitMainThread = (Sys_InitializeCriticalSections_t)0x4301B0;
DB_InitThread_t DB_InitThread = (DB_InitThread_t)0x4E0FB0;
Com_InitDvars_t Com_InitDvars = (Com_InitDvars_t)0x60AD10;
Win_InitLocalization_t Win_InitLocalization = (Win_InitLocalization_t)0x406D10;
PMem_Init_t PMem_Init = (PMem_Init_t)0x64A020;
R_RegisterDvars_t R_RegisterDvars = (R_RegisterDvars_t)0x5196C0;
FS_Init_t FS_Init = (FS_Init_t)0x429080;
LargeLocalInit_t LargeLocalInit = (LargeLocalInit_t)0x4A62A0;
#elif defined(IWTOOL_IW4C)
SL_Init_t SL_Init = (SL_Init_t)0x4D2280;
Swap_Init_t Swap_Init = (Swap_Init_t)0x47F390;
Com_InitHunkMemory_t Com_InitHunkMemory = (Com_InitHunkMemory_t)0x420830;
Sys_InitializeCriticalSections_t Sys_InitializeCriticalSections = (Sys_InitializeCriticalSections_t)0x42F0A0;
DB_InitThread_t DB_InitThread = (DB_InitThread_t)0x4E0FB0;
Com_InitDvars_t Com_InitDvars = (Com_InitDvars_t)0x60AD10;
DB_LoadXAssets_t DB_LoadXAssets = (DB_LoadXAssets_t)0x4E5930;
Win_InitLocalization_t Win_InitLocalization = (Win_InitLocalization_t)0x406D10;
PMem_Init_t PMem_Init = (PMem_Init_t)0x64A020;
R_RegisterDvars_t R_RegisterDvars = (R_RegisterDvars_t)0x5196C0;
DB_FindXAssetHeader_t DB_FindXAssetHeader = (DB_FindXAssetHeader_t)0x407930;
#endif

unsigned int InstallCallHook(unsigned int callAddress, unsigned int targetFunction)
{
	unsigned int originalTarget = (callAddress + 5) + *(int*)(callAddress + 1);

	int targetOffset = targetFunction - (callAddress + 5);
	*(unsigned char*)callAddress = 0xE8;
	*(unsigned int*)(callAddress + 1) = targetOffset;

	return originalTarget;
}

void Save_Material(void* material);
void Save_MaterialTechniqueSet(void* techset);
void Save_DoThatThing(int count);
void Save_Init(int count);

void Tool_Run()
{
	printf("IWTool v0.1 running...\n");

	printf("Attempting to initialize IW.\n");

	Sys_InitializeCriticalSections();
	Sys_InitMainThread();
	Win_InitLocalization(0);
	SL_Init();
	Swap_Init();
	Com_InitHunkMemory();
	PMem_Init();
	DB_InitThread();
	Com_InitDvars();
	R_RegisterDvars();
	LargeLocalInit();
	FS_Init();

	printf("Initialized IW.\n");

	Tool_Handle();
	ExitProcess(1);

	printf("Loading source zone...\n");

	if (true)
	{
		*(DWORD*)0x4158F4 = 0x115;

		//*(BYTE*)0x4970D0 = 0xCC;
		SetUseFilePointers(true);

		XZoneInfo infox[4];
		infox[0].name = "iw4_wc_shades";
		infox[0].type1 = 3;
		infox[0].type2 = 0;

		infox[1].name = "localized_code_post_gfx_mp";
		infox[1].type1 = 3;
		infox[1].type2 = 0;

		useEntryNames = true;

		DB_LoadXAssets(infox, 1, 0);

		/*XZoneInfo z;
		//z.name = "iw4_wc_shaders";
		z.name = "mp_terminal";
		z.type1 = 3;
		z.type2 = 0;

		DB_LoadXAssets(&z, 1, 0);*/

		//void* techSet = DB_FindXAssetHeader(ASSET_TYPE_TECHSET, "wc_sky");
		void* techSet2 = DB_FindXAssetHeader(ASSET_TYPE_TECHSET, "2d");

		while (true)
		{
			Sleep(1);
		}
	}

	XZoneInfo info[4];
	info[0].name = "code_post_gfx_mp";
	info[0].type1 = 3;
	info[0].type2 = 0;

	info[1].name = "localized_code_post_gfx_mp";
	info[1].type1 = 3;
	info[1].type2 = 0;

	info[2].name = "common_mp";
	info[2].type1 = 3;
	info[2].type2 = 0;

	info[3].name = "mp_nightshift";
	info[3].type1 = 3;
	info[3].type2 = 0;

	DB_LoadXAssets(info, 4, 0);

	printf("Continue as planned.\n");

	const char* techsets[] = {
		"2d",
		"mc_l_sm_r0c0d0n0s0p0",
		"mc_l_sm_r0c0p0",
		"mc_l_sm_r0c0s0p0",
		"mc_l_sm_r0c0n0p0",
		"mc_l_sm_r0c0n0s0p0",
		"mc_l_sm_r0c0d0p0",
		"mc_l_sm_r0c0d0s0p0",
		"mc_l_sm_r0c0d0n0p0",
		"mc_l_hsm_r0c0p0",
		"mc_l_hsm_r0c0s0p0",
		"mc_l_hsm_r0c0n0p0",
		"mc_l_hsm_r0c0n0s0p0",
		"mc_l_hsm_r0c0d0p0",
		"mc_l_hsm_r0c0d0s0p0",
		"mc_l_hsm_r0c0d0n0p0",
		"mc_l_hsm_r0c0d0n0s0p0",
		"mc_l_sm_r0c0d0n0s0",
		"mc_l_sm_r0c0",
		"mc_l_sm_r0c0s0",
		"mc_l_sm_r0c0n0",
		"mc_l_sm_r0c0n0s0",
		"mc_l_sm_r0c0d0",
		"mc_l_sm_r0c0d0s0",
		"mc_l_sm_r0c0d0n0",
		"mc_l_hsm_r0c0",
		"mc_l_hsm_r0c0s0",
		"mc_l_hsm_r0c0n0",
		"mc_l_hsm_r0c0n0s0",
		"mc_l_hsm_r0c0d0",
		"mc_l_hsm_r0c0d0s0",
		"mc_l_hsm_r0c0d0n0",
		"mc_l_hsm_r0c0d0n0s0",
		"mc_l_sm_b0c0n0s0p0",
		"mc_l_sm_b0c0p0",
		"mc_l_sm_b0c0s0p0",
		"mc_l_sm_b0c0n0p0",
		"mc_l_hsm_b0c0p0",
		"mc_l_hsm_b0c0s0p0",
		"mc_l_hsm_b0c0n0p0",
		"mc_l_hsm_b0c0n0s0p0",
		"mc_reflexsight",
		"mc_unlit_replace_lin_ua",
		"mc_l_sm_b0c0n0s0",
		"mc_l_sm_b0c0",
		"mc_l_sm_b0c0s0",
		"mc_l_sm_b0c0n0",
		"mc_l_hsm_b0c0",
		"mc_l_hsm_b0c0s0",
		"mc_l_hsm_b0c0n0",
		"mc_l_hsm_b0c0n0s0",
		"effect_zfeather_add_nofog",
		"effect_add_nofog",
		"distortion_scale_zfeather",
		"distortion_scale",
		"effect_zfeather_blend",
		"effect_blend",
		"effect_screen_nofog",
		"mc_effect_replace",
		"mc_unlit_blend_lin_ua",
		"mc_unlit_replace_lin",
		"mc_unlit_blend_lin",
		"mc_unlit_add_lin",
		"unlit_blend_lin",
		"unlit_add_lin",
		"effect_falloff_screen_nofog",
		"mc_l_sm_t0c0n0",
		"mc_l_sm_t0c0",
		"mc_l_hsm_t0c0",
		"mc_l_hsm_t0c0n0",
		"effect_zfeather_add",
		"effect_add",
		"effect_zfeather_falloff_add_nofog",
		"effect_falloff_add_nofog",
		"effect_zfeather_screen_nofog",
		"wc_l_sm_b0c0",
		"wc_l_hsm_b0c0",
		"wc_unlit_replace_lin",
		"particle_cloud_add",
		"mc_l_sm_t0c0n0s0p0",
		"mc_l_sm_t0c0p0",
		"mc_l_sm_t0c0s0p0",
		"mc_l_sm_t0c0n0p0",
		"mc_l_hsm_t0c0p0",
		"mc_l_hsm_t0c0s0p0",
		"mc_l_hsm_t0c0n0p0",
		"mc_l_hsm_t0c0n0s0p0",
		"effect_zfeather_screen",
		"effect_screen",
		"effect_falloff_blend",
		"mc_l_sm_r0c0q0n0s0p0",
		"mc_l_sm_r0c0q0p0",
		"mc_l_sm_r0c0q0s0p0",
		"mc_l_sm_r0c0q0n0p0",
		"mc_l_hsm_r0c0q0p0",
		"mc_l_hsm_r0c0q0s0p0",
		"mc_l_hsm_r0c0q0n0p0",
		"mc_l_hsm_r0c0q0n0s0p0",
		"mc_l_sm_r0c0q0n0s0",
		"mc_l_sm_r0c0q0",
		"mc_l_sm_r0c0q0s0",
		"mc_l_sm_r0c0q0n0",
		"mc_l_hsm_r0c0q0",
		"mc_l_hsm_r0c0q0s0",
		"mc_l_hsm_r0c0q0n0",
		"mc_l_hsm_r0c0q0n0s0",
		"mc_l_sm_r0c0s0_custom_grenade",
		"mc_l_sm_r0c0_custom_grenade",
		"mc_l_hsm_r0c0_custom_grenade",
		"mc_l_hsm_r0c0s0_custom_grenade",
		"effect_zfeather_add_eyeoffset",
		"effect_add_eyeoffset",
		"effect_zfeather_falloff_blend",
		"mc_l_sm_t0c0s0",
		"mc_l_hsm_t0c0s0",
		"particle_cloud_sparkf_add",
		"effect_zfeather_falloff_screen_nofog",
		"wc_unlit_add_lin",
		"mc_l_sm_t0c0n0s0",
		"mc_l_hsm_t0c0n0s0",
		"mc_effect_blend",
		"particle_cloud",
		"effect_blend_nofog",
		"mc_unlit_add_lin_custom_objective",
		"mc_objective",
		"mc_effect_replace_lin",
		"effect_add_nofog_eyeoffset",
		"mc_l_sm_b0c0d0p0",
		"mc_l_hsm_b0c0d0p0",
		"mc_l_sm_flag_t0c0n0s0",
		"mc_l_sm_flag_t0c0",
		"mc_l_sm_flag_t0c0s0",
		"mc_l_sm_flag_t0c0n0",
		"mc_l_hsm_flag_t0c0",
		"mc_l_hsm_flag_t0c0s0",
		"mc_l_hsm_flag_t0c0n0",
		"mc_l_hsm_flag_t0c0n0s0",
		"mc_l_sm_r0c0n0s0_custom_objective",
		"mc_l_sm_r0c0_custom_objective",
		"mc_l_sm_r0c0s0_custom_objective",
		"mc_l_sm_r0c0n0_custom_objective",
		"mc_l_hsm_r0c0_custom_objective",
		"mc_l_hsm_r0c0s0_custom_objective",
		"mc_l_hsm_r0c0n0_custom_objective",
		"mc_l_hsm_r0c0n0s0_custom_objective",
		"mc_l_sm_t0c0_nocast",
		"mc_l_hsm_t0c0_nocast",
		"mc_unlit_multiply_lin",
		"mc_l_sm_a0c0_nocast",
		"mc_l_hsm_a0c0_nocast",
		"mc_l_sm_r0c0n0s0_nocast",
		"mc_l_sm_r0c0_nocast",
		"mc_l_sm_r0c0s0_nocast",
		"mc_l_sm_r0c0n0_nocast",
		"mc_l_hsm_r0c0_nocast",
		"mc_l_hsm_r0c0s0_nocast",
		"mc_l_hsm_r0c0n0_nocast",
		"mc_l_hsm_r0c0n0s0_nocast",
		"mc_effect_zfeather_falloff_add_nofog_eyeoffset",
		"mc_effect_falloff_add_nofog_eyeoffset",
		"mc_l_sm_b0c0_nocast",
		"mc_l_hsm_b0c0_nocast",
		"mc_l_sm_t0c0n0_nocast",
		"mc_l_hsm_t0c0n0_nocast",
		"mc_l_sm_t0c0q0n0s0",
		"mc_l_sm_t0c0q0",
		"mc_l_sm_t0c0q0s0",
		"mc_l_sm_t0c0q0n0",
		"mc_l_hsm_t0c0q0",
		"mc_l_hsm_t0c0q0s0",
		"mc_l_hsm_t0c0q0n0",
		"mc_l_hsm_t0c0q0n0s0",
		"mc_unlit_replace_lin_nocast",
		"mc_effect_add_nofog",
		"mc_effect_zfeather_falloff_add_lin_nofog",
		"mc_effect_falloff_add_lin_nofog",
		"effect_zfeather_blend_spot_sm",
		"effect_blend_spot_sm",
		"effect_blend_spot_hsm",
		"effect_zfeather_blend_spot_hsm",
		"mc_tools",
		"mc_l_sm_b0c0q0n0s0",
		"mc_l_sm_b0c0q0",
		"mc_l_sm_b0c0q0s0",
		"mc_l_sm_b0c0q0n0",
		"mc_l_hsm_b0c0q0",
		"mc_l_hsm_b0c0q0s0",
		"mc_l_hsm_b0c0q0n0",
		"mc_l_hsm_b0c0q0n0s0",
		"m_l_sm_b0c0n0s0p0",
		"m_l_sm_b0c0p0",
		"m_l_sm_b0c0s0p0",
		"m_l_sm_b0c0n0p0",
		"m_l_hsm_b0c0p0",
		"m_l_hsm_b0c0s0p0",
		"m_l_hsm_b0c0n0p0",
		"m_l_hsm_b0c0n0s0p0",
		"m_l_sm_b0c0q0n0s0",
		"m_l_sm_b0c0",
		"m_l_sm_b0c0s0",
		"m_l_sm_b0c0n0",
		"m_l_sm_b0c0n0s0",
		"m_l_sm_b0c0q0",
		"m_l_sm_b0c0q0s0",
		"m_l_sm_b0c0q0n0",
		"m_l_hsm_b0c0",
		"m_l_hsm_b0c0s0",
		"m_l_hsm_b0c0n0",
		"m_l_hsm_b0c0n0s0",
		"m_l_hsm_b0c0q0",
		"m_l_hsm_b0c0q0s0",
		"m_l_hsm_b0c0q0n0",
		"m_l_hsm_b0c0q0n0s0",
		"wc_l_sm_r0c0n0s0",
		"wc_l_sm_r0c0",
		"wc_l_sm_r0c0s0",
		"wc_l_sm_r0c0n0",
		"wc_l_hsm_r0c0",
		"wc_l_hsm_r0c0s0",
		"wc_l_hsm_r0c0n0",
		"wc_l_hsm_r0c0n0s0",
		"wc_l_sm_r0c0q0n0s0",
		"wc_l_sm_r0c0q0",
		"wc_l_sm_r0c0q0s0",
		"wc_l_sm_r0c0q0n0",
		"wc_l_hsm_r0c0q0",
		"wc_l_hsm_r0c0q0s0",
		"wc_l_hsm_r0c0q0n0",
		"wc_l_hsm_r0c0q0n0s0",
		"wc_l_sm_b0c0n0s0_nocast",
		"wc_l_sm_b0c0_nocast",
		"wc_l_sm_b0c0s0_nocast",
		"wc_l_sm_b0c0n0_nocast",
		"wc_l_hsm_b0c0_nocast",
		"wc_l_hsm_b0c0s0_nocast",
		"wc_l_hsm_b0c0n0_nocast",
		"wc_l_hsm_b0c0n0s0_nocast",
		"wc_unlit_multiply_lin",
		"wc_l_sm_b0c0n0s0",
		"wc_l_sm_b0c0s0",
		"wc_l_sm_b0c0n0",
		"wc_l_hsm_b0c0s0",
		"wc_l_hsm_b0c0n0",
		"wc_l_hsm_b0c0n0s0",
		"wc_l_sm_t0c0n0s0",
		"wc_l_sm_t0c0",
		"wc_l_sm_t0c0s0",
		"wc_l_sm_t0c0n0",
		"wc_l_hsm_t0c0",
		"wc_l_hsm_t0c0s0",
		"wc_l_hsm_t0c0n0",
		"wc_l_hsm_t0c0n0s0",
		"wc_tools",
		"wc_sky",
		"wc_shadowcaster",
		"effect_zfeather_outdoor_blend",
		"effect_outdoor_blend",
		"effect_zfeather_falloff_screen",
		"effect_falloff_screen",
		"effect_zfeather_blend_nofog",
		"effect_zfeather_falloff_add",
		"effect_falloff_add",
		"effect_zfeather_falloff_add_eyeoffset",
		"effect_falloff_add_eyeoffset",
	};

	XAssetEntry* entries = new XAssetEntry[sizeof(techsets) / sizeof(char*)];
	//Save_Init(sizeof(techsets) / sizeof(char*));

	for (int i = 0; i < sizeof(techsets) / sizeof(char*); i++)
	{
		printf("adding %s\n", techsets[i]);

		void* material = DB_FindXAssetHeader(ASSET_TYPE_TECHSET, techsets[i]);
		entries[i].asset = material;
		entries[i].type = ASSET_TYPE_TECHSET;
		//Save_MaterialTechniqueSet(material);
	}

	Save_LinkFile("iw4_wc_shades", entries, sizeof(techsets) / sizeof(char*));

	delete[] entries;

	//Save_DoThatThing(sizeof(techsets) / sizeof(char*));

/*	while (true)
	{
		Sleep(1);
	}*/

	ExitProcess(0);
}

#include "Hooking.h"

CallHook addEntryNameHook;
DWORD addEntryNameHookLoc = 0x5BB697;

void AddEntryNameHookFunc(int type, const char* name)
{
	if (!useEntryNames)
	{
		return;
	}

	char blah[1024];
	_snprintf(blah, 1024, "%d: %s\n", type, name);
	OutputDebugString(blah);
	printf(blah);
}

void __declspec(naked) AddEntryNameHookStub()
{
	__asm
	{
		push ecx
		push eax
		call AddEntryNameHookFunc
		pop eax
		pop ecx
		jmp addEntryNameHook.pOriginal
	}
}

void PatchMW2_FifthInfinity();
void PatchMW2_AssetRestrict();

void TexDataHookFunc(GfxImage* image)
{
	if (image->texture)
	{
		int texSize = 16 + *(int*)(image->texture + 12);
		char* newTexture = new char[texSize];
		memcpy(newTexture, image->texture, texSize);

		image->texture = newTexture;
	}
}

void __declspec(naked) TexDataHookStub()
{
	__asm
	{
		pushad
		push esi
		call TexDataHookFunc
		add esp, 4h
		popad

		push 51F500h
		retn
	}
}

void Sys_RunInit()
{
#ifdef IWTOOL_IW4
	InstallCallHook(0x6BABA1, (DWORD)Tool_Run);

	InstallCallHook(0x4AA88B, (DWORD)printf);

	*(DWORD*)0x1CDE7FC = GetCurrentThreadId();

	// r_loadForRenderer
	*(BYTE*)0x519DDF = 0x0;

	// dirty disk breakpoint
	*(BYTE*)0x4CF7F0 = 0xCC;

	// delay loading of images, disable it
	*(BYTE*)0x51F450 = 0xC3;

	// don't remove the 'texture data' pointer from GfxImage
	memset((void*)0x51F4FA, 0x90, 6);

	static StompHook texDataHook;
	texDataHook.initialize(0x51F4FA, TexDataHookStub);
	texDataHook.installHook();

	// needed for the above to make Image_Release not misinterpret the texture data as a D3D texture
	*(BYTE*)0x51F03D = 0xEB;

	// don't zero out pixel shaders
	memset((void*)0x505AFB, 0x90, 7);

	// don't zero out vertex shaders
	memset((void*)0x505BDB, 0x90, 7);

	// don't memset vertex declarations (not needed?)
	memset((void*)0x51E5CB, 0x90, 5);

	// set fs_basegame to 'm2demo' (will apply before fs_game, unlike the above line)
	*(DWORD*)0x6431D1 = (DWORD)"m2demo";

	// allow loading of IWffu (unsigned) files
	*(BYTE*)0x4158D9 = 0xEB; // main function
	*(WORD*)0x4A1D97 = 0x9090; // DB_AuthLoad_InflateInit

	// basic checks (hash jumps, both normal and playlist)
	*(WORD*)0x5B97A3 = 0x9090;
	*(WORD*)0x5BA493 = 0x9090;

	*(WORD*)0x5B991C = 0x9090;
	*(WORD*)0x5BA60C = 0x9090;

	*(WORD*)0x5B97B4 = 0x9090;
	*(WORD*)0x5BA4A4 = 0x9090;

	// some other, unknown, check
	*(BYTE*)0x5B9912 = 0xB8;
	*(DWORD*)0x5B9913 = 1;

	*(BYTE*)0x5BA602 = 0xB8;
	*(DWORD*)0x5BA603 = 1;

	// something related to image loading
	*(BYTE*)0x54ADB0 = 0xC3;

	// dvar setting function, unknown stuff related to server thread sync
	*(BYTE*)0x647781 = 0xEB;

	// hookie
	addEntryNameHook.initialize(addEntryNameHookLoc, AddEntryNameHookStub);
	addEntryNameHook.installHook();

	PatchMW2_FifthInfinity();
	PatchMW2_AssetRestrict();
#endif
}
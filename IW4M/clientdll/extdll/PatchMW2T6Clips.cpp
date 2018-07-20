// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Stuff to get T6-style clip models on weapons.
//
// Initial author: NTAuthority
// Started: 2013-02-25
// ==========================================================

#include "StdInc.h"

CallHook createWeaponDObjHook;
DWORD createWeaponDObjHookLoc = 0x59C196;

typedef struct DObjModel_s
{
	XModel* model;
	short attachTag;
	short someFlag;
} DObjModel_t;

typedef int (__cdecl * Com_ClientDObjCreate_t)(DObjModel_t* models, int count, int a3, int a4);
Com_ClientDObjCreate_t Com_ClientDObjCreate = (Com_ClientDObjCreate_t)0x430780;

static int weaponDef;
static char*** bg_weaponNames = (char***)0x7BEAA8;

typedef short (__cdecl * GScr_AllocString_t)(const char* string);
GScr_AllocString_t GScr_AllocString = (GScr_AllocString_t)0x4BC450;

int CreateWeaponDObjHookFunc(DObjModel_t* models, int count, int a3, int a4)
{
	static short thisTag;
	static short thisTag2;
	static short thisTag3;
	static short thisTag4;
	static short thisTag5;
	static short thisTag6;

	if (!thisTag)
	{
		thisTag = GScr_AllocString("tag_clip");
		//thisTag2 = GScr_AllocString("tag_ironsight");
		thisTag2 = GScr_AllocString("tag_silencer");
		thisTag3 = GScr_AllocString("tag_eotech");
		thisTag4 = GScr_AllocString("tag_stock");
		thisTag5 = GScr_AllocString("tag_grip");
		thisTag6 = GScr_AllocString("tag_foregrip");
	}

	DObjModel_t newModels[16];
	memcpy(newModels, models, sizeof(DObjModel_t) * count);

	if (!strcmp(*(bg_weaponNames[weaponDef]), "m16_shotgun_xmags_mp"))
	{
		newModels[count].model = (XModel*)DB_FindXAssetHeader(ASSET_TYPE_XMODEL, "t6_attach_mag_peacekeeper_view");
		newModels[count].attachTag = thisTag;//*(short*)0x1AA2EC2; // tag_clip
		newModels[count].someFlag = 0;

		count++;
	}

	if (!strcmp(*(bg_weaponNames[weaponDef]), "hi_mp") || !strcmp(*(bg_weaponNames[weaponDef]), "*hi_mp_banana"))
	{
		newModels[count].model = (XModel*)DB_FindXAssetHeader(ASSET_TYPE_XMODEL, "muz_flashhiderarstrike_ssncm_blk_vmgun");
		newModels[count].attachTag = thisTag2;//*(short*)0x1AA2EC2; // tag_clip
		newModels[count].someFlag = 0;

		count++;

		newModels[count].model = (XModel*)DB_FindXAssetHeader(ASSET_TYPE_XMODEL, "mag_default_ak104ncm_blk_vmgun");
		newModels[count].attachTag = thisTag;//*(short*)0x1AA2EC2; // tag_clip
		newModels[count].someFlag = 0;

		count++;

		newModels[count].model = (XModel*)DB_FindXAssetHeader(ASSET_TYPE_XMODEL, "eot_553_ss_11_vmgun");
		newModels[count].attachTag = thisTag3;//*(short*)0x1AA2EC2; // tag_clip
		newModels[count].someFlag = 0;

		count++;

		newModels[count].model = (XModel*)DB_FindXAssetHeader(ASSET_TYPE_XMODEL, "sto_ar15emod_ss_08_vmgun");
		newModels[count].attachTag = thisTag4;//*(short*)0x1AA2EC2; // tag_clip
		newModels[count].someFlag = 0;

		count++;

		newModels[count].model = (XModel*)DB_FindXAssetHeader(ASSET_TYPE_XMODEL, "gri_default_ak104ncm_blk_vmgun");
		newModels[count].attachTag = thisTag5;//*(short*)0x1AA2EC2; // tag_clip
		newModels[count].someFlag = 0;

		count++;

		newModels[count].model = (XModel*)DB_FindXAssetHeader(ASSET_TYPE_XMODEL, "rai_default_ak104_05_vmgun");
		newModels[count].attachTag = thisTag6;//*(short*)0x1AA2EC2; // tag_clip
		newModels[count].someFlag = 0;

		count++;
	}

	return Com_ClientDObjCreate(newModels, count, a3, a4);
}

void __declspec(naked) CreateWeaponDObjHookStub()
{
	__asm
	{
		mov eax, [esp + 60h + 0Ch] // weapon index offset
		mov weaponDef, eax

		jmp CreateWeaponDObjHookFunc
	}
}

void PatchMW2_T6Clips()
{
	createWeaponDObjHook.initialize(createWeaponDObjHookLoc, CreateWeaponDObjHookStub);
	createWeaponDObjHook.installHook();
}
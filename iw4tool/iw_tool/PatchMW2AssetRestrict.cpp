// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Modern Warfare 2 patches: asset restrictions and
//          pre-load modifications.
//
// Initial author: NTAuthority
// Started: 2011-05-20
// ==========================================================

#include "StdInc.h"
#include "Hooking.h"
#include <unordered_map>

const char* allowedImagery[] =
{
	"~us_army_up_body_a_spc-rgb&us~001a0797",
	"us_army_up_body_a_nml",
	"us_army_up_body_a_col",
	"mc/mtl_us_army_up_body_a",
	"~us_army_low_body_a_spc-rgb&u~a5e5484a",
	"us_army_low_body_a_nml",
	"us_army_low_body_a_col",
	"mc/mtl_us_army_low_body_a",
	"~us_army_assault_spc-rgb&us_a~62cfd2d8",
	"us_army_assault_nml",
	"us_army_assault_col",
	"mc/mtl_us_army_load_out_assault",
	"~us_army_assault_spc-r18g18b1~5673d2d0",
	"mc/mtl_us_army_metal_load_out_assault",
	"mc/mtl_weapon_beretta",
	"us_army_body_assault_a_lod030",
	"us_army_body_assault_a_lod130",
	"us_army_body_assault_a_lod230",
	"us_army_body_assault_a_lod330",
	"mp_body_us_army_assault_a",
	"mc_l_sm_b0c0n0s0p0",
	"mc_l_sm_b0c0s0p0",
	"mc_l_hsm_b0c0s0p0",
	"mc_l_hsm_b0c0n0s0p0",
	"~us_army_headgear_a_spc-rgb&u~47616a01",
	"us_army_headgear_a_nml",
	"us_army_headgear_a_col",
	"mc/mtl_us_army_headgear_a",
	"~us_army_head_a_spc-rgb&us_ar~da9c9fb9",
	"us_army_head_a_nml",
	"us_army_head_a_col",
	"mc/mtl_us_army_head_a",
	"~us_army_alpha_a_spc-rgb&us_a~2ef4b7fb",
	"us_army_alpha_a_nml",
	"us_army_alpha_a_col",
	"mc/mtl_us_army_alpha_a",
	"mc/mtl_us_army_metal_headgear_a",
	"~lightgreen_eye_spc-rgb&light~e199ad83",
	"lightgree_eye_col",
	"mc/mtl_lightgreen_eye",
	"~shiny_lense_spc-rgb&shiny_le~210c19d5",
	"shiny_lense_nml",
	"shiny_lense_col",
	"mc/mtl_shiny_lense",
	"us_army_head_a_lod010",
	"us_army_head_a_lod110",
	"us_army_head_a_lod210",
	"us_army_head_a_lod310",
	"head_us_army_a",
	"~us_army_head_b_spc-rgb&us_ar~5d70f7a7",
	"us_army_head_b_nml",
	"us_army_head_b_col",
	"mc/mtl_us_army_head_b",
	"~us_army_headgear_b_spc-rgb&u~2e9520b5",
	"us_army_headgear_b_nml",
	"us_army_headgear_b_col",
	"mc/mtl_us_army_headgear_b",
	"~lightblue_eye_spc-rgb&lightb~a0b0be4e",
	"lightblue_eye_col",
	"mc/mtl_lightblue_eye",
	"mc/mtl_us_army_metal_headgear_b",
	"~us_army_alpha_b_spc-rgb&us_a~5f69e578",
	"us_army_alpha_b_nml",
	"us_army_alpha_b_col",
	"mc/mtl_us_army_alpha_b",
	"us_army_head_b_lod010",
	"us_army_head_b_lod110",
	"us_army_head_b_lod210",
	"us_army_head_b_lod310",
	"head_us_army_b",
	"~brown_eye_spc-rgb&brown_eye_cos-l-67",
	"brown_eye_col",
	"mc/mtl_brown_eye",
	"~us_army_head_c_spc-rgb&us_ar~d4471d8d",
	"us_army_head_c_nml",
	"us_army_head_c_col",
	"mc/mtl_us_army_head_c",
	"~us_army_headgear_c_spc-rgb&u~defd3c33",
	"us_army_headgear_c_nml",
	"us_army_headgear_c_col",
	"mc/mtl_us_army_headgear_c",
	"mc/mtl_us_army_metal_headgear_c",
	"us_army_head_c_lod010",
	"us_army_head_c_lod110",
	"us_army_head_c_lod210",
	"us_army_head_c_lod310",
	"head_us_army_c",
	"~us_army_head_d_spc-rgb&us_ar~4a1f0932",
	"us_army_head_d_nml",
	"us_army_head_d_col",
	"mc/mtl_us_army_head_d",
	"~us_army_headgear_d_spc-rgb&u~1a38e7d9",
	"us_army_headgear_d_nml",
	"us_army_headgear_d_col",
	"mc/mtl_us_army_headgear_d",
	"mc/mtl_us_army_metal_headgear_d",
	"us_army_head_d_lod010",
	"us_army_head_d_lod110",
	"us_army_head_d_lod210",
	"us_army_head_d_lod310",
	"head_us_army_d",
	"~us_army_headgear_f_spc-rgb&u~1a66cf82",
	"us_army_headgear_f_nml",
	"us_army_headgear_f_col",
	"mc/mtl_us_army_headgear_f",
	"~shad_co_headgear_b_spc-rgb&s~71aad0fb",
	"shad_co_headgear_b_nml",
	"shad_co_headgear_b_col",
	"mc/mtl_shad_co_headgear_b",
	"~shad_co_alpha_b_spc-rgb&shad~fbbc49e0",
	"shad_co_alpha_b_nml",
	"shad_co_alpha_b_col",
	"mc/mtl_shad_co_alpha_b",
	"us_army_head_f_lod010",
	"us_army_head_f_lod110",
	"us_army_head_f_lod210",
	"us_army_head_f_lod310",
	"head_us_army_f",
	"mc/mtl_us_army_glove",
	"mc/mtl_us_army_gps_sleeve",
	"mc/mtl_us_army_alpha",
	"mc/mtl_us_army_metal_gps_sleeve",
	"viewhands_us_army40",
	"viewhands_us_army",
	"us_army_up_body_b_col",
	"mc/mtl_us_army_up_body_b",
	"us_army_low_body_b_col",
	"mc/mtl_us_army_low_body_b",
	"mp_body_us_army_assault_b",
	"mp_body_us_army_assault_c",
	"~us_army_lmg_spc-rgb&us_army_~9da4dacd",
	"us_army_lmg_nml",
	"us_army_lmg_col",
	"mc/mtl_us_army_loadout_lmg",
	"mc/mtl_us_army_metal_loadout_lmg",
	"us_army_body_lmg_lod030",
	"us_army_body_lmg_lod130",
	"us_army_body_lmg_lod230",
	"us_army_body_lmg_lod330",
	"mp_body_us_army_lmg",
	"mp_body_us_army_lmg_b",
	"mp_body_us_army_lmg_c",
	"~us_army_shotgun_spc-rgb&us_a~282bf951",
	"us_army_shotgun_nml",
	"us_army_shotgun_col",
	"mc/mtl_us_army_loadout_shotgun",
	"mc/mtl_us_army_metal_loadout_shotgun",
	"us_army_body_shotgun_lod030",
	"us_army_body_shotgun_lod130",
	"us_army_body_shotgun_lod230",
	"us_army_body_shotgun_lod330",
	"mp_body_us_army_shotgun",
	"mp_body_us_army_shotgun_b",
	"mp_body_us_army_shotgun_c",
	"~us_army_smg_spc-rgb&us_army_~e2cc6c77",
	"us_army_smg_nml",
	"us_army_smg_col",
	"mc/mtl_us_army_loadout_smg",
	"~us_army_smg_alpha_spc-rgb&us~23048253",
	"us_army_smg_alpha_nml",
	"us_army_smg_alpha_col",
	"mc/mtl_us_army_loadout_smg_alpha",
	"mc/mtl_us_army_metal_loadout_smg",
	"us_army_body_smg_lod030",
	"us_army_body_smg_lod130",
	"us_army_body_smg_lod230",
	"us_army_body_smg_lod330",
	"mp_body_us_army_smg",
	"mp_body_us_army_smg_b",
	"mp_body_us_army_smg_c",
	"~sniper_upperbody_spc-rgb&sni~648b34e6",
	"sniper_upperbody_nrml",
	"ally_army_sniper_up_body_col",
	"mc/mtl_ally_army_sniper_upbody",
	"~sniper_loadout_spc-rgb&snipe~611d64f9",
	"sniper_loadout_nrml",
	"ally_army_sniper_loadout_col",
	"mc/mtl_ally_army_sniper_loadout",
	"~sniper_lowerbody_spc-rgb&sni~add46f95",
	"sniper_lowerbody_nrml",
	"ally_army_sniper_low_body_col",
	"mc/mtl_ally_army_sniper_lowbody",
	"~mtl_weapon_colt1911_spec-r18~ad330beb",
	"mtl_weapon_colt1911_col",
	"mc/mtl_weapon_colt1911",
	"opforce_body_sniper_lod030",
	"opforce_body_sniper_lod130",
	"opforce_body_sniper_lod230",
	"opforce_body_sniper_lod330",
	"mp_body_army_sniper",
	"~ally_sniper_head_spc-rgb&all~cf48f091",
	"ally_sniper_head_nrml",
	"ally_sniper_army_head_col",
	"mc/mtl_ally_army_sniper_head",
	"~ally_sniper_hood_spc-rgb&all~474a3899",
	"ally_sniper_hood_nrml",
	"ally_army_sniper_hood_col",
	"mc/mtl_ally_army_sniper_hood",
	"~hazel_eye_spc-rgb&hazel_eye_cos-l-67",
	"eric_eye_col",
	"mc/mtl_eric_eye",
	"ally_sniper_head_lod010",
	"ally_sniper_head_lod110",
	"ally_sniper_head_lod210",
	"ally_sniper_head_lod310",
	"head_allies_us_army_sniper",
	"~view_hands_ghillie_cos-rgb&v~3a723f3c",
	"view_hands_sniper_nml",
	"viewhands_us_army_sniper_col",
	"mc/mtl_sniper_us_army",
	"viewhands_marksman40",
	"viewhands_sniper_us_army",
	"~us_army_riot_shield_spc-rgb&~80436926",
	"us_army_riot_shield_nml",
	"us_army_riot_shield_col",
	"mc/mtl_us_army_loadout_riot_shield",
	"us_army_body_riot_lod030",
	"us_army_body_riot_lod130",
	"us_army_body_riot_lod230",
	"us_army_body_riot_lod330",
	"mp_body_us_army_riot",
	"~us_army_alpha_c_spc-rgb&us_a~7357af62",
	"us_army_alpha_c_nml",
	"us_army_alpha_c_col",
	"mc/mtl_us_army_alpha_c",
	"~us_army_head_e_spc-rgb&us_ar~0fb0df9e",
	"us_army_head_e_nml",
	"us_army_head_e_col",
	"mc/mtl_us_army_head_e",
	"hazel_eye_col",
	"mc/mtl_hazel_eye",
	"~us_army_headgear_e_spc-rgb&u~81b373ca",
	"us_army_headgear_e_nml",
	"us_army_headgear_e_col",
	"mc/mtl_us_army_headgear_e",
	"mc/mtl_us_army_cloth_headgear_e",
	"head_opforce_fsb_b_lod010",
	"head_opforce_fsb_b_lod110",
	"head_opforce_fsb_b_lod210",
	"head_opforce_fsb_b_lod310",
	"head_us_army_e",
	"~viewhands_marine_gloves_spc-~c3211c78",
	"viewhands_marine_gloves_nml",
	"viewhands_marine_gloves_col",
	"mc/mtl_marine_glove",
	"~viewhands_marine_gloves_spc-~ee9961b6",
	"mc/mtl_marine_glovesleeve",
	"viewhands_usmc40",
	"viewhands_usmc",
	"mc/mtl_plastic_case_dummy",
	"faction_128_rangers_crate",
	"mc/mtl_plastic_case_logo_rangers",
	"plastic_case_big_us_dirt_lod011",
	"airdrop_crate_big",
	"com_plasticcase_rangers",
	"mc_l_sm_flag_t0c0n0s0",
	"mc_l_sm_flag_t0c0",
	"mc_l_sm_flag_t0c0s0",
	"mc_l_sm_flag_t0c0n0",
	"mc_l_hsm_flag_t0c0",
	"mc_l_hsm_flag_t0c0s0",
	"mc_l_hsm_flag_t0c0n0",
	"mc_l_hsm_flag_t0c0n0s0",
	"~flag_ranger_spc-rgb&flag_ran~72cc60a4",
	"flag_ranger_nml",
	"flag_ranger_clr",
	"mc/mtl_flag_ranger",
	"flag2_lod000",
	"flag2_lod100",
	"prop_flag_ranger",
	"flag_carry00",
	"prop_flag_ranger_carry",
	"objpoint_flag_rangers",
	"faction_128_rangers",
	"headicon_rangers",
	"faction_128_rangers_fade",
	"~airborne_lower_body_a_spc-rg~99719340",
	"airborne_lower_body_a_nml",
	"airborne_lower_body_a_col",
	"mc/mtl_airborne_low_body_a",
	"~airborne_upper_body_a_spc-rg~328c3b74",
	"airborne_upper_body_a_nml",
	"airborne_upper_body_a_col",
	"mc/mtl_airborne_up_body_cloth_a",
	"~airborne_loadout_assault_spc~b04e71d7",
	"airborne_loadout_assault_nml",
	"airborne_loadout_assault_col",
	"mc/mtl_airborne_assault",
	"~airborne_hands_a_spc-rgb&air~009a51d6",
	"airborne_hands_a_nml",
	"airborne_hands_a_col",
	"mc/mtl_airborne_hands_a",
	"airborne_body_assault_a_lod030",
	"airborne_body_assault_a_lod130",
	"airborne_body_assault_a_lod230",
	"airborne_body_assault_a_lod330",
	"mp_body_airborne_assault_a",
	"~airborne_headgear_a_spc-rgb&~97faa408",
	"airborne_headgear_a_nml",
	"airborne_headgear_a_col",
	"mc/mtl_airborne_headgear_a",
	"~airborne_head_a_spc-rgb&airb~27203c18",
	"airborne_head_a_nml",
	"airborne_head_a_clr",
	"mc/mtl_airborne_head_a",
	"~airborne_alpha_a_spc-rgb&air~a2bbe18e",
	"airborne_alpha_a_nml",
	"airborne_alpha_a_col",
	"mc/mtl_airborne_alpha_a",
	"~airborne_loadout_shotgun_spc~33bb8edd",
	"airborne_loadout_shotgun_nml",
	"airborne_loadout_shotgun_col",
	"mc/mtl_airborne_shotgun",
	"airborne_head_a_lod010",
	"airborne_head_a_lod110",
	"airborne_head_a_lod210",
	"airborne_head_a_lod310",
	"head_airborne_a",
	"~airborne_head_b_spc-rgb&airb~7db81114",
	"airborne_head_b_nml",
	"airborne_head_b_col",
	"mc/mtl_airborne_head_b",
	"~airborne_alpha_b_spc-rgb&air~ad68d29e",
	"airborne_alpha_b_nml",
	"airborne_alpha_b_col",
	"mc/mtl_airborne_alpha_b",
	"~airborne_headgear_b_spc-rgb&~623705c0",
	"airborne_headgear_b_nml",
	"airborne_headgear_b_col",
	"mc/mtl_airborne_headgear_b",
	"airborne_head_b_lod010",
	"airborne_head_b_lod110",
	"airborne_head_b_lod210",
	"airborne_head_b_lod310",
	"head_airborne_b",
	"~airborne_headgear_c_spc-rgb&~0bf2415b",
	"airborne_headgear_c_nml",
	"airborne_headgear_c_col",
	"mc/mtl_airborne_headgear_c",
	"mc/mtl_airborne_headgear_ghillie_c",
	"mc/mtl_airborne_headgear_lens_c",
	"~airborne_head_c_spc-rgb&airb~b1633430",
	"airborne_head_c_nml",
	"airborne_head_c_clr",
	"mc/mtl_airborne_head_c",
	"airborne_head_c_lod010",
	"airborne_head_c_lod110",
	"airborne_head_c_lod210",
	"airborne_head_c_lod310",
	"head_airborne_c",
	"~airborne_head_d_spc-rgb&airb~502f2221",
	"airborne_head_d_nml",
	"airborne_head_d_clr",
	"mc/mtl_airborne_head_d",
	"airborne_head_d_lod010",
	"airborne_head_d_lod110",
	"airborne_head_d_lod210",
	"airborne_head_d_lod310",
	"head_airborne_d",
	"~airborne_head_e_spc-rgb&airb~502f2221",
	"airborne_head_e_nml",
	"airborne_head_e_clr",
	"mc/mtl_airborne_head_e",
	"airborne_head_e_lod010",
	"airborne_head_e_lod110",
	"airborne_head_e_lod210",
	"airborne_head_e_lod310",
	"head_airborne_e",
	"~russian_airborne_glove_spc-r~a6b388ce",
	"russian_airborne_glove_nml",
	"russian_airborne_glove_col",
	"mc/mtl_russian_airborne_glove",
	"~russian_airborne_sleeve_spc-~90092c8c",
	"russian_airborne_sleeve_nml",
	"russian_airborne_sleeve_col",
	"mc/mtl_russian_airborne_sleeve",
	"viewhands_russian_airborne40",
	"viewhands_russian_airborne",
	"airborne_lower_body_b_col",
	"mc/mtl_airborne_low_body_b",
	"airborne_upper_body_b_col",
	"mc/mtl_airborne_up_body_cloth_b",
	"mp_body_airborne_assault_b",
	"mp_body_airborne_assault_c",
	"~airborne_loadout_lmg_spc-rgb~5c294308",
	"airborne_loadout_lmg_nml",
	"airborne_loadout_lmg_col",
	"mc/mtl_airborne_lmg",
	"airborne_body_lmg_lod030",
	"airborne_body_lmg_lod130",
	"airborne_body_lmg_lod230",
	"airborne_body_lmg_lod330",
	"mp_body_airborne_lmg",
	"mp_body_airborne_lmg_b",
	"mp_body_airborne_lmg_c",
	"airborne_body_shotgun_lod030",
	"airborne_body_shotgun_lod130",
	"airborne_body_shotgun_lod230",
	"airborne_body_shotgun_lod330",
	"mp_body_airborne_shotgun",
	"mp_body_airborne_shotgun_b",
	"mp_body_airborne_shotgun_c",
	"~airborne_loadout_smg_spc-rgb~ba61347c",
	"airborne_loadout_smg_nml",
	"airborne_loadout_smg_col",
	"mc/mtl_airborne_smg",
	"airborne_body_smg_lod030",
	"airborne_body_smg_lod130",
	"airborne_body_smg_lod230",
	"airborne_body_smg_lod330",
	"mp_body_airborne_smg",
	"mp_body_airborne_smg_b",
	"mp_body_airborne_smg_c",
	"opforce_sniper_upperbody_nml",
	"op_airborne_up_body_col",
	"mc/mtl_op_airborne_sniper_upbody",
	"op_airborne_sniper_loadout_col",
	"mc/mtl_op_airborne_sniper_loadout",
	"op_airborne_sniper_low_body_col",
	"mc/mtl_op_airborne_sniper_lowbody",
	"mp_body_op_airborne_sniper",
	"~price_eye_spc-rgb&price_eye_cos-l-67",
	"price_eye_col",
	"mc/mtl_price_eye",
	"~sniper_head_spc-rgb&sniper_h~0748b739",
	"sniper_head_nrml",
	"op_sniper_airborne_head_col",
	"mc/mtl_op_airborne_sniper_head",
	"opforce_head_sniper_lod010",
	"opforce_head_sniper_lod110",
	"opforce_head_sniper_lod210",
	"opforce_head_sniper_lod310",
	"head_op_airborne_sniper",
	"viewhands_op_airborne_sniper_col",
	"mc/mtl_sniper_op_airborne",
	"viewhands_sniper_op_airborne",
	"riot_up_body_camo_red_col",
	"mc/mtl_up_body_op_airborne_riot",
	"riot_low_body_camo_red_col",
	"mc/mtl_low_body_op_airborne_riot",
	"riot_load_outs_black_b_col",
	"mc/mtl_loadout_riot_black_b",
	"mp_body_riot_op_airborne",
	"riot_headgear_airborne_col",
	"mc/mtl_headgear_op_airborne_riot",
	"head_riot_op_airborne",
	"faction_128_ussr_crate",
	"mc/mtl_plastic_case_logo_ussr",
	"com_plasticcase_ussr",
	"~flag_speznas_spc-rgb&flag_sp~96c9931d",
	"flag_speznas_nml",
	"flag_speznas_clr",
	"mc/mtl_flag_speznas",
	"prop_flag_speznas",
	"prop_flag_speznas_carry",
	"objpoint_flag_ussr",
	"faction_128_ussr",
	"headicon_ussr",
	"faction_128_ussr_fade",
	"~ghillie_sniper_upperbody_spc~91ce7a87",
	"ally_tf141_urban_ghillie_up_body_col",
	"mc/mtl_ally_tf141_urban_ghillie_upbody",
	"~ghillie_sniper_lower_body_sp~e9f90f80",
	"ally_tf141_urban_ghillie_low_body_col",
	"mc/mtl_ally_tf141_urban_ghillie_lowbody",
	"~sniper_ghillie_core_spc-rgb&~4a0a272a",
	"sniper_ghillie_core_nml",
	"sniper_ghillie_urban_col",
	"mc/mtl_sniper_ghillie_urban",
	"opforce_body_sniper_ghillie_lod030",
	"opforce_body_sniper_ghillie_lod130",
	"opforce_body_sniper_ghillie_lod230",
	"opforce_body_sniper_ghillie_lod330",
	"mp_body_ally_sniper_ghillie_urban",
	"head_allies_sniper_ghillie_urban",
	"~view_hands_ghillie_spc-rgb&v~3a723f3c",
	"view_hands_ghillie_urban_col",
	"mc/mtl_hands_ghillie_urban",
	"viewhands_marine_sniper40",
	"viewhands_ghillie_urban",
	"mp_body_op_sniper_ghillie_urban",
	"op_ghillie_urban_head_col",
	"mc/mtl_op_ghille_urban_head",
	"opforce_head_sniper_ghillie_lod010",
	"opforce_head_sniper_ghillie_lod110",
	"opforce_head_sniper_ghillie_lod210",
	"opforce_head_sniper_ghillie_lod310",
	"head_op_sniper_ghillie_urban",
	0
};

std::unordered_map<std::string, bool> _allowedAssetMap;

void InitializeImageLoad()
{
	for (const char** imageName = allowedImagery; *imageName != NULL; imageName++)
	{
		_allowedAssetMap[*imageName] = true;
	}
}


#define CURRENT_ZONE_NAME (char*)(*(DWORD*)0x112A680 + 4)

static std::unordered_map<DWORD, bool> deadAssets;

// TODO: provide generic hooking for these calls
bool AssetRestrict_RestrictFromMaps(assetType_t type, const char* name, const char* zone)
{
	/*if (!_stricmp(zone, "mp_terminal"))
	{
		// don't load other maps
		if (type == ASSET_TYPE_GAME_MAP_MP || type == ASSET_TYPE_COL_MAP_MP || type == ASSET_TYPE_GFX_MAP || type == ASSET_TYPE_MAP_ENTS || type == ASSET_TYPE_COM_MAP || type == ASSET_TYPE_FX_MAP)
		{
			return true;
		}

		// also don't load localize/fx
		if (type == ASSET_TYPE_LOCALIZE/* || type == ASSET_TYPE_FX*//*) // we need to link 'fx' assets as otherwise we
			// crash at Mark_FxEffectDefAsset...
			// guess rule #1 needs to be expanded:
			// so rule #2 becomes 'don't touch fastfiles through code
			// if you do not understand the code'.
			// rule #1 still stands: 'don't touch fastfiles'
		{
			return true;
		}

		// and don't load * images (GfxWorld internal)
		if (type == ASSET_TYPE_IMAGE && name[0] == '*')
		{
			return true;
		}

		// don't load external bad images
		if (type != ASSET_TYPE_VERTEXSHADER && type != ASSET_TYPE_PIXELSHADER && type != ASSET_TYPE_VERTEXDECL && type != ASSET_TYPE_TECHSET &&
			type != ASSET_TYPE_XANIM)
		{
			if (_allowedAssetMap.find(name) == _allowedAssetMap.end())
			{
				return true;
			}
		}
	}*/

	if (type == ASSET_TYPE_WEAPON && strcmp(name, "defaultweapon_mp"))
	{
		return true;
	}

	return false;
}

void AssetRestrict_PreLoadFromMaps(assetType_t type, void* entry, const char* zone)
{

}

void AssetRestrict_PreLoadFromExperimental(assetType_t type, void* entry, const char* zone)
{

}

StompHook dbAddXAssetHook;
DWORD dbAddXAssetHookLoc = 0x5BB650;
DWORD dbAddXAssetHookRet = 0x5BB657;

typedef const char* (__cdecl * DB_GetXAssetNameHandler_t)(void* asset);
DB_GetXAssetNameHandler_t* DB_GetXAssetNameHandlers = (DB_GetXAssetNameHandler_t*)0x799328;

char CanWeLoadAsset(assetType_t type, void* entry)
{
	const char* name = DB_GetXAssetNameHandlers[type](entry);
	if (type == ASSET_TYPE_WEAPON)
	{
		// somewhat-workaround for issue 'could not load weapon "destructible_car"' and cars not doing any damage
		if (strcmp(CURRENT_ZONE_NAME, "common_tc_mp") && (!strcmp(name, "none") || !strcmp(name, "destructible_car"))) // common_tc_mp also has these
		{
			return 1;
		}

		if (!strcmp(CURRENT_ZONE_NAME, "patch_mp"))
		{
			return 0;
		}
	}

	if (AssetRestrict_RestrictFromMaps(type, name, CURRENT_ZONE_NAME))
	{
		//deadAssets[*(DWORD*)entry] = true;
		return 2;
	}

	return 1;
}

void DoBeforeLoadAsset(assetType_t type, void** entry)
{
	if (entry)
	{
		AssetRestrict_PreLoadFromMaps(type, *entry, CURRENT_ZONE_NAME);
		AssetRestrict_PreLoadFromExperimental(type, *entry, CURRENT_ZONE_NAME);
	}
}

void* LoadDefaultAsset(assetType_t atype)
{
	void* defaultAsset;

	__asm
	{
		push edi
		mov edi, atype
		mov eax, 5BB210h
		call eax
		pop edi

		mov defaultAsset, eax
	}

	static void* retStuff[2];
	retStuff[0] = 0;
	retStuff[1] = defaultAsset;

	return retStuff;
}

void __declspec(naked) DB_AddXAssetHookStub()
{
	__asm
	{
		mov eax, [esp + 4]
		mov ecx, [esp + 8]

		push ecx
		push eax
		call CanWeLoadAsset
		add esp, 08h

		cmp al, 2h
		je doDefault

		test al, al
		jz doNotLoad

		mov eax, [esp + 4]
		mov ecx, [esp + 8]
		push ecx
		push eax
		call DoBeforeLoadAsset
		add esp, 08h

		mov eax, [esp + 8]
		sub esp, 14h
		jmp dbAddXAssetHookRet

doNotLoad:
		mov eax, [esp + 8]
		retn

doDefault:
		mov eax, [esp + 4]

		push eax
		call LoadDefaultAsset
		add esp, 4h

		retn
	}
}

#if 0
StompHook markAssetHook;
DWORD markAssetHookLoc = 0x5BBA30;
DWORD markAssetHookRet = 0x5BBA38;

bool MarkAssetHookFunc(DWORD ptr)
{
	return (deadAssets.find(ptr) != deadAssets.end());
}

void __declspec(naked) MarkAssetHookStub()
{
	__asm
	{
		mov eax, [esp + 4h]

		push eax
		call MarkAssetHookFunc
		add esp, 4

		cmp al, 1
		je returnFalse

		sub esp, 8
		push edi
		mov edi, [esp + 0Ch + 4]
		jmp markAssetHookRet

returnFalse:
		xor eax, eax
		retn
	}
}
#endif

typedef int (__cdecl * DB_GetXAssetSizeHandler_t)();

void** DB_XAssetPool = (void**)0x7998A8;
unsigned int* g_poolSize = (unsigned int*)0x7995E8;

DB_GetXAssetSizeHandler_t* DB_GetXAssetSizeHandlers = (DB_GetXAssetSizeHandler_t*)0x799488;

void* ReallocateAssetPool(assetType_t type, unsigned int newSize)
{
	int elSize = DB_GetXAssetSizeHandlers[type]();
	void* poolEntry = malloc(newSize * elSize);
	DB_XAssetPool[type] = poolEntry;
	g_poolSize[type] = newSize;
	return poolEntry;
}

void PatchMW2_AssetRestrict()
{
	dbAddXAssetHook.initialize(dbAddXAssetHookLoc, DB_AddXAssetHookStub, 7);
	dbAddXAssetHook.installHook();

	InitializeImageLoad();

	// addon_map_ents entry size
	*(DWORD*)0x799690 = 16;

	DWORD gameWorldSP = (DWORD)ReallocateAssetPool(ASSET_TYPE_GAME_MAP_SP, 1);
	ReallocateAssetPool(ASSET_TYPE_ADDON_MAP_ENTS, 256);
	ReallocateAssetPool(ASSET_TYPE_XMODEL, 3072);
	//ReallocateAssetPool(ASSET_TYPE_IMAGE, 7168);
	ReallocateAssetPool(ASSET_TYPE_LOADED_SOUND, 2700);
	ReallocateAssetPool(ASSET_TYPE_FX, 1200);
	ReallocateAssetPool(ASSET_TYPE_LOCALIZE, 14000);
	ReallocateAssetPool(ASSET_TYPE_XANIM, 8192);
	ReallocateAssetPool(ASSET_TYPE_PHYSPRESET, 128);
	ReallocateAssetPool(ASSET_TYPE_PIXELSHADER, 10000);
	ReallocateAssetPool(ASSET_TYPE_VERTEXDECL, 196);

	//markAssetHook.initialize(markAssetHookLoc, MarkAssetHookStub);
	//markAssetHook.installHook();
}
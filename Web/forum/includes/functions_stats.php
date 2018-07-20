<?php
if (!defined('IN_PHPBB'))
{
	exit;
}

function do_stats_for_viewtopic(&$user_cache)
{
	$uids = array_keys($user_cache);
	$stats = get_user_stats($uids);

	foreach ($stats as $user_id => $data)
	{
		$user_cache[$user_id]['stats'] = $data;
	}
}

function do_stats_for_template($user_id)
{
	global $phpbb_root_path, $phpEx, $cache;

	$sd = new StructuredData($phpbb_root_path . 'stats/playerdata.xml');

	$database = mysqli_connect('localhost', 'aiw3', 'NRvFafRdHVcUzBRR', 'aiw3_xnp');
	$sql = "SELECT * FROM iw4_profiles WHERE user_id = '" . $user_id . "';";
	
	$result = mysqli_query($database, $sql);
	
	if ($result->num_rows) {
		while($row = $result->fetch_assoc()) {
			return array(
						'S_STATS_PRESTIGE' => $row['prestige'],
						'S_STATS_PRESTIGEICON' => ($row['prestige'] > 0 && $row['prestige'] <= 11) ? $phpbb_root_path . 'images/prestige/' . 'cardicon_prestige_' . $row['prestige'] . '.png' : '',
						'S_STATS_EXPERIENCE' => $row['experience'],
						'S_STATS_RANK' => get_rank_for_experience($row['experience']),
						'S_STATS_RANKICON' => $phpbb_root_path . 'images/prestige/' . get_icon_for_experience($row['experience']),
						'S_STATS_TITLE' => get_text_for_cardtitle($row['cardtitle']),
						'S_STATS_CARDICON' => $phpbb_root_path . 'images/prestige/' . get_icon_for_cardicon($row['cardicon']),
			);
		}
	}
	else
	{
		return array();
	}
}

function get_icon_for_cardicon($id)
{
	static $icons;
	
	if (empty($icons))
	{
		$icons = array(
			'cardicon_default' => 'cardicon_dogtags_1.png',
			'cardicon_pvt1' => 'cardicon_rank_pvt1.png',
			'cardicon_pfc1' => 'cardicon_rank_pfc1.png',
			'cardicon_spc1' => 'cardicon_rank_spc1.png',
			'cardicon_cpl1' => 'cardicon_rank_cpl1.png',
			'cardicon_sgt1' => 'cardicon_rank_sgt1.png',
			'cardicon_ssgt1' => 'cardicon_rank_ssgt1.png',
			'cardicon_sfc1' => 'cardicon_rank_sgtfc1.png',
			'cardicon_msgt1' => 'cardicon_rank_msgt1.png',
			'cardicon_1stsgt1' => 'cardicon_rank_fsgt1.png',
			'cardicon_sgtmaj1' => 'cardicon_rank_sgtmaj1.png',
			'cardicon_cmdsgtmaj1' => 'cardicon_rank_cmdsgtmaj1.png',
			'cardicon_2ndlt1' => 'cardicon_rank_2ndlt1.png',
			'cardicon_1stlt1' => 'cardicon_rank_1stlt1.png',
			'cardicon_capt1' => 'cardicon_rank_capt1.png',
			'cardicon_maj1' => 'cardicon_rank_maj1.png',
			'cardicon_ltcol1' => 'cardicon_rank_ltcol1.png',
			'cardicon_col1' => 'cardicon_rank_col1.png',
			'cardicon_bgen1' => 'cardicon_rank_bgen1.png',
			'cardicon_majgen1' => 'cardicon_rank_majgen1.png',
			'cardicon_ltgen1' => 'cardicon_rank_ltgen1.png',
			'cardicon_gen1' => 'cardicon_rank_gen1.png',
			'cardicon_comm1' => 'cardicon_rank_comm.png',
			'cardicon_patch' => 'cardicon_patch.png',
			'cardicon_compass' => 'cardicon_mig.png',
			'cardicon_b2' => 'cardicon_b2.png',
			'cardicon_harrier' => 'cardicon_harrier.png',
			'cardicon_noseart1' => 'cardicon_f15.png',
			'cardicon_pavelow' => 'cardicon_pavelow.png',
			'cardicon_blue' => 'cardicon_abrams.png',
			'cardicon_tsuenami' => 'cardicon_stryker.png',
			'cardicon_macgregor' => 'cardicon_hummer.png',
			'cardicon_grunt_2' => 'cardicon_launcher.png',
			'cardicon_nates' => 'cardicon_tank_01.png',
			'cardicon_brassknuckle' => 'cardicon_aircraft_01.png',
			'cardicon_ac130_angelflare' => 'cardicon_ac130_angelflare.png',
			'cardicon_skullaward' => 'cardicon_paratrooper.png',
			'cardicon_umbracatervae' => 'cardicon_noseart1.png',
			'cardicon_brad' => 'cardicon_helmet_medic.png',
			'cardicon_heartbeatsensor' => 'cardicon_heartbeatsensor.png',
			'cardicon_bullets_50cal' => 'cardicon_bullets_50cal.png',
			'cardicon_bulletcase' => 'cardicon_bullet_case.png',
			'cardicon_shotgun_shells' => 'cardicon_shotgun_shells.png',
			'cardicon_doubletap' => 'cardicon_doubletap.png',
			'cardicon_fmj' => 'cardicon_fmj.png',
			'cardicon_grunt' => 'cardicon_brit_beret.png',
			'cardicon_juggernaut_2' => 'cardicon_russian_beret.png',
			'cardicon_gasmask' => 'cardicon_gasmask.png',
			'cardicon_nightvision_1' => 'cardicon_nightvision_1.png',
			'cardicon_tsunami' => 'cardicon_nightvision_2.png',
			'cardicon_sniperscope' => 'cardicon_sniperscope.png',
			'cardicon_badgirl' => 'cardicon_compass.png',
			'cardicon_iss' => 'cardicon_missile_1.png',
			'cardicon_minigun' => 'cardicon_minigun.png',
			'cardicon_ball_basketball_1' => 'cardicon_hat_n_knife.png',
			'cardicon_helmet_football_1' => 'cardicon_vest_1.png',
			'cardicon_lion' => 'cardicon_simplegun.png',
			'cardicon_frag' => 'cardicon_frag_grenade.png',
			'cardicon_semtex' => 'cardicon_semtex.png',
			'cardicon_tacticalinsertion' => 'cardicon_flare.png',
			'cardicon_blastshield' => 'cardicon_blast_shield.png',
			'cardicon_c4' => 'cardicon_c4.png',
			'cardicon_claymore' => 'cardicon_claymore.png',
			'cardicon_stab' => 'cardicon_throwing_knive.png',
			'cardicon_sheppard' => 'cardicon_boots_01.png',
			'cardicon_comic_shepherd' => 'cardicon_binoculars_1.png',
			'cardicon_koiker_hound' => 'cardicon_helmet_pilot.png',
			'cardicon_grigsby' => 'cardicon_grigsby.png',
			'cardicon_taskforcearmy01' => 'cardicon_taskforcearmy01.png',
			'cardicon_ghost_bust' => 'cardicon_ghost_bust.png',
			'cardicon_ghillie' => 'cardicon_ghillie.png',
			'cardicon_chicken' => 'cardicon_boots_02.png',
			'cardicon_price_ww2' => 'cardicon_price_ww2.png',
			'cardicon_pricewoodland' => 'cardicon_pricewoodland.png',
			'cardicon_pricearctic' => 'cardicon_pricearctic.png',
			'cardicon_assad' => 'cardicon_assad.png',
			'cardicon_makarov' => 'cardicon_makarov.png',
			'cardicon_brock' => 'cardicon_snowmobile.png',
			'cardicon_throwingknife' => 'cardicon_eagle.png',
			'cardicon_league_magnum' => 'cardicon_veteran.png',
			'cardicon_dive' => 'cardicon_booniehat.png',
			'cardicon_ball_soccer_1' => 'cardicon_helmet_ranger.png',
			'cardicon_ball_baseball_1' => 'cardicon_helmet_brit_ww2.png',
			'cardicon_ball_football_1' => 'cardicon_helmet_army.png',
			'cardicon_kitten' => 'cardicon_hipflask.png',
			'cardicon_pushingupdaisies' => 'cardicon_pushindaisies.png',
			'cardicon_biohazard' => 'cardicon_biohazard.png',
			'cardicon_juggernaut_1' => 'cardicon_juggernaut_1.png',
			'cardicon_ghost_skull' => 'cardicon_juggernaut_2.png',
			'cardicon_impale' => 'death_impale.png',
			'cardicon_riotdeath' => 'death_riot_shield.png',
			'cardicon_headshot' => 'cardicon_headshot.png',
			'cardicon_car' => 'cardicon_car.png',
			'cardicon_gumby' => 'cardicon_gumby.png',
			'cardicon_spetsnaz' => 'cardicon_gloss_grenade.png',
			'cardicon_redhand' => 'cardicon_redhand.png',
			'cardicon_tf141' => 'cardicon_tf141.png',
			'cardicon_ball_volleyball_1' => 'cardicon_umbracatervae.png',
			'cardicon_iwlogo' => 'cardicon_default.png',
			'cardicon_porterjustice' => 'cardicon_porterjustice.png',
			'cardicon_knife_logo' => 'cardicon_throwing_knife_soda.png',
			'cardicon_humantrophy' => 'cardicon_humantrophy.png',
			'cardicon_knife' => 'cardicon_shuriken.png',
			'cardicon_skull' => 'cardicon_skull.png',
			'cardicon_xray' => 'cardicon_xray.png',
			'cardicon_skull_black' => 'cardicon_skull_black.png',
			'cardicon_nightvision_2' => 'cardicon_snakeeyes.png',
			'cardicon_moon' => 'cardicon_motorcycle.png',
			'cardicon_radiation' => 'cardicon_radiation.png',
			'cardicon_goodgirl' => 'cardicon_tsunami.png',
			'cardicon_seasnipers' => 'cardicon_seasnipers.png',
			'cardicon_soap' => 'cardicon_soap_bar.png',
			'cardicon_gold' => 'cardicon_gold.png',
			'cardicon_pirate' => 'cardicon_pirate_1.png',
			'cardicon_pirateflag' => 'cardicon_pirateflag_1.png',
			'cardicon_helmet_baseball_1' => 'cardicon_moon.png',
			'cardicon_hockey_1' => 'cardicon_mexican_blue.png',
			'cardicon_hockey_2' => 'cardicon_mexican_red.png',
			'cardicon_tennisracket' => 'cardicon_helmet_samurai.png',
			'cardicon_tiger' => 'cardicon_tiger.png',
			'cardicon_honeybadger01' => 'cardicon_honeybadger01.png',
			'cardicon_kinggorilla' => 'cardicon_kinggorilla.png',
			'cardicon_thecow' => 'cardicon_thecow.png',
			'cardicon_warpig' => 'cardicon_warpig.png',
			'cardicon_birdbrain' => 'cardicon_birdbrain.png',
			'cardicon_claw' => 'cardicon_claw.png',
			'cardicon_boot' => 'cardicon_boot.png',
			'cardicon_devilfinger' => 'cardicon_devilfinger.png',
			'cardicon_brassknuckles' => 'cardicon_brassknuckles.png',
			'cardicon_8ball' => 'cardicon_8ball.png',
			'cardicon_mushroom' => 'cardicon_weed.png',
			'cardicon_jets' => 'cardicon_award_jets.png',
			'cardicon_tictac' => 'cardicon_tictacboom.png',
			'cardicon_joystick' => 'cardicon_joystick.png',
			'cardicon_devil' => 'cardicon_red_devil.png',
			'cardicon_thebomb' => 'cardicon_thebomb.png',
			'cardicon_sniper' => 'cardicon_skullaward.png',
			'cardicon_icecream' => 'cardicon_chicken.png',
			'cardicon_gametype' => 'cardicon_comic_shepherd.png',
			'cardicon_comic_price' => 'cardicon_comic_price.png',
			'cardicon_toon_price_1' => 'cardicon_toon_price_1.png',
			'cardicon_8bit_price' => 'cardicon_8_bit_price.png',
			'cardicon_readhead' => 'cardicon_redhead.png',
			'cardicon_pacifier_blue' => 'cardicon_pacifier_blue.png',
			'cardicon_pacifier_pink' => 'cardicon_pacifier_pink.png',
			'cardicon_cod4' => 'cardicon_cod4.png',
			'cardicon_iw' => 'cardicon_nvg_star.gif',
			'cardicon_riot_shield' => 'cardicon_riot_shield.png',
			'cardicon_korean' => 'cardicon_iss.png',
			'cardicon_gears' => 'cardicon_ghost.png',
			'cardicon_treasurechest' => 'cardicon_treasurechest_1.png',
			'cardicon_treasuremap' => 'cardicon_treasuremap_1.png',
			'cardicon_weed' => 'cardicon_gears.png',
			'cardicon_league_1911' => 'cardicon_ghost_mic.png',
			'cardicon_league_grenade' => 'cardicon_league_grenade.png',
			'cardicon_rhino' => 'cardicon_rhino.png',
			'cardicon_bear' => 'cardicon_bear.png',
			'cardicon_hyena' => 'cardicon_hyena.png',
			'cardicon_sugarglider' => 'cardicon_sugarglider.png',
			'cardicon_stop' => 'cardicon_stop.png',
			'cardicon_burgertown' => 'cardicon_burgertown.png',
			'cardicon_bulb' => 'cardicon_bulb.png',
			'cardicon_tire' => 'cardicon_tire.png',
			'cardicon_loadedfinger' => 'cardicon_loadedfinger.png',
			'cardicon_girlskull' => 'cardicon_girlskull.png',
			'cardicon_abduction' => 'cardicon_abduction.png',
			'cardicon_illuminati' => 'cardicon_illuminati.png',
			'cardicon_copycat' => 'cardicon_copycat.png',
			'cardicon_painkiller' => 'cardicon_painkiller.png',
			'cardicon_martyrdom' => 'cardicon_grenadepulldeath.png',
			'cardicon_finalstand' => 'cardicon_final_stand.png',
			'cardicon_prestige1' => 'cod4_prestige_01.png',
			'cardicon_prestige2' => 'cod4_prestige_02.png',
			'cardicon_prestige3' => 'cod4_prestige_03.png',
			'cardicon_prestige4' => 'cod4_prestige_04.png',
			'cardicon_prestige5' => 'cod4_prestige_05.png',
			'cardicon_prestige6' => 'cod4_prestige_06.png',
			'cardicon_prestige7' => 'cod4_prestige_07.png',
			'cardicon_prestige8' => 'cod4_prestige_08.png',
			'cardicon_prestige9' => 'cod4_prestige_09.png',
			'cardicon_prestige10' => 'cardicon_prestige10anim.gif',
			'cardicon_prestige10_02' => 'cardicon_prestige10anim_02.gif',
			'cardicon_mw2_prestige1' => 'cardicon_prestige_1.png',
			'cardicon_mw2_prestige2' => 'cardicon_prestige_2.png',
			'cardicon_mw2_prestige3' => 'cardicon_prestige_3.png',
			'cardicon_mw2_prestige4' => 'cardicon_prestige_4.png',
			'cardicon_mw2_prestige5' => 'cardicon_prestige_5.png',
			'cardicon_mw2_prestige6' => 'cardicon_prestige_6.png',
			'cardicon_mw2_prestige7' => 'cardicon_prestige_7.png',
			'cardicon_mw2_prestige8' => 'cardicon_prestige_8.png',
			'cardicon_mw2_prestige9' => 'cardicon_prestige_9.png',
			'cardicon_mw2_prestige10' => 'cardicon_prestige_10.png',
			'cardicon_mw2_prestige11' => 'cardicon_t5_prestige_1.png',
			'cardicon_ac130' => 'dpad_ks_ac130.gif',
			'cardicon_uav' => 'dpad_ks_uav.gif',
			'cardicon_carepackage' => 'dpad_care_package.gif',
			'cardicon_counteruav' => 'dpad_counter_uav.gif',
			'cardicon_sentrygun' => 'dpad_sentry_gun.gif',
			'cardicon_predatormissile' => 'dpad_hellfire_missile.gif',
			'cardicon_precair' => 'dpad_precision_airstrike.gif',
			'cardicon_harrierstrike' => 'dpad_harrier.gif',
			'cardicon_attackchopper' => 'dpad_helicoptor_support.gif',
			'cardicon_emergencyair' => 'dpad_emergency_airdrop.gif',
			'cardicon_pavelowkillstreak' => 'dpad_pavelow.gif',
			'cardicon_stealthkillstreak' => 'dpad_stealth_bomber.gif',
			'cardicon_choppergunner' => 'dpad_cobra_gunner.gif',
			'cardicon_empkillstreak' => 'dpad_emp.gif',
			'cardicon_tacticalnuke' => 'dpad_nuke.gif',
			'cardicon_marathon' => 'specialty_marathon_upgrade.png',
			'cardicon_sleightofhand' => 'specialty_fastreload_upgrade.png',
			'cardicon_scavenger' => 'specialty_scavenger_upgrade.png',
			'cardicon_dangerclose' => 'specialty_danger_close_upgrade.png',
			'cardicon_bling' => 'specialty_bling_upgrade.png',
			'cardicon_stoppingpower' => 'specialty_bulletdamage_upgrade.png',
			'cardicon_lightweight' => 'specialty_lightweight_upgrade.png',
			'cardicon_hardline' => 'specialty_hardline_upgrade.png',
			'cardicon_coldblooded' => 'specialty_cold_blooded_upgrade.png',
			'cardicon_oma' => 'specialty_one_man_army_upgrade.png',
			'cardicon_commando' => 'specialty_commando_upgrade.png',
			'cardicon_steadyaim' => 'specialty_bulletaccuracy_upgrade.png',
			'cardicon_scrambler' => 'specialty_scrambler_upgrade.png',
			'cardicon_sitrep' => 'specialty_sitrep_pro.png',
			'cardicon_laststand' => 'specialty_pistoldeath_upgrade.png',
			'cardicon_ninja' => 'specialty_ninja_pro.png',
			'cardicon_ak47_marksman' => 'cardicon_marksman_ak47.png',
			'cardicon_masada_marksman' => 'cardicon_marksman_masada.png',
			'cardicon_m16_marksman' => 'cardicon_marksman_m16.png',
			'cardicon_tavor_marksman' => 'cardicon_marksman_tavor.png',
			'cardicon_fn2000_marksman' => 'cardicon_marksman_fn2000.png',
			'cardicon_m4_marksman' => 'cardicon_marksman_m4.png',
			'cardicon_scar_marksman' => 'cardicon_marksman_scar.png',
			'cardicon_fal_marksman' => 'cardicon_marksman_fal.png',
			'cardicon_famas_marksman' => 'cardicon_marksman_famas.png',
			'cardicon_mp5k_marksman' => 'cardicon_marksman_mp5k.png',
			'cardicon_uzi_marksman' => 'cardicon_marksman_uzi.png',
			'cardicon_kriss_marksman' => 'cardicon_marksman_kriss.png',
			'cardicon_ump45_marksman' => 'cardicon_marksman_ump45.png',
			'cardicon_p90_marksman' => 'cardicon_marksman_p90.png',
			'cardicon_aug_marksman' => 'cardicon_marksman_aug.png',
			'cardicon_m240_marksman' => 'cardicon_marksman_m240.png',
			'cardicon_sa80_marksman' => 'cardicon_marksman_sa80.png',
			'cardicon_rpd_marksman' => 'cardicon_marksman_rpd.png',
			'cardicon_mg4_marksman' => 'cardicon_marksman_mg4.png',
			'cardicon_cheytac_marksman' => 'cardicon_marksman_cheytac.png',
			'cardicon_barrett_marksman' => 'cardicon_marksman_barrett.png',
			'cardicon_m21_marksman' => 'cardicon_marksman_m21.png',
			'cardicon_wa2000_marksman' => 'cardicon_marksman_wa2000.png',
			'cardicon_glock_marksman' => 'cardicon_marksman_glock.png',
			'cardicon_beretta393_marksman' => 'cardicon_marksman_beretta393.png',
			'cardicon_tmp_marksman' => 'cardicon_marksman_tmp.png',
			'cardicon_pp2000_marksman' => 'cardicon_marksman_pp2000.png',
			'cardicon_beretta_marksman' => 'cardicon_marksman_beretta.png',
			'cardicon_usp_marksman' => 'cardicon_marksman_usp.png',
			'cardicon_deserteagle_marksman' => 'cardicon_marksman_deserteagle.png',
			'cardicon_coltanaconda_marksman' => 'cardicon_marksman_coltanaconda.png',
			'cardicon_m1014_marksman' => 'cardicon_marksman_m1014.png',
			'cardicon_striker_marksman' => 'cardicon_marksman_striker.png',
			'cardicon_aa12_marksman' => 'cardicon_marksman_aa12.png',
			'cardicon_spas12_marksman' => 'cardicon_marksman_spas12.png',
			'cardicon_model1887_marksman' => 'cardicon_marksman_model1887.png',
			'cardicon_ranger_marksman' => 'cardicon_marksman_ranger.png',
			'cardicon_ak47_expert' => 'cardicon_expert_ak47.png',
			'cardicon_masada_expert' => 'cardicon_expert_masada.png',
			'cardicon_m16_expert' => 'cardicon_expert_m16.png',
			'cardicon_tavor_expert' => 'cardicon_expert_tavor.png',
			'cardicon_fn2000_expert' => 'cardicon_expert_fn2000.png',
			'cardicon_m4_expert' => 'cardicon_expert_m4.png',
			'cardicon_scar_expert' => 'cardicon_expert_scar.png',
			'cardicon_fal_expert' => 'cardicon_expert_fal.png',
			'cardicon_famas_expert' => 'cardicon_expert_famas.png',
			'cardicon_mp5k_expert' => 'cardicon_expert_mp5k.png',
			'cardicon_uzi_expert' => 'cardicon_expert_uzi.png',
			'cardicon_kriss_expert' => 'cardicon_expert_kriss.png',
			'cardicon_ump45_expert' => 'cardicon_expert_ump45.png',
			'cardicon_p90_expert' => 'cardicon_expert_p90.png',
			'cardicon_aug_expert' => 'cardicon_expert_aug.png',
			'cardicon_m240_expert' => 'cardicon_expert_m240.png',
			'cardicon_sa80_expert' => 'cardicon_expert_sa80.png',
			'cardicon_rpd_expert' => 'cardicon_expert_rpd.png',
			'cardicon_mg4_expert' => 'cardicon_expert_mg4.png',
			'cardicon_cheytac_expert' => 'cardicon_expert_cheytac.png',
			'cardicon_barrett_expert' => 'cardicon_expert_barrett.png',
			'cardicon_m21_expert' => 'cardicon_expert_m21.png',
			'cardicon_wa2000_expert' => 'cardicon_expert_wa2000.png',
			'cardicon_glock_expert' => 'cardicon_expert_glock.png',
			'cardicon_tmp_expert' => 'cardicon_expert_tmp.png',
			'cardicon_beretta393_expert' => 'cardicon_expert_beretta393.png',
			'cardicon_pp2000_expert' => 'cardicon_expert_pp2000.png',
			'cardicon_m1014_expert' => 'cardicon_expert_m1014.png',
			'cardicon_striker_expert' => 'cardicon_expert_striker.png',
			'cardicon_aa12_expert' => 'cardicon_expert_aa12.png',
			'cardicon_spas12_expert' => 'cardicon_expert_spas12.png',
			'cardicon_model1887_expert' => 'cardicon_expert_model1887.png',
			'cardicon_ranger_expert' => 'cardicon_expert_ranger.png',
			'cardicon_beretta_expert' => 'cardicon_expert_beretta.png',
			'cardicon_usp_expert' => 'cardicon_expert_usp.png',
			'cardicon_deserteagle_expert' => 'cardicon_expert_deserteagle.png',
			'cardicon_coltanaconda_expert' => 'cardicon_expert_coltanaconda.png',
		);
	}
	
	return $icons[$id];
}

function get_text_for_cardtitle($id)
{
	static $titles;
	
	if (empty($titles))
	{
		$titles = array(
			'cardtitle_default' => 'FNG',
			'cardtitle_ssdd' => 'S.S.D.D.',
			'cardtitle_20' => 'Sgt.',
			'cardtitle_30' => '1st Sgt.',
			'cardtitle_40' => '1st Lt.',
			'cardtitle_50' => 'Maj.',
			'cardtitle_60' => 'Gen.',
			'cardtitle_70' => 'Commander',
			'cardtitle_70a' => '',
			'cardtitle_dishtherock' => 'Dish the Rock',
			'cardtitle_teamplayer' => 'Team Player',
			'cardtitle_sidekick' => 'Sidekick',
			'cardtitle_lonewolf' => 'Lone Wolf',
			'cardtitle_hairtrigger' => 'Hair Trigger',
			'cardtitle_sureshot' => 'Sureshot',
			'cardtitle_flyswatter' => 'Flyswatter',
			'cardtitle_closesupport' => 'Close Support',
			'cardtitle_overwatch' => 'Overwatch',
			'cardtitle_nbk' => 'NBK',
			'cardtitle_ghilliemist' => 'Ghillie in the Mist',
			'cardtitle_artofstealth' => 'Art of Stealth',
			'cardtitle_timeontarget' => 'Time on Target',
			'cardtitle_preemptive' => 'Preemptive Strike',
			'cardtitle_clusterbomb' => 'Cluster Bomb',
			'cardtitle_lowprofile' => 'Low Profile',
			'cardtitle_ironlungs' => 'Iron Lungs',
			'cardtitle_rival' => 'Rival',
			'cardtitle_grassyknoll' => 'Grassy Knoll',
			'cardtitle_uavjammer' => 'UAV Jammer',
			'cardtitle_prestige4' => '4 the Record',
			'cardtitle_destroyer' => 'Destroyer',
			'cardtitle_its_sabotage' => 'It\'s Sabotage!',
			'cardtitle_shockandawe' => 'Shock and Awe',
			'cardtitle_pyro' => 'Pyromaniac',
			'cardtitle_concussive' => 'Concussive Barrage',
			'cardtitle_backfire' => 'Backfire',
			'cardtitle_shotdown' => 'Shot Down',
			'cardtitle_hotshot' => 'Hot Shot',
			'cardtitle_denier' => 'The Denier',
			'cardtitle_quickdraw' => 'Quickdraw',
			'cardtitle_hiredgun' => 'Hired Gun',
			'cardtitle_bountyhunter' => 'Bounty Hunter',
			'cardtitle_absenteekiller' => 'Absentee Killer',
			'cardtitle_blindfire' => 'Blindfire',
			'cardtitle_gat' => 'Gat Trigger',
			'cardtitle_handsfree' => 'Hands Free',
			'cardtitle_nohands' => 'Look! No Hands!',
			'cardtitle_automator' => 'The Automator',
			'cardtitle_rainoffire' => 'Rain of Fire',
			'cardtitle_steelreign' => 'Reign Down',
			'cardtitle_globalthermo' => 'Global Thermonuclear War',
			'cardtitle_bombshell' => 'Bombshell',
			'cardtitle_thabomb' => 'Tha Bomb',
			'cardtitle_wopr' => 'MOAB',
			'cardtitle_20a' => 'Sarge',
			'cardtitle_dictator' => 'Dictator',
			'cardtitle_enemyofthestate' => 'Enemy of the State',
			'cardtitle_rollingthunder' => 'Rolling Thunder',
			'cardtitle_bombsaway' => 'Bomb\'s Away!',
			'cardtitle_eod' => 'EOD',
			'cardtitle_explosiveordinance' => 'Explosive Ordinance',
			'cardtitle_madbomber' => 'The Mad Bomber',
			'cardtitle_boomboompow' => 'Boom Boom',
			'cardtitle_silentstrike' => 'Silent Strike',
			'cardtitle_invisible' => 'Invisible',
			'cardtitle_suppressor' => 'Suppressor',
			'cardtitle_plasticman' => 'Plastique',
			'cardtitle_c4andafter' => 'C4 and After',
			'cardtitle_iceman' => 'I.C.E. Man',
			'cardtitle_jumpjet' => 'Jump Jet',
			'cardtitle_truelies' => 'True Liar',
			'cardtitle_afterburner' => 'Afterburner',
			'cardtitle_angelofdeath' => 'Angel of Death',
			'cardtitle_godhand' => 'Godhand',
			'cardtitle_deathfromabove' => 'Death From Above',
			'cardtitle_readyfire' => 'Ready. Aim. Fire!',
			'cardtitle_invincible' => 'The Invincible',
			'cardtitle_spectre' => 'Spectre',
			'cardtitle_planewhisperer' => 'Plane Whisperer',
			'cardtitle_brokenarrow' => 'Broken Arrow',
			'cardtitle_carpetbomber' => 'Carpet Bomber',
			'cardtitle_hardtarget' => 'Hard Target',
			'cardtitle_wargasm' => 'Wargasm',
			'cardtitle_unbreakable' => 'Unbreakable',
			'cardtitle_tangodown' => 'Tango Down',
			'cardtitle_avenger' => 'The Avenger',
			'cardtitle_protectserve' => 'Protect and Serve',
			'cardtitle_fullarsenal' => 'Full Arsenal',
			'cardtitle_mvpassassin' => 'MVP Assassin',
			'cardtitle_robinhood' => 'Robin Hood',
			'cardtitle_earlydetection' => 'Early Detection',
			'cardtitle_hidef' => 'Hi Def',
			'cardtitle_bigbrother' => 'Big Brother',
			'cardtitle_amplifier' => 'Amplifier',
			'cardtitle_givinstatic' => 'Givin\' Static',
			'cardtitle_technokiller' => 'Techno Killer',
			'cardtitle_gettothechoppa' => 'Get to the Choppa!',
			'cardtitle_cobrakai' => 'Cobra Strike',
			'cardtitle_cobracommander' => 'Sky Commander',
			'cardtitle_dzclear' => 'DZ Clear',
			'cardtitle_airwolf' => 'Eyes Above',
			'cardtitle_stringfellow' => 'Direct Connect',
			'cardtitle_blackout' => 'Flying Tank',
			'cardtitle_transformer' => 'Transformer',
			'cardtitle_shotover' => 'Shot Over',
			'cardtitle_badaboom' => 'Big Bada Boom',
			'cardtitle_genocidal' => 'Kill for Good',
			'cardtitle_sonicboom' => 'Sonic Boom',
			'cardtitle_1bullet2kills' => '1bullet2kills',
			'cardtitle_kfactor' => 'K Factor',
			'cardtitle_bitethebullet' => 'Bite the Bullet',
			'cardtitle_harcoreonly' => 'Hardcore Only',
			'cardtitle_bandolier' => 'Bandolier',
			'cardtitle_bulletproof' => 'Bulletproof',
			'cardtitle_sigint' => 'SIGINT',
			'cardtitle_starfishprime' => 'Starfish Prime',
			'cardtitle_counterintel' => 'Counterintelligence',
			'cardtitle_booyah' => 'Booyah!',
			'cardtitle_og' => 'OG',
			'cardtitle_finishingtouch' => 'Finishing Touch',
			'cardtitle_boomheadshot' => 'BOOM!  Headshot.',
			'cardtitle_bloodmoney' => 'Blood Money',
			'cardtitle_bam' => 'BAM!',
			'cardtitle_intergalactic' => 'Intergalactic',
			'cardtitle_watchman' => 'The Watchman',
			'cardtitle_livelong' => 'Live Long...',
			'cardtitle_clickclickboom' => 'Click Click Boom',
			'cardtitle_stungun' => 'Stun Gun',
			'cardtitle_hardtokill' => 'Hard to Kill',
			'cardtitle_popoff' => 'Pop Off',
			'cardtitle_faceoff' => 'Faceoff',
			'cardtitle_topgun' => 'Top Gun',
			'cardtitle_armedanddangerous' => 'Armed and Dangerous',
			'cardtitle_reloaded' => 'Reloaded',
			'cardtitle_highcaliber' => 'High Caliber',
			'cardtitle_itspersonal' => 'It\'s Personal',
			'cardtitle_doctor' => 'My name is...',
			'cardtitle_fired' => 'Fired!',
			'cardtitle_ambush' => 'Ambush!',
			'cardtitle_behindenemy' => 'Behind Enemy Lines',
			'cardtitle_pathfinder' => 'Pathfinder',
			'cardtitle_takeastab' => 'Take a Stab',
			'cardtitle_backstabber' => 'Backstabber',
			'cardtitle_inciser' => 'The Inciser',
			'cardtitle_highlander' => 'Highlander',
			'cardtitle_excalibur' => 'Excalibur',
			'cardtitle_impaler' => 'Impaler',
			'cardtitle_bob' => 'Blood Brothers',
			'cardtitle_legend' => 'The Legend',
			'cardtitle_epic' => 'Epic',
			'cardtitle_sprayandpray' => 'Remote Viewer',
			'cardtitle_submittoauthority' => 'Submit to Authority',
			'cardtitle_xrayvision' => 'X-Ray Vision',
			'cardtitle_santaclaus' => 'Charity Case',
			'cardtitle_timeismoney' => 'Time is Money',
			'cardtitle_imrich' => 'I\'m Rich!',
			'cardtitle_pinpuller' => 'Pin Puller',
			'cardtitle_steelnerves' => 'Nerves of Steel',
			'cardtitle_skycaptain' => 'Airborne',
			'cardtitle_bullseye' => 'Bullseye',
			'cardtitle_stickman' => 'Stickman',
			'cardtitle_theripper' => 'The Ripper',
			'cardtitle_ghostrider' => 'Ghostrider',
			'cardtitle_rezero' => 'Re-Zero',
			'cardtitle_vulture' => 'Vulture',
			'cardtitle_mastatdon' => 'The Mastodon',
			'cardtitle_grimreaper' => 'The Grim Reaper',
			'cardtitle_jollygreen' => 'The Dragon',
			'cardtitle_fireforget' => 'Fire and Forget',
			'cardtitle_allyourbase' => 'All Your Base',
			'cardtitle_dangerclose' => 'Danger Close',
			'cardtitle_moneyshot' => 'Money Shot!',
			'cardtitle_makeitrain' => 'Rainmaker',
			'cardtitle_reversaloffortune' => 'Reversal of Fortune',
			'cardtitle_doubleagent' => 'Double Agent',
			'cardtitle_spygame' => 'Spy Game',
			'cardtitle_madman' => 'Mad Man',
			'cardtitle_philanthropist' => 'Philanthropist',
			'cardtitle_headsup' => 'Heads Up!',
			'cardtitle_biggertheyare' => 'The Bigger They Are...',
			'cardtitle_harderthey' => '...The Harder They Fall.',
			'cardtitle_partinggift' => 'Parting Gift',
			'cardtitle_deadline' => 'Deadline',
			'cardtitle_deadmansswitch' => 'Dead Man\'s Switch',
			'cardtitle_dyingbreath' => 'Dying Breath',
			'cardtitle_prestige6' => '6 Feet Under',
			'cardtitle_hijacker' => 'Hijacker',
			'cardtitle_masterblaster' => 'Solid Steel',
			'cardtitle_decathlete' => 'Decathlete',
			'cardtitle_mach5' => 'Mach 5',
			'cardtitle_2fast' => '2fast',
			'cardtitle_speeddemon' => 'Speed Demon',
			'cardtitle_drifter' => 'Drifter',
			'cardtitle_fullthrottle' => 'Full Throttle',
			'cardtitle_trackstar' => 'Trackstar',
			'cardtitle_getreal' => 'Get Real',
			'cardtitle_dronekiller' => 'Dronekiller',
			'cardtitle_devastator' => 'Devastator',
			'cardtitle_bowdown' => 'Bow Down',
			'cardtitle_omnipotent' => 'Omnipotent',
			'cardtitle_armyof1' => 'Army of 1',
			'cardtitle_unbelievable' => 'Unbelievable',
			'cardtitle_coldsteel' => 'Cold Steel',
			'cardtitle_omnicide' => 'Omnicide',
			'cardtitle_backsmasher' => 'Backsmasher',
			'cardtitle_headrush' => 'Headrush',
			'cardtitle_streaker' => 'Streaker',
			'cardtitle_theextreme' => 'To the Extreme',
			'cardtitle_allpro' => 'All Pro',
			'cardtitle_freerunner' => 'Freerunner',
			'cardtitle_doubledown' => 'Double Down',
			'cardtitle_gambler' => 'Pro Gambler',
			'cardtitle_prestige7' => 'Lucky 7',
			'cardtitle_dominofx' => 'The Domino Effect',
			'cardtitle_6fears7' => '6fears7',
			'cardtitle_droppincrates' => 'Droppin\' Crates',
			'cardtitle_publicenemy' => 'Public Enemy',
			'cardtitle_feared' => 'The Feared',
			'cardtitle_predator' => 'Predator',
			'cardtitle_accidentprone' => 'Accident Prone',
			'cardtitle_50a' => 'a Major Pain',
			'cardtitle_sbd' => 'SBD',
			'cardtitle_no' => 'No.',
			'cardtitle_crackinskulls' => 'Crackin\' Skulls',
			'cardtitle_owned' => 'owned.',
			'cardtitle_tagyoureit' => 'Tag! You\'re it!',
			'cardtitle_smashhit' => 'Smash Hit',
			'cardtitle_omfg' => 'OMFG',
			'cardtitle_juggernaut' => 'Juggernaut',
			'cardtitle_surgical' => 'Surgical',
			'cardtitle_survivor' => 'Survivor',
			'cardtitle_cloakanddagger' => 'Cloak and Dagger',
			'cardtitle_copperfield' => 'Ta-Da!',
			'cardtitle_quickchange' => 'Quick Change Artist',
			'cardtitle_lightsout' => 'Lights Out!',
			'cardtitle_oneinchpunch' => 'One Inch Punch',
			'cardtitle_prestige5' => 'Five Sided Fistagon',
			'cardtitle_jointops' => 'Joint Ops',
			'cardtitle_40a' => 'High Command',
			'cardtitle_blunttrauma' => 'Blunt Trauma',
			'cardtitle_comfortablynumb' => 'Completely Numb',
			'cardtitle_darkbringer' => 'Darkbringer',
			'cardtitle_sharepackage' => 'Share Package',
			'cardtitle_stuckonyou' => 'Stuck on You',
			'cardtitle_perfectionist' => 'Perfectionist',
			'cardtitle_30a' => '30 Something',
			'cardtitle_voyeur' => 'Voyeur',
			'cardtitle_chickmagnet' => 'Chick Magnet',
			'cardtitle_flasher' => 'The Flasher',
			'cardtitle_silentknight' => 'Silence',
			'cardtitle_rejected' => 'Rejected',
			'cardtitle_klepto' => 'Kleptomaniac',
			'cardtitle_companioncrate' => 'Companion Crate',
			'cardtitle_heart' => '<3',
			'cardtitle_heartbreaker' => 'Heartbreaker',
			'cardtitle_fragout' => 'Frag Out!',
			'cardtitle_catchshrapnel' => 'Catch Shrapnel',
			'cardtitle_pineappleexpress' => 'Pineapple Express',
			'cardtitle_toxicavenger' => 'Toxic',
			'cardtitle_livingdead' => 'Living Dead',
			'cardtitle_lastresort' => 'Last Resort',
			'cardtitle_evildead' => 'Mmmmm Brains...',
			'cardtitle_phoenixrising' => 'Phoenix Rising',
			'cardtitle_infected' => 'Infected',
			'cardtitle_friendswith' => 'Friends With Benefits',
			'cardtitle_uavrays' => 'UAV Rays',
			'cardtitle_mylilpwny' => 'My Li\'l Pwny',
			'cardtitle_martyr' => 'Martyr',
			'cardtitle_enemybenefits' => 'Enemy With Benefits',
			'cardtitle_howthe' => 'How the?',
			'cardtitle_idthief' => 'ID Thief',
			'cardtitle_disruptor' => 'Disruptor',
			'cardtitle_flatliner' => 'Flatliner',
			'cardtitle_tacticaldeletion' => 'TacDel',
			'cardtitle_mastermind' => 'Mastermind',
			'cardtitle_boilermaker' => 'Boilermaker',
			'cardtitle_pushindaisy' => 'Pushin\' Daisies',
			'cardtitle_notintheface' => 'Not in the face!',
			'cardtitle_plague' => 'STD',
			'cardtitle_blingbling' => 'Bling Bling',
			'cardtitle_claypigeon' => 'Clay Pigeon',
			'cardtitle_spirit' => 'Pyrotechnical',
			'cardtitle_straightup' => 'Straight Up',
			'cardtitle_anarchist' => 'The Anarchist',
			'cardtitle_newjack' => 'New Jack',
			'cardtitle_ninja' => 'Ninja',
			'cardtitle_thinkfast' => 'Think Fast!',
			'cardtitle_fullforce' => 'Full Force',
			'cardtitle_squawkbox' => 'Squawk Box',
			'cardtitle_duckhunter' => 'Skeet Shooter',
			'cardtitle_cr1zby' => '3xt4 cr1zby',
			'cardtitle_jackinthebox' => 'Jack-in-the-Box',
			'cardtitle_halfbaked' => 'So Baked',
			'cardtitle_clayback' => 'Dishes are Done',
			'cardtitle_60a' => 'Schooled',
			'cardtitle_noobtuber' => 'Noob Tuber',
			'cardtitle_endofline' => 'End of Line',
			'cardtitle_flag_united_states' => '',
			'cardtitle_flag_uk' => '',
			'cardtitle_flag_canada' => '',
			'cardtitle_flag_australia' => '',
			'cardtitle_flag_spain' => '',
			'cardtitle_flag_austria' => '',
			'cardtitle_flag_japan' => '',
			'cardtitle_flag_brazil' => '',
			'cardtitle_flag_swiss' => '',
			'cardtitle_flag_russia' => '',
			'cardtitle_flag_mexico' => '',
			'cardtitle_flag_france' => '',
			'cardtitle_flag_germany' => '',
			'cardtitle_flag_italy' => '',
			'cardtitle_flag_norway' => '',
			'cardtitle_flag_greece' => '',
			'cardtitle_flag_sweden' => '',
			'cardtitle_flag_ireland' => '',
			'cardtitle_flag_belgium' => '',
			'cardtitle_flag_finland' => '',
			'cardtitle_flag_luxemburg' => '',
			'cardtitle_flag_czech' => '',
			'cardtitle_flag_netherlands' => '',
			'cardtitle_flag_newzealand' => '',
			'cardtitle_flag_philippines' => '',
			'cardtitle_flag_poland' => '',
			'cardtitle_flag_portugal' => '',
			'cardtitle_flag_denmark' => '',
			'cardtitle_flag_saudiarabia' => '',
			'cardtitle_flag_singapore' => '',
			'cardtitle_flag_southafrica' => '',
			'cardtitle_flag_southkorea' => '',
			'cardtitle_flag_hongkong' => '',
			'cardtitle_flag_india' => '',
			'cardtitle_flag_taiwan' => '',
			'cardtitle_flag_united_arab_emirates' => '',
			'cardtitle_flag_elsalvador' => '',
			'cardtitle_flag_china' => '',
			'cardtitle_flag_iran' => '',
			'cardtitle_patriot' => 'Patriot',
			'cardtitle_fng' => 'The Specialist',
			'cardtitle_greatwhite' => 'Great White North',
			'cardtitle_continental' => 'Continental',
			'cardtitle_conquerer' => 'The Conquerer',
			'cardtitle_terminator' => 'The Exterminator',
			'cardtitle_risingsun' => 'Rising Sun',
			'cardtitle_redeemer' => 'The Redeemer',
			'cardtitle_neutral' => 'Neutral',
			'cardtitle_prestige1' => 'The Prestige',
			'cardtitle_prestige2' => 'Round 2',
			'cardtitle_prestige3' => 'Third Time Charm',
			'cardtitle_prestige8' => 'Hard Eight',
			'cardtitle_prestige9' => '9 Lives',
			'cardtitle_prestige10' => 'Prestige 10',
			'cardtitle_famas_marksman' => 'FAMAS',
			'cardtitle_m4_marksman' => 'M4A1',
			'cardtitle_scar_marksman' => 'SCAR-H',
			'cardtitle_ak47_marksman' => 'AK-47',
			'cardtitle_m16_marksman' => 'M16A4',
			'cardtitle_tavor_marksman' => 'TAR-21',
			'cardtitle_fn2000_marksman' => 'F2000',
			'cardtitle_masada_marksman' => 'ACR',
			'cardtitle_fal_marksman' => 'FAL',
			'cardtitle_ump45_marksman' => 'UMP45',
			'cardtitle_mp5k_marksman' => 'MP5K',
			'cardtitle_kriss_marksman' => 'Vector',
			'cardtitle_p90_marksman' => 'P90',
			'cardtitle_uzi_marksman' => 'Mini-Uzi',
			'cardtitle_sa80_marksman' => 'L86 LSW',
			'cardtitle_rpd_marksman' => 'RPD',
			'cardtitle_m240_marksman' => 'M240',
			'cardtitle_aug_marksman' => 'AUG HBAR',
			'cardtitle_mg4_marksman' => 'MG4',
			'cardtitle_barrett_marksman' => 'Barrett .50cal',
			'cardtitle_cheytac_marksman' => 'Intervention',
			'cardtitle_m21_marksman' => 'M21 EBR',
			'cardtitle_wa2000_marksman' => 'WA2000',
			'cardtitle_pp2000_marksman' => 'PP2000',
			'cardtitle_beretta393_marksman' => 'M93 Raffica',
			'cardtitle_glock_marksman' => 'G18',
			'cardtitle_tmp_marksman' => 'TMP',
			'cardtitle_usp_marksman' => 'USP .45',
			'cardtitle_beretta_marksman' => 'M9',
			'cardtitle_deserteagle_marksman' => 'Desert Eagle',
			'cardtitle_coltanaconda_marksman' => '.44 Magnum',
			'cardtitle_m1014_marksman' => 'M1014',
			'cardtitle_aa12_marksman' => 'AA-12',
			'cardtitle_striker_marksman' => 'Striker',
			'cardtitle_model1887_marksman' => 'Model 1887',
			'cardtitle_spas12_marksman' => 'SPAS-12',
			'cardtitle_ranger_marksman' => 'Ranger',
			'cardtitle_at4_marksman' => 'AT4-HS',
			'cardtitle_javelin_marksman' => 'Javelin',
			'cardtitle_m79_marksman' => 'Thumper',
			'cardtitle_rpg_marksman' => 'RPG-7',
			'cardtitle_stinger_marksman' => 'Stinger',
			'cardtitle_famas_expert' => 'FAMAS',
			'cardtitle_m4_expert' => 'M4A1',
			'cardtitle_scar_expert' => 'SCAR-H',
			'cardtitle_ak47_expert' => 'AK-47',
			'cardtitle_m16_expert' => 'M16A4',
			'cardtitle_tavor_expert' => 'TAR-21',
			'cardtitle_fn2000_expert' => 'F2000',
			'cardtitle_masada_expert' => 'ACR',
			'cardtitle_fal_expert' => 'FAL',
			'cardtitle_ump45_expert' => 'UMP45',
			'cardtitle_mp5k_expert' => 'MP5K',
			'cardtitle_kriss_expert' => 'Vector',
			'cardtitle_p90_expert' => 'P90',
			'cardtitle_uzi_expert' => 'Mini-Uzi',
			'cardtitle_sa80_expert' => 'L86 LSW',
			'cardtitle_rpd_expert' => 'RPD',
			'cardtitle_m240_expert' => 'M240',
			'cardtitle_aug_expert' => 'AUG HBAR',
			'cardtitle_mg4_expert' => 'MG4',
			'cardtitle_barrett_expert' => 'Barrett .50cal',
			'cardtitle_cheytac_expert' => 'Intervention',
			'cardtitle_m21_expert' => 'M21 EBR',
			'cardtitle_wa2000_expert' => 'WA2000',
			'cardtitle_pp2000_expert' => 'PP2000',
			'cardtitle_beretta393_expert' => 'M93 Raffica',
			'cardtitle_glock_expert' => 'G18',
			'cardtitle_tmp_expert' => 'TMP',
			'cardtitle_usp_expert' => 'USP .45',
			'cardtitle_beretta_expert' => 'M9',
			'cardtitle_deserteagle_expert' => 'Desert Eagle',
			'cardtitle_coltanaconda_expert' => '.44 Magnum',
			'cardtitle_m1014_expert' => 'M1014',
			'cardtitle_aa12_expert' => 'AA-12',
			'cardtitle_striker_expert' => 'Striker',
			'cardtitle_model1887_expert' => 'Model 1887',
			'cardtitle_spas12_expert' => 'SPAS-12',
			'cardtitle_ranger_expert' => 'Ranger',
			'cardtitle_at4_expert' => 'AT4-HS',
			'cardtitle_javelin_expert' => 'Javelin',
			'cardtitle_m79_expert' => 'Thumper',
			'cardtitle_rpg_expert' => 'RPG-7',
			'cardtitle_stinger_expert' => 'Stinger',
			'cardtitle_famas_sharpshooter' => 'FAMAS',
			'cardtitle_m4_sharpshooter' => 'M4A1',
			'cardtitle_scar_sharpshooter' => 'SCAR-H',
			'cardtitle_ak47_sharpshooter' => 'AK-47',
			'cardtitle_m16_sharpshooter' => 'M16A4',
			'cardtitle_tavor_sharpshooter' => 'TAR-21',
			'cardtitle_fn2000_sharpshooter' => 'F2000',
			'cardtitle_masada_sharpshooter' => 'ACR',
			'cardtitle_fal_sharpshooter' => 'FAL',
			'cardtitle_ump45_sharpshooter' => 'UMP45',
			'cardtitle_mp5k_sharpshooter' => 'MP5K',
			'cardtitle_kriss_sharpshooter' => 'Vector',
			'cardtitle_p90_sharpshooter' => 'P90',
			'cardtitle_uzi_sharpshooter' => 'Mini-Uzi',
			'cardtitle_sa80_sharpshooter' => 'L86 LSW',
			'cardtitle_rpd_sharpshooter' => 'RPD',
			'cardtitle_m240_sharpshooter' => 'M240',
			'cardtitle_aug_sharpshooter' => 'AUG HBAR',
			'cardtitle_mg4_sharpshooter' => 'MG4',
			'cardtitle_barrett_sharpshooter' => 'Barrett .50cal',
			'cardtitle_cheytac_sharpshooter' => 'Intervention',
			'cardtitle_m21_sharpshooter' => 'M21 EBR',
			'cardtitle_wa2000_sharpshooter' => 'WA2000',
			'cardtitle_famas_veteran' => 'FAMAS',
			'cardtitle_m4_veteran' => 'M4A1',
			'cardtitle_scar_veteran' => 'SCAR-H',
			'cardtitle_ak47_veteran' => 'AK-47',
			'cardtitle_m16_veteran' => 'M16A4',
			'cardtitle_tavor_veteran' => 'TAR-21',
			'cardtitle_fn2000_veteran' => 'F2000',
			'cardtitle_masada_veteran' => 'ACR',
			'cardtitle_fal_veteran' => 'FAL',
			'cardtitle_ump45_veteran' => 'UMP45',
			'cardtitle_mp5k_veteran' => 'MP5K',
			'cardtitle_kriss_veteran' => 'Vector',
			'cardtitle_p90_veteran' => 'P90',
			'cardtitle_uzi_veteran' => 'Mini-Uzi',
			'cardtitle_sa80_veteran' => 'L86 LSW',
			'cardtitle_rpd_veteran' => 'RPD',
			'cardtitle_m240_veteran' => 'M240',
			'cardtitle_aug_veteran' => 'AUG HBAR',
			'cardtitle_mg4_veteran' => 'MG4',
			'cardtitle_barrett_veteran' => 'Barrett .50cal',
			'cardtitle_cheytac_veteran' => 'Intervention',
			'cardtitle_m21_veteran' => 'M21 EBR',
			'cardtitle_wa2000_veteran' => 'WA2000',
			'cardtitle_pp2000_veteran' => 'PP2000',
			'cardtitle_beretta393_veteran' => 'M93 Raffica',
			'cardtitle_glock_veteran' => 'G18',
			'cardtitle_tmp_veteran' => 'TMP',
			'cardtitle_usp_veteran' => 'USP .45',
			'cardtitle_beretta_veteran' => 'M9',
			'cardtitle_deserteagle_veteran' => 'Desert Eagle',
			'cardtitle_coltanaconda_veteran' => '.44 Magnum',
			'cardtitle_m1014_veteran' => 'M1014',
			'cardtitle_aa12_veteran' => 'AA-12',
			'cardtitle_striker_veteran' => 'Striker',
			'cardtitle_model1887_veteran' => 'Model 1887',
			'cardtitle_spas12_veteran' => 'SPAS-12',
			'cardtitle_ranger_veteran' => 'Ranger',
			'cardtitle_at4_veteran' => 'AT4-HS',
			'cardtitle_javelin_veteran' => 'Javelin',
			'cardtitle_m79_veteran' => 'Thumper',
			'cardtitle_rpg_veteran' => 'RPG-7',
			'cardtitle_stinger_veteran' => 'Stinger',
			'cardtitle_sentryveteran' => 'Sentry Gun',
			'cardtitle_predatorveteran' => 'Predator Missile',
			'cardtitle_airstrikeveteran' => 'Precision Airstrike',
			'cardtitle_harrierveteran' => 'Harrier Strike',
			'cardtitle_heliveteran' => 'Attack Helicopter',
			'cardtitle_pavelowveteran' => 'Pave Low',
			'cardtitle_stealthveteran' => 'Stealth Bomber',
			'cardtitle_chopperveteran' => 'Chopper Gunner',
			'cardtitle_ac130veteran' => 'AC130',
			'cardtitle_bladeveteran' => 'Blade Veteran',
			'cardtitle_laststandveteran' => 'Stand Veteran',
			'cardtitle_silencerveteran' => 'Silent Veteran',
			'cardtitle_flashveteran' => 'Flash Veteran',
			'cardtitle_stunveteran' => 'Stunner Veteran',
			'cardtitle_famas_master' => 'FAMAS',
			'cardtitle_m4_master' => 'M4A1',
			'cardtitle_scar_master' => 'SCAR-H',
			'cardtitle_ak47_master' => 'AK-47',
			'cardtitle_m16_master' => 'M16A4',
			'cardtitle_tavor_master' => 'TAR-21',
			'cardtitle_fn2000_master' => 'F2000',
			'cardtitle_masada_master' => 'ACR',
			'cardtitle_fal_master' => 'FAL',
			'cardtitle_ump45_master' => 'UMP45',
			'cardtitle_mp5k_master' => 'MP5K',
			'cardtitle_kriss_master' => 'Vector',
			'cardtitle_p90_master' => 'P90',
			'cardtitle_uzi_master' => 'Mini-Uzi',
			'cardtitle_sa80_master' => 'L86 LSW',
			'cardtitle_rpd_master' => 'RPD',
			'cardtitle_m240_master' => 'M240',
			'cardtitle_aug_master' => 'AUG HBAR',
			'cardtitle_mg4_master' => 'MG4',
			'cardtitle_barrett_master' => 'Barrett .50cal',
			'cardtitle_cheytac_master' => 'Intervention',
			'cardtitle_m21_master' => 'M21 EBR',
			'cardtitle_wa2000_master' => 'WA2000',
			'cardtitle_pp2000_master' => 'PP2000',
			'cardtitle_beretta393_master' => 'M93 Raffica',
			'cardtitle_glock_master' => 'G18',
			'cardtitle_tmp_master' => 'TMP',
			'cardtitle_usp_master' => 'USP .45',
			'cardtitle_beretta_master' => 'M9',
			'cardtitle_deserteagle_master' => 'Desert Eagle',
			'cardtitle_coltanaconda_master' => '.44 Magnum',
			'cardtitle_m1014_master' => 'M1014',
			'cardtitle_aa12_master' => 'AA-12',
			'cardtitle_striker_master' => 'Striker',
			'cardtitle_model1887_master' => 'Model 1887',
			'cardtitle_spas12_master' => 'SPAS-12',
			'cardtitle_ranger_master' => 'Ranger',
			'cardtitle_at4_master' => 'AT4-HS',
			'cardtitle_javelin_master' => 'Javelin',
			'cardtitle_m79_master' => 'Thumper',
			'cardtitle_rpg_master' => 'RPG-7',
			'cardtitle_stinger_master' => 'Stinger',
			'cardtitle_sentrymaster' => 'Sentry Gun',
			'cardtitle_predatormaster' => 'Predator Missile',
			'cardtitle_airstrikemaster' => 'Precision Airstrike',
			'cardtitle_harriermaster' => 'Harrier Strike',
			'cardtitle_helimaster' => 'Attack Helicopter',
			'cardtitle_pavelowmaster' => 'Pave Low',
			'cardtitle_stealthmaster' => 'Stealth Bomber',
			'cardtitle_choppermaster' => 'Chopper Gunner',
			'cardtitle_ac130master' => 'AC130',
			'cardtitle_blademaster' => 'Blademaster',
			'cardtitle_laststandmaster' => 'The Stand',
			'cardtitle_silencermaster' => 'The Silent',
			'cardtitle_flashmaster' => 'The Flash',
			'cardtitle_stunmaster' => 'The Stunner',
		);
	}
	
	return $titles[$id];
}

function get_icon_for_experience($experience)
{
	static $rankicon;
	
	if (empty($rankicon))
	{
		$rankicon = array(
			0 => 'cardicon_rank_pvt1.png',
			1 => 'cardicon_rank_pvt1.png',
			2 => 'cardicon_rank_pvt1.png',
			3 => 'cardicon_rank_pvt1.png',
			4 => 'cardicon_rank_pfc1.png',
			5 => 'cardicon_rank_pfc1.png',
			6 => 'cardicon_rank_pfc1.png',
			7 => 'cardicon_rank_spc1.png',
			8 => 'cardicon_rank_spc1.png',
			9 => 'cardicon_rank_spc1.png',
			10 => 'cardicon_rank_cpl1.png',
			11 => 'cardicon_rank_cpl1.png',
			12 => 'cardicon_rank_cpl1.png',
			13 => 'cardicon_rank_sgt1.png',
			14 => 'cardicon_rank_sgt1.png',
			15 => 'cardicon_rank_sgt1.png',
			16 => 'cardicon_rank_ssgt1.png',
			17 => 'cardicon_rank_ssgt1.png',
			18 => 'cardicon_rank_ssgt1.png',
			19 => 'cardicon_rank_sfc1.png',
			20 => 'cardicon_rank_sfc1.png',
			21 => 'cardicon_rank_sfc1.png',
			22 => 'cardicon_rank_msgt1.png',
			23 => 'cardicon_rank_msgt1.png',
			24 => 'cardicon_rank_msgt1.png',
			25 => 'cardicon_rank_1stsgt1.png',
			26 => 'cardicon_rank_1stsgt1.png',
			27 => 'cardicon_rank_1stsgt1.png',
			28 => 'cardicon_rank_sgtmaj1.png',
			29 => 'cardicon_rank_sgtmaj1.png',
			30 => 'cardicon_rank_sgtmaj1.png',
			31 => 'cardicon_rank_cmdsgtmaj1.png',
			32 => 'cardicon_rank_cmdsgtmaj1.png',
			33 => 'cardicon_rank_cmdsgtmaj1.png',
			34 => 'cardicon_rank_2ndlt1.png',
			35 => 'cardicon_rank_2ndlt1.png',
			36 => 'cardicon_rank_2ndlt1.png',
			37 => 'cardicon_rank_1stlt1.png',
			38 => 'cardicon_rank_1stlt1.png',
			39 => 'cardicon_rank_1stlt1.png',
			40 => 'cardicon_rank_capt1.png',
			41 => 'cardicon_rank_capt1.png',
			42 => 'cardicon_rank_capt1.png',
			43 => 'cardicon_rank_maj1.png',
			44 => 'cardicon_rank_maj1.png',
			45 => 'cardicon_rank_maj1.png',
			46 => 'cardicon_rank_ltcol1.png',
			47 => 'cardicon_rank_ltcol1.png',
			48 => 'cardicon_rank_ltcol1.png',
			49 => 'cardicon_rank_ltcol1.png',
			50 => 'cardicon_rank_col1.png',
			51 => 'cardicon_rank_col1.png',
			52 => 'cardicon_rank_col1.png',
			53 => 'cardicon_rank_col1.png',
			54 => 'cardicon_rank_bgen1.png',
			55 => 'cardicon_rank_bgen1.png',
			56 => 'cardicon_rank_bgen1.png',
			57 => 'cardicon_rank_bgen1.png',
			58 => 'cardicon_rank_majgen1.png',
			59 => 'cardicon_rank_majgen1.png',
			60 => 'cardicon_rank_majgen1.png',
			61 => 'cardicon_rank_majgen1.png',
			62 => 'cardicon_rank_ltgen1.png',
			63 => 'cardicon_rank_ltgen1.png',
			64 => 'cardicon_rank_ltgen1.png',
			65 => 'cardicon_rank_ltgen1.png',
			66 => 'cardicon_rank_gen1.png',
			67 => 'cardicon_rank_gen1.png',
			68 => 'cardicon_rank_gen1.png',
			69 => 'cardicon_rank_gen1.png',
			70 => 'cardicon_rank_comm1.png',
		);
	}
	
	return $rankicon[get_rank_for_experience($experience)];
}

function get_rank_for_experience($experience)
{
	static $rankxp;
	
	if (empty($rankxp))
	{
		$rankxp = array(
			1 => 0,
			2 => 500,
			3 => 1700,
			4 => 3600,
			5 => 6200,
			6 => 9500,
			7 => 13500,
			8 => 18200,
			9 => 23600,
			10 => 29700,
			11 => 36500,
			12 => 44300,
			13 => 53100,
			14 => 62900,
			15 => 73700,
			16 => 85500,
			17 => 98300,
			18 => 112100,
			19 => 126900,
			20 => 142700,
			21 => 159500,
			22 => 177300,
			23 => 196100,
			24 => 215900,
			25 => 236700,
			26 => 258500,
			27 => 281300,
			28 => 305100,
			29 => 329900,
			30 => 355700,
			31 => 382700,
			32 => 410900,
			33 => 440300,
			34 => 470900,
			35 => 502700,
			36 => 535700,
			37 => 569900,
			38 => 605300,
			39 => 641900,
			40 => 679700,
			41 => 718700,
			42 => 758900,
			43 => 800300,
			44 => 842900,
			45 => 886700,
			46 => 931700,
			47 => 977900,
			48 => 1025300,
			49 => 1073900,
			50 => 1123700,
			51 => 1175000,
			52 => 1227800,
			53 => 1282100,
			54 => 1337900,
			55 => 1395200,
			56 => 1454000,
			57 => 1514300,
			58 => 1576100,
			59 => 1639400,
			60 => 1704200,
			61 => 1770500,
			62 => 1838300,
			63 => 1907600,
			64 => 1978400,
			65 => 2050700,
			66 => 2124500,
			67 => 2199800,
			68 => 2276600,
			69 => 2354900,
			70 => 2434700
		);
		
		$rankxp = array_reverse($rankxp, true);
	}
	
	foreach ($rankxp as $rank => $xp)
	{
		if ($experience > $xp)
		{
			return $rank;
		}
	}
	
	return 0;
}

function get_profile_fields($user_id)
{
	global $db;

	$sql = 'SELECT *
		FROM ' . PROFILE_FIELDS_DATA_TABLE . '
		WHERE ' . $db->sql_in_set('user_id', array_map('intval', $user_id));
	$result = $db->sql_query($sql);

	$field_data = array();
	while ($row = $db->sql_fetchrow($result))
	{
		$field_data[$row['user_id']] = $row;
	}
	$db->sql_freeresult($result);

	$user_fields = array();

	$fields = array('steamid');
	foreach ($fields as $used_ident)
	{
		foreach ($field_data as $user_id => $row)
		{
			$user_fields[$user_id][$used_ident]['value'] = $row['pf_' . $used_ident];
		}
	}

	return $user_fields;
}

function get_user_stats($user_id)
{
	global $phpbb_root_path, $phpEx, $cache;

	// TODO: change this when game stores user ID
	include_once($phpbb_root_path . 'includes/functions_profile_fields.' . $phpEx);
	$cp = new custom_profile();
	$profile_fields = get_profile_fields($user_id);//$cp->generate_profile_fields_template('grab', $user_id);
	
	$sd = new StructuredData($phpbb_root_path . 'stats/playerdata.xml');
	$data = array();

	$breakNext = false;

	foreach ($profile_fields as $uid => $field)
	{
		//$xuid = $field['steamid']['value'];
		
		if (preg_match('/[^0-9a-fA-F]/', $xuid))
		{
			continue;
		}
		
		if (($udata = $cache->get('_stats_' . $xuid)) === false)
		{
			try
			{
				$database = mysqli_connect('localhost', 'aiw3', 'NRvFafRdHVcUzBRR', 'aiw3_xnp');
				$sql = "SELECT * FROM iw4_profiles WHERE user_id = '" . $user_id . "';";
				
				$result = mysqli_query($database, $sql);
				
				if ($result->num_rows) {
					while($row = $result->fetch_assoc()) {
						$stats_ordered = array(
							"experience" => $result['experience'],
							"prestige"   => $result['presitge'],
							"cardicon"   => $result['cardicon'],
							"cardtitle"  => $result['cardtitle'],
						);
						
						$udata = $statsOrdered;
					}
				}
			}
			catch (Exception $e)
			{
				$udata = array();
			}
			
			$cache->put('_stats_' . $xuid, $udata, 3600);
		}
		
		$data[$uid] = $udata;
		
		if ($breakNext)
		{
			break;
		}
	}
	
	$cache->save();

	return $data;
}

function parse_basic_data($sd)
{
	return array(
		'experience' => $sd->get('experience'),
		'prestige' => $sd->get('prestige'),
		'cardicon' => $sd->get('cardIcon'),
		'cardtitle' => $sd->get('cardTitle')
	);
}

class StructuredData
{
	private $version;
	private $structs;
	private $enums;
	private $enumArrays;
	
	private $dataString;

	public function __construct($file)
	{
		$this->readStructuredData($file);
	}
	
	public function setData($string)
	{
		$this->dataString = $string;
		
		$this->validateVersion();
	}
	
	public function get($path)
	{
		$path = explode('.', $path);
		$item = $this->trace($path);
		
		return $this->readItem($item);
	}
	
	private function readItem($item)
	{
		$offset = $item['offset'] + 4;
	
		switch ($this->getActualType($item['type']))
		{
			case 'int':
				return $this->readInt32($offset);
			case 'short':
				return $this->readInt16($offset);
			case 'byte':
				return $this->readInt8($offset);
			case 'float':
				return $this->readFloat($offset);
			case 'enum':
				if (isset($this->enums[$item['type']]))
				{
					$short = $this->readInt16($offset);
				
					foreach ($this->enums[$item['type']] as $key => $value)
					{
						if ($short == $value)
						{
							return $key;
						}
					}
				}
				
				break;
			case 'string':
				return $this->readString($offset, $item['data']['length']);
			case 'bool':
				$value = $this->readInt8($offset);
			
				if (isset($item['data']['bit']))
				{
					$value >>= ($item['data']['bit']);
					$value &= 1;
				}
				
				return ($value == 1) ? true : false;
		}
		
		return "";
	}
	
	private function trace($path)
	{
		$item = array('type' => 'playerdata', 'offset' => 0);
	
		foreach ($path as $name)
		{
			switch ($this->getActualType($item['type']))
			{
				case 'struct':
					$item = $this->getStructChild($item, $name);
				break;
				case 'indexedarr':
					$item = $this->getArrayIndex($item, $name);
				break;
				case 'enumarr':
					$item = $this->getArrayEnum($item, $name);
				break;
			}
		}
		
		return $item;
	}
	
	private function getArrayEnum($item, $name)
	{
		$found = false;
		$enumArray = $this->enumArrays[$item['type']];
		$enum = $this->enums[$enumArray['enum']];
		
		foreach ($enum as $key => $index)
		{
			if ($key == $name)
			{
				$item['data'] = array();
				$item['type'] = $enumArray['type'];
				
				if ($item['type'] == 'bool')
				{
					$item['offset'] += floor($index / 8);
					$item['data']['bit'] = ($index % 8);
				}
				else
				{
					$item['offset'] += ($index * $enumArray['size']);
				}
				
				$found = true;
				break;
			}
		}
		
		if (!$found)
		{
			throw new Exception("No such item in the specified enum.");
		}
		
		return $item;
	}
	
	private function getArrayIndex($item, $name)
	{
		$index = intval($name);
		
		if ($index >= $item['data']['length'])
		{
			throw new Exception("Index out of indexedarr bounds.");
		}
		
		$childSize = $item['data']['childSize'];
		$item['type'] = $item['data']['child'];
		$item['data'] = array();
		
		if ($item['type'] == 'bool')
		{
			$item['offset'] += floor($index / 8);
			$item['data']['bit'] = ($index % 8);
		}
		else
		{
			$item['offset'] += ($index * $childSize);
		}
		
		return $item;
	}
	
	private function getStructChild($item, $name)
	{
		$found = false;
		$struct = $this->structs[$item['type']];
		foreach ($struct as $key => $child)
		{
			if ($key == $name)
			{
				$item['data'] = array();
				$item['type'] = $child['type'];
				$item['offset'] += $child['offset'];
				
				if (isset($child['length']))
				{
					$item['data']['length'] = $child['length'];
				}
				
				if (isset($child['child']))
				{
					$item['data']['child'] = $child['child'];
					$item['data']['childSize'] = $child['childSize'];
				}
				
				$found = true;
				break;
			}
		}
		
		if (!$found)
		{
			throw new Exception("No such item in the specified struct.");
		}
		
		return $item;
	}
	
	private function getActualType($type)
	{
		if (isset($this->structs[$type]))
		{
			return 'struct';
		}
		else if (isset($this->enumArrays[$type]))
		{
			return 'enumarr';
		}
		else if (isset($this->enums[$type]))
		{
			return 'enum';
		}
		else
		{
			return $type;
		}
	}
	
	private function validateVersion()
	{
		$version = $this->readInt32(4);
		
		if ($version != $this->version)
		{
			throw new Exception("Versions of structured data definition and structured data file do not match.");
		}
	}
	
	private function readStructuredData($file)
	{
		$document = simplexml_load_file($file);
		$this->version = intval($document['version']);
		$this->enums = $this->readEnums($document);
		$this->structs = $this->readStructs($document);
		$this->enumArrays = $this->readEnumArrays($document);
	}
	
	private function readEnums($document)
	{
		$enums = array();
		foreach ($document->enums->enum as $enumElement)
		{
			$enum = array();
			foreach ($enumElement->index as $index)
			{
				$enum[(string)$index['name']] = (string)$index['index'];
			}
			
			$enums[(string)$enumElement['name']] = $enum;
		}
		
		return $enums;
	}	
	
	private function readStructs($document)
	{
		$structs = array();
		foreach ($document->structs->struct as $structElement)
		{
			$struct = array();
			foreach ($structElement->children() as $child)
			{
				$item = array();	
				$item['type'] = $child->getName();
				$item['name'] = (string)$child['name'];
				$item['offset'] = (string)$child['offset'];
				
				if ($item['type'] == 'indexedarr' || $item['type'] == 'string')
				{
					$item['length'] = (string)$child['length'];
				}
				
				$children = $child->children();
				$content = $children[0];
					
				if (!empty($content))
				{
					$item['child'] = $content->getName();
					$item['childSize'] = (string)$content['size'];
				}
				
				$struct[$item['name']] = $item;
			}
			
			$structs[(string)$structElement['name']] = $struct;
		}
		
		return $structs;
	}
	
	private function readEnumArrays($document)
	{
		$enumArrays = array();
		foreach ($document->enumarrays->enumarray as $enumArrayElement)
		{
			$enumArray = array();
			$enumArray['enum'] = (string)$enumArrayElement['enum'];
			
			$child = $enumArrayElement->children();
			$enumArray['type'] = $child[0]->getName();
			$enumArray['size'] = (string)$child[0]['size'];
			
			$enumArrays[(string)$enumArrayElement['name']] = $enumArray;
		}
		
		return $enumArrays;
	}
	
	private function readInt32($offset)
	{
		$els = unpack('l', $this->getBytes($offset, 4));
		return $els[1];
	}
	
	private function readInt16($offset)
	{
		$els = unpack('s', $this->getBytes($offset, 2));
		return $els[1];
	}
	
	private function readInt8($offset)
	{
		$els = unpack('C', $this->getBytes($offset, 2));
		return $els[1];
	}
	
	private function readFloat($offset)
	{
		$els = unpack('f', $this->getBytes($offset, 4));
		return $els[1];
	}
	
	private function readString($offset, $length)
	{
		$string = $this->getBytes($offset, $length);
		$els = explode("\0", $string);
		return $els[0];
	}
	
	private function getBytes($offset, $num)
	{
		return substr($this->dataString, $offset, $num);
	}
}
#include common_scripts\utility;
#include maps\mp\_utility;
#include maps\mp\gametypes\_hud_util;
/*
	Sharpshooter
	Objective: 	Accumulate the most points by killing enemies
	Map ends:	When the score or time limit is reached
	Respawning:	No wait / Away from other players

	Level requirements
	------------------
		Spawnpoints:
			classname		mp_dm_spawn
			All players spawn from these. The spawnpoint chosen is dependent on the current locations of enemies at the time of spawn.
			Players generally spawn away from enemies.

		Spectator Spawnpoints:
			classname		mp_global_intermission
			Spectators spawn from these and intermission is viewed from these positions.
			Atleast one is required, any more and they are randomly chosen between.
*/

/*QUAKED mp_dm_spawn (1.0 0.5 0.0) (-16 -16 0) (16 16 72)
Players spawn away from enemies at one of these positions.*/


main()
{
	maps\mp\gametypes\_globallogic::init();
	maps\mp\gametypes\_callbacksetup::SetupCallbacks();
	maps\mp\gametypes\_globallogic::SetupCallbacks();

	registerTimeLimitDvar( level.gameType, 10, 0, 1440 );
	registerScoreLimitDvar( level.gameType, 3000, 0, 5000 );
	registerWinLimitDvar( level.gameType, 1, 0, 5000 );
	registerRoundLimitDvar( level.gameType, 1, 0, 10 );
	registerNumLivesDvar( level.gameType, 0, 0, 10 );
	registerHalfTimeDvar( level.gameType, 0, 0, 1 );

	level.onPrecacheGameType = ::onPrecacheGameType;
	level.onStartGameType = ::onStartGameType;
	level.getSpawnPoint = ::getSpawnPoint;
	
	SetDvarIfUninitialized( "scr_ss_weapons", "aa12 ak47 at4 aug barrett beretta beretta393 cheytac coltanaconda deserteagle wa2000 fal famas fn2000 glock ump45 kriss m1014 m16 m21 m240 m4 m79 masada mg4 model1887 mp5k p90 pp2000 ranger usp rpd rpg sa80 scar spas12 uzi striker tavor tmp" );
	SetDvarIfUninitialized( "scr_ss_switchtime", 45 );
	
	setDvar("sv_cheats", 1);
	setDvar("scr_game_allowkillcam", 1);
	setDvar("scr_game_hardpoints", 0);
	setDvar("scr_game_matchstarttime", 5);
	setDvar("ui_showEndOfGame", 1);
	setDvar("g_hardcore", 1);
	setDvar("scr_player_maxhealth", 100);
	setDvar("cg_drawCrosshair", 1);
	setDvar("sv_cheats", 0);

	game["dialog"]["gametype"] = "ss";
	
	level.ss = true;
	level.sstime = getDvarInt("scr_ss_switchtime");

	if ( getDvarInt( "camera_thirdPerson" ) )
		game["dialog"]["gametype"] = "thirdp_" + game["dialog"]["gametype"];
	else if ( getDvarInt( "scr_diehard" ) )
		game["dialog"]["gametype"] = "dh_" + game["dialog"]["gametype"];
	else if (getDvarInt( "scr_" + level.gameType + "_promode" ) )
		game["dialog"]["gametype"] = game["dialog"]["gametype"] + "_pro";
	
	level thread onPlayerConnect();

}

onPlayerConnect()
{
	for ( ;; )
	{
		level waittill( "connected", player );
		player thread doConnect();
		player thread onJoinedTeam();
	}
}

onJoinedTeam()
{
	self endon("disconnect");

	for(;;)
	{
		self waittill( "joined_team" );
		self thread onPlayerSpawned();
	}
}

onPlayerSpawned()
{
	self endon("disconnect");

	for(;;)
	{
		self waittill("spawned_player");
		wait 0.01;
		self thread doSpawn();
	}
}

onPrecacheGameType()
{
	precacheShader("cardtitle_bloodsplat");
	precacheShader("specialty_bulletdamage_upgrade"); //
	precacheShader("specialty_commando_upgrade"); //
	precacheShader("specialty_lightweight_upgrade"); //
	precacheShader("specialty_fastreload_upgrade");
	precacheShader("specialty_marathon_upgrade");
	precacheShader("specialty_bulletaccuracy_upgrade");
	precacheShader("hud_icon_wager");
}

onStartGameType()
{
	setClientNameMode("auto_change");

	setObjectiveText( "allies", &"OBJECTIVES_DM" );
	setObjectiveText( "axis", &"OBJECTIVES_DM" );

	if ( level.splitscreen )
	{
		setObjectiveScoreText( "allies", &"OBJECTIVES_DM" );
		setObjectiveScoreText( "axis", &"OBJECTIVES_DM" );
	}
	else
	{
		setObjectiveScoreText( "allies", &"OBJECTIVES_DM_SCORE" );
		setObjectiveScoreText( "axis", &"OBJECTIVES_DM_SCORE" );
	}
	setObjectiveHintText( "allies", &"OBJECTIVES_DM_HINT" );
	setObjectiveHintText( "axis", &"OBJECTIVES_DM_HINT" );

	level.spawnMins = ( 0, 0, 0 );
	level.spawnMaxs = ( 0, 0, 0 );
	maps\mp\gametypes\_spawnlogic::addSpawnPoints( "allies", "mp_dm_spawn" );
	maps\mp\gametypes\_spawnlogic::addSpawnPoints( "axis", "mp_dm_spawn" );
	level.mapCenter = maps\mp\gametypes\_spawnlogic::findBoxCenter( level.spawnMins, level.spawnMaxs );
	setMapCenter( level.mapCenter );
	
	allowed[0] = "dm";
	maps\mp\gametypes\_gameobjects::main(allowed);

	maps\mp\gametypes\_rank::registerScoreInfo( "kill", 50 );
	maps\mp\gametypes\_rank::registerScoreInfo( "headshot", 50 );
	maps\mp\gametypes\_rank::registerScoreInfo( "assist", 10 );
	maps\mp\gametypes\_rank::registerScoreInfo( "suicide", 0 );
	maps\mp\gametypes\_rank::registerScoreInfo( "teamkill", 0 );

	level.ssended = false;
	level thread registerWeapons();
	level thread registerPerks();
	level thread registerAttachments();
	level.currentWeapon = getRandomWeapon();
	level.highestRank = [];
	level.scores = [];
	
	level.QuickMessageToAll = true;
}


getSpawnPoint()
{
	spawnPoints = maps\mp\gametypes\_spawnlogic::getTeamSpawnPoints( self.pers["team"] );
	spawnPoint = maps\mp\gametypes\_spawnlogic::getSpawnpoint_DM( spawnPoints );

	return spawnPoint;
}

doConnect()
{
	self closepopupMenu();
	self closeInGameMenu();
	self notify("menuresponse", "changeclass", "class1");
	self thread maps\mp\gametypes\_bohud::createIcons(self);
	self.ssperks = [];
	self.ssperks[0] = "";
	self.ssperks[1] = "";
	self.ssperks[2] = "";
	self.cperks = 0;
	wait 1.0;
	self thread monitorPos();
}

doSpawn()
{
	
	self.perk1Icon.alpha = 0;
	self.perk2Icon.alpha = 0;
	self.perk3Icon.alpha = 0;
	self.perk1Icon.x = -120;
	self.perk2Icon.x = -120;
	self.perk3Icon.x = -120;
	self.perk1Text.alpha = 0;
	self.perk2Text.alpha = 0;
	self.perk3Text.alpha = 0;
	
	foreach(player in level.players)
	{
		player notify("sentStats");
	}

	self.ssspawn = true;
	
	self takeAllWeapons();
	
	self _clearPerks();
	
	self.ssperks = [];
	self.ssperks[0] = "";
	self.ssperks[1] = "";
	self.ssperks[2] = "";
	self.cperks = 0;
	
	if ( isSubStr( level.currentWeapon, "akimbo" ) )
	{
		self giveWeapon(level.currentWeapon, 0, true);
	}
	else
	{
		self giveWeapon(level.currentWeapon, 0, false);
	}
	self giveMaxAmmo(level.currentWeapon);
	wait 0.2;
	self switchToWeapon(level.currentWeapon);
	
	self.notifying = false;
	self setClientDvar("g_compassShowEnemies", 1);
	self thread monitorAmmo();
	self thread monitorAmmo2();
	self thread monitorLaunchers();
	
}

registerPerks()
{

	level.perkList = [];
	
	level.perkList[0] = spawnStruct();
	level.perkList[0].perk = "Commando"; //specialty_commando_upgrade
	level.perkList[0].perks = [];
	level.perkList[0].perks[0] = "specialty_extendedmelee";
	
	level.perkList[1] = spawnStruct();
	level.perkList[1].perk = "Lightweight"; //specialty_lightweight_upgrade
	level.perkList[1].perks = [];
	level.perkList[1].perks[0] = "specialty_fastsprintrecovery";
	level.perkList[1].perks[1] = "specialty_lightweight";
	
	level.perkList[2] = spawnStruct();
	level.perkList[2].perk = "Sleight of Hand"; //specialty_fastreload_upgrade
	level.perkList[2].perks = [];
	level.perkList[2].perks[0] = "specialty_fastsnipe";
	level.perkList[2].perks[1] = "specialty_fastreload";
	level.perkList[2].perks[2] = "specialty_quickdraw";
	
	level.perkList[3] = spawnStruct();
	level.perkList[3].perk = "Marathon"; //specialty_marathon_upgrade
	level.perkList[3].perks = [];
	level.perkList[3].perks[0] = "specialty_marathon";
	
	level.perkList[4] = spawnStruct();
	level.perkList[4].perk = "Stopping Power"; //specialty_bulletdamage_upgrade
	level.perkList[4].perks = [];
	level.perkList[4].perks[0] = "specialty_bulletdamage";
	level.perkList[4].perks[1] = "specialty_bulletpenetration";

}

registerWeapons()
{

	level.weaponList = StrTok( getDvar("scr_ss_weapons"), " " );
	
	level.currentWeapon = "usp_mp";
	
}

registerAttachments()
{

	level.attList = [];
	
	level.attList[0] = "acog";
	level.attList[1] = "grip";
	level.attList[2] = "gl";
	level.attList[3] = "tactical";
	level.attList[4] = "reflex";
	level.attList[5] = "silencer";
	level.attList[6] = "akimbo";
	level.attList[7] = "thermal";
	level.attList[8] = "shotgun";
	level.attList[9] = "heartbeat";
	level.attList[10] = "eotech";
	level.attList[11] = "fmj";
	level.attList[12] = "xmags";

}

getRandomAttachment(weapon)
{

	randAttach = [];
	
	randAttach[0] = "";
	
	if ( weapon == "berretta" || weapon == "usp" )
	{
		randAttach = [];
		randAttach[0] = level.attList[6];
		randAttach[1] = level.attList[11];
		randAttach[2] = level.attList[5];
		randAttach[3] = level.attList[3];
		randAttach[4] = level.attList[12];
	}
	
	if ( weapon == "deserteagle" || weapon == "coltanaconda" )
	{
		randAttach = [];
		randAttach[0] = level.attList[6];
		randAttach[1] = level.attList[11];
		randAttach[2] = level.attList[3];
	}
	
	if ( weapon == "glock" || weapon == "beretta393" || weapon == "pp2000" || weapon == "tmp" )
	{
		randAttach = [];
		randAttach[0] = level.attList[6];
		randAttach[1] = level.attList[10];
		randAttach[2] = level.attList[11];
		randAttach[3] = level.attList[4];
		randAttach[4] = level.attList[5];
		randAttach[5] = level.attList[12];
	}
	
	if ( weapon == "mp5k" || weapon == "uzi" || weapon == "p90" || weapon == "kriss" || weapon == "ump45" )
	{
		randAttach = [];
		randAttach[0] = level.attList[0];
		randAttach[1] = level.attList[6];
		randAttach[2] = level.attList[10];
		randAttach[3] = level.attList[11];
		randAttach[4] = level.attList[4];
		randAttach[5] = level.attList[5];
		randAttach[6] = level.attList[7];
		randAttach[7] = level.attList[12];
	}
	
	if ( weapon == "ak47" || weapon == "m16" || weapon == "m4" || weapon == "fn2000" || weapon == "masada" || weapon == "famas" || weapon == "fal" || weapon == "scar" || weapon == "tavor" )
	{
		randAttach = [];
		randAttach[0] = level.attList[0];
		randAttach[1] = level.attList[10];
		randAttach[2] = level.attList[11];
		randAttach[3] = level.attList[2];
		randAttach[4] = level.attList[9];
		randAttach[5] = level.attList[4];
		randAttach[6] = level.attList[42];
		randAttach[7] = level.attList[5];
		randAttach[8] = level.attList[7];
		randAttach[9] = level.attList[12];
	}
	
	if ( weapon == "barrett" || weapon == "wa2000" || weapon == "m21" || weapon == "cheytac" )
	{
		randAttach = [];
		randAttach[0] = level.attList[0];
		randAttach[1] = level.attList[11];
		randAttach[2] = level.attList[9];
		randAttach[3] = level.attList[5];
		randAttach[4] = level.attList[7];
		randAttach[5] = level.attList[12];
		randAttach[6] = level.attList[12];
	}
	
	if ( weapon == "ranger" || weapon == "model1887" )
	{
		randAttach = [];
		randAttach[0] = level.attList[6];
		randAttach[1] = level.attList[11];
	}
	
	if ( weapon == "striker" || weapon == "aa12" || weapon == "m1014" )
	{
		randAttach = [];
		randAttach[0] = level.attList[10];
		randAttach[1] = level.attList[11];
		randAttach[2] = level.attList[1];
		randAttach[3] = level.attList[4];
		randAttach[4] = level.attList[5];
		randAttach[5] = level.attList[12];
	}
	
	if ( weapon == "rpd" || weapon == "sa80" || weapon == "mg4" || weapon == "m240" || weapon == "aug" )
	{
		randAttach = [];
		randAttach[0] = level.attList[0];
		randAttach[1] = level.attList[10];
		randAttach[2] = level.attList[11];
		randAttach[3] = level.attList[1];
		randAttach[4] = level.attList[9];
		randAttach[5] = level.attList[4];
		randAttach[6] = level.attList[5];
		randAttach[7] = level.attList[7];
		randAttach[8] = level.attList[12];
	}

	attach = "";

	if ( randAttach.size == 1 )
	{
		attach = "";
	}
	else
	{
		validAtt = getValidAtt();
		if ( validAtt[0] == "none" )
		{
			attach = "";
		}
		else
		{
			attach = randAttach[randomint(randAttach.size)];
			valid = false;
			foreach( att in validAtt )
			{
				if ( attach == att ) 
				{
					valid = true;
				}
			}
			
			i = 0;
			
			while( valid == false && i < 10 )
			{
				i += 1;
			
				attach = randAttach[randomint(randAttach.size)];
				foreach( att in validAtt )
				{
					if ( attach == att ) 
					{
						valid = true;
					}
				}
				wait 0.01;
			}
		}
	}
	
	if ( attach == "" )
	{
		return weapon + "_mp";
	}
	else
	{
		return weapon + "_" + attach + "_mp";
	}
	
}

getValidAtt()
{

	attCat = [];
	
	attCat[0] = "none";
	attCat[1] = "sight";
	attCat[3] = "sight";
	attCat[4] = "sight";
	attCat[5] = "sight";
	attCat[6] = "sight";
	attCat[7] = "sight";
	attCat[8] = "sight";
	attCat[9] = "other";
	attCat[10] = "other";
	attCat[11] = "other";
	attCat[12] = "final";
	
	cat = attCat[randomint(attCat.size)];
	
	validAtt = [];
	
	if ( cat == "none" )
	{
		validAtt = [];
		validAtt[0] = "none";
	}
	
	if ( cat == "sight" )
	{
		validAtt = [];
		validAtt[0] = "acog";
		validAtt[0] = "reflex";
		validAtt[0] = "eotech";
	}
	
	if ( cat == "other" )
	{
		validAtt = [];
		validAtt[0] = "grip";
		validAtt[0] = "gl";
		validAtt[0] = "tactical";
		validAtt[0] = "silencer";
		validAtt[0] = "akimbo";
		validAtt[0] = "shotgun";
		validAtt[0] = "fmj";
		validAtt[0] = "xmags";
	}
	
	if ( cat == "final" )
	{
		validAtt = [];
		validAtt[0] = "thermal";
		validAtt[0] = "heartbeat";
	}
	
	return validAtt;

}

getSetRandomPerk()
{

	if ( self.cperks < 3 )
	{
		
		randPerk = level.perkList[randomint(level.perkList.size)].perk;
		
		used = false;

		
		foreach( perk in self.ssperks )
		{
			if ( perk == randPerk )
			{
				used = true;
			}
		}
		
		while(used == true)
		{
			used = false;
			randPerk = level.perkList[randomint(level.perkList.size)].perk;
			foreach( perk in self.ssperks )
			{
				if ( perk == randPerk )
				{
					used = true;
				}
			}
			wait 0.01;
		}
		
		perkNum = 0;
		
		for( i = 0; i < level.perkList.size; i++ )
		{
			if ( level.perkList[i].perk == randPerk )
			{
				perkNum = i;
			}
		}
		
		foreach( speciality in level.perkList[perkNum].perks )
		{
			self _setPerk( speciality );
		}
		
		self.ssperks[self.cperks] = randPerk;
		self.cperks += 1;
		self thread maps\mp\gametypes\_bohud::perkAnim(self, randPerk);
	}
	else
	{
		// TO-DO: Score Multiplier
	}
	
}

getRandomWeapon()
{

	curWeap = level.currentWeapon;
	
	randWeap = level.weaponList[randomint(level.weaponList.size)];
	
	used = false;

	if ( !isDefined(level.usedWeaps) )
	{
		level.usedWeaps = [];
		level.usedWeaps[0] = randWeap;
	}
	else
	{
		if ( level.usedWeaps.size == level.weaponList.size )
		{
			level.usedWeaps = [];
			used = false;
		}
		else
		{
			foreach( wep in level.usedWeaps )
			{
				if ( wep == randWeap )
				{
					used = true;
				}
			}
		}
	}
	
	while(used == true)
	{
		used = false;
		randWeap = level.weaponList[randomint(level.weaponList.size)];
		foreach( wep in level.usedWeaps )
		{
			if ( wep == randWeap )
			{
				used = true;
			}
		}
		wait 0.01;
	}
	
	level.usedWeaps[level.usedWeaps.size] = randWeap;
	
	finalWeap = getRandomAttachment(randWeap);
		
	return finalWeap;
	
}

weaponSwitchLogic(first)
{

	self endon("ssended");
	self endon("disconnect");
	
	while(1)
	{
		if ( first == true )
		{
			first = false;
		}
		else
		{
			level.currentWeapon = getRandomWeapon();
			
			logPrint("------------------------------------------------------------\n");
			logPrint("Switching weapon to: " + level.currentWeapon + "\n");
			logPrint("------------------------------------------------------------\n");
			
			level thread switchWpn();
			if ( !isDefined(level.switchtime) )
			{
				level.switchtime = getDvarInt("scr_ss_switchtime");
			}
			else
			{
				tempTime = getDvarInt("scr_ss_switchtime");
				if ( tempTime != level.switchtime )
				{
					level.switchtime = tempTime;
				}
			}
		}
		level.sstime = level.switchtime;
		wait (level.switchtime + 0.35);	
	}

}

switchWpn()
{

	foreach ( player in level.players )
	{
		curWeap = "";
	
		player takeAllWeapons();
		if ( isSubStr( level.currentWeapon, "akimbo" ) )
		{
			player giveWeapon(level.currentWeapon, 0, true);
		}
		else
		{
			player giveWeapon(level.currentWeapon, 0, false);
		}
		player giveMaxAmmo(level.currentWeapon);
		wait 0.2;
		player switchToWeapon(level.currentWeapon);
	}

}

timerStuff()
{

	self endon("ssended");
	self endon("disconnect");
	
	while(1)
	{
		
		foreach(player in level.players)
		{
			if ( level.sstime > 9 )
			{
				player.aliveText setText("Weapons cycling in 0:" + level.sstime);
			}
			else if ( level.sstime < 1 )
			{
				player.aliveText setText("Weapons cycling in 0:00");
			}
			else
			{
				player.aliveText setText("Weapons cycling in 0:0" + level.sstime);
			}
		}
		level.sstime -= 1;
		wait 1.0;
	
	}

}

monitorAmmo()
{
	self endon("death");
	self endon("disconnect");
	while(1)
	{
		self thread updateHUDAmmo();

		wait 0.3;
	}
}

monitorLaunchers()
{

	self endon("disconnect");
	self endon("death");
	
	while(1)
	{
		curWeap = self getCurrentWeapon();
		if ( curWeap == "at4_mp" || curWeap == "m79_mp" || curWeap == "rpg_mp" )
		{
			self giveMaxAmmo(curWeap);
		}
		wait 3.5;
	}

}

monitorAmmo2()
{
	self endon("death");
	self endon("disconnect");
	while(1)
	{
		self waittill("weapon_fired");
		self thread updateHUDAmmo();

		wait 0.01;
	}
}

monitorPos()
{

	self endon("disconnect");
	
	while(1)
	{
		self waittill("sentStats");
		g = 0;
		there = false;
		maps\mp\gametypes\_gamescore::updatePlacement();
		foreach(player in level.players)
		{
			if ( player.score == 0 )
			{
				g += 1;
			}
			if ( player == self && player.score == 0 )
			{
				there = true;
			}
		}
		if ( there == true )
		{
			if ( g > 9 )
			{
				self.rankText.x = self.rankX - 13;
				self.rankShadow.x = self.rankX - 14;
				self.rankShadow2.x = self.rankX - 12;
			}
			else
			{
				self.rankText.x = self.rankX;
				self.rankShadow.x = self.rankX - 1;
				self.rankShadow2.x = self.rankX + 1;
			}
			self.rankText setText( g );
			self.rankShadow setText( "^0" + g );
			self.rankShadow2 setText( "^0" + g );
		}
		else
		{
			for(i = 0; i < level.placement["all"].size; i++)
			{
				if(level.placement["all"][i] == self)
				{
					s = i + 1;
					if ( s > 9 )
					{
						self.rankText.x = self.rankX - 13;
						self.rankShadow.x = self.rankX - 14;
						self.rankShadow2.x = self.rankX - 12;
					}
					else
					{
						self.rankText.x = self.rankX;
						self.rankShadow.x = self.rankX - 1;
						self.rankShadow2.x = self.rankX + 1;
					}
					self.rankText setText( s );
					self.rankShadow setText( "^0" + s );
					self.rankShadow2 setText( "^0" + s );
				}
			}
		}
		wait 0.05;
	}

}

updateHUDAmmo()
{

	if ( isSubStr( self getCurrentWeapon(), "akimbo" ) )
	{
		self.ammoClip2.alpha = 1.0;
		self.ammoSep.alpha = 0.35;
		self.ammoClip1 setText(self GetWeaponAmmoClip( self getCurrentWeapon(), "right" ));
		self.ammoClip2 setText(self GetWeaponAmmoClip( self getCurrentWeapon(), "left" ));
		self.ammoStock setText(self getWeaponAmmoStock( self getCurrentWeapon() ));
	}
	else
	{
		self.ammoClip2.alpha = 0;
		self.ammoSep.alpha = 0;
		self.ammoClip1 setText(self GetWeaponAmmoClip( self getCurrentWeapon() ));
		self.ammoStock setText(self getWeaponAmmoStock( self getCurrentWeapon() ));
	}

}

killedEnemy( means )
{

	if ( self.score == 1950 )
	{
		level.ssended = true;
		level.sstime = 0;
		self thread endGameFinal();
	}
	foreach(player in level.players)
	{
		player notify("sentStats");
	}
	
	self thread getSetRandomPerk();
	wait 0.5;
	self.scoreText setText(self.score);
	
}

endGameFinal()
{

	foreach(player in level.players)
	{
		player notify("ssended");
		player.aliveText setText("Weapons cycling in 0:00");
	}
	self notify("ssended");
	
	self thread maps\mp\gametypes\_gamelogic::endGame(level.finalRanking[1], "", false);

}

gotKilled( means, suicide )
{

	self notify("iDie");
	foreach(player in level.players)
	{
		player notify("sentStats");
	}
	wait 0.5;
	self.scoreText setText(self.score);

}
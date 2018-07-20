#include common_scripts\utility;
#include maps\mp\_utility;
#include maps\mp\gametypes\_hud_util;
/*
	Gun Game
	Objective: 	Progress through all weapon tiers by killing enemy players
	Map ends:	When a player progresses through all weapon tiers
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

	registerTimeLimitDvar( level.gameType, 0, 0, 1440 );
	registerScoreLimitDvar( level.gameType, 0, 0, 5000 );
	registerWinLimitDvar( level.gameType, 1, 0, 5000 );
	registerRoundLimitDvar( level.gameType, 1, 0, 10 );
	registerNumLivesDvar( level.gameType, 0, 0, 10 );
	registerHalfTimeDvar( level.gameType, 0, 0, 1 );

	level.onPrecacheGameType = ::onPrecacheGameType;
	level.onStartGameType = ::onStartGameType;
	level.getSpawnPoint = ::getSpawnPoint;
	
	setDvar("sv_cheats", 1);
	setDvar("scr_game_allowkillcam", 1);
	setDvar("scr_game_hardpoints", 0);
	setDvar("scr_game_matchstarttime", 5);
	setDvar("ui_showEndOfGame", 1);
	setDvar("g_hardcore", 1);
	setDvar("cg_drawCrosshair", 1);
	setDvar("scr_dm_scorelimit", 0);
	setDvar("sv_cheats", 0);

	game["dialog"]["gametype"] = "gg";

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

	level.ggended = false;
	level thread registerWeapons();
	level.highestRank = [];
	level.scores = [];
	level thread doRanking();
	
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
	self.tier = 1;
	wait 1.0;
	self thread monitorPos();
}

doSpawn()
{
	self.score = self.tier;
	
	foreach (player in level.players)
	{
		player UpdateDMScores();
	}

	self.aliveText.alpha = 0;

	self.ggspawn = true;
	
	self takeAllWeapons();
	self.scoreText setText("Weapon Tier: " + self.tier + " of 20");
	if ( self.tier == 2 || self.tier == 8 )
	{
		self giveWeapon(level.weaponList[self.tier], 0, true);
	}
	else
	{
		self giveWeapon(level.weaponList[self.tier], 0, false);
	}
	self giveMaxAmmo(level.weaponList[self.tier]);
	wait 0.2;
	self switchToWeapon(level.weaponList[self.tier]);
	
	self _clearPerks();
	self.notifying = false;
	self setClientDvar("g_compassShowEnemies", 1);
	self thread monitorAmmo();
	self thread monitorAmmo2();
	self thread monitorKnives();
	self thread sillyWeaponCheck();
	
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

monitorKnives()
{

	self endon("disconnect");
	self endon("death");
	
	while(1)
	{
		if ( self.tier == 20 )
		{
			wait 1.5;
			self giveMaxAmmo(level.weaponList[self.tier]);
		}
		wait 0.01;
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
		for(i = 1; i < level.finalRanking.size + 1; i++)
		{
			if(level.finalRanking[i].name == self.name)
			{
				self.rankText setText( i );
				self.rankShadow setText( "^0" + i );
				self.rankShadow2 setText( "^0" + i );
			}
		}
		wait 0.05;
	}

}

updateHUDAmmo()
{

	if ( self.tier == 2 || self.tier == 8 )
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

registerWeapons()
{

	level.weaponList = [];
	
	level.weaponList[1] = "coltanaconda_mp";
	level.weaponList[2] = "beretta_akimbo_mp";
	level.weaponList[3] = "beretta393_mp";
	level.weaponList[4] = "spas12_mp";
	level.weaponList[5] = "m1014_mp";
	level.weaponList[6] = "uzi_mp";
	level.weaponList[7] = "mp5k_mp";
	level.weaponList[8] = "tmp_akimbo_mp";
	level.weaponList[9] = "famas_mp";
	level.weaponList[10] = "scar_mp";
	level.weaponList[11] = "fal_mp";
	level.weaponList[12] = "aug_mp";
	level.weaponList[13] = "m240_mp";
	level.weaponList[14] = "m21_mp";
	level.weaponList[15] = "cheytac_mp";
	level.weaponList[16] = "barrett_mp";
	level.weaponList[17] = "at4_mp";
	level.weaponList[18] = "m79_mp";
	level.weaponList[19] = "rpg_mp";
	level.weaponList[20] = "throwingknife_mp";

}

sillyWeaponCheck()
{
	self endon("death");
	self endon("disconnect");

	while(1)
	{
		if ( self getCurrentWeapon() != level.weaponList[self.tier] )
		{
			self takeAllWeapons();
			if ( self.tier == 2 || self.tier == 8 )
			{
				self giveWeapon(level.weaponList[self.tier], 0, true);
			}
			else
			{
				self giveWeapon(level.weaponList[self.tier], 0, false);
			}
			self giveMaxAmmo(level.weaponList[self.tier]);
			wait 0.2;
			self switchToWeapon(level.weaponList[self.tier]);
		}
		wait 0.1;
	}

}

killedEnemy( means )
{

	if ( means == "MOD_MELEE" )
	{
		self thread maps\mp\gametypes\_bohud::noticeAnim("Humiliation", "Demoted enemy!");
	}
	else
	{
		if ( self.tier == 20 )
		{
			level.ggended = true;
			self thread endGameFinal();
		}
		else
		{
			self.tier += 1;
			self thread maps\mp\gametypes\_bohud::noticeAnim("Player Killed", "Advanced to next tier!");
			self.scoreText setText("Weapon Tier: " + self.tier + " of 20");
			wait 0.25;
			self takeAllWeapons();
			if ( self.tier == 2 || self.tier == 8 )
			{
				self giveWeapon(level.weaponList[self.tier], 0, true);
			}
			else
			{
				self giveWeapon(level.weaponList[self.tier], 0, false);
			}
			self giveMaxAmmo(level.weaponList[self.tier]);
			wait 0.2;
			self switchToWeapon(level.weaponList[self.tier]);
		}
	}
}

endGameFinal()
{

	self thread doRanking();
	self notify("gameEnded");
	
	self thread maps\mp\gametypes\_gamelogic::endGame(level.finalRanking[1], "", false);

}

gotKilled( means, suicide )
{
	
	if ( means == "MOD_MELEE" && self.tier > 1 )
	{
		self thread maps\mp\gametypes\_bohud::noticeAnim("Humiliated", "Demoted!");
		self.tier -= 1;
		self.scoreText setText("Weapon Tier: " + self.tier + " of 20");
		self.score = self.tier;
	
		foreach (player in level.players)
		{
			player UpdateDMScores();
		}
	}
	if ( suicide && self.tier > 1 )
	{
		self.tier -= 1;
		self.scoreText setText("Weapon Tier: " + self.tier + " of 20");
		self.score = self.tier;
	
		foreach (player in level.players)
		{
			player UpdateDMScores();
		}
	}

}

addPlayer(rplayer, score)
{
	player = spawnstruct();
	player.player = rplayer;
	player.score = score;
	return player;
}


sortScore(array, start, end)
{
	i = start;
	k = end;

	if (end - start >= 1)
    {
        pivot = array[start].score;  

        while (k > i)         
        {
	        while (array[i].score <= pivot && i <= end && k > i)  
	        	i++;                                 
	        while (array[k].score > pivot && k >= start && k >= i) 
	            k--;                                      
	        if (k > i)                                 
	           array = swapArr(array, i, k);                    
        }
        array = swapArr(array, start, k);                                               
        array = sortScore(array, start, k - 1); 
        array = sortScore(array, k + 1, end);   
    }
	else
    	return array;
    
    return array;
}

swapArr(array, index1, index2) 
{
	temp = array[index1];          
	array[index1] = array[index2];     
	array[index2] = temp;   
	return array;         
}


doRanking() 
{ 

	self endon("disconnect");
	self endon("gameEnded");
	
	while(1)
	{

		level.scores = [];
		level.preFinalRanking = [];
		level.finalRanking = [];
		i = 0;
		h = 1;
		tplayer = false;
				
		exists = false;
				
		foreach ( player in level.players )
		{
			level.scores[i] = addPlayer(player, player.tier);
			
			i += 1;
		}
		
		level.preFinalRanking = self thread sortScore(level.scores, 0, level.scores.size);

		for(e = level.preFinalRanking.size - 1; e > -1; e--)
		{
				level.finalRanking[h] = level.preFinalRanking[e].player;
				h += 1;
		}
		
		foreach( player in level.players )
		{
			player notify("sentStats");
		}
		wait 1.0;
	}
	
}
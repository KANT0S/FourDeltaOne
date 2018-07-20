#include common_scripts\utility;
#include maps\mp\_utility;
#include maps\mp\gametypes\_hud_util;
/*
	One In The Chamber
	Objective: 	Stay alive for as much as you can while eliminating enemy players
	Map ends:	When only one player is alive
	Respawning:	Every player spawns with 3 lives

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
	
	setDvar("sv_cheats", 1);
	setDvar("scr_dm_numlives", 3);
	setDvar("scr_player_maxhealth", 20);
	setDvar("scr_game_allowkillcam", 1);
	setDvar("scr_game_hardpoints", 0);
	setDvar("scr_game_matchstarttime", 15);
	setDvar("ui_showEndOfGame", 1);
	setDvar("g_hardcore", 1);
	setDvar("sv_cheats", 0);
	
	maps\mp\gametypes\_globallogic::init();
	maps\mp\gametypes\_callbacksetup::SetupCallbacks();
	maps\mp\gametypes\_globallogic::SetupCallbacks();

	registerTimeLimitDvar( level.gameType, 0, 0, 1440 );
	registerScoreLimitDvar( level.gameType, 0, 0, 5000 );
	registerWinLimitDvar( level.gameType, 1, 0, 5000 );
	registerRoundLimitDvar( level.gameType, 1, 0, 10 );
	registerNumLivesDvar( level.gameType, 3, 0, 10 );
	registerHalfTimeDvar( level.gameType, 0, 0, 1 );

	level.onPrecacheGameType = ::onPrecacheGameType;
	level.onStartGameType = ::onStartGameType;
	level.getSpawnPoint = ::getSpawnPoint;

	game["dialog"]["gametype"] = "oitc";

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

		player thread onJoinedTeam();
	}
}

onJoinedTeam()
{
	self endon("disconnect");

	for(;;)
	{
		self waittill( "joined_team" );
		self thread doConnect();
		self thread onPlayerSpawned();
	}
}

onPlayerSpawned()
{
	self endon("disconnect");

	for(;;)
	{
		self waittill("spawned_player");
		self thread doSpawn();
	}
}

onPrecacheGameType()
{
	precacheShader("cardtitle_bullet");
	precacheShader("cardtitle_bloodsplat");
	precacheShader("hud_icon_wager");
	precacheShader("hud_status_dead");
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
	
	level.oitc = true;
	self thread keepCountUpToDate();
	self thread checkUAV();
	self thread monitorFirstPos();
	
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
	self thread monitorPlayerCount();
}

doSpawn()
{
	self.bospawn = true;
	self takeAllWeapons();
	self _clearPerks();
	self.maxhealth = 20;
	self.health = 20;
	self.notifying = false;
	self.gameRank = level.players.size;
	self SetClientDvar( "lowAmmoWarningColor1", "0 0 0 0" );
	self SetClientDvar( "lowAmmoWarningColor2", "0 0 0 0" );
	self SetClientDvar( "lowAmmoWarningNoAmmoColor1", "0 0 0 0" );
	self SetClientDvar( "lowAmmoWarningNoAmmoColor2", "0 0 0 0" );
	self SetClientDvar( "lowAmmoWarningNoReloadColor1", "0 0 0 0" );
	self SetClientDvar( "lowAmmoWarningNoReloadColor2", "0 0 0 0" );
	self setClientDvar("g_compassShowEnemies", 1);
	self giveWeapon( "beretta_mp", 0, false );
	self setWeaponAmmoClip( "beretta_mp", 1 );
	self setWeaponAmmoStock( "beretta_mp", 0 );
	wait 0.1;
	self switchToWeapon("beretta_mp");
	self thread waitForKill();
	self thread noStockAmmo();
	self thread monitorIcons();

}


keepCountUpToDate()
{
	self endon("disconnect");
	while(1)
	{
		level.alivePPL = 0;
		foreach ( player in level.players )
		{
			if ( !player.pers["lives"] && gameHasStarted() && !isAlive(player) )
			{
			}
			else
			{
				if ( player.team != "spectator" && isDefined(player.bospawn) )
				{
					level.alivePPL += 1;
				}
			}
		}
		wait 0.05;
	}
}

checkUAV()
{
	self endon("disconnect");
	wait 45.0;
	while(1)
	{
		if ( level.alivePPL < 4 )
		{
			foreach ( player in level.players )
			{
				if ( player.hasUAV != 1 )
				{
					player.hasUAV = 1;
					player thread maps\mp\gametypes\_bohud::noticeAnim("Heads Up", "UAV Online");
					player thread maps\mp\killstreaks\_uav::launchUAV(player, player.team, 99999, false);
				}
			}
		}
		wait 0.05;
	}
}

gotKilled()
{
	if (!isDefined(self.deathCount))
	{
		self.deathCount = 1;
	}
	else
	{
		self.deathCount += 1;
	}
	if ( self.deathCount == 3 )
	{
		self notify("deadDead");
		self.gameRank = level.alivePPL;
		self.rankText setText(level.alivePPL);
	}
}

monitorPlayerCount()
{

	self endon("disconnect");
	self endon("deadDead");
	
	while(1)
	{
		self.scoreText setText(self.score);
		self.aliveText setText( (level.alivePPL - 1) + " Alive" );
		if ( level.alivePPL > 9 )
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
		self.rankText setText( level.alivePPL );
		self.rankShadow setText( "^0" + level.alivePPL );
		self.rankShadow2 setText( "^0" + level.alivePPL );
		wait 0.5;
	}
}

monitorFirstPos()
{
	self endon("disconnect");
	wait 35.0;
	
	while(1)
	{
		if ( level.alivePPL == 1 )
		{
			foreach ( player in level.players )
			if (isAlive(player))
			{
				player.gameRank = 1;
			}
		}
		wait 0.05;
	}
}

setPlayerRanking()
{
	level.finalRanking = [];
	for( i = 1; i < level.players.size + 1; i++ )
	{
		foreach ( player in level.players )
		{
			if ( isDefined(player.gameRank) && player.gameRank == i && player.team != "spectator" )
			{
				level.finalRanking[i] = player;
			}
		}
	}
}

monitorIcons()
{
	self endon("death");
	self endon("disconnect");
	
	while(1)
	{
		for ( i = 1; i < self getWeaponAmmoClip( "beretta_mp" ) + 1; i++ )
		{
			self.bulletIcon[i].alpha = 1.0;
		}
		for ( i = 15; i > self getWeaponAmmoClip( "beretta_mp" ); i-- )
		{
			self.bulletIcon[i].alpha = 0;
		}
		if ( self.pers["lives"] == 2 )
		{
			self.life1Icon.alpha = 1.0;
			self.life2Icon.alpha = 1.0;
			self.life3Icon.alpha = 1.0;
		}
		if ( self.pers["lives"] == 1 )
		{
			self.life1Icon.alpha = 0;
			self.life2Icon.alpha = 1.0;
			self.life3Icon.alpha = 1.0;
		}
		if ( self.pers["lives"] == 0 )
		{
			self.life1Icon.alpha = 0;
			self.life2Icon.alpha = 0;
			self.life3Icon.alpha = 1.0;
		}
		if ( self.pers["lives"] != 2 && self.pers["lives"] != 1 && self.pers["lives"] != 0 )
		{
			self.life1Icon.alpha = 0;
			self.life2Icon.alpha = 0;
			self.life3Icon.alpha = 0;
		}
		wait 0.05;
	}
}

noStockAmmo()
{
	self endon("death");
	self endon("disconnect");
	
	while(1)
	{
		self setWeaponAmmoStock( "beretta_mp", 0 );
		wait 0.05;
	}
}

waitForKill()
{
	self endon("death");
	self endon("disconnect");
	
	while(1)
	{
		self waittill("killed_enemy");
		self thread maps\mp\gametypes\_bohud::noticeAnim("Player Killed", "+1 Bullet");
		if ( self getWeaponAmmoClip( "beretta_mp" ) != 15 )
		{
			self setWeaponAmmoClip( "beretta_mp", self getWeaponAmmoClip( "beretta_mp" ) + 1 );
		}
		else
		{
			self setWeaponAmmoClip( "beretta_mp", 15 );
		}
		wait 0.05;
	}
}
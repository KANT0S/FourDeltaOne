#include common_scripts\utility;

main()
{
	//precacheItem("fake_mp");

	maps\mp\_load::main();

	//createSpawnpoint( "mp_tdm_spawn_allies_start", (33, 15, -243), 0.0 );
	//createSpawnpoint( "mp_tdm_spawn_axis_start", (770, 954, -233), 0.0 );
	//createSpawnpoint( "mp_tdm_spawn", (770, 954, -233), 0.0 );
	//createSpawnpoint( "mp_tdm_spawn", (33, 15, -243), 0.0 );

	game[ "attackers" ] = "allies";
	game[ "defenders" ] = "axis";

	maps\mp\_compass::setupMiniMap( "compass_map_oilrig_lvl_3" );
	setdvar( "compassmaxrange", "4000" );

	array_thread( getentarray( "breach_solid", "targetname" ), ::self_delete );

	array_thread( getentarray( "compassTriggers", "targetname" ), ::compass_triggers_think );
	thread killtrigger_ocean_on();
	thread custom_killtrigger();
	thread level_think();
	thread exploding_barrels();
}

self_delete()
{
	self delete();
}

exploding_barrels()
{
	barrelPosition = (661, 1456, -241);
	level.barrelsExploded = false;

	while (!level.barrelsExploded)
	{
		wait 0.1;

		foreach (player in level.players)
		{
			if (!isDefined(player) || !isPlayer(player))
			{
				continue;
			}
			
			if (distance(player.origin, barrelPosition) < 75)
			{
				if (player useButtonPressed())
				{
					player explodeBarrels();
				}
			}
		}
	}
}

explodeBarrels()
{
	level.barrelsExploded = true;

	barrels = getEntArray("c4_barrel", "targetname");
	
	foreach (barrel in barrels)
	{
		if (barrel.origin[2] > -350.0)
		{
			barrel kaboom(self);
		}
	}
}

kaboom(player)
{
	self playSound("oilrig_ambush_explosion");
	//effect = loadFX("explosions/small_vehicle_explosion");
	level._effect[ "ambush_explosion_room" ] = loadfx( "explosions/room_explosion_oilrig" );
	playFX(level._effect["ambush_explosion_room"], self.origin);
	earthquake(0.3, 1, self.origin, 300);
	radiusDamage(self.origin, 150, 100, 60, player);

	//self delete();
}

custom_killtrigger()
{
	while (true)
	{
		wait 0.1;

		foreach (player in level.players)
		{
			if (!isDefined(player) || !isPlayer(player))
			{
				continue;
			}

			if (player.origin[2] < -350.0)
			{
				player suicide();
			}
		}
	}
}

killtrigger_ocean_on()
{
	killtrigger_ocean = getent( "killtrigger_ocean", "targetname" );
	assert( isdefined( killtrigger_ocean ) );
	killtrigger_ocean notify( "turn_off" );
	killtrigger_ocean endon( "turn_off" );
	while( true )
	{
		killtrigger_ocean waittill( "trigger", other );
		if ( ( isdefined( other ) ) && ( isPlayer ( other ) ) )
		{
			other suicide();
		}
	}
}

compass_triggers_think()
{
	assertex( isdefined( self.script_noteworthy ), "compassTrigger at " + self.origin + " needs to have a script_noteworthy with the name of the minimap to use" );
	while( true )
	{
		wait( 1 );
		self waittill( "trigger" );
		maps\mp\_compass::setupMiniMap( self.script_noteworthy );
	}
}

createSpawnpoint( classname, origin, yaw )
{
	spawnpoint = spawn( "script_origin", origin );
	spawnpoint.angles = (0,yaw,0);
	
	if ( !isdefined( level.extraspawnpoints ) )
		level.extraspawnpoints = [];
	if ( !isdefined( level.extraspawnpoints[classname] ) )
		level.extraspawnpoints[classname] = [];
	level.extraspawnpoints[classname][ level.extraspawnpoints[classname].size ] = spawnpoint;
}

level_think()
{

	/*-----------------------
	MOVING DERRICK DRILL THING
	-------------------------*/
	eDerrick_thing = getent( "derrick_thing", "targetname" );
	eDerrick_thing.origin = eDerrick_thing.origin + ( 0, 0, -2816 );
	assert( isdefined( eDerrick_thing ) );
	time = 2;
	speed = 300;
	while ( true )
	{
		eDerrick_thing rotatevelocity( ( 0, speed, 0 ), time );
		wait( time );
	}

}
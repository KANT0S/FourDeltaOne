#include common_scripts\utility;
 
main()
{
        maps\mp\_load::main();
 
/*      createSpawnpoint( "mp_tdm_spawn_axis_start", (-1465, -2051, 1870), 0.0 );
        createSpawnpoint( "mp_tdm_spawn_allies_start", (-2667, 1136, 1869), 0.0 );
        createSpawnpoint( "mp_tdm_spawn", (-2667, 1136, 1869), 0.0 );
        createSpawnpoint( "mp_tdm_spawn", (-1576, -1922, 1869), 0.0 ); */
 
		maps\mp\gulag_fx::main();
		maps\createfx\gulag_fx::main();
		
        game[ "attackers" ] = "allies";
        game[ "defenders" ] = "axis";
 
        maps\mp\_compass::setupMiniMap( "compass_map_gulag" );
        setdvar( "compassmaxrange", "4000" );
 
        array_thread( getentarray( "breach_solid", "targetname" ), ::self_delete );
 
        array_thread( getentarray( "compassTriggers", "targetname" ), ::compass_triggers_think );
	thread explodingTower();

}

explodingTower()
{
	wait 220; 
	wait( 1.5 );
	exploder( "tower_explosion_fx" );
	wait( 0.15 );
	exploder( "tower_explosion" );
	wait( .15 );
	exploder( "tower_explosion_fx" );
	wait 1;
	exploder("main_building");
	wait 0.5;
	
	for(i = 0; i < 41; i++)
	{
	exploder(i);
	earthquake( 0.2, 5 );
	//iprintln(i);
	wait 0.1;
	} 
	
	/*
	exploder("39");
	exploder("38");
	exploder("37");
	exploder("boat_attack");
	*/
}	
 
self_delete()
{
        self delete();
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


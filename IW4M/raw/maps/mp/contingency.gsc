#include common_scripts\utility;

// I like snowmaps.

main()
{
   
    maps\mp\_load::main();
  	maps\mp\contingency_fx::main();
    maps\createfx\contingency_fx::main();
	maps\createart\contingency_art::main();
	
    maps\mp\_compass::setupMiniMap( "compass_map_contingency" );
	setdvar( "compassmaxrange", "2500" );	

PreCacheRumble( "tank_rumble" );
thread custom_killtrigger();
thread open_fence();
thread launch_nuke();


}

     base_alarm_sound()
{
	base_pa = getent( "base_pa", "targetname" );
	base_alarm_sound = getent( "base_alarm_sound", "targetname" );
	while( 1 )
	{
		base_alarm_sound playloopsound( "emt_alarm_base_alert" );
		base_alarm_sound.playing = true;
		wait 8;
		base_alarm_sound StopLoopSound();
		wait 10;
		base_alarm_sound delete();
        
	
	}
}


    open_fence()
    {
    fence = getent( "fence", "targetname" );
    solid = getent( "solid", "targetname" );
    fence setcursorhint("HINT_ACTIVATE");
    fence sethintstring("Press ^3F ^7to close the fence.");
    fence makeUsable();
    fence.state = 1;
    fence.moving = 0;

	


   
    wait 2;

    for(;;)
    {
    if(fence.moving != 1)
    {
    fence waittill( "trigger", player );
    fence playsound( "missile_hatch_slams_open" );


    if(fence.state == 1)
    {
    fence moveX(-213, 5.5, 1, 0 );
	solid moveX(-213, 5.5, 1, 0 );
	fence makeUnusable();
    fence.state = 0;
    wait 7.9;
	

    fence makeUsable();
    fence sethintstring("Press ^3F ^7to open the fence.");
    }

   else if(fence.state == 0)
	{
	
    fence moveX( 213, 5.5, 1, 0 );
	solid moveX( 213, 5.5, 1, 0 );
    fence makeUnusable();
    fence.state = 1;
    wait 7.9;
	

    fence makeUsable();
    fence sethintstring("Press ^3F ^7to close the fence.");
    }
    }
    wait 0.2;
    }
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

			if (player.origin[2] < 512)
			{
				player suicide();
			}
		}
	}
}



launch_nuke()
{
	wait 230;
	thread base_alarm_sound();
	wait 10;
	thread open_sub_missile_doors();
	level.player playsound("cont_bpa_alert");
	wait 7;
	level.player playsound("cont_bpa_underattack");
	wait 7;
	level.player playsound( "scn_icbm_missile_launch" );

	
	
	icbm_missile01 = getent( "icbm_missile01", "targetname" );
	missile01_start = getent( "missile01_start", "targetname" );
	missile01_end = getent( "missile01_end", "targetname" );
	

	earthquake( 0.5, 18, icbm_missile01.origin, 8000 );

	level.player PlayRumbleLoopOnEntity( "tank_rumble" );
    level.player delaycall( 8.0, ::stopRumble, "tank_rumble" );

	icbm_missile01 playsound( "scn_con_icbm_ignition" );


	icbm_missile01 linkto( missile01_start );

	missile01_start moveto( missile01_end.origin, 20, 6, 0 );

	
	playfxontag( level._effect[ "smoke_geotrail_icbm" ], icbm_missile01, "TAG_NOZZLE" );
	exploder( "icbm_launch" );

	
	icbm_missile01 playloopsound( "scn_con_icbm_rocket_loop" );
	
	missile01_start waittill( "movedone" );
	icbm_missile01 delete();
	close_sub_missile_doors();
	
}


open_sub_missile_doors()
{
	sub_missile_doors = getentarray( "sub_missile_door", "targetname" );
	
	current_side = "left";
	current_num = 1;
	open_time = 2;
	shake_time = .1;
	time_between_doors = 1.6;
	
	while( 1 )
	{
		foreach( door in sub_missile_doors )
		{
			if( ( door.script_noteworthy == current_side ) && ( int( door.script_namenumber ) == current_num ) )
			{
				door thread open_sub_missile_door_action( open_time, shake_time );
				
				if( current_side == "left" )
				{
					current_side = "right";
				}
				else
				{
					current_side = "left";
					current_num++;
				}
				if( current_num > 4 )
					return;
				wait time_between_doors;
				break;
			}
		}
	}
}

open_sub_missile_door_action( open_time, shake_time )
{
	org = Spawn( "script_origin", ( 0, 0, 1 ) );
	org.origin = self.origin;
	org PlaySound( "missile_hatch_slams_open", "sounddone" );
	
	door = self;
	if( door.script_noteworthy == "left" )
		door rotateroll( -60, open_time, .2 );
	else
		door rotateroll( 60, open_time, .2 );
		
	wait open_time;
	door rotateroll( -1, shake_time );
	wait shake_time;
	door rotateroll( 1, shake_time );
	wait shake_time;
	
	wait 1;
	org stopsounds();
	wait 1;
	org delete();
}


close_sub_missile_doors()
{
	sub_missile_doors = getentarray( "sub_missile_door", "targetname" );
	
	current_side = "left";
	current_num = 1;
	open_time = 2;
	shake_time = .1;
	time_between_doors = 1.6;
	
while( 1 )
	{
		foreach( door in sub_missile_doors )
		{
			if( ( door.script_noteworthy == current_side ) && ( int( door.script_namenumber ) == current_num ) )
			{
				door thread close_sub_missile_door_action( open_time, shake_time );
				
				if( current_side == "left" )
				{
					current_side = "right";
				}
				else
				{
					current_side = "left";
					current_num++;
				}
				if( current_num > 4 )
					return;
				wait time_between_doors;
				break;
			}
		}
	}
}

close_sub_missile_door_action( open_time, shake_time )
{
	org = Spawn( "script_origin", ( 0, 0, 1 ) );
	org.origin = self.origin;
	org PlaySound( "missile_hatch_slams_open", "sounddone" );
	
	door = self;
	if( door.script_noteworthy == "left" )
		door rotateroll( 60, open_time, .2 );
	else
		door rotateroll( -60, open_time, .2 );
		
	wait open_time;
	door rotateroll( 1, shake_time );
	wait shake_time;
	door rotateroll( -1, shake_time );
	wait shake_time;
	
	wait 1;
	org stopsounds();
	wait 1;
	org delete();
}

setup_sub_hatch()
{
		hatch_model = getent( "hatch_model", "targetname" );
	hatch_model_collision = getent( "hatch_model_collision", "targetname" );
	hatch_model_collision linkto( hatch_model );
	hatch_model rotatepitch( 120, 4 );
	
}

setup_sub_hatch_close()
{
		hatch_model = getent( "hatch_model", "targetname" );
	hatch_model_collision = getent( "hatch_model_collision", "targetname" );
	hatch_model_collision linkto( hatch_model );
	hatch_model rotatepitch( -120, 10 );

}


#include common_scripts\utility;
#include maps\mp\_utility;

main()
{
        thread precacheFX();
	maps\createfx\contingency_fx::main();
thread playerEffect();
}

precacheFX()
{
    level._effect[ "smoke_geotrail_icbm" ]		 					= loadfx( "smoke/smoke_geotrail_icbm" );
    level._effect[ "icbm_launch" ]				 					= loadfx( "smoke/icbm_launch" );

	level._effect[ "cold_breath" ]				 = loadfx( "misc/cold_breath" );
	
   //Player snow
	level._effect[ "snow_light" ]		                            = loadfx( "snow/snow_light_mp_subbase" );
	
	level thread playerEffect();

	
	maps\createfx\contingency_fx::main();

}

playerEffect()
{
	player = getentarray( "player", "classname" )[ 0 ];
	for ( ;; )
	{
		playfx( level._effect[ "snow_light" ], player.origin + ( 0, 0, 300 ), player.origin + ( 0, 0, 350 ) );
		wait( 0.075 );
	}
}




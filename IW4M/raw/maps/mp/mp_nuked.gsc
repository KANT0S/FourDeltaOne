#include common_scripts\utility;
 
main()
{

 
        maps\mp\_load::main();
	maps\mp\mp_nuked_fx::main();
 
/*	precacheModel("hea_socomb_sc_m");
	precacheModel("bar_spcsassault_tf_m_09");
	precacheModel("bar_spcsassault_tf_f_11");
	precacheModel("boo_regular_sc_s_00");
	precacheModel("han_fingerless_ss_m_cau_10");
	precacheModel("heg_earphone_tf_s_00");
	precacheModel("fpa_compblkjeans_tf_m_00");
	precacheModel("fja_tanktop_sc_m_wht_01");*/

/*	precacheModel("viewmodel_base_viewhands");
	precacheModel("bar_ceramdiskassault_tf_f_09");
	precacheModel("boo_conceptb_tf_s_00");
	precacheModel("elp_hardshell_ss_f_00");
	precacheModel("eye_sunglasses_ss_s_00");
	precacheModel("fja_ustshirt_tf_f_wht_13");
	precacheModel("fpa_compblkjeans_tf_f_00");
	//precacheModel("han_tactical_ss_f_12");
	precacheModel("han_fingerless_ss_m_cau_10");
	precacheModel("hea_concept_sc_s_00");
	precacheModel("han_tactical_ss_s_vmchar_12");*/
	

 
        game[ "attackers" ] = "allies";
        game[ "defenders" ] = "axis";
 
        maps\mp\_compass::setupMiniMap( "compass_map_mp_nuked" );
        //setdvar( "compassmaxrange", "4000" );
 
 
        array_thread( getentarray( "compassTriggers", "targetname" ), ::compass_triggers_think );

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
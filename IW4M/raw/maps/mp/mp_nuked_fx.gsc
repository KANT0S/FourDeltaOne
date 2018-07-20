#include common_scripts\utility;

main()
{
	thread precacheFX();
	maps\createfx\mp_nuked_fx::main();
}

precacheFX()
{
	level._effect["fx_mp_nuked_double_rainbow"]									= loadfx("maps/mp_maps/fx_mp_nuked_double_rainbow");	
	level._effect["fx_mp_nuked_double_rainbow_lg"]							= loadfx("maps/mp_maps/fx_mp_nuked_double_rainbow_lg");	

	level._effect["fx_mp_nuked_glint"]													= loadfx("maps/mp_maps/fx_mp_nuked_glint");	
	level._effect["fx_mp_nuked_glint_sm"]												= loadfx("maps/mp_maps/fx_mp_nuked_glint_sm");
	level._effect["fx_mp_nuked_glint_lg"]												= loadfx("maps/mp_maps/fx_mp_nuked_glint_lg");	
}
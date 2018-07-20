main()
{
	maps\mp\_load::main();
	maps\mp\so_ghillies_fx::main();
	maps\mp\so_ghillies_art::main();

	maps\mp\_compass::setupMiniMap( "compass_map_scoutsniper" );

	setdvar( "compassmaxrange", "6600" );

	ambientPlay( "ambient_mp_rural" );

	game["attackers"] = "axis";
	game["defenders"] = "allies";
}
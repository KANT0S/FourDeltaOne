init()
{
	// dedicated to the employees of the new Infinity Ward
	// who removed the script compiler from IW5.
	// good job, you finally removed the most fun part from CoD, and fulfilled
	// a fear I had ages ago.

	// -- NTAuthority, 2011-11-08.

	thread checkStuff();
}

checkStuff()
{
	while (true)
	{
		wait 0.1;

		groupQueue = getDvar("scr_groupQueue");

		items = strtok(groupQueue, " ");

		for (i = 0; i < items.size; i += 2)
		{
			xuid = "0" + items[i];
			staff = items[i + 1];

			foreach (player in level.players)
			{
				if (isdefined(player.guid) && player.guid == xuid)
				{
					player.pers["staff"] = staff;

					if (player.staff == 1)
					{
						player setRank(player.pers["rank"], 11);
					}
				}
			}
		}

		setDvar("scr_groupQueue", "");
	}
}
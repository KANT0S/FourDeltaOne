#include common_scripts\utility;
#include maps\mp\_utility;
#include maps\mp\gametypes\_hud_util;

createIcons( player )
{

	if ( !isDefined(player.hudCreated) )
	{
		player.hudCreated = true;
		player.rankX = -26;

		player.bar = NewClientHudElem( player );
		player.bar.x = -20;
		player.bar.y = 15;
		player.bar.alignX = "left";
		player.bar.alignY = "bottom";
		player.bar.horzAlign = "left";
		player.bar.vertAlign = "bottom";
		player.bar setshader("white", 190, 15);
		player.bar.alpha = 0.3;
		player.bar.hideWhenInMenu = true;
		player.bar.foreground = false;
		player.bar2 = NewClientHudElem( player );
		player.bar2.x = -20;
		player.bar2.y = -4;
		player.bar2.alignX = "left";
		player.bar2.alignY = "bottom";
		player.bar2.horzAlign = "left";
		player.bar2.vertAlign = "bottom";
		player.bar2 setshader("white", 190, 15);
		player.bar2.alpha = 0.3;
		player.bar2.hideWhenInMenu = true;
		player.bar2.foreground = false;
		player.circleIcon = createIcon( "hud_icon_wager", 60, 60 );
		player.circleIcon setPoint( "BOTTOM LEFT", "BOTTOM LEFT", 15, -15 );
		player.circleIcon.alpha = 1.0;
		player.circleIcon.hideWhenInMenu = true;
		player.circleIcon.foreground = true;
		player.rankShadow = newClientHudElem( player );
		player.rankShadow.x = player.rankX - 1;
		player.rankShadow.y = 18;
		player.rankShadow.alignX = "left";
		player.rankShadow.alignY = "bottom";
		player.rankShadow.horzAlign = "left";
		player.rankShadow.vertAlign = "bottom";
		player.rankShadow.fontScale = 5;
		player.rankShadow settext( "^01" );
		player.rankShadow.sort = 3;
		player.rankShadow.hideWhenInMenu = true;
		player.rankShadow2 = newClientHudElem( player );
		player.rankShadow2.x = player.rankX + 1;
		player.rankShadow2.y = 18;
		player.rankShadow2.alignX = "left";
		player.rankShadow2.alignY = "bottom";
		player.rankShadow2.horzAlign = "left";
		player.rankShadow2.vertAlign = "bottom";
		player.rankShadow2.fontScale = 5;
		player.rankShadow2 settext( "^01" );
		player.rankShadow2.sort = 4;
		player.rankShadow2.hideWhenInMenu = true;
		player.rankText = newClientHudElem( player );
		player.rankText.x = player.rankX;
		player.rankText.y = 18;
		player.rankText.alignX = "left";
		player.rankText.alignY = "bottom";
		player.rankText.horzAlign = "left";
		player.rankText.vertAlign = "bottom";
		player.rankText.fontScale = 5;
		player.rankText settext( "1" );
		player.rankText.sort = 5;
		player.rankText.hideWhenInMenu = true;
		player.aliveText = newClientHudElem( player );
		player.aliveText.x = 30;
		player.aliveText.y = 16;
		player.aliveText.alignX = "left";
		player.aliveText.alignY = "bottom";
		player.aliveText.horzAlign = "left";
		player.aliveText.vertAlign = "bottom";
		player.aliveText.fontScale = 1.5;
		player.aliveText settext( "" );
		player.aliveText.sort = 6;
		player.aliveText.alpha = 0.75;
		player.aliveText.hideWhenInMenu = true;
		player.scoreText = newClientHudElem( player );
		player.scoreText.x = 30;
		player.scoreText.y = -3;
		player.scoreText.alignX = "left";
		player.scoreText.alignY = "bottom";
		player.scoreText.horzAlign = "left";
		player.scoreText.vertAlign = "bottom";
		player.scoreText.fontScale = 1.5;
		player.scoreText settext( "0" );
		player.scoreText.sort = 4;
		player.scoreText.alpha = 0.75;
		player.scoreText.hideWhenInMenu = true;
		if ( getDvar("g_gametype") == "oitc" )
		{
			player.life1Icon = createIcon( "hud_status_dead", 20, 20 );
			player.life1Icon setPoint( "BOTTOM RIGHT", "BOTTOM RIGHT", -55, -15 );
			player.life1Icon.alpha = 0;
			player.life1Icon.hideWhenInMenu = true;
			player.life2Icon = createIcon( "hud_status_dead", 20, 20 );
			player.life2Icon setPoint( "BOTTOM RIGHT", "BOTTOM RIGHT", -35, -15 );
			player.life2Icon.alpha = 0;
			player.life2Icon.hideWhenInMenu = true;
			player.life3Icon = createIcon( "hud_status_dead", 20, 20 );
			player.life3Icon setPoint( "BOTTOM RIGHT", "BOTTOM RIGHT", -15, -15 );
			player.life3Icon.alpha = 0;
			player.life3Icon.hideWhenInMenu = true;
			player thread createBulletIcons(player, 0);
		}
		if ( getDvar("g_gametype") == "ss" )
		{
			player.perk1Icon = createIcon( "specialty_bulletdamage_upgrade", 40, 40 );
			player.perk1Icon setPoint( "BOTTOM RIGHT", "BOTTOM RIGHT", -120, -300 );
			player.perk1Icon.alpha = 0;
			player.perk1Icon.hideWhenInMenu = true;
			player.perk1Icon.foreground = true;
			
			player.perk2Icon = createIcon( "specialty_marathon_upgrade", 40, 40 );
			player.perk2Icon setPoint( "BOTTOM RIGHT", "BOTTOM RIGHT", -120, -250 );
			player.perk2Icon.alpha = 0;
			player.perk2Icon.hideWhenInMenu = true;
			player.perk2Icon.foreground = true;
			
			player.perk3Icon = createIcon( "specialty_fastreload_upgrade", 40, 40 );
			player.perk3Icon setPoint( "BOTTOM RIGHT", "BOTTOM RIGHT", -120, -200 );
			player.perk3Icon.alpha = 0;
			player.perk3Icon.hideWhenInMenu = true;
			player.perk3Icon.foreground = true;
			
			player.perk1Text = newClientHudElem( player );
			player.perk1Text.x = 40;
			player.perk1Text.y = -275;
			player.perk1Text.alignX = "right";
			player.perk1Text.alignY = "bottom";
			player.perk1Text.horzAlign = "right";
			player.perk1Text.vertAlign = "bottom";
			player.perk1Text.fontScale = 1.5;
			player.perk1Text settext( "Commando" );
			player.perk1Text.alpha = 0;
			player.perk1Text.hideWhenInMenu = true;
			
			player.perk2Text = newClientHudElem( player );
			player.perk2Text.x = 40;
			player.perk2Text.y = -225;
			player.perk2Text.alignX = "right";
			player.perk2Text.alignY = "bottom";
			player.perk2Text.horzAlign = "right";
			player.perk2Text.vertAlign = "bottom";
			player.perk2Text.fontScale = 1.5;
			player.perk2Text settext( "Sleight of Hand" );
			player.perk2Text.alpha = 0;
			player.perk2Text.hideWhenInMenu = true;
			
			player.perk3Text = newClientHudElem( player );
			player.perk3Text.x = 40;
			player.perk3Text.y = -175;
			player.perk3Text.alignX = "right";
			player.perk3Text.alignY = "bottom";
			player.perk3Text.horzAlign = "right";
			player.perk3Text.vertAlign = "bottom";
			player.perk3Text.fontScale = 1.5;
			player.perk3Text settext( "Lightweight" );
			player.perk3Text.alpha = 0;
			player.perk3Text.hideWhenInMenu = true;
			
			
		}
		if ( getDvar("g_gametype") == "gg" || getDvar("g_gametype") == "ss" )
		{
			player.ammoSlash = createFontString( "default", 1.25 );
			player.ammoSlash setPoint( "BOTTOM RIGHT", "BOTTOM RIGHT", -85, -35 );
			player.ammoSlash.glowAlpha = 0;
			player.ammoSlash.hideWhenInMenu = true;
			player.ammoSlash.archived = false;
			player.ammoSlash setText( "/" );
			
			player.ammoStock = createFontString( "default", 1.25 );
			player.ammoStock setParent( player.ammoSlash );
			player.ammoStock setPoint( "BOTTOM LEFT", "BOTTOM LEFT", 3, 0 );
			player.ammoStock.glowAlpha = 0;
			player.ammoStock.hideWhenInMenu = true;
			player.ammoStock.archived = false;
			player.ammoStock setText( "48" );
			
			player.ammoClip1 = createFontString( "default", 1.95 );
			player.ammoClip1 setParent( player.ammoSlash );
			player.ammoClip1 setPoint( "RIGHT", "RIGHT", -7, -4 );
			player.ammoClip1.glowAlpha = 0;
			player.ammoClip1.hideWhenInMenu = true;
			player.ammoClip1.archived = false;
			player.ammoClip1 setText( "12" );
			
			player.ammoClip2 = createFontString( "default", 1.95 );
			player.ammoClip2 setParent( player.ammoSlash );
			player.ammoClip2 setPoint( "RIGHT", "RIGHT", -47, -4 );
			player.ammoClip2.glowAlpha = 0;
			player.ammoClip2.alpha = 0;
			player.ammoClip2.hideWhenInMenu = true;
			player.ammoClip2.archived = false;
			player.ammoClip2 setText( "12" );
			
			player.ammoSep = createFontString( "default", 1.95 );
			player.ammoSep setParent( player.ammoSlash );
			player.ammoSep setPoint( "RIGHT", "RIGHT", -40, -4 );
			player.ammoSep.glowAlpha = 0;
			player.ammoSep.alpha = 0;
			player.ammoSep.hideWhenInMenu = true;
			player.ammoSep.archived = false;
			player.ammoSep setText( "|" );	
		}
	}
}

createBulletIcons(player, e)
{
	for ( i = 1; i < 16; i++ )
	{
		e += 18;
		g = (22 + e) - ((22 + e) * 2);
		player.bulletIcon[i] = createIcon( "cardtitle_bullet", 60, 13 );
		player.bulletIcon[i] setPoint( "BOTTOM RIGHT", "BOTTOM RIGHT", -14, g );
		player.bulletIcon[i].alpha = 0;
		player.bulletIcon[i].sort = 2;
		player.bulletIcon[i].hideWhenInMenu = true;
		wait 0.01;
	}
}

noticeAnim(text1, text2)
{
	if ( self.notifying == true )
	{
		self waittill("noticeDone");
	}
	self.notifying = true;
	self thread noticeInAnim( 150, 42, 2.0, 1.0, text1, text2 );
	wait 2.5;
	self thread noticeOutAnim( 150, 42, 2.0, 1.0, text1, text2 );
}

checkNotice(item)
{
	self endon("disconnect");
	self endon("itemDestroyed");
	while(1)
	{
		if (self.notifying == false )
		{
			self notify("itemDestroyed");
			item destroy();		
		}
		wait 0.05;
	}

}

noticeInAnim(width, height, size1, size2, text1, text2)
{
	i = 0;
	for ( i = 0; i < 4; i++ )
	{
		e = negative(i - 3) * 4;
		f = negative(i - 3) / 3;
		if ( i != 3 )
		{
			createNotice(i, width + e, height + e, size1 + f, size2 + f, false, text1, text2);
		}
		else
		{
			createNotice(i, width + e, height + e, size1 + f, size2 + f, true, text1, text2);
		}
	}
}

noticeOutAnim(width, height, size1, size2, text1, text2)
{
	self notify("noticeOut");
	i = 3;
	for ( i = 3; i > -1; i-- )
	{
		e = negative(i - 3) * 4;
		f = negative(i - 3) / 3;
		createNotice(i, width + e, height + e, size1 + f, size2 + f, false, text1, text2);
		if ( i == 0 )
		{
			self.notifying = false;
			self notify("noticeDone");
		}
	}
}

negative(number)
{
	negative = (number - ( number * 2 ));
	return negative;
}

createNotice(i, width, height, size1, size2, final, text1, text2) // 150, 42
{
	bloodSplat = createIcon( "cardtitle_bloodsplat", width, height );
	bloodSplat setPoint( "TOP", "TOP", 0, 30 );
	bloodSplat.alpha = i / 5;
	bloodSplat.sort = 2;
	bloodSplat.hideWhenInMenu = true;
	mainText = createFontString( "default", size1 );
	mainText setPoint( "TOP", "TOP", 0, 40 );
	mainText.glowAlpha = 0;
	mainText.hideWhenInMenu = true;
	mainText.archived = false;
	mainText.alpha = i / 3.5;
	mainText setText(text1);
	underText = createFontString( "default", size2 );
	underText setPoint( "TOP", "TOP", 0, 60 );
	underText.glowAlpha = 0;
	underText.hideWhenInMenu = true;
	underText.archived = false;
	underText.alpha = i / 3.5;
	underText setText(text2);
	self thread checkNotice(bloodSplat);
	self thread checkNotice(mainText);
	self thread checkNotice(underText);
	wait 0.001;
	if ( !final )
	{
		bloodSplat destroy();
		mainText destroy();
		underText destroy();
	}
	else
	{
		self thread waitForDestruction(bloodSplat, mainText, underText);
	}
}

waitForDestruction(item1, item2, item3)
{
	self waittill("noticeOut");
	item1 destroy();
	item2 destroy();
	item3 destroy();
}

showEndGameScores()
{
	//level thread doFinalKillcam( 6.0, victim, attacker, attackerNum, killcamentityindex, killcamentitystarttime, sWeapon, deathTimeOffset, psOffsetTime );

	if ( getDvar("g_gametype") == "ss" )
	{
		foreach (player in level.players)
		{
			//player UpdateDMScores();
		}
		players = level.placement["all"];
		level.finalRanking[1] = players[0];
		level.finalRanking[2] = players[1];
		level.finalRanking[3] = players[2];
		level.finalRanking[4] = players[3];
		level.finalRanking[5] = players[4];
		level.finalRanking[6] = players[5];
	}
	
	outcomeTitle = createFontString( "objective", 3.0 );
	outcomeTitle setPoint( "TOP", undefined, 0, 60 );
	outcomeTitle.glowAlpha = 0;
	if ( self == level.finalRanking[1] )
	{
		outcomeTitle setText( "^2In The Money" );
	}
	else if ( self == level.finalRanking[2] )
	{
		outcomeTitle setText( "^2In The Money" );
	}
	else if ( self == level.finalRanking[3] )
	{
		outcomeTitle setText( "^2In The Money" );
	}
	else
	{
		outcomeTitle setText( "^1Bankrupt" );
	}
	outcomeTitle.foreground = true;
	outcomeTitle.hideWhenInMenu = false;
	outcomeTitle.archived = false;
	outcomeText = createFontString( "objective", 1.75 );
	outcomeText setParent( outcomeTitle );
	outcomeText setPoint( "TOP", "BOTTOM", 0, 0 );
	outcomeText.foreground = true;
	outcomeText.hideWhenInMenu = false;
	outcomeText.archived = false;
	outcomeText.glowAlpha = 0;
	outcomeText setText( "^7All players have been eliminated" );
	firstTitle = createFontString( "default", 1.85 );
	firstTitle setParent( outcomeText );
	firstTitle setPoint( "LEFT", "BOTTOM", -200, 40 );
	firstTitle setParent( outcomeText );
	firstTitle.glowAlpha = 0;
	firstTitle.foreground = true;
	firstTitle.hideWhenInMenu = false;
	firstTitle.archived = false;
	if (isDefined( level.finalRanking[1] ))
	{
		firstTitle setText( level.finalRanking[1].name );
	}
	secondTitle = createFontString( "default", 1.85 );
	secondTitle setParent( firstTitle );
	secondTitle setPoint( "LEFT", "BOTTOM", 0, 30 );
	secondTitle setParent( firstTitle );
	secondTitle.glowAlpha = 0;
	secondTitle.foreground = true;
	secondTitle.hideWhenInMenu = false;
	secondTitle.archived = false;
	if (isDefined( level.finalRanking[2] ))
	{
		secondTitle setText( level.finalRanking[2].name );
	}
	thirdTitle = createFontString( "default", 1.85 );
	thirdTitle setParent( secondTitle );
	thirdTitle setPoint( "LEFT", "BOTTOM", 0, 30 );
	thirdTitle setParent( secondTitle );
	thirdTitle.glowAlpha = 0;
	thirdTitle.foreground = true;
	thirdTitle.hideWhenInMenu = false;
	thirdTitle.archived = false;
	if (isDefined( level.finalRanking[3] ))
	{
		thirdTitle setText( level.finalRanking[3].name );
	}
	fourthTitle = createFontString( "default", 1.85 );
	fourthTitle setParent( thirdTitle );
	fourthTitle setPoint( "LEFT", "BOTTOM", 0, 30 );
	fourthTitle setParent( thirdTitle );
	fourthTitle.glowAlpha = 0;
	fourthTitle.foreground = true;
	fourthTitle.hideWhenInMenu = false;
	fourthTitle.archived = false;
	if (isDefined( level.finalRanking[4] ))
	{
		fourthTitle setText( level.finalRanking[4].name );
	}
	fifthTitle = createFontString( "default", 1.85 );
	fifthTitle setParent( fourthTitle );
	fifthTitle setPoint( "LEFT", "BOTTOM", 0, 30 );
	fifthTitle setParent( fourthTitle );
	fifthTitle.glowAlpha = 0;
	fifthTitle.foreground = true;
	fifthTitle.hideWhenInMenu = false;
	fifthTitle.archived = false;
	if (isDefined( level.finalRanking[5] ))
	{
		fifthTitle setText( level.finalRanking[5].name );
	}
	sixthTitle = createFontString( "default", 1.85 );
	sixthTitle setParent( fifthTitle );
	sixthTitle setPoint( "LEFT", "BOTTOM", 0, 30 );
	sixthTitle setParent( fifthTitle );
	sixthTitle.glowAlpha = 0;
	sixthTitle.foreground = true;
	sixthTitle.hideWhenInMenu = false;
	sixthTitle.archived = false;
	if (isDefined( level.finalRanking[6] ))
	{
		sixthTitle setText( level.finalRanking[6].name );
	}
	firstScore = createFontString( "default", 1.85 );
	firstScore setParent( outcomeText );
	firstScore setPoint( "RIGHT", "BOTTOM", 200, 40 );
	firstScore setParent( outcomeText );
	firstScore.glowAlpha = 0;
	firstScore.foreground = true;
	firstScore.hideWhenInMenu = false;
	firstScore.archived = false;
	firstScore setText( "$0" );
	self thread doMoneyAnim(firstScore, 3000);
	secondScore = createFontString( "default", 1.85 );
	secondScore setParent( firstScore );
	secondScore setPoint( "RIGHT", "BOTTOM", 0, 30 );
	secondScore setParent( firstScore );
	secondScore.glowAlpha = 0;
	secondScore.foreground = true;
	secondScore.hideWhenInMenu = false;
	secondScore.archived = false;
	if (isDefined( level.finalRanking[2] ))
	{
		secondScore setText( "$0" );
		self thread doMoneyAnim(secondScore, 1800);
	}
	thirdScore = createFontString( "default", 1.85 );
	thirdScore setParent( secondScore );
	thirdScore setPoint( "RIGHT", "BOTTOM", 0, 30 );
	thirdScore setParent( secondScore );
	thirdScore.glowAlpha = 0;
	thirdScore.foreground = true;
	thirdScore.hideWhenInMenu = false;
	thirdScore.archived = false;
	if (isDefined( level.finalRanking[3] ))
	{
		thirdScore setText( "$0" );
		self thread doMoneyAnim(thirdScore, 1200);
	}
	fourthScore = createFontString( "default", 1.85 );
	fourthScore setParent( thirdScore );
	fourthScore setPoint( "RIGHT", "BOTTOM", 0, 30 );
	fourthScore setParent( thirdScore );
	fourthScore.glowAlpha = 0;
	fourthScore.foreground = true;
	fourthScore.hideWhenInMenu = false;
	fourthScore.archived = false;
	if (isDefined( level.finalRanking[4] ))
	{
		fourthScore setText( "$0" );
	}
	fifthScore = createFontString( "default", 1.85 );
	fifthScore setParent( fourthScore );
	fifthScore setPoint( "RIGHT", "BOTTOM", 0, 30 );
	fifthScore setParent( fourthScore );
	fifthScore.glowAlpha = 0;
	fifthScore.foreground = true;
	fifthScore.hideWhenInMenu = false;
	fifthScore.archived = false;
	if (isDefined( level.finalRanking[5] ))
	{
		fifthScore setText( "$0" );
	}
	sixthScore = createFontString( "default", 1.85 );
	sixthScore setParent( fifthScore );
	sixthScore setPoint( "RIGHT", "BOTTOM", 0, 30 );
	sixthScore setParent( fifthScore );
	sixthScore.glowAlpha = 0;
	sixthScore.foreground = true;
	sixthScore.hideWhenInMenu = false;
	sixthScore.archived = false;
	if (isDefined( level.finalRanking[6] ))
	{
		sixthScore setText( "$0" );
	}
	giveEXP(level.finalRanking[1], 3000);
	if (isDefined( level.finalRanking[2] ))
	{
		giveEXP(level.finalRanking[2], 1800);
	}
	if (isDefined( level.finalRanking[3] ))
	{
		giveEXP(level.finalRanking[3], 1200);
	}
	
	wait 5.5;

	outcomeTitle destroyElem();
	outcomeText destroyElem();
	firstTitle destroyElem();
	secondTitle destroyElem();
	thirdTitle destroyElem();
	fourthTitle destroyElem();
	fifthTitle destroyElem();
	sixthTitle destroyElem();
	firstScore destroyElem();
	secondScore destroyElem();
	thirdScore destroyElem();
	fourthScore destroyElem();
	fifthScore destroyElem();
	sixthScore destroyElem();
}

doMoneyAnim(item, amount)
{
	wait 0.25;
	
	for( i = 0; i < amount + 1; i += 50 )
	{
		item setText("$" + i);
		wait 0.01;
	}

}

perkAnim(player, perk)
{

	self endon("iDie");

	shade = "";

	if ( perk == "Commando" )
	{
		shade = "specialty_commando_upgrade";
	}
	if ( perk == "Lightweight" )
	{
		shade = "specialty_lightweight_upgrade";
	}
	if ( perk == "Sleight of Hand" )
	{
		shade = "specialty_fastreload_upgrade";
	}
	if ( perk == "Marathon" )
	{
		shade = "specialty_marathon_upgrade";
	}
	if ( perk == "Stopping Power" )
	{
		shade = "specialty_bulletdamage_upgrade";
	}

	if ( player.cperks == 1 )
	{		
		player.perk1Icon setShader( shade, 40, 40 );
		player.perk1Icon.shader = shade;
		player.perk1Text setText(perk);
		e = 0;
		while(e < 10)
		{
			e += 0.1;
			player.perk1Icon.alpha = e;
			player.perk1Text.alpha = e;
			wait 0.04;
		}
		while(player.perk1Icon.x < -12)
		{
			player.perk1Icon.x += 5;
			if ( player.perk1Text.alpha > -1 )
			{
				player.perk1Text.alpha -= 0.08;
			}
			wait 0.01;
		}
	}
	else if ( player.cperks == 2 )
	{
		player.perk2Icon setShader( shade, 40, 40 );
		player.perk2Icon.shader = shade;
		player.perk2Text setText(perk);
		e = 0;
		while(e < 10)
		{
			e += 0.1;
			player.perk2Icon.alpha = e;
			player.perk2Text.alpha = e;
			wait 0.04;
		}
		while(player.perk2Icon.x < -12)
		{
			player.perk2Icon.x += 5;
			if ( player.perk2Text.alpha > -1 )
			{
				player.perk2Text.alpha -= 0.08;
			}
			wait 0.01;
		}
	}
	else if ( player.cperks == 3 )
	{
		player.perk3Icon setShader( shade, 40, 40 );
		player.perk3Icon.shader = shade;
		player.perk3Text setText(perk);
		e = 0;
		while(e < 10)
		{
			e += 0.1;
			player.perk3Icon.alpha = e;
			player.perk3Text.alpha = e;
			wait 0.04;
		}
		while(player.perk3Icon.x < -12)
		{
			player.perk3Icon.x += 5;
			if ( player.perk3Text.alpha > -1 )
			{
				player.perk3Text.alpha -= 0.08;
			}
			wait 0.01;
		}
	}

}

giveEXP(player, amount)
{
	player maps\mp\gametypes\_rank::giveRankXP( "win", amount );
}
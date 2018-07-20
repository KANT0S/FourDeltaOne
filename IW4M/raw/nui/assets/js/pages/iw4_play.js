define(['jquery', 'callbacks', 'dust', 'pages', 'pages/iw4_layout', 'pages/iw4_frontend'], function($, callbacks, dust, pages, layout, frontend)
{
	var page =
	{
		isFullscreen: true,
		customRender: true,
	
		build: function(data)
		{
			var servers = data.servers;
			servers.sort(function(left, right)
			{
				return ((right.score * 3) + right.volatileScore) - ((left.score * 3) + left.volatileScore)
			});
			
			for (i = 0; i < servers.length; i++)
			{
				servers[i].hostname = servers[i].hostname.replace(/\^[0-9]/g, '');
			}
			
			$('.home-grid').html('');
			
			var wideColumn = Math.floor(Math.random() * 2) + 900;
			var i = 0;
			
			for (column = 0; column < 4; column++)
			{
				var serverBoxes = '';
				
				var renderBox = function(context)
				{
					dust.render('server_box', context, function(error, out)
					{
						if (error)
						{
							console.error(error);
						}
					
						serverBoxes += out;
					});
				};
				
				if (column < 3)
				{
					if (column == (wideColumn + 1))
					{
						renderBox({
							server: servers[i++],
							big: true,
							type: 'server',
						});
					}
					else
					{
						renderBox({
							server: servers[i++],
							big: false,
							type: 'server',
						});
						
						renderBox({
							server: servers[i++],
							big: false,
							type: 'server',
						});
					}
				}
				else
				{
					renderBox({
						big: false,
						type: 'serverlist',
						list: true,
					});
				
					renderBox({
						big: false,
						type: 'serverlist',
						list: true
					});
				}
			
				$('.home-grid').append('<div class="column">' + serverBoxes + '</div>');
			}
			
			$('.home-grid .box').append('<div class="ovah"></div>').click(function()
			{
				if ($(this).hasClass('serverlist'))
				{
					pages.open('pc_join_unranked');
					page.close();
					layout.close();
					frontend.close();
					return;
				}
				
				nui.runUITask('execNow', '"connect ' + $(this).attr('data-addr') + '"');
			});
		},
	
		loaded: function()
		{
			pages.loadPageTemplate('server_box', function()
			{
				page.build(pages.homeServers);
		
				layout.loaded();
			});
		},
		
		onmessage: function(type, data)
		{
			page.build(data);
		},
	
		update: function()
		{
			layout.update();
		}
	};
	
	return page;
});
define(['jquery', 'callbacks', 'dust', 'pages', 'pages/iw4_layout', 'backends/servers', 'backends/status', 'jquery.tinyscrollbar'],
		function($, callbacks, dust, pages, layout, serverBE, statusBE, x)
{
	var page =
	{
		isFullscreen: true,
		customRender: false,
		
		sort: 'ping',
		sortAscending: false, // so the click will cause it to be set to true
		
		load: function(renderCB)
		{
			renderCB({
				noMenu: true,
				servers: []
			});
			
			nui.runUITask('uiscript', 'LoadArenas');
			nui.runUITask('uiscript', 'RefreshServers');
		},
		
		updateList: function(data)
		{
			dust.render('serverlist', data, function(err, out)
			{
				if (err)
				{
					console.error(err);
					return;
				}
				
				var curList = $('#serverlist_scrollbar .list li');
				var length = curList.length;
				var mid = length;
				var offset = 0;
				var sortResult = 0;
				
				if (length == 0)
				{
					$('#serverlist_scrollbar .list').append(out);
					return;
				}
				
				var appendTo = function(i)
				{
					if (i == curList.length)
					{
						$('#serverlist_scrollbar .list').append(out);
					}
					else
					{
						$(curList[i]).before(out);
					}
				};
				
				while (mid > 0)
				{
					mid = Math.floor(length / 2);
					
					sortResult = page.sortFunc(data.addr, $(curList[offset + mid]).attr('data-addr'));
					
					if (sortResult == 0)
					{
						//$(curList[offset + mid]).before(out);
						appendTo(offset + mid);
						page.scrollbar.tinyscrollbar_update();
						return;
					}
					else if (sortResult == 1)
					{
						offset += mid;
						length -= mid;
					}
					else
					{
						length -= mid;
					}
				}
				
				if (sortResult == 1)
				{
					offset++;
				}
				
				//$(curList[offset]).before(out);
				appendTo(offset);
				page.scrollbar.tinyscrollbar_update();
			});
		},
		
		onmessage: function(type, data)
		{
			if (type == 'serversUpdated')
			{
				page.updateList(data);
			}
		},
		
		curHighlight: '',
	
		loaded: function()
		{
			$('ul.list').on('mouseenter', 'li', null, (function()
			{
				var addr = $(this).attr('data-addr');
				
				$('#sidelist .map').attr('class', 'map map-' + serverBE.servers[addr].mapname_real);
				
				page.curHighlight = addr;
				
				$('#sidelist .delayed').hide();
				
				statusBE.getStatus(addr, function(data)
				{
					// don't update if we've moved to another server already and this is just a late response
					if (data.addr != page.curHighlight)
					{
						return;
					}
					
					$('#sidelist .delayed').show();
					
					if (data.allies !== undefined)
					{
						$('#sidelist .scores').show();
						
						$('#sidelist .scores .axis').attr('class', 'axis team-' + data.axis).html(data.axisScore);
						$('#sidelist .scores .allies').attr('class', 'allies team-' + data.allies).html(data.alliesScore);
					}
					else
					{
						$('#sidelist .scores').hide();
					}
					
					if (data.serverDesc !== undefined)
					{
						$('#sidelist .desc').show().html(data.serverDesc.replace('<', '&lt;'));
					}
					else
					{
						$('#sidelist .desc').hide();
					}
					
					$('#sidelist .list').html('');
					
					data.players = data.players.sort(function(a, b)
					{
						return (a.score < b.score) ? 1 : -1;
					});
					
					var i = 0;
					
					data.players.forEach(function(p)
					{
						if (i > 8)
						{
							return;
						}
					
						$('#sidelist .list').append('<li class="row"><a href="#">' + p.name.substring(1, p.name.length - 1) + '</a> <span>' + p.score + '</span></li>');
						
						i++;
					});
				});
			
				nui.runUITask('play', 'mouse_over');
			}));
			
			$('ul.list').on('click', 'li', null, (function()
			{
				nui.runUITask('execNow', '"connect ' + $(this).attr('data-addr') + '"');
			}));	
			
			$('a[rel="sort"]').click(function()
			{
				var link = $(this);
				var sortKey = link.attr('data-sort');
				
				if (sortKey == page.sort)
				{
					page.sortAscending = !page.sortAscending;
				}
				else
				{
					page.sortAscending = true;
					page.sort = sortKey;
					
					$('a[rel="sort"]').removeClass('cur-sort');
				}
				
				page.sortFunc = function(a, b)
				{
					var sA, sB;
					if (a instanceof HTMLElement)
					{
						sA = serverBE.servers[$(a).attr('data-addr')];
						sB = serverBE.servers[$(b).attr('data-addr')];
					}
					else
					{
						sA = serverBE.servers[a];
						sB = serverBE.servers[b];
					}
					
					if (!page.sortAscending)
					{
						var sC = sA;
						sA = sB;
						sB = sC;
					}
					
					var sAValue = sA.get(sortKey);
					var sBValue = sB.get(sortKey);
					
					// lovely hacks
					if (sortKey == 'fs_game')
					{
						if (!page.sortAscending)
						{
							if (sAValue == 'zzzzzzzzzz')
							{
								sAValue = 'aaaaaaaaaa';
							}
							
							if (sBValue == 'zzzzzzzzzz')
							{
								sBValue = 'aaaaaaaaaa';
							}
						}
					}
					
					if (sAValue == sBValue)
					{
						return (sA.get('ping') > sB.get('ping')) ? 1 : -1;
					}
					
					return (sAValue > sBValue) ? 1 : -1;
				};
				
				link.addClass('cur-sort');
				
				if (!page.sortAscending)
				{
					link.addClass('sort-desc');
				}
				else
				{
					link.removeClass('sort-desc');
				}
				
				$('.left ul.list li').sort(page.sortFunc).appendTo('.left ul.list');

				return false;
			});
			
			$('a[data-sort="ping"]').click();
			
			$('li.row').click(function()
			{
				nui.runUITask('uiscript', 'RefreshServers');
			});
			
			page.scrollbar = $('#serverlist_scrollbar').tinyscrollbar(
			{
				wheel: 400
			});
			
			layout.loaded();
		},
	
		update: function()
		{
			layout.update();
		}
	};
	
	serverBE.onClear = function()
	{
		$('.left ul.list li').remove();
	};
	
	return page;
});
var r = require;

define(['require', 'jquery', 'dust'], function(require, $, dust)
{
	var pages =
	{
		activePages: { },
		loadingPages: { },
	
		init: function()
		{
			pages.loadPageTemplate('iw4_header');
			pages.loadPageTemplate('iw4_footer');
			pages.loadPageTemplate('serverlist');
			
			// load backends
			// also preload connect as somehow it fails when done during level load
			require(['backends/servers', 'backends/arenas', 'pages/connect'], function()
			{
			
			});
		},
		
		setActivePage: function(pageHandler, openCB)
		{
			var renderCB = function(context)
			{
				if (context === undefined)
				{
					context = {};
				}
			
				dust.render(pageHandler.page, context, function(err, out)
				{
					if (err)
					{
						console.error(err);
						return;
					}
					
					delete pages.loadingPages[pageHandler.page];
					pages.activePages[pageHandler.page] = pageHandler;
					
					if (!pageHandler.customRender)
					{
						$('#page-placeholder').append('<div class="page" id="page-' + pageHandler.page + '">' + out + '</div>');
					}
					
					pageHandler.rendered = true;
					
					if (openCB !== undefined)
					{
						openCB(out);
					}
					
					if (pageHandler.loaded !== undefined)
					{
						pageHandler.loaded(out);
					}
					
					pages.enablePageLinks();
				});
			};
			
			if (pageHandler.load !== undefined)
			{
				pageHandler.load(renderCB);
			}
			else
			{
				renderCB();
			}
		},
		
		enablePageLinks: function()
		{
			$('a[rel=page]').click(function()
			{
				var newPage = $(this).attr('href').substring(1);
			
				pages.open(newPage, function()
				{
					for (page in pages.activePages)
					{
						if (page != newPage && pages.activePages[page].isFullscreen)
						{
							pages.close(page);
						}
					}
				});
				
				return false;
			});
		},
		
		loadPageTemplate: function(page, callback)
		{
			if (page in dust.cache)
			{
				callback();
				return;
			}
			
			$.get('/assets/pages/' + page + '.html', '', function(data)
			{
				var compiled = dust.compileFn(data, page);
				dust.loadSource(compiled);
				
				if (callback !== undefined)
				{
					callback();
				}
				
				console.log('loading page ' + page);
			}).fail(function()
			{
				console.error('could not load page ' + page);
				
				if (page in pages.loadingPages)
				{
					delete pages.loadingPages[page];
				}
			});
		},
		
		closeAll: function()
		{
			for (page in pages.activePages)
			{
				pages.close(page);
			}
		},
		
		close: function(page)
		{
			if (page in pages.activePages)
			{
				if (pages.activePages[page].unload !== undefined)
				{
					pages.activePages[page].unload();
				}
				
				$('#page-' + page).remove();
				
				delete pages.activePages[page];
			}
		},
		
		open: function(page, openCB)
		{
			if (page in pages.activePages)
			{
				return;
			}
			
			if (page in pages.loadingPages)
			{
				return;
			}
			
			pages.loadingPages[page] = true;
			
			pages.loadPageTemplate(page, function()
			{
				/*var reqContext = r.config({
					context: page + new Date().getTime(),
					baseUrl: '/assets/js/'
				});*/
			
				require(['pages/' + page], function(pageHandler)
				{
					if (!pageHandler)
					{
						console.error('could not load ' + page + '.js');
						delete pages.loadingPages[page];
						return;
					}
				
					pageHandler.page = page;
					
					pageHandler.close = function()
					{
						pages.close(pageHandler.page);
					};
					
					pages.setActivePage(pageHandler, openCB);
				});
			});		
		},
		
		broadcast: function(type, data)
		{
			// special cases
			if (type == "homeServersUpdated")
			{
				pages.homeServers = data;
			}
			
			// normal stuff		
			for (page in pages.activePages)
			{
				if (pages.activePages[page].onmessage !== undefined)
				{
					pages.activePages[page].onmessage(type, data);
				}
			}
		},
		
		reload: function(page)
		{
			delete dust.cache[page];
		
			pages.close(page);
			pages.open(page);
		},
		
		runFrame: function()
		{
			for (page in pages.activePages)
			{
				if (!pages.activePages[page].rendered)
				{
					continue;
				}
			
				if (pages.activePages[page].update !== undefined)
				{
					pages.activePages[page].update();
				}
			}
		},
		
		uiCallbacks: {},
		
		uiCallbackHandler: function(name)
		{
		    var args = [];
			Array.prototype.push.apply( args, arguments );

			args.shift();
			
			if (name in pages.uiCallbacks)
			{
				pages.uiCallbacks[name].apply(null, args);
			}
		},
		
		registerCallback: function(name, func)
		{
			pages.uiCallbacks[name] = func;
		}
	};
	
	window.registerUICallback(pages.uiCallbackHandler);
	
	var colorMap =
	{
		'1': '#ff4444',
		'2': '#99cc00',
		'3': '#ffbb33',
		'4': '#0099cc',
		'5': '#33b5e5',
		'6': '#aa66cc',
		'8': '#cc0000',
		'9': '#cc0000',
		//'0': '#000000; text-shadow: 1px 1px #aaa, -1px -1px #aaa, 1px -1px #aaa, -1px 1px #aaa;',
		//'0': '#666',
	};
	
	$.extend($, 
	{
		colorize: function(string)
		{
			var newString = '';
			var inSpan = false;
			
			for (i = 0; i < string.length; i++)
			{
				if (string[i] == '^')
				{
					if (string[i + 1] == '7' || string[i + 1] == '0')
					{
						if (inSpan)
						{
							newString += '</span>';
							
							inSpan = false;
						}
						
						i += 2;
					}
					else if (string[i + 1] in colorMap)
					{
						if (inSpan)
						{
							newString += '</span>';
						}
					
						i += 2;
						newString += '<span style="color: ' + colorMap[string[i - 1]] + '">';
						
						inSpan = true;
					}
				}
			
				newString += string[i];
			}
			
			if (inSpan)
			{
				newString += '</span>';
			}
			
			return newString;
		},
		
		stripColors: function(string)
		{
			var newString = '';
			var inSpan = false;
			
			for (i = 0; i < string.length; i++)
			{
				if (string[i] == '^')
				{
					if (string[i + 1] >= '0' && string[i + 1] <= '9')
					{
						i++;
						continue;
					}
				}
				
				newString += string[i];
			}
			
			return newString;
		},
	});
	
	dust.helpers.colorize = function(chunk, context, bodies, params)
	{
		var text = dust.helpers.tap(params.text, chunk, context);
		return chunk.write($.colorize(text));
	};
	
	return pages;
});
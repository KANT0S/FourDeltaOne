define(['require', 'pages', 'backends/arenas'], function(require, pages, arenas)
{
	var serverBE = 
	{
		servers: {}
	};

	pages.registerCallback('addServer', function(ip, info)
	{
		//console.log(ip + ": " + info);
		// parse the infostring
		var str = info.split('\\');
		var data = {};
		
		for (i = 1; i < str.length; i++)
		{
			data[str[i]] = str[i + 1];
			i++;
		}
		
		if (data['fs_game'] !== undefined)
		{
			data['fs_game'] = data['fs_game'].substring(5);
		}
		
		if (data['mapname'])
		{
			data['mapname_real'] = data['mapname'];
			
			if (data['mapname'] in arenas.data.maps)
			{
				data['mapname'] = arenas.data.maps[data.mapname];
			}
		}
		
		if (data['rgametype'])
		{
			// funny, 'rgametype' is 'real gametype', now we've 'gametype_real' too :)
			data['gametype_real'] = data['rgametype'];
			
			if (data['rgametype'] in arenas.data.gametypes)
			{
				data['rgametype'] = arenas.data.gametypes[data.rgametype];
			}
		}
		
		data.cleaned = {};
		
		data.cleaned['hostname'] = $.stripColors(data.hostname.toUpperCase()).replace(/[^A-Z0-9]/g, '');
		data.cleaned['ping'] = parseInt(data.ping);
		data.cleaned['clients'] = parseInt(data.clients);
		
		if (data['fs_game'] === undefined)
		{
			data.cleaned['fs_game'] = 'zzzzzzzzzz'; // hack to mostly show on bottom
		}
		
		data.get = function(keyName)
		{
			if (keyName in data.cleaned)
			{
				return data.cleaned[keyName];
			}
			
			return data[keyName];
		};
		
		serverBE.servers[data.addr] = data;
		
		pages.broadcast('serversUpdated', data);
	});
	
	pages.registerCallback('clearServers', function()
	{
		serverBE.servers = {};
		
		if (serverBE.onClear !== undefined)
		{
			serverBE.onClear();
		}
	});
	
	return serverBE;
});
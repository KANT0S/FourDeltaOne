define(['require', 'pages'], function(require, pages)
{
	var statusBE = 
	{
		cache: {},
		cbs: {},
		
		getStatus: function(addr, cb)
		{
			if (addr in statusBE.cache)
			{
				var time = new Date().getTime();
			
				// only valid for 15 seconds
				if (time <= (statusBE.cache[addr].time + 15000))
				{
					cb(statusBE.cache[addr]);
					return;
				}
			}
			
			statusBE.cbs[addr] = cb;
			nui.getStatus(addr);
		}
	};

	pages.registerCallback('statusResponse', function(data)
	{
		if (data.addr in statusBE.cbs)
		{
			var cb = statusBE.cbs[data.addr];

			data.time = new Date().getTime();
			statusBE.cache[data.addr] = data;
			
			cb(data);
		
			delete statusBE.cbs[data.addr];
		}
	});
	
	return statusBE;
});
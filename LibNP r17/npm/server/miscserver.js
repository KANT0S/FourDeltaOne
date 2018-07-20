var fs = require('fs');
var dgram = require('dgram');

function handleDataRequest(filename, rinfo, server)
{
	if(filename !== undefined)
	{
		var fsFile = "data/pub/" + filename.replace('\\', '');
		fs.exists(fsFile, function(exists)
		{
			if (!exists)
			{
				return;
			}
			else
			{
				fs.readFile(fsFile, function(err, content)
		        {
		            if (err)
		            {
		                return;
		            }
		            
		            server.send(content, 0, content.length, rinfo.port, rinfo.address);
		            return;
		        });   
			}
		});
	}
}
exports.start = function()
{
	var server = dgram.createSocket('udp4');
	server.on('message', function(data, rinfo)
	{
		var message = data.toString();
        var parts = message.trim().split(' ');
        switch(parts[0])
		{
			case "data":
				handleDataRequest(parts[1], rinfo, server);
				break;
		}
	});
	server.bind(3029);
}
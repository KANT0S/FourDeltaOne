var NPServer = require('./server').NPServer;
var fs = require('fs');

exports.start = function()
{
	var interval = 10 * 60 * 1000; // 10 minute interval
	var delay = interval - new Date() % interval;
	setTimeout(function()
	{
		loop();
		setInterval(loop, interval);
	}, delay);
}
function loop()
{
	var players = Object.keys(NPServer.instance.clientsByID).length;
	var data = Math.floor(Date.now()/1000) + "," + players + '\n';
	fs.appendFile('data/pub/playerlog.csv', data, 'ascii');
}
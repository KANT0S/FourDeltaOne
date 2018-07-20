var mysql = require('mysql');
var net = require('net');
var dgram = require('dgram');

var client = mysql.createClient({
	host: '178.33.234.202',
	user: 'fourdeltaone',
	password: 'n:/|h:=~V/%<85,|6L&<_2@t88Bx,$u'
});

//client.query('USE alteriw');

var server = dgram.createSocket('udp4', function (msg, rinfo)
{
	var data = msg.toString();
	var parts = data.trim().split(' ');

	//console.log(data);

	switch (parts[0])
	{
		case "checkSession":
			var token = parts[1];
			var ip = parts[2];

			//console.log("rofl");

			client.query('USE phpbb3_4d1');
			client.query('SELECT * FROM phpbb_sessions s, phpbb_users u WHERE session_id = ? AND s.session_user_id = u.user_id', [token], function(err, results, fields)
			{
				if (err)
				{
					throw err;
				}

				var valid = false;
				var user = 0;
				var extra = ' 0';

				//console.log('results: %d', results.length);

				if (results.length == 1)
				{
					console.log('session_ip: %s - ip: %s', results[0].session_ip, ip);

					if (results[0].session_ip == ip || ip.indexOf('192.168.') == 0)
					{
						valid = true;
						user = results[0].session_user_id;
						extra = ' ' + results[0].group_id;
					}
				}

				var buffer = new Buffer('sessionResult ' + token + ' ' + ((valid) ? '1' : '0') + ' ' + user + extra + "\r\n");
				server.send(buffer, 0, buffer.length, rinfo.port, rinfo.address);
			});
		break;
	}
});

server.bind(3105);

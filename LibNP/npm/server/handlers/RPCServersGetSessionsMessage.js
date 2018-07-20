var server = require('./../server').NPServer;
var int64 = require('node-int64');

module.exports = function(data, id)
{
    var connection = this;
    
    if (!connection.authenticated)
    {
        return;
    }
    
    var sessionsToReply = [];
    
    for (session in server.instance.sessions)
    {
        var sess = server.instance.sessions[session];
        var match = true;
        
        //for (key in data.infos)
        data.infos.forEach(function(i)
        {
            if (i.value[0] == '[')
			{
				var arr = JSON.parse(i.value);
				var subMatch = false;				
				
				arr.forEach(function(j)
				{
					if (sess.keys[i.key] == j)
					{
						subMatch = true;
					}
				});
				
				if (!subMatch)
				{
					match = false;
				}
			}
			else
			{
				if (sess.keys[i.key] != i.value)
				{
					match = false;
				}
			}
        });
        
        if (!match)
        {
            continue;
        }
        
        sess.sid = session;
        sessionsToReply.push(sess);
    }
   
    connection.reply('RPCServersGetSessionsResultMessage', id,
    {
        servers: sessionsToReply
    });
};
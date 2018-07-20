var server = require('./../server').NPServer;
var int64 = require('node-int64');

module.exports = function(data, id)
{
    var connection = this;
    
    if (!connection.authenticated)
    {
        connection.reply('RPCServersCreateSessionResultMessage', id,
        {
            result: 1,
            sessionid: ''
        });
        return;
    }
    
    server.instance.sessionID++;
    var sessionID = new int64(0, server.instance.sessionID).toOctetString();
    
    data.info.npid = connection.npID;
    data.info.keys = {};
    
    data.info.data.forEach(function(a)
    {
        data.info.keys[a.key] = a.value;
    });
	
	data.info.keys['sid'] = sessionID;
	data.info.keys['npid'] = '0' + connection.npID; // the DWLoop code does %016llx, and sessionID is 16 chars too
    
    server.instance.sessions[sessionID] = data.info;
    
    var callback = function()
    {
        if (sessionID in server.instance.sessions)
        {
            delete server.instance.sessions[sessionID];
        }
        
        connection.removeListener('closed', callback);
    };
    
    connection.on('closed', callback);
    
    connection.reply('RPCServersCreateSessionResultMessage', id,
    {
        result: 0,
        sessionid: sessionID,
    });
};
var server = require('./../server').NPServer;
var int64 = require('node-int64');

module.exports = function(data, id)
{
    var connection = this;
    
    if (!connection.authenticated)
    {
        connection.reply('RPCServersDeleteSessionResultMessage', id,
        {
            result: 1
        });
        return;
    }
    
    var sessionID = data.sessionid;
    
    if (!(sessionID in server.instance.sessions) ||
        connection.npID != server.instance.sessions[sessionID].info.npid)
    {
        connection.reply('RPCServersDeleteSessionResultMessage', id,
        {
            result: 1
        });
        return;
    }
    
    if (sessionID in server.instance.sessions)
    {
        delete server.instance.sessions[sessionID];
    }
    
    connection.reply('RPCServersDeleteSessionResultMessage', id,
    {
        result: 0
    });
}
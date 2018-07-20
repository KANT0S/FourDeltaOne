var server = require('./../server').NPServer;
var int64 = require('node-int64');

module.exports = function(data, id)
{
    var connection = this;
    
    if (!connection.authenticated)
    {
        connection.reply('RPCServersUpdateSessionResultMessage', id,
        {
            result: 1,
            sessionid: ''
        });
        return;
    }
    
    var sessionID = data.sessionid;
    
    if (!(sessionID in server.instance.sessions) ||
        connection.npID != server.instance.sessions[sessionID].info.npid)
    {
        connection.reply('RPCServersUpdateSessionResultMessage', id,
        {
            result: 1,
            sessionid: ''
        });
        return;
    }
    
    data.info.npid = connection.npID;
    data.info.keys = {};
    
    data.info.data.forEach(function(a)
    {
        data.info.keys[a.key] = a.value;
    });
    
    server.instance.sessions[sessionID] = data.info;
    
    connection.reply('RPCServersUpdateSessionResultMessage', id,
    {
        result: 0,
        sessionid: sessionID,
    });
};
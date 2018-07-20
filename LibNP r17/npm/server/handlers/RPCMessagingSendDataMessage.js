var server = require('./../server').NPServer;

module.exports = function(data, id)
{
    var connection = this;
    
    if (!connection.authenticated)
    {
        return;
    }
    
    var remoteID = data.npid;
    
    if (remoteID in server.instance.clientsByID)
    {
        var remoteConn = server.instance.clientsByID[remoteID];
        
        remoteConn.reply('RPCMessagingSendDataMessage', 0, {
            npid: connection.npID,
            data: data.data
        });
    }
};